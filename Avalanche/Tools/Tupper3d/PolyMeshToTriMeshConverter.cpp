////////////////////////////////////////////////////////////////////////////
//
// PolyMeshToTriMeshConverter
//
// Handle the conversion from a polymesh to a trimesh
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
#include "PolyMeshToTriMeshConverter.h"

bool TupPolyMeshToTriMeshConverter::Convert(int objectRef,TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool,TupArray<TupArray<int> > &derivedObjectsRefs)
{
	TupPolyMeshToTriMeshConverter Converter(tupObjectPool,tupModifierPool);
	return Converter.ConvertData(objectRef,derivedObjectsRefs[objectRef]);
}

TupPolyMeshToTriMeshConverter::TupPolyMeshToTriMeshConverter(TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool) :
	m_tupObjectPool(tupObjectPool),
	m_tupModifierPool(tupModifierPool)
{
}

static bool IsOutsideEdge(int index1,int index2,int totalPoints)
{
//	return true; // if we want it to not combine...
	// check for adjacent indices
	
	if (index1+1==index2) // 1->2
	{
		return true;
	}
	else if (index1-1==index2) // 2->1
	{
		return true;
	}
	else if (index1==0 && (index2+1==totalPoints))  // 0->5 (if totalPoints was 6)
	{
		return true;
	}
	else if ((index1+1==totalPoints) && (index2==0)) // 5->0 (if totalPoints was 6)
	{
		return true;
	}
	return false;
}

