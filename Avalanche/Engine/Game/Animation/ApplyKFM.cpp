/*
 *
 * ApplyKFM.C
 * 
 * Derived from Hash's AMSDK code (from KFM.CPP and BONE.CPP and MODEL.CPP).
 * Applies KFM data (as translated into an .MDB file via MOTNSET3.EXE) to a bone group.
 * KFM data allows varying number of bones in bodies that it applies to, as well as allowing the bodies TO BE OF DIFFERENT SIZES.
 * Thus, one animation can be applied to any number of skeletons, substantially reducing storage requirements.
 *
 * Note: Hash's implentation uses column-major matrices, ours is row-major (thus the reverse order of matrix multiplies).
 *
 * D. Ross, 3/14/00
 *
 * ned says 11/22/03: for better data size, we use linear interpolation between key frames instead of spline curves.
 * it's been this way for a long time now. i have just switched code over to MotionSet5, Adam's re-write of MotnSet4
 * which incorporates better curve analysis before spitting out key frame data
 *
 */

/******************* includes ****************************/

#include "Game/GamePCH.h"

enum { XTRANS, YTRANS, ZTRANS, XSCALE, YSCALE, ZSCALE, XQUAT, YQUAT, ZQUAT, WQUAT, NUMCHANNELITEMS };

struct TSQ
{
   Vector3		      translate;
   Vector3   		   scale;
   ts_Quatvw	  		quat;
};

/* linear interpolation of key frame data */

float InterpolateValueLinear( ts_BoneChannelItem *pBoneChannelItem, const u16 u16Jiffies, const unsigned int index)
{
	/* linear interpolation - from a to b */

	float evalx = u16Jiffies;
	float xa = pBoneChannelItem->d_pKeyFrames[index].d_u16Time;
	float xb = pBoneChannelItem->d_pKeyFrames[index+1].d_u16Time;
	float ya = ANIMVALUE_TO_FLOAT(pBoneChannelItem->d_pKeyFrames[index].d_s16Value, pBoneChannelItem->d_fScaleValue);
	float yb = ANIMVALUE_TO_FLOAT(pBoneChannelItem->d_pKeyFrames[index+1].d_s16Value, pBoneChannelItem->d_fScaleValue);

	float evaly = ((evalx - xa) / (xb - xa)) * (yb - ya);
	evaly = evaly + ya;

	return evaly;
}

