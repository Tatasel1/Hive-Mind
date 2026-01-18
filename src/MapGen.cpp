#include "Map.h"
#include<fstream>
#include<iostream>

/// Functia care genereaza o harta procedural
Map* ProceduralMapGenerator::generate(){

    int rows = config.row;
    int cols = config.col;
    int attempts = 0;

    /// Incerc sa generez o harta valida de maxim 1000 de ori
    while (attempts < 1000){
        attempts++;
        Map* map = new Map(rows, cols);

        /// Aleg o pozitie random pentru baza
        int x = std::rand() % rows;
        int y = std::rand() % cols;
        map->setCell(x, y, 'B');

        /// Am decis ca peretii sa acopere o zona de aproximativ 25% din harta, astfel incat sa nu fie nici prea usoara nici prea grea de parcurs
        int wall = (rows * cols) / 4;
        for (int i = 0; i < wall; ++i){
            /// Pozitii random pentru pereti
            int wx = std::rand() % rows;
            int wy = std::rand() % cols;

            if (wx != x || wy != y){
               if (map->getCell(wx,wy) == '.'){
                   map->setCell(wx, wy, '#');
               }
            }
        }

        ///Generarea clientilor, un numar random, maxim cel din config
        int nrclients_gen = rand() % (config.clients_count + 1);
        for (int i = 0; i < nrclients_gen; i++){
            int cx = std::rand() % rows;
            int cy = std::rand() % cols;
            if (map->getCell(cx,cy) == '.'){
                map->setCell(cx, cy, 'D'); // Set client
            }
            else{
                i--;
            }
        }

        /// Generarea statiilor, un numar random, cel din config
        int nrstations_gen = rand() % (config.stations + 1);
        for (int i = 0; i < nrstations_gen; i++){
            int sx = std::rand() % rows;
            int sy = std::rand() % cols;
            if (map->getCell(sx,sy) == '.'){
                map->setCell(sx, sy, 'S');
            }
            else{
                i--;
            }
        }

        /// Testez daca harta e valida
        if (isValidMap(map)){
            return map;
        } else {
            delete map;

        }

    }

    return nullptr;
}

/// Functia care incarca o harta dintr-un fisier text
Map* FileMapLoader::generate(){
    std::ifstream file;
    std::string line;
    std::vector<std::string> lines;

    try{
        file.open(filename);

        if (!file.is_open()){
            throw std::runtime_error("File not found");
        }

        /// Citesc fiecare linie din fisier si o adaug intr-un vector de stringuri
        while (std::getline(file, line)){
            if (!line.empty() && line.back() == '\r')  line.pop_back();

            if (!line.empty()) lines.push_back(line);
        }
        file.close();

        if (lines.empty()){
            throw std::runtime_error("Empty map file");
        }

        int rows = lines.size();
        int cols = lines[0].size();

        /// Creez harta si o populez cu caracterele din fisier
        Map* map = new Map(rows, cols);
        for (int i = 0; i < rows; ++i){
            if (lines[i].size() != cols){
                delete map;
                throw std::runtime_error("Wrong map size");

            }

            for (int j = 0; j < cols; ++j){
                map->setCell(i,j,lines[i][j]);
            }
        }

        /// Testez daca harta e valida
        if (isValidMap(map)){
            std::cout<<"Map is valid"<<std::endl;
            return map;
        }
        else{
            delete map;
            throw std::runtime_error("Map Invalid");
        }
    }
    catch (const std::exception &e){
        std::cerr<<"Error: " << e.what() <<std::endl;
        return nullptr;
    }
}
