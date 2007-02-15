////////////////////////////////////////////////////////////////////////////
//
// TriMeshToPolyMeshConverter
//
// Handle the conversion from a trimesh to a polymesh
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "object.h"
#include "trimesh.h"
#include "trimeshmap.h"
#include "polymesh.h"
#include "polymeshmap.h"
#include "objectpool.h"
#include "modifierpool.h"
#include "tupperware/keeper.h"
#include "TupVertex.h"
#include "modifier.h"
#include "derived.h"
#include "derivedmodifier.h"
#include "faceattributesmodifiercontext.h"
#include "applyvcmodifier.h"
#include "TriMeshConnectivity.h"
#include "TriMeshConnectivityParts.h"
#include "TriMeshToPolyMeshConverter.h"


bool TupTriMeshToPolyMeshConverter::Convert(int objectRef,TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool,TupArray<TupArray<int> > &derivedObjectsRefs)
{
	TupTriMeshToPolyMeshConverter Converter(tupObjectPool,tupModifierPool);
	return Converter.ConvertData(objectRef,derivedObjectsRefs[objectRef]);
}
	
TupTriMeshToPolyMeshConverter::TupTriMeshToPolyMeshConverter(TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool) :
	m_tupObjectPool(tupObjectPool),
	m_tupModifierPool(tupModifierPool)
{
}

