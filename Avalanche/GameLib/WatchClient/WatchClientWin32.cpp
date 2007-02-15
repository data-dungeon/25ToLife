//===================================================================
//  File:  WatchClientWin32.cpp
//  Description:
//     Implementation of the Win32 specific WatchClient stuff.
//===================================================================

#if defined (WIN32) && !defined (_XBOX)
#include <stdio.h>
#include "Layers/Debug.h"

#include "WatchClient/WatchClientWin32.h"
#ifdef ENABLE_WATCHCLIENT
#include "WatchInfo.h"

WatchClientWin32::WatchClientWin32()
{
	s2 = INVALID_SOCKET;
	MEM_SET_ONE_SHOT_NEW_NAME("sockaddr_in");
	server = new sockaddr_in;
	Init();
}

WatchClientWin32::~WatchClientWin32()
{
	delete server;
	closesocket(s2);
}

void WatchClientWin32::Start()
{
	STARTUPINFO start;
	ZeroMemory(&start, sizeof(STARTUPINFO));
	start.cb = sizeof(STARTUPINFO);
	start.wShowWindow = SW_SHOWNOACTIVATE;
	start.dwFlags = STARTF_USESHOWWINDOW;

// rhn, this line starts the watch client automatically, which we don't want right now.
//	::CreateProcess(0, "Watch.exe PC", 0, 0, true, 0, 0, 0, &start, &pinfo);
}

void WatchClientWin32::End()
{
	// Let the watch program know that we're done.
//	SendMessage(HWND_BROADCAST, RegisterWindowMessage("WM_RASHDONE"), 0, 0);
	PostMessage(HWND_BROADCAST, RegisterWindowMessage("WM_RASHDONE"), 0, 0);
}

//----------------------------------------------------------------------
// WatchClient::InitWIN32
//    Initialization for Win 32 client.
//----------------------------------------------------------------------
bool WatchClientWin32::Init()
{
	//---------------------------------------------
	//  Startup the network.
	//---------------------------------------------
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	//---------------------------------------------
	//  Create the socket.
	//---------------------------------------------
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		char msg[256];
		sprintf(msg, "socket client fail:  %d\n", WSAGetLastError());
		conPuts(msg);
		return false;
	}

	//---------------------------------------------
	//  Modify socket to non_block
	//---------------------------------------------
    unsigned long non_block = 1;
    if (SOCKET_ERROR == ioctlsocket(s, FIONBIO, &non_block))
	{
		char msg[256];
		sprintf(msg, "ioctlsocket() call failed", WSAGetLastError());
		conPuts(msg);

        closesocket(s);
		return false;
    }

	//---------------------------------------------
	//  Bind to socket.
	//---------------------------------------------
	char* localHost = "127.0.0.1";
	u_short port = 8510;

	ZeroMemory((char *)server, sizeof(struct sockaddr_in));

	server->sin_family = AF_INET;
	server->sin_addr.s_addr = INADDR_ANY;
	server->sin_port = htons(port);
	
	if (SOCKET_ERROR == bind(s, (const struct sockaddr *)server, 16))
	{
		char msg[256];
		sprintf(msg, "client binding fail:  %d\n", WSAGetLastError());
		conPuts(msg);

        closesocket(s);
		return false;
	}

    if (SOCKET_ERROR == listen(s, 1))
	{
		char msg[256];
		sprintf(msg, "listen fail:  %d\n", WSAGetLastError());
		conPuts(msg);

        closesocket(s);
		return false;
	}
	
	return true;
}

void WatchClientWin32::CheckConnection()
{
	if (INVALID_SOCKET == s2)
	{
		int addrlen = 16;

		s2 = accept(s, (struct sockaddr *)server, &addrlen);
	}
}

//----------------------------------------------------------------------
// WatchClient32::Run
//    This is called in each run loop of the application.
//----------------------------------------------------------------------
void WatchClientWin32::Run()
{
	WatchClient::Run();

	CheckConnection();

	if (INVALID_SOCKET == s2)
		return;
	
	char recvbuf[256];
	unsigned long nbytes;
	ioctlsocket(s2, FIONREAD, &nbytes);
	while ( nbytes >= sizeof(PKT) )
	{
		int n, recvlen;
		for ( recvlen = 0 ; recvlen < sizeof(PKT) ; recvlen += n )
		{
			n = recv(s2, recvbuf + recvlen, sizeof(PKT) - recvlen, 0);
			if ( n == SOCKET_ERROR )
			{
				ASSERT( false );
				break;
			}
		}
		Packet((PKT*)recvbuf);
		ioctlsocket(s2, FIONREAD, &nbytes);
	}
}

bool WatchClientWin32::SendPacket(int type, int length, char* buf)
{
	PKT pkt;
	pkt.deci2.length = sizeof(PKT);
	pkt.pktType = type;
	ASSERT( length <= sizeof( pkt.text ) );
	memcpy(pkt.text, buf, length);
	int r = send(s2, (const char*)&pkt, sizeof(PKT), 0);
	return r == sizeof(PKT);  // error handling is done at a higher level... no need to resend here on errors.
}

#endif //ENABLE_WATCHCLIENT
#endif // WIN32
