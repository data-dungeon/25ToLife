/*
**
**  File:   Players.cpp
**  Date:   March 25, 2004
**  By:     Bryant Collard
**  Desc:   Encapsulate players from simloop.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/main/Players.cpp $
**  $Revision: #95 $
**  $DateTime: 2005/08/06 15:00:27 $
**    $Author: Dwain_Skinner $
**
*/

// Precompiled header
#include "TTLPCH.h"

#include "components/KineControl/BipedKineControl.h"
#include "components/KineControl/ViewDriver.h"
#include "components/Respawn.h"
#include "avatar/pstatMP.h"
#include "avatar/pstatSP.h"
#include "main/TTL.h"
#include "environ/ActorCreator.h"
#include "environ/PlayerSpawnMgr.h"
#include "Loop/LoopScenes.h"
#include "InterfacesTTL/ParticipantIF.h"
#include "main/PlayerDB.h"
#include "main/TTLConstants.h"
#include "weapons/ttlweapinv.h"

#include "camera/pathcam/pathcam.h"
#include "camera/generalcam/generalcam.h"
#include "frontend/savegame.h"

DeclareSingleton(CPlayers);

/***************************************************************************/
// Get player starting pos out of script file
/***************************************************************************/
CPlayers::CPlayers() : d_messageOwner("Players")
{
	Reset();
}

/***************************************************************************/
/***************************************************************************/
void CPlayers::Initialize(void)
{
	g_debugOutput.RegisterCategory("Spawn", "SPN");
	Reset();

	d_manDownHandler.Initialize("ManDown", MESSAGE_HANDLE_IF_MASTER,
			d_messageOwner, this);
}

/***************************************************************************/
/***************************************************************************/
void CPlayers::Uninitialize(void)
{
	d_messageOwner.UnregisterAll();
	Reset();
}

/***************************************************************************/
// Get player starting pos out of script file
/***************************************************************************/
void CPlayers::GetPlayerStartPos(const char* i_varFile)
{
	char *context = "player";
	g_console.DeleteContext(context);
	g_console.BindScript(context, i_varFile);

	char title[15];
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		sprintf(title, "startPos%d", i);
		playerStartPos[i].Clear();
		g_console.CreateVar(context, title, (Vector3Packed*)&playerStartPos[i]);

		sprintf(title, "startRot%d", i);
		playerStartHeading[i] = 0.0f;
		g_console.CreateVar(context, title, playerStartHeading[i], CONVAR_DEGREES);
	}

	g_console.ExecuteContext(context);
}

/***************************************************************************/
/***************************************************************************/
void CPlayers::SavePlayerStartPos(void)
{
	char *context = "player";

	for (int i = 0; i < players; i++)
	{
		playerStartPos[i] = player[i]->GetBodyInWorld();
		Vector3 dir = player[i]->GetBodyToWorld().GetRow2();
		playerStartHeading[i] = Math::ArcTan2Deg(dir.X(), dir.Z());
	}

	g_console.SaveContext(context);
}

/***************************************************************************/
/***************************************************************************/
void CPlayers::SetPlayerStartPos(int i_player, Vector3CRef i_pos,
		float i_heading)
{
	if ((i_player < 0) || (i_player >= MAX_PLAYERS))
		return;

	playerStartPos[i_player] = i_pos;
	playerStartHeading[i_player] = i_heading;
}

/***************************************************************************/
/***************************************************************************/
void CPlayers::CreateFromStrings( CLoopScenes* i_scene, uint8 i_iParticipantID, const char *szModel, const char *szWeapon, bool bKeepLocal )
{
	ASSERT(players == 0);
	players = 1;
	d_scene = i_scene;

	// Create the player
	CreatePlayer(0, szModel, szWeapon, i_iParticipantID);

	CreatePlayerSceneDetails( 0 );

	// Distribute to network - does not persist when this host quits
	if ( !bKeepLocal )
	{
		player[0]->DistributeNetActor(false);
	}
}

