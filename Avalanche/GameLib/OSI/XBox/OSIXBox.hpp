#pragma once

#include "../osi.hpp"
#include <xonline.h>
#include <xhv.h>
#include <uix.h>
#include "OSIXboxPlayersList.hpp"
#include "XboxMatchSim.h"
#include "OSIXboxUtility.hpp"
#include "OSIXboxPresenceUtility.hpp"
#include "OSIXboxMatchmakingUtility.hpp"

// This is for debugging complex problems in DVD builds.
// All the logging code is #ifdef'd out and should not be enabled on a normal build.  
// This is intended only for special builds that QA gets directly from a programmer.
// DO NOT CHECK THIS FILE IN WITH THE FOLLOWING LINE UNCOMMENTED, AND DO NOT
// CHECK IN ANYTHING THAT WILL DEFINE ENABLE_OSI_XBOX_LOG UNDER ANY CIRCUMSTANCES.
//#define ENABLE_OSI_XBOX_LOG

class OSIXbox : public OSI
{
public:
	OSIXbox();
	virtual ~OSIXbox();
	static void*	operator new( size_t size )		{ return OSImalloc( size ); }
	static void		operator delete( void* memPtr )	{ OSIfree( memPtr ); }

	static bool isOkToRequest();

	///--------------------------------------------------------
	//! \name Hardware Profiles
	//! \{
		/* empty */ virtual OSIHardwareProfileID&	getDefaultHardwareProfile( bool loadFromMemoryCard ) { static OSIHardwareProfileID i; return i; }
		/* empty */ virtual bool	requestHardwareProfileActivation( OSIHardwareProfileID& profileID ) { return REQUEST_SUCCEEDED; }
	//! \}
	///--------------------------------------------------------

	///--------------------------------------------------------
	//! \name Logon
	//! \{
	virtual bool	requestLogout();
	virtual void	launchExternalAccountMaintenanceApp();
		/* empty */ virtual bool	requestAccountInfo() { return REQUEST_SUCCEEDED; }
		/* empty */ virtual bool	requestCreateAccount( OSIAccount& account ) { return false; }
		/* empty */ virtual bool	requestEditAccount(const OSIchar *name, const OSIchar *password, OSIAccount& account) { return false; }
		/* empty */ virtual bool	requestDestroyAccount( OSIAccountID& accountID ) { return false; }
		/* empty */ virtual bool	requestLogin( OSIAccountID& accountID ) { return REQUEST_SUCCEEDED; }
		/* empty */ virtual bool	requestLogin( const OSIchar*, const OSIchar* ) { return REQUEST_SUCCEEDED; }
		/* empty */ virtual bool	isRequiredMessagePending() { return false; }
		/* empty */ virtual bool	isRecommendedMessagePending() { return false; }
	//! \}
	///--------------------------------------------------------

	///--------------------------------------------------------
	//! \name Friends
	//! \{
	virtual bool	requestFriendList();
	virtual bool	requestPublishOnlineState( OSIPlayerState& state );
	bool			quietPublishOnlineState( OSIPlayerState& state );
	virtual bool	requestMatchForFriend( OSIPlayerID& id );
	virtual bool	requestOfferMyFriendship( OSIPlayerID& id );
		/* empty */ virtual bool	requestAuthorizeTheirFriendship( OSIPlayerID& id ) { return false; }
		/* empty */ virtual bool	requestDenyTheirFriendship( OSIPlayerID& id ) { return false; }
		/* empty */ virtual bool	requestBlockTheirFriendship( OSIPlayerID& id ) { return false; }
		/* empty */ virtual bool	requestUnBlockTheirFriendship( OSIPlayerID& id ) { return false; }
		/* empty */ virtual bool	requestOfferMyFriendship( const OSIchar* name ) { return false; }
		/* empty */ virtual bool	requestCancelMyFriendshipOffer( OSIPlayerID& id ) { return false; }
		/* empty */ virtual bool	requestEndFriendship( OSIPlayerID& id ) { return false; }
		/* empty */ virtual bool	isNewMatchInvitationWaiting() { return false; }
	//! \}
	///--------------------------------------------------------
	
	///--------------------------------------------------------
	//! \name Players
	//! \{
		/* empty */ virtual bool	requestSendPlayerFeedback( OSIPlayerID& id, OSIFeedback& feedback ) { return false; }
		/* empty */ virtual bool	requestMutePlayer( OSIPlayerID& id )								{ return false; }
	//! \}
	///--------------------------------------------------------

