#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream> 
#include <board.h>

// Converts a sudoku file output by the python script into a 2D array of ints
std::array<std::array<int, SIZE>, SIZE> Board::file_to_grid(std::string puzzle_file)
{
    std::array<std::array<int, SIZE>, SIZE> grid;

    std::string full_path = Board::puzzle_dir + puzzle_file;
    std::ifstream file_board(full_path);
    if(!file_board)
    {
        std::cerr << "Error opening file: \"" << full_path << "\". Please check that it exists" << std::endl;
        throw "File Not Found";
    }

    int row_counter = 0;
    int col_counter = 0;
    std::string row;
    while(std::getline(file_board, row))
    {
        std::stringstream ss(row);
        for(char c; ss >> c;)
        {
            if(c == '_')
            {
                grid[row_counter][col_counter] = 0;
            }
            else
            {
                // Convert '0-9' to 0-9, from string to int
                int value = (int) (c - 48);
                grid[row_counter][col_counter] = value;
            }

            col_counter++;
        }

        row_counter++;
        col_counter = 0;
    }

    return grid; 
}

// Given a 2D array of ints, formulates the constraint satisfaction problem (CSP)
Board::Board(std::array<std::array<int, SIZE>, SIZE> grid)
{
    // Store initial variable constraints
    std::vector<Variable*> initial_inferences;
    for(int i = 0; i < SIZE; i++)
    {
        for(int j = 0; j < SIZE; j++)
        {
            if(grid[i][j] == 0)
            {
                board[i][j] = Variable(i, j);
                unassigned.push_back(&board[i][j]);
            } else {
                board[i][j] = Variable(i, j, grid[i][j]);
                initial_inferences.push_back(&board[i][j]);
            }
        }
    }

    // Apply the inital variable constraints
    for(Variable *var: initial_inferences)
    {
        // TODO --> make this throw an exception if provided a conflicting board assignment
        limit(var);
    }
}


// Given a newly assigned variable, attemps to:
//
// (1) Check consistency of assignment
// (2) Propogate constraints
//
// If either (1) or (2) fails, return failure along with any inferences made.
// If a domain is reduced to 0, fail.
// If a domain is reduced to 1, add that variable to the list, and repeat steps 1-2 on that variable.
std::tuple<bool, std::unordered_map<Variable*, std::set<int>>> Board::limit(Variable *v)
{
    std::vector<Variable*> testing_variables;
    testing_variables.push_back(v);

    auto inferences = std::unordered_map<Variable*, std::set<int>>();
    while(testing_variables.size() > 0)
    {
        Variable *current_var = testing_variables.back();
        testing_variables.pop_back();
        if(!consistency_check(current_var))
        {
            return {false, inferences};
        }

        std::set<Variable *> neighbors = get_neighbors(current_var);
        for(Variable *neighbor : neighbors)
        {
            if(neighbor == current_var || neighbor->assigned)
            {
                continue;
            }

            // If we can limit the domain, add to inferences
            if(neighbor->limit_domain(current_var->assigned))
            {
                inferences[neighbor].insert(current_var->assigned);
            }

            // If there are no legal values left for a variable, we are done with this guess.
            if(neighbor->domain.size() == 0)
            {
                return {false, inferences};
            }

            // If there is exactly one value left, then propagate constraints!
            if(neighbor->domain.size() == 1 && neighbor->assigned == 0)
            {
                unassigned.erase(std::remove(unassigned.begin(), unassigned.end(), neighbor));
                testing_variables.push_back(neighbor);
                neighbor->assigned = *(neighbor->domain.begin());
            }
            steps++;
        }
    }

    return {true, inferences};
}

// Sudoku has an implicit ALLDIFF constraint between each square and every other
// square within its
// - ROW
// - COL
// - BLOCK (3x3)
//
// This means that a square should be different from all of it's "neighbors"
bool Board::consistency_check(Variable *choice)
{
    auto neighbors = get_neighbors(choice);
    for(auto neighbor : neighbors)
    {
        if(choice->assigned == neighbor->assigned && choice != neighbor)
        {
            return false;
        }
    }

    return true;
}

// Gets a set of all Variables that are in a variable's block, row, column
// NOTE: This includes itself
std::set<Variable *> Board::get_neighbors(Variable *current_variable)
{
    std::set<Variable*> neighbors;
    for(int i = 0; i < SIZE; i++)
    {
        // Calculations for iterating through a 3x3 sudoku block, left->right, top->bottom
        int block_row_base = (current_variable->row / BLOCK_SIZE) * BLOCK_SIZE;
        int block_row_step = i / BLOCK_SIZE;
        int block_col_base = (current_variable->col / BLOCK_SIZE) * BLOCK_SIZE;
        int block_col_step = i % BLOCK_SIZE;
        int adjusted_row = block_row_base + block_row_step;
        int adjusted_col = block_col_base + block_col_step;

        // Build up the list of all variables who share a row, column, or block with the given variable
        neighbors.insert(&board[current_variable->row][i]);
        neighbors.insert(&board[i][current_variable->col]);
        neighbors.insert(&board[adjusted_row][adjusted_col]);
    }

    return neighbors;
}

// Given an unordered map of variables --> values those variables cannot take on,
// will restore these values to that variable's domain.
void Board::restore_inferences(std::unordered_map<Variable*, std::set<int>> &inferences)
{
    // std::cout << "Restoring those inferneces\n";
    for (auto const& [var, domain] : inferences)
    {
        if(var->assigned)
        {
            var->assigned = 0;
            unassigned.push_back(var);
        }
        var->restore_domain(domain);
    }
}

// Stolen from: https://www.tutorialspoint.com/sudoku-solver-in-cplusplus
// I wanted to see how my solution compared to this much simpler solution.
// I like how they printed the board, so I stole it and modified it slightly
//
// NOTE: There are MANY magic variables, so this should get refactored at some point.
void Board::print_board(){
   for (int row = 0; row < SIZE; row++){
      for (int col = 0; col < SIZE; col++){
         if(col == 3 || col == 6)
            std::cout << " | ";
         std::cout << board[row][col].assigned << " ";
      }
      if(row == 2 || row == 5){
         std::cout << std::endl;
         for(int i = 0; i < 3; i++)
            std::cout << "- - - - ";
      }
      std::cout << std::endl;
   }
}