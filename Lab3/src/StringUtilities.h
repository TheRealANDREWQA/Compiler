#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include "ResizableStream.h"

typedef struct {
	char* characters;
	size_t size;
} string;

string StringFromLiteral(const char* literal);

string ReadFile(const char* path);

string FindCharacter(string characters, char character);

string FindString(string characters, string _string);

string StringUntil(string characters, char character);

string StringAfter(string original_string, string subrange);

string StringAdvance(string _string, size_t count);

string StringMallocCopy(string _string);

string StringMallocCopyFromPointer(const char* characters);

string InvalidString();

// Returns the index in the stream if it finds it, else -1 if it doesn't exist
size_t FindStringInStream(ResizableStream strings, string _string);

// Deallocates all elements, but not the stream itself
void DeallocateStrings(ResizableStream strings);

string StringRemoveLeadingChar(string _string, char character);

string StringRemoveEndingChar(string _string, char character);

string StringRemoveLeadingAndEndingChar(string _string, char leading_character, char ending_character);

bool StringEqual(string a, string b);

bool IsDigitChar(char character);

bool IsLowercaseChar(char character);

bool IsUppercaseChar(char character);

bool IsWhitespaceChar(char character);

// Tokens must have as element type string
void ParseTokensFromLines(string parse_range, ResizableStream* tokens);

// Tokens must have as element type string
void ParseTokensFromWhitespace(string parse_range, ResizableStream* tokens);

// Tokens must have as element type size_t
// Fills in all the positions of all occurences of the token
void FindAllOccurences(string parse_range, string token, ResizableStream* tokens);

void ParseTokensWithSeparators(
	string parse_range, 
	ResizableStream separators, 
	ResizableStream operators, 
	ResizableStream* tokens, 
	bool skip_whitespace
);

// Tokens must have as element type string
void ParseTokensByCharacter(string parse_range, char character, ResizableStream* tokens);

// strings must have as element type string
bool ParseStringsFromFormat(string parse_range, string format_string, ResizableStream* strings);

string ParseStringInBetween(string characters, char start_character, char end_character);

bool StringStartsWith(string characters, string substring);