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
    for(int i=0; i < SIZE; i++)
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

    for(int i = 0; i < SIZE; i++)
    {
        for(int j = 0; j < SIZE; j++)
        {
            // References to all possible variables that hold a value a square could be
            get_row_neighbors(&board[i][0], rows[i][j]);
            get_col_neighbors(&board[0][i], cols[i][j]);
            get_block_neighbors(&board[(i / BLOCK_SIZE) * BLOCK_SIZE][(i * BLOCK_SIZE) % SIZE], blocks[i][j]);
        }
    }

    // Apply the inital variable constraints
    for(Variable *var: initial_inferences)
    {
        // Make sure the row/cols/blocks match up
        //std::set<Variable*> val = {var};
        for(int i = 0; i < SIZE; i++)
        {
            blocks[var->block][i].erase(var);
            rows[var->row][i].erase(var);
            cols[var->col][i].erase(var);
        }
        blocks[var->block][var->assigned - 1] = {var};
        rows[var->row][var->assigned - 1] = {var};
        cols[var->col][var->assigned - 1] = {var};

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
        int test_value = current_var->assigned;

        std::set<Variable *> neighbors = get_neighbors(current_var);
        for(Variable *neighbor : neighbors)
        {
            // Don't modify existing choices, or yourself
            if(neighbor == current_var || neighbor->assigned)
            {
                continue;
            }

            // If we can limit the domain, add to inferences
            if(neighbor->limit_domain(test_value))
            {
                inferences[neighbor].insert(test_value);
                blocks[neighbor->block][test_value - 1].erase(neighbor);
                rows[neighbor->row][test_value - 1].erase(neighbor);
                cols[neighbor->col][test_value - 1].erase(neighbor);
            }

            // If there are no legal values left for a variable, we are done with this guess.
            if(neighbor->domain.size() == 0)
            {
                return {false, inferences};
            }

            // If this guess eliminated the last place for this number in another block, row, or col, we are similarly done
            if(blocks[neighbor->block][test_value - 1].size() == 0
                || rows[neighbor->row][test_value - 1].size() == 0
                || cols[neighbor->col][test_value - 1].size() == 0
            ) {
                return {false, inferences};
            }

            // If there is exactly one value left, then propagate constraints!
            if(neighbor->domain.size() == 1 && neighbor->assigned == 0)
            {
                unassigned.erase(std::remove(unassigned.begin(), unassigned.end(), neighbor));
                testing_variables.push_back(neighbor);
                neighbor->assigned = *(neighbor->domain.begin());
            }

            // Check to see if we can simplify based on the counts of the test variable for a neighbors row/block/column
            count_consistencies(blocks, neighbor->block, test_value, testing_variables, inferences);
            count_consistencies(rows, neighbor->row, test_value, testing_variables, inferences);
            count_consistencies(cols, neighbor->col, test_value, testing_variables, inferences);

            steps++;
        }
    }

    return {true, inferences};
}

void Board::count_consistencies(std::array<std::array<std::set<Variable*>, SIZE>, SIZE> &count_tracker,
                         int index, int value,
                         std::vector<Variable*>& testing_variables,
                         std::unordered_map<Variable*, std::set<int>>& inferences)
{
    Variable *test_var = *count_tracker[index][value - 1].begin();
    if(count_tracker[index][value - 1].size() == 1 && test_var->assigned == 0)
    {
        unassigned.erase(std::remove(unassigned.begin(), unassigned.end(), test_var));
        testing_variables.push_back(test_var);
        test_var->assigned = value;

        inferences[test_var].insert(test_var->domain.begin(), test_var->domain.end());
        inferences[test_var].erase(value);
        test_var->domain = {value};
    }
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
    get_row_neighbors(current_variable, neighbors);
    get_col_neighbors(current_variable, neighbors);
    get_block_neighbors(current_variable, neighbors);

    return neighbors;
}

void Board::get_row_neighbors(Variable *current_variable, std::set<Variable*> &neighbors)
{
    for(int i = 0; i < SIZE; i++)
    {
        neighbors.insert(&board[current_variable->row][i]);
    }
}
void Board::get_col_neighbors(Variable *current_variable, std::set<Variable*> &neighbors)
{
    for(int i = 0; i < SIZE; i++)
    {
        neighbors.insert(&board[i][current_variable->col]);
    }
}
void Board::get_block_neighbors(Variable *current_variable, std::set<Variable*> &neighbors)
{
    int starting_row = (current_variable->row / 3) * 3;
    int starting_col = (current_variable->col / 3) * 3;
    for(int i = 0; i < BLOCK_SIZE; i++)
    {
        for(int j = 0; j < BLOCK_SIZE; j++)
        {
            neighbors.insert(&board[starting_row + i][starting_col + j]);
        }
    }
}

// Given an unordered map of variables --> values those variables cannot take on,
// will restore these values to that variable's domain.
void Board::restore_inferences(std::unordered_map<Variable*, std::set<int>> &inferences)
{
    for (auto const& [var, domain] : inferences)
    {
        if(var->assigned)
        {
            var->assigned = 0;
            unassigned.push_back(var);
        }
        for( int val : domain)
        {
            blocks[var->block][val - 1].insert(var);
            rows[var->row][val - 1].insert(var);
            cols[var->col][val - 1].insert(var);
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