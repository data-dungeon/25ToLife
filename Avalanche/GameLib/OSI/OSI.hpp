#ifndef _INCLUDED_OSI_
#define _INCLUDED_OSI_

// forward declaration
class OSI ;

#include "OSIPlatformTypes.h"
#include "OSIBaseList.hpp"
#include "OSIHardwareProfile.hpp"
#include "OSIHardwareProfileList.hpp"
#include "OSIPlayer.hpp"
#include "OSIPlayerList.hpp"
#include "OSIMatch.hpp"
#include "OSIMatchList.hpp"
#include "OSIAccount.hpp"
#include "OSIAccountList.hpp"
#include "OSIStringList.hpp"
#include "OSIStringTable.hpp"
#include "OSIPlayerStats.hpp"
#include "OSIGameConfig.hpp"
#include "OSIClans.hpp"
#include "OSIClanIDList.hpp"
#include "OSIPlayerIDList.hpp"
#include "OSIClanInfoList.hpp"
#include "OSIClanMembershipList.hpp"
#include "OSIClanInvitationList.hpp"
#include "OSIClanNoticeList.hpp"
#include "OSIRoundStats.hpp"

typedef unsigned int OSIPlayerSortType ;

enum OSIRequestStatus
{
	STATUS_WORKING,
	STATUS_SUCCEEDED,
	STATUS_FAILED,
};

#define REQUEST_SUCCEEDED		true
#define REQUEST_FAILED			false

#define PLAYER_HOSTER		0
#define PLAYER_JOINER		1

const int OSI_DEFAULT_TIMEOUT = 16000; // milliseconds to wait before timing out on all OSI requests

typedef enum 
{
	NEG_FEEDBACK_HARASSMENT,
	NEG_FEEDBACK_LEWDNESS,
	NEG_FEEDBACK_GAMEPLAY,
	NEG_FEEDBACK_NICKNAME,
	POS_FEEDBACK_ATTITUDE,
	POS_FEEDBACK_SESSION
} OSIFeedback ;


class OSI
{
public:
	OSI();
	virtual ~OSI();

	virtual bool					isConnected()									{ return _isConnected ; }
	virtual bool					isOrphanSession( void )							{ return false ; }

	static inline void				runFrame()			{ if ( _theOSI ) _theOSI->_runFrame(); }
	static inline OSI*				getSingletonPtr()	{ return _theOSI; }
	static void						createOSI( int memoryBlock );
	static bool						isCreated( void )	{ return ( _theOSI != NULL ); }
	static void						destroyOSI();

	virtual void					updateInvitations()		= 0;

	virtual bool					processLastRequest()	= 0;
	virtual bool					isLastErrorFatal( void ) = 0;
	OSIRequestStatus				getLastRequestStatus();
	void							setLastRequestStatus( OSIRequestStatus requestStatus )	{ _requestStatus = requestStatus ; }
	virtual const OSIchar*			getErrorString()		= 0;
	OSIStringEnum					getErrorCode()			{ return _errorCode ; }

	// State Accessors
	OSIStringEnum					getActiveState()						{ return _activeState; }
//	OSIchar*						getActiveStateString()					{ return GetOSIStringByEnum(_activeState); }
	void							setActiveState(OSIStringEnum state)		{ _activeState = state; }

	// Hardware Profiles
#ifndef DIRECTX_PC
	OSIHardwareProfileList&			getHardwareProfileList();
	virtual OSIHardwareProfileID&	getDefaultHardwareProfile( bool loadFromMemoryCard ) = 0;
	virtual bool					requestHardwareProfileActivation( OSIHardwareProfileID& profileID ) = 0;
#endif

