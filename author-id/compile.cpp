#include "stdafx.h"
#include "compile.h"

void compile_input(string filename)
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
	string filename_author_sum = "Authors/" + author_name + "-SUM.txt";
	string filename_author_word = "Authors/" + author_name + "-WORD.csv";

	cout << "Into:" << endl <<
		"\t" << filename_author_sum << endl <<
		"\t" << filename_author_word << endl << endl;

	ifstream file_author_sum(filename_author_sum);
	read_author(file_author_sum);

	string filename_temp = "temp.txt";
	ofstream file_new(filename_temp);
	file_new << "Author: " << author_name << endl << endl;

	int word_N = read_word_N(file_sum);
	int word_N_author = read_word_N(file_author_sum);
	int word_N_new = word_N + word_N_author;
	file_new << "words (N):\t\t\t" << word_N_new << endl;

	double word_u = read_word_u(file_sum);
	double word_u_author = read_word_u(file_author_sum);
	double word_u_new = static_cast<double>(word_u * word_N + word_u_author * word_N_author) /
		static_cast<double>(word_N_new);
	file_new << "word len, µ:\t\t" << word_u_new << endl;

	double word_s = read_word_s(file_sum);
	double word_s_author = read_word_s(file_author_sum);
	double word_v = word_s * word_s;
	double word_v_author = word_s_author * word_s_author;
	double word_v_new = static_cast<double>(word_v * word_N + word_v_author * word_N_author);
	double word_s_new = sqrt(word_v_new);
	file_new << "word len, s:\t\t" << word_s_new << endl;
	
	file_sum.close();
	file_author_sum.close();
	file_new.close();
}

string strip_colon(ifstream& stream, int leading_chars)
{
	string buffer;
	getline(stream, buffer, ':');
	getline(stream, buffer);
	return string(buffer.begin() + leading_chars, buffer.end());
}

string read_author(ifstream& stream)
{
	string buffer;
	getline(stream, buffer, ':'); // discard  "Author:"
	getline(stream, buffer);
	return string(buffer.begin() + 1, buffer.end()); // discard leading " "
}

int read_word_N(ifstream& stream)
{
	string buffer;
	getline(stream, buffer); // blank line
	string raw = strip_colon(stream, 3); // discard "\t\t\t"
	return std::atoi(raw.c_str());
}

double read_word_u(ifstream& stream)
{
	string buffer;
	getline(stream, buffer, ':'); // discard "word len, µ:"
	string raw = strip_colon(stream, 2); // discard "\t\t"
	return std::atof(raw.c_str());
}

double read_word_s(ifstream& stream)
{
	return read_word_u(stream);
}
