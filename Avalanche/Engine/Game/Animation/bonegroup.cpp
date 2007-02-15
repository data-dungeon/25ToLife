/************************************/
/*                                  */
/*   bonegroup.c                    */
/*												*/
/*												*/
/*												*/
/*												*/
/*                                  */
/************************************/

/******************* includes ****************************/

#include "Game/GamePCH.h"



/******************* defines *****************************/
#define ANIMATION_TO_NEW_INTERP_FRAMES 5
#define BONEGROUP_MINIMUM_PRIORITY 0xffffffff
/******************* macros ******************************/

/******************* externs *****************************/

/******************* global variables ********************/

/******************* local variables *********************/

/******************* local prototypes ********************/
ts_BoneGroup *CBoneGroupTable__FindNextPriorityBoneGroup(ts_BoneGroupTable *pBoneGroupTable, u32 u32ProcessMarker);

/******************* functions ***************************/
ts_BoneGroupTable *CBoneGroupTable__Create(ts_Body *pBody)
{
	ts_BoneGroupTable *pBoneGroupTable;

	MEM_SET_ALLOC_NAME("Bone group table");
	pBoneGroupTable = (ts_BoneGroupTable *) memAlloc( sizeof(ts_BoneGroupTable));
	MEM_SET_ALLOC_NAME("Bone groups");
	pBoneGroupTable->pBoneGroups = (ts_BoneGroup *) memAlloc( pBody->usBoneGroups * sizeof(ts_BoneGroup));
	pBoneGroupTable->pBody = pBody;
	pBoneGroupTable->u32BoneGroupEntries = pBody->usBoneGroups;
	CBoneGroupTable__Init(pBoneGroupTable);
	return pBoneGroupTable;
}

void CBoneGroupTable__Destroy(ts_BoneGroupTable *pBoneGroupTable)
{
	memFree(pBoneGroupTable->pBoneGroups);
	memFree(pBoneGroupTable);
	pBoneGroupTable = NULL;
}

