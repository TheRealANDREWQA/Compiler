#pragma once
#include "ResizableStream.h"
#include <stdbool.h>
#include "StringUtilities.h"
#include "FiniteAutomata.h"

typedef enum {
	TOKEN_IDENTIFIER,
	TOKEN_INT_CONSTANT,
	TOKEN_FLOAT_CONSTANT,
	TOKEN_BOOL_CONSTANT,
	TOKEN_STRING_CONSTANT,
	TOKEN_RESERVED,
	TOKEN_OPERATOR,
	TOKEN_SEPARATOR
} TOKEN_CLASS;

typedef struct {
	TOKEN_CLASS token_class;
	// The entry index for tokens of class reserved word, operator or separator
	// Is the index inside the array
	size_t entry_index;
} Token;

typedef struct {
	// Element type is string
	ResizableStream reserved_words;
	// Element type is string
	ResizableStream operators;
	// Element type is string
	ResizableStream separators;
	// Element type is Token
	ResizableStream token_order;

	FiniteAutomata identifier_fa;
	FiniteAutomata integer_constant_fa;
} ProgramInternalForm;

ProgramInternalForm CreatePIF();

bool ReadTokenFile(ProgramInternalForm* pif, const char* path);

// Returns -1 if it doesn't find it
size_t FindReservedWord(const ProgramInternalForm* pif, string token);

// Returns -1 if it doesn't find it
size_t FindOperator(const ProgramInternalForm* pif, string token);

// Returns -1 if it doesn't find it
size_t FindSeparator(const ProgramInternalForm* pif, string token);

void DestroyPIF(ProgramInternalForm* pif);

bool WritePIFToFile(const ProgramInternalForm* pif, const char* path);