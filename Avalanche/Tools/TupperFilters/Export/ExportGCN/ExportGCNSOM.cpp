////////////////////////////////////////////////////////////////////////////
//
// ExportGCNSOM
//
// Export Simple Ordinary Meshes into the DBL file (GAMECUBE)
//
////////////////////////////////////////////////////////////////////////////

#include "ExportPch.h"
#include "ExportGCNSOM.h"
#include "ExportBones.h"

#include "dolphin/gd.h"
#include "Dolphin/os.h"

#include <container/dllist.h>

static u32 gxMtxID[] =
{
	GX_PNMTX0,
	GX_PNMTX1,
	GX_PNMTX2,
	GX_PNMTX3,
	GX_PNMTX4,
	GX_PNMTX5,
	GX_PNMTX6,
	GX_PNMTX7,
	GX_PNMTX8,
	GX_PNMTX9
};

static u32 gxTexMtxID[] =
{
	GX_TEXMTX0,
	GX_TEXMTX1,
	GX_TEXMTX2,
	GX_TEXMTX3,
	GX_TEXMTX4,
	GX_TEXMTX5,
	GX_TEXMTX6,
	GX_TEXMTX7
};

class CGXVtxDescList
{
	public:
		CGXVtxDescList( )
		{
			Init( );
		}

		void Init( void)
		{
			for( int i = 0; i < GX_VA_MAX_ATTR + 1; i++)
			{
				m_aGXVtxDescList[i].attr = GX_VA_NULL;
				m_aGXVtxDescList[i].type = GX_NONE;
			}
			m_Index = 0;
		}

		void Add( GXAttr attr, GXAttrType type)
		{
			if( m_Index >= GX_VA_MAX_ATTR)
			{
				return;
			}
			m_aGXVtxDescList[m_Index].attr = attr;
			m_aGXVtxDescList[m_Index].type = type;
			m_Index++;
		}

		operator GXVtxDescList *( void)
		{
			return( m_aGXVtxDescList);
		}

		bool operator == ( CGXVtxDescList &right)
		{
			if( m_Index != right.m_Index)
			{
				return( false);
			}

			for( int iLeft = 0; iLeft < m_Index; iLeft++)
			{
				
				bool	bFound = false;

				for( int iRight = 0; iRight < m_Index; iRight++)
				{
					if( m_aGXVtxDescList[iLeft].attr == right.m_aGXVtxDescList[iRight].attr)
					{
						bFound = true;
						if( m_aGXVtxDescList[iLeft].type != right.m_aGXVtxDescList[iRight].type)
						{
							return( false);
						}
						break;
					}
				}

				if( !bFound)
				{
					return( false);
				}
			}

			return( true);
		}

		bool operator != ( CGXVtxDescList &right)
		{
			return( !( *this == right));
		}

	private:
		GXVtxDescList	m_aGXVtxDescList[GX_VA_MAX_ATTR + 1];

		int				m_Index;
};

#define MatrixLoaderNumMtx			( 10)
#define MatrixLoaderNoMtxLoaded	( -1)

class CGCNMatrixSet
{
	public:
		CGCNMatrixSet		*prev,
								*next;

		CGCNMatrixSet( int stripIndex)
		{
			m_StripIndex = stripIndex;
			m_SlotsUsed = 0;
			for( int i = 0; i < MatrixLoaderNumMtx; i++)
			{
				m_MtxLoaded[i] = MatrixLoaderNoMtxLoaded;
			}
		}

		bool AddBoneIndices( ts_VertexWeightingBones *boneIndices)
		{

			int	bonesUsed = 1;

			int	addMask = 0;

			for( int iBone = 1; iBone < 4; iBone++)
			{
				if( boneIndices->u8BoneIndex[iBone] != 0)
				{
					bonesUsed++;
				}
			}

			int	slotsNeeded = bonesUsed;
			
			for( iBone = 0; iBone < bonesUsed; iBone++)
			{

				int	boneNeeded = 1 << iBone;

				for( int iSlot = 0; iSlot < m_SlotsUsed; iSlot++)
				{
					if( m_MtxLoaded[iSlot] == boneIndices->u8BoneIndex[iBone])
					{
						slotsNeeded--;
						boneNeeded = 0;
					}
				}

				addMask |= boneNeeded;
			}

			if( ( m_SlotsUsed + slotsNeeded) > MatrixLoaderNumMtx)
			{
				return( false);
			}

			for( iBone = 0; iBone < bonesUsed; iBone++)
			{
				if( addMask & 0x01)
				{
					m_MtxLoaded[m_SlotsUsed] = boneIndices->u8BoneIndex[iBone];
					m_SlotsUsed++;
				}
				addMask >>= 1;
			}

			return( true);
		}

		int GetStripIndex( void)
		{
			return( m_StripIndex);
		}
		
		u32 GetGCNMtxIdx( int bone)
		{

			u32	retVal = gxMtxID[0];

			for( int i = 0; i < m_SlotsUsed; i++)
			{
				if( m_MtxLoaded[i] == bone)
				{
					retVal = gxMtxID[i];
				}
			}
			return( retVal);
		}

		void Load( void)
		{
			for( int iSlot = 0; iSlot < m_SlotsUsed; iSlot++)
			{
				GDLoadPosMtxIndx( m_MtxLoaded[iSlot], gxMtxID[iSlot]);
				GDLoadNrmMtxIndx3x3( m_MtxLoaded[iSlot], gxMtxID[iSlot]);
			}
		}
		
	private:
		int						m_StripIndex;
		int						m_SlotsUsed;
		int						m_MtxLoaded[MatrixLoaderNumMtx];
};

class CGCNMatrixLoader
{
	public:
		CGCNMatrixLoader( void) : m_pCurrMtxSet(NULL)
		{
		}

		u8 GetGCNMtxIdx( int bone)
		{
			return( (u8)m_pCurrMtxSet->GetGCNMtxIdx( bone));
		}