	// Login
	virtual bool					requestAccountInfo() = 0;
	inline OSIAccount*				getActiveAccount()	{ return _activeAccount; }
	inline OSIPlayer*				getActivePlayer()	{ return ((_activeAccount != NULL) ? (&_activeAccount->player) : (NULL)); }
	OSIAccount&						getAccountInfo();
	virtual bool					requestCreateAccount( OSIAccount& account ) = 0;
	virtual bool					requestEditAccount(const OSIchar *name, const OSIchar *password, OSIAccount& account) = 0;
	virtual bool					requestDestroyAccount( OSIAccountID& accountID ) = 0;
	virtual bool					requestLogin( OSIAccountID& accountID ) = 0;
	virtual bool					requestLogin( const OSIchar* name, const OSIchar* password ) = 0;
	virtual bool					requestLogout() = 0;
	virtual bool					isRequiredMessagePending() = 0;
	virtual bool					isRecommendedMessagePending() = 0;
	virtual void					launchExternalAccountMaintenanceApp() = 0;
	virtual bool					requestDNSLookup( const OSIchar* const hostName ) { return true; }

	// Friends
	virtual bool					requestFriendList() = 0;
	OSIPlayerList&					getFriendList();
	virtual bool					requestAuthorizeTheirFriendship( OSIPlayerID& id ) = 0;
	virtual bool					requestDenyTheirFriendship( OSIPlayerID& id ) = 0;
	virtual bool					requestBlockTheirFriendship( OSIPlayerID& id ) = 0;
	virtual bool					requestUnBlockTheirFriendship( OSIPlayerID& id ) = 0;
	virtual bool					requestOfferMyFriendship( OSIPlayerID& id ) = 0; // OSIFIXME: should consider renaming this, since friendship is nonmutual on ps2
	virtual bool					requestOfferMyFriendship( const OSIchar* name ) = 0; // OSIFIXME: should consider renaming this, since friendship is nonmutual on ps2
	virtual bool					requestCancelMyFriendshipOffer( OSIPlayerID& id ) = 0;
	virtual bool					requestPublishOnlineState( OSIPlayerState& state ) = 0;
	virtual bool					requestEndFriendship( OSIPlayerID& id ) = 0;
	virtual bool					requestMatchForFriend( OSIPlayerID& id ) = 0;

	// Alerts
	virtual bool					isNewMatchInvitationWaiting() = 0;
	virtual bool					isNewClanInvitationWaiting() { return false; }
	virtual bool					isNewFriendInvitationWaiting() { return false; }
	
	// Players
	OSIPlayerList&					getRecentPlayerList();
	OSIPlayerList&					getCurrentPlayerList();
	virtual bool					requestSendPlayerFeedback( OSIPlayerID& id, OSIFeedback& feedback ) = 0;
	virtual bool					requestMutePlayer( OSIPlayerID& id ) = 0;
	bool							isPlayerInMatch( OSIPlayer* player );
	bool							isPlayerInMatch( const OSIchar* const playerName );

	// Rounds
	virtual bool					requestCreateRound() = 0;
	virtual bool					requestJoinRound() = 0;
	virtual bool					requestStartRound() = 0;
	virtual bool					requestEndRound( OSIRoundStats& stats ) = 0;
	void							setRoundID( const OSIRoundID& roundID )			{ _currentRoundID = roundID; }
	OSIRoundID						getRoundID()							const	{ return _currentRoundID; }

	// Ladder & Stats
	virtual bool					requestStatsPlayerList( OSIPlayerStatsList& players ) = 0;
	virtual bool					requestStatsPlayerLadder( OSIPlayerID& pivotPlayer, const int numRanks ) = 0;
	virtual bool					requestStatsTopPlayers( const int numRanks ) = 0;
	OSIPlayerStatsList&				getStatsPlayerList();

	virtual bool					requestStatsTopClans( int numClans ) = 0;
	OSIClanInfoList&				getStatsClanList();

	// Voice
	virtual bool					requestOpenVoiceConnection( OSIPlayerID& id ) = 0;
	virtual bool					requestCloseVoiceConnection( OSIPlayerID& id ) = 0;
	virtual bool					startVoiceClipRecording() = 0;
	virtual bool					stopVoiceClipRecording() = 0;
	virtual bool					attachVoiceClipToNextGameInvitation() = 0;
	
