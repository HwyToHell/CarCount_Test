#include "stdafx.h"
#include <cstdio> // rand()
#include <map>
#include "../../videoprocessing/include/config.h" // includes tracker.h
#include "../../../cpp/inc/program_options.h"
//#include <sys/stat.h> TODO for Unix
//#include <unistd.h> Linux: rmdir

using namespace std;


/// TODO test directory manipulation functions
// appendDirToPath: dir + path with and w/o delimiter
// isFileExist:		check for file and directory, nonexistent, no access, ... 


SCENARIO("#dir001 getHomePath", "[Config]") {
	#if defined (_WIN32)
	char delim = '\\';
	#elif defined (__linux__)
	char delim = '/';
	#else
	throw "unsupported OS";
	#endif
	GIVEN("$(HOME)") {
		string home(getenv("HOMEDRIVE"));
		home += getenv("HOMEPATH");

		WHEN("config is created") {
			THEN("home path is returned") {
				REQUIRE(home == getHomePath());
			}
		}
	}
}


SCENARIO("#dir003 appendDirToPath", "[Config]") {
	// path with trailing slash
	string first("first");
	string second("second");
	GIVEN("first and second string without trailing slash") {
		WHEN("second string is appended") {
			string path = first;
			appendDirToPath(path, second);
			THEN("additional delimiter is inserted") {
				REQUIRE(path.at(5) == '\\');
			}
		}
		WHEN("second string with trailing is appended") {
			string path = first + "\\";
			appendDirToPath(path, second);
			THEN("no additional delimiter is inserted") {
				size_t pos = path.find('\\');
				++pos;
				REQUIRE(path.find('\\', pos) == string::npos);
			}
		}
	}
}


SCENARIO("#dir002 isFileExist", "[Config]") {
	GIVEN("unique test path underneath /home") {
		
		// generate test dir name	
		string testDir("test123");
		// underneath /home
		string testPath = getHomePath();
		appendDirToPath(testPath, testDir);

		WHEN("test path is created") {
			REQUIRE(makePath(testPath) == true);
			THEN("test path does exist") {
				REQUIRE(isFileExist(testPath) == true);
			}
		}

		WHEN("test path is deleted") {
			#if defined (_WIN32)
			REQUIRE(_rmdir(testPath.c_str()) == 0);
			#elif defined (__linux__)
			REQUIRE(rmdir(testPath.c_str()) == 0);
			#else
			throw "unsupported OS";
			#endif
			THEN("test path does not exist") {
				REQUIRE(isFileExist(testPath) == false);
			}
		}
	}
} // end SCENARIO("isFileEist #dir002", "[Config]") {


SCENARIO("#cfg001 Config::Config", "[Config]") {
	GIVEN("config with std parameters") {
		Config config;
		
		// check application_path existence
		// check other parameters for != ""
		WHEN("config is constructed") {
			THEN("application_path is set") {
				string appPath = getHomePath();
				appendDirToPath(appPath, "counter");
				REQUIRE(config.getParam("application_path") == appPath);
				REQUIRE(isFileExist(appPath));
			}
			AND_THEN("all other parameters return valid string values") {
				size_t nParams = sizeof(configParams) / sizeof(configParams[0]);
				for (size_t n = 0; n < nParams; ++n) {
					string name = configParams[n];
					string value = config.getParam(name);
					REQUIRE(value != "");
				}
			}
		}
	}
} // end SCENARIO("#cfg001 Config::Config", "[Config]")



