/*************************************/
/*                                   */
/*   som_load.c                      */
/*   avalanche software              */
/*   originally by joe barnes        */
/*   visual c++ 6.0                  */
/*   load SOM's from a DBL file      */
/*                                   */
/*************************************/

/******************* includes ****************************/

#include "Game/GamePCH.h"


#ifdef GCN
#include "Display/GCN/Shader.h"
#endif

#ifdef DIRECTX
#include "Display/DirectX/DXPublished.h"
#include "Display/DirectX/PatchHelper.h"
#endif

/******************* defines *****************************/

#define WSWAP(a) ((((a)&0xff)<<8)|(((a)>>8)&0xff))
#define LSWAP(a) ((WSWAP(((a)>>16)&0xffff))|(WSWAP(((a)&0xffff))<<16))

/******************* macros ******************************/

/******************* structures **************************/

/******************* externs *****************************/

/******************* global variables ********************/

/******************* local variables *********************/

/******************* local prototypes ********************/

#ifdef GCN
void _FixGCNSOM(ts_PatchSOM *pSOM, ts_RecordList *pTextureContextList, ts_RecordList *pWorldTextureContextList);
void _FixGCNSOM2(ts_PatchSOM *pSOM, ts_RecordList *pTextureContextList, ts_RecordList *pWorldTextureContextList);
#endif //GCN

void _FixDMASOM(ts_PatchSOM *pSOM, ts_RecordList *pTextureContextList, ts_RecordList *pWorldTextureContextList);

#ifdef PS2
u_int PS2RendererTextureBindCallback(u_int textureID, u_int textureLayer);
#endif //PS2

void _FixMeshSOM(void *pSOM, ts_RecordList *pTextureContextList, ts_RecordList *pWorldTextureContextList);
void _FixSOM(ts_PatchSOM *pSOM, ts_RecordList *pTextureContextList, ts_RecordList *pWorldTextureContextList, bool bSOMArray);

#ifdef WIN32	// only intended for use by terraintoola and obbworld
void _FixCollisionSOM(ts_CollisionSOM *pCollisionSOM);
#endif //WIN32

void _FixBones( ts_ModelBone *pRoot, ts_ModelBone *pBone, ts_ModelBone *pParent, /*Graphics4x4 *pMatrix,*/ int iBoneCount, BoneGroupBitMask *boneGroupBits);
void *_GetSOMEnd(ts_PatchSOM *pSOM);
unsigned short _CountGroupBits( BoneGroupBitMask boneGroupBits );

/******************* functions ***************************/

bool LoadSingleSOM( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize)
{
	/* chunk data is a single SOM */

	ts_PatchSOM *pSOM = (ts_PatchSOM *) pData;

	/* point to texture context chunk for this database */

   ts_RecordList *pTextureContextList = (ts_RecordList *) dblGetChunkDataCurrentDBL(DB_TEXTURE_CONTEXT, dblGetChunkName(pChunkHdr));
   ts_RecordList *pWorldTextureContextList = NULL;
	
	if( g_loadSOM.GetWorldHandle( ) != INVALID_HANDLE)
	{
		pWorldTextureContextList = (ts_RecordList *) dblGetChunkData( DB_TEXTURE_CONTEXT, dblGetChunkName(pChunkHdr), g_loadSOM.GetWorldHandle( ));
	}


	/* fix up SOM */

	switch(pSOM->dh.u8DataType)
	{
#ifdef GCN
	case DATA_GCN_SOM:
		_FixGCNSOM(pSOM, pTextureContextList, pWorldTextureContextList);
		break;
	case DATA_GCN_SOM2:
		_FixGCNSOM2(pSOM, pTextureContextList, pWorldTextureContextList);
		break;
#endif //GCN
	case DATA_PS2_DMA_SOM:
		_FixDMASOM(pSOM, pTextureContextList, pWorldTextureContextList);
		break;
	case DATA_MULTI_SOM:
		_FixMeshSOM(pSOM, pTextureContextList, pWorldTextureContextList);
		break;
	default:
		_FixSOM(pSOM, pTextureContextList, pWorldTextureContextList, false);
		break;
	}

	/* read bone data, if any */

	if (pSOM->dh.u16Flags & SOMM_HAS_BONES)
		ReadBoneData(pSOM, (ts_ModelBone *) _GetSOMEnd(pSOM));

	/* this is permanent data */

	pChunkHdr->u16Flags &= ~(DBL_FLUSHABLE);
	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	return TRUE;
}

bool LoadSOMArray( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize)
{
	ts_PatchSOM *pLastSOM = NULL;

	/* point to chunk data */

	ts_RecordList *pSOMList = (ts_RecordList *) pData;
	ResolveRecordList(pSOMList);

	/* point to texture context chunk for this database */

   ts_RecordList *pTextureContextList = (ts_RecordList *) dblGetChunkDataCurrentDBL(DB_TEXTURE_CONTEXT, dblGetChunkName(pChunkHdr));
   ts_RecordList *pWorldTextureContextList = NULL;
	
	if( g_loadSOM.GetWorldHandle( ) != INVALID_HANDLE)
	{
		pWorldTextureContextList = (ts_RecordList *) dblGetChunkData( DB_TEXTURE_CONTEXT, dblGetChunkName(pChunkHdr), g_loadSOM.GetWorldHandle( ));
	}

	/* we use a DB_SOMARRAYBONE chunk for handling LOD models for things like riders. each model is required to */
	/* have the same skeleton. bone data for first SOM will allocate memory for the final computer matrices, */
	/* subsequent models will point to that same memory so that they all share the final bone matrices. in the future, */
	/* the subsequent models will not have any bone data, and the bone-to-vertex translation will be attached to the */
	/* SOM instead of the bone data */

	/* fix up each SOM */

	for (int i = 0; i < pSOMList->usRecordCount; i++)
	{
		ts_PatchSOM *pSOM = (ts_PatchSOM *) GetRecordAddress(pSOMList, i);
		switch(pSOM->dh.u8DataType)
		{
#ifdef GCN
		case DATA_GCN_SOM:
			_FixGCNSOM( pSOM, pTextureContextList, pWorldTextureContextList);
			break;
		case DATA_GCN_SOM2:
			_FixGCNSOM2( pSOM, pTextureContextList, pWorldTextureContextList);
			break;
#endif //GCN
		case DATA_PS2_DMA_SOM:
			_FixDMASOM(pSOM, pTextureContextList, pWorldTextureContextList);
			break;
		case DATA_MULTI_SOM:
			_FixMeshSOM(pSOM, pTextureContextList, pWorldTextureContextList);
			break;
		default:
			_FixSOM( pSOM, pTextureContextList, pWorldTextureContextList, true);
			break;
		}

		/* read bone data, if any. point any LOD model's matrix arrays back to the highest LOD's bone data */

		if ((pSOM->dh.u16Flags & SOMM_HAS_BONES) && !(pSOM->dh.u16Flags & SOMM_BONES_SLAVE))
		{
			/* this model has bones, process them */

			if ((pSOM->dh.u16Flags & SOMM_IS_LOD) == 0)
			{
				ReadBoneData(pSOM, (ts_ModelBone *) _GetSOMEnd(pSOM));
				pLastSOM = pSOM; //assumes LOD's are processed immediately after LOD master
			}
			else
			{		
				/* LOD's use master's bones */

				ASSERT(pLastSOM);
				pSOM->pBody = pLastSOM->pBody;
			}
		}
	}

	/* this is permanent data */

	pChunkHdr->u16Flags &= ~(DBL_FLUSHABLE);
	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	return(TRUE);
}

