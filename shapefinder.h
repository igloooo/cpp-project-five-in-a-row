#ifndef SHAPEFINDER_H
#define SHAPEFINDER_H
#include "gamemodel.h"
#include "coordinate.h"
#include "stone.h"
#include <vector>
#include <stack>
using namespace std;


/*
 * Function: have_five_at
 * Usage: if(have_five_at(board, color, xy))...
 *        if(have_five_at(board, color, x, y))...
 *        if(have_five_at(board, color, xy, "x"))...
 *        if(have_five_at(board, color, x, y, "d", st))...
 * --------------------------------------------------
 * These functions check if there is continuous five
 * or even more of the given volor on the board passing
 * the given center xy. The additional argument "direc"
 * specifies a certain direction, while "new_move" denotes
 * the a new move which hasn't been appended to the board.
 */
bool have_five_at(const vector<vector<int>> &board, string color, Coordinate xy);
bool have_five_at(const vector<vector<int>> &board, string color, int x, int y);
bool have_five_at(const vector<vector<int>> &board, string color, Coordinate xy,
                string direc, Stone new_move = Stone(-1,-1,"empty"));

/*
 * Function: have_active_four_at
 * Usage: if(have_active_four_at(board, color, xy))...
 *        if(have_active_four_at(board, color, x, y))...
 *        if(have_active_four_at(board, color, xy, "a"))...
 *        if(have_active_four_at(board, color, xy, "y", st))...
 * --------------------------------------------------
 * Similar to the above functions. These functions checks
 * for active four.
 */
bool have_active_four_at(const vector<vector<int>> &board, string color, Coordinate xy);
bool have_active_four_at(const vector<vector<int>> &board, string color, Coordinate xy,
                        string direc, Stone new_moves = Stone(-1,-1,"empty"));
bool have_active_four_at(const vector<vector<int>> &board, string color, int x, int y);

/*
 * Function: have_half_four_at
 * Usage: if(have_half_four_at(board, color, xy, "y"))...
 * --------------------------------------------------
 * Similar to the above functions. This function checks
 * for half four at xy along the specified direction.
 */
bool have_half_four_at(const vector<vector<int>> &board, string color, Coordinate xy, string direc);
bool have_half_four_at(const vector<vector<int>> &board, string color, Coordinate xy);

/*
 * Function: num_of_half_four_at
 * Usage: number = num_of_half_four_at(board, color, xy)
 *        number = num_of_half_four_at(board, color, x, y)
 * --------------------------------------------------
 * These functions checks for number of half four shapes
 * passing through xy or (x,y).
 */
int num_of_half_four_at(const vector<vector<int>> &board, string color, Coordinate xy);
int num_of_half_four_at(const vector<vector<int>> &board, string color, int x, int y);

/*
 * Function: have_active_three_at
 * Usage: if(have_active_three_at(board, color, xy, "y"))...
 * --------------------------------------------------
 * Similar to the above functions. This function checks
 * for active three at xy along the give direction.
 */
bool have_active_three_at(const vector<vector<int>> &board, string color, Coordinate xy, string direc);
bool have_active_three_at(const vector<vector<int>> &board, string color, Coordinate xy);

/*
 * Function: num_of_active_three_at
 * Usage: number = num_of_active_three_at(board, color, xy)
 *        number = num_of_active_three_at(board, color, x, y)
 * --------------------------------------------------
 * These functions checks for number of active three shapes
 * passing through xy or (x,y).
 */
int num_of_active_three_at(const vector<vector<int>> &board, string color, Coordinate xy);
int num_of_active_three_at(const vector<vector<int>> &board, string color, int x, int y);

/*
 * Function: get_influence_domain
 * Usage: get_influence_domain(inf_dom, board, xy)
 * ----------------------------------------------
 * This function finds all the positions that might
 * connect with xy. The results are filled into the
 * given stack.
 */
void get_influence_domain(stack<Coordinate> & inf_dom, const vector<vector<int>> &board,
                          Coordinate co);
void get_influence_domain(stack<Coordinate> &inf_dom, const vector<vector<int> > &board,
                           Coordinate co, string direc);
//the boards are supposed to be BOARDSIZE.x*BOARDSIZE.y
void ShowBoard(const vector<vector<int>> &board, string mode="symbol");
void ShowBoard(const int board[15][15], string mode="symbol");

#endif // SHAPEFINDER_H
