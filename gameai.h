/*
 * File: gameai.h
 * ------------------------------
 * This is the header file for game
 * ai class.
 */
#ifndef GAMEAI_H
#define GAMEAI_H
#include <iostream>
#include "gamemodel.h"
#include "stone.h"
#include "coordinate.h"
#include <map>
#include <stack>
#include <queue>
#include <string>
using namespace std;

/*
 * Constant: WHITE_R, BLACK_R, EMPTY_R
 * ------------------------------------------
 * These constants defines the representations
 * of all the colors on the board. They are in
 * conform with REPRESENTATION in gamemodel.
 */
const int WHITE_R = -1;
const int BLACK_R = 1;
const int EMPTY_R = 0;

/*
 * Constants: BOARDSIZEX, BOARDSIZEY
 * ------------------------------------------
 * These constants defines size of the board.
 * They are in conform with BOARDSIZE in gamemodel.
 */
const int BOARDSIZEX = 15;
const int BOARDSIZEY = 15;

/*
 * Class: GameAI
 * ---------------------------------------------------
 * This class contains functions and instances necessary
 * for analyzing a game board and making a sensible move.
 */
class GameAI: public GameModel
{
public:
    /*
     * Method: constructor
     * Usage: GameAI myAI(game, "black", 5)
     * ----------------------------------------
     * This function initializes a game ai and copies
     * the game board, history and other parameters.
     * Color is either "black" or "white", which
     * specify the order of playing. Max depth
     * specicies maximal searching depth.
     */
    GameAI(GameModel &game, string color, int max_depth=4);
    /*
     * Method: Update
     * Usage: myAI.Update(game)
     * ---------------------------------------
     * This function is used to copy all the
     * state information in game to the ai. This
     * keeps the ai in conformity with game.
     */
    void Update(GameModel &game);

    /*
     * Method: quick_computer_move
     * Usage: next_move = myAI.quick_computer_move()
     * ----------------------------------------
     * This method generates next move very
     * fast. Before calling the method, Update
     * must be called to synchronize the game information.
     */
    Coordinate quick_computer_move();

    /*
     * Method: computer_move
     * Usage: next_move = myAI.computer_move()
     * ----------------------------------------
     * This method generates next move using
     * a slower but better algorithm. Before
     * calling the method, Update must be called
     * to synchronize the game information.
     */
    Coordinate computer_move();//main searching algorithm

    /*
     * Method: TryMove
     * Usage: myAI.TryMove()
     * ----------------------------------------------
     * This method makes a move on the "fictional"
     * game board stored in gameai and make some
     * changes to state (unless the move is illegal.
     * It it called during searching.The method returns
     * strings terminated/continuing/illegal.
     */
    string TryMove(Coordinate move);

    /*
     * Method: CancelTry
     * Usage: myAI.CancelTry
     * --------------------------------------
     * This method retract the last try move
     * and cancel the change caused by TryMove.
     * It is called during searching.
     */
    void CancelTry();

    /*
     * Method: ABSearch
     * Usage: myAI.ABSearch()
     * ------------------------------------
     * This method searches the game three
     * for a certain depth and returns the
     * estimated score. alpha, beta are lower
     * bounds and upper bounds of the estimated
     * score, which helps prunning.
     */
    int ABSearch(int alpha, int beta);

    /*
     * Method: RankOptions
     * Usage: myAI.RankOptions(options)
     * ---------------------------------
     * This method accepts an empty queue
     * and pushes the coordinate of positions
     * based on their priority during searching.
     *
     */
    void RankOptions(queue<Coordinate> & options);
    /*
     * Mehtod: WorstOption
     * Usage: move = WorstOption()
     * --------------------------------------
     * This function returns the most unlikely
     * move, which is equivalent to let opponent
     * to take an extra move.
     */
    Coordinate WorstOption();
    /*
     * Method: EstimateState
     * Usage: score = myAI.EstimateState()
     * ----------------------------------
     * This method estimates the score of
     * a certain board position during its
     * searching. The larger the score is,
     * the more advantageous it is for the
     * ai.
     */
    int EstimateState();//in whose_turn's perspective

