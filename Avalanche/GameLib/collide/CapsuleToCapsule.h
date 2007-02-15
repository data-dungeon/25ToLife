#ifndef CAPSULE_TO_CAPSULE_H
#define CAPSULE_TO_CAPSULE_H

// Gamelib includes //
#include "collide/Intersection.h"
#include "collide/Capsule.h"

class CCollisionStack;

class CCapsuleToCapsuleIntersection : public CCollisionIntersection
{
public:
	CCapsuleToCapsuleIntersection();
	CCapsuleToCapsuleIntersection(Capsule &i_capsuleA, Capsule &i_capsuleB);

	// Allocate/deallocate a new intersection object off/on the global stack.
	static CCapsuleToCapsuleIntersection* New(Capsule &i_capsuleA, Capsule &i_capsuleB,
		CCollisionStack* i_stack);

	// Assignment
	CCapsuleToCapsuleIntersection& operator=(CCapsuleToCapsuleIntersection &i_source);

	// Assign new capsules.
	void SetCapsules(Capsule* i_capsuleA, Capsule* i_capsuleB);

	Capsule &GetCapsuleA(void) {return(*d_capsuleA);}
	Capsule &GetCapsuleB(void) {return(*d_capsuleB);}

	// A number of tests can be performed. As they are performed, useful
	// results are noted and stored.

	// Invalidates any results computed so far. Call this if one of the capsules
	// changes state.
	void ClearTests(void) {d_validFlags = 0;}

	bool IntersectionTest(void);

	Vector3CRef ClosestPointOnA( void );
	Vector3CRef ClosestPointOnB( void );

	// Determine if two capsules intersect and allocate an intersection object
	// if they do.
	static CCollisionIntersection* TestForIntersection(Capsule* i_capsuleA, Capsule* i_capsuleB,
		CCollisionStack* i_stack);

	// Draw points.
	void DrawPoints(DrawUtility::COLOR i_color = DrawUtility::BLUE);

#ifdef DEBUG_STACK
	virtual const char* GetName(void);
#endif

private:
	// Control construction on the stack.
	CCapsuleToCapsuleIntersection(Capsule &i_capsuleA, Capsule &i_capsuleB, CCollisionStack &i_stack);

	Capsule* d_capsuleA;
	Capsule* d_capsuleB;

	uint d_validFlags;

	bool d_intersects;

	Vector3 d_closestPointOnA;
	Vector3 d_closestPointOnB;

	// Valid data masks.
	static const uint INTERSECTION_TEST;
};

#endif