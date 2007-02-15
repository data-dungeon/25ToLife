/*
**
**  File:   XZEntity.h
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   Stage entities that serve up line collision objects.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/CollideHelper/XZEntity.h $
**  $Revision: 7 $
**      $Date: 7/24/03 4:41p $
**    $Author: Bcollard $
**
*/

#ifndef XZ_ENTITY_H
#define XZ_ENTITY_H

#include "collide/QueryEntity.h"
#include "collide/Line.h"
#include "Math/Vector.h"

class CXZEntity : public CQueryEntity
{
  public:
	// Construct.
	CXZEntity();
	CXZEntity(float i_x, float i_z);

	// Access class type and line.
	static const CEntityType &ClassType(void);
	CLineCollisionObject &Line(void);

	// Setup the world extents
	static void SetWorldExtents(float i_floorY, float i_ceilingY);

	void Set(float i_x, float i_z);
	void SetDetectionAbility(uint i_detectionAbility);

	void FindHeight(void);
	void FindHeight(float i_x, float i_z);
	void FindDepth(void);
	void FindDepth(float i_x, float i_z);
	void FindFloor(float i_y);
	void FindFloor(float i_x, float i_y, float i_z);
	void FindFloor(Vector3CRef i_loc);
	void FindCeiling(float i_y);
	void FindCeiling(float i_x, float i_y, float i_z);
	void FindCeiling(Vector3CRef i_loc);

	float YVal(void) const;
	void Intersection(Vector3 &o_intersection) const;
	Vector3CRef Intersection(void) const;

	// Override CQueryEntity
	virtual void BeginQuery(void);
	virtual void EndQuery(void);

	// Override CStageEntity
	virtual bool GetCollisionCull(CCollisionCull &o_cull);
	virtual const char* GetClassName(void);

  private:
	CLineCollisionObject d_line;
	Vector3 d_intersection;
	static const CEntityType s_xzEntityClass;
	static float s_ceiling, s_floor, s_height;
};

#define XZ_ENTITY_HPP
#include "CollideHelper/XZEntity.hpp"

#endif // XZ_ENTITY_H
