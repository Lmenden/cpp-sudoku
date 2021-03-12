// NOTE:
//  The code shown here is essentially taken from:
//      - https://www.tutorialspoint.com/sudoku-solver-in-cplusplus
//      - https://www.geeksforgeeks.org/sudoku-backtracking-7/ (second solution)
//
// This is to provide a good baseline for testing against my solution (time, space, steps taken, etc...)
#include <simple_solver.h>
#define N 9

bool simple_solver::isPresentInCol(int col, int num){ //check whether num is present in col or not
   for (int row = 0; row < N; row++)
      if (my_board[row][col] == num)
         return true;
   return false;
}
bool simple_solver::isPresentInRow(int row, int num){ //check whether num is present in row or not
   for (int col = 0; col < N; col++)
      if (my_board[row][col] == num)
         return true;
   return false;
}
bool simple_solver::isPresentInBox(int boxStartRow, int boxStartCol, int num){
//check whether num is present in 3x3 box or not
   for (int row = 0; row < 3; row++)
      for (int col = 0; col < 3; col++)
         if (my_board[row+boxStartRow][col+boxStartCol] == num)
            return true;
   return false;
}
void simple_solver::sudokuGrid(){ //print the sudoku grid after solve
   for (int row = 0; row < N; row++){
      for (int col = 0; col < N; col++){
         if(col == 3 || col == 6)
            std::cout << " | ";
         std::cout << my_board[row][col] <<" ";
      }
      if(row == 2 || row == 5){
         std::cout << std::endl;
         for(int i = 0; i<N; i++)
            std::cout << "---";
      }
      std::cout << std::endl;
   }
}
bool simple_solver::findEmptyPlace(int &row, int &col){ //get empty location and update row and column
   for (row = 0; row < N; row++)
      for (col = 0; col < N; col++)
         if (my_board[row][col] == 0) //marked with 0 is empty
            return true;
   return false;
}
bool simple_solver::isValidPlace(int row, int col, int num){
   //when item not found in col, row and current 3x3 box
   return !isPresentInRow(row, num) && !isPresentInCol(col, num) && !isPresentInBox(row - row%3 , col - col%3, num);
}
bool simple_solver::solveSudoku(){
   int row, col;
   if (!findEmptyPlace(row, col))
      return true; //when all places are filled
   for (int num = 1; num <= 9; num++){ //valid numbers are 1 - 9 
      steps++;
      if (isValidPlace(row, col, num)){ //check validation, if yes, put the number in the grid
         my_board[row][col] = num;
         if (solveSudoku()) //recursively go for other rooms in the grid
            return true;
         my_board[row][col] = 0; //turn to unassigned space when conditions are not satisfied
      }
   }
   return false;
}