/////////////////////////////////////////////////////////////////////////////
// Simple class to see if you can see through stuff
/////////////////////////////////////////////////////////////////////////////
#ifndef SEETHRU_H
#define SEETHRU_H

#define MAX_SEETHRU_MATERIALS		16

class SeeThru
{
public:
	// Call this every time the table gets loaded
	static void SetupMaterials();

	// Can I see through this material?
	static bool Transparent(u8 material);

	// Can this material be targeted thru?
	static bool TargetThru(u8 material);

	// Look (fill out this structure)
	struct LookResult
	{
		Vector3		intersection;
		Vector3		normal;
		float			d;
		CActor		*actor;
		u8				material;
	};
	// Look down a line, what can you see?
	// Returns false if no intersection, true if intersection
	static bool Look(const Vector3 &eye, const Vector3 &dir, float range, CActor *ignore, LookResult &result, bool bothSides = false);

	// Test a chord, what can you see?
	// Returns false if no intersection, true if intersection
	static bool Look(const Vector3 &start, const Vector3 &end, CActor *ignore, LookResult &result, bool bothSides = false);

	// A handy direct callback for the AimSim
	static bool AimSimCallback(void *context, u8 material, int depth);

private:
	// The see thru list (this is way cheaper than looking up a sighting material)
	struct MaterialInfo
	{
		const char *materialName;
		bool			targetThru;
	};
	static MaterialInfo s_materialInfo[];
	struct SurfaceType
	{
		u8			material;
		bool		targetThru;
	};
	static SurfaceType s_surfaceType[MAX_SEETHRU_MATERIALS];
	static int s_surfaceTypes;
};

#endif
