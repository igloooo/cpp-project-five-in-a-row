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
#include "stone.h"
using namespace std;


void move_and_display(GameModel & model, int x, int y){
    cout<<model.TakeMove(x, y)<<endl;
    cout<<model<<endl;
    cout<<endl;
}
void showBoard(GameModel & model){
    for(int i=0;i<BOARDSIZE.x;i++){
        if(i!=0){
            cout<<"-----------------------------------------------------------"<<endl;
        }
        for(int j=0;j<BOARDSIZE.y;j++){
            string piece = model.at(i,j).color;
            if(piece=="black"){
                cout<<" X ";
            }else if(piece=="empty"){
                cout<<"   ";
            }else{
                cout<<" O ";
            }
            if(j<BOARDSIZE.y-1){
                cout<<"|";
            }
        }
        cout<<endl;
    }
}

void test_game_model(){
    GameModel myModel = GameModel();
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
    myModel.CancelLastMove();
    cout<<myModel<<endl;
    myModel.CancelLastMove();
    cout<<myModel<<endl;
    showBoard(myModel);
}

int main()
/*
 * Function: main
 * usage: main()
 * -----------------------------
 * This function is used to call the functions in the three other files.
 */
{
    test_game_model();
}
