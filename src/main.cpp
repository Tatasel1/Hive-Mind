#include <iostream>
#include <vector>
#include <utility>
#include <ctime>
#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>

#include "Map.h"
#include "AgentFactory.h"
#include "Packets.h"
#include "ConfigMap.h"
#include "HiveMind.h"

std::pair<int, int> getBasePosition(const Map* map) {
    for (int i = 0; i < map->getRow(); i++) {
        for (int j = 0; j < map->getCol(); j++) {
            if (map->getCell(i, j) == 'B') return {i, j};
        }
    }
    return {0, 0};
}

void printSimulation(const Map* map, const std::vector<Agent*>& agents,
                     const std::vector<std::vector<int>>& assignments,
                     int tick, int budget, int delivered, int lost) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    int rows = map->getRow();
    int cols = map->getCol();
    std::vector<std::string> displayGrid(rows);
    for (int i = 0; i < rows; i++) {
        std::string rowStr = "";
        for (int j = 0; j < cols; j++) rowStr += map->getCell(i, j);
        displayGrid[i] = rowStr;
    }

    for (auto* agent : agents) {
        if (agent->getState() != "DEAD") {
            auto pos = agent->getPosition();
            displayGrid[pos.first][pos.second] = agent->getSymbol();
        }
    }

    std::cout << "\n===========================================\n";
    std::cout << " HIVE MIND SIMULATION | Tick: " << tick << "\n";
    std::cout << "===========================================\n";
    std::cout << "BUDGET: " << budget << "$ | Delivered: " << delivered << " | Lost: " << lost << "\n";
    std::cout << "-------------------------------------------\n";

    for (const auto& line : displayGrid)
        std::cout << "  " << line << "\n";

    std::cout << "-------------------------------------------\n";
    std::cout << " FLEET STATUS:\n";
    std::cout << "-------------------------------------------\n";

    for (size_t i = 0; i < agents.size(); ++i) {
        Agent* a = agents[i];
        auto pos = a->getPosition();
        std::cout << "[" << a->getSymbol() << "] "
                  << "Pos:(" << pos.first << "," << pos.second << ") "
                  << "Bat:" << a->getBattery() << "/" << a->getMaxBattery() << " "
                  << "State:" << a->getState() << "\n";
    }
}

HiveMind* HiveMind::instance = nullptr;

int main() {
    std::srand(static_cast<unsigned int>(time(NULL)));
    ConfigMap config = ConfigMap::loadFromFile("simulation_setup.txt");

    std::cout << "1. Load from file\n2. Procedural Generation\nChoice: ";
    int choice; std::cin >> choice;

    IMapGenerator* mapGenerator = (choice == 1) ?
        (IMapGenerator*)new FileMapLoader("map_file.txt") :
        (IMapGenerator*)new ProceduralMapGenerator(config);

    Map* map = mapGenerator->generate();
    delete mapGenerator;
    if (!map) return -1;

    auto basePos = getBasePosition(map);
    auto dropOffPoints = map->getDropOffPoints(map);

    std::vector<Agent*> agents;
    for (int i = 0; i < config.drones; i++)
        agents.push_back(AgentFactory::createAgent("DRONE", basePos.first, basePos.second));
    for (int i = 0; i < config.robots; i++)
        agents.push_back(AgentFactory::createAgent("ROBOT", basePos.first, basePos.second));
    for (int i = 0; i < config.scooters; i++)
        agents.push_back(AgentFactory::createAgent("SCOOTER", basePos.first, basePos.second));

    std::vector<Package> packages;
    std::vector<bool> taken, lost, delivered;
    std::vector<std::vector<int>> agentAssignments(agents.size());

    int budget = 0, generatedPackages = 0, deliveredCount = 0, lostCount = 0;

    // === STATISTICS ===
    int totalRewards = 0;
    int penaltyLostPackages = 0;
    int penaltyLateDeliveries = 0;
    int penaltyDeadAgents = 0;

    std::cin.ignore();
    std::cout << "Press ENTER to start...";
    std::cin.get();

    for (int tick = 0; tick <= config.ticks; tick++) {

        if (tick > 0 && tick % config.spawn_freq == 0 && generatedPackages < config.packages) {
            int idx = std::rand() % dropOffPoints.size();
            auto dest = dropOffPoints[idx];

            int reward = 200 + (std::rand() % 601);
            int duration = 10 + (std::rand() % 11);

            packages.emplace_back(generatedPackages, dest.first, dest.second,
                                  basePos.first, basePos.second,
                                  reward, duration, tick);

            taken.push_back(false);
            delivered.push_back(false);
            lost.push_back(false);
            generatedPackages++;
        }

        for (size_t p = 0; p < packages.size(); ++p) {
            if (!taken[p] && !delivered[p] && !lost[p] && packages[p].isLate(tick)) {
                lost[p] = true;
                budget -= 200;
                penaltyLostPackages += 200;
                lostCount++;
            }
        }

        printSimulation(map, agents, agentAssignments, tick, budget, deliveredCount, lostCount);

        HiveMind::getInstance()->assignPackages(
            agents, packages, agentAssignments, taken, delivered, lost, tick, basePos, map);

        HiveMind::getInstance()->moveAgents(
            agents, map, packages, agentAssignments,
            delivered, lost, budget, deliveredCount, lostCount, tick, basePos);

        for (size_t p = 0; p < packages.size(); ++p) {
            if (delivered[p]) {
                totalRewards += packages[p].getReward();
                if (packages[p].isLate(tick)) {
                    penaltyLateDeliveries += 50;
                }
                delivered[p] = false;
            }
        }

        if (generatedPackages >= config.packages &&
            deliveredCount + lostCount == config.packages)
            break;
    }

    int deadAgents = 0;
    for (auto* a : agents)
        if (a->getState() == "DEAD")
            deadAgents++;

    penaltyDeadAgents = deadAgents * 500;
    int aliveAgents = agents.size() - deadAgents;

    std::ofstream report("simulation.txt");
    report << "========= FINAL SIMULATION REPORT =========\n\n";
    report << "Packages Delivered: " << deliveredCount << "\n";
    report << "Packages Lost: " << lostCount << "\n\n";
    report << "Total Rewards: +" << totalRewards << "\n";
    report << "Penalty Lost Packages: -" << penaltyLostPackages << "\n";
    report << "Penalty Late Deliveries: -" << penaltyLateDeliveries << "\n";
    report << "Penalty Dead Agents: -" << penaltyDeadAgents << "\n\n";
    report << "Final Profit: " << budget << "\n\n";
    report << "Agents Total: " << agents.size() << "\n";
    report << "Agents Survived: " << aliveAgents << "\n";
    report << "Agents Dead: " << deadAgents << "\n";
    report.close();

    for (auto* a : agents) delete a;
    delete map;
    return 0;
}
