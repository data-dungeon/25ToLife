///////////////////////////////////////////////////////////////////////////////
// Our specific subclass for sound collision tests
///////////////////////////////////////////////////////////////////////////////
#include "EngineHelper/EngineHelperPCH.h"
#include "collide/collsys.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SoundCollisionTest::SoundCollisionTest()
{
	iterator = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SoundCollisionTest::~SoundCollisionTest()
{
}

///////////////////////////////////////////////////////////////////////////////
// Begin an occlusion test, return false if there are no occlusions
///////////////////////////////////////////////////////////////////////////////
bool SoundCollisionTest::Begin(
Vector3CRef source,
Vector3CRef listener)
{
	// Setup the line
	CLineCollisionObject line;
	line.SetOriginEnd(source, listener);
	line.SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	line.SetFindMode(CLineCollisionObject::FIND_ALL);

	// Wave the majik wand
	g_collisionSystem.ResetStack();

	// Now create the intersection object
	iterator = (CLineToTerrainIntersection*)CLineToTerrainIntersection::New(line,
						*(CTerrainCollisionObject*)g_collisionSystem.TerrainEntity()->GetCollisionObject(),
						&g_collisionSystem.GetStack());
	if (!iterator)
		return false;

	// Draw it
#if defined(_DEBUG) && defined(FOOD) && defined(WIN32) && !defined(_XBOX)
	DrawUtility::Point(source);
	iterator->DrawTerrain();
#endif

	// Reset the enumeration
	index = 0;

	// Do the intersections (will return the number)
	return iterator->IntersectionTest();
}

///////////////////////////////////////////////////////////////////////////////
// Get the next occlusion,  returns false if there are no more
// Your subclass fills out the intersection point, the material and the
// t along the line segment.
///////////////////////////////////////////////////////////////////////////////
bool SoundCollisionTest::GetNext(
Vector3 &intersection,
u8 &material)
{
	// Did they really even start it
	if (!iterator)
		return false;

	// Get the intersectoin
	CLineToTerrainPoint *point = iterator->GetIntersection(index++);
	if (!point)
		return false;

	// Got a point!
	intersection = point->Intersection();
	material = point->Material();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Called to match every begin...so long as it returns true
///////////////////////////////////////////////////////////////////////////////
void SoundCollisionTest::End()
{
	// Do we free this off the global stack?
	iterator = NULL;
}