bool TupPolyMeshToTriMeshConverter::ConvertData(int objectRef,const TupArray<int> &derivedObjectRefs)
{
	TupperwareAggregate *pObjectAgg = m_tupObjectPool.GetObjectAggregate(objectRef);
	if (!pObjectAgg)
		return false;

	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()!=TupObject::POLY_MESH)
		return false;

	TupPolyMesh tupPolyMesh(pObjectAgg);

	int numIndices;
	int numFaces;

	int numPositionVerts;
	TupVertex3f *pPositionVerts;
	int *pPositionIndices;

	int numNormalVerts;
	TupVertex3f *pNormalVerts;
	int *pNormalIndices;

	int *pFaceVertexCounts;

	int numFaceNormalVerts;
	TupVertex3f *pFaceNormalVerts;
	int *pFaceNormalIndices;

	int *pFaceFlags;
	int *pFaceMaterialIndices;

	int *pFaceTriangleCounts;
	int *pTrianglePointIndices;
	int numTrianglePointIndices;

	BuildTriangleData(tupPolyMesh);

	tupPolyMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);
	tupPolyMesh.GetPositionIndices(&pPositionIndices,numIndices);
	tupPolyMesh.GetNormalVerts((float **)&pNormalVerts,numNormalVerts);
	tupPolyMesh.GetNormalIndices(&pNormalIndices,numIndices);
	TupperwareList *pFaceVertexCountsList = tupPolyMesh.GetFaceVertexCounts(&pFaceVertexCounts,numFaces);
	tupPolyMesh.GetFaceNormalVerts((float **)&pFaceNormalVerts,numFaceNormalVerts);
	tupPolyMesh.GetFaceNormalIndices(&pFaceNormalIndices,numFaces);
	tupPolyMesh.GetFaceFlags(&pFaceFlags,numFaces);
	tupPolyMesh.GetFaceMaterialIndices(&pFaceMaterialIndices,numFaces);
	TupperwareList *pFaceTriangleCountsList = tupPolyMesh.GetFaceTriangleCounts(&pFaceTriangleCounts,numFaces);
	TupperwareList *pTrianglePointIndicesList = tupPolyMesh.GetTrianglePointIndices(&pTrianglePointIndices,numTrianglePointIndices);

	TupArray<int> triPositionIndices;
	TupArray<int> triNormalIndices;
	Keeper<TupVertex3f> triFaceNormalVerts;
	TupArray<int> triFaceNormalIndices;
	TupArray<int> triFaceFlags;
	TupArray<int> triFaceMaterialIndices;
	TupArray<int> triFaceShadingGroups;

	int faceIndex;
	int totalTriangles=0;
	for (faceIndex=0;faceIndex<numFaces;faceIndex++)
	{
		totalTriangles += pFaceTriangleCounts[faceIndex];
	}

	int totalTriangles3 = (totalTriangles<<1)+totalTriangles;
	triPositionIndices.SetSize(totalTriangles3);
	triNormalIndices.SetSize(totalTriangles3);
	triFaceNormalIndices.SetSize(totalTriangles);
	triFaceFlags.SetSize(totalTriangles);
	triFaceMaterialIndices.SetSize(totalTriangles);
	triFaceShadingGroups.SetSize(totalTriangles);

	int numMeshMaps = tupPolyMesh.GetNumMeshMaps();
	TupArray<PolyMeshMap> polyMeshMaps;
	TupArray<TupArray<int> > triMeshMapsIndices;
	polyMeshMaps.SetSize(numMeshMaps);
	triMeshMapsIndices.SetSize(numMeshMaps);
	int meshMapIndex;
	for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
	{
		triMeshMapsIndices[meshMapIndex].SetSize(totalTriangles3);
		TupperwareAggregate *pPolyMeshMapAgg = tupPolyMesh.GetMeshMap(meshMapIndex);
		TupPolyMeshMap tupPolyMeshMap(pPolyMeshMapAgg);
		polyMeshMaps[meshMapIndex].m_mapChannel = tupPolyMeshMap.GetMapChannel();
		tupPolyMeshMap.GetMapName(&polyMeshMaps[meshMapIndex].m_pMapName);
		tupPolyMeshMap.GetMapFlags(polyMeshMaps[meshMapIndex].m_mapFlags);
		tupPolyMeshMap.GetMapIndices(&polyMeshMaps[meshMapIndex].m_pMapIndices,polyMeshMaps[meshMapIndex].m_numIndices);
		tupPolyMeshMap.GetMapVerts((float **)&polyMeshMaps[meshMapIndex].m_pMapVerts,polyMeshMaps[meshMapIndex].m_numVerts);
	}

	int polyIndexOffset = 0;
	int polyTriIndex = 0;
	int polyTriIndex3 = 0;
	for (faceIndex=0;faceIndex<numFaces;faceIndex++)
	{
		int numVertsInPoly = pFaceVertexCounts[faceIndex];
		int numTrisInPoly = pFaceTriangleCounts[faceIndex];

		int triangleIndex;
		int triangleIndex3;
		for (triangleIndex=0,triangleIndex3=0;triangleIndex<numTrisInPoly;triangleIndex++,triangleIndex3+=3)
		{
			// what poly points is this triangle?
			int polyIndex1 = pTrianglePointIndices[polyTriIndex3+triangleIndex3];
			int polyIndex2 = pTrianglePointIndices[polyTriIndex3+triangleIndex3+1];
			int polyIndex3 = pTrianglePointIndices[polyTriIndex3+triangleIndex3+2];
			// position indices
			triPositionIndices[polyTriIndex3+triangleIndex3] = pPositionIndices[polyIndexOffset+polyIndex1];
			triPositionIndices[polyTriIndex3+triangleIndex3+1] = pPositionIndices[polyIndexOffset+polyIndex2];
			triPositionIndices[polyTriIndex3+triangleIndex3+2] = pPositionIndices[polyIndexOffset+polyIndex3];
			// normal indices
			triNormalIndices[polyTriIndex3+triangleIndex3] = pNormalIndices[polyIndexOffset+polyIndex1];
			triNormalIndices[polyTriIndex3+triangleIndex3+1] = pNormalIndices[polyIndexOffset+polyIndex2];
			triNormalIndices[polyTriIndex3+triangleIndex3+2] = pNormalIndices[polyIndexOffset+polyIndex3];
			// face normals
			TupVertex3f faceNormal = TupVertex3f::ComputeFaceNormal(pPositionVerts[triPositionIndices[polyTriIndex3+triangleIndex3]],
																					pPositionVerts[triPositionIndices[polyTriIndex3+triangleIndex3+1]],
																					pPositionVerts[triPositionIndices[polyTriIndex3+triangleIndex3+2]]);
			triFaceNormalIndices[polyTriIndex+triangleIndex] = triFaceNormalVerts.Add(faceNormal);
			// face flags
			int faceFlags = 0;
			int sourceFaceFlags = pFaceFlags[faceIndex];
			if (sourceFaceFlags & TupPolyMesh::FLAGS_FACE_HIDDEN)
				faceFlags |= TupTriMesh::FLAGS_FACE_HIDDEN;
			if (IsOutsideEdge(polyIndex1,polyIndex2,numVertsInPoly))
				faceFlags |= TupTriMesh::FLAGS_EDGE_0_VISIBLITY;
			if (IsOutsideEdge(polyIndex2,polyIndex3,numVertsInPoly))
				faceFlags |= TupTriMesh::FLAGS_EDGE_1_VISIBLITY;
			if (IsOutsideEdge(polyIndex3,polyIndex1,numVertsInPoly))
				faceFlags |= TupTriMesh::FLAGS_EDGE_2_VISIBLITY;
			triFaceFlags[polyTriIndex+triangleIndex] = faceFlags;

			triFaceMaterialIndices[polyTriIndex+triangleIndex] = pFaceMaterialIndices[faceIndex]; // set material index
			triFaceShadingGroups[polyTriIndex+triangleIndex] = 1; // shading groups (use 1 for now)

			// mesh maps
			for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
			{
				triMeshMapsIndices[meshMapIndex][polyTriIndex3+triangleIndex3] = polyMeshMaps[meshMapIndex].m_pMapIndices[polyIndexOffset+polyIndex1];
				triMeshMapsIndices[meshMapIndex][polyTriIndex3+triangleIndex3+1] = polyMeshMaps[meshMapIndex].m_pMapIndices[polyIndexOffset+polyIndex2];
				triMeshMapsIndices[meshMapIndex][polyTriIndex3+triangleIndex3+2] = polyMeshMaps[meshMapIndex].m_pMapIndices[polyIndexOffset+polyIndex3];
			}
		}
		polyIndexOffset += numVertsInPoly;
		polyTriIndex3 += (numTrisInPoly<<1)+numTrisInPoly;
		polyTriIndex += numTrisInPoly;
	}

	// change type
	tupObject.SetObjectType(TupObject::TRI_MESH);
	TupTriMesh tupTriMesh(pObjectAgg);

	// replace portions to tri mesh data
	tupTriMesh.AddPositionIndices(triPositionIndices.GetData(),totalTriangles);
	tupTriMesh.AddNormalIndices(triNormalIndices.GetData(),totalTriangles);
	tupTriMesh.AddFaceNormalIndices(triFaceNormalIndices.GetData(),totalTriangles);
	tupTriMesh.AddFaceNormalVerts((float *)triFaceNormalVerts.GetData(),triFaceNormalVerts.GetSize());
	tupTriMesh.AddFaceFlags(triFaceFlags.GetData(),totalTriangles);
	tupTriMesh.AddFaceMaterialIndices(triFaceMaterialIndices.GetData(),totalTriangles);
	tupTriMesh.AddFaceShadingGroups(triFaceShadingGroups.GetData(),totalTriangles);
	// mesh maps
	for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
	{
		TupperwareAggregate *pMeshMapAgg = tupTriMesh.GetMeshMap(meshMapIndex);
		TupTriMeshMap tupMeshMap(pMeshMapAgg);
		tupMeshMap.AddMapIndices(triMeshMapsIndices[meshMapIndex].GetData(),totalTriangles);
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
				if (!ConvertModifier(pDerivedModifierAgg,numFaces,totalTriangles,pFaceVertexCounts,pFaceTriangleCounts,pTrianglePointIndices))
				{
					return false;
				}
			}
		}
	}

	// delete atoms that are not needed
	pFaceVertexCountsList->Detatch();
	delete pFaceVertexCountsList;
	pFaceTriangleCountsList->Detatch();
	delete pFaceTriangleCountsList;
	pTrianglePointIndicesList->Detatch();
	delete pTrianglePointIndicesList;

	return true;
}

