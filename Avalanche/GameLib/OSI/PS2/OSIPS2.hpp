//------------------------------------------------------
// OSIPS2.hpp -- Definition of OSIPS2 class.  This class
// is a implementation of the Online Services Interface
// (OSI).  It is built for the PS2, and wraps Quazal's
// RendezVous implementation for lobby services for PS2.
//------------------------------------------------------

// Forward declarations
class OSIPS2 ;
class OSPS2NotificationEventHandler ;

#ifndef _OSIPS2_HPP_
#define _OSIPS2_HPP_

#include "../OSI.hpp"
#include <rendezvous.h>
#include <services/matchmaking.h>
#include <services/friends.h>
#include <services/accountmanagement.h>
#include <services/competition.h>
#include <services/constants/statisticflags.h>
#include <services/peer/client/peermanager.h>
#include <services/peer/client/peerservice.h>
#include <services/extensions/hostmigration.h>
#include <services/nattraversal.h>

#ifndef DIRECTX_PC
#include <platform/stack/ps2common/ps2profilelist.h>
#endif // DIRECTX_PC

#include <platform/core/map.h>
 
#include "OSI/PS2/TTLSessionDDL.h"
#include "OSI/PS2/TTLRoundDDL.h"
#include "OSI/PS2/TTLMatchMakingDDL.h"
#include "OSI/PS2/EidosFriendDDL.h"
#include "OSI/PS2/EidosAccountPublicDDL.h"
#include "OSI/PS2/EidosAccountPrivateDDL.h"
#include "OSI/PS2/OSIPS2Alerts.hpp"

// Maximum timeout for DNAS connection (in seconds)
const int DNAS_TIMEOUT = 20;

#ifdef DIRECTX_PC
#define CD_KEY_LENGTH	19
#endif

//------------------------------------------------------------------
// Forward Declarations
//------------------------------------------------------------------
namespace Quazal
{
	class EidosAccountPublic;
	class EidosAccountPrivate;
}

class DNSResolutionThread;

///------------------------------------------------------------------
/// This class is registered with RendezVous to get callbacks on all
/// major events.  It enables OSIPS2 to take action on events.
///------------------------------------------------------------------
class OSIPS2NotificationEventHandler : public NotificationEventHandler
{
	public:
		OSIPS2NotificationEventHandler()	{ }
		~OSIPS2NotificationEventHandler()	{ }

		virtual void	ProcessNotificationEvent( const NotificationEvent& event );
};

///------------------------------------------------------------------
/// Light weight match class that cotains Rendezvous specific data
///------------------------------------------------------------------
class RVMatch
{
	public:
		typedef enum {
			PING_UNKNOWN,
			PING_WORKING,
			PING_KNOWN,
		} PingStatus ;

		// full constructor
		RVMatch( GatheringHolder& gHolder, PeerManager* manager ):
			holder(&gHolder),
			peerManager(manager),
			pingStatus( PING_UNKNOWN ),
			lastCallResult(false),
			commandFailed(false),
			hasMatchURLs(false),
			ping(Infinite),
			gID(gHolder.Get()->GetID()) 
			{ }

		// destructor
		~RVMatch()
		{
			if( context.GetState() == CallContext::CallInProgress )
				context.Cancel();
		}

		bool	update( void );
		void	beginPingCheck( void );

		ProtocolCallContext context;
		qList<StationURL>	lstStationURLs;
		GatheringHolder*	holder;
		PeerManager*		peerManager ;
		GatheringID			gID ;
		Chrono				pingChrono;
		PingStatus			pingStatus ;
		bool				lastCallResult;
		bool				commandFailed;
		bool				hasMatchURLs;
		int					ping;
};


///-----------------------------------------------------
/// PS2-specific implementation of OSI.  This is a wrapper
/// around Quazal's RendezVous.
///-----------------------------------------------------
class OSIPS2 : public OSI
{
public:

