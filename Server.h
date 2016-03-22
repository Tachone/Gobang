#ifndef SERVER_H_H
#define SERVER_H_H

#include "BaseSocket.h"
#include <iostream>
#include <vector>
using namespace std;

class CGameServer:public CBaseSocket
{
public:
	CGameServer(RECVFUNC pRecv);
    ~CGameServer();

	bool SendPacket(void* pMsg, int len);
	bool Initialize(int Port, char* pAddress);

private:
	static DWORD WINAPI RecvThread(LPVOID lpPara);
	static DWORD WINAPI AcceptThread(LPVOID lpPara);

private:
	static vector<SOCKET> ClientVec;
};

#endif
