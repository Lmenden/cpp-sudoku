// TODO: Define pseudo-code for the algorithm so we can make it more generic!
#include <algorithm>
#include <unordered_map>
#include <board.h>
#include <variable.h>

bool backtracking_search(Board &board)
{
    // Assignment complete? Return assignment
    if (board.unassigned.size() == 0) return true;

    // The idea was to sort the variables by minimum remaining values (MRV) heuristic
    // However, sorting seems to add more steps in almost all of my cases (not even counting sorting time...)
    // In some cases, sorting my MOST remaining values seemed to reduce steps, but not by very much.
    //std::sort(board.unassigned.rbegin(), board.unassigned.rend());
    //std::sort(board.unassigned.begin(), board.unassigned.end());

    Variable *var = board.unassigned.back();
    board.unassigned.pop_back();

    // TODO: Make sure this line is necessary
    std::set<int> values = var->domain;

    // Attempts
    for(int value: values)
    {
        var->assigned = value;

        auto [success, new_inferences] = board.limit(var);
        if(success && backtracking_search(board))
        {
            return true;
        }

        // Otherwise, remove inferences and try again...
        board.restore_inferences(new_inferences);
    }

    // Nothing worked, clear var, add to unassigned, and return
    board.unassigned.push_back(var);
    var->assigned = 0;
    return false;
}