// load the actual textures this som needs, instead of all textures (which is the default
// if this chunk doesn't exist)
bool LoadSOMTextures( void* pData, ts_DBChunkHdr* pChunkHdr, void** ppRetData, u32* pu32DataSize )
{
	ts_RecordList* pSOMTextureList = (ts_RecordList*) pData;
	ResolveRecordList( pSOMTextureList );

	// find the SOM array chunk in the file before us
	ts_RecordList* pSOMList;
	pSOMList = (ts_RecordList*) dblGetChunkDataCurrentDBL( DB_SOMARRAY, dblGetChunkName( pChunkHdr ) );
	if ( !pSOMList )
		pSOMList = (ts_RecordList*) dblGetChunkDataCurrentDBL( DB_SOMARRAYBONE, dblGetChunkName( pChunkHdr ) );
	#ifdef WIN32	// for terraintoola and obbworld
	if ( !pSOMList )
		pSOMList = (ts_RecordList*) dblGetChunkDataCurrentDBL( DB_COLLISIONSOMARRAY, dblGetChunkName( pChunkHdr ) );
	#endif //WIN32
	if ( !pSOMList )
		pSOMList = (ts_RecordList*) dblGetChunkDataCurrentDBL( DB_DMASOMARRAY, dblGetChunkName( pChunkHdr ) );
	ASSERT_PTR( pSOMList );

	// find the texture database in this DBL
	TextureDatabase* tdb;
	tdb = (TextureDatabase*) dblGetChunkDataCurrentDBL( DB_TEXTURE_SET, dblGetChunkName( pChunkHdr ) );
	if ( !tdb )
		tdb = (TextureDatabase*) dblGetChunkDataCurrentDBL( DB_MERGED_TEXTURE_SET, dblGetChunkName( pChunkHdr ) );
	if ( !tdb )
		return false;

	// attach the textures to each SOM.  Clear their defaults that was filled in
	// earlier on only attach the textures that are needed.

	TextureMgr::Cache().Begin();

	int i;
	for ( i = 0 ; i < pSOMTextureList->usRecordCount ; i++ )
	{
		int* usedTextureList = (int*) GetRecordAddress( pSOMTextureList, i );
		ts_PatchSOM* SOM  = (ts_PatchSOM*) GetRecordAddress( pSOMList, i );
		ASSERT_PTR( usedTextureList );
		ASSERT_PTR( SOM );

		// get the texture array pointer from the som
		TextureArray* textureArray = NULL;
		if ( SOM->dh.u8DataType == DATA_MULTI_SOM )
			textureArray = ( ( SMeshSOM* )SOM )->d_textureArray;
		else
			textureArray = SOM->pTextureArray;
		ASSERT_PTR( textureArray );

		// add the used set of textures to the array
		int usedTextureCount = *usedTextureList++;
		for ( int j = 0 ; j < usedTextureCount ; j++ )
		{
			int textureNum = usedTextureList[ j ];
			ASSERT( textureNum < tdb->TextureCount() );

			// HACK!!!! HACK!!!! HACK!!!!  This assumes that tdb->GetTexture(i)
			// is the same texture as pDBLTextureSet->TextureChunkRecord[i]
			textureArray->AddTexture( tdb->GetTexture( textureNum ) );
		}

	 	textureArray->AddTexture( TextureMgr::GetWhiteTexture() );

		// cache it up
		TextureMgr::Cache().Add( textureArray );
	}

	// optimize the cache
	TextureMgr::Cache().End();

	// fill in the cache IDs
	for ( i = 0 ; i < pSOMTextureList->usRecordCount ; i++ )
	{
		ts_PatchSOM* SOM  = (ts_PatchSOM*) GetRecordAddress( pSOMList, i );

		// get the texture array pointer from the som
		TextureArray* textureArray = NULL;
		if ( SOM->dh.u8DataType == DATA_MULTI_SOM )
			textureArray = ( ( SMeshSOM* )SOM )->d_textureArray;
		else
			textureArray = SOM->pTextureArray;
		ASSERT_PTR( textureArray );

		textureArray->SetCacheID( TextureMgr::Cache().GetID( textureArray ) );
	}

	return true;
}

/* this chunk type is strictly for collision data generation by terraintoola or obbworld */

#ifdef WIN32	// only intended for use by terraintoola and obbworld
bool LoadCollisionSOMArray( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize)
{
	/* point to chunk data */

	ts_RecordList *pSOMList = (ts_RecordList *) pData;
	ResolveRecordList(pSOMList);

	/* fix up each SOM */

	for (int i = 0; i < pSOMList->usRecordCount; i++)
	{
		ts_CollisionSOM *pCollisionSOM = (ts_CollisionSOM *) GetRecordAddress(pSOMList, i);
		_FixCollisionSOM(pCollisionSOM);
	}

	/* this is permanent data */

	pChunkHdr->u16Flags &= ~(DBL_FLUSHABLE);
	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	return(TRUE);
}
#endif //WIN32

/* load in the info that allows bone-to-group number mapping and left-right bone name mapping */

