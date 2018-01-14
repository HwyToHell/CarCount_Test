#include "stdafx.h"
#include "../../../cpp/inc/program_options.h"


using namespace std;

SCENARIO("Parse command line options", "[Config]")
{
	GIVEN("command line with 1 short and 2 long options") {
		// create argc from argv
		char* argv_[] = { "progname", "-v", "videoinput", "-c", "-o", "filename", "d" };
		int argc_ = (sizeof(argv_)/sizeof(argv_[0]));		
		ProgramOptions cmdlnOpts;
		WHEN("command line is parsed")
		{
			cmdlnOpts.parseCmdLine(argc_, argv_, "co:v:");
			THEN("arguments are stored in opt_map") {
				REQUIRE(cmdlnOpts.getOptMap().count('c') > 0);
				REQUIRE(cmdlnOpts.getOptMap().count('o') > 0);
				REQUIRE(cmdlnOpts.getOptMap()['o'] == "filename");
				REQUIRE(cmdlnOpts.getOptMap().count('v') > 0);
				REQUIRE(cmdlnOpts.getOptMap()['v'] == "videoinput");
			}
			THEN("opt_map has 3 elements") {
				REQUIRE(cmdlnOpts.getOptMap().size() == 3);
			}
			AND_THEN("opt_map has still 3 elements, if parsed again") {
				cmdlnOpts.parseCmdLine(argc_, argv_, "co:v:");
				REQUIRE(cmdlnOpts.getOptMap().size() == 3);
			}
		}
	}

	GIVEN("command line with no options") {
		// create argc from argv
		char* argv_[] = { "progname" };
		int argc_ = (sizeof(argv_)/sizeof(argv_[0]));	
		ProgramOptions cmdlnOpts;
		WHEN("command line is parsed") {
			cmdlnOpts.parseCmdLine(argc_, argv_, "co:v:");
			THEN("opt_map has 0 elements") {
				REQUIRE(cmdlnOpts.getOptMap().size() == 0);
			}
		}
	}

	GIVEN("command line with wrong options") {
		// create argc from argv
		char* argv_[] = { "progname", "a", "--c", "--o", "filename", "v" };
		int argc_ = (sizeof(argv_)/sizeof(argv_[0]));		
		ProgramOptions cmdlnOpts;
		WHEN("command line is parsed")
		{
			cmdlnOpts.parseCmdLine(argc_, argv_, "co:v:");
			THEN("no arguments should inserted into opt_map") {
				REQUIRE(cmdlnOpts.getOptMap().count('a') == 0);
				REQUIRE(cmdlnOpts.getOptMap().count('c') == 0);
				REQUIRE(cmdlnOpts.getOptMap().count('o') == 0);
				REQUIRE(cmdlnOpts.getOptMap().count('v') == 0);
			}
			THEN("opt_map has 0 elements") {
				REQUIRE(cmdlnOpts.getOptMap().size() == 0);
			}
		}	
	}

	GIVEN("command line with extended option and no optional arguments") {
		// create argc from argv
		char* argv_[] = { "progname", "-o", "-c", "-v" };
		int argc_ = (sizeof(argv_)/sizeof(argv_[0]));		
		ProgramOptions cmdlnOpts;
		WHEN("command line is parsed")
		{
			cmdlnOpts.parseCmdLine(argc_, argv_, "co:v:");
			THEN("'-c' is a simple option") {
				REQUIRE(cmdlnOpts.getOptMap().count('c') > 0);
				REQUIRE(cmdlnOpts.getOptMap()['c'] == "");
			}
			THEN("extended option '-o' does not have an 'opt arg'") {
				// "-c" is not an opt arg for "-o"
				REQUIRE(cmdlnOpts.getOptMap()['o'] == ""); 
			}
			THEN("extended option -v' at the end does not have an 'opt arg'") {
				// "-v" does not have an opt arg
				REQUIRE(cmdlnOpts.getOptMap()['v'] == "");
			}
			THEN("opt_map has 3 elements") {
				REQUIRE(cmdlnOpts.getOptMap().size() == 3);
			}
		}	
	}

	GIVEN("command line with multiple occurances of the same option") {
		// create argc from argv
		char* argv_[] = { "progname", "-v", "firstOptArg", "-v", "secondOptArg", 
			"-o", "-c" };
		int argc_ = (sizeof(argv_)/sizeof(argv_[0]));		
		ProgramOptions cmdlnOpts;
		WHEN("command line is parsed")
		{
			cmdlnOpts.parseCmdLine(argc_, argv_, "co:v:");
			THEN("only the first option is inserted into map, others are skipped") {
				REQUIRE(cmdlnOpts.getOptMap()['v'] == "firstOptArg");
			}
			THEN("extended option '-o' does not have an 'opt arg'") {
				REQUIRE(cmdlnOpts.getOptMap()['o'] == "");
			}
			THEN("'-c' is a simple option") {
				REQUIRE(cmdlnOpts.getOptMap().count('c') > 0);
				REQUIRE(cmdlnOpts.getOptMap()['c'] == "");
			}
			THEN("opt_map has 3 elements") {
				REQUIRE(cmdlnOpts.getOptMap().size() == 3);
			}
		}	
	}
}