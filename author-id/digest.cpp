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
	string filename_words = create_filename("Digests/", filename, "-WORD.csv");
	string filename_sentences = create_filename("Digests/", filename, "-SENT.csv");
	cout << "Author: " << author_name << endl << endl;
	cout << "Output: " << filename + "-SUM.txt" << endl <<
		"        " << filename + "-WORD.csv" << endl <<
		"        " << filename + "-SENT.csv" << endl << endl;

	Memory RAM;
	string raw_input;
	cout << "Parsing text";
	for (int i = 0; getline(file_text, raw_input); ++i) {
		add_data_from_line(RAM, raw_input);
		// TODO: Make constants settable via command-line options (i.e. 10000, 3000)
		if (RAM.word_list.size() > 10000) {
			cout << "Flushing buffer..." << endl << endl;
			combine_list_file(	RAM.word_list,
								filename_words,
								RAM.word_list.begin() + 3000,
								RAM.word_list.end()	);
		}
		if (i % 1000 == 0) {
			int cycle = (i / 1000) % 4;
			switch (cycle) {
				case 0 :
				case 1 :
				case 2 :
					cout << ".";
					break;
				case 3 :
					cout << '\b' << '\b' << '\b';
					cout << "   "; // 3 spaces
					cout << '\b' << '\b' << '\b';
					break;
			}
		}
	}
	cout << endl << endl;

	// Final write:
	cout << "Writing frequency files..." << endl << endl;
	std::sort(RAM.word_list.begin(), RAM.word_list.end(), word_compare());
	combine_list_file(RAM, filename_words);
	get_list_from_file(RAM, filename_words);
	ofstream file_sentences(filename_sentences);
	file_sentences << "WORDS" << endl;
	for (unsigned int i = 0; i < RAM.sentence_len.size(); ++i) {
		file_sentences << RAM.sentence_len[i] << endl;
	}
	file_sentences.close();
	cout << "Writing summary file..." << endl << endl;
	write_summary(filename_summary, &RAM, author_name);

	cout << endl << "Done!" << endl;
}



bool does_word_match_list(string word, vector<string> list) {
	bool does_match = false;
	for (unsigned int i = 0; i < list.size(); ++i) {
		if (word == list[i]) {
			does_match = true;
			break;
		}
	}
	return does_match;
}

