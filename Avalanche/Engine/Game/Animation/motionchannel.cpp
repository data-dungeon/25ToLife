//============================================================================
//=
//= MotionChannel.cpp
//=
//============================================================================

#include "Game/GamePCH.h"

#include "Layers/Timing.h"

//============================================================================

MotionChannelTable::MotionChannelTable( u32 maxMotionChannelEntries, ts_Body* body )
{
	d_motionChannels = new MotionChannel[ maxMotionChannelEntries ];
	memset( d_motionChannels, 0, sizeof( MotionChannel ) * maxMotionChannelEntries );	// remove when MotionChannel is a class.
	d_maxMotionChannelEntries = maxMotionChannelEntries;
	d_boneGroupTable = CBoneGroupTable__Create( body );
	d_numBoneGroups = body->usBoneGroups;
}

//============================================================================

MotionChannelTable::~MotionChannelTable()
{
	ASSERT( d_boneGroupTable );
	CBoneGroupTable__Destroy( d_boneGroupTable );
	delete [] d_motionChannels;
}

//============================================================================

bool MotionChannelTable::Process( float secondsSinceLastTick, bool bAnimate )
{
	//go thru the motionchannel table in priority order and setup the bonegroup table
	
	static u32 u32ProcessMarker = 1;
	MotionChannel *pMotionChannel;
	u32 u32Index;
	bool bFoundMotionChannel = FALSE;

	Profiler__Start( PROFILE_MOTION_PROCESSING );

	//first do all the NON-BLENDED motions
	// set up new already-been-processed-marker 
	u32ProcessMarker++;

	//find the next highest priority and add to the bonegroup table
	while( (pMotionChannel = FindNextPriorityMotionChannel( u32ProcessMarker, false ) ) )
	{
		CBoneGroup__Add( d_boneGroupTable, pMotionChannel->d_boneGroupBitMask, pMotionChannel->d_boneGroupAnimFunction, pMotionChannel->d_boneGroupAnimFunctionContext, pMotionChannel->d_motnInf, pMotionChannel->d_priority, secondsSinceLastTick, bAnimate );
		bFoundMotionChannel = TRUE;
	}

	//and apply the motion data to the body
	CBoneGroupTable__Process( d_boneGroupTable );

#if 1
	//next do all the BLENDED motions
	// set up new already-been-processed-marker 
	u32ProcessMarker++;
	ts_BoneGroupTable *blendTable = NULL;

	//find the next highest priority and add to the bonegroup table
	while( ( pMotionChannel = FindNextPriorityMotionChannel( u32ProcessMarker, true ) ) )
	{
		if ( blendTable == NULL )
			blendTable = CBoneGroupTable__Create( d_boneGroupTable->pBody );
		CBoneGroup__Add(blendTable, pMotionChannel->d_boneGroupBitMask, pMotionChannel->d_boneGroupAnimFunction, pMotionChannel->d_boneGroupAnimFunctionContext, pMotionChannel->d_motnInf, pMotionChannel->d_priority, secondsSinceLastTick, bAnimate);
		bFoundMotionChannel = TRUE;
	}

	//and apply the motion data to the body
	if ( blendTable )
	{
		CBoneGroupTable__Process( blendTable );
		CBoneGroupTable__Destroy( blendTable );
	}
#endif
	
	//do any control manipulation
	for ( u32Index = 0; u32Index < d_maxMotionChannelEntries; u32Index++ )
	{
		pMotionChannel = &(d_motionChannels[u32Index]);
		
		if( pMotionChannel->d_boneGroupAnimFunction)//valid motionchannel?
		{
			if( pMotionChannel->d_motnInf && !(pMotionChannel->d_motnInf->u32MotnFlags & MOTNINF_FLAG_PAUSE) )
			{
				//update this anim's timer
				CMotion__AdvanceTimers(pMotionChannel->d_motnInf, secondsSinceLastTick);
				
				//did an animation just finish?
				if( CMotion__IsMotionDone( pMotionChannel->d_motnInf ) )
				{
					if( pMotionChannel->d_motnInf->u32MotnFlags & MOTNINF_FLAG_LOOP )//give LOOP flag precidence over ONESHOT
					{
						if( pMotionChannel->d_motnInf->animationRate < 0.0f )
							//going backwards in time, reset to the end of the animation
							pMotionChannel->d_motnInf->currentTime = pMotionChannel->d_motnInf->currentTime + CMotion__GetAnimTime(pMotionChannel->d_motnInf->pMotionHeader);
						else
							//a looped motion just finished - restart
							pMotionChannel->d_motnInf->currentTime = pMotionChannel->d_motnInf->currentTime - CMotion__GetAnimTime(pMotionChannel->d_motnInf->pMotionHeader);
					}
					else
					if( pMotionChannel->d_motnInf->u32MotnFlags & MOTNINF_FLAG_ONESHOT)
					{
						//a one-shot just finished, free up this channel
						RemoveChannel( pMotionChannel->d_owner );
					}
				}
			}

			
		}
	}

	Profiler__Stop( PROFILE_MOTION_PROCESSING );

	return bFoundMotionChannel;
}

//============================================================================

