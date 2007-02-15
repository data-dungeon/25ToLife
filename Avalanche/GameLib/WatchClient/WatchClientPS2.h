//===================================================================
//  File:  WatchClientPS2.h
//  Description:
//     This is the client side of the Watch tool.  This will allow
//     the developer to change and watch variables on a running
//     PSXII application.
//===================================================================

#ifndef __WATCHCLIENTPS2_H__
#define __WATCHCLIENTPS2_H__
#ifdef PS2

#include <eekernel.h>
#include <stdio.h>
#include <deci2.h>
#include "WatchClient/WatchClient.h"
#ifdef ENABLE_WATCHCLIENT

struct OPT
{
	volatile int    s;			// socket descriptor
	volatile u_int  wlen;		// write len
	volatile u_int  rlen;
	u_char         *wptr;		// write buffer pointer
	u_char         *rptr;		// read  buffer pointer
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

class WatchClientPS2 : public WatchClient
{
// Setup
protected:
	WatchClientPS2();
	~WatchClientPS2();

// Member functions
public:
	bool Init();
	void EventHandlerPS2(int event, int param, OPT* o);
	bool SendPacket(int type, int length, char* buf);
	void SendPacketWait(int type, int length, char* buf) {};

// Attributes
protected:
	OPT opt;

	friend class WatchClient;
};

#endif //ENABLE_WATCHCLIENT
#endif // PS2
#endif // __WATCHCLIENTPS2_H__
