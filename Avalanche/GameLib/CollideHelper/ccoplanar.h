///////////////////////////////////////////////////////////////////////////////
// Fast coplanar check
//
// - Builds AABB
// - Collects terrain triangles
// - Tests each edge
///////////////////////////////////////////////////////////////////////////////
#ifndef CCOPLANAR_H
#define CCOPLANAR_H

#include "collide/aabb.h"

class CheckCoplanar
{
public:
	// Are these coplanar on the terrain in the world?
	static bool Check(const Vector3 corner[4], Vector3CRef normal, float start = -0.01f, float end = 0.02f, bool enforceMaterial = false, bool cheap = true);
#ifdef PS2
	static bool Check(const Vector3Packed corner[4], Vector3CRef normal, float start = -0.01f, float end = 0.02f, bool enforceMaterial = false, bool cheap = true);
#endif
};

#endif
