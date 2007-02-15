/*
**
**  File:   ObbNode.cpp
**  Date:   July 12, 2004
**  By:     Bryant Collard
**  Desc:   A collision node using a single obb and an associated actor.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/CollideHelper/ObbNode.cpp $
**  $Revision: #8 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

// Pre-compiled header
#include "CollideHelper/CollideHelperPCH.h"

// My header (always first)
#include "CollideHelper/ObbNode.h"

// Engine includes //
#include "Game/Database/Som.h"     // For INVALID_MATERIALINDEX

// GameLib headers
#include "stage/protagonist.h"
#include "stage/SurfaceType.h"
#include "geomUtil/BoxExtents.h"
#include "EngineHelper/drawutil.h"

/******************************************************************************/
/******************************************************************************/
CObbNode::CObbNode()
{
	Vector3 extent(0.5f, 0.5f, 0.5f);
	d_obb.GetAABB().Set(extent.Negated(), extent);
	d_obb.UpdateRadius();
	d_obbInActor.Clear();
	d_obbToActor.Identity();
	d_surfaceType = INVALID_MATERIALINDEX;
}

/******************************************************************************/
/******************************************************************************/
CInterface* CObbNode::GetInterface(uint i_id)
{
	if ((i_id == CCollisionIF::GetID()) ||
			(i_id == COrientedCollisionIF::GetID()))
		return((CInterface*)this);

	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
bool CObbNode::GetCollisionCull(CCollisionCull &o_cull)
{
	d_obb.ComputeWorldExtents(o_cull.SelectAABB());
	return(true);
}

/******************************************************************************/
/******************************************************************************/
void CObbNode::UpdateCollisionHull(float i_deltaSec, XZRect &i_collisionHull)
{
	ASSERT_PTR(d_protagonist);

	const float radius = d_obb.Radius();
	float minX = d_obb.Position().X() - radius;
	float minZ = d_obb.Position().Z() - radius;
	float maxX = d_obb.Position().X() + radius;
	float maxZ = d_obb.Position().Z() + radius;

	// Get velocity of obb center.
	Vector3 vel = d_protagonist->GetVelocityInWorld() +
			d_protagonist->GetAngVelocityInBody().Cross(d_obbInActor) *
			d_protagonist->GetBodyToWorld();

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
void CObbNode::SyncWithActor(void)
{
	if (d_protagonist != NULL)
	{
		d_obb.Position() = (d_obbInActor * d_protagonist->GetBodyToWorld()) +
				d_protagonist->GetBodyInWorld();
		d_obb.Orientation() = d_obbToActor * d_protagonist->GetBodyToWorld();
	}
}

/******************************************************************************/
/******************************************************************************/
void CObbNode::SyncWithAnimation(void)
{
	SyncWithActor();
}

/******************************************************************************/
/******************************************************************************/
uint8 CObbNode::Material(CCollisionPair* i_pair)
{
	return(d_surfaceType);
}

/******************************************************************************/
/******************************************************************************/
void CObbNode::Draw(CCollisionEvent* i_event)
{
	d_obb.Draw(DrawUtility::RED);
}

/******************************************************************************/
/******************************************************************************/
void CObbNode::SetSurfaceType(uint8 i_surfaceType)
{
	d_surfaceType = i_surfaceType;
}

/******************************************************************************/
/******************************************************************************/
void CObbNode::SetSurfaceName(const char* i_surfaceName)
{
	char name[256];
	strncpy(name, i_surfaceName, 256);
	int surfaceType = g_surfaceType.GetValidDynamicIndex(name, -1, 256);
	// This assert means that a non-dynamic material was used to mark
	// this ObbNode *or* the material does not exist in the database
	// In either case, this is something you should fix
#ifdef _DEBUG
	if (strcmpi(i_surfaceName, name) != 0)
	{
		TRACE("Material %s assigned to ObbNode does not exist (or is not dynamic!)\n", i_surfaceName);
		ASSERTS(false, "missing dynamic material!");	
	}
#else
	ASSERTS(strcmpi(i_surfaceName, name) == 0, "missing dynamic material!");
#endif
	d_surfaceType = (surfaceType >= 0) ? surfaceType : INVALID_MATERIALINDEX;
}

/******************************************************************************/
/******************************************************************************/
Vector3 CObbNode::GetCollisionInWorld(void)
{
	return(d_obb.Position());
}

/******************************************************************************/
/******************************************************************************/
Vector3 CObbNode::GetCollisionInActor(void)
{
	return(d_obbInActor);
}

/******************************************************************************/
/******************************************************************************/
void CObbNode::SetCollisionInActor(Vector3CRef i_collisionInActor)
{
	d_obbInActor = i_collisionInActor;
	SyncWithActor();
}

/******************************************************************************/
/******************************************************************************/
void CObbNode::MoveCollisionInActor(Vector3CRef i_offsetInActor)
{
	d_obbInActor += i_offsetInActor;
	SyncWithActor();
}

/******************************************************************************/
/******************************************************************************/
DirCos3x3 CObbNode::GetCollisionToWorld(void)
{
	return(d_obb.Orientation());
}

/******************************************************************************/
/******************************************************************************/
DirCos3x3 CObbNode::GetCollisionToActor(void)
{
	return(d_obbToActor);
}

/******************************************************************************/
/******************************************************************************/
void CObbNode::SetCollisionToActor(const DirCos3x3 &i_collisionToActor)
{
	d_obbToActor = i_collisionToActor;
	SyncWithActor();
}

/******************************************************************************/
/******************************************************************************/
void CObbNode::RotateCollisionToActor(const DirCos3x3 &i_newToOldCollision)
{
	d_obbToActor = i_newToOldCollision * d_obbToActor;
	SyncWithActor();
}

/******************************************************************************/
/******************************************************************************/
float CObbNode::GetXSize(void)
{
	return(d_obb.GetConstAABB().XSize());
}

/******************************************************************************/
/******************************************************************************/
float CObbNode::GetYSize(void)
{
	return(d_obb.GetConstAABB().YSize());
}

/******************************************************************************/
/******************************************************************************/
float CObbNode::GetZSize(void)
{
	return(d_obb.GetConstAABB().ZSize());
}

/******************************************************************************/
/******************************************************************************/
void CObbNode::SetXSize(float i_size)
{
	float halfSize = 0.5f * i_size;
	d_obb.GetAABB().maxV.X(halfSize);
	d_obb.GetAABB().minV.X(-halfSize);
	d_obb.UpdateRadius();
}

/******************************************************************************/
/******************************************************************************/
void CObbNode::SetYSize(float i_size)
{
	float halfSize = 0.5f * i_size;
	d_obb.GetAABB().maxV.Y(halfSize);
	d_obb.GetAABB().minV.Y(-halfSize);
	d_obb.UpdateRadius();
}

/******************************************************************************/
/******************************************************************************/
void CObbNode::SetZSize(float i_size)
{
	float halfSize = 0.5f * i_size;
	d_obb.GetAABB().maxV.Z(halfSize);
	d_obb.GetAABB().minV.Z(-halfSize);
	d_obb.UpdateRadius();
}

/******************************************************************************/
/******************************************************************************/
void CObbNode::SetSize(Vector3CRef i_size)
{
	d_obb.GetAABB().maxV = -0.5f * i_size;
	d_obb.GetAABB().minV = d_obb.GetConstAABB().maxV.Negated();
	d_obb.UpdateRadius();
}

/******************************************************************************/
/******************************************************************************/
void CObbNode::ScaleXSize(float i_scale)
{
	float halfSize = i_scale * d_obb.GetAABB().maxV.X();
	d_obb.GetAABB().maxV.X(halfSize);
	d_obb.GetAABB().minV.X(-halfSize);
	d_obb.UpdateRadius();
}

/******************************************************************************/
/******************************************************************************/
void CObbNode::ScaleYSize(float i_scale)
{
	float halfSize = i_scale * d_obb.GetAABB().maxV.Y();
	d_obb.GetAABB().maxV.Y(halfSize);
	d_obb.GetAABB().minV.Y(-halfSize);
	d_obb.UpdateRadius();
}

/******************************************************************************/
/******************************************************************************/
void CObbNode::ScaleZSize(float i_scale)
{
	float halfSize = i_scale * d_obb.GetAABB().maxV.Z();
	d_obb.GetAABB().maxV.Z(halfSize);
	d_obb.GetAABB().minV.Z(-halfSize);
	d_obb.UpdateRadius();
}

/******************************************************************************/
/******************************************************************************/
void CObbNode::ScaleSize(Vector3CRef i_scale)
{
	d_obb.GetAABB().maxV = d_obb.GetAABB().maxV.Diag(i_scale);
	d_obb.GetAABB().minV = d_obb.GetConstAABB().maxV.Negated();
	d_obb.UpdateRadius();
}

/******************************************************************************/
// Set the obb size and location. The min and max values are expressed in a
// coordinate system aligned with the obb and with its origin at the actor's
// origin.
/******************************************************************************/
void CObbNode::SetExtents(Vector3CRef i_min, Vector3CRef i_max)
{
	// Find the extents.
	Vector3 extent = 0.5f * (i_max - i_min);

	// Build a symmetric obb.
	d_obb.GetAABB().Set(extent.Negated(), extent);
	d_obb.UpdateRadius();

	// Place the obb.
	d_obbInActor = i_min + extent;
	SyncWithActor();
}

/******************************************************************************/
// Set the obb to contain the actor's geometry. If i_index is negative, the box
// will contain all valid geometries; otherwise, only the specified geometry
// will be used. If an invalid geometry is specified, no changes will be made
// and the function will return false. If the minimum y-value ends up within
// i_yOriginBuffer of 0, adjustments will be made to make the minimum y-value
// equal to i_yOriginBuffer. The intended use of this capability is to keep the
// bottom of the obb from intersecting the ground in models where the origin is
// supposed to be at the bottom of the model.
/******************************************************************************/
bool CObbNode::FitObbToActor(int i_index, float i_yOriginBuffer)
{
	// Make sure there is a protagonist.
	ASSERT_PTR(d_protagonist);

	// Barring some more sophisticated approach, the obb will be aligned with
	// the actor.
	d_obbToActor.Identity();

	// Find the model extents.
	Vector3 minExtent, maxExtent;
	if (!FindModelExtents(i_index, minExtent, maxExtent))
		return(false);

	// Adjust the minimum y-value if desired.
	if (Math::Abs(minExtent.Y()) < Math::Abs(i_yOriginBuffer))
		minExtent.Y(i_yOriginBuffer);

	// Build an aligned obb.
	SetExtents(minExtent, maxExtent);

	return(true);
}

/******************************************************************************/
/******************************************************************************/
bool CObbNode::FindModelExtents(int i_index, Vector3 &o_min, Vector3 &o_max)
{
	bool foundGeom = false;
	CBoxExtents boxExtents;

	if (i_index >= 0)
		foundGeom = TestGeometryExtents(i_index, boxExtents);
	else
	{
		for (int i = 0; i < MAX_ACTOR_INSTANCES ; i++)
			foundGeom = (TestGeometryExtents(i, boxExtents) || foundGeom);
	}

	if (foundGeom)
	{
		AABB aabb;
		aabb.minV = boxExtents.GetMinExtent().Diag(d_protagonist->Scale());
		aabb.maxV = boxExtents.GetMaxExtent().Diag(d_protagonist->Scale());
		aabb.Normalize();
		o_min = aabb.minV;
		o_max = aabb.maxV;
	}

	return(foundGeom);
}

/******************************************************************************/
/******************************************************************************/
bool CObbNode::TestGeometryExtents(int i_index, CBoxExtents &io_boxExtents)
{
	// Right now, all geometries are co-located with the actor. Hopefully, more
	// powerful configuration capabilities will be added. So, the whole box
	// extents stuff is overkill but the foundation is laid.
	ts_BoundingBox* box = d_protagonist->Instance(i_index) ?
								 d_protagonist->Instance(i_index)->ModelBoundingBox() : NULL;
	if (box == NULL)
		return(false);

	io_boxExtents.TestExtents(box->Min, box->Max);
	return(true);
}
