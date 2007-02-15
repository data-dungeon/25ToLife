#include "StdAfx.h"
#include "Server.h"
#include "IMMDLLInternal.h"
#include "Connection.h"

IMM::Server::Server( ) :
	mListeningCount( 0 )
{
}

IMM::Server::~Server( )
{
	ASSERT( mListeningCount == 0 );

	Connection::CloseAll();
}

void IMM::Server::StartListening( )
{
	// I know this looks weird, but it assures that every thread
	// has WSA setup and cleanedup correctly.

	WSADATA wsaData;
	WSAStartup( MAKEWORD( 2, 0 ), &wsaData );

	if ( mListeningCount++ > 0 )
		return;

	WSAStartup( MAKEWORD( 2, 0 ), &wsaData );

	Start( );
}

void IMM::Server::StopListening( )
{
	// I know this looks weird, but it assures that every thread
	// has WSA setup and cleanedup correctly.

	WSACleanup( );

	if ( --mListeningCount > 0 )
		return;

	Stop( );

	WSACleanup( );
}

int IMM::Server::Run( )
{
	// create the socket
	SOCKET s = socket( AF_INET, SOCK_STREAM, 0 );
	if ( s == INVALID_SOCKET )
	{
		DebugOut("IMM::Server Can create socket");
		return -2;
	}

	// bind the socket
	sockaddr_in addr;
	memset( &addr, 0, sizeof( addr ) );
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl( INADDR_ANY );
	addr.sin_port = htons(8953);
	int result = bind( s, (sockaddr*) &addr, sizeof( addr ) );
	if ( result != 0 )
	{
		DebugOut("IMM::Server Can bind socket");
		closesocket( s );
		return -3;
	}

	// set socket to non-blocking mode
	unsigned long nonblocking = 1;
	result = ioctlsocket( s, FIONBIO, &nonblocking );
	if ( result != 0 )
	{
		DebugOut("IMM::Server Can set socket to non-blocking");
		closesocket( s );
		return -4;
	}

	// start listening
	result = listen( s, 5 );
	if ( result != 0 )
	{
		DebugOut("IMM::Server Can listen on socket");
		closesocket( s );
		return -5;
	}

	DebugOut("IMM::Server Awaiting connection");
	while ( WaitForSingleObject( mStopThreadRequest, 0 ) == WAIT_TIMEOUT )
	{
		SOCKET newsocket = accept( s, NULL, NULL );
		if ( newsocket == INVALID_SOCKET )
		{
			if ( WSAGetLastError( ) != WSAEWOULDBLOCK )
				break;

			Sleep( 100 );
			continue;
		}

		DebugOut("IMM::Server got connection");
		CreateConnection( newsocket );
	}

	DebugOut("IMM::Server Shutting down");
	closesocket( s );

	return 0;
}

void IMM::Server::CreateConnection( SOCKET s )
{
	IMM::NewConnection( new Connection( s ) );
}


