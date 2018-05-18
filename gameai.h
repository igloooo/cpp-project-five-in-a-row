#ifndef GAMEAI_H
#define GAMEAI_H
#include <iostream>
#include "gamemodel.h"
#include "stone.h"
#include "coordinate.h"
#include <map>
#include <stack>
#include <string>
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
    int ABSearch(int alpha, int beta);//recurrent steps

    string TryMove(Coordinate move);//TakeMove and update hash. Return terminated/continuing/illegal. Illegal result will have hash updated.
    void CancelTry();//cancel last move and update hash. ?Illegal action throw error

    bool isMaxNode();
    int get_relative_depth();

    void InitHash();
    void UpdateHash(Stone last_move);
    void RedoHash();//one step redo

    //part of the policy
    void RankOptions(stack<Coordinate> & options);

    int EstimateState();//in whose_turn's perspective

    //The following two methods operates on board. Bug warning!
    void FindMoves(stack<Coordinate> &moves, string color, bool(*finder)(const vector<vector<int>>&, string, Coordinate));
    void FindKillingMoves(stack<Coordinate> & killing_moves, string color);
    void FindHalfFourMoves(stack<Coordinate> & half_four_moves, string color);
    void FindActiveThreeMoves(stack<Coordinate> & active_three_moves, string color);

    //output whether can suceed, and a victory move. Your killing moves are also taken into account.
    //But opponents' four, five are not taken into account.
    Coordinate CalculateVCT(bool & canWin, bool provide_move=false);
    void RespondVCT(bool & canDefend);//type = "four" or "three"
    string toString();

private:
    static const int LARGEST_NUMBER = 1000000;
    static const int SCORE_FOR_MY_HALF_THREE = 50;
    static const int SCORE_FOR_MY_ACTIVE_TWO = 60;
    static const int SCORE_FOR_YOUR_HALF_FOUR = -90;
    static const int SCORE_FOR_YOUR_ACTIVE_THREE = -100;
    static const int SCORE_FOR_YOUR_HALF_THREE = -20;
    static const int SCORE_FOR_YOUR_ACTIVE_TWO = -30;
    static const int HASH_SIZE = 4;
    int global_step=0;
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

    int current_board2[BOARDSIZEX][BOARDSIZEY];

    //They are part of policy
    value fiveValue[BOARDSIZEX][BOARDSIZEY];
    int siteValue[BOARDSIZEX][BOARDSIZEY];


    struct TwoBitStrings{
        int black_vec = 0;
        int white_vec = 0;
    };
    //related to state estimation
    //They will be used during state estimation or VCT
    //When entering state estimation, they will be initialized
    //partially updated in VCT
    /*
    map<string, vector<int>> black_vecs = {{"x", vector<int>(15)},
                                           {"y", vector<int>(15)},
                                           {"d", vector<int>(21)}, //vector with direc, index saved in [direc][index+10]
                                           {"a", vector<int>(21)}};
    map<string, vector<int>> white_vecs = {{"x", vector<int>(15)},
                                           {"y", vector<int>(15)},
                                           {"d", vector<int>(21)},
                                           {"a", vector<int>(21)}};
    */

    //following propoperties can be modified by method FitTemplates

    //accessing order: initialize(first time being used)=>Template Fit=>VCT
    //They are being partially updated before accessed by VCT
    int black_fours[15][15];
    int white_fours[15][15];
    int black_half_fours[15][15];
    int black_active_threes[15][15];
    int black_half_threes[15][15];
    int black_active_twos[15][15];
    int white_half_fours[15][15];
    int white_active_threes[15][15];
    int white_half_threes[15][15];
    int white_active_twos[15][15];
    //only accessed by FitTemplates then state estimation.
    //initialized during state estimation
    //unused in VCT, so they are not updated
    int num_of_black_wins = 0;
    int num_of_white_wins = 0;
    int num_of_black_half_fours = 0;
    int num_of_black_active_threes = 0;
    int num_of_black_half_threes = 0;
    int num_of_black_active_twos = 0;
    int num_of_white_half_fours = 0;
    int num_of_white_active_threes = 0;
    int num_of_white_half_threes = 0;
    int num_of_white_active_twos = 0;

    /*
     * Implementation notes:
     * direc "x" means fix y vary x, index range from 0 to 14;
     * "y" fix x vary y, index range from 0 to 14
     * "d" means diagonol, index range from -10 to 10, start from [index][0] or [0][-index], increase by 1,1
     * "a" means antidiagonol, index range from -10 to 10, start from [14+index][0] or [14][-index], increase by -1,1
     * vectors of whose_turn's color and vectors of opponent's color are both saved in board_vecs
     */
    struct GameAI::TwoBitStrings GetVector(int index, string direc);
    /*
     * bitstring represent by int, len is its length
     * index, direc represent the position of the vector
     * shape templates are hard-coded in the function.
     */
    void FitTemplates(struct GameAI::TwoBitStrings vecs, int len, int index, string direc);
    /*
     * Fit two template1 and template2 to black vectors
     */
    void FitTheTemplate(int black_vec, int white_vec, int len, int init_x, int init_y, int dx, int dy,
                       int temp_size, int temp1, int temp2, int* centers, int num_of_centers,
                       int &black_count, int &white_count, int black_locations[15][15], int white_locations[15][15]);
    /*
     * This method performs state etimation for the whole board
     * It is called by the method update.
     */
    void InitializeStateEstimation();

    /*helper methods*/
    void sort(vector<int> &keys, vector<Coordinate> &values, int h, int t);//sort in increasing order
    void apply_xor(vector<int> &result, const vector<int> &bitstr1, const vector<int> &bitstr2);
    //void ReverseBoard();

    //The following two hard-coded methods only work under default representations.
    void set_five_value();
    void set_site_value();
    int chart(int grade);

};
ostream & operator << (ostream & os, GameAI & ai);

#endif // GAMEAI_H
