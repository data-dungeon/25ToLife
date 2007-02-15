////////////////////////////////////////////////////////////////////////////
//
// MergeMesh
//
// Class to handle merging meshes together into one
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "CombineMeshesPch.h"
#include "MergeMesh.h"

MergeMeshesManager::MergeMeshesManager(TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,
													TupModifierPool &tupModifierPool,TupMaterialPool &tupMaterialPool,
													TupConfigRoot &tupNodeConfigRoot,TupConfigRoot &tupFaceConfigRoot) :
	m_tupNodePool(tupNodePool),m_tupObjectPool(tupObjectPool),
	m_tupModifierPool(tupModifierPool),m_tupMaterialPool(tupMaterialPool),
	m_tupNodeConfigRoot(tupNodeConfigRoot),m_tupFaceConfigRoot(tupFaceConfigRoot),
	m_positionToleranceSq(0.0f),m_positionTolerance(0.0f)
{
}

void MergeMeshesManager::SetPositionTolerance(float positionTolerance)
{
	m_positionTolerance = positionTolerance;
	m_positionToleranceSq = positionTolerance*positionTolerance;
}

void MergeMeshesManager::AddInstances(const TupArray<int> &instanceRefs)
{
	int numInstanceRefs = instanceRefs.GetSize();
	assert(numInstanceRefs>1);
	m_subMeshDataArray.SetSize(numInstanceRefs-1);
	GetMeshData(instanceRefs[0],m_baseMeshData);
	CopyCollisionAttributesToFaces(m_baseMeshData);
	RemoveCollisionAttributeFromInstance(m_baseMeshData);

	TupMatrix4x4d baseToWorldMatrix = TupMatrix4x4Utils::Convert(m_baseMeshData.m_localToWorldMatrix);
	TupMatrix4x4d worldToBaseMatrix;
	baseToWorldMatrix.Inverse(worldToBaseMatrix);

	for (int subInstanceIndex=1;subInstanceIndex<numInstanceRefs;subInstanceIndex++)
	{
		MeshData &subMeshData = m_subMeshDataArray[subInstanceIndex-1];
		GetMeshData(instanceRefs[subInstanceIndex],subMeshData);
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"  Merging Sub Instance node %s",(const char *)subMeshData.m_nodeName);

		CopyCollisionAttributesToFaces(subMeshData);

		if (!m_baseMeshData.m_localToWorldMatrix.Compare(subMeshData.m_localToWorldMatrix))
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"    Transforming into space of base node");
			TupMatrix4x4d localToWorldMatrix = TupMatrix4x4Utils::Convert(subMeshData.m_localToWorldMatrix);
			TupMatrix4x4d localToBaseMatrix;
			// set up the local to base matrix
			worldToBaseMatrix.Multiply(localToWorldMatrix,localToBaseMatrix);
			// transform sub mesh to local coordinate system of base
			TupArray<TupVertex3d> tempPositionVerts,tempNormalVerts,tempFaceNormalVerts;
			TupVertex3Utils::Convert(tempPositionVerts,subMeshData.m_positionVerts);
			TupVertex3Utils::Convert(tempNormalVerts,subMeshData.m_normalVerts.GetArray());
			TupVertex3Utils::Convert(tempFaceNormalVerts,subMeshData.m_faceNormalVerts.GetArray());
			localToBaseMatrix.TransformVertices(tempPositionVerts);
			localToBaseMatrix.TransformNormals(tempNormalVerts);
			localToBaseMatrix.TransformNormals(tempFaceNormalVerts);
			TupVertex3Utils::Convert(subMeshData.m_positionVerts,tempPositionVerts);
			TupVertex3Utils::Convert(subMeshData.m_normalVerts.GetArray(),tempNormalVerts);
			TupVertex3Utils::Convert(subMeshData.m_faceNormalVerts.GetArray(),tempFaceNormalVerts);
		}
		MergeMeshToBase(m_baseMeshData,subMeshData);
	}
}

void MergeMeshesManager::AddToScene(int parentNodeRef)
{
	TupperwareAggregate *pTupParentNodeAgg = m_tupNodePool.GetNode(parentNodeRef);
	TupNode tupParentNode(pTupParentNodeAgg);
	TupperwareAggregate *pTupBaseNodeAgg = m_tupNodePool.GetNode(m_baseMeshData.m_nodeRef);
	TupNode tupBaseNode(pTupBaseNodeAgg);
	int previousParentNodeRef;
	tupBaseNode.GetParentNodeReference(previousParentNodeRef);
	if (parentNodeRef != previousParentNodeRef)
	{
		TupperwareAggregate *pTupPreviousParentNodeAgg = m_tupNodePool.GetNode(previousParentNodeRef);
		TupNode tupPreviousParentNode(pTupParentNodeAgg);
		tupPreviousParentNode.RemoveChildNodeReference(m_baseMeshData.m_nodeRef);
		tupParentNode.AddChildNodeReference(m_baseMeshData.m_nodeRef);
	}
	unsigned long meshCRC = GetMeshCRC(m_baseMeshData);
	int oldKeeperSize = m_addedMeshCRCs.GetSize();
	int keeperIndex = m_addedMeshCRCs.Add(meshCRC);
	// if we need to create a new mesh
	if (keeperIndex == oldKeeperSize)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"  Adding to scene by creating new mesh for %s",(const char *)m_baseMeshData.m_mayaObjectName);

		int newMaterialRef = CreateMaterialFromRefs();
		tupBaseNode.AddMaterialReference(newMaterialRef);

		int newObjectRef = CreateMeshFromBase();

		AddedCombinedMeshes newMesh;
		newMesh.m_CRC = meshCRC;
		newMesh.m_materialRef = newMaterialRef;
		newMesh.m_nodeName = m_baseMeshData.m_nodeName;
		newMesh.m_mayaObjectName = m_baseMeshData.m_mayaObjectName;
		newMesh.m_objectRef = newObjectRef;

		// add in the vertex color information
		newObjectRef = AddVertexColorModifier(newObjectRef);
		unsigned long vertexColorCRC = GetVertexColorCRC(m_baseMeshData);
		newMesh.m_vertexColorCRCs.Add(vertexColorCRC);
		newMesh.m_vertexColorRefs.Add(newObjectRef);

		m_addedCombinedMeshes.Add(newMesh);

		// add in the shape name to the derived object
		TupperwareAggregate *pObjectAgg = m_tupObjectPool.GetObjectAggregate(newObjectRef);
		pObjectAgg->AddScalarString("MayaObjectName",NULL,m_baseMeshData.m_mayaObjectName);

		if (m_baseMeshData.m_faceAttributeVerts.GetSize()>1)
		{
			newObjectRef = AddFaceAttributeModifier(newObjectRef);
			TupperwareAggregate *pObjectAgg = m_tupObjectPool.GetObjectAggregate(newObjectRef);
			pObjectAgg->AddScalarString("MayaObjectName",NULL,m_baseMeshData.m_mayaObjectName);
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"      Adding face attributes");
		}

		tupBaseNode.AddObjectReference(newObjectRef);
	}
	else
	{
		AddedCombinedMeshes &linkMesh = m_addedCombinedMeshes[keeperIndex];
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"    Adding to scene by linking to mesh %s from node %s",(const char *)linkMesh.m_mayaObjectName,(const char *)linkMesh.m_nodeName);
		tupBaseNode.AddMaterialReference(linkMesh.m_materialRef);

		int objectRef = linkMesh.m_objectRef; // start with ref of linked mesh

		unsigned long vertexColorCRC = GetVertexColorCRC(m_baseMeshData);
		int oldVertexColorCRCKeeperSize = linkMesh.m_vertexColorCRCs.GetSize();
		int vertexColorKeeperIndex = linkMesh.m_vertexColorCRCs.Add(vertexColorCRC);
		// if we need to create another set of vertex colors
		if (vertexColorKeeperIndex == oldVertexColorCRCKeeperSize)
		{
			objectRef = AddVertexColorModifier(objectRef);
			linkMesh.m_vertexColorRefs.Add(objectRef);
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"      Creating a new set of vertex colors");
		}
		else
		{
			objectRef = linkMesh.m_vertexColorRefs[vertexColorKeeperIndex];
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"      Linking to an existing set of vertex colors");
		}

		// add in the shape name to the derived object
		TupperwareAggregate *pObjectAgg = m_tupObjectPool.GetObjectAggregate(objectRef);
		pObjectAgg->AddScalarString("MayaObjectName",NULL,m_baseMeshData.m_mayaObjectName);

		if (m_baseMeshData.m_faceAttributeVerts.GetSize()>1)
		{
			objectRef = AddFaceAttributeModifier(objectRef);
			TupperwareAggregate *pObjectAgg = m_tupObjectPool.GetObjectAggregate(objectRef);
			pObjectAgg->AddScalarString("MayaObjectName",NULL,m_baseMeshData.m_mayaObjectName);
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"      Adding face attributes");
		}

		tupBaseNode.AddObjectReference(objectRef);
	}

	// remove sub mesh nodes from scene
	int numSubMeshes = m_subMeshDataArray.GetSize();
	for (int subMeshIndex=0;subMeshIndex<numSubMeshes;subMeshIndex++)
	{
		MeshData &subMeshData = m_subMeshDataArray[subMeshIndex];
		TupperwareAggregate *pTupSubNodeAgg = m_tupNodePool.GetNode(subMeshData.m_nodeRef);
		TupNode tupSubNode(pTupSubNodeAgg);
		int previousParentNodeRef;
		tupSubNode.GetParentNodeReference(previousParentNodeRef);
		TupperwareAggregate *pTupPreviousParentNodeAgg = m_tupNodePool.GetNode(previousParentNodeRef);
		TupNode tupPreviousParentNode(pTupParentNodeAgg);
		tupPreviousParentNode.RemoveChildNodeReference(subMeshData.m_nodeRef);
		m_tupNodePool.DeleteNode(subMeshData.m_nodeRef);
	}

}

