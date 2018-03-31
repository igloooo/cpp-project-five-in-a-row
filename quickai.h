#ifndef QUICKAI_H
#define QUICKAI_H

#include "gamemodel.h"


typedef struct
{
    int x,y,l,r;
}value;

class QuickAI:GameModel
{
public:
    QuickAI();
    void set_five_value();
    void get_five_value();
    int chart(int grade);
    Qpoint quick_computerPutdown();

#include "quickai.h"
};

#endif // QUICKAI_H
