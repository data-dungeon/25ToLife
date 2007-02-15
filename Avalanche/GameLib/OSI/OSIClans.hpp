#ifndef _INCLUDED_OSI_CLANS_
#define _INCLUDED_OSI_CLANS_

#include "OSIPlatformTypes.h"
#include "OSIPlayerID.hpp"

class OSIMatchList;
class OSIClanStats;

///-------------------------------------------------------------------------------------------------
union OSIClanID
///-------------------------------------------------------------------------------------------------
{
	OSIu32 ps2;

	// XUID - Keep in mind that this structure may not be packed and the sizeof this
	// whole union may be 16 bytes (it is on xbox/debug at least).
	struct 
	{
		OSIu64	clanId;
		OSIu32	userFlags;
	} xBox;

	OSIClanID( OSIu64 xboxId = 0, OSIu32 xboxFlags = 0 )
	{
		xBox.clanId = xboxId;
		xBox.userFlags = xboxFlags;
	}

	OSIClanID( OSIu32 ps2Id )
	{
		ps2 = ps2Id;
	}

	bool isValid() const { return xBox.clanId != 0 ; }
	void setInvalid()	{ xBox.clanId = 0 ; }
};

inline bool operator ==( const OSIClanID& lhs, const OSIClanID& rhs )
{
#ifdef _XBOX
	return( lhs.xBox.clanId == rhs.xBox.clanId );
#else
	return( lhs.ps2 == rhs.ps2 );
#endif
}

inline bool operator !=( const OSIClanID& lhs, const OSIClanID& rhs )
{
	return !(lhs == rhs);
}





///-------------------------------------------------------------------------------------------------
class OSIClanStats
///-------------------------------------------------------------------------------------------------
{

public:
	enum ClanStatIndex {
		CLANSTAT_RANK,
		CLANSTAT_RATING,
		CLANSTAT_KILLS,
		NUM_CLANSTATS
	};

	OSIClanStats()
	{
		clearStats();
	}

	typedef float StatValueType;
	StatValueType	statsArray[ NUM_CLANSTATS ];

	OSIClanID clanID;

	inline void clearStats();
	inline void setStat(ClanStatIndex statIndex, float value);
	inline void setStat(ClanStatIndex statIndex, int value );
	inline float getStat(ClanStatIndex statIndex) const;
};

// Clear all the stats to 0.
void OSIClanStats::clearStats()
{
	for ( int i = 0; i<NUM_CLANSTATS; i++ )
		statsArray[i] = 0;
}

void OSIClanStats::setStat(ClanStatIndex statIndex, float value)
{
	statsArray[statIndex] = (StatValueType) value;
}

void OSIClanStats::setStat(ClanStatIndex statIndex, int value)
{
	statsArray[statIndex] = (StatValueType) value;
}

float OSIClanStats::getStat(ClanStatIndex statIndex) const
{
	return statsArray[statIndex];
}





///-------------------------------------------------------------------------------------------------
//! \brief	Contains all of the information about a clan, but no information about the players
//!	\brief	who are members of the clan.
///-------------------------------------------------------------------------------------------------
struct OSIClanInfo
///-------------------------------------------------------------------------------------------------
{
	OSIClanInfo();

	static const OSIu32 MAX_TAG_LENGTH	= 4;
	static const OSIu32 MAX_NAME_LENGTH	= 16;
	static const OSIu32 MAX_FULL_NAME_LENGTH = MAX_NAME_LENGTH + MAX_TAG_LENGTH + 2;


	//! \name	Volatile data.
	//! \brief	Data that is never guaranteed to be current.  It's whatever was last retrieved 
	//!	\brief	from the network service, which may have changed externally since it was last 
	//!	\brief	retrieved.
	//! \{
	OSIchar			tag[ MAX_TAG_LENGTH ];		//!< e.g. "WTF"
	OSIchar			name[ MAX_NAME_LENGTH ];	//!< e.g. "WHaT THe FiG"

	//! \brief	The number of members that were in the clan the last time this data was retrieved
	//!			from the online service.
	OSIu32			memberCount;

	//! \brief	The name of the leader of this clan.
	OSIchar			leaderName[ OSI_PLAYER_NAME_LENGTH ];

	//! \brief	The stats ladder data for this clan.
	OSIClanStats	stats;
	//! \}
	
	//! \name	Constant, non-volatile data.
	//! \brief	Data that is set when the clan is created and doesn't change for any reason.
	//! \{
	//! \brief	The unique identifier for this clan.
	OSIClanID		clanID;
	//! \}

};

