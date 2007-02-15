#ifndef LINE_TO_CAPSULE_H
#define LINE_TO_CAPSULE_H

// Engine includes //
#include "Math/Vector.h"

// Gamelib includes //
#include "collide/Intersection.h"
#include "collide/Line.h"
#include "collide/Capsule.h"

class CCollisionStack;

class CLineToCapsuleIntersection : public CCollisionIntersection
{
public:
	CLineToCapsuleIntersection();
	CLineToCapsuleIntersection(CLineCollisionObject &i_line,
		Capsule &i_capsule);
	~CLineToCapsuleIntersection();

	// Allocate/deallocate a new intersection object off/on the global stack.
	static CLineToCapsuleIntersection* New(CLineCollisionObject &i_line,
		Capsule &i_capsule, CCollisionStack* i_stack);

	// Assignment
	CLineToCapsuleIntersection& operator=(CLineToCapsuleIntersection &i_source);

	// Assign new Capsules.
	void Set(CLineCollisionObject* i_line, Capsule* i_capsule);

	CLineCollisionObject& GetLine(void) {return(*d_line);}
	Capsule& GetCapsule(void) {return(*d_capsule);}

	// A number of tests can be performed. As they are performed, useful
	// results are noted and stored.

	// Invalidates any results computed so far. Call this if one of the Capsules
	// changes state.
	void ClearTests(void);

	// Initial intersection test
	bool IntersectionTest(void);

	// Entry data
	bool Enters(void);
	float EntryFrac(void);
	const Vector3 &EntryPoint(void);
	const Vector3 &EntryNormal(void);

	// Exit data
	bool Exits(void);
	float ExitFrac(void);
	const Vector3 &ExitPoint(void);
	const Vector3 &ExitNormal(void);

	// Found data
	const Vector3 &FoundPoint(void) {return(d_foundPoint);}
	const Vector3 &FoundNormal(void) {return(d_foundNormal);}

	// Determine if a line and a Capsule intersect and allocate an intersection
	// object if they do.
	static CCollisionIntersection* TestForIntersection(
		CLineCollisionObject* i_line, Capsule* i_capsule,
		CCollisionStack* i_stack);

#ifdef DEBUG_STACK
	virtual const char* GetName(void);
#endif

private:
	// Control construction on the stack.
	CLineToCapsuleIntersection(CLineCollisionObject &i_line,
		Capsule &i_capsule, CCollisionStack &i_stack);

	bool TestCapsuleSphere( Vector3CRef center, float radiusSquared, Vector3CRef lineOrigin,
		Vector3CRef lineDirection, float &entry, float &exit );
	void PerformIntersectionTest(void);

	CLineCollisionObject* d_line;
	Capsule* d_capsule;

	uint d_validFlags;

	// Basic intersection info
	bool d_intersects;
	bool d_enters;
	bool d_exits;
	float d_entryFrac;
	float d_exitFrac;

	// Entry point in world coords
	Vector3 d_entryPoint;

	// Entry normal in world coords
	Vector3 d_entryNormal;

	// Exit point in world coords
	Vector3 d_exitPoint;

	// Exit normal in world coords
	Vector3 d_exitNormal;

	// Find point
	bool d_modifiedLine;
	CCollisionIntersection* d_previousModifier;
	float d_previousPoint;
	Vector3 d_foundPoint;
	Vector3 d_foundNormal;

	// Valid data masks.
	static const uint INTERSECTION_TEST;
	static const uint ENTRY_POINT;
	static const uint ENTRY_NORMAL;
	static const uint EXIT_POINT;
	static const uint EXIT_NORMAL;
};

#endif // LINE_TO_SPHERE_H
