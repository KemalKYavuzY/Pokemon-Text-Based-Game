#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <climits>
#include <cstring>
#include <cmath>
#include <unistd.h>
#include <ncurses.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <limits>
#include <algorithm>
#include <functional>

// Board Size
#define WIDTH 80
#define HEIGHT 21
#define MAP_SIZE 401
#define MAP_CENTER 200

// Terrain types
#define BORDER '%'
#define GRASS '.'
#define TALL_GRASS ':'
#define TREE '^'
#define WATER '~'
#define PATH '#'
#define POKEMART 'M'
#define POKEMON_CENTER 'C'

// Trainer types
#define PC '@'
#define HIKER 'h'
#define RIVAL 'r'
#define PACER 'p'
#define WANDERER 'w'
#define SENTRY 's'
#define EXPLORER 'e'

// Region generation parameters
#define MIN_RADIUS 2
#define MAX_RADIUS 5
#define MIN_REGIONS 5
#define MAX_REGIONS 12

int Pokeballs = 3; // Number of Pokeballs available
int Potions = 5;   // Number of Potions available
int Revives = 2;   // Number of Revives available
int ToxicOrb = 3;
int LightBall = 3;
int FlameOrb = 3;
int FreezeBomb = 3;
int SleepPowder = 3;
int ConfusionBomb = 3;

int curPokeIndex = 0;
int attemps = 0;
bool battleContinues = true;

struct Pokemon {
    int id;
    std::string identifier;
    int species_id;
    int height;
    int weight;
    int base_experience;
    int order;
    bool is_default;
};

struct PokemonWithMoves {
    Pokemon pokemon;
    std::vector<std::string> moves;
    int level;
    int hp;          // Hit points
    int max_hp;      // Maximum hit points
    int attack;      // Attack value
    int defense;     // Defense value
    int speed;       // Speed value
    bool is_knocked_out; // Knock out status
    int is_poisoned;
    int is_paralyzed;
    int is_burned;
    int is_asleep;       // Tracks sleep duration
    int is_frozen;       // Tracks freeze duration
    int is_confused;     // Tracks confusion duration
    int current_exp; // Current experience points
    int next_level_exp; // Experience points needed for the next level
};

std::vector<PokemonWithMoves> pcPokemons;
PokemonWithMoves* currentWildPokemon = nullptr;

struct Move {
    int id;
    std::string identifier;
    int generation_id;
    int type_id;
    int power;
    int pp;
    int accuracy;
    int priority;
    int target_id;
    int damage_class_id;
    int effect_id;
    int effect_chance;
    int contest_type_id;
    int contest_effect_id;
    int super_contest_effect_id;
};

struct PokemonMove {
    int pokemon_id;
    int version_id;
    int move_id;
    int pokemon_move_method_id;
    int level;
    int order;
};

struct PokemonSpecies {
    int id;
    std::string identifier;
    int generation_id;
    int evolves_from_species_id;
    int evolution_chain_id;
    int color_id;
    int shape_id;
    int habitat_id;
    int gender_rate;
    int capture_rate;
    int base_happiness;
    bool is_baby;
    int hatch_counter;
    bool has_gender_differences;
    int growth_rate_id;
    bool forms_switchable;
    bool is_legendary;
    bool is_mythical;
    int order;
    int conquest_order;
};

struct Experience {
    int growth_rate_id;
    int level;
    int experience;
};

struct TypeName {
    int type_id;
    int local_language_id;
    std::string name;
};

struct PokemonStat {
    int pokemon_id;
    int stat_id;
    int base_stat;
    int effort;
};

struct Stat {
    int id;
    std::string identifier; // String to hold the identifier
    int damage_class_id; // Assuming there is a damage class id
    bool is_battle_only;
    int game_index;
};


struct PokemonType {
    int pokemon_id;
    int type_id;
    int slot;
};

struct PokemonHabitat {
    int id;
    std::string identifier;
};

std::vector<Pokemon> parsePokemonCSV(const std::string& filename) {
    std::vector<Pokemon> pokemons;
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // Skip the header line

    while (std::getline(file, line)) {
        std::stringstream linestream(line);
        std::string cell;
        Pokemon pokemon;

        std::getline(linestream, cell, ',');
        pokemon.id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, pokemon.identifier, ',');

        std::getline(linestream, cell, ',');
        pokemon.species_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        pokemon.height = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        pokemon.weight = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        pokemon.base_experience = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        pokemon.order = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        pokemon.is_default = cell.empty() ? false : (cell == "1");

        pokemons.push_back(pokemon);
    }

    return pokemons;
}

std::vector<Move> parseMovesCSV(const std::string& filename) {
    std::vector<Move> moves;
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // Skip the header line

    while (std::getline(file, line)) {
        std::stringstream linestream(line);
        std::string cell;
        Move move;

        std::getline(linestream, cell, ',');
        move.id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, move.identifier, ',');

        std::getline(linestream, cell, ',');
        move.generation_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        move.type_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        move.power = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        move.pp = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        move.accuracy = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        move.priority = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        move.target_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        move.damage_class_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        move.effect_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        move.effect_chance = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        move.contest_type_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        move.contest_effect_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        move.super_contest_effect_id = cell.empty() ? INT_MAX : std::stoi(cell);

        moves.push_back(move);
    }

    return moves;
}

std::vector<PokemonMove> parsePokemonMovesCSV(const std::string& filename) {
    std::vector<PokemonMove> pokemonMoves;
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // Skip the header line

    while (std::getline(file, line)) {
        std::stringstream linestream(line);
        std::string cell;
        PokemonMove pokemonMove;

        std::getline(linestream, cell, ',');
        pokemonMove.pokemon_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        pokemonMove.version_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        pokemonMove.move_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        pokemonMove.pokemon_move_method_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        pokemonMove.level = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        pokemonMove.order = cell.empty() ? INT_MAX : std::stoi(cell);

        pokemonMoves.push_back(pokemonMove);
    }

    return pokemonMoves;
}

std::vector<PokemonSpecies> parsePokemonSpeciesCSV(const std::string& filename) {
    std::vector<PokemonSpecies> pokemonSpecies;
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // Skip the header line

    while (std::getline(file, line)) {
        std::stringstream linestream(line);
        std::string cell;
        PokemonSpecies species;

        std::getline(linestream, cell, ',');
        species.id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, species.identifier, ',');

        std::getline(linestream, cell, ',');
        species.generation_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        species.evolves_from_species_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        species.evolution_chain_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        species.color_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        species.shape_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        species.habitat_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        species.gender_rate = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        species.capture_rate = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        species.base_happiness = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        species.is_baby = cell.empty() ? false : (cell == "1");

        std::getline(linestream, cell, ',');
        species.hatch_counter = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        species.has_gender_differences = cell.empty() ? false : (cell == "1");

        std::getline(linestream, cell, ',');
        species.growth_rate_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        species.forms_switchable = cell.empty() ? false : (cell == "1");

        std::getline(linestream, cell, ',');
        species.is_legendary = cell.empty() ? false : (cell == "1");

        std::getline(linestream, cell, ',');
        species.is_mythical = cell.empty() ? false : (cell == "1");

        std::getline(linestream, cell, ',');
        species.order = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        species.conquest_order = cell.empty() ? INT_MAX : std::stoi(cell);

        pokemonSpecies.push_back(species);
    }

    return pokemonSpecies;
}

std::vector<Experience> parseExperienceCSV(const std::string& filename) {
    std::vector<Experience> experiences;
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // Skip the header line

    while (std::getline(file, line)) {
        std::stringstream linestream(line);
        std::string cell;
        Experience experience;

        std::getline(linestream, cell, ',');
        experience.growth_rate_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        experience.level = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        experience.experience = cell.empty() ? INT_MAX : std::stoi(cell);

        experiences.push_back(experience);
    }

    return experiences;
}

std::vector<TypeName> parseTypeNamesCSV(const std::string& filename) {
    std::vector<TypeName> typeNames;
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // Skip the header line

    while (std::getline(file, line)) {
        std::stringstream linestream(line);
        std::string cell;
        TypeName typeName;

        std::getline(linestream, cell, ',');
        typeName.type_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        typeName.local_language_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, typeName.name, ',');

        typeNames.push_back(typeName);
    }

    return typeNames;
}

std::vector<PokemonStat> parsePokemonStatsCSV(const std::string& filename) {
    std::vector<PokemonStat> pokemonStats;
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // Skip the header line

    while (std::getline(file, line)) {
        std::stringstream linestream(line);
        std::string cell;
        PokemonStat pokemonStat;

        std::getline(linestream, cell, ',');
        pokemonStat.pokemon_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        pokemonStat.stat_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        pokemonStat.base_stat = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        pokemonStat.effort = cell.empty() ? INT_MAX : std::stoi(cell);

        pokemonStats.push_back(pokemonStat);
    }

    return pokemonStats;
}

std::vector<Stat> parseStatsCSV(const std::string& filename) {
    std::vector<Stat> stats;
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // Skip the header line

    while (std::getline(file, line)) {
        std::stringstream linestream(line);
        std::string cell;
        Stat stat;

        std::getline(linestream, cell, ',');
        stat.id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        stat.damage_class_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, stat.identifier, ',');

        std::getline(linestream, cell, ',');
        stat.is_battle_only = cell == "1";

        std::getline(linestream, cell, ',');
        stat.game_index = cell.empty() ? INT_MAX : std::stoi(cell);

        stats.push_back(stat);
    }

    return stats;
}

std::vector<PokemonType> parsePokemonTypesCSV(const std::string& filename) {
    std::vector<PokemonType> pokemonTypes;
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // Skip the header line

    while (std::getline(file, line)) {
        std::stringstream linestream(line);
        std::string cell;
        PokemonType pokemonType;

        std::getline(linestream, cell, ',');
        pokemonType.pokemon_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        pokemonType.type_id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, cell, ',');
        pokemonType.slot = cell.empty() ? INT_MAX : std::stoi(cell);

        pokemonTypes.push_back(pokemonType);
    }

    return pokemonTypes;
}

std::vector<PokemonHabitat> parsePokemonHabitatsCSV(const std::string& filename) {
    std::vector<PokemonHabitat> pokemonHabitats;
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // Skip the header line

    while (std::getline(file, line)) {
        std::stringstream linestream(line);
        std::string cell;
        PokemonHabitat habitat;

        std::getline(linestream, cell, ',');
        habitat.id = cell.empty() ? INT_MAX : std::stoi(cell);

        std::getline(linestream, habitat.identifier, ',');

        pokemonHabitats.push_back(habitat);
    }

    return pokemonHabitats;
}

bool fileExists(const std::string& filePath) {
    std::ifstream file(filePath);
    return file.good();
}

// Declare global variables
std::vector<Pokemon> allPokemons;
std::vector<Move> allMoves;
std::vector<PokemonMove> allPokemonMoves;
std::vector<PokemonSpecies> allPokemonSpecies;
std::vector<Experience> allExperiences;
std::vector<TypeName> allTypeNames;
std::vector<PokemonStat> allPokemonStats;
std::vector<Stat> allStats;
std::vector<PokemonType> allPokemonTypes;
std::vector<PokemonHabitat> allPokemonHabitats;

std::string findFilePath(const std::string& fileType) {
    std::string filePath;

    // Try the first location
    filePath = "/share/Pokemon-Text-Based-Game/pokedex/pokedex/data/csv/" + fileType + ".csv";
    if (fileExists(filePath)) {
        return filePath;
    }

    // Try the second location
    const char* homeDir = getenv("HOME");
    if (homeDir != nullptr) {
        filePath = std::string(homeDir) + "/.poke327/pokedex/pokedex/data/csv/" + fileType + ".csv";
        if (fileExists(filePath)) {
            return filePath;
        }
    }

    // Try the third location
    filePath = "c:/Pokemon-Text-Based-Game/pokedex/pokedex/data/csv/" + fileType + ".csv";
    if (fileExists(filePath)) {
        return filePath;
    }

    // If the file is not found in any location, return an empty string
    return "";
}

void loadData() {
    allPokemons = parsePokemonCSV(findFilePath("pokemon"));
    allMoves = parseMovesCSV(findFilePath("moves"));
    allPokemonMoves = parsePokemonMovesCSV(findFilePath("pokemon_moves"));
    allPokemonSpecies = parsePokemonSpeciesCSV(findFilePath("pokemon_species"));
    allExperiences = parseExperienceCSV(findFilePath("experience"));
    allTypeNames = parseTypeNamesCSV(findFilePath("type_names"));
    allPokemonStats = parsePokemonStatsCSV(findFilePath("pokemon_stats"));
    allStats = parseStatsCSV(findFilePath("stats"));
    allPokemonTypes = parsePokemonTypesCSV(findFilePath("pokemon_types"));
    allPokemonHabitats = parsePokemonHabitatsCSV(findFilePath("pokemon_habitats"));
}

class PriorityQueueNode {
public:
    int x;
    int y;
    int cost;

    PriorityQueueNode() : x(0), y(0), cost(0) {}  // Default constructor

    // You can also add a constructor for initialization
    PriorityQueueNode(int x, int y, int cost) : x(x), y(y), cost(cost) {}
};

class map {
public:
    char board[HEIGHT][WIDTH];    // Represents the game board
    int North, East, South, West; // Represents the positions of the paths (roads)
    int VB, HB;

    map() {
        // Initialize the board and directions, if needed
        for (int i = 0; i < HEIGHT; ++i) {
            for (int j = 0; j < WIDTH; ++j) {
                board[i][j] = ' '; // Assuming ' ' is your default empty cell
            }
        }
        North = East = South = West = -1; // Assuming -1 means uninitialized or no path
        VB = HB = -1;
    }
};

class NPC {
public:
    int x;
    int y;
    int cost;
    int dx;  // Direction in the x-axis
    int dy;  // Direction in the y-axis
    char type; // 'h' for hiker, 'r' for rival, etc.
    bool is_direction_initialized; // Flag to indicate if the direction is initialized
    bool is_defeated;
    std::vector<PokemonWithMoves> pokemons;

    // Default constructor
    NPC() : x(0), y(0), cost(0), dx(0), dy(0), type(' '), is_direction_initialized(false), is_defeated(false) {}

    // Parameterized constructor
    NPC(int x, int y, int cost, int dx, int dy, char type, bool is_direction_initialized, bool is_defeated)
        : x(x), y(y), cost(cost), dx(dx), dy(dy), type(type), is_direction_initialized(is_direction_initialized), is_defeated(is_defeated) {}
};

map *worldMap[MAP_SIZE][MAP_SIZE]; // Represents the world map as a 2D array of pointers to maps
map *firstLayer[MAP_SIZE][MAP_SIZE];

// Adds a new node with given coordinates and cost to the priority queue.
void pq_add(PriorityQueueNode pq[], int *pq_size, int x, int y, int cost)
{
    pq[*pq_size] = (PriorityQueueNode){x, y, cost};
    (*pq_size)++;
}

// Extracts and returns the node with the minimum cost from the priority queue.
PriorityQueueNode pq_extract_min(PriorityQueueNode pq[], int *pq_size)
{
    int minIndex = 0;
    for (int i = 1; i < *pq_size; i++)
    {
        if (pq[i].cost < pq[minIndex].cost)
        {
            minIndex = i;
        }
    }
    PriorityQueueNode minNode = pq[minIndex];
    for (int i = minIndex; i < *pq_size - 1; i++)
    {
        pq[i] = pq[i + 1];
    }
    (*pq_size)--;
    return minNode;
}

// Updates the cost of a node in the priority queue if the new cost is lower.
void pq_update_priority(PriorityQueueNode pq[], int pq_size, int x, int y, int new_cost)
{
    for (int i = 0; i < pq_size; i++)
    {
        if (pq[i].x == x && pq[i].y == y && pq[i].cost > new_cost)
        {
            pq[i].cost = new_cost;
            break;
        }
    }
}

// Checks if a given cell at (x, y) is part of a forest by counting adjacent trees.
int is_forest(map *worldMap, int x, int y)
{
    int treeCount = 0;
    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            if (dx == 0 && dy == 0)
                continue;
            int newX = x + dx;
            int newY = y + dy;
            if (newX >= 0 && newX < WIDTH && newY >= 0 && newY < HEIGHT && worldMap->board[newY][newX] == TREE)
            {
                treeCount++;
            }
        }
    }
    return (treeCount > 1);
}

// Checks if a given path at (x, y) is a gate by being adjacent to two borders.
int is_gate(map *worldMap, int x, int y)
{
    if (worldMap->board[y][x] != PATH)
    {
        return 0; // Not a path, so not a gate
    }

    // Check if the path is between two borders
    if ((x == 0 && worldMap->board[y + 1][x] == BORDER && worldMap->board[y - 1][x] == BORDER) ||
        (x == WIDTH - 1 && worldMap->board[y + 1][x] == BORDER && worldMap->board[y - 1][x] == BORDER) ||
        (y == 0 && worldMap->board[y][x + 1] == BORDER && worldMap->board[y][x - 1] == BORDER) ||
        (y == HEIGHT - 1 && worldMap->board[y][x + 1] == BORDER && worldMap->board[y][x - 1] == BORDER))
    {
        return 1; // It's a gate
    }

    return 0; // Not a gate
}

// Calculates the movement cost for a trainer type at a given cell (x, y) based on terrain.
int get_movement_cost(map *worldMap, int x, int y, char trainer_type)
{
    char terrain = worldMap->board[y][x];
    switch (terrain)
    {
    case BORDER:
        return SHRT_MAX; // Impassable for all trainers
    case TREE:
        if (is_forest(worldMap, x, y))
        {
            if (trainer_type == HIKER)
            {
                return 15; // Cost for hikers moving through a forest
            }
            else
            {
                return SHRT_MAX; // Impassable for other trainers
            }
        }
        else
        {
            return SHRT_MAX; // Single Trees are impassable for all trainers
        }
    case WATER:
        return SHRT_MAX; // Impassable for all trainers except Swimmers
    case PATH:
        if (is_gate(worldMap, x, y) && trainer_type != PC)
        {
            return SHRT_MAX; // Gates are impassable for NPCs, only PC can pass the gate
        }
        else
        {
            return 10; // Cost for moving on a path
        }
    case POKEMART:
        if (trainer_type != PC)
        {
            return 50;
        }
        else
        {
            return 10;
        }
    case POKEMON_CENTER:
        if (trainer_type != PC)
        {
            return 50;
        }
        else
        {
            return 10;
        }
    case TALL_GRASS:
        if (trainer_type == HIKER)
        {
            return 15; // Cost for hikers moving on tall grass
        }
        else
        {
            return 20; // Cost for rivals moving on tall grass
        }
    case GRASS:
        return 10; // Cost for moving on grass for all trainers except Swimmers
    default:
        return SHRT_MAX; // Default case for unrecognized terrain or impassable for all trainers
    }
}

// Implements Dijkstra's algorithm using a priority queue to find the shortest paths from (srcX, srcY).
void dijkstra_with_priority_queue(map *worldMap, int srcX, int srcY, int dist[HEIGHT][WIDTH], char trainer_type)
{
    PriorityQueueNode pq[HEIGHT * WIDTH];
    int pq_size = 0;

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            dist[y][x] = SHRT_MAX;
            pq_add(pq, &pq_size, x, y, dist[y][x]);
        }
    }

    dist[srcY][srcX] = 0;
    pq_update_priority(pq, pq_size, srcX, srcY, dist[srcY][srcX]);

    while (pq_size > 0)
    {
        PriorityQueueNode u = pq_extract_min(pq, &pq_size);

        for (int dy = -1; dy <= 1; dy++)
        {
            for (int dx = -1; dx <= 1; dx++)
            {
                if (dy == 0 && dx == 0)
                    continue;

                int vY = u.y + dy;
                int vX = u.x + dx;

                if (vY >= 0 && vY < HEIGHT && vX >= 0 && vX < WIDTH)
                {
                    int cost = get_movement_cost(worldMap, vX, vY, trainer_type);
                    if (cost != SHRT_MAX && dist[u.y][u.x] + cost < dist[vY][vX])
                    {
                        dist[vY][vX] = dist[u.y][u.x] + cost;
                        pq_update_priority(pq, pq_size, vX, vY, dist[vY][vX]);
                    }
                }
            }
        }
    }
}

