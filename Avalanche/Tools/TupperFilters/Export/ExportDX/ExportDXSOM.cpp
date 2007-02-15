////////////////////////////////////////////////////////////////////////////
//
// ExportSOM
//
// Export Simple Ordinary Meshes into the DBL file
//
////////////////////////////////////////////////////////////////////////////

#include "ExportPch.h"
#include "ExportDXSOM.h"
#include "ExportBones.h"

#include <map>
#include <vector>
#include <algorithm>
#include <iterator>

#include <d3d9types.h>

#include "DX\FileRecords.h"
#include "DX\FVFMatrixCache.h"
#include "DX\FVFVBBuilder.h"
#include "DX\FVFVertexBuilder.h"
#include "DX\FVFVertexStream.h"
#include "DX\FVFVertexStream_TC.h"
#include "DX\StreamDescriptors.h"
#include "DX\TriStripState.h"

#include "DX\XBOX.h"

#include <stdio.h>

//#define WRITE_DEBUG_FILE

unsigned int			modelno = 0;

#ifdef WRITE_DEBUG_FILE
	char						modelbinname[128];
	char						modeltxtname[128];
#endif

ExportSOM::ExportSOM(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool,TupObjectPool &tupObjectPool,int currentPartition) :
	m_pDBLFileName(pDBLFileName),
	m_options(options),
	m_dataManager(dataManager),
	m_tupMaterialPool(tupMaterialPool),
	m_tupMapPool(tupMapPool),
	m_tupImagePool(tupImagePool),
	m_tupObjectPool(tupObjectPool),
	m_currentPartition(currentPartition),
	m_pBoneBuilder(NULL), 
	m_bOutputBones(false),
	m_sizeMatrixCache(16)
{
	const char * pPlatform = m_options.GetValue( "Platform" );

	if( pPlatform  &&  (stricmp( pPlatform, "xbox" ) == 0) )
		m_Xbox = true;
	else
		m_Xbox = false;

	if( m_Xbox )
	{
		unsigned int dummyCount = 0;
		char XVDFileName[256] = { 0 };

		const char *				pDBLFileName = DBLUtil::GetDBLFileName( );
		assert( pDBLFileName );
		assert( strlen( pDBLFileName ) < 255 );

		strcpy( XVDFileName, pDBLFileName );

		char *						pXVDFileEnd = XVDFileName + strlen( XVDFileName );

		while( (pXVDFileEnd != XVDFileName)  &&  *pXVDFileEnd != '.' )
			pXVDFileEnd--;

		if( pXVDFileEnd != XVDFileName )
			*pXVDFileEnd = '\0';

		m_XPRBundler.setXPRExt( ".xvd" );
		m_XPRBundler.setHDRExt( ".xvh" );
		m_XPRBundler.startBundle( XVDFileName );
		
	}

}

