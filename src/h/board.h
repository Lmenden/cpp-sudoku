#pragma once
#define SIZE 9
#define BLOCK_SIZE 3

#include <set>
#include <array>
#include <vector>
#include <unordered_map>

#include <string>
#include <tuple>

#include <variable.h>

class Board
{
private:
    /* data */
public:
    // Puzzle files are found here
    inline static const std::string puzzle_dir = "puzzles_unsolved/";

    int steps = 0;
    std::vector<Variable *> unassigned;
    std::array<std::array<Variable, SIZE>, SIZE> board;

    static std::array<std::array<int, SIZE>, SIZE> file_to_grid(std::string);
    Board(std::array<std::array<int, SIZE>, SIZE>);

    std::tuple<bool, std::unordered_map<Variable*, std::set<int>>> limit(Variable*);
    std::set<Variable*> get_neighbors(Variable*);
    bool consistency_check(Variable*);
    void restore_inferences(std::unordered_map<Variable*, std::set<int>> &inferences);
    
    void print_board();
};
