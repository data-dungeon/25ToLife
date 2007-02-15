/*************************************/
/*                                   */
/*   world_load.c                    */
/*   big juju prototype  02/24/00    */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   .DBL chunk loading routines     */
/*                                   */
/*************************************/

/* loading routines for world chunk types. a .DBL file is composed of data chunks, each chunk has a different
loading routine */

/******************* includes ****************************/

#include "Game/GamePCH.h"





/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/******************* externs *****************************/

/******************* global variables ********************/

/******************* local variables *********************/

/******************* local prototypes ********************/

static void _FixLOD( ts_LOD *pLOD, ts_RecordList *pSOMList);
static void _FixGroup( ts_Group *pGroup, ts_RecordList *pInstList, CInstance ***pppGroupInst);

/******************* functions ***************************/

/* load the DB_LOD chunk-- an array of LOD records */

bool LoadLODs( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize)
{
	unsigned short usIndex;
	ts_RecordList *pLODList;
	ts_RecordList *pSOMList;
	ts_LOD *pLOD;

	/* point to data */

	pLODList = (ts_RecordList *) pData;
	ResolveRecordList(pLODList);

	/* get SOM data (previously loaded chunk in this database) for patching LOD SOM references */

   pSOMList = (ts_RecordList *) dblGetChunkDataCurrentDBL(DB_SOMARRAY, dblGetChunkName(pChunkHdr));
	if (pSOMList == NULL)
	   pSOMList = (ts_RecordList *) dblGetChunkDataCurrentDBL(DB_SOMARRAYBONE, dblGetChunkName(pChunkHdr));
#ifdef WIN32	// for terraintoola and obbworld
	if (pSOMList == NULL)
	   pSOMList = (ts_RecordList *) dblGetChunkDataCurrentDBL(DB_COLLISIONSOMARRAY, dblGetChunkName(pChunkHdr));
#endif //WIN32
	ASSERT(pSOMList);

	/* fix up each LOD */

	for (usIndex = 0; usIndex < pLODList->usRecordCount; usIndex++)
	{
		pLOD = (ts_LOD *) GetRecordAddress(pLODList, usIndex);
		_FixLOD(pLOD, pSOMList);
	}

	/* this is permanent data */

	pChunkHdr->u16Flags &= ~(DBL_FLUSHABLE);
	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	return TRUE;
}

/* fix up a single LOD record */

static void _FixLOD( ts_LOD *pLOD, ts_RecordList *pSOMList)
{
	int i;
	bool bLastLODFound = FALSE;

	for (i = 0; i < MAX_LODS; i++)
	{
		if (bLastLODFound)
			pLOD->pSOM[i] = NULL;
		else
			pLOD->pSOM[i] = (ts_PatchSOM *) GetRecordAddress(pSOMList, pLOD->u32SOMIndex[i]);
		if (pLOD->fSwitchRange[i] == LOD_INFINITY)
			bLastLODFound = TRUE;
	}
}	

/* load the DB_GROUP chunk-- an array of group records */

bool LoadGroups( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize)
{
	u32				u32Index;
	ts_RecordList	*pGroupList;
	ts_Group			*pGroup;
	CInstance		**ppGroupInst;
	ts_RecordList	*pInstList;

	/* ignore group chunk for .dbl files-- it will be obsolete soon. but i can override this if i want to keep the data */
	/* for scene manager comparison during transition to new manager */

	if (pChunkHdr->u16Flags & DBL_FLUSHABLE)
	{
		*ppRetData = NULL;
		*pu32DataSize = 0;
		return(TRUE);
	}

	/* point to data */

	pGroupList = (ts_RecordList *) pData;

	/* get instance data (previously loaded chunk in this database) for patching group references */

   pInstList = (ts_RecordList *) dblGetChunkDataCurrentDBL(DB_INSTANCE, dblGetChunkName(pChunkHdr));

	/* process groups. group chunk has some variable-size data following the group record list that is the instance lists */
	/* for each group. FixGroup updates ppGroupInst to keep moving through the instance lists */

	ppGroupInst = (CInstance **) GetRecordAddress(pGroupList, pGroupList->usRecordCount);	// instance lists follow group list
	for (u32Index = 0; u32Index < pGroupList->usRecordCount; u32Index++)
	{
		pGroup = (ts_Group *) GetRecordAddress(pGroupList, u32Index);
		_FixGroup(pGroup, pInstList, &ppGroupInst);
	}

	/* this is permanent data */
	
	pChunkHdr->u16Flags &= ~(DBL_FLUSHABLE);
	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	return TRUE;
}

