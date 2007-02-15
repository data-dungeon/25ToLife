/*
**
**  File:   NetZConnection.cpp
**  Date:   November 21, 2003
**  By:     Bryant Collard
**  Desc:   The NetZ network connection.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/NetZConnection.cpp $
**  $Revision: #56 $
**  $DateTime: 2005/08/23 20:51:24 $
**    $Author: Dwain_Skinner $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// --- quazal needs this defined now for memory tracking
#define LEAK_DETECTION_ENABLED

// My header
#include "Network/NetZ/NetZConnection.h"

// Engine includes
#include "Layers/Debug.h"
#include "EngineHelper/timer.h"
#include "GameHelper/ParamMgr.h"
#include "EngineHelper/Singleton.h"
#include "cmdcon/cmdcon.h"
#include "Layers/Semaphore/Semaphore.h"
#include "GameHelper/DebugOutput.h"

// NetZ includes
#include <netz.h>

// compression
#include "Network/NetZ/NetZCompress.h"

// --- temporary waiting for a fix from martin
#include "Network/Session.h"
#include "Network/NetZ/DOTalker.h"
#include "Voice/NetZ/CVoiceChat.h"

// encryption
#include "Network/NetZ/NetZEncrypt.h"

// XBox includes
#ifdef _XBOX
#include <platform/stack/core/xboxnetworking.h>
#endif

// PS2 includes
#ifdef PS2
#include <qnet.h>
#include <libinsck.h>
#include "platform/PS2/CDllLoader.h"
#endif

// Memory management
void* OurMalloc(size_t stSize);
void OurFree(void *pBlock);
void AllocOurMem();
void FreeOurMem();

// Logging
#define DEBUG_LOG_OUTPUT      1
#define SCREEN_LOG_OUTPUT     2
#define FILE_LOG_OUTPUT       3
#ifndef CDROM
#define TRACE_LOG_OUTPUT      DEBUG_LOG_OUTPUT
#define EVENT_LOG_OUTPUT      FILE_LOG_OUTPUT
#else
#define TRACE_LOG_OUTPUT      SCREEN_LOG_OUTPUT
#define EVENT_LOG_OUTPUT      SCREEN_LOG_OUTPUT
#endif
#ifdef TRACE_LOG
LogDevice* CNetZConnection::s_traceLogDevice = NULL;
#endif
#ifdef EVENT_LOG
LogDevice* CNetZConnection::s_eventLogDevice = NULL;
#endif

#define TRACE_STAT_PERIOD_MS  1000
#define NOT_THREADED
#define USE_ENCRYPTION
#define USE_COMPRESSION

#if !defined(CDROM) && !defined(AVUSER_TJC)
//#define COLLECT_MEMORY_STATS
#define USE_LINERS
#ifdef USE_LINERS
//#define TRACK_ALLOCS
#endif
#endif

CSemaphore netzMemMutex(false);

// Define static members
CNetZConnection::EConfigurationStatus CNetZConnection::s_configured =
		CNetZConnection::NOT_CONFIGURED;

void *CNetZConnection::d_profileList = NULL;
int CNetZConnection::d_profileIndex = -1;
bool CNetZConnection::s_linkError = false;

CNetZConnection::SNetworkConfigSettings CNetZConnection::s_netSettings =
{
	true,         // bool d_bundleEnabled;
	true,			  // bool d_bundleMixed;
	4096,         // uint32 d_bundleMaxSize;
	83,           // int32 d_bundleMaxFlushDelay;
	true,         // bool d_streamBundleEnabled;
	0,            // int32 d_streamBundleMaxFlushDelay;
	1000,         // uint32 d_prudpKeepAliveTimeout;
	20,           // uint32 d_prudpMaxRetransmission;
	12000,        // uint32 d_prudpLongMaxSilenceTime;
	7000,         // uint32 d_prudpShortMaxSilenceTime;
	50,           // uint32 d_prudpPS2TimeSlice;
	10,           // uint32 d_prudpOtherTimeSlice;
	8,            // uint32 d_prudpWindowSize;
	1.25f,        // float d_prudpRetransmitTimeoutMultiplier;
	1.0f,         // float d_prudpExtraRetransmitTimeoutMultiplier;
	50,           // uint32 d_prudpExtraRetransmitTimeoutTrigger;
	30,           // uint32 d_prudpRttRetransmit;
	750,          // uint32 d_prudpInitialRtt;
	false,		  // bool d_connectionStabilityEnabled
	2.0f,			  // float d_connectionStabilityMinThreshold
	5.0f,			  // float d_connectionStabilityMaxThreshold
	0.15f,		  // float d_connectionStabilityResentThreshold
	0.15f,		  // float d_connectionStabilityLostThreshold
	2,				  // uint32 d_connectionStabilityMinPlayers
	TRACE_ALWAYS  // uint32 d_traceFlags;
};

/******************************************************************************/
/******************************************************************************/
CNetZConnection::CNetZConnection()
{
	d_net = NULL;
	d_servicing = false;
	d_compressionAlgorithm = NULL;
	d_encryptionAlgorithm = NULL;

#ifdef TRACE_STATS
	d_nextStatTime = 0;
#endif

#ifdef SIMULATE_CONDITIONS
	d_simParam = NULL;
	d_messageOwner.SetOwnerName("NetZConnection")
#endif
}

/******************************************************************************/
/******************************************************************************/
CNetZConnection::~CNetZConnection()
{
	Disable();
}

/******************************************************************************/
/******************************************************************************/
void CNetZConnection::SetNetworkConfiguration( void *list, int index )
{
	d_profileList = list;
	d_profileIndex = index;
}

/******************************************************************************/
/******************************************************************************/
void CNetZConnection::GetNetworkConfiguration( void **list, int *index )
{
	*list = d_profileList;
	*index = d_profileIndex;
}

/******************************************************************************/
/******************************************************************************/
bool CNetZConnection::Configure(void)
{
	// --- we need to load any DLL's that are neccessary
//	loadNetworkDLLs( );

	if (s_configured == NOT_CONFIGURED)
	{
#ifdef _XBOX
		// ---Tell NetZ to use the VDP protocol on XBox for TCR compliance
#ifndef CDROM
		XBoxNetworking::UseVDP(false);
#else
		XBoxNetworking::UseVDP(true);
#endif
		// --- turn off secure networking until Martin finds out what is wrong
		XBoxNetworking::SetSecure(true);
#endif

#ifdef NOT_THREADED
		NetZ::UseThreads(false);
		NetZ::SelectThreadSafeExecution(false);
#endif

		// Register debug output categories.
		g_debugOutput.RegisterCategory("Network", "NET");
	}

	if (s_configured != CONFIGURATION_SUCCEEDED)
	{
//#if defined(PS2) && defined(SN)
		if( InitiateConnection( false ) )
		{
			s_configured = CONFIGURATION_SUCCEEDED;
		}
		else
			s_configured = CONFIGURATION_FAILED;
//#else
//		s_configured = CONFIGURATION_SUCCEEDED;
//#endif
	}

	return(s_configured == CONFIGURATION_SUCCEEDED);
}

/******************************************************************************/
/******************************************************************************/
void CNetZConnection::SetConnectionTimeout( bool i_useLongTimeout )
{
	if ((d_net != NULL) && d_net->IsValid())
	{
		// Timeouts
		Transport* pTransport =
			TransportProtocol::GetInstance()->FindTransport(PRUDP_TRANSPORT_TYPE);
		if( pTransport )
		{
			if( i_useLongTimeout )
				pTransport->SetOption(PRUDP_MAXSILENCETIME,
						UserContext(qUnsignedInt32(s_netSettings.d_prudpLongMaxSilenceTime)));
			else
				pTransport->SetOption(PRUDP_MAXSILENCETIME,
						UserContext(qUnsignedInt32(s_netSettings.d_prudpShortMaxSilenceTime)));
		}
	}
}

