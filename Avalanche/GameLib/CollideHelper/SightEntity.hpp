/*
**
**  File:   SightEntity.hpp
**  Date:   July 26, 2004
**  By:     Bryant Collard
**  Desc:   A query entity that performs sight checks. It has a sense of
**          range and field of view. It also can perform partial visibility
**          checks.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/CollideHelper/SightEntity.hpp $
**  $Revision: #2 $
**  $DateTime: 2004/08/25 19:34:54 $
**    $Author: Bryant_Collard $
**
*/

#ifdef SIGHT_ENTITY_HPP
#undef SIGHT_ENTITY_H

#include "Math/MathClass.h"

//////////////////////////////// CObscureEntity ////////////////////////////////

/******************************************************************************/
/******************************************************************************/
inline CObscureEntity::CObscureEntity(uint &i_config,
		Vector3 &i_locationInWorld, CActor* &i_actor) :
	d_config(i_config),
	d_locationInWorld(i_locationInWorld),
	d_actor(i_actor)
{
	d_node.Set(&d_obb);
	d_target = NULL;
}

////////////////////////////////// CSightNode //////////////////////////////////

/******************************************************************************/
/******************************************************************************/
inline CSightNode::CSightNode()
{
	d_sphere.SetCenterTest(true);
	d_fov = 0.0f;
	d_cosFovSquared = 1.0f;
	d_directionInWorld.Set(0.0f, 0.0f, 1.0f);
}

/******************************************************************************/
/******************************************************************************/
inline void CSightNode::SetRange(float i_range)
{
	d_sphere.SetRadius(i_range);
}

/******************************************************************************/
/******************************************************************************/
inline void CSightNode::SetFov(float i_fov)
{
	if (i_fov < 0.0f)
		d_fov = 0.0f;
	else if (i_fov > Math::Pi)
		d_fov = Math::Pi;
	else
		d_fov = i_fov;
	float cosFov = Math::Cos(d_fov);
	d_cosFovSquared = cosFov * cosFov;
}

/******************************************************************************/
/******************************************************************************/
inline void CSightNode::SetUnitDirection(Vector3CRef i_directionInWorld)
{
	d_directionInWorld = i_directionInWorld;
}

///////////////////////////////// CSightEvent //////////////////////////////////

/******************************************************************************/
/******************************************************************************/
inline CSightEvent::CSightEvent()
{
	d_pctVisible = 0.0f;
	d_centroid.Clear();
}

/******************************************************************************/
/******************************************************************************/
inline CSightEvent::CSightEvent(CCollisionStack &i_stack) :
	CCollisionEvent(i_stack)
{
	d_pctVisible = 0.0f;
	d_centroid.Clear();
}

/******************************************************************************/
/******************************************************************************/
inline CSightEvent* CSightEvent::New(CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CSightEvent* event = (CSightEvent*)i_stack->Allocate(sizeof(CSightEvent));

		if (event)
			new(event) CSightEvent(*i_stack);

		return(event);
	}
	return(new CSightEvent());
}

///////////////////////////////// CSightEntity /////////////////////////////////

/******************************************************************************/
/******************************************************************************/
inline CSightEntity::CSightEntity() :
	d_obscure(d_config, d_sightNode.Sphere().Center(), d_actor)
{
	d_config = TEST_ONLY;
	d_actor = NULL;
	d_locationInActor.Clear();
	d_directionInActor.Set(0.0f, 0.0f, 1.0f);
}

/******************************************************************************/
/******************************************************************************/
inline void CSightEntity::SetConfiguration(uint i_config)
{
	d_config = i_config;
}

/******************************************************************************/
/******************************************************************************/
inline void CSightEntity::SetRange(float i_range)
{
	d_sightNode.SetRange(i_range);
}

/******************************************************************************/
/******************************************************************************/
inline void CSightEntity::SetFov(float i_fov)
{
	d_sightNode.SetFov(i_fov);
}

/******************************************************************************/
/******************************************************************************/
inline void CSightEntity::SetVolume(float i_range, float i_fov)
{
	d_sightNode.SetRange(i_range);
	d_sightNode.SetFov(i_fov);
}

/******************************************************************************/
/******************************************************************************/
inline void CSightEntity::SetLocation(Vector3CRef i_locationInWorld)
{
	d_sightNode.Sphere().SetCenter(i_locationInWorld);
}

/******************************************************************************/
/******************************************************************************/
inline void CSightEntity::SetUnitDirection(Vector3CRef i_directionInWorld)
{
	d_sightNode.SetUnitDirection(i_directionInWorld);
}

/******************************************************************************/
/******************************************************************************/
inline void CSightEntity::SetDirection(Vector3CRef i_directionInWorld)
{
	Vector3 directionInWorld = i_directionInWorld;
	if (directionInWorld.SafeNormalize())
		d_sightNode.SetUnitDirection(directionInWorld);
}

/******************************************************************************/
/******************************************************************************/
inline void CSightEntity::SetActor(CActor* i_actor)
{
	d_actor = i_actor;
	d_locationInActor.Clear();
	d_directionInActor.Set(0.0f, 0.0f, 1.0f);
}

/******************************************************************************/
/******************************************************************************/
inline void CSightEntity::SetActor(CActor* i_actor,
		Vector3CRef i_locationInActor, Vector3CRef i_directionInActor)
{
	d_actor = i_actor;
	d_locationInActor = i_locationInActor;
	d_directionInActor = i_directionInActor;
	if (!d_directionInActor.SafeNormalize())
	{
		ASSERT(false);
		d_directionInActor.Set(0.0f, 0.0f, 1.0f);
	}
}

#endif
