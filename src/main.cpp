
/*
	Copyright (c) 2009 by Chad Nelson
	Released under the MIT License.
	See the provided LICENSE.TXT file for details.
*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>

#include <boost/optional.hpp>
//#include <boost/regex.hpp> // For the 'test' option

#include "markdown.h"

using std::cerr;
using std::cout;
using std::endl;

namespace {

	class Options {
		public:
		Options(): mDebug(false), mTest(false) { }

		int readOptions(int argc, char *argv[]);

		void showHelp();

		bool debug() const { return mDebug; }
		bool test() const { return mTest; }
		std::string inputFile() const { return mInputFile; }
        std::string outFile() const { return mOutputFile; }

		private:
		std::string mInputFile;
        std::string mOutputFile;
        bool mDebug, mTest;
	};

	int Options::readOptions(int argc, char *argv[]) {
		bool help=false, err=false;
		for (int x=1; x<argc && !help && !err; ++x) {
			if (argv[x][0]=='-' && argv[x][1]=='-' && argv[x][2]!=0) {
				// It's a full-word option.
				std::string opt(argv[x]+2);
				if (opt=="debug") {
					mDebug=true;
				} else if (opt=="test") {
					mTest=true;
                } else if (opt == "help") {
                    help = true;
                } else if (opt == "out") {
                    if ((x + 1) >= argc) {
                        err = true;
                        cerr << "Option " << argv[x] << " must be followed by output file, use -? for help."
                            << endl;
                    }
                    else {
                        x++;
                        if (mOutputFile.empty()) {
                            mOutputFile = argv[x];
                        }
                        else {
                            err = true;
                            cerr << "Too many parameters. Already had out '" << mOutputFile
                                << "', found '" << argv[x] << "' too. Use -? for help."
                                << endl;
                        }

                    }
				} else {
					err=true;
					cerr << "Unknown option " << argv[x] << ", use -? for help."
						<< endl;
				}
			} else if (argv[x][0]=='-') {
				// It's one or more option flags.
				const char *i=argv[x]+1;
				while (*i!=0 && !help && !err) {
					switch (*i) {
						case '?': help=true; break;
						case 'd': mDebug=true; break;
                        case 'o':
                            if ((x + 1) >= argc) {
                                err = true;
                                cerr << "Option " << argv[x] << " must be followed by output file, use -? for help."
                                    << endl;
                            }
                            else {
                                x++;
                                if (mOutputFile.empty()) {
                                    mOutputFile = argv[x];
                                }
                                else {
                                    err = true;
                                    cerr << "Too many parameters. Already had output '" << mOutputFile
                                        << "', found '" << argv[x] << "' too. Use -? for help."
                                        << endl;
                                }

                            }

                            break;
						default:
						err=true;
						cerr << "Uknown option flag '" << *i << "', use -? for "
							"help." << endl;
					}
					++i;
				}
			} else {
				// It's a parameter.
				if (mInputFile.empty()) {
					mInputFile=argv[x];
				} else {
					err=true;
					cerr << "Too many parameters. Already had '" << mInputFile
						<< "', found '" << argv[x] << "' too. Use -? for help."
						<< endl;
				}
			}
		}

		if (help) {
			showHelp();
			return 2;
		} else if (err) {
			return 1;
		} 
        return 0;
	}

	void Options::showHelp() {
        const char *cHelpScreen =
            "This program converts input (from an input file or stdin) from Markdown syntax\n"
            "to HTML using the cpp-markdown library.\n"
            "\n"
            "Usage:\n"
            "    cpp-markdown [<option>...] [input-file]\n"
            "\n"
            "Available options are:\n"
            "    -?, --help      Show this screen.\n"
            "    -d, --debug     Show tokens instead of HTML output.\n"
            "    -o, --out file  Write output to this file.\n";
		cerr << endl << cHelpScreen << endl;
	}

} // namespace

int main(int argc, char *argv[]) 
{
    int iret = 0;
	Options cfg;
    iret = cfg.readOptions(argc, argv);
    if (iret) {
        if (iret == 2)
            iret = 0;
        return iret;
    }

//	if (cfg.test()) {
//		return 1;
//	}

	std::ifstream ifile;
    std::ofstream oFile;

	std::istream *in = &std::cin;
    std::ostream *out = &std::cout;

	if (!cfg.inputFile().empty()) {
		cerr << "Reading file '" << cfg.inputFile() << "'..." << endl;
		ifile.open(cfg.inputFile().c_str());
		if (!ifile) {
			cerr << "Error: Can't open file." << endl;
			return 1;
		} else in=&ifile;
	} else cerr << "Reading standard input..." << endl;

	markdown::Document doc;
	doc.read(*in);

    if (cfg.outFile().size()) {
        cerr << "Writing file '" << cfg.outFile() << "'..." << endl;
        oFile.open(cfg.outFile().c_str());
        if (!oFile) {
            cerr << "Error: Can't open out file." << endl;
            return 1;
        }
        else out = &oFile;

    }

    if (cfg.debug()) {
        doc.writeTokens(*out);
    }
    else {
        doc.write(*out);
    }
	return iret;
}

/* eof */
