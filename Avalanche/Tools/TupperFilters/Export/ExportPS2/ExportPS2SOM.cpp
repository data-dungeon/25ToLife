////////////////////////////////////////////////////////////////////////////
//
// ExportDMASOM
//
// Export Simple Ordinary Meshes into the DBL file (for ps2)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportDMASOM.cpp $
 * 
 * *****************  Version 31  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:34a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 30  *****************
 * User: Dwain Skinner Date: 4/25/03    Time: 4:10p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 29  *****************
 * User: Adam Clayton Date: 4/25/03    Time: 1:18p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * get right of | for maya name
 * 
 * *****************  Version 28  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:25p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * new bounding
 * 
 * *****************  Version 27  *****************
 * User: Dwain Skinner Date: 4/04/03    Time: 11:22a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 26  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 12:17p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 25  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 11:48a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * update for streaming system
 * 
 * *****************  Version 24  *****************
 * User: Adam Clayton Date: 3/07/03    Time: 12:24p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * mesh names come from maya
 * 
 * *****************  Version 23  *****************
 * User: Joe Barnes   Date: 2/26/03    Time: 3:22p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 22  *****************
 * User: Joe Barnes   Date: 2/25/03    Time: 5:23p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 21  *****************
 * User: Joe Barnes   Date: 2/10/03    Time: 1:14p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * jlb: changes for new PS2 renderer
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 2/07/03    Time: 2:59p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * environment mapping
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 1/27/03    Time: 10:46a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * alpha sort
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 1/24/03    Time: 5:41p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * alpha sort flag
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 9/17/02    Time: 2:26p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 9/17/02    Time: 11:27a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * multi-texturing pass 1 complete
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 8/21/02    Time: 4:17p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * DBL Debug Code
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 8/06/02    Time: 2:30p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * dma size warning
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 2:17p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * use passed bone builder
 * 
 * *****************  Version 12  *****************
 * User: Nmartin      Date: 7/05/02    Time: 12:01p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * move som.h to game\database
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 7/03/02    Time: 4:01p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * normal - lighting issues
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 7/03/02    Time: 3:44p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * force realtime lighting
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 7/01/02    Time: 3:48p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * updated some flags
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 6/03/02    Time: 4:29p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * First pass of bones for ps2
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 5/13/02    Time: 1:07p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * Set # of tristrips in som
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 5/13/02    Time: 10:01a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 5/10/02    Time: 2:10p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * PS2_EXPORT define changed to EXPORT_PS2 
 * LIghting checked on meshes
 * can handle node with no material
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 5/07/02    Time: 2:39p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * Has alpha flag for texture contexts set
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/07/02    Time: 11:40a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * Working (have yet to test the data outputted)
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/06/02    Time: 4:30p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * DMA Som Code put in
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 5/06/02    Time: 2:56p
 * Created in $/Avalanche/tools/TupperFilters/ExportPS2
 */
 
#include "ExportPch.h"
#include "ExportPS2SOM.h"
#include "MeshProtocol.h"
#include "ExportBones.h"

#include <Game/Database/SOM.h>
#include <Game/Database/DBFile.h>
#include <Game/Database/World.h>
#include <Game/Database/Instance.h>
#include "Packetizer/ByteAligner.h"
#include "LocalMemManager.h"
#include "Packetizer/DMAObjectFactory.h"

#include <Game/Animation/Motion.h>
#include <Game/Animation/Bone.h>
#include "Display/PS2/Renderer/Common/VUDataHeaders.h" // DataHeader definition


ExportDMASOM::ExportDMASOM(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,
	TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool,int currentPartition) :
	m_pDBLFileName(pDBLFileName), m_options(options), m_dataManager(dataManager), 
	m_tupNodePool(tupNodePool),m_tupObjectPool(tupObjectPool),m_tupMaterialPool(tupMaterialPool),m_tupMapPool(tupMapPool),m_tupImagePool(tupImagePool),
	m_pBoneBuilder(NULL), m_bOutputBones(false),m_currentPartition(currentPartition)
{
}

