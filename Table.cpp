#include "Table.h"

CTable::CTable()
{
    ResetTable(); 
}

void CTable::ResetTable()
{
    while( !RecordStack.empty() )
    {
        RecordStack.pop();
    }

    for ( int i=0; i< NumOfLine; i++)
    {
        for ( int j=0; j < NumOfLine; j++)
        {
            pChessMan[i][j].Reset();
        }
    }

    Color = BLACK;
    State = READY;
    Winner = OTHER_SIDE;
}

bool CTable::SetChessMan(int x, int y)
{
    if ( pChessMan[x][y].Enable )
    {
        return false;
    }
   
    pChessMan[x][y].Enable = true;
    pChessMan[x][y].Color = Color;

    POINT p;
    p.x = x;
    p.y = y;

    RecordStack.push(p);

    if ( JudgeWinner( x, y) )
    {
        SetRunState(GAMEOVER);

        Color == WHITE ? Winner = WHITE_SIDE : Winner = BLACK_SIDE;

		return true;
    }

    ChangeState();

	return false;
}

CChessMan CTable::GetChessMan(int x, int y) const
{
    return pChessMan[x][y];
}

GAMESTATE CTable::GetRunState() const
{
    return State;
}

void CTable::SetRunState(GAMESTATE State)
{
    this->State = State;
}

bool CTable::GetLastChessMan(POINT &p) const
{
    if ( RecordStack.empty() || State == GAMEOVER )
    {
        return false;
    }

    p = RecordStack.top();

    return true;
}

void CTable::RegretLastStep()
{
    if ( RecordStack.empty() || State == GAMEOVER )
    {
        return;
    }

    POINT p = RecordStack.top();
    RecordStack.pop();

    Color = pChessMan[p.x][p.y].Color; 
    pChessMan[p.x][p.y].Reset();
}

bool CTable::JudgeWinner(int x, int y) const
{
    return  JudgeRowChessMan(x, y) ||
        JudgeColumnChessMan(x, y)||
        JudgeLeftChessMan(x, y) ||
        JudgeRightChessMan(x, y);
}

bool CTable::JudgeRowChessMan(int x, int y) const
{
    CChessMan Tmp[NumOfLine];

    for ( int i=0; i< NumOfLine; i++)
    {
        Tmp[i] = pChessMan[i][y];
    }

    return CalcChessmanNum(Tmp, x);
}

bool CTable::JudgeColumnChessMan(int x, int y) const
{
    CChessMan Tmp[NumOfLine];

    for ( int i=0; i< NumOfLine; i++)
    {
        Tmp[i] = pChessMan[x][i];
    }

    return CalcChessmanNum(Tmp, y);
}

bool CTable::JudgeLeftChessMan(int x, int y) const
{
    CChessMan Tmp[NumOfLine];

    int Pos = x > y ? y : x ;
    int Startx = (x-Pos) >0 ? (x-Pos) : 0;
    int Starty = (y-Pos) >0 ? (y-Pos) : 0;

    int i = 0;
    while ( Startx < NumOfLine && Starty < NumOfLine )
    {
        Tmp[i++] = pChessMan[Startx][Starty];
        Startx++;
        Starty++;
    }

    return CalcChessmanNum(Tmp, Pos);
}

bool CTable::JudgeRightChessMan(int x, int y) const
{
    CChessMan Tmp[NumOfLine];

    int Tmpx = NumOfLine-x;
    int Pos = Tmpx > y ? y : Tmpx ;
    int Startx = (x+Pos) >NumOfLine ? NumOfLine : (x+Pos);
    int Starty = (y-Pos) >0 ? (y-Pos) : 0;

    int i = 0;
    while ( Startx >= 0 && Starty < NumOfLine )
    {
        Tmp[i++] = pChessMan[Startx][Starty];
        Startx--;
        Starty++;
    }

    return CalcChessmanNum(Tmp, Pos);
}

bool CTable::CalcChessmanNum(CChessMan* pChessMan, int Pos) const
{
    int Sum = 0;
    int Startx = Pos-1;

    while ( Startx >=0 && pChessMan[Startx].Enable 
        && pChessMan[Startx].Color == pChessMan[Pos].Color )
    {
        Sum++;
        Startx--;
    }

    Startx = Pos+1;

    while ( Startx < NumOfLine && pChessMan[Startx].Enable 
        && pChessMan[Startx].Color == pChessMan[Pos].Color )
    {
        Sum++;
        Startx++;
    }

    if ( Sum >= 4 )
    {
        return true;
    }

    return false;
}

void CTable::ChangeState()
{
    if ( Color == WHITE )
    {
        Color = BLACK;
    }
    else
    {
        Color = WHITE;
    }

    return;
}

WINNER_SIDE CTable::GetWhiteSideWinFlag() const
{
    return Winner;
}

