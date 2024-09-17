# Pokemon Style Text-Based Game

## Description

This is a Pokemon-style text-based game where players explore a world represented by ASCII characters. The game world consists of different terrains, such as roads, grass, water, and various obstacles. Players control their character (PC) using keyboard inputs to navigate the world, encounter NPCs (Non-Playable Characters), and participate in battles. The game features dynamic world generation, pathfinding algorithms, and a priority queue system to handle NPC movement and interaction.

The game uses `ncurses` to render the world and handle real-time inputs, making it a great exercise in building terminal-based user interfaces. It also includes simplified turn-based battles, inspired by the classic Pokemon games, where players engage in combat with wild Pokémon and NPC trainers.

---

## Features
- **Map Legends:**
  - `#`: Road path (NS/WE)
  - `M`: Pokemart
  - `C`: Pokecenter
  - `:`: Tall grass (possible Pokémon encounters)
  - `.`: Short grass
  - `^`: Trees
  - `%`: Boulders
  - `~`: Water

- **Trainers:**
  - `@`: Player Character (PC)
  - `h`: Hiker (NPC)
  - `r`: Rival (NPC)
  - `p`: Pacer (NPC)
  - `w`: Wanderer (NPC)
  - `s`: Sentry (NPC)
  - `e`: Explorer (NPC)

- **NPC Behavior:**
  - NPCs have different movement behaviors based on their type. Hikers and Rivals follow the shortest path to the player, while others, such as Pacers and Wanderers, move randomly or remain stationary.

- **Battle Mechanics:**
  - Engage in turn-based battles with NPCs and wild Pokémon.
  - Utilize basic moves and items (such as Potions, Revives, and Pokeballs) in battles.

- **World Generation:**
  - The game generates a procedurally-created world map, complete with varying terrain types, Pokémon Centers, and Pokemarts.
  - Roads connect different sections of the map, allowing the player to navigate seamlessly between maps.

- **Priority Queue:**
  - The game uses a priority queue to manage NPC turns, ensuring a dynamic and fair turn-based system based on movement costs.

## Controls
- `7`, `y`: Move PC one cell to the upper left.
- `8`, `k`: Move PC one cell up.
- `9`, `u`: Move PC one cell to the upper right.
- `6`, `l`: Move PC one cell to the right.
- `3`, `n`: Move PC one cell to the lower right.
- `2`, `j`: Move PC one cell down.
- `1`, `b`: Move PC one cell to the lower left.
- `4`, `h`: Move PC one cell to the left.
- `>`: Interact with Pokemarts or Pokecenters when standing on the corresponding cell.
- `5`, ` `, `. (space)`: Rest for a turn. NPCs still move.
- `t`: Display a list of trainers on the map with their symbol and position relative to the PC.
- `f`: Fly to a certain grid point (user input required).
- `Q`: Quit the game.

---

## Game Mechanics

- **Dynamic World Generation:**
  - Each playthrough generates a unique world with procedurally placed elements (roads, grass, trees, etc.). This keeps the experience fresh each time the game is played.

- **NPC Movement:**
  - NPCs have distinct behaviors: Hikers and Rivals follow the least-cost path to the player, Pacers move in straight lines, Wanderers move randomly, and Sentries stay stationary.

- **Turn-Based Battle System:**
  - Players encounter wild Pokémon or NPCs in tall grass. Battles follow a turn-based system inspired by the Pokemon series, where players can choose to Fight, Run, or use items.

- **Healing and Restoring:**
  - Players can visit Pokemarts to restock supplies and Pokecenters to fully heal Pokémon after battles.

---

---

## Pokedex Database

The game relies on a **Pokedex** folder containing several important `.csv` files that provide data for generating Pokémon, their attributes, and battle statistics. You need to ensure the `pokedex` folder exists in one of the following locations depending on your operating system or setup.

### Pokedex Directory Locations:
1. `/share/Pokemon-Text-Based-Game/pokedex/pokedex/data/csv/`
2. `~/.poke327/pokedex/pokedex/data/csv/`
3. `c:/Pokemon-Text-Based-Game/pokedex/pokedex/data/csv/`

You should have the following CSV files located inside the `csv` folder:

### Required CSV Files:
1. `pokemon.csv`: Contains basic information about Pokémon, including their ID, name, base stats, and evolution levels.
2. `moves.csv`: Contains information about the different moves each Pokémon can learn, including move name, power, accuracy, and type.
3. `pokemon_moves.csv`: Links Pokémon with the moves they can learn.
4. `pokemon_species.csv`: Provides additional species-level data such as growth rate and base experience.
5. `experience.csv`: Specifies how much experience is required for each Pokémon to level up.
6. `type_names.csv`: Defines different Pokémon types, such as Water, Fire, Grass, etc.
7. `pokemon_stats.csv`: Provides detailed statistics (HP, Attack, Defense, etc.) for each Pokémon.
8. `stats.csv`: Lists the available stats each Pokémon can have.
9. `pokemon_types.csv`: Lists the types associated with each Pokémon.
10. `pokemon_habitats.csv`: Lists of habitats.

---

## Build and Run Instructions

### Requirements

- **g++ compiler** for compiling the C++ source code.
- **ncurses** library for rendering the game interface in the terminal.

### Build Instructions

#### Linux/macOS:

1. **Install ncurses** if you don't have it already:
    - On Ubuntu/Debian:
      ```bash
      sudo apt-get install libncurses5-dev libncursesw5-dev
      ```
    - On macOS:
      ```bash
      brew install ncurses
      ```

2. **Clone the repository** and navigate into the project folder:
    ```bash
    git clone <repository-url>
    cd <repository-folder>
    ```

3. **Compile the game** by running the `make` command:
    ```bash
    make
    ```

4. **Run the game**:
    ```bash
    ./Poke
    ```

#### Windows (Using WSL or Cygwin):

1. **Option 1: WSL (Windows Subsystem for Linux)**
   - Install **WSL** if you haven't already.
   - Open a WSL terminal and follow the same steps as **Linux/macOS**.

2. **Option 2: Cygwin**
   - Install **Cygwin** and include packages for `gcc`, `make`, and `ncurses`.
   - Run the `make` command to build and run the game.
