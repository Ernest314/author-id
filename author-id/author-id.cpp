#include "stdafx.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

enum RunMode {
	ERROR_UNKNOWN	= -1,
	ERROR_NO_ARG	= -2,
	ERROR_BAD_ARG	= -3,
	ERROR_NO_PARAM	= -4,
	ERROR_BAD_PARAM	= -5,

	MODE_ABOUT	= 0,
	MODE_HELP,
	MODE_NUM
};

RunMode hash_string(std::string input);

using std::cout;
using std::endl;
using std::cin;
using std::string;

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
				cout << "-h, --help\n" <<
					"\tShows these help instructions." << endl << endl;
				cout << endl;
				break;
			default :
				break;
		}
		args.erase(args.begin(), args.begin() + erase_num);
	}

	cout << endl;
	return 0;
}

RunMode hash_string(std::string input)
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
				case 'h' :
					output = MODE_HELP;
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
			} else {
				output = ERROR_BAD_ARG;
			}
		}
	}
	return output;
}
