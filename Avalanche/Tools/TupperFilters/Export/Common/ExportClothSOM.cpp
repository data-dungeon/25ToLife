////////////////////////////////////////////////////////////////////////////
//
// ExportClothSOM
//
// Export Cloth SOMS
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportClothSOM.cpp $
 * 
 * *****************  Version 23  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:26a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 22  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 10:39a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * moved mesh connectivity to tupper3d
 * 
 * *****************  Version 21  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 3:54p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 4/25/03    Time: 1:19p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * get right of | for maya name
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 10:50a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * streaming system updates
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 3/07/03    Time: 12:24p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * mesh names come from maya
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 2/07/03    Time: 11:25a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * set up max uv channels to come from the tupmeshmap class
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 10/02/02   Time: 9:33a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * changed uv transform code
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 10/01/02   Time: 9:32a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * build uv transforms additions (rotated images)
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 9/20/02    Time: 1:24p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 9/20/02    Time: 10:04a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * moved build texture contexts into plugin library
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 9/17/02    Time: 2:57p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 9/17/02    Time: 11:27a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Multi Texturing pass 1 complete
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 9/12/02    Time: 4:37p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Handle multi-sub object material with a -1 reference
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 9/04/02    Time: 3:18p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * texture id problem
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 8/07/02    Time: 4:23p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:32p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Memory Leak
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 7/22/02    Time: 2:47p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Cloth Working (first time)
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 7/16/02    Time: 8:38a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * warning
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 7/10/02    Time: 1:27p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * cloth code complete (not tested yet)
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 7/09/02    Time: 3:51p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * cloth SOM part 2
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 7/08/02    Time: 4:00p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * cloth SOM start
 */

#include "ExportPch.h"
#include "ExportClothSOM.h"
#include "ExportBones.h"

// SubMeshVertex methods
ExportClothSOM::SubMeshVertex::SubMeshVertex(void)
{
	m_nodeIndex = 0;
	for (int coordIndex=0;coordIndex<MAX_SOM_TEXTURES_PER_PATCH;coordIndex++)
	{
		m_textureCoordArray[coordIndex].Set(0.0f,0.0f);
	}
}

bool ExportClothSOM::SubMeshVertex::operator==(const ExportClothSOM::SubMeshVertex& rhs)
{
	if (m_nodeIndex!=rhs.m_nodeIndex)
		return false;

	for (int coordIndex=0;coordIndex<MAX_SOM_TEXTURES_PER_PATCH;coordIndex++)
	{
		if (m_textureCoordArray[coordIndex]!=rhs.m_textureCoordArray[coordIndex])
			return false;
	}
	return true;
}

// ExportClothSOM methods
ExportClothSOM::ExportClothSOM(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool) :
	m_pDBLFileName(pDBLFileName), m_options(options), m_dataManager(dataManager),m_tupMaterialPool(tupMaterialPool),m_tupMapPool(tupMapPool),m_tupImagePool(tupImagePool)
{
}

