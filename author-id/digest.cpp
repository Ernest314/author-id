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
	string filename_summary = create_file_and_name("Digests/", filename, "-SUM.txt");
	string filename_words = create_file_and_name("Digests/", filename, "-WORD.csv");
	string filename_sentences = create_file_and_name("Digests/", filename, "-SENT.csv");
	cout << "Author: " << author_name << endl << endl;
	cout << "Output: " << filename + "-SUM.txt" << endl <<
		"        " << filename + "-WORD.csv" << endl <<
		"        " << filename + "-SENT.csv" << endl << endl;

	Memory RAM;
	string raw_input;
	cout << "Parsing text: |";
	const int bar_width = 50;
	for (int i = 0; i<bar_width; ++i) { cout << " "; }
	cout << "|  0.00%";
	for (int i = 0; getline(file_text, raw_input); ++i) {
		add_data_from_line(RAM, raw_input);
		// TODO: Make constants settable via command-line options (i.e. 10000, 3000)
		if (RAM.word_list.size() > 10000) {
			cout << endl << endl << "Flushing buffer..." << endl << endl;
			combine_list_file(	RAM.word_list,
								filename_words,
								RAM.word_list.begin() + 3000,
								RAM.word_list.end()	);
			cout << "Parsing text: |";
			for (int i = 0; i<bar_width; ++i) { cout << " "; }
			cout << "|  0.00%";
		}
		// Progress bar stuff:
		if (i % 120 == 0) {
			cout << "\b\b\b\b\b\b\b\b"; // "| ##.##%"
			for (int i = 0; i < bar_width; ++i) { cout << "\b"; }
			ifstream file_sizer(filename_input);
			file_sizer.seekg(0, ios::end);
			float size = static_cast<float>(file_sizer.tellg());
			file_sizer.close();
			float current = static_cast<float>(file_text.tellg());
			float percentage = current / size * 100;
			int chars_filled = static_cast<int>(floor(percentage/100.0*bar_width));
			for (int i = 0; i < bar_width; ++i) {
				if (i < chars_filled) {
					cout << "#";
				} else {
					cout << " ";
				}
			}
			std::streamsize precision_init = cout.precision();
			int correct_precision = get_precision(5, percentage);
			cout.precision(correct_precision);
			std::streamsize width_init = cout.width();
			cout << "| " << std::setw(5) << percentage << "%";
			cout.precision(precision_init);
			cout << std::setw(width_init);
		}
	}
	cout << "\b\b\b\b\b\b\b\b"; // "| ##.##%"
	for (int i = 0; i < bar_width; ++i) { cout << "\b"; }
	for (int i = 0; i < bar_width; ++i) { cout << "#"; }
	cout << "| -DONE-" << endl << endl;

	// Final write:
	cout << "Writing frequency files..." << endl << endl;
	std::sort(RAM.word_list.begin(), RAM.word_list.end(), word_compare());
	combine_list_file(RAM.word_list, filename_words);
	get_list_from_file(RAM.word_list, filename_words);
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
	vector<string> words_cntrct{	"ain�t",	"won�t",	"let�s",	"ain�t",
									"can�t",	"couldn�t",	"shouldn�t","wouldn�t",
									"haven�t",	"hasn�t",	"hadn�t",	"aren�t",
									"could�ve",	"should�ve","would�ve",	"might�ve",
									"he�d",		"she�d",	"i�d",		"they�d",
									"it�d",		"you�d",	"we�d",		"i�ll",
									"you�ll",	"she�ll",	"he�ll",	"they�ll",
									"it�ll",	"we�ll",	"i�ve",		"you�ve",
									"it�s",		"that�ll",	"there�ll",	"what�ll",
									"don�t",	"doesn�t",	"didn�t",	"wasn�t",
									"we�re",	"they�re",	"�twas",	"�ere",
									"e�er",		"ne�er",	"o�er",		"who�ll",
									"hadn�t",	"hasn�t",	"haven�t",	"i�m",
									"ya�ll"		};
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
	vector<string> words_M{ "he", "he�d", "he�ll", "he�s", "him", "himself", "his" };
	vector<string> words_F{ "she", "she�d", "she�ll", "she�s", "her", "herself", "hers" };
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
								"nor",		"cannot",	"ain�t",	"aren�t",
								"can�t",	"couldn�t",	"didn�t",	"doesn�t",
								"don�t",	"hadn�t",	"hasn�t",	"haven�t",
								"isn�t",	"shouldn�t","wasn�t",	"weren�t",
								"wouldn�t"	};
	for (int i = 0; i < word_list_size; ++i) {
		string word_read = mem->word_list[i].text;
		if (does_word_match_list(word_read, words_neg)) {
			freq_neg += mem->word_list[i].freq;
		}
	}
	double neg_ratio = static_cast<double>(freq_neg) / word_count;

	// Modal verb ratio
	int freq_modal = 0;
	vector<string> words_modal{ "can",		"can�t",	"could",	"couldn�t",
								"cannot",	"shall",	"should",	"shouldn�t",
								"will",		"would",	"wouldn�t",	"i�ll",
								"he�ll",	"she�ll",	"you�ll",	"we�ll",
								"it�ll",	"they�ll",	"that�ll"	};
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
								"i�ll",	"i�m",	"i�d",	"we�ll",	"we�d",
								"i�ve",	"we�ve"	};
	vector<string> words_2nd{	"you",		"your",		"yourself",	"yours",
								"you�ll",	"you�d",	"ya�ll",	"you�ve"	};
	vector<string> words_3rd{	"he",		"him",		"his",		"himself",
								"she",		"her",		"hers",		"herself",
								"he�ll",	"he�d",		"they�ll",	"they�d",
								"she�ll",	"she�d",	"it�ll",	"it�d",
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

	remove(filename.c_str());
	ofstream file_summary(filename);
	file_summary << "Author: " << author_name << endl << endl;
	file_summary << "words (N):\t\t\t" << word_count << endl;
	file_summary << "word len, �:\t\t" << word_len_avg << endl;
	file_summary << "word len, s:\t\t" << word_len_dev << endl << endl;
	file_summary << "sentences (N):\t\t" << sentence_count << endl;
	file_summary << "sentence len, �:\t" << sentence_len_avg << endl;
	file_summary << "sentence len, s:\t" << sentence_len_dev << endl << endl;
	file_summary << "sentence uniformity, rms:\t" << sentence_diff_rms << endl << endl;
	file_summary << "contraction ratio:\t\t" << cntrct_ratio << endl << endl;
	file_summary << "gender ratio (M-F):\t\t" << M_F_ratio << endl;
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
	file_summary << "[�] " << mem->punc_freq[PUNC_DASH] << endl;
	file_summary << "[�] " << mem->punc_freq[PUNC_ELLIPSIS] << endl;
	file_summary.close();
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

void add_data_from_line(Memory& mem, string& line)
{
	regex regex_apostrophe("'");
	std::regex_replace(line, regex_apostrophe, "�");
	regex regex_word("([a-zA-z������������'�]+)");
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
		regex("(?:[-��]\\s*){1,2}"),
		regex("�|(?:\\.\\s*){3}")
	};
	for (int i = 0; i < PUNC_NUM; ++i) {
		sregex_iterator find_punc(line.begin(), line.end(), regex_punc[i]);
		mem.punc_freq[i] += std::distance(find_punc, sregex_iterator());
	}
	regex regex_split_line("^([^.!?�]*[.!?�])?((?:[^.!?�]*[.!?�])*?)([^.!?�]*)$");
	std::sregex_token_iterator split_line_prev(line.begin(), line.end(), regex_split_line, 1);
	std::sregex_token_iterator split_line_cont(line.begin(), line.end(), regex_split_line, 2);
	std::sregex_token_iterator split_line_xtra(line.begin(), line.end(), regex_split_line, 3);
	string sentence_prev = *split_line_prev;
	int sentence_prev_len = get_word_count(sentence_prev);
	if (sentence_prev_len > 0) {
		sentence_prev_len += mem.sentence_carry;
		mem.sentence_len.push_back(sentence_prev_len);
		mem.sentence_carry = 0;
	}
	string sentence_cont = *split_line_cont;
	regex regex_split_sentence("([^\\.!?�]*)[\\.!?�]");
	sregex_iterator find_sentence(sentence_cont.begin(), sentence_cont.end(), regex_split_sentence);
	for (sregex_iterator find_end; find_sentence != find_end; ++find_sentence) {
		string sentence_found = find_sentence->str();
		int sentence_len = get_word_count(sentence_found);
		mem.sentence_len.push_back(sentence_len);
		mem.sentence_carry = 0;
	}
	string sentence_xtra = *split_line_xtra;
	int sentence_xtra_len = get_word_count(sentence_xtra);
	if (sentence_xtra_len > 0) {
		mem.sentence_carry += sentence_xtra_len;
	}
}
