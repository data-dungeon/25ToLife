// Precompiled header
#include "TTLPCH.h"

// My header
#include "NetZTTL/dosubsonic.h"

// Local includes
#include "Network/NetZ/DRSixDOF.h"
#include "Network/NetZ/SessionTime.h"

// Gamelib includes
#include "EngineHelper/timer.h"
#include "Environ/ActorInitializer.h"
#include "stage/CCompActor.h"
#include "Components/weapons/projlib.h"
#include "weapons/subsonic.h"

/******************************************************************************/
/******************************************************************************/
DOSubsonic::DOSubsonic()
{
	d_state.SetAccelerationDataset(d_acceleration);
}

/******************************************************************************/
/******************************************************************************/
DOSubsonic::~DOSubsonic()
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
CActor* DOSubsonic::CreateProxy(const char* i_class)
{
	// Create the actor and delete it when no longer in a cast.
	CCompActor* actor = new CCompActor;
	ASSERT_PTR(actor);

	// Attach a basic duplication component.
	actor->GetComponents().Add(new DRSixDOFComponent(*actor, *this, d_state));

	// Setup the FX component
	SubsonicProjectileProperties *props = (SubsonicProjectileProperties *)g_projectileLib.GetProperties(d_subsonicProps.d_type);
	ASSERT(props);
	if (props && (props->m_liveSound[0] || props->m_liveParticleEmitter[0]))
		actor->AttachComponent(new SubsonicFXComponent(*actor, *props));

	return(actor);
}

/**********************************************************************/
/**********************************************************************/
void DOSubsonic::InitMaster(void)
{
	CCompActor* actor = (CCompActor*)d_actor;
	SubsonicFXComponent *fx = (SubsonicFXComponent *)actor->GetComponentByName("SubsonicFX");
	d_subsonicProps.d_type = fx ? fx->GetProperties().GetIndex() : 0;

	DRSixDOFComponent::Push(d_actor, d_state, g_timer.GetEndSec());
	d_actor->SetStateDistributed(true);

	DOActor::InitMaster();
}

/**********************************************************************/
/**********************************************************************/
void DOSubsonic::InitProxy(void)
{
	CCompActor* actor = (CCompActor*)d_actor;
	DRSixDOFComponent* dr = (DRSixDOFComponent*)actor->GetComponentByName(
			"DRSixDOF");
	dr->Pull();
	DOActor::InitProxy();
}

/**********************************************************************/
/**********************************************************************/
void DOSubsonic::PropagateMaster(float i_gameTime)
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
void DOSubsonic::Terminate(void)
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
