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
    string toString() const;
    Coordinate & operator=(const Coordinate &rhs);
};
ostream & operator<<(ostream &os, Coordinate & coor);
Coordinate operator+(Coordinate co1, Coordinate co2);
bool operator<(Coordinate co1, Coordinate co2);
bool operator<=(Coordinate co1, Coordinate co2);
bool operator > (Coordinate co1, Coordinate co2);
bool operator>=(Coordinate co1, Coordinate co2);
bool operator==(Coordinate co1, Coordinate co2);
Coordinate multiply(Coordinate co, int n);

#endif // COORDINATE_H
