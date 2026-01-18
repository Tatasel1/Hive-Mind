#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

class ConfigMap
{
public:

    int row;
    int col;
    int ticks;
    int stations;
    int clients_count;

    int drones;
    int robots;
    int scooters;

    int packages;
    int spawn_freq;

    // Initializare valori default in constructor default
    ConfigMap() : row(10), col(10), ticks(100), stations(1), clients_count(2),
                  drones(1), robots(1), scooters(1), packages(10), spawn_freq(5) {}

    static ConfigMap loadFromFile(const std::string& filename);
};

// Functie care parseaza fisierul de configurare si atribuie aceste valori unui obiect Config
inline ConfigMap ConfigMap::loadFromFile(const std::string& filename)
{
    ConfigMap config;
    std::ifstream file;

    try
    {
        file.open(filename);

        if (!file.is_open())
        {
            throw std::runtime_error("Could not open file " + filename);
        }
    }
    catch ( const std::exception& e)
    {
        std::cerr << "Error: "<<e.what() << std::endl;
        return config;

    }

    std::string line;
    while (std::getline(file, line))
    {
       if (line.empty()) continue;

        std::stringstream ss(line);
        std::string key;

        ss>> key;
        if (!key.empty() && key.back() == ':')
        {
            key.pop_back();
        }

        if (key == "MAP_SIZE")
        {
            ss >> config.row >> config.col;
        }
        else if (key == "MAX_TICKS")
        {
            ss >> config.ticks;
        }
        else if (key == "MAX_STATIONS")
        {
            ss >> config.stations;
        }
        else if (key == "CLIENTS_COUNT")
        {
            ss >> config.clients_count;
        }
        else if (key == "DRONES")
        {
            ss >> config.drones;
        }
        else if (key == "ROBOTS")
        {
            ss >> config.robots;
        }
        else if (key == "SCOOTERS")
        {
            ss >> config.scooters;
        }
        else if (key == "TOTAL_PACKAGES")
        {
            ss >> config.packages;
        }
        else if (key == "SPAWN_FREQUENCY")
        {
            ss >> config.spawn_freq;
        }

    }

    file.close();
    return config;

}