/******************************************************************************/
/******************************************************************************/
bool CNetZConnection::Enable(bool i_lanSession)
{
	// See if the connection is already enabled.
	if (d_net != NULL)
		return(true);

// Make sure the network is configured.
	if( s_configured != CONFIGURATION_SUCCEEDED )
	{
		if (!Configure())
			return(false);

		int wait = 30;
		bool result = false;
		while( wait && result == false )
		{
			result = CNetZConnection::UpdateConnectionStatus();
			Platform::Sleep( 100 );
		}

		if( !result )
		{
			s_configured = CONFIGURATION_FAILED;
			return( false );
		}
	}
	else
	{
		// --- we need to load any DLL's that are neccessary
//		loadNetworkDLLs( );

		// --- make sure the connection is still good
		if( !CNetZConnection::UpdateConnectionStatus() )
			return( false );
	}

	// Enable/disable LAN session discovery.
	if (i_lanSession)
		LANSessionDiscovery::Enable();
	else
		LANSessionDiscovery::Disable();

	// Create a NetZ object.
	d_net = new NetZ();
	if ((d_net != NULL) && d_net->IsValid())
	{
		// Timeouts
		Transport* pTransport =
				TransportProtocol::GetInstance()->FindTransport(PRUDP_TRANSPORT_TYPE);
		pTransport->SetOption(PRUDP_KEEPALIVETIMEOUT,
				UserContext(qUnsignedInt32(s_netSettings.d_prudpKeepAliveTimeout)));
		pTransport->SetOption(PRUDP_MAXSILENCETIME,
				UserContext(qUnsignedInt32(s_netSettings.d_prudpShortMaxSilenceTime)));
		pTransport->SetOption(PRUDP_MAXRETRANSMISSION,
				UserContext(qUnsignedInt32(s_netSettings.d_prudpMaxRetransmission)));
#ifdef PS2
		pTransport->SetOption(PRUDP_TIMESLICE,
				UserContext(qUnsignedInt32(s_netSettings.d_prudpPS2TimeSlice)));
#else
		pTransport->SetOption(PRUDP_TIMESLICE,
				UserContext(qUnsignedInt32(s_netSettings.d_prudpOtherTimeSlice)));
#endif
		pTransport->SetOption(PRUDP_WINDOW_SIZE,
				UserContext(qUnsignedInt32(s_netSettings.d_prudpWindowSize)));
#ifdef PS2
		pTransport->SetOption(PRUDP_RETRANSMIT_TIMEOUT_MULTIPLIER,
				UserContext(qReal(s_netSettings.d_prudpRetransmitTimeoutMultiplier)));
		pTransport->SetOption(PRUDP_EXTRA_RETRANSMIT_TIMEOUT_MULTIPLIER,
				UserContext(qReal(s_netSettings.d_prudpExtraRetransmitTimeoutMultiplier)));
		pTransport->SetOption(PRUDP_EXTRA_RETRANSMIT_TIMEOUT_TRIGGER,
				UserContext(qUnsignedInt32(s_netSettings.d_prudpExtraRetransmitTimeoutTrigger)));
		pTransport->SetOption(PRUDP_RTT_RETRANSMIT,
				UserContext(qUnsignedInt32(s_netSettings.d_prudpRttRetransmit)));
		pTransport->SetOption(PRUDP_INITIAL_RTT,
				UserContext(qUnsignedInt32(s_netSettings.d_prudpInitialRtt)));
#endif

		// -- Compression/Encryption
#ifdef USE_COMPRESSION
		d_compressionAlgorithm = new CNetZCompress;
		ASSERTS(d_compressionAlgorithm, "unable to create compression algorithm");
#endif

#ifdef USE_ENCRYPTION
		d_encryptionAlgorithm = new CNetZEncryption;
		ASSERTS(d_encryptionAlgorithm, "unable to create encryption algorithm");
#endif

		TransportProtocol::GetInstance()->SelectCompressionAlgorithm( d_compressionAlgorithm );
		TransportProtocol::GetInstance()->SelectEncryptionAlgorithm( d_encryptionAlgorithm );

		// --- enable bundling
		if ( s_netSettings.d_bundleEnabled )
			BundlingPolicy::GetInstance( )->Enable( );
		else
			BundlingPolicy::GetInstance( )->Disable( );
		BundlingPolicy::GetInstance( )->SetMaximumBundleSize( s_netSettings.d_bundleMaxSize );
		BundlingPolicy::GetInstance( )->SetMaximumFlushDelay( s_netSettings.d_bundleMaxFlushDelay );

#ifdef PS2
		// --- new bundling, mix reliable and unreliable in same bundles
		if ( s_netSettings.d_bundleMixed )
			BundlingPolicy::GetInstance( )->SetFlag(BundlingPolicy::MixReliability);

		// --- don't mix reliable and unreliable in same bundles
		else
			BundlingPolicy::GetInstance( )->ClearFlag(BundlingPolicy::MixReliability);
#endif

		if (s_netSettings.d_streamBundleEnabled)
			Stream::GetSettings( Stream::DO ).GetBundling().Enable( s_netSettings.d_streamBundleMaxFlushDelay );
		else
			Stream::GetSettings( Stream::DO ).GetBundling().Disable();

#ifdef PS2
		//QNet::GetInstance()->SetThreadsPrio(32); // For changing QNet Threads Prio (default is 32)
		//QNet::GetInstance()->SetInetThreadPrio(32); // For changing Inet Thread Prio (default is 32)
#endif

		// Install as the connection singleton.
		ASSERT(!ValidSingleton(CConnection));
		if (!ValidSingleton(CConnection))
			SetSingleton(CConnection, (CConnection*)this);
	}
	
	// Failed to build a valid NetZ object.
	else
	{
		char msg[256];
		QTRACE(TRACE_ALWAYS, ("Error creating NetZ object.\n"));
		g_console.Error("Error creating NetZ object.\n");
		SystemError::GetErrorString(SystemError::GetLast(), msg, 256);
		QTRACE(TRACE_ALWAYS, ("NetZError: %s\n", msg));
		g_console.Error("NetZError: %s\n", msg);
		delete d_net;
		d_net = NULL;
	}

	return(d_net != NULL);
}

/******************************************************************************/
/******************************************************************************/
void CNetZConnection::Disable(void)
{
	if (d_net != NULL)
	{
		delete d_net;
		d_net = NULL;

		delete d_compressionAlgorithm;
		d_compressionAlgorithm = NULL;

		delete d_encryptionAlgorithm;
		d_encryptionAlgorithm = NULL;

		// Uninstall singleton.
		if (IsSingleton(CConnection, this))
			ClearSingleton(CConnection);

		g_CVoiceChat.postNetZTearDown( );
	}
}

/******************************************************************************/
/******************************************************************************/
void CNetZConnection::Service(void)
{
	// Protect against reentrant calls.
	if (d_servicing)
		return;
	d_servicing = true;

	// --- check the cable link status
	s_linkError = !GetLinkStatus();

	static u32 timeLast = 0;

	if (d_net != NULL)
	{
		u32 timeNow = g_timer.GetOSTime();
		if (timeLast && (timeNow - timeLast) > 250)
			g_debugOutput.Print(g_debugOutput.Category("LAG"), "LongTime: %dms between service", timeNow - timeLast);
		timeLast = timeNow;

#ifdef NOT_THREADED
		Scheduler::GetInstance()->DispatchAll();
#else
		Platform::Sleep(0);
#endif
	}
	else
	{
		timeLast = 0;
	}

	d_servicing = false;
}

/******************************************************************************/
/******************************************************************************/
void CNetZConnection::InitializeTestSuite(void)
{
	ResetPerformanceCounters();
//	InitializeLog();
	InitializeNetworkSimulation();
}

/******************************************************************************/
/******************************************************************************/
void CNetZConnection::TerminateTestSuite(void)
{
	TerminateNetworkSimulation();
	DumpAverageStats();
//	TerminateLog();
}

/******************************************************************************/
/******************************************************************************/
u32 CNetZConnection::GetMemoryUsage(void)
{
	MemoryManager *QuazalMemoryManager = MemoryManager::GetDefaultMemoryManager();
	return(QuazalMemoryManager->GetMemoryUsage( ));
}

////////////////////////////// Memory Management ///////////////////////////////

#ifdef COLLECT_MEMORY_STATS

#define STORED_SIZE_OFFSET	16

class Stat
{
public:
	Stat()
	{
		Init();
	}
	void Add( int size )
	{
		totH++;    	curH++;	   if ( curH > maxH ) { maxH = curH; }
		totB+=size; curB+=size; if ( curB > maxB ) { maxB = curB; }
	}
	void Sub( int size )
	{
		curH--;
		curB-=size;
	}
	void Print( const char* header )
	{
		//QTRACE(TRACE_ALWAYS,("-NETZ: '%5s' Handles( cur=%4u max=%4u tot=%7u )  Bytes( cur=%8u max=%8u tot=%8u ave=%5u )\n",
		//	header, curH, maxH, totH, curB, maxB, totB, totH ? totB / totH : 0));
		//g_console.Echo("-NETZ: '%5s' Handles( cur=%4u max=%4u tot=%7u )  Bytes( cur=%8u max=%8u tot=%8u ave=%5u )\n",
		//	header, curH, maxH, totH, curB, maxB, totB, totH ? totB / totH : 0 );
		dbgPrint( "-NETZ: '%5s' Handles( cur=%4u max=%4u tot=%7u )  Bytes( cur=%8u max=%8u tot=%8u ave=%5u )\n",
			header, curH, maxH, totH, curB, maxB, totB, totH ? totB / totH : 0);
	}
	void Init()
	{
		totH = maxH = curH = totB = maxB = curB = 0;
	}
	int Average()
	{
		return ( totH > 0 ) ? ( totB / totH ) : 0;
	}
	unsigned int totH;
	unsigned int maxH;
	unsigned int curH;
	unsigned int totB;
	unsigned int maxB;
	unsigned int curB;
};