void write_summary(string filename, Memory* mem, string author_name)
{
	// Word length mu and sigma
	int word_list_size = mem->word_list.size();
	double word_count = 0;
	double total_letters = 0;
	for (int i = 0; i < word_list_size; ++i) {
		int word_freq = mem->word_list[i].freq;
		word_count += word_freq;
		int word_len = mem->word_list[i].text.size();
		total_letters += static_cast<double>(word_len)* word_freq;
	}
	double word_len_avg = total_letters / word_count;
	double word_total_error = 0;
	for (int i = 0; i < word_list_size; ++i) {
		double error = mem->word_list[i].text.size() - word_len_avg;
		word_total_error += error * error * mem->word_list[i].freq;
	}
	double word_len_dev = sqrt(word_total_error / word_count);

	// Sentence length mu and sigma
	int sentence_count = mem->sentence_len.size();
	double sentence_total_words = 0;
	for (int i = 0; i < sentence_count; ++i) {
		sentence_total_words += mem->sentence_len[i];
	}
	double sentence_len_avg = sentence_total_words / static_cast<double>(sentence_count);
	double sentence_total_error = 0;
	for (int i = 0; i < sentence_count; ++i) {
		double error = mem->sentence_len[i] - sentence_len_avg;
		sentence_total_error += error * error;
	}
	double sentence_len_dev = sqrt(sentence_total_error/sentence_count);

	// Sentence uniformity
	double sentence_diff_total = 0;
	for (int i = 1; i < sentence_count; ++i) {
		int diff = mem->sentence_len[i] - mem->sentence_len[i - 1];
		sentence_diff_total += diff * diff / static_cast<double>(sentence_count);
	}
	double sentence_diff_rms = sqrt(sentence_diff_total);

	// Contraction ratio
	int freq_cntrct = 0;
	vector<string> words_cntrct{	"ain’t",	"won’t",	"let’s",	"ain’t",
									"can’t",	"couldn’t",	"shouldn’t","wouldn’t",
									"haven’t",	"hasn’t",	"hadn’t",	"aren’t",
									"could’ve",	"should’ve","would’ve",	"might’ve",
									"he’d",		"she’d",	"i’d",		"they’d",
									"it’d",		"you’d",	"we’d",		"i’ll",
									"you’ll",	"she’ll",	"he’ll",	"they’ll",
									"it’ll",	"we’ll",	"i’ve",		"you’ve",
									"it’s",		"that’ll",	"there’ll",	"what’ll",
									"don’t",	"doesn’t",	"didn’t",	"wasn’t",
									"we’re",	"they’re",	"’twas",	"’ere",
									"e’er",		"ne’er",	"o’er",		"who’ll",
									"hadn’t",	"hasn’t",	"haven’t",	"i’m",
									"ya’ll"		};
	for (int i = 0; i < word_list_size; ++i) {
		string word_read = mem->word_list[i].text;
		if (does_word_match_list(word_read, words_cntrct)) {
			freq_cntrct += mem->word_list[i].freq;
		}
	}
	double cntrct_ratio = static_cast<double>(freq_cntrct) / word_count;

	// Gender ratio (M-F)
	int freq_M = 0;
	int freq_F = 0;
	vector<string> words_M{ "he", "he’d", "he’ll", "he’s", "him", "himself", "his" };
	vector<string> words_F{ "she", "she’d", "she’ll", "she’s", "her", "herself", "hers" };
	for (int i = 0; i < word_list_size; ++i) {
		string word_read = mem->word_list[i].text;
		if (does_word_match_list(word_read, words_M)) {
			freq_M += mem->word_list[i].freq;
		}
		if (does_word_match_list(word_read, words_F)) {
			freq_F += mem->word_list[i].freq;
		}
	}
	double M_F_ratio = static_cast<double>(freq_M) / static_cast<double>(freq_F);

	// Negation ratio
	int freq_neg = 0;
	vector<string> words_neg {	"no",		"not",		"never",	"nothing",
								"none",		"nobody",	"nowhere",	"neither",
								"nor",		"cannot",	"ain’t",	"aren’t",
								"can’t",	"couldn’t",	"didn’t",	"doesn’t",
								"don’t",	"hadn’t",	"hasn’t",	"haven’t",
								"isn’t",	"shouldn’t","wasn’t",	"weren’t",
								"wouldn’t"	};
	for (int i = 0; i < word_list_size; ++i) {
		string word_read = mem->word_list[i].text;
		if (does_word_match_list(word_read, words_neg)) {
			freq_neg += mem->word_list[i].freq;
		}
	}
	double neg_ratio = static_cast<double>(freq_neg) / word_count;

	// Modal verb ratio
	int freq_modal = 0;
	vector<string> words_modal{ "can",		"can’t",	"could",	"couldn’t",
								"cannot",	"shall",	"should",	"shouldn’t",
								"will",		"would",	"wouldn’t",	"i’ll",
								"he’ll",	"she’ll",	"you’ll",	"we’ll",
								"it’ll",	"they’ll",	"that’ll"	};
	for (int i = 0; i < word_list_size; ++i) {
		string word_read = mem->word_list[i].text;
		if (does_word_match_list(word_read, words_modal)) {
			freq_modal += mem->word_list[i].freq;
		}
	}
	double modal_ratio = static_cast<double>(freq_modal) / word_count;

	// PoV freq
	int freq_1st = 0;
	int freq_2nd = 0;
	int freq_3rd = 0;
	vector<string> words_1st{	"i",	"me",	"my",	"myself",	"mine",
								"we",	"us",	"our",	"ourselves","ours",
								"i’ll",	"i’m",	"i’d",	"we’ll",	"we’d",
								"i’ve",	"we’ve"	};
	vector<string> words_2nd{	"you",		"your",		"yourself",	"yours",
								"you’ll",	"you’d",	"ya’ll",	"you’ve"	};
	vector<string> words_3rd{	"he",		"him",		"his",		"himself",
								"she",		"her",		"hers",		"herself",
								"he’ll",	"he’d",		"they’ll",	"they’d",
								"she’ll",	"she’d",	"it’ll",	"it’d",
								"it",		"its",		"itself",	"themselves",
								"they",		"them",		"their",	"theirs"	};
	for (int i = 0; i < word_list_size; ++i) {
		string word_read = mem->word_list[i].text;
		if (does_word_match_list(word_read, words_1st)) {
			freq_1st += mem->word_list[i].freq;
		}
		if (does_word_match_list(word_read, words_2nd)) {
			freq_2nd += mem->word_list[i].freq;
		}
		if (does_word_match_list(word_read, words_3rd)) {
			freq_3rd += mem->word_list[i].freq;
		}
	}

	// Article ratio (def-indef)
	int freq_indef = 0;
	int freq_def = 0;
	for (int i = 0; i < word_count; ++i) {
		string word_read = mem->word_list[i].text;
		if (word_read == "the") {
			freq_indef = mem->word_list[i].freq;
		} else if (word_read == "a") {
			freq_def = mem->word_list[i].freq;
		}
		if (freq_def * freq_indef != 0) {
			break;
		}
	}
	float article_ratio = static_cast<float>(freq_def) / static_cast<float>(freq_indef);

	ofstream file_summary(filename);
	file_summary << "Author: " << author_name << endl << endl;
	file_summary << "words (N):\t\t\t" << word_count << endl;
	file_summary << "word len, µ:\t\t" << word_len_avg << endl;
	file_summary << "word len, s:\t\t" << word_len_dev << endl << endl;
	file_summary << "sentences (N):\t\t" << sentence_count << endl;
	file_summary << "sentence len, µ:\t" << sentence_len_avg << endl;
	file_summary << "sentence len, s:\t" << sentence_len_dev << endl << endl;
	file_summary << "sentence uniformity, rms:\t" << sentence_diff_rms << endl << endl;
	file_summary << "contraction ratio:\t\t" << cntrct_ratio << endl << endl;
	file_summary << "gender ratio (M-F):\t\t" << M_F_ratio << endl << endl;
	file_summary << "gender count (N=M+F):\t" << freq_M + freq_F << endl << endl;
	file_summary << "negation ratio:\t\t\t" << neg_ratio << endl << endl;
	file_summary << "modal verb ratio:\t\t" << modal_ratio << endl << endl;
	file_summary << "article ratio (def-indef):\t" << article_ratio << endl << endl;
	file_summary << "PoV freq" << endl;
	file_summary << "1st person:\t" << freq_1st << endl;
	file_summary << "2nd person:\t" << freq_2nd << endl;
	file_summary << "3rd person:\t" << freq_3rd << endl << endl;
	file_summary << "punctuation freq table" << endl;
	file_summary << "[.] " << mem->punc_freq[PUNC_PERIOD] << endl;
	file_summary << "[,] " << mem->punc_freq[PUNC_COMMA] << endl;
	file_summary << "[;] " << mem->punc_freq[PUNC_SEMICOLON] << endl;
	file_summary << "[:] " << mem->punc_freq[PUNC_COLON] << endl;
	file_summary << "[!] " << mem->punc_freq[PUNC_EXCLAMATION] << endl;
	file_summary << "[?] " << mem->punc_freq[PUNC_QUESTION] << endl;
	file_summary << "[—] " << mem->punc_freq[PUNC_DASH] << endl;
	file_summary << "[…] " << mem->punc_freq[PUNC_ELLIPSIS] << endl;
	file_summary.close();
}

