// ----------------------------------------------------------------------------
// File				-> NetZPlatformConnection.cpp
// Created			-> Jan 12, 2005
// Author			-> Dwain Skinner
// Description		-> Platform specific implementation of CNetZConnection member functions
// ----------------------------------------------------------------------------

// Precompiled header
#include "Network/NetworkPCH.h"

// --- quazal needs this defined now for memory tracking
#define LEAK_DETECTION_ENABLED

#include <netz.h>
#include <platform/stack/ps2common/ps2networking.h>
#include <platform/stack/ps2common/ps2profilelist.h>
#include <platform/stack/ps2common/ps2module.h>
#include <utilities/consoleio/consoleio.h>
#include <liblgaud.h>
#include <sifrpc.h>
#include <libdbc.h>
#include <libmc2.h>
#include <sifrpc.h>
#include <eznetcnf.h>
#include <libinsck.h>

#include "platform/PS2/PS2app.h"
#include "Network/NetworkPCH.h"
#include "Layers/Assert/Assert.h"
#include "Layers/Debug.h"
#include "EngineHelper/timer.h"
#include "Network/NetZ/NetZConnection.h"
#include <qnet.h>
#include "GameHelper/DebugOutput.h"

#include "platform/MemoryCard/MemoryCard.h"
#include "Cmdcon/cmdcon.h"

extern const char* g_RemoteIPAddress;
void	LoadModule( const char *module, int args, const char *argp );

// --- paths for NetZ
#ifdef CDROM
const PS2DirectoryPath 	oModulePath(PS2DirectoryPath::CDDVDDevice,_T("/data_ps2/irx/"));
const PS2DirectoryPath 	oNetConfigPath(PS2DirectoryPath::CDDVDDevice,_T("/data_ps2/conf/net/"));
#else
const PS2DirectoryPath 	oModulePath(PS2DirectoryPath::HostDevice,_T("data_ps2/irx/"));
const PS2DirectoryPath 	oNetConfigPath(PS2DirectoryPath::HostDevice,_T("data_ps2/conf/net/"));
#endif

static bool modulesLoaded = false;
static bool init = false;
static bool m_networkUp = false;
static PS2Networking::ConnectionState m_eState = PS2Networking::Stopped;
static u32 m_lastChecked;
static char m_networkMsg[128];
static PS2ProfileList oProfileList;
static u32 m_lastLinkUpdate = 0;
static PS2Networking::LinkStatus m_linkStatus = PS2Networking::Down;

// --- platform specific class
class CNetZPlatformConnection
{

public:
	static bool LoadAtWinMon( );
	static void InitiateConnection( PS2ConnectionProfile *pProfile );
	static bool WaitForConnection( int secs );
};

