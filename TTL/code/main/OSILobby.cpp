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
**      $File: //depot/TTL/code/main/OSILobby.cpp $
**  $Revision: #113 $
**  $DateTime: 2005/08/17 16:18:22 $
**    $Author: Brian_Eiserloh $
**
*/
#if defined(NETWORKING)
// Precompiled header.
#include "TTLPCH.h"

// My header
#include "main/OSILobby.h"

// GameLib includes
#include "Network/Session.h"
#include "Network/SessionSpec.h"
#include "GameHelper/ErrorStack.h"
#include "OSI/OSI.hpp"
#include "frontend/UseCase/UseCase.hpp"
#include "frontend/UseCase/UseCaseJoinMatch.hpp"
#include "frontend/UseCase/UseCaseReadyUp.hpp"
#ifdef _XBOX
#include "OSI/XBox/OSIXBox.hpp"
#endif

// TTL includes
#include "environ/ConfigMgr.h"
#include "weapons/ttlweapstats.h"

#define l_osi (*OSI::getSingletonPtr())

/******************************************************************************/
/******************************************************************************/
int GetNumPlayersInMatch( uint8 idPlayerLeaving = INVALID_PARTICIPANT )
{
	uint8 Player = INVALID_PARTICIPANT;
	int totalPlayers = 0;
	while ( ( Player = g_referee.GetNextPlayer( Player ) ) != INVALID_PARTICIPANT )
	{
		// Don't count the leaving player
		if( Player != idPlayerLeaving )
			++totalPlayers;
	}
	return totalPlayers;
}

/******************************************************************************/
/******************************************************************************/
COSILobby::COSILobby() : d_messageOwner("OSILobby")
{
	Reset();
}

/******************************************************************************/
/******************************************************************************/
COSILobby::~COSILobby()
{
	Reset();
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::Activate(void)
{
//#ifdef DIRECTX_PC
//	// we need a valid cdkey to play
//	l_osi.setCDKey( g_gameEnvironment.GetCdKeyPtr() );
//#endif
	uint8 category = g_debugOutput.RegisterCategory("OSI Lobby", "LBY");
	g_debugOutput.TSPrint(category, "Activating OSI Lobby");
	d_changeConfigHandler.Initialize("GameOptionsChanged",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
	d_changeHostHandler.Initialize("SessionMasterChanged",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
	d_addHandler.Initialize("PlayerAdded",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
	d_removeHandler.Initialize("PlayerRemoved",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
	d_quitHandler.Initialize("Quit",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
	d_filterTextHandler.Initialize("FilterText",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
	Reset();
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::Deactivate(void)
{
	g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
			"Deactivating OSI Lobby");
	d_messageOwner.UnregisterAll();
	Reset();
}

void COSILobby::UpdatePlayerRankAndRating( void )
{
#if defined( PS2 ) || defined( DIRECTX_PC )
	OSIPlayer *activePlayer = l_osi.getActivePlayer();
	if ( activePlayer )
	{
		g_playerProfile.SetRank( activePlayer->statRanking );
		g_playerProfile.SetRating( activePlayer->statRating );
	}
#endif
}

bool COSILobby::IsOrphanSession( void )
{
	return l_osi.isOrphanSession();
}


//---------------------------------------------------------------------------
// Returns true if we can issue an OSI request.  Currently, we should not
// call OSI if we are in an error state, or if the network cable is disconnected.
//---------------------------------------------------------------------------
bool COSILobby::IsSafeToMakeAnOSIRequest( void )
{
#if defined(NETZ)
	if ( d_requestType != REQUEST_ERROR && CNetZConnection::GetLinkStatus() )
		return true ;
#endif
	return false ;
}


/******************************************************************************/
/******************************************************************************/
const char* COSILobby::GetDisconnectMessage(void)
{
	static const char gameDiscoMsg[] = "Disconnecting from game...";
	static const char networkDiscoMsg[] = "Disconnecting from network...";

	// If we are in a game, use game disconnect message
	if( OnlineUI::ReadyUp::getSingletonPtr()->isRunning() )
		return gameDiscoMsg;

	return networkDiscoMsg;
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::CChangeConfig::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	if ( !OSI::isCreated() ) return ;

	if (g_session.IsMaster() && (l_osi.getCurrentMatch() != NULL) &&
			(d_ref->d_requestType != REQUEST_ERROR))
	{
		if (d_ref->d_requestType == NO_REQUEST)
		{
			OSIMatchOptions options;
			OSIMatch* osimatch = l_osi.getCurrentMatch();
			if ( osimatch )
			{
				options = osimatch->options;
			}
			PopulateMatchOptions(options);
			l_osi.requestUpdateMatchOptions(options);
			d_ref->d_requestType = MODIFY_MATCH;
			g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
					"Request update match options");
		}
		else
			d_ref->d_changeConfig = true;
	}
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::CChangeHost::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	ASSERT(i_data != NULL);
	if (i_data != NULL)
	{
		ASSERT(sizeof(OSIPlayerID) <= ACCOUNT_ID_SIZE);
		memcpy(&d_ref->d_hostId, i_data, sizeof(OSIPlayerID));
		if (d_ref->d_requestType == NO_REQUEST)
		{
			if( OSIMatch* osimatch = l_osi.getCurrentMatch() )
			{
				PopulateMatchOptions( osimatch->options );
				
				bool success = l_osi.requestHostMigration(d_ref->d_hostId, GetNumPlayersInMatch());
				if (success)
					d_ref->d_requestType = CHANGE_HOST;
				g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
						"Request host migration");
			}
		}
		else
			d_ref->d_changeHost = true;
	}
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::CAdd::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	d_ref->ChangeMatchMembership((uint8)i_data, true);
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::CRemove::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	d_ref->ChangeMatchMembership((uint8)i_data, false);
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::CQuit::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	// Tell OSI we are quiting
	l_osi.activePlayerWantingToQuit();
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::CFilterText::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	d_ref->FilterText( (const char*) i_data );
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::ChangeMatchMembership(uint8 i_id, bool i_add)
{
	if (IsSafeToMakeAnOSIRequest() && ValidSingleton(Referee))
	{
		CPlayerAgent* agent = g_referee.GetPlayerAgent(i_id);
		if (agent != NULL)
		{
			ASSERT(sizeof(OSIPlayerID) <= ACCOUNT_ID_SIZE);
			OSIPlayerID playerId;
			memcpy(&playerId, agent->GetAccountId(), sizeof(OSIPlayerID));
			if (i_add)
			{
				l_osi.playerJoinCurrentMatch(agent->GetAccountName(),
						playerId, GetNumPlayersInMatch());
				g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
						"Request player join current match");
			}
			else
			{
				l_osi.playerLeaveCurrentMatch(agent->GetAccountName(),
						playerId, GetNumPlayersInMatch( i_id ));
				g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
						"Request player leave current match");
			}
		}
	}
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::Reset(void)
{
	d_requestType = NO_REQUEST;
	d_changeHost = false;
	d_changeConfig = false;
	d_quitRound = false;
	d_sessionSearch.End(); // clean up our search results.
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::Update(void)
{
	if( OSI::isCreated() )
	{
		OSI::runFrame();
	} 
	if (OnlineUI::UseCase::allowUpdate() && !OnlineUI::UseCase::pendingNetworkErrorClose())
	{
		if( OnlineUI::UseCase::getActiveUseCase() )
		{
			OnlineUI::UseCase::getActiveUseCase()->update(); // IMPORTANT: OSI::runFrame MUST be called every frame BEFORE UseCase::update()!
		}
	}

	if ( (d_requestType != NO_REQUEST) && (d_requestType != REQUEST_ERROR) )
	{
		if (l_osi.processLastRequest())
		{
			switch(l_osi.getLastRequestStatus())
			{
			 case STATUS_WORKING:
				ASSERT(false);
				break;
			 case STATUS_SUCCEEDED:
				switch(d_requestType)
				{
				 case HOST_MATCH:
					g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
							"Host match succeeded");
					break;
				 case JOIN_MATCH:
					g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
							"Join match succeeded");
					break;
				 case QUIT_MATCH:
					g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
							"Quit match succeeded");
					break;
				 case MODIFY_MATCH:
					g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
							"Update match options succeeded");
					break;
				 case CHANGE_HOST:
					g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
							"Host migration succeeded");
					break;
				 case CREATE_ROUND:
					g_messageDispatcher.SendMessageToAll("RoundOpened", NULL,
							INVALID_OBJECT_HANDLE);
					g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
							"Create round succeeded");
					break;
				 case JOIN_ROUND:
					g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
							"Join round succeeded");
					break;
				 case START_ROUND:
					g_messageDispatcher.SendMessageToAll("RoundStarted", NULL,
							INVALID_OBJECT_HANDLE);
					g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
							"Start round succeeded");
					break;
				 case END_ROUND:
					g_messageDispatcher.SendMessageToAll("RoundEnded", NULL,
							INVALID_OBJECT_HANDLE);
					g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
							"End round succeeded");
					UpdatePlayerRankAndRating();
					break;
				 case GET_STATS:
					g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
							"Request stats player list succeeded");
					ReportStatsForEndRound();
					return ; // purposefully return, because we don't want to hit the d_requestType = NO_REQUEST below
					         // because ReportStatsForEndRound sets the RequestType to be END_ROUND.  This is on purpose.
				 case FILTER_TEXT:
					g_messageDispatcher.SendMessageToLocalHost( "TextFiltered", (void*)false,
								INVALID_OBJECT_HANDLE );
					g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
							"Request filter text succeeded");
					break;
				}
				break;
			 case STATUS_FAILED:
				if ( l_osi.isLastErrorFatal() )
				{
					if (d_requestType != REQUEST_ERROR)
					{
						OSIchar errorMsg[ 512 ];
						if( GetDisconnectMessage() )
						{
							OSIsnprintf( errorMsg, NUM_ARRAY_ELEMENTS(errorMsg), "%s %s",
								OSI::getSingletonPtr()->getErrorString(), GetDisconnectMessage()
							);
						}
						else
						{
							OSIstrncat( errorMsg, OSI::getSingletonPtr()->getErrorString(), NUM_ARRAY_ELEMENTS(errorMsg) );
						}
						g_MemCardManager->SetCancelOperation(true);
						g_errorStack.SetError("Fatal Lobby Error", errorMsg,
							CErrorStack::ERROR_FATAL, 5000);
						d_requestType = REQUEST_ERROR;
						g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
							"Request failed: %s", l_osi.getErrorString());
					}
				}
				else if( d_requestType != REQUEST_ERROR )
				{
					switch(d_requestType)
					{
					case FILTER_TEXT:
						g_messageDispatcher.SendMessageToLocalHost( "TextFiltered", (void*)true,
								INVALID_OBJECT_HANDLE );
						g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
							"Request filter text failed");
						break;
					}
					d_requestType = NO_REQUEST;
				}
				return;
			}
			d_requestType = NO_REQUEST;
		}
	}

	if (d_requestType == NO_REQUEST)
	{
		if (d_changeHost)
		{
			bool success = l_osi.requestHostMigration(d_hostId, GetNumPlayersInMatch());
			if( success )
				d_requestType = CHANGE_HOST;
			d_changeHost = false;
			g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
					"Delayed request host migration");
		}
		else if (d_changeConfig)
		{
			OSIMatchOptions options ;
			OSIMatch* osimatch = l_osi.getCurrentMatch();
			if ( osimatch )
			{
				options = osimatch->options ;
			}
			PopulateMatchOptions(options);
			l_osi.requestUpdateMatchOptions(options);
			d_requestType = MODIFY_MATCH;
			d_changeConfig = false;
			g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
					"Delayed request update match options");
		}
	}
}