bool file_check_create(string filename)
{
	bool didExist = false;
	ifstream f_check(filename);
	didExist = f_check.good();
	f_check.close();
	if (didExist) {
		remove(filename.c_str());
	}
	ofstream f_create(filename);
	f_create << endl;
	f_create.close();
	return didExist;
}

string create_filename(string prepend, string main, string append)
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

int count_words(string input)
{
	regex regex_word("([a-zA-záéíóúñÁÉÍÓÚÑ'’]+)");
	sregex_iterator find_word(input.begin(), input.end(), regex_word);
	int count = std::distance(find_word, sregex_iterator());
	return count;
}

void add_data_from_line(Memory& mem, string& line)
{
	regex regex_apostrophe("'");
	std::regex_replace(line, regex_apostrophe, "’");
	regex regex_word("([a-zA-záéíóúñÁÉÍÓÚÑ'’]+)");
	sregex_iterator find_word(line.begin(), line.end(), regex_word);
	for (sregex_iterator find_end; find_word != find_end; ++find_word) {
		string word_found = find_word->str();
		word_found = to_lower_case(word_found);
		update_word(mem.word_list, word_found);
	}
	vector<regex> regex_punc = {
		regex("\\."),
		regex(","),
		regex(";"),
		regex(":"),
		regex("!"),
		regex("\\?"),
		regex("(?:[-–—]\\s*){1,2}"),
		regex("…|(?:\\.\\s*){3}")
	};
	for (int i = 0; i < PUNC_NUM; ++i) {
		sregex_iterator find_punc(line.begin(), line.end(), regex_punc[i]);
		mem.punc_freq[i] += std::distance(find_punc, sregex_iterator());
	}
	regex regex_split_line("^([^.!?…]*[.!?…])?((?:[^.!?…]*[.!?…])*?)([^.!?…]*)$");
	std::sregex_token_iterator split_line_prev(line.begin(), line.end(), regex_split_line, 1);
	std::sregex_token_iterator split_line_cont(line.begin(), line.end(), regex_split_line, 2);
	std::sregex_token_iterator split_line_xtra(line.begin(), line.end(), regex_split_line, 3);
	string sentence_prev = *split_line_prev;
	int sentence_prev_len = count_words(sentence_prev);
	if (sentence_prev_len > 0) {
		sentence_prev_len += mem.sentence_carry;
		mem.sentence_len.push_back(sentence_prev_len);
		mem.sentence_carry = 0;
	}
	string sentence_cont = *split_line_cont;
	regex regex_split_sentence("([^\\.!?…]*)[\\.!?…]");
	sregex_iterator find_sentence(sentence_cont.begin(), sentence_cont.end(), regex_split_sentence);
	for (sregex_iterator find_end; find_sentence != find_end; ++find_sentence) {
		string sentence_found = find_sentence->str();
		int sentence_len = count_words(sentence_found);
		mem.sentence_len.push_back(sentence_len);
		mem.sentence_carry = 0;
	}
	string sentence_xtra = *split_line_xtra;
	int sentence_xtra_len = count_words(sentence_xtra);
	if (sentence_xtra_len > 0) {
		mem.sentence_carry += sentence_xtra_len;
	}
}