/* fix up a single group record */

static void _FixGroup( ts_Group *pGroup, ts_RecordList *pInstList, CInstance ***pppGroupInst)
{
	int	nInst;

	pGroup->u8Flags = 0;
	if( pGroup->usDispStatInst != 0)
	{
		pGroup->ppDispStatArray = *pppGroupInst;
		*pppGroupInst += pGroup->usDispStatInst;
		for( nInst = 0; nInst < pGroup->usDispStatInst; nInst++)
		{
			pGroup->ppDispStatArray[nInst] = (CInstance *) GetRecordAddress(pInstList, (u32) (pGroup->ppDispStatArray[nInst]));
		}
	}
	else
	{
		pGroup->ppDispStatArray = NULL;
	}
	pGroup->pDispDynList = NULL;

	if( pGroup->usCollStatInst != 0)
	{
		pGroup->ppCollStatArray = *pppGroupInst;
		*pppGroupInst += pGroup->usCollStatInst;
		for( nInst = 0; nInst < pGroup->usCollStatInst; nInst++)
		{
			pGroup->ppCollStatArray[nInst] = (CInstance *) GetRecordAddress(pInstList, (u32) (pGroup->ppCollStatArray[nInst]));
		}
	}
	else
		pGroup->ppCollStatArray = NULL;
	pGroup->pCollDynList = NULL;
}	

/* load the DB_PATHS chunk */

//#define SEE_PATH_NAMES
#ifdef SEE_PATH_NAMES
#include "path/Path.h"	// for ts_SplinePath
#endif //SEE_PATH_NAMES
bool LoadPaths( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize)
{
	ResolveRecordList((ts_RecordList *) pData);

	/* this is permanent data */
	
	pChunkHdr->u16Flags &= ~(DBL_FLUSHABLE);
	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

#ifdef SEE_PATH_NAMES
	/* report path names */

	dbgPrint("Paths in path chunk:\n");
	ts_RecordList *pPathList = (ts_RecordList *) pData;
	for (int i = 0; i < pPathList->usRecordCount; i++)
	{
		ts_SplinePath *pPath = (ts_SplinePath *) GetRecordAddress(pPathList, i);
		dbgPrint("   %02d: %s\n", i, pPath->name);
	}
#endif //SEE_PATH_NAMES

	return TRUE;
}

/* load the DB_NAMED_POINTS chunk */

bool LoadNamedPoints(void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize)
{
	ResolveRecordList((ts_RecordList *) pData);

	/* this is permanent data */
	
	pChunkHdr->u16Flags &= ~(DBL_FLUSHABLE);
	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

//#define SEE_POINT_NAMES
#ifdef SEE_POINT_NAMES
	/* report path names */

	dbgPrint("Points in named points chunk:\n");
	ts_RecordList *pPointList = (ts_RecordList *) pData;
	for (int i = 0; i < pPointList->usRecordCount; i++)
	{
		ts_NamedPosition *pNamedPoint = (ts_NamedPosition *) GetRecordAddress(pPointList, i);
		dbgPrint("   %02d: %s\n", i, pNamedPoint->Name);
	}
#endif //SEE_POINT_NAMES

	return TRUE;
}

bool LoadSoundSources(void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize)
{
	ResolveRecordList((ts_RecordList *)pData);

	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	return TRUE;
}

bool LoadSoundFields(void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize)
{
	ResolveRecordList((ts_RecordList *)pData);

	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	return TRUE;
}

bool LoadHalo(void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize)
{
	ResolveRecordList((ts_RecordList *)pData);

	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	return TRUE;
}

bool LoadBloom(void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize)
{
	ResolveRecordList((ts_RecordList *)pData);

	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	return TRUE;
}

bool LoadStationaryCam(void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize)
{
	ResolveRecordList((ts_RecordList *)pData);

	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	return TRUE;
}

bool LoadWater(void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize)
{
	ResolveRecordList((ts_RecordList *)pData);

	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	return TRUE;
}

