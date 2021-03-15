#include <chrono>

#include <iostream>
#include <string>
#include <board.h>
#include <simple_solver.h>

const std::string name = "solver.exe";
// using namespace std; BAD --> Pollutes global namespace
// using std::cout;     BETTER --> is clearer about what it's trying to do.

int main(int argc, char **argv) {
    if(argc != 2 && argc != 3)
    {
        std::cout << "USAGE:\n./" << name << " <name of sudoku file> [comp]" << std::endl;
        exit(1);
    }

    // Initialize the 2d array of ints representing a sudoku grid
    std::array<std::array<int, SIZE>, SIZE> grid = Board::file_to_grid(argv[1]);
    if(argc == 3)
    {
        // Create and time my program solving the grid
        auto my_start = std::chrono::high_resolution_clock::now();
        Board my_board = Board(grid);
        my_board.backtracking_search();
        auto my_end = std::chrono::high_resolution_clock::now();
        auto my_duration = std::chrono::duration_cast<std::chrono::microseconds>(my_end - my_start);

        // Create and time the simple program solving the grid
        auto their_start = std::chrono::high_resolution_clock::now();
        simple_solver their_board = simple_solver(grid);
        their_board.solveSudoku();
        auto their_end = std::chrono::high_resolution_clock::now();
        auto their_duration = std::chrono::duration_cast<std::chrono::microseconds>(their_end - their_start);

        std::cout << "My steps: " << my_board.steps << ", time: " << my_duration.count() << std::endl;
        std::cout << "Their steps: " << their_board.steps << ", time: " << their_duration.count() << std::endl;
    } else
    {
        Board my_board = Board(grid);
        std::cout << "Before:\n";
        my_board.print_board();

        bool result = my_board.backtracking_search();

        if(result)
        {
            std::cout << "\nAfter:\n";
            my_board.print_board();
            std::cout << "Steps: " << my_board.steps << std::endl;
        } else
        {
            std::cout << "Search FAILED" << std::endl;
        }
    }

    return 0;
}
