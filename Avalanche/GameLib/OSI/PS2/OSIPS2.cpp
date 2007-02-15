//-----------------------------------------------------------------
// OSIPS2.cpp -- Implementation of OSIPS2 class.
//-----------------------------------------------------------------

#include "OSIPS2.hpp"
#include <stdio.h>
#include <netz.h>
#include <rendezvous.h>
#include <Extensions/NATTraversal/NATTraversalExtension.h>

#ifndef DIRECTX_PC
#include <platform/stack/ps2common/ps2networking.h>
#include <platform/stack/ps2common/ps2module.h>
#include <platform/stack/ps2common/ps2directorypath.h>
#endif // DIRECTX_PC

#include <services/scriptengineplugins/python/pythonerrors.h>

#ifndef DIRECTX_PC
#include <libsn.h>
#include <sifdev.h>
#include <eekernel.h>
#include <dnas2.h>
#include <libinsck.h>
#endif // DIRECTX_PC

#include "platform/BaseType.h"
#include "Layers/OSThreading/OSThread.h"

#ifndef DIRECTX_PC
#include "platform/PS2/CDllLoader.h"
#endif // DIRECTX_PC

#include "platform/wantpal.h"
#ifndef DIRECTX_PC
#include "OSIPlayerStats.hpp"
#else
#include "osi/OSIPlayerStats.hpp"
#endif // DIRECTX_PC

#include "GameHelper/DebugOutput.h"
#include "GameHelper/Translator.h"

#ifndef DIRECTX_PC
#include "../PS2/do_netstart_dll.h"
#define DNAS_DLL (g_regionUSA ? "dnasdll.rel" : "dnasdllPAL.rel")

// These are the default values for finding IOP modules and the
// network configuration database that holds the various connection
// profiles.  You should change these to match your SCE installation
// or CDDVD file layout.
#if defined CD_BUILD || defined DVD_BUILD
const PS2DirectoryPath 	oModulePath(PS2DirectoryPath::CDDVDDevice,_T(""));
const PS2DirectoryPath 	oNetConfigPath(PS2DirectoryPath::CDDVDDevice,_T("cnf/"));
#else
const PS2DirectoryPath 	oModulePath(PS2DirectoryPath::HostDevice,_T("/usr/local/sce/iop/modules/"));
const PS2DirectoryPath 	oNetConfigPath(PS2DirectoryPath::HostDevice,_T("/usr/local/sce/conf/net/"));
#endif

#endif // DIRECTX_PC


#define GUEST_PLAYER_NAME _T("Guest")
#define GUEST_PLAYER_PASSWORD _T("h7fyctiuucf")

OSIchar undefinedString[] = OSI_STRING_LITERAL( "UNDEFINED TEXT" );

#if defined(CONSUMER_BUILD) && !defined(DIRECTX_PC) //CHANGEME
char			g_rendez_vous_server[256] = _T("ttllivepc.quazal.net");
unsigned int	g_rendez_vous_port = 30125;
#else
char			g_rendez_vous_server[256] = _T("ttlpc.quazal.net");
unsigned int	g_rendez_vous_port = 10150;
#endif

unsigned int	OSIPS2::_opponentsInRound = 0 ;

const int RENDEZVOUS_DISPATCH_TIME_SLICE = 50; // time slice in milliseconds for rendezvous scheduler to do work
const int ALERT_CHECK_TIME = 60000; // time interval in milliseconds when OSI checks for new alerts
const int SONY_PORT_NUMBER = 4659 ; // This is an official Sony port number for online lobbys

///---------------------------------------------------------
/// Returns status string in the form:
///		<title>|<platform>|<region>[|<other>]
/// FIXME: what about the PC? need to find a good place for
///        this function and it's probably not here
///---------------------------------------------------------
const OSIchar* GetStatusString()
{
	// FIXME: find out specifics about platform and region and construct
	//        this string appropriately
#ifdef PS2
	// TODO: figure out what region we are in
	// if( !PAL )
	return "25ToLife|PS2|EFIGS-NTSC";
	// else
	// return "25ToLife|PS2|EFIGS-PAL";
#else
	return "25ToLife|PC|ALL";
#endif
}

bool IsOnlineFromStatusString(const OSIchar* status)
{
	// If the string does not say "Offline", then as far as
	// we are concerned, they are online
	const OSIchar offlineStatus[] = "Offline";
	if(OSIstrncmp(status, offlineStatus, sizeof(offlineStatus)) == 0) {
		return false;
	}

	return true;
}

///---------------------------------------------------------
/// \brief Encode an extended ASCII string into UTF-8
///
/// UTF-8 is defined by the Unicode Standard [UNICODE].  Descriptions and
/// formulae can also be found in Annex D of ISO/IEC 10646-1 [ISO.10646]
///
/// In UTF-8, characters from the U+0000..U+10FFFF range (the UTF-16
/// accessible range) are encoded using sequences of 1 to 4 octets.  The
/// only octet of a "sequence" of one has the higher-order bit set to 0,
/// the remaining 7 bits being used to encode the character number.  In a
/// sequence of n octets, n>1, the initial octet has the n higher-order
/// bits set to 1, followed by a bit set to 0.  The remaining bit(s) of
/// that octet contain bits from the number of the character to be
/// encoded.  The following octet(s) all have the higher-order bit set to
/// 1 and the following bit set to 0, leaving 6 bits in each to contain
/// bits from the character to be encoded.
///
/// The table below summarizes the format of these different octet types.
/// The letter x indicates bits available for encoding bits of the
/// character number.
///
/// Char. number range  |        UTF-8 octet sequence
///    (hexadecimal)    |              (binary)
/// --------------------+---------------------------------------------
/// 0000 0000-0000 007F | 0xxxxxxx
/// 0000 0080-0000 07FF | 110xxxxx 10xxxxxx
/// 0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
/// 0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
///
/// Encoding a character to UTF-8 proceeds as follows:
///
/// 1.  Determine the number of octets required from the character number
///     and the first column of the table above.  It is important to note
///     that the rows of the table are mutually exclusive, i.e., there is
///     only one valid way to encode a given character.
///
/// 2.  Prepare the high-order bits of the octets as per the second
///     column of the table.
///
/// 3.  Fill in the bits marked x from the bits of the character number,
///     expressed in binary.  Start by putting the lowest-order bit of
///     the character number in the lowest-order position of the last
///     octet of the sequence, then put the next higher-order bit of the
///     character number in the next higher-order position of that octet,
///     etc.  When the x bits of the last octet are filled in, move on to
///     the next to last octet, then to the preceding one, etc. until all
///     x bits are filled in.
///
/// \param outEncodedStr String to fill up with UTF-8 (Could need to be twice as long as string to encode)
/// \param strToEncode String to encode in UTF-8
/// \param maxLength Maximum length to traverse string (to prevent buffer overflow)
///---------------------------------------------------------
char* UTF8Encode( char* outEncodedStr, const char* strToEncode, size_t maxLength )
{
	ASSERT( outEncodedStr != NULL );
	if( strToEncode == NULL || outEncodedStr == NULL )
	{
		if( outEncodedStr != NULL ) outEncodedStr[0] = 0;
		return outEncodedStr;
	}
	ASSERT( outEncodedStr != strToEncode );
	// Set up cursors to iterate over strings
	const char* cursor = strToEncode;
	char* encCursor = outEncodedStr;

	// Spit out the UTF8 version of each character
	int i = 0;
	for( ; i < maxLength && *cursor; cursor++ )
	{
		unsigned char c = *cursor;
		if( c < 0x7F )
		{
			// Normal ASCII goes straight across
			*encCursor = *cursor;
			encCursor++; ++i;
		}
		else
		{
			// Rest gets encoded into two bytes
			// right byte is 10 followed by the last six bits in the char
			unsigned char rightByte = 0x80 | (c & 0x3f);
			// left byte is 110 followed by remaining bits of char
			unsigned char leftByte = 0xC0 | ((c & 0xC0) >> 6);
			*encCursor = leftByte;
			encCursor++; ++i;
			*encCursor = rightByte;
			encCursor++; ++i;
		}
	}
	if( i == maxLength ) --encCursor;
	*encCursor = 0;

	return outEncodedStr;
}

///---------------------------------------------------------
/// \brief Decode a UTF-8 string to extended ASCII
///
/// NOTE: this function makes an assumption that the UTF-8
/// string is only from an extended ASCII string and not from
/// a unicode string
///
/// \param outDecodedStr String to populate with decoded info
/// \param strToDecode String to decode (neeeds to be UTF-8 limited to two bytes)
/// \param maxLength Maximum length to traverse string (to prevent buffer overflow)
///
/// \return True on succesfully decoding, false otherwise
///---------------------------------------------------------
bool UTF8Decode( char* outDecodedStr, const char* strToDecode, size_t maxLength )
{
	ASSERT( outDecodedStr != NULL );
	if( strToDecode == NULL || outDecodedStr == NULL )
	{
		if( outDecodedStr != NULL ) outDecodedStr[0] = 0;
		return outDecodedStr;
	}
	ASSERT( outDecodedStr != strToDecode );
	const char* cursor = strToDecode;
	char* decCursor = outDecodedStr;

	int i = 0;
	for( ; i < maxLength && *cursor; ++i )
	{
		unsigned char c = *cursor;
		// If most significant bit is set, it's encoded
		if( c & 0x80 )
		{
			// There should be a 1 and a 0 after the MSB
			if( !(c & 0x40) || (c & 0x20) )
			{
				*decCursor = 0;
				return false;
			}
			unsigned char leftByte = *cursor;
			cursor++;
			unsigned char rightByte = *cursor;
			unsigned char decodedLeft = (leftByte & 0x03) << 6;
			unsigned char decodedRight = rightByte & 0x3f;
			*decCursor = decodedLeft | decodedRight;
		}
		else
		{
			// straight ASCII, copy over
			*decCursor = *cursor;
		}
		decCursor++;
		cursor++;
	}
	if( i == maxLength ) --decCursor;
	*decCursor = 0;
	return true;
}


// This needs to stay in sync with the friend errors in TTLMatchMaking.py
namespace Quazal
{
	namespace PythonCoreErrorCode
	{
		enum ErrorCode2
		{
			FriendInvitePlayerSelf = 101,
			FriendInvitePlayerName = 102,
			FriendInviteLocalLimit = 103,
			FriendInviteOtherLimit = 104
		};
	}
}

void ResetAndStartTimer( Chrono &timer )
{
	timer.Reset();
	timer.Start();
}

// Thread stuff for resolving DNS name to an IP address to solve a blocking
// issue with sony gethostbyname since OSI runs in the main thread.
class DNSResolutionThread : public OSThread
{
public:
	void SetHostDNSName( const char* hostName ) { OSIstrncpy( m_szMachineName, hostName, NUM_ARRAY_ELEMENTS( m_szMachineName ) ); }
	const char* GetIPAddress() const { return m_szNumericAddress; }

	bool IsResolutionDone() const { return m_bResolutionDone; }
	bool DidDNSResolve() const { return m_szNumericAddress[ 0 ] != 0; }
protected:
	/*virtual*/ void Startup() {}
	/*virtual*/ bool Run();
	/*virtual*/ void Cleanup() {}
private:
	qAnsiChar	m_szMachineName[128];
	qAnsiChar	m_szNumericAddress[16]; // 255.255.255.255
	qBool		m_bResolutionDone;	
};

bool DNSResolutionThread::Run()
{
	struct hostent *pHostEntry;
	pHostEntry = gethostbyname( m_szMachineName );
	if( pHostEntry == NULL )
	{
		// Failed to resolved
		m_szNumericAddress[0] = '\0';
	}
	else
	{
		struct sockaddr_in oSaddr;
		oSaddr.sin_addr.s_addr = *(unsigned int*)*pHostEntry->h_addr_list;
		strcpy( m_szNumericAddress, inet_ntoa(oSaddr.sin_addr) );
	}
	m_bResolutionDone = true;
	return false;
}


/// ****************************************************************************
///	 __  __ _              _ _
///	|  \/  (_)___  ___ ___| | | __ _ _ __   ___  ___  _   _ ___
///	| |\/| | / __|/ __/ _ \ | |/ _` | '_ \ / _ \/ _ \| | | / __|
///	| |  | | \__ \ (_|  __/ | | (_| | | | |  __/ (_) | |_| \__ \
///	|_|  |_|_|___/\___\___|_|_|\__,_|_| |_|\___|\___/ \__,_|___/
///
/// ****************************************************************************

///---------------------------------------------------------
/// Constructor
///---------------------------------------------------------
OSIPS2::OSIPS2()
	: _sessionStarted( false ),
	_commandInitiated( false ),
	_currentContext(0),
	_rendezVous(0),
	_credentials(0),
	_accountManagementClient(0),
	_competitionClient(0),
	_matchMakingClient(0),
	_friendsClient(0),
	_NATTraversalClient(0),
	_peerService(0),
	_peerManager(0),
	_state(0),
	_TTLMatchMakingClient(0),
//	_hostProcessState(HOST_STATE_CREATE_GATHERING),
	_roundID(0),
	_requestType(REQUEST_TYPE_LOGIN),
	_notificationEventHandler(0),
	_isHost(false),
	_isWantingToQuit(false),
	_isCompetitionRound(false),
	_lastCallResult(false),
	_lastClanCallResult( 1 ),
	_requestingHardwareConnection(false),
	_hostMigrationCallback(NULL),
	_alertContext( 0 ),
	_alertCheckTime( 0 ),
	_alertCommandInitiated( false ),
	_gatheringUpdateContext( 0 ),
	_isGatheringUpdateReady( false ),
	_gatheringUpdateCommandInitiated( false ),
	_dnsResolutionThread( 0 )
{
	_errorString[0] = 0;
	// Purposefully not added to list!
	OSIstrcpy( _guestAccount.player.name,	_T("Guest") );
	OSIstrcpy( _guestAccount.password,		_T(GUEST_PLAYER_PASSWORD) );
	// No ID necessary

	start();
}



///---------------------------------------------------------
/// Destructor
///---------------------------------------------------------
OSIPS2::~OSIPS2()
{
	g_debugOutput.TSPrint(g_debugOutput.Category("OSI"), "Killing OSI");

	if( _dnsResolutionThread != NULL )
	{
		// Our DNS lookup thread is still running, kill it
#ifdef PS2
		sceInsockAbortGethostbyname( g_rendez_vous_server );
#endif
		_dnsResolutionThread->Kill( true );
		delete _dnsResolutionThread;
		_dnsResolutionThread = NULL;
	}

	_logout();

#ifdef DIRECTX_PC
	BackEndServices::SetSandboxAccessKey(_T(""));
#else // DIRECTX_PC
	BackEndServices::SetSandboxAccessKey(_T(NULL));
#endif // DIRECTX_PC
}



// OSIFIXME AV
///---------------------------------------------------------
/// The main OSI fucntion that's called every frame
///---------------------------------------------------------
void OSIPS2::_runFrame( void )
{
	static bool rendezVousStarted = false;

	if ( _rendezVous )
	{
		rendezVousStarted = true;
//rhn		Scheduler::GetInstance()->Dispatch( RENDEZVOUS_DISPATCH_TIME_SLICE );
		Scheduler::GetInstance()->DispatchAll();

		if( _isConnected )
		{
			_checkForNewAlerts();
			_checkForGatheringUpdates();
		}
	}
	else if( rendezVousStarted )
	{
		printf("-- OSI -- RendezVous has cleaned itself up.\n");
		rendezVousStarted = false;
	}
	//_isConnected = false ;
	//if ( _rendezVous )
	//{
	//	_isConnected = true ;
	//	PxNetInterface::lan_front_end_tick();
	//}
}



///---------------------------------------------------------
/// This initializes the NetZ dependent stack.
///---------------------------------------------------------
bool OSIPS2::start( void )
{
	//MemoryManager::SetBasicMemoryFunctions( OSImalloc, OSIfree );

//	TraceLog::GetInstance()->SetFlag(D_TRACE_TRANSPORT_PROTO|D_TRACE_TRANSPORT|TRACE_INIT);

//	// Initialize Sif RPC. It should be removed if it was called before calling
//	// this method.
//	sceSifInitRpc(0);

	// EENET internally uses five threads. For your application to run correctly
	// using EENET, you should give your application threads a higher priority than
	// EENET (the default EENET thread priorities are 32, 34, 35, 36 and 48).
	// In order to give EENET some CPU time, you should implement the VSYNC
	// interrupt/semaphore technique (see Sony's atok\softkb sample) instead of
	// calling sceGsSyncV, which is actively spining.
	// To chage EENET thread pririties, see PS2Networking::SetEENETParams.
	//
	// Otherwise, you can change the Main Thread Priority to be lower than EENET so that
	// EENET will not be starving.  However, this is not the prefered solution.
	// ::ChangeThreadPriority(::GetThreadId(),50);


	// To enable single-thread mode, uncomment the next 2 lines of code.
	// You then need to call Scheduler::GetInstance()->DispatchAll() periodically.
	// We suggest that you call this method twice a frame before and after your game
	// logic calculations such as physic and AI (which normally update the duplicated
	// objects) and just before your rendering.  See the Threading Model section of the
	// user manual for details.
	//
//	NetZ::UseThreads(false);
//	NetZ::SelectThreadSafeExecution(false);


	// STEP 1. Read the network configuration profiles from either:
	// 		- A file located on the host machine (for dev use only)
	// 		  or on a CDDVD.
	// 		- A memory card.
//	PS2Networking::SetNetConfigPath(&oNetConfigPath);
//	PS2Module::SetDefaultPath(&oModulePath);

//	PS2Module::Load(_T("sio2man"));
//	PS2Module::Load(_T("mcman"));
//	PS2Module::Load(_T("mcserv"));

	uint8 category = g_debugOutput.RegisterCategory("OSI", "OSI");
	g_debugOutput.TSPrint(category, "Starting OSI");

#ifdef CONSUMER_BUILD
	BackEndServices::SetSandboxAccessKey(_T("dc77bh"));
#else
	BackEndServices::SetSandboxAccessKey(_T("sw32h"));
#endif

	return true ;
}

bool OSIPS2::isOrphanSession( void )
{
	if ( _hostMigrationCallback )
		return _hostMigrationCallback->SessionHostIsOrphan();
	return false ;
}


///-------------------------------------------------------
/// PS2-specific process routine for doing some work on
/// outstanding requests.
///
/// Returns true if the last request is finished processing
///	and an outcome (either success or failure) is known.
///--------------------------------------------------------
bool OSIPS2::processLastRequest()
{
	bool finished = false ;

	switch ( _requestType )
	{
#ifndef DIRECTX_PC
		case REQUEST_TYPE_HARDWARE_ACTIVATION:			finished = _processLastHardwareConnectionRequest();	break;
#endif
		case REQUEST_TYPE_ACCOUNT_INFO:					finished = _updateAccountInfo();					break;
		case REQUEST_TYPE_LOGIN:						finished = _updateLogin();							break;
		case REQUEST_TYPE_DNS_LOOKUP:					finished = _updateDNSLookup();						break;
		case REQUEST_TYPE_RENDEZVOUS_HOST_MATCH:		finished = _updateHostMatch();						break;
		case REQUEST_TYPE_RENDEZVOUS_FIND_MATCH:		finished = _processFindMatchRequest();				break;
		case REQUEST_TYPE_RENDEZVOUS_JOIN_MATCH:		finished = _updateJoinMatch();						break;
		case REQUEST_TYPE_RENDEZVOUS_CREATE_ACCOUNT:	finished = _updateCreateAccount();					break;
		case REQUEST_TYPE_RENDEZVOUS_EDIT_ACCOUNT:		finished = _processEditAccount();					break;
		case REQUEST_TYPE_RENDEZVOUS_SIMPLE_CALL:		finished = _processRendezVousCall();				break;
		case REQUEST_TYPE_RENDEZVOUS_FRIENDS_LIST:		finished = _updateGetFriends();						break;
		case REQUEST_TYPE_ACCEPT_MATCH_INVITE:			finished = _processAcceptMatchInvite();				break;
		case REQUEST_TYPE_AUTHORIZE_FRIENDSHIP:			finished = _updateAuthorizeFriendship();			break;
		case REQUEST_TYPE_DENY_FRIENDSHIP:				finished = _updateDenyFriendship();					break;
		case REQUEST_TYPE_BLOCK_FRIENDSHIP:				finished = _updateBlockFriendship();				break;
		case REQUEST_TYPE_UNBLOCK_FRIENDSHIP:			finished = _updateUnblockFriendship();				break;
		case REQUEST_TYPE_OFFER_FRIENDSHIP:				finished = _updateOfferFriendship();				break;
		case REQUEST_TYPE_END_FRIENDSHIP:				finished = _updateEndFriendship();					break;
		case REQUEST_TYPE_CANCEL_FRIENDSHIP:			finished = _updateCancelFriendshipRequest();		break;
		case REQUEST_TYPE_GET_PLAYERS_STATS:			finished = _updateGetPlayersStats();				break;
		case REQUEST_TYPE_GET_LADDER_STATS:				finished = _updateGetLadderStats();					break;
		case REQUEST_TYPE_GET_TOP_PLAYERS_STATS:		finished = _updateGetTopPlayersStats();				break;
		case REQUEST_TYPE_GET_TOP_CLANS_STATS:			finished = _updateGetTopClansStats();				break;
		case REQUEST_TYPE_RENDEZVOUS_CREATE_ROUND:		finished = _updateCreateRound();					break;
		case REQUEST_TYPE_RENDEZVOUS_JOIN_ROUND:		finished = _updateJoinRound();						break;
		case REQUEST_TYPE_RENDEZVOUS_START_ROUND:		finished = _updateStartRound();						break;
		case REQUEST_TYPE_RENDEZVOUS_END_ROUND:			finished = _updateEndRound();						break;
		case REQUEST_TYPE_RENDEZVOUS_QUIT_MATCH:		finished = _updateEndMatch();			 			break;
		case REQUEST_TYPE_UPDATE_MATCH_OPTIONS:			finished = _processRendezVousUpdateMatchOptions();	break;
		case REQUEST_TYPE_LOGOUT:						finished = true;									break;
		case REQUEST_TYPE_STATION_URL_LIST:				finished = _updateStationURLListRequest();			break;
		case REQUEST_TYPE_HOST_MIGRATION:				finished = _updateUpdateGathering();				break;
		case REQUEST_TYPE_INVITE_TO_MATCH:				finished = _updateInviteToMatch();					break;
		case REQUEST_TYPE_GET_MATCH_INVITATIONS:		finished = _updateGetMatchInvitations();			break;
		case REQUEST_TYPE_PLAYER_JOIN_CURRENT_MATCH:	finished = _updateUpdateGathering();				break;
		case REQUEST_TYPE_PLAYER_LEAVE_CURRENT_MATCH:	finished = _updateUpdateGathering();				break;
		case REQUEST_TYPE_FILTER_TEXT_FOR_PROFANITY:	finished = _updateFilterTextForProfanity();			break;

		// Clan requests
		case REQUEST_TYPE_CLAN_CREATE:					finished = _updateClanCreate();						break;
		case REQUEST_TYPE_CLAN_UPDATE_MY_INFO:			finished = _updateClanUpdateMyClanInformation();	break;
		case REQUEST_TYPE_CLAN_JOIN_BY_INVITATION:		finished = _updateClanJoinByInvitation();			break;
		case REQUEST_TYPE_CLAN_DENY_INVITATION:			finished = _updateClanDenyInvitation();				break;
		case REQUEST_TYPE_CLAN_BLOCK_INVITATION:		finished = _updateClanBlockInvitation();			break;
		case REQUEST_TYPE_CLAN_SEND_INVITATION:			finished = _updateClanSendInvitation();				break;
		case REQUEST_TYPE_CLAN_CANCEL_SENT_INVITATION:	finished = _updateClanCancelSentInvitation();		break;
		case REQUEST_TYPE_CLAN_INVITATION_LIST:			finished = _updateClanInvitationList();				break;
		case REQUEST_TYPE_CLAN_ID_LIST_FOR_PLAYER:		finished = _updateClanIdListForPlayer();			break;
		case REQUEST_TYPE_CLAN_INFO:					finished = _updateClanInfoList();					break;
		case REQUEST_TYPE_CLAN_MEMBERSHIP_LIST:			finished = _updateClanMembershipList();				break;
		case REQUEST_TYPE_CLAN_LEAVE:					finished = _updateClanLeave();						break;
		case REQUEST_TYPE_CLAN_NOTICE_LIST:				finished = _updateClanNoticeList();					break;
		case REQUEST_TYPE_CLAN_POST_NOTICE:				finished = _updateClanPostNotice();					break;
		case REQUEST_TYPE_CLAN_KICK_MEMBER:				finished = _updateClanKickMember();					break;
		case REQUEST_TYPE_CLAN_DEMOTE_MEMBER:			finished = _updateClanDemoteMember();				break;
		case REQUEST_TYPE_CLAN_PROMOTE_MEMBER:			finished = _updateClanPromoteMember();				break;
		case REQUEST_TYPE_CLAN_TRANSFER_LEADERSHIP:		finished = _updateClanTransferLeadership();			break;
		case REQUEST_TYPE_MESSAGE_OF_THE_DAY:			finished = _updateMessageOfTheDay();				break;

		default:
			setLastRequestStatus( STATUS_SUCCEEDED );
			finished = true ;
			break ;
	}

	if ( finished )
	{
		g_debugOutput.TSPrint( g_debugOutput.Category("OSI"), "end" );
		setActiveState(OSI_STATE_IDLE);
	}
	return finished ;
}



///--------------------------------------------------------
/// Generic rendezvous update routine for giving time to
/// call contexts.
///--------------------------------------------------------
bool OSIPS2::_processRendezVousCall( void )
{
	setActiveState(OSIPS2_STATE_REQUEST_TYPE_RENDEZVOUS_SIMPLE_CALL);
	bool isOutcomeKnown	= _currentContext->Wait( _defaultTimeSlice );
	if ( isOutcomeKnown )
	{
		if ( !_lastCallResult )
		{
			return true ;
		}
	}

	// Handle Timeouts
	static int waitCount = 0;
	if ( waitCount > _defaultTimeout )
	{
		waitCount = 0;
		setLastRequestStatus(STATUS_FAILED);
		setErrorString(GetOSIStringByEnum(OSIPS2_ERROR_TIMEOUT_ON_PROCESS_RENDEZVOUS_CALL));
		return true;
	}
	else
		waitCount++;

	return false ;
}


// ------------------------------------------
// Cleans up objects that were created by a login attempt
// ------------------------------------------
void OSIPS2::_cleanupRendezVous(bool deleteRendezVous)
{
	if( _currentContext != NULL && _currentContext->GetState() == CallContext::CallInProgress )
	{
		_currentContext->Cancel();
	}

	if( _alertContext != NULL && _alertContext->GetState() == CallContext::CallInProgress )
	{
		_alertContext->Cancel();
	}

	if( _gatheringUpdateContext != NULL && _gatheringUpdateContext->GetState() == CallContext::CallInProgress )
	{
		_gatheringUpdateContext->Cancel();
	}

	_lstRVMatches.clear();

	if ( Scheduler::GetInstance() )
	{
		Scheduler::GetInstance()->DispatchAll();
	}

	if (_competitionClient != NULL)
	{
		qDelete _competitionClient;
		_competitionClient = NULL;
	}

	if (_matchMakingClient != NULL)
	{
		if (_TTLMatchMakingClient != NULL)
		{
			_matchMakingClient->UnregisterExtraProtocol( _TTLMatchMakingClient );
			qDelete _TTLMatchMakingClient;
			_TTLMatchMakingClient = NULL;
		}
		qDelete _matchMakingClient;
		_matchMakingClient = NULL;
	}

	if ( _notificationEventHandler != NULL )
	{
		qDelete _notificationEventHandler ;
		_notificationEventHandler = NULL ;
	}

	if (_friendsClient != NULL)
	{
		qDelete _friendsClient;
		_friendsClient = NULL;
	}

	if (_accountManagementClient != NULL)
	{
		qDelete _accountManagementClient;
		_accountManagementClient = NULL;
	}

	if (_currentContext != NULL)
	{
		qDelete _currentContext;
		_currentContext = NULL;
	}

	if (_alertContext != NULL)
	{
		qDelete _alertContext;
		_alertContext = NULL;
	}

	if (_gatheringUpdateContext != NULL)
	{
		qDelete _gatheringUpdateContext;
		_gatheringUpdateContext = NULL;
	}

	if ( _peerService != NULL )
	{
		qDelete _peerService;
		_peerService = NULL;
	}

	if ( _peerManager != NULL )
	{
		qDelete _peerManager;
		_peerManager = NULL;
	}

	if ( _NATTraversalClient != NULL )
	{
		qDelete _NATTraversalClient;
		_NATTraversalClient = NULL;
	}

	if ( deleteRendezVous )
	{
		if ( _rendezVous != NULL )
		{
			qDelete _rendezVous;
			_rendezVous	= NULL;
		}

		_credentials = NULL ; // if we delete them, Quazal crashes!
	}
}



//-------------------------------------------
// This function is not implemented on PS2
// because RendezVous does not have pending
// messages.
//--------------------------------------------
bool OSIPS2::isRequiredMessagePending()
{
	return false ;
}



//-------------------------------------------
// This function is not implemented on PS2
// because RendezVous does not have pending
// messages.
//--------------------------------------------
bool OSIPS2::isRecommendedMessagePending()
{
	return false ;
}



//-------------------------------------------
// This function is not implemented on the PS2
// because RendezVous does not have an external
// account maintenance app (like XBox Dashboard)
//--------------------------------------------
void OSIPS2::launchExternalAccountMaintenanceApp()
{
}



//-------------------------------------------------------------------------
// Converts a quazal <FriendData> into an <OSIPlayer>.
//-------------------------------------------------------------------------
void OSIPS2::_convertQuazalPlayerToOSIPlayer( const FriendData& quazalPlayer, OSIPlayer& osiPlayer, bool temp_hack_isPendingOutgoing )
{
	//FriendData now returns strings rather than bytes.  Our code
	// must change!  OSIFIXME!

	String statusString = quazalPlayer.GetStatus();
	qByte relationship = quazalPlayer.GetRelationship();
//	qUnsignedInt32 details = quazalPlayer.GetDetails();

	/// Get quazal player status, relationship, details
//	qByte status		= 0 ;//quazalPlayer.GetStatus();
//	qByte relationship	= 0 ;//quazalPlayer.GetRelationship();
//		unsigned int quazalPlayerDetails	= quazalPlayer.GetDetails();

	osiPlayer.id.ps2 = quazalPlayer.GetPrincipalID();
	UTF8Decode( osiPlayer.name, quazalPlayer.GetName().CStr(), NUM_ARRAY_ELEMENTS( osiPlayer.name ) );
	osiPlayer.state.currentControllerIndex		= 0;
	osiPlayer.state.isOnline					= IsOnlineFromStatusString( statusString.CStr() );//FriendsClient::Offline);
	osiPlayer.state.isCurrentGameJoinable		= false;
	osiPlayer.state.isPlayingGame				= false; // OSIFIXME: Help!  This used to work, but Quazal removed it from their status states!
	osiPlayer.state.isVoiceAvailable			= true;
	osiPlayer.lookingToPlay						= false;
	osiPlayer.isAFriend							= (relationship == PlayerRelationships::Friend);
	osiPlayer.hasRequestedMyFriendship			= (relationship == PlayerRelationships::Pending) && !temp_hack_isPendingOutgoing;
	osiPlayer.haveRequestedTheirFriendship		= (relationship == PlayerRelationships::Pending) && temp_hack_isPendingOutgoing;
}



//-------------------------------------------------------------------------
// Appends each <FriendData> item in a Quazal player list
//	into an OSIPlayer and appends it to the end of the specified
//	OSIPlayerList.
//-------------------------------------------------------------------------
void OSIPS2::_appendQuazalPlayerListToOSIPlayerList( qList<FriendData>& quazalPlayerList, OSIPlayerList& osiPlayerList, bool temp_hack_isPendingOutgoing )
{
	/// Transfer / convert / append each player until the Quazal list is empty
	while( !quazalPlayerList.empty() )
	{
		/// Get the quazal player at the front of the list
		FriendData& quazalPlayer = quazalPlayerList.front();

		/// Create an equivalent OSI player
		OSIPlayer osiPlayer;
		_convertQuazalPlayerToOSIPlayer( quazalPlayer, osiPlayer, temp_hack_isPendingOutgoing );

		/// Remove the converted player and continue (if any remain)
		quazalPlayerList.pop_front();

		/// Add the player to the specified player list
		osiPlayerList.push_back( osiPlayer );
	}
}


///------------------------------------------------------------
/// Converts a TTLSession to a OSIMatch
///------------------------------------------------------------
void OSIPS2::_sessionToMatch( TTLSession &session, OSIMatch &match)
{
	/// copy session
	match.id.ps2				= session.GetID();
	match.maximumPlayers		= session.GetMaxParticipants();
	match.numberOfPlayers		= session.GetMinParticipants();
	UTF8Decode( match.options.description, session.GetDescription().CStr(), NUM_ARRAY_ELEMENTS( match.options.description ) );

	// copy session options OSIFIXME_MATCHOPTIONS
	match.options.levelIndex	= session.levelIndex ;
	match.options.gameTypeIndex	= session.gameType ;
	match.options.timeLimit		= session.timeLimit ;
	UTF8Decode( match.options.levelName, session.levelName.CStr(), NUM_ARRAY_ELEMENTS( match.options.levelName ) );

	match.options.scoreLimit			= session.scoreLimit;
	match.options.tagLimit				= session.tagLimit;
	match.options.bestOfSeries			= session.bestOfSeries;
	match.options.maxPlayers			= session.maxPlayers;
	match.options.reservedPlayers		= session.reservedPlayers;
	match.options.totalConnectedPlayers = session.totalConnectedPlayers;
	match.options.isCustomQuery			= session.isCustomQuery;
	match.options.isRanked				= session.isRanked;
	match.options.isPrivate				= session.isPrivate;
	match.options.playersRespawn		= session.playersRespawn;
	match.options.friendlyFire			= session.friendlyFire;
	match.options.apprehension			= session.apprehension;
	match.options.arcadeDamage			= session.arcadeDamage;
	match.options.selfDamage			= session.selfDamage;
	match.options.alternatingTeams		= session.alternatingTeams;
}

