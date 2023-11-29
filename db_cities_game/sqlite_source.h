#pragma once

#include <algorithm>
#include "data_source.h"
#include "../../libs_cpp/sqlite/sqlite3.h"

class sqlite_source: public data_source 
{
public:
	sqlite_source(const char* db_name) 
	{
		int res = sqlite3_open(db_name, &db);  
		if (res != SQLITE_OK)
		{
			//
		}
	}
	~sqlite_source()
	{
		sqlite3_close(db);
	}
	std::string get_city(const char firstL, const std::vector<std::string>& existed) override
	{
		const char* query = "SELECT name FROM cities WHERE name LIKE ? ORDER BY RANDOM() LIMIT 10";
		sqlite3_stmt* stmt;
		int res = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr); 
		if (res != SQLITE_OK)
		{
			return std::string();
		}
		std::string q(" %"); 
		q[0] = firstL;
		sqlite3_bind_text(stmt, 1, q.c_str(), -1, SQLITE_STATIC); 
		
		while ((res = sqlite3_step(stmt)) == SQLITE_ROW) 
		{
			const char* city = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
			bool exist = std::any_of(existed.begin(), existed.end(), [city](const std::string& el) {return el == city; });
			if (!exist)
			{
				std::string c(city);
				sqlite3_finalize(stmt);
				return c; // --> change here 
			}
		}
		sqlite3_finalize(stmt);  
		return std::string();
	}

	bool check_city(const std::string& city) override
	{
		const char* query = "SELECT name FROM cities WHERE name = ? LIMIT 1"; 
		sqlite3_stmt* stmt; 
		int res = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr); 
		if (res != SQLITE_OK) 
		{
			sqlite3_finalize(stmt);  
			return false;
		}
		sqlite3_bind_text(stmt, 1, city.c_str(), -1, SQLITE_STATIC); 

		if((res = sqlite3_step(stmt)) == SQLITE_ROW)
		{
			sqlite3_finalize(stmt); 
			return true; 
		}
		sqlite3_finalize(stmt);  
		return false;
	}

private:
	sqlite3* db;

};

data_source* make_data_source(const char* db_name)
{
	return new sqlite_source(db_name);
}
