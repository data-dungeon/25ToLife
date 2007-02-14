/////////////////////////////////////////////////////////////////////////////
// Simple class to see if you can see through stuff
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "stage/SurfaceType.h"
#include "weapons/seethru.h"

// How many things will we look through?
#define MAX_LOOK_THRU_DEPTH	 3

//#define SEETHRU_DEBUG

SeeThru::MaterialInfo SeeThru::s_materialInfo[] =
{
	{ "glass", true },
	{ "vegitation", false },
	{ "chainlink",	true },
	{ "nothing", true },
	{ "climbable", true },
	{ "vaultable", true },
	{ "metalgate", true },
	{ "water", true },
	{ "ladder", true },
	{ "fence", true },
	{ "vaulttrigger", true },
	{ "forcefield", true },
	{ NULL, false }
};

SeeThru::SurfaceType SeeThru::s_surfaceType[MAX_SEETHRU_MATERIALS] = { 0 };
int SeeThru::s_surfaceTypes = 0;

/////////////////////////////////////////////////////////////////////////////
// Call this every time the table gets loaded
/////////////////////////////////////////////////////////////////////////////
void SeeThru::SetupMaterials()
{
	int onMaterial = 0;
	s_surfaceTypes = 0;
	while (s_materialInfo[onMaterial].materialName)
	{
		int surfaceType = g_surfaceType.GetIndex(s_materialInfo[onMaterial].materialName);
		ASSERT(surfaceType >= 0);
		ASSERT(s_surfaceTypes < MAX_SEETHRU_MATERIALS);

		if (surfaceType >= 0 && s_surfaceTypes < MAX_SEETHRU_MATERIALS)
		{
			s_surfaceType[s_surfaceTypes].material = (u8)surfaceType;
			s_surfaceType[s_surfaceTypes].targetThru = s_materialInfo[onMaterial].targetThru;
			++s_surfaceTypes;
		}

#ifdef SEETHRU_DEBUG
		TRACE("Material \"%s\" (%d) is transparent.\n", s_materialInfo[onMaterial], surfaceType);
#endif
		++onMaterial;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Can I see through this material
/////////////////////////////////////////////////////////////////////////////
bool SeeThru::Transparent(u8 material)
{
#ifdef SEETHRU_DEBUG
	char *test = g_surfaceType.GetName(material);
#endif

	for (int s = 0; s < s_surfaceTypes; s++)
	{
		if (s_surfaceType[s].material == material)
		{
#ifdef SEETHRU_DEBUG
			char *against = g_surfaceType.GetName(s_surfaceType[s].material);
			TRACE("Transparent matched %d (%s) against %d (%s)\n", material, test, s_surfaceType[s].material, against);
#endif
			return true;
		}
	}

	// Can't see thru
#ifdef SEETHRU_DEBUG
	TRACE("Transpatent NO MATCH %d (%s)\n", material, test);
#endif
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// Can I target through this transparent material
/////////////////////////////////////////////////////////////////////////////
bool SeeThru::TargetThru(u8 material)
{
	ASSERT(Transparent(material));

	for (int s = 0; s < s_surfaceTypes; s++)
	{
		if (s_surfaceType[s].material == material)
			return s_surfaceType[s].targetThru;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Look down a line, what can you see?
// Returns false if no intersection, true if intersection
/////////////////////////////////////////////////////////////////////////////
bool SeeThru::Look(
const Vector3 &eye,
const Vector3 &dir,
float range,
CActor *ignore,
LookResult &result,
bool bothSides)
{
	// compute what we are using for the aim
	Vector3 startScan = eye;
	float totalD = 0.0f;
	int depth = 0;
	while (depth <= MAX_LOOK_THRU_DEPTH)
	{
		// Setup the line
		CLineEntity line;
		line.Line().SetOriginBasis(startScan, dir);
		line.Line().SetMaxBoundary(range);
		if (bothSides)
			line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_BOTH_SIDES);
		else
			line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
		line.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
		line.SetIgnoreEntity(ignore);

		// Now cast it
		g_collisionSystem.ResetStack();
		CLineQuery test;
		if (!test.Test(line, CStageEntity::USE_COMPLEX))
			return false;
		else
		{
			// We have a hit, is it transparent?
			if (Transparent(test.material))
			{
				const float tweak = 0.10f;
				startScan = test.intersection + dir * tweak;
				range -= test.d;
				totalD += test.d;
			}
			else
			{
				// Save the results
				result.intersection = test.intersection;
				result.normal = test.normal;
				result.d = totalD + test.d;
				result.actor = test.actor;
				result.material = test.material;
				return true;
			}
		}

		++depth;
	}

	// No hit
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// Look down a line, what can you see?
// Returns false if no intersection, true if intersection
/////////////////////////////////////////////////////////////////////////////
bool SeeThru::Look(
const Vector3 &start,
const Vector3 &end,
CActor *ignore,
LookResult &result,
bool bothSides)
{
	// compute what we are using for the aim
	Vector3 startScan = start;
	Vector3 dir = end - start;
	dir.SafeNormalize();
	float totalD = 0.0f;
	int depth = 0;
	while (depth <= MAX_LOOK_THRU_DEPTH)
	{
		// Setup the line
		CLineEntity line;
		line.Line().SetOriginEnd(start, end);
		if (bothSides)
			line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_BOTH_SIDES);
		else
			line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
		line.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
		line.SetIgnoreEntity(ignore);

		// Now cast it
		g_collisionSystem.ResetStack();
		CLineQuery test;
		if (!test.Test(line, CStageEntity::USE_COMPLEX))
			return false;
		else
		{
			// We have a hit, is it transparent?
			if (Transparent(test.material))
			{
				const float tweak = 0.10f;
				startScan = test.intersection + dir * tweak;
				totalD += test.d;
			}
			else
			{
				// Save the results
				result.intersection = test.intersection;
				result.normal = test.normal;
				result.d = totalD + test.d;
				result.actor = test.actor;
				result.material = test.material;
				return true;
			}
		}

		++depth;
	}

	// No hit
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// The aimsim penetration callback
/////////////////////////////////////////////////////////////////////////////
bool SeeThru::AimSimCallback(
void *context,
u8 material,
int depth)
{
	// Make sure we aren't out of control
	if (depth >= MAX_LOOK_THRU_DEPTH)
		return false;

	// Invoke SeeThru
	return Transparent(material);
}