bool LoadBoneInfo( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize)
{
	/* point to chunk data */

	ts_BoneInfo *pBoneInfo = (ts_BoneInfo *) pData;

	/* fix BoneInfo pointers */

	pBoneInfo->pGroupBoneMap = (ts_GroupBoneMap *)((uint)pBoneInfo + sizeof(ts_BoneInfo));
	pBoneInfo->pLeftRightMap = (ts_LeftRightMap *)((uint)pBoneInfo->pGroupBoneMap + sizeof(ts_GroupBoneMap) * pBoneInfo->numGBEntries);

	/* ned says: this data chunk is based on the file AvalancheToolSet\FilterChain\JuJu\BoneGroupTable.TUP, and is the */
	/* same for all boned .dbl's. so only keep the first one and throw away the rest. the intention is to get rid of the */
	/* DB_BONEINFO chunk for all .dbl files except a special one that only contains that chunk, but this code will be */
	/* valid in that situation as well */

	static bool bLoaded = false;
	if (bLoaded)
	{
#ifdef _DEBUG
#if 0
		/* check to make sure it's the same */

		ts_BoneInfo *pFirstBoneInfo = (ts_BoneInfo *) dblGetChunkDataAllDBLs(DB_BONEINFO, DEFAULT_CHUNK_KEY_STRING, NULL);
		ASSERTS(pFirstBoneInfo, "Can't find bone info data!");

		/* check bone groups */

		int i;
		for (i = 0; i < pBoneInfo->numGBEntries; i++)
		{
			/* find it in first bone info data */

			int j;
			for (j = 0; j < pFirstBoneInfo->numGBEntries; j++)
			{
				if (strcmp(pFirstBoneInfo->pGroupBoneMap[j].cBoneName, pBoneInfo->pGroupBoneMap[i].cBoneName) == 0)
					break;
			}
			if (j == pFirstBoneInfo->numGBEntries)
				ASSERTS(false, "Bone group table does not match!");
			else
				ASSERTS(pFirstBoneInfo->pGroupBoneMap[j].boneGroupID == pBoneInfo->pGroupBoneMap[i].boneGroupID, "Bone group id does not match!");
		}

		/* check left/right mapping */

		for (i = 0; i < pBoneInfo->numLREntries; i++)
		{
			/* find it in first bone info data */

			int j;
			for (j = 0; j < pFirstBoneInfo->numLREntries; j++)
			{
				if (strcmp(pFirstBoneInfo->pLeftRightMap[j].cBoneName1, pBoneInfo->pLeftRightMap[i].cBoneName1) == 0)
					break;
			}
			if (j == pFirstBoneInfo->numLREntries)
				ASSERTS(false, "Bone left/right table does not match!");
			else
				ASSERTS(strcmp(pFirstBoneInfo->pLeftRightMap[j].cBoneName2, pBoneInfo->pLeftRightMap[i].cBoneName2) == 0, "Bone left/right partner does not match!");
		}
#endif //0

#if 0
		/* print them out for visual compraison */

		FileStream *pLog = new FileStream;
		pLog->Open("boneinfo.txt", FileStream::WRITE, true);
		pLog->Print("===first bone info\n");
		for (uint i = 0; i < pFirstBoneInfo->numLREntries; i++)
			pLog->Print("%d %s %s\n", i, pFirstBoneInfo->pLeftRightMap[i].cBoneName1, pFirstBoneInfo->pLeftRightMap[i].cBoneName2);
		for (i = 0; i < pFirstBoneInfo->numGBEntries; i++)
			pLog->Print("%d %s %d\n", i, pFirstBoneInfo->pGroupBoneMap[i].cBoneName, pFirstBoneInfo->pGroupBoneMap[i].boneGroupID);
		pLog->Print("===bone info\n");
		for (i = 0; i < pBoneInfo->numLREntries; i++)
			pLog->Print("%d %s %s\n", i, pBoneInfo->pLeftRightMap[i].cBoneName1, pBoneInfo->pLeftRightMap[i].cBoneName2);
		for (i = 0; i < pBoneInfo->numGBEntries; i++)
			pLog->Print("%d %s %d\n", i, pBoneInfo->pGroupBoneMap[i].cBoneName, pBoneInfo->pGroupBoneMap[i].boneGroupID);
		pLog->Close();
		delete pLog;
#endif //0
#endif //_DEBUG

		/* flush the data */

		pChunkHdr->u16Flags |= DBL_FLUSHABLE;
		*ppRetData = NULL;
		*pu32DataSize = 0;
	}
	else
	{
		/* keep the first one */

		pChunkHdr->u16Flags &= ~(DBL_FLUSHABLE);
		bLoaded = true;
		*ppRetData = pData;
		*pu32DataSize = pChunkHdr->u32Size;
	}

	/* success */

	return(true);
}


/* load in the info that allows bonegroup-to-bonegroup left-right mapping */

bool LoadBoneGroupInfo( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize)
{
	/* point to chunk data */

	ts_BoneGroupInfo *pBoneGroupInfo = (ts_BoneGroupInfo *) pData;

	static bool bLoaded = false;
	if (bLoaded)
	{
		/* flush the data */

		pChunkHdr->u16Flags |= DBL_FLUSHABLE;
		*ppRetData = NULL;
		*pu32DataSize = 0;
	}
	else
	{
		/* keep the first one */

		pChunkHdr->u16Flags &= ~(DBL_FLUSHABLE);
		bLoaded = true;
		*ppRetData = pData;
		*pu32DataSize = pChunkHdr->u32Size;
	}

	/* success */

	return(true);
}

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/

/* read the bone data that immediately follows a SOM, and hook it up to the SOM. */

bool ReadBoneData(ts_PatchSOM *pSOM, ts_ModelBone *pMBones)
{
	int					iBoneCount;
	ts_Body				*pBody;
	BoneGroupBitMask	boneGroupBits = 0;


	iBoneCount = (int) pMBones->boneID;	//first boneID special: contains number of bones
	pMBones->boneID = 0;						//fix so that first bone is numbered 0

//djr: most recent data build from new pipeline filter chain has a boned model with 0 bone count - assume that
//it is a fix to dynamically light models on the PS2 (adds a bogus bone).
//	ASSERT( iBoneCount > 1 );	//single bone obj's are ignored (everything has a design bone - so objects with more than that are considered to be boned)
	if(iBoneCount < 1)
		iBoneCount = 1;

	/* set up a body structure is pointed to by the SOM (for easy fixups of instance bone structures) */
	MEM_SET_ALLOC_FILENAME("Body", pSOM->Name);
	pBody = (ts_Body *) memAlloc(sizeof(ts_Body));
	pBody->usBones = (unsigned short) iBoneCount;
	pBody->pMBones = pMBones;
	pBody->pBones = NULL;
	pBody->pBoneGroupList = NULL;
	pBody->pBoneInfo = (ts_BoneInfo *) dblGetChunkDataAllDBLs(DB_BONEINFO, DEFAULT_CHUNK_KEY_STRING, NULL);
	pBody->pBoneGroupInfo = (ts_BoneGroupInfo *) dblGetChunkDataAllDBLs(DB_BONE_GROUP_MIRRORS, DEFAULT_CHUNK_KEY_STRING, NULL);
	ASSERTS(pBody->pBoneInfo, "Can't find bone info data!");
	pBody->updatePending = false;

	_FixBones( pMBones, pMBones, NULL, iBoneCount, &boneGroupBits);
	
	pBody->usBoneGroups = Math::Max(1, _CountGroupBits( boneGroupBits));//there needs to be at least one default bone group

	pSOM->pBody = pBody;
	return TRUE;
}