/******************************************************************************/
/******************************************************************************/
bool COSILobby::IsRequestPending(void)
{
	return((d_requestType != NO_REQUEST) && (d_requestType != REQUEST_ERROR));
}

/******************************************************************************/
/******************************************************************************/
bool COSILobby::GetLocalAccountId(byte* o_accountId)
{
	OSIPlayer* player = l_osi.getActivePlayer();
	if (player != NULL)
	{
		ASSERT(sizeof(OSIPlayerID) <= ACCOUNT_ID_SIZE);
		memcpy(o_accountId, &player->id, sizeof(OSIPlayerID));
		return(true);
	}
	return(false);
}

/******************************************************************************/
/******************************************************************************/
bool COSILobby::GetRoundId(byte* o_roundId)
{
	ASSERT(sizeof(OSIRoundID) <= ROUND_ID_SIZE);
	OSIRoundID roundId = l_osi.getRoundID();
	memcpy(o_roundId, &roundId, sizeof(OSIRoundID));
	return(true);
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::SetRoundId(byte* i_roundId)
{
	if (!g_session.IsMaster())
	{
		ASSERT(sizeof(OSIRoundID) <= ROUND_ID_SIZE);
		OSIRoundID roundId;
		memcpy(&roundId, i_roundId, sizeof(OSIRoundID));
		l_osi.setRoundID(roundId);
	}
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::OpenMatch(void)
{
	// TTLReset destroys the UIX render callback, so re-register it.
#ifdef _XBOX
    OSIXbox* osix = reinterpret_cast< OSIXbox* >( OSI::getSingletonPtr() );
	ASSERT( osix );
	bool UIXRenderCallback( void* dataPtr );
    RenderMgr__RegisterCallback(RENDER_UIX_CALLBACK, UIXRenderCallback, (void *)osix->getFontPlugin());
#endif

	if ( !IsSafeToMakeAnOSIRequest() )
	{
		//g_messageDispatcher.SendMessageToAll("RoundOpened", NULL, INVALID_OBJECT_HANDLE); // success message
		g_messageDispatcher.SendMessageToLocalHost("Quit", NULL, INVALID_OBJECT_HANDLE);			// failure message
		g_debugOutput.TSPrint(g_debugOutput.Category("LBY"), "Unable to request host match.");
		return;
	}

	ASSERT(d_requestType == NO_REQUEST);
	if (g_session.IsMaster())
	{
		OSIMatchOptions options ;
		// there is no match yet, so we just pass in empty options
		PopulateMatchOptions(options);
		l_osi.requestHostMatch(options);
		d_requestType = HOST_MATCH;
		g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
				"Request host match");
	}
	else
	{
		l_osi.registerForHostMigration();
		g_debugOutput.TSPrint(g_debugOutput.Category("LBY"), "Registered for host migration" );
	}
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::CloseMatch(void)
{
	if ( !IsSafeToMakeAnOSIRequest() )
	{
		g_debugOutput.TSPrint(g_debugOutput.Category("LBY"), "Unable to request quit match round.");
		return;
	}

	l_osi.requestQuitMatch();
	d_requestType = QUIT_MATCH;
	g_debugOutput.TSPrint(g_debugOutput.Category("LBY"), "Request quit match");
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::OpenRound(void)
{
	if ( !IsSafeToMakeAnOSIRequest() )
	{
		g_debugOutput.TSPrint(g_debugOutput.Category("LBY"), "Unable to request create round.");
		return;
	}

	ASSERT(d_requestType == NO_REQUEST);
	if (g_session.IsMaster())
	{
		l_osi.requestCreateRound();
		d_requestType = CREATE_ROUND;
		g_debugOutput.TSPrint(g_debugOutput.Category("LBY"), "Request create round");
	}
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::JoinRound(void)
{
	if ( !IsSafeToMakeAnOSIRequest() )
	{
		g_debugOutput.TSPrint(g_debugOutput.Category("LBY"), "Unable to request join round.");
		return;
	}

	ASSERT(d_requestType == NO_REQUEST);
	l_osi.requestJoinRound();
	d_requestType = JOIN_ROUND;
	g_debugOutput.TSPrint(g_debugOutput.Category("LBY"), "Request join round");
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::StartRound(void)
{
	if ( !IsSafeToMakeAnOSIRequest() )
	{
		//g_messageDispatcher.SendMessageToAll("RoundStarted", NULL, INVALID_OBJECT_HANDLE);	// success message
		g_messageDispatcher.SendMessageToLocalHost("Quit", NULL, INVALID_OBJECT_HANDLE);		// failure message
		g_debugOutput.TSPrint(g_debugOutput.Category("LBY"), "Unable to request start round.");
		return;
	}

	ASSERT(d_requestType == NO_REQUEST);
	if (g_session.IsMaster())
	{
		l_osi.requestStartRound();
		d_requestType = START_ROUND;
		g_debugOutput.TSPrint(g_debugOutput.Category("LBY"),
				"Request start round");
	}
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::PopulateRoundStats(OSIPlayerStats& stats, CPlayerAgent* pAgent)
{
	CScoringModel& score = g_referee.GetScoringModel();
	CTeamingModel& team = g_referee.GetTeamingModel();
	TTLWeaponStats* weapon = TTLWeaponStats::GetComponent(CActor::ToHandle(g_referee.GetActor(pAgent->Id())));

	uint8 me = pAgent->Id();
	uint8 us = score.GetPlayerTeam(me);
	uint8 them = INVALID_TEAM;
	if (team.IsTeamInGroupAlpha(us))
		them = team.FindTeamInGroupBeta();
	else if (team.IsTeamInGroupBeta(us))
		them = team.FindTeamInGroupAlpha();
	bool isGhost = g_referee.IsGhost(me);

	// Arbitrated stats (on PS2)
	stats.setStat(OSIPlayerStats::PLAYERSTAT_USER_RATING, 0 ); // Calculated Later
	stats.setStat(OSIPlayerStats::PLAYERSTAT_COP_RATING, 0 ); // Calculated Later
	stats.setStat(OSIPlayerStats::PLAYERSTAT_GANG_RATING, 0 ); // Calculated Later
	stats.setStat(OSIPlayerStats::PLAYERSTAT_COP_WINS, stats.isCop && !isGhost ? (score.GetTeamStatus(us) == score.WON ? 1 : 0 ) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_COP_GAMES_PLAYED, stats.isCop && !isGhost ? 1 : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_COP_PLAYERS_IN_ROUND, stats.isCop ? score.GetTeamPlayerCount(us) : score.GetTeamPlayerCount(them) );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_GANG_WINS, !stats.isCop && !isGhost ? (score.GetTeamStatus(us) == score.WON ? 1 : 0 ) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_GANG_GAMES_PLAYED, !stats.isCop && !isGhost ? 1 : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_GANG_PLAYERS_IN_ROUND, stats.isCop ? score.GetTeamPlayerCount(them) : score.GetTeamPlayerCount(us) );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_KILLS_AS_COP, stats.isCop && !isGhost ? score.GetPlayerCount(score.ENEMY_KILLS, me) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_DEATHS_AS_COP, stats.isCop && !isGhost ? score.GetPlayerCount(score.DEATHS, me) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_KILLS_AS_GANGSTA, !stats.isCop && !isGhost ? score.GetPlayerCount(score.ENEMY_KILLS, me) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_DEATHS_AS_GANGSTA, !stats.isCop && !isGhost ? score.GetPlayerCount(score.DEATHS, me) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_COP_MVPS, stats.isCop && !isGhost ? me == score.GetTeamMVP(us) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_GANG_MVPS, !stats.isCop && !isGhost ? me == score.GetTeamMVP(us) : 0 );

	// Start of non-arbitrated stats
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_RANK, stats.isCop ? 0 : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_WINS, stats.isCop && !isGhost ? (score.GetTeamStatus(us) == score.WON ? 1 : 0 ) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_LOSSES, stats.isCop && !isGhost ? (score.GetTeamStatus(us) == score.LOST ? 1 : 0 ) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_TIES, stats.isCop && !isGhost ? (score.GetTeamStatus(us) == score.TIED ? 1 : 0 ) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_WINS, !stats.isCop && !isGhost ? (score.GetTeamStatus(us) == score.WON ? 1 : 0 ) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_LOSSES, !stats.isCop && !isGhost ? (score.GetTeamStatus(us) == score.LOST ? 1 : 0 ) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_TIES, !stats.isCop && !isGhost ? (score.GetTeamStatus(us) == score.TIED ? 1 : 0 ) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_KILLS, stats.isCop && !isGhost ? score.GetPlayerCount(score.ENEMY_KILLS, me) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_KILLS, !stats.isCop && !isGhost ? score.GetPlayerCount(score.ENEMY_KILLS, me) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATHS, stats.isCop && !isGhost ? score.GetPlayerCount(score.DEATHS, me) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATHS, !stats.isCop && !isGhost ? score.GetPlayerCount(score.DEATHS, me) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_SUICIDES, stats.isCop && !isGhost ? score.GetPlayerCount(score.SUICIDES, me) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_SUICIDES, !stats.isCop && !isGhost ? score.GetPlayerCount(score.SUICIDES, me) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_TEAM_KILLS,  stats.isCop && !isGhost ? score.GetPlayerCount(score.TEAM_KILLS, me) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_TEAM_KILLS, !stats.isCop && !isGhost ? score.GetPlayerCount(score.TEAM_KILLS, me) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_TEAM_DEATHS,  stats.isCop && !isGhost ? score.GetPlayerCount(score.TEAM_KILLED, me) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_TEAM_DEATHS, !stats.isCop && !isGhost ? score.GetPlayerCount(score.TEAM_KILLED, me) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_NPC_KILLS,  stats.isCop && !isGhost ? score.GetPlayerCount(score.NPC_KILLS, me) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_NPC_KILLS, !stats.isCop && !isGhost ? score.GetPlayerCount(score.NPC_KILLS, me) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_APPREHENSIONS, stats.isCop && !isGhost ? score.GetPlayerCount(score.APPREHENDS, me) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_APPREHENDED, !stats.isCop && !isGhost ? score.GetPlayerCount(score.APPREHENDS, me) : 0 );

	if (weapon && !isGhost)
	{
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_HEADSHOTS, stats.isCop ? weapon->GetStat(weapon->HEAD_SHOTS) : 0 );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_HEADSHOTS, stats.isCop ? 0 : weapon->GetStat(weapon->HEAD_SHOTS) );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTS_FIRED, stats.isCop ? weapon->GetStat(weapon->USED) : 0 );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTS_FIRED, stats.isCop ? 0 : weapon->GetStat(weapon->USED) );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTS_HIT, stats.isCop ? weapon->GetStat(weapon->HITS) : 0 );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTS_HIT, stats.isCop ? 0 : weapon->GetStat(weapon->HITS) );
	}
	else
	{
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_HEADSHOTS, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_HEADSHOTS, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTS_FIRED, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTS_FIRED, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTS_HIT, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTS_HIT, 0);
	}

	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_TIME_PLAYED, stats.isCop && !isGhost ? 
		(!d_quitRound ? g_configmgr.GetProposedParameters()->GetInt( "tl" ) : g_referee.GetElapsedPhaseTime() / 1000) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_TIME_PLAYED, !stats.isCop && !isGhost ?
		(!d_quitRound ? g_configmgr.GetProposedParameters()->GetInt( "tl" ) : g_referee.GetElapsedPhaseTime() / 1000) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_LOOT_SCORES, !stats.isCop && !isGhost ?
		(g_configmgr.TestOptionValue("gameModes", "robbery") ? score.GetPlayerCount(score.OBJECTIVES, me) : 0) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_RAID_SCORES, stats.isCop && !isGhost ?
		(g_configmgr.TestOptionValue("gameModes", "raid") ? score.GetPlayerCount(score.OBJECTIVES, me) : 0 ) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_TAG_SCORES, !stats.isCop && !isGhost ?
		(g_configmgr.TestOptionValue("gameModes", "tag") ? score.GetPlayerCount(score.OBJECTIVES, me) : 0 ) : 0 );

	if (weapon && !isGhost)
	{
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_MELEE, stats.isCop ? weapon->GetStat(weapon->KILLS, weapon->WST_MELEE) : 0 );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_GRENADE, stats.isCop ? weapon->GetStat(weapon->KILLS, weapon->WST_GRENADE) : 0 );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_SPECIAL, stats.isCop ? weapon->GetStat(weapon->KILLS, weapon->WST_SPECIAL) : 0 );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_SUB_MACHINEGUN, stats.isCop ? weapon->GetStat(weapon->KILLS, weapon->WST_SUB_MACHINEGUN) : 0 );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_HEAVY_MACHINEGUN, stats.isCop ? weapon->GetStat(weapon->KILLS, weapon->WST_HEAVY_MACHINEGUN) : 0 );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTGUN, stats.isCop ? weapon->GetStat(weapon->KILLS, weapon->WST_SHOTGUN) : 0 );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_MELEE, stats.isCop ? 0 : weapon->GetStat(weapon->KILLS, weapon->WST_MELEE) );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_GRENADE, stats.isCop ? 0 : weapon->GetStat(weapon->KILLS, weapon->WST_GRENADE) );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_SPECIAL, stats.isCop ? 0 : weapon->GetStat(weapon->KILLS, weapon->WST_SPECIAL) );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_SUB_MACHINEGUN, stats.isCop ? 0 : weapon->GetStat(weapon->KILLS, weapon->WST_SUB_MACHINEGUN) );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_HEAVY_MACHINEGUN, stats.isCop ? 0 : weapon->GetStat(weapon->KILLS, weapon->WST_HEAVY_MACHINEGUN) );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTGUN, stats.isCop ? 0 : weapon->GetStat(weapon->KILLS, weapon->WST_SHOTGUN) );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_MELEE, stats.isCop ? weapon->GetStat(weapon->DEATH_HOLDING, weapon->WST_MELEE) : 0 );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_GRENADE, stats.isCop ? weapon->GetStat(weapon->DEATH_HOLDING, weapon->WST_GRENADE) : 0 );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_SPECIAL, stats.isCop ? weapon->GetStat(weapon->DEATH_HOLDING, weapon->WST_SPECIAL) : 0 );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_SUB_MACHINEGUN, stats.isCop ? weapon->GetStat(weapon->DEATH_HOLDING, weapon->WST_SUB_MACHINEGUN) : 0 );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_HEAVY_MACHINEGUN, stats.isCop ? weapon->GetStat(weapon->DEATH_HOLDING, weapon->WST_HEAVY_MACHINEGUN) : 0 );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_SHOTGUN, stats.isCop ? weapon->GetStat(weapon->DEATH_HOLDING, weapon->WST_SHOTGUN) : 0 );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_MELEE, stats.isCop ? 0 : weapon->GetStat(weapon->DEATH_HOLDING, weapon->WST_MELEE) );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_GRENADE, stats.isCop ? 0 : weapon->GetStat(weapon->DEATH_HOLDING, weapon->WST_GRENADE) );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_SPECIAL, stats.isCop ? 0 : weapon->GetStat(weapon->DEATH_HOLDING, weapon->WST_SPECIAL) );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_SUB_MACHINEGUN, stats.isCop ? 0 : weapon->GetStat(weapon->DEATH_HOLDING, weapon->WST_SUB_MACHINEGUN) );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_HEAVY_MACHINEGUN, stats.isCop ? 0 : weapon->GetStat(weapon->DEATH_HOLDING, weapon->WST_HEAVY_MACHINEGUN) );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_SHOTGUN, stats.isCop ? 0 : weapon->GetStat(weapon->DEATH_HOLDING, weapon->WST_SHOTGUN) );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_MELEE, stats.isCop ? weapon->GetStat(weapon->DEATH_BY, weapon->WST_MELEE) : 0 );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_GRENADE, stats.isCop ? weapon->GetStat(weapon->DEATH_BY, weapon->WST_GRENADE) : 0 );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_SPECIAL, stats.isCop ? weapon->GetStat(weapon->DEATH_BY, weapon->WST_SPECIAL) : 0 );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_SUB_MACHINEGUN, stats.isCop ? weapon->GetStat(weapon->DEATH_BY, weapon->WST_SUB_MACHINEGUN) : 0 );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_HEAVY_MACHINEGUN, stats.isCop ? weapon->GetStat(weapon->DEATH_BY, weapon->WST_HEAVY_MACHINEGUN) : 0 );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_SHOTGUN, stats.isCop ? weapon->GetStat(weapon->DEATH_BY, weapon->WST_SHOTGUN) : 0 );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_MELEE, stats.isCop ? 0 : weapon->GetStat(weapon->DEATH_BY, weapon->WST_MELEE) );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_GRENADE, stats.isCop ? 0 : weapon->GetStat(weapon->DEATH_BY, weapon->WST_GRENADE) );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_SPECIAL, stats.isCop ? 0 : weapon->GetStat(weapon->DEATH_BY, weapon->WST_SPECIAL) );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_SUB_MACHINEGUN, stats.isCop ? 0 : weapon->GetStat(weapon->DEATH_BY, weapon->WST_SUB_MACHINEGUN) );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_HEAVY_MACHINEGUN, stats.isCop ? 0 : weapon->GetStat(weapon->DEATH_BY, weapon->WST_HEAVY_MACHINEGUN) );
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_SHOTGUN, stats.isCop ? 0 : weapon->GetStat(weapon->DEATH_BY, weapon->WST_SHOTGUN) );
	}
	else
	{
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_MELEE, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_GRENADE, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_SPECIAL, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_SUB_MACHINEGUN, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_HEAVY_MACHINEGUN, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_SHOTGUN, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_MELEE, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_GRENADE, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_SPECIAL, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_SUB_MACHINEGUN, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_HEAVY_MACHINEGUN, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_SHOTGUN, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_MELEE, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_GRENADE, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_SPECIAL, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_SUB_MACHINEGUN, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_HEAVY_MACHINEGUN, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_HOLDING_SHOTGUN, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_MELEE, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_GRENADE, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_SPECIAL, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_SUB_MACHINEGUN, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_HEAVY_MACHINEGUN, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_HOLDING_SHOTGUN, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_MELEE, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_GRENADE, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_SPECIAL, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_SUB_MACHINEGUN, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_HEAVY_MACHINEGUN, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_COP_DEATH_BY_SHOTGUN, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_MELEE, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_GRENADE, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_SPECIAL, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_SUB_MACHINEGUN, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_HEAVY_MACHINEGUN, 0);
		stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_GANG_DEATH_BY_SHOTGUN, 0);
	}

	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_TAG_WINS, g_configmgr.TestOptionValue("gameModes", "tag") && !isGhost ? score.GetTeamStatus(us) == score.WON : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_TAG_GAMES_PLAYED, g_configmgr.TestOptionValue("gameModes", "tag") && !isGhost ? 1 : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_TAG_TIME, g_configmgr.TestOptionValue("gameModes", "tag") && !isGhost ?
		(!d_quitRound ? g_configmgr.GetProposedParameters()->GetInt( "tl" ) : g_referee.GetElapsedPhaseTime() / 1000) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_WAR_WINS, g_configmgr.TestOptionValue("gameModes", "war") && !isGhost ? score.GetTeamStatus(us) == score.WON : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_WAR_GAMES_PLAYED, g_configmgr.TestOptionValue("gameModes", "war") && !isGhost ? 1 : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_WAR_TIME, g_configmgr.TestOptionValue("gameModes", "war") && !isGhost ?
		(!d_quitRound ? g_configmgr.GetProposedParameters()->GetInt( "tl" ) : g_referee.GetElapsedPhaseTime() / 1000) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_ROBBERY_WINS, g_configmgr.TestOptionValue("gameModes", "robbery") && !isGhost ? score.GetTeamStatus(us) == score.WON : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_ROBBERY_GAMES_PLAYED, g_configmgr.TestOptionValue("gameModes", "robbery") && !isGhost ? 1 : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_ROBBERY_TIME, g_configmgr.TestOptionValue("gameModes", "robbery") && !isGhost ?
		(!d_quitRound ? g_configmgr.GetProposedParameters()->GetInt( "tl" ) : g_referee.GetElapsedPhaseTime() / 1000) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_RAID_WINS, g_configmgr.TestOptionValue("gameModes", "raid") && !isGhost ? score.GetTeamStatus(us) == score.WON : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_RAID_GAMES_PLAYED, g_configmgr.TestOptionValue("gameModes", "raid") && !isGhost ? 1 : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_RAID_TIME, g_configmgr.TestOptionValue("gameModes", "raid") && !isGhost ?
		(!d_quitRound ? g_configmgr.GetProposedParameters()->GetInt( "tl" ) : g_referee.GetElapsedPhaseTime() / 1000) : 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_MODE5_WINS, 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_MODE5_GAMES_PLAYED, 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_MODE5_TIME, 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_MODE6_WINS, 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_MODE6_GAMES_PLAYED, 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_MODE6_TIME, 0 );
	stats.setStat(OSIPlayerStats::PLAYERSTAT_NA_INCOMPLETES, d_quitRound ? 1 : 0 );

	d_quitRound = false;
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::PrepareStats()
{
	Referee &ref = g_referee;
	CScoringModel &score = ref.GetScoringModel();
	const CTeamingModel &teaming = ref.GetTeamingModel();
	int maxPlayers = g_referee.GetMaxPlayers();

	// We're going to do two things at the same time as we loop through all the players that were in the round.
	//		1)	We're going to add all the pids to the currentStats sturcture, 
	//			so we can retrieve their stats from the server.
	//		2)	We're going to fill in the roundStats structure with all the stats from this round.
	roundStats.playerStatsList.clear();
	currentStats.clear();
	for ( int playerIdx = 0; playerIdx < maxPlayers; playerIdx++ )
	{
		CPlayerAgent* pAgent = ref.GetPlayerAgent( playerIdx );

		// Skip this player if he's not valid or didn't participate
		if ( !pAgent ) continue;
		if ( ref.IsGhost( playerIdx ) ) continue;
	
		// Put this players pid in the currentStats list
		OSIPlayerStats pidStats;
		memcpy( &pidStats.playerID, pAgent->GetAccountId(), sizeof( pidStats.playerID ) );
		pidStats.clearStats();
		currentStats.push_back(pidStats);

		// Populate the roundStats from the round.
		OSIPlayerStats playerStats;
		memcpy( &playerStats.playerID, pAgent->GetAccountId(), sizeof( playerStats.playerID ) );
		playerStats.teamID = ref.GetTeam(playerIdx);
		if ( teaming.GetPlayerProfileVariant(playerIdx) == CPlayerProfile::COP )
			playerStats.isCop = true;
		else
			playerStats.isCop = false;
		PopulateRoundStats(playerStats, pAgent);
		roundStats.playerStatsList.push_back(playerStats);
	}
}

