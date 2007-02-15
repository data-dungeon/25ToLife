/*
**
**  File:   ObbEntity.h
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   An obb entity used for queries.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/CollideHelper/ObbEntity.h $
**  $Revision: 4 $
**      $Date: 7/23/03 5:50p $
**    $Author: Bcollard $
**
*/

#ifndef OBB_ENTITY_H
#define OBB_ENTITY_H

#include "platform/BaseType.h"
#include "collide/QueryEntity.h"
#include "collide/obb.h"
#include "collide/Cull.h"

class CCollisionObject;
class CCollisionPair;
class CCollisionIntersection;

class CObbEntity : public CQueryEntity
{
  public:
	// Construct
	CObbEntity() : CQueryEntity(s_obbEntityClass)
		{ManageCollisionComplexity(&d_obb);}

	// Access
	static const CEntityType &ClassType(void) {return(s_obbEntityClass);}
	OBBInstance &Obb(void) {return(d_obb);}

	// Override CStageEntity
	virtual bool GetCollisionCull(CCollisionCull &o_cull)
		{
			d_obb.ComputeWorldExtents(o_cull.SelectRect());
			return(true);
		}
	virtual const char* GetClassName(void) {return("Obb");}

  protected:
	OBBInstance d_obb;

  private:
	static const CEntityType s_obbEntityClass;
};

#endif // OBB_ENTITY_H