float Interpolate(ts_BoneChannelItem *pBoneChannelItem, const u16 u16Jiffies)
{
	/* maybe no key frame data (but at a non-default value for unit TSQ) */

	if (pBoneChannelItem->d_pKeyFrames == NULL)
		return(pBoneChannelItem->d_fFlatNonDefaultValue);

	/* past end of key frame data? then return last key frame value */

   if (u16Jiffies >= pBoneChannelItem->d_pKeyFrames[pBoneChannelItem->d_u16NumKeyFrames-1].d_u16Time)
      return(ANIMVALUE_TO_FLOAT(pBoneChannelItem->d_pKeyFrames[pBoneChannelItem->d_u16NumKeyFrames-1].d_s16Value, pBoneChannelItem->d_fScaleValue));
	
	/* before start of key frame data? then return first key frame value */
	
	if (u16Jiffies <= pBoneChannelItem->d_pKeyFrames[0].d_u16Time)
      return(ANIMVALUE_TO_FLOAT(pBoneChannelItem->d_pKeyFrames[0].d_s16Value, pBoneChannelItem->d_fScaleValue));

	/* typically, we'll still be within last-used key frame pair */

   if (u16Jiffies >= pBoneChannelItem->d_pKeyFrames[pBoneChannelItem->d_u16CachedKeyFrameIndex].d_u16Time)
	{
		for (int nIndex = pBoneChannelItem->d_u16CachedKeyFrameIndex; nIndex < pBoneChannelItem->d_u16NumKeyFrames-1; nIndex++)
		{
			if (u16Jiffies == pBoneChannelItem->d_pKeyFrames[nIndex].d_u16Time)
			{
				/* right at first key frame's time */

            pBoneChannelItem->d_u16CachedKeyFrameIndex = nIndex;
				return(ANIMVALUE_TO_FLOAT(pBoneChannelItem->d_pKeyFrames[nIndex].d_s16Value, pBoneChannelItem->d_fScaleValue));
			}
			else if (u16Jiffies == pBoneChannelItem->d_pKeyFrames[nIndex+1].d_u16Time)
			{
				/* right at second key frame's time */

				pBoneChannelItem->d_u16CachedKeyFrameIndex = nIndex + 1;	// typically time will go forward from here
				return(ANIMVALUE_TO_FLOAT(pBoneChannelItem->d_pKeyFrames[nIndex+1].d_s16Value, pBoneChannelItem->d_fScaleValue));
			}
			else if (u16Jiffies < pBoneChannelItem->d_pKeyFrames[nIndex+1].d_u16Time)
			{
				/* in between key frames, interpolate */

				pBoneChannelItem->d_u16CachedKeyFrameIndex = nIndex;
				return(InterpolateValueLinear( pBoneChannelItem, u16Jiffies, nIndex));
			}
		}
		ASSERT(false);	// shouldn't reach here
		return(ANIMVALUE_TO_FLOAT(pBoneChannelItem->d_pKeyFrames[pBoneChannelItem->d_u16NumKeyFrames-1].d_s16Value, pBoneChannelItem->d_fScaleValue));
	}

	/* probably moving backwards through animation data */

	for (int nIndex = pBoneChannelItem->d_u16CachedKeyFrameIndex-1; nIndex >= 0; nIndex--)
	{
		if (u16Jiffies == pBoneChannelItem->d_pKeyFrames[nIndex].d_u16Time)
		{
			/* right at first key frame's time */

			pBoneChannelItem->d_u16CachedKeyFrameIndex = nIndex;
			return(ANIMVALUE_TO_FLOAT(pBoneChannelItem->d_pKeyFrames[nIndex].d_s16Value, pBoneChannelItem->d_fScaleValue));
		}
		else if (u16Jiffies == pBoneChannelItem->d_pKeyFrames[nIndex+1].d_u16Time)
		{
			/* right at second key frame's time */

			pBoneChannelItem->d_u16CachedKeyFrameIndex = nIndex+1;
			return(ANIMVALUE_TO_FLOAT(pBoneChannelItem->d_pKeyFrames[nIndex+1].d_s16Value, pBoneChannelItem->d_fScaleValue));
		}
		else if (u16Jiffies > pBoneChannelItem->d_pKeyFrames[nIndex].d_u16Time)
		{
			/* in between key frames, interpolate */

			pBoneChannelItem->d_u16CachedKeyFrameIndex = nIndex;
			return(InterpolateValueLinear( pBoneChannelItem, u16Jiffies, nIndex));
		}
	}
	ASSERT(false);	// shouldn't reach here
	return(ANIMVALUE_TO_FLOAT(pBoneChannelItem->d_pKeyFrames[0].d_s16Value, pBoneChannelItem->d_fScaleValue));
}

/* evaluating animation data for a point in time (done by GetTSQ) starts with a "unit" translation/scale/quaternion value,
then the components that exist in a particular channel are evaluated on top of the unit values */

struct UNITTSQ : public TSQ
{
	UNITTSQ()
	{ 
		translate.Set(0.0f, 0.0f, 0.0f);
		scale.Set(1.0f, 1.0f, 1.0f);
		quat.V.Set(0.0f, 0.0f, 0.0f);
		quat.W = 1.0f;
	}
};
static UNITTSQ UnitTSQ;