void MergeMeshesManager::GetMeshData(int nodeRef,MergeMeshesManager::MeshData &meshData)
{
	int numZeroAreaFaces = 0;
	meshData.m_nodeRef = nodeRef;
	TupperwareAggregate *pTupNodeAgg = m_tupNodePool.GetNode(nodeRef);
	TupNode tupNode(pTupNodeAgg);
	char *pName;
	tupNode.GetName(&pName);
	meshData.m_nodeName = pName;

	TupperwareAggregate *pTransformAgg = tupNode.GetTransform( );
	if( pTransformAgg )
	{
		TupTransform tupTransform( pTransformAgg );
		if( tupTransform.GetTransformType( ) == TupTransform::STATIC_MATRIX )
		{
			TupStaticMatrix tupStaticMatrix( pTransformAgg );
			float *pMatrix;
			tupStaticMatrix.GetMatrix(&pMatrix);
			memcpy(&meshData.m_localToWorldMatrix,pMatrix,16*sizeof(float));
		}
	}

	char *pNodeAttributesString;
	tupNode.GetUserProperties( &pNodeAttributesString );
	if (pNodeAttributesString)
	{
		meshData.m_objectAttributes = pNodeAttributesString;
	}

	int objectRef;
	tupNode.GetObjectReference(objectRef);
	TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(m_tupObjectPool,objectRef);
	TupObject tupObject(pBaseMeshAgg);
	assert(tupObject.GetObjectType() == TupObject::TRI_MESH); // this better be a trimesh
	TupTriMesh tupTriMesh(pBaseMeshAgg);

	meshData.m_mayaObjectName.Empty();
	TupperwareScalar *pMayaObjectNameScalar = pBaseMeshAgg->FindScalarByKey("MayaObjectName");
	if (pMayaObjectNameScalar)
	{
		meshData.m_mayaObjectName = pMayaObjectNameScalar->GetAsString();
	}

	int tempValue;
	int numPositionVerts, numNormalVerts;
	TupVertex3f *pPositionVerts, *pNormalVerts;
	int *pPositionIndices, *pNormalIndices;
	int *pFaceMaterialIndices,*pFaceFlags;
	int numFaces;
	int numFaceNormalVerts;
	TupVertex3f *pFaceNormalVerts;
	int *pFaceNormalIndices;
	int *pFaceShadingGroups;

	tupTriMesh.GetFaceFlags( &pFaceFlags, numFaces );
	tupTriMesh.GetPositionVerts( (float **)&pPositionVerts, numPositionVerts );
	tupTriMesh.GetPositionIndices( &pPositionIndices, tempValue );
	tupTriMesh.GetNormalVerts( (float **)&pNormalVerts, numNormalVerts );
	tupTriMesh.GetNormalIndices( &pNormalIndices, tempValue );
	tupTriMesh.GetFaceNormalVerts( (float **)&pFaceNormalVerts, numFaceNormalVerts );
	tupTriMesh.GetFaceNormalIndices( &pFaceNormalIndices, tempValue );
	tupTriMesh.GetFaceMaterialIndices(&pFaceMaterialIndices,tempValue);
	tupTriMesh.GetFaceNormalIndices(&pFaceNormalIndices,tempValue);
	tupTriMesh.GetFaceShadingGroups(&pFaceShadingGroups,tempValue);

	int numFaces3 = numFaces*3;
	int faceIndex,faceIndex3;
	meshData.m_faceFlags.SetSize(numFaces);
	meshData.m_positionIndices.SetSize(numFaces3);
	meshData.m_positionVerts.SetSize(numPositionVerts);
	meshData.m_normalIndices.SetSize(numFaces3);
	meshData.m_faceNormalIndices.SetSize(numFaces);
	meshData.m_faceMaterialRefIndices.SetSize(numFaces);
	meshData.m_faceShadingGroups.SetSize(numFaces);
	meshData.m_nextStageFaceArray.SetSize(numFaces);
	meshData.m_prevStageFaceArray.SetSize(numFaces);
	// reset keepers
	meshData.m_normalVerts.RemoveAll();
	meshData.m_faceNormalVerts.RemoveAll();
	meshData.m_faceAttributeVerts.RemoveAll();
	TupString emptyString;
	meshData.m_faceAttributeVerts.Add(emptyString);
	meshData.m_faceMaterialRefVerts.RemoveAll();
	meshData.m_vertexColorVerts.RemoveAll();
	meshData.m_vertexAlphaVerts.RemoveAll();
	meshData.m_meshMaps.RemoveAll();
	meshData.m_mapChannelToMeshMapIndexKeeper.RemoveAll();

	TupArray<int> materialRefs;
	GetMaterialRefs(tupNode,materialRefs);

	TupArray<TupVertex3f> vertexColorVerts;
	TupArray<int> vertexColorIndices;
	TupArray<float> vertexAlphaVerts;
	TupArray<int> vertexAlphaIndices;
	bool bMixedColors; // ignored
	TupObjectHelper::FindVertexColors(m_tupObjectPool,objectRef,m_tupModifierPool,vertexColorVerts,vertexColorIndices,vertexAlphaVerts,vertexAlphaIndices,bMixedColors);
	// if no vertex colors
	if (!vertexColorIndices.GetSize())
	{
		vertexColorIndices.SetSize(numFaces3);
		TupVertex3f dummy(1.0f,1.0f,1.0f);
		vertexColorVerts.Add(dummy);
		for (faceIndex3=0;faceIndex3<numFaces3;faceIndex3++)
		{
			vertexColorIndices[faceIndex3] = 0;
		}
	}
	// if no vertex alpha
	if (!vertexAlphaIndices.GetSize())
	{
		vertexAlphaIndices.SetSize(numFaces3);
		float dummy(1.0f);
		vertexAlphaVerts.Add(dummy);
		for (faceIndex3=0;faceIndex3<numFaces3;faceIndex3++)
		{
			vertexAlphaIndices[faceIndex3] = 0;
		}
	}

	meshData.m_vertexColorIndices.SetSize(numFaces3);
	meshData.m_vertexAlphaIndices.SetSize(numFaces3);

	TupArray<int> faceAttributesIndices;
	TupArray<TupString> faceAttributesVerts;
	TupObjectHelper::FindFaceAttributes(m_tupObjectPool,objectRef,m_tupModifierPool,faceAttributesVerts,faceAttributesIndices);
	// if no faceAttributes
	if (!faceAttributesIndices.GetSize())
	{
		faceAttributesIndices.SetSize(numFaces);
		TupString dummy;
		faceAttributesVerts.Add(dummy);
		for (faceIndex=0;faceIndex<numFaces;faceIndex++)
		{
			faceAttributesIndices[faceIndex] = 0;
		}
	}
	meshData.m_faceAttributeIndices.SetSize(numFaces);

	// fill in the arrays from the tup data
	for (faceIndex=faceIndex3=0;faceIndex<numFaces;faceIndex++,faceIndex3+=3)
	{
		meshData.m_faceFlags[faceIndex] = pFaceFlags[faceIndex];
		meshData.m_faceNormalIndices[faceIndex] = meshData.m_faceNormalVerts.Add(pFaceNormalVerts[pFaceNormalIndices[faceIndex]]);
		meshData.m_faceShadingGroups[faceIndex] = pFaceShadingGroups[faceIndex];
		meshData.m_faceAttributeIndices[faceIndex] = meshData.m_faceAttributeVerts.Add(faceAttributesVerts[faceAttributesIndices[faceIndex]]);
		int faceMaterialIndex = pFaceMaterialIndices[faceIndex];
		if (faceMaterialIndex<0 || faceMaterialIndex>=materialRefs.GetSize())
		{
			faceMaterialIndex = 0;
		}
		if (materialRefs.GetSize())
		{
			meshData.m_faceMaterialRefIndices[faceIndex] = meshData.m_faceMaterialRefVerts.Add(materialRefs[faceMaterialIndex]);
		}
		else
		{
			meshData.m_faceMaterialRefIndices[faceIndex] = meshData.m_faceMaterialRefVerts.Add(-1);
		}
		bool bZeroAreaFace = false;
		for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
		{
			meshData.m_vertexColorIndices[faceIndex3+facePointIndex] = meshData.m_vertexColorVerts.Add(vertexColorVerts[vertexColorIndices[faceIndex3+facePointIndex]]);
			meshData.m_vertexAlphaIndices[faceIndex3+facePointIndex] = meshData.m_vertexAlphaVerts.Add(vertexAlphaVerts[vertexAlphaIndices[faceIndex3+facePointIndex]]);

			meshData.m_positionIndices[faceIndex3+facePointIndex] = pPositionIndices[faceIndex3+facePointIndex];
			meshData.m_normalIndices[faceIndex3+facePointIndex] = meshData.m_normalVerts.Add(pNormalVerts[pNormalIndices[faceIndex3+facePointIndex]]);
			int nextPointIndex = facePointIndex<2 ? facePointIndex+1 : 0;
			if (pPositionIndices[faceIndex3+facePointIndex]==pPositionIndices[faceIndex3+nextPointIndex])
				bZeroAreaFace = true;
		}
		if (bZeroAreaFace)
		{
			numZeroAreaFaces++;
		}

		meshData.m_nextStageFaceArray[faceIndex] = -1; // No link for now
		meshData.m_prevStageFaceArray[faceIndex] = -1;
	}
	// copy the position verts (others use keepers)
	int vertIndex;
	for (vertIndex=0;vertIndex<numPositionVerts;vertIndex++)
	{
		meshData.m_positionVerts[vertIndex] = pPositionVerts[vertIndex];
	}
	// do the uvs
	int meshMapIndex;
	int numMeshMaps = tupTriMesh.GetNumMeshMaps();
	for (meshMapIndex = 0; meshMapIndex < numMeshMaps; meshMapIndex++ )
	{
		TupTriMeshMap  meshMap( tupTriMesh.GetMeshMap( meshMapIndex ) );
		int mapChannel = meshMap.GetMapChannel();
		if (mapChannel>=TupTriMeshMap::MAP_CHANNEL_TEXTURE_UV_START)
		{
			meshData.m_mapChannelToMeshMapIndexKeeper.Add(mapChannel);
		}
	}

	// make actual mesh maps the same size as the number in the keeper
	meshData.m_meshMaps.SetSize(meshData.m_mapChannelToMeshMapIndexKeeper.GetSize());

	// fill in the meshmaps
	for (meshMapIndex = 0; meshMapIndex < numMeshMaps; meshMapIndex++ )
	{
		TupTriMeshMap  meshMap( tupTriMesh.GetMeshMap( meshMapIndex ) );
		int mapChannel = meshMap.GetMapChannel();
		if (mapChannel>=TupTriMeshMap::MAP_CHANNEL_TEXTURE_UV_START)
		{
			// get actual index for this channel
			int meshMapIndex = meshData.m_mapChannelToMeshMapIndexKeeper.GetIndex(mapChannel);
			meshData.m_meshMaps[meshMapIndex].m_mapChannel = mapChannel;
			meshData.m_meshMaps[meshMapIndex].m_indices.SetSize(numFaces3);
			meshData.m_meshMaps[meshMapIndex].m_verts.RemoveAll();
			// make first vert a dummy...
			TupVertex3f dummyUV(0.0f,0.0f,0.0f);
			meshData.m_meshMaps[meshMapIndex].m_verts.Add(dummyUV);
			TupVertex3f *pUVVerts;
			int numUVVerts;
			int *pUVIndices;
			meshMap.GetMapVerts((float **) &pUVVerts, numUVVerts);
			meshMap.GetMapIndices( &pUVIndices, tempValue);
			for (faceIndex3=0;faceIndex3<numFaces3;faceIndex3++)
			{
				meshData.m_meshMaps[meshMapIndex].m_indices[faceIndex3] = meshData.m_meshMaps[meshMapIndex].m_verts.Add(pUVVerts[pUVIndices[faceIndex3]]);
			}
		}
	}
	if (numZeroAreaFaces)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE,"    Warning Mesh contains %d zero area faces",numZeroAreaFaces);
	}
}

