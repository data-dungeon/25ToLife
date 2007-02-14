/*
**
**  File:   Respawn.cpp
**  Date:   June 30, 2005
**  By:     Bryant Collard
**  Desc:   Manage actor respawn.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/components/Respawn.cpp $
**  $Revision: #9 $
**  $DateTime: 2005/08/29 19:13:07 $
**    $Author: Rob $
**
*/

// Precompiled header
#include "TTLPCH.h"
#include "main/TTL.h"

// My header
#include "components/Respawn.h"

// GameLib headers
#include "Components/ComponentCreator.h"

// TTL headers
#include "environ/ConfigMgr.h"
#include "environ/PlayerSpawnMgr.h"

STANDARD_COMPONENT_CREATOR(CRespawn, Respawn, COMPONENT_NEUTRAL_PRIORITY);

/******************************************************************************/
/******************************************************************************/
CRespawn::CRespawn(CCompActor &i_actor) : CActorComponent(i_actor)
{
	d_state = DISABLED;
	d_faded = false;
	d_minSpawnTime = 0;
	d_spawnWavePeriod = 1;
	d_fadeTime = -1;
	d_respawnTime = 0;
	d_apprehensionMult = 0.0f;
}

/******************************************************************************/
/******************************************************************************/
void CRespawn::Initialize(void)
{
	const char* value = g_configmgr.GetValue("teamTicketCount");
	d_state = ((value != NULL) && (atoi(value) > 0)) ? INACTIVE : DISABLED;

	value = g_configmgr.GetValue("spawnMessage");
	d_showMessage = (value != NULL) ? (atoi(value) != 0) : false;

	value = g_configmgr.GetValue("spawnFadeTime");
	d_fadeTime = (value != NULL) ? atoi(value) : -1;

	value = g_configmgr.GetValue("minSpawnTime");
	d_minSpawnTime = (value != NULL) ? atoi(value) : 0;
	d_minSpawnTime = Math::Max(d_minSpawnTime, Math::Max(d_fadeTime, 0));

	value = g_configmgr.GetValue("spawnWavePeriod");
	d_spawnWavePeriod = (value != NULL) ? atoi(value) : 1;
	d_spawnWavePeriod = Math::Max(d_spawnWavePeriod, 1);

	value = g_configmgr.GetValue("apprehensionMult");
	d_apprehensionMult = (value != NULL) ? (float)atoi(value) : 0.0f;
	if (d_apprehensionMult < 0)
		d_apprehensionMult = 0.0f;

	d_respawnTime = 0;

	d_manDownHandler.Initialize("ManDown",
			MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_blockRespawnHandler.Initialize("BlockRespawn",
			MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_unblockRespawnHandler.Initialize("UnblockRespawn",
			MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
}

/******************************************************************************/
/******************************************************************************/
void CRespawn::BeginFrame(void)
{
	if ((d_state != DISABLED) && (d_state != INACTIVE))
	{
		CParticipantIF* participantIF =
				(CParticipantIF*)actor.GetInterface(CParticipantIF::GetID());
		if ((participantIF != NULL) && !participantIF->GetAlive())
		{
			if ((d_state != WAITING) || (g_PlayerSpawnMgr.HowManySpawnPoints(
					participantIF->GetParticipantId()) > 0) ||
					(g_PlayerSpawnMgr.TotalSpawnPoints() == 0))
			{
				int timeLeft = d_respawnTime - g_session.GetSessionTimeMS();
				if (timeLeft <= 0)
				{
					// Timer just ran out. Let everyone know.
					if (d_state == WAITING)
					{
						d_state = READY;
						g_messageDispatcher.SendMessageToAll("StartingRespawn", NULL,
								actor.GetHandle());
					}

					// Respawn if not blocked. Note that the only way to block a
					// respawn is to do so in response to the "StartingRespawn"
					// message.
					if (d_state != BLOCKED)
					{
						if (g_players.RespawnParticipant(
								participantIF->GetParticipantId(), false, true, false, g_ttl->IsSinglePlayer()))
						{
							g_messageDispatcher.SendMessageToAll("FinishedRespawn",
									NULL, actor.GetHandle());
							actor.StartAnimation( "IdleNoInterp", true, true );
						}
					}
				}
				else
				{
					if ((timeLeft <= d_fadeTime) && !d_faded)
					{
						RenderMgr__FadeOut(FADE_TO_BLACK_FAST, false, 0);
						d_faded = true;
					}
					if (d_showMessage)
					{
						char text[80];
						sprintf(text, "Respawning in %d seconds",
								((timeLeft - 1) / 1000) + 1);
						GameHUD_ShowMessage msg = {text, 0.001f * (float)timeLeft,
								false, true};
						GameHUD::SendMessage(GAMEHUD_SHOWMESSAGE, &msg);
					}
				}
			}
			else
			{
				if (d_faded)
				{
					RenderMgr__FadeIn(FADE_TO_BLACK_FAST, false, 0);
					d_faded = false;
				}
				if (d_respawnTime < g_session.GetSessionTimeMS())
					d_respawnTime += d_spawnWavePeriod;
				if (d_showMessage)
				{
					char text[80];
					int timeLeft = d_respawnTime - g_session.GetSessionTimeMS();
					sprintf(text, "No respawn points available, searching for point "
							"in %d seconds", ((timeLeft - 1) / 1000) + 1);
					GameHUD_ShowMessage msg = {text, 0.001f * (float)timeLeft,
							false, true};
					GameHUD::SendMessage(GAMEHUD_SHOWMESSAGE, &msg);
				}
			}
		}
		else
		{
			d_state = INACTIVE;
			if (d_faded)
			{
				RenderMgr__FadeIn(FADE_TO_BLACK_FAST, false, 0);
				d_faded = false;
			}
		}
	}
}

/******************************************************************************/
/******************************************************************************/
void CRespawn::CManDown::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	if ((d_ref->d_state == INACTIVE) && (i_sender == d_ref->actor.GetHandle()))
	{
		CParticipantIF* participantIF =
				(CParticipantIF*)d_ref->actor.GetInterface(CParticipantIF::GetID());
		if ((participantIF != NULL) && participantIF->GetSpawnTicket())
		{
			d_ref->d_respawnTime =
					g_session.GetSessionTimeMS() + d_ref->d_minSpawnTime;
			Property* prop = d_ref->actor.QueryProperty("ApprehendTime");
			if (prop != NULL)
			{
				d_ref->d_respawnTime +=
						(int)(1000.0f * d_ref->d_apprehensionMult * prop->GetFloat());
			}
			prop = d_ref->actor.QueryProperty("KilledHostage");
			if( prop )
			{
				if( prop->GetBool() )
				{
					int participantID = (g_referee.GetLocalPlayerAgent())->Id();
					if(g_referee.GetTeamingModel().IsPlayerBlue(participantID))
					{
						// 6 second penalty if a cop kills a hostage
						d_ref->d_respawnTime += 6000;
					}
					prop->Set( false );
				}
			}

			if (d_ref->d_respawnTime > 0)
			{
				d_ref->d_respawnTime = d_ref->d_spawnWavePeriod *
						(((d_ref->d_respawnTime - 1) / d_ref->d_spawnWavePeriod) + 1);
			}
			else
			{
				d_ref->d_respawnTime = -d_ref->d_spawnWavePeriod *
						(-d_ref->d_respawnTime / d_ref->d_spawnWavePeriod);
			}
			d_ref->d_state = WAITING;
		}
		else if (d_ref->d_showMessage)
		{
			char text[80];
			sprintf(text, "Spawn tickets consumed, player eliminated");
			GameHUD_ShowMessage msg = {text, 3.0f, false, true};
			GameHUD::SendMessage(GAMEHUD_SHOWMESSAGE, &msg);
		}
	}
}

/******************************************************************************/
/******************************************************************************/
void CRespawn::CBlockRespawn::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	if (d_ref->d_state == READY)
		d_ref->d_state = BLOCKED;
}

/******************************************************************************/
/******************************************************************************/
void CRespawn::CUnblockRespawn::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	if (d_ref->d_state == BLOCKED)
		d_ref->d_state = READY;
}