    /*
     * Method: CalculateVCT
     * Usage: CalculateVCT(canWin, 15)
     *        move = myAI.CalculateVCT(canWin, 15, true)
     * -------------------------------------------
     * This method determines whether the ai can
     * win under a certain board position by continuously
     * producing 3 or 4. It returns the result by
     * modifying the boolean canWin. The max depth
     * for searching is specified by max_depth.
     * If provide_move is true and the ai has a way to
     * win, it will return a meaningful coordinate to
     * represent the optimal move.
     */
    Coordinate CalculateVCT(bool & canWin, int max_depth, bool provide_move=false);

    /*
     * Method: RespondVCT
     * Usage: myAI.RepsondVCT(canDefend, 15)
     * -------------------------------------------
     * This method determines whether the ai has a
     * way to defend a VCT offense. It is only
     * called by CalculateVCT to help his calculation.
     */
    void RespondVCT(bool & canDefend, int max_depth);//type = "four" or "three"

    /*
     * Method: InitHash, UpdateHash, RedoHash
     * Usage: myAI.InitHash(), myAI.UpdateHash(last_move), myAI.RedoHash()
     * --------------------------------------------
     * These three methods are responsible for updating
     * the hash value for ai's fictional game board.
     * They are called by Update, TakeMove and CancelMove
     * respectively.
     */
    void InitHash();
    void UpdateHash(Stone last_move);
    void RedoHash();

    /*
     * Method: isMaxNode
     * Usage: if(myAI.isMaxNode())..
     * --------------------------------------------
     * This function is used by ABSearch to determine
     * whether current node in game tree is a max node.
     */
    bool isMaxNode();

    /*
     * Method: get_relative_depth
     * Usage: depth = myAI.get_relative_depth()
     * -------------------------------------
     * This function is used by ABSearch to
     * determine the current node's depth in
     * the game tree.
     */
    int get_relative_depth();

    /*
     * Method: toString
     * Usage: cout<<myAI.toString()
     * --------------------------------------
     * This method prints some parameters and
     * state variables of the ai.
     */
    string toString();

    /*
     * Method: get_fives
     * Usage: stones_in_five = myAI.get_fives()
     * --------------------------------------
     * This methods returns a vector of Stone
     * objects which represents the stones lies
     * in a continuous five, if any.
     */
    vector<Stone> get_fives();

private:
    /*parameters that controls searching*/
    //scores for a certain chess form. used during state estimation.
    //"my" means judging from current fictional player's perspective.
    static const int LARGEST_NUMBER = 9000; //score for a winning state
    static const int SCORE_FOR_MY_HALF_THREE = 33;
    static const int SCORE_FOR_MY_ACTIVE_TWO = 15;
    static const int SCORE_FOR_YOUR_HALF_FOUR = -64;
    static const int SCORE_FOR_YOUR_ACTIVE_THREE = -31;
    static const int SCORE_FOR_YOUR_HALF_THREE = -10;
    static const int SCORE_FOR_YOUR_ACTIVE_TWO = -4;
    //maximal searching depth for different kinds of searching
    static const int INTREE_VCT_DEPTH = 5;
    static const int OUTTREE_VCT_DEPTH = 20;
    const int MAX_DEPTH;

    /*fixed constant*/
    static const int HASH_SIZE = 4;

    /*variables invariant during searching*/
    string my_color;
    int game_depth;//the number of steps that has been played

    /*variables for searching*/
    int global_step=0;
    int whose_turn_repre;
    int opponent_repre;
    int current_board2[BOARDSIZEX][BOARDSIZEY];

    /*hash section*/
    //a hash code is represented as a int vector of length 4
    struct cmp_bitstr{ //comparator of int vector
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
    map<Stone, vector<int>> hash_lookup; //hash value for each stone
    map<vector<int>, int, cmp_bitstr> state_values; //cleared each step
    vector<int> cur_hash; //hash value for current board under checking


