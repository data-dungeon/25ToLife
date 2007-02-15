/*
**
**  File:   AabbEntity.h
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   Stage entities that serve up aabb collision objects.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/CollideHelper/AabbEntity.h $
**  $Revision: 5 $
**      $Date: 7/23/03 5:50p $
**    $Author: Bcollard $
**
*/

#ifndef AABB_ENTITY_H
#define AABB_ENTITY_H

#include "platform/BaseType.h"
#include "collide/QueryEntity.h"
#include "collide/AabbObject.h"
#include "collide/Cull.h"

class CCollisionObject;
class CCollisionPair;
class CCollisionIntersection;

///////////////////////////// CAabbEntity /////////////////////////////

class CAabbEntity : public CQueryEntity
{
  public:
	// Construct
	CAabbEntity() : CQueryEntity(s_aabbEntityClass)
		{ManageCollisionComplexity(&d_aabb);}

	// Access
	static const CEntityType &ClassType(void) {return(s_aabbEntityClass);}
	CAabbCollisionObject &Aabb(void) {return(d_aabb);}

	// Override CStageEntity
	virtual bool GetCollisionCull(CCollisionCull &o_cull)
		{
			o_cull.SelectAABB() = d_aabb.GetConstAABB();
			return(true);
		}
	virtual const char* GetClassName(void) {return("Aabb");}

  protected:
	CAabbCollisionObject d_aabb;

  private:
	static const CEntityType s_aabbEntityClass;
};

#endif // AABB_ENTITY_H
