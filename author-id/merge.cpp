#include "stdafx.h"
#include "merge.h"

void merge_input(string filename)
{
	cout << "Processing started..." << endl << endl;
	string filename_root(filename.begin(), filename.end()-4); // ".txt"
	string filename_sum = "Digests/" + filename_root + "-SUM.txt";
	string filename_word = "Digests/" + filename_root + "-WORD.csv";
	string filename_sent = "Digests/" + filename_root + "-SENT.csv";
	cout << "Merging:" << endl <<
		"\t" << filename_sum << endl <<
		"\t" << filename_word << endl <<
		"\t" << filename_sent << endl << endl;
	
	ifstream file_sum(filename_sum);
	string author_name = read_author(file_sum);
	file_sum.close();
	string filename_author_sum = "Authors/" + author_name + "-SUM.txt";
	string filename_author_word = "Authors/" + author_name + "-WORD.csv";
	string filename_author_sent = "Authors/" + author_name + "-SENT.csv";

	cout << "Into:" << endl <<
		"\t" << filename_author_sum << endl <<
		"\t" << filename_author_word << endl <<
		"\t" << filename_author_sent << endl << endl;

	// merge words
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
	ofstream file_temp_word(filename_temp);
	file_temp_word << "WORD,FREQ" << endl;
	for (auto itr = RAM_init.word_list.begin(); itr != RAM_init.word_list.end(); ++itr) {
		file_temp_word << itr->text << "," << itr->freq << endl;
	}
	file_temp_word.close();
	remove(filename_author_word.c_str());
	rename(filename_temp.c_str(), filename_author_word.c_str());

	// merge sentences
	get_sentences_from_file(RAM_init.sentence_len, filename_author_sent);
	get_sentences_from_file(RAM_new.sentence_len, filename_sent);
	RAM_init.sentence_len.insert(RAM_init.sentence_len.end(),
		RAM_new.sentence_len.begin(),
		RAM_new.sentence_len.end());

	ofstream file_temp_sent(filename_temp);
	file_temp_sent << "WORDS" << endl;
	for (auto itr = RAM_init.sentence_len.begin(); itr != RAM_init.sentence_len.end(); ++itr) {
		file_temp_sent << *itr << endl;
	}
	file_temp_sent.close();
	remove(filename_author_sent.c_str());
	rename(filename_temp.c_str(), filename_author_sent.c_str());

	// done
	write_summary(filename_author_sum, &RAM_init, author_name);
}

string read_author(ifstream& stream)
{
	string buffer;
	getline(stream, buffer, ':'); // discard  "Author:"
	getline(stream, buffer);
	return string(buffer.begin() + 1, buffer.end()); // discard leading " "
}