	///--------------------------------------------------------
	//! \name Rounds
	//! \{
	virtual bool	requestCreateRound();
	virtual bool	requestJoinRound();
	virtual bool	requestStartRound();
	virtual bool	requestEndRound( OSIRoundStats& stats);
	//! \}
	///--------------------------------------------------------

	///--------------------------------------------------------
	//! \name Ladder & Stats
	//! \{
	virtual bool	requestStatsPlayerList( OSIPlayerStatsList& players );
	virtual bool	requestStatsPlayerLadder( OSIPlayerID& pivotPlayer, const int numRanks );
	virtual bool	requestStatsTopPlayers( const int numRanks );

	virtual bool	requestStatsTopClans( int numClans );
	//! \}
	///--------------------------------------------------------

	///--------------------------------------------------------
	//! \name Voice
	//! \{
		/* empty */ virtual bool	requestOpenVoiceConnection( OSIPlayerID& id )			{ return false; }
		/* empty */ virtual bool	requestCloseVoiceConnection( OSIPlayerID& id )			{ return false; }
		/* empty */ virtual bool	startVoiceClipRecording()								{ return false; }
		/* empty */ virtual bool	stopVoiceClipRecording()								{ return false; }
		/* empty */ virtual bool	attachVoiceClipToNextGameInvitation()					{ return false; }
	//! \}
	///--------------------------------------------------------
	
	///--------------------------------------------------------
	//! \name Matchmaking
	//! \{
	virtual bool	requestHostMatch( OSIMatchOptions& options );
	virtual bool	requestJoinMatch( OSIMatchID& matchID );
	virtual bool	requestMatchList( OSIMatchOptions& options );
	virtual bool	requestUpdateMatchOptions( OSIMatchOptions& options );
	virtual bool	requestQuitMatch();
	virtual bool	requestValidateMatchName( const OSIchar* matchName );
		/* empty */ virtual bool	requestLadderMatch( OSIMatchOptions& options )			{ return false;}
		/* empty */ virtual bool	requestInvitePlayerToMatch( OSIPlayerID& playerID )		{ return false;}
		/* empty */ virtual bool	requestDeclineMatchInvite( OSIPlayerID& playerID )		{ return false;}
		/* empty */ virtual bool	requestMatchInvitations()								{ return false;}
	//! \}
	///--------------------------------------------------------

	///--------------------------------------------------------
	//! \name Match Lobby
	//! \{
		/* empty */ virtual bool	requestLaunchGame()										{ return false; }
		/* empty */ virtual bool	requestLaunchChat()										{ return false; }
		/* empty */ virtual bool	requestSendTextMessage( const OSIchar* message )		{ return false; }
		/* empty */ virtual void	publishGlobalVoiceMute( bool muteAllVoice )				{}
		/* empty */ virtual void	publishMyGameReadiness( bool readyToPlay )				{}
		/* empty */ virtual bool	isOpponentReadyToPlayGame()								{ return false; }
	//! \}
	///--------------------------------------------------------

	///--------------------------------------------------------
	//! \name Notification of player network events
	//! \{
	virtual bool	playerJoinCurrentMatch( const OSIchar* playerName, OSIPlayerID& id, int numPlayersInMatch );
	virtual bool	playerLeaveCurrentMatch( const OSIchar* playerName, OSIPlayerID& id, int numPlayersInMatch );
	//! \}
	///--------------------------------------------------------

	///--------------------------------------------------------
	//! \name Clans
	//! \{
	virtual bool	requestClanCreate( const OSIchar* const tag, const OSIchar* const name );
	virtual bool	requestClanUpdateMyClanInformation();
	virtual bool 	requestClanJoinByInvitation( const OSIClanInvitation& invitation );
	virtual bool 	requestClanDenyInvitation( const OSIClanInvitation& invitation );
	virtual bool	requestClanBlockInvitation( const OSIClanInvitation& invitation );
	virtual bool	requestClanSendInvitation( const OSIClanID& clanID, const OSIchar* const playerName );
	virtual bool	requestClanCancelSentInvitation( const OSIClanInvitation& invitation );
	virtual bool	requestClanInvitationList();
	virtual bool	requestClanIDListForPlayer( const OSIPlayerID& playerID );
	virtual bool	requestClanInfoList( const OSIClanID& clanID );
	virtual bool	requestClanMembershipList( const OSIClanID& clanID );
	virtual bool	requestClanLeave( const OSIClanID& clanID );
	virtual bool	requestClanNoticeList( const OSIClanID& clanID );
	virtual bool	requestClanPostNotice( const OSIClanID& clanID, const OSIchar* const notice );
	virtual bool	requestClanKickMember( const OSIClanID& clanID, const OSIPlayerID& playerID );
	virtual bool	requestClanDemoteMember( const OSIClanID& clanID, const OSIPlayerID& playerID );
	virtual bool	requestClanPromoteMember( const OSIClanID& clanID, const OSIPlayerID& playerID );
	virtual bool	requestClanTransferLeadership( const OSIClanID& clanID, const OSIPlayerID& playerID );
	//! \}
	///--------------------------------------------------------

