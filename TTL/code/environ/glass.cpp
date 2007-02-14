///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "environ/glass.h"
#include "Environ/Munition.h"
#include "EngineHelper/sfx.h"
#include "weapons/ttlmunition.h"
#ifdef NETZ
#include "NetZTTL/DOGlass.h"
#endif
#include "stage/ActorCreator.h"
#include "CollideHelper/ObbNode.h"
#include "Effects/bloom.h"

struct GlassParams
{
	// Shatter to invoke
	char		shatter[32];

#ifdef WANT_BULLETHOLES
	// Hole stuff
	float		holeStartScale;
	float		holeEndScaleMin;
	float		holeEndScaleMax;
	float		holeScaleRate;
#endif
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CParam<GlassParams>::RegisterVariables()
{
	// Env wrap
	RegisterVariable(d_data.shatter, "shatter", "fastglass", sizeof(d_data.shatter) - 1);

#ifdef WANT_BULLETHOLES
	// Hole stuff
	RegisterVariable(d_data.holeStartScale, "holeStartScale", 0.1f, 0.0f, 10.0f);
	RegisterVariable(d_data.holeEndScaleMin, "holeEndScaleMin", 1.0f, 0.0f, 10.0f);
	RegisterVariable(d_data.holeEndScaleMax, "holeEndScaleMax", 2.0f, 0.0f, 10.0f);
	RegisterVariable(d_data.holeScaleRate, "holeScaleRate", 0.1f, 0.0f, 10.0f);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// Define a creator and add it to the creator list.
///////////////////////////////////////////////////////////////////////////////
#define WANTGLASS
CUSTOM_ACTOR_CREATOR(CGlass, glass)::Create()
{
#ifdef WANTGLASS
	return(new CGlass);
#else
	return(NULL);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// constructor
///////////////////////////////////////////////////////////////////////////////
CGlass::CGlass() :
CProtagonist(s_glassClass)
{
	// Get our params
	m_params = CParam<GlassParams>::GetParam("glass", "var");
	ASSERT(m_params);

	// Set category
	Category() |= TRANSPARENT_CATEGORY | FIXED_CATEGORY;

	// clear the structs
	memset(&m_hole, 0, sizeof(m_hole));
	m_holes = 0;
#ifdef WANT_BULLETHOLES
	m_animatingHoles = 0;
#endif

	// not shattered yet
	m_shattered = false;
}

///////////////////////////////////////////////////////////////////////////////
// destructor
///////////////////////////////////////////////////////////////////////////////
CGlass::~CGlass()
{
}

///////////////////////////////////////////////////////////////////////////////
// initialize
///////////////////////////////////////////////////////////////////////////////
bool CGlass::Initialize()
{
	// Create the collision node I want. CProtagonist destructor deletes it.
	SetCollisionNode(&m_obbNode, SIMPLE, false);

	// First, fit the obb to the model.
	m_obbNode.FitObbToActor();
	OBB *obb = (OBB*)m_obbNode.GetCollisionObject();

	// What is the minimum axis
	Axis minAxis = MinimumAxis(*obb);
	ComputeCorners(*obb, minAxis, m_corner);
	InflateSmallAxis(m_obbNode, minAxis, Parm().GetFloat("OBBDepth"));

	m_normal = Vector3::Cross(m_corner[3] - m_corner[0], m_corner[1] - m_corner[0]).Normalized();

	// init the parent class
	if (CProtagonist::Initialize() == false)
		return false;

	// We are transparent
	SetPositiveInclude(PLAYER_CATEGORY | QUERY_CATEGORY | CAMERAQUERY_CATEGORY | SUBSONIC_CATEGORY);

	// Look up our material
	m_material = g_surfaceType.GetIndex("glass");

	// number of bullets til shatter
	m_fullHealth = (float)Parm().GetInt("ai/health");
	m_health = m_fullHealth;

	// shatter message
	m_messageDie = Parm().GetString("messageDie");

	// Initialize message handlers
	m_munitionImpactHandler.Initialize(MUNITION_IMPACT_ACTOR, MESSAGE_HANDLE_IF_MASTER,
													GetMessageOwner(), this);

	m_munitionImpactFXHandler.Initialize(MUNITION_IMPACT_ACTORFX,
														MESSAGE_HANDLE_IF_MASTER | MESSAGE_HANDLE_IF_DUPLICA,
														GetMessageOwner(), this);

	m_checkpointResetHandler.Initialize("KillAllAtCheckpoint",
														MESSAGE_HANDLE_IF_MASTER | MESSAGE_HANDLE_IF_DUPLICA,
														GetMessageOwner(), this);

	m_shatterHandler.Initialize("Shatter", MESSAGE_HANDLE_IF_MASTER |
			MESSAGE_ROUTE_TO_MASTER, GetMessageOwner(), this );

	if ( m_messageDie && strlen( m_messageDie ) > 0 )
	{
		m_deathHandler.Initialize( m_messageDie, MESSAGE_HANDLE_IF_MASTER |
			MESSAGE_HANDLE_IF_DUPLICA, GetMessageOwner(), this );
	}

#if 0
	// Bloom test
	ts_bRGBA color = { 255, 255, 255, 255 };
	Vector3 center = (m_corner[0] + m_corner[1] + m_corner[2] + m_corner[3]) * 0.25f;
	Vector3 right, up;
	float width, height;
	ComputeDimensions(*obb, minAxis, right, up, width, height);
	g_bloom.Add(0, center, width, height, -m_normal, right, up, color);
#endif

	// Setup the damage animator stuff
	m_damageAnimator.SetInstance(*Instance());
	m_damageAnimator.SetUVScale(3.0f, 3.0f);
	m_damageAnimator.SetUVScale(0, 1.0f, 1.0f);
	m_damageAnimator.SetUVScale(1, 2.0f, 2.0f);

	// l8z
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CGlass::InitializeVisualProperties(void) 
{	
	CInstance *instance = Instance();
	ASSERT(instance);

	// glass doesn't move, so no vis region reassignment
	SceneMgr::SetInstanceVisRegionStatic(instance, false);
	instance->SetZWriteDisabled(true);
	instance->SetAlphaSort(true);
	instance->SetDynamicallyLit(false);
	instance->SetReceiveShadows(false);
} 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CGlass::Reset()
{
	// Unhide
	Hide(false);
	SetPositiveInclude(PLAYER_CATEGORY | QUERY_CATEGORY | CAMERAQUERY_CATEGORY | SUBSONIC_CATEGORY);

#ifdef WANT_BULLETHOLES
	// kill the particles for bullet holes
	for (int i = 0; i < m_holes; i++)
	{
		Particle__Kill(m_hole[i].handle, false);
		m_hole[i].handle = INVALID_HANDLE;
	}
#endif

	// restore state
	m_holes = 0;
	m_health = m_fullHealth;
	m_shattered = false;

	// First frame of animation
	m_damageAnimator.Reset();
}

///////////////////////////////////////////////////////////////////////////////
// adds a bullet to the pane..
// Creates an owned particle and attaches at the bullet intersection point to the glass object
///////////////////////////////////////////////////////////////////////////////
bool CGlass::AddBulletToPane( Vector3CRef position, float i_damage ,CActor* i_sender )
{
	// duh
	if (m_shattered)
		return false;

	// Decrement health
	m_health -= i_damage;

	// can we add any more bulletholes?
	if (m_holes < MAX_BULLETS_PER_PANE)
	{
		// Nice alias
		BulletHole &hole = m_hole[m_holes++];

		// fill out the bullet info
		Vector3 inWindow = position - m_corner[0];
		hole.position = position - m_normal * Vector3::Dot(inWindow, m_normal);

		// set up his scale
#ifdef WANT_BULLETHOLES
		hole.scale = m_params->holeStartScale;
		hole.scaleTarget = Random::InRange(m_params->holeEndScaleMin, m_params->holeEndScaleMax);
		++m_animatingHoles;
#endif

#ifdef WANT_BULLETHOLES
		// create the particle
		const float scale = 1.0f;
		Vector3Padded particlePos = position;
		hole.handle = ParticleMgr__CreateOwnedParticle("BulletHole",						//	const char *pEffectName,
																		&particlePos,						//	const Vector3 *pvpos,
																		NULL,									//	const Vector3 *pvVelUnitVector,
																		NULL,									//	const float *pfVelMagnitude,
																		NULL,									//	const Vector3 *pvAdditiveVel,
																		NULL,									//	const t_ParticleDeathCallback DeathCallback,
																		this,									//	void *context,
																		scale); 								//	const float fScale = 1.0f);
		ASSERTS(hole.handle != INVALID_HANDLE, "failed to create BulletHole particle");

		// set his normal
		hole.normal.Set(m_normal);
		Particle__SetNormal(hole.handle, &hole.normal);

		// set his scale
		Particle__SetScale(hole.handle, hole.scale);

		// set rotation
		hole.rotation.y(Random::UpTo(Math::Deg2Rad(90.0f)));
		Particle__SetRotation(hole.handle, &hole.rotation);
#endif
	}

	// Handle the damage animator
	if (m_holes == 1)
		m_damageAnimator.SetStage(1);
	else if (m_health < m_fullHealth * 0.5f)
		m_damageAnimator.SetStage(2);

	// l8z
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Time to shatter?
///////////////////////////////////////////////////////////////////////////////
bool CGlass::ThresholdReached()
{
	if (m_health < 0.0f || m_holes >= MAX_BULLETS_PER_PANE)
		return true;
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
// Shetter the Glass - Use the pre-created mesh to shatter.. turns off the initial Actor
// kill the bullethole particles
///////////////////////////////////////////////////////////////////////////////
bool CGlass::ShatterGlass()
{
	// Already shattered
	if (m_shattered)
		return true;

#ifdef WANT_BULLETHOLES
	// kill the particles for bullet holes
	for (int i = 0; i < m_holes; i++)
	{
		Particle__Kill(m_hole[i].handle, false);
		m_hole[i].handle = INVALID_HANDLE;
	}
#endif

	// switch out the mesh, and start the new shatter mesh
	Hide(1);

	// turn off his OBB...
	SetPositiveInclude(NO_CATEGORY);

 	// create our shatter mesh
	ShatterObject *shatter = g_shatter.Create(m_params->shatter);
	ASSERT(shatter);
	if (shatter)
	{
		// Put the bullet holes into object space
		Vector3 bulletHole[MAX_BULLETS_PER_PANE];
		for (int i = 0; i < m_holes; i++)
			bulletHole[i] = m_hole[i].position;

 		// Init our ShatterMesh.. pass in 4 corner points
		shatter->Start(m_corner, bulletHole, m_holes);
	}

	// l8z
	m_shattered = true;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifdef WANT_BULLETHOLES
void CGlass::EndUpdate(
float i_deltaSec)
{
	// update other shite
	CProtagonist::EndUpdate(i_deltaSec);

	if (m_animatingHoles > 0)
	{
		const float scaleRate = m_params->holeScaleRate;
		for (int h = 0; h < m_holes; h++)
		{
			// set up his scale
			if (!Math::Equal(m_hole[h].scale, m_hole[h].scaleTarget))
			{
				Math::MoveToValue(&m_hole[h].scale, m_hole[h].scaleTarget, scaleRate * i_deltaSec);
				Particle__SetScale(m_hole[h].handle, m_hole[h].scale);
				if (Math::Equal(m_hole[h].scale, m_hole[h].scaleTarget))
					--m_animatingHoles;
			}
		}
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////
// got a collision
///////////////////////////////////////////////////////////////////////////////
void CGlass::CollisionAccept(
CStageEntity* i_entity,
CCollisionEvent* i_event)
{
	// if colliding entity is not a protagnonist (ie, terrain), do nothing.
	if (!i_entity->IsA(CProtagonist::ClassType()))
		return;

	// get colliding actor
	CActor *pColActor = (CActor*)i_entity;

	// avatar (if we want avatar to break glass.. it goes in here
	if (pColActor->IsA(CProtagonist::ClassType()))
	{
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CGlass::Axis CGlass::MinimumAxis(
const OBB &obb)
{
	const AABB &aabb = obb.GetConstAABB();
	const float x = aabb.maxV.X();
	const float y = aabb.maxV.Y();
	const float z = aabb.maxV.Z();

	if (x < y && x < z)
		return AXIS_X;
	else if (y < x && y < z)
		return AXIS_Y;
	else
		return AXIS_Z;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CGlass::ComputeCorners(
const OBB &obb,
Axis minAxis,
Vector3 corner[4])
{
	Vector3 inWorld[8];
	obb.Transform(inWorld);

	switch (minAxis)
	{
		case AXIS_X:
			corner[0] = (inWorld[0] + inWorld[1]) * 0.5f;
			corner[1] = (inWorld[2] + inWorld[3]) * 0.5f;
			corner[2] = (inWorld[6] + inWorld[7]) * 0.5f;
			corner[3] = (inWorld[4] + inWorld[5]) * 0.5f;
			break;

		case AXIS_Y:
			corner[0] = (inWorld[0] + inWorld[4]) * 0.5f;
			corner[1] = (inWorld[1] + inWorld[5]) * 0.5f;
			corner[2] = (inWorld[3] + inWorld[7]) * 0.5f;
			corner[3] = (inWorld[2] + inWorld[6]) * 0.5f;
			break;

		case AXIS_Z:
			corner[0] = (inWorld[0] + inWorld[2]) * 0.5f;
			corner[1] = (inWorld[1] + inWorld[3]) * 0.5f;
			corner[2] = (inWorld[5] + inWorld[7]) * 0.5f;
			corner[3] = (inWorld[4] + inWorld[6]) * 0.5f;
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// creates a new OBB depth from vars in Maya
///////////////////////////////////////////////////////////////////////////////
void CGlass::InflateSmallAxis(
CObbNode &node,
Axis minAxis,
float obbDepth)
{
	if (obbDepth <= 0.0f || obbDepth > 5.0f)
		obbDepth = 0.5f;

	switch (minAxis)
	{
		case AXIS_X:
			node.SetXSize(obbDepth);
			break;

		case AXIS_Y:
			node.SetYSize(obbDepth);
			break;

		case AXIS_Z:
			node.SetZSize(obbDepth);
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CGlass::ComputeDimensions(
const OBB &obb,
Axis minAxis,
Vector3 &right,
Vector3 &up,
float &width,
float &height)
{
	const AABB &aabb = obb.GetConstAABB();
	const DirCos3x3 &o = *obb.Matrix();

	switch (minAxis)
	{
		case AXIS_X:
			right = o.GetRow2();
			up = o.GetRow1();
			width = aabb.maxV.Z() * 2.0f;
			height = aabb.maxV.Y() * 2.0f;
			break;

		case AXIS_Y:
			right = o.GetRow2();
			up = o.GetRow0();
			width = aabb.maxV.Z() * 2.0f;
			height = aabb.maxV.X() * 2.0f;
			break;

		case AXIS_Z:
			right = o.GetRow0();
			up = o.GetRow1();
			width = aabb.maxV.X() * 2.0f;
			height = aabb.maxV.Y() * 2.0f;
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CGlass::MunitionImpact::HandleMessage(
void* i_data,
ActorHandle i_sender,
bool i_posted)
{
	ASSERT_PTR(d_ref);
	CMunitionImpact *hit = (CMunitionImpact *)i_data;

	// We got hit
	g_ttlMunition.AckMunitionImpact(CActor::FromHandle(i_sender), hit, d_ref, true);

	// add a bullet
	 if (d_ref->ThresholdReached())
		d_ref->ShatterGlass();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CGlass::MunitionImpactFX::HandleMessage(
void* i_data,
ActorHandle i_sender,
bool i_posted)
{
	ASSERT_PTR(d_ref);
	CMunitionImpact *hit = (CMunitionImpact *)i_data;

	d_ref->AddBulletToPane(hit->d_intersection, hit->d_damage, CActor::FromHandle(i_sender));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CGlass::CShatter::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	d_ref->ShatterGlass();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CGlass::CDeath::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	d_ref->ShatterGlass();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CGlass::CheckpointReset::HandleMessage(
void* i_data,
ActorHandle i_sender,
bool i_posted)
{
	d_ref->Reset();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CNetActor* CGlass::CreateNetActor(void)
{
#ifdef NETZ
	return (CNetActor*)DOGlass::Create();
#else
	return NULL;
#endif
}


