/*
**
**  File:   AnimatedChain.h
**  Date:   April 19, 2001
**  By:     Bryant Collard
**  Desc:   A dynamic chain run by the animation system.
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/stage/AnimatedChain.h $
**  $Revision: 20 $
**      $Date: 6/18/03 12:30p $
**    $Author: Bcollard $
**
*/

#ifndef ANIMATED_CHAIN_H
#define ANIMATED_CHAIN_H

#include "stage/DynamicChain.h"
#include "platform/BaseType.h"
#include "Game/Animation/Bone.h"
#include "Game/Animation/Motion.h"
#include "Game/Animation/MotionChannel.h"

class CAnimatedChain;
class CObjectList;

class CActorChains
{
  public:
	CActorChains();
	~CActorChains();
	static CActorChains* BuildChains(CActorInstance* i_inst);
	void LoadParams(void);
	void ResetChains(void);
	void DisableChains(void);
	void EnableChains(void);
	void SetDeltaSec(float i_deltaSec);
	void SetObjectList(CObjectList* i_objectList);
	void SetConstraintScale(float i_scale);
	void GetExtents(Vector3 &io_minExtent, Vector3 &io_maxExtent,
			bool i_compare = false);
	CAnimatedChain* GetHead(void) {return(d_head);};
	bool DidCollide(void);
	void RegisterAnimation(CActorInstance* i_inst, BoneGroupBitMask i_bitMask);
	static void RunCallback(void* i_context, ts_Body* i_body,
			BoneGroupID i_boneGroup, ts_MotnInf* i_motionInfo);
	void Run(BoneGroupID i_boneGroup, ts_MotnInf *i_motionInfo);
  private:
	CAnimatedChain *d_head;
	MotionChannelTable* d_motionChannelTable;
	MotionChannel* d_motionChannel;
};

class CAnimatedChain : public CDynamicChain
{
  public:
	CAnimatedChain();
	~CAnimatedChain();
	bool Initialize(CActorInstance* i_inst, char* i_name);
	virtual bool IsName(char* i_queryName);
	void SetNext(CAnimatedChain* i_next) {d_next = i_next;};
	CAnimatedChain* GetNextChain(void);
	CAnimatedChain* GetNextPartnership(void);
	CAnimatedChain* GetPartner(void);
	BoneGroupID GetBoneGroup(void);
	void SetDeltaSec(float i_deltaSec) {d_currentDeltaSec = i_deltaSec;};
	void Run(ts_MotnInf *i_motionInfo);
	void SetObjectList(CObjectList* i_objectList)
		{d_objectList = i_objectList;};
	bool IsObjectList(void) {return(d_objectList != NULL);}
	void SetDisable(bool i_disabled) {d_disabled = i_disabled;}
  private:
	CAnimatedChain* d_next;
	float d_currentDeltaSec;
	CObjectList* d_objectList;
	bool d_disabled;
};

inline CAnimatedChain* CAnimatedChain::GetNextChain(void)
{
	if (d_partner)
		return((CAnimatedChain*)d_partner);
	return (d_next);
}

inline CAnimatedChain* CAnimatedChain::GetNextPartnership(void)
{
	return (d_next);
}

inline CAnimatedChain* CAnimatedChain::GetPartner(void)
{
	return((CAnimatedChain*)d_partner);
}

#endif // ANIMATED_CHAIN_H