/***************************************************************************/
/***************************************************************************/
void CPlayers::CreatePlayerSceneDetails( int i_iPlayerIndex )
{
	// Build a control mapper
	AvatarControlMapper* control = new AvatarControlMapper;
	ASSERT_PTR(control);

	g_controlFocus.Add(control, i_iPlayerIndex);
	control->Initialize();  // Must be added to the control focus list first
	g_controlFocus.Push(control);

	player[i_iPlayerIndex]->AddProperty( "ControlMapper", Property_pointer );
	player[i_iPlayerIndex]->SetProperty( "ControlMapper", control );

	// Make sure we use the profile
	control->SetupFromProfile();

	// Build a camera group
	if ((d_scene != NULL) && (d_scene->Scene(i_iPlayerIndex) != NULL))
	{
		// Create a camera group
		CameraGroup *group = new CameraGroup("Avatar");
		ASSERT_PTR(group);
		d_scene->Scene(i_iPlayerIndex)->AddGroup(group);

		// Orbit camera
		OrbitCamera *orbitCam = new OrbitCamera("Orbit");
		ASSERT_PTR(orbitCam);
		group->AddTail(orbitCam);
		orbitCam->Setup(2.0f, 0.2f, 20.0f);
		orbitCam->SetTarget(player[i_iPlayerIndex]);
		Vector3 head(0.0f, 1.7f, 0.0f);
		orbitCam->SetTargetOffset(head);

		// Death camera
		DeathCamera *deathCam = new DeathCamera("Death");
		ASSERT_PTR(deathCam);
		group->AddTail(deathCam);
		deathCam->SetTarget(player[i_iPlayerIndex]);

		// Shooter camera
		ShooterCamera *shooterCam = new ShooterCamera("Shooter");
		ASSERT_PTR(shooterCam);
		group->AddTail(shooterCam);
		shooterCam->SetFollow(player[i_iPlayerIndex]);

		// ghost camera
		GhostCamera *ghostCam = new GhostCamera("Ghost", *control);
		ASSERT_PTR(ghostCam);
		group->AddTail(ghostCam);
		ghostCam->SetFollow(player[i_iPlayerIndex]);

		// path camera
		PathCamera *pathCam = new PathCamera("Path");
		ASSERT_PTR(pathCam);
		group->AddTail(pathCam);
		//pathCam->SetFollow(player[i_iPlayerIndex]);

		// general camera
		GeneralCamera *generalCam = new GeneralCamera("General");
		ASSERT_PTR(generalCam);
		group->AddTail(generalCam);
//		generalCam->SetFollow(player[i_iPlayerIndex]);

		// Select the shooter camera by default
		group->Select(shooterCam);

		// Put on the stack
		d_scene->Scene(i_iPlayerIndex)->PushGroup(group);
	}
}

/***************************************************************************/
/***************************************************************************/
void CPlayers::Destroy(void)
{
	// clear cached pointer to controller before we destroy it
	if (ValidSingleton(AvatarFX))
		g_avatarFX.ClearController();

	for (int i = 0; i < players; i++)
	{
		if ((d_scene != NULL) && (d_scene->Scene(i) != NULL))
		{
			CameraGroup* group = d_scene->Scene(i)->FindGroup("Avatar");
			CameraGroup* anotherGroup = d_scene->Scene(i)->PopGroup("Avatar");
			ASSERT( group == anotherGroup );
			d_scene->Scene(i)->RemoveGroup(group);
		}
		g_controlFocus.Pop("Avatar", i);
		g_controlFocus.Remove("Avatar", i);
	}

	players = 0;
	d_scene = NULL;
}

/***************************************************************************/
/***************************************************************************/
void CPlayers::SetOrbit(bool i_enter)
{
	for (int i = 0; i < players; i++)
		SetOrbit(i, i_enter);
}

