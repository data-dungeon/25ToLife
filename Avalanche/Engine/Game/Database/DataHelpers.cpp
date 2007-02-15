/*************************************/
/*                                   */
/*   DataHelpers.c                   */
/*   big juju prototype  07/14/00    */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   data structure helper functions */
/*                                   */
/*************************************/

/* provide helpful functions for traversing game data structures */

/******************* includes ****************************/

#include "Game/GamePCH.h"





/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/******************* externs *****************************/

/******************* global variables ********************/

/******************* local variables *********************/

/******************* local prototypes ********************/

/******************* functions ***************************/

/* return model for geometry (highest LOD if it's an LOD record) */

bool GetBoneBox(ts_PatchSOM *pPatchSOM, BoneID boneID, ts_BoundingBox *pBox)
{
	int i;
	bool bFound = false;
	BoneID boneID2 = INVALID_BONEID;

	BoundingBox__Clear(pBox);

	//there may be no verticies attached to this bone, but may be attached to orient-like bones - check these too
	ts_ModelBone *pMBone = pPatchSOM->pBody->pMBones;
	u16 u16Bones = pPatchSOM->pBody->usBones;
	for(i=0; i<u16Bones; i++, pMBone++)
	{
		if(pMBone->boneID == boneID || pMBone->OrientBone == boneID)
		{
			bFound = true;
			BoundingBox__Union(pBox, &pMBone->mBoneBox, pBox);
		}
	}
	return(bFound);
}


bool RemoveModelBoneData(ts_PatchSOM *pSOM)
{
	/* if this SOM has bones and is NOT an LOD, then free it's bones.  (LOD's use their master's bones) */

	if (pSOM && (pSOM->dh.u8DataType != DATA_MULTI_SOM) && pSOM->pBody && !(pSOM->dh.u16Flags & SOMM_IS_LOD))
	{
		ts_Body *pBody = pSOM->pBody;
		bool success = memFree(pBody);
		pSOM->pBody = NULL;
		return success;
	}
	else
		return true;
}


/* generic name list access routines */

int GetNameCount(t_Handle Handle, int nChunkType, char *pChunkName /*= NULL*/)
{
	ts_RecordList *pRecordList;

	/* look for name list chunk */

	pRecordList = (ts_RecordList *) dblGetChunkData((u16) nChunkType, pChunkName, Handle);
	if (pRecordList)
		return(pRecordList->usRecordCount);
	else
		return(0);
}
char *GetName(t_Handle Handle, int nIndex, int nChunkType, char *pChunkName /*= NULL*/)
{
	ts_RecordList *pRecordList;

	/* look for name list chunk */

	pRecordList = (ts_RecordList *) dblGetChunkData((u16) nChunkType, pChunkName, Handle);
	if (pRecordList)
	{
		ASSERT(nIndex < pRecordList->usRecordCount);
		return((char *) GetRecordAddress(pRecordList, nIndex));
	}
	else
		return(NULL);
}

/* let user query terrain material list */

int GetTerrainCount(t_Handle Handle)
{
	return(GetNameCount(Handle, DB_TERRAIN_LIST));
}
char *GetTerrainName(t_Handle Handle, int nIndex)
{
	return(GetName(Handle, nIndex, DB_TERRAIN_LIST));
}

