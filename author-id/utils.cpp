#include "stdafx.h"
#include "utils.h"

bool does_word_match_list(string word, vector<string> list) {
	bool does_match = false;
	for (unsigned int i = 0; i < list.size(); ++i) {
		if (word == list[i]) {
			does_match = true;
			break;
		}
	}
	return does_match;
}

int get_word_count(string input)
{
	regex regex_word("([a-zA-záéíóúñÁÉÍÓÚÑ'’]+)");
	sregex_iterator find_word(input.begin(), input.end(), regex_word);
	int count = std::distance(find_word, sregex_iterator());
	return count;
}

string to_lower_case(string input)
{
	string output;
	for (unsigned int i = 0; i < input.length(); i++) {
		if (input[i] >= 'A' && input[i] <= 'Z') {
			output += input[i] - ('A' - 'a'); // this trick works because ASCII :P
		}
		else if (input[i] == 'Á' ||
			input[i] == 'É' ||
			input[i] == 'Í' ||
			input[i] == 'Ó' ||
			input[i] == 'Ú' ||
			input[i] == 'Ñ') {
			// otherwise we must assign each character individually.
			// Note: these are inside a single "else if" clause to optimize
			// evaluation; doing this *should* reduce the number of branches.
			switch (input[i]) {
			case 'Á':
				output = 'á';
				break;
			case 'É':
				output = 'é';
				break;
			case 'Í':
				output = 'í';
				break;
			case 'Ó':
				output = 'ó';
				break;
			case 'Ú':
				output = 'ú';
				break;
			case 'Ñ':
				output = 'ñ';
				break;
			}
		}
		else {
			output += input[i];
		}
	}
	return output;
}