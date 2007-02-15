//============================================================================
//=
//= CollisionInfo.cpp - A component to sends out data of normal, position of 
//=                     intersect etc on a collision.
//=
//============================================================================

#include "Components/ComponentsPCH.h"
#include "Components/CollisionInfo.h"
#include "Components/ComponentCreator.h"

#include "Script/ScriptSystem.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(CCollisionInfo, CollisionInfo,
		COMPONENT_NEUTRAL_PRIORITY);

//============================================================================

CCollisionInfo::CCollisionInfo( CCompActor& i_actor ) :
	CActorComponent( i_actor )
{
}

//============================================================================

CCollisionInfo::~CCollisionInfo()
{
}

//============================================================================

void CCollisionInfo::BeginUpdate()
{
}

//============================================================================

void CCollisionInfo::AttemptUpdate( float i_deltaSec )
{
}

//============================================================================

uint CCollisionInfo::CollisionDetect(CStageEntity* i_entity, CCollisionEvent* i_event, bool i_mateDisplaced)
{
	uint effect = CProtagonist::EFFECT_NONE;
	return effect;
}

//============================================================================

void CCollisionInfo::CollisionAccept( CStageEntity* i_entity, CCollisionEvent* i_event )
{
	bool					result = false;
	SCollisionInfo		colinfo;

	// make sure we are reacting to avatar only
	if ( i_entity->IsA( CProtagonist::ClassType() ) )
	{
		// get plane
		result = CEventMiner::PlaneFit( i_event, m_CollisionPoint, m_CollisionNormal );

		// get info
		colinfo.d_intersection = m_CollisionPoint;
		colinfo.d_normal = m_CollisionNormal;
		colinfo.d_sender = (CActor*)i_entity;

		// send message out containing the data
		g_messageDispatcher.SendMessageToActor("CollisionInfo", &colinfo, actor.GetHandle(), actor.GetHandle());
	}
}