///--------------------------------------------------------------
/// Converts a OSIMatch to a TTLSession.  TTLSession structure:
///--------------------------------------------------------------
void OSIPS2::_matchToSession( OSIMatch &match, TTLSession &session)
{
	char buffer[ OSI_STRING_LENGTH_HUGE ];
	// copy match
	session.SetDescription( UTF8Encode( buffer, match.options.description, NUM_ARRAY_ELEMENTS( buffer ) ) );
	session.SetMinParticipants( match.numberOfPlayers );
	session.SetMaxParticipants( match.maximumPlayers );

	// copy match options OSIFIXME_MATCHOPTIONS
	session.levelIndex					= match.options.levelIndex ;
	session.gameType					= match.options.gameTypeIndex ;
	session.timeLimit					= match.options.timeLimit ;
	session.levelName					= UTF8Encode( buffer, match.options.levelName, NUM_ARRAY_ELEMENTS( buffer ) );
	session.scoreLimit					= match.options.scoreLimit;
	session.tagLimit					= match.options.tagLimit;
	session.bestOfSeries				= match.options.bestOfSeries;
	session.maxPlayers					= match.options.maxPlayers;
	session.reservedPlayers				= match.options.reservedPlayers;
	session.totalConnectedPlayers		= match.options.totalConnectedPlayers;
	session.isCustomQuery				= match.options.isCustomQuery;
	session.isRanked					= match.options.isRanked;
	session.isPrivate					= match.options.isPrivate;
	session.playersRespawn				= match.options.playersRespawn;
	session.friendlyFire				= match.options.friendlyFire;
	session.apprehension				= match.options.apprehension;
	session.arcadeDamage				= match.options.arcadeDamage;
	session.selfDamage					= match.options.selfDamage;
	session.alternatingTeams			= match.options.alternatingTeams;
}


///----------------------------------------------------------
/// Reset AND start the timer for an OSI asynchronous call.
///----------------------------------------------------------
void OSIPS2::_resetTimeoutTimer()
{
	_chrono.Reset();
	_chrono.Start();
}

///----------------------------------------------------------
/// This gives the current context a timeslice to do some
/// work.  Returns an enumeration of either WORKING, COMPLETE,
/// or TIMEOUT.
///----------------------------------------------------------
bool OSIPS2::_checkForCommandCompletionOrTimeout( int& stateVariable, int failureState, ProtocolCallContext* context )
{
	if ( _chrono.Check() > _defaultTimeout )
	{
		_commandInitiated	= false ;
		_chrono.Reset();
		stateVariable		= failureState ;
		setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_REQUEST_TIMED_OUT) );
		setLastRequestStatus( STATUS_FAILED );
		return false ;
	}

	if ( context )
		return context->Wait( _defaultTimeSlice );

	return _currentContext->Wait( _defaultTimeSlice );
}


//--------------------------------------------------------
// This is called when an event occurs enabling us to take
// action.
//--------------------------------------------------------
void OSIPS2NotificationEventHandler::ProcessNotificationEvent( const NotificationEvent& event )
{
	String	strMessage;
	OSIPS2*	theOSIPS2	= NULL ;

	switch (event.GetType())
	{
		case NotificationEvents::FriendEvent:
			break;
		case NotificationEvents::SessionLaunched:
			break;
		case NotificationEvents::ParticipationEvent:
			theOSIPS2 = (OSIPS2*)OSI::getSingletonPtr();
			if ( theOSIPS2 )
			{
				theOSIPS2->addOpponent( event.GetParam1(), event.GetParam2() );
			}
			break;
		default:
			printf( "UNEXPECTED EVENT TYPE %d\n", event.GetType() );
			break;
	}
}



///---------------------------------------------------------
/// setRequestType
///
/// Mutator for _requestType, the state variable for the
///  main OSIPS2 state machine.
///---------------------------------------------------------
void OSIPS2::setRequestType( RequestType requestType )
{
	const OSIchar* RequestTypeStrings[] =
	{
		"REQUEST_TYPE_HARDWARE_ACTIVATION",
		"REQUEST_TYPE_ACCOUNT_INFO",
		"REQUEST_TYPE_LOGIN",
		"REQUEST_TYPE_DNS_LOOKUP",
		"REQUEST_TYPE_RENDEZVOUS_HOST_MATCH",
		"REQUEST_TYPE_RENDEZVOUS_FIND_MATCH",
		"REQUEST_TYPE_RENDEZVOUS_JOIN_MATCH",
		"REQUEST_TYPE_RENDEZVOUS_CREATE_ACCOUNT",
		"REQUEST_TYPE_RENDEZVOUS_EDIT_ACCOUNT",
		"REQUEST_TYPE_RENDEZVOUS_SIMPLE_CALL",
		"REQUEST_TYPE_RENDEZVOUS_FRIENDS_LIST",
		"REQUEST_TYPE_RENDEZVOUS_QUIT_MATCH",
		"REQUEST_TYPE_RENDEZVOUS_BEGIN_ROUND",
		"REQUEST_TYPE_RENDEZVOUS_END_ROUND",
		"REQUEST_TYPE_ACCEPT_MATCH_INVITE",
		"REQUEST_TYPE_AUTHORIZE_FRIENDSHIP",
		"REQUEST_TYPE_DENY_FRIENDSHIP",
		"REQUEST_TYPE_BLOCK_FRIENDSHIP",
		"REQUEST_TYPE_UNBLOCK_FRIENDSHIP",
		"REQUEST_TYPE_OFFER_FRIENDSHIP",
		"REQUEST_TYPE_CANCEL_FRIENDSHIP",
		"REQUEST_TYPE_END_FRIENDSHIP",
		"REQUEST_TYPE_INVITE_TO_MATCH",
		"REQUEST_TYPE_GET_MATCH_INVITATIONS",
		"REQUEST_TYPE_LOGOUT",
		"REQUEST_TYPE_GET_PLAYERS_STATS",
		"REQUEST_TYPE_GET_TOP_PLAYERS_STATS",
		"REQUEST_TYPE_GET_TOP_CLANS_STATS",
		"REQUEST_TYPE_GET_LADDER_STATS",
		"REQUEST_TYPE_RENDEZVOUS_CREATE_ROUND",
		"REQUEST_TYPE_RENDEZVOUS_JOIN_ROUND",
		"REQUEST_TYPE_RENDEZVOUS_START_ROUND",
		"REQUEST_TYPE_UPDATE_MATCH_OPTIONS",
		"REQUEST_TYPE_FILTER_TEXT_FOR_PROFANITY",

		// Clans
		"REQUEST_TYPE_CLAN_CREATE",
		"REQUEST_TYPE_CLAN_UPDATE_MY_INFO",
		"REQUEST_TYPE_CLAN_JOIN_BY_INVITATION",
		"REQUEST_TYPE_CLAN_DENY_INVITATION",
		"REQUEST_TYPE_CLAN_BLOCK_INVITATION",
		"REQUEST_TYPE_CLAN_SEND_INVITATION",
		"REQUEST_TYPE_CLAN_CANCEL_SENT_INVITATION",
		"REQUEST_TYPE_CLAN_INVITATION_LIST",
		"REQUEST_TYPE_CLAN_ID_LIST_FOR_PLAYER",
		"REQUEST_TYPE_CLAN_INFO",
		"REQUEST_TYPE_CLAN_MEMBERSHIP_LIST",
		"REQUEST_TYPE_CLAN_LEAVE",
		"REQUEST_TYPE_CLAN_NOTICE_LIST",
		"REQUEST_TYPE_CLAN_POST_NOTICE",
		"REQUEST_TYPE_CLAN_KICK_MEMBER",
		"REQUEST_TYPE_CLAN_DEMOTE_MEMBER",
		"REQUEST_TYPE_CLAN_PROMOTE_MEMBER",
		"REQUEST_TYPE_CLAN_TRANSFER_LEADERSHIP",

		"REQUEST_TYPE_STATION_URL_LIST",
		"REQUEST_TYPE_HOST_MIGRATION",
		"REQUEST_TYPE_PLAYER_JOIN_CURRENT_MATCH",
		"REQUEST_TYPE_PLAYER_LEAVE_CURRENT_MATCH",
		"REQUEST_TYPE_MESSAGE_OF_THE_DAY",
		"REQUEST_TYPE_LAST_REQUEST_TYPE",
	};

	printf( "*** OSIPS2 STATE: %s -> %s\n", RequestTypeStrings[ _requestType ], RequestTypeStrings[ requestType ] );
	g_debugOutput.TSPrint( g_debugOutput.Category("OSI"), "Request: %s", RequestTypeStrings[ requestType ] );
	_requestType = requestType;
}



///----------------------------------------------------------------------
/// Not implemented
///----------------------------------------------------------------------
bool OSIPS2::requestSendTextMessage( const OSIchar* message )
{
	return true ;
}



///----------------------------------------------------------------------
/// Not implemented
///----------------------------------------------------------------------
void OSIPS2::publishGlobalVoiceMute( bool muteAllVoice )
{
}


// OSIFIXME AV
///------------------------------------------------------------------------
/// Publishes (to PxNetInterface, ultimately to NetZ).
///------------------------------------------------------------------------
void OSIPS2::publishMyGameReadiness( bool readyToPlay )
{
}



///-------------------------------------------------------------------------
/// Queries whether or not our opponent is ready to play.
///-------------------------------------------------------------------------
bool OSIPS2::isOpponentReadyToPlayGame()
{
	return false;
}



///---------------------------------------------------------
///
///---------------------------------------------------------
bool OSIPS2::_updateAccountInfo( void )
{
	switch (_state)
	{
		case ACCOUNT_INFO: _processAccountInfo(COMMAND_SUCCESS, COMMAND_FAILURE); break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus(STATUS_FAILED); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus(STATUS_SUCCEEDED);
			return true ; // complete, success
	}

	return false ; // keep working
}

void OSIPS2::_processAccountInfo( int successState, int failureState )
{
	setLastRequestStatus(STATUS_WORKING);
	if ( !_commandInitiated )
	{
		bool result	= _TTLMatchMakingClient->CallGetEidosAccountInfo(_currentContext, &_privateAccountData, &_publicAccountData, &_motd);
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FAILED_ACTIVE_ACCOUNT) );
			_state = failureState ;
			return;
		}
		_commandInitiated	= true ;
		_resetTimeoutTimer();
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		_currentAccount.flags = _privateAccountData.m_flags;
		OSIstrncpy( _currentAccount.postalCode, _privateAccountData.m_postalCode.CStr(), NUM_ARRAY_ELEMENTS( _currentAccount.postalCode ) );
		OSIstrncpy( _currentAccount.email, _motd.CStr(), NUM_ARRAY_ELEMENTS( _currentAccount.email ) );
		_currentAccount.countryCode = _publicAccountData.m_country;
		_currentAccount.birthdayDay = _privateAccountData.m_dateOfBirth.GetDay();
		_currentAccount.birthdayMonth = _privateAccountData.m_dateOfBirth.GetMonth();
		_currentAccount.birthdayYear = _privateAccountData.m_dateOfBirth.GetYear();
		_commandInitiated	= false ;
		_state				= successState ;
	}
}

///-------------------------------------------------------------------------
/// Periodically checks for new alerts to tell the user about.
///-------------------------------------------------------------------------
void OSIPS2::_checkForNewAlerts()
{
	if( _TTLMatchMakingClient == NULL ) return;

	// If it is time to check for alerts
	if( _alertChrono.Check() >= _alertCheckTime )
	{
		// Kick off a call to RendezVous for alerts and wait for results
		if( !_alertCommandInitiated )
		{
			_TTLMatchMakingClient->CallGetAlerts( _alertContext, &_alerts.alertsCurrent );
			_alertCommandInitiated = true;
			ResetAndStartTimer( _alertTimeoutChrono );
		}
		
		_checkForAlertCommandCompletionOrTimeout();
	}
}

///-------------------------------------------------------------------------
/// Check for command completion or timeout for an alert call
///-------------------------------------------------------------------------
bool OSIPS2::_checkForAlertCommandCompletionOrTimeout()
{
	bool timeout = _alertTimeoutChrono.Check() > _defaultTimeout;
	bool commandCompleted = _alertContext->Wait( _defaultTimeSlice );

	if( timeout || commandCompleted )
	{
		_alertCommandInitiated	= false ;
		_alertTimeoutChrono.Stop();
		ResetAndStartTimer( _alertChrono );
		if( _alertCheckTime == 0 )
		{
			_alertCheckTime = ALERT_CHECK_TIME;
		}
		if( commandCompleted )
		{
			_alerts.CheckAlerts();
		}
		return true;
	}
	
	return false;
}

///-------------------------------------------------------------------------
/// I absolutely can't stand doing this, but I don't care.  This code is
/// giant and unwieldy already, not like this is making it much worse.
///-------------------------------------------------------------------------
void OSIPS2::_checkForGatheringUpdates( void )
{
	// If it is time to update the gathering (don't call if we are updating match options somewhere else)
	if( _isGatheringUpdateReadyForProcess() )
	{
		_gatheringState = FIND_GATHERING_BY_ID;
		_isGatheringUpdateReady = false;
	}

	_updateGatheringUpate();
}

///-------------------------------------------------------------------------
/// Is a gathering update ready to be processed.  Make sure we wait until
/// any states in the primary state machine and our gathering update
/// state machine aren't doing anything that would be bad for us.
///-------------------------------------------------------------------------
bool OSIPS2::_isGatheringUpdateReadyForProcess( void )
{
	return _isGatheringUpdateReady &&
		_state != UPDATE_MATCH_OPTIONS &&
		_state != FIND_GATHERING_BY_ID &&
		_state != UPDATE_GATHERING &&
		_gatheringState != FIND_GATHERING_BY_ID &&
		_gatheringState != UPDATE_GATHERING;
}

///-------------------------------------------------------------------------
/// Check for command completion or timeout for an update gathering call
///-------------------------------------------------------------------------
bool OSIPS2::_checkForGatheringUpdatesCommandCompletionOrTimeout( int failureState )
{
	bool timeout = _gatheringUpdateTimeoutChrono.Check() > _defaultTimeout;
	bool commandCompleted = _gatheringUpdateContext->Wait( _defaultTimeSlice );

	if( timeout || commandCompleted )
	{
		_gatheringUpdateCommandInitiated = false ;
		_gatheringUpdateTimeoutChrono.Stop();
		if( timeout )
		{
			_gatheringState = failureState;
			return false;
		}
		return true;
	}
	
	return false;
}

///---------------------------------------------------------
/// First refresh the current gathering and then update
/// it with the correct total connected players.
///---------------------------------------------------------
bool OSIPS2::_updateGatheringUpate( void )
{
	switch (_gatheringState)
	{
		case FIND_GATHERING_BY_ID:
			_processRefreshCurrentGathering( UPDATE_GATHERING, COMMAND_FAILURE );
			break ;
		case UPDATE_GATHERING:
			_processUpdateCurrentGathering( COMMAND_SUCCESS, COMMAND_FAILURE );
			break ;
		case COMMAND_SUCCESS:
		case COMMAND_FAILURE:
			return true ; // we don't care if we fail or succeed
	}

	return false ; // keep working
}

///---------------------------------------------------------
/// Get current gathering info from the server.
///---------------------------------------------------------
void OSIPS2::_processRefreshCurrentGathering( int successState, int failureState )
{
	if ( !_gatheringUpdateCommandInitiated )
	{
		ResultRange defaultRange;
		bool result = _matchMakingClient->FindByID( 
			_gatheringUpdateContext, _currentGathering->GetID(), &_gatheringUpdateCallResult, &_currentGathering
		);
		if ( !result )
		{
			_gatheringState = failureState;
			return;
		}
		_gatheringUpdateCommandInitiated = true;
		ResetAndStartTimer( _gatheringUpdateTimeoutChrono );
	}

	if ( _checkForGatheringUpdatesCommandCompletionOrTimeout( failureState ) )
	{
		if( !_gatheringUpdateCallResult )
		{
			_gatheringState = failureState;
		}
		else
		{
			// Update the session with the total connected players
			TTLSession* session = (TTLSession*)_currentGathering.Get();
			session->totalConnectedPlayers = _totalConnectedPlayers;
			_gatheringState = successState;
		}
	}
}

///---------------------------------------------------------
/// Update the current gathering with the correct number
/// of connected players.
///---------------------------------------------------------
void OSIPS2::_processUpdateCurrentGathering( int successState, int failureState )
{
	if( !_gatheringUpdateCommandInitiated )
	{
		bool result = _matchMakingClient->UpdateGathering( _gatheringUpdateContext, &_gatheringUpdateCallResult, _currentGathering );
		if( !result )
		{
			// Error calling, just try again next frame.  Most likely other requests
			// will fail badly as well, so we don't need to worry about it.
			_gatheringState = failureState;
		}
		_gatheringUpdateCommandInitiated = true;
		ResetAndStartTimer( _gatheringUpdateTimeoutChrono );
	}
	
	if( _checkForGatheringUpdatesCommandCompletionOrTimeout( failureState ) )
	{
		if( !_gatheringUpdateCallResult )
		{
			_gatheringState = failureState;
		}
		else
		{
			_gatheringState = successState;
		}
	}
}






















































#ifndef DIRECTX_PC

/// ***************************************************************************
///	 ____  _   _    _    ____
///	|  _ \| \ | |  / \  / ___|
///	| | | |  \| | / _ \ \___ \
///	| |_| | |\  |/ ___ \ ___) |
///	|____/|_| \_/_/   \_\____/
///
/// ***************************************************************************

extern void SuspendGameIOPUse();
extern void ResumeGameIOPUse();

 ///---------------------------------------------------------
/// Starts the DNAS authorization process
///
/// Returns false if this has FAILED
///---------------------------------------------------------
bool OSIPS2::beginDNASAuth()
{
	int proxy_state = 0;
	char proxy_host[512];
	unsigned short proxy_port = 0;

	setActiveState(OSIPS2_STATE_BEGIN_DNAS_AUTH);

	// load the DNAS DLL
	bool success = g_DllLoader.Load( DNAS_DLL, true );
	if ( !success )
	{
		char buffer[ OSI_STRING_LENGTH_HUGE ];
		OSIsprintf(buffer, "DNAS Error (%d) %s", sceDNAS2_SS_INVALID_MEDIA, g_translator.Translate(GetOSIStringByEnum(OSIPS2_DNAS_DISC_INFO_ERROR)));
		setErrorString( buffer );
		return false;
	}

	// We need to suspend our IOP useage
	scePrintf("Suspend IOP\n");
	SuspendGameIOPUse();

	// Call the initialize function in the DNAS dll (returns 0 if succeeded)
	bool ret = (initialize_dnas_dll(proxy_state, proxy_host, proxy_port, DNAS_TIMEOUT) == 0);
	if (!ret)
	{
		ResumeGameIOPUse();
		g_DllLoader.Unload( DNAS_DLL );
		setErrorString( GetOSIStringByEnum( OSIPS2_DNAS_UNSPECIFIED_ERROR ) );
	}

	_dnasPaused = false;

	return (ret);
}



///---------------------------------------------------------
/// Does processing on the DNAS authorization process
///
/// Returns 0 if not done, 1 if done succesfully, error code otherwise
///---------------------------------------------------------
int OSIPS2::updateDNASAuth()
{
    int result;
	OSIchar buffer[OSI_STRING_LENGTH_HUGE];

	// Unpause us once the update function has been called
	if( _dnasPaused )
	{
		scePrintf("Suspend IOP\n");
		SuspendGameIOPUse();
		_dnasPaused = false;
	}

    // Execute DNAS authentication
    result = process_dnas_dll();

    // Still more to do
    if (0 == result)
    {
        return 0;
    }

    
	// Need to resume our IOP processing
	scePrintf("Resuming IOP\n");
	ResumeGameIOPUse();
	
    // Unload DNAS dll
	 g_DllLoader.Unload( DNAS_DLL );

    // Success!
    if (1 == result)
    {
        return 1;
    }

    // Else there was an error

    // Put the proper error string into the class's error string
    switch (result)
    {
        // DNAS errors
        case sceDNAS2_SS_SERVER_BUSY:
            OSIsprintf(buffer, "DNAS Error (%d) %s", result, g_translator.Translate(GetOSIStringByEnum(OSIPS2_DNAS_AUTH_SERVER_BUSY)));
            break;

        case sceDNAS2_SS_BEFORE_SERVICE:
        case sceDNAS2_SS_OUT_OF_SERVICE:
            OSIsprintf(buffer, "DNAS Error (%d) %s", result, g_translator.Translate(GetOSIStringByEnum(OSIPS2_DNAS_SOFTWARE_TITLE_NOT_IN_SERVICE)));
            break;

        case sceDNAS2_SS_END_OF_SERVICE:
            OSIsprintf(buffer, "DNAS Error (%d) %s", result, g_translator.Translate(GetOSIStringByEnum(OSIPS2_DNAS_AUTH_SERVER_NOT_IN_SERVICE)));
            break;

        case sceDNAS2_SS_SESSION_TIME_OUT:
            OSIsprintf(buffer, "DNAS Error (%d) %s", result, g_translator.Translate(GetOSIStringByEnum(OSIPS2_DNAS_AUTH_SERVER_TIMEOUT)));
            break;

        case sceDNAS2_SS_EXTERNAL_ERROR:
        case sceDNAS2_SS_INTERNAL_ERROR:
        case sceDNAS2_SS_INVALID_SERVER:
            OSIsprintf(buffer, "DNAS Error (%d) %s", result, g_translator.Translate(GetOSIStringByEnum(OSIPS2_DNAS_AUTH_SYSTEM_ERROR)));
            break;

        case sceDNAS2_SS_INVALID_PS2:
        case sceDNAS2_SS_INVALID_HDD_BINDING:
            OSIsprintf(buffer, "DNAS Error (%d) %s", result, g_translator.Translate(GetOSIStringByEnum(OSIPS2_DNAS_HARDWARE_INFO_ERROR)));
            break;

        case sceDNAS2_SS_INVALID_MEDIA:
        case sceDNAS2_SS_INVALID_AUTHDATA:
            OSIsprintf(buffer, "DNAS Error (%d) %s", result, g_translator.Translate(GetOSIStringByEnum(OSIPS2_DNAS_DISC_INFO_ERROR)));
            break;

        // Network errors
        case sceDNAS2_ERR_GLUE_ABORT:
            OSIsprintf(buffer, "Network Error (%d) %s", result, g_translator.Translate(GetOSIStringByEnum(OSIPS2_DNAS_NETWORK_CONNECTION_ABORTED)));
            break;

        case sceDNAS2_ERR_NET_PROXY:
            OSIsprintf(buffer, "Network Error (%d) %s", result, g_translator.Translate(GetOSIStringByEnum(OSIPS2_DNAS_ERROR_IN_PROXY_SETTING)));
            break;

        case sceDNAS2_ERR_NET_TIMEOUT:
        case sceDNAS2_ERR_NET_ETIMEOUT:
            OSIsprintf(buffer, "Network Error (%d) %s", result, g_translator.Translate(GetOSIStringByEnum(OSIPS2_DNAS_CONNECTION_TIMEOUT)));
            break;

        case sceDNAS2_ERR_NET_DNS_TRY_AGAIN:
            OSIsprintf(buffer, "Network Error (%d) %s", result, g_translator.Translate(GetOSIStringByEnum(OSIPS2_DNAS_DNS_NO_RESPONSE)));
            break;

        case sceDNAS2_ERR_NET_DNS_HOST_NOT_FOUND:
        case sceDNAS2_ERR_NET_DNS_NO_RECOVERY:
        case sceDNAS2_ERR_NET_DNS_NO_DATA:
            OSIsprintf(buffer, "Network Error (%d) %s", result, g_translator.Translate(GetOSIStringByEnum(OSIPS2_DNAS_DNS_RESPONSE_ERROR)));
            break;

        case sceDNAS2_ERR_NET_DNS_OTHERS:
            OSIsprintf(buffer, "Network Error (%d) %s", result, g_translator.Translate(GetOSIStringByEnum(OSIPS2_DNAS_DNS_ERROR)));
            break;

        case sceDNAS2_ERR_NET_SSL:
        case sceDNAS2_ERR_NET_EISCONN:
        case sceDNAS2_ERR_NET_ECONNREFUSED:
        case sceDNAS2_ERR_NET_ENETUNREACH:
        case sceDNAS2_ERR_NET_ENOTCONN:
        case sceDNAS2_ERR_NET_ECONNRESET:
            OSIsprintf(buffer, "Network Error (%d) %s", result, g_translator.Translate(GetOSIStringByEnum(OSIPS2_DNAS_CONNECTION_TO_DNAS_FAILED)));
            break;

        case sceDNAS2_ERR_NET_ENOBUFS:
        case sceDNAS2_ERR_NET_EMFILE:
        case sceDNAS2_ERR_NET_EBADF:
        case sceDNAS2_ERR_NET_EINVAL:
        case sceDNAS2_ERR_NET_OTHERS:
            OSIsprintf(buffer, "Network Error (%d) %s", result, g_translator.Translate(GetOSIStringByEnum(OSIPS2_DNAS_CONNECTION_ERROR)));
            break;

        default:
            OSIsprintf(buffer, "DNAS Error (%d) %s", result, g_translator.Translate(GetOSIStringByEnum(OSIPS2_DNAS_AUTH_ERROR)));
            break;
    }

	setErrorString(buffer);
    return result;
}



///---------------------------------------------------------
/// Abort DNAS authorization and clean up
///---------------------------------------------------------
int OSIPS2::abortDNASAuth()
{
    int retVal = abort_dnas_dll();

    // Unload DNAS dll
    if (retVal == 0)
    {
		 g_DllLoader.Unload( DNAS_DLL );

		// Need to resume our IOP processing
		 scePrintf("Resuming IOP\n");
		 ResumeGameIOPUse();
		 _dnasPaused = false;
    }

	return retVal;
}

///---------------------------------------------------------
/// Like abort but we don't care if we succeed or fail
///---------------------------------------------------------
void OSIPS2::cancelDNASAuth()
{
	abort_dnas_dll();
	g_DllLoader.Unload( DNAS_DLL );

	// Need to resume our IOP processing
	scePrintf("Resuming IOP\n");
	ResumeGameIOPUse();
}

///---------------------------------------------------------
/// Abort DNAS authorization and clean up
///---------------------------------------------------------
void OSIPS2::pauseDNASAuth()
{
	if( !_dnasPaused )
	{
		scePrintf("Resuming IOP\n");
		ResumeGameIOPUse();
		_dnasPaused = true;
	}
}

#endif // DIRECTX_PC



























































/// ***************************************************************************
///	    _                             _
///	   / \   ___ ___ ___  _   _ _ __ | |_
///	  / _ \ / __/ __/ _ \| | | | '_ \| __|
///	 / ___ \ (_| (_| (_) | |_| | | | | |_
///	/_/   \_\___\___\___/ \__,_|_| |_|\__|
///
///	 __  __                                                   _
///	|  \/  | __ _ _ __   __ _  __ _  ___ _ __ ___   ___ _ __ | |_
///	| |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '_ ` _ \ / _ \ '_ \| __|
///	| |  | | (_| | | | | (_| | (_| |  __/ | | | | |  __/ | | | |_
///	|_|  |_|\__,_|_| |_|\__,_|\__, |\___|_| |_| |_|\___|_| |_|\__|
///                          |___/
/// ***************************************************************************

///-----------------------------------------------------------
/// Update function for the login state machine.
///-----------------------------------------------------------
bool OSIPS2::_updateLogin( void )
{
	switch ( _state )
	{        // STATE                           // UPDATE FUNCTION                // ON SUCCESS GO TO               // ON FAILURE GO TO             // ADDITIONAL PARAMETERS
		case LOGIN_LOGIN:						_processLogin					( LOGIN_START_PEER_MANAGER,			COMMAND_FAILURE );																	break ;
		case LOGIN_START_PEER_MANAGER:			_processStartPeerManager		( LOGIN_START_NAT_CLIENT,			COMMAND_FAILURE);																	break ;
		case LOGIN_START_NAT_CLIENT:			_processStartNATClient			( LOGIN_START_ACCOUNT_CLIENT,		COMMAND_FAILURE );																	break ;
		case LOGIN_START_ACCOUNT_CLIENT:		_processStartAccountClient		( LOGIN_START_FRIENDS_CLIENT,		COMMAND_FAILURE );																	break ;
		case LOGIN_START_FRIENDS_CLIENT:		_processStartFriendsClient		( LOGIN_START_MATCHMAKING_CLIENT,	COMMAND_FAILURE );																	break ;
		case LOGIN_START_MATCHMAKING_CLIENT:	_processStartMatchMakingClient	( LOGIN_FIND_NONLADDER_RANKING,		COMMAND_FAILURE );																	break ;
//#ifdef DIRECTX_PC
//		case LOGIN_FIND_NONLADDER_RANKING:		_processFindRanking				( LOGIN_CHECK_CD_KEY,				LOGIN_CREATE_NONLADDER_RANKING,		"Nonladder", _nonladderRanking );				break ;
//		case LOGIN_CREATE_NONLADDER_RANKING:	_processCreateRanking			( LOGIN_HOST_NONLADDER_COMPETITION,	COMMAND_FAILURE,					"Nonladder", _nonladderRanking );				break ;
//		case LOGIN_HOST_NONLADDER_COMPETITION:	_processHostCompetition			( LOGIN_CHECK_CD_KEY,				COMMAND_FAILURE,					_nonladderRanking );							break ;
//		case LOGIN_CHECK_CD_KEY:				_processCDKeyCheck				( CLAN_UPDATE_MY_INFO,				COMMAND_FAILURE );																	break ;
//#else
		case LOGIN_FIND_NONLADDER_RANKING:		_processFindRanking				( CLAN_UPDATE_MY_INFO,				LOGIN_CREATE_NONLADDER_RANKING,		"Nonladder", _nonladderRanking );				break ;
		case LOGIN_CREATE_NONLADDER_RANKING:	_processCreateRanking			( LOGIN_HOST_NONLADDER_COMPETITION,	COMMAND_FAILURE,					"Nonladder", _nonladderRanking );				break ;
		case LOGIN_HOST_NONLADDER_COMPETITION:	_processHostCompetition			( CLAN_UPDATE_MY_INFO,				COMMAND_FAILURE,					_nonladderRanking );							break ;
//#endif
		case CLAN_UPDATE_MY_INFO:				_processClanUpdateMyClanInformation( LOGIN_UPDATE_STATUS,			COMMAND_FAILURE );																	break ;
		case LOGIN_UPDATE_STATUS:				_processUpdateStatus			( STATS_GET_ALL_STATS,				COMMAND_FAILURE);																	break ;
		case STATS_GET_ALL_STATS:				_processGetStatsForPlayers		( COMMAND_SUCCESS,					COMMAND_FAILURE );																	break ;
		case COMMAND_FAILURE:
			_cleanupRendezVous();
			setLastRequestStatus( STATUS_FAILED );
			return true ; // complete, failure
		case COMMAND_SUCCESS:
			_setActiveAccountPlayerClanInfo();
			_isConnected = true;
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ; // complete, success
	}

	return false ; // keep working
}

///-----------------------------------------------------------
/// Update function for the login state machine.
///-----------------------------------------------------------
bool OSIPS2::requestDNSLookup( const OSIchar* const hostName )
{
	OSIstrncpy( _currentRequestStr, hostName, NUM_ARRAY_ELEMENTS(_currentRequestStr) );
	setRequestType( REQUEST_TYPE_DNS_LOOKUP );
	_state = LOGIN_RESOLVE_DNS;
	_commandInitiated = false ;
	return true;
}

///-----------------------------------------------------------
/// Update function for the login state machine.
///-----------------------------------------------------------
bool OSIPS2::_updateDNSLookup( void )
{
	switch ( _state )
	{
		case LOGIN_RESOLVE_DNS: _processDNSLookup( COMMAND_SUCCESS, COMMAND_FAILURE ); break;
		case COMMAND_FAILURE:
			setLastRequestStatus( STATUS_FAILED );
			return true ; // complete, failure
		case COMMAND_SUCCESS:
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ; // complete, success
	}

	return false; // keep working
}

///-----------------------------------------------------------
/// Update function for the login state machine.
///-----------------------------------------------------------
void OSIPS2::_processDNSLookup( int successState, int failureState )
{
	if( !_commandInitiated )
	{
		if( _dnsResolutionThread != NULL )
		{
			delete _dnsResolutionThread;
		}
		// Create our thread, set the DNS host name to look up, and start this baby up
		_dnsResolutionThread = new DNSResolutionThread;
		_dnsResolutionThread->SetHostDNSName( g_rendez_vous_server );
		_dnsResolutionThread->Start( OSThread::HIGH );
		_commandInitiated = true;
		return;
	}

	if( _dnsResolutionThread == NULL )
	{
		setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FAILED_AUTHENTICATION) );
		_state = failureState;
		_commandInitiated = false;
		_dnsResolvedIP[ 0 ] = 0;
		return;
	}

	// Wait until the thread is done running
	if( _dnsResolutionThread->IsRunning() )
	{
		return;
	}
	else if( _dnsResolutionThread->IsResolutionDone() )
	{
		if( !_dnsResolutionThread->DidDNSResolve() )
		{
			// We could not resolve the IP address of the host
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FAILED_AUTHENTICATION) );
			_state = failureState;
			_dnsResolvedIP[ 0 ] = 0;
		}
		else
		{
			// We have the IP address, save it off
			OSIstrncpy( _dnsResolvedIP, _dnsResolutionThread->GetIPAddress(), NUM_ARRAY_ELEMENTS( _dnsResolvedIP ) );
			_state = successState;
		}
		delete _dnsResolutionThread;
		_dnsResolutionThread = NULL;
		_commandInitiated = false;
	}
}

