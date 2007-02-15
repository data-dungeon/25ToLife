/*
**
**  File:   Sticky.cpp
**  Date:   July 1, 2004
**  By:     Bryant Collard
**  Desc:   Implements a class that represents a coordinate system that is
**          attached to (or stuck on) some entity or object. The manner of
**          attachment is highly dependent on the object the sticky is attached
**          to. A sticky on terrain, for example, is very simple while one on
**          a boned model could be quite complex. This diversity of attachment
**          cannot, however, be addressed by deriving classes from a sticky
**          base class. A goal of this class is to avoid dynamic allocation and
**          to not require entities to implement code to supply appropriate
**          stickies. Rather, a user of this class will generally have a
**          a sticky and stick it to any entity of interest.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/GameHelper/Sticky.cpp $
**  $Revision: #4 $
**  $DateTime: 2004/10/25 12:08:32 $
**    $Author: Food $
**
*/

// Pre-compiled header
#include "GameHelper/GameHelperPCH.h"

// My header
#include "GameHelper/Sticky.h"

// Engine headers
#include "Math/Vector.h"
#include "Math/Matrix.h"

/******************************************************************************/
/******************************************************************************/
CSticky::CSticky()
{
	d_type = STICKY_NONE;
}

/******************************************************************************/
/******************************************************************************/
bool CSticky::SetInWorld(CStageEntity* i_entity, Vector3CRef i_locationInWorld)
{
	if (i_entity && i_entity->IsA(CActor::ClassType()))
	{
		d_type = STICKY_BODY;
		d_body.d_actor = (CActor*)i_entity;
		d_body.d_locationInBody.Set(
				(i_locationInWorld - d_body.d_actor->GetBodyInWorld()) /
				d_body.d_actor->GetBodyToWorld());
	}
	else
	{
		d_type = STICKY_TERRAIN;
		d_terrain.d_locationInWorld.Set(i_locationInWorld);
	}

	return(true);
}

/******************************************************************************/
/******************************************************************************/
bool CSticky::SetInBody(CStageEntity* i_entity, Vector3CRef i_locationInBody)
{
	if (i_entity && i_entity->IsA(CActor::ClassType()))
	{
		d_type = STICKY_BODY;
		d_body.d_actor = (CActor*)i_entity;
		d_body.d_locationInBody.Set(i_locationInBody);
	}
	else
	{
		d_type = STICKY_TERRAIN;
		d_terrain.d_locationInWorld.Set(i_locationInBody);
	}

	return(true);
}