	// If you add any new requests, make sure these match one to one
	// with the string table in the function setRequestType.
	enum RequestType
	{
		REQUEST_TYPE_HARDWARE_ACTIVATION,
		REQUEST_TYPE_ACCOUNT_INFO,
		REQUEST_TYPE_LOGIN,
		REQUEST_TYPE_DNS_LOOKUP,
		REQUEST_TYPE_RENDEZVOUS_HOST_MATCH,
		REQUEST_TYPE_RENDEZVOUS_FIND_MATCH,
		REQUEST_TYPE_RENDEZVOUS_JOIN_MATCH,
		REQUEST_TYPE_RENDEZVOUS_CREATE_ACCOUNT,
		REQUEST_TYPE_RENDEZVOUS_EDIT_ACCOUNT,
		REQUEST_TYPE_RENDEZVOUS_SIMPLE_CALL,
		REQUEST_TYPE_RENDEZVOUS_FRIENDS_LIST,
		REQUEST_TYPE_RENDEZVOUS_QUIT_MATCH,
		REQUEST_TYPE_RENDEZVOUS_BEGIN_ROUND,
		REQUEST_TYPE_RENDEZVOUS_END_ROUND,
		REQUEST_TYPE_ACCEPT_MATCH_INVITE,
		REQUEST_TYPE_AUTHORIZE_FRIENDSHIP,
		REQUEST_TYPE_DENY_FRIENDSHIP,
		REQUEST_TYPE_BLOCK_FRIENDSHIP,
		REQUEST_TYPE_UNBLOCK_FRIENDSHIP,
		REQUEST_TYPE_OFFER_FRIENDSHIP,
		REQUEST_TYPE_CANCEL_FRIENDSHIP,
		REQUEST_TYPE_END_FRIENDSHIP,
		REQUEST_TYPE_INVITE_TO_MATCH,
		REQUEST_TYPE_GET_MATCH_INVITATIONS,
		REQUEST_TYPE_LOGOUT,
		REQUEST_TYPE_GET_PLAYERS_STATS,
		REQUEST_TYPE_GET_TOP_PLAYERS_STATS,
		REQUEST_TYPE_GET_TOP_CLANS_STATS,
		REQUEST_TYPE_GET_LADDER_STATS,
		REQUEST_TYPE_RENDEZVOUS_CREATE_ROUND,
		REQUEST_TYPE_RENDEZVOUS_JOIN_ROUND,
		REQUEST_TYPE_RENDEZVOUS_START_ROUND,
		REQUEST_TYPE_UPDATE_MATCH_OPTIONS,
		REQUEST_TYPE_FILTER_TEXT_FOR_PROFANITY,

		// Clans
		REQUEST_TYPE_CLAN_CREATE,
		REQUEST_TYPE_CLAN_UPDATE_MY_INFO,
		REQUEST_TYPE_CLAN_JOIN_BY_INVITATION,
		REQUEST_TYPE_CLAN_DENY_INVITATION,
		REQUEST_TYPE_CLAN_BLOCK_INVITATION,
		REQUEST_TYPE_CLAN_SEND_INVITATION,
		REQUEST_TYPE_CLAN_CANCEL_SENT_INVITATION,
		REQUEST_TYPE_CLAN_INVITATION_LIST,
		REQUEST_TYPE_CLAN_ID_LIST_FOR_PLAYER,
		REQUEST_TYPE_CLAN_INFO,
		REQUEST_TYPE_CLAN_MEMBERSHIP_LIST,
		REQUEST_TYPE_CLAN_LEAVE,
		REQUEST_TYPE_CLAN_NOTICE_LIST,
		REQUEST_TYPE_CLAN_POST_NOTICE,
		REQUEST_TYPE_CLAN_KICK_MEMBER,
		REQUEST_TYPE_CLAN_DEMOTE_MEMBER,
		REQUEST_TYPE_CLAN_PROMOTE_MEMBER,
		REQUEST_TYPE_CLAN_TRANSFER_LEADERSHIP,

		REQUEST_TYPE_STATION_URL_LIST,
		REQUEST_TYPE_HOST_MIGRATION,
		REQUEST_TYPE_PLAYER_JOIN_CURRENT_MATCH,
		REQUEST_TYPE_PLAYER_LEAVE_CURRENT_MATCH,
		REQUEST_TYPE_MESSAGE_OF_THE_DAY,

		REQUEST_TYPE_LAST_REQUEST_TYPE,
	};

	enum States
	{
		COMMAND_FAILURE,
		COMMAND_SUCCESS,

		// LOGIN STATES
		LOGIN_LOGIN,
		LOGIN_START_PEER_MANAGER,
		LOGIN_START_NAT_CLIENT,
		LOGIN_START_ACCOUNT_CLIENT,
		LOGIN_START_FRIENDS_CLIENT,
		LOGIN_START_MATCHMAKING_CLIENT,
		LOGIN_START_COMPETITION_CLIENT,
		LOGIN_FIND_NONLADDER_RANKING,
		LOGIN_CREATE_NONLADDER_RANKING,
		LOGIN_HOST_NONLADDER_COMPETITION,
		LOGIN_GET_PLAYER_NONLADDER_STATS,
		LOGIN_UPDATE_STATUS,
		LOGIN_RESOLVE_DNS,
#ifdef DIRECTX_PC
		LOGIN_CHECK_CD_KEY,
#endif

		// CREATE ACCOUNT
		CREATE_ACCOUNT_CREATE_ACCOUNT,
		CREATE_ACCOUNT_PREP_NEW_PLAYER,
		CREATE_ACCOUNT_LOGIN_NEW_PLAYER,
		CREATE_ACCOUNT_START_ACCOUNT_CLIENT,
		CREATE_ACCOUNT_UPDATE_DATA,
		CREATE_ACCOUNT_UPDATE_EMAIL,
		CREATE_ACCOUNT_LOGOUT,

