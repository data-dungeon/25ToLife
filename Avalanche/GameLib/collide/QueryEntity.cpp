/*
**
**  File:   QueryEntity.cpp
**  Date:   April 3, 2003
**  By:     Bryant Collard
**  Desc:   A base class for entities used to perform collision queries.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/QueryEntity.cpp $
**  $Revision: 2 $
**      $Date: 7/24/03 4:40p $
**    $Author: Bcollard $
**
*/

// Not using pre-compiled headers //

// My header (always first) //
#include "collide/QueryEntity.h"

/**********************************************************************/
/**********************************************************************/
void CQueryEntity::BeginQuery(void)
{
}

/**********************************************************************/
/**********************************************************************/
void CQueryEntity::EndQuery(void)
{
}

/**********************************************************************/
/**********************************************************************/
CCollisionNode* CQueryEntity::BeginCollisionTest(CStageEntity* i_entity,
		ESelect i_select)
{
	if (IsIgnoredEntity(i_entity))
		return(NULL);

	return(&d_node);
}

/***************************************************************************/
/***************************************************************************/
const char* CQueryEntity::GetClassName(void)
{
	return("Query");
}
