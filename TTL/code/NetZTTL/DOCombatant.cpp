/*
**
**  File:   DOCombatant.cpp
**  Date:   January 13, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the combatant distributed object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DOCombatant.cpp $
**  $Revision: #82 $
**  $DateTime: 2005/08/19 15:38:47 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "TTLPCH.h"

// My header
#include "NetZTTL/DOCombatant.h"

// NetZ headers
#include "Network/NetZ/DRGroundFollower.h"
#include "Network/NetZ/DRPathFollower.h"
#include "Network/NetZ/SessionTime.h"

// Gamelib headers
#include "stage/CCompActor.h"
#include "Interfaces/ViewIF.h"
#include "Components/ViewComponent.h"
#include "Environ/Munition.h"
#include "components/painstats.h"
#include "Script/ScriptSystem.h"

// TTL headers
#include "environ/ActorCreator.h"
#include "components/KineControl/BipedModel.h"
#include "InterfacesTTL/ParticipantIF.h"

#include "weapons/supersonic.h"

#define PROPERTY_ON_GROUND  0x01
#define MODEL_MODE_AIM      0x02
#define MODEL_MODE_CROUCH   0x04
#define MODEL_MODE_COWER    0x08
#define MODEL_MODE_HANDS_UP 0x10
#define MODEL_MODE_DEAD     0x20

///////////////////////////////// DOCombatant //////////////////////////////////

/******************************************************************************/
/******************************************************************************/
DOCombatant::~DOCombatant()
{
	if (d_actor)
	{
		CActorComponent* comp;
		CCompActor* actor = (CCompActor*)d_actor;
		comp = actor->GetComponentByName("DRPathFollower");
		if (comp)
			actor->DetachComponent(comp);
		comp = actor->GetComponentByName("DRGroundFollower");
		if (comp)
			actor->DetachComponent(comp);
		comp = actor->GetComponentByName("DOCombatant");
		if (comp)
			actor->DetachComponent(comp);

		CParticipantIF* participantIF = CParticipantIF::GetInterface(d_actor);
		if (participantIF != NULL)
			participantIF->SetNetParticipant(&d_participant);
	}
}

/******************************************************************************/
/******************************************************************************/
void DOCombatant::InitDO()
{
	d_participant.SetIsMaster(IsADuplicationMaster());

	DOActor::InitDO();
}

/******************************************************************************/
/******************************************************************************/
CActor* DOCombatant::CreateProxy(const char* i_class)
{
	// Create the actor and delete it when no longer in a cast.
	CCombatant *actor = new CCombatant;
	ASSERT(actor != NULL);

	// Load up the parameter set
	actor->Parm().SetFromString(d_props.d_parm.CStr());

	// Attach network components
	actor->GetComponents().Add(new DOCombatantComponent(*actor, *this,
			d_mode, d_state));

	// Add the correct DR algorithm and detach the unused one
	if (actor->Parm().GetString("path_name"))
	{
		d_coordinates.SetIsActive(false);
		d_path.SetIsActive(true);
		actor->GetComponents().Add(new DRPathFollowerComponent(*actor, 
				*this, d_path));
	}
	else
	{
		d_path.SetIsActive(false);
		d_coordinates.SetIsActive(true);
		actor->GetComponents().Add(new DRGroundFollowerComponent(*actor, 
				*this, d_coordinates));
	}

	// Attach a view component. Lock the view so the view interface really
	// returns what I put in.
	ViewComponent* viewComponent = new ViewComponent(*actor);
	viewComponent->LockView();
	actor->GetComponents().Add(viewComponent);

	return((CActor*)actor);
}

/******************************************************************************/
/******************************************************************************/
GameInitGeometry DOCombatant::GetInitGeometryFunc(void)
{
	return(ActorCreator::InitGameSpecificGeometry);
}