		// EDIT ACCOUNT
		EDIT_ACCOUNT_PASSWORD,

		// LOGOUT
		LOGOUT_LOGOUT,

		// FIND MATCH
		FIND_MATCH_SEARCH,
		FIND_MATCH_GETURLS,
		FIND_MATCH_PING,
		FIND_GATHERING_BY_ID,

		// HOST MATCH
		HOST_CREATE_GATHERING,
		HOST_REGISTER_GATHERING,
		HOST_FETCH_URL,
		HOST_JOIN_GATHERING,
		HOST_LAUNCH_SESSION,

		// CREATE_ROUND
		CREATE_ROUND_START_SESSION,
		CREATE_ROUND_CREATE_ROUND,
		CREATE_ROUND_REGISTER_ROUND,
		CREATE_ROUND_COMPETITION,
		CREATE_ROUND_INVITE_PLAYERS,
		
		// JOIN_ROUND
		JOIN_ROUND,
		CHECK_FOR_CONNECTIVITY,

		// START_ROUND
		LAUNCH_ROUND,
		START_ROUND,

		// END_ROUND
		FINISH_ROUND,
		REPORT_STATS,
		REPORT_PERSONAL_STATS,
		END_ROUND_PARTICIPATION,

		// END_MATCH
		FINISH_MATCH,
		CANCEL_PARTICIPATION,
		UNREGISTER_MATCH,

		// Updating match options
		UPDATE_MATCH_OPTIONS,

		// Clans - creation
		CLAN_CREATE,

		CLAN_UPDATE_MY_INFO,

		CLAN_JOIN_BY_INVITATION,
		CLAN_DENY_INVITATION,
		CLAN_BLOCK_INVITATION,
		CLAN_SEND_INVITATION,
		CLAN_CANCEL_SENT_INVITATION,
		CLAN_INVITATION_LIST,
		CLAN_ID_LIST_FOR_PLAYER,
		CLAN_INFO,
		CLAN_MEMBERSHIP_LIST,
		CLAN_LEAVE,
		CLAN_NOTICE_LIST,
		CLAN_POST_NOTICE,
		CLAN_KICK_MEMBER,
		CLAN_DEMOTE_MEMBER,
		CLAN_PROMOTE_MEMBER,
		CLAN_TRANSFER_LEADERSHIP,

		// FRIENDS LIST
		FRIENDS_GET_LIST,
		FRIENDS_OFFER_FRIENDSHIP,
		FRIENDS_ACCEPT_FRIENDSHIP,
		FRIENDS_DENY_FRIENDSHIP,
		FRIENDS_BLOCK_FRIENDSHIP,
		FRIENDS_CANCEL_FRIENDSHIP_REQUEST,
		FRIENDS_END_FRIENDSHIP,

		// JOIN MATCH
		JOIN_REQUEST_PARTICIPATION,
		JOIN_REQUEST_SESSION_URLS,

		INVITE_PLAYER_TO_MATCH,
		INVITE_PLAYER_TO_MATCH_BY_NAME,
		GET_MATCH_INVITATIONS,

		// STATION_URL_LIST
		POPULATE_RV_LIST,

		// STATS
		STATS_GET_ALL_STATS,
		STATS_GET_TOP_PLAYERS,
		STATS_GET_LADDER_PLAYERS,
		STATS_GET_TOP_CLANS,

		FILTER_TEXT_FOR_PROFANITY,
		MESSAGE_OF_THE_DAY,
		UPDATE_GATHERING,
		ACCOUNT_INFO,
		WAIT_SPECIFIED_TIME,
	};

	///------------------------------------------------------------------
	/// Consctuction / Destruction
	///------------------------------------------------------------------
	OSIPS2();
	~OSIPS2();

	
	///------------------------------------------------------------------
	/// New / Delete overrides
	///------------------------------------------------------------------
	static void*					operator new( size_t size )								{ return OSImalloc( size ); }
	static void						operator delete( void* memPtr )							{ OSIfree( memPtr ); }


	// General Functions
	virtual bool					processLastRequest();
	virtual bool					start( void );
	virtual const OSIchar*			getErrorString()										{ return _errorString ; }
	void							setErrorString( OSIchar* errorString )					{ OSIstrcpy( _errorString, errorString ); }
	void							setRequestType( RequestType requestType );
	virtual bool					isLastErrorFatal();
	OSIchar*						GetOSIStringByEnum( OSIStringEnum string_id );
	bool							isOrphanSession( void );

	// Hardware Profiles
#ifndef DIRECTX_PC
	virtual OSIHardwareProfileID&	getDefaultHardwareProfile( bool loadFromMemoryCard );
	virtual bool					requestHardwareProfileActivation( OSIHardwareProfileID& profileID );
#endif

