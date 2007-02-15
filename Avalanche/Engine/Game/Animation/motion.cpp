	/* --------------------------------------------------
 * File    : MOTION.C
 * Created : Fri Mar  5 15:20:12 1999
 * Descript:
 * --------------------------------------------------*/

/******************* includes ****************************/

#include "Game/GamePCH.h"	



/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/

/* --------------------------------------------------
* Local Structures
* --------------------------------------------------*/

struct ts_MFrame
{
	ts_Quatvw		Rot;
	Vector3Packed	Trans;
};

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/
void motApplyKFMBoneGroup( ts_MotionHeader *pMHdr, ts_Body *pBody, float currentTime, BoneGroupID boneGroupID, u32 u32MotionFlags);
void motInterpKFMBoneGroup( ts_MotionHeader *pMHdr, ts_Body *pBody, float currentTime, float secondsSinceLastUpdate, float totalTime, u32 u32MotionFlags, BoneGroupID boneGroupID, bool additiveBlend = false);
void _MFrameFromMat(Graphics4x4 *pMat, ts_MFrame *pMFrame);
void _MFrameToMat(ts_MFrame *pMFrame, Graphics4x4 *pMat);

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/


/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

void CMotion__Animate(void *pContext, ts_Body *pBody, BoneGroupID boneGroupID, ts_MotnInf *pMotnInf, float secondsSinceLastUpdate)
{
	//this is the standard routine to use to animate one bone group of a motion-data driven animation

	if(!(pMotnInf->u32MotnFlags & MOTNINF_FLAG_PAUSE))
  	{
		if (pMotnInf->u32MotnFlags & MOTNINF_FLAG_INTERP)
  		{
			motInterpKFMBoneGroup( pMotnInf->pNextMotionHeader, pBody, pMotnInf->currentTime, secondsSinceLastUpdate, pMotnInf->interpTime, pMotnInf->u32MotnFlags, boneGroupID);
  		}
		else if( pMotnInf->u32MotnFlags & MOTNINF_FLAG_BLEND )
		{
			motInterpKFMBoneGroup( pMotnInf->pNextMotionHeader, pBody, pMotnInf->currentTime, pMotnInf->blendRatio, 1.0, pMotnInf->u32MotnFlags, boneGroupID);
		}
		else if( pMotnInf->u32MotnFlags & MOTNINF_FLAG_BLENDFULL )
		{
			motInterpKFMBoneGroup( pMotnInf->pNextMotionHeader, pBody, pMotnInf->currentTime, 1.0f, 1.0f, pMotnInf->u32MotnFlags, boneGroupID, true);
		}
  		else
  		{
//			//if we're idling on the last frame don't bother computing the same old frame
//			if (pMotnInf->usCurrentTime <= CMotion__GetAnimTime(pMotnInf->pMotionHeader))
			{
				motApplyKFMBoneGroup( pMotnInf->pMotionHeader, pBody, pMotnInf->currentTime, boneGroupID, pMotnInf->u32MotnFlags);
			}
  		}
  	}
}


// This function is used to update the position of a mesh that has no skeleton that is attached to
// a bone from another geoms skeleton
void CMotion__UpdateSlavePositions( CActorInstance* master, CActorInstance* slave, Graphics4x4 &m )
{
	ts_Bone* masterBone;
	if ( !slave->Hidden() && boneGetBone( master->Body(), &masterBone, slave->d_boneName ) )
	{
		// setup our offset/rotation matrix
		Graphics4x4 offset;
		offset.SetTranslation( slave->d_offset );
		offset.CatXRotation( slave->d_rotation.x() );
		offset.CatYRotation( slave->d_rotation.y() );
		offset.CatZRotation( slave->d_rotation.z() );

		// get bone to model matrix
		Graphics4x4 boneToModel;
		boneGetCurrentBoneToModelFromBone( masterBone, &boneToModel );

		// set the final output matrix
		slave->Matrix() = ( offset * boneToModel ) * m;
	}
}

