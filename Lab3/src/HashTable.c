#include "HashTable.h"
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define HASH_TABLE_DISTANCE_MASK 0xF8
#define HASH_TABLE_MAX_DISTANCE 32

void* OffsetPointer(const void* pointer, size_t offset) {
	uintptr_t ptr = (uintptr_t)pointer;
	return (void*)(ptr + offset);
}

unsigned char GetDistance(unsigned char metadata) {
	return metadata;
}

unsigned char GetDistanceIndex(const HashTable* table, size_t index) {
	return GetDistance(table->metadata[index]);
}

size_t GetExtendedCapacity(size_t capacity) {
	return capacity + HASH_TABLE_MAX_DISTANCE;
}

int HashTableCompareIntegers(const void* first, const void* second)
{
	const unsigned int* first_int = first;
	const unsigned int* second_int = second;
	return *first_int == *second_int;
}

int HashTableCompareSizeT(const void* first, const void* second)
{
	const size_t* first_sizet = first;
	const size_t* second_sizet = second;
	return *first_sizet == *second_sizet;
}

size_t HashTableMapPowerOfTwo(size_t index, size_t capacity)
{
	return index & (capacity - 1);
}

size_t HashTableHashInteger(const void* identifier)
{
	return *(unsigned int*)identifier;
}

size_t HashTableHashSizeT(const void* identifier)
{
	return *(size_t*)identifier;
}

size_t HashTableGrowPowerOfTwo(size_t capacity)
{
	return capacity << 1;
}

HashTable CreateTable(size_t capacity, size_t element_size, size_t identifier_size, HashTableMapFunction map_function, HashTableHashFunction hash_function, HashTableIdentifierCompare compare_function)
{
	HashTable table;

	if (capacity > 0) {
		size_t table_byte_size = MemoryOfTable(element_size, identifier_size, capacity);
		
		// Partition the buffer appropriately
		size_t extended_capacity = GetExtendedCapacity(capacity);
		table.buffer = malloc(table_byte_size);
		table.identifiers = OffsetPointer(table.buffer, element_size * extended_capacity);
		table.metadata = OffsetPointer(table.identifiers, identifier_size * extended_capacity);
		memset(table.metadata, 0, sizeof(unsigned char) * extended_capacity);
	}
	else {
		table.buffer = NULL;
		table.identifiers = NULL;
		table.metadata = NULL;
	}

	table.capacity = capacity;
	table.size = 0;
	table.element_size = element_size;
	table.identifier_size = identifier_size;
	table.map_function = map_function;
	table.hash_function = hash_function;
	table.identifier_compare = compare_function;
	table.max_search_length = 0;

	return table;
}

int AddTable(HashTable* table, const void* element, const void* identifier)
{
	size_t key = table->hash_function(identifier);

	// calculating the index at which the key wants to be
	size_t index = table->map_function(key, table->capacity);
	size_t hash = index;
	unsigned char distance = 1;

	void* temp_value = _alloca(table->element_size);
	void* temp_identifier = _alloca(table->identifier_size);

	void* current_element = _alloca(table->element_size);
	void* current_identifier = _alloca(table->identifier_size);

	memcpy(current_element, element, table->element_size);
	memcpy(current_identifier, identifier, table->identifier_size);

	// probing the next slots
	while (1) {
		// getting the element's distance
		unsigned char element_distance = GetDistanceIndex(table, index);

		// if the element is "poorer" than us, we keep probing 
		while (element_distance > distance) {
			distance++;
			index++;
			element_distance = GetDistanceIndex(table, index);
		}

		// if the slot is empty, the key can be placed here
		if (element_distance == 0) {
			table->metadata[index] = distance;
			void* table_element = GetTablePtr(table, index);
			memcpy(table_element, current_element, table->element_size);
			void* table_identifier = GetTableIdentifierPtr(table, index);
			memcpy(table_identifier, current_identifier, table->identifier_size);
			table->size++;

			table->max_search_length = max(distance, table->max_search_length);
			break;
		}

		// if it finds a richer slot than us, we swap the keys and keep searching to find 
		// an empty slot for the richer slot
		else {
			unsigned char metadata_temp = table->metadata[index];	

			void* table_value = GetTablePtr(table, index);
			void* table_identifier = GetTableIdentifierPtr(table, index);
			memcpy(temp_value, table_value, table->element_size);
			memcpy(temp_identifier, table_identifier, table->identifier_size);

			memcpy(table_value, current_element, table->element_size);
			memcpy(table_identifier, current_identifier, table->identifier_size);
			table->metadata[index] = distance;

			distance = metadata_temp + 1;
			memcpy(current_element, temp_value, table->element_size);
			memcpy(current_identifier, temp_identifier, table->identifier_size);
			table->max_search_length = max(table->max_search_length, distance);
			index++;
		}
	}
	if ((table->size * 100 / table->capacity > HASH_TABLE_MAX_LOAD_FACTOR) || distance == HASH_TABLE_MAX_DISTANCE)
		return 1;
	return 0;
}

