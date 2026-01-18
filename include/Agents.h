#pragma once
#include <utility>
#include <vector>
#include <string>
#include <utility>
#include <cassert>

#include "Map.h"
#include "PathFinder.h"

class Agent
{
protected:
    char symbol;
    int speed;
    int currBattery;
    int maxBattery;
    int consumption;
    int cost;
    int capacity;
    std::string state;
    /// Am adaugat o variabila care retine daca agentul are un pachet incarcat sau nu care ma va ajuta la logica de incarcare/descarcare
    bool hasPackage = false;

    /// std::vector<int> inventory;

    int posX;
    int posY;
public:
    Agent(char symb, int spd, int currBtr, int maxBtr, int cons, int cost, int cap, int startX, int startY) :
    symbol(symb),speed(spd),currBattery(currBtr), maxBattery(maxBtr), consumption(cons), cost(cost), capacity(cap), posX(startX), posY(startY) {}

    void setState(const std::string& newState){
        /// Daca agentul e deja mort nu mai poate schimba starea
        if (state == "DEAD") return;
        state = newState;
    }
    void setHasPackage(bool hasPkg){
        hasPackage = hasPkg;
    }

    /// Aici am folosit "pair" din STL pentru a returna pozitia agentului, aceasta fiind salvata in doua variabile, pentru linie si coloana.
    /// Pair imi permite sa returnez doua variabile in aceeasi functie
    std::pair<int, int> getPosition(){
        return {posX, posY};
    }
    char getSymbol() const {
        return symbol;
    }
    int getSpeed() const {
        return speed;
    }
    bool isDead() const {
        return currBattery <= 0;
    }
    int getBattery() const {
        return currBattery;
    }
    int getMaxBattery() const {
        return maxBattery;
    }
    int getConsumption() const{
        return consumption;
    }
    int getCost() const {
        return cost;
    }
    int getCapacity() const {
        return capacity;
    }
    std::string getState() const{
        return state;
    }
    bool getHasPackage() const{
        return hasPackage;
    }

    /// Functia pentru update este booleana pentru a testa direct cu ea la rularea aplicatiei.
    /// In acest mod nu va mai fi nevoie de o testare suplimentara, daca agentul e mort, se va returna false (daca functia era void, era nevoie de un if suplimentar)
    bool updateState(const Map& map){
        if (currBattery <= 0){
            state = "DEAD";
            currBattery = 0;
            return false;
        }

        if (state == "CHARGING"){
        char cell = map.getCell(posX, posY);
        if (cell == 'B' || cell == 'S') {
            int chargeAmount = maxBattery / 4;
            currBattery += chargeAmount;
            if (currBattery >= maxBattery) {
                currBattery = maxBattery;
                state = "IDLE";
            }
        } else {
            state = "IDLE";
        }
    }

    if (state == "MOVING") {
        currBattery -= consumption;
    }

    if (currBattery <= 0) {
        state = "DEAD";
        currBattery = 0;
        return false;
    }

    return true;
    }
/*
 * Am incercat sa fac o logica de inventar pentru agenti, pentru a putea livra mai multe pachete odata, acum doar le rezerva, dar am renuntat la ea deoarece nu am mai avut timp avand nevoie sa schimb aproape toata logica:(((
    /// Pentru a incarca inventarul cu pachetele pe care le are agentul (in baza)
    void loadInventory(const std::vector<int>& packages){
        inventory = packages;
    }

    /// Pentru a sterge un pachet din inventar dupa livrare
    void removePackage(int packageIdx){
        auto it = std::find(inventory.begin(), inventory.end(), packageIdx);
        if (it != inventory.end()){
            inventory.erase(it);
        }
    }

    /// Pentru a verifica daca un anumit pachet se afla in inventar
    bool hasPackageInventory(int pIdx) const {
        for (int id : inventory){
            if (id == pIdx){
                return true;
            }

        }
        return false;
    }
    */
    virtual void move(int posX, int posY, const Map& map) = 0;

    virtual ~Agent() = default;
};
