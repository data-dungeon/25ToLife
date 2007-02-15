// IMMDLL.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "IMMDLLInternal.h"

namespace IMM
{
	IMMDLL_API bool AddNewConnectionCallback( NewConnectionCallback* callback )
	{
		DWORD result = WaitForSingleObject( gCallbackListMutex, CALLBACK_MUTEX_TIMEOUT );
		if ( result != WAIT_OBJECT_0 )
		{
			return false;
		}

		CallbackContainer* it = new CallbackContainer;
		it->nextCallback = gCallbacks;
		it->callback = callback;
		gCallbacks = it;

		gServer.StartListening();

		ReleaseMutex( gCallbackListMutex );
		return true;
	}

	IMMDLL_API bool RemoveNewConnectionCallback( NewConnectionCallback* callback )
	{
		DWORD result = WaitForSingleObject( gCallbackListMutex, CALLBACK_MUTEX_TIMEOUT );
		if ( result != WAIT_OBJECT_0 )
		{
			return false;
		}

		gServer.StopListening();

		CallbackContainer* prev = gCallbacks;
		ASSERT( prev != NULL );
		if ( prev == NULL )
		{
			ReleaseMutex( gCallbackListMutex );
			return false;
		}

		CallbackContainer* curr = prev->nextCallback;

		if ( prev->callback == callback )
		{
			gCallbacks = curr;
			delete prev;
			ReleaseMutex( gCallbackListMutex );
			return true;
		}

		while ( curr->callback != callback )
		{
			prev = curr;
			curr = prev->nextCallback;
		}

		ASSERT( curr != NULL );
		if ( curr == NULL )
		{
			ReleaseMutex( gCallbackListMutex );
			return false;
		}

		prev->nextCallback = curr->nextCallback;
		delete curr;

		ReleaseMutex( gCallbackListMutex );
		return true;
	}
}
