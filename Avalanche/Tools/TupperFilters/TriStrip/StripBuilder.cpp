////////////////////////////////////////////////////////////////////////////
//
// StripBuilder
//
// class to handle building of strips
//
////////////////////////////////////////////////////////////////////////////

#include "TriStripPch.h"
#include <Tupper3d/tupvertex.h>

#pragma warning(disable:4786)
#include <stack>
#include <assert.h>

#include "StripBuilder.h"
#include "parts.h"

int StripBuilder::m_numNonRequestedSwaps;

// for debugging
//#define __CHECKFACES

StripBuilder::StripBuilder(void) :
	m_pPositionVerts(NULL),
	m_pNormalVerts(NULL),
	m_pPositionIndices(NULL),
	m_pNormalIndices(NULL),
	m_pFaceMaterialIndices(NULL),
	m_pFaceFlags(NULL),
	m_pFaceNormalVerts(NULL),
	m_pFaceNormalIndices(NULL),
	m_numPositionVerts(0),
	m_numNormalVerts(0),
	m_numFaces(0),
	m_pFaces(NULL),
	m_faceFlagMask(-1),
	m_pBoneVerts(NULL),
	m_nBonesPerStrip(0),
	m_bAllowSwaps(true),
   m_bOptimizeVertexCache(false),
	m_pUVLimitsArray(NULL),
	m_pFaceUVArray(NULL),
	m_normalThreshold(0.001f),
	m_mapThreshold(0.001f),
	m_vertexColorThreshold(0.01f),
	m_vertexAlphaThreshold(0.01f),
	m_bStripStageFaces(true),
   m_nVertexCacheSize(20),
   m_nVertexCacheStart(13),
	m_pNextStagedFaceIndices(NULL),
	m_name("NONE")
{
	m_pStats = new Stats;
}

StripBuilder::~StripBuilder()
{
	// I own this data therefore delete it
	delete [] m_pFaces;
	delete m_pStats;
}

void StripBuilder::SetNumFaces(int numFaces)
{
	// make sure we have not done this before
	assert(!m_numFaces);
	// make sure there are some faces to do
	assert(numFaces>0);

	m_numFaces = numFaces;
	m_pFaces = new TriFace[numFaces];
}

void StripBuilder::AddPositionVerts(float *pPositionVerts,int numPositionVerts)
{
	// make sure we have not done this before
	assert(!m_pPositionVerts);
	// make sure there are some verts in the list
	assert(numPositionVerts>0);
	// make sure there is some data
	assert(pPositionVerts);
	m_numPositionVerts = numPositionVerts;
	m_pPositionVerts = (Vertex *)pPositionVerts; // just point to the data (faster)
}

void StripBuilder::AddNormalVerts(float *pNormalVerts,int numNormalVerts)
{
	// make sure we have not done this before
	assert(!m_pNormalVerts);
	// make sure there are some verts in the list
	assert(numNormalVerts>0);
	// make sure there is some data
	assert(pNormalVerts);
	m_numNormalVerts = numNormalVerts;
	m_pNormalVerts = (Vertex *)pNormalVerts; // just point to the data (faster)
}

void StripBuilder::AddPositionIndices(int *pPositionIndices)
{
	// make sure we have not done this before
	assert(!m_pPositionIndices);
	// make sure I have set the number of faces
	assert(m_numFaces>0);
	// make sure there is some data
	assert(pPositionIndices);
	m_pPositionIndices = pPositionIndices; // just point to the data (faster)
}

void StripBuilder::AddNormalIndices(int *pNormalIndices)
{
	// make sure we have not done this before
	assert(!m_pNormalIndices);
	// make sure I have set the number of faces
	assert(m_numFaces>0);
	// make sure there is some data
	assert(pNormalIndices);
	m_pNormalIndices = pNormalIndices; // just point to the data (faster)
}

void StripBuilder::AddFaceMaterialIndices(int *pFaceMaterialIndices)
{
	// make sure we have not done this before
	assert(!m_pFaceMaterialIndices);
	// make sure I have set the number of faces
	assert(m_numFaces>0);
	// make sure there is some data
	assert(pFaceMaterialIndices);
	m_pFaceMaterialIndices = pFaceMaterialIndices; // just point to the data (faster)
}

void StripBuilder::AddFaceFlags(int *pFaceFlags)
{
	// make sure we have not done this before
	assert(!m_pFaceFlags);
	// make sure I have set the number of faces
	assert(m_numFaces>0);
	// make sure there is some data
	assert(pFaceFlags);
	m_pFaceFlags = pFaceFlags; // just point to the data (faster)
}

void StripBuilder::AddFaceNormalVerts(float *pFaceNormalVerts,int numFaceNormalVerts)
{
	// make sure we have not done this before
	assert(!m_pFaceNormalVerts);
	// make sure there are some verts in the list
	assert(numFaceNormalVerts>0);
	// make sure there is some data
	assert(pFaceNormalVerts);
	m_numFaceNormalVerts = numFaceNormalVerts;
	m_pFaceNormalVerts = (Vertex *)pFaceNormalVerts; // just point to the data (faster)
}

void StripBuilder::AddFaceNormalIndices(int *pFaceNormalIndices)
{
	// make sure we have not done this before
	assert(!m_pFaceNormalIndices);
	// make sure I have set the number of faces
	assert(m_numFaces>0);
	// make sure there is some data
	assert(pFaceNormalIndices);
	m_pFaceNormalIndices = pFaceNormalIndices; // just point to the data (faster)
}

void StripBuilder::SetFaceFlagMask(int faceFlagMask)
{
	m_faceFlagMask = faceFlagMask;
}

void StripBuilder::AddBoneVerts(int *pBoneVerts,int numBoneVerts)
{
	// make sure we have not done this before
	assert(!m_pBoneVerts);
	// make sure the number of bone verts are the same as the position verts
	assert(numBoneVerts==m_numPositionVerts);
	// make sure there is some data
	assert(pBoneVerts);
	m_pBoneVerts = pBoneVerts;
}

void StripBuilder::SetBonesPerStrip(int nBonesPerStrip)
{
	m_nBonesPerStrip = nBonesPerStrip;
}

void StripBuilder::SetAllowSwaps(bool bAllowSwaps)
{
	m_bAllowSwaps = bAllowSwaps;
}

void StripBuilder::SetOptimizeVertexCache(bool bOptimizeVertexCache)
{
   m_bOptimizeVertexCache = bOptimizeVertexCache;
}

void StripBuilder::SetNormalThreshold(float normalThreshold)
{
	m_normalThreshold = normalThreshold;
}

void StripBuilder::SetMapThreshold(float mapThreshold)
{
	m_mapThreshold = mapThreshold;
}

void StripBuilder::SetVertexColorThreshold(float vertexColorThreshold)
{
	m_vertexColorThreshold = vertexColorThreshold;
}

void StripBuilder::SetVertexAlphaThreshold(float vertexAlphaThreshold)
{
	m_vertexAlphaThreshold = vertexAlphaThreshold;
}

void StripBuilder::SetStripStageFaces(bool bStripStageFaces)
{
	m_bStripStageFaces = bStripStageFaces;
}

void StripBuilder::SetVertexCacheSize(int nVertexCacheSize)
{
   m_nVertexCacheSize = nVertexCacheSize;
}

void StripBuilder::SetVertexCacheStart(int nVertexCacheStart)
{
   m_nVertexCacheStart = nVertexCacheStart;
}


// max strip pixels stuff
void StripBuilder::AddUVLimitsArray(TupArray<TupArray<Vector3> > &uvLimitsArray)
{
	m_pUVLimitsArray = &uvLimitsArray;
}

void StripBuilder::AddFaceUVArray(TupArray<TupArray<Vector3> > &faceUVArray)
{
	m_pFaceUVArray = &faceUVArray;
}

void StripBuilder::AddMeshMap(float *pMapVerts,int numMapVerts,int *pMapIndices)
{
	// should not be null
	assert(pMapVerts);
	// should not be null
	assert(pMapIndices);
	// make sure there are some verts
	assert(numMapVerts>0);
	// make sure I have set the number of faces
	assert(m_numFaces>0);

	int oldSize = m_meshMaps.GetSize();
	m_meshMaps.SetSize(oldSize+1);
	int newIndex = oldSize;
	m_meshMaps[newIndex].AddMapVerts(pMapVerts,numMapVerts);
	m_meshMaps[newIndex].AddMapIndices(pMapIndices);
}

void StripBuilder::AddVertexColorMap(float *pColorVerts,int numColorVerts,int *pColorIndices)
{
	// should not be null
	assert(pColorVerts);
	// should not be null
	assert(pColorIndices);
	// make sure there are some verts
	assert(numColorVerts>0);
	// make sure I have set the number of faces
	assert(m_numFaces>0);

	int oldSize = m_vertexColorMaps.GetSize();
	m_vertexColorMaps.SetSize(oldSize+1);
	int newIndex = oldSize;
	m_vertexColorMaps[newIndex].AddColorVerts(pColorVerts,numColorVerts);
	m_vertexColorMaps[newIndex].AddColorIndices(pColorIndices);
}

void StripBuilder::AddVertexAlphaMap(float *pColorVerts,int numColorVerts,int *pColorIndices)
{
	// should not be null
	assert(pColorVerts);
	// should not be null
	assert(pColorIndices);
	// make sure there are some verts
	assert(numColorVerts>0);
	// make sure I have set the number of faces
	assert(m_numFaces>0);

	int oldSize = m_vertexAlphaMaps.GetSize();
	m_vertexAlphaMaps.SetSize(oldSize+1);
	int newIndex = oldSize;
	m_vertexAlphaMaps[newIndex].AddColorVerts(pColorVerts,numColorVerts);
	m_vertexAlphaMaps[newIndex].AddColorIndices(pColorIndices);
}

void StripBuilder::AddName(const char *pName)
{
	m_name = pName;
}

void StripBuilder::AddNextStagedFaceIndices(int *pNextStagedFaceIndices)
{
	// make sure we have not done this before
	assert(!m_pNextStagedFaceIndices);
	// make sure I have set the number of faces
	assert(m_numFaces>0);
	m_pNextStagedFaceIndices = pNextStagedFaceIndices; // just point to the data (faster)
}

const StripBuilder::Stats &StripBuilder::GetStats(void) const
{
	return *m_pStats;
}

void StripBuilder::ComputeStrips(void)
{
	m_pStats->Init();
	ConnectFaces(); // connect edges of faces together

	BuildConnectedFaceGroups(); // calculate the number of connected face groups

//#define __CHECKFACES
#ifdef __CHECKFACES
	CheckFaces();
#endif
	BuildPaths();
	PeelPaths();
	FixPeeledRoots();
#ifdef __CHECKFACES
	CheckStrips();
#endif
	BuildStripsFromPaths();
#ifdef __CHECKFACES
	CheckStrips();
#endif
	BuildStripBones();
	BuildStripUVMinMax();
	ComputeStripNumPoints();
	CombineStrips();
#ifdef __CHECKFACES
	CheckStrips();
#endif
	BuildPointStrips();
	AddStagedFaceStrips();
	SortStrips();
	BuildSortedStripStats();
#ifdef __CHECKFACES
	CheckPointStrips();
#endif
}

