//
// Author: Tyler Colbert
// Purpose: This is a part of the Water class.  Please see the Water.h header for details.  This
//    file has the code to actually render the water.
//

#include "Effects/EffectsPCH.h"


#include "Display/PS2/Renderer/Code/PS2Renderer.h"
#include "Display/PS2/Renderer/Code/MasterList.h"
#include "Display/PS2/Renderer/Common/VIFInline.h"
#include "Display/PS2/Renderer/Common/GIFTag.h"
#include "Display/PS2/Renderer/Common/PRIMReg.h"
#include "Effects/Water/WaterAffector.h"

#include "Effects/Water/Water.h"
#include "Effects/Water/WaterPool.h"
#include "Effects/Water/WaterEquation.h"
#include "Effects/Water/WaterSprite.h"

#include "Game/Managers/TextureMgr/PlatformImage.h"

using namespace VIFInline;

extern float WaterFuzzFactor;

struct VULayer
{
	union {
		float scaleU;
		u8 layerType;
	};
	
	union {
		float scaleV;
		u8 numLayersInLayer0;
	};
	union {
		float flowU;
		u8 ALPHA1ABCD;
	};
	union {
		float flowV;
		u8 ALPHA1FIX;
	};

	u64 MIPTBP1;
	u64 MIPTBP2;

	u64 TEX0;
	u64 TEX1;

	u64 CLAMP;

	Vector3Packed vtxAdjParameter;
	Vector3Packed normAdjParameter;
	u32 layerFlags; // only bottom 16 are valid
	Vector3Packed light;
};


struct VUAffector {
	float x;
	union {
		float minRadius;
		u8 maxSubDivDepth;
	};
	float z;
	union {
		float maxRadius;
		u8 affectorCountInAffector0;
	};
};

struct VUWaterRing
{
	float centerX;
	float startTime;
	float centerZ;

	union {
		float unused;
		u8 ringCountIn0;
	};
};

struct VURings
{
	VUWaterRing rings[MAX_RINGS];
};

struct VUMatrix
{
	float m[4][4];
	void operator=(const Matrix4x4& rhs) { memcpy(this, &rhs, sizeof(Graphics4x4)); }
};

struct VUWaterSprite
{
	union {
		VUMatrix textureMatrix;
		struct {
			union {
				float m00;
				u8 R;
			};
			union {
				float m01;
				u8 G;
			};
			union {
				float m02;
				u8 B;
			};
			union {
				float m03;
				u8 A;
			};
			union {
				float m10;
			};
			union {
				float m11;
			};
			union {
				float m12;
				u8 SpriteCountInSprite0;
			};
		};
	};

	u64 MIPTBP1;
	u64 MIPTBP2;

	u64 TEX0;
	u64 TEX1;

	u64 CLAMP;
	u64 ALPHA1;
};

struct VUBump
{
	u128 qw;
};

struct VUAffectors {
	VUAffector affector[MAX_AFFECTORS];
};

struct VUWakes
{
	struct {
		union {
			float x;
			u8 scale;
		};
		union {
			float z;
			u8 countInWake0;
		};
	} wake[MAX_WAKES];
};

struct VUWaterSprites
{
	VUWaterSprite sprite[MAX_SPRITES];
};

struct VULayers
{
	VULayer layer[MAX_LAYERS];
};

struct VUMiscData
{
	// qw 1
	float scaleX;
	float noiseYScale;
	union {
		float scaleZ;
		u8 minSubDivDepth;
	};
	union {
		float noiseTScale;
		u8 maxSubDivDepth;
	};

	// qw 2
	union {
		u128 fantag;
		struct {
			float pad[3];
			float mT;
		};
	};

	// qw 3
	union {
		u128 striptag;
		struct {
			float pad2[3];
			float eyePointY;
		};
	};

	// qw 4
	union {
		u128 misctag;
		struct {
			float pad3[3];
			float unused1;
		};
	};

	// qw 5
	union {
		float eyePointX;
		u8 flags0;
	};
	union {
		float noiseXScale;
		u8 flags1;
	};
	float eyePointZ;

	float noiseZScale;

