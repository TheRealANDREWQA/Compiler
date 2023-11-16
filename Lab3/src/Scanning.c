#include "Scanning.h"
#include "ParsingRules.h"
#include <stdio.h>

string ScanSourceFile(ProgramInternalForm* pif, SymbolTable* symbol_table, const char* source_file)
{
	string source_file_content = ReadFile(source_file);
	if (source_file_content.size == 0) {
		return StringFromLiteral("Could not open source file");
	}

	ResizableStream lines = CreateStream(16, sizeof(string));
	ParseTokensFromLines(source_file_content, &lines);

	for (size_t index = 0; index < lines.size; index++) {
		ResizableStream line_tokens = CreateStream(16, sizeof(string));
		const string* current_line = GetElement(lines, index);
		ParseTokensWithSeparators(*current_line, pif->separators, pif->operators, &line_tokens, true);

		for (size_t subindex = 0; subindex < line_tokens.size; subindex++) {
			const string* current_token = GetElement(line_tokens, subindex);

			Token token;
			token.entry_index = -1;
			size_t reserved_word_index = FindReservedWord(pif, *current_token);
			if (reserved_word_index != -1) {
				token.token_class = TOKEN_RESERVED;
				token.entry_index = reserved_word_index;
			}

			if (token.entry_index == -1) {
				size_t operator_index = FindOperator(pif, *current_token);
				if (operator_index != -1) {
					token.token_class = TOKEN_OPERATOR;
					token.entry_index = operator_index;
				}
			}

			if (token.entry_index == -1) {
				size_t separator_index = FindSeparator(pif, *current_token);
				if (separator_index != -1) {
					token.token_class = TOKEN_SEPARATOR;
					token.entry_index = separator_index;
				}
			}

			if (token.entry_index == -1) {
				// Check if it is a string constant first since it has special \" characters
				bool is_string_constant = IsStringConstant(*current_token);
				if (is_string_constant) {
					token.token_class = TOKEN_STRING_CONSTANT;
					token.entry_index = AddOrGetSymbolTableEntry(symbol_table, *current_token);
				}
				else {
					if (!IsTokenValid(*current_token)) {
						char null_terminated_token[256];
						memcpy(null_terminated_token, current_token->characters, sizeof(char) * current_token->size);
						null_terminated_token[current_token->size] = '\0';
						
						char temp_memory[256];
						temp_memory[0] = '\0';
						sprintf(temp_memory, "Invalid token %s on line %zu", null_terminated_token, index + 1);

						free(source_file_content.characters);
						return StringMallocCopyFromPointer(temp_memory);
					}

					bool is_int_constant = IsIntConstant(*current_token);
					if (is_int_constant) {
						token.token_class = TOKEN_INT_CONSTANT;
						token.entry_index = AddOrGetSymbolTableEntry(symbol_table, *current_token);
					}
					else {
						bool is_float_constant = IsFloatConstant(*current_token);
						if (is_float_constant) {
							token.token_class = TOKEN_FLOAT_CONSTANT;
							token.entry_index = AddOrGetSymbolTableEntry(symbol_table, *current_token);
						}
						else {
							bool is_bool_constant = IsBoolConstant(*current_token);
							if (is_bool_constant) {
								token.token_class = TOKEN_BOOL_CONSTANT;
								token.entry_index = AddOrGetSymbolTableEntry(symbol_table, *current_token);
							}
							else {
								bool is_valid_identifier = IsValidIdentifier(*current_token);
								if (is_valid_identifier) {
									token.token_class = TOKEN_IDENTIFIER;
									token.entry_index = AddOrGetSymbolTableEntry(symbol_table, *current_token);
								}
								else {
									char null_terminated_token[256];
									memcpy(null_terminated_token, current_token->characters, sizeof(char) * current_token->size);
									null_terminated_token[current_token->size] = '\0';

									char temp_memory[256];
									temp_memory[0] = '\0';
									sprintf(temp_memory, "Invalid identifier %s on line %zu", null_terminated_token, index + 1);
									free(source_file_content.characters);
									return StringMallocCopyFromPointer(temp_memory);
								}
							}
						}
					}
				}
			}

			Add(&pif->token_order, &token);
		}

		FreeStream(line_tokens);
	}

	DeallocateStrings(lines);
	FreeStream(lines);
	free(source_file_content.characters);
	return InvalidString();
}
