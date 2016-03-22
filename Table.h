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
    COLORREF  Color;   //��ǰ�����������ɫ
    GAMESTATE  State;   //��ǰ��Ϸ״̬Runing,Waiting,GameOver

    CChessMan pChessMan[NumOfLine][NumOfLine];   //��ʼ����������������
    stack<POINT> RecordStack;   //��¼����켣����ҪΪ����ʹ��

    WINNER_SIDE Winner;    //����������ķ���ʤ
};

#endif 