    /*----------------------heuristic function section--------------------*/
    /*
     * This section defines instances and methods that scores each position
     * on a board. Then they are used to rank the position and suggest moves
     * that worth looking into.
     */
    typedef struct
    {
        int x,y,l,r;
    }value;
    value fiveValue[BOARDSIZEX][BOARDSIZEY];
    int siteValue[BOARDSIZEX][BOARDSIZEY];
    void set_five_value();
    void set_site_value();
    int chart(int grade);


    /*----------------------state estimation section----------------------*/
    /*
     * For convenience of analysis, board is partitioned from different directions
     * into "vectors" which are represented as bitstrings.
     */
    struct TwoBitStrings{  //pack up two vectors
        int black_vec = 0;
        int white_vec = 0;
    };


    /*
     * Variables recording the position of the board for VCT or state
     * estimation. They are updated partially whenever changes are
     * made to the board. When large changes to board happens(when
     * Update is called), they  need to be reinitialized.
     */
    int black_fives[BOARDSIZEX][BOARDSIZEY];
    int black_fours[BOARDSIZEX][BOARDSIZEY];
    int black_half_fours[BOARDSIZEX][BOARDSIZEY];
    int black_active_threes[BOARDSIZEX][BOARDSIZEY];
    int black_half_threes[BOARDSIZEX][BOARDSIZEY];
    int black_active_twos[BOARDSIZEX][BOARDSIZEY];
    int white_fives[BOARDSIZEX][BOARDSIZEY];
    int white_fours[BOARDSIZEX][BOARDSIZEY];
    int white_half_fours[BOARDSIZEX][BOARDSIZEY];
    int white_active_threes[BOARDSIZEX][BOARDSIZEY];
    int white_half_threes[BOARDSIZEX][BOARDSIZEY];
    int white_active_twos[BOARDSIZEX][BOARDSIZEY];
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
     * Method: GetVector
     * Usage: FitTemplate(GetVector(2,"x"),15, 2, "d", "false", 2,2)
     * ------------------------------------------------------------
     * It's a method that extracts a row, colomn, diagonol or
     * antidiagonol as a bitstring. Index is the index of the line,
     * direc means direction of the line.
     */
    struct GameAI::TwoBitStrings GetVector(int index, string direc);

    /*
     * Method: FitTemplates, FitTheTemplate
     * Usage: FitTemplate(GetVector(2,"x"), 15, 2, "d", "false", 2, 2)
     * -------------------------------------------------------------
     * FitTemplates compares part of the bit strings with the given
     * representations of various shapes. They get the number
     * of a certain shape and position of them.
     * index, direc represent the position of the vector.
     * Shape representations are hard-coded in FitTemplate.
     * FitTheTemplate is a helper method for FitTemplates.
     */
    void FitTemplates(struct GameAI::TwoBitStrings vecs, int len, int index, string direc,
                      bool recalculate = true, int changed_point=-1, int old_value=-2);
    void FitTheTemplate(int black_vec, int white_vec, int old_black_vec, int old_white_vec,
                       int len, int init_x, int init_y, int dx, int dy,
                       int temp_size, int temp1, int temp2, int* centers, int num_of_centers,
                       int &black_count, int &white_count,
                       int black_locations[BOARDSIZEX][BOARDSIZEY], int white_locations[BOARDSIZEX][BOARDSIZEY]);
    /*
     * Method: InitializeStateEstimation
     * Usage: InitializeStateEstimation()
     * -------------------------------------------------------
     * This method performs state etimation for the whole board
     * It is called by the method Update.
     */
    void InitializeStateEstimation();

    /*helper methods*/
    /*
     * Method: sort
     * Usage: sort(keys, values, 0,n)
     * -----------------------------------------------------
     * This function sorts a vector array according to their
     * keys.
     */
    void sort(vector<int> &keys, vector<Coordinate> &values, int h, int t);
    /*
     * Method: apply_xor
     * Usage: apply_xor(result, bitstr1, bitstr2)
     * ------------------------------------------------------
     * This function performs elementwise xor operation on two
     * int vectors. The result is modified in the first parameter.
     */
    void apply_xor(vector<int> &result, const vector<int> &bitstr1, const vector<int> &bitstr2);
};
ostream & operator << (ostream & os, GameAI & ai);

#endif // GAMEAI_H
