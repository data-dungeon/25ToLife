/*
**
**  File:   TerrainEntity.h
**  Date:   March 7, 2003
**  By:     Bryant Collard
**  Desc:   A terrain stage entity.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/collide/TerrainEntity.h $
**  $Revision: #15 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#ifndef TERRAIN_ENTITY_H
#define TERRAIN_ENTITY_H

// Engine includes //
#include "Layers/LayersStruct.h"

// Gamelib includes //
#include "collide/Terrain.h"
#include "stage/StageEntity.h"
#include "stage/EntityCategory.h"
#include "collide/Event.h"

// Forward declarations //
class TerrainCollideLayerList;
class FileStream;

class CTerrainEntity : public CStageEntity
{
  public:
	CTerrainEntity(unsigned int &i_layerMask);
	static const CEntityType &ClassType(void) {return(s_terrainEntityClass);}
	void Initialize(void) {}
	void Terminate(void) {d_object.Terminate();}
	TerrainCollideLayerList* Activate(t_Handle i_handle)
		{return(d_object.Activate(i_handle));}
	void Deactivate(TerrainCollideLayerList* i_list)
		{d_object.Deactivate(i_list);}

	// Override CStageEntity
	virtual uint16 Color(CCollisionPair* i_pair);
	virtual uint8 Material(CCollisionPair* i_pair);
	virtual void Draw(CCollisionEvent* i_event);
	virtual const char* GetClassName(void) {return("Terrain");}

	void Validate(FileStream* i_fs) {d_object.Validate(i_fs);}
	void DrawSuspects(void) {d_object.DrawSuspects();}

  private:
	static const CEntityType s_terrainEntityClass;
	CTerrainCollisionObject d_object;
};

/***************************************************************************/
/***************************************************************************/
inline CTerrainEntity::CTerrainEntity(unsigned int &i_layerMask) :
		CStageEntity(s_terrainEntityClass), d_object(i_layerMask)
{
	d_category |= TERRAIN_CATEGORY;
	d_collisionConfiguration = SIMPLE;
	d_ignoreEntity = NULL;
	d_collisionObject = &d_object;
}

/***************************************************************************/
/***************************************************************************/
inline uint16 CTerrainEntity::Color(CCollisionPair* i_pair)
{
	switch(i_pair->GetIntersectionType())
	{
	 case CCollisionIntersection::LINE_TO_TERRAIN:
		return(((CLineToTerrainIntersection*)i_pair->GetIntersection(
				))->GetFoundPoint()->Color());
	 case CCollisionIntersection::OBB_TO_TERRAIN:
		ASSERT(false);
		break;
	 case CCollisionIntersection::AABB_TO_TERRAIN:
		ASSERT(false);
		break;
	 default:
		break;
	}
	return(0xffff);
}

/***************************************************************************/
/***************************************************************************/
inline uint8 CTerrainEntity::Material(CCollisionPair* i_pair)
{
	switch(i_pair->GetIntersectionType())
	{
	 case CCollisionIntersection::LINE_TO_TERRAIN:
		return(((CLineToTerrainIntersection*)i_pair->GetIntersection(
				))->GetFoundPoint()->Material());
	 case CCollisionIntersection::OBB_TO_TERRAIN:
		ASSERT(false);
		break;
	 case CCollisionIntersection::AABB_TO_TERRAIN:
		ASSERT(false);
		break;
	 default:
		break;
	}
	return(INVALID_MATERIALINDEX);
}

/***************************************************************************/
/***************************************************************************/
inline void CTerrainEntity::Draw(CCollisionEvent* i_event)
{
	CCollisionPair* pair = i_event->GetHeadPair();
	while (pair != NULL)
	{
		switch(pair->GetIntersectionType())
		{
		 case CCollisionIntersection::LINE_TO_TERRAIN:
			((CLineToTerrainIntersection*)pair->GetIntersection())->DrawTerrain();
			break;
		 case CCollisionIntersection::OBB_TO_TERRAIN:
			((CObbToTerrainIntersection*)pair->GetIntersection())->DrawTerrain();
			break;
		 case CCollisionIntersection::AABB_TO_TERRAIN:
			((CAabbToTerrainIntersection*)pair->GetIntersection())->DrawTerrain();
			break;
		 default:
			break;
		}
		pair = pair->GetNext();
	}
}

#endif // TERRAIN_ENTITY_H
