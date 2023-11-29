// sqlite3_progect.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

//sqlite
#include "sqlite3.h"

#include <algorithm>
#include <vector>
#include <thread>
#include <future>
#include <mutex>

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

static int callback(void* NotUsed, int argc, char** argv, char** azColName)
{
    for (size_t i = 0; i < argc; i++)
    {
        std::cout << argv[i] << '\t';
    }
    std::cout << std::endl;
    return 0;
}

void create_table(sqlite3* db)
{
    const char* query = "CREATE TABLE cities(id INTEGER PRIMARY KEY, name, country_code, district, population)";
    char* zErrMsg = nullptr;
    int res = sqlite3_exec(db, query, NULL, NULL, &zErrMsg);
    if (res != SQLITE_OK)
    {
        std::cout << zErrMsg << std::endl; 
        sqlite3_free(zErrMsg);
        sqlite3_close(db); 
        exit(1);
    }
}

int main()
{
    using namespace std;

    sqlite3* db; 
    int res = sqlite3_open("world.db", &db); 
    char* zErrMsg = nullptr; 
    if (res != SQLITE_OK) 
    {
        std::cout << "isnt open!\n"; 
        sqlite3_close(db); 
        return 1; 
    } 
    create_table(db); 


    boost::asio::io_context ctx;
    boost::asio::ssl::context ssl_ctx(boost::asio::ssl::context::tls_client); 
    boost::mysql::tcp_ssl_connection conn(ctx.get_executor(), ssl_ctx); 

    boost::asio::ip::tcp::resolver resolver(ctx.get_executor());
    auto endpoints = resolver.resolve("localhost", boost::mysql::default_port_string);

    boost::mysql::handshake_params params(
        "root",					 // username
        "20051andr",             // password
        "world"					 // database
    );

    conn.connect(*endpoints.begin(), params);
    const char* sql = "SELECT COUNT(*) from City";
    boost::mysql::results result; 
    conn.execute(sql, result); 

    std::cout << result.rows()[0].at(0).as_int64() << std::endl;   
    // execution_state stores state about our operation, and must be passed to all functions
    boost::mysql::execution_state st;  

    // Writes the query request and reads the server response, but not the rows
    conn.start_execution("SELECT * FROM city", st);  

    // Reads all the returned rows, in batches.
    // st.complete() returns true once there are no more rows to read
    while (!st.complete())
    {
        // row_batch will be valid until conn performs the next network operation
        boost::mysql::rows_view row_batch = conn.read_some_rows(st); 

        for (boost::mysql::row_view post : row_batch) 
        {
  
            // Process post as required 
            const char* query = "INSERT INTO cities (name, country_code, district, population) VALUES (?, ?, ?, ?)"; 
            sqlite3_stmt* stmt;
            res = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
            
            std::string name(post.at(1).as_string());
            std::string country_code(post.at(2).as_string());
            std::string district(post.at(3).as_string());

            sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, country_code.c_str(), -1, SQLITE_STATIC); 
            sqlite3_bind_text(stmt, 3, district.c_str(), -1, SQLITE_STATIC); 
            sqlite3_bind_int(stmt, 4, post.at(4).as_int64());

            res = sqlite3_step(stmt); 
            if (res != SQLITE_DONE)  
            {
                std::cout << "wrong stmt\n";
                sqlite3_finalize(stmt);  
                conn.close(); 
                sqlite3_close(db);
                return 1;
            }
            sqlite3_finalize(stmt);
        }
    }

    res = sqlite3_exec(db, "SELECT COUNT(*) FROM cities", callback, 0, &zErrMsg);
    if (res != SQLITE_OK) 
    {
        sqlite3_free(zErrMsg); 
    }

    conn.close();
    sqlite3_close(db); 
    //////
    return 0;
}

