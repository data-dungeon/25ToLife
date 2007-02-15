////////////////////////////////////////////////////////////////////////////
//
// Tupper3dTest
//
// Class to test various parts of hydra
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: Tupper3dTest.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 12:28p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 5  *****************
 * User: Tyler Colbert Date: 6/12/03    Time: 6:09p
 * Updated in $/Avalanche/tools/Tupper3d
 * fixed some vector3 stuff
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/07/01   Time: 9:22a
 * Updated in $/Avalanche/tools/Tupper3d
 * Example of converting a tupperware file from the zup coordinate system
 * (max) to the yup coordinate system (game)
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 10/02/01   Time: 2:54p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 9/24/01    Time: 4:16p
 * Updated in $/Avalanche/tools/Tupper3d
 */

////////////////////////////////////////////////////////////////////////////
#include <assert.h>
#include <stdio.h>
#include "tupper3d/tupper3dtest.h"

#include "tupperware/aggregate.h"
#include "tupperware/loadsave.h"

#include "tupper3d/trimesh.h"
#include "tupper3d/node.h"
#include "tupper3d/transform.h"
#include "tupper3d/staticmatrix.h"
#include "tupper3d/scene.h"
#include "tupper3d/object.h"
#include "tupper3d/objectpool.h"
#include "math/matrix.h"
#include "math/vector.h"
#include "math/mathclass.h"

#undef GetObject

int main( int argc, char** argv )
{
	int result;
	printf("Tupper3dTest - Tupper3d Example that will transform the meshes from z-up to y-up\n");
	if (argc<3)
	{
		printf("Usage: Tupper3dTest infile outfile\n");
		return 1;
	}
	TupperwareAggregate *root;
	printf("Reading %s\n",argv[1]);
	result = TupperwareLoadSaveManager::Read(argv[1],&root);
	if (!result)
	{
		printf("Error Reading %s\n",argv[1]);
		return 1;
	}
	printf("Transforming scene...\n");
	TransformScene(root);
	printf("Saving %s\n",argv[2]);
	result = TupperwareLoadSaveManager::Write(TupperwareLoadSaveManager::ASCII,argv[2],root);
	if (!result)
	{
		printf("Error Writing %s\n",argv[2]);
		return 1;
	}
	return 0;
}

void TransformScene(TupperwareAggregate *pRoot)
{
	TupScene tupScene(pRoot);
	
	TupperwareAggregate *pObjectPoolAgg = tupScene.GetObjectPool();
	assert(pObjectPoolAgg);
	TupObjectPool tupObjectPool(pObjectPoolAgg);
	int total = tupObjectPool.GetNumObjects();
	for (int i=0;i<total;i++)
	{
		TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(i);
		if (pObjectAgg)
		{
			TransformObject(pObjectAgg);
		}
	}
}

void TransformObject(TupperwareAggregate *pObjectAgg)
{
	int i;
	Vector3 *pArray;
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType() == TupObject::TRI_MESH)
	{
		TupTriMesh tupTriMesh(pObjectAgg);
		// do positions
		int numPositionVerts;
		float *pPositionVerts;
		tupTriMesh.GetPositionVerts(&pPositionVerts,numPositionVerts);
		pArray = (Vector3 *)pPositionVerts;
		for (i=0;i<numPositionVerts;i++)
		{
			Vector3 &vector = pArray[i];
			float temp = vector.y();
			vector.y(vector.z());
			vector.z(-temp);
		}
		// do normals
		int numNormalVerts;
		float *pNormalVerts;
		tupTriMesh.GetNormalVerts(&pNormalVerts,numNormalVerts);
		pArray = (Vector3 *)pNormalVerts;
		for (i=0;i<numNormalVerts;i++)
		{
			Vector3 &vector = pArray[i];
			float temp = vector.y();
			vector.y(vector.z());
			vector.z(-vector.y());
		}
		// do face normals
		int numFaceNormalVerts;
		float *pFaceNormalVerts;
		tupTriMesh.GetFaceNormalVerts(&pFaceNormalVerts,numFaceNormalVerts);
		pArray = (Vector3 *)pFaceNormalVerts;
		for (i=0;i<numFaceNormalVerts;i++)
		{
			Vector3 &vector = pArray[i];
			float temp = vector.y();
			vector.y( vector.z());
			vector.z(-vector.y());
		}
	}
}