	///--------------------------------------------------------
	//! \name Misc	
	//! \{
	virtual bool isLastErrorFatal( void );
	virtual const OSIchar* getErrorString();
	virtual bool	start();
	virtual bool	processLastRequest();
	virtual bool	requestStationURLListForMatch( OSIMatchID& matchID );
	virtual bool	requestHostMigration( OSIPlayerID& newHostID, int numPlayersInMatch );
	bool			setIsVoiceInputAvailable( const bool isVoiceInputAvailable );
	HRESULT			getLastError() const { return _errorHR; }
	virtual bool	requestFilterTextForProfanity( const OSIchar* textToFilter );
		/* empty */ virtual void	updateInvitations() {}
	//! \}
	///--------------------------------------------------------

	///--------------------------------------------------------
	//! \name OSIXbox specific
	//! \{
	XONLINE_ACCEPTED_GAMEINVITE* getCrossTitleInvitation();
	bool isCrossTitleInvitiationPresent();
	void beginMatchSearchesForClanMembershipList();
	int	updateMatchSearchesForClanMembershipList(); ///< returns the number of members that had their match info updated
	void endMatchSearchesForClanMembershipList();
	bool requestSendFeedback( const XUID& feedbackFor, const XONLINE_FEEDBACK_TYPE feedbackType );
	bool requestUpdateMyRankAndRating();
	//! \}
	///--------------------------------------------------------

	
	///--------------------------------------------------------
	//! \name UIX Configuration
	//! \{
	void	uixSetSupportPlugin( IPluginSupport* p );
	void	uixSetTitleAudioPlugin( ITitleAudioPlugin* p );
	void	uixSetTitleFontRendererPlugin( ITitleFontRenderer* p );
	void	uixSetVoicemailPlugin( XHVEngine* p );
	//! \}
	///--------------------------------------------------------
		
	///--------------------------------------------------------
	//! \name UIX Startup / Shutdown
	//! \{
	void	uixStartup();
	bool	uixIsStarted() { return _uix != NULL; }
	void	uixSetInputForControllerPort( const int controllerPort, XINPUT_STATE* const input );
	bool	uixDoWorkEveryFrame();
	void	uixShutdown();
	void	uixEndFeature();
	void	uixSetPlayVoicemailOnSpeakers( bool voicemailOnSpeakers );
	//! \}
	///--------------------------------------------------------

	///--------------------------------------------------------
	//! \name UIX Status
	//! \{
	bool	uixMustReboot();
	void	uixRebootNow();
	bool	uixMustRender();
	void	uixRenderNow( IDirect3DSurface8* pSurface );
	bool	uixIsProcessingInput();
	//! \}
	///--------------------------------------------------------

	///--------------------------------------------------------
	//! \name UIX Player status
	//! \{
	bool	uixIsReportingGameInviteNotification();
	bool	uixIsReportingFriendRequestNotification();
	//! \}
	///--------------------------------------------------------

	///--------------------------------------------------------
	//! \name UIX Logon / Logoff
	//! \{
	void	uixBeginLogon( const UIX_LOGON_TYPE logonType, const XONLINE_LOGON_STATE* const optionalRetrievedLogonState, XONLINE_ACCEPTED_GAMEINVITE* const optionalCrossTitleInvite );
	bool	uixUpdateLogon();
	bool	uixDidLogonFinishThisFrame();
	bool	uixWasLogonRejectedByServer();
	bool	uixWasLogonCancelledByUser();
	void	uixLogoff();
	//! \}
	///--------------------------------------------------------

	///--------------------------------------------------------
	//! \name UIX Friends
	//! \{
	void	uixBeginFriendList( const OSIPlayerID* const optionalInitialSelectedFriend );
	void	uixUpdateFriendList();
	bool	uixDidFriendListFinishThisFrame();
	bool	uixWasFriendListInviteAcceptedForThisTitle();
	bool	uixWasFriendListInviteAcceptedForOtherTitle();
	bool	uixGetFriendForAcceptedInvite( OSIPlayerID& getFriendForAcceptedInvite );
	void	uixSetAllowPlayerToInviteFriends( const bool allowPlayerToInviteFriends );
	bool	uixGetSelectedFriendInFriendList( OSIPlayerID& id );
	//! \}
	///--------------------------------------------------------

