#include "gameai.h"
#include <iostream>
#include "gamemodel.h"
#include "stone.h"
#include "coordinate.h"
#include <random>
#include "shapefinder.h"
#include <stack>
#include <vector>
#include <map>
#include <time.h>

using namespace std;


GameAI::GameAI(GameModel & game, string color, int max_depth):MAX_DEPTH(max_depth),GameModel(game.get_first_player(), game.get_second_player()){
    //check if color and player type matches
    Player my_player;
    if(color==WHO_FIRST){
        my_player = get_first_player();
    }else{
        my_player = get_second_player();
    }
    if(my_player!=COMPUTER){
        throw "mismatch between specified color and player type.";
    }
    my_color = color;

    InitHash();
    Update(game);

    fiveValue.resize(BOARDSIZE.x);
    for(int i=0;i<BOARDSIZE.x;i++){
        fiveValue.resize(BOARDSIZE.y);
    }
    siteValue.resize(BOARDSIZE.x);
    for(int i=0;i<BOARDSIZE.x;i++){
        siteValue.resize(BOARDSIZE.y);
    }
}

void GameAI::Update(GameModel &game){
    whose_turn = game.get_whose_turn();
    terminated = game.isTerminated();
    winner = game.get_winner();
    round = game.get_round();
    current_board.resize(BOARDSIZE.x);
    for(int i=0;i<BOARDSIZE.x;i++){
        current_board[i].resize(BOARDSIZE.y);
        for(int j=0;j<BOARDSIZE.y;j++){
            current_board[i][j] = REPRESENTATION.at(game.at(i,j).color);
        }
    }
    for(int i=0; i<game.get_steps(); i++){
        history_moves.push_back(game.get_history_move(i));
        UpdateHash(history_moves[i]);
    }
    num_of_empty_places = game.get_num_of_empty_places();
    game_depth = game.get_steps();
    state_values.clear();//since searching depth is changed, state values are cleared in each step
}

Coordinate GameAI::Decide(){
    //see if vct exists
    bool canWin;
    Coordinate VCT_move;
    CalculateVCT(canWin, VCT_move);
    if(canWin){
        return VCT_move;
    }
    //do ab search, direct winning has been excluded.
    stack<Coordinate> options;
    RankOptions(options);
    Coordinate move;//the move to try
    Coordinate opt_move;
    int child_ab;
    int ab = -LARGEST_NUMBER;
    while(!options.empty()){
        move = options.top();
        options.pop();
        TryMove(move);
        child_ab = -ABSearch(ab);//opponent's score should be reversed
        if(child_ab>ab){
            ab = child_ab;
            opt_move = move;
        }
        CancelTry();
    }
    return opt_move;
}

int GameAI::ABSearch(int p_ab){
    //global_step += 1;
    //if(global_step%100==0)
    //    cout<<global_step<<endl;
    if(get_relative_depth()<MAX_DEPTH){
    //in tree phase
        stack<Coordinate> options;
        RankOptions(options);
        int ab = -LARGEST_NUMBER;
        Coordinate opt_move;
        while(!options.empty()){
            Coordinate move = options.top();
            options.pop();
            string result = TryMove(move);
            if(result=="terminated"){
                return LARGEST_NUMBER;
            }else{
                int child_ab = -ABSearch(ab);//opponents' score should be inversed
                //keep track of optimal move and ab
                if(child_ab>ab){
                    ab = child_ab;
                    opt_move = move;
                }
                //prunning
                if(-ab<=p_ab){
                    cout<<"prunning happens!"<<endl;
                    return ab;
                }
            }
            CancelTry();
        }
        return ab;
    }else{
    //at the leaf
        int score;
        bool canWin;
        Coordinate rubbish;
        CalculateVCT(canWin, rubbish);
        if(canWin){
            return LARGEST_NUMBER;
        }else{
            return EstimateState();
        }
    }
}

string GameAI::TryMove(Coordinate move){
    //the order cannot be changed.
    string result = TakeMove(move);
    if(result=="illegal"){
        return result;
    }
    UpdateHash(get_last_move());
    return result;
}

