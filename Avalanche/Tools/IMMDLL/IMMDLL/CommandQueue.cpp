#include "StdAfx.h"
#include "IMMDLLInternal.h"
#include "CommandQueue.h"

IMM::CommandQueue::CommandQueue( ) :
	mHead( NULL )
{
	mMutex = CreateMutex( NULL, FALSE, NULL );
	if ( mMutex == NULL )
		DebugOut( "Mutex creation failed" );
	mCommandCountSemaphore = CreateSemaphore( NULL, 0, 10000, NULL );
	if ( mCommandCountSemaphore == NULL )
		DebugOut( "Semaphore creation failed" );
}

IMM::CommandQueue::~CommandQueue( )
{
	CloseHandle( mCommandCountSemaphore );
	CloseHandle( mMutex );
}

IMM::CommandQueue::CommandQueueEntry* IMM::CommandQueue::CreateEntry( Command* cmd )
{
	CommandQueueEntry* newEntry = new CommandQueueEntry();
	newEntry->mNext = NULL;
	newEntry->mCommand = cmd;
	return newEntry;
}

bool IMM::CommandQueue::ContainsSimilar( Command* cmd )
{
	DWORD result = WaitForSingleObject( mMutex, MUTEX_TIMEOUT );
	ASSERT( result != WAIT_TIMEOUT );
	if ( result == WAIT_TIMEOUT )
	{
		// is it reasonable to return true to try to keep more commands from
		// building up while we wait?
		return true;
	}

	if ( mHead == NULL )
	{
		ReleaseMutex( mMutex );
		return false;
	}

	CommandQueueEntry* curr = mHead;
	unsigned long signature = cmd->Signature( );

	while ( curr )
	{
		if ( curr->mCommand->Signature( ) == signature )
		{
			ReleaseMutex( mMutex );
			return true;
		}

		curr = curr->mNext;
	}

	ReleaseMutex( mMutex );

	return false;
}

void IMM::CommandQueue::Enqueue( Command* cmd )
{
	DWORD result = WaitForSingleObject( mMutex, MUTEX_TIMEOUT );
	ASSERT( result != WAIT_TIMEOUT );
	if ( result == WAIT_TIMEOUT )
	{
		delete cmd;
		return;
	}

	bool retval = false;

	if ( mHead == NULL )
	{
		mHead = CreateEntry( cmd );
	}
	else
	{
		CommandQueueEntry* prev = mHead;
		CommandQueueEntry* curr = mHead;
		unsigned long signature = cmd->Signature( );

		while ( curr )
		{
			if ( curr->mCommand->Signature( ) == signature )
				break;

			prev = curr;
			curr = prev->mNext;
		}

		if ( curr )
		{
			delete curr->mCommand;
			curr->mCommand = cmd;
		}
		else
			prev->mNext = CreateEntry( cmd );
	}

	ReleaseMutex( mMutex );
	ReleaseSemaphore( mCommandCountSemaphore, 1, NULL );

	return;
}

IMM::Command* IMM::CommandQueue::Dequeue( )
{
	DWORD result = WaitForSingleObject( mMutex, MUTEX_TIMEOUT );
	ASSERT( result != WAIT_TIMEOUT );
	if ( result == WAIT_TIMEOUT )
		return NULL;

	CommandQueueEntry* oldHead = mHead;
	Command* retval = NULL;
	if ( oldHead )
	{
		retval = oldHead->mCommand;
		mHead = oldHead->mNext;
		delete oldHead;
	}

	ReleaseMutex( mMutex );

	return retval;
}

IMM::Command* IMM::CommandQueue::Dequeue( unsigned long signature )
{
	DWORD result = WaitForSingleObject( mMutex, MUTEX_TIMEOUT );
	ASSERT( result != WAIT_TIMEOUT );
	if ( result == WAIT_TIMEOUT )
		return NULL;

	if ( mHead == NULL )
	{
		ReleaseMutex( mMutex );
		return NULL;
	}

	CommandQueueEntry* retentry = NULL;
	if ( mHead->mCommand->Signature( ) == signature )
	{
		retentry = mHead;
		mHead = mHead->mNext;
	}
	else
	{
		CommandQueueEntry* curr = mHead->mNext;
		CommandQueueEntry* prev = mHead;

		while ( curr )
		{
			if ( curr->mCommand->Signature( ) == signature )
			{
				retentry = curr;
				prev->mNext = curr->mNext;
				break;
			}

			prev = curr;
			curr = curr->mNext;
		}
	}

   ReleaseMutex( mMutex );

	if ( retentry == NULL )
		return NULL;

	Command* retval = retentry->mCommand;
	delete retentry;
	return retval;
}