///--------------------------------------------------------------------
/// Handles initial RendezVous login
///--------------------------------------------------------------------
void OSIPS2::_processLogin( int successState, int failureState )
{
	setActiveState(OSIPS2_STATE_LOGIN_REQUESTING_LOGIN);

	RootTransport::UseThreads( false );
	RootTransport::SetWellKnownPortNumber( SONY_PORT_NUMBER );
	if ( !_commandInitiated )
	{
		if ( !_activeAccount )
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_ERROR_FAILED_ACTIVE_ACCOUNT));
			_state				= failureState ;
			return ;
		}

		_currentContext			= qNew ProtocolCallContext();
		if ( !_currentContext )
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_CREATE_CALLING_CONTEXT));
			_state				= failureState ;
			return ;
		}

		_alertContext			= qNew ProtocolCallContext();
		if ( !_alertContext )
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_CREATE_CALLING_CONTEXT));
			_state				= failureState ;
			return ;
		}

		_gatheringUpdateContext	= qNew ProtocolCallContext();
		if ( !_gatheringUpdateContext )
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_CREATE_CALLING_CONTEXT));
			_state				= failureState ;
			return ;
		}

		_rendezVous				= qNew BackEndServices();
		if ( !_rendezVous )
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_CREATE_RENDEZVOUS));
			_state				= failureState ;
			return ;
		}

		OSIchar name[ OSI_PLAYER_NAME_LENGTH*2 ];
		OSIchar password[ OSI_PASSWORD_LENGTH*2 ];
		UTF8Encode( name, _activeAccount->player.name, NUM_ARRAY_ELEMENTS( name ) );
		UTF8Encode( password, _activeAccount->password, NUM_ARRAY_ELEMENTS( password ) );
		int			port		= g_rendez_vous_port ;

		//TraceLog::GetInstance()->SetFlag(TRACE_CONNECTION | TRACE_INIT );
		bool		success		= _rendezVous->Login( _currentContext, &_lastCallQResult, name, password, _dnsResolvedIP, port, &_credentials );

		if ( !success )
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_ERROR_FAILED_VERIFY_CREDENTIALS));
			_state				= failureState ;
			return ;
		}

 		NATTraversalExtension::SetProbeLifetime( 30000 );
		_commandInitiated = true ;
		_resetTimeoutTimer();
		_alertCommandInitiated = false;
		ResetAndStartTimer( _alertChrono );
		_alerts.Clear();
		_alertCheckTime = 0;
		_defaultTimeout			= 120000 ; // 2 minutes
		return ;
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		_defaultTimeout		= OSI_DEFAULT_TIMEOUT ;
		_commandInitiated	= false ;
		if ( _lastCallQResult )
		{
			if ( !_credentials )
			{
				setErrorString( GetOSIStringByEnum( OSIPS2_ERROR_FAILED_VERIFY_CREDENTIALS) );
				_state	= failureState ;
				return ;
			}
			else
			{
				printf("-- OSI -- Logged in successfully.\n");
				_activeAccount->player.id.ps2 = _credentials->GetPrincipalID();
				// Put the players id into the stats list so we can fetch his rating and rank
				_statsListPlayers.clear();
				OSIPlayerStats myPlayerStats;
				myPlayerStats.playerID = _activeAccount->player.id;
				_statsListPlayers.push_back( myPlayerStats );
				//TraceLog::GetInstance()->SetFlag(TRACE_TEARDOWN);
				_rendezVous->AcceptRemoteTraces();
				_state				= successState ;
			}
		}
		else
		{
			switch ( _lastCallQResult.GetReturnCode() )
			{
				case QERROR(RendezVous, ConnectionFailure):		setErrorString( GetOSIStringByEnum(OSIPS2_CONNECTION_FAILURE) );		break;
				case QERROR(RendezVous, NotAuthenticated):		setErrorString( GetOSIStringByEnum(OSIPS2_COULD_NOT_AUTHENTICATE) );	break;
				case QERROR(RendezVous, InvalidUsername):		setErrorString( GetOSIStringByEnum(OSIPS2_INVALID_USER_NAME) );			break;
				case QERROR(RendezVous, InvalidPassword):		setErrorString( GetOSIStringByEnum(OSIPS2_INVALID_PASSWORD) );			break;
				case QERROR(RendezVous, UsernameAlreadyExists): setErrorString( GetOSIStringByEnum(OSIPS2_USER_ALREADY_EXISTS) );		break;
				case QERROR(RendezVous, AccountDisabled):		setErrorString( GetOSIStringByEnum(OSIPS2_ACCOUNT_DISABLED) );			break;
				case QERROR(RendezVous, ConcurrentLoginDenied): setErrorString( GetOSIStringByEnum(OSIPS2_CONCURRENT_LOGIN_ERROR) );	break;
				case QERROR(RendezVous, EncryptionFailure):		setErrorString( GetOSIStringByEnum(OSIPS2_ENCRYPTION_FAILURE) );		break;
				default: setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FAILED_VERIFY_CREDENTIALS) );									break;
			}
			_state = failureState;
		}
	}
}

///--------------------------------------------------------------------
/// After succesful login, update the status string to indicate player is online
///--------------------------------------------------------------------
void OSIPS2::_processUpdateStatus( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		bool result = _accountManagementClient->UpdateStatus(_currentContext, GetStatusString());
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_REQUEST_UPDATE_STATUS) );
			_state = failureState ;
			return;
		}
		_commandInitiated	= true ;
		_resetTimeoutTimer();
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		_commandInitiated	= false ;
		_state				= successState ;
		printf("-- OSI -- Updated Status.\n");
	}
}

///--------------------------------------------------------------------
/// Handles the start up of the Peer Manager
///--------------------------------------------------------------------
void OSIPS2::_processStartPeerManager( int successState, int failureState )
{
	// Start the PeerService (for ping checking suppport)
	_peerService = qNew PeerService;
	bool success = _peerService->Start(_credentials);
	if ( !success )
	{
		setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_START_PEER_MANAGER) );
		_state	= failureState ;
		return;
	}
	else
	{
		_peerManager = qNew PeerManager(_credentials);
		if ( !_peerManager )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_START_PEER_MANAGER) );
			_state	= failureState ;
			return;
		}
		
		printf("-- OSI -- Started Peer Manager.\n");
		_state		= successState ;
	}
}



///--------------------------------------------------------------------
/// Handles all the work for starting up the account client
///--------------------------------------------------------------------
void OSIPS2::_processStartAccountClient( int successState, int failureState )
{
	setActiveState(OSIPS2_STATE_LOGIN_CONNECTING_TO_ACCOUNT_SERVICE);

	_accountManagementClient = qNew AccountManagementClient();
	if ( !_accountManagementClient->Bind(_credentials) )
	{
		setErrorString(GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_CONNECT_TO_ACCOUNT_MANAGEMENT_SERVICE));
		_state	= failureState ;
		return ;
	}

	printf("-- OSI -- Started Account Client.\n");
	_state	= successState ;
}



///--------------------------------------------------------------------
/// Handles all the work for starting up the Friend's Client
///--------------------------------------------------------------------
void OSIPS2::_processStartFriendsClient( int successState, int failureState )
{
	setActiveState(OSIPS2_STATE_LOGIN_CONNECTING_TO_FRIENDS_SERVICE);

	_friendsClient = qNew FriendsClient();
	if ( !_friendsClient->Bind(_credentials) )
	{
		setErrorString(GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_CONNECT_TO_FRIENDS_SERVICE));
		_state		= failureState ;
		return ;
	}

	printf("-- OSI -- Started Friends Client.\n");
	_state		= successState ;
}



///--------------------------------------------------------------------
/// Handles all the work for starting up the MatchMakingClient.
///--------------------------------------------------------------------
void OSIPS2::_processStartMatchMakingClient( int successState, int failureState )
{
	setActiveState(OSIPS2_STATE_LOGIN_CONNECTING_TO_MATCHMAKING_SERVICE);

	_matchMakingClient = qNew MatchMakingClient();
	if (!_matchMakingClient->Bind(_credentials))
	{
		setErrorString(GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_CONNECT_TO_MATCHMAKING_SERVICE));
		_state			= failureState ;
		return ;
	}

	_matchMakingClient->RegisterPrototype( qNew TTLSession() );
	_matchMakingClient->RegisterPrototype( qNew TTLRound() );
	_TTLMatchMakingClient	= qNew TTLMatchMakingClient();
	_matchMakingClient->RegisterExtraProtocol( _TTLMatchMakingClient, 101);

	// This should have been a new state!
	setActiveState(OSIPS2_STATE_LOGIN_CONNECTING_TO_COMPETITION_SERVICE);
	_competitionClient = qNew CompetitionClient(_matchMakingClient);
	if ( ! _competitionClient->Bind(_credentials))
	{
		setErrorString(GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_CONNECT_TO_COMPETITION_CLIENT));
		return ;
	}

	printf("-- OSI -- Started Matchmaking Client.\n");
	_state	= successState ;
}



///---------------------------------------------------------------------
/// Handles all the NATTraversalClient startup.
///---------------------------------------------------------------------
void OSIPS2::_processStartNATClient( int successState, int failureState )
{
	_NATTraversalClient	= qNew NATTraversalClient();
	if (!_NATTraversalClient->Bind( _credentials))
	{
		setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_START_NAT_TRAVERSAL) );
		_state	= failureState ;
		return ;
	}

	printf("-- OSI -- Started NAT Client.\n");
	_state = successState ;
}



///-----------------------------------------------------------
/// Finds the Ladder Gathering, if available.
///-----------------------------------------------------------
bool OSIPS2::_processFindRanking( int successState, int failureState, const String& rankingName, GatheringHolder& rankingHolder )
{
	if ( !_commandInitiated )
	{
		ResultRange defaultRange;
		bool result			= _matchMakingClient->FindByDescription(_currentContext, rankingName, defaultRange, &_lstRankings);
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_REQUEST_FIND_RANKING) );
			_state			= failureState ;
			return true ;
		}
		_commandInitiated	= true ;
		_resetTimeoutTimer();
		return false ;
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		_commandInitiated	= false ;
		if ( _lstRankings.size() == 0)
		{
			rankingHolder	= NULL ;
			_state			= failureState ;
			return true ; // some state machines use true to indicate failures
		}

		rankingHolder		= _lstRankings.front().Release();
		_state				= successState ;
	}

	return false ;
}



///-----------------------------------------------------------
/// Creates the Gathering for player stats.
///-----------------------------------------------------------
bool OSIPS2::_processCreateRanking( int successState, int failureState, const String& rankingName, GatheringHolder& rankingHolder )
{
	if ( !_commandInitiated )
	{
		//// TODO: This should be created automatically by the backend.
		String	rankingLogic( rankingName );
		rankingLogic += "Ranking" ;

		Ranking*	pRanking		= qNew Ranking();
		pRanking->m_strDescription	= _T(rankingName);
		pRanking->m_uiFlags			= GatheringFlags::Persistent;
		pRanking->UseGroupParticipationPolicy(BuiltInGroups::Players);
		pRanking->SetLogic( _T(rankingLogic) );

		rankingHolder				= pRanking ;
		bool result					= _matchMakingClient->RegisterGathering(_currentContext, rankingHolder );
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_CREATE_RANKING) );
			_state					= failureState ;
		}
		else
		{
			_state					= successState ;
			_commandInitiated		= true ;
		}
		return false ;
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		if ( rankingHolder == NULL )
		{
			_state					= successState ;
			_commandInitiated			= false ;
		}
		else
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_CREATE_RANKING) );
			_state					= failureState ;
		}
	}

	return false ;
}



///-----------------------------------------------------------
/// Hosts a competition
///-----------------------------------------------------------
bool OSIPS2::_processHostCompetition( int successState, int failureState, GatheringHolder& holder )
{
	if ( !_commandInitiated )
	{
		bool result			= _competitionClient->HostCompetition( _currentContext, &_lastCallResult, holder->GetID() );
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_HOST_COMPETITION) );
			_state				= failureState ;
			return true ;
		}
		_commandInitiated	= true ;
		_resetTimeoutTimer();
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_HOST_COMPETITION) );
			_state			= failureState ;
		}
		else
		{
			_state			= successState ;
		}
		_commandInitiated	= false ;
	}

	return false ;
}

//#ifdef DIRECTX_PC
/////----------------------------------------------------------
///// Checks to make sure you have a valid cd key
/////----------------------------------------------------------
//void OSIPS2::_processCDKeyCheck( int successState, int failureState )
//{
//	if ( !_commandInitiated )
//	{
//		bool result		= _TTLMatchMakingClient->CallCheckForValidCDKey(_currentContext, &_lastCallResult, _cdkey );
//		if ( !result )
//		{
//			setErrorString( GetOSIStringByEnum( OSIPS2_CONNECTION_FAILURE ));
//			_state				= failureState ;
//		}
//		_commandInitiated	= true ;
//		_resetTimeoutTimer();
//	}
//
//	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
//	{
//		if ( !_lastCallResult )
//		{
//			setErrorString( "invalid cd key or key already in use"  );
//			_state			= failureState ;
//		}
//		else
//		{
//			_state			= successState ;
//		}
//		_commandInitiated	= false ;
//	}
//}
//#endif


///----------------------------------------------------------
/// Special process function for handling hardware connection
/// requests.
///----------------------------------------------------------
#ifndef DIRECTX_PC
bool OSIPS2::_processLastHardwareConnectionRequest( void )
{
	PS2Networking::ConnectionState	state;
	OSIchar							statusMessage[ OSI_STRING_LENGTH_LONG ];
	bool							completed = false;

	setActiveState(OSIPS2_STATE_REQUEST_TYPE_HARDWARE_ACTIVATION);
	state		= PS2Networking::GetConnectionState( statusMessage, sizeof( statusMessage ) );
	completed	= ( state == PS2Networking::Error) || ( state == PS2Networking::Started);

	if ( state == PS2Networking::Error )
	{
		setLastRequestStatus( STATUS_FAILED );
		setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FAILED_HARDWARE_CONNECTION) );
		_requestingHardwareConnection	= false ;
	}
	else if ( state == PS2Networking::Started )
	{
		setLastRequestStatus( STATUS_SUCCEEDED );
		_requestingHardwareConnection	= false ;
	}
	else
	{
		setLastRequestStatus( STATUS_WORKING );
	}

	return completed;
}
#endif // DIRECTX_PC



///------------------------------------------------------
/// Requests the activation of a hardware profile.  This
/// initiates the connection.
///------------------------------------------------------
#ifndef DIRECTX_PC
bool OSIPS2::requestHardwareProfileActivation( OSIHardwareProfileID& profileID )
{
	OSIHardwareProfile*				hardwareProfile			= _hardwareProfileList.findHardwareProfileByID( profileID );
	PS2ConnectionProfile*			rendezvousProfile		= NULL ;
		
	if ( !hardwareProfile )
		return false ;
	
	rendezvousProfile	= _oProfileList.GetProfile( hardwareProfile->id.ps2 ) ;
	rendezvousProfile->Trace(TRACE_ALWAYS);

	PS2Networking::LoadProfileModules( rendezvousProfile );

//	// override insock malloc to use system instead of Quazal. !! Must be after LoadProfileModules
//	sceInsockSetMallocFunction( NULL );
//	sceInsockSetFreeFunction( NULL );

 	PS2Networking::InitiateConnection( rendezvousProfile );
	setRequestType( REQUEST_TYPE_HARDWARE_ACTIVATION );

	return true ;
}



///-------------------------------------------------------
/// Retrieves the default hardware profile.  This should
/// call into PS2Networking.
///--------------------------------------------------------
OSIHardwareProfileID& OSIPS2::getDefaultHardwareProfile( bool loadFromMemoryCard )
{
	return _defaultProfile ;
}
#endif // DIRECTX_PC



///-------------------------------------------------------
/// Initiates the requesting of the account list.  This list
/// is stored locally, however we're making it asynchronous
/// in case the overall process is slow.
///--------------------------------------------------------
bool OSIPS2::requestAccountInfo()
{
	setRequestType( REQUEST_TYPE_ACCOUNT_INFO );
	_state = ACCOUNT_INFO;
	return true ;
}



///--------------------------------------------------------
/// Requests the creation of a new player account.
///--------------------------------------------------------
bool OSIPS2::requestCreateAccount( OSIAccount& account )
{
	_cleanupRendezVous();
	_newAccount = account;

	setRequestType( REQUEST_TYPE_RENDEZVOUS_CREATE_ACCOUNT );

	_state				= LOGIN_LOGIN ;

	_activeAccount			= &_guestAccount ;
    _commandInitiated       = false ;
    return true ;
}


///--------------------------------------------------------
/// Requests editing of the players account
///--------------------------------------------------------
bool OSIPS2::requestEditAccount(const OSIchar *name, const OSIchar *password, OSIAccount& account )
{
	_cleanupRendezVous();
	_newAccount = account;

	OSIAccount loginAccount ;
	loginAccount.id.ps2				= 1 ;
	loginAccount.id.xBox.userId		= 0 ;
	loginAccount.id.xBox.userFlags	= 0 ;

	OSIstrncpy( loginAccount.password, password, NUM_ARRAY_ELEMENTS( loginAccount.password ) );
	OSIstrncpy( loginAccount.player.name, name, NUM_ARRAY_ELEMENTS( loginAccount.player.name ) );
	_accountList.clear();
	_accountList.push_back( loginAccount );
	_activeAccount	= _accountList.findAccountByID( loginAccount.id );
	if ( !_activeAccount )
	{
		setErrorString(GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_FIND_ACCOUNT));
		setLastRequestStatus( STATUS_FAILED );
		return false ;
	}

	setRequestType( REQUEST_TYPE_RENDEZVOUS_EDIT_ACCOUNT );
	_state = LOGIN_LOGIN ;
	return true ;
}


///---------------------------------------------------------
/// Requests the destruction of an account from the Memory card.
///---------------------------------------------------------
bool OSIPS2::requestDestroyAccount( OSIAccountID& accountID )
{
	return true ;
}



///---------------------------------------------------------------------
/// Initiates a login requset
///---------------------------------------------------------------------
bool OSIPS2::requestLogin( const OSIchar* name, const OSIchar* password )
{
	/// This first bit of logic is to create a new account in the account
	/// list and then find it.  This is because we have a lot of legacy code
	/// that assumes the login account is in the account list.  And that
	/// the _activeAccount pointer points to it.  So I'm just hacking here
	/// to make that work.
	OSIAccount loginAccount ;
	loginAccount.id.ps2				= 1 ;
	loginAccount.id.xBox.userId		= 0 ;
	loginAccount.id.xBox.userFlags	= 0 ;

	OSIstrncpy( loginAccount.password, password, NUM_ARRAY_ELEMENTS( loginAccount.password ) );
	OSIstrncpy( loginAccount.player.name, name, NUM_ARRAY_ELEMENTS( loginAccount.player.name ) );
	_accountList.clear();
	_accountList.push_back( loginAccount );
	_activeAccount	= _accountList.findAccountByID( loginAccount.id );
	if ( !_activeAccount )
	{
		setErrorString(GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_FIND_ACCOUNT));
		setLastRequestStatus( STATUS_FAILED );
		return false ;
	}

	/// This kicks off the request.
	setRequestType( REQUEST_TYPE_LOGIN );
	_state				= LOGIN_LOGIN ;
	_commandInitiated	= false ;
	return true ;
}



//-------------------------------------------
// Initiates a logout from RendezVous
//-------------------------------------------
bool OSIPS2::requestLogout()
{
	_logout();

	// reset state here in case the last request had failed
	// since this is a blocking request
	_state = COMMAND_SUCCESS;
	setLastRequestStatus( STATUS_SUCCEEDED );
	setRequestType(REQUEST_TYPE_LOGOUT);
	return true ;
}

///------------------------------------------------------
/// This function logs us out of RendezVous, then cleans
/// up RendezVous afterwards.
///------------------------------------------------------
void OSIPS2::_logout()
{
	_cleanupRendezVous(false);
	_isConnected = false;

	// Clear recent and current player lists
	_recentPlayerList.clear();
	_currentPlayerList.clear();

	// The actual logout
	printf("-- OSI -- Logging out...\n");
	if (_rendezVous != NULL && _credentials != NULL )
	{
		//TraceLog::GetInstance()->SetFlag(TRACE_SCHEDULER);
		_rendezVous->Logout(_credentials);
	}

	if ( _rendezVous != NULL )
	{
		qDelete _rendezVous;
		_rendezVous	= NULL;
	}

	if ( _credentials != NULL )
	{
		//delete _credentials; // this crashes Quazal so they said don't do it.
		_credentials = NULL;
	}
	printf("-- OSI -- Logged out.\n");
}



///-----------------------------------------------------------
/// Update function for the create account state machine.
///-----------------------------------------------------------
bool OSIPS2::_updateCreateAccount( void )
{
	switch ( _state )
	{        // STATE								// UPDATE FUNCTION                // ON SUCCESS GO TO					// ON FAILURE GO TO             // ADDITIONAL PARAMETERS
		case LOGIN_LOGIN:							_processLogin					( LOGIN_START_PEER_MANAGER,				COMMAND_FAILURE );																break;
		case LOGIN_START_PEER_MANAGER:				_processStartPeerManager		( LOGIN_START_NAT_CLIENT,				COMMAND_FAILURE);																break;
		case LOGIN_START_NAT_CLIENT:				_processStartNATClient			( LOGIN_START_ACCOUNT_CLIENT,			COMMAND_FAILURE );																break;
		case LOGIN_START_ACCOUNT_CLIENT:			_processStartAccountClient		( LOGIN_START_FRIENDS_CLIENT,			COMMAND_FAILURE );																break;
		case LOGIN_START_FRIENDS_CLIENT:			_processStartFriendsClient		( LOGIN_START_MATCHMAKING_CLIENT,		COMMAND_FAILURE );																break;
		case LOGIN_START_MATCHMAKING_CLIENT:		_processStartMatchMakingClient	( CREATE_ACCOUNT_CREATE_ACCOUNT,		COMMAND_FAILURE );																break;
		case CREATE_ACCOUNT_CREATE_ACCOUNT:			_processCreateAccount			( LOGOUT_LOGOUT,						COMMAND_FAILURE );																break;
		case LOGOUT_LOGOUT:							_processLogout					( CREATE_ACCOUNT_PREP_NEW_PLAYER,		COMMAND_FAILURE );																break;
		case CREATE_ACCOUNT_PREP_NEW_PLAYER:		_processPrepNewPlayer			( CREATE_ACCOUNT_LOGIN_NEW_PLAYER,		COMMAND_FAILURE );																break;
		case CREATE_ACCOUNT_LOGIN_NEW_PLAYER:		_processLogin					( CREATE_ACCOUNT_START_ACCOUNT_CLIENT,	COMMAND_FAILURE );																break;
		case CREATE_ACCOUNT_START_ACCOUNT_CLIENT:	_processStartAccountClient		( CREATE_ACCOUNT_UPDATE_DATA,			COMMAND_FAILURE );																break;
		case CREATE_ACCOUNT_UPDATE_DATA:			_processUpdateNewPlayerData		( CREATE_ACCOUNT_UPDATE_EMAIL,			COMMAND_FAILURE );																break;
		case CREATE_ACCOUNT_UPDATE_EMAIL:			_processUpdateNewPlayerEmail	( CREATE_ACCOUNT_LOGOUT,				COMMAND_FAILURE );																break;
		case CREATE_ACCOUNT_LOGOUT:					_processLogout					( COMMAND_SUCCESS,						COMMAND_FAILURE );																break;

			
		case COMMAND_FAILURE:			
			_cleanupRendezVous();
			setLastRequestStatus( STATUS_FAILED ); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ; // complete, success
	}

	return false ; // keep working
}



///-----------------------------------------------------------
/// Logout Process
///-----------------------------------------------------------
void OSIPS2::_processLogout( int successState, int failureState)
{
	_logout();
	_state = successState;
}



///-----------------------------------------------------------
/// Update a new players private and public data
///-----------------------------------------------------------
void OSIPS2::_processUpdateNewPlayerData( int successState, int failureState)
{
	if ( !_commandInitiated )
	{	
		Quazal::AnyDataAdapter::GetInstance()->RegisterPrototype(qNew Quazal::EidosAccountPrivate);
		Quazal::AnyDataAdapter::GetInstance()->RegisterPrototype(qNew Quazal::EidosAccountPublic);
		_newAccountPrivateData = qNew Quazal::EidosAccountPrivate();
		_newAccountPublicData = qNew Quazal::EidosAccountPublic();

		_newAccountPrivateData->m_flags = _newAccount.flags;
		_newAccountPrivateData->m_dateOfBirth = Quazal::DateTime(_newAccount.birthdayYear, _newAccount.birthdayMonth, _newAccount.birthdayDay, 1, 00, 00);
		_newAccountPrivateData->m_postalCode = _newAccount.postalCode;
		_newAccountPublicData->m_country = _newAccount.countryCode;

		bool result =  _accountManagementClient->UpdateCustomData(_currentContext, &_lastCallQResult, _newAccountPublicData, _newAccountPrivateData);
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_CREATE_ACCOUNT) );
			_state				= failureState ;
			return;
		}
		_commandInitiated	= true ;
		_resetTimeoutTimer();
	}
	
	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		if (!QSUCCEEDED(_currentContext->GetOutcome().GetReturnCode())) 
		{
			qResult outcome = _currentContext->GetOutcome();
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_CREATE_ACCOUNT) );
			_state			= failureState ;
		}
		else
		{
			_state			= successState ;
		}
		_commandInitiated	= false ;
	}
}



///-----------------------------------------------------------
/// Update a new players email address
///-----------------------------------------------------------
void OSIPS2::_processUpdateNewPlayerEmail( int successState, int failureState)
{
	if ( !_commandInitiated )
	{	
		const Quazal::String str(_newAccount.email);
		bool result =  _accountManagementClient->UpdateAccountEmail(_currentContext, &_lastCallQResult, str);
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_CREATE_ACCOUNT) );
			_state				= failureState ;
			return;
		}
		_commandInitiated	= true ;
		_resetTimeoutTimer();
	}
	
	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		if (!QSUCCEEDED(_currentContext->GetOutcome().GetReturnCode())) 
		{ 
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_CREATE_ACCOUNT) );
			_state			= failureState ;
		}
		else
		{
			_state			= successState ;
		}
		_commandInitiated	= false ;
	}
}


///-----------------------------------------------------------
/// Update account password
///-----------------------------------------------------------
void OSIPS2::_processUpdatePassword( int successState, int failureState)
{
	if ( !_commandInitiated )
	{	
		const Quazal::String str(_newAccount.email);
		char password[ OSI_PASSWORD_LENGTH*2 ];
		UTF8Encode( password, _newAccount.password, NUM_ARRAY_ELEMENTS( password ) );
		bool result =  _accountManagementClient->ChangePassword(_currentContext, &_lastCallResult, password);
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_CREATE_ACCOUNT) );
			_state				= failureState ;
			return;
		}
		_commandInitiated	= true ;
		_resetTimeoutTimer();
	}
	
	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		if (!QSUCCEEDED(_currentContext->GetOutcome().GetReturnCode())) 
		{ 
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_CREATE_ACCOUNT) );
			_state			= failureState ;
		}
		else
		{
			_state			= successState ;
		}
		_commandInitiated	= false ;
	}
}


///-----------------------------------------------------------
/// Get the new player ready to log in
///-----------------------------------------------------------
void OSIPS2::_processPrepNewPlayer( int successState, int failureState)
{
	printf("-- OSI -- Preparing New Account.\n");
	_activeAccount = &_newAccount;
	_state = successState;
}


///-----------------------------------------------------------
/// Create Account Process
///-----------------------------------------------------------
void OSIPS2::_processCreateAccount( int successState, int failureState)
{
	if ( !_commandInitiated )
	{
		String emailAddress;
		if( _newAccountEmail )
			emailAddress = _newAccountEmail;
		else
			emailAddress = _T("name@name.com");

		setActiveState(OSIPS2_STATE_CREATE_ACCOUNT_REQUEST_ACCOUNT_CREATION);

		_currentContext->Reset();
		_lastCallQResult = false;
		char name[ OSI_PLAYER_NAME_LENGTH*2 ];
		char password[ OSI_PASSWORD_LENGTH*2 ];
		UTF8Encode( name, _newAccount.player.name, NUM_ARRAY_ELEMENTS( name ) );
		UTF8Encode( password, _newAccount.password, NUM_ARRAY_ELEMENTS( password ) );
		if ( !_accountManagementClient->CreateAccount( _currentContext, &_lastCallQResult, name, password, BuiltInGroups::Players, emailAddress )) 
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_CREATE_ACCOUNT) );
			return;
		}

		_commandInitiated = true;
		_resetTimeoutTimer();
		return;
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		_commandInitiated = false;


		int rc = _currentContext->GetOutcome().GetReturnCode();
		if (!QSUCCEEDED(rc)) 
		{ 
			switch ( _currentContext->GetOutcome().GetReturnCode() )
			{
				case QERROR(RendezVous, UsernameAlreadyExists): 
					setErrorString( GetOSIStringByEnum(OSIPS2_USER_ALREADY_EXISTS) );
					break;

				case QERROR(PythonCore, ValidationError):
					setErrorString( GetOSIStringByEnum(OSIPS2_FILTER_ERROR) );
					break;
			}
			_state = failureState;
			return;
		}

		if ( !_lastCallQResult )
		{
			switch ( _lastCallQResult.GetReturnCode() )
			{
				case QERROR(RendezVous, ConnectionFailure):		setErrorString( GetOSIStringByEnum(OSIPS2_CONNECTION_FAILURE) );		break;
				case QERROR(RendezVous, NotAuthenticated):		setErrorString( GetOSIStringByEnum(OSIPS2_COULD_NOT_AUTHENTICATE) );	break;
				case QERROR(RendezVous, InvalidUsername):		setErrorString( GetOSIStringByEnum(OSIPS2_INVALID_USER_NAME) );			break;
				case QERROR(RendezVous, InvalidPassword):		setErrorString( GetOSIStringByEnum(OSIPS2_INVALID_PASSWORD) );			break;
				case QERROR(RendezVous, UsernameAlreadyExists): setErrorString( GetOSIStringByEnum(OSIPS2_USER_ALREADY_EXISTS) );		break;
				case QERROR(RendezVous, AccountDisabled):		setErrorString( GetOSIStringByEnum(OSIPS2_ACCOUNT_DISABLED) );			break;
				case QERROR(RendezVous, ConcurrentLoginDenied): setErrorString( GetOSIStringByEnum(OSIPS2_CONCURRENT_LOGIN_ERROR) );	break;
				case QERROR(RendezVous, EncryptionFailure):		setErrorString( GetOSIStringByEnum(OSIPS2_ENCRYPTION_FAILURE) );		break;
				case QERROR(PythonCore, ValidationError):		setErrorString( GetOSIStringByEnum(OSIPS2_FILTER_ERROR) );				break;
				default: setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_CREATE_ACCOUNT) );									break;
			}
			_state = failureState;
			return;
		}

		printf("-- OSI -- Account Created\n");
		_state = successState;
	}
}

///-----------------------------------------------------------
/// Update function for the create account state machine.
///-----------------------------------------------------------
bool OSIPS2::_processEditAccount( void )
{
	switch ( _state )
	{        // STATE								// UPDATE FUNCTION                // ON SUCCESS GO TO					// ON FAILURE GO TO             // ADDITIONAL PARAMETERS
		case LOGIN_LOGIN:							_processLogin					( LOGIN_START_ACCOUNT_CLIENT,			COMMAND_FAILURE );																break;
		case LOGIN_START_ACCOUNT_CLIENT:			_processStartAccountClient		( CREATE_ACCOUNT_UPDATE_DATA,			COMMAND_FAILURE );		
		case CREATE_ACCOUNT_UPDATE_DATA:			_processUpdateNewPlayerData		( CREATE_ACCOUNT_UPDATE_EMAIL,			COMMAND_FAILURE );																break;
		case CREATE_ACCOUNT_UPDATE_EMAIL:			_processUpdateNewPlayerEmail	( EDIT_ACCOUNT_PASSWORD,				COMMAND_FAILURE );																break;
		case EDIT_ACCOUNT_PASSWORD:					_processUpdatePassword			( LOGOUT_LOGOUT,						COMMAND_FAILURE );																break;
		case LOGOUT_LOGOUT:							_processLogout					( COMMAND_SUCCESS,						COMMAND_FAILURE );																break;

			
		case COMMAND_FAILURE:			
			_cleanupRendezVous();
			setLastRequestStatus( STATUS_FAILED ); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ; // complete, success
	}

	return false ; // keep working
}

























































/// ***************************************************************************
///	 ____  _        _   _     _   _          
///	/ ___|| |_ __ _| |_(_)___| |_(_) ___ ___ 
///	\___ \| __/ _` | __| / __| __| |/ __/ __|
///	 ___) | || (_| | |_| \__ \ |_| | (__\__ \
///	|____/ \__\__,_|\__|_|___/\__|_|\___|___/
///                                         
/// ***************************************************************************

