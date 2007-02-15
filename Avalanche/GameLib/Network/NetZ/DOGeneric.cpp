/*
**
**  File:   DOGeneric.cpp
**  Date:   October 14, 2003
**  By:     Bryant Collard
**  Desc:   Implementation of the generic distributed object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DOGeneric.cpp $
**  $Revision: #38 $
**  $DateTime: 2005/06/02 20:03:22 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/NetZ/DOGeneric.h"

// Local includes
#include "Network/NetZ/DRSixDOF.h"
#include "Network/NetZ/SessionTime.h"

// Gamelib includes
#include "EngineHelper/timer.h"
#include "stage/CCompActor.h"
#include "Components/ComponentCreator.h"

/******************************************************************************/
/******************************************************************************/
DOGeneric::DOGeneric()
{
	d_state.SetAccelerationDataset(d_acceleration);
}

/******************************************************************************/
/******************************************************************************/
DOGeneric::~DOGeneric()
{
	if (d_actor)
	{
		CCompActor* actor = (CCompActor*)d_actor;
		CActorComponent* comp = actor->GetComponentByName("DRSixDOF");
		if (comp)
			actor->DetachComponent(comp);
	}
}

/******************************************************************************/
/******************************************************************************/
void DOGeneric::SetBodyAcceleration(Vector3CRef i_acc)
{
	d_acceleration.SetVector(i_acc);
}

/******************************************************************************/
/******************************************************************************/
CActor* DOGeneric::CreateProxy(const char* i_class)
{
	// Create the actor.
	CCompActor* actor = new CCompActor;
	ASSERT_PTR(actor);

	// Attach a basic duplication component.
	actor->GetComponents().Add(new DRSixDOFComponent(*actor, *this, d_state));

	return((CActor*)actor);
}

/******************************************************************************/
/******************************************************************************/
void DOGeneric::InitMaster(void)
{
	d_debug = false;
	d_toggleDebugHandler.Initialize("ToggleDeadReckoningDebug",
			MESSAGE_HANDLE_IF_MASTER | MESSAGE_ROUTE_TO_MASTER,
			d_actor->GetMessageOwner(), this);
	d_clearDebugHandler.Initialize("ClearDeadReckoningDebug",
			MESSAGE_HANDLE_IF_MASTER | MESSAGE_ROUTE_TO_MASTER,
			d_actor->GetMessageOwner(), this);

	DRSixDOFComponent::Push(d_actor, d_state, g_timer.GetEndSec());
	d_actor->SetStateDistributed(true);
	DOActor::InitMaster();
}

/******************************************************************************/
/******************************************************************************/
void DOGeneric::InitProxy(void)
{
	d_toggleDebugHandler.Initialize("ToggleDeadReckoningDebug",
			MESSAGE_HANDLE_IF_MASTER | MESSAGE_ROUTE_TO_MASTER,
			d_actor->GetMessageOwner(), this);
	d_clearDebugHandler.Initialize("ClearDeadReckoningDebug",
			MESSAGE_HANDLE_IF_MASTER | MESSAGE_ROUTE_TO_MASTER,
			d_actor->GetMessageOwner(), this);

	CCompActor* actor = (CCompActor*)d_actor;
	DRSixDOFComponent* dr = (DRSixDOFComponent*)actor->GetComponentByName(
			"DRSixDOF");
	dr->Pull();
	DOActor::InitProxy();
}

/******************************************************************************/
/******************************************************************************/
void DOGeneric::BecomeMaster(void)
{
	d_debug = false;
	d_actor->SetStateDistributed(true);

	DOActor::BecomeMaster();

	// Remove the dead reckoning component.
	CCompActor* actor = (CCompActor*)d_actor;
	CActorComponent* comp = actor->GetComponentByName("DRSixDOF");
	if (comp)
		actor->DetachComponent(comp);
}

/******************************************************************************/
/******************************************************************************/
void DOGeneric::BecomeProxy(void)
{
	CCompActor* actor = (CCompActor*)d_actor;

	// Attach a basic duplication component.
	DRSixDOFComponent* dr = new DRSixDOFComponent(*actor, *this, d_state);
	ASSERT_PTR(dr);
	actor->GetComponents().Add(dr);
	dr->Pull();

	DOActor::BecomeProxy();
}

/******************************************************************************/
/******************************************************************************/
void DOGeneric::CToggleDebugHandler::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	d_ref->d_debug = !d_ref->d_debug;
}

/******************************************************************************/
/******************************************************************************/
void DOGeneric::CClearDebugHandler::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	d_ref->d_debug = false;
}

/******************************************************************************/
/******************************************************************************/
void DOGeneric::PropagateMaster(float i_gameTime)
{
	DRSixDOFComponent::Push(d_actor, d_state, i_gameTime, d_debug);
	DOActor::PropagateMaster(i_gameTime);
}

/******************************************************************************/
/******************************************************************************/
void DOGeneric::Terminate(void)
{
	if (d_actor)
	{
		CCompActor* actor = (CCompActor*)d_actor;
		CActorComponent* comp = actor->GetComponentByName("DRSixDOF");
		if (comp)
			actor->DetachComponent(comp);
	}
	DOActor::Terminate();
}

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(DOGenericComponent, GenericNetActor,
		COMPONENT_NEUTRAL_PRIORITY);

/******************************************************************************/
/******************************************************************************/
DOGenericComponent::DOGenericComponent(CCompActor &i_actor) :
	CActorComponent(i_actor)
{
	d_genericNetActor = NULL;
}

/******************************************************************************/
/******************************************************************************/
CNetActor* DOGenericComponent::CreateNetActor(void)
{
	// Temporary until Quazal answers some questions.
	d_genericNetActor = DOGeneric::Create(0);
	ASSERT(d_genericNetActor != NULL);
	return((CNetActor*)d_genericNetActor);
}

/******************************************************************************/
/******************************************************************************/
void DOGenericComponent::EndFrame(void)
{
	if (d_genericNetActor != NULL)
	{
		Property* accProp = actor.QueryProperty("bodyAcc");
		if (accProp != NULL)
			d_genericNetActor->SetBodyAcceleration(accProp->GetVector3());
	}
}