void StripBuilder::ConnectFaces(void)
{
	// construct an array of edge link pointers (edges from the triangles will be connect to this array)
	// this array uses the vertex position index as the hash
	FaceEdgeLink **pFaceEdgeLinkPtrs = new FaceEdgeLink *[m_numPositionVerts];
	// clear pointers 
	for (int vertIndex=0;vertIndex<m_numPositionVerts;vertIndex++)
	{
		pFaceEdgeLinkPtrs[vertIndex] = NULL;
	}
	// construct an array of edge link data (each face gets 3)
	FaceEdgeLink *pFaceEdgeLinks = new FaceEdgeLink[m_numFaces*3];

	int *pCurrentFacePositionIndices = m_pPositionIndices; // pointer used to access the position indices for the current face
	FaceEdgeLink *pCurrentFaceEdgeLinks = pFaceEdgeLinks; // pointer used to access the face edge links for the current face
	// loop through the faces and add edges for each into the array
	for (int faceIndex=0;faceIndex<m_numFaces;faceIndex++,pCurrentFacePositionIndices+=3,pCurrentFaceEdgeLinks+=3)
	{
		// if this face is a stage or hidden then don't connect to any other face and ignore at this stage of the tristripper
		if (m_pFaceFlags[faceIndex] & (TupTriMesh::FLAGS_FACE_STAGE_MASK|TupTriMesh::FLAGS_FACE_HIDDEN))
		{
			TriFace &triFace = m_pFaces[faceIndex];
			int flags = triFace.GetFlags();
			triFace.SetFlags(flags|TriFace::VISITED);
			continue;
		}

		for (int triPoint=0;triPoint<3;triPoint++)
		{
			// get current index
			int index = pCurrentFacePositionIndices[triPoint];
			// get index of the other point on the edge
			int endPointIndex = triPoint<2 ? pCurrentFacePositionIndices[triPoint+1] : pCurrentFacePositionIndices[0];
			// set up face edge link for this edge
			FaceEdgeLink &edgeLink = pCurrentFaceEdgeLinks[triPoint];

			edgeLink.SetEdge(Edge(faceIndex,triPoint)); // set up edge (cache for easy access)
			edgeLink.SetNextLink(pFaceEdgeLinkPtrs[index]); // set to link into the current face edge link pointer array
			edgeLink.SetEndPointIndex(endPointIndex); // set up index for end point (cache for easy access)
			// hook to pointer array
			pFaceEdgeLinkPtrs[index] = &edgeLink;
		}
	}
	// go through the list and hook the faces together
	for (vertIndex=0;vertIndex<m_numPositionVerts;vertIndex++)
	{
		FaceEdgeLink *pCurrentLink = pFaceEdgeLinkPtrs[vertIndex];
		while (pCurrentLink)
		{
			int faceIndex1 = pCurrentLink->GetEdge().GetFaceIndex();
			int pointIndex1 = pCurrentLink->GetEdge().GetPointIndex();
			Edge &edge1 = m_pFaces[faceIndex1].GetEdge(pointIndex1);
			// if this has not been set yet
			if (edge1.GetFaceIndex()==-1)
			{
				// scan through the list on the end point to find a match
				FaceEdgeLink *pOtherPointLink = pFaceEdgeLinkPtrs[pCurrentLink->GetEndPointIndex()];
				while (pOtherPointLink)
				{
					// if this links to our edge
					if (pOtherPointLink->GetEndPointIndex()==vertIndex)
					{
						int faceIndex2 = pOtherPointLink->GetEdge().GetFaceIndex();
						int pointIndex2 = pOtherPointLink->GetEdge().GetPointIndex();
						Edge &edge2 = m_pFaces[faceIndex2].GetEdge(pointIndex2);

						// if this has not been set yet
						if (edge2.GetFaceIndex()==-1)
						{
							int stageFaceIndex1 = faceIndex1;
							int stageFaceIndex2 = faceIndex2;
							bool bConnectFaces = false;
							// loop through all possible stages
							while (1)
							{
								// check the normals, materials, flags, uv coordiates, etc.
								if (m_pFaceMaterialIndices[stageFaceIndex1]!=m_pFaceMaterialIndices[stageFaceIndex2])
								{              
									m_pStats->m_connectionBreakMaterialIndex++;
									break; // material indices don't match
								}
								if ((m_pFaceFlags[stageFaceIndex1]&m_faceFlagMask)!=(m_pFaceFlags[stageFaceIndex2]&m_faceFlagMask))
								{
									m_pStats->m_connectionBreakFaceFlags++;
									break; // face flags don't match
								}

								int face1IndicesIndex1 = (stageFaceIndex1<<1)+stageFaceIndex1+pointIndex1;
								int face1IndicesIndex2 = (stageFaceIndex1<<1)+stageFaceIndex1+((pointIndex1<2) ? pointIndex1+1 : 0);

								int face2IndicesIndex1 = (stageFaceIndex2<<1)+stageFaceIndex2+pointIndex2;
								int face2IndicesIndex2 = (stageFaceIndex2<<1)+stageFaceIndex2+((pointIndex2<2) ? pointIndex2+1 : 0);

								if (m_pNormalVerts)
								{
									// check and make sure the normals are the same
									Vertex &face1Normal1 = m_pNormalVerts[m_pNormalIndices[face1IndicesIndex1]];
									Vertex &face1Normal2 = m_pNormalVerts[m_pNormalIndices[face1IndicesIndex2]];
									Vertex &face2Normal1 = m_pNormalVerts[m_pNormalIndices[face2IndicesIndex1]];
									Vertex &face2Normal2 = m_pNormalVerts[m_pNormalIndices[face2IndicesIndex2]];
									if (!face1Normal1.Compare(face2Normal2,m_normalThreshold) ||
										!face1Normal2.Compare(face2Normal1,m_normalThreshold))
									{
										m_pStats->m_connectionBreakVertexNormals++;
										break;
									}
								}

								// check and make sure the maps are the same
								bool mapOk = true;
								int numMeshMaps = m_meshMaps.GetSize();
								for (int meshMapIndex = 0;meshMapIndex<numMeshMaps;meshMapIndex++)
								{
									MeshMap &meshMap = m_meshMaps[meshMapIndex];
									int *pMapIndices = meshMap.GetMapIndices();
									Vertex *pMapVerts = meshMap.GetMapVerts();
									Vertex &face1Vert1 = pMapVerts[pMapIndices[face1IndicesIndex1]];
									Vertex &face1Vert2 = pMapVerts[pMapIndices[face1IndicesIndex2]];
									Vertex &face2Vert1 = pMapVerts[pMapIndices[face2IndicesIndex1]];
									Vertex &face2Vert2 = pMapVerts[pMapIndices[face2IndicesIndex2]];
									if (!face1Vert1.Compare(face2Vert2,m_mapThreshold) ||
										!face1Vert2.Compare(face2Vert1,m_mapThreshold))
									{
										mapOk = false;
										break;
									}
								}
								// kick out if not ok
								if (!mapOk)
								{
									m_pStats->m_connectionBreakMeshMaps++;
									break;
								}

								// check and make sure the vertex colors are the same
								bool vertexColorsOk = true;
								int numVertexColorMaps = m_vertexColorMaps.GetSize();
								for (int vertexColorMapIndex = 0;vertexColorMapIndex<numVertexColorMaps;vertexColorMapIndex++)
								{
									VertexColorMap &vertexColorMap = m_vertexColorMaps[vertexColorMapIndex];
									int *pVertexColorIndices = vertexColorMap.GetColorIndices();
									Vertex *pColorVerts = vertexColorMap.GetColorVerts();
									Vertex &face1Vert1 = pColorVerts[pVertexColorIndices[face1IndicesIndex1]];
									Vertex &face1Vert2 = pColorVerts[pVertexColorIndices[face1IndicesIndex2]];
									Vertex &face2Vert1 = pColorVerts[pVertexColorIndices[face2IndicesIndex1]];
									Vertex &face2Vert2 = pColorVerts[pVertexColorIndices[face2IndicesIndex2]];
									if (!face1Vert1.Compare(face2Vert2,m_vertexColorThreshold) ||
										!face1Vert2.Compare(face2Vert1,m_vertexColorThreshold))
									{
										vertexColorsOk = false;
										break;
									}
								}
								// kick out if not ok
								if (!vertexColorsOk)
								{
									m_pStats->m_connectionBreakVertexColors++;
									break;
								}

								// check and make sure the vertex alphas are the same
								bool vertexAlphasOk = true;
								int numVertexAlphaMaps = m_vertexAlphaMaps.GetSize();
								for (int vertexAlphaMapIndex = 0;vertexAlphaMapIndex<numVertexAlphaMaps;vertexAlphaMapIndex++)
								{
									VertexColorMap &vertexAlphaMap = m_vertexAlphaMaps[vertexAlphaMapIndex];
									int *pVertexAlphaIndices = vertexAlphaMap.GetColorIndices();
									Vertex *pAlphaVerts = vertexAlphaMap.GetColorVerts();
									Vertex &face1Vert1 = pAlphaVerts[pVertexAlphaIndices[face1IndicesIndex1]];
									Vertex &face1Vert2 = pAlphaVerts[pVertexAlphaIndices[face1IndicesIndex2]];
									Vertex &face2Vert1 = pAlphaVerts[pVertexAlphaIndices[face2IndicesIndex1]];
									Vertex &face2Vert2 = pAlphaVerts[pVertexAlphaIndices[face2IndicesIndex2]];
									if (!face1Vert1.Compare(face2Vert2,m_vertexAlphaThreshold) ||
										!face1Vert2.Compare(face2Vert1,m_vertexAlphaThreshold))
									{
										vertexAlphasOk = false;
										break;
									}
								}
								// kick out if not ok
								if (!vertexAlphasOk)
								{
									m_pStats->m_connectionBreakVertexAlphas++;
									break;
								}


								if (m_pNextStagedFaceIndices)
								{
									// face 1 does not have a next stage
									if (m_pNextStagedFaceIndices[stageFaceIndex1] == -1)
									{
										// face 2 does not have a next stage
										if (m_pNextStagedFaceIndices[stageFaceIndex2] == -1)
										{
											bConnectFaces = true;
											break;
										}
										// face 2 has a next stage
										else
										{
											m_pStats->m_connectionBreakStages++;
											break; // no connections 
										}
									}
									// face 1 has a next stage
									else
									{
										// face 2 does not have a next stage
										if (m_pNextStagedFaceIndices[stageFaceIndex2] == -1)
										{
											m_pStats->m_connectionBreakStages++;
											break; // no connections
										}
										// face 2 has a next stage
										else
										{
											// point to next stage and loop again
											stageFaceIndex1 = m_pNextStagedFaceIndices[stageFaceIndex1];
											stageFaceIndex2 = m_pNextStagedFaceIndices[stageFaceIndex2];
										}
									}
								}
								else
								{
									bConnectFaces = true;
									break;
								}
							}
							if (bConnectFaces)
							{
								m_pStats->m_totalConnections++;
								// link together
								edge1.SetFaceIndex(faceIndex2);
								edge1.SetPointIndex(pointIndex2);

								edge2.SetFaceIndex(faceIndex1);
								edge2.SetPointIndex(pointIndex1);
							}
							break; // don't look for another one
						}
					}
					pOtherPointLink = pOtherPointLink->GetNextLink();
				}
			}
			pCurrentLink = pCurrentLink->GetNextLink();
		}
	}
	delete [] pFaceEdgeLinkPtrs; // delete the pointer array
	delete [] pFaceEdgeLinks; // delete the edge link array
}

void StripBuilder::BuildConnectedFaceGroups(void)
{
	m_connectedFaceGroups.SetSize(m_numFaces);
	int faceIndex;
	for (faceIndex = 0; faceIndex < m_numFaces; faceIndex++)
	{
		m_connectedFaceGroups[faceIndex] = -1;
	}

	std::stack<int> searchStack;

	int currentConnectedGroupNum = 0;
	m_connectedFaceGroupOffsets.Add(currentConnectedGroupNum); // first group starts at 0

	TriFace *pFace = m_pFaces; // this pointer moves through the faces
	for (int faceIndex=0;faceIndex<m_numFaces;faceIndex++,pFace++)
	{
		int flags = pFace->GetFlags();
		if ((m_connectedFaceGroups[faceIndex] == -1) && !(flags&TriFace::VISITED))
		{
			m_connectedFaceGroups[faceIndex] = currentConnectedGroupNum;
			m_connectedFaces.Add(faceIndex);

			searchStack.push(faceIndex);

			while(!searchStack.empty())
			{
				// get last item off the stack
				int fillFaceIndex = searchStack.top(); 
				searchStack.pop();
				TriFace &face = m_pFaces[fillFaceIndex];

				for (int triPoint=0;triPoint<3;triPoint++)
				{
					Edge &edge = face.GetEdge(triPoint);
					int adjacentFaceIndex = edge.GetFaceIndex();
					// active link?
					if (adjacentFaceIndex!=-1)
					{
						int adjacentFaceFlags = m_pFaces[adjacentFaceIndex].GetFlags();
						if ((m_connectedFaceGroups[adjacentFaceIndex] == -1) && !(adjacentFaceFlags&TriFace::VISITED))
						{
							m_connectedFaceGroups[adjacentFaceIndex] = currentConnectedGroupNum;
							m_connectedFaces.Add(adjacentFaceIndex);
							searchStack.push(adjacentFaceIndex);
						}
					}
				}
			}
			m_connectedFaceGroupOffsets.Add(m_connectedFaces.GetSize());
			currentConnectedGroupNum++;
		}
	}
	// pull off last one since it is not needed
	m_connectedFaceGroupOffsets.SetSize(m_connectedFaceGroupOffsets.GetSize()-1);
	m_pStats->m_totalFaceConnectionGroups = m_connectedFaceGroupOffsets.GetSize();
}

// make sure the faces are connected together properly
void StripBuilder::CheckFaces(void)
{
	for (int faceIndex=0;faceIndex<m_numFaces;faceIndex++)
	{
		TriFace &face = m_pFaces[faceIndex];
		for (int edgeIndex=0;edgeIndex<3;edgeIndex++)
		{
			Edge &edge = face.GetEdge(edgeIndex);
			int nextFaceIndex = edge.GetFaceIndex();
			int nextFacePoint = edge.GetPointIndex();
			if (nextFaceIndex!=-1)
			{
				TriFace &nextFace = m_pFaces[nextFaceIndex];
				assert(nextFace.GetEdge(nextFacePoint).GetFaceIndex()==faceIndex);
				assert(nextFace.GetEdge(nextFacePoint).GetPointIndex()==edgeIndex);
			}
		}
	}
}

void StripBuilder::BuildPaths(void)
{
	// keep trying until no more faces
	while (1)
	{
		// loop through the faces and try to find the best one to do
		int bestFace=-1;
		int bestFaceNeighbors;
		TriFace *pFace = m_pFaces; // this pointer moves through the faces
		for (int faceIndex=0;faceIndex<m_numFaces;faceIndex++,pFace++)
		{
			int flags = pFace->GetFlags();
			if (!(flags&TriFace::VISITED))
			{
				int neighborCount = 0;
				for (int triPoint=0;triPoint<3;triPoint++)
				{
					Edge &edge = pFace->GetEdge(triPoint);
					int adjacentFaceIndex = edge.GetFaceIndex();
					// active link?
					if (adjacentFaceIndex!=-1)
						neighborCount++;
				}
				// if we found a face with 1 or no neighbors
				if (neighborCount<2)
				{
					// can't do better than this...
					bestFace = faceIndex;
					break; // lets do this one
				}
				// if we have 2 or more neighbors then lets keep the best one at least
				if (bestFace==-1 || (neighborCount<bestFaceNeighbors))
				{
					bestFace = faceIndex;
					bestFaceNeighbors = neighborCount;
				}
			}
		}
		// stop if there is no faces left
		if (bestFace==-1)
			break;

		// use best face and build the path
		TriFace &triFace = m_pFaces[bestFace];
		// add face to array (keeps track of all path roots)
		m_buildPathRoots.Add(bestFace);
		// mark as visited
		int flags = triFace.GetFlags();
		triFace.SetFlags(flags|TriFace::VISITED);
		// start with no splits
		int numSplits = 0;
		// start with no children
		int numChildren = 0;
		// if we have bone verts then we need to find the best face that uses the same bones as me
		Keeper<int> usedBones;
		if (m_pBoneVerts)
		{
			int baseIndex = bestFace+(bestFace<<1);
			for (int pointIndex=0;pointIndex<3;pointIndex++)
			{
				usedBones.Add(m_pBoneVerts[m_pPositionIndices[baseIndex+pointIndex]]);
			}
		}
      VertexCache usedVerts(m_nVertexCacheSize);
      if (m_bAllowSwaps && m_bOptimizeVertexCache)
      {
         int baseIndex = bestFace+(bestFace<<1);
         for (int pointIndex=0;pointIndex<3;pointIndex++)
         {
            usedVerts.AddValue(m_pPositionIndices[baseIndex+pointIndex]);
         }
      }
		TupArray<TupArray<MinMaxUV> > minMaxUVArray;
		// max strip pixels stuff
		if (m_pFaceUVArray)
		{
			TupArray<int> stagedFaces;
			GetStagedFaces(bestFace,stagedFaces);
			int numStages = stagedFaces.GetSize();
			minMaxUVArray.SetSize(numStages);
			for (int stageIndex=0;stageIndex<numStages;stageIndex++)
			{
				int stageFace = stagedFaces[stageIndex];
				int stageFace3 = stageFace+(stageFace<<1);
				int materialIndex = m_pFaceMaterialIndices[stageFace];

				TupArray<Vector3> &limitsArray = (*m_pUVLimitsArray)[materialIndex];
				int numLayers = (*m_pFaceUVArray)[stageFace3].GetSize();
				minMaxUVArray[stageIndex].SetSize(numLayers);
				for (int layerIndex=0;layerIndex<numLayers;layerIndex++)
				{
					MinMaxUV &minMaxUV = minMaxUVArray[stageIndex][layerIndex];
					for (int pointIndex=0;pointIndex<3;pointIndex++)
					{
						minMaxUV.AddCoor((*m_pFaceUVArray)[stageFace3+pointIndex][layerIndex]);
					}
				}
			}
		}
		// find a the best face that is connected to this one
		while (1)
		{
			int bestPoint = -1;
			int bestWeight;
			for (int triPoint=0;triPoint<3;triPoint++)
			{
				Edge &edge = triFace.GetEdge(triPoint);
				int adjacentFaceIndex = edge.GetFaceIndex();
				// active link?
				if (adjacentFaceIndex!=-1)
				{
					TriFace &adjacentFace = m_pFaces[adjacentFaceIndex];
					int adjacentFaceFlags =  adjacentFace.GetFlags();
					// if our neighbor has not been visited then do the search through this neighbor
					if (!(adjacentFaceFlags&TriFace::VISITED))
					{
						int weight = 0;
						// if we are checking bones
						if (m_pBoneVerts)
						{
							int baseIndex = adjacentFaceIndex+(adjacentFaceIndex<<1);
							int adjacentPointIndex = edge.GetPointIndex(); 
							int prevPoint = adjacentPointIndex>0 ? adjacentPointIndex-1 : 2;
							int boneIndex = m_pBoneVerts[m_pPositionIndices[baseIndex+prevPoint]];
							// if the new face is not adding a new bone
							if (usedBones.GetIndex(boneIndex)!=-1)
							{
								weight += 100;
							}
							// if this one will add a new bone but there is no room
							else if (usedBones.GetSize()==m_nBonesPerStrip)
							{
								m_pStats->m_buildPathBreaksBones++;
								continue; // try the next edge
							}
						}
						// max strip pixels stuff
						if (m_pFaceUVArray)
						{
							TupArray<int> stagedFaces;
							GetStagedFaces(adjacentFaceIndex,stagedFaces);
							int numStages = stagedFaces.GetSize();
							assert(minMaxUVArray.GetSize() == numStages);
							bool bToBig = false;
							int adjacentPointIndex = edge.GetPointIndex(); 
							int prevPoint = adjacentPointIndex>0 ? adjacentPointIndex-1 : 2;
							for (int stageIndex=0;stageIndex<numStages;stageIndex++)
							{
								int stageFace = stagedFaces[stageIndex];
								int stageFace3 = stageFace+(stageFace<<1);
								int materialIndex = m_pFaceMaterialIndices[stageFace];

								TupArray<Vector3> &limitsArray = (*m_pUVLimitsArray)[materialIndex];
								int numLayers = (*m_pFaceUVArray)[stageFace3].GetSize();
								assert(minMaxUVArray[stageIndex].GetSize()==numLayers);
								for (int layerIndex=0;layerIndex<numLayers;layerIndex++)
								{
									MinMaxUV &minMaxUV = minMaxUVArray[stageIndex][layerIndex];
									if (!minMaxUV.TestCoor((*m_pFaceUVArray)[stageFace3+prevPoint][layerIndex],limitsArray[layerIndex]))
									{
										bToBig = true;
										break;
									}
								}
								if (bToBig)
								{
									break; // if to big then stop
								}
							}
							if (bToBig)
							{
								m_pStats->m_buildPathBreaksUVLimit++;
								continue; // if to big then skip this edge
							}
						}
						// TODO - add other tests that will change the weight
						if (bestPoint==-1 || bestWeight<weight)
						{
							bestPoint = triPoint;
							bestWeight = weight;
						}
					}
				}
			}
			// if we found one
			if (bestPoint!=-1)
			{
				Edge &edge = triFace.GetEdge(bestPoint);
				int adjacentFaceIndex = edge.GetFaceIndex();
				assert(adjacentFaceIndex!=-1);
				TriFace &adjacentFace = m_pFaces[adjacentFaceIndex];
				int adjacentFaceFlags =  adjacentFace.GetFlags();
				assert(!(adjacentFaceFlags&TriFace::VISITED));

            int baseIndex = adjacentFaceIndex+(adjacentFaceIndex<<1);
            int adjacentPointIndex = edge.GetPointIndex(); 
            int prevPoint = adjacentPointIndex>0 ? adjacentPointIndex-1 : 2;
            int nextPoint = adjacentPointIndex<2 ? adjacentPointIndex+1 : 0;

				// keep track of bones used
				if (m_pBoneVerts)
				{
					int boneIndex = m_pBoneVerts[m_pPositionIndices[baseIndex+prevPoint]];
					// if the new face is not adding a new bone
					usedBones.Add(boneIndex);
					assert(usedBones.GetSize()<=m_nBonesPerStrip);
				}
            // if we are checking for optimizing for vertex cache
            if (m_bAllowSwaps && m_bOptimizeVertexCache)
            {
               int vertIndex = m_pPositionIndices[baseIndex+prevPoint];
               usedVerts.AddValue(vertIndex);
            }

				// keep track of uv limits
				if (m_pFaceUVArray)
				{
					TupArray<int> stagedFaces;
					GetStagedFaces(adjacentFaceIndex,stagedFaces);
					int numStages = stagedFaces.GetSize();
					assert(minMaxUVArray.GetSize() == numStages);
					for (int stageIndex=0;stageIndex<numStages;stageIndex++)
					{
						int stageFace = stagedFaces[stageIndex];
						int stageFace3 = stageFace+(stageFace<<1);
						int materialIndex = m_pFaceMaterialIndices[stageFace];

						TupArray<Vector3> &limitsArray = (*m_pUVLimitsArray)[materialIndex];
						int numLayers = (*m_pFaceUVArray)[stageFace3].GetSize();
						assert(minMaxUVArray[stageIndex].GetSize()==numLayers);
						for (int layerIndex=0;layerIndex<numLayers;layerIndex++)
						{
							MinMaxUV &minMaxUV = minMaxUVArray[stageIndex][layerIndex];
							minMaxUV.AddCoor((*m_pFaceUVArray)[stageFace3+prevPoint][layerIndex]);
						}
					}
				}

				adjacentFace.SetFlags(adjacentFaceFlags|TriFace::VISITED);
				triFace.EnablePath(bestPoint); // set the appropriate bits for a path to my neighbor
				numChildren++; // add this child to the count
				// search down through the connected faces to this neighbor
				numSplits += BuildPathSearch(adjacentFaceIndex,nextPoint,prevPoint,usedBones,usedVerts,minMaxUVArray);
				// set my neighbor's parent to the current face (leave -1 for the DepthFirstSearch call)
				adjacentFace.SetPathParentFace(bestFace);
			}
			else
			{
				break;
			}
		}
		triFace.SetPathNumSplits(numSplits);
	}
}

