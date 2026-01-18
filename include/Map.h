#pragma once

#include<vector>
#include<string>
#include<cstdlib>
#include<ctime>

#include "ConfigMap.h"

class Map
{
    int row;
    int col;
    std::vector<std::vector<char>> map;

    int basePosX;
    int basePosY;

public:
    Map(int r, int c) : row(r), col(c), basePosX(-1), basePosY(-1) {
        map.assign(row, std::vector<char>(col, '.'));
    }

    void setCell(int row, int col, char c);
    char getCell(int row, int col) const;
    int getRow() const;
    int getCol() const;
    void printMap();
    std::vector<std::pair<int, int>> getDropOffPoints(const Map* map);
    friend bool isValidMap(const Map* map);
};

class IMapGenerator {
public:
    virtual Map* generate() = 0;
    virtual ~IMapGenerator() = default;
};

class FileMapLoader : public IMapGenerator {
    std::string filename;
public:
    FileMapLoader(const std::string& file) : filename(file) {}
    Map* generate() override;
};

class ProceduralMapGenerator : public IMapGenerator
{
    const ConfigMap& config;
    int randNr;
public:
    ProceduralMapGenerator(const ConfigMap& cfg) : config(cfg)
    {
        srand(time(NULL));
    }

    Map* generate() override;
};