/* load the patch bounds expansion tables. these are 3 tables, one each for x, y, and z. they are tables of floats, 256
entries max. they handle the collision issue that a collision-tessellated patch may have a larger bounding box than the
non-tessellated patch. the non-tessellated patch bounding box is the box that includes the patch's vertices. the expansion
tables can expand that bounding box in x, y, and z to include all the tessellated patch's triangles */

bool LoadExpansionTables( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize)
{
	/* this is permanent data */

	pChunkHdr->u16Flags &= ~(DBL_FLUSHABLE);
	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	return TRUE;
}

/* load the particle effects chunk. particle effects are composed of one or more channels, each channel can
be a particle, or a sound, or an event */

bool LoadParticleEffects( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize)
{
	unsigned short	usIndex;
	ts_RecordList	*pEffectList;
	ts_RecordList	*pParticleSourceList;
	ts_Effect	*pEffect;

	/* point to data */

	pEffectList = (ts_RecordList *) pData;
	ResolveRecordList(pEffectList);

	/* get particle data (previously loaded chunk in this database) for patching effect references */

   pParticleSourceList = (ts_RecordList *) dblGetChunkDataCurrentDBL(DB_PARTICLE_SOURCE, dblGetChunkName(pChunkHdr));
	ASSERT(pParticleSourceList);

	/* fix up each effect */

	for (usIndex = 0; usIndex < pEffectList->usRecordCount; usIndex++)
	{
		pEffect = (ts_Effect *) GetRecordAddress(pEffectList, usIndex);
		FixParticleEffect(pEffect, pParticleSourceList);
	}

	/* this is permanent data */

	pChunkHdr->u16Flags &= ~(DBL_FLUSHABLE);
	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	/* success */

	return(TRUE);
}

/* fix-up for particle effects. go through all channels and replace particle indices with pointers */

void FixParticleEffect(ts_Effect *pEffect, ts_RecordList *pParticleSourceList)
{
	int i;

	ts_Channel *pChannel = &pEffect->ChannelData[0];
	for (i = 0; i < (signed) pEffect->u32ChannelCount; i++, pChannel++)
	{
		if ((pChannel->u8Flags & CHANNEL_IS_SOUND) == 0)	// leave sound indicies alone
			pChannel->pParticleSource = (ts_ParticleSource *) GetRecordAddress(pParticleSourceList, pChannel->u32ParticleIndex);
	}
}	

/* load the particle data chunk. this data describes the behavior of single particles */

bool LoadParticleSource( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize)
{
	unsigned short		usIndex;
	ts_RecordList		*pParticleSourceList;
	ts_ParticleSource	*pParticleSource;

	/* point to data */

	pParticleSourceList = (ts_RecordList *) pData;
	ResolveRecordList(pParticleSourceList);

	/* fix up each particle data record */

	for (usIndex = 0; usIndex < pParticleSourceList->usRecordCount; usIndex++)
	{
		pParticleSource = (ts_ParticleSource *) GetRecordAddress(pParticleSourceList, usIndex);
		FixParticleSource(pParticleSource);
	}

	/* this is permanent data */

	pChunkHdr->u16Flags &= ~(DBL_FLUSHABLE);
	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	/* success */

	return(TRUE);
}

/* load the swoosh data chunk. this data describes the behavior of single swooshes */

bool LoadSwooshSource( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize)
{
	unsigned short		usIndex;
	ts_RecordList		*pSwooshSourceList;
	ts_SwooshSource	*pSwooshSource;

	/* point to data */

	pSwooshSourceList = (ts_RecordList *) pData;
	ResolveRecordList(pSwooshSourceList);

	/* fix up each swoosh data record */

	for (usIndex = 0; usIndex < pSwooshSourceList->usRecordCount; usIndex++)
	{
		pSwooshSource = (ts_SwooshSource *) GetRecordAddress(pSwooshSourceList, usIndex);
		FixSwooshSource(pSwooshSource);
	}

	/* this is permanent data */

	pChunkHdr->u16Flags &= ~(DBL_FLUSHABLE);
	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	/* success */

	return(TRUE);
}

/* load the swoosh emitter data chunk. this data describes the behavior of swoosh emitters */

