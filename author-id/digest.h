#ifndef DIGEST_H
#define DIGEST_H

#include <vector>
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
using std::ifstream;
using std::ofstream;
using std::fstream;
using std::ios;

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
		return (a.freq < b.freq);
	}
};

string to_lower_case(string input);	// Converts A-Z and { Á,É,Í,Ó,Ú,Ñ }
void print_data_size(ifstream& stream);
void add_word_to_file(string filename, string word, int freq);
void update_word(vector<Word>& list, string word);
void add_words_from_line(vector<Word>& list, string line, std::regex regex);
void update_word_freq(string filename, string word, int freq);
void combine_list_file(vector<Word>& list, fstream& stream, string filename);
void combine_list_file(	vector<Word>& list,
						fstream& stream,
						string filename,
						vector<Word>::iterator itr_beg,
						vector<Word>::iterator itr_end	);

void digest_input(string filename);

#endif // DIGEST_H