		void AddBoneIndices( int stripIndex, ts_VertexWeightingBones *boneIndices)
		{
			// first time through allocate first set (moved from constructor since some meshes never get here)
			if (stripIndex==0)
			{
				m_pCurrMtxSet = new CGCNMatrixSet(stripIndex);
			}

			if( !m_pCurrMtxSet->AddBoneIndices( boneIndices))
			{
				m_MtxSetList.AddTail( m_pCurrMtxSet);
				m_pCurrMtxSet = new CGCNMatrixSet( stripIndex);
				m_pCurrMtxSet->AddBoneIndices( boneIndices);
			}
		}

		void InitMtxSetLoad( int numStrips)
		{
			//add the last one we were working on
			m_MtxSetList.AddTail( m_pCurrMtxSet);

			//add a dummy one so CheckStripIndex will have one to compare to
			m_pCurrMtxSet = new CGCNMatrixSet( numStrips);
			m_MtxSetList.AddTail( m_pCurrMtxSet);

			m_pCurrMtxSet = m_MtxSetList.Head( );
			m_pCurrMtxSet->Load( );
		}

		void CheckStripIndex( int stripIndex)
		{
			if( stripIndex == m_pCurrMtxSet->next->GetStripIndex( ))
			{
				m_MtxSetList.Remove( m_pCurrMtxSet);
				m_pCurrMtxSet = m_MtxSetList.Head( );
				m_pCurrMtxSet->Load( );
			}
		}
		
	private:
		DoublyLinkedList<CGCNMatrixSet>	m_MtxSetList;
		CGCNMatrixSet							*m_pCurrMtxSet;
};


ExportGCNSOM::ExportGCNSOM(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupObjectPool &tupObjectPool,TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool,int currentPartition) :
	m_pDBLFileName(pDBLFileName), m_options(options), m_dataManager(dataManager),
	m_tupObjectPool(tupObjectPool),m_tupMaterialPool(tupMaterialPool),m_tupMapPool(tupMapPool),m_tupImagePool(tupImagePool),
	m_currentPartition(currentPartition)
{
}

void ExportGCNSOM::StartChunk(int numMeshes,int numBoneMeshes)
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
u32 ExportGCNSOM::FinishChunk(void)
{
	/* finish record list */
	DBLUtil::FinishRecordList();
	return DBLUtil::FinishChunk(FALSE);
}


