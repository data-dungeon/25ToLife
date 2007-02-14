/*
**
**  File:   Players.h
**  Date:   March 25, 2004
**  By:     Bryant Collard
**  Desc:   Encapsulate players from simloop.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/main/Players.h $
**  $Revision: #28 $
**  $DateTime: 2005/07/19 21:32:56 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef PLAYER_H
#define PLAYER_H

#include "main/TTLConstants.h"
#include "Math/Vector.h"
#include "EngineHelper/Singleton.h"

class CCompActor;
class CCombatant;
class ParameterSet;
class CLoopScenes;
class AvatarControlMapper;
class ControllerInputDriver;

struct ControlState
{
	CActorComponent *kineDriver;
	CActorComponent *viewDriver;
	bool mapperActive;
};

class CPlayers
{
  public:
	CPlayers();

	// Manage message handlers
	void Initialize(void);
	void Uninitialize(void);

	// Load and save start positions from/to .var file.
	void GetPlayerStartPos(const char* i_varFile);
	void SavePlayerStartPos(void);
	void SetPlayerStartPos(int i_player, Vector3CRef i_pos, float i_heading);

	// Create some players.
	void CreateFromStrings( CLoopScenes* i_scene, uint8 i_iParticipantID, const char *szModel, const char *szWeapon, bool bKeepLocal = false );
	void Destroy(void);

	// Lock into orbit camera.
	void SetOrbit(bool i_enter);
	void SetOrbit(int i_player, bool i_enter);

	// Attach a hud.
	void AttachHud(int i_player);

	// Get at the combatant's controllers.
	void ActivateControls(bool i_activate);
	ControlState GetCurrentControlState( int nPlayerID );
	void RestoreControlState( int nPlayerID, ControlState &control );

	// Clear controller vibration.
	void ClearVibration(void);
	void ClearVibration(int i_player);

	// Reset players.
	bool RespawnPlayer(int nPlayerID, bool i_force = true, bool i_rearm = false, bool i_random = false, bool i_ignoreOccupancy = false);
	void RespawnPlayer(int nPlayerID, Vector3CRef position, float heading, bool i_rearm = false);
	bool RespawnParticipant(int i_participantID, bool i_force = true, bool i_rearm = false, bool i_random = false, bool i_ignoreOccupancy = false);
	void RespawnParticipant(int i_participantID, Vector3CRef position, float heading, bool i_rearm = false);
	void ResetPlayerPositions(void);

	// Get at players.
	int NumPlayers(void) {return(players);}
	CCompActor* GetPlayer(int i);
	AvatarControlMapper* GetMapper(int i_player, bool i_needsFocus = false);
	ControllerInputDriver* GetDevice(int i_player, bool i_needsFocus = false);
	uint8 GetParticipantId(int i_player);
	int GetPlayerNumber(uint8 i_participantID);
	CCompActor* GetParticipant(int i_participantID);

	void CreatePlayer(int i_index, int i_character, uint8 i_iParticipantID = INVALID_PARTICIPANT);
	void CreatePlayer( int i_index, const char *szInit, const char *szWeaponLoadOut = NULL, uint8 i_iParticipantID = INVALID_PARTICIPANT );

	// Controller options have changed
	void ControllerSetupChanged();

  private:
   void CompleteCreatePlayer( int i_index, CCombatant *pPlayer, uint8 i_iParticipantID );
	void CreatePlayerSceneDetails( int i_iPlayerIndex );

	// Stuff for the player
	int			players;
	CCombatant	*player[MAX_PLAYERS];

	// Starting positions and headings
	Vector3	playerStartPos[MAX_PLAYERS];
	float		playerStartHeading[MAX_PLAYERS];

	// Get at scenes
	CLoopScenes* d_scene;

	// Reset everything
	void Reset();

	// Message handling
	CMessageOwner d_messageOwner;
	REF_SEND_MESSAGE_HANDLER(CManDown, CPlayers) d_manDownHandler;
};

DefineSingleton(CPlayers)
#define g_players GetSingleton(CPlayers)

#endif // PLAYER_H