	// Login
	virtual bool					requestAccountInfo();
	virtual bool					requestCreateAccount( OSIAccount& account );
	virtual bool					requestDestroyAccount( OSIAccountID& accountID );
	virtual bool					requestLogin( OSIAccountID& accountID ) { return true; }
	virtual bool					requestLogin( const OSIchar* name, const OSIchar* password );
	virtual bool					requestLogout();
	virtual bool					isRequiredMessagePending();
	virtual bool					isRecommendedMessagePending();
	virtual void					launchExternalAccountMaintenanceApp();
	virtual bool					requestDNSLookup( const OSIchar* const hostName );

	// Edit Account
	virtual bool					requestEditAccount(const OSIchar *name, const OSIchar *password, OSIAccount& account);

	// Friends
	virtual bool					requestFriendList();
	virtual bool					requestAuthorizeTheirFriendship( OSIPlayerID& id );
	virtual bool					requestDenyTheirFriendship( OSIPlayerID& id );
	virtual bool					requestBlockTheirFriendship( OSIPlayerID& id );
	virtual bool					requestUnBlockTheirFriendship( OSIPlayerID& id );
	virtual bool					requestOfferMyFriendship( OSIPlayerID& id );
	virtual bool					requestOfferMyFriendship( const OSIchar* name );
	virtual bool					requestCancelMyFriendshipOffer( OSIPlayerID& id );
	virtual bool					requestPublishOnlineState( OSIPlayerState& state );
	virtual bool					requestEndFriendship( OSIPlayerID& id );
	virtual bool					requestMatchForFriend( OSIPlayerID& id );
	virtual void					updateInvitations();

	// Alerts
	virtual bool					isNewMatchInvitationWaiting();
	virtual bool					isNewClanInvitationWaiting();
	virtual bool					isNewFriendInvitationWaiting();
	
	// Players
	virtual bool					refreshPlayerList();
	virtual bool					requestSendPlayerFeedback( OSIPlayerID& id, OSIFeedback& feedback );
	virtual bool					requestMutePlayer( OSIPlayerID& id );

	// Ladder
	virtual bool					requestRankingList();
	virtual bool					requestStatsPlayerLadder( OSIPlayerID& pivotPlayer, const int numRanks );
	virtual bool					requestStatsTopPlayers( const int numRanks );
	virtual bool					requestStatsTopClans( int numRanks );
	
	// Rounds
	virtual bool					requestCreateRound();
	virtual bool					requestJoinRound();
	virtual bool					requestStartRound();
	virtual bool					requestEndRound( OSIRoundStats& stats );

	// Stats
	virtual bool					requestStatsPlayerList( OSIPlayerStatsList& players );

	// Voice
	virtual bool					requestOpenVoiceConnection( OSIPlayerID& id );
	virtual bool					requestCloseVoiceConnection( OSIPlayerID& id );
	virtual bool					startVoiceClipRecording();
	virtual bool					stopVoiceClipRecording();
	virtual bool					attachVoiceClipToNextGameInvitation();
	
	// Match Making
	void							addOpponent( qUnsignedInt32 gatheringID, qUnsignedInt32 principleID );
	virtual bool					requestValidateMatchName( const OSIchar* matchName );
	virtual bool					requestHostMatch( OSIMatchOptions& options );
	virtual bool					requestStationURLListForMatch( OSIMatchID &matchID );
	virtual bool					requestJoinMatch( OSIMatchID& matchID );
	virtual bool					requestMatchList( OSIMatchOptions& options );
	virtual bool					requestLadderMatch( OSIMatchOptions& options );
	virtual bool					requestInvitePlayerToMatch( OSIPlayerID& playerID );
	virtual bool					requestInvitePlayerToMatch( const OSIchar* const playerName );
	virtual bool					requestDeclineMatchInvite( OSIPlayerID& playerID );
	virtual bool					requestUpdateMatchOptions( OSIMatchOptions& options );
	virtual	bool					requestQuitMatch();
	virtual bool					requestMatchInvitations();
	virtual bool					isHost() { return _isHost; }
	virtual void					setIsHost( bool isHost ) { _isHost = isHost ; }
	virtual void					activePlayerWantingToQuit() { _isWantingToQuit = true; }

	// Match Lobby
	virtual bool					requestLaunchGame();
	virtual bool					requestLaunchChat();
	virtual bool					requestSendTextMessage( const OSIchar* message );
	virtual void					publishGlobalVoiceMute( bool muteAllVoice );
	virtual void					publishMyGameReadiness( bool readyToPlay );
	virtual bool					isOpponentReadyToPlayGame();
	virtual bool					requestHostMigration( OSIPlayerID& newHostID, int numPlayersInMatch );
	virtual bool					registerForHostMigration( void );
	virtual bool					unregisterForHostMigration( void );
	virtual bool					requestMessageOfTheDay( const OSIchar* language );

