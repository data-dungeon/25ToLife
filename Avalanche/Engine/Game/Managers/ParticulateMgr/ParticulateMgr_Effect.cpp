#include "Game/GamePCH.h"

namespace ParticulateMgr
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////
	//// Effect:: routines
	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////

	Effect::Effect(const Effect& baseEffect) :
		mInitialPositionOffset(baseEffect.mInitialPositionOffset),
		mInitialVelocity(baseEffect.mInitialVelocity),
		mInitialAngularPositionRange(baseEffect.mInitialAngularPositionRange),
		mInitialAngularVelocityRange(baseEffect.mInitialAngularVelocityRange),
		mInitialScaleRange(baseEffect.mInitialScaleRange),
		mLifetimeRange(baseEffect.mLifetimeRange),
		mEmitterLifetimeRange(baseEffect.mEmitterLifetimeRange),
		mColorEnvelope(baseEffect.mColorEnvelope),
		mUseTexture(baseEffect.mUseTexture),
		mDragCoefficient(baseEffect.mDragCoefficient),
		mAccelerationVector(baseEffect.mAccelerationVector),
		mSpawnRate(baseEffect.mSpawnRate),
		mScaleRate(baseEffect.mScaleRate),
		mAngluarAcceleration(baseEffect.mAngluarAcceleration),

		mLastABSTime(0.0f),
		mSpawnsThisCycle(0),
		mSpawnTimeRemaining(0.0f),
		mEmitterCount(0)
	{
		sEffects.AddHead(this);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	Effect::Effect(const Cone& initialPositionOffset,
						const Cone& initialVelocity,
						const FloatRange& initialAngularPositionRange,
						const FloatRange& initialAngularVelocityRange,
						const FloatRange& initialScaleRange,
						const FloatRange& lifetimeRange,
						const FloatRange& emitterLifetimeRange, // negative values are frame counts
						const ColorEnvelope& colorEnvelope,
						Texture* useTexture,
						float dragCoefficient,
						Vector3CRef accelerationVector,
						float spawnRate,
						const FloatSpline& scaleEnvelope,
						float angluarAcceleration) :
		mInitialPositionOffset(initialPositionOffset),
		mInitialVelocity(initialVelocity),
		mInitialAngularPositionRange(initialAngularPositionRange),
		mInitialAngularVelocityRange(initialAngularVelocityRange),
		mInitialScaleRange(initialScaleRange),
		mLifetimeRange(lifetimeRange),
		mEmitterLifetimeRange(emitterLifetimeRange),
		mColorEnvelope(colorEnvelope),
		mUseTexture(useTexture),
		mDragCoefficient(dragCoefficient),
		mAccelerationVector(accelerationVector),
		mSpawnRate(spawnRate),
		mScaleEnvelope(scaleEnvelope),
		mAngluarAcceleration(angluarAcceleration),

		mLastABSTime(0.0f),
		mSpawnsThisCycle(0),
		mSpawnTimeRemaining(0.0f),
		mEmitterCount(0)
	{
		sEffects.AddHead(this);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	Effect::~Effect()
	{
		// You MUST first delete all emitters that use this effect
		ASSERT(mEmitterCount == 0);

		sEffects.Remove(this);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	void Effect::Update(float absTime)
	{
		if(mLastABSTime == 0.0f)
			mLastABSTime = absTime;
		float deltaTime = absTime - mLastABSTime;

		if(mEmitterLifetimeRange.mMin < 0.0f)
		{
			mSpawnsThisCycle = (s32)mSpawnRate;
			return;
		}
		else
		{
			// max delta time out at the max lifetime of a particulate
			// this is to keep us from going haywire...  just a sanity thing
			deltaTime = Math::Min(deltaTime, mLifetimeRange.mMax);
			// figure out how many spawned in that much time.
			mSpawnsThisCycle = (s32)((deltaTime + mSpawnTimeRemaining) / mSpawnRate);
			// figure out how much time went unsued that needs to be accounted for next cycle.
			mSpawnTimeRemaining = (deltaTime + mSpawnTimeRemaining) - mSpawnRate * mSpawnsThisCycle;
		}

		mLastABSTime = absTime;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	void Effect::InitializeParticulate(Particulate* particulate, float absTime, Vector3CRef position)
	{
		particulate->mBirthTime = g_random.InRange(mLastABSTime, absTime);
		particulate->mDeathTime = particulate->mBirthTime + mLifetimeRange.Random();
		particulate->mInitialPosition = position + mInitialPositionOffset.Random();
		particulate->mInitialAngularVelocity = mInitialAngularVelocityRange.Random();
		particulate->mInitialAngularPosition = mInitialAngularPositionRange.Random();
		particulate->mInitialScale = mInitialScaleRange.Random();
		particulate->mInitialVelocity = mInitialVelocity.Random();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	Effect* Effect::Clone()
	{
		return new Effect(*this);
	}

};