#if defined(DIRECTX)
void _FixSOM(ts_PatchSOM *pSOM, ts_RecordList *pTextureContextList, ts_RecordList *pWorldTextureContextList, bool bSOMArray)
{
	/* we are only loading PatchSOM's at this point (that's a misnomer, typically they are tri-strip based) */
	ASSERT(pSOM->dh.u8DataType == DATA_SOM);

#if 0
	/* if the SHIP_IT flag is ever implemented, we shouldn't have names in the data (a SHIP_IT version of the data, though */
	/* i'm not sure how this data would be created (maybe a SHIP_IT version of the filter chain?)... */
	ASSERT((pSOM->dh.u16Flags & SOMM_HAS_NAME) == 0);
#endif

	ASSERT( pSOM->u32PatchOffset == 0 );
	pSOM->pPatchBuffer = NULL;

// 	pSOM->pTextureArray = TextureMgr::GetRecentTextureArray( (u16) pSOM->u32TextureSubSetIndex );
//		pSOM->pTextureArray->AddTexture( TextureMgr::GetWhiteTexture( ) );
	pSOM->pTextureArray = new TextureArray;
//	pSOM->pTextureArray->AddTexture( TextureMgr::GetWhiteTexture() );

	unsigned int meshCount = 0;

	pSOM->DisplayListRecord.pDisplayList = (char *) PatchSOM__BuildMeshData( (char *) pSOM + sizeof( ts_PatchSOM), pTextureContextList, pWorldTextureContextList, meshCount );
	ASSERT_PTR( pSOM->DisplayListRecord.pDisplayList );

	pSOM->DisplayListRecord.u16QuadWordCount = meshCount;
	pSOM->DisplayListRecord.u16PolyCount = 0;

	DXMeshInterface ** ppMeshInts = (DXMeshInterface **) pSOM->DisplayListRecord.pDisplayList;

	for( unsigned int mesh = 0; mesh < meshCount; mesh++ )
	{
		ASSERT_PTR( ppMeshInts[mesh] );

		unsigned int numMaterials = 0;

		StandardTCMaterial ** ppMaterials = (StandardTCMaterial **) ppMeshInts[mesh]->getDefaultMaterials( numMaterials );
		ASSERT( numMaterials );
		ASSERT_PTR( ppMaterials );

		if( ppMaterials  &&  numMaterials )
			for( unsigned int mat = 0; mat < numMaterials; mat++ )
			{
				ASSERT_PTR( ppMaterials[mat] );

				const ts_TextureContext * pTC = ppMaterials[mat]->getTextureContext( );
				ASSERT_PTR( pTC );

				const ts_TextureLayer * pTL = pTC->TextureLayer;
				ASSERT_PTR( pTL );

				for( int tex = 0; tex < pTC->TextureCount; tex++, pTL++ )
				{

					Texture * pTex = pTL->textureDatabase->GetTexture( pTL->s16TextureIndex );
					ASSERT_PTR( pTex );

					if( pTex->GetFlags( ) & Texture::REFLECTS )
					{
						/* add this piece of data to som's miscellaneous data. since the data is just a bind value, */
						/* just pass it directly as the data pointer, instead of copying it off somewhere */
						DataMgr::AddOwner( pSOM, DATA_SOM )->AddData( pTex, SOM_DATA_REFLECT_BIND, 0, 0 );
					}

					/* check for animated textures too */
					if( pTex->GetFlags( ) & Texture::ANIMATED )
					{
						/* add this piece of data to som's miscellaneous data. since the data is just a bind value, */
						/* just pass it directly as the data pointer, instead of copying it off somewhere */
						DataMgr::AddOwner( pSOM, DATA_SOM )->AddData( pTex, SOM_DATA_ANIMATED_TEXTURE, 0, 0 );
					}
				}
			}
		}
}
#endif //DIRECTX

