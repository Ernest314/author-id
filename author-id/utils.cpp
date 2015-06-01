#include "stdafx.h"
#include "utils.h"

int get_precision(int width, float input)
{
	int sig_figs = width - 1; // decimal point
	int precision = sig_figs;
	float comparer = 1.0f;
	for (int i = 0; i < sig_figs-1; ++i) {
		if (input < comparer) {
			precision--;
			comparer *= 0.1f;
		}
	}
	if (precision < 0) {
		precision = 0;
	}
	return precision;
}

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
		} else if (input[i] == 'Á' ||
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
		} else {
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

void get_list_from_file(vector<Word>& list, string filename)
{
	list.clear();
	ifstream file_read(filename);
	string buffer;
	getline(file_read, buffer); // discard first line ("WORD,FREQ")
	while (getline(file_read, buffer)) {
		if (buffer.length() > 0) {
			stringstream converter(buffer);
			string word_read;
			getline(converter, word_read, ',');
			getline(converter, buffer);
			int freq_read = std::stoi(buffer);
			list.push_back(Word(word_read, freq_read));
		}
	}
	file_read.close();
}

void get_sentences_from_file(vector<int>& list, string filename)
{
	list.clear();
	ifstream file_read(filename);
	string buffer;
	getline(file_read, buffer); // discard first line ("WORDS")
	while (getline(file_read, buffer)) {
		if (buffer.length() > 0) {
			string raw;
			getline(file_read, raw);
			int freq = std::stoi(raw);
			list.push_back(freq);
		}
	}
	file_read.close();
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

void combine_list_file(vector<Word>& list, string filename)
{
	combine_list_file(list, filename, list.begin(), list.end());
}

void combine_list_file(	vector<Word>& list,
						string filename,
						vector<Word>::iterator itr_beg,
						vector<Word>::iterator itr_end)
{
	string filename_temp = "Digests/temp.dat";
	ifstream file_read(filename);
	vector<Word> list_read;

	string buffer;
	getline(file_read, buffer);
	while (getline(file_read, buffer)) {
		if (buffer.length() > 0) {
			stringstream converter(buffer);
			string word_read;
			getline(converter, word_read, ',');
			getline(converter, buffer);
			int freq_read = std::stoi(buffer);
			list_read.push_back(Word(word_read, freq_read));
		}
	}
	file_read.close();

	for (auto itr_list = itr_beg; itr_list != itr_end; ++itr_list) {
		bool wordExists = false;
		for (auto itr_file = list_read.begin(); itr_file != list_read.end(); ++itr_file) {
			if (itr_list->text == itr_file->text) {
				itr_file->freq += itr_list->freq;
				wordExists = true;
				break;
			}
		}
		if (!wordExists) {
			list_read.push_back(Word(itr_list->text, itr_list->freq));
		}
	}
	list.erase(itr_beg, itr_end);

	std::sort(list_read.begin(), list_read.end(), word_compare());
	ofstream file_updated(filename_temp);
	file_updated << "WORD,FREQ" << endl;
	for (auto itr = list_read.begin(); itr != list_read.end(); ++itr) {
		file_updated << itr->text << "," << itr->freq << endl;
	}
	file_updated.close();

	remove(filename.c_str());
	rename(filename_temp.c_str(), filename.c_str());
}
