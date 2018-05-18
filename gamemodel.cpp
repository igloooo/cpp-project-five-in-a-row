#include "gamemodel.h"
#include <iostream>
#include <map>
#include <vector>
#include "coordinate.h"
#include "stone.h"
#include "shapefinder.h"
using namespace std;

GameModel::GameModel():FIRST_PLAYER(HUMAN),SECOND_PLAYER(HUMAN){}

GameModel::GameModel(Player first_player, Player second_player):FIRST_PLAYER(first_player), SECOND_PLAYER(second_player){
    whose_turn = WHO_FIRST;
    terminated = false;
    winner = "None";
    round = 1;
    current_board.resize(BOARDSIZEX);
    for(int i=0;i<BOARDSIZEX;i++){
        current_board[i].resize(BOARDSIZEY);
        for(int j=0; j<BOARDSIZEY; j++){
            current_board[i][j] = EMPTY_R;
        }
    }
    num_of_empty_places = BOARDSIZEX*BOARDSIZEY;
    whose_turn_repre = REPRESENTATION.at(whose_turn);
    opponent_repre = REPRESENTATION.at(ReverseColor(whose_turn));
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
            current_board[xy.x][xy.y] = whose_turn_repre;
            Stone new_stone_record = Stone(xy.x, xy.y, whose_turn);
            history_moves.push_back(new_stone_record);
            num_of_empty_places -= 1;
            whose_turn = ReverseColor(whose_turn);
            whose_turn_repre = REPRESENTATION.at(whose_turn);
            opponent_repre = REPRESENTATION.at(ReverseColor(whose_turn));
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
    string result;
    if((xy.x)<0||(xy.x>=BOARDSIZEX)||(xy.y<0)||(xy.y>=BOARDSIZEY)){
        result = "illegal";
    }else if(current_board[xy.x][xy.y]!=EMPTY_R){
        result = "illegal";
    }else{
        //this is a dangerous line since it changes board without invoking methods
        current_board[xy.x][xy.y] = whose_turn_repre;
        if(have_five_at(current_board, whose_turn, xy)){
            result = whose_turn;
        }else if(num_of_empty_places==1){
            result = "tie";
        }else if(num_of_empty_places<1) {
            result = "unexpected error: num_of_empty_places = " + to_string(num_of_empty_places);
        }else{
            result = "continuing";
        }
        current_board[xy.x][xy.y] = EMPTY_R;
    }
    return result;
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
        history_moves.pop_back();
        current_board[last_move.x][last_move.y] = EMPTY_R;
        if(!isFirstPlayer(last_move.color)){
            round -= 1;
        }
        whose_turn = ReverseColor(whose_turn);
        num_of_empty_places += 1;
        return true;
    }else{
        return false;
    }
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
int GameModel::get_steps(){
    return (int)history_moves.size();
}

bool GameModel::isEmptyBoard(){
    return history_moves.empty();
}

int GameModel::get_num_of_empty_places(){
    return num_of_empty_places;
}
Player GameModel::get_first_player(){
    return FIRST_PLAYER;
}
Player GameModel::get_second_player(){
    return SECOND_PLAYER;
}
Player GameModel::get_cur_player(){
    if(whose_turn==WHO_FIRST){
        return get_first_player();
    }else{
        return get_second_player();
    }
}

Stone GameModel::at(Coordinate xy){
    return at(xy.x,xy.y);
}

Stone GameModel::at(int x, int y){
    int value = current_board[x][y];
    if(value==BLACK_R){
        return Stone(x,y,"black");
    }else if(value==WHITE_R){
        return Stone(x,y,"white");
    }else if(value==EMPTY_R){
        return Stone(x,y,"empty");
    }else{
        throw "unexpected error";
    }
}

Stone GameModel::get_history_move(int round, string color){
    int index;
    if(isFirstPlayer(color)){
        index = round*2-2;
    }else{
        index = round*2-1;
    }
    if((index>=(int)history_moves.size())||(index<0)){
        throw "index "+to_string(index)+" larger than container size "
              +to_string((int)history_moves.size());
    }
    return history_moves[index];
}
Stone GameModel::get_history_move(int step){
    if((step>=(int)history_moves.size())||step<0){
        throw "index "+to_string(step)+" larger than container size "
              +to_string((int)history_moves.size());
    }
    return history_moves[step];
}
Stone GameModel::get_last_move(){
    if(isEmptyBoard()){
        throw "The board is still empty!";
    }
    return history_moves[(int)history_moves.size()-1];
}

vector<Stone> GameModel::get_all_stones(){
    vector<Stone> all_stones;
    for(int i=0;i<get_steps();i++){
        all_stones[i] = history_moves[i];
    }
    return all_stones;
}

string GameModel::toString(){
    string str;
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

string ReverseColor(string color){
    if(color=="black"){
        return "white";
    }else if(color=="white"){
        return "black";
    }else{
        throw "\'color\' expects \'black\' or \'white\', get "+color;
    }
}

ostream & operator<<(ostream & os, GameModel & rhs){
    return os<<rhs.toString();
}

Player ReversePlayer(Player player){
    if(player==COMPUTER){
        return HUMAN;
    }else{
        return COMPUTER;
    }
}