void ExportDMASOM::StartChunk(int numMeshes,int numBoneMeshes)
{
	unsigned short u16ChunkType;
	unsigned short u16ChunkVersion;

	/* start chunk */

	u16ChunkType = DB_DMASOMARRAY;
	u16ChunkVersion = DB_DMASOMARRAY_VERSION;

	DBLUtil::StartChunk(u16ChunkType, u16ChunkVersion, DBL_CHUNK_NO_IGNORE, "1000");

	/* start record list-- writes out dummy data, to be replaced later */
	DBLUtil::StartRecordList(numMeshes, RECORD_LIST_HAS_OFFSET_TABLE, 0);

	/* alignment */
	DBLUtil::AlignFile();
}

/* write out SOM model offset table after all SOM models have been output */
u32 ExportDMASOM::FinishChunk(void)
{
	/* finish record list */
	DBLUtil::FinishRecordList();
	return DBLUtil::FinishChunk(FALSE);
}

void ExportDMASOM::WriteMesh(TupTriMesh &tupTriMesh,BoneBuilder &boneBuilder)
{
	m_pBoneBuilder = &boneBuilder;
	MeshProtocol			Cvt(tupTriMesh,*m_pBoneBuilder,m_tupObjectPool,m_tupMaterialPool,m_tupMapPool,m_tupImagePool,m_dataManager,m_currentPartition);
	MemBlock					**pMemBlockObject;
	LocalMemManager		LocalMem;
	DMAObjectFactory		Factory;
	unsigned int			uiPadSize;
	bool						bLOD=false;
	u32						u32MemBlockSize;
	u32						u32numMemBlocks;
	u32						u32MatrixMemBlockSize=0;
	Position3<float>		Trans, Scale;

	FILE *pDBLFile = DBLUtil::GetDBLFile();
	char cPad = (char)0xDD;

	DBLUtil::RecordListEntry();

	// reserve top 32 lines of vumem..
	Factory.setVUMemUsage( 1024 - 32);

	ts_PatchSOM tmpSOM;
	memset(&tmpSOM, 0, sizeof(ts_PatchSOM));

	unsigned long ulBoneSize = 0;
	bool hasRealBones = false;
	bool isLOD = false;

	// set the has bone flag if this mesh has bones as well as write them
	if (m_pBoneBuilder->GetNumBones())
	{
		hasRealBones = true;
	}

	bool hasNormals = false;

	TupperwareAggregate *pMeshAgg = tupTriMesh.GetBaseAggregate();

	bool bALit = pMeshAgg->FindScalarByKey("RealTimeLighting_Ambient")->GetAsInt() ? true : false;
	bool bPLit = pMeshAgg->FindScalarByKey("RealTimeLighting_Point")->GetAsInt() ? true : false;
	bool bDLit = pMeshAgg->FindScalarByKey("RealTimeLighting_Directional")->GetAsInt() ? true : false;

	// save normals if realtime lit or has bones
	if (bALit || bPLit || bDLit || hasRealBones)
	{
		hasNormals = true;
	}

	// are we handling fake bones?
	if (hasRealBones)
	{
		tmpSOM.dh.u16Flags |= SOMM_HAS_BONES;
		// if the bones were not remapped to master
		// if it is remapped the default of 0 will do
		if (!m_pBoneBuilder->RemappedToMaster())
		{
			ulBoneSize = sizeof(ts_ModelBone)*m_pBoneBuilder->GetNumBones();
		}
		else
		{
			tmpSOM.dh.u16Flags |= SOMM_BONES_SLAVE; // mark that we have bones but no bones are going to be output after the som
		}
	}
	
	// set the alpha sort type
	TupString alphaSortString = "UseTextures";
	TupperwareScalar *pAlphaSortStringScalar = pMeshAgg->FindScalarByKey("Annotate_AlphaSort");
	if (pAlphaSortStringScalar)
	{
		alphaSortString = pAlphaSortStringScalar->GetAsString();
	}
	if (!strcmp(alphaSortString,"UseTextures"))
	{
		int meshHas8BitTextureAlpha = pMeshAgg->FindScalarByKey("Annotate_MeshHasTexture8BitAlpha")->GetAsInt();
		if (meshHas8BitTextureAlpha)
			tmpSOM.dh.u16Flags |= SOMM_ALPHA_SORT;
	}
	else if (!strcmp(alphaSortString,"Enabled"))
	{
		tmpSOM.dh.u16Flags |= SOMM_ALPHA_SORT;
	}

	// TODO set the tmpSOM.dh.u16Flags:SOMM_TRI_STRIP_NORMALS if any strips are using normals due to environment mapping
	// TODO set the tmpSOM.dh.u16Flags:SOMM_NEEDS_ENV_MAPPING if any strips are using environment mapping
	
	// --- add normal flag to the somm
	if( hasNormals ) tmpSOM.dh.u16Flags |= SOMM_TRI_STRIP_NORMALS;

	//Build the actual DMA Memory block

	Factory.setMemManager(LocalMem);
	if (hasRealBones)
	{
		assert(hasNormals);
		pMemBlockObject = Factory.buildBonedDMABlockArray( Cvt, 0, ( unsigned int & )u32numMemBlocks );
	}
	else
	{
		pMemBlockObject = Factory.buildStaticDMABlockArray( Cvt, 0, ( unsigned int & )u32numMemBlocks );

		for( unsigned int i = 0; i < u32numMemBlocks; i++ )
		{
			if( pMemBlockObject[ i ]->size( ) > 0x00100000 )
			{
				TupString name("None");
				TupperwareScalar *pModelNameScalar = tupTriMesh.GetBaseAggregate()->FindScalarByKey("Annotate_ModelName");
				if (pModelNameScalar)
				{
					name = pModelNameScalar->GetAsString();
				}
				else // if modelname is not there then look to see the maya object name (saved only from Maya)
				{
					TupperwareScalar *pMayaObjectNameScalar = tupTriMesh.GetBaseAggregate()->FindScalarByKey("MayaObjectName");
					if (pMayaObjectNameScalar)
					{
						name = pMayaObjectNameScalar->GetAsString();
					}
				}
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"Warning - Model %s DMA Size exceeds 1 MB - Model May Crash PS2 Renderer",(const char *)name);
			}
		}
	}

	//Set the SOM up for writing
	tmpSOM.dh.u8DataType = DATA_PS2_DMA_SOM;
	tmpSOM.pBody=(ts_Body*)ulBoneSize;
	tmpSOM.u32VertexOffset=0;
	tmpSOM.u32PatchOffset=0;
	tmpSOM.u32TriStripOffset=0;
	tmpSOM.u32ControlVertexOffset=0;
	tmpSOM.u32NormalOffset=0;
	tmpSOM.u32UVOffset=0;

	TupperwareAggregate *pTriangleStripsAgg = tupTriMesh.GetTriangleStrips();
	assert (pTriangleStripsAgg);
	TupTriangleStripPool tupTriangleStripPool(pTriangleStripsAgg);
	tmpSOM.u32TriStripCount = tupTriangleStripPool.GetNumTriangleStrips();

	tmpSOM.BoundingBox.Min = *((Vector3 *)pMeshAgg->FindListByKey("Bounding_BoundingBoxMin")->GetAsFloat());
	tmpSOM.BoundingBox.Max = *((Vector3 *)pMeshAgg->FindListByKey("Bounding_BoundingBoxMax")->GetAsFloat());
	tmpSOM.BoundingSphere.Center = *((Vector3 *)pMeshAgg->FindListByKey("Bounding_BoundingSphereCenter")->GetAsFloat());
	tmpSOM.BoundingSphere.Radius = pMeshAgg->FindScalarByKey("Bounding_BoundingSphereRadius")->GetAsFloat();

	/* TJC - fill in the texture subset as well */
	tmpSOM.u32TextureSubSetIndex = pMeshAgg->FindScalarByKey("Annotate_TextureSet")->GetAsInt();
	tmpSOM.pTextureArray = NULL;

	/* flags */