// performs a depth first search without recursive calls (stack does not like this)
// returns the number of faces (descendants as well as the seed face) that have 2 children
int StripBuilder::BuildPathSearch(int faceIndex,int nextPointIndex,int prevPointIndex,Keeper<int> &usedBones,VertexCache &usedVerts,TupArray<TupArray<MinMaxUV> > &minMaxUVArray)
{
	std::stack<BuildPathStackEntry> searchStack;
	int numSplits = 0;

	BuildPathStackEntry stackEntry(faceIndex,nextPointIndex,prevPointIndex,false);
	searchStack.push(stackEntry);

	while(!searchStack.empty())
	{
		// get last item off the stack
		BuildPathStackEntry &entry = searchStack.top(); 
		TriFace &face = m_pFaces[entry.GetFaceIndex()];
		int stage = entry.GetStage();
		// set next stage here 
		entry.SetStage(stage+1);
		// mimick a recursive situation (next, prev, pass child stages)

		switch (stage)
		{
         case 1: // prev check
			case 0: // next check
				{
               // only do one path through if optimizing for vertex cache
               if (m_bAllowSwaps && m_bOptimizeVertexCache && stage == 1)
                  break;

					int bestPoint = -1;
					int bestWeight;
					// find the best face to search through
					for (int pointCount=0;pointCount<2;pointCount++)
					{
						int pointIndex = pointCount==0 ? entry.GetNextPointIndex() : entry.GetPrevPointIndex();
						Edge &edge = face.GetEdge(pointIndex);
						int adjacentFaceIndex = edge.GetFaceIndex();
						// active link?
						if (adjacentFaceIndex!=-1)
						{
							TriFace &adjacentFace = m_pFaces[adjacentFaceIndex];
							int adjacentFaceFlags =  adjacentFace.GetFlags();
							if (!(adjacentFaceFlags&TriFace::VISITED))
							{
								int weight = 0;
								if (pointCount == 0)
								{
									weight += 50; // optimal edge has greater weight
								}

                        int baseIndex = adjacentFaceIndex+(adjacentFaceIndex<<1);
                        int adjacentPointIndex = edge.GetPointIndex(); 
                        int prevPoint = adjacentPointIndex>0 ? adjacentPointIndex-1 : 2;

								if (m_pBoneVerts)
								{
									int boneIndex = m_pBoneVerts[m_pPositionIndices[baseIndex+prevPoint]];
									// if the new face is not adding a new bone
									if (usedBones.GetIndex(boneIndex)!=-1)
									{
										weight += 60; // if the needed bone will fit add some weight
									}
									// if this one will add a new bone but there is no room
									else if (usedBones.GetSize()==m_nBonesPerStrip)
									{
										m_pStats->m_buildPathBreaksBones++;
										continue; // try the next edge
									}
								}
                        if (m_bAllowSwaps && m_bOptimizeVertexCache )
                        {
                           int vertIndex = m_pPositionIndices[baseIndex+prevPoint];
                           // if new vert is already in cache
                           if (usedVerts.InCache(vertIndex))
                           {
                              weight += 300;
                           }
                           else
                           {
                              if (usedVerts.GetNumValues()>=m_nVertexCacheStart)
                              {
                                 int lookAheadDepth = SearchAheadForCacheHit(usedVerts,adjacentFaceIndex,adjacentPointIndex,3);
                                 if (lookAheadDepth == 2)
                                 {
                                    weight += 200;
                                 }
                                 if (lookAheadDepth == 3)
                                 {
                                    weight += 100;
                                 }
                              }
                           }
                        }
								// max strip pixel stuff
								if (m_pFaceUVArray)
								{
									TupArray<int> stagedFaces;
									GetStagedFaces(adjacentFaceIndex,stagedFaces);
									int numStages = stagedFaces.GetSize();
									assert(minMaxUVArray.GetSize() == numStages);
									bool bToBig = false;
									for (int stageIndex=0;stageIndex<numStages;stageIndex++)
									{
										int stageFace = stagedFaces[stageIndex];
										int stageFace3 = stageFace+(stageFace<<1);
										int materialIndex = m_pFaceMaterialIndices[stageFace];

										TupArray<Vector3> &limitsArray = (*m_pUVLimitsArray)[materialIndex];
										int numLayers = (*m_pFaceUVArray)[stageFace3].GetSize();
										assert(minMaxUVArray[stageIndex].GetSize()==numLayers);
										for (int layerIndex=0;layerIndex<numLayers;layerIndex++)
										{
											MinMaxUV &minMaxUV = minMaxUVArray[stageIndex][layerIndex];
											if (!minMaxUV.TestCoor((*m_pFaceUVArray)[stageFace3+prevPoint][layerIndex],limitsArray[layerIndex]))
											{
												bToBig = true;
												break;
											}
										}
										if (bToBig)
										{
											break; // if to big then stop
										}
									}
									if (bToBig)
									{
										m_pStats->m_buildPathBreaksUVLimit++;
										continue; // if to big then skip this edge
									}
								}

								// add other tests that will change the weight
								if (bestPoint==-1 || bestWeight<weight)
								{
									bestPoint = pointCount;
									bestWeight = weight;
								}
							}
						}
					}
					// continue with the best point
					if (bestPoint!=-1)
					{
						int pointIndex = bestPoint==0 ? entry.GetNextPointIndex() : entry.GetPrevPointIndex();
						Edge &edge = face.GetEdge(pointIndex);
						int adjacentFaceIndex = edge.GetFaceIndex();
						TriFace &adjacentFace = m_pFaces[adjacentFaceIndex];
						assert(adjacentFaceIndex!=-1);

                  int baseIndex = adjacentFaceIndex+(adjacentFaceIndex<<1);
                  int adjacentPointIndex = edge.GetPointIndex(); 
                  int prevPoint = adjacentPointIndex>0 ? adjacentPointIndex-1 : 2;
                  int nextPoint = adjacentPointIndex<2 ? adjacentPointIndex+1 : 0;

						// keep track of bones used
						if (m_pBoneVerts)
						{
							int boneIndex = m_pBoneVerts[m_pPositionIndices[baseIndex+prevPoint]];
							// if the new face is not adding a new bone
							usedBones.Add(boneIndex);
							assert(usedBones.GetSize()<=m_nBonesPerStrip);
						}
                  // keep track of verts used
                  if (m_bAllowSwaps && m_bOptimizeVertexCache)
                  {
                     int vertIndex = m_pPositionIndices[baseIndex+prevPoint];
                     usedVerts.AddValue(vertIndex);
                  }
						// keep track of uv limits
						if (m_pFaceUVArray)
						{
							TupArray<int> stagedFaces;
							GetStagedFaces(adjacentFaceIndex,stagedFaces);
							int numStages = stagedFaces.GetSize();
							assert(minMaxUVArray.GetSize() == numStages);
							for (int stageIndex=0;stageIndex<numStages;stageIndex++)
							{
								int stageFace = stagedFaces[stageIndex];
								int stageFace3 = stageFace+(stageFace<<1);
								int materialIndex = m_pFaceMaterialIndices[stageFace];

								TupArray<Vector3> &limitsArray = (*m_pUVLimitsArray)[materialIndex];
								int numLayers = (*m_pFaceUVArray)[stageFace3].GetSize();
								assert(minMaxUVArray[stageIndex].GetSize()==numLayers);
								for (int layerIndex=0;layerIndex<numLayers;layerIndex++)
								{
									MinMaxUV &minMaxUV = minMaxUVArray[stageIndex][layerIndex];
									minMaxUV.AddCoor((*m_pFaceUVArray)[stageFace3+prevPoint][layerIndex]);
								}
							}
						}

						// mark as visited
						int adjacentFaceFlags =  adjacentFace.GetFlags();
						assert(!(adjacentFaceFlags&TriFace::VISITED));
						adjacentFace.SetFlags(adjacentFaceFlags|TriFace::VISITED);
						face.EnablePath(pointIndex);
						adjacentFace.SetPathParentFace(entry.GetFaceIndex());
						if (entry.GetParity())
						{
							BuildPathStackEntry adjacentEntry(adjacentFaceIndex,nextPoint,prevPoint,!entry.GetParity());
							searchStack.push(adjacentEntry);
						}
						else
						{
							BuildPathStackEntry adjacentEntry(adjacentFaceIndex,prevPoint,nextPoint,!entry.GetParity());
							searchStack.push(adjacentEntry);
						}
					}
				}
				break;
			case 2: // done
				{
					// count how many children I have
					for (int pathEdge=0,pathCount=0;pathEdge<3;pathEdge++)
					{
						if (face.PathEnabled(pathEdge))
							pathCount++;
					}
					int pathParent = face.GetPathParentFace();
					int faceSplitCount = face.GetPathNumSplits()+(pathCount>1 ? 1 : 0);
					if (pathCount>1)
						face.SetFlags(face.GetFlags()|TriFace::SPLIT); // mark as a split face (more than one path)
					if (pathParent!=-1)
					{
						TriFace &parentFace = m_pFaces[pathParent];
						parentFace.SetPathNumSplits(parentFace.GetPathNumSplits()+faceSplitCount);
					}
					else
					{
						numSplits += faceSplitCount;
					}
					searchStack.pop();
				}
				break;
		}
	}
	return numSplits;
}

int StripBuilder::SearchAheadForCacheHit(const VertexCache &usedVerts,int faceIndex,int pointIndex,int maxDepth)
{
   Keeper<int> usedFaces;
   return SearchAheadForCacheHit(usedVerts,faceIndex,pointIndex,usedFaces,1,maxDepth);
}

int StripBuilder::SearchAheadForCacheHit(const VertexCache &usedVerts,int faceIndex,int pointIndex,const Keeper<int> &usedFacesSource,int currentDepth,int maxDepth)
{
   TriFace &face = m_pFaces[faceIndex];
   Edge &baseEdge = face.GetEdge(pointIndex);

   int baseIndex = faceIndex+(faceIndex<<1);
   int prevPoint = pointIndex>0 ? pointIndex-1 : 2;

   int vertIndex = m_pPositionIndices[baseIndex+prevPoint];
   // do we have a cache hit?
   if (usedVerts.InCache(vertIndex))
   {
      return currentDepth;
   }
   currentDepth++;
   if (currentDepth>maxDepth)
   {
      return -1; // not found here
   }

   // make a copy of the currently used faces and add faceIndex
   Keeper<int> usedFaces(usedFacesSource);
   usedFaces.Add(faceIndex);

   // try adjacent face 1
   int tryPoint1Depth = -1;
   int tryPointIndex1 = pointIndex<2 ? pointIndex+1 : 0;
   Edge &tryEdge1 = face.GetEdge(tryPointIndex1);
   int tryFaceIndex1 = tryEdge1.GetFaceIndex();
   // face is linked and not currently used
   if (tryFaceIndex1 != -1 && usedFaces.GetIndex(tryFaceIndex1)==-1)
   {
      TriFace &tryFace1 = m_pFaces[tryFaceIndex1];
      int tryFaceFlags1 =  tryFace1.GetFlags();
      // make sure not already marked
      if (!(tryFaceFlags1&TriFace::VISITED))
      {
         tryPoint1Depth = SearchAheadForCacheHit(usedVerts,tryFaceIndex1,tryEdge1.GetPointIndex(),usedFaces,currentDepth,maxDepth);
      }
   }

   // try adjacent face 2
   int tryPoint2Depth = -1;
   int tryPointIndex2 = pointIndex>0 ? pointIndex-1 : 2;
   Edge &tryEdge2 = face.GetEdge(tryPointIndex2);
   int tryFaceIndex2 = tryEdge2.GetFaceIndex();
   // face is linked and not currently used
   if (tryFaceIndex2 != -1 && usedFaces.GetIndex(tryFaceIndex2)==-1)
   {
      TriFace &tryFace2 = m_pFaces[tryFaceIndex2];
      int tryFaceFlags2 =  tryFace2.GetFlags();
      // make sure not already marked
      if (!(tryFaceFlags2&TriFace::VISITED))
      {
         tryPoint2Depth = SearchAheadForCacheHit(usedVerts,tryFaceIndex2,tryEdge2.GetPointIndex(),usedFaces,currentDepth,maxDepth);
      }
   }

   // return the best of the two depths
   if (tryPoint1Depth!=-1)
   {
      if (tryPoint2Depth!=-1)
      {
         return (tryPoint1Depth<tryPoint2Depth) ? tryPoint1Depth : tryPoint2Depth;
      }
      return tryPoint1Depth;
   }
   if (tryPoint2Depth!=-1)
   {
      return tryPoint2Depth;
   }
   // return -1 if none found
   return -1;
}

