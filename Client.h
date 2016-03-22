#ifndef CLIENT_H_H
#define CLIENT_H_H

#include "BaseSocket.h"
#include <iostream>

using namespace std;

class CGameClient:public CBaseSocket
{
public:
    CGameClient(RECVFUNC pRecv);
    ~CGameClient();

	bool SendPacket(void* pMsg, int len);
	bool Initialize(int Port, char* pAddress);

private:
	static DWORD WINAPI RecvThread(LPVOID lpPara);
};

#endif
