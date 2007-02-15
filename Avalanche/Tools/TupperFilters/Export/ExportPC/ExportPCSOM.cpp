////////////////////////////////////////////////////////////////////////////
//
// ExportSOM
//
// Export Simple Ordinary Meshes into the DBL file
//
////////////////////////////////////////////////////////////////////////////

#include "ExportPch.h"
#include "ExportPCSOM.h"
#include "ExportBones.h"

ExportSOM::ExportSOM(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool,int currentPartition) :
	m_pDBLFileName(pDBLFileName),
	m_options(options),
	m_dataManager(dataManager),
	m_tupMaterialPool(tupMaterialPool),
	m_tupMapPool(tupMapPool),
	m_tupImagePool(tupImagePool),
	m_currentPartition(currentPartition),
	m_pBoneBuilder(NULL), 
	m_bOutputBones(false)
{
}

void ExportSOM::StartChunk(int numMeshes,int numBoneMeshes)
{
	unsigned short u16ChunkType;
	unsigned short u16ChunkVersion;

	/* start chunk */

	if (numBoneMeshes) // if there are any boned meshes in the scene
	{
		u16ChunkType = DB_SOMARRAYBONE;
		u16ChunkVersion = DB_SOMARRAYBONE_VERSION;
		m_bOutputBones = true;
	}
	else
	{
		u16ChunkType = DB_SOMARRAY;
		u16ChunkVersion = DB_SOMARRAY_VERSION;
		m_bOutputBones = false;
	}

	DBLUtil::StartChunk(u16ChunkType, u16ChunkVersion, DBL_CHUNK_NO_IGNORE, "1000");

	/* start record list-- writes out dummy data, to be replaced later */
	DBLUtil::StartRecordList(numMeshes, RECORD_LIST_HAS_OFFSET_TABLE, 0);

	/* alignment */
	DBLUtil::AlignFile();
}

/* write out SOM model offset table after all SOM models have been output */
u32 ExportSOM::FinishChunk(void)
{
	/* finish record list */
	DBLUtil::FinishRecordList();
	return DBLUtil::FinishChunk(FALSE);
}

void ExportSOM::WriteMesh(TupTriMesh &tupTriMesh,BoneBuilder &boneBuilder)
{
	// check and build the bones from the mesh
	m_pBoneBuilder = &boneBuilder;
	m_pTupTriMesh = &tupTriMesh;

	// get which material this mesh will use
	TupperwareAggregate *pObjectAgg = m_pTupTriMesh->GetBaseAggregate();
	// there should be only one material per mesh
	int materialRef = pObjectAgg->FindScalarByKey("Annotate_MaterialRef")->GetAsInt();

	// get the texture context ids for the materials used in the mesh
	m_textureContextIDArray.RemoveAll();
	m_UVTransformArray.RemoveAll();
	m_materialFlagsArray.RemoveAll();

	BuildTextureContextIDs(materialRef);
	BuildUVTransformArray(materialRef);
	BuildFlagsArray(materialRef);

	/* mark new record list entry */
	DBLUtil::RecordListEntry();
	// set up som flags
	m_modelSOMFlags = 0;

	/* start SOM model */
	StartModel();

	WriteTriStrips();

	FILE *pDBLFile = DBLUtil::GetDBLFile();
	// set the has bone flag if this mesh has bones as well as write them
	if (m_bOutputBones && m_pBoneBuilder->GetNumBones())
	{
		m_modelSOMFlags |= SOMM_HAS_BONES;
		if (!m_pBoneBuilder->RemappedToMaster())
		{
			DBLUtil::AlignChunkDataQuadWord();
			m_modelEndStripsFileOffset = ftell(pDBLFile) - m_modelHeaderFilePos;
			WriteBoneData();
		}
		else
		{
			m_modelEndStripsFileOffset = ftell(pDBLFile) - m_modelHeaderFilePos;
			m_modelSOMFlags |= SOMM_BONES_SLAVE; // use the master bones
		}
	}
	else
	{
		m_modelEndStripsFileOffset = ftell(pDBLFile) - m_modelHeaderFilePos;
	}

	// set the alpha sort type
	TupString alphaSortString = "UseTextures";
	TupperwareScalar *pAlphaSortStringScalar = pObjectAgg->FindScalarByKey("Annotate_AlphaSort");
	if (pAlphaSortStringScalar)
	{
		alphaSortString = pAlphaSortStringScalar->GetAsString();
	}
	if (!strcmp(alphaSortString,"UseTextures"))
	{
		int meshHas8BitTextureAlpha = pObjectAgg->FindScalarByKey("Annotate_MeshHasTexture8BitAlpha")->GetAsInt();
		if (meshHas8BitTextureAlpha)
			m_modelSOMFlags |= SOMM_ALPHA_SORT;
	}
	else if (!strcmp(alphaSortString,"Enabled"))
	{
		m_modelSOMFlags |= SOMM_ALPHA_SORT;
	}

	/* SOM model is done-- rewrite completed SOM header */
	FinishModel();

	m_textureContextIDArray.RemoveAll();
	m_UVTransformArray.RemoveAll();
	m_materialFlagsArray.RemoveAll();
	m_pBoneBuilder = NULL; // we don't own this one
}

