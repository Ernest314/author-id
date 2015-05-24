#include "stdafx.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using std::cout;
using std::endl;
using std::cin;
using std::string;

int main(int argc, char* argv[])
{
	cout << "  AUTHOR-ID PROGRAM" << endl;
	cout << "    Ernest Gu & Nathan Yang" << endl << endl;
	if (argc < 2) {
		cout << "You must enter at least one (1) argument.\n" <<
			"Type -h for a list of accepted arguments.\n\n";
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
		string mode = args[0];
		int erase_num = 1; // number of params per argument +1
		switch (mode.c_str()[1]) {
			// mode.c_str()[0] should contain a hyphen ('-')
			case 'a' :	// "About"
				cout << "This program is for our final AP Stats project." << endl <<
					"It creates statistics for a given author from sample" << endl <<
					"texts, and will try to infer the author of an unknown" << endl <<
					"input text." << endl << endl;
				break;
			case 'h' :	// "Help"
				cout << "Below is a list of accepted commands:\n";
				break;
			default :
				break;
		}
		mode.erase(mode.begin(), mode.begin() + erase_num);
	}

	int x;
	cin >> x;

	return 0;
}
