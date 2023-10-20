#include <stdio.h>
#include "SymbolTable.h"
#include <string.h>

int main() {
	SymbolTable symbol_table = CreateSymbolTable(16);

	SymbolTableEntry entry;
	entry.type = Identifier;
	entry.value.identifier_string = "a";
	size_t identifier_index = AddSymbolTableEntry(&symbol_table, entry);

	entry.type = IntConstant;
	entry.value.integer_constant = 10;
	size_t integer_index = AddSymbolTableEntry(&symbol_table, entry);

	entry.type = BoolConstant;
	entry.value.bool_constant = false;
	size_t bool_index = AddSymbolTableEntry(&symbol_table, entry);

	entry.type = FloatConstant;
	entry.value.float_constant = 25.0f;
	size_t float_index = AddSymbolTableEntry(&symbol_table, entry);

	entry.type = StringConstant;
	entry.value.string_constant = "Gigachad";
	size_t string_index = AddSymbolTableEntry(&symbol_table, entry);

	SymbolTableEntry identifier_entry = FindSymbolTableEntry(&symbol_table, identifier_index);
	if (identifier_entry.type != Identifier || strcmp(identifier_entry.value.identifier_string, "a") != 0) {
		printf("Identifier entry is incorrect");
		return 1;
	}

	SymbolTableEntry integer_entry = FindSymbolTableEntry(&symbol_table, integer_index);
	if (integer_entry.type != IntConstant || integer_entry.value.integer_constant != 10) {
		printf("Int constant entry is incorrect");
		return 1;
	}

	SymbolTableEntry bool_entry = FindSymbolTableEntry(&symbol_table, bool_index);
	if (bool_entry.type != BoolConstant || bool_entry.value.bool_constant != false) {
		printf("Bool constant entry is incorrect");
		return 1;
	}

	SymbolTableEntry float_entry = FindSymbolTableEntry(&symbol_table, float_index);
	if (float_entry.type != FloatConstant || float_entry.value.float_constant != 25.0f) {
		printf("Float constant entry is incorrect");
		return 1;
	}

	SymbolTableEntry string_entry = FindSymbolTableEntry(&symbol_table, string_index);
	if (string_entry.type != StringConstant || strcmp(string_entry.value.string_constant, "Gigachad") != 0) {
		printf("Bool constant entry is incorrect");
		return 1;
	}

	size_t get_identifier_index = GetSymbolTableIndex(&symbol_table, identifier_entry);
	if (get_identifier_index != identifier_index) {
		printf("Get identifier index is incorrect");
		return 1;
	}

	size_t get_int_index = GetSymbolTableIndex(&symbol_table, integer_entry);
	if (get_int_index != integer_index) {
		printf("Get integer index is incorrect");
		return 1;
	}

	size_t get_bool_index = GetSymbolTableIndex(&symbol_table, bool_entry);
	if (get_bool_index != bool_index) {
		printf("Get bool index is incorrect");
		return 1;
	}

	size_t get_float_index = GetSymbolTableIndex(&symbol_table, float_entry);
	if (get_float_index != float_index) {
		printf("Get float index is incorrect");
		return 1;
	}

	size_t get_string_index = GetSymbolTableIndex(&symbol_table, string_entry);
	if (get_string_index != string_index) {
		printf("Get string index is incorrect");
		return 1;
	}

	DeleteSymbolTable(&symbol_table);
	return 0;
}