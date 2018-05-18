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
#include "gameai.h"
using namespace std;


void move_and_display(GameModel & model, int x, int y){
    cout<<model.TakeMove(x, y)<<endl;
    cout<<model<<endl;
    cout<<endl;
}
void showBoard(GameModel & model){
    for(int i=0;i<BOARDSIZEX;i++){
        if(i!=0){
            cout<<"-----------------------------------------------------------"<<endl;
        }
        for(int j=0;j<BOARDSIZEY;j++){
            string piece = model.at(i,j).color;
            if(piece=="black"){
                cout<<" X ";
            }else if(piece=="empty"){
                cout<<"   ";
            }else{
                cout<<" O ";
            }
            if(j<BOARDSIZEY-1){
                cout<<"|";
            }
        }
        cout<<endl;
    }
}

void test_game_model(){
    GameModel myModel = GameModel(HUMAN,COMPUTER);
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


/*
 * Function: main
 * usage: implicitly used
 * -----------------------------
 *
 */
int main()
{
    GameModel game(HUMAN,COMPUTER);
    //game.TakeMove(1,1);
    game.TakeMove(6,6);
    game.TakeMove(2,2);
    game.TakeMove(10,10);
    game.TakeMove(3,3);
    game.TakeMove(11,11);
    game.TakeMove(4,4);
    game.TakeMove(13,13);
    game.TakeMove(3,5);
    game.TakeMove(0,14);
    game.TakeMove(5,3);
    game.TakeMove(14,0);
    //game.TakeMove(6,2);
    showBoard(game);
    //game.TakeMove(7,7);
    //game.TakeMove(7,6);
    GameAI ai(game,"white");
    Coordinate new_move = ai.Decide();
    cout<<new_move.x<<","<<new_move.y<<endl;
    return 0;
}