// Function to generate circular areas of terrain types with random number of regions and random sizes
void growRegion(char board[HEIGHT][WIDTH], char terrainType)
{
    int numRegions = rand() % (MAX_REGIONS - MIN_REGIONS + 1) + MIN_REGIONS; // Random number of regions (5 to 13)

    for (int region = 0; region < numRegions; ++region)
    {
        int centerX = rand() % WIDTH;  // Random x-coordinate within the map
        int centerY = rand() % HEIGHT; // Random y-coordinate within the map

        int radius = rand() % (MAX_RADIUS - MIN_RADIUS + 1) + MIN_RADIUS; // Random radius for the circular region

        for (int i = centerY - radius; i <= centerY + radius; ++i)
        {
            for (int j = centerX - radius; j <= centerX + radius; ++j)
            {
                // Check if the current position is within the circular region
                if ((i - centerY) * (i - centerY) + (j - centerX) * (j - centerX) <= radius * radius)
                {
                    // Ensure the position is within the bounds of the map
                    if (i >= 0 && i < HEIGHT && j >= 0 && j < WIDTH)
                    {
                        // Place the terrain type in the circular region
                        if (board[i][j] == GRASS || board[i][j] == TREE)
                        {
                            board[i][j] = terrainType;
                        }
                    }
                }
            }
        }
    }
}

// Function to generate the world map
void generateMap(map *world[MAP_SIZE][MAP_SIZE], int x, int y, int preX, int preY, int numtrainers, int flying)
{
    // Check if the map at coordinates (x, y) is not already generated
    if (world[y][x] == NULL)
    {
        worldMap[y][x] = new map; // Allocate memory for a new map when it is first generated.
        int i, j;

        /*
         * World Generation
         * % Rock
         * . Normal Clearing Grass
         * : Tall Grass
         * ^ Tree
         */
        for (i = 0; i < HEIGHT; ++i)
        {
            for (j = 0; j < WIDTH; ++j)
            {
                // Generate borders and terrain types randomly
                if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1)
                {
                    world[y][x]->board[i][j] = BORDER; // Border
                }
                else
                {
                    int random = rand() % 100;

                    if (random < 98)
                    {
                        // Initialize 98% of the cells as grass
                        world[y][x]->board[i][j] = GRASS;
                    }
                    else
                    {
                        // Place individual trees
                        world[y][x]->board[i][j] = TREE;
                    }
                }
            }
        }

        // Region growing for water
        growRegion(world[y][x]->board, WATER); // Generate circular regions of water

        // Region growing for tall grass
        growRegion(world[y][x]->board, TALL_GRASS); // Generate circular regions of tall grass

        // Region growing for forests
        growRegion(world[y][x]->board, TREE); // Generate circular regions of forests

        // Generate additional borders to ensure nothing goes out of line
        for (i = 0; i < HEIGHT; ++i)
        {
            for (j = 0; j < WIDTH; ++j)
            {
                if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1)
                {
                    world[y][x]->board[i][j] = BORDER; // Border
                }
            }
        }

        // Road placement
        int North, South, East, West;

        // For the North road:
        if (y > 0 && world[y - 1][x] != NULL)
        {
            // If the map to the north exists, use its southern road position
            North = world[y - 1][x]->South;
        }
        else
        {
            // If there is no map to the north or we're at the edge, generate a random position
            North = rand() % (WIDTH - 4) + 2;
        }

        // For the South road:
        if (y < MAP_SIZE - 1 && world[y + 1][x] != NULL)
        {
            // If the map to the south exists, use its northern road position
            South = world[y + 1][x]->North;
        }
        else
        {
            // If there is no map to the south or we're at the edge, generate a random position
            South = rand() % (WIDTH - 4) + 2;
        }

        // For the East road:
        if (x < MAP_SIZE - 1 && world[y][x + 1] != NULL)
        {
            // If the map to the east exists, use its western road position
            East = world[y][x + 1]->West;
        }
        else
        {
            // If there is no map to the east or we're at the edge, generate a random position
            East = rand() % (HEIGHT - 4) + 2;
        }

        // For the West road:
        if (x > 0 && world[y][x - 1] != NULL)
        {
            // If the map to the west exists, use its eastern road position
            West = world[y][x - 1]->East;
        }
        else
        {
            // If there is no map to the west or we're at the edge, generate a random position
            West = rand() % (HEIGHT - 4) + 2;
        }

        // Minimum gap between gates
        int minGap = 2;

        if (abs(North - South) < minGap)
        {
            // Adjust the positions to ensure minimum gap
            if (North < South)
            {
                if (South - minGap < 3)
                {
                    North += minGap * 2;
                }
                else
                {
                    North = South - minGap;
                }
            }
            else
            {
                if (North - minGap < 3)
                {
                    South += minGap * 2;
                }
                else
                {
                    South = North - minGap;
                }
            }
        }

        if (abs(West - East) < minGap)
        {
            // Adjust the positions to ensure minimum gap
            if (West < East)
            {
                if (East - minGap < 3)
                {
                    West += minGap * 2;
                }
                else
                {
                    West = East - minGap;
                }
            }
            else
            {
                if (West - minGap < 3)
                {
                    East += minGap * 2;
                }
                else
                {
                    East = West - minGap;
                }
            }
        }

        // Update the current map's path positions
        world[y][x]->North = North;
        world[y][x]->South = South;
        world[y][x]->East = East;
        world[y][x]->West = West;

        // Adding Roads at their correct positions
        // Generate a random vertical breaking point between 1/4 and 3/4 of the map height
        int VerticalBreakingPoint = rand() % ((HEIGHT - 1) / 2) + ((HEIGHT - 1) / 4);

        // Ensure the vertical breaking point doesn't overlap with the E-W path
        while (VerticalBreakingPoint == West || VerticalBreakingPoint == East)
        {
            VerticalBreakingPoint = rand() % ((HEIGHT - 1) / 2) + ((HEIGHT - 1) / 4);
        }

        // Add path segments above the breaking point
        for (i = 0; i < VerticalBreakingPoint; i++)
        {
            world[y][x]->board[i][North] = PATH;
        }
        // Add path segments below the breaking point
        for (i = HEIGHT - 1; i > VerticalBreakingPoint; i--)
        {
            world[y][x]->board[i][South] = PATH;
        }

        // Fill in the path vertically from the southern gate to the northern gate, ensuring continuity.
        if (North > South)
        {
            for (i = South; i <= North; i++)
            {
                world[y][x]->board[VerticalBreakingPoint][i] = PATH;
            }
        }
        else
        {
            for (i = North; i <= South; i++)
            {
                world[y][x]->board[VerticalBreakingPoint][i] = PATH;
            }
        }

        // Generate a random horizontal breaking point between 1/4 and 3/4 of the map width
        int HorizontalBreakingPoint = rand() % (WIDTH / 2) + (WIDTH / 4);

        // Ensure the horizontal breaking point does not overlap with the N-S path
        while (HorizontalBreakingPoint == North || HorizontalBreakingPoint == South)
        {
            HorizontalBreakingPoint = rand() % (WIDTH / 2) + (WIDTH / 4);
        }

        // Add path segments to the left of the breaking point
        for (i = 0; i < HorizontalBreakingPoint; i++)
        {
            world[y][x]->board[West][i] = PATH;
        }

        // Add path segments to the right of the breaking point
        for (i = WIDTH - 1; i > HorizontalBreakingPoint; i--)
        {
            world[y][x]->board[East][i] = PATH;
        }

        // Fill in the path horizontally from the western gate to the eastern gate, ensuring continuity.
        if (West > East)
        {
            for (i = East; i <= West; i++)
            {
                world[y][x]->board[i][HorizontalBreakingPoint] = PATH;
            }
        }
        else
        {
            for (i = West; i <= East; i++)
            {
                world[y][x]->board[i][HorizontalBreakingPoint] = PATH;
            }
        }

        world[y][x]->VB = VerticalBreakingPoint;
        world[y][x]->HB = HorizontalBreakingPoint;

        // Place borders at the edges of the map to indicate its boundaries.
        // Borders at (-200, y)
        if (x == 0)
        {
            world[y][x]->board[West][0] = BORDER;
        }

        // Borders at (200, y)
        if (x == MAP_SIZE - 1)
        {
            world[y][x]->board[East][WIDTH - 1] = BORDER;
        }
        
        // Borders at (x, -200)
        if (y == 0)
        {
            world[y][x]->board[0][North] = BORDER;
        }

        // Borders at (x, 200)
        if (y == MAP_SIZE - 1)
        {
            world[y][x]->board[HEIGHT - 1][South] = BORDER;
        }

        // Add PokeMart and PokeCenter with a probability based on the distance from the center
        double manDist, chance;

        manDist = abs(preX - x) + abs(preY - y);
        chance = ((-45.0 * manDist) / 200.0) + 50;

        int randTop, randLeft, randBottom, randRight;
        int l = 1, m = 1;

        int buildingOffset = rand() % 8;
        int random = rand() % 4;
        randTop = rand() % (VerticalBreakingPoint - 1) + 1;
        randLeft = rand() % (HorizontalBreakingPoint - 1) + 1;
        randBottom = rand() % (HEIGHT - 1 - VerticalBreakingPoint) - 1;
        randRight = rand() % (WIDTH - 1 - HorizontalBreakingPoint) - 1;

        int firstArray[4] = {1, 1, -1, -1};
        int secondArray[4] = {1, -1, 1, -1};

        // Placing the Pokemart and Pokemon Center to Random Spots next to paths
        if (x == MAP_CENTER && y == MAP_CENTER)
        {
            if (buildingOffset == 0)
            {
                while (world[y][x]->board[randTop + l][North + firstArray[random]] == PATH)
                {
                    l--;
                }
                world[y][x]->board[randTop + l][North + firstArray[random]] = POKEMART;

                while (world[y][x]->board[West + secondArray[random]][randLeft + m] == PATH)
                {
                    m--;
                }
                world[y][x]->board[West + secondArray[random]][randLeft + m] = POKEMON_CENTER;
            }
            else if (buildingOffset == 1)
            {
                while (world[y][x]->board[randTop + l][North + firstArray[random]] == PATH)
                {
                    l--;
                }
                world[y][x]->board[randTop + l][North + firstArray[random]] = POKEMART;

                while (world[y][x]->board[East + secondArray[random]][HorizontalBreakingPoint + randRight + m] == PATH)
                {
                    m++;
                }
                world[y][x]->board[East + secondArray[random]][HorizontalBreakingPoint + randRight + m] = POKEMON_CENTER;
            }
            else if (buildingOffset == 2)
            {
                while (world[y][x]->board[VerticalBreakingPoint + randBottom + l][South + firstArray[random]] == PATH)
                {
                    l++;
                }
                world[y][x]->board[VerticalBreakingPoint + randBottom + l][South + firstArray[random]] = POKEMART;

                while (world[y][x]->board[West + secondArray[random]][randLeft + m] == PATH)
                {
                    m--;
                }
                world[y][x]->board[West + secondArray[random]][randLeft + m] = POKEMON_CENTER;
            }
            else if (buildingOffset == 3)
            {
                while (world[y][x]->board[VerticalBreakingPoint + randBottom + l][South + firstArray[random]] == PATH)
                {
                    l++;
                }
                world[y][x]->board[VerticalBreakingPoint + randBottom + l][South + firstArray[random]] = POKEMART;

                while (world[y][x]->board[East + secondArray[random]][HorizontalBreakingPoint + randRight + m] == PATH)
                {
                    m++;
                }
                world[y][x]->board[East + secondArray[random]][HorizontalBreakingPoint + randRight + m] = POKEMON_CENTER;
            }
            else if (buildingOffset == 4)
            {
                while (world[y][x]->board[randTop + l][North + firstArray[random]] == PATH)
                {
                    l--;
                }
                world[y][x]->board[randTop + l][North + firstArray[random]] = POKEMON_CENTER;

                while (world[y][x]->board[West + secondArray[random]][randLeft + m] == PATH)
                {
                    m--;
                }
                world[y][x]->board[West + secondArray[random]][randLeft + m] = POKEMART;
            }
            else if (buildingOffset == 5)
            {
                while (world[y][x]->board[randTop + l][North + firstArray[random]] == PATH)
                {
                    l--;
                }
                world[y][x]->board[randTop + l][North + firstArray[random]] = POKEMON_CENTER;

                while (world[y][x]->board[East + secondArray[random]][HorizontalBreakingPoint + randRight + m] == PATH)
                {
                    m++;
                }
                world[y][x]->board[East + secondArray[random]][HorizontalBreakingPoint + randRight + m] = POKEMART;
            }
            else if (buildingOffset == 6)
            {
                while (world[y][x]->board[VerticalBreakingPoint + randBottom + l][South + firstArray[random]] == PATH)
                {
                    l++;
                }
                world[y][x]->board[VerticalBreakingPoint + randBottom + l][South + firstArray[random]] = POKEMON_CENTER;

                while (world[y][x]->board[West + secondArray[random]][randLeft + m] == PATH)
                {
                    m--;
                }
                world[y][x]->board[West + secondArray[random]][randLeft + m] = POKEMART;
            }
            else if (buildingOffset == 7)
            {
                while (world[y][x]->board[VerticalBreakingPoint + randBottom + l][South + firstArray[random]] == PATH)
                {
                    l++;
                }
                world[y][x]->board[VerticalBreakingPoint + randBottom + l][South + firstArray[random]] = POKEMON_CENTER;

                while (world[y][x]->board[East + secondArray[random]][HorizontalBreakingPoint + randRight + m] == PATH)
                {
                    m++;
                }
                world[y][x]->board[East + secondArray[random]][HorizontalBreakingPoint + randRight + m] = POKEMART;
            }
        }
        else
        {
            if (rand() % 101 < chance)
            {
                if (buildingOffset == 0)
                {
                    while (world[y][x]->board[randTop + l][North + firstArray[random]] == PATH)
                    {
                        l--;
                    }
                    world[y][x]->board[randTop + l][North + firstArray[random]] = POKEMART;
                }
                else if (buildingOffset == 1)
                {
                    while (world[y][x]->board[randTop + l][North + firstArray[random]] == PATH)
                    {
                        l--;
                    }
                    world[y][x]->board[randTop + l][North + firstArray[random]] = POKEMART;
                }
                else if (buildingOffset == 2)
                {
                    while (world[y][x]->board[VerticalBreakingPoint + randBottom + l][South + firstArray[random]] == PATH)
                    {
                        l++;
                    }
                    world[y][x]->board[VerticalBreakingPoint + randBottom + l][South + firstArray[random]] = POKEMART;
                }
                else if (buildingOffset == 3)
                {
                    while (world[y][x]->board[VerticalBreakingPoint + randBottom + l][South + firstArray[random]] == PATH)
                    {
                        l++;
                    }
                    world[y][x]->board[VerticalBreakingPoint + randBottom + l][South + firstArray[random]] = POKEMART;
                }
                else if (buildingOffset == 4)
                {
                    while (world[y][x]->board[West + secondArray[random]][randLeft + m] == PATH)
                    {
                        m--;
                    }
                    world[y][x]->board[West + secondArray[random]][randLeft + m] = POKEMART;
                }
                else if (buildingOffset == 5)
                {
                    while (world[y][x]->board[East + secondArray[random]][HorizontalBreakingPoint + randRight + m] == PATH)
                    {
                        m++;
                    }
                    world[y][x]->board[East + secondArray[random]][HorizontalBreakingPoint + randRight + m] = POKEMART;
                }
                else if (buildingOffset == 6)
                {
                    while (world[y][x]->board[West + secondArray[random]][randLeft + m] == PATH)
                    {
                        m--;
                    }
                    world[y][x]->board[West + secondArray[random]][randLeft + m] = POKEMART;
                }
                else if (buildingOffset == 7)
                {
                    while (world[y][x]->board[East + secondArray[random]][HorizontalBreakingPoint + randRight + m] == PATH)
                    {
                        m++;
                    }
                    world[y][x]->board[East + secondArray[random]][HorizontalBreakingPoint + randRight + m] = POKEMART;
                }
            }

            l = 1, m = 1;

            if (rand() % 101 < chance)
            {
                if (buildingOffset == 0)
                {
                    while (world[y][x]->board[West + secondArray[random]][randLeft + m] == PATH)
                    {
                        m--;
                    }
                    world[y][x]->board[West + secondArray[random]][randLeft + m] = POKEMON_CENTER;
                }
                else if (buildingOffset == 1)
                {
                    while (world[y][x]->board[East + secondArray[random]][HorizontalBreakingPoint + randRight + m] == PATH)
                    {
                        m++;
                    }
                    world[y][x]->board[East + secondArray[random]][HorizontalBreakingPoint + randRight + m] = POKEMON_CENTER;
                }
                else if (buildingOffset == 2)
                {
                    while (world[y][x]->board[West + secondArray[random]][randLeft + m] == PATH)
                    {
                        m--;
                    }
                    world[y][x]->board[West + secondArray[random]][randLeft + m] = POKEMON_CENTER;
                }
                else if (buildingOffset == 3)
                {
                    while (world[y][x]->board[East + secondArray[random]][HorizontalBreakingPoint + randRight + m] == PATH)
                    {
                        m++;
                    }
                    world[y][x]->board[East + secondArray[random]][HorizontalBreakingPoint + randRight + m] = POKEMON_CENTER;
                }
                else if (buildingOffset == 4)
                {
                    while (world[y][x]->board[randTop + l][North + firstArray[random]] == PATH)
                    {
                        l--;
                    }
                    world[y][x]->board[randTop + l][North + firstArray[random]] = POKEMON_CENTER;
                }
                else if (buildingOffset == 5)
                {
                    while (world[y][x]->board[randTop + l][North + firstArray[random]] == PATH)
                    {
                        l--;
                    }
                    world[y][x]->board[randTop + l][North + firstArray[random]] = POKEMON_CENTER;
                }
                else if (buildingOffset == 6)
                {
                    while (world[y][x]->board[VerticalBreakingPoint + randBottom + l][South + firstArray[random]] == PATH)
                    {
                        l++;
                    }
                    world[y][x]->board[VerticalBreakingPoint + randBottom + l][South + firstArray[random]] = POKEMON_CENTER;
                }
                else if (buildingOffset == 7)
                {
                    while (world[y][x]->board[VerticalBreakingPoint + randBottom + l][South + firstArray[random]] == PATH)
                    {
                        l++;
                    }
                    world[y][x]->board[VerticalBreakingPoint + randBottom + l][South + firstArray[random]] = POKEMON_CENTER;
                }
            }
        }

        // Allocate a new map structure for the first layer
        firstLayer[y][x] = new map;

        // Copy the contents of the current map to the first layer
        for (int i = 0; i < HEIGHT; ++i) {
            for (int j = 0; j < WIDTH; ++j) {
                firstLayer[y][x]->board[i][j] = worldMap[y][x]->board[i][j];
            }
        }

        buildingOffset = rand() % 4;
        randTop = rand() % (VerticalBreakingPoint - 1) + 1;
        randLeft = rand() % (HorizontalBreakingPoint - 1) + 1;
        randBottom = rand() % (HEIGHT - 1 - VerticalBreakingPoint) - 1;
        randRight = rand() % (WIDTH - 1 - HorizontalBreakingPoint) - 1;
        l = 1, m = 1;

        if ((x == MAP_CENTER && y == MAP_CENTER) || (flying == 1)){
            if (buildingOffset == 0)
            {
                while (world[y][x]->board[West][randLeft + m] != PATH)
                {
                    m--;
                }
                world[y][x]->board[West][randLeft + m] = PC;
            }
            else if (buildingOffset == 1)
            {
                while (world[y][x]->board[East][HorizontalBreakingPoint + randRight + m] != PATH)
                {
                    m++;
                }
                world[y][x]->board[East][HorizontalBreakingPoint + randRight + m] = PC;
            }
            else if (buildingOffset == 2)
            {
                while (world[y][x]->board[randTop + l][North] != PATH)
                {
                    l--;
                }
                world[y][x]->board[randTop + l][North] = PC;
            }
            else if (buildingOffset == 3)
            {
                while (world[y][x]->board[VerticalBreakingPoint + randBottom + l][South] != PATH)
                {
                    l++;
                }
                world[y][x]->board[VerticalBreakingPoint + randBottom + l][South] = PC;
            }
        }

        int random_x, random_y, random_case;
        
        // Ensure there is at least one hiker and one rival on the map unless numtrainers is less than 2
        if (numtrainers == 2) {
            random_x = rand() % (WIDTH - 2) + 1;
            random_y = rand() % (HEIGHT - 2) + 1;

            while (world[y][x]->board[random_y][random_x] != GRASS && world[y][x]->board[random_y][random_x] != TALL_GRASS && world[y][x]->board[random_y][random_x] != PATH && !(world[y][x]->board[random_y][random_x] == TREE && is_forest(world[y][x], random_x, random_y)))
            {
                random_x = rand() % (WIDTH - 2) + 1;
                random_y = rand() % (HEIGHT - 2) + 1;
            }

            if (world[y][x]->board[random_y][random_x] == GRASS || world[y][x]->board[random_y][random_x] == TALL_GRASS || world[y][x]->board[random_y][random_x] == PATH || (world[y][x]->board[random_y][random_x] == TREE && is_forest(world[y][x], random_x, random_y)))
            {
                world[y][x]->board[random_y][random_x] = HIKER;
            }

            random_x = rand() % (WIDTH - 2) + 1;
            random_y = rand() % (HEIGHT - 2) + 1;

            while (world[y][x]->board[random_y][random_x] != GRASS && world[y][x]->board[random_y][random_x] != TALL_GRASS && world[y][x]->board[random_y][random_x] != PATH)
            {
                random_x = rand() % (WIDTH - 2) + 1;
                random_y = rand() % (HEIGHT - 2) + 1;
            }

            if (world[y][x]->board[random_y][random_x] == GRASS || world[y][x]->board[random_y][random_x] == TALL_GRASS || world[y][x]->board[random_y][random_x] == PATH)
            {
                world[y][x]->board[random_y][random_x] = RIVAL;
            }
        } else if (numtrainers > 2) {
            random_x = rand() % (WIDTH - 2) + 1;
            random_y = rand() % (HEIGHT - 2) + 1;
            int maxAttempts = WIDTH * HEIGHT;
            int attempts = 0;

            while (attempts < maxAttempts && world[y][x]->board[random_y][random_x] != GRASS && world[y][x]->board[random_y][random_x] != TALL_GRASS && world[y][x]->board[random_y][random_x] != PATH && !(world[y][x]->board[random_y][random_x] == TREE && is_forest(world[y][x], random_x, random_y)))
            {
                random_x = rand() % (WIDTH - 2) + 1;
                random_y = rand() % (HEIGHT - 2) + 1;
                attempts++;
            }

            if (world[y][x]->board[random_y][random_x] == GRASS || world[y][x]->board[random_y][random_x] == TALL_GRASS || world[y][x]->board[random_y][random_x] == PATH || (world[y][x]->board[random_y][random_x] == TREE && is_forest(world[y][x], random_x, random_y)))
            {
                world[y][x]->board[random_y][random_x] = HIKER;
            }

            random_x = rand() % (WIDTH - 2) + 1;
            random_y = rand() % (HEIGHT - 2) + 1;

            attempts = 0;
            while (attempts < maxAttempts && world[y][x]->board[random_y][random_x] != GRASS && world[y][x]->board[random_y][random_x] != TALL_GRASS && world[y][x]->board[random_y][random_x] != PATH)
            {
                random_x = rand() % (WIDTH - 2) + 1;
                random_y = rand() % (HEIGHT - 2) + 1;
                attempts++;
            }

            if (world[y][x]->board[random_y][random_x] == GRASS || world[y][x]->board[random_y][random_x] == TALL_GRASS || world[y][x]->board[random_y][random_x] == PATH)
            {
                world[y][x]->board[random_y][random_x] = RIVAL;
            }

            for (int c = 0; c < numtrainers - 2; c++){
                random_case = rand() % 6;

                if (random_case == 0){
                    random_x = rand() % (WIDTH - 2) + 1;
                    random_y = rand() % (HEIGHT - 2) + 1;

                    attempts = 0;
                    while (attempts < maxAttempts && world[y][x]->board[random_y][random_x] != GRASS && world[y][x]->board[random_y][random_x] != TALL_GRASS && world[y][x]->board[random_y][random_x] != PATH && !(world[y][x]->board[random_y][random_x] == TREE && is_forest(world[y][x], random_x, random_y)))
                    {
                        random_x = rand() % (WIDTH - 2) + 1;
                        random_y = rand() % (HEIGHT - 2) + 1;
                        attempts++;
                    }

                    if (world[y][x]->board[random_y][random_x] == GRASS || world[y][x]->board[random_y][random_x] == TALL_GRASS || world[y][x]->board[random_y][random_x] == PATH || (world[y][x]->board[random_y][random_x] == TREE && is_forest(world[y][x], random_x, random_y)))
                    {
                        world[y][x]->board[random_y][random_x] = HIKER;
                    }
                } else if (random_case == 1){
                    random_x = rand() % (WIDTH - 2) + 1;
                    random_y = rand() % (HEIGHT - 2) + 1;

                    attempts = 0;
                    while (attempts < maxAttempts && world[y][x]->board[random_y][random_x] != GRASS && world[y][x]->board[random_y][random_x] != TALL_GRASS && world[y][x]->board[random_y][random_x] != PATH)
                    {
                        random_x = rand() % (WIDTH - 2) + 1;
                        random_y = rand() % (HEIGHT - 2) + 1;
                        attempts++;
                    }

                    if (world[y][x]->board[random_y][random_x] == GRASS || world[y][x]->board[random_y][random_x] == TALL_GRASS || world[y][x]->board[random_y][random_x] == PATH)
                    {
                        world[y][x]->board[random_y][random_x] = RIVAL;
                    }
                } else if (random_case == 2){
                    random_x = rand() % (WIDTH - 2) + 1;
                    random_y = rand() % (HEIGHT - 2) + 1;

                    attempts = 0;
                    while (attempts < maxAttempts && world[y][x]->board[random_y][random_x] != GRASS && world[y][x]->board[random_y][random_x] != TALL_GRASS && world[y][x]->board[random_y][random_x] != PATH)
                    {
                        random_x = rand() % (WIDTH - 2) + 1;
                        random_y = rand() % (HEIGHT - 2) + 1;
                        attempts++;
                    }

                    if (world[y][x]->board[random_y][random_x] == GRASS || world[y][x]->board[random_y][random_x] == TALL_GRASS || world[y][x]->board[random_y][random_x] == PATH)
                    {
                        world[y][x]->board[random_y][random_x] = PACER;
                    }
                } else if (random_case == 3){
                    random_x = rand() % (WIDTH - 2) + 1;
                    random_y = rand() % (HEIGHT - 2) + 1;

                    attempts = 0;
                    while (attempts < maxAttempts && world[y][x]->board[random_y][random_x] != GRASS && world[y][x]->board[random_y][random_x] != TALL_GRASS && world[y][x]->board[random_y][random_x] != PATH)
                    {
                        random_x = rand() % (WIDTH - 2) + 1;
                        random_y = rand() % (HEIGHT - 2) + 1;
                        attempts++;
                    }

                    if (world[y][x]->board[random_y][random_x] == GRASS || world[y][x]->board[random_y][random_x] == TALL_GRASS || world[y][x]->board[random_y][random_x] == PATH)
                    {
                        world[y][x]->board[random_y][random_x] = WANDERER;
                    }
                } else if (random_case == 4){
                    random_x = rand() % (WIDTH - 2) + 1;
                    random_y = rand() % (HEIGHT - 2) + 1;

                    attempts = 0;
                    while (attempts < maxAttempts && world[y][x]->board[random_y][random_x] != GRASS && world[y][x]->board[random_y][random_x] != TALL_GRASS && world[y][x]->board[random_y][random_x] != PATH)
                    {
                        random_x = rand() % (WIDTH - 2) + 1;
                        random_y = rand() % (HEIGHT - 2) + 1;
                        attempts++;
                    }

                    if (world[y][x]->board[random_y][random_x] == GRASS || world[y][x]->board[random_y][random_x] == TALL_GRASS || world[y][x]->board[random_y][random_x] == PATH)
                    {
                        world[y][x]->board[random_y][random_x] = SENTRY;
                    }
                } else {
                    random_x = rand() % (WIDTH - 2) + 1;
                    random_y = rand() % (HEIGHT - 2) + 1;

                    attempts = 0;
                    while (attempts < maxAttempts && world[y][x]->board[random_y][random_x] != GRASS && world[y][x]->board[random_y][random_x] != TALL_GRASS && world[y][x]->board[random_y][random_x] != PATH)
                    {
                        random_x = rand() % (WIDTH - 2) + 1;
                        random_y = rand() % (HEIGHT - 2) + 1;
                        attempts++;
                    }

                    if (world[y][x]->board[random_y][random_x] == GRASS || world[y][x]->board[random_y][random_x] == TALL_GRASS || world[y][x]->board[random_y][random_x] == PATH)
                    {
                        world[y][x]->board[random_y][random_x] = EXPLORER;
                    }
                }
            }
        } else if (numtrainers == 1) {
            // Place either a hiker or a rival on the map
            // Your code to place either a hiker or a rival...
            random_case = rand() % 2;
            if (random_case == 1){
                random_x = rand() % (WIDTH - 2) + 1;
                random_y = rand() % (HEIGHT - 2) + 1;

                while (world[y][x]->board[random_y][random_x] != GRASS && world[y][x]->board[random_y][random_x] != TALL_GRASS && world[y][x]->board[random_y][random_x] != PATH && !(world[y][x]->board[random_y][random_x] == TREE && is_forest(world[y][x], random_x, random_y)))
                {
                    random_x = rand() % (WIDTH - 2) + 1;
                    random_y = rand() % (HEIGHT - 2) + 1;
                }

                if (world[y][x]->board[random_y][random_x] == GRASS || world[y][x]->board[random_y][random_x] == TALL_GRASS || world[y][x]->board[random_y][random_x] == PATH || (world[y][x]->board[random_y][random_x] == TREE && is_forest(world[y][x], random_x, random_y)))
                {
                    world[y][x]->board[random_y][random_x] = HIKER;
                }
            } else {
                random_x = rand() % (WIDTH - 2) + 1;
                random_y = rand() % (HEIGHT - 2) + 1;

                while (world[y][x]->board[random_y][random_x] != GRASS && world[y][x]->board[random_y][random_x] != TALL_GRASS && world[y][x]->board[random_y][random_x] != PATH)
                {
                    random_x = rand() % (WIDTH - 2) + 1;
                    random_y = rand() % (HEIGHT - 2) + 1;
                }

                if (world[y][x]->board[random_y][random_x] == GRASS || world[y][x]->board[random_y][random_x] == TALL_GRASS || world[y][x]->board[random_y][random_x] == PATH)
                {
                    world[y][x]->board[random_y][random_x] = RIVAL;
                }
            }
        }
    } else if (flying == 1){
        int buildingOffset = rand() % 4;
        int randTop = rand() % (world[y][x]->VB - 1) + 1;
        int randLeft = rand() % (world[y][x]->HB - 1) + 1;
        int randBottom = rand() % (HEIGHT - 1 - world[y][x]->VB) - 1;
        int randRight = rand() % (WIDTH - 1 - world[y][x]->HB) - 1;
        int l = 1, m = 1;

        if ((x == MAP_CENTER && y == MAP_CENTER) || (flying == 1)){
            if (buildingOffset == 0)
            {
                while (world[y][x]->board[world[y][x]->West][randLeft + m] != PATH)
                {
                    m--;
                }
                world[y][x]->board[world[y][x]->West][randLeft + m] = PC;
            }
            else if (buildingOffset == 1)
            {
                while (world[y][x]->board[world[y][x]->East][world[y][x]->HB + randRight + m] != PATH)
                {
                    m++;
                }
                world[y][x]->board[world[y][x]->East][world[y][x]->HB + randRight + m] = PC;
            }
            else if (buildingOffset == 2)
            {
                while (world[y][x]->board[randTop + l][world[y][x]->North] != PATH)
                {
                    l--;
                }
                world[y][x]->board[randTop + l][world[y][x]->North] = PC;
            }
            else if (buildingOffset == 3)
            {
                while (world[y][x]->board[world[y][x]->VB + randBottom + l][world[y][x]->South] != PATH)
                {
                    l++;
                }
                world[y][x]->board[world[y][x]->VB + randBottom + l][world[y][x]->South] = PC;
            }
        }
    }
}

