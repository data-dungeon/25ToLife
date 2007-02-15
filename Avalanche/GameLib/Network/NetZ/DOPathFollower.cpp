/*
**
**  File:   DOPathFollower.cpp
**  Date:   March 2, 2005
**  By:     Bryant Collard
**  Desc:   Implementation of the path follower distributed object.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DOPathFollower.cpp $
**  $Revision: #3 $
**  $DateTime: 2005/05/03 18:53:50 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/NetZ/DOPathFollower.h"

// Local includes
#include "Network/NetZ/DRPathFollower.h"
#include "Network/NetZ/SessionTime.h"

// Gamelib includes
#include "EngineHelper/timer.h"
#include "stage/CCompActor.h"
#include "Components/ComponentCreator.h"

/******************************************************************************/
/******************************************************************************/
DOPathFollower::~DOPathFollower()
{
	if (d_actor)
	{
		CCompActor* actor = (CCompActor*)d_actor;
		CActorComponent* comp = actor->GetComponentByName("DRPathFollower");
		if (comp)
			actor->DetachComponent(comp);
	}
}

/******************************************************************************/
/******************************************************************************/
CActor* DOPathFollower::CreateProxy(const char* i_class)
{
	// Create the actor.
	CCompActor* actor = new CCompActor;
	ASSERT_PTR(actor);

	// Attach a basic duplication component.
	actor->GetComponents().Add(new DRPathFollowerComponent(*actor, *this, d_state));

	return((CActor*)actor);
}

/******************************************************************************/
/******************************************************************************/
void DOPathFollower::InitMaster(void)
{
	CCompActor* actor = (CCompActor*)d_actor;
	DRPathFollowerComponent::SetPathFollowerObject(
			(CPathFollower*)actor->GetComponentByName("PathFollower"), d_state);
	DRPathFollowerComponent::Push(d_actor, d_state, g_timer.GetEndSec());
	d_actor->SetStateDistributed(true);
	DOActor::InitMaster();
}

/******************************************************************************/
/******************************************************************************/
void DOPathFollower::InitProxy(void)
{
	CCompActor* actor = (CCompActor*)d_actor;
	((DRPathFollowerComponent*)actor->GetComponentByName(
			"DRPathFollower"))->Pull();
	DOActor::InitProxy();
}

/******************************************************************************/
/******************************************************************************/
void DOPathFollower::BecomeMaster(void)
{
	d_actor->SetStateDistributed(true);

	DOActor::BecomeMaster();

	// Remove the dead reckoning component.
	CCompActor* actor = (CCompActor*)d_actor;
	CActorComponent* comp = actor->GetComponentByName("DRPathFollower");
	if (comp)
		actor->DetachComponent(comp);
}

/******************************************************************************/
/******************************************************************************/
void DOPathFollower::BecomeProxy(void)
{
	CCompActor* actor = (CCompActor*)d_actor;

	// Attach a basic duplication component.
	DRPathFollowerComponent* dr =
			new DRPathFollowerComponent(*actor, *this, d_state);
	ASSERT_PTR(dr);
	actor->GetComponents().Add(dr);
	dr->Pull();

	DOActor::BecomeProxy();
}

/******************************************************************************/
/******************************************************************************/
void DOPathFollower::PropagateMaster(float i_gameTime)
{
	DRPathFollowerComponent::Push(d_actor, d_state, i_gameTime);
	DOActor::PropagateMaster(i_gameTime);
}

/******************************************************************************/
/******************************************************************************/
void DOPathFollower::Terminate(void)
{
	if (d_actor)
	{
		CCompActor* actor = (CCompActor*)d_actor;
		CActorComponent* comp = actor->GetComponentByName("DRPathFollower");
		if (comp)
			actor->DetachComponent(comp);
	}
	DOActor::Terminate();
}

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(DOPathFollowerComponent, PathFollowerNetActor,
		COMPONENT_NEUTRAL_PRIORITY);

/******************************************************************************/
/******************************************************************************/
CNetActor* DOPathFollowerComponent::CreateNetActor(void)
{
	return(DOPathFollower::Create());
}
