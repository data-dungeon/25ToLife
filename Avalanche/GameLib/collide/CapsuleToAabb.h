#ifndef __CAPSULE_TO_AABB_H_INCLUDED__
#define __CAPSULE_TO_AABB_H_INCLUDED__


// Gamelib includes //
#include "collide/Intersection.h"
#include "collide/Capsule.h"
#include "collide/AabbObject.h"

class CCollisionStack;

class CCapsuleToAabbIntersection : public CCollisionIntersection
{
public:
	CCapsuleToAabbIntersection(Capsule &i_capsule, CAabbCollisionObject &i_aabb);
	CCapsuleToAabbIntersection(Capsule &i_capsule, CAabbCollisionObject &i_aabb, CCollisionStack &i_stack);

	static CCapsuleToAabbIntersection* New(Capsule &i_capsule, CAabbCollisionObject &i_aabb,
		CCollisionStack* i_stack);

	static CCollisionIntersection* TestForIntersection( Capsule* i_capsule,
		CAabbCollisionObject* i_aabb, CCollisionStack* i_stack);

	void ClearTests(void) {d_validFlags = 0;}

	bool SeparatingAxisTest(void);
	bool RadiusTest(void);

private:
	// no default constructor
	CCapsuleToAabbIntersection();

	Capsule* d_capsule;
	CAabbCollisionObject* d_aabb;

	uint d_validFlags;

	bool d_radiusIntersection;
	bool d_intersects;

	static const uint SEPARATING_AXIS_TEST;
	static const uint RADIUS_TEST;
};

#endif