bool LoadSwooshEmitters( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize)
{
	/* point to data */

	ts_RecordList *pSwooshEmitterList = (ts_RecordList *) pData;
	ResolveRecordList(pSwooshEmitterList);

	/* this is permanent data */

	pChunkHdr->u16Flags &= ~(DBL_FLUSHABLE);
	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	/* success */

	return(TRUE);
}

/* fix-up for particle data. replace texture index with OpenGL bind value */

void FixParticleSource(ts_ParticleSource *pParticleSource)
{
	pParticleSource->pTextureDatabase = TextureMgr::GetLastLoadedDatabase();

	/* standard gravity */

	if (pParticleSource->u32Flags & PARTICLE_STANDARD_GRAVITY)
		pParticleSource->fGravity = -9.8f;

	/* pre-multiply spline envelope middle terms by 3.0f for very narginal optimization-- but Particle__EvaluateEnvelope */
	/* is called a lot */

	for (int i = 0; i < MAX_ENVELOPES; i++)
	{
		ts_ParticleEnvelope *pEnvelope = &pParticleSource->Envelope[i];
		if (pEnvelope->u8Type == SPLINE_ENVELOPE)
		{
			pEnvelope->SplinePoint1.y(pEnvelope->SplinePoint1.y() * 3.0f);
			pEnvelope->SplinePoint2.y(pEnvelope->SplinePoint2.y() * 3.0f);
		}
	}
}

/* fix-up for swoosh data. replace texture index with OpenGL bind value */

void FixSwooshSource(ts_SwooshSource *pSwooshSource)
{
	TextureDatabase* db = TextureMgr::GetLastLoadedDatabase();
	int index = pSwooshSource->u16FirstIndex;

	if ( index >= 0 && index < db->TextureCount() )
		pSwooshSource->pTexture = db->GetTexture( index );
	else
		pSwooshSource->pTexture = TextureMgr::GetWhiteTexture();
}

/* load the particle emitter data chunk. this data describes the behavior of pre-defined particle emitters */

bool LoadParticleEmitters( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize)
{
	ts_RecordList *pParticleEmitterList;

	/* point to data */

	pParticleEmitterList = (ts_RecordList *) pData;
	ResolveRecordList(pParticleEmitterList);

	/* this is permanent data */

	pChunkHdr->u16Flags &= ~(DBL_FLUSHABLE);
	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	/* success */

	return(TRUE);
}

/* load the particle emitters placed in the world by the artists */

bool LoadWorldEmitters(void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize)
{
	/* resolve the record list */

	ts_RecordList *pEmitters = (ts_RecordList *) pData;
	ResolveRecordList(pEmitters);

	/* this is permanent data-- it gets processed later by SimLoop::SetupWorldEmitters */
	
	pChunkHdr->u16Flags &= ~( DBL_FLUSHABLE );
	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	/* success */

	return(TRUE);
}

/* load the distortions placed in the world by the artists */

bool LoadWorldDistortions(void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize)
{
	/* this is permanent data-- it gets processed later by SimLoop::SetupWorldDistortions */
	
	pChunkHdr->u16Flags &= ~( DBL_FLUSHABLE );
	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	/* success */

	return(TRUE);
}

//fix up instance bone array 

void FixInstanceBones( ts_Bone *pRoot, ts_Bone *pBone, ts_Bone *pParent, ts_Body *pBody, ts_ModelBone *pMRoot, ts_ModelBone *pMBone, ts_ModelBone *pMParent)
{
	pBone->Matrix.Identity();
	pBone->LocalBoneMatrix.Identity();
	pBone->bIdentity = true;

	pBone->scale = SCALE_ONE;	//initially all bones are set to unit scale factor
	pBone->bLocalMatrixChanged = FALSE;
	pBone->lock = 0;
	pBone->boneID = pMBone->boneID;

	pBone->pModelBone = pMBone;	//ptr to assocaited model bone
	pBone->pBody = pBody;		//ptr back to body

	pBone->m_parent = pParent;	//record this bone's parent
	pBone->pNextBoneInGroup = NULL;//ensure bone group lsit ptr is NULL (for fixup in _FixBoneGroupLists)

	pBone->pChildren = NULL;	//in case this is terminal node of tree
	pBone->pSiblings = NULL;

	pBone->pMChannel = NULL;

	//fix up the full tree
	if( pMBone->pChildren)
	{
		pBone->pChildren = pRoot + (pMBone->pChildren - pMRoot);
		FixInstanceBones( pRoot, pBone->pChildren, pBone, pBody, pMRoot, pMBone->pChildren, pMBone);
	}

	if( pMBone->pSiblings)
	{
		pBone->pSiblings = pRoot + (pMBone->pSiblings - pMRoot);
		FixInstanceBones( pRoot, pBone->pSiblings, pParent, pBody, pMRoot, pMBone->pSiblings, pMParent);
	}
}