bool TupTriMeshToPolyMeshConverter::ConvertData(int objectRef,const TupArray<int> &derivedObjectRefs)
{
	TupperwareAggregate *pObjectAgg = m_tupObjectPool.GetObjectAggregate(objectRef);
	if (!pObjectAgg)
		return false;
	
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()!=TupObject::TRI_MESH)
		return false;
	
	TupTriMesh tupTriMesh(pObjectAgg);
	
	int numFaces;

	int numPositionVerts;
	TupVertex3f *pPositionVerts;
	int *pPositionIndices;
	
	int numNormalVerts;
	TupVertex3f *pNormalVerts;
	int *pNormalIndices;
	
	int numFaceNormalVerts;
	TupVertex3f *pFaceNormalVerts;
	int *pFaceNormalIndices;
	int *pFaceShadingGroups;
	
	int *pFaceFlags;
	int *pFaceMaterialIndices;
	
	tupTriMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);
	tupTriMesh.GetPositionIndices(&pPositionIndices,numFaces);
	tupTriMesh.GetNormalVerts((float **)&pNormalVerts,numNormalVerts);
	tupTriMesh.GetNormalIndices(&pNormalIndices,numFaces);
	tupTriMesh.GetFaceNormalVerts((float **)&pFaceNormalVerts,numFaceNormalVerts);
	tupTriMesh.GetFaceNormalIndices(&pFaceNormalIndices,numFaces);
	tupTriMesh.GetFaceFlags(&pFaceFlags,numFaces);
	tupTriMesh.GetFaceMaterialIndices(&pFaceMaterialIndices,numFaces);
	TupperwareList *pFaceShadingGroupsList = tupTriMesh.GetFaceShadingGroups(&pFaceShadingGroups,numFaces);

	int numMeshMaps = tupTriMesh.GetNumMeshMaps();
	TupArray<TriMeshMap> triMeshMaps;
	TupArray<TupArray<int> > polyMeshMapsIndices;
	triMeshMaps.SetSize(numMeshMaps);
	polyMeshMapsIndices.SetSize(numMeshMaps);
	int meshMapIndex;
	for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
	{
		TupperwareAggregate *pTriMeshMapAgg = tupTriMesh.GetMeshMap(meshMapIndex);
		TupTriMeshMap tupTriMeshMap(pTriMeshMapAgg);
		triMeshMaps[meshMapIndex].m_mapChannel = tupTriMeshMap.GetMapChannel();
		tupTriMeshMap.GetMapName(&triMeshMaps[meshMapIndex].m_pMapName);
		tupTriMeshMap.GetMapFlags(triMeshMaps[meshMapIndex].m_mapFlags);
		tupTriMeshMap.GetMapIndices(&triMeshMaps[meshMapIndex].m_pMapIndices,triMeshMaps[meshMapIndex].m_numIndices);
		tupTriMeshMap.GetMapVerts((float **)&triMeshMaps[meshMapIndex].m_pMapVerts,triMeshMaps[meshMapIndex].m_numVerts);
	}
	
	TupArray<int> polyPositionIndices;
	TupArray<int> polyNormalIndices;
	Keeper<TupVertex3f> polyFaceNormalVerts;
	TupArray<int> polyFaceNormalIndices;
	TupArray<int> polyFaceFlags;
	TupArray<int> polyFaceMaterialIndices;

	TupArray<int> polyFaceVertexCounts;
	TupArray<int> polyFaceTriangleCounts;
	TupArray<int> polyTrianglePointIndices;

	TupArray<int> polyPointTriFaceIndices; // map a poly point to a triangle face
	TupArray<int> polyPointTriFacePointIndices; // map a poly point to a triangle face point
	TupArray<int> polyTriFaceIndices; // map a poly index to a triangle face index

	TupTriMeshConnectivity tupTriMeshConnectivity;
	tupTriMeshConnectivity.SetMesh(tupTriMesh,TupTriMeshConnectivity::FLAGS_FACE_CHECK_MATERIAL_INDEX);

	TupArray<bool> faceDoneFlags;
	faceDoneFlags.SetSize(numFaces);

	int faceIndex;
	int faceTriOffset;
	for (faceIndex=0,faceTriOffset=0;faceIndex<numFaces;faceIndex++,faceTriOffset+=3)
	{
		int faceEdgeVisiblity = pFaceFlags[faceIndex]&EDGE_VISIBILITY_MASK;
		// find a face that is not used as well as has a visible edge (and does not have all 3 sides visible)
		if (!faceDoneFlags[faceIndex] && faceEdgeVisiblity!=EDGE_VISIBILITY_ALL && faceEdgeVisiblity != EDGE_VISIBILITY_NONE)
		{
			Keeper<int> facesToAdd;
			Keeper<int> pointsToAdd;

			facesToAdd.Add(faceIndex);

			int edgeToSearch;
			int faceToSearch = faceIndex;
			switch (faceEdgeVisiblity)
			{
				case EDGE_VISIBILITY_0:
					pointsToAdd.Add(pPositionIndices[faceTriOffset]);
					pointsToAdd.Add(pPositionIndices[faceTriOffset+1]);
					edgeToSearch = 1;
					break;
				case EDGE_VISIBILITY_1:
					pointsToAdd.Add(pPositionIndices[faceTriOffset+1]);
					pointsToAdd.Add(pPositionIndices[faceTriOffset+2]);
					edgeToSearch = 2;
					break;
				case EDGE_VISIBILITY_0_1:
					pointsToAdd.Add(pPositionIndices[faceTriOffset+0]);
					pointsToAdd.Add(pPositionIndices[faceTriOffset+1]);
					pointsToAdd.Add(pPositionIndices[faceTriOffset+2]);
					edgeToSearch = 2;
					break;
				case EDGE_VISIBILITY_2:
					pointsToAdd.Add(pPositionIndices[faceTriOffset+2]);
					pointsToAdd.Add(pPositionIndices[faceTriOffset+0]);
					edgeToSearch = 0;
					break;
				case EDGE_VISIBILITY_0_2:
					pointsToAdd.Add(pPositionIndices[faceTriOffset+2]);
					pointsToAdd.Add(pPositionIndices[faceTriOffset+0]);
					pointsToAdd.Add(pPositionIndices[faceTriOffset+1]);
					edgeToSearch = 1;
					break;
				case EDGE_VISIBILITY_1_2:
					pointsToAdd.Add(pPositionIndices[faceTriOffset+1]);
					pointsToAdd.Add(pPositionIndices[faceTriOffset+2]);
					pointsToAdd.Add(pPositionIndices[faceTriOffset+0]);
					edgeToSearch = 0;
					break;
				default:
					assert(false);
			}
			bool bIsLoop = false;
			while (1)
			{
				TupTriMeshConnectivity::TriFace &face = tupTriMeshConnectivity.GetFace(faceToSearch);
				TupTriMeshConnectivity::Edge &edge = face.GetEdge(edgeToSearch);
				int adjacentFaceIndex = edge.GetFaceIndex();
				if (adjacentFaceIndex==-1) // if no adjacent face then we have a problem
					break;
				if (faceDoneFlags[adjacentFaceIndex]) // if we did this face before then we have a problem
					break;

				facesToAdd.Add(adjacentFaceIndex);

				int adjacentFacePoint = edge.GetPointIndex();
				adjacentFacePoint = adjacentFacePoint<2 ? adjacentFacePoint+1 : 0; // up one

				faceEdgeVisiblity = pFaceFlags[adjacentFaceIndex]&EDGE_VISIBILITY_MASK;
				// is this edge on the edge of the poly?
				if (faceEdgeVisiblity & (1<<adjacentFacePoint))
				{
					int pointToAdd = adjacentFacePoint<2 ? adjacentFacePoint+1 : 0; // up another one
					int numOldPoints = pointsToAdd.GetSize();
					int newIndex = pointsToAdd.Add(pPositionIndices[(adjacentFaceIndex<<1)+adjacentFaceIndex+pointToAdd]);
					// if we have looped around then stop
					if (newIndex!=numOldPoints)
					{
						// if first one (we looped back!)
						if (newIndex==0)
							bIsLoop = true;
						break; // break if we didn't add a new point
					}
					// if the next one on the edge as well
					if (faceEdgeVisiblity & (1<<pointToAdd))
					{
						pointToAdd = pointToAdd<2 ? pointToAdd+1 : 0; // up another one
						int numOldPoints = pointsToAdd.GetSize();
						newIndex = pointsToAdd.Add(pPositionIndices[(adjacentFaceIndex<<1)+adjacentFaceIndex+pointToAdd]);
						// if we have looped around then stop
						if (newIndex!=numOldPoints)
						{
							// if first one (we looped back!)
							if (newIndex==0)
								bIsLoop = true;
							break; // break if we didn't add a new point
						}
						// if we have a problem (3 edge flags set
						if (faceEdgeVisiblity & (1<<pointToAdd))
						{
							break;
						}
					}
					faceToSearch = adjacentFaceIndex;
					edgeToSearch = pointToAdd;
				}
				// face inside of poly (part of a fan at current point
				else
				{
					// search around point
					faceToSearch = adjacentFaceIndex;
					edgeToSearch = adjacentFacePoint;
				}
			}
			if (bIsLoop && (pointsToAdd.GetSize()-2)!=facesToAdd.GetSize())
			{
				bIsLoop = false; // not a valid loop if the counts do not match
			}
			if (bIsLoop)
			{

				int numPointsToAdd = pointsToAdd.GetSize();
				int numTrianglesToAdd = facesToAdd.GetSize();

				polyFaceVertexCounts.Add(numPointsToAdd);
				polyFaceTriangleCounts.Add(numTrianglesToAdd);

				int polyPointBaseIndex = polyPointTriFaceIndices.GetSize();
				int polyBaseIndex = polyTriFaceIndices.GetSize();

				// this data keeps track of what points on tris were used to build the poly
				polyPointTriFaceIndices.SetSize(polyPointBaseIndex+numPointsToAdd);
				polyPointTriFacePointIndices.SetSize(polyPointBaseIndex+numPointsToAdd);
				polyTriFaceIndices.Add(facesToAdd[0]);

				TupVertex3f faceNormal;
				for (int triangleIndex=0;triangleIndex<numTrianglesToAdd;triangleIndex++)
				{
					int triangleFaceIndex = facesToAdd[triangleIndex];
					// mark so we don't do it again
					faceDoneFlags[triangleFaceIndex] = true;

					int triangleFaceIndex3 = (triangleFaceIndex<<1)+triangleFaceIndex;
					for (int pointIndex=0;pointIndex<3;pointIndex++)
					{
						int polyIndex = pointsToAdd.GetIndex(pPositionIndices[triangleFaceIndex3+pointIndex]);
						assert(polyIndex!=-1);
						polyPointTriFaceIndices[polyPointBaseIndex+polyIndex] = triangleFaceIndex;
						polyPointTriFacePointIndices[polyPointBaseIndex+polyIndex] = pointIndex;
						polyTrianglePointIndices.Add(polyIndex);
					}
					faceNormal+=pFaceNormalVerts[pFaceNormalIndices[triangleFaceIndex]];
				}
				faceNormal.Normalize();
				polyFaceNormalIndices.Add(polyFaceNormalVerts.Add(faceNormal));
				int faceFlags = 0;
				polyFaceFlags.Add(faceFlags);
				polyFaceMaterialIndices.Add(pFaceMaterialIndices[facesToAdd[0]]);

				for (int polyPointIndex=0;polyPointIndex<numPointsToAdd;polyPointIndex++)
				{
					int triangleFaceIndex = polyPointTriFaceIndices[polyPointBaseIndex+polyPointIndex];
					int triangleFaceIndex3 = (triangleFaceIndex<<1)+triangleFaceIndex;
					int trianglePointIndex = polyPointTriFacePointIndices[polyPointBaseIndex+polyPointIndex];
					polyPositionIndices.Add(pPositionIndices[triangleFaceIndex3+trianglePointIndex]);
					polyNormalIndices.Add(pNormalIndices[triangleFaceIndex3+trianglePointIndex]);

					for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
					{
						polyMeshMapsIndices[meshMapIndex].Add(triMeshMaps[meshMapIndex].m_pMapIndices[triangleFaceIndex3+trianglePointIndex]);
					}
				}
			}
			// handle non loop (treat each faces as a 3 sided poly)
			else
			{
				int numTrianglesToAdd = facesToAdd.GetSize();
				
				const int numPointsPerPoly = 3;
				const int numTrisPerPoly = 1;
				
				for (int triangleIndex=0;triangleIndex<numTrianglesToAdd;triangleIndex++)
				{
					polyFaceVertexCounts.Add(numPointsPerPoly);
					polyFaceTriangleCounts.Add(numTrisPerPoly);

					int triangleFaceIndex = facesToAdd[triangleIndex];
					// mark so we don't do it again
					faceDoneFlags[triangleFaceIndex] = true;

					// this data keeps track of what points on tris were used to build the poly
					polyTriFaceIndices.Add(triangleFaceIndex);
					
					int triangleFaceIndex3 = (triangleFaceIndex<<1)+triangleFaceIndex;
					for (int pointIndex=0;pointIndex<3;pointIndex++)
					{
						polyPointTriFaceIndices.Add(triangleFaceIndex);
						polyPointTriFacePointIndices.Add(pointIndex);

						polyPositionIndices.Add(pPositionIndices[triangleFaceIndex3+pointIndex]);
						polyNormalIndices.Add(pNormalIndices[triangleFaceIndex3+pointIndex]);
						
						for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
						{
							polyMeshMapsIndices[meshMapIndex].Add(triMeshMaps[meshMapIndex].m_pMapIndices[triangleFaceIndex3+pointIndex]);
						}
						polyTrianglePointIndices.Add(pointIndex);
					}
					polyFaceNormalIndices.Add(polyFaceNormalVerts.Add(pFaceNormalVerts[pFaceNormalIndices[triangleFaceIndex]]));
					int faceFlags = 0;
					polyFaceFlags.Add(faceFlags);
					polyFaceMaterialIndices.Add(pFaceMaterialIndices[triangleFaceIndex]);
				}
			}
		}
	}
	// loop through and do all the faces that are not done yet
	for (faceIndex=0,faceTriOffset=0;faceIndex<numFaces;faceIndex++,faceTriOffset+=3)
	{
		if (!faceDoneFlags[faceIndex])
		{
			const int numPointsPerPoly = 3;
			const int numTrisPerPoly = 1;
				
			polyFaceVertexCounts.Add(numPointsPerPoly);
			polyFaceTriangleCounts.Add(numTrisPerPoly);

			polyTriFaceIndices.Add(faceIndex);
			
			int triangleFaceIndex3 = (faceIndex<<1)+faceIndex;
			for (int pointIndex=0;pointIndex<3;pointIndex++)
			{
				polyPointTriFaceIndices.Add(faceIndex);
				polyPointTriFacePointIndices.Add(pointIndex);

				polyPositionIndices.Add(pPositionIndices[triangleFaceIndex3+pointIndex]);
				polyNormalIndices.Add(pNormalIndices[triangleFaceIndex3+pointIndex]);
				
				for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
				{
					polyMeshMapsIndices[meshMapIndex].Add(triMeshMaps[meshMapIndex].m_pMapIndices[triangleFaceIndex3+pointIndex]);
				}
				polyTrianglePointIndices.Add(pointIndex);
			}
			polyFaceNormalIndices.Add(polyFaceNormalVerts.Add(pFaceNormalVerts[pFaceNormalIndices[faceIndex]]));
			int faceFlags = 0;
			polyFaceFlags.Add(faceFlags);
			polyFaceMaterialIndices.Add(pFaceMaterialIndices[faceIndex]);
		}
	}

	// change type
	tupObject.SetObjectType(TupObject::POLY_MESH);
	TupPolyMesh tupPolyMesh(pObjectAgg);
	
	// replace portions to tri mesh data
	tupPolyMesh.AddPositionIndices(polyPositionIndices.GetData(),polyPositionIndices.GetSize());
	tupPolyMesh.AddNormalIndices(polyNormalIndices.GetData(),polyNormalIndices.GetSize());
	tupPolyMesh.AddFaceNormalIndices(polyFaceNormalIndices.GetData(),polyFaceNormalIndices.GetSize());
	tupPolyMesh.AddFaceNormalVerts((float *)polyFaceNormalVerts.GetData(),polyFaceNormalVerts.GetSize());
	tupPolyMesh.AddFaceFlags(polyFaceFlags.GetData(),polyFaceFlags.GetSize());
	tupPolyMesh.AddFaceMaterialIndices(polyFaceMaterialIndices.GetData(),polyFaceMaterialIndices.GetSize());
	tupPolyMesh.AddFaceVertexCounts(polyFaceVertexCounts.GetData(),polyFaceVertexCounts.GetSize());
	tupPolyMesh.AddFaceTriangleCounts(polyFaceTriangleCounts.GetData(),polyFaceTriangleCounts.GetSize());
	tupPolyMesh.AddTrianglePointIndices(polyTrianglePointIndices.GetData(),polyTrianglePointIndices.GetSize());

	// mesh maps
	for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
	{
		TupperwareAggregate *pMeshMapAgg = tupPolyMesh.GetMeshMap(meshMapIndex);
		TupPolyMeshMap tupPolyMeshMap(pMeshMapAgg);
		tupPolyMeshMap.AddMapIndices(polyMeshMapsIndices[meshMapIndex].GetData(),polyMeshMapsIndices[meshMapIndex].GetSize());
	}

	int numDerivedObjectRefs = derivedObjectRefs.GetSize();
	for (int derivedObjectIndex=0;derivedObjectIndex<numDerivedObjectRefs;derivedObjectIndex++)
	{
		int derivedObjectRef = derivedObjectRefs[derivedObjectIndex];
		TupperwareAggregate *pDerivedObjectAgg = m_tupObjectPool.GetObjectAggregate(derivedObjectRef);
		if (!pDerivedObjectAgg)
			return false;
		
		TupObject tupObject(pDerivedObjectAgg);
		if (tupObject.GetObjectType()!=TupObject::DERIVED)
			return false;
		
		TupDerived tupDerived(pDerivedObjectAgg);
		int totalModifierSlots = tupDerived.GetNumModifierSlots();
		// loop through the modifiers and see if any deal with the vertex colors
		for (int modifierIndex=0;modifierIndex<totalModifierSlots;modifierIndex++)
		{
			TupperwareAggregate *pDerivedModifierAgg = tupDerived.GetModifierBySlot(modifierIndex);
			if (pDerivedModifierAgg)
			{
				if (!ConvertModifier(pDerivedModifierAgg,numFaces,polyTriFaceIndices,polyPointTriFaceIndices,polyPointTriFacePointIndices))
				{
					return false;
				}
			}
		}
	}

	pFaceShadingGroupsList->Detatch();
	delete pFaceShadingGroupsList;

	return true;
}

