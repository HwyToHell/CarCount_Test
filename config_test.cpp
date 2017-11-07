#include "stdafx.h"
#include "../../VideoProcessing/include/config.h"
#include <sys/stat.h>

using namespace std;

#if defined (_WIN32)
char delim = '\\';
#elif defined (__linux__)
char delim = '/';
#else
throw 1;
#endif

/*
 * check composing path
 * delete test.sqlite
 */
string dbFile = "test.sqlite";

// check composing full path to dbFile
// delete dbFile, if exists
SCENARIO("directory manipulation functions") {
	GIVEN("$(HOME)/count_traffic") {
		string fullPath = getHome();
		fullPath += "count_traffic";

		if (!pathExists(fullPath))
			REQUIRE(makePath(fullPath) == true);

		fullPath += delim;
		fullPath += dbFile;
		WHEN("'test.sqlite' exists") {
			struct _stat buffer;
			int exists = _stat(fullPath.c_str(), &buffer);
			if (_stat(fullPath.c_str(), &buffer) == 0) { // dbFile exists
			THEN("delete it")
				REQUIRE(_unlink(fullPath.c_str()) == 0);
			}
			AND_THEN("working directory has been cleaned")
				REQUIRE(_stat(fullPath.c_str(), &buffer) != 0);
		}
	}
}

// insert test_value in dbConfig and read it back
SCENARIO("construction of config, load from and save to db") {
	GIVEN("fresh Config") {
		Config cfg(dbFile);
		string value, noRet = "";
		string sqlCreate = "create table if not exists config (name text, type text, value text);";
		WHEN("no table existent") {
			THEN("create table, insert test record, read back")
				REQUIRE(cfg.queryDbSingle(sqlCreate, noRet) == true);
				string sqlInsert = "insert into config values ('test_value', 'double', '9.4');";
				REQUIRE(cfg.queryDbSingle(sqlInsert, noRet) == true);
				string sqlSelect = "select value from config where name='test_value';"; 
				REQUIRE(cfg.queryDbSingle(sqlSelect, value) == true);
				REQUIRE(value == "9.4");
		}

	}
	GIVEN("'test.sqlite' db with config table") {
		Config cfg(dbFile);
		cfg.init(); // load parameter from 'test.sqlite'
		cfg.getDouble("test_value");

}
/*
 * construct Config
 * change parameter and read back
 * check setValue, getDouble, getIn
 */
