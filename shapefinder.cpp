#include "shapefinder.h"
#include "gamemodel.h"
#include "coordinate.h"
#include "map.h"
using namespace std;

bool have_five_at(const Grid<int> & board, string color, Coordinate xy){
    Vector<string> keys = DIRECTIONS.keys();
    int count;
    int i;
    Coordinate cur;
    for(string key:keys){
        count = 0;
        i = 0;
        while(true){
            cur = xy+multiply(DIRECTIONS[key],i);
            if(!((cur<BOARDSIZE)&&(cur>=Coordinate(0,0))&&
             (board.get(cur.x, cur.y)==REPRESENTATION[color]))){
                 break;
            }
            count++;
            i++;
        }
        i = -1;
        while(true){
            cur = xy+multiply(DIRECTIONS[key],i);
            if(!((cur<BOARDSIZE)&&(cur>=Coordinate(0,0))&&
             (board.get(cur.x, cur.y)==REPRESENTATION[color]))){
                 break;
            }
            count++;
            i--;
        }
        if(count>=5){
            return true;
        }
    }
    return false;
}

bool have_five_at(const Grid<int> &board, string color, int x, int y){
    return have_five_at(board, color, Coordinate(x,y));
}

bool have_active_four_at(const Grid<int> & board, string color, Coordinate xy){
    Vector<string> keys = DIRECTIONS.keys();
    int count;
    int i;
    Coordinate cur;
    bool isFree;
    for(string key:keys){
        count = 0;
        i = 0;
        isFree = true;//whether there are empty spaces on both sides
        while(true){
            cur = xy+multiply(DIRECTIONS[key],i);
            if(!((cur<BOARDSIZE)&&(cur>=Coordinate(0,0)))){
               isFree = false;
               break;
            }else if(board.get(cur.x, cur.y)==REPRESENTATION["empty"]){
               break;
            }else if(board.get(cur.x, cur.y)==REPRESENTATION[ReverseColor(color)]){
               isFree = false;
               break;
            }
            count++;
            i++;
        }
        i = -1;
        while (true){
            cur = xy+multiply(DIRECTIONS[key],i);
            if(!((cur<BOARDSIZE)&&(cur>=Coordinate(0,0)))){
               isFree = false;
               break;
            }else if(board.get(cur.x, cur.y)==REPRESENTATION["empty"]){
               break;
            }else if(board.get(cur.x, cur.y)==REPRESENTATION[ReverseColor(color)]){
               isFree = false;
               break;
            }
            count++;
            i--;
        }
        if((count==4)&&isFree){
            return true;
        }
    }
    return false;
}

bool have_active_four_at(const Grid<int> &board, string color, int x, int y){
    return have_active_four_at(board, color, Coordinate(x, y));
}