	///--------------------------------------------------------
	//! \name UIX Recent players
	//! \{
	void	uixSetShowCurrentPlayersInPlayersList( const bool showCurrentPlayers );
	void	uixBeginPlayerList( const OSIPlayerID* const optionalInitialSelectedPlayer );
	void	uixUpdatePlayerList();
	bool	uixDidPlayerListFinishThisFrame();
	bool	uixGetSelectedPlayerInPlayerList( OSIPlayerID& id );
	//! \}
	///--------------------------------------------------------

	///--------------------------------------------------------
    //! \name UIX Accessors
	//! \{
	ITitleFontRenderer *getFontPlugin() { return _fontPlugin; }
	//! \}
	///--------------------------------------------------------

protected: //===============================================================================================================================================================

	friend class PlayerPresenceHelper;

	///--------------------------------------------------------
	/// Defines the states that OSI can be in.  
	/// No request funciton should be called when 
	/// _currentRequestType != REQUESTTYPE_NONE
	///--------------------------------------------------------
	enum RequestType
	///--------------------------------------------------------
	{
		REQUESTTYPE_NONE,
		REQUESTTYPE_HOST_MATCH,
		REQUESTTYPE_COMMON_MATCH_QUERY,
		REQUESTTYPE_JOIN_MATCH,
		REQUESTTYPE_MATCH_FOR_FRIEND,
		REQUESTTYPE_LOGOUT,
		REQUESTTYPE_FRIEND_LIST,
		REQUESTTYPE_END_ROUND,
		REQUESTTYPE_STATS_FOR_PLAYERS,
		REQUESTTYPE_STATS_FOR_PLAYER_LADDER,
		REQUESTTYPE_STATS_FOR_CLAN_LADDER,
		REQUESTTYPE_STATION_URL_LIST_FOR_MATCH,
		REQUESTTYPE_VERIFY_STRING,
		REQUESTTYPE_SEND_FEEDBACK,
		REQUESTTYPE_QUIT_HOSTED_MATCH,
		REQUESTTYPE_UPDATE_RANK_AND_RATING,
		
		// Clans
		REQUESTTYPE_CLAN_CREATE,
		REQUESTTYPE_CLAN_JOIN_BY_INVITATION,
		REQUESTTYPE_CLAN_DENY_INVITATION,
		REQUESTTYPE_CLAN_BLOCK_INVITATION,
		REQUESTTYPE_CLAN_SEND_INVITATION,
		REQUESTTYPE_CLAN_INVITATION_LIST,
		REQUESTTYPE_CLAN_ID_LIST_FOR_PLAYER,
		REQUESTTYPE_CLAN_INFO,
		REQUESTTYPE_CLAN_MEMBERSHIP_LIST,
		REQUESTTYPE_CLAN_LEAVE,
		REQUESTTYPE_CLAN_NOTICE_LIST,
		REQUESTTYPE_CLAN_POST_NOTICE,
		REQUESTTYPE_CLAN_KICK_MEMBER,
		REQUESTTYPE_CLAN_DEMOTE_MEMBER,
		REQUESTTYPE_CLAN_PROMOTE_MEMBER,
		REQUESTTYPE_CLAN_DELETE,
		REQUESTTYPE_CLAN_MYINFO,
		REQUESTTYPE_CLAN_TRANSFER_LEADERSHIP,

		NUM_REQUESTTYPE
	};

