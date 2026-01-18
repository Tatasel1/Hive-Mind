#pragma once
#include "Agents.h"

class Scooter : public Agent
{
public:
    Scooter(int startX, int startY) : Agent('S', 2, 200, 200, 5, 4, 2, startX, startY) {}

    void move(int targetX, int targetY, const Map& map) override{
        for (int i = 0; i < speed; i++){

            if (posX == targetX && posY == targetY) break;

            std::pair<int, int> nextPos = PathFinder::nextMove(posX, posY, targetX, targetY, map);

            if (nextPos.first == posX && nextPos.second == posY) break;

            posX = nextPos.first;
            posY = nextPos.second;
        }

    }
};