void MergeMeshesManager::GetMaterialRefs(TupNode &tupNode,TupArray<int> &materialRefs)
{
	materialRefs.RemoveAll();

	int materialRef;
	TupperwareScalar *pMaterialRefScalar = tupNode.GetMaterialReference(materialRef);
	if (pMaterialRefScalar)
	{
		// get the material refs used for the mesh
		if (materialRef!=-1)
		{
			TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial(materialRef);
			TupMaterial tupMaterial(pMaterialAgg);
			if (tupMaterial.GetMaterialType()==TupMaterial::MULTI_SUB_OBJECT_MATERIAL)
			{
				TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMaterialAgg);
				int numSubMaterials = tupMultiSubObjectMaterial.GetNumSubMaterials();
				materialRefs.SetSize(numSubMaterials);
				for (int subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
				{
					TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(subMaterialIndex);
					TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
					int subMaterialRef;
					tupSubMaterial.GetMaterialReference(subMaterialRef);
					materialRefs[subMaterialIndex] = subMaterialRef;
				}
			}
			else
			{
				materialRefs.Add(materialRef); // add main ref (only one material for this mesh
			}
		}
		else
		{
			materialRefs.Add(materialRef); // add -1
		}
	}
}

void MergeMeshesManager::MergeMeshToBase(MeshData &baseMeshData,MeshData &subMeshData)
{
	int numBaseFaces = baseMeshData.m_faceFlags.GetSize();
	int numBaseFaces3 = numBaseFaces*3;
	int numSubFaces = subMeshData.m_faceFlags.GetSize();
	int numSubFaces3 = numSubFaces*3;
	int faceIndex;
	int faceIndex3;
	int facePointIndex;

	baseMeshData.m_faceFlags.Append(subMeshData.m_faceFlags);
	baseMeshData.m_faceShadingGroups.Append(subMeshData.m_faceShadingGroups);
	baseMeshData.m_positionIndices.SetSize(numBaseFaces3+numSubFaces3);
	baseMeshData.m_normalIndices.SetSize(numBaseFaces3+numSubFaces3);
	baseMeshData.m_faceNormalIndices.SetSize(numBaseFaces+numSubFaces);
	baseMeshData.m_faceMaterialRefIndices.SetSize(numBaseFaces+numSubFaces);
	baseMeshData.m_vertexColorIndices.SetSize(numBaseFaces3+numSubFaces3);
	baseMeshData.m_vertexAlphaIndices.SetSize(numBaseFaces3+numSubFaces3);
	baseMeshData.m_faceAttributeIndices.SetSize(numBaseFaces+numSubFaces);
	baseMeshData.m_nextStageFaceArray.SetSize(numBaseFaces+numSubFaces);
	baseMeshData.m_prevStageFaceArray.SetSize(numBaseFaces+numSubFaces);

	// any meshmap channels that are in the sub mesh but not in the base
	// get added set up for the base mesh (point to dummy)
	int meshMapIndex;
	int numSubMeshMaps = subMeshData.m_meshMaps.GetSize();
	for (meshMapIndex=0;meshMapIndex<numSubMeshMaps;meshMapIndex++)
	{
		int mapChannel = subMeshData.m_meshMaps[meshMapIndex].m_mapChannel;
		// see if this channel is currently used in the base mesh
		int oldBaseSize = baseMeshData.m_mapChannelToMeshMapIndexKeeper.GetSize();
		int baseMeshMapIndex = baseMeshData.m_mapChannelToMeshMapIndexKeeper.Add(mapChannel);
		// if this channel was not there
		if (baseMeshMapIndex==oldBaseSize)
		{
			baseMeshData.m_meshMaps.SetSize(oldBaseSize+1);
			TupVertex3f dummyUV(0.0f,0.0f,0.0f);
			baseMeshData.m_meshMaps[baseMeshMapIndex].m_verts.Add(dummyUV);
			baseMeshData.m_meshMaps[baseMeshMapIndex].m_indices.SetSize(numBaseFaces3);
			for (faceIndex3=0;faceIndex3<numBaseFaces3;faceIndex3++)
			{
				baseMeshData.m_meshMaps[baseMeshMapIndex].m_indices[faceIndex3] = 0;
			}
		}
	}
	// adjust the meshmaps to hold the submesh
	int numBaseMeshMaps = baseMeshData.m_meshMaps.GetSize();
	for (meshMapIndex=0;meshMapIndex<numBaseMeshMaps;meshMapIndex++)
	{
		// set aside space for the indices
		baseMeshData.m_meshMaps[meshMapIndex].m_indices.SetSize(numBaseFaces3+numSubFaces3);
	}

	// keep track of which faces are using which position verts
	TupArray<TupArray<int> > basePositionFaces;
	TupArray<TupArray<int> > basePositionFacePointIndices;
	basePositionFaces.SetSize(baseMeshData.m_positionVerts.GetSize());
	basePositionFacePointIndices.SetSize(baseMeshData.m_positionVerts.GetSize());
	for (faceIndex=0,faceIndex3=0;faceIndex<numBaseFaces;faceIndex++,faceIndex3+=3)
	{
		// only do faces that are not linked to another layer
		if (baseMeshData.m_nextStageFaceArray[faceIndex]==-1)
		{
			for (facePointIndex=0;facePointIndex<3;facePointIndex++)
			{
				int positionIndex = baseMeshData.m_positionIndices[faceIndex3+facePointIndex];
				basePositionFaces[positionIndex].Add(faceIndex);
				basePositionFacePointIndices[positionIndex].Add(facePointIndex);
			}
		}
	}

	int numSubMeshPositionVerts = subMeshData.m_positionVerts.GetSize();
	// this array keeps track of which position verts were added to the base mesh
	TupArray<int> addedPosVertRemapTable;
	addedPosVertRemapTable.SetSize(numSubMeshPositionVerts);

	// find the base position verts that are close to the submesh position verts
	TupArray<TupArray<int> > closeBaseVertexTable;
	closeBaseVertexTable.SetSize(numSubMeshPositionVerts);
	int numAddedPosVertsStage1 = 0;
	int numLinkedFaces = 0;
	int subVertIndex;
	float lowerNoFittingDistanceSq=1.0e10f;
	float higherNoFittingDistanceSq=0.0f;
	int numOriginalBaseMeshPositionVerts = baseMeshData.m_positionVerts.GetSize();
	for (subVertIndex=0;subVertIndex<numSubMeshPositionVerts;subVertIndex++)
	{
		TupVertex3f &subVertex = subMeshData.m_positionVerts[subVertIndex];
		float smallestNoFittingDistanceSq=1.0e10f;
		bool bFoundMatch = false;
		for (int baseVertIndex=0;baseVertIndex<numOriginalBaseMeshPositionVerts;baseVertIndex++)
		{
			TupVertex3f &baseVertex = baseMeshData.m_positionVerts[baseVertIndex];
			float distanceSq = TupVertex3f::DistanceSq(subVertex,baseVertex);
			// keep track of ones that are within the threshold
			if (distanceSq<m_positionToleranceSq)
			{
				bFoundMatch = true;
				closeBaseVertexTable[subVertIndex].Add(baseVertIndex);
			}
			// we also keep track of the closest distance that didn't fit
			else if (distanceSq<smallestNoFittingDistanceSq)
			{
				smallestNoFittingDistanceSq = distanceSq;
			}
		}
		// if there are some verts close set that we have not added a vert
		if (bFoundMatch)
		{
			addedPosVertRemapTable[subVertIndex] = -1;
		}
		// if no verts close then add vert and set up remap
		else
		{
			baseMeshData.m_positionVerts.Add(subVertex);
			addedPosVertRemapTable[subVertIndex] = baseMeshData.m_positionVerts.GetSize()-1;
			numAddedPosVertsStage1++;
			// keep track of distances that didn't fit (for the whole mesh)
			if (smallestNoFittingDistanceSq<lowerNoFittingDistanceSq)
			{
				lowerNoFittingDistanceSq = smallestNoFittingDistanceSq;
			}
			if (smallestNoFittingDistanceSq>higherNoFittingDistanceSq)
			{
				higherNoFittingDistanceSq = smallestNoFittingDistanceSq;
			}
		}
	}
	// set up the merged faces
	int numAddedPosVertsStage2 = 0;
	for (faceIndex=faceIndex3=0;faceIndex<numSubFaces;faceIndex++,faceIndex3+=3)
	{
		baseMeshData.m_faceNormalIndices[numBaseFaces+faceIndex] = baseMeshData.m_faceNormalVerts.Add(subMeshData.m_faceNormalVerts[subMeshData.m_faceNormalIndices[faceIndex]]);
		baseMeshData.m_faceMaterialRefIndices[numBaseFaces+faceIndex] = baseMeshData.m_faceMaterialRefVerts.Add(subMeshData.m_faceMaterialRefVerts[subMeshData.m_faceMaterialRefIndices[faceIndex]]);
		baseMeshData.m_faceAttributeIndices[numBaseFaces+faceIndex] = baseMeshData.m_faceAttributeVerts.Add(subMeshData.m_faceAttributeVerts[subMeshData.m_faceAttributeIndices[faceIndex]]);

		// set the next stage array to the new face index if valid
		if (subMeshData.m_nextStageFaceArray[faceIndex]!=-1)
			baseMeshData.m_nextStageFaceArray[numBaseFaces+faceIndex] = subMeshData.m_nextStageFaceArray[faceIndex]+numBaseFaces;
		else
			baseMeshData.m_nextStageFaceArray[numBaseFaces+faceIndex] = -1;
		// set the prev stage array to the new face index if valid
		if (subMeshData.m_prevStageFaceArray[faceIndex]!=-1)
			baseMeshData.m_prevStageFaceArray[numBaseFaces+faceIndex] = subMeshData.m_prevStageFaceArray[faceIndex]+numBaseFaces;
		else
			baseMeshData.m_prevStageFaceArray[numBaseFaces+faceIndex] = -1;

		// next step is to try to find a face to link to from the base mesh
		// this sets up the next/prev stage as well as the position information
		int numMatchesFound = 0;
		int bestFace;
		float bestFaceDistance;
		int bestFaceRotation;

		// we need to rotate as well to find links
		for (int faceRotation=0;faceRotation<3;faceRotation++)
		{
			int facePointIndex1 = faceRotation;
			int facePointIndex2 = faceRotation<2 ? faceRotation+1 : 0;
			int facePointIndex3 = faceRotation>0 ? faceRotation-1 : 2;
			TupArray<int> &closeBaseVertices1 = closeBaseVertexTable[subMeshData.m_positionIndices[faceIndex3]];
			TupArray<int> &closeBaseVertices2 = closeBaseVertexTable[subMeshData.m_positionIndices[faceIndex3+1]];
			TupArray<int> &closeBaseVertices3 = closeBaseVertexTable[subMeshData.m_positionIndices[faceIndex3+2]];
			for (int close1Index=0;close1Index<closeBaseVertices1.GetSize();close1Index++)
			{
				TupArray<int> &baseFaces1 = basePositionFaces[closeBaseVertices1[close1Index]];
				TupArray<int> &baseFacePoints1 = basePositionFacePointIndices[closeBaseVertices1[close1Index]];
				for (int baseFaces1Index=0;baseFaces1Index<baseFaces1.GetSize();baseFaces1Index++)
				{
					int baseFace1 = baseFaces1[baseFaces1Index];
					int baseFace1t3 = baseFace1+(baseFace1<<1);
					int baseFacePoint1 = baseFacePoints1[baseFaces1Index];
					if (baseFacePoint1!=facePointIndex1)
						continue;
					for (int close2Index=0;close2Index<closeBaseVertices2.GetSize();close2Index++)
					{
						TupArray<int> &baseFaces2 = basePositionFaces[closeBaseVertices2[close2Index]];
						TupArray<int> &baseFacePoints2 = basePositionFacePointIndices[closeBaseVertices2[close2Index]];
						for (int baseFaces2Index=0;baseFaces2Index<baseFaces2.GetSize();baseFaces2Index++)
						{
							int baseFace2 = baseFaces2[baseFaces2Index];
							int baseFacePoint2 = baseFacePoints2[baseFaces2Index];
							if (baseFacePoint2!=facePointIndex2)
								continue;
							// if face exists for both verts
							if (baseFace2==baseFace1)
							{
								for (int close3Index=0;close3Index<closeBaseVertices3.GetSize();close3Index++)
								{
									TupArray<int> &baseFaces3 = basePositionFaces[closeBaseVertices3[close3Index]];
									TupArray<int> &baseFacePoints3 = basePositionFacePointIndices[closeBaseVertices3[close3Index]];
									for (int baseFaces3Index=0;baseFaces3Index<baseFaces3.GetSize();baseFaces3Index++)
									{
										int baseFace3 = baseFaces3[baseFaces3Index];
										int baseFacePoint3 = baseFacePoints3[baseFaces3Index];
										if (baseFacePoint3!=facePointIndex3)
											continue;
										// if face matches for this one too
										// we have found a face that is using verts that are close
										// check how far these verts are from my face and pick the best one
										if (baseFace3==baseFace1)
										{
											float faceDistance;
											faceDistance = TupVertex3f::Distance(subMeshData.m_positionVerts[subMeshData.m_positionIndices[faceIndex3]],
																							baseMeshData.m_positionVerts[baseMeshData.m_positionIndices[baseFace1t3+facePointIndex1]]);
											faceDistance += TupVertex3f::Distance(subMeshData.m_positionVerts[subMeshData.m_positionIndices[faceIndex3+1]],
																							baseMeshData.m_positionVerts[baseMeshData.m_positionIndices[baseFace1t3+facePointIndex2]]);
											faceDistance += TupVertex3f::Distance(subMeshData.m_positionVerts[subMeshData.m_positionIndices[faceIndex3+2]],
																							baseMeshData.m_positionVerts[baseMeshData.m_positionIndices[baseFace1t3+facePointIndex3]]);
											if (!numMatchesFound || faceDistance<bestFaceDistance)
											{
												numMatchesFound++;
												bestFace = baseFace1;
												bestFaceDistance = faceDistance;
												bestFaceRotation = faceRotation;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		// if we found a face to link to 
		if (numMatchesFound)
		{
			numLinkedFaces++;
			int bestFace3 = bestFace+(bestFace<<1);
			for (facePointIndex=0;facePointIndex<3;facePointIndex++)
			{
				baseMeshData.m_positionIndices[numBaseFaces3+faceIndex3+facePointIndex] = baseMeshData.m_positionIndices[bestFace3+facePointIndex];
			}
			// link from the base mesh to my face
			if (baseMeshData.m_prevStageFaceArray[numBaseFaces+faceIndex]==-1)
			{
				baseMeshData.m_prevStageFaceArray[numBaseFaces+faceIndex] = bestFace;
				baseMeshData.m_nextStageFaceArray[bestFace] = numBaseFaces+faceIndex;
				assert(bestFace<(numBaseFaces+numSubFaces));
				assert(faceIndex<numSubFaces);
			}
		}
		// if no face to link to
		else
		{
			bestFaceRotation = 0; // set rotation for later
			for (facePointIndex=0;facePointIndex<3;facePointIndex++)
			{
				int subVertIndex = subMeshData.m_positionIndices[faceIndex3+facePointIndex];
				int addedPosVertIndex = addedPosVertRemapTable[subVertIndex];
				// if one not added yet
				if (addedPosVertIndex==-1)
				{
					baseMeshData.m_positionVerts.Add(subMeshData.m_positionVerts[subVertIndex]);
					addedPosVertIndex = addedPosVertRemapTable[subVertIndex] = baseMeshData.m_positionVerts.GetSize()-1;
					numAddedPosVertsStage2++;
				}
				baseMeshData.m_positionIndices[numBaseFaces3+faceIndex3+facePointIndex] = addedPosVertIndex;
			}
		}

		// add these based on the face rotation
		for (facePointIndex=0;facePointIndex<3;facePointIndex++)
		{
			int rotatedFacePointIndex = facePointIndex+bestFaceRotation;
			// wrap back to 0-1
			if (rotatedFacePointIndex>2) 
			{
				rotatedFacePointIndex-=3;
			}
			baseMeshData.m_normalIndices[numBaseFaces3+faceIndex3+rotatedFacePointIndex] = baseMeshData.m_normalVerts.Add(subMeshData.m_normalVerts[subMeshData.m_normalIndices[faceIndex3+facePointIndex]]);
			baseMeshData.m_vertexColorIndices[numBaseFaces3+faceIndex3+rotatedFacePointIndex] = baseMeshData.m_vertexColorVerts.Add(subMeshData.m_vertexColorVerts[subMeshData.m_vertexColorIndices[faceIndex3+facePointIndex]]);
			baseMeshData.m_vertexAlphaIndices[numBaseFaces3+faceIndex3+rotatedFacePointIndex] = baseMeshData.m_vertexAlphaVerts.Add(subMeshData.m_vertexAlphaVerts[subMeshData.m_vertexAlphaIndices[faceIndex3+facePointIndex]]);

			// add the uvs to the base mesh
			for (meshMapIndex=0;meshMapIndex<numBaseMeshMaps;meshMapIndex++)
			{
				int subMeshMapIndex = subMeshData.m_mapChannelToMeshMapIndexKeeper.GetIndex(baseMeshData.m_meshMaps[meshMapIndex].m_mapChannel);
				// sub mesh has a matching mesh map for this channel
				if (subMeshMapIndex!=-1)
				{
					// set the indices while adding to the verts
					baseMeshData.m_meshMaps[meshMapIndex].m_indices[numBaseFaces3+faceIndex3+rotatedFacePointIndex] = 
						baseMeshData.m_meshMaps[meshMapIndex].m_verts.Add
						(subMeshData.m_meshMaps[subMeshMapIndex].m_verts[subMeshData.m_meshMaps[subMeshMapIndex].m_indices[faceIndex3+facePointIndex]]);
				}
			}
		}
	}

	if (numAddedPosVertsStage1==0 && numAddedPosVertsStage2==0)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"    All %d position verts merged into base mesh",numSubMeshPositionVerts);
	}
	else 
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"    %d of %d position verts merged into base mesh",numSubMeshPositionVerts-(numAddedPosVertsStage1+numAddedPosVertsStage2),numSubMeshPositionVerts);
		if (numAddedPosVertsStage1)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"    lower no fitting distance = %f",TupVertex3Sqrt(lowerNoFittingDistanceSq));
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"    higher no fitting distance = %f",TupVertex3Sqrt(higherNoFittingDistanceSq));
		}
	}

	if (numLinkedFaces==numSubFaces)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"    All %d faces linked with base mesh",numLinkedFaces);
	}
	else
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"    %d of %d faces linked with base mesh",numLinkedFaces,numSubFaces);
	}
}

