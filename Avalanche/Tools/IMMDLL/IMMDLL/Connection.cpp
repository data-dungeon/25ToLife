#include "StdAfx.h"
#include "IMMDLLInternal.h"
#include "Connection.h"

//////////////////////////////////////////////////////////////////////////////

HANDLE IMM::Connection::sConnectionMutex = INVALID_HANDLE_VALUE;
IMM::Connection* IMM::Connection::sConnectionList = NULL;

//////////////////////////////////////////////////////////////////////////////

IMM::Connection::Connection( SOCKET s ) : 
	mSocket( s ),
	mSink( NULL )
{
	DebugOut("IMM::Connection created");

	AddToConnectionList();

	// associate an event object with the socket.
	mSocketEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	if ( mSocketEvent == NULL )
	{
		Close( );
		return;
	}
	int retval = WSAEventSelect( mSocket, mSocketEvent, FD_READ|FD_CLOSE );
	if( retval != 0 )
		Close( );
}

//////////////////////////////////////////////////////////////////////////////

IMM::Connection::~Connection(void)
{
	Stop( );
	Close( );
	CloseHandle( mSocketEvent );
	DebugOut("IMM::Connection destroyed");

	// remove us from the list...
	RemoveFromConnectionList();
}

//////////////////////////////////////////////////////////////////////////////

void IMM::Connection::AddToConnectionList()
{
	if ( sConnectionMutex == INVALID_HANDLE_VALUE )
		sConnectionMutex = CreateMutex( NULL, TRUE, NULL );
	else
		WaitForSingleObject( sConnectionMutex, INFINITE );

	mNextConnection = sConnectionList;
	sConnectionList = this;

	ReleaseMutex( sConnectionMutex );
}

//////////////////////////////////////////////////////////////////////////////

void IMM::Connection::RemoveFromConnectionList()
{
	WaitForSingleObject( sConnectionMutex, INFINITE );

	if ( sConnectionList == this )
		sConnectionList = mNextConnection;
	else
	{
		Connection* prev = sConnectionList;

		while ( prev->mNextConnection )
		{
			if ( prev->mNextConnection == this )
			{
				prev->mNextConnection = mNextConnection;
				break;
			}
		}
	}

	ReleaseMutex( sConnectionMutex );

	mNextConnection = NULL;
}

//////////////////////////////////////////////////////////////////////////////

void IMM::Connection::CloseAll( )
{
	if ( sConnectionMutex == INVALID_HANDLE_VALUE )
		return;

	Connection* curr;

	do
	{
		WaitForSingleObject( sConnectionMutex, INFINITE );
		curr = sConnectionList;
		ReleaseMutex( sConnectionMutex );
		if ( curr )
			delete curr;
	} while ( curr );
}

//////////////////////////////////////////////////////////////////////////////

void IMM::Connection::Close( )
{
	if( mSocket == INVALID_SOCKET )
		return;

	shutdown( mSocket, SD_SEND );

	char buff;
	// TJC - this really should be a time based thing, not a loop counter.
	int loopcount = 100000;
	while ( loopcount-- && ( recv( mSocket, &buff, 1, 0 ) > 0 || WSAGetLastError() == WSAEWOULDBLOCK ) );
	if ( loopcount <= 0 )
		DebugOut( "IMM::Connection giving up on a graceful close of the game conneciton." );

	shutdown( mSocket, SD_RECEIVE );
	closesocket( mSocket );
	mSocket = INVALID_SOCKET;

	if ( mSink )
		mSink->OnCloseConnection( );
}

//////////////////////////////////////////////////////////////////////////////

