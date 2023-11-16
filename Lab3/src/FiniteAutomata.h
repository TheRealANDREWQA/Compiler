#pragma once
#include "ResizableStream.h"
#include "HashTable.h"
#include <stdbool.h>
#include "StringUtilities.h"

typedef struct {
	size_t target_state_index;
	char terminal;
} FATransition;

typedef struct {
	// Element type is FATransition
	ResizableStream stream;
} FATableEntry;

typedef struct {
	size_t initial_state;
	// Element is a string
	ResizableStream states;
	// Element is a character
	ResizableStream alphabet;
	// Element is a size_t, index into states
	ResizableStream final_states;
	// Element is FATableEntry, Identifier is size_t, the state index
	HashTable transitions;
} FiniteAutomata;

FiniteAutomata CreateFiniteAutomata();

size_t FindFAStateIndex(const FiniteAutomata* finite_automata, string state_string);

bool ReadFiniteAutomataFile(const char* path, FiniteAutomata* finite_automata);

bool FiniteAutomataVerifySequence(const FiniteAutomata* finite_automata, string sequence);

void FiniteAutomataConsole(const FiniteAutomata* finite_automata);

void DestroyFiniteAutomata(FiniteAutomata* finite_automata);