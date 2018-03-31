#include "coordinate.h"
#include <iostream>
#include <ostream>
using namespace std;

Coordinate::Coordinate(){

}
Coordinate::Coordinate(int x, int y){
    this->x = x;
    this->y = y;
}
Coordinate::Coordinate(const Coordinate & rhs){
    this->x = rhs.x;
    this->y = rhs.y;
}
string Coordinate::toString(){
    return "(" + to_string(x) + "," + to_string(y) + ")";
}

Coordinate & Coordinate::operator=(const Coordinate & rhs){
    this->x = rhs.x;
    this->y = rhs.y;
    return *this;
}

ostream & operator<<(ostream &os, Coordinate & coor){
    return os<<coor.toString();
}
