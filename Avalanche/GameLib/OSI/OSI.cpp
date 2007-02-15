//---------------------------------------------------------------------------
// OSI.cpp
// 
// Main header file for Online Services Interface base class.  Most of
//	this class's methods are pure virtual and therefore will not appear
//	in this implementation file; only those which are either platform
//	independent or have meaningful default behavior are implemented here.
//---------------------------------------------------------------------------
#include "OSI.hpp"
#include "Layers/Memory.h"

#ifdef _XBOX
	#include "xbox/pch.hpp"
	#include "XBox/OSIXBox.hpp"
#elif defined( PS2 ) || defined( DIRECTX_PC )
	#include "PS2/OSIPS2.hpp"
#endif

#include <stdio.h>
#include <Assert.h>

//#include "../utility/util_memorymanager.h"

// statics
OSI*	OSI::_theOSI		= NULL ;
int		OSI::_memoryBlock	= 0 ;

#define USE_NETZ_HEAP

int osiHandles = 0;

///------------------------------------------------------------------------
/// OSI Malloc
///------------------------------------------------------------------------
void* OSImalloc( size_t size )
{
	MEM_SET_RETURN_ADDRESS(); 

	osiHandles++;

	#ifdef USE_NETZ_HEAP
		// use netz private heap so get less fragmentation...
		#ifdef MEM_DEBUG_BLOCKS
			extern u32 memRetrieveReturnAddress();	// do this because we may bypass our memory
			memRetrieveReturnAddress();		// manager and we don't want to record a fake RA.
		#endif
		extern void* OurMalloc(size_t stSize);
		return OurMalloc( size );
	#else
		MEM_SET_ONE_SHOT_NEW_NAME("OSI");
		void *p = (void*) ::new char[ size ];
		assert( p != NULL );
		return p;
	#endif
}


///------------------------------------------------------------------------
/// OSI free function
///------------------------------------------------------------------------
void OSIfree( void* ptr )
{
	osiHandles--;

	#ifdef USE_NETZ_HEAP
		extern void OurFree( void *pBlock );
		OurFree( ptr );
	#else
		::delete [] (char*)ptr;
	#endif
}


///-------------------------------------------------------------------------
/// Constructor
///-------------------------------------------------------------------------
OSI::OSI( void )
	:	_isConnected( false ),
		_defaultTimeout(OSI_DEFAULT_TIMEOUT),
	_defaultTimeSlice(5),
	_activeState(OSI_STATE_IDLE),
	_errorCode(OSI_STATE_IDLE)
{
	_messageOfTheDay[0] = '\0';
}


///-------------------------------------------------------------------------
/// Destructor
///-------------------------------------------------------------------------
OSI::~OSI( void )
{
	//
}


///-------------------------------------------------------------------------
/// Returns the singleton ptr.
///-------------------------------------------------------------------------
void OSI::createOSI( int memoryBlock )
{
	_memoryBlock = memoryBlock ;

#if defined(_XBOX)
	OSIXbox* theOSI = new OSIXbox();
	_theOSI = theOSI;
#elif defined(PS2) || defined(DIRECTX_PC)
	OSIPS2* theOSI = new OSIPS2();
	_theOSI = theOSI;
#else
	_theOSI = NULL;
#endif
}

//---------------------------------------------------------------------------
// Destroys the OSI pointer
//---------------------------------------------------------------------------
void OSI::destroyOSI( void )
{
	if ( !_theOSI )
		return ;

	delete _theOSI;
	_theOSI = NULL;
}


//---------------------------------------------------------------------------
// getLastRequestStatus
//---------------------------------------------------------------------------
OSIRequestStatus OSI::getLastRequestStatus( void )
{
	return _requestStatus ;
}


#ifndef DIRECTX_PC
//---------------------------------------------------------------------------
// getHardwareProfileList
//---------------------------------------------------------------------------
OSIHardwareProfileList& OSI::getHardwareProfileList()
{
	_populateHardwareProfileList();
	return _hardwareProfileList;
}
#endif