void GameAI::CancelTry(){
    //the order cannot be changed.
    RedoHash();
    CancelLastMove();
}

bool GameAI::isMaxNode(){
    int relative_depth = get_relative_depth();
    return relative_depth%2==0;
}

int GameAI::get_relative_depth(){
    return get_steps()-game_depth;
}

void GameAI::InitHash(){
    vector<string> colors = {"white", "black"};
    vector<int> bitstr(HASH_SIZE);
    for(string &color:colors){
        for(int i=0;i<BOARDSIZE.x;i++){
            for(int j=0;j<BOARDSIZE.y;j++){
                for(int k=0;k<HASH_SIZE;k++){
                    srand((unsigned)time(0));
                    bitstr[k] = rand();//15 bit
                }
                Stone temp_st(i,j,color);
                hash_lookup[temp_st] = bitstr;
            }
        }
    }
    cur_hash.resize(HASH_SIZE);
    for(int i=0;i<HASH_SIZE;i++){
        cur_hash[i] = 0;
    }
}

void GameAI::UpdateHash(Stone last_move){
    if(last_move.color=="empty"){
        throw "cannot update by empty stone.";
    }
    vector<int> last_move_hash(hash_lookup[last_move]);
    vector<int> new_hash(HASH_SIZE);
    apply_xor(new_hash, last_move_hash, cur_hash);
    for(int i=0;i<HASH_SIZE;i++){
        cur_hash[i] = new_hash[i];
    }
}

void GameAI::RedoHash(){
    //actually identical with UpdateHash due to property of xor.
    Stone last_move = get_last_move();
    UpdateHash(last_move);
}

void GameAI::RankOptions(stack<Coordinate> &options){
    set_five_value();
    set_site_value();
    int len = BOARDSIZE.x*BOARDSIZE.y;
    vector<int> keys(len);
    vector<Coordinate> values(len);
    for(int i=0;i<BOARDSIZE.x;i++){
        for(int j=0;j<BOARDSIZE.y;j++){
            keys[i*BOARDSIZE.x+j] = siteValue[i][j];
            values[i*BOARDSIZE.x+j] = Coordinate(i,j);
        }
    }
    sort(keys, values, 0, len);
    for(int i=0;i<len;i++){
        options.push(values[i]);
    }
}

int GameAI::EstimateState(){
//we simply add up our policy scores?
    //this is wrong, cannot find pointers
    if(state_values.find(cur_hash)!=state_values.end()){
        return state_values[cur_hash];
    }else{
        set_five_value();
        set_site_value();
        int score = 0;
        for(int i=0;i<BOARDSIZE.x;i++){
            for(int j=0;j<BOARDSIZE.y;j++){
                score += siteValue[i][j];
            }
        }
        state_values[cur_hash] = score;
        return score;
    }
}

//watch out for change of board
void GameAI::FindKillingMoves(stack<Coordinate> & killing_moves, string color){
    if((color!="black")&&(color!="white")){
        throw "invalid argument: color should be \'black\' or \'white\'";
    }
    for(int i=0;i<BOARDSIZE.x;i++){
        for(int j=0;j<BOARDSIZE.y;j++){
            if(current_board[i][j]==REPRESENTATION.at("empty")){
                current_board[i][j] = REPRESENTATION.at(color);
                if(have_five_at(current_board, color, i, j)||
                   have_active_four_at(current_board, color, i, j)){
                    killing_moves.push(Coordinate(i, j));
                }
                current_board[i][j] = REPRESENTATION.at("empty");
            }
        }
    }
}

