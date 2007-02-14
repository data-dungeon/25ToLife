/*
**
**  File:   Lobby.h
**  Date:   October 22, 2004
**  By:     Bryant Collard
**  Desc:   The lobby.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/main/Lobby.h $
**  $Revision: #13 $
**  $DateTime: 2005/08/25 14:58:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef LOBBY_H
#define LOBBY_H

// GameLib includes
#include "EngineHelper/Singleton.h"
#ifdef NETZ
#include "Network/NetZ/NetZSessionSearch.h"
#else
#include "Network/SessionSearch.h"
#endif

class CLobby
{
  public:
	// Construct / Destruct
	virtual ~CLobby() {}

	// Queries
	virtual bool IsLAN(void) = 0;
	virtual bool IsOrphanSession(void) = 0;
	virtual const char* GetDisconnectMessage(void) { return NULL; }

	// Maintainance
	virtual void Activate(void) {}
	virtual void Deactivate(void) {}
	virtual void Reset(void) = 0;
	virtual void Update(void) {}
	virtual bool IsRequestPending(void) {return(false);}

	// IDs
	virtual bool GetLocalAccountId(byte* o_accountId) = 0;
	virtual bool GetRoundId(byte* o_roundId) = 0;
	virtual void SetRoundId(byte* i_roundId) = 0;

	// Match Management
	virtual void OpenMatch(void) {}
	virtual void CloseMatch(void) {}

	// Round Management
	virtual void OpenRound(void) = 0;
	virtual void JoinRound(void) {}
	virtual void StartRound(void) = 0;
	virtual void EndRound(void) = 0;
	virtual void QuitRound(void) = 0;

	// Stats
	virtual int32 GetLocalPlayerRating(void) = 0;
	virtual int32 GetLocalPlayerRank(void) = 0;

	// Manipulate Singleton
	static void Start(CLobby &i_lobby);
	static void Stop(void);
	static bool RequestComplete(void);

	// Session Search
	virtual CSessionSearch &SessionSearch(void) = 0;
};

class CLanLobby : public CLobby
{
  public:
	// Construction
	CLanLobby();

	// Queries
	virtual bool IsLAN(void) {return(true);}
	virtual bool IsOrphanSession(void) {return(false);}

	// Maintainance
	virtual void Reset(void);

	// IDs
	virtual bool GetLocalAccountId(byte* o_accountId);
	virtual bool GetRoundId(byte* o_roundId);
	virtual void SetRoundId(byte* i_roundId);

	// Round Management
	virtual void OpenRound(void);
	virtual void StartRound(void);
	virtual void EndRound(void);
	virtual void QuitRound(void) { EndRound(); }

	// Stats
	virtual int32 GetLocalPlayerRating(void);
	virtual int32 GetLocalPlayerRank(void);

	// Session Search
	virtual CSessionSearch &SessionSearch(void) {return(d_sessionSearch);}

  private:
	byte d_roundId[ROUND_ID_SIZE];
#ifdef NETZ
	CNetZSessionSearch d_sessionSearch;
#else
	CSessionSearch d_sessionSearch;
#endif
};

DefineSingleton(CLobby)
#define g_lobby GetSingleton(CLobby)

#endif // LOBBY_H