///------------------------------------------------------
///------------------------------------------------------
TTLStats* OSIPS2::_getRecievedStatsByPid( OSIPlayerID id )
{
	qList<TTLStats>::iterator statsListIter = _retrievedStatsList.begin();
	while ( statsListIter != _retrievedStatsList.end() )
	{
		TTLStats& ttlstats = *statsListIter;
		if ( ttlstats.m_pidPlayer == id.ps2 )
		{
			g_debugOutput.TSPrint( g_debugOutput.Category("OSI"), "stat pid %d, size=%d", id.ps2, ttlstats.m_lstValues.size() );
			return &ttlstats;
		}

		statsListIter++;
	}

	return NULL;
}


///------------------------------------------------------
/// Populates the _statsListPlayers from the _retrievedStatsList
///------------------------------------------------------
void OSIPS2::_populatePlayerStatsList()
{
	OSIPlayerStats* playerStats;
	_statsListPlayers.begin();

	int playerCount = 0;
	int statCount = 0;

	while ( (playerStats = _statsListPlayers.getCurrent()) != NULL )
	{
		playerCount++;

		TTLStats* stats = _getRecievedStatsByPid(playerStats->playerID);
		playerStats->clearStats();
		if ( !stats ) 
		{
			_statsListPlayers.next();
			continue;
		}

		playerStats->clanRating = stats->m_clanRating;
		playerStats->clanKills = stats->m_clanKills;

		qList<float>::iterator iter = stats->m_lstValues.begin();
		for ( int i=0; i<OSIPlayerStats::NUM_PLAYERSTATS; i++ )
		{
			statCount++;
			playerStats->statsArray[i] = (float)(*iter);
			iter++;
		}

		// If there are stats for the logged in player, add them to our account player info
		if( playerStats->playerID == _activeAccount->player.id )
		{
			_activeAccount->player.statRanking = int(playerStats->getStat( OSIPlayerStats::PLAYERSTAT_NA_RANK ));
			_activeAccount->player.statRating = int(playerStats->getStat( OSIPlayerStats::PLAYERSTAT_USER_RATING ));
		}

		_statsListPlayers.next();
	}

	g_debugOutput.TSPrint( g_debugOutput.Category("OSI"), "_populatePlayerStats: %d players, %d stats", playerCount, statCount );
}



///------------------------------------------------------
/// Gets all stats for the list of players
///------------------------------------------------------
void OSIPS2::_processGetStatsForPlayers( int successState, int failureState )
{
	qList<PrincipalID> lstParticipants;

	if ( !_commandInitiated )
	{	
		g_debugOutput.TSPrint( g_debugOutput.Category("OSI"), "_processGetStatsForPlayers" );
		// Create the list of playerIDs to retrieve stats for
		_statsListPlayers.begin();
		while ( _statsListPlayers.getCurrent() != NULL )
		{
			OSIPlayerStats *playerStats = _statsListPlayers.getCurrent();
			lstParticipants.push_back(playerStats->playerID.ps2);
			_statsListPlayers.next();
		}

		bool result =  _TTLMatchMakingClient->CallGetStatsForPlayers(_currentContext, lstParticipants, &_retrievedStatsList);
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_GET_NONLADDER_STATS) );
			_state				= failureState ;
			return;
		}

		_commandInitiated	= true ;
		_resetTimeoutTimer();
	}
	
	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		g_debugOutput.TSPrint( g_debugOutput.Category("OSI"), "stat elements=%d", _retrievedStatsList.size() );
		_populatePlayerStatsList();
		printf("-- OSI -- Retrieved All Stats for Player List.\n");
		_state			= successState ;
		_commandInitiated	= false ;
	}
}

///------------------------------------------------------
/// Processing state machine for player stats
///------------------------------------------------------
bool OSIPS2::_updateGetPlayersStats( void )
{
	if ( _statsListPlayers.count() == 0 )
		_state = COMMAND_SUCCESS; // Empty list, just bail out.

	switch ( _state )
	{        // STATE               // UPDATE FUNCTION               // ON SUCCESS GO TO				// ON FAILURE GO TO 
		case STATS_GET_ALL_STATS:	_processGetStatsForPlayers		( COMMAND_SUCCESS,					COMMAND_FAILURE );			break ;
		case COMMAND_FAILURE:			
			g_debugOutput.TSPrint( g_debugOutput.Category("OSI"), "_updateGetPlayersStats FAIL" );
			setLastRequestStatus( STATUS_FAILED ); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:
			g_debugOutput.TSPrint( g_debugOutput.Category("OSI"), "_updateGetPlayersStats OK" );
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ; // complete, success
	}

	return false ; // keep working
}



///----------------------------------------------------------------
/// Request stats for the players in the list
///----------------------------------------------------------------
bool OSIPS2::requestStatsPlayerList( OSIPlayerStatsList& players )
{
	_statsListPlayers = players;
	_statsListPlayers.begin();
	_state = STATS_GET_ALL_STATS;
	setRequestType(REQUEST_TYPE_GET_PLAYERS_STATS);
	return true;	
}








///------------------------------------------------------
/// Gets all stats for the list of players
///------------------------------------------------------
void OSIPS2::_processGetLadderStats( int successState, int failureState )
{
	if ( !_commandInitiated )
	{	
		bool result =  _TTLMatchMakingClient->CallGetLadderStats(_currentContext, _pivotID.ps2, _rankCount, &_retrievedStatsList);
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_GET_NONLADDER_STATS) );
			_state				= failureState ;
			return;
		}

		_commandInitiated	= true ;
		_resetTimeoutTimer();
	}
	
	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		_populateTopPlayerStatsList(); // This function will serve our purpose, eventhough it's ment for Top Players.
		printf("-- OSI -- Retrieved Ladder Rankings wtih pivot=%d\n", _pivotID.ps2 );
		_state			= successState ;
		_commandInitiated	= false ;
	}
}


///------------------------------------------------------
/// Processing state machine for ladder stats
///------------------------------------------------------
bool OSIPS2::_updateGetLadderStats( void )
{
	switch ( _state )
	{        // STATE               // UPDATE FUNCTION               // ON SUCCESS GO TO				// ON FAILURE GO TO 
		case STATS_GET_LADDER_PLAYERS:	_processGetLadderStats		( COMMAND_SUCCESS,					COMMAND_FAILURE );			break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus( STATUS_FAILED ); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ; // complete, success
	}

	return false ; // keep working
}


///----------------------------------------------------------------
/// Get the stats for the players around the pivotPlayer.
///----------------------------------------------------------------
bool OSIPS2::requestStatsPlayerLadder( OSIPlayerID& pivotPlayer, const int numRanks )
{
	_state = STATS_GET_LADDER_PLAYERS;
	_rankCount = numRanks;
	_pivotID = pivotPlayer;
	setRequestType(REQUEST_TYPE_GET_LADDER_STATS);
	return true;
}





///------------------------------------------------------
/// Populates the _statsListPlayers from the _retrievedStatsList
///------------------------------------------------------
void OSIPS2::_populateTopPlayerStatsList()
{
	qList<TTLStats>::iterator statsListIter = _retrievedStatsList.begin();
	_statsListPlayers.clear();
	while ( statsListIter != _retrievedStatsList.end() )
	{
		TTLStats& ttlstats = *statsListIter;

		OSIPlayerStats playerStats;
		UTF8Decode(playerStats.name, ttlstats.m_strPlayer.CStr(), OSI_PLAYER_NAME_LENGTH);
		UTF8Decode(playerStats.clanAbbr, ttlstats.m_strClanAbbr.CStr(), OSI_CLAN_TAG_LENGTH);
		playerStats.clanRating = ttlstats.m_clanRating;
		playerStats.clanKills = ttlstats.m_clanKills;
		playerStats.playerID.ps2 = ttlstats.m_pidPlayer;

		qList<float>::iterator iter = ttlstats.m_lstValues.begin();
		for ( int i=0; i<OSIPlayerStats::NUM_PLAYERSTATS; i++ )
		{
			playerStats.statsArray[i] = (float)(*iter);
			iter++;
		}

		statsListIter++;
		_statsListPlayers.push_back(playerStats);
	}
}

///------------------------------------------------------
/// Processing function for getting the top N players stats
///------------------------------------------------------
bool OSIPS2::_updateGetTopPlayersStats( void )
{
	switch ( _state )
	{        // STATE               // UPDATE FUNCTION               // ON SUCCESS GO TO				// ON FAILURE GO TO 
		case STATS_GET_TOP_PLAYERS:	_processGetTopPlayersStats		( COMMAND_SUCCESS,					COMMAND_FAILURE );			break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus( STATUS_FAILED ); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ; // complete, success
	}

	return false ; // keep working
}



///------------------------------------------------------
/// Gets all stats for the list of players
///------------------------------------------------------
void OSIPS2::_processGetTopPlayersStats( int successState, int failureState )
{
	if ( !_commandInitiated )
	{	
		_retrievedStatsList.clear();
		bool result =  _TTLMatchMakingClient->CallGetTopPlayers(_currentContext, _rankCount, &_retrievedStatsList);
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_GET_NONLADDER_STATS) );
			_state				= failureState ;
			return;
		}

		_commandInitiated	= true ;
		_resetTimeoutTimer();
	}
	
	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		_populateTopPlayerStatsList();
		printf("-- OSI -- Retrieved Top Players' Stats.\n");
		_state			= successState ;
		_commandInitiated	= false ;
	}
}


///----------------------------------------------------------------
/// Get stats for players in order starting at firstRank
///----------------------------------------------------------------
bool OSIPS2::requestStatsTopPlayers( const int numRanks )
{
	_state = STATS_GET_TOP_PLAYERS;
	_rankCount = numRanks;
	setRequestType(REQUEST_TYPE_GET_TOP_PLAYERS_STATS);
	return true;
}






///------------------------------------------------------
/// Populates the _statsListClans from the _retrievedClanList
///------------------------------------------------------
void OSIPS2::_populateTopClansStatsList()
{
	qList<EidosClanProps>::iterator statsListIter = _retrievedClansList.begin();
	_statsListClans.clear();
	while ( statsListIter != _retrievedClansList.end() )
	{
		EidosClanProps& clanStats = *statsListIter;

		OSIClanInfo clanInfo;
		UTF8Decode(clanInfo.name, clanStats.m_ClanName.CStr(), OSI_CLAN_NAME_LENGTH);
		UTF8Decode(clanInfo.tag, clanStats.m_ClanAbbr.CStr(), OSI_CLAN_TAG_LENGTH);
		clanInfo.clanID.ps2 = clanStats.m_ClanID;

		clanInfo.stats.setStat(OSIClanStats::CLANSTAT_RANK, (float)clanStats.m_ClanRank);
		clanInfo.stats.setStat(OSIClanStats::CLANSTAT_RATING, (float)clanStats.m_ClanRating);
		clanInfo.stats.setStat(OSIClanStats::CLANSTAT_KILLS, (float)clanStats.m_ClanKills);
		_statsListClans.push_back(clanInfo);

		statsListIter++;
	}
}

///------------------------------------------------------
/// Processing function for getting the top N clan stats
///------------------------------------------------------
bool OSIPS2::_updateGetTopClansStats( void )
{
	switch ( _state )
	{        // STATE               // UPDATE FUNCTION               // ON SUCCESS GO TO				// ON FAILURE GO TO 
		case STATS_GET_TOP_CLANS:	_processGetTopClansStats		( COMMAND_SUCCESS,					COMMAND_FAILURE );			break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus( STATUS_FAILED ); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ; // complete, success
	}

	return false ; // keep working
}



///------------------------------------------------------
/// Gets all stats for the list of players
///------------------------------------------------------
void OSIPS2::_processGetTopClansStats( int successState, int failureState )
{
	if ( !_commandInitiated )
	{	
		_retrievedClansList.clear();
		bool result =  _TTLMatchMakingClient->CallGetTopClans(_currentContext, _rankCount, &_retrievedClansList);
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_GET_NONLADDER_STATS) );
			_state				= failureState ;
			return;
		}

		_commandInitiated	= true ;
		_resetTimeoutTimer();
	}
	
	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		_populateTopClansStatsList();
		printf("-- OSI -- Retrieved Top Clans' Stats.\n");
		_state			= successState ;
		_commandInitiated	= false ;
	}
}


///----------------------------------------------------------------
/// Get stats for players in order starting at firstRank
///----------------------------------------------------------------
bool OSIPS2::requestStatsTopClans( int numRanks )
{
	_state = STATS_GET_TOP_CLANS;
	_rankCount = numRanks;
	setRequestType(REQUEST_TYPE_GET_TOP_CLANS_STATS);
	return true;
}











































/// ***************************************************************************
///	 _____     _                _     
///	|  ___| __(_) ___ _ __   __| |___ 
///	| |_ | '__| |/ _ \ '_ \ / _` / __|
///	|  _|| |  | |  __/ | | | (_| \__ \
///	|_|  |_|  |_|\___|_| |_|\__,_|___/
///                                  
/// ***************************************************************************
///--------------------------------------------
/// Requests a new friend list from RendezVous.
/// Returns true if the request was initiated.
///--------------------------------------------
bool OSIPS2::requestFriendList()
{
	/// Make old data invalid
	_friendList.setNetworkDataReady( false );

	/// Set up the state (and sub-state) machine(s)
	setRequestType( REQUEST_TYPE_RENDEZVOUS_FRIENDS_LIST );
	_state				= FRIENDS_GET_LIST ;
	_commandInitiated	= false ;

	return true;
}

///----------------------------------------------------
/// Processing state machine for a Friends List request
/// 
/// Returns true if the request is finished processing
///	and an outcome (either success or failure) is known.
///----------------------------------------------------
bool OSIPS2::_updateGetFriends( void )
{
	switch ( _state )
	{        // STATE                           // UPDATE FUNCTION                // ON SUCCESS GO TO               // ON FAILURE GO TO             // ADDITIONAL PARAMETERS
		case FRIENDS_GET_LIST:					_processFriendsGetFriends		( COMMAND_SUCCESS,					COMMAND_FAILURE );																break ;
		default:
		case COMMAND_FAILURE:			
			setLastRequestStatus( STATUS_FAILED ); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ; // complete, success
	}

	return false ; // keep working
}


///--------------------------------------------------------
/// Kicks off a request to RendezVous for an updated friend list.
///
/// Returns true if the request failed.
///--------------------------------------------------------
void OSIPS2::_processFriendsGetFriends( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		/// Get the list of friends
		bool result			= _TTLMatchMakingClient->CallFriendsQuery(_currentContext, g_translator.GetLanguage(), &_lstFriends);
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FRIENDS_REQUEST_FRIENDS_LIST_FAILED) );
			_state			= failureState ;
			return ;
		}
		_commandInitiated	= true ;
		_resetTimeoutTimer();
		return ;
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		_commandInitiated	= false ;
		_state				= successState ;
		_friendList.setNetworkDataReady( true ); // data is ready for primetime
	}
}


///--------------------------------------------------------
/// Populates the OSI::_friendsList
///--------------------------------------------------------
void OSIPS2::_populateFriendList( void )
{
	_friendList.clear();
	qList<EidosFriend>::iterator friendIter ;
	for ( friendIter = _lstFriends.begin(); friendIter != _lstFriends.end(); ++friendIter )
	{
		EidosFriend& eidosFriend	= (*friendIter);
		OSIPlayer playerData ;

		// Copy EidosFriend data across to OSIPlayer data
		const String& friendName	= eidosFriend.m_friendData.GetName();
		UTF8Decode( playerData.name, friendName.CStr(), NUM_ARRAY_ELEMENTS( playerData.name ) );
		playerData.id.ps2										= eidosFriend.m_friendData.GetPrincipalID() ;
		playerData.isAFriend									= false ;
		playerData.hasRequestedMyFriendship						= false ;
		playerData.haveRequestedTheirFriendship					= false ;
		playerData.lookingToPlay								= eidosFriend.m_isLookingToPlay ;

		String statusString = eidosFriend.m_friendData.GetStatus();
		if ( IsOnlineFromStatusString( statusString.CStr() ) )
		{
			playerData.state.isOnline							= true ;
			playerData.state.isVoiceAvailable					= false ;
		}
		OSIchar decodedStatus[ 256 ];
		UTF8Decode( decodedStatus, statusString.CStr(), NUM_ARRAY_ELEMENTS( decodedStatus ) );
		playerData.SetStatusString( decodedStatus );

		switch ( eidosFriend.m_friendData.GetRelationship() )
		{
			case 0:		playerData.hasRequestedMyFriendship		= true ; break ;
			case 1:		playerData.haveRequestedTheirFriendship	= true ; break ;
			case 2:		playerData.isAFriend					= true ; break ;
			default: break ;
		}

		/// This should iteratre through a single match, if the friend is in a match
		qList<GatheringHolder>::iterator gatheringIter ;
		for ( gatheringIter = eidosFriend.m_lstGatherings.begin(); gatheringIter != eidosFriend.m_lstGatherings.end(); ++gatheringIter )
		{
			OSIMatch			match ;
			GatheringHolder&	holder	= (*gatheringIter);
			TTLSession*			session	= (TTLSession*)holder.Get();

			_sessionToMatch( *session, match );
			playerData.currentMatchList->push_back( match );
			playerData.state.isPlayingGame						= true ;
		}

		UTF8Decode( playerData.clanInfo.tag, eidosFriend.m_strClanAbbr.CStr(), NUM_ARRAY_ELEMENTS( playerData.clanInfo.tag ));

		_friendList.push_back( playerData );
	}

	_friendList.setNetworkDataReady( false ); // data is ready for primetime
	_lstFriends.resize( 0 ); // clear out all memory
}


///--------------------------------------------------------
/// Returns true upon failure.
///--------------------------------------------------------
bool OSIPS2::_processFriendsListRequestUpdateInvitations( void )
{
	setActiveState( OSIPS2_STATE_FRIENDSLIST_REQUEST_UPDATE_INVITATIONS );

	/// Clear the invitations list
	_lstInvitations.clear();

	/// Make sure we're connected
	if( !_matchMakingClient->IsConnected() )
		return true; // failure - OSIFIXME: set error conditions, error strings, etc

	/// Check if we have any current invitations
	ResultRange defaultRange;
	if( !_matchMakingClient->FindInvitations( _currentContext, defaultRange, &_lstInvitations ) )
		return true; // failure - OSIFIXME: set error conditions, error strings, etc

	/// Advance the sub-state machine to the next stage
//	setFriendsListRequestState( STATE_FRIENDSLIST_WAITING_ON_UPDATE_INVITATIONS );
	return false; // success
}



///--------------------------------------------------------
/// _processFriendsListWaitingOnUpdateInvitations
///
/// Returns true upon failure.
///--------------------------------------------------------
bool OSIPS2::_processFriendsListWaitingOnUpdateInvitations( void )
{
	setActiveState( OSIPS2_STATE_FRIENDSLIST_WAITING_ON_UPDATE_INVITATIONS );

	bool isOutcomeKnown	= _currentContext->Wait( _defaultTimeSlice );
	if ( isOutcomeKnown )
	{
		/// Iterate through each invitation type and update the friend list accordingly
		qList<GatheringHolder>::iterator iter;
		for( iter = _lstInvitations.begin(); iter != _lstInvitations.end(); ++ iter )  
		{
			// Find the right friend to set the flag properly
			_friendList.begin();
			OSIPlayer* scan = _friendList.getCurrent();
			for ( ; scan != NULL; _friendList.next(), scan = _friendList.getCurrent() )
			{
				scan->lookingToPlay = false;

				/// Balk if this friend ID doesn't match the invite ID
				if( scan->id.ps2 != (*iter)->GetOwnerPID() )
					continue;

				/// Set either <lookingToPlay> or <lookingToChat>, depending on the invitiation type
				if( (*iter)->GetGatheringType() == _T( "TTLSession" ) )
				{
					scan->lookingToPlay = true;
				}
				else
				{
//					assert( 0 ); // what does this mean?  I removed lookingToChat because we don't use it.  -jm
					//scan->lookingToChat = true;
					scan->lookingToPlay = true;
				}
			}
		}

		_friendList.setNetworkDataReady( true ); // data is ready for primetime
		return false; // success
	}

	// Handle Timeouts
	static int waitCount = 0;
    waitCount ++;
	if ( waitCount > _defaultTimeout )
	{
		waitCount = 0;
		setLastRequestStatus( STATUS_FAILED );
		setErrorString( GetOSIStringByEnum( OSIPS2_DNAS_UNSPECIFIED_ERROR ) ); // OSIFIXME: create error strings for each of these
		return true;
	}

	return false;
}


//---------------------------------------------
// Requests RendezVous to authorize friendship
// from the specified player.  
//
// Returns true if the request was successfully initiated.
//---------------------------------------------
bool OSIPS2::requestAuthorizeTheirFriendship( OSIPlayerID& id )
{
	if ( _commandInitiated )
	{
		setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_REQUEST_ALREADY_IN_PROGRESS) );
		return false ;
	}

	if ( !_TTLMatchMakingClient->CallFriendAccept( _currentContext, &_lastCallResult, id.ps2 ) )
	{
		setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FRIENDS_REQUEST_AUTHORIZE_FAILED) );
		setLastRequestStatus( STATUS_FAILED );
		return false ;
	}

	setRequestType( REQUEST_TYPE_AUTHORIZE_FRIENDSHIP );
	_state				= FRIENDS_ACCEPT_FRIENDSHIP ;
	_commandInitiated	= true ;
	return true ;
}



///--------------------------------------------
/// Update function for authorizing friendship.  Returns false if
/// the command is working, true when it completes.
///--------------------------------------------
bool OSIPS2::_updateAuthorizeFriendship( void )
{
	if ( _checkForCommandCompletionOrTimeout( _state, COMMAND_FAILURE ) )
	{
		_commandInitiated = false ;
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_AUTHORIZE_FRIENDSHIP) );
			setLastRequestStatus( STATUS_FAILED );
		}
		else
		{
			setLastRequestStatus( STATUS_SUCCEEDED );
		}
		return true ;
	}

	return false ;
}


//----------------------------------------------
// Requests RendezVous to deny the friendship request
// from the specified player.
//
// Returns true if the request was initiated.
//----------------------------------------------
bool OSIPS2::requestDenyTheirFriendship( OSIPlayerID& id )
{
	if ( _commandInitiated )
	{
		setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_REQUEST_ALREADY_IN_PROGRESS) );
		return false ;
	}

	if ( !_TTLMatchMakingClient->CallFriendReject( _currentContext, &_lastCallResult, id.ps2, false ) )
	{
		setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FRIENDS_REQUEST_DENY_FAILED) );
		setLastRequestStatus( STATUS_FAILED );
		return false ;
	}

	setRequestType( REQUEST_TYPE_AUTHORIZE_FRIENDSHIP );
	_state				= FRIENDS_DENY_FRIENDSHIP ;
	_commandInitiated	= true ;
	return true ;
}


///--------------------------------------------
/// Update function for denying friendship.  Returns false if
/// the command is working, true when it completes.
///--------------------------------------------
bool OSIPS2::_updateDenyFriendship( void )
{
	if ( _checkForCommandCompletionOrTimeout( _state, COMMAND_FAILURE ) )
	{
		_commandInitiated = false ;
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_DENY_FRIENDSHIP) );
			setLastRequestStatus( STATUS_FAILED );
		}
		else
		{
			setLastRequestStatus( STATUS_SUCCEEDED );
		}
		return true ;
	}

	return false ;
}



//----------------------------------------------
// Requests RendezVous to block all friendship requests
// from the specified player.
//
// Returns true if the request was initiated.
//----------------------------------------------
bool OSIPS2::requestBlockTheirFriendship( OSIPlayerID& id )
{
	if ( _commandInitiated )
	{
		setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FRIENDS_REQUEST_BLOCK_FAILED) );
		return false ;
	}

	if ( !_TTLMatchMakingClient->CallFriendReject( _currentContext, &_lastCallResult, id.ps2, true ) )
	{
		setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_BLOCK_FRIENDSHIP) );
		setLastRequestStatus( STATUS_FAILED );
		return false ;
	}

	setRequestType( REQUEST_TYPE_AUTHORIZE_FRIENDSHIP );
	_state				= FRIENDS_BLOCK_FRIENDSHIP ;
	_commandInitiated	= true ;
	return true ;
}



///--------------------------------------------
/// Update function for denying friendship.  Returns false if
/// the command is working, true when it completes.
///--------------------------------------------
bool OSIPS2::_updateBlockFriendship( void )
{
	if ( _checkForCommandCompletionOrTimeout( _state, COMMAND_FAILURE ) )
	{
		_commandInitiated = false ;
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_BLOCK_FRIENDSHIP) );
			setLastRequestStatus( STATUS_FAILED );
		}
		else
		{
			setLastRequestStatus( STATUS_SUCCEEDED );
		}
		return true ;
	}

	return false ;
}



///----------------------------------------------
/// Requests RendezVous release any block(s)
///  placed on the specified player.
///
/// Returns true if the request was initiated.
///----------------------------------------------
bool OSIPS2::requestUnBlockTheirFriendship( OSIPlayerID& id )
{
	if ( !_commandInitiated )
	{
		bool result = false ;//_friendsClient->ClearRelationship( _currentContext, &_lastCallResult, id.ps2 );
		_commandInitiated = true ;
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FRIENDS_REQUEST_UNBLOCK_FAILED) );
			setLastRequestStatus( STATUS_FAILED );
			return false ;
		}
		_commandInitiated	= false ;
		setRequestType( REQUEST_TYPE_UNBLOCK_FRIENDSHIP );
		return true ;
	}

	return true;
}



///--------------------------------------------
/// Update function for denying friendship.  Returns false if
/// the command is working, true when it completes.
///--------------------------------------------
bool OSIPS2::_updateUnblockFriendship( void )
{
	if ( _checkForCommandCompletionOrTimeout( _state, COMMAND_FAILURE ) )
	{
		_commandInitiated = false ;
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_UNBLOCK_FRIENDSHIP) );
			setLastRequestStatus( STATUS_FAILED );
		}
		else
		{
			setLastRequestStatus( STATUS_SUCCEEDED );
		}
		return true ;
	}

	return false ;
}



///----------------------------------------------
/// Requests RendezVous to offer friendship to the
/// specified player.
///
/// Returns true
///----------------------------------------------
bool OSIPS2::requestOfferMyFriendship( OSIPlayerID& id )
{
	if ( _commandInitiated )
	{
		setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_REQUEST_ALREADY_IN_PROGRESS) );
		return false ;
	}

	if ( !_TTLMatchMakingClient->CallFriendInviteByID( _currentContext, &_lastCallResult, id.ps2 ) )
	{
		setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FRIENDS_REQUEST_OFFER_FAILED) );
		setLastRequestStatus( STATUS_FAILED );
		return false ;
	}

	setRequestType( REQUEST_TYPE_OFFER_FRIENDSHIP );
	_state				= FRIENDS_OFFER_FRIENDSHIP ;
	_commandInitiated	= true ;
	return true ;
}



///----------------------------------------------
/// Requests RendezVous to offer friendship to the
/// specified player.
///
/// Returns true
///----------------------------------------------
bool OSIPS2::requestOfferMyFriendship( const OSIchar* name )
{
	if ( _commandInitiated )
	{
		setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_REQUEST_ALREADY_IN_PROGRESS) );
		return false ;
	}

	char utf8Name[ OSI_PLAYER_NAME_LENGTH*2 ];
	UTF8Encode( utf8Name, name, NUM_ARRAY_ELEMENTS( utf8Name ) );
	if ( !_TTLMatchMakingClient->CallFriendInvite( _currentContext, &_lastCallResult, utf8Name ) )
	{
		setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FRIENDS_REQUEST_OFFER_FAILED) );
		setLastRequestStatus( STATUS_FAILED );
		return false ;
	}

	setRequestType( REQUEST_TYPE_OFFER_FRIENDSHIP );
	_state				= FRIENDS_OFFER_FRIENDSHIP ;
	_commandInitiated	= true ;
	return true ;
}


///------------------------------------------------
/// Update function for Friendship offer
///------------------------------------------------
bool OSIPS2::_updateOfferFriendship( void )
{
	if ( _checkForCommandCompletionOrTimeout( _state, COMMAND_FAILURE ) )
	{
		_commandInitiated = false ;
		int rc = _currentContext->GetOutcome().GetReturnCode();
		if (!QSUCCEEDED(rc)) 
		{ 
			switch ( rc )
			{
				case QERROR(PythonCore, FriendInvitePlayerSelf): 
					setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FRIENDS_INVITED_SELF) );
					break;

				case QERROR(PythonCore, FriendInvitePlayerName):
					setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FRIENDS_INVALID_NAME) );
					break;

				case QERROR(PythonCore, FriendInviteLocalLimit):
					setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FRIENDS_YOUR_LIMIT_REACHED) );
					break;

				case QERROR(PythonCore, FriendInviteOtherLimit):
					setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FRIENDS_THEIR_LIMIT_REACHED) );
					break;
			}
			setLastRequestStatus( STATUS_FAILED );
			return true;
		}
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_PLAYER_ALREADY_FRIEND) );
			setLastRequestStatus( STATUS_FAILED );
		}
		else
		{
			setLastRequestStatus( STATUS_SUCCEEDED );
		}
		return true ;
	}

	return false ;
}



///-----------------------------------------------------------
/// Cancels a friendship offer
///-----------------------------------------------------------
bool OSIPS2::requestCancelMyFriendshipOffer( OSIPlayerID& id )
{
	if ( _commandInitiated )
	{
		setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_REQUEST_ALREADY_IN_PROGRESS) );
		return false ;
	}

	//OSIFIXME: Need to be able to cancel friendship requestss
	if ( !_TTLMatchMakingClient->CallFriendDelete( _currentContext, &_lastCallResult, id.ps2 ) )
	{
		setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FRIENDS_REQUEST_CANCEL_OFFER_FAILED) );
		setLastRequestStatus( STATUS_FAILED );
		return false ;
	}

	setRequestType( REQUEST_TYPE_CANCEL_FRIENDSHIP );
	_state				= FRIENDS_CANCEL_FRIENDSHIP_REQUEST ;
	_commandInitiated	= true ;
	return true ;
}



///-------------------------------------------------------------
/// Update function for cancelling friendship
///-------------------------------------------------------------
bool OSIPS2::_updateCancelFriendshipRequest( void )
{
	if ( _checkForCommandCompletionOrTimeout( _state, COMMAND_FAILURE ) )
	{
		_commandInitiated = false ;
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_CANCEL_FRIENDSHIP) );
			setLastRequestStatus( STATUS_FAILED );
		}
		else
		{
			setLastRequestStatus( STATUS_SUCCEEDED );
		}
		return true ;
	}

	return false ;
}


///-------------------------------------------------------------
/// Change my published state (visible, hidden).  Not used on PS2
///-------------------------------------------------------------
bool OSIPS2::requestPublishOnlineState( OSIPlayerState& state )
{
	return true ;
}



///--------------------------------------------------------------
/// Request end of a friendship with specified player
///--------------------------------------------------------------
bool OSIPS2::requestEndFriendship( OSIPlayerID& id )
{
	if ( _commandInitiated )
	{
		setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_REQUEST_ALREADY_IN_PROGRESS) );
		return false ;
	}

	if ( !_TTLMatchMakingClient->CallFriendDelete( _currentContext, &_lastCallResult, id.ps2 ) )
	{
		setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FRIENDS_REQUEST_END_FAILED) );
		setLastRequestStatus( STATUS_FAILED );
		return false ;
	}

	setRequestType( REQUEST_TYPE_END_FRIENDSHIP );
	_state				= FRIENDS_END_FRIENDSHIP ;
	_commandInitiated	= true ;
	return true ;
}


///----------------------------------------------------------------
/// Update function for ending a relationship
///----------------------------------------------------------------
bool OSIPS2::_updateEndFriendship( void )
{
	if ( _checkForCommandCompletionOrTimeout( _state, COMMAND_FAILURE ) )
	{
		_commandInitiated = false ;
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_END_FRIENDSHIP) );
			setLastRequestStatus( STATUS_FAILED );
		}
		else
		{
			setLastRequestStatus( STATUS_SUCCEEDED );
		}
		return true ;
	}

	return false ;	
}



///----------------------------------------------------------------
/// Request invitations from the specified friend
///----------------------------------------------------------------
bool OSIPS2::requestMatchForFriend( OSIPlayerID& id )
{
	/// Clear the match/gatherings lists
	_lstGatherings.clear();
	_matchList.clear();

	/// Search the list of match invitations
	qList<GatheringHolder>::iterator iter;
	for( iter = _lstInvitations.begin(); iter != _lstInvitations.end(); ++ iter )
	{
		GatheringHolder& holder = (*iter);

		/// Make sure the invitation whose owner matches <id>
		if( holder->GetOwnerPID() != id.ps2 )
			continue;

		/// Make sure the invitation is of the correct type (match invite, not chat invite)
		if( holder->GetGatheringType() != _T( "TTLSession" ) )
			continue;

		/// Add this invitation match to <_lstGatherings>
		_lstGatherings.push_back( holder );

		/// Set "data ready" so the match list will get populated
		_matchList.setNetworkDataReady( true );
		setRequestType( REQUEST_TYPE_ACCEPT_MATCH_INVITE );
		return true;
	}

	return false;
}
























































/// ***************************************************************************
///	 __  __       _       _       __  __       _    _             
///	|  \/  | __ _| |_ ___| |__   |  \/  | __ _| | _(_)_ __   __ _ 
///	| |\/| |/ _` | __/ __| '_ \  | |\/| |/ _` | |/ / | '_ \ / _` |
///	| |  | | (_| | || (__| | | | | |  | | (_| |   <| | | | | (_| |
///	|_|  |_|\__,_|\__\___|_| |_| |_|  |_|\__,_|_|\_\_|_| |_|\__, |
///                                                        |___/ 
/// ***************************************************************************



