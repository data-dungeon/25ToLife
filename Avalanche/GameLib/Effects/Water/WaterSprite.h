#ifndef __WATERSPRITE_H__
#define __WATERSPRITE_H__

//
// Author: TJC
// Purpose: A sprite floating on the waters surface...
//

class WaterSprite
{
public:
	WaterSprite(
		u16 texBind, 
		const Vector3& position,
		float duration, 
		// rotation information
		float initialRotation,
		float initialRotationRandomness,
		float rotationSpeed,
		float rotationSpeedRandomness,
		// scale envelope
		float scaleAttackTime,
		float scaleSustainTime,
		float scaleInitialSize,
		float scaleSustainSize,
		float scaleFinalSize,
		// alpha envelope
		float alphaAttackTime,
		float alphaSustainTime,
		float alphaInitialOpacity,
		float alphaSustainOpacity,
		float alphaFinalOpacity,
		// blend function... (RS_BLENDFUNC_*)
		int alphaBlendFunc);

	~WaterSprite();

	void UpdatePosition(const Vector3& position);
	void SetScale(float scale)							{ mScale = scale; UpdatePosition(); }
	void SetScaleOffset(Vector3& offset)			{ mScaleOffset = offset; UpdatePosition(); }

	Graphics4x4 mTexMatrix; // PAB moved

protected:
	friend class WaterPool;
	friend class Water;

	void AddedToWater(WaterPool* water);
	void Update(float deltaTime);
	void UpdatePosition(void);
	int GetAlphaBlendFunc(void)						{ return mAlphaBlendFunc; }
	u32 Color(void)										{ return mColor; }

	u32 mColor;
	Texture* mTexture;
	u16 mTexBind;
	float mLifeRemaining;
	WaterPool* mWater;
	Vector3 mPosition;
	Vector3 mScaleOffset;
	float mOrientation;

	// rotation information
	float mRotationSpeed;
	// scale envelope
	float mScaleAttackEndTime;
	float mScaleSustainEndTime;
	float mScale;
	float mScaleAttackRate;
	float mScaleReleaseRate;
	// alpha envelope
	float mAlphaAttackEndTime;
	float mAlphaSustainEndTime;
	float mAlpha;
	float mAlphaAttackRate;
	float mAlphaReleaseRate;
	// blend function... (RS_BLENDFUNC_*)
	int mAlphaBlendFunc;
};

#endif // __WATERSPRITE_H__

