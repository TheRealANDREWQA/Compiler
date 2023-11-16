#include "ProgramInternalForm.h"
#include "StringUtilities.h"
#include "ParsingRules.h"
#include <stdio.h>
#include <string.h>

ProgramInternalForm CreatePIF() {
	ProgramInternalForm pif;

	pif.reserved_words = CreateStream(0, sizeof(string));
	pif.operators = CreateStream(0, sizeof(string));
	pif.separators = CreateStream(0, sizeof(string));
	pif.token_order = CreateStream(0, sizeof(Token));

	return pif;
}

bool ReadTokenFile(ProgramInternalForm* pif, const char* path) {
	string token_file = ReadFile(path);
	if (token_file.size == 0) {
		return false;
	}

	string first_line = StringUntil(token_file, '\n');
	string first_line_parse_region = FindCharacter(first_line, ':');
	first_line_parse_region = StringAdvance(first_line_parse_region, 1);
	ParseTokensFromWhitespace(first_line_parse_region, &pif->operators);

	string token_file_after_first_line = StringAfter(token_file, first_line);
	string second_line = StringUntil(token_file_after_first_line, '\n');
	string second_line_parse_region = FindCharacter(second_line, ':');
	second_line_parse_region = StringAdvance(second_line_parse_region, 1);
	ParseTokensFromWhitespace(second_line_parse_region, &pif->separators);

	string third_line = StringUntil(StringAfter(token_file_after_first_line, second_line), '\n');
	string third_line_parse_region = FindCharacter(third_line, ':');
	third_line_parse_region = StringAdvance(third_line_parse_region, 1);
	ParseTokensFromWhitespace(third_line_parse_region, &pif->reserved_words);
	free(token_file.characters);
	return true;
}

size_t FindReservedWord(const ProgramInternalForm* pif, string token) {
	return FindStringInStream(pif->reserved_words, token);
}

size_t FindOperator(const ProgramInternalForm* pif, string token) {
	return FindStringInStream(pif->operators, token);
}

size_t FindSeparator(const ProgramInternalForm* pif, string token) {
	return FindStringInStream(pif->separators, token);
}

void DestroyPIF(ProgramInternalForm* pif) {
	DeallocateStrings(pif->reserved_words);
	DeallocateStrings(pif->operators);
	DeallocateStrings(pif->separators);

	FreeStream(pif->reserved_words);
	FreeStream(pif->operators);
	FreeStream(pif->separators);
	FreeStream(pif->token_order);

	memset(pif, 0, sizeof(*pif));
}

bool WritePIFToFile(const ProgramInternalForm* pif, const char* path)
{
	FILE* file = fopen(path, "wt");
	if (file) {
		for (size_t index = 0; index < pif->token_order.size; index++) {
			const Token* current_token = GetElement(pif->token_order, index);
			char token_string[256];
			memset(token_string, ' ', 20);
			if (current_token->token_class == TOKEN_RESERVED) {
				const string* word = GetElement(pif->reserved_words, current_token->entry_index);
				sprintf(token_string, "%s | %zu\n", word->characters, current_token->entry_index);
			}
			else if (current_token->token_class == TOKEN_OPERATOR) {
				const string* operator_ = GetElement(pif->operators, current_token->entry_index);
				sprintf(token_string, "%s | %zu\n", operator_->characters, current_token->entry_index);
			}
			else if (current_token->token_class == TOKEN_SEPARATOR) {
				const string* separator = GetElement(pif->separators, current_token->entry_index);
				sprintf(token_string, "%s | %zu\n", separator->characters, current_token->entry_index);
			}
			else if (current_token->token_class == TOKEN_INT_CONSTANT) {
				sprintf(token_string, "int constant | %zu\n", current_token->entry_index);
			}
			else if (current_token->token_class == TOKEN_BOOL_CONSTANT) {
				sprintf(token_string, "bool constant | %zu\n", current_token->entry_index);
			}
			else if (current_token->token_class == TOKEN_FLOAT_CONSTANT) {
				sprintf(token_string, "float constant | %zu\n", current_token->entry_index);
			}
			else if (current_token->token_class == TOKEN_STRING_CONSTANT) {
				sprintf(token_string, "string constant | %zu\n", current_token->entry_index);
			}
			else if (current_token->token_class == TOKEN_IDENTIFIER) {
				sprintf(token_string, "identifier | %zu\n", current_token->entry_index);
			}

			fprintf(file, token_string);
		}

		fclose(file);
		return true;
	}
	return false;
}
