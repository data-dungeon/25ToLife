
#include "pch.hpp"
#include "OSIXbox.hpp"

///------------------------------------------------------------------------------------------------
//!	\brief Starts getting the friend list from UIX; only implemented with UIX support enabled.
//! \return True if the *request* (not the thing being requested) was successfully created.
///------------------------------------------------------------------------------------------------
bool OSIXbox::requestFriendList()
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );
	_setCurrentRequestType( REQUESTTYPE_FRIEND_LIST );
	const HRESULT getHR = _uix->GetFriendsForUser( getActivePlayer()->state.currentControllerIndex, &_uixFriendsList );
	if( FAILED( getHR ) )
	{
		_setError( getHR );
		_setCurrentRequestType( REQUESTTYPE_NONE );
		setLastRequestStatus( STATUS_FAILED );
		return REQUEST_FAILED;
	}
	return REQUEST_SUCCEEDED;

}


///------------------------------------------------------------------------------------------------
void OSIXbox::_processFriendList()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_FRIEND_LIST == _currentRequestType );
	ASSERT( _uixFriendsList );

	setLastRequestStatus( STATUS_WORKING );

	if( _uixFriendsList->IsReady() )
	{
		setLastRequestStatus( STATUS_SUCCEEDED );
		_setCurrentRequestType( REQUESTTYPE_NONE );

		// Populate the friend list manually, don't use FriendList::setNetworkDataReady.
		// This is because the XDK docs say:
		// "When the friends feature is active (displaying the friends list), the title cannot call 
		//  any ILiveFriendsList methods. Because of this, and because an extant ILiveFriendsList 
		//  consumes resources, the title should free all of its ILiveFriendsList interfaces before 
		//  starting the friends feature. The title should also not call any direct Xbox Live friends 
		//  functions (such as XOnlineFriendsGetLatest), while the friends feature or 
		//  ILiveFriendsList is active."
		
		// The problem with using _populateFriendsList/setNetworkDataReady is that there is no 
		// guarantee about when the client code will call getFriendsList()/_populateFriendsList, and
		// guarding against timing and resource issues (as per XDK doc blurb above) would just cause
		// a lot of unnecessary code to be written.  Why not just do it right here and be done?

		_friendList.clear();
		XONLINE_FRIEND xFriend;
		OSIPlayer osiPlayer;
		for( DWORD i = 0; i < _uixFriendsList->Count(); i++ )
		{
			_uixFriendsList->GetFriendByIndex( i, &xFriend );
			XUID_to_OSIPlayerID( xFriend.xuid, osiPlayer.id );
			OSIstrncpy( osiPlayer.name, xFriend.szGamertag, NUM_ARRAY_ELEMENTS( osiPlayer.name ) );
			osiPlayer.isAFriend						= (xFriend.dwFriendState & (XONLINE_FRIENDSTATE_FLAG_RECEIVEDREQUEST | XONLINE_FRIENDSTATE_FLAG_SENTREQUEST)) == 0;
			osiPlayer.state.currentControllerIndex	= -1;
			osiPlayer.state.isPlayingGame			= 0 != (xFriend.dwFriendState & XONLINE_FRIENDSTATE_FLAG_PLAYING);
			osiPlayer.state.isOnline				= 0 != (xFriend.dwFriendState & XONLINE_FRIENDSTATE_FLAG_ONLINE);
			osiPlayer.state.isCurrentGameJoinable	= 0 != (xFriend.dwFriendState & XONLINE_FRIENDSTATE_FLAG_JOINABLE);
			osiPlayer.state.isVoiceAvailable		= 0 != (xFriend.dwFriendState & XONLINE_FRIENDSTATE_FLAG_VOICE);
			_friendList.push_back( osiPlayer );
		}

		_uixFriendsList->Release();
		_uixFriendsList = 0;
	}
}

///------------------------------------------------------------------------------------------------
HRESULT OSIXbox::_setNotificationState( OSIPlayerState& state )
///------------------------------------------------------------------------------------------------
{
	if( !_uix ) { return E_FAIL; }
	if( !XOnlineGetLogonUsers() ) { return E_FAIL; }

	DWORD stateFlags = 0;
	if( state.isCurrentGameJoinable )		{ stateFlags |= XONLINE_FRIENDSTATE_FLAG_JOINABLE; }
	if( state.isOnline )					{ stateFlags |= XONLINE_FRIENDSTATE_FLAG_ONLINE; }
	if( state.isPlayingGame )				{ stateFlags |= XONLINE_FRIENDSTATE_FLAG_PLAYING; }
	if( state.isVoiceAvailable )			{ stateFlags |= XONLINE_FRIENDSTATE_FLAG_VOICE; }

	XNKID nkid;
	if( _currentMatch.id.isValid() )
	{
		OSIMatchID_to_XNKID( _currentMatch.id, nkid );
	}
	else
	{
		memset( &nkid, 0, sizeof( nkid ) );
	}

	const HRESULT setStateHR = _uix->NotificationSetState(
		state.currentControllerIndex,
		stateFlags,
		nkid,
		0, NULL ); // no userdata

	return setStateHR;
}

///------------------------------------------------------------------------------------------------
//!	\brief Update the player's online state information for his/her friends; only implemented with UIX support enabled.
//! \return True if the *request* (not the thing being requested) was successfully created.
//! \param state The player state data that should be published.
///------------------------------------------------------------------------------------------------
bool OSIXbox::requestPublishOnlineState( OSIPlayerState& state )
///------------------------------------------------------------------------------------------------
{
	// UIX handles any async issues for us
	_setCurrentRequestType( REQUESTTYPE_NONE );

	HRESULT setStateHR = _setNotificationState( state );
	if( FAILED( setStateHR ) )
	{
		_setError( setStateHR );
		setLastRequestStatus( STATUS_FAILED );
		return REQUEST_FAILED;
	}
	else
	{
		setLastRequestStatus( STATUS_SUCCEEDED );
		return REQUEST_SUCCEEDED;
	}
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::quietPublishOnlineState( OSIPlayerState& state )
///------------------------------------------------------------------------------------------------
{
	return SUCCEEDED( _setNotificationState( state ) );
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestOfferMyFriendship( OSIPlayerID& id )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );

	if( !id.isValid() )
	{
		_setError( E_FAIL ); 
		setLastRequestStatus( STATUS_FAILED );
		return false;
	}

	OSIPlayerID_to_XUID( id, _requestPlayerXUID );

	HRESULT hr = XOnlineFriendsRequest(
		getActivePlayer()->state.currentControllerIndex,
		_requestPlayerXUID );

	if( FAILED( hr ) )
	{
		_setError( hr );
		setLastRequestStatus( STATUS_FAILED );
		return REQUEST_FAILED;
	}
	else
	{
		setLastRequestStatus( STATUS_SUCCEEDED );
		return REQUEST_SUCCEEDED;
	}
}