void get_list_from_file(Memory& mem, string filename)
{
	mem.word_list.clear();
	ifstream file_read(filename);
	string buffer;
	getline(file_read, buffer);
	while (getline(file_read, buffer)) {
		if (buffer.length() > 0) {
			stringstream converter(buffer);
			string word_read;
			getline(converter, word_read, ',');
			getline(converter, buffer);
			int freq_read = std::stoi(buffer);
			mem.word_list.push_back(Word(word_read, freq_read));
		}
	}
	file_read.close();
}

void combine_list_file(Memory& mem, string filename)
{
	combine_list_file(mem.word_list, filename, mem.word_list.begin(), mem.word_list.end());
}

void combine_list_file(	vector<Word>& list,
						string filename,
						vector<Word>::iterator itr_beg,
						vector<Word>::iterator itr_end)
{
	string filename_temp = "Digests/temp.dat";
	ifstream file_read(filename);
	vector<Word> list_read;

	string buffer;
	getline(file_read, buffer);
	while (getline(file_read, buffer)) {
		if (buffer.length() > 0) {
			stringstream converter(buffer);
			string word_read;
			getline(converter, word_read, ',');
			getline(converter, buffer);
			int freq_read = std::stoi(buffer);
			list_read.push_back(Word(word_read, freq_read));
		}
	}
	file_read.close();

	for (auto itr_list = itr_beg; itr_list != itr_end; ++itr_list) {
		bool wordExists = false;
		for (auto itr_file = list_read.begin(); itr_file != list_read.end(); ++itr_file) {
			if (itr_list->text == itr_file->text) {
				itr_file->freq += itr_list->freq;
				wordExists = true;
				break;
			}
		}
		if (!wordExists) {
			list_read.push_back(Word(itr_list->text, itr_list->freq));
		}
	}
	list.erase(itr_beg, itr_end);

	std::sort(list_read.begin(), list_read.end(), word_compare());
	ofstream file_updated(filename_temp);
	file_updated << "WORD,FREQ" << endl;
	for (auto itr = list_read.begin(); itr != list_read.end(); ++itr) {
		file_updated << itr->text << "," << itr->freq << endl;
	}
	file_updated.close();

	remove(filename.c_str());
	rename(filename_temp.c_str(), filename.c_str());
}
