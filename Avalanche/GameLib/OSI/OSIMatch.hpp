#ifndef	_INCLUDED_OSI_MATCH_
#define	_INCLUDED_OSI_MATCH_

class MatchOptions;

#include "OSIPlatformTypes.h"
#include "OSIPlayer.hpp"
#include "OSIStringList.hpp"

union OSIMatchID
{
	OSIu32			ps2;
	unsigned char	xBox[8];	// XNKID
	OSIu64			quadword;	// for convenience in testing/comparing	values

	bool			isValid( void )	const	{ return quadword != (OSIu64)0 ; }
	void			setInvalid( void )		{ quadword = (OSIu64)0 ; }
};

//-------------------------------------------------------------------------------------------------
// XBox-Specific
//
#if defined(_XBOX)
//-------------------------------------------------------------------------------------------------
typedef OSIu64 OSIRoundID;
//-------------------------------------------------------------------------------------------------
// PS2-Specific
//
#else 
//-------------------------------------------------------------------------------------------------
typedef OSIu32 OSIRoundID;
#endif 


///////////////////////////////////////////////////////////////////////////////////////
///	The	options	describing a match.	 This is used in queries to
///	find matches that meet certain criteria.
///////////////////////////////////////////////////////////////////////////////////////
class OSIMatchOptions
{
public:
	static void*			operator new( size_t size )								{ return OSImalloc( size ); }
	static void				operator delete( void* memPtr )							{ OSIfree( memPtr ); }

	OSIchar					description[ OSI_STRING_LENGTH_LONG ] ;		//	Descriptive name
	OSIchar					hostURL[ OSI_STRING_LENGTH_LONG ] ;			// this is the URL of the host that is advertised on Rendesvouz/PS2 - unused on Xbox
	OSIchar					levelName[ OSI_LEVEL_NAME_LENGTH ] ;		// level name (default is "any")
	int						gameTypeIndex;
	int						levelIndex;
	int						timeLimit;

	int						scoreLimit;
	int						tagLimit;
	int 					bestOfSeries;
	int						maxPlayers;
	int						reservedPlayers;
	int						totalConnectedPlayers;

	bool					isCustomQuery ;								// is this a custom	(non-default) query?
	bool					isRanked;
	bool					isPrivate;
	bool					playersRespawn;
	bool					friendlyFire;
	bool					apprehension;
	bool					arcadeDamage;
	bool					selfDamage;
	bool					alternatingTeams;
	
	inline void clear()
	{
		description[0]			= '\0';
		hostURL[0]				= '\0';
		levelName[0]			= '\0';
		gameTypeIndex			= 0;
		levelIndex				= 0;
		timeLimit				= 0;
		scoreLimit				= 0;
		tagLimit				= 0;
		bestOfSeries			= 0;
		maxPlayers				= 0;
		reservedPlayers			= 0;
		totalConnectedPlayers	= 0;
		isPrivate				= false;
		isCustomQuery			= false;
		isRanked				= false;
		playersRespawn			= false;
		friendlyFire			= false;
		apprehension			= false;
		arcadeDamage			= false;
		selfDamage				= false;
		alternatingTeams		= false;
		_wildcardFlags			= 0; // default to 0 so we can more easily catch errors in initializing during search/host
	}

	inline OSIMatchOptions()
	{
		clear();
	}

	enum MatchOptionsWildcardFlag
	{
		MATCHOPTIONFLAG_LEVELNAME				= ( 1 << 0 ),
		MATCHOPTIONFLAG_LEVELINDEX				= ( 1 << 1 ),
		MATCHOPTIONFLAG_GAMETYPE				= ( 1 << 2 ),
		MATCHOPTIONFLAG_TIMELIMIT				= ( 1 << 3 ),
		MATCHOPTIONFLAG_SCORELIMIT				= ( 1 << 4 ),
		MATCHOPTIONFLAG_TAGLIMIT				= ( 1 << 5 ),
		MATCHOPTIONFLAG_BESTOFSERIES			= ( 1 << 6 ),
		MATCHOPTIONFLAG_MAXPLAYERS				= ( 1 << 7 ),
		MATCHOPTIONFLAG_RESERVEDPLAYERS			= ( 1 << 8 ),
		MATCHOPTIONFLAG_ISCUSTOMQUERY			= ( 1 << 9 ),
		MATCHOPTIONFLAG_ISRANKED				= ( 1 << 10 ),
		MATCHOPTIONFLAG_ISPRIVATE				= ( 1 << 11 ),
		MATCHOPTIONFLAG_PLAYERSRESPAWN			= ( 1 << 12 ),
		MATCHOPTIONFLAG_FRIENDLYFIRE			= ( 1 << 13 ),
		MATCHOPTIONFLAG_APPREHENSION			= ( 1 << 14 ),
		MATCHOPTIONFLAG_ARCADEDAMAGE			= ( 1 << 15 ),
		MATCHOPTIONFLAG_SELFDAMAGE				= ( 1 << 16 ),
		MATCHOPTIONFLAG_ALTERNATINGTEAMS		= ( 1 << 17 ),
		MATCHOPTIONFLAG_TOTALCONNECTEDPLAYERS	= ( 1 << 18 ),
		// update MATCHOPTIONFLAG_QUICKMATCH_MASK to be ((1 << (last bit + 1)) - 1) if you add something here

		MATCHOPTIONFLAG_QUICKMATCH_MASK			= ( 1 << 19 ) - 1, // all bits below this one

		MATCHOPTIONFLAG_FORCE32BIT				= 0xffffffff,
	};

	inline unsigned int getWildcardFlags() const { return _wildcardFlags; }
	inline void setWildcardFlags( const unsigned int newFlags ) { _wildcardFlags = newFlags; }
	inline void addWildcardFlag( const unsigned int flagToSet ) { _wildcardFlags |= flagToSet; }
	inline void removeWildcardFlag( const unsigned int flagToRemove ) { _wildcardFlags &= (~flagToRemove); }
	inline bool isWildcardFlagSet( const unsigned int testFlag ) { return ((_wildcardFlags & testFlag) != 0); }

protected:
	unsigned int _wildcardFlags;

};


///////////////////////////////////////////////////////////////////////////////////////
///	Data for a single match.
///////////////////////////////////////////////////////////////////////////////////////
class OSIMatch
{
public:
	static void*					operator new( size_t size )								{ return OSImalloc( size ); }
	static void						operator delete( void* memPtr )							{ OSIfree( memPtr ); }

	OSIMatch()
	{
		hostURLs.clear();
		id.quadword				= 0;
		numberOfPlayers			= 0;
		maximumPlayers			= 0;
		ping					= 0;
	}

	OSIMatchID				id ;								//	Unique ID of this match	(gathering)
	OSIStringList			hostURLs ;							//	URLs to the hosting machine
	int						numberOfPlayers	;					//	Currently joined players
	int						maximumPlayers ;					//	Maximum	number of players
	int						ping ;								//	Latency	in millisecs
	OSIMatchOptions			options	;							//	Describes options for this game
};


#endif // _INCLUDED_OSI_MATCH_

