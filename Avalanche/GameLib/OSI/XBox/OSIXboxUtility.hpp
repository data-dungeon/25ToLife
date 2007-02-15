#pragma once

#include <NetZ.h>
#include <Platform/Stack/Core/XBoxNetworking.h>
#include "../../TTL/code/main/GameEnv.h"

inline void XUID_to_OSIPlayerID( const XUID& source, OSIPlayerID& dest )
{
	dest.xBox.userId = source.qwUserID;
	dest.xBox.userFlags = source.dwUserFlags;
	ASSERT( source.qwUserID );
	ASSERT( !XOnlineXUIDIsTeam( &source ) );
}

inline void OSIPlayerID_to_XUID( const OSIPlayerID& source, XUID& dest )
{
	dest.qwUserID = source.xBox.userId;
	dest.dwUserFlags = source.xBox.userFlags;
	ASSERT( dest.qwUserID );
	ASSERT( !XOnlineXUIDIsTeam( &dest ) );
}

inline void XUID_to_OSIClanID( const XUID& source, OSIClanID& dest )
{
	dest.xBox.clanId = source.qwTeamID;
	dest.xBox.userFlags = source.dwUserFlags;
	ASSERT( source.qwTeamID );
	ASSERT( XOnlineXUIDIsTeam( &source ) );
}

inline void OSIClanID_to_XUID( const OSIClanID& source, XUID& dest )
{
	dest.qwTeamID = source.xBox.clanId;
	dest.dwUserFlags = source.xBox.userFlags;
	ASSERT( dest.qwTeamID );
	ASSERT( XOnlineXUIDIsTeam( &dest ) );
}


inline void XNKID_to_OSIMatchID( const XNKID& source, OSIMatchID& dest ) { memcpy( &dest.xBox, source.ab, sizeof( dest.xBox ) ); }
inline void OSIMatchID_to_XNKID( const OSIMatchID& source, XNKID& dest ) { memcpy( &dest.ab, source.xBox, sizeof( dest.ab ) ); }

//inline bool operator == ( const OSIAccountID& left, const XUID& right ) { return (left.xBox.userId == right.qwUserID) && (left.xBox.userFlags == right.dwUserFlags); }
//inline bool operator == ( const XUID& left, const OSIAccountID& right ) { return (left.qwUserID == right.xBox.userId) && (left.dwUserFlags == right.xBox.userFlags); }

inline bool operator == ( const XUID& left, const XUID& right ) { return XOnlineAreUsersIdentical( &left, &right ); }
inline bool operator != ( const XUID& left, const XUID& right ) { return !XOnlineAreUsersIdentical( &left, &right ); }

//inline bool operator == ( const XNKID& left, const XNKID& right ) { return !memcmp( &left, &right, 8 ); }
//inline bool operator != ( const XNKID& left, const XNKID& right ) {	return !(left == right); }

//inline bool operator == ( const unsigned char left[8], const XNKID& right ) { return !memcmp( &left, &right, 8 ); }
//inline bool operator != ( const unsigned char left[8], const XNKID& right ) { return !(left == right); }

//inline bool operator == ( const XNKID& left, const unsigned char right[8] ) { return !memcmp( &left, &right, 8 ); }
//inline bool operator != ( const XNKID& left, const unsigned char right[8] ) { return !(left == right); }


inline void OSIClearMatch( OSIMatch& match )
{
	match.hostURLs.clear();
	match.id.quadword = 0;
	match.maximumPlayers = 0;
	match.numberOfPlayers = 0;
	match.ping = 0;
	match.options.clear();
}

inline bool OSIUpdatePlayerStateForMatch( OSIPlayerState& state, OSIMatch& match )
{
	const OSIPlayerState oldState = state;

	if( state.isOnline && match.id.quadword )
	{
		state.isCurrentGameJoinable = (match.numberOfPlayers < match.maximumPlayers);
		state.isPlayingGame = true;
	}
	else
	{
		state.isCurrentGameJoinable = false;
		state.isPlayingGame = false;
	}

	return (oldState.isCurrentGameJoinable != state.isCurrentGameJoinable) 
		|| (oldState.isPlayingGame != state.isPlayingGame);
}