void ExportSOM::StartModel(void)
{
	FILE *pDBLFile = DBLUtil::GetDBLFile();

	/* record header position in file */

	m_modelHeaderFilePos = ftell(pDBLFile);

	/* write out a dummy header */
	ts_PatchSOM PatchSOMHdr;
	DBLWrite::WriteRecord(&PatchSOMHdr, pDBLFile);

	/* alignment */
	DBLUtil::AlignFile();
}

/* write out completed header after SOM model is complete */

void ExportSOM::FinishModel(void)
{
	FILE *pDBLFile = DBLUtil::GetDBLFile();

	ts_PatchSOM PatchSOMHdr;
	memset(&PatchSOMHdr, 0, sizeof(ts_PatchSOM));

	PatchSOMHdr.dh.u8DataType = DATA_SOM;

	TupperwareAggregate *pMeshAgg = m_pTupTriMesh->GetBaseAggregate();
	PatchSOMHdr.BoundingBox.Min = *((Vector3 *)pMeshAgg->FindListByKey("Bounding_BoundingBoxMin")->GetAsFloat());
	PatchSOMHdr.BoundingBox.Max = *((Vector3 *)pMeshAgg->FindListByKey("Bounding_BoundingBoxMax")->GetAsFloat());
	PatchSOMHdr.BoundingSphere.Center = *((Vector3 *)pMeshAgg->FindListByKey("Bounding_BoundingSphereCenter")->GetAsFloat());
	PatchSOMHdr.BoundingSphere.Radius = pMeshAgg->FindScalarByKey("Bounding_BoundingSphereRadius")->GetAsFloat();

	PatchSOMHdr.u32TriStripCount = m_modelTriStripCount;
	PatchSOMHdr.u32TriStripVertexColorCount = m_modelTriStripVertexColorCount;
	PatchSOMHdr.u32TriStripOffset = m_modelTriStripFileOffset;

	PatchSOMHdr.u32UVOffset = m_modelEndStripsFileOffset; // need for GetSomEnd to work

	/* TJC - fill in the texture subset as well */
	PatchSOMHdr.u32TextureSubSetIndex = pMeshAgg->FindScalarByKey("Annotate_TextureSet")->GetAsInt();
	PatchSOMHdr.pTextureArray = NULL;

	/* flags */

	PatchSOMHdr.dh.u16Flags = m_modelSOMFlags;

#ifndef SHIP_IT
	/* include name for easier debugging */
	PatchSOMHdr.dh.u16Flags |= SOMM_HAS_NAME;
	// try to see if annotate added the model name to this one
	TupperwareScalar *pModelNameScalar = m_pTupTriMesh->GetBaseAggregate()->FindScalarByKey("Annotate_ModelName");
	if (pModelNameScalar)
	{
		strncpy(PatchSOMHdr.Name, pModelNameScalar->GetAsString(), SOM_NAME_LENGTH);
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

			strncpy(PatchSOMHdr.Name, modelName, SOM_NAME_LENGTH);
		}
		else
		{
			// default to None if not on the mesh
			strncpy(PatchSOMHdr.Name, "None", SOM_NAME_LENGTH);
		}
	}
	PatchSOMHdr.Name[SOM_NAME_LENGTH - 1] = 0;	// make sure it's null-terminated
