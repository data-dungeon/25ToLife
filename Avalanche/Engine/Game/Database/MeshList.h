/*
**
**  File:   MeshList.h
**  Date:   April 25, 2002
**  By:     Bryant Collard
**  Desc:   Nested instances.
**
**  Copyright (c) 2002, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/EngineNew/Game/Database/MeshList.h $
**  $Revision: 14 $
**      $Date: 8/22/03 5:57p $
**    $Author: Bcollard $
**
*/

#ifndef MESH_LIST_H
#define MESH_LIST_H

// Engine includes.
#include "Game/Database/Instance.h"
#include "Math/Vector.h"

// Forward declarations.
struct SMeshSOM;
class CObjectList;

class CMeshList
{
  public:
	CMeshList(int i_numMesh, CInstance &i_inst);
	~CMeshList();
	bool AddMesh(SMeshSOM &i_meshSOM, void* i_owner);
	void AddInstancesToScene();
	void RemoveInstancesFromScene();
	void RecomputeConstraints(void);
	void Reset(void);
	void Update(float i_deltaSec);
	void SetObjectList(CObjectList* i_objectList);
	void GetExtents(Vector3 &io_minExtent, Vector3 &io_maxExtent,
			bool i_compare = false);
	bool NeedObjectList(void) {return(d_needObjectList);}
	int GetNumMeshes(void) { return(d_numMesh); }
	void SetDiffuseColor(uint32 i_diffuseColor);
	CInstance& GetInstance(int i_entry) {return(d_mesh[i_entry]);}
	static void MakeDummyInstance(CInstance &i_inst);
	void DrawConstraints(void);
  private:
	CMeshList();

	CInstance &d_inst;
	int d_numMesh;
	int d_maxMesh;
	CInstance* d_mesh;
	bool d_needObjectList;
};

#endif MESH_LIST_H
