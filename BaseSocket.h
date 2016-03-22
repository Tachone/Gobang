#ifndef BASESOCKET_H_H
#define BASESOCKET_H_H

#include <windows.h>
#include "Table.h"
#include "Data.h"

#define  MAX_MESSAGE 1024

typedef void (* RECVFUNC)(char *pMsg, int len);

class CBaseSocket
{
public:
	CBaseSocket();
    virtual ~CBaseSocket();

	virtual bool SendPacket(void* pMsg, int len) = 0;
	virtual bool Initialize(int Port, char* pAddress) = 0;
   
protected:
	bool InitWinSock();

protected:
	SOCKET hSocket;
	static RECVFUNC pRecv;
};

#endif
