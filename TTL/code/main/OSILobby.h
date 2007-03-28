/*
**
**  File:   OSILobby.h
**  Date:   October 22, 2004
**  By:     Bryant Collard
**  Desc:   Lobby wrapper for OSI.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/main/OSILobby.h $
**  $Revision: #25 $
**  $DateTime: 2005/08/16 16:51:53 $
**    $Author: Brian_Eiserloh $
**
*/
#if defined(NETWORKING)
#pragma once

#ifndef OSI_LOBBY_H
#define OSI_LOBBY_H

// GameLib includes
#include "GameHelper/Message.h"
#include "OSI/OSI.hpp"
#include "Network/OnlineSessionSearch.h"

// TTL includes
#include "main/Lobby.h"

class COSILobby : public CLobby
{
  public:
	// Construction
	COSILobby();
	virtual ~COSILobby();

	// Queries
	virtual bool IsLAN(void) {return(false);}
	virtual bool IsOrphanSession(void);
	virtual const char* GetDisconnectMessage(void);

	// Called when made the lobby singleton.
	virtual void Activate(void);

	// Called when replaced as the lobby singleton.
	virtual void Deactivate(void);

	// Called when entering and leaving the network loop to clear error
	// conditions and queued requests.
	virtual void Reset(void);

	// Called every frame when installed as the lobby singleton.
	virtual void Update(void);

	// Test to see if any requests have not completed.
	virtual bool IsRequestPending(void);

	// Fills in o_accountId with OSIPlayerID if one is valid.
	virtual bool GetLocalAccountId(byte* o_accountId);

	// Fills in o_roundId with the round id.
	virtual bool GetRoundId(byte* o_roundId);

	// Sets the round id if not the match host.
	virtual void SetRoundId(byte* i_roundId);

	// Called just after a session is opened. The loop waits until completion.
	// On the host, it requests a match.
	// On non-hosts it get the player id?
	virtual void OpenMatch(void);

	// Called just after a session is closed.
	virtual void CloseMatch(void);

	// Called when the referee enters the POPULATING phase.
	// On the host, it requests the creation of a round and waits. When a round,
	// is created, the referee puts the round id in its do.
	// On non-hosts, it waits until a round id shows up in the referee.
	virtual void OpenRound(void);

	// Called when a player is granted a lock so it can publish. Publishing waits
	// until the join is allowed.
	virtual void JoinRound(void);

	// Called on the host when all players have joined (except the host) and
	// published. On the host, it joins then starts the round.
	virtual void StartRound(void);

	// Called when a round ends.
	virtual void EndRound(void);

	// Called when a player voluntarily quits a round.
	virtual void QuitRound(void);

	// Called after ending the round to report stats to OSI (and for OSI to end the round on the server)
	virtual void ReportStatsForEndRound(void);

	// Tests to see if it is safe to make an OSI request
	virtual bool IsSafeToMakeAnOSIRequest( void );

	// Stats
	virtual int32 GetLocalPlayerRating(void);
	virtual int32 GetLocalPlayerRank(void);

	// Session Search
	virtual CSessionSearch &SessionSearch(void) {return(d_sessionSearch);}

  private:
	// Types of requests supported.
	enum
	{
		NO_REQUEST,
		REQUEST_ERROR,
		HOST_MATCH,
		JOIN_MATCH,
		QUIT_MATCH,
		MODIFY_MATCH,
		CHANGE_HOST,
		CREATE_ROUND,
		JOIN_ROUND,
		START_ROUND,
		END_ROUND,
		GET_STATS,
		FILTER_TEXT
	} d_requestType;

	enum RankingMode 
	{
		USER,
		COP,
		GANG
	};

	// Convert a config manager options string into OSI options.
	static bool PopulateMatchOptions(OSIMatchOptions &o_options);
	static void _setOSIMatchOptionFromConfigManager( 
		OSIMatchOptions& options, 
		const OSIMatchOptions::MatchOptionsWildcardFlag flag, 
		int& value, 
		const char* const name );

	static void _setOSIMatchOptionFromConfigManager( 
		OSIMatchOptions& options, 
		const OSIMatchOptions::MatchOptionsWildcardFlag flag, 
		bool& value, 
		const char* const name );

	static void _setOSIMatchOptionFromConfigManager( 
		OSIMatchOptions& options, 
		const OSIMatchOptions::MatchOptionsWildcardFlag flag, 
		OSIchar value[ OSI_STRING_LENGTH_LONG ], 
		const char* const name );

	void UpdatePlayerRankAndRating( void );
	void PopulateRoundStats(OSIPlayerStats& stats, CPlayerAgent* pAgent);
	void PrepareStats();
	void CalculateStats();
	float GetPlayerRating(OSIPlayerID id, RankingMode mode);
	void CalcMVPScores();
	void CalcMVPAward();
	float CalcPlayerRatingDelta(OSIPlayerStats& playerStats, float rating, RankingMode mode);
	void FilterText(const char* text);

	// Requests that are more event driven may need to be queued.
	bool d_changeHost;
	OSIPlayerID d_hostId;  // Is there an invalid OSIPlayerID?
	OSIRoundStats roundStats;
	OSIPlayerStatsList currentStats;
	bool d_changeConfig;
	bool d_quitRound;

	// Messages
	CMessageOwner d_messageOwner;
	REF_SEND_MESSAGE_HANDLER(CChangeConfig, COSILobby) d_changeConfigHandler;
	REF_SEND_MESSAGE_HANDLER(CChangeHost, COSILobby) d_changeHostHandler;
	REF_SEND_MESSAGE_HANDLER(CAdd, COSILobby) d_addHandler;
	REF_SEND_MESSAGE_HANDLER(CRemove, COSILobby) d_removeHandler;
	REF_SEND_MESSAGE_HANDLER(CQuit, COSILobby) d_quitHandler;
	REF_BASIC_MESSAGE_HANDLER(CFilterText, MESSAGE_STRING, COSILobby) d_filterTextHandler;

	void ChangeMatchMembership(uint8 i_id, bool i_add);

	COnlineSessionSearch d_sessionSearch;
};

#endif // OSI_LOBBY_H
#endif