	// Matchmaking
	virtual bool					requestValidateMatchName( const OSIchar* matchName ) = 0;
	virtual bool					requestHostMatch( OSIMatchOptions& options ) = 0;
	OSIMatch*						getCurrentMatch();
	virtual bool					requestStationURLListForMatch( OSIMatchID& matchID ) = 0;
	OSIStringList&					getStationURLList();
	virtual bool					requestJoinMatch( OSIMatchID& matchID ) = 0;
	virtual bool					requestMatchList( OSIMatchOptions& options ) = 0;
	OSIMatchList&					getMatchList();
	virtual bool					requestLadderMatch( OSIMatchOptions& options ) = 0;
	virtual bool					requestInvitePlayerToMatch( OSIPlayerID& playerID ) = 0;
	virtual bool					requestInvitePlayerToMatch( const OSIchar* const playerName ) { return false; }
	virtual bool					requestDeclineMatchInvite( OSIPlayerID& playerID ) = 0;
	virtual bool					requestUpdateMatchOptions( OSIMatchOptions& options ) = 0;
	virtual bool					requestMatchInvitations() = 0;
	virtual bool					isHost() { return false; }
	virtual void					setIsHost( bool isHost ) {}
	virtual void					activePlayerWantingToQuit() {}

	// Match Lobby
	virtual bool					requestLaunchGame() = 0;
	virtual bool					requestLaunchChat() = 0;
	virtual bool					requestSendTextMessage( const OSIchar* message ) = 0;
	virtual void					publishGlobalVoiceMute( bool muteAllVoice ) = 0;
	virtual void					publishMyGameReadiness( bool readyToPlay ) = 0;
	virtual bool					isOpponentReadyToPlayGame() = 0;
	virtual	bool					requestQuitMatch() = 0;
	const OSIchar*					getNextPendingTextMessage();
	virtual bool					requestHostMigration( OSIPlayerID& newHostID, int numPlayersInMatch ) = 0;
	virtual bool					registerForHostMigration( void )					{ return true ; }
	virtual bool					unregisterForHostMigration( void )					{ return true ; }
	virtual bool					requestMessageOfTheDay( const OSIchar* language )	{ return true; }
	const OSIchar*					getMessageOfTheDay() const { return _messageOfTheDay; }

//	virtual void					updatePlayerList() = 0;
	static int						_memoryBlock ; ///< The ID of the memory pool that OSImalloc should allocate from.

	virtual bool					requestFilterTextForProfanity( const OSIchar* textToFilter ) { return true; }

	// Notification of player network events
	virtual bool					playerJoinCurrentMatch( const OSIchar* playerName, OSIPlayerID& id, int numPlayersInMatch ) = 0;
	virtual bool					playerLeaveCurrentMatch( const OSIchar* playerName, OSIPlayerID& id, int numPlayersInMatch ) = 0;

	// Clans
	virtual bool					requestClanCreate( const OSIchar* const tag, const OSIchar* const name ) = 0;
	virtual bool					requestClanUpdateMyClanInformation() = 0;

	// Clans - invitation stuff
	virtual bool 					requestClanJoinByInvitation( const OSIClanInvitation& invitation ) = 0;
	virtual bool 					requestClanDenyInvitation( const OSIClanInvitation& invitation ) = 0;
	virtual bool 					requestClanBlockInvitation( const OSIClanInvitation& invitation ) = 0;
	virtual bool					requestClanSendInvitation( const OSIClanID& clanID, const OSIchar* const playerName ) = 0;
	virtual bool					requestClanCancelSentInvitation( const OSIClanInvitation& invitation ) = 0;

	virtual bool					requestClanInvitationList() = 0;
	OSIClanInvitationList&			getClanInvitationList();
	OSIClanInvitationList&			getClanSentInvitationList();