// compile-time assertion
#define COMPILE_TIME_ASSERT(expn) typedef char __C_ASSERT__[(expn)?1:-1]

inline ULONGLONG MatchOptionOrWildcardValue( const unsigned int wildcards, const OSIMatchOptions::MatchOptionsWildcardFlag testFlag, const unsigned int optionValue )
{
	if( (wildcards & testFlag) == testFlag )
	{
		return X_MATCH_NULL_INTEGER;
	}
	else
	{
		return optionValue;
	}
}


inline void 
CSpecificMatchSearchResult_to_OSIMatch( 
	const CSpecificMatchResult& searchResult,
	OSIMatch& match )
{
	//	OSIMatchID				id ;								//	Unique ID of this match	(gathering)
	XNKID_to_OSIMatchID( searchResult.SessionID, match.id );

	//	OSIchar					name[ OSI_STRING_LENGTH_LONG ] ;	//	Descriptive name
	OSIstrncpy( match.options.description, searchResult.HostTitle, NUM_ARRAY_ELEMENTS( match.options.description ) );

	//	OSIStringList			hostURLs ;							//	URLs to the hosting machine
	StationURL url;
	url.SetXBoxURL( &searchResult.HostAddress, &searchResult.SessionID, &searchResult.KeyExchangeKey );
	match.hostURLs.push_back( url.CStr() );

	//	int						numberOfPlayers	;					//	Currently joined players
	match.numberOfPlayers = (int)searchResult.TotalConnectedPlayers;

	//	int						maximumPlayers ;					//	Maximum	number of players
	match.maximumPlayers = (int)searchResult.MaxPlayers;

	//	int						ping ;								//	Latency	in millisecs
	match.ping = 0; // no ping for specific match query!

	//	OSIMatchOptions			options	;							//	Describes options for this game
	//		OSIchar					hostURL[ OSI_STRING_LENGTH_LONG ] ;			// temp storage hack by Ken
	match.options.hostURL[0] = 0;

	//		OSIchar					levelName[ OSI_STRING_LENGTH_SHORT ] ;		// level name (default is "any")
	OSIstrncpy( match.options.levelName, CGameEnv::GetWorldDisplayName( match.options.levelIndex ), NUM_ARRAY_ELEMENTS( match.options.levelName ) );

	//		int						gameTypeIndex;
	match.options.gameTypeIndex = (int)searchResult.GameType;

	//		int						levelIndex;
	match.options.levelIndex = (int)searchResult.LevelIndex;

	//		int						timeLimit;
	match.options.timeLimit = (int)searchResult.TimeLimit;

	//		int						scoreLimit;
	match.options.scoreLimit = (int)searchResult.ScoreLimit;

	//		int						tagLimit;
	match.options.tagLimit = (int)searchResult.TagLimit;

	//		int 					bestOfSeries;
	match.options.bestOfSeries = (int)searchResult.BestOfSeries;

	//		int						maxPlayers;
	match.options.maxPlayers = (int)searchResult.MaxPlayers;

	//		int						reservedPlayers;
	match.options.reservedPlayers = (int)searchResult.PrivateOpen + searchResult.PrivateFilled;

	//		int						totalConnectedPlayers;
	match.options.totalConnectedPlayers = (int)searchResult.TotalConnectedPlayers;

	//		bool					isCustomQuery ;								// is this a custom	(non-default) query?
	match.options.isCustomQuery = false; // only used on PS2

	//		bool					isRanked;
	match.options.isRanked = ( 0 != searchResult.IsRanked );

	//		bool					isPrivate;
	match.options.isPrivate = (searchResult.PrivateOpen > 0);
                                                             	
	//		bool					playersRespawn;
	match.options.playersRespawn = ( 0 != searchResult.PlayersRespawn );

	//		bool					friendlyFire;
	match.options.friendlyFire = ( 0 != searchResult.FriendlyFire );

	//		bool					apprehension;
	match.options.apprehension = ( 0 != searchResult.Apprehension );

	//		bool					arcadeDamage;
	match.options.arcadeDamage = false; // fixme: remove

	//		bool					selfDamage;
	match.options.selfDamage = ( 0 != searchResult.SelfDamage );

	//		bool					alternatingTeams;
	match.options.alternatingTeams = ( 0 != searchResult.AlternatingTeams );
}