void ExportGCNSOM::WriteMesh(TupTriMesh &tupTriMesh,BoneBuilder &boneBuilder)
{
	m_pBoneBuilder = &boneBuilder;
	m_pTupTriMesh = &tupTriMesh;

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
	BuildTextureContexts(materialRef);
	BuildFlagsArray(materialRef);

	/* mark new record list entry */
	DBLUtil::RecordListEntry();
	// set up som flags
	m_modelSOMFlags = 0;

	// set flag here for tristrip access
	if (m_bOutputBones && m_pBoneBuilder->GetNumBones())
	{
		m_modelSOMFlags |= SOMM_HAS_BONES;
	}

	/* start SOM model */
	StartModel();

	WriteTriStrips();

	// if the mesh needs to output bones
	if (m_modelSOMFlags & SOMM_HAS_BONES)
	{
		if (!m_pBoneBuilder->RemappedToMaster())
		{
			WriteBoneData();
		}
		else
		{
			m_modelSOMFlags |= SOMM_BONES_SLAVE;
		}
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
	m_pBoneBuilder = NULL; // we don't own this
}

void ExportGCNSOM::StartModel(void)
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

void ExportGCNSOM::FinishModel(void)
{
	FILE *pDBLFile = DBLUtil::GetDBLFile();

	ts_PatchSOM PatchSOMHdr;
	memset(&PatchSOMHdr, 0, sizeof(ts_PatchSOM));

	PatchSOMHdr.dh.u8DataType = DATA_GCN_SOM2;

	TupperwareAggregate *pMeshAgg = m_pTupTriMesh->GetBaseAggregate();
	PatchSOMHdr.BoundingBox.Min = *((Vector3 *)pMeshAgg->FindListByKey("Bounding_BoundingBoxMin")->GetAsFloat());
	PatchSOMHdr.BoundingBox.Max = *((Vector3 *)pMeshAgg->FindListByKey("Bounding_BoundingBoxMax")->GetAsFloat());
	PatchSOMHdr.BoundingSphere.Center = *((Vector3 *)pMeshAgg->FindListByKey("Bounding_BoundingSphereCenter")->GetAsFloat());
	PatchSOMHdr.BoundingSphere.Radius = pMeshAgg->FindScalarByKey("Bounding_BoundingSphereRadius")->GetAsFloat();

	PatchSOMHdr.u32TriStripCount = m_modelTriStripCount;
	PatchSOMHdr.u32TriStripVertexColorCount = m_modelTriStripVertexColorCount;
	PatchSOMHdr.u32TriStripOffset = m_modelTriStripFileOffset;
	PatchSOMHdr.u32VertexOffset = m_modelPositionFileOffset;
	PatchSOMHdr.u32NormalOffset = m_modelNormalFileOffset;
	PatchSOMHdr.u32UVOffset = m_modelUVFileOffset;

	PatchSOMHdr.u32VertexCount = m_modelPositionCount;
	PatchSOMHdr.u32NormalCount = m_modelNormalCount;
	PatchSOMHdr.u32UVCount = m_modelUVCount;

	pMeshAgg->AddScalarInt("ExportGCN - VertexCount",NULL,m_modelPositionCount);
	pMeshAgg->AddScalarInt("ExportGCN - NormalCount",NULL,m_modelNormalCount);
	pMeshAgg->AddScalarInt("ExportGCN - UVCount",NULL,m_modelUVCount);


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

static bool CompareTriStripHeaders(ts_GCNTriStripHeader &header1,ts_GCNTriStripHeader &header2)
{

	u8		flagMask = ( TRI_STRIP_TWO_SIDED | TRI_STRIP_GCN_RECEIVE_SHADOWS);

	if( header1.u32TextureContextIndex != header2.u32TextureContextIndex)
	{
		return( false);
	}

	if( ( header1.u8Flags & flagMask) != ( header2.u8Flags & flagMask))
	{
		return( false);
	}
	return true;
}

class CompressedNormal
{
public:
	char x;
	char y;
	char z;
	void Set(Vector3 &source) 
	{
		x = BuildValue(source.x( ) );
		y = BuildValue(source.y( ) );
		z = BuildValue(source.z( ) );
	}
	bool operator == (const CompressedNormal& src)
	{
		return (x==src.x && y==src.y && z==src.z);
	}
private:
	static char BuildValue(float source)
	{
		if (source>=0.0f)
		{
			return (char)((source*64.0f)+0.5f); // round up or down based on sign
		}
		else
		{
			return (char)((source*64.0f)-0.5f);
		}
	}
};

void ExportGCNSOM::GetConsolidatedColorIndices(TupArray<int> &indices,int colorRef,int alphaRef,bool b32bitColors)
{
	indices.RemoveAll();

	Vector3 *pVertexColorVerts = NULL;
	int *pVertexColorIndices = NULL;
	int numVertexColorVerts;
	int numVertexColorFaces;
	
	Vector3 *pVertexAlphaVerts = NULL;
	int *pVertexAlphaIndices = NULL;
	int numVertexAlphaVerts;
	int numVertexAlphaFaces;
	
	if (colorRef!=-1)
	{
		TupperwareAggregate *pChildVertexColorMeshMapAgg = AnnotateHelper::FindChildVertexColorMeshMap(m_tupObjectPool,colorRef);
		TupTriMeshMap tupTriMeshMap(pChildVertexColorMeshMapAgg);
		tupTriMeshMap.GetMapVerts((float **) &pVertexColorVerts, numVertexColorVerts );
		tupTriMeshMap.GetMapIndices( &pVertexColorIndices, numVertexColorFaces );
	}

	if (alphaRef!=-1)
	{
		TupperwareAggregate *pChildVertexAlphaMeshMapAgg = AnnotateHelper::FindChildVertexAlphaMeshMap(m_tupObjectPool,alphaRef);
		TupTriMeshMap tupTriMeshMap(pChildVertexAlphaMeshMapAgg);
		tupTriMeshMap.GetMapVerts((float **) &pVertexAlphaVerts, numVertexAlphaVerts );
		tupTriMeshMap.GetMapIndices( &pVertexAlphaIndices, numVertexAlphaFaces );
	}

	Keeper<RGBAColor> colorKeeperL;
	Keeper<u16> colorKeeperS;

	TupperwareAggregate *pTriangleStripsAgg = m_pTupTriMesh->GetTriangleStrips();
	if (!pTriangleStripsAgg)
		throw(PluginException(ERROR_FATAL,"Cannot find tristrip data on mesh"));
	
	TupTriangleStripPool tupTriangleStripPool(pTriangleStripsAgg);
	int totalStrips = tupTriangleStripPool.GetNumTriangleStrips();
	if (!totalStrips)
		throw(PluginException(ERROR_FATAL,"Cannot find tristrip data on mesh"));
	
	// add to the pools to determine the size (can handle 8 bit indicies)
	int stripIndex;
	for (stripIndex=0;stripIndex<totalStrips;stripIndex++)
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

		for (int pointIndex=0;pointIndex<numFacePointIndices;pointIndex++)
		{
			int faceIndex = pFaceIndices[pointIndex];
			int facePointIndex = pFacePointIndices[pointIndex];
			
			if (b32bitColors)
			{
				RGBAColor destColor32;
				// do vertex color
				if (pVertexColorVerts)
				{
					int vertexIndex = pVertexColorIndices[(faceIndex<<1)+faceIndex+facePointIndex];
					Vector3 &sourceColor = pVertexColorVerts[vertexIndex];
					destColor32.R = (u8)(sourceColor.x() *255.0f);
					destColor32.G = (u8)(sourceColor.y() *255.0f);
					destColor32.B = (u8)(sourceColor.z() *255.0f);
				}
				else
				{
					destColor32.R = 255;
					destColor32.G = 0;
					destColor32.B = 255;
				}

				// do vertex alpha
				if (pVertexAlphaVerts)
				{
					int vertexIndex = pVertexAlphaIndices[(faceIndex<<1)+faceIndex+facePointIndex];
					Vector3 &sourceColor = pVertexAlphaVerts[vertexIndex];
					destColor32.A = (u8)(sourceColor.x() *255.0f);
				}
				else
				{
					destColor32.A = 255;
				}

				indices.Add(colorKeeperL.Add(destColor32));
			}
			else
			{
				u16 destColor16;
				
				if (pVertexColorIndices)
				{
					int vertexIndex = pVertexColorIndices[(faceIndex<<1)+faceIndex+facePointIndex];
					Vector3 &sourceColor = pVertexColorVerts[vertexIndex];
					destColor16 = ((((u8)(sourceColor.x()*31.0f))&0x1f)<<11)|
						((((u8)(sourceColor.y()*63.0f))&0x3f)<<5)|
						((((u8)(sourceColor.z()*31.0f))&0x1f)<<0);
				}
				else
				{
					destColor16 = (0x1f<<11)|0x1f;
				}
				indices.Add(colorKeeperS.Add(destColor16));
			}
		}
	}
}

void DLOverflowCallback( void)
{
	assert( 0);
}

void ExportGCNSOM::WriteTriStrips(void)
{
	FILE *pDBLFile = DBLUtil::GetDBLFile();

	TupScene tupScene(m_dataManager.GetSceneRoot());

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

	// set up vertex color info so we can later determine if we need to increment the vertex color offsets
	int *pVertexColorRefData = NULL;
	int *pVertexAlphaRefData = NULL;
	int nNumColorSets = 0;

	bool bUseConsolidatedColors=false;
	TupArray<int> consolidatedColorIndices;

	bool bFoundAlpha = false;
	if (bHasVertexColors)
	{
		TupperwareList *pVertexColorRefList = pMeshAgg->FindListByKey("Annotate_VertexColorRefList");
		TupperwareList *pVertexAlphaRefList = pMeshAgg->FindListByKey("Annotate_VertexAlphaRefList");
		if (pVertexColorRefList && pVertexAlphaRefList)
		{
			pVertexColorRefData = pVertexColorRefList->GetAsInt();
			pVertexAlphaRefData = pVertexAlphaRefList->GetAsInt();
			nNumColorSets = pVertexColorRefList->GetLength();

			for (int setIndex=0;setIndex<nNumColorSets;setIndex++)
			{
				if (pVertexAlphaRefData[setIndex] !=-1)
				{
					bFoundAlpha = true;
					break;
				}
			}

			if (nNumColorSets==1)
			{
				// check if all references are pointing to a -1 reference (standard white)
				if (pVertexColorRefData[0] == -1 && pVertexAlphaRefData[0] == -1)
				{
					// lets say there are no vertex color sets
					nNumColorSets = 0;
				}
				else
				{
					// get a set of indices for use (only one color set used)
					bUseConsolidatedColors = true;
					GetConsolidatedColorIndices(consolidatedColorIndices,pVertexColorRefData[0],pVertexAlphaRefData[0],bFoundAlpha);
				}
			}
		}
	}

	/* nothing yet */

	m_modelTriStripCount = 0;
	m_modelTriStripVertexColorCount = 0;
	m_modelTriStripFaceCount = 0;

	DBLUtil::AlignChunkData32Byte();
	/* first record where data is going to be. data should be quad-word aligned for ps2 */
	m_modelTriStripFileOffset = ftell(pDBLFile) - m_modelHeaderFilePos;

	/* record file position for calculation of data size statistic */
	m_totalStripDataSize = ftell(pDBLFile);

	/* count them first, set up for saving vertex lists (for vertex color arrangement) */
	/* point to vertex data, normal data, etc */
	/* and point to index lists */
	int numPosVerts, numNormalVerts, tempValue;
	Vector3 *pPositionVerts, *pNormalVerts;
	int *pPositionIndices, *pNormalIndices;
	int *pMaterialIndices,*pFaceFlags;

	TupArray<UVMap> uvMaps;
	Keeper<int> uvChannelToIndexKeeper;

	int numFaces;

	m_pTupTriMesh->GetFaceFlags( &pFaceFlags, numFaces );
	m_pTupTriMesh->GetPositionVerts((float **) &pPositionVerts, numPosVerts );
	m_pTupTriMesh->GetNormalVerts((float **) &pNormalVerts, numNormalVerts );
	m_pTupTriMesh->GetPositionIndices( &pPositionIndices, tempValue );
	m_pTupTriMesh->GetNormalIndices( &pNormalIndices, tempValue );
	m_pTupTriMesh->GetFaceMaterialIndices(&pMaterialIndices,tempValue);

	// get the opacities for the tristrips (comes from the enviroment maps if any)
	float *pSubMaterialEnvironmentMapOpacityValues = pMeshAgg->FindListByKey("Annotate_SubMaterialEnvironmentMapOpacityList")->GetAsFloat();
	int *pSubMaterialHasEnvironmentMapValues = pMeshAgg->FindListByKey("Annotate_SubMaterialHasEnvironmentMapList")->GetAsInt();

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
	int nRunningPointCount=0;

	TupperwareAggregate *pTriangleStripsAgg = m_pTupTriMesh->GetTriangleStrips();
	if (!pTriangleStripsAgg)
		throw(PluginException(ERROR_FATAL,"Cannot find tristrip data on mesh"));

	Keeper<Vector2> uvPool;
	Keeper<Vector3> posPool;
	Keeper<CompressedNormal> normalPool;

	TupTriangleStripPool tupTriangleStripPool(pTriangleStripsAgg);
	int totalStrips = tupTriangleStripPool.GetNumTriangleStrips();
	if (!totalStrips)
		throw(PluginException(ERROR_FATAL,"Cannot find tristrip data on mesh"));

	ts_GCNTriStripHeader baseTriStripHeader;	
	ts_GCNTriStripHeader tryTriStripHeader;	
	
	CGCNMatrixLoader	mtxLoader;

	u32 u32HeaderPos;

	// get rough count of display list components so we can guess as to the size of the display list
	u32 numDLComponents = 0;

	// add to the pools to determine the size (can handle 8 bit indicies)
	ts_VertexWeightingBones *boneIndices = new ts_VertexWeightingBones[totalStrips];

	int stripIndex;
	for (stripIndex=0;stripIndex<totalStrips;stripIndex++)
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

		int materialIndex = pMaterialIndices[pFaceIndices[0]];
		TupArray<UVTransform> &uvTransforms = m_UVTransformArray[materialIndex];
		int numLayers = uvTransforms.GetSize();

		memset( &boneIndices[stripIndex], 0, sizeof(ts_VertexWeightingBones));
		if (m_pBoneBuilder->GetNumBones())
		{
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
					if (boneIndex==boneIndices[stripIndex].u8BoneIndex[boneStripIndex])
						break;
				}
				// if we didn't find it
				if (boneStripIndex==numBonesInStrip)
				{
					assert(numBonesInStrip<4); // only can hold 4
					boneIndices[stripIndex].u8BoneIndex[numBonesInStrip] = boneIndex;
					numBonesInStrip++;
				}
			}
			// if more than one bone in the strip
			if( numBonesInStrip > 1)
			{
				// sort bone indices. this is so that we can match bone indices between patches that use same bones but in 
				// a different order 
				for (int boneStripIndex = 1; boneStripIndex < numBonesInStrip; boneStripIndex++)
				{
					int sortIndex = boneStripIndex;
					while ((sortIndex > 0) && (boneIndices[stripIndex].u8BoneIndex[sortIndex] < boneIndices[stripIndex].u8BoneIndex[sortIndex - 1]))
					{
						u8 u8Temp = boneIndices[stripIndex].u8BoneIndex[sortIndex];
						boneIndices[stripIndex].u8BoneIndex[sortIndex] = boneIndices[stripIndex].u8BoneIndex[sortIndex - 1];
						boneIndices[stripIndex].u8BoneIndex[sortIndex - 1] = u8Temp;
						sortIndex--;
					}
				}
				for (boneStripIndex = 0; boneStripIndex < numBonesInStrip - 1; boneStripIndex++)
				{
					assert(boneIndices[stripIndex].u8BoneIndex[boneStripIndex] < boneIndices[stripIndex].u8BoneIndex[boneStripIndex + 1]);	// double-check sort
				}
			}

			mtxLoader.AddBoneIndices( stripIndex, boneIndices + stripIndex);
		}
		
		// add entries to pools
		for (int pointIndex=0;pointIndex<numFacePointIndices;pointIndex++)
		{
			int faceIndex = pFaceIndices[pointIndex];
			int facePointIndex = pFacePointIndices[pointIndex];

			// positionPool
			posPool.Add(pPositionVerts[pPositionIndices[(faceIndex<<1)+faceIndex+facePointIndex]]);
			numDLComponents++;

			if( pSubMaterialHasEnvironmentMapValues[materialIndex])	// environment mapping
			{
				for( int iLayer = 0; iLayer < numLayers; iLayer++)
				{
					numDLComponents++;
				}
			}
			
			if( bHasNormals || pSubMaterialHasEnvironmentMapValues[materialIndex])	// either due to lighting or environment mapping
			{
				// normal pool
				CompressedNormal normal; 
				normal.Set(pNormalVerts[pNormalIndices[(faceIndex<<1)+faceIndex+facePointIndex]]);
				normalPool.Add(normal);
				numDLComponents++;
			}

			// uv pool
			for (int layerIndex=0;layerIndex<numLayers;layerIndex++)
			{
				UVTransform &uvTransform = uvTransforms[layerIndex];
				int mapChannel = uvTransform.m_mapChannel;

				int uvMapIndex = uvChannelToIndexKeeper.GetIndex(mapChannel);
				Vector3 tempVert;
				// if this channel exists in the mesh
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
				uvPool.Add(*(Vector2 *)&tempVert);
				numDLComponents++;
			}
		}
	}
	bool bSmallPosPool = (posPool.GetSize()<256); // small pool can handle 
	bool bSmallNormalPool = (normalPool.GetSize()<256);
	bool bSmallUVPool = (uvPool.GetSize()<256);

	GXAttrType	gxatPos = ( bSmallPosPool) ? GX_INDEX8 : GX_INDEX16;
	GXAttrType	gxatNorm = ( bSmallNormalPool) ? GX_INDEX8 : GX_INDEX16;
	GXAttrType	gxatUV = ( bSmallUVPool) ? GX_INDEX8 : GX_INDEX16;

	GDLObj	gdlObj;

	u32		dlEstLength = numDLComponents * sizeof(short) * 100;

	dlEstLength = OSRoundUp32B( dlEstLength + 64);

	void		*pDL,
				*pDLBase;
	
	pDLBase = malloc( dlEstLength);

	pDL = (void *)OSRoundUp32B( pDLBase);

	CGXVtxDescList gxVtxDescListCurr,
						gxVtxDescListPrev;

	u8					u8Opacity,
						u8PrevOpacity;

