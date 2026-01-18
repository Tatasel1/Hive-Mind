#include <iostream>
#include <queue>
#include <utility>
#include <cassert>

#include "Map.h"

std::vector<std::pair<int, int>> Map:: getDropOffPoints(const Map* map){
    std::vector<std::pair<int, int>> points;
    for (int i = 0; i < map->getRow(); i++) {
        for (int j = 0; j < map->getCol(); j++) {
            char c = map->getCell(i, j);
            if (c == 'D' || c == 'S') { // Destinatie sau Statie
                points.push_back({i, j});
            }
        }
    }
    return points;
}
void Map:: setCell(int r, int c, char value) {
    /// Assert: nu e voie sa fie celule in afara hartii (la fel in getCell)
    /// assert(r >= 0 && r < row);
    /// assert(c >= 0 && c < col);

    if (r >= 0 && r < row && c >= 0 && c < col) {
        map[r][c] = value;
        if (value == 'B') {
            basePosX = r;
            basePosY = c;
        }
    }
}

char Map:: getCell(int r, int c) const {
    /// assert(r >= 0 && r < row);
    /// assert(c >= 0 && c < col);

    if (r >= 0 && r < row && c >= 0 && c < col) {
        return map[r][c];
    }
    return '\0'; /// Se returneaza caracter null daca coordonatele sunt invalide
}
int Map:: getRow() const {
    return row;
}
int Map:: getCol() const {
    return col;
}
void Map:: printMap() {
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            std::cout << map[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

/// Functia care verifica daca o harta este valida prin algoritmul BFS
bool isValidMap(const Map* map){

    if (map -> basePosX == -1 || map -> basePosY == -1){
        return false; // Nu exista baza
    }

    /// Verific daca exista destinatii sau statii
    int nrTargets = 0;
    for (const auto& row : map->map){
        for (const auto& cell : row){
            if (cell == 'D' || cell == 'S'){
                nrTargets++;
            }
        }
    }

    if (nrTargets == 0){
        return false; /// Nu exista destinatii sau statii
    }

    /// Definesc vectori de directie pentru parcurgerea vecinilor
    int dRow [] = {-1, 0 , 1, 0};
    int dCol [] = {0, 1, 0, -1};

    /// Initializez o coada pentru a putea salva pozitia celulelor parcurse prin BFS
    /// Dau enqueue la pozitia bazei
    std::queue<std::pair<int, int>> q;
    q.push({map->basePosX, map->basePosY});

    /// Initializez o matrice de booleane care sa tina evidenta celulelor vizitate de pe harta
    std::vector<std::vector<bool>> visited (map->row, std::vector<bool>(map->col, false));
    visited[map->basePosX][map->basePosY] = true;

    int foundTargets = 0;

    /// Parcurg harta cat timp coada nu e goala
    while (!q.empty()){
        auto current = q.front();
        q.pop();

        int x = current.first;
        int y = current.second;

        /// Salvez numarul de statii sau destinatii gasite intr-o variabila
        char cell = map -> map[x][y];
        if (cell == 'D' || cell == 'S'){
            foundTargets++;
        }

        /// Parcurg vecinii celulei curente, ii marchez ca vizitati si ii adaug in coada daca nu sunt zid
        for (int i = 0; i < 4; i++){
            int newX = x + dRow[i];
            int newY = y + dCol[i];

            if (newX >= 0 && newX < map->row && newY >= 0 && newY < map->col){
                char neighborCell = map -> map[newX][newY];

                /// Verific daca vecinul este accesibil si nu a fost vizitat
                if (neighborCell != '#' && !visited[newX][newY]){
                    visited[newX][newY] = true;
                    q.push({newX, newY});
                }
            }
        }
    }

    /// Daca numarul de statii si destinatii gasite este egal cu cel din harta, atunci harta e valida
    return foundTargets == nrTargets;
}