SCENARIO("#cfg002 Config::readCmdLine", "[Config]") {
	// arguments
	//  i(nput): cam (single digit number) or file name,
	//		if empty take standard cam
	//  r(ate):	fps for video device
	//  v(ideo size): frame size from pick_list (single digit number)
	//  w(orking directory): working dir, starting in $home
	char* optstr = "i:qr:v:w:";

	GIVEN("args for video input -i") {
		char* iFile_OK = "D:\\Users\\Holger\\counter\\traffic640x480.avi";
		char* iCam_OK = "9";
		char* iCam_Wrong = "a";
		
		Config config;

		WHEN("i: no video input specified") {
			char* av[] = {"progname", "-i"};
			int ac = (sizeof(av)/sizeof(av[0]));	
			ProgramOptions po(ac, av, optstr);
			
			THEN("use standard cam == 0") {
				REQUIRE(config.readCmdLine(po) == true);
				string vidDevice = config.getParam("cam_device_ID");
				REQUIRE(vidDevice == "0"); 
			}
		}

		WHEN("i: cam input device specified") {
			char* av[] = {"progname", "-i", iCam_OK};
			int ac = (sizeof(av)/sizeof(av[0]));	
			ProgramOptions po(ac, av, optstr);
			
			THEN("config returns device ID") {
				REQUIRE(config.readCmdLine(po) == true);
				string vidDevice = config.getParam("cam_device_ID");
				string fromCam = config.getParam("is_video_from_cam");
				REQUIRE(vidDevice == string(iCam_OK)); 
				REQUIRE(fromCam == "true"); 
			}
		}

		WHEN("i: wrong cam input device specified") {
			char* av[] = {"progname", "-i", iCam_Wrong};
			int ac = (sizeof(av)/sizeof(av[0]));	
			ProgramOptions po(ac, av, optstr);
			
			THEN("readCmdLine returns false") {
				REQUIRE(config.readCmdLine(po) == false);
			}
		}

		WHEN("i: video file specified") {
			char* av[] = {"progname", "-i", iFile_OK};
			int ac = (sizeof(av)/sizeof(av[0]));	
			ProgramOptions po(ac, av, optstr);
			
			THEN("config returns file name, from_cam flag == false") {
				REQUIRE(config.readCmdLine(po) == true);
				string vidFile = config.getParam("video_file_path");
				string fromCam = config.getParam("is_video_from_cam");
				REQUIRE(vidFile == string(iFile_OK)); 
				REQUIRE(fromCam == "false"); 
			}
		}
	} // end GIVEN("args for video input -i")

	GIVEN("args for video frame rate -r") {
		Config config;
		// quiet must be specified in order to avoid readCmdLine to return false

		WHEN("r: frame rate correctly specified") {
			string frameRate_OK("25");
			char* av[] = {"progname", "-q", "-r", (char*)frameRate_OK.c_str()};
			int ac = (sizeof(av)/sizeof(av[0]));	
			ProgramOptions po(ac, av, optstr);
			
			THEN("config returns frame rate") {
				REQUIRE(config.readCmdLine(po) == true);
				REQUIRE(frameRate_OK == config.getParam("cam_fps"));
			}
		}

		WHEN("r: frame rate specified as float") {
			string floatRate("20.0");
			char* av[] = {"progname", "-q", "-r", (char*)floatRate.c_str()};
			int ac = (sizeof(av)/sizeof(av[0]));	
			ProgramOptions po(ac, av, optstr);
			
			THEN("readCmdLine returns false") {
				REQUIRE(config.readCmdLine(po) == false);
			}
		}
		
		WHEN("r: frame rate missing completely") {
			char* av[] = {"progname", "-q", "-r", ""};
			int ac = (sizeof(av)/sizeof(av[0]));	
			ProgramOptions po(ac, av, optstr);
			
			THEN("readCmdLine returns false") {
				REQUIRE(config.readCmdLine(po) == false);
			}
		}

	} // end GIVEN("args for video frame rate -r")

	GIVEN("args for cam resolution ID -v") {
		string vResolution_OK = "0";
		string vResolution_Wrong = "99";
		Config config;
		// quiet must be specified in order to avoid readCmdLine to return false

		WHEN("v: resolution ID correctly specified") {
			char* av[] = {"progname", "-q", "-v", (char*)vResolution_OK.c_str()};
			int ac = (sizeof(av)/sizeof(av[0]));
			ProgramOptions po(ac, av, optstr);

			THEN("config returns resolution ID") {
				REQUIRE(config.readCmdLine(po) == true);
				REQUIRE(vResolution_OK == config.getParam("cam_resolution_ID"));
			}
		}
		
		WHEN("v: resolution ID is missing") {
			char* av[] = {"progname", "-q", "-v", ""};
			int ac = (sizeof(av)/sizeof(av[0]));
			ProgramOptions po(ac, av, optstr);

			THEN("readCmdLine returns false") {
				REQUIRE(config.readCmdLine(po) == false);
			}
		}

		WHEN("v: resolution ID wrong") {
			char* av[] = {"progname", "-q", "-v", (char*)vResolution_Wrong.c_str()};
			int ac = (sizeof(av)/sizeof(av[0]));
			ProgramOptions po(ac, av, optstr);

			THEN("readCmdLine returns false") {
				REQUIRE(config.readCmdLine(po) == false);
			}
		}
	} // end GIVEN("args for cam resolution ID -v")
} // end SCENARIO("save command line args in config", "[Config]")

