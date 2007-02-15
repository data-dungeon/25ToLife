//============================================================================
//=
//= MotionChannel.h
//=
//============================================================================

#ifndef __MOTIONCHANNEL_H
#define __MOTIONCHANNEL_H

#include "Game/Animation/Motion.h"		// for BoneGroupBitMask typedef
#include "Game/Animation/BoneGroup.h"	// for BoneGroupAnimFunction declaration

//============================================================================

#define IK_TO_ANIMATION_INTERP_FRAMES		2
#define ANIMATION_TO_NEW_INTERP_FRAMES		5

//============================================================================

class MotionChannel
{
public:
	inline bool Unused() const                               { return d_boneGroupAnimFunction == NULL; }

	inline void SetPriority( u32 priority )                  { d_priority = priority; }

	inline void SetBoneGroupBitMask( BoneGroupBitMask mask ) { d_boneGroupBitMask = mask; }

	void Change( BoneGroupAnimFunction*	pBoneGroupAnimFunction,
					 void*				      pBoneGroupAnimFunctionContext,
					 u32		               u32Priority,
					 BoneGroupBitMask       bonegroupBitMask,
					 void*                  pMotnInf);

public:
	u32							d_priority;								//motionchannel priority: 0=highest...
	u32							d_processedMarker;					//marker indicating whether a given motionchannel has been processed yet
	BoneGroupAnimFunction*	d_boneGroupAnimFunction;			//function to call to apply animation data to each bone bonegroup
	void*							d_boneGroupAnimFunctionContext;	//context for the anim function (ie, C++ data object)
	BoneGroupBitMask			d_boneGroupBitMask;					//bit mask of what bonegroup's this motionchannel's function/data can handle
	ts_MotnInf*					d_motnInf;								//ptr to animation status structure - what to do for IK?
	MotionChannel**			d_owner;									//ptr to ptr to the motion channel (so owner can be cleared when channel terminates)
};

//============================================================================

class MotionChannelTable
{
public:
	MotionChannelTable( u32 maxMotionChannelEntries, ts_Body* body );
	~MotionChannelTable();

	bool Process( float secondsSinceLastTick, bool animate );

	bool AddChannel( BoneGroupAnimFunction* pBoneGroupAnimFunction,
							void* pBoneGroupAnimFunctionContext,
							u32 u32Priority,
							BoneGroupBitMask boneGroupBitMask,
							ts_MotnInf* pMotnInf,
							MotionChannel** ppOwner );

	void RemoveChannel( MotionChannel** pMotionChannel, float interpSeconds = ( (float)ANIMATION_TO_NEW_INTERP_FRAMES * SECONDS_PER_ANIMFRAME ) );

private:
	MotionChannel*		FindNextPriorityMotionChannel( u32 processMarker, bool blendFlag ) const;

private:
	u32						d_maxMotionChannelEntries;
	ts_BoneGroupTable*	d_boneGroupTable;
	u32						d_numBoneGroups;
	MotionChannel*			d_motionChannels;
};

//============================================================================


//============================================================================

#endif // __MOTIONCHANNEL_H