void ExportClothSOM::WriteMeshes(TupTriMesh &tupTriMesh,BoneBuilder &boneBuilder)
{
	m_pTupTriMesh = &tupTriMesh;
	m_pBoneBuilder = &boneBuilder;

	// get which material this mesh will use
	TupperwareAggregate *pObjectAgg = tupTriMesh.GetBaseAggregate();
	// there should be only one material per mesh
	int materialRef = pObjectAgg->FindScalarByKey("Annotate_MaterialRef")->GetAsInt();

	// get the texture context ids for the materials used in the mesh
	m_textureContextIDArray.RemoveAll();
	m_UVTransformArray.RemoveAll();
	m_materialFlagsArray.RemoveAll();

	BuildTextureContextIDs(materialRef);
	BuildUVTransformArray(materialRef);
	BuildFlagsArray(materialRef);

	/* count them first, set up for saving vertex lists (for vertex color arrangement) */
	/* point to vertex data, normal data, etc */
	/* and point to index lists */
	int tempValue;

	tupTriMesh.GetFaceFlags( &m_pFaceFlags, m_numFaces );
	tupTriMesh.GetPositionVerts((float **) &m_pPositionVerts, m_numVerts );
	tupTriMesh.GetPositionIndices( &m_pPositionIndices, tempValue );
	tupTriMesh.GetFaceMaterialIndices(&m_pMaterialIndices,tempValue);

	int numMeshMaps = tupTriMesh.GetNumMeshMaps();
	for (int meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
	{
		TupperwareAggregate *pMeshMapAgg = tupTriMesh.GetMeshMap(meshMapIndex);
		TupTriMeshMap tupTriMeshMap(pMeshMapAgg);
		int mapChannel = tupTriMeshMap.GetMapChannel();
		if (mapChannel>=TupTriMeshMap::MAP_CHANNEL_TEXTURE_UV_START)
		{
			UVMap newUVMap;
			newUVMap.m_mapChannel = mapChannel;
			int oldSize = m_uvChannelToIndexKeeper.GetSize();
			int newIndex = m_uvChannelToIndexKeeper.Add(mapChannel); // keep track of mapChannel to array index
			if (oldSize==newIndex)
			{
				int numMapFaces;
				tupTriMeshMap.GetMapVerts((float **) &newUVMap.m_pUVVerts, newUVMap.m_uvVerts );
				tupTriMeshMap.GetMapIndices( &newUVMap.m_pUVIndices, numMapFaces);
				m_uvMaps.Add(newUVMap);
			}
		}
	}

	TupperwareAggregate *pClothMeshAgg = pObjectAgg->FindAggregateByKey("Annotate_ClothMeshData");
	assert(pClothMeshAgg);
	int numClothSOMs = pClothMeshAgg->FindScalarByKey("NumClothSOMs")->GetAsInt();
	assert(numClothSOMs);

	// get flags for each cloth point
	TupperwareList *pClothPointFlagsList = pClothMeshAgg->FindListByKey("PointFlags");
	assert(pClothPointFlagsList);
	m_pClothPointFlags = pClothPointFlagsList->GetAsInt();
	assert(m_numVerts==pClothPointFlagsList->GetLength());
	// get connections
	TupperwareList *pClothConnectionPointsList = pClothMeshAgg->FindListByKey("ConnectionPoints");
	assert(pClothConnectionPointsList);
	m_pClothConnectionPoints = pClothConnectionPointsList->GetAsInt();
	m_numConnections = pClothConnectionPointsList->GetLength();
	assert((m_numConnections&1)==0); // this better be a even number
	m_numConnections=m_numConnections>>1; // convert from number of ints to the number of connections

	// get SOM indices for faces and points
	TupperwareList *pFaceClothSOMIndicesList = pClothMeshAgg->FindListByKey("FaceClothSOMIndices");
	assert(pFaceClothSOMIndicesList);
	m_pFaceClothSOMIndices = pFaceClothSOMIndicesList->GetAsInt();
	assert(m_numFaces==pFaceClothSOMIndicesList->GetLength());
	TupperwareList *pVertClothSOMIndicesList = pClothMeshAgg->FindListByKey("VertClothSOMIndices");
	assert(pVertClothSOMIndicesList);
	m_pVertClothSOMIndices = pVertClothSOMIndicesList->GetAsInt();
	assert(m_numVerts==pVertClothSOMIndicesList->GetLength());

	m_pTupTriMeshConnectivity = new TupTriMeshConnectivity;
	m_pTupTriMeshConnectivity->SetMeshFaceFlagMask(TupTriMesh::FLAGS_FACE_USES_CLOTH);
	m_pTupTriMeshConnectivity->SetMesh(tupTriMesh,TupTriMeshConnectivity::FLAGS_FACE_CHECK_FACE_FLAGS);


	// do each cloth SOM
	for (int SOMIndex=0;SOMIndex<numClothSOMs;SOMIndex++)
	{
		WriteMesh(SOMIndex);
	}

	m_textureContextIDArray.RemoveAll();
	m_UVTransformArray.RemoveAll();
	m_materialFlagsArray.RemoveAll();
	m_pBoneBuilder = NULL;
	// delete the mesh connectivity data
	delete m_pTupTriMeshConnectivity;
	m_pTupTriMeshConnectivity = NULL;

}

void ExportClothSOM::WriteMesh(int SOMIndex)
{
	m_currentSOMIndex = SOMIndex;
	DBLUtil::RecordListEntry();

	FILE *pDBLFile = DBLUtil::GetDBLFile();

	memset(&m_meshSOM, 0, sizeof(SMeshSOM));

	m_nodeArray.RemoveAll();
	m_constraintArray.RemoveAll();
	m_collisionFaceArray.RemoveAll();
	m_collisionEdgeArray.RemoveAll();
	m_collisionPointArray.RemoveAll();
	m_neighborArray.RemoveAll();
	m_positionNodeRemapArray.RemoveAll();

	BuildNodes();
	BuildConstraints();
	BuildCollisions();
	BuildNodeNeighbors();
	BuildSubMeshes();
	SetDragAndPositionNodes();

	m_meshSOM.dh.u8DataType = DATA_MULTI_SOM;

	TupperwareScalar *pModelNameScalar = m_pTupTriMesh->GetBaseAggregate()->FindScalarByKey("Annotate_ModelName");
	if (pModelNameScalar)
	{
		strncpy(m_meshSOM.d_name, pModelNameScalar->GetAsString(), MESH_NAME_LENGTH - 1);
	}
	else // if modelname is not there then look to see the maya object name (saved only from Maya)
	{
		TupperwareScalar *pMayaObjectNameScalar = m_pTupTriMesh->GetBaseAggregate()->FindScalarByKey("MayaObjectName");
		if (pMayaObjectNameScalar)
		{
			TupString modelName = pMayaObjectNameScalar->GetAsString();
			int foundIndex =  modelName.ReverseFind('|');
			// if we find a | then get the characters to the right
			if (foundIndex!=-1)
			{
				TupString tempName = modelName.Mid(foundIndex+1);
				modelName = tempName;
			}

			strncpy(m_meshSOM.d_name, modelName, MESH_NAME_LENGTH - 1);
		}
		else
		{
			// default to None if not on the mesh
			strncpy(m_meshSOM.d_name, "None", MESH_NAME_LENGTH - 1);
		}
	}

	// set the position and drag nodes
	m_meshSOM.d_positionNode = m_positionNode;
	m_meshSOM.d_dragNode = m_dragNode;

	int nMeshSOMHeaderFilePos = ftell(pDBLFile);
	DBLWrite::WriteRecord(&m_meshSOM, pDBLFile);
	DBLUtil::AlignFile();

	// now rest of data, and start filling in offsets. we also clean up as we export 

	SMeshSOM::SNodeSOM *node = m_nodeArray.GetData();
	m_meshSOM.d_nodeOffset = ftell(pDBLFile) - nMeshSOMHeaderFilePos;
	m_meshSOM.d_numNode = m_nodeArray.GetSize();
	DBLWrite::WriteRecords(m_nodeArray.GetData(), pDBLFile, m_nodeArray.GetSize());
	DBLUtil::AlignFile();

	/* constraints */
	if (m_constraintArray.GetSize() > 0)
	{
		m_meshSOM.d_constraintOffset = ftell(pDBLFile) - nMeshSOMHeaderFilePos;
		m_meshSOM.d_numConstraint = m_constraintArray.GetSize();
		DBLWrite::WriteRecords(m_constraintArray.GetData(), pDBLFile, m_constraintArray.GetSize());
		DBLUtil::AlignFile();
	}

	/* collision face */
	if (m_collisionFaceArray.GetSize() > 0)
	{
		m_meshSOM.d_collisionFaceOffset = ftell(pDBLFile) - nMeshSOMHeaderFilePos;
		m_meshSOM.d_numCollisionFace = m_collisionFaceArray.GetSize();
		DBLWrite::WriteRecords(m_collisionFaceArray.GetData(), pDBLFile, m_collisionFaceArray.GetSize());
		DBLUtil::AlignFile();
	}

	/* collision edge */
	if (m_collisionEdgeArray.GetSize() > 0)
	{
		m_meshSOM.d_collisionEdgeOffset = ftell(pDBLFile) - nMeshSOMHeaderFilePos;
		m_meshSOM.d_numCollisionEdge = m_collisionEdgeArray.GetSize();
		DBLWrite::WriteRecords(m_collisionEdgeArray.GetData(), pDBLFile, m_collisionEdgeArray.GetSize());
		DBLUtil::AlignFile();
	}

	/* collision point */
	if (m_collisionPointArray.GetSize() > 0)
	{
		m_meshSOM.d_collisionPointOffset = ftell(pDBLFile) - nMeshSOMHeaderFilePos;
		m_meshSOM.d_numCollisionPoint = m_collisionPointArray.GetSize();
		DBLWrite::WriteRecords(m_collisionPointArray.GetData(), pDBLFile, m_collisionPointArray.GetSize());
		DBLUtil::AlignFile();
	}

	// sub-meshes. each sub mesh has associated data that needs fix up 
	int numSubMeshes = m_subMeshArray.GetSize();
	if (numSubMeshes)
	{
		TupArray<SMeshSOM::SSubMesh> subMeshArrayWrite;
		subMeshArrayWrite.SetSize(numSubMeshes);

		// write out the headers first (we will write to them later with the correct offsets)
		int nSubMeshHeaderFilePos = ftell(pDBLFile);
		m_meshSOM.d_subMeshOffset = nSubMeshHeaderFilePos - nMeshSOMHeaderFilePos;
		m_meshSOM.d_numSubMesh = numSubMeshes;
		DBLWrite::WriteRecords(subMeshArrayWrite.GetData(), pDBLFile, numSubMeshes);
		DBLUtil::AlignFile();

		// now write out data associated with each sub mesh, and update records as we go 
		for (int subMeshIndex = 0;subMeshIndex<numSubMeshes;subMeshIndex++)
		{
			SMeshSOM::SSubMesh &subMeshWrite = subMeshArrayWrite[subMeshIndex];
			SubMesh &subMeshRead = m_subMeshArray[subMeshIndex];

			// get texture context index
			subMeshWrite.d_textureContextIndex = subMeshRead.m_textureContextIndex;

			// node index
			subMeshWrite.d_nodeIndexOffset = ftell(pDBLFile) - nMeshSOMHeaderFilePos;
			int numNodes = subMeshRead.m_vertexKeeper.GetSize();
			int nodeIndex;
			for (nodeIndex=0;nodeIndex<numNodes;nodeIndex++)
			{
				DBLWrite::WriteRecord(&subMeshRead.m_vertexKeeper[nodeIndex].m_nodeIndex,pDBLFile);
			}
			DBLUtil::AlignFile();

			// texture coord 
			// only one set for now
			subMeshWrite.d_textureCoordOffset = ftell(pDBLFile) - nMeshSOMHeaderFilePos;
			for (nodeIndex=0;nodeIndex<numNodes;nodeIndex++)
			{
				DBLWrite::WriteRecord(&subMeshRead.m_vertexKeeper[nodeIndex].m_textureCoordArray[0],pDBLFile);
			}
			DBLUtil::AlignFile();

			// tri strip head
			subMeshWrite.d_triStripHeadOffset = ftell(pDBLFile) - nMeshSOMHeaderFilePos;
			DBLWrite::WriteRecords(subMeshRead.m_triStripHeadArray.GetData(),pDBLFile,subMeshRead.m_triStripHeadArray.GetSize());
			DBLUtil::AlignFile();
			subMeshWrite.d_numTriStrip = subMeshRead.m_triStripHeadArray.GetSize();

			// tri strip points
			subMeshWrite.d_triStripOffset = ftell(pDBLFile) - nMeshSOMHeaderFilePos;
			DBLWrite::WriteRecords(subMeshRead.m_triStripArray.GetData(),pDBLFile,subMeshRead.m_triStripArray.GetSize());

			// number of nodes in the node index array
			subMeshWrite.d_numVertex = numNodes;
			DBLUtil::AlignFile();
		}
		
		// re-write sub-mesh records now that we know offsets
		fseek(pDBLFile, nSubMeshHeaderFilePos, SEEK_SET);
		DBLWrite::WriteRecords(subMeshArrayWrite.GetData(), pDBLFile, numSubMeshes);
		fseek(pDBLFile, 0, SEEK_END);	// back to end of file
	}

	// neighbor 
	if (m_neighborArray.GetSize() > 0)
	{
		m_meshSOM.d_neighborOffset = ftell(pDBLFile) - nMeshSOMHeaderFilePos;
		DBLWrite::WriteRecords(m_neighborArray.GetData(), pDBLFile, m_neighborArray.GetSize());
		DBLUtil::AlignFile();
	}

	// Re-write SMeshSOM header now that it's filled in 
	fseek(pDBLFile, nMeshSOMHeaderFilePos, SEEK_SET);
	DBLWrite::WriteRecord(&m_meshSOM, pDBLFile);
	fseek(pDBLFile, 0, SEEK_END);	// back to end of file
}

// this creates an array of ints and fills it with the proper texture context ids for the mesh
void ExportClothSOM::BuildTextureContextIDs(int materialReference)
{
	TupperwareAggregate *pMaterialAgg = NULL;
	if (materialReference!=-1)
		pMaterialAgg = m_tupMaterialPool.GetMaterial(materialReference);

	if (pMaterialAgg)
	{
		TupMaterial tupMaterial(pMaterialAgg);

		if ( tupMaterial.GetMaterialType( ) == TupMaterial::MULTI_SUB_OBJECT_MATERIAL )
		{
			// we need to deal with multiple materials
			TupMultiSubObjectMaterial tupMultiSubObjectMaterial(tupMaterial);
			int totalTextureContextIDs = tupMultiSubObjectMaterial.GetNumSubMaterials();
			m_textureContextIDArray.SetSize(totalTextureContextIDs);
			if (totalTextureContextIDs)
			{
				for (int materialIndex = 0;materialIndex<totalTextureContextIDs;materialIndex++)
				{
					TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(materialIndex);
					TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
					int materialPoolIndex;
					tupSubMaterial.GetMaterialReference(materialPoolIndex);
					if (materialPoolIndex!=-1)
					{
						TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial( materialPoolIndex );
						TupMaterial tupSubMat( pMaterialAgg );
						if (tupSubMat.GetMaterialType()==TupMaterial::STANDARD_MATERIAL)
						{
							// get id from material (this better be a standard material for now)
							TupperwareList *pTextureContextIDList = pMaterialAgg->FindListByKey("Annotate_TextureContextIDs");
							if (pTextureContextIDList)
							{
								int *pTextureContextIDs = pTextureContextIDList->GetAsInt();
								int textureID = pTextureContextIDs[0]; // always from the first
								assert(textureID!=-1);
								m_textureContextIDArray[materialIndex] = textureID;
							}
							else
								m_textureContextIDArray[materialIndex] = 0; // use the blank one if not set (this slot should not be used)
						}
						else
						{
							m_textureContextIDArray[materialIndex] = 0;
							char *pMultiName;
							tupMultiSubObjectMaterial.GetName(&pMultiName);
							TupperwareScalar *pSubMatNameScalar = tupSubMat.GetBaseAggregate()->FindScalarByKey("Name");
							if (pSubMatNameScalar)
							{
								PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
									"Warning - Multi-Sub Object Material %s referencing non standard material %s in slot %d",
									pMultiName,pSubMatNameScalar->GetAsString(),materialIndex);
							}
						}
					}
					else
					{
						m_textureContextIDArray[materialIndex] = 0;
					}
				}
			}
		}
		else if ( tupMaterial.GetMaterialType( ) == TupMaterial::STANDARD_MATERIAL )
		{
			// just deal with 1 material
			m_textureContextIDArray.SetSize(1);
			// get id from material (this better be a standard material for now)
			TupperwareList *pTextureContextIDList = pMaterialAgg->FindListByKey("Annotate_TextureContextIDs");
			if (pTextureContextIDList)
			{
				int *pTextureContextIDs = pTextureContextIDList->GetAsInt();
				int textureID = pTextureContextIDs[0]; // always from the first
				assert(textureID!=-1);
				m_textureContextIDArray[0] = textureID;
			}
			else
				m_textureContextIDArray[0] = 0; // use the blank one if not set (this slot should not be used)
		}
		else // any other materials
		{
			// just deal with 1 material
			m_textureContextIDArray.SetSize(1);
			m_textureContextIDArray[0] = 0; // use the first one (no texture)
			TupperwareScalar *pMatNameScalar = pMaterialAgg->FindScalarByKey("Name");
			if (pMatNameScalar)
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"Warning - Unknown material %s",pMatNameScalar->GetAsString());
			}
		}
	}
	else // not using a material (-1)
	{
		// just deal with 1 material
		m_textureContextIDArray.SetSize(1);
		m_textureContextIDArray[0] = 0; // use the first one (no texture)
//		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"Warning - Using Material Reference of -1");
	}
}

