#ifndef TABLE_H_H
#define TABLE_H_H

#include <stack>
#include "Data.h"

using namespace std;

class CTable
{
public:
    CTable();

    void ResetTable();
    
    bool SetChessMan(int x, int y);
    CChessMan GetChessMan(int x, int y) const;

    GAMESTATE  GetRunState() const;
    void SetRunState(GAMESTATE State);

    bool GetLastChessMan(POINT &p) const;

    void RegretLastStep();

    WINNER_SIDE GetWhiteSideWinFlag() const;

	void SetGameMode( GAME_MODE Mode );

private:
    bool JudgeWinner(int x, int y) const;
    
    bool JudgeRowChessMan(int x, int y) const;
    bool JudgeColumnChessMan(int x, int y) const;
    bool JudgeLeftChessMan(int x, int y) const;
    bool JudgeRightChessMan(int x, int y) const;
    
    bool CalcChessmanNum(CChessMan* pChessMan, int Pos) const;
    
    void ChangeState();

private:
    COLORREF  Color;   //当前下棋的棋子颜色
    GAMESTATE  State;   //当前游戏状态Runing,Waiting,GameOver

    CChessMan pChessMan[NumOfLine][NumOfLine];   //初始化棋盘上所有棋子
    stack<POINT> RecordStack;   //记录下棋轨迹，主要为悔棋使用

    WINNER_SIDE Winner;    //白棋与黑棋哪方获胜
};

#endif 