///////////////////////////////////////////////////////////////////////////////
//	 __                     __     
//	(_ |_ _ |_. _  _   /  \|__)|   
//	__)|_(_||_|(_)| )  \__/| \ |__ 
//
///////////////////////////////////////////////////////////////////////////////
///--------------------------------------------------------
/// Requests the list of station URLs
///--------------------------------------------------------
bool OSIPS2::requestStationURLListForMatch( OSIMatchID &matchID )
{
	_commandInitiated	= false ;
	_state				= FIND_GATHERING_BY_ID ;
	_matchToJoinID		= matchID.ps2 ;
	setRequestType( REQUEST_TYPE_STATION_URL_LIST );
	return true ;
}

///--------------------------------------------------------
/// Updates function for the StationURLList request
///--------------------------------------------------------
bool OSIPS2::_updateStationURLListRequest( void )
{
	switch ( _state )
	{        // STATE							// UPDATE FUNCTION                // ON SUCCESS GO TO               // ON FAILURE GO TO 
		case FIND_GATHERING_BY_ID:				_processFindGatheringByID		( FIND_MATCH_GETURLS,				COMMAND_FAILURE );	break ;
		case FIND_MATCH_GETURLS:				_processJoinRequestStationURLs	( COMMAND_SUCCESS,					COMMAND_FAILURE );	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus( STATUS_FAILED ); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ; // complete, success
	}

	return false ; // keep working
}


///----------------------------------------------------------
/// Once we've participated in a gathering, get its sesssion URL 
/// so we can directly join it.
///----------------------------------------------------------
void OSIPS2::_processJoinRequestStationURLs( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		g_debugOutput.TSPrint(g_debugOutput.Category("OSI"), "Joining: Requesting URLs");
		PrincipalID pID		= _currentGathering->GetHostPID();
		_currentStationURLs.clear();
		// Retrieve the URLs of the session host and return it back.
		bool result = _rendezVous->GetSecureConnectionClient()->RequestURLs( _currentContext, pID, &_currentStationURLs );
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum( OSIPS2_ERROR_JOIN_MATCH_UNABLE_TO_REQUEST_MATCH_IP ) );
			_state			= failureState ;
			return ;
		}

		_commandInitiated	= true ;
		_resetTimeoutTimer();
	}
	else if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		_commandInitiated	= false ;

		g_debugOutput.TSPrint(g_debugOutput.Category("OSI"), "Joining: Received URLs");
		_stationURLList.setNetworkDataReady( true );
		_state				= successState ;
	}
}


///--------------------------------------------------------
/// Process function for the POPULATE_RV_LIST state
///--------------------------------------------------------
void OSIPS2::_processPopulateRVList( int successState, int failureState )
{
	RVMatch match(_currentGathering, _peerManager );

	_lstRVMatches.clear();
	_lstRVMatches.push_back(match);
	_stationURLList.setNetworkDataReady( true );
	_state				= successState ;
}


///--------------------------------------------------------
///--------------------------------------------------------
void OSIPS2::_populateStationURLList( void )
{
	_stationURLList.clear();
	qList<StationURL>::iterator stationURLIter ;
	for ( stationURLIter = _currentStationURLs.begin(); stationURLIter != _currentStationURLs.end(); ++stationURLIter )
	{
		StationURL& stationURL = *stationURLIter ;
		_stationURLList.push_back( stationURL.CStr() );
	}
}


///--------------------------------------------------------
///--------------------------------------------------------
bool OSIPS2::_processAcceptMatchInvite( void )
{
	setLastRequestStatus( STATUS_SUCCEEDED );
	setActiveState( OSI_STATE_IDLE );
	return true;
}










///////////////////////////////////////////////////////////////////////////////
//	
//	/  \ _  _| _ |_ _  |\/| _ |_ _|_   /  \ _ |_. _  _  _ 
//	\__/|_)(_|(_||_(-  |  |(_||_(_| )  \__/|_)|_|(_)| )_) 
//	    |                                  |              
//
///////////////////////////////////////////////////////////////////////////////
///----------------------------------------------------------
/// Requests to update match options on RendezVous server
///----------------------------------------------------------
bool OSIPS2::requestUpdateMatchOptions( OSIMatchOptions& options )
{
	_currentMatch.maximumPlayers	= options.maxPlayers;
	_currentMatch.options			= options ;
	TTLSession* session				= (TTLSession*)(_currentGathering.Get()) ;
	_currentMatch.options.totalConnectedPlayers	= session->totalConnectedPlayers;
	_matchToSession( _currentMatch, *session );
	setRequestType(REQUEST_TYPE_UPDATE_MATCH_OPTIONS);
	_state							= UPDATE_MATCH_OPTIONS ;
	return true ;
}

///--------------------------------------------------------
///--------------------------------------------------------
bool OSIPS2::_processRendezVousUpdateMatchOptions( void )
{
	switch ( _state )
	{
		case UPDATE_MATCH_OPTIONS:	_updateMatchOptions(COMMAND_SUCCESS, COMMAND_FAILURE);	break ;
		case COMMAND_SUCCESS:
		{
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ;
		}
		case COMMAND_FAILURE:			
		{
			setLastRequestStatus( STATUS_FAILED ); 
			return true;
		}
	}

	setLastRequestStatus( STATUS_WORKING );
	return false ;
}

///--------------------------------------------------------
/// Calls UpdateGathering to update current gathering options.
///--------------------------------------------------------
void OSIPS2::_updateMatchOptions( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		bool success			= _matchMakingClient->UpdateGathering( _currentContext, &_lastCallResult, _currentGathering);
		if ( !success )
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_ERROR_FAILED_VERIFY_CREDENTIALS)); // OSIFIXME
			_state				= failureState ;
			return ;
		}

		_commandInitiated = true ;
		_resetTimeoutTimer();
		return ;
	}
	
	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum( OSIPS2_ERROR_FAILED_VERIFY_CREDENTIALS) );// OSIFIXME
			_state	= failureState ;
			return ;
		}
		else
		{
			_state				= successState ;
			_commandInitiated	= false ;
		}
	}
}










///////////////////////////////////////////////////////////////////////////////
//	
//	|__| _  _|_  |\/| _ |_ _|_  
//	|  |(_)_)|_  |  |(_||_(_| ) 
//
///////////////////////////////////////////////////////////////////////////////

///----------------------------------------------------------
/// Requests RendezVous to filter match name.
///----------------------------------------------------------
bool OSIPS2::requestValidateMatchName( const OSIchar* matchName )
{
	UTF8Encode(_currentRequestStr, matchName, NUM_ARRAY_ELEMENTS(_currentRequestStr));

	setRequestType( REQUEST_TYPE_FILTER_TEXT_FOR_PROFANITY );
	_state = FILTER_TEXT_FOR_PROFANITY;
	return true;
}

///----------------------------------------------------------
/// Requests RendezVous to host a new match.
///----------------------------------------------------------
bool OSIPS2::requestHostMatch( OSIMatchOptions& options )
{	
	// Save the match info in the _currentMatch.
	_currentMatch.numberOfPlayers	= 1 ; // OSIFIXME_NPLAYERS
	_currentMatch.maximumPlayers	= options.maxPlayers;
	_currentMatch.options			= options ;

	_isHost = true;
	_isWantingToQuit = false;
	setRequestType( REQUEST_TYPE_RENDEZVOUS_HOST_MATCH );
	_state = HOST_CREATE_GATHERING;
	return true ;
}

///-------------------------------------------------------
/// Master state handler for host requests
///-------------------------------------------------------
bool OSIPS2::_updateHostMatch( void )
{
	switch ( _state )
	{        // STATE                           // UPDATE FUNCTION			// ON SUCCESS GO TO				// ON FAILURE GO TO
		case HOST_CREATE_GATHERING:				_processCreateGathering		( HOST_REGISTER_GATHERING,		COMMAND_FAILURE );		break ;
		case HOST_REGISTER_GATHERING:			_processRegisterGathering	( HOST_JOIN_GATHERING,			COMMAND_FAILURE );		break ;
		case HOST_FETCH_URL:					_processFetchURL			( HOST_JOIN_GATHERING,			COMMAND_FAILURE );		break ;
		case HOST_JOIN_GATHERING:				_processJoinGathering		( HOST_LAUNCH_SESSION,			COMMAND_FAILURE );		break ;
		case HOST_LAUNCH_SESSION:				_processLaunchSession		( COMMAND_SUCCESS,				COMMAND_FAILURE );		break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus( STATUS_FAILED ); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ; // complete, success
	}

	return false ; // keep working
}

///---------------------------------------------------
///---------------------------------------------------
void OSIPS2::_processCreateGathering( int successState, int failureState )
{
	// Create an instance of Gatherings and initialize its decription with the 
	// match info.
	TTLSession* session				= qNew TTLSession();
	_currentGathering				= session;
	_currentGathering->SetParticipationPolicy( BuiltInParticipationPolicies::OngoingParticipation );
	_currentGathering->SetFlag( GatheringFlags::MigrateOwner );
	setActiveState(OSIPS2_STATE_HOST_CREATE_GATHERING);
	_matchToSession( _currentMatch, *session );
	_state							= successState;
	_commandInitiated				= false; 
	printf("-- OSI -- Created the Session\n");
}


///----------------------------------------------------
///----------------------------------------------------
void OSIPS2::_processRegisterGathering( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		bool result	= _matchMakingClient->RegisterGathering( _currentContext, _currentGathering );
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FAILED_TO_REGISTER_GATHERING) );
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		// No way this can fail? RegisterGathering does not take a _lastCallResult parameter.
		_commandInitiated = false;
		printf("-- OSI -- Registered the Session\n");
		_state = successState;
	}
}


///---------------------------------------------------------------------
/// Fetches the URL of the newly registered match
///---------------------------------------------------------------------
void OSIPS2::_processFetchURL( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		bool result = _matchMakingClient->FindByID( _currentContext, _currentGathering->GetID(), &_lastCallResult, &_currentGathering );
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_REQUEST_FETCH_URL_FAILED) );
			_state = failureState;
			return;
		}
		_commandInitiated = true ;
		_resetTimeoutTimer();
	}
	else if (_checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		_commandInitiated = false ;
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FINDING_HOSTED_MATCH) );
			_state = failureState ;
		}
		else
		{
			_state = successState;
			printf("-- OSI -- Fetched Session URL\n");
		}
	}
}


///----------------------------------------------------
///----------------------------------------------------
void OSIPS2::_processJoinGathering( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		GatheringID gID = _currentGathering->GetID();
		bool result	= _matchMakingClient->Participate(_currentContext, gID, &_lastCallResult);
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FAILED_TO_PARTICIPATE_ON_JOIN) );
			_state = failureState;
			return;
		}

		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FAILED_TO_PARTICIPATE_ON_JOIN) );
			_state = failureState;
		}
		else
		{
			registerForHostMigration(); // now that we've participated
			printf("-- OSI -- Participating in the Session\n");
			_state = successState;
		}
		_commandInitiated = false;
	}	
}


///-----------------------------------------------------
///-----------------------------------------------------
void OSIPS2::_processLaunchSession( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		OSIchar	hostURL[ OSI_STRING_LENGTH_LONG ];
		OSIstrncpy( hostURL, _currentMatch.options.hostURL, OSI_STRING_LENGTH_LONG );
		GatheringID gID = _currentGathering->GetID();

		bool result	= _matchMakingClient->LaunchSession(_currentContext, gID, &_lastCallResult, hostURL);
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_LAUNCH_SESSION_ON_LAUNCH) );
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_LAUNCH_SESSION_WAITING_ON_LAUNCH) );
			_state = failureState;
		}
		else
		{
			_currentMatch.id.ps2 = _currentGathering->GetID();
			printf("-- OSI -- Launched Session\n");
			_state = successState;
		}
		_commandInitiated = false;
	}
}












///////////////////////////////////////////////////////////////////////////////
//
//	  | _ . _   |\/| _ |_ _|_  
//	__)(_)|| )  |  |(_||_(_| ) 
//
///////////////////////////////////////////////////////////////////////////////
///---------------------------------------------------------
/// Request RendezVous to join the specified match.
///---------------------------------------------------------
bool OSIPS2::requestJoinMatch( OSIMatchID& matchID )
{
	_matchToJoinID		= matchID.ps2 ;
	_isHost				= false;
	_isWantingToQuit	= false;
	_state				= FIND_GATHERING_BY_ID ;
	setRequestType( REQUEST_TYPE_RENDEZVOUS_JOIN_MATCH );
	printf( "-- OSI -- Requesting Join Match\n" );
	return true ;
}


///-------------------------------------------------------
/// Master state handler for join requests
///-------------------------------------------------------
bool OSIPS2::_updateJoinMatch( void )
{
	switch ( _state )
	{        // STATE                           // UPDATE FUNCTION					// ON SUCCESS GO TO					// ON FAILURE GO TO     // ADDITIONAL PARAMETERS
		case FIND_GATHERING_BY_ID:				_processFindGatheringByID			( JOIN_REQUEST_PARTICIPATION,		COMMAND_FAILURE );		break ;
		case JOIN_REQUEST_PARTICIPATION:		_processJoinRequestParticipation	( COMMAND_SUCCESS,					COMMAND_FAILURE );		break ;
 		case POPULATE_RV_LIST:					_processPopulateRVList				( FIND_MATCH_GETURLS,				COMMAND_FAILURE );		break ;
 		case FIND_MATCH_GETURLS:				_processFindMatchGetURLs			( CHECK_FOR_CONNECTIVITY,			COMMAND_FAILURE );		break ;
		case CHECK_FOR_CONNECTIVITY:			_processCheckForConnectivity		( COMMAND_SUCCESS,					COMMAND_FAILURE );		break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus( STATUS_FAILED ); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ; // complete, success
	}

	return false ; // keep working
}

///------------------------------------------------------
/// Requests the Gathering matching the gID that we're trying
/// to join.  This allows us to join any gathering by ID, not
/// just ones OSI already knows about.
///------------------------------------------------------
void OSIPS2::_processFindGatheringByID( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		ResultRange defaultRange;
		printf( " -- OSI -- Finding gathering by ID\n" );
		g_debugOutput.TSPrint(g_debugOutput.Category("OSI"), "Finding Gathering by ID");
		bool result			= _matchMakingClient->FindByID(_currentContext, _matchToJoinID, &_lastCallResult, &_currentGathering );
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_REQUEST_FIND_MATCH_BY_ID_FAILED) );
			_state			= failureState ;
			return ;
		}
		_commandInitiated	= true ;
		_resetTimeoutTimer();
	}
	else if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		_commandInitiated	= false ;
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum( OSIPS2_ERROR_UNABLE_TO_FIND_MATCH_BY_ID) );
			_state			= failureState ;
			return ;
		}

		g_debugOutput.TSPrint(g_debugOutput.Category("OSI"), "Found Gathering by ID");
		_state				= successState ;
	}
}

///-------------------------------------------------------
/// Issues the join request to RendezVous.
/// Returns true if failed, false if successful (and pushed
/// us into the next state).
///-------------------------------------------------------
void OSIPS2::_processJoinRequestParticipation( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		g_debugOutput.TSPrint(g_debugOutput.Category("OSI"), "Joining: Participating");
		bool result			= _matchMakingClient->Participate( _currentContext, _currentGathering->GetID(), &_lastCallResult );
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum( OSIPS2_ERROR_JOIN_MATCH_UNABLE_TO_REQUEST_PARTICIPATION ) );
			_state			= failureState ;
			return ;
		}

		_commandInitiated	= true ;
		_resetTimeoutTimer();
	}
	else if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		_commandInitiated	= false ;
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum( OSIPS2_ERROR_JOIN_MATCH_UNABLE_TO_REQUEST_PARTICIPATION ) );
			_state			= failureState ;
			return ; 
		}
		g_debugOutput.TSPrint(g_debugOutput.Category("OSI"), "Joining: Participated");
		// These two lines are NECESSARY to ensure that the current match has the joined match's ID in it.
		TTLSession*	session = (TTLSession*)(_currentGathering.Get()) ;
		_sessionToMatch(*session, _currentMatch );
		_state				= successState ;
	}
}


///-------------------------------------------------------
/// This determines if we can talk to all current participants in the match.
///-------------------------------------------------------
void OSIPS2::_processCheckForConnectivity( int successState, int failureState )
{
	// all we need to do here is check the ping time on the match we fetched.
	RVMatch& match = _lstRVMatches.front();
	if ( match.ping != 9999 && match.ping != Infinite )
	{
		_state = successState ;
	}
	else
	{
		// Should cancel participation here
		_state = failureState ;
	}
}

///--------------------------------------------------------------------------
/// Registers us as a potential host for the specified gathering
///--------------------------------------------------------------------------
void OSIPS2::_registerGatheringForHostMigration( GatheringID gID )
{
	_lstGatheringIDs.push_back( gID );
	_hostMigrationCallback->Register( gID );
}

///--------------------------------------------------------------------------
/// Registers us as a potential host for the specified gathering
///--------------------------------------------------------------------------
void OSIPS2::_unregisterGatheringForHostMigration( GatheringID gID )
{
	_lstGatheringIDs.remove( gID );
	if ( _hostMigrationCallback )
		_hostMigrationCallback->Unregister( gID );
}


///---------------------------------------------------------------------------
/// Called after we join a NetZ session, to register for host migration.
/// We cannot do this at the time of joining the match (the gathering)
/// because NetZ is not yet active--we haven't joined it yet--and this
/// depends on NetZ.
///----------------------------------------------------------------------------
bool OSIPS2::registerForHostMigration( void )
{
	if ( _currentGathering->GetID() )
	{
		_hostMigrationCallback		= qNew HostMigrationCallback( _matchMakingClient );
		if ( _hostMigrationCallback )
		{
			_registerGatheringForHostMigration( _currentGathering->GetID() );
		}
		return true ;
	}
	return false ;
}

bool OSIPS2::unregisterForHostMigration( void )
{
	if ( _hostMigrationCallback != NULL )
	{
		// lets make sure we don't have any outstanding gatherings we're registered for
		qList<GatheringID>::iterator gatheringIter ;
		for ( gatheringIter = _lstGatheringIDs.begin(); gatheringIter != _lstGatheringIDs.end(); ++gatheringIter )
		{
			GatheringID gID = (*gatheringIter);
			printf( "Lingering Gathering ID: %d\n", gID );
			_hostMigrationCallback->Unregister( gID );
		}
		_lstGatheringIDs.clear();

		qDelete _hostMigrationCallback ;
		_hostMigrationCallback = NULL ;
	}

	return true ;
}



///////////////////////////////////////////////////////////////////////////////
//
//	|\/| _ |_ _|_   | _   .|_. _  _  
//	|  |(_||_(_| )  || )\/||_|| )(_) 
//	                             _/  
//
///////////////////////////////////////////////////////////////////////////////

bool OSIPS2::requestInvitePlayerToMatch( const OSIchar* const playerName )
{
	UTF8Encode( _currentRequestStr, playerName, NUM_ARRAY_ELEMENTS(_currentRequestStr) );

	setRequestType( REQUEST_TYPE_INVITE_TO_MATCH );
	_state = INVITE_PLAYER_TO_MATCH_BY_NAME;
	return true;
}

///-----------------------------------------------------------
/// Invite player to match
///-----------------------------------------------------------
bool OSIPS2::requestInvitePlayerToMatch( OSIPlayerID& playerID )
{
	_currentPlayerID = playerID;

	setRequestType( REQUEST_TYPE_INVITE_TO_MATCH );
	_state = INVITE_PLAYER_TO_MATCH;
	return true;
}

///----------------------------------------------------------------------
/// Update state machine pump
///----------------------------------------------------------------------
bool OSIPS2::_updateInviteToMatch( void )
{
	switch (_state)
	{
		case INVITE_PLAYER_TO_MATCH_BY_NAME:_processFindPIDFromName( INVITE_PLAYER_TO_MATCH, COMMAND_FAILURE ); break;
		case INVITE_PLAYER_TO_MATCH:		_processInviteToMatch( COMMAND_SUCCESS, COMMAND_FAILURE ); break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus(STATUS_FAILED); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus(STATUS_SUCCEEDED);
			return true ; // complete, success
	}

	return false ; // keep working
}

///----------------------------------------------------------------------
/// Is this char a special MySQL regular expression char?
///----------------------------------------------------------------------
bool IsRegularExpSpecialChar( unsigned char ch )
{
	switch( ch )
	{
		case '^':
		case '$':
		case '.':
		case '*':
		case '+':
		case '?':
		case '|':
		case '\\':
		case '(':
		case ')':
		case '[':
		case ']':
		case '{':
		case '}':
			return true;
		default:
			return false;
	}
}

///----------------------------------------------------------------------
/// Escape any MySQL regular expression characters with a '\'
///----------------------------------------------------------------------
char* EscapeRegularExpChars( char* outEscapedStr, const char* inStrToEscape, size_t maxLength )
{
	ASSERT( outEscapedStr != NULL );
	if( inStrToEscape == NULL || outEscapedStr == NULL )
	{
		if( outEscapedStr != NULL ) outEscapedStr[0] = 0;
		return outEscapedStr;
	}
	ASSERT( outEscapedStr != inStrToEscape );
	// Set up cursors to iterate over strings
	const char* cursor = inStrToEscape;
	char* encCursor = outEscapedStr;

	// Spit out the UTF8 version of each character
	int i = 0;
	for( ; i < maxLength && *cursor; cursor++ )
	{
		unsigned char c = *cursor;
		if( IsRegularExpSpecialChar( c ) )
		{
			// Escape special char with a \
			unsigned char rightByte = 0x80 | (c & 0x3f);
			// left byte is 110 followed by remaining bits of char
			*encCursor = '\\';
			encCursor++; ++i;
			*encCursor = c;
		}
		else
		{
			// Normal ASCII goes straight across
			*encCursor = *cursor;
		}
		encCursor++; ++i;
	}
	if( i == maxLength ) --encCursor;
	*encCursor = 0;

	return outEscapedStr;
}

///-------------------------------------------------------------
/// Process the request.  Calls to rendezvous and checks for completion.
///-------------------------------------------------------------
void OSIPS2::_processFindPIDFromName( int successState, int failureState )
{
	setLastRequestStatus( STATUS_WORKING );

	// Is this our first time through this process?
	if( !_commandInitiated )
	{
		_lstAccounts.clear();
		OSIchar escapedStr[ OSI_PLAYER_NAME_LENGTH*2 ];
		EscapeRegularExpChars( escapedStr, _currentRequestStr, NUM_ARRAY_ELEMENTS( escapedStr ) );
		// call RendezVous across the network
		bool result	= _accountManagementClient->FindByNameRegex(
			_currentContext, BuiltInGroups::Players, escapedStr, ResultRange(0, 1), &_lstAccounts
		);
		if( !result )
		{
			setErrorString( GetOSIStringByEnum( OSIPS2_ERROR_REQUEST_FIND_PID_FROM_NAME_FAILED ) );
			_state = failureState;
			return;
		}
		_commandInitiated = true;
		_resetTimeoutTimer();
	}

	if( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		if( _lstAccounts.size() > 0 )
		{
			_currentPlayerID.ps2 = _lstAccounts.front().m_pidOwner;
			_lstAccounts.resize(0);
			_state = successState;
		}
		else
		{
			setErrorString( GetOSIStringByEnum( OSIPS2_CLAN_INVALID_PLAYER ) );
			_state = failureState;
		}
		_commandInitiated = false;
	}
}

///-------------------------------------------------------------
/// Process the request.  Calls to rendezvous and checks for completion.
///-------------------------------------------------------------
void OSIPS2::_processInviteToMatch( int successState, int failureState )
{
	setLastRequestStatus( STATUS_WORKING );

	// Is this our first time through this process?
	if( !_commandInitiated )
	{
		// call RendezVous across the network
		bool result	= _matchMakingClient->Invite(
			_currentContext, _currentGathering->GetID(), &_lastCallResult, _currentPlayerID.ps2, "Invitation to TTL Match"
		);
		if( !result )
		{
			setErrorString( GetOSIStringByEnum( OSIPS2_ERROR_REQUEST_INVITE_TO_MATCH_FAILED ) );
			_state = failureState;
			return;
		}
		_commandInitiated = true;
		_resetTimeoutTimer();
	}

	if( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		if( _lastCallResult )
		{
			_state = successState;
		}
		else
		{
			setErrorString( GetOSIStringByEnum( OSIPS2_ERROR_PLAYER_ALREADY_INVITED ) );
			_state = failureState;
		}
		_commandInitiated = false;
	}
}


///-------------------------------------------------------------
///-------------------------------------------------------------
bool OSIPS2::requestDeclineMatchInvite( OSIPlayerID& playerID )
{
	/// Get the match ID matching the specified playerID
	qList<GatheringHolder>::iterator iter;
	for( iter = _lstInvitations.begin(); iter != _lstInvitations.end(); ++ iter )
	{
		GatheringHolder& holder = (*iter);

		/// Make sure the invitation's owner matches <id>
		if( holder->GetOwnerPID() != playerID.ps2 )
			continue;

		/// Make sure the invitation is of the correct type (match invite, not chat invite)
		if( holder->GetGatheringType() != _T( "TTLSession" ) )
			continue;

		GatheringID gatheringID = holder->GetID();

		bool successfullySent;
		if( _matchMakingClient->DeclineInvitation( _currentContext, gatheringID, &successfullySent, "Decline of match invitation." ) )
		{
			printf( "--OSI-- BLOCKING CALL (in OSIPS2::requestDeclineMatchInvite) : DeclineInvitation\n" );
			_currentContext->Wait(); // OSIFIXME: All blocking network calls should be made asynchronous
		}

		return( successfullySent );
	}

	return false;
}



///-----------------------------------------------------------
/// Get the any match invites.
///-----------------------------------------------------------
bool OSIPS2::requestMatchInvitations()
{
	setRequestType( REQUEST_TYPE_GET_MATCH_INVITATIONS );
	_state = GET_MATCH_INVITATIONS;
	return true;
}


///----------------------------------------------------
/// Processing state machine to get match invitations
///----------------------------------------------------
bool OSIPS2::_updateGetMatchInvitations( void )
{
	switch ( _state )
	{        // STATE                           // UPDATE FUNCTION              // ON SUCCESS GO TO               // ON FAILURE GO TO             // ADDITIONAL PARAMETERS
		case GET_MATCH_INVITATIONS:				_processGetMatchInvitations		( COMMAND_SUCCESS,					COMMAND_FAILURE );																	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus( STATUS_FAILED ); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ; // complete, success
	}

	return false ; // keep working
}

///----------------------------------------------------
/// Processing state machine to get match invitations
///----------------------------------------------------
void OSIPS2::_processGetMatchInvitations( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		ResultRange defaultRange;
		_lstInvitations.clear();
		bool result	= _matchMakingClient->FindInvitations( _currentContext, defaultRange, &_lstInvitations );
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_REQUEST_GET_MATCH_INVITES_FAILED) );
			_state = failureState;
			return;
		}

		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		printf("-- OSI -- Retrieved Match Invitations, found %i\n", _lstInvitations.size());
		_state = successState;
		_commandInitiated = false;
	}	
}








///////////////////////////////////////////////////////////////////////////////
//	 __                       
//	|_. _  _|  |\/| _ |_ _|_  
//	| || )(_|  |  |(_||_(_| ) 
//
///////////////////////////////////////////////////////////////////////////////
///-----------------------------------------------------------------------
/// Gets the match list
///-----------------------------------------------------------------------
bool OSIPS2::requestMatchList( OSIMatchOptions& options )
{
	// make old data invalid.
	_matchList.setNetworkDataReady( false );
	_matchOptions = options ;
	//_state = FIND_MATCH_SEARCH;
	_state = GET_MATCH_INVITATIONS;
	setRequestType( REQUEST_TYPE_RENDEZVOUS_FIND_MATCH );
	return true ;
}

///----------------------------------------------------
/// Processing state machine for a Find Match Request
/// 
/// Returns true if the last request is finished processing
///	and an outcome (either success or failure) is known.
///----------------------------------------------------
bool OSIPS2::_processFindMatchRequest( void )
{
	switch ( _state )
	{        // STATE                           // UPDATE FUNCTION                // ON SUCCESS GO TO               // ON FAILURE GO TO             // ADDITIONAL PARAMETERS
		case GET_MATCH_INVITATIONS:				_processGetMatchInvitations		(FIND_MATCH_SEARCH,					COMMAND_FAILURE );																	break ;
		case FIND_MATCH_SEARCH:					_processFindMatchSearch			(FIND_MATCH_GETURLS,				COMMAND_FAILURE );																	break ;
		case FIND_MATCH_GETURLS:				_processFindMatchGetURLs		(FIND_MATCH_PING,					COMMAND_FAILURE );																	break ;
		case FIND_MATCH_PING:					_processFindMatchPing			(COMMAND_SUCCESS,					COMMAND_FAILURE );																	break ;

		case COMMAND_FAILURE:			
			setLastRequestStatus( STATUS_FAILED ); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ; // complete, success
	}

	return false ; // keep working
}

///--------------------------------------------------------
/// Requests a match list fomr the MatchMakingClient.
/// Matches are put into _tempGatherings.
///--------------------------------------------------------
void OSIPS2::_processFindMatchSearch( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		ResultRange defaultRange;
		OSIchar queryStr[512];
		setActiveState(OSIPS2_STATE_FIND_MATCH_REQUEST_CUSTOM_QUERY);
		_tempGatherings.clear();

		if ( !_generateSQLMatchQuery(queryStr, 512) )
		{
			_state = failureState;
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_REQUEST_CUSTOM_MATCHES) );
			return;
		}

		bool result	= _TTLMatchMakingClient->CallFindBySQLQuery(_currentContext, queryStr, defaultRange, &_tempGatherings);
		if ( !result )
		{
			_state = failureState;
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_REQUEST_CUSTOM_MATCHES) );
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		PrincipalID pID = 0;
		printf("-- OSI -- Searched for Matches, found %d\n", _tempGatherings.size());
		_populateRVMatches();
		_state = successState;
		_commandInitiated = false;
	}
}


///--------------------------------------------------------
/// Get the participant URLS for the people in the currently
/// active gathering search result.
///--------------------------------------------------------
void OSIPS2::_populateRVMatches()
{
	_lstRVMatches.clear();
	qList<GatheringID> invitationList;
	qList<GatheringHolder>::iterator iter;	

	// Add matches we were invited to.
	for ( iter = _lstInvitations.begin(); iter != _lstInvitations.end(); iter++ )
	{
		RVMatch match( (*iter), _peerManager );
		TTLSession*	session	= (TTLSession*)match.holder->Get();
		session->isPrivate = 1;
	
		invitationList.push_back( match.gID);
		_lstRVMatches.push_back(match);
	}

	// Add the search results.
	for ( iter = _tempGatherings.begin(); iter != _tempGatherings.end(); iter++ )
	{
		RVMatch match( (*iter), _peerManager );

		qList<GatheringID>::iterator findResult = find( invitationList.begin(), invitationList.end(), match.gID );
		if ( findResult == invitationList.end() )
			_lstRVMatches.push_back(match);
	}
}


///--------------------------------------------------------
/// Get the participant URLS for the people in the currently
/// active gathering search result.
///--------------------------------------------------------
void OSIPS2::_processFindMatchGetURLs( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		if ( !_lstRVMatches.size() ) // Empty List
		{
			_matchList.clear(); // Make sure this is empty also.
			_state = COMMAND_SUCCESS; // Bail out completely.
			return;
		}

		qList<RVMatch>::iterator iter;
		for ( iter = _lstRVMatches.begin(); iter != _lstRVMatches.end(); iter++ )
		{
			RVMatch& rvMatch			= (*iter);
			bool result	= _matchMakingClient->GetParticipantsURLs(&rvMatch.context, rvMatch.gID, &rvMatch.lstStationURLs);
			if ( !result )
			{
				printf("-- OSI -- Retrieved Match URL FAILED\n");
				rvMatch.commandFailed = true;
			}
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}
	else
	{
		qList<RVMatch>::iterator iter;
		for ( iter = _lstRVMatches.begin(); iter != _lstRVMatches.end(); iter++ )
		{
			RVMatch&	rvMatch			= (*iter);
			if ( !rvMatch.hasMatchURLs )
			{
				if ( _checkForCommandCompletionOrTimeout( _state, failureState, &(rvMatch.context) ) )
				{
					rvMatch.hasMatchURLs = true;
				}
			}
		}

		bool done = true;
		for ( iter = _lstRVMatches.begin(); iter != _lstRVMatches.end(); iter++ )
		{
			RVMatch& rvMatch	= (*iter);
			if ( !rvMatch.commandFailed )
				if ( !rvMatch.hasMatchURLs )
					done = false;
		}

		if ( done )
		{
			printf("-- OSI -- Retrieved Match URLs\n");
			_commandInitiated = false;
			done = false;
			_state = successState;
		}
	}
}

///--------------------------------------------------------
/// Requests from RendezVous the connection info (ping times, etc.)
///--------------------------------------------------------
void OSIPS2::_processFindMatchPing( int successState, int failureState )
{
	int numActivePingChecks = 0 ;
	qList<RVMatch>::iterator iter ;
	for ( iter = _lstRVMatches.begin(); iter != _lstRVMatches.end(); ++iter )
	{
		RVMatch& rvMatch = (*iter);
		switch ( rvMatch.pingStatus )
		{
		case RVMatch::PING_KNOWN: 
			break ;
		case RVMatch::PING_WORKING:
			numActivePingChecks++ ;
			if ( rvMatch.update() ) 
				numActivePingChecks-- ;
			break ;
		case RVMatch::PING_UNKNOWN:
			if ( numActivePingChecks < 5 )
			{
				rvMatch.beginPingCheck();
				numActivePingChecks++ ;
			}
			break ;
		}
	}

	if ( numActivePingChecks == 0 )
	{
		_state = successState ;
		_matchList.setNetworkDataReady( true );
	}		
}

