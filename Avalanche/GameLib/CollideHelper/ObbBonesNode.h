/*
**
**  File:   ObbBonesNode.h
**  Date:   July 12, 2004
**  By:     Bryant Collard
**  Desc:   A collision node made up of a parent OBB and child OBBs attached
**          to bones.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/CollideHelper/ObbBonesNode.h $
**  $Revision: #7 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#ifndef OBB_BONES_NODE_H
#define OBB_BONES_NODE_H

#include "collide/Node.h"
#include "CollideHelper/ObbNode.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Game/Animation/Bone.h"

class CBoxExtents;

/////////////////////////////// CAttachedObbNode ///////////////////////////////

class CAttachedObbNode : public CCollisionNode
{
  public:
	// Construction
	CAttachedObbNode();

	// Override CCollisionNode
	virtual CCollisionObject* BeginObjectTest(void) {return(&d_obb);}

	// Access
	Vector3 &GetObbInWorld(void) {return((Vector3 &)d_obb.Position());}
	DirCos3x3 &GetObbToWorld(void) {return(d_obb.Orientation());}
	Vector3 &GetObbInActor(void) {return(d_obbInActor);}
	DirCos3x3 &GetObbToActor(void) {return(d_obbToActor);}
	float GetXSize(void) {return(d_obb.GetConstAABB().XSize());}
	float GetYSize(void) {return(d_obb.GetConstAABB().YSize());}
	float GetZSize(void) {return(d_obb.GetConstAABB().ZSize());}
	Vector3 &GetSizeInBone(void) {return(d_obbExtentInBone);}
	ts_Bone* GetBone(void) {return(d_bone);}
	CCollisionObject* GetCollisionObject(void) {return(&d_obb);}
	uint8 Material(void) {return(d_surfaceType);}

	// Test
	void TestExtents(CBoxExtents &io_boxExtents);

	// Setup routines
	void SetScaleUsed(bool i_scaleUsed) {d_scaleUsed = i_scaleUsed;}
	void Setup(CActor* i_actor, ts_Bone* i_bone);
	void SetObbInBone(Vector3CRef i_obbInBone);
	void MoveObbInBone(Vector3CRef i_offsetInBone);
	void SetObbToBone(const DirCos3x3 &i_obbToBone);
	void RotateObbToBone(const DirCos3x3 &i_newToOldObb);
	void SetXSize(float i_size);
	void SetYSize(float i_size);
	void SetZSize(float i_size);
	void SetSize(Vector3CRef i_size);
	void SetXSizeInBone(float i_size);
	void SetYSizeInBone(float i_size);
	void SetZSizeInBone(float i_size);
	void SetSizeInBone(Vector3CRef i_size);
	void ScaleXSize(float i_scale);
	void ScaleYSize(float i_scale);
	void ScaleZSize(float i_scale);
	void ScaleSize(Vector3CRef i_scale);
	void SetSurfaceType(uint8 i_surfaceType);
	void SetSurfaceName(const char* i_surfaceName);

	// Sync
	void SyncWithBone(void);
	void SyncWithActor(void);

  private:
	Vector3 ComputeScale(void);
	void SyncSize(Vector3CRef i_scale);

	CActor* d_actor;
	ts_Bone* d_bone;
	OBBInstance d_obb;
	Vector3 d_obbExtentInBone;
	Cartesian4x4 d_obbToBone;
	Vector3 d_obbInActor;
	DirCos3x3 d_obbToActor;
	bool d_scaleUsed;
	uint8 d_surfaceType;
};

//////////////////////////////// CObbBonesNode /////////////////////////////////

class CObbBonesNode : public CObbNode
{
  public:
	// Construction
	CObbBonesNode();
	virtual ~CObbBonesNode();

	// Override CCollisionNode
	virtual bool IgnoreIntersection(CCollisionIntersection* i_intersection)
		{return(true);}
	virtual void EndObjectTest(ETestResult i_result,
			CCollisionPair* i_pair = NULL);
	virtual CCollisionNode* GetChild(void);

	// Override CProtagonistNode
	virtual void SyncWithActor(void);
	virtual void SyncWithAnimation(void);
	virtual uint8 Material(CCollisionPair* i_pair);
	virtual void Draw(CCollisionEvent* i_event);

	// Override CCollisionIF
	virtual void SetCollisionInActor(Vector3CRef i_collisionInActor);
	virtual void MoveCollisionInActor(Vector3CRef i_offsetInActor);

	// Override COrientedCollisionIF
	virtual void SetXSize(float i_size);
	virtual void SetYSize(float i_size);
	virtual void SetZSize(float i_size);
	virtual void SetSize(Vector3CRef i_size);
	virtual void ScaleXSize(float i_scale);
	virtual void ScaleYSize(float i_scale);
	virtual void ScaleZSize(float i_scale);
	virtual void ScaleSize(Vector3CRef i_scale);

	// Setup routines
	int SetupAttached(const char** i_boneNames, int i_numBones = -1);
	void SetScaleUsed(bool i_scaleUsed);
	void SetObbInBones(Vector3Packed* i_obbInBone,
			int i_stride = sizeof(Vector3Packed));
	void MoveObbInBones(Vector3Packed* i_offsetInBone,
			int i_stride = sizeof(Vector3Packed));
	void SetObbToBones(DirCos3x3* i_obbToBone, int i_stride = sizeof(DirCos3x3));
	void RotateObbToBones(DirCos3x3* i_newToOldObb,
			int i_stride = sizeof(DirCos3x3));
	void SetSizes(Vector3Packed* i_size, int i_stride = sizeof(Vector3Packed));
	void SetSizeInBones(Vector3Packed* i_size,
			int i_stride = sizeof(Vector3Packed));
	void ScaleSizes(Vector3Packed* i_scale, int i_stride = sizeof(Vector3Packed));
	void SetSurfaceTypes(uint8 *i_surfaceType, int i_stride = sizeof(uint8));
	void SetSurfaceNames(const char* i_name, int i_stride = sizeof(char));

	// Turn fitting on or off.
	void SetFitToAttached(bool i_fitToAttached)
		{d_fitToAttached = i_fitToAttached;}

	// Clamp the fit obb. The min and max values are expressed in a coordinate
	// system with its origin at the actor's origin and oriented with the obb.
	Vector3 &GetClampMin() {return(d_clampMin);}
	Vector3 &GetClampMax() {return(d_clampMax);}

	// Access
	CAttachedObbNode* GetAttached(const char* i_name);
	CAttachedObbNode* GetAttached(int i_index);

  protected:
	ts_Bone* GetBone(const char* i_name);

	int d_numAttached;
	CAttachedObbNode* d_attached;
	int d_currTest;
	bool d_scaleUsed;
	bool d_fitToAttached;
	Vector3 d_clampMax;
	Vector3 d_clampMin;
};

#endif // OBB_BONES_NODE_H