	// qw 6-9
	VUMatrix localToClip;
	// qw 10-13
	VUMatrix clipToScreen;
	// qw 14-17
	VUMatrix worldToLocal;
	// qw 18-21
	VUMatrix screenToClip;
	// qw 22-25
	VUMatrix clipToWorld;

	// qw 26
	float offsetX;
	float surfaceHeight;
	float offsetZ;
	union {
		float wakeRadiusSquared;
		u8 numWakePairs;
	};

	// qw 27
	float ringGrowRate;
	float ringWaveStartPos;
	float ringFrequency;
	float ringInvCount;

	// qw 28
	float ringPhase;
	float ringHeight;
	float ringAttack;
	float ringInvAttack;

	// qw 29
	float ringAttackSustain;
	float ringInvRelease;
	float unused2;
	float unused3;
};

struct VUGrids
{
	// 85 is pretty much a magic number.  no doubts about it... :)
#define GRID_QWC 86
	float qw[GRID_QWC * 4];
};

struct VUBumps
{
	VUBump bumps[MAX_BUMPS];
};

#if 1
extern unsigned int	Water_DmaTag		__attribute__( ( section( ".vudata" )));
extern unsigned int	Water_CodeStart	__attribute__( ( section( ".vudata" )));
extern unsigned int	Water_CodeEnd		__attribute__( ( section( ".vudata" )));
#else
extern unsigned int	Water2_DmaTag		__attribute__( ( section( ".vudata" )));
extern unsigned int	Water2_CodeStart	__attribute__( ( section( ".vudata" )));
extern unsigned int	Water2_CodeEnd		__attribute__( ( section( ".vudata" )));
#define Water_DmaTag Water2_DmaTag
#define Water_CodeStart Water2_CodeStart
#define Water_CodeEnd Water2_CodeEnd
#endif



static float RandUnit(void)
{
	return((float)(rand() & 0xffff) / (float)0xffff);
}

