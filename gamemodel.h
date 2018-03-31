#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <iostream>
#include "map.h"
#include "grid.h"
#include "coordinate.h"
#include "stone.h"
using namespace std;

/* GameModel, and Stone class use some string command
 * "white""black""empty" for chess board position,
 * "illegal""continuing""terminated" for action, rule
 * checking feedback, "white""black" for stone color,
 * and player specification(whose_turn), "white""black"
 * "tie""None" for winner specification.
 */

class GameModel
{
public:
    GameModel(string who_first);
    GameModel(string who_first, Map<string, int> representation,
             Coordinate board_size);

    string TakeMove(Coordinate xy);//"illegal""continuing""terminated"
    string TakeMove(int x, int y);
    string CheckRule(Coordinate xy);//"illegal""continuing""white""black""tie"
    string CheckRule(int x, int y);

    bool EndGame(string winner);

    /* change history record and current board
     * true if successful, false if unsuccessful;
     * just cancel one step, so somethimes it should be used twice;
     */
    bool CancelLastMove();

    Map<string, int> & get_representation();
    Coordinate & get_board_size();
    string get_who_first();
    bool isFirstPlayer(string color);
    string get_whose_turn();
    bool isTerminated();
    string get_winner();
    int get_round();
    bool isEmptyBoard();
    Stone get_history_move(int round, string color);
    Stone get_history_move(int step);
    Stone get_last_move();

    string toString();
#include "gamemodelpriv.h"
};
ostream & operator<<(ostream & os, GameModel & rhs);

#endif // GAMEMODEL_H
