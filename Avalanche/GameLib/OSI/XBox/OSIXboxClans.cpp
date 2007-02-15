
#include "pch.hpp"
#include "OSIXbox.hpp"
#include <memory>
#include "OSIXboxClansUtility.hpp"
#include "OSIXboxStatsUtility.hpp"
#include "platform/xbox/xboxlive.h"

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestClanCreate( const OSIchar* const tag, const OSIchar* const name )
///------------------------------------------------------------------------------------------------
{ 
	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );
	ASSERT( !_currentOnlineTaskHandle );

	if( !tag || !tag[0] || !name || !name[0] )
	{
		_setError( E_FAIL ); 
		setLastRequestStatus( STATUS_FAILED );
		return false;
	}

	XONLINE_TEAM_PROPERTIES			teamProps;
	XONLINE_TEAM_MEMBER_PROPERTIES	memberProps;
	
	// initialize the team properties
	OSIstrncpy( teamProps.wszTeamName, tag, NUM_ARRAY_ELEMENTS( teamProps.wszTeamName ) );
	OSIstrncpy( teamProps.wszDescription, name, NUM_ARRAY_ELEMENTS( teamProps.wszDescription ) );
	teamProps.wszMotto[0]	= 0;
	teamProps.wszURL[0]		= 0;
	teamProps.TeamDataSize	= 0; //FIXME: how big does this need to be?

	// initialize the member properties, give creator total control over team
	memberProps.dwPrivileges = s_ClanLeaderPrivileges;
	memberProps.TeamMemberDataSize = 0; //FIXME: how big does this need to be?
	
	const HRESULT createHR = XOnlineTeamCreate( 
		getActivePlayer()->state.currentControllerIndex,
		&teamProps,
		&memberProps,
		OSI_MAX_CLAN_MEMBERS,
		NULL,
		&_currentOnlineTaskHandle );

	if( FAILED( createHR ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setError( createHR );
		return REQUEST_FAILED;
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_CLAN_CREATE );
		return REQUEST_SUCCEEDED;
	}
}


