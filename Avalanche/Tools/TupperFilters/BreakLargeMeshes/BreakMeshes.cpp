////////////////////////////////////////////////////////////////////////////
//
// BreakMeshes
//
// Class to handle the breaking up of meshes into smaller ones
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "BreakLargeMeshesPch.h"
#include "BreakLargeMeshes.h"
#include "BreakMeshes.h"

BreakMeshesManager::BreakMeshesManager(TupNodePool &tupNodePool,
													TupObjectPool &tupObjectPool,
													TupMaterialPool &tupMaterialPool,
													TupMapPool &tupMapPool,
													TupImagePool &tupImagePool,
													TupModifierPool &tupModifierPool,
													int tupRootNodeRef,
													TupConfigRoot &tupConfigRoot,
													BreakUpSettings &settings) :
	m_tupNodePool(tupNodePool), 
	m_tupObjectPool(tupObjectPool), 
	m_tupMaterialPool(tupMaterialPool),
	m_tupMapPool(tupMapPool),
	m_tupImagePool(tupImagePool),
	m_tupModifierPool(tupModifierPool), 
	m_tupRootNodeRef(tupRootNodeRef), 
	m_tupConfigRoot(tupConfigRoot), 
	m_settings(settings)
{
}

void BreakMeshesManager::BreakUpMarkedNodesOnGrid(void)
{
	int totalNodes = m_tupNodePool.GetNumNodes();
	int nodeIndex;
	// this pass will count how many nodes reference each base mesh
	for (nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_tupNodePool.GetNode(nodeIndex);

		// if empty slot - continue
		if (!pNodeAgg)
			continue;

		// check if this node was marked to be broken up
		TupperwareScalar *pBreakUpFlagScalar = pNodeAgg->FindScalarByKey("BreakLargeMeshes_BreakUpFlag");
		if (!pBreakUpFlagScalar)
			continue;
		int breakUpFlag = pBreakUpFlagScalar->GetAsInt();
		if (!breakUpFlag)
			continue;

		// node is needing to be broken up
		Vector3 boundingBoxMin = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_BoundingBoxMin")->GetAsFloat();
		Vector3 boundingBoxMax = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_BoundingBoxMax")->GetAsFloat();

		float invGridSize = 1.0f / m_settings.m_gridSize;

		float startingGridPosX = m_settings.m_gridSize * ((int) (boundingBoxMin.x() * invGridSize));
		// if minus then move down one grid dimension
		if (startingGridPosX<0.0f && boundingBoxMin.x()!=startingGridPosX)
			startingGridPosX-=m_settings.m_gridSize;

		float startingGridPosZ = m_settings.m_gridSize * ((int) (boundingBoxMin.z() * invGridSize));
		// if minus then move down one grid dimension
		if (startingGridPosZ<0.0f && boundingBoxMin.z()!=startingGridPosZ)
			startingGridPosZ-=m_settings.m_gridSize;

		int numGridX = ((int)((boundingBoxMax.x() - startingGridPosX) * invGridSize))+1;
		int numGridZ = ((int)((boundingBoxMax.z() - startingGridPosZ) * invGridSize))+1;
		assert(numGridX>0 && numGridZ>0);

		// set up array for the face counts (keeps track of how many faces belong to each grid cell
		TupArray<TupArray<int> > faceCountArray;
		int gridIndexX,gridIndexZ;
		faceCountArray.SetSize(numGridX);
		for (gridIndexX=0;gridIndexX<numGridX;gridIndexX++)
		{
			faceCountArray[gridIndexX].SetSize(numGridZ);
			for (gridIndexZ=0;gridIndexZ<numGridZ;gridIndexZ++)
			{
				faceCountArray[gridIndexX][gridIndexZ] = 0; // no faces for now
			}
		}

		TupNode tupNode(pNodeAgg);
		char *pMeshName;
		tupNode.GetName(&pMeshName);

		int objectRef;	
		tupNode.GetObjectReference(objectRef);
		assert(objectRef!=-1);

		int baseRef = AnnotateHelper::FindBaseObjectRef(m_tupObjectPool,objectRef);

		TupperwareAggregate *pTransformAgg = tupNode.GetTransform();
		assert(pTransformAgg);

		Graphics4x4 mLocalToWorld;
		TupTransform tupTransform(pTransformAgg);
		if (tupTransform.GetTransformType()==TupTransform::STATIC_MATRIX)
		{
			TupStaticMatrix tupStaticMatrix(pTransformAgg);
			float *pMatrix;
			tupStaticMatrix.GetMatrix(&pMatrix);

			mLocalToWorld = Graphics4x4(
				pMatrix[0], pMatrix[1], pMatrix[2],
				pMatrix[4], pMatrix[5], pMatrix[6],
				pMatrix[8], pMatrix[9], pMatrix[10],
				pMatrix[12], pMatrix[13], pMatrix[14]);      // translation
		}
		else
		{
			mLocalToWorld.Identity(); // if we don't have a static matrix set to identity
		}

		TupperwareAggregate *pMeshAgg = m_tupObjectPool.GetObjectAggregate(baseRef);
		TupTriMesh tupTriMesh(pMeshAgg);

		TupperwareScalar *pMayaObjectNameScalar = pMeshAgg->FindScalarByKey("MayaObjectName");
		if (pMayaObjectNameScalar)
		{
			pMeshName = pMayaObjectNameScalar->GetAsString();
		}

		int numFaces;
		int *pPositionIndices;
		Vector3 *pPositionVerts;
		int numVerts;
		tupTriMesh.GetPositionVerts((float **)&pPositionVerts,numVerts);
		tupTriMesh.GetPositionIndices(&pPositionIndices,numFaces);

		// this array keeps track of which cell each face belongs to
		TupArray<int> faceCellIndexArray;
		faceCellIndexArray.SetSize(numFaces);

		// create an array of verts to hold the transformed positions
		TupArray<Vector3> tempVertsArray;
		tempVertsArray.SetSize(numVerts);
		for (int vertexIndex=0;vertexIndex<numVerts;vertexIndex++)
		{
			tempVertsArray[vertexIndex] = pPositionVerts[vertexIndex]*mLocalToWorld;
		}

		// go through each face and assign each to a cell as well as increment the counters
		int faceIndex;
		for (faceIndex=0;faceIndex<numFaces;faceIndex++)
		{
			int facePointIndex = (faceIndex<<1)+faceIndex;
			Vector3 faceCenterPos = tempVertsArray[pPositionIndices[facePointIndex+0]];
			faceCenterPos += tempVertsArray[pPositionIndices[facePointIndex+1]];
			faceCenterPos += tempVertsArray[pPositionIndices[facePointIndex+2]];
			faceCenterPos /= 3.0f;

			int gridPointX = (int)((faceCenterPos.x() - startingGridPosX) * invGridSize);
			int gridPointZ = (int)((faceCenterPos.z() - startingGridPosZ) * invGridSize);
			int faceCellValue = (gridPointX*numGridZ)+gridPointZ;
			faceCellIndexArray[faceIndex] = faceCellValue;
			faceCountArray[gridPointX][gridPointZ]++;
		}

		// count how many cells we are using
		int usedCellCount = 0;

		for (gridIndexX=0;gridIndexX<numGridX;gridIndexX++)
		{
			for (gridIndexZ=0;gridIndexZ<numGridZ;gridIndexZ++)
			{
				if (faceCountArray[gridIndexX][gridIndexZ])
					usedCellCount++;
			}
		}
		assert(usedCellCount);
		if (usedCellCount>1) // if more than one cell used
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Breaking up %s into %d parts based on grid.",pMeshName,usedCellCount+1);
			// create the copies of the node to have the faces deleted from
			CopyObjectsManager copyObjectsManager(m_tupNodePool,m_tupObjectPool,m_tupModifierPool,m_tupRootNodeRef);
			DeleteFaceManager deleteFaceManager(m_tupNodePool,m_tupObjectPool,m_tupModifierPool);

			int firstNodeCellValue;
			int firstNodeCellX;
			int firstNodeCellZ;

			usedCellCount = 0;
			for (gridIndexX=0;gridIndexX<numGridX;gridIndexX++)
			{
				for (gridIndexZ=0;gridIndexZ<numGridZ;gridIndexZ++)
				{
					if (faceCountArray[gridIndexX][gridIndexZ])
					{
						int faceCellValue = (gridIndexX*numGridZ)+gridIndexZ;
						// if we are the first one
						if (usedCellCount==0)
						{
							firstNodeCellValue = faceCellValue; // save this cell # for later (will delete faces after all others)
							firstNodeCellX = gridIndexX;
							firstNodeCellZ = gridIndexZ;
						}
						else
						{
							int newNodeRef = copyObjectsManager.CopyNode(nodeIndex); // make a copy

							TupperwareAggregate *pNewNodeAgg = m_tupNodePool.GetNode(newNodeRef);

							pNewNodeAgg->DeleteAtomsByKey("BreakLargeMeshes_BreakUpFlag");

							TupNode newNode(pNewNodeAgg);

							int objectRef;	
							newNode.GetObjectReference(objectRef);

							int baseRef = AnnotateHelper::FindBaseObjectRef(m_tupObjectPool,objectRef);
							TupperwareAggregate *pNewMeshAgg = m_tupObjectPool.GetObjectAggregate(baseRef);

							TupperwareScalar *pMayaObjectNameScalar = pNewMeshAgg->FindScalarByKey("MayaObjectName");
							if (pMayaObjectNameScalar)
							{
								TupString newMeshName;
								newMeshName.Format("%s(grid %d, %d)",pMayaObjectNameScalar->GetAsString(),gridIndexX,gridIndexZ);
								pMayaObjectNameScalar->SetAsString(newMeshName);
							}

							// delete the faces and keep only the ones marked with the faceCellValue
							deleteFaceManager.DeleteNodeFaces(faceCellIndexArray,faceCellValue,newNodeRef);
						}
						usedCellCount++;
					}
				}
			}
			// delete the faces of the first one
			if (pMayaObjectNameScalar)
			{
				TupString newMeshName;
				newMeshName.Format("%s(grid %d, %d)",pMayaObjectNameScalar->GetAsString(),firstNodeCellX,firstNodeCellZ);
				pMayaObjectNameScalar->SetAsString(newMeshName);
			}

			deleteFaceManager.DeleteNodeFaces(faceCellIndexArray,firstNodeCellValue,nodeIndex);
		}
	}
}

