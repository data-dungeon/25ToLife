#ifndef __WATER_SPRITE_EFFECT_H__
#define __WATER_SPRITE_EFFECT_H__

class WaterSpriteEffect
{
public:
	WaterSpriteEffect(char* context, char* directory);
	~WaterSpriteEffect();
	void Trigger(u32 poolIdx, const Vector3& position, const Vector3& velocity);

protected:
#define MAX_ENTRIES 3
	
	struct SpriteParam {
		int mTexIndex;
		int mCount;
		float mRandRadius;
		float mDuration;
		float mScaleAttackTime;
		float mScaleSustainTime;
		float mInitialScale;
		float mSustainScale;
		float mFinalScale;
		float mAlphaAttackTime;
		float mAlphaSustainTime;
		float mInitialAlpha;
		float mSustainAlpha;
		float mFinalAlpha;
		float mVelAdj;
		int mAlphaBlendFunc;
		// rotation information
		float mInitialRotation;
		float mInitialRotationRandomness;
		float mRotationSpeed;
		float mRotationSpeedRandomness;
	};
	struct SpriteParams {
		SpriteParam param[MAX_ENTRIES];
	};
	SpriteParams *mParams;
};


#endif // __WATER_SPRITE_EFFECT_H__
