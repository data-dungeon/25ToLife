#ifndef CAPSULE_BONES_NODE_H
#define CAPSULE_BONES_NODE_H

#include "collide/Node.h"
#include "CollideHelper/CapsuleNode.h"
#include "CollideHelper/ObbBonesNode.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Game/Animation/Bone.h"

//////////////////////////////// CCapsuleBonesNode /////////////////////////////////

class CCapsuleBonesNode : public CapsuleNode
{
public:
	// Construction
	CCapsuleBonesNode();
	virtual ~CCapsuleBonesNode();

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
	void SetFitToAttached(bool i_fitToAttached)	{d_fitToAttached = i_fitToAttached;}
	void SetFixedOffset( Vector3CRef offset ) {d_fixedOffset = true; fixedOffset = offset;}
	void UseFixedOffset( bool bUse ) {d_fixedOffset = bUse;}
	Vector3 GetFixedOffset( void ) { if( d_fixedOffset ) return fixedOffset; return Vector3( 0.0f, 0.0f, 0.0f ); }

	void SetAxisExtents( float i_minAxis, float i_maxAxis ) { minAxis = i_minAxis; maxAxis = i_maxAxis; }
	void SetRadiusExtents( float i_minRadius, float i_maxRadius ) { minRadius = i_minRadius; maxRadius = i_maxRadius; }

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
	bool d_fixedOffset;

	float minAxis, maxAxis;
	float minRadius, maxRadius;

	Vector3 fixedOffset;
};

#endif // OBB_BONES_NODE_H