/******************************************************************************/
/******************************************************************************/
float COSILobby::GetPlayerRating(OSIPlayerID id, RankingMode mode)
{
	OSIPlayerStats *playerStats = NULL;
	currentStats.begin();
	float retval = 1600.0f;
	while ( (playerStats = currentStats.getCurrent()) != NULL )
	{
		if ( playerStats->playerID ==  id )
		{
			if ( mode == USER )
				retval = playerStats->getStat(OSIPlayerStats::PLAYERSTAT_USER_RATING);
			else if ( mode == COP )
				retval = playerStats->getStat(OSIPlayerStats::PLAYERSTAT_COP_RATING);
			else if ( mode == GANG )
				retval = playerStats->getStat(OSIPlayerStats::PLAYERSTAT_GANG_RATING);

			
			if ( retval <= 0.0f ) 
				return 1600.0f; // Starting value
			else
				return retval;
		}
		
		currentStats.next();
	}

	return retval;
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::CalcMVPScores()
{	
	roundStats.playerStatsList.begin();
	OSIPlayerStats *stats = 0;
	while ( (stats = roundStats.playerStatsList.getCurrent()) != NULL )
	{
		OSIPlayerStats::StatValueType *s = stats->statsArray;
		if ( stats->isCop )
			stats->MVPScore = 	(s[OSIPlayerStats::PLAYERSTAT_KILLS_AS_COP] - s[OSIPlayerStats::PLAYERSTAT_DEATHS_AS_COP]) + 
								(5.0f*s[OSIPlayerStats::PLAYERSTAT_NA_RAID_SCORES]) + 
								(3.0f*s[OSIPlayerStats::PLAYERSTAT_NA_APPREHENSIONS]) - 
								(1.0f*s[OSIPlayerStats::PLAYERSTAT_NA_COP_NPC_KILLS]) ;
		else
			stats->MVPScore =	(s[OSIPlayerStats::PLAYERSTAT_KILLS_AS_GANGSTA] - s[OSIPlayerStats::PLAYERSTAT_DEATHS_AS_GANGSTA]) + 
								(2.0f*s[OSIPlayerStats::PLAYERSTAT_NA_TAG_SCORES]) + 
								(5.0f*s[OSIPlayerStats::PLAYERSTAT_NA_LOOT_SCORES]) - 
								(5.0f*s[OSIPlayerStats::PLAYERSTAT_NA_APPREHENDED]);

		roundStats.playerStatsList.next();
	}
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::CalcMVPAward(void)
{	
	roundStats.playerStatsList.begin();
	OSIPlayerStats *stats = 0;
	OSIPlayerStats *winner = 0;
	float max = -9999.0f;
	while ( (stats = roundStats.playerStatsList.getCurrent()) != NULL )
	{
		// Default everyone to 0
		stats->setStat(OSIPlayerStats::PLAYERSTAT_COP_MVPS, 0.0f);
		stats->setStat(OSIPlayerStats::PLAYERSTAT_GANG_MVPS, 0.0f);

		if ( stats->MVPScore > max )
		{
			max = stats->MVPScore;
			winner = stats;
		}

		roundStats.playerStatsList.next();
	}

	// Set the winner
	if ( winner )
	{
		if ( winner->isCop )
			winner->setStat(OSIPlayerStats::PLAYERSTAT_COP_MVPS, 1.0f);
		else
			winner->setStat(OSIPlayerStats::PLAYERSTAT_GANG_MVPS, 1.0f);
	}
}


/******************************************************************************/
/******************************************************************************/
float COSILobby::CalcPlayerRatingDelta(OSIPlayerStats& playerStats, float rating, RankingMode mode)
{
	roundStats.playerStatsList.begin();
	OSIPlayerStats *stats = 0;
	float w = 0.0f;
	float wDelta = 0.0f;
	float we = 0.0f;
	while ( (stats = roundStats.playerStatsList.getCurrent()) != NULL )
	{
		if ( stats->teamID != playerStats.teamID )
		{
			// Calculate W
			if ( playerStats.MVPScore > stats->MVPScore )
				w = 1.0f;
			else if ( playerStats.MVPScore == stats->MVPScore )
				w = 0.5f;
			else
				w = 0.0f;

			// Get the opponents Rating
			float oppRating = 0.0f;
			if ( mode == USER )
				oppRating = GetPlayerRating(stats->playerID, USER);
			else
			{
				if ( stats->isCop )
					oppRating = GetPlayerRating(stats->playerID, COP);
				else
					oppRating = GetPlayerRating(stats->playerID, GANG);
			}

			// Calculte the rest
			float exp = (oppRating - (float)rating ) / 400.0f;
			we = 1.0f / (Math::Pow(10.0,exp) + 1.0f);
			wDelta += (w - we);
		}

		roundStats.playerStatsList.next();
	}

	return wDelta;
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::CalculateStats(void)
{	
	float  oldRating = 0.0f;
	float  ratingDelta = 0.0f;
	float wDelta = 0.0f;

	// Do Calculations Here
	CalcMVPScores();
	CalcMVPAward();
	
	int playerCount = 0;
	int countMax = roundStats.playerStatsList.count();
	while ( playerCount < countMax )
	{
		OSIPlayerStats* player = roundStats.playerStatsList.getPlayerAtIndex(playerCount);

		oldRating = GetPlayerRating(player->playerID, USER);
		wDelta = CalcPlayerRatingDelta(*player, oldRating, USER);
		ratingDelta = (float)((16.0f + countMax ) * wDelta);
		
		// JM - I hate to do this like this but time is short.
		player->setStat(OSIPlayerStats::PLAYERSTAT_USER_RATING, ratingDelta);

		if ( player->isCop )
		{
			oldRating = GetPlayerRating(player->playerID, COP);
			wDelta = CalcPlayerRatingDelta(*player, oldRating, COP);
			ratingDelta = (float)((16.0f + countMax ) * wDelta);
			player->setStat(OSIPlayerStats::PLAYERSTAT_COP_RATING, ratingDelta);
		}
		else
		{
			oldRating = GetPlayerRating(player->playerID, GANG);
			wDelta = CalcPlayerRatingDelta(*player, oldRating, GANG);
			ratingDelta = (float)((16.0f + countMax ) * wDelta);
			player->setStat(OSIPlayerStats::PLAYERSTAT_GANG_RATING, ratingDelta);
		}

		++playerCount;
	}
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::QuitRound(void)
{
	// This just hammers the isRanked flag of the match to false in order to 
	// prevent stats from being reported.
	// We did it this way because we know all of the code paths for EndRound
	// will work as desired in this case.  
	// If there are issues arising from this hack then COSILobby will need
	// to remember the state of the flag and reset it to the previous value
	// after EndRound processing has completed.
	if( OSIMatch* match = l_osi.getCurrentMatch() )
	{
		OSI::getSingletonPtr()->setIsHost( false );
		match->options.isRanked = false;
	}

	// Tell the round stats that we're bailing out early.
	d_quitRound = true;
	l_osi.activePlayerWantingToQuit();

	EndRound();
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::EndRound(void)
{
	if ( !IsSafeToMakeAnOSIRequest() )
	{
		//g_messageDispatcher.SendMessageToAll("RoundEnded", NULL, INVALID_OBJECT_HANDLE);		// success message
		g_messageDispatcher.SendMessageToLocalHost("Quit", NULL, INVALID_OBJECT_HANDLE);				// failure message
		g_debugOutput.TSPrint(g_debugOutput.Category("LBY"), "Unable to request stats list.");
		return;
	}

	if( l_osi.getCurrentMatch() && l_osi.getCurrentMatch()->options.isRanked )
	{
		PrepareStats();
		l_osi.requestStatsPlayerList(currentStats);
	}

	d_requestType = GET_STATS;
	g_debugOutput.TSPrint(g_debugOutput.Category("LBY"), "Request stats player list");
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::ReportStatsForEndRound(void)
{
	if ( !IsSafeToMakeAnOSIRequest() )
	{
		g_debugOutput.TSPrint(g_debugOutput.Category("LBY"), "Unable to request end round.");
		return;
	}

	if( l_osi.getCurrentMatch() && l_osi.getCurrentMatch()->options.isRanked )
	{
		currentStats.clear();
		currentStats = l_osi.getStatsPlayerList();

		CalculateStats();
	}
	else
	{
		roundStats.playerStatsList.clear();
	}

	l_osi.requestEndRound(roundStats);
	roundStats.playerStatsList.clear();
	d_requestType = END_ROUND;
	g_debugOutput.TSPrint(g_debugOutput.Category("LBY"), "Request end round");
}

/******************************************************************************/
/******************************************************************************/
void COSILobby::FilterText(const char* text)
{
	if ( !IsSafeToMakeAnOSIRequest() )
	{
		//g_messageDispatcher.SendMessageToLocalHost( "TextFiltered", (void*)false, INVALID_OBJECT_HANDLE );	// success message
		g_messageDispatcher.SendMessageToLocalHost( "TextFiltered", (void*)true, INVALID_OBJECT_HANDLE );	// failure message
		g_debugOutput.TSPrint(g_debugOutput.Category("LBY"), "Unable to request filter text.");
		return;
	}

	const bool request = l_osi.requestFilterTextForProfanity( text );
	d_requestType = FILTER_TEXT;
	g_debugOutput.TSPrint(g_debugOutput.Category("LBY"), "Request filter text");
}


/******************************************************************************/
/******************************************************************************/
int32 COSILobby::GetLocalPlayerRating(void)
{
	return(l_osi.getActivePlayer()->statRating);
}

/******************************************************************************/
/******************************************************************************/
int32 COSILobby::GetLocalPlayerRank(void)
{
	return(l_osi.getActivePlayer()->statRanking);
}


/******************************************************************************/
/******************************************************************************/
bool COSILobby::PopulateMatchOptions(OSIMatchOptions &o_options)
{
	// i hate to do this with a bunch of copy-paste function calls rather than some tables, 
	// but this is the fastest-to-implement and simplest/most-straightforward thing that will work

	if( ValidSingleton( CSession ) )
	{
		const char* URL = g_session.GetSessionURL();
		const char* name = g_sessionSpec.GetName();
		if( URL) { OSIstrncpy( o_options.hostURL, g_session.GetSessionURL(), OSI_STRING_LENGTH_LONG ); }
		if( name ) { OSIstrncpy( o_options.description, g_sessionSpec.GetName(), OSI_STRING_LENGTH_LONG ); }
	}

//	_setOSIMatchOptionFromConfigManager( o_options, OSIMatchOptions::MATCHOPTIONFLAG_LEVELNAME,			o_options.levelName,			"gameMap" ); special
	_setOSIMatchOptionFromConfigManager( o_options, OSIMatchOptions::MATCHOPTIONFLAG_LEVELINDEX,		o_options.levelIndex,			"gameMap" );
//	_setOSIMatchOptionFromConfigManager( o_options, OSIMatchOptions::MATCHOPTIONFLAG_GAMETYPE,			o_options.gameTypeIndex,		"gameModes" ); special
	_setOSIMatchOptionFromConfigManager( o_options, OSIMatchOptions::MATCHOPTIONFLAG_TIMELIMIT,			o_options.timeLimit,			"timeLimit" );
	_setOSIMatchOptionFromConfigManager( o_options, OSIMatchOptions::MATCHOPTIONFLAG_SCORELIMIT,		o_options.scoreLimit,			"scoreLimit" );
	_setOSIMatchOptionFromConfigManager( o_options, OSIMatchOptions::MATCHOPTIONFLAG_TAGLIMIT,			o_options.tagLimit,				"tagLimit" );
	_setOSIMatchOptionFromConfigManager( o_options, OSIMatchOptions::MATCHOPTIONFLAG_BESTOFSERIES,		o_options.bestOfSeries,			"bestOfSeries" );
	_setOSIMatchOptionFromConfigManager( o_options, OSIMatchOptions::MATCHOPTIONFLAG_MAXPLAYERS,		o_options.maxPlayers,			"maxPlayers" );
//	_setOSIMatchOptionFromConfigManager( o_options, OSIMatchOptions::MATCHOPTIONFLAG_RESERVEDPLAYERS,	o_options.reservedPlayers,		"reservedSlots" );
//	_setOSIMatchOptionFromConfigManager( o_options, OSIMatchOptions::MATCHOPTIONFLAG_ISCUSTOMQUERY,		o_options.isCustomQuery,		"" ); not related to configmgr
	_setOSIMatchOptionFromConfigManager( o_options, OSIMatchOptions::MATCHOPTIONFLAG_ISRANKED,			o_options.isRanked,				"rankedMatch" );
	_setOSIMatchOptionFromConfigManager( o_options, OSIMatchOptions::MATCHOPTIONFLAG_PLAYERSRESPAWN,	o_options.playersRespawn,		"allowRespawn" );
	_setOSIMatchOptionFromConfigManager( o_options, OSIMatchOptions::MATCHOPTIONFLAG_FRIENDLYFIRE,		o_options.friendlyFire,			"friendlyFire" );
	_setOSIMatchOptionFromConfigManager( o_options, OSIMatchOptions::MATCHOPTIONFLAG_APPREHENSION,		o_options.apprehension,			"apprehension" );
	_setOSIMatchOptionFromConfigManager( o_options, OSIMatchOptions::MATCHOPTIONFLAG_ARCADEDAMAGE,		o_options.arcadeDamage,			"arcadeDamage" );
	_setOSIMatchOptionFromConfigManager( o_options, OSIMatchOptions::MATCHOPTIONFLAG_SELFDAMAGE,		o_options.selfDamage,			"selfDamage" );
	_setOSIMatchOptionFromConfigManager( o_options, OSIMatchOptions::MATCHOPTIONFLAG_ALTERNATINGTEAMS,	o_options.alternatingTeams,		"alternatingTeams" );

	// special case: the level name value string comes back as a string that is an index (e.g. "14") instead of the actual level name
	o_options.levelName[0] = '\0';
	const int levelIndex = g_configmgr.GetOptionIndexByName( "gameMap" );
	ASSERT( levelIndex >= 0 );
	if( levelIndex >= 0 )
	{
		const char* const levelName = g_configmgr.GetDisplayValue( levelIndex );
		OSIstrcpy( o_options.levelName, levelName );
	}
	ASSERT( o_options.levelName[0] != '\0' );
	
	// special case: the game mode is a string (for some reason) in the configmanager instead of an index,
	// but OSI needs it to be an integer because Live doesn't support searching and filtering by strings.
	o_options.gameTypeIndex = -1;
	const int modeIndex = g_configmgr.GetOptionIndexByName( "gameModes" );
	ASSERT( modeIndex >= 0 );
	if( modeIndex >= 0 )
	{
		const char* const modeString = g_configmgr.GetValue( modeIndex );

		// Here I'm using the lookup table from COnlineSessionSearch.  I know, it's kinda crappy.
		// The nice thing about this is that it helps guarantee that the searching and advertising code will be in sync.
		ASSERT( COnlineSessionSearch::GameTypeIndexToStringLUTSize == sizeof( COnlineSessionSearch::GameTypeIndexToStringLUT ) / sizeof( COnlineSessionSearch::GameTypeIndexToStringLUT[0] ) ); // paranoia - if this fails, update _gameTypeIndexToStringLUT and this function
		for( int i = 0; i < COnlineSessionSearch::GameTypeIndexToStringLUTSize; i++ ) 
		{
			if( !stricmp( COnlineSessionSearch::GameTypeIndexToStringLUT[ i ], modeString ) )
			{
				o_options.gameTypeIndex = i;
				break;
			}
		}
	}
	ASSERT( -1 != o_options.gameTypeIndex ); // set to -1 before this block


	// Special case for setting the isPrivate flag.  It's based on whether or not there are any reserved slots.
	const int idx = g_configmgr.GetOptionIndexByName( "reservedSlots");
	if( idx >= 0 )
	{
		const char* const configValue = g_configmgr.GetValue( idx );
		if ( atoi( configValue ) == 0 )
			o_options.isPrivate = true;
		else 
			o_options.isPrivate = false;

		o_options.removeWildcardFlag( OSIMatchOptions::MATCHOPTIONFLAG_ISPRIVATE );
	}


	// force wildcard flags for things that aren't set through the config manager
	o_options.addWildcardFlag( OSIMatchOptions::MATCHOPTIONFLAG_BESTOFSERIES );
	o_options.addWildcardFlag( OSIMatchOptions::MATCHOPTIONFLAG_TOTALCONNECTEDPLAYERS );
	o_options.addWildcardFlag( OSIMatchOptions::MATCHOPTIONFLAG_ISCUSTOMQUERY );
	if ( o_options.totalConnectedPlayers == 0 )
	{
		o_options.totalConnectedPlayers = 1 ;
	}
	return true;
}

/******************************************************************************/
// GLUE function for pasting OSIMatchOptions to the ConfigMgr
/******************************************************************************/
void COSILobby::_setOSIMatchOptionFromConfigManager( 
	OSIMatchOptions& options, 
	const OSIMatchOptions::MatchOptionsWildcardFlag flag, 
	int& value, 
	const char* const name )
{
	ASSERT( name );

	value = false;
	options.addWildcardFlag( flag );
	ConfigMgr& configMgr = g_configmgr;
	const int index = configMgr.GetOptionIndexByName( name );
	if( index >= 0 )
	{
		const char* const configValue = configMgr.GetValue( index );
		if ( configValue )
		{
			value = atoi( configValue );
			options.removeWildcardFlag( flag );
		}
	}
}

/******************************************************************************/
// GLUE function for pasting OSIMatchOptions to the ConfigMgr
/******************************************************************************/
void COSILobby::_setOSIMatchOptionFromConfigManager( 
	OSIMatchOptions& options, 
	const OSIMatchOptions::MatchOptionsWildcardFlag flag, 
	bool& value, 
	const char* const name )
{
	ASSERT( name );

	value = false;
	options.addWildcardFlag( flag );
	ConfigMgr& configMgr = g_configmgr;
	const int index = configMgr.GetOptionIndexByName( name );
	if( index >= 0 )
	{
		const char* const configValue = configMgr.GetValue( index );
		if ( configValue )
		{
			if( atoi( configValue ) )	{ value = true; }
			else						{ value = false; }
			options.removeWildcardFlag( flag );
		}
	}
}

/******************************************************************************/
// GLUE function for pasting OSIMatchOptions to the ConfigMgr
/******************************************************************************/
void COSILobby::_setOSIMatchOptionFromConfigManager( 
	OSIMatchOptions& options, 
	const OSIMatchOptions::MatchOptionsWildcardFlag flag, 
	OSIchar value[ OSI_STRING_LENGTH_LONG ], 
	const char* const name )
{
	ASSERT( name );

	value[0] = '\0';
	options.addWildcardFlag( flag );

	ConfigMgr& configMgr = g_configmgr;
	const int index = configMgr.GetOptionIndexByName( name );
	if( index >= 0 )
	{
		const char* const configValue = configMgr.GetValue( index );
		if ( configValue )
		{
			OSIstrncpy( value, configValue, sizeof( value ) / sizeof( value[0] ) );
			options.removeWildcardFlag( flag );
		}
	}
}


#endif