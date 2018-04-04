#include "gamemodel.h"
#include <iostream>
#include "map.h"
#include "grid.h"
#include "coordinate.h"
#include "stone.h"
using namespace std;

GameModel::GameModel(string who_first){
    if((who_first!="white")&&(who_first!="black")){
        throw "\'who_first\' should be either \'black\' or \'white\', get " + who_first;
    }
    REPRESENTATION.put("white", -1);
    REPRESENTATION.put("black", 1);
    REPRESENTATION.put("empty", 0);
    BOARDSIZE.x = 15;
    BOARDSIZE.y = 15;
    //the following lines are identical with the next constructor
    WHO_FIRST = who_first;
    whose_turn = who_first;
    terminated = false;
    winner = "None";
    round = 1;
    current_board.resize(BOARDSIZE.x,BOARDSIZE.y);
    int empty_repr = REPRESENTATION["empty"];
    for(int i=0;i<BOARDSIZE.x;i++){
        for(int j=0;j<BOARDSIZE.y;j++){
            current_board[i][j] = empty_repr;
        }
    }
    num_of_empty_places = BOARDSIZE.x*BOARDSIZE.y;
}
GameModel::GameModel(string who_first, Map<string, int> representation, Coordinate board_size){
    if(representation["white"]==representation["black"]||
       representation["white"]==representation["empty"]||
       representation["black"]==representation["empty"]){
        throw "different states should have different representations!"+representation.toString();
    }
    REPRESENTATION.put("white", representation["white"]);
    REPRESENTATION.put("black", representation["black"]);
    REPRESENTATION.put("empty", representation["empty"]);
    if((board_size.x<=0)||(board_size.y<=0)){
        throw "expect positive board size, get "+to_string(board_size.x)+","+to_string(board_size.y);
    }
    BOARDSIZE.x = board_size.x;
    BOARDSIZE.y = board_size.y;
    if((who_first!="black")&&(who_first!="white")){
       throw "\'who_first\' should be either \'black\' or \'white\'";
    }
    WHO_FIRST = who_first;
    whose_turn = who_first;
    terminated = false;
    winner = "None";
    round = 1;
    current_board.resize(BOARDSIZE.x,BOARDSIZE.y);
    int empty_repr = REPRESENTATION["empty"];
    for(int i=0;i<BOARDSIZE.x;i++){
        for(int j=0;j<BOARDSIZE.y;j++){
            current_board[i][j] = empty_repr;
        }
    }
    num_of_empty_places = BOARDSIZE.x*BOARDSIZE.y;
}


string GameModel::TakeMove(Coordinate xy){
    if(isTerminated()){
        return "illegal";
    }else{
        string checking_result = CheckRule(xy);
        if(checking_result=="illegal"){
            return "illegal";
        }else{
            if(!isFirstPlayer(whose_turn)){
                round += 1;
            }
            current_board[xy.x][xy.y] = REPRESENTATION[whose_turn];
            Stone new_stone_record = Stone(xy.x, xy.y, whose_turn);
            history_moves.add(new_stone_record);
            num_of_empty_places -= 1;
            whose_turn = ReverseColor(whose_turn);

            if((checking_result=="white")||
                 (checking_result=="black")||
                 (checking_result=="tie")){
                EndGame(checking_result);
                return "terminated";
            }else if(checking_result=="continuing"){
                return "continuing";
            }else{
                throw "unexpected error: checking_result ="+checking_result;
            }
        }
    }
}

string GameModel::TakeMove(int x, int y){
    return TakeMove(Coordinate(x, y));
}

string GameModel::CheckRule(Coordinate xy){
    if((xy.x)<0||(xy.x>=BOARDSIZE.x)||(xy.y<0)||(xy.y>=BOARDSIZE.y)){
        return "illegal";
    }else if(current_board[xy.x][xy.y]!=REPRESENTATION["empty"]){
        return "illegal";
    }else{       
        //horizontal line
        int max_count = 1;
        int count = 1;
        int i = 1;
        while((xy.x+i<BOARDSIZE.x)&&
             (current_board[xy.x+i][xy.y]==REPRESENTATION[whose_turn])){
             count++;
             i++;
        }
        i = -1;
        while((xy.x+i>=0)&&
             (current_board[xy.x+i][xy.y]==REPRESENTATION[whose_turn])){
             count++;
             i--;
        }
        if(count>max_count){
            max_count = count;
        }
        //vertical line
        count = 1;
        i = 1;
        while((xy.y+i<BOARDSIZE.y)&&
             (current_board[xy.x][xy.y+i]==REPRESENTATION[whose_turn])){
             count++;
             i++;
        }
        i = -1;
        while((xy.y+i>=0)&&
             (current_board[xy.x][xy.y+i]==REPRESENTATION[whose_turn])){
             count++;
             i--;
        }
        if(count>max_count){
            max_count = count;
        }
        //left_down to right_up
        count = 1;
        i = 1;
        while((xy.x+i<BOARDSIZE.x)&&(xy.y+i<BOARDSIZE.y)&&
              current_board[xy.x+i][xy.y+i]==REPRESENTATION[whose_turn]){
              count++;
              i++;
        }
        i = -1;
        while((xy.x+i>=0)&&(xy.y+i>=0)&&
              current_board[xy.x+i][xy.y+i]==REPRESENTATION[whose_turn]){
              count++;
              i--;
        }
        if(count>max_count){
            max_count = count;
        }
        //left_up to right_down
        count = 1;
        i = 1;
        while((xy.x+i<BOARDSIZE.x)&&(xy.y-i>=0)&&
              current_board[xy.x+i][xy.y-i]==REPRESENTATION[whose_turn]){
              count++;
              i++;
        }
        i = -1;
        while((xy.x+i>=0)&&(xy.y-i<BOARDSIZE.y)&&
              current_board[xy.x+i][xy.y-i]==REPRESENTATION[whose_turn]){
              count++;
              i--;
        }
        if(count>max_count){
            max_count = count;
        }
        if(max_count>=5){
            return whose_turn;
        }else if(num_of_empty_places==1){
            return "tie";
        }else if(num_of_empty_places<1) {
            throw "unexpected error: num_of_empty_places = " + to_string(num_of_empty_places);
        }else{
            return "continuing";
        }
    }
}

