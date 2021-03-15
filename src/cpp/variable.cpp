#include <variable.h>
#include <algorithm>
#include <iterator>

Variable::Variable(int row, int col, int assigned)
{
    this->row = row;
    this->col = col;

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

// Because domains can be equal in size, this will NOT allow you to insert into sets
//
// Only used if sorting by most/minimum remaining values
bool Variable::operator<(const Variable &other) const
{
    return domain.size() < other.domain.size();
}

std::ostream& operator<<(std::ostream &os, Variable &var)
{
    os << var.assigned << ", " << "(" << var.row << "," << var.col << ")";
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

// Remove an int from the domain. Returns true if any elements were erased, false otherwise
bool Variable::limit_domain(int limit)
{
    return domain.erase(limit) > 0;
}

// Given a set of integers, add these values back into the variable's domain
void Variable::restore_domain(std::set<int> restore)
{
    std::set<int> temp;
    std::set_union(domain.begin(), domain.end(),
                        restore.begin(), restore.end(),
                        std::inserter(temp, temp.begin()));

    domain.swap(temp);
}