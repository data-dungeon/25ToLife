
#include "stdafx.h"
#include "IMMDLLInternal.h"
#include <stdio.h>

HANDLE IMM::gCallbackListMutex = INVALID_HANDLE_VALUE;
IMM::CallbackContainer *IMM::gCallbacks;

static int sListeningCount = 0;
IMM::Server IMM::gServer;

void IMM::NewConnection( Connection* connection )
{
	DWORD result = WaitForSingleObject( gCallbackListMutex, CALLBACK_MUTEX_TIMEOUT );
	ASSERT( result == WAIT_OBJECT_0 );
	if ( result != WAIT_OBJECT_0 )
	{
		delete connection;
		return;
	}

	CallbackContainer* curr = gCallbacks;

	while ( curr )
	{
		NewConnectionAction action;

		action = (*curr->callback) ( connection );

		if ( action == ACCEPT_CONNECTION )
			break;

		curr = curr->nextCallback;
	}

	// no one accepted it, so delete it.
	if ( curr == NULL )
	{
		DebugOut("No one accepted new connection");
		delete connection;
	}
	else
	{
		// go ahead and start it up then...
		connection->Start();
	}

	ReleaseMutex( gCallbackListMutex );
}

void IMM::DebugOut(const char* msg)
{
	char buffer[256];
	sprintf(buffer, "%s (%d : %d)\n", msg, GetLastError(), WSAGetLastError());
	OutputDebugString(buffer);
}

void IMM::DebugOut(const char* msg, int value)
{
	char buffer[256];
	sprintf(buffer, "%s -- %d (%d : %d)\n", msg, value, GetLastError(), WSAGetLastError());
	OutputDebugString(buffer);
}

void IMM::DebugOut(const char* msg, const void* data, int datalen)
{
	char buffer[256];
	const char* dat = (const char*)data;

	sprintf(buffer, "%s -- %d (%d : %d)", msg, datalen, GetLastError(), WSAGetLastError());
	OutputDebugString(buffer);
	for ( int i = 0; i < datalen; i++ )
	{
		if ( ( i % 8 ) == 0 )
			OutputDebugString( "\n\t" );
		sprintf( buffer, "%02.2X ", *dat++ );
		OutputDebugString( buffer );
	}
	OutputDebugString( "\n" );
}


// cant call out to other dlls except Kernel32.dll
static void LocalInitialize( )
{
	IMM::gCallbackListMutex = CreateMutex( NULL, FALSE, NULL );
}

// cant call out to other dlls except Kernel32.dll
static void LocalDeinitialize( )
{
	CloseHandle( IMM::gCallbackListMutex );
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved )
{
	static int processCount = 0;

	switch ( ul_reason_for_call )
	{
	case DLL_PROCESS_ATTACH:
		if ( processCount++ == 0 )
			LocalInitialize( );
		break;
	case DLL_PROCESS_DETACH:
		if ( --processCount == 0 )
			LocalDeinitialize( );
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
    return TRUE;
}