string GameModel::CheckRule(int x, int y){
    return CheckRule(Coordinate(x, y));
}

bool GameModel::EndGame(string winner){
    if(!isTerminated()){
        terminated = true;
        this->winner = winner;
        return true;
    }else{
        return false;
    }

}

bool GameModel::CancelLastMove(){
    if(!isEmptyBoard()){
        if(isTerminated()){
            terminated = false;
            winner = "None";
        }
        Stone last_move = get_last_move();
        history_moves.remove(history_moves.size()-1);
        current_board[last_move.x][last_move.y] = REPRESENTATION["empty"];
        if(isFirstPlayer(last_move.color)){
            round -= 1;
        }
        whose_turn = ReverseColor(whose_turn);
        num_of_empty_places += 1;
        return true;
    }else{
        return false;
    }
}

Map<string, int> & GameModel::get_representation(){
    Map<string, int> repre;
    repre.put("white", REPRESENTATION["white"]);
    repre.put("black", REPRESENTATION["black"]);
    repre.put("empty", REPRESENTATION["empty"]);
    return repre;
}
Coordinate & GameModel::get_board_size(){
    Coordinate *board_size = new Coordinate(BOARDSIZE.x, BOARDSIZE.y);
    return *board_size;
}
string GameModel::get_who_first(){
    return WHO_FIRST;
}
bool GameModel::isFirstPlayer(string color){
    if(color!="white"&&color!="black"){
        throw "\'color\' expects \'black\' or \'white\', get "+color;
    }else{
        return color==WHO_FIRST;
    }
}
string GameModel::get_whose_turn(){
    return whose_turn;
}
bool GameModel::isTerminated(){
    return terminated;
}
string GameModel::get_winner(){
    return winner;
}
int GameModel::get_round(){
    return round;
}


bool GameModel::isEmptyBoard(){
    return history_moves.isEmpty();
}

Stone GameModel::get_history_move(int round, string color){
    int index;
    if(isFirstPlayer(color)){
        index = round*2-2;
    }else{
        index = round*2-1;
    }
    if((index>=history_moves.size())||(index<0)){
        throw "index "+to_string(index)+" larger than container size "
              +to_string(history_moves.size());
    }
    return history_moves[index];
}
Stone GameModel::get_history_move(int step){
    if((step>=history_moves.size())||step<0){
        throw "index "+to_string(step)+" larger than container size "
              +to_string(history_moves.size());
    }
    return history_moves[step];
}
Stone GameModel::get_last_move(){
    if(isEmptyBoard()){
        throw "The board is still empty!";
    }
    return history_moves[history_moves.size()-1];
}

string GameModel::toString(){
    string str;
    str += "REPRESENTATION: " + REPRESENTATION.toString() + "\n";
    str += "BOARDSIZE: " + BOARDSIZE.toString() + "\n";
    str += "WHO_FIRST: " + WHO_FIRST + "\n";
    str += "whose_turn: " + whose_turn + "\n";
    str += "terminiated: " + to_string(terminated) + "\n";
    str += "winner: " + winner + "\n";
    str += "round: " + to_string(round) + "\n";
    if(isEmptyBoard()){
        str += "last_move doesn\'t exist \n";
    }else{
        str += "last_move: " + get_last_move().toString() + "\n";
    }
    str += "num_of_empty_places: " + to_string(num_of_empty_places) + "\n";
    return str;
}

string GameModel::ReverseColor(string color){
    if(color=="black"){
        return "white";
    }else if(color=="white"){
        return "black";
    }else{
        throw "\'color\' expects \'black\' or \'white\', get "+color;
    }
}
/*
void GameModel::CopyProperties(const GameModel & rhs){
    this->REPRESENTATION = rhs.REPRESENTATION;
    this->BOARDSIZE = rhs.BOARDSIZE
}
*/
ostream & operator<<(ostream & os, GameModel & rhs){
    return os<<rhs.toString();
}
