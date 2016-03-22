#include "Gobang.h"

HDC windowDC;
HDC bufferDC;
HDC topDC;

HBRUSH hbrushWall;

int ClientWitdh;
int ClientHeight;

HINSTANCE hInst;
HWND ghDlg;
HWND gSubhDlg;
HMENU hmenu;

CTable gTable;
CBaseSocket *pSocket = NULL;

HCURSOR hNormal;
HCURSOR hPause;

GAME_MODE gMode = LOCAL_MODE;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					PSTR szCmdLine, int iCmdShow)
{
	hInst = hInstance;

	DialogBoxParam (hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, MainDlg, NULL);

	return 0;   
}

BOOL CALLBACK SetNetworkDlgProc (HWND hDlg, UINT message, 
								 WPARAM wParam, LPARAM lParam)
{
	char pAddress[] ="000.000.000.000";

	switch (message)
	{
	case WM_INITDIALOG:
		::CheckRadioButton(hDlg, IDC_RADI_HOST, IDC_RADIO_CLIENT, IDC_RADI_HOST);
		EnableWindow( GetDlgItem(hDlg, IDC_EDIT_IP), FALSE);
		SetDlgItemText(hDlg, IDC_EDIT_IP, "127.0.0.1");
		gSubhDlg = hDlg;
		break;

	case WM_COMMAND :
		switch (LOWORD (wParam))  		            
		{
		case IDC_RADI_HOST:
			EnableWindow( GetDlgItem(hDlg, IDC_EDIT_IP), FALSE);
			break;

		case IDC_RADIO_CLIENT:
			EnableWindow( GetDlgItem(hDlg, IDC_EDIT_IP), TRUE);
			break;

		case IDOK :
			if(IsDlgButtonChecked(hDlg, IDC_RADI_HOST))
			{
				pSocket = new CGameServer(RecvPktCallBack);
				gMode = SERVER_MODE;
				EnableWindow( GetDlgItem(hDlg, IDOK), FALSE);
			}
			else
			{
				pSocket = new CGameClient(RecvPktCallBack);
				gMode = CLIENT_MODE;
				GetDlgItemText(hDlg, IDC_EDIT_IP, pAddress, sizeof(pAddress) );
			}

			if( !pSocket->Initialize(5555, pAddress) )
			{
				gMode = LOCAL_MODE;
				delete pSocket;
				pSocket = NULL;

				MessageBox(hDlg, "网络连接错误!","注意", MB_OK|MB_ICONERROR);
				EnableWindow( GetDlgItem(hDlg, IDOK), TRUE);
			}
			else if (IsDlgButtonChecked(hDlg, IDC_RADIO_CLIENT))
			{
				ReceiveAcceptMsg();
				return TRUE;
			}

			break;

		case IDCANCEL:
			EndDialog (hDlg, 0) ;
			return TRUE ;
		}

		break ;
	}

	return FALSE;
}

BOOL CALLBACK MainDlg (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	SetGameChessCuror();

	switch (message) 
	{
	case WM_INITDIALOG:
		ghDlg = hDlg;
		hmenu  =  GetMenu(hDlg);
		Initialize(hDlg);
		break; 

	case WM_PAINT:
		ReDrawGameArea();
		break; 

	case WM_LBUTTONDOWN:
		if ( gTable.GetRunState() == RUNNING )
		{
			MouseButtonDown(LOWORD(lParam), HIWORD(lParam));
		}
		break; 

	case WM_CLOSE:
		DestroyWindow(hDlg);                               
		break; 

	case WM_COMMAND: 
		switch (LOWORD (wParam))                    
		{
		case ID_CREATE_CONNECT:
			if ( pSocket == NULL )
			{
				DialogBox (hInst, MAKEINTRESOURCE (IDD_DIALOG_CONNECT), hDlg, SetNetworkDlgProc);
			}
			else
			{
				MessageBox(hDlg, "网络已经连接!","注意", MB_OK);
			}

			break;

		case ID_REQUEST_START:
			if ( pSocket != NULL )
			{
				if ( gTable.GetRunState() == RUNNING || gTable.GetRunState() ==WAITTING)
				{
					MessageBox(hDlg, "正在游戏中,不允许退出!","注意", MB_OK);
				}
				else
				{
					RequestStartGame();
				}

			}
			else
			{
				MessageBox(hDlg, "请先建立网络连接,再请求网络对战!","注意", MB_OK);
			}
			break;

		case ID_REQUEST_REGERET:
			if ( gTable.GetRunState() == WAITTING )
			{
				RequestRegretLastStep();
			}
			break;

			//////////////////////////////////////////////////////////////////////////
		case ID_START:
			if ( pSocket != NULL )
			{
				if(MessageBox(ghDlg, "网络连接已经建立,是否断开开始本地游戏?", "提示", MB_YESNO) == IDYES )
				{
					delete pSocket;
					pSocket = NULL;
					StartNetWorkModeGame(WAITTING);
					ResponseStartGame();
				}
			}
			else
			{
				StartLocalModeGame();
			}

			break;

		case ID_END_GAME:
			EndChessGame();
			break;

		case ID_RETURN:
			gTable.RegretLastStep();
			ReDrawGameArea();
			break;

		case IDCANCEL:
			EndDialog (hDlg, 0) ;
			return TRUE;
		}

		break; 

	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_SPACE:
			if( gTable.GetRunState() == GAMEOVER )
			{
				if ( pSocket != NULL )
				{
					RequestStartGame();
				}
				else
				{
					StartLocalModeGame();
				}

				break;
			}
		}

		break; 
	}

	return FALSE;
}

