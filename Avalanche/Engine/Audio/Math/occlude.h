///////////////////////////////////////////////////////////////////////////////
// Sound occlusion, BABY!
///////////////////////////////////////////////////////////////////////////////
#ifndef OCCLUDE_H
#define OCCLUDE_H

// Subclass this to provide me with an occlusion test
class SoundOcclusionTest
{
public:
	SoundOcclusionTest();
	virtual ~SoundOcclusionTest();

	// setup the material properties
	struct MaterialProperty
	{
		float	volumeFactor;	// Volume multipler
		float	panFactor;		// pan multipler
	};
	void ResetMaterialProperties();
	void SetMaterialProperty(u8 material, const MaterialProperty &prop);

	// For a given listner and source, return the volume
	// and pan multipliers (you don't have to initialize them)
	// Returns true if volumeFactor is non-zero
	bool Test(Vector3CRef source, Vector3CRef listener, float &volumeFactor, float &panFactor);

protected:
	// You write these for your engine 

	// Begin an occlusion test, return false if there are no occlusions
	virtual bool Begin(Vector3CRef source, Vector3CRef listener) = 0;

	// Get the next occlusion,  returns false if there are no more
	// Your subclass fills out the intersection point, the material and the
	// t along the line segment.
	virtual bool GetNext(Vector3 &intersection, u8 &material) = 0;

	// Called to match every begin...so long as it returns true
	virtual void End() {}

private:
	// The materials table
	MaterialProperty		materialProp[256];
};

#endif