// config file path for test case #cfg004
// module global in order to delete 
static string cfg004_FilePath;
SCENARIO("#cfg004 Config::saveConfigToFile and read back", "[Config]") {
	GIVEN("good parameter list with test value") {
		Config config;
		cfg004_FilePath = config.getParam("application_path");
		appendDirToPath(cfg004_FilePath, "test.sqlite");
		
		// 1 st pass: config file does not exist
		WHEN("test values are transferred to config") {
			string testValue("999");
			size_t nParams = sizeof(configParams) / sizeof(configParams[0]);
			for (size_t n = 0; n < nParams; ++n) {
				REQUIRE(config.setParam(configParams[n], testValue));
			}
			THEN("test values are saved in config file") {
				REQUIRE(config.saveConfigToFile(cfg004_FilePath));
			}
			AND_THEN("same test values are read back from config file") {
				REQUIRE(config.readConfigFile(cfg004_FilePath));
				for (size_t n = 0; n < nParams; ++n) {
					REQUIRE(config.getParam(configParams[n]) == testValue);
				}
			}
		}

		// 2nd pass: config file does exist
		WHEN("config file exists and second test values are transferred to config") {
			string testValue2nd("222");
			size_t nParams = sizeof(configParams) / sizeof(configParams[0]);
			for (size_t n = 0; n < nParams; ++n) {
				REQUIRE(config.setParam(configParams[n], testValue2nd));
			}
			THEN("test values are saved in config file") {
				REQUIRE(config.saveConfigToFile(cfg004_FilePath));
			}
			AND_THEN("same test values are read back from config file") {
				REQUIRE(config.readConfigFile(cfg004_FilePath));
				for (size_t n = 0; n < nParams; ++n) {
					REQUIRE(config.getParam(configParams[n]) == testValue2nd);
				}
			}
		}
	} // ~Config -> db is closed

	// tear down
	WHEN("tests are passed, config file #004 can deleted") {
		REQUIRE(remove(cfg004_FilePath.c_str()) == 0);
	}
} // end SCENARIO("#cfg004 Config::saveConfigToFile and read back", "[Config]")