void StartLocalModeGame()
{
	EnableMenuItem(hmenu, ID_CREATE_CONNECT, MF_GRAYED);
	EnableMenuItem(hmenu, ID_REQUEST_START, MF_GRAYED);
	EnableMenuItem(hmenu, ID_REQUEST_REGERET, MF_GRAYED);

	StartChessGame(RUNNING);
}

void StartNetWorkModeGame(GAMESTATE GameState)
{
	//EnableMenuItem(hmenu,ID_START, MF_GRAYED);
	EnableMenuItem(hmenu,ID_END_GAME, MF_GRAYED);
	EnableMenuItem(hmenu,ID_RETURN, MF_GRAYED);

	StartChessGame(GameState);
}

void StartChessGame(GAMESTATE GameState)
{
	gTable.ResetTable();
	gTable.SetRunState(GameState);

	ReDrawGameArea();
}

void EndChessGame()
{
	EnableMenuItem(hmenu,ID_CREATE_CONNECT, MF_ENABLED);
	EnableMenuItem(hmenu,ID_REQUEST_START, MF_ENABLED);
	EnableMenuItem(hmenu, ID_REQUEST_REGERET, MF_ENABLED);

	EnableMenuItem(hmenu,ID_START, MF_ENABLED);
	EnableMenuItem(hmenu,ID_END_GAME, MF_ENABLED);
	EnableMenuItem(hmenu,ID_RETURN, MF_ENABLED);

	gTable.SetRunState(GAMEOVER);

	ReDrawGameArea();
}

void ReDrawGameArea()
{
	ReDrawGameTable();

	for (int i=0; i< NumOfLine; i++)
	{
		for ( int j =0; j < NumOfLine; j++ )
		{
			CChessMan pStatus = gTable.GetChessMan(i, j);

			if ( pStatus.Enable)
			{
				DrawOneChessMan(bufferDC, i, j, pStatus.Color);
			}
		}
	}

	POINT p;
	if ( gTable.GetLastChessMan(p) )
	{
		HPEN hpen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
		SelectObject(bufferDC, hpen);

		MoveToEx(bufferDC, WidthtGap+ p.x*Width, HeightGap+p.y*Width-Width/8, NULL);
		LineTo(bufferDC, WidthtGap+ p.x*Width, HeightGap+p.y*Width-Width/4);

		MoveToEx(bufferDC, WidthtGap+ p.x*Width, HeightGap+p.y*Width+Width/8, NULL);
		LineTo(bufferDC, WidthtGap+ p.x*Width, HeightGap+p.y*Width+Width/4);

		MoveToEx(bufferDC, WidthtGap+ p.x*Width-Width/8, HeightGap+p.y*Width, NULL);
		LineTo(bufferDC, WidthtGap+ p.x*Width-Width/4, HeightGap+p.y*Width);

		MoveToEx(bufferDC, WidthtGap+ p.x*Width+Width/8, HeightGap+p.y*Width, NULL);
		LineTo(bufferDC, WidthtGap+ p.x*Width+Width/4, HeightGap+p.y*Width);

		DeleteObject(hpen);
	}

	if ( gTable.GetRunState() == GAMEOVER )
	{
		switch ( gTable.GetWhiteSideWinFlag() )
		{
		case WHITE_SIDE:
			DrawChessGameOver("白棋胜!",ClientWitdh/2, ClientHeight*2/5, 30);
			break;

		case BLACK_SIDE:
			DrawChessGameOver("黑棋胜!",ClientWitdh/2, ClientHeight*2/5, 30);
			break;

		default:
			DrawChessGameOver("游戏结束!",ClientWitdh/2, ClientHeight*2/5, 30);
		}

		DrawChessGameOver("按空格键可以重新开始!", ClientWitdh/2, ClientHeight/2, 20);
	}

	if ( gTable.GetRunState() == READY && gMode != LOCAL_MODE)
	{
		DrawChessGameOver("请求对方开始游戏!", ClientWitdh/2, ClientHeight*2/5, 20);
	}


	BitBlt(windowDC,0, 0, ClientWitdh, ClientHeight, bufferDC,0,0, SRCCOPY);
}

