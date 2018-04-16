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
string Coordinate::toString() const{
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

Coordinate operator+(Coordinate co1, Coordinate co2){
    return Coordinate(co1.x+co2.x,co1.y+co2.y);
}

bool operator<(Coordinate co1, Coordinate co2){
    return (co1.x<co2.x)&&(co1.y<co2.y);
}

bool operator<=(Coordinate co1, Coordinate co2){
    return (co1.x<=co2.x)&&(co1.y<=co2.y);
}

bool operator > (Coordinate co1, Coordinate co2){
    return (co1.x>co2.x)&&(co1.y>co2.y);
}

bool operator>=(Coordinate co1, Coordinate co2){
    return (co1.x>=co2.x)&&(co1.y>=co2.y);
}

bool operator==(Coordinate co1, Coordinate co2){
    return (co1.x==co2.x)&&(co1.y==co2.y);
}

Coordinate multiply(Coordinate co, int n){
    return Coordinate(co.x*n, co.y*n);
}
