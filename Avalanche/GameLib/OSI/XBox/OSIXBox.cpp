
#include "pch.hpp"
#include "OSIXbox.hpp"
#include "GameHelper/ErrorStack.h"
#include "OSIXboxStatsUtility.hpp"
#include "OSIXboxClansUtility.hpp"

#include "platform/systemi.h"
#include "network/session.h"
#include "voice/netz/xbox/XHVVoiceManager.h"
#include "voice/netz/cvoicechat.h"
#include "platform/xbox/xboxlive.h"
#include "gamehelper/translator.h"
#include "hud/stringex.h"

#ifdef ENABLE_OSI_XBOX_LOG
/*static*/ FILE* OSIXbox::_logFile = 0;
#endif // ENABLE_OSI_XBOX_LOG

///------------------------------------------------------------------------------------------------
OSIXbox::OSIXbox()
///------------------------------------------------------------------------------------------------
:	_errorHR							( S_OK ),
	_currentRequestType					( REQUESTTYPE_NONE ),
	_lastRequestType					( REQUESTTYPE_NONE ),
	_requestState						( REQUESTSTATE_NONE ),
	_currentOnlineTaskHandle			( 0 ),
	_isCrossTitleInviteValid			( false ),
	_clanNoticeServerBufferBytesUsed	( 0 ),
	_playerPresenceHelper				( 0 ),
	_matchSearchHelper					( 0 ),
	_fontPlugin							( 0 ),
	_audioPlugin						( 0 ),
	_uiPlugin							( 0 ),
	_supportPlugin						( 0 ),
	_voicemailPlugin					( 0 ),
	_uix								( 0 ),
	_lastUIXWorkFlags					( 0 ),
	_currentFeature						( OSI_UIX_NO_FEATURE_ACTIVE ),
	_lastExitedFeature					( OSI_UIX_NO_FEATURE_ACTIVE ),
	_didAnyFeatureExitThisFrame			( false ),
	_uixFriendsList						( 0 ),
	_showCurrentPlayersInPlayersList	( false ),
	_allowPlayerToInviteFriends			( false )
{
	_activeAccount = &_activeAccountData;
	start();

	_requestPlayerXUID.dwUserFlags = 0;
	_requestPlayerXUID.qwTeamID = 0;
	_requestClanXUID.dwUserFlags = 0;
	_requestClanXUID.qwTeamID = 0;
	_requestPlayerName[0] = '\0';
	_matchForFriendID.xBox.userFlags = 0;
	_matchForFriendID.xBox.userId = 0;
	memset( &_crossTitleInvite, 0, sizeof( _crossTitleInvite ) );

	ZeroMemory( _clanNoticeServerBuffer, sizeof( _clanNoticeServerBuffer ) );
}

///------------------------------------------------------------------------------------------------
OSIXbox::~OSIXbox()
///------------------------------------------------------------------------------------------------
{
	if( _uixFriendsList )					{ _uixFriendsList->Release(); }
	if( _uix )								{ uixShutdown(); }
	if( _matchSearchHelper )				{ delete _matchSearchHelper; }
	if( _playerPresenceHelper )				{ delete _playerPresenceHelper; }
	if( ClansAdapter::OSIGetSingleton() )	{ ClansAdapter::OSIDestroySingleton(); }
	if( StatsAdapter::OSIGetSingleton() )	{ StatsAdapter::OSIDestroySingleton(); }
}


