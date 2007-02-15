//===================================================================
//  File:  WatchClientGCN.cpp
//  Description:
//     Implementation of the GCN specific WatchClient stuff.
//===================================================================

#ifdef GCN
#include "WatchClient/WatchClientGCN.h"
#ifdef ENABLE_WATCHCLIENT
#include "WatchInfo.h"

static bool bWriteDone = true;

static void EventHandlerGCN(int e, int p, void *o)
{
	((WatchClientGCN*)WatchClient::Instance())->EventHandlerGCN(e, p, (OPT*)o);
}

WatchClientGCN::WatchClientGCN()
{
	Init();
}

WatchClientGCN::~WatchClientGCN()
{
}

bool WatchClientGCN::Init()
{
	return 0;
}

void WatchClientGCN::EventHandlerGCN(int event, int param, OPT* opt)
{
}

bool WatchClientGCN::SendPacket(int type, int length, char* buf)
{
	return false;
}

#endif //ENABLE_WATCHCLIENT
#endif // GCN
