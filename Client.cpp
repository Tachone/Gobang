#include "Client.h"

CGameClient::CGameClient(RECVFUNC pRecv)
{
    this->pRecv = pRecv; 
}

CGameClient::~CGameClient()
{
    closesocket(hSocket);
}

bool CGameClient::SendPacket(void* pMsg, int len)
{
	int nBytes = send(hSocket, (const char*)pMsg, len, 0);
    
	if ( nBytes == SOCKET_ERROR )
	{
		return false;
	}

	return true;
}

bool CGameClient::Initialize( int Port, char* pAddress )
{
    if ( !InitWinSock())
    {
		return false;
    }

	hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(Port);
	ServerAddr.sin_addr.s_addr = inet_addr(pAddress);

	int Ret = connect(hSocket, (sockaddr*)&ServerAddr, sizeof(ServerAddr));
	if ( Ret == SOCKET_ERROR)
	{
		return false;
	}

	HANDLE hThread = CreateThread(NULL, 0, RecvThread, (LPVOID)hSocket, 0, 0);

	if ( hThread == NULL) 
	{
		return false;
	}

	CloseHandle(hThread);

	return true;
}

DWORD WINAPI CGameClient::RecvThread( LPVOID lpPara )
{
	SOCKET hClient = (SOCKET)lpPara;

	char pRecvMsg[MAX_MESSAGE] ={0};

	while (true)
	{
		memset(pRecvMsg, 0x00, sizeof(pRecvMsg) );

		int nBytes = recv(hClient, pRecvMsg, sizeof(pRecvMsg), 0);
		if ( nBytes == SOCKET_ERROR || nBytes == 0 )
		{
			MSGTYPE Type = EXIT_MSG;
            pRecv( (char*)&Type, sizeof(Type) );
			break;
		}
		else
		{
            pRecv(pRecvMsg, nBytes);
		}
	}

	return 0;
}