/******************************************************************************/
/******************************************************************************/
void DOCombatant::InitMaster(void)
{
	d_debug = false;
	d_toggleDebugHandler.Initialize("ToggleDeadReckoningDebug",
			MESSAGE_HANDLE_IF_MASTER | MESSAGE_ROUTE_TO_MASTER,
			d_actor->GetMessageOwner(), this);
	d_clearDebugHandler.Initialize("ClearDeadReckoningDebug",
			MESSAGE_HANDLE_IF_MASTER | MESSAGE_ROUTE_TO_MASTER,
			d_actor->GetMessageOwner(), this);

	CParticipantIF* participantIF = CParticipantIF::GetInterface(d_actor);
	if (participantIF != NULL)
		participantIF->SetNetParticipant(&d_participant);

	d_mode.d_flags = ~0x0;
	Push();

	// Initialize the DR components
	CCompActor* actor = (CCompActor*)d_actor;
	if (actor->GetComponentByName("PathFollower") != NULL)
	{
		d_coordinates.SetIsActive(false);
		d_path.SetIsActive(true);
		DRPathFollowerComponent::SetPathFollowerObject(
				(CPathFollower*)actor->GetComponentByName("PathFollower"), d_path);
		DRPathFollowerComponent::Push(d_actor, d_path, g_timer.GetEndSec(),
				actor->GetProperty("onGround")->GetBool());
	}
	else
	{
		d_coordinates.SetIsActive(true);
		d_path.SetIsActive(false);
		DRGroundFollowerComponent::Push(d_actor, d_coordinates,
				g_timer.GetEndSec(), actor->GetProperty("onGround")->GetBool());
	}
	actor->SetStateDistributed(true);

	DOActor::InitMaster();
}

/******************************************************************************/
/******************************************************************************/
void DOCombatant::InitProxy(void)
{
	d_toggleDebugHandler.Initialize("ToggleDeadReckoningDebug",
			MESSAGE_HANDLE_IF_MASTER | MESSAGE_ROUTE_TO_MASTER,
			d_actor->GetMessageOwner(), this);
	d_clearDebugHandler.Initialize("ClearDeadReckoningDebug",
			MESSAGE_HANDLE_IF_MASTER | MESSAGE_ROUTE_TO_MASTER,
			d_actor->GetMessageOwner(), this);

	// Hook up the participant interface.
	CParticipantIF* participantIF = CParticipantIF::GetInterface(d_actor);
	if (participantIF != NULL)
		participantIF->SetNetParticipant(&d_participant);

	// Get master data. Do the ground follower first so the coordinate state
	// is set for the combatant.
	CCompActor* actor = (CCompActor*)d_actor;
	if (d_path.IsActive())
	{
		((DRPathFollowerComponent*)actor->GetComponentByName(
				"DRPathFollower"))->Pull();
	}
	else
	{
		((DRGroundFollowerComponent*)actor->GetComponentByName(
				"DRGroundFollower"))->Pull();
	}
	DOCombatantComponent* component =
			(DOCombatantComponent*)actor->GetComponentByName("DOCombatant");
	component->StateChanged();
	component->Pull();

	DOActor::InitProxy();

	// Get everything running.
	actor->GetComponentByName("BipedModel")->EndFrame();
	for (int i = 0; i < 5; i++)
		actor->RunAnimation(true);
}

/******************************************************************************/
/******************************************************************************/
void DOCombatant::CToggleDebugHandler::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	d_ref->d_debug = !d_ref->d_debug;
}

/******************************************************************************/
/******************************************************************************/
void DOCombatant::CClearDebugHandler::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	d_ref->d_debug = false;
}

