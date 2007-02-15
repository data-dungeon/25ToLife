class OSIPlayerState;
class OSIPlayer;
class OSIMatchList ;

#ifndef _INCLUDED_OSI_PLAYER_
#define _INCLUDED_OSI_PLAYER_

#include "OSIPlatformTypes.h"
#include "OSIPlayerStats.hpp"
#include "OSIPlayerID.hpp"
#include "OSIClans.hpp"

///////////////////////////////////////////////////////////////////////////////////////
/// Stores information about a player that is published on the network.
///////////////////////////////////////////////////////////////////////////////////////
class OSIPlayerState
{
public:
	char			currentControllerIndex;		///< The controller index for the player.
	bool			isOnline;					///< True if player is online.  Can be false for local player if they are hiding.
	bool			isCurrentGameJoinable;		///< True if player is in a game that is joinable (valid in lobby only).
	bool			isPlayingGame;				///< True if player is playing a game right now (not including lobby).
	bool			isVoiceAvailable;			///< True if player has voice communication capability.

	OSIPlayerState()
	{
		currentControllerIndex	= 0;
		isOnline				= false;
		isCurrentGameJoinable	= false;
		isPlayingGame			= false;
		isVoiceAvailable		= false;
	}
};

///////////////////////////////////////////////////////////////////////////////////////
/// OSIPlayer stores key data for a single online player.
/// Note that this does not include the avatar of the player (which
/// is most likely dynamic and chosen at match time).
///////////////////////////////////////////////////////////////////////////////////////
class OSIPlayer
{
public:
	static void*					operator new( size_t size )								{ return OSImalloc( size ); }
	static void						operator delete( void* memPtr )							{ OSIfree( memPtr ); }

	OSIPlayer( void );
	OSIPlayer( const OSIPlayer& copyme );
	~OSIPlayer( void );
	const OSIPlayer& operator=( const OSIPlayer &a );

	const OSIchar*			GetFullName( OSIchar* nameBuffer );
	void					SetStatusString( const OSIchar* status );

	OSIPlayerID				id ;                                    ///< Unique ID
	OSIchar					name[ OSI_PLAYER_NAME_LENGTH ] ;		///< Player name (aka Login, account name, etc.)
	OSIPlayerState			state;	    							///< Current player state
	OSIClanInfo				clanInfo;								///< Current clan information
	OSIClanRole				clanRole;								///< Current role in this clan (Leader, etc...)
	OSIMatchList*			currentMatchList ;						///< Pointer to current match
	bool					lookingToPlay ;		    				///< Wants to play a game
	bool					isAFriend ;					    		///< Is on my friends list (and I'm on theirs)
	bool					hasRequestedMyFriendship ;		    	///< Has offered me a friend request
	bool					haveRequestedTheirFriendship ;		    ///< I have offered them a friend request
	int						statRating;								///< Online multiplayer stat rating
	int						statRanking;							///< Online multiplayer stat ranking

	OSIchar*				statusString;							///< Status string for online players
};

#endif //_INCLUDED_OSI_PLAYER_
