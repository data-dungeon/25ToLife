/*
**
**  File:   Terrain.hpp
**  Date:   January 31, 2003
**  By:     Bryant Collard
**  Desc:   A terrain collision object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/gamelibnew/collide/Terrain.hpp $
**  $Revision: 7 $
**      $Date: 6/04/03 1:41p $
**    $Author: Food $
**
*/

#ifdef TERRAIN_HPP
#undef TERRAIN_HPP

////////////////////////// CTerrainCollisionObject //////////////////////////

/***************************************************************************/
/***************************************************************************/
inline CTerrainCollisionObject::CTerrainCollisionObject(
		unsigned int &i_layerMask) :
	CCollisionObject(TERRAIN),
	d_layerMask(i_layerMask),
	d_dbList(true)
{
	d_numSuspectTriangles = 0;
}

/***************************************************************************/
/***************************************************************************/
inline CTerrainCollisionObject::~CTerrainCollisionObject()
{
}

/***************************************************************************/
/***************************************************************************/
inline void CTerrainCollisionObject::Terminate(void)
{
	d_dbList.RemoveAll();
}

/***************************************************************************/
/***************************************************************************/
inline TerrainCollideLayerList* CTerrainCollisionObject::Activate(
		t_Handle i_handle)
{
	TerrainCollideLayerList* layerList =
			TerrainCollideLayerList::Create(i_handle, d_layerMask);
	if (layerList != NULL)
		d_dbList.AddTail(layerList);
	return(layerList);
}

/***************************************************************************/
/***************************************************************************/
inline void CTerrainCollisionObject::Deactivate(
		TerrainCollideLayerList* i_layerList)
{
	if (i_layerList != NULL)
		d_dbList.Remove(i_layerList);
}

//////////////////////////// CTerrainAABBIterator ///////////////////////////

/***************************************************************************/
/***************************************************************************/
inline bool CTerrainAABBIterator::Initialize(CTerrainCollisionObject &i_terrain)
{
	d_layerList = i_terrain.d_dbList.Head();

	while((d_layerList != NULL) && !d_iterator.Initialize(*d_layerList))
	{
		d_layerList = d_layerList->next;
	}

	return(d_layerList != NULL);
}

/***************************************************************************/
/***************************************************************************/
inline TerrainTriangle* CTerrainAABBIterator::GetNextTriangle(void)
{
	TerrainTriangle* triangle = d_iterator.GetNextTriangle();

	while((triangle == NULL) && ((d_layerList = d_layerList->next) != NULL))
	{
		if (d_iterator.Initialize(*d_layerList))
			triangle = d_iterator.GetNextTriangle();
	}

	#ifdef TERRAIN_TRI_TEST_STATS
	if(triangle)
		triangleCount++;
	#endif

	return(triangle);
}

//////////////////////////// CTerrainOBBIterator ///////////////////////////

/***************************************************************************/
/***************************************************************************/
inline bool CTerrainOBBIterator::Initialize(CTerrainCollisionObject &i_terrain)
{
	d_layerList = i_terrain.d_dbList.Head();

	while((d_layerList != NULL) && !d_iterator.Initialize(*d_layerList))
	{
		d_layerList = d_layerList->next;
	}

	return(d_layerList != NULL);
}

/***************************************************************************/
/***************************************************************************/
inline TerrainTriangle* CTerrainOBBIterator::GetNextTriangle(void)
{
	TerrainTriangle* triangle = d_iterator.GetNextTriangle();

	while((triangle == NULL) && ((d_layerList = d_layerList->next) != NULL))
	{
		if (d_iterator.Initialize(*d_layerList))
			triangle = d_iterator.GetNextTriangle();
	}

	#ifdef TERRAIN_TRI_TEST_STATS
	if(triangle)
		triangleCount++;
	#endif
	
	return(triangle);
}

//////////////////////////// CTerrainLineIterator ///////////////////////////

/***************************************************************************/
/***************************************************************************/
inline bool CTerrainLineIterator::Initialize(CTerrainCollisionObject &i_terrain)
{
	d_layerList = i_terrain.d_dbList.Head();

	while((d_layerList != NULL) && !d_iterator.Initialize(*d_layerList))
	{
		d_layerList = d_layerList->next;
	}

	return(d_layerList != NULL);
}

/***************************************************************************/
/***************************************************************************/
inline TerrainTriangle* CTerrainLineIterator::GetNextTriangle(void)
{
	TerrainTriangle* triangle = d_iterator.GetNextTriangle();

	while((triangle == NULL) && ((d_layerList = d_layerList->next) != NULL))
	{
		if (d_iterator.Initialize(*d_layerList))
			triangle = d_iterator.GetNextTriangle();
	}

	#ifdef TERRAIN_TRI_TEST_STATS
	if(triangle)
		triangleCount++;
	#endif

	return(triangle);
}

#endif // TERRAIN_HPP
