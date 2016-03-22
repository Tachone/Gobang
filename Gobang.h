#ifndef CHESS_H_H
#define CHESS_H_H

#include <Windows.h>
#include "Table.h"
#include "resource.h"
#include "BaseSocket.h"
#include "Client.h"
#include "Server.h"

BOOL CALLBACK MainDlg   (HWND, UINT, WPARAM, LPARAM);

void Initialize(HWND hDlg);

void StartChessGame(GAMESTATE GameState);
void StartLocalModeGame();
void StartNetWorkModeGame(GAMESTATE GameState);
void EndChessGame();

void ReDrawGameTable();
void ReDrawGameArea();
void DrawChessGameOver(const char* pMsg, int x, int y, int FontSize);
void DrawOneChessMan(HDC hDC, int x, int y, COLORREF Color);

void MouseButtonDown(int x, int y);

void SetGameChessCuror();

void RecvPktCallBack(char *pMsg, int len);
void ReceiveAcceptMsg();
void ReceiveExitMsg();

void RequestStartGame();
void ResponseStartGame();

void RequestRegretLastStep();
void ResponseRegretLastStep(int Flag);

#endif