float CMotion__GetAnimTime( ts_MotionHeader *pMHdr )
{
	return( (float)pMHdr->d_u16NumFrames * SECONDS_PER_ANIMFRAME );
}

u16 CMotion__GetAnimFrames( ts_MotionHeader *pMHdr )
{
	return( pMHdr->d_u16NumFrames );
}

void CMotion__SetProceduralPosition( ts_MotnInf *pMotnInf, float zeroToOne )
{
	//set the current animation time as a percentage from 0 to 1
	ASSERT( zeroToOne >= 0.0f && zeroToOne <= 1.0f);
	ASSERT( pMotnInf );

	if( pMotnInf->pMotionHeader && pMotnInf->u32MotnFlags & MOTNINF_FLAG_PROCEDURAL )
	{
		float time = CMotion__GetAnimTime( pMotnInf->pMotionHeader ) * zeroToOne;

		CMotion__SetCurrentTime( pMotnInf, time );
	}
}

void CMotion__SetCurrentTime( ts_MotnInf *pMotnInf, float time)
{
	pMotnInf->currentTime = time;
}

void CMotion__SetCurrentFrame( ts_MotnInf *pMotnInf, unsigned short frame)
{
	pMotnInf->currentTime = (float)frame * SECONDS_PER_ANIMFRAME;
}

u16 CMotion__GetCurrentFrame( ts_MotnInf *pMotnInf )
{
	return SECONDS_TO_ANIMFRAMES( pMotnInf->currentTime );
}

float CMotion__GetCurrentTime( ts_MotnInf *pMotnInf )
{
	return (pMotnInf->currentTime);
}

float CMotion__GetCurrentCompletion( ts_MotnInf *pMotnInf)
{
	//if there is a current motion, return it's percentage completion
	if(pMotnInf->pMotionHeader)
		return( pMotnInf->currentTime / CMotion__GetAnimTime(pMotnInf->pMotionHeader) );
	else
		return( 1.0f );//else assume the previous motion is being queried, so say that it's complete
}

void CMotion__AdvanceTimers(  ts_MotnInf *pMotnInf, float seconds )
{
	float scaledTime = CMotion__ScaleTime(pMotnInf, seconds);

	if( !CMotion__IsInterpDone( pMotnInf ) )
	{
		//we're interpolating, should we advance the timer?
		if( pMotnInf->u32MotnFlags & MOTNINF_FLAG_BLENDINTERP )
			pMotnInf->currentTime += scaledTime;//blending into a moving anim, advance anim

		//decrement interpolation time
		pMotnInf->interpTime -= Math::Abs(scaledTime);//time may be going backward, but interp timer always counts down

		//test for finishing enterpolation
		if( CMotion__IsInterpDone( pMotnInf ) )
			pMotnInf->u32MotnFlags &= ~MOTNINF_FLAG_INTERP;
	}
	else
	{
		//we're not interpolating, advance animation timer (but only if this is NOT a procedural animation)
		if((pMotnInf->u32MotnFlags & MOTNINF_FLAG_PROCEDURAL) == 0)
		{
			float newTime = pMotnInf->currentTime + scaledTime;
			float oldTime = pMotnInf->currentTime;

			//if we don't care if the last frame is displayed, then use the new, updated timer
			if( (pMotnInf->u32MotnFlags & MOTNINF_FLAG_ENSURELASTFRAME) == 0)
			{
				pMotnInf->currentTime = newTime;
			}
			else
			{

				// Make sure that last frame is displayed if time step takes it past the end of the motion

				if ( ( pMotnInf->animationRate < 0 ) )
				{
					// clamp at first frame
					if ( newTime < 0.0f )
						newTime = 0.0f;
				}
				else
				{
					// clamp at last frame
					// actually, clamp at the last frame plus an arbitrary second, so that we don't have
					// floating point comparison inaccuracies causing major random stress.
					float animTime = CMotion__GetAnimTime( pMotnInf->pMotionHeader ) + 1.0f;
					if ( newTime > animTime )
						newTime = animTime;
				}

				pMotnInf->currentTime = newTime;
			}
		}
	}
}