void CBoneGroupTable__Process(ts_BoneGroupTable *pBoneGroupTable)
{
	//step through the bonegroup table from beginning to end (this should follow the bone hierarchy if the artist named the bones right)
	//calling the animation function for each bonegroup along the way
	ts_BoneGroup *pBoneGroups = pBoneGroupTable->pBoneGroups;
	u32 u32Index;
	bool bBoneGroupTableUpdated = false;//assume no bones are going to be updated


	for( u32Index=0; u32Index < pBoneGroupTable->u32BoneGroupEntries; u32Index++)
	{
		if( pBoneGroups[u32Index].pBoneGroupAnimFunction )//are we to animate this group?
		{
			if( (pBoneGroups[u32Index].pMotnInf == NULL) || !(pBoneGroups[u32Index].pMotnInf->u32MotnFlags & MOTNINF_FLAG_NOINTERP) )//no interpolation control if we're not supposed to
			{
				if( (pBoneGroups[u32Index].usFlags & BONEGROUP_FLAG_RESTART) )
				{
					//this bone group had a recent channel change: interpolate into the new channel
					if( (pBoneGroups[u32Index].motnInf.u32MotnFlags & MOTNINF_FLAG_INTERP) == 0)//unless this new one is already set to interpolate
					{
						if( pBoneGroups[u32Index].interpTime <= 0)
						{
							//done interpolating, turn off
							pBoneGroups[u32Index].usFlags &= ~BONEGROUP_FLAG_RESTART;
							pBoneGroups[u32Index].motnInf.u32MotnFlags &= ~(MOTNINF_FLAG_INTERP | MOTNINF_FLAG_MATRIXINTERP);
						}
						else
						{
							//tell animation routines to interpolate (unless we're already interp'ing)
							pBoneGroups[u32Index].motnInf.interpTime = pBoneGroups[u32Index].interpTime;
							pBoneGroups[u32Index].motnInf.u32MotnFlags |= (MOTNINF_FLAG_INTERP | MOTNINF_FLAG_MATRIXINTERP);
							pBoneGroups[u32Index].interpTime -= pBoneGroups[u32Index].secondsSinceLastUpdate;
						}
					}
					else
						pBoneGroups[u32Index].usFlags &= ~BONEGROUP_FLAG_RESTART;
				}
			}

			//perform the requested animation function for this bone group
			if(pBoneGroups[u32Index].usFlags & BONEGROUP_FLAG_ANIMATE)
			{
				if(pBoneGroups[u32Index].usFlags & BONEGROUP_FLAG_REANIMATE)
					pBoneGroups[u32Index].motnInf.u32MotnFlags |= MOTNINF_FLAG_NEW_UPDATE;
				else
					pBoneGroups[u32Index].motnInf.u32MotnFlags &= ~MOTNINF_FLAG_NEW_UPDATE;

				pBoneGroups[u32Index].pBoneGroupAnimFunction( pBoneGroups[u32Index].pBoneGroupAnimFunctionContext, pBoneGroupTable->pBody, (BoneGroupID)u32Index, &pBoneGroups[u32Index].motnInf, pBoneGroups[u32Index].secondsSinceLastUpdate );
				bBoneGroupTableUpdated = true;
			}

			//update the per-motion MotnInf structure from the per-group MotnInf struct (unless we're restarting this bonegroup)
			if( pBoneGroups[u32Index].pMotnInf && ((pBoneGroups[u32Index].usFlags & BONEGROUP_FLAG_RESTART) == 0))
				*pBoneGroups[u32Index].pMotnInf = pBoneGroups[u32Index].motnInf;
			else
				//advance the anim timer for this bone group since it's not done with an outside motInf 
				CMotion__AdvanceTimers( &pBoneGroups[u32Index].motnInf, pBoneGroups[u32Index].secondsSinceLastUpdate);
		}

	}
	
	//and concatenate the local bone matrices into final tree
	if(bBoneGroupTableUpdated || pBoneGroupTable->pBody->updatePending)
		boneFinalRunBones( pBoneGroupTable->pBody);
}

void CBoneGroupTable__Init(ts_BoneGroupTable *pBoneGroupTable)
{
	//reset the BoneGroups array
	CBoneGroup__Remove(pBoneGroupTable, ALL_BONE_GROUPS, 0);
}

