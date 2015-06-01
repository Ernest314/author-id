#ifndef COMPILE_H
#define COMPILE_H

#include "utils.h"

string read_author(ifstream& stream);
int read_word_N(ifstream& stream);
double read_word_u(ifstream& stream);
double read_word_s(ifstream& stream);

void compile_input(string filename);

#endif // COMPILE_H
