#ifndef STONE_H
#define STONE_H
#include <iostream>
using namespace std;


class Stone
{
public:
    int x;
    int y;
    string color;
    Stone();
    Stone(int x, int y, string color);
    Stone(const Stone & rhs);
    string toString();
    Stone & operator=(const Stone &rhs);
};
ostream & operator<<(ostream & os, Stone & rhs);

#endif // STONE_H
