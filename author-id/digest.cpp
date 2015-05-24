#include "stdafx.h"
#include "digest.h"

string to_lower_case(string input);	// Converts A-Z and { Á,É,Í,Ó,Ú,Ñ }

void print_data_size(std::ifstream& stream);



void digest_input(string filename)
{
	// Print basic information and advance read cursor past author line.
	// (Author line must always exist and must be the first line.)
	cout << "Processing started..." << endl << endl;
	cout << "Input: " << filename << endl << endl;
	string filename_input = "Texts/" + filename;
	std::ifstream file_text(filename_input);
	print_data_size(file_text);
	string author_name;
	getline(file_text, author_name);
	string filename_author = "Authors/" + author_name + ".csv";
	std::fstream file_author(filename_author, std::ios::in | std::ios::out);
	cout << "Author: " << author_name << endl << endl;
	cout << "Output: " << author_name + ".csv" << endl << endl;

	// These are the data sorted using std::sort().
	// The "compare" struct is used to actually sort the data.
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
	std::vector<Word> word_list;	// Working list of words held in memory.

	string raw_input;
	std::regex word("([a-zA-záéíóúñÁÉÍÓÚÑ'’]+)");
	while (getline(file_text, raw_input)) {
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
		// TODO: Make constants settable via command-line options (5000, 2000, etc.)
		if (word_list.size() > 5000) {
			for (auto itr = word_list.begin() + 2000; itr != word_list.end(); ++itr) {
				file_author.seekg(0, std::ios::beg);
				string buffer; // to dump the first line into, for converting string to char, and rewriting file
				getline(file_author, buffer);
				string word_existing;
				bool wordExists = false;
				while (getline(file_author, word_existing, ',')) {
					if (itr->text == word_existing) {
						wordExists = true;
						getline(file_author, buffer);
						string line_old = word_existing + "," + buffer;
						int freq_existing = std::stoi(buffer);
						int freq_updated = freq_existing + itr->freq;
						std::stringstream line_write;
						line_write << word_existing << "," << freq_updated;
						string line_new = line_write.str();
						string filename_temp = "Authors/tmp/csv";
						std::ofstream file_updated(filename_temp);
						file_author.seekg(0, std::ios::beg);
						while (getline(file_author, buffer)) {
							if (buffer != line_old) {
								file_updated << buffer << endl;
							} else {
								file_updated << line_new << endl;
							}
						}
						file_updated.close();
						remove(filename_author.c_str());
						rename(filename_temp.c_str(), filename_author.c_str());
						file_author.open(filename_author);
						break;
					}
				}
				if (!wordExists) {
					file_author.seekp(0, std::ios::end);
					file_author << itr->text << "," << itr->freq << endl;
				}
			}
		}
	}
	std::sort(word_list.begin(), word_list.end(), word_compare());
	file_author.close();
	file_author.open(filename_author);
	file_author << "WORD,FREQ" << endl;
	for (auto itr = word_list.begin(); itr != word_list.end(); ++itr) {
		file_author << itr->text << "," << itr->freq << endl;
	}
	file_author.close();
	cout << endl << "Done!" << endl;
}

string to_lower_case(string input)
{
	string output;
	for (unsigned int i = 0; i < input.length(); i++) {
		if (input[i] >= 'A' && input[i] <= 'Z') {
			output += input[i] - ('A'-'a');
		} else if (	input[i] == 'Á' ||
					input[i] == 'É' ||
					input[i] == 'Í' ||
					input[i] == 'Ó' ||
					input[i] == 'Ú' ||
					input[i] == 'Ñ'		) {
			switch (input[i]) {
				case 'Á' :
					output = 'á';
					break;
				case 'É':
					output = 'é';
					break;
				case 'Í':
					output = 'í';
					break;
				case 'Ó':
					output = 'ó';
					break;
				case 'Ú':
					output = 'ú';
					break;
				case 'Ñ':
					output = 'ñ';
					break;
			}
		} else {
			output += input[i];
		}
	}
	return output;
}

void print_data_size(std::ifstream& stream)
{
	// First, get file size in bytes:
	stream.seekg(0, std::ios::end);
	float text_size_kb = stream.tellg() / 1024.0f;
	stream.seekg(0, std::ios::beg);
	cout << "Size: " << text_size_kb << " kilobytes" << endl << endl;

	// Translate bytes -> words: Wolfram Alpha claims an avg. 5.1 char / word.
	// If the word estimate is greater than 10000 (MAGIC_NUM!), round to the nearest thousand.
	int word_num = static_cast<int>(round(text_size_kb / 5.1 * 1000));
	if (word_num > 10000) {
		word_num = static_cast<int>(round(word_num / 1000.0f)) * 1000;
	}
	cout << "(Approximately " << word_num << " words.)" << endl << endl;
}