//---------------------------------------------------------------------------
// getHardwareProfileList
//---------------------------------------------------------------------------
OSIAccount& OSI::getAccountInfo()
{
	return _currentAccount;
}

//---------------------------------------------------------------------------
// getFriendList
//---------------------------------------------------------------------------
OSIPlayerList& OSI::getFriendList()
{
	if( _friendList.isNetworkDataReady() )
	{
		_populateFriendList();
	}

	return _friendList;
}


//---------------------------------------------------------------------------
// getRecentPlayerList
//---------------------------------------------------------------------------
OSIPlayerList& OSI::getRecentPlayerList()
{
	return _recentPlayerList;
}


//---------------------------------------------------------------------------
// getCurrentPlayerList
//---------------------------------------------------------------------------
OSIPlayerList& OSI::getCurrentPlayerList()
{
	return _currentPlayerList;
}

//---------------------------------------------------------------------------
// Determine if a player is in current players list
//---------------------------------------------------------------------------
bool OSI::isPlayerInMatch( OSIPlayer* player )
{
	if( player == NULL || player->name[ 0 ] == 0 )
		return false;
	
	// Check for active player
	if( player == getActivePlayer() ||
		OSIstrncmp( player->name, getActivePlayer()->name, OSI_PLAYER_NAME_LENGTH ) == 0 )
	{
		return true;
	}

	OSIPlayer* tempPlayer;
	OSIPlayerList tempList;
	tempList = getCurrentPlayerList();
	tempList.begin();
	while ( (tempPlayer = tempList.getCurrent()) != NULL )
	{
		if( OSIstrncmp( tempPlayer->name, player->name, OSI_PLAYER_NAME_LENGTH ) == 0 )
			return true;
		tempList.next();
	}

	return false;
}

//---------------------------------------------------------------------------
// Determine if a player is in current players list
//---------------------------------------------------------------------------
bool OSI::isPlayerInMatch( const OSIchar* const playerName )
{
	if( playerName == NULL || playerName[ 0 ] == 0 )
		return false;

	OSIPlayer player;
	OSIstrncpy( player.name, playerName, OSI_PLAYER_NAME_LENGTH );
	return isPlayerInMatch( &player );
}

//---------------------------------------------------------------------------
// getStatsPlayerList
//---------------------------------------------------------------------------
OSIPlayerStatsList& OSI::getStatsPlayerList() 
{ 
	return _statsListPlayers; 
}

//---------------------------------------------------------------------------
// getStatsClanList
//---------------------------------------------------------------------------
OSIClanInfoList& OSI::getStatsClanList() 
{ 
	return _statsListClans; 
}

//---------------------------------------------------------------------------
// getCurrentMatch
//---------------------------------------------------------------------------
OSIMatch* OSI::getCurrentMatch()
{
	// if match id is zero, there is no match
	if( _currentMatch.id.quadword == 0 )
		return NULL;
	else
		return &_currentMatch;
}


//---------------------------------------------------------------------------
// getMatchList
//---------------------------------------------------------------------------
OSIMatchList& OSI::getMatchList()
{
	if( _matchList.isNetworkDataReady() )
	{
		_populateMatchList();
	}
	return _matchList;
}

//---------------------------------------------------------------------------
// getNextPendingTextMessage
//---------------------------------------------------------------------------
const OSIchar* OSI::getNextPendingTextMessage()
{
	return OSI_STRING_LITERAL("");
}


///--------------------------------------------------------------------------
/// Fetches the station url list (list of URLs to a given match).  If new
/// network data has been fetched, calls _populateStationURLList to convert
/// platform specific data to platform-independent data
///--------------------------------------------------------------------------
OSIStringList& OSI::getStationURLList()
{
	if ( _stationURLList.isNetworkDataReady() )
	{
		_populateStationURLList();
	}
	return _stationURLList ;
}





//---------------------------------------------------------------------------
// getClanInvitationList
//---------------------------------------------------------------------------
OSIClanInvitationList& OSI::getClanInvitationList()
{
	if( _clanInvitationList.isNetworkDataReady() )
	{
		_populateClanInvitationList();
	}
	return _clanInvitationList;
}

