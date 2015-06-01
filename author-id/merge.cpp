#include "stdafx.h"
#include "merge.h"

void merge_input(string filename)
{
	cout << "Processing started..." << endl << endl;
	string filename_root(filename.begin(), filename.end()-4); // ".txt"
	string filename_sum = "Digests/" + filename_root + "-SUM.txt";
	string filename_word = "Digests/" + filename_root + "-WORD.txt";
	cout << "Merging:" << endl <<
		"\t" << filename_sum << endl <<
		"\t" << filename_word << endl << endl;
	
	ifstream file_sum(filename_sum);
	string author_name = read_author(file_sum);
	file_sum.close();
	string filename_author_sum = "Authors/" + author_name + "-SUM.txt";
	string filename_author_word = "Authors/" + author_name + "-WORD.csv";

	cout << "Into:" << endl <<
		"\t" << filename_author_sum << endl <<
		"\t" << filename_author_word << endl << endl;

	Memory RAM_init;
	get_list_from_file(RAM_init.word_list, filename_author_word);
	Memory RAM_new;
	get_list_from_file(RAM_new.word_list, filename_word);
	for (auto itr = RAM_new.word_list.begin(); itr != RAM_new.word_list.end(); ++itr) {
		bool wasFound = false;
		for (auto jtr = RAM_init.word_list.begin(); jtr != RAM_init.word_list.end(); ++jtr) {
			if (jtr->text == itr->text) {
				jtr->freq += itr->freq;
				wasFound = true;
				break;
			}
		}
		if (!wasFound) {
			RAM_init.word_list.push_back(Word(itr->text, itr->freq));
		}
	}
	std::sort(RAM_init.word_list.begin(), RAM_init.word_list.end(), word_compare());
	
	string filename_temp = "temp.dat";
	ofstream file_temp(filename_temp);
	file_temp << "WORD,FREQ" << endl;
	for (auto itr = RAM_init.word_list.begin(); itr != RAM_init.word_list.end(); ++itr) {
		file_temp << itr->text << "," << itr->freq << endl;
	}
	file_temp.close();
	remove(filename_author_word.c_str());
	rename(filename_temp.c_str(), filename_author_word.c_str());

}

string read_author(ifstream& stream)
{
	string buffer;
	getline(stream, buffer, ':'); // discard  "Author:"
	getline(stream, buffer);
	return string(buffer.begin() + 1, buffer.end()); // discard leading " "
}