void StripBuilder::PeelPaths(void)
{
	int numPathRoots = m_buildPathRoots.GetSize();
	for (int rootIndex=0;rootIndex<numPathRoots;rootIndex++)
	{
		int faceIndex = m_buildPathRoots[rootIndex];
		PeelPathSearch(faceIndex);
	}
}

// performs a depth first search without recursive calls (stack does not like this)
void StripBuilder::PeelPathSearch(int faceIndex)
{
	std::stack<PeelPathStackEntry> searchStack;
	int numSplits = 0;

	PeelPathStackEntry stackEntry(faceIndex);
	searchStack.push(stackEntry);

	while(!searchStack.empty())
	{
		// get last item off the stack
		PeelPathStackEntry &entry = searchStack.top(); 
		faceIndex = entry.GetFaceIndex();
		TriFace &face = m_pFaces[faceIndex];
		int pathParent = face.GetPathParentFace();

		// if there are no splits beneath me
		if (face.GetPathNumSplits()<=face.GetPathSplitCredit())
		{
			// count how many children I have
			for (int pathEdge=0,pathCount=0;pathEdge<3;pathEdge++)
			{
				if (face.PathEnabled(pathEdge))
					pathCount++;
			}
			if (pathCount>1)
			{
				// if there are 3 paths from this root
				if (pathCount==3)
				{
					// peel one of the three paths from the root and keep the other 2
					// do first one (it does not matter)
					Edge &peelEdge = face.GetEdge(0);
					int adjacentFaceIndex = peelEdge.GetFaceIndex();
					// active link?
					assert(adjacentFaceIndex!=-1);
					TriFace &adjacentFace = m_pFaces[adjacentFaceIndex];
					adjacentFace.SetPathParentFace(-1); // kill parent link
					face.DisablePath(0); // kill path link
					m_peeledRoots.Add(adjacentFaceIndex); // add to peeled roots array
				}
				m_peeledRoots.Add(faceIndex); // add to peeled roots array
				if (pathParent!=-1)
				{
					TriFace &parentFace = m_pFaces[pathParent];
					// tell our parent about myself and my children's credits
					int splitCredit = face.GetPathSplitCredit()+1;
					parentFace.SetPathSplitCredit(parentFace.GetPathSplitCredit()+splitCredit);
					face.SetPathParentFace(-1); // sever link to parent
					// find which edge points to me
					for (int parentPoint=0;parentPoint<3;parentPoint++)
					{
						if (parentFace.GetEdge(parentPoint).GetFaceIndex()==faceIndex)
						{
							// sever link from my parent to me
							parentFace.DisablePath(parentPoint);
							break;
						}
					}
				}
			}
			// if I have only one or no children then try to go up the tree
			else
			{
				// if I am a root (can't go up)
				if (pathParent==-1)
				{
					m_peeledRoots.Add(faceIndex); // add to peeled roots array
				}
				// If I have a parent then tell it about my credits
				else
				{
					// add one if I was originally a split
					int splitCredit = face.GetPathSplitCredit()+((face.GetFlags()&TriFace::SPLIT)?1:0);
					TriFace &parentFace = m_pFaces[pathParent];
					parentFace.SetPathSplitCredit(parentFace.GetPathSplitCredit()+splitCredit);
				}
			}
			searchStack.pop(); // go up or done
		}
		// if there are some splits underneath me
		else
		{
			// try the edge using the stage
			int stage = entry.GetStage();
			assert(stage<3);
			// set next stage here 
			entry.SetStage(stage+1);
			int pointIndex = stage; // use the stage for which child to look at
			// try this child
			if (face.PathEnabled(stage))
			{
				Edge &edge = face.GetEdge(pointIndex);
				PeelPathStackEntry stackEntry(edge.GetFaceIndex());
				searchStack.push(stackEntry);
			}
		}
	}
}

// this routine with take a peeled root that has 2 children and fix the links on one side of the tree to use
// the last child as the new root.
void StripBuilder::FixPeeledRoots(void)
{
	int numPeeledRoots = m_peeledRoots.GetSize();
	for (int rootIndex=0;rootIndex<numPeeledRoots;rootIndex++)
	{
		int faceIndex = m_peeledRoots[rootIndex];
		TriFace *pFace = &m_pFaces[faceIndex];
		int childPath1=0;
		int childPath2=0;
		int childFaceAvoid=-1; // this is the face to avoid (makes sure we are going in the same direction)
		// loop until we find the end of one of the paths
		while (1)
		{
			for (int pathEdge=0,pathCount=0;pathEdge<3;pathEdge++)
			{
				if (pFace->PathEnabled(pathEdge))
				{
					childPath2 = childPath1; // keep track of last child path
					childPath1 = pathEdge; // keep track of last child path
					pathCount++;
				}
			}
			assert(pathCount<3); // better not have 3 childern
			if (pathCount==2) // if it has 2 children
			{
				// if going down childPath1 is hitting our last face
				int childFaceIndex = pFace->GetEdge(childPath1).GetFaceIndex();
				assert(childFaceIndex!=-1);
				// if we picked the wrong one
				if (childFaceIndex==childFaceAvoid)
				{
					// go down other path (childPath2)
					childPath1 = childPath2; // use the childpath1 variable as the path we are going down
					childFaceIndex = pFace->GetEdge(childPath2).GetFaceIndex();
					assert(childFaceIndex!=-1);
				}
				childFaceAvoid = faceIndex; // set for next time
				pFace->DisablePath(childPath1); // disable path from face to child
				pFace->SetPathParentFace(childFaceIndex); // child now becomes a parent to face

				TriFace *pChildFace = &m_pFaces[childFaceIndex];
				pChildFace->SetPathParentFace(-1); // this is now a root
				// find the edge on the child face that links to the parent face
				for (int edgeIndex=0;edgeIndex<3;edgeIndex++)
				{
					if (pChildFace->GetEdge(edgeIndex).GetFaceIndex()==faceIndex)
					{
						// child now has a path to face
						pChildFace->EnablePath(edgeIndex);
						break;
					}
				}
				// face is now the child
				faceIndex = childFaceIndex;
				pFace = pChildFace;
			}
			else
			{
				// make the root in the table the face we ended up with
				m_peeledRoots[rootIndex] = faceIndex;
				break;
			}
		}
	}
}

void StripBuilder::BuildStripsFromPaths(void)
{
	// we may add new entires to the m_peeledRoots array
	for (int rootIndex=0;rootIndex<m_peeledRoots.GetSize();rootIndex++)
	{
		int stripIndex = m_strips.GetSize(); // this is the strip we are

		int startFaceIndex = m_peeledRoots[rootIndex];
		TriFace &startFace = m_pFaces[startFaceIndex];
		startFace.SetStripIndex(stripIndex);
		int startFaceFlags = startFace.GetFlags();
		int secondToFirstEdge;
		int secondFaceIndex;
		switch (startFaceFlags&TriFace::PATHS)
		{
			// no child paths
			case 0:
				secondFaceIndex = -1;
				break;
			case TriFace::PATH_0:
				secondFaceIndex = startFace.GetEdge(0).GetFaceIndex();
				secondToFirstEdge = startFace.GetEdge(0).GetPointIndex();
				break;
			case TriFace::PATH_1:
				secondFaceIndex = startFace.GetEdge(1).GetFaceIndex();
				secondToFirstEdge = startFace.GetEdge(1).GetPointIndex();
				break;
			case TriFace::PATH_2:
				secondFaceIndex = startFace.GetEdge(2).GetFaceIndex();
				secondToFirstEdge = startFace.GetEdge(2).GetPointIndex();
				break;
			default:
				assert(false);
		}
		if (secondFaceIndex==-1)
		{
			m_pStats->m_buildStripFromPathOneFace++;
			// we have a strip with one face
			Strip newStrip(startFaceIndex,startFaceIndex,1);
			m_strips.Add(newStrip);
			continue;
		}
		TriFace &secondFace = m_pFaces[secondFaceIndex];
		secondFace.SetStripIndex(stripIndex);
		int secondFaceFlags = secondFace.GetFlags();
		int thirdFaceIndex;
		int secondToThirdEdge,thirdToSecondEdge;
		switch (secondFaceFlags&TriFace::PATHS)
		{
			// no child paths
			case 0:
				thirdFaceIndex = -1;
				break;
			case TriFace::PATH_0:
				thirdFaceIndex = secondFace.GetEdge(0).GetFaceIndex();
				thirdToSecondEdge = secondFace.GetEdge(0).GetPointIndex();
				secondToThirdEdge = 0;
				break;
			case TriFace::PATH_1:
				thirdFaceIndex = secondFace.GetEdge(1).GetFaceIndex();
				thirdToSecondEdge = secondFace.GetEdge(1).GetPointIndex();
				secondToThirdEdge = 1;
				break;
			case TriFace::PATH_2:
				thirdFaceIndex = secondFace.GetEdge(2).GetFaceIndex();
				thirdToSecondEdge = secondFace.GetEdge(2).GetPointIndex();
				secondToThirdEdge = 2;
				break;
			default:
				assert(false);
		}
		if (thirdFaceIndex==-1)
		{
			// we have a strip with 2 faces
			m_pStats->m_buildStripFromPathTwoFace++;
			Strip newStrip(startFaceIndex,secondFaceIndex,2);
			m_strips.Add(newStrip);
			continue;
		}
		TriFace &thirdFace = m_pFaces[thirdFaceIndex];
		int rightHandEdge = secondToFirstEdge<2 ? secondToFirstEdge+1 : 0;
		bool nextEdgeRight = true; // default to next needing to be right hand
		if (secondToThirdEdge==rightHandEdge)
		{
			nextEdgeRight = false; // if our third edge is to the right then we need to look for the left
		}
		// hook any additional faces to the first 3
		int numStripFaces = 3; // start with 3 faces in strip
		int testFaceIndex = thirdFaceIndex;
		TriFace *pTestFace = &thirdFace;
		int testToPrevEdge = thirdToSecondEdge;
		while(1)
		{
			pTestFace->SetStripIndex(stripIndex); // this belongs to the strip
			int testFaceFlags = pTestFace->GetFlags();
			int nextFaceIndex;
			int testToNextEdge,nextToTestEdge;
			switch (testFaceFlags&TriFace::PATHS)
			{
				// no child paths
				case 0:
					nextFaceIndex = -1;
					break;
				case TriFace::PATH_0:
					nextFaceIndex = pTestFace->GetEdge(0).GetFaceIndex();
					nextToTestEdge = pTestFace->GetEdge(0).GetPointIndex();
					testToNextEdge = 0;
					break;
				case TriFace::PATH_1:
					nextFaceIndex = pTestFace->GetEdge(1).GetFaceIndex();
					nextToTestEdge = pTestFace->GetEdge(1).GetPointIndex();
					testToNextEdge = 1;
					break;
				case TriFace::PATH_2:
					nextFaceIndex = pTestFace->GetEdge(2).GetFaceIndex();
					nextToTestEdge = pTestFace->GetEdge(2).GetPointIndex();
					testToNextEdge = 2;
					break;
				default:
					assert(false);
			}
			// if we ran out of faces
			if (nextFaceIndex==-1)
			{
				Strip newStrip(startFaceIndex,testFaceIndex,numStripFaces);
				m_strips.Add(newStrip);
				break;
			}
			else 
			{
				bool bContinueOK = true; // default to keep going through the faces

				// which edge does the next one need to link to?
				int desiredNextEdge = nextEdgeRight ? 
					((testToPrevEdge<2) ? testToPrevEdge+1 : 0) :
					((testToPrevEdge>0) ? testToPrevEdge-1 : 2);

				// if the next face cannot be added to the current strip
				if (!m_bAllowSwaps && testToNextEdge!=desiredNextEdge)
				{
					m_pStats->m_buildStripFromPathBadEdge++;
					bContinueOK = false; // can't continue
				}
				// if we currently have an odd number of strips (we are going to add one to be even)
				// we need to check if the next one is ok as well so we can avoid an even numbered strip
				// An even numbered strip may have an unavoidable swap contained within it
				// odd numbered ones can be reversed to fix the swap problem
				if (!m_bAllowSwaps && numStripFaces&1 && bContinueOK)
				{
					TriFace *pTestNextFace = &m_pFaces[nextFaceIndex];
					int testNextToPrevEdge = nextToTestEdge;
					int testNextEdgeRight = !nextEdgeRight; // alternate left/right

					int testNextFaceFlags = pTestNextFace->GetFlags();
					int testNextNextFaceIndex;
					int testNextToNextNextEdge;
					switch (testNextFaceFlags&TriFace::PATHS)
					{
						// no child paths
						case 0:
							testNextNextFaceIndex = -1;
							break;
						case TriFace::PATH_0:
							testNextNextFaceIndex = pTestNextFace->GetEdge(0).GetFaceIndex();
							testNextToNextNextEdge = 0;
							break;
						case TriFace::PATH_1:
							testNextNextFaceIndex = pTestNextFace->GetEdge(1).GetFaceIndex();
							testNextToNextNextEdge = 1;
							break;
						case TriFace::PATH_2:
							testNextNextFaceIndex = pTestNextFace->GetEdge(2).GetFaceIndex();
							testNextToNextNextEdge = 2;
							break;
						default:
							assert(false);
					}
					if (testNextNextFaceIndex==-1)
					{
						m_pStats->m_buildStripFromPathBadEvenStrip++;
						bContinueOK = false;
					}
					else
					// if we have a next next face
					{
						// which edge does the next one need to link to?
						int desiredNextEdge = testNextEdgeRight ? 
							((testNextToPrevEdge<2) ? testNextToPrevEdge+1 : 0) :
							((testNextToPrevEdge>0) ? testNextToPrevEdge-1 : 2);

						// if the next next face cannot be added to the current strip
						if (testNextToNextNextEdge!=desiredNextEdge)
						{
							m_pStats->m_buildStripFromPathBadEvenStripEdge++;
							bContinueOK = false;
						}

					}
				}
				// we need to stop for whatever reasom
				if (!bContinueOK)
				{
					// sever path between test and next faces
					TriFace &nextFace = m_pFaces[nextFaceIndex];
					nextFace.SetPathParentFace(-1); // make it a root
					pTestFace->DisablePath(testToNextEdge);
					// add another root
					m_peeledRoots.Add(nextFaceIndex);
					// this strip is complete
					Strip newStrip(startFaceIndex,testFaceIndex,numStripFaces);
					m_strips.Add(newStrip);
					break;
				}
			}
			// set up for next face
			pTestFace = &m_pFaces[nextFaceIndex];
			testFaceIndex = nextFaceIndex;
			testToPrevEdge = nextToTestEdge;
			nextEdgeRight = !nextEdgeRight; // alternate left/right
			numStripFaces++;
		}
	}
}

