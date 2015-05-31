#ifndef DIGEST_H
#define DIGEST_H

#include "utils.h"

#include <cmath>
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
using std::sregex_iterator;
using std::ifstream;
using std::ofstream;
using std::fstream;
using std::ios;

void write_summary(string filename, Memory* mem, string author_name);
void print_data_size(ifstream& stream);
void add_data_from_line(Memory& mem, string& line);

void digest_input(string filename);

#endif // DIGEST_H
