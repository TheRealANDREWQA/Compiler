#include "ParsingRules.h"

bool IsTokenValid(string token) {
	for (size_t index = 0; index < token.size; index++) {
		if (!IsUppercaseChar(token.characters[index])) {
			if (!IsLowercaseChar(token.characters[index])) {
				if (!IsDigitChar(token.characters[index])) {
					// Dot can appear in float constants
					if (token.characters[index] != '.') {
						return false;
					}
				}
			}
		}
	}
	return true;
}

bool IsValidIdentifier(string token) {
	// Assumes that the token passed the general alphabet test
	if (IsDigitChar(token.characters[0])) {
		return false;
	}
	return true;
}

bool IsIntConstant(string token) {
	for (size_t index = 0; index < token.size; index++) {
		if (!IsDigitChar(token.characters[index])) {
			return false;
		}
	}
	return true;
}

bool IsFloatConstant(string token) {
	for (size_t index = 0; index < token.size; index++) {
		if (!IsDigitChar(token.characters[index]) && token.characters[index] != '.') {
			return false;
		}
	}
	return true;
}

bool IsBoolConstant(string token) {
	string true_string = StringFromLiteral("true");
	if (StringEqual(true_string, token)) {
		return true;
	}
	string false_string = StringFromLiteral("false");
	if (StringEqual(false_string, token)) {
		return true;
	}

	return false;
}

bool IsStringConstant(string token) {
	if (token.size >= 2) {
		if (token.characters[0] == '\"' && token.characters[token.size - 1] == '\"') {
			return true;
		}
	}
	return false;
}