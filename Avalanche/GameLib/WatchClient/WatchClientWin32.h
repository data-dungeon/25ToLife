//===================================================================
//  File:  WatchClientWin32.h
//  Description:
//     This is the client side of the Watch tool.  This will allow
//     the developer to change and watch variables on a running
//     WIN32 application.
//===================================================================

#ifndef __WATCHCLIENTWIN32_H__
#define __WATCHCLIENTWIN32_H__
#ifdef WIN32
#ifndef _XBOX

#include "platform/BaseType.h"
#include <windows.h>
#include "WatchClient/WatchClient.h"

#ifdef ENABLE_WATCHCLIENT

// forwards
struct sockaddr_in;

class WatchClientWin32 : public WatchClient
{
// Setup
protected:
	WatchClientWin32();
	~WatchClientWin32();

// Member functions
public:

	bool Init();
	void CheckConnection();

	void Start();
	void End();
	void Run();
	bool SendPacket(int, int, char*);
	void SendPacketWait(int a, int b, char* c) {}

// Attributes
protected:
	long s;
	long s2;
	sockaddr_in *server;

	PROCESS_INFORMATION pinfo;

	friend class WatchClient;
};

#endif //ENABLE_WATCHCLIENT
#endif // _XBOX
#endif // WIN32
#endif // __WATCHCLIENTWIN32_H__