void CMotion__SetAnimationRate( ts_MotnInf *pMotnInf, float animRate )
{
	pMotnInf->animationRate = animRate;
}

float CMotion__GetAnimationRate( ts_MotnInf *pMotnInf )
{
	return pMotnInf->animationRate;
}

void CMotion__SetBlendRatio( ts_MotnInf *pMotnInf, float blendRatio )
{
	ASSERT(Math::Abs(blendRatio) <= 1.0f);
	pMotnInf->blendRatio = blendRatio;
}

float CMotion__GetBlendRatio( ts_MotnInf *pMotnInf )
{
	return pMotnInf->blendRatio;
}

float CMotion__ScaleTime( ts_MotnInf *pMotnInf, float seconds )
{
	float scaledTime = ( seconds * pMotnInf->animationRate );
	return scaledTime;
}

bool CMotion__IsMotionDone( ts_MotnInf *pMotnInf )
{
	bool noMotion = false;
	bool interpolating = false;
	bool procedural = false;

	// done if we don't have valid pointers
	if( pMotnInf == NULL || (pMotnInf->pMotionHeader == NULL) )
		return TRUE;

	// not done if we're interpolating or doing a procedural anim, no matter what the timer is
	if( pMotnInf->u32MotnFlags & MOTNINF_FLAG_INTERP ||
			pMotnInf->u32MotnFlags & MOTNINF_FLAG_PROCEDURAL )
		return FALSE;

	// done if our timer is past the end
	if( pMotnInf->animationRate >= 0.0f && 
			pMotnInf->currentTime >= CMotion__GetAnimTime( pMotnInf->pMotionHeader ) )
		return TRUE;

	// going backwards and past 0?
	if( pMotnInf->animationRate < 0.0f && pMotnInf->currentTime <= 0.0f )
		return TRUE;

	return FALSE;
}

bool CMotion__IsInterpDone( ts_MotnInf *pMotnInf )
{
	if	( pMotnInf
		&& pMotnInf->u32MotnFlags & MOTNINF_FLAG_INTERP
		&& (pMotnInf->interpTime > 0 && pMotnInf->pMotionHeader)
		)
	{
		return FALSE;
	}
	pMotnInf->pMotionHeader = pMotnInf->pNextMotionHeader;
	return TRUE;
}

bool CMotion__IsPaused( ts_MotnInf *pMotnInf )
{
	if	( !CMotion__IsMotionDone(pMotnInf) && (pMotnInf->u32MotnFlags & MOTNINF_FLAG_PAUSE) )
		return TRUE;
	else
		return FALSE;
}

void CMotion__StartInterpolation( ts_MotnInf *pMotnInf, unsigned short usFrames )
{
	float seconds = (float)usFrames * SECONDS_PER_ANIMFRAME;

	pMotnInf->interpTime = seconds;

	pMotnInf->u32MotnFlags |= MOTNINF_FLAG_INTERP;
}



void CMotion__Restart( ts_MotnInf *pMotnInf )
{
	pMotnInf->currentTime = 0.0f;
}

void CMotion__Pause( ts_MotnInf *pMotnInf )
{
	pMotnInf->u32MotnFlags |= MOTNINF_FLAG_PAUSE;
}

void CMotion__UnPause( ts_MotnInf *pMotnInf )
{
	pMotnInf->u32MotnFlags &= ~MOTNINF_FLAG_PAUSE;
}

void CMotion__Terminate( ts_MotnInf *pMotnInf )
{
	pMotnInf->currentTime = CMotion__GetAnimTime(pMotnInf->pMotionHeader);	//set current time to max
}

//Return the bone name that is the mirror of the input bone name or the bone name if there is no mirror
char *LeftRight( char *cBoneName, ts_BoneInfo *pBoneInfo )
{
	//for any bone that has an opposite, return the opposite's bone name

	if (pBoneInfo)
	{
		for (uint i = 0; i < pBoneInfo->numLREntries; i++)
		{
			if (strncmp(cBoneName, pBoneInfo->pLeftRightMap[i].cBoneName1, MAX_BONENAME_LEN) == 0)
				return pBoneInfo->pLeftRightMap[i].cBoneName2;
			if (strncmp(cBoneName, pBoneInfo->pLeftRightMap[i].cBoneName2, MAX_BONENAME_LEN) == 0)
				return pBoneInfo->pLeftRightMap[i].cBoneName1;
		}
	}

	return cBoneName;
}

