#ifndef CAPSULE_TO_OBB_H
#define CAPSULE_TO_OBB_H

// Gamelib includes //
#include "collide/Intersection.h"
#include "collide/Capsule.h"
#include "collide/obb.h"

class CCollisionStack;

class CCapsuleToObbIntersection : public CCollisionIntersection
{
public:
	CCapsuleToObbIntersection();
	CCapsuleToObbIntersection(Capsule &i_capsule, OBB &i_obb);

	// Allocate/deallocate a new intersection object off/on the global stack.
	static CCapsuleToObbIntersection* New(Capsule &i_capsule, OBB &i_obb,
		CCollisionStack* i_stack);

	// Assignment
	CCapsuleToObbIntersection& operator=(CCapsuleToObbIntersection &i_source);

	// Assign new capsules.
	void Set(Capsule* i_capsule, OBB* i_obb);

	Capsule &GetCapsule(void) {return(*d_capsule);}
	OBB &GetOBB(void) {return(*d_obb);}

	// A number of tests can be performed. As they are performed, useful
	// results are noted and stored.

	// Invalidates any results computed so far. Call this if one of the capsules
	// changes state.
	void ClearTests(void) {d_validFlags = 0;}

	bool RadiusTest( void );
	bool SeparatingAxisTest( void );
	bool ClosestFeatureTest( void );
	bool DirectionalFeatureTest( Vector3CRef dir, Vector3 &o_normal, float &o_depth );
	
	bool GetResolutionNormal( Vector3 &o_normal, float &o_depth );
	bool GetFirstPointOfContact( Vector3CRef movementDirection, Vector3 &o_point, Vector3 &o_normal, float &o_depth );
	Vector3 GetClosestPointOnOBB( void ) { return d_closestPointOnObb; }

	// Determine if a capsule and obb intersect and allocate an intersection object
	// if they do.
	static CCollisionIntersection* TestForIntersection(Capsule* i_capsule, OBB* i_obb,
		CCollisionStack* i_stack);

#ifdef DEBUG_STACK
	virtual const char* GetName(void);
#endif

private:
	// Control construction on the stack.
	CCapsuleToObbIntersection(Capsule &i_capsule, OBB &i_obb, CCollisionStack &i_stack);

	Capsule* d_capsule;
	OBB* d_obb;

	uint d_validFlags;

	bool d_radiusIntersection;
	bool d_intersects;

	Vector3 d_closestPointOnCapsule;
	Vector3 d_closestPointOnObb;

	Vector3 d_resolutionNormal;
	float d_resolutionDepth;

	Vector3 d_firstPoint;
	Vector3 d_firstNormal;
	float d_firstDepth;
	bool d_foundFirstPoint;

	int d_closestAxis;
	float d_closestAxisDepth;
	Vector3 d_closestAxisNormal;

	enum ESeparatingAxis
	{
		A0XB,
		A1XB,
		A2XB,
		P1,
		P2,
		P3,
		P4,
		P5,
		P6,
		P7,
		P8,
		S1,
		S2,
		NUM_SEPARATING_AXIS
	};

	float d_axisOverlap[NUM_SEPARATING_AXIS];

	// Valid data masks.
	static const uint RADIUS_TEST;
	static const uint SEPARATING_AXIS_TEST;
	static const uint CLOSEST_FEATURE_TEST;
	static const uint CLOSEST_AXIS_TEST;
	static const uint FIRST_POINT_OF_CONTACT;
};

#endif