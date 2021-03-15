#pragma once
#include <set>
#include <iostream>

class Variable
{
private:
public:
    int row, col;
    int assigned;
    std::set<int> domain;

    // Do-nothing initializer required for board.h to have an arry of Variable Objects
    Variable(){};
    Variable(int row, int col, int assigned=0);
    
    bool operator==(const Variable &other) const;
    bool operator<(const Variable &other) const;
    friend std::ostream& operator<<(std::ostream& os, Variable& var);

    void print_domain();
    bool limit_domain(int);
    void restore_domain(std::set<int>);
};


// An example of specializing the hash template found in std
// Allows you to hash custom objects without passing a hash functor to the unordered_(set,map)
//
// UNUSED --> since I changed from storing Variable to Variable*
//
// namespace std
// {
//     template<>
//     struct hash<Variable>
//     {
//         size_t operator()(const Variable& var) const noexcept
//         {
//             // Cantor Pairing of two integers, these integers are in each in the range 0-9
//             // Still probably should make a better hash function, can test performance later.
//             return (1/2 * (var.row + var.col) * (var.row + var.col + 1) + var.col);
//         }
//     };
// }
