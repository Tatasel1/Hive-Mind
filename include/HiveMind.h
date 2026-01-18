#pragma once
#include <vector>
#include <string>
#include <cmath>
#include <algorithm> /// Pentru sort
#include <iostream>
#include <cassert>

#include "Agents.h"
#include "Map.h"
#include "Offers.h"
#include "Packets.h"
#include "PathFinder.h"

class HiveMind
{
private:
    static HiveMind* instance;
    HiveMind() {}
public:

    static HiveMind* getInstance(){
        if (instance == nullptr){
            instance = new HiveMind();
        }
        return instance;
    }

    /// Functia care aloca pachete agentilor astfel incat profitul sa fie maximizat
    /// Am creat o matrice de asignari astfel incat sa ii pot atribui unui agent cate pachete poate duce
    void assignPackages(std::vector<Agent*>& agents, const std::vector<Package>& packages,
                        std::vector<std::vector<int>>& agentAssignments, std::vector<bool>& taken, const std::vector<bool>& delivered,
                        const std::vector<bool>& lost, int tick, std::pair<int, int> basePos, Map* map){
        std::vector<int> availablePackages;

        for (int i = 0; i < packages.size(); i++){
            if (!taken[i] && !delivered[i] && !lost[i]){
                availablePackages.push_back(i);
            }

        }

        std::vector<Offer> availableOffers;

        for (size_t i = 0; i < agents.size(); i++){
            Agent* agent = agents[i];

            if (agent->getState() == "DEAD") continue;

            if (agentAssignments[i].size() >= agent->getCapacity()) continue;

            for (int j : availablePackages){
                const Package& p = packages[j];

                /// Calculez distanta totala pe care agentul trebuie sa o parcurga pentru a livra pachetul si pentru a se intoarce la baza (i.e. de la baza la destinatie, de la pozitia agentului la baza dar si drumul de intoarcere la baza)
                int distanceToDelivery = 0;
                int distanceToPickup = 0;

                /// Verific daca are deja pachete de preluat astfel incat sa nu calculeze costul de preluare al tuturor pachetelor eronat
                /// (daca are deja un pachet de preluat,costul de preluare pentru celelalte va fi mai mic pentru ca agentul oricum se intoarce in baza)
                bool hasPicked = false;
                if (!agentAssignments[i].empty()){
                    hasPicked = true;
                }

                /// Daca e drona folosesc Manhattan Distance
                if (agent->getSymbol() == '^'){
                    distanceToDelivery = PathFinder::manhattanDistance(basePos.first, basePos.second, p.getDestination().first, p.getDestination().second);

                    /// Daca agentul are deja un pachet de preluat nu mai calculez distanta pana la baza
                    if (hasPicked){
                        distanceToPickup = 0;
                    }
                    else{
                        distanceToPickup = PathFinder::manhattanDistance(agent->getPosition().first, agent->getPosition().second, basePos.first, basePos.second);
                    }
                }
                /// Altfel folosesc BFS pentru a calcula distanta reala
                else{
                    distanceToDelivery = PathFinder::realDistance(basePos.first, basePos.second, p.getDestination().first, p.getDestination().second, *map);

                    if (hasPicked){
                        distanceToPickup = 0;
                    }
                    else{
                        distanceToPickup = PathFinder::realDistance(agent->getPosition().first, agent->getPosition().second, basePos.first, basePos.second, *map);
                    }

                    if (distanceToDelivery == -1 || (!hasPicked && distanceToPickup == -1)){
                        /// Nu exista drum posibil, trecem la urmatorul pachet
                        continue;
                    }
                }

                /// Cand se calculeaza distanta totala se ia in considerare si drumul de intoarcere la baza (i.e 2* distToDelivery)
                int distance = (2*distanceToDelivery) + distanceToPickup;

                int speed = agent->getSpeed();

                /// Estimez timpul (numarul aproximativ de tick-uri) total pt a fi livrat pachetul cu formula fizica "timp = distanta / viteza".
                /// Am mai luat o marja de eroare de aproximativ 2 tick-uri just to be sure
                int time = (distance / speed) + 2;

                /// Calculez cata baterie e necesara pentru livrarea pachetului
                int batteryNeeded = time * agent->getConsumption();

                /// Daca agentul are suficienta baterie se calculeaza profitul estimat
                if (agent->getBattery() >= batteryNeeded){
                    /// Calculez timpul necesar pentru livrare si verific daca pachetul va fi livrat la timp
                    int timeElapsed = tick - p.getSpawnTick();
                    int timeLeft = p.getDuration() - timeElapsed;

                    bool late = (time > timeLeft);

                    int cost = time * agent->getCost();
                    int reward = p.getReward();

                    if (late){
                        reward -= 50; // Penalizare intarziere
                    }

                    /// Verific si modific profitul maxim, adagand oferta in lista de oferte disponibile
                    int profitEstimate = reward - cost;
                    if (profitEstimate > 0){
                        availableOffers.emplace_back(profitEstimate, i, j, false);
                    }
                }
                /// Altfel caut drum catre cea mai apropiata statie de incarcare
                else{
                    std::pair<int, int> stationPos = PathFinder::findNearestStation(agent->getPosition().first, agent->getPosition().second, map, agent->getSymbol());

                    try{
                        if (stationPos.first == -1)
                            throw std::runtime_error("No available charging stations");
                    }
                    catch (const std::exception& e){
                        ///std::cerr<<agent->getSymbol()<<": "<<e.what()<<"\n";
                        continue;
                    }

                    /// Calculez distantele pana la preluarea pachetului din baza, pana la statie, de la statie la destinatie si de la destinatie inapoi la baza precum si timpii necesari
                    int distPickup = 0;
                    int distStation = 0;
                    int distStationToDest = 0;
                    int distReturnToBase = 0;

                    if (agent->getSymbol() == '^'){
                        distPickup = PathFinder::manhattanDistance(agent->getPosition().first, agent->getPosition().second, basePos.first, basePos.second);
                        distStation = PathFinder::manhattanDistance(basePos.first, basePos.second, stationPos.first, stationPos.second);
                        distStationToDest = PathFinder::manhattanDistance(stationPos.first, stationPos.second, p.getDestination().first, p.getDestination().second);
                        distReturnToBase = PathFinder::manhattanDistance(p.getDestination().first, p.getDestination().second, basePos.first, basePos.second);
                    }
                    else{
                        distPickup = PathFinder::realDistance(agent->getPosition().first, agent->getPosition().second, basePos.first, basePos.second, *map);
                        distStation = PathFinder::realDistance(basePos.first, basePos.second, stationPos.first, stationPos.second, *map);
                        distStationToDest = PathFinder::realDistance(stationPos.first, stationPos.second, p.getDestination().first, p.getDestination().second, *map);
                        distReturnToBase = PathFinder::realDistance(p.getDestination().first, p.getDestination().second, basePos.first, basePos.second, *map);

                        if (distPickup == -1 || distStation == -1 || distStationToDest == -1 || distReturnToBase == -1){
                            /// Nu exista drum posibil, trecem la urmatorul pachet
                            continue;
                        }
                    }

                    int totalDistance = distPickup + distStation + distStationToDest + distReturnToBase;
                    int totalMoveTime = totalDistance / speed + 2; ///+2 ticks marja de eroare

                    /// Calculez cata baterie are la ajungerea la statie
                    int batAtStation = agent->getBattery() - ( (distStation + distPickup) * agent->getConsumption());
                    /// Daca e pe minus trecem la urmatorul (Imposibil but just to be safe, se decide daca se poate ajunge sau nu la statie, daca nu trecem la urmatorul agent)
                    if (batAtStation < 0) continue;

                    /// Calculez cata baterie e necesara pentru a ajunge la destinatie de la statie, precum si deficitul, adica cu cat trebuie incarcata
                    int batNeeded =  (distStationToDest + distReturnToBase) * agent->getConsumption();
                    if (batNeeded > agent->getMaxBattery()) continue; /// Imposibil de livrat, trece la urm agent

                    int deficit = batNeeded - batAtStation;

                    int charge = agent->getMaxBattery() / 4; ///25% per tick

                    int chargeTicks = 0;
                    if (deficit > 0){
                        chargeTicks = (deficit / charge) + 1; ///+1 just to be sure
                    }

                    /// Verific si timpul de livrare, daca se poate ajunge la timp
                    int timeDelivery = ((distPickup + distStation +distStationToDest) / speed) + 2 + chargeTicks;

                    int timeElapsed = tick - p.getSpawnTick();
                    int timeLeft = p.getDuration() - timeElapsed;

                    bool late = (timeDelivery > timeLeft);
                    int reward = p.getReward();

                    if (late){
                        reward -= 50; // Penalizare intarziere
                    }

                    int cost = totalMoveTime * agent->getCost();
                    int profitEstimate = reward - cost;

                    if (profitEstimate > 0){
                        availableOffers.emplace_back(profitEstimate, i, j, true);
                    }

                }
            }

        }
                /// Sortez toate ofertele disponibile in functie de profit si le aloc agentilor pana se termina pachetele sau capacitatea agentilor
                /// Acest lucru il fac pentru a se distribui pachetele astfel incat profitul sa fie maximizat, chiar daca drona poate duce pachetul la timp, aceasta este mai scupa ca un robot
                std::sort(availableOffers.begin(), availableOffers.end());
                for (const Offer& offer : availableOffers){
                    int aIdx = offer.getAgentIdx();
                    int pIdx = offer.getPackageIdx();
                    Agent* agent = agents[aIdx];

                    if (taken[pIdx]) continue;

                    /// Assert: nu se depaseste capacitatea agentului, chiar daca e if ul mai jos am tesstat just in case
                    /// assert(agentAssignments[aIdx].size() < agent->getCapacity());
                    if (agentAssignments[aIdx].size() >= agent->getCapacity()) continue;

                    agentAssignments[aIdx].push_back(pIdx);
                    taken[pIdx] = true;

                    std::cout<<"AGENT "<<agents[aIdx]->getSymbol()<<" HAS TAKEN PACKAGE "<<pIdx<<" WITH PROFIT "<<offer.getProfit()<<"\n";
                    if (offer.getStation()) std::cout<<"REDIRECTED TO STATION FOR CHARGING BEFORE DELIVERY\n";
                    else std::cout<<"DELIVERING DIRECTLY FROM BASE\n";

                    std::cout<<"PROFIT: "<<offer.getProfit()<<"\n";
                }
    }

