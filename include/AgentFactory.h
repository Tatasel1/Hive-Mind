#pragma once
#include <string>
#include "Agents.h"
#include "Drone.h"
#include "Robot.h"
#include "Scooter.h"

/// Design Pattern-ul Factory pentru Agenti si creearea lor
class AgentFactory {
public:
    static Agent* createAgent(const std::string& type, int startX, int startY) {
        if (type == "DRONE") {
            return new Drone(startX, startY);
        } else if (type == "ROBOT") {
            return new Robot(startX, startY);
        } else if (type == "SCOOTER") {
            return new Scooter(startX, startY);
        }
        return nullptr;
    }
};
