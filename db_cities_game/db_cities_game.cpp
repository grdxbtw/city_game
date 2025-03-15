// db_cities_game.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//mySql and openSSl
#include <boost/mysql/error_with_diagnostics.hpp>
#include <boost/mysql/handshake_params.hpp>
#include <boost/mysql/results.hpp>
#include <boost/mysql/tcp_ssl.hpp>

// for mySql asynchronus i/o context
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/system/system_error.hpp>

//log
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>

// for xml
#include <boost/property_tree/ptree.hpp> 
#include <boost/property_tree/xml_parser.hpp> 

//cpp
#include <iostream>
#include <string>
#include<vector>

// city game
#include "city_game.h"

namespace pt = boost::property_tree;
namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;

using namespace logging::trivial;


//[ example_tutorial_file_advanced
void init()
{
	logging::add_file_log
	(
		keywords::file_name = "info_game.log",                                         /*< file name pattern >*/ 
		keywords::rotation_size = 10 * 1024 * 1024,                                    /*< rotate files every 10 MiB... >*/
		keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), /*< ...or at midnight >*/
		keywords::format = "[%TimeStamp%]: %Message%"                                 /*< log record format >*/ 
	);

	logging::core::get()->set_filter 
	(
		logging::trivial::severity >= logging::trivial::info 
	);

}

int main()
{
    std::cout << "Hello World!\n";

	src::severity_logger<severity_level> lg; 
	init(); 

	player p3,p2, p1;
	p1.name = "PC"; 
	p1.isPc = true; 
	p2.name = "Anastasiia";
	p3.name = "Andrey";

	std::vector<player> players{p1,p2};  

	city_game game(players); 

	game.start('a');
	auto res = game.async_save("statistic.xml");    


	std::cout << "USE World DB!\n";

	// The execution context, required to run I/O operations.
	boost::asio::io_context ctx;

	// The SSL context, required to establish TLS connections.
	// The default SSL options are good enough for us at this point.
	boost::asio::ssl::context ssl_ctx(boost::asio::ssl::context::tls_client);

	// Represents a connection to the MySQL server.
	boost::mysql::tcp_ssl_connection conn(ctx.get_executor(), ssl_ctx);

	// Resolve the hostname to get a collection of endpoints
	boost::asio::ip::tcp::resolver resolver(ctx.get_executor());
	auto endpoints = resolver.resolve("localhost", boost::mysql::default_port_string);

	// The username, password and database to use
	boost::mysql::handshake_params params(
		"root",					// username
		"pass",             // password
		"world"					// database
	);

	// Connect to the server using the first endpoint returned by the resolver
	conn.connect(*endpoints.begin(), params);

	// Issue the SQL query to the server
	const char* sql = "SELECT name from City WHERE CountryCode='UKR' LIMIT 3"; 
	boost::mysql::results result;
	conn.execute(sql, result);

	// Print the first field in the first row
	std::cout << result.rows().size() << std::endl;
	for (boost::mysql::row_view city : result.rows())
	{
		std::cout << city.at(0).as_string() << std::endl;
	}

	// Close the connection
	conn.close();

	res.wait(); // wait statistic to xml

    return 0;
}

