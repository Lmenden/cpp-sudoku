## Sudoku Solver

First project in C++. Given a file representation of a sudoku grid, will solve the puzzle using backtracking search. My solver stores the domains of each square in the sudoku grid, and uses this information when checking possible solutions.

By keeping track of variable domains, the solver can quickly rule out many dead ends (when a variable's domain is reduced to 0), or avoid checking impossible values (values outside of a variable's domain).

This solution is more complex than similar solutions provided at:

     - https://www.tutorialspoint.com/sudoku-solver-in-cplusplus

     - https://www.geeksforgeeks.org/sudoku-backtracking-7/ (second solution)

So, I included a simple method for comparison. [See: Running](#running)

### Folder Layout

├── anomaly_puzzles             : Puzzles that take significantly longer on the simpler solution

├── build                       : Directory created upon compilation usuing Makefile. Executable created here

├── puzzles_unsolved            : Puzzle files are stored here

└── src                         : Include cpp and header file directories

    ├── cpp
    
    └── h

puzzle_fetch.py                 : Program to fetch puzzle files. Unfortunately, the site seems to be down.

### Running

Make sure you have a `puzzles_unsolved` directory. Run `puzzle_fetch.py` to populate.

`Make` should create the executable under `build/solver`

To solve a puzzle, run:
`./build/solver puzzle_<X>`
from the main directory

To compare my solver against the simpler solution, run:
`./build/solver puzzle_<X> comp`
from the main directory