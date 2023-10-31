#pragma once
#include "ResizableStream.h"
#include "StringUtilities.h"

// Returns false if it contains foreign characters
bool IsTokenValid(string token);

bool IsValidIdentifier(string token);

// This doesn't take into account the case for +/- in front of the token
// You must check manually that
bool IsIntConstant(string token);

// This doesn't take into account the case for +/- in front of the token
// You must check manually that
bool IsFloatConstant(string token);

bool IsBoolConstant(string token);

bool IsStringConstant(string token);