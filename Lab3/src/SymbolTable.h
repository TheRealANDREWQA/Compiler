#pragma once
#include "HashTable.h"
#include <stdbool.h>

typedef enum {
	Identifier,
	IntConstant,
	BoolConstant,
	FloatConstant,
	StringConstant
} SymbolType;

typedef union {
	const char* identifier_string;
	size_t integer_constant;
	bool bool_constant;
	float float_constant;
	const char* string_constant;
} SymbolTableValue;

typedef struct {
	SymbolTableValue value;
	SymbolType type;
} SymbolTableEntry;

typedef struct {
	HashTable storage;
} SymbolTable;

SymbolTable CreateSymbolTable(size_t initial_capacity);

size_t AddSymbolTableEntry(SymbolTable* table, SymbolTableEntry entry);

SymbolTableEntry FindSymbolTableEntry(const SymbolTable* table, size_t entry_index);

size_t GetSymbolTableIndex(const SymbolTable* table, SymbolTableEntry entry);

void RemoveSymbolTableEntry(SymbolTable* table, size_t entry_index);

void DeleteSymbolTable(SymbolTable* table);