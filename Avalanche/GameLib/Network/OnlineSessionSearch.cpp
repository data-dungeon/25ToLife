/*
**
**  File:   OnlineSessionSearch.cpp
**  Date:   October 22, 2004
**  By:     Chris Stark
**  Desc:   Search for NetZ sessions.
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/OnlineSessionSearch.h"
#include "Network/SessionSpec.h"
#include "OSI/OSI.hpp"

const char* const COnlineSessionSearch::GameTypeIndexToStringLUT[ GameTypeIndexToStringLUTSize ] =
{
	"war",
	"raid",
	"robbery",
	"tag",
	"wipeout",
	"sp",		// singleplayer - this should never come up here
	"dev"		// dev mode - this should never come up here
};

/******************************************************************************/
/******************************************************************************/
COnlineSessionSearch::COnlineSessionSearch()
{
	_resultsList.clear();
	_newResults = false;
	_joiningState = IDLE;
	_sessionSpec = NULL;
}

/******************************************************************************/
/******************************************************************************/
void COnlineSessionSearch::Begin(void)
{
//	ASSERT(_joiningState == IDLE);
}

/******************************************************************************/
/******************************************************************************/
bool COnlineSessionSearch::Update(bool reFresh)
{
	if (_newResults && (_joiningState == IDLE))
	{
		_newResults = false;
		return true;
	}
	return false;
}

/******************************************************************************/
/******************************************************************************/
CSessionSearch::EEndStatus COnlineSessionSearch::End(void)
{
	if (_joiningState == JOINING)
		return SEARCH_IN_PROGRESS;

	_resultsList.clear();
	_TTLConfigStringList.clear();
	_newResults = false;
	EEndStatus status = _joiningState != JOIN_FAILURE ? SEARCH_CLEAN : SEARCH_ERROR;
	_joiningState = IDLE;
	_sessionSpec = NULL;
	return status;
}

/******************************************************************************/
/******************************************************************************/
int COnlineSessionSearch::GetNumFound(void)
{
	return _resultsList.count();
}

/******************************************************************************/
/******************************************************************************/
const char* COnlineSessionSearch::GetName(int i_index)
{
	ASSERT( i_index < (int) _resultsList.count() );
	static char nameWithID[32];
	if (i_index >= (int)_resultsList.count())
    {
	    return(NULL);
    }
    else
    {
		if ( _resultsList[i_index]->options.isPrivate )
			sprintf(nameWithID,"(Invited) %s",_resultsList[i_index]->options.description);
		else
			sprintf(nameWithID,"%s",_resultsList[i_index]->options.description);
        return nameWithID;
    }
}

/******************************************************************************/
/******************************************************************************/
const char* COnlineSessionSearch::GetSessionID(int i_index)
{
	static char sessionID[32];
	ASSERT( i_index < (int) _resultsList.count() );
	if (i_index >= (int)_resultsList.count())
    {
	    return(NULL);
    }
    else
    {
		sprintf(sessionID,"%d",_resultsList[i_index]->id.ps2);
        return sessionID;
    }
}

/******************************************************************************/
/******************************************************************************/
const char* COnlineSessionSearch::GetMapName(int i_index)
{
	ASSERT( i_index < (int) _resultsList.count() );
	if (i_index >= (int)_resultsList.count())
    {
	    return(NULL);
    }
    else
    {
		return _resultsList[i_index]->options.levelName;
    }	
}

/******************************************************************************/
/******************************************************************************/
int COnlineSessionSearch::GetMapIndex(int i_index)
{
	ASSERT( i_index < (int) _resultsList.count() );
	if( i_index >= (int)_resultsList.count() )
	{
		return -1;
	}
	else
	{
		return _resultsList[ i_index ]->options.levelIndex;
	}
}