#endif //SHIP_IT

	
	/* rewrite completed header */

	fseek(pDBLFile, m_modelHeaderFilePos, SEEK_SET);
	DBLWrite::WriteRecord(&PatchSOMHdr, pDBLFile);

	/* back to end of file */

	fseek(pDBLFile, 0, SEEK_END);
}



void ExportSOM::WriteTriStrips(void)
{
	FILE *pDBLFile = DBLUtil::GetDBLFile();

	/* build triangle strips-- currently this chews up all patches, but eventually it will just chew up patches that aren't */
	/* curved surfaces, or some other subset (e.g. all patches if it's a lower LOD model, or a model known to be far from */
	/* the camera, or something) */

	ts_TriStripHeader TriStripHeader;
	u32 u32HeaderPos = 0;

	bool bModelsOnly = false;
	bool bHasVertexColors = false;
	bool bHasNormals = false;

	const char *pValue = m_options.GetValue( "ModelsOnly");
	if( pValue && !stricmp( pValue,"true"))
	{
		bModelsOnly = true;
	}
	else
	{
		m_modelSOMFlags |= SOMM_HAS_COLORS;
		bHasVertexColors = true; // always set if doing the world
	}

	TupperwareAggregate *pMeshAgg = m_pTupTriMesh->GetBaseAggregate();
	// check environmental type if other than "None"

	bool bALit = pMeshAgg->FindScalarByKey("RealTimeLighting_Ambient")->GetAsInt() ? true : false;
	bool bPLit = pMeshAgg->FindScalarByKey("RealTimeLighting_Point")->GetAsInt() ? true : false;
	bool bDLit = pMeshAgg->FindScalarByKey("RealTimeLighting_Directional")->GetAsInt() ? true : false;

	// save normals if realtime lit or has bones
	if (bALit || bPLit || bDLit || (m_modelSOMFlags & SOMM_HAS_BONES))
	{
		bHasNormals = true;
	}

	/* nothing yet */
	m_modelTriStripCount = 0;
	m_modelTriStripVertexColorCount = 0;
	m_modelTriStripFaceCount = 0;

	/* first record where data is going to be. data should be quad-word aligned for ps2 */
	m_modelTriStripFileOffset = ftell(pDBLFile) - m_modelHeaderFilePos;

	/* record file position for calculation of data size statistic */
	m_totalStripDataSize = ftell(pDBLFile);

	/* count them first, set up for saving vertex lists (for vertex color arrangement) */
	/* point to vertex data, normal data, etc */
	/* and point to index lists */
	int numVerts, tempValue;
	Vector3 *pPositionVerts, *pNormalVerts;
	int *pPositionIndices, *pNormalIndices;
	int *pMaterialIndices,*pFaceFlags;

	TupArray<UVMap> uvMaps;
	Keeper<int> uvChannelToIndexKeeper;

	// get the face opacities
	float *pSubMaterialEnvironmentMapOpacityValues = pMeshAgg->FindListByKey("Annotate_SubMaterialEnvironmentMapOpacityList")->GetAsFloat();
	int *pSubMaterialHasEnvironmentMapValues = pMeshAgg->FindListByKey("Annotate_SubMaterialHasEnvironmentMapList")->GetAsInt();

	int numFaces;

	m_pTupTriMesh->GetFaceFlags( &pFaceFlags, numFaces );
	m_pTupTriMesh->GetPositionVerts((float **) &pPositionVerts, numVerts );
	m_pTupTriMesh->GetNormalVerts((float **) &pNormalVerts, tempValue );
	m_pTupTriMesh->GetPositionIndices( &pPositionIndices, tempValue );
	m_pTupTriMesh->GetNormalIndices( &pNormalIndices, tempValue );
	m_pTupTriMesh->GetFaceMaterialIndices(&pMaterialIndices,tempValue);

	for (int meshMapIndex = 0; meshMapIndex < m_pTupTriMesh->GetNumMeshMaps(); meshMapIndex++ )
	{
		TupTriMeshMap  tupTriMeshMap( m_pTupTriMesh->GetMeshMap( meshMapIndex ) );
		int mapChannel = tupTriMeshMap.GetMapChannel();
		if (mapChannel>=TupTriMeshMap::MAP_CHANNEL_TEXTURE_UV_START)
		{
			UVMap newUVMap;
			newUVMap.m_mapChannel = mapChannel;
			int oldSize = uvChannelToIndexKeeper.GetSize();
			int newIndex = uvChannelToIndexKeeper.Add(mapChannel); // keep track of mapChannel to array index
			if (oldSize==newIndex)
			{
				int numMapFaces;
				tupTriMeshMap.GetMapVerts((float **) &newUVMap.m_pUVVerts, newUVMap.m_uvVerts );
				tupTriMeshMap.GetMapIndices( &newUVMap.m_pUVIndices, numMapFaces);
				uvMaps.Add(newUVMap);
			}
		}
	}

	int stripID=0;
	TupperwareAggregate *pTriangleStripsAgg = m_pTupTriMesh->GetTriangleStrips();
	if (!pTriangleStripsAgg)
		throw(PluginException(ERROR_FATAL,"Cannot find TriStrips"));

	TupTriangleStripPool tupTriangleStripPool(pTriangleStripsAgg);
	int totalStrips = tupTriangleStripPool.GetNumTriangleStrips();
	if (!totalStrips)
		throw(PluginException(ERROR_FATAL,"Cannot find TriStrips"));

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

		// check if this tristrip is not a cloth strip
		if (pFaceFlags[pFaceIndices[0]]&TupTriMesh::FLAGS_FACE_USES_CLOTH)
			continue; // don't save these here

		memset(&TriStripHeader, 0, sizeof(ts_TriStripHeader));
		TriStripHeader.u32ID = stripID++;

		int materialIndex = pMaterialIndices[pFaceIndices[0]];

		if (bHasNormals || pSubMaterialHasEnvironmentMapValues[materialIndex])
		{
			TriStripHeader.u8Flags |= TRI_STRIP_HAS_NORMALS;
			m_modelSOMFlags |= SOMM_TRI_STRIP_NORMALS;
			// if using environment mapping
			if (pSubMaterialHasEnvironmentMapValues[materialIndex])
			{
				TriStripHeader.u8Flags |= TRI_STRIP_ENVIRONMENT_MAP;
				m_modelSOMFlags |= SOMM_NEEDS_ENV_MAPPING;
			}
		}

		/* count points, do some checking */
		TriStripHeader.u16PointCount = numFacePointIndices;

		/* for exporter-created data, skip-point-count is same as point count. for jeff's tire tracks, he wants to */
		/* allocate a triangle strip with room for some fixed number of vertices, but only partially fill it. so his */
		/* point count would be <= the skip-point-count. no execution penalty */
		TriStripHeader.u16SkipPointCount = TriStripHeader.u16PointCount;

		if (m_pBoneBuilder->GetNumBones())
		{
			ts_VertexWeightingBones boneIndices;
			memset(&boneIndices, 0, sizeof(ts_VertexWeightingBones));
			int numBonesInStrip = 0;
			const int *pBoneIndices = m_pBoneBuilder->GetBoneVerts().GetData();

			for (int pointIndex=0;pointIndex<numFacePointIndices;pointIndex++)
			{
				int faceIndex = pFaceIndices[pointIndex];
				int facePointIndex = pFacePointIndices[pointIndex];

				int vertexIndex = pPositionIndices[(faceIndex<<1)+faceIndex+facePointIndex];
				int boneIndex = pBoneIndices[vertexIndex];

				// build up ts_VertexWeightingBones structure 
				assert(boneIndex < 256);	// we allow a byte per bone

				// bone already added to structure? 
				for (int boneStripIndex=0;boneStripIndex<numBonesInStrip;boneStripIndex++)
				{
					// if already there?
					if (boneIndex==boneIndices.u8BoneIndex[boneStripIndex])
						break;
				}
				// if we didn't find it
				if (boneStripIndex==numBonesInStrip)
				{
					assert(numBonesInStrip<4); // only can hold 4
					boneIndices.u8BoneIndex[numBonesInStrip] = boneIndex;
					numBonesInStrip++;
				}
			}
			// if more than one bone in the strip
			if (numBonesInStrip>1)
			{
				// sort bone indices. this is so that we can match bone indices between patches that use same bones but in 
				// a different order 
				for (int boneStripIndex = 1; boneStripIndex < numBonesInStrip; boneStripIndex++)
				{
					int sortIndex = boneStripIndex;
					while ((sortIndex > 0) && (boneIndices.u8BoneIndex[sortIndex] < boneIndices.u8BoneIndex[sortIndex - 1]))
					{
						u8 u8Temp = boneIndices.u8BoneIndex[sortIndex];
						boneIndices.u8BoneIndex[sortIndex] = boneIndices.u8BoneIndex[sortIndex - 1];
						boneIndices.u8BoneIndex[sortIndex - 1] = u8Temp;
						sortIndex--;
					}
				}
				for (boneStripIndex = 0; boneStripIndex < numBonesInStrip - 1; boneStripIndex++)
				{
					assert(boneIndices.u8BoneIndex[boneStripIndex] < boneIndices.u8BoneIndex[boneStripIndex + 1]);	// double-check sort
				}

				// now copy into bone index to be used from here on in
				assert(sizeof(u32) == sizeof(ts_VertexWeightingBones));
				TriStripHeader.u32BoneIndex = *(u32 *) &boneIndices;
				assert(TriStripHeader.u32BoneIndex & VERTEX_WEIGHTING);	// make sure we can identify vertex weighting
			}
			else
			{
				// only use first bone
				TriStripHeader.u32BoneIndex = boneIndices.u8BoneIndex[0];
			}
		}
		else
		{
			TriStripHeader.u32BoneIndex = 0; // no bones
		}

		/* finish filling in header */
		TriStripHeader.u32NextOffset = 0;

		// set the texture context ID
		// if we have an array of ids for the mesh
		assert(m_textureContextIDArray.GetSize());
		assert(materialIndex<m_textureContextIDArray.GetSize());
		TriStripHeader.u32TextureContextIndex = m_textureContextIDArray[materialIndex];

		// deal with the material flags
		assert(m_materialFlagsArray.GetSize());
		assert(materialIndex<m_materialFlagsArray.GetSize());
		int materialFlags = m_materialFlagsArray[materialIndex];
		// don't do much with this right now

		if (pFaceFlags[pFaceIndices[0]] & TupTriMesh::FLAGS_FACE_TWO_SIDED)
		{
			TriStripHeader.u8Flags |= TRI_STRIP_TWO_SIDED;
		}

		if (pFaceFlags[pFaceIndices[0]] & TupTriMesh::FLAGS_FACE_NO_Z_WRITE)
		{
//			TriStripHeader.u8Flags |= TRI_STRIP_Z_WRITE_OFF;
//			m_modelSOMFlags |= SOMM_MIXED_ZWRITE;
		}

		// set strip opacities
		TriStripHeader.u8Opacity = (u8)(pSubMaterialEnvironmentMapOpacityValues[materialIndex]*100.0f);

		assert(m_UVTransformArray.GetSize());
		assert(materialIndex<m_UVTransformArray.GetSize());

		/* remember location, output header */
		u32HeaderPos = ftell(pDBLFile);

		DBLWrite::WriteRecord(&TriStripHeader, pDBLFile);

		Vector4 PS2Vertex;

		// output vertex positions
		// if the strip is using more than 1 bone
		if (TriStripHeader.u32BoneIndex & VERTEX_WEIGHTING)
		{
			ts_VertexWeightingBones &boneArray = *((ts_VertexWeightingBones *)&TriStripHeader.u32BoneIndex);
			const int *pBoneIndices = m_pBoneBuilder->GetBoneVerts().GetData();
			for (int pointIndex=0;pointIndex<numFacePointIndices;pointIndex++)
			{
				int faceIndex = pFaceIndices[pointIndex];
				int facePointIndex = pFacePointIndices[pointIndex];

				int vertexIndex = pPositionIndices[(faceIndex<<1)+faceIndex+facePointIndex];
				int boneIndex = pBoneIndices[vertexIndex];

				memcpy(&PS2Vertex, &pPositionVerts[vertexIndex], sizeof(Vector3));
				// if more than one bone used in the strip then add the bone used here
				for (int i=0; i < 4; i++)
				{
					if (boneIndex == boneArray.u8BoneIndex[i])
						break;
				}
				assert(i < 4);	// must find it
				PS2Vertex.w(*((float *)&i));
				DBLWrite::WriteRecord(&PS2Vertex, pDBLFile);
			}
		}
		else
		{
			for (int pointIndex=0;pointIndex<numFacePointIndices;pointIndex++)
			{
				int faceIndex = pFaceIndices[pointIndex];
				int facePointIndex = pFacePointIndices[pointIndex];

				int vertexIndex = pPositionIndices[(faceIndex<<1)+faceIndex+facePointIndex];

				memcpy(&PS2Vertex, &pPositionVerts[vertexIndex], sizeof(Vector3));
				PS2Vertex.w( 0.0f );
				DBLWrite::WriteRecord(&PS2Vertex, pDBLFile);
			}
		}

		// output normals
		if (bHasNormals || pSubMaterialHasEnvironmentMapValues[materialIndex])	// either due to lighting or environment mapping
		{
			for (int pointIndex=0;pointIndex<numFacePointIndices;pointIndex++)
			{
				int faceIndex = pFaceIndices[pointIndex];
				int facePointIndex = pFacePointIndices[pointIndex];

				int vertexIndex = pNormalIndices[(faceIndex<<1)+faceIndex+facePointIndex];

				memcpy(&PS2Vertex, &pNormalVerts[vertexIndex], sizeof(Vector3));
				PS2Vertex.w( 0.0f );
				DBLWrite::WriteRecord(&PS2Vertex, pDBLFile);
			}
		}

		// output texture coordinates
		TupArray<UVTransform> &uvTransforms = m_UVTransformArray[materialIndex];
		int numLayers = uvTransforms.GetSize();
		for (int layerIndex=0;layerIndex<numLayers;layerIndex++)
		{
			UVTransform &uvTransform = uvTransforms[layerIndex];
			int mapChannel = uvTransform.m_mapChannel;
			int uvMapIndex = uvChannelToIndexKeeper.GetIndex(mapChannel);
			for (int pointIndex=0;pointIndex<numFacePointIndices;pointIndex++)
			{
				int faceIndex = pFaceIndices[pointIndex];
				int facePointIndex = pFacePointIndices[pointIndex];
				Vector3 tempVert;
				if (uvMapIndex!=-1)
				{
					// get the position (lookup by face and point on face)
					tempVert = uvMaps[uvMapIndex].m_pUVVerts[uvMaps[uvMapIndex].m_pUVIndices[(faceIndex<<1)+faceIndex+facePointIndex]];
					ApplyUVTransform::Apply(uvTransform,tempVert);
				}
				// if there is no vert channel then set to 0
				else
				{
					tempVert.x( 0.0f );
					tempVert.y( 0.0f ); 
				}
				DBLWrite::WriteRecord((Vector2 *)&tempVert, pDBLFile);
			}
			if (TriStripHeader.u16PointCount & 1)
			{
				unsigned long Pad[2] = {0xeeeeeeee, 0xeeeeeeee};
				DBLWrite::WriteData(&Pad, sizeof(Vector2), pDBLFile);
			}
		}

		/* if not the last strip, update offset in header */
		TriStripHeader.u32NextOffset = ftell(pDBLFile) - u32HeaderPos;
		fseek(pDBLFile, u32HeaderPos, SEEK_SET);
		DBLWrite::WriteRecord(&TriStripHeader, pDBLFile);
		fseek(pDBLFile, 0, SEEK_END);	// back to end of file

		m_modelTriStripPointCount+=numFacePointIndices;
		m_modelTriStripFaceCount+=numFacePointIndices-2;
		m_modelTriStripCount++;
		m_modelTriStripVertexColorCount+=numFacePointIndices;
	}
	// put a 0 in the next offset for the last strip
	if (u32HeaderPos)
	{
		fseek(pDBLFile, u32HeaderPos + offsetof(ts_TriStripHeader, u32NextOffset), SEEK_SET);
		u32 clearOffset = 0;
		DBLWrite::WriteRecord(&clearOffset, pDBLFile);
		fseek(pDBLFile, 0, SEEK_END);	// back to end of file
	}

	/* alignment */

	DBLUtil::AlignFile();

	/* finish data size statistic */
	m_totalStripDataSize = ftell(pDBLFile) - m_totalStripDataSize;
}