	virtual bool					requestFilterTextForProfanity( const OSIchar* textToFilter );

	// Notification of player network events
	virtual bool					playerJoinCurrentMatch( const OSIchar* playerName, OSIPlayerID& id, int numPlayersInMatch );
	virtual bool					playerLeaveCurrentMatch( const OSIchar* playerName, OSIPlayerID& id, int numPlayersInMatch );

#ifndef DIRECTX_PC
	// DNAS
    bool                            beginDNASAuth();
    int                             updateDNASAuth();
    int                             abortDNASAuth();
	void							cancelDNASAuth();
	void							pauseDNASAuth();
#endif // DIRECTX_PC

	// Clans
	virtual bool					requestClanCreate( const OSIchar* const tag, const OSIchar* const name );
	virtual bool					requestClanUpdateMyClanInformation();

	// Clans - invitation stuff
	virtual bool 					requestClanJoinByInvitation( const OSIClanInvitation& invitation );
	virtual bool 					requestClanDenyInvitation( const OSIClanInvitation& invitation );
	virtual bool 					requestClanBlockInvitation( const OSIClanInvitation& invitation );
	virtual bool					requestClanSendInvitation( const OSIClanID& clanID, const OSIchar* const playerName );
	virtual bool 					requestClanCancelSentInvitation( const OSIClanInvitation& invitation );
	virtual bool					requestClanInvitationList();

	// Clans - Info
	virtual bool					requestClanIDListForPlayer( const OSIPlayerID& playerID );
	virtual bool					requestClanInfoList( const OSIClanID& clanID );
	virtual bool					requestClanMembershipList( const OSIClanID& clanID );
	virtual bool					requestClanLeave( const OSIClanID& clanID );

	// Clans - Notices
	virtual bool					requestClanNoticeList( const OSIClanID& clanID );
	virtual bool					requestClanPostNotice( const OSIClanID& clanID, const OSIchar* const notice );

	// Clans - Administrative
	virtual bool					requestClanKickMember( const OSIClanID& clanID, const OSIPlayerID& playerID );
	virtual bool					requestClanDemoteMember( const OSIClanID& clanID, const OSIPlayerID& playerID );
	virtual bool					requestClanPromoteMember( const OSIClanID& clanID, const OSIPlayerID& playerID );
	virtual bool					requestClanTransferLeadership( const OSIClanID& clanID, const OSIPlayerID& playerID );

//#ifdef DIRECTX_PC
//	// CDKEY - set cdkey for OSI login
//	void							setCDKey(char* cdkey) { _cdkey = cdkey; }
//#endif

protected:
	bool							_processLastHardwareConnectionRequest( void );
	bool							_updateAccountInfo( void );
	void							_processAccountInfo( int successState, int failureState );
	bool							_processRendezVousCall( void );
	bool							_updateHostMatch( void );
	bool							_updateJoinMatch( void );
	bool							_processAcceptMatchInvite( void );
	bool							_processRendezVousUpdateMatchOptions( void );

	void							_checkForNewAlerts( void );
	bool							_checkForAlertCommandCompletionOrTimeout();

	void							_checkForGatheringUpdates( void );
	bool							_updateGatheringUpate( void );
	void							_processRefreshCurrentGathering( int successState, int failureState );
	void							_processUpdateCurrentGathering( int successState, int failureState );
	bool							_checkForGatheringUpdatesCommandCompletionOrTimeout( int failureState );
	bool							_isGatheringUpdateReadyForProcess( void );

	// List Population Functions
	void							_populateMatchList( void );
	void							_populateHardwareProfileList( void );
	void							_populateAccountList( void );
	virtual void					_populateFriendList( void );
	virtual void					_populateClanInvitationList();
	virtual void					_populateClanSentInvitationList();
	virtual void					_populateClanIDList();
	virtual void					_populateClanInfoList();
	virtual void					_populateClanMembershipList();
	virtual void					_populateClanNoticeList();
	virtual void					_populateStationURLList();

	// Hosting Functions
	void							_processCreateGathering( int successState, int failureState );
	void							_processRegisterGathering( int successState, int failureState );
	void							_processFetchURL( int successState, int failureState );
	void							_processJoinGathering( int successState, int failureState );
	void							_processLaunchSession( int successState, int failureState );

	bool							_updateFilterTextForProfanity( void );
	void							_processFilterTextForProfanity( int successState, int failureState );
	bool							_updateMessageOfTheDay( void );
	void							_processMessageOfTheDay( int successState, int failureState );

	// Find Match Functions
	bool							_processFindMatchRequest( void );
	void							_processFindMatchSearch( int successState, int failureState );
	void							_processFindMatchGetURLs( int successState, int failureState );
	void							_processFindMatchPing( int successState, int failureState );
	void							_processFindGatheringByID( int successState, int failureState );
	void							_populateRVMatches();
	void							_processGetMatchInvitations( int successState, int failureState );
	bool							_updateGetMatchInvitations( void );

