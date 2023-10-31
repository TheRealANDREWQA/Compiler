#pragma once
#include "HashTable.h"
#include "StringUtilities.h"

typedef struct {
	HashTable storage;
} SymbolTable;

SymbolTable CreateSymbolTable(size_t initial_capacity);

// If the token already exists, returns its index, else it adds it and generates a new entry index
size_t AddOrGetSymbolTableEntry(SymbolTable* table, string token);

// Retrieve the entry index for that token
size_t GetSymbolTableEntry(const SymbolTable* table, string token);

void RemoveSymbolTableEntry(SymbolTable* table, string token);

void DeleteSymbolTable(SymbolTable* table);

bool WriteSymbolTableToFile(const SymbolTable* table, const char* path);