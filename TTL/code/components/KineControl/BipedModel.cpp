/*
**
**  File:   BipedModel.h
**  Date:   February 6, 2004
**  By:     Bryant Collard
**  Desc:   Manages a biped model.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/components/KineControl/BipedModel.cpp $
**  $Revision: #124 $
**  $DateTime: 2005/08/19 15:38:47 $
**    $Author: Bryant_Collard $
**
*/

#include "TTLPCH.h"
#include "components/KineControl/BipedModel.h"
#include "Components/ComponentCreator.h"
#include "Interfaces/ViewIF.h"
#include "data_h_files/players/players_s.h"
#include "EngineHelper/sfx.h"
#include "Game/Animation/Bone.h"
#include "components/KineControl/BipedKineControl_s.h"
#include "Components/Slave.h"
#include "weapons/ttlweapon.h"
#include "weapons/firearm.h"
#include "components/affliction.h"
#include "components/footfall.h"
#include "environ/ConfigMgr.h"
#include "avatar/pstat.h"
#include "Script/ScriptVM.h"

#define SIGHT_PITCH_SCALE 1.0f
#define SIGHT_PITCH_MAX 20.0f
#define SIGHT_PITCH_MIN -20.0f
#define ROTATE_IN_ANIM_FIRST

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(BipedModel, BipedModel, COMPONENT_NEUTRAL_PRIORITY);

/***************************************************************************/
/***************************************************************************/
BipedModel::BipedModel(CCompActor &i_actor) : CActorComponent(i_actor)
{
	for (int i = 0; i < NUM_MODES; i++)
	{
		d_mode[i].d_active = false;
		d_mode[i].d_commanded = false;
		d_mode[i].d_time = -1.0f;
	}

	d_onlyForwardMotions = false;
	d_weapon = NULL;

	actor.AddProperty( "cappedSpeed", Property_float );
	actor.AddProperty( "hostage", Property_handle );
	actor.AddProperty( "hostageTaker", Property_handle );

	actor.SetProperty( "cappedSpeed", MAXFLOAT );
	actor.SetProperty( "hostage", INVALID_OBJECT_HANDLE );
	actor.SetProperty( "hostageTaker", INVALID_OBJECT_HANDLE );

	GetActor().AddProperty( "stopPushing", Property_int );
	GetActor().SetProperty( "stopPushing", 0 );
	GetActor().AddProperty( "terrainCollide", Property_int );
	GetActor().SetProperty( "terrainCollide", 0 );
}

