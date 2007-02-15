#pragma once

#include "IMMDLL.h"
#include "Server.h"
#include "Connection.h"

namespace IMM
{

	struct CallbackContainer
	{
		CallbackContainer* nextCallback;
		NewConnectionCallback* callback;
	};
	
	#define CALLBACK_MUTEX_TIMEOUT 10000
	extern HANDLE gCallbackListMutex;
	extern CallbackContainer *gCallbacks;
	extern Server gServer;

	extern void NewConnection( Connection* connection );
	extern void DebugOut(const char* msg);
	extern void DebugOut(const char* msg, int value);
	extern void DebugOut(const char* msg, const void* data, int datalen);
}
