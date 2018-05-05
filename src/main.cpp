
/*
	Copyright (c) 2009 by Chad Nelson
	Released under the MIT License.
	See the provided LICENSE.TXT file for details.
*/

#include <sys/types.h>
#include <sys/stat.h>
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

//////////////////////////////////////////////////////
/// utils
#ifdef _WIN32
#define M_IS_DIR _S_IFDIR
#else // !_WIN32
#define M_IS_DIR S_IFDIR
#endif

#define MDT_NONE 0
#define MDT_FILE 1
#define MDT_DIR  2

static struct stat buf;
static int is_file_or_directory(const char * path)
{
    if (!path)
        return MDT_NONE;
    if (stat(path, &buf) == 0)
    {
        if (buf.st_mode & M_IS_DIR)
            return MDT_DIR;
        else
            return MDT_FILE;
    }
    return MDT_NONE;
}

static size_t get_last_file_size() { return buf.st_size; }
//////////////////////////////////////////////////////

static const char *headFile = 0;
static char *headContents = 0;

namespace {

	class Options {
		public:
		Options(): mDebug(false), mTest(false) { }

		int readOptions(int argc, char *argv[]);

		void showHelp();
        void showVersion();

		bool debug() const { return mDebug; }
		bool test() const { return mTest; }
		std::string inputFile() const { return mInputFile; }
        std::string outFile() const { return mOutputFile; }

		private:
		std::string mInputFile;
        std::string mOutputFile;
        bool mDebug, mTest;
	};

	int Options::readOptions(int argc, char *argv[]) 
    {
        int x, c, x2;
		bool help=false, err=false;
        char *arg, *sarg;
        for (x = 1; x < argc && !help && !err; x++) {
            arg = argv[x];
            x2 = x + 1;
            c = *arg;
            if (c == '-') {
                sarg = &arg[1];
                while (*sarg == '-')
                    sarg++;
                c = *sarg;
                switch (c) {
                case 'v':
                    showVersion();
                    return 2;
                case 'h':
                case '?':
                    help = true;
                    break;
                case 'd':
                    mDebug = true;
                    break;
                case 'o':
                    if (x2 < argc) {
                        x++;
                        sarg = argv[x];
                        if (mOutputFile.empty()) {
                            mOutputFile = sarg;
                        }
                        else {
                            err = true;
                            cerr << "Too many parameters. Already had out '" << mOutputFile
                                << "', found '" << arg << "' too. Use -? for help."
                                << endl;
                        }
                    }
                    else {
                        err = true;
                        cerr << "Option " << arg << " must be followed by output file, use -? for help." << endl;
                    }
                    break;
                case 'H':   /* get users HTML HEADER file */
                    if (x2 < argc) {
                        x++;
                        sarg = argv[x];
                        if (is_file_or_directory(sarg) == MDT_FILE) {
                            if (headFile) {
                                err = true;
                                cerr << "Too many parameters. Already had out '" << headFile
                                    << "', found '" << arg << " " << sarg << "' too. Use -? for help." << endl;
                            }
                            else {
                                size_t len = get_last_file_size();
                                headContents = new char[len+1];    // allocate memory for a buffer of appropriate dimension
                                if (!headContents) {
                                    err = true;
                                    cerr << "Error: Unable to allocate " << (len+1) << " bytes!" << endl;
                                    return 1;
                                }
                                memset(headContents, 0, len + 1);   /* is this really required? But have 'seen' some rubbish! */
                                std::ifstream t;
                                t.open(sarg);
                                if (t.bad()) {
                                    err = true;
                                    cerr << "Error: Unable to 'open' " << sarg << endl;
                                    return 1;
                                }
                                t.read(headContents, len);    // read the whole file into the buffer
                                t.close();                    // close file handle
                                headContents[len] = 0;
                                headFile = sarg;
                            }
                        }
                        else {
                            err = true;
                            cerr << "Error: Unable to 'stat' file " << sarg << endl;
                        }
                    }
                    else {
                        err = true;
                        cerr << "Option " << arg << " must be followed by html header file, use -? for help." << endl;
                    }
                    break;
                default:
                    err = true;
                    cerr << "Unknown option " << arg << ", use -? for help." << endl;
                    break;
                }
            }
            else {
                if (mInputFile.empty()) {
                    mInputFile = arg;
                }
                else {
                    err = true;
                    cerr << "Too many parameters. Already had '" << mInputFile
                        << "', found '" << arg << "' too. Use -? for help." << endl;
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

    void Options::showVersion() {
        const char *cVersion =
            "cpp-markdown: Version: " MD_VERSION ", Date: " MD_DATE " ";
        cerr << endl << cVersion << endl;
    }

	void Options::showHelp() {
        const char *cHelpScreen =
            "Usage: cpp-markdown [options...] [input-file]\n"
            "Options:\n"
            "    -v, --version   Show version, and exit(0).\n"
            "    -?, --help      Show this screen, and exit(0).\n"
            "    -d, --debug     Show tokens instead of HTML output.\n"
            "    -o, --out file  Write output to this file.\n"
            "    -H, --HEAD file Get 'doctype' and <head>...</head> from file.\n"
            "\n"
            "  This program converts input (from an input file or stdin) from Markdown syntax\n"
            "  to HTML using the 'cppmarkdown' library.\n";
        showVersion();
        cerr << cHelpScreen << endl;
	}

} // namespace

const char *cDefHead =
"<!DOCTYPE html>\n"
"<head lang=\"en\">\n"
"<meta charset=\"utf-8\">\n"
"<title>md2html</title>\n"
"<style>\n"
"code {\n"
"    background-color: #f8f8f8;\n"
"}\n"
".blk {\n"
"    display: block;\n"
"}\n"
"pre {\n"
"    display: block;\n"
"    width: 100%;\n"
"    background-color: #f8f8f8;\n"
"    padding: 6px 2em 6px 2em;\n"
"    border: 1px solid #ddd;\n"
"}\n"
".p {\n"
"    color: #6f00b1;\n"
"    background-color: #ffffff;\n"
"}\n"
"</style>\n"
"</head>\n";

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
        std::stringstream ss;
        std::string s;
        if (headContents) {
            s = headContents;
            delete headContents;
        }
        else {
            s = cDefHead;
        }
        s += "<body>\n";
        doc.write(ss);  // add the body html
        s += ss.str();  // add to output
        s += "</body>\n";
        s += "</html>\n";
        *out << s; // send it to the output - std::cout or file
    }
	return iret;
}

/* eof */
