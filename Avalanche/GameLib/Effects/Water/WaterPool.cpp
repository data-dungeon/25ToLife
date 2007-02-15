//
// Author: Tyler Colbert
// Purpose: manages a set of related Water squares.
//

#include "Effects/EffectsPCH.h"



#include "WaterPool.h"
#include "Water.h"
#include "WaterEquation.h"
#include "WaterAffector.h"
#include "WaterSprite.h"
#include "collide/ObbToObb.h"
#include "cmdcon/cmdcon.h"


WaterPool::WaterPool(void)
	: mEquations(4, 4, true), // start expecting 4 and grow 4 at a time..
	  mAffectors(1, 1, false), // start expecting 1 and grow 1 at a time..
	  mSprites(MAX_SPRITES, 1, true) // expect the max # of sprites possible & grow 1 at a time (who cares)...
{
	mEquationFlowRateX = 0.0f;
	mEquationFlowRateZ = 0.0f;
	mWakePoints = 0;
	mSurfaceHeight = mSurfaceHeightTarget = mSurfaceHeightVelocity = 0.0f;
	mT = 0.0f;
	mName = 0;
}

WaterPool::~WaterPool(void)
{
	if(mWakePoints)
		delete[] mWakePoints;
	delete [] mName;
}

void WaterPool::Update(float deltaTime, Graphics4x4& cameraMatrix)
{
	u32 count = Count();
	u32 idx;

	mDeltaT = deltaTime;
	mT += deltaTime;

	// update the surface height...
	if(mSurfaceHeight != mSurfaceHeightTarget)
	{
		if(mSurfaceHeightTarget < mSurfaceHeight)
		{
			mSurfaceHeight -= mSurfaceHeightVelocity * mDeltaT;
			if(mSurfaceHeight < mSurfaceHeightTarget)
				mSurfaceHeight = mSurfaceHeightTarget;
		}
		else
		{
			mSurfaceHeight += mSurfaceHeightVelocity * mDeltaT;
			if(mSurfaceHeight > mSurfaceHeightTarget)
				mSurfaceHeight = mSurfaceHeightTarget;
		}
	}

	// we need to update the OBB to handle the change in surface height...
	mObb = mObbWithoutSurfaceHeight;
	mObb.Position().y(mObb.Position().y() + mSurfaceHeight);
	
	// kill old equations
	ExpireEquations(deltaTime);

	// expire sprites that are timed...
	ExpireWaterSprites(deltaTime);

	// apply affectors motion to the cells
	ApplyAffectorsForce();

	#ifndef USE_ANIHF_WATER
	// update the wakes
	UpdateWakePoints(deltaTime);
	#endif

// 	// determine affector mState
// 	UpdateAffectorStates();

	for(idx = 0; idx < count; idx++)
	{
		Get(idx)->Update(deltaTime, cameraMatrix);
	}
}