void motBindKFMMotion( ts_MotionHeader *pMHdr, ts_Body *pBody, BoneGroupID boneGroupID, u32 u32MotionFlags )
{
	//if this bone group hasn't been bound to this motion, do so

	if( pBody->pBoneGroupList[boneGroupID].pMHdr != ((u32MotionFlags & MOTNINF_FLAG_MIRROR) ? MOTION_HEADER_FLIPPED(pMHdr) : pMHdr) )
	{

		//go thru the bones in this group, prep'ing them for application of KFM data
		ts_Bone *pBone = pBody->pBoneGroupList[boneGroupID].pNextBoneInGroup;

		while( pBone )
		{
			//find the motion channel that corresponds to this bone
			if ((pBone->pMChannel = KFMFindChannelFromName( pMHdr, (u32MotionFlags & MOTNINF_FLAG_MIRROR) ? LeftRight(pBone->pModelBone->cBoneName, pBody->pBoneInfo) : pBone->pModelBone->cBoneName)))
				KFMComputeStandardToAction( pBone, &pBone->pMChannel->d_pBoneEntry->d_mBoneToParent );

			pBone = pBone->pNextBoneInGroup;
		}

		//indicate that this bone group has been bound to this motion
		pBody->pBoneGroupList[boneGroupID].pMHdr = (u32MotionFlags & MOTNINF_FLAG_MIRROR) ? MOTION_HEADER_FLIPPED(pMHdr) : pMHdr;
	}
}

void motInterpKFMBoneGroup( ts_MotionHeader *pMHdr, ts_Body *pBody, float currentTime, float secondsSinceLastUpdate, float totalTime, u32 u32MotionFlags, BoneGroupID boneGroupID, bool additiveBlend)
{
	ts_Bone			*pBone;
	Graphics4x4		matrix;
	ts_MFrame		slrpFrame1;
	ts_MFrame		slrpFrame2;
	ts_MFrame		interpFrames;
	float				fDelta;
	float				fEpsilon;

	//compute the interpoation amount
	fDelta = Math::Abs( secondsSinceLastUpdate ) / totalTime;

	//should result in value (0..1) - ensure this is so
	if(fDelta <= 0.0f) fDelta = 0.0f;
	if(fDelta > 1.0f) fDelta = 1.0f;
	fEpsilon = 1 - fDelta;

	motBindKFMMotion( pMHdr, pBody, boneGroupID, u32MotionFlags);

	pBone = pBody->pBoneGroupList[boneGroupID].pNextBoneInGroup;

	while( pBone )
	{
		if( pBone->boneID == 0)
		{
			// JTM: Looking at shortcut bone to see if we need to interp our angles back to 0.
			// JTM: Trying to use the shortcut bone to allow Anim/Programmer angular anim control + interp.
			// HACKHACKHACK
			if ((pBone->LocalBoneMatrix.IsIdentity() == FALSE) && (pBone->lock == 0) )
			{
				_MFrameFromMat(&pBone->LocalBoneMatrix, &slrpFrame1);
				matrix.Identity();
				_MFrameFromMat(&matrix, &slrpFrame2);
				qatLerpBetter(&slrpFrame1.Rot, &slrpFrame2.Rot, fDelta, &interpFrames.Rot);
				qatToRotMat( &(interpFrames.Rot), (Cartesian4x4*)&pBone->LocalBoneMatrix);
				boneDirtyLocalMatrix(pBone);

			}
		}
		else
		{
			Graphics4x4 *pMatrix = &pBone->LocalBoneMatrix;
			Graphics4x4 m, m2;

			//convert the old bone matrix to a quat
			_MFrameFromMat(&pBone->LocalBoneMatrix, &slrpFrame1);

			//get the new, desired bone matrix
			bool bHasTranslations = false;
				
			if( pBone->pMChannel )
				bHasTranslations = KFMApplyBone( pBone->pMChannel, pBone, (Graphics4x4 *)&matrix, (u16)(currentTime * JIFFIES_PER_SECOND), (bool)((u32MotionFlags & MOTNINF_FLAG_MIRROR)!=0) );
			else
				matrix.Identity();

			//convert to a quat
			_MFrameFromMat(&matrix, &slrpFrame2);

			if( additiveBlend )
			{
				qatMul( &slrpFrame1.Rot, &slrpFrame2.Rot, &interpFrames.Rot );
			}
			else
			{
				//do the interpolation
				qatSlerpBetter(&slrpFrame1.Rot, &slrpFrame2.Rot, fDelta, &interpFrames.Rot);
			}

			//convert back to a matrix
			qatToRotMat( &(interpFrames.Rot), (Cartesian4x4*)pMatrix);

			//djr: I don't yet know why, but bones that have translation values in the anim data (like the pelvis) need to have the final bone translations computed differently
			if(bHasTranslations)
			{
				//has translations - do a linear interpolation
				Vector3 pos = slrpFrame1.Trans * fEpsilon + slrpFrame2.Trans * fDelta;
				pMatrix->PokeTranslation(pos);
			}
			else
			{
				//has no translations - translate via transformation
				//interpolating from a matrix - translate bone to object origin, rotate, and translate back
				ts_ModelBone *pMBone = pBone->pModelBone;
				m.SetTranslation(pMBone->vPivot);
				m2.FastMultiply(*pMatrix, m);
				m.SetTranslation(-pMBone->vPivot);
				pMatrix->FastMultiply(m, m2);
			}

			boneDirtyLocalMatrix(pBone);

			//and it's just that easy...
		}



		//do next bone in the group
		pBone = pBone->pNextBoneInGroup;
	}

}