Stat NETZTOTAL;
DynamicArray<Stat> NETZSIZES(0, 128);

/******************************************************************************/
/******************************************************************************/
static void StatInc( int size )
{
	NETZTOTAL.Add( size );

	int i;
	for ( i = 0 ; i < NETZSIZES.Count() ; i++ )
		if ( NETZSIZES[ i ].Average() >= size )
			break;
	if ( i >= NETZSIZES.Count() || NETZSIZES[ i ].Average() != size )
		NETZSIZES.Add( Stat(), i );
	NETZSIZES[ i ].Add( size );

	static float nextUpdateTime = 0.0f;
	float curTime = g_timer.GetOSStartSec();
	if ( curTime >= nextUpdateTime )
	{
		nextUpdateTime = curTime + 10.0f;
		NETZTOTAL.Print( "TOTAL" );
		for ( i = 0 ; i < NETZSIZES.Count() ; i++ )
			NETZSIZES[ i ].Print( "--" );
	}
}

/******************************************************************************/
/******************************************************************************/
static void StatDec( int size )
{
	NETZTOTAL.Sub( size );

	int i;
	for ( i = 0 ; i < NETZSIZES.Count() ; i++ )
		if ( NETZSIZES[ i ].Average() == size )
			break;

	ASSERT( i < NETZSIZES.Count() );
	if ( i < NETZSIZES.Count() )
		NETZSIZES[ i ].Sub( size );
}

/******************************************************************************/
/******************************************************************************/
static void StatReset()
{
	NETZTOTAL.Init();
	NETZSIZES.Reset();
}

#else

#define STORED_SIZE_OFFSET		0
#define StatInc(x)				((void)0)
#define StatDec(x)				((void)0)
#define StatReset()				((void)0)

#endif

/******************************************************************************/
/******************************************************************************/

#ifdef USE_LINERS

	#define MEM_HEAD_LINER 16
	#define MEM_TAIL_LINER 16

	#define ALLOCATED1 0xdddaddda
	#define ALLOCATED2 0xdddbdddb
	#define FREED1     0xfffafffa
	#define FREED2     0xfffbfffb

#ifdef TRACK_ALLOCS

	static u32 l_allocCount = 0;

	#define SET_LINER( addr, value ) \
	{ \
		*(int*)((int)(addr) + 0 ) = (value); \
		*(int*)((int)(addr) + 4 ) = (value); \
		*(int*)((int)(addr) + 8 ) = (value); \
		*(int*)((int)(addr) + 12) = (l_allocCount); \
	}

	#define CHECK_LINER( addr, value ) \
	{ \
		ASSERT( *(int*)((int)(addr) + 0 ) == (value) && \
				*(int*)((int)(addr) + 4 ) == (value) && \
				*(int*)((int)(addr) + 8 ) == (value) ); \
	}

#else

	#define SET_LINER( addr, value ) \
	{ \
		*(int*)((int)(addr) + 0 ) = (value); \
		*(int*)((int)(addr) + 4 ) = (value); \
		*(int*)((int)(addr) + 8 ) = (value); \
		*(int*)((int)(addr) + 12) = (value); \
	}

	#define CHECK_LINER( addr, value ) \
	{ \
		ASSERT( *(int*)((int)(addr) + 0 ) == (value) && \
				*(int*)((int)(addr) + 4 ) == (value) && \
				*(int*)((int)(addr) + 8 ) == (value) && \
				*(int*)((int)(addr) + 12) == (value) ); \
	}

#endif

	#define CLEAR_BLOCK( addr, size ) \
		memset( (void*)(addr), 0xcd, (size) )

#else

	#define MEM_HEAD_LINER 0
	#define MEM_TAIL_LINER 0
	#define SET_LINER( addr, value )    ((void)0)
	#define CHECK_LINER( addr, value )  ((void)0)
	#define CLEAR_BLOCK( addr, size )   ((void)0)

#endif

/******************************************************************************/
/******************************************************************************/
class MemPool
{
public:
	MemPool( int blockSize, int numBlocks )
	{
		// only accept 16 byte multiples
		blockSize = ( blockSize + 15 ) & ~15;
		blockSize += MEM_HEAD_LINER + MEM_TAIL_LINER;
		blockSize += STORED_SIZE_OFFSET;

		// init member vars
		d_blockSize     = blockSize;
		d_numBlocks     = numBlocks;
		d_numFreeBlocks = 0;
		d_buffer        = NULL;
		d_freeBlocks    = NULL;
	}
	~MemPool()
	{
		Deinit();
	}
	void Deinit()
	{
		ASSERT( d_buffer == NULL || Empty() );
		d_numFreeBlocks = 0;
		d_buffer        = NULL;
		d_freeBlocks    = NULL;
	}
	bool Empty()
	{
		return d_numFreeBlocks == d_numBlocks;
	}
	void Dump()
	{
#ifdef TRACK_ALLOCS
		if ( d_buffer && d_numFreeBlocks != d_numBlocks )
		{
			for ( int i = 0 ; i < d_numBlocks ; i++ )
			{
				int blockAddr = (int)d_buffer + i * d_blockSize;
				if ( *(u32*)blockAddr == ALLOCATED1 )
				{
					dbgPrint( "ALLOCATED BLOCK: count ref=%d\n", *(u32*)(blockAddr + 12) );
				}
			}
		}
#endif
	}
	u32 SizeNeeded()
	{
		int dataSize  = d_numBlocks * d_blockSize;
		int freeSize  = d_numBlocks * sizeof( short );
		int totalSize = ( dataSize + freeSize + 15 ) & ~15;
		return totalSize;
	}
	void Init( void* blockAddr )
	{
		// init pointers to data
		ASSERT_PTR( blockAddr );
		ASSERT( d_buffer == NULL );
		ASSERT( d_freeBlocks == NULL );
		d_buffer     = blockAddr;
		d_freeBlocks = (short*) ( (int)d_buffer + ( d_numBlocks * d_blockSize ) );

		// init array of free blocks
		for ( int i = 0 ; i < d_numBlocks ; i++ )
		{
			d_freeBlocks[ i ] = i;
			int blockAddr = (int)d_buffer + i * d_blockSize;
			CLEAR_BLOCK( blockAddr, d_blockSize );
			SET_LINER( blockAddr, FREED1 );
			SET_LINER( blockAddr + d_blockSize - MEM_TAIL_LINER, FREED2 );
		}
		d_numFreeBlocks = d_numBlocks;
	}
	inline int BlockSize() const
	{
		return d_blockSize - MEM_HEAD_LINER - MEM_TAIL_LINER;
	}
	void* Alloc( int size )
	{
//		Verify();
		if ( size <= BlockSize() && d_numFreeBlocks )
		{
			int block = d_freeBlocks[ --d_numFreeBlocks ];
			int blockAddr = (int)d_buffer + block * d_blockSize;
			CHECK_LINER( blockAddr, FREED1 );
			CHECK_LINER( blockAddr + d_blockSize - MEM_TAIL_LINER, FREED2 );
			SET_LINER( blockAddr, ALLOCATED1 );
			SET_LINER( blockAddr + d_blockSize - MEM_TAIL_LINER, ALLOCATED2 );
			return (void*) ( blockAddr + MEM_HEAD_LINER );
		}
		else
		{
			return NULL;
		}
	}
	void  Free( void* ptr )
	{
//		Verify();
		int blockAddr = (int)ptr - MEM_HEAD_LINER;
		int block = ( blockAddr - (int)d_buffer ) / d_blockSize;
		ASSERT( block >= 0 && block < d_numBlocks );
		ASSERT( (int)d_buffer + block * d_blockSize == blockAddr );
		ASSERT( d_numFreeBlocks < d_numBlocks );

		CHECK_LINER( blockAddr, ALLOCATED1 );
		CHECK_LINER( blockAddr + d_blockSize - MEM_TAIL_LINER, ALLOCATED2 );
		CLEAR_BLOCK( blockAddr, d_blockSize );
		SET_LINER( blockAddr, FREED1 );
		SET_LINER( blockAddr + d_blockSize - MEM_TAIL_LINER, FREED2 );

		d_freeBlocks[ d_numFreeBlocks++ ] = block;
	}
	bool  Owns( void* ptr )
	{
		return ( (int)ptr >= (int)d_buffer &&
			      (int)ptr  < (int)d_buffer + d_numBlocks * d_blockSize );
	}
	bool	Valid()
	{
		return d_numBlocks > 0;
	}
	void Verify()
	{
		#ifdef USE_LINERS
		for ( int i = 0 ; i < d_numBlocks ; i++ )
		{
			int blockAddr = (int)d_buffer + i * d_blockSize;
			if ( *(int*)blockAddr == ALLOCATED1 )
			{
				CHECK_LINER( blockAddr, ALLOCATED1 );
				CHECK_LINER( blockAddr + d_blockSize - MEM_TAIL_LINER, ALLOCATED2 );
			}
			else if ( *(int*)blockAddr == FREED1 )
			{
				CHECK_LINER( blockAddr, FREED1 );
				CHECK_LINER( blockAddr + d_blockSize - MEM_TAIL_LINER, FREED2 );
			}
			else
			{
				ASSERTS( false, "corruption" );
			}
		}
		#endif
	}
private:
	int	 d_blockSize;
	int	 d_numBlocks;
	int    d_numFreeBlocks;
	void*  d_buffer;
	short* d_freeBlocks;
};

