//
//
//  File:   OSI.ddl
//  Date:   September 27, 2004
//  By:     Bryant Collard
//  Desc:   RendezVous specific DDL definitions.
//
//  Copyright (c) 2004, Avalanche Software Inc.
//  All Rights Reserved.
//
//      $File: //depot/Avalanche/GameLib/OSI/PS2/OSI.ddl $
//  $Revision: #41 $
//  $DateTime: 2005/08/17 00:16:58 $
//    $Author: Food $
//
//

#include <Services/MatchMaking.ddl>
#include <Services/Friends.ddl>
#include <Services/Extensions/HostMigration.ddl>

class TTLSession: Gathering {
	string levelName;
	//string hostAvatar;
	//string joineeAvatar;
	
	uint32 levelIndex;
	uint32 gameType;
	uint32 timeLimit;	
	uint32 scoreLimit;
	uint32 tagLimit;
	uint32 bestOfSeries;
	uint32 maxPlayers;
	uint32 reservedPlayers;
	uint32 totalConnectedPlayers;

	bool isCustomQuery;
	bool isRanked;
	bool isPrivate;
	bool playersRespawn;
	bool friendlyFire;
	bool apprehension;
	bool arcadeDamage;
	bool selfDamage;
	bool alternatingTeams;	
} concrete;


//--------------------------------------
// This is a single round within a match
// It is used to track scoring of the round.
//--------------------------------------
class TTLRound: Gathering {
	bool isPrivate;
	uint32 levelIndex;
	uint32 gameType;
	uint32 timeLimit;
	string levelName;
	uint32 ranking;
} concrete ;

class EidosFriend
{
	FriendData					m_friendData;
	string 						m_strClanAbbr;		// Clan abbreviation, or "" for unaffiliated.
	std_list<GatheringHolder>	m_lstGatherings;	// This is either TTLSession Gathering or empty.
	bool						m_isLookingToPlay;	// has this friend invited me to his match?
}
concrete;


///////////////////////////////////////////////////////////////////////////////
/// Alert system for knowing about new stuff in RendezVous
///////////////////////////////////////////////////////////////////////////////
class EidosAlerts
{
	std_list<principalid> m_friendInvitations;
	std_list<principalid> m_clanInvitations;
	std_list<principalid> m_matchInvitations;
} concrete;


///////////////////////////////////////////////////////////////////////////////
/// Clan classes and structures
///////////////////////////////////////////////////////////////////////////////

//
// NOTE: "string" types are actually UTF8Strings.
// The wire protocol and Python backend shouldn't care about this,
// as long as they are zero-terminated char strings.
// C++ code must properly wrap/unwrap these sz char arrays.
// The only issue is with database sorting, but that's not within our
// control, since RV doesn't currently support UTF8 Strings.
//

// Wire protocol description of clan notice.
// (Mirrors real class NetCommClanNotice)
class EidosClanNotice
{
	string	m_From;		// author who updated notice.
	string	m_Body;		// freeform text contents of notice.
} concrete;

// Wire protocol description of a clan.
// (Mirrors real class EidosClanProps)
class EidosClanProps
{
	uint32							m_ClanID;		   // Clan ID.
	string							m_ClanAbbr;		   // Clan abbreviation.
	string							m_ClanName;		   // Clan name.
	int32							m_ClanRank;		   // Rank of clan compared to other clans. -1 = unknown.
	int32							m_ClanRating;      // Magic number rating of clan.
	uint32							m_ClanKills;       // Number of kills clan members have made while in clan.
	uint32							m_ClanMemberCount; // Number of members in the clan.
	string							m_ClanLeader;      // Name of the leader of the clan.
} concrete;

// Wire protocol description of clan member.
// (Mirrors real class EidosClanMember)
class EidosClanMember
{
	principalid					m_PlayerID;					// Player ID.
	string						m_Player;					// Player name.
	string						m_ClanAbbr;					// Clan abbreviation.
	bool						m_ClanIsLeader;				// Is this member the (1) leader of the clan?
	bool						m_ClanIsOfficer;			// Is this member an officer in clan?
	bool						m_IsAcceptingInvites;		// Is this member accepting clan invitations at all?
	std_list<GatheringHolder>	m_NetLobbyMatch;			// match info (server, game, players, etc.) empty=not in game or irrelevant.
	bool						m_IsFriend;					// Is this member a friend of mine?
	bool						m_IsOnline;					// Is this member online (in this SKU)?
} concrete;

// Wire protocol description of a clan invitation.
// (Mirrors real class EidosClanInvite)
class EidosClanInvite
{
	uint32		m_InviterClanID;			// ID of clan sending invitation.
	string		m_InviterClanAbbr;			// Abbr of clan sending invitation.
	string		m_InviterClanName;			// Name of clan sending invitation.
	string		m_InviterPlayer;			// Player who sent invitation.
	int32		m_InviterClanRank;			// Rank of inviting clan. -1 = unknown.
	uint32		m_InviterClanMembers;		// Members in inviting clan.
	string		m_InviteePlayer;			// Person who was invited.
	bool		m_IsNew;					// New invitation? (false = blocked).
} concrete;