/******************************************************************************/
/******************************************************************************/
const char* COnlineSessionSearch::GetMode(int i_index)
{
	ASSERT( i_index < (int) _resultsList.count() );

	static char modeStr[4];
	if (i_index >= (int)_resultsList.count())
    {
	    return(NULL);
    }
    else
    {
		sprintf(modeStr, "%s", GameTypeIndexToStringLUT[_resultsList[i_index]->options.gameTypeIndex]);
		return modeStr;
    }
}

/******************************************************************************/
/******************************************************************************/
const char* COnlineSessionSearch::GetPlayers(int i_index)
{
	ASSERT( i_index < (int) _resultsList.count() );
	static char pStr[4];
	if (i_index >= (int)_resultsList.count())
    {
	    return(NULL);
    }
    else
    {
		// Players is numberOfPlayers / maximumPlayers
		const OSIMatch*			match	= _resultsList[i_index];
		sprintf(pStr, "%d/%d", match->options.totalConnectedPlayers, match->maximumPlayers );
		return pStr;
    }
}

/******************************************************************************/
/******************************************************************************/
const char* COnlineSessionSearch::GetPing(int i_index)
{
	ASSERT( i_index < (int) _resultsList.count() );
	static char pingStr[8];
	if (i_index >= (int)_resultsList.count())
    {
	    return(NULL);
    }
    else
    {
		sprintf(pingStr,"%d",_resultsList[i_index]->ping);
		return pingStr;
    }
}

