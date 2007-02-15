/* --------------------------------------------------
 * File    : G_BONE.H
 * Created : Mon Mar  1 15:04:04 1999
 * Descript:
 * --------------------------------------------------*/
#ifndef __G_BONE_H
#define __G_BONE_H

/******************* includes ****************************/

/* system includes */

/* engine includes */

#include "Game/Misc/Bounds.h" // needed for ts_BoundingBox

#if defined(__cplusplus)
extern "C" {
#endif

/******************* forward declarations ****************/

struct ts_BoneChannel;	// from g_MotCtrl.h
struct ts_Body;			// forward ref

/******************* defines *****************************/

typedef unsigned char BoneID;
#define INVALID_BONEID 0xff

typedef unsigned char BoneGroupID;
#define MAX_BONE_GROUP_ID		31		//largest group ID that will fit in a boneGroupMask
#define DEFAULT_BONE_GROUP_ID 0		//special groupID indicating this bone belongs to no specified group (ie it belongs to the default bone group)
#define NO_BONE_GROUPS_ID		255	//special ID that indicates that this bone is NOT a member of a bone group (including the default bone group)
#define MAX_BONENAME_LEN		16		//bone name used to correlate motion channels: bones names with motion MUST be unique in this many characters

//using a byte (for now) to scale bones
#define SCALE_ONE					32		// this fits in a byte, so this is esstentially 3.5 bit precision
#define SCALE_MAX					255
#define SCALE_ZERO				0

/******************* macros ******************************/

/******************* structures **************************/

//this bone structure is constant and is associated with the model's bones (as opposed to CInstanceBone which is associated with an instance's bone)
struct ts_ModelBone
{
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
	//bone-to-model matrix
	Cartesian4x4		boneToModel;

	//added for KFM support:
	Graphics4x4			m_bonetostandard;


	ts_ModelBone		*pChildren;
	ts_ModelBone		*pSiblings;

	unsigned short		usStartVtxIdx;	//first vertex associated with this bone
	unsigned short		usNumVtxIdxs;	//number of vertices associated with this bone
	
	unsigned short		usStartCtlVtxIdx;//first control vertex associated with this bone
	unsigned short		usNumCtlVtxIdxs;//number of control vertices associated with this bone

	
	unsigned short		usStartNrmlIdx;//first normal associated with this bone
	unsigned short		usNumNrmlIdxs;	//number of normals associated with this bone

	Vector3Packed		vPivot;		//the pivot point of this bone, in base pose
	Vector3Packed		vBoneEnd;	//the end point of this bone, in base pose

	float					fBoneLen;	//the length of this bone


	char					cBoneName[MAX_BONENAME_LEN];//bone name - necessary for KFM motion (for bonename/action channel matching)


	//added for variable skeleton support
	ts_ModelBone		*pNextBoneInGroup;	//singly linked list ptr to next bone in this bone group (allows quickly traversing a bone group)
	ts_ModelBone		*m_parent;		//ptr to the bone that is this bone's parent (NULL if no parent)


	BoneGroupID			boneGroupID;	//ID number of the group this bone belongs to
	BoneID				boneID;			//unique ID number of this particular bone
	unsigned char		bOrientXYZ;		//bool indicating interpretation of orient values: if TRUE -> use ucOrientX, ucOrientY, uc OrientZ. if FALSE -> ucOrientLikeValue, ucRollLikeValue, ucOrient2LikeValue.

	union{
		unsigned char	ucOrientLikeValue;	//0->not an orient like bone, else 1..100
		unsigned char	ucOrientX;				//If ORIENTXYZ_FLAG set, then Orienttatioon about X axis.
	};

	union{
		unsigned char	ucOrient2LikeValue;	//0->not an orient like bone, else 1..100. 
		unsigned char	ucOrientY;				//If ORIENTXYZ_FLAG set, then Orienttatioon about Y axis.
	};

	union{
		unsigned char	ucRollLikeValue;		//0->not a roll like bone, else 1..100
		unsigned char	ucOrientZ;				//If ORIENTXYZ_FLAG set, then Orienttatioon about Z axis.
	};

	BoneID				OrientBone;		//index to the bone that this boone is oriented- or rolled-like
	BoneID				Orient2Bone;	//index to additional bone that this boone is oriented-like (if ucOrient2LikeValue != 0)

	ts_BoundingBox		mBoneBox;				// a bounding box surrounding all verticies attached to this bone (on the PS2, the data
														// format does not allow us to determine this easily at run time).

	u8						ucScaleLikeValue;		//0->not a scale-like bone else 1...100 scale like percentage
	u8						bUnscaleBone;			//0->not an un-scale bone, else this bone does nothing but undo any scaling of parent bone

	char					pad[6];					//this structure must be an even multiple of 16 bytes (for PS2)

/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
};

//this part of the bone structure is duplicated for each instance that uses a model's bone
//it is that part of a bone that gets updated (as a result of animation, IK, bone visibility changes, etc).
//for PS2 compatibility, this structure MUST be 16-byte aligned
struct ts_Bone
{
	Graphics4x4			Matrix;	//final, composite matrix
	Graphics4x4			LocalBoneMatrix;		//local matrix for this bone
	Graphics4x4			m_standardtoaction;	//used by KFM animation

	ts_Bone				*m_parent;		//ptr to the bone that is this bone's parent (NULL if no parent)
	ts_Bone				*pNextBoneInGroup;	//singly linked list ptr to next bone in this bone group (allows quickly traversing a bone group)
	ts_Bone				*pChildren;
	ts_Bone				*pSiblings;

	ts_Body				*pBody;			//ptr back to our body

	ts_BoneChannel		*pMChannel;			//ptr to the motion channel data animating us

	ts_ModelBone		*pModelBone;			//ptr to the model bone
	
	u8						scale;			//value to scale bone by (divided by 128, i.e., 128==1.0, 255=1.99, 0==0.0)
	u8						bLocalMatrixChanged	: 1,	//bool denoting that the local matrix has changed and thus the composite matrix needs to be recomputed
							bIdentity				: 1,	//bool denoting if this local matrix is identity
							Pad						: 6;
	BoneID				boneID;				//unique ID number of this particular bone (actually the bone's index into the array of ts_Bone and CInstanceBone)
	char					lock;			//for Jon Mats - to enable/disable bone0 from being interpolated
};

struct ts_BoneGroupList
{
	void					*pMHdr;					//the motion that this bone group has been most recently bound to
	ts_Bone				*pNextBoneInGroup;	//start of the bone group list
};

/* maps a bonename to it's bone group number */
struct ts_GroupBoneMap
{
	BoneGroupID			boneGroupID;	//ID number of the group this bone belongs to
	char					cBoneName[MAX_BONENAME_LEN];//bone name - necessary for KFM motion (for bonename/action channel matching)
};

/* associates bones on one side of a bi-symmetric model to the equivalent bone name on the opposite side of body (for mirroring motions) */
struct ts_LeftRightMap
{
	char					cBoneName1[MAX_BONENAME_LEN];//bone name of a bone that has a mirrored equivalent
	char					cBoneName2[MAX_BONENAME_LEN];//bone name of the bone that is the mirrored equivalent
};

/* info to map bonenames to bonegroup numbers and bonenames to it's left-right counterpart */
struct ts_BoneInfo
{
	u16					numGBEntries;
	u16					numLREntries;
	ts_GroupBoneMap	*pGroupBoneMap;
	ts_LeftRightMap	*pLeftRightMap;
};

/* info to map a bonegroups to it's left-right counterpart */
struct ts_BoneGroupInfo
{
	u32				BoneGroupMap[MAX_BONE_GROUP_ID];
};

//kinda cheesey, but saves space and cycles: use the low-order bit of pMotionHeader ptr to remember if it was bound to a flipped anim or not
#define MOTION_HEADER_LR_FLIPPED			1
#define MOTION_HEADER_PTR_MASK			(~MOTION_HEADER_LR_FLIPPED)
#define MOTION_HEADER_FLIPPED(ptr)		((ts_MotionHeader *)((unsigned int)ptr | MOTION_HEADER_LR_FLIPPED))

struct ts_Body
{
	//constant: same for each instance
	unsigned short		usBones;
	unsigned short		usBoneGroups;
	ts_ModelBone		*pMBones;
	ts_BoneInfo			*pBoneInfo;
	ts_BoneGroupInfo	*pBoneGroupInfo;

	//variable: different for each instance
	ts_Bone				*pBones;
	ts_BoneGroupList	*pBoneGroupList;//ptr to the bonegrouplist array for this body
	bool					updatePending;
};


/* --------------------------------------------------
 * Exported Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
void boneDirtyLocalMatrix(ts_Bone *pBone);
void boneSetLocalMatrix(ts_Bone *pBone, Graphics4x4 *mLocalBoneMatrix);
void boneSetLocalMatrixToIdentity(ts_Bone *pBone);
Graphics4x4 *boneGetLocalMatrix(ts_Bone *pBone);

ts_Bone *boneGetBonePtr(ts_Body *pBody, BoneID boneID);
ts_ModelBone *boneGetModelBone(ts_Body *pBody, BoneID boneID);
void boneFinalRunBones( ts_Body *pBody);
Graphics4x4 *boneRunBonesToBone( ts_Bone *pBone);
void boneCopyLocalMatricies( ts_Body *pBody, Graphics4x4 *pMat, bool bExchange );
void boneComputeBoneToParent(ts_Bone *pBone, Graphics4x4 *pMat);
void boneComputeNeutralBoneToParent(ts_Bone *pBone, Cartesian4x4 *pMat);
void boneSetBoneToParent(ts_Bone *pBone, Graphics4x4 *pMat);
Cartesian4x4 *boneGetBoneToModel(ts_Bone *pBone);
void boneSetBoneToNeutralBone(ts_Bone *pBone, Graphics4x4 *pMat);
void boneComputeCurrentNeutralBoneToModel(ts_Bone *pBone, Graphics4x4 *pMat);
void boneSetVisibility(ts_Bone *pBone, bool bVisible);
inline bool boneGetVisibility(ts_Bone *pBone) { return (pBone && pBone->scale != SCALE_ZERO); }
void boneSetScale(ts_Bone *pBone, float scale);
void boneSetBoneTreeIdentity(ts_Bone *pBone);

inline Graphics4x4 *boneGetLocalMatrix(ts_Bone *pBone)
{
	return (pBone) ? &pBone->LocalBoneMatrix : NULL;
}

inline ts_Bone *boneGetBonePtr(ts_Body *pBody, BoneID boneID)
{
	ASSERT(pBody);
	return (boneID < pBody->usBones) ? &pBody->pBones[boneID] : NULL;
}

inline ts_ModelBone *boneGetModelBone(ts_Body *pBody, BoneID boneID)
{
	ASSERT(pBody);
	return (boneID < pBody->usBones) ? &pBody->pMBones[boneID] : NULL;
}

int boneGetBone(ts_Body *pBody, ts_Bone **pBone, const char *BoneName);
int boneGetBoneID(ts_Body *pBody, BoneID *boneID, const char *BoneName);
void boneGetMatrix(ts_Body *pBody, BoneID boneID, Graphics4x4 *mLocalBoneMatrix);
void boneSetMatrix(ts_Body *pBody, BoneID boneID, Graphics4x4 *mLocalBoneMatrix);
void boneGetCompositeMatrix(ts_Body *pBody, BoneID boneID, Graphics4x4 *mCompositeBoneMatrix);
void boneGetParentCompositeMatrix(ts_Body *pBody, BoneID boneID, Graphics4x4 *mCompositeBoneMatrix);
void boneGetCurrentBoneToModel(ts_Body *pBody, BoneID boneID, Graphics4x4 *mCurrentBoneToModel);
void boneGetEndPosition(ts_Body *pBody, BoneID boneID, Vector3 *pBonePosition);
void boneGetStartPosition(ts_Body *pBody, BoneID boneID, Vector3 *pBonePosition);
void boneGetInitialPosition(ts_Body *pBody, BoneID boneID, Vector3 *pBonePosition);
void boneGetInitialStartPosition(ts_Body *pBody, BoneID boneID, Vector3 *pBonePosition);
void boneGetPositionFromBone(ts_Bone *pBone, Vector3 *pBonePosition);
void boneGetStartPositionFromBone(ts_Bone *pBone, Vector3 *pBonePosition);
void boneGetInitialPositionFromBone(ts_Bone *pBone, Vector3 *pBonePosition);
void boneGetInitialStartPositionFromBone(ts_Bone *pBone, Vector3 *pBonePosition);
void boneGetCurrentBoneToModelFromBone(ts_Bone *pBone, Graphics4x4 *mCurrentBoneToModel);


#if defined(__cplusplus)
}
#endif

#endif __G_BONE_H