int MergeMeshesManager::CreateMeshFromBase(void)
{
	int objectRef = m_tupObjectPool.AddObjectAggregate();
	TupperwareAggregate *pMeshAgg = m_tupObjectPool.GetObjectAggregate(objectRef);
	TupString mayaObjectName = m_baseMeshData.m_mayaObjectName;
	mayaObjectName += " Merged";
	pMeshAgg->AddScalarString("MayaObjectName",NULL,mayaObjectName);
	TupObject tupObject(pMeshAgg);
	tupObject.SetObjectType(TupObject::TRI_MESH);
	TupTriMesh tupTriMesh(pMeshAgg);

	int numFaces = m_baseMeshData.m_faceFlags.GetSize();

	SetFaceFlagsForStages();

	tupTriMesh.AddPositionVerts((const float *)m_baseMeshData.m_positionVerts.GetData(),m_baseMeshData.m_positionVerts.GetSize());
	tupTriMesh.AddPositionIndices(m_baseMeshData.m_positionIndices.GetData(),numFaces);

	tupTriMesh.AddNormalVerts((const float *)m_baseMeshData.m_normalVerts.GetData(),m_baseMeshData.m_normalVerts.GetSize());
	tupTriMesh.AddNormalIndices(m_baseMeshData.m_normalIndices.GetData(),numFaces);

	tupTriMesh.AddFaceNormalVerts((const float *)m_baseMeshData.m_faceNormalVerts.GetData(),m_baseMeshData.m_faceNormalVerts.GetSize());
	tupTriMesh.AddFaceNormalIndices(m_baseMeshData.m_faceNormalIndices.GetData(),numFaces);

	tupTriMesh.AddFaceFlags(m_baseMeshData.m_faceFlags.GetData(),numFaces);

	tupTriMesh.AddFaceShadingGroups(m_baseMeshData.m_faceShadingGroups.GetData(),numFaces);
	tupTriMesh.AddFaceMaterialIndices(m_baseMeshData.m_faceMaterialRefIndices.GetData(),numFaces);

	int numMeshMaps = m_baseMeshData.m_meshMaps.GetSize();

	pMeshAgg->AddListInt("CombineMeshes_NextStageFaceIndices",NULL,m_baseMeshData.m_nextStageFaceArray);

	// add in the uv sets
	for (int meshMapIndex = 0; meshMapIndex < numMeshMaps; meshMapIndex++)
	{
		// make sure there are some vertices to do
		if (m_baseMeshData.m_meshMaps[meshMapIndex].m_verts.GetSize()>1)
		{
			TupperwareAggregate *pUVMeshMapAgg = tupTriMesh.AddMeshMap(m_baseMeshData.m_meshMaps[meshMapIndex].m_mapChannel);
			TupTriMeshMap tupTriMeshMap(pUVMeshMapAgg);
			tupTriMeshMap.AddMapFlags(TupTriMeshMap::MESHMAP_FLAG_TEXTURE);
			tupTriMeshMap.AddMapIndices(m_baseMeshData.m_meshMaps[meshMapIndex].m_indices.GetData(),numFaces);
			tupTriMeshMap.AddMapVerts((const float *)m_baseMeshData.m_meshMaps[meshMapIndex].m_verts.GetData(),m_baseMeshData.m_meshMaps[meshMapIndex].m_verts.GetSize());
		}
	}
	return objectRef;
}

