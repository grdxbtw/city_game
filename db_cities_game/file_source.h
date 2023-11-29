#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

//.h
#include "data_source.h"

class file_source: public data_source
{
public:
	file_source(const std::string& filename)
	{
		cities.reserve(4100);
		load(filename);
	}
	void load(const std::string& filename)
	{
		std::fstream fin(filename); 
		if (fin.is_open())
		{
			while (!fin.eof()) 
			{
				std::string city_name; 
				std::getline(fin, city_name); 
				cities.push_back(city_name); 
			}
		}
		fin.close(); 
	}

	std::string get_city(const char firstL, const std::vector<std::string>& existed) override
	{
		for (const std::string& city: cities)
		{
			bool exist = std::any_of(existed.begin(), existed.end(), [&city](const auto& str) {return city == str; }); 
			if (!exist) 
			{			
				if ( city[0] == firstL)
					return city;
			}
		}
		return std::string();
	}

	bool check_city(const std::string& city) override
	{
		return std::find(cities.begin(), cities.end(), city) != cities.end();
	}

private:
	std::vector<std::string> cities;

};

data_source* make_data_source(const std::string& filename)
{
	return new file_source(filename);
}


