////////////////////////////////////////////////////////////////////////////
//
// MakeDummyMesh
//
// Class to build a dummy mesh object
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MakeDummyMesh.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:03a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/05/03    Time: 3:07p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * centered bounding box
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/02/02   Time: 4:09p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * added make dummy mesh class
 */

////////////////////////////////////////////////////////////////////////////

#include "PluginsPch.h"
#include "MakeDummyMesh.h"

static const int numDummyPositionVerts = 8; 
static const float dummyPositionVerts[] = 
{
	-0.500000f,-0.500000f,-0.500000f,
	 0.500000f,-0.500000f,-0.500000f,
	-0.500000f,0.500000f,-0.500000f,
	 0.500000f,0.500000f,-0.500000f,
	-0.500000f,-0.500000f,0.500000f,
	 0.500000f,-0.500000f,0.500000f,
	-0.500000f,0.500000f,0.500000f,
	 0.500000f,0.500000f,0.500000f
};

static const int numDummyFaces = 12;

static const int dummyPositionIndices[] =
{
	0,2,3,
	3,1,0,
	4,5,7,
	7,6,4,
	0,1,5,
	5,4,0,
	1,3,7,
	7,5,1,
	3,2,6,
	6,7,3,
	2,0,4,
	4,6,2
};

static const int numDummyNormalVerts = 11;

static const float dummyNormalVerts[] =
{
	0.000000f,0.000000f,-1.000000f,
	0.000000f,0.000000f,1.000000f,
	0.000000f,0.000000f,1.000000f,
	0.000000f,0.000000f,1.000000f,
	0.000000f,-1.000000f,0.000000f,
	1.000000f,0.000000f,0.000000f,
	1.000000f,0.000000f,0.000000f,
	1.000000f,0.000000f,0.000000f,
	0.000000f,1.000000f,0.000000f,
	-1.000000f,0.000000f,0.000000f,
	-1.000000f,0.000000f,0.000000f
};

static const int dummyNormalIndices[] = 
{
	0,0,0,
	0,0,0,
	1,2,1,
	1,3,1,
	4,4,4,
	4,4,4,
	5,6,5,
	5,7,5,
	8,8,8,
	8,8,8,
	9,9,9,
	9,10,9
};

static const int numDummyFaceNormalVerts = 10;
static const float dummyFaceNormalVerts[] = 
{
	0.000000f,0.000000f,-1.000000f,
	0.000000f,0.000000f,1.000000f,
	0.000000f,0.000000f,1.000000f,
	0.000000f,-1.000000f,0.000000f,
	1.000000f,0.000000f,0.000000f,
	1.000000f,0.000000f,0.000000f,
	0.000000f,1.000000f,0.000000f,
	-1.000000f,0.000000f,0.000000f,
	-1.000000f,0.000000f,0.000000f
};

static const int dummyFaceNormalIndices[] = 
{
	0,0,1,
	2,3,3,
	4,5,6,
	6,7,8
};

static const int dummyFaceFlags[] = 
{
	67,67,67,
	67,67,67,
	67,67,67,
	67,67,67
};

static const int dummyFaceMaterialIndices[] = 
{
	0,0,0,
	0,0,0,
	0,0,0,
	0,0,0
};

static const int dummyFaceShadingGroups[] = 
{
	2,2,4,
	4,8,8,
	16,16,32,
	32,64,64
};

void MakeDummyMesh::AddToAgg(TupperwareAggregate *pMeshAgg)
{
	TupObject tupObject(pMeshAgg);
	tupObject.SetObjectType(TupObject::TRI_MESH);
	TupTriMesh tupTriMesh(pMeshAgg);
	tupTriMesh.AddPositionVerts(dummyPositionVerts,numDummyPositionVerts);
	tupTriMesh.AddPositionIndices(dummyPositionIndices,numDummyFaces);
	tupTriMesh.AddNormalVerts(dummyNormalVerts,numDummyNormalVerts);
	tupTriMesh.AddNormalIndices(dummyNormalIndices,numDummyFaces);
	tupTriMesh.AddFaceNormalVerts(dummyFaceNormalVerts,numDummyFaceNormalVerts);
	tupTriMesh.AddFaceNormalIndices(dummyFaceNormalIndices,numDummyFaces);
	tupTriMesh.AddFaceFlags(dummyFaceFlags,numDummyFaces);
	tupTriMesh.AddFaceMaterialIndices(dummyFaceMaterialIndices,numDummyFaces);
	tupTriMesh.AddFaceShadingGroups(dummyFaceShadingGroups,numDummyFaces);
}