#ifndef SHIP_IT
	/* include name for easier debugging */
	tmpSOM.dh.u16Flags |= SOMM_HAS_NAME;
	// try to see if annotate added the model name to this one
	TupperwareScalar *pModelNameScalar = tupTriMesh.GetBaseAggregate()->FindScalarByKey("Annotate_ModelName");
	if (pModelNameScalar)
	{
		strncpy(tmpSOM.Name, pModelNameScalar->GetAsString(), SOM_NAME_LENGTH);
	}
	else // if modelname is not there then look to see the maya object name (saved only from Maya)
	{
		TupperwareScalar *pMayaObjectNameScalar = tupTriMesh.GetBaseAggregate()->FindScalarByKey("MayaObjectName");
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

			strncpy(tmpSOM.Name, modelName, SOM_NAME_LENGTH);
		}
		else
		{
			// default to None if not on the mesh
			strncpy(tmpSOM.Name, "None", SOM_NAME_LENGTH);
		}
	}
	tmpSOM.Name[SOM_NAME_LENGTH - 1] = 0;	// make sure it's null-terminated
#endif //SHIP_IT

	//Save out the ts_PatchSOM header
	DBLWrite::WriteRecord(&tmpSOM, pDBLFile);

	//Save out a slot for the bone data ptr
	u32 u32BonePad = 0xdeadbeef;
	DBLWrite::WriteRecord(&u32BonePad, pDBLFile);

	//Save out the number of dma blocks we have
	DBLWrite::WriteRecord(&u32numMemBlocks, pDBLFile);

	//Save out an array of block sizes
	for(unsigned int i = 0; i < u32numMemBlocks; i++ )
	{
		u32MemBlockSize	= pMemBlockObject[ i ]->size( );

		//write out the size of the memory block
		DBLWrite::WriteRecord( &u32MemBlockSize,pDBLFile );
	}

	// --- loop through all the dma blocks and save them out
	for( i = 0; i < u32numMemBlocks; i++ )
	{
		u32MemBlockSize	= pMemBlockObject[ i ]->size( );

		//Figure out the number of bytes that need to be padded for alignment
		uiPadSize = ftell(pDBLFile) - DBLUtil::GetChunkDataStart();
		uiPadSize = 128 - (uiPadSize % 128);
		if (uiPadSize < 4)
			uiPadSize += 128;

		//Subtract sizeof(unsigned int) from padsize, and write out the value so we know how much we padded (for loading).
		uiPadSize -= sizeof(unsigned int);
		DBLWrite::WriteData(&uiPadSize,sizeof(unsigned int),pDBLFile);

		//Assuming the ts_PatchSOM is 128 byte aligned, pad for 128 byte alignment for the DMA Block
		DBL_FILEWRITE(&cPad,1,uiPadSize,pDBLFile);

		//write out DMA buffer
//		DBLWrite::WriteData(pData,u32MemBlockSize,pDBLFile);
		DBLWrite::WriteData( pMemBlockObject[ i ]->buffer( ), u32MemBlockSize, pDBLFile );
		
		
	}
	
	//save out the bones
	if (hasRealBones && (!(isLOD)) && !m_pBoneBuilder->RemappedToMaster())
	{
		int numBones = m_pBoneBuilder->GetNumBones();
		for (int boneIndex=0;boneIndex<numBones;boneIndex++)
		{
			ts_ModelBone tempBone;
			m_pBoneBuilder->FillBoneStruct(boneIndex,tempBone);
			DBLWrite::WriteRecord(&tempBone,pDBLFile);
		}
	}

	//Make sure we're still 128 byte aligned
	DBL_FILEWRITE(&cPad,1,8,pDBLFile); //pad 8 bytes to compensate for the recordlist info

	for( i = 0; i < u32numMemBlocks; i++)
		delete pMemBlockObject[i];

		//delete pDMAObject;
	free( pMemBlockObject );

	m_pBoneBuilder = NULL;
}

