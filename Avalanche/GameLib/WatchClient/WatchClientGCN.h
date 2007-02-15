//===================================================================
//  File:  WatchClientGCN.h
//  Description:
//     This is the client side of the Watch tool.  This will allow
//     the developer to change and watch variables on a running
//     PSXII application.
//===================================================================

#ifndef __WATCHCLIENTGCN_H__
#define __WATCHCLIENTGCN_H__
#ifdef GCN

#include "WatchClient/WatchClient.h"
#ifdef ENABLE_WATCHCLIENT

struct OPT
{
	volatile int    s;			// socket descriptor
	volatile int    rbytes;
};

#define PROTO_SAMPLE	0xe001
#define NODE_EE		'E'
#define NODE_HOST	'H'
#ifdef CDROM
#define BUFSZ 1           // not supported in CDROM mode
#else
#define BUFSZ 0x10000           // must be multiple of 64 for cache
#endif

class WatchClientGCN : public WatchClient
{
// Setup
protected:
	WatchClientGCN();
	~WatchClientGCN();

// Member functions
public:
	bool Init();
	void EventHandlerGCN(int event, int param, OPT* o);
	bool SendPacket(int type, int length, char* buf);
	void SendPacketWait(int type, int length, char* buf) {};

// Attributes
protected:
	OPT opt;

	friend class WatchClient;
};

#endif //ENABLE_WATCHCLIENT
#endif // GCN
#endif // __WATCHCLIENTGCN_H__
