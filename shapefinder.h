#ifndef SHAPEFINDER_H
#define SHAPEFINDER_H
#include "gamemodel.h"
#include "Coordinate.h"
#include <vector>
using namespace std;

/*
 * Function: have_five_at
 * Usage: is_win = have_five_at(board, color, xy);
 * --------------------------------------------------
 * This function checks if there is continuous five
 * or even more of the given volor on the board passing
 * the given center xy.
 */
bool have_five_at(const vector<vector<int>> &board, string color, Coordinate xy);
bool have_five_at(const vector<vector<int>> &board, string color, int x, int y);


/*
 * Function: have_five_at
 * Usage: has_active_four = have_five_at(board, color, xy);
 * --------------------------------------------------
 * This function checks if there is active four of the
 * given volor on the board passing the given center xy.
 * This function excludes the case when there is a continuous
 * five.
 */
bool have_active_four_at(const vector<vector<int>> &board, string color, Coordinate xy);
bool have_active_four_at(const vector<vector<int>> &board, string color, int x, int y);

#endif // SHAPEFINDER_H
