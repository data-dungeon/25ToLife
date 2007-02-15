
//
// Author: TJC
// Purpose: Represent an equation modifying the height of the surface of the water
//


#include "Effects/EffectsPCH.h"


#include "Effects/Water/Water.h"
#include "Effects/Water/WaterEquation.h"
#include "Effects/Water/WaterPool.h"
#include "Effects/siggen.h"

bool WaterEquation::Decay(float deltaTime, float flowX, float flowZ)
{
	if(mTimeRemaining != 0.0f)
	{
		mTimeRemaining -= deltaTime; 
		if(mTimeRemaining <= 0.0f) 
			return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////
// WaterEqRing
///////////////////////////////////////////////////////////////////////////////////////

WaterEqRing::WaterEqRing(
	float centerX,
	float centerZ,
	float Attack,
	float Sustain, 
	float Release,
	float GrowRate, 
	float WaveLength,
	float WaveStartPosition,
	float WaveHeight,
	int WaveCount)
	: 	WaterEquation(Attack + Sustain + Release, 1),
		mCenterX(centerX),
		mCenterZ(centerZ),
		mAttack(Attack),
		mSustain(Sustain),
		mRelease(Release),
		mGrowRate(GrowRate),
		mWaveLength(WaveLength),
		mWaveStartPosition(WaveStartPosition * WaveLength + (WaveCount - 1.0f)),
		mWaveFrequency(Math::Inverse(WaveLength) * Math::TwoPi),
		mWaveHeight(WaveHeight),
		mWaveCount((float)WaveCount * Math::TwoPi),
		mInvWaveCount(Math::Inverse(mWaveCount))
{
}

WaterEqRing::~WaterEqRing()
{
}

float WaterEqRing::Evaluate(float t, float x, float z)
{
// 	x *= mWater->GetXScale();
// 	z *= mWater->GetZScale();
	
	float age = Age();
	float deltax = x - mCenterX;
	float deltaz = z - mCenterZ;
	float distance = Math::Sqrt(deltax * deltax + deltaz * deltaz);

	float angle = (distance - mGrowRate*age + mWaveStartPosition) * mWaveFrequency;
	if(angle < 0.0f || angle > mWaveCount)
		return 0.0f;

	// get ramp from 0 - 1 - 0...
	float ramppos = 1 - Math::Abs(((angle * mInvWaveCount)*2.0f) - 1.0f);
	//float ramppos = 1.0f;

	float result = Math::Cos(angle) * ramppos * mWaveHeight;

	// deal with attack/sustain/release
	if(age < mAttack)
	{
		// wave growing...
		return result * (age / mAttack);
	}
	else
	{
		age -= mAttack;
		if(age < mSustain)
		{
			return result;
		}
		else
		{
			// wave shrinking...
			age -= mSustain;
			age -= mRelease;
			return result * (age / -mRelease);
		}
	}
}

void WaterEqRing::AddedToWater(WaterPool* water)
{
	// move center into water space...
// 	Vector3 v;
// 	v.Set(mCenterX, 0.0f, mCenterZ);
// 	v = v * water->GetWorldToLocal();
// 	mCenterX = v.x * water->GetXScale();
// 	mCenterZ = v.z * water->GetZScale();
	
// 	mWater = water;
}

bool WaterEqRing::Decay(float deltaTime, float flowX, float flowZ)
{
	mCenterX += flowX;
	mCenterZ += flowZ;

	return WaterEquation::Decay(deltaTime, flowX, flowZ);
}