void motApplyKFMBoneGroup( ts_MotionHeader *pMHdr, ts_Body *pBody, float currentTime, BoneGroupID boneGroupID, u32 u32MotionFlags)
{
	ts_Bone *pBone;


	motBindKFMMotion( pMHdr, pBody, boneGroupID, u32MotionFlags);

	pBone = pBody->pBoneGroupList[boneGroupID].pNextBoneInGroup;

	while( pBone )
	{
		if( pBone->pMChannel )
		{
			KFMApplyBone( pBone->pMChannel, pBone, (Graphics4x4 *)&pBone->LocalBoneMatrix, (u16)(currentTime * JIFFIES_PER_SECOND), (bool)((u32MotionFlags & MOTNINF_FLAG_MIRROR)!=0) );
			boneDirtyLocalMatrix(pBone);
		}
		else
			boneSetLocalMatrixToIdentity(pBone);

		pBone = pBone->pNextBoneInGroup;
	}

}


void motInterpKFMKeyFrames( ts_MotionHeader *pMotion, ts_Body *pBody, float currentTime, BoneGroupID boneGroupID)
{}
void motKFMKeyFrames( ts_MotionHeader *pMotion, ts_Body *pBody, float currentTime, BoneGroupID boneGroupID)
{}

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/

void _MFrameFromMat(Graphics4x4 *pMat, ts_MFrame *pMFrame)
{
	qatFromMat(pMat, &pMFrame->Rot);

//yuch-- use GetTranslation
	pMFrame->Trans.x( pMat->tx);
	pMFrame->Trans.y( pMat->ty);
	pMFrame->Trans.z( pMat->tz);
}

void _MFrameToMat(ts_MFrame *pMFrame, Graphics4x4 *pMat)
{
	qatToRotMat( &pMFrame->Rot, (Cartesian4x4*)pMat);

//yuch-- use GetTranslation
	pMat->tx = pMFrame->Trans.x();
	pMat->ty = pMFrame->Trans.y();
	pMat->tz = pMFrame->Trans.z();
}