void GameAI::FindHalfFourMoves(stack<Coordinate> & half_four_moves, string color){
    if((color!="black")&&(color!="white")){
        throw "invalid argument: color should be \'black\' or \'white\'";
    }
    for(int i=0;i<BOARDSIZE.x;i++){
        for(int j=0;j<BOARDSIZE.y;j++){
            if(current_board[i][j]==REPRESENTATION.at("empty")){
                current_board[i][j] = REPRESENTATION.at(color);
                stack<Coordinate> inf_dom;
                get_influence_domain(inf_dom, Coordinate(i,j));//empty places influenced by (i,j)
                bool isHalfFourMove = false;
                while(!inf_dom.empty()){
                    Coordinate move = inf_dom.top();
                    inf_dom.pop();
                    current_board[move.x][move.y] = REPRESENTATION.at(color);
                    if(have_five_at(current_board, color, move.x, move.y)){
                        isHalfFourMove = true;
                        break;
                    }
                    current_board[move.x][move.y] = REPRESENTATION.at("empty");
                }
                if(isHalfFourMove){
                    half_four_moves.push(Coordinate(i,j));
                }
                current_board[i][j] = REPRESENTATION.at("empty");
            }
        }
    }
}

void GameAI::FindActiveThreeMoves(stack<Coordinate> & active_three_moves, string color){
    if((color!="black")&&(color!="white")){
        throw "invalid argument: color should be \'black\' or \'white\'";
    }
    for(int i=0;i<BOARDSIZE.x;i++){
        for(int j=0;j<BOARDSIZE.y;j++){
            if(current_board[i][j]==REPRESENTATION.at("empty")){
                current_board[i][j] = REPRESENTATION.at(color);
                stack<Coordinate> inf_dom;
                get_influence_domain(inf_dom, Coordinate(i,j));//empty places influenced by (i,j)
                bool isActiveThreeMove = false;
                while(!inf_dom.empty()){
                    Coordinate move = inf_dom.top();
                    inf_dom.pop();
                    current_board[move.x][move.y] = REPRESENTATION.at(color);
                    if(have_active_four_at(current_board, color, move.x, move.y)){
                        isActiveThreeMove = true;
                        break;
                    }
                    current_board[move.x][move.y] = REPRESENTATION.at("empty");
                }
                if(isActiveThreeMove){
                    active_three_moves.push(Coordinate(i,j));
                }
                current_board[i][j] = REPRESENTATION.at("empty");
            }
        }
    }
}


void GameAI::CalculateVCT(bool &canWin, Coordinate &vic_move){
    stack<Coordinate> killing_moves;
    FindKillingMoves(killing_moves, whose_turn);
    if(!killing_moves.empty()){
        canWin = true;
        vic_move = killing_moves.top();
        killing_moves.pop();
        return;
    }

    stack<Coordinate> half_four_moves;
    stack<Coordinate> active_three_moves;
    FindHalfFourMoves(half_four_moves, whose_turn);
    FindActiveThreeMoves(active_three_moves, whose_turn);
    canWin = false;
    //The order cannot be changed, because there might be repetition in two of them.
    while(!half_four_moves.empty()){
        Coordinate move = half_four_moves.top();
        half_four_moves.pop();
        TryMove(move);
        bool canDefend;
        RespondVCT(canDefend, move, "four");
        if(!canDefend){
            canWin = true;
            vic_move = move;
            return;
        }
        CancelTry();
    }
    while(!active_three_moves.empty()){
        Coordinate move = active_three_moves.top();
        active_three_moves.pop();
        TryMove(move);
        bool canDefend;
        RespondVCT(canDefend, move, "three");
        if(!canDefend){
            canWin = true;
            return;
        }
    }
    return;
}

void GameAI::CalculateVCT(bool &canWin){
    Coordinate rubbish;
    CalculateVCT(canWin, rubbish);

}

void GameAI::RespondVCT(bool &canDefend, Coordinate op_move, string type){
    if(type=="three"){
        stack<Coordinate> half_four_moves;
        FindHalfFourMoves(half_four_moves, whose_turn);
        //choose to create half four to break opponents' VCT!
        if(!half_four_moves.empty()){
            canDefend = true;
        }
    }
    //try to stop killing moves. In case of type four threat, two points can be fatal;
    //in case of type three threat, three points are fatal.
    stack<Coordinate> killing_moves;
    FindKillingMoves(killing_moves, ReverseColor(whose_turn));
    canDefend = true;
    bool opcanWin;
    while(!killing_moves.empty()){
        Coordinate move = killing_moves.top();
        killing_moves.pop();
        TryMove(move);
        CalculateVCT(opcanWin);
        if(opcanWin){
            canDefend = false;
        }
        CancelTry();
    }
    return;
}