	///--------------------------------------------------------
	/// Defines substates for the OSI states in RequestType.
	/// These are used to switch on different _process* functions.
	///--------------------------------------------------------
	enum RequestState
	///--------------------------------------------------------
	{
		REQUESTSTATE_NONE,
		
		REQUESTSTATE_FINDMATCH_GET_LIST,
		REQUESTSTATE_FINDMATCH_QOS_PROBE,

		REQUESTSTATE_JOINMATCH_RESOLVE_MATCH_ID,

		REQUESTSTATE_MATCHFORFRIEND_WAIT_FOR_FRIEND_LIST,
		REQUESTSTATE_MATCHFORFRIEND_WAIT_FOR_FRIEND_SEARCH,

		REQUESTSTATE_CLAN_PROMOTE_DEMOTE_GET_MEMBERS,
		REQUESTSTATE_CLAN_PROMOTE_DEMOTE_UPDATE_MEMBER_PRIVILEGES,

		REQUESTSTATE_CLAN_MYINFO_GET_ID,
		REQUESTSTATE_CLAN_MYINFO_GET_MEMBERS,
		REQUESTSTATE_CLAN_MYINFO_GET_STATS,

		REQUESTSTATE_CLAN_POST_NOTICE_VERIFY_STRING,
		REQUESTSTATE_CLAN_POST_NOTICE_UPLOAD,

		REQUESTSTATE_CLAN_MEMBERSHIP_GET_LIST,

		REQUESTSTATE_CLAN_TRANSFER_PROMOTE_OTHER,
		REQUESTSTATE_CLAN_TRANSFER_DEMOTE_SELF,

		REQUESTSTATE_STATS_FOR_PLAYERS_GET_CLAN_INFO,
		REQUESTSTATE_STATS_FOR_PLAYERS_GET_STATS,

		REQUESTSTATE_CLAN_DELETE_CLAN,
		REQUESTSTATE_CLAN_DELETE_STATS,

		REQUESTSTATE_STATS_FOR_CLAN_LADDER_GET_STATS,
		REQUESTSTATE_STATS_FOR_CLAN_LADDER_GET_CLAN_INFO,

		REQUESTSTATE_CLAN_INVITE_SEND_ENUMERATE_MEMBERS,
		REQUESTSTATE_CLAN_INVITE_SEND,
	};

	///--------------------------------------------------------
	//! \name Inherited private functionality
	//! \{
	virtual void	_runFrame();
	virtual void	_populateMatchList();
	// Except where noted, these are not implemented/used because Live data shouldn't 
	// wait around for this to be called - it should always be processed immediately after completion.
		/* empty */ virtual void	_populateFriendList( void )				{}
		/* empty */ virtual void	_populateAccountList( void )			{} // not used because UIX handles all of the account/login functionality.
		/* empty */ virtual void	_populateHardwareProfileList( void )	{} // not used because UIX handles all of the account/login functionality.
		/* empty */ virtual void	_populateClanInvitationList()			{} 
		/* empty */ virtual void	_populateClanSentInvitationList()		{} 
		/* empty */ virtual void	_populateClanIDList()					{} 
		/* empty */ virtual void	_populateClanInfoList()					{} 
		/* empty */ virtual void	_populateClanMembershipList()			{} 
		/* empty */ virtual void	_populateClanNoticeList()				{} 
		/* empty */ virtual void	_populateStationURLList()				{} 
	//! \}
	///--------------------------------------------------------

