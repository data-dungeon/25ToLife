// Pre-compiled header
#include "CollideHelper/CollideHelperPCH.h"

// My header (always first)
#include "CollideHelper/CapsuleNode.h"

// Engine includes //
#include "Game/Database/Som.h"     // For INVALID_MATERIALINDEX

// GameLib headers
#include "stage/protagonist.h"
#include "stage/SurfaceType.h"
#include "geomUtil/BoxExtents.h"
#include "EngineHelper/drawutil.h"


CapsuleNode::CapsuleNode()
{
	d_originInActor.Clear();
	d_endInActor.Clear();
	d_radius = 0.0f;
}

/******************************************************************************/
/******************************************************************************/
bool CapsuleNode::GetCollisionCull(CCollisionCull &o_cull)
{
	capsule.ComputeWorldExtents(o_cull.SelectAABB());
	return(true);
}

/******************************************************************************/
/******************************************************************************/
void CapsuleNode::UpdateCollisionHull(float i_deltaSec, XZRect &i_collisionHull)
{
	ASSERT_PTR(d_protagonist);

	Vector3 origin, end;
	float radius = capsule.Radius();
	origin = capsule.Origin();
	end = capsule.End();
	// figure out the distance of the center of the capsule from
	// the actor's bodyinworld
	Vector3 centerFromActor = (origin + end) * 0.5f;
	centerFromActor -= d_protagonist->GetBodyInWorld();

	float minX = Math::Min( origin.X(), end.X() ) - radius;
	float minZ = Math::Min( origin.Z(), end.Z() ) - radius;
	float maxX = Math::Max( origin.X(), end.X() ) + radius;
	float maxZ = Math::Max( origin.Z(), end.Z() ) + radius;

	// Get velocity of obb center.
	// angular velocity should be figured in here, but I haven't
	// quite yet figured out how to really do that.  :/
	Vector3 vel = d_protagonist->GetVelocityInWorld();

	// now add it in
	if (vel.X() < 0.0f)
		minX += vel.X() * i_deltaSec;
	else
		maxX += vel.X() * i_deltaSec;

	if (vel.Z() < 0.0f)
		minZ += vel.Z() * i_deltaSec;
	else
		maxZ += vel.Z() * i_deltaSec;

	i_collisionHull.Set(minX, minZ, maxX, maxZ);
}

/******************************************************************************/
/******************************************************************************/
void CapsuleNode::SyncWithActor(void)
{
	if( d_protagonist != NULL )
	{
		capsule.SetExtents( d_originInActor * d_protagonist->GetBodyToWorld() + d_protagonist->GetBodyInWorld(),
			d_endInActor * d_protagonist->GetBodyToWorld() + d_protagonist->GetBodyInWorld(),
			d_radius );
	}
}

/******************************************************************************/
/******************************************************************************/
void CapsuleNode::SyncWithAnimation(void)
{
	SyncWithActor();
}

/******************************************************************************/
/******************************************************************************/
uint8 CapsuleNode::Material(CCollisionPair* i_pair)
{
	return(d_surfaceType);
}

/******************************************************************************/
/******************************************************************************/
void CapsuleNode::Draw(CCollisionEvent* i_event)
{
	capsule.Draw( DrawUtility::RED );
}

void CapsuleNode::SetSurfaceType(uint8 i_surfaceType)
{
	d_surfaceType = i_surfaceType;
}

/******************************************************************************/
/******************************************************************************/
void CapsuleNode::SetSurfaceName(const char* i_surfaceName)
{
	char name[256];
	strncpy(name, i_surfaceName, 256);
	int surfaceType = g_surfaceType.GetValidDynamicIndex(name, -1, 256);
	d_surfaceType = (surfaceType >= 0) ? surfaceType : INVALID_MATERIALINDEX;
}

void CapsuleNode::SetExtents( Vector3CRef origin, Vector3CRef end, float radius )
{
	d_originInActor = origin;
	d_endInActor = end;
	d_radius = radius;
	SyncWithActor();
}