int GameAI::chart(int grade){
    int temp = 0;
    if(!isFirstPlayer(whose_turn)){
        switch(grade){
           case 0: temp=3;break;
           case 4: temp=35;break;
           case 8: temp=800;break;
           case 12: temp=15000;break;
           case 16: temp=800000;break;
           case 5: temp=15;break;
           case 10: temp=400;break;
           case 15: temp=1800;break;
           case 20: temp=100000;break;
           default: temp=0;
        }
     }
    else if(isFirstPlayer(whose_turn)){
        switch(grade){
           case 0: temp=3;break;
           case 4: temp=15;break;
           case 8: temp=400;break;
           case 12: temp=1800;break;
           case 16: temp=100000;break;
           case 5: temp=35;break;
           case 10: temp=800;break;
           case 15: temp=15000;break;
           case 20: temp=800000;break;
           default: temp=0;
        }
    }
    return temp;
}

void GameAI::set_five_value(){
    if(get_whose_turn()!=my_color){
    //change to opponent's perspective
        ReverseBoard();
    }
    fiveValue.resize(BOARDSIZE.x);
    for(int i=0;i<BOARDSIZE.x;i++){
        fiveValue[i].resize(BOARDSIZE.y);
        for(int j=0;j<BOARDSIZE.y;j++){
            int grade=0;          //x
            if(j<BOARDSIZE.y-4){
                for(int n=0;n<5;n++){
                    grade+=current_board[i][j]+4;
                }
                fiveValue[i][j].y=chart(grade);
            }
            grade=0;             //y
            if(i<BOARDSIZE.x-4){
                for(int n=0;n<5;n++){
                    grade+=current_board[i][j]+4;
                }
                fiveValue[i][j].x=chart(grade);
            }
             grade=0;            //r
            if(i<BOARDSIZE.x-4&&j<BOARDSIZE.y-4){
                for(int n=0;n<5;n++){
                    grade+=current_board[i][j]+4;
                }
                fiveValue[i][j].r=chart(grade);
            }
            grade=0;           //l
            if(j>=4&&i<BOARDSIZE.x-4){
                for(int n=0;n<5;n++){
                    grade+=current_board[i][j]+4;
                }
                fiveValue[i][j].l=chart(grade);
            }
        }
    }
    if(get_whose_turn()!=my_color){
    //reverse the board back
        ReverseBoard();
    }
}
void GameAI::set_site_value()
{
    siteValue.resize(BOARDSIZE.x);
    for(int i=0;i<BOARDSIZE.x;i++){
        siteValue[i].resize(BOARDSIZE.y);
        for(int j=0;j<BOARDSIZE.y;j++)
        {
            siteValue[i][j]=fiveValue[i][j].l+fiveValue[i][j].r+fiveValue[i][j].x+fiveValue[i][j].y;
            for(int n=1;n<5;n++)
            {
                if(j-n>=0)
                    siteValue[i][j]+=fiveValue[i][j-n].y;
                if(i-n>=0)
                    siteValue[i][j]+=fiveValue[i-n][j].x;
                if(i-n>=0&&j-n>=0)
                    siteValue[i][j]+=fiveValue[i-n][j-n].r;
                if(i-n>=0&&j+n<=BOARDSIZE.y)
                    siteValue[i][j]+=fiveValue[i-n][j+n].l;
            }
        }
    }
}

