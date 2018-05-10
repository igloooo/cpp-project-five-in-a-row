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
    int alpha = -LARGEST_NUMBER-1;
    int beta = LARGEST_NUMBER + 1;
    while(!options.empty()){
        move = options.top();
        options.pop();
        string result = TryMove(move);
        if(result=="illegal"){
            continue;
        }
        int new_alpha = ABSearch(alpha, beta);
        if(new_alpha>alpha){
            alpha = new_alpha;
            opt_move = move;
        }
        CancelTry();
    }
    //see if opponent's vct exist
    TryMove(opt_move);
    CalculateVCT(canWin, VCT_move);
    CancelTry();
    if(canWin){
        return VCT_move;
    }else{
        return opt_move;
    }
}

int GameAI::ABSearch(int alpha, int beta){
    global_step += 1;
    //cout<<get_relative_depth()<<endl;
    if(global_step%99==0){
        cout<<global_step<<endl;
        cout<<get_relative_depth()<<endl;
        cout<<alpha<<","<<beta<<endl;
        //ShowBoard(current_board);
    }
    if(get_relative_depth()<MAX_DEPTH){
    //in tree phase
        stack<Coordinate> options;
        RankOptions(options);
        Coordinate opt_move;
        Coordinate move;
        while(!options.empty()){
            move = options.top();
            options.pop();
            string result = TryMove(move);
            if(result=="illegal"){
                continue;
            }
            if(isMaxNode()){
                if(result=="terminated"){
                    CancelTry();
                    //ShowBoard();
                    return LARGEST_NUMBER;
                }else{
                    int new_alpha = ABSearch(alpha, beta);
                    //keep track of optimal move and ab
                    if(new_alpha>alpha){
                        alpha = new_alpha;
                        opt_move = move;
                    }
                    //prunning
                    if(alpha >= beta){
                        cout<<"prunning happens"<<endl;
                        CancelTry();
                        return alpha;
                    }
                }
            }else{
                if(result=="terminated"){
                    CancelTry();
                    //ShowBoard();
                    return -LARGEST_NUMBER;
                }else{
                    int new_beta = ABSearch(alpha, beta);
                    if(new_beta<beta){
                        beta = new_beta;
                        opt_move = move;
                    }
                    if(alpha>=beta){
                        cout<<"prunning happens"<<endl;
                        CancelTry();
                        return beta;
                    }
                }
            }
            CancelTry();
        }
        if(isMaxNode()){
            return alpha;
        }else{
            return beta;
        }
    }else{
    //at the leaf
        if(isMaxNode()){
            return EstimateState();
        }else{
            return -EstimateState();
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

/* Implementation Note:
 * The state is estimated from the perspective of
 * current player. The larger the score, the better
 * the situation is for the player represented by
 * whose_turn.
 */

int GameAI::EstimateState(){
    if(state_values.find(cur_hash)!=state_values.end()){
        return state_values[cur_hash];
    }else{
        /*
         * check for win
         * priority:
         * my five move
         * you five move
         * my four move
         * my four+three move
         * your four move
         * your four+three move
         * your half four move
         * my double three move
         */
        stack<Coordinate> my_five_moves;
        FindMoves(my_five_moves, whose_turn, &have_five_at);
        if(!my_five_moves.empty()){
            //ShowBoard(current_board);
            return LARGEST_NUMBER;
        }
        stack<Coordinate> your_five_moves;
        FindMoves(your_five_moves, ReverseColor(whose_turn), &have_five_at);
        if((int)your_five_moves.size()>1){
            return -LARGEST_NUMBER;
        }else if((int)your_five_moves.size()==1){
        }else{
            stack<Coordinate> my_four_moves;
            FindMoves(my_four_moves, whose_turn, &have_active_four_at);
            if(!my_four_moves.empty()){
                //ShowBoard(current_board);
                return LARGEST_NUMBER;
            }
        }
        vector<vector<int>> my_active_three_chart(BOARDSIZE.x);
        vector<vector<int>> my_half_four_chart(BOARDSIZE.x);
        for(int i=0;i<BOARDSIZE.x;i++){
            my_active_three_chart[i].resize(BOARDSIZE.y);
            my_half_four_chart[i].resize(BOARDSIZE.y);
            for(int j=0;j<BOARDSIZE.y;j++){
                if(current_board[i][j]!=REPRESENTATION.at("empty")){
                    my_active_three_chart[i][j] = 0;
                    my_half_four_chart[i][j] = 0;
                    continue;
                }
                current_board[i][j] = REPRESENTATION.at(whose_turn);
                my_active_three_chart[i][j] = num_of_active_three_at(current_board, whose_turn, i,j);
                my_half_four_chart[i][j] = num_of_half_four_at(current_board, whose_turn, i, j);
                current_board[i][j] = REPRESENTATION.at("empty");
            }
        }
        bool winByDouble=false;
        for(int i=0;i<BOARDSIZE.x;i++){
            for(int j=0;j<BOARDSIZE.y;j++){
               if((my_active_three_chart[i][j]>0)&&(my_half_four_chart[i][j]>0)){
                   winByDouble = true;
                   break;
               }else if(my_half_four_chart[i][j]>1){
                   winByDouble = true;
                   break;
               }
            }
            if(winByDouble){
               break;
            }
        }
        if(winByDouble){
            //ShowBoard(current_board);
            return LARGEST_NUMBER;
        }
        bool winByDoubleThree = false;
        stack<Coordinate> your_four_moves;
        stack<Coordinate> your_half_four_moves;
        FindMoves(your_four_moves, ReverseColor(whose_turn), &have_active_four_at);
        FindMoves(your_half_four_moves, ReverseColor(whose_turn), &have_half_four_at);
        if(((int)your_four_moves.size()>0)||((int)your_half_four_moves.size())>0){
        }else{
            for(int i=0;i<BOARDSIZE.x;i++){
                for(int j=0;j<BOARDSIZE.y;j++){
                    if(my_active_three_chart[i][j]>1){
                        winByDoubleThree = true;
                        break;
                    }
                }
                if(winByDoubleThree){
                    break;
                }
            }
        }
        if(winByDoubleThree){
            return LARGEST_NUMBER;
        }
        /*
         * counting:
         * your five move
         * your four move
         * my half four move
         * your half four move
         * my active three move
         * your active three move
         */
        stack<Coordinate> your_active_three_moves;
        FindMoves(your_active_three_moves, ReverseColor(whose_turn), &have_active_three_at);
        int score = 0;
        score += YOUR_FIVE_MOVE_SCORE*(int)your_five_moves.size();
        score += YOUR_FOUR_MOVE_SCORE*(int)your_four_moves.size();
        score += YOUR_HALF_FOUR_MOVE_SCORE*(int)your_half_four_moves.size();
        score += YOUR_ACTIVE_THREE_MOVE_SCORE*(int)your_active_three_moves.size();
        for(int i=0;i<BOARDSIZE.x;i++){
            for(int j=0;j<BOARDSIZE.y;j++){
                score += MY_HALF_FOUR_MOVE_SCORE*my_half_four_chart[i][j];
                score += MY_ACTIVE_THREE_MOVE_SCORE*my_active_three_chart[i][j];
            }
        }
        /*
        if(score!=0){
            cout<<endl;
            cout<<score<<endl;
            ShowBoard(current_board);
            cout<<"my half four"<<endl;
            ShowBoard(my_half_four_chart,"number");
            cout<<"my active four"<<endl;
            ShowBoard(my_active_three_chart,"number");
            cout<<"your five moves "<<your_five_moves.size()<<endl;
            cout<<"your four moves "<<your_four_moves.size()<<endl;
            cout<<"your half four moves "<<your_half_four_moves.size()<<endl;
            cout<<"you active three moves "<<your_active_three_moves.size()<<endl;
            cout<<endl;
            cout<<"site value"<<endl;
            ShowBoard(siteValue, "number");
        }
        */
        return score;
    }
}

//watch out for change of board
void GameAI::FindMoves(stack<Coordinate> & moves, string color,
                       bool (*finder)(const vector<vector<int>>&, string, Coordinate)){
    for(int i=0;i<BOARDSIZE.x;i++){
        for(int j=0;j<BOARDSIZE.y;j++){
            if(current_board[i][j]==REPRESENTATION.at("empty")){
                current_board[i][j] = REPRESENTATION.at(color);
                if(finder(current_board, color, Coordinate(i,j))){
                    moves.push(Coordinate(i,j));
                }
                current_board[i][j] = REPRESENTATION.at("empty");
            }
        }
    }
}
void GameAI::FindKillingMoves(stack<Coordinate> &killing_moves, string color){
    FindMoves(killing_moves, color, &have_five_at);
    FindMoves(killing_moves, color, &have_active_four_at);
}
void GameAI::FindHalfFourMoves(stack<Coordinate> &half_four_moves, string color){
    FindMoves(half_four_moves, color, &have_half_four_at);
}
void GameAI::FindActiveThreeMoves(stack<Coordinate> &active_three_moves, string color){
    FindMoves(active_three_moves, color, &have_active_three_at);
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
        RespondVCT(canDefend,  "four");
        CancelTry();
        if(!canDefend){
            canWin = true;
            vic_move = move;
            break;
        }
    }
    if(canWin){
        return;
    }
    while(!active_three_moves.empty()){
        Coordinate move = active_three_moves.top();
        active_three_moves.pop();
        TryMove(move);
        bool canDefend;
        RespondVCT(canDefend, "three");
        CancelTry();
        if(!canDefend){
            canWin = true;
            vic_move = move;
            break;
        }
    }
    return;
}

void GameAI::CalculateVCT(bool &canWin){
    Coordinate rubbish;
    CalculateVCT(canWin, rubbish);

}

void GameAI::RespondVCT(bool &canDefend, string type){
    /*
     * if last move is passed, some time might be saved?
     */
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
        CancelTry();
        if(!opcanWin){
            break;
        }
    }
    canDefend = !opcanWin;
    return;
}


int GameAI::chart(int grade){
    int temp = 0;
    if(whose_turn!=my_color){//?
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
    else if(whose_turn==my_color){//?
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
                    grade+=current_board[i][j+n]+4;
                }
                fiveValue[i][j].y=chart(grade);
            }
            grade=0;             //y
            if(i<BOARDSIZE.x-4){
                for(int n=0;n<5;n++){
                    grade+=current_board[i+n][j]+4;
                }
                fiveValue[i][j].x=chart(grade);
            }
             grade=0;            //r
            if(i<BOARDSIZE.x-4&&j<BOARDSIZE.y-4){
                for(int n=0;n<5;n++){
                    grade+=current_board[i+n][j+n]+4;
                }
                fiveValue[i][j].r=chart(grade);
            }
            grade=0;           //l
            if(j>=4&&i<BOARDSIZE.x-4){
                for(int n=0;n<5;n++){
                    grade+=current_board[i+n][j-n]+4;
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
                if((i-n>=0)&&(j-n>=0))
                    siteValue[i][j]+=fiveValue[i-n][j-n].r;
                if((i-n>=0)&&(j+n<=BOARDSIZE.y))
                    siteValue[i][j]+=fiveValue[i-n][j+n].l;
            }
            siteValue[i][j] = siteValue[i][j]/10000;//no need for division
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