#if !defined(DIRECTX)
void _FixSOM(ts_PatchSOM *pSOM, ts_RecordList *pTextureContextList, ts_RecordList *pWorldTextureContextList, bool bSOMArray)
{
	int i;
	ts_Patch *pPatch;
	ts_TriStripHeader *pTriStripHeader;

#ifdef GCN
	ASSERTF( 0,( "Loaded old format SOM %s\n", pSOM->Name));
#endif

	/* we are only loading PatchSOM's at this point (that's a misnomer, typically they are tri-strip based) */

	ASSERT(pSOM->dh.u8DataType == DATA_SOM);

#if 0
	/* if the SHIP_IT flag is ever implemented, we shouldn't have names in the data (a SHIP_IT version of the data, though */
	/* i'm not sure how this data would be created (maybe a SHIP_IT version of the filter chain?)... */

	ASSERT((pSOM->dh.u16Flags & SOMM_HAS_NAME) == 0);
#endif

	/* if SOM contains patches, set buffer pointer */

	if (pSOM->u32PatchCount != 0)
		pSOM->pPatchBuffer = (ts_Patch *) ((u32) pSOM + pSOM->u32PatchOffset);
	else
		pSOM->pPatchBuffer = NULL;

	/* if SOM contains triangle strips, link them together (they are variable length) */

	if (pSOM->u32TriStripCount != 0)
	{
		ts_TriStripHeader *pTriStripHeader;

		pSOM->pTriStripBuffer = (ts_TriStripHeader *) ((u32) pSOM + pSOM->u32TriStripOffset);
		pTriStripHeader = pSOM->pTriStripBuffer;
		while (pTriStripHeader)
		{
			/* zero the pad, since DirectX uses it (hackily) to cache a "display list" number. */
			memset(pTriStripHeader->Pad2, 0, sizeof(pTriStripHeader->Pad2));
			if (pTriStripHeader->u32NextOffset != 0)
				pTriStripHeader->pNext = (ts_TriStripHeader *) ((char *) pTriStripHeader + pTriStripHeader->u32NextOffset);
			pTriStripHeader = pTriStripHeader->pNext;
		}
	}
	else
		pSOM->pTriStripBuffer = NULL;

	/* convert remaining offsets into pointers */

	pSOM->pVertexBuffer = (VCT_TYPE *) ((u32) pSOM + pSOM->u32VertexOffset);
	pSOM->pNormalBuffer = (VCT_TYPE *) ((u32) pSOM + pSOM->u32NormalOffset);
	pSOM->pControlVertexBuffer = (VCT_TYPE *) ((u32) pSOM + pSOM->u32ControlVertexOffset);
	pSOM->pUVBuffer = (Vector2 *) ((u32) pSOM + pSOM->u32UVOffset);

//	/* set texture set pointer-- it's possible that there is no texture set if no textures are defined (all flat shaded) */
//	pSOM->pTextureArray = TextureMgr::GetRecentTextureArray((u16)pSOM->u32TextureSubSetIndex);
//	pSOM->pTextureArray->AddTexture(TextureMgr::GetWhiteTexture());
	pSOM->pTextureArray = new TextureArray;
//	pSOM->pTextureArray->AddTexture( TextureMgr::GetWhiteTexture() );

	/* fix up texture contexts for patches and tri strips-- first patches */

	pPatch = pSOM->pPatchBuffer;
	for (i = 0; i < (signed) pSOM->u32PatchCount; i++, pPatch++)
	{
		if( pPatch->u32TextureContextIndex & 0xFF000000)
		{
			pPatch->pTextureContext = (ts_TextureContext *) GetRecordAddress(pWorldTextureContextList, pPatch->u32TextureContextIndex);
		}
		else
		{
			pPatch->pTextureContext = (ts_TextureContext *) GetRecordAddress(pTextureContextList, pPatch->u32TextureContextIndex);
		}
	}

//#define PRINT_VERTICES
#ifdef PRINT_VERTICES
	/* can be useful to look at vertices */

	bool bPrintVertices = (strstr(pSOM->Name, "locker") != NULL);
//	bool bPrintVertices = true;
	if (bPrintVertices)
		dbgPrint("Vertices for model %s:\n", pSOM->Name);
#endif //PRINT_VERTICES

	/* loop through tri strips for this model */

	pTriStripHeader = pSOM->pTriStripBuffer;
	for (i = 0; i < (signed) pSOM->u32TriStripCount; i++)
	{
		if( pTriStripHeader->u32TextureContextIndex & 0xFF000000)
		{
			pTriStripHeader->pTextureContext = (ts_TextureContext *) GetRecordAddress(pWorldTextureContextList, pTriStripHeader->u32TextureContextIndex);
		}
		else
		{
			pTriStripHeader->pTextureContext = (ts_TextureContext *) GetRecordAddress(pTextureContextList, pTriStripHeader->u32TextureContextIndex);
		}

		/* i need to know whether this model uses a mirror/aperture texture (a texture to render a separate view into */
		/* and then use as a texture), and if so, what its bind is */

		ts_TextureLayer *pTextureLayer = pTriStripHeader->pTextureContext->TextureLayer;

		for (int j = 0; j < pTriStripHeader->pTextureContext->TextureCount; j++, pTextureLayer++)
		{
			TextureDatabase* db = pTextureLayer->textureDatabase;
			ASSERT_PTR(db);
			Texture* tex = db->GetTexture(pTextureLayer->s16TextureIndex);
			ASSERT_PTR(tex);
			if(tex->GetFlags() & Texture::REFLECTS)
			{
				/* add this piece of data to som's miscellaneous data. since the data is just a bind value, */
				/* just pass it directly as the data pointer, instead of copying it off somewhere */
				DataMgr::AddOwner( pSOM, DATA_SOM )->AddData( tex, SOM_DATA_REFLECT_BIND, 0, 0 );
			}

			/* check for animated textures too */

			if(tex->GetFlags() & Texture::ANIMATED)
			{
				/* add this piece of data to som's miscellaneous data. since the data is just a bind value, */
				/* just pass it directly as the data pointer, instead of copying it off somewhere */
				DataMgr::AddOwner( pSOM, DATA_SOM )->AddData( tex, SOM_DATA_ANIMATED_TEXTURE, 0, 0 );
			}
		}

		/* skip over header to point to vertices */

		void *pData = TRI_STRIP_SKIP_HEADER(pTriStripHeader);

#ifdef PRINT_VERTICES
		/* can be useful to look at vertices */

		if (bPrintVertices)
		{
			dbgPrint("   Tri strip %d of %d:\n", i, pSOM->u32TriStripCount);
			Vector4 *pVertex = (Vector4 *) pData;
			for (int nVertexIndex = 0; nVertexIndex < pTriStripHeader->u16PointCount; nVertexIndex++, pVertex++)
				dbgPrint("      %.2f %.2f %.2f\n", pVertex->x(), pVertex->y(), pVertex->z());
		}
#endif //PRINT_VERTICES

		/* skip over vertices to point to next data-- normals or uv's */

		pData = TRI_STRIP_SKIP_VERTICES(pData, pTriStripHeader->u16SkipPointCount);

		/* for pc version sharing ps2 data, have to repack the normals (if any) because they are output as quad words for */
		/* the ps2 but the pc needs Vector3's */

#if defined(WIN32) || defined(GCN)
		if (pTriStripHeader->u8Flags & TRI_STRIP_HAS_NORMALS)
		{
			Vector4 *pSourceNormal = (Vector4 *) pData;
			Vector3 *pDestNormal = (Vector3 *) pData;
			for (int j = 0; j < pTriStripHeader->u16PointCount; j++, pSourceNormal++, pDestNormal++)
				memcpy(pDestNormal, pSourceNormal, sizeof(Vector3));
		}
#endif //WIN32

		pTriStripHeader = pTriStripHeader->pNext;	// to next triangle strip
	}
}
#endif //!DIRECTX


/* fix up a mesh som-- for dynamic mesh support */

void _FixMeshSOM(void *pSOM, ts_RecordList *pTextureContextList, ts_RecordList *pWorldTextureContextList)
{
	SMeshSOM *pMeshSOM = (SMeshSOM *) pSOM;

	/* we are only loading PatchSOM's at this point (that's a misnomer, typically they are tri-strip based) */

	ASSERT(pMeshSOM->dh.u8DataType == DATA_MULTI_SOM);

	/* fix up offsets into pointers */

	pMeshSOM->d_node = (SMeshSOM::SNodeSOM *) ((u32) pMeshSOM + pMeshSOM->d_nodeOffset);
	pMeshSOM->d_constraint = (SMeshSOM::SIndexPair *) ((u32) pMeshSOM + pMeshSOM->d_constraintOffset);
	pMeshSOM->d_collisionFace = (SMeshSOM::SIndexTrio *) ((u32) pMeshSOM + pMeshSOM->d_collisionFaceOffset);
	pMeshSOM->d_collisionEdge = (SMeshSOM::SIndexPair *) ((u32) pMeshSOM + pMeshSOM->d_collisionEdgeOffset);
	pMeshSOM->d_collisionPoint = (uintCount *) ((u32) pMeshSOM + pMeshSOM->d_collisionPointOffset);
	pMeshSOM->d_subMesh = (SMeshSOM::SSubMesh *) ((u32) pMeshSOM + pMeshSOM->d_subMeshOffset);
	pMeshSOM->d_neighbor = (uintCount *) ((u32) pMeshSOM + pMeshSOM->d_neighborOffset);

	/* and fix up sub meshes */

	for (int i = 0; i < pMeshSOM->d_numSubMesh; i++)
	{
		if( pMeshSOM->d_subMesh[i].d_textureContextIndex & 0xFF000000)
		{
			pMeshSOM->d_subMesh[i].d_textureContext = (ts_TextureContext *) GetRecordAddress(pWorldTextureContextList, pMeshSOM->d_subMesh[i].d_textureContextIndex);
		}
		else
		{
			pMeshSOM->d_subMesh[i].d_textureContext = (ts_TextureContext *) GetRecordAddress(pTextureContextList, pMeshSOM->d_subMesh[i].d_textureContextIndex);
		}
		pMeshSOM->d_subMesh[i].d_nodeIndex = (uintCount *) ((u32) pMeshSOM + pMeshSOM->d_subMesh[i].d_nodeIndexOffset);
		pMeshSOM->d_subMesh[i].d_textureCoord = (Vector2 *) ((u32) pMeshSOM + pMeshSOM->d_subMesh[i].d_textureCoordOffset);
		pMeshSOM->d_subMesh[i].d_triStripHead = (uintCount *) ((u32) pMeshSOM + pMeshSOM->d_subMesh[i].d_triStripHeadOffset);
		pMeshSOM->d_subMesh[i].d_triStrip = (uint16 *) ((u32) pMeshSOM + pMeshSOM->d_subMesh[i].d_triStripOffset);
	}

	// if we don't have a valid name from the tool chain, get it from the dbl.
	if ( pMeshSOM->d_name[ 0 ] == 0 )
	{
		char* dblName = dblGetCurrentFileName();
		ASSERT_PTR( dblName );
		char* p;
		for ( p = dblName + strlen( dblName ) - 1 ; p >= dblName && *p != '/' && *p != '\\' ; )
			p--;
		strncpy( pMeshSOM->d_name, p + 1, sizeof( pMeshSOM->d_name ) - 1 );
	}

	/* set texture set. subset index should be valid (filled in by filter chain) */

	// Ick!  this is a bad way to do this, but its compatible with the
	// old code, so it stays for now.  
//	pMeshSOM->d_textureArray = TextureMgr::GetRecentTextureArray((u16)pMeshSOM->d_textureSubSetIndex);
//	pMeshSOM->d_textureArray->AddTexture(TextureMgr::GetWhiteTexture());
	pMeshSOM->d_textureArray = new TextureArray;
//	pMeshSOM->d_textureArray->AddTexture( TextureMgr::GetWhiteTexture() );
}