///--------------------------------------------------------
/// Generates an SQL query sring for the current match options.
///--------------------------------------------------------
bool OSIPS2::_generateSQLMatchQuery(OSIchar* buffer, int bufsize)
{
	OSIchar localbuf[768]; // More than enough
	OSIchar tmpbuf[768];
	bool prevQuery = false;
	unsigned short flags = _matchOptions.getWildcardFlags(); 

	OSIstrcpy(localbuf, "WHERE ");

	// We are testing to see if the wildcard flag is NOT set, which means that the user has
	// a specific filter.

	if ( !_matchOptions.isWildcardFlagSet(OSIMatchOptions::MATCHOPTIONFLAG_LEVELNAME) )
	{
		OSIchar utf8LevelName[ OSI_LEVEL_NAME_LENGTH*2 ];
		OSIsprintf(tmpbuf, "TTLSessions.levelName = '%s'",
			UTF8Encode( utf8LevelName, _matchOptions.levelName, NUM_ARRAY_ELEMENTS( utf8LevelName ) ));
		prevQuery = true;
		OSIstrcat(localbuf, tmpbuf);
	}

	// Game Type
	if ( !_matchOptions.isWildcardFlagSet(OSIMatchOptions::MATCHOPTIONFLAG_GAMETYPE) )
	{
		if ( prevQuery ) OSIstrcat(localbuf, " AND ");
		OSIsprintf(tmpbuf, "TTLSessions.gameType = %d", _matchOptions.gameTypeIndex);
		prevQuery = true;
		OSIstrcat(localbuf, tmpbuf);
	}

	// Time Limit
	if ( !_matchOptions.isWildcardFlagSet(OSIMatchOptions::MATCHOPTIONFLAG_TIMELIMIT) )
	{
		if ( prevQuery ) OSIstrcat(localbuf, " AND ");
		OSIsprintf(tmpbuf, "TTLSessions.timeLimit = %d", _matchOptions.timeLimit);
		prevQuery = true;
		OSIstrcat(localbuf, tmpbuf);
	}

	// Score Limit
	if ( !_matchOptions.isWildcardFlagSet(OSIMatchOptions::MATCHOPTIONFLAG_SCORELIMIT) )
	{
		if ( prevQuery ) OSIstrcat(localbuf, " AND ");
		OSIsprintf(tmpbuf, "TTLSessions.scoreLimit = %d", _matchOptions.scoreLimit);
		prevQuery = true;
		OSIstrcat(localbuf, tmpbuf);
	}

	// Tag Limit
	if ( !_matchOptions.isWildcardFlagSet(OSIMatchOptions::MATCHOPTIONFLAG_TAGLIMIT) )
	{
		if ( prevQuery ) OSIstrcat(localbuf, " AND ");
		OSIsprintf(tmpbuf, "TTLSessions.tagLimit = %d", _matchOptions.tagLimit);
		prevQuery = true;
		OSIstrcat(localbuf, tmpbuf);
	}

	// Best of Series (need this?)
	if ( !_matchOptions.isWildcardFlagSet(OSIMatchOptions::MATCHOPTIONFLAG_BESTOFSERIES) )
	{
		if ( prevQuery ) OSIstrcat(localbuf, " AND ");
		OSIsprintf(tmpbuf, "TTLSessions.bestOfSeries = %d", _matchOptions.bestOfSeries);
		prevQuery = true;
		OSIstrcat(localbuf, tmpbuf);
	}

	// Max Players
	if ( !_matchOptions.isWildcardFlagSet(OSIMatchOptions::MATCHOPTIONFLAG_MAXPLAYERS) )
	{
		if ( prevQuery ) OSIstrcat(localbuf, " AND ");
		OSIsprintf(tmpbuf, "TTLSessions.maxPlayers = %d", _matchOptions.maxPlayers);
		prevQuery = true;
		OSIstrcat(localbuf, tmpbuf);
	}

	// Reserved Players (need this?)
	if ( !_matchOptions.isWildcardFlagSet(OSIMatchOptions::MATCHOPTIONFLAG_RESERVEDPLAYERS) )
	{
		if ( prevQuery ) OSIstrcat(localbuf, " AND ");
		OSIsprintf(tmpbuf, "TTLSessions.reservedPlayers = %d", _matchOptions.reservedPlayers);
		prevQuery = true;
		OSIstrcat(localbuf, tmpbuf);
	}

	// Total Connected Players (need this?)
	if ( !_matchOptions.isWildcardFlagSet(OSIMatchOptions::MATCHOPTIONFLAG_TOTALCONNECTEDPLAYERS) )
	{
		if ( prevQuery ) OSIstrcat(localbuf, " AND ");
		OSIsprintf(tmpbuf, "TTLSessions.totalConnectedPlayers = %d", _matchOptions.totalConnectedPlayers);
		prevQuery = true;
		OSIstrcat(localbuf, tmpbuf);
	}

	// Ranked Game Flag
	if ( !_matchOptions.isWildcardFlagSet(OSIMatchOptions::MATCHOPTIONFLAG_ISRANKED) )
	{
		if ( prevQuery ) OSIstrcat(localbuf, " AND ");
		OSIstrcat(localbuf,  _matchOptions.isRanked ? "TTLSessions.isRanked = 1" : "TTLSessions.isRanked = 0");
		prevQuery = true;
	}

	// Respawn Flag
	if ( !_matchOptions.isWildcardFlagSet(OSIMatchOptions::MATCHOPTIONFLAG_PLAYERSRESPAWN) )
	{
		if ( prevQuery ) OSIstrcat(localbuf, " AND ");
		OSIstrcat(localbuf,  _matchOptions.playersRespawn ? "TTLSessions.playersRespawn = 1" : "TTLSessions.playersRespawn = 0");
		prevQuery = true;
	}

	// Friendly Fire Flag
	if ( !_matchOptions.isWildcardFlagSet(OSIMatchOptions::MATCHOPTIONFLAG_FRIENDLYFIRE) )
	{
		if ( prevQuery ) OSIstrcat(localbuf, " AND ");
		OSIstrcat(localbuf,  _matchOptions.friendlyFire ? "TTLSessions.friendlyFire = 1" : "TTLSessions.friendlyFire = 0");
		prevQuery = true;
	}

	// Apprehension Flag
	if ( !_matchOptions.isWildcardFlagSet(OSIMatchOptions::MATCHOPTIONFLAG_APPREHENSION) )
	{
		if ( prevQuery ) OSIstrcat(localbuf, " AND ");
		OSIstrcat(localbuf,  _matchOptions.apprehension ? "TTLSessions.apprehension = 1" : "TTLSessions.apprehension = 0");
		prevQuery = true;
	}

	// Arcade Damage Flag
	if ( !_matchOptions.isWildcardFlagSet(OSIMatchOptions::MATCHOPTIONFLAG_ARCADEDAMAGE) )
	{
		if ( prevQuery ) OSIstrcat(localbuf, " AND ");
		OSIstrcat(localbuf,  _matchOptions.arcadeDamage ? "TTLSessions.arcadeDamage = 1" : "TTLSessions.arcadeDamage = 0");
		prevQuery = true;
	}

	// Self Damage Flag
	if ( !_matchOptions.isWildcardFlagSet(OSIMatchOptions::MATCHOPTIONFLAG_SELFDAMAGE) )
	{
		if ( prevQuery ) OSIstrcat(localbuf, " AND ");
		OSIstrcat(localbuf,  _matchOptions.selfDamage ? "TTLSessions.selfDamage = 1" : "TTLSessions.selfDamage = 0");
		prevQuery = true;
	}

	// Alternating Teams Flag
	if ( !_matchOptions.isWildcardFlagSet(OSIMatchOptions::MATCHOPTIONFLAG_ALTERNATINGTEAMS) )
	{
		if ( prevQuery ) OSIstrcat(localbuf, " AND ");
		OSIstrcat(localbuf,  _matchOptions.alternatingTeams ? "TTLSessions.alternatingTeams = 1" : "TTLSessions.alternatingTeams = 0");
		prevQuery = true;
	}

	// Only find TTLSession types.
	if ( prevQuery ) OSIstrcat(localbuf, " AND ");
	OSIsprintf( tmpbuf, "Gatherings.type = 'TTLSession' AND TTLSessions.isPrivate = %i", _matchOptions.isPrivate ? 1 : 0 );
	OSIstrcat(localbuf, tmpbuf);

	if ( OSIstrlen(localbuf) < (unsigned)bufsize )
	{
		OSIstrncpy(buffer, localbuf, bufsize);
		return true;
	}

	return false; // Error, string too long.
}


///-----------------------------------------------
/// This populates the match list with all matches
/// that met the match options.
///-----------------------------------------------
void OSIPS2::_populateMatchList( void )
{
	_matchList.clear();

	qList<RVMatch>::iterator iter;
	for ( iter = _lstRVMatches.begin(); iter != _lstRVMatches.end(); iter++ )
	{
		OSIMatch			match ;
		RVMatch& rvMatch	= (*iter);
		
		// Throw out bad matches.
		if ( rvMatch.ping == Infinite )
			continue;

		GatheringHolder*	holder	= rvMatch.holder;
		TTLSession*			session	= (TTLSession*)holder->Get();

		_sessionToMatch(*session, match);
		match.ping = ( rvMatch.ping != Infinite ) ? rvMatch.ping : 9999 ;

		if ( match.options.totalConnectedPlayers == match.maximumPlayers )
		{
			printf("-- OSI -- Dropping match %d because it is full\n", rvMatch.gID );
			continue;
		}

		const OSIMatch* curmatch = NULL;
		for ( _matchList.begin(); _matchList.getCurrent() != NULL; _matchList.next() )
		{
			curmatch = _matchList.getCurrent();
			if ( match.ping < curmatch->ping )
				break;
		}

		if ( _matchList.getCurrent() == NULL )
			_matchList.push_back( match );
		else
			_matchList.insert( match );
	}
}










///////////////////////////////////////////////////////////////////////////////
//
//	            __                 
//	|\/|. _ _  |__)_ _     _ _|_ _ 
//	|  ||_)(_  | \(-(_||_|(-_)|__) 
//	                  |      
//
///////////////////////////////////////////////////////////////////////////////
///---------------------------------------------------------
/// 
///---------------------------------------------------------
bool OSIPS2::playerJoinCurrentMatch( const OSIchar* playerName, OSIPlayerID& id, int numPlayersInMatch )
{
	// if this is anyone but us, we add them to the current players list
	if ( strcmp(playerName, _activeAccount->player.name) != 0 )
	{
		OSIPlayer joiningPlayer;
		joiningPlayer.id = id;
		OSIstrcpy( joiningPlayer.name, playerName );
		_removeRecentPlayer( joiningPlayer );
		_addCurrentPlayer( joiningPlayer );
		if( _isHost )
		{
			// Update the current gathering
			TTLSession* session = (TTLSession*)_currentGathering.Get();
			ASSERT( 0 < numPlayersInMatch && numPlayersInMatch <= session->maxPlayers );
			printf( "OSI :: Total connected players is %d\n", session->totalConnectedPlayers );
			_totalConnectedPlayers = numPlayersInMatch;
			printf( "OSI :: Updating total connected players to %d\n", _totalConnectedPlayers );
			_isGatheringUpdateReady = true;
		}
	}
	return true;
}

///---------------------------------------------------------
/// 
///---------------------------------------------------------
bool OSIPS2::playerLeaveCurrentMatch( const OSIchar* playerName, OSIPlayerID& id, int numPlayersInMatch )
{
	OSIPlayer leavingPlayer;
	leavingPlayer.id = id;
	OSIstrcpy( leavingPlayer.name, playerName );
	_removeCurrentPlayer( leavingPlayer );

	if( id != _activeAccount->player.id )
	{
		_removeRecentPlayer( leavingPlayer );
		_addRecentPlayer( leavingPlayer );
	}

	if( numPlayersInMatch < 1 )
		return true;

	if( _isHost && !_isWantingToQuit )
	{
		TTLSession* session = (TTLSession*)_currentGathering.Get();
		ASSERT( 0 < numPlayersInMatch && numPlayersInMatch <= session->maxPlayers );
		printf( "OSI :: Total connected players is %d\n", session->totalConnectedPlayers );
		_totalConnectedPlayers = numPlayersInMatch;
		printf( "OSI :: Updating total connected players to %d\n", _totalConnectedPlayers );
		_isGatheringUpdateReady = true;
	}
	return true;
}


///-----------------------------------------------------------
/// Requests the Message Of The Day from the Quazal server.
///-----------------------------------------------------------
bool OSIPS2::requestMessageOfTheDay( const OSIchar* language )
{
	OSIstrncpy( _currentRequestStr, language, NUM_ARRAY_ELEMENTS( _currentRequestStr ) );
	_messageOfTheDay[0] = 0;

	setRequestType( REQUEST_TYPE_MESSAGE_OF_THE_DAY );
	_state = MESSAGE_OF_THE_DAY;

	return true;
}

///----------------------------------------------------------------------
/// Update state machine pump
///----------------------------------------------------------------------
bool OSIPS2::_updateMessageOfTheDay()
{
	switch (_state)
	{
		case MESSAGE_OF_THE_DAY: _processMessageOfTheDay(COMMAND_SUCCESS, COMMAND_FAILURE);	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus(STATUS_FAILED); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus(STATUS_SUCCEEDED);
			return true ; // complete, success
	}

	return false ; // keep working
}

///----------------------------------------------------------------------
/// Process the request.  Calls to rendezvous and checks for completion.
///----------------------------------------------------------------------
void OSIPS2::_processMessageOfTheDay(int successState, int failureState)
{
	setLastRequestStatus(STATUS_WORKING);
	// Is this our first time through this process?
	if (!_commandInitiated)
	{
		// call RendezVous across the network
		bool result	= _TTLMatchMakingClient->CallGetMessageOfTheDay(_currentContext, _currentRequestStr, &_motd);
		if (!result)
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_ERROR_REQUEST_MESSAGE_OF_THE_DAY_FAILED));
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	if(_checkForCommandCompletionOrTimeout(_state, failureState))
	{
		UTF8Decode(_messageOfTheDay, _motd.CStr(), NUM_ARRAY_ELEMENTS(_messageOfTheDay));
		_state = successState;
		_commandInitiated = false;
	}
}

///----------------------------------------------------------------------
/// Waits _timeToWaitFor before going to successState.
/// NOTE: make sure to set _timeToWaitFor in the request.
///----------------------------------------------------------------------
void OSIPS2::_processWaitSpecifiedTime( int successState, int failureState )
{
	setLastRequestStatus(STATUS_WORKING);

	if( !_commandInitiated )
	{
		_commandInitiated	= true;
		ResetAndStartTimer( _waitChrono );
	}

	if( _waitChrono.Check() > _timeToWaitFor )
	{
		_state = successState;
		_commandInitiated = false;
	}
}

///----------------------------------------------------------
/// Requests RendezVous to filter match name.
///----------------------------------------------------------
bool OSIPS2::requestFilterTextForProfanity( const OSIchar* textToFilter )
{
	UTF8Encode(_currentRequestStr, textToFilter, NUM_ARRAY_ELEMENTS(_currentRequestStr));

	setRequestType( REQUEST_TYPE_FILTER_TEXT_FOR_PROFANITY );
	_state = FILTER_TEXT_FOR_PROFANITY;
	return true;
}

///-------------------------------------------------------
/// Master state handler for host requests
///-------------------------------------------------------
bool OSIPS2::_updateFilterTextForProfanity( void )
{
	switch ( _state )
	{
		case FILTER_TEXT_FOR_PROFANITY: _processFilterTextForProfanity( COMMAND_SUCCESS, COMMAND_FAILURE ); break ;

		case COMMAND_FAILURE:			
			setLastRequestStatus( STATUS_FAILED ); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ; // complete, success
	}

	return false ; // keep working
}

///---------------------------------------------------
///---------------------------------------------------
void OSIPS2::_processFilterTextForProfanity( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		bool result = _TTLMatchMakingClient->CallFilterText( _currentContext, &_lastCallResult, _currentRequestStr );
		if ( !result )
		{
			_state = failureState ;
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_REQUEST_FILTER_TEXT_FAILED) );
			return;
		}
		_commandInitiated = true ;
		_resetTimeoutTimer();
	}
	else if (_checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		_commandInitiated = false ;
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_TEXT_FILTER_ERROR) );
			_state = failureState ;
		}
		else
		{
			_state = successState;
		}
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	|__| _  _|_  |\/|. _  _ _ |_. _  _  
//	|  |(_)_)|_  |  ||(_)| (_||_|(_)| ) 
//	                  _/                
//
///////////////////////////////////////////////////////////////////////////////
///-----------------------------------------------------
/// Requests that we migrate the host of the gathering.
/// In reality, the host will be migrated by RendezVous
/// automatically since we registered the gathering with
/// the host migration callback.
///
/// But, if we're the new host, we need to recognize the
/// fact, and we need to update the description of the
/// gathering to be our name, rather than the former host's.
///
/// We may also need to do other work in the future, so
/// I'm putting in a whole request for it.
///------------------------------------------------------
bool OSIPS2::requestHostMigration( OSIPlayerID &newHostID, int numPlayersInMatch )
{
	if ( newHostID.isValid() )
	{
		if ( newHostID.ps2 == getActivePlayer()->id.ps2 )
		{
			_isHost = true ;
		}
	}

	if( _isHost )
	{
		TTLSession* session = (TTLSession*)_currentGathering.Get();
		printf( "OSI :: Total connected players is %d\n", session->totalConnectedPlayers );
		_totalConnectedPlayers = numPlayersInMatch;
		printf( "OSI :: Updating total connected players to %d\n", _totalConnectedPlayers );
		if ( !isOrphanSession() )
		{
			_isGatheringUpdateReady = true;
		}
	}

	return false ;
}

///-----------------------------------------------------
/// Update _currentGathering
///------------------------------------------------------
bool OSIPS2::_updateUpdateGathering( void )
{
	switch ( _state )
	{        // STATE               // UPDATE FUNCTION          // ON SUCCESS GO TO     // ON FAILURE GO TO
		case UPDATE_GATHERING:		_processUpdateGathering	(	COMMAND_SUCCESS,		COMMAND_FAILURE );	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus( STATUS_FAILED ); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ; // complete, success
	}

	return false ; // keep working
}

///-----------------------------------------------------
/// Send the changed _currentGathering to RendezVous
///------------------------------------------------------
void OSIPS2::_processUpdateGathering( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		bool result = _matchMakingClient->UpdateGathering( _currentContext, &_lastCallResult, _currentGathering);
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_REQUEST_UPDATE_PLAYER_JOINING_FAILED) );
			_state = failureState ;
			return ;
		}
		_commandInitiated = true ;
		_resetTimeoutTimer();
		return ;
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		_commandInitiated = false ;
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_REQUEST_UPDATE_PLAYER_JOINING_FAILED) );
			_state = failureState ;
			return ;
		}
		_state = successState ;
	}
}









































/// ***************************************************************************
///	  ____                        __  __       _    _             
///	 / ___| __ _ _ __ ___   ___  |  \/  | __ _| | _(_)_ __   __ _ 
///	| |  _ / _` | '_ ` _ \ / _ \ | |\/| |/ _` | |/ / | '_ \ / _` |
///	| |_| | (_| | | | | | |  __/ | |  | | (_| |   <| | | | | (_| |
///	 \____|\__,_|_| |_| |_|\___| |_|  |_|\__,_|_|\_\_|_| |_|\__, |
///                                                        |___/ 
/// ***************************************************************************
///-----------------------------------------------------
/// Processing state machine for a create round request
/// When the request successfully completes, you can get
/// the round ID through getRoundID().
///-----------------------------------------------------
bool OSIPS2::_updateCreateRound( void )
{
	switch ( _state )
	{
		case CREATE_ROUND_START_SESSION:	_processCreateRoundStartSession	(CREATE_ROUND_CREATE_ROUND,			COMMAND_FAILURE ); break ;
		case CREATE_ROUND_CREATE_ROUND:		_processCreateRoundCreateRound	(CREATE_ROUND_REGISTER_ROUND,		COMMAND_FAILURE ); break ;
		case CREATE_ROUND_REGISTER_ROUND:	_processCreateRoundRegisterRound(_isCompetitionRound ? CREATE_ROUND_COMPETITION : COMMAND_SUCCESS, COMMAND_FAILURE ); break ;
		case CREATE_ROUND_COMPETITION:		_processCreateRoundCompetition	(COMMAND_SUCCESS,					COMMAND_FAILURE ); break ;
		//case CREATE_ROUND_INVITE_PLAYERS:	_processCreateRoundInvitePlayers(COMMAND_SUCCESS,					COMMAND_FAILURE ); break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus( STATUS_FAILED ); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ; // complete, success
	}

	return false ; // keep working
}

///----------------------------------------------------
/// Process function for starting the session
///----------------------------------------------------
void OSIPS2::_processCreateRoundStartSession( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		bool result	= _matchMakingClient->SetState(_currentContext, _currentGathering->GetID(), &_lastCallResult, GatheringStates::HasStarted);
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_START_ROUND) );
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_HOST_COMPETITION) );
			_state = failureState;
		}
		else
		{
			_sessionStarted = true;
			printf("-- OSI -- Started the match\n");
			_state = successState;
		}
		_commandInitiated = false;
	}

	return;
}

///----------------------------------------------------
/// Process function for creating a round
///----------------------------------------------------
void OSIPS2::_processCreateRoundCreateRound( int successState, int failureState )
{
	TTLRound* newRound	= qNew TTLRound();

	if ( newRound )
	{
		newRound->isPrivate		= false ;
		newRound->levelIndex	= 0 ;
		newRound->levelName		= "Unused" ; // UF8
		newRound->timeLimit		= 0 ;
		newRound->ranking		= 0 ;

		_currentRound		= newRound ;
		_currentRound->SetParticipationPolicy( BuiltInParticipationPolicies::OpenSessionParticipation );
		_currentRound->SetDescription( _currentGathering->GetDescription() );

		_state = successState;
		printf("-- OSI -- Created round\n");
	}
	else
	{
		_state = failureState;
	}
}

///----------------------------------------------------
/// Process function for registering round
///----------------------------------------------------
void OSIPS2::_processCreateRoundRegisterRound( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		bool result	= _matchMakingClient->RegisterGathering( _currentContext, _currentRound );
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FAILED_TO_REGISTER_GATHERING) );
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		// No way to check for success or failure, so we are assuming success
		setRoundID(_currentRound->GetID()); // We should have a roundID now.
		printf("-- OSI -- Registered the round\n");
		
		// If this is not a ranked match, skip over registering with the competition server
		if ( !_currentMatch.options.isRanked )
			_state = COMMAND_SUCCESS;
		else
			_state = successState;
		_commandInitiated = false;
	}
}

///----------------------------------------------------
/// Process function for register round with competition client
///----------------------------------------------------
void OSIPS2::_processCreateRoundCompetition( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		OSIRoundID roundID = getRoundID();
		bool result	= _competitionClient->RegisterCompetitionEvent( _currentContext, &_lastCallResult, _nonladderRanking->GetID(), roundID);
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_REGISTER_EVENT) );
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_REGISTER_EVENT) );
			_state = failureState;
		}
		else
		{
			printf("-- OSI -- Registered the round to the competition\n" );
			_state = successState;
		}
		_commandInitiated = false;
	}
}

///----------------------------------------------------
/// Process function for inviting players
///----------------------------------------------------
void OSIPS2::_processCreateRoundInvitePlayers( int successState, int failureState )
{
	_state = successState;
}


///----------------------------------------------------
/// Processing state machine for join round request
///----------------------------------------------------
bool OSIPS2::_updateJoinRound( void )
{
	switch ( _state )
	{        // STATE                           // UPDATE FUNCTION                // ON SUCCESS GO TO               // ON FAILURE GO TO             // ADDITIONAL PARAMETERS
		case JOIN_ROUND:						_processJoinRound				( COMMAND_SUCCESS,					COMMAND_FAILURE );																	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus( STATUS_FAILED ); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ; // complete, success
	}

	return false ; // keep working
}

///----------------------------------------------------
/// Processing state machine for joining a round
///----------------------------------------------------
void OSIPS2::_processJoinRound( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		OSIRoundID roundID = getRoundID();
		printf("-- OSI -- Participating in the round\n");
		bool result	= _matchMakingClient->Participate(_currentContext, roundID, &_lastCallResult);
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FAILED_TO_PARTICIPATE_ON_JOIN) );
			_state = failureState;
			return;
		}

		//_registerGatheringForHostMigration( roundID );

		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_FAILED_TO_PARTICIPATE_ON_JOIN) );
			_state = failureState;
		}
		else
		{
			printf("-- OSI -- Participated in the round\n");
			_state = successState;
		}
		_commandInitiated = false;
	}	
}


///----------------------------------------------------
/// Processing state machine for start round request
///----------------------------------------------------
bool OSIPS2::_updateStartRound( void )
{
	switch ( _state )
	{        // STATE                           // UPDATE FUNCTION	    // ON SUCCESS GO TO			// ON FAILURE GO TO 
		case LAUNCH_ROUND:						_processLaunchRound		( START_ROUND,				COMMAND_FAILURE );	break ;
		case START_ROUND:						_processStartRound		( COMMAND_SUCCESS,			COMMAND_FAILURE );	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus( STATUS_FAILED ); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ; // complete, success
	}

	return false ; // keep working
}

///-----------------------------------------------------
/// Processing state machine for launching a round
///-----------------------------------------------------
void OSIPS2::_processLaunchRound( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		OSIRoundID roundID = getRoundID();
		OSIchar	hostURL[ OSI_STRING_LENGTH_LONG ];
		OSIstrncpy( hostURL, _currentMatch.options.hostURL, OSI_STRING_LENGTH_LONG );
		printf("-- OSI -- Launching round\n");

		bool result	= _matchMakingClient->LaunchSession(_currentContext, roundID, &_lastCallResult, hostURL);
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_BEGIN_LAUNCHING_ROUND) );
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_LAUNCH_ROUND) );
			_state = failureState;
		}
		else
		{
			printf("-- OSI -- Launched round\n");
			_state = successState;
		}
		_commandInitiated = false;
	}
}

///-----------------------------------------------------
/// Processing state machine for starting a round
///-----------------------------------------------------
void OSIPS2::_processStartRound( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		OSIRoundID roundID = getRoundID();
		printf("-- OSI -- Starting the round\n");
		bool result	= _matchMakingClient->SetState(_currentContext, roundID, &_lastCallResult, GatheringStates::HasStarted);
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_START_ROUND) );
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_START_ROUND) );
			_state = failureState;
		}
		else
		{
			printf("-- OSI -- Started the round\n");
			_state = successState;
		}
		_commandInitiated = false;
	}
}


///-----------------------------------------------------
/// Processing state machine for the end round request
///-----------------------------------------------------
bool OSIPS2::_updateEndRound( void )
{
	int secondState = _isCompetitionRound ? REPORT_STATS : END_ROUND_PARTICIPATION ;

	switch ( _state )
	{
		case FINISH_ROUND:				_processFinishRound				(secondState,				COMMAND_FAILURE ); break ;
		case REPORT_STATS:				_processReportStats				(REPORT_PERSONAL_STATS,		COMMAND_FAILURE ); break ;
		case REPORT_PERSONAL_STATS:		_processReportPersonalStats		(END_ROUND_PARTICIPATION,	COMMAND_FAILURE ); break ;
		case END_ROUND_PARTICIPATION:	_processEndRoundParticipation	(COMMAND_SUCCESS,			COMMAND_FAILURE ); break ;

		case COMMAND_FAILURE:			
			setLastRequestStatus( STATUS_FAILED ); 
			g_debugOutput.TSPrint( g_debugOutput.Category("OSI"), "_updateEndRound FAILED" );
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			g_debugOutput.TSPrint( g_debugOutput.Category("OSI"), "_updateEndRound done" );
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ; // complete, success
	}

	return false ; // keep working
}


///----------------------------------------------------
/// This use to set the state of the round gathering, but
/// now that we're using OpenSessionParticipation, we don't
/// need to do it because it will be done automatically when
/// all players end participation.
///----------------------------------------------------
void OSIPS2::_processFinishRound( int successState, int failureState )
{
	// We no longer do anything in this state
	_state = successState ;
}


///-----------------------------------------------------
/// Gather stats 
///-----------------------------------------------------
void OSIPS2::_gatherStats()
{
	_lstStats.clear();
	int count = _roundStats.playerStatsList.count();
	for ( int i = 0; i < count; i++ )
	{
		const OSIPlayerStats* stats = _roundStats.playerStatsList[i];

		Quazal::GatheringStats gathStats;
		gathStats.m_pidParticipant = stats->playerID.ps2;
		gathStats.m_uiFlags = Quazal::StatisticFlags::Finished;

		// NOTE: This MUST match the order of the entries in the NonladderRanking class in the
		// TTLRankings.py script!
		for ( int i=0; i<=OSIPlayerStats::LAST_ARBITRATED_STAT; i++ )
		{
			gathStats.m_lstValues.push_back((float)stats->statsArray[i]);
		}

		if( stats->playerID == _activeAccount->player.id )
		{
			_activeAccount->player.statRating += int(stats->getStat( OSIPlayerStats::PLAYERSTAT_USER_RATING ));
		}

		_lstStats.push_back(gathStats);
	}
}


///-----------------------------------------------------
/// Reporting Stats
///-----------------------------------------------------
void OSIPS2::_processReportStats( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		g_debugOutput.TSPrint( g_debugOutput.Category("OSI"), "_processReportStats" );
		_gatherStats();
		OSIRoundID roundID = getRoundID();
		bool result	= _matchMakingClient->ReportStats(_currentContext, roundID, &_lastCallResult, _lstStats);
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_REPORT_STATS) );
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_REPORT_STATS) );
			_state = failureState;
		}
		else
		{
			printf("-- OSI -- Reported Stats\n");
			_state = successState;
		}
		_commandInitiated = false;
	}
}


///-----------------------------------------------------
/// Reporting personal stats
///-----------------------------------------------------
void OSIPS2::_processReportPersonalStats( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		g_debugOutput.TSPrint( g_debugOutput.Category("OSI"), "_processReportPersonalStats" );
		bool result	= _TTLMatchMakingClient->CallAddStatsPersonal(_currentContext, _lstPersonalStats);
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_REPORT_STATS) );
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		printf("-- OSI -- Reported Personal Stats\n");
		_roundStats.playerStatsList.clear();
		_state = successState;
		_commandInitiated = false;
	}
}


///-----------------------------------------------------
/// Unregistering a round
///-----------------------------------------------------
void OSIPS2::_processEndRoundParticipation( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		OSIRoundID roundID = getRoundID();
		char buffer[ 256 ] ;
		sprintf( buffer, "_processEndRoundParticipation Round %d", roundID );
		g_debugOutput.TSPrint( g_debugOutput.Category("OSI"), buffer );
		bool result = true ;

		if ( _isWantingToQuit )
		{
			result = _matchMakingClient->CancelParticipation( _currentContext, roundID, &_lastCallResult, "Cancelled Participation" );
		}
		else
		{
			result	= _matchMakingClient->EndParticipation(_currentContext, roundID, &_lastCallResult, "Ended Participation");
		}

		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_END_PARTICIPATION) );
			_state = failureState;
			setRoundID( 0 );
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_END_PARTICIPATION) );
			_state = failureState;
		}
		else
		{
			printf("-- OSI -- Ended Round Participation\n");
			_state = successState;
		}
		_commandInitiated = false;
		//setRoundID( 0 );
	}
}


///-----------------------------------------------------
/// Processing state machine for the end match
///-----------------------------------------------------
bool OSIPS2::_updateEndMatch( void )
{
	switch ( _state )
	{        // STATE               // UPDATE FUNCTION                // ON SUCCESS GO TO               // ON FAILURE GO TO             // ADDITIONAL PARAMETERS
		case FINISH_MATCH:			_processFinishMatch				(CANCEL_PARTICIPATION,				COMMAND_FAILURE );																	break ;
		case CANCEL_PARTICIPATION:	_processCancelParticipation		(UNREGISTER_MATCH,					COMMAND_FAILURE );																	break ;
		case UNREGISTER_MATCH:		_processUnregisterMatch			(COMMAND_SUCCESS,					COMMAND_FAILURE );																	break ;
		case COMMAND_FAILURE:			
			_isHost = false;
			_isWantingToQuit = false;
			setLastRequestStatus( STATUS_FAILED ); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:
			_isHost = false;
			_isWantingToQuit = false;
			_currentMatch.id.quadword = 0 ;
			setLastRequestStatus( STATUS_SUCCEEDED );
			return true ; // complete, success
	}

	return false ; // keep working
}


///-----------------------------------------------------
/// Finishing a match
///-----------------------------------------------------
void OSIPS2::_processFinishMatch( int successState, int failureState )
{
	if ( !_isHost ) // If we are not the host, skip this state
	{
		_state = successState;
		return;
	}

	if ( !_commandInitiated )
	{
		g_debugOutput.TSPrint( g_debugOutput.Category("OSI"), "_processFinishMatch" );
		OSIRoundID matchID = _currentGathering->GetID();
		bool result	= _matchMakingClient->SetState(_currentContext, matchID, &_lastCallResult, GatheringStates::HasFinished);
		if ( !result )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_FINISH_SESSION) );
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_FINISH_SESSION) );
			_state = failureState;
		}
		else
		{
			printf("-- OSI -- Finished the match\n");
			_state = successState;
		}
		_commandInitiated = false;
	}
}


///-----------------------------------------------------
/// Cancel Participation
///-----------------------------------------------------
void OSIPS2::_processCancelParticipation( int successState, int failureState )
{
	if ( !_commandInitiated )
	{
		g_debugOutput.TSPrint( g_debugOutput.Category("OSI"), "_processCancelParticipation" );
		GatheringID gID = _currentGathering->GetID();
		_unregisterGatheringForHostMigration( gID );

		if (!_matchMakingClient->EndParticipation(_currentContext, gID, &_lastCallResult, "Ending Session Participation")) 
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_FINISH_SESSION) );
			_state = failureState;
			return;
		}

		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	if ( _checkForCommandCompletionOrTimeout( _state, failureState ) )
	{
		if ( !_lastCallResult )
		{
			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_FINISH_SESSION) );
			_state = failureState;
		}
		else
		{
			printf("-- OSI -- Cancelled Participation\n");
			_state = successState;
		}
		_commandInitiated = false;
	}
}