void  ReDrawGameTable()
{
	HPEN hpen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	SelectObject(bufferDC, hpen);

	//Draw background
	SelectObject(bufferDC, hbrushWall);
	PatBlt(bufferDC, 0, 0, ClientWitdh, ClientHeight,PATCOPY);

	//Draw table top
	BitBlt(bufferDC,0, 0, ClientWitdh, HeightGap, topDC,0,0,SRCCOPY);

	//Draw form
	for (int i =0; i< NumOfLine; i++)
	{
		MoveToEx(bufferDC, WidthtGap, HeightGap+i*Width, NULL);
		LineTo(bufferDC, WidthtGap+(NumOfLine-1)*Width, HeightGap+i*Width);
	}

	for (int i =0; i< NumOfLine; i++)
	{
		MoveToEx(bufferDC, WidthtGap+i*Width, HeightGap, NULL);
		LineTo(bufferDC, WidthtGap+i*Width, HeightGap+(NumOfLine-1)*Width);
	}

	int WidthDis = (NumOfLine-1)/4*Width;
	int HeightDis = (NumOfLine-1)/4*Width;
	int FlagWidth = Width/8;

	HBRUSH hBlackBrush = CreateSolidBrush(RGB(0,0,0));
	SelectObject(bufferDC, hBlackBrush);

	RoundRect(bufferDC, 
		ClientWitdh/2-FlagWidth,
		ClientHeight/2-FlagWidth,
		ClientWitdh/2+FlagWidth,
		ClientHeight/2+FlagWidth,
		0, 0);

	RoundRect(bufferDC, 
		WidthtGap+WidthDis-FlagWidth,
		HeightGap+HeightDis-FlagWidth,
		WidthtGap+WidthDis+FlagWidth,
		HeightGap+HeightDis+FlagWidth,
		0, 0);

	RoundRect(bufferDC, 
		ClientWitdh-WidthtGap-WidthDis-FlagWidth,
		HeightGap+HeightDis-FlagWidth,
		ClientWitdh-WidthtGap-WidthDis+FlagWidth,
		HeightGap+HeightDis+FlagWidth,
		0, 0);

	RoundRect(bufferDC, 
		ClientWitdh-WidthtGap-WidthDis-FlagWidth,
		ClientHeight-HeightGap-HeightDis-FlagWidth,
		ClientWitdh-WidthtGap-WidthDis+FlagWidth,
		ClientHeight-HeightGap-HeightDis+FlagWidth,
		0, 0);

	RoundRect(bufferDC, 
		WidthtGap+WidthDis-FlagWidth,
		ClientHeight-HeightGap-HeightDis-FlagWidth,
		WidthtGap+WidthDis+FlagWidth,
		ClientHeight-HeightGap-HeightDis+FlagWidth,
		0, 0);

	DeleteObject(hpen);
	DeleteObject(hBlackBrush);

	return;
}

void DrawOneChessMan(HDC hDC, int x, int y, COLORREF Color)
{
	HBRUSH hBrush = CreateSolidBrush(Color);

	SelectObject(hDC, hBrush);

	Ellipse(hDC, 
		WidthtGap+x*Width-Width/2,
		HeightGap+y*Width-Width/2,
		WidthtGap+x*Width+Width/2,
		HeightGap+y*Width+Width/2);

	DeleteObject(hBrush);

	return;
}

