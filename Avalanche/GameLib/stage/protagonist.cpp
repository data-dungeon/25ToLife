/*
**
**  File:   protagonist.cpp
**  Date:   March 28, 2000
**  By:     Bryant Collard
**  Desc:   A protagonist is an actor that interacts with other
**          protagonists in some direct, physical way.
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

//============================================================================

#include "stage/StagePCH.h"

#include "EngineHelper/timer.h"
#include "CollideHelper/AabbEntity.h"
#include "CollideHelper/ObjectList.h"
#include "CollideHelper/ProtagonistNode.h"
#include "CollideHelper/ObbNode.h"
#include "stage/EntityCategory.h"
#include "stage/ActorCreator.h"

// Define a creator and add it to the creator list.
STANDARD_ACTOR_CREATOR(CProtagonist, protagonist);

//============================================================================

// Constants used in a bitfield to provide hints to aid updating and colliding
// protagonists. See their definition for descriptions.

// Set if the protagonist does not cause another protagonist to displace
// when they collide. Note that it may cause a discontinuity in velocity.
const uint CProtagonist::HINT_IMPOTENT       = 0x01;

// Set if the protagonist does not displace as a result of a collision with
// another protagonist. Note that a collision may result in a discontinuity
// in velocity.
const uint CProtagonist::HINT_JUGGERNAUT     = 0x02;

// Set if the protagonist has a fixed location, that is, if the protagonist
// always stays within a fairly tight volume. The protagonist can rotate or
// move slightly. This will be used (along with the impotent and juggernaut
// hints) during initialization to keep the protagonist out of the cluster
// system and use static spatial database instead.
const uint CProtagonist::HINT_FIXED          = 0x04;

// Set if the protagonist uses the PROCESSED_CATEGORY with collision tests in
// AttemptUpdate().
const uint CProtagonist::HINT_TEST_PROCESSED = 0x08;

// Set if the protagonist does not change state but does want to be involved
// in collision tests. None of the update calls (BeginFrame, BeginUpdate,
// AttemptUpdate, EndUpdate, and EndFrame) will be called but the collision
// calls (CollisionDetect and CollisionAccept) will be.
const uint CProtagonist::HINT_STATIONARY     = 0x10;

// Set if the protagonist is completely inactive. No update or collision calls
// will be made.
const uint CProtagonist::HINT_INACTIVE       = 0x20;

// Constants used in a bitfield to describe the effect of a collision.
const uint CProtagonist::EFFECT_NONE           = 0x0;
const uint CProtagonist::EFFECT_DISPLACED      = 0x1;
const uint CProtagonist::EFFECT_DISCONTINUITY  = 0x2;
const uint CProtagonist::EFFECT_IRRECONCILABLE = 0x4;

const uint CProtagonist::EFFECT_DISPLACEMENT =
		CProtagonist::EFFECT_DISPLACED | CProtagonist::EFFECT_DISCONTINUITY;
const uint CProtagonist::EFFECT_VELOCITY_ONLY =
		CProtagonist::EFFECT_DISCONTINUITY;

CProtagonist::CProtagonist() :
	CActor(s_protagonistClass)
{
	d_clusterElement = NULL;
	d_collisionHull.Set( 0.0f, 0.0f, 0.0f, 0.0f);
	d_collisionNode = NULL;
	d_ownCollisionNode = false;
	d_interactionHint = 0;
	d_updating = false;

	//TRACE("Protagonist 0x%x created, def cons.\n", this);
}

CProtagonist::CProtagonist(const CEntityType &i_type) :
	CActor(i_type)
{
	d_clusterElement = NULL;
	d_collisionHull.Set( 0.0f, 0.0f, 0.0f, 0.0f);
	d_collisionNode = NULL;
	d_ownCollisionNode = false;
	d_interactionHint = 0;
	d_updating = false;

	//TRACE("Protagonist 0x%x created, def cons.\n", this);
}

CProtagonist::~CProtagonist()
{
	//TRACE("Protagonist 0x%x destroyed.\n", this);

	// Remove us, if the collision system hasn't already died
	if (ValidSingleton(CollisionSystem))
		g_collisionSystem.RemoveProtagonist(this);

	/* maybe some stuff attached to him */
	AttachMgr__KillProtagonistAttachers(this);

	// Clean up the collision node.
	if (d_ownCollisionNode)
		delete d_collisionNode;
}

CProtagonist* CProtagonist::FromHandle(ActorHandle i_handle)
{
	CActor* actor = CActor::FromHandle(i_handle);
	if ((actor != NULL) && actor->IsA(s_protagonistClass))
		return((CProtagonist*)actor);
	return(NULL);
}

bool CProtagonist::Initialize(void)
{
	if ( !CActor::Initialize() )
	{
		//TRACE("Protagonist 0x%x failed init.\n", this);
		return(false);
	}

	// Make sure a collision node is set up.
	if (d_collisionNode == NULL)
	{
		SetCollisionNode(new CObbNode, SIMPLE, true);
		((CObbNode*)d_collisionNode)->FitObbToActor();
	}

	d_chainExclude = NO_CATEGORY;

	//TRACE("Protagonist 0x%x init'd.\n", this);
	return(true);
}

uint16 CProtagonist::Color(CCollisionPair* i_pair)
{
	if (d_collisionNode != NULL)
		return(d_collisionNode->Color(i_pair));
	return(CActor::Color(i_pair));
}

uint8 CProtagonist::Material(CCollisionPair* i_pair)
{
	if (d_collisionNode != NULL)
		return(d_collisionNode->Material(i_pair));
	return(CActor::Material(i_pair));
}

void CProtagonist::Draw(CCollisionEvent* i_event)
{
	if (d_collisionNode != NULL)
		d_collisionNode->Draw(i_event);
}