int MergeMeshesManager::AddVertexColorModifier(int objectRef)
{
	int derivedObjectRef = m_tupObjectPool.AddObjectAggregate();
	TupperwareAggregate *pDerivedObjectAgg = m_tupObjectPool.GetObjectAggregate(derivedObjectRef);
	TupObject tupObject(pDerivedObjectAgg);
	tupObject.SetObjectType(TupObject::DERIVED);

	TupDerived tupDerived(pDerivedObjectAgg);
	tupDerived.AddObjectReference(objectRef);

	TupperwareAggregate *pDerivedModifierAgg = tupDerived.AddModifier(0);
	TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
	tupDerivedModifier.AddModifierContext();

	int modifierReference = m_tupModifierPool.AddModifier();
	TupperwareAggregate *pModifierAgg = m_tupModifierPool.GetModifier(modifierReference);
	TupModifier tupModifier(pModifierAgg);
	tupModifier.SetModifierType(TupModifier::APPLY_VC_MODIFIER);

	TupApplyVCModifier tupApplyVCModifier(pModifierAgg);
	tupApplyVCModifier.AddName("ApplyVC");

	tupApplyVCModifier.AddColorVerts((const float *)m_baseMeshData.m_vertexColorVerts.GetData(),m_baseMeshData.m_vertexColorVerts.GetSize());
	tupApplyVCModifier.AddColorIndices(m_baseMeshData.m_vertexColorIndices.GetData(),m_baseMeshData.m_vertexColorIndices.GetSize());

	int flags = TupApplyVCModifier::FLAGS_ENABLED;

	// if there is alpha to do
	if (m_baseMeshData.m_vertexAlphaVerts.GetSize()>1)
	{
		flags |= TupApplyVCModifier::FLAGS_HAS_ALPHA;
		tupApplyVCModifier.AddAlphaVerts((const float *)m_baseMeshData.m_vertexAlphaVerts.GetData(),m_baseMeshData.m_vertexAlphaVerts.GetSize());
		tupApplyVCModifier.AddAlphaIndices(m_baseMeshData.m_vertexAlphaIndices.GetData(),m_baseMeshData.m_vertexAlphaIndices.GetSize());
	}
	tupApplyVCModifier.AddFlags(flags);

	tupDerivedModifier.AddModifierReference(modifierReference);
	return derivedObjectRef;
}