	///--------------------------------------------------------
	//! \name Request processing
	//! \{
	void _processCommonMatchQuery();						///< REQUESTTYPE_COMMON_MATCH_QUERY
	void _processCommonMatchQueryGetList();					///< REQUESTTYPE_COMMON_MATCH_QUERY - REQUESTSTATE_FINDMATCH_GET_LIST
	void _processCommonMatchQueryQosProbe();				///< REQUESTTYPE_COMMON_MATCH_QUERY - REQUESTSTATE_FINDMATCH_QOS_PROBE
	void _processHostMatch();								///< REQUESTTYPE_HOST_MATCH
	void _processJoinMatch();								///< REQUESTTYPE_JOIN_MATCH
	void _processJoinMatchResolveMatchID();					///< REQUESTTYPE_JOIN_MATCH - REQUESTSTATE_JOINMATCH_RESOLVE_MATCH_ID
	void _processMatchForFriend();							///< REQUESTTYPE_MATCH_FOR_FRIEND
	void _processMatchForFriendWaitForFriendList();			///< REQUESTTYPE_MATCH_FOR_FRIEND - REQUESTSTATE_MATCHFORFRIEND_WAIT_FOR_FRIEND_LIST
	void _processMatchForFriendWaitForFriendSearch();		///< REQUESTTYPE_MATCH_FOR_FRIEND - REQUESTSTATE_MATCHFORFRIEND_WAIT_FOR_FRIEND_SEARCH
	void _processLogout();									///< REQUESTTYPE_LOGOUT
	void _processFriendList();								///< REQUESTTYPE_FRIEND_LIST
	void _processEndRound();								///< REQUESTTYPE_END_ROUND
	void _processEndRoundGetClanInfo();						///< REQUESTTYPE_END_ROUND - REQUESTSTATE_ENDROUND_GET_CLAN_INFO
	void _processEndRoundSubmitStats();						///< REQUESTTYPE_END_ROUND - REQUESTSTATE_ENDROUND_SUBMIT_STATS
	void _processClanCreate();								///< REQUESTTYPE_CLAN_CREATE
	void _processClanJoinByInvitation();					///< REQUESTTYPE_CLAN_JOIN_BY_INVITATION
	void _processClanDenyInvitation();						///< REQUESTTYPE_CLAN_DENY_INVITATION
	void _processClanBlockInvitation();						///< REQUESTTYPE_CLAN_BLOCK_INVITATION
	void _processClanSendInvitation();						///< REQUESTTYPE_CLAN_SEND_INVITATION
	void _processClanSendInvitationEnumerateMembers();		///< REQUESTTYPE_CLAN_SEND_INVITATION - REQUESTSTATE_CLAN_INVITE_SEND_ENUMERATE_MEMBERS
	void _processClanSendInvitationSend();					///< REQUESTTYPE_CLAN_SEND_INVITATION - REQUESTSTATE_CLAN_INVITE_SEND
	void _processClanInvitationList();						///< REQUESTTYPE_CLAN_INVITATION_LIST
	bool _processClanInvitationListTeamDetailsBatch( XONLINETASK_HANDLE taskHandle ); ///< REQUESTTYPE_CLAN_INVITATION_LIST
	void _processClanIdListForPlayer();						///< REQUESTTYPE_CLAN_ID_LIST_FOR_PLAYER
	void _processClanInfo();								///< REQUESTTYPE_CLAN_INFO
	void _processClanMembershipList();						///< REQUESTTYPE_CLAN_MEMBERSHIP_LIST
	void _processClanMembershipGetList();					///< REQUESTTYPE_CLAN_MEMBERSHIP_LIST - REQUESTSTATE_CLAN_MEMBERSHIP_GET_LIST
	void _processClanLeave();								///< REQUESTTYPE_CLAN_LEAVE
	void _processClanNoticeList();							///< REQUESTTYPE_CLAN_NOTICE_LIST
	void _processClanPostNotice();							///< REQUESTTYPE_CLAN_POST_NOTICE
	void _processClanPostNoticeVerify();					///< REQUESTTYPE_CLAN_POST_NOTICE - REQUESTSTATE_CLAN_POST_NOTICE_VERIFY_STRING
	void _processClanPostNoticeUpload();					///< REQUESTTYPE_CLAN_POST_NOTICE - REQUESTSTATE_CLAN_POST_NOTICE_UPLOAD
	void _processClanKickMember();							///< REQUESTTYPE_CLAN_KICK_MEMBER
	void _processClanDemoteMember();						///< REQUESTTYPE_CLAN_DEMOTE_MEMBER
	void _processClanPromoteMember();						///< REQUESTTYPE_CLAN_PROMOTE_MEMBER
	void _processClanDelete();								///< REQUESTTYPE_CLAN_DELETE
	void _processClanDeleteClan();							///< REQUESTTYPE_CLAN_DELETE - REQUESTSTATE_CLAN_DELETE_CLAN
	void _processClanDeleteStats();							///< REQUESTTYPE_CLAN_DELETE - REQUESTSTATE_CLAN_DELETE_STATS
	void _processClanUpdateMyClanInformation();				///< REQUESTTYPE_CLAN_MYINFO
	void _processClanUpdateMyClanInformationGetID();		///< REQUESTTYPE_CLAN_MYINFO - REQUESTSTATE_CLAN_MYINFO_GET_ID
	void _processClanUpdateMyClanInformationGetMembers();	///< REQUESTTYPE_CLAN_MYINFO - REQUESTSTATE_CLAN_MYINFO_GET_MEMBERS
	void _processClanUpdateMyClanInformationGetStats();		///< REQUESTTYPE_CLAN_MYINFO - REQUESTSTATE_CLAN_MYINFO_GET_STATS
	void _processClanPromoteDemoteGetMembers();				///< REQUESTTYPE_CLAN_DEMOTE_MEMBER | REQUESTTYPE_CLAN_PROMOTE_MEMBER - REQUESTSTATE_CLAN_PROMOTE_DEMOTE_GET_MEMBERS
	void _processClanPromoteDemoteUpdateMemberPrivileges();	///< REQUESTTYPE_CLAN_DEMOTE_MEMBER | REQUESTTYPE_CLAN_PROMOTE_MEMBER - REQUESTSTATE_CLAN_PROMOTE_DEMOTE_UPDATE_MEMBER_PRIVILEGES
	void _processClanTransferLeadership();					///< REQUESTTYPE_CLAN_TRANSFER_LEADERSHIP
	void _processClanTransferPromoteOther();				///< REQUESTTYPE_CLAN_TRANSFER_LEADERSHIP - REQUESTSTATE_CLAN_TRANSFER_PROMOTE_OTHER
	void _processClanTransferDemoteSelf();					///< REQUESTTYPE_CLAN_TRANSFER_LEADERSHIP - REQUESTSTATE_CLAN_TRANSFER_DEMOTE_SELF
	void _processStatsForPlayers();							///< REQUESTTYPE_STATS_FOR_PLAYERS
	void _processStatsForPlayersGetClanInfo();				///< REQUESTTYPE_STATS_FOR_PLAYERS - REQUESTSTATE_STATS_FOR_PLAYERS_GET_CLAN_INFO
	void _processStatsForPlayersGetStats();					///< REQUESTTYPE_STATS_FOR_PLAYERS - REQUESTSTATE_STATS_FOR_PLAYERS_GET_STATS
	void _processStatsForPlayerLadder();					///< REQUESTTYPE_STATS_FOR_PLAYER_LADDER 
	void _processStatsForClanLadder();						///< REQUESTTYPE_STATS_FOR_CLAN_LADDER
	void _processStatsForClanLadderGetStats();				///< REQUESTTYPE_STATS_FOR_CLAN_LADDER - REQUESTSTATE_STATS_FOR_CLAN_LADDER_GET_STATS
	void _processStatsForClanLadderGetClanInfo();			///< REQUESTTYPE_STATS_FOR_CLAN_LADDER - REQUESTSTATE_STATS_FOR_CLAN_LADDER_GET_CLAN_INFO
	void _processStationURLListForMatch();					///< REQUESTTYPE_STATION_URL_LIST_FOR_MATCH
	void _processVerifyString();							///< REQUESTTYPE_VERIFY_STRING
	void _processSendFeedback();							///< REQUESTTYPE_SEND_FEEDBACK
	void _processQuitHostedMatch();							///< REQUESTTYPE_QUIT_HOSTED_MATCH
	void _processUpdateMyRankAndRating();					///< REQUESTTYPE_UPDATE_RANK_AND_RATING
	//! \}
	///--------------------------------------------------------

