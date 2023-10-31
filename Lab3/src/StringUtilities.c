#include "StringUtilities.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

string StringFromLiteral(const char* literal)
{
	return (string){ literal, strlen(literal) };
}

string ReadFile(const char* path)
{
	FILE* file = fopen(path, "rt");
	if (file) {
		const size_t ALLOCATION_SIZE = 1024;
		char* string_allocation = malloc(sizeof(char) * ALLOCATION_SIZE);
		size_t characters_read = fread(string_allocation, sizeof(char), ALLOCATION_SIZE, file);
		if (characters_read == ALLOCATION_SIZE) {
			characters_read--;
		}
		string_allocation[characters_read] = '\0';
		return (string){ string_allocation, characters_read };
	}
	return InvalidString();
}

string FindCharacter(string characters, char character)
{
	for (size_t index = 0; index < characters.size; index++) {
		if (characters.characters[index] == character) {
			return (string) { characters.characters + index, characters.size - index };
		}
	}
	return InvalidString();
}

string StringUntil(string characters, char character)
{
	for (size_t index = 0; index < characters.size; index++) {
		if (characters.characters[index] == character) {
			return (string) { characters.characters, index + 1 };
		}
	}
	return InvalidString();
}

string StringAfter(string original_string, string subrange)
{
	return (string) { subrange.characters + subrange.size, original_string.size - subrange.size - (size_t)original_string.characters + (size_t)subrange.characters };
}

string StringAdvance(string _string, size_t count) {
	return (string) { _string.characters + count, _string.size - count };
}

string StringMallocCopy(string _string)
{
	char* allocation = malloc(sizeof(char) * (_string.size + 1));
	memcpy(allocation, _string.characters, sizeof(char) * _string.size);
	allocation[_string.size] = '\0';
	return (string) { allocation, _string.size };
}

string StringMallocCopyFromPointer(const char* characters)
{
	return StringMallocCopy((string) { characters, strlen(characters) });
}

string InvalidString() {
	return (string) { NULL, 0 };
}

size_t FindString(ResizableStream strings, string _string)
{
	for (size_t index = 0; index < strings.size; index++) {
		const string* current_string = GetElement(strings, index);
		if (StringEqual(*current_string, _string)) {
			return index;
		}
	}
	return -1;
}

void DeallocateStrings(ResizableStream strings)
{
	for (size_t index = 0; index < strings.size; index++) {
		const string* current_string = GetElement(strings, index);
		free(current_string->characters);
	}
}

bool StringEqual(string a, string b)
{
	if (a.size == b.size) {
		return memcmp(a.characters, b.characters, sizeof(char) * a.size) == 0;
	}
	return false;
}

bool IsDigitChar(char character)
{
	return character >= '0' && character <= '9';
}

bool IsLowercaseChar(char character)
{
	return character >= 'a' && character <= 'z';
}

bool IsUppercaseChar(char character)
{
	return character >= 'A' && character <= 'Z';
}

bool IsWhitespaceChar(char character)
{
	if (character == ' ' || character == '\t' || character == '\n') {
		return true;
	}
	return false;
}

void ParseTokensFromLines(string parse_range, ResizableStream* tokens)
{
	size_t starting_index = 0;
	for (size_t index = 0; index < parse_range.size; index++) {
		while (index < parse_range.size && parse_range.characters[index] == '\n') {
			index++;
		}
		while (index < parse_range.size && parse_range.characters[index] != '\n') {
			index++;
		}
		if (starting_index < index) {
			string allocated_token = StringMallocCopy((string) { parse_range.characters + starting_index, index - starting_index });
			Add(tokens, &allocated_token);
			starting_index = index + 1;
		}
	}

	if (starting_index < parse_range.size) {
		string allocated_token = StringMallocCopy((string) { parse_range.characters + starting_index, parse_range.size - starting_index });
		Add(tokens, &allocated_token);
	}
}

void ParseTokensFromWhitespace(string parse_range, ResizableStream* tokens) {
	size_t index = 0;
	while (index < parse_range.size) {
		while (index < parse_range.size && IsWhitespaceChar(parse_range.characters[index])) {
			index++;
		}
		size_t token_start = index;
		while (index < parse_range.size && !IsWhitespaceChar(parse_range.characters[index])) {
			index++;
		}
		size_t token_end = index;
		if (token_start < token_end) {
			string allocated_token = StringMallocCopy((string) { parse_range.characters + token_start, token_end - token_start });
			Add(tokens, &allocated_token);
		}
	}
}

void FindAllOccurences(string parse_range, string token, ResizableStream* tokens)
{
	for (size_t index = 0; index < parse_range.size - token.size + 1; index++) {
		if (parse_range.characters[index] == token.characters[0]) {
			if (memcmp(parse_range.characters + index, token.characters, sizeof(char) * token.size) == 0) {
				Add(tokens, &index);
				index += token.size - 1;
			}
		}
	}
}

void ParseTokensWithSeparators(string parse_range, ResizableStream separators, ResizableStream operators, ResizableStream* tokens, bool skip_whitespace) {
	size_t starting_token_index = 0;
	for (size_t index = 0; index < parse_range.size; index++) {
		if (skip_whitespace) {
			bool skipped = false;
			while (index < parse_range.size && IsWhitespaceChar(parse_range.characters[index])) {
				index++;
				skipped = true;
			}
			if (skipped) {
				if (starting_token_index < index) {
					if (!IsWhitespaceChar(parse_range.characters[starting_token_index])) {
						string token_string = (string){ parse_range.characters + starting_token_index, index - starting_token_index - 1 };
						Add(tokens, &token_string);
						starting_token_index = index;
					}
					else {
						starting_token_index = index;
					}
				}
			}

			if (index == parse_range.size) {
				break;
			}
		}

		size_t operator_index = 0; 
		for (; operator_index < operators.size; operator_index++) {
			const string* operator_ = GetElement(operators, operator_index);
			if (parse_range.size - index >= operator_->size) {
				string current_string = (string){ parse_range.characters + index, operator_->size };
				if (memcmp(parse_range.characters + index, operator_->characters, sizeof(char) * operator_->size) == 0) {
					if (index != starting_token_index) {
						string token_string = (string){ parse_range.characters + starting_token_index, index - starting_token_index };
						Add(tokens, &token_string);
					}

					Add(tokens, &current_string);
					index += operator_->size - 1;
					starting_token_index = index + 1;
					break;
				}
			}
		}
		
		size_t separator_index = 0;
		if (operator_index == operators.size) {
			for (; separator_index < separators.size; separator_index++) {
				const string* separator = GetElement(separators, separator_index);
				if (parse_range.size - index >= separator->size) {
					string current_string = (string){ parse_range.characters + index, separator->size };
					if (memcmp(parse_range.characters + index, separator->characters, sizeof(char) * separator->size) == 0) {
						if (index != starting_token_index) {
							string token_string = (string){ parse_range.characters + starting_token_index, index - starting_token_index };
							Add(tokens, &token_string);
						}

						Add(tokens, &current_string);
						index += separator->size - 1;
						starting_token_index = index + 1;
						break;
					}
				}
			}
		}
	}

	if (starting_token_index < parse_range.size) {
		string token_string = (string){ parse_range.characters + starting_token_index, parse_range.size - starting_token_index };
		Add(tokens, &token_string);
	}
}