#if 0
	GXCullMode		gxCullMode,
						gxPrevCullMode;
#endif

	bool	bThisStripHasEnvMap = false;
	bool	bPrevStripHasEnvMap = false;

	for (stripIndex=0;stripIndex<totalStrips;stripIndex++)
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

		memset(&tryTriStripHeader, 0, sizeof(ts_GCNTriStripHeader));
		tryTriStripHeader.u32ID = stripID++;

		/* count points, do some checking */
		tryTriStripHeader.u16VertexCount = numFacePointIndices;
		tryTriStripHeader.u16TriangleCount = numFacePointIndices-2;

		/* finish filling in header */

		tryTriStripHeader.u32BoneIndex = *(u32 *)&boneIndices[stripIndex];

		int materialIndex = pMaterialIndices[pFaceIndices[0]];
		TupArray<UVTransform> &uvTransforms = m_UVTransformArray[materialIndex];
		ts_FullTextureContext &textureContext = m_textureContextArray[materialIndex];
		int numLayers = uvTransforms.GetSize();

		tryTriStripHeader.u32NextOffset = 0;

		// set the normal flag
		if (bHasNormals || pSubMaterialHasEnvironmentMapValues[materialIndex])
		{
			tryTriStripHeader.u8Flags |= TRI_STRIP_HAS_NORMALS;
			m_modelSOMFlags |= SOMM_TRI_STRIP_NORMALS;
			// if using environment mapping
			if (pSubMaterialHasEnvironmentMapValues[materialIndex])
			{
				tryTriStripHeader.u8Flags |= TRI_STRIP_ENVIRONMENT_MAP;
				m_modelSOMFlags |= SOMM_NEEDS_ENV_MAPPING;
			}
		}

		// set the texture context ID
		// if we have an array of ids for the mesh
		assert(m_textureContextIDArray.GetSize());
		assert(materialIndex<m_textureContextIDArray.GetSize());
		tryTriStripHeader.u32TextureContextIndex = m_textureContextIDArray[materialIndex];

		// deal with the material flags
		assert(m_materialFlagsArray.GetSize());
		assert(materialIndex<m_materialFlagsArray.GetSize());
		int materialFlags = m_materialFlagsArray[materialIndex];
		// don't do much with this right now

		//GC assume clockwise vertices are frontface, we generate counterclockwise vertices,
		//so our normal mode is to cull front faces;