bool TupTriMeshToPolyMeshConverter::ConvertModifier(TupperwareAggregate *pDerivedModifierAgg,
																	 int numFaces,
																	 const TupArray<int> polyTriFaceIndices,
																	 const TupArray<int> polyPointTriFaceIndices,
																	 const TupArray<int> polyPointTriFacePointIndices)
{
	int numPolys = polyTriFaceIndices.GetSize();
	int numTotalPolyPoints = polyPointTriFaceIndices.GetSize();

	TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
	int modifierReference;
	tupDerivedModifier.GetModifierReference(modifierReference);
	TupperwareAggregate *pModifierAgg = m_tupModifierPool.GetModifier(modifierReference);
	TupModifier tupModifier(pModifierAgg);
	switch(tupModifier.GetModifierType())
	{
			case TupModifier::FACE_ATTRIBUTE_MODIFIER:
				{
					TupperwareAggregate *pModifierContextAgg = tupDerivedModifier.GetModifierContext();
					TupFaceAttributesModifierContext tupFaceAttributesModifierContext(pModifierContextAgg);

					int *pFaceAttributeIndices;
					int numAttributeFaces;
					tupFaceAttributesModifierContext.GetFaceAttributeIndices(&pFaceAttributeIndices,numAttributeFaces);

					TupArray<int> polyFaceAttributeIndices;
					polyFaceAttributeIndices.SetSize(numPolys);

					int polyIndexOffset = 0;
					int polyTriIndex = 0;
					for (int polyIndex=0;polyIndex<numPolys;polyIndex++)
					{
						int triFaceIndex = polyTriFaceIndices[polyIndex];
						if (triFaceIndex<numAttributeFaces)
						{
							polyFaceAttributeIndices[polyIndex] = pFaceAttributeIndices[triFaceIndex];
						}
						else
						{
							polyFaceAttributeIndices[polyIndex] = 0;
						}
					}
					// replace with indices for triangles
					tupFaceAttributesModifierContext.AddFaceAttributeIndices(polyFaceAttributeIndices.GetData(),numPolys);
				}
				break;
			case TupModifier::APPLY_VC_MODIFIER:
				{
					TupApplyVCModifier tupApplyVCModifier(pModifierAgg);
					int flags;
					tupApplyVCModifier.GetFlags(flags);

					// if per point per face color
					if (!(flags & TupApplyVCModifier::FLAGS_MIXED_COLORS))
					{
						int numModifierIndices;
						int *pColorIndices;
						tupApplyVCModifier.GetColorIndices(&pColorIndices,numModifierIndices);

						TupArray<int> polyColorIndices;
						polyColorIndices.SetSize(numTotalPolyPoints);

						int *pAlphaIndices=NULL;
						if (flags & TupApplyVCModifier::FLAGS_HAS_ALPHA)
						{
							tupApplyVCModifier.GetAlphaIndices(&pAlphaIndices,numModifierIndices);
						}
						TupArray<int> polyAlphaIndices;
						polyAlphaIndices.SetSize(numTotalPolyPoints);

						int polyIndexOffset = 0;
						int polyTriIndex = 0;
						int polyTriIndex3 = 0;
						for (int polyPointIndex=0;polyPointIndex<numTotalPolyPoints;polyPointIndex++)
						{
							int triFaceIndex = polyPointTriFaceIndices[polyPointIndex];
							int triFaceIndex3 = (triFaceIndex<<1)+triFaceIndex;
							polyColorIndices[polyPointIndex] = pColorIndices[triFaceIndex3+polyPointTriFacePointIndices[polyPointIndex]];

							if (flags & TupApplyVCModifier::FLAGS_HAS_ALPHA)
							{
								polyAlphaIndices[polyPointIndex] = pAlphaIndices[triFaceIndex3+polyPointTriFacePointIndices[polyPointIndex]];
							}
						}
						// replace with indices for triangles
						tupApplyVCModifier.AddColorIndices(polyColorIndices.GetData(),numTotalPolyPoints);
						if (flags & TupApplyVCModifier::FLAGS_HAS_ALPHA)
						{
							tupApplyVCModifier.AddAlphaIndices(polyAlphaIndices.GetData(),numTotalPolyPoints);
						}
					}
				}
				break;
			// these I can leave alone
			case TupModifier::CLOTH_MODIFIER:
			case TupModifier::SKIN_MODIFIER:
				break;
			// these are a problem (for now)
			case TupModifier::VERTEX_PAINT_MODIFIER:
			case TupModifier::HOLD_VC_MODIFIER:
			default:
				return false;
	}
	return true;
}