///-----------------------------------------------------
/// Unregistering a TTLSession
///-----------------------------------------------------
void OSIPS2::_processUnregisterMatch( int successState, int failureState )
{
	unregisterForHostMigration();
	_state = successState;
	return;
}

///-----------------------------------------------------------------
/// Adds an opponent to the current round
///-----------------------------------------------------------------
void OSIPS2::addOpponent( qUnsignedInt32 gatheringID, qUnsignedInt32 principleID )
{
	if ( _isHost )
	{
		if ( principleID != _activeAccount->player.id.ps2 )
		{
			_opponentsInRound++ ;
		}
	}
	else
	{
		_roundID = gatheringID ;
	}
}


///-----------------------------------------------------------------
/// Fills in the personal stats map
///-----------------------------------------------------------------
void OSIPS2::_populatePersonalStats()
{
	// We're assuming _roundStats was filled in by requestEndRound prior to this call.
	const OSIPlayerStats *playerStats = 0;

	_lstPersonalStats.clear();
	int count = _roundStats.playerStatsList.count();
	for ( int i = 0; i < count; i++ )
	{
		const OSIPlayerStats* stats = _roundStats.playerStatsList[i];

		if ( stats->playerID.ps2 == _activeAccount->player.id.ps2 )
		{
			playerStats = stats;
			break;
		}
	}

	if ( !playerStats )
	{
		printf("-- OSI -- Non-Critical Error! Personal Stats NOT reported, player not found.");
		return;
	}

	for ( int i = OSIPlayerStats::FIRST_NONARBITRATED_STAT; i<OSIPlayerStats::NUM_PLAYERSTATS; i++ )
	{
		float value = playerStats->statsArray[i];
		_lstPersonalStats.push_back( value );
	}

}


//------------------------------------------------------------------------------------------------
//		requestCreateRound
///
/// \return True if the *request* (not the thing being requested) was successfully created.
///
bool OSIPS2::requestCreateRound()
{
	printf("-- OSI -- Request to CREATE round\n");

	setRequestType( REQUEST_TYPE_RENDEZVOUS_CREATE_ROUND );
	TTLSession* session = (TTLSession*)_currentGathering.Get();

	_isCompetitionRound = false ;
	OSIMatch* match = getCurrentMatch();
	if ((session->totalConnectedPlayers > 1) &&
		(match->options.isRanked == true) )
	{
		_isCompetitionRound = true ;
	}

	_state = CREATE_ROUND_CREATE_ROUND;
	return REQUEST_SUCCEEDED;
}

//------------------------------------------------------------------------------------------------
//		requestJoinRound
///
/// \return True if the *request* (not the thing being requested) was successfully created.
///
bool OSIPS2::requestJoinRound()
{
	printf("-- OSI -- Request to JOIN round\n");
	setRequestType( REQUEST_TYPE_RENDEZVOUS_JOIN_ROUND );
	_state = JOIN_ROUND;
	return REQUEST_SUCCEEDED;
}

//------------------------------------------------------------------------------------------------
//		requestStartRound
///
/// \return True if the *request* (not the thing being requested) was successfully created.
///
bool OSIPS2::requestStartRound()
{
	printf("-- OSI -- Request to START round\n");
	setRequestType( REQUEST_TYPE_RENDEZVOUS_START_ROUND );
	_state = LAUNCH_ROUND;
	return REQUEST_SUCCEEDED;
}



///----------------------------------------------------------
/// Requests the current match to end, with the ending condition
/// specified.  This should post the results to the RendezVous
/// server, and shut down the match.
///----------------------------------------------------------
bool OSIPS2::requestEndRound( OSIRoundStats& stats )
{
	printf("-- OSI -- Request to END round\n");

	_roundStats = stats;
	setRequestType( REQUEST_TYPE_RENDEZVOUS_END_ROUND );
	_state = FINISH_ROUND;

	_isCompetitionRound = false ;
	OSIMatch* match = getCurrentMatch();
	if ((stats.playerStatsList.count() > 1) &&
		(match->options.isRanked == true ) )
	{
		_isCompetitionRound = true ;
	}

	_populatePersonalStats();

	// This is an attempt to skip all end round steps if we're orphaned.  Unfortunately,
	// RendezVous's orphan session code doesn't appear to work at all.
//	if ( isOrphanSession() )
//	{
//		_state = COMMAND_SUCCESS ;
//	}

	if ( !_isHost )
	{
		_currentPlayerList.begin();
	}

	return true ;
}



///---------------------------------------------------------------------
/// This shuts down the TTLSession (the master session).
///---------------------------------------------------------------------
bool OSIPS2::requestQuitMatch()
{
	setRequestType( REQUEST_TYPE_RENDEZVOUS_QUIT_MATCH );
	_moveCurrentPlayersToRecentPlayers();
	_state = CANCEL_PARTICIPATION;
	return true;
}

///---------------------------------------------------------------------
/// Populate the recent players list with all our current players
/// and then clear out the current players list.
///---------------------------------------------------------------------
void OSIPS2::_moveCurrentPlayersToRecentPlayers()
{
	_currentPlayerList.begin();
	while( OSIPlayer* currentPlayer = _currentPlayerList.getCurrent() )
	{
		_removeRecentPlayer( *currentPlayer );
		_addRecentPlayer( *currentPlayer );

		_currentPlayerList.next();
	}

	_currentPlayerList.clear();
}
















































/// ***************************************************************************
///	 _                               ___ 
///	| |    ___  __ _  __ _  ___ _   |__ \
///	| |   / _ \/ _` |/ _` |/ __| | | |/ /
///	| |__|  __/ (_| | (_| | (__| |_| |_| 
///	|_____\___|\__, |\__,_|\___|\__, (_) 
///           |___/            |___/    
/// ***************************************************************************

/////----------------------------------------------------------------
///// Checks for Round Invitations
/////----------------------------------------------------------------
//bool OSIPS2::_processBeginRoundCheckForRoundInvitation( void )
//{
//	setActiveState(OSIPS2_STATE_ROUND_CHECKING_FOR_INVITATION);
//	_lstInvitations.clear();
//
//	if ( _roundID != 0 )
//	{
//		if ( !_matchMakingClient->FindByID( _currentContext, _roundID, &_lastCallResult, &_currentRound ) )
//		{
//			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_FIND_ROUND_INVITATION) );
//			return true ;
//		}
//		_stateBeginRound = STATE_BEGIN_ROUND_WAITING_ON_INVITATION_CHECK ;
//	}
//
//	return false ;
//}
//
//
//
/////----------------------------------------------------------------
///// Waits until the check for invitations completes.  When it completes,
///// if we didn't have an invitation, we go back to the previous
///// state to check again.  If we did get an invitation, we move on.
/////
///// Returns TRUE on failure (timeout)
/////----------------------------------------------------------------
//bool OSIPS2::_processBeginRoundWaitingOnInvitationCheck( void )
//{
//	setActiveState(OSIPS2_STATE_ROUND_WAITING_FOR_INVITATION_CHECK);
//	bool isOutcomeKnown = _currentContext->Wait( _defaultTimeSlice );
//
//	if ( isOutcomeKnown )
//	{
//		OSIPlayerID	ownerID ;
//		ownerID.ps2			= _currentGathering->GetOwnerPID();
//		bool		found	= _findRoundGatheringByOwnerID( ownerID );
//
//		_stateBeginRound	= found ? STATE_BEGIN_ROUND_JOIN_ROUND : STATE_BEGIN_ROUND_CHECK_FOR_ROUND_INVITATION ;
//	}
//
//	return false ;
//}

/////------------------------------------------------------
///// Invite opponents (hard-coded to invite second player in TTLSession)
/////------------------------------------------------------
//bool OSIPS2::_processBeginRoundInviteOpponents( void )
//{
//	setActiveState( OSIPS2_STATE_INVITING_OPPONENTS );
//
//	GatheringID gID				= _currentRound->GetID();
//
//	// OSIFIXME AV
//	//PxNetInterface::Quadword id = (PxNetInterface::Quadword)gID ;
//	//PxNetInterface::setLocalRoundID( 0, id );
//
//	//invite opponent to gathering here
//	_currentPlayerList.begin();
//	OSIPlayer*	player	= _currentPlayerList.getCurrent();
//	while ( player != NULL )
//	{
//		OSIPlayerID	pID		= player->id ;
//		if ( pID.ps2 != _activeAccount->player.id.ps2 )
//		{
//			break ;
//		}
//		_currentPlayerList.next();
//		player	= _currentPlayerList.getCurrent();
//	}
//
//	if ( player )
//	{
//		if( !_matchMakingClient->Invite( _currentContext, gID, &_lastCallResult, player->id.ps2, "TTL Round Invitation" ) )
//		{
//			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_INVITE_OPPONENTS) );
//			setLastRequestStatus( STATUS_FAILED );
//			return true ;
//		}
//	}
//
//	_stateBeginRound	= STATE_BEGIN_ROUND_WAITING_ON_INVITE ;
//	return false ;
//}
//
//
//
/////---------------------------------------------------------------
///// Wait for our invitation to succeed
/////---------------------------------------------------------------
//bool OSIPS2::_processBeginRoundWaitingOnInvitation( void )
//{
//	setActiveState( OSIPS2_STATE_HOST_WAITING_ON_INVITATION );
//
//	bool isOutcomeKnown	= _currentContext->Wait( _defaultTimeSlice );
//	if ( isOutcomeKnown )
//	{
//		if ( !_lastCallResult ) 
//		{
//			setErrorString(GetOSIStringByEnum(OSIPS2_ERROR_FAILED_TO_PARTICIPATE_WAITING_ON_JOIN));
//			return true ;
//		}
//		_stateBeginRound = STATE_BEGIN_ROUND_WAITING_ON_OPPONENTS;
//	}
//
//	// Handle Timeouts
//	static int waitCount = 0;
//	waitCount++ ;
//	if ( waitCount > _defaultTimeout )
//	{
//		waitCount = 0;
//		setLastRequestStatus(STATUS_FAILED);
//		setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_TIMEOUT_ON_WAITING_FOR_JOIN) );
//		return true;
//	}
//
//	return false ;	
//}

/////----------------------------------------------------------
///// Report stats
/////----------------------------------------------------------
//bool OSIPS2::_processEndRoundReportStats()
//{
//	setActiveState(OSIPS2_STATE_ROUND_REPORT_STATS);
//	_currentContext->Reset();
//	_lastCallResult	= true ;
//
//	qList<qFloat> lstStats;
//
//	_currentPlayerList.begin();
//	OSIPlayer*	player	= _currentPlayerList.getCurrent();
//	while ( player != NULL )
//	{
//		OSIPlayerID pID	= player->id ;
//		if ( pID.ps2 == _activeAccount->player.id.ps2 )
//		{
//			break ;
//		}
//		_currentPlayerList.next();
//		player	= _currentPlayerList.getCurrent();
//	}
//
//	// OSIFIXME_NPLAYERS (maybe?  could work)
//	if ( player->endRoundCondition == OSI_END_CONDITION_WIN )
//		lstStats.push_back(1.0f);
//	else
//		lstStats.push_back(0.0f); // Loss, from a real lose or a backdown. FIXME if we support draws.
//
//	GatheringID gID = _currentRound->GetID();
//	PrincipalID pID = _credentials->GetPrincipalID();
//
//	if ( !_matchMakingClient->ReportStats(_currentContext, gID, &_lastCallResult, pID, StatisticFlags::Finished, lstStats) )
//	{
//		setErrorString(GetOSIStringByEnum(OSIPS2_ERROR_UNABLE_TO_REPORT_STATS));
//		return true;
//	}
//
//	_stateEndRound = STATE_END_ROUND_WAITING_ON_STATS ;
//
//	return false;
//}
//
//
//
/////----------------------------------------------------------
///// Wait for reporting stats
/////----------------------------------------------------------
//bool OSIPS2::_processEndRoundWaitingOnReportStats()
//{
//	setActiveState(OSIPS2_STATE_ROUND_WAITING_ON_REPORT_STATS);
//	bool isOutcomeKnown = _currentContext->Wait( _defaultTimeSlice );
//	if ( isOutcomeKnown )
//	{
//		if ( !_lastCallResult )
//		{
//			setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_WAITING_TO_REPORT_STATS) );
//			return true;
//		} 
//
//		setLastRequestStatus( STATUS_SUCCEEDED );
//
//		// OSIFIXME_NPLAYERS
//		// If there's more players to report stats for, go back to the report stat
//		// state.  Otherwise we are going to unregister the round.
//		//_currentPlayerList.next();
//		//if ( _currentPlayerList.getCurrent() != NULL )
//		//	_stateEndRound = STATE_END_ROUND_REPORT_STATS;
//		//else
//		//{
//			// Host continues and unregisters... The joiner is done.
//			if ( _isHost )
//				_stateEndRound = STATE_END_ROUND_UNREGISTER_MATCH;
//			else
//				_stateEndRound = STATE_END_ROUND_REQUEST_SUCCEEDED;
//		//}
//
//		return false ;
//	}
//
//	// Handle Timeouts
//	static int waitCount = 0;
//	waitCount++ ;
//	if ( waitCount > _defaultTimeout )
//	{
//		waitCount = 0;
//		setLastRequestStatus( STATUS_FAILED );
//		setErrorString( GetOSIStringByEnum(OSIPS2_ERROR_WAITING_TO_REPORT_STATS) );
//		return true;
//	}
//	return false;
//}

///----------------------------------------------------------------------
/// Not implemented
///----------------------------------------------------------------------
bool OSIPS2::requestLadderMatch( OSIMatchOptions& options )
{
	return true;
}

///----------------------------------------------------------------------
/// Not implemented
///----------------------------------------------------------------------
bool OSIPS2::requestLaunchGame()
{
	return true ;
}



///----------------------------------------------------------------------
/// Not implemented
///----------------------------------------------------------------------
bool OSIPS2::requestLaunchChat()
{
	return true ;
}



///----------------------------------------------------------------------
/// Are there any new match invitations since last call to server?
///----------------------------------------------------------------------
bool OSIPS2::isNewMatchInvitationWaiting()
{
	return _alerts.AreNewMatchInvitations();
}

///----------------------------------------------------------------------
/// Are there any new clan invitations since last call to server?
///----------------------------------------------------------------------
bool OSIPS2::isNewClanInvitationWaiting()
{
	return _alerts.AreNewClanInvitations();
}

///----------------------------------------------------------------------
/// Are there any new friend invitations since last call to server?
///----------------------------------------------------------------------
bool OSIPS2::isNewFriendInvitationWaiting()
{
	return _alerts.AreNewFriendInvitations();
}



///----------------------------------------------------------------------
/// Not implemented
///----------------------------------------------------------------------
bool OSIPS2::requestSendPlayerFeedback( OSIPlayerID& id, OSIFeedback& feedback )
{
	return true ;
}



///----------------------------------------------------------------------
/// Not implemented
///----------------------------------------------------------------------
bool OSIPS2::requestMutePlayer( OSIPlayerID& id )
{
	return true ;
}



///----------------------------------------------------------------------
/// Not implemented
///----------------------------------------------------------------------
bool OSIPS2::requestRankingList()
{
	return true ;
}



///----------------------------------------------------------------------
/// Not implemented
///----------------------------------------------------------------------
bool OSIPS2::requestOpenVoiceConnection( OSIPlayerID& id )
{
	return true ;
}



///----------------------------------------------------------------------
/// Not implemented
///----------------------------------------------------------------------
bool OSIPS2::requestCloseVoiceConnection( OSIPlayerID& id )
{
	return true ;
}



///----------------------------------------------------------------------
/// Not implemented
///----------------------------------------------------------------------
bool OSIPS2::startVoiceClipRecording()
{
	return true ;
}



///----------------------------------------------------------------------
/// Not implemented
///----------------------------------------------------------------------
bool OSIPS2::stopVoiceClipRecording()
{
	return true ;
}



///----------------------------------------------------------------------
/// Not implemented
///----------------------------------------------------------------------
bool OSIPS2::attachVoiceClipToNextGameInvitation()
{
	return true ;
}



///----------------------------------------------------------------------
/// Legacy Code?
///----------------------------------------------------------------------
void OSIPS2::_populateHardwareProfileList( void )
{
#ifndef DIRECTX_PC
	return ;
	PS2Networking::ReadProfileListFromMC(&_oProfileList, 0);
	unsigned int numProfiles = _oProfileList.GetNbProfiles();
	for (unsigned int i=0; i< numProfiles; i++ )
    {

		PS2ConnectionProfile *rendezvousProfile = _oProfileList.GetProfile(i);

		if (rendezvousProfile != NULL)
        {
			OSIHardwareProfile	hardwareProfile ;
			hardwareProfile.id.ps2	= i ;
			OSIstrcpy( hardwareProfile.name, rendezvousProfile->GetProfileName());
			_hardwareProfileList.push_back( hardwareProfile );
		}
	}
#endif // DIRECTX_PC
}



///----------------------------------------------------------------------
/// Legacy Code?
///----------------------------------------------------------------------
void OSIPS2::_populateAccountList( void )
{
	OSIstrcpy( _guestAccount.player.name,	_T("Guest") );
	OSIstrcpy( _guestAccount.password,		_T(GUEST_PLAYER_PASSWORD) );

	_accountList.setNetworkDataReady(false);
}



///----------------------------------------------------------------------
/// Legacy Code?
///----------------------------------------------------------------------
void OSIPS2::updateInvitations()
{
	printf( "********* WARNING : OSIPS2::updateInvitations() called - this should be depricated!\n" );
}



///----------------------------------------------------------------------
/// Legacy Code?
///----------------------------------------------------------------------
bool OSIPS2::refreshPlayerList()
{
	return true;
}





/// ***************************************************************************
///   ____ _                 
///  / ___| | __ _ _ __  ___ 
/// | |   | |/ _` | '_ \/ __|
/// | |___| | (_| | | | \__ \
///  \____|_|\__,_|_| |_|___/
///
/// ***************************************************************************

/// ***************************************************************************
/// ERROR MAPPINGS FOR RENDEZVOUS
/// ***************************************************************************

///----------------------------------------------------------------------
/// These correspond with error string enums in OSIStringTable.hpp
/// The python server returns these error codes so make sure these
/// match with what is in TTLMatchMaking.py in the class EidosResCodes.
/// They are mapped in the function _checkClanCallCompletion.
///----------------------------------------------------------------------
enum
{
	RV_ERROR_NONE,

	RV_ERROR_GENERAL,
	RV_ERROR_PERMISSION_DENIED,
	RV_ERROR_NOT_IN_CLAN,
	RV_ERROR_INVALID_CLAN,
	RV_ERROR_INVALID_PLAYER,
	RV_ERROR_CLAN_ABBR_TAKEN,
	RV_ERROR_CLAN_NAME_TAKEN,
	RV_ERROR_ALREADY_IN_CLAN,
	RV_ERROR_ALREADY_INVITED,
	RV_ERROR_NOT_ACCEPTING_INVITES,
	RV_ERROR_INVALID_INVITATION,
	RV_ERROR_CLAN_NOT_EMPTY,
	RV_ERROR_CLAN_FULL,
	RV_ERROR_CLAN_NAME_NOT_ALLOWED,
	RV_ERROR_IN_SAME_CLAN,

	RV_ERROR_TOTAL
};

/// ***************************************************************************
/// CREATE A NEW CLAN WITH MYSELF AS LEADER
/// ***************************************************************************

///----------------------------------------------------------------------
/// Request to create a clan with the tag and name given
///----------------------------------------------------------------------
bool OSIPS2::requestClanCreate(const OSIchar* const tag, const OSIchar* const name)
{
	// Save off the tag and name for this attempt at creating a clan
	// Save off in UTF-8 format
	UTF8Encode( _newClanTag, tag, NUM_ARRAY_ELEMENTS(_newClanTag) );
	UTF8Encode( _newClanName, name, NUM_ARRAY_ELEMENTS(_newClanName) );

	setRequestType(REQUEST_TYPE_CLAN_CREATE);
	_state = CLAN_CREATE;
	return true;
}

///----------------------------------------------------------------------
/// Update state machine pump
///----------------------------------------------------------------------
bool OSIPS2::_updateClanCreate()
{
	switch (_state)
	{        // STATE                           // UPDATE FUNCTION	    // ON SUCCESS GO TO			// ON FAILURE GO TO 
		case CLAN_CREATE:						_processClanCreate		(COMMAND_SUCCESS,			COMMAND_FAILURE);	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus(STATUS_FAILED); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus(STATUS_SUCCEEDED);
			return true ; // complete, success
	}

	return false ; // keep working
}

///----------------------------------------------------------------------
/// Process the request.  Calls to rendezvous and checks for completion.
///----------------------------------------------------------------------
void OSIPS2::_processClanCreate(int successState, int failureState)
{
	setLastRequestStatus(STATUS_WORKING);
	// Is this our first time through this process?
	if (!_commandInitiated)
	{
		bool result	= _TTLMatchMakingClient->CallClanCreateClan(_currentContext, &_lastClanCallResult, _newClanName, _newClanTag);
		if (!result)
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_CLAN_REQUEST_CLAN_CREATE_FAILED));
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	_checkClanCallCompletion(successState, failureState);
}

/// ***************************************************************************
/// UPDATE MY CLAN INFORMATION (NAME, TAG, ROLE)
/// ***************************************************************************

///----------------------------------------------------------------------
/// Request to get all my clan information
///----------------------------------------------------------------------
bool OSIPS2::requestClanUpdateMyClanInformation()
{
	_currentPlayerID = _activeAccount->player.id;

	setRequestType(REQUEST_TYPE_CLAN_UPDATE_MY_INFO);
	_state = CLAN_UPDATE_MY_INFO;
	return true;
}

bool OSIPS2::_updateClanUpdateMyClanInformation()
{
	switch (_state)
	{        // STATE                        // UPDATE FUNCTION	        // ON SUCCESS GO TO			// ON FAILURE GO TO 
		case CLAN_UPDATE_MY_INFO:			_processClanUpdateMyClanInformation(COMMAND_SUCCESS,	COMMAND_FAILURE);	break ;
		case COMMAND_FAILURE:
			setLastRequestStatus(STATUS_FAILED); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:
			// set up our RendezVous list representations
			_lstClanID.clear();
			_lstClans.clear();
			if(_currentClan.m_ClanAbbr != "") {
				// we are in a clan, add the data to our lists
				_lstClanID.push_back(_currentClan.m_ClanID);
				_lstClans.push_back(_currentClan);
			}
			// tell OSI the data is ready
			_clanIDList.setNetworkDataReady(true);
			_clanInfoList.setNetworkDataReady(true);
			_setActiveAccountPlayerClanInfo();
			setLastRequestStatus(STATUS_SUCCEEDED);
			return true ; // complete, success
	}

	return false ; // keep working
}

void OSIPS2::_processClanUpdateMyClanInformation(int successState, int failureState)
{
	setLastRequestStatus(STATUS_WORKING);
	// Is this our first time through this process?
	if (!_commandInitiated)
	{
		// call RendezVous across the network
		bool result	= _TTLMatchMakingClient->CallClanGetMyClanInfo(
			_currentContext, &_lastClanCallResult,
			&_currentClan, &_currentClanMember
		);
		if (!result)
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_CLAN_REQUEST_CLAN_UPDATE_MY_INFO_FAILED));
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	_checkClanCallCompletion(successState, failureState);
}

/// ***************************************************************************
/// JOIN A CLAN BY AN INVITATION SENT TO ME
/// ***************************************************************************

///----------------------------------------------------------------------
/// Request to join a clan with the given invitation
///----------------------------------------------------------------------
bool OSIPS2::requestClanJoinByInvitation(const OSIClanInvitation &invitation)
{
	_currentClanInvitation = invitation;

	setRequestType(REQUEST_TYPE_CLAN_JOIN_BY_INVITATION);
	_state = CLAN_JOIN_BY_INVITATION;
	return true;
}

///----------------------------------------------------------------------
/// Update state machine pump
///----------------------------------------------------------------------
bool OSIPS2::_updateClanJoinByInvitation()
{
	switch (_state)
	{        // STATE                           // UPDATE FUNCTION	    // ON SUCCESS GO TO			// ON FAILURE GO TO 
		case CLAN_JOIN_BY_INVITATION:			_processClanJoinByInvitation(COMMAND_SUCCESS,			COMMAND_FAILURE);	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus(STATUS_FAILED); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus(STATUS_SUCCEEDED);
			return true ; // complete, success
	}

	return false ; // keep working
}

///----------------------------------------------------------------------
/// Process the request.  Calls to rendezvous and checks for completion.
///----------------------------------------------------------------------
void OSIPS2::_processClanJoinByInvitation(int successState, int failureState)
{
	setLastRequestStatus(STATUS_WORKING);
	// Is this our first time through this process?
	if (!_commandInitiated)
	{
		// call RendezVous across the network
		bool result	= _TTLMatchMakingClient->CallClanAcceptInvitation(
			_currentContext, &_lastClanCallResult,
			_currentClanInvitation.clanInfo.clanID.ps2,
			_currentClanInvitation.senderName
		);
		if (!result)
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_CLAN_REQUEST_CLAN_JOIN_BY_INVITATION_FAILED));
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	_checkClanCallCompletion(successState, failureState);
}

/// ***************************************************************************
/// DENY AN INVITATION INTO A CLAN
/// ***************************************************************************

///----------------------------------------------------------------------
/// Request to deny the given invitation
///----------------------------------------------------------------------
bool OSIPS2::requestClanDenyInvitation(const OSIClanInvitation& invitation)
{
	_currentClanInvitation = invitation;

	setRequestType(REQUEST_TYPE_CLAN_DENY_INVITATION);
	_state = CLAN_DENY_INVITATION;
	return true;
}

///----------------------------------------------------------------------
/// Update state machine pump
///----------------------------------------------------------------------
bool OSIPS2::_updateClanDenyInvitation()
{
	switch (_state)
	{        // STATE                           // UPDATE FUNCTION	    // ON SUCCESS GO TO			// ON FAILURE GO TO 
		case CLAN_DENY_INVITATION:				_processClanDenyInvitation		(COMMAND_SUCCESS,			COMMAND_FAILURE);	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus(STATUS_FAILED); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus(STATUS_SUCCEEDED);
			return true ; // complete, success
	}

	return false ; // keep working
}

///----------------------------------------------------------------------
/// Process the request.  Calls to rendezvous and checks for completion.
///----------------------------------------------------------------------
void OSIPS2::_processClanDenyInvitation(int successState, int failureState)
{
	setLastRequestStatus(STATUS_WORKING);
	// Is this our first time through this process?
	if (!_commandInitiated)
	{
		// call RendezVous across the network
		bool result	= _TTLMatchMakingClient->CallClanRejectInvitation(
			_currentContext,&_lastClanCallResult,
			_currentClanInvitation.clanInfo.clanID.ps2,
			_currentClanInvitation.senderName
		);
		if (!result)
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_CLAN_REQUEST_CLAN_DENY_INVITATION_FAILED));
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	_checkClanCallCompletion(successState, failureState);
}

/// ***************************************************************************
/// BLOCK SENDER OF AN INVITATION FROM SENDING ME CLAN INVITES
/// ***************************************************************************

///----------------------------------------------------------------------
/// Request to block the user of the given invitation
///----------------------------------------------------------------------
bool OSIPS2::requestClanBlockInvitation(const OSIClanInvitation& invitation)
{
	_currentClanInvitation = invitation;

	setRequestType(REQUEST_TYPE_CLAN_BLOCK_INVITATION);
	_state = CLAN_BLOCK_INVITATION;
	return true;
}

///----------------------------------------------------------------------
/// Update state machine pump
///----------------------------------------------------------------------
bool OSIPS2::_updateClanBlockInvitation()
{
	switch (_state)
	{        // STATE                           // UPDATE FUNCTION	    // ON SUCCESS GO TO			// ON FAILURE GO TO 
		case CLAN_BLOCK_INVITATION:				_processClanBlockInvitation		(COMMAND_SUCCESS,			COMMAND_FAILURE);	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus(STATUS_FAILED); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus(STATUS_SUCCEEDED);
			return true ; // complete, success
	}

	return false ; // keep working
}

///----------------------------------------------------------------------
/// Process the request.  Calls to rendezvous and checks for completion.
///----------------------------------------------------------------------
void OSIPS2::_processClanBlockInvitation(int successState, int failureState)
{
	setLastRequestStatus(STATUS_WORKING);
	// Is this our first time through this process?
	if (!_commandInitiated)
	{
		// call RendezVous across the network
		bool result	= _TTLMatchMakingClient->CallClanBlockInvitation(
			_currentContext, &_lastClanCallResult,
			_currentClanInvitation.clanInfo.clanID.ps2,
			_currentClanInvitation.senderName
		);
		if (!result)
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_CLAN_REQUEST_CLAN_BLOCK_INVITATION_FAILED));
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	_checkClanCallCompletion(successState, failureState);
}

/// ***************************************************************************
/// SEND AN INVITATION FOR A PLAYER TO JOIN MY CLAN
/// ***************************************************************************

///----------------------------------------------------------------------
/// Request to send an invitation into my clan to the given player
///----------------------------------------------------------------------
bool OSIPS2::requestClanSendInvitation(const OSIClanID& clanID, const OSIchar* const playerName)
{
	_currentClanID = clanID;
	UTF8Encode(_currentRequestStr, playerName, NUM_ARRAY_ELEMENTS(_currentRequestStr));

	setRequestType(REQUEST_TYPE_CLAN_SEND_INVITATION);
	_state = CLAN_SEND_INVITATION;
	return true;
}

///----------------------------------------------------------------------
/// Update state machine pump
///----------------------------------------------------------------------
bool OSIPS2::_updateClanSendInvitation()
{
	switch (_state)
	{        // STATE                           // UPDATE FUNCTION	    // ON SUCCESS GO TO			// ON FAILURE GO TO 
		case CLAN_SEND_INVITATION:				_processClanSendInvitation		(COMMAND_SUCCESS,			COMMAND_FAILURE);	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus(STATUS_FAILED); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus(STATUS_SUCCEEDED);
			return true ; // complete, success
	}

	return false ; // keep working
}

///----------------------------------------------------------------------
/// Process the request.  Calls to rendezvous and checks for completion.
///----------------------------------------------------------------------
void OSIPS2::_processClanSendInvitation(int successState, int failureState)
{
	setLastRequestStatus(STATUS_WORKING);
	// Is this our first time through this process?
	if (!_commandInitiated)
	{
		// Set up done here

		// call RendezVous across the network
		bool result	= _TTLMatchMakingClient->CallClanInvitePlayer(_currentContext, &_lastClanCallResult, _currentRequestStr);
		if (!result)
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_CLAN_REQUEST_CLAN_SEND_INVITATION_FAILED));
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	_checkClanCallCompletion(successState, failureState);
}

/// ***************************************************************************
/// CANCEL AN INVITATION I HAVE SENT
/// ***************************************************************************

///----------------------------------------------------------------------
/// Request to cancel an invitation
///----------------------------------------------------------------------
bool OSIPS2::requestClanCancelSentInvitation(const OSIClanInvitation& clanInvite)
{
	_currentClanInvitation = clanInvite;

	setRequestType(REQUEST_TYPE_CLAN_CANCEL_SENT_INVITATION);
	_state = CLAN_CANCEL_SENT_INVITATION;
	return true;
}

///----------------------------------------------------------------------
/// Update state machine pump
///----------------------------------------------------------------------
bool OSIPS2::_updateClanCancelSentInvitation()
{
	switch (_state)
	{
		case CLAN_CANCEL_SENT_INVITATION:	_processClanCancelSentInvitation	(COMMAND_SUCCESS,			COMMAND_FAILURE);	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus(STATUS_FAILED); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus(STATUS_SUCCEEDED);
			return true ; // complete, success
	}

	return false ; // keep working
}

///----------------------------------------------------------------------
/// Process the request.  Calls to rendezvous and checks for completion.
///----------------------------------------------------------------------
void OSIPS2::_processClanCancelSentInvitation(int successState, int failureState)
{
	setLastRequestStatus(STATUS_WORKING);
	// Is this our first time through this process?
	if (!_commandInitiated)
	{
		// call RendezVous across the network
		bool result	= _TTLMatchMakingClient->CallClanCancelInvitation(_currentContext, &_lastClanCallResult, _currentClanInvitation.receiverName);
		if (!result)
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_CLAN_REQUEST_CLAN_CANCEL_SENT_INVITATION_FAILED));
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	_checkClanCallCompletion(successState, failureState);
}

/// ***************************************************************************
/// GET MY CLAN INVITATION LIST (BOTH SENT AND RECEIVED)
/// ***************************************************************************

///----------------------------------------------------------------------
/// Request my clan invitation list of sent and received clan invites
///----------------------------------------------------------------------
bool OSIPS2::requestClanInvitationList()
{

	setRequestType(REQUEST_TYPE_CLAN_INVITATION_LIST);
	_state = CLAN_INVITATION_LIST;
	return true;
}

///----------------------------------------------------------------------
/// Update state machine pump
///----------------------------------------------------------------------
bool OSIPS2::_updateClanInvitationList()
{
	switch (_state)
	{        // STATE                           // UPDATE FUNCTION	    // ON SUCCESS GO TO			// ON FAILURE GO TO 
		case CLAN_INVITATION_LIST:				_processClanInvitationList		(COMMAND_SUCCESS,			COMMAND_FAILURE);	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus(STATUS_FAILED); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus(STATUS_SUCCEEDED);
			/// Tell the process we have new data ready
			_clanInvitationList.setNetworkDataReady(true);
			_clanSentInvitationList.setNetworkDataReady(true);
			return true ; // complete, success
	}

	return false ; // keep working
}

