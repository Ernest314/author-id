#include "stdafx.h"
#include "compare.h"

void compare_input(string filename_A, string filename_B)
{
	cout << "Processing started..." << endl << endl;
	string author_name_A(filename_A.begin(), filename_A.end() - 4); // ".txt"
	string author_name_B(filename_B.begin(), filename_B.end() - 4); // ".txt"
	string filename_word_A = "Authors/" + author_name_A + "-WORD.csv";
	string filename_word_B = "Authors/" + author_name_B + "-WORD.csv";
	cout << "Input A (base): " << filename_word_A << endl;
	cout << "Input B (head): " << filename_word_B << endl << endl;

	string filename_output = "COMPARE/" + author_name_A + " - " + author_name_B + ".csv";
	cout << "Output: " << filename_output << endl << endl;

	Memory RAM_A;
	get_list_from_file(RAM_A.word_list, filename_word_A);
	Memory RAM_B_old;
	get_list_from_file(RAM_B_old.word_list, filename_word_B);
	Memory RAM_B_new;

	for (auto itr = RAM_A.word_list.begin(); itr != RAM_A.word_list.end(); ++itr) {
		string base_word = itr->text;
		bool wasFound = false;
		for (auto jtr = RAM_B_old.word_list.begin(); jtr != RAM_B_old.word_list.end(); ++jtr) {
			if (jtr->text == base_word) {
				wasFound = true;
				RAM_B_new.word_list.push_back(Word(base_word, jtr->freq));
				break;
			}
		}
		if (!wasFound) {
			RAM_B_new.word_list.push_back(Word(base_word, 0));
		}
	}

	remove(filename_output.c_str());
	ofstream file_output(filename_output);
	file_output << "WORD,FREQ_A,FREQ_B" << endl;
	unsigned int output_size = RAM_B_new.word_list.size();
	for (unsigned int i = 0; i < output_size; ++i) {
		file_output << RAM_A.word_list[i].text << "," <<
			RAM_A.word_list[i].freq << "," <<
			RAM_B_new.word_list[i].freq << endl;
	}
	file_output.close();

	cout << "Done!" << endl << endl << endl;
}
