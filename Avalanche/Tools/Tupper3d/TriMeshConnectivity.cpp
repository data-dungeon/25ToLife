////////////////////////////////////////////////////////////////////////////
//
// TriMeshConnectivity
//
// Class to handle build connectivity information for the TupTriMesh
//
////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include <stack>
#include "trimesh.h"
#include "trimeshmap.h"
#include "TriMeshConnectivity.h"
#include "TriMeshConnectivityParts.h"

// for debugging
//#define __CHECKFACES

TupTriMeshConnectivity::TupTriMeshConnectivity(void) :
	m_pPositionVerts(NULL),
	m_pNormalVerts(NULL),
	m_pPositionIndices(NULL),
	m_pNormalIndices(NULL),
	m_pFaceMaterialIndices(NULL),
	m_pFaceFlags(NULL),
	m_pFaceOpacityValues(NULL),
	m_numPositionVerts(0),
	m_numNormalVerts(0),
	m_faceFlagMask(-1)
{
}

TupTriMeshConnectivity::~TupTriMeshConnectivity()
{
}

void TupTriMeshConnectivity::SetMesh(TupTriMesh &tupTriMesh,int faceCheckFlags)
{
	// this holds flags for what to check
	m_faceCheckFlags = faceCheckFlags;

	tupTriMesh.GetPositionVerts((float **)&m_pPositionVerts,m_numPositionVerts);
	tupTriMesh.GetNormalVerts((float **)&m_pNormalVerts,m_numNormalVerts);
	tupTriMesh.GetPositionIndices(&m_pPositionIndices,m_numFaces);
	tupTriMesh.GetNormalIndices(&m_pNormalIndices,m_numFaces);
	tupTriMesh.GetFaceMaterialIndices(&m_pFaceMaterialIndices,m_numFaces);
	tupTriMesh.GetFaceFlags(&m_pFaceFlags,m_numFaces);

	TupperwareAggregate *pMeshAgg = tupTriMesh.GetBaseAggregate();
	TupperwareList *pFaceOpacityList = pMeshAgg->FindListByKey("Annotate_FaceOpacityList");
	if (pFaceOpacityList)
	{
		m_pFaceOpacityValues = pFaceOpacityList->GetAsInt();
	}

	assert(m_numFaces!=0);

	m_faces.SetSize(m_numFaces);

	// check zbias value,two sided flag,as well as cloth
	m_faceFlagMask = TupTriMesh::FLAGS_FACE_Z_BIAS_MASK|TupTriMesh::FLAGS_FACE_TWO_SIDED|TupTriMesh::FLAGS_FACE_NO_Z_WRITE|TupTriMesh::FLAGS_FACE_USES_CLOTH;

	int numMeshMaps = tupTriMesh.GetNumMeshMaps();
	for (int meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
	{
		TupperwareAggregate *pMeshMapAgg = tupTriMesh.GetMeshMap(meshMapIndex);
		TupTriMeshMap tupTriMeshMap(pMeshMapAgg);
		int mapChannel = tupTriMeshMap.GetMapChannel();
		if (mapChannel >= TupTriMeshMap::MAP_CHANNEL_TEXTURE_UV_START) // do uv channels for now
		{
			float *pMapVerts;
			int numMapVerts;
			int *pMapIndices;
			int numMapFaces;
			tupTriMeshMap.GetMapVerts(&pMapVerts,numMapVerts);
			tupTriMeshMap.GetMapIndices(&pMapIndices,numMapFaces);
			assert(numMapFaces==m_numFaces);
			AddMeshMap(pMapVerts,numMapVerts,pMapIndices,mapChannel,meshMapIndex);
		}
	}
	ConnectFaces();
#ifdef __CHECKFACES
	CheckFaces();
#endif
}

void TupTriMeshConnectivity::SetMeshFaceFlagMask(int faceFlagMask)
{
	m_faceFlagMask = faceFlagMask;
}

int TupTriMeshConnectivity::GetNumFaces(void) const
{
	return m_faces.GetSize();
}

const TupTriMeshConnectivity::TriFace &TupTriMeshConnectivity::GetFace(int faceIndex) const
{
	assert(faceIndex>=0 && faceIndex<m_faces.GetSize());
	return m_faces[faceIndex];
}

TupTriMeshConnectivity::TriFace &TupTriMeshConnectivity::GetFace(int faceIndex)
{
	assert(faceIndex>=0 && faceIndex<m_faces.GetSize());
	return m_faces[faceIndex];
}

int TupTriMeshConnectivity::GetNumMeshMaps(void) const
{
	return m_meshMaps.GetSize();
}

TupTriMeshConnectivity::MeshMap &TupTriMeshConnectivity::GetMeshMap(int meshMapIndex)
{
	assert(meshMapIndex>=0 && meshMapIndex<m_meshMaps.GetSize());
	return m_meshMaps[meshMapIndex];
}

const TupTriMeshConnectivity::MeshMap &TupTriMeshConnectivity::GetMeshMap(int meshMapIndex) const
{
	assert(meshMapIndex>=0 && meshMapIndex<m_meshMaps.GetSize());
	return m_meshMaps[meshMapIndex];
}

void TupTriMeshConnectivity::AddMeshMap(float *pMapVerts,int numMapVerts,int *pMapIndices,int mapChannel,int mapIndex)
{
	// should not be null
	assert(pMapVerts);
	// should not be null
	assert(pMapIndices);
	// make sure there are some verts
	assert(numMapVerts>0);
	// make sure I have set the number of faces
	assert(m_numFaces>0);

	TupTriMeshConnectivity::MeshMap newMeshMap;
	newMeshMap.AddMapVerts(pMapVerts,numMapVerts);
	newMeshMap.AddMapIndices(pMapIndices);
	newMeshMap.SetMapChannel(mapChannel);
	newMeshMap.SetMapIndex(mapIndex);
	m_meshMaps.Add(newMeshMap);
}


void TupTriMeshConnectivity::ConnectFaces(void)
{
	// set up an array of face edge links (connect the faces-edges that share the same position index)
	m_faceEdgeLinks.SetSize(m_numPositionVerts);

	// clear pointers 
	for (int vertIndex=0;vertIndex<m_numPositionVerts;vertIndex++)
	{
		m_faceEdgeLinks[vertIndex].SetInvalid(); // no faces using this point yet
	}

	int *pCurrentFacePositionIndices = m_pPositionIndices; // pointer used to access the position indices for the current face
	// loop through the faces and add edges for each into the array
	for (int faceIndex=0;faceIndex<m_numFaces;faceIndex++,pCurrentFacePositionIndices+=3)
	{
		for (int triPoint=0;triPoint<3;triPoint++)
		{
			// get current index
			int index = pCurrentFacePositionIndices[triPoint];

			// set mine to the one based on the position index
			m_faces[faceIndex].SetFaceEdgeLink(m_faceEdgeLinks[index],triPoint);
			
			m_faceEdgeLinks[index].SetNextLink(TupTriMeshConnectivity::Edge(faceIndex,triPoint));
		}
	}
	// if we are not checking material then we do two passes (first does the material check, the other does not care)
	// this will attempt to link faces that share the same sub material first
	int numPasses = (m_faceCheckFlags & FLAGS_FACE_CHECK_MATERIAL_INDEX) ? 1 : 2;

	for (int pass = 0;pass<numPasses;pass++)
	{
		// go through the list and hook the faces together
		for (vertIndex=0;vertIndex<m_numPositionVerts;vertIndex++)
		{
			const TupTriMeshConnectivity::FaceEdgeLink *pCurrentLink = &m_faceEdgeLinks[vertIndex];
			while (pCurrentLink->Valid())
			{
				int faceIndex1 = pCurrentLink->GetNextLink().GetFaceIndex();
				int pointIndex1 = pCurrentLink->GetNextLink().GetPointIndex();

				TupTriMeshConnectivity::Edge &edge1 = m_faces[faceIndex1].GetEdge(pointIndex1);
				// if this has not been set yet
				if (edge1.GetFaceIndex()==-1)
				{
					int face1IndicesIndex1 = (faceIndex1<<1)+faceIndex1+pointIndex1;
					int face1IndicesIndex2 = (faceIndex1<<1)+faceIndex1+((pointIndex1<2) ? pointIndex1+1 : 0);

					int endPointIndex1 = m_pPositionIndices[face1IndicesIndex2];
					// scan through the list on the end point to find a match
					TupTriMeshConnectivity::FaceEdgeLink *pOtherPointLink = &m_faceEdgeLinks[endPointIndex1];
					while (pOtherPointLink->Valid())
					{
						int faceIndex2 = pOtherPointLink->GetNextLink().GetFaceIndex();
						int pointIndex2 = pOtherPointLink->GetNextLink().GetPointIndex();
						int nextPointIndex2 = pointIndex2<2 ? pointIndex2+1 : 0;
						int endPointIndex2 = m_pPositionIndices[(faceIndex2<<1)+faceIndex2+nextPointIndex2];

						// if this links to our edge
						if (endPointIndex2==vertIndex)
						{
							TupTriMeshConnectivity::Edge &edge2 = m_faces[faceIndex2].GetEdge(pointIndex2);

							// if this has not been set yet
							if (edge2.GetFaceIndex()==-1)
							{
								// check the normals, materials, flags, uv coordiates, etc.
								if (pass==0 && m_pFaceMaterialIndices[faceIndex1]!=m_pFaceMaterialIndices[faceIndex2])
								{
									break; // material indices don't match
								}
								if ((m_faceCheckFlags & FLAGS_FACE_CHECK_FACE_FLAGS ) && 
									(m_pFaceFlags[faceIndex1]&m_faceFlagMask)!=(m_pFaceFlags[faceIndex2]&m_faceFlagMask))
								{
									break; // face flags don't match
								}
								if ((m_faceCheckFlags & FLAGS_FACE_CHECK_FACE_OPACITY ) && 
									 m_pFaceOpacityValues && m_pFaceOpacityValues[faceIndex1]!=m_pFaceOpacityValues[faceIndex2])
								{
									break; // if opacity values don't match
								}

								int face2IndicesIndex1 = (faceIndex2<<1)+faceIndex2+pointIndex2;
								int face2IndicesIndex2 = (faceIndex2<<1)+faceIndex2+((pointIndex2<2) ? pointIndex2+1 : 0);

								if (m_faceCheckFlags & FLAGS_FACE_CHECK_NORMALS)
								{
									// check and make sure the normals are the same
									TupTriMeshConnectivity::Vertex &face1Normal1 = m_pNormalVerts[m_pNormalIndices[face1IndicesIndex1]];
									TupTriMeshConnectivity::Vertex &face1Normal2 = m_pNormalVerts[m_pNormalIndices[face1IndicesIndex2]];
									TupTriMeshConnectivity::Vertex &face2Normal1 = m_pNormalVerts[m_pNormalIndices[face2IndicesIndex1]];
									TupTriMeshConnectivity::Vertex &face2Normal2 = m_pNormalVerts[m_pNormalIndices[face2IndicesIndex2]];
									if (!face1Normal1.Compare(face2Normal2,0.001f) ||
										!face1Normal2.Compare(face2Normal1,0.001f))
									{
										break;
									}
								}

								if (m_faceCheckFlags & FLAGS_FACE_CHECK_MESHMAPS)
								{
									// check and make sure the maps are the same
									bool mapOk = true;
									int numMeshMaps = m_meshMaps.GetSize();
									for (int meshMapIndex = 0;meshMapIndex<numMeshMaps;meshMapIndex++)
									{
										const TupTriMeshConnectivity::MeshMap &meshMap = m_meshMaps[meshMapIndex];
										int *pMapIndices = meshMap.GetMapIndices();
										TupTriMeshConnectivity::Vertex *pMapVerts = meshMap.GetVerts();
										TupTriMeshConnectivity::Vertex &face1Vert1 = pMapVerts[pMapIndices[face1IndicesIndex1]];
										TupTriMeshConnectivity::Vertex &face1Vert2 = pMapVerts[pMapIndices[face1IndicesIndex2]];
										TupTriMeshConnectivity::Vertex &face2Vert1 = pMapVerts[pMapIndices[face2IndicesIndex1]];
										TupTriMeshConnectivity::Vertex &face2Vert2 = pMapVerts[pMapIndices[face2IndicesIndex2]];
										if (!face1Vert1.Compare(face2Vert2,0.001f) ||
											!face1Vert2.Compare(face2Vert1,0.001f))
										{
											mapOk = false;
											break;
										}
									}
									// kick out if not ok
									if (!mapOk)
										break;
								}

								// link together
								edge1.SetFaceIndex(faceIndex2);
								edge1.SetPointIndex(pointIndex2);

								edge2.SetFaceIndex(faceIndex1);
								edge2.SetPointIndex(pointIndex1);

								break; // don't look for another one
							}
						}
						pOtherPointLink = &m_faces[faceIndex2].GetFaceEdgeLink(pointIndex2);
					}
				}
				pCurrentLink = &m_faces[faceIndex1].GetFaceEdgeLink(pointIndex1);
			}
		}
	}
}

// make sure the faces are connected together properly
void TupTriMeshConnectivity::CheckFaces(void)
{
	for (int faceIndex=0;faceIndex<m_numFaces;faceIndex++)
	{
		TriFace &face = m_faces[faceIndex];
		for (int edgeIndex=0;edgeIndex<3;edgeIndex++)
		{
			Edge &edge = face.GetEdge(edgeIndex);
			int nextFaceIndex = edge.GetFaceIndex();
			int nextFacePoint = edge.GetPointIndex();
			if (nextFaceIndex!=-1)
			{
				TriFace &nextFace = m_faces[nextFaceIndex];
				assert(nextFace.GetEdge(nextFacePoint).GetFaceIndex()==faceIndex);
				assert(nextFace.GetEdge(nextFacePoint).GetPointIndex()==edgeIndex);
			}
		}
	}
}


void TupTriMeshConnectivity::AddAllConnectedFaces(int seedFaceIndex,Keeper<int> &connectedFace) const
{
	TupArray<int> markedFaces;
	markedFaces.SetSize(m_numFaces);
	int faceIndex;
	// keep track of which faces were done
	for (faceIndex=0;faceIndex<m_numFaces;faceIndex++)
		markedFaces[faceIndex] = 0;
	markedFaces[seedFaceIndex] = 1;
	// add the seed face to the keeper
	connectedFace.Add(seedFaceIndex);

	// this stack keeps track of the recursion without killing the cpu stack
	std::stack<GetConnectedFacesStackEntry> findStack;

	GetConnectedFacesStackEntry stackEntry(seedFaceIndex);
	findStack.push(stackEntry);

	while(!findStack.empty())
	{
		// get last item off the stack
		GetConnectedFacesStackEntry &entry = findStack.top(); 
		faceIndex = entry.GetFaceIndex();
		int stage = entry.GetStage();
		const TriFace &face = m_faces[faceIndex];
		const Edge &edge = face.GetEdge(stage);
		int adjacentFaceIndex = edge.GetFaceIndex();
		stage++;
		if (stage<3)
		{
			entry.SetStage(stage); // set to go to the next edge on the face
		}
		else
		{
			findStack.pop(); // we are done with this face
		}
		if ((adjacentFaceIndex!=-1) && !markedFaces[adjacentFaceIndex])
		{
			// add the new face and continue
			connectedFace.Add(adjacentFaceIndex);
			markedFaces[adjacentFaceIndex] = 1;
			GetConnectedFacesStackEntry stackEntry(adjacentFaceIndex);
			findStack.push(stackEntry);
		}
	}
}

void TupTriMeshConnectivity::AddAllConnectedPositionIndices(int seedPositionIndex,TupArray<int> &connectedPositionIndices,bool bAddFacePoints,int maxDistance) const 
{
	TupArray<int> pointDistanceValues;

	GetAllConnectedPositionDistances(seedPositionIndex,pointDistanceValues,bAddFacePoints,maxDistance);

	for (int posIndex=0;posIndex<m_numPositionVerts;posIndex++)
	{
		if (pointDistanceValues[posIndex]!=-1)
		{
			connectedPositionIndices.Add(posIndex);
		}
	}
}

void TupTriMeshConnectivity::GetAllConnectedPositionDistances(int seedPositionIndex,TupArray<int> &pointDistanceValues,bool bAddFacePoints,int maxDistance) const 
{
	pointDistanceValues.SetSize(m_numPositionVerts);
	for (int posDistanceIndex=0;posDistanceIndex<m_numPositionVerts;posDistanceIndex++)
	{
		pointDistanceValues[posDistanceIndex]=-1;
	}

	// if set use the distances from the connected faces instead of edges
	if (bAddFacePoints)
	{
		TupArray<int> faceDistanceValues;
		GetAllConnectedPositionFaceDistances(seedPositionIndex,faceDistanceValues,maxDistance);
		int numFaces = m_faces.GetSize();
		for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
		{
			int distance = faceDistanceValues[faceIndex];

			for (int pointIndex=0;pointIndex<3;pointIndex++)
			{
				int posIndex = m_pPositionIndices[(faceIndex<<1)+faceIndex+pointIndex];
				if (pointDistanceValues[posIndex] ==-1 || distance<pointDistanceValues[posIndex])
				{
					pointDistanceValues[posIndex]=distance;
				}
			}
		}
	}
	else
	{
		// this stack keeps track of the recursion without killing the cpu stack
		std::stack<GetConnectedPointsStackEntry> findStack;
		
		int faceIndex = m_faceEdgeLinks[seedPositionIndex].GetNextLink().GetFaceIndex();
		int pointIndex = m_faceEdgeLinks[seedPositionIndex].GetNextLink().GetPointIndex();
		
		GetConnectedPointsStackEntry stackEntry(faceIndex,pointIndex,0);
		
		findStack.push(stackEntry);
		
		while(!findStack.empty())
		{
			// get last item off the stack
			GetConnectedPointsStackEntry &entry = findStack.top(); 
			faceIndex = entry.GetFaceIndex();
			pointIndex = entry.GetPointIndex();
			int distance = entry.GetDistance();
			
			int nextFaceIndex = m_faces[faceIndex].GetFaceEdgeLink(pointIndex).GetNextLink().GetFaceIndex();
			int nextPointIndex = m_faces[faceIndex].GetFaceEdgeLink(pointIndex).GetNextLink().GetPointIndex();
			
			// next face in edge link chain is valid
			if (nextFaceIndex!=-1)
			{
				entry.SetFaceIndex(nextFaceIndex);
				entry.SetPointIndex(nextPointIndex);
			}
			else
			{
				findStack.pop();
			}
			
			int posIndex = m_pPositionIndices[(faceIndex<<1)+faceIndex+pointIndex];
			
			// we haven't done this point yet or we have a smaller distance
			if (pointDistanceValues[posIndex]==-1 || distance<pointDistanceValues[posIndex])
			{
				pointDistanceValues[posIndex] = distance;
				
				if (distance<maxDistance)
				{
					int faceIndex3 = (faceIndex<<1)+faceIndex;
					int pointIndexNext = (pointIndex<2) ? pointIndex+1 : 0;
					
					int nextPointPosIndex = m_pPositionIndices[faceIndex3+pointIndexNext];
					GetConnectedPointsStackEntry newNextEntry(m_faceEdgeLinks[nextPointPosIndex].GetNextLink().GetFaceIndex(),
						m_faceEdgeLinks[nextPointPosIndex].GetNextLink().GetPointIndex(),
						distance+1);
					findStack.push(newNextEntry);
					
					int pointIndexPrev = (pointIndex>0) ? pointIndex-1 : 2;
					
					int prevPointPosIndex = m_pPositionIndices[faceIndex3+pointIndexPrev];
					GetConnectedPointsStackEntry newPrevEntry(m_faceEdgeLinks[prevPointPosIndex].GetNextLink().GetFaceIndex(),
						m_faceEdgeLinks[prevPointPosIndex].GetNextLink().GetPointIndex(),
						distance+1);
					findStack.push(newPrevEntry);
				}
			}
		}
	}
}

void TupTriMeshConnectivity::GetAllConnectedPositionFaceDistances(int seedPositionIndex,TupArray<int> &faceDistanceValues,int maxDistance) const 
{
	int numFaces = m_faces.GetSize();
	faceDistanceValues.SetSize(numFaces);

	int faceIndex;
	int pointIndex;
	for (faceIndex=0;faceIndex<numFaces;faceIndex++)
	{
		faceDistanceValues[faceIndex]=-1;
	}
	
	// this stack keeps track of the recursion without killing the cpu stack
	std::stack<GetConnectedPointsStackEntry> findStack;

	const FaceEdgeLink *pCurrentLink = &m_faceEdgeLinks[seedPositionIndex];
	while (pCurrentLink->Valid())
	{
		faceIndex = pCurrentLink->GetNextLink().GetFaceIndex();
		pointIndex = pCurrentLink->GetNextLink().GetPointIndex();

		faceDistanceValues[faceIndex]=0; // mark as close

		GetConnectedPointsStackEntry stackEntry(pCurrentLink->GetNextLink().GetFaceIndex(),
															 0, // not used here
															 0);
		findStack.push(stackEntry);

		pCurrentLink = &m_faces[faceIndex].GetFaceEdgeLink(pointIndex);
	}
	
	while(!findStack.empty())
	{
		// get last item off the stack
		GetConnectedPointsStackEntry &entry = findStack.top(); 
		faceIndex = entry.GetFaceIndex();
		pointIndex = entry.GetPointIndex();
		int distance = entry.GetDistance();

		if (pointIndex<2)
		{
			entry.SetPointIndex(pointIndex+1); // set to go to the next edge on the face
		}
		else
		{
			findStack.pop(); // we are done with this face
		}

		const Edge &edge = m_faces[faceIndex].GetEdge(pointIndex);
		int nextFaceIndex = edge.GetFaceIndex();
		int nextFacePointIndex = edge.GetPointIndex();
		if (nextFaceIndex!=-1)
		{
			// edge visible add one to distance
			int distanceToSet = distance + ((m_pFaceFlags[faceIndex] & (1<<pointIndex)) ? 1 : 0);
			if (distanceToSet<maxDistance)
			{
				if (faceDistanceValues[nextFaceIndex]==-1 || distance<faceDistanceValues[nextFaceIndex])
				{
					faceDistanceValues[nextFaceIndex]=distanceToSet;
					GetConnectedPointsStackEntry newEntry(nextFaceIndex,0,distanceToSet);
					findStack.push(newEntry);
				}
			}
		}
	}
}
