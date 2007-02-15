/*****************************************************************************/
// Simple class to do a bunch of stuff with random numbers
// Right now uses rand() as a base, but eventually it will use something
// faster with better distribution.
/*****************************************************************************/
#ifndef FASTRAND_H
#define FASTRAND_H

#include "platform/BaseType.h"
#include "math/mtwister.h"

class Vector2;

class FastRandom
{
public:
	// We can be instanced!
	FastRandom();
	FastRandom(int seed);

	// What is the maximum random number that can be generated?
	const int MaxInt()
		{ return 0x7fffffff; }

	// Seed the generator
	inline void Seed(int seed)
		{ m_twister.Seed(seed); }

	// Return a 0 or 1 with good distribution
	inline bool CoinToss()
		{ return ((m_twister.RandInt() & 0x1) == 0); }

	// Returns a number between [0.. maxInt] inclusive
	inline int Int()
		{ return m_twister.RandSignedInt(); }

	// Returns a number between [0.0..1.0] inclusive
	inline float Unit()
		{ return m_twister.Rand(); }

	// Returns a number between [0.0..1.0) excluding 1.0
	inline float UnitExcludeHigh()
		{ return m_twister.RandExcludeHigh(); }

	// Access to nonuniform random number distributions
	inline float Nonuniform(float mean = 0.5f, float variance = 0.5f)
		{ return m_twister.RandNorm(mean, variance); }

	// Returns a number between [-1.0..1.0] inclusive
	inline float NegPos()
		{ return 1.0f - m_twister.Rand() * 2.0f; }

	// Returns a random -1 or 1
	inline int IntSign()
		{ return CoinToss() ? 1 : -1; }
	inline float Sign()
		{ return CoinToss() ? 1.0f : -1.0f; }

	// return a random number between [0..high] inclusive
	inline int UpTo(int high)
		{ return high <= 0 ? 0 : (Int() % (high + 1)); }
	inline float UpTo(float high)
		{ return high <= 0.0f ? 0.0f : Unit() * high; }

	// return a random number between [0..high) not including high
	inline int UpToExcludeHigh(int high)
		{ return high <= 0 ? 0 : (Int() % high); }
	inline float UpToExcludeHigh(float high)
		{ return high <= 0.0f ? 0.0f : (m_twister.RandExcludeHigh(high)); }

	// Return a random number between [low..0] inclusive
	// Low should be negative, or you always will get 0
	inline int DownTo(int low)
		{ return -UpTo(-low); }
	inline float DownTo(float low)
		{ return low >= 0.0f ? 0.0f : Unit() * low; }

	// Return a random number between [low..0) excluding low
	inline int DownToExcludeLow(int low)
		{ return -UpToExcludeHigh(-low); }
	inline float DownToExcludeLow(float low)
		{ return -UpToExcludeHigh(-low); }

	// Return a random number in range [low..high] inclusive
	inline int InRange(int low, int high)
		{ return low + UpTo(high - low); }
	inline float InRange(float low, float high)
		{ return low + UpTo(high - low); }

	// Exclusion versions (you could write these until the cows came home)
	inline int InRangeExcludeHigh(int low, int high)
		{ return low + UpToExcludeHigh(high - low); }
	inline float InRangeExcludeHigh(float low, float high)
		{ return low + UpToExcludeHigh(high - low); }

	// Vector utilities

	// Return a point in a unit box/cube
	// (spanning coord's xyz[-1 1]),
	// meaning it's not unit it area/volume
	Vector2 InUnitBox();
	Vector3 InUnitCube();

	// Return a point on a circle/sphere
	Vector2 OnCircle(float radius);
	Vector3 OnSphere(float radius);

	// Return a point in a unit circle/sphere
	Vector2 InUnitCircle()
		{ return OnCircle(UpTo(1.0f)); }

	// Return a random unit vector (always unit in length)
	Vector3 UnitVector()
		{ return OnSphere(1.0f); }

	// Randomize this dir with the cone defined by 2 * rad
	Vector3 InCone(Vector3CRef dir, float halfAngleRad);

	// Randomize this dir with the cone defined by 2 * outerrad
	// but keep it out of the cone defined by 2*innerrad
	Vector3 InCone(Vector3CRef dir, float halfAngleInnerRad, float halfAngleOuterRad);

	// Debugging test
	bool Test();

protected:
	// Our random number generator
	MTwister 	m_twister;
};

extern FastRandom g_random;

#endif