//---------------------------------------------------------------------------
// getClanSentInvitationList
//---------------------------------------------------------------------------
OSIClanInvitationList& OSI::getClanSentInvitationList()
{
	if( _clanSentInvitationList.isNetworkDataReady() )
	{
		_populateClanSentInvitationList();
	}
	return _clanSentInvitationList;
}


//---------------------------------------------------------------------------
// getClanIDList
//---------------------------------------------------------------------------
OSIClanIDList& OSI::getClanIDList()
{
	if( _clanIDList.isNetworkDataReady() )
	{
		_populateClanIDList();
	}
	return _clanIDList;
}


//---------------------------------------------------------------------------
// getClanInfoList
//---------------------------------------------------------------------------
OSIClanInfoList& OSI::getClanInfoList()
{
	if( _clanInfoList.isNetworkDataReady() )
	{
		_populateClanInfoList();
	}
	return _clanInfoList;
}


//---------------------------------------------------------------------------
// getClanMembershipList
//---------------------------------------------------------------------------
OSIClanMembershipList& OSI::getClanMembershipList()
{
	if( _clanMembershipList.isNetworkDataReady() )
	{
		_populateClanMembershipList();
	}
	return _clanMembershipList;
}


//---------------------------------------------------------------------------
// getClanNoticeList
//---------------------------------------------------------------------------
OSIClanNoticeList& OSI::getClanNoticeList()
{
	if( _clanNoticeList.isNetworkDataReady() )
	{
		_populateClanNoticeList();
	}
	return _clanNoticeList;
}

//---------------------------------------------------------------------------
// Add a player to the recent players list, popping off least recent player
// if the max count has been reached.
//---------------------------------------------------------------------------
void OSI::_addRecentPlayer( OSIPlayer& player )
{
	if( _recentPlayerList.count() == OSI_MAX_RECENT_PLAYERS )
	{
		_recentPlayerList.begin();
		_recentPlayerList.erase();
	}

	if( !_recentPlayerList.findPlayerByID( player.id ) )
	{
		_recentPlayerList.push_back( player );
	}
}

//---------------------------------------------------------------------------
// Remove a player from the recent player list
//---------------------------------------------------------------------------
bool OSI::_removeRecentPlayer( OSIPlayer& player )
{
	for ( _recentPlayerList.begin(); _recentPlayerList.getCurrent() != NULL; _recentPlayerList.next() )
	{
		const OSIPlayer* playerIndex = _recentPlayerList.getCurrent();
		if( playerIndex->id == player.id )
		{
			/// Remove this player from the list
			_recentPlayerList.erase(); // this removes the current list element only
			return true;
		}
	}
	return false;
}

//---------------------------------------------------------------------------
// Add a player to the current players list as long as they don't exist
//---------------------------------------------------------------------------
void OSI::_addCurrentPlayer( OSIPlayer& player )
{
	// See if this player is already on the current player list
	for ( _currentPlayerList.begin(); _currentPlayerList.getCurrent() != NULL; _currentPlayerList.next() )
	{
		const OSIPlayer* playerIndex = _currentPlayerList.getCurrent();
		if( playerIndex->id == player.id )
		{
			return; // nothing to do; player is already on the list!
		}
	}

	if( !_currentPlayerList.findPlayerByID( player.id ) )
	{
		_currentPlayerList.push_back( player );
	}
}

//---------------------------------------------------------------------------
// Remove a player from the current players list
//---------------------------------------------------------------------------
bool OSI::_removeCurrentPlayer( OSIPlayer& player )
{
	// Find this player in the current player list
	for ( _currentPlayerList.begin(); _currentPlayerList.getCurrent() != NULL; _currentPlayerList.next() )
	{
		const OSIPlayer* playerIndex = _currentPlayerList.getCurrent();
		if( playerIndex->id == player.id )
		{
			// Remove this player from the list
			_currentPlayerList.erase(); // this removes the current list element only
			return true;
		}
	}
	return false;
}