class MemPoolSet
{
public:
	MemPoolSet( MemPool* pools )
	{
		d_pools  = pools;
		d_size   = SizeNeeded();
		d_buffer = NULL;
	}
	~MemPoolSet()
	{
		Deinit();
	}
	bool Empty()
	{
		for ( MemPool* p = d_pools ; p->Valid() ; p++ )
			if ( !p->Empty() )
				return false;
		return true;
	}
	void Dump()
	{
		for ( MemPool* p = d_pools ; p->Valid() ; p++ )
			p->Dump();
	}
	void Deinit()
	{
		ASSERT( d_buffer == NULL || Empty() );
		for ( MemPool* p = d_pools ; p->Valid() ; p++ )
			p->Deinit();
		if ( d_buffer )
			memFree( d_buffer );
		d_buffer = NULL;
	}
	void Init()
	{
		ASSERT( d_buffer == NULL );
		d_buffer = memAlloc( d_size, 16 );
		u32 addr = (u32)d_buffer;
		for ( MemPool* p = d_pools ; p->Valid() ; p++ )
		{
			p->Init( (void*)addr );
			addr += p->SizeNeeded();
		}
	}
	int SizeNeeded()
	{
		int size = 0;
		for ( MemPool* p = d_pools ; p->Valid() ; p++ )
			size += p->SizeNeeded();
		return size;
	}
	void* Alloc( int size )
	{
		CScopeLockSemaphore lock( netzMemMutex );
		void* mem = NULL;
		for ( MemPool* p = d_pools ; p->Valid() ; p++ )
			if ( ( mem = p->Alloc( size ) ) != NULL )
				break;
		return mem;
	}
	void Free( void* ptr )
	{
		CScopeLockSemaphore lock( netzMemMutex );
		ASSERT( Owns( ptr ) );
		for ( MemPool* p = d_pools ; p->Valid() ; p++ )
		{
			if ( p->Owns( ptr ) )
			{
				p->Free( ptr );
				return;
			}
		}
		ASSERTS( false, "can't find memory in pools" );
		return;
	}
	bool Owns( void* ptr )
	{
		return d_buffer && ( ptr >= d_buffer && (u32)ptr < ( (u32)d_buffer + d_size ) );
	}
private:
	MemPool*	d_pools;
	void*		d_buffer;
	int		d_size;
};

/******************************************************************************/
/******************************************************************************/

static MemPool pools[] =
{
	#if defined(PS2)
	//[[ 10 ]] TOTAL: 1075024 (waste   75936, 7%) (rounded waste 1075024, 100%)
	//From NetZMemAnalyzer.pl
	MemPool(   32, 2553 ),
	MemPool(   48, 1484 ),
	MemPool(   96,  550 ),
	MemPool(  208,  298 ),
	MemPool(  272,  195 ),
	MemPool(  368,  151 ),
	MemPool(  544,  135 ),
	MemPool( 1040,  503 ),
	MemPool( 1360,   28 ),
	MemPool( 2192,   24 ),
	MemPool( 8208,    8 ),
	MemPool(65908,    1 ),  // for qnet
	MemPool(67724,    2 ),  // for qnet and OSI, here so it's not fragmented
	MemPool( 0, 0 ),
	#else
	// TOTAL:  1002024 + overhead
	MemPool(   80, 1820*3/2 ),
	MemPool(   96, 1899*3/2 ),
	MemPool(  176,  230*3/2 ),
	MemPool(  224,  381*3/2 ),
	MemPool(  352,  104*3/2 ),
	MemPool(  432,  136*3/2 ),
	MemPool(  624,   45*3/2 ),
	MemPool( 1088,   64*3/2 ),
	MemPool( 1632,   13*3/2 ),
	MemPool( 0, 0 ),
	#endif
};

static MemPoolSet* poolSet = NULL;

int allocCatch1 = -1;
int allocCatch2 = -1;

/******************************************************************************/
/******************************************************************************/
void* OurMalloc( size_t stSize ) 
{
	void* mem;

	ASSERTS( poolSet, "NetZ MALLOC called while not active" );

	#ifdef TRACK_ALLOCS
	if (l_allocCount == allocCatch1 || l_allocCount == allocCatch2)
	{
		dbgPrint("NetZ allocating %d\n", l_allocCount);
	}
	#endif

	#ifdef COLLECT_MEMORY_STATS
	StatInc( stSize );
	stSize += STORED_SIZE_OFFSET;	// space to store allocated size at start
	#endif

	if ( poolSet && ( mem = poolSet->Alloc( stSize ) ) != NULL )
	{
		// fall through
	}
	else
	{
//		memSetFitMethod( FIT_FIRST );	// faster
		mem = memAlloc( ( u32 )stSize );
//		memSetFitMethod( FIT_BEST );
	}

	// die, no ram!
	if ( mem == 0 )
		LOCKUP();

	#ifdef COLLECT_MEMORY_STATS
	// remember our size
	*(int*)mem = stSize - STORED_SIZE_OFFSET;
	mem = (void*) ( (int)mem + STORED_SIZE_OFFSET );
	#endif

	#ifdef TRACK_ALLOCS
	l_allocCount++;
	#endif

	return mem;
}

/******************************************************************************/
/******************************************************************************/
void OurFree( void *pBlock ) 
{
	if ( pBlock )
	{
		ASSERTS( poolSet, "NetZ FREE called while not active" );

		#ifdef COLLECT_MEMORY_STATS
		pBlock = (void*) ( (int)pBlock - STORED_SIZE_OFFSET );
		int size = *(int*)pBlock;
		StatDec( size );
		#endif

		if ( poolSet && poolSet->Owns( pBlock ) )
		{
			poolSet->Free( pBlock );
		}
		else
		{
			memFree( pBlock );
		}
	}
}

/******************************************************************************/
/******************************************************************************/
void AllocOurMem()
{
	if ( !netzMemMutex.IsInitialized() )
		netzMemMutex.Initialize();

	if ( poolSet == NULL )
	{
		poolSet = new MemPoolSet( pools );
		poolSet->Init();
	}

	// make NetZ use our memory manager for allocations
	MemoryManager::SetBasicMemoryFunctions(OurMalloc, OurFree);
}

/******************************************************************************/
/******************************************************************************/
void FreeOurMem( void )
{
	if ( poolSet )
	{
		if ( poolSet->Empty() )
		{
			delete poolSet;
			poolSet = NULL;
		}
		else
		{
			poolSet->Dump();
			ASSERTS( false, "NETZ HEAP ISN'T EMPTY, AND WE'RE TRYING TO RELEASE IT." );
		}
	}

	StatReset();
}

///////////////////////////// Performance Counters /////////////////////////////