//		gxCullMode = GX_CULL_FRONT;
		if (pFaceFlags[pFaceIndices[0]] & TupTriMesh::FLAGS_FACE_TWO_SIDED)
		{
			tryTriStripHeader.u8Flags |= TRI_STRIP_TWO_SIDED;
//			gxCullMode = GX_CULL_NONE;
		}

		if (pFaceFlags[pFaceIndices[0]] & TupTriMesh::FLAGS_FACE_NO_Z_WRITE)
		{
			// TriStripHeader.u8Flags |= TRI_STRIP_ZWRITE_OFF;
			// m_modelSOMFlags |= SOMM_MIXED_ZWRITE;
		}


		// if marked to receive shadows
		if (!(pFaceFlags[pFaceIndices[0]] & TupTriMesh::FLAGS_FACE_NO_RECEIVE_SHADOWS))
		{
			tryTriStripHeader.u8Flags |= TRI_STRIP_GCN_RECEIVE_SHADOWS;
		}

		// set strip opacities
		u8Opacity = tryTriStripHeader.u8Opacity = (u8)( pSubMaterialEnvironmentMapOpacityValues[materialIndex] * 100.0f);

		assert(m_UVTransformArray.GetSize());
		assert(materialIndex<m_UVTransformArray.GetSize());

		if (stripIndex==0) // if first strip
		{
			/* remember location, output header */
			u32HeaderPos = ftell(pDBLFile);
			DBLWrite::WriteRecord(&tryTriStripHeader, pDBLFile);

			baseTriStripHeader = tryTriStripHeader; // we are now the base

			GDInitGDLObj( &gdlObj, pDL, dlEstLength);
			GDSetCurrent( &gdlObj);
			GDSetOverflowCallback( DLOverflowCallback);

			gxVtxDescListCurr.Init( );
			if( m_modelSOMFlags & SOMM_HAS_BONES)
			{
				gxVtxDescListCurr.Add( GX_VA_PNMTXIDX, GX_DIRECT);
			}

			if( pSubMaterialHasEnvironmentMapValues[materialIndex])	// environment mapping
			{
				for( int iLayer = 0; iLayer < numLayers; iLayer++)
				{
					gxVtxDescListCurr.Add( (GXAttr)( GX_VA_TEX0MTXIDX + iLayer), GX_DIRECT);
				}
			}

			gxVtxDescListCurr.Add( GX_VA_POS, gxatPos);
			
			if( bHasNormals || pSubMaterialHasEnvironmentMapValues[materialIndex])	// either due to lighting or environment mapping
			{
				gxVtxDescListCurr.Add( GX_VA_NRM, gxatNorm);
			}
			
			if( !bModelsOnly)
			{
				gxVtxDescListCurr.Add( GX_VA_CLR0, GX_INDEX16);
			}

			for( int iLayer = 0, iUVSet = 0; iLayer < numLayers; iLayer++)
			{
				if( !( textureContext.TextureLayer[iLayer].u8Flags & TEXTURE_IS_ENVIRONMENT_MAP))
				{
					gxVtxDescListCurr.Add( (GXAttr)(GX_VA_TEX0 + iUVSet), gxatUV);
					iUVSet++;
				}
			}
			GDSetVtxDescv( gxVtxDescListCurr);
			gxVtxDescListPrev = gxVtxDescListCurr;

			GXColor	gxColor;
			gxColor.r = 0xff;
			gxColor.g = 0xff;
			gxColor.b = 0xff;
			gxColor.a = (u8)( (float)u8Opacity * 2.55f);

			GDSetTevKColor( GX_KCOLOR1, gxColor);
			u8PrevOpacity = u8Opacity;

#if 0
			GDSetCullMode( gxCullMode);
			gxPrevCullMode = gxCullMode;
#endif

			if( m_modelSOMFlags & SOMM_HAS_BONES)
			{
				mtxLoader.InitMtxSetLoad( totalStrips);
			}
		}
		else
		if( !CompareTriStripHeaders(tryTriStripHeader,baseTriStripHeader)) // if new strip is different than base
		{
			m_modelTriStripCount++;

			GDPadCurr32( );

			u32 u32DLLength = GDGetCurrOffset( );
			ASSERT( ( u32DLLength & 0x1F) == 0);

			DBLWrite::WriteData( pDL, u32DLLength, pDBLFile);

			baseTriStripHeader.u32DisplayListSize = u32DLLength;
			baseTriStripHeader.u32NextOffset = ftell(pDBLFile) - u32HeaderPos;

			// go back and write the base header (need a new base)
			fseek(pDBLFile, u32HeaderPos, SEEK_SET);
			DBLWrite::WriteRecord(&baseTriStripHeader, pDBLFile);
			fseek(pDBLFile, 0, SEEK_END);	// back to end of file

			baseTriStripHeader = tryTriStripHeader; // we are now the base

			GDInitGDLObj( &gdlObj, pDL, dlEstLength);
			GDSetCurrent( &gdlObj);
			GDSetOverflowCallback( DLOverflowCallback);

			u32HeaderPos = ftell(pDBLFile);
			DBLWrite::WriteRecord(&baseTriStripHeader, pDBLFile);
		}
		else // if new strip is the same as the base
		{
			baseTriStripHeader.u16VertexCount += tryTriStripHeader.u16VertexCount;
			baseTriStripHeader.u16TriangleCount += tryTriStripHeader.u16TriangleCount;
		}

		bThisStripHasEnvMap = false;
		if( pSubMaterialHasEnvironmentMapValues[materialIndex])	// environment mapping
		{
			bThisStripHasEnvMap = true;
			for( int iLayer = 1; iLayer < numLayers; iLayer++)
			{
				if( ( textureContext.TextureLayer[iLayer].u8Flags & TEXTURE_IS_ENVIRONMENT_MAP))
				{
					if( m_modelSOMFlags & SOMM_HAS_BONES)
					{
						GDLoadTexMtxIndx( boneIndices[stripIndex].u8BoneIndex[0], gxTexMtxID[iLayer], GX_MTX3x4);
					}
					else
					{
						GDLoadTexMtxIndx( 0, gxTexMtxID[iLayer], GX_MTX3x4);
					}
				}
			}
		}
		
		if( bPrevStripHasEnvMap && !bThisStripHasEnvMap)
		{
			GDSetCurrentMtx(	GX_PNMTX0,
									GX_TEXMTX0,
									GX_IDENTITY,
									GX_IDENTITY,
									GX_IDENTITY,
									GX_IDENTITY,
									GX_IDENTITY,
									GX_IDENTITY,
									GX_IDENTITY);
		}
		bPrevStripHasEnvMap = bThisStripHasEnvMap;

		gxVtxDescListCurr.Init( );
		
		if( m_modelSOMFlags & SOMM_HAS_BONES)
		{
			gxVtxDescListCurr.Add( GX_VA_PNMTXIDX, GX_DIRECT);
		}
		
		if( pSubMaterialHasEnvironmentMapValues[materialIndex])	// environment mapping
		{
			for( int iLayer = 0; iLayer < numLayers; iLayer++)
			{
				gxVtxDescListCurr.Add( (GXAttr)( GX_VA_TEX0MTXIDX + iLayer), GX_DIRECT);
			}
		}
		
		gxVtxDescListCurr.Add( GX_VA_POS, gxatPos);
		
		if( bHasNormals || pSubMaterialHasEnvironmentMapValues[materialIndex])	// either due to lighting or environment mapping
		{
			gxVtxDescListCurr.Add( GX_VA_NRM, gxatNorm);
		}
		
		if( !bModelsOnly)
		{
			gxVtxDescListCurr.Add( GX_VA_CLR0, GX_INDEX16);
		}
		
		for( int iLayer = 0, iUVSet = 0; iLayer < numLayers; iLayer++)
		{
			if( !( textureContext.TextureLayer[iLayer].u8Flags & TEXTURE_IS_ENVIRONMENT_MAP))
			{
				gxVtxDescListCurr.Add( (GXAttr)(GX_VA_TEX0 + iUVSet), gxatUV);
				iUVSet++;
			}
		}

		if( gxVtxDescListCurr != gxVtxDescListPrev)
		{
			GDSetVtxDescv( gxVtxDescListCurr);
			gxVtxDescListPrev = gxVtxDescListCurr;
		}

		if( u8Opacity != u8PrevOpacity)
		{
			GXColor	gxColor;
			gxColor.r = 0xff;
			gxColor.g = 0xff;
			gxColor.b = 0xff;
			gxColor.a = (u8)( (float)u8Opacity * 2.55f);

			GDSetTevKColor( GX_KCOLOR1, gxColor);
			u8PrevOpacity = u8Opacity;
		}

