# HiveMind: Autonomous Logistics Simulator

**HiveMind** is a C++ simulation engine designed to coordinate a heterogeneous fleet of autonomous robots in a futuristic urban environment. [cite_start]The system acts as a central decision-making unit to maximize logistics profit through algorithmic task allocation and pathfinding[cite: 5, 7, 10].

---

## 1. Context & Objectives

**Scenario:** It is the year 2030. Urban logistics are managed by autonomous fleets. [cite_start]You act as the **Software Architect** for the "Hive Mind" system [cite: 3-5].

**Primary Objectives:**
1.  [cite_start]**Procedural Generation:** Create dynamic city maps or load them from files[cite: 8].
2.  [cite_start]**Physics Simulation:** Simulate agent movement, battery consumption, and lifecycle[cite: 9].
3.  [cite_start]**Algorithmic Optimization:** Maximize **Net Profit** using the "Hive Mind" allocation algorithm[cite: 10, 57].

---

## 2. Software Architecture

[cite_start]The application is built using modular C++ and strict Object-Oriented Programming (OOP) principles[cite: 7, 96].

### Design Patterns
* [cite_start]**Singleton:** Implemented for the `HiveMind` class to ensure a unique, centralized coordination point for the fleet[cite: 126].
* [cite_start]**Strategy Pattern:** Used via the `IMapGenerator` interface to switch between map generation methods[cite: 25, 111]:
    * [cite_start]`FileMapLoader`: Loads a map from a text file (for debugging/benchmarking)[cite: 28, 120].
    * [cite_start]`ProceduralMapGenerator`: Generates a random map with walls, stations, and clients[cite: 29, 113].
* [cite_start]**Polymorphism:** A hierarchy of classes derived from `Agent` (Drone, Robot, Scooter) [cite: 36-37].

---

## 3. The World (Simulation Environment)

### 3.1 The Map (The Grid)
[cite_start]The city is an $M \times N$ matrix where each cell has a specific role[cite: 13]:

* [cite_start]`[.]` **Road (Space):** Accessible area[cite: 14].
* `[#]` **Wall:** Obstacle. [cite_start]Cannot be crossed by ground units[cite: 15].
* `[B]` **Base (Hub):** Central depot. [cite_start]Spawns agents/packages and acts as a charging station [cite: 16-19].
* [cite_start]`[S]` **Station:** Supplementary charging locations[cite: 20].
* [cite_start]`[D]` **Client (Destination):** Delivery locations for packages[cite: 21].

### 3.2 Map Generation & Validation
* [cite_start]**Generation:** Procedural maps have ~25% wall density[cite: 116].
* **Validation:** After generation, the map is validated using **BFS (Breadth-First Search)** or Flood Fill. [cite_start]It guarantees that a path exists from the Base (`B`) to all Clients (`D`) and Stations (`S`) [cite: 31-33].

---

## 4. The Fleet (Agents & Physics)

### 4.1 Agent Specifications
[cite_start]Each agent type has distinct economic and physical attributes[cite: 38].

| Agent Type | Symbol | Movement | Speed (cells/tick) | Max Battery | Cost ($/tick) | Capacity |
| :--- | :---: | :--- | :---: | :---: | :---: | :---: |
| **Drone** | `A` | Air (Ignores Walls) | 3 | 100 | 15 | 1 |
| **Robot** | `R` | Ground (Avoids Walls) | 1 | 300 | 1 | 4 |
| **Scooter** | `S` | Ground (Avoids Walls) | 2 | 200 | 4 | 2 |

### 4.2 Lifecycle (State Machine)
[cite_start]Agents transition through the following states [cite: 42-47]:
1.  **IDLE:** Waiting at Base (auto-charging).
2.  **MOVING:** Traveling to a destination (consumes battery).
3.  **CHARGING:** Regains 25% battery per tick at `B` or `S`.
4.  **DEAD:** If battery hits 0 outside a station, the agent is lost forever (-500 credit penalty).

---

## 5. HiveMind Algorithm (Business Logic)

### 5.1 Profit Formula
[cite_start]The system maximizes the following formula [cite: 60-61]:
$$Profit = (Rewards) - (Operating Costs) - (Penalties)$$

* **Rewards:** 200-800 credits per package (random)[cite: 54].
* **Operating Cost:** Sum of cost/tick for all active agents[cite: 62].
* **Penalties:**
    * -500 credits for a Dead Agent[cite: 63].
    * -200 credits for a Lost Package[cite: 63].
    * -50 credits for a Late Package[cite: 63].

### 5.2 Decision Strategy
1.  **Task Allocation (Dispatching):**
    * Calculates "Estimated Profit" for every package/agent pair.
    * **Battery Check:** Simulates the full path (Pickup → Delivery → Return). [cite_start]If battery is insufficient, it simulates a detour to a Charging Station (`S`) and recalculates costs [cite: 130-131].
    * **Sorting:** Offers are sorted by profit; high-value tasks go to the most efficient agents[cite: 132].

2.  **Pathfinding (Routing):**
    * **Drones:** Use Manhattan Distance (direct flight)[cite: 140].
    * **Ground Units:** Use BFS (Real Distance) to navigate around walls[cite: 140].
    * **Safety:** If an agent reaches critical battery, it is force-routed to the nearest station[cite: 141].

---

## 6. Configuration & Input Files

The simulation is configured via external text files.

### `simulation_setup.txt`
Controls the simulation parameters .

```text
MAP_SIZE: 20 20
MAX_TICKS: 1000
MAX_STATIONS: 3
CLIENTS_COUNT: 10
DRONES: 3
ROBOTS: 2
SCOOTERS: 1
TOTAL_PACKAGES: 50
SPAWN_FREQUENCY: 10
```
