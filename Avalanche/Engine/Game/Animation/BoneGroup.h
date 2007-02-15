/************************************/
/*                                  */
/*  Bone BoneGroups                 */
/*                                  */
/*    DJRoss, 1/17/99               */
/*                                  */
/*                                  */
/*                                  */
/************************************/

#ifndef __BONEGROUP_H
#define __BONEGROUP_H

/******************* includes *****************************/

/* system includes */

/* engine includes */

#include "Game/Animation/Motion.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/******************* forward declarations ****************/

struct ts_Body;	// in g_Bone.h
struct ts_MotnInf;

/******************* defines *****************************/

#define BONEGROUP_FLAG_RESTART	0x0001
#define BONEGROUP_FLAG_ANIMATE	0x0002
#define BONEGROUP_FLAG_REANIMATE	0x0004

/******************* macros ******************************/

/******************* structures **************************/

typedef void (BoneGroupAnimFunction)(void *, ts_Body *, BoneGroupID, ts_MotnInf *, float);

struct ts_BoneGroup
{
	BoneGroupAnimFunction *pBoneGroupAnimFunction;	//function to call to apply animation data to each bone group
	void				*pBoneGroupAnimFunctionContext;	//context for function to call to apply animation data to each bone group
	//the MotnInf structure gets updated by the animation application code, but multiple groups per motion would result in multiple updates per frame.
	//so maintain a ptr to the real motnInf and make a copy of it for each group that gets updated.
	//Then update the real one with one of the updated group ones, thus getting updated only once per frame.
	ts_MotnInf		*pMotnInf;	//ptr to the real, program controlled MotnInf struct
	ts_MotnInf		motnInf;		//local copy of motnInf for animating each bone group (will be updated for each bone group)
	unsigned short	usFlags;
	float			secondsSinceLastUpdate; //how much time has passed since last update cycle
	u32				u32Priority;
	float			interpTime;
};


struct ts_BoneGroupTable
{
	u32				u32BoneGroupEntries; //number of groups allocated for group table
	ts_Body			*pBody;
	ts_BoneGroup	*pBoneGroups; //ptr to array of BoneGroups
};

/******************* global variables ********************/

/******************* global prototypes *******************/

ts_BoneGroupTable *CBoneGroupTable__Create(ts_Body *pBody);
void CBoneGroupTable__Destroy(ts_BoneGroupTable *pBoneGroupTable);
void CBoneGroupTable__Process(ts_BoneGroupTable *pBoneGroupTable);
void CBoneGroupTable__Init(ts_BoneGroupTable *pBoneGroupTable);

BoneGroupBitMask CBoneGroup__MirrorMask(BoneGroupBitMask bonegroupBitMask, ts_BoneGroupInfo *pBoneGroupInfo);

void CBoneGroup__Add(ts_BoneGroupTable *pBoneGroupTable, BoneGroupBitMask bonegroupBitMask, BoneGroupAnimFunction *pBoneGroupAnimFunction, void *pBoneGroupAnimFunctionContext, ts_MotnInf *pMotnInf, u32 u32Priority, float secondsSinceLastTick, bool bAnimate);
void CBoneGroup__Remove(ts_BoneGroupTable *pBoneGroupTable, BoneGroupBitMask bonegroupBitMask, float interpSeconds);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __BONEGROUP_H

