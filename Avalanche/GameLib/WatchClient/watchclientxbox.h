//===================================================================
//  File:  WatchClientXBOX.h
//  Description:
//     This is the client side of the Watch tool.  This will allow
//     the developer to change and watch variables on a running
//     PSXII application.
//===================================================================

#ifndef __WATCHCLIENTXBOX_H__
#define __WATCHCLIENTXBOX_H__
#ifdef _XBOX

#include "WatchClient/WatchClient.h"
#ifdef ENABLE_WATCHCLIENT

struct sockaddr_in;

class WatchClientXbox : public WatchClient
{
// Setup
protected:
	WatchClientXbox();
	~WatchClientXbox();

// Member functions
public:
	bool Init();
	void CheckConnection();

	void Run();
	bool SendPacket(int type, int length, char* buf);
	void SendPacketWait(int type, int length, char* buf) {}

// Attributes
protected:
	long s;
	long s2;
	sockaddr_in *server;

	friend class WatchClient;
};
#endif // ENABLE_WATCHCLIENT

#endif // XBOX
#endif // __WATCHCLIENTXBOX_H__