// this creates an array of ints and fills it with the proper texture context ids for the mesh
void ExportClothSOM::BuildUVTransformArray(int materialRef)
{
#if EXPORT_PS2
	bool bPs2 = true;
#else
	bool bPs2 = false;
#endif

	BuildUVTransformManager buildUVTransformManager(m_tupMaterialPool,m_tupMapPool,m_tupImagePool,bPs2);

	TupperwareAggregate *pMaterialAgg = NULL;
	if (materialRef!=-1)
		pMaterialAgg = m_tupMaterialPool.GetMaterial(materialRef);

	if (pMaterialAgg)
	{
		TupMaterial tupMat(pMaterialAgg);

		if ( tupMat.GetMaterialType( ) == TupMaterial::MULTI_SUB_OBJECT_MATERIAL )
		{
			// we need to deal with multiple materials
			TupMultiSubObjectMaterial tupMultiSubObjectMaterial(tupMat);
			int totalUVTransforms = tupMultiSubObjectMaterial.GetNumSubMaterials();
			if (totalUVTransforms)
			{
				m_UVTransformArray.SetSize(totalUVTransforms);
				for (int materialIndex = 0;materialIndex<totalUVTransforms;materialIndex++)
				{
					TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(materialIndex);
					TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
					int subMaterialRef;
					tupSubMaterial.GetMaterialReference(subMaterialRef);
					buildUVTransformManager.BuildUVTransforms(subMaterialRef,m_UVTransformArray[materialIndex]);
				}
			}
			else
			{
				m_UVTransformArray.SetSize(1);
				m_UVTransformArray[0].SetSize(1);
				m_UVTransformArray[0][0].Init();
			}
		}
		else
		{
			m_UVTransformArray.SetSize(1);
			buildUVTransformManager.BuildUVTransforms(materialRef,m_UVTransformArray[0]);
		}
	}
	else
	{
		m_UVTransformArray.SetSize(1);
		m_UVTransformArray[0].SetSize(1);
		m_UVTransformArray[0][0].Init();
	}
}