/******************************************************************************/
/******************************************************************************/
TransportPerfCounters* CNetZConnection::GetPerformanceCounters(void)
{
	if (d_net != NULL)
	{
		Transport *pTransport =
				TransportProtocol::GetInstance()->FindTransport(_T("prudp"));
		if (pTransport != NULL)
			return(pTransport->GetPerfCounters());
	}
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
void CNetZConnection::ResetPerformanceCounters(void)
{
#ifdef TRACE_STATS
	d_nextStatTime = g_timer.GetOSTime();
	ResetBPSCounters();
#endif
}

/******************************************************************************/
/******************************************************************************/
uint32 CNetZConnection::GetPerformanceCount(int i_counterID, int i_counterValue)
{
#ifdef TRACE_STATS
	TransportPerfCounters* counters = GetPerformanceCounters();
	if (counters != NULL)
	{
		SystemError::ClearLast();
		uint32 value = counters->GetValue(i_counterID, i_counterValue);
		if (SystemError::GetLast() != 0)
		{
			char msg[256];
			g_console.Error("Error getting performance counter.\n");
			SystemError::GetErrorString(SystemError::GetLast(), msg, 256);
			g_console.Error("NetZError: %s\n", msg);
		}
		return(value);
	}
#endif
	return(0);
}

/**********************************************************************/
/**********************************************************************/
void CNetZConnection::ResetBPSCounters( void )
{
//#ifdef TRACE_STATS
	TransportPerfCounters* counters = GetPerformanceCounters();
	if (counters != NULL)
		counters->Reset();
//#endif
}

/**********************************************************************/
/**********************************************************************/
u32 CNetZConnection::GetBPSReceivedAvg( void )
{
	return(GetPerformanceCount(PERF_TRANSPORT_BPS_RECV, PERF_FREQUENCY_AVERAGE));
}

/**********************************************************************/
/**********************************************************************/
u32 CNetZConnection::GetBPSReceivedMax( void )
{
	return(GetPerformanceCount(PERF_TRANSPORT_BPS_RECV, PERF_FREQUENCY_MAX));
}

/**********************************************************************/
/**********************************************************************/
u32 CNetZConnection::GetBPSReceivedCur( void )
{
	return(GetPerformanceCount(PERF_TRANSPORT_BPS_RECV, PERF_FREQUENCY_LAST));
}

/**********************************************************************/
/**********************************************************************/
u32 CNetZConnection::GetBPSSentAvg( void )
{
	return(GetPerformanceCount(PERF_TRANSPORT_BPS_SENT, PERF_FREQUENCY_AVERAGE));
}

/**********************************************************************/
/**********************************************************************/
u32 CNetZConnection::GetBPSSentMax( void )
{
	return(GetPerformanceCount(PERF_TRANSPORT_BPS_SENT, PERF_FREQUENCY_MAX));
}

/**********************************************************************/
/**********************************************************************/
u32 CNetZConnection::GetBPSSentCur( void )
{
	return(GetPerformanceCount(PERF_TRANSPORT_BPS_SENT, PERF_FREQUENCY_LAST));
}

/**********************************************************************/
/**********************************************************************/
u32 CNetZConnection::GetPacketsReceived( void )
{
	return(GetPerformanceCount(PERF_TRANSPORT_PACKETS_RECV,
			PERF_FREQUENCY_AVERAGE));
}

/**********************************************************************/
/**********************************************************************/
u32 CNetZConnection::GetPacketsSent( void )
{
	return(GetPerformanceCount(PERF_TRANSPORT_PACKETS_SENT,
			PERF_FREQUENCY_AVERAGE));
}

/******************************************************************************/
/******************************************************************************/
u32 CNetZConnection::GetPacketsLost( void )
{
	return(GetPerformanceCount(PERF_TRANSPORT_PACKETS_LOST,
			PERF_FREQUENCY_LAST));
}

/******************************************************************************/
/******************************************************************************/
u32 CNetZConnection::GetPacketsLostTotal( void )
{
	return(GetPerformanceCount(PERF_TRANSPORT_PACKETS_LOST,
		PERF_COUNT_TOTAL));
}

/******************************************************************************/
/******************************************************************************/
u32 CNetZConnection::GetReliableTransmittedTotal( void )
{
#ifdef PS2
	return(GetPerformanceCount(PERF_TRANSPORT_RELIABLE_TRANSMIT,
		 PERF_COUNT_TOTAL));
#else
	return 0;
#endif
}

/******************************************************************************/
/******************************************************************************/
u32 CNetZConnection::GetReliableRetransmittedTotal( void )
{
#ifdef PS2
	return(GetPerformanceCount(PERF_TRANSPORT_RELIABLE_RETRANSMIT,
		PERF_COUNT_TOTAL));
#else
	return 0;
#endif
}

/******************************************************************************/
/******************************************************************************/
u32 CNetZConnection::GetReliableTransmittedCur( void )
{
#ifdef PS2
	return(GetPerformanceCount(PERF_TRANSPORT_RELIABLE_TRANSMIT,
		PERF_FREQUENCY_LAST));
#else
	return 0;
#endif
}

/******************************************************************************/
/******************************************************************************/
u32 CNetZConnection::GetReliableReTransmittedCur( void )
{
#ifdef PS2
	return(GetPerformanceCount(PERF_TRANSPORT_RELIABLE_TRANSMIT,
		PERF_FREQUENCY_LAST));
#else
	return 0;
#endif
}

/******************************************************************************/
/******************************************************************************/
void CNetZConnection::DumpCurrentStats(void)
{
#ifdef DWAIN_TRACE_STATS
	uint32 currentStatTime = g_timer.GetOSTime();
	if (d_nextStatTime < currentStatTime)
	{
		TransportPerfCounters* counters = GetPerformanceCounters();
		if (counters != NULL)
		{
			int bpsRecv = counters->GetValue(PERF_TRANSPORT_BPS_RECV,
					PERF_FREQUENCY_LAST);
			int bpsSend = counters->GetValue(PERF_TRANSPORT_BPS_SENT,
					PERF_FREQUENCY_LAST);
			int packetsRecv = counters->GetValue(PERF_TRANSPORT_PACKETS_RECV,
					PERF_FREQUENCY_LAST);
			int packetsSend = counters->GetValue(PERF_TRANSPORT_PACKETS_SENT,
					PERF_FREQUENCY_LAST);
			int packetsLost = counters->GetValue(PERF_TRANSPORT_PACKETS_LOST,
					PERF_FREQUENCY_LAST);

			int mUse = MemoryManager::GetDefaultMemoryManager()->GetMemoryUsage();
	
			QTRACE(TRACE_ALWAYS, ("Net-Z memory usage = %d", mUse));
			QTRACE(TRACE_ALWAYS, ("BitsPerSec Received = %d...Sent = %d",
					bpsRecv, bpsSend));
			QTRACE(TRACE_ALWAYS, ("Packets Received = %d...Sent = %d...Lost = %d",
					packetsRecv, packetsSend, packetsLost));
			g_console.Echo("Net-Z memory usage = %d", mUse);
			g_console.Echo("BitsPerSec Received = %d...Sent = %d",
					bpsRecv, bpsSend);
			g_console.Echo("Packets Received = %d...Sent = %d...Lost = %d",
					packetsRecv, packetsSend, packetsLost);
		}

		d_nextStatTime += TRACE_STAT_PERIOD_MS;
		while(d_nextStatTime < currentStatTime)
			d_nextStatTime += TRACE_STAT_PERIOD_MS;

	}
#endif
}

/******************************************************************************/
/******************************************************************************/
void CNetZConnection::DumpAverageStats(void)
{
#ifdef TRACE_STATS
	TransportPerfCounters* counters = GetPerformanceCounters();
	if (counters != NULL)
	{
		int bpsRecv = counters->GetValue(PERF_TRANSPORT_BPS_RECV,
				PERF_FREQUENCY_AVERAGE);
		int bpsSend = counters->GetValue(PERF_TRANSPORT_BPS_SENT,
				PERF_FREQUENCY_AVERAGE);
		int packetsRecv = counters->GetValue(PERF_TRANSPORT_PACKETS_RECV,
				PERF_FREQUENCY_AVERAGE);
		int packetsSend = counters->GetValue(PERF_TRANSPORT_PACKETS_SENT,
				PERF_FREQUENCY_AVERAGE);
		int packetsLost = counters->GetValue(PERF_TRANSPORT_PACKETS_LOST,
				PERF_FREQUENCY_AVERAGE);

		QTRACE(TRACE_ALWAYS, ("Avg BitsPerSec Received = %d...Sent = %d",
				bpsRecv, bpsSend));
		QTRACE(TRACE_ALWAYS, ("Avg Packets Received = %d...Sent = %d...Lost = %d",
				packetsRecv, packetsSend, packetsLost));
		g_console.Echo("Avg BitsPerSec Received = %d...Sent = %d",
				bpsRecv, bpsSend);
		g_console.Echo("Avg Packets Received = %d...Sent = %d...Lost = %d",
				packetsRecv, packetsSend, packetsLost);
	}
#endif
}

/******************************************************************************/
/******************************************************************************/
void CNetZConnection::FlushBundles(void)
{
	g_debugOutput.TSPrint(g_debugOutput.Category("NET"),
			"Flushing all NetZ bundles");
	Station::FlushAllBundles();
}

/******************************************************************************/
/******************************************************************************/
void CNetZConnection::ToggleBundling(void)
{
	if (BundlingPolicy::GetInstance()->IsEnabled())
	{
		g_debugOutput.TSPrint(g_debugOutput.Category("NET"),
				"Disabling NetZ bundling");
		BundlingPolicy::GetInstance()->Disable();
	}
	else
	{
		g_debugOutput.TSPrint(g_debugOutput.Category("NET"),
				"Enabling NetZ bundling");
		BundlingPolicy::GetInstance()->Enable();
	}
}

/******************************************************************************/
/******************************************************************************/
void CNetZConnection::MarkTrace(void)
{
	static int markCount = 0;
	g_debugOutput.TSPrint(g_debugOutput.Category("NET"),
			"Setting NetZ Trace Mark %d", markCount);
	QTRACE(TRACE_ALWAYS, ("+++++ Mark %d +++++", markCount++));
}

/////////////////////////////////// Logging ////////////////////////////////////

/******************************************************************************/
/******************************************************************************/
class AvalancheLogDeviceOutput : public LogDeviceDebugOutput
{
	virtual void Output(const LogEntry &oLogEntry)
	{
		dbgPrint( "%s\n", oLogEntry.CStr() );
	}
};

/******************************************************************************/
/******************************************************************************/
class CDebugOutputLogDevice : public LogDeviceDebugOutput
{
  public:
	CDebugOutputLogDevice() {d_category = INVALID_CATEGORY;}
	CDebugOutputLogDevice(uint8 i_category) {d_category = i_category;}
	virtual void Output(const LogEntry &i_logEntry)
	{
		g_debugOutput.TSPrint(d_category, "%s", i_logEntry.CStr());
	}

  private:
	uint8 d_category;
};

/******************************************************************************/
/******************************************************************************/
void CNetZConnection::InitializeLog(void)
{
	// Count connections for log file names.
	static uint32 connectionCount = 0;

#ifdef TRACE_LOG
	ASSERT(s_traceLogDevice == NULL);
#if TRACE_LOG_OUTPUT == DEBUG_LOG_OUTPUT
	// log traces in the debug output
	s_traceLogDevice = new AvalancheLogDeviceOutput();
#elif TRACE_LOG_OUTPUT == SCREEN_LOG_OUTPUT
	// log traces to the screen
	s_traceLogDevice = new CDebugOutputLogDevice(
			g_debugOutput.RegisterCategory("NetZ Log", "NETZ"));
#else // FILE_LOG_OUTPUT
	// log traces in a file
	char traceFileName[64];
	sprintf(traceFileName, "trace_%03d.log", connectionCount);
	s_traceLogDevice = new LogDeviceFile();
	((LogDeviceFile*)s_traceLogDevice)->SetFile( _T(traceFileName) );
#endif // TRACE_LOG_OUTPUT

	TraceLog::GetInstance()->SetLogDevice( s_traceLogDevice );
#if TRACE_LOG_OUTPUT != SCREEN_LOG_OUTPUT
	TraceLog::GetInstance()->GetOutputFormat()->ShowSessionTime( true );
	TraceLog::GetInstance()->GetOutputFormat()->ShowLocalTime( true );
#endif
#ifndef TRACE_LOG_DEBUG_SUPPORT
	TraceLog::GetInstance()->GetOutputFormat()->ShowDateTime( true );
#endif
	TraceLog::GetInstance()->SetFlag(s_netSettings.d_traceFlags);
#endif // TRACE_LOG

#ifdef EVENT_LOG
	ASSERT(s_eventLogDevice == NULL);
#if EVENT_LOG_OUTPUT == DEBUG_LOG_OUTPUT
	// log events in the debug output
	s_eventLogDevice = new AvalancheLogDeviceOutput();
#elif EVENT_LOG_OUTPUT == SCREEN_LOG_OUTPUT
	// log events to the screen
	s_eventLogDevice = new CDebugOutputLogDevice(
			g_debugOutput.RegisterCategory("NetZ Log", "NETZ"));
#else // FILE_LOG_OUTPUT
	// log events in a file
	char eventFileName[64];
	sprintf(eventFileName, "event_%03d.log", connectionCount);
	s_eventLogDevice = new LogDeviceFile();
	((LogDeviceFile*)s_eventLogDevice)->SetFile( _T(eventFileName) );
#endif // EVENT_LOG_OUTPUT

	EventLog::GetInstance()->SetLogDevice( s_eventLogDevice );
#if EVENT_LOG_OUTPUT != SCREEN_LOG_OUTPUT
	EventLog::GetInstance()->GetOutputFormat()->ShowSessionTime( true );
	EventLog::GetInstance()->GetOutputFormat()->ShowLocalTime( true );
	EventLog::GetInstance()->GetOutputFormat()->ShowDateTime( true );
#endif
	EventLog::GetInstance()->SetLogLevel(EventLog::Verbose);
#endif // EVENT_LOG

	connectionCount++;
}

/******************************************************************************/
/******************************************************************************/
void CNetZConnection::TerminateLog(void)
{
#ifdef TRACE_LOG
	// Deletes the device as well.
	TraceLog::GetInstance()->SetLogDevice(NULL);
	s_traceLogDevice = NULL;
#endif

#ifdef EVENT_LOG
	// Deletes the device as well.
	EventLog::GetInstance()->SetLogDevice(NULL);
	s_eventLogDevice = NULL;
#endif
}

////////////////////////////// Network Simulation //////////////////////////////

#ifdef SIMULATE_CONDITIONS
// ID changes made by console or watch.
enum
{
	PARAM_CHANGE_INPUT_PACKET_DROP_PROBABILITY,
	PARAM_CHANGE_INPUT_ROUTER_BUFFER_SIZE,
	PARAM_CHANGE_INPUT_BANDWIDTH,
	PARAM_CHANGE_INPUT_JITTER,
	PARAM_CHANGE_INPUT_LATENCY,
	PARAM_CHANGE_OUTPUT,
	PARAM_CHANGE_OUTPUT_BANDWIDTH,
	PARAM_CHANGE_OUTPUT_JITTER,
	PARAM_CHANGE_OUTPUT_LATENCY
};
#endif

/******************************************************************************/
/******************************************************************************/
void CNetZConnection::InitializeNetworkSimulation(void)
{
#ifdef SIMULATE_CONDITIONS
	// get the simulated network conditions parameters.
	d_simParam =
			CParam<SNetworkSimParam>::GetParam("SimulatedNetwork", "var");
	ASSERT_PTR(d_simParam);

	// Setup message handlers.
	d_simInputHandler.Initialize("SimulatedNetworkInputChange",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
	d_simOutputHandler.Initialize("SimulatedNetworkOutputChange",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);

	ResetNetworkSimulation();
#endif
}

/******************************************************************************/
/******************************************************************************/
void CNetZConnection::ResetNetworkSimulation(void)
{
#ifdef SIMULATE_CONDITIONS
	if (d_net != NULL)
	{
		Transport* pTransport = TransportProtocol::GetInstance()->FindTransport(
				PRUDP_TRANSPORT_TYPE);
		if (pTransport)
		{
			// simulate input network conditions
			InputEmulationDevice* input = pTransport->GetInputEmulationDevice();
			if (input != NULL)
			{
				input->Enable();
				input->SetPacketDropProbability(d_simParam->packetDropProbability);
				input->SetRouterBufferSize(d_simParam->routerBufferSize);
				input->SetBandwidth(d_simParam->inputBandwidth);
				input->SetJitter(d_simParam->inputJitter);
				input->SetLatency(d_simParam->inputLatency);

				QTRACE(TRACE_ALWAYS, ("Simulated Network PacketDropProbability = %f",
						input->GetPacketDropProbability()));
				QTRACE(TRACE_ALWAYS,
						("Simulated Input Bandwidth = %d...Jitter = %d...Latency = %d",
						input->GetBandwidth(),input->GetJitter(), input->GetLatency()));
			}

			// simulate output network conditions
			OutputEmulationDevice* output = pTransport->GetOutputEmulationDevice();
			if (output != NULL)
			{
				output->Enable();
				output->SetBandwidth(d_simParam->outputBandwidth);
				output->SetJitter(d_simParam->outputJitter);
				output->SetLatency(d_simParam->outputLatency);

				QTRACE(TRACE_ALWAYS,
						("Simulated Output Bandwidth = %d...Jitter = %d...Latency = %d",
						output->GetBandwidth(), output->GetJitter(),
						output->GetLatency()));
			}
		}
	}
#endif
}

/******************************************************************************/
/******************************************************************************/
void CNetZConnection::TerminateNetworkSimulation(void)
{
#ifdef SIMULATE_CONDITIONS
	d_simParam = NULL;

	g_messageDispatcher.Unregister(&d_simInputHandler);
	g_messageDispatcher.Unregister(&d_simOutputHandler);

	if (d_net != NULL)
	{
		Transport* pTransport = TransportProtocol::GetInstance()->FindTransport(
				PRUDP_TRANSPORT_TYPE);
		if (pTransport)
		{
			InputEmulationDevice* input = pTransport->GetInputEmulationDevice();
			if (input != NULL)
			{
				input->SetPacketDropProbability(0.0f)
				input->SetRouterBufferSize(ROUTER_BUFFER_SIZE_DEFAULT);
				input->SetBandwidth(INFINITE_BANDWIDTH);
				input->SetJitter(0);
				input->SetLatency(0);
			}

			OutputEmulationDevice* output = pTransport->GetOutputEmulationDevice();
			if (output != NULL)
			{
				output->SetBandwidth(INFINITE_BANDWIDTH);
				output->SetJitter(0);
				output->SetLatency(0);
			}
		}
	}
#endif
}

#ifdef SIMULATE_CONDITIONS
/******************************************************************************/
/******************************************************************************/
void CParam<SNetworkSimParam>::RegisterVariables(void)
{
	RegisterVariable( d_data.packetDropProbability, "packetDropProbability", 
			0.0f, 0.0f, 1.0f, (void *)PARAM_CHANGE_INPUT_PACKET_DROP_PROBABILITY );
	RegisterVariable( d_data.routerBufferSize, "routerBufferSize", 
			ROUTER_BUFFER_SIZE_DEFAULT, 0, ~0,
			(void *)PARAM_CHANGE_INPUT_ROUTER_BUFFER_SIZE );
	RegisterVariable( d_data.inputBandwidth, "inputBandwidth", 
			131072 /*INFINITE_BANDWIDTH*/, 0, ~0,
			(void *)PARAM_CHANGE_INPUT_BANDWIDTH );
	RegisterVariable( d_data.inputJitter, "inputJitter", 
			0, 0, ~0, (void *)PARAM_CHANGE_INPUT_JITTER );
	RegisterVariable( d_data.inputLatency, "inputLatency", 
			200 /*0*/, 0, ~0, (void *)PARAM_CHANGE_INPUT_LATENCY );
	RegisterVariable( d_data.outputBandwidth, "outputBandwidth", 
			131072 /*INFINITE_BANDWIDTH*/, 0, ~0,
			(void *)PARAM_CHANGE_OUTPUT_BANDWIDTH );
	RegisterVariable( d_data.outputJitter, "outputJitter", 
			0, 0, ~0, (void *)PARAM_CHANGE_OUTPUT_JITTER );
	RegisterVariable( d_data.outputLatency, "outputLatency", 
			200 /*0*/, 0, ~0, (void *)PARAM_CHANGE_OUTPUT_LATENCY );
}

/******************************************************************************/
/******************************************************************************/
void CParam<SNetworkSimParam>::CParamMessageHandler::HandleMessage(
		void* i_data, ActorHandle i_sender, bool i_posted)
{
	if ((uint)i_data < PARAM_CHANGE_OUTPUT)
		g_messageDispatcher.SendMessageToAll("SimulatedNetworkInputChange",
				i_data, i_sender);
	else
		g_messageDispatcher.SendMessageToAll("SimulatedNetworkOutputChange",
				i_data, i_sender);
}

/******************************************************************************/
/******************************************************************************/
void CNetZConnection::CSimInputHandler::HandleMessage(
		void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);

	if (d_ref->d_net != NULL)
	{
		Transport* pTransport = TransportProtocol::GetInstance()->FindTransport(
				PRUDP_TRANSPORT_TYPE);
		if (pTransport)
		{
			InputEmulationDevice* input = pTransport->GetInputEmulationDevice();
			if (input != NULL)
			{
				switch( (uint)i_data )
				{
				 case PARAM_CHANGE_INPUT_PACKET_DROP_PROBABILITY:
					input->SetPacketDropProbability(
							d_ref->d_simParam->packetDropProbability);
					QTRACE(TRACE_ALWAYS, ("New Packet Drop Probability = %f",
							d_ref->d_simParam->packetDropProbability));
					break;
				 case PARAM_CHANGE_INPUT_ROUTER_BUFFER_SIZE:
					input->SetRouterBufferSize(d_ref->d_simParam->routerBufferSize);
					QTRACE(TRACE_ALWAYS, ("New Router Buffer Size = %d",
							d_ref->d_simParam->routerBufferSize));
					break;
				 case PARAM_CHANGE_INPUT_BANDWIDTH:
					input->SetBandwidth(d_ref->d_simParam->inputBandwidth);
					QTRACE(TRACE_ALWAYS, ("New Input Bandwidth = %d",
							d_ref->d_simParam->inputBandwidth));
					break;
				 case PARAM_CHANGE_INPUT_JITTER:
					input->SetJitter(d_ref->d_simParam->inputJitter);
					QTRACE(TRACE_ALWAYS, ("New Input Jitter = %d",
							d_ref->d_simParam->inputJitter));
					break;
				 case PARAM_CHANGE_INPUT_LATENCY:
					input->SetLatency( d_ref->d_simParam->inputLatency);
					QTRACE(TRACE_ALWAYS, ("New Input Latency = %d",
							d_ref->d_simParam->inputLatency));
					break;
				}
			}
		}
	}
}

/******************************************************************************/
/******************************************************************************/
void CNetZConnection::CSimOutputHandler::HandleMessage(
		void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);

	if (d_ref->d_net != NULL)
	{
		Transport* pTransport = TransportProtocol::GetInstance()->FindTransport(
				PRUDP_TRANSPORT_TYPE);
		if (pTransport)
		{
			OutputEmulationDevice* output = pTransport->GetOutputEmulationDevice();
			if (output != NULL)
			{
				switch( (uint)i_data )
				{
				 case PARAM_CHANGE_OUTPUT_BANDWIDTH:
					output->SetBandwidth(d_ref->d_simParam->outputBandwidth);
					QTRACE(TRACE_ALWAYS, ("New Output Bandwidth = %d",
							d_ref->d_simParam->outputBandwidth));
					break;
				 case PARAM_CHANGE_OUTPUT_JITTER:
					output->SetJitter(d_ref->d_simParam->outputJitter);
					QTRACE(TRACE_ALWAYS, ("New Output Jitter = %d",
							d_ref->d_simParam->outputJitter));
					break;
				 case PARAM_CHANGE_OUTPUT_LATENCY:
					output->SetLatency(d_ref->d_simParam->outputLatency);
					QTRACE(TRACE_ALWAYS, ("New Output Latency = %d",
							d_ref->d_simParam->outputLatency));
					break;
				}
			}
		}
	}
}
#endif // SIMULATE_CONDITIONS

