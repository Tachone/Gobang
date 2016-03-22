#ifndef DATA_H_H
#define DATA_H_H

#include <stack>
#include <Windows.h>

const int Port = 8888;
const int StartxPos = 200;
const int StartyPos = 100;
const int Width  = 30;
const int NumOfLine =19;
const int WidthtGap = 80;
const int HeightGap = 80;
const int MaxLen = 1000;

enum GAMESTATE
{
    READY    = 0,
    RUNNING  = 1,
    GAMEOVER = 2,
	WAITTING = 3,
};

enum WINNER_SIDE
{
    WHITE_SIDE = 0,
	BLACK_SIDE = 1,
	OTHER_SIDE = 2,
};

enum GAME_MODE
{
	LOCAL_MODE  = 0,
	SERVER_MODE = 1,
	CLIENT_MODE = 2,
};

enum MSGTYPE
{
	REQUEST_MSG  = 0,
	RESPONSE_MSG = 1,
	ACCEPT_MSG   = 2,
	MOVE_MSG     = 3,
	REGRET_MSG   = 4,
	CHAT_MSG     = 5,
	EXIT_MSG     = 6,
	REQUEST_REGRET = 7,
    RESPONSE_REGRET = 8,
};

#define  BLACK  RGB(0,0,0)
#define  WHITE  RGB(255,255,255)

class CChessMan
{
public:
    CChessMan()
    {
        Reset();
    }

    void Reset()
    {
        Enable = false;
        Color = BLACK;
    }

    bool Enable;
    COLORREF Color;
};

#pragma pack(1)

typedef struct CSendPkt
{
    MSGTYPE Type;
	int x;
	int y;
}CSendPkt;

typedef struct CRegretPkt
{
	MSGTYPE Type;
	int Flag;
}CRegretPkt;

typedef struct CChatPkt
{
    MSGTYPE Type;
	char  Msg[MaxLen];
}CChatPkt;

#pragma pack()

#endif