/******************************************************************************/
/******************************************************************************/
void DOCombatant::BecomeMaster(void)
{
	d_debug = false;
	d_actor->SetStateDistributed(true);

	DOActor::BecomeMaster();

	// Remove proxy components.
	CActorComponent* comp;
	CCompActor* actor = (CCompActor*)d_actor;
	comp = actor->GetComponentByName("DRPathFollower");
	if (comp)
		actor->DetachComponent(comp);
	comp = actor->GetComponentByName("DRGroundFollower");
	if (comp)
		actor->DetachComponent(comp);
	comp = actor->GetComponentByName("DOCombatant");
	if (comp)
		actor->DetachComponent(comp);

	// Remove AI control and AI associated components.
	comp = actor->GetComponentByName("AiBehavior");
	if (comp)
		actor->DetachComponent(comp);
	comp = actor->GetComponentByName("AiBodyControl");
	if (comp)
		actor->DetachComponent(comp);
	comp = actor->GetComponentByName("AiCombatStrategy");
	if (comp)
		actor->DetachComponent(comp);
	comp = actor->GetComponentByName("AiNavigation");
	if (comp)
		actor->DetachComponent(comp);
	comp = actor->GetComponentByName("AiPerception");
	if (comp)
		actor->DetachComponent(comp);
	comp = actor->GetComponentByName("AiPsyche");
	if (comp)
		actor->DetachComponent(comp);
//	comp = actor->GetComponentByName("Say");
//	if (comp)
//		actor->DetachComponent(comp);
	comp = actor->GetComponentByName("AiWeaponControl");
	if (comp)
		actor->DetachComponent(comp);
	comp = actor->GetComponentByName("PathFollower");
	if (comp)
		actor->DetachComponent(comp);
	comp = actor->GetComponentByName("Rider");
	if (comp)
		actor->DetachComponent(comp);
	comp = actor->GetComponentByName("Taunt");
	if (comp)
		actor->DetachComponent(comp);
	comp = actor->GetComponentByName("ViewComponent");
	if (comp)
		actor->DetachComponent(comp);
	comp = actor->GetComponentByName("WallVault");
	if (comp)
		actor->DetachComponent(comp);

	// Initialize master components.
	actor->InitializeComponents();
	((CCombatant*)d_actor)->Reset();

	// Load AI script.
	if (actor->Script(AI_SCRIPT))
	{
		g_ScriptSystem.DestroyVM(actor->Script(AI_SCRIPT));
		actor->SetScript(AI_SCRIPT, NULL);
	}
	ActorInitializer::LoadAIScript(actor, actor->Parm().GetString("ai"));

	// Initialize AI components.
	actor->InitializeComponents();
}

/******************************************************************************/
/******************************************************************************/
void DOCombatant::BecomeProxy(void)
{
	CCompActor* actor = (CCompActor*)d_actor;

	// Attach network components
	actor->GetComponents().Add(new DOCombatantComponent(*actor, *this,
			d_mode, d_state));

	// Add the correct DR algorithm and detach the unused one
	if (actor->Parm().GetString("path_name"))
	{
		d_coordinates.SetIsActive(false);
		d_path.SetIsActive(true);
		DRPathFollowerComponent* dr = new DRPathFollowerComponent(*actor, 
				*this, d_path);
		ASSERT_PTR(dr);
		actor->GetComponents().Add(dr);
		dr->Pull();
	}
	else
	{
		d_path.SetIsActive(false);
		d_coordinates.SetIsActive(true);
		DRGroundFollowerComponent* dr = new DRGroundFollowerComponent(*actor, 
				*this, d_coordinates);
		ASSERT_PTR(dr);
		actor->GetComponents().Add(dr);
		dr->Pull();
	}

	// Components, etc.?

	DOActor::BecomeProxy();
}

/******************************************************************************/
/******************************************************************************/
void DOCombatant::PropagateMaster(float i_gameTime)
{
	if (d_actor)
	{
		Push();
		if (d_path.IsActive())
		{
			DRPathFollowerComponent::Push(d_actor, d_path, i_gameTime,
					((CCompActor*)d_actor)->GetProperty("onGround")->GetBool(),
					d_debug);
		}
		else
		{
			DRGroundFollowerComponent::Push(d_actor, d_coordinates, i_gameTime,
					((CCompActor*)d_actor)->GetProperty("onGround")->GetBool(),
					d_debug);
		}
	}
	DOActor::PropagateMaster(i_gameTime);
}

