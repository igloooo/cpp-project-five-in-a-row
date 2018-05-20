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
#include <stdlib.h>
#include <assert.h>
#include <algorithm>
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

    for(int i=0;i<BOARDSIZEX;i++){
        for(int j=0;j<BOARDSIZEY;j++){
            fiveValue[i][j] = {0,0,0,0};
        }
    }
    for(int i=0;i<BOARDSIZEX;i++){
        for(int j=0;j<BOARDSIZEY;j++){
            siteValue[i][j] = 0;
        }
    }
}

void GameAI::Update(GameModel &game){
    whose_turn = game.get_whose_turn();
    whose_turn_repre = REPRESENTATION.at(whose_turn);
    opponent_repre = REPRESENTATION.at(ReverseColor(whose_turn));
    terminated = game.isTerminated();
    winner = game.get_winner();
    round = game.get_round();
    current_board.resize(BOARDSIZEX);
    for(int i=0;i<BOARDSIZEX;i++){
        current_board[i].resize(BOARDSIZEY);
        for(int j=0;j<BOARDSIZEY;j++){
            current_board[i][j] = REPRESENTATION.at(game.at(i,j).color);
            current_board2[i][j] = current_board[i][j]*((current_board[i][j]>0)?4:5);//black, empty, white, 4, 0, 5
        }
    }
    history_moves.clear();
    for(int i=0; i<game.get_steps(); i++){
        history_moves.push_back(game.get_history_move(i));
        UpdateHash(history_moves[i]);
        /*
        for(int x=-AVAILABLE_NEIGHBORHOOD;x<=AVAILABLE_NEIGHBORHOOD;x++){
            for(int y=-AVAILABLE_NEIGHBORHOOD;y<=AVAILABLE_NEIGHBORHOOD;y++){
                availability[history_moves.x+x][history_moves.y+y] = 1;
            }
        }
        */
    }
    num_of_empty_places = game.get_num_of_empty_places();
    game_depth = game.get_steps();
    state_values.clear();//since searching depth is changed, state values are cleared in each step

    InitializeStateEstimation();
}

Coordinate GameAI::quick_computer_move(){
    queue<Coordinate> options;
    RankOptions(options);
    Coordinate move = options.front();
    while(CheckRule(move)=="illegal"){
        options.pop();
        move = options.front();
    }
    return move;
}

Coordinate GameAI::computer_move(){
    //see if vct exists
    bool canWin = false;
    Coordinate VCT_move = CalculateVCT(canWin, OUTTREE_VCT_DEPTH, true);
    if(canWin){
        //for(int i=0;i<(int)main_variation.size();i++){
        //    Stone cur = main_variation[i];
        //    cout<<cur.x<<","<<cur.y<<cur.color<<endl;
        //}
        return VCT_move;
    }
    /*
    //do ab search, direct winning has been excluded.
    queue<Coordinate> options;
    RankOptions(options);
    Coordinate move;//the move to try
    Coordinate opt_move;
    int alpha = -LARGEST_NUMBER-1;
    int beta = LARGEST_NUMBER + 1;
    while(!options.empty()){
        move = options.front();
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
    */
    Coordinate opt_move = quick_computer_move();

    //see if opponent's vct exist
    canWin = false;
    TryMove(opt_move);
    VCT_move = CalculateVCT(canWin, OUTTREE_VCT_DEPTH, true);
    CancelTry();
    if(canWin){
        cout<<"dangerous!"<<endl;
        //for(int i=0;i<(int)main_variation.size();i++){
        //    Stone cur = main_variation[i];
        //    cout<<cur.x<<","<<cur.y<<cur.color<<endl;
        //}
        return VCT_move;
    }else{
        return opt_move;
    }
}