class VisibilitySortData
{
public:
	OBBInstance m_obb;
	int m_nodeRef;
	float m_size;
	
	VisibilitySortData(void) : m_nodeRef(-1),m_size(0)
	{
	}
	
	bool VisibilitySortData::operator==(const VisibilitySortData& other) const
	{
		return (m_size==other.m_size);
	}
	
	bool VisibilitySortData::operator<=(const VisibilitySortData& other) const
	{
		return (m_size<=other.m_size);
	}
	
	bool VisibilitySortData::operator>=(const VisibilitySortData& other) const
	{
		return (m_size>=other.m_size);
	}
	
	bool VisibilitySortData::operator<(const VisibilitySortData& other) const
	{
		return (m_size<other.m_size);
	}
	
	bool VisibilitySortData::operator>(const VisibilitySortData& other) const
	{
		return (m_size>other.m_size);
	}
};


void BreakMeshesManager::BreakUpMarkedNodesWithVisibilitySets(void)
{
	TupArray<VisibilitySortData> visibilitySets;

	int totalNodes = m_tupNodePool.GetNumNodes(); // initial number of nodes
	int nodeIndex;
	// this pass will build the list of visibility sets
	for (nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_tupNodePool.GetNode(nodeIndex);
		
		// if empty slot - continue
		if (!pNodeAgg)
			continue;
		
		//-- check to see if instance_type is visibility_region
		TupNode tupNode( pNodeAgg );
		char *pNodeAttributesString;
		tupNode.GetUserProperties( &pNodeAttributesString );
		
		TupKeyValue tupKeyValue( pNodeAttributesString, BEGIN_BLOCK, END_BLOCK );
		TupKeyValueContext tupKeyValueContext;
		tupKeyValueContext.AddKey( "instance_type" );
		tupKeyValueContext.AddKey( "cut_terrain" );
		const int INSTANCE_TYPE = 0;
		const int CUT_TERRAIN = 1;
		m_tupConfigRoot.GetKeyValueContexts( tupKeyValue, tupKeyValueContext, false );
		
		if ( tupKeyValueContext.ValueValidAt(INSTANCE_TYPE) && // this is a vis region
			  strcmp( tupKeyValueContext.GetValueAsString(INSTANCE_TYPE), "visibility_region" ) == 0 &&
			  tupKeyValueContext.ValueValidAt(CUT_TERRAIN) && // this is marked to cut
			  tupKeyValueContext.GetValueAsBool(CUT_TERRAIN))
		{
			VisibilitySortData visibilitySortData;

			visibilitySortData.m_obb.Orientation() = *(DirCos3x3 *)pNodeAgg->FindListByKey("Bounding_OBBOrientation")->GetAsFloat();
			visibilitySortData.m_obb.Position() = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_OBBCenter")->GetAsFloat();
			visibilitySortData.m_obb.GetAABB().maxV = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_OBBExtents")->GetAsFloat();
			visibilitySortData.m_obb.GetAABB().minV = - visibilitySortData.m_obb.GetAABB().maxV;
			visibilitySortData.m_nodeRef = nodeIndex;
			visibilitySortData.m_size = Bounding::GetVolume(visibilitySortData.m_obb.GetAABB().minV,visibilitySortData.m_obb.GetAABB().maxV);
			visibilitySets.Add(visibilitySortData);
		}
	}

	int numVisibilitySets = visibilitySets.GetSize();
	if (!numVisibilitySets) // if there aren't any to cut by then bail
		return;

	// sort so that the smallest ones are first
	TupSort<VisibilitySortData>::SortAscending(visibilitySets.GetData(),numVisibilitySets);

	int visibilitySetIndex;
	// realign the pointers inside the obbs to point to the correct data
	for (visibilitySetIndex=0;visibilitySetIndex<numVisibilitySets;visibilitySetIndex++)
	{
		visibilitySets[visibilitySetIndex].m_obb.AlignBase();
	}

	// now go through and see which ones can be cut
	for (nodeIndex=0;nodeIndex<totalNodes;nodeIndex++) // only go through the nodes that were here at the start
	{
		TupperwareAggregate *pNodeAgg = m_tupNodePool.GetNode(nodeIndex);

		// if empty slot - continue
		if (!pNodeAgg)
			continue;

		// check if this node was marked to be broken up
		TupperwareScalar *pBreakUpFlagScalar = pNodeAgg->FindScalarByKey("BreakLargeMeshes_BreakUpFlag");
		if (!pBreakUpFlagScalar)
			continue;
		int breakUpType = pBreakUpFlagScalar->GetAsInt();
		if (breakUpType==0)
			continue;

		Keeper<int> nodeVisibilitySets;
		TupArray<int> nodeVisibilitySetFaceCounts;
		int faceInWorldCount = 0 ;

		TupNode tupNode(pNodeAgg);
		char *pMeshName;
		tupNode.GetName(&pMeshName);

		int objectRef;	
		tupNode.GetObjectReference(objectRef);
		assert(objectRef!=-1);

		int baseRef = AnnotateHelper::FindBaseObjectRef(m_tupObjectPool,objectRef);

		TupperwareAggregate *pTransformAgg = tupNode.GetTransform();
		assert(pTransformAgg);

		Graphics4x4 mLocalToWorld;
		TupTransform tupTransform(pTransformAgg);
		if (tupTransform.GetTransformType()==TupTransform::STATIC_MATRIX)
		{
			TupStaticMatrix tupStaticMatrix(pTransformAgg);
			float *pMatrix;
			tupStaticMatrix.GetMatrix(&pMatrix);

			mLocalToWorld = Graphics4x4(
				pMatrix[0], pMatrix[1], pMatrix[2],
				pMatrix[4], pMatrix[5], pMatrix[6],
				pMatrix[8], pMatrix[9], pMatrix[10],
				pMatrix[12], pMatrix[13], pMatrix[14]);      // translation
		}
		else
		{
			mLocalToWorld.Identity(); // if we don't have a static matrix set to identity
		}

		TupperwareAggregate *pMeshAgg = m_tupObjectPool.GetObjectAggregate(baseRef);
		TupTriMesh tupTriMesh(pMeshAgg);

		TupperwareScalar *pMayaObjectNameScalar = pMeshAgg->FindScalarByKey("MayaObjectName");
		if (pMayaObjectNameScalar)
		{
			pMeshName = pMayaObjectNameScalar->GetAsString();
		}

		int numFaces;
		int *pPositionIndices;
		Vector3 *pPositionVerts;
		int numVerts;
		tupTriMesh.GetPositionVerts((float **)&pPositionVerts,numVerts);
		tupTriMesh.GetPositionIndices(&pPositionIndices,numFaces);

		// this array keeps track of which visibility set each face belongs to
		TupArray<int> faceVisibilitySetArray;
		faceVisibilitySetArray.SetSize(numFaces);

		// create an array of verts to hold the transformed positions
		TupArray<Vector3> tempVertsArray;
		tempVertsArray.SetSize(numVerts);
		for (int vertexIndex=0;vertexIndex<numVerts;vertexIndex++)
		{
			tempVertsArray[vertexIndex] = pPositionVerts[vertexIndex]*mLocalToWorld;
		}

		// go through each face and assign each to a visibility set as well as increment the counters
		int faceIndex;
		int visibilitySetIndex;

		if (breakUpType==1) // find smallest
		{
			for (faceIndex=0;faceIndex<numFaces;faceIndex++)
			{
				int facePointIndex = (faceIndex<<1)+faceIndex;
				Vector3 faceCenterPos = tempVertsArray[pPositionIndices[facePointIndex+0]];
				faceCenterPos += tempVertsArray[pPositionIndices[facePointIndex+1]];
				faceCenterPos += tempVertsArray[pPositionIndices[facePointIndex+2]];
				faceCenterPos /= 3.0f;

				faceVisibilitySetArray[faceIndex] = -1; // start with not in a visibility set

				// scan forwards from the smallest to the largest vis region
				for (visibilitySetIndex=0;visibilitySetIndex<numVisibilitySets;visibilitySetIndex++)
				{
					if (visibilitySets[visibilitySetIndex].m_obb.PointInBox(faceCenterPos))
					{
						int currentNumSets = nodeVisibilitySets.GetSize();
						int keeperIndex = nodeVisibilitySets.Add(visibilitySetIndex);
						// if we just added one
						if (keeperIndex == currentNumSets)
						{
							int newCount = 0;
							nodeVisibilitySetFaceCounts.Add(newCount);
						}
						faceVisibilitySetArray[faceIndex] = keeperIndex;
						nodeVisibilitySetFaceCounts[keeperIndex]++;
						break;
					}
				}
				// if not found
				if (faceVisibilitySetArray[faceIndex]==-1)
				{
					faceInWorldCount++;
				}
			}
		}
		else // find largest
		{
			for (faceIndex=0;faceIndex<numFaces;faceIndex++)
			{
				int facePointIndex = (faceIndex<<1)+faceIndex;
				Vector3 faceCenterPos = tempVertsArray[pPositionIndices[facePointIndex+0]];
				faceCenterPos += tempVertsArray[pPositionIndices[facePointIndex+1]];
				faceCenterPos += tempVertsArray[pPositionIndices[facePointIndex+2]];
				faceCenterPos /= 3.0f;
				
				faceVisibilitySetArray[faceIndex] = -1; // start with not in a visibility set
				
				// scan backwards from the largest to the smallest vis region
				for (visibilitySetIndex=numVisibilitySets-1;visibilitySetIndex>=0;visibilitySetIndex--)
				{
					if (visibilitySets[visibilitySetIndex].m_obb.PointInBox(faceCenterPos))
					{
						int currentNumSets = nodeVisibilitySets.GetSize();
						int keeperIndex = nodeVisibilitySets.Add(visibilitySetIndex);
						// if we just added one
						if (keeperIndex == currentNumSets)
						{
							int newCount = 0;
							nodeVisibilitySetFaceCounts.Add(newCount);
						}
						faceVisibilitySetArray[faceIndex] = keeperIndex;
						nodeVisibilitySetFaceCounts[keeperIndex]++;
						break;
					}
				}
				// if not found
				if (faceVisibilitySetArray[faceIndex]==-1)
				{
					faceInWorldCount++;
				}
			}
		}

		int numNodeVisibilitySets = nodeVisibilitySets.GetSize();
		// all faces are not in a visibility set
		if (numNodeVisibilitySets==0)
		{
			continue;
		}
		// if all of the faces are in 1 set
		else if (numNodeVisibilitySets==1 && faceInWorldCount==0)
		{
			// mark the mesh to be in this set
			pNodeAgg->AddScalarInt("BreakLargeMeshes_VisibilitySetPlaceNodeRef",NULL,visibilitySets[nodeVisibilitySets[0]].m_nodeRef);
			continue;
		}
		// if we need to cut up the mesh but leave some in the world
		else 
		{
			int startingVisibilitySetIndex = (faceInWorldCount>0) ? 0 : 1;
			bool bInitialMeshIsWorld = (faceInWorldCount>0);
			int numCopies = (faceInWorldCount>0) ? numNodeVisibilitySets : numNodeVisibilitySets-1;

			PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Breaking up %s into %d parts based on visiblity sets.",pMeshName,numCopies+1);
			// create the copies of the node to have the faces deleted from
			CopyObjectsManager copyObjectsManager(m_tupNodePool,m_tupObjectPool,m_tupModifierPool,m_tupRootNodeRef);
			DeleteFaceManager deleteFaceManager(m_tupNodePool,m_tupObjectPool,m_tupModifierPool);

			for (visibilitySetIndex=startingVisibilitySetIndex;visibilitySetIndex<numNodeVisibilitySets;visibilitySetIndex++)
			{
				int newNodeRef = copyObjectsManager.CopyNode(nodeIndex); // make a copy

				TupperwareAggregate *pNewNodeAgg = m_tupNodePool.GetNode(newNodeRef);

				pNewNodeAgg->DeleteAtomsByKey("BreakLargeMeshes_BreakUpFlag");

				TupNode newNode(pNewNodeAgg);

				int objectRef;	
				newNode.GetObjectReference(objectRef);

				int baseRef = AnnotateHelper::FindBaseObjectRef(m_tupObjectPool,objectRef);
				TupperwareAggregate *pNewMeshAgg = m_tupObjectPool.GetObjectAggregate(baseRef);

				TupperwareScalar *pMayaObjectNameScalar = pNewMeshAgg->FindScalarByKey("MayaObjectName");
				if (pMayaObjectNameScalar)
				{
					TupString newMeshName;
					newMeshName.Format("%s(Vis %d)",pMayaObjectNameScalar->GetAsString(),visibilitySetIndex);
					pMayaObjectNameScalar->SetAsString(newMeshName);
				}

				// delete the faces and keep only the ones marked with the faceCellValue
				deleteFaceManager.DeleteNodeFaces(faceVisibilitySetArray,visibilitySetIndex,newNodeRef);
				// assign to the visibility set
				pNewNodeAgg->AddScalarInt("BreakLargeMeshes_VisibilitySetPlaceNodeRef",NULL,visibilitySets[nodeVisibilitySets[visibilitySetIndex]].m_nodeRef);
			}
			// delete the faces of the first one
			if (bInitialMeshIsWorld)
			{
				if (pMayaObjectNameScalar)
				{
					TupString newMeshName;
					newMeshName.Format("%s(Vis World)",pMayaObjectNameScalar->GetAsString());
					pMayaObjectNameScalar->SetAsString(newMeshName);
				}

				deleteFaceManager.DeleteNodeFaces(faceVisibilitySetArray,-1,nodeIndex);
			}
			else
			{
				if (pMayaObjectNameScalar)
				{
					TupString newMeshName;
					newMeshName.Format("%s(Vis 0)",pMayaObjectNameScalar->GetAsString());
					pMayaObjectNameScalar->SetAsString(newMeshName);
				}

				deleteFaceManager.DeleteNodeFaces(faceVisibilitySetArray,0,nodeIndex);
				pNodeAgg->AddScalarInt("BreakLargeMeshes_VisibilitySetPlaceNodeRef",NULL,visibilitySets[nodeVisibilitySets[0]].m_nodeRef);
			}
		}
	}
}

