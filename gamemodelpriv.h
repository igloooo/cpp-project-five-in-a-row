#ifndef GAMEMODELPRIV_H
#define GAMEMODELPRIV_H
/*
#include<iostream>
#include "map.h"
#include "coordinate.h"
#include "stone.h"
using namespace std;
*/
protected:
    string whose_turn;
    bool terminated;
    string winner;//"black","white","tie","None"
    int round;

    Grid<int> current_board;
    Vector<Stone> history_moves;

    int num_of_empty_places;

#endif // GAMEMODELPRIV_H
