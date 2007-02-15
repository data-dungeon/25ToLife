//-----------------------------------------------------------------------------
// File: MutelistManager.cpp
//
// Desc: Tracks mutelists for online players
//
// Hist: 08.12.03 - New for the Nov 2003 XDK
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "MutelistManager.h"
#include <assert.h>
#include "cmdcon/cmdcon.h"
#include "voice/NetZ/CVoiceChat.h"

// # of users to shift off list when full
const DWORD MUTELIST_DECREMENT = 25;

//-----------------------------------------------------------------------------
// Name: CMutelistManager (ctor)
// Desc: Performs one-time initialization
//-----------------------------------------------------------------------------
CMutelistManager::CMutelistManager()
: m_muteListState( NotInitialized ),
  m_isSuspended( false )
{
	Clear();
}

//-----------------------------------------------------------------------------
void CMutelistManager::Clear()
{
	ASSERT( NotInitialized == m_muteListState );
	ASSERT( 0 == m_hMutelistStartup );

	for( DWORD i = 0; i < XGetPortCount(); i++ )
	{
		m_PlayerState[ i ]				= NotRegistered;
		m_hMutelistGet[ i ]				= NULL;
		m_MutelistUsers[ i ]			= NULL;
		m_TempMutelistUsers[ i ]		= NULL;
		m_dwNumMutelistUsers[ i ]		= 0;
		m_TempdwNumMutelistUsers[ i ]	= 0;
	}
	m_muteListState = NotInitialized;
}

//-----------------------------------------------------------------------------
void CMutelistManager::Suspend()
{
	if( m_hMutelistStartup ) { XOnlineTaskClose( m_hMutelistStartup ); }
	m_hMutelistStartup = 0;
	m_isSuspended = true;
}

//-----------------------------------------------------------------------------
void CMutelistManager::Resume()
{
	ASSERT( m_isSuspended );
	ASSERT( !m_hMutelistStartup );
	XOnlineMutelistStartup( NULL, &m_hMutelistStartup );
	m_isSuspended = false;
}

//-----------------------------------------------------------------------------
// Name: ~CMutelistManager (dtor)
// Desc: Ensures final cleanup
//-----------------------------------------------------------------------------
CMutelistManager::~CMutelistManager()
{
	Shutdown();
}


//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Acquires resources and performs other initialization
//-----------------------------------------------------------------------------
HRESULT CMutelistManager::Initialize()
{
	Clear();

	m_isSuspended = false;
   m_muteListState = NotReady;
	HRESULT hr = XOnlineMutelistStartup( NULL, &m_hMutelistStartup );
	if( FAILED( hr ) )
		return hr;

	return hr;
}


