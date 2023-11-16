#pragma once
#include <stdint.h>

/*
	This function is used to determine when 2 identifiers are equal. The identifiers are given as parameters
*/
typedef int (*HashTableIdentifierCompare)(const void* first, const void* second);

/*
	This function is used to map the hash value into an index for the hash table (usually either power of two trick
	or % capacity)
*/
typedef size_t (*HashTableMapFunction)(size_t index, size_t capacity);

/*
	This function converts an identifier into an integer value
*/
typedef size_t (*HashTableHashFunction)(const void* identifier);

/*
	Determines what is the next capacity that the table should grow to when it is necessary.
	Takes the current capacity and returns the new value
*/
typedef size_t (*HashTableGrowFunction)(size_t capacity);

/*
	This function is used to iterate through the hash table values. Receives as parameters the value and the identifier
	and the extra data that the function might need. It should return 1 if the iteration should stop, else 0 to continue
*/
typedef int (*HashTableIterate)(void* element, void* identifier, void* extra_data);

/*
	Default hash table compare function for integers (unsigned int)
*/
int HashTableCompareIntegers(const void* first, const void* second);

/*
	Default hash table compare function for integers (size_t)
*/
int HashTableCompareSizeT(const void* first, const void* second);

/*
	Default hash table map power of two function
*/
size_t HashTableMapPowerOfTwo(size_t index, size_t capacity);

/*
	Default fallthrough hash function for integers (unsigned ints). It returns the same value.
*/
size_t HashTableHashInteger(const void* identifier);

/*
	Default fallthrough hash function for size_t. It returns the same value.
*/
size_t HashTableHashSizeT(const void* identifier);

/*
	Default hash table grow function which returns the current capacity (assumed to be a power of two) multiplied by 2
*/
size_t HashTableGrowPowerOfTwo(size_t capacity);

/*
	The maximum load factor of the table after which a grow is required
*/
#define HASH_TABLE_MAX_LOAD_FACTOR 90

typedef struct {
	void* buffer;
	void* identifiers;
	unsigned char* metadata;
	size_t max_search_length;
	size_t capacity;
	size_t size;
	size_t element_size;
	size_t identifier_size;

	HashTableIdentifierCompare identifier_compare;
	HashTableMapFunction map_function;
	HashTableHashFunction hash_function;
} HashTable;

/*
	Initializes a hash table. Capacity is the initial capacity, element_size the byte size of the elements,
	identifier_size the byte size of the identifiers (keys), and the hash table functions
*/
HashTable CreateTable(
	size_t capacity,
	size_t element_size,
	size_t identifier_size,
	HashTableMapFunction map_function,
	HashTableHashFunction hash_function,
	HashTableIdentifierCompare compare_function
);

/*
	Adds the element in the table by hashing it. 
	Returns 1 if the table needs a resize, else 0
*/
int AddTable(HashTable* table, const void* element, const void* identifier);

/*
	Searches for an identifier and returns its index inside the table if found, 
	else -1 if the identifier cannot be located
*/
size_t FindTable(const HashTable* table, const void* identifier);


/*
	The same as the other find, except that it returns a pointer to the element if found, else NULL
*/
void* FindTablePtr(const HashTable* table, const void* identifier);

/*
	Returns a pointer to an element given by its index. Does not do bounds checking.
*/
void* GetTablePtr(const HashTable* table, size_t index);

/*
	Returns a pointer to an identifier given by its index. Does not do bounds checking.
*/
void* GetTableIdentifierPtr(const HashTable* table, size_t index);

/*
	Returns 1 if an element exists at that index location, else 0. Does not do bounds checking.
*/
int IsTableElementAt(const HashTable* table, size_t index);

/*
	Removes an entry from the table. Returns 1 if the identifier could be located, else 0.
*/
int RemoveTable(HashTable* table, const void* identifier);

/*
	Iterate through the collection of values inside the table. A function alongside its external data needs 
	to be provided.
*/
void IterateTable(const HashTable* table, HashTableIterate iterate_function, void* extra_data);

/*
	Removes an entry from the table given by the index. It does not do bounds checking for the index.
*/
void RemoveTableIndex(HashTable* table, size_t index);

/*
	Resets the internal counter of the table in order to improve the search speed. (It is mostly needed after deleting many elements)
*/
void ResetTableLengthCounter(HashTable* table);

/*
	Clears the memory used by the table
*/
void DestroyTable(const HashTable* table);

/*
	Copies the table into a new one with the exact same capacity.
*/
HashTable CopyTable(const HashTable* table);

/*
	Returns how much bytes are needed for a table with the given element byte size, identifier byte size
	and a certain capacity of the table.
*/
size_t MemoryOfTable(size_t element_size, size_t identifier_size, size_t capacity);

/*
	Grows the table to the next available capacity.
*/
void GrowTable(HashTable* table, HashTableGrowFunction grow_function);

/*
	Grows the table to a certain capacity value. The capacity must respect any capacity conditions. (example
	for power of two size it must be a power of two)
*/
void GrowTableToCapacity(HashTable* table, size_t capacity);