// set the number of points for each strip
void StripBuilder::ComputeStripNumPoints(void)
{
	if (m_bAllowSwaps) // if we can do swaps then don't bother
		return;

	int totalStrips = m_strips.GetSize();
	// optimal pass (each merge will save 2 points)
	for (int stripIndex=0;stripIndex<totalStrips;stripIndex++)
	{
		Strip &strip = m_strips[stripIndex];
		if (!(strip.GetFlags()&Strip::REMOVED))
		{
			ComputeStripNumPoints(strip);
		}
	}
}

// this will set up the number of actual points the strip is using
// and will also reverse the strip if needed
void StripBuilder::ComputeStripNumPoints(Strip &strip)
{
	PointStrip pointStrip;
	BuildPointStrip(strip,pointStrip);
	int numPoints = pointStrip.GetNumPoints();
	int numSwaps = numPoints - (pointStrip.GetNumTriangles()+2);
	// if the strip has swaps we need to check the reversal
	if (numSwaps)
	{
		ReverseStrip(strip);
		BuildPointStrip(strip,pointStrip);
		int numReversedPoints = pointStrip.GetNumPoints();
		// if we are better off with the reversed strip
		if (numReversedPoints<numPoints)
		{
			numPoints = numReversedPoints;			
			// don't reverse back (great the way it is now)
		}
		else
		{
			ReverseStrip(strip);
		}
	}
	strip.SetNumPoints(numPoints);
}

void StripBuilder::SaveStrip(const Strip &strip,SavedStrip &savedStrip) const
{
	savedStrip.SetStrip(strip);

	int startFaceIndex = strip.GetStartFace();
	int currentFaceIndex = startFaceIndex;
	
	while(currentFaceIndex!=-1)
	{
		TriFace &triFace = m_pFaces[currentFaceIndex];
		savedStrip.AddFace(currentFaceIndex,triFace);

		int nextFaceIndex;
		
		int faceFlags = triFace.GetFlags();
		switch (faceFlags&TriFace::PATHS)
		{
				// last one in strip
			case 0:
				nextFaceIndex=-1; // we are done
				break;
			case TriFace::PATH_0:
				nextFaceIndex = triFace.GetEdge(0).GetFaceIndex();
				break;
			case TriFace::PATH_1:
				nextFaceIndex = triFace.GetEdge(1).GetFaceIndex();
				break;
			case TriFace::PATH_2:
				nextFaceIndex = triFace.GetEdge(2).GetFaceIndex();
				break;
			default:
				assert(false);
		}
		currentFaceIndex=nextFaceIndex;
	}
}

void StripBuilder::RestoreStrip(Strip &strip,const SavedStrip &savedStrip)
{
	strip = savedStrip.GetStrip();
	int numFaces = savedStrip.GetNumFaces();
	for (int index=0;index<numFaces;index++)
	{
		int faceIndex = savedStrip.GetFaceIndex(index);
		m_pFaces[faceIndex] = savedStrip.GetFace(index);
	}
}

// Merge strip 2 into strip1
// the type is how the ends are to be hooked up
// will return if it was able to merge due to the threshold value
// threshold = 2 - will not allow any new swaps to be made
// threshold = 1 - will 1 new swap to be made
// threshold = 0 - will 2 new swaps to be made
bool StripBuilder::MergeStrips(Strip &strip1,Strip &strip2,CombineStripsType combineStripsType,int threshold)
{
	if (!m_bAllowSwaps)
	{
		// save info about both strips to restore later if we cannot merge them together due to threshold issues
		SavedStrip savedStrip1,savedStrip2;
		SaveStrip(strip1,savedStrip1);
		SaveStrip(strip2,savedStrip2);

		// remember the number of swaps for the strips before the merge
		int numSwaps1 = strip1.GetNumPoints()-(strip1.GetNumFaces()+2);
		int numSwaps2 = strip2.GetNumPoints()-(strip2.GetNumFaces()+2);

		switch (combineStripsType)
		{
			case START_TO_END: // start of strip2 to end of strip1
				MergeStripsSimple(strip1,strip2);
				break;
			case END_TO_START: // end of strip2 to start of strip1
				ReverseStrip(strip1);
				ReverseStrip(strip2);
				MergeStripsSimple(strip1,strip2);
				break;
			case END_TO_END: // end of strip2 to end of strip1
				ReverseStrip(strip2);
				MergeStripsSimple(strip1,strip2);
				break;
			case START_TO_START: // start of strip2 to start of strip1
				ReverseStrip(strip1);
				MergeStripsSimple(strip1,strip2);
				break;
		}
		ComputeStripNumPoints(strip1);
		
		int numSwaps = strip1.GetNumPoints()-(strip1.GetNumFaces()+2);
		int numAddedSwaps = numSwaps-(numSwaps1+numSwaps2);
		if (numAddedSwaps>2)
			numAddedSwaps = 2;
		// if the merge was within the threshold
		if (numAddedSwaps<3-threshold)
		{
			int startFaceIndex = strip1.GetStartFace();
			TriFace &startFace = m_pFaces[startFaceIndex];
			int strip1Index = startFace.GetStripIndex();
			
			// set up the bones used for the merged strip
			if (m_pBoneVerts)
			{
				BuildStripBones(strip1Index);
			}
			if (m_pFaceUVArray)
			{
				BuildStripUVMinMax(strip1Index);
			}
			return true;
		}
		else
		{
			// if we didn't return then we need to restore stuff back and return false
			RestoreStrip(strip1,savedStrip1);
			RestoreStrip(strip2,savedStrip2);
			return false;
		}
	}
	// we allow swaps (don't bother with the true swaps it just slows things down and gives worse results)
	else
	{
		switch (combineStripsType)
		{
		case START_TO_END: // start of strip2 to end of strip1
			MergeStripsSimple(strip1,strip2);
			break;
		case END_TO_START: // end of strip2 to start of strip1
			ReverseStrip(strip1);
			ReverseStrip(strip2);
			MergeStripsSimple(strip1,strip2);
			break;
		case END_TO_END: // end of strip2 to end of strip1
			ReverseStrip(strip2);
			MergeStripsSimple(strip1,strip2);
			break;
		case START_TO_START: // start of strip2 to start of strip1
			ReverseStrip(strip1);
			MergeStripsSimple(strip1,strip2);
			break;
		}
		int startFaceIndex = strip1.GetStartFace();
		TriFace &startFace = m_pFaces[startFaceIndex];
		int strip1Index = startFace.GetStripIndex();
		
		// set up the bones used for the merged strip
		if (m_pBoneVerts)
		{
			BuildStripBones(strip1Index);
		}
		if (m_pFaceUVArray)
		{
			BuildStripUVMinMax(strip1Index);
		}
		return true;
	}
}

// Merge strip2 into strip1
// the start of strip2 is merged to the end of strip1
// this function will set up the optimal links again for the new strip
void StripBuilder::MergeStripsSimple(Strip &strip1,Strip &strip2)
{
	int endFaceIndex = strip1.GetEndFace();
	TriFace &endFace = m_pFaces[endFaceIndex];
	int startFaceIndex = strip2.GetStartFace();
	TriFace &startFace = m_pFaces[startFaceIndex];

	for (int edgeIndex=0;edgeIndex<3;edgeIndex++)
	{
		if (endFace.GetEdge(edgeIndex).GetFaceIndex() == startFaceIndex)
			break;
	}
	assert(edgeIndex<3); // better have found it
	endFace.EnablePath(edgeIndex); // link end1 to start2
	startFace.SetPathParentFace(endFaceIndex); // link start2 to end1
	int currentFaceIndex = startFaceIndex;
	int strip1Index = endFace.GetStripIndex(); // get strip index from face
	// set up the strip index on strip2 to be the same as strip1
	while(currentFaceIndex!=-1)
	{
		TriFace &face = m_pFaces[currentFaceIndex];
		face.SetStripIndex(strip1Index);
		int faceFlags = face.GetFlags();
		switch (faceFlags&TriFace::PATHS)
		{
			// last one in strip2
			case 0:
				// mark strip with new end face
				strip1.SetEndFace(currentFaceIndex);
				currentFaceIndex=-1; // we are done
				break;
			case TriFace::PATH_0:
				currentFaceIndex = face.GetEdge(0).GetFaceIndex();
				break;
			case TriFace::PATH_1:
				currentFaceIndex = face.GetEdge(1).GetFaceIndex();
				break;
			case TriFace::PATH_2:
				currentFaceIndex = face.GetEdge(2).GetFaceIndex();
				break;
			default:
				assert(false);
		}
	}
	strip2.SetFlags(strip2.GetFlags()|Strip::REMOVED); // mark as removed
	strip1.SetNumFaces(strip1.GetNumFaces()+strip2.GetNumFaces()); // update the number of faces
}

// this function will reverse the direction of the strip
// this function does not fix any optimal links
void StripBuilder::ReverseStrip(Strip &strip)
{
	int startFaceIndex = strip.GetStartFace();
	int endFaceIndex = strip.GetEndFace();

	TriFace *pFace = &m_pFaces[startFaceIndex];
	int faceFlags = pFace->GetFlags(); // get this first
	pFace->ClearPaths(); // this one should not have any paths now
	int nextFaceIndex=startFaceIndex;
	int nextEdgeIndex; // which edge to go back to parent (before reversal)
	while(nextFaceIndex!=-1)
	{
		// the flags need to float ahead since we are messing with the paths
		switch (faceFlags&TriFace::PATHS)
		{
			// last one in strip
			case 0:
				nextFaceIndex=-1; // we are done
				break;
			case TriFace::PATH_0:
				nextFaceIndex = pFace->GetEdge(0).GetFaceIndex();
				nextEdgeIndex = pFace->GetEdge(0).GetPointIndex();
				break;
			case TriFace::PATH_1:
				nextFaceIndex = pFace->GetEdge(1).GetFaceIndex();
				nextEdgeIndex = pFace->GetEdge(1).GetPointIndex();
				break;
			case TriFace::PATH_2:
				nextFaceIndex = pFace->GetEdge(2).GetFaceIndex();
				nextEdgeIndex = pFace->GetEdge(2).GetPointIndex();
				break;
			default:
				assert(false);
		}
		pFace->SetPathParentFace(nextFaceIndex); // point to my new parent
		if (nextFaceIndex!=-1)
		{
			// link to new child
			pFace = &m_pFaces[nextFaceIndex];
			faceFlags = pFace->GetFlags(); // set this before we stomp on them
			pFace->ClearPaths();
			pFace->EnablePath(nextEdgeIndex); // and set to this one
		}
	}
	strip.SetStartFace(endFaceIndex);
	strip.SetEndFace(startFaceIndex);
}

// given 3 linked faces, find the next one following the strip pattern
int StripBuilder::GetOptimalFaceInPattern(int face1,int face2,int face3)
{
	TriFace &secondFace = m_pFaces[face2];
	int secondToThirdEdge=-1;
	int secondToFirstEdge=-1;
	int thirdToSecondEdge;
	int thirdToNextEdge;
	for (int edgeIndex = 0;edgeIndex<3;edgeIndex++)
	{
		Edge &edge = secondFace.GetEdge(edgeIndex);
		if (edge.GetFaceIndex()==face1)
			secondToFirstEdge = edgeIndex;
		if (edge.GetFaceIndex()==face3)
		{
			secondToThirdEdge = edgeIndex;
			thirdToSecondEdge = edge.GetPointIndex();
		}
	}
	// these better be linked!
	assert(secondToThirdEdge!=-1);
	assert(secondToFirstEdge!=-1);

	int rightHandEdge = secondToFirstEdge<2 ? secondToFirstEdge+1 : 0;
	// if face 2 to 3 is right hand
	if (secondToThirdEdge==rightHandEdge)
	{
		// next is a left
		thirdToNextEdge = thirdToSecondEdge>0 ? thirdToSecondEdge-1 : 2;
	}
	else
	{
		// next is a right
		thirdToNextEdge = thirdToSecondEdge<2 ? thirdToSecondEdge+1 : 0;
	}
	// return face on the other side of this edge on face3
	return m_pFaces[face3].GetEdge(thirdToNextEdge).GetFaceIndex();
}

// given 3 linked faces, find the next one that is nest to the face following the strip pattern
int StripBuilder::GetAlternateFaceInPattern(int face1,int face2,int face3)
{
	TriFace &secondFace = m_pFaces[face2];
	int secondToThirdEdge=-1;
	int secondToFirstEdge=-1;
	int thirdToSecondEdge;
	int thirdToNextEdge;
	for (int edgeIndex = 0;edgeIndex<3;edgeIndex++)
	{
		Edge &edge = secondFace.GetEdge(edgeIndex);
		if (edge.GetFaceIndex()==face1)
			secondToFirstEdge = edgeIndex;
		if (edge.GetFaceIndex()==face3)
		{
			secondToThirdEdge = edgeIndex;
			thirdToSecondEdge = edge.GetPointIndex();
		}
	}
	// these better be linked!
	assert(secondToThirdEdge!=-1);
	assert(secondToFirstEdge!=-1);

	int rightHandEdge = secondToFirstEdge<2 ? secondToFirstEdge+1 : 0;
	// if face 2 to 3 is right hand
	if (secondToThirdEdge==rightHandEdge)
	{
		// next is a right
		thirdToNextEdge = thirdToSecondEdge<2 ? thirdToSecondEdge+1 : 0;
	}
	else
	{
		// next is a left
		thirdToNextEdge = thirdToSecondEdge>0 ? thirdToSecondEdge-1 : 2;
	}
	// return face on the other side of this edge on face3
	return m_pFaces[face3].GetEdge(thirdToNextEdge).GetFaceIndex();
}

// given 2 linked faces, find one that is next in the pattern
int StripBuilder::GetOneFaceInPattern(int face1,int face2)
{
	TriFace &secondFace = m_pFaces[face2];
	int secondToFirstEdge=-1;
	for (int edgeIndex = 0;edgeIndex<3;edgeIndex++)
	{
		Edge &edge = secondFace.GetEdge(edgeIndex);
		if (edge.GetFaceIndex()==face1)
			secondToFirstEdge = edgeIndex;
	}
	// these better be linked!
	assert(secondToFirstEdge!=-1);
	int secondToThirdEdge = secondToFirstEdge<2 ? secondToFirstEdge+1 : 0;
	return m_pFaces[face2].GetEdge(secondToThirdEdge).GetFaceIndex();
}

// given 2 linked faces, find one that is next in the pattern
int StripBuilder::GetAnotherFaceInPattern(int face1,int face2)
{
	TriFace &secondFace = m_pFaces[face2];
	int secondToFirstEdge=-1;
	for (int edgeIndex = 0;edgeIndex<3;edgeIndex++)
	{
		Edge &edge = secondFace.GetEdge(edgeIndex);
		if (edge.GetFaceIndex()==face1)
			secondToFirstEdge = edgeIndex;
	}
	// these better be linked!
	assert(secondToFirstEdge!=-1);
	int secondToThirdEdge = secondToFirstEdge>0 ? secondToFirstEdge-1 : 2;
	return m_pFaces[face2].GetEdge(secondToThirdEdge).GetFaceIndex();
}


