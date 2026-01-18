#pragma once
#include "Agents.h"

class Robot : public Agent
{
public:
    Robot(int startX, int startY) : Agent('R', 1, 300, 300, 2, 1, 4 , startX, startY) {}

    void move(int targetX, int targetY, const Map& map) override{
        /// Robotul si scuterul trebuie sa ocoleasca zidurile, asa ca am facut o functie BFS asemanatoare cu cea care verifica harta, care returneaza urmatoarea pozitie pe care se va muta agentul
        std::pair<int, int> nextPos = PathFinder::nextMove(posX, posY, targetX, targetY, map);

        posX = nextPos.first;
        posY = nextPos.second;
    }
};