	// Friends List Functions
	bool							_updateGetFriends( void );
	void							_processFriendsGetFriends( int successState, int failureState );
	bool							_processFriendsListRequestUpdateInvitations( void );
	bool							_processFriendsListWaitingOnUpdateInvitations( void );

	// Join Match Functions
	void							_processJoinRequestParticipation( int successState, int failureState );
	void							_processJoinRequestStationURLs( int successState, int failureState );
	void							_processCheckForConnectivity( int successState, int failureState );

	// Create Round Functions
	bool							_updateCreateRound( void );
	void							_processCreateRoundStartSession( int successState, int failureState );
	void							_processCreateRoundCreateRound( int successState, int failureState );
	void							_processCreateRoundRegisterRound( int successState, int failureState );
	void							_processCreateRoundCompetition( int successState, int failureState );
	void							_processCreateRoundJoinRound( int successState, int failureState );
	void							_processCreateRoundInvitePlayers( int successState, int failureState );

	// Edit Account functions
	bool							_processEditAccount( void );
	void							_processUpdatePassword( int successState, int failureState);

	// Join Round Functions
	bool							_updateJoinRound( void );
	void							_processJoinRound( int successState, int failureState );
	
	// Start Round Functions
	bool							_updateStartRound( void );
	void							_processLaunchRound( int successState, int failureState );
	void							_processStartRound( int successState, int failureState );

	// Quit Match
	bool							_updateEndMatch( void );
	void							_processFinishMatch( int successState, int failureState );
	void							_processUnregisterMatch( int successState, int failureState );
	void							_processCancelParticipation( int successState, int failureState );

	// End Round Functions
	bool							_updateEndRound( void );
	void							_processFinishRound( int successState, int failureState );
	void							_processReportStats( int successState, int failureState );
	void							_processReportPersonalStats( int successState, int failureState );
	void							_processEndRoundParticipation( int successState, int failureState );
	void							_gatherStats();

	// Login / Logout Functions
	bool							_updateLogin( void );
	void							_processLogin( int successState, int failureState );
	void							_processStartPeerManager( int successState, int failureState );
	void							_processStartNATClient( int successState, int failureState );
	void							_processStartAccountClient( int successState, int failureState );
	void							_processStartFriendsClient( int successState, int failureState );
	void							_processStartMatchMakingClient( int successState, int failureState );
	bool							_processFindRanking( int successState, int failureState, const String& matchName, GatheringHolder& rankingHolder );
	bool							_processCreateRanking( int successState, int failureState, const String& matchName, GatheringHolder& rankingHolder );
	bool							_processHostCompetition( int successState, int failureState, GatheringHolder& holder );
	void							_processUpdateStatus( int successState, int failureState );
	void							_processLogout( int successState, int failureState);
	void							_logout( void );
	bool							_updateDNSLookup( void );
	void							_processDNSLookup( int successState, int failureState );

//#ifdef DIRECTX_PC
//	void							_processCDKeyCheck( int successState, int failureState );
//#endif

	// Create Account Functions
	bool							_updateCreateAccount( void );
	void							_processCreateAccount( int successState, int failureState);
	void							_processPrepNewPlayer( int successState, int failureState);
	void							_processUpdateNewPlayerData( int successState, int failureState);
	void							_processUpdateNewPlayerEmail( int successState, int failureState);

	// Friendship Functions
	bool							_updateAuthorizeFriendship( void );
	bool							_updateDenyFriendship( void );
	bool							_updateBlockFriendship( void );
	bool							_updateUnblockFriendship( void );
	bool							_updateOfferFriendship( void );
	bool							_updateCancelFriendshipRequest( void );
	bool							_updateEndFriendship( void );

	// Player Stats
	bool							_updateGetPlayersStats( void );
	void							_populatePersonalStats( void );
	void							_processGetStatsForPlayers( int successState, int failureState );
	void							_populatePlayerStatsList( void );
	void							_populateTopPlayerStatsList( void );
	bool							_updateGetTopPlayersStats( void );
	void							_processGetTopPlayersStats( int successState, int failureState );
	bool							_updateGetLadderStats( void );
	void							_processGetLadderStats( int successState, int failureState );
	TTLStats*						_getRecievedStatsByPid( OSIPlayerID id );

	// Clan Stats
	void							_populateTopClansStatsList( void );
	bool							_updateGetTopClansStats( void );
	void							_processGetTopClansStats( int successState, int failureState );


