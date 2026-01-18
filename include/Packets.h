#pragma once
#include <utility>

/// Clasa care reprezinta un pachet ce trebuie livrat de catre agenti
class Package
{
    int id;
    int targetX;
    int targetY;
    int destX;
    int destY;
    int reward;
    int duration;
    int spawnTick;
public:
    Package(int id_, int targX, int targY, int desX, int desY,int rew, int dur, int spawnTick) :
    id(id_), targetX(targX), targetY(targY), destX(desX), destY(desY), reward(rew), duration(dur), spawnTick(spawnTick) {};

    std::pair<int, int> getDestination() const {
        return {targetX, targetY};
    }

    std::pair<int, int> getCurrentPosition() const {
        return {destX, destY};
    }

    int getReward() const {
        return reward;
    }

    bool isLate(int currTick) const {
        return currTick > (spawnTick + duration);
    }

    int getSpawnTick() const{
        return spawnTick;
    }

    int getDuration() const{
        return duration;
    }

};