void displayMap(map *world[MAP_SIZE][MAP_SIZE], int x, int y) {
    int i, j;

    start_color(); // Initialize color functionality
    init_pair(1, COLOR_GREEN, COLOR_BLACK);   // Light green for normal grass
    init_pair(2, COLOR_GREEN, COLOR_BLACK);   // Darker green for tall grass (You might need to define a custom color for a darker green)
    init_pair(3, COLOR_GREEN, COLOR_BLACK);   // More dark green for trees (Again, a custom color may be needed)
    init_pair(4, COLOR_CYAN, COLOR_BLACK);    // Light blue for water
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);  // Yellow for paths
    init_pair(6, COLOR_WHITE, COLOR_BLACK);   // Grey for borders (You might need to define a custom color for grey)
    init_pair(7, COLOR_RED, COLOR_BLACK);     // Red for NPCs
    init_pair(8, COLOR_MAGENTA, COLOR_BLACK); // Purple for PC
    init_pair(9, COLOR_YELLOW, COLOR_RED); // Orange for PokeMart and PokeCenters

    // Display the generated map
    for (i = 0; i < HEIGHT; ++i)
    {
        for (j = 0; j < WIDTH; ++j)
        {   
            if (world[y][x]->board[i][j] == GRASS) {
                attron(COLOR_PAIR(1)); // Turn on color pair
                mvprintw(i + 1, j, "%c", world[y][x]->board[i][j]); // Move and print each character
                attroff(COLOR_PAIR(1)); // Turn off color pair
            } else if (world[y][x]->board[i][j] == TALL_GRASS) {
                attron(COLOR_PAIR(2)); // Turn on color pair
                mvprintw(i + 1, j, "%c", world[y][x]->board[i][j]); // Move and print each character
                attroff(COLOR_PAIR(2)); // Turn off color pair
            } else if (world[y][x]->board[i][j] == TREE) {
                attron(COLOR_PAIR(3)); // Turn on color pair
                mvprintw(i + 1, j, "%c", world[y][x]->board[i][j]); // Move and print each character
                attroff(COLOR_PAIR(3)); // Turn off color pair
            } else if (world[y][x]->board[i][j] == WATER) {
                attron(COLOR_PAIR(4)); // Turn on color pair
                mvprintw(i + 1, j, "%c", world[y][x]->board[i][j]); // Move and print each character
                attroff(COLOR_PAIR(4)); // Turn off color pair
            } else if (world[y][x]->board[i][j] == PATH) {
                attron(COLOR_PAIR(5)); // Turn on color pair
                mvprintw(i + 1, j, "%c", world[y][x]->board[i][j]); // Move and print each character
                attroff(COLOR_PAIR(5)); // Turn off color pair
            } else if (world[y][x]->board[i][j] == BORDER) {
                attron(COLOR_PAIR(6)); // Turn on color pair
                mvprintw(i + 1, j, "%c", world[y][x]->board[i][j]); // Move and print each character
                attroff(COLOR_PAIR(6)); // Turn off color pair
            } else if (world[y][x]->board[i][j] == WANDERER || world[y][x]->board[i][j] == EXPLORER || world[y][x]->board[i][j] == RIVAL || world[y][x]->board[i][j] == PACER || world[y][x]->board[i][j] == HIKER || world[y][x]->board[i][j] == SENTRY) {
                attron(COLOR_PAIR(7)); // Turn on color pair
                mvprintw(i + 1, j, "%c", world[y][x]->board[i][j]); // Move and print each character
                attroff(COLOR_PAIR(7)); // Turn off color pair
            } else if (world[y][x]->board[i][j] == PC) {
                attron(COLOR_PAIR(8)); // Turn on color pair
                mvprintw(i + 1, j, "%c", world[y][x]->board[i][j]); // Move and print each character
                attroff(COLOR_PAIR(8)); // Turn off color pair
            } else if (world[y][x]->board[i][j] == POKEMART || world[y][x]->board[i][j] == POKEMON_CENTER) {
                attron(COLOR_PAIR(9)); // Turn on color pair
                mvprintw(i + 1, j, "%c", world[y][x]->board[i][j]); // Move and print each character
                attroff(COLOR_PAIR(9)); // Turn off color pair
            }
        }
    }
    refresh();
}

void move_npc(NPC *npc, map *world[MAP_SIZE][MAP_SIZE], int x, int y, int hikerDist[HEIGHT][WIDTH], int rivalDist[HEIGHT][WIDTH]) {
    int dx[] = {-1, -1, -1, 0, 1, 1, 1, 0};
    int dy[] = {-1, 0, 1, 1, 1, 0, -1, -1};
    
    if ((npc->type == PACER || npc->type == WANDERER || npc->type == EXPLORER) && npc->is_direction_initialized != 1) {
        // Initialize the direction randomly
        int random_direction = rand() % 8;
        npc->dx = dx[random_direction];
        npc->dy = dy[random_direction];
        npc->is_direction_initialized = 1;
    }
    
    int minCost = SHRT_MAX;
    int nextX = npc->x;
    int nextY = npc->y;

    if (npc->is_defeated == false) {
        if(npc->type == HIKER || npc->type == RIVAL){
            for (int direction = 0; direction < 8; direction++) {
                int newX = npc->x + dx[direction];
                int newY = npc->y + dy[direction];

                // Check if the new position is within the map boundaries and not occupied by the PC or another NPC
                if (newX > 0 && newX < WIDTH - 1 && newY > 0 && newY < HEIGHT - 1 && world[y][x]->board[newY][newX] != PC && world[y][x]->board[newY][newX] != HIKER && world[y][x]->board[newY][newX] != RIVAL && world[y][x]->board[newY][newX] != PACER && world[y][x]->board[newY][newX] != WANDERER && world[y][x]->board[newY][newX] != SENTRY && world[y][x]->board[newY][newX] != EXPLORER) {
                    int cost;
                    if (npc->type == HIKER) {
                        cost = hikerDist[newY][newX];
                    } else {
                        cost = rivalDist[newY][newX];
                    }

                    if (cost < minCost) {
                        minCost = cost;
                        nextX = newX;
                        nextY = newY;
                    }
                }
            }
        } else if (npc->type == PACER) {
            int newX = npc->x + npc->dx;
            int newY = npc->y + npc->dy;
            
            if (world[y][x]->board[newY][newX] != BORDER && world[y][x]->board[newY][newX] != TREE && world[y][x]->board[newY][newX] != WATER && !is_gate(worldMap[y][x], newX, newY) && world[y][x]->board[newY][newX] != PC && world[y][x]->board[newY][newX] != HIKER && world[y][x]->board[newY][newX] != RIVAL && world[y][x]->board[newY][newX] != PACER && world[y][x]->board[newY][newX] != WANDERER && world[y][x]->board[newY][newX] != SENTRY && world[y][x]->board[newY][newX] != EXPLORER) {
                nextX = newX;
                nextY = newY;
            } else {
                npc->dx *= -1;
                npc->dy *= -1;
            }
        } else if (npc->type == WANDERER) {
            int newX = npc->x + npc->dx;
            int newY = npc->y + npc->dy;
            int tries = 0;

            while (tries < 8 && ((firstLayer[y][x]->board[newY][newX] != firstLayer[y][x]->board[npc->y][npc->x]) || is_gate(worldMap[y][x], newX, newY) || world[y][x]->board[newY][newX] == BORDER || world[y][x]->board[newY][newX] == PC || world[y][x]->board[newY][newX] == HIKER || world[y][x]->board[newY][newX] == RIVAL || world[y][x]->board[newY][newX] == PACER || world[y][x]->board[newY][newX] == WANDERER || world[y][x]->board[newY][newX] == SENTRY || world[y][x]->board[newY][newX] == EXPLORER)) {
                int random_direction = rand() % 8;
                npc->dx = dx[random_direction];
                npc->dy = dy[random_direction];
                newX = npc->x + npc->dx;
                newY = npc->y + npc->dy;
                tries++;
            }

            if (tries < 8) {
                nextX = newX;
                nextY = newY;
            }
        } else if (npc->type == EXPLORER) {
            int newX = npc->x + npc->dx;
            int newY = npc->y + npc->dy;
            int tries = 0;

            while (tries < 8 && ((world[y][x]->board[newY][newX] == BORDER || world[y][x]->board[newY][newX] == TREE || world[y][x]->board[newY][newX] == WATER) || (is_gate(worldMap[y][x], newX, newY) || world[y][x]->board[newY][newX] == PC || world[y][x]->board[newY][newX] == HIKER || world[y][x]->board[newY][newX] == RIVAL || world[y][x]->board[newY][newX] == PACER || world[y][x]->board[newY][newX] == WANDERER || world[y][x]->board[newY][newX] == SENTRY || world[y][x]->board[newY][newX] == EXPLORER))) {
                int random_direction = rand() % 8;
                npc->dx = dx[random_direction];
                npc->dy = dy[random_direction];
                newX = npc->x + npc->dx;
                newY = npc->y + npc->dy;
                tries++;
            }

            if (tries < 8) {
                nextX = newX;
                nextY = newY;
            }
        }

        // Update the NPC's position
        if (nextX != npc->x || nextY != npc->y) {
            world[y][x]->board[npc->y][npc->x] = firstLayer[y][x]->board[npc->y][npc->x]; // Replace the old position with grass
            npc->x = nextX;
            npc->y = nextY;
            npc->cost = npc->cost + get_movement_cost(world[y][x], nextX, nextY, npc->type);
            world[y][x]->board[nextY][nextX] = npc->type; // Place the NPC at the new position
        }
    }
}

