#include <stdio.h>
#include "SymbolTable.h"
#include "ProgramInternalForm.h"
#include "Scanning.h"
#include "FiniteAutomata.h"

int main() {
	ProgramInternalForm pif = CreatePIF();
	SymbolTable symbol_table = CreateSymbolTable(0);

	ReadTokenFile(&pif, "token.in");
	string error_string = ScanSourceFile(&pif, &symbol_table, "p2.txt");

	if (error_string.size > 0) {
		printf("Lexical error: %s", error_string.characters);
	}
	else {
		printf("Lexically correct\n");
		bool success = WritePIFToFile(&pif, "PIF2.out");
		if (!success) {
			printf("Failed to write PIF.out\n");
		}
		success = WriteSymbolTableToFile(&symbol_table, "ST2.out");
		if (!success) {
			printf("Failed to write ST.out\n");
		}
	}

	/*FiniteAutomata finite_automata = CreateFiniteAutomata();
	bool success = ReadFiniteAutomataFile("FA.in", &finite_automata);
	if (!success) {
		printf("Failed reading FA.in\n");
	}
	else {
		printf("The file was successfully read\n");
		FiniteAutomataConsole(&finite_automata);
	}
	DestroyFiniteAutomata(&finite_automata);*/

	return 0;
}