#include "SymbolTable.h"
#include <string.h>
#include <malloc.h>
#include <stdio.h>

static size_t ENTRY_COUNTER = 0;

static int SymbolTableIdentifierCompare(const void* first, const void* second) {
	return StringEqual(*(string*)first, *(string*)second);
}

static size_t SymbolTableMapFunction(size_t index, size_t capacity) {
	return index % capacity;
}

static size_t SymbolTableHashFunction(const void* identifier) {
	const string* string_identifier = (const string*)identifier;
	size_t hash = 0;
	for (size_t index = 0; index < string_identifier->size; index++) {
		hash += string_identifier->characters[index] * index;
	}
	return hash;
}

static size_t SymbolTableGrowFunction(size_t capacity)
{
	return capacity == 0 ? 16 : capacity << 1;
}

SymbolTable CreateSymbolTable(size_t initial_capacity) {
	SymbolTable result;
	result.storage = CreateTable(
		initial_capacity,
		sizeof(size_t),
		sizeof(string),
		SymbolTableMapFunction,
		SymbolTableHashFunction,
		SymbolTableIdentifierCompare
	);
	return result;
}

size_t AddOrGetSymbolTableEntry(SymbolTable* table, string token) {
	size_t existing_index = GetSymbolTableEntry(table, token);
	if (existing_index != -1) {
		return existing_index;
	}

	size_t entry_index = ENTRY_COUNTER;

	if (table->storage.capacity == table->storage.size) {
		GrowTable(&table->storage, SymbolTableGrowFunction);
	}

	char* token_allocation = (char*)malloc((token.size + 1) * sizeof(char));
	memcpy(token_allocation, token.characters, sizeof(char) * token.size);
	token_allocation[token.size] = '\0';
	token.characters = token_allocation;

	AddTable(&table->storage, &entry_index , &token);
	ENTRY_COUNTER++;

	return entry_index;
}

size_t GetSymbolTableEntry(const SymbolTable* table, string token) {
	const size_t* entry_ptr = FindTablePtr(&table->storage, &token);
	return entry_ptr == NULL ? -1 : *entry_ptr;
}

void RemoveSymbolTableEntry(SymbolTable* table, string token) {
	RemoveTable(&table->storage, &token);
}

void DeleteSymbolTable(SymbolTable* table) {
	DestroyTable(&table->storage);
	memset(table, 0, sizeof(*table));
}

int WriteToFileIterate(void* element, void* identifier, void* extra_data) {
	FILE* file = (FILE*)extra_data;
	const string* key = (const string*)identifier;
	const size_t* entry_index = (const size_t*)element;

	fprintf(file, "%s | %zu\n", key->characters, *entry_index);
	return 0;
}

bool WriteSymbolTableToFile(const SymbolTable* table, const char* path) {
	FILE* file = fopen(path, "wt");
	if (file) {
		IterateTable(&table->storage, WriteToFileIterate, file);
		fclose(file);
		return true;
	}
	return false;
}