///------------------------------------------------------------------------------------------------
bool OSIXbox::_clanAnswerInvitation( const OSIClanInvitation& invitation, const XONLINE_PEER_ANSWER_TYPE answer, const RequestType requestType )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );
	ASSERT( !_currentOnlineTaskHandle );

	OSIClanID_to_XUID( invitation.clanInfo.clanID, _requestClanXUID );
	
	const HRESULT answerHR = XOnlineTeamMemberAnswerRecruit(
		getActivePlayer()->state.currentControllerIndex,
		_requestClanXUID,
		answer,
		NULL,
		&_currentOnlineTaskHandle );

	if( FAILED( answerHR ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setError( answerHR );
		return REQUEST_FAILED;
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( requestType );
		return REQUEST_SUCCEEDED;
	}
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestClanUpdateMyClanInformation()
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );
	ASSERT( !_currentOnlineTaskHandle );

	OSIPlayerID_to_XUID( getActivePlayer()->id, _requestPlayerXUID );

	getActivePlayer()->clanInfo = OSIClanInfo(); // re-initialize
	getActivePlayer()->clanRole = OSI_CLAN_ROLE_UNKNOWN;

	const HRESULT enumerateHR = XOnlineTeamEnumerateByUserXUID(
		getActivePlayer()->state.currentControllerIndex,
		_requestPlayerXUID,
		NULL,
		&_currentOnlineTaskHandle );

	if( FAILED( enumerateHR ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setError( enumerateHR );
		return REQUEST_FAILED;
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_CLAN_MYINFO );
		_requestState = REQUESTSTATE_CLAN_MYINFO_GET_ID;
		return REQUEST_SUCCEEDED;
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanUpdateMyClanInformation()
///------------------------------------------------------------------------------------------------
{
	switch( _requestState )
	{
	case REQUESTSTATE_CLAN_MYINFO_GET_ID:		_processClanUpdateMyClanInformationGetID(); break;
	case REQUESTSTATE_CLAN_MYINFO_GET_MEMBERS:	_processClanUpdateMyClanInformationGetMembers(); break;
	case REQUESTSTATE_CLAN_MYINFO_GET_STATS:	_processClanUpdateMyClanInformationGetStats(); break;
	default: ASSERT( 0 );
	};
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanUpdateMyClanInformationGetID()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_MYINFO == _currentRequestType );
	ASSERT( _currentOnlineTaskHandle );
	ASSERT( REQUESTSTATE_CLAN_MYINFO_GET_ID == _requestState );

	const XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	const OSIRequestStatus status = getLastRequestStatus();

	if( STATUS_WORKING == status )
	{
		return;
	}

	else if( STATUS_FAILED == status )
	{
		_requestState = REQUESTSTATE_NONE;
		XOnlineTaskClose( taskHandle );
		return;
	}

	// task succeeded

	DWORD teamCount;
	XUID teamIDs[ XONLINE_MAX_TEAM_COUNT ];
	const HRESULT getResultsHR = XOnlineTeamEnumerateGetResults( taskHandle, &teamCount, teamIDs );
	if( FAILED( getResultsHR ) )
	{
		XOnlineTaskClose( taskHandle );
		setLastRequestStatus( STATUS_FAILED );
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_setError( getResultsHR );
		return;
	}

	// not on a team - stop here
	if( 0 == teamCount )
	{
		XOnlineTaskClose( taskHandle );
		return;
	}

	// on a team - go get the info
	XONLINE_TEAM teamInfo;
	const HRESULT getDetailsHR = XOnlineTeamGetDetails( taskHandle, teamIDs[0], &teamInfo );
	XOnlineTaskClose( taskHandle ); // don't need taskhandle after this point
	if( FAILED( getDetailsHR ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_setError( getDetailsHR );
		return;
	}

	XUID_to_OSIClanID( teamInfo.xuidTeam, getActivePlayer()->clanInfo.clanID );
	getActivePlayer()->clanInfo.memberCount = teamInfo.dwMemberCount;
	OSIstrncpy( getActivePlayer()->clanInfo.name, teamInfo.TeamProperties.wszDescription, NUM_ARRAY_ELEMENTS( getActivePlayer()->clanInfo.name ) );
	OSIstrncpy( getActivePlayer()->clanInfo.tag, teamInfo.TeamProperties.wszTeamName, NUM_ARRAY_ELEMENTS( getActivePlayer()->clanInfo.tag ) );

	// now go get rank
	const HRESULT memberEnumerateHR = XOnlineTeamMembersEnumerate(
		getActivePlayer()->state.currentControllerIndex,
		teamInfo.xuidTeam,
		0, 
		NULL,
		&_currentOnlineTaskHandle );
	if( FAILED( memberEnumerateHR ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_setError( memberEnumerateHR );
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_CLAN_MYINFO );
		_requestState = REQUESTSTATE_CLAN_MYINFO_GET_MEMBERS;
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanUpdateMyClanInformationGetMembers()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_MYINFO == _currentRequestType );
	ASSERT( _currentOnlineTaskHandle );
	ASSERT( REQUESTSTATE_CLAN_MYINFO_GET_MEMBERS == _requestState );

	const XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	const OSIRequestStatus status = getLastRequestStatus();

	if( STATUS_WORKING == status )
	{
		return;
	}

	else if( STATUS_FAILED == status )
	{
		_requestState = REQUESTSTATE_NONE;
		XOnlineTaskClose( taskHandle );
		return;
	}

	// task succeeded

	{ // just need to call XOnlineTeamMembersEnumerateGetResults before XOnlineTeamMemberGetDetails, don't care about the results
		DWORD memberCount;
		XUID members[ XONLINE_MAX_TEAM_MEMBER_COUNT ];
		const HRESULT getResultsHR = XOnlineTeamMembersEnumerateGetResults( taskHandle, &memberCount, members );
		if( FAILED( getResultsHR ) )
		{
			XOnlineTaskClose( taskHandle );
			setLastRequestStatus( STATUS_FAILED );
			_setCurrentRequestType( REQUESTTYPE_NONE );
			_setError( getResultsHR );
			return;
		}

		// iterate the list of members and find the leader
		for( unsigned int i = 0; i < memberCount; i++ )
		{
			XONLINE_TEAM_MEMBER memberInfo;
			const HRESULT getDetailsHR = XOnlineTeamMemberGetDetails( taskHandle, members[ i ], &memberInfo );
			if( memberInfo.TeamMemberProperties.dwPrivileges == s_ClanLeaderPrivileges )
			{
				OSIstrncpy( 
					getActivePlayer()->clanInfo.leaderName, 
					memberInfo.szGamertag, 
					NUM_ARRAY_ELEMENTS( getActivePlayer()->clanInfo.leaderName ) );
			}
		}
	}

	XONLINE_TEAM_MEMBER memberInfo;
	const HRESULT getDetailsHR = XOnlineTeamMemberGetDetails( taskHandle, _requestPlayerXUID, &memberInfo );
	XOnlineTaskClose( taskHandle ); // don't need task handle from here on
	if( FAILED( getDetailsHR ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_setError( getDetailsHR );
		return;
	}

    getActivePlayer()->clanRole = XboxPrivileges_to_OSIClanRole( memberInfo.TeamMemberProperties.dwPrivileges );

	// kick off clan stats download
	
	_scratchClanInfoList.clear();
	_scratchClanInfoList.push_back( getActivePlayer()->clanInfo );

	StatsAdapter::OSICreateSingleton();
	StatsAdapter* statIO = StatsAdapter::OSIGetSingleton();
	HRESULT hr = statIO->BeginDownload( _currentOnlineTaskHandle, NULL, &_scratchClanInfoList );
	if( FAILED( hr ) )
	{
		StatsAdapter::OSIDestroySingleton();
		setLastRequestStatus( STATUS_FAILED );
		_setError( hr );
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_CLAN_MYINFO );
		_requestState = REQUESTSTATE_CLAN_MYINFO_GET_STATS;
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanUpdateMyClanInformationGetStats()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_MYINFO == _currentRequestType );
	ASSERT( REQUESTSTATE_CLAN_MYINFO_GET_STATS == _requestState );
	ASSERT( _currentOnlineTaskHandle );

	const XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	const OSIRequestStatus status = getLastRequestStatus();
	if( STATUS_WORKING == status )
	{
		return;
	}

	else if( STATUS_FAILED == status )
	{
		StatsAdapter::OSIDestroySingleton();
		_scratchClanInfoList.clear();
		return;
	}

	// task succeeded

	StatsAdapter* statIO = StatsAdapter::OSIGetSingleton();
	HRESULT hr = statIO->ProcessDownloadResult( taskHandle );
	if( FAILED( hr ) )
	{
		StatsAdapter::OSIDestroySingleton();
		setLastRequestStatus( STATUS_FAILED );
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_setError( hr );
		_scratchClanInfoList.clear();
		return;
	}

	if( !statIO->IsDownloadComplete() )
	{
		hr = statIO->ContinueDownload( _currentOnlineTaskHandle );
        if( FAILED( hr ) )
		{
			StatsAdapter::OSIDestroySingleton();
			setLastRequestStatus( STATUS_FAILED );
			_setError( hr );
			_scratchClanInfoList.clear();
		}
		else
		{
			setLastRequestStatus( STATUS_WORKING );
			_setCurrentRequestType( REQUESTTYPE_CLAN_MYINFO );
			_requestState = REQUESTSTATE_CLAN_MYINFO_GET_MEMBERS;
		}
		return;
	}

	// download is complete

	// _commonXonlineTaskProcessing should have set up OSI for a success state already
	// At this point, every clan in _scratchClanInfoList should have valid stats.
	StatsAdapter::OSIDestroySingleton();

	OSIClanInfo* clanInfoWithStats = _scratchClanInfoList.findClanInfoByID( getActivePlayer()->clanInfo.clanID );
	if( clanInfoWithStats )
	{
		getActivePlayer()->clanInfo = *clanInfoWithStats;
	}

	_scratchClanInfoList.clear();
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestClanJoinByInvitation( const OSIClanInvitation& invitation )
///------------------------------------------------------------------------------------------------
{
	return _clanAnswerInvitation( 
		invitation, 
		XONLINE_PEER_ANSWER_YES, 
		REQUESTTYPE_CLAN_JOIN_BY_INVITATION );
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestClanDenyInvitation( const OSIClanInvitation& invitation )
///------------------------------------------------------------------------------------------------
{ 
	return _clanAnswerInvitation( 
		invitation, 
		XONLINE_PEER_ANSWER_NO, 
		REQUESTTYPE_CLAN_DENY_INVITATION );
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestClanBlockInvitation( const OSIClanInvitation& invitation )
///------------------------------------------------------------------------------------------------
{ 
	return _clanAnswerInvitation( 
		invitation, 
		XONLINE_PEER_ANSWER_NEVER, 
		REQUESTTYPE_CLAN_BLOCK_INVITATION );
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestClanSendInvitation( const OSIClanID& clanID, const OSIchar* const playerName )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

    ASSERT( REQUESTTYPE_NONE == _currentRequestType );
	ASSERT( !_currentOnlineTaskHandle );
	ASSERT( playerName && playerName[0] );

	if( !clanID.isValid() || !playerName || !playerName[0] )
	{
		_setError( E_FAIL ); 
		setLastRequestStatus( STATUS_FAILED );
		return false;
	}


	OSIstrncpy( _requestPlayerName, playerName, NUM_ARRAY_ELEMENTS( _requestPlayerName ) );
	OSIClanID_to_XUID( clanID, _requestClanXUID );

	const HRESULT enumerateHR = XOnlineTeamMembersEnumerate( 
		getActivePlayer()->state.currentControllerIndex,
		_requestClanXUID,
		XONLINE_TEAM_SHOW_RECRUITS,
		0, 
		&_currentOnlineTaskHandle );

	if( FAILED( enumerateHR ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setError( enumerateHR );
		return REQUEST_FAILED;
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_CLAN_SEND_INVITATION );
		_requestState = REQUESTSTATE_CLAN_INVITE_SEND_ENUMERATE_MEMBERS;
		return REQUEST_SUCCEEDED;
	}
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestClanCancelSentInvitation( const OSIClanInvitation& invitation )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }
	return true;
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestClanInvitationList()
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );
	ASSERT( !_currentOnlineTaskHandle );

	_clanInvitationList.clear();
	
	ASSERT( _scratchClanInfoList.count() == 0 );
	_scratchClanInfoList.clear();

	// allocate some memory for messages
	XONLINE_MSG_SUMMARY* msgArray 
		= new XONLINE_MSG_SUMMARY[ XONLINE_MAX_NUM_MESSAGES ];
	ZeroMemory( 
		msgArray, 
		sizeof( XONLINE_MSG_SUMMARY ) * XONLINE_MAX_NUM_MESSAGES );
	DWORD msgCount = 0;

	// get all the current messages
	const HRESULT enumerateMsgHR = XOnlineMessageEnumerate(
		getActivePlayer()->state.currentControllerIndex,
		msgArray,
		&msgCount );
	if( FAILED( enumerateMsgHR ) )
	{
		delete [] msgArray;
		setLastRequestStatus( STATUS_FAILED );
		_setError( enumerateMsgHR );
		return REQUEST_FAILED;
	}

	// iterate through all the messages and find the ones that are clan 
	// invites for this titleid
	for( DWORD i = 0; (i < msgCount) && (_clanInvitationList.count() <= 20) ; i++ )
	{
		const XONLINE_MSG_SUMMARY& msg = msgArray[ i ];

		// ignore non-team-recruit messages
		if( XONLINE_MSG_TYPE_TEAM_RECRUIT != msg.bMsgType )
		{
			continue;
		}

		// ignore messages from other games
		if( s_ThisTitleID != msg.dwSenderTitleID )
		{
			continue;
		}


		// formulate an invite structure
		OSIClanInvitation invite;

		// don't re-insert the same clan ID
		invite.clanInfo.clanID.xBox.clanId = msg.qwMessageContext;
		if( !_scratchClanInfoList.findClanInfoByID( invite.clanInfo.clanID ) )
		{
			_scratchClanInfoList.push_back( invite.clanInfo );
		}

		invite.invitationID = msg.dwMessageID;
		XUID_to_OSIPlayerID( msg.xuidSender, invite.senderID );
		OSIstrncpy( 
			invite.senderName, 
			msg.szSenderName, 
			NUM_ARRAY_ELEMENTS( invite.senderName ) );
		OSIstrncpy( 
			invite.receiverName, 
			getActivePlayer()->name, 
			NUM_ARRAY_ELEMENTS( invite.receiverName ) );
		_clanInvitationList.push_back( invite );
	}
    
	delete [] msgArray;

	if( _scratchClanInfoList.count() == 0 )
	{
		setLastRequestStatus( STATUS_SUCCEEDED );
		_setCurrentRequestType( REQUESTTYPE_NONE );
		return REQUEST_SUCCEEDED;
	}

	ClansAdapter::OSICreateSingleton();
	ClansAdapter* clanIO = ClansAdapter::OSIGetSingleton();
	clanIO->BeginDownloadClanInfo( &_scratchClanInfoList );

	setLastRequestStatus( STATUS_WORKING );
	_setCurrentRequestType( REQUESTTYPE_CLAN_INVITATION_LIST );
	return REQUEST_SUCCEEDED;
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestClanIDListForPlayer( const OSIPlayerID& playerID )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );
	ASSERT( !_currentOnlineTaskHandle );

	if( !playerID.isValid() ) 
	{
		_setError( E_FAIL ); 
		setLastRequestStatus( STATUS_FAILED );
		return false;
	}

	OSIPlayerID_to_XUID( playerID, _requestPlayerXUID );

	_clanIDList.clear();

	const HRESULT enumerateHR = XOnlineTeamEnumerateByUserXUID(
		getActivePlayer()->state.currentControllerIndex,
		_requestPlayerXUID,
		NULL,
		&_currentOnlineTaskHandle );

	if( FAILED( enumerateHR ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setError( enumerateHR );
		return REQUEST_FAILED;
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_CLAN_ID_LIST_FOR_PLAYER );
		return REQUEST_SUCCEEDED;
	}
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestClanInfoList( const OSIClanID& clanID )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );
	ASSERT( !_currentOnlineTaskHandle );

	if( !clanID.isValid() )
	{
		_setError( E_FAIL ); 
		setLastRequestStatus( STATUS_FAILED );
		return false;
	}

	OSIClanID_to_XUID( clanID, _requestClanXUID );

	_clanInfoList.clear();

	const HRESULT enumerateHR = XOnlineTeamEnumerate( 
		getActivePlayer()->state.currentControllerIndex,
		1,
		&_requestClanXUID,
		NULL,
		&_currentOnlineTaskHandle );

	if( FAILED( enumerateHR ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setError( enumerateHR );
		return REQUEST_FAILED;
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_CLAN_INFO );
		return REQUEST_SUCCEEDED;
	}
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestClanMembershipList( const OSIClanID& clanID )
///------------------------------------------------------------------------------------------------
{ 
	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );
	ASSERT( !_currentOnlineTaskHandle );

	if( !clanID.isValid() )
	{
		_setError( E_FAIL ); 
		setLastRequestStatus( STATUS_FAILED );
		return false;
	}

	OSIClanID_to_XUID( clanID, _requestClanXUID );

	_clanMembershipList.clear();

	const HRESULT enumerateHR = XOnlineTeamMembersEnumerate( 
		getActivePlayer()->state.currentControllerIndex,
		_requestClanXUID,
		XONLINE_TEAM_SHOW_RECRUITS,
		NULL,
		&_currentOnlineTaskHandle );

	if( FAILED( enumerateHR ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setError( enumerateHR );
		return REQUEST_FAILED;
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_CLAN_MEMBERSHIP_LIST );
		_requestState = REQUESTSTATE_CLAN_MEMBERSHIP_GET_LIST;
		return REQUEST_SUCCEEDED;
	}
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestClanLeave( const OSIClanID& clanID )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	if( !clanID.isValid() )
	{
		_setError( E_FAIL ); 
		setLastRequestStatus( STATUS_FAILED );
		return false;
	}

	const bool result = requestClanKickMember( clanID, getActivePlayer()->id );
	if( result )
	{
		_setCurrentRequestType( REQUESTTYPE_CLAN_LEAVE );
	}
	return result;
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestClanNoticeList( const OSIClanID& clanID )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );
	ASSERT( !_currentOnlineTaskHandle );

	if( !clanID.isValid() )
	{
		_setError( E_FAIL ); 
		setLastRequestStatus( STATUS_FAILED );
		return false;
	}

	OSIClanID_to_XUID( clanID, _requestClanXUID );

	WCHAR serverPath[256];
	DWORD serverPathLen = NUM_ARRAY_ELEMENTS( serverPath );

	HRESULT hr = XOnlineStorageCreateServerPath(
		XONLINESTORAGE_FACILITY_TEAMS,
		0,
		_requestClanXUID.qwTeamID,
		L"notices",
		serverPath,
		&serverPathLen );
	if( SUCCEEDED( hr ) )
	{
		hr = XOnlineStorageDownloadToMemory(
			XONLINESTORAGE_FACILITY_TEAMS,
			0,
			serverPath,
			(BYTE*)_clanNoticeServerBuffer,
			sizeof( _clanNoticeServerBuffer ),
			0,
			NULL,
			&_currentOnlineTaskHandle );
		
		if( SUCCEEDED( hr ) )
		{
			_setCurrentRequestType( REQUESTTYPE_CLAN_NOTICE_LIST );
			setLastRequestStatus( STATUS_WORKING );
			return REQUEST_SUCCEEDED;
		}
	}

	// something failed
	_setError( hr );
	setLastRequestStatus( STATUS_FAILED );
	return REQUEST_FAILED;
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestClanPostNotice( const OSIClanID& clanID, const OSIchar* const noticeText )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );
	ASSERT( !_currentOnlineTaskHandle );
	ASSERT( noticeText );
