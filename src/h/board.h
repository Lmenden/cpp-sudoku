#pragma once

#include <set>
#include <array>
#include <vector>
#include <unordered_map>

#include <string>
#include <tuple>

#include <constants.h>
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
    std::array<std::array<std::set<Variable*>, SIZE>, SIZE> blocks, rows, cols;

    static std::array<std::array<int, SIZE>, SIZE> file_to_grid(std::string);
    Board(std::array<std::array<int, SIZE>, SIZE>);

    std::tuple<bool, std::unordered_map<Variable*, std::set<int>>> limit(Variable*);
    std::set<Variable*> get_neighbors(Variable*);
    void get_row_neighbors(Variable*, std::set<Variable*>&);
    void get_col_neighbors(Variable*, std::set<Variable*>&);
    void get_block_neighbors(Variable*, std::set<Variable*>&);
    bool consistency_check(Variable*);
    void restore_inferences(std::unordered_map<Variable*, std::set<int>> &inferences);

    void count_consistencies(std::array<std::array<std::set<Variable*>, SIZE>, SIZE>&, int, int, std::vector<Variable*>&, std::unordered_map<Variable*, std::set<int>>&);
    
    void print_board();
};