size_t FindTable(const HashTable* table, const void* identifier)
{
	if (table->capacity == 0) {
		return -1;
	}

	size_t key = table->hash_function(identifier);

	// calculating the index for the array with the hash function
	size_t index = table->map_function(key, table->capacity);

	for (size_t distance = 1; distance <= table->max_search_length; distance++) {
		unsigned char current_distance = GetDistanceIndex(table, index);
		if (current_distance == distance) {
			// Verify the identifier now
			if (table->identifier_compare(identifier, OffsetPointer(table->identifiers, index * table->identifier_size))) {
				return index;
			}
		}
		index++;
	}

	return -1;
}

void* FindTablePtr(const HashTable* table, const void* identifier)
{
	size_t index = FindTable(table, identifier);
	return index == -1 ? NULL : OffsetPointer(table->buffer, index * table->element_size);
}

void* GetTablePtr(const HashTable* table, size_t index)
{
	return OffsetPointer(table->buffer, index * table->element_size);
}

void* GetTableIdentifierPtr(const HashTable* table, size_t index) {
	return OffsetPointer(table->identifiers, index * table->identifier_size);
}

int IsTableElementAt(const HashTable* table, size_t index)
{
	return table->metadata[index] != 0;
}

int RemoveTable(HashTable* table, const void* identifier)
{
	size_t index = FindTable(table, identifier);
	if (index != -1) {
		RemoveTableIndex(table, index);
		return 1;
	}
	return 0;
}

void IterateTable(const HashTable* table, HashTableIterate iterate_function, void* extra_data)
{
	size_t extended_capacity = GetExtendedCapacity(table->capacity);
	for (size_t index = 0; index < extended_capacity; index++) {
		if (IsTableElementAt(table, index)) {
			if (iterate_function(GetTablePtr(table, index), GetTableIdentifierPtr(table, index), extra_data)) {
				break;
			}
		}
	}
}

void RemoveTableIndex(HashTable* table, size_t index)
{
	// getting the distance of the next slot;
	index++;
	unsigned char distance = GetDistanceIndex(table, index);

	// while we find a slot that is not empty and that is not in its ideal hash position
	// backward shift the elements
	while (distance > 1) {
		distance--;
		table->metadata[index - 1] = distance;
		void* destination_value = GetTablePtr(table, index - 1);
		void* source_value = GetTablePtr(table, index);

		void* destination_identifier = GetTablePtr(table, index - 1);
		void* source_identifier = GetTablePtr(table, index);
		memcpy(destination_value, source_value, table->element_size);
		memcpy(destination_identifier, source_identifier, table->identifier_size);
		index++;
		distance = GetDistanceIndex(table, index);
	}
	table->metadata[index - 1] = 0;
	table->size--;
}

void ResetTableLengthCounter(HashTable* table) {
	table->max_search_length = 0;
	for (size_t index = 0; index < table->capacity; index++) {
		table->max_search_length = max(table->max_search_length, table->metadata[index]);
	}
}

void DestroyTable(const HashTable* table)
{
	if (table->buffer != NULL) {
		free(table->buffer);
	}
}

HashTable CopyTable(const HashTable* table)
{
	HashTable new_table = CreateTable(table->capacity, table->element_size, table->identifier_size, table->map_function, table->hash_function, table->identifier_compare);
	memcpy(new_table.buffer, table->buffer, MemoryOfTable(table->element_size, table->identifier_size, table->capacity));
	new_table.size = table->size;
	new_table.max_search_length = table->max_search_length;
	return new_table;
}

size_t MemoryOfTable(size_t element_size, size_t identifier_size, size_t capacity)
{
	size_t per_element_size = element_size + identifier_size + sizeof(unsigned char);
	return GetExtendedCapacity(capacity) * per_element_size;
}

void GrowTable(HashTable* table, HashTableGrowFunction grow_capacity)
{
	size_t new_capacity = grow_capacity(table->capacity);
	GrowTableToCapacity(table, new_capacity);
}

void GrowTableToCapacity(HashTable* table, size_t capacity)
{
	// Allocate a new set of buffers
	size_t table_size = MemoryOfTable(table->element_size, table->identifier_size, capacity);
	void* new_buffer = malloc(table_size);

	// Partition the buffer
	size_t extended_capacity = GetExtendedCapacity(capacity);

	void* new_elements = new_buffer;
	void* new_identifiers = OffsetPointer(new_buffer, table->element_size * extended_capacity);
	void* new_metadata = OffsetPointer(new_identifiers, table->identifier_size * extended_capacity);
	memset(new_metadata, 0, sizeof(unsigned char) * extended_capacity);

	void* old_elements = table->buffer;
	void* old_identifiers = table->identifiers;
	unsigned char* old_metadata = table->metadata;

	size_t old_extended_capacity = GetExtendedCapacity(table->capacity);

	// Set the new values
	table->capacity = capacity;
	table->buffer = new_elements;
	table->identifiers = new_identifiers;
	table->metadata = new_metadata;
	table->max_search_length = 0;
	table->size = 0;

	if (old_metadata != NULL) {
		// Now for each element, rehash it and insert it into the table
		for (size_t index = 0; index < old_extended_capacity; index++) {
			if (old_metadata[index] != 0) {
				AddTable(table, OffsetPointer(old_elements, index * table->element_size), OffsetPointer(old_identifiers, index * table->identifier_size));
			}
		}
	}
}
