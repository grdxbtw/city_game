#pragma once
#include "data_source.h"
#include <algorithm>

//mySql and openSSl
#include <boost/mysql/error_with_diagnostics.hpp>
#include <boost/mysql/handshake_params.hpp>
#include <boost/mysql/results.hpp>
#include <boost/mysql/tcp_ssl.hpp>

//for mySql asynchronus i/o context
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/system/system_error.hpp>

class db_source: public data_source 
{
public:
	db_source(const std::string& adress, const std::string& db_name, const std::string& login, const std::string& pass) 
		:ssl_ctx(boost::asio::ssl::context::tls_client), conn(ctx.get_executor(), ssl_ctx)
	{
		connect(adress, db_name, login, pass); 

		/*const char* q = "CREATE TEMPORARY TABLE existed ( name VARCHAR(255) );"; 
		boost::mysql::results r; 
		conn.execute(q, r); */  // --> Temp table to save here existed cities
	}

	~db_source()
	{
		conn.close();
	}

	void connect(const std::string& adress, const std::string& db_name, const std::string& login, const std::string& pass)
	{
		// Resolve the hostname to get a collection of endpoints
		boost::asio::ip::tcp::resolver resolver(ctx.get_executor());
		auto endpoints = resolver.resolve(adress, boost::mysql::default_port_string);

		boost::mysql::handshake_params params(
			login,					// username 
			pass,					// password
			db_name					// database 
		);

		conn.connect(*endpoints.begin(), params);
	}

	std::string get_city(const char firstL,const std::vector<std::string>& existed) override 
	{
		//stmt
		boost::mysql::statement stmt = conn.prepare_statement(  
			"SELECT name FROM city WHERE name LIKE ? ORDER BY RAND() LIMIT 10" // --> 'firstL'
		);
		boost::mysql::results result; 
		std::string query(" %");
		query[0] = firstL; 

		conn.execute(stmt.bind(query), result); 
		for (boost::mysql::row_view city : result.rows()) 
		{
			std::string city_name = city.at(0).as_string();
			bool exist = std::any_of(existed.begin(), existed.end(), [&city_name](const std::string str) {return city_name == str; });
			if (!exist)
			{
				return city_name; 
			}
		}
		return std::string();
	}

	bool check_city(const std::string& city) override
	{
		boost::mysql::statement stmt = conn.prepare_statement(
			"SELECT name FROM city WHERE name = ? LIMIT 1"
		);
		boost::mysql::results result;
		conn.execute(stmt.bind(city), result);
		return !result.rows().empty();
	}

private:

	boost::asio::io_context ctx;
	boost::asio::ssl::context ssl_ctx;
	boost::mysql::tcp_ssl_connection conn;

};

data_source* make_data_source(const std::string& adress, const std::string& db_name, const std::string& login, const std::string& pass)
{
	return new db_source(adress, db_name, login, pass);
}