int MergeMeshesManager::AddFaceAttributeModifier(int objectRef)
{
	int derivedObjectRef = m_tupObjectPool.AddObjectAggregate();
	TupperwareAggregate *pDerivedObjectAgg = m_tupObjectPool.GetObjectAggregate(derivedObjectRef);
	TupObject tupObject(pDerivedObjectAgg);
	tupObject.SetObjectType(TupObject::DERIVED);

	TupDerived tupDerived(pDerivedObjectAgg);
	tupDerived.AddObjectReference(objectRef);

	TupperwareAggregate *pDerivedModifierAgg = tupDerived.AddModifier(0);
	TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);

	// modifier
	int modifierIndex = m_tupModifierPool.AddModifier();
	tupDerivedModifier.AddModifierReference(modifierIndex);
	TupperwareAggregate *pModifierAgg = m_tupModifierPool.GetModifier(modifierIndex);
	TupModifier tupModifier(pModifierAgg);
	tupModifier.SetModifierType(TupModifier::FACE_ATTRIBUTE_MODIFIER);

	TupFaceAttributesModifier tupFaceAttributesModifier(pModifierAgg);
	tupFaceAttributesModifier.AddName("FaceAttributes");
	int flags = TupSkinModifier::FLAGS_ENABLED;
	tupFaceAttributesModifier.AddFlags(flags);

	// modifier context
	TupperwareAggregate *pModifierContextAgg = tupDerivedModifier.AddModifierContext();
	TupFaceAttributesModifierContext tupFaceAttributesModifierContext(pModifierContextAgg);
	tupFaceAttributesModifierContext.AddFaceAttributeIndices(m_baseMeshData.m_faceAttributeIndices.GetData(),m_baseMeshData.m_faceAttributeIndices.GetSize());
	tupFaceAttributesModifierContext.AddFaceAttributeVerts(m_baseMeshData.m_faceAttributeVerts.GetArray());

	return derivedObjectRef;
}

