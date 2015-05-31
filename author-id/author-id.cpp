#include "stdafx.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "digest.h"
#include "compile.h"

using std::cout;
using std::endl;
using std::cin;
using std::string;

enum RunMode {
	ERROR_UNKNOWN	= -1,
	ERROR_NO_ARG	= -2,
	ERROR_BAD_ARG	= -3,
	ERROR_NO_PARAM	= -4,
	ERROR_BAD_PARAM	= -5,
	ERROR_NOT_TXT	= -6,

	MODE_ABOUT	= 0,
	MODE_HELP,
	MODE_LIST,
	MODE_DIGEST,
	MODE_COMPILE,
	MODE_NUM
};

RunMode hash_string(string input);

int main(int argc, char* argv[])
{
	cout << endl;
	cout << "==== AUTHOR-ID PROGRAM: Ernest Gu & Nathan Yang ====" << endl << endl;
	if (argc < 2) {
		cout << "You must enter at least one (1) argument.\n" <<
			"Type \"-h\" for a list of accepted arguments.\n\n";
		// WARNING: SKIPS TO END
		return 0;
	}

	// If there is more than one argument, continue parsing.
	// (But first, copy arguments into a vector--easier to work with.)
	std::vector<string> args;
	for (int i = 1; i < argc; i++) {
		// start @ i=1 because 0 holds the name of the .exe
		args.push_back(argv[i]);
	}

	// Loop through all the arguments supplied.
	while (args.size() > 0) {
		int erase_num = 1; // number of params per argument +1
		switch (hash_string(args[0])) {
			case ERROR_UNKNOWN :
				cout << "ERROR: Failed to parse command." << endl;
				break;
			case ERROR_NO_ARG :
				cout << "ERROR: No argument specified." <<
					" Type \"-h\" for a list of accepted arguments." << endl;
				break;
			case ERROR_BAD_ARG :
				cout << "ERROR: Argument not recognized." <<
					" Type \"-h\" for a list of accepted arguments." << endl;
				break;
			case ERROR_NO_PARAM :
				cout << "ERROR: No parameter(s) specified." <<
					" Type \"-h\" for a list of accepted arguments." << endl;
				break;
			case ERROR_BAD_PARAM :
				cout << "ERROR: Parameter(s) not recognized." <<
					" Type \"-h\" for a list of accepted arguments." << endl;
				break;
			case ERROR_NOT_TXT:
				cout << "ERROR: Parameter(s) is not a \".txt\" file." <<
					" Type \"-h\" for a list of accepted arguments." << endl;
				break;
			case MODE_ABOUT :
				cout << "This program is for our AP Statistics final project." << endl << endl <<
					"It creates statistics for a given author from a database of sample" << endl <<
					"texts, and will try to infer the author of an unknown text from" << endl <<
					"those pre-calculated statistics." << endl << endl;
				break;
			case MODE_HELP :
				cout << "Below is a list of accepted commands:" << endl << endl;
				cout << "-a, --about\n" <<
					"\tDescription of this program." << endl << endl;
				cout << "-?, -h, --help\n" <<
					"\tShows these help instructions." << endl << endl;
				cout << "-l, --list\n" <<
					"\tLists all authors for which there is data." << endl << endl;
				cout << "-d, --digest filename.txt\n" <<
					"\tDigests the specified file (in the \"Texts\" directory)." << endl << endl;
				cout << "-c, --compile file_A.txt [file_B.txt] [...]\n" <<
					"\tCompiles the listed files into their author's file. The files\n" <<
					"\tmust be in the \"Texts\" directory." << endl << endl;
				cout << endl;
				break;
			case MODE_LIST :
				cout << "All authors for which data has been collected:" << endl << endl;
				// NOTE: Enclosed within a block to prevent scoping issues with switch statement.
				{
					string filename_author_key = "Authors/KEY.txt";
					std::ifstream author_list(filename_author_key);
					string disp_author;
					while (getline(author_list, disp_author)) {
						cout << disp_author << endl;
					}
					cout << endl;
				}
				break;
			case MODE_DIGEST :
				erase_num = 2;
				// WARNING: If there aren't enough parameters, breaks out of switch immediately.
				// Also breaks out of switch immediately if filename extension is not ".txt".
				if (args.size() < 2) {
					// then size() must be 1, since it is also >0
					erase_num = 1;
					args.push_back("--NO_PARAM");
					break;
				} else {
					string filename_digest_input = args[1];
					string buf(	filename_digest_input,
								filename_digest_input.size() - 4,
								filename_digest_input.size()	);
					if (buf != ".txt") {
						erase_num = 1;
						args[1] = "--NOT_TXT";
						break;
					} else {
						digest_input(filename_digest_input);
					}
				}
				break;
			case MODE_COMPILE :
				erase_num = 2;
				// WARNING: If there aren't enough parameters, breaks out of switch immediately.
				// Also breaks out of switch immediately if filename extension is not ".txt".
				if (args.size() < 2) {
					// then size() must be 1, since it is also >0
					erase_num = 1;
					args.push_back("--NO_PARAM");
					break;
				} else {
					string filename_compile_input = args[1];
					string buf(filename_compile_input,
						filename_compile_input.size() - 4,
						filename_compile_input.size());
					if (buf != ".txt") {
						erase_num = 1;
						args[1] = "--NOT_TXT";
						break;
					} else {
						compile_input(filename_compile_input);
					}
				}
				break;
			default :
				break;
		}
		args.erase(args.begin(), args.begin() + erase_num);
	}

	cout << endl;
	return 0;
}

RunMode hash_string(string input)
{
	RunMode output = ERROR_UNKNOWN;
	int input_size = input.size();

	if (input_size == 0) {
		output = ERROR_NO_ARG;
	} else if (input_size == 2) {
		if (input[0] != '-') {
			output = ERROR_BAD_ARG;
		} else {
			switch (input[1]) {
				case 'a' :
					output = MODE_ABOUT;
					break;
				case '?' :
				case 'h' :
					output = MODE_HELP;
					break;
				case 'l' :
					output = MODE_LIST;
					break;
				case 'd':
					output = MODE_DIGEST;
					break;
				case 'c' :
					output = MODE_COMPILE;
					break;
				default :
					output = ERROR_BAD_ARG;
					break;
			}
		}
	} else {
		if (input[0] != '-' || input[1] != '-') {
			output = ERROR_BAD_ARG;
		} else {
			std::string converter(input, 2, input_size);
			if (converter == "about") {
				output = MODE_ABOUT;
			} else if (converter == "help") {
				output = MODE_HELP;
			} else if (converter == "list") {
				output = MODE_LIST;
			}
			else if (converter == "digest") {
				output = MODE_DIGEST;
			} else if (converter == "compile") {
				output = MODE_COMPILE;
			} else {
				if (converter == "NO_PARAM") {
					output = ERROR_NO_PARAM;
				} else if (converter == "BAD_PARAM") {
					output = ERROR_BAD_PARAM;
				} else if (converter == "NOT_TXT") {
					output = ERROR_NOT_TXT;
				} else {
					output = ERROR_BAD_ARG;
				}
			}
		}
	}
	return output;
}
