/*
**
**  File:   SphereEntity.h
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   An sphere entity used for queries.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/CollideHelper/SphereEntity.h $
**  $Revision: 3 $
**      $Date: 7/23/03 5:50p $
**    $Author: Bcollard $
**
*/

#ifndef SPHERE_ENTITY_H
#define SPHERE_ENTITY_H

#include "platform/BaseType.h"
#include "collide/QueryEntity.h"
#include "collide/Sphere.h"
#include "collide/Cull.h"

class CCollisionObject;
class CCollisionPair;
class CCollisionIntersection;

class CSphereEntity : public CQueryEntity
{
  public:
	// Construct
	CSphereEntity() : CQueryEntity(s_sphereEntityClass)
		{ManageCollisionComplexity(&d_sphere);}

	// Access
	static const CEntityType &ClassType(void) {return(s_sphereEntityClass);}
	CSphereCollisionObject &Sphere(void) {return(d_sphere);}

	// Override CStageEntity
	virtual bool GetCollisionCull(CCollisionCull &o_cull)
		{
			d_sphere.ComputeWorldExtents(o_cull.SelectAABB());
			return(true);
		}
	virtual const char* GetName(void) {return("Sphere");}

  protected:
	CSphereCollisionObject d_sphere;

  private:
	static const CEntityType s_sphereEntityClass;
};

#endif // SPHERE_ENTITY_H
