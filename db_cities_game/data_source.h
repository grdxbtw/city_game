#pragma once
#include <string>
#include <vector>

class data_source
{
public:
	virtual std::string get_city(const char firstL, const std::vector<std::string>& existed) = 0;
	virtual bool check_city(const std::string&) = 0;

private:
	 
};