// ----------------------------------------------------------------------------
void CNetZConnection::loadNetworkDLLs( )
{
#if defined(PS2) && defined(SN)
	bool success = g_DllLoader.Load( "Voice.rel", false );
	ASSERTS( success, "!!! FATAL: unable to load Voice.rel !!!" );
#endif
}


// ----------------------------------------------------------------------------
void CNetZConnection::unLoadNetworkDLLs( )
{
#ifdef PS2
	if ( g_DllLoader.IsLoaded( "Voice.rel" ) )
	{
		bool success = g_DllLoader.Unload( "Voice.rel" );
		ASSERTS( success, "!!! FATAL: unable to unload Voice.rel !!!" );
	}
#endif
}

// ----------------------------------------------------------------------------

void CNetZConnection::LoadNetworkConfigSettings(void)
{
	char* context = "NetworkConfig";
	g_console.BindScript(context, "NetworkConfig");

	g_console.CreateVar(context, "bundle_enabled",
			s_netSettings.d_bundleEnabled);
	g_console.CreateVar(context, "bundle_mixed",
			s_netSettings.d_bundleMixed);
	g_console.CreateVar(context, "bundle_max_size",
			s_netSettings.d_bundleMaxSize);
	g_console.CreateVar(context, "bundle_max_flush_delay",
			s_netSettings.d_bundleMaxFlushDelay);
	g_console.CreateVar(context, "stream_bundle_enabled",
			s_netSettings.d_streamBundleEnabled);
	g_console.CreateVar(context, "stream_bundle_Max_flush_delay",
			s_netSettings.d_streamBundleMaxFlushDelay);
	g_console.CreateVar(context, "prudp_keep_alive_timeout",
			s_netSettings.d_prudpKeepAliveTimeout);
	g_console.CreateVar(context, "prudp_max_retransmission",
			s_netSettings.d_prudpMaxRetransmission);
	g_console.CreateVar(context, "prudp_long_max_silence_time",
			s_netSettings.d_prudpLongMaxSilenceTime);
	g_console.CreateVar(context, "prudp_short_max_silence_time",
			s_netSettings.d_prudpShortMaxSilenceTime);
	g_console.CreateVar(context, "prudp_PS2_time_slice",
			s_netSettings.d_prudpPS2TimeSlice);
	g_console.CreateVar(context, "prudp_other_time_slice",
			s_netSettings.d_prudpOtherTimeSlice);
	g_console.CreateVar(context, "prudp_window_size",
			s_netSettings.d_prudpWindowSize);
	g_console.CreateVar(context, "prudp_retransmit_mult",
			s_netSettings.d_prudpRetransmitTimeoutMultiplier);
	g_console.CreateVar(context, "prudp_extra_retransmit_mult",
			s_netSettings.d_prudpExtraRetransmitTimeoutMultiplier);
	g_console.CreateVar(context, "prudp_extra_retransmit_trigger",
			s_netSettings.d_prudpExtraRetransmitTimeoutTrigger);
	g_console.CreateVar(context, "prudp_RttRetransmit",
			s_netSettings.d_prudpRttRetransmit);
	g_console.CreateVar(context, "prudp_InitialRtt",
			s_netSettings.d_prudpInitialRtt);
	g_console.CreateVar(context, "stability_enabled",
		s_netSettings.d_connectionStabilityEnabled);
	g_console.CreateVar(context, "stability_minTime",
		s_netSettings.d_connectionStabilityMinThreshold);
	g_console.CreateVar(context, "stability_maxTime",
		s_netSettings.d_connectionStabilityMaxThreshold);
	g_console.CreateVar(context, "stability_minResent",
		s_netSettings.d_connectionStabilityResentThreshold);
	g_console.CreateVar(context, "stability_minLost",
		s_netSettings.d_connectionStabilityLostThreshold);
	g_console.CreateVar(context, "stability_minPlayers",
		s_netSettings.d_connectionStabilityMinPlayers);

	bool traceAll = false;
	g_console.CreateVar(context, "trace_all", traceAll);
	bool traceInit = ((s_netSettings.d_traceFlags & TRACE_INIT) != 0);
	g_console.CreateVar(context, "trace_init", traceInit);
	bool traceUpdate = ((s_netSettings.d_traceFlags & TRACE_UPDATE) != 0);
	g_console.CreateVar(context, "trace_update", traceUpdate);
	bool traceOperation = ((s_netSettings.d_traceFlags & TRACE_OPERATION) != 0);
	g_console.CreateVar(context, "trace_operation", traceOperation);
	bool traceFault = ((s_netSettings.d_traceFlags & TRACE_FAULT) != 0);
	g_console.CreateVar(context, "trace_fault", traceFault);
	bool traceDupspace = ((s_netSettings.d_traceFlags & TRACE_DUPSPACE) != 0);
	g_console.CreateVar(context, "trace_dupspace", traceDupspace);
	bool traceLoadBalancing = ((s_netSettings.d_traceFlags & TRACE_LOADBALANCING) != 0);
	g_console.CreateVar(context, "trace_load_balancing", traceLoadBalancing);
	bool tracePhbdr = ((s_netSettings.d_traceFlags & TRACE_PHBDR) != 0);
	g_console.CreateVar(context, "trace_phbdr", tracePhbdr);
	bool traceDOProtocol = ((s_netSettings.d_traceFlags & TRACE_DOPROTOCOL) != 0);
	g_console.CreateVar(context, "trace_doprotocol", traceDOProtocol);
	bool traceScheduler = ((s_netSettings.d_traceFlags & TRACE_SCHEDULER) != 0);
	g_console.CreateVar(context, "trace_scheduler", traceScheduler);
	bool traceUser = ((s_netSettings.d_traceFlags & TRACE_USER) != 0);
	g_console.CreateVar(context, "trace_user", traceUser);
	bool traceTeardown = ((s_netSettings.d_traceFlags & TRACE_TEARDOWN) != 0);
	g_console.CreateVar(context, "trace_teardown", traceTeardown);
	bool traceSessionDiscovery = ((s_netSettings.d_traceFlags & TRACE_SESSIONDISCOVERY) != 0);
	g_console.CreateVar(context, "trace_session_discovery", traceSessionDiscovery);
	bool traceConnection = ((s_netSettings.d_traceFlags & TRACE_CONNECTION) != 0);
	g_console.CreateVar(context, "trace_connection", traceConnection);
	bool traceSyncSim = ((s_netSettings.d_traceFlags & TRACE_SYNCSIM) != 0);
	g_console.CreateVar(context, "trace_sync_sim", traceSyncSim);
	bool traceServiceRequests = ((s_netSettings.d_traceFlags & TRACE_SERVICE_REQUESTS) != 0);
	g_console.CreateVar(context, "trace_service_requests", traceServiceRequests);
	bool traceSystemState = ((s_netSettings.d_traceFlags & TRACE_SYSTEM_STATE) != 0);
	g_console.CreateVar(context, "trace_system_state", traceSystemState);
	bool traceNat = ((s_netSettings.d_traceFlags & TRACE_NAT) != 0);
	g_console.CreateVar(context, "trace_nat", traceNat);
	bool traceRefCount = ((s_netSettings.d_traceFlags & D_TRACE_REFCOUNT) != 0);
	g_console.CreateVar(context, "trace_ref_count", traceRefCount);
	bool traceDOProtocolLevel2 = ((s_netSettings.d_traceFlags & D_TRACE_DOPROTOCOL_LVL_2) != 0);
	g_console.CreateVar(context, "trace_doprotocol_lvl_2", traceDOProtocolLevel2);
	bool traceDebug = ((s_netSettings.d_traceFlags & D_TRACE_DEBUG) != 0);
	g_console.CreateVar(context, "trace_debug", traceDebug);
	bool traceWait = ((s_netSettings.d_traceFlags & D_TRACE_WAIT) != 0);
	g_console.CreateVar(context, "trace_wait", traceWait);
	bool traceTimeSync = ((s_netSettings.d_traceFlags & D_TRACE_TIME_SYNC) != 0);
	g_console.CreateVar(context, "trace_time_sync", traceTimeSync);
	bool traceTransportConnect = ((s_netSettings.d_traceFlags & D_TRACE_TRANSPORT_CONNECT) != 0);
	g_console.CreateVar(context, "trace_transport_connect", traceTransportConnect);
	bool traceTransport = ((s_netSettings.d_traceFlags & D_TRACE_TRANSPORT ) != 0);
	g_console.CreateVar(context, "trace_transport", traceTransport);
	bool traceTransportProto = ((s_netSettings.d_traceFlags & D_TRACE_TRANSPORT_PROTO) != 0);
	g_console.CreateVar(context, "trace_transport_proto", traceTransportProto);
	bool traceMessageBundling = ((s_netSettings.d_traceFlags & D_TRACE_MESSAGE_BUNDLING  ) != 0);
	g_console.CreateVar(context, "trace_message_bundling", traceMessageBundling);
	bool traceProfiling = ((s_netSettings.d_traceFlags & D_TRACE_PROFILING) != 0);
	g_console.CreateVar(context, "trace_profiling", traceProfiling);
	bool traceTest = ((s_netSettings.d_traceFlags & D_TRACE_TEST) != 0);
	g_console.CreateVar(context, "trace_test", traceTest);
	bool traceMigration = ((s_netSettings.d_traceFlags & D_TRACE_MIGRATION) != 0);
	g_console.CreateVar(context, "trace_migration", traceMigration);

	g_console.ExecuteContext(context);
	g_console.DeleteContext(context);

	s_netSettings.d_traceFlags = 0;
	if (traceAll)
		s_netSettings.d_traceFlags |= TRACE_ALL;
	if (traceInit)
		s_netSettings.d_traceFlags |= TRACE_INIT;
	if (traceUpdate)
		s_netSettings.d_traceFlags |= TRACE_UPDATE;
	if (traceOperation)
		s_netSettings.d_traceFlags |= TRACE_OPERATION;
	if (traceFault)
		s_netSettings.d_traceFlags |= TRACE_FAULT;
	if (traceDupspace)
		s_netSettings.d_traceFlags |= TRACE_DUPSPACE;
	if (traceLoadBalancing)
		s_netSettings.d_traceFlags |= TRACE_LOADBALANCING;
	if (tracePhbdr)
		s_netSettings.d_traceFlags |= TRACE_PHBDR;
	if (traceDOProtocol)
		s_netSettings.d_traceFlags |= TRACE_DOPROTOCOL;
	if (traceScheduler)
		s_netSettings.d_traceFlags |= TRACE_SCHEDULER;
	if (traceUser)
		s_netSettings.d_traceFlags |= TRACE_USER;
	if (traceTeardown)
		s_netSettings.d_traceFlags |= TRACE_TEARDOWN;
	if (traceSessionDiscovery)
		s_netSettings.d_traceFlags |= TRACE_SESSIONDISCOVERY;
	if (traceConnection)
		s_netSettings.d_traceFlags |= TRACE_CONNECTION;
	if (traceSyncSim)
		s_netSettings.d_traceFlags |= TRACE_SYNCSIM;
	if (traceServiceRequests)
		s_netSettings.d_traceFlags |= TRACE_SERVICE_REQUESTS;
	if (traceSystemState)
		s_netSettings.d_traceFlags |= TRACE_SYSTEM_STATE;
	if (traceNat)
		s_netSettings.d_traceFlags |= TRACE_NAT;
	if (traceRefCount)
		s_netSettings.d_traceFlags |= D_TRACE_REFCOUNT;
	if (traceDOProtocolLevel2)
		s_netSettings.d_traceFlags |= D_TRACE_DOPROTOCOL_LVL_2;
	if (traceDebug)
		s_netSettings.d_traceFlags |= D_TRACE_DEBUG;
	if (traceWait)
		s_netSettings.d_traceFlags |= D_TRACE_WAIT;
	if (traceTimeSync)
		s_netSettings.d_traceFlags |= D_TRACE_TIME_SYNC;
	if (traceTransportConnect)
		s_netSettings.d_traceFlags |= D_TRACE_TRANSPORT_CONNECT;
	if (traceTransport)
		s_netSettings.d_traceFlags |= D_TRACE_TRANSPORT ;
	if (traceTransportProto)
		s_netSettings.d_traceFlags |= D_TRACE_TRANSPORT_PROTO;
	if (traceMessageBundling)
		s_netSettings.d_traceFlags |= D_TRACE_MESSAGE_BUNDLING  ;
	if (traceProfiling)
		s_netSettings.d_traceFlags |= D_TRACE_PROFILING;
	if (traceTest)
		s_netSettings.d_traceFlags |= D_TRACE_TEST;
	if (traceMigration)
		s_netSettings.d_traceFlags |= D_TRACE_MIGRATION;
}
