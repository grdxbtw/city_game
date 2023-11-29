#include "city_game.h"

#if defined DB 
#include "db_source.h"   
#elif defined SQLITE3
#include "sqlite_source.h"
#else
#include "file_source.h"  
#endif // DB

namespace pt = boost::property_tree;

data_source* make_data_source();
std::string trim(const std::string& city);

city_game::city_game()
{
    existed.reserve(100);
    ds = make_data_source(); 
}

city_game::city_game(const std::string& filename)
{
    load(filename);
    ds = make_data_source(); 
}

city_game::city_game(const std::vector<player>& players):players(players)
{
    existed.reserve(100);
    ds = make_data_source();  
}

bool city_game::load(const std::string& filename)
{
    pt::ptree tree; 
    pt::read_xml(filename, tree);  
    for (const pt::ptree::value_type& v : tree.get_child("Game.Players"))
    {
        player p; 
        p.name = v.second.get("name",""); 
        p.score = v.second.get("score", 0); 
        p.isPc = v.second.get<bool>("<xmlattr>.type");  
        players.push_back(p); 
    }

    if (players.empty())
        return false; 

    existed.reserve(100); 
    for (const pt::ptree::value_type& v : tree.get_child("Game.Steps"))
    {
        std::string city = v.second.data(); 
        if(city != "-") 
            existed.push_back(city);  
    } 
    return true;
}

void city_game::save(const std::string& filename) 
{
    for (const auto& el: players)
    {
        pt::ptree node;
        node.put("<xmlattr>.type",static_cast<int>( el.isPc));
        node.put("name",el.name);
        node.put("score", el.score); 
        head.add_child("Game.Players.player", node); 
    }

    for (size_t i = 0; i < existed.size(); i++)
    { 
        pt::ptree node;    
        node.put("<xmlattr>.player_id", i);      
        node.put_value(existed[i]); 
        head.add_child("Game.Steps.city", node);   
    }

    pt::write_xml(filename, head);  
}

std::future<void> city_game::async_save(const std::string& filename)
{
    std::packaged_task<void()> task(std::bind(&city_game::save, this, filename));
    std::future<void> result = task.get_future();
    std::thread thr(std::move(task));  
    thr.detach();
    return result;
}

void city_game::start(const char startL)
{
    char firstL = startL; 
    while (true)
    {
        for (size_t i = 0; i < players.size(); i++)
        {   
            std::string city;
            firstL = std::toupper(static_cast<unsigned char>(firstL));
            std::cout << "step of player " << i + 1 << "#\n";

            if (players[i].isPc)  
            {
                step_pc(city, firstL);
                std::cout << city << std::endl;  
            }
            else
            {
                step_human(city,firstL);
            }

            if (city == "q")
                return;

            if (city == "-")
            {
                std::cout << "i'm skipping(\n";
            }
            else
            {
                firstL = city.back(); 
                players[i].score++; 
                existed.push_back(city); 
            }

            if (players[i].score >= 100)   
            {
                std::cout << "player #" << i + 1 << "won!\n";
                return;
            }

        }
    }
}

void city_game::step_human(std::string& city,  const char firstL)
{
    std::cin.clear();
    std::getline(std::cin, city); 
    city = trim(city); 

    while ((city.empty() || !check_city(city, firstL) || !is_real_city(city)) && !is_cntrl(city))
    {
        std::cout << "try again!\n"; 
        std::getline(std::cin, city);
        city = trim(city); 

    }
}

void city_game::step_pc(std::string& city, const char firstL)
{
    if (!ds)
    {
        city = "-";
        return;
    }

    city = ds->get_city(firstL, existed);
    if (city.empty())
        city = "-";
}

bool city_game::check_city(const std::string& city, const char firstL)
{
    bool exist = std::any_of(existed.begin(), existed.end(), [&city](const auto& str) {return city == str; });
    if (!exist)
    {
        if (city[0] == firstL)
            return true;
    }
    return false;
}

bool city_game::is_real_city(const std::string& city) const
{
    if (!ds)
        return true;

    return ds->check_city(city);
}

bool city_game::is_cntrl(const std::string& city) const
{
    return city == "q" || city == "-";
}


data_source* make_data_source()
{
#ifdef DB
    return new db_source(DB_ADRESS, DB_NAME, DB_LOGIN, DB_PASS);
#elif defined SQLITE3
    return new sqlite_source(SQLITE3);
#else
    return new file_source(FILE_SOURCE);
#endif // DB 
}

std::string trim(const std::string& city)
{
    size_t pos = city.find_first_not_of(" \t");
    if (pos == std::string::npos)
        return std::string(); 
    size_t last = city.find_last_not_of(" \t");
    return city.substr(pos, last - pos + 1);
}