#if 0
		if( gxCullMode != gxPrevCullMode)
		{
			GDSetCullMode( gxCullMode);
			gxPrevCullMode = gxCullMode;
		}
#endif

		u8		u8Data;
		u16	u16Data;

		if( m_modelSOMFlags & SOMM_HAS_BONES)
		{
			mtxLoader.CheckStripIndex( stripIndex);
		}

		// output tri strip op code
		if( bFoundAlpha)
		{
			GDBegin( GX_TRIANGLESTRIP, GX_VTXFMT1, numFacePointIndices); //GX_VTXFMT1 has 32-bit color
		}
		else
		{
			GDBegin( GX_TRIANGLESTRIP, GX_VTXFMT3, numFacePointIndices); //GX_VTXFMT3 has 16-bit color
		}

		// output tristrip info
		for( int pointIndex=0;pointIndex<numFacePointIndices;pointIndex++)
		{
			int faceIndex = pFaceIndices[pointIndex];
			int facePointIndex = pFacePointIndices[pointIndex];

			// make sure this mesh has bones (otherwize no matrix index is outputted)
			if (m_modelSOMFlags & SOMM_HAS_BONES)
			{
				// output position matrix index 
				// if the strip is using more than 1 bone
				const int *pBoneIndices = m_pBoneBuilder->GetBoneVerts().GetData();
				int vertexIndex = pPositionIndices[(faceIndex<<1)+faceIndex+facePointIndex];
				int boneIndex = pBoneIndices[vertexIndex];

				u8Data = mtxLoader.GetGCNMtxIdx( boneIndex);
				GDMatrixIndex1x8( u8Data);
			}

			if( pSubMaterialHasEnvironmentMapValues[materialIndex])	// environment mapping
			{
				GDMatrixIndex1x8( GX_TEXMTX0);
				for( int iLayer = 1; iLayer < numLayers; iLayer++)
				{
					if( textureContext.TextureLayer[iLayer].u8Flags & TEXTURE_IS_ENVIRONMENT_MAP)
					{
						GDMatrixIndex1x8( (u8)gxTexMtxID[iLayer]);
					}
					else
					{
						GDMatrixIndex1x8( GX_IDENTITY);
					}
				}
			}
			
			// output position index 
			u16Data = posPool.GetIndex(pPositionVerts[pPositionIndices[(faceIndex<<1)+faceIndex+facePointIndex]]);
			assert(u16Data!=-1);
			if (bSmallPosPool)
			{
				assert(u16Data<255);
				u8Data = (u8)u16Data;
				GDPosition1x8( u8Data);
			}
			else
			{
				GDPosition1x16( u16Data);
			}

			if( bHasNormals || pSubMaterialHasEnvironmentMapValues[materialIndex])	// either due to lighting or environment mapping
			{
				/* output normal index */
				CompressedNormal normal; 
				normal.Set(pNormalVerts[pNormalIndices[(faceIndex<<1)+faceIndex+facePointIndex]]);
				u16Data = normalPool.Add(normal);
				assert(u16Data!=-1);
				if (bSmallNormalPool)
				{
					assert(u16Data<255);
					u8Data = (u8)u16Data;
					GDNormal1x8( u8Data);
				}
				else
				{
					GDNormal1x16( u16Data);
				}
			}
			// output color indicies
			// only when we have a color set
			if( bHasVertexColors)
			{
				if( nNumColorSets == 0)
				{
					u16Data = 0;
				}
				else
				{
					// use condensed pool
					if (bUseConsolidatedColors)
					{
						u16Data = consolidatedColorIndices[nRunningPointCount];
					}
					else
					{
						// not consolidated (use point counter)
						u16Data = nRunningPointCount;	// output running color index
					}
					nRunningPointCount++;
				}
				GDColor1x16( u16Data);
			}

			// output texture coordinates
			for( int layerIndex = 0; layerIndex < numLayers; layerIndex++)
			{
				if( textureContext.TextureLayer[layerIndex].u8Flags & TEXTURE_IS_ENVIRONMENT_MAP)
				{
					continue;
				}

				UVTransform &uvTransform = uvTransforms[layerIndex];
				int mapChannel = uvTransform.m_mapChannel;

				int uvMapIndex = uvChannelToIndexKeeper.GetIndex(mapChannel);
				Vector3 tempVert;
				// if this channel exists in the mesh
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
				// get the index from the keeper
				u16Data = uvPool.GetIndex(*(Vector2 *)&tempVert);
				assert(u16Data!=-1);
				if (bSmallUVPool)
				{
					assert(u16Data<255);
					u8Data = (u8)u16Data;
					GDTexCoord1x8( u8Data);
				}
				else
				{
					GDTexCoord1x16( u16Data);
				}
			}
		}
		// if last strip
		if (stripIndex+1==totalStrips)
		{
			m_modelTriStripCount++;

			if( ( m_modelSOMFlags & SOMM_HAS_BONES) || bPrevStripHasEnvMap)
			{
				GDSetCurrentMtx(	GX_PNMTX0,
										GX_TEXMTX0,
										GX_IDENTITY,
										GX_IDENTITY,
										GX_IDENTITY,
										GX_IDENTITY,
										GX_IDENTITY,
										GX_IDENTITY,
										GX_IDENTITY);
			}
			// pad out the display list with null
			GDPadCurr32( );

			u32 u32DLLength = GDGetCurrOffset( );
			ASSERT( ( u32DLLength & 0x1F) == 0);

			DBLWrite::WriteData( pDL, u32DLLength, pDBLFile);

			baseTriStripHeader.u32DisplayListSize = u32DLLength;

			// go back and write the base header (need a new base)
			fseek(pDBLFile, u32HeaderPos, SEEK_SET);
			DBLWrite::WriteRecord(&baseTriStripHeader, pDBLFile);
			fseek(pDBLFile, 0, SEEK_END);	// back to end of file
		}
		m_modelTriStripPointCount+=numFacePointIndices;
		m_modelTriStripFaceCount+=numFacePointIndices-2;
		m_modelTriStripVertexColorCount+=numFacePointIndices;
	}

	free( pDLBase);
	delete [] boneIndices;

	DBLUtil::AlignFile();
	/* finish data size statistic */
	m_totalStripDataSize = ftell(pDBLFile) - m_totalStripDataSize;

	DBLUtil::AlignChunkData32Byte( );
	m_modelPositionFileOffset = ftell(pDBLFile) - m_modelHeaderFilePos;
	m_modelPositionCount = posPool.GetSize();
	for( int posIndex = 0; posIndex < m_modelPositionCount; posIndex++)
	{
		DBLWrite::WriteRecord(&posPool[posIndex],pDBLFile); // 3 floats (used to be 4)
	}

	DBLUtil::AlignChunkData32Byte( );
	m_modelNormalFileOffset = ftell(pDBLFile) - m_modelHeaderFilePos;
	m_modelNormalCount = normalPool.GetSize();
	for( int normalIndex = 0; normalIndex < m_modelNormalCount; normalIndex++)
	{
		CompressedNormal &normal = normalPool[normalIndex];
		int normalSize = sizeof(CompressedNormal);
		DBLWrite::WriteRecord(&normal.x,pDBLFile);
		DBLWrite::WriteRecord(&normal.y,pDBLFile);
		DBLWrite::WriteRecord(&normal.z,pDBLFile);
	}

	DBLUtil::AlignChunkData32Byte( );
	m_modelUVFileOffset = ftell(pDBLFile) - m_modelHeaderFilePos;
	int uvPoolSize = uvPool.GetSize();
	if (uvPoolSize)
	{
		for( int uvIndex = 0; uvIndex < uvPoolSize; uvIndex++)
		{
			DBLWrite::WriteRecord(&uvPool[uvIndex],pDBLFile);
		}
		m_modelUVCount = uvPoolSize;
	}
	else
	{
		Vector2 v2;
		v2.Set( 0.0f, 0.0f );
//		v2.x = 0.0f;
//		v2.y = 0.0f;
		DBLWrite::WriteRecord(&v2,pDBLFile);
		m_modelUVCount = 1;
	}
	DBLUtil::AlignChunkData32Byte( );
}

