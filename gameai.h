#ifndef GAMEAI_H
#define GAMEAI_H
#include <iostream>
#include "gamemodel.h"
#include "stone.h"
#include "coordinate.h"
#include <map>
#include <stack>
using namespace std;

typedef struct
{
    int x,y,l,r;
}value;


class GameAI: private GameModel
{
public:
    GameAI(GameModel &game, string color, int max_depth=6);//copy all the attributes
    void Update(GameModel &game);//compare the history and do updates

    Coordinate Decide();//main searching algorithm
    int ABSearch(int p_ab);//recurrent steps

    string TryMove(Coordinate move);//TakeMove and update hash. Return terminated/continuing/illegal. Illegal result will have hash updated.
    void CancelTry();//cancel last move and update hash. ?Illegal action throw error

    bool isMaxNode();
    int get_relative_depth();

    void InitHash();
    void UpdateHash(Stone last_move);
    void RedoHash();//one step redo

    //part of the policy
    void RankOptions(stack<Coordinate> & options);

    int EstimateState();//by adding up point scores

    //The following two methods operates on board. Bug warning!
    void FindKillingMoves(stack<Coordinate> & killing_moves, string color);
    void FindHalfFourMoves(stack<Coordinate> & half_four_moves, string color);
    void FindActiveThreeMoves(stack<Coordinate> & active_three_moves, string color);

    //output whether can suceed, and a victory move. Your killing moves are also taken into account.
    //But opponents' four, five are not taken into account.
    void CalculateVCT(bool & canWin,Coordinate & vic_move);
    void CalculateVCT(bool & canWin);
    void RespondVCT(bool & canDefend, Coordinate op_move, string type);//type = "four" or "three"
    string toString();


private:
    static const int LARGEST_NUMBER = 1000000;
    static const int HASH_SIZE = 4;
    //int global_step=0;
    const int MAX_DEPTH;
    string my_color;
    int game_depth;//the number of steps that has been played

    struct cmp_bitstr{
        bool operator()(const vector<int> &a, const vector<int> & b) const{
            int i=0;
            while(a[i]==b[i]){
                i++;
                if(i==HASH_SIZE){
                    return false;
                }
            }
            return a[i]<b[i];
        }
    };

    map<Stone, vector<int>> hash_lookup;//hash value for each stone
    map<vector<int>, int, cmp_bitstr> state_values;//cleared each step
    vector<int> cur_hash;

    /*The following inherited properties record the state of current search node*/
    /*
    string whose_turn;
    bool terminated;
    string winner;//"black","white","tie","None"
    int round;
    Grid<int> current_board;
    Vector<Stone> history_moves;

    int num_of_empty_places;
    */

    //They are part of policy
    vector<vector<value>> fiveValue;
    vector<vector<int>> siteValue;


    /*helper methods*/
    void sort(vector<int> &keys, vector<Coordinate> &values, int h, int t);//sort in increasing order
    void apply_xor(vector<int> &result, const vector<int> &bitstr1, const vector<int> &bitstr2);
    void ReverseBoard();//This method is to help set_five_value. It only works under default representation
    void get_influence_domain(stack<Coordinate> & inf_dom, Coordinate co);//return empty places influenced by co


    //The following two hard-coded methods only work under default representations.
    void set_five_value();
    void set_site_value();
    int chart(int grade);

};
ostream & operator << (ostream & os, GameAI & ai);

#endif // GAMEAI_H
