#include "stone.h"
#include <iostream>
#include "coordinate.h"
using namespace std;

Stone::Stone(){}

Stone::Stone(int x, int y, string color){
    if((color!="white")&&(color!="black")&&(color!="empty")){
        throw "invalid argument color: Stone color should be either \'black\' \'white\' or \'empty\'";
    }
    this->x = x;
    this->y = y;
    this->color = color;
}


Stone::Stone(Coordinate co, string color){
    if((color!="white")&&(color!="black")&&(color!="empty")){
        throw "invalid argument color: Stone color should be either \'black\' \'white\' or \'empty\'";
    }
    this->x = co.x;
    this->y = co.y;
    this->color = color;
}


Stone::Stone(const Stone &rhs){
    this->x = rhs.x;
    this->y = rhs.y;
    this->color = rhs.color;
}

string Stone::toString() const{
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

bool operator < (const Stone &st1, const Stone &st2){
    if(st1.color!=st2.color){
        return st1.color<st2.color;
    }else if(st1.x!=st2.x){
        return st1.x<st2.x;
    }else{
        return st1.y<st2.y;
    }
}
ostream & operator<<(ostream & os, Stone & rhs){
    return os<<rhs.toString();
}
