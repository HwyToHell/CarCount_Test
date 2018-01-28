#include "stdafx.h"
#include "../../../cpp/inc/program_options.h"


using namespace std;


int mapSize_test(ProgramOptions& opts) {
	return opts.mOptMap.size(); 
}

SCENARIO("Parse command line options", "[Config]")
{
	GIVEN("command line with 1 short and 2 long options") {
		// create argc from argv
		char* argv_[] = { "progname", "-v", "videoinput", "-c", "-o", "filename", "d" };
		int argc_ = (sizeof(argv_)/sizeof(argv_[0]));		
		ProgramOptions cmdlnOpts(argc_, argv_, "co:v:");
		WHEN("command line is parsed")
		{
			//cmdlnOpts.parseCmdLine(argc_, argv_, "co:v:");
			THEN("arguments are stored in opt_map") {
				REQUIRE(cmdlnOpts.exists('c') == true);
				REQUIRE(cmdlnOpts.exists('o') == true);
				REQUIRE(cmdlnOpts.getOptArg('o') == "filename");
				REQUIRE(cmdlnOpts.exists('v') == true);
				REQUIRE(cmdlnOpts.getOptArg('v') == "videoinput");
			}
			THEN("opt_map has 3 elements") {
				REQUIRE(mapSize_test(cmdlnOpts) == 3);
			}
		}
	}

	GIVEN("command line with no options") {
		// create argc from argv
		char* argv_[] = { "progname" };
		int argc_ = (sizeof(argv_)/sizeof(argv_[0]));	
		ProgramOptions cmdlnOpts(argc_, argv_, "co:v:");
		WHEN("command line is parsed") {
			THEN("opt_map has 0 elements") {
				REQUIRE(mapSize_test(cmdlnOpts) == 0);
			}
		}
	}

	GIVEN("command line with wrong options") {
		// create argc from argv
		char* argv_[] = { "progname", "a", "--c", "--o", "filename", "v" };
		int argc_ = (sizeof(argv_)/sizeof(argv_[0]));		
		ProgramOptions cmdlnOpts(argc_, argv_, "co:v:");
		WHEN("command line is parsed")
		{
			THEN("no arguments should inserted into opt_map") {
				REQUIRE(cmdlnOpts.exists('a') == false);
				REQUIRE(cmdlnOpts.exists('c') == false);
				REQUIRE(cmdlnOpts.exists('o') == false);
				REQUIRE(cmdlnOpts.exists('v') == false);
			}
			THEN("opt_map has 0 elements") {
				REQUIRE(mapSize_test(cmdlnOpts) == 0);
			}
		}	
	}

	GIVEN("command line with extended option and no optional arguments") {
		// create argc from argv
		char* argv_[] = { "progname", "-o", "-c", "-v" };
		int argc_ = (sizeof(argv_)/sizeof(argv_[0]));		
		ProgramOptions cmdlnOpts(argc_, argv_, "co:v:");
		WHEN("command line is parsed")
		{
			THEN("'-c' is a simple option") {
				REQUIRE(cmdlnOpts.exists('c') == true);
				REQUIRE(cmdlnOpts.getOptArg('c') == "");
			}
			THEN("extended option '-o' does not have an 'opt arg'") {
				// "-c" is not an opt arg for "-o"
				REQUIRE(cmdlnOpts.getOptArg('o') == "");
			}
			THEN("extended option -v' at the end does not have an 'opt arg'") {
				// "-v" does not have an opt arg
				REQUIRE(cmdlnOpts.getOptArg('v') == "");
			}
			THEN("opt_map has 3 elements") {
				REQUIRE(mapSize_test(cmdlnOpts) == 3);
			}
		}	
	}

	GIVEN("command line with multiple occurances of the same option") {
		// create argc from argv
		char* argv_[] = { "progname", "-v", "firstOptArg", "-v", "secondOptArg", 
			"-o", "-c" };
		int argc_ = (sizeof(argv_)/sizeof(argv_[0]));		
		ProgramOptions cmdlnOpts(argc_, argv_, "co:v:");
		WHEN("command line is parsed")
		{
			THEN("only the first option '-v' is inserted into map, others are skipped") {
				REQUIRE(cmdlnOpts.getOptArg('v') == "firstOptArg");
			}
			THEN("extended option '-o' does not have an 'opt arg'") {
				REQUIRE(cmdlnOpts.getOptArg('o') == "");
			}
			THEN("'-c' is a simple option") {
				REQUIRE(cmdlnOpts.exists('c') == true);
				REQUIRE(cmdlnOpts.getOptArg('c') == "");
			}
			THEN("opt_map has 3 elements") {
				REQUIRE(mapSize_test(cmdlnOpts) == 3);
			}
		}	
	}
}