/*****************************************************************************

OK, this is going to be one hell of a heinous hack.  Here's the deal:

On the LAN:
  - HOST reads multiplayer session attributes from a nice XML file, e.g. time limit, map name, etc
  - HOST uses nice XML objects to generate a string that represents the current session's settings.
    It looks something like this: 
		"rs=0;md=war;gm=13;tl=300;sl=0;tgl=0;mp=4;gs=1;re=1;ff=0;ap=0;ad=0;sd=1;at=0;rk=0;"
  - HOST uses NetZ to distribute this string with the session.
  - CLIENT finds this data via NetZ and parses it to discover what kind of sessions are available.
  - Everything is happy.

The problem with this on the internet side is that you have to explicitly call out all of your 
session attributes so that the Live/Rendesvouz servers can sort and filter your search results
for you.  And neither of the online services know anything about the nice XML objects.

It's concievable that Rendesvouz could be taught how to deal with it, but it would still put
unnecessary load on the server by forcing it to parse a string for every session with a 
python script... it wouldn't be too bad but it certainly isn't ideal.  And then there are the
maintenance headaches involved during development with making the python script, making sure
the XML file on the servers are current, debugging any python problems, etc etc etc.  Plus, 
there's no way to do this on Live.

SO then what we have here is a failure to communicate.  TTL wants a string of key/value pairs,
but Live/Rendesvous, via OSI, are giving us an OSIMatchOptions structure.  

This function has to translate (i.e. fake) OSI's data into a string that TTL
can swallow.  Unfortunately, since the XML is flexible, data-driven, and easily editable 
(which is nice), but the OSI data is custom, straightforward, and designed to work well with 
Live/Rendesvouz (which is nice), we have no good way to automate the translation.  So it has 
to be called out explicitly and hard-coded.  Which is really too bad.

The problems, obviously, are:
- This code is going to be a nasty and fragile hack.  
- If the XML attributes change, this code has to be updated manually to match.
- The LAN and Online sessions may not always want to advertise the same data.
  I'm not exactly sure how this will complicate things, but it's definitely a risk.

*****************************************************************************

This version of the code is based on the following XML, dated 11/03/04

<?xml version="1.0" encoding="utf-8" ?>
<optionSet>
	<gameModes key="md" disp="Game Mode" type="mode" defIdx="0">
	Choose the game mode you'd like to play.
		<choice disp="War" modeType="mp" val="war">
			<script name="mode_war" />
		</choice>
		<choice disp="Raid" modeType="mp" val="raid">
			<script name="mode_raid" />
		</choice>
		<choice disp="Robbery" modeType="mp" val="robbery">
			<script name="mode_robbery" />
		</choice>
		<choice disp="Tag" modeType="mp" val="tag">
			<script name="mode_tag" />
		</choice>
		<choice disp="Single-Player" modeType="sp" hidden="1" val="sp">
			<script name="mode_sp" />
		</choice>
		<choice disp="DEV" modeType="dev" debug="1" hidden="1" val="dev">
			<script name="mode_dev" />
		</choice>
	</gameModes>
	<gameMap key="gm" disp="Map" type="map" depend="gameModes">
	Choose the map you'd like to play on.
	</gameMap>
	<timeLimit key="tl" disp="Time Limit" type="int" defIdx="2" scriptfunc="SetTimeLimit">
	Would you like to use a time limit in this game?  If so, how much time?
		<choice disp="No time limit" val="0" />
		<choice disp="3 minutes" val="180" />
		<choice disp="5 minutes" val="300" />
		<choice disp="7 minutes" val="420" />
		<choice disp="10 minutes" val="600" />
	</timeLimit>
	<scoreLimit key="sl" disp="Score Limit" type="int" defIdx="0" scriptfunc="SetScoreLimit">
	Would you like to use a score limit in this game?  If so, what is the limit?
		<choice disp="No score limit" val="0" />
		<choice disp="15 Points" val="15" />
		<choice disp="30 Points" val="30" />
		<choice disp="50 Points" val="50" />
		<choice disp="75 Points" val="75" />
		<choice disp="100 Points" val="100" />
		<choice disp="150 Points" val="150" />
	</scoreLimit>
	<tagLimit key="tgl" disp="Tag Limit" type="int" depend="gameModes" dependValue="tag" defIdx="0" scriptfunc="SetTagLimit">
	Would you like to use a Tag limit in this game?  If so, what is the limit?
		<choice disp="No Tag limit" val="0" />
		<choice disp="10 Tags" val="10" />
		<choice disp="15 Tags" val="15" />
		<choice disp="20 Tags" val="20" />
		<choice disp="25 Tags" val="25" />
	</tagLimit>
	<maxPlayers key="mp" disp="Max Players" type="int" defIdx="1">
	Select the maximum number of players allowed in this game.
		<choice disp="2 Players" val="2" />
		<choice disp="4 Players" val="4" />
		<choice disp="8 Players" val="8" />
		<choice disp="12 Players" val="12" />
		<choice disp="16 Players" val="16" />
	</maxPlayers>
	<reservedSlots key="rs" disp="Reserved Slots" type="int" min="0" max="=maxPlayers" step="1" defVal="0" depend="maxPlayers">
	How many of the player slots would you like to reserve for people you invite from your Friends list?
	</reservedSlots>
	<gameSeries key="gs" disp="Best-of Series" type="int" defIdx="0">
	How many games would you like to play without coming back to the ready-up lobby screen?
		<choice disp="1 Match" val="1" />
		<choice disp="3 Matches" val="3" />
		<choice disp="5 Matches" val="5" />
		<choice disp="7 Matches" val="7" />
		<choice disp="9 Matches" val="9" />
	</gameSeries>
	<allowRespawn key="re" disp="Players Respawn" type="bool" trueTag="On" falseTag="Off" defVal="1" scriptfunc="SetAllowRespawn">
	Should players respawn after they die?
	</allowRespawn>
	<friendlyFire key="ff" disp="Friendly Fire" type="bool" trueTag="On" falseTag="Off" defVal="0" scriptfunc="SetAllowFriendlyFire">
	Would you like to be able to injure your team mates?
	</friendlyFire>
	<apprehension key="ap" disp="Apprehension" type="bool" trueTag="On" falseTag="Off" defVal="0" scriptfunc="SetApprehension">
	Would you like to be able to apprehend your opponents?
	</apprehension>
	<arcadeDamage key="ad" disp="Arcade Damage" type="bool" trueTag="On" falseTag="Off" defVal="0" scriptfunc="SetArcadeDamage">
	Would you like the damage in the game to be more 'Arcade'-like (weapons do less damage)?
	</arcadeDamage>
	<selfDamage key="sd" disp="Self Damage" type="bool" trueTag="On" falseTag="Off" defVal="1" scriptfunc="SetSelfDamage">
	Would you like your grenades to hurt you, in addition to others?
	</selfDamage>
	<alternatingTeams key="at" disp="Alternating Teams" trueTag="On" falseTag="Off" type="bool" defVal="0" scriptfunc="SetAlternatingTeams">
	Would you like the teams to switch sides after each match?
	</alternatingTeams>
	<rankedMatch key="rk" disp="Ranked Match" type="bool" trueTag="On" falseTag="Off" defVal="0" scriptfunc="SetRankedMatch">
	Would you like to have the results of this match reported for stat-tracking?
	</rankedMatch>
	<!-- SCL : Are we going to implement this option?
		<option key="lo" disp="Load-out Limit" type="int" defIdx="0" scriptfunc="SetLoadOutLimit">
			<choice disp="None" val="0"/>
			<choice disp="Handguns Only" val="1"/>
			<choice disp="Shotgun only" val="2"/>
			<choice disp="No Assault" val="3"/>
			<choice disp="No Grenade" val="4"/>
		</option>
	-->
</optionSet>

*****************************************************************************
These are the attributes that are defined in the XML above.
I thought about setting up a lookup table to translate each match option into
a key name, but it started getting ugly with pointer-to-zero field offsets
and crap like that so decided that, since there are only a dozen or so fields,
that it's probably better to just do it the easy way.

md		game mode			OSIMatchOptions::gameType
gm 		map					OSIMatchOptions::levelIndex
tl 		time limit			OSIMatchOptions::timeLimit
sl 		score limit			OSIMatchOptions::scoreLimit
tgl		tag limit			OSIMatchOptions::tagLimit
mp		max players			OSIMatchOptions::maxPlayers
rs		reserved slots		OSIMatchOptions::reservedPlayers
gs		best-of series		OSIMatchOptions::bestOfSeries
re		players respawn		OSIMatchOptions::playersRespawn
ff		friendly fire		OSIMatchOptions::friendlyFire
ap		apprehension		OSIMatchOptions::apprehension
ad		arcade damage		OSIMatchOptions::arcadeDamage
sd		self damage			OSIMatchOptions::selfDamage
at		alternating teams	OSIMatchOptions::alternatingTeams
rk		ranked match		OSIMatchOptions::isLadder

*****************************************************************************/