void GameAI::sort(vector<int> &keys, vector<Coordinate> &values, int h, int t){
    int n = t-h;
    if(n<=1){
        return;
    }else if(n==2){
        if(keys[h]<=keys[h+1]){
            return;
        }else{
            int temp1 = keys[h];
            keys[h] = keys[h+1];
            keys[h+1] = temp1;
            Coordinate temp2 = values[h];
            values[h] = values[h+1];
            values[h+1] = temp2;
        }
    }else{
        sort(keys, values, h, h+n/2);
        sort(keys, values, h+n/2, t);
        int temp1[n];
        Coordinate temp2[n];
        int l_ind=h;
        int r_ind=h+n/2;
        for(int i=0; i<n; i++){
            if((l_ind<h+n/2)&&(r_ind<t)){
                if(keys[l_ind]<=keys[r_ind]){
                    temp1[i] = keys[l_ind];
                    temp2[i] = values[l_ind];
                    l_ind++;
                }else{
                    temp1[i] = keys[r_ind];
                    temp2[i] = values[r_ind];
                    r_ind++;
                }
            }else if(r_ind==n){
                temp1[i] = keys[l_ind];
                temp2[i] = values[l_ind];
                l_ind++;
            }else{
                temp1[i] = keys[r_ind];
                temp2[i] = values[r_ind];
                r_ind++;
            }
        }
        for(int i=0;i<n;i++){
            keys[i+h] = temp1[i];
            values[i+h] = temp2[i];
        }
    }
}

void GameAI::apply_xor(vector<int> &result, const vector<int> &bitstr1, const vector<int> &bitstr2){
    for(int i=0;i<HASH_SIZE;i++){
        result[i] = bitstr1[i]^bitstr2[i];
    }
}


void GameAI::ReverseBoard(){
    for(int i=0;i<BOARDSIZE.x;i++){
        for(int j=0;j<BOARDSIZE.y;j++){
            current_board[i][j] = -current_board[i][j];
        }
    }
}

void GameAI::get_influence_domain(stack<Coordinate> &inf_dom, Coordinate co){
    map<string, Coordinate>::const_iterator it = DIRECTIONS.cbegin();
    while(it!=DIRECTIONS.cend()){
        for(int n=-HASH_SIZE;n<5;n++){
            Coordinate cur = co+multiply(it->second,n);
            if((cur>=Coordinate(0,0))&&(cur<BOARDSIZE)&&
                current_board[cur.x][cur.y]==REPRESENTATION.at("empty")){
                inf_dom.push(cur);
            }
        }
        it++;
    }
}

string GameAI::toString(){
    string str;
    str += string("--------------round") + to_string(round) + " " + (isFirstPlayer(whose_turn)?"first hand":"second hand") + "-------------\n";
    str += "-------------game info----------------\n";
    str += string("FIRST_PLAYER ") + ((FIRST_PLAYER==HUMAN)?"HUMAN":"COMPUTER") + "\n";
    str += string("SECOND_PLAYER ") + ((SECOND_PLAYER==HUMAN)?"HUMAN":"COMPUTER") + "\n";
    str += string("whose_turn ") + whose_turn + "\n";
    str += string("current player type ") + ((get_cur_player()==HUMAN)?"HUMAN":"COMPUTER") + "\n";
    str += string("terminated ") + (isTerminated()?"true":"false") + "\n";
    str += string("winner ") + winner + "\n";
    str += string("num_of_empty_places ") + to_string(get_num_of_empty_places()) + "\n";
    str += "-------------ai info------------------\n";
    str += string("MAX_DEPTH ") + to_string(MAX_DEPTH) + "\n";
    str += string("my_color ") + my_color + "\n";
    str += string("game_depth ") + to_string(game_depth) + "\n";
    str += string("hash_lookup size ") + to_string(hash_lookup.size()) + "\n";
    str += string("state_values size ") + to_string(state_values.size()) + "\n";
    str += "cur_hash ";
    for(int i=0;i<HASH_SIZE;i++){
        str += to_string(cur_hash[i]);
    }
    str += "\n";
    str += "fiveValue size " + to_string(fiveValue.size()) + "\n";
    str += "siteValue size " + to_string(siteValue.size()) + "\n";
    return str;
}

ostream & operator << (ostream & os, GameAI & ai){
    return os<<ai.toString();
}