///------------------------------------------------------------------------------------------------
bool OSIXbox::start()
///------------------------------------------------------------------------------------------------
{
	return true;
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_runFrame()
///------------------------------------------------------------------------------------------------
{
	if( !g_errorStack.IsEmpty() ) { return; }

	_runFrameVoice();
	bool workResult = uixDoWorkEveryFrame();

	_isConnected = false;
	if( workResult )
	{
		_isConnected = true;
	}
	else
	{
		g_errorStack.SetError( " ", getErrorString(), CErrorStack::ERROR_FATAL, 7000 );
		return;
	}

	// make sure the hosted session is always processed.
	// creation does its own processing because it cares if the process fails, 
	// and process only returns the result of the operation one time.  if it 
	// happened to return it here it would be lost.
	// the hresult could be broken out into a class member or something, but fuck it.
	if( _isConnected && _hostedSession.Exists() && !_hostedSession.IsCreating() )
	{
		HRESULT hr = _hostedSession.Process();
		ASSERT( !FAILED( hr ) ); // SUCCEEDED and !FAILED are not the same
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_runFrameVoice()
///------------------------------------------------------------------------------------------------
{
	// Make sure XHV engine is updated regularly.
	CXHVVoiceManager* pXHVManager = NULL;
	pXHVManager = (CXHVVoiceManager*) g_CVoiceChat.getPlatformVoiceManager();
	if( pXHVManager && pXHVManager->GetXHVEnginePtr() )
	{
		pXHVManager->DoWork();
	}

	if( !g_input.ControllerLocked( 0 ) ) { return; }

	const ControllerInputDriver::HeadSetStatus headsetStatus 
		= g_input.GetController( 0 )->GetHeadSetStatus();
	setIsVoiceInputAvailable( ControllerInputDriver::INSERTED == headsetStatus );
	uixSetPlayVoicemailOnSpeakers( ControllerInputDriver::REMOVED == headsetStatus );
}

///------------------------------------------------------------------------------------------------
//! \brief Pump whichever online task is active.
//! \return True if the last request has completed.  
//! \return Use getLastRequestStatus to determine if the task was successful or not.
///------------------------------------------------------------------------------------------------
bool OSIXbox::processLastRequest()
///------------------------------------------------------------------------------------------------
{
	if( !g_errorStack.IsEmpty() ) { return false; }

	if( REQUESTTYPE_NONE == _currentRequestType ) { return true; }

	if( !XOnlineGetLogonUsers() )
	{ 
		_setError( XONLINE_E_LOGON_CONNECTION_LOST );
		g_errorStack.SetError( " ", getErrorString(), CErrorStack::ERROR_FATAL, 7000 );
		return false; 
	}

	switch( _currentRequestType )
	{
	case REQUESTTYPE_COMMON_MATCH_QUERY:				_processCommonMatchQuery();				break;
	case REQUESTTYPE_HOST_MATCH:						_processHostMatch();					break;
	case REQUESTTYPE_JOIN_MATCH:						_processJoinMatch();					break;
	case REQUESTTYPE_MATCH_FOR_FRIEND:					_processMatchForFriend();				break;
	case REQUESTTYPE_LOGOUT:							_processLogout();						break;
	case REQUESTTYPE_FRIEND_LIST:						_processFriendList();					break;
	case REQUESTTYPE_END_ROUND:							_processEndRound();						break;
	case REQUESTTYPE_STATS_FOR_PLAYERS:					_processStatsForPlayers();				break;
	case REQUESTTYPE_STATION_URL_LIST_FOR_MATCH:		_processStationURLListForMatch();		break;
	case REQUESTTYPE_STATS_FOR_PLAYER_LADDER:			_processStatsForPlayerLadder();			break;
	case REQUESTTYPE_STATS_FOR_CLAN_LADDER:				_processStatsForClanLadder();			break;
	case REQUESTTYPE_VERIFY_STRING:						_processVerifyString();					break;
	case REQUESTTYPE_SEND_FEEDBACK:						_processSendFeedback();					break;
	case REQUESTTYPE_QUIT_HOSTED_MATCH:					_processQuitHostedMatch();				break;
	case REQUESTTYPE_UPDATE_RANK_AND_RATING:			_processUpdateMyRankAndRating();		break;

	case REQUESTTYPE_CLAN_CREATE:						_processClanCreate();					break;
	case REQUESTTYPE_CLAN_JOIN_BY_INVITATION:			_processClanJoinByInvitation();			break;
	case REQUESTTYPE_CLAN_DENY_INVITATION:				_processClanDenyInvitation();			break;
	case REQUESTTYPE_CLAN_BLOCK_INVITATION:				_processClanBlockInvitation();			break;
	case REQUESTTYPE_CLAN_SEND_INVITATION:				_processClanSendInvitation();			break;
	case REQUESTTYPE_CLAN_INVITATION_LIST:				_processClanInvitationList();			break;
	case REQUESTTYPE_CLAN_ID_LIST_FOR_PLAYER:			_processClanIdListForPlayer();			break;
	case REQUESTTYPE_CLAN_INFO:							_processClanInfo();						break;
	case REQUESTTYPE_CLAN_MEMBERSHIP_LIST:				_processClanMembershipList();			break;
	case REQUESTTYPE_CLAN_LEAVE:						_processClanLeave();					break;
	case REQUESTTYPE_CLAN_NOTICE_LIST:					_processClanNoticeList();				break;
	case REQUESTTYPE_CLAN_POST_NOTICE:					_processClanPostNotice();				break;
	case REQUESTTYPE_CLAN_KICK_MEMBER:					_processClanKickMember();				break;
	case REQUESTTYPE_CLAN_DEMOTE_MEMBER:				_processClanDemoteMember();				break;
	case REQUESTTYPE_CLAN_PROMOTE_MEMBER:				_processClanPromoteMember();			break;
	case REQUESTTYPE_CLAN_DELETE:						_processClanDelete();					break;
	case REQUESTTYPE_CLAN_MYINFO:						_processClanUpdateMyClanInformation();	break;
	case REQUESTTYPE_CLAN_TRANSFER_LEADERSHIP:			_processClanTransferLeadership();		break;

	case REQUESTTYPE_NONE:								return true;
	default: ASSERT( 0 );
	};

	return _requestStatus != STATUS_WORKING; // return true when processing is finished
}


///------------------------------------------------------------------------------------------------
//!	\brief Only implemented with UIX support enabled.
///------------------------------------------------------------------------------------------------
void OSIXbox::launchExternalAccountMaintenanceApp()
///------------------------------------------------------------------------------------------------
{
	if( _uix ) { _uix->Reboot( 0 ); }
}

///------------------------------------------------------------------------------------------------
//!	\brief Logout using UIX; only implemented with UIX support enabled.
//! \return true
///------------------------------------------------------------------------------------------------
bool OSIXbox::requestLogout()
///------------------------------------------------------------------------------------------------
{
//	ASSERT( REQUESTTYPE_NONE == _currentRequestType );
//	_setCurrentRequestType( REQUESTTYPE_LOGOUT );
	uixLogoff();
	return REQUEST_SUCCEEDED;
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processLogout()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_LOGOUT == _currentRequestType );

	//FIXME: does ILiveEngine::Logout really complete immediately?  I can't find any
	// task to pump or condition to check.
	_setCurrentRequestType( REQUESTTYPE_NONE );
	setLastRequestStatus( STATUS_SUCCEEDED );
}

///------------------------------------------------------------------------------------------------
//! \brief	This will do normal pumping operation on an XONLINETASK_HANDLE and handle setting status and state
//!			variables when the task is complete.  The calling code has the option of overriding changes that happen
//!			here after calling this function.
//! \return The handle of the task that completed.  This function sets _currentOnlineTaskHandle to 0 to indicate
//!			that there is no current task, but returns the handle unmolested so that calling code can use the handle
//!			to retrieve information.  The calling code must call XOnlineTaskClose when it is done with the handle.
///------------------------------------------------------------------------------------------------
XONLINETASK_HANDLE OSIXbox::_commonXOnlineTaskProcessing()
///------------------------------------------------------------------------------------------------
{
	ASSERT( _currentOnlineTaskHandle );
	
	const XONLINETASK_HANDLE handle = _currentOnlineTaskHandle;
	OSIRequestStatus status;

	const HRESULT continueHR = XOnlineTaskContinue( _currentOnlineTaskHandle );
	if( FAILED( continueHR ) ) //|| (S_FALSE == continueHR) )
	{
		status = STATUS_FAILED;
		_setCurrentRequestType( REQUESTTYPE_NONE );
		_setError( continueHR );
		_currentOnlineTaskHandle = 0;
		_requestState = REQUESTSTATE_NONE;
	}
	else if( (XONLINETASK_S_SUCCESS == continueHR) )// || (S_OK == continueHR) )
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
	return handle;
}


///------------------------------------------------------------------------------------------------
bool OSIXbox::isCrossTitleInvitiationPresent()
///------------------------------------------------------------------------------------------------
{
	static bool alreadyRetrievedInvite = false;

	if( alreadyRetrievedInvite )
	{
		return _isCrossTitleInviteValid;
	}
	else
	{
		const HRESULT getInviteHR = XOnlineFriendsGetAcceptedGameInvite( &_crossTitleInvite );
		_isCrossTitleInviteValid = (S_OK == getInviteHR) ;
		alreadyRetrievedInvite = true;
		return _isCrossTitleInviteValid;
	}
}

///------------------------------------------------------------------------------------------------
XONLINE_ACCEPTED_GAMEINVITE* OSIXbox::getCrossTitleInvitation()
///------------------------------------------------------------------------------------------------
{
	return &_crossTitleInvite;
}

///------------------------------------------------------------------------------------------------
//! \brief Should return XBox-specific and TCR-compliant message for whatever
///------------------------------------------------------------------------------------------------
const OSIchar* OSIXbox::getErrorString()
///------------------------------------------------------------------------------------------------
{
	switch( _errorHR )
	{
	case 0x80154005: // XONLINE_E_ACCOUNTS_ACCOUNT_BANNED, I don't know why this is not #defined anywhere...
		return "An Xbox Live account limitation does not allow you to use this feature.";

	case XONLINE_E_LOGON_CONNECTION_LOST:
		return "The connection to Xbox Live was lost.";

	case XONLINE_E_TEAMS_URL_CONTAINS_BAD_WORDS:
	case XONLINE_E_TEAMS_MOTTO_CONTAINS_BAD_WORDS:
	case XONLINE_E_TEAMS_DESCRIPTION_CONTAINS_BAD_WORDS:
	case XONLINE_E_TEAMS_NAME_CONTAINS_BAD_WORDS:
	case XONLINE_E_STRING_OFFENSIVE_TEXT:
		return "The text entered appears to contain offensive language and is not allowed. See the Xbox Live Code of Conduct for more information.";

	case XONLINE_E_NOTIFICATION_BLOCKED:
		return "That player has chosen to block any invitations sent from you.  If they send you an invitation, the block will be removed.";

	case XONLINE_E_LOGON_INVALID_USER:
		return "This account is not current. You need to update the account in Account Recovery.";

	case XONLINE_E_MATCH_INVALID_SESSION_ID:
		return( ::Printf( g_translator.Translate( "This game session is no longer available. Press %c to continue." ), 0x11) );

	case XONLINE_E_TEAMS_TEAM_ALREADY_EXISTS:
		return "That name is taken. Please try again.";

	case XONLINE_E_LOGON_USER_ACCOUNT_REQUIRES_MANAGEMENT:
		return "You have an important message from Xbox Live. You must read this message before you can sign in.";

	case XONLINE_E_LOGON_KICKED_BY_DUPLICATE_LOGON:
		return "You were signed out of Xbox Live because another person signed in using your account.";

	case XONLINE_E_LOGON_SERVERS_TOO_BUSY:
	case XONLINE_E_NOTIFICATION_SERVER_BUSY:
	case XONLINE_E_TEAMS_SERVER_BUSY:
		return "The Xbox Live service is very busy. Please try again later.";

	case XONLINE_E_LOGON_UPDATE_REQUIRED:
		return "A required update is available for the Xbox Live service. You cannot connect to Xbox Live until the update is installed.";
	
	case XONLINE_E_LOGON_CANNOT_ACCESS_SERVICE:
		return "Your Xbox console cannot connect to Xbox Live. Do you want to start the troubleshooter?";

	case XONLINE_E_TEAMS_USER_NOT_FOUND:
		return "Could not find a player with that name.";

	case XONLINE_E_TEAMS_TEAM_FULL:
		return "The clan is full. Please remove a member or cancel an outgoing clan invitation, then try again.";

	case XONLINE_E_TEAMS_TOO_MANY_REQUESTS:
		return "Too many requests.";

	case XONLINE_E_TEAMS_USER_ALREADY_EXISTS:
		return "That player is already a member, or has already been invited to be a member.";

	case XONLINE_E_TEAMS_NO_ADMIN:
		return "You must promote another clan member to leader status before leaving the clan.";

	case XONLINE_E_STAT_NOT_ENOUGH_PARAMETERS:
	case XONLINE_E_STAT_INVALID_PROCEDURE:
	case XONLINE_E_STAT_EXCEEDED_WRITE_READ_LIMIT:
	case XONLINE_E_STAT_LEADERBOARD_READONLY:
	case XONLINE_E_MATCH_INVALID_DATA_TYPE:
	case XONLINE_E_MATCH_REQUEST_TOO_SMALL:
	case XONLINE_E_MATCH_REQUEST_TRUNCATED:
	case XONLINE_E_MATCH_INVALID_VERSION:
	case XONLINE_E_MATCH_OVERFLOW:
	case XONLINE_E_MATCH_INVALID_RESULT_COL:
	case XONLINE_E_MATCH_INVALID_STRING:
	case XONLINE_E_MATCH_SESSION_ALREADY_EXISTS:
	case XONLINE_E_MATCH_CRITICAL_DB_ERR:
	case XONLINE_E_MATCH_NOT_ENOUGH_COLUMNS:
	case XONLINE_E_MATCH_PERMISSION_DENIED:
	case XONLINE_E_MATCH_INVALID_PART_SCHEME:
	case XONLINE_E_MATCH_DATA_TYPE_MISMATCH:
	case XONLINE_E_MATCH_NO_USERS:
	case XONLINE_E_MATCH_INVALID_BLOB:
	case XONLINE_E_NOTIFICATION_BAD_CONTENT_TYPE:
	case XONLINE_E_NOTIFICATION_REQUEST_TOO_SMALL:
	case XONLINE_E_NOTIFICATION_INVALID_MESSAGE_TYPE:
	case XONLINE_E_NOTIFICATION_NO_ADDRESS:
	case XONLINE_E_NOTIFICATION_NO_CONNECTION:
	case XONLINE_E_NOTIFICATION_SEND_FAILED:
	case XONLINE_E_NOTIFICATION_RECV_FAILED:
	case XONLINE_E_NOTIFICATION_MESSAGE_TRUNCATED:
	case WSAEADDRNOTAVAIL:
	case WSAECONNRESET:
	case XONLINE_E_OUT_OF_MEMORY:
	case E_OUTOFMEMORY:
	case XONLINE_E_NO_SESSION:
	case XONLINE_E_NO_USER:
	case XONLINE_E_USER_NOT_LOGGED_ON:
	case XONLINE_E_NO_GUEST_ACCESS:
	case XONLINE_E_NOT_INITIALIZED:
	default:
		return "An unexpected problem occurred. Please try again later.";
	};
	
	#ifdef _DEBUG
	char number[ 32 ];
	ltoa( _errorHR, number, 32 );
	OutputDebugString( number );
	#endif
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::isLastErrorFatal( void )
///------------------------------------------------------------------------------------------------
{
	switch( _errorHR )
	{
	case XONLINE_E_NO_SESSION:
	case XONLINE_E_NO_USER:
	case XONLINE_E_USER_NOT_LOGGED_ON:
	case XONLINE_E_NO_GUEST_ACCESS:
	case XONLINE_E_NOT_INITIALIZED:
	case XONLINE_E_LOGON_INVALID_USER:
	case XONLINE_E_LOGON_USER_ACCOUNT_REQUIRES_MANAGEMENT:
	case XONLINE_E_LOGON_KICKED_BY_DUPLICATE_LOGON:
	case XONLINE_E_LOGON_SERVERS_TOO_BUSY:
	case XONLINE_E_LOGON_UPDATE_REQUIRED:
	case XONLINE_E_LOGON_CANNOT_ACCESS_SERVICE:
	case XONLINE_E_LOGON_CONNECTION_LOST:
	case XONLINE_E_STAT_NOT_ENOUGH_PARAMETERS:
	case XONLINE_E_STAT_INVALID_PROCEDURE:
	case XONLINE_E_STAT_EXCEEDED_WRITE_READ_LIMIT:
	case XONLINE_E_STAT_LEADERBOARD_READONLY:
	case XONLINE_E_MATCH_INVALID_DATA_TYPE:
	case XONLINE_E_MATCH_REQUEST_TOO_SMALL:
	case XONLINE_E_MATCH_REQUEST_TRUNCATED:
	case XONLINE_E_MATCH_INVALID_VERSION:
	case XONLINE_E_MATCH_OVERFLOW:
	case XONLINE_E_MATCH_INVALID_RESULT_COL:
	case XONLINE_E_MATCH_INVALID_STRING:
	case XONLINE_E_MATCH_SESSION_ALREADY_EXISTS:
	case XONLINE_E_MATCH_CRITICAL_DB_ERR:
	case XONLINE_E_MATCH_NOT_ENOUGH_COLUMNS:
	case XONLINE_E_MATCH_PERMISSION_DENIED:
	case XONLINE_E_MATCH_INVALID_PART_SCHEME:
	case XONLINE_E_MATCH_DATA_TYPE_MISMATCH:
	case XONLINE_E_MATCH_NO_USERS:
	case XONLINE_E_MATCH_INVALID_BLOB:
	case XONLINE_E_NOTIFICATION_BAD_CONTENT_TYPE:
	case XONLINE_E_NOTIFICATION_REQUEST_TOO_SMALL:
	case XONLINE_E_NOTIFICATION_INVALID_MESSAGE_TYPE:
	case XONLINE_E_NOTIFICATION_NO_ADDRESS:
	case XONLINE_E_NOTIFICATION_NO_CONNECTION:
	case XONLINE_E_NOTIFICATION_SEND_FAILED:
	case XONLINE_E_NOTIFICATION_RECV_FAILED:
	case XONLINE_E_NOTIFICATION_SERVER_BUSY:
	case XONLINE_E_NOTIFICATION_MESSAGE_TRUNCATED:
	case WSAEADDRNOTAVAIL:
	case WSAECONNRESET:
	case XONLINE_E_OUT_OF_MEMORY:
	case E_OUTOFMEMORY:
		return true;

	default: 
		return false;
	};
}


///------------------------------------------------------------------------------------------------
OSIClanRole OSIXbox::XboxPrivileges_to_OSIClanRole( DWORD privileges )
///------------------------------------------------------------------------------------------------
{
	switch( privileges )
	{
	case s_ClanSoldierPrivileges:
		return OSI_CLAN_ROLE_SOLDIER;

	case s_ClanOfficerPrivileges:
		return OSI_CLAN_ROLE_OFFICER;

	case s_ClanLeaderPrivileges:
		return OSI_CLAN_ROLE_LEADER;

	default:
		return OSI_CLAN_ROLE_UNKNOWN;
	};
}

///------------------------------------------------------------------------------------------------
DWORD OSIXbox::OSIClanRole_to_XboxPrivileges( OSIClanRole role )
///------------------------------------------------------------------------------------------------
{
	switch( role )
	{
	case OSI_CLAN_ROLE_SOLDIER:
		return s_ClanSoldierPrivileges;

	case OSI_CLAN_ROLE_OFFICER:
		return s_ClanOfficerPrivileges;

	case OSI_CLAN_ROLE_LEADER:
		return s_ClanLeaderPrivileges;

	default:
		return 0;
	};
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_setHostedSessionOptions( const OSIMatchOptions& options )
///------------------------------------------------------------------------------------------------
{
	WCHAR hostTitle[ 16 ];
	OSIstrncpy( hostTitle, options.description, NUM_ARRAY_ELEMENTS( hostTitle ) );
	_hostedSession.SetConsoleRegion( XGetGameRegion() );
    _hostedSession.SetGameVersion( 1 ); // FIXME: get real game version
    _hostedSession.SetAdditionalContent( 1 ); // FIXME: get real content value
    _hostedSession.SetLevelIndex( options.levelIndex );
    _hostedSession.SetGameType( options.gameTypeIndex );
    _hostedSession.SetTimeLimit( options.timeLimit );
    _hostedSession.SetScoreLimit( options.scoreLimit );
    _hostedSession.SetTagLimit( options.tagLimit );
    _hostedSession.SetBestOfSeries( options.bestOfSeries );
    _hostedSession.SetPlayersRespawn( options.playersRespawn );
    _hostedSession.SetFriendlyFire( options.friendlyFire );
    _hostedSession.SetApprehension( options.apprehension );
    _hostedSession.SetSelfDamage( options.selfDamage );
    _hostedSession.SetAlternatingTeams( options.alternatingTeams );
    _hostedSession.SetMaxPlayers( options.maxPlayers );
	_hostedSession.SetHostTitle( hostTitle );
    _hostedSession.SetIsRanked( options.isRanked );
    _hostedSession.SetTotalConnectedPlayers( options.totalConnectedPlayers );
    _hostedSession.SetLoadOutLimit( 0 ); //FIXME: update matchoptions
    _hostedSession.SetVoiceCount( 0 ); //FIXME: update matchoptions

	if( options.isPrivate )
	{
		_hostedSession.PublicFilled				= 0;
		_hostedSession.PrivateFilled			= options.totalConnectedPlayers;
		_hostedSession.PublicOpen				= 0;
		_hostedSession.PrivateOpen				= options.maxPlayers - options.totalConnectedPlayers;
	}
	else
	{
		_hostedSession.PublicFilled				= options.totalConnectedPlayers;
		_hostedSession.PrivateFilled			= 0;
		_hostedSession.PublicOpen				= options.maxPlayers - options.totalConnectedPlayers;
		_hostedSession.PrivateOpen				= 0;
	}

}

///------------------------------------------------------------------------------------------------
bool OSIXbox::setIsVoiceInputAvailable( const bool isVoiceInputAvailable )
///------------------------------------------------------------------------------------------------
{
	OSIPlayer* player = getActivePlayer();
	
	if( player->state.isVoiceAvailable == isVoiceInputAvailable ) { return true; }

	bool oldValue = player->state.isVoiceAvailable;
	player->state.isVoiceAvailable = isVoiceInputAvailable;
	bool succeeded = quietPublishOnlineState( player->state );
	if( !succeeded )
	{
		// keep trying until it succeeds
		player->state.isVoiceAvailable = oldValue;
	}
	return succeeded;
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestSendFeedback( const XUID& feedbackFor, const XONLINE_FEEDBACK_TYPE feedbackType )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );
	ASSERT( !_currentOnlineTaskHandle );

	if( (0 == feedbackFor.qwUserID)
		|| ((int) feedbackType >= NUM_XONLINE_FEEDBACK_TYPES) ) 
	{
		_setError( E_FAIL ); 
		setLastRequestStatus( STATUS_FAILED );
		return false;
	}

	_requestPlayerXUID = feedbackFor;
	XONLINE_FEEDBACK_PARAMS feedbackParams;
	feedbackParams.lpStringParam = 0;

	const HRESULT hr = XOnlineFeedbackSend( 
		getActivePlayer()->state.currentControllerIndex,
		_requestPlayerXUID,
		feedbackType,
		&feedbackParams,
		0,
		&_currentOnlineTaskHandle );
	if( FAILED( hr ) )
	{
		setLastRequestStatus( STATUS_FAILED );
		_setError( hr );
		return REQUEST_FAILED;
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
		_setCurrentRequestType( REQUESTTYPE_SEND_FEEDBACK );
		return REQUEST_SUCCEEDED;
	}
}

///------------------------------------------------------------------------------------------------
void OSIXbox::_processSendFeedback()
///------------------------------------------------------------------------------------------------
{
	ASSERT( REQUESTTYPE_SEND_FEEDBACK == _currentRequestType );
	XONLINETASK_HANDLE taskHandle = _commonXOnlineTaskProcessing();
	if( STATUS_WORKING != getLastRequestStatus() )
	{
		XOnlineTaskClose( taskHandle );
	}
}

///------------------------------------------------------------------------------------------------
bool OSIXbox::requestFilterTextForProfanity( const OSIchar* textToFilter )
///------------------------------------------------------------------------------------------------
{
	if( !isOkToRequest() ) { return false; }

	ASSERT( REQUESTTYPE_NONE == _currentRequestType );
	ASSERT( !_currentOnlineTaskHandle );
	ASSERT( textToFilter );
	
	if( !textToFilter || !textToFilter[0] )
	{
		_setError( E_FAIL ); 
		setLastRequestStatus( STATUS_FAILED );
		return false;
	}

	//WCHAR matchNameWideCopy[ 256 ]; 
	const int len = OSIstrlen( textToFilter );
	WCHAR* wideCopyPtr = new WCHAR[ len + 1 ];
	const WCHAR* wideCopyConstPtr = wideCopyPtr;
	OSIstrncpy( wideCopyPtr, textToFilter, len + 1 );
	HRESULT hr = XOnlineStringVerify( 
		1,
		&wideCopyConstPtr,
		CXboxLive::GetFilteredLanguage(),
		NULL,
		&_currentOnlineTaskHandle );
	
	delete [] wideCopyPtr;

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
		_setCurrentRequestType( REQUESTTYPE_VERIFY_STRING );
		return REQUEST_SUCCEEDED;
	}
}

//-------------------------------------------------------------------------------------------------
bool OSIXbox::isOkToRequest()
{
	if( g_errorStack.IsFatal() ) 
	{ 
#ifdef ENABLE_OSI_XBOX_LOG
		Log( "isOkToRequest failed, g_errorStack.IsFatal() == true (%s, %s)\n", g_errorStack.GetTitle(), g_errorStack.GetMessage() );
#endif

		return false; 
	}

	OSIXbox* osix = (OSIXbox*) OSI::getSingletonPtr();
	if( !osix )
	{
		ASSERT( osix );
		return false; 
	}

	if( !XOnlineGetLogonUsers() ) 
	{ 
#ifdef ENABLE_OSI_XBOX_LOG
		OSIXbox::Log( "isOkToRequest failed, !XOnlineGetLogonUsers()\n" );
#endif 

		osix->_setError( XONLINE_E_LOGON_CONNECTION_LOST );
		return false; 
	}

	if( osix->_currentRequestType != REQUESTTYPE_NONE ) 
	{ 
#ifdef ENABLE_OSI_XBOX_LOG
		OSIXbox::Log( "isOkToRequest failed, _currentRequestType == %u\n", osix->_currentRequestType );
#endif

		//osix->_setError( E_FAIL );
		ASSERT( REQUESTTYPE_NONE == osix->_currentRequestType ); 
		return false;
	}

	return true;
}

#ifdef ENABLE_OSI_XBOX_LOG
/*static*/ void OSIXbox::_openLog()
{
	if( _logFile ) { return; }

	char filename[ 256 ];
	SYSTEMTIME time;
	GetLocalTime( &time );
	
	XNADDR xna;
	XNetGetDebugXnAddr( &xna );
	__int64 mac = 0;
	memcpy( &mac, xna.abEnet, 6 );
	
	sprintf( filename, "z:\\%I64X-%02i_%02i_%04i-%02i_%02i_%02i", 
		mac,
		time.wMonth, time.wDay, time.wYear,
		time.wHour, time.wMinute, time.wSecond );
	_logFile = fopen( filename, "wt" );
	ASSERTS( _logFile, "OSI Xbox Log failed to open" );
	
	if( !_logFile ) { return; }

	setvbuf( _logFile, 0, _IONBF, 0 );
}

/*static*/ void OSIXbox::LogNoStamp( const char* format, ... )
{
	_openLog();
	ASSERTS( _logFile, "OSI Xbox Log failed to open" );
	if( !_logFile ) { return; }
	va_list args;
	va_start( args, format );
	vfprintf( _logFile, format, args );
	va_end( args );
}

/*static*/ void OSIXbox::Log( const char* format, ... )
{
	_openLog();
	ASSERTS( _logFile, "OSI Xbox Log failed to open" );
	if( !_logFile ) { return; }

	SYSTEMTIME time;
	GetLocalTime( &time );
	fprintf( _logFile, "[%02u:%02u:%02u.%03u] ",
		time.wHour, time.wMinute, time.wSecond, time.wMilliseconds );

	va_list args;
	va_start( args, format );
	vfprintf( _logFile, format, args );
	va_end( args );
}
#endif // ENABLE_OSI_XBOX_LOG
