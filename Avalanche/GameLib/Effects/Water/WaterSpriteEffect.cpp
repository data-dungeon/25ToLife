
//
// Author: TJC
// Purpose: A collection of watersprites
//

#include "Effects/EffectsPCH.h"

#include "Effects/Water/WaterMgr.h"
#include "Effects/Water/Water.h"
#include "Effects/Water/WaterPool.h"
#include "Effects/Water/WaterSprite.h"
#include "Effects/Water/WaterSpriteEffect.h"
#include "GameHelper/ParamMgr.h"

WaterSpriteEffect::WaterSpriteEffect(char *context, char * path)
{
	mParams = CParam<SpriteParams>::GetParam(context, "var");
};

WaterSpriteEffect::~WaterSpriteEffect()
{
}

void WaterSpriteEffect::Trigger(u32 poolIdx, const Vector3& position, const Vector3& velocity)
{
	int i;
	WaterPool* pool = g_waterMgr[poolIdx];
	ASSERT(pool);

	for(i = 0; i < MAX_ENTRIES; i++)
	{
		// skip empty ones...
		if(mParams->param[i].mCount == 0) 
			continue;

		for(int sprite = 0; sprite < mParams->param[i].mCount; sprite++)
		{
			// randomize the position a little
			float x, z;
			x = g_random.Unit();
			z = g_random.Unit();

			x -= 0.5f;
			z -= 0.5f;

			x *= mParams->param[i].mRandRadius*2.0f;
			z *= mParams->param[i].mRandRadius*2.0f;

			if(rand() & 0x1000)
				x = -x;
			if(rand() & 0x1000)
				z = -z;

			float length = velocity.Length();

			Vector3 newvel = velocity * mParams->param[i].mVelAdj;

			Vector3 hitpoint = position;
			hitpoint.x( hitpoint.x() + x + newvel.x());
			hitpoint.z( hitpoint.z() + z + newvel.z());

			WaterSprite* temp = new WaterSprite(
				mParams->param[i].mTexIndex,			// texture index
				hitpoint,		// where/orient
				
				mParams->param[i].mDuration,
				
				mParams->param[i].mInitialRotation,
				mParams->param[i].mInitialRotationRandomness,
				mParams->param[i].mRotationSpeed,
				mParams->param[i].mRotationSpeedRandomness,
	
				mParams->param[i].mScaleAttackTime * mParams->param[i].mDuration,
				mParams->param[i].mScaleSustainTime * mParams->param[i].mDuration,
				
				mParams->param[i].mInitialScale,
				mParams->param[i].mSustainScale,
				mParams->param[i].mFinalScale,
				
				mParams->param[i].mAlphaAttackTime * mParams->param[i].mDuration,
				mParams->param[i].mAlphaSustainTime * mParams->param[i].mDuration,
				
				mParams->param[i].mInitialAlpha,
				mParams->param[i].mSustainAlpha,
				mParams->param[i].mFinalAlpha,

				mParams->param[i].mAlphaBlendFunc
				);
			pool->AddWaterSprite(temp);
		}
	}
}

char* buildString(const char* string, int index)
{
	static char buffer[100];
	sprintf(buffer, "Sprite%d_%s", index, string);
	return buffer;
}

void CParam<WaterSpriteEffect::SpriteParams>::RegisterVariables(void)
{
	for(int i = 0; i < MAX_ENTRIES; i++)
	{
		RegisterVariable(d_data.param[i].mTexIndex, buildString("TextureIndex", i), 0, 1, 10);
		RegisterVariable(d_data.param[i].mCount, buildString("Count", i), 0, 1, 10);
		RegisterVariable(d_data.param[i].mRandRadius, buildString("RandRadius", i), 0.0f, 0.0f, 3.0f);
		RegisterVariable(d_data.param[i].mDuration, buildString("Duration", i), 1.0f, 0.0f, 2.0f);
		RegisterVariable(d_data.param[i].mScaleAttackTime, buildString("ScaleAttackTime", i), 0.0f, 0.0f, 1.0f);
		RegisterVariable(d_data.param[i].mScaleSustainTime, buildString("ScaleSustainTime", i), 1.0f, 0.0f, 1.0f);
		RegisterVariable(d_data.param[i].mInitialScale, buildString("InitialScale", i), 0.0f, 0.0f, 0.0f);
		RegisterVariable(d_data.param[i].mSustainScale, buildString("SustainScale", i), 0.0f, 0.0f, 1.0f);
		RegisterVariable(d_data.param[i].mFinalScale, buildString("FinalScale", i), 0.0f, 0.0f, 0.0f);
		RegisterVariable(d_data.param[i].mAlphaAttackTime, buildString("AlphaAttackTime", i), 0.0f, 0.0f, 1.0f);
		RegisterVariable(d_data.param[i].mAlphaSustainTime, buildString("AlphaSustainTime", i), 1.0f, 0.0f, 1.0f);
		RegisterVariable(d_data.param[i].mInitialAlpha, buildString("InitialAlpha", i), 1.0f, 0.0f, 1.0f);
		RegisterVariable(d_data.param[i].mSustainAlpha, buildString("SustainAlpha", i), 1.0f, 0.0f, 1.0f);
		RegisterVariable(d_data.param[i].mFinalAlpha, buildString("FinalAlpha", i), 1.0f, 0.0f, 1.0f);
		RegisterVariable(d_data.param[i].mVelAdj, buildString("VelAdj", i), 0.0f, -1.0f, 1.0f);
		RegisterVariable(d_data.param[i].mAlphaBlendFunc, buildString("AlphaBlendFunc", i), 1, 10, 1);
		RegisterVariable(d_data.param[i].mInitialRotation, buildString("InitialRotation", i), 0.0f, -180.0f, 180.0f);
		RegisterVariable(d_data.param[i].mInitialRotationRandomness, buildString("InitialRotationRandomness", i), 180.0f, 0.0f, 180.0f);
		RegisterVariable(d_data.param[i].mRotationSpeed,	buildString("RotationSpeed", i), 0.0f, -720.0f, 720.0f);
		RegisterVariable(d_data.param[i].mRotationSpeedRandomness,	buildString("RotationSpeedRandomness", i), 0.0f, 0.0f, 720.0f);
	}
}
