
#ifndef __WATERAFFECTOR_H__
#define __WATERAFFECTOR_H__

class Water;
class OBB;

// Inherit from this to define something that can affect the water
// physics/lod calculations
// For example, if you want High LOD to be based on the location of
// the primary actor, derive from this an object that reports info
// about the actor and register it.
class WaterAffector
{
public:
	//////////////////////////////////////////////////////////////
	// some simple construction / destruction rules
	//////////////////////////////////////////////////////////////
	WaterAffector(void) : 
		mWaterVerticalTessReduction(false)
	{
	}
	virtual ~WaterAffector(void) {};

	//////////////////////////////////////////////////////////////
	// these variables are checked by Water to determine
	// information about the Affector
	// these are variables because they may be checked frequently
	// so avoiding the overhead of a virtual call is important.
	//////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////
	// NOTE! the order that the affectors are added to the water
	// will control the priority of the subdivision.  Since dynamic
	// objects tend to be more important, and added last, the list
	// is walked in reverse order.  So, the most recently added
	// affector gets its subdivision performed first, then so on
	// up the list.
	//////////////////////////////////////////////////////////////

	// the area around us that should be tesselated is bounded by 2
	// radii.  Under the mMinRadius, the subdivider will try to make ==
	// mMaxSubDivisionDepth, while between min & max, the subdivider
	// will try to feather the number of subdivisions performed.
	// Setting both to the same value will turn off the feathering.
	float mWaterMinRadius;
	float mWaterMaxRadius;

	// this is how deep the subdivision is allowed to go that is
	// caused by this affector.  Set to zero to keep this affector
	// from causing tesselation
	u8 mWaterMaxSubDivisionDepth;

	// this bool determines if vertical distance is a factor in
	// determining subdivision levels with this affector.  In other
	// words, if this bool is true, then as the affector moves further
	// above/below the surface of the water, the tesselation will be
	// reduced.  Contrarywise, if the bool is false, the y component
	// of the position of the affector is ignored when determining 
	// tesselation levels.  Long variable name eh?  It was much longer.
	bool mWaterVerticalTessReduction;

	// return the position of the affector.  It should be in world space.
	// also used in physics perturb below...
	virtual Vector3CRef WorldPosition(void) = 0L;

	// return the bounding box around the affector (this is used for mState above
	// and for physics calculations.  But it is NOT used during tesselation, 
	// and is therefore unrelated to the 2 radii above.)
	virtual OBB* WorldOBB(void) = 0L;

	//////////////////////////////////////////////////////////////
	// these are used to perturb the water...
	//////////////////////////////////////////////////////////////

	// get velocity in world of the affector
	virtual Vector3CRef WorldVelocity(void) = 0L;

	// this is how much force the motion of this affector causes on the world
	// set to 0 if you dont want this affector to affect physics
	float mWaterForceScale;
	// this is how fast the affector can move in the world
	float mWaterMaxSpeed;


	//////////////////////////////////////////////////////////////
	// these are used internally by the Water system.
	// while deterimining tile allocation for LOD
	//////////////////////////////////////////////////////////////
private:
};

#endif // __WATERAFFECTOR_H__