// NOTE: Update EidosAccounts.py when this changes!
class EidosAccountPrivate : Data
{
	// bit0: gender (0=female)
	// bit1: eidosCanSendMeEmails
	// bit2: eidosPartnersCanSendMeEmails
	byte		m_flags;

	string		m_postalCode;
	datetime	m_dateOfBirth;
}
concrete;

// NOTE: Update EidosAccounts.py when this changes!
class EidosAccountPublic : Data
{
	uint32		m_country;
}
concrete;

class TTLStats
{
	string				m_strPlayer;
	string				m_strClanAbbr;
	float				m_clanRating;
	float				m_clanKills;
	principalid			m_pidPlayer;
	std_list<float>		m_lstValues;
}
concrete;

protocol TTLMatchMaking {

	// Query API
	void FindByLevelName(string levelName, ResultRange range, out std_list<GatheringHolder> lstGathering);
	void FindBySQLQuery(string query, ResultRange range, out std_list<GatheringHolder> lstGathering);
	void FindByRanking(uint32 ranking, uint32 maxdiff, ResultRange range, out std_list<GatheringHolder> lstGathering);
	void FindRankingPosition(gatheringid gid, out uint32 position);
	void GetTopPlayers(uint32 count, out std_list<TTLStats> lstGameStats);
	void GetStatsForPlayers(std_list<principalid> pidList, out std_list<TTLStats> lstGameStats);
	void GetLadderStats(principalid pivotPlayer, uint32 count, out std_list<TTLStats> lstGameStats);
	//void GetDetailedParticipantsByRanking(gatheringid ladderid, gatheringid nonladderid, uint32 ranking, uint32 maxdiff, ResultRange range, out std_list<string> lstName, out std_list<TTLGameStats> lstGameStats);
	void GetAlerts(out EidosAlerts outAlerts);
	void AddStatsPersonal(std_list<float> statValues);
	bool FilterText(string textToFilter);
	
	// Friend API
	bool FriendInvite(string strPlayer);
	bool FriendInviteByID( principalid uiPlayer );
	bool FriendAccept(principalid uiPlayer);
	bool FriendReject(principalid uiPlayer, bool bBlock);
	bool FriendDelete(principalid uiPlayer);
	void FriendsQuery(string strLocale, out std_list<EidosFriend> lstFriendsList);	
	
	// Clan API
	// Major clan functions:
	uint32 ClanGetMyClanInfo(out EidosClanProps outProps, out EidosClanMember outPlayer);

	uint32 ClanCreateClan(string inClanName, string inClanAbbr);

	uint32 ClanGetMyClanMembershipList(out std_list<EidosClanMember> outMembers);
	
	uint32 ClanGetClanIDListForPlayer(principalid					inPid,
									  out std_list<principalid>	outClansList);
	
	uint32 ClanGetClanInfoFromClanID(principalid					inCid,
									 out EidosClanProps				outProps);

	// Invitations:
	uint32 ClanGetMyInvitations(out std_list<EidosClanInvite>		outInvitesToMe,
								out std_list<EidosClanInvite>		outInvitesFromMe);

	uint32 ClanInvitePlayer(string playerToInvite);
	uint32 ClanCancelInvitation(string playerToUninvite);

	uint32 ClanAcceptInvitation(uint32 clanIDInviter, string playerNameInviter);
	uint32 ClanRejectInvitation(uint32 clanIDInviter, string playerNameInviter);
	uint32 ClanBlockInvitation(uint32 clanIDInviter, string playerNameInviter);

	uint32 ClanSetInvitationStatus(bool isAcceptingInvites);

	// Member Management:
	uint32 ClanRemoveMember(principalid playerToRemove);
	uint32 ClanAbandonClan();

	uint32 ClanPromote(principalid playerToPromote);
	uint32 ClanDemote(principalid playerToDemote);

	uint32 ClanNewLeader(principalid playerNewLeader);

	// Misc:
	uint32 ClanPostNotice(string newNotice);
	uint32 ClanGetMyNotices(out std_list<EidosClanNotice> outNotices);

	uint32 ClanMarkAlertKickedRead();
	
	// Clan Stats
	void GetTopClans(uint32 count, out std_list<EidosClanProps> lstClanStats);
	
	void GetMessageOfTheDay(string inLanguage, out string outMessage);
	void GetEidosAccountInfo(out EidosAccountPrivate outPrivateData, out EidosAccountPublic outPublicData, out string email);
	
	// PC CD key check function
	//bool CheckForValidCDKey(string cdkey);
};