void CBoneGroup__Add(ts_BoneGroupTable *pBoneGroupTable, BoneGroupBitMask bonegroupBitMask, BoneGroupAnimFunction *pBoneGroupAnimFunction, void *pBoneGroupAnimFunctionContext, ts_MotnInf *pMotnInf, u32 u32Priority, float secondsSinceLastTick, bool bAnimate)
{
	//step through the bonegroupBitMask, store info for any bonegroups not already allocated
	
	ts_BoneGroup *pBoneGroups = pBoneGroupTable->pBoneGroups;
	u32 u32Index;
	BoneGroupBitMask currentBoneGroupBitMask;

	float scaledTime;
	if(pMotnInf)
		scaledTime = CMotion__ScaleTime(pMotnInf, secondsSinceLastTick);
	else
		scaledTime = secondsSinceLastTick;


	for( u32Index=0, currentBoneGroupBitMask=1; u32Index < pBoneGroupTable->u32BoneGroupEntries; u32Index++, currentBoneGroupBitMask<<=1)
	{
		if( currentBoneGroupBitMask & bonegroupBitMask)//is this bonegroup covered by this channel?
			if( pBoneGroups[u32Index].pBoneGroupAnimFunction == NULL || pBoneGroups[u32Index].u32Priority >= u32Priority )//yes, is this bonegroup unallocated or of lower priority?
			{
				//allocate this bonegroup to this channel
				//first see if this is a new channel for this bonegroup - setup to interpolate if it is
				if ( ( pBoneGroups[u32Index].pBoneGroupAnimFunction != pBoneGroupAnimFunction ) || ( pBoneGroups[u32Index].pMotnInf != pMotnInf ) )
				{
					//setup this bone group to interpolate into whatever it's new channel is going to be
					pBoneGroups[u32Index].usFlags = BONEGROUP_FLAG_RESTART;
					pBoneGroups[u32Index].interpTime = (float)ANIMATION_TO_NEW_INTERP_FRAMES / ANIMFRAMES_PER_SECOND;
				}

				pBoneGroups[u32Index].pBoneGroupAnimFunction = pBoneGroupAnimFunction;
				pBoneGroups[u32Index].pBoneGroupAnimFunctionContext = pBoneGroupAnimFunctionContext;
				pBoneGroups[u32Index].u32Priority = u32Priority;
				pBoneGroups[u32Index].pMotnInf = pMotnInf;
				pBoneGroups[u32Index].secondsSinceLastUpdate = scaledTime;
				pBoneGroups[u32Index].usFlags &= ~BONEGROUP_FLAG_REANIMATE;

				if(bAnimate)
				{
					//we're to animate, see if last update as a non-animate update and set flag if so
					if((pBoneGroups[u32Index].usFlags & BONEGROUP_FLAG_ANIMATE) == 0)
						pBoneGroups[u32Index].usFlags |= BONEGROUP_FLAG_REANIMATE;

					pBoneGroups[u32Index].usFlags |= BONEGROUP_FLAG_ANIMATE;
				}
				else
					pBoneGroups[u32Index].usFlags &= ~BONEGROUP_FLAG_ANIMATE;
				if(pMotnInf)
				{
					pBoneGroups[u32Index].motnInf = *pMotnInf;
				}
			}
	}

}

void CBoneGroup__Remove(ts_BoneGroupTable *pBoneGroupTable, BoneGroupBitMask bonegroupBitMask, float interpSeconds)
{
	//step through the bonegroupBitMask, remove bonegroups from this channel
	
	ts_BoneGroup *pBoneGroups = pBoneGroupTable->pBoneGroups;
	u32 u32Index;
	BoneGroupBitMask currentBoneGroupBitMask;

	for( u32Index=0, currentBoneGroupBitMask=1; u32Index < pBoneGroupTable->u32BoneGroupEntries; u32Index++, currentBoneGroupBitMask<<=1)
	{
		if( currentBoneGroupBitMask & bonegroupBitMask)//is this bonegroup covered by this channel?
		{
			//deallocate this bonegroup from this channel
			pBoneGroups[u32Index].pBoneGroupAnimFunction = NULL;

			//setup this bone group to interpolate into whatever it's new channel is going to be
			pBoneGroups[u32Index].usFlags = BONEGROUP_FLAG_RESTART;
			pBoneGroups[u32Index].interpTime = interpSeconds;
			pBoneGroups[u32Index].u32Priority = BONEGROUP_MINIMUM_PRIORITY;
			pBoneGroups[u32Index].motnInf.u32MotnFlags = 0;
		}
	}

}

//to support mirroring animations, we provide a way to "flip" a set of bone groups 
// (a mask) from left-to-right or vice versa

BoneGroupBitMask CBoneGroup__MirrorMask(BoneGroupBitMask bonegroupBitMask, ts_BoneGroupInfo *pBoneGroupInfo)
{
	BoneGroupBitMask outMask = 0;

	ASSERT( pBoneGroupInfo );	//ensure that there is a BoneGroupInfo table

	for( int i = 0; i <= MAX_BONE_GROUP_ID; i++ )
	{
		//set the output bonegroups to be the left-right counterparts of the input bonegroups
		if ( bonegroupBitMask & ( 1 << i ) )
			outMask |= ( 1 << pBoneGroupInfo->BoneGroupMap[i] );
	}

	/* done */

	return( outMask );
}

	