	// Core utility functions
	bool							_updateInviteToMatch( void );
	void							_processFindPIDFromName( int successState, int failureState );
	void							_processInviteToMatch( int successState, int failureState );
	void							_resetTimeoutTimer( void ) ;
	bool							_checkForCommandCompletionOrTimeout( int& stateVariable, int failureState, ProtocolCallContext* context = 0 );
	void							_sessionToMatch( TTLSession& session, OSIMatch& match ); 
	void							_matchToSession( OSIMatch& match, TTLSession& session );
	void							_runFrame( void );
	bool							_generateSQLMatchQuery(OSIchar* buffer, int bufsize);
	void							_convertQuazalPlayerToOSIPlayer( const FriendData& quazalPlayer, OSIPlayer& osiPlayer, bool temp_hack_isPendingOutgoing );
	void							_appendQuazalPlayerListToOSIPlayerList( qList<FriendData>& quazalPlayerList, OSIPlayerList& osiPlayerList, bool temp_hack_isPendingOutgoing );
	void							_cleanupRendezVous(bool deleteRendezVous = true);
	bool							_updateUpdateGathering( void );
	void							_processUpdateGathering( int successState, int failureState );
	void							_processWaitSpecifiedTime( int successState, int failureState );

	bool							_updateStationURLListRequest( void );
	void							_processPopulateRVList( int successState, int failureState );
	void							_updateMatchOptions( int successState, int failureState );

	// Clans - creation
	bool							_updateClanCreate();
	void							_processClanCreate(int successState, int failureState);
	bool							_updateClanUpdateMyClanInformation();
	void							_processClanUpdateMyClanInformation(int successState, int failureState);
	// Clans - invitations
	bool							_updateClanJoinByInvitation();
	void							_processClanJoinByInvitation(int successState, int failureState);
	bool							_updateClanDenyInvitation();
	void							_processClanDenyInvitation(int successState, int failureState);
	bool							_updateClanBlockInvitation();
	void							_processClanBlockInvitation(int successState, int failureState);
	bool							_updateClanSendInvitation();
	void							_processClanSendInvitation(int successState, int failureState);
	bool							_updateClanCancelSentInvitation();
	void							_processClanCancelSentInvitation(int successState, int failureState);
	bool							_updateClanInvitationList();
	void							_processClanInvitationList(int successState, int failureState);
	// Clans - info
	bool							_updateClanIdListForPlayer();
	void							_processClanIdListForPlayer(int successState, int failureState);
	bool							_updateClanInfoList();
	void							_processClanInfoList(int successState, int failureState);
	bool							_updateClanMembershipList();
	void							_processClanMembershipList(int successState, int failureState);
	bool							_updateClanLeave();
	void							_processClanLeave(int successState, int failureState);
	// Clans - notices
	bool							_updateClanNoticeList();
	void							_processClanNoticeList(int successState, int failureState);
	bool							_updateClanPostNotice();
	void							_processClanPostNotice(int successState, int failureState);
	// Clans - administration
	bool							_updateClanKickMember();
	void							_processClanKickMember(int successState, int failureState);
	bool							_updateClanDemoteMember();
	void							_processClanDemoteMember(int successState, int failureState);
	bool							_updateClanPromoteMember();
	void							_processClanPromoteMember(int successState, int failureState);
	bool							_updateClanTransferLeadership();
	void							_processClanTransferLeadership(int successState, int failureState);

	// Clans - helper functions
	void							_checkClanCallCompletion(int successState, int failureState);
	OSIClanRole						_getOSIClanRoleFromRendezVous(bool isLeader, bool isOfficer);
	void							_setActiveAccountPlayerClanInfo();

	// Host Migration Helper Functions
	void							_registerGatheringForHostMigration( GatheringID gID );
	void							_unregisterGatheringForHostMigration( GatheringID gID );

	void							_moveCurrentPlayersToRecentPlayers();
private:

	// Account Management
	ProtocolCallContext*			_currentContext;
	BackEndServices*				_rendezVous ;
	Credentials*					_credentials;
	AccountManagementClient*		_accountManagementClient ;
	CompetitionClient*				_competitionClient;
	MatchMakingClient*				_matchMakingClient;
	FriendsClient*					_friendsClient;
	NATTraversalClient*				_NATTraversalClient ;
	qResult							_lastCallQResult;
	OSIAccount						_guestAccount ;
	OSIAccount						_newAccount ;
	OSIchar							_newAccountEmail[ OSI_EMAIL_LENGTH ] ;

	// Peer System (for pings)
	PeerService*					_peerService;
	PeerManager*					_peerManager;

	// State Variables
	int								_state ;

