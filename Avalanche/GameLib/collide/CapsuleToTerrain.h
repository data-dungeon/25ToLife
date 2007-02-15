#ifndef __CAPSULE_TO_TERRAIN_H__INCLUDED__
#define __CAPSULE_TO_TERRAIN_H__INCLUDED__

// Engine include //
#include "Math/Vector.h"

// Gamelib includes //
#include "collide/Intersection.h"
#include "collide/Capsule.h"
#include "collide/Terrain.h"
#include "collide/TriangleList.h"

class TerrainTriangle;
class CCollisionStack;
template<class T> class CCollisionStackArray;

class CCapsuleToTerrainIntersection : public CCollisionIntersection
{
public:
	CCapsuleToTerrainIntersection(Capsule &i_capsule, CTerrainCollisionObject &i_terrain);

	// Allocate/deallocate a new intersection object off/on the global stack.
	static CCapsuleToTerrainIntersection* New(Capsule &i_capsule,
		CTerrainCollisionObject &i_terrain, CCollisionStack* i_stack);

	Capsule &GetCapsule(void) {return(d_capsule);}
	CTerrainCollisionObject &Terrain(void) {return(d_terrain);}

	void ClearTests(void) {d_validFlags = 0;}

	// See if the Capsule touches the terrain at all.
	bool IntersectionTest(void);

	// Determine if an Capsule and terrain intersect and allocate an intersection
	// object if they do.
	static CCollisionIntersection* TestForIntersection(Capsule* i_capsule,
		CTerrainCollisionObject* i_terrain, CCollisionStack* i_stack);

	Vector3CRef GetResolutionInfo( float &o_depth ) { o_depth = resolutionDistance; return resolutionNormal; }

#ifdef DEBUG_STACK
	virtual const char* GetName(void);
	virtual void AppendDescription(char* io_msg);
#endif

private:
	// Prevent construction without an Capsule and terrain.
	CCapsuleToTerrainIntersection();

	// Control construction on the stack.
	CCapsuleToTerrainIntersection(Capsule &i_capsule, CTerrainCollisionObject &i_terrain,
		CCollisionStack &i_stack);

	// Participants.
	Capsule &d_capsule;
	CTerrainCollisionObject &d_terrain;

	CTerrainAABBIterator d_iterator;

	// Valid data flags.
	uint d_validFlags;

	// Intersection test.
	bool d_intersects;

	Vector3 resolutionNormal;
	float resolutionDistance;

	// Valid data masks.
	static const uint INTERSECTION_TEST;
};

#endif //__CAPSULE_TO_TERRAIN_H_INCLUDED__
