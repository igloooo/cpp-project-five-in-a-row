#include "gametreenode.h"
#include "gamemodel.h"
#include "coordinate.h"
#include <iostream>
#include <string>
#include <ostream>
using namespace std;


GameTreeNode::GameTreeNode(int x, int y, string color){
    set_xy(x, y);
    set_color(color);

    state_value = 0;
    parent = NULL;
    sibling_h = NULL;
    sibling_t = NULL;
    child = NULL;
}

GameTreeNode::GameTreeNode(Coordinate xy, string color){
    set_xy(xy.x, xy.y);
    set_color(color);

    state_value = 0;
    parent = NULL;
    sibling_h = NULL;
    sibling_t = NULL;
    child = NULL;
}

bool GameTreeNode::hasParent(){
    return (parent!=NULL);
}
bool GameTreeNode::hasSiblingH(){
    return (sibling_h!=NULL);
}
bool GameTreeNode::hasSiblingT(){
    return (sibling_t!=NULL);
}
bool GameTreeNode::hasChild(){
    return (child!=NULL);
}



GameTreeNode & GameTreeNode::get_parent(){
    if(parent==NULL){
        throw "The node has no parent";
    }else{
        return *parent;
    }
}
GameTreeNode & GameTreeNode::get_sibling_h(){
    if(parent==NULL){
        throw "The node has no head side sibling";
    }else{
        return *sibling_h;
    }
}
GameTreeNode & GameTreeNode::get_sibling_t(){
    if(parent==NULL){
        throw "The node has no tail side sibling";
    }else{
        return *sibling_t;
    }
}
GameTreeNode & GameTreeNode::get_child(){
    if(child==NULL){
        throw "The node has no child";
    }else{
        return *child;
    }
}
void GameTreeNode::set_parent(GameTreeNode & node){
    parent = &node;
}
void GameTreeNode::set_sibling_h(GameTreeNode & node){
    sibling_h = &node;
}
void GameTreeNode::set_sibling_t(GameTreeNode & node){
    sibling_t = &node;
}
void GameTreeNode::set_child(GameTreeNode & node){
    child = &node;
}


int GameTreeNode::get_x(){
    return x;
}
int GameTreeNode::get_y(){
    return y;
}
string GameTreeNode::get_color(){
    return color;
}
int GameTreeNode::get_state_value(){
    return state_value;
}
void GameTreeNode::set_state_value(int value){
    state_value = value;
}


GameTreeNode & GameTreeNode::ExpandSiblingH(int x, int y){
    GameTreeNode *p_new_node = new GameTreeNode(x, y, color);
    if(hasSiblingH()){
        GameTreeNode *p_head_side_node = sibling_h;
        sibling_h = p_new_node;
        p_head_side_node->set_sibling_t(*p_new_node);
        p_new_node->set_sibling_h(*p_head_side_node);
        p_new_node->set_sibling_t(*this);
        p_new_node->set_parent(*parent);
    }else{
        sibling_h = p_new_node;
        parent->set_child(*p_new_node);
        p_new_node->set_sibling_t(*this);
        p_new_node->set_parent(*parent);
    }
    return *p_new_node;
}
GameTreeNode & GameTreeNode::ExpandSiblingH(Coordinate xy){
    return ExpandSiblingH(xy.x, xy.y);
}

GameTreeNode & GameTreeNode::ExpandSiblingT(int x, int y){
    GameTreeNode *p_new_node = new GameTreeNode(x, y, color);
    if(hasSiblingT()){
        GameTreeNode *p_tail_side_node = sibling_t;
        sibling_t = p_new_node;
        p_tail_side_node->set_sibling_h(*p_new_node);
        p_new_node->set_sibling_h(*this);
        p_new_node->set_sibling_t(*p_tail_side_node);
        p_new_node->set_parent(*parent);
    }else{
        sibling_t = p_new_node;
        p_new_node->set_sibling_h(*this);
        p_new_node->set_parent(*parent);
    }
    return *p_new_node;
}
GameTreeNode & GameTreeNode::ExpandSiblingT(Coordinate xy){
    return ExpandSiblingT(xy);
}

GameTreeNode & GameTreeNode::ExpandChild(int x, int y){
    if(hasChild()){
        return child->ExpandSiblingH(x, y);
    }else{
        GameTreeNode *p_new_node = new GameTreeNode(x, y, ReverseColor(color));
        child = p_new_node;
        p_new_node->set_parent(*this);
        return *p_new_node;
    }
}
GameTreeNode & GameTreeNode::ExpandChild(Coordinate xy){
    return ExpandChild(xy.x, xy.y);
}

GameTreeNode & GameTreeNode::GetNewState(int x, int y){
    if(!hasChild()){
        return ExpandChild(x, y);
    }else{
        for(GameTreeNode *cur=child; cur!=NULL; cur = &(cur->get_sibling_t())){
            cout<<*cur<<endl;
            if(((cur->get_x())==x) && ((cur->get_y())==y)){
                return *cur;
            }
        }
        return ExpandChild(x, y);
    }
}
GameTreeNode & GameTreeNode::GetNewState(Coordinate xy){
    return GetNewState(xy.x, xy.y);
}

string GameTreeNode::toString(){
    string str;
    str += "position: (" + to_string(x) + ","
          + to_string(y) + ")\n";
    str += "color: " + color + "\n";
    str += "state value: " + to_string(state_value) + "\n";
    return str;
}

void GameTreeNode::set_xy(int x, int y){
    if((x<0)||(x>=BOARDSIZEX)||(y<0)||(y>=BOARDSIZEY)){
        cerr<<"invalid xy when initializing a node"<<endl;
        throw to_string(x)+","+to_string(y);
    }else{
        this->x = x;
        this->y = y;
    }
}
void GameTreeNode::set_color(string color){
    if((color!="white")&&(color!="black")){
        cerr<<"invalid color when initializing a node"<<endl;
        cerr<<"\'color\' argument should be \'white\' or \'black\'"<<endl;
        throw color;
    }else{
        this->color = color;
    }
}


ostream & operator<<(ostream &os, GameTreeNode & node){
    return os<<node.toString();
}

