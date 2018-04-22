#include "shapefinder.h"
#include "gamemodel.h"
#include "coordinate.h"
#include <map>
#include <vector>
using namespace std;

bool have_five_at(const vector<vector<int>> & board, string color, Coordinate xy){
    map<string, Coordinate>::const_iterator it = DIRECTIONS.cbegin();
    int count;
    int i;
    Coordinate cur;
    while(it!=DIRECTIONS.cend()){
        count = 0;
        i = 0;
        while(true){
            cur = xy+multiply(it->second,i);
            if(!((cur<BOARDSIZE)&&(cur>=Coordinate(0,0))&&
             (board[cur.x][cur.y]==REPRESENTATION.at(color)))){
                 break;
            }
            count++;
            i++;
        }
        i = -1;
        while(true){
            cur = xy+multiply(it->second,i);
            if(!((cur<BOARDSIZE)&&(cur>=Coordinate(0,0))&&
             (board[cur.x][cur.y]==REPRESENTATION.at(color)))){
                 break;
            }
            count++;
            i--;
        }
        if(count>=5){
            return true;
        }
        it++;
    }
    return false;
}

bool have_five_at(const vector<vector<int>> &board, string color, int x, int y){
    return have_five_at(board, color, Coordinate(x,y));
}

bool have_active_four_at(const vector<vector<int>> & board, string color, Coordinate xy){
    map<string, Coordinate>::const_iterator it = DIRECTIONS.cbegin();
    int count;
    int i;
    Coordinate cur;
    bool isFree;
    while(it!=DIRECTIONS.cend()){
        count = 0;
        i = 0;
        isFree = true;//whether there are empty spaces on both sides
        while(true){
            cur = xy+multiply(it->second,i);
            if(!((cur<BOARDSIZE)&&(cur>=Coordinate(0,0)))){
               isFree = false;
               break;
            }else if(board[cur.x][cur.y]==REPRESENTATION.at("empty")){
               break;
            }else if(board[cur.x][cur.y]==REPRESENTATION.at(ReverseColor(color))){
               isFree = false;
               break;
            }
            count++;
            i++;
        }
        i = -1;
        while (true){
            cur = xy+multiply(it->second,i);
            if(!((cur<BOARDSIZE)&&(cur>=Coordinate(0,0)))){
               isFree = false;
               break;
            }else if(board[cur.x][cur.y]==REPRESENTATION.at("empty")){
               break;
            }else if(board[cur.x][cur.y]==REPRESENTATION.at(ReverseColor(color))){
               isFree = false;
               break;
            }
            count++;
            i--;
        }
        if((count==4)&&isFree){
            return true;
        }
        it++;
    }
    return false;
}

bool have_active_four_at(const vector<vector<int>> &board, string color, int x, int y){
    return have_active_four_at(board, color, Coordinate(x, y));
}
