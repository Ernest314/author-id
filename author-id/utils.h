#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include <vector>
#include <iterator>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <regex>

using std::vector;
using std::endl;
using std::string;
using std::regex;
using std::sregex_iterator;
using std::stringstream;
using std::ifstream;
using std::ofstream;
using std::fstream;
using std::ios;

// These are the return codes for the main .exe
enum RunMode {
	ERROR_UNKNOWN = -1,
	ERROR_NO_ARG = -2,
	ERROR_BAD_ARG = -3,
	ERROR_NO_PARAM = -4,
	ERROR_BAD_PARAM = -5,
	ERROR_NOT_TXT = -6,

	MODE_ABOUT = 0,
	MODE_HELP,
	MODE_LIST,
	MODE_DIGEST,
	MODE_COMPILE,
	MODE_NUM
};

// This enum is used for referring to punctuation.
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

// These are the structs to sort later with std::sort().
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

// This struct holds all the program needs to know when
// processing files (digest, compile, etc.).
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

int get_word_count(string input);

string to_lower_case(string input);	// Converts A-Z and { Á,É,Í,Ó,Ú,Ñ }

void update_word(vector<Word>& list, string word);

void get_list_from_file(vector<Word>& list, string filename);

bool file_check_create(string filename);

string create_file_and_name(string prepend, string main, string append);

void combine_list_file(vector<Word>& list, string filename);

void combine_list_file(	vector<Word>& mem,
						string filename,
						vector<Word>::iterator itr_beg,
						vector<Word>::iterator itr_end	);

#endif // UTILS_H