void DrawChessGameOver(const char* pMsg, int x, int y, int FontSize)
{
	SetBkMode(bufferDC, TRANSPARENT);
	SetTextColor(bufferDC, RGB(128,50,0));
	LOGFONT  LogFont;
	memset(&LogFont, 0, sizeof(LOGFONT));
	LogFont.lfHeight = FontSize;
	LogFont.lfWidth = FontSize;
	LogFont.lfCharSet=GB2312_CHARSET;
	lstrcpy(LogFont.lfFaceName,"隶书");

	HFONT hFont =CreateFontIndirect(&LogFont);
	SelectObject(bufferDC, hFont);
	SetTextAlign(bufferDC, VTA_CENTER);
	TextOut(bufferDC, x, y, pMsg, (int)strlen(pMsg));
	DeleteObject(hFont);
}

void MouseButtonDown(int x, int y)
{
	x += Width/2;
	y += Width/2;

	if ( x <WidthtGap || x > (WidthtGap+NumOfLine*Width)
		|| y <HeightGap || y > (WidthtGap+NumOfLine*Width) )
	{
		return;
	}

	int iTileX = (x-WidthtGap) / Width;
	int iTileY = (y-HeightGap) / Width; 

	if( pSocket != NULL)
	{
		CSendPkt SendPacket;
		SendPacket.Type = MOVE_MSG;
		SendPacket.x = iTileX;
		SendPacket.y = iTileY;

		if ( pSocket->SendPacket(&SendPacket, sizeof(SendPacket) ))
		{
			gTable.SetRunState(WAITTING);
		}
		else
		{
			MessageBox(ghDlg, "发送数据失败!", "错误", NULL);
		}
	}

	if(gTable.SetChessMan(iTileX, iTileY))
	{
		EndChessGame();
	}

	ReDrawGameArea();

	return;
}

void SetGameChessCuror()
{
	SetCursor(hNormal);

	/*if ( (gMode == LOCAL_MODE && gTable.GetRunState() == READY)
	|| gTable.GetRunState() == RUNNING  )
	{
	SetCursor(hNormal);
	}
	else
	{
	SetCursor(hPause);
	}*/
}

void UnPackMoveMsg(char* pMsg, int len)
{
	if ( len < 12 )
	{
		return;
	}

	CSendPkt *p = (CSendPkt*)pMsg;

	if ( p->x <0 || p->x > NumOfLine || p->y <0 || p->y > NumOfLine)
	{
		MessageBox(ghDlg, "接收数据不正确","提示", MB_OK);
		return;
	}

	gTable.SetRunState(RUNNING);
	gTable.SetChessMan(p->x, p->y);

	ReDrawGameArea();
}

void ReceiveAcceptMsg()
{
	EndDialog (gSubhDlg, 0);
	gTable.ResetTable();
	gTable.SetRunState(READY);

	//EnableMenuItem(hmenu,ID_START, MF_GRAYED);
	EnableMenuItem(hmenu,ID_END_GAME, MF_GRAYED);
	EnableMenuItem(hmenu,ID_RETURN, MF_GRAYED);

	ReDrawGameArea();

	return;
}

void ReceiveExitMsg()
{
	delete pSocket;
	pSocket = NULL;
	gMode = LOCAL_MODE;

	EnableMenuItem(hmenu,ID_START, MF_ENABLED);
	EnableMenuItem(hmenu,ID_END_GAME, MF_ENABLED);
	EnableMenuItem(hmenu,ID_RETURN, MF_ENABLED);

	MessageBox(ghDlg, "对方退出游戏,网络连接已断开!", "五子棋", MB_OK);
	EndChessGame();

	return;
}

void UnPackChatMsg(char* pMsg, int len)
{
	char* ShowMsg = (char*)(pMsg+4);

	MessageBox(ghDlg, ShowMsg, "消息", MB_OK);

	return;
}

