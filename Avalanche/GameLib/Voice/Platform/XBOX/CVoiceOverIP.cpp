// ---------------------------------------------------------------------------
// File			-> CVoiceOverIP.cpp
// Author		-> Dwain Skinner
// Created		-> January 22, 2004
// Description	-> Platform dependent VoIP class
//
//  Copyright (c) 2004, Avalanche Software Inc.
//  All Rights Reserved.
// ---------------------------------------------------------------------------

// --- Precompiled header
#include "Voice/VoicePCH.h"

// --- local includes

// --- platform includes

// ----------------------------------------------------------------------------
CVoiceOverIP::CVoiceOverIP( ):
#ifdef ENABLE_DEBUG_STATS
m_largestPacketReceived( 0 ),
m_largestPacketSent( 0 ),
m_debugOutputFrequency( DEBUG_OUTPUT_FREQUENCY ),
#endif
m_networkUp( false )
{
}


// ----------------------------------------------------------------------------
CVoiceOverIP::~CVoiceOverIP( )
{
	destroy( );
}


// ----------------------------------------------------------------------------
bool CVoiceOverIP::create( void )
{
	return( true );
}

// ----------------------------------------------------------------------------
bool CVoiceOverIP::destroy( void )
{
	return( true );
}


// ----------------------------------------------------------------------------
bool CVoiceOverIP::update( void  )
{
	return( true );
}

static bool turnedOn = true;
// ---------------------------------------------------------------------------
u32 CVoiceOverIP::sendPacket( u8 *pSrc, u32 size, u32 ip  )
{
	int result = 0;
	sockaddr_in socket_out;

	memset( &socket_out, 0, sizeof( socket_out ) );
	socket_out.sin_family		= AF_INET;
	socket_out.sin_port			= htons( 9600 );
	socket_out.sin_addr.s_addr	= htonl( ip );

	while( ( result = sendto( m_udpSocket, ( char * )pSrc, size, 0, ( struct sockaddr * )&socket_out, sizeof( socket_out ) ) ) <= 0 )
	{
		if( result < 0 )
		{
			//printf( "error sending data...->%d\n", sceInsockErrno );
			break;
		}
	}
	return( result );
}


// ---------------------------------------------------------------------------
u32 CVoiceOverIP::receivePacket( u8 *pDst, u32 ip )
{
	int ret;
	int size = 0;
	FD_SET	set;
	timeval	waitTimeStr;
	sockaddr_in socketAddr;
	int len;

	FD_ZERO( &set );
	FD_SET( m_udpSocket, &set );
	waitTimeStr.tv_sec = 0;
	waitTimeStr.tv_usec= 0;
	ret = select( FD_SETSIZE, &set, NULL, NULL, &waitTimeStr );
	if( ret == 0 ) return( 0 );

	memset( &socketAddr, 0, sizeof( socketAddr ) );
	socketAddr.sin_family		= AF_INET;
	socketAddr.sin_port			= htons( 9600 );
	len								= sizeof( socketAddr );
	socketAddr.sin_addr.s_addr	= htonl( ip );


	size = recvfrom( m_udpSocket, ( char * )pDst, 512, 0, ( sockaddr * )&socketAddr, &len );
//	size = recv( m_udpSocket, ( char * )pDst, 512, 0 );
	if( size > 512 )
	{
		dbgPrint( "received bytes was larger than anticipated" );
		ASSERTS( false, "packet was larger than anticipated" );
	}
	if( size < 0 )
	{
		dbgPrint( "receive error = %d\n", WSAGetLastError( ) );
	}
	else
	{
		//dbgPrint( "received %d bytes...\n", size ) ;
	}
	return( size >= 0 ? size : 0 );
}


// --------------------------------------------------------------------------------------------------
bool CVoiceOverIP::initNetwork( )
{
	int	ret = false;
	int	timeout = 0;

	// --- create our socket
	if( ( m_udpSocket = socket( AF_INET, SOCK_DGRAM, 0 ) ) == SOCKET_ERROR )
	{
		ASSERTS( false, "unable to create udp socket.." );
		return( false );
	}

	sockaddr_in socket_in;
	memset( &socket_in, 0, sizeof( socket_in ) );
	socket_in.sin_family			= AF_INET;
	socket_in.sin_port			= htons( 9600 );
	socket_in.sin_addr.s_addr	= htonl(INADDR_ANY);

	if( ( ret = bind( m_udpSocket, ( struct sockaddr* )&socket_in, sizeof( socket_in ) ) ) == SOCKET_ERROR )
	{
		ASSERTS( ret != SOCKET_ERROR, "error binding socket" );
		return( false );
	}

	return( true );
}


