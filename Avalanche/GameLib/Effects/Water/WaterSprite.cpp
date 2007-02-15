
//
// Author: TJC
// Purpose: A sprite floating on the waters surface...
//

#include "Effects/EffectsPCH.h"

#include "Effects/Water/Water.h"
#include "Effects/Water/WaterPool.h"
#include "Effects/Water/WaterSprite.h"

WaterSprite::WaterSprite(
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
		int alphaBlendFunc
		)
	: mTexBind(texBind), 
	  mLifeRemaining(duration), 
	  mWater(NULL),
	  // scale envelope
	  mScaleAttackEndTime(duration - scaleAttackTime),
	  mScaleSustainEndTime(duration - (scaleSustainTime + scaleAttackTime)),
	  mScale(scaleInitialSize),
	  mScaleAttackRate((scaleSustainSize - scaleInitialSize) / scaleAttackTime),
	  mScaleReleaseRate((scaleFinalSize - scaleSustainSize) / (duration - (scaleAttackTime + scaleSustainTime))),
	  // alpha envelope
	  mAlphaAttackEndTime(duration - alphaAttackTime),
	  mAlphaSustainEndTime(duration - (alphaSustainTime + alphaAttackTime)),
	  mAlpha(alphaInitialOpacity),
	  mAlphaAttackRate((alphaSustainOpacity - alphaInitialOpacity) / alphaAttackTime),
	  mAlphaReleaseRate((alphaFinalOpacity - alphaSustainOpacity) / (duration - (alphaAttackTime + alphaSustainTime))),
	  // blend function... (RS_BLENDFUNC_*)
	  mAlphaBlendFunc(alphaBlendFunc)
	
{
#if defined(GCN)				
	mTexMatrix.Identity();	
	mPosition.Clear();
	mScaleOffset.Clear();
	mOrientation = 0.0;
	mScaleReleaseRate = 0.0;
	mAlphaAttackEndTime = 0.0;
	mAlphaSustainEndTime = 0.0;
	mAlphaAttackRate = 0.0;
	mAlphaReleaseRate = 0.0;
#endif
	mPosition = position;
	mScaleOffset.Set(0.5f, 0.5f, 0.5f);
	mOrientation = initialRotation + g_random.InRange(-initialRotationRandomness, initialRotationRandomness);
	mRotationSpeed = rotationSpeed + g_random.InRange(-rotationSpeedRandomness, rotationSpeedRandomness);

	mOrientation = Math::Deg2Rad(mOrientation);
	mRotationSpeed = Math::Deg2Rad(mRotationSpeed);
}

WaterSprite::~WaterSprite()
{
}

void WaterSprite::UpdatePosition(const Vector3& position)
{
	mPosition = position;
	
	UpdatePosition();
}

float	x = 0.0f;
float	y = 0.0f;
float	z = 0.0f;

void WaterSprite::UpdatePosition(void)
{
	if(mWater)
	{
		Graphics4x4 m;
		m.Identity();

		// into texture space
		m.CatXRotation(Math::HalfPi);

		#ifdef USE_ANIHF_WATER
		AnimatedHeightField *A = g_aniHFMgr.Find(mWater->mPoolNumber);
		Graphics4x4 m2 = A->instance->Matrix();
		Vector3 waterPos = m2.GetRow3();
		Vector3 localPos = (waterPos - mPosition);
		m2.Identity();
		m2.SetYRotation(-mWater->Angle());
		localPos = m2.Transform(localPos.x(), localPos.y(), localPos.z());

		m.CatTranslation(0.5f, 0.0f, 0.5f);
		m.CatScale(mWater->XScale(), 1.0f, mWater->ZScale());
		m.CatTranslation(localPos.x(), localPos.y(), localPos.z());

		#else		// Tyler's water on PS2

		// move into position
		Vector3 localPos = -mWater->WorldToLocal().Transform(mPosition.x(), mPosition.y(), mPosition.z());
		m.CatTranslation(localPos.x(), localPos.y(), localPos.z());
		// scale to fit water
		m.CatScale(mWater->XScale(), 1.0f, mWater->ZScale());
		#endif	// USE_ANIHF_WATER

		// apply scale
		float invScale = 1.0f / mScale;
		m.CatScale(invScale, invScale, invScale);

		// apply the orientation
		m.CatYRotation(mOrientation);

		// where does the scale/rotation happen from
		m.CatTranslation(mScaleOffset.x(), mScaleOffset.y(), mScaleOffset.z());

		// back out of texture space
		m.CatXRotation(-Math::HalfPi);

		mTexMatrix = m;
	}
}

void WaterSprite::AddedToWater(WaterPool* water)
{
	// remember our water...
	mWater = water;

	#ifndef USE_ANIHF_WATER

	mTexture = (*(*water)[0]->mTextureArray)[mTexBind];

	#else
	AnimatedHeightField * A = g_aniHFMgr.Find(water->mPoolNumber);		// PAB use.. anihf textures instead
	if (A != NULL)

		mTexture = (*A->textureArray)[mTexBind];
	#endif


	// build matrix...
	UpdatePosition();
}

void WaterSprite::Update(float deltaTime)
{
	bool updatePosition = false;

	// rotate the sprite...
	if(!Math::Zero(mRotationSpeed))
	{
		mOrientation += mRotationSpeed * deltaTime;
		updatePosition = true;
	}

	// flow the sprite...
	mPosition.Set(mPosition.x() + deltaTime*mWater->mSpriteFlowX, mPosition.y(), mPosition.z() + deltaTime*mWater->mSpriteFlowZ); 
	
	if(mLifeRemaining > mScaleAttackEndTime)
	{
		mScale += mScaleAttackRate * deltaTime;
		updatePosition = true;
	}
	else if(mLifeRemaining < mScaleSustainEndTime)
	{
		mScale += mScaleReleaseRate * deltaTime;
		updatePosition = true;
	}

	if(mLifeRemaining > mAlphaAttackEndTime)
	{
		mAlpha += mAlphaAttackRate * deltaTime;
	}
	else if(mLifeRemaining < mAlphaSustainEndTime)
	{
		mAlpha += mAlphaReleaseRate * deltaTime;
	}

	// rebuild color
	u32 alpha = (u32)(mAlpha * 0xFF);
	alpha = Math::Max(alpha, (u32)0x00000000);
	alpha = Math::Min(alpha, (u32)0x000000FF);
	mColor = 0x00FFFFFF | (alpha << 24);

	if(updatePosition)
		UpdatePosition();
}