void ExportClothSOM::BuildFlagsArray(int materialRef)
{
	TupperwareAggregate *pMaterialAgg = NULL;
	if (materialRef!=-1)
		pMaterialAgg = m_tupMaterialPool.GetMaterial(materialRef);

	if (pMaterialAgg)
	{
		TupMaterial tupMat(pMaterialAgg);

		if ( tupMat.GetMaterialType( ) == TupMaterial::MULTI_SUB_OBJECT_MATERIAL )
		{
			// we need to deal with multiple materials
			TupMultiSubObjectMaterial tupMultiSubObjectMaterial(tupMat);
			int totalMaterialFlags = tupMultiSubObjectMaterial.GetNumSubMaterials();
			if (totalMaterialFlags)
			{
				m_materialFlagsArray.SetSize(totalMaterialFlags);
				for (int materialIndex = 0;materialIndex<totalMaterialFlags;materialIndex++)
				{
					TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(materialIndex);
					TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
					int materialPoolIndex;
					tupSubMaterial.GetMaterialReference(materialPoolIndex);
					m_materialFlagsArray[materialIndex] = 0; // default
					if (materialPoolIndex!=-1)
					{
						TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial( materialPoolIndex );
						TupMaterial tupSubMat( pMaterialAgg );
						if (tupSubMat.GetMaterialType()==TupMaterial::STANDARD_MATERIAL)
						{
							TupStandardMaterial tupStandardMaterial(pMaterialAgg);
							int flags;
							tupStandardMaterial.GetFlags(flags);
							m_materialFlagsArray[materialIndex] = flags;
						}
					}
				}
			}
			else
			{
				m_materialFlagsArray.SetSize(1);
				m_materialFlagsArray[0] = 0; 
			}
		}
		else if ( tupMat.GetMaterialType( ) == TupMaterial::STANDARD_MATERIAL )
		{
			// just deal with 1 material
			m_materialFlagsArray.SetSize(1);
			TupStandardMaterial tupStandardMaterial(tupMat);
			int flags;
			tupStandardMaterial.GetFlags(flags);
			m_materialFlagsArray[0] = flags;
		}
		else // any other materials
		{
			m_materialFlagsArray.SetSize(1);
			m_materialFlagsArray[0] = 0; 
		}
	}
	else
	{
		m_materialFlagsArray.SetSize(1);
		m_materialFlagsArray[0] = 0; 
	}
}

void ExportClothSOM::BuildNodes(void)
{
	m_positionNodeRemapArray.SetSize(m_numVerts);
	int vertIndex;

	// this array indicates which bone each point uses (0 means no bone)
	const TupArray<int> &boneVerts = m_pBoneBuilder->GetBoneVerts();
	bool hasBones = (boneVerts.GetSize()!=0);

	// add the front points first
	for (vertIndex = 0;vertIndex<m_numVerts;vertIndex++)
	{
		// make sure point is for this SOM as well as not a back point
		if (m_pVertClothSOMIndices[vertIndex] == m_currentSOMIndex &&
			((m_pClothPointFlags[vertIndex] & TupClothModifierContext::FLAGS_POINT_BACK)==0))
		{
			SMeshSOM::SNodeSOM newNode;
			memset(&newNode, 0, sizeof(SMeshSOM::SNodeSOM));
			newNode.d_baseVertexInModel = m_pPositionVerts[vertIndex];
			// set up the bone id (set to bone or invalid if 0)
			newNode.d_masterBoneID = (hasBones && boneVerts[vertIndex]!=0) ? boneVerts[vertIndex] : INVALID_BONEID;
			// set the attached flag
			if (m_pClothPointFlags[vertIndex] & TupClothModifierContext::FLAGS_POINT_ATTACHED)
				newNode.d_flag |= NODE_ATTACHED;
			// remap points to index in node array
			m_positionNodeRemapArray[vertIndex] = m_nodeArray.GetSize();
			m_nodeArray.Add(newNode);
		}
		else
		{
			m_positionNodeRemapArray[vertIndex] = -1;
		}
	}
	// loop through connections and link back points to front points
	for (int connectionIndex=0;connectionIndex<m_numConnections;connectionIndex++)
	{
		int connectionA = m_pClothConnectionPoints[(connectionIndex<<1)];
		int connectionB = m_pClothConnectionPoints[(connectionIndex<<1)+1];
		if (m_pVertClothSOMIndices[connectionA] == m_currentSOMIndex)
		{
			assert(m_pVertClothSOMIndices[connectionB] == m_currentSOMIndex);
			// if first is a front point
			if ((m_pClothPointFlags[connectionA] & TupClothModifierContext::FLAGS_POINT_BACK)==0)
			{
				// second is a back point
				if ((m_pClothPointFlags[connectionB] & TupClothModifierContext::FLAGS_POINT_BACK)!=0)
				{
					// make second point remap the same node as the front point
					int connectionANodeIndex = m_positionNodeRemapArray[connectionA];
					assert(connectionANodeIndex!=-1);
					m_positionNodeRemapArray[connectionB] = connectionANodeIndex;
					SMeshSOM::SNodeSOM &node = m_nodeArray[connectionANodeIndex];
					node.d_flag |= NODE_HAS_THICKNESS;
					node.d_numBackSide++; // keep track of the number of back points (use this value differently later)
					node.d_thickness += (m_pPositionVerts[connectionA]-m_pPositionVerts[connectionB]).Length();
				}
			}
			// first is a back point
			else
			{
				// second is a front point
				if ((m_pClothPointFlags[connectionB] & TupClothModifierContext::FLAGS_POINT_BACK)==0)
				{
					// make second point remap the same node as the front point
					int connectionBNodeIndex = m_positionNodeRemapArray[connectionB];
					assert(connectionBNodeIndex!=-1);
					m_positionNodeRemapArray[connectionA] = connectionBNodeIndex;
					SMeshSOM::SNodeSOM &node = m_nodeArray[connectionBNodeIndex];
					node.d_flag |= NODE_HAS_THICKNESS;
					node.d_numBackSide++; // keep track of the number of back points (use this value differently later)
					node.d_thickness += (m_pPositionVerts[connectionA]-m_pPositionVerts[connectionB]).Length();
				}
			}
		}
	}
	// find any back points that are not connected to front points and make them front points
	for (vertIndex = 0;vertIndex<m_numVerts;vertIndex++)
	{
		// make sure point is for this SOM as well as not a back point
		if (m_pVertClothSOMIndices[vertIndex] == m_currentSOMIndex &&
			m_positionNodeRemapArray[vertIndex] == -1 &&
			((m_pClothPointFlags[vertIndex] & TupClothModifierContext::FLAGS_POINT_BACK)!=0))
		{
			SMeshSOM::SNodeSOM newNode;
			memset(&newNode, 0, sizeof(SMeshSOM::SNodeSOM));
			newNode.d_baseVertexInModel = m_pPositionVerts[vertIndex];
			newNode.d_masterBoneID = INVALID_BONEID;
			// make this back point now a front point
			m_pClothPointFlags[vertIndex] &= ~TupClothModifierContext::FLAGS_POINT_BACK;
			// set the attached flag
			if (m_pClothPointFlags[vertIndex] & TupClothModifierContext::FLAGS_POINT_ATTACHED)
				newNode.d_flag |= NODE_ATTACHED;
			// remap points to index in node array
			m_positionNodeRemapArray[vertIndex] = m_nodeArray.GetSize();
			m_nodeArray.Add(newNode);
		}
	}
	// go through the nodes and set the thicknesses to be an average based on the number of back points
	int numNodes = m_nodeArray.GetSize();
	int nodeIndex;
	for (nodeIndex = 0;nodeIndex<numNodes;nodeIndex++)
	{
		SMeshSOM::SNodeSOM &node = m_nodeArray[nodeIndex];
		if (node.d_numBackSide)
		{
			node.d_thickness/=node.d_numBackSide;
			node.d_numBackSide = 0;
		}
	}
}

