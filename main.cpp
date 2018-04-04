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


void move_and_display(GameModel & model, int x, int y){
    cout<<model.TakeMove(x, y)<<endl;
    cout<<model<<endl;
    cout<<endl;
}
void test_game_model(){
    GameModel myModel("black");
    move_and_display(myModel,0,0);
    move_and_display(myModel,1,0);
    move_and_display(myModel,0,1);
    move_and_display(myModel,0,0);
    move_and_display(myModel,1,0);
    move_and_display(myModel,0,1);
    move_and_display(myModel,1,1);
    move_and_display(myModel,0,2);
    move_and_display(myModel,1,2);
    move_and_display(myModel,0,3);
    move_and_display(myModel,1,3);
    move_and_display(myModel,0,4);
    move_and_display(myModel,1,4);
}

int main()
/*
 * Function: main
 * usage: main()
 * -----------------------------
 * This function is used to call the functions in the three other files.
 */
{

}