int IMM::Connection::Run( )
{
	HANDLE handles[3];
	handles[0] = mQueue.mCommandCountSemaphore;
	handles[1] = mSocketEvent;
	handles[2] = mStopThreadRequest;

	while ( mSocket != INVALID_SOCKET )
	{
		WSANETWORKEVENTS events;

		// wait until there is more to process
		DWORD result = WaitForMultipleObjects( 3, &handles[0], FALSE, INFINITE );
		switch ( result )
		{
		case WAIT_OBJECT_0+0:
			ProcessOutgoing( );
			break;
		case WAIT_OBJECT_0+1:
			if ( WSAEnumNetworkEvents( mSocket, mSocketEvent, &events) != 0 )
				break;

			if ( events.lNetworkEvents & FD_READ )
				ProcessIncomming();

			if ( events.lNetworkEvents & FD_CLOSE )
				Close( );

			break;
		case WAIT_FAILED:
		case WAIT_OBJECT_0+2:
			Close( );
			break;
		case WAIT_TIMEOUT:
			// ping the sucker...
			Ping();
			break;
		}
	}
	DebugOut("IMM::Connection shutting down");
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

void IMM::Connection::ProcessOutgoing( )
{
	Command* cmd = mQueue.Dequeue( );
	if ( !cmd )
		return;

	DebugOut("IMM::Connection sending data packet");
	cmd->Send( mSocket );

	mAckQueue.Enqueue(cmd);
}

//////////////////////////////////////////////////////////////////////////////

void IMM::Connection::ProcessIncomming( )
{
	// see if its an ACK...
	char byte;
	if ( Receive( byte, false ) )
	{
		DebugOut( "IMM:Connection got packet type:", byte );
		if ( byte == 0x00 )
		{
			unsigned long signature;
			if ( Receive( signature ) )
			{
				Command* cmd = mAckQueue.Dequeue( signature );
				if ( cmd )
					delete cmd;

				Command* nextCmd = mStallQueue.Dequeue( signature );
				if ( nextCmd )
					mQueue.Enqueue( nextCmd );

				return;
			}
			else
			{
				DebugOut("IMM::Connection failed receiving signature to ACK");
			}
		}
		else if ( byte == 0x01 )
		{
			// NAK!
			unsigned long signature;
			if ( Receive( signature ) )
			{
				Command* cmd = mAckQueue.Dequeue( signature );
				if ( cmd )
				{
					DebugOut("IMM::Connection received NAK for packet sig", signature);
					Command* nextCmd = mStallQueue.Dequeue( signature );
					if ( nextCmd == NULL )
						nextCmd = cmd;
					else
						delete cmd;

					mQueue.Enqueue( nextCmd );
					return;
				}
				else
				{
					DebugOut("IMM::Connection received NAK for unknown packet", signature);
				}
			}
			else
			{
				DebugOut("IMM::Connection failed receiving signature to NAK");
			}
		}
		else
		{
			//What?
			DebugOut("IMM::Connection recieved unrecognized data");
			Close( );
			return;
		}
	}
}


//////////////////////////////////////////////////////////////////////////////

void IMM::Connection::Ping( )
{
	unsigned long pingpacket = 0;
	DebugOut("ping");
	Send(pingpacket);
}

//////////////////////////////////////////////////////////////////////////////

void IMM::Connection::Queue( Command* cmd )
{
	if ( mAckQueue.ContainsSimilar( cmd ) )
		mStallQueue.Enqueue( cmd );
	else
		mQueue.Enqueue( cmd );
}

//////////////////////////////////////////////////////////////////////////////

template <class T>
bool IMM::Connection::Receive( T& data, bool waitforit )
{
	return Receive( &data, sizeof( data ), waitforit );
}

//////////////////////////////////////////////////////////////////////////////

bool IMM::Connection::Receive( void* buffer, int length, bool waitforit )
{
	char* buff = (char*)buffer;
	int retryCount = 100;

	while ( length && retryCount )
	{
		int bytes = recv( mSocket, buff, length, 0 );

		if ( bytes > 0 )
		{
			length -= bytes;
			buff += bytes;
		}
		else
		{
			int err = WSAGetLastError( );

			if ( err == WSAEWOULDBLOCK && waitforit == false )
			{
				return false;
			}
			else if ( err == WSAEWOULDBLOCK )
			{
				Sleep(10);
				retryCount--;
			}
			else
			{
				Close( );
				return false;
			}
		}
	}

	DebugOut("IMM::Connection::Receive", buffer, length);

	return true;
}

//////////////////////////////////////////////////////////////////////////////

template <class T> void IMM::Connection::Send( const T& data )
{
	Send( &data, sizeof( data ) );
}

//////////////////////////////////////////////////////////////////////////////

void IMM::Connection::Send( const void* buffer, int length )
{
	//DebugOut("IMM::Connection::Send", buffer, length);

	int bytes = send( mSocket, (char*)buffer, length, 0 );
	if ( bytes < 0 )
	{
		DebugOut("IMM::Connection::Send failed");
		Close( );
	}
	if ( bytes != length )
	{
		DebugOut("IMM::Connection::Send failed");
		Close( );
	}
}

//////////////////////////////////////////////////////////////////////////////

void IMM::Connection::UpdateCamera( const Vector3& eye, const Vector3& focus, const Vector3& up )
{
	class XCommand : public Command
	{
		Connection* mConnection;
		struct
		{
			Vector3 mEye, mFocus, mUp;
		} data;
	public:
		XCommand(Connection* connection, const Vector3& eye, const Vector3& focus, const Vector3& up ) : 
			mConnection( connection )
		{
			data.mEye = eye;
			data.mFocus = focus;
			data.mUp = up;
		}
		virtual unsigned long Signature( ) { return 0x00000001; }
		virtual void Send( SOCKET socket )
		{
			mConnection->Send( Signature( ) );
			mConnection->Send( data );
		}
	};
	XCommand* cmd = new XCommand( this, eye, focus, up );
	Queue( cmd );
}

//////////////////////////////////////////////////////////////////////////////

void IMM::Connection::SetEventSink( IEventSink* sink )
{
	mSink = sink;
}

//////////////////////////////////////////////////////////////////////////////

void IMM::Connection::MoveInstanceByName( const char* instanceName, const Matrix& localtoworld )
{
	class XCommand : public Command
	{
		Connection* mConnection;
		struct blah
		{
			char mName[64];
			Matrix mTransform;
		} data;
	public:
		XCommand(Connection* connection, const char* name, const Matrix& transform ) : 
			mConnection( connection )
		{
			memset( &data.mName[0], 0, sizeof( data.mName ) );
			strncpy( data.mName, name, 63 );
			data.mTransform = transform;
		}
		virtual unsigned long Signature( ) { return Hash24(data.mName) | 0x02; }
		virtual void Send( SOCKET socket )
		{
			mConnection->Send( Signature( ) );
			mConnection->Send( data );
		}
	};
	XCommand* cmd = new XCommand( this, instanceName, localtoworld );
	Queue( cmd );
}


//////////////////////////////////////////////////////////////////////////////

unsigned int IMM::Connection::Hash24(const char* string)
{
	return 0;
}