void handleGameOver(std::vector<PokemonWithMoves>& pcPokemons) {
    clear();
    int currentLine = 0;

    // Calculate total experience for the score
    int totalExperience = 0;
    int maxLevelCount = 0;
    for (const auto& pokemon : pcPokemons) {
        totalExperience += pokemon.current_exp;
        if (pokemon.level >= 100) {
            maxLevelCount++;
        }
    }

    // Check if all Pokémon are knocked out
    bool allKnockedOut = true;
    for (const auto& pokemon : pcPokemons) {
        if (!pokemon.is_knocked_out) {
            allKnockedOut = false;
            break;
        }
    }

    if (allKnockedOut && Revives == 0) {
        mvprintw(currentLine++, 0, "All your Pokémon are knocked out! Game Over.");
        mvprintw(currentLine++, 0, "Your score: %d", totalExperience);
        refresh();
        getch();
    } else {
        char response;
        mvprintw(currentLine++, 0, "Are you sure you want to quit? (Y/N): ");
        refresh();
        response = getch();
        if (response == 'Y' || response == 'y') {
            mvprintw(currentLine++, 0, "Quitting the game...");
            mvprintw(currentLine++, 0, "Your score: %d", totalExperience);
            if (static_cast<size_t>(maxLevelCount) == pcPokemons.size()) {
                mvprintw(currentLine++, 0, "Victory! All Pokémon are at max level!");
            }
            refresh();
            getch();
        } else {
            mvprintw(currentLine++, 0, "Continuing game...");
            refresh();
            getch();
            return; // Return to continue the game
        }
    }

    // If game ends, print final messages and close the application or reset the game
    mvprintw(currentLine++, 0, "Press any key to exit...");
    refresh();
    getch();
    endwin(); // Close the ncurses window, adjust as needed if using a different system
    exit(0);  // Exit the program
}


int findGrowthRateId(int speciesId) {
    for (const auto& species : allPokemonSpecies) {
        if (species.id == speciesId) {
            return species.growth_rate_id;
        }
    }
    return -1; // Return an invalid ID if not found
}

int experienceNeeded(int currentLevel, int speciesId) {
    int growthRateId = findGrowthRateId(speciesId);
    for (const auto& exp : allExperiences) {
        if (exp.growth_rate_id == growthRateId && exp.level == currentLevel + 1) {
            return exp.experience;
        }
    }
    return INT_MAX; // If not found, return a large number to prevent leveling up
}

void levelUp(PokemonWithMoves* pokemon) {
    pokemon->level++;
    // Calculate stat increases based on level. These are simple formulas that you can adjust.
    int hpIncrease = 3 + (pokemon->level / 20);  // Increases by 5 plus an additional 1 for every 10 levels.
    int statIncrease = 2 + (pokemon->level / 20); // Increases by 2 plus an additional 1 for every 20 levels.

    // Apply the increases
    pokemon->max_hp += hpIncrease;
    pokemon->attack += statIncrease;
    pokemon->defense += statIncrease;
    pokemon->speed += statIncrease;
    pokemon->hp = pokemon->max_hp / 5; // Heal Pokémon to full health on level up

    // Update the experience needed for the next level
    pokemon->next_level_exp = experienceNeeded(pokemon->level, pokemon->pokemon.species_id);
    
    // Check if level is maxed out and handle accordingly
    if (pokemon->level >= 100) {
        pokemon->next_level_exp = INT_MAX; // No more leveling
    }

    clear();
    mvprintw(0, 0, "%s grew to level %d!", pokemon->pokemon.identifier.c_str(), pokemon->level);
    refresh();
}


void showPokemonList() {
    clear();
    mvprintw(0, 0, "Your Pokemons:");
    for (size_t i = 0; i < pcPokemons.size(); ++i) {
        mvprintw(static_cast<int>(i) + 1, 0, "%d. %s (HP: %d/%d) %s", static_cast<int>(i + 1), pcPokemons[i].pokemon.identifier.c_str(), pcPokemons[i].hp, pcPokemons[i].max_hp, pcPokemons[i].is_knocked_out ? "(Knocked out)" : "");
    }
    refresh();
}

void checkBattleOutcome(NPC* npc, int& currentLine) {
    bool allKnockedOut = std::all_of(npc->pokemons.begin(), npc->pokemons.end(), [](const PokemonWithMoves& mon) { return mon.is_knocked_out; });
    if (allKnockedOut) {
        mvprintw(currentLine++, 0, "You have defeated the trainer!");
    } else {
        mvprintw(currentLine++, 0, "Battle continues...");
    }
}

int determinePokemonLevel(int curX, int curY) {
    // Calculate the Manhattan distance from the center of the map
    double man_distance = abs(curX - MAP_CENTER) + abs(curY - MAP_CENTER);

    // Determine the level of the Pokémon based on the distance
    int level;
    if (man_distance <= 1) {
        level = 1;
    } else if (man_distance > 200) {
        level = (rand() % (int)((100 - ((man_distance - 200) / 2) + 1)) + ((man_distance - 200) / 2));
    } else {
        level = (rand() % (int)(man_distance / 2)) + 1;
    }

    return level;
}

PokemonWithMoves generateRandomPokemonWithMoves(int curX, int curY, int habitatId) {
    std::vector<PokemonSpecies> habitatFilteredSpecies;
    if (habitatId != -1) {
        // Filter Pokemon species by habitat
        for (const auto& species : allPokemonSpecies) {
            if (species.habitat_id == habitatId) {
                habitatFilteredSpecies.push_back(species);
            }
        }
    }

    // If no species are found for this habitat or habitatId is -1, default to using all species
    if (habitatFilteredSpecies.empty()) {
        habitatFilteredSpecies = allPokemonSpecies;
    }

    // Now select a random Pokémon species from the filtered list
    int random_species_index = rand() % habitatFilteredSpecies.size();
    int species_id = habitatFilteredSpecies[random_species_index].id;

    PokemonWithMoves newPokemon;
    newPokemon.pokemon = allPokemons[random_species_index];

    int IV_hp = rand() % 16;
    int IV_attack = rand() % 16;
    int IV_defense = rand() % 16;
    //int IV_special_attack = rand() % 16;
    //int IV_special_defense = rand() % 16;
    int IV_speed = rand() % 16;

    // Determine the level of the Pokémon based on the distance
    newPokemon.level = determinePokemonLevel(curX, curY);

    int base_hp, base_attack, base_defense, /*base_special_attack, base_special_defense,*/ base_speed;
    for (const auto& stat : allPokemonStats) {
        if (stat.pokemon_id == species_id) {
            switch (stat.stat_id) {
                case 1: base_hp = stat.base_stat; break;
                case 2: base_attack = stat.base_stat; break;
                case 3: base_defense = stat.base_stat; break;
                //case 4: base_special_attack = stat.base_stat; break;
                //case 5: base_special_defense = stat.base_stat; break;
                case 6: base_speed = stat.base_stat; break;
            }
        }
    }

    int hp = ((base_hp + IV_hp) * 2 * newPokemon.level) / 100 + newPokemon.level + 10;
    int attack = ((base_attack + IV_attack) * 2 * newPokemon.level) / 100 + 5;
    int defense = ((base_defense + IV_defense) * 2 * newPokemon.level) / 100 + 5;
    //int special_attack = ((base_special_attack + IV_special_attack) * 2 * newPokemon.level) / 100 + 5;
    //int special_defense = ((base_special_defense + IV_special_defense) * 2 * newPokemon.level) / 100 + 5;
    int speed = ((base_speed + IV_speed) * 2 * newPokemon.level) / 100 + 5;

    newPokemon.hp = hp;
    newPokemon.max_hp = hp;
    newPokemon.attack = attack;
    newPokemon.defense = defense;
    newPokemon.speed = speed;
    newPokemon.is_knocked_out = false;
    newPokemon.is_burned = 0;
    newPokemon.is_paralyzed = 0;
    newPokemon.is_poisoned = 0;
    newPokemon.is_asleep = 0;
    newPokemon.is_confused = 0;
    newPokemon.is_frozen = 0;
    newPokemon.current_exp = 0;
    newPokemon.next_level_exp = experienceNeeded(newPokemon.level, newPokemon.pokemon.species_id);

    for (const auto& pokemon_move : allPokemonMoves) {
            if (pokemon_move.pokemon_id == species_id && pokemon_move.level <= newPokemon.level) {
                for (const auto& move : allMoves) {
                    if (move.id == pokemon_move.move_id) {
                        newPokemon.moves.push_back(move.identifier);
                        break;
                    }
                }
                if (newPokemon.moves.size() >= 2) {
                    break; // Limit to 2 moves
                }
            }
        }

    return newPokemon;
}

void chooseStartingPokemon() {
    // Generate three random level 1 Pokémon for the player to choose from
    std::vector<PokemonWithMoves> options;
    int IV_hp = rand() % 16;
    int IV_attack = rand() % 16;
    int IV_defense = rand() % 16;
    //int IV_special_attack = rand() % 16;
    //int IV_special_defense = rand() % 16;
    int IV_speed = rand() % 16;
    for (int i = 0; i < 3; ++i) {
        int random_species_index = rand() % allPokemonSpecies.size();
        int species_id = allPokemonSpecies[random_species_index].id;

        PokemonWithMoves newPokemon;
        newPokemon.pokemon = allPokemons[random_species_index];
        newPokemon.level = 1;
        int base_hp, base_attack, base_defense, /*base_special_attack, base_special_defense,*/ base_speed;
        for (const auto& stat : allPokemonStats) {
            if (stat.pokemon_id == species_id) {
                switch (stat.stat_id) {
                    case 1: base_hp = stat.base_stat; break;
                    case 2: base_attack = stat.base_stat; break;
                    case 3: base_defense = stat.base_stat; break;
                    //case 4: base_special_attack = stat.base_stat; break;
                    //case 5: base_special_defense = stat.base_stat; break;
                    case 6: base_speed = stat.base_stat; break;
                }
            }
        }

        int hp = ((base_hp + IV_hp) * 2 * newPokemon.level) / 100 + newPokemon.level + 10;
        int attack = ((base_attack + IV_attack) * 2 * newPokemon.level) / 100 + 5;
        int defense = ((base_defense + IV_defense) * 2 * newPokemon.level) / 100 + 5;
        //int special_attack = ((base_special_attack + IV_special_attack) * 2 * newPokemon.level) / 100 + 5;
        //int special_defense = ((base_special_defense + IV_special_defense) * 2 * newPokemon.level) / 100 + 5;
        int speed = ((base_speed + IV_speed) * 2 * newPokemon.level) / 100 + 5;

        newPokemon.hp = hp;
        newPokemon.max_hp = hp;
        newPokemon.attack = attack;
        newPokemon.defense = defense;
        newPokemon.speed = speed;
        newPokemon.is_knocked_out = false;
        newPokemon.is_burned = 0;
        newPokemon.is_paralyzed = 0;
        newPokemon.is_poisoned = 0;
        newPokemon.is_asleep = 0;
        newPokemon.is_confused = 0;
        newPokemon.is_frozen = 0;
        newPokemon.current_exp = 0;
        newPokemon.next_level_exp = experienceNeeded(newPokemon.level, newPokemon.pokemon.species_id);

        // Find moves for the Pokémon that it can learn at level 1
        for (const auto& pokemon_move : allPokemonMoves) {
            if (pokemon_move.pokemon_id == species_id && pokemon_move.level == 1) {
                for (const auto& move : allMoves) {
                    if (move.id == pokemon_move.move_id) {
                        newPokemon.moves.push_back(move.identifier);
                        break;
                    }
                }
                if (newPokemon.moves.size() >= 2) {
                    break; // Limit to 2 moves
                }
            }
        }

        // Add the new Pokémon to the options
        options.push_back(newPokemon);
    }

    // Display the options and let the player choose
    clear();
    mvprintw(0, 0, "Choose your starting Pokemon:");
    int currentLine = 1; // Keep track of the current line for printing
    for (size_t i = 0; i < options.size(); ++i) {
        mvprintw(currentLine++, 0, "%d. %s", static_cast<int>(i + 1), options[i].pokemon.identifier.c_str());
        for (size_t j = 0; j < options[i].moves.size(); ++j) {
            mvprintw(currentLine++, 4, "- %s", options[i].moves[j].c_str());
            
        }
    }
    refresh();

    int choice;
    echo();
    mvprintw(currentLine++, 0, "Enter your choice (1-3): ");
    scanw("%d", &choice);
    noecho();

    // Assign the chosen Pokémon to the player's PC
    pcPokemons.push_back(options[choice - 1]);
}

Move* findMove(const std::string& moveName) {
    for (auto& move : allMoves) {
        //std::cerr << "Checking move: " << move.identifier << " against " << moveName << "\n";
        if (move.identifier == moveName) {
            return &move;
        }
    }
    //std::cerr << "Move not found: " << moveName << "\n";
    return nullptr;
}

void resetStatusEffects(PokemonWithMoves* pokemon) {
    pokemon->is_asleep = 0;
    pokemon->is_burned = 0;
    pokemon->is_confused = 0;
    pokemon->is_frozen = 0;
    pokemon->is_paralyzed = 0;
    pokemon->is_poisoned = 0;
}

void executeMove(PokemonWithMoves* attacker, PokemonWithMoves* defender, int moveIndex) {
    clear();
    int currentLine = 0;
    if (moveIndex >= 0 && static_cast<size_t>(moveIndex) < attacker->moves.size()) {
        std::string moveName = attacker->moves[moveIndex];
        Move* selectedMove = findMove(moveName);
        
        if (selectedMove) {
            if (rand() % 100 < selectedMove->accuracy) {
                int attack = attacker->attack; 
                int defense = defender->defense;
                int level = attacker->level;
                int power = selectedMove->power;
                float random = static_cast<float>(rand() % 16 + 85) / 100.0f;
                float stab = 1.0;
                for (const auto& pokemonType : allPokemonTypes) {
                    if (pokemonType.pokemon_id == attacker->pokemon.id && pokemonType.type_id == selectedMove->type_id) {
                        stab = 1.5;
                        break;
                    }
                }
                // Calculate critical hit
                float critical = (rand() % 256) < (attacker->speed / 2) ? 1.5f : 1.0f;
                float type = 1.0;
                int damage = static_cast<int>((2 * level / 5 + 2) * power * attack / defense / 50 + 2) * critical * random * stab * type;

                if (attacker->is_asleep == 1){
                    resetStatusEffects(attacker);
                    resetStatusEffects(defender);
                    defender->is_asleep = 2;
                } else if (attacker->is_burned == 1) {
                    resetStatusEffects(attacker);
                    resetStatusEffects(defender);
                    defender->is_burned = 2;
                } else if (attacker->is_confused == 1){
                    resetStatusEffects(attacker);
                    resetStatusEffects(defender);
                    defender->is_confused = 2;
                } else if (attacker->is_frozen == 1){
                    resetStatusEffects(attacker);
                    resetStatusEffects(defender);
                    defender->is_frozen = 2;
                } else if (attacker->is_paralyzed == 1){
                    resetStatusEffects(attacker);
                    resetStatusEffects(defender);
                    defender->is_paralyzed = 2;
                } else if (attacker->is_poisoned == 1){
                    resetStatusEffects(attacker);
                    resetStatusEffects(defender);
                    defender->is_poisoned = 2;
                }

                int extraDamage = 0;

                if (attacker->is_asleep >= 2) {
                    attacker->is_asleep++;
                } else if (defender->is_burned >= 2) {
                    defender->is_burned++;
                    extraDamage = defender->max_hp / 8;
                } else if (attacker->is_confused >= 2) {
                    attacker->is_confused++;
                } else if (attacker->is_frozen >= 2) {
                    attacker->is_frozen++;
                } else if (attacker->is_paralyzed >= 2) {
                    attacker->is_paralyzed++;
                } else if (defender->is_poisoned >= 2){
                    defender->is_poisoned++;
                    extraDamage = defender->max_hp / 16;
                }

                if (damage <= 0){
                    damage = 0;
                }
                
                if (!(defender->is_burned >= 2 || defender->is_poisoned >= 2 || attacker->is_paralyzed >= 2 || attacker->is_frozen >= 2 || attacker->is_confused >= 2 || attacker->is_asleep >= 2)) {
                    mvprintw(currentLine++, 0, "%s uses %s causing %d damage!", attacker->pokemon.identifier.c_str(), selectedMove->identifier.c_str(), damage);
                } else if (defender->is_poisoned >= 2) {
                    mvprintw(currentLine++, 0, "%s uses %s causing %d damage!", attacker->pokemon.identifier.c_str(), selectedMove->identifier.c_str(), damage);
                    damage += extraDamage;
                    mvprintw(currentLine++, 0, "%s is poisoned, taking extra %d damage!", defender->pokemon.identifier.c_str(), extraDamage);
                } else if (defender->is_burned >= 2) {
                    mvprintw(currentLine++, 0, "%s uses %s causing %d damage!", attacker->pokemon.identifier.c_str(), selectedMove->identifier.c_str(), damage);
                    damage += extraDamage;
                    mvprintw(currentLine++, 0, "%s is burned, taking extra %d damage!", defender->pokemon.identifier.c_str(), extraDamage);
                } else if (attacker->is_paralyzed >= 2) {
                    if (rand() % 4 == 0) { // 25% chance to not move
                        damage = 0;
                        mvprintw(currentLine++, 0, "%s is paralyzed and can't move!", attacker->pokemon.identifier.c_str());
                    } else {
                        mvprintw(currentLine++, 0, "%s uses %s causing %d damage!", attacker->pokemon.identifier.c_str(), selectedMove->identifier.c_str(), damage);
                    }
                } else if (attacker->is_frozen >= 2) {
                    damage = 0;
                    mvprintw(currentLine++, 0, "%s is frozen solid!", attacker->pokemon.identifier.c_str());
                } else if (attacker->is_asleep >= 2) {
                    damage = 0;
                    mvprintw(currentLine++, 0, "%s is asleep and can't attack!", attacker->pokemon.identifier.c_str());
                } else if (attacker->is_confused >= 2) {
                    if (rand() % 2 == 0) { // 50% chance to hurt itself
                        damage = attacker->level / 8; // Self-inflicted damage
                        mvprintw(currentLine++, 0, "%s is confused and hurts itself with %d damage in its confusion!", attacker->pokemon.identifier.c_str(), damage);
                    } else {
                        mvprintw(currentLine++, 0, "%s uses %s causing %d damage!", attacker->pokemon.identifier.c_str(), selectedMove->identifier.c_str(), damage);
                    }
                }
                
                if (!(attacker->is_confused >= 2)) {
                    defender->hp -= damage;
                } else if (attacker->is_confused >= 2) {
                    attacker->hp -= damage;
                }

                if (attacker->is_asleep >= 4) {
                    resetStatusEffects(attacker);
                    resetStatusEffects(defender);
                } else if (defender->is_burned >= 4) {
                    resetStatusEffects(attacker);
                    resetStatusEffects(defender);
                } else if (attacker->is_confused >= 4) {
                    resetStatusEffects(attacker);
                    resetStatusEffects(defender);
                } else if (attacker->is_frozen >= 4){
                    resetStatusEffects(attacker);
                    resetStatusEffects(defender);
                } else if (attacker->is_paralyzed >= 4) {
                    resetStatusEffects(attacker);
                    resetStatusEffects(defender);
                } else if (defender->is_poisoned >= 4) {
                    resetStatusEffects(attacker);
                    resetStatusEffects(defender);
                }

                if (defender->hp <= 0) {
                    mvprintw(currentLine++, 0, "%s is knocked out!", defender->pokemon.identifier.c_str());
                    defender->is_knocked_out = true;
                    int levelDiff = defender->level - attacker->level;
                    if (levelDiff <= 0){
                        levelDiff = 1;
                    }
                    attacker->current_exp += (10 + (10 * (levelDiff)));
                    if (attacker->current_exp >= attacker->next_level_exp) {
                        levelUp(attacker);
                    }
                }
            } else {
                mvprintw(currentLine++, 0, "%s's %s missed!", attacker->pokemon.identifier.c_str(), selectedMove->identifier.c_str());
            }
        } else {
            mvprintw(currentLine++, 0, "Invalid move selected.");
        }
    } else {
        mvprintw(currentLine++, 0, "Invalid move index.");
    }
    mvprintw(currentLine++, 0, "Press any key to continue.");
    refresh();
    getch();  // Pause to view the outcome
    clear();
}