/***************************************************************************/
/***************************************************************************/
void BipedModel::Initialize(void)
{
	d_weapon = NULL;

	d_weaponSelectedHandler.Initialize("WeaponSelected", MESSAGE_HANDLE_IF_MASTER
			| MESSAGE_HANDLE_IF_DUPLICA, actor.GetMessageOwner(), this);
	d_weaponDeselectedHandler.Initialize("WeaponDeselected", MESSAGE_HANDLE_IF_MASTER
			| MESSAGE_HANDLE_IF_DUPLICA, actor.GetMessageOwner(), this);
	d_destructHandler.Initialize("ActorDied", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_deathHandler.Initialize("ManDown", MESSAGE_HANDLE_IF_MASTER | MESSAGE_HANDLE_IF_DUPLICA,
			actor.GetMessageOwner(), this);
	d_hostageTakerHandler.Initialize("HostageTaker", MESSAGE_HANDLE_IF_MASTER | MESSAGE_HANDLE_IF_DUPLICA |
		MESSAGE_ROUTE_TO_NEIGHBORS, actor.GetMessageOwner(), this);
	d_hostageHandler.Initialize("Hostage", MESSAGE_HANDLE_IF_MASTER | MESSAGE_HANDLE_IF_DUPLICA |
		MESSAGE_ROUTE_TO_NEIGHBORS, actor.GetMessageOwner(), this);

	cullRadius = actor.Instance( 0 )->GetCullRadius();

	actor.SetLightColorSource(CActor::LIGHT_COLOR_SOURCE_TERRAIN);
	actor.SetLightColorFloor(0.3f);
	actor.SetBaseAmbientLight( g_lightSet.getAmbientLight() );

	rootBone = actor.GetBone((BoneID)0);
	hipsBone = actor.GetBone("hips1");
	headBone = actor.GetBone("head");
}

/***************************************************************************/
/***************************************************************************/
void BipedModel::Activate(void)
{
	CActorComponent::Activate();
	Reset();
}

/***************************************************************************/
/***************************************************************************/
InteractionMonitor::InteractionType BipedModel::QueryInteractionType( CCompActor *whoWantsToKnow )
{
	if( g_configmgr.TestOptionValue( "apprehension", "1" ) )
	{
		// Attacker must be a cop, and we must be a gangster
		char *role = whoWantsToKnow->Parm().GetString( "role" );
		if( role && strcmp( role, "cop" ) == 0 )
		{
			role = GetActor().Parm().GetString( "role" );
			if( !role || strcmp( role, "cop" ) != 0 )
			{
				// Ignore NPCs in network games
				if( !(ValidSingleton(CSession) && g_session.IsNetworked()) || !((CCombatant*)&GetActor())->GetIsNpc() )
				{
					Property *prop = actor.QueryProperty( "ApprehendTime" );
					if( prop && prop->GetFloat() > 0.0f )
						return InteractionMonitor::INTERACTION_APPREHEND;
				}
			}
		}
	}

	return InteractionMonitor::INTERACTION_NONE;
}

/***************************************************************************/
/***************************************************************************/
void BipedModel::Reset(void)
{
	actor.StartAnimation("RoamUpright", true, true);
	actor.StartAnimation("Init", true, true, ANIM2_SCRIPT);
	actor.Instance( 0 )->SetCullRadius( cullRadius );
	//UpdateAnimationState();
	//UpdateSecondaryAnimationState();
	for (int i = 0; i < NUM_MODES; i++)
	{
		d_mode[i].d_active = false;
		d_mode[i].d_commanded = false;
		d_mode[i].d_time = -1.0f;
	}
	actor.SetProperty( "cappedSpeed", MAXFLOAT );
}

/***************************************************************************/
/***************************************************************************/
void BipedModel::SyncActor(void)
{
}

/***************************************************************************/
/***************************************************************************/
void BipedModel::BeginFrame(void)
{
	if (!IsActive())
		return;

	// on fire and need to stop, or not on fire and need to start?
	#ifdef GOOFY_LOOKING_FIRE
		Afflictions *afflict = (Afflictions *)GetActor().GetComponentByName( "Afflictions" );
		if( afflict )
		{
			if( GetActor().AnimActionSecondary( ANIM2_SCRIPT ) != ACTION_ROAM_ONFIRE &&
					afflict->Afflicted( Afflictions::ONFIRE ) )
				UpdateSecondaryAnimationState();
			else if( GetActor().AnimActionSecondary( ANIM2_SCRIPT ) == ACTION_ROAM_ONFIRE &&
					!afflict->Afflicted( Afflictions::ONFIRE ) )
					UpdateSecondaryAnimationState();
		}
	#endif
}

/***************************************************************************/
/***************************************************************************/
void BipedModel::AttemptUpdate(float i_deltaSec)
{
	if( !IsActive() )
		return;

}

/***************************************************************************/
/***************************************************************************/
void BipedModel::EndUpdate(float i_deltaSec)
{
	if (!IsActive())
		return;

	// Process timeouts.
	for (int i = 0; i < NUM_MODES; i++)
	{
		// Process timeouts.
		if (d_mode[i].d_time > 0.0f)
		{
			d_mode[i].d_time -= i_deltaSec;
			if (d_mode[i].d_time <= 0.0f)
			{
				d_mode[i].d_commanded = false;
				d_mode[i].d_time = -1.0f;
			}
		}
	}

	// check for the end of the ladder climb up animation.  When it's done,
	// we need to offset the actor's position
	if( actor.AnimAction() == ACTION_ROAM_LADDER_ONTOP )
	{
		if( CMotion__IsMotionDone( actor.GetMotnInf( MOTION1 ) ) )
		{
			Vector3 face = actor.GetBodyToWorld().GetRow2();
			Vector3 up = actor.GetBodyToWorld().GetRow1();
			float faceOffset, upOffset, downVel;
			faceOffset = actor.Script( ANIM1_SCRIPT )->GetFloat( "faceOffset" );
			upOffset = actor.Script( ANIM1_SCRIPT )->GetFloat( "upOffset" );
			downVel = actor.Script( ANIM1_SCRIPT )->GetFloat( "downVel" );
			face *= faceOffset;
			up *= upOffset;
			actor.GetBodyInWorld() += face;
			actor.GetBodyInWorld() += up;
			actor.StartAnimation( "UnStowWeapon", true, false );

			actor.GetVelocityInWorld().y( actor.GetVelocityInWorld().y() - downVel );

			actor.SetDiscontinuity( DISCONTINUITY_POSITION | DISCONTINUITY_VELOCITY );
			actor.SyncState();
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void BipedModel::EndFrame(void)
{
	if (!IsActive())
		return;

	Property *prop = actor.QueryProperty( "ApprehendTime" );
	if( prop && prop->GetFloat() > 0.0f )
	{
		prop->Set( prop->GetFloat() - g_timer.GetFrameSec() );
		if( prop->GetFloat() < 0.0f )
		{
			prop->Set( 0.0f );

			// BET - we only get here if the player was stunned to death but was never apprehended.
			PlayerStats* pStats = (PlayerStats*)actor.GetInterface(PlayerStats::GetID());
			if (pStats)
			{
				uint32 data = ((0xFF & pStats->GetStunner()) << 8) | (0xFF & CParticipantIF::GetParticipantIdFromActor(&actor));
				g_messageDispatcher.SendMessageToOwner("RecordKill", (void*)data, actor.GetHandle(), g_referee.GetMessageOwner());
			}
		}
	}

	// Find the desired state based on commanded state and external overrides.
	int i;
	for (i = 0; i < NUM_MODES; i++)
	{
		switch(OverrideExternal((EMode)i))
		{
		 case OVERRIDE_ACTIVE:
			d_mode[i].d_desired = true;
			break;
		 case OVERRIDE_INACTIVE:
			d_mode[i].d_desired = false;
			break;
		 case OVERRIDE_NONE:
			d_mode[i].d_desired = d_mode[i].d_commanded;
			break;
		}
	}

	// Check for internal overrides and stop any modes that need to be.
	for (i = 0; i < NUM_MODES; i++)
	{
		switch(OverrideInternal((EMode)i))
		{
		 case OVERRIDE_ACTIVE:
			d_mode[i].d_desired = true;
			break;
		 case OVERRIDE_INACTIVE:
			d_mode[i].d_desired = false;
			break;
		}

		if (!d_mode[i].d_desired && d_mode[i].d_active)
		{
			d_mode[i].d_active = false;
			StopMode((EMode)i);
		}
	}

	// Start any modes that need to be.
	for (i = 0; i < NUM_MODES; i++)
	{
		if (d_mode[i].d_desired && !d_mode[i].d_active)
		{
			d_mode[i].d_active = true;
			StartMode((EMode)i);
		}
	}

	// tell the animation script the actual long and sidespeeds, after
	// any modifications by collision responses.
	Vector3 velInBody = actor.GetVelocityInWorld() / actor.GetBodyToWorld();

	// Clamp relative velocities if only forward motions are available.
	if (d_onlyForwardMotions)
	{
		velInBody.Z(Math::Abs(velInBody.Z()));
		velInBody.X(Math::Clamp(velInBody.X(), -velInBody.Z(), velInBody.Z()));
	}

	if( OnLadder() )
	{
		actor.Script(ANIM1_SCRIPT)->SetVar( "longspeed", velInBody.y() );
	}
	else
	{
		actor.Script(ANIM1_SCRIPT)->SetVar( "longspeed", velInBody.z() );
	}
	actor.Script(ANIM1_SCRIPT)->SetVar( "sidespeed", velInBody.x() );
}

void BipedModel::RunBones(void)
{
	// Build the trunk to reference transformation.
	float pos = 0.5f;
	Cartesian4x4 trunkToRef;
	// Find the vector, in model space, that points out the front of the
	// desired trunk orientation.
	Vector3 direction;

	direction = actor.GetProperty( "aimDirection" )->GetVector3();

	// scale then clamp
	float pitch = -Math::ArcSinDeg( direction.Y() ) * SIGHT_PITCH_SCALE;
	// Find the clamped desired pitch angle.
	pitch = Math::Clamp( pitch, SIGHT_PITCH_MIN, SIGHT_PITCH_MAX );

	// Find the clamped desired turn angle.
	float turn = Math::Clamp(Math::ArcTan2Deg(direction.X(), direction.Z()),
		-90.0f, 90.0f);

	// Find the position in the procedural turn animation.
#ifdef ROTATE_IN_ANIM_FIRST
	if( turn < -45.0f )
	{
		pos = 0.0f;
		turn += 45.0f;
	}
	else if( turn > 45.0f )
	{
		pos = 1.0f;
		turn -= 45.0f;
	}
	else
	{
		pos = Math::LinearMap( turn, -45.0f, 45.0f, 0.0f, 1.0f );
		turn = 0.0f;
	}
#endif

	// Compose the matrix.
	trunkToRef.SetXRotation(Math::Deg2Rad(pitch));
	trunkToRef.CatYRotation(Math::Deg2Rad(turn));

	// Consider a model M. The model contains bones A and B that can rotate and
	// translate with respect to their parents. All of the other bones in the
	// model are fixed relative to their parents in this analysis.
	//
	// The model M has a coordinate system m fixed in M. Bone A has a coordinate
	// system a' fixed in A. Let coordinate system a be fixed in M and coincident
	// with a' when bone A is in its initial pose with respect to M. Bone B has a
	// coordinate system b" fixed in B. Let coordinate system b' be fixed in A
	// and coincident with b" when B is in its initial pose with respect to A.
	// Similarly, let coordinate system b be fixed in M and coincident with b'
	// when A is in its initial pose with respect to M. Note that b is coincident
	// with b" when both A and B are in their initial poses. Finally, let
	// reference coordinate system r be fixed in M, parallel to m but colocated
	// with b. Also, let coordinate system r' be fixed in A and coincident with r
	// when A is in its initial pose with respect to M. The following illustrates
	// the coordinate systems and their relationships:
	//
	//        -- Fixed in M
	//       |
	//       v      -- Fixed in A
	//             |
	//     m       v      -- Fixed in B
	//     |\            |
	//     | a - a'      v
	//     |\    |\
	//     | b   | b' - b"
	//      \     \
	//       r     r'
	//
	// The transformation from r' to r is
	//
	//     r'Tr = r'Ta' x a'Ta x aTm x mTr              (1)
	//
	// Let
	//
	//     aTr = aTm x mTr                              (2)
	//
	// Substituting (2) into (1) yields
	//
	//     r'Tr = r'Ta' x a'Ta x aTr                    (3)
	//
	// The definition of the coordinate systems implies that when a'Ta is
	// identity, r'Tr is identity.
	//
	//     I = r'Ta' x I x aTr = r'Ta' x aTr            (4)
	//
	// Therefore
	//
	//     r'Ta' = rTa                                  (5)
	//
	// Subsititute (5) into (3)
	//
	//     r'Tr = rTa x a'Ta x aTr                      (6)
	//
	// Solving for a'Ta yields
	//
	//     a'Ta = aTr x r'Tr x rTa                      (7)
	//
	// The transformation from b' to b is
	//
	//     b'Tb = b'Ta' x a'Ta x aTm x mTb              (8)
	//
	// Let
	//
	//     aTb = aTm x mTb                              (9)
	//
	// Following the steps used to go from (1) to (5) yields
	//
	//     b'Ta' = bTa                                  (10)
	//
	// The transformation from b" to b is
	//
	//     b"Tb = b"Tb' x b'Ta' x a'Ta x aTm x mTb      (11)
	//
	// Substitute (10), (7), and (9) into (11) to get
	//
	//     b"Tb = b"Tb' x bTa x aTr x r'Tr x rTa x aTb  (12)
	//
	// Let
	//
	//     bTr = bTa x aTr                              (13)
	//
	// Solve for b"Tb'
	//
	//     b"Tb' = b"Tb x bTr x rTr' x rTb              (14)
	//
	// If B is to be kept in its initial pose with respect to M, b"Tb must be
	// identity. Therefore
	//
	//     b"Tb' = bTr x rTr' x rTb                     (15)
	//
	// So, given some r'Tr, equation (7) can be used to find a'Ta and equation
	// (15) can be used to find b"Tb' such that B will be in its design pose
	// with respect to M.

	// Get at the bones.
	ASSERT((rootBone != NULL) && (hipsBone != NULL));

	// Start by cleaning up the matricies just in case.
	boneSetLocalMatrixToIdentity(rootBone);
	boneSetLocalMatrixToIdentity(hipsBone);

	bool stayUpright = false;
	// check for all the various cases in which we want to ignore hip rotation
	if( (actor.GetProperty("health")->GetFloat() <= 0.0f) || Kicking() || OnLadder() )
		stayUpright = true;
	else if( actor.AnimAction() == ACTION_ROAM_VAULT || actor.AnimAction() == ACTION_ROAM_STUNNED )
		stayUpright = true;
	else if( ACTION_TYPE_MASK( actor.AnimAction() ) == ACTION_ROAM_HOSTAGE )
		stayUpright = true;
	else if( actor.AnimAction() == ACTION_ROAM_UNINTERRUPTIBLE_UPRIGHT )
		stayUpright = true;

	Property *prop = actor.QueryProperty( "stayUpright" );
	if( prop && prop->GetBool() )
	{
		stayUpright = true;
		prop->Set( false );
	}

	// See if the actor is running around.
	if( !stayUpright )
	{
		// Apply procedural animation.
		if( actor.GetMotnInf( MOTION2 )->u32MotnFlags & MOTNINF_FLAG_PROCEDURAL )
			CMotion__SetProceduralPosition( actor.GetMotnInf( MOTION2 ), pos );

		// Get the current transformation from the hips to the model.
		Graphics4x4 hipsToRef;
		boneGetCurrentBoneToModelFromBone(hipsBone, &hipsToRef);

		// Build the root to reference transformations.
		Graphics4x4 rootToRef, refToRoot;
		boneGetCurrentBoneToModelFromBone(rootBone, &rootToRef);
		rootToRef.SetTranslation( Vector3( 0.0f, 0.0f, 0.0f ) );
		rootToRef.PokeTranslation( -(hipsToRef.GetTranslation()));
		bool result = rootToRef.Invert(refToRoot);
		ASSERT(result);

		// Build the hips to reference transformation.
		Graphics4x4 refToHips;
		hipsToRef.PokeTranslation(Vector3(0.0f, 0.0f, 0.0f));
		result = hipsToRef.Invert(refToHips);
		ASSERT(result);

		// Set the tranformation from the root to its initial state.
		Graphics4x4 rootToInitialRoot;
		rootToInitialRoot = rootToRef * trunkToRef * refToRoot;
		boneSetBoneToNeutralBone(rootBone, &rootToInitialRoot);

		// Set the tranformation from the hips bone to its initial state.
		Graphics4x4 hipsToInitialHips;
		Cartesian4x4 refToTrunk;
		trunkToRef.Invert(refToTrunk);
		hipsToInitialHips = hipsToRef * refToTrunk * refToHips;
		boneSetBoneToNeutralBone(hipsBone, &hipsToInitialHips);
	}

	// The actor is not running around.
	else
	{
		if( actor.GetMotnInf( MOTION2 )->u32MotnFlags & MOTNINF_FLAG_PROCEDURAL )
			CMotion__SetProceduralPosition( actor.GetMotnInf( MOTION2 ), 0.5f );

		if( actor.QueryProperty( "heightOffset" ) )
		{
			Graphics4x4 rootMatrix;
			Matrix3x3 rootOrientation;
			boneGetCurrentBoneToModelFromBone(rootBone, &rootMatrix);
			rootMatrix.GetOrientation(rootOrientation);
			rootOrientation.Invert();
			rootMatrix.SetTranslation(
				actor.GetProperty( "heightOffset" )->GetFloat() *
				rootOrientation.GetRow1());
			boneSetBoneToNeutralBone(rootBone, &rootMatrix);
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void BipedModel::RunAnimation(void)
{

}

/***************************************************************************/
/***************************************************************************/
void BipedModel::DeselectWeapon(Weapon* i_weapon)
{
	// Detach the old
	if (i_weapon)
  		i_weapon->Detach(actor);

	// No weeapon selected
	d_weapon = NULL;

	UpdateAnimationState();
	UpdateSecondaryAnimationState();
}

/***************************************************************************/
/***************************************************************************/
void BipedModel::SelectWeapon(Weapon* i_weapon)
{
	if (d_weapon == i_weapon)
		return;

	// Save it
	d_weapon = i_weapon;
	if (d_weapon == NULL)
	{
		CommandStop(MODE_AIM, false);
		CommandStop(MODE_AIMSNAP, false);
		return;
	}

	// Attach the new
	d_weapon->Attach(actor);

	UpdateAnimationState();
	UpdateSecondaryAnimationState();
}

/***************************************************************************/
/***************************************************************************/
bool BipedModel::Jumping(void)
{
	return(actor.AnimAction() == ACTION_ROAM_JUMP);
}

/***************************************************************************/
/***************************************************************************/
bool BipedModel::Kicking(void)
{
	return(actor.AnimAction() == ACTION_ROAM_KICK);
}

/***************************************************************************/
/***************************************************************************/
void BipedModel::CommandStart(EMode i_mode, float i_time)
{
	if( GetActor().AnimAction() == ACTION_ROAM_STUNNED )
	{
		// don't do this shit when you're stunned.
		return;
	}
	d_mode[i_mode].d_commanded = true;
	d_mode[i_mode].d_time = i_time;
}

/***************************************************************************/
/***************************************************************************/
void BipedModel::CommandStop(EMode i_mode, bool i_allowTimeout)
{
	if (!i_allowTimeout || (d_mode[i_mode].d_time <= 0.0f))
	{
		d_mode[i_mode].d_commanded = false;
		d_mode[i_mode].d_time = -1.0f;
	}
}

/***************************************************************************/
/***************************************************************************/
void BipedModel::CommandSetTime(EMode i_mode, float i_time)
{
	d_mode[i_mode].d_time = i_time;
}

/***************************************************************************/
/***************************************************************************/
BipedModel::EOverride BipedModel::OverrideExternal(EMode i_mode)
{
	switch(i_mode)
	{
	 case MODE_AIM:
	 case MODE_AIMSNAP:
	 {
		Property* zoomProperty = actor.QueryProperty("zoomLevel");
		if (zoomProperty && (zoomProperty->GetInt() > 0))
			return(OVERRIDE_ACTIVE);
		break;
	 }
	}
	return(OVERRIDE_NONE);
}

/***************************************************************************/
/***************************************************************************/
BipedModel::EOverride BipedModel::OverrideInternal(EMode i_mode)
{
	switch(i_mode)
	{
	 case MODE_AIM:
	 case MODE_AIMSNAP:
		if (d_weapon == NULL)
			return(OVERRIDE_INACTIVE);
		break;
	 case MODE_CROUCH:
		if (Jumping())
			return(OVERRIDE_INACTIVE);
		break;
	}
	return(OVERRIDE_NONE);
}

/***************************************************************************/
/***************************************************************************/
void BipedModel::StartMode(EMode i_mode)
{
	ts_Bone *bone = NULL;
	switch(i_mode)
	{
		case MODE_AIM:
			// don't interrupt recoils to aim or unaim
			if (actor.AnimActionSecondary(ANIM2_SCRIPT) != ACTION_ROAM_RECOIL &&
					actor.AnimActionSecondary(ANIM2_SCRIPT) != ACTION_ROAM_RELOAD )
				UpdateSecondaryAnimationState();
			break;
		case MODE_AIMSNAP:
			if (actor.AnimActionSecondary(ANIM2_SCRIPT) != ACTION_ROAM_RECOIL &&
					actor.AnimActionSecondary(ANIM2_SCRIPT) != ACTION_ROAM_RELOAD )
				g_messageDispatcher.SendMessageToActor( "AimSnap", 0, actor.GetHandle(), actor.GetHandle() );
			break;

		case MODE_CROUCH:
			actor.SetProperty("cappedSpeed",
					actor.Script(ANIM1_SCRIPT)->GetFloat("walkSpeed"));
			if( !Dead() && ACTION_TYPE_MASK( actor.AnimAction() ) != ACTION_ROAM_LADDER )
			{
				actor.StartAnimation("Crouch", true, true);
			}
			if (d_weapon)
			{
				d_weapon->GetAimSim().SetStance(1);
				d_weapon->GetAimSim().TriggerEvent("crouch");
			}
			break;
		case MODE_COWER:
			if( !Dead() )
			{
				actor.StartAnimation("Cower", true, true);
			}
			break;
		case MODE_HANDS_UP:
			// not if they're stunned
			if( !Dead() )
			{
				if( actor.AnimAction() == ACTION_ROAM_UNINTERRUPTIBLE )
				{
					// I don't care how busy you are, or what you're doing.  stop it and get em up!
					actor.StartAnimation( "IdleTransition", true, true );
				}
				if( actor.AnimActionSecondary( ANIM2_SCRIPT ) != ACTION_ROAM_ONFIRE )
					actor.StartAnimation("HandsUp", true, true, ANIM2_SCRIPT);

				// send message to Actor putting hands up - drop your loot baby!
				g_messageDispatcher.SendMessageToActor("DropPickupSpawner", 0, actor.GetHandle(), actor.GetHandle());
			}
			break;
		case MODE_DIE:
			actor.StartAnimation("Die", true, true);
			break;
		case MODE_LADDER:
			ladderStartPosition = actor.GetBodyInWorld();
			if( !Dead() )
			{
				actor.StartAnimation("Ladder", true, true);
			}
			break;
		case MODE_LADDERSLIDE:
			if( !Dead() )
			{
				actor.StartAnimation("LadderSlide", true, true);
			}
			break;
		case MODE_TAG:
			if( !Dead() )
			{
				actor.StartMotion( "TAG", 0, MOTNINF_FLAG_LOOP | MOTNINF_FLAG_MOTION2 | MOTNINF_FLAG_PRIORITY_HIGH );
				if( d_weapon )
					d_weapon->Hide();
			}
			break;
	}
}

/***************************************************************************/
/***************************************************************************/
void BipedModel::StopMode(EMode i_mode)
{
	switch(i_mode)
	{
		case MODE_AIM:
		case MODE_AIMSNAP:
			// don't interrupt recoils to aim or unaim
			if (actor.AnimActionSecondary(ANIM2_SCRIPT) != ACTION_ROAM_RECOIL &&
					actor.AnimActionSecondary(ANIM2_SCRIPT) != ACTION_ROAM_RELOAD )
				UpdateSecondaryAnimationState();
			break;
		case MODE_CROUCH:
			if( actor.GetProperty( "hostage" )->GetHandle() == INVALID_HANDLE )
				actor.SetProperty("cappedSpeed", MAXFLOAT);
			if( !Dead() && ACTION_TYPE_MASK( actor.AnimAction() ) != ACTION_ROAM_LADDER )
			{
				if( actor.AnimAction() != ACTION_ROAM_JUMP )
					actor.StartAnimation("UnCrouch");
			}
			if (d_weapon)
			{
				d_weapon->GetAimSim().SetStance(0);
				d_weapon->GetAimSim().TriggerEvent("stand");
			}
			break;
		case MODE_COWER:
			if( !Dead() )
			{
				actor.StartAnimation("UnCower");
			}
			break;
		case MODE_HANDS_UP:
			if( !Dead() )
			{
				if( actor.AnimActionSecondary( ANIM2_SCRIPT ) != ACTION_ROAM_ONFIRE )
					actor.StartAnimation("HandsDown", true, true, ANIM2_SCRIPT);
			}
			break;
		case MODE_DIE:
			Reset();
			break;
		case MODE_LADDER:
			if( !Dead() )
			{
				if( actor.AnimAction() != ACTION_ROAM_IDLE )
					actor.StartAnimation("IdleNoInterp", true, true);
			}
			break;
		case MODE_LADDERSLIDE:
			if( !Dead() )
			{
				actor.StartAnimation("LadderSlideLand");
			}
			break;
		case MODE_TAG:
			if( !Dead() )
			{
				if( d_weapon )
					d_weapon->Show();
			}
	}
}

/***************************************************************************/
/***************************************************************************/
void BipedModel::UpdateAnimationState(void)
{
	switch(actor.AnimAction())
	{
		case ACTION_ROAM_IDLE:
		case ACTION_ROAM_TURNLEFT:
		case ACTION_ROAM_TURNRIGHT:
			actor.StartAnimation("IdleTransition", true);
			break;

		case ACTION_ROAM_WALK:
		case ACTION_ROAM_WALKBACK:
			actor.StartAnimation("WalkTransition", true);
			break;

		case ACTION_ROAM_RUN:
		case ACTION_ROAM_RUNBACK:
			actor.StartAnimation("RunTransition", true);
			break;

		case ACTION_ROAM_SIDESTEPLEFT:
		case ACTION_ROAM_SIDESTEPRIGHT:
			actor.StartAnimation("SideTransition", true);
			break;

		case ACTION_ROAM_SIDESTEPRUNLEFT:
		case ACTION_ROAM_SIDESTEPRUNRIGHT:
			actor.StartAnimation("SideRunTransition", true);
			break;

		default:
			break;
	}
}

/***************************************************************************/
/***************************************************************************/
void BipedModel::UpdateSecondaryAnimationState( void )
{
	g_messageDispatcher.SendMessageToActor( "UpdateSecondaryAnimation", 0, actor.GetHandle(), actor.GetHandle() );
}

/***************************************************************************/
/***************************************************************************/
bool BipedModel::ScriptOnGround(void)
{
	ASSERT(actor.IsA(CCompActor::ClassType()));
	return(actor.GetProperty("onGround")->GetBool());
}

/***************************************************************************/
/***************************************************************************/
void BipedModel::ScriptFootprint(int footIndex)
{
	ASSERT(actor.IsA(CCompActor::ClassType()));

	// Get the footfall handler
	Footfall *footfall = (Footfall *)actor.GetComponentByName("Footfall");
	if (!footfall)
		return;

	// What foot are we on?
	Footfall::Foot foot = (footIndex == 0) ? Footfall::FOOTFALL_LEFT : Footfall::FOOTFALL_RIGHT;

	switch( actor.AnimAction() )
	{
#ifdef HAVESCUFFS
		case ACTION_ROAM_RUN:
		case ACTION_ROAM_RUNBACK:
			footfall->Step(Footfall::FOOTFALL_RUN, foot);
			break;

		case ACTION_ROAM_WALK:
		case ACTION_ROAM_WALKBACK:
			footfall->Step(Footfall::FOOTFALL_WALK, foot);
			break;

		case ACTION_ROAM_SIDESTEPLEFT:
		case ACTION_ROAM_SIDESTEPRIGHT:
		case ACTION_ROAM_SIDESTEPRUNLEFT:
		case ACTION_ROAM_SIDESTEPRUNRIGHT:
			footfall->Step(Footfall::FOOTFALL_SIDESTEP, foot);
			break;
#else
		case ACTION_ROAM_RUN:
		case ACTION_ROAM_RUNBACK:
		case ACTION_ROAM_SIDESTEPRUNLEFT:
		case ACTION_ROAM_SIDESTEPRUNRIGHT:
			footfall->Step(Footfall::FOOTFALL_RUN, foot);
			break;

		case ACTION_ROAM_WALK:
		case ACTION_ROAM_WALKBACK:
		case ACTION_ROAM_SIDESTEPLEFT:
		case ACTION_ROAM_SIDESTEPRIGHT:
			footfall->Step(Footfall::FOOTFALL_WALK, foot);
			break;
#endif
	}
}

/***************************************************************************/
/***************************************************************************/
bool BipedModel::ScriptLaunch(Vector3CRef i_vel)
{
	// only jump if we're on ground
	if (actor.GetProperty("onGround")->GetBool() || OnLadder() )
	{
		g_messageDispatcher.SendMessageToActor("Launch", (void*)&i_vel,
				actor.GetHandle(), actor.GetHandle());
		return(true);
	}

	return(false);
}

/***************************************************************************/
// The script needs an index back that it can use to reference into the big
// ugly nasty array of weapon anims.
/***************************************************************************/
int BipedModel::ScriptCurrentWeapon(void)
{
	// Do we have a weapon?
	if (!d_weapon)
		return WEAPONHOLD_NONE;
	else
	{
		WeaponInventory *inv = (WeaponInventory *)GetActor().GetComponentByName( "WeaponInventory" );
		if( inv )
		{
			Weapon *weapon = inv->GetSelected();
			if( weapon )
				if( weapon->IsHidden() )
					return WEAPONHOLD_NONE;
		}
		return ((TTLWeapon *)d_weapon)->GetWeaponHold();
	}
}

int BipedModel::ScriptCurrentWeaponReloadType(void)
{
	if( !d_weapon )
		return WEAPONRELOAD_NONE;
	else
		return ((TTLWeapon *)d_weapon)->GetWeaponReload();
}

/***************************************************************************/
/***************************************************************************/
void BipedModel::ScriptKick(void)
{
	g_messageDispatcher.SendMessageToActor("KickContact", NULL, actor.GetHandle(), actor.GetHandle());
}

/***************************************************************************/
/***************************************************************************/
void BipedModel::WeaponSelectedHandler::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	ASSERT(&d_ref->actor == CActor::FromHandle(i_sender));
	d_ref->SelectWeapon((Weapon*)i_data);
}

/***************************************************************************/
/***************************************************************************/
void BipedModel::WeaponDeselectedHandler::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	ASSERT(&d_ref->actor == CActor::FromHandle(i_sender));
	d_ref->DeselectWeapon((Weapon*)i_data);
}

void BipedModel::DestructHandler::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	// if we're a hostage and our master just got destroyed (probably due to getting disconnected),
	// make sure we get released
	ActorHandle hostageTaker = d_ref->GetActor().GetProperty( "hostageTaker" )->GetHandle();
	if( hostageTaker == i_sender )
	{
		g_messageDispatcher.SendMessageToActor( "Hostage", 0, INVALID_OBJECT_HANDLE, d_ref->actor.GetHandle() );
	}
}

/***************************************************************************/
/***************************************************************************/
void BipedModel::DeathHandler::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	// did our hostage just die? OR did I just die holding a hostage?
	ActorHandle hostage = d_ref->GetActor().GetProperty( "hostage" )->GetHandle();
	if( ( hostage == i_sender ) || ((&d_ref->GetActor() == CActor::FromHandle(i_sender)) && hostage.Valid()))
	{
		g_messageDispatcher.SendMessageToActor( "HostageTaker", CMessageDispatcher::HandleToData(INVALID_OBJECT_HANDLE), d_ref->actor.GetHandle(), d_ref->actor.GetHandle() );
		g_messageDispatcher.SendMessageToActor( "Hostage", 0, INVALID_OBJECT_HANDLE, hostage );
	}

	if (CActor::FromHandle(i_sender) == &d_ref->actor)
		d_ref->StartDie();
}

/***************************************************************************/

void BipedModel::HostageHandler::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	// I've become a hostage of somebody else.  Let them drag me around
	d_ref->StopCrouch();

	ActorHandle oldTaker = d_ref->GetActor().GetProperty( "hostageTaker" )->GetHandle();
	if( oldTaker != i_sender )
	{
		d_ref->GetActor().SetProperty( "hostageTaker", (uint32)i_sender );

		if( i_sender )
		{
			Slave *slave = (Slave *)d_ref->GetActor().GetComponentByName( "Slave" );
			if( !slave )
			{
				slave = (Slave *)d_ref->GetActor().AttachComponent( "Slave" );
				d_ref->GetActor().InitializeComponents();
			}
			CProtagonist* sender = CProtagonist::FromHandle( i_sender );
			ASSERT_PTR( sender );
			slave->AttachToActor( sender );
			CActorComponent *comp = d_ref->GetActor().GetActiveComponentInClass( "CollisionBehavior" );
			if( comp )
				comp->Deactivate();
		}
		else
		{
			Slave *slave = (Slave *)d_ref->GetActor().GetComponentByName( "Slave" );
			if( slave )
			{
				d_ref->GetActor().DetachComponent( slave );
			}
			CActorComponent *comp = d_ref->GetActor().GetDefaultComponentInClass( "CollisionBehavior" );
			if( comp )
				comp->Activate();

			CCompActor *taker = CCompActor::FromHandle( oldTaker );
			if( taker )
			{
				// bump the hostage forward, and the taker back
				Vector3 forward = taker->GetBodyToWorld().GetRow2();
				d_ref->GetActor().GetBodyInWorld() += forward * 1.0f;
			}

			d_ref->GetActor().SetProperty( "terrainCollide", 2 );
			d_ref->GetActor().SyncActor();
			d_ref->GetActor().SyncState();
		}
	}
}

void BipedModel::HostageTakerHandler::HandleMessage(void* i_data, ActorHandle i_sender,
	   bool i_posted)
{
	// I've taken a hostage.  Drag them around
	d_ref->StopCrouch();

	ActorHandle oldHostage = d_ref->GetActor().GetProperty( "hostage" )->GetHandle();
	ActorHandle newHostage = CMessageDispatcher::DataToHandle(i_data);
	if( oldHostage != newHostage )
	{
		d_ref->GetActor().SetProperty( "hostage", (uint32)newHostage );
		if( newHostage )
			d_ref->GetActor().SetProperty("cappedSpeed", d_ref->GetActor().Script(ANIM1_SCRIPT)->GetFloat("walkSpeed"));
		else
			d_ref->GetActor().SetProperty("cappedSpeed", MAXFLOAT);
		d_ref->GetActor().SetIgnoreEntity( CActor::FromHandle(newHostage) );
	}

	// Put them into secondary mode
	WeaponInventory *inv = (WeaponInventory *)d_ref->GetActor().GetComponentByName("WeaponInventory");
	ASSERT(inv);
	if (newHostage)
		inv->Select(TTLWeapon::SECONDARY);

	// If this is a firearm, turn off the offhand weapon and reloading
	Weapon *weapon = inv->GetSelected();
	Firearm *firearm = weapon ? (Firearm *)weapon->GetInterface("firearm") : NULL;
	if (firearm)
	{
		firearm->EnableOffHand(!newHostage.Valid());
		firearm->EnableReload(false);
	}

	// We don't have a hostage
	if (newHostage == INVALID_OBJECT_HANDLE)
	{
		if( firearm )
			firearm->EnableReload(true);

		// don't let the hostage push us around too much
		d_ref->GetActor().SetProperty( "stopPushing", 2 );

		d_ref->GetActor().SyncState();
		d_ref->GetActor().SyncActor();
	}
}

CInterface *BipedModel::GetInterface( uint i_id )
{
	if( i_id == InteractionIF::GetID() )
	{
		Property *prop = GetActor().QueryProperty( "ApprehendTime" );
		if( prop && prop->GetFloat() > 0.0f )
			return (InteractionIF *)this;
	}

	return NULL;
}