///-------------------------------------------------------------------------------------------------
enum OSIClanRole
///-------------------------------------------------------------------------------------------------
{
	OSI_CLAN_ROLE_UNKNOWN,
	OSI_CLAN_ROLE_LEADER,
	OSI_CLAN_ROLE_OFFICER,
	OSI_CLAN_ROLE_SOLDIER,
	
	MAX_OSI_CLAN_ROLE
};

///-------------------------------------------------------------------------------------------------
inline OSIClanRole OSIClanRolePromote( OSIClanRole currentRole )
///-------------------------------------------------------------------------------------------------
{
	switch( currentRole )
	{
	case OSI_CLAN_ROLE_LEADER:	return OSI_CLAN_ROLE_LEADER;
	case OSI_CLAN_ROLE_OFFICER:	return OSI_CLAN_ROLE_LEADER;
	case OSI_CLAN_ROLE_SOLDIER:	return OSI_CLAN_ROLE_OFFICER;
	default:					return OSI_CLAN_ROLE_SOLDIER;
	};
}

///-------------------------------------------------------------------------------------------------
inline OSIClanRole OSIClanRoleDemote( OSIClanRole currentRole )
///-------------------------------------------------------------------------------------------------
{
	switch( currentRole )
	{
	case OSI_CLAN_ROLE_LEADER:	return OSI_CLAN_ROLE_OFFICER;
	case OSI_CLAN_ROLE_OFFICER:	return OSI_CLAN_ROLE_SOLDIER;
	case OSI_CLAN_ROLE_SOLDIER:	return OSI_CLAN_ROLE_SOLDIER;
	default:					return OSI_CLAN_ROLE_SOLDIER;
	};
}

///-------------------------------------------------------------------------------------------------
//! \brief	A link between a player and a clan.
///-------------------------------------------------------------------------------------------------
class OSIClanMembership
///-------------------------------------------------------------------------------------------------
{
public:
	OSIClanMembership();
	OSIClanMembership( const OSIClanMembership& copyme );
	OSIClanMembership& operator=( const OSIClanMembership& copyme );
	~OSIClanMembership();

	//! \brief	Unique identifier for this player (same PlayerID that would appear in OSIPlayer).
	OSIPlayerID playerID;
	
	//! \brief	The clan that this member data applies to.
	OSIClanID clanID;

	//! \brief  The name of this clan member.
	OSIchar memberName[ OSI_PLAYER_NAME_LENGTH ];

	//! \brief  List of the current match this member is in, empty for not in a match or irrelevant.
	OSIMatchList *currentMatchList;

	//! \brief  Is this player currently online?
	bool isOnline;

	//! \brief  Is this player a friend of mine?
	bool isFriend;

	//! \brief	The player's role and permissions in the clan
	OSIClanRole role;
};

///-------------------------------------------------------------------------------------------------
//! \brief	A notice is a piece of text posted for the clan by a member of the clan.
///-------------------------------------------------------------------------------------------------
struct OSIClanNotice
///-------------------------------------------------------------------------------------------------
{
	//! \brief  The name of the poster of the notice.
	OSIchar posterName[ OSI_PLAYER_NAME_LENGTH ];
	//! \brief  The text of the notice.
	OSIchar noticeText[ OSI_STRING_LENGTH_LONG ];
};

///-------------------------------------------------------------------------------------------------
typedef OSIu32 OSIClanInvitationID;

///-------------------------------------------------------------------------------------------------
//! \brief A single instance of an invitation from a player in a clan to another player.
///-------------------------------------------------------------------------------------------------
struct OSIClanInvitation
///-------------------------------------------------------------------------------------------------
{
	OSIClanInvitation();

	//! \brief	The ID of this invitation.
	OSIClanInvitationID invitationID;

	//! \brief	Information about the clan that would be joined if the invitation is accepted.
	OSIClanInfo clanInfo;
	
	//! \brief	The name of the person sending the invitation; same as OSIPlayer::name.
	OSIchar senderName[ OSI_PLAYER_NAME_LENGTH ];

	//! \brief	The ID of the person sending the invitation; same as OSIPlayer::id.
	OSIPlayerID senderID;

	//! \brief	The name of the person receiving the invitation; same as OSIPlayer::name.
	OSIchar receiverName[ OSI_PLAYER_NAME_LENGTH ];

#ifdef _XBOX
	OSIPlayerID receiverID;
#endif // _XBOX
};

// Utility functions for Clans
const OSIchar* OSIClanRoleToStr(OSIClanRole osiClanRole);

const OSIchar* GetFullPlayerName( OSIchar* fullNameBuffer, const OSIchar* clanAbbr, const OSIchar* name );



#endif // _INCLUDED_OSI_CLANS_