void COnlineSessionSearch::_ConvertOSIMatchOptionsToTTLConfigString( 
	const OSIMatch* const match, 
	char* outputString, 
	const unsigned int outputStringMaxLength )
{
	ASSERT( match );
	ASSERT( outputString );

	const OSIMatchOptions& matchOptions = match->options;

	// make sure the game type index fits inside of the index-to-string lookup table, and 
	// that the table is correct.
	// I don't understand why everything is an index except the game mode...
	ASSERT( GameTypeIndexToStringLUTSize == sizeof( GameTypeIndexToStringLUT ) / sizeof( GameTypeIndexToStringLUT[0] ) ); // if this fails, update _gameTypeIndexToStringLUT and this function
	ASSERT( matchOptions.gameTypeIndex >= 0 );
	ASSERT( matchOptions.gameTypeIndex < sizeof( GameTypeIndexToStringLUT ) / sizeof( GameTypeIndexToStringLUT[0] ) );

	// I've broken this out as much as I can to try and make it readable
	const int printedCharsCount = _snprintf( outputString, outputStringMaxLength, 
		"md=%s;"	\
		"gm=%i;"	\
		"tl=%i;"	\
		"sl=%i;"	\
		"tgl=%i;"	\
		"mp=%i;"	\
		"rs=%i;"	\
		"gs=%i;"	\
		"re=%i;"	\
		"ff=%i;"	\
		"ap=%i;"	\
		"ad=%i;"	\
		"sd=%i;"	\
		"at=%i;"	\
		"rk=%i;",
		/*md */	GameTypeIndexToStringLUT[ matchOptions.gameTypeIndex ],	// NOTICE: this is a string
		/*gm */	static_cast< int >( matchOptions.levelIndex ),
		/*tl */	static_cast< int >( matchOptions.timeLimit ),
		/*sl */	static_cast< int >( matchOptions.scoreLimit ),
		/*tgl*/	static_cast< int >( matchOptions.tagLimit ),
		/*mp */	static_cast< int >( matchOptions.maxPlayers ),
		/*rs */	static_cast< int >( matchOptions.reservedPlayers ),
		/*gs */	static_cast< int >( matchOptions.bestOfSeries ),
		/*re */	static_cast< int >( matchOptions.playersRespawn ),
		/*ff */	static_cast< int >( matchOptions.friendlyFire ),
		/*ap */	static_cast< int >( matchOptions.apprehension ),
		/*ad */	static_cast< int >( matchOptions.arcadeDamage ),
		/*sd */	static_cast< int >( matchOptions.selfDamage ),
		/*at */	static_cast< int >( matchOptions.alternatingTeams ),
		/*rk */	static_cast< int >( matchOptions.isRanked ) 
	);

	ASSERT( printedCharsCount >= 0 ); // _snprintf returns <0 if it ran out of space in the buffer
}

