#include "stdafx.h"

#include "utils.h"
#include "digest.h"
#include "merge.h"
#include "compare.h"

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
					"\tDigests the specified file (from the \"Texts\" directory)." << endl << endl;
				cout << "-m, --merge filename.txt\n" <<
					"\tMerges the listed file into its author's file. The file\n" <<
					"\tmust be from the \"Digests\" directory." << endl << endl;
				cout << "-c, --compare author-A.txt author-B.txt\n" <<
					"\tRearranges the lists of author-B to match author-A. The\n" <<
					"\tfiles must both be in the \"Authors\" directory." << endl << endl;
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
			case MODE_MERGE :
				erase_num = 2;
				// WARNING: If there aren't enough parameters, breaks out of switch immediately.
				// Also breaks out of switch immediately if filename extension is not ".txt".
				if (args.size() < 2) {
					// then size() must be 1, since it is also >0
					erase_num = 1;
					args.push_back("--NO_PARAM");
					break;
				} else {
					string filename_merge_input = args[1];
					string buf(filename_merge_input,
						filename_merge_input.size() - 4,
						filename_merge_input.size());
					if (buf != ".txt") {
						erase_num = 1;
						args[1] = "--NOT_TXT";
						break;
					} else {
						merge_input(filename_merge_input);
					}
				}
				break;
			case MODE_COMPARE :
				erase_num = 3;
				// WARNING: If there aren't enough parameters, breaks out of switch immediately.
				// Also breaks out of switch immediately if filename extension is not ".txt".
				if (args.size() < 3) {
					// then size() must be 1, since it is also >0
					erase_num = 1;
					args.push_back("--NO_PARAM");
					break;
				}
				else {
					string filename_compare_input_A = args[1];
					string filename_compare_input_B = args[2];
					string buf_A(filename_compare_input_A,
						filename_compare_input_A.size() - 4,
						filename_compare_input_A.size());
					string buf_B(filename_compare_input_B,
						filename_compare_input_B.size() - 4,
						filename_compare_input_B.size());
					if (buf_A != ".txt" || buf_B != ".txt") {
						erase_num = 1;
						args[1] = "--NOT_TXT";
						break;
					}
					else {
						compare_input(filename_compare_input_A, filename_compare_input_B);
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
				case 'm' :
					output = MODE_MERGE;
					break;
				case 'c':
					output = MODE_COMPARE;
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
			} else if (converter == "digest") {
				output = MODE_DIGEST;
			} else if (converter == "merge") {
				output = MODE_MERGE;
			} else if (converter == "compare") {
				output = MODE_COMPARE;
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
