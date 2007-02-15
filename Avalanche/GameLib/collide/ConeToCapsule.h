#ifndef CONE_TO_CAPSULE_H
#define CONE_TO_CAPSULE_H

// Gamelib includes //
#include "collide/Intersection.h"
#include "collide/Capsule.h"
#include "collide/Cone.h"

class CCollisionStack;

class CConeToCapsuleIntersection : public CCollisionIntersection
{
public:
	CConeToCapsuleIntersection();
	CConeToCapsuleIntersection(ConeCollisionObject &i_cone, Capsule &i_capsule);

	// Allocate/deallocate a new intersection object off/on the global stack.
	static CConeToCapsuleIntersection* New(ConeCollisionObject &i_cone, Capsule &i_capsule,
		CCollisionStack* i_stack);

	// Assignment
	CConeToCapsuleIntersection& operator=(CConeToCapsuleIntersection &i_source);

	// Assign new objects
	void Set(ConeCollisionObject* i_cone, Capsule* i_capsule);

	ConeCollisionObject &GetCone(void) {return(*d_cone);}
	Capsule &GetCapsule(void) {return(*d_capsule);}

	// A number of tests can be performed. As they are performed, useful
	// results are noted and stored.

	// Invalidates any results computed so far. Call this if one of the objects
	// changes state.
	void ClearTests(void) {d_validFlags = 0;}

	bool IntersectionTest(void);

	// Determine if the objects intersect and allocate an intersection object
	// if they do.
	static CCollisionIntersection* TestForIntersection(ConeCollisionObject* i_cone, Capsule* i_capsule,
		CCollisionStack* i_stack);

	// Draw points.
	void DrawPoints(DrawUtility::COLOR i_color = DrawUtility::BLUE);

#ifdef DEBUG_STACK
	virtual const char* GetName(void);
#endif

private:
	// Control construction on the stack.
	CConeToCapsuleIntersection(ConeCollisionObject &i_cone, Capsule &i_capsule, CCollisionStack &i_stack);

	ConeCollisionObject* d_cone;
	Capsule* d_capsule;

	uint d_validFlags;

	bool d_intersects;

	// Valid data masks.
	static const uint INTERSECTION_TEST;
};

#endif