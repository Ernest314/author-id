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

void write_summary(string filename, Memory* mem, string author_name);
bool file_check_create(string filename);
string create_filename(string prepend, string main, string append);
void print_data_size(ifstream& stream);
void update_word(vector<Word>& list, string word);
void add_data_from_line(Memory& mem, string& line);
void get_list_from_file(Memory& mem, string filename);
void combine_list_file(Memory& mem, string filename);
void combine_list_file(	vector<Word>& mem,
						string filename,
						vector<Word>::iterator itr_beg,
						vector<Word>::iterator itr_end	);

void digest_input(string filename);

#endif // DIGEST_H
