#ifndef COORDINATE_H
#define COORDINATE_H

#include <iostream>
#include <ostream>
using namespace std;


class Coordinate
{
public:
    int x;
    int y;
    Coordinate();
    Coordinate(int x, int y);
    Coordinate(const Coordinate & rhs);
    string toString();
    Coordinate & operator=(const Coordinate &rhs);
};
ostream & operator<<(ostream &os, Coordinate & coor);

#endif // COORDINATE_H
