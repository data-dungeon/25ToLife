/*****************************************************************************/
// Simple class to do a bunch of stuff with random numbers
// Right now uses rand() as a base, but eventually it will use something
// faster with better distribution.
/*****************************************************************************/
#include "Math/MathPCH.h"

#include "Math/mtwister.h"
#include "layers/debug.h"
#include "math/fastrand.h"

// Our handy litle generator!
FastRandom g_random;

/*****************************************************************************/
/*****************************************************************************/
FastRandom::FastRandom()
{
	ASSERT(Test());
}

/*****************************************************************************/
/*****************************************************************************/
FastRandom::FastRandom(
int seed)
{
	Seed(seed);
}

/*****************************************************************************/
// Return a point in a unit box/cube
/*****************************************************************************/
Vector2 FastRandom::InUnitBox()
{
	Vector2 v;

	v.x(NegPos());
	v.y(NegPos());

	return v;
}

/*****************************************************************************/
// Return a point in a unit box/cube
/*****************************************************************************/
Vector3 FastRandom::InUnitCube()
{
	Vector3 v;

	v.x(NegPos());
	v.y(NegPos());
	v.z(NegPos());

	return v;
}

/////////////////////////////////////////////////////////////////////////////
// Return a point on a circle/sphere
/////////////////////////////////////////////////////////////////////////////
Vector2 FastRandom::OnCircle(float radius)
{
	const float phi = UpTo(Math::TwoPi);

	float sinphi, cosphi;
	Math::SinCos(phi, sinphi, cosphi);

	return Vector2(radius * cosphi, radius * sinphi);
}

/////////////////////////////////////////////////////////////////////////////
// Return a point on a circle/sphere
/////////////////////////////////////////////////////////////////////////////
Vector3 FastRandom::OnSphere(float radius)
{
	const float phi = UpTo(Math::TwoPi);
	const float theta = UpTo(Math::Pi);

	float sinphi, cosphi;
	float sintheta, costheta;
	Math::SinCos(phi, sinphi, cosphi);
	Math::SinCos(theta, sintheta, costheta);

	return Vector3(radius * sintheta * cosphi, radius * sintheta * sinphi, radius * costheta);
}