int MergeMeshesManager::CreateMaterialFromRefs(void)
{
	// build an array for remapping the indicies
	int numSubMaterials = m_baseMeshData.m_faceMaterialRefVerts.GetSize();

	if (numSubMaterials==0)
	{
		return -1;
	}
	else if (numSubMaterials==1)
	{
		return m_baseMeshData.m_faceMaterialRefVerts[0]; // use this material (since there is only one)
	}
	else
	{
		int materialRef = m_tupMaterialPool.AddMaterial();
		TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial(materialRef);
		TupMaterial tupMaterial(pMaterialAgg);
		tupMaterial.SetMaterialType(TupMaterial::MULTI_SUB_OBJECT_MATERIAL);

		TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMaterialAgg);
		tupMultiSubObjectMaterial.AddName("MultiSub");

		for (int index=0;index<numSubMaterials;index++)
		{
			TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.AddSubMaterial(index);
			TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
			int subMaterialRef = m_baseMeshData.m_faceMaterialRefVerts[index];
			tupSubMaterial.AddMaterialReference(subMaterialRef);
			int flags = 0;
			if (subMaterialRef!=-1)
				flags |= TupMultiSubObjectMaterialSubMaterial::ENABLED;
			tupSubMaterial.AddFlags(flags);
			tupSubMaterial.AddSlotName("");
		}
		return materialRef;
	}
}

void MergeMeshesManager::SetFaceFlagsForStages(void)
{
	int numFaces = m_baseMeshData.m_faceFlags.GetSize();

	int faceIndex;
	for (faceIndex=0;faceIndex<numFaces;faceIndex++)
	{
		// possibly the beginning of a stage chain
		if (m_baseMeshData.m_prevStageFaceArray[faceIndex] == -1)
		{
			// stage 0
			m_baseMeshData.m_faceFlags[faceIndex] &= ~TupTriMesh::FLAGS_FACE_STAGE_MASK;
			int nextStageFace = m_baseMeshData.m_nextStageFaceArray[faceIndex];
			// start of stage chain
			if (nextStageFace!=-1)
			{
				int stageIndex = 1; // start at stage 1
				while(1)
				{
					m_baseMeshData.m_faceFlags[nextStageFace] = (m_baseMeshData.m_faceFlags[nextStageFace] & ~TupTriMesh::FLAGS_FACE_STAGE_MASK)|(stageIndex<<TupTriMesh::FLAGS_FACE_STAGE_SHIFT);
					int nextNextStageFace = m_baseMeshData.m_nextStageFaceArray[nextStageFace];
					// if another one in the chain found
					if (nextNextStageFace != -1)
					{
						nextStageFace = nextNextStageFace;
						stageIndex++;
					}
					else
					{
						break; // we are done
					}
				}
			}
		}
	}
}

