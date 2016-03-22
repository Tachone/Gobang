#include "Server.h"

vector<SOCKET> CGameServer::ClientVec(0);

CGameServer::CGameServer(RECVFUNC pRecv)
{
    this->pRecv = pRecv;
	ClientVec.clear();
}

CGameServer::~CGameServer()
{
	closesocket(hSocket);
}

bool CGameServer::SendPacket(void* pMsg, int len)
{
	for ( vector<SOCKET>::iterator Itor = ClientVec.begin(); 
		Itor != ClientVec.end(); Itor++)
	{
		int nBytes = send(*Itor, (const char*)pMsg, len, 0);

		if ( nBytes == SOCKET_ERROR )
		{
			return false;
		} 
	}

	return true;
}

bool CGameServer::Initialize( int Port, char* pAddress )
{
	if ( !InitWinSock())
	{
		return false;
	}

	hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(Port);
	ServerAddr.sin_addr.s_addr = INADDR_ANY;

	int Ret = bind(hSocket, (sockaddr*)&ServerAddr, sizeof(ServerAddr));
	if ( Ret == SOCKET_ERROR)
	{
		return false;
	}
    
	Ret = listen(hSocket, 5);
    if ( Ret == SOCKET_ERROR )
    {
		return false;
    }

	HANDLE hThread = CreateThread(NULL, 0, AcceptThread, (LPVOID)this, 0, 0);

	if ( hThread == NULL) 
	{
		return false;
	}

	CloseHandle(hThread);
    
	return true;
}

DWORD WINAPI CGameServer::AcceptThread( LPVOID lpPara )
{
	CGameServer* pServer = (CGameServer*)lpPara;

	//while ( true )
	{
        sockaddr_in ClientAddr;
		int len = sizeof(ClientAddr);
		SOCKET hClient = accept(pServer->hSocket, (sockaddr*)&ClientAddr, &len);
		if ( hClient == INVALID_SOCKET )
		{
			cout << "Accept error "<< GetLastError() << endl;
			//break;
			return 0;
		}
        
		HANDLE hThread = CreateThread(NULL, 0, RecvThread, (LPVOID)hClient, 0, 0);

		if ( hThread == NULL) 
		{
			//break;
			return 0;
		}

        ClientVec.push_back(hClient);

		CloseHandle(hThread);
        
		MSGTYPE Type = ACCEPT_MSG;
		pRecv( (char*)&Type,  sizeof(Type));
	}

	return 0;
}

DWORD WINAPI CGameServer::RecvThread( LPVOID lpPara )
{
	SOCKET hClient = (SOCKET)lpPara;

	char pRecvMsg[MAX_MESSAGE] ={0};

	while (true)
	{
		memset(pRecvMsg, 0x00, sizeof(pRecvMsg) );

		int nBytes = recv(hClient, pRecvMsg, sizeof(pRecvMsg), 0);
		if ( nBytes == SOCKET_ERROR || nBytes == 0 )
		{
			for ( vector<SOCKET>::iterator Itor = ClientVec.begin(); 
				Itor != ClientVec.end(); Itor++)
			{
                if ( *Itor == hClient )
                {
					ClientVec.erase(Itor);
					break;
                }
			}

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