/******************************************************************************/
/******************************************************************************/
void DOCombatant::Terminate(void)
{
	if (d_actor)
	{
		CActorComponent* comp;
		CCompActor* actor = (CCompActor*)d_actor;
		comp = actor->GetComponentByName("DRPathFollower");
		if (comp)
			actor->DetachComponent(comp);
		comp = actor->GetComponentByName("DRGroundFollower");
		if (comp)
			actor->DetachComponent(comp);
		comp = actor->GetComponentByName("DOCombatant");
		if (comp)
			actor->DetachComponent(comp);

		CParticipantIF* participantIF = CParticipantIF::GetInterface(d_actor);
		if (participantIF != NULL)
			participantIF->SetNetParticipant(NULL);
	}
	DOActor::Terminate();
}

/******************************************************************************/
/******************************************************************************/
void DOCombatant::Push(void)
{
	CCompActor* actor = (CCompActor*)d_actor;
	uint8 flags = 0x0;
	if (actor->GetProperty("onGround")->GetBool())
		flags |= PROPERTY_ON_GROUND;
	BipedModel* model = (BipedModel*)actor->GetComponentByName("BipedModel");
	if (model)
	{
		if (model->Aiming())
			flags |= MODEL_MODE_AIM;
		if (model->Crouching())
			flags |= MODEL_MODE_CROUCH;
		if (model->Cowering())
			flags |= MODEL_MODE_COWER;
		if (model->HandsUp())
			flags |= MODEL_MODE_HANDS_UP;
		if (model->Dead())
			flags |= MODEL_MODE_DEAD;
	}

	float health = actor->GetProperty("health")->GetFloat();
	ASSERTS( health < 256.0f, "Contact Dwain about this" );

	u8 currentWeapon = 0;
	WeaponInventory* inv =
			(WeaponInventory*)actor->GetComponentByName("WeaponInventory");
	if (inv)
	{
		currentWeapon = inv->GetSelectedSlot();
//		Weapon* selected = inv->GetSelected();
//		if(selected)
//			currentWeapon = selected->Name();
	}

	int currentPainState = 0;
	CPainStats* pain = (CPainStats*)actor->GetComponentByName("PainStats");
	if (pain)
		currentPainState = pain->GetState();
	
	u8 healthu8 = ( u8 )health;
	if ((d_mode.d_flags != flags) || (d_mode.d_health != healthu8) || 
			(d_mode.d_weapon != currentWeapon) || (d_mode.d_painState != currentPainState) )
	{
		d_mode.d_flags = flags;
		d_mode.d_health = healthu8;
		d_mode.d_weapon = currentWeapon;
		d_mode.d_painState = currentPainState;
		d_mode.RequestUpdate();
	}

	Vector3 sightVector;
	sightVector = actor->GetProperty( "aimDirection" )->GetVector3();
	sightVector = sightVector * actor->GetBodyToWorld();

//	ASSERTS( sightVector.Length( ) <= 1.0f, "not normalized" );
//	d_state.d_sightX = CONVERT_FLOAT_TO_S16( sightVector.X(), 1.0f );
//	d_state.d_sightY = CONVERT_FLOAT_TO_S16( sightVector.Y(), 1.0f );
//	d_state.d_sightZ = CONVERT_FLOAT_TO_S16( sightVector.Z(), 1.0f );

	d_state.d_sightX = sightVector.X();
	d_state.d_sightY = sightVector.Y();
	d_state.d_sightZ = sightVector.Z();
}

/******************************************************************************/
/******************************************************************************/
void DOCombatant::StateChanged(void)
{
	if (d_actor)
		((DOCombatantComponent*)((CCompActor*)d_actor)->GetComponentByName(
				"DOCombatant"))->StateChanged();
}

///////////////////////////// DOCombatantComponent /////////////////////////////

/******************************************************************************/
/******************************************************************************/
DOCombatantComponent::DOCombatantComponent(CCompActor &i_actor,
		DuplicatedObject &i_do, DSCombatantMode &i_mode,
		DSCombatantState &i_state):
	CActorComponent(i_actor),
	d_do(i_do),
	d_mode(i_mode),
	d_state(i_state)
{
	actor.AddProperty("health", Property_float);
	actor.AddProperty("onGround", Property_bool);
	actor.AddProperty("updateLevel", Property_int);
	actor.AddProperty("aimDirection", Property_Vector3);

	actor.SetProperty("health", 100.0f);
	actor.SetProperty("onGround", false);
	actor.SetProperty("updateLevel", 0);
	actor.SetProperty("aimDirection", Vector3( 0.0f, 0.0f, 1.0f ) );
	d_state.GetErrorToleranceFunction()->SetConstantError(0.2f);

	d_drClampAtGround = false;
}

