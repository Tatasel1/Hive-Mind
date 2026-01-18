# HiveMind: Autonomous Logistics Simulator

**HiveMind** is a C++ Object-Oriented simulation engine designed to coordinate a heterogeneous fleet of autonomous robots. It acts as a central decision-making unit ("The Hive Mind") to optimize urban logistics in a futuristic 2030 setting.

The system maximizes profit by managing pathfinding, battery physics, and task allocation for Drones, Ground Robots, and Scooters .

---

## 📋 Project Context & Objectives

**Scenario:** It is the year 2030. Urban logistics are handled by autonomous fleets. You play the role of the Software Architect for the "Hive Mind" system .

**Core Objectives:**
1.  **Procedural Generation:** Create dynamic city maps or load them from files [cite: 8, 28-29].
2.  **Physics Simulation:** Manage battery life, movement, and charging cycles for agents[cite: 9].
3.  **Algorithmic Optimization:** Maximize **Net Profit** through intelligent dispatching and routing[cite: 10, 57].

---

## 🏗️ Software Architecture

The project is built on modular C++ principles, utilizing specific Design Patterns to ensure flexibility:

* **Singleton Pattern:** The `HiveMind` class is a singleton, ensuring a centralized brain for the entire fleet.
* **Strategy Pattern:** Used for Map Generation (`IMapGenerator`), allowing the system to switch between loading files (`FileMapLoader`) or procedural generation (`ProceduralMapGenerator`) at runtime .
* **Polymorphism:** A hierarchy of `Agent` classes (Drone, Robot, Scooter) handles distinct physical attributes and movement logic.

---

## 🌍 The Simulation Environment

### 1. The Grid Map
The city is represented as an $M \times N$ matrix.

* `[.]` **Space:** Walkable area.
* `[#]` **Wall:** Obstacle. Impassable for ground units.
* `[B]` **Base (Hub):** Central depot for spawning agents/packages and charging .
* `[S]` **Station:** Supplementary battery charging locations.
* `[D]` **Destination (Client):** Delivery locations for packages.

**Validation:** Procedurally generated maps are validated using **BFS (Breadth-First Search)** to ensure all Clients and Stations are reachable from the Base .

### 2. Map Input Example (`map_file.txt`)
When using `FileMapLoader`, the map is parsed from ASCII text:

```text
..#.................
.B#D................
.#...###############
.#.................#
.#################.#
.#.................#
.#.#################
.#..................
.###########..######
....................
```

## 🤖 The Agents (The Fleet)

The fleet consists of three agent types, each with specific trade-offs handled by the HiveMind logic.

| Agent Type | Symbol | Movement | Speed (cells/tick) | Max Battery | Cost ($/tick) | Capacity |
| :--- | :---: | :--- | :---: | :---: | :---: | :---: |
| **Drone** | `A` | Air (Ignores Walls) | 3 | 100 | 15 | 1 |
| **Robot** | `R` | Ground (Avoids Walls) | 1 | 300 | 1 | 4 |
| **Scooter** | `S` | Ground (Avoids Walls) | 2 | 200 | 4 | 2 |


### Lifecycle
* **IDLE:** Waiting at Base.
* **MOVING:** Consuming battery to reach a target.
* **CHARGING:** Regaining 25% battery/tick at Base (`B`) or Station (`S`).
* **DEAD:** Battery hits 0 outside a station (Agent lost, -500 credit penalty).

---

## 🧠 Algorithmic Logic (HiveMind)

### 1. Economic Dispatch
The `assignPackages` method calculates the "Estimated Profit" for every package/agent combination:

$$Profit = Reward - (OpCost \times EstTime) - PotentialPenalties$$

* **Battery Awareness:** The system simulates the full trip (Pickup $\rightarrow$ Delivery $\rightarrow$ Return). If the battery is insufficient, it calculates a detour to a Station (`S`) and factors in the charging time/cost .
* **Priority:** Offers are sorted by profit; high-value packages are assigned to the most efficient agents first.

### 2. Pathfinding
* **Drones:** Use Manhattan Distance (direct flight, ignoring walls).
* **Ground Units:** Use BFS to find the shortest path around walls.

---

## ⚙️ Configuration (`simulation_setup.txt`)

The simulation is driven by an external configuration file.

```text
MAP_SIZE: 20 20       # Grid dimensions (Rows Cols)
MAX_TICKS: 1000       # Duration of simulation
MAX_STATIONS: 3       # Number of extra charging stations
CLIENTS_COUNT: 10     # Number of delivery destinations
DRONES: 3             # Fleet composition
ROBOTS: 2
SCOOTERS: 1
TOTAL_PACKAGES: 50    # Total orders to generate
SPAWN_FREQUENCY: 10   # New package every N ticks
```

## 📊 Outputs & Scoring

[cite_start]The goal is to maximize **Net Profit**[cite: 57].

* [cite_start]**Dead Agent:** -500 credits[cite: 63].
* [cite_start]**Lost Package:** -200 credits[cite: 63].
* [cite_start]**Late Package:** -50 credits[cite: 63].
* [cite_start]**Package Reward:** 200-800 credits (random)[cite: 54].

### Final Report Example (`simulation.txt`)

[cite_start]At the end of the run, the system generates a performance report[cite: 87, 168].

```text
========= FINAL SIMULATION REPORT =========

Packages Delivered: 50
Packages Lost: 0

Total Rewards: +24536
Penalty Lost Packages: -0
Penalty Late Deliveries: -1750
Penalty Dead Agents: -0

Final Profit: 21128

Agents Total: 6
Agents Survived: 6
Agents Dead: 0
```