//	ASSERT( strlen( notice ) < NUM_ARRAY_ELEMENTS( OSIClanNotice::noticeText ) );

	if( !clanID.isValid() || !noticeText || !noticeText[0] )
	{
		_setError( E_FAIL ); 
		setLastRequestStatus( STATUS_FAILED );
		return false;
	}

	OSIClanID_to_XUID( clanID, _requestClanXUID );
	OSIstrcpy( (wchar_t*)_clanNoticeServerBuffer, noticeText );
	
	LPCWSTR bufferPtr = (LPCWSTR) _clanNoticeServerBuffer;
	LPCWSTR* bufferPtrPtr = &bufferPtr;
	HRESULT hr = XOnlineStringVerify( 
		1,
		bufferPtrPtr,
		CXboxLive::GetFilteredLanguage(),
		NULL,
		&_currentOnlineTaskHandle );
	if( FAILED( hr ) )
	{
		// something failed
		_setError( hr );
		setLastRequestStatus( STATUS_FAILED );
		return REQUEST_FAILED;
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_CLAN_POST_NOTICE );
		_requestState = REQUESTSTATE_CLAN_POST_NOTICE_VERIFY_STRING;
		return REQUEST_SUCCEEDED;
	}
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestClanKickMember( const OSIClanID& clanID, const OSIPlayerID& playerID )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );
	ASSERT( !_currentOnlineTaskHandle );

	if( !clanID.isValid() || !playerID.isValid() )
	{
		_setError( E_FAIL ); 
		setLastRequestStatus( STATUS_FAILED );
		return false;
	}

	OSIClanID_to_XUID( clanID, _requestClanXUID );
	OSIPlayerID_to_XUID( playerID, _requestPlayerXUID );

	const HRESULT enumerateHR = XOnlineTeamMemberRemove( 
		getActivePlayer()->state.currentControllerIndex,
		_requestClanXUID,
		_requestPlayerXUID,
		NULL,
		&_currentOnlineTaskHandle );

	if( FAILED( enumerateHR ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setError( enumerateHR );
		return REQUEST_FAILED;
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_CLAN_KICK_MEMBER );
		return REQUEST_SUCCEEDED;
	}
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::_commonRequestClanPromoteDemoteMember( const OSIClanID& clanID, const OSIPlayerID& playerID )
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_NONE == _currentRequestType );
	ASSERT( !_currentOnlineTaskHandle );

	// first have to get the member properties, then submit the new properties
	OSIPlayerID_to_XUID( playerID, _requestPlayerXUID );
	OSIClanID_to_XUID( clanID, _requestClanXUID );
	
	const HRESULT enumerateHR = XOnlineTeamMembersEnumerate( 
		getActivePlayer()->state.currentControllerIndex,
		_requestClanXUID,
		0,
		NULL,
		&_currentOnlineTaskHandle );

	if( FAILED( enumerateHR ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setError( enumerateHR );
		return REQUEST_FAILED;
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		//_setCurrentRequestType( );// this should be set in the function that calls this function
		_requestState = REQUESTSTATE_CLAN_PROMOTE_DEMOTE_GET_MEMBERS;
		return REQUEST_SUCCEEDED;
	}
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestClanDemoteMember( const OSIClanID& clanID, const OSIPlayerID& playerID )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	if( !clanID.isValid() || !playerID.isValid() )
	{
		_setError( E_FAIL ); 
		setLastRequestStatus( STATUS_FAILED );
		return false;
	}

	const bool result = _commonRequestClanPromoteDemoteMember( clanID, playerID );
	if( result )
	{
		_setCurrentRequestType( REQUESTTYPE_CLAN_DEMOTE_MEMBER );
	}
	return result;
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestClanPromoteMember( const OSIClanID& clanID, const OSIPlayerID& playerID )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	if( !clanID.isValid() || !playerID.isValid() )
	{
		_setError( E_FAIL ); 
		setLastRequestStatus( STATUS_FAILED );
		return false;
	}

	const bool result = _commonRequestClanPromoteDemoteMember( clanID, playerID );
	if( result )
	{
		_setCurrentRequestType( REQUESTTYPE_CLAN_PROMOTE_MEMBER );
	}
	return result;
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestClanTransferLeadership( const OSIClanID& clanID, const OSIPlayerID& playerID )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );
	ASSERT( !_currentOnlineTaskHandle );

	if( !clanID.isValid() || !playerID.isValid() )
	{
		_setError( E_FAIL ); 
		setLastRequestStatus( STATUS_FAILED );
		return false;
	}

	// first have to get the member properties, then submit the new properties
	OSIPlayerID_to_XUID( playerID, _requestPlayerXUID );
	OSIClanID_to_XUID( clanID, _requestClanXUID );

	XONLINE_TEAM_MEMBER_PROPERTIES memberProps;
	memberProps.TeamMemberDataSize = 0;
	memberProps.dwPrivileges = s_ClanLeaderPrivileges;

	HRESULT hr = XOnlineTeamMemberSetProperties( 
		getActivePlayer()->state.currentControllerIndex,
		_requestClanXUID,
		_requestPlayerXUID,
		&memberProps,
		0, &_currentOnlineTaskHandle );
	if( FAILED( hr ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setError( hr );
		return REQUEST_FAILED;
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_CLAN_TRANSFER_LEADERSHIP );
		_requestState = REQUESTSTATE_CLAN_TRANSFER_PROMOTE_OTHER;
		return REQUEST_SUCCEEDED;
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanTransferLeadership()
///------------------------------------------------------------------------------------------------
{
	switch( _requestState )
	{
	case REQUESTSTATE_CLAN_TRANSFER_DEMOTE_SELF:	_processClanTransferDemoteSelf(); break;
	case REQUESTSTATE_CLAN_TRANSFER_PROMOTE_OTHER:	_processClanTransferPromoteOther(); break;
	default: ASSERT( 0 ); break;
	}
}


///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanTransferPromoteOther()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_TRANSFER_LEADERSHIP == _currentRequestType );
	ASSERT( REQUESTSTATE_CLAN_TRANSFER_PROMOTE_OTHER == _requestState );
	ASSERT( _currentOnlineTaskHandle );

	const XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	const OSIRequestStatus status = getLastRequestStatus();

	if( STATUS_WORKING == status )
	{
		return;
	}

	else if( STATUS_FAILED == status )
	{
		_requestState = REQUESTSTATE_NONE;
		XOnlineTaskClose( taskHandle );
		return;
	}

	// task succeeded
	XOnlineTaskClose( taskHandle );

	XUID myXUID;
	OSIPlayerID_to_XUID( getActivePlayer()->id, myXUID );

	XONLINE_TEAM_MEMBER_PROPERTIES memberProps;
	memberProps.TeamMemberDataSize = 0;
	memberProps.dwPrivileges = s_ClanOfficerPrivileges;

	HRESULT hr = XOnlineTeamMemberSetProperties( 
		getActivePlayer()->state.currentControllerIndex,
		_requestClanXUID,
		myXUID,
		&memberProps,
		0, &_currentOnlineTaskHandle );
	if( FAILED( hr ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setError( hr );
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_CLAN_TRANSFER_LEADERSHIP );
		_requestState = REQUESTSTATE_CLAN_TRANSFER_DEMOTE_SELF;
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanTransferDemoteSelf()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_TRANSFER_LEADERSHIP == _currentRequestType );
	ASSERT( REQUESTSTATE_CLAN_TRANSFER_DEMOTE_SELF == _requestState );
	XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	if( STATUS_WORKING != getLastRequestStatus() )
	{
		XOnlineTaskClose( taskHandle );
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanLeave()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_LEAVE == _currentRequestType );
	XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	
	switch( getLastRequestStatus() )
	{
	case STATUS_WORKING:
		return;

	case STATUS_SUCCEEDED:
		XOnlineTaskClose( taskHandle );
		return;

	case STATUS_FAILED:
		XOnlineTaskClose( taskHandle );

		if( (XONLINE_E_TEAMS_NO_ADMIN == _errorHR)
		&& (getActivePlayer()->clanInfo.memberCount <= 1) )
		{
			// Tried to remove self and leave no admin?  Just delete the team.
			// This means that the game code is reponsible for not letting you leave a team that has >1 person
			// without promoting someone to admin status first.

			// have to delete stats before deleting the team

			XONLINE_STAT stats[2];
			StatsAdapter::ConvertClanOsiStatToXbox( stats[0], OSIClanStats::CLANSTAT_RATING, -10000 );
			StatsAdapter::ConvertClanOsiStatToXbox( stats[1], OSIClanStats::CLANSTAT_KILLS, 0 );

			XONLINE_STAT_PROC procs[2];
			procs[0].wProcedureID = XONLINE_STAT_PROCID_CONDITIONAL;
			procs[0].Conditional.xuid = _requestClanXUID;
			procs[0].Conditional.dwLeaderBoardID = StatsAdapter::s_LeaderboardIdForClanStats0to63;
			procs[0].Conditional.bComparisonType = XONLINE_STAT_COMPTYPE_EXISTS;
			procs[0].Conditional.StatToCompare.wID = StatsAdapter::ConvertClanOsiStatIDToXbox( OSIClanStats::CLANSTAT_RATING );
			procs[0].Conditional.StatToCompare.type = XONLINE_STAT_LONGLONG;
			procs[0].Conditional.StatToCompare.llValue = 1;

			StatsAdapter::FillUpdate( 
				procs[1].Update, 
				_requestClanXUID, 
				StatsAdapter::s_LeaderboardIdForClanStats0to63, 
				NUM_ARRAY_ELEMENTS( stats ), 
				stats );
			procs[1].wProcedureID = XONLINE_STAT_PROCID_UPDATE_REPLACE;
			procs[1].Update.dwConditionalIndex = 1; // index is 1-based, actually means proc[0]

			const HRESULT statHR = XOnlineStatWriteEx( 2, procs, 0, &_currentOnlineTaskHandle );

			if( FAILED( statHR ) )
			{
				setLastRequestStatus( STATUS_FAILED );
				_setError( statHR );
			}
			else
			{
				setLastRequestStatus( STATUS_WORKING );
				_setCurrentRequestType( REQUESTTYPE_CLAN_DELETE );
				_requestState = REQUESTSTATE_CLAN_DELETE_STATS;
			}
		}
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanDelete()
///------------------------------------------------------------------------------------------------
{
	switch( _requestState )
	{
	case REQUESTSTATE_CLAN_DELETE_CLAN:		_processClanDeleteClan(); break;
	case REQUESTSTATE_CLAN_DELETE_STATS:	_processClanDeleteStats(); break;
	default: ASSERT( 0 );
	};
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanDeleteClan()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_DELETE == _currentRequestType );
	ASSERT( REQUESTSTATE_CLAN_DELETE_CLAN == _requestState );
	XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	if( STATUS_WORKING != getLastRequestStatus() )
	{
		XOnlineTaskClose( taskHandle );
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanDeleteStats()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_DELETE == _currentRequestType );
	ASSERT( REQUESTSTATE_CLAN_DELETE_STATS == _requestState );
	XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	if( STATUS_WORKING != getLastRequestStatus() )
	{
		XOnlineTaskClose( taskHandle );

		const HRESULT deleteHR = XOnlineTeamDelete( 
			getActivePlayer()->state.currentControllerIndex,
			_requestClanXUID,
			NULL,
			&_currentOnlineTaskHandle );
		if( FAILED( deleteHR ) )
		{
			setLastRequestStatus( STATUS_FAILED );
			_setError( deleteHR );
		}
		else
		{
			setLastRequestStatus( STATUS_WORKING );
			_setCurrentRequestType( REQUESTTYPE_CLAN_DELETE );
			_requestState = REQUESTSTATE_CLAN_DELETE_CLAN;
		}
	}
}


///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanNoticeList()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_NOTICE_LIST == _currentRequestType );
	
	const XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	const OSIRequestStatus status = getLastRequestStatus();

	if( STATUS_WORKING == status )
	{
		return;
	}

	else if( STATUS_FAILED == status )
	{
		XOnlineTaskClose( taskHandle );
		if( XONLINE_E_STORAGE_FILE_NOT_FOUND == _errorHR )
		{
			_clanNoticeList.clear();
			setLastRequestStatus( STATUS_SUCCEEDED );
			_setError( S_OK );
		}
		return;
	}

	// task succeeded

	DWORD numBytesOnServer;

	const HRESULT resultsHR = XOnlineStorageDownloadToMemoryGetResults(
		taskHandle,
		NULL, 
		&_clanNoticeServerBufferBytesUsed,
		&numBytesOnServer,
		NULL,
		NULL );
	XOnlineTaskClose( taskHandle );
	if( FAILED( resultsHR ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setError( resultsHR );
		return;
	}

	ASSERT( _clanNoticeServerBufferBytesUsed == numBytesOnServer );

	_convertClanNoticeServerChunkToList();
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanPostNotice()
///------------------------------------------------------------------------------------------------
{
	switch( _requestState )
	{
	case REQUESTSTATE_CLAN_POST_NOTICE_UPLOAD:			_processClanPostNoticeUpload(); break;
	case REQUESTSTATE_CLAN_POST_NOTICE_VERIFY_STRING:	_processClanPostNoticeVerify(); break;
	default: ASSERT( 0 );
	};
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanPostNoticeVerify()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_POST_NOTICE == _currentRequestType );
	ASSERT( REQUESTSTATE_CLAN_POST_NOTICE_VERIFY_STRING == _requestState );

	XONLINETASK_HANDLE taskHandle = _currentOnlineTaskHandle;
	OSIRequestStatus status;

	const HRESULT continueHR = XOnlineTaskContinue( _currentOnlineTaskHandle );

	if( FAILED( continueHR ) || (S_FALSE == continueHR) )
	{
		status = STATUS_FAILED;
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_setError( continueHR );
		_currentOnlineTaskHandle = 0;
		_requestState = REQUESTSTATE_NONE;
	}
	else if( (XONLINETASK_S_SUCCESS == continueHR) ) //|| (S_OK == continueHR) )
	{
		status = STATUS_SUCCEEDED;
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_currentOnlineTaskHandle = 0;
		_requestState = REQUESTSTATE_NONE;
	}
	else
	{
		status = STATUS_WORKING;
	}

	setLastRequestStatus( status );

	if( STATUS_WORKING == status )
	{
		return;
	}

	// see if the string was rejected
	HRESULT verifyHR = E_FAIL;
	HRESULT hr = XOnlineStringVerifyGetResults(
		taskHandle,
		1,
		&verifyHR );
	XOnlineTaskClose( taskHandle );

	if( FAILED( verifyHR ) )
	{
		// string verify failed
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_requestState = REQUESTSTATE_NONE;
		setLastRequestStatus( STATUS_FAILED );
		_setError( verifyHR );
		return;
	}
	if( FAILED( hr ) )
	{
		// call to GetResults get results failed
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_requestState = REQUESTSTATE_NONE;
		setLastRequestStatus( STATUS_FAILED );
		_setError( hr );
		return;
	}

	// task succeeded

    // if a notice has to be deleted, remove the oldest (which is always the last in the list)
	if( _clanNoticeList.count() == s_MaxClanNotices )
	{
		_clanNoticeList.end();
		_clanNoticeList.erase();
	}

	// build the notice
	OSIClanNotice notice;
	ZeroMemory( &notice, sizeof( notice ) );
	OSIstrncpy( notice.posterName, getActivePlayer()->name, NUM_ARRAY_ELEMENTS( notice.posterName ) );
	OSIstrncpy( notice.noticeText, (WCHAR*)_clanNoticeServerBuffer, NUM_ARRAY_ELEMENTS( notice.noticeText ) );

	// put it at the front of the list
	_clanNoticeList.push_front( notice );

	// start the upload
	WCHAR serverPath[256];
	DWORD serverPathLen;

	hr = XOnlineStorageCreateServerPath(
		XONLINESTORAGE_FACILITY_TEAMS,
		0,
		_requestClanXUID.qwTeamID,
		L"notices",
		serverPath,
		&serverPathLen );
	if( SUCCEEDED( hr ) )
	{
		_convertClanNoticeListToServerChunk();
		
		FILETIME expire;
		expire.dwHighDateTime = 0;
		expire.dwLowDateTime = 0;
		
		hr = XOnlineStorageUploadFromMemory(
			XONLINESTORAGE_FACILITY_TEAMS,
			0,
			serverPath,
			expire,
			(BYTE*)_clanNoticeServerBuffer,
			_clanNoticeServerBufferBytesUsed,
			NULL,
			NULL,
			&_currentOnlineTaskHandle );
		
		if( SUCCEEDED( hr ) )
		{
			_setCurrentRequestType( REQUESTTYPE_CLAN_POST_NOTICE );
			_requestState = REQUESTSTATE_CLAN_POST_NOTICE_UPLOAD;
			setLastRequestStatus( STATUS_WORKING );
		}
	}

	if( FAILED( hr ) )
	{
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_requestState = REQUESTSTATE_NONE;
		setLastRequestStatus( STATUS_FAILED );
		_setError( hr );
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanPostNoticeUpload()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_POST_NOTICE == _currentRequestType );
	ASSERT( REQUESTSTATE_CLAN_POST_NOTICE_UPLOAD == _requestState );

	XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	if( STATUS_WORKING != getLastRequestStatus() )
	{
		XOnlineTaskClose( taskHandle );
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanKickMember()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_KICK_MEMBER == _currentRequestType );
	XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	if( STATUS_WORKING != getLastRequestStatus() )
	{
		XOnlineTaskClose( taskHandle );
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_commonProcessClanPromoteDemoteMember()
///------------------------------------------------------------------------------------------------
{
	switch( _requestState )
	{
	case REQUESTSTATE_CLAN_PROMOTE_DEMOTE_GET_MEMBERS:				_processClanPromoteDemoteGetMembers(); break;
	case REQUESTSTATE_CLAN_PROMOTE_DEMOTE_UPDATE_MEMBER_PRIVILEGES:	_processClanPromoteDemoteUpdateMemberPrivileges(); break;
	default: ASSERT( 0 );
	};
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanDemoteMember()
///------------------------------------------------------------------------------------------------
{
	_commonProcessClanPromoteDemoteMember();
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanPromoteMember()
///------------------------------------------------------------------------------------------------
{
	_commonProcessClanPromoteDemoteMember();
}


///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanPromoteDemoteGetMembers()
///------------------------------------------------------------------------------------------------
{
	ASSERT( (REQUESTTYPE_CLAN_PROMOTE_MEMBER == _currentRequestType) || (REQUESTTYPE_CLAN_DEMOTE_MEMBER == _currentRequestType ) );
	ASSERT( REQUESTSTATE_CLAN_PROMOTE_DEMOTE_GET_MEMBERS == _requestState );

	const XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	const OSIRequestStatus status = getLastRequestStatus();

	if( STATUS_WORKING == status )
	{
		return;
	}

	else if( STATUS_FAILED == status )
	{
		_requestState = REQUESTSTATE_NONE;
		XOnlineTaskClose( taskHandle );
		return;
	}

	// task succeeded

	// note: _commonXOnlineTaskProcessing will have reset _currentRequestType already, so use _lastRequestType to find
	// out what kind of request is being made (promote or demote).  It also clears _requestState.

	DWORD numTeamMembers;
	XUID ids[ XONLINE_MAX_TEAM_MEMBER_COUNT ];    

	// parse the results immediately
	HRESULT hresult = XOnlineTeamMembersEnumerateGetResults(
		taskHandle,
		&numTeamMembers,
		ids );

	if( SUCCEEDED( hresult ) )
	{
		// now find get the member info for the player being demoted/promoted
		XONLINE_TEAM_MEMBER memberInfo;
		hresult = XOnlineTeamMemberGetDetails(
			taskHandle,
			_requestPlayerXUID,
			&memberInfo );
			
		if( SUCCEEDED( hresult ) )
		{
			// go ahead and close this task since we're done with it
			XOnlineTaskClose( taskHandle );

			// get the privileges out of the results
			OSIClanRole currentRole = XboxPrivileges_to_OSIClanRole( memberInfo.TeamMemberProperties.dwPrivileges );
			
			// do the proper promote/demote calculation
			OSIClanRole newRole;
			if( REQUESTTYPE_CLAN_PROMOTE_MEMBER == _lastRequestType )	{ newRole = OSIClanRolePromote( currentRole ); }
			else /*DEMOTE by default*/									{ newRole = OSIClanRoleDemote( currentRole ); }

			// begin the data update on Live
			memberInfo.TeamMemberProperties.dwPrivileges = OSIClanRole_to_XboxPrivileges( newRole );

			const HRESULT setPropsHR = XOnlineTeamMemberSetProperties(
				getActivePlayer()->state.currentControllerIndex,
				_requestClanXUID,
				_requestPlayerXUID,
				&memberInfo.TeamMemberProperties,
				NULL,
				&_currentOnlineTaskHandle );

			if( SUCCEEDED( setPropsHR ) )
			{
				_setCurrentRequestType( _lastRequestType ); // continue the same request type
				setLastRequestStatus( STATUS_WORKING );
				_requestState = REQUESTSTATE_CLAN_PROMOTE_DEMOTE_UPDATE_MEMBER_PRIVILEGES;
				return;
			}
		}
	}
    
	// only gets here on failure
	setLastRequestStatus( STATUS_FAILED );
	_setError( hresult );
	XOnlineTaskClose( taskHandle );
}


///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanPromoteDemoteUpdateMemberPrivileges()
///------------------------------------------------------------------------------------------------
{
	ASSERT( (REQUESTTYPE_CLAN_PROMOTE_MEMBER == _currentRequestType) || (REQUESTTYPE_CLAN_DEMOTE_MEMBER == _currentRequestType ) );
	ASSERT( REQUESTSTATE_CLAN_PROMOTE_DEMOTE_UPDATE_MEMBER_PRIVILEGES == _requestState );

	XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	if( STATUS_WORKING != getLastRequestStatus() )
	{
		XOnlineTaskClose( taskHandle );
	}
}

///------------------------------------------------------------------------------------------------
//! Takes the data in _clanNoticeServerBuffer and uses it to populate _clanNoticeList.
//! The format of the _clanNoticeServerBuffer is:
//!		char: number of notices
//!		(and for each notice)
//!			unsigned short: length of notice text
//!			char[..]: notice text
//!			char[15]: gamertag of poster
///------------------------------------------------------------------------------------------------
void OSIXbox::_convertClanNoticeServerChunkToList()
///------------------------------------------------------------------------------------------------
{
	OSIClanNoticeList& list = _clanNoticeList;
	char* dataCursor = _clanNoticeServerBuffer;

	list.clear();
	int count = *((unsigned char*)dataCursor);
	++dataCursor;
	ASSERT( dataCursor <= _clanNoticeServerBuffer + _clanNoticeServerBufferBytesUsed );

	for( int i = 0; i < count; i++ )
	{
		OSIClanNotice notice;
		ZeroMemory( &notice, sizeof( notice ) );

		unsigned short textLen = *((unsigned short*)dataCursor);
		dataCursor += sizeof( textLen );
		ASSERT( dataCursor < _clanNoticeServerBuffer + _clanNoticeServerBufferBytesUsed );

		memcpy( notice.noticeText, dataCursor, textLen );
		dataCursor += textLen;
		ASSERT( dataCursor < _clanNoticeServerBuffer + _clanNoticeServerBufferBytesUsed );

		memcpy( notice.posterName, dataCursor, XONLINE_MAX_GAMERTAG_LENGTH );
		dataCursor += XONLINE_MAX_GAMERTAG_LENGTH;
		ASSERT( dataCursor <= _clanNoticeServerBuffer + _clanNoticeServerBufferBytesUsed );

		list.push_back( notice );
	}

	ASSERT( dataCursor == _clanNoticeServerBuffer + _clanNoticeServerBufferBytesUsed );
}

///------------------------------------------------------------------------------------------------
//! Takes the _clanNoticeList and uses it to create the data in _clanNoticeServerBuffer.
//! The format of the _clanNoticeServerBuffer is:
//!		char: number of notices
//!		(and for each notice)
//!			unsigned short: length of notice text
//!			char[..]: notice text
//!			char[15]: gamertag of poster
///------------------------------------------------------------------------------------------------
void OSIXbox::_convertClanNoticeListToServerChunk()
///------------------------------------------------------------------------------------------------
{
	OSIClanNoticeList& list = _clanNoticeList;
	char* dataCursor = _clanNoticeServerBuffer;
	
	*((unsigned char*)dataCursor) = (unsigned char) list.count();
	++dataCursor;

	list.begin();
	for( unsigned int i = 0; i < list.count(); i++ )
	{
		OSIClanNotice* notice = list.getCurrent();
		unsigned short textLen = strlen( notice->noticeText );

		*((unsigned short*)dataCursor) = textLen;
		dataCursor += sizeof( textLen );

		memcpy( dataCursor, notice->noticeText, textLen );
		dataCursor += textLen;

		memcpy( dataCursor, notice->posterName, XONLINE_MAX_GAMERTAG_LENGTH );
		dataCursor += XONLINE_MAX_GAMERTAG_LENGTH;

		list.next();
	}

	_clanNoticeServerBufferBytesUsed = (int)(dataCursor - _clanNoticeServerBuffer);
}



///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanCreate()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_CREATE == _currentRequestType );
	XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	if( STATUS_WORKING != getLastRequestStatus() )
	{
		XOnlineTaskClose( taskHandle );
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanJoinByInvitation()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_JOIN_BY_INVITATION == _currentRequestType );
	XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	if( STATUS_WORKING != getLastRequestStatus() )
	{
		XOnlineTaskClose( taskHandle );
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanDenyInvitation()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_DENY_INVITATION == _currentRequestType );
	XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	if( STATUS_WORKING != getLastRequestStatus() )
	{
		XOnlineTaskClose( taskHandle );
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanBlockInvitation()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_BLOCK_INVITATION == _currentRequestType );
	XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	if( STATUS_WORKING != getLastRequestStatus() )
	{
		XOnlineTaskClose( taskHandle );
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanSendInvitation()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_SEND_INVITATION == _currentRequestType );
	switch( _requestState )	
	{
	case REQUESTSTATE_CLAN_INVITE_SEND_ENUMERATE_MEMBERS:	_processClanSendInvitationEnumerateMembers(); break;
	case REQUESTSTATE_CLAN_INVITE_SEND:						_processClanSendInvitationSend(); break;
	default: ASSERT( 0 );
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanSendInvitationEnumerateMembers()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_SEND_INVITATION == _currentRequestType );
	ASSERT( REQUESTSTATE_CLAN_INVITE_SEND_ENUMERATE_MEMBERS == _requestState );

	const XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	const OSIRequestStatus status = getLastRequestStatus();

	if( STATUS_WORKING == status )
	{
		return;
	}

	else if( STATUS_FAILED == status )
	{
		_requestState = REQUESTSTATE_NONE;
		XOnlineTaskClose( taskHandle );
		return;
	}

	// task succeeded
	DWORD memberCnt = XONLINE_MAX_TEAM_MEMBER_COUNT;
	XUID memberIds[ XONLINE_MAX_TEAM_MEMBER_COUNT ];
	HRESULT hr = XOnlineTeamMembersEnumerateGetResults(
		taskHandle,
		&memberCnt,
		memberIds );

	_requestState = REQUESTSTATE_NONE;
	
	if( FAILED( hr ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		XOnlineTaskClose( taskHandle );
		return;
	}
	
	// look for the invite recipient's name in the list of recruits and members to make sure that
	// the invitation isn't redundant.

	for( DWORD i = 0; i < memberCnt; ++i )
	{
		XONLINE_TEAM_MEMBER member;
		hr = XOnlineTeamMemberGetDetails( taskHandle, memberIds[i], &member );
		if( FAILED( hr ) )
		{
			setLastRequestStatus( STATUS_FAILED );
			XOnlineTaskClose( taskHandle );
			return;
		}

		if( !OSIstrcmp( member.szGamertag, _requestPlayerName ) ) 
		{
			// found the invited player is already in the clan, emulate that error
			setLastRequestStatus( STATUS_FAILED );
			_setError( XONLINE_E_TEAMS_USER_ALREADY_EXISTS );
			XOnlineTaskClose( taskHandle );
			return;
		}
	}

	XOnlineTaskClose( taskHandle );

	// user is not already in the team and has no pending invite to the team, so 
	// go ahead and send the invitation.

	XONLINE_TEAM_MEMBER_PROPERTIES memberProps;
	memberProps.dwPrivileges = s_ClanSoldierPrivileges;
	memberProps.TeamMemberDataSize = 0;

	const HRESULT inviteHR = XOnlineTeamMemberRecruitByName(
		getActivePlayer()->state.currentControllerIndex,
		_requestClanXUID,
		_requestPlayerName,
		&memberProps,
		NULL, // default message
		NULL,
		&_currentOnlineTaskHandle );

	if( FAILED( inviteHR ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setError( inviteHR );
		return;
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_CLAN_SEND_INVITATION );
		_requestState = REQUESTSTATE_CLAN_INVITE_SEND;
		return;
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanSendInvitationSend()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_SEND_INVITATION == _currentRequestType );
	XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	if( STATUS_WORKING != getLastRequestStatus() )
	{
		XOnlineTaskClose( taskHandle );
	}
}

/*
///------------------------------------------------------------------------------------------------
bool OSIXbox::_processClanInvitationListTeamDetailsBatch( XONLINETASK_HANDLE taskHandle )
///------------------------------------------------------------------------------------------------
{
	// Copy results from the last batch of XUID submitted via XOnlineTeamEnumerate
	// into the clan invitation list; have to call enumerateGetResults for GetDetails to work.
	XUID teamXUIDBatch[ XONLINE_MAX_TEAM_COUNT ];
	DWORD teamXUIDBatchCount = 0;
	
	const HRESULT getResultsHR = XOnlineTeamEnumerateGetResults( 
		taskHandle,
		&teamXUIDBatchCount,
		teamXUIDBatch );

	if( FAILED( getResultsHR ) )
	{
		return false;
	}

	for( DWORD teamXUIDScan = 0; teamXUIDScan < teamXUIDBatchCount; teamXUIDScan++ )
	{
		XONLINE_TEAM teamInfo;
		const HRESULT getDetailsHR = XOnlineTeamGetDetails(
			taskHandle,
			teamXUIDBatch[ teamXUIDScan ],
			&teamInfo );

		if( FAILED( getDetailsHR ) )
		{
			return false;
		}
		
		// find the message that matches the team info
		XONLINE_MSG_SUMMARY* msg = NULL;
		for( DWORD msgScan = _xonlineMessageSummaryArrayIndex; 
			msgScan < (_xonlineMessageSummaryArrayIndex + teamXUIDBatchCount); 
			msgScan++ )
		{
			if( teamInfo.xuidTeam.qwTeamID == _xonlineMessageSummaryArray[ msgScan ].qwMessageContext )
			{
				msg = &_xonlineMessageSummaryArray[ msgScan ];
				break;
			}
		}

		if( NULL == msg )
		{
			// this would scare me - it means that XOnlineTeamGetDetails returned info for a team that 
			// we didn't ask for.
			return false;
		}

		OSIClanInvitation invite;
		XUID_to_OSIPlayerID( msg->xuidSender, invite.senderID );

		OSIstrncpy( invite.senderName, msg->szSenderName, NUM_ARRAY_ELEMENTS(invite.senderName) );
		invite.invitationID = msg->dwMessageID;
		invite.clanInfo.memberCount = teamInfo.dwMemberCount;
		OSIstrncpy( invite.clanInfo.name, teamInfo.TeamProperties.wszDescription, NUM_ARRAY_ELEMENTS(invite.clanInfo.name) );
		OSIstrncpy( invite.clanInfo.tag, teamInfo.TeamProperties.wszTeamName, NUM_ARRAY_ELEMENTS(invite.clanInfo.tag) );
		invite.clanInfo.clanID.xBox.clanId = msg->qwMessageContext;
		invite.clanInfo.clanID.xBox.userFlags = 0;

		_clanInvitationList.push_back( invite );
	}

	XOnlineTaskClose( taskHandle );

	return true;
}
*/

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanInvitationList()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_INVITATION_LIST == _currentRequestType );

	// Process clan info downloads.
	ClansAdapter* clanIO = ClansAdapter::OSIGetSingleton();
	if( !clanIO->IsDownloadClanInfoComplete() )
	{
		clanIO->ContinueDownloadClanInfo();
		return;
	}
	clanIO->FinishDownloadClanInfo();
	ClansAdapter::OSIDestroySingleton();

	// Iterate through the invite list and find claninfo for each one.
	OSIClanInvitation* invite;
	for( _clanInvitationList.begin()
		; invite = _clanInvitationList.getCurrent()
		; _clanInvitationList.next() )
	{
		OSIClanInfo* clanInfo
			= _scratchClanInfoList.findClanInfoByID( invite->clanInfo.clanID );
		if( !clanInfo || clanInfo->memberCount == 0 )
		{
			// delete invitations to clans that don't exist anymore
			_clanInvitationList.erase();
			_clanInvitationList.begin();
		}
		else
		{
			invite->clanInfo = *clanInfo;
		}
	}

	_scratchClanInfoList.clear();

	setLastRequestStatus( STATUS_SUCCEEDED );
	_setCurrentRequestType( REQUESTTYPE_NONE );
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanIdListForPlayer()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_ID_LIST_FOR_PLAYER == _currentRequestType );
	
	const XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	const OSIRequestStatus status = getLastRequestStatus();

	if( STATUS_WORKING == status )
	{
		return;
	}

	else if( STATUS_FAILED == status )
	{
		XOnlineTaskClose( taskHandle );
		return;
	}

	// task succeeded
	DWORD count;
	XUID xuids[ XONLINE_MAX_TEAM_COUNT ];    

	// parse the results immediately
	const HRESULT getHR = XOnlineTeamEnumerateGetResults(
		taskHandle, 
		&count,
		xuids );

	if( FAILED( getHR ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setError( getHR );
	}
	else 
	{
		_clanIDList.clear();
		for( DWORD i = 0; i < count; i++ )
		{
			OSIClanID clanID;
			XUID_to_OSIClanID( xuids[ i ], clanID );
			_clanIDList.push_back( clanID );
		}
	}

	XOnlineTaskClose( taskHandle );
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanInfo()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_INFO == _currentRequestType );
	
	const XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	const OSIRequestStatus status = getLastRequestStatus();

	if( STATUS_WORKING == status )
	{
		return;
	}

	else if( STATUS_FAILED == status )
	{
		XOnlineTaskClose( taskHandle );
		return;
	}

	// task succeeded

	DWORD teamCount;
	XUID xuids[ XONLINE_MAX_TEAM_COUNT ];

	// have to call EnumerateGetResults first before getdetails will work
	HRESULT hresult = XOnlineTeamEnumerateGetResults(
		taskHandle, 
		&teamCount, 
		xuids );

	if( SUCCEEDED( hresult ) )
	{
		XONLINE_TEAM teamInfo;
		hresult = XOnlineTeamGetDetails(
			taskHandle, 
			_requestClanXUID,
			&teamInfo );

		if( SUCCEEDED( hresult ) )
		{
			OSIClanInfo clanInfo;
			XUID_to_OSIClanID( teamInfo.xuidTeam, clanInfo.clanID );
			clanInfo.memberCount = teamInfo.dwMemberCount;
			OSIstrncpy( clanInfo.name, teamInfo.TeamProperties.wszDescription, NUM_ARRAY_ELEMENTS( clanInfo.name ) );
			OSIstrncpy( clanInfo.tag, teamInfo.TeamProperties.wszTeamName, NUM_ARRAY_ELEMENTS( clanInfo.tag ) );

			_clanInfoList.clear();
			_clanInfoList.push_back( clanInfo );
			
			XOnlineTaskClose( taskHandle );
			return;
		}
	}

	// only gets here on failure
	setLastRequestStatus( STATUS_FAILED );
	_setError( hresult );
	XOnlineTaskClose( taskHandle );
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanMembershipList()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_MEMBERSHIP_LIST == _currentRequestType );
	switch( _requestState )	
	{
	case REQUESTSTATE_CLAN_MEMBERSHIP_GET_LIST:			_processClanMembershipGetList(); break;
	default: ASSERT( 0 );
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processClanMembershipGetList()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_CLAN_MEMBERSHIP_LIST == _currentRequestType );
	ASSERT( REQUESTSTATE_CLAN_MEMBERSHIP_GET_LIST == _requestState );
	
	const XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	const OSIRequestStatus status = getLastRequestStatus();

	if( STATUS_WORKING == status )
	{
		return;
	}

	else if( STATUS_FAILED == status )
	{
		XOnlineTaskClose( taskHandle );
		return;
	}

	// task succeeded

	DWORD numTeamMembers;
	XUID ids[ XONLINE_MAX_TEAM_MEMBER_COUNT ];    

	// parse the results immediately
	const HRESULT getHR = XOnlineTeamMembersEnumerateGetResults(
		taskHandle,
		&numTeamMembers,
		ids );
	if( FAILED( getHR ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setError( getHR );
		XOnlineTaskClose( taskHandle );
		return;
	}

	_clanSentInvitationList.clear();
	_clanMembershipList.clear();
	for( DWORD i = 0; i < numTeamMembers; i++ )
	{
		OSIClanMembership member;
		XUID_to_OSIClanID( _requestClanXUID, member.clanID );
		XUID_to_OSIPlayerID( ids[ i ], member.playerID );

		XONLINE_TEAM_MEMBER memberDetails;
		const HRESULT getDetailsHR = XOnlineTeamMemberGetDetails(
			taskHandle, ids[ i ], &memberDetails );
		if( SUCCEEDED( getDetailsHR ) )
		{
			if( XONLINE_TEAM_MEMBER_RECRUITED == (memberDetails.dwFlags & XONLINE_TEAM_MEMBER_RECRUITED) )
			{
				OSIClanInvitation invite;
				XUID_to_OSIPlayerID( memberDetails.xuidTeamMember, invite.receiverID );
				invite.clanInfo = getActivePlayer()->clanInfo;
				invite.invitationID = 0;
				OSIstrncpy( invite.receiverName, memberDetails.szGamertag, NUM_ARRAY_ELEMENTS( invite.receiverName ) );
				invite.senderID = getActivePlayer()->id;
				OSIstrncpy( invite.senderName, getActivePlayer()->name, NUM_ARRAY_ELEMENTS( invite.senderName ) );
				_clanSentInvitationList.push_back( invite );
			}
			else
			{
				OSIstrncpy( member.memberName, memberDetails.szGamertag, NUM_ARRAY_ELEMENTS( member.memberName ) );
				member.role = XboxPrivileges_to_OSIClanRole( memberDetails.TeamMemberProperties.dwPrivileges );
				_clanMembershipList.push_back( member );
			}
		}
	}

	// close the member enum task 
	XOnlineTaskClose( taskHandle );
	setLastRequestStatus( STATUS_SUCCEEDED );
	_setCurrentRequestType( REQUESTTYPE_NONE );
	_requestState = REQUESTSTATE_NONE;
}

///------------------------------------------------------------------------------------------------
void OSIXbox::beginMatchSearchesForClanMembershipList()
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return; }

	// don't do anything if there are no clan members
	if( _clanMembershipList.count() == 0 )
	{
		return;
	}

	ASSERT( !_playerPresenceHelper );
	ASSERT( !_matchSearchHelper );

	_playerPresenceHelper = new PlayerPresenceHelper;
	_matchSearchHelper = new MatchSearchHelper;

	OSIClanMembership* member;
	for( _clanMembershipList.begin();
		member = _clanMembershipList.getCurrent();
		_clanMembershipList.next() )
	{
		_playerPresenceHelper->AddPlayer( member->playerID );
		member->currentMatchList->clear();
	}
}