bool WaterPool::CreateWater(const char* poolFileName, const WaterParams& params, int x, int z)
{
	// Create the height field!
	Water *water = new Water();

	if(water)
	{
		// init it from the parameters && add it to us
		if(water->InitFromParams(params, x, z, this) && Add(water))
		{
#ifdef ENABLE_WATCHCLIENT
			WatchClient *wc = WatchClient::Instance();

#define REGISTERLAYERVAR(name, min, max)																															\
			sprintf(buffer, "cell_%d_%d_Layer_%d_" #name, x, z, layer);																						\
			wc->RegisterVariable(const_cast<char*>(poolFileName), &water->mLayerModes[layer].m ## name, water->mLayerModes[layer].m ## name, min, max, &buffer[0]);

			char buffer[100];
			sprintf(buffer, "cell_%d_%d_Elasticity", x, z);
			wc->RegisterVariable(const_cast<char*>(poolFileName), &water->mElasticity, water->mElasticity, 0.0f, 1.0f, &buffer[0]);
			for(int layer = 0; layer < water->mNumLayers; layer++)
			{
				REGISTERLAYERVAR(ScaleU, 0.0f, 10.0f);
				REGISTERLAYERVAR(ScaleV, 0.0f, 10.0f);
				REGISTERLAYERVAR(FlowU, 0.0f, 10.0f);
				REGISTERLAYERVAR(FlowV, 0.0f, 10.0f);
				REGISTERLAYERVAR(OpacityOverTime, 0.0f, 5.0f);
				REGISTERLAYERVAR(Opacity, 0.0f, 1.0f);
			}
#endif //ENABLE_WATCHCLIENT
			return true;
		}

		delete water;
	}
	ASSERT(false);
	return false;
}

bool WaterPool::Create(const char *directory, const char *poolFileName)
{
	// Allocate a parameter field
	WaterParams params;
	Defaults(params);

	// Load in the parameters
	if (!LoadScript(directory, poolFileName, params))
		return(false);

	params.angle = Math::Deg2Rad(params.angle);

	#ifndef USE_ANIHF_WATER		// PAB - use Tylers water for PS2 only
	return Create(params, poolFileName);
	#else
	return Create(1, params, poolFileName, -1, -1);
	#endif
}

///////////////////////////////////////////////////////////////////////
// Special versions of Create to run Food's old water system instead
// 
// The 'gc' param isnt used, its just to distinguish from the original
// for overloading reasons.
//
// Author: PAB 7/2/03
///////////////////////////////////////////////////////////////////////
bool WaterPool::Create(int gc, const WaterParams& params, const char *name)
{
	mXScale = params.cellSizeX;
	mZScale = params.cellSizeZ;
	mInvXScale = 1.0f / mXScale;
	mInvZScale = 1.0f / mZScale;
	mPosition = params.pos;
	mSpriteFlowX = 0.0f;
	mSpriteFlowZ = 0.0f;
	mAngle = params.angle;
	g_aniHFMgr.Create(params,name,-1,-1);
	return true;
}

bool WaterPool::Create(int gc, const WaterParams& params, const char *name, int x, int z)
{
	mXScale = params.cellSizeX;
	mZScale = params.cellSizeZ;
	mInvXScale = 1.0f / mXScale;
	mInvZScale = 1.0f / mZScale;
	mPosition = params.pos;
	mSpriteFlowX = 0.0f;
	mSpriteFlowZ = 0.0f;
	mAngle = params.angle;
	g_aniHFMgr.Create(params,name,x,z);
	return true;
}

///////////////////////////////////////////////////////////////////////
// dbl loaded water

bool WaterPool::Create(const WaterParams& params, const char* name)
{
	bool success = true;

	// save the cell scales
	mXScale = params.cellSizeX;
	mZScale = params.cellSizeZ;
	mInvXScale = 1.0f / mXScale;
	mInvZScale = 1.0f / mZScale;
	mPosition = params.pos;
	mAngle = params.angle;
	mSpriteFlowX = -params.spriteFlowX * mXScale;
	mSpriteFlowZ = -params.spriteFlowZ * mZScale;

	mPhysicsForce = params.physicsForce;
	mPhysicsTrailAffector = params.physicsTrailAffector;

	// make OBB...
	// +/- 1.0 in y is assumed...
	AABB a;
	a.maxV.Set(0.5f * params.cellsX * mXScale,
				  0.1f,
				  0.5f * params.cellsZ * mZScale);
	a.minV = -a.maxV;
	// ok, that I can see, there is no way to generate a rotation for a DirCos3x3... REALLY STUPID!
	// and unfortunately, it leaves me to either write one... which im not really fond of doing right now
	// or cheat... ICK... WHY CANT WE HAVE GOOD MATRIX AND VECTOR CLASSES?
	Cartesian4x4 m;
	m.SetYRotation(mAngle);
	mObbWithoutSurfaceHeight.Orientation() = m;
	Vector3 center = a.maxV * m;
	mObbWithoutSurfaceHeight.Position().Set(mPosition.x() + center.x(), mPosition.y(), mPosition.z() + center.z());
	mObbWithoutSurfaceHeight.GetAABB() = a;
	mObbWithoutSurfaceHeight.UpdateRadius();

	mObb = mObbWithoutSurfaceHeight;
	

	// setup wakes...
	mWakePointCount = params.wakePointCount;

	#ifdef USE_ANIHF_WATER
	mWakePointCount = 0;		// no wakes in this water system
	#endif

	if(mWakePointCount)
	{
#ifdef PS2
		if(mWakePointCount > MAX_WAKES)
			mWakePointCount = MAX_WAKES;
#endif

		mWakePoints = new WakePoint[mWakePointCount];
		mWakeIndex = 0;
		mWakeRadiusSquared = params.wakeRadius * params.wakeRadius;
		mInvWakeRadiusSquared = Math::Inverse(mWakeRadiusSquared);
		mWakeHeight = params.wakeHeight;
		mWakeLifetime = params.wakeLifetime;
		mInvWakeLifetime = Math::Inverse(mWakeLifetime);
		mWakeForwardContribution = params.wakeForwardContribution;
		mWakePerpendicularContribution = params.wakePerpendicularContribution;
		mWakeMinSpeedRatio = params.wakeMinSpeedRatio;
		mWakeTrailAffector = params.wakeTrailAffector;
		mWakeDelay = params.wakeDelay;

		// clear them out so they dont contribute
		for(int i = 0; i < mWakePointCount; i++)
		{
			mWakePoints[i].lifeLeft = 0.0f;
			mWakePoints[i].pos.Clear();
			mWakePoints[i].vel.Clear();
			mWakePoints[i].scale = 0.0f;
		}
	}

	// setup rings...
	mRingAttack = params.ringAttack;
	mRingSustain = params.ringSustain;
	mRingRelease = params.ringRelease;
	mRingGrowRate = params.ringGrowRate;
	mRingWaveLength = params.ringWaveLength;
	mRingWaveStartPosition = params.ringWaveStartPosition;
	mRingWaveHeight = params.ringWaveHeight;
	mRingWaveCount = params.ringWaveCount;
	mRingWavePhaseShift = params.ringWavePhaseShift;
	for(int i = 0; i < MAX_RINGS; i++)
	{
		// this assures that they dont contribute to the water...
		mRings[i].mStartTime = -(mRingAttack + mRingSustain + mRingRelease + 1.0f);
	}
	
	// Need some matricies
	mLocalToWorld.Identity();
	mLocalToWorld.CatScale(mXScale, 1.0f, mZScale);
	mLocalToWorld.CatYRotation(mAngle);
	mLocalToWorld.PokeTranslation(mPosition);
	mLocalToWorldBasis.Identity();
	mLocalToWorldBasis.CatYRotation(mAngle);
	mLocalToWorldBasis.PokeTranslation(mPosition);

	// we need the inverse of our local to world matrix...
	VERIFY(mLocalToWorld.Invert(mWorldToLocal));
	VERIFY(mLocalToWorldBasis.Invert(mWorldToLocalBasis));

	// loop through all grid cells
	for(int x = 0; x < params.cellsX; x++)
	{
		for(int z = 0; z < params.cellsZ; z++)
		{
			if(params.cellInfo[x][z].flags.inUse)
			{
				#ifndef USE_ANIHF_WATER			// use Tyler's water on PS2 only (PAB)
				success = success && CreateWater(name, params, x, z);
				#else
				success = success && Create(1, params, name, x, z);
				#endif
			}
		}
	}

	// create equations
	mNoiseScale = params.noiseScale;
	mNoiseTScale = params.noiseFrequency;

	// register those variables that are pool wide...
#ifdef ENABLE_WATCHCLIENT
	WatchClient *wc = WatchClient::Instance();
	wc->RegisterVariable(const_cast<char*>(name), &mNoiseTScale, mNoiseTScale, -10.0f, 10.0f, "NoiseTScale");
	wc->RegisterVariable(const_cast<char*>(name), &mEquationFlowRateX, mEquationFlowRateX, -1.0f, 1.0f, "EquationFlowRateX");
	wc->RegisterVariable(const_cast<char*>(name), &mEquationFlowRateZ, mEquationFlowRateZ, -1.0f, 1.0f, "EquationFlowRateZ");
	wc->RegisterVariable(const_cast<char*>(name), &mSurfaceHeightTarget, mSurfaceHeightTarget, -10.0f, 10.0f, "SurfaceHeightTarget");
	wc->RegisterVariable(const_cast<char*>(name), &mSurfaceHeightVelocity, mSurfaceHeightVelocity, 0.0f, 1.0f, "SurfaceHeightVelocity");
	wc->RegisterVariable(const_cast<char*>(name), &mSpriteFlowX, mSpriteFlowX, -1.0f, 1.0f, "SpriteFlowX");
	wc->RegisterVariable(const_cast<char*>(name), &mSpriteFlowZ, mSpriteFlowZ, -1.0f, 1.0f, "SpriteFlowZ");
	wc->RegisterVariable(const_cast<char*>(name), &mWakeRadiusSquared, mWakeRadiusSquared, 0.0f, 0.0125f, "WakeRadiusSquared");
	wc->RegisterVariable(const_cast<char*>(name), &mWakeHeight, mWakeHeight, -1.0f, 1.0f, "WakeHeight");
	wc->RegisterVariable(const_cast<char*>(name), &mWakeLifetime, mWakeLifetime, 0.0f, 4.0f, "WakeLifetime");
	wc->RegisterVariable(const_cast<char*>(name), &mWakeForwardContribution, mWakeForwardContribution, -2.0f, 2.0f, "WakeForwardContribution");
	wc->RegisterVariable(const_cast<char*>(name), &mWakePerpendicularContribution, mWakePerpendicularContribution, 0.0f, 15.0f, "WakePerpendicularContribution");
	wc->RegisterVariable(const_cast<char*>(name), &mWakeMinSpeedRatio, mWakeMinSpeedRatio, 0.0f, 1.0f, "wakeMinSpeedRatio");
	wc->RegisterVariable(const_cast<char*>(name), &mWakeTrailAffector, mWakeTrailAffector, 0.0f, 2.0f, "WakeTrailAffector");
	wc->RegisterVariable(const_cast<char*>(name), &mWakeDelay, mWakeDelay, 0.0f, 0.25f, "WakeDelay");
	wc->RegisterVariable(const_cast<char*>(name), &mPhysicsForce, mPhysicsForce, 0.0f, 2.0f, "PhysicsForce");
	wc->RegisterVariable(const_cast<char*>(name), &mPhysicsTrailAffector, mPhysicsTrailAffector, 0.0f, 2.0f, "PhysicsTrailAffector");

	wc->RegisterVariable(const_cast<char*>(name), &mRingAttack, mRingAttack, 0.0f, 1.0f, "RingAttack");
	wc->RegisterVariable(const_cast<char*>(name), &mRingSustain, mRingSustain, 0.0f, 5.0f, "RingSustain");
	wc->RegisterVariable(const_cast<char*>(name), &mRingRelease, mRingRelease, 0.0f, 1.0f, "RingRelease");
	wc->RegisterVariable(const_cast<char*>(name), &mRingGrowRate, mRingGrowRate, 0.0f, 1.0f, "RingGrowRate");
	wc->RegisterVariable(const_cast<char*>(name), &mRingWaveLength, mRingWaveLength, 0.0f, 1.0f, "RingWaveLength");
	wc->RegisterVariable(const_cast<char*>(name), &mRingWaveStartPosition, mRingWaveStartPosition, 0.0f, 1.0f, "RingWaveStartPosition");
	wc->RegisterVariable(const_cast<char*>(name), &mRingWaveHeight, mRingWaveHeight, 0.0f, 1.0f, "RingWaveHeight");
	wc->RegisterVariable(const_cast<char*>(name), &mRingWaveCount, mRingWaveCount, 0.0f, 5.0f, "RingWaveCount");
	wc->RegisterVariable(const_cast<char*>(name), &mRingWavePhaseShift, mRingWavePhaseShift, 0.0f, 1.0f, "WavePhaseShift");

#endif // ENABLE_WATCHCLIENT

	// store the name for later lookup.
	mName = new char[strlen(name) + 1];
	strcpy( mName, name );

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// initialize the params
/////////////////////////////////////////////////////////////////////////////
void WaterPool::Defaults(WaterParams &params)
{
	// clear this
	memset(&params, '\0', sizeof(WaterParams));

	params.disableVisReg = false;
	params.cellsX = 1;
	params.cellsZ = 1;
	params.cellSizeX = 1.0f;
	params.cellSizeZ = 1.0f;
	params.subDivNodes = 0x7F;
	params.maxSubDivDepth = 4;
	params.minSubDivDepth = 0;
	params.dampenSubDivisions = 0;
	params.shadowSubDivisions = 0;
	params.physicsSubDivisions = 0;
	params.physicsWaveSpeed = 0.5f;
	params.physicsElasticity = 0.9f;
	params.physicsForce = 1.0f;
	params.physicsTrailAffector = 0.0f;

	params.spriteFlowX = 0.0f;
	params.spriteFlowZ = 0.0f;

	params.noiseScale.y(0.0f);

	// wakes
	params.wakePointCount = 0;
	params.wakeRadius = 0.0125f;
	params.wakeHeight = 0.125f;
	params.wakeLifetime = 0.25f;
	params.wakeForwardContribution = 0.25f;
	params.wakePerpendicularContribution = 1.0f;
	params.wakeMinSpeedRatio = 0.2f;
	params.wakeTrailAffector = 0.325f;
	params.wakeDelay = 0.0f;

	// rings
	params.ringAttack = 0.1f;
	params.ringSustain = 1.0f;
	params.ringRelease = 1.0f;
	params.ringGrowRate = 0.2f;
	params.ringWaveLength = 0.02f;
	params.ringWaveStartPosition = 0.1f;
	params.ringWaveHeight = 0.1f;
	params.ringWaveCount = 10.0f;
	params.ringWavePhaseShift = 3.14f;

	for(int i = 0; i < MAX_GRID_SIZE; i++)
	{
		for(int j = 0; j < MAX_GRID_SIZE; j++)
		{
			params.cellInfo[i][j].flags.inUse = 1;
			params.cellInfo[i][j].flags.receiveShadows = 0;
			params.cellInfo[i][j].flags.runPhysics = 1;
			params.cellInfo[i][j].flags.collides = 1;
			params.cellInfo[i][j].flags.clampN = 0;
			params.cellInfo[i][j].flags.clampS = 0;
			params.cellInfo[i][j].flags.clampE = 0;
			params.cellInfo[i][j].flags.clampW = 0;
			params.cellInfo[i][j].flags.scaleWaveOnDepth = 0;
			params.cellInfo[i][j].flags.slowWavesOnDepth = 0;
		}
	}

	// Now init the material
	for (int l = 0; l < MAX_LAYERS; l++)
	{
		params.layers[l].mTextureLayer = l;
		params.layers[l].mScaleU = 1.0f;
		params.layers[l].mScaleV = 1.0f;
		params.layers[l].mFlowU = 0.0f;
		params.layers[l].mFlowV = 0.0f;
		params.layers[l].mVtxAdjParameter.Set(1.0f, 1.0f, 1.0f);
		params.layers[l].mNormAdjParameter.Set(0.0f, 1.0f, 0.0f);

		params.layers[l].mOpacityOverTime = 0.0f;
		params.layers[l].mOpacity = 1.0f;
		params.layers[l].mFadeEdges = false;
		params.layers[l].mFadeCenter = false;
		params.layers[l].mFadeHeight = false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Load the params from a script
/////////////////////////////////////////////////////////////////////////////
bool WaterPool::LoadScript(const char *directory,const char *scriptName,WaterParams &params)
{
	// Alias the context
	const char *context = scriptName;

	// Setup all the variables
	SetupVariables(context, params);

	// Build the script file name & bind it
	char pathFile[256];
	PathFile(directory, scriptName, pathFile);
	g_console.BindScript(context, pathFile);

	// Execute the script
	bool ret = g_console.ExecuteContext(context);

	// Now clear this context (so we don't get dangling pointers)
	g_console.DeleteContext(context);

#ifdef GCN
	params.dampenSubDivisions = 1;
	params.shadowSubDivisions = 0;
	params.physicsSubDivisions = 0;
	params.subDivNodes = 1;
	params.maxSubDivDepth = 0;
#endif

	// If we couldn't load a file, we've failed
	return(ret);
}

/////////////////////////////////////////////////////////////////////////////
// Setup all the variables
/////////////////////////////////////////////////////////////////////////////
void WaterPool::SetupVariables(const char *context, WaterParams &params)
{
	int i, j;

	g_console.CreateVar(const_cast<char*>(context), "disableVisRegion", params.disableVisReg);
	g_console.CreateVar(const_cast<char*>(context), "cellSizeX", params.cellSizeX);
	g_console.CreateVar(const_cast<char*>(context), "cellSizeZ", params.cellSizeZ);
	g_console.CreateVar(const_cast<char*>(context), "subDivNodes", params.subDivNodes);
	g_console.CreateVar(const_cast<char*>(context), "maxSubDivDepth", params.maxSubDivDepth);
	g_console.CreateVar(const_cast<char*>(context), "minSubDivDepth", params.minSubDivDepth);
	g_console.CreateVar(const_cast<char*>(context), "dampenSubDivisions", params.dampenSubDivisions);
	g_console.CreateVar(const_cast<char*>(context), "shadowSubDivisions", params.shadowSubDivisions);
	g_console.CreateVar(const_cast<char*>(context), "physicsSubDivisions", params.physicsSubDivisions);
	g_console.CreateVar(const_cast<char*>(context), "physicsWaveSpeed", params.physicsWaveSpeed);
	g_console.CreateVar(const_cast<char*>(context), "physicsElasticity", params.physicsElasticity);
	g_console.CreateVar(const_cast<char*>(context), "physicsForce", params.physicsForce);
	g_console.CreateVar(const_cast<char*>(context), "physicsTrailAffector", params.physicsTrailAffector);

	g_console.CreateVar(const_cast<char*>(context), "spriteFlowX", params.spriteFlowX);
	g_console.CreateVar(const_cast<char*>(context), "spriteFlowZ", params.spriteFlowZ);

	g_console.CreateVar(const_cast<char*>(context), "cellsX", params.cellsX);
	g_console.CreateVar(const_cast<char*>(context), "cellsZ", params.cellsZ);
	g_console.CreateVar(const_cast<char*>(context), "position", (Vector3Packed*)&params.pos);
	g_console.CreateVar(const_cast<char*>(context), "angle", params.angle);

	// wakes
	g_console.CreateVar(const_cast<char*>(context), "wakePointCount", params.wakePointCount);
	g_console.CreateVar(const_cast<char*>(context), "wakeRadius", params.wakeRadius);
	g_console.CreateVar(const_cast<char*>(context), "wakeHeight", params.wakeHeight);
	g_console.CreateVar(const_cast<char*>(context), "wakeLifetime", params.wakeLifetime);
	g_console.CreateVar(const_cast<char*>(context), "wakeForwardContribution", params.wakeForwardContribution);
	g_console.CreateVar(const_cast<char*>(context), "wakePerpendicularContribution", params.wakePerpendicularContribution);
	g_console.CreateVar(const_cast<char*>(context), "wakeTrailAffector", params.wakeTrailAffector);
	g_console.CreateVar(const_cast<char*>(context), "wakeDelay", params.wakeDelay);

	// rings
	g_console.CreateVar(const_cast<char*>(context), "ringAttack", params.ringAttack);
	g_console.CreateVar(const_cast<char*>(context), "ringSustain", params.ringSustain);
	g_console.CreateVar(const_cast<char*>(context), "ringRelease", params.ringRelease);
	g_console.CreateVar(const_cast<char*>(context), "ringGrowRate", params.ringGrowRate);
	g_console.CreateVar(const_cast<char*>(context), "ringWaveLength", params.ringWaveLength);
	g_console.CreateVar(const_cast<char*>(context), "ringWaveStartPosition", params.ringWaveStartPosition);
	g_console.CreateVar(const_cast<char*>(context), "ringWaveHeight", params.ringWaveHeight);
	g_console.CreateVar(const_cast<char*>(context), "ringWavePhaseShift", params.ringWavePhaseShift);

	for(i = 0; i < MAX_GRID_SIZE; i++) for(j = 0; j < MAX_GRID_SIZE; j++)
	{
		g_console.CreateVar(const_cast<char*>(context), varName("cell_", i, j, "_inUse"), params.cellInfo[i][j].flags.inUse);
		g_console.CreateVar(const_cast<char*>(context), varName("cell_", i, j, "_receiveShadows"), params.cellInfo[i][j].flags.receiveShadows);
		g_console.CreateVar(const_cast<char*>(context), varName("cell_", i, j, "_runPhysics"), params.cellInfo[i][j].flags.runPhysics);
		g_console.CreateVar(const_cast<char*>(context), varName("cell_", i, j, "_collides"), params.cellInfo[i][j].flags.collides);
		g_console.CreateVar(const_cast<char*>(context), varName("cell_", i, j, "_clampN"), params.cellInfo[i][j].flags.clampN);
		g_console.CreateVar(const_cast<char*>(context), varName("cell_", i, j, "_clampS"), params.cellInfo[i][j].flags.clampS);
		g_console.CreateVar(const_cast<char*>(context), varName("cell_", i, j, "_clampE"), params.cellInfo[i][j].flags.clampE);
		g_console.CreateVar(const_cast<char*>(context), varName("cell_", i, j, "_clampW"), params.cellInfo[i][j].flags.clampW);
		g_console.CreateVar(const_cast<char*>(context), varName("cell_", i, j, "_scaleWaveOnDepth"), params.cellInfo[i][j].flags.scaleWaveOnDepth);
		g_console.CreateVar(const_cast<char*>(context), varName("cell_", i, j, "_slowWavesOnDepth"), params.cellInfo[i][j].flags.slowWavesOnDepth);
	}

	// texture info
	g_console.CreateVar(const_cast<char*>(context), "textureFile", params.textureFile, sizeof(params.textureFile) - 1);
	for(i = 0; i < MAX_LAYERS; i++)
	{
		g_console.CreateVar(const_cast<char*>(context), varName("Layer", i, "_textureIndex"), params.layers[i].mTextureLayer);
		g_console.CreateVar(const_cast<char*>(context), varName("Layer", i, "_method"), params.layers[i].mMethod, sizeof(params.layers[i].mMethod) - 1);
		g_console.CreateVar(const_cast<char*>(context), varName("Layer", i, "_pointsample"), params.layers[i].mPointSample);
		g_console.CreateVar(const_cast<char*>(context), varName("Layer", i, "_blend"), params.layers[i].mBlendFunc, sizeof(params.layers[i].mBlendFunc) - 1);
		g_console.CreateVar(const_cast<char*>(context), varName("Layer", i, "_mapType"), params.layers[i].mLayerType, sizeof(params.layers[i].mLayerType) - 1);
		g_console.CreateVar(const_cast<char*>(context), varName("Layer", i, "_uWrap"), params.layers[i].mScaleU);
		g_console.CreateVar(const_cast<char*>(context), varName("Layer", i, "_vWrap"), params.layers[i].mScaleV);
		g_console.CreateVar(const_cast<char*>(context), varName("Layer", i, "_uFlowRate"), params.layers[i].mFlowU);
		g_console.CreateVar(const_cast<char*>(context), varName("Layer", i, "_vFlowRate"), params.layers[i].mFlowV);
		g_console.CreateVar(const_cast<char*>(context), varName("Layer", i, "_vertexAdj"), (Vector3Packed*)&params.layers[i].mVtxAdjParameter);
		g_console.CreateVar(const_cast<char*>(context), varName("Layer", i, "_normalAdj"), (Vector3Packed*)&params.layers[i].mNormAdjParameter);
		g_console.CreateVar(const_cast<char*>(context), varName("Layer", i, "_light"), (Vector3Packed*)&params.layers[i].mWorldLight);

		g_console.CreateVar(const_cast<char*>(context), varName("Layer", i, "_fadeEdges"), params.layers[i].mFadeEdges);
		g_console.CreateVar(const_cast<char*>(context), varName("Layer", i, "_fadeCenter"), params.layers[i].mFadeCenter);
		g_console.CreateVar(const_cast<char*>(context), varName("Layer", i, "_fadeHeight"), params.layers[i].mFadeHeight);
		g_console.CreateVar(const_cast<char*>(context), varName("Layer", i, "_opacityOverTime"), params.layers[i].mOpacityOverTime);
		g_console.CreateVar(const_cast<char*>(context), varName("Layer", i, "_opacity"), params.layers[i].mOpacity);
	}

	// noise
	g_console.CreateVar(const_cast<char*>(context), "noiseScale", (Vector3Packed*)&params.noiseScale);
	g_console.CreateVar(const_cast<char*>(context), "noiseFrequency", params.noiseFrequency);
}

static char varNameBuffer[100];
char* WaterPool::varName(char *base, int index, char* name)
{
	sprintf(varNameBuffer, "%s%d%s", base, index, name);
	return varNameBuffer;
}
char* WaterPool::varName(char *base, int index1, int index2, char* name)
{
	sprintf(varNameBuffer, "%s%d_%d%s", base, index1, index2, name);
	return varNameBuffer;
}

/////////////////////////////////////////////////////////////////////////////
// test against the water cells
/////////////////////////////////////////////////////////////////////////////
bool WaterPool::Test(Segment &seg, Vector3* point)
{
	// trivially reject on the obb...
	float entryFrac, exitFrac;
	uint entryFace, exitFace;
	if(!mObb.IntersectLineInWorld(seg.start, seg.end - seg.start, entryFace, entryFrac, exitFace, exitFrac))
		return false;


	int cellCount = Count();
	for(int idx = 0; idx < cellCount; idx++)
	{
		Vector3 pt;
		if(Get(idx)->TestCollision(seg, pt))
		{
			if(point) *point = pt;
			return true;
		}
	}
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// What is the full path file?
/////////////////////////////////////////////////////////////////////////////
void WaterPool::PathFile(const char *path,const char *file,char *pathFile)
{
	// Build the script file name
	strcpy(pathFile, path);
	int len = strlen(pathFile);
	if (len > 0 && pathFile[len - 1] != '\\')
		strcat(pathFile, "\\");
	strcat(pathFile, file);
}


/////////////////////////////////////////////////////////////////////////////
// expire old equations
/////////////////////////////////////////////////////////////////////////////
void WaterPool::ExpireEquations(float deltaTime)
{
	// we go backwards because mEquations.Delete() is more efficient
	// that way (see comments in darray.h), and it keeps 'i' from
	// needing to be fixed up every delete
	int eqCount = mEquations.Count();
	float flowX = mEquationFlowRateX * deltaTime;
	float flowZ = mEquationFlowRateZ * deltaTime;

	for(int i = eqCount - 1; i >= 0; i--)
	{
		if(mEquations[i]->Decay(deltaTime, flowX, flowZ))
		{
			WaterEquation* eq = mEquations[i];
			mEquations.Delete(i);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// expire old sprites
/////////////////////////////////////////////////////////////////////////////
void WaterPool::ExpireWaterSprites(float deltaTime)
{
	// we go backwards because mEquations.Delete() is more efficient
	// that way (see comments in darray.h), and it keeps 'i' from
	// needing to be fixed up every delete
	int spCount = mSprites.Count();
	for(int i = spCount - 1; i >= 0; i--)
	{
		WaterSprite* sp = mSprites[i];

		// handle growing/shrinking sprites
		sp->Update(deltaTime);

		// expire some...
		if(sp->mLifeRemaining > 0.0f)
		{
			if((sp->mLifeRemaining -= deltaTime) <= 0.0f)
			{
				mSprites.Delete(i);
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// add/evaluate rings
/////////////////////////////////////////////////////////////////////////////
void WaterPool::AddRippleRing(Vector3 pos)
{
	int oldestRing = 0;

	// find earliest ring...
	for(int i = 1; i < MAX_RINGS; i++)
	{
		if(mRings[i].mStartTime < mRings[oldestRing].mStartTime)
			oldestRing = i;
	}

	// init it...
	Vector3 vpos = pos * WorldToLocal();
	mRings[oldestRing].mCenter.Set(vpos.x(),
											 vpos.z());
	mRings[oldestRing].mStartTime = mT;
}

float WaterPool::EvaluateWaterRings(float t, float x, float z)
{
	float height = 0.0f;

	float oldest = mT - (mRingAttack + mRingSustain + mRingRelease);
	float attacksustain = mRingAttack + mRingSustain;
	float attack = mT - mRingAttack;

	for(int i = 0; i < MAX_RINGS; i++)
	{
		float timescale;
		float posscale;

		// ring is invalid?
		ASSERT(mRings[i].mStartTime <= mT);


		// easyout the ones that are too old.
		if(mRings[i].mStartTime < oldest) continue;

		// calculate age
		float age = mT - mRings[i].mStartTime;

		// is attacking
		if(age < mRingAttack)
		{
			timescale = age / mRingAttack;
		}
		// is sustaining...
		else if(age < attacksustain)
		{
			timescale = 1.0f;
		}
		// is releasing
		else
		{
			timescale = 1.0f - ((age - attacksustain) / mRingRelease);
		}

		// ok, now we have the timescale value... lets generate the posscale value...
		float deltax = x - mRings[i].mCenter.x();
		float deltaz = z - mRings[i].mCenter.y();
		float distance = Math::Sqrt(deltax * deltax + deltaz * deltaz);

		float angle = (distance - mRingGrowRate*age + mRingWaveStartPosition) * (1.0f / mRingWaveLength);
		if(angle < 0.0f || angle > mRingWaveCount)
			continue;

		// get ramp from 0 - 1 - 0...
		float ramppos = 1 - Math::Abs(((angle * (1.0f / mRingWaveCount))*2.0f) - 1.0f);
		//float ramppos = 1.0f;
		
		posscale = Math::Cos(angle + mRingWavePhaseShift) * ramppos * mRingWaveHeight;

		// ok, now affect the height
		height += timescale * posscale;
	}
	return height;
}


/////////////////////////////////////////////////////////////////////////////
// add/evaluate wakes
/////////////////////////////////////////////////////////////////////////////
void WaterPool::AddWakePoint(Vector3 pos, Vector3 vel, float scale)
{
	Vector3 vpos = pos * WorldToLocal();
	Vector3 vvel = vel.BasisTransform(WorldToLocal());
	mWakePoints[mWakeIndex].pos.Set(vpos.x(),
											  vpos.z());
	mWakePoints[mWakeIndex].vel.Set(vvel.x() * InvXScale(),
											  vvel.z() * InvZScale());
	mWakePoints[mWakeIndex].lifeLeft = 1.0f;
	mWakePoints[mWakeIndex].scale = scale * mWakeHeight;
	mWakeIndex++;
	if(mWakeIndex == mWakePointCount)
		mWakeIndex = 0;
	mHasSomeLiveWakes = true;
}

void WaterPool::UpdateWakePoints(float deltaTime)
{
	float invWakeLifeTime = Math::Inverse(mWakeLifetime);
	for(int i = 0; i < mWakePointCount; i++)
	{
		mWakePoints[i].lifeLeft -= deltaTime * invWakeLifeTime;
		mWakePoints[i].pos.Set(
			mWakePoints[i].pos.x() + mWakePoints[i].vel.x() * deltaTime,
			mWakePoints[i].pos.y() + mWakePoints[i].vel.y() * deltaTime);
	}
}

float WaterPool::EvaluateWakes(float t, float x, float z)
{
	if(!mHasSomeLiveWakes)
		return 0.0f;

	Vector2 pt(x, z);
	float h = 0.0f;
	bool someAlive = false;

	for(int i = 0; i < mWakePointCount; i++)
	{
		if(mWakePoints[i].lifeLeft < 0.0f) continue;

		someAlive = true;

		float lenSq = (mWakePoints[i].pos - pt).LengthSquared();
		if(lenSq < mWakeRadiusSquared)
		{
			float scale = (mWakeRadiusSquared - lenSq) * (1.0f / mWakeRadiusSquared);
			float lifeleft = mWakePoints[i].lifeLeft;

			lifeleft *= 1.0f + mWakeDelay;
			if(lifeleft > 1.0f) lifeleft = 1.0f - (lifeleft - 1.0f);
			
			h = Math::Max(h, scale * lifeleft * mWakePoints[i].scale);
		}
	}

	mHasSomeLiveWakes = someAlive;

	return h;
}

/////////////////////////////////////////////////////////////////////////////
// Evaluate noise
/////////////////////////////////////////////////////////////////////////////
float WaterPool::EvaluateNoise(float t, float x, float z)
{
 	return mNoiseScale.y() * 
		(SigGen::GetNoise().Noise(t * mNoiseTScale, x*mNoiseScale.x()) * 
		 SigGen::GetNoise().Noise(t * mNoiseTScale, z*mNoiseScale.z() + 100.0f));
}

/////////////////////////////////////////////////////////////////////////////
// add/remove equations
/////////////////////////////////////////////////////////////////////////////
void WaterPool::AddEquation(WaterEquation* equation, bool Dampen)
{
	mEquations.Add(equation);
	equation->SetDampen(Dampen);
	equation->AddedToWater(this);
}

void WaterPool::RemoveEquation(WaterEquation* equation)
{
	// free up space in the affector
	int pos = mEquations.Find(equation);
	if(pos != -1)
		mEquations.Delete(pos);

	// TODO: remove an equation without delete'ing it...
}

/////////////////////////////////////////////////////////////////////////////
// add/remove sprites
/////////////////////////////////////////////////////////////////////////////

void WaterPool::AddWaterSprite(WaterSprite* sprite)
{
	// cap # of sprites...
	int numSprites = mSprites.Count();
	if(numSprites == MAX_SPRITES)
	{
		float lifeleft = sprite->mLifeRemaining;
		WaterSprite* curr = sprite;
		int index = 0;

		// find sprite that will die the soonest in favor of this one...
		for(int i = 0; i < numSprites; i++)
		{
			WaterSprite* tsprite = mSprites[i];
			if(tsprite->mLifeRemaining < lifeleft)
			{
				lifeleft = tsprite->mLifeRemaining;
				curr = tsprite;
				index = i;
			}
		}
		// did we find one?
		if(curr != sprite)
		{
			// release it...
			mSprites.Delete(index);
		}
		else
		{
			// the new one would die the soonest, so ignore it..
			delete sprite;
			return;
		}
	}

	// add the sprite...
	mSprites.Add(sprite);

	// let it know who owns it..
	sprite->AddedToWater(this);
}


/////////////////////////////////////////////////////////////////////////////
// add/remove affectors
/////////////////////////////////////////////////////////////////////////////

// when you add an affector, you loose ownership of the affector.
// ie. when ~Water() happens, it deletes all its affectors.
void WaterPool::AddAffector(WaterAffector* affector)
{
#ifdef ENABLE_WATCHCLIENT
	// add some variables for this affector...
	char buffer[100];
	sprintf(buffer,"WaterAff%d", mAffectors.Count());
	WatchClient *wc = WatchClient::Instance();
	wc->RegisterVariable(buffer, &affector->mWaterMinRadius, affector->mWaterMinRadius, 0.0f, 10.0f, "MinRadius");
	wc->RegisterVariable(buffer, &affector->mWaterMaxRadius, affector->mWaterMaxRadius, 0.0f, 10.0f, "MaxRadius");
	wc->RegisterVariable(buffer, &affector->mWaterMaxSpeed, affector->mWaterMaxSpeed, 1.0f, 6.0f, "MaxSpeed");
	wc->RegisterVariable(buffer, &affector->mWaterForceScale, affector->mWaterForceScale, -1.0f, +1.0f, "PhysicsForceScale");
#endif

	mAffectors.Add(affector);
}

// when you remove an affector, the Water does not delete it, you must.
// It is uncommon to remove an affector.
void WaterPool::RemoveAffector(WaterAffector* affector)
{
	// free up space in the affector
	int pos = mAffectors.Find(affector);
	if(pos != -1)
		mAffectors.Delete(pos);
}

/////////////////////////////////////////////////////////////////////////////
// apply affectors motion to the cells
/////////////////////////////////////////////////////////////////////////////
void WaterPool::ApplyAffectorsForce(void)
{
	int cellCount = Count();
	int afCount = mAffectors.Count();

	// ignore this function if time hasnt changed...
	if(mDeltaT == 0.0f) return;

	if(!cellCount) return;

	static int flipflop = 0;
	flipflop ^= 1;

	for(int i = 0; i < afCount; i++)
	{
		WaterAffector* aff = mAffectors.Get(i);

		if(aff->mWaterForceScale != 0.0f)
		{
			// Ignore cases where the actor is OVER/UNDER/NOT-IN the water
			// currently, this uses the AABB... thats icky except when rotation % 90 == 0
			Vector3 pos = aff->WorldPosition();

			bool point = mObb.PointInBox(pos);
			CObbToObbIntersection intersection(mObb, *aff->WorldOBB());
			bool box = intersection.SeparatingAxisTest();
			if(!point && !box)
				continue;
			
			// foods physics
			{

				Vector3 v = aff->WorldVelocity();
				//v.y *= 0.25f; // dampen y velocity...
				//v.y = Math::Max(v.y, 0.0f); // ingore upward motion..
				v.y(v.y() * 3.0f);
				float currentSpeed = v.Length();
				float speedRatio = Math::Clamp(currentSpeed / aff->mWaterMaxSpeed, 0.0f, 1.0f);
				float realForce = speedRatio * aff->mWaterForceScale * mPhysicsForce;

				pos -=  v * mPhysicsTrailAffector;
				
				// tell each cell about it...
				if (!Math::Zero(realForce))
				{
					for(int j = 0; j < cellCount; j++)
					{
						Water* cell = Get(j);
						cell->ApplyForce(pos, realForce);
					}
				}
			}
			// my wakes
			if(mWakePoints && box)
			{
				Vector3 v = aff->WorldVelocity();
				//v.y *= 0.25f; // dampen y velocity...
				//v.y = Math::Max(v.y, 0.0f); // ingore upward motion..
				v.y(0.0f);
				float currentSpeed = v.Length();
				float speedRatio = Math::Clamp(currentSpeed / aff->mWaterMaxSpeed, 0.0f, 1.0f);
				float realForce = speedRatio * aff->mWaterForceScale;

				
				// tell each cell about it...
				if (speedRatio > mWakeMinSpeedRatio)
				{
					pos = aff->WorldPosition();
					pos -= v * mWakeTrailAffector;

					Vector3 up(0.0f, 1.0f, 0.0f);
					
					// most movement is perp to our movement...
					Vector3 perp = Vector3::Cross(v, up) * speedRatio * mWakePerpendicularContribution;
					
					// factor in a little of our movement to make the wake follow
					// us after we stopped...
					Vector3 forward = v * (1.0f - speedRatio) * mWakeForwardContribution;

//  					if(flipflop)
						AddWakePoint(pos, perp + forward, speedRatio);
//  					else
						AddWakePoint(pos, perp.Negated() + forward, speedRatio);
				}
			}
		}
	}
}

// void Water::AddSegmentBump(float x, float z, float range, float height, float endx, float endz)
// {
// 	Bump* b = AddBump(x, z, range, height);
// 	b->type = BUMP_SEGMENT;
// 	b->mData.mSegment.mEndPoint.Set(endx, 0.0f, endz);
// 	b->mData.mSegment.mEndPoint = b->mData.mSegment.mEndPoint * GetWorldToLocal();
// 	b->mData.mSegment.mEndPoint.y = 0.0f;
// //	b->mData.mSegment.mEndPoint.Set(b->mData.mSegment.mEndPoint.x * GetXScale(), 0.0f, b->mData.mSegment.mEndPoint.z * GetZScale());
// }

// void Water::AddSemiCircleBump(float x, float z, float range, float height, float radius, float startAngle, float endAngle)
// {
// 	Bump* b = AddBump(x, z, range, height);
// 	b->type = BUMP_SEMICIRCLE;
// 	b->mData.mSemiCircle.mRadius = radius;
// 	b->mData.mSemiCircle.mStartAngle = startAngle;
// 	if(endAngle < startAngle)
// 		b->mData.mSemiCircle.mEndAngle = Math::TwoPi + endAngle;
// 	else
// 		b->mData.mSemiCircle.mEndAngle = endAngle;
	
// }

// Water::Bump* Water::AddBump(float x, float z, float range, float height)
// {
// 	Bump bump;
// 	bump.mRange = range;
// 	bump.mHeight = height;
// 	bump.mReferencePoint.Set(x, 0.0f, z);
// 	bump.mReferencePoint = bump.mReferencePoint * GetWorldToLocal();
// 	bump.mReferencePoint.y = 0.0f;
// //	bump.mReferencePoint.Set(bump.mReferencePoint.x * GetXScale(), 0.0f, bump.mReferencePoint.z * GetZScale());
// 	bool success = mBumps.Add(bump);
// 	ASSERT(success);
// 	return &mBumps[mBumps.Count()-1];
// }