// ----------------------------------------------------------------------------
void CNetZConnection::InitPaths()
{
	PS2Networking::SetNetConfigPath( &oNetConfigPath );
	PS2Module::SetDefaultPath( &oModulePath );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
bool CNetZConnection::InitiateConnection( bool wait )
{
	bool success = false;
	init = true;
	m_networkUp = false;
	memset( m_networkMsg, 0, sizeof( m_networkMsg ) );

	// --- networking on atwinmon decks.
	if ( g_RemoteIPAddress != NULL )
	{
		m_networkUp = CNetZPlatformConnection::LoadAtWinMon( );
		return true;
	}

	// --- make sure NetZ knows where everything is
	InitPaths();

	// --- use the selected Your Network Configuration profile
	PS2ConnectionProfile *pProfile = NULL;
	PS2ProfileList *pProfileList = NULL;
	int index;

	CNetZConnection::GetNetworkConfiguration( (void**) &pProfileList, &index );
	if( pProfileList && index >= 0 )
	{
		pProfile = pProfileList->GetProfile( index );
	}
	else
	{
//		PS2Module::Load(_T("sio2man"));
//		PS2Module::Load(_T("dbcman"));
//		PS2Module::Load(_T("sio2d"));
//		PS2Module::Load(_T("mc2_d"));
//		sceDbcInit();
//		sceMc2Init(0);
		success = PS2Networking::ReadProfileListFromNetConfig( &oProfileList );
		pProfile = oProfileList.GetDefaultProfile( );
	}

	ASSERTS( pProfile, "pProfile == NULL, unable to get profile" );
	pProfile->Trace(TRACE_ALWAYS);

	// --- Load irx modules needed by the chosen connection profile
	//if( !modulesLoaded )
	{
		m_networkUp = modulesLoaded = ( success = PS2Networking::LoadProfileModules( pProfile ) );

//		// override insock malloc to use system instead of Quazal. !! Must be after LoadProfileModules
//		sceInsockSetMallocFunction( NULL );
//		sceInsockSetFreeFunction( NULL );

		ASSERTS( success, "unable to bootstrap the network" );
		if ( !success )
			return false;
	}

	// --- bring up the network connection
	CNetZPlatformConnection::InitiateConnection( pProfile );
	m_lastChecked = g_timer.GetOSTime() + ( 1 * 1000 );

	// --- wait for a connection to appear
	if( wait )
	{
		m_networkUp = CNetZPlatformConnection::WaitForConnection( 30 );
	
		if( !m_networkUp )
		{
			dbgPrint( "*** Connection Failed ***\n" );
			qBool bDisconnectionSuccessful = PS2Networking::TearDown( );
		}
		return( m_networkUp );
	}
	return( true );
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
bool CNetZPlatformConnection::LoadAtWinMon( void )
{
	dbgPrint( "ATWINMON DETECTED - Doing special network init\n" );
	PS2App* app = PS2App::IAm();
	if ( !app->LoadModule( "IRX\\USBD.IRX" ) )
		return false;
	if ( !app->LoadModule( "IRX\\LGAUD.IRX" ) )
		return false;
	lgAudInit( NULL, NULL );
	if ( !app->LoadModule( "IRX\\MSIFRPC.IRX" ) )
		return false;
	if ( !app->LoadModule( "IRX\\QNET.IRX" ) )
		return false;
	if ( !QNet::CreateInstance() )
		return false;
	return true;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
bool CNetZConnection::ConnectionUp( void )
{
	if ( g_RemoteIPAddress != NULL )
		return true;

	return( m_eState == PS2Networking::Started );
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
bool CNetZConnection::UpdateConnectionStatus( void )
{
	if ( g_RemoteIPAddress != NULL )
		return true;

	if( g_timer.GetOSTime() > m_lastChecked )
	{
		// --- only check every 1 second
		m_lastChecked = g_timer.GetOSTime() + ( 1 * 1000 );
		m_eState = PS2Networking::GetConnectionState( m_networkMsg, sizeof( m_networkMsg ) );
		if( m_eState == PS2Networking::Started ) 
			m_networkUp = true;
	}
	return( m_eState == PS2Networking::Started );

}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
bool CNetZConnection::TearDown( void )
{
	bool tryDisconnect = true;
	bool tryTearDown = true;
//	unLoadNetworkDLLs();

	if ( g_RemoteIPAddress != NULL )
		return true;

	g_MemCardManager->GetNetworkProfileList()->Clear();
	s_configured = NOT_CONFIGURED;

	int tries = 0;
	while ( tries < 10 )
	{
		m_eState = PS2Networking::GetConnectionState( m_networkMsg, sizeof(m_networkMsg) );
		g_console.Echo("PS2 Networking state = %d, message=%s\n", m_eState, m_networkMsg );
		switch ( m_eState )
		{
			// connected - start disconnect
			case PS2Networking::Starting:
			case PS2Networking::Retrying:
			case PS2Networking::Started:
				g_console.Echo( "---Calling Initiate Disconnect\n" );
				if( tryDisconnect )
				{
					PS2Networking::InitiateDisconnection();
					tryDisconnect = false;
				}
				break;

			// disconnect - start teardown
			case PS2Networking::Detached:
			case PS2Networking::Stopped:
				g_console.Echo( "---Calling Tear Down\n" );
				if( tryTearDown )
				{
					PS2Networking::TearDown();
					tryTearDown = false;
				}
				tries++;
				break;

			// torn down - we're finished
			case PS2Networking::Error:
				return true;

			// busy shutting down - wait
			case PS2Networking::Stopping:
			case PS2Networking::Detaching:
				break;

			default:
				ASSERTS( false, "unknown code from GetConnectionState" );
				break;
		}
		Platform::Sleep( 200 );
	}

	g_debugOutput.Print( "network teardown FAILED after multiple attempts" );
	return false;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
bool CNetZConnection::GetLinkStatus( void )
{
	if ( g_RemoteIPAddress != NULL )
		return true;

	// --- check the network link status every so often
	if( g_timer.GetOSTime() > m_lastLinkUpdate )
	{
		m_linkStatus = PS2Networking::GetLinkStatus();
		m_lastLinkUpdate = g_timer.GetOSTime() + ( 5 * 500 );
	}
	return( m_linkStatus == PS2Networking::Up );
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
void CNetZPlatformConnection::InitiateConnection( PS2ConnectionProfile *pProfile )
{
	if ( g_RemoteIPAddress != NULL )
		return;

	PS2Networking::InitiateConnection( pProfile );
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
bool CNetZPlatformConnection::WaitForConnection( int secs )
{
	if ( g_RemoteIPAddress != NULL )
		return true;

	// --- try to connect to the network
	qBool bDone = false;
	PS2Networking::ConnectionState eState = PS2Networking::Error;
	while( !bDone && secs ) 
	{
		Platform::Sleep( 1000 );
		qChar szMsg[ 100 ];
		eState = PS2Networking::GetConnectionState( szMsg, 100 );
		if( szMsg != NULL )  
		{
			dbgPrint( "*** Connection Status: %s\n", szMsg );
		} 
		bDone = ( ( eState == PS2Networking::Error ) || ( eState == PS2Networking::Started ) );
		secs--;
	}
	return( eState == PS2Networking::Started );
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
const char *CNetZConnection::GetConnectionErrorMessage(void )
{
	return( m_networkMsg );
}
// ----------------------------------------------------------------------------
