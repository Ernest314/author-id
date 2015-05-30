#ifndef DIGEST_H
#define DIGEST_H

#include <vector>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <string>
#include <fstream>
#include <regex>

using std::vector;
using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::regex;
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
	bool isDLG;

	Word(string text_in, int freq_in, bool isDLG_in) :
		text(text_in),
		freq(freq_in),
		isDLG(isDLG_in)
	{}
};
struct word_compare {
	inline bool operator()(Word a, Word b) {
		return (a.freq > b.freq);
	}
};

struct Memory {
	int total_words_nar;
	int total_words_dlg;
	vector<Word> word_list;
	vector<int> punc_freq;
	vector<int> sentence_len;
	int squared_dlg_dist_sum;

	Memory() :
		punc_freq(static_cast<int>(PUNC_NUM), 0)
	{}
};

bool file_check_create(string filename);
string create_filename(string prepend, string main, string append);
string to_lower_case(string input);	// Converts A-Z and { Á,É,Í,Ó,Ú,Ñ }
void print_data_size(ifstream& stream);
void add_word_to_file(string filename, string word, int freq);
void update_word(vector<Word>& list, string word);
void add_data_from_line(Memory& mem, string line);
void update_word_freq(string filename, string word, int freq);
void combine_list_file(vector<Word>& list, fstream& stream, string filename);
void combine_list_file(	vector<Word>& list,
						fstream& stream,
						string filename,
						vector<Word>::iterator itr_beg,
						vector<Word>::iterator itr_end	);

void digest_input(string filename);

#endif // DIGEST_H