void RecvPktCallBack(char *pMsg, int len)
{
	switch( *(MSGTYPE*)pMsg )
	{
	case ACCEPT_MSG:
		ReceiveAcceptMsg();
		break;

	case REQUEST_MSG:
		if(MessageBox(ghDlg, "对方请求游戏，是否开始?", "提示", MB_YESNO) == IDYES )
		{
			StartNetWorkModeGame(WAITTING);
			ResponseStartGame();
		}
		break;

	case REQUEST_REGRET:
		if(MessageBox(ghDlg, "对方请求悔棋，是否同意?", "提示", MB_YESNO) == IDYES )
		{
			ResponseRegretLastStep(1);
			gTable.RegretLastStep();
			gTable.SetRunState(WAITTING);
			ReDrawGameArea();
			break;
		}
		else
		{
			ResponseRegretLastStep(0);
		}
		break;

	case RESPONSE_REGRET:
		gTable.RegretLastStep();
		gTable.SetRunState(RUNNING);
		ReDrawGameArea();
		break;

	case RESPONSE_MSG:
		StartNetWorkModeGame(RUNNING);
		break;

	case MOVE_MSG:
		UnPackMoveMsg(pMsg, len);
		break;

	case CHAT_MSG:
		break;

	case EXIT_MSG:	
		ReceiveExitMsg();
		break;

	default:
		break;
	}

	return;
}

void RequestStartGame()
{
	MSGTYPE MsgType = REQUEST_MSG;

	if( pSocket != NULL && !pSocket->SendPacket(&MsgType, sizeof(MsgType) ))
	{
		MessageBox(ghDlg, "请求对方开始游戏,发送数据失败!", "错误", NULL);
		return;
	}

	return;
}

void ResponseStartGame()
{
	MSGTYPE MsgType = RESPONSE_MSG;

	if( pSocket != NULL && !pSocket->SendPacket(&MsgType, sizeof(MsgType) ))
	{
		MessageBox(ghDlg, "回应对方开始游戏,发送数据失败!!", "错误", NULL);
		return;
	}

	return;
}

void RequestRegretLastStep()
{
	POINT p;
	if ( !gTable.GetLastChessMan(p) )
	{
		return;
	}

	CSendPkt SendPkt;
	SendPkt.Type = REQUEST_REGRET;
	SendPkt.x = p.x;
	SendPkt.y = p.y;

	if( pSocket != NULL && !pSocket->SendPacket(&SendPkt, sizeof(SendPkt) ))
	{
		MessageBox(ghDlg, "请求对方悔棋,发送数据失败!!", "错误", NULL);
		return;
	}

	return;
}

void ResponseRegretLastStep(int Flag)
{
	CRegretPkt SendPkt;
	SendPkt.Type = RESPONSE_REGRET;
	SendPkt.Flag = Flag;//0拒绝 1同意

	if( pSocket != NULL && !pSocket->SendPacket(&SendPkt, sizeof(SendPkt) ))
	{
		MessageBox(ghDlg, "回应对方悔棋,发送数据失败!!", "错误", NULL);
		return;
	}

	return;
}

void Initialize(HWND hDlg)
{
	ClientWitdh = WidthtGap*2 + (NumOfLine-1)*Width;
	ClientHeight = HeightGap*2 + (NumOfLine-1)*Width;

	int WindowWitdh = ClientWitdh+ GetSystemMetrics(SM_CXFIXEDFRAME) * 2 ;
	int WindowHeight = ClientHeight+GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + 
		GetSystemMetrics(SM_CYCAPTION);

	MoveWindow(hDlg, StartxPos, StartyPos, WindowWitdh, WindowHeight, true);

	SendMessage(hDlg,WM_SETICON,ICON_BIG,LPARAM(LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON1))));

	windowDC=GetDC(hDlg);

	bufferDC = CreateCompatibleDC(windowDC);
	topDC= CreateCompatibleDC(windowDC);

	HBITMAP topBMP = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_TOP_BITMAP),IMAGE_BITMAP,
		ClientWitdh,HeightGap-20,LR_DEFAULTCOLOR);
	SelectObject(topDC,topBMP);

	HBITMAP bufferBMP=CreateCompatibleBitmap(windowDC,ClientWitdh,ClientHeight);
	SelectObject(bufferDC, bufferBMP);

	HBITMAP  hbmpWall= (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BACK_BITMAP),IMAGE_BITMAP,0,0,LR_DEFAULTCOLOR);
	hbrushWall = CreatePatternBrush(hbmpWall);

	hNormal = (HCURSOR)LoadImage(hInst, MAKEINTRESOURCE(IDC_CURSOR_NORMAL),IMAGE_CURSOR,0,0,LR_DEFAULTCOLOR);
	hPause = (HCURSOR)LoadImage(hInst, MAKEINTRESOURCE(IDC_CURSOR_PAUSE),IMAGE_CURSOR,20,20,LR_DEFAULTCOLOR);

	return;
}
