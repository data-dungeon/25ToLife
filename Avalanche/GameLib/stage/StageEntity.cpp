/*
**
**  File:   StageEntity.cpp
**  Date:   January 29, 2003
**  By:     Bryant Collard
**  Desc:   The base class for all entities in the world.
**
**  Copyright (c) 2003, 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/stage/StageEntity.cpp $
**  $Revision: #11 $
**  $DateTime: 2005/06/08 16:04:21 $
**    $Author: Bryant_Collard $
**
*/

// Not using pre-compiled headers. //

// My header (always first) //
#include "stage/StageEntity.h"

// Engine includes //
#include "Game/Database/Som.h"     // For INVALID_MATERIALINDEX

/***************************************************************************/
// Establish the bounds of where the entity is to cull collision checks.
/***************************************************************************/
bool CStageEntity::GetCollisionCull(CCollisionCull &o_cull)
{
	return (false);
}

/***************************************************************************/
// Get a collision node for a collision test with the given entity.
/***************************************************************************/
CCollisionNode* CStageEntity::BeginCollisionTest(CStageEntity* i_entity,
		ESelect i_select)
{
	return(NULL);
}

/***************************************************************************/
// Tell the entity that the collision test is complete.
/***************************************************************************/
bool CStageEntity::EndCollisionTest(CCollisionEvent* i_event)
{
	return(true);
}

/***************************************************************************/
// Get the color of the entity associated with a collision pair.
/***************************************************************************/
uint16 CStageEntity::Color(CCollisionPair* i_pair)
{
	return(0xffff);
}
	
/***************************************************************************/
// Get the material of the entity associated with a collision pair.
/***************************************************************************/
uint8 CStageEntity::Material(CCollisionPair* i_pair)
{
	return(INVALID_MATERIALINDEX);
}

/***************************************************************************/
// Draw a collision event where the entity is the query entity.
/***************************************************************************/
void CStageEntity::Draw(CCollisionEvent* i_event)
{
}

/***************************************************************************/
// Supply the name of the entity.
/***************************************************************************/
const char* CStageEntity::GetClassName(void)
{
	return("StageEntity");
}

/***************************************************************************/
// Append a description of the entity to a string.
/***************************************************************************/
void CStageEntity::AppendDescription(char* io_msg)
{
}