int GameAI::ABSearch(int alpha, int beta){
    global_step += 1;
    //cout<<get_relative_depth()<<endl;

    if(global_step%9999==0){
        cout<<global_step<<endl;
        cout<<get_relative_depth()<<endl;
        cout<<alpha<<","<<beta<<endl;
        ShowBoard(current_board);
        //ShowBoard(black_active_twos,"number");
    }

    /*
    if(get_relative_depth()==5){
        cout<<alpha<<","<<beta<<endl;
    }
    */
    if(get_relative_depth()<MAX_DEPTH){
    //in tree phase
        queue<Coordinate> options;
        RankOptions(options);
        Coordinate opt_move;
        Coordinate move;
        while(!options.empty()){
            move = options.front();
            options.pop();
            if(isMaxNode()){
                string result = TryMove(move);
                if(result=="illegal"){
                    continue;
                }else if(result=="terminated"){
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
                        //cout<<"prunning happens"<<endl;
                        CancelTry();
                        return alpha;
                    }
                }
            }else{
                string result = TryMove(move);
                if(result=="illegal"){
                    continue;
                }else if(result=="terminated"){
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
                        //cout<<"prunning happens"<<endl;
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
    }else if(get_relative_depth()==MAX_DEPTH-1){
        bool canWin=false;
        CalculateVCT(canWin, INTREE_VCT_DEPTH);
        if(canWin){
            if(whose_turn==my_color){
                return LARGEST_NUMBER;
            }else{
                return -LARGEST_NUMBER;
            }
        }
    }else{
    //at the leaf
        if(my_color=="black"){
            int score = EstimateState();
            //cout<<score<<endl;
            return score;
        }else{
            int score = -EstimateState();
            //cout<<score<<endl;
            return score;
        }
    }
}

string GameAI::TryMove(Coordinate move){
    //the order cannot be changed.
    string result = TakeMove(move);
    if(result=="illegal"){
        return result;
    }

    current_board2[move.x][move.y] = (whose_turn_repre>0)?4:5;
    whose_turn_repre = REPRESENTATION.at(whose_turn);
    opponent_repre = REPRESENTATION.at(ReverseColor(whose_turn));

    FitTemplates(GetVector(move.y, "x"), 15, move.y, "x", false, move.x, EMPTY_R);
    FitTemplates(GetVector(move.x, "y"), 15, move.x, "y", false, move.y, EMPTY_R);
    FitTemplates(GetVector(move.x-move.y, "d"), 15-abs(move.x-move.y), move.x-move.y, "d", false, min(move.x, move.y), EMPTY_R);
    FitTemplates(GetVector(14-(move.x+move.y), "a"), 15-abs(14-(move.x+move.y)), 14-(move.x+move.y), "a", false, min(14-move.x, move.y), EMPTY_R);
    //ShowBoard(current_board);
    UpdateHash(get_last_move());
    return result;
}

void GameAI::CancelTry(){//doesn't capture the error of cancel in empty board, but that doesn't matter
    //the order cannot be changed.
    RedoHash();

    Stone move = get_last_move();
    current_board2[move.x][move.y] = 0;

    CancelLastMove();
    whose_turn_repre = REPRESENTATION.at(whose_turn);
    opponent_repre = REPRESENTATION.at(ReverseColor(whose_turn));

    FitTemplates(GetVector(move.y, "x"), 15, move.y, "x", false, move.x, whose_turn_repre);
    FitTemplates(GetVector(move.x, "y"), 15, move.x, "y", false, move.y, whose_turn_repre);
    FitTemplates(GetVector(move.x-move.y, "d"), 15-abs(move.x-move.y), move.x-move.y, "d", false, min(move.x, move.y), whose_turn_repre);
    FitTemplates(GetVector(14-(move.x+move.y), "a"), 15-abs(14-(move.x+move.y)), 14-(move.x+move.y), "a", false, min(14-move.x, move.y), whose_turn_repre);
    //ShowBoard(current_board);
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
    srand((unsigned)time(0));
    for(string &color:colors){
        for(int i=0;i<BOARDSIZEX;i++){
        for(int j=0;j<BOARDSIZEY;j++){
            for(int k=0;k<HASH_SIZE;k++){
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

void GameAI::RankOptions(queue<Coordinate> &options){
    set_five_value();
    set_site_value();
    int len = BOARDSIZEX*BOARDSIZEY;
    vector<int> keys(len);
    vector<Coordinate> values(len);
    for(int i=0;i<BOARDSIZEX;i++){
    for(int j=0;j<BOARDSIZEY;j++){
        keys[i*BOARDSIZEX+j] = siteValue[i][j];
        values[i*BOARDSIZEX+j] = Coordinate(i,j);
    }
    }
    sort(keys, values, 0, len);
    int n = 0;
    for(int i=len;i>0;i--){
        if(n>=10){
            break;
        }
        if(CheckRule(values[i])!="illegal"){
            options.push(values[i]);
            n += 1;
        }
    }
}

/*
 * Implementation Note:
 * The state is estimated based on color. The
 * larger the color, the more advantage black
 * has.
 */

int GameAI::EstimateState(){
    if(state_values.find(cur_hash)!=state_values.end()){
        return state_values[cur_hash];
    }else{
        int static_score = 0;
        if(whose_turn=="black"){
            /*
             * check for win (might not be necessary if VCT is present)
             * priority:
             * my five or four
             * your five or four
             * my double
             * your half four
             * my double three
             */
            if(num_of_black_wins||num_of_black_half_fours){
            //existence of four or half four
                state_values[cur_hash] = LARGEST_NUMBER;
                return LARGEST_NUMBER;
            }else if(num_of_white_wins){
            //existence of opponent's four
                state_values[cur_hash] = -LARGEST_NUMBER;
                return -LARGEST_NUMBER;
            }else if(num_of_black_active_threes){
                state_values[cur_hash] = LARGEST_NUMBER;
                return LARGEST_NUMBER;
            }else{

              bool canWin = false;
              CalculateVCT(canWin, INTREE_VCT_DEPTH);
              if(canWin){
                  state_values[cur_hash] = LARGEST_NUMBER;
                  return LARGEST_NUMBER;
              }

                /*
                for(int i=0;i<15;i++){
                    for(int j=0;j<15;j++){
                        if(black_half_threes[i][j]>1){
                            state_values[cur_hash] = LARGEST_NUMBER;
                            return LARGEST_NUMBER;
                        }else if(black_half_threes[i][j]&&black_active_twos[i][j]){
                            state_values[cur_hash] = LARGEST_NUMBER;
                            return LARGEST_NUMBER;
                        }
                    }
                }
                if(num_of_white_half_fours==0){
                    for(int i=0;i<15;i++){
                        for(int j=0;i<15;j++){
                            if(black_active_twos[i][j]>1){
                                state_values[cur_hash] = LARGEST_NUMBER;
                                return LARGEST_NUMBER;
                            }
                        }
                    }
                }
                */
                //score counting
                static_score += SCORE_FOR_MY_HALF_THREE*num_of_black_half_threes;
                static_score += SCORE_FOR_MY_ACTIVE_TWO*num_of_black_active_twos;
                static_score += SCORE_FOR_YOUR_HALF_FOUR*num_of_white_half_fours;
                static_score += SCORE_FOR_YOUR_ACTIVE_THREE*num_of_white_active_threes;
                static_score += SCORE_FOR_YOUR_HALF_THREE*num_of_white_half_threes;
                static_score += SCORE_FOR_YOUR_ACTIVE_TWO*num_of_white_active_twos;
                state_values[cur_hash] = static_score;
                return static_score;
            }
        }
        if(whose_turn=="white"){
            /*
             * check for win (might not be necessary if VCT is present)
             * priority:
             * my five or four
             * your five or four
             * my double
             * your half four
             * my double three
             */
            if(num_of_white_wins||num_of_white_half_fours){
            //existence of four or half four
                state_values[cur_hash] = -LARGEST_NUMBER;
                return -LARGEST_NUMBER;
            }else if(num_of_black_wins){
            //existence of opponent's four
                state_values[cur_hash] = LARGEST_NUMBER;
                return LARGEST_NUMBER;
            }else if(num_of_white_active_threes){
                state_values[cur_hash] = -LARGEST_NUMBER;
                return -LARGEST_NUMBER;
            }else{

                bool canWin=false;
                CalculateVCT(canWin, INTREE_VCT_DEPTH);
                if(canWin){
                    state_values[cur_hash] = -LARGEST_NUMBER;
                    return -LARGEST_NUMBER;
                }

                /*
                for(int i=0;i<15;i++){
                    for(int j=0;j<15;j++){
                        if(white_half_threes[i][j]>1){
                            state_values[cur_hash] = -LARGEST_NUMBER;
                            return -LARGEST_NUMBER;
                        }else if(white_half_threes[i][j]&&white_active_twos[i][j]){
                            state_values[cur_hash] = -LARGEST_NUMBER;
                            return -LARGEST_NUMBER;
                        }
                    }
                }
                if(num_of_black_half_fours==0){
                    for(int i=0;i<15;i++){
                        for(int j=0;i<15;j++){
                            if(white_active_twos[i][j]>1){
                                state_values[cur_hash] = -LARGEST_NUMBER;
                                return -LARGEST_NUMBER;
                            }
                        }
                    }
                }
                */
                //score counting
                static_score -= SCORE_FOR_MY_HALF_THREE*num_of_white_half_threes;
                static_score -= SCORE_FOR_MY_ACTIVE_TWO*num_of_white_active_twos;
                static_score -= SCORE_FOR_YOUR_HALF_FOUR*num_of_black_half_fours;
                static_score -= SCORE_FOR_YOUR_ACTIVE_THREE*num_of_black_active_threes;
                static_score -= SCORE_FOR_YOUR_HALF_THREE*num_of_black_half_threes;
                static_score -= SCORE_FOR_YOUR_ACTIVE_TWO*num_of_black_active_twos;
                state_values[cur_hash] = static_score;
                return static_score;
            }
        }
    }
}

//watch out for change of board
void GameAI::FindMoves(stack<Coordinate> & moves, string color,
                       bool (*finder)(const vector<vector<int>>&, string, Coordinate)){
    for(int i=0;i<BOARDSIZEX;i++){
        for(int j=0;j<BOARDSIZEY;j++){
            if(current_board[i][j]==EMPTY_R){
                current_board[i][j] = REPRESENTATION.at(color);
                if(finder(current_board, color, Coordinate(i,j))){
                    moves.push(Coordinate(i,j));
                }
                current_board[i][j] = EMPTY_R;
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

Coordinate GameAI::CalculateVCT(bool &canWin, int max_depth, bool provide_move){
    /*
     * Implementation note:
     * Assume that opponents have no five or four
     * Assume that current player has no five as
     * well.
     * They are guarenteed by the implementation
     * of EstimateState.
     *
     */
     //cout<<"calculating vct..."<<endl;
     //ShowBoard(current_board);
     if(get_relative_depth()>max_depth){
         ShowBoard(current_board);
         canWin = false;
         return Coordinate(-1,-1);
     }
     if(whose_turn_repre==BLACK_R){
         if(num_of_black_wins>0){
             canWin = true;
             VariationRecorder1();
             if(provide_move){
             //if there is request for moves
                 for(int i=0;i<BOARDSIZEX;i++){
                 for(int j=0;j<BOARDSIZEY;j++){
                     if(black_fours[i][j]>0){
                         return Coordinate(i,j);
                     }
                 }
                 }
             }else{
                 return Coordinate(-1,-1);
             }
         }else if(num_of_black_half_fours>0){
             canWin = true;
             VariationRecorder1();
             if(provide_move){
                 for(int i=0;i<BOARDSIZEX;i++){
                 for(int j=0;j<BOARDSIZEY;j++){
                     if(black_half_fours[i][j]>0){
                         return Coordinate(i,j);
                     }
                 }
                 }
             }else{
                 return Coordinate(-1,-1);
             }
         }else if(num_of_white_wins){
         //give up if opponent has four
             canWin = false;
             return Coordinate(-1,-1);
         }else if(num_of_black_active_threes*num_of_white_half_fours>0){
             bool canDefend = true;
             for(int i=0;i<BOARDSIZEX;i++){
             for(int j=0;j<BOARDSIZEY;j++){
                 if(white_half_fours[i][j]>0){
                     TryMove(Coordinate(i,j));
                     RespondVCT(canDefend, max_depth);
                     if(!canDefend){
                         VariationRecorder1();
                     }else{
                         VariationRecorder2();
                     }
                     CancelTry();
                     if(!canDefend){
                         canWin = true;
                         return Coordinate(i,j);
                     }
                 }
             }
             }
             canWin = false;
             return Coordinate(-1,-1);
         }else if(num_of_black_active_threes-num_of_white_half_fours<0){
             canWin = false;
             return Coordinate(-1,-1);
         }else if(num_of_black_active_threes-num_of_white_half_fours>0){
             canWin = true;
             VariationRecorder1();
             if(provide_move){
                 for(int i=0;i<BOARDSIZEX;i++){
                 for(int j=0;j<BOARDSIZEY;j++){
                     if(black_active_threes[i][j]>0){
                         return Coordinate(i,j);
                     }
                 }
                 }
             }else{
                 return Coordinate(-1,-1);
             }
         }

         //try to generate half four
         if(num_of_black_half_threes>0){
             bool canDefend = true;
             for(int i=0;i<15;i++){
             for(int j=0;j<15;j++){
                 if(black_half_threes[i][j]>0){
                     Coordinate cur_move = Coordinate(i,j);
                     assert(TryMove(cur_move)=="continuing");
                     RespondVCT(canDefend, max_depth);
                     if(!canDefend){
                         VariationRecorder1();
                     }else{
                         VariationRecorder2();
                     }
                     CancelTry();
                     if(!canDefend){
                         canWin = true;
                         return cur_move;
                     }
                 }
             }
             }
         }
         //try to generate active three
         if(num_of_white_active_threes){
         //prunning when opponent already has active three while you
         //are just going to produce a three
         //when canWin==false, no need to assign vic_move
             canWin = false;
             return Coordinate(-1,-1);
         }else if(num_of_black_active_twos>0){
             bool canDefend = true;
             for(int i=0;i<15;i++){
             for(int j=0;j<15;j++){
                 if(black_active_twos[i][j]>0){
                     Coordinate cur_move = Coordinate(i,j);
                     assert(TryMove(cur_move)=="continuing");
                     RespondVCT(canDefend, max_depth);
                     if(!canDefend){
                         VariationRecorder1();
                     }else{
                         VariationRecorder2();
                     }
                     CancelTry();
                     if(!canDefend){
                         canWin = true;
                         return cur_move;
                     }
                 }
             }
             }
         }else{
             canWin = false;
             return Coordinate(-1,-1);
         }
     }else{
     //this part is reverse of last part
         if(num_of_white_wins>0){
             canWin = true;
             VariationRecorder1();
             if(provide_move){
             //if there is request for moves
                 for(int i=0;i<BOARDSIZEX;i++){
                 for(int j=0;j<BOARDSIZEY;j++){
                     if(white_fours[i][j]>0){
                         return Coordinate(i,j);
                     }
                 }
                 }
             }else{
                 return Coordinate(-1,-1);
             }
         }else if(num_of_white_half_fours>0){
             canWin = true;
             VariationRecorder1();
             if(provide_move){
                 for(int i=0;i<BOARDSIZEX;i++){
                 for(int j=0;j<BOARDSIZEY;j++){
                     if(white_half_fours[i][j]>0){
                         return Coordinate(i,j);
                     }
                 }
                 }
             }else{
                 return Coordinate(-1,-1);
             }
         }else if(num_of_black_wins>0){
             canWin = false;
             return Coordinate(-1,-1);
         }else if(num_of_white_active_threes*num_of_black_half_fours>0){
             bool canDefend = true;
             for(int i=0;i<BOARDSIZEX;i++){
             for(int j=0;j<BOARDSIZEY;j++){
                 if(black_half_fours[i][j]>0){
                     TryMove(Coordinate(i,j));
                     RespondVCT(canDefend, max_depth);
                     if(!canDefend){
                         VariationRecorder1();
                     }else{
                         VariationRecorder2();
                     }
                     CancelTry();
                     if(!canDefend){
                         canWin = true;
                         return Coordinate(i,j);
                     }
                 }
             }
             }
             canWin = false;
             return Coordinate(-1,-1);
         }else if(num_of_white_active_threes-num_of_black_half_fours<0){
             canWin = false;
             return Coordinate(-1,-1);
         }else if(num_of_white_active_threes-num_of_black_half_fours>0){
             canWin = true;
             VariationRecorder1();
             if(provide_move){
                 for(int i=0;i<BOARDSIZEX;i++){
                 for(int j=0;j<BOARDSIZEY;j++){
                     if(white_active_threes[i][j]>0){
                         return Coordinate(i,j);
                     }
                 }
                 }
             }else{
                 return Coordinate(-1,-1);
             }
         }

         if(num_of_white_half_threes>0){
             bool canDefend = true;
             for(int i=0;i<15;i++){
             for(int j=0;j<15;j++){
                 if(white_half_threes[i][j]>0){
                     Coordinate cur_move = Coordinate(i,j);
                     assert(TryMove(cur_move)=="continuing");
                     RespondVCT(canDefend, max_depth);
                     if(!canDefend){
                         VariationRecorder1();
                     }else{
                         VariationRecorder2();
                     }
                     CancelTry();
                     if(!canDefend){
                         canWin = true;
                         return cur_move;
                     }
                 }
             }
             }
         }
         if(num_of_black_active_threes){
             canWin = false;
             return Coordinate(-1,-1);
         }else if(num_of_white_active_twos>0){
             bool canDefend = true;
             for(int i=0;i<15;i++){
             for(int j=0;j<15;j++){
                 if(white_active_twos[i][j]>0){
                     Coordinate cur_move = Coordinate(i,j);
                     assert(TryMove(cur_move)=="continuing");
                     RespondVCT(canDefend, max_depth);
                     if(!canDefend){
                         VariationRecorder1();
                     }else{
                         VariationRecorder2();
                     }
                     CancelTry();
                     if(!canDefend){
                         canWin = true;
                         return cur_move;
                     }
                 }
             }
             }    
         }else{
             canWin = false;
             return Coordinate(-1,-1);
         }
     }
}

void GameAI::RespondVCT(bool &canDefend, int max_depth){
    /*
     * Implementation note:
     * Assume it has no five, four, or half four.
     * The strategy for defending could be either
     * to block or to produce half four (active three
     * is a bit too slow)
     */
     if(whose_turn_repre==BLACK_R){
         canDefend = false;
         bool canWin = true;
         if(num_of_white_wins>0){
             return;
         }else if(num_of_white_half_fours>0){
         //if opponent has half fours, then effective response must be to block it, if any
             for(int i=0;i<15;i++){
             for(int j=0;j<15;j++){
                 if(white_half_fours[i][j]>0){
                     VariationRecorder2();
                     TryMove(Coordinate(i,j));
                     CalculateVCT(canWin, max_depth);
                     CancelTry();
                     if(!canWin){
                     //if there is a way when he cannot win, then you can defend successfully!
                         canDefend = true;
                         return;
                     }
                 }
             }
             }
             canDefend = false;
             return;
         }else if(num_of_white_active_threes>0){
         //if opponent has active threes, then effective response could
         //be to block it or to produce half four
             for(int i=0;i<15;i++){
             for(int j=0;j<15;j++){
                 if(black_active_threes[i][j]|black_half_threes[i][j]|white_active_threes[i][j]>0){
                 //to retaliate or to block
                     VariationRecorder2();
                     TryMove(Coordinate(i,j));
                     CalculateVCT(canWin, max_depth);
                     CancelTry();
                     if(!canWin){
                         canDefend = true;
                         return;
                     }
                 }
             }
             }
             canDefend = false;
             return;
         }else{
             //This case is due to the imcompleteness of the assessment of VCT
             canDefend = true;
             return;
         }
     }
     if(whose_turn_repre==WHITE_R){
         canDefend = false;
         bool canWin = true;
         if(num_of_black_wins>0){
             return;
         }else if(num_of_black_half_fours>0){
             for(int i=0;i<15;i++){
             for(int j=0;j<15;j++){
                 if(black_half_fours[i][j]>0){
                     VariationRecorder2();
                     TryMove(Coordinate(i,j));
                     CalculateVCT(canWin, max_depth);
                     CancelTry();
                     if(!canWin){
                         canDefend = true;
                         return;
                     }
                 }
             }
             }
             canDefend = false;
             return;
         }else if(num_of_black_active_threes>0){
             for(int i=0;i<15;i++){
             for(int j=0;j<15;j++){
                 if(white_active_threes[i][j]|white_half_threes[i][j]|black_active_threes[i][j]>0){
                     VariationRecorder2();
                     TryMove(Coordinate(i,j));
                     CalculateVCT(canWin, max_depth);
                     CancelTry();
                     if(!canWin){
                         canDefend = true;
                         return;
                     }
                 }
             }
             }
             canDefend = false;
             return;
         }else{
             canDefend = true;
             return;
         }
     }
}


int GameAI::chart(int grade){
    int temp = 0;
    if(whose_turn_repre==BLACK_R){
        switch(grade){
           case 0: temp=3;break;
           case 4: temp=35;break;//one black stone in the direction
           case 8: temp=800;break;
           case 12: temp=15000;break;
           case 16: temp=800000;break;
           case 5: temp=15;break;//one white stone in the direction
           case 10: temp=400;break;
           case 15: temp=1800;break;
           case 20: temp=100000;break;
           default: temp=0;
        }
     }
    else if(whose_turn_repre==WHITE_R){
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
    for(int i=0;i<BOARDSIZEX;i++){
        for(int j=0;j<BOARDSIZEY;j++){
            int grade=0;          //y
            if(j<BOARDSIZEY-4){
                for(int n=0;n<5;n++){
                    grade+=current_board2[i][j+n];//4 if black, 5 if white, 0 if empty
                }
                fiveValue[i][j].y=chart(grade);
            }
            grade=0;             //x
            if(i<BOARDSIZEX-4){
                for(int n=0;n<5;n++){
                    grade+=current_board2[i+n][j];
                }
                fiveValue[i][j].x=chart(grade);
            }
             grade=0;            //r
            if(i<BOARDSIZEX-4&&j<BOARDSIZEY-4){
                for(int n=0;n<5;n++){
                    grade+=current_board2[i+n][j+n];
                }
                fiveValue[i][j].r=chart(grade);
            }
            grade=0;           //l
            if(j>=4&&i<BOARDSIZEX-4){
                for(int n=0;n<5;n++){
                    grade+=current_board2[i+n][j-n];
                }
                fiveValue[i][j].l=chart(grade);
            }
        }
    }
}
void GameAI::set_site_value()
{
    for(int i=0;i<BOARDSIZEX;i++){
        for(int j=0;j<BOARDSIZEY;j++)
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
                if((i-n>=0)&&(j+n<=BOARDSIZEY))
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
            return;
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
            }else if(r_ind==t){
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

void GameAI::PrintVariation(){
    int cur_steps = get_steps();
    for(int n=cur_steps;n>game_depth;n--){
        Stone move = history_moves[n];
        cout<<move.x<<","<<move.y<<" "<<move.color<<endl;
    }
}

void GameAI::VariationRecorder1(){

    for(int k=game_depth;k<get_steps();k++){
        main_variation.push_back(history_moves[k]);
    }
}

void GameAI::VariationRecorder2(){
    main_variation.clear();
}

/*
void GameAI::ReverseBoard(){
    for(int i=0;i<BOARDSIZEX;i++){
        for(int j=0;j<BOARDSIZEY;j++){
            current_board[i][j] = -current_board[i][j];
        }
    }
}
*/

string GameAI::toString(){
    string str;
    str += string("--------------round") + to_string(round) + " " + (isFirstPlayer(whose_turn)?"first hand":"second hand") + "-------------\n";
    str += "-------------game info----------------\n";
    str += string("FIRST_PLAYER ") + ((FIRST_PLAYER==HUMAN)?"HUMAN":"COMPUTER") + "\n";
    str += string("SECOND_PLAYER ") + ((SECOND_PLAYER==HUMAN)?"HUMAN":"COMPUTER") + "\n";
    str += string("whose_turn ") + whose_turn + "\n";
    str += "whose_turn_repre: " + to_string(whose_turn_repre) + "\n";
    str += "opponent_repre: " + to_string(opponent_repre) + "\n";
    str += string("current player type ") + ((get_cur_player()==HUMAN)?"HUMAN":"COMPUTER") + "\n";
    str += string("terminated ") + (isTerminated()?"true":"false") + "\n";
    str += string("winner ") + winner + "\n";
    str += string("num_of_empty_places ") + to_string(get_num_of_empty_places()) + "\n";
    str += "current steps: " + to_string(get_steps()) + "\n";
    str += "-------------ai info------------------\n";
    str += string("MAX_DEPTH ") + to_string(MAX_DEPTH) + "\n";
    str += string("my_color ") + my_color + "\n";
    str += string("game_depth ") + to_string(game_depth) + "\n";
    str += "cur_hash ";
    for(int i=0;i<HASH_SIZE;i++){
        str += to_string(cur_hash[i]);
    }
    str += "\n";
    return str;
}

ostream & operator << (ostream & os, GameAI & ai){
    return os<<ai.toString();
}

struct GameAI::TwoBitStrings GameAI::GetVector(int index, string direc){
    int black_vec = 0;
    int white_vec = 0;
    if(direc=="x"){
        for(int k=0; k<15;k++){
            int cur = current_board[k][index];
            if(cur==BLACK_R){
                black_vec += 1<<k;
            }else if(cur==WHITE_R){
                white_vec += 1<<k;
            }
        }
        //black_vecs[direc][index] = black_vec;
        //white_vecs[direc][index] = white_vec;
    }else if(direc == "y"){
        for(int k=0; k<15; k++){
            int cur = current_board[index][k];
            if(cur==BLACK_R){
                black_vec += 1<<k;
            }else if(cur==WHITE_R){
                white_vec += 1<<k;
            }
        }
        //black_vecs[direc][index] = black_vec;
        //white_vecs[direc][index] = white_vec;
    }else if(direc == "d"){
        if(index>=0){
            int range = 15-index;
            for(int k=0; k<range; k++){
                int cur = current_board[index+k][k];
                if(cur==BLACK_R){
                    black_vec += 1<<k;
                }else if(cur==WHITE_R){
                    white_vec += 1<<k;
                }
            }
        }else{
            int range = 15+index;
            for(int k=0; k<range; k++){
                int cur = current_board[k][-index+k];
                if(cur==BLACK_R){
                    black_vec += 1<<k;
                }else if(cur==WHITE_R){
                    white_vec += 1<<k;
                }
            }
        }
        //black_vecs[direc][index+10] = black_vec;
        //white_vecs[direc][index+10] = white_vec;
    }else if(direc == "a"){
        if(index>=0){
            int range = 15-index;
            for(int k=0; k<range; k++){
                int cur = current_board[14-index-k][k];
                if(cur==BLACK_R){
                    black_vec += 1<<k;
                }else if(cur==WHITE_R){
                    white_vec += 1<<k;
                }
            }
        }else{
            int range = 15+index;
            for(int k=0; k<range; k++){
                int cur = current_board[14-k][-index+k];
                if(cur==BLACK_R){
                    black_vec += 1<<k;
                }else if(cur==WHITE_R){
                    white_vec += 1<<k;
                }
            }
        }
        //black_vecs[direc][index+10] = black_vec;
        //white_vecs[direc][index+10] = white_vec;
    }
    struct GameAI::TwoBitStrings black_and_white_vecs;
    black_and_white_vecs.black_vec = black_vec,
    black_and_white_vecs.white_vec = white_vec;
    return black_and_white_vecs;
}

void GameAI::FitTemplates(struct GameAI::TwoBitStrings black_and_white_vecs, int len, int index, string direc, bool recalculate, int changed_point, int old_value){
      if(len<5){
          return;
      }      
      int init_x;
      int init_y;
      int dx;
      int dy;
      if(direc=="x"){
          init_x = 0;
          init_y = index;
          dx = 1;
          dy = 0;
      }else if(direc =="y"){
          init_x = index;
          init_y = 0;
          dx = 0;
          dy = 1;
      }else if(direc == "d"){
          if(index>=0){
              init_x = index;
              init_y = 0;
          }else{
              init_x = 0;
              init_y = -index;
          }
          dx = 1;
          dy = 1;
      }else if(direc == "a"){
          if(index>=0){
              init_x = 14-index;
              init_y = 0;
          }else{
              init_x = 14;
              init_y = -index;
          }
          dx = -1;
          dy = 1;
      }else{
          throw "typo";
      }

      int black_vec = black_and_white_vecs.black_vec;
      int white_vec = black_and_white_vecs.white_vec;
      int old_black_vec = black_vec;
      int old_white_vec = white_vec;
      if(!recalculate){
          if(old_value==BLACK_R){
              old_black_vec = black_vec|(1<<changed_point);
          }else if(old_value==WHITE_R){
              old_white_vec = white_vec|(1<<changed_point);
          }else{//old_value==EMPTY_R
              old_black_vec = black_vec&(~(1<<changed_point));
              old_white_vec = white_vec&(~(1<<changed_point));
          }
      }else{
          //suppose all the state estimations are already reset
          old_black_vec = 0;//let them not fitting any template
          old_white_vec = 0;
      }

      /*
       * Type: black five
       * Shape: XXXXX
       * Length: 5
       * Representation: 31, 0
       */
      int centers[10];
      int num_of_centers = 0;
      FitTheTemplate(black_vec, white_vec, old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 5, 31, 0,
                 centers, num_of_centers, num_of_black_wins, num_of_white_wins, NULL, NULL);

      /*
       * Type: black four
       * Shape: _XXXX_
       * Length: 6
       * Representation: 30, 0
       */
      centers[0] = 0;
      centers[1] = 5;
      num_of_centers = 2;
      FitTheTemplate(black_vec, white_vec,  old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 6, 30, 0,
                    centers, num_of_centers, num_of_black_wins, num_of_white_wins, black_fours, white_fours);

      /*
       * Type: black half four type 1
       * Shape: _XXXXO
       * Length: 6
       * Center: 0
       * Representation: 30, 32
       */
      centers[0] = 0;
      num_of_centers = 1;
      FitTheTemplate(black_vec, white_vec,  old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 6, 30, 32,
                     centers, num_of_centers, num_of_black_half_fours, num_of_white_half_fours, black_half_fours, white_half_fours);

      /*
       * Type: black half four type 1'
       * Shape: OXXXX_
       * Length: 6
       * Center: 5
       * Representation: 30, 1
       */
      centers[0] = 5;
      num_of_centers = 1;
      FitTheTemplate(black_vec, white_vec,  old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 6, 30, 1,
               centers, num_of_centers, num_of_black_half_fours, num_of_white_half_fours, black_half_fours, white_half_fours);

      /*
       * Type: black half four type 2
       * Shape: X_XXX
       * Length: 5
       * Center: 1
       * Representation: 29, 0
       */
      centers[0] = 1;
      num_of_centers = 1;
      FitTheTemplate(black_vec, white_vec,  old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 5, 29, 0,
               centers, num_of_centers, num_of_black_half_fours, num_of_white_half_fours, black_half_fours, white_half_fours);

      /*
       * Type: black half four type 2'
       * Shape: XXX_X
       * Length: 5
       * Center: 3
       * Representation: 23, 0
       */
      centers[0] = 3;
      num_of_centers = 1;
      FitTheTemplate(black_vec, white_vec,  old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 5, 23, 0,
              centers, num_of_centers, num_of_black_half_fours, num_of_white_half_fours, black_half_fours, white_half_fours);

      /*
       * Type: black half four type 3
       * Shape: XX_XX
       * Length: 5
       * Center: 2
       * Representation: 27, 0
       */
      centers[0] = 2;
      num_of_centers = 1;
      FitTheTemplate(black_vec, white_vec,  old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 5, 27, 0,
           centers, num_of_centers, num_of_black_half_fours, num_of_white_half_fours, black_half_fours, white_half_fours);

      /*
       * Type: black active three type 1
       * Shape: _XXX__
       * Length: 6
       * Center: 4
       * Representation: 14, 0
       */
      centers[0] = 4;
      num_of_centers = 1;
      FitTheTemplate(black_vec, white_vec,  old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 6, 14, 0,
              centers, num_of_centers, num_of_black_active_threes, num_of_white_active_threes, black_active_threes, white_active_threes);
      /*
       * Type: black active three type 1'
       * Shape: __XXX_
       * Length: 6
       * Center: 1
       * Representation: 28, 0
       */
      centers[0] = 1;
      num_of_centers = 1;
      FitTheTemplate(black_vec, white_vec, old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 6, 28, 0,
              centers, num_of_centers, num_of_black_active_threes, num_of_white_active_threes, black_active_threes, white_active_threes);
      /*
       * Type: black active three type 2
       * Shape: _X_XX_
       * Length: 6
       * Center: 2
       * Representation: 26, 0
       */
      centers[0] = 2;
      num_of_centers = 1;
      FitTheTemplate(black_vec, white_vec, old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 6, 26, 0,
          centers, num_of_centers, num_of_black_active_threes, num_of_white_active_threes, black_active_threes, white_active_threes);

      /*
       * Type: black active three type 2'
       * Shape: _XX_X_
       * Length: 6
       * Center: 3
       * Representation: 22, 0
       */
      centers[0] = 3;
      num_of_centers = 1;
      FitTheTemplate(black_vec, white_vec, old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 6, 22, 0,
               centers, num_of_centers, num_of_black_active_threes, num_of_white_active_threes, black_active_threes, white_active_threes);

      /*
       * Type: black half three type 1
       * Shape: __XXXO
       * Length: 6
       * Center: 0 and 1
       * Representation: 28, 32
       */
      centers[0] = 0;
      centers[1] = 1;
      num_of_centers = 2;
      FitTheTemplate(black_vec, white_vec,  old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 6, 28, 32,
          centers, num_of_centers, num_of_black_half_threes, num_of_white_half_threes, black_half_threes, white_half_threes);

      /*
       * Type: black half three type 1'
       * Shape: OXXX__
       * Length: 6
       * Center: 4 and 5
       * Representation: 14, 1
       */
      centers[0] = 4;
      centers[1] = 5;
      num_of_centers = 2;
      FitTheTemplate(black_vec, white_vec,  old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 6, 14, 1,
          centers, num_of_centers, num_of_black_half_threes, num_of_white_half_threes, black_half_threes, white_half_threes);

     /*
      * Type: black half three type 2
      * Shape: _X_XXO
      * Length: 6
      * Center: 0 and 2
      * Representation: 26, 32
      */
      centers[0] = 0;
      centers[1] = 2;
      num_of_centers = 2;
      FitTheTemplate(black_vec, white_vec, old_black_vec, old_white_vec,  len, init_x, init_y, dx, dy, 6, 26, 32,
              centers, num_of_centers, num_of_black_half_threes, num_of_white_half_threes, black_half_threes, white_half_threes);

     /*
      * Type: black half three type 2'
      * Shape: OXX_X_
      * Length: 6
      * Centers: 3 and 5
      * Representation: 22, 1
      */
      centers[0] = 3;
      centers[1] = 5;
      num_of_centers = 2;
      FitTheTemplate(black_vec, white_vec,  old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 6, 22, 1,
            centers, num_of_centers, num_of_black_half_threes, num_of_white_half_threes, black_half_threes, white_half_threes);

     /*
      * Type: black half three type 3
      * Shape: OX_XX_
      * Length: 6
      * Center: 2 and 5
      * Representation: 26, 1
      */
      centers[0] = 2;
      centers[1] = 5;
      num_of_centers = 2;
      FitTheTemplate(black_vec, white_vec,  old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 6, 26, 1,
             centers, num_of_centers, num_of_black_half_threes, num_of_white_half_threes, black_half_threes, white_half_threes);

     /*
      * Type: black half three type 3'
      * Shape: _XX_XO
      * Length: 6
      * Center: 0 and 3
      * Representation: 22, 32
      */
      centers[0] = 0;
      centers[1] = 3;
      num_of_centers = 2;
      FitTheTemplate(black_vec, white_vec,  old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 6, 22, 32,
             centers, num_of_centers, num_of_black_half_threes, num_of_white_half_threes, black_half_threes, white_half_threes);

     /*
      * Type: black half three type 4
      * Shape: X__XX
      * Length: 5
      * Center: 1 and 2
      * Representation: 25, 0
      */
      centers[0] = 1;
      centers[1] = 2;
      num_of_centers = 2;
      FitTheTemplate(black_vec, white_vec,  old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 5, 25, 0,
            centers, num_of_centers, num_of_black_half_threes, num_of_white_half_threes, black_half_threes, white_half_threes);
     /*
      * Type: black half three type 4'
      * Shape: XX__X
      * Length: 5
      * Center: 2 and 3
      * Representation: 19, 0
      */
      centers[0] = 2;
      centers[1] = 3;
      num_of_centers = 2;
      FitTheTemplate(black_vec, white_vec,  old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 5, 19, 0,
           centers, num_of_centers, num_of_black_half_threes, num_of_white_half_threes, black_half_threes, white_half_threes);

     /*
      * Type: black half three type 5
      * Shape: X_X_X
      * Length: 5
      * Center: 1 and 3
      * Representation: 21, 0
      */
      centers[0] = 1;
      centers[1] = 3;
      num_of_centers = 2;
      FitTheTemplate(black_vec, white_vec,  old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 5, 21, 0,
          centers, num_of_centers, num_of_black_half_threes, num_of_white_half_threes, black_half_threes, white_half_threes);

     /*
      * Type: black active two type 1
      * Shape: _XX__
      * Length: 5
      * Center: 3
      * Representation: 6, 0
      */
      centers[0] = 3;
      num_of_centers = 1;
      FitTheTemplate(black_vec, white_vec,  old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 5, 6, 0,
           centers, num_of_centers, num_of_black_active_twos, num_of_white_active_twos, black_active_twos, white_active_twos);

     /*
      * Type: black active two type 1'
      * Shape: __XX_
      * Length: 5
      * Center: 1
      * Representation: 12, 0
      */
      centers[0] = 1;
      num_of_centers = 1;
      FitTheTemplate(black_vec, white_vec,  old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 5, 12, 0,
           centers, num_of_centers, num_of_black_active_twos, num_of_white_active_twos, black_active_twos, white_active_twos);

     /*
      * Type: black active two type 2
      * Shape: _X_X_
      * Length: 5
      * Center: 2
      * Representation: 10, 0
      */
      centers[0] = 2;
      num_of_centers = 1;
      FitTheTemplate(black_vec, white_vec,  old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 5, 10, 0,
          centers, num_of_centers, num_of_black_active_twos, num_of_white_active_twos, black_active_twos, white_active_twos);

     /*
      * Type: black active two type 3
      * Shape: _X__X_
      * Length: 6
      * Center: 2, 3
      * Representation: 18, 0
      */
      centers[0] = 2;
      centers[1] = 3;
      num_of_centers = 2;
      FitTheTemplate(black_vec, white_vec,  old_black_vec, old_white_vec, len, init_x, init_y, dx, dy, 6, 18, 0,
           centers, num_of_centers, num_of_black_active_twos, num_of_white_active_twos, black_active_twos, white_active_twos);
}

void GameAI::FitTheTemplate(int black_vec, int white_vec, int old_black_vec, int old_white_vec, int len, int init_x, int init_y, int dx, int dy,
                           int temp_size, int temp1, int temp2, int *centers, int num_of_centers,
                           int &black_count, int &white_count, int black_locations[15][15], int white_locations[15][15]){
    bool fitsNew, fitsOld;
    int center_x, center_y;
    int iters = len-temp_size+1;
    int x = init_x;
    int y = init_y;
    for(int n=0; n<iters; n++){
        fitsNew = (((black_vec>>n)&((1<<temp_size)-1))==temp1)&&
                  (((white_vec>>n)&((1<<temp_size)-1))==temp2);
        fitsOld = (((old_black_vec>>n)&((1<<temp_size)-1))==temp1)&&
                  (((old_white_vec>>n)&((1<<temp_size)-1))==temp2);
        if(fitsNew&&!fitsOld){
            //cout<<"adding black"<<endl;
            /*
            cout<<"checking for black shape..."<<endl;
            cout<<"temp1 "<<temp1<<" temp2 "<<temp2<<endl;
            cout<<"cur_black_vec "<<cur_black_vec<<" cur_white_vec "<<cur_white_vec<<endl;
            cout<<endl;
            */
            for(int i=0; i<num_of_centers; i++){
                center_x = x + centers[i]*dx;
                center_y = y + centers[i]*dy;
                black_locations[center_x][center_y] += 1;
            }
            black_count += 1;
        }else if(!fitsNew&&fitsOld){
            //cout<<"deleting black"<<endl;
            for(int i=0;i<num_of_centers;i++){
                center_x = x + centers[i]*dx;
                center_y = y + centers[i]*dy;
                black_locations[center_x][center_y] -= 1;
            }
            black_count -= 1;
        }
        x += dx;
        y += dy;
    }
    x = init_x;
    y = init_y;
    for(int n=0; n<iters; n++){
        //reverse of previous section
        fitsNew = (((white_vec>>n)&((1<<temp_size)-1))==temp1)&&
                  (((black_vec>>n)&((1<<temp_size)-1))==temp2);
        fitsOld = (((old_white_vec>>n)&((1<<temp_size)-1))==temp1)&&
                  (((old_black_vec>>n)&((1<<temp_size)-1))==temp2);
        if(fitsNew&&(!fitsOld)){
            /*
            cout<<"checking for black shape..."<<endl;
            cout<<"temp1 "<<temp1<<" temp2 "<<temp2<<endl;
            cout<<"cur_black_vec "<<cur_black_vec<<" cur_white_vec "<<cur_white_vec<<endl;
            cout<<endl;
            */
            //cout<<"adding white"<<endl;
            for(int i=0; i<num_of_centers; i++){
                center_x = x + centers[i]*dx;
                center_y = y + centers[i]*dy;
                white_locations[center_x][center_y] += 1;
            }
            white_count += 1;
        }else if((!fitsNew)&&fitsOld){
            //cout<<"deleting white"<<endl;
            for(int i=0;i<num_of_centers;i++){
                center_x = x + centers[i]*dx;
                center_y = y + centers[i]*dy;
                white_locations[center_x][center_y] -= 1;
            }
            white_count -= 1;
        }
        x += dx;
        y += dy;
    }
}

void GameAI::InitializeStateEstimation(){
    for(int i=0;i<15;i++){
        for(int j=0;j<15;j++){
            black_half_fours[i][j] = 0;
            black_active_threes[i][j] = 0;
            black_half_threes[i][j] = 0;
            black_active_twos[i][j] = 0;
            white_half_fours[i][j] = 0;
            white_active_threes[i][j] = 0;
            white_half_threes[i][j] = 0;
            white_active_twos[i][j] = 0;
        }
    }
    num_of_black_wins = 0;
    num_of_white_wins = 0;
    num_of_black_half_fours = 0;
    num_of_black_active_threes = 0;
    num_of_black_half_threes = 0;
    num_of_black_active_twos = 0;
    num_of_white_half_fours = 0;
    num_of_white_active_threes = 0;
    num_of_white_half_threes = 0;
    num_of_white_active_twos = 0;
    string direcs[4] = {"x","y","d","a"};
    for(int k = 0; k<2; k++){
        for(int index=0; index<15; index++){
            //cout<<"index: "<<index<<" direction "<<direcs[k]<<endl;
            //struct TwoBitStrings vecs = GetVector(index, direcs[k]);
            //cout<<"black vec: "<<vecs.black_vec<<" white vec: "<<vecs.white_vec<<endl;
            FitTemplates(GetVector(index, direcs[k]), 15, index, direcs[k]);
        }
    }
    for(int k = 2; k<4; k++){
        for(int index=-10; index<11; index++){
            //cout<<"index: "<<index<<" direction "<<direcs[k]<<endl;
            //struct TwoBitStrings vecs = GetVector(index, direcs[k]);
            //cout<<"black vec: "<<vecs.black_vec<<" white vec: "<<vecs.white_vec<<endl;
            FitTemplates(GetVector(index, direcs[k]), 15-abs(index), index, direcs[k]);
        }
    }
    //ShowBoard(black_active_twos);
    //ShowBoard(white_active_twos);
    //cout<<num_of_black_active_twos<<endl;
    //cout<<num_of_white_active_twos<<endl;
}