// this creates an array of ints and fills it with the proper texture context ids for the mesh
void ExportSOM::BuildTextureContextIDs(int materialReference)
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
								int textureID = pTextureContextIDs[m_currentPartition];
								assert(textureID!=-1);
								m_textureContextIDArray[materialIndex] = textureID;
							}
							else
							{
								if (m_currentPartition==0)
									m_textureContextIDArray[materialIndex] = 0; // use the blank one if not set (this slot should not be used)
								else
									m_textureContextIDArray[materialIndex] = 0xff000000; // use the blank one from world partition
							}
						}
						else
						{
							if (m_currentPartition==0)
								m_textureContextIDArray[materialIndex] = 0; // use the blank one if not set (this slot should not be used)
							else
								m_textureContextIDArray[materialIndex] = 0xff000000; // use the blank one from world partition
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
						if (m_currentPartition==0)
							m_textureContextIDArray[materialIndex] = 0; // use the blank one if not set (this slot should not be used)
						else
							m_textureContextIDArray[materialIndex] = 0xff000000; // use the blank one from world partition
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
				int textureID = pTextureContextIDs[m_currentPartition];
				assert(textureID!=-1);
				m_textureContextIDArray[0] = textureID;
			}
			else
			{
				if (m_currentPartition==0)
					m_textureContextIDArray[0] = 0; // use the blank one if not set (this slot should not be used)
				else
					m_textureContextIDArray[0] = 0xff000000; // use the blank one from world partition
			}
		}
		else // any other materials
		{
			// just deal with 1 material
			m_textureContextIDArray.SetSize(1);
			if (m_currentPartition==0)
				m_textureContextIDArray[0] = 0; // use the blank one if not set (this slot should not be used)
			else
				m_textureContextIDArray[0] = 0xff000000; // use the blank one from world partition
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
		if (m_currentPartition==0)
			m_textureContextIDArray[0] = 0; // use the blank one if not set (this slot should not be used)
		else
			m_textureContextIDArray[0] = 0xff000000; // use the blank one from world partition
//		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"Warning - Using Material Reference of -1");
	}
}

// this creates an array of ints and fills it with the proper texture context ids for the mesh
void ExportSOM::BuildUVTransformArray(int materialRef)
{
	BuildUVTransformManager buildUVTransformManager(m_tupMaterialPool,m_tupMapPool,m_tupImagePool,false);

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


void ExportSOM::BuildFlagsArray(int materialRef)
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

void ExportSOM::WriteBoneData(void)
{
	FILE *pDBLFile = DBLUtil::GetDBLFile();

	int numBones = m_pBoneBuilder->GetNumBones();
	for (int boneIndex=0;boneIndex<numBones;boneIndex++)
	{
		ts_ModelBone tempBone;
		m_pBoneBuilder->FillBoneStruct(boneIndex,tempBone);
		DBLWrite::WriteRecord(&tempBone,pDBLFile);
	}
}
