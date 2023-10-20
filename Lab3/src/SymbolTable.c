#include "SymbolTable.h"
#include <string.h>

static size_t ENTRY_COUNTER = 0;

static int SymbolTableIdentifierCompare(const void* first, const void* second) {
	return *(size_t*)first == *(size_t*)second;
}

static size_t SymbolTableMapFunction(size_t index, size_t capacity) {
	return index % capacity;
}

static size_t SymbolTableHashFunction(const void* identifier) {
	return *(size_t*)identifier;
}

static size_t SymbolTableGrowFunction(size_t capacity)
{
	return capacity << 1;
}

SymbolTable CreateSymbolTable(size_t initial_capacity) {
	SymbolTable result;
	result.storage = CreateTable(
		initial_capacity,
		sizeof(SymbolTableEntry),
		sizeof(size_t),
		SymbolTableMapFunction,
		SymbolTableHashFunction,
		SymbolTableIdentifierCompare
	);
	return result;
}

size_t AddSymbolTableEntry(SymbolTable* table, SymbolTableEntry entry) {
	size_t entry_index = ENTRY_COUNTER;

	if (entry.type == Identifier) {
		entry.value.identifier_string = _strdup(entry.value.identifier_string);
	}
	else if (entry.type == StringConstant) {
		entry.value.string_constant = _strdup(entry.value.string_constant);
	}

	int needs_resize = AddTable(&table->storage, &entry, &entry_index);
	ENTRY_COUNTER++;

	if (needs_resize) {
		GrowTable(&table->storage, SymbolTableGrowFunction);
	}

	return entry_index;
}

SymbolTableEntry FindSymbolTableEntry(const SymbolTable* table, size_t entry_index) {
	return *(const SymbolTableEntry*)FindTablePtr(&table->storage, &entry_index);
}

typedef struct {
	size_t entry_index;
	SymbolTableEntry entry;
} GetSymbolTableIterateFunctionData;

static int GetSymbolTableIterateFunction(void* element, void* identifier, void* extra_data) {
	GetSymbolTableIterateFunctionData* data = (GetSymbolTableIterateFunctionData*)extra_data;
	SymbolTableEntry current_entry = *(SymbolTableEntry*)element;
	size_t current_entry_index = *(size_t*)identifier;

	if (data->entry.type == current_entry.type) {
		switch (data->entry.type) {
		case Identifier:
		{
			if (strcmp(data->entry.value.identifier_string, current_entry.value.identifier_string) == 0) {
				data->entry_index = current_entry_index;
				return 1;
			}
		}
			break;
		case IntConstant:
		{
			if (data->entry.value.integer_constant == current_entry.value.integer_constant) {
				data->entry_index = current_entry_index;
				return 1;
			}
		}
			break;
		case BoolConstant:
		{
			if (data->entry.value.bool_constant == current_entry.value.bool_constant) {
				data->entry_index = current_entry_index;
				return 1;
			}
		}
			break;
		case FloatConstant:
		{
			if (data->entry.value.float_constant == current_entry.value.float_constant) {
				data->entry_index = current_entry_index;
				return 1;
			}
		}
			break;
		case StringConstant:
		{
			if (strcmp(data->entry.value.string_constant, current_entry.value.string_constant) == 0) {
				data->entry_index = current_entry_index;
				return 1;
			}
		}
			break;
		}
	}

	return 0;
}

size_t GetSymbolTableIndex(const SymbolTable* table, SymbolTableEntry entry)
{
	GetSymbolTableIterateFunctionData iterate_data;
	iterate_data.entry_index = -1;
	iterate_data.entry = entry;
	
	IterateTable(&table->storage, GetSymbolTableIterateFunction, &iterate_data);

	return iterate_data.entry_index;
}

void RemoveSymbolTableEntry(SymbolTable* table, size_t entry_index) {
	RemoveTable(&table->storage, &entry_index);
}

void DeleteSymbolTable(SymbolTable* table) {
	DestroyTable(&table->storage);
	memset(table, 0, sizeof(*table));
}