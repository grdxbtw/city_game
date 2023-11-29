#define BOOST_TEST_MODULE city_game
#include <boost/test/unit_test.hpp>
#include "file_source.h"
#include "db_source.h"
#include "city_game.h"

#define DB_NAME "world"   
#define DB_LOGIN "root"  
#define DB_PASS "20051andr"  
#define DB_ADRESS "localhost"  

namespace test = boost::unit_test;

BOOST_AUTO_TEST_SUITE(f_source) 

BOOST_AUTO_TEST_CASE(file_source_check_city)
{
	const std::string filename("test_file_source.txt");
	file_source fc(filename);
	BOOST_TEST(!fc.check_city("lxewlxwe"));
	const char* cities[] { 
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

	for (const auto el: cities) 
	{
		BOOST_TEST(fc.check_city(el), "not found city: " << el); 
	}
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

	for (const auto el : cities)
	{
		BOOST_TEST(db.check_city(el), "not found city: " << el); 
	}
}

BOOST_AUTO_TEST_CASE(db_source_get_city) 
{
	db_source db(DB_ADRESS, DB_NAME, DB_LOGIN, DB_PASS); 
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

	BOOST_TEST(!db.get_city('A', existed).empty());  
	existed.clear();
	BOOST_TEST(!db.get_city('A', existed).empty());  

}

BOOST_AUTO_TEST_SUITE_END()

//////////////////////////////////////


