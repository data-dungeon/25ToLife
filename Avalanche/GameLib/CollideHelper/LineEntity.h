/*
**
**  File:   LineEntity.h
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   Stage entities that serve up line collision objects.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/CollideHelper/LineEntity.h $
**  $Revision: 6 $
**      $Date: 7/24/03 4:41p $
**    $Author: Bcollard $
**
*/

#ifndef LINE_ENTITY_H
#define LINE_ENTITY_H

#include "collide/QueryEntity.h"
#include "collide/Line.h"

class CCollisionPair;
class CActor;

class CLineEntity : public CQueryEntity
{
  public:
	// Construct.
	CLineEntity();

	// Access class type and line.
	static const CEntityType &ClassType(void);
	CLineCollisionObject &Line(void);

	// Process intersections
	virtual void BeginQuery(void);
	virtual void EndQuery(void);

	// Override CStageEntity
	virtual bool GetCollisionCull(CCollisionCull &o_cull);
	virtual const char* GetClassName(void);

  private:
	CLineCollisionObject d_line;
	static const CEntityType s_lineEntityClass;
};

#define LINE_ENTITY_HPP
#include "CollideHelper/LineEntity.hpp"

#endif // LINE_ENTITY_H