/////////////////////////////////////////////////////////////////////////////
// Randomize this dir
/////////////////////////////////////////////////////////////////////////////
Vector3 FastRandom::InCone(
Vector3CRef dir,
float halfAngleRad)
{
#if 1
	// Make a random vector
	const Vector3 perp = MakeOrthogonal(dir);

	const float randomAngle = g_random.UpTo(halfAngleRad);
	Vector3 newDir = RotatePointAroundAxis(dir, perp, randomAngle);

	const float rotVariance = g_random.InRange(-Math::Pi, Math::Pi);
	newDir = RotatePointAroundAxis(newDir, dir, rotVariance);

	return newDir;
#else
	Vector3 v(InRange(-halfAngleRad, halfAngleRad), InRange(-halfAngleRad, halfAngleRad), InRange(-halfAngleRad, halfAngleRad));
	DirCos3x3 o;
	o.SetXZYRotation(v);
	return dir * o;
#endif
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Vector3 FastRandom::InCone(
Vector3CRef dir,
float halfAngleInnerRad,
float halfAngleOuterRad)
{
#if 1
	// Make a random vector
	const Vector3 perp = MakeOrthogonal(dir);

	const float randomAngle = g_random.InRange(halfAngleInnerRad, halfAngleOuterRad);
	Vector3 newDir = RotatePointAroundAxis(dir, perp, randomAngle);

	const float rotVariance = g_random.InRange(-Math::Pi, Math::Pi);
	newDir = RotatePointAroundAxis(newDir, dir, rotVariance);

	return newDir;
#else
	Graphics4x4 mTransform;
	Graphics4x4 mTransform2;
	float fRadians;
	Vector3 vB;
	Vector3 vC;
	static Vector3 vUnitVector100(1.0f, 0.0f, 0.0f);

	/* calculate randomizing matrix-- this will transform unit vector (1, 0, 0) into a cone centered on */
	/* the positive x-axis */

	fRadians = UpTo(Math::TwoPi);	// 0.0 to 2PI
	mTransform.SetXRotation(fRadians);
	fRadians = halfAngleInnerRad;
	fRadians += UpTo(halfAngleOuterRad - halfAngleInnerRad);
	mTransform.ReverseCatYRotation(fRadians);

	/* we have gimbal lock issues with input vector = (1, 0, 0) (vB ends up (0, 0, 0)) */
	if (Math::Equal(dir.x(), 1.0f))
		mTransform2.Identity();   // need to transform from (1, 0, 0) to (1, 0, 0)-- identity
	else if (Math::Equal(dir.x(), -1.0f))
		mTransform2.SetYRotation(Math::Pi);      // transform from (1, 0, 0) to (-1, 0, 0), 180 degree y-rotation should do it
	else
	{
		/* calculate transform matrix that will transform unit vector X (1, 0, 0) to the passed */
		/* unit vector. we have the input vector vVelUnitVector, think of it as vA. then calculate */
		/* vB = X - (vA * (vA dot X)), and vC = vA x vB. then the transform matrix is: */
		/*
		/*                      vA.x()    vA.y(), vA.z()
		/*                      vB.x()    vB.y(), vB.z()
		/*                      vC.x()    vC.y(), vC.z()
		/*
		/* vA dot X = (vA.x(), vA.y(), vA.z()) dot (1, 0, 0) = vA.x() */

		vB = Vector3(1.0f, 0.0f, 0.0f) - dir * dir.x();
		vB.SafeNormalize();
		vC = Vector3::Cross(dir, vB);

		/* fill in transform matrix */
		mTransform2.Identity();
		mTransform2.r00 = dir.x();
		mTransform2.r01 = dir.y();
		mTransform2.r02 = dir.z();
		mTransform2.r10 = vB.x();
		mTransform2.r11 = vB.y();
		mTransform2.r12 = vB.z();
		mTransform2.r20 = vC.x();
		mTransform2.r21 = vC.y();
		mTransform2.r22 = vC.z();
	}

	/* and now put the unit vector (1, 0, 0) through both matrices-- the first */
	/* one randomizes it into a cone around the positive x-axis, the second transforms */
	/* it to match the passed velocity direction */

	Vector3 newdir = vUnitVector100 * mTransform;
	newdir = newdir * mTransform2;
	return newdir;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Debugging test
/////////////////////////////////////////////////////////////////////////////
bool FastRandom::Test()
{
	bool ret = true;

#ifdef RANDOM_DEBUG
	// Make sure 2^24 is the highest safe float (should be
	const unsigned int start = (1 << 23);
	unsigned int testI = start;
	float testF = (float)start;
	while ((unsigned int)testF == testI)
	{
		++testI;
		testF += 1.0f;
	}
	TRACE("FastRandom--> Highest safe float %f %ld\n", testF, testI);

	// coin toss
	int heads = 0;
	int tails = 0;
	for (int coin = 0; coin < 1000; coin++)
	{
		if (CoinToss())
			++heads;
		else
			++tails;
	}
	TRACE("FastRandom--> heads vs tails %d %d\n", heads, tails);

	// duh
	for (int intRand = 0; intRand < 10; intRand++)
	{
		TRACE("Int rand: %d\n", Int());
	}
	for (int fRand = 0; fRand < 10; fRand++)
	{
		TRACE("Float rand: %f\n", Unit());
	}

	// Range tests
	int rangeTest;
	int bad = 0;
	int dist[16];
	dist[0] = dist[1] = dist[2] = dist[3] = dist[4] = 0;
	for (rangeTest = 0; rangeTest < 10000; rangeTest++)
	{
		int rand = UpTo(4);
		ASSERT(rand >= 0 && rand <= 4);
		if (rand >= 0 && rand <= 4)
			++dist[rand];
		else
		{
			ret = false;
			++bad;
		}
	}
	TRACE("Distribution UpTo(4): 0-%d 1-%d 2=%d 3-%d 4-%d (out of range: %d)\n", dist[0], dist[1], dist[2], dist[3], dist[4], bad);

	bad = dist[0] = dist[1] = dist[2] = dist[3] = 0;
	for (rangeTest = 0; rangeTest < 10000; rangeTest++)
	{
		int rand = UpToExcludeHigh(4);
		ASSERT(rand >= 0 && rand < 4);
		if (rand >= 0 && rand < 4)
			++dist[rand];
		else
		{
			ret = false;
			++bad;
		}
	}
	TRACE("Distribution UpToExcludeHigh(4): 0-%d 1-%d 2=%d 3-%d (out of range: %d)\n", dist[0], dist[1], dist[2], dist[3], bad);

	bad = dist[0] = dist[1] = dist[2] = dist[3] = dist[4] = dist[5] = dist[6] = dist[7] = dist[8] = 0;
	for (rangeTest = 0; rangeTest < 10000; rangeTest++)
	{
		int rand  = InRange(-4, 4);
		ASSERT(rand >= -4 && rand <= 4);
		if (rand >= -4 && rand <= 4)
			++dist[rand + 4];
		else
		{
			ret = false;
			++bad;
		}
	}
	TRACE("Distribution InRange(-4, 4): -4-%d -3-%d -2-%d -1-%d 0-%d 1-%d 2=%d 3-%d 4-%d (out of range: %d)\n",
				dist[0], dist[1], dist[2], dist[3], dist[4], dist[5], dist[6], dist[7], dist[8], bad);

#endif
	return ret;
}