// this creates an array of ints and fills it with the proper texture context ids for the mesh
void ExportGCNSOM::BuildTextureContextIDs(int materialReference)
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
void ExportGCNSOM::BuildUVTransformArray(int materialRef)
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
	}
}

// this creates an array of ints and fills it with the proper texture context ids for the mesh
void ExportGCNSOM::BuildTextureContexts(int materialRef)
{
	BuildTextureContextManager buildTextureContextManager(m_dataManager,m_tupMaterialPool,m_tupMapPool,m_tupImagePool,m_currentPartition,true);
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
			int totalTextureContexts = tupMultiSubObjectMaterial.GetNumSubMaterials();
			if (totalTextureContexts)
			{
				m_textureContextArray.SetSize(totalTextureContexts);
				for (int materialIndex = 0;materialIndex<totalTextureContexts;materialIndex++)
				{
					TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(materialIndex);
					TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
					int materialPoolIndex;
					tupSubMaterial.GetMaterialReference(materialPoolIndex);
					if (materialPoolIndex!=-1)
					{
						TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial( materialPoolIndex );
						TupMaterial tupSubMat( pMaterialAgg );
						buildTextureContextManager.BuildTextureContext(tupSubMat,m_textureContextArray[materialIndex]);
					}
					else
					{
						buildTextureContextManager.BuildDummyTextureContext(m_textureContextArray[materialIndex]);
					}
				}
			}
			else
			{
				// just deal with 1 material
				m_textureContextArray.SetSize(1);
				buildTextureContextManager.BuildDummyTextureContext(m_textureContextArray[0]);
			}
		}
		else
		{
			// just deal with 1 material
			m_textureContextArray.SetSize(1);
			buildTextureContextManager.BuildTextureContext(tupMat,m_textureContextArray[0]);
		}
	}
	else
	{
		m_textureContextArray.SetSize(1);
		buildTextureContextManager.BuildDummyTextureContext(m_textureContextArray[0]);
	}
}

void ExportGCNSOM::BuildFlagsArray(int materialRef)
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

void ExportGCNSOM::WriteBoneData(void)
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
