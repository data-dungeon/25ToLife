#ifndef SPHERE_TO_CAPSULE_H
#define SPHERE_TO_CAPSULE_H

#include "collide/Intersection.h"
#include "collide/Sphere.h"
#include "collide/Capsule.h"

class CCollisionStack;

class CSphereToCapsuleIntersection : public CCollisionIntersection
{
public:
	CSphereToCapsuleIntersection();
	CSphereToCapsuleIntersection(CSphereCollisionObject &i_sphere, Capsule &i_capsule);

	// Allocate/deallocate a new intersection object off/on the global stack.
	static CSphereToCapsuleIntersection* New(CSphereCollisionObject &i_sphere,
		Capsule &i_capsule, CCollisionStack* i_stack);

	// Assignment
	CSphereToCapsuleIntersection& operator=(CSphereToCapsuleIntersection &i_source);

	void Set(CSphereCollisionObject* i_sphere, Capsule* i_capsule);

	// Access
	CSphereCollisionObject &GetSphere(void) {return(*d_sphere);}
	Capsule &GetCapsule(void) {return(*d_capsule);}

	// Invalidates any results computed so far. Call this if one of the objects
	// change state.
	void ClearTests(void) {d_validFlags = 0;}

	bool IntersectionTest(void);

	Vector3CRef ClosestPoint(void);

	// Determine if a sphere and Capsule intersect and allocate an intersection
	// object if they do.
	static CCollisionIntersection* TestForIntersection(
		CSphereCollisionObject* i_sphere, Capsule* i_capsule,
		CCollisionStack* i_stack);

#ifdef DEBUG_STACK
	virtual const char* GetName(void);
#endif

private:
	// Control construction on the stack.
	CSphereToCapsuleIntersection(CSphereCollisionObject &i_sphere, Capsule &i_capsule,
		CCollisionStack &i_stack);

	CSphereCollisionObject* d_sphere;
	Capsule* d_capsule;

	uint d_validFlags;

	bool d_intersects;

	// Point closest to the center of the sphere
	Vector3 d_closestPoint;
	Vector3 d_closestPointOnCapsuleAxis;

	// Valid data masks.
	static const uint INTERSECTION_TEST;
	static const uint CLOSEST_POINT;
};

#endif