void ExportClothSOM::BuildConstraints(void)
{
	int sameIndexCount=0;
	int backBackConnectionCount=0;
	int attachedConnectionCount=0;
	int duplcateConnectionCount=0;
	// loop through connections and link back points to front points
	for (int connectionIndex=0;connectionIndex<m_numConnections;connectionIndex++)
	{
		int connectionA = m_pClothConnectionPoints[(connectionIndex<<1)];
		int connectionB = m_pClothConnectionPoints[(connectionIndex<<1)+1];

		// don't do if the index is the same
		if (connectionA==connectionB)
		{
			sameIndexCount++; // keep track of these for a warning
			continue;
		}

		if (m_pVertClothSOMIndices[connectionA] == m_currentSOMIndex)
		{
			assert(m_pVertClothSOMIndices[connectionB] == m_currentSOMIndex);
			// if first and second are front points
			if (((m_pClothPointFlags[connectionA] & TupClothModifierContext::FLAGS_POINT_BACK)==0) &&
				((m_pClothPointFlags[connectionB] & TupClothModifierContext::FLAGS_POINT_BACK)==0))
			{
				// make sure one is not attached
				if (((m_pClothPointFlags[connectionA] & TupClothModifierContext::FLAGS_POINT_ATTACHED)==0) ||
					((m_pClothPointFlags[connectionB] & TupClothModifierContext::FLAGS_POINT_ATTACHED)==0))
				{
					SMeshSOM::SIndexPair newPair;
					newPair.d_index[0] = m_positionNodeRemapArray[connectionA];
					newPair.d_index[1] = m_positionNodeRemapArray[connectionB];
					assert(newPair.d_index[0] !=-1 && newPair.d_index[1] !=-1);
					assert(newPair.d_index[0] != newPair.d_index[1]);
					if (newPair.d_index[1]<newPair.d_index[0])
					{
						uintCount temp = newPair.d_index[0];
						newPair.d_index[0] = newPair.d_index[1];
						newPair.d_index[1] = temp;
					}
					// make sure we don't add a duplicate constraint
					int numConstraints = m_constraintArray.GetSize();
					int constraintIndex;
					for (constraintIndex=0;constraintIndex<numConstraints;constraintIndex++)
					{
						SMeshSOM::SIndexPair &pair = m_constraintArray[constraintIndex];
						if (pair.d_index[0] == newPair.d_index[0] &&
							pair.d_index[1] == newPair.d_index[1])
						{
							duplcateConnectionCount++;
							break;
						}
					}
					// add if not found
					if (constraintIndex==numConstraints)
						m_constraintArray.Add(newPair);
				}
				else // if both attached
				{
					attachedConnectionCount++; // keep track of connections between two attached points
				}
			}
			// check if both back points
			else if (((m_pClothPointFlags[connectionA] & TupClothModifierContext::FLAGS_POINT_BACK)!=0) &&
				((m_pClothPointFlags[connectionB] & TupClothModifierContext::FLAGS_POINT_BACK)!=0))
			{
				backBackConnectionCount++; // keep track of these for a warning
			}
		}
	}
	if (sameIndexCount)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"Warning - Cloth Mesh has %d Connections refering to the same point",sameIndexCount);
	}
	if (backBackConnectionCount)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"Warning - Cloth Mesh has %d Connections between back points",backBackConnectionCount);
	}
	if (attachedConnectionCount)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"Warning - Cloth Mesh has %d Connections between attached points",attachedConnectionCount);
	}
	if (duplcateConnectionCount)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"Warning - Cloth Mesh has %d Connections that are duplicated",duplcateConnectionCount);
	}
}

