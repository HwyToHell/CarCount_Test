#include "stdafx.h"
#include <map>
#include "../../videoprocessing/include/config.h" // includes tracker.h
#include "../../../cpp/inc/program_options.h"
//#include <sys/stat.h> TODO for Unix

using namespace std;


// TODO check env with separate variable
SCENARIO("read env", "[Config]") {
	#if defined (_WIN32)
	char delim = '\\';
	#elif defined (__linux__)
	char delim = '/';
	#else
	throw 1;
	#endif
	GIVEN("$(HOME)") {
		// getenv() <cstdlib>
		string home(getenv("HOMEDRIVE"));
		home += getenv("HOMEPATH");
		home += delim;
		cout << home << endl;
		WHEN("config is created") {
			Config config;
			THEN("home dir is returned") {
				REQUIRE(home == config.readEnvHome());
			}
		}
	}
}

SCENARIO("save command line args in config", "[Config]") {
	// arguments
	//  i(nput): cam (single digit number) or file name,
	//		if empty take standard cam
	//  r(ate):	fps for video device
	//  v(ideo size): frame size in px (width x height)
	//  w(orking directory): working dir, starting in $home
	char* optstr = "i:r:v:w:";

	GIVEN("args for video input -i") {
		char* iFile_OK = "videofile.mp4";
		char* iCam_OK = "9";
		char* iCam_Wrong = "a";
		
		Config config;

		WHEN("i: no video input specified") {
			char* av[] = {"progname", "-i"};
			int ac = (sizeof(av)/sizeof(av[0]));	
			ProgramOptions po(ac, av, optstr);
			
			THEN("use standard cam == 0") {
				REQUIRE(config.readCmdLine(po) == true);
				string vidDevice = config.getParam("video_device");
				REQUIRE(vidDevice == "0"); 
			}
		}

		WHEN("i: cam input device specified") {
			char* av[] = {"progname", "-i", iCam_OK};
			int ac = (sizeof(av)/sizeof(av[0]));	
			ProgramOptions po(ac, av, optstr);
			
			THEN("config returns device ID") {
				REQUIRE(config.readCmdLine(po) == true);
				string vidDevice = config.getParam("video_device");
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
				string vidFile = config.getParam("video_file");
				string fromCam = config.getParam("is_video_from_cam");
				REQUIRE(vidFile == string(iFile_OK)); 
				REQUIRE(fromCam == "false"); 
			}
		}
	} // end GIVEN("args for video input -i")

	GIVEN("args for video frame rate -r") {
		Config config;

		WHEN("r: frame rate correctly specified") {
			string frameRate_OK("25");
			char* av[] = {"progname", "-r", (char*)frameRate_OK.c_str()};
			int ac = (sizeof(av)/sizeof(av[0]));	
			ProgramOptions po(ac, av, optstr);
			
			THEN("config returns frame rate") {
				REQUIRE(config.readCmdLine(po) == true);
				REQUIRE(frameRate_OK == config.getParam("frame_rate"));
			}
		}

		WHEN("r: frame rate specified as float") {
			string floatRate("20.0");
			char* av[] = {"progname", "-r", (char*)floatRate.c_str()};
			int ac = (sizeof(av)/sizeof(av[0]));	
			ProgramOptions po(ac, av, optstr);
			
			THEN("readCmdLine returns false") {
				REQUIRE(config.readCmdLine(po) == false);
			}
		}
		
		WHEN("r: frame rate missing completely") {
			char* av[] = {"progname", "-r", ""};
			int ac = (sizeof(av)/sizeof(av[0]));	
			ProgramOptions po(ac, av, optstr);
			
			THEN("readCmdLine returns false") {
				REQUIRE(config.readCmdLine(po) == false);
			}
		}

	} // end GIVEN("args for video frame rate -r")

	GIVEN("args for video frame size -v") {
		string vFrameSize_x("320");
		string vFrameSize_y("240");
		Config config;

		WHEN("v: video size correctly specified") {
			string frameSize_OK(vFrameSize_x + "x" + vFrameSize_y);
			char* av[] = {"progname", "-v", (char*)frameSize_OK.c_str()};
			int ac = (sizeof(av)/sizeof(av[0]));	
			ProgramOptions po(ac, av, optstr);
			
			THEN("config returns video width and video height") {
				REQUIRE(config.readCmdLine(po) == true);
				REQUIRE(vFrameSize_x == config.getParam("framesize_x"));
				REQUIRE(vFrameSize_y == config.getParam("framesize_y"));
			}
		}

		WHEN("v: video size misses width") {
			string missingWidth("x" + vFrameSize_y);
			char* av[] = {"progname", "-v", (char*)missingWidth.c_str()};
			int ac = (sizeof(av)/sizeof(av[0]));	
			ProgramOptions po(ac, av, optstr);
			
			THEN("readCmdLine returns false") {
				REQUIRE(config.readCmdLine(po) == false);
			}
		}

		WHEN("v: video size misses height") {
			string missingHeight(vFrameSize_x + "x");
			char* av[] = {"progname", "-v", (char*)missingHeight.c_str()};
			int ac = (sizeof(av)/sizeof(av[0]));	
			ProgramOptions po(ac, av, optstr);
			
			THEN("readCmdLine returns false") {
				REQUIRE(config.readCmdLine(po) == false);
			}
		}

		WHEN("v: video size missing completely") {
			char* av[] = {"progname", "-v", ""};
			int ac = (sizeof(av)/sizeof(av[0]));	
			ProgramOptions po(ac, av, optstr);
			
			THEN("readCmdLine returns false") {
				REQUIRE(config.readCmdLine(po) == false);
			}
		}

	} // end GIVEN("args for video frame size -v")

	GIVEN("args for working directory -w") {
		string wWorkDir_OK = "workdir";
		Config config;
		string videoPath = config.readEnvHome();
		videoPath = appendDirToPath(videoPath, wWorkDir_OK);

		WHEN("w: work dir correctly specified") {
			char* av[] = {"progname", "-w", (char*)wWorkDir_OK.c_str()};
			int ac = (sizeof(av)/sizeof(av[0]));	
			ProgramOptions po(ac, av, optstr);
			
			THEN("config returns correct work path") {
				REQUIRE(config.readCmdLine(po) == true);
				REQUIRE(videoPath == config.getParam("video_path"));
			}
		}
	} // end GIVEN("args for working directory -w")

} // end SCENARIO("save command line args in config", "[Config]")


SCENARIO("adjust video size dependent parameters", "[Config]") {
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
				REQUIRE( new_size_x == stoi(config.getParam("framesize_x")) );
				REQUIRE( new_size_y == stoi(config.getParam("framesize_y")) );
			}
			
		}


	} 
} // end SCENARIO("adjust video size dependent parameters", "[Config]")