#ifdef GCN
void _FixGCNSOM(ts_PatchSOM *pSOM, ts_RecordList *pTextureContextList, ts_RecordList *pWorldTextureContextList)
{
	
	int							i;

	ts_GCNTriStripHeader		*pGCNTriStripHeader;

#ifdef GCN
	ASSERTF( 0,( "Loaded DATA_GCN_SOM format SOM %s\n", pSOM->Name));
#endif
	
	pSOM->pPatchBuffer = NULL;
	pSOM->u32PatchCount = 0;

	/* set texture set pointer-- it's possible that there is no texture set if no textures are defined (all flat shaded) */
	// Ick!  this is a bad way to do this, but its compatible with the
	// old code, so it stays for now.  
//	pMeshSOM->d_textureArray = TextureMgr::GetRecentTextureArray(pMeshSOM->d_textureSubSetIndex);
//	pMeshSOM->d_textureArray->AddTexture(TextureMgr::GetWhiteTexture());
	pMeshSOM->d_textureArray = new TextureArray;
//	pMeshSOM->d_textureArray->AddTexture( TextureMgr::GetWhiteTexture() );

	// TJC - Im not sure what to do here...  but it will probably need to know about the
	// most recent texture database:
	TextureDatabase* tdb = TextureMgr::GetLastLoadedDatabase();
	// it will then probably use:
	//tdb->pTextureContextList
	// to find the contexts and:
	//tdb->GetTexture(index);
	// to get the textures from the database.
	LOCKUP();

	/* convert remaining offsets into pointers */
	pSOM->pVertexBuffer = (Vector4 *)( (u32)pSOM + pSOM->u32VertexOffset);
	pSOM->pNormalBuffer = (Vector4 *)( (u32) pSOM + pSOM->u32NormalOffset);
	pSOM->pUVBuffer = (Vector2 *)( (u32)pSOM + pSOM->u32UVOffset);
}

void _FixGCNSOM2(ts_PatchSOM *pSOM, ts_RecordList *pTextureContextList, ts_RecordList *pWorldTextureContextList)
{
	
	int							i;

	ts_GCNTriStripHeader		*pGCNTriStripHeader;

	/* we are only loading PatchSOM's at this point (that's a misnomer, typically they are tri-strip based) */
	ASSERT( pSOM->dh.u8DataType == DATA_GCN_SOM2);

	ASSERT( pSOM->u32VertexCount <= 65535);
	ASSERT( pSOM->u32UVCount <= 65535);
	ASSERT( pSOM->u32NormalCount <= 65535);

	pSOM->pPatchBuffer = NULL;
	pSOM->u32PatchCount = 0;

	/* set texture set pointer-- it's possible that there is no texture set if no textures are defined (all flat shaded) */
	// TJC - Im not sure what to do here...  but it will probably need to know about the
	// most recent texture database:
	TextureDatabase* tdb = TextureMgr::GetLastLoadedDatabase();
	// it will then probably use:
	//tdb->pTextureContextList
	// to find the contexts and:
	//tdb->GetTexture(index);
	// to get the textures from the database.
	LOCKUP();

	/* convert remaining offsets into pointers */
	pSOM->pVertexBuffer = (Vector4 *)( (u32)pSOM + pSOM->u32VertexOffset);
	pSOM->pNormalBuffer = (Vector4 *)( (u32) pSOM + pSOM->u32NormalOffset);
	pSOM->pUVBuffer = (Vector2 *)( (u32)pSOM + pSOM->u32UVOffset);
}
#endif

/* fix-up for ps2 dma soms-- have to patch gs register values since they aren't known until load time (e.g. vram addr of
texture). ned says-- this whole routine used to be #ifdef PS2, but i need to be able to do some processing of
ps2 dma soms on pc for MakeEnv mode (pc build is used to convert ps2 .oa to .env)-- specifically, i need to be able
to get to the bone data, and _GetSOMEnd relies on the fixup contained in this routine */

