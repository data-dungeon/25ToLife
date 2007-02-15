#ifndef __CAPSULENODE_H_INCLUDED__
#define __CAPSULENODE_H_INCLUDED__

#include "CollideHelper/ProtagonistNode.h"
#include "collide/Capsule.h"

class CapsuleNode : public CProtagonistNode
{
public:
	// Construction
	CapsuleNode();

	// Override CCollisionNode
	virtual CCollisionObject* BeginObjectTest(void) {return(&capsule);}

	// Override CProtagonistNode
	virtual CCollisionObject* GetCollisionObject(void) {return(&capsule);}
	virtual bool GetCollisionCull(CCollisionCull &o_cull);
	virtual void UpdateCollisionHull(float i_deltaSec, XZRect &i_collisionHull);
	virtual void SyncWithActor(void);
	virtual void SyncWithAnimation(void);
	virtual uint8 Material(CCollisionPair* i_pair);
	virtual void Draw(CCollisionEvent* i_event);

	// Override CCollisionIF
	virtual void SetSurfaceType(uint8 i_surfaceType);
	virtual void SetSurfaceName(const char* i_surfaceName);

	void SetExtents(Vector3CRef origin, Vector3CRef end, float radius);

protected:
	Capsule capsule;
	uint8 d_surfaceType;
	Vector3 d_originInActor;
	Vector3 d_endInActor;
	float d_radius;
};

#endif