void Water::RenderPlatform(const CInstance* pInstance, const Graphics4x4* pmLocalToCamera, float fZCamDist)
{
	int i;
#ifdef DEBUG_WATER
	if(Water_Dbg.mDisableDrawWater)
		return;
#endif
	
	Texture* backBuffer = NULL;
	for(i = 0;i < mNumLayers; i++)
	{
		if(mLayerModes[i].mLayerType == WaterLayerModes::LAYER_DISTORT)
		{
			//backBuffer = TextureMgr::CopyBackBuffer(mBackBufferCopyHandle);
			backBuffer = TextureMgr::CopyBackBuffer(WaterFuzzFactor);
			break;
		}
	}

	static const u32 size1 = 
		sizeof(VUMiscData) + 
		sizeof(VUWaterSprites);
	static const u32 size2 = 
		sizeof(VULayers) + 
		sizeof(VUAffectors) + 
		sizeof(VUBumps) +
		sizeof(VUWakes) +
		sizeof(VURings) +
		64 * sizeof(float); // for the noise table
	static const u32 size3 = 
		sizeof(VUGrids);

	static const u32 num1 = size1 / sizeof(u128);
	static const u32 num2 = size2 / sizeof(u128);
	static const u32 num3 = size3 / sizeof(u128);

	ASSERT(num1 < 256);
	ASSERT(num2 < 256);
	ASSERT(num3 < 256);

//	if(mTextureArray)
//	{
//		mTextureArray->Lock();
//	}
	TextureMgr::Cache().Lock( mTextureArray );

	// we need to use the poorer (but non-negative) dither matrix here...
	GSState__Set(SCE_GS_DIMX, GSState.POSDIMX);

	GSState__Send();

	u32* data = (u32*)MasterList::StartMiscData();
	
	// first a dma tag...
	*(u64*)data = MAKE_DMATAG(num1, 0, TAGID_CALL, 0, &Water_DmaTag);
	data += 2;

	// a flush... we need one of these
	*data++ = createFLUSH();

	// an unpack for the data...
	*data++ = createUNPACK(0, V4_32, num1, false);

#define DATA_PTR(type, name) \
		  type* name = (type*)data; \
		  data += (sizeof(type) >> 2);


	//////////////////////
	// fill it all in
	//////////////////////
	
	////////////
	// misc data
	////////////
	DATA_PTR(VUMiscData, miscData);

	// scales
	miscData->scaleX = mXScale;
	miscData->scaleZ = mZScale;

	int iip = 1;
#ifdef DEBUG_WATER
	if(Water_Dbg.mDrawSubDivisionRings)
	{
		iip = 0;
	}
#endif

	// giftag
	GIFTag* fantag = (GIFTag*)&miscData->fantag;
	GIFTag* striptag = (GIFTag*)&miscData->striptag;

	fantag->setNLOOP(0);
	fantag->setEOP(1);
	fantag->setPRE(1);
	//                           prim             iip tme fge abe aa1 fst ctx fix
	fantag->setPRIM(createPRIM(PRIM_TRIANGLE_FAN, iip,  1,  0,  1,  0,  0,  0,  0));
	fantag->setFLG(GIFTag::FLG_PACKED);
	fantag->setRegs(3, 0x512);

	striptag->setNLOOP(0);
	striptag->setEOP(1);
	striptag->setPRE(1);
	//                             prim             iip tme fge abe aa1 fst ctx fix
	striptag->setPRIM(createPRIM(PRIM_TRIANGLE_STRIP, iip,  1,  0,  1,  0,  0,  0,  0));
	striptag->setFLG(GIFTag::FLG_PACKED);
	striptag->setRegs(3, 0x512);

	GIFTag* misctag = (GIFTag*)&miscData->misctag;
	misctag->setNLOOP(0);
	misctag->setEOP(1);
	misctag->setPRE(0);
	misctag->setFLG(GIFTag::FLG_PACKED);
	misctag->setRegs(1, 0xE);

	// local to clip
	miscData->localToClip = mInstance->Matrix() * *(const Graphics4x4*)&PS2Renderer::GetWorldToClip();

	// clip to screen
	miscData->clipToScreen = *(const Graphics4x4*)&PS2Renderer::GetClipToScreen();

	miscData->worldToLocal = mWorldToLocal;
	miscData->screenToClip = *(const Graphics4x4*)&PS2Renderer::GetScreenToClip();

	Matrix4x4 clipToWorld = *(const Matrix4x4*)&PS2Renderer::GetWorldToClip();
	clipToWorld.Invert();
	miscData->clipToWorld = clipToWorld;

#ifdef DEBUG_WATER
	if(Water_Dbg.mDisableSurfaceHeight)
		miscData->surfaceHeight = 0.0f;
	else
#endif
		miscData->surfaceHeight = mPool->CurrentSurfaceHeight();

	miscData->wakeRadiusSquared = mPool->mWakeRadiusSquared;

	miscData->offsetX = mXOffset;
	miscData->offsetZ = mZOffset;

	miscData->mT = mT;

	miscData->eyePointX = mEyePoint.x();
	miscData->eyePointY = mEyePoint.y();
	miscData->eyePointZ = mEyePoint.z();

	miscData->flags0 = 0x00;
	miscData->flags1 = 0x00;
#ifdef DEBUG_WATER
	if(Water_Dbg.mDrawNormalLines)
		miscData->flags0 |= 0x01;
	// this one is really random vertex colors
	if(Water_Dbg.mDrawSubDivisionRings)
		miscData->flags0 |= 0x10;
#endif

	// report the noise to the microcode...
#ifdef DEBUG_WATER
	if(Water_Dbg.mDisableNoise)
	{
		miscData->noiseYScale = 0.0f;
	}
	else
#endif
	{
		miscData->noiseXScale = mPool->mNoiseScale.x();
		miscData->noiseYScale = mPool->mNoiseScale.y();
		miscData->noiseZScale = mPool->mNoiseScale.z();
		miscData->noiseTScale = mPool->mNoiseTScale;
	}

	// subdivision values
#ifdef Water_DmaTag
	miscData->maxSubDivDepth = Math::Min(Math::Max(mMaxSubDivDepth+1, 1), 10);
#else
	miscData->maxSubDivDepth = Math::Min(Math::Max(mMaxSubDivDepth+2, 1), 10);
#endif
	miscData->minSubDivDepth = Math::Min(Math::Max(mMinSubDivDepth+2, 0), 10);

	// rings
	miscData->ringGrowRate = mPool->mRingGrowRate;
	miscData->ringWaveStartPos = mPool->mRingWaveStartPosition;
	miscData->ringFrequency = Math::Inverse(mPool->mRingWaveLength);
	miscData->ringInvCount = Math::Inverse(mPool->mRingWaveCount);
	miscData->ringPhase = mPool->mRingWavePhaseShift;
	miscData->ringHeight = mPool->mRingWaveHeight;
	miscData->ringAttack = mPool->mRingAttack;
	miscData->ringInvAttack = Math::Inverse(mPool->mRingAttack);
	miscData->ringAttackSustain = mPool->mRingAttack + mPool->mRingSustain;
	miscData->ringInvRelease = Math::Inverse(mPool->mRingRelease);

	////////////
	// sprites
	////////////
	DATA_PTR(VUWaterSprites, waterSprites);
	u32 sprites = mPool->Sprites().Count();
	for(i = 0; i < sprites; i++)
	{
		WaterSprite* sprite = mPool->Sprites()[i];
		u32 color = sprite->Color();
		
		u_long tex0_1 = 0;
		u_long tex1_1 = 0;
		u_long clamp_1 = 0;
		u_long miptbp1_1 = 0;
		u_long miptbp2_1 = 0;

		// this is temporary since the test texture Im using is not clamped... and should be...
		//LOCKUP();
		clamp_1 = SCE_GS_SET_CLAMP_1(1, 1, 0, 0, 0, 0);

		// force bilerp on...
		tex1_1 |= 0x00000020;

		waterSprites->sprite[i].textureMatrix = sprite->mTexMatrix;
		waterSprites->sprite[i].R = (color >> 1) & 0x7F;
		waterSprites->sprite[i].G = (color >> 9) & 0x7F;
		waterSprites->sprite[i].B = (color >> 17) & 0x7F;
		waterSprites->sprite[i].A = (color >> 25) & 0x7F;
		waterSprites->sprite[i].ALPHA1 = _RenderState__GetALPHA1Settings_BLENDFUNC(sprite->GetAlphaBlendFunc());
		waterSprites->sprite[i].MIPTBP1 = miptbp1_1;
		waterSprites->sprite[i].MIPTBP2 = miptbp2_1;
		waterSprites->sprite[i].TEX0 = tex0_1;
		waterSprites->sprite[i].TEX1 = tex1_1;
		waterSprites->sprite[i].CLAMP = clamp_1;
	}
	waterSprites->sprite[0].SpriteCountInSprite0 = sprites;
	MasterList::EndMiscData(data);
	
	////////////
	// bumps
	////////////
	// first a dma tag...
	data = (u32*)MasterList::StartMiscData();
	*(u64*)data = MAKE_DMATAG(num2, 0, TAGID_CNT, 0, 0);
	data += 2;
	*data++ = createNOP();
	// an unpack for the data...
	*data++ = createUNPACK(num1, V4_32, num2, false);


	DATA_PTR(VUBumps, bumps);
	for(i = 0; i < MAX_BUMPS; i++)
	{
		u32* d = (u32*)bumps;
		for(int j = 0; j < sizeof(VUBump); j+=4)
		{
			*d++ = 0xBBBB;
		}
	}
	// do nothing for now...

	////////////
	// wake pairs
	////////////
	DATA_PTR(VUWakes, wakes);
	memset(wakes, 0, sizeof(VUWakes));
	int currPoint;
	for(i = 0, currPoint = 0; i < mPool->mWakePointCount; i++)
	{
		if(mPool->mWakePoints[i].lifeLeft < 0.0f) continue;
		u8 scale = (u8)(mPool->mWakePoints[i].scale * mPool->mWakePoints[i].lifeLeft * 127.5f);
		if(scale == 0) continue;
		wakes->wake[currPoint].x = mPool->mWakePoints[i].pos.x();
		wakes->wake[currPoint].z = mPool->mWakePoints[i].pos.y();
		wakes->wake[currPoint++].scale = scale;
	}
	// we need an even count...
	if(currPoint & 0x1)
	{
		wakes->wake[currPoint].x = 0.0f;
		wakes->wake[currPoint].z = 0.0f;
		wakes->wake[currPoint++].scale = 0;
	}
	wakes->wake[0].countInWake0 = currPoint >> 1;
#ifdef DEBUG_WATER
	if(Water_Dbg.mDisableWakes)
		wakes->wake[0].countInWake0 = 0;
#endif

	////////////
	// rings...
	////////////
	DATA_PTR(VURings, rings);
	int numRings = 0;
	float oldest = mT - (mPool->mRingAttack + mPool->mRingSustain + mPool->mRingRelease);
	for(i = 0; i < MAX_RINGS; i++)
	{
		if(mPool->mRings[i].mStartTime < oldest) continue;

		rings->rings[numRings].centerX = mPool->mRings[i].mCenter.x();
		rings->rings[numRings].centerZ = mPool->mRings[i].mCenter.y();
		rings->rings[numRings].startTime = mPool->mRings[i].mStartTime;
		numRings++;
	}
	rings->rings[0].ringCountIn0 = numRings;

#ifdef DEBUG_WATER
	if(Water_Dbg.mDisableRings)
		rings->rings[0].ringCountIn0 = 0;
#endif


	////////////
	// affectors
	////////////
	DATA_PTR(VUAffectors, affectors);
	u32 affs = mPool->Affectors().Count();
	for(int i = 0; i < affs; i++)
	{
		WaterAffector* affector = mPool->Affectors()[i];
		Vector3 pos = affector->WorldPosition();

		affectors->affector[i].x = pos.x();
		affectors->affector[i].z = pos.z();
		affectors->affector[i].minRadius = affector->mWaterMinRadius;
		affectors->affector[i].maxRadius = affector->mWaterMaxRadius;
		affectors->affector[i].maxSubDivDepth = Math::Max(affector->mWaterMaxSubDivisionDepth - 1, 1);
	}
	affectors->affector[0].affectorCountInAffector0 = affs;
	

	////////////
	// layerinfo
	////////////
	DATA_PTR(VULayers, layers);
	for(int i = 0;i < mNumLayers; i++)
	{
		layers->layer[i].scaleU = mLayerModes[i].mScaleU;
		layers->layer[i].scaleV = mLayerModes[i].mScaleV;
		layers->layer[i].flowU = mLayerModes[i].mFlowU;
		layers->layer[i].flowV = mLayerModes[i].mFlowV;
		layers->layer[i].layerType = mLayerModes[i].mLayerType;
		layers->layer[i].vtxAdjParameter = mLayerModes[i].mVtxAdjParameter;
		layers->layer[i].normAdjParameter = mLayerModes[i].mNormAdjParameter;
		layers->layer[i].light = mLayerModes[i].mLight;
		layers->layer[i].layerFlags = 0x0000;
		if(mLayerModes[i].mFlags & WaterLayerModes::LAYER_FADE_EDGES)
			layers->layer[i].layerFlags |= 0x0001;
		if(mLayerModes[i].mFlags & WaterLayerModes::LAYER_FADE_CENTER)
			layers->layer[i].layerFlags |= 0x0002;

		Texture* texture;
		if(mLayerModes[i].mLayerType == WaterLayerModes::LAYER_DISTORT)
			texture = backBuffer;
		else if(mLayerModes[i].mFlags & WaterLayerModes::LAYER_TEX_BIND)
			texture = (Texture*)mLayerModes[i].mTexture;
		else
			texture = mTextureArray->GetTexture(mLayerModes[i].mTextureIndex);
		
		u_long tex0_1 = 0;
		u_long tex1_1 = 0;
		u_long clamp_1 = 0;
		u_long miptbp1_1 = 0;
		u_long miptbp2_1 = 0;

		// make the clamp register correct.
		TextureState__Request(0, TS_WRAPU, TS_WRAP_REPEAT);
		TextureState__Request(0, TS_WRAPV, TS_WRAP_REPEAT);

		texture->GetImage()->GetPlatformImage()->GetRegisterValues(&tex0_1, &tex1_1, &clamp_1, &miptbp1_1, &miptbp2_1);

		// force bilerp on...
		tex1_1 |= 0x00000020;

		layers->layer[i].MIPTBP1 = miptbp1_1;
		layers->layer[i].MIPTBP2 = miptbp2_1;
		layers->layer[i].TEX0 = tex0_1;
		layers->layer[i].TEX1 = tex1_1;
		layers->layer[i].CLAMP = clamp_1;
		layers->layer[i].ALPHA1ABCD = 0x44;
		layers->layer[i].ALPHA1FIX = (u8)(mLayerModes[i].mOpacity * 255.0f);
	}
	layers->layer[0].numLayersInLayer0 = mNumLayers;

	////////////
	// noise table
	////////////
	{
		static float noisetable[64];
		static bool initednoise = false;
		if(!initednoise)
		{
			// fill table with evenly distributed values...
			for(int i = 0; i < 64; i++)
			{
				noisetable[i] = i / 64.0f;
			}
			// randomize the values...
			for(int i = 0; i < 256; i++)
			{
				int x = (rand() >> 10) & 0x3F;
				int z = (rand() >> 10) & 0x3F;
				Math::Swap(noisetable[x], noisetable[z]);
			}
			initednoise = true;
		};
		typedef float VUNoiseTable[64];
		DATA_PTR(VUNoiseTable, noise);
		memcpy(noise, noisetable, 64 * sizeof(float));
	}

	MasterList::EndMiscData(data);

	////////////
	// grids
	////////////
	// first a dma tag...
	data = (u32*)MasterList::StartMiscData();

	*(u64*)data = MAKE_DMATAG(num3, 0, TAGID_CNT, 0, NULL);
	data += 2;
	*data++ = createNOP();
	*data++ = createUNPACK(num2 + num1, V4_32, num3, false);

	DATA_PTR(VUGrids, grids);
	u32* d = (u32*)grids;
	// this should ALWAYS be the case!
	ASSERT(mDampeningGrid.mGridSize == 17);
	// this should also ALWAYS be the case!
	ASSERT(mPhysicsEnabled == false || mPhysicsGrid.mGridSize == 17);

	// we merge the 2 grids so that the top 24 bits is the physics height and the bottom
	// 8 bits is the dampen value.
	// we arrange it so that the value we want can be found a little faster...

	memset(d, 0, sizeof(VUGrids));
	for(int x = 0; x < 17; x++)
	{
		for(int z = 0; z < 17; z++)
		{
			int qword = z / 4;
			int word = z - qword*4;
			qword += x * 5 + 1;

			ASSERT(qword < GRID_QWC);
			ASSERT(qword*4 + word < GRID_QWC*4);

			float physicsVal;
			u8 dampval;

#ifdef DEBUG_WATER
			if(Water_Dbg.mDisablePhysics || !mPhysicsEnabled)
#else
			if(!mPhysicsEnabled)
#endif
				physicsVal = 0.0f;
			else
				physicsVal = mPhysicsGrid(x, z);
				
#ifdef DEBUG_WATER
			if(Water_Dbg.mDisableDampening)
				dampval = 255;
			else
#endif
				dampval = mDampeningGrid(x, z);

			d[qword*4 + word] = (*(unsigned int*)&physicsVal & 0xFFFFFF00) | dampval;
		}
//		d[x*5 + 4*4 + 2] = 0.0f;
//		d[x*5 + 4*4 + 3] = 0.0f;
	}
	MasterList::EndMiscData(data);

	////////////
	// fire it off
	////////////
	MasterList::AppendDMATAG();
	MasterList::Append(createMSCAL(0));
	MasterList::Append(createFLUSH());
	
//	if (mTextureArray)
//	{
//		mTextureArray->Unlock();
//	}

	PS2Renderer::JustTrashedVUInstructionMem((u_int)&Water_CodeStart, (u_int)&Water_CodeEnd - (u_int)&Water_CodeStart);

	// restore the dimx
	GSState__Set(SCE_GS_DIMX, GSState.DIMX);

}
