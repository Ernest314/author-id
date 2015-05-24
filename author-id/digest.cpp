#include "stdafx.h"
#include "digest.h"

void digest_input(string filename)
{
	cout << "Processing started..." << endl << endl;
	string author_name;
	std::ifstream file_in("Texts/" + filename);
	getline(file_in, author_name);
	cout << "Author: " << author_name << endl << endl;
}