	// Match Making / Gathering related
	GatheringHolder					_currentGathering ;
	GatheringHolder					_currentRound ;
	GatheringHolder					_nonLadderCompetition;
	qList<GatheringHolder>			_tempGatherings;
	qList<GatheringHolder>			_lstGatherings;
	qList<GatheringHolder>			_lstInvitations;
	qList<GatheringHolder>			_lstRankings;
	qList<GatheringHolder>::iterator _gatheringIter ;
	qList<StationURL>				_currentStationURLs ;
	qList<EidosFriend>				_lstFriends ;
	qList<StationURL>				_lstStationURLs;
	qList<BasicAccountInfo>			_lstAccounts;
	TTLMatchMakingClient*			_TTLMatchMakingClient;
	GatheringHolder					_nonladderRanking ;
	OSIMatchOptions					_matchOptions ;
	qUnsignedInt32					_roundID ;
	Chrono							_pingChrono;
	Chrono							_chrono;
	Chrono							_waitChrono;
	int								_timeToWaitFor;
	static unsigned int				_opponentsInRound ;
	GatheringID						_matchToJoinID ;
	HostMigrationCallback*			_hostMigrationCallback ;


	// Stats related
	OSIRoundStats					_roundStats;
	qList<String>					_playerNameList;
	qList<GatheringStats>			_lstStats;
	qList<float>					_lstPersonalStats;
	qList<TTLStats>					_retrievedStatsList;
	int								_rankCount;
	OSIPlayerID						_pivotID;
	qList<EidosClanProps>			_retrievedClansList;


	// Clan related
	OSIchar							_newClanTag[OSI_CLAN_TAG_LENGTH*2];			///< Storage for clan tag when creating a new clan
	OSIchar							_newClanName[OSI_CLAN_NAME_LENGTH*2];		///< Storage for clan name when creating a new clan
	qList<OSIu32>					_lstClanID;									///< RendezVous representation of clan ID list
	qList<EidosClanProps>			_lstClans;									///< RendezVous representation of clan list
	qList<EidosClanInvite>			_lstClanInvitesToMe;						///< RendezVous representation of clan invites sent to me
	qList<EidosClanInvite>			_lstClanInvitesFromMe;						///< RendezVous representation of clan invites I have sent
	qList<EidosClanMember>			_lstClanMembers;							///< RendezVous representation of my clan membership list
	qList<EidosClanNotice>			_lstClanNotices;							///< RendezVous representation of my clan notices

	unsigned int					_lastClanCallResult;						///< Last call result to RendezVous for clans, returns error code, 0 being success, all else failure
	OSIClanID						_currentClanID;								///< Current clan ID being used by active request
	OSIPlayerID						_currentPlayerID;							///< Current player ID being used by active request
	OSIClanInvitation				_currentClanInvitation;						///< Current clan invitation being used by active request
	EidosClanProps					_currentClan;								///< Current clan info used by RendezVous
	EidosClanMember					_currentClanMember;							///< Current clan member used by RendezVous
	OSIchar							_currentRequestStr[OSI_STRING_LENGTH_HUGE]; ///< Current string being used by a request

	DNSResolutionThread*			_dnsResolutionThread;
	OSIchar							_dnsResolvedIP[ 256 ];

	// Other
#ifndef DIRECTX_PC
	OSIHardwareProfileID			_defaultProfile ;
	PS2ProfileList					_oProfileList;

	bool							_dnasPaused;
#endif // DIRECTX_PC

	// cdkey
//#ifdef DIRECTX_PC
//	String							_cdkey;
//#endif

	OSIchar							_errorString[ OSI_STRING_LENGTH_HUGE ] ;
	RequestType						_requestType ;
	Time							_nextUpdateTime;
	qList<FriendData>				_quazalFriendList;
	OSIPlayerList					_friendAccumulateList;
	OSIPS2NotificationEventHandler*	_notificationEventHandler ;
	int								_timeoutTimer ;
	bool							_sessionStarted;
	bool							_commandInitiated ;
	bool							_isHost ;
	bool							_isWantingToQuit;
	bool							_isCompetitionRound;
	bool							_lastCallResult ;
	bool							_requestingHardwareConnection ;
	String							_motd;

	// Alerts
	ProtocolCallContext*			_alertContext;
	Chrono							_alertChrono;
	Chrono							_alertTimeoutChrono;
	int								_alertCheckTime;
	bool							_alertCommandInitiated;
	OSIPS2Alerts					_alerts;

	// Update Gathering info
	ProtocolCallContext*			_gatheringUpdateContext;
	Chrono							_gatheringUpdateTimeoutChrono;
	bool							_isGatheringUpdateReady;
	bool							_gatheringUpdateCommandInitiated;
	bool							_gatheringUpdateCallResult;
	int								_gatheringState;
	unsigned int					_totalConnectedPlayers;

	qList<RVMatch>					_lstRVMatches;
	qList<GatheringID>				_lstGatheringIDs ;

	EidosAccountPublic				_publicAccountData;
	EidosAccountPrivate				_privateAccountData;

	Quazal::CustomDataHolder<Quazal::EidosAccountPublic> _newAccountPublicData;
	Quazal::CustomDataHolder<Quazal::EidosAccountPrivate> _newAccountPrivateData;

	friend class OSIPS2NotificationEventHandler ;
};





#endif /* _OSIPS2_HPP_ */