void MergeMeshesManager::CopyCollisionAttributesToFaces(MeshData &meshData)
{
	TupperwareAggregate *pNodeAgg = m_tupNodePool.GetNode(meshData.m_nodeRef);
	TupNode tupNode(pNodeAgg);

	char *pNodeAttributesString;
	tupNode.GetUserProperties( &pNodeAttributesString );

	TupKeyValue tupKeyValue( pNodeAttributesString, BEGIN_BLOCK, END_BLOCK );
	TupKeyValueContext tupKeyValueContext;

	const int COLLISIONKEY = 0;
	const int DEFAULTCOLLISIONKEY = 1;
	const int SURFACETYPEKEY = 2;
	const int OBBVALUEKEY = 3;

	tupKeyValueContext.AddKey("CollisionType");
	tupKeyValueContext.AddKey("DefaultCollisionType");
	tupKeyValueContext.AddKey("SurfaceType");
	tupKeyValueContext.AddKey("OBBValue");
	m_tupNodeConfigRoot.GetKeyValueContexts(tupKeyValue,tupKeyValueContext,false);

	TupKeyValue replaceFaceCollisionKeysArray;
	TupKeyValue augmentFaceDefaultCollisionKeysArray;

	TupString collisionKey(tupKeyValueContext.GetValueAsString(COLLISIONKEY));
	TupString defaultCollisionKey(tupKeyValueContext.GetValueAsString(DEFAULTCOLLISIONKEY));
	const char *pSurfaceType = tupKeyValueContext.GetValueAsString(SURFACETYPEKEY);
	const char *pObbValue = tupKeyValueContext.GetValueAsString(OBBVALUEKEY);

	// UseMesh
	if (!strcmp(collisionKey,"UseMesh"))
	{
		if (!strcmp(defaultCollisionKey,"UseMesh"))
		{
			// leave defaults
		}
		else if (!strcmp(defaultCollisionKey,"Disabled"))
		{
			augmentFaceDefaultCollisionKeysArray.AddKeyValue("DefaultCollisionType","Disabled");
		}
		else if (!strcmp(defaultCollisionKey,"TerrainOnly"))
		{
			augmentFaceDefaultCollisionKeysArray.AddKeyValue("DefaultCollisionType","TerrainOnly");
			augmentFaceDefaultCollisionKeysArray.AddKeyValue("SurfaceType",pSurfaceType);
		}
		else if (!strcmp(defaultCollisionKey,"OBBOnly"))
		{
			augmentFaceDefaultCollisionKeysArray.AddKeyValue("DefaultCollisionType","OBBOnly");
			augmentFaceDefaultCollisionKeysArray.AddKeyValue("SurfaceType",pSurfaceType);
			augmentFaceDefaultCollisionKeysArray.AddKeyValue("OBBValue",pObbValue);
		}
		else if (!strcmp(defaultCollisionKey,"TerrainAndOBB"))
		{
			augmentFaceDefaultCollisionKeysArray.AddKeyValue("DefaultCollisionType","TerrainAndOBB");
			augmentFaceDefaultCollisionKeysArray.AddKeyValue("SurfaceType",pSurfaceType);
			augmentFaceDefaultCollisionKeysArray.AddKeyValue("OBBValue",pObbValue);
		}
	}
	else if (!strcmp(collisionKey,"Disabled"))
	{
		replaceFaceCollisionKeysArray.AddKeyValue("CollisionType","Disabled");
	}
	else if (!strcmp(collisionKey,"TerrainOnly"))
	{
		replaceFaceCollisionKeysArray.AddKeyValue("CollisionType","TerrainOnly");
		replaceFaceCollisionKeysArray.AddKeyValue("SurfaceType",pSurfaceType);
	}
	else if (!strcmp(collisionKey,"OBBOnly"))
	{
		replaceFaceCollisionKeysArray.AddKeyValue("CollisionType","OBBOnly");
		replaceFaceCollisionKeysArray.AddKeyValue("SurfaceType",pSurfaceType);
		replaceFaceCollisionKeysArray.AddKeyValue("OBBValue",pObbValue);
	}
	else if (!strcmp(collisionKey,"TerrainAndOBB"))
	{
		replaceFaceCollisionKeysArray.AddKeyValue("CollisionType","TerrainAndOBB");
		replaceFaceCollisionKeysArray.AddKeyValue("SurfaceType",pSurfaceType);
		replaceFaceCollisionKeysArray.AddKeyValue("OBBValue",pObbValue);
	}
	// if any replacements to do
	if (replaceFaceCollisionKeysArray.GetNumAttribs() ||
		 augmentFaceDefaultCollisionKeysArray.GetNumAttribs())
	{
		int numFaceAttributeVerts = meshData.m_faceAttributeVerts.GetSize();
		TupArray<int> faceAttributeIndexRemapArray;
		faceAttributeIndexRemapArray.SetSize(numFaceAttributeVerts);
		Keeper<TupString> newFaceAttributeVerts;
		TupString dummy;
		newFaceAttributeVerts.Add(dummy);
		for (int vertIndex=0;vertIndex<numFaceAttributeVerts;vertIndex++)
		{
			TupKeyValue tupKeyValue( meshData.m_faceAttributeVerts[vertIndex]);
			// if any to replace
			if (replaceFaceCollisionKeysArray.GetNumAttribs())
			{
				tupKeyValue.MergeKeyValues(replaceFaceCollisionKeysArray);
			}
			else // some to augment (only when the faces is "UseMesh" - "UseMesh"
			{
				TupKeyValueContext tupKeyValueContext;

				tupKeyValueContext.AddKey("CollisionType");
				tupKeyValueContext.AddKey("DefaultCollisionType");
				m_tupFaceConfigRoot.GetKeyValueContexts(tupKeyValue,tupKeyValueContext,false);

				TupString collisionKey(tupKeyValueContext.GetValueAsString(COLLISIONKEY));
				TupString defaultCollisionKey(tupKeyValueContext.GetValueAsString(DEFAULTCOLLISIONKEY));

				// UseMesh
				if (!strcmp(collisionKey,"UseMesh") && !strcmp(defaultCollisionKey,"UseMesh"))
				{
					tupKeyValue.MergeKeyValues(augmentFaceDefaultCollisionKeysArray); // add augment keys
				}
			}
			TupString newAttribString;
			for (int keyIndex=0;keyIndex<tupKeyValue.GetNumAttribs();keyIndex++)
			{
				newAttribString+=tupKeyValue.GetKeyAsString(keyIndex);
				newAttribString+="=";
				newAttribString+=tupKeyValue.GetValueAsString(keyIndex);
				newAttribString+="\n";
			}
			faceAttributeIndexRemapArray[vertIndex] = newFaceAttributeVerts.Add(newAttribString);
		}
		meshData.m_faceAttributeVerts = newFaceAttributeVerts; // replace verts with new ones
		int numFaceAttributeIndices = meshData.m_faceAttributeIndices.GetSize();
		// remap indices to new verts
		for (int faceIndex3=0;faceIndex3<numFaceAttributeIndices;faceIndex3++)
		{
			meshData.m_faceAttributeIndices[faceIndex3] = faceAttributeIndexRemapArray[meshData.m_faceAttributeIndices[faceIndex3]];
		}
	}
}

void MergeMeshesManager::RemoveCollisionAttributeFromInstance(MeshData &meshData)
{
	TupperwareAggregate *pNodeAgg = m_tupNodePool.GetNode(meshData.m_nodeRef);
	TupNode tupNode(pNodeAgg);

	char *pNodeAttributesString;
	tupNode.GetUserProperties( &pNodeAttributesString );

	TupKeyValue tupKeyValue( pNodeAttributesString, BEGIN_BLOCK, END_BLOCK );
	// change these keys to be no collision
	tupKeyValue.AddKeyValue("CollisionType","UseMesh");
	tupKeyValue.AddKeyValue("DefaultCollisionType","UseMesh");

	TupString newAttributesString = tupKeyValue.BuildString(pNodeAttributesString,NULL,BEGIN_BLOCK,END_BLOCK);
	tupNode.AddUserProperties(newAttributesString);
}

unsigned long MergeMeshesManager::GetMeshCRC(const MeshData &meshData)
{
	CRC newCRC;

	newCRC.AnalyzeData(meshData.m_positionVerts.GetData(),meshData.m_positionVerts.GetSize()*sizeof(TupVertex3f));
	newCRC.AnalyzeData(meshData.m_positionIndices.GetData(),meshData.m_positionIndices.GetSize()*sizeof(int));
	newCRC.AnalyzeData(meshData.m_faceFlags.GetData(),meshData.m_faceFlags.GetSize()*sizeof(int));
	newCRC.AnalyzeData(meshData.m_faceMaterialRefVerts.GetData(),meshData.m_faceMaterialRefVerts.GetSize()*sizeof(int));
	newCRC.AnalyzeData(meshData.m_faceMaterialRefIndices.GetData(),meshData.m_faceMaterialRefIndices.GetSize()*sizeof(int));
	newCRC.AnalyzeData(meshData.m_nextStageFaceArray.GetData(),meshData.m_nextStageFaceArray.GetSize()*sizeof(int));

	int numMeshMaps = meshData.m_meshMaps.GetSize();
	for (int meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
	{
		newCRC.AnalyzeData(meshData.m_meshMaps[meshMapIndex].m_mapChannel);
		newCRC.AnalyzeData(meshData.m_meshMaps[meshMapIndex].m_verts.GetData(),meshData.m_meshMaps[meshMapIndex].m_verts.GetSize()*sizeof(TupVertex3f));
		newCRC.AnalyzeData(meshData.m_meshMaps[meshMapIndex].m_indices.GetData(),meshData.m_meshMaps[meshMapIndex].m_indices.GetSize()*sizeof(int));
	}
	return newCRC.GetResult();
}

unsigned long MergeMeshesManager::GetVertexColorCRC(const MeshData &meshData)
{
	CRC newCRC;

	newCRC.AnalyzeData(meshData.m_vertexColorVerts.GetData(),meshData.m_vertexColorVerts.GetSize()*sizeof(TupVertex3f));
	newCRC.AnalyzeData(meshData.m_vertexColorIndices.GetData(),meshData.m_vertexColorIndices.GetSize()*sizeof(int));
	newCRC.AnalyzeData(meshData.m_vertexAlphaVerts.GetData(),meshData.m_vertexAlphaVerts.GetSize()*sizeof(float));
	newCRC.AnalyzeData(meshData.m_vertexAlphaIndices.GetData(),meshData.m_vertexAlphaIndices.GetSize()*sizeof(int));

	return newCRC.GetResult();
}