bool GetTSQ(TSQ *tsq, ts_BoneChannel *pBoneChannel, const unsigned short time, bool bXMirror)
{
	unsigned short usBitArray = pBoneChannel->d_u16ChannelItemBitArray;
	ts_BoneChannelItem *pBoneChannelItem = pBoneChannel->d_pBoneChannelItem;
	*tsq = UnitTSQ;
	bool bReturnValue = false;

   if(usBitArray & ((1<<XTRANS)|(1<<YTRANS)|(1<<ZTRANS)))//translations are pretty rare - discard quickly
	{
		bReturnValue = true;//we got translations, indicate so

		if(usBitArray & (1<<XTRANS))
			tsq->translate.x( Interpolate( pBoneChannelItem++, time));
		if(usBitArray & (1<<YTRANS))
			tsq->translate.y( Interpolate( pBoneChannelItem++, time));
		if(usBitArray & (1<<ZTRANS))
			tsq->translate.z( Interpolate( pBoneChannelItem++, time));
	}
	
   if(usBitArray & ((1<<XSCALE)|(1<<YSCALE)|(1<<ZSCALE)))//scales are pretty rare - discard quickly
	{
		if(usBitArray & (1<<XSCALE))
			tsq->scale.x( Interpolate( pBoneChannelItem++, time));
		if(usBitArray & (1<<YSCALE))
			tsq->scale.y( Interpolate( pBoneChannelItem++, time));
		if(usBitArray & (1<<ZSCALE))
			tsq->scale.z( Interpolate( pBoneChannelItem++, time));
	}
   
	if(usBitArray & (1<<XQUAT))
		tsq->quat.V.x( Interpolate( pBoneChannelItem++, time));
   if(usBitArray & (1<<YQUAT))
		tsq->quat.V.y( Interpolate( pBoneChannelItem++, time));
   if(usBitArray & (1<<ZQUAT))
		tsq->quat.V.z( Interpolate( pBoneChannelItem++, time));
   if(usBitArray & (1<<WQUAT))
		tsq->quat.W = Interpolate( pBoneChannelItem++, time);

#if 0
	static FILE * file=NULL;
	if(time == 0)
	{
		if(file == NULL )
			file = fopen("xquat.log", "wt");
		else
		{
			static count = 0;
			count++;
			if(count > 1)
			{
			fclose(file);
			file = NULL;
			}
		}
	}


	float fsecs = JIFFIES_TO_SECONDS(time);
		
	int frs = fsecs * 30;
	frs %= 30;

	int secs = fsecs;
	secs %= 60;

	int mins = fsecs / 60;

	if(file)
		fprintf(file, "XQ %f, time = %d:%d:%d\n", tsq->quat.V.x, mins, secs, frs);
#endif


	if( bXMirror )//flip along the X axis?
	{
#if 1 //either of these #if paths should work to mirror about the X-axis
		tsq->quat.V.x(tsq->quat.V.x() * -1.0f);
		tsq->quat.W *= -1.0f;
#else
		tsq->quat.V.y(tsq->quat.V.y() * -1.0f);
		tsq->quat.V.z(tsq->quat.V.z() * -1.0f);
#endif
		tsq->translate.x(tsq->translate.x() * -1.0f);
	}
   
	qatNormalize(&tsq->quat);

	return bReturnValue;
   
}

ts_BoneChannel *KFMFindChannelFromName( ts_MotionHeader *pMotionHeader, char cBoneName[MAX_BONENAME_LEN] )
{
	int i;
	ts_BoneChannel *pBoneChannel = (ts_BoneChannel *)pMotionHeader->d_pBoneChannel;
	
	for( i=0; i<pMotionHeader->d_u16NumBoneChannels; i++)
	{
		if( strnicmp(pBoneChannel[i].d_pBoneEntry->d_Name, cBoneName, MAX_BONENAME_LEN-1) == 0)	//-1 since it appears filter chain is null terminating the bonename
			return &pBoneChannel[i];
	}
	return NULL;
}

void ComputeParentToBone(ts_Bone *pBone, Matrix3x3 *m33)
{
   if ( pBone->m_parent )
	{
		Matrix3x3 a(pBone->pModelBone->m_bonetostandard);
		Matrix3x3 b(pBone->m_parent->pModelBone->m_bonetostandard);
		b.Transpose();
		*m33 = b * a;
	}
   else
		m33->Identity();
}

void KFMComputeStandardToAction(ts_Bone *pBone, Matrix3x3 *bonetoparent)
{
	Matrix3x3 m33, m233;
	Graphics4x4 m34;

	pBone->pModelBone->m_bonetostandard.Invert(pBone->m_standardtoaction);
	ComputeParentToBone( pBone, &m33);
	m233 = *bonetoparent * m33;
	m34.Set(m233);
	pBone->m_standardtoaction.FastMultiply(m34, pBone->m_standardtoaction);
}

void CreateMatrix( ts_Bone *pBone, Graphics4x4 *pM34, TSQ tsq )
{
   Graphics4x4 actionmatrix;
	Graphics4x4 m34;
	
	qatToRotMat( &tsq.quat, (Cartesian4x4*)&actionmatrix);
	actionmatrix.ReverseCatScale(tsq.scale.x(), tsq.scale.y(), tsq.scale.z());
	actionmatrix.PokeTranslation(tsq.translate);
	m34.FastMultiply(actionmatrix, pBone->m_standardtoaction);
	pM34->FastMultiply(pBone->pModelBone->m_bonetostandard, m34);
}

bool KFMApplyBone( ts_BoneChannel *pBoneChannel, ts_Bone *pBone, Graphics4x4 *pMatrix, u16 jiffies, bool bXMirror )
{
	TSQ tsq;

	bool bReturn = GetTSQ(&tsq, pBoneChannel, jiffies, bXMirror);

   CreateMatrix( pBone, pMatrix, tsq);

	return bReturn;
}
