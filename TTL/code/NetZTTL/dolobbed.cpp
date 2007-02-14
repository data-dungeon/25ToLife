/*
**
**  File:   DOLobbed.cpp
**  Date:   October 14, 2003
**  By:     Bryant Collard
**  Desc:   Implementation of the generic distributed object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/dolobbed.cpp $
**  $Revision: #12 $
**  $DateTime: 2005/06/02 20:03:22 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "TTLPCH.h"

// My header
#include "NetZTTL/dolobbed.h"

// Local includes
#include "Network/NetZ/DRSixDOF.h"
#include "Network/NetZ/SessionTime.h"

// Gamelib includes
#include "EngineHelper/timer.h"
#include "Environ/ActorInitializer.h"
#include "stage/CCompActor.h"
#include "weapons/lobbed.h"

/******************************************************************************/
/******************************************************************************/
DOLobbed::DOLobbed()
{
	d_state.SetAccelerationDataset(d_acceleration);
}

/******************************************************************************/
/******************************************************************************/
DOLobbed::~DOLobbed()
{
	if (d_actor)
	{
		CCompActor* actor = (CCompActor*)d_actor;
		CActorComponent* comp = actor->GetComponentByName("DRSixDOF");
		if (comp)
			actor->DetachComponent(comp);
	}
}

/**********************************************************************/
/**********************************************************************/
CActor* DOLobbed::CreateProxy(const char* i_class)
{
	// Create the actor and delete it when no longer in a cast.
	CCompActor* actor = new CCompActor;
	ASSERT_PTR(actor);

	// Attach a basic duplication component.
	actor->GetComponents().Add(new DRSixDOFComponent(*actor, *this, d_state));

	// Setup the FX component
	LobbedProperties *props = (LobbedProperties *)g_weaponLib.GetProperties(d_lobbedProps.d_type);
	ASSERT(props);
	if (props && (props->m_liveSound[0] || props->m_liveParticleEmitter[0]))
		actor->AttachComponent(new LobbedFXComponent(*actor, *props));

	return(actor);
}

/**********************************************************************/
/**********************************************************************/
void DOLobbed::InitMaster(void)
{
	CCompActor* actor = (CCompActor*)d_actor;
	LobbedFXComponent *fx = (LobbedFXComponent *)actor->GetComponentByName("LobbedFX");
	d_lobbedProps.d_type = fx ? fx->GetProperties().GetIndex() : 0;

	DRSixDOFComponent::Push(d_actor, d_state, g_timer.GetEndSec());
	d_actor->SetStateDistributed(true);

	DOActor::InitMaster();
}

/**********************************************************************/
/**********************************************************************/
void DOLobbed::InitProxy(void)
{
	CCompActor* actor = (CCompActor*)d_actor;
	DRSixDOFComponent* dr = (DRSixDOFComponent*)actor->GetComponentByName(
			"DRSixDOF");
	dr->Pull();
	DOActor::InitProxy();
}

/**********************************************************************/
/**********************************************************************/
void DOLobbed::PropagateMaster(float i_gameTime)
{
	CCompActor* actor = (CCompActor*)d_actor;
	Property* accProp = actor->QueryProperty("bodyAcc");
	if (accProp != NULL)
		d_acceleration.SetVector(accProp->GetVector3());
	DRSixDOFComponent::Push(d_actor, d_state, i_gameTime);
	DOActor::PropagateMaster(i_gameTime);
}

/******************************************************************************/
/******************************************************************************/
void DOLobbed::Terminate(void)
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
