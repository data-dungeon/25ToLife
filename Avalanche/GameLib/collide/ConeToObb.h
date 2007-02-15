#ifndef CONE_TO_OBB_H
#define CONE_TO_OBB_H

// Gamelib includes //
#include "collide/Intersection.h"
#include "collide/obb.h"
#include "collide/Cone.h"

class CCollisionStack;

class CConeToObbIntersection : public CCollisionIntersection
{
public:
	CConeToObbIntersection();
	CConeToObbIntersection(ConeCollisionObject &i_cone, OBB &i_obb);

	// Allocate/deallocate a new intersection object off/on the global stack.
	static CConeToObbIntersection* New(ConeCollisionObject &i_cone, OBB &i_obb,
		CCollisionStack* i_stack);

	// Assignment
	CConeToObbIntersection& operator=(CConeToObbIntersection &i_source);

	// Assign new objects.
	void Set(ConeCollisionObject* i_cone, OBB* i_obb);

	ConeCollisionObject &GetCone(void) {return(*d_cone);}
	OBB &GetObb(void) {return(*d_obb);}

	// A number of tests can be performed. As they are performed, useful
	// results are noted and stored.

	// Invalidates any results computed so far. Call this if one of the objects
	// changes state.
	void ClearTests(void) {d_validFlags = 0;}

	bool IntersectionTest(void);

	// Determine if the objects intersect and allocate an intersection object
	// if they do.
	static CCollisionIntersection* TestForIntersection(ConeCollisionObject* i_cone, OBB* i_obb,
		CCollisionStack* i_stack);

	// Draw points.
	void DrawPoints(DrawUtility::COLOR i_color = DrawUtility::BLUE);

#ifdef DEBUG_STACK
	virtual const char* GetName(void);
#endif

private:
	// Control construction on the stack.
	CConeToObbIntersection(ConeCollisionObject &i_cone, OBB &i_obb, CCollisionStack &i_stack);

	ConeCollisionObject* d_cone;
	OBB* d_obb;

	uint d_validFlags;

	bool d_intersects;

	// Valid data masks.
	static const uint INTERSECTION_TEST;
};

#endif