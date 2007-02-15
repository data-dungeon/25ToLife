/*
**
**  File:   Terrain.h
**  Date:   January 31, 2003
**  By:     Bryant Collard
**  Desc:   A terrain collision object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/gamelibnew/collide/Terrain.h $
**  $Revision: 12 $
**      $Date: 8/11/03 11:49a $
**    $Author: Food $
**
*/

#ifndef TERRAIN_H
#define TERRAIN_H

// Gamelib includes //
#include "collide/Object.h"
#include "collide/tcllist.h"
#include "container/dllist.h"
#include "collide/tsuspect.h"
#include "Math/Vector.h"

// Forward declarations
class CTerrainAABBIterator;
class CTerrainLineIterator;
class TerrainTriangle;
class CCollisionStack;
class AABB;
class CTerrainSectorInfo;
template<class T> class CCollisionStackArray;
class FileStream;

#define MAX_SUSPECT_TRIANGLES 	512

//#ifndef CDROM
#define TERRAIN_TRI_TEST_STATS
//#endif

////////////////////////// CTerrainCollisionObject //////////////////////////

class CTerrainCollisionObject : public CCollisionObject
{
  public:
	CTerrainCollisionObject(unsigned int &i_layerMask);
	~CTerrainCollisionObject();

	void Terminate(void);
	TerrainCollideLayerList* Activate(t_Handle i_handle);
	void Deactivate(TerrainCollideLayerList* i_list);

	struct SDrawState
	{
		struct SEdge
		{
			Vector3 d_start;
			Vector3 d_end;
		};
		CCollisionStack* d_stack;
		CCollisionStackArray<SEdge>* d_edgeList;
	};
	void InitializeDraw(CCollisionStack* i_stack, SDrawState &o_drawState);
	bool DrawTriangle(TerrainTriangle* i_triangle, SDrawState &io_state);
	void TerminateDraw(SDrawState &i_state);

	void Validate(FileStream* i_fs);
	void DrawSuspects(void);

  private:
	CTerrainCollisionObject();
	bool DrawEdge(Vector3CRef i_start, Vector3CRef i_end, SDrawState &i_state);

	unsigned int& d_layerMask;
	DoublyLinkedList<TerrainCollideLayerList> d_dbList;

	TriangleSuspect::STriangle d_suspectList[MAX_SUSPECT_TRIANGLES];
	u32 d_numSuspectTriangles;

	friend class CTerrainAABBIterator;
	friend class CTerrainOBBIterator;
	friend class CTerrainLineIterator;
};

//////////////////////////// CTerrainAABBIterator ///////////////////////////

class CTerrainAABBIterator
{
  public:
	#ifdef TERRAIN_TRI_TEST_STATS
	static int triangleCount;
	#endif
	
	CTerrainAABBIterator(CCollisionStack *i_stack) : d_iterator(i_stack) {}
	AABB &Bounds(void) {return(d_iterator.Bounds());}
	bool Initialize(CTerrainCollisionObject &i_terrain);
	void Terminate(void) {d_iterator.Terminate();}
	TerrainTriangle* GetNextTriangle(void);

  private:
	TerrainCollideLayerList* d_layerList;
	CTerrainLayerAABBIterator d_iterator;
};

//////////////////////////// CTerrainAABBIterator ///////////////////////////

class CTerrainOBBIterator
{
  public:
	#ifdef TERRAIN_TRI_TEST_STATS
	static int triangleCount;
	#endif
	
	CTerrainOBBIterator(CCollisionStack *i_stack) : d_iterator(i_stack) {}
	OBB &Bounds(void) {return(d_iterator.Bounds());}
	bool Initialize(CTerrainCollisionObject &i_terrain);
	void Terminate(void) {d_iterator.Terminate();}
	TerrainTriangle* GetNextTriangle(void);

  private:
	TerrainCollideLayerList* d_layerList;
	CTerrainLayerOBBIterator d_iterator;
};

//////////////////////////// CTerrainLineIterator ///////////////////////////

class CTerrainLineIterator
{
  public:
	#ifdef TERRAIN_TRI_TEST_STATS
	static int triangleCount;
	#endif
	
	CTerrainLineIterator(CCollisionStack *i_stack) : d_iterator(i_stack) {}
	void Setup(Vector3CRef i_start, Vector3CRef i_end)
		{d_iterator.Setup(i_start, i_end);}
	bool Initialize(CTerrainCollisionObject &i_terrain);
	void Terminate(void) {d_iterator.Terminate();}
	TerrainTriangle* GetNextTriangle(void);
	void FoundEnd(const Vector3 &i_intersection) { d_iterator.FoundEnd(i_intersection); }

  private:
	TerrainCollideLayerList* d_layerList;
	CTerrainLayerLineIterator d_iterator;
};

#define TERRAIN_HPP
#include "collide/Terrain.hpp"

#endif // TERRAIN_H