//-----------------------------------------------------------------------------
// Name: DoWork
// Desc: Continues to work on any pending tasks
//-----------------------------------------------------------------------------
HRESULT CMutelistManager::DoWork()
{
	if( NotInitialized == m_muteListState ) { return S_OK; }
	if( m_isSuspended ) { return S_OK; }

	// Keep pumping any pending mutelist gets
	for( DWORD i = 0; i < XGetPortCount(); i++ )
	{
		if( m_PlayerState[ i ] == GettingMutelist )
		{
			assert( m_hMutelistGet[ i ] != NULL );

			HRESULT hr = XOnlineTaskContinue( m_hMutelistGet[ i ] );
			if( hr != XONLINETASK_S_RUNNING )
			{
				XOnlineTaskClose( m_hMutelistGet[ i ] );
				m_hMutelistGet[ i ] = NULL;
				m_PlayerState[ i ]  = UpToDate;
				m_muteListState = JustBecameReady;
				g_console.Echo("Mute list is up to date, and contains %d players\n", m_dwNumMutelistUsers[ i ]);
				g_console.Echo("Temp Mute list is up to date, and contains %d players\n", m_TempdwNumMutelistUsers[ i ]);
				g_CVoiceChat.updateMuting();
			}
		}
	}

	XOnlineTaskContinue( m_hMutelistStartup );

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Shutdown
// Desc: Releases resources and performs other cleanup
//-----------------------------------------------------------------------------
HRESULT CMutelistManager::Shutdown()
{
	if( NotInitialized == m_muteListState ) { return S_OK; }

	for( DWORD i = 0; i < XGetPortCount(); i++ )
	{
		UnregisterLocalPlayer( i );
	}

	if( m_hMutelistStartup ) { XOnlineTaskClose( m_hMutelistStartup ); }
	m_hMutelistStartup = 0;
	m_muteListState = NotInitialized;

	return S_OK;
}

HRESULT CMutelistManager::RegisterLocalPlayer( DWORD dwPort )
{
	if( NotInitialized == m_muteListState ) { return E_FAIL; }
	if( m_isSuspended ) { return E_FAIL; }

	if( !m_MutelistUsers[ dwPort ] )
	{
		m_MutelistUsers[ dwPort ] = new XONLINE_MUTELISTUSER[ MAX_MUTELISTUSERS ];
		m_TempMutelistUsers[ dwPort ] = new XONLINE_MUTELISTUSER[ MAX_MUTELISTUSERS ];
		if( !m_MutelistUsers[ dwPort ] ) 
			return E_OUTOFMEMORY;
		if( !m_TempMutelistUsers[ dwPort ] ) 
			return E_OUTOFMEMORY;
	}
	return( GetMuteList( dwPort ) );
}

HRESULT CMutelistManager::GetMuteList( DWORD dwPort )
{
	if( NotInitialized == m_muteListState ) { return E_FAIL; }
	if( m_isSuspended ) { return E_FAIL; }

	m_muteListState = NotReady;
	if( m_MutelistUsers[ dwPort ] )
	{
		// Request the mutelist.  If this fails, we'll treat it as empty
		HRESULT hr = XOnlineMutelistGet( dwPort,
			MAX_MUTELISTUSERS,
			NULL,
			&m_hMutelistGet[ dwPort ],
			m_MutelistUsers[ dwPort ],
			&m_dwNumMutelistUsers[ dwPort ] );
		if( FAILED( hr ) )
		{
			m_PlayerState[ dwPort ] = UpToDate;
			m_muteListState = JustBecameReady;
		}
		else
		{
			m_PlayerState[ dwPort ] = GettingMutelist;
		}
		return S_OK;
	}
	return( S_FALSE );
}


HRESULT CMutelistManager::UnregisterLocalPlayer( DWORD dwPort )
{
	if( NotInitialized == m_muteListState ) { return E_FAIL; }
	if( m_isSuspended ) { return E_FAIL; }

	HRESULT hr = S_OK;

	// Shutdown any pending mutelist get
	if( m_hMutelistGet[ dwPort ] != NULL )
	{
		assert( m_PlayerState[ dwPort ] == GettingMutelist );

		do
		{
			hr = XOnlineTaskContinue( m_hMutelistGet[ dwPort ] );
		} while( hr == XONLINETASK_S_RUNNING );

		XOnlineTaskClose( m_hMutelistGet[ dwPort ] );
		m_hMutelistGet[ dwPort ] = NULL;
		m_dwNumMutelistUsers[ dwPort ] = 0;
	}

	if( m_MutelistUsers[ dwPort ] != NULL )
	{
		delete[] m_MutelistUsers[ dwPort ];
		m_MutelistUsers[ dwPort ] = NULL;
	}

	m_TempdwNumMutelistUsers[ dwPort ] = 0;
	if( m_TempMutelistUsers[ dwPort ] != NULL )
	{
		delete[] m_TempMutelistUsers[ dwPort ];
		m_TempMutelistUsers[ dwPort ] = NULL;
	}

	m_PlayerState[ dwPort ] = NotRegistered;

	return hr;
}

HRESULT CMutelistManager::MutePlayer( DWORD dwPort, XUID xuidPlayer )
{
	if( NotInitialized == m_muteListState ) { return E_FAIL; }
	if( m_isSuspended ) { return E_FAIL; }

	// Can't mute players while getting the list
	if( m_PlayerState[ dwPort ] != UpToDate )
		return E_FAIL;

	HRESULT hr = S_OK; //XOnlineMutelistAdd( dwPort, xuidPlayer );
	if( SUCCEEDED( hr ) )
	{
		// Once the initial list is populated, we handle our own updates
		// by emulating the mute list behavior.

		// If the player is not already in the list, add them.
		if( !IsPlayerMuted( dwPort, xuidPlayer ) )
		{
			if( m_TempdwNumMutelistUsers[ dwPort ] == MAX_MUTELISTUSERS )
			{
				memmove( &m_TempMutelistUsers[ dwPort ][ 0 ], 
					&m_TempMutelistUsers[ dwPort ][ MUTELIST_DECREMENT ],
					sizeof( XONLINE_MUTELISTUSER ) * ( MAX_MUTELISTUSERS - MUTELIST_DECREMENT ) );
				m_TempdwNumMutelistUsers[ dwPort ] -= MUTELIST_DECREMENT;
			}

			m_TempMutelistUsers[ dwPort ][ m_TempdwNumMutelistUsers[ dwPort ] ].xuid = xuidPlayer;
			++m_TempdwNumMutelistUsers[ dwPort ];
		}
		{
			g_console.Echo( "Player is already in my temp mute list, will not add him\n" );
		}
	}

	return hr;
}

HRESULT CMutelistManager::UnmutePlayer( DWORD dwPort, XUID xuidPlayer )
{
	if( NotInitialized == m_muteListState ) { return E_FAIL; }
	if( m_isSuspended ) { return E_FAIL; }

	// Can't mute players while getting the list
	if( m_PlayerState[ dwPort ] != UpToDate )
		return E_FAIL;


	HRESULT hr = S_OK; //XOnlineMutelistRemove( dwPort, xuidPlayer );
	DWORD dwIndex = GetTempMutelistIndex( dwPort, xuidPlayer );

	if( dwIndex < MAX_MUTELISTUSERS )
	{
		memmove( &m_TempMutelistUsers[ dwPort ][ dwIndex ],
			&m_TempMutelistUsers[ dwPort ][ dwIndex + 1 ],
			sizeof( XONLINE_MUTELISTUSER ) * ( m_TempdwNumMutelistUsers[ dwPort ] - dwIndex - 1 ) );
		--m_TempdwNumMutelistUsers[ dwPort ];
	}
	else
	{
		g_console.Echo( "Player does not exist in my temp mute list, will not unmute him\n" );
	}
	return hr;
}

BOOL CMutelistManager::IsPlayerMutedInMasterList( DWORD dwPort, XUID xuidPlayer )
{
	if( NotInitialized == m_muteListState ) { return TRUE; }
	if( NotReady == m_muteListState ) { return TRUE; }
	if( m_isSuspended ) { return TRUE; }

	return ( GetMutelistIndex( dwPort, xuidPlayer ) != MAX_MUTELISTUSERS  ) ;
}

BOOL CMutelistManager::IsPlayerMuted( DWORD dwPort, XUID xuidPlayer )
{
	if( NotInitialized == m_muteListState ) { return TRUE; }
	if( NotReady == m_muteListState ) { return TRUE; }
	if( m_isSuspended ) { return TRUE; }

	return ( ( GetMutelistIndex( dwPort, xuidPlayer ) != MAX_MUTELISTUSERS ) || GetTempMutelistIndex( dwPort, xuidPlayer ) != MAX_MUTELISTUSERS ) ;
}

BOOL CMutelistManager::IsUpToDate()
{
	if( NotInitialized == m_muteListState ) { return FALSE; }
	if( NotReady == m_muteListState ) { return FALSE; }
	if( m_isSuspended ) { return FALSE; }

	for( DWORD i = 0; i < XGetPortCount(); i++ )
	{
		if( m_PlayerState[ i ] == GettingMutelist )
			return FALSE;
	}

	return TRUE;
}

DWORD CMutelistManager::GetMutelistIndex( DWORD dwPort, XUID xuidPlayer )
{
	if( NotInitialized == m_muteListState ) { return MAX_MUTELISTUSERS; }

	// Note that if the player is not up-to-date, then m_dwNumMutelistUsers
	// will be zero, so we'll return MAX_MUTELISTUSERS as expected.
	g_console.Echo("There are %d users in my muteList\n", m_dwNumMutelistUsers[ dwPort ]);
	g_console.Echo("Looking for player xuid=%x\n", xuidPlayer.qwUserID );

	for( DWORD i = 0; i < m_dwNumMutelistUsers[ dwPort ]; i++ )
	{
		g_console.Echo("Mute list xuid=%x\n", m_MutelistUsers[dwPort][i].xuid.qwUserID);

		if( XOnlineAreUsersIdentical( &xuidPlayer,
			&m_MutelistUsers[ dwPort ][ i ].xuid ) )
		{
			g_console.Echo("Player xuid=%x found in my mute list\n", xuidPlayer.qwUserID );
			return i;
		}
	}

	return MAX_MUTELISTUSERS;
}


DWORD CMutelistManager::GetTempMutelistIndex( DWORD dwPort, XUID xuidPlayer )
{
	if( NotInitialized == m_muteListState ) { return MAX_MUTELISTUSERS; }

	// Note that if the player is not up-to-date, then m_dwNumMutelistUsers
	// will be zero, so we'll return MAX_MUTELISTUSERS as expected.
	g_console.Echo("There are %d users in my temp muteList\n", m_TempdwNumMutelistUsers[ dwPort ]);
	g_console.Echo("Looking for player xuid=%x\n", xuidPlayer.qwUserID );

	for( DWORD i = 0; i < m_TempdwNumMutelistUsers[ dwPort ]; i++ )
	{
		g_console.Echo("Mute list xuid=%x\n", m_TempMutelistUsers[dwPort][i].xuid.qwUserID);

		if( XOnlineAreUsersIdentical( &xuidPlayer,
			&m_TempMutelistUsers[ dwPort ][ i ].xuid ) )
		{
			g_console.Echo("Player xuid=%x found in my mute temp list\n", xuidPlayer.qwUserID );
			return i;
		}
	}

	return MAX_MUTELISTUSERS;
}

