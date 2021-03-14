#include <algorithm>
#include <iterator>
#include <constants.h>
#include <variable.h>

Variable::Variable(int row, int col, int assigned)
{
    this->row = row;
    this->col = col;
    this->block = row / BLOCK_SIZE * BLOCK_SIZE + col / BLOCK_SIZE;

    this->assigned = assigned;
    if(assigned == 0) {
        domain = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    } else {
        domain = {assigned};
    }
}

bool Variable::operator==(const Variable &other) const
{
    return col == other.col && row == other.row;
}

// TODO --> I don't think we use this function anymore unless we attempt to order the variables.
// Look into removing it.
bool Variable::operator<(const Variable &other) const
{
    // Check for equality to make sure no duplicates are inserted?
    // TODO: check to see how this effects runtime.
    //       ALSO - it probably isn't necessary
    return domain.size() < other.domain.size();
}

// TODO --> maybe combine this with "print_domain" at some point.
std::ostream& operator<<(std::ostream &os, Variable &var)
{
    os << var.assigned << ", " << var.block << ":(" << var.row << "," << var.col << ")";
    return os;
}

// Convenience function for seeing showing the current domain of a variable
void Variable::print_domain()
{
    std::cout << "Current domain of " << *this << " is: {";
    for (auto it = domain.begin(); it != domain.end(); ++it)
    {
        std::cout << ' ' << *it;
    }
    std::cout << " }" << std::endl;
}

// Remove an int from the domain. Returns true if any elements were erased, false otherwise.
bool Variable::limit_domain(int limit)
{
    return domain.erase(limit) > 0;
}

void Variable::restore_domain(std::set<int> restore)
{
    // I read somewhere that if you use set_union with another set, it stops
    // unnecessary iteration (instead of always trying to insert from the beginning).
    std::set<int> temp;
    std::set_union(domain.begin(), domain.end(),
                        restore.begin(), restore.end(),
                        std::inserter(temp, temp.begin()));

    domain.swap(temp);
}