/*****************************************************************************/
/*****************************************************************************/
void COnlineSessionSearch::SetResults( OSIMatchList& results )
{
	// flag new results
	_newResults = true;

	// copy the results list
	_resultsList = results;

	// clear the parallel list of TTL config strings so it can be repopulated to match the new results list
	_TTLConfigStringList.clear();

	// go through all the results and generate a TTL config string for each one.
	char tempString[ OSIStringList::MAX_STRING_LENGTH ];
	for( results.begin(); NULL != results.getCurrent(); results.next() )
	{
		const OSIMatch* const iteratedMatch = results.getCurrent();
		
		_ConvertOSIMatchOptionsToTTLConfigString( 
			iteratedMatch, 
			tempString, 
			OSIStringList::MAX_STRING_LENGTH );

		_TTLConfigStringList.push_back( tempString );
	}
}

/*****************************************************************************/
/*****************************************************************************/
const char* COnlineSessionSearch::GetConfig(int i_index)
{
	ASSERT( i_index >= 0 );
	ASSERT( i_index < (int) _TTLConfigStringList.count() );
	if( i_index >= (int) _TTLConfigStringList.count() )
	{
		return NULL;
	}
	else
	{
		return _TTLConfigStringList[ i_index ];
	}
}



/******************************************************************************/
/******************************************************************************/
bool COnlineSessionSearch::SpecifyFoundSession(int i_index, CSessionSpec &i_sessionSpec)
{
	if (_joiningState != JOINING)
	{
		_joiningState = JOINING;
		_sessionSpec = &i_sessionSpec;
		g_messageDispatcher.SendMessageToAll( "SessionSelected", (void *)i_index, INVALID_OBJECT_HANDLE );
		return( true );
	}
	return( false );
}

/******************************************************************************/
/******************************************************************************/
void COnlineSessionSearch::MatchJoinResult(bool success)
{
	//ASSERT((_joiningState == JOINING) && (_sessionSpec != NULL));

	_joiningState = JOIN_FAILURE;
	if ( success )
		_joiningState = JOIN_SUCCESS ;
/*
	if (success)
	{
		_sessionSpec->Clear();
	
		OSIMatch* match = OSI::getSingletonPtr()->getCurrentMatch();
		if (match != NULL)
		{
			bool			addResult	= true ;
			OSIStringList&	stringList	= match->hostURLs ;
			for ( stringList.begin(); stringList.getCurrent(); stringList.next() )
			{
				OSIchar*	sessionURL	= stringList.getCurrent();
				addResult				= _sessionSpec->AddEntry( sessionURL );
			}
			if (addResult)
				_joiningState = JOIN_SUCCESS;
		}
	}
*/
}










