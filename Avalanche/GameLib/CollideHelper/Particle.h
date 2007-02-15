/*
**
**  File:   Particle.h
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   Tests particle collisions.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/CollideHelper/Particle.h $
**  $Revision: 3 $
**      $Date: 4/24/03 11:54a $
**    $Author: Bcollard $
**
*/

#ifndef PARTICLE_COLLISION_H
#define PARTICLE_COLLISION_H

#include "collide/collsys.h"
#include "CollideHelper/LineEntity.h"
#include "CollideHelper/LineQuery.h"
#include "stage/EntityCategory.h"

inline bool ParticleCollisionTest(Vector3 *pos, Vector3 *lastPos,
		Vector3 *collisionPos, Vector3 *collisionNormal,
		u32 *collisionMaterial, bool testTerrain, bool testDynamic)
{
	CLineEntity entity;
	entity.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	entity.Line().SetOriginEnd(*lastPos, *pos);
	entity.SetPositiveInclude((testTerrain ? WORLD_CATEGORY : NO_CATEGORY) |
			(testDynamic ? ACTOR_CATEGORY : NO_CATEGORY));

	g_collisionSystem.ResetStack();
	CLineQuery collision;
	if (collision.Test(entity))
	{
		*collisionPos = collision.intersection;
		*collisionNormal = collision.normal;
		*collisionMaterial = collision.material;
		return(true);
	}

	return(false);
}

#endif // PARTICLE_COLLISION_H