bool TupPolyMeshToTriMeshConverter::ConvertModifier(TupperwareAggregate *pDerivedModifierAgg,
																	 int numFaces,
																	 int numTriangles,
																	 int *pFaceVertexCounts,
																	 int *pFaceTriangleCounts,
																	 int *pTrianglePointIndices)
{
	int numTriangles3 = (numTriangles<<1)+numTriangles;
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
					assert(numFaces==numAttributeFaces);

					TupArray<int> triFaceAttributeIndices;
					triFaceAttributeIndices.SetSize(numTriangles);

					int polyIndexOffset = 0;
					int polyTriIndex = 0;
					for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
					{
						int numVertsInPoly = pFaceVertexCounts[faceIndex];
						int numTrisInPoly = pFaceTriangleCounts[faceIndex];
						
						int triangleIndex;
						for (triangleIndex=0;triangleIndex<numTrisInPoly;triangleIndex++)
						{
							triFaceAttributeIndices[polyTriIndex+triangleIndex] = pFaceAttributeIndices[faceIndex];
						}
						polyIndexOffset += numVertsInPoly;
						polyTriIndex += numTrisInPoly;
					}
					// replace with indices for triangles
					tupFaceAttributesModifierContext.AddFaceAttributeIndices(triFaceAttributeIndices.GetData(),numTriangles);
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

						TupArray<int> triColorIndices;
						triColorIndices.SetSize(numTriangles3);

						int *pAlphaIndices=NULL;
						if (flags & TupApplyVCModifier::FLAGS_HAS_ALPHA)
						{
							tupApplyVCModifier.GetAlphaIndices(&pAlphaIndices,numModifierIndices);
						}
						TupArray<int> triAlphaIndices;
						triAlphaIndices.SetSize(numTriangles3);

						int polyIndexOffset = 0;
						int polyTriIndex3 = 0;
						for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
						{
							int numVertsInPoly = pFaceVertexCounts[faceIndex];
							int numTrisInPoly = pFaceTriangleCounts[faceIndex];
							
							int triangleIndex;
							int triangleIndex3;
							for (triangleIndex=0,triangleIndex3=0;triangleIndex<numTrisInPoly;triangleIndex++,triangleIndex3+=3)
							{
								// what poly points is this triangle?
								int polyIndex1 = pTrianglePointIndices[polyTriIndex3+triangleIndex3];
								int polyIndex2 = pTrianglePointIndices[polyTriIndex3+triangleIndex3+1];
								int polyIndex3 = pTrianglePointIndices[polyTriIndex3+triangleIndex3+2];
								
								triColorIndices[polyTriIndex3+triangleIndex3] = pColorIndices[polyIndexOffset+polyIndex1];
								triColorIndices[polyTriIndex3+triangleIndex3+1] = pColorIndices[polyIndexOffset+polyIndex2];
								triColorIndices[polyTriIndex3+triangleIndex3+2] = pColorIndices[polyIndexOffset+polyIndex3];

								if (flags & TupApplyVCModifier::FLAGS_HAS_ALPHA)
								{
									triAlphaIndices[polyTriIndex3+triangleIndex3] = pAlphaIndices[polyIndexOffset+polyIndex1];
									triAlphaIndices[polyTriIndex3+triangleIndex3+1] = pAlphaIndices[polyIndexOffset+polyIndex2];
									triAlphaIndices[polyTriIndex3+triangleIndex3+2] = pAlphaIndices[polyIndexOffset+polyIndex3];
								}
							}
							polyIndexOffset += numVertsInPoly;
							polyTriIndex3 += (numTrisInPoly<<1)+numTrisInPoly;
						}
						// replace with indices for triangles
						tupApplyVCModifier.AddColorIndices(triColorIndices.GetData(),numTriangles3);
						if (flags & TupApplyVCModifier::FLAGS_HAS_ALPHA)
						{
							tupApplyVCModifier.AddAlphaIndices(triAlphaIndices.GetData(),numTriangles3);
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

bool TupPolyMeshToTriMeshConverter::BuildTriangleData(TupPolyMesh &tupPolyMesh)
{
	int numFaces;

	int *pFaceTriangleCounts;
	int *pTrianglePointIndices;
	int numTrianglePointIndices;

	TupperwareList *pFaceTriangleCountsList = tupPolyMesh.GetFaceTriangleCounts(&pFaceTriangleCounts,numFaces);
	TupperwareList *pTrianglePointIndicesList = tupPolyMesh.GetTrianglePointIndices(&pTrianglePointIndices,numTrianglePointIndices);

	if (pFaceTriangleCountsList && pTrianglePointIndices)
		return true;

	int numIndices;
	
	int numPositionVerts;
	TupVertex3f *pPositionVerts;
	int *pPositionIndices;
	
	int *pFaceVertexCounts;

	TupVertex3f *pFaceNormalVerts;
	int numFaceNormalVerts;
	int *pFaceNormalIndices;
	
	tupPolyMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);
	tupPolyMesh.GetPositionIndices(&pPositionIndices,numIndices);
	tupPolyMesh.GetFaceVertexCounts(&pFaceVertexCounts,numFaces);

	tupPolyMesh.GetFaceNormalVerts((float **)&pFaceNormalVerts,numFaceNormalVerts);
	tupPolyMesh.GetFaceNormalIndices(&pFaceNormalIndices,numFaces);

	TupArray<int> faceTriangleCounts;
	TupArray<int> trianglePointIndices;

	int posIndex=0;

	for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
	{
		TupVertex3f &faceNormal = pFaceNormalVerts[pFaceNormalIndices[faceIndex]];

		int numPoints = pFaceVertexCounts[faceIndex];
		int i,j,k,l;
		TupVertex3f right,left,cross,hyp;
		float dotProduct,lengthSq;
		bool foundBest;
		int bestPoint1,bestPoint2,bestPoint3;
		double bestFactor;
		TupVertex3f point21Vector,point23Vector,point13Vector;
		TupVertex3f testVector;

		TupArray<bool> pointUsedArray;
		pointUsedArray.SetSize(numPoints);
		for (i=0;i<numPoints;i++)
		{
			pointUsedArray[i] = false;
		}
		int pointsLeft = numPoints;
		while (pointsLeft>2)
		{
			foundBest = false;

			for (i=0;i<numPoints;i++)
			{
				if (pointUsedArray[i])
					continue;
				// i is the first point in the triangle
				j=i+1<numPoints ? i+1 : 0;
				while(pointUsedArray[j])
				{
					j=j+1<numPoints ? j+1 : 0;
				}
				// j is the second point
				k=j+1<pointUsedArray.GetSize() ? j+1 : 0;
				while(pointUsedArray[k])
				{
					k=k+1<numPoints ? k+1 : 0;
				}
				// k is the third point
				TupVertex3f &point1 = pPositionVerts[pPositionIndices[i+posIndex]];
				TupVertex3f &point2 = pPositionVerts[pPositionIndices[j+posIndex]];
				TupVertex3f &point3 = pPositionVerts[pPositionIndices[k+posIndex]];
				// put this one in if we don't have any yet
				if (!foundBest)
				{
					bestFactor = 3.402823466e+38f;
					foundBest = true;
					bestPoint1 = i;
					bestPoint2 = j;
					bestPoint3 = k;
				}

				right = point3-point2;
				right.Normalize();
				left = point1-point2;
				left.Normalize();
				// Take the cross product to get a vector normal to the face.
				cross = TupVertex3f::CrossProduct(right,left);
				lengthSq = cross.LengthSq();
				if (lengthSq<1.0e-4) // if in a line don't do
					continue;

				dotProduct = TupVertex3f::DotProduct(cross,faceNormal);
				// if new face normal is pointing away from the current face normal we have a problem
				if (dotProduct<0.0) 
					continue;
				hyp = point3-point1;
				double hypLengthSq = hyp.LengthSq();

				// if new edge is really really small then kick out
				if (hypLengthSq<1.0e-4)
					continue;

				// factor is based on how close the face is perpendicular to the face normal,
				// how close the angle at point 2 is from 90 degrees
				// how small the edge between point 1 and point 3 is
				double factor = hypLengthSq; // (dotProduct*lengthSq)/hypLengthSq;

				point21Vector = TupVertex3f::CrossProduct(cross,left);
				point23Vector = TupVertex3f::CrossProduct(cross,right);
				point13Vector = TupVertex3f::CrossProduct(cross,hyp);
				bool foundProblem = false;
				// loop through remaining points and see if any lie within triangle
				for (l=0;l<numPoints;l++)
				{
					if (l==i||l==j||l==k)
						continue;
					if (pointUsedArray[l])
						continue;
					TupVertex3f &testPoint = pPositionVerts[pPositionIndices[l+posIndex]];
					testVector = testPoint-point1;
					dotProduct = TupVertex3f::DotProduct(point13Vector,testVector);
					if (dotProduct>1.0e-4) // on correct side of plane (ok)
						continue;
					testVector = testPoint-point2;
					dotProduct = TupVertex3f::DotProduct(point21Vector,testVector);
					if (dotProduct>1.0e-4) // on correct side of plane (ok)
						continue;
					dotProduct = TupVertex3f::DotProduct(point23Vector,testVector);
					if (dotProduct<-1.0e-4) // on correct side of plane (ok)
						continue;
					foundProblem = true; // if all are on incorrect side then we have a problem
					break;
				}

				if (foundProblem)
					continue;

				if (bestFactor>factor)
				{
					bestFactor = factor;
					foundBest = true;
					bestPoint1 = i;
					bestPoint2 = j;
					bestPoint3 = k;
				}
			}
			assert(foundBest);
			trianglePointIndices.Add(bestPoint1);
			trianglePointIndices.Add(bestPoint2);
			trianglePointIndices.Add(bestPoint3);
			pointUsedArray[bestPoint2] = true; // make point as gone
			pointsLeft--;
		}
		faceTriangleCounts.Add(numPoints-2);
		posIndex+=numPoints;
	}

	tupPolyMesh.AddFaceTriangleCounts(faceTriangleCounts.GetData(),numFaces);
	tupPolyMesh.AddTrianglePointIndices(trianglePointIndices.GetData(),trianglePointIndices.GetSize());

	return true;
}