void StripBuilder::CombineStrips(void)
{
	int totalStrips = m_strips.GetSize();
	// optimal pass (each merge will save 2 points)
	for (int stripIndex=0;stripIndex<totalStrips;stripIndex++)
	{
		Strip &strip = m_strips[stripIndex];
		if (!(strip.GetFlags()&Strip::REMOVED))
		{
			while(CombineStrips(stripIndex,2)); // keep trying until it returns false
		}
	}
	// if there can be no swaps then stop here
	if (!m_bAllowSwaps) return;

	// pass for tying to save 1 point
	for (stripIndex=0;stripIndex<totalStrips;stripIndex++)
	{
		Strip &strip = m_strips[stripIndex];
		if (!(strip.GetFlags()&Strip::REMOVED))
		{
			while(CombineStrips(stripIndex,1)); // keep trying until it returns false
		}
	}

	// pass for trying to save 0 points
	for (stripIndex=0;stripIndex<totalStrips;stripIndex++)
	{
		Strip &strip = m_strips[stripIndex];
		if (!(strip.GetFlags()&Strip::REMOVED))
		{
			while(CombineStrips(stripIndex,0)); // keep trying until it returns false
		}
	}
}

// try to combine any strips that are close to this one
bool StripBuilder::CombineStrips(int stripIndex,int threshold)
{
	assert(threshold>=0&&threshold<3);
	Strip &strip = m_strips[stripIndex];
	int firstFace,secondFace,thirdFace;
	int numFaces = strip.GetNumFaces();

	if (numFaces>2) // we have more than 2 faces
	{
		// try the end of my strip to another strip
		thirdFace = strip.GetEndFace();
		secondFace = m_pFaces[thirdFace].GetPathParentFace();
		firstFace = m_pFaces[secondFace].GetPathParentFace();
		if (CombineStrips(firstFace,secondFace,thirdFace,stripIndex,threshold,true))
		{
			m_pStats->m_combineStrips++;
			return true;
		}
		// try the start of my strip to another strip
		thirdFace = strip.GetStartFace();
		secondFace = m_pFaces[thirdFace].GetPathChildFace();
		firstFace = m_pFaces[secondFace].GetPathChildFace();
		if (CombineStrips(firstFace,secondFace,thirdFace,stripIndex,threshold,false))
		{
			m_pStats->m_combineStrips++;
			return true;
		}
	}
	else if (numFaces==2)
	{
		// try the end of my strip to another strip
		secondFace = strip.GetEndFace();
		firstFace = m_pFaces[secondFace].GetPathParentFace();
		if (CombineStrips(firstFace,secondFace,stripIndex,threshold,true))
		{
			m_pStats->m_combineStrips++;
			return true;
		}
		// try the start of my strip to another strip
		secondFace = strip.GetStartFace();
		firstFace = m_pFaces[secondFace].GetPathChildFace();
		if (CombineStrips(firstFace,secondFace,stripIndex,threshold,false))
		{
			m_pStats->m_combineStrips++;
			return true;
		}
	}
	// I have 1 face
	else
	{
		if (CombineStrips(strip.GetStartFace(),stripIndex,threshold))
		{
			m_pStats->m_combineStrips++;
			return true;
		}
	}
	return false;
}

// try to combine a strip with at least 3 faces to its neighboors
// will return true if it merged two strips together
bool StripBuilder::CombineStrips(int firstFace,int secondFace,int thirdFace,int stripIndex,int threshold,bool stripEnd)
{
	int tryFace,tryStripIndex;
	tryFace = GetOptimalFaceInPattern(firstFace,secondFace,thirdFace);
	if (tryFace!=-1)
	{
		tryStripIndex = m_pFaces[tryFace].GetStripIndex();
		if (StripCombineCheck(stripIndex,tryStripIndex))
		{
			if (CombineStrips(firstFace,secondFace,thirdFace,tryFace,stripIndex,threshold,stripEnd))
				return true;
		}
	}
	// try alternate face (will result in a swap)
	if (threshold<2)
	{
		tryFace = GetAlternateFaceInPattern(firstFace,secondFace,thirdFace);
		if (tryFace!=-1)
		{
			tryStripIndex = m_pFaces[tryFace].GetStripIndex();
			if (StripCombineCheck(stripIndex,tryStripIndex))
			{
				if (CombineStrips(firstFace,secondFace,thirdFace,tryFace,stripIndex,threshold,stripEnd))
					return true;
			}
		}
	}
	return false;
}

// try to combine a strip with 2 faces to its neighboors
// will return true if it merged two strips together
bool StripBuilder::CombineStrips(int firstFace,int secondFace,int stripIndex,int threshold,bool stripEnd)
{
	int tryFace,tryStripIndex;
	tryFace = GetOneFaceInPattern(firstFace,secondFace);
	if (tryFace!=-1)
	{
		tryStripIndex = m_pFaces[tryFace].GetStripIndex();
		if (StripCombineCheck(stripIndex,tryStripIndex))
		{
			if (CombineStrips(-1,firstFace,secondFace,tryFace,stripIndex,threshold,stripEnd))
				return true;
		}
	}
	// try other face (either will do)
	tryFace = GetAnotherFaceInPattern(firstFace,secondFace);
	if (tryFace!=-1)
	{
		tryStripIndex = m_pFaces[tryFace].GetStripIndex();
		if (StripCombineCheck(stripIndex,tryStripIndex))
		{
			if (CombineStrips(-1,firstFace,secondFace,tryFace,stripIndex,threshold,stripEnd))
				return true;
		}
	}
	return false;
}

// try to combine a strip with 1 face to its neighboors
bool StripBuilder::CombineStrips(int firstFace,int stripIndex,int threshold)
{
	int tryFace;
	int otherSecondFace;
	int otherThirdFace;
	Strip &strip = m_strips[stripIndex];
	TriFace &face = m_pFaces[strip.GetStartFace()];
	// loop through my neighboors
	for (int edgeIndex=0;edgeIndex<3;edgeIndex++)
	{
		tryFace = face.GetEdge(edgeIndex).GetFaceIndex();
		if (tryFace!=-1) // if this is a valid face
		{
			int tryStripIndex;
			tryStripIndex = m_pFaces[tryFace].GetStripIndex();
			// check the strips and see if they can combine (checks the bones)
			if (StripCombineCheck(stripIndex,tryStripIndex))
			{
				Strip &otherStrip = m_strips[tryStripIndex];
				int numOtherFaces = otherStrip.GetNumFaces();
				if (numOtherFaces>2) // other is more than 2 faces
				{
					if (tryFace == otherStrip.GetStartFace())
					{
						otherSecondFace =  m_pFaces[tryFace].GetPathChildFace();
						otherThirdFace =  m_pFaces[otherSecondFace].GetPathChildFace();
						// my face fits the other strips pattern
						if (GetOptimalFaceInPattern(otherThirdFace,otherSecondFace,tryFace)==firstFace)
						{
							// merging these 2 strips will result in no swaps
							return MergeStrips(strip,otherStrip,START_TO_END,threshold);
						}
						else if (threshold<2) 
						{
							// merging these 2 strips will result in 1 swap
							return MergeStrips(strip,otherStrip,START_TO_END,threshold);
						}
					}
					if (tryFace == otherStrip.GetEndFace())
					{
						otherSecondFace =  m_pFaces[tryFace].GetPathParentFace();
						otherThirdFace =  m_pFaces[otherSecondFace].GetPathParentFace();
						// my face fits the other strips pattern
						if (GetOptimalFaceInPattern(otherThirdFace,otherSecondFace,tryFace)==firstFace)
						{
							// merging these 2 strips will result in no swaps
							return MergeStrips(strip,otherStrip,END_TO_END,threshold);
						}
						else if (threshold<2) 
						{
							// merging these 2 strips will result in 1 swap
							return MergeStrips(strip,otherStrip,END_TO_END,threshold);
						}
					}
				}
				else if (numOtherFaces==2)
				{
					if (tryFace == otherStrip.GetStartFace())
					{
						// merging these 2 strips will result in no swaps
						return MergeStrips(strip,otherStrip,START_TO_END,threshold);
					}
					if (tryFace == otherStrip.GetEndFace())
					{
						// merging these 2 strips will result in no swaps
						return MergeStrips(strip,otherStrip,END_TO_END,threshold);
					}
				}
				else // both are 1 face
				{
					// merging these 2 strips will result in no swaps
					return MergeStrips(strip,otherStrip,START_TO_END,threshold);
				}
			}
		}
	}
	return false;
}

// general function that will do the merging (2 or 3 faces for the first strip)
bool StripBuilder::CombineStrips(int firstFace,int secondFace,int thirdFace,int tryFirstFace,int stripIndex,int threshold,bool stripEnd)
{
	Strip &strip = m_strips[stripIndex];
	int tryStrip;
	int trySecondFace,tryThirdFace;
	int otherSecondFace,otherThirdFace;

	assert(tryFirstFace!=-1);
	tryStrip = m_pFaces[tryFirstFace].GetStripIndex();
	assert(tryStrip!=stripIndex);

	Strip &otherStrip = m_strips[tryStrip];
	int numOtherFaces = otherStrip.GetNumFaces();
	if (numOtherFaces>2) // other is more than 2 faces
	{
		CombineStripsType combineStripsType = stripEnd ? START_TO_END : START_TO_START;
		if (tryFirstFace == otherStrip.GetStartFace())
		{
			// get the next face in the strip and pattern
			otherSecondFace =  m_pFaces[tryFirstFace].GetPathChildFace();
			trySecondFace = GetOptimalFaceInPattern(secondFace,thirdFace,tryFirstFace);
			// if these match then try one more
			if (trySecondFace == otherSecondFace)
			{
				otherThirdFace =  m_pFaces[otherSecondFace].GetPathChildFace();
				tryThirdFace = GetOptimalFaceInPattern(thirdFace,tryFirstFace,trySecondFace);
				if (tryThirdFace==otherThirdFace)
				{
					// merging these 2 strips will result in no swaps
					return MergeStrips(strip,otherStrip,combineStripsType,threshold);
				}
				else if (threshold<2)
				{
					// merging these 2 strips will result in 1 swap
					return MergeStrips(strip,otherStrip,combineStripsType,threshold);
				}
			}
			else if (threshold<2)
			{
				// use other second face and see if that pattern is ok
				otherThirdFace =  m_pFaces[otherSecondFace].GetPathChildFace();
				tryThirdFace = GetOptimalFaceInPattern(thirdFace,tryFirstFace,otherSecondFace);
				if (tryThirdFace==otherThirdFace)
				{
					// merging these 2 strips will result in 1 swap
					return MergeStrips(strip,otherStrip,combineStripsType,threshold);
				}
				else if (threshold<1)
				{
					// merging these 2 strips will result in 2 swaps
					return MergeStrips(strip,otherStrip,combineStripsType,threshold);
				}
			}
		}
		if (tryFirstFace == otherStrip.GetEndFace())
		{
			CombineStripsType combineStripsType = stripEnd ? END_TO_END : END_TO_START;
			
			// get the next face in the strip and pattern
			otherSecondFace =  m_pFaces[tryFirstFace].GetPathParentFace();
			trySecondFace = GetOptimalFaceInPattern(secondFace,thirdFace,tryFirstFace);
			// if these match then try one more
			if (trySecondFace == otherSecondFace)
			{
				otherThirdFace =  m_pFaces[otherSecondFace].GetPathParentFace();
				tryThirdFace = GetOptimalFaceInPattern(thirdFace,tryFirstFace,trySecondFace);
				if (tryThirdFace==otherThirdFace)
				{
					// merging these 2 strips will result in no swaps
					return MergeStrips(strip,otherStrip,combineStripsType,threshold);
				}
				else if (threshold<2)
				{
					// merging these 2 strips will result in 1 swap
					return MergeStrips(strip,otherStrip,combineStripsType,threshold);
				}
			}
			else if (threshold<2)
			{
				// use other second face and see if that pattern is ok
				otherThirdFace =  m_pFaces[otherSecondFace].GetPathParentFace();
				tryThirdFace = GetOptimalFaceInPattern(thirdFace,tryFirstFace,otherSecondFace);
				if (tryThirdFace==otherThirdFace)
				{
					// merging these 2 strips will result in 1 swap
					return MergeStrips(strip,otherStrip,combineStripsType,threshold);
				}
				else if (threshold<1)
				{
					// merging these 2 strips will result in 2 swaps
					return MergeStrips(strip,otherStrip,combineStripsType,threshold);
				}
			}
		}
	}
	else if (numOtherFaces==2) // other is 2 faces
	{
		if (tryFirstFace == otherStrip.GetStartFace())
		{
			CombineStripsType combineStripsType = stripEnd ? START_TO_END : START_TO_START;
			
			// get the next face in the strip and pattern
			otherSecondFace =  otherStrip.GetEndFace();
			trySecondFace = GetOptimalFaceInPattern(secondFace,thirdFace,tryFirstFace);
			// if these match then try one more
			if (trySecondFace == otherSecondFace)
			{
				// merging these 2 strips will result in no swaps
				return MergeStrips(strip,otherStrip,combineStripsType,threshold);
			}
			else if (threshold<2)
			{
				// merging these 2 strips will result in 1 swap
				return MergeStrips(strip,otherStrip,combineStripsType,threshold);
			}
		}
		if (tryFirstFace == otherStrip.GetEndFace())
		{
			CombineStripsType combineStripsType = stripEnd ? END_TO_END : END_TO_START;
			// get the next face in the strip and pattern
			otherSecondFace =  otherStrip.GetStartFace();
			trySecondFace = GetOptimalFaceInPattern(secondFace,thirdFace,tryFirstFace);
			// if these match then try one more
			if (trySecondFace == otherSecondFace)
			{
				// merging these 2 strips will result in no swaps
				return MergeStrips(strip,otherStrip,combineStripsType,threshold);
			}
			else if (threshold<2)
			{
				// merging these 2 strips will result in 1 swap
				return MergeStrips(strip,otherStrip,combineStripsType,threshold);
			}
		}
	}
	else // other is 1 face
	{
		// merging these 2 strips will result in no swaps (at least at this level)
		CombineStripsType combineStripsType = stripEnd ? START_TO_END : START_TO_START;
		return MergeStrips(strip,otherStrip,combineStripsType,threshold);
	}
	return false;
}

void StripBuilder::CheckStrips(void)
{
	m_numNonRequestedSwaps = 0;
	int totalStrips = m_strips.GetSize();
	// optimal pass (each merge will save 2 points)
	for (int stripIndex=0;stripIndex<totalStrips;stripIndex++)
	{
		Strip &strip = m_strips[stripIndex];
		if (!(strip.GetFlags()&Strip::REMOVED))
		{
			CheckStrip(stripIndex);
		}
	}
	if (m_numNonRequestedSwaps)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Warning mesh %s has %d non requested swaps",(const char *)m_name,m_numNonRequestedSwaps);
	}
}

