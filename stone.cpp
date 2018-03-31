#include "stone.h"
#include <iostream>
using namespace std;

Stone::Stone(){}

Stone::Stone(int x, int y, string color){
    this->x = x;
    this->y = y;
    this->color = color;
}

Stone::Stone(const Stone &rhs){
    this->x = rhs.x;
    this->y = rhs.y;
    this->color = rhs.color;
}

string Stone::toString(){
    return ("(" + to_string(x)) + "," + to_string(y) + "," + color + ")";
}

Stone & Stone::operator=(const Stone & rhs){
    if(this!=&rhs){
        this->x = rhs.x;
        this->y = rhs.y;
        this->color = rhs.color;
    }
    return *this;
}

ostream & operator<<(ostream & os, Stone & rhs){
    return os<<rhs.toString();
}
