/*
**
**  File:   MeshList.cpp
**  Date:   April 25, 2002
**  By:     Bryant Collard
**  Desc:   Nested instances.
**
**  Copyright (c) 2002, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/EngineNew/Game/Database/MeshList.cpp $
**  $Revision: 17 $
**      $Date: 8/22/03 5:57p $
**    $Author: Bcollard $
**
*/

/******************* includes ****************************/

#include "Game/GamePCH.h"





CMeshList::CMeshList(int i_numMesh, CInstance &i_inst) :
	d_inst(i_inst)
{
	d_maxMesh = i_numMesh;
	if (d_maxMesh <= 0)
		d_mesh = NULL;
	else
		d_mesh = new CInstance[d_maxMesh];

	d_numMesh = 0;
	d_needObjectList = false;
}

CMeshList::~CMeshList()
{
	for (int i = 0; i < d_numMesh; i++)
	{
		SceneMgr::RemoveInstance(&d_mesh[i]);
		d_mesh[i].RemoveBones();
		d_mesh[i].RemoveDisplayList();
		delete (CMeshInstance*)(d_mesh[i].DynamicModel());
		d_mesh[i].SetDynamicModel( NULL );
	}
	delete[] d_mesh;
}

bool CMeshList::AddMesh(SMeshSOM &i_meshSOM, void* i_owner)
{
	if (d_numMesh >= d_maxMesh)
		return(false);

	CMeshInstance* mesh = new CMeshInstance(i_meshSOM, d_inst, NULL, NULL);
	d_mesh[ d_numMesh ].InitFromDynamicObject( mesh );
	d_mesh[ d_numMesh ].SetOwner( (CActor*) i_owner );
	SceneMgr::AddInstance(&d_mesh[d_numMesh]);
	d_numMesh++;

	return(true);
}

void CMeshList::AddInstancesToScene()
{
	for (int i = 0; i < d_numMesh; i++)
	{
		SceneMgr::AddInstance(&d_mesh[i]);
	}
}

void CMeshList::RemoveInstancesFromScene()
{
	for (int i = 0; i < d_numMesh; i++)
	{
		SceneMgr::RemoveInstance(&d_mesh[i]);
	}
}

void CMeshList::RecomputeConstraints(void)
{
	for (int i = 0; i < d_numMesh; i++)
	{
		((CMeshInstance*)(d_mesh[i].DynamicModel()))->RecomputeConstraints();
	}
}

void CMeshList::Reset(void)
{
	for (int i = 0; i < d_numMesh; i++)
	{
		((CMeshInstance*)(d_mesh[i].DynamicModel()))->Reset();
	}
}

void CMeshList::Update(float i_deltaSec)
{
	d_needObjectList = false;
	for (int i = 0; i < d_numMesh; i++)
	{
		d_needObjectList = (d_needObjectList ||
				((CMeshInstance*)(d_mesh[i].DynamicModel()))->Update(i_deltaSec,
				d_mesh[i].RenderedLastFrame()));
		((CMeshInstance*)(d_mesh[i].DynamicModel()))->UpdateBoundingVolume(&d_mesh[i]);
	}
}

void CMeshList::SetObjectList(CObjectList* i_objectList)
{
	for (int i = 0; i < d_numMesh; i++)
	{
		((CMeshInstance*)(d_mesh[i].DynamicModel()))->SetObjectList(i_objectList);
	}
}

void CMeshList::GetExtents(Vector3 &io_minExtent, Vector3 &io_maxExtent,
		bool i_compare)
{
	((CMeshInstance*)(d_mesh[0].DynamicModel()))->GetExtents(io_minExtent,
			io_maxExtent, i_compare);
	for (int i = 0; i < d_numMesh; i++)
	{
		((CMeshInstance*)(d_mesh[0].DynamicModel()))->GetExtents(io_minExtent,
				io_maxExtent, true);
	}
}

void CMeshList::SetDiffuseColor(uint32 i_diffuseColor)
{
	for (int i = 0; i < d_numMesh; i++)
	{
		((CMeshInstance*)(d_mesh[i].DynamicModel()))->SetDiffuseColor(i_diffuseColor);
	}
}

void CMeshList::MakeDummyInstance(CInstance &i_inst)
{
	i_inst.Reset();
	i_inst.SetName( "MeshList Dummy" );
}

void CMeshList::DrawConstraints(void)
{
	for (int i = 0; i < d_numMesh; i++)
	{
		((CMeshInstance*)(d_mesh[i].DynamicModel()))->DrawConstraints();
	}
}

