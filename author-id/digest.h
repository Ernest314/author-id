#ifndef DIGEST_H
#define DIGEST_H

#include "utils.h"

void write_summary(string filename, Memory* mem, string author_name);
void print_data_size(ifstream& stream);
void add_data_from_line(Memory& mem, string& line);

void digest_input(string filename);

#endif // DIGEST_H
