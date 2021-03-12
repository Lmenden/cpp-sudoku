// NOTE:
//  The code shown here is essentially taken from:
//      - https://www.tutorialspoint.com/sudoku-solver-in-cplusplus
//      - https://www.geeksforgeeks.org/sudoku-backtracking-7/ (second solution)
//
// This is to provide a good baseline for testing against my solution (time, space, steps taken, etc...)
#pragma once
#include <iostream>
#include <array>
#define N 9

class simple_solver
{
   private:
   public:
   std::array<std::array<int, N>, N> my_board;
   int steps = 0;

   simple_solver(std::array<std::array<int, N>, N> board) : my_board(board) {};

   bool isPresentInCol(int col, int num); //check whether num is present in col or not
   bool isPresentInRow(int row, int num); //check whether num is present in row or not
   bool isPresentInBox(int boxStartRow, int boxStartCol, int num); //check whether num is present in 3x3 box or not
   void sudokuGrid(); //print the sudoku grid after solve
   bool findEmptyPlace(int &row, int &col); //get empty location and update row and column
   bool isValidPlace(int row, int col, int num); //when item not found in col, row and current 3x3 box
   bool solveSudoku();
};