    /// Functia care misca agentii pe harta, actualizeaza starea acestora si gestioneaza livrarile
    void moveAgents(std::vector<Agent*>& agents, Map* map, std::vector<Package>& packages, std::vector<std::vector<int>>& agentAssignments,
         std::vector<bool>& delivered, std::vector <bool>& lost, int& budget,
        int& deliveredCount, int& lostCount, int tick, std::pair<int, int> basePos){

        for (size_t i = 0; i < agents.size(); i++){
            Agent* agent = agents[i];
            try{
                /// Daca agentul e mort se trece la urmatorul
                if (agent->getState() == "DEAD") continue;

                if (!agentAssignments[i].empty()){

                    int pIdx = agentAssignments[i][0];
                    const Package& targetPackage = packages[pIdx];

                    /// Variabilele pentru pozitia tinta si cea curenta
                    std::pair<int, int> targetPosition = targetPackage.getDestination();
                    std::pair<int, int> currentPosition = agent->getPosition();

                    /// Daca e in baza si are pachete hasPackage e true
                    if (currentPosition == basePos){
                        agent->setHasPackage(true);
                    }

                    /// Variabila pentru a determina daca agentul e in drum spre preluarea pachetului
                    std::pair<int, int> finalPosition;
                    /// Daca are pachet se duce catre destinatie
                    if (agent->getHasPackage()){
                        finalPosition = targetPosition;
                    }
                    else{
                        finalPosition = basePos;
                    }

                    /// Calculez distanta catre urmatoarea oprire a agentului (statie sau destinatie)
                    int distTarget = 0;
                    if (agent->getSymbol() == '^'){
                        distTarget = PathFinder::manhattanDistance(currentPosition.first, currentPosition.second, finalPosition.first, finalPosition.second);
                    }
                    else{
                        distTarget = PathFinder::realDistance(currentPosition.first, currentPosition.second, finalPosition.first, finalPosition.second, *map);
                    }

                    int distReturn = 0;

                    /// Daca nu e in baza verific daca e vreo statie in apropiere
                    if (finalPosition != basePos){
                        std::pair<int, int> stationFromDest = PathFinder::findNearestStation(finalPosition.first, finalPosition.second, map, agent->getSymbol());

                        /// Daca nu gaseste agentul o statie in apropiere, cea mai buna incercare e intoarcerea in baza
                        std::pair<int, int> tryPos;
                        if (stationFromDest.first != -1){
                            tryPos = stationFromDest;
                        }
                        else{
                            tryPos = basePos;
                        }

                        /// Calculez distanta de la destinatie inapoi la baza pentru a verifica daca are suficienta baterie pentru intoarcere
                        if (agent->getSymbol() == '^'){
                            distReturn = PathFinder::manhattanDistance(finalPosition.first, finalPosition.second, tryPos.first, tryPos.second);
                        }
                        else{
                            distReturn = PathFinder::realDistance(finalPosition.first, finalPosition.second, tryPos.first, tryPos.second, *map);
                        }
                    }

                    /// Daca nu se poate ajunge la tinta sau inapoi la baza setez distantele la valori mari pentru a forta agentul sa nu se miste
                    if (distTarget == -1 || distReturn == -1) {
                        distTarget = 99999;
                        distReturn = 0;
                    }

                    /// Calculez distanta totala si cata baterie e necesara pentru livrare + intoarcere
                    int totalDistance = distTarget + distReturn;
                    int speed = agent->getSpeed();
                    /// Assert: speed nu poate fi 0
                    /// assert(speed > 0);

                    int needForDelivery = (totalDistance / speed) * agent->getConsumption();
                    int safetyMargin = std::max(5, (int)(needForDelivery * 0.1)); // 10% din necesar sau minim 5 unitati marja de eroare
                    int totalNeeded = safetyMargin + needForDelivery;

                    std::pair<int ,int> nextPos = finalPosition;

                    bool charge = false;

                    if (agent->getBattery() < totalNeeded){
                        /// Setez starile agentului corespunzator
                        char cell = map->getCell(currentPosition.first, currentPosition.second);
                        if (cell == 'S' || cell == 'B'){
                            if (agent->getBattery() >= totalNeeded){
                                agent->setState("MOVING");
                            }

                            else{
                                if (agent->getBattery() < agent->getMaxBattery()){
                                    agent->setState("CHARGING");
                                    charge = true;
                                }
                                else{
                                    agent->setState("MOVING");
                                }
                            }
                        }
                        else{
                            std::pair<int, int> station = PathFinder::findNearestStation(currentPosition.first, currentPosition.second, map, agent->getSymbol());
                            if (station.first != -1){
                                nextPos = station;
                            }
                            else{
                                nextPos = finalPosition;
                            }
                        }

                    }

                    if (!charge){
                        agent->setState("MOVING");
                        agent->move(nextPos.first, nextPos.second, *map);
                    }
                    /// Verific daca agentul a ajuns la destinatie pentru a livra pachetul (la timp sau late)
                    if (agent->getPosition().first == targetPosition.first && agent->getPosition().second == targetPosition.second && agent->getHasPackage()){
                       int reward = targetPackage.getReward();

                        if (targetPackage.isLate(tick)){
                            std::cout<<"LATE DELIVERY!!! PENALTY APPLIED\n";
                            reward -= 50;
                        }
                        else{
                            std::cout<<"DELIVERY ON TIME!!! YAYY\n";
                        }
                        budget += reward;
                        delivered[pIdx] = true;
                        deliveredCount++;
                        std::cout<<"AGENT "<<agent->getSymbol()<<" DELIVERED PACKAGE "<<pIdx<<" AND EARNED "<<reward<<"$\n";

                        /// Stergem pachetul din lista pacheteleor agentului
                        agentAssignments[i].erase(agentAssignments[i].begin());

                        /// Setez automat hasPackage la false dupa livrare pentru a nu se "teleporta" un pachet la robot, fara ca acesta sa treaca prin baza
                        agent->setHasPackage(false);
                    }

                }
                else{
                    agent->setHasPackage(false);

                    std::pair<int, int> currentPosition = agent->getPosition();
                    /// Daca agentul e in baza ori se incarca ori e IDLE
                    if (currentPosition.first == basePos.first && currentPosition.second == basePos.second){
                        if (agent->getBattery() < agent->getMaxBattery()){
                            agent->setState("CHARGING");
                        }
                        else{
                            agent->setState("IDLE");
                        }
                    }
                    /// Daca nu e in baza e MOVING
                    else{
                        agent->setState("MOVING");
                        agent->move(basePos.first, basePos.second, *map);
                    }

                }

                /// Scad costul de operare al agentului din buget
                if (agent->getState() == "MOVING"){
                    budget -= agent->getCost();
                }

                bool alive = agent->updateState(*map);
                if (!alive){
                    throw std::runtime_error("Agent has died due to battery depletion");
                }

            }
            catch (const std::exception& e){

                /// Daca agentul moare se scade si din buget (exceptie in catch)
                std::cerr<<"AGENT "<<agent->getSymbol()<<" DIED: "<<e.what()<<"\n";

                budget -= 500;
                if (!agentAssignments[i].empty()){
                    for (int k : agentAssignments[i]){
                        lost[k] = true;
                        lostCount++;
                        budget -=200;
                        std::cout<<"LOST PACKAGE "<<k<<" WOMP WOMP\n";

                    }
                    agentAssignments[i].clear();
                }
            }
        }
    }

};