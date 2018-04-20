/*
 * File: gametreenode.h
 * -----------------------------
 * The file defines the interface for GameTreeNode class.
 */


#ifndef GAMETREENODE_H
#define GAMETREENODE_H
#include "gamemodel.h"
#include <iostream>
#include <string>
#include <ostream>
using namespace std;

/*
 * Class: GameTreeNode
 * --------------------------
 * This class represents a game tree. Note that the game tree currently uses
 * double linked lists to store children.
 */
class GameTreeNode
{
public:
    GameTreeNode(int x, int y, string color);
    GameTreeNode(int x, int y, string color, int board_size_x, int board_size_y);
    //~GameTreeNode();
    bool hasParent();
    bool hasSiblingH();
    bool hasSiblingT();
    bool hasChild();
    GameTreeNode & get_parent();
    GameTreeNode & get_sibling_h();
    GameTreeNode & get_sibling_t();
    GameTreeNode & get_child();
    void set_parent(GameTreeNode & parent);
    void set_sibling_h(GameTreeNode & sibling);
    void set_sibling_t(GameTreeNode & sibling);
    void set_child(GameTreeNode & child);
    int get_x();
    int get_y();
    string get_color();
    int get_state_value();
    void set_state_value(int value);
    GameTreeNode & ExpandSiblingH(int x, int y);
    GameTreeNode & ExpandSiblingT(int x, int y);
    GameTreeNode & ExpandChild(int x, int y);
    //Use this method when trying to take a move
    GameTreeNode & GetNewState(int x, int y);
    string toString();
private:
    int BOARDSIZEX;  //15*15 by default
    int BOARDSIZEY;
    int x;
    int y;
    string color;
    int state_value;
    GameTreeNode *parent;
    GameTreeNode *sibling_h;
    GameTreeNode *sibling_t;
    GameTreeNode *child;
    void set_xy(int x, int y);
    void set_color(string color);
};
ostream & operator<<(ostream &os, GameTreeNode & node);

#endif // GAMETREENODE_H