	// Clans - Info
	virtual bool					requestClanIDListForPlayer( const OSIPlayerID& playerID ) = 0;
	OSIClanIDList&					getClanIDList();

	virtual bool					requestClanInfoList( const OSIClanID& clanID ) = 0;
	OSIClanInfoList&				getClanInfoList();

	virtual bool					requestClanMembershipList( const OSIClanID& clanID ) = 0;
	OSIClanMembershipList&			getClanMembershipList();

	virtual bool					requestClanLeave( const OSIClanID& clanID ) = 0;

	// Clans - Notices
	virtual bool					requestClanNoticeList( const OSIClanID& clanID ) = 0;
	OSIClanNoticeList&				getClanNoticeList();

	virtual bool					requestClanPostNotice( const OSIClanID& clanID, const OSIchar* const notice ) = 0;

	// Clans - Administrative
	virtual bool					requestClanKickMember( const OSIClanID& clanID, const OSIPlayerID& playerID ) = 0;
	virtual bool					requestClanDemoteMember( const OSIClanID& clanID, const OSIPlayerID& playerID ) = 0;
	virtual bool					requestClanPromoteMember( const OSIClanID& clanID, const OSIPlayerID& playerID ) = 0;
	virtual bool					requestClanTransferLeadership( const OSIClanID& clanID, const OSIPlayerID& playerID ) = 0;

//#ifdef DIRECTX_PC
//	// sets cdkey for osi login
//	virtual void					setCDKey(char* cdkey) = 0;
//#endif


protected:
	virtual void					_runFrame( void ) = 0;
	virtual void					_populateFriendList( void ) = 0 ;
	virtual void					_populateAccountList( void ) = 0 ;
	virtual void					_populateHardwareProfileList( void ) = 0 ;
	virtual void					_populateMatchList( void ) = 0 ;
	virtual void					_populateClanInvitationList() = 0;
	virtual void					_populateClanSentInvitationList() = 0;
	virtual void					_populateClanIDList() = 0;
	virtual void					_populateClanInfoList() = 0;
	virtual void					_populateClanMembershipList() = 0;
	virtual void					_populateClanNoticeList() = 0;
	virtual void					_populateStationURLList() = 0;

	void							_addRecentPlayer( OSIPlayer& player );
	bool							_removeRecentPlayer( OSIPlayer& player );
	void							_addCurrentPlayer( OSIPlayer& player );
	bool							_removeCurrentPlayer( OSIPlayer& player );

	OSIPlayerList					_friendList;				///< request/getFriendList
	OSIPlayerList					_currentPlayerList ;
	OSIPlayerList					_recentPlayerList ;
	OSIPlayerStatsList				_statsListPlayers ;
	OSIClanInfoList					_statsListClans;
	OSIAccountList					_accountList;				///< request/getAccountInfo
	OSIAccount						_currentAccount;			///< request/getAccountInfo
	OSIHardwareProfileList			_hardwareProfileList;		///< getHardwareProfileList
	OSIMatchList					_matchList;					///< requestMatchList, requestMatchForFriend, requestLadderMatch, getMatchList
	OSIClanInvitationList			_clanInvitationList;
	OSIClanInvitationList			_clanSentInvitationList;
	OSIClanIDList					_clanIDList;
	OSIClanInfoList					_clanInfoList;
	OSIClanMembershipList			_clanMembershipList;
	OSIClanNoticeList				_clanNoticeList;
	OSIStringList					_stationURLList ;
	OSIAccount*						_activeAccount ;
	OSIRequestStatus				_requestStatus ;
	OSIchar							_messageOfTheDay[ OSI_STRING_LENGTH_LONG ];
	bool							_isConnected ;
	int								_defaultTimeout;
	const int						_defaultTimeSlice;
	OSIMatch						_currentMatch;
	OSIRoundID						_currentRoundID;
	OSIStringEnum					_activeState;
	OSIStringEnum					_errorCode;
	static OSI*						_theOSI ;
};

#endif // _INCLUDED_OSI_