static int nAnimatedTextureBindInModel;
void _FixDMASOM( ts_PatchSOM *pSOM, ts_RecordList *pTextureContextList, ts_RecordList *pWorldTextureContextList)
{
#ifdef PS2
	// TJC - this really only has to be done once.. but... Im lazy today..
	PS2Renderer::SetTextureBindCallback(&PS2RendererTextureBindCallback);
	nAnimatedTextureBindInModel = NO_TEXTURE;	// this will be set if callback finds an animated texture in model
#endif // PS2

//	/* set texture set pointer-- it's possible that there is no texture set if no textures are defined (all flat shaded) */
//	pSOM->pTextureArray = TextureMgr::GetRecentTextureArray((u16)pSOM->u32TextureSubSetIndex);
//	pSOM->pTextureDatabase = TextureMgr::GetLastLoadedDatabase();
//	pSOM->pTextureArray->AddTexture(TextureMgr::GetWhiteTexture());
//	ASSERT_PTR(pSOM->pTextureArray);
	pSOM->pTextureDatabase = TextureMgr::GetLastLoadedDatabase();
	pSOM->pTextureArray = new TextureArray;
//	pSOM->pTextureArray->AddTexture( TextureMgr::GetWhiteTexture() );

	/* for dma som, offset to bone data is a bit more complicated. we have a patch som record, followed by a 4-byte */
	/* memblock size, followed by a 4-byte pad size, followed by the padding, followed by the memblock, followed */
	/* by the bone data. we do this for pc as well as for ps2 because pc needs to be able to parse ps2 data for */
	/* -MakeEnv option (pc pre-converts ps2 .oa files into .env files) */

	char *		pMemBlockBase = (char *) pSOM + sizeof(ts_PatchSOM);
	char *		pMemBlock = pMemBlockBase;

	u32 *			pDataBlockSize = (u32 *) pMemBlock;	pMemBlock += sizeof( u32 *);

	ASSERT( (*pDataBlockSize == 0xdeadbeef)  ||  (*pDataBlockSize == 0) );

	u32			u32MemBlocks = * (u32 *) pMemBlock;	pMemBlock += sizeof( u32);

	u32 *			pHdrArray = (u32 *) pMemBlock;		pMemBlock += u32MemBlocks * sizeof( u32);

	for( unsigned int i = 0; i < u32MemBlocks; i++)
	{
		u32 u32MemBlockSize = pHdrArray[i];
		ASSERT(u32MemBlockSize % BYTES_PER_QUAD_WORD == 0);

		u32 u32PadSize = * (u32 *) pMemBlock;			pMemBlock += sizeof( u32) + u32PadSize;
		
		ASSERT( IS_DMA_ALIGNED( pMemBlock));
		pHdrArray[i] = (u32)  pMemBlock;					pMemBlock += u32MemBlockSize;
	}

	/* this can only happen once (pHdrArray is converted from offsets to pointers), so support 0xdeadbeef ASSERT above */

	*pDataBlockSize = pMemBlock - pMemBlockBase;

#ifdef PS2
	PS2Renderer::fixupDDMAObjectTextures( pHdrArray, u32MemBlocks);

	/* if this model contains animated textures, mark it so instances can create a texture animator */

	/* CreateDMAObject does the register fix-up. models that have bones, or are used with multiple vertex color */
	/* sets, can't be fixed up until instantiation time */

	if ((PS2Renderer::getPatchSetCount( pHdrArray) > 1) || (pSOM->dh.u16Flags & SOMM_HAS_BONES))
	{
		/* can't fix up now, point off to data to use at fix-up time */

		pSOM->DisplayListRecord.pDisplayList = (char *) pHdrArray;
		pSOM->DisplayListRecord.u16QuadWordCount = u32MemBlocks;
	}
	else
	{
		/* fix up now */

		memSetUseBit(MEM_DMA_DATA);
		DDMAObject * dmaObject = PS2Renderer::createDDMAObject( pHdrArray, u32MemBlocks, 0);
		pSOM->DisplayListRecord.pDisplayList = (char*) dmaObject;
		pSOM->DisplayListRecord.u16QuadWordCount = 0; // to let us know it's been fixed up

		// jlb - patch for objects with no VertexColorCount
		pSOM->u32TriStripVertexColorCount = PS2Renderer::getDMAObjectVertices( dmaObject);
		memClearUseBit(MEM_DMA_DATA);
		
	}

	if (nAnimatedTextureBindInModel != NO_TEXTURE)
	{
		/* add this piece of data to som's miscellaneous data. since the data is just a bind value, */
		/* just pass it directly as the data pointer, instead of copying it off somewhere */
		DataMgr::AddOwner( pSOM, DATA_SOM )->AddData( (void*)nAnimatedTextureBindInModel, SOM_DATA_ANIMATED_TEXTURE, 0, 0 );
	}
#endif // PS2

}

#ifdef PS2
/* force a model to create a dma object for itself. typically, dma objects that have bones do NOT create a dma object
because there may be more than one actor that uses that model, and they need to animate independently. these actors
get the dma object attached to the instance instead of the model. (this also applies to static models that are used 
with multiple vertex color sets). in special circumstances it may be desirable to put the dma object at the model level
instead of the instance level. the case that has led to the creation of this routine is that we want to create the
dma object for tak up front, one time only. then every time tak's instance gets created (every level), he gets
the model's dma object instead of having to create his own */

bool ForceDMAObject(ts_PatchSOM *pSOM)
{
	/* calculate offset to bone data. this is a copy of the code in _FixDMASOM, without the ASSERT */

	char *pMemBlockBase = (char *) pSOM + sizeof(ts_PatchSOM);
	char *pMemBlock = pMemBlockBase;
	pMemBlock += sizeof(u32 *);	// skip pDataBlockSize
	u32 u32MemBlocks = *(u32 *) pMemBlock;	pMemBlock += sizeof( u32);
	u32 *pHdrArray = (u32 *) pMemBlock;

	/* the model should not have been fixed up yet. it should have been ineligible for fixup in _FixDMASOM */

	ASSERT(pSOM->DisplayListRecord.u16QuadWordCount != 0);

	/* fix up now */

	memSetUseBit(MEM_DMA_DATA);
	DDMAObject * dmaObject = PS2Renderer::createDDMAObject( pHdrArray, u32MemBlocks, 0);
	pSOM->DisplayListRecord.pDisplayList = (char*) dmaObject;
	pSOM->DisplayListRecord.u16QuadWordCount = 0; // to let us know it's been fixed up

	// jlb - patch for objects with no VertexColorCount
	pSOM->u32TriStripVertexColorCount = PS2Renderer::getDMAObjectVertices( dmaObject);
	memClearUseBit(MEM_DMA_DATA);
	return(true);
}

/* the dma data contains texture context index and layer, and a link to the next one to fix-up. these need to be fixed up 
into gs register values by following the links and fixing each one */

u_int PS2RendererTextureBindCallback(u_int textureID, u_int layerIndex)
{
	u32 u32Bind = 0;

	if( textureID == -1 )
		return 0xFFFFFFFF;
	else if(layerIndex == 0xFFFFFFFF) // we are passed a bind...
	{
		ASSERT_PTR(textureID);
		u32Bind = textureID;
	}
	else
	{
		/* point to proper texture context layer */

		ts_TextureLayer *pTextureLayer = TextureMgr::GetTextureContextLayer(textureID, layerIndex);
		if(pTextureLayer->textureDatabase == NULL)
		{
			if(pTextureLayer->u8Flags & TEXTURE_USE_WHITE_TEXTURE)
				return 0xFFFFFFFF;
			else
				ASSERT(false);
		}
		else
		{
			return (u_int)pTextureLayer;
		}
	}

	return u32Bind;
}
#endif // PS2

/* fix up offsets/pointers for a collision data SOM-- this is not renderable, it is used by terraintoola and
obbworld to generate collision data */

#ifdef WIN32	// only intended for use by terraintoola and obbworld
void _FixCollisionSOM(ts_CollisionSOM *pCollisionSOM)
{
	/* fix-up pointers */

	pCollisionSOM->pVertexBuffer = (Vector3 *) ((u32) pCollisionSOM + pCollisionSOM->u32VertexOffset);
	pCollisionSOM->pNormalBuffer = (Vector3 *) ((u32) pCollisionSOM + pCollisionSOM->u32NormalOffset);
	pCollisionSOM->pPolyBuffer = (ts_CollisionPoly *) ((u32) pCollisionSOM + pCollisionSOM->u32PolyOffset);
}
#endif //WIN32

/* calculate where end of SOM is. uv buffer is last buffer output in SOM export */

