/*
**
**  File:   LineEntity.hpp
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   Stage entities that serve up line collision objects.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/CollideHelper/LineEntity.hpp $
**  $Revision: 4 $
**      $Date: 7/18/03 3:41p $
**    $Author: Nmartin $
**
*/

#ifdef LINE_ENTITY_HPP
#undef LINE_ENTITY_HPP

#include "collide/Pair.h"
#include "collide/Event.h"
#include "collide/Intersection.h"
#include "collide/LineToObb.h"
#include "stage/StageEntity.h"

#define REALLYLARGEY		100000.0f

/**********************************************************************/
/**********************************************************************/
inline CLineEntity::CLineEntity() :
	CQueryEntity(s_lineEntityClass)
{
	d_collisionConfiguration = SIMPLE;
	d_collisionObject = &d_line;
	ManageCollisionComplexity(&d_line);
}

/**********************************************************************/
/**********************************************************************/
inline const CEntityType &CLineEntity::ClassType(void)
{
	return(s_lineEntityClass);
}

/**********************************************************************/
/**********************************************************************/
inline CLineCollisionObject &CLineEntity::Line(void)
{
	return(d_line);
}

#endif // LINE_ENTITY_HPP
