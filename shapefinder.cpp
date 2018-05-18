#include "shapefinder.h"
#include "gamemodel.h"
#include "coordinate.h"
#include <map>
#include <vector>
using namespace std;

bool have_five_at(const vector<vector<int>> & board, string color, Coordinate xy){
    map<string, Coordinate>::const_iterator it = DIRECTIONS.cbegin();
    while(it!=DIRECTIONS.cend()){
        if(have_five_at(board, color, xy, it->first)){
            return true;
        }
        it++;
    }
    return false;
}

bool have_five_at(const vector<vector<int> > &board, string color, Coordinate xy,
                string direc, Stone new_move){
    int count=0;
    int i=0;
    Coordinate cur;
    while(true){
        cur = xy+multiply(DIRECTIONS.at(direc),i);
        bool inRange = (cur<BOARDSIZE)&&(cur>=Coordinate(0,0));
        if(!inRange){
             break;
        }else{
            bool sameColor = board[cur.x][cur.y]==REPRESENTATION.at(color);
            if(new_move.color!="empty"){
                sameColor = sameColor||(Stone(cur,color)==new_move);
            }
            if(!sameColor){
                break;
            }
        }
        count++;
        i++;
    }
    i = -1;
    while(true){
        cur = xy+multiply(DIRECTIONS.at(direc),i);
        bool inRange = (cur<BOARDSIZE)&&(cur>=Coordinate(0,0));
        if(!inRange){
             break;
        }else{
            bool sameColor = board[cur.x][cur.y]==REPRESENTATION.at(color);
            if(new_move.color!="empty"){
                sameColor = sameColor||(Stone(cur,color)==new_move);
            }
            if(!sameColor){
                break;
            }
        }
        count++;
        i--;
    }
    if(count>=5){
        return true;
    }else{
        return false;
    }
}

bool have_five_at(const vector<vector<int>> &board, string color, int x, int y){
    return have_five_at(board, color, Coordinate(x,y));
}

bool have_active_four_at(const vector<vector<int>> & board, string color, Coordinate xy){
    map<string, Coordinate>::const_iterator it = DIRECTIONS.cbegin();
    while(it!=DIRECTIONS.cend()){
        if(have_active_four_at(board, color, xy, it->first)){
            return true;
        }
        it++;
    }
    return false;
}

bool have_active_four_at(const vector<vector<int> > &board, string color, Coordinate xy,
                        string direc, Stone new_move){
    int count=0;
    int i=0;
    Coordinate cur;
    bool isFree=true;//whether there are empty spaces on both sides
    while(true){
        cur = xy+multiply(DIRECTIONS.at(direc),i);
        bool inRange = (cur<BOARDSIZE)&&(cur>=Coordinate(0,0));
        if(!inRange){
            isFree = false;
            break;
        }else{
            bool sameColor = board[cur.x][cur.y]==REPRESENTATION.at(color);
            if(new_move.color!="empty"){
                sameColor = sameColor||(Stone(cur,color)==new_move);
            }
            if(!sameColor){
               if(board[cur.x][cur.y]==REPRESENTATION.at(ReverseColor(color))){
                   isFree = false;
               }
               break;
            }
        }
        count++;
        i++;
    }
    i = -1;
    while (true){
        cur = xy+multiply(DIRECTIONS.at(direc),i);
        bool inRange = (cur<BOARDSIZE)&&(cur>=Coordinate(0,0));
        if(!inRange){
            isFree = false;
            break;
        }else{
            bool sameColor = board[cur.x][cur.y]==REPRESENTATION.at(color);
            if(new_move.color!="empty"){
                sameColor = sameColor||(Stone(cur,color)==new_move);
            }
            if(!sameColor){
               if(board[cur.x][cur.y]==REPRESENTATION.at(ReverseColor(color))){
                   isFree = false;
               }
               break;
            }
        }
        count++;
        i--;
    }
    if((count==4)&&isFree){
        return true;
    }else{
        return false;
    }
}

bool have_active_four_at(const vector<vector<int>> &board, string color, int x, int y){
    return have_active_four_at(board, color, Coordinate(x, y));
}

/*
 * Implementation Note:
 * This function tries to by trying to make
 * move at influence domain of xy and see
 * if continuous five emerges.
 */
bool have_half_four_at(const vector<vector<int> > &board, string color,
                       Coordinate xy, string direc){
    stack<Coordinate> inf_dom;
    get_influence_domain(inf_dom, board, xy, direc);
    Coordinate cur; //current try
    bool have_half_four = false;
    while(!inf_dom.empty()){
        cur = inf_dom.top();
        inf_dom.pop();
        if(have_five_at(board, color, xy, direc, Stone(cur, color))){
            have_half_four = true;
            break;
        }
    }
    return have_half_four;
}
bool have_half_four_at(const vector<vector<int> > &board, string color, Coordinate xy){
    map<string, Coordinate>::const_iterator it = DIRECTIONS.cbegin();
    while(it!=DIRECTIONS.cend()){
        if(have_half_four_at(board, color, xy, it->first)){
            return true;
        }
        it++;
    }
    return false;
}

