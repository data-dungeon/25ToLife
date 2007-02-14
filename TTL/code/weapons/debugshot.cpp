/////////////////////////////////////////////////////////////////////////////
// Class for helping debug weapons
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"
#include "EngineHelper/drawutil.h"
#include "weapons/debugshot.h"

#ifdef DEBUG_SHOTS

Vector3 DebugShot::s_shot[TRACKSHOTS][2];
int DebugShot::s_shots = 0;
Vector3 DebugShot::s_hit[TRACKHITS];
int DebugShot::s_hits = 0;
Vector3 DebugShot::s_registerHit[TRACKHITS][2];
int DebugShot::s_registerHits = 0;
CMunitionImpactSurface DebugShot::s_impact[TRACKIMPACTS];
int DebugShot::s_impacts = 0;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void DebugShot::Draw()
{
	int i;
	for (i = 0; i < TRACKSHOTS; i++)
		DrawUtility::Line(s_shot[i][0], s_shot[i][1], DrawUtility::WHITE);
	for (i = 0; i < TRACKHITS; i++)
		DrawUtility::Point(s_hit[i], DrawUtility::WHITE);
	for (i = 0; i < TRACKHITS; i++)
		DrawUtility::Normal(s_registerHit[i][0], s_registerHit[i][1],DrawUtility::MAGENTA);

	for (i = 0; i < TRACKIMPACTS; i++)
	{
		DrawUtility::Sphere(s_impact[i].d_shotOrigin, DrawUtility::CYAN, 0.15f);
		DrawUtility::Sphere(s_impact[i].d_intersection, DrawUtility::YELLOW, 0.15f);
		DrawUtility::Normal(s_impact[i].d_intersection, s_impact[i].d_surfaceNormal, DrawUtility::YELLOW);
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void DebugShot::Line(
const Vector3 &a,
const Vector3 &b)
{
	s_shot[s_shots][0] = a;
	s_shot[s_shots++][1] = b;
	if (s_shots == TRACKSHOTS)
		s_shots = 0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void DebugShot::Hit(
const Vector3 &v)
{
	s_hit[s_hits++] = v;
	if (s_hits == TRACKHITS)
		s_hits = 0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void DebugShot::RegisterHit(
const Vector3 &pos, 
const Vector3 &fromDir)
{
	s_registerHit[s_registerHits][0] = pos;
	s_registerHit[s_registerHits][1] = fromDir;
	if (++s_registerHits == TRACKHITS)
		s_registerHits = 0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void DebugShot::Impact(
const CMunitionImpactSurface &impact)
{
	s_impact[s_impacts++] = impact;
	if (s_impacts == TRACKIMPACTS)
		s_impacts = 0;
}

#endif