void StripBuilder::CheckStrip(int stripIndex)
{
	Strip &strip = m_strips[stripIndex];

	int startFaceIndex = strip.GetStartFace();
	int endFaceIndex = strip.GetEndFace();
	int numFaces=0;
	int parentFaceIndex = -1;
	int currentFaceIndex = startFaceIndex;

	Keeper<int> bonesPerStrip;

	while(currentFaceIndex!=-1)
	{
		numFaces++;
		TriFace *pFace = &m_pFaces[currentFaceIndex];
		int nextFaceIndex;
		int nextEdgeIndex;
		int nextPathIndex;

		if (m_pBoneVerts)
		{
			for (int pointIndex = 0;pointIndex<3;pointIndex++)
			{
				int boneIndex = m_pBoneVerts[m_pPositionIndices[currentFaceIndex+currentFaceIndex+currentFaceIndex+pointIndex]];
				bonesPerStrip.Add(boneIndex);
			}
			assert(bonesPerStrip.GetSize()<=m_nBonesPerStrip);
		}

		assert(pFace->GetPathParentFace()==parentFaceIndex);
		int faceFlags = pFace->GetFlags();
		assert(pFace->GetStripIndex()==stripIndex);
		switch (faceFlags&TriFace::PATHS)
		{
			// last one in strip
			case 0:
				nextFaceIndex=-1; // we are done
				break;
			case TriFace::PATH_0:
				nextFaceIndex = pFace->GetEdge(0).GetFaceIndex();
				nextEdgeIndex = pFace->GetEdge(0).GetPointIndex();
				nextPathIndex = 0;
				break;
			case TriFace::PATH_1:
				nextFaceIndex = pFace->GetEdge(1).GetFaceIndex();
				nextEdgeIndex = pFace->GetEdge(1).GetPointIndex();
				nextPathIndex = 1;
				break;
			case TriFace::PATH_2:
				nextFaceIndex = pFace->GetEdge(2).GetFaceIndex();
				nextEdgeIndex = pFace->GetEdge(2).GetPointIndex();
				nextPathIndex = 2;
				break;
			default:
				assert(false);
		}
		if (nextFaceIndex!=-1)
		{
			TriFace *pNextFace = &m_pFaces[nextFaceIndex];
			assert(pNextFace->GetEdge(nextEdgeIndex).GetFaceIndex()==currentFaceIndex);
			assert(pNextFace->GetEdge(nextEdgeIndex).GetPointIndex()==nextPathIndex);
			parentFaceIndex=currentFaceIndex;
			currentFaceIndex=nextFaceIndex;
		}
		else
		{
			assert(currentFaceIndex==endFaceIndex);
			currentFaceIndex=-1;
		}
	}
	assert(numFaces==strip.GetNumFaces());

	if (!m_bAllowSwaps)
	{
		PointStrip pointStrip;
		BuildPointStrip(strip,pointStrip);
		int numSwapsA = pointStrip.GetNumPoints()-(pointStrip.GetNumTriangles()+2);
		if (numSwapsA)
		{
			ReverseStrip(strip);
			BuildPointStrip(strip,pointStrip);
			ReverseStrip(strip);
			// if the reversed one is better then use it
			int numSwapsB = pointStrip.GetNumPoints()-(pointStrip.GetNumTriangles()+2);
			if (numSwapsB)
			{
				m_numNonRequestedSwaps += numSwapsA>numSwapsB ? numSwapsA : numSwapsB;
			}	
		}
	}
}

// this function will take the final face paths and build the actual point strips to be used by 
// the TriStripTriMesh class
void StripBuilder::BuildPointStrips(void)
{
	int validStripIndex;
	int currentStripIndex;
	// count how many valid strips there are currently
	for (currentStripIndex=0,validStripIndex=0;currentStripIndex<m_strips.GetSize();currentStripIndex++)
	{
		Strip &strip = m_strips[currentStripIndex];
		if (!(strip.GetFlags()&Strip::REMOVED))
		{
			validStripIndex++;
		}
	}
	m_pointStrips.SetSize(validStripIndex);
	// build the point strips
	m_numNonRequestedSwaps = 0;
	for (currentStripIndex=0,validStripIndex=0;currentStripIndex<m_strips.GetSize();currentStripIndex++)
	{
		Strip &strip = m_strips[currentStripIndex];
		if (!(strip.GetFlags()&Strip::REMOVED))
		{
			PointStrip &pointStrip = m_pointStrips[validStripIndex];
			BuildPointStrip(strip,pointStrip);
			int numSwaps = pointStrip.GetNumPoints()-(pointStrip.GetNumTriangles()+2);
			// if there is a swap
			if (numSwaps)
			{
				// if we allow swaps then reverse the strip
				if (m_bAllowSwaps)
				{
					ReverseStrip(strip);
					PointStrip tempPointStrip;
					BuildPointStrip(strip,tempPointStrip);
					int numReversedSwaps = pointStrip.GetNumPoints()-(pointStrip.GetNumTriangles()+2);
					if (numReversedSwaps<numSwaps)
					{
						pointStrip = tempPointStrip; // use reverse
					}
				}
				// this should never happen
				else
				{
					m_numNonRequestedSwaps += numSwaps;
				}
			}
			validStripIndex++;
		}
	}
	if (m_numNonRequestedSwaps)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Warning mesh %s has %d non requested swaps",(const char *)m_name,m_numNonRequestedSwaps);
	}
}

// builds a single pointstrip from a strip
void StripBuilder::BuildPointStrip(Strip &strip, PointStrip &pointStrip)
{
	bool parity;
	pointStrip.Init();
	pointStrip.SetNumTriangles(strip.GetNumFaces()); // set the triangle count (for informational purposes only)
	// add first triangle
	if (strip.GetNumFaces()==1)
	{
		pointStrip.AddFaceIndex(strip.GetStartFace());
		pointStrip.AddPointIndex(0);

		pointStrip.AddFaceIndex(strip.GetStartFace());
		pointStrip.AddPointIndex(1);

		pointStrip.AddFaceIndex(strip.GetStartFace());
		pointStrip.AddPointIndex(2);
	}
	else
	{
		int startFaceIndex = strip.GetStartFace();
		TriFace &startFace = m_pFaces[startFaceIndex];

		int childEdgeIndex = startFace.GetPathChildEdge();
		assert(childEdgeIndex>=0 && childEdgeIndex<=2);
		pointStrip.AddFaceIndex(startFaceIndex);
		pointStrip.AddPointIndex(childEdgeIndex>0 ? childEdgeIndex-1 : 2);

		pointStrip.AddFaceIndex(startFaceIndex);
		pointStrip.AddPointIndex(childEdgeIndex);

		pointStrip.AddFaceIndex(startFaceIndex);
		pointStrip.AddPointIndex(childEdgeIndex<2 ? childEdgeIndex+1 : 0);

		int childFaceIndex = startFace.GetPathChildFace();
		int childFaceParentEdgeIndex = startFace.GetEdge(childEdgeIndex).GetPointIndex();
		assert(childFaceParentEdgeIndex>=0 && childFaceParentEdgeIndex<=2);
		TriFace &childFace = m_pFaces[childFaceIndex];
		int secondChildEdgeIndex = childFace.GetPathChildEdge();
		// if there is a child edge
		if (secondChildEdgeIndex!=-1)
		{
			// if making a left turn (great)
			if (secondChildEdgeIndex==(childFaceParentEdgeIndex>0 ? childFaceParentEdgeIndex-1 : 2))
			{
				parity = true; // the next face is an even face

			}
			else // making a right turn (bad)
			{
				// add the child edge again (swap)
				pointStrip.AddFaceIndex(startFaceIndex);
				assert(childEdgeIndex>=0 && childEdgeIndex<=2);
				pointStrip.AddPointIndex(childEdgeIndex);
				parity = false; // the next face is an odd face
			}
		}
		// now loop through the remaining faces
		while (1)
		{
			int currentFaceIndex = childFaceIndex;
			int currentFaceParentEdgeIndex = childFaceParentEdgeIndex;
			TriFace &currentFace = m_pFaces[currentFaceIndex];
			int childEdgeIndex = currentFace.GetPathChildEdge();

			// add point on my face
			pointStrip.AddFaceIndex(currentFaceIndex);
			assert(currentFaceParentEdgeIndex>=0 && currentFaceParentEdgeIndex<=2);
			pointStrip.AddPointIndex(currentFaceParentEdgeIndex>0 ? currentFaceParentEdgeIndex-1 : 2);
			childFaceIndex = currentFace.GetPathChildFace();
			// if all done with children then we are done
			if (childFaceIndex==-1)
				break;
			childFaceParentEdgeIndex = currentFace.GetEdge(childEdgeIndex).GetPointIndex();
			assert(childFaceParentEdgeIndex>=0 && childFaceParentEdgeIndex<=2);
			TriFace &childFace = m_pFaces[childFaceIndex];
			int secondChildEdgeIndex = childFace.GetPathChildEdge();
			// if there is a second child to worry about
			if (secondChildEdgeIndex!=-1)
			{
				if (parity)
				{
					// if making a right turn (great)
					if (secondChildEdgeIndex==(childFaceParentEdgeIndex<2 ? childFaceParentEdgeIndex+1 : 0))
					{
						parity = false; // the next face is an odd face
					}
					else // making a left turn (bad)
					{
						// add the swap
						pointStrip.AddFaceIndex(currentFaceIndex);
						assert(currentFaceParentEdgeIndex>=0 && currentFaceParentEdgeIndex<=2);
						pointStrip.AddPointIndex(currentFaceParentEdgeIndex);
						parity = true; // the next face is an even face
					}
				}
				else
				{
					// if making a left turn (great)
					if (secondChildEdgeIndex==(childFaceParentEdgeIndex>0 ? childFaceParentEdgeIndex-1 : 2))
					{
						parity = true; // the next face is an even face
					}
					else // making a right turn (bad)
					{
						// add the swap
						pointStrip.AddFaceIndex(currentFaceIndex);
						assert(currentFaceParentEdgeIndex>=0 && currentFaceParentEdgeIndex<=2);
						pointStrip.AddPointIndex(currentFaceParentEdgeIndex<2 ? currentFaceParentEdgeIndex+1 : 0);
						parity = false; // the next face is an odd face
					}
				}
			}
		}
	}
}

void StripBuilder::BuildStripBones(void)
{
	if (m_pBoneVerts)
	{
		int numStrips = m_strips.GetSize();
		m_stripBones.SetSize(numStrips);
		for (int stripIndex = 0;stripIndex<numStrips;stripIndex++)
		{
			BuildStripBones(stripIndex);
		}
	}
}

void StripBuilder::BuildStripBones(int stripIndex)
{
	assert(m_pBoneVerts);
	Keeper<int> &bones = m_stripBones[stripIndex];
	Strip &strip = m_strips[stripIndex];

	bones.RemoveAll(); // start with none

	// go through each face in the strip and add the bones used for each face to the keeper

	int numFaces = strip.GetNumFaces();
	int faceIndex = strip.GetStartFace();
	while(faceIndex!=-1)
	{
		int baseIndex = faceIndex+(faceIndex<<1);
		for (int pointIndex=0;pointIndex<3;pointIndex++)
		{
			bones.Add(m_pBoneVerts[m_pPositionIndices[baseIndex+pointIndex]]);
			assert(bones.GetSize()<=m_nBonesPerStrip);
		}
		faceIndex = m_pFaces[faceIndex].GetPathChildFace();
	}
}

void StripBuilder::BuildStripUVMinMax(void)
{
	if (m_pFaceUVArray)
	{
		int numStrips = m_strips.GetSize();
		m_stripUVMinMaxArray.SetSize(numStrips);
		for (int stripIndex = 0;stripIndex<numStrips;stripIndex++)
		{
			BuildStripUVMinMax(stripIndex);
		}
	}
}

void StripBuilder::BuildStripUVMinMax(int stripIndex)
{
	assert(m_pFaceUVArray);
	TupArray<TupArray<MinMaxUV> > &minMaxUVArray = m_stripUVMinMaxArray[stripIndex];
	Strip &strip = m_strips[stripIndex];

	// go through each face in the strip and add the bones used for each face to the keeper
	int numFaces = strip.GetNumFaces();
	int faceIndex = strip.GetStartFace();

	while(faceIndex!=-1)
	{
		TupArray<int> stagedFaces;
		GetStagedFaces(faceIndex,stagedFaces);
		int numStages = stagedFaces.GetSize();
		minMaxUVArray.SetSize(numStages);
		for (int stageIndex=0;stageIndex<numStages;stageIndex++)
		{
			int stageFace = stagedFaces[stageIndex];
			int stageFace3 = stageFace+(stageFace<<1);
			int materialIndex = m_pFaceMaterialIndices[stageFace];

			TupArray<Vector3> &limitsArray = (*m_pUVLimitsArray)[materialIndex];
			int numLayers = (*m_pFaceUVArray)[stageFace3].GetSize();
			minMaxUVArray[stageIndex].SetSize(numLayers);
			for (int layerIndex=0;layerIndex<numLayers;layerIndex++)
			{
				MinMaxUV &minMaxUV = minMaxUVArray[stageIndex][layerIndex];
				for (int pointIndex=0;pointIndex<3;pointIndex++)
				{
					minMaxUV.AddCoor((*m_pFaceUVArray)[stageFace3+pointIndex][layerIndex]);
				}
			}
		}
		faceIndex = m_pFaces[faceIndex].GetPathChildFace();
	}
}

bool StripBuilder::StripCombineCheck(int strip1Index,int strip2Index)
{
	// these better be valid!
	assert(strip1Index!=-1 && strip2Index!=-1);

	// can't combine to itself
	if (strip1Index == strip2Index)
		return false;

	// check bones to make sure the strips can be combined
	if (m_pBoneVerts)
	{
		Keeper<int> tempBones(m_stripBones[strip1Index]); // build a copy of the first strip's bones
		Keeper<int> &bones = m_stripBones[strip2Index];
		int numBones = bones.GetSize();
		assert(tempBones.GetSize()<=m_nBonesPerStrip);
		assert(numBones<=m_nBonesPerStrip);
		for (int boneIndex=0;boneIndex<numBones;boneIndex++)
		{
			tempBones.Add(bones[boneIndex]);
			// if there are too many after adding this one then stop and return false (don't combine)
			if (tempBones.GetSize()>m_nBonesPerStrip)
				return false;
		}
	}

	if (m_pFaceUVArray)
	{
		Strip &strip1 = m_strips[strip1Index];
		int face1Index = strip1.GetStartFace();
		TupArray<int> stagedFaces1;
		GetStagedFaces(face1Index,stagedFaces1);

		Strip &strip2 = m_strips[strip2Index];
		int face2Index = strip2.GetStartFace();
		TupArray<int> stagedFaces2;
		GetStagedFaces(face2Index,stagedFaces2);

		TupArray<TupArray<MinMaxUV> > &minMaxUVArray1 = m_stripUVMinMaxArray[strip1Index];
		TupArray<TupArray<MinMaxUV> > &minMaxUVArray2 = m_stripUVMinMaxArray[strip2Index];

		int numStages = minMaxUVArray1.GetSize();
		assert(numStages==minMaxUVArray2.GetSize());
		for (int stageIndex=0;stageIndex<numStages;stageIndex++)
		{
			int numLayers = minMaxUVArray1[stageIndex].GetSize();
			assert(numLayers==minMaxUVArray2[stageIndex].GetSize());

			int materialIndex = m_pFaceMaterialIndices[stagedFaces1[stageIndex]];
			assert(materialIndex == m_pFaceMaterialIndices[stagedFaces2[stageIndex]]);

			TupArray<Vector3> &limitsArray = (*m_pUVLimitsArray)[materialIndex];

			for (int layerIndex=0;layerIndex<numLayers;layerIndex++)
			{
				MinMaxUV &minMaxUV1 = minMaxUVArray1[stageIndex][layerIndex];
				MinMaxUV &minMaxUV2 = minMaxUVArray2[stageIndex][layerIndex];
				Vector3 &limits = limitsArray[layerIndex];
				if (!minMaxUV1.TestCombine(minMaxUV2,limits))
					return false;
			}
		}
	}

	// TODO - add other checks for the combine when needed

	return true; // ok to combine
}

