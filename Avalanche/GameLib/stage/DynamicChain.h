/*
**
**  File:   DynamicChain.h
**  Date:   April 19, 2001
**  By:     Bryant Collard
**  Desc:   Applies dynamics to chained bones in a model.
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/stage/DynamicChain.h $
**  $Revision: #74 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#ifndef DYNAMIC_CHAIN_H
#define DYNAMIC_CHAIN_H

#include "stage/Chain.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "stage/cactor.h" // Just to get ENABLE_RECORD

struct ts_Bone;
struct ts_ModelBone;
struct ts_PatchSOM;
class OBB;
class CObjectList;
class CCustomChainConstraint;
class CDynamicChainData;
template<class Type> class CParam;

class CDynamicChain : public CChain
{
  public:
	CDynamicChain();
	virtual ~CDynamicChain();
	virtual bool InitializeFail(void);
	virtual bool SetNumLinks(int i_numLinks);
	virtual bool InitLink(int i_link, ts_Bone* i_bone);
	virtual bool CompleteInit(CInstance* i_inst,
			Graphics4x4* i_modelToWorld);
	bool TryAddPartner(CDynamicChain* i_queryChain);
	void LoadPartnershipParams(void);
	void LoadParams(void);
	virtual void Reset(void);
	void Update(float i_deltaSec, CObjectList* i_objectList);
	void PrepUpdate(void);
	void TryUpdate(float i_deltaSec, CObjectList* i_objectList);
	void EnforcePartnershipOBBConstraints(CObjectList* i_objectList);
	void EndUpdate(void);
	void ClearExternalForces(void);
	int ConstrainToPoint(Vector3CRef i_point, int i_desiredNode,
			int i_iterations = 10);
	void ConstrainToHole(OBB* i_obb, float i_xHalfWidth,
			float i_zHalfWidth, int i_iterations = 10);
	float GetCurrentTimeStep(void) const {return(d_deltaSec);}
	bool GetAttachPoint(int i_link, ts_Bone* &o_attachBone,
			Vector3 &o_attachPoint) const;
	void SetAttachPoint(int i_link, ts_Bone* i_attachBone,
			Vector3CRef i_attachPoint);
	float GetLengthSquared(int i_link) const;
	float GetConstraintRadius(int i_link) const;
	Vector3 GetVelocityInWorld(int i_link) const;
	void SetControlMask(int i_link, int i_mask);
	int GetControlMask(int i_link) const;
	void SetMass(int i_link, float i_mass);
	float GetMass(int i_link);
	void SetVelocityInWorld(int i_link, Vector3 &i_velocity);
	void AddExternalForce(int i_link, Vector3 &i_force);
	void SetExternalForce(int i_link, Vector3CRef i_force);
	Vector3& GetExternalForce(int i_link);
	void SetTargetDynamics(int i_link, float i_naturalFrequency,
			float i_dampingFactor);
	void SetDragCoefficient(int i_link, float i_dragCoefficient);
	void SetIterationRate(float i_iterationRate)
		{d_iterationRate = i_iterationRate;}
	float GetIterationRate(void) {return d_iterationRate;}
	void SetGlobalDamping(float i_damping) {d_globalDamping = i_damping;}
	void SetGravity(Vector3 &i_gravity) {d_gravity = i_gravity;}
	void SetGravity(float i_xGravity, float i_yGravity, float i_zGravity)
		{d_gravity.Set(i_xGravity, i_yGravity, i_zGravity);}
	void SetCustomConstraint(CCustomChainConstraint* i_customConstraint)
		{d_customConstraint = i_customConstraint;}
	bool IsInOBB(int i_link, OBB &i_obb);
	bool DidCollide(void) {return(d_collided);}
	void DrawConstraint(int i_startColor = 0, bool i_drawBone = false);
	void DrawBoxConstraint(bool i_draw) {d_drawBoxConstraint = i_draw;}
	void SetBoxConstraintParent(ts_Bone* i_parent, Cartesian4x4 &i_boxToParent);
	void SetBoxConstraintExtent(Vector3CRef i_extent)
		{d_boxConstraintExtent = i_extent;}
	void SetLocations(Vector3 *i_particleInWorld, int i_iterations = 10);
	void SetConstraintScale(float i_scale) {d_constraintScale = i_scale;}
	void SetParticleInWorld(int i_link, Vector3CRef i_particleInWorld);

#ifdef ENABLE_RECORD
	bool CreateRecordBuffer(int i_numTicks);
	void DeleteRecordBuffer(void);
	void RecordState(int i_entry);
	void LoadState(int i_entry);
#endif

	static const int LINK_ATTACH;
	static const int LINK_TARGET;
	static const int LINK_GRAVITY;
	static const int LINK_DRAG;
	static const int LINK_BOX;
	static const int LINK_NO_OBB;
  protected:
	CDynamicChain* d_dynamicPartner;
	struct SDynamicLink
	{
		float d_lengthSquared;
		int d_controlMask;
		ts_Bone* d_control;
		Vector3 d_attachPoint;
		Vector3 d_particleInWorld;
		Vector3 d_velocityInWorld;
		Vector3 d_newTargetInWorld;
		Vector3 d_targetInWorld;
		Vector3 d_externalForce;
		float d_baseInvMass;
		float d_currInvMass;
		float d_targetNaturalFrequency;
		float d_targetDampingFactor;
		float d_dragCoefficient;
		float d_constraintRadius;
	} *d_dynamicLink;
	float d_iterationRate;
	float d_globalDamping;
	Vector3 d_gravity;
	CCustomChainConstraint* d_customConstraint;
	Vector3 d_windVelocity;
	float d_relativeWindSpeed;
	int d_lastOrient;
	float d_deltaSec;
	bool d_collided;
/*
 *	Box constraint.
 */
	bool d_drawBoxConstraint;
	bool d_interactiveBoxConstraint;
	char d_boxConstraintParentName[MAX_BONENAME_LEN];
	Vector3 d_boxConstraintInParent;
	Vector3 d_boxConstraintRotation;
	ts_Bone* d_boxConstraintParent;
	Cartesian4x4 d_boxConstraintToParent;
	Vector3 d_boxConstraintExtent;
	bool d_boxConstraintActive;
	Graphics4x4 d_boxConstraintToWorld;
	Graphics4x4 d_worldToBoxConstraint;
	float d_constraintScale;

  private:
	enum ESeparatingPlane
	{
		NO_INTERSECTION,
		POS_X_FACE,
		NEG_X_FACE,
		POS_Y_FACE,
		NEG_Y_FACE,
		POS_Z_FACE,
		NEG_Z_FACE,
		CROSS_AXIS_FACE
	};

	bool RigidAttach(int i_link);
	bool TargetBase(int i_link);
	bool ConstrainToObb(int i_link);
	void EnforceConstraints(CObjectList* i_objectList, int i_iterations);
	void EnforceOBBConstraint(int i_link, OBB* i_obb);
	void EnforceOBBConstraint(int i_link, CObjectList* i_objectList);
	ESeparatingPlane ResolveOBBConstraint(OBB* i_obb, Vector3 &i_linkCenter,
			Vector3 &i_linkSize, Vector3 &i_obbCorner, float &o_depthSquared,
			Vector3 &o_crossAxisDisplacement);
	void ComputeConstraintInfo(OBB* i_obb, Vector3 &i_point,
			float i_constraintRadius, Vector3 &o_pointInOBB, Vector3 &o_extent);
	void ComputeLinkConstraintInfo(OBB* i_obb, int i_link, Vector3 &o_endInOBB,
			Vector3 &o_linkCenter, Vector3 &o_linkSize, Vector3 &o_obbCorner);
	bool InsideOBB(Vector3 &i_point, Vector3 &i_extent);
	int FindPenetrateFace(Vector3 &i_start, Vector3 &i_end, Vector3 &i_extent);
	int FindClosestFace(Vector3 &i_point, Vector3 &i_extent);
	void MoveToFace(int i_face, OBB* i_obb, Vector3 &io_pointInOBB,
			Vector3 &i_extent, Vector3 &o_pointInWorld);
	bool ImposePointConstraint(Vector3CRef i_point, int i_closestNode,
			int &io_minNode);
	void ConstrainNodeToHole(int i_node, OBB* i_obb, float i_xHalfWidth,
			float i_zHalfWidth, Vector3 &o_nodeInOBB);
	void ConstrainLinkToHole(int i_link, OBB* i_obb, float i_xHalfWidth,
			float i_zHalfWidth);
	void EndUpdateDebug(void);

	static Vector3 s_startInOBB;

	friend class CParam<CDynamicChainData>;