ExportSOM::~ExportSOM( )
{
	if( m_Xbox )
		m_XPRBundler.finalizeBundle( );
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

void ExportSOM::WriteMeshArray( TupTriMesh &tupTriMesh, BoneBuilder &boneBuilder, unsigned int defMeshOptions)
{
	// check and build the bones from the mesh
	m_pBoneBuilder = &boneBuilder;
	m_pTupTriMesh = &tupTriMesh;

	m_sizeMatrixCache = 16;

	// get which material this mesh will use
	TupperwareAggregate *pObjectAgg = m_pTupTriMesh->GetBaseAggregate( );
	// there should be only one material per mesh
	int materialRef = pObjectAgg->FindScalarByKey( "Annotate_MaterialRef" )->GetAsInt( );

	// get the texture context ids for the materials used in the mesh
	m_textureContextIDArray.RemoveAll( );
	m_UVTransformArray.RemoveAll( );
	m_materialFlagsArray.RemoveAll( );
	m_modelOffsetsArray.RemoveAll( );

	BuildTextureContextIDs( materialRef );
	BuildUVTransformArray( materialRef );
	BuildFlagsArray( materialRef );

	/* mark new record list entry */
	DBLUtil::RecordListEntry( );

	unsigned int				numColorSets = 0;

	FILE *						pDBLFile = DBLUtil::GetDBLFile( );

	TupperwareAggregate *	pMeshAgg = tupTriMesh.GetBaseAggregate( );

	int *							pColorRefData = NULL;
	int *							pAlphaRefData = NULL;

	TupperwareList *			pColorRefList = pMeshAgg->FindListByKey( "Annotate_VertexColorRefList" );
	TupperwareList *			pAlphaRefList = pMeshAgg->FindListByKey( "Annotate_VertexAlphaRefList" );

	if( pColorRefList  &&  pAlphaRefList )
	{
		pColorRefData = pColorRefList->GetAsInt( );
		pAlphaRefData = pAlphaRefList->GetAsInt( );

		numColorSets = pColorRefList->GetLength( );

		if( numColorSets == 1  &&  pColorRefData[0] == -1  &&  pAlphaRefData[0] == -1 )
			numColorSets = 0;
	}

	// set up som flags
	m_modelSOMFlags = 0;

	/* start SOM model */
	StartModel( numColorSets == 0 ? 1 : numColorSets );

	modelno++;

#ifdef WRITE_DEBUG_FILE
	sprintf( modelbinname, "\\dxdebug%02d.xbm", modelno );
	sprintf( modeltxtname, "\\dxdebug%02d.txt", modelno );
	FILE *pFile = fopen( modelbinname, "wb" );
	if( pFile )
	{
		unsigned int numModels = numColorSets ? numColorSets : 1;
		fwrite( & numModels, sizeof( unsigned int ), 1, pFile );
		fclose( pFile );
	}
#endif

	if( numColorSets == 0)
	{
		m_modelOffsetsArray.Add( ftell( pDBLFile ) );
		WriteTriStrips( NULL, NULL, defMeshOptions );
	}
	else
	{
		for( unsigned int i = 0; i < numColorSets; i++)
		{
			int						colorObjectRef = pColorRefData[i];
			int						alphaObjectRef = pAlphaRefData[i];

			TupTriMeshMap *		pColorMap;
			TupTriMeshMap *		pAlphaMap;

			if( colorObjectRef != -1 )
			{
				TupperwareAggregate *	pChildMeshAgg = AnnotateHelper::FindChildVertexColorMeshMap( m_tupObjectPool, colorObjectRef );
				assert( pChildMeshAgg );

				pColorMap = new TupTriMeshMap( pChildMeshAgg );
			}
			else
				pColorMap = 0;

			if( alphaObjectRef != -1 )
			{
				TupperwareAggregate *	pChildMeshAgg = AnnotateHelper::FindChildVertexAlphaMeshMap( m_tupObjectPool, alphaObjectRef );
				assert( pChildMeshAgg );

				pAlphaMap = new TupTriMeshMap( pChildMeshAgg );
			}
			else
				pAlphaMap = 0;

			m_modelOffsetsArray.Add( ftell( pDBLFile ) );
			WriteTriStrips( pColorMap, pAlphaMap, defMeshOptions );

			if( pColorMap )
				delete pColorMap;
			if( pAlphaMap )
				delete pAlphaMap;
		}
	}

	// set the has bone flag if this mesh has bones as well as write them
	if (m_bOutputBones && m_pBoneBuilder->GetNumBones( ) )
	{
		m_modelSOMFlags |= SOMM_HAS_BONES;
		DBLUtil::AlignChunkDataQuadWord( );
		m_modelEndStripsFileOffset = ftell(pDBLFile) - m_modelHeaderFilePos;
		WriteBoneData( );
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
	FinishModel( );

	m_textureContextIDArray.RemoveAll( );
	m_UVTransformArray.RemoveAll( );
	m_materialFlagsArray.RemoveAll( );
	m_pBoneBuilder = NULL; // we don't own this one
}

void ExportSOM::StartModel(int lengthModelArray)
{
	assert( lengthModelArray > 0 );

	FILE *pDBLFile = DBLUtil::GetDBLFile( );


	/* record header position in file */
	m_modelHeaderFilePos = ftell( pDBLFile);

	/* write out a dummy header */
	ts_PatchSOM PatchSOMHdr;
	DBLWrite::WriteRecord( &PatchSOMHdr, pDBLFile );

	/* write model offsets count */
	DBLWrite::WriteRecord( &lengthModelArray, pDBLFile );

	/* write model offsets */
	int				temp = 0;

	for( int i = 0; i < lengthModelArray; i++)
		DBLWrite::WriteRecord( &temp, pDBLFile);

	m_lengthCurrentModelArray = lengthModelArray;

	/* alignment */
	DBLUtil::AlignFile( );

	/* nothing yet */
	m_modelTriStripCount = 0;
	m_modelTriStripVertexColorCount = 0;
	m_modelTriStripFaceCount = 0;

	/* first record where data is going to be. data should be quad-word aligned for ps2 */
	m_modelTriStripFileOffset = ftell( pDBLFile ) - m_modelHeaderFilePos;

	/* record file position for calculation of data size statistic */
	m_totalStripDataSize = ftell( pDBLFile );


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

	assert( m_lengthCurrentModelArray > 0 );
	assert( m_lengthCurrentModelArray == m_modelOffsetsArray.GetSize( ) );

	DBLWrite::WriteRecord( &m_lengthCurrentModelArray, pDBLFile );

	unsigned int modelArrayOffset = ftell( pDBLFile );

	for( int i = 0; i < m_modelOffsetsArray.GetSize( ); i++)
	{
		int	offset = m_modelOffsetsArray[i] - modelArrayOffset;

		DBLWrite::WriteRecord( &offset, pDBLFile );
	}

	/* back to end of file */

	fseek(pDBLFile, 0, SEEK_END);
}

std::vector< SingleStreamDescriptor >::iterator addUniqueSSDesc( std::vector< SingleStreamDescriptor > & array, const SingleStreamDescriptor & item)
{
	std::vector< SingleStreamDescriptor >::iterator entry;

	entry = std::find( array.begin( ), array.end( ), item );

	if( entry == array.end( ) )
	{
		array.push_back( item );
		entry = std::find( array.begin( ), array.end( ), item );
	}

	return entry;
}

unsigned int addUniqueShader( std::vector< ShaderDescriptor > & array, const ShaderDescriptor & item)
{
	std::vector< ShaderDescriptor >::iterator entry;

	entry = std::find( array.begin( ), array.end( ), item );

	if( entry == array.end( ) )
	{
		array.push_back( item );
		entry = std::find( array.begin( ), array.end( ), item );
	}

	return std::distance( array.begin( ), entry );
}

std::vector< FVFVBBuilder >::iterator addUniqueVB( std::vector< FVFVBBuilder > & array, const FVFVBBuilder & item )
{
	std::vector< FVFVBBuilder >::iterator entry;

	for( entry = array.begin( ); entry != array.end( ); ++entry)
	{
		if( stride( item.d_Desc ) == stride( (*entry).d_Desc )  &&  !(*entry).full( ) )
		{
			(*entry).changeDescriptor( item.d_Desc );
			return entry;
		}
	}

	array.push_back( item );

	entry = std::find( array.begin( ), array.end( ), item );

	return entry;
}

unsigned int dbgIdx, dbgSlot;
unsigned int dbgStripIndex, dbgMLIndex;

void ExportSOM::WriteTriStrips( TupTriMeshMap *pTupTriColorMeshMap, TupTriMeshMap *pTupTriAlphaMeshMap, unsigned int defMeshOptions )
{
	D3DVERTEXELEMENT9		Decl9End =				D3DDECL_END( );

#ifdef WRITE_DEBUG_FILE
	FILE *pDBLFile = fopen( modelbinname, "r+b" );
	fseek( pDBLFile, 0, SEEK_END );
#else
	FILE *pDBLFile = DBLUtil::GetDBLFile( );
#endif
	/* build triangle strips-- currently this chews up all patches, but eventually it will just chew up patches that aren't */
	/* curved surfaces, or some other subset (e.g. all patches if it's a lower LOD model, or a model known to be far from */
	/* the camera, or something) */

	long				u32CurrPos = 0;
	long				u32HeaderPos = 0;

	/* count them first, set up for saving vertex lists (for vertex color arrangement) */
	/* point to vertex data, normal data, etc */
	/* and point to index lists */
	int 				numFaces;
	int				numVerts, numIndices;
	int				numNormals, numNormalIndices;
	int				numColors, numColorIndices;
	int				numAlphas, numAlphaIndices;
	int				numMaterialIndices;

	int *				pPositionIndices;
	int *				pNormalIndices;
	int *				pMaterialIndices;
	int *				pColorIndices;
	int *				pAlphaIndices;
	int *				pFaceFlags;

	const int *		pBoneIndices;

	Vector3 *		pPositionVerts;
	Vector3 *		pNormalVerts;
	Vector3 *		pColorVerts;
	Vector3 *		pAlphaVerts;

	TupArray<UVMap> uvMaps;
	Keeper<int> uvChannelToIndexKeeper;

	FVFVertexStream_Position *			pStreamVertices = 0;
	FVFVertexStream_PositionW *		pStreamBonedVertices = 0;
	FVFVertexStream *						pStreamNormals = 0;
	FVFVertexStream_RGBA	 *				pStreamColors = 0;

	FVFVertexStream_PadTC2				StreamDummyTC2( Vector2( 0.0, 0.0) );
	FVFVertexStream_PadRGBA				StreamDummyRGBA( 255, 255, 255, 255 );

	FVFMatrixCache							MCache( m_sizeMatrixCache );

	std::vector< FVFVertexStream_TC * >	TCArray;

	TupperwareAggregate * pMeshAgg = m_pTupTriMesh->GetBaseAggregate( );

	// get the face opacities
	float *			pSubMaterialEnvironmentMapOpacityValues = pMeshAgg->FindListByKey( "Annotate_SubMaterialEnvironmentMapOpacityList")->GetAsFloat( );
	int *				pSubMaterialHasEnvironmentMapValues = pMeshAgg->FindListByKey("Annotate_SubMaterialHasEnvironmentMapList")->GetAsInt( );

	m_pTupTriMesh->GetFaceFlags( &pFaceFlags, numFaces );
	assert( pFaceFlags );

	// setup vertex based streams
	m_pTupTriMesh->GetPositionVerts((float **) &pPositionVerts, numVerts );
	m_pTupTriMesh->GetPositionIndices( &pPositionIndices, numIndices );
	assert( pPositionVerts );
	assert( pPositionIndices );

	pStreamVertices = new FVFVertexStream_Position( pPositionVerts, numVerts, pPositionIndices, numIndices * 3);

	int maxBone = -1;

	if( m_pBoneBuilder  &&  m_pBoneBuilder->GetNumBones( ) )
	{
		pBoneIndices = m_pBoneBuilder->GetBoneVerts( ).GetData( );
		assert( pBoneIndices );

		unsigned int numBones = m_pBoneBuilder->GetNumBones( );
		const TupArray<int> & verts = m_pBoneBuilder->GetBoneVerts( );
		unsigned int count = verts.GetSize( );

		for( unsigned int i = 0; i < count; i++ )
			if( pBoneIndices[i] > maxBone )
				maxBone = pBoneIndices[i];

		pStreamBonedVertices = new FVFVertexStream_PositionW( pPositionVerts, numVerts, pPositionIndices, numIndices * 3, (int *) pBoneIndices, MCache );
	}
	else
		pBoneIndices = 0;

	// setup normal based streams
	m_pTupTriMesh->GetNormalVerts( (float **) &pNormalVerts, numNormals );
	m_pTupTriMesh->GetNormalIndices( &pNormalIndices, numNormalIndices );

	if( pNormalVerts  &&  pNormalIndices )
		pStreamNormals = new FVFVertexStream_Normal( pNormalVerts, numNormals, pNormalIndices, numNormalIndices * 3);

	// setup color based streams
	if( pTupTriColorMeshMap)
	{
		pTupTriColorMeshMap->GetMapVerts( (float **) &pColorVerts, numColors );
		pTupTriColorMeshMap->GetMapIndices( &pColorIndices, numColorIndices );
	}
	else
	{
		numColors = 0;
		numColorIndices = 0;

		pColorVerts = 0;
		pColorIndices = 0;
	}

	if( pTupTriAlphaMeshMap)
	{
		pTupTriAlphaMeshMap->GetMapVerts( (float **) &pAlphaVerts, numAlphas );
		pTupTriAlphaMeshMap->GetMapIndices( &pAlphaIndices, numAlphaIndices );
	}
	else
	{
		numAlphas = 0;
		numAlphaIndices = 0;

		pAlphaVerts = 0;
		pAlphaIndices = 0;
	}

	if( numColors  ||  numAlphas )
		pStreamColors = new FVFVertexStream_RGBA( pColorVerts, numColors, pColorIndices, numColorIndices * 3, pAlphaVerts, numAlphas, pAlphaIndices, numAlphaIndices * 3 );

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

	m_pTupTriMesh->GetFaceMaterialIndices( &pMaterialIndices, numMaterialIndices );

	int 				stripID = 0;
	int				stripIndex;

	TupperwareAggregate * pTriangleStripsAgg = m_pTupTriMesh->GetTriangleStrips( );
	if( !pTriangleStripsAgg)
		throw( PluginException( ERROR_FATAL,"Cannot find TriStrips") );

	TupTriangleStripPool tupTriangleStripPool( pTriangleStripsAgg);
	int totalStrips = tupTriangleStripPool.GetNumTriangleStrips( );
	if( !totalStrips)
		throw( PluginException( ERROR_FATAL,"Cannot find TriStrips") );

	std::vector< std::pair< TriStripState, std::vector<int> > > triStripArray;
	
	std::pair< TriStripState, std::vector<int> >	currentState;
	
	for( stripIndex = 0; stripIndex < totalStrips; stripIndex++)
	{	
		TriStripState	 workState;

		TupperwareAggregate *pTriangleStripAgg = tupTriangleStripPool.GetTriangleStrip(stripIndex);
		assert(pTriangleStripAgg);
		TupTriangleStrip tupTriangleStrip(pTriangleStripAgg);

		int *				pFaceIndices;
		int 				numFaceIndices;

		int *				pFacePointIndices;
		int 				numFacePointIndices;

		tupTriangleStrip.GetFaceIndices( &pFaceIndices,numFaceIndices);
		assert( numFaceIndices);

		tupTriangleStrip.GetFacePointIndices( &pFacePointIndices,numFacePointIndices);
		assert( numFacePointIndices);
		assert( numFaceIndices == numFacePointIndices);

		// check if this tristrip is not a cloth strip
		// don't save these here
		if( pFaceFlags[pFaceIndices[0]] & TupTriMesh::FLAGS_FACE_USES_CLOTH)
			continue; 

		unsigned int materialIndex = pMaterialIndices[pFaceIndices[0]];

		memset( &workState, 0, sizeof( workState) );

		// set the texture context ID
		// if we have an array of ids for the mesh
		assert( m_textureContextIDArray.GetSize( ) );
		assert( m_UVTransformArray.GetSize( ) );

		assert( (int) materialIndex < m_UVTransformArray.GetSize( ) );
		assert( (int) materialIndex < m_textureContextIDArray.GetSize( ) );

		workState.d_TextureContextIndex = m_textureContextIDArray[materialIndex];

		unsigned int currentStreamOffset = 0;

		if( m_pBoneBuilder  &&  m_pBoneBuilder->GetNumBones( ) )
		{
			assert( pStreamBonedVertices );

			workState.d_ShaderFlags |= ShaderRecord::SHFLAG_ANIM;

			workState.d_StreamDesc.push_back( createStreamElement( StreamElement::STREAM_POSITION_W, pStreamBonedVertices) );
			workState.d_VertexDesc.push_back( createD3DVERTEXELEMENT9( 0, currentStreamOffset, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0) );
			currentStreamOffset += 4 * sizeof( float );
		}
		else
		{
			assert( pStreamVertices );
			
			workState.d_StreamDesc.push_back( createStreamElement( StreamElement::STREAM_POSITION, pStreamVertices) );
			workState.d_VertexDesc.push_back( createD3DVERTEXELEMENT9( 0, currentStreamOffset, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0) );
			currentStreamOffset += 3 * sizeof( float );
		}

		if( (defMeshOptions & MESH_WRITE_NORMALS )  ||  pSubMaterialHasEnvironmentMapValues[materialIndex] )
		{
			assert( pStreamNormals );

			workState.d_StreamDesc.push_back( createStreamElement( StreamElement::STREAM_NORMAL, pStreamNormals ) );
			workState.d_VertexDesc.push_back( createD3DVERTEXELEMENT9( 0, currentStreamOffset, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0) );
			currentStreamOffset += 3 * sizeof( float );
		}

		if( defMeshOptions & (MESH_WRITE_DIFFUSE_COLORS | defMeshOptions & MESH_NEEDS_DIFFUSE_COLORS) )
		{
			if( pStreamColors )
			{
				workState.d_StreamDesc.push_back( createStreamElement( StreamElement::STREAM_DIFFUSE, pStreamColors ) );
				workState.d_VertexDesc.push_back( createD3DVERTEXELEMENT9( 0, currentStreamOffset, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 ) );
				currentStreamOffset += sizeof( int );
			}
			else if ( defMeshOptions & MESH_NEEDS_DIFFUSE_COLORS )
			{
				workState.d_StreamDesc.push_back( createStreamElement( StreamElement::STREAM_DIFFUSE | StreamElement::STREAM_FLAG_PAD, & StreamDummyRGBA ) );
				workState.d_VertexDesc.push_back( createD3DVERTEXELEMENT9( 0, currentStreamOffset, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 ) );
				currentStreamOffset += sizeof( int );
			}
		}

		TupArray<UVTransform> &	uvTransforms = m_UVTransformArray[materialIndex];

		int 							numTCLayers = uvTransforms.GetSize( );

		if( numTCLayers )
		{
			for( int layerIndex = 0; layerIndex < numTCLayers; layerIndex++ )
			{
				UVTransform &			uvTransform = uvTransforms[layerIndex];			
				int						mapChannel = uvTransform.m_mapChannel;

				int uvMapIndex = uvChannelToIndexKeeper.GetIndex(mapChannel);
				// if this channel exists in the mesh
				if (uvMapIndex!=-1)
				{
					FVFVertexStream_TC * pNewTCs = new FVFVertexStream_TC( uvMaps[uvMapIndex].m_pUVVerts,uvMaps[uvMapIndex].m_uvVerts,
																							 uvMaps[uvMapIndex].m_pUVIndices,numFaces * 3,
																							 2 * sizeof(float), &uvTransform);
					TCArray.push_back( pNewTCs );

					workState.d_StreamDesc.push_back( createStreamElement( StreamElement::STREAM_TEXCOORD, pNewTCs) );
					workState.d_VertexDesc.push_back( createD3DVERTEXELEMENT9( 0, currentStreamOffset, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, layerIndex) );
					currentStreamOffset += 2 * sizeof( float );
				}
				else if( defMeshOptions & MESH_WRITE_EMPTY_TCS )
				{
					workState.d_StreamDesc.push_back( createStreamElement( StreamElement::STREAM_TEXCOORD | StreamElement::STREAM_FLAG_PAD, & StreamDummyTC2) );
					workState.d_VertexDesc.push_back( createD3DVERTEXELEMENT9( 0, currentStreamOffset, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, layerIndex) );
					currentStreamOffset += 2 * sizeof( float );
				}
			}

		}
		else if( defMeshOptions & MESH_NEEDS_TEXTURE_COORDINATES )
		{
			workState.d_StreamDesc.push_back( createStreamElement( StreamElement::STREAM_TEXCOORD | StreamElement::STREAM_FLAG_PAD, & StreamDummyTC2) );
			workState.d_VertexDesc.push_back( createD3DVERTEXELEMENT9( 0, currentStreamOffset, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0) );
			currentStreamOffset += 2 * sizeof( float );
		}

		workState.d_VertexDesc.push_back( Decl9End );
		// end decl

		// set some flags
		if( (defMeshOptions & MESH_WRITE_NORMALS)  &&  pStreamNormals )
			m_modelSOMFlags |= SOMM_TRI_STRIP_NORMALS;

		// if using environment mapping
		if( pSubMaterialHasEnvironmentMapValues[materialIndex] )
		{
			assert( pStreamNormals );

			workState.d_ShaderFlags |= ShaderRecord::SHFLAG_ENV_MAP;
			m_modelSOMFlags |= SOMM_NEEDS_ENV_MAPPING;
		}

		workState.d_MaterialIndex = materialIndex;
		// set strip opacities
		workState.d_EVMapOpacity = pSubMaterialEnvironmentMapOpacityValues[materialIndex] * 100.0f;

		if( pFaceFlags[pFaceIndices[0]] & TupTriMesh::FLAGS_FACE_TWO_SIDED )
		{
			workState.d_ShaderFlags |= ShaderRecord::SHFLAG_TWO_SIDED;
		}

		if( pFaceFlags[pFaceIndices[0]] & TupTriMesh::FLAGS_FACE_NO_RECEIVE_SHADOWS )
		{
			workState.d_ShaderFlags |= ShaderRecord::SHFLAG_NO_SHADOW;
		}
		// deal with the material flags
		// don't do much with this right now
		assert( m_materialFlagsArray.GetSize( ) );
		assert( (int) materialIndex < m_materialFlagsArray.GetSize( ) );

		int 				materialFlags = m_materialFlagsArray[materialIndex];

		if( stripIndex == 0 )
			currentState.first = workState;
		else if( currentState.first != workState )
		{
			triStripArray.push_back( currentState );

			currentState.first = workState;
			currentState.second.clear( );
		}

		currentState.second.push_back( stripIndex );
		
	}	

	triStripArray.push_back( currentState );
#ifdef WRITE_DEBUG_FILE
	FILE *pDebugFile = fopen( modeltxtname, "wt");
#else
	FILE *pDebugFile = 0;
#endif

	if( pDebugFile) fprintf( pDebugFile, "strips: %d verts: %d\n", totalStrips, numVerts);

	std::map< unsigned int, FVFVBBuilder >	builderMap;

	std::vector< int > 					IBList;
	FVFVertexBuilder						VertexBuilder;

	std::vector< FVFVBBuilder >		VBBuilderArray;
	std::vector< SubMeshRecord >		SubMeshArray;
	std::vector< FVFMatrixCache >		MCacheArray;

	std::vector< ShaderDescriptor >				ShaderArray;
	std::vector< SingleStreamDescriptor >		SSDescArray;
	std::vector< MultiStreamDescriptor >		MSDescArray;
	std::vector< unsigned int >					MaterialIndexArray;

	unsigned int							stateCount = 0;

	for( unsigned int state = 0; state < triStripArray.size( ); state++ )
	{
		int						idxMin = INT_MAX;
		int						idxMax = INT_MIN;

		SubMeshRecord				NewSubMesh;

		stateCount++;

		const TriStripState &	thisState = triStripArray[state].first;
		std::vector< int > &		stripArray = triStripArray[state].second;

		std::vector< FVFVBBuilder >::iterator myVBPos = addUniqueVB( VBBuilderArray, FVFVBBuilder( thisState.d_VertexDesc, 2048 ) );
		unsigned int				myVBIdx = std::distance( VBBuilderArray.begin( ), myVBPos);

		SingleStreamDescriptor	myDesc( thisState.d_VertexDesc, thisState.d_StreamDesc, myVBIdx );

		std::vector< SingleStreamDescriptor >::iterator myDescPos = addUniqueSSDesc( SSDescArray, myDesc );
		unsigned int				myStreamIdx = std::distance( SSDescArray.begin( ), myDescPos );

		// set up vertex stream
		VertexBuilder.startDataStreams( numVerts);

		for( unsigned int stream = 0; stream < thisState.d_StreamDesc.size( ); stream++ )
		{
			StreamElement element = thisState.d_StreamDesc[stream];
			
			VertexBuilder.pushDataStream( element.ptr );
		}

		bool						bNextCCW = false;

		NewSubMesh.d_DIPData.d_DIPPrimitiveCount = -2;
		NewSubMesh.d_DIPData.d_DIPType = D3DPT_TRIANGLESTRIP;
		NewSubMesh.d_ShaderListIndex = addUniqueShader( ShaderArray, ShaderDescriptor( thisState.d_ShaderFlags, myStreamIdx, thisState.d_TextureContextIndex, thisState.d_EVMapOpacity ) );
		NewSubMesh.d_SubMeshFlags = 0;

		NewSubMesh.d_IndexStreamIndex = 0;
		NewSubMesh.d_VertexStreamIndex = myStreamIdx;
		
		NewSubMesh.d_MatrixListIndex = MCacheArray.size( );

		if( m_Xbox  &&  (IBList.size( ) % 8) )
		{
			while( IBList.size( ) % 8 )
				IBList.push_back( 0xdead );
		}

		NewSubMesh.d_DIPData.d_IBBase = IBList.size( );

		unsigned int			numStrips = stripArray.size( );
		unsigned int			debugStripPoints = 0;

		if( pDebugFile) fprintf( pDebugFile, "state: %d strips\n", numStrips);

		for( unsigned int stripArrayIdx = 0; stripArrayIdx < numStrips; stripArrayIdx++)
		{
			stripIndex = stripArray[stripArrayIdx];
			dbgStripIndex = stripIndex;
			
			TupperwareAggregate *pTriangleStripAgg = tupTriangleStripPool.GetTriangleStrip(stripIndex);
			assert(pTriangleStripAgg);
			TupTriangleStrip tupTriangleStrip(pTriangleStripAgg);

			int				pointIndex;

			int *				pFaceIndices;
			int 				numFaceIndices;

			int *				pFacePointIndices;
			int 				numFacePointIndices;

			tupTriangleStrip.GetFaceIndices( &pFaceIndices, numFaceIndices);
			assert( numFaceIndices);

			tupTriangleStrip.GetFacePointIndices( &pFacePointIndices, numFacePointIndices);
			assert( numFacePointIndices);

			assert( numFaceIndices == numFacePointIndices);

			if( m_pBoneBuilder  &&  m_pBoneBuilder->GetNumBones( ) )
			{
				const int *				pBoneIndices = m_pBoneBuilder->GetBoneVerts( ).GetData( );

				{
					std::set< int >			UniqueBones;

					for( pointIndex = 0; pointIndex < numFacePointIndices; pointIndex++)
					{
						int faceIndex = pFaceIndices[pointIndex];
						int facePointIndex = pFacePointIndices[pointIndex];
						int vertexIndex = pPositionIndices[(faceIndex << 1) + faceIndex + facePointIndex];
		
						int boneIndex = pBoneIndices[vertexIndex];

						assert( boneIndex < 256 );

						UniqueBones.insert( boneIndex );
					}

					unsigned int size = UniqueBones.size( );

					assert( (int) UniqueBones.size( ) < m_sizeMatrixCache );
				}

				FVFMatrixCache					Temp( MCache );

				for( pointIndex = 0; pointIndex < numFacePointIndices; pointIndex++)
				{
					int faceIndex = pFaceIndices[pointIndex];
					int facePointIndex = pFacePointIndices[pointIndex];
					int vertexIndex = pPositionIndices[(faceIndex << 1) + faceIndex + facePointIndex];

					int boneIdx = pBoneIndices[vertexIndex];
					int boneSlot = Temp.addIndex( boneIdx );

					if( boneSlot == -1 )
						break;
				}

				if( pointIndex != numFacePointIndices )
				{
					MCacheArray.push_back( MCache );
					MCache.clearAll( );

					if( NewSubMesh.d_DIPData.d_DIPPrimitiveCount != -2)
					{
						NewSubMesh.d_DIPData.d_DIPMinIndex = idxMin;
						NewSubMesh.d_DIPData.d_DIPNumVertices = (idxMax + 1) - idxMin;

						SubMeshArray.push_back( NewSubMesh );
						SSDescArray[myStreamIdx].d_MeshCount++;		
					}

					// start new SubMesh
					bNextCCW = false;

					idxMin = INT_MAX;
					idxMax = INT_MIN;

					NewSubMesh.d_DIPData.d_DIPPrimitiveCount = -2;

					if( m_Xbox  &&  (IBList.size( ) % 8) )
					{
						while( IBList.size( ) % 8 )
							IBList.push_back( 0xdead );
					}
					NewSubMesh.d_DIPData.d_IBBase = IBList.size( );

					NewSubMesh.d_MatrixListIndex = MCacheArray.size( );
					dbgMLIndex = MCacheArray.size( );
				}
			}

			debugStripPoints += numFacePointIndices;
			if( pDebugFile) fprintf( pDebugFile, "  strip: %d points: %d\n", stripArrayIdx, numFacePointIndices);

			int							idx;

			if( numFacePointIndices > (int) VBBuilderArray[myVBIdx].verticesAvailable( ) )
			{
				VBBuilderArray[myVBIdx].close( );

				std::vector< FVFVBBuilder >::iterator newVBPos = addUniqueVB( VBBuilderArray, FVFVBBuilder( thisState.d_VertexDesc, 2048 ) );

				unsigned int newVBIdx = std::distance( VBBuilderArray.begin( ), newVBPos);
				assert( myVBIdx != newVBIdx );

				myVBIdx = newVBIdx;

				SingleStreamDescriptor	myDesc( thisState.d_VertexDesc, thisState.d_StreamDesc, myVBIdx );

				std::vector< SingleStreamDescriptor >::iterator newDescPos = addUniqueSSDesc( SSDescArray, myDesc );
				assert( myDescPos != newDescPos );

				unsigned int				newStreamIdx = std::distance( SSDescArray.begin( ), newDescPos );
				assert( myStreamIdx != newStreamIdx );

				if( NewSubMesh.d_DIPData.d_DIPPrimitiveCount != -2)
				{
					NewSubMesh.d_DIPData.d_DIPMinIndex = idxMin;
					NewSubMesh.d_DIPData.d_DIPNumVertices = (idxMax + 1) - idxMin;

					SubMeshArray.push_back( NewSubMesh );

					SSDescArray[myStreamIdx].d_MeshCount++;
				}

				myStreamIdx = newStreamIdx;

				// start new SubMesh
				bNextCCW = false;

				idxMin = INT_MAX;
				idxMax = INT_MIN;

				if( m_Xbox  &&  (IBList.size( ) % 8) )
				{
					while( IBList.size( ) % 8 )
						IBList.push_back( 0xdead );
				}

				NewSubMesh.d_DIPData.d_IBBase = IBList.size( );
				NewSubMesh.d_DIPData.d_DIPPrimitiveCount = -2;
				NewSubMesh.d_VertexStreamIndex = myStreamIdx;
			}

			for( pointIndex = 0; pointIndex < numFacePointIndices; pointIndex++)
			{
				int faceIndex = pFaceIndices[pointIndex];
				int facePointIndex = pFacePointIndices[pointIndex];

				idx = VBBuilderArray[myVBIdx].addVertex( VertexBuilder[ (faceIndex << 1) + faceIndex + facePointIndex ] );
				assert( idx != -1 );

				if( idx < idxMin)
					idxMin = idx;
				if( idx > idxMax)
					idxMax = idx;

				if( pDebugFile)
				{
					Vector3 & Vtx = pPositionVerts[pPositionIndices[(faceIndex << 1) + faceIndex + facePointIndex]];

					fprintf( pDebugFile, "   in idx: %05d out idx: %05d", (faceIndex << 1) + faceIndex + facePointIndex, idx);
					if( m_pBoneBuilder)
						fprintf( pDebugFile, " ml: %02d mtx idx: %02d mtx slot: %2d", MCacheArray.size( ), dbgIdx, dbgSlot );
//					fprintf( pDebugFile, " in vtx: %f %f %f\n", Vtx.x( ), Vtx.y( ), Vtx.z( ) );
					fprintf( pDebugFile, "\n" );
				}

				if( stripArrayIdx  &&  (pointIndex == 0) )
				{
					if( bNextCCW == false )
					{
						NewSubMesh.d_DIPData.d_DIPPrimitiveCount++;
						IBList.push_back( idx ); bNextCCW = !bNextCCW;
					}

					NewSubMesh.d_DIPData.d_DIPPrimitiveCount++;
					IBList.push_back( idx ); bNextCCW = !bNextCCW;
				}

				NewSubMesh.d_DIPData.d_DIPPrimitiveCount++;
				IBList.push_back( idx ); bNextCCW = !bNextCCW;
			}

			if( stripArrayIdx < (numStrips - 1) )
			{
				NewSubMesh.d_DIPData.d_DIPPrimitiveCount++;
				IBList.push_back( idx ); bNextCCW = !bNextCCW;
			}

		}

		if( pDebugFile) fprintf( pDebugFile, "new prims: %d num points: %d\n", NewSubMesh.d_DIPData.d_DIPPrimitiveCount, debugStripPoints);

		NewSubMesh.d_DIPData.d_DIPMinIndex = idxMin;
		NewSubMesh.d_DIPData.d_DIPNumVertices = (idxMax + 1) - idxMin;

		SubMeshArray.push_back( NewSubMesh );
		SSDescArray[myStreamIdx].d_MeshCount++;		

	}

	// delete any streams we created
	for( unsigned int tc = 0; tc < TCArray.size( ); tc++ )
		if( TCArray[tc] )
			delete TCArray[tc];

	if( pStreamVertices )
		delete pStreamVertices;

	if( pStreamBonedVertices )
		delete pStreamBonedVertices;

	if( pStreamNormals )
		delete pStreamNormals;

	if( pStreamColors )
		delete pStreamColors;

	if( m_pBoneBuilder  &&  m_pBoneBuilder->GetNumBones( ) )
		if( MCache.numSends( ) )
			MCacheArray.push_back( MCache );

	unsigned int		i;
	FileIOHelper		IO( pDBLFile );
	MeshRecord			MeshRec;

	MeshRec.d_MeshFlags = 0xdeadbeef;
	MeshRec.d_MatrixCacheSize = 16;

	MeshRec.d_OffsetIDRecords = 0;
	MeshRec.d_NumIDRecords = 1;

	MeshRec.d_OffsetVDRecords = 0;
	MeshRec.d_NumVDRecords = VBBuilderArray.size( );

	MeshRec.d_OffsetSSRecords = 0;
	MeshRec.d_NumSSRecords = SSDescArray.size( );

	MeshRec.d_OffsetMSRecords = 0;
	MeshRec.d_NumMSRecords = MSDescArray.size( );

	MeshRec.d_OffsetMLRecords = 0;
	MeshRec.d_NumMLRecords = MCacheArray.size( );

	MeshRec.d_OffsetShaderRecords = 0;
	MeshRec.d_NumShaderRecords = ShaderArray.size( );

	MeshRec.d_OffsetSubMeshRecords = 0;
	MeshRec.d_NumSubMeshRecords = SubMeshArray.size( );
	
	if( pDebugFile)
		fprintf( pDebugFile, "Mesh VBs: %d SStreams: %d MStreams: %d MLs: %d SMs: %d\n",
			MeshRec.d_NumVDRecords, MeshRec.d_NumSSRecords, MeshRec.d_NumMSRecords, MeshRec.d_NumMLRecords, MeshRec.d_NumSubMeshRecords );


	/* remember location, output header */
	u32HeaderPos = ftell( pDBLFile);

	// write MeshRecord
	fwrite( &MeshRec, sizeof( MeshRecord), 1, pDBLFile);

	// write IBDataRecord
	unsigned int u32IBRecPos = ftell( pDBLFile);

	MeshRec.d_OffsetIDRecords = u32IBRecPos - u32HeaderPos;

	IndexDataRecord		IBRec;

	IBRec.d_NumIndices = IBList.size( );
	IBRec.d_CIBFormat = D3DFMT_INDEX16;

	fwrite( &IBRec, sizeof( IBRec), 1, pDBLFile );

	if( pDebugFile) fprintf( pDebugFile, "IB Size: %d\n", IBList.size( ) );

	if( m_Xbox )
	{
		char				strIdentifier[128];
		HRESULT			result = S_OK;
			
		result = m_XPRBundler.padToAlignment( D3DINDEXBUFFER_ALIGNMENT );
		assert( result == S_OK );

		IBRec.d_OffsetIndexData = m_XPRBundler.m_cbHeader;

		D3DIndexBuffer d3dib;

		d3dib.Common =	D3DCOMMON_TYPE_INDEXBUFFER |	   // Type
							1;                               // Initial Refcount

		d3dib.Data   = m_XPRBundler.m_cbData;				// Offset of data in data file
		d3dib.Lock   = 0;											// Must be zero

		// Write the resource header out
		result = m_XPRBundler.writeHeader( &d3dib, sizeof(d3dib) );
		assert( result == S_OK );

		for( i = 0; i < IBList.size( ); i++)
		{
			unsigned short 	ibval = IBList[i];
			result = m_XPRBundler.writeData( &ibval, sizeof( unsigned short ) );
			assert( result == D3D_OK );
		}

		sprintf( strIdentifier, "IB%02d", m_XPRBundler.m_dwNumResources );
		m_XPRBundler.addResourceItem( strIdentifier, strIdentifier, sizeof( d3dib ), IBList.size( ) * sizeof( unsigned short ) );
	
	}
	else
	{
	IBRec.d_OffsetIndexData = ftell( pDBLFile ) - u32HeaderPos;
	// write IBIndicies
	for( i = 0; i < IBList.size( ); i++)
	{
		unsigned short 	ibval = IBList[i];
		fwrite( &ibval, sizeof( unsigned short), 1, pDBLFile );
	}
	}

	// re-write the VBDataRecords
	u32CurrPos = ftell( pDBLFile );
	fseek( pDBLFile, u32IBRecPos, SEEK_SET );

	fwrite( &IBRec, sizeof( IBRec), MeshRec.d_NumIDRecords, pDBLFile );

	fseek( pDBLFile, u32CurrPos, SEEK_SET );

	// write the VBDataRecords
	VertexDataRecord *	pVBRecs = new VertexDataRecord[MeshRec.d_NumVDRecords];

	unsigned int			u32VBRecPos = ftell( pDBLFile );

	MeshRec.d_OffsetVDRecords = u32VBRecPos - u32HeaderPos;

	// write VBData
	fwrite( pVBRecs, sizeof( VertexDataRecord), MeshRec.d_NumVDRecords, pDBLFile );

	if( pDebugFile) fprintf( pDebugFile, "VBs: %d\n", MeshRec.d_NumVDRecords);

	// and fill VBDataRecords
	for( i = 0; i < MeshRec.d_NumVDRecords; i++)
	{
		FVFVBBuilder &			thisVB = VBBuilderArray[i];
		if( pDebugFile) fprintf( pDebugFile, "VB: %d verts: %d stride: %d\n", i, thisVB.vertices( ), thisVB[0].bytes( ) );

		pVBRecs[i].d_NumVertices = thisVB.vertices( );
		pVBRecs[i].d_VertexBytes = thisVB[0].bytes( );
		pVBRecs[i].d_BufferBytes = thisVB.bytes( );

		if( m_Xbox )
		{
			char				strIdentifier[128];
			HRESULT			result = S_OK;
			
			result = m_XPRBundler.padToAlignment( D3DVERTEXBUFFER_ALIGNMENT );
			assert( result == S_OK );

			pVBRecs[i].d_OffsetVertexData = m_XPRBundler.m_cbHeader;
			D3DVertexBuffer d3dvb;

			d3dvb.Common =	D3DCOMMON_TYPE_VERTEXBUFFER |    // Type
								D3DCOMMON_VIDEOMEMORY |          // VB lives in video memory
								1;                               // Initial Refcount

			d3dvb.Data   = m_XPRBundler.m_cbData;				// Offset of data in data file
			d3dvb.Lock   = 0;											// Must be zero

			// Write the resource header out
			result = m_XPRBundler.writeHeader( &d3dvb, sizeof(d3dvb) );
			assert( result == S_OK );

			std::for_each( thisVB.d_VB.begin( ), thisVB.d_VB.end( ), XPRIOHelper( m_XPRBundler) );

			sprintf( strIdentifier, "VB%02d", m_XPRBundler.m_dwNumResources );
			m_XPRBundler.addResourceItem( strIdentifier, strIdentifier, sizeof( d3dvb ), pVBRecs[i].d_BufferBytes );

		}
		else
		{
			pVBRecs[i].d_OffsetVertexData = ftell( pDBLFile ) - u32HeaderPos;

		std::for_each( thisVB.d_VB.begin( ), thisVB.d_VB.end( ), FileIOHelper( pDBLFile ) );
	}
	}

	// re-write the VBDataRecords
	u32CurrPos = ftell( pDBLFile );
	fseek( pDBLFile, u32VBRecPos, SEEK_SET );

	fwrite( pVBRecs, sizeof( VertexDataRecord), MeshRec.d_NumVDRecords, pDBLFile );

	fseek( pDBLFile, u32CurrPos, SEEK_SET );
	
	delete [] pVBRecs;

	if( MeshRec.d_NumSSRecords )
	{
		unsigned int			u32SSRecPos = ftell( pDBLFile );
		SSDataRecord			*pSSRecs = new SSDataRecord[MeshRec.d_NumSSRecords];

		MeshRec.d_OffsetSSRecords = ftell( pDBLFile ) - u32HeaderPos;
		
		fwrite( pSSRecs, sizeof( SSDataRecord ), MeshRec.d_NumSSRecords, pDBLFile );

		if( pDebugFile) fprintf( pDebugFile, "SubStreams: %d\n", MeshRec.d_NumSSRecords );

		for( i = 0; i < MeshRec.d_NumSSRecords; i++ )
		{
			SingleStreamDescriptor & thisDesc = SSDescArray[i];

			pSSRecs[i].d_FVF = thisDesc.d_FVF;
			pSSRecs[i].d_StreamFlags = thisDesc.d_StreamDesc;
			pSSRecs[i].d_VBDataIndex = thisDesc.d_VDIndex;
			pSSRecs[i].d_NumSubMeshes = thisDesc.d_MeshCount;
			pSSRecs[i].d_OffsetDescriptor = ftell( pDBLFile ) - u32HeaderPos;

			if( pDebugFile ) fprintf( pDebugFile, "SS %d VDIdx: %d SMs: %d DL: %d\n", i, pSSRecs[i].d_VBDataIndex, pSSRecs[i].d_NumSubMeshes, thisDesc.d_Desc.size( ) );

			for( unsigned int element = 0; element < thisDesc.d_Desc.size( ); element++ )
				fwrite( (char *) &thisDesc.d_Desc[element], sizeof( D3DVERTEXELEMENT9), 1, pDBLFile );


		}

		// re-write the records
		unsigned int				u32CurrPos = ftell( pDBLFile );

		fseek( pDBLFile, u32SSRecPos, SEEK_SET );
		fwrite( pSSRecs, sizeof( SSDataRecord ), MeshRec.d_NumSSRecords, pDBLFile );
		fseek( pDBLFile, u32CurrPos, SEEK_SET );

	}

	// write the MultiStream Records
	if( MeshRec.d_NumMSRecords )
	{
		MSDataRecord 			MSRec;

		MeshRec.d_OffsetMSRecords = ftell( pDBLFile ) - u32HeaderPos;

		for( i = 0; i < MeshRec.d_NumMSRecords; i++)
		{
			unsigned int				u32RecPos = ftell( pDBLFile );

			MultiStreamDescriptor &	thisDesc = MSDescArray[i];

			fwrite( (char *) &MSRec, sizeof( MSDataRecord ), 1, pDBLFile );

			MSRec.d_NumStreamSets = thisDesc.a_StreamIndexes.size( );
			MSRec.d_NumSubMeshes = thisDesc.d_MeshCount;

			MSRec.d_OffsetDescriptor = ftell( pDBLFile ) - u32HeaderPos;

			unsigned int				element;

			for( element = 0; element < thisDesc.d_Desc.size( ); element++ )
				fwrite( (char *) &thisDesc.d_Desc[element], sizeof( D3DVERTEXELEMENT9), 1, pDBLFile );

			MSRec.d_OffsetStreamSetData = ftell( pDBLFile ) - u32HeaderPos;

			for( element = 0; element < thisDesc.a_StreamIndexes.size( ); element++ )
				fwrite( (char *) &thisDesc.a_StreamIndexes[element], sizeof( streamPair ), 1, pDBLFile );

			unsigned int				u32CurrPos = ftell( pDBLFile );

			fseek( pDBLFile, u32RecPos, SEEK_SET );
			fwrite( (char *) &MSRec, sizeof( MSDataRecord ), 1, pDBLFile );
			fseek( pDBLFile, u32CurrPos, SEEK_SET );
		}

	}

	// write the Matrix Caches
	if( MCacheArray.size( ) )
	{
		unsigned int			u32MLRecPos = ftell( pDBLFile );
		MLDataRecord *			pMLRecs = new MLDataRecord[MeshRec.d_NumMLRecords];

		MeshRec.d_OffsetMLRecords = u32MLRecPos - u32HeaderPos;

		if( pDebugFile) fprintf( pDebugFile, "ML Size: %d\n", MeshRec.d_NumMLRecords );

		fwrite( pMLRecs, sizeof( MLDataRecord ), MeshRec.d_NumMLRecords, pDBLFile );

		unsigned int			maxIndex = 0;

		for( i = 0; i < MeshRec.d_NumMLRecords; i++)
		{
			unsigned int			numSends = 0;
			FVFMatrixCache &		MCache = MCacheArray[i];

			pMLRecs[i].d_NumMatrices = MCache.numSends( );
			pMLRecs[i].d_OffsetML = ftell( pDBLFile ) - u32HeaderPos;

			if( pDebugFile) fprintf( pDebugFile, "ML %d Size: %d\n", i, MCache.numSends( ) );

			for( unsigned int mtx = 0; mtx < MCache.size( ); mtx++)
			{
				if( MCache.needsSend( mtx ) )
				{
					unsigned char idx = MCache[mtx];		
					unsigned char slot = mtx;

					if( pDebugFile) fprintf( pDebugFile, " idx: %d slot: %d\n", idx, slot );

					fwrite( &idx, sizeof( idx ), 1, pDBLFile );
					fwrite( &slot, sizeof( slot), 1, pDBLFile );

					numSends++;

					if( idx > maxIndex )
						maxIndex = idx;
				}
			}

			assert( numSends == MCache.numSends( ) );
		}

		MeshRec.d_MatrixIndexMax = maxIndex;

		// re-write the records
		u32CurrPos = ftell( pDBLFile );
		fseek( pDBLFile, u32MLRecPos, SEEK_SET );
		
		fwrite( pMLRecs, sizeof( MLDataRecord ), MeshRec.d_NumMLRecords, pDBLFile );

		fseek( pDBLFile, u32CurrPos, SEEK_SET );

		delete [] pMLRecs;
	}

	if( pDebugFile )
	{
		for( i = 0; i < MeshRec.d_NumSubMeshRecords; i++)
		{
			SubMeshRecord & Rec = SubMeshArray[i];

			fprintf( pDebugFile, "SIdx: %d MLIdx: %d IBBase: %d TC: %d\n", Rec.d_VertexStreamIndex, Rec.d_MatrixListIndex, Rec.d_DIPData.d_IBBase, Rec.d_ShaderListIndex);
			fprintf( pDebugFile, "MI: %d NV: %d PC: %d\n", Rec.d_DIPData.d_DIPMinIndex, Rec.d_DIPData.d_DIPNumVertices, Rec.d_DIPData.d_DIPPrimitiveCount );
		}
	}

	// write the material indexes
	MeshRec.d_OffsetShaderRecords = ftell( pDBLFile ) - u32HeaderPos;
	
	ShaderRecord	SHRec = { 0 };

	for( i = 0; i < ShaderArray.size( ); i++ )
	{
		ShaderDescriptor & desc = ShaderArray[i];

		SHRec.d_ShaderFlags = desc.d_ShaderFlags;
		SHRec.d_StreamIndex = desc.d_StreamIndex;
		SHRec.d_TextureContextID = desc.d_TextureContextID;
		SHRec.d_EnvMapOpacity = desc.d_EnvMapOpacity;

		fwrite( & SHRec, sizeof( ShaderRecord ), 1, pDBLFile );
	}

	// write the Submeshes
	MeshRec.d_OffsetSubMeshRecords = ftell( pDBLFile) - u32HeaderPos;
	fwrite( &SubMeshArray[0], sizeof( SubMeshRecord), MeshRec.d_NumSubMeshRecords, pDBLFile );

	// rewrite the master mesh record
	MeshRec.d_MeshFlags = 0xdeadbeef;

	u32CurrPos = ftell( pDBLFile );

	fseek( pDBLFile, u32HeaderPos, SEEK_SET );	
	fwrite( &MeshRec, sizeof( MeshRecord), 1, pDBLFile );

	fseek( pDBLFile, u32CurrPos, SEEK_SET );

	/* alignment */
	DBLUtil::AlignFile( );

	if( pDebugFile) fclose( pDebugFile );

	/* finish data size statistic */
	m_totalStripDataSize = ftell( pDBLFile ) - m_totalStripDataSize;

#ifdef WRITE_DEBUG_FILE
	fclose( pDBLFile );
#endif
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