void ExportClothSOM::BuildCollisions(void)
{
	int faceIndex,pointIndex,nodeIndex;
	int unattachedPointCount;

	enum pointFlags
	{
		FLAGS_COLLISION_FACE = 0x0001,
		FLAGS_COLLISION_EDGE = 0x0002,
		FLAGS_COLLISION_POINT = 0x0004,
	};

	// array of flags for the nodes
	TupArray<int> nodeFlags;
	nodeFlags.SetSize(m_nodeArray.GetSize());
	for (nodeIndex=0;nodeIndex<nodeFlags.GetSize();nodeIndex++)
		nodeFlags[nodeIndex] = 0;

	// array of flags for the faces
	TupArray<int> faceFlags;
	faceFlags.SetSize(m_numFaces);

	// do the face collisions
	for (faceIndex=0;faceIndex<m_numFaces;faceIndex++)
	{
		faceFlags[faceIndex] = 0; // clear face flag (indicates if this face was done previously (for edge and point loops)
		// do only faces that belong to my SOM
		if (m_pFaceClothSOMIndices[faceIndex] == m_currentSOMIndex)
		{
			SMeshSOM::SIndexTrio newFaceCollision;
			unattachedPointCount = 0;
			// check each point on the face and see if it is attached
			for (pointIndex=0;pointIndex<3;pointIndex++)
			{
				int vertIndex = m_pPositionIndices[(faceIndex<<1) + faceIndex + pointIndex];
				assert(m_pVertClothSOMIndices[vertIndex] == m_currentSOMIndex);
				int flags = m_pClothPointFlags[vertIndex];
				// make sure it is not attached and is a front point
				if ((flags & (TupClothModifierContext::FLAGS_POINT_ATTACHED | TupClothModifierContext::FLAGS_POINT_BACK))==0)
				{
					unattachedPointCount++;
					nodeIndex = m_positionNodeRemapArray[vertIndex];
					assert(nodeIndex!=-1);
					newFaceCollision.d_index[pointIndex] = nodeIndex;
				}
			}
			// if all three points are not attached then add to collision face array
			if (unattachedPointCount==3)
			{
				m_collisionFaceArray.Add(newFaceCollision);
				faceFlags[faceIndex] = 1; // mark face as been done previously
				for (pointIndex=0;pointIndex<3;pointIndex++)
				{
					// mark point as in a face collision
					nodeFlags[newFaceCollision.d_index[pointIndex]] |= FLAGS_COLLISION_FACE;
				}
			}
		}
	}
	// do the edge collisions
	for (faceIndex=0;faceIndex<m_numFaces;faceIndex++)
	{
		// do only faces that belong to my SOM as well as not done previously
		if (m_pFaceClothSOMIndices[faceIndex] == m_currentSOMIndex &&
			faceFlags[faceIndex] == 0)
		{
			SMeshSOM::SIndexPair newEdgeCollision;
			unattachedPointCount = 0;
			// check each point on the face and see if it is attached
			for (pointIndex=0;pointIndex<3;pointIndex++)
			{
				int vertIndex = m_pPositionIndices[(faceIndex<<1) + faceIndex + pointIndex];
				assert(m_pVertClothSOMIndices[vertIndex] == m_currentSOMIndex);
				int flags = m_pClothPointFlags[vertIndex];
				nodeIndex = m_positionNodeRemapArray[vertIndex];
				assert(nodeIndex!=-1);
				// make sure it is not attached, is a front point and not done previously
				if ((flags & (TupClothModifierContext::FLAGS_POINT_ATTACHED | TupClothModifierContext::FLAGS_POINT_BACK))==0 &&
					nodeFlags[nodeIndex]==0)
				{
					// only keep track of two points at the most
					newEdgeCollision.d_index[unattachedPointCount] = nodeIndex;
					unattachedPointCount++;
				}
			}
			// if two of the three points are not attached then add to collision edge array
			assert(unattachedPointCount!=3); // this better not happen
			if (unattachedPointCount==2)
			{
				m_collisionEdgeArray.Add(newEdgeCollision);
				faceFlags[faceIndex] = 1; // mark face as been done previously
				for (pointIndex=0;pointIndex<2;pointIndex++)
				{
					// mark point as in a edge collision
					nodeFlags[newEdgeCollision.d_index[pointIndex]] |= FLAGS_COLLISION_EDGE;
				}
			}
		}
	}
	// do the point collisions
	for (faceIndex=0;faceIndex<m_numFaces;faceIndex++)
	{
		// do only faces that belong to my SOM as well as not done previously
		if (m_pFaceClothSOMIndices[faceIndex] == m_currentSOMIndex &&
			faceFlags[faceIndex] == 0)
		{
			uintCount newPointCollision;
			unattachedPointCount = 0;
			// check each point on the face and see if it is attached
			for (pointIndex=0;pointIndex<3;pointIndex++)
			{
				int vertIndex = m_pPositionIndices[(faceIndex<<1) + faceIndex + pointIndex];
				assert(m_pVertClothSOMIndices[vertIndex] == m_currentSOMIndex);
				int flags = m_pClothPointFlags[vertIndex];
				nodeIndex = m_positionNodeRemapArray[vertIndex];
				assert(nodeIndex!=-1);
				// make sure it is not attached, is a front point and not done previously
				if ((flags & (TupClothModifierContext::FLAGS_POINT_ATTACHED | TupClothModifierContext::FLAGS_POINT_BACK))==0 &&
					nodeFlags[nodeIndex]==0)
				{
					// only keep track of 1 point at the most
					newPointCollision = nodeIndex;
					unattachedPointCount++;
				}
			}
			// if two of the three points are not attached then add to collision point array
			assert(unattachedPointCount<2); // this better not happen
			if (unattachedPointCount==1)
			{
				m_collisionPointArray.Add(newPointCollision);
				faceFlags[faceIndex] = 1; // mark face as been done previously
				// mark point as in a point collision
				nodeFlags[newPointCollision] |= FLAGS_COLLISION_POINT;
			}
		}
	}
}

