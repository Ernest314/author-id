#ifndef MERGE_H
#define MERGE_H

#include "utils.h"
#include "digest.h"

string read_author(ifstream& stream);

void merge_input(string filename);

#endif // MERGE_H