void *_GetSOMEnd(ts_PatchSOM *pSOM)
{
	uint SOMEnd;

#ifdef DIRECTX
	SOMEnd = (unsigned int ) pSOM + pSOM->u32UVOffset;
#else
	SOMEnd = ((uint) pSOM->pUVBuffer + (pSOM->u32UVCount * sizeof(Vector2)));

#ifdef GCN
	if(	pSOM->dh.u8DataType == DATA_GCN_SOM ||
			pSOM->dh.u8DataType == DATA_GCN_SOM2)
	{
		// bones are 32-byte aligned on DATA_GCN_SOM
		SOMEnd = (SOMEnd + 0x1f) & ~0x1f;
	}
	else
	{
		// bones are 16-byte aligned otherwise
		SOMEnd = (SOMEnd + 0x0f) & ~0x0f;
	}
#else
	/* this next section used to be #ifdef PS2, but in MakeEnv mode (pc build does .oa to .env conversion for ps2 data) */
	/* i need to be able to process ps2 data */

	if (pSOM->dh.u8DataType == DATA_PS2_DMA_SOM)
	{
		/* for dma som, offset to bone data follows the patch som record */

		char * pBoneData = (char *) pSOM;
		pBoneData += sizeof(ts_PatchSOM);
		pBoneData += * ((u32 *) pBoneData);

		ASSERT(IS_DMA_ALIGNED(pBoneData));
		SOMEnd = (uint) pBoneData;
	}
#endif //GCN
#endif //DIRECTX

	return( (void *) SOMEnd);
}

void _FixBones( ts_ModelBone *pRoot, ts_ModelBone *pBone, ts_ModelBone *pParent, int iBoneCount, BoneGroupBitMask *boneGroupBits)
{
	if( pBone->boneGroupID <= MAX_BONE_GROUP_ID )//if this bone is a member of a valid group
		*boneGroupBits |= 1<<(pBone->boneGroupID);//then add it's bone group to the mask of valid groups it contains
	
	pBone->m_parent = pParent;//record this bone's parent
	pBone->pNextBoneInGroup = NULL;//ensure bone group lsit ptr is NULL (for fixup in _FixBoneGroupLists)

#ifdef GCN
	//hack-- some bone boxes are not endian swapped (bug in filter chain)

	bool bSwap = false;
	if ((Math::Abs(pBone->mBoneBox.Max.x( )) > 10000.0f) ||
		 (Math::Abs(pBone->mBoneBox.Max.y( )) > 10000.0f) ||
		 (Math::Abs(pBone->mBoneBox.Max.z( )) > 10000.0f) ||
		 (Math::Abs(pBone->mBoneBox.Min.x( )) > 10000.0f) ||
		 (Math::Abs(pBone->mBoneBox.Min.y( )) > 10000.0f) ||
		 (Math::Abs(pBone->mBoneBox.Min.z( )) > 10000.0f))
	{
		bSwap = true;	// at least one huge value
	}
	if ((Math::Abs(pBone->mBoneBox.Max.x( )) < 0.01f) &&
		 (Math::Abs(pBone->mBoneBox.Max.y( )) < 0.01f) &&
		 (Math::Abs(pBone->mBoneBox.Max.z( )) < 0.01f) &&
		 (Math::Abs(pBone->mBoneBox.Min.x( )) < 0.01f) &&
		 (Math::Abs(pBone->mBoneBox.Min.y( )) < 0.01f) &&
		 (Math::Abs(pBone->mBoneBox.Min.z( )) < 0.01f))
	{
		bSwap = true;	// all values are very small
	}
	if ((pBone->mBoneBox.Max.x( ) == -MAXFLOAT) &&
		 (pBone->mBoneBox.Max.y( ) == -MAXFLOAT) &&
		 (pBone->mBoneBox.Max.z( ) == -MAXFLOAT) &&
		 (pBone->mBoneBox.Min.x( ) == MAXFLOAT) &&
		 (pBone->mBoneBox.Min.y( ) == MAXFLOAT) &&
		 (pBone->mBoneBox.Min.z( ) == MAXFLOAT))
	{
		bSwap = false;	// empty box, not swap needed
	}
	if (bSwap)
	{
		AABB BoneBox = *(AABB *) &pBone->mBoneBox;
		BoneBox.EndianSwap();
		if ((Math::Abs(BoneBox.maxV.x( )) < 10000.0f) &&
			 (Math::Abs(BoneBox.maxV.y( )) < 10000.0f) &&
			 (Math::Abs(BoneBox.maxV.z( )) < 10000.0f) &&
			 (Math::Abs(BoneBox.minV.x( )) < 10000.0f) &&
			 (Math::Abs(BoneBox.minV.y( )) < 10000.0f) &&
			 (Math::Abs(BoneBox.minV.z( )) < 10000.0f))
		{
			pBone->mBoneBox = *(ts_BoundingBox *) &BoneBox;	// no sign of bad swap, so it must be ok
		}
		if ((BoneBox.maxV.x( ) == -MAXFLOAT) &&
			 (BoneBox.maxV.y( ) == -MAXFLOAT) &&
			 (BoneBox.maxV.z( ) == -MAXFLOAT) &&
			 (BoneBox.minV.x( ) == MAXFLOAT) &&
			 (BoneBox.minV.y( ) == MAXFLOAT) &&
			 (BoneBox.minV.z( ) == MAXFLOAT))
		{
			pBone->mBoneBox = *(ts_BoundingBox *) &BoneBox;	// swapped box is empty, this is ok
		}
	}
#endif //GCN

	if( pBone->pChildren)
	{
		pBone->pChildren = pRoot + ((u32) pBone->pChildren);
		_FixBones( pRoot, pBone->pChildren, pBone, iBoneCount, boneGroupBits);
	}

	if( pBone->pSiblings)
	{
		pBone->pSiblings = pRoot + ((u32) pBone->pSiblings);
		_FixBones( pRoot, pBone->pSiblings, pParent, iBoneCount, boneGroupBits);
	}
}

unsigned short _CountGroupBits( BoneGroupBitMask boneGroupBits )
{
	//boneGroupBits is a bit array, bit0==1 -> group0 has data, and so on.
	//compute the number of groups (one bits)

	u32 u32NumBits = sizeof(BoneGroupBitMask)*8;
	bool bEncounteredZeroBit = FALSE;
	unsigned short usOneBits = 0, usNumGroups = 0;
	unsigned short usIndex;

	for( usIndex = 0; usIndex < u32NumBits; usIndex++)
	{
		if( (boneGroupBits & 1) == 0)
		{
			if( !bEncounteredZeroBit )
			{
				usNumGroups = usOneBits;
				bEncounteredZeroBit = TRUE;
			}
		}
		else
		{
			usOneBits++;
			if( bEncounteredZeroBit )
			{
				usNumGroups = usIndex + 1;
			}
		}

		boneGroupBits >>= 1;
	}
	return usNumGroups;
}

