#include "StdAfx.h"
#include ".\thread.h"

DWORD IMM::Thread::ThreadProc( LPVOID lpParameter )
{
	IMM::Thread* thread = (IMM::Thread*)lpParameter;

	int retval = thread->Run();

	return retval;
}


IMM::Thread::Thread(void) :
	mHandle( INVALID_HANDLE_VALUE ),
	mStopThreadRequest( INVALID_HANDLE_VALUE )
{
}

IMM::Thread::~Thread(void)
{
	Stop( );
}

void IMM::Thread::Start( )
{
	if ( mHandle != INVALID_HANDLE_VALUE )
		Stop( );

	mStopThreadRequest = CreateEvent( NULL, TRUE, FALSE, NULL );
	ASSERT( mStopThreadRequest != NULL );
	mHandle = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)&ThreadProc, this, 0, NULL );
}

int IMM::Thread::Stop( )
{
	DWORD retval = -1;
	if ( mHandle == INVALID_HANDLE_VALUE )
		return retval;

	SetEvent( mStopThreadRequest ); 
	DWORD result = WaitForSingleObject( mHandle, 1000 );
	if ( result == WAIT_TIMEOUT )
	{
		TerminateThread( mHandle, retval );
	}

	retval = GetExitCodeThread( mHandle, &retval );

	mHandle = INVALID_HANDLE_VALUE;

	return retval;
}
