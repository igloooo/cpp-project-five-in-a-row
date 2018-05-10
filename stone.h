#ifndef STONE_H
#define STONE_H
#include <iostream>
#include "coordinate.h"
using namespace std;


class Stone
{
public:
    int x;
    int y;
    string color;
    Stone();
    Stone(int x, int y, string color);
    Stone(Coordinate co, string color);
    Stone(const Stone & rhs);
    string toString() const;
    Stone & operator=(const Stone &rhs);

};
bool operator< (const Stone &st1, const Stone &st2);
bool operator == (const Stone &st1, const Stone &st2);
ostream & operator<<(ostream & os, Stone & rhs);

#endif // STONE_H
