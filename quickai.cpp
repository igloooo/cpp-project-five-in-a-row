#include "quickai.h"
#include "gamemodel.h"

QuickAI::QuickAI()
{

}

int GameModel::chart(int grade)
{
    int temp = 0;
    if(get_who_first()=="white")
    {
        switch(grade)
        {
           case 0: temp=3;break;
           case 4: temp=35;break;
           case 8: temp=800;break;
           case 12: temp=15000;break;
           case 16: temp=800000;break;
           case 5: temp=15;break;
           case 10: temp=400;break;
           case 15: temp=1800;break;
           case 20: temp=100000;break;
           default: temp=0;
        }
     }
    else if(get_who_first()=="black")
    {
        switch(grade)
        {
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

void GameModel::set_five_value()
{
    for(int i=0;i<BOARDSIZE.x;i++)
        for(int j=0;j<BOARDSIZE.y;j++)
        {
            int grade=0;          //x
            if(j<BOARDSIZE.y-4)
            {
                for(int n=0;n<5;n++)
                {
                  grade+=current_board[i][j]+4;
                }
                fiveValue[i][j].y=chart(grade);
            }
            grade=0;             //y
            if(i<BOARDSIZE.x-4)
            {
                for(int n=0;n<5;n++)
                {
                  grade+=current_board[i][j]+4;
                }
                fiveValue[i][j].x=chart(grade);
            }
             grade=0;            //r
            if(i<BOARDSIZE.x-4&&j<BOARDSIZE.y-4)
            {
                for(int n=0;n<5;n++)
                {
                  grade+=current_board[i][j]+4;
                }
                fiveValue[i][j].r=chart(grade);
            }
            grade=0;           //l
            if(j>=4&&i<BOARDSIZE.x-4)
            {
                for(int n=0;n<5;n++)
                {
                 grade+=current_board[i][j]+4;
                }
                fiveValue[i][j].l=chart(grade);
            }
        }
}
void GameModel::get_site_value()
{
    for(int i=0;i<BOARDSIZE.x;i++)
        for(int j=0;j<BOARDSIZE.y;j++)
        {
            siteValue[i][j]=fiveValue[i][j].l+fiveValue[i][j].r+fiveValue[i][j].x+fiveValue[i][j].y;
            for(int n=1;n<5;n++)
            {
                if(j-n>=0)
                    siteValue[i][j]+=fiveValue[i][j-n].y;
                if(i-n>=0)
                    siteValue[i][j]+=fiveValue[i-n][j].x;
                if(i-n>=0&&j-n>=0)
                    siteValue[i][j]+=fiveValue[i-n][j-n].r;
                if(i-n>=0&&j+n<=14)
                    siteValue[i][j]+=fiveValue[i-n][j+n].l;
            }
        }
}

QPoint GameModel::quick_computerPutdown()
{
    int temp=0,x=0,y=0;
    setFiveValue();
    getSiteValue();
    for(int i=0;i<BOARDSIZE.x;i++)
        for(int j=0;j<BOARDSIZE.y;j++)
            {
                if(current_board[i][j]==0)
                    if(siteValue[i][j]>=temp)
                    {
                        temp=siteValue[i][j];
                        x=i;
                        y=j;
                    }

            }

    if(round%2==0)
    {
        int player;
        if(isFirstPlayer(whose_turn)){
            player = 0;
        }else{
            player = 1;
        }
        siteValue[7][7]=player++%2+4;
        chessPoint.push_back(QPoint(7,7));
    }
    else
    {
        int player;
        if(isFirstPlayer(whose_turn)){
            player = 0;
        }else{
            player = 1;
        }
        siteValue[i][j]=player++%2+4;
        chessPoint.push_back(QPoint(x,y));
    }

    return QPoint(x,y);
}
