#include "Layers/Assert/Assert.h"
#include <stdio.h>

#include "OSIClans.hpp"

#include "OSIMatchList.hpp"

///////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Set correct initial values
///////////////////////////////////////////////////////////////////////////////////////
OSIClanInfo::OSIClanInfo()
{
	tag[0] = '\0'; 
	name[0] = '\0'; 
	memberCount = 0; 
	clanID.xBox.clanId = 0; 
	clanID.xBox.userFlags = 0;
	leaderName[0] = '\0';
}

///////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Set correct initial values
///////////////////////////////////////////////////////////////////////////////////////
OSIClanMembership::OSIClanMembership()
{
	playerID.xBox.userFlags = 0;
	playerID.xBox.userId = 0;
	clanID.xBox.userFlags = 0;
	clanID.xBox.clanId = 0;
	memberName[0] = '\0';
	role = OSI_CLAN_ROLE_UNKNOWN;
	currentMatchList = new OSIMatchList;
	isOnline = false;
	isFriend = false;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Copy Constructor: Set correct initial values
///////////////////////////////////////////////////////////////////////////////////////
OSIClanMembership::OSIClanMembership(const OSIClanMembership& copyme)
{
	playerID = copyme.playerID;
	clanID = copyme.clanID;
	role = copyme.role;
	memcpy( memberName, copyme.memberName, sizeof( memberName ) );
	currentMatchList = new OSIMatchList( *copyme.currentMatchList );
	isOnline = copyme.isOnline;
	isFriend = copyme.isFriend;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Deconstructor: cleanup dynamic memory
///////////////////////////////////////////////////////////////////////////////////////
OSIClanMembership::~OSIClanMembership()
{
	delete currentMatchList; currentMatchList = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Equals operator: copy over everything
///////////////////////////////////////////////////////////////////////////////////////
OSIClanMembership& OSIClanMembership::operator=(const OSIClanMembership& copyme)
{
	if( &copyme == this ) { return *this; }

	playerID = copyme.playerID;
	clanID = copyme.clanID;
	role = copyme.role;
	memcpy( memberName, copyme.memberName, sizeof( memberName ) );
	*currentMatchList = *copyme.currentMatchList;
	isOnline = copyme.isOnline;
	isFriend = copyme.isFriend;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Set correct initial values
///////////////////////////////////////////////////////////////////////////////////////
OSIClanInvitation::OSIClanInvitation()
{
	invitationID = 0;
	senderName[0] = '\0';
	senderID.xBox.userFlags = 0;
	senderID.xBox.userId = 0;
	receiverName[0] = '\0';
}


///////////////////////////////////////////////////////////////////////////////////////
/// OSIClanRole enum to string
/// OSIFIXME: change this static string array to match whatever solution is
///           decided for the localization fix
///////////////////////////////////////////////////////////////////////////////////////
const OSIchar* OSIClanRoleToStr(OSIClanRole osiClanRole)
{
	static const OSIchar clanRoleNames[MAX_OSI_CLAN_ROLE][OSI_STRING_LENGTH_LONG] = {
		"Unknown",
		"Leader",
		"Officer",
		"Soldier"
	};

	return clanRoleNames[osiClanRole];
}

///////////////////////////////////////////////////////////////////////////////////////
/// Concatenate the players clan abbreviation and name
///
/// \param fullNameBuffer buffer to return the full name, must be large enough to hold
///                       max clan tag length with name length with 3 extra chars
/// \param clanAbbr the clan abbreviation for the player
/// \param name the players online name
///////////////////////////////////////////////////////////////////////////////////////
const OSIchar* GetFullPlayerName( OSIchar* fullNameBuffer, const OSIchar* clanAbbr, const OSIchar* name )
{
	ASSERT( fullNameBuffer != NULL );

	fullNameBuffer[0] = 0;

	if( clanAbbr != NULL && clanAbbr[0] != 0 )
	{
		OSIsnprintf( fullNameBuffer, OSIClanInfo::MAX_FULL_NAME_LENGTH, "<%s> ", clanAbbr );
	}

	return OSIstrncat( fullNameBuffer, name, OSIClanInfo::MAX_FULL_NAME_LENGTH );
}



