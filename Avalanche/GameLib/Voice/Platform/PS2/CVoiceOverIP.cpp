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
#include <eekernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <sifdev.h>
#include <libnet.h>
#include <libmrpc.h>
#include <libinsck.h>
#include <netz.h>
#include <qnet.h>
#include <errno.h>
#include <eeregs.h>
#include <eetypes.h>
#include <libgraph.h>
#include <libdev.h>
#include <eekernel.h>
#include <libpad.h>


//#define dbgPrint //
//#define printf //
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

static u32 ip = 0x0a000421;
// ---------------------------------------------------------------------------
u32 CVoiceOverIP::sendPacket( u8 *pSrc, u32 size, u32 ip  )
{
	int result = 0;
#if 0
	sockaddr_in socket_out;
	memset( &socket_out, 0, sizeof( socket_out ) );
	socket_out.sin_family		= AF_INET;
	socket_out.sin_port			= htons( m_udpPortIn );
	socket_out.sin_len			= 4;
	socket_out.sin_addr.s_addr	= htonl( ip );

	while( ( result = sendto( m_udpSocket, pSrc, size, 0, ( struct sockaddr * )&socket_out, 16 ) ) <= 0 )
	{
		if( result < 0 )
		{
			//printf( "error sending data...->%d\n", sceInsockErrno );
			break;
		}
	}
#endif
	return( result );
}


// ---------------------------------------------------------------------------
u32 CVoiceOverIP::receivePacket( u8 *pDst, u32 ip )
{
	int size = 0;
#if 0
	sockaddr_in socketAddr;
	int len;
	
	memset( &socketAddr, 0, sizeof( socketAddr ) );
	socketAddr.sin_family		= AF_INET;
	socketAddr.sin_port			= htons( m_udpPortIn );
	socketAddr.sin_len			= 4;
	len								= sizeof( socketAddr );

	socketAddr.sin_addr.s_addr	= htonl( ip );
	size = recvfrom( m_udpSocketIn, pDst, 512, 0, ( sockaddr * )&socketAddr, ( sceInsockSocklen_t * )&len );
	if( size > 512 )
	{
		printf( "received bytes was larger than anticipated" );
		ASSERTS( false, "packet was larger than anticipated" );
	}
	if( size < 0 )
	{
		if( sceInsockErrno != ETIMEDOUT )
			printf( "receive error = %d\n", sceInsockErrno );
	}
#endif
	return( size >= 0 ? size : 0 );
}


#ifdef ENABLE_DEBUG_STATS
// ----------------------------------------------------------------------------
void CVoiceOverIP::dumpStats( )
{
#if 0
	if( m_debugOutputFrequency == 0 )
	{
		u32 iopMemory = sceSifQueryTotalFreeMemSize( );
		u32 iopBlock  = sceSifQueryMaxFreeMemSize( );
		dbgPrint( "-------------------------------------------------------\n" );
		dbgPrint( "largest packet sent      = %06d\n", m_largestPacketSent );
		dbgPrint( "largest packet received  = %06d\n", m_largestPacketReceived );
		dbgPrint( "playback bytes in buffer = %06d\n", m_pCVoiceDevice->getPlayBackBytes( ) );
		dbgPrint( "Total Free IOP Memory    = %07d\n", iopMemory );
		dbgPrint( "Largest available block  = %07d\n", iopBlock );
		dbgPrint( " ------------------------------------------------------\n\n" );
		m_debugOutputFrequency = DEBUG_OUTPUT_FREQUENCY;
	}
	m_debugOutputFrequency--;
#endif
}
#endif


// ---------------------------------------------------------------------------
bool CVoiceOverIP::initNetwork( )
{
	return false;

#if 0
	int ret;

	// --- make sure we aren't running on a atwinmon downloader, because
	// --- the network's clash
	extern const char* g_RemoteIPAddress;
	if ( g_RemoteIPAddress != NULL )
	{
		m_networkUp = false;
		return false;
	}

#ifdef CDROM
	//	if( !( m_networkUp = PS2Init( memcard, 0, 0 ) ) )
	if( !( m_networkUp = PS2Init( false, 0, 0xffffffff ) ) )
		return( false );
#else
//	if( iopMem == NULL )
//	{
		// --- we want the devkit to behave like a test deck
		//		iopMem = ( char * )sceSifAllocIopHeap( 6 * 1024 * 1024 );
		//		ASSERTS( iopMem, "could not allocate iop memory" );
//	}
	if( !( m_networkUp = PS2Init( false, 0, 0xffffffff ) ) )
		return( false );
#endif


	// --- lets create our sockets
	m_udpSocket = socket( AF_INET, SOCK_DGRAM, 0 );
	ASSERTS( m_udpSocket >= 0, "unable to create udp socket.." );

	sockaddr_in socket_in;
	memset( &socket_in, 0, sizeof( socket_in ) );
	socket_in.sin_family			= AF_INET;
	socket_in.sin_port			= htons( 8000 );
	socket_in.sin_addr.s_addr	= htonl(INADDR_NONE);
	socket_in.sin_len				= 4;

	ret = bind( m_udpSocket, ( struct sockaddr* )&socket_in, sizeof( socket_in ) );
	if( ret < 0 )
	{
		printf("bind socket error = %d...\n", sceInsockErrno );
		return( false );
	}

	// --- socket timeout settings
	sceInsockSetRecvTimeout( m_udpSocket, 0 );
	sceInsockSetSendTimeout( m_udpSocket, 0 );
	int iFastModeFlag = 2;
	//setsockopt(m_udpSocket,IPPROTO_IP,IP_MULTICAST_LOOP,(char*)&iFastModeFlag,sizeof(int));


	// --- create our socket
	m_udpPortIn = 9600;
	m_udpSocketIn = socket( AF_INET, SOCK_DGRAM, 0 );
	ASSERTS( m_udpSocketIn >= 0, "unable to create udp socket.." );

	memset( &socket_in, 0, sizeof( socket_in ) );
	socket_in.sin_family			= AF_INET;
	socket_in.sin_port			= htons( m_udpPortIn );
	socket_in.sin_addr.s_addr	= htonl(INADDR_ANY);
	socket_in.sin_len				= 4;

	ret = bind( m_udpSocketIn, ( struct sockaddr* )&socket_in, sizeof( socket_in ) );
	if( ret < 0 )
	{
		printf("bind socket error = %d...\n", sceInsockErrno );
		return( false );
	}

	// --- socket timeout settings
	sceInsockSetRecvTimeout( m_udpSocketIn, 0 );
	sceInsockSetSendTimeout( m_udpSocketIn, 0 );

	//setsockopt(m_udpSocketIn,IPPROTO_IP,IP_MULTICAST_LOOP,(char*)&iFastModeFlag,sizeof(int));
	return( true );
#endif
}


#if 0
void	LoadModule( const char *module, int args, const char *argp )
{
	while ( sceSifLoadModule( module, args, argp ) < 0 ) {
		printf( "%s cannot be loaded.\n", module );
	}
}



int CThread(void *func, char *stackp, int size, int prio, void *argp) 
{
	struct ThreadParam tp;
	int tid;

	tp.entry = (void (*)(void *))func;
	tp.stack = stackp;
	tp.stackSize = size;
	tp.initPriority = prio;
	tp.gpReg = &_gp;

	if ((tid = CreateThread(&tp)) >= 0)
		StartThread(tid, argp);
	else {
		printf("CThread(): CreateThread() failed.\n");
		return -1;
	}
	return 0;
}
#endif