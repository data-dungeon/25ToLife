/////////////////////////////////////////////////////////////////////////////
// Simple shadow component
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"
#include "Components/ComponentCreator.h"
#include "Game/Managers/ShadowMgr.h"
#include "collide/Object.h"
#include "components/shadow.h"
#include "collide/Capsule.h"
#include "interfaces/groundheightif.h"

STANDARD_COMPONENT_CREATOR(Shadow, Shadow, COMPONENT_NEUTRAL_PRIORITY);

/////////////////////////////////////////////////////////////////////////////
// Pass in a list (terminated with a NULL string)
/////////////////////////////////////////////////////////////////////////////
Shadow::Shadow(
CCompActor &owner) :
CActorComponent(owner)
{
	m_hasProjectedShadow = false;
	m_projectedShadow = INVALID_HANDLE;
	m_dropShadow = NULL;
	m_boneID = INVALID_BONEID;
	m_shadowCenter.Clear();
	m_radius = 1.0f;

	memset(&m_fakeSun, 0, sizeof(m_fakeSun));
	m_fakeSun.u32Flags = LIGHT_DIRECTIONAL;
	m_fakeSun.Pos.Set(0.0f, 1.0f, 1.0f);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Shadow::~Shadow()
{
	// Wee!
	if (m_projectedShadow != INVALID_HANDLE)
		ShadowMgr__KillShadow(m_projectedShadow);

	if (m_dropShadow)
		g_dropShadow.Remove(m_dropShadow);
}

/////////////////////////////////////////////////////////////////////////////
// The initialize
/////////////////////////////////////////////////////////////////////////////
void Shadow::Initialize()
{
	// Call the parent
	CActorComponent::Initialize();

	// Do they want a shadow?
#if defined(FOOD) && defined(_DEBUG)
	m_hasProjectedShadow = false;
#else
	m_hasProjectedShadow = (actor.Parm().GetInt( "projectedShadow" ) == 1) && actor.IsNetMaster();
#endif

	// We are both going to use the hips as the center of projection
	int ret = boneGetBoneID(actor.Instance()->Body(), &m_boneID, (char *)"hips");
	ASSERT(ret >= 0);
	ASSERT(m_boneID != INVALID_BONEID);
	UpdateBones();

	// Create the shadow
	if (!m_hasProjectedShadow)
	{
		const float startAlphaFadeDistance = 0.1f;
		const float endAlphaFadeDistance = 2.0f;
#ifdef PS2
		const float disappearDistance = 8.0f;
#else
		const float disappearDistance = 16.0f;
#endif
		m_dropShadow = g_dropShadow.Add(0, GetRadii(), m_shadowCenter, actor.d_bodyToWorld,
													startAlphaFadeDistance, endAlphaFadeDistance, disappearDistance);
		if (m_dropShadow)
		{
#ifdef PS2
			g_dropShadow.SetBaseAlpha(m_dropShadow, 0.5f);
#endif
			g_dropShadow.EnableCameraAlphaFade(m_dropShadow, 0.8f);
			if (!((CCombatant*)&actor)->GetIsNpc())
				g_dropShadow.SetCoplanarCheck(m_dropShadow, 0.6f);
		}
	}
	else
	{
		// Update the shadow stuff
		UpdateRadius();

		// Make it
		if (actor.Instance(0))
			m_projectedShadow = ShadowMgr__RegisterShadow( actor.Instance(0), &m_fakeSun, 1.0f, &m_shadowCenter, &m_radius );
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Shadow::RunAnimation()
{
	// Call the parent
	CActorComponent::RunAnimation();

	// Update the bones
 	UpdateBones();

	// --- get a real light
	if (m_hasProjectedShadow && m_projectedShadow != INVALID_HANDLE)
	{
		UpdateRadius();
#ifdef DEBUGSHADOWVOLUME
		DrawUtility::Sphere(m_shadowCenter, DrawUtility::CYAN, m_radius);
#endif

		CActorInstance *pCInst = actor.Instance(0);
		if (pCInst->DynamicLightInfluences() && pCInst->DynamicallyLit() )
		{
			Vector3 acculatedLightDir = ( pCInst->DynamicLightInfluences( ) )->getShadowLight( actor.GetFocusInWorld( ) );
			Vector3 lightDirDelta = acculatedLightDir - m_fakeSun.Pos;

			float d1 = lightDirDelta.LengthSquared();
			float d2 = acculatedLightDir.LengthSquared();
			if ( d1 > 0.0f && d2 > 0.01f )
			{
				m_fakeSun.Pos.Set( acculatedLightDir );
				ShadowMgr__UpdateLight( m_projectedShadow, &m_fakeSun );
			}
		}
	}

	// Update the drop shadow
	if (!m_hasProjectedShadow && m_dropShadow)
	{
		// Can we cheat?
		CGroundHeightIF *groundHeight = CGroundHeightIF::GetInterface(actor);
		if (groundHeight && groundHeight->Status() == CGroundHeightIF::HIT)
		{
			CGroundHeightIF::SCheckResults *results;
			if (groundHeight->Get(results))
			{
				Vector3 realHit = m_shadowCenter - results->d_normal * Vector3::Dot(m_shadowCenter - results->d_intersection, results->d_normal);
				g_dropShadow.SetGroundPosition(m_dropShadow, realHit, results->d_normal, results->d_color);
			}
		}

		g_dropShadow.ChangeSize(m_dropShadow, GetRadii());
	}
}

/////////////////////////////////////////////////////////////////////////////
// Get the radii
/////////////////////////////////////////////////////////////////////////////
inline Vector3 Shadow::GetRadii()
{
	// Get the collision object for a simple collision configuration.
	CCollisionObject *collision = actor.GetCollisionObject();

	ASSERT( collision && "No collision object present" );
	switch( collision->GetObjectType() )
	{
		case CCollisionObject::OBB_OBJECT:
			{
				OBB *obb = (OBB *)collision;
				return obb->GetConstAABB().maxV * 1.2f;
			}
			break;
		case CCollisionObject::CAPSULE:
			{
				Capsule *capsule = (Capsule *)collision;
				AABB aabb;
				capsule->ComputeWorldExtents( aabb );
				return ((aabb.maxV - aabb.minV) * 0.5f);
			}
			break;
		default:
			ASSERT( 0 && "Don't know how to get the radius of this collision object type" );
			break;
	}

	return Vector3(0.5f, 0.5f, 0.5f);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Shadow::UpdateBones()
{
	m_shadowCenter = GetBoneInWorld(actor, m_boneID);
	if (!m_hasProjectedShadow)
		m_shadowCenter.Y(actor.d_bodyInWorld.Y());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Shadow::UpdateRadius()
{
	// Get the collision object for a simple collision configuration.
	CCollisionObject *collision = actor.GetCollisionObject();
	ASSERT( collision && "No collision object present" );

	// holy shit this is ugly.  Isn't this like, a perfect, textbook example of
	// where you might want to be able to add a virtual function to CCollisionObject?  :(
	switch( collision->GetObjectType() )
	{
		case CCollisionObject::OBB_OBJECT:
			{
				OBB *obb = (OBB *)collision;
				m_radius = obb->Radius();
			}
			break;
		case CCollisionObject::CAPSULE:
			{
				Capsule *capsule = (Capsule *)collision;
				m_radius = capsule->AxisLength() * 0.5f + capsule->Radius() + 0.25f;
			}
			break;
		default:
			ASSERT( 0 && "Don't know how to get the center and radius of this collision object type" );
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Vector3 Shadow::GetBoneInWorld(
CActor &forWho,
BoneID boneID)
{
	if (boneID == INVALID_BONEID)
		return forWho.GetBodyInWorld();
	else
	{
		Vector3 pos;
		boneGetStartPosition(forWho.Instance()->Body(), boneID, &pos);
		return pos * forWho.Instance()->Matrix();
	}
}

