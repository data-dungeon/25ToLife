//===================================================================
//  File:  WatchClientXBOX.cpp
//  Description:
//     Implementation of the XBOX specific WatchClient stuff.
//===================================================================

#if defined (_XBOX)
#include <string.h>
#include <stdio.h>
#include "Layers/Debug.h"
#include "WatchClient/watchclientxbox.h"

#ifdef ENABLE_WATCHCLIENT
#include "WatchInfo.h"

WatchClientXbox::WatchClientXbox()
{
	s2 = INVALID_SOCKET;
	server = new sockaddr_in;
	Init();
}

WatchClientXbox::~WatchClientXbox()
{
	delete server;
	closesocket(s2);
}

//----------------------------------------------------------------------
// WatchClient::InitWIN32
//    Initialization for Win 32 client.
//----------------------------------------------------------------------
bool WatchClientXbox::Init()
{
	// Default Xbox SNL initialization
	XNetStartupParams xnsp;
	memset(&xnsp, 0, sizeof(xnsp));
	xnsp.cfgSizeOfStruct = sizeof(XNetStartupParams);
	xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;
	INT err = XNetStartup(&xnsp);
	if (err != 0)
	{	
		conPuts("Cannot startup XNet\n");
		return(false);
	}

	//---------------------------------------------
	//  Startup the network.
	//---------------------------------------------
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{
		conPuts("Cannot startup WSA\n");
		return(false);
	}

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

void WatchClientXbox::CheckConnection()
{
	static int bitchHo = -1;

	if (INVALID_SOCKET == s2)
	{
		int addrlen = 16;

		s2 = accept(s, (struct sockaddr *)server, &addrlen);
		if (s2 != INVALID_SOCKET)
			conPuts("Connection accepted!\n");
		int error = WSAGetLastError();
		if (error != bitchHo)
		{
			bitchHo = error;
			TRACE("Accept status %d\n", error);
		}
	}
}

//----------------------------------------------------------------------
// WatchClient32::Run
//    This is called in each run loop of the application.
//----------------------------------------------------------------------
void WatchClientXbox::Run()
{
	WatchClient::Run();

	CheckConnection();

	if (INVALID_SOCKET == s2)
		return;

	char recvbuf[256];
	unsigned long nbytes;
	ioctlsocket(s2, FIONREAD, &nbytes);
	if (nbytes > 0)
	{
		do
		{
			recv(s2, recvbuf, sizeof(PKT), 0);
			Packet((PKT*)recvbuf);
			ioctlsocket(s2, FIONREAD, &nbytes);
		} while (nbytes > 0);
	}
}

bool WatchClientXbox::SendPacket(int type, int length, char* buf)
{
	PKT pkt;
	pkt.deci2.length = sizeof(PKT);
	pkt.pktType = type;
	memcpy(pkt.text, buf, length);
	int r = send(s2, (const char*)&pkt, sizeof(PKT), 0);
	return r == sizeof(PKT);
}

#endif
#endif // XBOX