/******************************************************************************/
/******************************************************************************/
#define STICKY_ACCESS(Name, Type)                                              \
Type &CSticky::Get##Name(Type &o_result) const                                 \
{                                                                              \
	switch(d_type)                                                              \
	{                                                                           \
	 case STICKY_TERRAIN:                                                       \
		return(d_terrain.Get##Name(o_result));                                   \
	 case STICKY_BODY:                                                          \
		return(d_body.Get##Name(o_result));                                      \
	 case STICKY_BONE:                                                          \
		return(d_bone.Get##Name(o_result));                                      \
	 case STICKY_NONE:                                                          \
	 default:                                                                   \
		ASSERT(false);                                                           \
		return(o_result);                                                        \
	}                                                                           \
}

STICKY_ACCESS(Location, Vector3)
STICKY_ACCESS(Orientation, DirCos3x3)
STICKY_ACCESS(Cartesian, Cartesian4x4)
STICKY_ACCESS(Transform, Graphics4x4)
STICKY_ACCESS(Velocity, Vector3)
STICKY_ACCESS(AngularVelocity, Vector3)

//////////////////////////////// CTerrainSticky ////////////////////////////////

/******************************************************************************/
/******************************************************************************/
Vector3 &CSticky::CTerrainSticky::GetLocation(Vector3 &o_result) const
{
	o_result = d_locationInWorld.Get();
	return(o_result);
}

/******************************************************************************/
/******************************************************************************/
DirCos3x3 &CSticky::CTerrainSticky::GetOrientation(DirCos3x3 &o_result) const
{
	o_result.Identity();
	return(o_result);
}

/******************************************************************************/
/******************************************************************************/
Cartesian4x4 &CSticky::CTerrainSticky::GetCartesian(Cartesian4x4 &o_result) const
{
	o_result.SetTranslation(d_locationInWorld.Get());
	return(o_result);
}

/******************************************************************************/
/******************************************************************************/
Graphics4x4 &CSticky::CTerrainSticky::GetTransform(Graphics4x4 &o_result) const
{
	o_result.SetTranslation(d_locationInWorld.Get());
	return(o_result);
}

/******************************************************************************/
/******************************************************************************/
Vector3 &CSticky::CTerrainSticky::GetVelocity(Vector3 &o_result) const
{
	o_result.Clear();
	return(o_result);
}

/******************************************************************************/
/******************************************************************************/
Vector3 &CSticky::CTerrainSticky::GetAngularVelocity(Vector3 &o_result) const
{
	o_result.Clear();
	return(o_result);
}

///////////////////////////////// CBodySticky //////////////////////////////////

/******************************************************************************/
/******************************************************************************/
Vector3 &CSticky::CBodySticky::GetLocation(Vector3 &o_result) const
{
	o_result = d_actor->GetBodyInWorld() + d_locationInBody.Get() *
			d_actor->GetBodyToWorld();
	return(o_result);
}

/******************************************************************************/
/******************************************************************************/
DirCos3x3 &CSticky::CBodySticky::GetOrientation(DirCos3x3 &o_result) const
{
	o_result = d_actor->GetBodyToWorld();
	return(o_result);
}

/******************************************************************************/
/******************************************************************************/
Cartesian4x4 &CSticky::CBodySticky::GetCartesian(Cartesian4x4 &o_result) const
{
	o_result = d_actor->GetBodyToWorld();
	o_result.PokeTranslation(d_actor->GetBodyInWorld() + d_locationInBody.Get() *
			d_actor->GetBodyToWorld());
	return(o_result);
}

/******************************************************************************/
/******************************************************************************/
Graphics4x4 &CSticky::CBodySticky::GetTransform(Graphics4x4 &o_result) const
{
	o_result = d_actor->GetBodyToWorld();
	o_result.PokeTranslation(d_actor->GetBodyInWorld() + d_locationInBody.Get() *
			d_actor->GetBodyToWorld());
	return(o_result);
}

/******************************************************************************/
/******************************************************************************/
Vector3 &CSticky::CBodySticky::GetVelocity(Vector3 &o_result) const
{
	o_result = d_actor->GetVelocityInWorld() -
			d_locationInBody.Get().Cross(d_actor->GetAngVelocityInBody()) *
			d_actor->GetBodyToWorld();
	return(o_result);
}

/******************************************************************************/
/******************************************************************************/
Vector3 &CSticky::CBodySticky::GetAngularVelocity(Vector3 &o_result) const
{
	o_result = d_actor->GetAngVelocityInBody() * d_actor->GetBodyToWorld();
	return(o_result);
}

///////////////////////////////// CBoneSticky //////////////////////////////////

/******************************************************************************/
/******************************************************************************/
Vector3 &CSticky::CBoneSticky::GetLocation(Vector3 &o_result) const
{
	return(o_result);
}

/******************************************************************************/
/******************************************************************************/
DirCos3x3 &CSticky::CBoneSticky::GetOrientation(DirCos3x3 &o_result) const
{
	return(o_result);
}

/******************************************************************************/
/******************************************************************************/
Cartesian4x4 &CSticky::CBoneSticky::GetCartesian(Cartesian4x4 &o_result) const
{
	return(o_result);
}

/******************************************************************************/
/******************************************************************************/
Graphics4x4 &CSticky::CBoneSticky::GetTransform(Graphics4x4 &o_result) const
{
	return(o_result);
}

/******************************************************************************/
/******************************************************************************/
Vector3 &CSticky::CBoneSticky::GetVelocity(Vector3 &o_result) const
{
	return(o_result);
}

/******************************************************************************/
/******************************************************************************/
Vector3 &CSticky::CBoneSticky::GetAngularVelocity(Vector3 &o_result) const
{
	return(o_result);
}