/******************************************************************************/
/******************************************************************************/
void DOCombatantComponent::Initialize(void)
{
	d_munitionImpactAckHandler.Initialize(MUNITION_IMPACT_ACK, 
														MESSAGE_HANDLE_IF_MASTER | MESSAGE_ROUTE_TO_MASTER, 
														actor.GetMessageOwner(), this );
}

/******************************************************************************/
/******************************************************************************/
void DOCombatantComponent::BeginFrame(void)
{
	d_time = g_timer.GetStartSec();
}

/******************************************************************************/
/******************************************************************************/
void DOCombatantComponent::EndUpdate(float i_deltaSec)
{
	// Move the dead reckoned state to the end of the update and pull the
	// results into the actor.
	d_time += i_deltaSec;
	d_do.Refresh(d_state, g_sessionTime.ConvertTimeGameToNet(d_time));
	Pull();

	CActorComponent* comp = actor.GetComponentByName("DRGroundFollower");
	d_drClampAtGround = (comp == NULL) ? false :
			((DRGroundFollowerComponent*)comp)->ClampAtGround();
	actor.SetProperty("onGround", d_drClampAtGround ||
			(((uint8)d_mode.d_flags & PROPERTY_ON_GROUND) != 0));
}

/******************************************************************************/
/******************************************************************************/
void DOCombatantComponent::Pull(void)
{
	// Update the sight vector.
	Vector3 sightVector(d_state.d_sightX, d_state.d_sightY, d_state.d_sightZ);
	if (!sightVector.SafeNormalize())
		sightVector.Set(0.0f, 0.0f, 1.0f);

	ViewIF *viewIF = (ViewIF*)actor.GetInterface(ViewIF::GetID());
	if (viewIF)
	{
		viewIF->LookDirection(sightVector, Vector3(0.0f, 1.0f, 0.0f));
	}

	actor.SetProperty( "aimDirection", sightVector / actor.GetBodyToWorld() );
}

/******************************************************************************/
/******************************************************************************/
void DOCombatantComponent::StateChanged(void)
{
	uint8 flags = (uint8)d_mode.d_flags;
	actor.SetProperty("onGround", ((flags & PROPERTY_ON_GROUND) != 0) ||
			d_drClampAtGround);
	BipedModel* model = (BipedModel*)actor.GetComponentByName("BipedModel");
	if (model)
	{
		if ((flags & MODEL_MODE_AIM) != 0)
			model->StartAim();
		else
			model->StopAim();
		if ((flags & MODEL_MODE_CROUCH) != 0)
			model->StartCrouch();
		else
			model->StopCrouch();
		if ((flags & MODEL_MODE_COWER) != 0)
			model->StartCower();
		else
			model->StopCower();
		if ((flags & MODEL_MODE_HANDS_UP) != 0)
			model->StartHandsUp();
		else
			model->StopHandsUp();
		if ((flags & MODEL_MODE_DEAD) != 0)
			model->StartDie();
		else
			model->StopDie();
	}

	actor.SetProperty("health", (float)((int8)d_mode.d_health));

	WeaponInventory* inv =
			(WeaponInventory*)actor.GetComponentByName("WeaponInventory");
	if (inv)
		inv->Select( ( int )d_mode.d_weapon );

	CPainStats* pain = (CPainStats*)actor.GetComponentByName("PainStats");
	if (pain)
	{
		if (d_mode.d_health > 100.0f)
			pain->Reset();
		pain->SetState( d_mode.d_painState );
	}
}

//============================================================================
void DOCombatantComponent::MunitionImpactAck::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	bool hit = (bool)(i_data != 0);
	ActorHandle victim = i_sender;
}