int num_of_half_four_at(const vector<vector<int> > &board, string color, Coordinate xy){
    map<string, Coordinate>::const_iterator it = DIRECTIONS.cbegin();
    int count=0;
    while(it!=DIRECTIONS.cend()){
        if(have_half_four_at(board, color, xy, it->first)){
            count++;
        }
        it++;
    }
    return count;
}
int num_of_half_four_at(const vector<vector<int> > &board, string color, int x, int y){
    return num_of_half_four_at(board, color, Coordinate(x, y));
}

bool have_active_three_at(const vector<vector<int> > &board, string color,
                         Coordinate xy, string direc){
    stack<Coordinate> inf_dom;
    get_influence_domain(inf_dom, board, xy, direc);
    Coordinate cur;
    bool have_active_three = false;
    while(!inf_dom.empty()){
        cur = inf_dom.top();
        inf_dom.pop();
        if(have_active_four_at(board, color, xy, direc, Stone(cur, color))){
            have_active_three = true;
            break;
        }
    }
    return have_active_three;
}
bool have_active_three_at(const vector<vector<int> > &board, string color, Coordinate xy){
    map<string, Coordinate>::const_iterator it = DIRECTIONS.cbegin();
    while(it!=DIRECTIONS.cend()){
        if(have_active_three_at(board, color, xy, it->first)){
            return true;
        }
        it++;
    }
    return false;
}

int num_of_active_three_at(const vector<vector<int> > &board, string color, Coordinate xy){
    map<string, Coordinate>::const_iterator it = DIRECTIONS.cbegin();
    int count=0;
    while(it!=DIRECTIONS.cend()){
        if(have_active_three_at(board, color, xy, it->first)){
            count++;
        }
        it++;
    }
    return count;
}
int num_of_active_three_at(const vector<vector<int> > &board, string color, int x, int y){
    return num_of_active_three_at(board, color, Coordinate(x, y));
}


void get_influence_domain(stack<Coordinate> & inf_dom, const vector<vector<int>> &board, Coordinate co){
    map<string, Coordinate>::const_iterator it = DIRECTIONS.cbegin();
    while(it!=DIRECTIONS.cend()){
        get_influence_domain(inf_dom, board, co, it->first);
        it++;
    }
}
void get_influence_domain(stack<Coordinate> &inf_dom, const vector<vector<int> > &board,
                         Coordinate co, string direc){
    for(int n=-4;n<5;n++){
        Coordinate cur = co+multiply(DIRECTIONS.at(direc),n);
        if((cur>=Coordinate(0,0))&&(cur<BOARDSIZE)&&
            board[cur.x][cur.y]==EMPTY_R){
            inf_dom.push(cur);
        }
    }
}


void ShowBoard(const vector<vector<int>> & board, string mode){
    if(mode=="symbol"){
        for(int i=0;i<BOARDSIZEX;i++){
                if(i!=0){
                    cout<<"-----------------------------------------------------------"<<endl;
                }
                for(int j=0;j<BOARDSIZEY;j++){
                    int piece = board[i][j];
                    if(piece==1){
                        cout<<" X ";
                    }else if(piece==0){
                        cout<<"   ";
                    }else{
                        cout<<" O ";
                    }
                    if(j<BOARDSIZEY-1){
                        cout<<"|";
                    }
                 }
                 cout<<endl;
             }
    }else if(mode=="number"){
        for(int i=0;i<BOARDSIZEX;i++){
                if(i!=0){
                    cout<<"-----------------------------------------------------------"<<endl;
                }
                for(int j=0;j<BOARDSIZEY;j++){
                    cout<<" "<<to_string(board[i][j])<<" ";
                    if(j<BOARDSIZEY-1){
                        cout<<"|";
                    }
                 }
                 cout<<endl;
             }
    }

}

void ShowBoard(const int board[BOARDSIZEX][BOARDSIZEY], string mode){
        if(mode=="symbol"){
        for(int i=0;i<BOARDSIZEX;i++){
                if(i!=0){
                    cout<<"-----------------------------------------------------------"<<endl;
                }
                for(int j=0;j<BOARDSIZEY;j++){
                    int piece = board[i][j];
                    if(piece==1){
                        cout<<" X ";
                    }else if(piece==0){
                        cout<<"   ";
                    }else{
                        cout<<" O ";
                    }
                    if(j<BOARDSIZEY-1){
                        cout<<"|";
                    }
                 }
                 cout<<endl;
             }
    }else if(mode=="number"){
        for(int i=0;i<BOARDSIZEX;i++){
                if(i!=0){
                    cout<<"-----------------------------------------------------------"<<endl;
                }
                for(int j=0;j<BOARDSIZEY;j++){
                    cout<<" "<<board[i][j]<<" ";
                    if(j<BOARDSIZEY-1){
                        cout<<"|";
                    }
                 }
                 cout<<endl;
             }
    }
}
