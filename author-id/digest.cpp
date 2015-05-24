#include "stdafx.h"
#include "digest.h"

void digest_input(string filename)
{
	// Print basic information and advance read cursor past author line.
	// (Author line must always exist and must be the first line.)
	cout << "Processing started..." << endl << endl;
	cout << "Input: " << filename << endl << endl;
	string filename_input = "Texts/" + filename;
	ifstream file_text(filename_input);
	print_data_size(file_text);
	string author_name;
	getline(file_text, author_name);
	string filename_author = "Authors/" + author_name + ".csv";
	fstream file_author(filename_author, ios::in | ios::out);
	cout << "Author: " << author_name << endl << endl;
	cout << "Output: " << author_name + ".csv" << endl << endl;

	vector<Word> word_list;	// Working list of words held in memory.
	string raw_input;
	std::regex regex_word("([a-zA-záéíóúñÁÉÍÓÚÑ'’]+)");
	while (getline(file_text, raw_input)) {
		add_words_from_line(word_list, raw_input, regex_word);

		// TODO: Make constants settable via command-line options (5000, 2000, etc.)
		if (word_list.size() > 5000) {
			for (auto itr = word_list.begin() + 2000; itr != word_list.end(); ++itr) {
				file_author.seekg(0, ios::beg);
				string buffer; // to dump the first line into, for converting string to char, and rewriting file
				getline(file_author, buffer);
				string word_existing;
				bool wordExists = false;
				while (getline(file_author, word_existing, ',')) {
					if (itr->text == word_existing) {
						wordExists = true;
						int freq_new = std::stoi(buffer) + itr->freq;
						update_word_freq(file_author, filename_author, word_existing, freq_new);
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
			output += input[i] - ('A'-'a'); // this trick works because ASCII :P
		} else if (	input[i] == 'Á' ||
					input[i] == 'É' ||
					input[i] == 'Í' ||
					input[i] == 'Ó' ||
					input[i] == 'Ú' ||
					input[i] == 'Ñ'		) {
			// otherwise we must assign each character individually.
			// Note: these are inside a single "else if" clause to optimize
			// evaluation; doing this *should* reduce the number of branches.
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

void print_data_size(ifstream& stream)
{
	// First, get file size in bytes:
	stream.seekg(0, ios::end);
	float text_size_kb = stream.tellg() / 1024.0f;
	stream.seekg(0, ios::beg);
	cout << "Size: " << text_size_kb << " kilobytes" << endl << endl;

	// Translate bytes -> words: Wolfram Alpha claims an avg. 5.1 char / word.
	// If the word estimate is greater than 10000 (MAGIC_NUM!), round to the nearest thousand.
	int word_num = static_cast<int>(round(text_size_kb / 5.1 * 1000));
	if (word_num > 10000) {
		word_num = static_cast<int>(round(word_num / 1000.0f)) * 1000;
	}
	cout << "(Approximately " << word_num << " words.)" << endl << endl;
}

void update_word(vector<Word>& list, string word)
{
	// If the word is already in memory, increment its count.
	// If it isn't, add the word to the end of the list (with a count of 1).
	bool doesExist = false;
	for (auto itr = list.begin(); itr != list.end(); ++itr) {
		if (word == itr->text) {
			(itr->freq)++;
			doesExist = true;
			break;
		}
	}
	if (!doesExist) {
		list.push_back(Word(word, 1));
	}
}

void add_words_from_line(vector<Word>& list, string line, std::regex regex)
{
	std::sregex_iterator find(line.begin(), line.end(), regex);
	for (std::sregex_iterator find_end; find != find_end; find++) {
		std::smatch match_word = *find;
		string word_found = match_word.str();
		word_found = to_lower_case(word_found);
		update_word(list, word_found);
	}
}

void update_word_freq(fstream& stream, string filename, string word, int freq)
{
	// To replace a line we must write an entirely new file,
	// then rename it and replace the old one.
	string filename_temp = "Authors/tmp.csv";
	ofstream file_updated(filename_temp);
	stream.seekg(0, ios::beg);

	string buffer; // swap buffer
	while (getline(stream, buffer, ',')) {
		file_updated << buffer << ",";
		// Now write the correct freq:
		if (buffer != word) {
			getline(stream, buffer); // continues getline until '\n', i.e. gets freq
			file_updated << buffer;
		} else {
			file_updated << freq;
		}
		file_updated << endl;
	}
	file_updated.close();

	// Delete old file, rename new one, and open fstream for I/O again.
	remove(filename.c_str());
	rename(filename_temp.c_str(), filename.c_str());
	stream.open(filename, ios::in | ios::out);
}