	///--------------------------------------------------------
	//! \name Misc
	//! \{
	inline void	_setCurrentRequestType( RequestType t );
	XONLINETASK_HANDLE _commonXOnlineTaskProcessing();
	bool _commonRequestClanPromoteDemoteMember( const OSIClanID& clanID, const OSIPlayerID& playerID );
	void _commonProcessClanPromoteDemoteMember();
	bool _clanAnswerInvitation( const OSIClanInvitation& invitation, const XONLINE_PEER_ANSWER_TYPE answer, const RequestType requestType );
	HRESULT _setNotificationState( OSIPlayerState& state );
	void _convertClanNoticeServerChunkToList();
	void _convertClanNoticeListToServerChunk();
	inline void	_setError( const HRESULT hr );
	void _setHostedSessionOptions( const OSIMatchOptions& options );
	void _runFrameVoice();
	//! \}
	///--------------------------------------------------------

	///--------------------------------------------------------
	//! \name Constants
	//! \{
	static const DWORD s_ThisTitleID				= 0x45530018;				///< TitleID of this game.
	static const DWORD s_ClanLeaderPrivileges		= 0xffffffff;
	static const DWORD s_ClanOfficerPrivileges		= XONLINE_TEAM_MODIFY_DATA | XONLINE_TEAM_DELETE_MEMBER | XONLINE_TEAM_RECRUIT_MEMBERS;
	static const DWORD s_ClanSoldierPrivileges		= 0;
	static const DWORD s_MaxClanNotices				= 4;
	static const DWORD s_ClanNoticeServerBufferSize = 
		sizeof( unsigned char )							// num notices
		+ sizeof( OSIClanNotice ) * s_MaxClanNotices;	// notice data
	static OSIClanRole	XboxPrivileges_to_OSIClanRole( DWORD privileges );
	static DWORD		OSIClanRole_to_XboxPrivileges( OSIClanRole role );
	//! \}
	///--------------------------------------------------------