void fight(NPC* npc, int curPokeIndex, int curX, int curY) {
    clear();
    int currentLine = 0;
    int npcIndex = 0;

    // Check if the current Pokémon is knocked out and force a swap if so
    if (pcPokemons[curPokeIndex].is_knocked_out) {
        mvprintw(currentLine++, 0, "Your %s is knocked out! You need to swap Pokemon.", pcPokemons[curPokeIndex].pokemon.identifier.c_str());
        showPokemonList();
        int pokeChoice = getch() - '0' - 1;
        if (pokeChoice >= 0 && static_cast<size_t>(pokeChoice) < pcPokemons.size() && !pcPokemons[pokeChoice].is_knocked_out && pokeChoice != curPokeIndex) {
            curPokeIndex = pokeChoice;
            mvprintw(currentLine++, 0, "Switched to %s.", pcPokemons[curPokeIndex].pokemon.identifier.c_str());
        } else {
            mvprintw(currentLine++, 0, "Invalid selection.");
        }
        return;  // Early return to end the current fight function call
    }

    mvprintw(currentLine++, 0, "Your %s (HP: %d/%d) is ready to fight!", pcPokemons[curPokeIndex].pokemon.identifier.c_str(), pcPokemons[curPokeIndex].hp, pcPokemons[curPokeIndex].max_hp);
    mvprintw(currentLine++, 0, "Select a move:");
    for (size_t i = 0; i < pcPokemons[curPokeIndex].moves.size(); ++i) {
        mvprintw(currentLine++, 0, "%d. %s", static_cast<int>(i + 1), pcPokemons[curPokeIndex].moves[i].c_str());
    }
    mvprintw(currentLine++, 0, "Enter the number of your choice:");
    refresh();

    int moveChoice = getch() - '0' - 1;  // Adjust for correct index
    if (moveChoice >= 0 && static_cast<size_t>(moveChoice) < static_cast<size_t>(pcPokemons[curPokeIndex].moves.size())) {
        for (int i = 0; i < 6; i++){
            if (npc->pokemons[i].is_knocked_out == false){
                executeMove(&pcPokemons[curPokeIndex], &npc->pokemons[i], moveChoice);
                // After player's move, check if the opponent got knocked out to avoid counter attack
                if (!npc->pokemons[i].is_knocked_out) {
                    executeMove(&npc->pokemons[i], &pcPokemons[curPokeIndex], rand() % 2); // Assuming NPC uses the first move
                }
                // Check if the player's Pokémon is knocked out after opponent's move
                if (pcPokemons[curPokeIndex].is_knocked_out) {
                    mvprintw(currentLine++, 0, "%s is knocked out! You need to swap Pokemon.", pcPokemons[curPokeIndex].pokemon.identifier.c_str());
                    refresh();
                    showPokemonList();
                    int pokeChoice = getch() - '0' - 1;
                    if (pokeChoice >= 0 && static_cast<size_t>(pokeChoice) < pcPokemons.size() && !pcPokemons[pokeChoice].is_knocked_out && pokeChoice != curPokeIndex) {
                        curPokeIndex = pokeChoice;
                        mvprintw(currentLine++, 0, "Switched to %s.", pcPokemons[curPokeIndex].pokemon.identifier.c_str());
                    } else {
                        mvprintw(currentLine++, 0, "Invalid selection.");
                    }
                    return;  // Early return to end the current fight function call
                }
                npcIndex = i;
                break;
            }
        }
    } else {
        mvprintw(currentLine++, 0, "Invalid move. Please try again.");
        getch();  // Pause to let user read the message
        fight(npc, curPokeIndex, curX, curY);  // Retry selecting a move
    }

    // Implement logic to check if all player's Pokémon are knocked out
    if (std::all_of(pcPokemons.begin(), pcPokemons.end(), [](const PokemonWithMoves& mon) { return mon.is_knocked_out; })) {
        if (Revives > 0) {
            mvprintw(currentLine++, 0, "All your Pokemon are knocked out! Use a Revive from your Bag.");
        } else {
            mvprintw(currentLine++, 0, "All your Pokemon are knocked out and you have no Revives! Game over!");
            // Implement game over logic
        }
    }
    currentLine = 0;
    // After moves execution, update battle information
    mvprintw(currentLine++, 0, "Your %s (HP: %d/%d)", pcPokemons[curPokeIndex].pokemon.identifier.c_str(), pcPokemons[curPokeIndex].hp, pcPokemons[curPokeIndex].max_hp);
    mvprintw(currentLine++, 0, "Opponent's %s (HP: %d/%d)", npc->pokemons[npcIndex].pokemon.identifier.c_str(), npc->pokemons[npcIndex].hp, npc->pokemons[npcIndex].max_hp); // Assuming NPC has at least one Pokemon
    refresh();
    checkBattleOutcome(npc, currentLine);
    getch();  // Pause to view the outcome
    clear();
}

void handleBagOption() {
    clear();
    int currentLine = 0;
    mvprintw(currentLine++, 0, "Bag Contents:");
    mvprintw(currentLine++, 0, "1. Pokeballs x%d", Pokeballs);
    mvprintw(currentLine++, 0, "2. Potions x%d", Potions);
    mvprintw(currentLine++, 0, "3. Revives x%d", Revives);
    mvprintw(currentLine++, 0, "4. Toxic Orbs x%d", ToxicOrb);
    mvprintw(currentLine++, 0, "5. Light Balls x%d", LightBall);
    mvprintw(currentLine++, 0, "6. Flame Orbs x%d", FlameOrb);
    mvprintw(currentLine++, 0, "7. Freeze Bombs x%d", FreezeBomb);
    mvprintw(currentLine++, 0, "8. Sleep Powders x%d", SleepPowder);
    mvprintw(currentLine++, 0, "9. Confusion Bombs x%d", ConfusionBomb);
    mvprintw(currentLine++, 0, "Choose an item to use (1-9, 0 to cancel):");
    refresh();

    int choice = getch() - '0';
    switch (choice) {
        case 1:
            if (Pokeballs > 0 && currentWildPokemon != nullptr && !currentWildPokemon->is_knocked_out) {
                Pokeballs--;
                pcPokemons.push_back(*currentWildPokemon);
                mvprintw(currentLine++, 0, "Pokemon caught successfully!");
            } else {
                mvprintw(currentLine++, 0, "No Pokeballs left or no wild Pokemon to catch!");
            }
            break;
        case 2:
            if (Potions > 0) {
                showPokemonList();
                mvprintw(currentLine++, 0, "Select a Pokemon to heal:");
                int healChoice = getch() - '0' - 1;
                if (healChoice >= 0 && static_cast<size_t>(healChoice) < pcPokemons.size() && pcPokemons[healChoice].hp > 0) {
                    pcPokemons[healChoice].hp = std::min(pcPokemons[healChoice].max_hp, pcPokemons[healChoice].hp + 20);
                    Potions--;
                    mvprintw(currentLine++, 0, "Healed %s by 20 HP.", pcPokemons[healChoice].pokemon.identifier.c_str());
                } else {
                    mvprintw(currentLine++, 0, "Invalid choice or Pokemon is at full health.");
                }
            } else {
                mvprintw(currentLine++, 0, "No Potions left!");
            }
            break;
        case 3:
            if (Revives > 0) {
                showPokemonList();
                mvprintw(currentLine++, 0, "Select a Pokemon to revive:");
                int reviveChoice = getch() - '0' - 1;
                if (reviveChoice >= 0 && static_cast<size_t>(reviveChoice) < pcPokemons.size() && pcPokemons[reviveChoice].is_knocked_out) {
                    pcPokemons[reviveChoice].hp = pcPokemons[reviveChoice].max_hp / 2;
                    pcPokemons[reviveChoice].is_knocked_out = false;
                    Revives--;
                    mvprintw(currentLine++, 0, "Revived %s.", pcPokemons[reviveChoice].pokemon.identifier.c_str());
                } else {
                    mvprintw(currentLine++, 0, "Invalid choice or Pokemon not knocked out.");
                }
            } else {
                mvprintw(currentLine++, 0, "No Revives left!");
            }
            break;
        case 4:  // Toxic Orbs
            if (ToxicOrb > 0) {
                pcPokemons[curPokeIndex].is_poisoned = 1;
                ToxicOrb--;
                mvprintw(currentLine++, 0, "%s is now holding a Toxic Orb.", pcPokemons[curPokeIndex].pokemon.identifier.c_str());
            } else {
                mvprintw(currentLine++, 0, "No Toxic Orbs left!");
            }
            break;
        case 5:  // Light Balls
            if (LightBall > 0) {
                pcPokemons[curPokeIndex].is_paralyzed = 1;
                LightBall--;
                mvprintw(currentLine++, 0, "%s is now holding a Light Ball.", pcPokemons[curPokeIndex].pokemon.identifier.c_str());
            } else {
                mvprintw(currentLine++, 0, "No Light Balls left!");
            }
            break;
        case 6:  // Flame Orbs
            if (FlameOrb > 0) {
                pcPokemons[curPokeIndex].is_burned = 1;
                FlameOrb--;
                mvprintw(currentLine++, 0, "%s is now holding a Flame Orb.", pcPokemons[curPokeIndex].pokemon.identifier.c_str());
            } else {
                mvprintw(currentLine++, 0, "No Flame Orbs left!");
            }
            break;
        case 7:  // Freeze Bombs
            if (FreezeBomb > 0) {
                pcPokemons[curPokeIndex].is_frozen = 1;
                FreezeBomb--;
                mvprintw(currentLine++, 0, "%s is now holding a Freeze Bomb.", pcPokemons[curPokeIndex].pokemon.identifier.c_str());
            } else {
                mvprintw(currentLine++, 0, "No Freeze Bombs left!");
            }
            break;
        case 8:  // Sleep Powders
            if (SleepPowder > 0) {
                pcPokemons[curPokeIndex].is_asleep = 1;
                SleepPowder--;
                mvprintw(currentLine++, 0, "%s is now holding Sleep Powder.", pcPokemons[curPokeIndex].pokemon.identifier.c_str());
            } else {
                mvprintw(currentLine++, 0, "No Sleep Powders left!");
            }
            break;
        case 9:  // Sleep Powders
            if (ConfusionBomb > 0) {
                pcPokemons[curPokeIndex].is_confused = 1;
                ConfusionBomb--;
                mvprintw(currentLine++, 0, "%s is now holding Confusion Bomb.", pcPokemons[curPokeIndex].pokemon.identifier.c_str());
            } else {
                mvprintw(currentLine++, 0, "No Confusion Bombs left!");
            }
            break;
        case 0:
            mvprintw(currentLine++, 0, "Cancelled item selection.");
            break;
        default:
            mvprintw(currentLine++, 0, "Invalid item selection.");
            break;
    }
    mvprintw(currentLine++, 0, "Press any key to continue.");
    refresh();
    getch();  // Wait for player to read the message
    clear();
}

bool tryToFlee(int trainerSpeed, int wildSpeed, int attempts) {
    if (wildSpeed == 0) return false;  // Prevent division by zero

    int oddsEscape = ((trainerSpeed * 32) / (wildSpeed / 4) % 256) + 30 * attempts;
    if ((rand() % 256) < oddsEscape) {
        return true;  // Successfully fled
    }
    return false;  // Failed to flee
}

void swapPokemon(int index) {
    clear();
    if (index >= 0 && static_cast<size_t>(index) < pcPokemons.size() && !pcPokemons[index].is_knocked_out) {
        curPokeIndex = index;  // Directly update curPokeIndex since tempIndex is not used
        mvprintw(0, 0, "Switched to %s.", pcPokemons[curPokeIndex].pokemon.identifier.c_str());
    } else {
        mvprintw(0, 0, "Invalid choice or Pokemon is knocked out.");
    }
    refresh();
    getch();  // Wait for player to read the message
    clear();
}


void printNPCTrainerDetails(NPC* npc, int curX, int curY) {
    clear();
    int currentLine = 0;
    bool allKnockedOut = true;

    if (npc->is_defeated) {
        mvprintw(currentLine++, 0, "You have already defeated this trainer.");
    } else {
        mvprintw(currentLine++, 0, "You are challenged by %c!", npc->type);
        for (size_t i = 0; i < npc->pokemons.size(); ++i) {
            mvprintw(currentLine++, 0, "Pokemon %d: %s (Level %d)", static_cast<int>(i + 1), npc->pokemons[i].pokemon.identifier.c_str(), npc->pokemons[i].level);
            for (size_t j = 0; j < npc->pokemons[i].moves.size(); ++j) {
                mvprintw(currentLine++, 4, "- %s", npc->pokemons[i].moves[j].c_str());
            }
            if (!npc->pokemons[i].is_knocked_out) {
                allKnockedOut = false;
            }
        }
    }
    refresh();

        int totalExperience = 0;
        int maxLevelCount = 0;
        for (const auto& pokemon : pcPokemons) {
            totalExperience += pokemon.current_exp;
            if (pokemon.level >= 100) {
                maxLevelCount++;
            }
        }

        bool allKnocked = true;
        for (const auto& pokemon : pcPokemons) {
            if (!pokemon.is_knocked_out) {
                allKnocked = false;
                break;
            }
        }
        
        if (allKnocked && Revives == 0) {
            clear();
            currentLine = 0;
            mvprintw(currentLine++, 0, "All your Pokemons are knocked out! Game Over.");
            mvprintw(currentLine++, 0, "Your score: %d", totalExperience);
            refresh();
            getch();
            endwin(); // Close the ncurses window, adjust as needed if using a different system
            exit(0);  // Exit the program
        } else if (allKnocked) {
            mvprintw(0, 0, "All your Pokemon are knocked out! Try to Revive Them.");
        }

    if (!allKnockedOut){
        mvprintw(currentLine++, 0, "Options: 1.Fight 2.Bag 3.Run 4.Swap Pokemon");
        int key = getch();  // Use int to handle special keys correctly
        int pokeChoice = 0;
        switch(key) {
            case '1':
                fight(npc, curPokeIndex, curX, curY);
                break;
            case '2':
                handleBagOption();
                break;
            case '3':
                mvprintw(currentLine++,0, "You can't run away from a Trainer battle.");
                mvprintw(currentLine++, 0, "Press any key to continue.");
                refresh();
                getch();  // Wait for player to read the message
                attemps++;
                break;
            case '4':
                showPokemonList();
                currentLine = pcPokemons.size() + 1;
                pokeChoice = getch() - '0' - 1;
                if (pokeChoice >= 0 && static_cast<size_t>(pokeChoice) < pcPokemons.size() && !pcPokemons[pokeChoice].is_knocked_out && pokeChoice != curPokeIndex) {
                    curPokeIndex = pokeChoice;
                    mvprintw(currentLine++, 0, "Switched to %s.", pcPokemons[curPokeIndex].pokemon.identifier.c_str());
                } else {
                    mvprintw(currentLine++, 0, "Invalid selection.");
                }
                mvprintw(currentLine++, 0, "Press any key to continue.");
                refresh();
                getch();  // Wait for player to read the message
                break;
            case 'Q':
            case 'q':
                handleGameOver(pcPokemons);
                break;
            default:
                mvprintw(currentLine++, 0, "Invalid option. Please choose again.");
                getch();
                break;
        }
        refresh();
    }

    if (allKnockedOut) {
        npc->is_defeated = true; // Mark the NPC as defeated only if all Pokémon are knocked out
        mvprintw(currentLine++, 0, "Trainer is defeated. Press any key to continue.");
    } else {
        mvprintw(currentLine++, 0, "Fight is not over yet. Press any key to continue.");
        printNPCTrainerDetails(npc, curX, curY);
    }

    clear(); // Clear the screen after viewing the details
}

void fightWildPokemon(PokemonWithMoves* pcPokemon, PokemonWithMoves* wildPokemon) {
    clear();
    int currentLine = 0;

    // Check if the current Pokémon is knocked out and force a swap if so
    if (pcPokemons[curPokeIndex].is_knocked_out) {
        mvprintw(currentLine++, 0, "Your %s is knocked out! You need to swap Pokemon.", pcPokemons[curPokeIndex].pokemon.identifier.c_str());
        showPokemonList();
        int pokeChoice = getch() - '0' - 1;
        currentLine++;
        if (pokeChoice >= 0 && static_cast<size_t>(pokeChoice) < pcPokemons.size() && !pcPokemons[pokeChoice].is_knocked_out && pokeChoice != curPokeIndex) {
            curPokeIndex = pokeChoice;
            mvprintw(currentLine++, 0, "Switched to %s.", pcPokemons[curPokeIndex].pokemon.identifier.c_str());
        } else {
            mvprintw(currentLine++, 0, "Invalid selection.");
        }
        return;  // Early return to end the current fight function call
    }

    mvprintw(currentLine++, 0, "Starting fight with wild %s...", wildPokemon->pokemon.identifier.c_str());
    mvprintw(currentLine++, 0, "Select a move:");
    for (size_t i = 0; i < pcPokemon->moves.size(); ++i) {
        mvprintw(currentLine++, 0, "%d. %s", static_cast<int>(i + 1), pcPokemon->moves[i].c_str());
    }
    refresh();
    
    int moveChoice = getch() - '0' - 1; // Adjust for index starting from 0
    if (moveChoice >= 0 && moveChoice < static_cast<int>(pcPokemon->moves.size())) {
        executeMove(pcPokemon, wildPokemon, moveChoice);
        if (!wildPokemon->is_knocked_out) {
            executeMove(wildPokemon, pcPokemon, rand() % 2);
        }
        if (wildPokemon->hp <= 0) {
            mvprintw(currentLine++, 0, "Wild %s fainted!", wildPokemon->pokemon.identifier.c_str());
        }
    } else if (getch() == 'q' || getch() == 'Q'){
        handleGameOver(pcPokemons);
    } else {
        mvprintw(currentLine++, 0, "Invalid move selection. Try again.");
    }
    currentLine = 0;
    // After moves execution, update battle information
    mvprintw(currentLine++, 0, "Your %s (HP: %d/%d)", pcPokemons[curPokeIndex].pokemon.identifier.c_str(), pcPokemons[curPokeIndex].hp, pcPokemons[curPokeIndex].max_hp);
    mvprintw(currentLine++, 0, "Opponent's %s (HP: %d/%d)", wildPokemon->pokemon.identifier.c_str(), wildPokemon->hp, wildPokemon->max_hp); // Assuming NPC has at least one Pokemon
    refresh();
}