///----------------------------------------------------------------------
/// Process the request.  Calls to rendezvous and checks for completion.
///----------------------------------------------------------------------
void OSIPS2::_processClanInvitationList(int successState, int failureState)
{
	setLastRequestStatus(STATUS_WORKING);
	// Is this our first time through this process?
	if (!_commandInitiated)
	{
		// call RendezVous across the network
		bool result	= _TTLMatchMakingClient->CallClanGetMyInvitations(_currentContext, &_lastClanCallResult,
						&_lstClanInvitesToMe, &_lstClanInvitesFromMe);
		if (!result)
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_CLAN_REQUEST_CLAN_INVITATION_LIST_FAILED));
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	_checkClanCallCompletion(successState, failureState);
}

///----------------------------------------------------------------------
/// Converts RendezVous data format to OSI data format
///----------------------------------------------------------------------
void OSIPS2::_populateClanInvitationList()
{
	_clanInvitationList.clear();
	/// Transfer the Rendezvous data over to the OSI data format
	EidosClanInvite rvInvite;		// rendezvous representation
	OSIClanInvitation osiInvite;	// OSI representation
	qList<EidosClanInvite>::iterator clanInvIter;
	// iterate over clan invitations to me
	for(clanInvIter = _lstClanInvitesToMe.begin(); clanInvIter != _lstClanInvitesToMe.end(); ++clanInvIter)
	{
		rvInvite = (*clanInvIter);
		osiInvite.clanInfo.clanID.ps2 = rvInvite.m_InviterClanID;
		UTF8Decode(osiInvite.clanInfo.tag, rvInvite.m_InviterClanAbbr.CStr(), OSIClanInfo::MAX_TAG_LENGTH);
		UTF8Decode(osiInvite.clanInfo.name, rvInvite.m_InviterClanName.CStr(), OSIClanInfo::MAX_NAME_LENGTH);
		osiInvite.clanInfo.memberCount = rvInvite.m_InviterClanMembers;
		UTF8Decode(osiInvite.senderName, rvInvite.m_InviterPlayer.CStr(), NUM_ARRAY_ELEMENTS(osiInvite.senderName));
		UTF8Decode(osiInvite.receiverName, rvInvite.m_InviteePlayer.CStr(), NUM_ARRAY_ELEMENTS(osiInvite.receiverName));
		osiInvite.clanInfo.stats.setStat( OSIClanStats::CLANSTAT_RANK, rvInvite.m_InviterClanRank );
		_clanInvitationList.push_back(osiInvite);
	}
	_clanInvitationList.setNetworkDataReady(false);

	_lstClanInvitesToMe.resize( 0 );
}

///----------------------------------------------------------------------
/// Converts RendezVous data format to OSI data format
///----------------------------------------------------------------------
void OSIPS2::_populateClanSentInvitationList()
{
	_clanSentInvitationList.clear();
	/// Transfer the Rendezvous data over to the OSI data format
	EidosClanInvite rvInvite;		// rendezvous representation
	OSIClanInvitation osiInvite;	// OSI representation
	qList<EidosClanInvite>::iterator clanInvIter;
	// iterate over clan invitations sent by me
	for(clanInvIter = _lstClanInvitesFromMe.begin(); clanInvIter != _lstClanInvitesFromMe.end(); ++clanInvIter)
	{
		rvInvite = (*clanInvIter);
		osiInvite.clanInfo.clanID.ps2 = rvInvite.m_InviterClanID;
		UTF8Decode(osiInvite.clanInfo.tag, rvInvite.m_InviterClanAbbr.CStr(), OSIClanInfo::MAX_TAG_LENGTH);
		UTF8Decode(osiInvite.clanInfo.name, rvInvite.m_InviterClanName.CStr(), OSIClanInfo::MAX_NAME_LENGTH);
		osiInvite.clanInfo.memberCount = rvInvite.m_InviterClanMembers;
		UTF8Decode(osiInvite.senderName, rvInvite.m_InviterPlayer.CStr(), NUM_ARRAY_ELEMENTS(osiInvite.senderName));
		UTF8Decode(osiInvite.receiverName, rvInvite.m_InviteePlayer.CStr(), NUM_ARRAY_ELEMENTS(osiInvite.receiverName));
		osiInvite.clanInfo.stats.setStat( OSIClanStats::CLANSTAT_RANK, rvInvite.m_InviterClanRank );
		_clanSentInvitationList.push_back(osiInvite);
	}
	_clanSentInvitationList.setNetworkDataReady(false);

	_lstClanInvitesFromMe.resize( 0 );
}

/// ***************************************************************************
/// GET LIST OF CLAN ID'S FOR PLAYER (EMPTY LIST FOR NO CLAN AFFILIATION)
/// ***************************************************************************

///----------------------------------------------------------------------
/// Request for a list of clan ID's associated with the given player ID
///----------------------------------------------------------------------
bool OSIPS2::requestClanIDListForPlayer(const OSIPlayerID& playerID)
{
	/// Save off the player ID for this request.
	_currentPlayerID = playerID;

	setRequestType(REQUEST_TYPE_CLAN_ID_LIST_FOR_PLAYER);
	_state = CLAN_ID_LIST_FOR_PLAYER;
	return true;
}

///----------------------------------------------------------------------
/// Update state machine pump
///----------------------------------------------------------------------
bool OSIPS2::_updateClanIdListForPlayer()
{
	switch (_state)
	{        // STATE                           // UPDATE FUNCTION	         // ON SUCCESS GO TO		// ON FAILURE GO TO 
		case CLAN_ID_LIST_FOR_PLAYER:			_processClanIdListForPlayer	(COMMAND_SUCCESS,			COMMAND_FAILURE);	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus(STATUS_FAILED); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus(STATUS_SUCCEEDED);
			/// Tell the process we have new data ready
			_clanIDList.setNetworkDataReady(true);
			return true ; // complete, success
	}
	return false;
}

///----------------------------------------------------------------------
/// Process the request.  Calls to rendezvous and checks for completion.
///----------------------------------------------------------------------
void OSIPS2::_processClanIdListForPlayer(int successState, int failureState)
{
	setLastRequestStatus(STATUS_WORKING);
	// Is this our first time through this process?
	if (!_commandInitiated)
	{
		// Call RendezVous with our data
		bool result	= _TTLMatchMakingClient->CallClanGetClanIDListForPlayer(_currentContext, &_lastClanCallResult, _currentPlayerID.ps2, &_lstClanID);
		if (!result)
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_CLAN_REQUEST_CLAN_ID_LIST_FOR_PLAYER_FAILED));
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();

	}

	_checkClanCallCompletion(successState, failureState);
}

///----------------------------------------------------------------------
/// Converts RendezVous data format to OSI data format
///----------------------------------------------------------------------
void OSIPS2::_populateClanIDList()
{
	/// Reset our current clan ID list
	_clanIDList.clear();
	/// Transfer the Rendezvous data over to the OSI data format
	qList<OSIu32>::iterator clanIDIter;
	for (clanIDIter = _lstClanID.begin(); clanIDIter != _lstClanID.end(); ++clanIDIter)
	{
		OSIClanID myID;
		myID.ps2 = (*clanIDIter);
		_clanIDList.push_back(myID);
	}
	_clanIDList.setNetworkDataReady(false);
}

/// ***************************************************************************
/// GET CLAN INFORMATION ON THE GIVEN CLAN ID
/// ***************************************************************************

///----------------------------------------------------------------------
/// Request for a clan info struct that matches the given clanID
///----------------------------------------------------------------------
bool OSIPS2::requestClanInfoList(const OSIClanID& clanID)
{
	/// Save off the clan ID for this request.
	_currentClanID = clanID;

	setRequestType(REQUEST_TYPE_CLAN_INFO);
	_state = CLAN_INFO;
	return true;
}

///----------------------------------------------------------------------
/// Update state machine pump
///----------------------------------------------------------------------
bool OSIPS2::_updateClanInfoList()
{
	switch (_state)
	{        // STATE                   // UPDATE FUNCTION	    // ON SUCCESS GO TO			// ON FAILURE GO TO 
		case CLAN_INFO:					_processClanInfoList	(COMMAND_SUCCESS,			COMMAND_FAILURE);	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus(STATUS_FAILED); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus(STATUS_SUCCEEDED);
			/// Tell the process we have new data ready
			_clanInfoList.setNetworkDataReady(true);
			return true ; // complete, success
	}

	return false ; // keep working
}

///----------------------------------------------------------------------
/// Process the request.  Calls to rendezvous and checks for completion.
///----------------------------------------------------------------------
void OSIPS2::_processClanInfoList(int successState, int failureState)
{
	setLastRequestStatus(STATUS_WORKING);
	// Is this our first time through this process?
	if (!_commandInitiated)
	{
		bool result	= _TTLMatchMakingClient->CallClanGetClanInfoFromClanID(_currentContext, &_lastClanCallResult, _currentClanID.ps2, &_currentClan);
		if (!result)
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_CLAN_REQUEST_CLAN_INFO_LIST_FAILED));
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	_checkClanCallCompletion(successState, failureState);
}

///----------------------------------------------------------------------
/// Converts RendezVous data format to OSI data format
///----------------------------------------------------------------------
void OSIPS2::_populateClanInfoList()
{
	_clanInfoList.clear();
	/// Add our current clan we got from RendezVous to our clan list
	OSIClanInfo osiClanInfo;
	osiClanInfo.clanID = _currentClanID;
	UTF8Decode(osiClanInfo.tag, _currentClan.m_ClanAbbr.CStr(), NUM_ARRAY_ELEMENTS(osiClanInfo.tag));
	UTF8Decode(osiClanInfo.name, _currentClan.m_ClanName.CStr(), NUM_ARRAY_ELEMENTS(osiClanInfo.name));
	UTF8Decode(osiClanInfo.leaderName, _currentClan.m_ClanLeader.CStr(), NUM_ARRAY_ELEMENTS(osiClanInfo.leaderName));
	osiClanInfo.stats.statsArray[ OSIClanStats::CLANSTAT_RANK ] = _currentClan.m_ClanRank;
	osiClanInfo.stats.statsArray[ OSIClanStats::CLANSTAT_RATING ] = _currentClan.m_ClanRating;
	osiClanInfo.stats.statsArray[ OSIClanStats::CLANSTAT_KILLS ] = _currentClan.m_ClanKills;
	osiClanInfo.memberCount = _currentClan.m_ClanMemberCount;
	_clanInfoList.push_back(osiClanInfo);

	_clanInfoList.setNetworkDataReady(false);
}

/// ***************************************************************************
/// GET LIST OF ALL MY CLAN MEMBERS
/// ***************************************************************************

///----------------------------------------------------------------------
/// Request for my clan membership list
///----------------------------------------------------------------------
bool OSIPS2::requestClanMembershipList(const OSIClanID& /*ignoring clan id*/)
{
	// Ignoring the clan ID for now; assuming current clan of logged in player

	setRequestType(REQUEST_TYPE_CLAN_MEMBERSHIP_LIST);
	_state = CLAN_MEMBERSHIP_LIST;
	return true;
}

///----------------------------------------------------------------------
/// Update state machine pump
///----------------------------------------------------------------------
bool OSIPS2::_updateClanMembershipList()
{
	switch (_state)
	{        // STATE                           // UPDATE FUNCTION	    // ON SUCCESS GO TO			// ON FAILURE GO TO 
		case CLAN_MEMBERSHIP_LIST:				_processClanMembershipList		(COMMAND_SUCCESS,			COMMAND_FAILURE);	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus(STATUS_FAILED); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus(STATUS_SUCCEEDED);
			/// Tell the process we have new data ready
			_clanMembershipList.setNetworkDataReady(true);
			return true ; // complete, success
	}

	return false ; // keep working
}

///----------------------------------------------------------------------
/// Process the request.  Calls to rendezvous and checks for completion.
///----------------------------------------------------------------------
void OSIPS2::_processClanMembershipList(int successState, int failureState)
{
	setLastRequestStatus(STATUS_WORKING);
	// Is this our first time through this process?
	if (!_commandInitiated)
	{
		// Set up done here

		// call RendezVous across the network
		bool result	= _TTLMatchMakingClient->CallClanGetMyClanMembershipList(_currentContext, &_lastClanCallResult, &_lstClanMembers);
		if (!result)
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_CLAN_REQUEST_CLAN_MEMBERSHIP_LIST_FAILED));
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	_checkClanCallCompletion(successState, failureState);
}

///----------------------------------------------------------------------
/// Converts RendezVous data format to OSI data format
///----------------------------------------------------------------------
void OSIPS2::_populateClanMembershipList()
{
	/// Reset our current clan ID list
	_clanMembershipList.clear();
	/// Transfer the Rendezvous data over to the OSI data format
	qList<EidosClanMember>::iterator clanMemberIter;
	for(clanMemberIter = _lstClanMembers.begin(); clanMemberIter != _lstClanMembers.end(); ++clanMemberIter)
	{
		OSIClanMembership osiClanMember;
		EidosClanMember rvClanMember = (*clanMemberIter);
		osiClanMember.playerID.ps2 = rvClanMember.m_PlayerID;
		UTF8Decode(osiClanMember.memberName, rvClanMember.m_Player.CStr(), OSI_PLAYER_NAME_LENGTH );
		osiClanMember.role = _getOSIClanRoleFromRendezVous(rvClanMember.m_ClanIsLeader, rvClanMember.m_ClanIsOfficer);
		// Is this member in a match?
		if(rvClanMember.m_NetLobbyMatch.size() > 0) {
			OSIMatch			osiMatch;
			// get the RendezVous representation of a match
			GatheringHolder&	rvHolder = rvClanMember.m_NetLobbyMatch.front();
			// cast that to the game specific session
			TTLSession*			session = (TTLSession*)rvHolder.Get();

			if(session) {
				_sessionToMatch(*session, osiMatch);
				osiClanMember.currentMatchList->push_back(osiMatch);
			}
		}
		osiClanMember.isOnline = rvClanMember.m_IsOnline;
		osiClanMember.isFriend = rvClanMember.m_IsFriend;
		_clanMembershipList.push_back(osiClanMember);
	}
	_clanMembershipList.setNetworkDataReady(false);

	_lstClanMembers.resize( 0 );
}

/// ***************************************************************************
/// LEAVE MY CURRENT CLAN
/// ***************************************************************************

///----------------------------------------------------------------------
/// Request to leave my current clan
///----------------------------------------------------------------------
bool OSIPS2::requestClanLeave(const OSIClanID& /*ignoring clan id*/)
{
	// Ignoring the clan ID for now; assuming current clan of logged in player

	setRequestType(REQUEST_TYPE_CLAN_LEAVE);
	_state = CLAN_LEAVE;
	return true;
}

///----------------------------------------------------------------------
/// Update state machine pump
///----------------------------------------------------------------------
bool OSIPS2::_updateClanLeave()
{
	bool failed = false;
	switch (_state)
	{        // STATE                           // UPDATE FUNCTION	    // ON SUCCESS GO TO			// ON FAILURE GO TO 
		case CLAN_LEAVE:						_processClanLeave		(COMMAND_SUCCESS,			COMMAND_FAILURE);	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus(STATUS_FAILED); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:
			setLastRequestStatus(STATUS_SUCCEEDED);
			return true ; // complete, success
	}

	return false ; // keep working
}

///----------------------------------------------------------------------
/// Process the request.  Calls to rendezvous and checks for completion.
///----------------------------------------------------------------------
void OSIPS2::_processClanLeave(int successState, int failureState)
{
	setLastRequestStatus(STATUS_WORKING);
	// Is this our first time through this process?
	if (!_commandInitiated)
	{
		// Set up done here

		// call RendezVous across the network
		bool result	= _TTLMatchMakingClient->CallClanAbandonClan(_currentContext, &_lastClanCallResult);
		if (!result)
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_CLAN_REQUEST_CLAN_LEAVE_FAILED));
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	_checkClanCallCompletion(successState, failureState);
}

/// ***************************************************************************
/// GET A LIST OF MY CLAN'S NOTICES
/// ***************************************************************************

///----------------------------------------------------------------------
/// Request my clan notice list
///----------------------------------------------------------------------
bool OSIPS2::requestClanNoticeList(const OSIClanID& /*ignoring clan id*/)
{
	// Ignoring the clan ID for now; assuming current clan of logged in player

	setRequestType(REQUEST_TYPE_CLAN_NOTICE_LIST);
	_state = CLAN_NOTICE_LIST;
	return true;
}

///----------------------------------------------------------------------
/// Update state machine pump
///----------------------------------------------------------------------
bool OSIPS2::_updateClanNoticeList()
{
	switch (_state)
	{        // STATE                           // UPDATE FUNCTION	    // ON SUCCESS GO TO			// ON FAILURE GO TO 
		case CLAN_NOTICE_LIST:					_processClanNoticeList		(COMMAND_SUCCESS,			COMMAND_FAILURE);	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus(STATUS_FAILED); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus(STATUS_SUCCEEDED);
			/// Tell the process we have new data ready
			_clanNoticeList.setNetworkDataReady(true);
			return true ; // complete, success
	}

	return false ; // keep working
}

///----------------------------------------------------------------------
/// Process the request.  Calls to rendezvous and checks for completion.
///----------------------------------------------------------------------
void OSIPS2::_processClanNoticeList(int successState, int failureState)
{
	setLastRequestStatus(STATUS_WORKING);
	// Is this our first time through this process?
	if (!_commandInitiated)
	{
		// Set up done here

		// call RendezVous across the network
		bool result	= _TTLMatchMakingClient->CallClanGetMyNotices(_currentContext, &_lastClanCallResult, &_lstClanNotices);
		if (!result)
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_CLAN_REQUEST_CLAN_NOTICE_LIST_FAILED));
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	_checkClanCallCompletion(successState, failureState);
}

///----------------------------------------------------------------------
/// Converts RendezVous data format to OSI data format
///----------------------------------------------------------------------
void OSIPS2::_populateClanNoticeList()
{
	/// Reset our current clan ID list
	_clanNoticeList.clear();
	/// Transfer the Rendezvous data over to the OSI data format
	qList<EidosClanNotice>::iterator clanNoticeIter;
	for(clanNoticeIter = _lstClanNotices.begin(); clanNoticeIter != _lstClanNotices.end(); ++clanNoticeIter)
	{
		OSIClanNotice osiClanNotice;
		EidosClanNotice rvClanNotice = (*clanNoticeIter);
		UTF8Decode(osiClanNotice.posterName, rvClanNotice.m_From.CStr(), NUM_ARRAY_ELEMENTS(osiClanNotice.posterName));
		UTF8Decode(osiClanNotice.noticeText, rvClanNotice.m_Body.CStr(), NUM_ARRAY_ELEMENTS(osiClanNotice.noticeText));
		_clanNoticeList.push_back(osiClanNotice);
	}
	_clanNoticeList.setNetworkDataReady(false);

	_lstClanNotices.resize( 0 );
}

/// ***************************************************************************
/// POST A NOTICE FOR MY CLAN
/// ***************************************************************************

///----------------------------------------------------------------------
/// Request to post a notice for my clan (only officer or leader)
///----------------------------------------------------------------------
bool OSIPS2::requestClanPostNotice(const OSIClanID& /*ignoring clan id*/, const OSIchar* const notice)
{
	// Ignoring the clan ID for now; assuming current clan of logged in player

	UTF8Encode(_currentRequestStr, notice, NUM_ARRAY_ELEMENTS(_currentRequestStr));

	setRequestType(REQUEST_TYPE_CLAN_POST_NOTICE);
	_state = CLAN_POST_NOTICE;
	return true;
}

///----------------------------------------------------------------------
/// Update state machine pump
///----------------------------------------------------------------------
bool OSIPS2::_updateClanPostNotice()
{
	switch (_state)
	{        // STATE                           // UPDATE FUNCTION	    // ON SUCCESS GO TO			// ON FAILURE GO TO 
		case CLAN_POST_NOTICE:					_processClanPostNotice		(COMMAND_SUCCESS,			COMMAND_FAILURE);	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus(STATUS_FAILED); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus(STATUS_SUCCEEDED);
			return true ; // complete, success
	}

	return false ; // keep working
}

///----------------------------------------------------------------------
/// Process the request.  Calls to rendezvous and checks for completion.
///----------------------------------------------------------------------
void OSIPS2::_processClanPostNotice(int successState, int failureState)
{
	setLastRequestStatus(STATUS_WORKING);
	// Is this our first time through this process?
	if (!_commandInitiated)
	{
		// call RendezVous across the network
		bool result	= _TTLMatchMakingClient->CallClanPostNotice(_currentContext, &_lastClanCallResult, _currentRequestStr);
		if (!result)
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_CLAN_REQUEST_CLAN_POST_NOTICE_FAILED));
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	_checkClanCallCompletion(successState, failureState);
}

/// ***************************************************************************
/// KICK A MEMBER FROM MY CLAN
/// ***************************************************************************

///----------------------------------------------------------------------
/// Request to kick a member of my clan.
/// Current functionality is only soldiers can be kicked by the leader
/// or officers.  So to kick an officer they must first be demoted.
///----------------------------------------------------------------------
bool OSIPS2::requestClanKickMember(const OSIClanID& /*ignoring clan id*/, const OSIPlayerID& playerID)
{
	// Ignoring the clan ID for now; assuming current clan of logged in player

	_currentPlayerID = playerID;

	setRequestType(REQUEST_TYPE_CLAN_KICK_MEMBER);
	_state = CLAN_KICK_MEMBER;
	return true;
}

///----------------------------------------------------------------------
/// Update state machine pump
///----------------------------------------------------------------------
bool OSIPS2::_updateClanKickMember()
{
	switch (_state)
	{        // STATE                           // UPDATE FUNCTION	    // ON SUCCESS GO TO			// ON FAILURE GO TO 
		case CLAN_KICK_MEMBER:					_processClanKickMember		(COMMAND_SUCCESS,			COMMAND_FAILURE);	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus(STATUS_FAILED); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus(STATUS_SUCCEEDED);
			return true ; // complete, success
	}

	return false ; // keep working
}

///----------------------------------------------------------------------
/// Process the request.  Calls to rendezvous and checks for completion.
///----------------------------------------------------------------------
void OSIPS2::_processClanKickMember(int successState, int failureState)
{
	setLastRequestStatus(STATUS_WORKING);
	// Is this our first time through this process?
	if (!_commandInitiated)
	{
		// call RendezVous across the network
		bool result	= _TTLMatchMakingClient->CallClanRemoveMember(_currentContext, &_lastClanCallResult, _currentPlayerID.ps2);
		if (!result)
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_CLAN_REQUEST_CLAN_KICK_MEMBER_FAILED));
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	_checkClanCallCompletion(successState, failureState);
}

/// ***************************************************************************
/// DEMOTE A MEMBER OF MY CLAN
/// ***************************************************************************

///----------------------------------------------------------------------
/// Request to demote a member of my clan
/// Only officers can be demoted to soldiers.
///----------------------------------------------------------------------
bool OSIPS2::requestClanDemoteMember(const OSIClanID& /*ignoring clan id*/, const OSIPlayerID& playerID)
{
	// Ignoring the clan ID for now; assuming current clan of logged in player

	_currentPlayerID = playerID;

	setRequestType(REQUEST_TYPE_CLAN_DEMOTE_MEMBER);
	_state = CLAN_DEMOTE_MEMBER;
	return true;
}

///----------------------------------------------------------------------
/// Update state machine pump
///----------------------------------------------------------------------
bool OSIPS2::_updateClanDemoteMember()
{
	switch (_state)
	{        // STATE                           // UPDATE FUNCTION	    // ON SUCCESS GO TO			// ON FAILURE GO TO 
		case CLAN_DEMOTE_MEMBER:				_processClanDemoteMember(COMMAND_SUCCESS,			COMMAND_FAILURE);	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus(STATUS_FAILED); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus(STATUS_SUCCEEDED);
			return true ; // complete, success
	}

	return false ; // keep working
}

///----------------------------------------------------------------------
/// Process the request.  Calls to rendezvous and checks for completion.
///----------------------------------------------------------------------
void OSIPS2::_processClanDemoteMember(int successState, int failureState)
{
	setLastRequestStatus(STATUS_WORKING);
	// Is this our first time through this process?
	if (!_commandInitiated)
	{
		// call RendezVous across the network
		bool result	= _TTLMatchMakingClient->CallClanDemote(_currentContext, &_lastClanCallResult, _currentPlayerID.ps2);
		if (!result)
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_CLAN_REQUEST_CLAN_DEMOTE_MEMBER_FAILED));
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	_checkClanCallCompletion(successState, failureState);
}

/// ***************************************************************************
/// PROMOTE A MEMBER OF MY CLAN
/// ***************************************************************************

///----------------------------------------------------------------------
/// Request to promote a member of my clan.
/// Currently this functionality is only for the leader to promote a
/// soldier to an officer.
///----------------------------------------------------------------------
bool OSIPS2::requestClanPromoteMember(const OSIClanID& /*ignoring clan id*/, const OSIPlayerID& playerID)
{
	// Ignoring the clan ID for now; assuming current clan of logged in player

	_currentPlayerID = playerID;

	setRequestType(REQUEST_TYPE_CLAN_PROMOTE_MEMBER);
	_state = CLAN_PROMOTE_MEMBER;
	return true;
}

///----------------------------------------------------------------------
/// Update state machine pump
///----------------------------------------------------------------------
bool OSIPS2::_updateClanPromoteMember()
{
	switch (_state)
	{        // STATE                           // UPDATE FUNCTION	    // ON SUCCESS GO TO			// ON FAILURE GO TO 
		case CLAN_PROMOTE_MEMBER:				_processClanPromoteMember(COMMAND_SUCCESS,			COMMAND_FAILURE);	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus(STATUS_FAILED); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus(STATUS_SUCCEEDED);
			return true ; // complete, success
	}

	return false ; // keep working
}

///----------------------------------------------------------------------
/// Process the request.  Calls to rendezvous and checks for completion.
///----------------------------------------------------------------------
void OSIPS2::_processClanPromoteMember(int successState, int failureState)
{
	setLastRequestStatus(STATUS_WORKING);
	// Is this our first time through this process?
	if (!_commandInitiated)
	{
		// call RendezVous across the network
		bool result	= _TTLMatchMakingClient->CallClanPromote(_currentContext, &_lastClanCallResult, _currentPlayerID.ps2);
		if (!result)
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_CLAN_REQUEST_CLAN_PROMOTE_MEMBER_FAILED));
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	_checkClanCallCompletion(successState, failureState);
}

/// ***************************************************************************
/// TRANSFER LEADERSHIP TO AN OFFICER
/// ***************************************************************************

///----------------------------------------------------------------------
/// Request to transfer leadership.
///----------------------------------------------------------------------
bool OSIPS2::requestClanTransferLeadership(const OSIClanID& /*ignoring clan id*/, const OSIPlayerID& playerID)
{
	// Ignoring the clan ID for now; assuming current clan of logged in player

	_currentPlayerID = playerID;

	setRequestType(REQUEST_TYPE_CLAN_TRANSFER_LEADERSHIP);
	_state = CLAN_TRANSFER_LEADERSHIP;
	return true;
}

///----------------------------------------------------------------------
/// Update state machine pump
///----------------------------------------------------------------------
bool OSIPS2::_updateClanTransferLeadership()
{
	switch (_state)
	{
		case CLAN_TRANSFER_LEADERSHIP:	_processClanTransferLeadership(COMMAND_SUCCESS,			COMMAND_FAILURE);	break ;
		case COMMAND_FAILURE:			
			setLastRequestStatus(STATUS_FAILED); 
			return true ; // complete, failure
		case COMMAND_SUCCESS:						
			setLastRequestStatus(STATUS_SUCCEEDED);
			return true ; // complete, success
	}

	return false ; // keep working
}

///----------------------------------------------------------------------
/// Process the request.  Calls to rendezvous and checks for completion.
///----------------------------------------------------------------------
void OSIPS2::_processClanTransferLeadership(int successState, int failureState)
{
	setLastRequestStatus(STATUS_WORKING);
	// Is this our first time through this process?
	if (!_commandInitiated)
	{
		// call RendezVous across the network
		bool result	= _TTLMatchMakingClient->CallClanNewLeader(_currentContext, &_lastClanCallResult, _currentPlayerID.ps2);
		if (!result)
		{
			setErrorString(GetOSIStringByEnum(OSIPS2_CLAN_REQUEST_CLAN_TRANSFER_LEADERSHIP_FAILED));
			_state = failureState;
			return;
		}
		_commandInitiated	= true;
		_resetTimeoutTimer();
	}

	_checkClanCallCompletion(successState, failureState);
}

/// ***************************************************************************
/// CLAN HELPER FUNCTIONS
/// ***************************************************************************

///----------------------------------------------------------------------
/// Check on a clan call for completion and behave appropriately
///----------------------------------------------------------------------
void OSIPS2::_checkClanCallCompletion(int successState, int failureState)
{
	if(_checkForCommandCompletionOrTimeout(_state, failureState)) {
		// Command to RendezVous has completed, change state based on result
		if(_lastClanCallResult == RV_ERROR_NONE) {
			// Command completed succesfully
			_state = successState;
		}
		else {
			// Command completed with error, set proper string
			ASSERT(_lastClanCallResult < RV_ERROR_TOTAL);
			// Since the OSI errors are in contiguous order but not necessarily starting
			// at 0 but the RendezVous errors are contiguous starting at 1,
			// the mapping is just the first error in OSI plus RendezVous' error minus 1.
			setErrorString(GetOSIStringByEnum( (OSIStringEnum)(OSIPS2_CLAN_GENERAL + _lastClanCallResult - 1) ));
			_state = failureState;
		}
		_commandInitiated = false;
	}
}

///----------------------------------------------------------------------
/// Given a RendezVous representation of roles, convert to OSI
///----------------------------------------------------------------------
OSIClanRole OSIPS2::_getOSIClanRoleFromRendezVous(bool isLeader, bool isOfficer)
{
	if(isLeader) {
		return OSI_CLAN_ROLE_LEADER;
	}
	else if(isOfficer) {
		return OSI_CLAN_ROLE_OFFICER;
	}
	else {
		return OSI_CLAN_ROLE_SOLDIER;
	}
}

///----------------------------------------------------------------------
/// Populate the current OSIPlayer with the relevant clan data we have
/// gotten from RendezVous.
///----------------------------------------------------------------------
void OSIPS2::_setActiveAccountPlayerClanInfo()
{
	OSIClanInfo &clanInfo = _activeAccount->player.clanInfo;
	clanInfo.clanID.ps2 = _currentClan.m_ClanID;
	UTF8Decode(clanInfo.tag, _currentClan.m_ClanAbbr.CStr(), NUM_ARRAY_ELEMENTS(clanInfo.tag));
	UTF8Decode(clanInfo.name, _currentClan.m_ClanName.CStr(), NUM_ARRAY_ELEMENTS(clanInfo.name));
	UTF8Decode(clanInfo.leaderName, _currentClan.m_ClanLeader.CStr(), NUM_ARRAY_ELEMENTS(clanInfo.leaderName));
	clanInfo.memberCount = _currentClan.m_ClanMemberCount;
	clanInfo.stats.statsArray[ OSIClanStats::CLANSTAT_RANK ] = _currentClan.m_ClanRank;
	clanInfo.stats.statsArray[ OSIClanStats::CLANSTAT_RATING ] = _currentClan.m_ClanRating;
	clanInfo.stats.statsArray[ OSIClanStats::CLANSTAT_KILLS ] = _currentClan.m_ClanKills;
	// If player is not in a clan, use unknown role
	if(_currentClan.m_ClanAbbr == "") {
		_activeAccount->player.clanRole = OSI_CLAN_ROLE_UNKNOWN;
	}
	else {
		_activeAccount->player.clanRole = _getOSIClanRoleFromRendezVous(_currentClanMember.m_ClanIsLeader, _currentClanMember.m_ClanIsOfficer);
	}
}

//-------------------------------------------------------------------
//        E R R O R   H A N D L I N G   F U N C T I O N S
//-------------------------------------------------------------------
OSIchar* OSIPS2::GetOSIStringByEnum( OSIStringEnum string_id )
{
	OSIchar *ptr;

	if(string_id >= OSI_END_TEXT_LIST || string_id < 0)
	{
		ptr = undefinedString;
	}
	else
	{
		ptr = OSIStringTable[string_id];
		_errorCode = string_id ;
	}

	return ptr;
}


bool OSIPS2::isLastErrorFatal( void )
{
//	return ( ( _errorCode > OSIPS2_ERROR_REQUEST_ALREADY_IN_PROGRESS ) && (_errorCode  < OSI_END_TEXT_LIST )  );
	return ( ( _errorCode > OSI_BEGIN_FATAL_ERRORS ) && (_errorCode  < OSI_END_TEXT_LIST )  );
}





///---------------------------------------------------------
/// These functions handle the ping checking for a match
///---------------------------------------------------------
void RVMatch::beginPingCheck( void )
{
	pingChrono.Reset();
	pingChrono.Start();

	if ( !peerManager )
	{
		pingStatus = PING_KNOWN ;
		return ;
	}

	peerManager->MonitorURLsConnectivity(&lstStationURLs);
	pingStatus = PING_WORKING ;
}

bool RVMatch::update()
{
	ping = peerManager->GetConnectivityStatus(&lstStationURLs);
	if ( ping == Infinite && pingChrono.Check() < 5000 )
		return false ;
	pingChrono.Stop();
	pingStatus = PING_KNOWN ;
	return true ;
}
