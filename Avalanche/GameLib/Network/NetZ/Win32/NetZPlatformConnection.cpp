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
#include "Layers/Assert/Assert.h"
#include "Layers/Debug.h"
#include "Network/NetZ/NetZConnection.h"

bool s_connectionUp = false;
static char m_networkMsg[128];

// ----------------------------------------------------------------------------
bool CNetZConnection::InitiateConnection( bool wait )
{
	memset( m_networkMsg, 0, sizeof( m_networkMsg ) );
	s_connectionUp = GetLinkStatus();
	return( s_connectionUp );
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
void CNetZConnection::InitPaths()
{
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
bool CNetZConnection::UpdateConnectionStatus( void )
{
	return( s_connectionUp );
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
bool CNetZConnection::TearDown( void )
{
	s_connectionUp = false;
	return( true );
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
bool CNetZConnection::GetLinkStatus( void )
{
#ifdef _XBOX
	if( XNetGetEthernetLinkStatus() )
		return( true );
	return( false );
#else
	return( true );
#endif
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
bool CNetZConnection::ConnectionUp( void )
{
	return( s_connectionUp );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
const char *CNetZConnection::GetConnectionErrorMessage(void )
{
	return( m_networkMsg );
}
// ----------------------------------------------------------------------------