void wildPokemonEncounter(std::vector<PokemonWithMoves>& pcPokemons, int activePokeIndex, PokemonWithMoves* wildPokemon, int attempts) {
    clear();
    int currentLine = 0;
    battleContinues = true;
    int pokeChoice;

    while (battleContinues) {
        currentLine = 0;
        mvprintw(currentLine++, 0, "Options: 1.Fight 2.Bag 3.Run 4.Swap Pokemon");
        refresh();

        int choice = getch();
        switch (choice) {
            case '1': // Fight
                if (wildPokemon) {
                    fightWildPokemon(&pcPokemons[activePokeIndex], wildPokemon);
                    if (wildPokemon->hp <= 0) {
                        battleContinues = false;
                    }
                } else {
                    mvprintw(currentLine++, 0, "No wild Pokemon to fight.");
                    battleContinues = false;
                }
                break;
            case '2': // Bag
                handleBagOption();
                break;
            case '3': // Run
                if (tryToFlee(pcPokemons[activePokeIndex].speed, wildPokemon->speed, attempts++)) {
                    mvprintw(currentLine++, 0, "Successfully fled the battle.");
                    battleContinues = false;
                } else {
                    mvprintw(currentLine++, 0, "Failed to flee.");
                }
                break;
            case '4':
                showPokemonList();
                currentLine = pcPokemons.size() + 1;
                pokeChoice = getch() - '0' - 1;
                if (pokeChoice >= 0 && static_cast<size_t>(pokeChoice) < pcPokemons.size() && !pcPokemons[pokeChoice].is_knocked_out && pokeChoice != curPokeIndex) {
                    curPokeIndex = pokeChoice;
                    mvprintw(currentLine++, 0, "Switched to %s.", pcPokemons[curPokeIndex].pokemon.identifier.c_str());
                } else {
                    mvprintw(currentLine++, 0, "Invalid selection.");
                }
                mvprintw(currentLine++, 0, "Press any key to continue.");
                refresh();
                getch();  // Wait for player to read the message
                break;
            case 'Q':
            case 'q':
                handleGameOver(pcPokemons);
                break;
            default:
                mvprintw(currentLine++, 0, "Invalid option. Please choose again.");
                getch();
                break;
        }

        int totalExperience = 0;
        int maxLevelCount = 0;
        for (const auto& pokemon : pcPokemons) {
            totalExperience += pokemon.current_exp;
            if (pokemon.level >= 100) {
                maxLevelCount++;
            }
        }

        bool allKnocked = true;
        for (const auto& pokemon : pcPokemons) {
            if (!pokemon.is_knocked_out) {
                allKnocked = false;
                break;
            }
        }
        
        if (allKnocked && Revives == 0) {
            clear();
            currentLine = 0;
            mvprintw(currentLine++, 0, "All your Pokemons are knocked out! Game Over.");
            mvprintw(currentLine++, 0, "Your score: %d", totalExperience);
            refresh();
            getch();
            endwin(); // Close the ncurses window, adjust as needed if using a different system
            exit(0);  // Exit the program
        } else if (allKnocked) {
            mvprintw(0, 0, "All your Pokemon are knocked out! Try to Revive Them.");
        }

        getch(); // Pause for user to read the outcome of the action
        clear(); // Clear the screen for the next round of actions
    }
}

