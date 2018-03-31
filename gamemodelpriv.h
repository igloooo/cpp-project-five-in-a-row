#ifndef GAMEMODELPRIV_H
#define GAMEMODELPRIV_H
/*
#include<iostream>
#include "map.h"
#include "coordinate.h"
#include "stone.h"
using namespace std;
*/
private:
    Map<string, int> REPRESENTATION;
    Coordinate BOARDSIZE;
    string WHO_FIRST;

    string whose_turn;
    bool terminated;
    string winner;//"black","white","tie","None"
    int round;

    Grid<int> current_board;
    Vector<Stone> history_moves;

    int num_of_empty_places;

    string ReverseColor(string color);
    void CopyProperties(const GameModel & rhs);
#endif // GAMEMODELPRIV_H
