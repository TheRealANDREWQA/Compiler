#include "FiniteAutomata.h"
#include "StringUtilities.h"
#include <stdio.h>

FiniteAutomata CreateFiniteAutomata() {
	FiniteAutomata fa;

	fa.states = CreateStream(0, sizeof(string));
	fa.alphabet = CreateStream(0, sizeof(char));
	fa.final_states = CreateStream(0, sizeof(size_t));
	fa.initial_state = -1;
	fa.transitions = CreateTable(
		32,
		sizeof(FATableEntry),
		sizeof(size_t),
		HashTableMapPowerOfTwo,
		HashTableHashSizeT,
		HashTableCompareSizeT
	);

	return fa;
}

size_t FindFAStateIndex(const FiniteAutomata* finite_automata, string state_string)
{
	return FindStringInStream(finite_automata->states, state_string);
}

bool ReadFiniteAutomataFile(const char* path, FiniteAutomata* finite_automata) {
	string file = ReadFile(path);
	if (file.size == 0) {
		return false;
	}

	ResizableStream lines = CreateStream(0, sizeof(string));
	ParseTokensFromLines(file, &lines);

#define DEALLOCATE free(file.characters); FreeStream(lines);

	if (lines.size != 5) {
		DEALLOCATE;
		return false;
	}

	string* first_line = GetElement(lines, 0);
	if (!StringStartsWith(*first_line, StringFromLiteral("alphabet = {"))) {
		DEALLOCATE;
		return false;
	}
	string alphabet_identifier = ParseStringInBetween(*first_line, '{', '}');
	if (alphabet_identifier.size == 0) {
		DEALLOCATE;
		return false;
	}
	ResizableStream alphabet_identifier_stream;
	alphabet_identifier_stream.buffer = alphabet_identifier.characters;
	alphabet_identifier_stream.element_size = sizeof(char);
	alphabet_identifier_stream.size = alphabet_identifier.size + 1;
	alphabet_identifier.characters[alphabet_identifier.size] = '\0';
	FreeStream(finite_automata->alphabet);
	finite_automata->alphabet = CopyStream(alphabet_identifier_stream);
	finite_automata->alphabet.size--;

	string* second_line = GetElement(lines, 1);
	if (!StringStartsWith(*second_line, StringFromLiteral("states = {"))) {
		DEALLOCATE;
		return false;
	}
	string states_range = ParseStringInBetween(*second_line, '{', '}');
	if (states_range.size == 0) {
		DEALLOCATE;
		return false;
	}

	ResizableStream states_tokens = CreateStream(8, sizeof(string));
	ParseTokensByCharacter(states_range, ',', &states_tokens);
	for (size_t index = 0; index < states_tokens.size; index++) {
		string* token = GetElement(states_tokens, index);
		*token = StringRemoveLeadingAndEndingChar(*token, ' ', ' ');
		string stable_token = StringMallocCopy(*token);
		Add(&finite_automata->states, &stable_token);
	}
	FreeStream(states_tokens);

	string* third_line = GetElement(lines, 2);
	if (!StringStartsWith(*third_line, StringFromLiteral("transitions = {"))) {
		DEALLOCATE;
		return false;
	}
	string transitions_range = ParseStringInBetween(*third_line, '{', '}');
	if (transitions_range.size == 0) {
		DEALLOCATE;
		return false;
	}

	ResizableStream transition_tokens = CreateStream(8, sizeof(string));
	ParseTokensByCharacter(transitions_range, ',', &transition_tokens);
	for (size_t index = 0; index < transition_tokens.size; index++) {
		string* token = GetElement(transition_tokens, index);
		*token = StringRemoveLeadingAndEndingChar(*token, ' ', ' ');
		ResizableStream parsed_values = CreateStream(3, sizeof(string));
		ParseTokensByCharacter(*token, '|', &parsed_values);
		if (parsed_values.size != 3) {
			FreeStream(parsed_values);
			FreeStream(transition_tokens);
			DEALLOCATE;
			return false;
		}

		size_t state_index = FindFAStateIndex(finite_automata, *(string*)GetElement(parsed_values, 0));
		if (state_index == -1) {
			FreeStream(parsed_values);
			FreeStream(transition_tokens);
			DEALLOCATE;
			return false;
		}

		FATableEntry* entry = FindTablePtr(&finite_automata->transitions, &state_index);
		string parsed_terminal = *(string*)GetElement(parsed_values, 1);
		if (entry == NULL) {
			FATableEntry new_entry;
			new_entry.stream = CreateStream(parsed_terminal.size, sizeof(FATransition));

			for (size_t terminal_index = 0; terminal_index < parsed_terminal.size; terminal_index++) {
				FATransition transition;
				transition.terminal = parsed_terminal.characters[terminal_index];
				transition.target_state_index = FindFAStateIndex(finite_automata, *(string*)GetElement(parsed_values, 2));
				Add(&new_entry.stream, &transition);
			}
			int should_resize = AddTable(&finite_automata->transitions, &new_entry, &state_index);
			if (should_resize) {
				GrowTable(&finite_automata->transitions, HashTableGrowPowerOfTwo);
			}
		}
		else {
			for (size_t terminal_index = 0; terminal_index < parsed_terminal.size; terminal_index++) {
				FATransition transition;
				transition.terminal = parsed_terminal.characters[terminal_index];
				transition.target_state_index = FindFAStateIndex(finite_automata, *(string*)GetElement(parsed_values, 2));
				Add(&entry->stream, &transition);
			}
		}
		FreeStream(parsed_values);
	}
	FreeStream(transition_tokens);

	string* fourth_line = GetElement(lines, 3);
	if (!StringStartsWith(*fourth_line, StringFromLiteral("initial_state = "))) {
		DEALLOCATE;
		return false;
	}
	string initial_state_after_equal = FindCharacter(*fourth_line, '=');
	initial_state_after_equal = StringAdvance(initial_state_after_equal, 1);
	initial_state_after_equal = StringRemoveLeadingAndEndingChar(initial_state_after_equal, ' ', ' ');
	string initial_state_token = initial_state_after_equal;
	size_t initial_state_index = FindFAStateIndex(finite_automata, initial_state_token);
	if (initial_state_index == -1) {
		DEALLOCATE;
		return false;
	}
	finite_automata->initial_state = initial_state_index;

	string* fifth_line = GetElement(lines, 4);
	if (!StringStartsWith(*fifth_line, StringFromLiteral("final_states = {"))) {
		DEALLOCATE;
		return false;
	}

	string final_state_range = ParseStringInBetween(*fifth_line, '{', '}');
	if (final_state_range.size == 0) {
		DEALLOCATE;
		return false;
	}

	ResizableStream final_state_tokens = CreateStream(8, sizeof(string));
	ParseTokensByCharacter(final_state_range, ',', &final_state_tokens);
	for (size_t index = 0; index < final_state_tokens.size; index++) {
		string* token = GetElement(final_state_tokens, index);
		*token = StringRemoveLeadingAndEndingChar(*token, ' ', ' ');
		size_t final_state_index = FindFAStateIndex(finite_automata, *token);
		if (final_state_index == -1) {
			FreeStream(final_state_tokens);
			DEALLOCATE;
			return false;
		}

		Add(&finite_automata->final_states, &final_state_index);
	}

	FreeStream(final_state_tokens);
	DEALLOCATE;
	return true;
#undef DEALLOCATE
}