///------------------------------------------------------------------------------------------------
int OSIXbox::updateMatchSearchesForClanMembershipList()
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return 0; }

	ASSERT( _playerPresenceHelper );
	ASSERT( _matchSearchHelper );

	int numUpdated = 0;

	// Update the presence helper.
	_playerPresenceHelper->Update();

	// Iterate through the clan member list and see if presence info is new for any clan members.
	OSIClanMembership* member;
	for( _clanMembershipList.begin();
		member = _clanMembershipList.getCurrent();
		_clanMembershipList.next() )
	{
		// Don't need to do any work if this player's presence info has already been processed.
		if( !_playerPresenceHelper->IsPlayerInfoNew( member->playerID ) )
		{
			continue; // next member
		}

		// Get the new info for this player.
		OSIMatchID matchID;
		bool isOnline;
		bool isPlaying;
		bool isJoinable;
		_playerPresenceHelper->GetPlayerInfo( 
			member->playerID,
			isOnline,
			isPlaying,
			isJoinable,
			matchID );
		
		++numUpdated;
		_playerPresenceHelper->SetIsPlayerInfoNew( member->playerID, false );
		member->currentMatchList->clear();

		// Copy relevant info into the member.
		member->isOnline = isOnline;

		if( !member->currentMatchList ) 
		{ 
			ASSERT( member->currentMatchList ); 
			continue; // next member
		}

		if( !isOnline || !isPlaying || !isJoinable )
		{
			// They can't have a valid match.
			continue; // next member
		}

		OSIMatch match;
		match.id = matchID;
		OSIstrncpy( match.options.description, "Searching...", NUM_ARRAY_ELEMENTS( match.options.description ) );
		member->currentMatchList->push_back( match );

		_matchSearchHelper->CancelSearch( matchID );
		_matchSearchHelper->AddSearch( matchID );
	}

	// Update the search helper.
	_matchSearchHelper->Update();

	// Iterate through the clan member list and see if any match searches have finished.
	for( _clanMembershipList.begin();
		member = _clanMembershipList.getCurrent();
		_clanMembershipList.next() )
	{
		if( !member->currentMatchList ) 
		{ 
			ASSERT( member->currentMatchList ); 
			continue; // next member
		}

		if( member->currentMatchList->count() != 1 )
		{
			continue; // next member
		}
        
		OSIMatch* match = (*member->currentMatchList)[0];
		if( !_matchSearchHelper->IsAlreadySearchingFor( match->id ) )
		{
			// This should only happen after the search has completed.
			continue; // next member
		}

		bool isError;
		bool found = _matchSearchHelper->GetSearchResults( match->id, *match, isError );
		if( found && !isError )
		{
			++numUpdated;
		}
		else
		{
			if( isError )
			{
				member->currentMatchList->clear();
			}
		}
	}

	return numUpdated;
}

///------------------------------------------------------------------------------------------------
void OSIXbox::endMatchSearchesForClanMembershipList()
///------------------------------------------------------------------------------------------------
{
	// delete is supposed to work OK for NULL arguments, but I don't trust the AV memory manager.
	if( _matchSearchHelper ) { delete _matchSearchHelper; }
	if( _playerPresenceHelper) { delete _playerPresenceHelper; }
	_matchSearchHelper = 0;
	_playerPresenceHelper = 0;
}