// Main function that initializes the game, processes user commands, and handles game logic.
int main(int argc, char *argv[])
{
    if (argc == 2) {
        std::string fileType = argv[1];
        std::string filePath;

        // Try to find the file in the first location
        filePath = "/share/cs327/pokedex/pokedex/data/csv/" + fileType + ".csv";
        if (!fileExists(filePath)) {
            // If the file is not found, try the second location
            const char* homeDir = getenv("HOME");
            if (homeDir != nullptr) {
                filePath = std::string(homeDir) + "/.poke327/pokedex/pokedex/data/csv/" + fileType + ".csv";
                if (!fileExists(filePath)) {
                    // If the file is still not found, try the third location
                    filePath = "c:/cs327/pokedex/pokedex/data/csv/" + fileType + ".csv";
                }
            } else {
                // If HOME environment variable isn't set, use the third location
                filePath = "c:/cs327/pokedex/pokedex/data/csv/" + fileType + ".csv";
            }
        }

        // At this point, filePath will either be the path to the found file, or the last location attempted
        if (!fileExists(filePath)) {
            // If the file is not found in any location, print an error message and exit
            std::cerr << "Error: File not found in any of the specified locations." << std::endl;
            return 1;
        }

        if (fileType == "pokemon") {
            std::vector<Pokemon> pokemons = parsePokemonCSV(filePath);
            for (const auto& pokemon : pokemons) {
                std::cout << "ID: " << (pokemon.id != INT_MAX ? std::to_string(pokemon.id) : "N/A")
                        << ", Identifier: " << (!pokemon.identifier.empty() ? pokemon.identifier : "N/A")
                        << ", Species ID: " << (pokemon.species_id != INT_MAX ? std::to_string(pokemon.species_id) : "N/A")
                        << ", Height: " << (pokemon.height != INT_MAX ? std::to_string(pokemon.height) : "N/A")
                        << ", Weight: " << (pokemon.weight != INT_MAX ? std::to_string(pokemon.weight) : "N/A")
                        << ", Base Experience: " << (pokemon.base_experience != INT_MAX ? std::to_string(pokemon.base_experience) : "N/A")
                        << ", Order: " << (pokemon.order != INT_MAX ? std::to_string(pokemon.order) : "N/A")
                        << ", Is Default: " << (pokemon.is_default ? "Yes" : "No")
                        << std::endl;
            }
        } else if (fileType == "moves") {
            std::vector<Move> moves = parseMovesCSV(filePath);
            for (const auto& move : moves) {
                std::cout << "ID: " << (move.id != INT_MAX ? std::to_string(move.id) : "N/A")
                        << ", Identifier: " << (!move.identifier.empty() ? move.identifier : "N/A")
                        << ", Generation ID: " << (move.generation_id != INT_MAX ? std::to_string(move.generation_id) : "N/A")
                        << ", Type ID: " << (move.type_id != INT_MAX ? std::to_string(move.type_id) : "N/A")
                        << ", Power: " << (move.power != INT_MAX ? std::to_string(move.power) : "N/A")
                        << ", PP: " << (move.pp != INT_MAX ? std::to_string(move.pp) : "N/A")
                        << ", Accuracy: " << (move.accuracy != INT_MAX ? std::to_string(move.accuracy) : "N/A")
                        << ", Priority: " << (move.priority != INT_MAX ? std::to_string(move.priority) : "N/A")
                        << ", Target ID: " << (move.target_id != INT_MAX ? std::to_string(move.target_id) : "N/A")
                        << ", Damage Class ID: " << (move.damage_class_id != INT_MAX ? std::to_string(move.damage_class_id) : "N/A")
                        << ", Effect ID: " << (move.effect_id != INT_MAX ? std::to_string(move.effect_id) : "N/A")
                        << ", Effect Chance: " << (move.effect_chance != INT_MAX ? std::to_string(move.effect_chance) : "N/A")
                        << ", Contest Type ID: " << (move.contest_type_id != INT_MAX ? std::to_string(move.contest_type_id) : "N/A")
                        << ", Contest Effect ID: " << (move.contest_effect_id != INT_MAX ? std::to_string(move.contest_effect_id) : "N/A")
                        << ", Super Contest Effect ID: " << (move.super_contest_effect_id != INT_MAX ? std::to_string(move.super_contest_effect_id) : "N/A")
                        << std::endl;
            }
        } else if (fileType == "pokemon_moves") {
            std::vector<PokemonMove> pokemonMoves = parsePokemonMovesCSV(filePath);
            for (const auto& pokemonMove : pokemonMoves) {
                std::cout << "Pokemon ID: " << (pokemonMove.pokemon_id != INT_MAX ? std::to_string(pokemonMove.pokemon_id) : "N/A")
                        << ", Version ID: " << (pokemonMove.version_id != INT_MAX ? std::to_string(pokemonMove.version_id) : "N/A")
                        << ", Move ID: " << (pokemonMove.move_id != INT_MAX ? std::to_string(pokemonMove.move_id) : "N/A")
                        << ", Pokemon Move Method ID: " << (pokemonMove.pokemon_move_method_id != INT_MAX ? std::to_string(pokemonMove.pokemon_move_method_id) : "N/A")
                        << ", Level: " << (pokemonMove.level != INT_MAX ? std::to_string(pokemonMove.level) : "N/A")
                        << ", Order: " << (pokemonMove.order != INT_MAX ? std::to_string(pokemonMove.order) : "N/A")
                        << std::endl;
            }
        } else if (fileType == "pokemon_species") {
            std::vector<PokemonSpecies> pokemonSpecies = parsePokemonSpeciesCSV(filePath);
            for (const auto& species : pokemonSpecies) {
                std::cout << "ID: " << (species.id != INT_MAX ? std::to_string(species.id) : "N/A")
                        << ", Identifier: " << (!species.identifier.empty() ? species.identifier : "N/A")
                        << ", Generation ID: " << (species.generation_id != INT_MAX ? std::to_string(species.generation_id) : "N/A")
                        << ", Evolves From Species ID: " << (species.evolves_from_species_id != INT_MAX ? std::to_string(species.evolves_from_species_id) : "N/A")
                        << ", Evolution Chain ID: " << (species.evolution_chain_id != INT_MAX ? std::to_string(species.evolution_chain_id) : "N/A")
                        << ", Color ID: " << (species.color_id != INT_MAX ? std::to_string(species.color_id) : "N/A")
                        << ", Shape ID: " << (species.shape_id != INT_MAX ? std::to_string(species.shape_id) : "N/A")
                        << ", Habitat ID: " << (species.habitat_id != INT_MAX ? std::to_string(species.habitat_id) : "N/A")
                        << ", Gender Rate: " << (species.gender_rate != INT_MAX ? std::to_string(species.gender_rate) : "N/A")
                        << ", Capture Rate: " << (species.capture_rate != INT_MAX ? std::to_string(species.capture_rate) : "N/A")
                        << ", Base Happiness: " << (species.base_happiness != INT_MAX ? std::to_string(species.base_happiness) : "N/A")
                        << ", Is Baby: " << (species.is_baby ? "Yes" : "No")
                        << ", Hatch Counter: " << (species.hatch_counter != INT_MAX ? std::to_string(species.hatch_counter) : "N/A")
                        << ", Has Gender Differences: " << (species.has_gender_differences ? "Yes" : "No")
                        << ", Growth Rate ID: " << (species.growth_rate_id != INT_MAX ? std::to_string(species.growth_rate_id) : "N/A")
                        << ", Forms Switchable: " << (species.forms_switchable ? "Yes" : "No")
                        << ", Is Legendary: " << (species.is_legendary ? "Yes" : "No")
                        << ", Is Mythical: " << (species.is_mythical ? "Yes" : "No")
                        << ", Order: " << (species.order != INT_MAX ? std::to_string(species.order) : "N/A")
                        << ", Conquest Order: " << (species.conquest_order != INT_MAX ? std::to_string(species.conquest_order) : "N/A")
                        << std::endl;
            }
        } else if (fileType == "experience") {
            std::vector<Experience> experiences = parseExperienceCSV(filePath);
            for (const auto& experience : experiences) {
                std::cout << "Growth Rate ID: " << (experience.growth_rate_id != INT_MAX ? std::to_string(experience.growth_rate_id) : "N/A")
                        << ", Level: " << (experience.level != INT_MAX ? std::to_string(experience.level) : "N/A")
                        << ", Experience: " << (experience.experience != INT_MAX ? std::to_string(experience.experience) : "N/A")
                        << std::endl;
            }
        } else if (fileType == "type_names") {
            std::vector<TypeName> typeNames = parseTypeNamesCSV(filePath);
            for (const auto& typeName : typeNames) {
                if (typeName.local_language_id == 9) { // Check if it's English
                    std::cout << "Type ID: " << (typeName.type_id != INT_MAX ? std::to_string(typeName.type_id) : "N/A")
                            << ", Local Language ID: " << (typeName.local_language_id != INT_MAX ? std::to_string(typeName.local_language_id) : "N/A")
                            << ", Name: " << (!typeName.name.empty() ? typeName.name : "N/A")
                            << std::endl;
                }
            }
        } else if (fileType == "pokemon_stats") {
            std::vector<PokemonStat> pokemonStats = parsePokemonStatsCSV(filePath);
            for (const auto& pokemonStat : pokemonStats) {
                std::cout << "Pokemon ID: " << (pokemonStat.pokemon_id != INT_MAX ? std::to_string(pokemonStat.pokemon_id) : "N/A")
                        << ", Stat ID: " << (pokemonStat.stat_id != INT_MAX ? std::to_string(pokemonStat.stat_id) : "N/A")
                        << ", Base Stat: " << (pokemonStat.base_stat != INT_MAX ? std::to_string(pokemonStat.base_stat) : "N/A")
                        << ", Effort: " << (pokemonStat.effort != INT_MAX ? std::to_string(pokemonStat.effort) : "N/A")
                        << std::endl;
            }
        } else if (fileType == "stats") {
            std::vector<Stat> stats = parseStatsCSV(filePath);
            for (const auto& stat : stats) {
                std::cout << "ID: " << (stat.id != INT_MAX ? std::to_string(stat.id) : "N/A") 
                        << ", Damage Class ID: " << (stat.damage_class_id != INT_MAX ? std::to_string(stat.damage_class_id) : "N/A")
                        << ", Identifier: " << (!stat.identifier.empty() ? stat.identifier : "N/A") 
                        << ", Is Battle Only: " << (stat.is_battle_only ? "Yes" : "No") 
                        << ", Game Index: " << (stat.game_index != INT_MAX ? std::to_string(stat.game_index) : "N/A") 
                        << std::endl;
            }
        } else if (fileType == "pokemon_types") {
            std::vector<PokemonType> pokemonTypes = parsePokemonTypesCSV(filePath);
            for (const auto& pokemonType : pokemonTypes) {
                std::cout << "Pokemon ID: " << (pokemonType.pokemon_id != INT_MAX ? std::to_string(pokemonType.pokemon_id) : "N/A")
                        << ", Type ID: " << (pokemonType.type_id != INT_MAX ? std::to_string(pokemonType.type_id) : "N/A")
                        << ", Slot: " << (pokemonType.slot != INT_MAX ? std::to_string(pokemonType.slot) : "N/A")
                        << std::endl;
            }
        } else {
            std::cerr << "Error: Invalid file type specified." << std::endl;
            return 1;
        }

        return 0; // Exit after parsing and printing the specified CSV file
    }

    loadData();

    // If no CSV parsing argument is provided, proceed with the game initialization
    int numtrainers = 10; // Default value

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--numtrainers") == 0) {
            if (i + 1 < argc) {
                numtrainers = atoi(argv[i + 1]);
                i++; // Skip the next argument as it is the value for --numtrainers
            } else {
                fprintf(stderr, "Error: --numtrainers option requires an argument.\n");
                return 1;
            }
        }
    }

    srand(time(NULL));
    int curX, curY, preX, preY;
    //char userInput;

    // Initialize player's current position to the center of the world
    curX = MAP_CENTER;
    curY = MAP_CENTER;
    preX = curX;
    preY = curY;

    int hikerDist[HEIGHT][WIDTH];
    int rivalDist[HEIGHT][WIDTH];

    generateMap(worldMap, curX, curY, preX, preY, numtrainers, 0);

    // Initialize ncurses
    initscr();
    cbreak();  // Line buffering disabled
    noecho();  // Don't echo while we do getch
    keypad(stdscr, TRUE); // Enable keypad for arrow keys and other special keys

    int ch;
    bool quit_game = false;

    chooseStartingPokemon();

    // Allocate memory for the NPC array
    NPC **npcs[MAP_SIZE][MAP_SIZE];
    // Initialize NPC pointers to NULL
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            npcs[i][j] = NULL;
        }
    }

    npcs[curY][curX] = new NPC*[numtrainers];
    for (int i = 0; i < numtrainers; i++) {
        npcs[curY][curX][i] = new NPC;
    }
    int npcIndex = 0;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (worldMap[curY][curX]->board[i][j] == HIKER || worldMap[curY][curX]->board[i][j] == RIVAL || worldMap[curY][curX]->board[i][j] == PACER || worldMap[curY][curX]->board[i][j] == WANDERER || worldMap[curY][curX]->board[i][j] == SENTRY || worldMap[curY][curX]->board[i][j] == EXPLORER) {
                npcs[curY][curX][npcIndex]->x = j;
                npcs[curY][curX][npcIndex]->y = i;
                npcs[curY][curX][npcIndex]->cost = 0;
                npcs[curY][curX][npcIndex]->type = worldMap[curY][curX]->board[i][j];

                int habitatId = -1;  // Default value indicating no specific habitat is set
                if (firstLayer[curY][curX]->board[i][j] == GRASS || firstLayer[curY][curX]->board[i][j] == TALL_GRASS) {
                    habitatId = 3;  // Grassland
                } else if (firstLayer[curY][curX]->board[i][j] == TREE) {
                    habitatId = 2;  // Forest
                }

                npcs[curY][curX][npcIndex]->is_defeated = false;

                // Generate Pokémon for this NPC
                int numPokemons = 1 + (rand() % 6); // Each NPC has 1 to 6 Pokémon
                for (int k = 0; k < numPokemons; k++) {
                    // Generate a random Pokémon with moves
                    
                    PokemonWithMoves pokemon = generateRandomPokemonWithMoves(curX, curY, habitatId);
                    npcs[curY][curX][npcIndex]->pokemons.push_back(pokemon);
                }
               
                npcIndex++;
            }
        }
    }

    clear();
    displayMap(worldMap, curX, curY);
    refresh();
    //printf("Logical Coordinates of the Current Map: (%d, %d)\n\n", curX - MAP_CENTER, curY - MAP_CENTER);

    int list_offset = 0; // This variable holds the current offset in the list
    bool show_list = true;

    // Main game loop
    while (!quit_game)
    {   
        // Find the position of the PC on the map
        int pcX = -1, pcY = -1;
        for (int i = 0; i < HEIGHT; i++)
        {
            for (int j = 0; j < WIDTH; j++)
            {
                if (worldMap[curY][curX]->board[i][j] == PC)
                {
                    pcX = j;
                    pcY = i;
                    break;
                }
            }
            if (pcX != -1) {
                break;
            }
        }

        dijkstra_with_priority_queue(worldMap[curY][curX], pcX, pcY, hikerDist, HIKER);
        dijkstra_with_priority_queue(worldMap[curY][curX], pcX, pcY, rivalDist, RIVAL);

        for (int i = 0; i < numtrainers; i++) {
            move_npc(npcs[curY][curX][i], worldMap, curX, curY, hikerDist, rivalDist);
        }

        displayMap(worldMap, curX, curY);
        refresh();

        // Get user input
        ch = getch();
        int curLine = 5;

        switch (ch) {
            case '7':
            case 'y':
                // Move up-left
                worldMap[curY][curX]->board[pcY][pcX] = firstLayer[curY][curX]->board[pcY][pcX] ; // Replace the old position with first layer
                if (worldMap[curY][curX]->board[pcY - 1][pcX - 1] != BORDER && worldMap[curY][curX]->board[pcY - 1][pcX - 1] != TREE && worldMap[curY][curX]->board[pcY - 1][pcX - 1] != WATER && worldMap[curY][curX]->board[pcY - 1][pcX - 1] != HIKER && worldMap[curY][curX]->board[pcY - 1][pcX - 1] != RIVAL && worldMap[curY][curX]->board[pcY - 1][pcX - 1] != EXPLORER && worldMap[curY][curX]->board[pcY - 1][pcX - 1] != WANDERER && worldMap[curY][curX]->board[pcY - 1][pcX - 1] != PACER && worldMap[curY][curX]->board[pcY - 1][pcX - 1] != SENTRY) {
                    pcX -= 1;
                    pcY -= 1;
                } else if (worldMap[curY][curX]->board[pcY - 1][pcX - 1] == HIKER || worldMap[curY][curX]->board[pcY - 1][pcX - 1] == RIVAL || worldMap[curY][curX]->board[pcY - 1][pcX - 1] == EXPLORER || worldMap[curY][curX]->board[pcY - 1][pcX - 1] == WANDERER || worldMap[curY][curX]->board[pcY - 1][pcX - 1] == PACER || worldMap[curY][curX]->board[pcY - 1][pcX - 1] == SENTRY) {
                    for (int i = 0; i < numtrainers; i++) {
                        if (npcs[curY][curX][i]->x == pcX - 1 && npcs[curY][curX][i]->y == pcY - 1) {
                            printNPCTrainerDetails(npcs[curY][curX][i], curX, curY);
                            break;
                        }
                    }
                }
                worldMap[curY][curX]->board[pcY][pcX] = PC; // Place the PC at the new position
                break;
            case '8':
            case 'k':
                // Move up
                worldMap[curY][curX]->board[pcY][pcX] = firstLayer[curY][curX]->board[pcY][pcX] ; // Replace the old position with first layer
                if (worldMap[curY][curX]->board[pcY - 1][pcX] != BORDER && worldMap[curY][curX]->board[pcY - 1][pcX] != TREE && worldMap[curY][curX]->board[pcY - 1][pcX] != WATER && worldMap[curY][curX]->board[pcY - 1][pcX] != HIKER && worldMap[curY][curX]->board[pcY - 1][pcX] != RIVAL && worldMap[curY][curX]->board[pcY - 1][pcX] != EXPLORER && worldMap[curY][curX]->board[pcY - 1][pcX] != WANDERER && worldMap[curY][curX]->board[pcY - 1][pcX] != PACER && worldMap[curY][curX]->board[pcY - 1][pcX] != SENTRY) {
                    pcY -= 1;
                } else if (worldMap[curY][curX]->board[pcY - 1][pcX] == HIKER || worldMap[curY][curX]->board[pcY - 1][pcX] == RIVAL || worldMap[curY][curX]->board[pcY - 1][pcX] == EXPLORER || worldMap[curY][curX]->board[pcY - 1][pcX] == WANDERER || worldMap[curY][curX]->board[pcY - 1][pcX] == PACER || worldMap[curY][curX]->board[pcY - 1][pcX] == SENTRY) {
                    for (int i = 0; i < numtrainers; i++) {
                        if (npcs[curY][curX][i]->x == pcX && npcs[curY][curX][i]->y == pcY - 1) {
                            printNPCTrainerDetails(npcs[curY][curX][i], curX, curY);
                            break;
                        }
                    }
                }
                worldMap[curY][curX]->board[pcY][pcX] = PC; // Place the PC at the new position
                break;
            case '9':
            case 'u':
                // Move up-right
                worldMap[curY][curX]->board[pcY][pcX] = firstLayer[curY][curX]->board[pcY][pcX] ; // Replace the old position with first layer
                if (worldMap[curY][curX]->board[pcY - 1][pcX + 1] != BORDER && worldMap[curY][curX]->board[pcY - 1][pcX + 1] != TREE && worldMap[curY][curX]->board[pcY - 1][pcX + 1] != WATER && worldMap[curY][curX]->board[pcY - 1][pcX + 1] != HIKER && worldMap[curY][curX]->board[pcY - 1][pcX + 1] != RIVAL && worldMap[curY][curX]->board[pcY - 1][pcX + 1] != EXPLORER && worldMap[curY][curX]->board[pcY - 1][pcX +  1] != WANDERER && worldMap[curY][curX]->board[pcY - 1][pcX + 1] != PACER && worldMap[curY][curX]->board[pcY - 1][pcX + 1] != SENTRY) {
                    pcX += 1;
                    pcY -= 1;
                } else if (worldMap[curY][curX]->board[pcY - 1][pcX + 1] == HIKER || worldMap[curY][curX]->board[pcY - 1][pcX + 1] == RIVAL || worldMap[curY][curX]->board[pcY - 1][pcX + 1] == EXPLORER || worldMap[curY][curX]->board[pcY - 1][pcX +  1] == WANDERER || worldMap[curY][curX]->board[pcY - 1][pcX + 1] == PACER || worldMap[curY][curX]->board[pcY - 1][pcX + 1] == SENTRY) {
                    for (int i = 0; i < numtrainers; i++) {
                        if (npcs[curY][curX][i]->x == pcX + 1 && npcs[curY][curX][i]->y == pcY - 1) {
                            printNPCTrainerDetails(npcs[curY][curX][i], curX, curY);
                            break;
                        }
                    }
                }
                worldMap[curY][curX]->board[pcY][pcX] = PC; // Place the PC at the new position
                break;
            case '6':
            case 'l':
                // Move right
                worldMap[curY][curX]->board[pcY][pcX] = firstLayer[curY][curX]->board[pcY][pcX] ; // Replace the old position with first layer
                if (worldMap[curY][curX]->board[pcY][pcX + 1] != BORDER && worldMap[curY][curX]->board[pcY][pcX + 1] != TREE && worldMap[curY][curX]->board[pcY][pcX + 1] != WATER && worldMap[curY][curX]->board[pcY][pcX + 1] != HIKER && worldMap[curY][curX]->board[pcY][pcX + 1] != RIVAL && worldMap[curY][curX]->board[pcY][pcX + 1] != EXPLORER && worldMap[curY][curX]->board[pcY][pcX +  1] != WANDERER && worldMap[curY][curX]->board[pcY][pcX + 1] != PACER && worldMap[curY][curX]->board[pcY][pcX + 1] != SENTRY) {
                    pcX += 1;
                } else if (worldMap[curY][curX]->board[pcY][pcX + 1] == HIKER || worldMap[curY][curX]->board[pcY][pcX + 1] == RIVAL || worldMap[curY][curX]->board[pcY][pcX + 1] == EXPLORER || worldMap[curY][curX]->board[pcY][pcX +  1] == WANDERER || worldMap[curY][curX]->board[pcY][pcX + 1] == PACER || worldMap[curY][curX]->board[pcY][pcX + 1] == SENTRY) {
                    for (int i = 0; i < numtrainers; i++) {
                        if (npcs[curY][curX][i]->x == pcX + 1 && npcs[curY][curX][i]->y == pcY) {
                            printNPCTrainerDetails(npcs[curY][curX][i], curX, curY);
                            break;
                        }
                    }
                }
                worldMap[curY][curX]->board[pcY][pcX] = PC; // Place the PC at the new position
                break;
            case '3':
            case 'n':
                // Move down-right
                worldMap[curY][curX]->board[pcY][pcX] = firstLayer[curY][curX]->board[pcY][pcX] ; // Replace the old position with grass
                if (worldMap[curY][curX]->board[pcY + 1][pcX + 1] != BORDER && worldMap[curY][curX]->board[pcY + 1][pcX + 1] != TREE && worldMap[curY][curX]->board[pcY + 1][pcX + 1] != WATER && worldMap[curY][curX]->board[pcY + 1][pcX + 1] != HIKER && worldMap[curY][curX]->board[pcY + 1][pcX + 1] != RIVAL && worldMap[curY][curX]->board[pcY + 1][pcX + 1] != EXPLORER && worldMap[curY][curX]->board[pcY + 1][pcX +  1] != WANDERER && worldMap[curY][curX]->board[pcY + 1][pcX + 1] != PACER && worldMap[curY][curX]->board[pcY + 1][pcX + 1] != SENTRY) {
                    pcX += 1;
                    pcY += 1;
                } else if (worldMap[curY][curX]->board[pcY + 1][pcX + 1] == HIKER || worldMap[curY][curX]->board[pcY + 1][pcX + 1] == RIVAL || worldMap[curY][curX]->board[pcY + 1][pcX + 1] == EXPLORER || worldMap[curY][curX]->board[pcY + 1][pcX +  1] == WANDERER || worldMap[curY][curX]->board[pcY + 1][pcX + 1] == PACER || worldMap[curY][curX]->board[pcY + 1][pcX + 1] == SENTRY) {
                    for (int i = 0; i < numtrainers; i++) {
                        if (npcs[curY][curX][i]->x == pcX + 1 && npcs[curY][curX][i]->y == pcY + 1) {
                            printNPCTrainerDetails(npcs[curY][curX][i], curX, curY);
                            break;
                        }
                    }
                }
                worldMap[curY][curX]->board[pcY][pcX] = PC; // Place the PC at the new position
                break;
            case '2':
            case 'j':
                // Move down
                worldMap[curY][curX]->board[pcY][pcX] = firstLayer[curY][curX]->board[pcY][pcX] ; // Replace the old position with grass
                if (worldMap[curY][curX]->board[pcY + 1][pcX] != BORDER && worldMap[curY][curX]->board[pcY + 1][pcX] != TREE && worldMap[curY][curX]->board[pcY + 1][pcX] != WATER && worldMap[curY][curX]->board[pcY + 1][pcX] != HIKER && worldMap[curY][curX]->board[pcY + 1][pcX] != RIVAL && worldMap[curY][curX]->board[pcY + 1][pcX] != EXPLORER && worldMap[curY][curX]->board[pcY + 1][pcX] != WANDERER && worldMap[curY][curX]->board[pcY + 1][pcX] != PACER && worldMap[curY][curX]->board[pcY + 1][pcX] != SENTRY) {
                    pcY += 1;
                } else if (worldMap[curY][curX]->board[pcY + 1][pcX] == HIKER || worldMap[curY][curX]->board[pcY + 1][pcX] == RIVAL || worldMap[curY][curX]->board[pcY + 1][pcX] == EXPLORER || worldMap[curY][curX]->board[pcY + 1][pcX] == WANDERER || worldMap[curY][curX]->board[pcY + 1][pcX] == PACER || worldMap[curY][curX]->board[pcY + 1][pcX] == SENTRY){
                    for (int i = 0; i < numtrainers; i++) {
                        if (npcs[curY][curX][i]->x == pcX && npcs[curY][curX][i]->y == pcY + 1) {
                            printNPCTrainerDetails(npcs[curY][curX][i], curX, curY);
                            break;
                        }
                    }
                }
                worldMap[curY][curX]->board[pcY][pcX] = PC; // Place the PC at the new position
                break;
            case '1':
            case 'b':
                // Move down-left
                worldMap[curY][curX]->board[pcY][pcX] = firstLayer[curY][curX]->board[pcY][pcX] ; // Replace the old position with grass
                if (worldMap[curY][curX]->board[pcY + 1][pcX - 1] != BORDER && worldMap[curY][curX]->board[pcY + 1][pcX - 1] != TREE && worldMap[curY][curX]->board[pcY + 1][pcX - 1] != WATER && worldMap[curY][curX]->board[pcY + 1][pcX - 1] != HIKER && worldMap[curY][curX]->board[pcY + 1][pcX - 1] != RIVAL && worldMap[curY][curX]->board[pcY + 1][pcX - 1] != EXPLORER && worldMap[curY][curX]->board[pcY + 1][pcX - 1] != WANDERER && worldMap[curY][curX]->board[pcY + 1][pcX - 1] != PACER && worldMap[curY][curX]->board[pcY + 1][pcX - 1] != SENTRY) {
                    pcX -= 1;
                    pcY += 1;
                } else if (worldMap[curY][curX]->board[pcY + 1][pcX - 1] == HIKER || worldMap[curY][curX]->board[pcY + 1][pcX - 1] == RIVAL || worldMap[curY][curX]->board[pcY + 1][pcX - 1] == EXPLORER || worldMap[curY][curX]->board[pcY + 1][pcX - 1] == WANDERER || worldMap[curY][curX]->board[pcY + 1][pcX - 1] == PACER || worldMap[curY][curX]->board[pcY + 1][pcX - 1] == SENTRY) {
                    for (int i = 0; i < numtrainers; i++) {
                        if (npcs[curY][curX][i]->x == pcX - 1 && npcs[curY][curX][i]->y == pcY + 1) {
                            printNPCTrainerDetails(npcs[curY][curX][i], curX, curY);
                            break;
                        }
                    }
                }
                worldMap[curY][curX]->board[pcY][pcX] = PC; // Place the PC at the new position
                break;
            case '4':
            case 'h':
                // Move left
                worldMap[curY][curX]->board[pcY][pcX] = firstLayer[curY][curX]->board[pcY][pcX] ; // Replace the old position with grass
                if (worldMap[curY][curX]->board[pcY][pcX - 1] != BORDER && worldMap[curY][curX]->board[pcY][pcX - 1] != TREE && worldMap[curY][curX]->board[pcY][pcX - 1] != WATER && worldMap[curY][curX]->board[pcY][pcX - 1] != HIKER && worldMap[curY][curX]->board[pcY][pcX - 1] != RIVAL && worldMap[curY][curX]->board[pcY][pcX - 1] != EXPLORER && worldMap[curY][curX]->board[pcY][pcX - 1] != WANDERER && worldMap[curY][curX]->board[pcY][pcX - 1] != PACER && worldMap[curY][curX]->board[pcY][pcX - 1] != SENTRY) {
                    pcX -= 1;
                } else if (worldMap[curY][curX]->board[pcY][pcX - 1] == HIKER || worldMap[curY][curX]->board[pcY][pcX - 1] == RIVAL || worldMap[curY][curX]->board[pcY][pcX - 1] == EXPLORER || worldMap[curY][curX]->board[pcY][pcX - 1] == WANDERER || worldMap[curY][curX]->board[pcY][pcX - 1] == PACER || worldMap[curY][curX]->board[pcY][pcX - 1] == SENTRY) {
                    for (int i = 0; i < numtrainers; i++) {
                        if (npcs[curY][curX][i]->x == pcX - 1 && npcs[curY][curX][i]->y == pcY) {
                            printNPCTrainerDetails(npcs[curY][curX][i], curX, curY);
                            break;
                        }
                    }
                }
                worldMap[curY][curX]->board[pcY][pcX] = PC; // Place the PC at the new position
                break;
            case '>':
                // Enter building
                if (firstLayer[curY][curX]->board[pcY][pcX] == POKEMON_CENTER) {
                    mvprintw(0, 0, "All your Pokemon have been healed. Press < to leave.");
                    for (auto& pokemon : pcPokemons) {
                        pokemon.hp = pokemon.max_hp;
                        pokemon.is_knocked_out = false;
                    }
                    refresh(); // Refresh the screen to show the message
                    do {
                        ch = getch();
                    } while (ch != '<');
                } else if (firstLayer[curY][curX]->board[pcY][pcX] == POKEMART) {
                    mvprintw(0, 0, "Your items have been restocked. Press < to leave.");
                    Pokeballs = 3; // Number of Pokeballs available
                    Potions = 5;   // Number of Potions available
                    Revives = 2;   // Number of Revives available
                    ToxicOrb = 3;
                    LightBall = 3;
                    FlameOrb = 3;
                    FreezeBomb = 3;
                    SleepPowder = 3;
                    ConfusionBomb = 3;
                    refresh(); // Refresh the screen to show the message
                    do {
                        ch = getch();
                    } while (ch != '<');
                }
                break;
            case ' ':
            case '.':
                // Rest for a turn
                break;
            case 'B':
                int itemChoice;
                clear();
                mvprintw(0, 0, "Accessing the bag:");
                mvprintw(1, 0, "1. Check Pokeballs, you have %d left!", Pokeballs);
                mvprintw(2, 0, "2. Use a Potion, you have %d left!", Potions);
                mvprintw(3, 0, "3. Use a Revive, you have %d left!", Revives);
                mvprintw(4, 0, "Choose an item to use:");
                refresh();
                itemChoice = getch() - '0'; // Adjust for ASCII offset
                
                switch (itemChoice) {
                case 1:  // Use Pokeball
                    if (Pokeballs > 0 && pcPokemons.size() < 6 && currentWildPokemon != nullptr) {
                        pcPokemons.push_back(*currentWildPokemon);  // Push the encountered Pokémon to the player's collection
                        Pokeballs--;  // Decrement the number of Pokeballs
                        mvprintw(curLine++, 0, "Pokemon caught successfully!");
                    } else if (currentWildPokemon == nullptr) {
                        mvprintw(curLine++, 0, "No wild Pokemon to catch!");
                    } else {
                        mvprintw(curLine++, 0, "No Pokeballs left or party full!");
                    }
                    break;
                case 2:  // Use Potion
                    if (Potions > 0) {
                        mvprintw(curLine++, 0, "Choose a Pokemon to heal:");
                        int healChoice = getch() - '0' - 1;
                        if (healChoice >= 0 && static_cast<size_t>(healChoice) < pcPokemons.size()) {
                            pcPokemons[healChoice].hp += 20;  // Heal 20 HP
                            Potions--;
                            mvprintw(curLine++, 0, "Healed %s by 20 HP.", pcPokemons[healChoice].pokemon.identifier.c_str());
                        } else {
                            mvprintw(curLine++, 0, "Invalid selection. No Pokemon healed.");
                        }
                    } else {
                        mvprintw(curLine++, 0, "No potions left!");
                    }
                    break;
                case 3:  // Use Revive
                    if (Revives > 0) {
                        mvprintw(curLine++, 0, "Choose a Pokemon to revive:");
                        int reviveChoice = getch() - '0' - 1;
                        if (reviveChoice >= 0 && static_cast<size_t>(reviveChoice) < pcPokemons.size() && pcPokemons[reviveChoice].is_knocked_out) {
                            pcPokemons[reviveChoice].hp = pcPokemons[reviveChoice].max_hp / 2;
                            pcPokemons[reviveChoice].is_knocked_out = false;
                            Revives--;
                            mvprintw(curLine++, 0, "Revived %s.", pcPokemons[reviveChoice].pokemon.identifier.c_str());
                        } else {
                            mvprintw(curLine++, 0, "Invalid selection. No Pokemon revived.");
                        }
                    } else {
                        mvprintw(curLine++, 0, "No revives left!");
                    }
                    break;
                    default:
                        mvprintw(5, 0, "Invalid choice.");
                    break;
                }
                getch(); // Wait for the player to read the message
                clear(); // Clear the screen after the action
                break;
            case 't':
                // Display list of trainers
                list_offset = 0;
                show_list = true;
                while (show_list) {
                    clear();
                    for (int i = list_offset; i < numtrainers; i++) {
                        // Calculate the relative position
                        int relative_x = npcs[curY][curX][i]->x - pcX;
                        int relative_y = npcs[curY][curX][i]->y - pcY;
                        // Determine the direction (north, south, east, west)
                        const char *ns = (relative_y > 0) ? "south" : "north";
                        const char *ew = (relative_x > 0) ? "east" : "west";
                        // Display the trainer info
                        mvprintw(i - list_offset, 0, "%c, %d %s and %d %s",
                        npcs[curY][curX][i]->type, abs(relative_y), ns, abs(relative_x), ew);
                    }
                    refresh();

                    ch = getch();
                    switch (ch) {
                        case KEY_UP:
                        case 450:
                            // Scroll list up
                            if (list_offset > 0) {
                                list_offset--;
                            }
                            break;
                        case KEY_DOWN:
                        case 456:
                            // Scroll list down
                            if (list_offset < numtrainers) {
                                list_offset++;
                            }
                            break;
                        case 27: // Escape key
                            show_list = false;
                            break;
                    }
                }
                break;
            case 'f':
                int flyX, flyY;
                echo(); // Enable echoing of input
                mvprintw(0, 0, "Where would you like to fly to? Enter X and Y coordinates (-200 to 200): ");
                scanw("%d %d", &flyX, &flyY);
                noecho(); // Turn off echoing of input

                // Translate relative coordinates to map indices
                flyX += MAP_CENTER;
                flyY += MAP_CENTER;

                // Validate coordinates
                if (flyX >= 0 && flyX < MAP_SIZE && flyY >= 0 && flyY < MAP_SIZE) {
                    // Clear previous messages
                    move(0, 0);
                    clrtoeol();

                    worldMap[curY][curX]->board[pcY][pcX] = firstLayer[curY][curX]->board[pcY][pcX];

                    // Update PC's current position to be within map bounds
                    preX = curX;
                    preY = curY;
                    curX = flyX;
                    curY = flyY;

                    // Place PC at a valid location on the map
                    // Update NPCs and turn queues for the new map
                    generateMap(worldMap, curX, curY, preX, preY, numtrainers, 1);
                    if (npcs[curY][curX] == NULL) {
                        npcs[curY][curX] = new NPC*[numtrainers];
                        for (int i = 0; i < numtrainers; i++) {
                            npcs[curY][curX][i] = new NPC;
                        }
                        int npcIndex = 0;
                        for (int i = 0; i < HEIGHT; i++) {
                            for (int j = 0; j < WIDTH; j++) {
                                if (worldMap[curY][curX]->board[i][j] == HIKER || worldMap[curY][curX]->board[i][j] == RIVAL || worldMap[curY][curX]->board[i][j] == PACER || worldMap[curY][curX]->board[i][j] == WANDERER || worldMap[curY][curX]->board[i][j] == SENTRY || worldMap[curY][curX]->board[i][j] == EXPLORER) {
                                    npcs[curY][curX][npcIndex]->x = j;
                                    npcs[curY][curX][npcIndex]->y = i;
                                    npcs[curY][curX][npcIndex]->cost = 0;
                                    npcs[curY][curX][npcIndex]->type = worldMap[curY][curX]->board[i][j];
                                    npcs[curY][curX][npcIndex]->is_defeated = false;
                                    int habitatId = -1;  // Default value indicating no specific habitat is set
                                    if (firstLayer[curY][curX]->board[i][j] == GRASS || firstLayer[curY][curX]->board[i][j] == TALL_GRASS) {
                                        habitatId = 3;  // Grassland
                                    } else if (firstLayer[curY][curX]->board[i][j] == TREE) {
                                        habitatId = 2;  // Forest
                                    }
                                    // Generate Pokémon for this NPC
                                    int numPokemons = 1 + (rand() % 6); // Each NPC has 1 to 6 Pokémon
                                    for (int k = 0; k < numPokemons; k++) {
                                        // Generate a random Pokémon with moves
                                        PokemonWithMoves pokemon = generateRandomPokemonWithMoves(curX, curY, habitatId);
                                        npcs[curY][curX][npcIndex]->pokemons.push_back(pokemon);
                                    }
                                    npcIndex++;
                                }
                            }
                        }
                    }
                }
                break;
            case 'Q':
            case 'q':
                handleGameOver(pcPokemons);
                break;
            default:
                // Any undefined key is ignored (no operation)
                break;
        }
        
        // Refresh the screen to show any updates
        clear();
        displayMap(worldMap, curX, curY);
        refresh();

        if (firstLayer[curY][curX]->board[pcY][pcX] == POKEMON_CENTER) {
            mvprintw(0, 0, "You are on a Pokemon Center. Press > to enter.");
            refresh(); // Refresh the screen to show the message
        } else if (firstLayer[curY][curX]->board[pcY][pcX] == POKEMART) {
            mvprintw(0, 0, "You are on a PokeMart. Press > to enter.");
            refresh(); // Refresh the screen to show the message
        }

        if (pcX == 0) {
            // Update the old position of the PC on the previous map
            worldMap[curY][curX]->board[pcY][pcX] = firstLayer[curY][curX]->board[pcY][pcX];
            // West gate, move to the map on the left
            preY = curY;
            preX = curX;
            curX--;
            pcX = WIDTH - 1; // Place the PC near the corresponding gate on the new map
            generateMap(worldMap, curX, curY, preX, preY, numtrainers, 0);
            if (npcs[curY][curX] == NULL) {
                npcs[curY][curX] = new NPC*[numtrainers];
                for (int i = 0; i < numtrainers; i++) {
                    npcs[curY][curX][i] = new NPC;
                }
                
                int npcIndex = 0;
                for (int i = 0; i < HEIGHT; i++) {
                    for (int j = 0; j < WIDTH; j++) {
                        if (worldMap[curY][curX]->board[i][j] == HIKER || worldMap[curY][curX]->board[i][j] == RIVAL || worldMap[curY][curX]->board[i][j] == PACER || worldMap[curY][curX]->board[i][j] == WANDERER || worldMap[curY][curX]->board[i][j] == SENTRY || worldMap[curY][curX]->board[i][j] == EXPLORER) {
                            npcs[curY][curX][npcIndex]->x = j;
                            npcs[curY][curX][npcIndex]->y = i;
                            npcs[curY][curX][npcIndex]->cost = 0;
                            npcs[curY][curX][npcIndex]->type = worldMap[curY][curX]->board[i][j];
                            npcs[curY][curX][npcIndex]->is_defeated = false;
                            int habitatId = -1;  // Default value indicating no specific habitat is set
                            if (firstLayer[curY][curX]->board[i][j] == GRASS || firstLayer[curY][curX]->board[i][j] == TALL_GRASS) {
                                habitatId = 3;  // Grassland
                            } else if (firstLayer[curY][curX]->board[i][j] == TREE) {
                                habitatId = 2;  // Forest
                            }
                            // Generate Pokémon for this NPC
                            int numPokemons = 1 + (rand() % 6); // Each NPC has 1 to 6 Pokémon
                            for (int k = 0; k < numPokemons; k++) {
                                // Generate a random Pokémon with moves
                                PokemonWithMoves pokemon = generateRandomPokemonWithMoves(curX, curY, habitatId);
                                npcs[curY][curX][npcIndex]->pokemons.push_back(pokemon);
                            }
                            npcIndex++;
                        }
                    }
                }
            }
            // Place the PC in the new position on the new map
            worldMap[curY][curX]->board[pcY][pcX] = PC;
        } else if (pcX == WIDTH - 1) {
            // Update the old position of the PC on the previous map
            worldMap[curY][curX]->board[pcY][pcX] = firstLayer[curY][curX]->board[pcY][pcX];
            // East gate, move to the map on the right
            preY = curY;
            preX = curX;
            curX++;
            pcX = 0; // Place the PC near the corresponding gate on the new map
            generateMap(worldMap, curX, curY, preX, preY, numtrainers, 0);
            if (npcs[curY][curX] == NULL) {
                npcs[curY][curX] = new NPC*[numtrainers];
                for (int i = 0; i < numtrainers; i++) {
                    npcs[curY][curX][i] = new NPC;
                }
                int npcIndex = 0;
                for (int i = 0; i < HEIGHT; i++) {
                    for (int j = 0; j < WIDTH; j++) {
                        if (worldMap[curY][curX]->board[i][j] == HIKER || worldMap[curY][curX]->board[i][j] == RIVAL || worldMap[curY][curX]->board[i][j] == PACER || worldMap[curY][curX]->board[i][j] == WANDERER || worldMap[curY][curX]->board[i][j] == SENTRY || worldMap[curY][curX]->board[i][j] == EXPLORER) {
                            npcs[curY][curX][npcIndex]->x = j;
                            npcs[curY][curX][npcIndex]->y = i;
                            npcs[curY][curX][npcIndex]->cost = 0;
                            npcs[curY][curX][npcIndex]->type = worldMap[curY][curX]->board[i][j];
                            npcs[curY][curX][npcIndex]->is_defeated = false;
                            int habitatId = -1;  // Default value indicating no specific habitat is set
                            if (firstLayer[curY][curX]->board[i][j] == GRASS || firstLayer[curY][curX]->board[i][j] == TALL_GRASS) {
                                habitatId = 3;  // Grassland
                            } else if (firstLayer[curY][curX]->board[i][j] == TREE) {
                                habitatId = 2;  // Forest
                            }
                            // Generate Pokémon for this NPC
                            int numPokemons = 1 + (rand() % 6); // Each NPC has 1 to 6 Pokémon
                            for (int k = 0; k < numPokemons; k++) {
                                // Generate a random Pokémon with moves
                                PokemonWithMoves pokemon = generateRandomPokemonWithMoves(curX, curY, habitatId);
                                npcs[curY][curX][npcIndex]->pokemons.push_back(pokemon);
                            }
                            npcIndex++;
                        }
                    }
                }
            }
            // Place the PC in the new position on the new map
            worldMap[curY][curX]->board[pcY][pcX] = PC;
        } else if (pcY == 0) {
            // Update the old position of the PC on the previous map
            worldMap[curY][curX]->board[pcY][pcX] = firstLayer[curY][curX]->board[pcY][pcX];
            // North gate, move to the map above
            preY = curY;
            preX = curX;
            curY--;
            pcY = HEIGHT - 1; // Place the PC near the corresponding gate on the new map
            generateMap(worldMap, curX, curY, preX, preY, numtrainers, 0);
            if (npcs[curY][curX] == NULL) {
                npcs[curY][curX] = new NPC*[numtrainers];
                for (int i = 0; i < numtrainers; i++) {
                    npcs[curY][curX][i] = new NPC;
                }
                int npcIndex = 0;
                for (int i = 0; i < HEIGHT; i++) {
                    for (int j = 0; j < WIDTH; j++) {
                        if (worldMap[curY][curX]->board[i][j] == HIKER || worldMap[curY][curX]->board[i][j] == RIVAL || worldMap[curY][curX]->board[i][j] == PACER || worldMap[curY][curX]->board[i][j] == WANDERER || worldMap[curY][curX]->board[i][j] == SENTRY || worldMap[curY][curX]->board[i][j] == EXPLORER) {
                            npcs[curY][curX][npcIndex]->x = j;
                            npcs[curY][curX][npcIndex]->y = i;
                            npcs[curY][curX][npcIndex]->cost = 0;
                            npcs[curY][curX][npcIndex]->type = worldMap[curY][curX]->board[i][j];
                            npcs[curY][curX][npcIndex]->is_defeated = false;
                            int habitatId = -1;  // Default value indicating no specific habitat is set
                            if (firstLayer[curY][curX]->board[i][j] == GRASS || firstLayer[curY][curX]->board[i][j] == TALL_GRASS) {
                                habitatId = 3;  // Grassland
                            } else if (firstLayer[curY][curX]->board[i][j] == TREE) {
                                habitatId = 2;  // Forest
                            }
                            // Generate Pokémon for this NPC
                            int numPokemons = 1 + (rand() % 6); // Each NPC has 1 to 6 Pokémon
                            for (int k = 0; k < numPokemons; k++) {
                                // Generate a random Pokémon with moves
                                PokemonWithMoves pokemon = generateRandomPokemonWithMoves(curX, curY, habitatId);
                                npcs[curY][curX][npcIndex]->pokemons.push_back(pokemon);
                            }
                            npcIndex++;
                        }
                    }
                }
            }
            // Place the PC in the new position on the new map
            worldMap[curY][curX]->board[pcY][pcX] = PC;
        } else if (pcY == HEIGHT - 1) {
            // Update the old position of the PC on the previous map
            worldMap[curY][curX]->board[pcY][pcX] = firstLayer[curY][curX]->board[pcY][pcX];
            // South gate, move to the map below
            preY = curY;
            preX = curX;
            curY++;
            pcY = 0; // Place the PC near the corresponding gate on the new map
            generateMap(worldMap, curX, curY, preX, preY, numtrainers, 0);
            if (npcs[curY][curX] == NULL) {
                npcs[curY][curX] = new NPC*[numtrainers];
                for (int i = 0; i < numtrainers; i++) {
                    npcs[curY][curX][i] = new NPC;
                }
                int npcIndex = 0;
                for (int i = 0; i < HEIGHT; i++) {
                    for (int j = 0; j < WIDTH; j++) {
                        if (worldMap[curY][curX]->board[i][j] == HIKER || worldMap[curY][curX]->board[i][j] == RIVAL || worldMap[curY][curX]->board[i][j] == PACER || worldMap[curY][curX]->board[i][j] == WANDERER || worldMap[curY][curX]->board[i][j] == SENTRY || worldMap[curY][curX]->board[i][j] == EXPLORER) {
                            npcs[curY][curX][npcIndex]->x = j;
                            npcs[curY][curX][npcIndex]->y = i;
                            npcs[curY][curX][npcIndex]->cost = 0;
                            npcs[curY][curX][npcIndex]->type = worldMap[curY][curX]->board[i][j];
                            npcs[curY][curX][npcIndex]->is_defeated = false;
                            int habitatId = -1;  // Default value indicating no specific habitat is set
                            if (firstLayer[curY][curX]->board[i][j] == GRASS || firstLayer[curY][curX]->board[i][j] == TALL_GRASS) {
                                habitatId = 3;  // Grassland
                            } else if (firstLayer[curY][curX]->board[i][j] == TREE) {
                                habitatId = 2;  // Forest
                            }
                            // Generate Pokémon for this NPC
                            int numPokemons = 1 + (rand() % 6); // Each NPC has 1 to 6 Pokémon
                            for (int k = 0; k < numPokemons; k++) {
                                // Generate a random Pokémon with moves
                                PokemonWithMoves pokemon = generateRandomPokemonWithMoves(curX, curY, habitatId);
                                npcs[curY][curX][npcIndex]->pokemons.push_back(pokemon);
                            }
                            npcIndex++;
                        }
                    }
                }
            }
            // Place the PC in the new position on the new map
            worldMap[curY][curX]->board[pcY][pcX] = PC;
        }

        if (firstLayer[curY][curX]->board[pcY][pcX] == TALL_GRASS) {
            clear();

            // Determine if an encounter occurs (10% chance)
            if (rand() % 10 == 0) {
                if (currentWildPokemon != nullptr) {
                    delete currentWildPokemon;
                    currentWildPokemon = nullptr;
                }

                // Create a new instance of PokemonWithMoves
                currentWildPokemon = new PokemonWithMoves();

                // Calculate the Manhattan distance from the center of the map
                double man_distance = abs(curX - MAP_CENTER) + abs(curY - MAP_CENTER);

                // Generate IVs for each stat
                int IV_hp = rand() % 16;
                int IV_attack = rand() % 16;
                int IV_defense = rand() % 16;
                int IV_special_attack = rand() % 16;
                int IV_special_defense = rand() % 16;
                int IV_speed = rand() % 16;

                // Determine the level of the encountered Pokémon
                int level;
                if (man_distance <= 1) {
                    level = 1;
                } else if (man_distance > 200) {
                    level = (rand() % (int)((100 - ((man_distance - 200) / 2) + 1)) + ((man_distance - 200) / 2));
                } else {
                    level = (rand() % (int)(man_distance / 2)) + 1;
                }

                // Select a random Pokémon species
                int random_species_index = rand() % allPokemonSpecies.size();
                int species_id = allPokemonSpecies[random_species_index].id;

                // Find the base stats for the selected Pokémon species
                int base_hp, base_attack, base_defense, base_special_attack, base_special_defense, base_speed;
                for (const auto& stat : allPokemonStats) {
                    if (stat.pokemon_id == species_id) {
                        switch (stat.stat_id) {
                            case 1: base_hp = stat.base_stat; break;
                            case 2: base_attack = stat.base_stat; break;
                            case 3: base_defense = stat.base_stat; break;
                            case 4: base_special_attack = stat.base_stat; break;
                            case 5: base_special_defense = stat.base_stat; break;
                            case 6: base_speed = stat.base_stat; break;
                        }
                    }
                }

                // Calculate the actual stats
                int hp = ((base_hp + IV_hp) * 2 * level) / 100 + level + 10;
                int attack = ((base_attack + IV_attack) * 2 * level) / 100 + 5;
                int defense = ((base_defense + IV_defense) * 2 * level) / 100 + 5;
                int special_attack = ((base_special_attack + IV_special_attack) * 2 * level) / 100 + 5;
                int special_defense = ((base_special_defense + IV_special_defense) * 2 * level) / 100 + 5;
                int speed = ((base_speed + IV_speed) * 2 * level) / 100 + 5;

                // Determine the moves for the Pokémon based on its level and species
                std::vector<int> move_ids;
                for (const auto& pokemon_move : allPokemonMoves) {
                    if (pokemon_move.pokemon_id == species_id && pokemon_move.level <= level) {
                        move_ids.push_back(pokemon_move.move_id);
                    }
                }

                // Ensure the Pokémon has at least one move
                if (move_ids.empty()) {
                    // Increase the level until the Pokémon has at least one move
                    while (move_ids.empty() && level < 100) {
                        level++;
                        for (const auto& pokemon_move : allPokemonMoves) {
                            if (pokemon_move.pokemon_id == species_id && pokemon_move.level <= level) {
                                move_ids.push_back(pokemon_move.move_id);
                            }
                        }
                    }
                }

                // Select up to four moves randomly
                std::vector<std::string> move_names;
                for (int i = 0; i < 2 && i < static_cast<int>(move_ids.size()); ++i) {
                    int move_id = move_ids[rand() % move_ids.size()];
                    for (const auto& move : allMoves) {
                        if (move.id == move_id) {
                            move_names.push_back(move.identifier);
                            break;
                        }
                    }
                }

                // Determine if the Pokémon is shiny
                bool shiny = (rand() % 8192 == 0);

                // Determine the gender of the Pokémon
                bool gender = (rand() % 2 == 0);

                // Display the encounter information
                mvprintw(0, 0, "A wild %s appeared!", allPokemonSpecies[random_species_index].identifier.c_str());
                mvprintw(1, 0, "Level: %d", level);
                mvprintw(2, 0, "HP: %d", hp);
                mvprintw(3, 0, "Attack: %d", attack);
                mvprintw(4, 0, "Defense: %d", defense);
                mvprintw(5, 0, "Special Attack: %d", special_attack);
                mvprintw(6, 0, "Special Defense: %d", special_defense);
                mvprintw(7, 0, "Speed: %d", speed);
                mvprintw(8, 0, "Shiny: %s", shiny ? "Yes" : "No");
                mvprintw(9, 0, "Gender: %s", gender ? "Female" : "Male");
                for (size_t i = 0; i < move_names.size(); ++i) {
                    mvprintw(10 + i, 0, "Move %d: %s", static_cast<int>(i + 1), move_names[i].c_str());
                }

                refresh();
                // Set up the new Pokémon's properties
                currentWildPokemon->pokemon.identifier = allPokemonSpecies[random_species_index].identifier;
                currentWildPokemon->level = level;
                currentWildPokemon->hp = hp;
                currentWildPokemon->max_hp = hp;  // Assuming max HP is set to the current HP initially
                currentWildPokemon->is_knocked_out = (hp <= 0);
                currentWildPokemon->attack = attack;
                currentWildPokemon->defense = defense;
                currentWildPokemon->speed = speed;
                currentWildPokemon->is_asleep = 0;
                currentWildPokemon->is_burned = 0;
                currentWildPokemon->is_confused = 0;
                currentWildPokemon->is_frozen = 0;
                currentWildPokemon->is_paralyzed = 0;
                currentWildPokemon->is_poisoned = 0;
                currentWildPokemon->current_exp = 0;
                currentWildPokemon->next_level_exp = experienceNeeded(currentWildPokemon->level, currentWildPokemon->pokemon.species_id);

                // Assigning moves to the currentWildPokemon
                currentWildPokemon->moves.clear();
                for (const auto& move_name : move_names) {
                    currentWildPokemon->moves.push_back(move_name);
                }
                getch();
                wildPokemonEncounter(pcPokemons, curPokeIndex, currentWildPokemon, attemps);
                if (currentWildPokemon != nullptr) {
                    delete currentWildPokemon;
                    currentWildPokemon = nullptr;
                }
            }
        }

        int totalExperience = 0;
        int maxLevelCount = 0;
        for (const auto& pokemon : pcPokemons) {
            totalExperience += pokemon.current_exp;
            if (pokemon.level >= 100) {
                maxLevelCount++;
            }
        }

        bool allKnockedOut = true;
        for (const auto& pokemon : pcPokemons) {
            if (!pokemon.is_knocked_out) {
                allKnockedOut = false;
                break;
            }
        }
        
        if (allKnockedOut && Revives == 0) {
            clear();
            curLine = 0;
            mvprintw(curLine++, 0, "All your Pokemons are knocked out! Game Over.");
            mvprintw(curLine++, 0, "Your score: %d", totalExperience);
            refresh();
            getch();
            endwin(); // Close the ncurses window, adjust as needed if using a different system
            exit(0);  // Exit the program
        } else if (allKnockedOut) {
            mvprintw(0, 0, "All your Pokemon are knocked out! Try to Revive Them.");
        }
    }
    
    // Free allocated memory for each map at the end of the program.
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            if (worldMap[i][j] != NULL && npcs[i][j] != NULL) { // Check if the NPC array was allocated
                for (int k = 0; k < numtrainers; k++) {
                    delete npcs[i][j][k];
                }
                delete[] npcs[i][j];
            }
            delete worldMap[i][j];
            delete firstLayer[i][j];
        }
    }

    // Clean up ncurses and exit
    mvprintw(0, 0, "Leaving the Game...");
    refresh(); // Refresh the screen to show the message
    endwin();
    return 0;
}