void BreakMeshesManager::BreakUpMarkedNodesOnTextureUsage(void)
{
	TupArray<int> imageSizes;
	imageSizes.SetSize(m_tupImagePool.GetNumImages());
	for (int imageIndex=0;imageIndex<m_tupImagePool.GetNumImages();imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_tupImagePool.GetImage(imageIndex);
		if (!pImageAgg)
			continue;

		imageSizes[imageIndex] = pImageAgg->FindScalarByKey("Annotate_ImageSize")->GetAsInt();
		imageSizes[imageIndex]+= pImageAgg->FindScalarByKey("Annotate_ImageSizeMipMaps")->GetAsInt();
	}

	int totalNodes = m_tupNodePool.GetNumNodes();
	int nodeIndex;
	// this pass will count how many nodes reference each base mesh
	for (nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_tupNodePool.GetNode(nodeIndex);

		// if empty slot - continue
		if (!pNodeAgg)
			continue;

		// check if this node was marked to be broken up
		TupperwareScalar *pBreakUpFlagScalar = pNodeAgg->FindScalarByKey("BreakLargeMeshes_BreakUpFlag");
		if (!pBreakUpFlagScalar)
			continue;
		int breakUpFlag = pBreakUpFlagScalar->GetAsInt();
		if (!breakUpFlag)
			continue;

		TupNode tupNode(pNodeAgg);
		char *pMeshName;
		tupNode.GetName(&pMeshName);

		int objectRef;	
		tupNode.GetObjectReference(objectRef);
		assert(objectRef!=-1);

		int baseRef = AnnotateHelper::FindBaseObjectRef(m_tupObjectPool,objectRef);

		TupperwareAggregate *pMeshAgg = m_tupObjectPool.GetObjectAggregate(baseRef);
		TupTriMesh tupTriMesh(pMeshAgg);

		TupperwareScalar *pMayaObjectNameScalar = pMeshAgg->FindScalarByKey("MayaObjectName");
		if (pMayaObjectNameScalar)
		{
			pMeshName = pMayaObjectNameScalar->GetAsString();
		}

		StageFaceManager stageFaceManager(tupTriMesh);

		TupArray<Keeper<int> > usedImagesForMaterialIndex;

		int materialRef = tupTriMesh.GetBaseAggregate()->FindScalarByKey("Annotate_MaterialRef")->GetAsInt();
		if (materialRef==-1)
			continue;

		int *pMaterialIndices;
		int numFaces,faceIndex;
		tupTriMesh.GetFaceMaterialIndices(&pMaterialIndices,numFaces);
		int *pFaceFlags;
		tupTriMesh.GetFaceFlags(&pFaceFlags,numFaces);

		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Breaking up %s with %d faces based on texture usage.",pMeshName,numFaces);

		TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial(materialRef);
		TupMaterial tupMaterial(pMaterialAgg);
		if (tupMaterial.GetMaterialType() == TupMaterial::MULTI_SUB_OBJECT_MATERIAL)
		{
			TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMaterialAgg);
			int numSubMaterials = tupMultiSubObjectMaterial.GetNumSubMaterials();
			if (numSubMaterials)
			{
				usedImagesForMaterialIndex.SetSize(numSubMaterials);
				int subMaterialIndex;
				for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
				{
					TupperwareAggregate *pSubObjectMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(subMaterialIndex);
					TupMultiSubObjectMaterialSubMaterial tupMultiSubObjectMaterialSubMaterial(pSubObjectMaterialAgg);
					int subMaterialRef;
					tupMultiSubObjectMaterialSubMaterial.GetMaterialReference(subMaterialRef);
					if (subMaterialRef==-1)
						continue;
					AnnotateHelper::CalculateUsedImages(subMaterialRef,m_tupMaterialPool,m_tupMapPool,m_tupImagePool,usedImagesForMaterialIndex[subMaterialIndex]);
				}
			}
		}
		else
		{
			usedImagesForMaterialIndex.SetSize(1);
			AnnotateHelper::CalculateUsedImages(materialRef,m_tupMaterialPool,m_tupMapPool,m_tupImagePool,usedImagesForMaterialIndex[0]);
		}
		// this array indicates which group each face belongs to (-1 means not assigned)
		TupArray<int> faceGroupArray;
		faceGroupArray.SetSize(numFaces);
		for (faceIndex=0;faceIndex<numFaces;faceIndex++)
		{
			faceGroupArray[faceIndex] = -1; // mark as unassigned
		}

		TupArray<int> faceImageSize;
		faceImageSize.SetSize(numFaces);

		TupArray<TupArray<int> > faceImageRefs;
		faceImageRefs.SetSize(numFaces);

		// calculate the used images for each face as well as the image size 
		for (faceIndex=0;faceIndex<numFaces;faceIndex++)
		{
			// only worry about first stage faces
			if ((pFaceFlags[faceIndex] & TupTriMesh::FLAGS_FACE_STAGE_MASK)!=0)
				continue;

			int numStages = stageFaceManager.GetNumStages(faceIndex);
			for (int stageIndex = 0;stageIndex<numStages;stageIndex++)
			{
				int stageFaceIndex = stageFaceManager.GetStageFace(faceIndex,stageIndex);
				int faceMaterialIndex = pMaterialIndices[stageFaceIndex];
				assert(faceMaterialIndex>=0 && faceMaterialIndex<usedImagesForMaterialIndex.GetSize());
				for (int imageIndex=0;imageIndex<usedImagesForMaterialIndex[faceMaterialIndex].GetSize();imageIndex++)
				{
					int imageRef = usedImagesForMaterialIndex[faceMaterialIndex][imageIndex];
					bool bFoundMatch = false;
					int numTestImageRefs = faceImageRefs[faceIndex].GetSize();
					for (int testIndex = 0;testIndex<numTestImageRefs;testIndex++)
					{
						if (faceImageRefs[faceIndex][testIndex] == imageRef)
						{
							bFoundMatch = true;
							break;
						}
					}
					if (!bFoundMatch)
					{
						faceImageRefs[faceIndex].Add(imageRef);
						faceImageSize[faceIndex]+=imageSizes[imageRef];
					}
				}
			}
		}

		int currentGroupIndex=0;
		while(1)
		{
			int largestFaceToUse = -1;
			int largestFaceToUseSize;

			// find the face that uses the most textures
			for (faceIndex=0;faceIndex<numFaces;faceIndex++)
			{
				// only ones that are not used
				if (faceGroupArray[faceIndex]!=-1)
					continue;

				// only worry about first stage faces
				if ((pFaceFlags[faceIndex] & TupTriMesh::FLAGS_FACE_STAGE_MASK)!=0)
					continue;

				if (largestFaceToUse==-1 || faceImageSize[faceIndex]>largestFaceToUseSize)
				{
					largestFaceToUse = faceIndex;
					largestFaceToUseSize = faceImageSize[faceIndex];
				}
			}
			// if no faces left then done
			if (largestFaceToUse==-1)
			{
				break;
			}

			int numFacesInGroup = 0;

			int numStages = stageFaceManager.GetNumStages(largestFaceToUse);
			for (int stageIndex = 0;stageIndex<numStages;stageIndex++)
			{
				int stageFaceIndex = stageFaceManager.GetStageFace(largestFaceToUse,stageIndex);
				faceGroupArray[stageFaceIndex] = currentGroupIndex;
				numFacesInGroup++;
			}

			TupArray<int> groupImageRefs = faceImageRefs[largestFaceToUse];
         int groupImageSize = faceImageSize[largestFaceToUse];

			// loop through and find any other faces that don't add to the groups size
			// as well as the best one that will add size
			while(1)
			{
				int bestAddFaceIndex = -1;
				float bestAddFaceFactor;

				// calculate the used images for each face as well as the image size 
				for (faceIndex=0;faceIndex<numFaces;faceIndex++)
				{
					// only ones that are not used
					if (faceGroupArray[faceIndex]!=-1)
						continue;

					// only worry about first stage faces
					if ((pFaceFlags[faceIndex] & TupTriMesh::FLAGS_FACE_STAGE_MASK)!=0)
						continue;

					int numImagesFoundInGroup = 0;
					int numImagesFoundInGroupSize = 0;
					int numImagesNotFoundInGroup = 0;
					int numImagesNotFoundInGroupSize = 0;

					int numImagesInFace = faceImageRefs[faceIndex].GetSize();
					for (int imageIndex=0;imageIndex<numImagesInFace;imageIndex++)
					{
						int imageRef = faceImageRefs[faceIndex][imageIndex];
						bool bImageFoundInGroup = false;
						for (int groupImageIndex=0;groupImageIndex<groupImageRefs.GetSize();groupImageIndex++)
						{
							if (imageRef == groupImageRefs[groupImageIndex])
							{
								bImageFoundInGroup = true;
								break;
							}
						}
						if (bImageFoundInGroup)
						{
							numImagesFoundInGroup++;
							numImagesFoundInGroupSize+=imageSizes[imageRef];
						}
						else
						{
							numImagesNotFoundInGroup++;
							numImagesNotFoundInGroupSize+=imageSizes[imageRef];
						}
					}

					// if this face does not add to the group
					if (numImagesNotFoundInGroup==0)
					{
						int numStages = stageFaceManager.GetNumStages(faceIndex);
						for (int stageIndex = 0;stageIndex<numStages;stageIndex++)
						{
							int stageFaceIndex = stageFaceManager.GetStageFace(faceIndex,stageIndex);
							faceGroupArray[stageFaceIndex] = currentGroupIndex;
							numFacesInGroup++;
						}
					}
					// if this face could add to the group
					else if (numImagesNotFoundInGroupSize+groupImageSize<=(m_settings.m_cutOnTextureUsageLimit<<10))
					{
						float	fitFactor = ((float)numImagesFoundInGroupSize)/((float)numImagesNotFoundInGroupSize);
						if (bestAddFaceIndex == -1 || fitFactor>bestAddFaceFactor)
						{
							bestAddFaceIndex = faceIndex;
							bestAddFaceFactor = fitFactor;
						}
					}
				}
				if (bestAddFaceIndex==-1) // no more faces that can fit in group
				{
					break;
				}
				else // add face that adds to the groups size
				{
					int numStages = stageFaceManager.GetNumStages(bestAddFaceIndex);
					for (int stageIndex = 0;stageIndex<numStages;stageIndex++)
					{
						int stageFaceIndex = stageFaceManager.GetStageFace(bestAddFaceIndex,stageIndex);
						faceGroupArray[stageFaceIndex] = currentGroupIndex;
						numFacesInGroup++;
					}

					int numImagesInFace = faceImageRefs[bestAddFaceIndex].GetSize();
					for (int imageIndex=0;imageIndex<numImagesInFace;imageIndex++)
					{
						int imageRef = faceImageRefs[bestAddFaceIndex][imageIndex];
						bool bImageFoundInGroup = false;
						for (int groupImageIndex=0;groupImageIndex<groupImageRefs.GetSize();groupImageIndex++)
						{
							if (imageRef == groupImageRefs[groupImageIndex])
							{
								bImageFoundInGroup = true;
								break;
							}
						}
						if (!bImageFoundInGroup)
						{
							groupImageRefs.Add(imageRef);
							groupImageSize+=imageSizes[imageRef];
						}
					}
					// loop and try again
				}
			}

			if (groupImageSize>(m_settings.m_cutOnTextureUsageLimit<<10))
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE,"Warning Group %d has %d faces that use too much texture space.",currentGroupIndex,numFacesInGroup);
			}
			else
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Group %d has %d faces.",currentGroupIndex,numFacesInGroup);
			}

			for (int imageIndex=0;imageIndex<groupImageRefs.GetSize();imageIndex++)
			{
				int imageRef = groupImageRefs[imageIndex];
				TupperwareAggregate *pImageAgg = m_tupImagePool.GetImage(imageRef);
				char *pFilePath = pImageAgg->FindScalarByKey("Annotate_ActualFilePath")->GetAsString();
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"  Texture %s uses %d bytes.",pFilePath,imageSizes[imageRef]);
			}
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"  Total of %d bytes used.",groupImageSize);

			currentGroupIndex++;
		}
		int numGroups = currentGroupIndex;
		if (numGroups>1)
		{
			// create the copies of the node to have the faces deleted from
			CopyObjectsManager copyObjectsManager(m_tupNodePool,m_tupObjectPool,m_tupModifierPool,m_tupRootNodeRef);
			DeleteFaceManager deleteFaceManager(m_tupNodePool,m_tupObjectPool,m_tupModifierPool);

			for (int groupIndex=1;groupIndex<numGroups;groupIndex++)
			{
				int newNodeRef = copyObjectsManager.CopyNode(nodeIndex); // make a copy

				TupperwareAggregate *pNewNodeAgg = m_tupNodePool.GetNode(newNodeRef);

				pNewNodeAgg->DeleteAtomsByKey("BreakLargeMeshes_BreakUpFlag");

				TupNode newNode(pNewNodeAgg);

				int objectRef;	
				newNode.GetObjectReference(objectRef);

				int baseRef = AnnotateHelper::FindBaseObjectRef(m_tupObjectPool,objectRef);
				TupperwareAggregate *pNewMeshAgg = m_tupObjectPool.GetObjectAggregate(baseRef);

				TupperwareScalar *pMayaObjectNameScalar = pNewMeshAgg->FindScalarByKey("MayaObjectName");
				if (pMayaObjectNameScalar)
				{
					TupString newMeshName;
					newMeshName.Format("%s(Tex %d)",pMayaObjectNameScalar->GetAsString(),groupIndex);
					pMayaObjectNameScalar->SetAsString(newMeshName);
				}

				// delete the faces and keep only the ones marked with the faceCellValue
				deleteFaceManager.DeleteNodeFaces(faceGroupArray,groupIndex,newNodeRef);
			}
			// change the name of the first mesh
			if (pMayaObjectNameScalar)
			{
				TupString newMeshName;
				newMeshName.Format("%s(Tex 0)",pMeshName);
				pMayaObjectNameScalar->SetAsString(newMeshName);
			}

			// delete the faces of the first one
			deleteFaceManager.DeleteNodeFaces(faceGroupArray,0,nodeIndex);
		}
	}
}
