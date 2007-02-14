/*
**
**  File:   Lobby.cpp
**  Date:   October 22, 2004
**  By:     Bryant Collard
**  Desc:   The lobby.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/main/Lobby.cpp $
**  $Revision: #7 $
**  $DateTime: 2005/08/10 23:15:11 $
**    $Author: Food $
**
*/

// Precompiled header.
#include "TTLPCH.h"

// My header
#include "main/Lobby.h"

DeclareSingleton(CLobby);

//////////////////////////////////// CLobby ////////////////////////////////////

/******************************************************************************/
/******************************************************************************/
void CLobby::Start(CLobby &i_lobby)
{
	if (!IsSingleton(CLobby, &i_lobby))
	{
		Stop();
		SetSingleton(CLobby, &i_lobby);
		g_lobby.Activate();
	}
}

/******************************************************************************/
/******************************************************************************/
void CLobby::Stop(void)
{
	if (ValidSingleton(CLobby))
	{
		g_lobby.Deactivate();
		ClearSingleton(CLobby);
	}
}

/******************************************************************************/
/******************************************************************************/
bool CLobby::RequestComplete(void)
{
	return(!g_lobby.IsRequestPending());
}

////////////////////////////////// CLanLobby ///////////////////////////////////

/******************************************************************************/
/******************************************************************************/
CLanLobby::CLanLobby()
{
	Reset();
}

/******************************************************************************/
/******************************************************************************/

void CLanLobby::Reset(void)
{
	memset(d_roundId, 0, ROUND_ID_SIZE);
}

/******************************************************************************/
/******************************************************************************/

bool CLanLobby::GetLocalAccountId(byte* o_accountId)
{
	memset(o_accountId, 0, ACCOUNT_ID_SIZE);
	return(true);
}

/******************************************************************************/
/******************************************************************************/
bool CLanLobby::GetRoundId(byte* o_roundId)
{
	memcpy(o_roundId, d_roundId, ROUND_ID_SIZE);
	return(true);
}

/******************************************************************************/
/******************************************************************************/
void CLanLobby::SetRoundId(byte* i_roundId)
{
	if (i_roundId != NULL)
		memcpy(d_roundId, i_roundId, ROUND_ID_SIZE);
}

/******************************************************************************/
/******************************************************************************/
void CLanLobby::OpenRound(void)
{
	memset(d_roundId, ~0, ROUND_ID_SIZE);
	g_messageDispatcher.SendMessageToAll("RoundOpened", NULL,
			INVALID_OBJECT_HANDLE);
}

/******************************************************************************/
/******************************************************************************/
void CLanLobby::StartRound(void)
{
	g_messageDispatcher.SendMessageToAll("RoundStarted", NULL,
			INVALID_OBJECT_HANDLE);
}

/******************************************************************************/
/******************************************************************************/
void CLanLobby::EndRound(void)
{
	memset(d_roundId, 0, ROUND_ID_SIZE);
	g_messageDispatcher.SendMessageToAll("RoundEnded", NULL,
			INVALID_OBJECT_HANDLE);
}

/******************************************************************************/
/******************************************************************************/
int32 CLanLobby::GetLocalPlayerRating(void)
{
	return(g_playerProfile.GetRating());
}

/******************************************************************************/
/******************************************************************************/
int32 CLanLobby::GetLocalPlayerRank(void)
{
	return(g_playerProfile.GetRank());
}