inline void 
CCommonMatchSearchResult_to_OSIMatch( 
	const CCommonMatchResult& searchResult,
	OSIMatch& match )
{
	//	OSIMatchID				id ;								//	Unique ID of this match	(gathering)
	XNKID_to_OSIMatchID( searchResult.SessionID, match.id );

	//	OSIchar					name[ OSI_STRING_LENGTH_LONG ] ;	//	Descriptive name
	OSIstrncpy( match.options.description, searchResult.HostTitle, NUM_ARRAY_ELEMENTS( match.options.description ) );

	//	OSIStringList			hostURLs ;							//	URLs to the hosting machine
	StationURL url;
	url.SetXBoxURL( &searchResult.HostAddress, &searchResult.SessionID, &searchResult.KeyExchangeKey );
	match.hostURLs.push_back( url.CStr() );

	//	int						numberOfPlayers	;					//	Currently joined players
	match.numberOfPlayers = (int)searchResult.TotalConnectedPlayers;

	//	int						maximumPlayers ;					//	Maximum	number of players
	match.maximumPlayers = (int)searchResult.MaxPlayers;

	//	int						ping ;								//	Latency	in millisecs
	if( searchResult.pQosInfo )
	{
		match.ping = (int)searchResult.pQosInfo->wRttMedInMsecs;
	}

	//	OSIMatchOptions			options	;							//	Describes options for this game
	//		OSIchar					hostURL[ OSI_STRING_LENGTH_LONG ] ;			// temp storage hack by Ken
	match.options.hostURL[0] = 0;

	//		int						levelIndex;
	match.options.levelIndex = (int)searchResult.LevelIndex;

	//		OSIchar					levelName[ OSI_STRING_LENGTH_SHORT ] ;		// level name (default is "any")
	OSIstrncpy( match.options.levelName, CGameEnv::GetWorldDisplayName( match.options.levelIndex ), NUM_ARRAY_ELEMENTS( match.options.levelName ) );

	//		int						gameTypeIndex;
	match.options.gameTypeIndex = (int)searchResult.GameType;

	//		int						timeLimit;
	match.options.timeLimit = (int)searchResult.TimeLimit;

	//		int						scoreLimit;
	match.options.scoreLimit = (int)searchResult.ScoreLimit;

	//		int						tagLimit;
	match.options.tagLimit = (int)searchResult.TagLimit;

	//		int 					bestOfSeries;
	match.options.bestOfSeries = (int)searchResult.BestOfSeries;

	//		int						maxPlayers;
	match.options.maxPlayers = (int)searchResult.MaxPlayers;

	//		int						reservedPlayers;
	match.options.reservedPlayers = (int)searchResult.PrivateOpen + searchResult.PrivateFilled;

	//		int						totalConnectedPlayers;
	match.options.totalConnectedPlayers = (int)searchResult.TotalConnectedPlayers;

	//		bool					isCustomQuery ;								// is this a custom	(non-default) query?
	match.options.isCustomQuery = false; // only used on PS2

	//		bool					isRanked;
	match.options.isRanked = ( 0 != searchResult.IsRanked );

	//		bool					isPrivate;
	match.options.isPrivate = (searchResult.PrivateOpen > 0);
                                                             	
	//		bool					playersRespawn;
	match.options.playersRespawn = ( 0 != searchResult.PlayersRespawn );

	//		bool					friendlyFire;
	match.options.friendlyFire = ( 0 != searchResult.FriendlyFire );

	//		bool					apprehension;
	match.options.apprehension = ( 0 != searchResult.Apprehension );

	//		bool					arcadeDamage;
	match.options.arcadeDamage = false; // fixme: remove

	//		bool					selfDamage;
	match.options.selfDamage = ( 0 != searchResult.SelfDamage );

	//		bool					alternatingTeams;
	match.options.alternatingTeams = ( 0 != searchResult.AlternatingTeams );
}
