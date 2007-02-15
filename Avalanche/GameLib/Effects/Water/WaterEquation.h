//
// This interface is primarily for prototyping different water
// movements.  Final movements should be recoded into the water pool
// for speed.  Also be aware that the PS2 will require a microcode
// version of any surface movements that you expect to have reflected
// graphically (not coding the ps2 version will result in your code
// modifying the physics, but not the graphics).
//
// Here are a bunch of the ways to affect the water surface that have
// been prototyped through this interface, but moved into WaterPool:
//
// WaterPool::ChangeSurfaceHeight(...) - Global height of the entire
// pool.
// 
// WaterPool::AddWakePoint(...) - create a particle like lump in the
// water.  Currently used by WaterPool internally to create movement
// wakes.
//
// WaterPool::AddSegmentBump(...)
// WaterPool::AddSemiCircleBump(...) - create static bumps in the
// water for representing changes in the water surface where it flows
// around an obstruction.  These dont have PS2 versions yet, and are
// not finalized.
//
//


#ifndef __WATER_EQUATION_H__
#define __WATER_EQUATION_H__

// headers
#include "collide/aabb.h"

// forward declares
class SigGen;
class WaterPool;

// the results of the equations added to the water are summed across
// the plane.  An equation may be evaluated outside its AABB, (but
// usually near the edges of the AABB) so clamping within the equation
// may be nessisary...
class WaterEquation
{
public:
	// if duration == 0.0f, this equation is considered permenant.
	WaterEquation(float duration, int type) : mTimeRemaining(duration), mLifeTime(duration), mType(type) { mDampen = true; }
	virtual ~WaterEquation() {};

	// Decay this equation by deltaTime.
	virtual bool Decay(float deltaTime, float flowX, float flowZ);

	// evaluate the equation at the given position.  This returns the Y
	// value for the equation
	virtual float Evaluate(float t, float x, float z) = 0L;

	// this gets called by the water when this equation is added into a water effect so that the
	// equation can modify itself (move locations into water space, for example)
	virtual void AddedToWater(WaterPool* water) {};

	// equation can be dampened or not, which affects where it is applied in Water::GetHeightLocal
	bool Dampen()						{ return(mDampen); }
	void SetDampen(bool Dampen)	{ mDampen = Dampen; }

	float Age(void) {	return mLifeTime - mTimeRemaining; }

	// world space region where this equation currently has influence
	// and should be evaluated.
	AABB mRegionOfInfluence;

	// for PS2 to direct access elements to pass them up to the vu...
	friend class Water;
protected:

	// this is a unique # represnting what kind of equation this is.  Currently, only 0 & 1 are globally defined.
	int mType;

	// time to live remaining.  if 0.0f, this equation is considered
	// permenant.
	float mTimeRemaining;
	// initial requested life time...
	float mLifeTime;

	// dampened or not
	bool mDampen;
};

// type 1
class WaterEqRing : public WaterEquation
{
public:
	// if duration == 0.0f, this equation is considered permenant.
	WaterEqRing(
		float centerX,
		float centerZ,
		float Attack,
		float Sustain, 
		float Release,
		float GrowRate, 
		float WaveLength,
		float WaveStartPosition,
		float WaveHeight,
		int WaveCount);
	virtual ~WaterEqRing();

	// evaluate the equation at the given position.  This returns the Y
	// value for the equation
	virtual float Evaluate(float t, float x, float z);

	// need one of these
	virtual void AddedToWater(WaterPool* water);

	// Decay this equation by deltaTime.
	virtual bool Decay(float deltaTime, float flowX, float flowZ);

	// for PS2 to direct access elements to pass them up to the vu...
	friend class Water;
protected:
	float mCenterX;
	float mCenterZ;
	float mAttack;
	float mSustain;
	float mRelease;
	float mGrowRate;
	float mWaveLength;
	float mWaveStartPosition;
	float mWaveFrequency;
	float mWaveHeight;
	float mWaveCount;
	float mInvWaveCount;

	Water* mWater;
};

#endif // __WATER_EQUATION_H__
