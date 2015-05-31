#ifndef DIGEST_H
#define DIGEST_H

#include "utils.h"

#include <cmath>
#include <vector>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <regex>

using std::vector;
using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::stringstream;
using std::regex;
using std::sregex_iterator;
using std::ifstream;
using std::ofstream;
using std::fstream;
using std::ios;

enum REGEX_PUNC {
	PUNC_PERIOD = 0,
	PUNC_COMMA,
	PUNC_SEMICOLON,
	PUNC_COLON,
	PUNC_EXCLAMATION,
	PUNC_QUESTION,
	PUNC_DASH,
	PUNC_ELLIPSIS,
	PUNC_NUM
};

// These are the data to sort later with std::sort().
// The "compare" struct is used to actually sort the data.
struct Word {
	string text;
	int freq;

	Word(string text_in, int freq_in) :
		text(text_in),
		freq(freq_in)
	{}
};
struct word_compare {
	inline bool operator()(Word a, Word b) {
		return (a.freq > b.freq);
	}
};

struct Memory {
	int sentence_carry;
	vector<Word> word_list;
	vector<int> punc_freq;
	vector<int> sentence_len;

	Memory() :
		sentence_carry(0),
		punc_freq(static_cast<int>(PUNC_NUM), 0)
	{}
};

bool does_word_match_list(string word, vector<string> list);
void write_summary(string filename, Memory* mem, string author_name);
bool file_check_create(string filename);
string create_filename(string prepend, string main, string append);
string to_lower_case(string input);	// Converts A-Z and { Á,É,Í,Ó,Ú,Ñ }
void print_data_size(ifstream& stream);
void update_word(vector<Word>& list, string word);
int count_words(string input);
void add_data_from_line(Memory& mem, string& line);
void get_list_from_file(Memory& mem, string filename);
void combine_list_file(Memory& mem, string filename);
void combine_list_file(	vector<Word>& mem,
						string filename,
						vector<Word>::iterator itr_beg,
						vector<Word>::iterator itr_end	);

void digest_input(string filename);

#endif // DIGEST_H