	///--------------------------------------------------------
	OSIPlayerList					_scratchPlayerList;			///< Used when OSIXbox needs to maintain a list of players (requestStatsPlayerList)
	OSIClanInfoList					_scratchClanInfoList;		///< Used when OSIXbox needs to maintain a list of clan info (requestClanInvitationList)
	HRESULT							_errorHR;					///< Remembers the last error code that should be resolved for getErrorString
	RequestType						_currentRequestType;		///< Indicates the current request being serviced by OSI
	RequestType						_lastRequestType;			///< So _populateMatchList knows which query to use
	RequestState					_requestState;
	XUID							_requestPlayerXUID;			///< The player XUID that the last request was called with; not always valid, and not for every request
	XUID							_requestClanXUID;			///< The clan XUID that the last clan request was called with; not always valid, and not for every request
	OSIchar							_requestPlayerName[ 128 ];	///< The player name that the last request was called with; not always valid, and not for every request
	CCommonMatchQuery				_commonMatchQuery;			///< matchsim-generated class, used for optimatch and quickmatch
	CSpecificMatchQuery				_specificMatchQuery;		///< matchsim-generated class, used for finding a specific match by ID
	CMatchSimSession    			_hostedSession;				///< matchsim-generated class, handles QOS listening, match advertising, etc
	XONLINETASK_HANDLE				_currentOnlineTaskHandle;	///< Call XOnlineTaskContinue for this if it's valid
	OSIPlayerID						_matchForFriendID;			///< The Player id of the friend that requestMatchForFriend is searching on
	OSIAccount						_activeAccountData;			///< so OSI::_activeAccount can point to something
	XONLINE_ACCEPTED_GAMEINVITE		_crossTitleInvite;
	bool							_isCrossTitleInviteValid;
	char							_clanNoticeServerBuffer[ s_ClanNoticeServerBufferSize ];	///< Data buffer for dealing with clan notices
	DWORD							_clanNoticeServerBufferBytesUsed;							///< How much of _clanNoticeServerBuffer should be uploaded to Live
	PlayerPresenceHelper*			_playerPresenceHelper;
	MatchSearchHelper*				_matchSearchHelper;
	///--------------------------------------------------------

	///--------------------------------------------------------
	/// Lets OSI keep track of which UIX feature is active.
	///--------------------------------------------------------
	enum OSI_UIX_FEATURE
	///--------------------------------------------------------
	{
		OSI_UIX_NO_FEATURE_ACTIVE,
		OSI_UIX_LOGON,
		OSI_UIX_FRIENDS,
		OSI_UIX_PLAYERS
	};

	///--------------------------------------------------------
	//! \name UIX Data
	//! \{
	void					_uixCommonFeatureExit();
	bool					_uixCommonGetFriendExitInvite( const XONLINE_FRIEND*& friendInfo );
	ITitleFontRenderer*		_fontPlugin;				///< Tells UIX how to render text.
	ITitleAudioPlugin*		_audioPlugin;				///< Tells UIX how to render sounds.
	ITitleUIPlugin*			_uiPlugin;					///< Tells UIX how to manage and render UI elements.
	IPluginSupport*			_supportPlugin;				///< Tells UIX how to access stored resources. 
	IXHVEngine*				_voicemailPlugin;			///< Tells UIX how to do voicemail.
	ILiveEngine*			_uix;						///< The main UIX object.  Massive.
	UIX_EXIT_INFO			_lastFeatureExitInfo;		///< Keeps track of whatever the last call to any UIX feature returned.
	DWORD					_lastUIXWorkFlags;			///< Keeps track of what flags were specified by ILiveEngine::DoWork.
	OSI_UIX_FEATURE			_currentFeature;
	OSI_UIX_FEATURE			_lastExitedFeature;
	bool					_didAnyFeatureExitThisFrame;
	LiveFriendsList*		_uixFriendsList;
	OSIXboxPlayersList		_remotePlayersList;
	bool					_showCurrentPlayersInPlayersList;
	bool					_allowPlayerToInviteFriends;
	//! \}
	///--------------------------------------------------------

	///--------------------------------------------------------
	//! \name Logging
	//! \{
#ifdef ENABLE_OSI_XBOX_LOG
private:
	static FILE* _logFile;
	static void _openLog();
public:
	static void Log( const char* format, ... );
	static void LogNoStamp( const char* format, ... );
#endif // ENABLE_OSI_XBOX_LOG
	//! \}
	///--------------------------------------------------------
};

//-------------------------------------------------------------------------------------------------
//! \brief Internal helper function.
//! \param t The type of request that is currently being serviced by OSI.  There can be only one.
inline void OSIXbox::_setCurrentRequestType( RequestType t ) 
{ 
	_lastRequestType = _currentRequestType; 
	_currentRequestType = t; 
}

//-------------------------------------------------------------------------------------------------
inline void OSIXbox::_setError( const HRESULT hr )
{
	// YES there is no way to get through this function without ASSERTing!
	//ASSERT( FAILED( hr ) );			// why are you setting an error code that is not an error?
	ASSERT( SUCCEEDED( hr ) );		// this is here just to bring attention to the fact that something failed.
	ASSERT( FAILED( hr ) || SUCCEEDED( hr ) ); // didn't fail and didn't succeed?  wtf?

	_errorHR = hr;
}


#include "OSIXboxUtility.hpp"