void CProtagonist::Update(float i_deltaSec)
{
	BeginUpdate();
	AttemptUpdate(i_deltaSec);
	EndUpdate(i_deltaSec);
}

void CProtagonist::SyncDependents(void)
{
	if (d_clusterElement && d_clusterElement->GetChild())
		d_clusterElement->GetChild()->ModifyMasterState();
}

void CProtagonist::ModifyMasterState(void)
{
	MasterStateChangeNotify();

	if (d_clusterElement)
	{
		if (d_clusterElement->GetNextSibling())
			d_clusterElement->GetNextSibling()->ModifyMasterState();
		if (d_clusterElement->GetChild())
			d_clusterElement->GetChild()->ModifyMasterState();
	}
}

void CProtagonist::SyncState()
{
	if (d_collisionNode != NULL)
		d_collisionNode->SyncWithActor();
}

void CProtagonist::SyncActor(void)
{
	SyncDependents();
}

CProtagonist::EDependOnResult CProtagonist::DependOn(CProtagonist* i_master)
{
	if (!d_clusterElement)
		return(DEPEND_ON_NO_CLUSTER_ELEMENT);
	return(d_clusterElement->DependOn(i_master));
}

CProtagonist *CProtagonist::GetMaster()
{
	if (!d_clusterElement)
		return(NULL);
	return(d_clusterElement->GetMaster());
}

bool CProtagonist::IsIndependent(void)
{
	return(!d_clusterElement || d_clusterElement->IsIndependent());
}

bool CProtagonist::IsNotInDependency(void)
{
	return(!d_clusterElement || d_clusterElement->IsNotInDependency());
}

void CProtagonist::RunAnimation(bool i_forceUpdate)
{
	STAGE_PROFILE_START(CStageProfile::CHAIN_OBB);
	Vector3 minExtent, maxExtent;
	CObjectListArray<50> objectList;
	if (GetDynamicExtents(minExtent, maxExtent))
	{
		g_collisionSystem.ResetStack();
		CAabbEntity entity;
		entity.Aabb().GetAABB().Set(minExtent, maxExtent);
		entity.SetPositiveExclude(TERRAIN_CATEGORY | d_chainExclude);
		entity.SetIgnoreEntity(this);
		objectList.Add(g_collisionSystem.Test(&entity));
	}
	SetObjectList(&objectList);
	STAGE_PROFILE_END(CStageProfile::CHAIN_OBB);

	CActor::RunAnimation(i_forceUpdate);

	if (d_collisionNode != NULL)
		d_collisionNode->SyncWithAnimation();
}

/* move from protagonist to non-protagonist cast, and vice versa. this involves a number of sub-system
updates, so formalize it. mostly used for environmental actors. non-protagonists don't participate in collisions */

void CProtagonist::MoveToProtagonistCast(void)
{
	g_stage.MoveToProtagonistCast(this);
}

void CProtagonist::MoveToNonProtagonistCast(void)
{
	g_stage.MoveToNonProtagonistCast(this);
}

// -------------------------------- Save/Load Game Functions -----------------------------

#define PROTAGONIST_PARENT_CLASS CActor

const u32 CProtagonist::GetLocalSaveGameMemoryNeeded( void ) const
{
	return( Align_RoundTo16( 0 ) );
}

// ---------------------------------------------------------------------------------------

const u32 CProtagonist::GetSaveGameMemoryNeeded( void )
{
	return( GetLocalSaveGameMemoryNeeded( ) + PROTAGONIST_PARENT_CLASS:: GetSaveGameMemoryNeeded( ) );
}

// ---------------------------------------------------------------------------------------

const u32 CProtagonist::SerializeSaveGame( char *pBuf, bool bDirection)
{
	// --- Save Data?
	if( bDirection )
	{
	}

	// --- Load Data?
	else
	{
	}

	// --- serialize parent class
	PROTAGONIST_PARENT_CLASS::SerializeSaveGame( pBuf + GetLocalSaveGameMemoryNeeded( ), bDirection );

	// --- return amount of memory we serialized in
	return( CProtagonist::GetSaveGameMemoryNeeded( ) );
}

CCollisionNode* CProtagonist::BeginCollisionTest(CStageEntity* i_entity,
		ESelect i_select)
{
	return(d_collisionNode);
}

void CProtagonist::SetCollisionNode(CProtagonistNode* i_collisionNode,
		ECollisionConfiguration i_collisionConfiguration, bool i_ownCollisionNode)
{
	if (d_ownCollisionNode)
		delete d_collisionNode;

	d_collisionNode = i_collisionNode;
	if (d_collisionNode != NULL)
	{
		d_collisionNode->Set(this);
		d_collisionObject = d_collisionNode->GetCollisionObject();;
		d_collisionConfiguration = i_collisionConfiguration;
		d_ownCollisionNode = i_ownCollisionNode;
	}
	else
	{
		d_collisionObject = NULL;
		d_collisionConfiguration = COMPLEX;
		d_ownCollisionNode = false;
	}
}

bool CProtagonist::GetCollisionCull(CCollisionCull &o_cull)
{
	if (d_collisionNode != NULL)
		return(d_collisionNode->GetCollisionCull(o_cull));
	return(false);
}

void CProtagonist::UpdateCollisionHull(float timeStep)
{
	if (d_collisionNode != NULL)
		d_collisionNode->UpdateCollisionHull(timeStep, d_collisionHull);
	else
		d_collisionHull.Set( 0.0f, 0.0f, 0.0f, 0.0f);
}

CInterface* CProtagonist::GetInterface(uint i_id)
{
	if (d_collisionNode != NULL)
		return(d_collisionNode->GetInterface(i_id));
	return(NULL);
}

const char* CProtagonist::GetClassName(void)
{
	return("Protagonist");
}