void ExportClothSOM::BuildNodeNeighbors(void)
{
	int nodeIndex;
	// have an array of arrays to store the neighbors for all the nodes
	TupArray<TupArray<uintCount> > nodeNeighborArrays;
	nodeNeighborArrays.SetSize(m_nodeArray.GetSize());
	// have an array to keep track of the number of fans for each node
	TupArray<int> nodeNumFansArray;
	nodeNumFansArray.SetSize(m_nodeArray.GetSize());
	for (nodeIndex=0;nodeIndex<m_nodeArray.GetSize();nodeIndex++)
		nodeNumFansArray[nodeIndex] = 0;
	// have an array of flags for each point on a face
	TupArray<int> facePointFlags;
	facePointFlags.SetSize((m_numFaces<<1) + m_numFaces);
	// clear all flags (not added to neighbor arrays)
	for (int facePointIndex=0;facePointIndex<facePointFlags.GetSize();facePointIndex++)
	{
		facePointFlags[facePointIndex] = 0;
	}
	// loop through all the faces to find a point that matches the node
	for (int faceIndex=0;faceIndex<m_numFaces;faceIndex++)
	{
		// make sure it is part of my SOM
		if (m_pFaceClothSOMIndices[faceIndex] == m_currentSOMIndex)
		{
			for (int pointIndex=0;pointIndex<3;pointIndex++)
			{
				// make sure this point has not been done before
				if (facePointFlags[(faceIndex<<1) + faceIndex + pointIndex] == 0)
				{
					int vertIndex = m_pPositionIndices[(faceIndex<<1) + faceIndex + pointIndex];
					assert(m_pVertClothSOMIndices[vertIndex] == m_currentSOMIndex);
					int flags = m_pClothPointFlags[vertIndex];
					// make sure it is a front point 
					if ((flags & TupClothModifierContext::FLAGS_POINT_BACK)==0)
					{
						nodeIndex = m_positionNodeRemapArray[vertIndex];
						int scanFace = faceIndex;
						int scanPoint = pointIndex;
						// try to walk through the adjacent faces clockwise to find the start of the fan
						while(1)
						{
							const TupTriMeshConnectivity::TriFace &triFace = m_pTupTriMeshConnectivity->GetFace(scanFace);
							const TupTriMeshConnectivity::Edge &edge = triFace.GetEdge(scanPoint);
							int adjacentFace = edge.GetFaceIndex();
							// if we hit our starting face or no face
							if (adjacentFace==-1 || adjacentFace==faceIndex)
								break;
							// continue checking 
							scanFace = adjacentFace;
							scanPoint = edge.GetPointIndex()<2 ? edge.GetPointIndex()+1 : 0;
							int adjacentVertIndex = m_pPositionIndices[(scanFace<<1) + scanFace + scanPoint];
							assert(adjacentVertIndex == vertIndex); // this better be the same point
						}
						// now walk counter clockwise starting with the scanface and add points to the neighbor array for the node
						int startFace = scanFace;
						scanPoint = scanPoint>0 ? scanPoint-1 : 2;
						while(1)
						{
							// we have 3 points - prevPoint,commonPoint & scanPoint
							// scanPoint is the point that marks the edge to the next counter clockwise face
							// prevPoint is the point we are going to add
							// commonPoint is the center point of the fan
							// commonPoint++ = prevPoint
							// prevPoint++ = scanPoint
							// scanPoint++ = commonPoint
							int prevPoint = scanPoint>0 ? scanPoint-1 : 2;
							int addVertIndex = m_pPositionIndices[(scanFace<<1) + scanFace + prevPoint];
							uintCount addNodeIndex = m_positionNodeRemapArray[addVertIndex];
							assert(addNodeIndex!=-1);
							// add the prevPoint node to the neighbor array
							nodeNeighborArrays[nodeIndex].Add(addNodeIndex);

							int commonPoint = prevPoint>0 ? prevPoint-1 : 2;
							// make sure the common point vertex is the same as our face
							assert(vertIndex == m_pPositionIndices[(scanFace<<1) + scanFace + commonPoint]);
							// make sure we haven't done this point on this face before
							assert(facePointFlags[(scanFace<<1) + scanFace + commonPoint]==0);
							// mark this point on this face done
							facePointFlags[(scanFace<<1) + scanFace + commonPoint]=1;
						
							// find the adjacent face using scanFace and scanPoint
							const TupTriMeshConnectivity::TriFace &triFace = m_pTupTriMeshConnectivity->GetFace(scanFace);
							const TupTriMeshConnectivity::Edge &edge = triFace.GetEdge(scanPoint);
							int adjacentFace = edge.GetFaceIndex();
							// if we hit our starting face or no face
							if (adjacentFace==-1 || adjacentFace==startFace)
							{
								// add the last node in the fan
								int addVertIndex = m_pPositionIndices[(scanFace<<1) + scanFace + scanPoint];
								uintCount addNodeIndex = m_positionNodeRemapArray[addVertIndex];
								assert(addNodeIndex!=-1);
								addNodeIndex|=0x8000; // mark as the end of the fan
								nodeNeighborArrays[nodeIndex].Add(addNodeIndex);
								nodeNumFansArray[nodeIndex]++; // one more fan
								break;
							}
							// continue checking 
							scanFace = adjacentFace;
							scanPoint = edge.GetPointIndex()>0 ? edge.GetPointIndex()-1 : 2;
						}
					}
				}
			}
		}
	}
	// now go through the nodes and remove the last neighbor if necessary
	for (nodeIndex=0;nodeIndex<m_nodeArray.GetSize();nodeIndex++)
	{
		int numFans = nodeNumFansArray[nodeIndex];
		assert(numFans!=0);
		// if there is one fan 
		if (numFans==1)
		{
			// check if the first and last neighbor node are the same
			int numNeighbors = nodeNeighborArrays[nodeIndex].GetSize();
			if (nodeNeighborArrays[nodeIndex][0] == (nodeNeighborArrays[nodeIndex][numNeighbors-1]&0x7fff))
			{
				// if they are the same remove the last one
				nodeNeighborArrays[nodeIndex].RemoveAt(numNeighbors-1);
			}
		}
	}
	// now add the neighbors into one big array and set the node neighbor offsets
	for (nodeIndex=0;nodeIndex<m_nodeArray.GetSize();nodeIndex++)
	{
		int numNeighbors = nodeNeighborArrays[nodeIndex].GetSize();
		for (int neighborIndex=0;neighborIndex<numNeighbors;neighborIndex++)
		{
			m_neighborArray.Add(nodeNeighborArrays[nodeIndex][neighborIndex]);
		}
		// mark the offset for the node to the last entry +1
		m_nodeArray[nodeIndex].d_endNeighbor = m_neighborArray.GetSize();
	}
}

