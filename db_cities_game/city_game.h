#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <future>
#include <thread>

//h
#include "Player.h"
#include "data_source.h"

//ptree
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#define DB 
//
#define FILE_SOURCE "cities.txt"
#define SQLITE3 "world.db"
//
#define DB_NAME "world" 
#define DB_LOGIN "root" 
#define DB_PASS "20051andr" 
#define DB_ADRESS "localhost" 

class city_game
{
private:
	std::vector<player> players;
	std::vector<std::string> existed;
	boost::property_tree::ptree head;
	data_source* ds;

public:
	city_game();
	city_game(const std::string& filename);
	city_game(const std::vector<player>& players);
	~city_game() 
	{
		delete ds;
	}
	bool load(const std::string& filename); 
	void save(const std::string& filename); 
	std::future<void> async_save(const std::string& filename);

	void start(const char); 
private:
	void step_human(std::string& city, const char firstL);  
	void step_pc(std::string& city, const char firstL);  
	bool check_city(const std::string& city, const char); 
	bool is_real_city(const std::string& city) const;
	bool is_cntrl(const std::string& city) const;
};

