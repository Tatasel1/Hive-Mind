#pragma once
#include <queue>
#include <vector>
#include <utility>

#include "Map.h"

class PathFinder
{
public:
    static std::pair<int, int> nextMove(int startX, int startY,int targetX, int targetY, const Map& map){

        int rows = map.getRow();
        int cols = map.getCol();
        int dRow[] = {-1, 0 , 1 , 0};
        int dCol[] = {0, 1 , 0 , -1};

        if (startX == targetX && startY == targetY){
            return {startX, startY};
        }

        std::queue<std::pair<int,int>> q;
        q.push({startX, startY});

        std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
        visited[startX][startY] = true;

        /// Aici creez o matrice de parinti, pentru a putea retine clar drumul care a fost parcurs
        std::vector<std::vector<std::pair<int,int>>> path (rows, std::vector<std::pair<int,int>>(cols, {-1, -1}));
        bool found = false;

        while (!q.empty()){
            auto curr = q.front();
            q.pop();

            if (curr.first == targetX && curr.second == targetY){
                found = true;
                break;
            }

            for (int i = 0; i < 4; ++i){
                int newX = curr.first + dRow[i];
                int newY = curr.second + dCol[i];

                if (newX >=0 && newX < rows && newY >= 0 && newY < cols){
                    char cell = map.getCell(newX, newY);

                    if (cell != '#' && !visited[newX][newY]){
                        visited[newX][newY] = true;
                        q.push({newX, newY});

                        path[newX][newY] = {curr.first, curr.second};
                    }
                }
            }
        }

        if (!found){
            return {startX, startY};
        }

        std::pair<int, int> curr = {targetX, targetY};
        while (true){
            auto p = path[curr.first][curr.second];

            if (p.first == startX && p.second == startY){
                return curr;
            }

            curr = p;

            if (curr.first == -1) return {startX, startY};
        }
    }

    /// Am facut o functie care calculeaza distanta intre doua puncte din matrice.
    /// Aceasta se numeste distanta Manhattan si returneaza suma diferentelor absolute dintre coordonatele celor doua puncte.
    static int manhattanDistance(int x1, int y1, int x2, int y2){
        int res = std::abs(x1 - x2) + std::abs(y1 - y2);

        /// Assert: res nu poate fi negativ
        /// assert(res >= 0);
        return res;
    }

    /// O functie care gaseste cea mai apropiata statie de incarcare 'S' pentru un agent aflat la pozitia (x, y)
    static std::pair<int, int> findNearestStation(int x, int y, Map* map, char agentSymbol){
        /// Folosesc manhattan Distance pentru Drona
        if (agentSymbol == '^'){
            std::pair<int, int> bestStation = {-1, -1};
            int minDistance = INT_MAX;

            for(int r = 0; r < map->getRow(); r++) {
                for(int c = 0; c < map->getCol(); c++) {
                    if (map->getCell(r, c) == 'S' || map->getCell(r, c) == 'B') {
                        int dist = manhattanDistance(x, y, r, c);
                        if (dist < minDistance) {
                            minDistance = dist;
                            bestStation = {r, c};
                        }
                    }
                }
            }
            return bestStation;
        }

        /// Pentru robot si scuter folosesc BFS, rescris deoarece trebuie sa se opreasca la prima statie gasita
        int rows = map->getRow();
        int cols = map->getCol();

        std::queue<std::pair<int, int>> q;
        q.push({x, y});

        std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
        visited[x][y] = true;

        int dRow[] = {-1, 0, 1, 0};
        int dCol[] = {0, 1, 0, -1};

        while (!q.empty()) {
            auto curr = q.front();
            q.pop();

            char cell = map->getCell(curr.first, curr.second);

            if (cell == 'S' || cell == 'B') {
                return {curr.first, curr.second};
            }

            for (int i = 0; i < 4; ++i) {
                int nx = curr.first + dRow[i];
                int ny = curr.second + dCol[i];

                if (nx >= 0 && nx < rows && ny >= 0 && ny < cols) {
                    char nextCell = map->getCell(nx, ny);
                    if (nextCell != '#' && !visited[nx][ny]) {
                        visited[nx][ny] = true;
                        q.push({nx, ny});
                    }
                }
            }
        }

        return {-1, -1};

    }

    /// Am mai facut o functie pe baza algoritmului BFS care sa returneze int, pentru pathfinding-ul robotilor si al scuterelor deoarece manhattan distance ar trece prin pereti
    static int realDistance(int startX, int startY, int targetX, int targetY, const Map& map){
        int rows = map.getRow();
        int cols = map.getCol();
        int dRow[] = {-1, 0 , 1 , 0};
        int dCol[] = {0, 1 , 0 , -1};

        if (startX == targetX && startY == targetY){
            return 0;
        }

        std::queue<std::pair<int,int>> q;
        q.push({startX, startY});

        std::vector<std::vector<int>> distance (rows, std::vector<int>(cols, -1));
        distance[startX][startY] = 0;

        while (!q.empty()){
            auto curr = q.front();
            q.pop();

            if (curr.first == targetX && curr.second == targetY){
                return distance[curr.first][curr.second];
            }

            for (int i = 0; i < 4; ++i){
                int newX = curr.first + dRow[i];
                int newY = curr.second + dCol[i];

                if (newX >=0 && newX < rows && newY >= 0 && newY < cols){
                    char cell = map.getCell(newX, newY);

                    if (cell != '#' && distance[newX][newY] == -1){
                        distance[newX][newY] = distance[curr.first][curr.second] + 1; /// Pozitia anterioara +1
                        q.push({newX, newY});
                    }
                }
            }
        }

        return -1; /// Nu s-a gasit drum
    }
};