bool FABacktracking(const FiniteAutomata* finite_automata, size_t current_state, string sequence) {
	if (sequence.size == 0) {
		return Find(finite_automata->final_states, &current_state) != -1;
	}

	FATableEntry* entry = GetTablePtr(&finite_automata->transitions, current_state);
	for (size_t index = 0; index < entry->stream.size; index++) {
		FATransition* transition = GetElement(entry->stream, index);
		if (transition->terminal == sequence.characters[0]) {
			if (FABacktracking(finite_automata, transition->target_state_index, StringAdvance(sequence, 1))) {
				return true;
			}
		}
	}
	return false;
}

bool FiniteAutomataVerifySequence(const FiniteAutomata* finite_automata, string sequence)
{
	return FABacktracking(finite_automata, finite_automata->initial_state, sequence);
}

int IteratePrintTransitions(void* element, void* identifier, void* user_data) {
	FiniteAutomata* fa = user_data;
	FATableEntry* entry = element;
	size_t* state_index = identifier;

	string* state_string = GetElement(fa->states, *state_index);
	for (size_t index = 0; index < entry->stream.size; index++) {
		FATransition* transition = GetElement(entry->stream, index);
		string* target_state_string = GetElement(fa->states, transition->target_state_index);
		printf("%s -> %c -> %s\n", state_string->characters, transition->terminal, target_state_string->characters);
	}
	return 0;
}

void FiniteAutomataConsole(const FiniteAutomata* finite_automata)
{
	while (true) {
		printf("Menu Options:\n1. Print states\n2. Print the alphabet\n3. Print all transitions\n4. Print initial state\n5. Print final states\n6. Verify sequence\n7. Exit\nChoose Options: ");
		int option;
		scanf("%d", &option);

		if (option == 7) {
			break;
		}
		else if (option == 1) {
			for (size_t index = 0; index < finite_automata->states.size; index++) {
				string* state = GetElement(finite_automata->states, index);
				printf("%s\n", state->characters);
			}
		}
		else if (option == 2) {
			char* alphabet = finite_automata->alphabet.buffer;
			printf("%s\n", alphabet);
		}
		else if (option == 3) {
			IterateTable(&finite_automata->transitions, IteratePrintTransitions, finite_automata);
		}
		else if (option == 4) {
			string* initial_state_string = GetElement(finite_automata->states, finite_automata->initial_state);
			printf("%s\n", initial_state_string->characters);
		}
		else if (option == 5) {
			for (size_t index = 0; index < finite_automata->final_states.size; index++) {
				string* state_string = GetElement(finite_automata->states, *(size_t*)GetElement(finite_automata->final_states, index));
				printf("%s\n", state_string->characters);
			}
		}
		else if (option == 6) {
			char sequence[512];
			scanf("%s", sequence);
			bool is_verified = FiniteAutomataVerifySequence(finite_automata, StringFromLiteral(sequence));
			if (is_verified) {
				printf("The sequence is accepted by the FA\n");
			}
			else {
				printf("The sequence is not accepted by the FA\n");
			}
		}
	}
}

int IterateDeallocateTransitions(void* element, void* identifier, void* extra_data) {
	FATableEntry* entry = element;
	FreeStream(entry->stream);
	return 0;
}

void DestroyFiniteAutomata(FiniteAutomata* finite_automata) {
	FreeStream(finite_automata->alphabet);
	FreeStream(finite_automata->final_states);
	DeallocateStrings(finite_automata->states);
	IterateTable(&finite_automata->transitions, IterateDeallocateTransitions, NULL);
	DestroyTable(&finite_automata->transitions);
	memset(finite_automata, 0, sizeof(*finite_automata));
}