/////////////////////////////////////////////////////////////////////////////
// Class for helping debug weapons
/////////////////////////////////////////////////////////////////////////////
#ifndef DEBUGSHOT_H
#define DEBUGSHOT_H

#include "environ/munition.h"

// Define this to debug shots
#if 0//defined(WIN32) && defined(_DEBUG) && defined(FOOD)
#define DEBUG_SHOTS
#endif

#ifdef DEBUG_SHOTS

#define TRACKSHOTS		10
#define TRACKHITS			10
#define TRACKIMPACTS		10

class DebugShot
{
public:
	static void Draw();
	static void Line(const Vector3 &a, const Vector3 &b);
	static void Hit(const Vector3 &v);
	static void Impact(const CMunitionImpactSurface &impact);
	static void RegisterHit(const Vector3 &pos, const Vector3 &fromDir);

private:
	static Vector3 s_shot[TRACKSHOTS][2];
	static int		s_shots;
	static Vector3 s_hit[TRACKHITS];
	static int		s_hits;
	static Vector3 s_registerHit[TRACKHITS][2];
	static int		s_registerHits;
	static CMunitionImpactSurface s_impact[TRACKIMPACTS];
	static int		s_impacts;
};

#else

// The null class
class DebugShot
{
public:
	static void Draw() {}
	static void Line(const Vector3 &a, const Vector3 &b) {}
	static void Hit(const Vector3 &v) {}
	static void Impact(const CMunitionImpactSurface &impact) {}
	static void RegisterHit(const Vector3 &pos, const Vector3 &fromDir) {}
};

#endif

#endif
