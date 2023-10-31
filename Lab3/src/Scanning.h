#pragma once
#include "ProgramInternalForm.h"
#include "SymbolTable.h"

// Returns an error string if an error has occured, else an empty string
string ScanSourceFile(ProgramInternalForm* pif, SymbolTable* symbol_table, const char* source_file);