void FixBoneGroupLists( ts_Bone *pBone, ts_BoneGroupList *pBoneGroupList)
{
	ts_Bone *pNextBoneList;

	//if this bone has no group ID, don't add it to any bone group list
	if( pBone->pModelBone->boneGroupID <= MAX_BONE_GROUP_ID )
	{
		//insert this bone at the end of the bonegorup list
		if( (pNextBoneList = pBoneGroupList[pBone->pModelBone->boneGroupID].pNextBoneInGroup) == NULL)
			pBoneGroupList[pBone->pModelBone->boneGroupID].pNextBoneInGroup = pBone;
		else
			while ( pNextBoneList )
			{
				if( pNextBoneList->pNextBoneInGroup == NULL )
				{
					pNextBoneList->pNextBoneInGroup = pBone;
					break;
				}
				pNextBoneList = pNextBoneList->pNextBoneInGroup;
			}
	}

	if( pBone->pChildren)
	{
		FixBoneGroupLists( pBone->pChildren, pBoneGroupList);
	}

	if( pBone->pSiblings)
	{
		FixBoneGroupLists( pBone->pSiblings, pBoneGroupList);
	}

}

/* fix up the scene tree chunk. the scene tree is a bunch of linked CSceneNode records */

bool LoadSceneTree( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize)
{
	/* we must have an instance chunk to reference into */

   ts_RecordList *pInstList = (ts_RecordList *) dblGetChunkDataCurrentDBL(DB_INSTANCE, dblGetChunkName(pChunkHdr));
// ATB removed ASSERT because some partitions may be NULL
//	ASSERT(pInstList);	// CSceneNode::FixDiskLoad is ok if it's null, but tree will be worthless (no instances)

	/* let the scene node's fix themselves up recursively. the first thing in the data is a scene node */

	CSceneNode *pTreeRoot = (CSceneNode *) pData;
	pTreeRoot->SetDiskLoadData(pData, pInstList);
	pTreeRoot->WalkToFixDiskLoad();

	/* this is permanent data */
	
	pChunkHdr->u16Flags &= ~( DBL_FLUSHABLE );
	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	/* success */

	return(TRUE);
}

/* fix up the vis regions chunk. this chunk is a list of visibility regions. each visibility region controls a branch of the
scene tree, and has a bounding box and a list of other visibility regions visible from it */

bool LoadVisRegions( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize)
{
	/* we must have a scene tree chunk to reference into */

	CSceneNode *pTreeRoot = (CSceneNode *) dblGetChunkDataCurrentDBL(DB_SCENE_TREE, dblGetChunkName(pChunkHdr));
	ASSERT(pTreeRoot);	// CSceneNode::FixDiskLoad is ok if it's null, but tree will be worthless (no instances)
	if (pTreeRoot == NULL)
		return(false);

	/* point to the data */

	ts_RecordList *pVisRegionList = (ts_RecordList *) pData;
	ResolveRecordList(pVisRegionList);

	/* fix up the vis regions */

	for (int i = 0; i < pVisRegionList->usRecordCount; i++)
	{
		CVisRegion *pRegion = (CVisRegion *) GetRecordAddress(pVisRegionList, i);
		pRegion->FixDiskLoad(pTreeRoot, pVisRegionList);
	}

	/* this is permanent data */
	
	pChunkHdr->u16Flags &= ~( DBL_FLUSHABLE );
	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	/* success */

	return(TRUE);
}

bool loadLightSet( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize )
{
	ts_RecordList *pLightSet = ( ts_RecordList * )pData;
	ResolveRecordList( pLightSet );

	pChunkHdr->u16Flags &= ~( DBL_FLUSHABLE );
	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	/* success */

	return(TRUE);
}








