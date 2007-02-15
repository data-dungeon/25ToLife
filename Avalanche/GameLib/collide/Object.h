/*
**
**  File:   Object.h
**  Date:   January 31, 2003
**  By:     Bryant Collard
**  Desc:   A base class for geometrical objects (OBBs, Segments, Terrain, etc.)
**          that can collide with each other.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/Object.h $
**  $Revision: 5 $
**      $Date: 6/13/03 10:49a $
**    $Author: Bcollard $
**
*/

#ifndef OBJECT_H
#define OBJECT_H

#include "EngineHelper/drawutil.h"

#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif

class CCollisionObject
{
  public:
	enum ECollisionObjectType
	{
		TERRAIN,
		LINE,
		OBB_OBJECT,
		AABB_OBJECT,
		SPHERE,
		TRIANGLE_LIST,
		CAPSULE
	};
	ECollisionObjectType GetObjectType(void) {return(d_type);}
	void Draw(DrawUtility::COLOR i_color);

#ifdef GCN
	void EndianSwap( void)
	{
		ByteSwap(d_type);
	}
#endif

  protected:
	CCollisionObject(ECollisionObjectType i_type) : d_type(i_type) {}

  private:
	CCollisionObject();

	ECollisionObjectType d_type;
};

#endif // OBJECT_H
