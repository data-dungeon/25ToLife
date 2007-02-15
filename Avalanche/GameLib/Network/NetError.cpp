//	----------------------------------------------------------------------------
// File			-> NetError.cpp
// Created		-> Jan 31, 2005
// Author		-> Dwain Skinner
// Description	-> platform independent way to let the system know about network errors
// ----------------------------------------------------------------------------


// Precompiled header
#include "Network/NetworkPCH.h"
#include "Layers/Debug.h"
#include "EngineHelper/timer.h"

#include "Network/NetError.h"



CNetError::eNetworkError CNetError::s_lastError = CNetError::ERROR_NONE;
CNetError::eNetworkError CNetError::s_lastDisplayedError = CNetError::ERROR_NONE;
int CNetError::s_displayTime = 15;
const char *CNetError::s_file = NULL;
int CNetError::s_line = 0;
unsigned int CNetError::s_startDisplayTime = 0;
static char msgBuffer[ 2048 ];

// ------------------------------- Error Messages -----------------------------
static const char *ErrorMsgs[ CNetError::MAX_ERRORS ] =
{
	NULL,
	"Failed to create a connection",
	"Failed to create a session",
	"Failed to join the session",
	"No Session name was supplied to join or create",
	"Failed to create a gathering",
	"Failed to register the gathering",
	"Failed waiting to register the gathering",
	"Failed to join the gathering",
	"Failed waiting to join the gathering",
	"Failed to launch the session",
	"Failed waiting to launch the session",
	"Failed trying to create player agent",
};


// ----------------------------------------------------------------------------
void CNetError::SetNetworkErrorReal( eNetworkError error, int displayTimeSecs, const char *file, int line )
{
	ASSERTS( error < MAX_ERRORS, "undefined error" );
	ASSERTS( displayTimeSecs < 60, "Do you really want to display this for more then 60 secs" );
	s_lastError = error;
	s_lastDisplayedError = error;
	s_displayTime = displayTimeSecs;
	s_startDisplayTime = g_timer.GetOSTime();
	s_file = file;
	s_line = line;
}

// ----------------------------------------------------------------------------
const char *CNetError::GetErrorMessage( void )
{
	if( g_timer.GetOSTime() > ( s_startDisplayTime + ( s_displayTime * 1000 ) ) )
	{
		s_lastError = ERROR_NONE;
	}
	if( s_lastError != ERROR_NONE )
	{
		sprintf( msgBuffer, "%s\n%s(%d)", ErrorMsgs[ s_lastError ], s_file, s_line );
		return( msgBuffer );
	}
	return( NULL );
//	return( ErrorMsgs[ s_lastError ] );
}