MotionChannel* MotionChannelTable::FindNextPriorityMotionChannel( u32 processMarker, bool blendFlag ) const
{
	//look thru the un-processed motionchannels finding the highest priority one
	MotionChannel *pBestMotionChannel = NULL;
	u32 u32HighestPriority = 0xffffffff;
	int i;
	
	for( i = 0 ; i < (int)d_maxMotionChannelEntries ; i++ )
	{
		if ( d_motionChannels[i].d_boneGroupAnimFunction )//valid motionchannel?
		{
			if ( d_motionChannels[i].d_processedMarker != processMarker )//been processed?
			{
				if ( d_motionChannels[i].d_priority <= u32HighestPriority )
				{
					if ( d_motionChannels[i].d_motnInf )
					{
						if ( blendFlag )
						{
							//only looking for blended motions
							if ( d_motionChannels[i].d_motnInf->u32MotnFlags & (MOTNINF_FLAG_BLEND | MOTNINF_FLAG_BLENDFULL) )
							{
								pBestMotionChannel = &d_motionChannels[i];
								u32HighestPriority = d_motionChannels[i].d_priority;
							}
						}
						else
						{
							//only looking for non-blended motions
							if ( (d_motionChannels[i].d_motnInf->u32MotnFlags & (MOTNINF_FLAG_BLEND | MOTNINF_FLAG_BLENDFULL)) == 0 )
							{
								pBestMotionChannel = &d_motionChannels[i];
								u32HighestPriority = d_motionChannels[i].d_priority;
							}
						}
					}
					else
					{
						if ( !blendFlag )//if no pMotInf flags to tell us wether to blend or not, then animate only during the non-blended pass
						{
							pBestMotionChannel = &d_motionChannels[i];
							u32HighestPriority = d_motionChannels[i].d_priority;
						}
					}
				}
			}
		}
	}

	if ( pBestMotionChannel )
	{
		pBestMotionChannel->d_processedMarker = processMarker;
	}

	return pBestMotionChannel;
}

//============================================================================

bool MotionChannelTable::AddChannel(   BoneGroupAnimFunction* pBoneGroupAnimFunction,
													void* pBoneGroupAnimFunctionContext,
													u32 u32Priority,
													BoneGroupBitMask boneGroupBitMask,
													ts_MotnInf* pMotnInf,
													MotionChannel** ppOwner )
{
	u32 u32Index;

	ASSERT(pBoneGroupAnimFunction);
	ASSERT(*ppOwner == NULL);

	// !!!!
	// !!!!	NOTICE:
	// !!!!	IF YOU'RE ASSERTING BELOW, MAKE SURE THE DBL WAS EXPORTED WITH -BONES=1 OPTION
	for( u32Index=0; u32Index < d_maxMotionChannelEntries; u32Index++ )
	{
		if ( d_motionChannels[ u32Index ].Unused() )
		{
			d_motionChannels[u32Index].d_priority = u32Priority;
			d_motionChannels[u32Index].d_boneGroupAnimFunction = pBoneGroupAnimFunction;
			d_motionChannels[u32Index].d_boneGroupAnimFunctionContext = pBoneGroupAnimFunctionContext;
			d_motionChannels[u32Index].d_boneGroupBitMask = boneGroupBitMask;
			d_motionChannels[u32Index].d_motnInf = pMotnInf;
			d_motionChannels[u32Index].d_owner = ppOwner;
			*ppOwner = &d_motionChannels[u32Index];
			if( pMotnInf)
			{
				CMotion__SetAnimationRate(pMotnInf, 1.0f);	//init animation rate to default of 1.0

				if( (pMotnInf->u32MotnFlags & MOTNINF_FLAG_NOINTERP) == 0)
				{
					//we're starting up an animated channel - we must assume that the bone groups in this animation need to be interpolated
					CMotion__StartInterpolation( pMotnInf, IK_TO_ANIMATION_INTERP_FRAMES );
					//motion channels are added with pMotionHeader pointing to the motion to play.  Since we're instructing the motion to interpolate and we don't know what the previous anim was, set the motion to interp to to be the current one.
					pMotnInf->pNextMotionHeader = pMotnInf->pMotionHeader;
					//tell code that the pMFrame array may not be valid, so interpolate from the bone's local matrices
					pMotnInf->u32MotnFlags |= MOTNINF_FLAG_MATRIXINTERP;
				}
//#define DEBUG_PRINT
#ifdef DEBUG_PRINT
				dbgPrint("MotChanAdd: %s\n", pMotnInf->pMotionHeader->name);
#endif
			}
			return( true);
		}
	}

	ASSERTS(false, "No unused motion channels to play this animation - too many concurrent animations or not enough channels allocated");
	
	return false;
}

//============================================================================

void MotionChannelTable::RemoveChannel( MotionChannel **pMotionChannel, float interpSeconds )
{
	if ( *pMotionChannel && !(*pMotionChannel)->Unused() )
	{
		(*pMotionChannel)->d_boneGroupAnimFunction = NULL;
		CBoneGroup__Remove( d_boneGroupTable, (*pMotionChannel)->d_boneGroupBitMask, interpSeconds );
		ASSERT( pMotionChannel == (*pMotionChannel)->d_owner );
		*((*pMotionChannel)->d_owner) = NULL;//tell owner he no longer has this channel
		if(*pMotionChannel)
		{
			ASSERT(0);
			*pMotionChannel = NULL;//note that this channel is no longer used
		}
	}
}

//============================================================================

void MotionChannel::Change(	BoneGroupAnimFunction*	pBoneGroupAnimFunction,
										void*					      pBoneGroupAnimFunctionContext,
										u32		               u32Priority,
										BoneGroupBitMask			bonegroupBitMask,
										void*							pMotnInf )
{
	ASSERT( d_boneGroupAnimFunction );
	d_priority								= u32Priority;
	d_boneGroupAnimFunction				= pBoneGroupAnimFunction;
	d_boneGroupAnimFunctionContext	= pBoneGroupAnimFunctionContext;
	d_boneGroupBitMask					= bonegroupBitMask;
	d_motnInf								= (ts_MotnInf *) pMotnInf;
}

//============================================================================
