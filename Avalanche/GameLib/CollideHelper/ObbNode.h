/*
**
**  File:   ObbNode.h
**  Date:   July 12, 2004
**  By:     Bryant Collard
**  Desc:   A collision node using a single obb and an associated actor.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/CollideHelper/ObbNode.h $
**  $Revision: #4 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#ifndef OBB_NODE_H
#define OBB_NODE_H

#include "CollideHelper/ProtagonistNode.h"
#include "collide/obb.h"
#include "Interfaces/CollisionIF.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"

class CCollisionObject;
class CBoxExtents;

class CObbNode : public CProtagonistNode, public COrientedCollisionIF
{
  public:
	// Construction
	CObbNode();

	// Override CCollisionNode
	virtual CCollisionObject* BeginObjectTest(void) {return(&d_obb);}

	// Override CProtagonistNode
	virtual CCollisionObject* GetCollisionObject(void) {return(&d_obb);}
	virtual CInterface* GetInterface(uint i_id);
	virtual bool GetCollisionCull(CCollisionCull &o_cull);
	virtual void UpdateCollisionHull(float i_deltaSec, XZRect &i_collisionHull);
	virtual void SyncWithActor(void);
	virtual void SyncWithAnimation(void);
	virtual uint8 Material(CCollisionPair* i_pair);
	virtual void Draw(CCollisionEvent* i_event);

	// Override CCollisionIF
	virtual void SetSurfaceType(uint8 i_surfaceType);
	virtual void SetSurfaceName(const char* i_surfaceName);
	virtual Vector3 GetCollisionInWorld(void);
	virtual Vector3 GetCollisionInActor(void);
	virtual void SetCollisionInActor(Vector3CRef i_collisionInActor);
	virtual void MoveCollisionInActor(Vector3CRef i_offsetInActor);

	// Override COrientedCollisionIF
	virtual DirCos3x3 GetCollisionToWorld(void);
	virtual DirCos3x3 GetCollisionToActor(void);
	virtual void SetCollisionToActor(const DirCos3x3 &i_collisionToActor);
	virtual void RotateCollisionToActor(const DirCos3x3 &i_newToOldCollision);
	virtual float GetXSize(void);
	virtual float GetYSize(void);
	virtual float GetZSize(void);
	virtual void SetXSize(float i_size);
	virtual void SetYSize(float i_size);
	virtual void SetZSize(float i_size);
	virtual void SetSize(Vector3CRef i_size);
	virtual void ScaleXSize(float i_scale);
	virtual void ScaleYSize(float i_scale);
	virtual void ScaleZSize(float i_scale);
	virtual void ScaleSize(Vector3CRef i_scale);

	// Set the obb size and location. The min and max values are expressed in
	// a coordinate system aligned with the obb and with its origin at the
	// actor's origin.
	void SetExtents(Vector3CRef i_min, Vector3CRef i_max);

	// Set the obb to contain the actor's geometry. If i_index is negative, the
	// box will contain all valid geometries; otherwise, only the specified
	// geometry will be used. If an invalid geometry is specified, no changes
	// will be made and the function will return false. If the minimum y-value
	// ends up within i_yOriginBuffer of 0, adjustments will be made to make the
	// minimum y-value equal to i_yOriginBuffer. The intended use of this
	// capability is to keep the bottom of the obb from intersecting the ground
	// in models where the origin is supposed to be at the bottom of the model.
	bool FitObbToActor(int i_index = -1, float i_yOriginBuffer = 0.0f);

  protected:
	bool FindModelExtents(int i_index, Vector3 &o_min, Vector3 &o_max);
	bool TestGeometryExtents(int i_index, CBoxExtents &io_boxExtents);

	OBBInstance d_obb;
	Vector3 d_obbInActor;
	DirCos3x3 d_obbToActor;
	uint8 d_surfaceType;
};

#endif // OBB_NODE_H
