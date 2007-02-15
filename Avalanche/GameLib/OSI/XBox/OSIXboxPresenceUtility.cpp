
#include "pch.hpp"
#include "OSIXbox.hpp"
#include "OSIXboxPresenceUtility.hpp"

///----------------------------------------------------------------------------
PlayerPresenceHelper
::PlayerPresenceHelper()
: m_presenceTask( (XONLINETASK_HANDLE) INVALID_HANDLE )
{
}

///----------------------------------------------------------------------------
PlayerPresenceHelper
::~PlayerPresenceHelper()
{
	Reset();
}

///----------------------------------------------------------------------------
void
PlayerPresenceHelper
::Reset()
{
	if( (XONLINETASK_HANDLE) INVALID_HANDLE != m_presenceTask )
	{
		XOnlineTaskClose( m_presenceTask );
		m_presenceTask = (XONLINETASK_HANDLE) INVALID_HANDLE;
	}

	const int userCnt = m_presenceUsers.Count();
	for( int userIter = 0; userIter < userCnt; userIter++ )
	{
		delete m_presenceUsers[ userIter ];
	}
	m_presenceUsers.Reset();
}

///----------------------------------------------------------------------------
void
PlayerPresenceHelper
::AddPlayer(
	const OSIPlayerID& playerID )
{
	if( (XONLINETASK_HANDLE) INVALID_HANDLE != m_presenceTask )
	{
		XOnlinePresenceClear( m_presenceTask );
		XOnlineTaskClose( m_presenceTask );
		m_presenceTask = (XONLINETASK_HANDLE) INVALID_HANDLE;
	}

	PresenceUser* newUser = new PresenceUser( playerID );
	m_presenceUsers.Add( newUser, -1 ); // -1 means push_back
}

///----------------------------------------------------------------------------
void
PlayerPresenceHelper
::Update()
{
	const int userCnt = m_presenceUsers.Count();
	if( !userCnt )
	{
		ASSERT( (XONLINETASK_HANDLE) INVALID_HANDLE == m_presenceTask );
		return;
	}

	if( (XONLINETASK_HANDLE) INVALID_HANDLE == m_presenceTask )
	{
		OSIXbox* osix = static_cast< OSIXbox* >( OSI::getSingletonPtr() );
		ASSERT( osix );
		HRESULT hr;
		hr = XOnlinePresenceInit( 
			osix->getActivePlayer()->state.currentControllerIndex, 
			0, 
			&m_presenceTask );
		if( FAILED( hr ) )
		{
			Reset();
			return;
		}
		for( int i = 0; i < userCnt; i++ )
		{	
			PresenceUser* user = m_presenceUsers[i];
			hr = XOnlinePresenceAdd( m_presenceTask, i, 1, &user->xuid );
			if( FAILED( hr ) )
			{
				Reset();
				return;
			}
		}
		hr = XOnlinePresenceSubmit( m_presenceTask );
		if( FAILED( hr ) )
		{
			Reset();
			return;
		}
	}

	XOnlineTaskContinue( m_presenceTask );

	for( int i = 0; i < userCnt; i++ )
	{
		PresenceUser* user = m_presenceUsers[i];
		const HRESULT hr = XOnlinePresenceGetLatest(
			m_presenceTask,
			i,
			1,
			&user->presence );
		if( FAILED( hr ) )
		{
			ASSERT( false ); // presence call failed
			Reset();
		}

		if( (user->presence.xuid.qwUserID == user->xuid.qwUserID) 
			&& memcmp( &user->presence, &user->oldPresence, sizeof( user->presence ) ) )
		{
			memcpy( &user->oldPresence, &user->presence, sizeof( user->presence ) );
			user->isUpdated = true;
		}
	}
}

///----------------------------------------------------------------------------
bool
PlayerPresenceHelper
::IsPlayerInfoNew(
	const OSIPlayerID& playerID )
const
{
	const int userCnt = m_presenceUsers.Count();
	for( int i = 0; i < userCnt; i++ )
	{
		PresenceUser* user = m_presenceUsers[ i ];
		if( user->xuid.qwUserID == playerID.xBox.userId )
		{
			return user->isUpdated;
		}
	}

	ASSERT( false ); // user ID not in list!
	return false;
}

///----------------------------------------------------------------------------
void
PlayerPresenceHelper
::SetIsPlayerInfoNew(
	const OSIPlayerID& playerID, 
	const bool isNew )
{
	const int userCnt = m_presenceUsers.Count();
	for( int i = 0; i < userCnt; i++ )
	{
		PresenceUser* user = m_presenceUsers[ i ];
		if( user->xuid.qwUserID == playerID.xBox.userId )
		{
			user->isUpdated = isNew;
			return;
		}
	}

	ASSERT( false ); // user ID not in list!
}

///----------------------------------------------------------------------------
void 
PlayerPresenceHelper
::GetPlayerInfo( 
	OSIPlayerID& playerID, 
	bool& isOnline, 
	bool& isPlaying, 
	bool& isJoinable, 
	OSIMatchID& matchID )
const
{
	isOnline = false;
	isPlaying = false;
	isJoinable = false;

	const int userCnt = m_presenceUsers.Count();
	PresenceUser* user = 0;
				 
	// find PresenceUser
	for( int userIdx = 0; userIdx < userCnt; userIdx++ )
	{
		if(	m_presenceUsers[ userIdx ]->xuid.qwUserID != playerID.xBox.userId )
		{
			continue;
		}
		user = m_presenceUsers[ userIdx ];
		break;
	}
	if( !user )
	{
		ASSERT( user );
		return;
	}

	ASSERT( user );

	XUID_to_OSIPlayerID( user->xuid, playerID );
	XNKID_to_OSIMatchID( user->presence.SessionID, matchID );
	isOnline = XONLINE_PRESENCE_FLAG_ONLINE == 
		(user->presence.dwUserState & XONLINE_PRESENCE_FLAG_ONLINE);
	isPlaying = XONLINE_PRESENCE_FLAG_PLAYING == 
		(user->presence.dwUserState & XONLINE_PRESENCE_FLAG_PLAYING);
	isJoinable = XONLINE_PRESENCE_FLAG_JOINABLE == 
		(user->presence.dwUserState & XONLINE_PRESENCE_FLAG_JOINABLE);

	if( user->presence.dwTitleID != OSIXbox::s_ThisTitleID )
	{
		matchID.quadword = 0;
		isJoinable = false;
	}
}