static string cfg003_FilePath;
SCENARIO("#cfg003 Config::readConfigFromFile", "[Config]") {
	GIVEN("good config and config file") {
		Config config;
		cfg003_FilePath = config.getParam("application_path");
		appendDirToPath(cfg003_FilePath, "delete.sqlite");
		REQUIRE(config.saveConfigToFile(cfg003_FilePath));

		// TODO after refactoring queryDbxxx() functions into separate class
		//	delete one data record, so that config file contains wrong number of parameters
		WHEN("config file contains wrong number of parameters") {
			THEN("reading config file fails") {
				;// TODO implementation
			}
		}

		WHEN("config file does not exist") {
			// make sure file does not exist
			string fileNotExistent = config.getParam("application_path");
			appendDirToPath(fileNotExistent, "not_existent.sqlite");
			REQUIRE(isFileExist(fileNotExistent) == false);

			THEN("reading config file fails") {
				REQUIRE(false == config.readConfigFile(fileNotExistent));
			}
		}
	}

	// tear down
	WHEN("tests are passed, config file #003 can deleted") {
		REQUIRE(remove(cfg003_FilePath.c_str()) == 0);
	}

} // end SCENARIO("#cfg003 Config::readConfigFromFile", "[Config]")

				
SCENARIO("#cfg005 adjust video size dependent parameters", "[Config]") {
	GIVEN("dependent parameter list, new video size") {
		Config config;
		
		// create map with test values
		typedef pair<const string, int> TPairStr;
		map<TPairStr::first_type, TPairStr::second_type> depParams;
		
		const int old_size_x = 320;
		const int old_size_y = 240;
		depParams.insert(TPairStr("framesize_x", old_size_x));
		depParams.insert(TPairStr("framesize_y", old_size_y));
		int new_size_x = 640;
		int new_size_y = 480;
		
		const int intToAdjust = 100;		
		depParams.insert(TPairStr("roi_x", intToAdjust));
		depParams.insert(TPairStr("roi_width", intToAdjust));
		depParams.insert(TPairStr("roi_y", intToAdjust));
		depParams.insert(TPairStr("roi_height", intToAdjust));

		depParams.insert(TPairStr("blob_area_min", intToAdjust));
		depParams.insert(TPairStr("blob_area_max", intToAdjust));		
		depParams.insert(TPairStr("track_max_distance", intToAdjust));

		depParams.insert(TPairStr("count_pos_x", intToAdjust));
		depParams.insert(TPairStr("count_track_length", intToAdjust));
		depParams.insert(TPairStr("truck_width_min", intToAdjust));		
		depParams.insert(TPairStr("truck_height_min", intToAdjust));

		class SetCfgParam {
			Config* m_cfg;
		public:
			SetCfgParam(Config* cfg) : m_cfg(cfg) {}
			void operator()(TPairStr& param) {
				m_cfg->setParam(param.first, to_string((long long)param.second));
			}
		};

		// copy test values to config
		for_each(depParams.begin(), depParams.end(), SetCfgParam(&config));
		
		WHEN("parameters are adjusted") {
			config.adjustFrameSizeDependentParams(new_size_x, new_size_y);

			THEN("getParam() shows adjusted values") {
				// dependent on x
				REQUIRE( depParams.at("roi_x") * new_size_x / old_size_x 
					== stoi(config.getParam("roi_x")) );
				REQUIRE( depParams.at("roi_width") * new_size_x / old_size_x 
					== stoi(config.getParam("roi_width")) );
				REQUIRE( depParams.at("count_pos_x") * new_size_x / old_size_x 
					== stoi(config.getParam("count_pos_x")) );
				REQUIRE( depParams.at("count_track_length") * new_size_x / old_size_x 
					== stoi(config.getParam("count_track_length")) );
				REQUIRE( depParams.at("truck_width_min") * new_size_x / old_size_x 
					== stoi(config.getParam("truck_width_min")) );
				
				// dependent on y
				REQUIRE( depParams.at("roi_y") * new_size_y / old_size_y 
					== stoi(config.getParam("roi_y")) );
				REQUIRE( depParams.at("roi_height") * new_size_y / old_size_y 
					== stoi(config.getParam("roi_height")) );
				REQUIRE( depParams.at("truck_height_min") * new_size_y / old_size_y 
					== stoi(config.getParam("truck_height_min")) );

				// dependent on (x + y) / 2
				REQUIRE( depParams.at("track_max_distance") 
					* (new_size_x /  old_size_x + new_size_y / old_size_y) / 2
					== stoi(config.getParam("track_max_distance")) );

				// dependent on x * y
				REQUIRE( depParams.at("blob_area_min") 
					* new_size_x /  old_size_x * new_size_y / old_size_y
					== stoi(config.getParam("blob_area_min")) );
				REQUIRE( depParams.at("blob_area_max") 
					* new_size_x /  old_size_x * new_size_y / old_size_y
					== stoi(config.getParam("blob_area_max")) );

				//new video size
				REQUIRE( new_size_x == stoi(config.getParam("frame_size_x")) );
				REQUIRE( new_size_y == stoi(config.getParam("frame_size_y")) );
			}
			
		}


	} 
} // end SCENARIO("adjust video size dependent parameters", "[Config]")
