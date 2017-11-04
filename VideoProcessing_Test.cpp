#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

using namespace std;


int main(int argc, char* argv[])
{
	int result = Catch::Session().run( argc, argv );
	
	cout << "Press <enter> to continue" << endl;

	string str;
	getline(cin, str);
	return 0;
}

