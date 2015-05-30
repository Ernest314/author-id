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
	string filename_summary = create_filename("Digests/", filename, "-SUM.txt");
	fstream file_summary(filename_summary, ios::in | ios::out);
	string filename_words = create_filename("Digests/", filename, "-WORD.csv");
	fstream file_words(filename_words, ios::in | ios::out);
	string filename_sentences = create_filename("Digests/", filename, "-SENT.csv");
	fstream file_sentences(filename_sentences, ios::in | ios::out);
	cout << "Author: " << author_name << endl << endl;
	cout << "Output: " << filename + "-SUM.txt" << endl <<
		"        " << filename + "-WORD.csv" << endl <<
		"        " << filename + "-SENT.csv" << endl << endl;

	Memory RAM;
	string raw_input;
	while (getline(file_text, raw_input)) {
		add_data_from_line(RAM, raw_input);
		// TODO: Make constants settable via command-line options (i.e. 10000, 3000)
		if (word_list.size() > 10000) {
			cout << "Flushing buffer..." << endl << endl;
			combine_list_file(	word_list,
								file_words,
								filename_author,
								word_list.begin() + 3000,
								word_list.end()		);
		}
	}

	// Final write:
	cout << "Writing frequency file..." << endl << endl;
	std::sort(word_list.begin(), word_list.end(), word_compare());
	combine_list_file(word_list, file_words, filename_author);
	file_words.close();

	// TODO: Sort

	cout << endl << "Done!" << endl;
}



bool file_check_create(string filename)
{
	bool didExist = false;
	ifstream f_check(filename);
	didExist = f_check.good();
	f_check.close();
	if (!didExist) {
		ofstream f_create(filename);
		f_create << endl;
		f_create.close();
	}
	return didExist;
}

string create_file(string prepend, string main, string append)
{
	string output(main.begin(), main.end() - 4);
	output = prepend + output + append;
	file_check_create(output);
	return output;
}

string to_lower_case(string input)
{
	string output;
	for (unsigned int i = 0; i < input.length(); i++) {
		if (input[i] >= 'A' && input[i] <= 'Z') {
			output += input[i] - ('A'-'a'); // this trick works because ASCII :P
		} else if (	input[i] == '¡' ||
					input[i] == '…' ||
					input[i] == 'Õ' ||
					input[i] == '”' ||
					input[i] == '⁄' ||
					input[i] == '—'		) {
			// otherwise we must assign each character individually.
			// Note: these are inside a single "else if" clause to optimize
			// evaluation; doing this *should* reduce the number of branches.
			switch (input[i]) {
				case '¡' :
					output = '·';
					break;
				case '…':
					output = 'È';
					break;
				case 'Õ':
					output = 'Ì';
					break;
				case '”':
					output = 'Û';
					break;
				case '⁄':
					output = '˙';
					break;
				case '—':
					output = 'Ò';
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

void add_word_to_file(string filename, string word, int freq)
{
	ofstream file_updated(filename, ios::app);
	file_updated << word << "," << freq << endl;
	file_updated.close();
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

void add_data_from_line(Memory& mem, string line)
{
	regex regex_word("([a-zA-z·ÈÌÛ˙Ò¡…Õ”⁄—'í]+)");
	vector<regex> regex_punc = {
		regex("(.)"),
		regex("(,)"),
		regex("(;)"),
		regex("(:)"),
		regex("(!)"),
		regex("(?)"),
		regex("((?:[-ñó]\s*){1,2})"),
		regex("(Ö|(?:\.\s*){3})")
	};
	regex regex_end_sentence("(?:([^\.!?Ö]+)(?:(?:\.\s*){3}|[\.!?Ö]))*([^\.!?Ö]+)\n");
	std::sregex_iterator find_word(line.begin(), line.end(), regex_word);
	for (std::sregex_iterator find_end; find_word != find_end; find_word++) {
		std::smatch match_word = *find_word;
		string word_found = match_word.str();
		word_found = to_lower_case(word_found);
		update_word(mem.word_list, word_found);
	}
	for (int i = 0; i < PUNC_NUM; ++i) {
		std::sregex_iterator find_punc(line.begin(), line.end(), regex_punc[i]);
		mem.punc_freq[i] += std::distance(find_punc, std::sregex_iterator());
	}
	std::sregex_iterator find_sentence(line.begin(), line.end(), regex_end_sentence);
	for (std::sregex_iterator find_end; find_sentence != find_end; find_sentence++) {
		std::smatch match_sentence = *find_sentence;
		string sentence_found = match_sentence.str();
		std::sregex_iterator count_words(sentence_found.begin(), sentence_found.end(), regex_word);
		mem. += std::distance(find_sentence, std::sregex_iterator());
	}
}

void update_word_freq(string filename, string word, int freq)
{
	// To replace a line we must write an entirely new file,
	// then rename it and replace the old one.
	string filename_temp = "Authors/tmp.csv";
	ofstream file_updated(filename_temp);
	ifstream file_read(filename);

	string buffer; // swap buffer
	getline(file_read, buffer); // First line should be "WORD,FREQ"
	file_updated << buffer << endl;
	while (getline(file_read, buffer, ',')) {
		file_updated << buffer << ",";
		getline(file_read, buffer); // continues getline until '\n', i.e. gets freq
		// Now write the correct freq:
		if (buffer != word) {
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
}

void combine_list_file(vector<Word>& list, fstream& stream, string filename)
{
	combine_list_file(list, stream, filename, list.begin(), list.end());
}

void combine_list_file(	vector<Word>& list,
						fstream& stream,
						string filename,
						vector<Word>::iterator itr_beg,
						vector<Word>::iterator itr_end)
{
	std::sort(list.begin(), list.end(), word_compare());
	for (auto itr = itr_beg; itr != itr_end; ++itr) {
		stream.seekg(0, ios::beg);
		string buffer; // to dump the first line into, for converting string to char, and rewriting file
		getline(stream, buffer);

		bool wordExists = false;
		string word_existing;
		while (getline(stream, word_existing, ',')) {
			getline(stream, buffer);
			if (itr->text == word_existing) {
				wordExists = true;
				int freq_new = std::stoi(buffer) + itr->freq;
				update_word_freq(filename, word_existing, freq_new);
				break;
			}
		}
		if (!wordExists) {
			stream.close();
			add_word_to_file(filename, itr->text, itr->freq);
			stream.open(filename, ios::in | ios::out);
		}
	}
	list.erase(itr_beg, itr_end);
}
