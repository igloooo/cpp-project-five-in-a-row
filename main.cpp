/*
 * File: main.cpp
 * -----------------------------
 * This file is used to call the functions in the three other files
 * in order to test them.
 */
#include <iostream>
#include "console.h"
#include "gametreenode.h"
#include "gamemodel.h"
using namespace std;

int main()
/*
 * Function: main
 * usage: main()
 * -----------------------------
 * This function is used to call the functions in the three other files.
 */
{
    GameModel myModel("white");
    cout<<myModel<<endl;
    cout<<endl;
    cout<<"move1 "<<myModel.TakeMove(2,2)<<endl;
    cout<<myModel<<endl;
    cout<<"move2 "<<myModel.TakeMove(2,2)<<endl;
    cout<<myModel<<endl;
    cout<<"move3 "<<myModel.TakeMove(15,15)<<endl;
    myModel.TakeMove(3,3);
    myModel.TakeMove(4,4);
    myModel.TakeMove(5,5);
    cout<<"move7 "<<myModel.TakeMove(6,6)<<endl;
    cout<<myModel<<endl;
}