#ifdef ENABLE_RECORD
	struct SDynamicChainRecordEntry
	{
		struct SRecordDynamicLink
		{
			Vector3 d_particleInWorld;
			Vector3 d_newTargetInWorld;
		} *d_dynamicLink;
		float d_deltaSec;
	} *d_dynamicChainRecordBuffer;
#endif
};

class CDynamicChainData
{
  public:
	CDynamicChainData() {d_refChain = NULL; d_dynamicLink = NULL;}
	~CDynamicChainData() {delete[] d_dynamicLink;}

	union
	{
		int d_numLinks;
		CDynamicChain* d_refChain;
	};
	struct SLinkData
	{
		int d_controlMask;
		float d_baseInvMass;
		float d_targetNaturalFrequency;
		float d_targetDampingFactor;
		float d_dragCoefficient;
	} *d_dynamicLink;

	float d_iterationRate;
	float d_globalDamping;
	Vector3 d_gravity;
	bool d_interactiveBoxConstraint;
	char d_boxConstraintParentName[MAX_BONENAME_LEN];
	Vector3 d_boxConstraintInParent;
	Vector3 d_boxConstraintRotation;
	Vector3 d_boxConstraintExtent;
	float d_constraintScale;
};

class CCustomChainConstraint
{
  public:
	virtual void ApplyConstraint(void) = 0;
};

#define DYNAMIC_CHAIN_HPP
#include "stage/DynamicChain.hpp"

#endif // DYNAMIC_CHAIN_H