void ExportClothSOM::BuildSubMeshes(void)
{
	TupperwareAggregate *pTriangleStripsAgg = m_pTupTriMesh->GetTriangleStrips();
	if (!pTriangleStripsAgg)
		throw(PluginException(ERROR_FATAL,"Cannot find TriStrips"));

	TupTriangleStripPool tupTriangleStripPool(pTriangleStripsAgg);
	int totalStrips = tupTriangleStripPool.GetNumTriangleStrips();
	for (int stripIndex=0;stripIndex<totalStrips;stripIndex++)
	{
		TupperwareAggregate *pTriangleStripAgg = tupTriangleStripPool.GetTriangleStrip(stripIndex);
		assert(pTriangleStripAgg);
		TupTriangleStrip tupTriangleStrip(pTriangleStripAgg);
		int *pFaceIndices;
		int *pFacePointIndices;
		int numFaceIndices;
		int numFacePointIndices;
		tupTriangleStrip.GetFaceIndices(&pFaceIndices,numFaceIndices);
		assert(numFaceIndices);
		tupTriangleStrip.GetFacePointIndices(&pFacePointIndices,numFacePointIndices);
		assert(numFacePointIndices);
		assert(numFaceIndices==numFacePointIndices);

		// make sure this strip is part of my SOM
		if (m_pFaceClothSOMIndices[pFaceIndices[0]]!=m_currentSOMIndex)
			continue; // don't use

		int materialIndex = m_pMaterialIndices[pFaceIndices[0]];

		// set the texture context ID
		// if we have an array of ids for the mesh
		assert(m_textureContextIDArray.GetSize());
		assert(materialIndex<m_textureContextIDArray.GetSize());
		uintOffset textureContextIndex = m_textureContextIDArray[materialIndex];

		// scan through the sub meshes and try to find a match that uses the same texture context
		int subMeshIndex;
		for (subMeshIndex=0;subMeshIndex<m_subMeshArray.GetSize();subMeshIndex++)
		{
			SubMesh &subMesh = m_subMeshArray[subMeshIndex];
			// stop when we find one
			if (subMesh.m_textureContextIndex == textureContextIndex)
				break;
		}
		// if we didn't find one then make a new submesh
		if (subMeshIndex==m_subMeshArray.GetSize())
		{
			SubMesh newSubMesh;
			newSubMesh.m_numTextureLayers = 1; // set for now
			newSubMesh.m_textureContextIndex = textureContextIndex;
			m_subMeshArray.Add(newSubMesh);
			// subMeshIndex is pointing to the one I just added
		}
		SubMesh &subMesh = m_subMeshArray[subMeshIndex];

		assert(m_UVTransformArray.GetSize());
		assert(materialIndex<m_UVTransformArray.GetSize());

		TupArray<UVTransform> &uvTransforms = m_UVTransformArray[materialIndex];
		int numLayers = uvTransforms.GetSize();
		assert(numLayers<=MAX_SOM_TEXTURES_PER_PATCH);

		// loop through the points in the tristrip
		for (int pointIndex=0;pointIndex<numFacePointIndices;pointIndex++)
		{
			int faceIndex = pFaceIndices[pointIndex];
			int facePointIndex = pFacePointIndices[pointIndex];
			int vertexIndex;

			SubMeshVertex newVertex;
			// set up the UV coordinate in all its glory
			for (int layerIndex=0;layerIndex<numLayers;layerIndex++)
			{
				UVTransform &uvTransform = uvTransforms[layerIndex];
				int mapChannel = uvTransform.m_mapChannel;
				int uvMapIndex = m_uvChannelToIndexKeeper.GetIndex(mapChannel);
				Vector3 tempVert;
				// if this channel exists in the mesh
				if (uvMapIndex!=-1)
				{
					// get the position (lookup by face and point on face)
					tempVert = m_uvMaps[uvMapIndex].m_pUVVerts[m_uvMaps[uvMapIndex].m_pUVIndices[(faceIndex<<1)+faceIndex+facePointIndex]];
					ApplyUVTransform::Apply(uvTransform,tempVert);
				}
				// if there is no vert channel then set to 0
				else
				{
					tempVert.x( 0.0f );
					tempVert.y( 0.0f ); 
				}
				newVertex.m_textureCoordArray[layerIndex].Set( tempVert.x( ),tempVert.y( ) );
			}
			// set up the node index
			vertexIndex = m_pPositionIndices[(faceIndex<<1) + faceIndex + facePointIndex];
			assert(m_pVertClothSOMIndices[vertexIndex] == m_currentSOMIndex);
			uintCount nodeIndex = m_positionNodeRemapArray[vertexIndex];
			assert(nodeIndex!=-1);
			// set the high bit if point is a back point
			if (m_pClothPointFlags[vertexIndex] & TupClothModifierContext::FLAGS_POINT_BACK)
				nodeIndex|=0x8000;
			newVertex.m_nodeIndex = nodeIndex;
			// add the vertex to the keeper
			int vertexKeeperIndex = subMesh.m_vertexKeeper.Add(newVertex);
			// add the keeper index to the tri strip array
			subMesh.m_triStripArray.Add(vertexKeeperIndex);
		}
		// add the current size of the tristrip array to the head array
		// instead of having a start and size, we have a single value that indicates the first point of the next strip
		uintCount nextStripStart = subMesh.m_triStripArray.GetSize();
		subMesh.m_triStripHeadArray.Add(nextStripStart);
	}
	// go through each sub mesh and increment the node front and back counters
	for (int subMeshIndex=0;subMeshIndex<m_subMeshArray.GetSize();subMeshIndex++)
	{
		const SubMesh &subMesh = m_subMeshArray[subMeshIndex];
		int numKeeperVerts = subMesh.m_vertexKeeper.GetSize();
		for (int vertIndex=0;vertIndex<numKeeperVerts;vertIndex++)
		{
			const SubMeshVertex &subMeshVertex = subMesh.m_vertexKeeper[vertIndex];
			uintCount nodeIndex = subMeshVertex.m_nodeIndex;
			assert(nodeIndex!=-1);
			// if a back side
			if (nodeIndex&0x8000)
			{
				nodeIndex&=0x7fff;
				assert((int)nodeIndex<m_nodeArray.GetSize());
				m_nodeArray[nodeIndex].d_numBackSide++;
			}
			else
			{
				assert((int)nodeIndex<m_nodeArray.GetSize());
				m_nodeArray[nodeIndex].d_numFrontSide++;
			}
		}
	}
	assert(m_subMeshArray.GetSize()); // make sure there is at least one submesh for the following code

	// find any nodes that were not referenced and add them to the first strips' keeper
	for (int nodeIndex=0;nodeIndex<m_nodeArray.GetSize();nodeIndex++)
	{
		SMeshSOM::SNodeSOM &node = m_nodeArray[nodeIndex];
		// if this node is not referenced
		if (node.d_numFrontSide+node.d_numBackSide == 0)
		{
			// add to keeper
			SubMeshVertex newVertex;
			newVertex.m_nodeIndex = nodeIndex;
			m_subMeshArray[0].m_vertexKeeper.Add(newVertex);
			node.d_numFrontSide++; // it is now referenced (at least by a submesh vertex array)
		}
	}
}

void ExportClothSOM::SetDragAndPositionNodes(void)
{
	Vector3 averagePosition;
	Vector3 averageDragPosition;
	averagePosition.Set(0.0f,0.0f,0.0f);
	averageDragPosition.Set(0.0f,0.0f,0.0f);
	int numDragPositions=0;
	int nodeIndex;

	for (nodeIndex=0;nodeIndex<m_nodeArray.GetSize();nodeIndex++)
	{
		SMeshSOM::SNodeSOM &node = m_nodeArray[nodeIndex];
		averagePosition += node.d_baseVertexInModel;
		if ((node.d_flag & NODE_ATTACHED) == 0)
		{
			averageDragPosition += node.d_baseVertexInModel;
			numDragPositions++;
		}
	}
	averagePosition/=(float)m_nodeArray.GetSize();
	if (numDragPositions)
		averageDragPosition/=(float)numDragPositions;

	float bestPositionDistance;
	int bestPositionNode=-1;
	float bestDragPositionDistance;
	int bestDragPositionNode=-1;

	for (nodeIndex=0;nodeIndex<m_nodeArray.GetSize();nodeIndex++)
	{
		SMeshSOM::SNodeSOM &node = m_nodeArray[nodeIndex];
		float distance;
		distance = (node.d_baseVertexInModel-averagePosition).Length();
		if (bestPositionNode==-1 || bestPositionDistance>distance)
		{
			bestPositionNode = nodeIndex;
			bestPositionDistance = distance;
		}
		if ((node.d_flag & NODE_ATTACHED) == 0)
		{
			distance = (node.d_baseVertexInModel-averageDragPosition).Length();
			if (bestDragPositionNode==-1 || bestDragPositionDistance>distance)
			{
				bestDragPositionNode = nodeIndex;
				bestDragPositionDistance = distance;
			}
		}
	}
	if (bestDragPositionNode==-1)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"Warning - Cloth does not have any non attached points");
		bestDragPositionNode=0;
	}
	m_dragNode=bestDragPositionNode;
	m_positionNode=bestPositionNode;
}
