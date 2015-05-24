#include "stdafx.h"
#include "digest.h"

string to_lower_case(string input);

void digest_input(string filename)
{
	cout << "Processing started..." << endl << endl;
	string author_name;
	std::ifstream file_in("Texts/" + filename);
	getline(file_in, author_name);
	std::fstream file_out("Authors/" + author_name + ".csv", std::ios::out);
	cout << "Author: " << author_name << endl << endl;
	string raw_input;

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
	std::vector<Word> word_list;

	std::regex word("([a-zA-záéíóúñÁÉÍÓÚÑ'’]+)");
	while (getline(file_in, raw_input)) {
		std::sregex_iterator find(raw_input.begin(), raw_input.end(), word);
		for (std::sregex_iterator find_end; find != find_end; find++) {
			std::smatch match_word = *find;
			string word_found = match_word.str();
			word_found = to_lower_case(word_found);
			bool doesExist = false;
			for (auto itr = word_list.begin(); itr != word_list.end(); ++itr) {
				if (word_found == itr->text) {
					(itr->freq)++;
					doesExist = true;
					break;
				}
			}
			if (!doesExist) {
				word_list.push_back(Word(word_found, 1));
			}
		}
	}
	std::sort(word_list.begin(), word_list.end(), word_compare());
	file_out << "WORD,FREQ" << endl;
	for (auto itr = word_list.begin(); itr != word_list.end(); ++itr) {
		file_out << itr->text << "," << itr->freq << endl;
		//cout << itr->text << "\t\t" << itr->freq << endl;
	}
	file_out.flush();
	file_out.close();
	cout << endl << "Done!" << endl;
}

string to_lower_case(string input)
{
	string output;
	for (unsigned int i = 0; i < input.length(); i++) {
		if (input[i] >= 'A' && input[i] <= 'Z') {
			output += input[i] - ('A'-'a');
		} else {
			output += input[i];
		}
	}
	return output;
}
