#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <iostream>
#include <map>
#include <vector>
#include "coordinate.h"
#include "stone.h"
using namespace std;

const map<string, int> REPRESENTATION = {{"black",1},{"white",-1},{"empty",0}};
const Coordinate BOARDSIZE(15,15);
const string WHO_FIRST = "black";
const map<string, Coordinate> DIRECTIONS = {{"x",Coordinate(1,0)},
                                            {"y",Coordinate(0,1)},
                                            {"d",Coordinate(1,1)},
                                            {"a",Coordinate(1,-1)}};
enum Player{HUMAN, COMPUTER};

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
    GameModel();
    GameModel(Player first_player, Player second_player);

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

    bool isFirstPlayer(string color);
    string get_whose_turn();
    bool isTerminated();
    string get_winner();
    int get_round();
    int get_steps();//size of history_moves
    bool isEmptyBoard();
    int get_num_of_empty_places();
    Player get_first_player();
    Player get_second_player();
    Player get_cur_player();

    Stone at(Coordinate xy);
    Stone at(int x, int y);
    Stone get_history_move(int round, string color);
    Stone get_history_move(int step);
    Stone get_last_move();
    vector<Stone> get_all_stones();

    string toString();

protected:
    const Player FIRST_PLAYER;
    const Player SECOND_PLAYER;
    string whose_turn;
    bool terminated;
    string winner;//"black","white","tie","None"
    int round;

    vector<vector<int>> current_board;
    vector<Stone> history_moves;

    int num_of_empty_places;
};
ostream & operator<<(ostream & os, GameModel & rhs);
string ReverseColor(string color);
Player ReversePlayer(Player player);

#endif // GAMEMODEL_H
