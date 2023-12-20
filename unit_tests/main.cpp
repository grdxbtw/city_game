#define BOOST_TEST_MODULE city_game
#define TEST 

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "../db_cities_game/file_source.h"
#include "../db_cities_game/db_source.h"
#include "../db_cities_game/city_game.h"

#define DB_NAME "world"   
#define DB_LOGIN "root"  
#define DB_PASS "20051andr"  
#define DB_ADRESS "localhost"  

namespace test = boost::unit_test;
namespace tt = boost::test_tools;
namespace utf = boost::unit_test;

const char* cities[]{
	"Kabul",
	"Qandahar",
	"Herat",
	"Amsterdam",
	"Rotterdam",
	"Haag",
	"Utrecht",
	"Eindhoven",
	"Tilburg",
	"Groningen",
	"Breda" };

BOOST_AUTO_TEST_SUITE(f_source) 
BOOST_AUTO_TEST_CASE(file_source_check_city) 
{
	const std::string filename("test_file_source.txt");
	file_source fc(filename);
	BOOST_TEST(!fc.check_city("lxewlxwe"));
	BOOST_TEST(fc.check_city("Rome"));
}

BOOST_DATA_TEST_CASE(dataset_check_city, boost::unit_test::data::make(cities))  
{
	const std::string filename("test_file_source.txt"); 
	file_source fc(filename); 
	BOOST_TEST(fc.check_city(sample));   
}

BOOST_AUTO_TEST_CASE(file_source_get_city) 
{
	const std::string filename("test_file_source.txt");
	file_source fc(filename);

	std::vector<std::string> existed{ 
		"Kabul",
			"Qandahar",
			"Herat",
			"Amsterdam",
			"Rotterdam",
			"Utrecht",
			"Eindhoven",
			"Tilburg",
			"Groningen",
			"Breda" }; 

	BOOST_TEST(fc.get_city('A', existed).empty());
	existed.clear();
	BOOST_TEST(fc.get_city('A', existed) == "Amsterdam");
	BOOST_TEST(fc.get_city('H', existed) == "Herat");
}

BOOST_AUTO_TEST_SUITE_END()

//////////////////////////////////////

BOOST_AUTO_TEST_SUITE(db_source_h) 
BOOST_AUTO_TEST_CASE(db_source_check_city)
{
	db_source db(DB_ADRESS, DB_NAME, DB_LOGIN, DB_PASS);
	BOOST_TEST(!db.check_city("lxewlxwe")); 
	BOOST_TEST(!db.check_city("Rome")); 
}
BOOST_DATA_TEST_CASE(db_source_check_city_2, boost::unit_test::data::make(cities)) 
{
	const std::string filename("test_file_source.txt");
	file_source fc(filename);
	BOOST_TEST(fc.check_city(sample));
}
BOOST_AUTO_TEST_CASE(db_source_get_city) 
{
	db_source db(DB_ADRESS, DB_NAME, DB_LOGIN, DB_PASS); 
	std::vector<std::string> existed;
	BOOST_TEST(!db.get_city('Z', existed).empty(), "is empty"); 
}

BOOST_AUTO_TEST_SUITE_END()

//////////////////////////////////////

BOOST_AUTO_TEST_SUITE(city_game_h)  
BOOST_AUTO_TEST_CASE(city_load)
{
	std::vector<player> pl;
	city_game game(pl); 
	BOOST_TEST(game.load("statistic.xml"));
}
BOOST_AUTO_TEST_SUITE_END()