/***************************************************************************/
/***************************************************************************/
void CPlayers::SetOrbit(int i_player, bool i_enter)
{
	if ((i_player >= 0) && (i_player < players) && (d_scene != NULL) &&
			(d_scene->Scene(i_player) != NULL))
	{
		CameraGroup* group = d_scene->Scene(i_player)->FindGroup("Avatar");
		ASSERT_PTR(group);
		bool inOrbit = (strcmp(group->Selected().Name(), "Orbit") == 0);
		if (i_enter)
		{
			if (!inOrbit)
			{
				group->Select("Orbit");
				group->Lock(true);
			}
		}
		else if (inOrbit)
		{
			group->Lock(false);
			group->Select("Shooter");
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void CPlayers::AttachHud(int i_player)
{
	g_hud.SetSubject(player[i_player]);
}

/***************************************************************************/
/***************************************************************************/
CCompActor* CPlayers::GetPlayer(int i)
{
	if ( i >= 0 && i < MAX_PLAYERS )
		return player[ i ];
	return NULL;
}

/***************************************************************************/
/***************************************************************************/
uint8 CPlayers::GetParticipantId(int i_player)
{
	if ((i_player >= 0) && (i_player < players))
	{
		CParticipantIF* participantIF = (CParticipantIF*)
				player[i_player]->GetInterface(CParticipantIF::GetID());
		if (participantIF != NULL)
			return(participantIF->GetParticipantId());
	}
	return(INVALID_PARTICIPANT);
}

/***************************************************************************/
/***************************************************************************/
int CPlayers::GetPlayerNumber(uint8 i_participantID)
{
	for (int i = 0; i < players; ++i)
	{
		CParticipantIF* participantIF = (CParticipantIF*)
			player[i]->GetInterface(CParticipantIF::GetID());
		if ((participantIF != NULL) && (participantIF->GetParticipantId() == i_participantID))
		{
			return(i);
		}
	}
	return(-1);
}

/***************************************************************************/
/***************************************************************************/
CCompActor* CPlayers::GetParticipant(int i_participantID)
{
	for (int i = 0; i < players; i++)
	{
		if (i_participantID == GetParticipantId(i))
			return(player[i]);
	}
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
AvatarControlMapper* CPlayers::GetMapper(int i_player, bool i_needsFocus)
{
	if ((i_player >= 0) && (i_player < players))
	{
		Property* property = player[i_player]->QueryProperty("ControlMapper");
		if (property != NULL)
		{
			AvatarControlMapper *mapper =
					(AvatarControlMapper*)property->GetPointer();
			if ((mapper != NULL) && (!i_needsFocus || mapper->HasFocus()))
				return(mapper);
		}
	}
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
ControllerInputDriver* CPlayers::GetDevice(int i_player, bool i_needsFocus)
{
	AvatarControlMapper* mapper = GetMapper(i_player, i_needsFocus);
	if (mapper != NULL)
		return(mapper->GetDevice());
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
void CPlayers::ActivateControls(bool i_activate)
{
	for (int i = 0; i < players; i++)
	{
		if( i_activate )
		{
			player[i]->ResumeSuspendedComponents();

			AvatarControlMapper *mapper = GetMapper(i, false);
			if (mapper)
				mapper->Enable(true);
		}
		else
		{
			CActorComponent *driver = player[i]->GetActiveComponentInClass( "KinematicDriver" );
			if( driver )
				driver->Suspend();
			CActorComponent *comp = player[i]->GetComponentByName( "ViewDriver" );
			if( comp )
				comp->Suspend();

			AvatarControlMapper *mapper = GetMapper(i, false);
			if (mapper)
				mapper->Enable(false);
		}
	}
}

ControlState CPlayers::GetCurrentControlState( int nPlayerID )
{
	ControlState control;

	control.kineDriver = NULL;
	control.viewDriver = NULL;
	control.mapperActive = false;

	if( nPlayerID < players )
	{
		control.kineDriver = player[nPlayerID]->GetActiveComponentInClass( "KinematicDriver" );
		control.viewDriver = player[nPlayerID]->GetComponentByName( "ViewDriver" );
		if( !control.viewDriver->IsActive() )
			control.viewDriver = NULL;

		AvatarControlMapper *mapper = GetMapper(nPlayerID, false);
		if (mapper && mapper->Enabled())
			control.mapperActive = true;
	}
	
	return control;
}

void CPlayers::RestoreControlState( int nPlayerID, ControlState &control )
{
	if( nPlayerID < players )
	{
		if( control.kineDriver )
		{
			if( control.kineDriver->Suspended() )
				control.kineDriver->Resume();
			else
				control.kineDriver->Activate();
		}
		if( control.viewDriver )
		{
			if( control.viewDriver->Suspended() )
				control.viewDriver->Resume();
			else
				control.viewDriver->Activate();
		}
		if( control.mapperActive )
		{
			AvatarControlMapper *mapper = GetMapper(nPlayerID, false);
			if (mapper)
				mapper->Enable( true );
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void CPlayers::ClearVibration(void)
{
	for (int i = 0; i < players; i++)
		ClearVibration(i);
}

/***************************************************************************/
/***************************************************************************/
void CPlayers::ClearVibration(int i_player)
{
	ControllerInputDriver* device = GetDevice(i_player, true);
	if (device != NULL)
		device->ClearVibration(true);
}

/***************************************************************************/
// Create a player
/***************************************************************************/
void CPlayers::CreatePlayer(int i_index, const char *szInit, const char *szWeaponLoadOut, uint8 i_iParticipantID )
{
	ASSERT( i_index >= 0 && i_index < MAX_PLAYERS );

	// Create the avatar
	CCombatant *pPlayer = new CCombatant;
	ASSERT( pPlayer );

	// set our name
	const char* accountName = g_playerProfile.GetAccountName();
	const char *playerName = (*accountName == '\0' ? "Debug" : accountName);
	pPlayer->GetMessageOwner().SetOwnerName("LocalAvatar", playerName);
	pPlayer->Parm().Set( "name", playerName );
	pPlayer->SetName( playerName );

	// set up character art
	pPlayer->Parm().SetFromString( szInit );
	pPlayer->Parm().SetFromString( szWeaponLoadOut );

	// init shtuff
	ActorInitializer::InitGeometry( pPlayer, ActorCreator::InitGameSpecificGeometry );

	pPlayer->Parm().Set( "projectedShadow", "true" );

	player[i_index] = pPlayer;

	CompleteCreatePlayer( i_index, pPlayer, i_iParticipantID );
}

/***************************************************************************/
// Create a player
/***************************************************************************/
void CPlayers::CreatePlayer(int i_index, int i_character, uint8 i_iParticipantID)
{
	ASSERT( i_index >= 0 && i_index < MAX_PLAYERS );

	// set up character art
	if ( i_character < 0 || i_character >= PlayerDB::NumCharacters() )
		i_character = 0;

	CreatePlayer( i_index, PlayerDB::CharacterInitString( i_character ) );
}

/***************************************************************************/
// Controller options have changed
/***************************************************************************/
void CPlayers::ControllerSetupChanged()
{
	for (int p = 0; p < players; p++)
	{
		if (player[p])
		{	
			Property *property = player[p]->GetProperty( "ControlMapper");
			if (property)
			{
				AvatarControlMapper *mapper = (AvatarControlMapper *)property->GetPointer();
				if (mapper)
					mapper->SetupFromProfile();
			}
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void CPlayers::CompleteCreatePlayer( int i_index, CCombatant *pPlayer, uint8 i_iParticipantID )
{
#ifdef ENABLE_RECORD
	pPlayer->CreateRecordBuffer(1000);
#endif

	pPlayer->AttachComponent( "Biped" );
	pPlayer->AttachComponent( "GroundFollower" );
	pPlayer->AttachComponent( "BipedPhysicalController" );
	pPlayer->AttachComponent( "PlayerStatsGrabber" );
	if (g_ttl->IsMultiPlayer())
		pPlayer->AttachComponent( "PlayerStatsMP" );
	else
		pPlayer->AttachComponent( "PlayerStatsSP" );
	pPlayer->AttachComponent( "InteractionMonitor" );
	pPlayer->AttachComponent( "Teleport" );
	pPlayer->AttachComponent( "InventoryList" );
	pPlayer->AttachComponent( "Rider" );
	pPlayer->AttachComponent( "Respawn" );
	pPlayer->InitializeComponents();

 	pPlayer->Initialize();

	pPlayer->SetProperty( "updateLevel", 1 );

 	pPlayer->Category() |= PROCESS_LAST_CATEGORY | ROAMER_CATEGORY | NO_SUPPORT_CATEGORY | PLAYER_CATEGORY;
 	pPlayer->SetPositiveInclude(ALL_CATEGORIES);

	// Place me
	pPlayer->Place(playerStartPos[i_index], playerStartHeading[i_index]);

	// Delete when no longer in a cast
	pPlayer->SetDeleteWhenNoCast(true);

	// Add to the stage
	g_stage.AddProtagonist( (CProtagonist *)pPlayer );

	// Set the participant id.
	CParticipantIF* participantIF =
		(CParticipantIF*)pPlayer->GetInterface(CParticipantIF::GetID());
	if (participantIF != NULL)
		participantIF->MakeAvatar(i_iParticipantID);

	// Register with the referee
	if (ValidSingleton(Referee))
		g_referee.RegisterParticipant(pPlayer);

	ActivateControls(false);  //turn off controls....spawning in will reactive them 
}

/***************************************************************************/
//	respawn a player after death or warp
/***************************************************************************/
bool CPlayers::RespawnPlayer(int nPlayerID, bool i_force, bool i_rearm,
		bool i_random, bool i_ignoreOccupancy)
{
	Vector3 location;
	float heading;
	PlayerSpawnMgr::ESpawnResult result;
	if (i_random)
		result = g_PlayerSpawnMgr.GetRandomSpawnPoint(GetParticipantId(nPlayerID),
				location, heading, i_ignoreOccupancy);
	else
		result = g_PlayerSpawnMgr.GetBestSpawnPoint(GetParticipantId(nPlayerID),
				location, heading, i_ignoreOccupancy);

	uint8 category = g_debugOutput.Category("SPN");

	switch(result)
	{
	 case PlayerSpawnMgr::NONE_AVAILABLE:
#ifndef CONSUMER_BUILD
		 g_debugOutput.TSPrint( category, "%f, %f, %f, NONE_AVAILABLE", (float)playerStartPos[ nPlayerID ].X(), (float)playerStartPos[ nPlayerID ].Y(), (float)playerStartPos[ nPlayerID ].Z() );
#endif
		RespawnPlayer(nPlayerID, playerStartPos[nPlayerID],
				playerStartHeading[nPlayerID], i_rearm);
		break;

	 case PlayerSpawnMgr::ALL_OCCUPIED:
#ifndef CONSUMER_BUILD
		 g_debugOutput.TSPrint( category, "%f, %f, %f, ALL_OCCUPIED", (float)playerStartPos[ nPlayerID ].X(), (float)playerStartPos[ nPlayerID ].Y(), (float)playerStartPos[ nPlayerID ].Z() );
#endif
		if (!i_force)
			return(false);
		RespawnPlayer(nPlayerID, playerStartPos[nPlayerID],
				playerStartHeading[nPlayerID], i_rearm);
		break;
			
	 case PlayerSpawnMgr::FOUND_VACANCY:
#ifndef CONSUMER_BUILD
		 g_debugOutput.TSPrint( category, "%f, %f, %f, FOUND_VACANCY", (float)location.X(), (float)location.Y(), (float)location.Z() );
#endif
		RespawnPlayer(nPlayerID, location, heading, i_rearm);
		break;
	}
	return(true);
}

/***************************************************************************/
/***************************************************************************/
void CPlayers::RespawnPlayer(int nPlayerID, Vector3CRef position, float heading,
		bool i_rearm)
{
	g_messageDispatcher.SendMessageToAll("PlayerSpawning", NULL,
			INVALID_OBJECT_HANDLE);

	player[nPlayerID]->Respawn();

	// reset the weapons for this spawning player
	g_messageDispatcher.SendMessageToActor("ResetWeapons", NULL,
		player[nPlayerID]->GetHandle(), player[nPlayerID]->GetHandle());

	TTLWeaponInventory *weaponInventory =
		(TTLWeaponInventory*)player[nPlayerID]->GetComponentByName("WeaponInventory");
	if (i_rearm && weaponInventory)
		weaponInventory->ReArm();

	player[nPlayerID]->Place(position, heading);

	// Reset player stats
	PlayerStats *stats = (PlayerStats*)player[nPlayerID]->GetInterface(PlayerStats::GetID());
	if( stats )
		stats->Reset();

	// BET - activate the interaction monitor for while we're alive.
	InteractionMonitor* interact =
			(InteractionMonitor*)player[nPlayerID]->GetComponentByName( "InteractionMonitor" );
	if (interact)
		interact->Activate();

	// active the default controller, driver, and viewdriver.
	CActorComponent *controller = player[nPlayerID]->GetDefaultComponentInClass( "KinematicController" );
	if( controller )
		controller->Activate();
	CActorComponent *driver = player[nPlayerID]->GetDefaultComponentInClass( "KinematicDriver" );
	if( driver )
		driver->Activate();
	CActorComponent *comp = player[nPlayerID]->GetComponentByName( "ViewDriver" );
	if( comp )
		comp->Activate();

	// Switch to the shooter cam.
	if ((d_scene != NULL) && (d_scene->Scene(nPlayerID) != NULL))
	{
		g_shooterCam.UnlockTarget();
		CameraGroup* group = d_scene->Scene(nPlayerID)->FindGroup("Avatar");
		if (group != NULL)
			group->Select("Shooter");
	}

	AvatarControlMapper *mapper = GetMapper(nPlayerID, false);
	if (mapper)
		mapper->Reset();

	ActivateControls(true);
}

/***************************************************************************/
//	respawn a player after death or warp
/***************************************************************************/
bool CPlayers::RespawnParticipant(int i_participantID, bool i_force,
		bool i_rearm, bool i_random, bool i_ignoreOccupancy)
{
	for (int i = 0; i < players; i++)
	{
		if (i_participantID == GetParticipantId(i))
			return(RespawnPlayer(i, i_force, i_rearm, i_random, i_ignoreOccupancy));
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
void CPlayers::RespawnParticipant(int i_participantID, Vector3CRef position,
		float heading, bool i_rearm)
{
	for (int i = 0; i < players; i++)
	{
		if (i_participantID == GetParticipantId(i))
			RespawnPlayer(i, position, heading, i_rearm);
	}
}

/***************************************************************************/
//	Places all the players back to their starting positions, and anything
// else necessary for a clean reset.
/***************************************************************************/
void CPlayers::ResetPlayerPositions(void)
{
	// Place players
	for (int i = 0; i < players; i++)
	{
		RespawnPlayer(i, true, false);
	}
}

/***************************************************************************/
/***************************************************************************/
void CPlayers::CManDown::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	for (int i = 0; i < d_ref->players; i++)
	{
		if (d_ref->player[i] == CActor::FromHandle(i_sender))
		{
			CActorComponent *driver =
					d_ref->player[i]->GetActiveComponentInClass("KinematicDriver");
			if(driver)
				driver->Deactivate();
			CActorComponent *comp = d_ref->player[i]->GetComponentByName( "ViewDriver" );
			if( comp )
				comp->Deactivate();

			if ((d_ref->d_scene != NULL) && (d_ref->d_scene->Scene(i) != NULL))
			{
				CameraGroup* group = d_ref->d_scene->Scene(i)->FindGroup("Avatar");
				if (group != NULL)
				{
					Camera& camera = group->Selected();
					if (!strcmp(camera.Name(), "Shooter"))
					{
						CParticipantIF* partIF =
								CParticipantIF::GetInterface(d_ref->player[i]);
						if ((partIF != NULL) && !partIF->GetSpawnTicket())
						{
							group->Select("Ghost");
							GhostCamera &ghostCamera = (GhostCamera&)group->Selected();
							ghostCamera.setParticipantID(partIF->GetParticipantId());
							ghostCamera.SetFollow(d_ref->player[i]);
						}
						else
						{
							group->Select("Death");
							DeathCamera &deathCamera = (DeathCamera&)group->Selected();
							deathCamera.SetPosition(camera.Eye());
						}
					}
				}
			}

			return;
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void CPlayers::Reset()
{
	players = 0;
	d_scene = NULL;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		player[i] = NULL;

		playerStartPos[i].Set((float)(3 * i), (float)0, (float)(3 * i));
		playerStartHeading[i] = 0.0f;
	}
}

