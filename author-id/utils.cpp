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

void update_word(vector<Word>& list, string word)
{
	// If the word is already in memory, increment its count.
	// If it isn't, add the word to the end of the list (with a count of 1).
	bool doesExist = false;
	for (auto itr = list.begin(); itr != list.end(); ++itr) {
		if (word == itr->text) {
			(itr->freq)++;
			doesExist = true;
			break;
		}
	}
	if (!doesExist) {
		list.push_back(Word(word, 1));
	}
}

bool file_check_create(string filename)
{
	bool didExist = false;
	ifstream f_check(filename);
	didExist = f_check.good();
	f_check.close();
	if (didExist) {
		remove(filename.c_str());
	}
	ofstream f_create(filename);
	f_create << endl;
	f_create.close();
	return didExist;
}

string create_file_and_name(string prepend, string main, string append)
{
	string output(main.begin(), main.end() - 4);
	output = prepend + output + append;
	file_check_create(output);
	return output;
}