void StripBuilder::SortStrips(void)
{
	int numStrips = m_pointStrips.GetSize();
	m_stripSortArray.SetSize(numStrips);
	int stripIndex;
	for (stripIndex = 0;stripIndex<numStrips;stripIndex++)
	{
		StripSortData &sortData = m_stripSortArray[stripIndex];
		const int *pFaceIndices = m_pointStrips[stripIndex].GetFaceIndices();
		int firstFace = pFaceIndices[0];
		int faceFlags = m_pFaceFlags[firstFace];
		sortData.m_stripIndex = stripIndex;
		int addValue;
		// zbias
		addValue = (faceFlags & TupTriMesh::FLAGS_FACE_Z_BIAS_MASK)>>TupTriMesh::FLAGS_FACE_Z_BIAS_SHIFT;
		sortData.m_compareData.m_array.Add(addValue);
		// no z write
		addValue = (faceFlags & TupTriMesh::FLAGS_FACE_NO_Z_WRITE) ? 1 : 0;
		sortData.m_compareData.m_array.Add(addValue);
	}

	// if we have staged faces that are linked but not additional strips
	if (!m_bStripStageFaces && m_pNextStagedFaceIndices)
	{
		// calculate the max number of stages for the mesh
		int maxStages = 0;
		for (stripIndex = 0;stripIndex<numStrips;stripIndex++)
		{
			const int *pFaceIndices = m_pointStrips[stripIndex].GetFaceIndices();
			int firstFace = pFaceIndices[0];
			int numStages = GetNumStages(firstFace);
			if (numStages>maxStages)
				maxStages = numStages;
		}
		for (stripIndex = 0;stripIndex<numStrips;stripIndex++)
		{
			StripSortData &sortData = m_stripSortArray[stripIndex];
			const int *pFaceIndices = m_pointStrips[stripIndex].GetFaceIndices();
			int firstFace = pFaceIndices[0];
			int faceFlags = m_pFaceFlags[firstFace];
			sortData.m_stripIndex = stripIndex;

			int addValue;
			// texture alpha before material indices in this case
			addValue = (faceFlags & TupTriMesh::FLAGS_FACE_HAS_TEXTURE_ALPHA) ? 1 : 0;
			sortData.m_compareData.m_array.Add(addValue);

			// add material indicies for the stages
			TupArray<int> stagedFaces;
			GetStagedFaces(firstFace,stagedFaces); // this array contains the first face
			// add the stages
			for (int stageIndex=0;stageIndex<maxStages;stageIndex++)
			{
				// if this face has a stage at this index
				if (stageIndex<stagedFaces.GetSize())
				{
					addValue = m_pFaceMaterialIndices[stagedFaces[stageIndex]];
				}
				else
				{
					addValue = -1;
				}
				sortData.m_compareData.m_array.Add(addValue);
			}
			// any other face flags
			addValue = (faceFlags & m_faceFlagMask);
			sortData.m_compareData.m_array.Add(addValue);
		}
	}
	else // handle case where strips may have stage flags 
	{
		for (int stripIndex = 0;stripIndex<numStrips;stripIndex++)
		{
			StripSortData &sortData = m_stripSortArray[stripIndex];
			const int *pFaceIndices = m_pointStrips[stripIndex].GetFaceIndices();
			int firstFace = pFaceIndices[0];
			int faceFlags = m_pFaceFlags[firstFace];
			int addValue;
			// stage
			addValue = (faceFlags & TupTriMesh::FLAGS_FACE_STAGE_MASK)>>TupTriMesh::FLAGS_FACE_STAGE_SHIFT;
			sortData.m_compareData.m_array.Add(addValue);

			// texture alpha before material indices and after the stage value
			addValue = (faceFlags & TupTriMesh::FLAGS_FACE_HAS_TEXTURE_ALPHA) ? 1 : 0;
			sortData.m_compareData.m_array.Add(addValue);

			// material index
			addValue = m_pFaceMaterialIndices[firstFace];
			sortData.m_compareData.m_array.Add(addValue);

			// any other face flags
			addValue = (faceFlags & m_faceFlagMask);
			sortData.m_compareData.m_array.Add(addValue);
		}
	}
   if (m_bAllowSwaps && m_bOptimizeVertexCache)
   {
      VertexCache usedVerts(m_nVertexCacheSize);
      TupArray<int> vertexCacheStripPositions;
      vertexCacheStripPositions.SetSize(numStrips);
      vertexCacheStripPositions[0] = 0;
      for (int stripIndex = 1; stripIndex < numStrips; stripIndex++)
      {
         vertexCacheStripPositions[stripIndex] = -1;
      }
      int nextStrip = 0;
      int sortValue = 1;
      do 
      {
         // add to the cache
         const PointStrip &pointStrip = m_pointStrips[nextStrip];
         const int *pFaceIndices = pointStrip.GetFaceIndices();
         const int *pFacePointIndices = pointStrip.GetPointIndices();
         int numStripPoints = pointStrip.GetNumPoints();
         for (int pointIndex = 0; pointIndex < numStripPoints; pointIndex++)
         {
            int faceIndex = pFaceIndices[pointIndex];
            int faceIndex3 = faceIndex+(faceIndex<<1);
            int facePointIndex = pFacePointIndices[pointIndex];
            usedVerts.AddValue(m_pPositionIndices[faceIndex3+facePointIndex]);
         }
         int bestStripIndex = -1;
         int bestStripHits;
         for (int testStripIndex = 1; testStripIndex < numStrips; testStripIndex++)
         {
            // do we try this one?
            if (vertexCacheStripPositions[testStripIndex] == -1)
            {
               VertexCache testUsedVerts(usedVerts);

               const PointStrip &pointStrip = m_pointStrips[testStripIndex];
               const int *pFaceIndices = pointStrip.GetFaceIndices();
               const int *pFacePointIndices = pointStrip.GetPointIndices();
               int numStripPoints = pointStrip.GetNumPoints();
               int numMisses = 0;
               int numHits = 0;
               for (int pointIndex = 0; pointIndex < numStripPoints; pointIndex++)
               {
                  int faceIndex = pFaceIndices[pointIndex];
                  int faceIndex3 = faceIndex+(faceIndex<<1);
                  int facePointIndex = pFacePointIndices[pointIndex];
                  if (testUsedVerts.AddValue(m_pPositionIndices[faceIndex3+facePointIndex]))
                  {
                     numMisses++;
                     if (numMisses == m_nVertexCacheSize)
                     {
                        break; // filled the cache so stop
                     }
                  }
                  else
                  {
                     numHits++;
                  }
               }
               if (bestStripIndex == -1 || bestStripHits < numHits)
               {
                  bestStripIndex = testStripIndex;
                  bestStripHits = numHits;
               }
            }
         }
         if (bestStripIndex != -1)
         {
            vertexCacheStripPositions[bestStripIndex] = sortValue++;
            nextStrip = bestStripIndex;
         }
         else
         {
            nextStrip = -1;
         }
      } while(nextStrip != -1);

      for (int stripIndex = 0;stripIndex<numStrips;stripIndex++)
      {
         StripSortData &sortData = m_stripSortArray[stripIndex];
         int addValue = vertexCacheStripPositions[stripIndex];
         sortData.m_compareData.m_array.Add(addValue);
      }
   }
	// bones (if any)
	if (m_pBoneVerts)
	{
		for (int stripIndex = 0;stripIndex<numStrips;stripIndex++)
		{
			StripSortData &sortData = m_stripSortArray[stripIndex];
			TupArray<int> sortedBoneValues = m_stripBones[stripIndex].GetArray();
			TupSort<int>::SortAscending(sortedBoneValues.GetData(),sortedBoneValues.GetSize()); // sort the bones
			for (int boneIndex=0;boneIndex<m_nBonesPerStrip;boneIndex++)
			{
				if (boneIndex<sortedBoneValues.GetSize())
					sortData.m_compareData.m_array.Add(sortedBoneValues[boneIndex]);
				else
					sortData.m_compareData.m_array.Add(0); // no bone for this slot
			}
		}
	}
	// sort the strips
	TupSort<StripSortData>::SortAscending(m_stripSortArray.GetData(),numStrips);
}

void StripBuilder::BuildSortedStripStats(void)
{
	int numFoundSwaps = 0;
	int numWindingProblems = 0;

	int numStrips = m_pointStrips.GetSize();
   int stripIndex;
	for (stripIndex=1;stripIndex<numStrips;stripIndex++)
	{
		// get correct strip (use sort data)
		PointStrip &previousStrip = m_pointStrips[m_stripSortArray[stripIndex-1].m_stripIndex];
		PointStrip &currentStrip = m_pointStrips[m_stripSortArray[stripIndex].m_stripIndex];

		int faceIndexPrevious = previousStrip.GetFaceIndex(0);
		int faceIndexCurrent = currentStrip.GetFaceIndex(0);

		if (m_pFaceMaterialIndices[faceIndexCurrent]!=m_pFaceMaterialIndices[faceIndexPrevious])
		{
			m_pStats->m_sortedStripsBreakMaterialIndex++;
		}

		if ((m_pFaceFlags[faceIndexCurrent]&m_faceFlagMask)!=(m_pFaceFlags[faceIndexPrevious]&m_faceFlagMask))
		{
			m_pStats->m_sortedStripsBreakFaceFlags++;
		}
	}
   if (m_bAllowSwaps && m_bOptimizeVertexCache)
   {
      VertexCache usedVerts(m_nVertexCacheSize);
      for (stripIndex=0;stripIndex<numStrips;stripIndex++)
      {
         const PointStrip &pointStrip = m_pointStrips[m_stripSortArray[stripIndex].m_stripIndex];

         const int *pFaceIndices = pointStrip.GetFaceIndices();
         const int *pFacePointIndices = pointStrip.GetPointIndices();
         int numStripPoints = pointStrip.GetNumPoints();
         for (int pointIndex = 0; pointIndex < numStripPoints; pointIndex++)
         {
            int faceIndex = pFaceIndices[pointIndex];
            int faceIndex3 = faceIndex+(faceIndex<<1);
            int facePointIndex = pFacePointIndices[pointIndex];
            if (usedVerts.AddValue(m_pPositionIndices[faceIndex3+facePointIndex]))
            {
               m_pStats->m_vertexCacheMisses++;
            }
            else
            {
               m_pStats->m_vertexCacheHits++;
            }
         }
      }
   }
}

void StripBuilder::CheckPointStrips(void)
{
	int numFoundSwaps = 0;
	int numWindingProblems = 0;

	int numStrips = m_pointStrips.GetSize();
	for (int stripIndex=0;stripIndex<numStrips;stripIndex++)
	{
		// get correct strip (use sort data)
		PointStrip &strip = m_pointStrips[m_stripSortArray[stripIndex].m_stripIndex];

		int numPoints = strip.GetNumPoints();
		bool bCCWWinding = true;
		for (int pointIndex=2;pointIndex<numPoints;pointIndex++,bCCWWinding=!bCCWWinding)
		{
			int faceIndex = strip.GetFaceIndex(pointIndex);

			int posIndex0 = m_pPositionIndices[(strip.GetFaceIndex(pointIndex-2)<<1)+strip.GetFaceIndex(pointIndex-2)+strip.GetPointIndex(pointIndex-2)];
			int posIndex1 = m_pPositionIndices[(strip.GetFaceIndex(pointIndex-1)<<1)+strip.GetFaceIndex(pointIndex-1)+strip.GetPointIndex(pointIndex-1)];
			int posIndex2 = m_pPositionIndices[(strip.GetFaceIndex(pointIndex-0)<<1)+strip.GetFaceIndex(pointIndex-0)+strip.GetPointIndex(pointIndex-0)];

			if (posIndex0 != posIndex1 &&
				 posIndex0 != posIndex2 &&
				 posIndex1 != posIndex2)
			{
				Vertex &srcPt0 = m_pPositionVerts[posIndex0];
				Vertex &srcPt1 = m_pPositionVerts[posIndex1];
				Vertex &srcPt2 = m_pPositionVerts[posIndex2];
				
				TupVertex3f point0(srcPt0.x,srcPt0.y,srcPt0.z);
				TupVertex3f point1(srcPt1.x,srcPt1.y,srcPt1.z);
				TupVertex3f point2(srcPt2.x,srcPt2.y,srcPt2.z);
				
				TupVertex3f faceNormal;
				if (bCCWWinding)
				{
					faceNormal = TupVertex3f::ComputeFaceNormal(point0,point1,point2);
				}
				else
				{
					faceNormal = TupVertex3f::ComputeFaceNormal(point2,point1,point0);
				}
				
				Vertex &srcFaceNormal = m_pFaceNormalVerts[m_pFaceNormalIndices[faceIndex]];
				TupVertex3f srcFaceNormal2(srcFaceNormal.x,srcFaceNormal.y,srcFaceNormal.z);
				float dotProduct = TupVertex3f::DotProduct(srcFaceNormal2,faceNormal);
				if (dotProduct<0.8f)
				{
					numWindingProblems++;
				}
			}
			// we have a swap
			else
			{
				numFoundSwaps++;
			}
		}
	}
}

void StripBuilder::AddStagedFaceStrips(void)
{
	if (!m_bStripStageFaces)
		return;
	if (!m_pNextStagedFaceIndices)
		return;

	// only go through the number of strips before we add any
	int numNormalStrips = m_pointStrips.GetSize();
	for (int stripIndex=0;stripIndex<numNormalStrips;stripIndex++)
	{
		int numStages = GetNumStages(m_pointStrips[stripIndex].GetFaceIndex(0));
		if (numStages==1)
			continue;

		int numPoints = m_pointStrips[stripIndex].GetNumPoints();
		int newStripStart = m_pointStrips.GetSize();
		m_pointStrips.SetSize(newStripStart+(numStages-1));
		int stageIndex;
		for (stageIndex=1;stageIndex<numStages;stageIndex++)
		{
			m_pointStrips[newStripStart+stageIndex-1].SetNumPoints(numPoints);
		}
		for (int facePointIndex=0;facePointIndex<numPoints;facePointIndex++)
		{
			TupArray<int> stagedFaces;
			// get all the faces that are stages of this face (including this one)
			GetStagedFaces(m_pointStrips[stripIndex].GetFaceIndex(facePointIndex),stagedFaces);
			assert(stagedFaces.GetSize()==numStages);
			int pointIndex = m_pointStrips[stripIndex].GetPointIndex(facePointIndex);
			// set up new point strips
			for (stageIndex=1;stageIndex<numStages;stageIndex++)
			{
				m_pointStrips[newStripStart+stageIndex-1].SetFaceIndex(facePointIndex,stagedFaces[stageIndex]);
				m_pointStrips[newStripStart+stageIndex-1].SetPointIndex(facePointIndex,pointIndex);
			}
		}
	}
}

int StripBuilder::GetNumStages(int faceIndex)
{
	int numStages = 1;
	if (m_pNextStagedFaceIndices)
	{
		while (1)
		{
			int nextStageFace = m_pNextStagedFaceIndices[faceIndex];
			if (nextStageFace!=-1)
			{
				numStages++;
				faceIndex=nextStageFace;
			}
			else
			{
				break;
			}
		}
	}
	return numStages;
}

void StripBuilder::GetStagedFaces(int faceIndex,TupArray<int> &stagedFaces)
{
	stagedFaces.RemoveAll();
	stagedFaces.Add(faceIndex);
	if (m_pNextStagedFaceIndices)
	{
		while (1)
		{
			int nextStageFace = m_pNextStagedFaceIndices[faceIndex];
			if (nextStageFace!=-1)
			{
				stagedFaces.Add(nextStageFace);
				faceIndex=nextStageFace;
			}
			else
			{
				break;
			}
		}
	}
}

