#pragma once

#include "Agents.h"
class Drone : public Agent
{
public:
    Drone(int startX, int startY) : Agent('^', 3,100, 100, 10, 15, 1, startX, startY) {}
    void move(int targetX, int targetY, const Map& map) override{
        /// For ul este numarul de celule parcurs in fiecare tick, drona care poate trece prin pereti se misca 3 celule pe tick
        for (int i = 0; i < speed; i++){
            /// Un algoritm simplu de calculare a distantei
            if (posX == targetX && posY == targetY) break;

            int nextX = posX;
            int nextY = posY;

            if (posX < targetX) nextX++;
            else if (posX > targetX) nextX--;

            if (posY < targetY) nextY++;
            else if (posY > targetY) nextY--;

            if (nextX >= 0 && nextX < map.getRow() && nextY >= 0 && nextY < map.getCol()){
                posX = nextX;
                posY = nextY;
            }
        }

    }

};
