#include <stdio.h>
#include "SymbolTable.h"
#include "ProgramInternalForm.h"
#include "Scanning.h"

int main() {
	ProgramInternalForm pif = CreatePIF();
	SymbolTable symbol_table = CreateSymbolTable(0);

	ReadTokenFile(&pif, "token.in");
	string error_string = ScanSourceFile(&pif, &symbol_table, "p1.txt");

	if (error_string.size > 0) {
		printf("Lexical error: %s", error_string.characters);
	}
	else {
		printf("Lexically correct\n");
		bool success = WritePIFToFile(&pif, "PIF.out");
		if (!success) {
			printf("Failed to write PIF.out\n");
		}
		success = WriteSymbolTableToFile(&symbol_table, "ST.out");
		if (!success) {
			printf("Failed to write ST.out\n");
		}
	}

	return 0;
}