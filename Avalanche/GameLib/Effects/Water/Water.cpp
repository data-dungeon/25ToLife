//
// Author: TJC
// Purpose: An attempt at an explicit surface created by a series of
// equations creating a dynamic height field that is sampled at
// arbitrary positions.  Ok, not so arbitrary.. it should coincide
// with LOD requirements.
//
// Notes: Not all of the code for the Water class is in this module
// the module would have been well over 3000 lines.. and I hate
// files that long.  So, its split up as follows
//
// Water.cpp - general initialization & cleanup & misc functions
// WaterRender.cpp - render the darn thing
// WaterUpdate.cpp - update the water
// WaterPhysics.cpp - make nifty waves around the avatar
// WaterCollide.cpp - deal with collision checking for the water surface
// WaterSubdivision.cpp - determine how to tesselate the water surface
//
// Addtionally, there are a few other helper files that dont contain
// code for the Water class, but are related to the water that I might
// as well mention here...:
//
// WaterSprite - float an image on the water surface
// WaterEquation - modify the waters surface
// WaterAffector - control the waters tesselation
// WaterGrid - used internally to map data values across a grid on the waters surface
//

#include "Effects/EffectsPCH.h"




#ifdef PS2
#include <eekernel.h>
#endif

#define DISPLAY_STATE_ACCESS
#include "Display/Common/DisplayState.h"

// headers
#include "Effects/Water/Water.h"
#include "Effects/Water/WaterAffector.h"
#include "Effects/Water/WaterEquation.h"
#include "Effects/Water/WaterSprite.h"
#include "Effects/Water/WaterPool.h"

#ifdef DEBUG_WATER
#include "Display/Common/RenderMisc.h"
#endif

///////////////////////////////////////////////////////////////////////////////////////
// Water - Construct / Destruct / Init / Cleanup
///////////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1300
// for some reason, vc7 compiles an error into the water code that causes
// it to disappear, so turn off optimization for this module :(.
#pragma optimize( "", off )
#endif

#ifdef DEBUG_WATER
WaterDEBUG Water_Dbg;
#endif
extern float WaterFuzzFactor;

Water::Water(void) :
	mT(0.0f),
	mPsi(NULL),
	mPsiA(NULL),
	mPsiV(NULL),
#ifdef SUPPORT_GENERIC_RENDERER
	mRenderNodeArray(NULL),
	mNodeArray(NULL),
#endif
	mWaveSpeedSquared(NULL),
	mPhysicsEnabled(false),
	mSegClipper(0.0f, 0.0f, 1.0f, 1.0f),
	mMinSubDivDepth(0),
	mBumps(0, 4, false)
{
}

Water::~Water(void)
{
	CleanupPhysics();

	CleanupInstance();

#ifdef SUPPORT_GENERIC_RENDERER
	if(mNodeArray)
		memFree(mNodeArray);
	if(mRenderNodeArray)
		memFree(mRenderNodeArray);
#endif

	delete[] mLayerModes;
	
}

void Water::CleanupInstance(void)
{
	// TODO: remove instance...
	// TODO: free instance...
	memFree(mInstance);
}

bool Water::InitializeInstance(bool disableVis)
{
	// Allocate all the data
	mInstance = new CInstance;
	ASSERT(mInstance);

	// basic bounding info (for now init it to zeros.  It will get
	// adjusted as new equations get added to the water)
	BoundingBox.Min.Set(0.0f, -1.0f, 0.0f);
	BoundingBox.Max.Set(1.0f, 1.0f, 1.0f);
	BoundingSphere.Center.Clear();
	BoundingSphere.Radius = 1.0f;

	mInstance->InitFromDynamicObject( this );
	mInstance->SetName( "Water" );

	// Set some arbitrary flags
	mInstance->SetAlphaSort( true );
	mInstance->SetVertexColor32Bit( true );
	mInstance->SetZWriteDisabled( true );
	mInstance->SetReceiveShadows( false );
	mInstance->SetDynamicallyLit( false );
	mInstance->SetRenderBucket( 1 );
	
	// Default matrix
	mInstance->Matrix().Identity();
	mInstance->Matrix().CatScale(mXScale, 1.0f, mZScale);
	mInstance->Matrix().CatYRotation(mOrientation);
	mInstance->Matrix().PokeTranslation(mPosition);

	mLocalToWorldBasis.Identity();
	mLocalToWorldBasis.CatYRotation(mOrientation);
	mLocalToWorldBasis.PokeTranslation(mPosition);
	
	// we need the inverse of our local to world matrix...
	VERIFY(mInstance->Matrix().Invert(mWorldToLocal));
	VERIFY(mLocalToWorldBasis.Invert(mWorldToLocalBasis));

	// calculate initial bounding data
	mInstance->UpdateBoundingVolumesAccurate();

	// Add it to the scene, to visibility region if possible (water doesn't move)
	SceneMgr::AddInstance(mInstance);

	return true;
}

#define OVERLAP 1.0f

bool Water::InitFromParams(const WaterParams& params, int icellx, int icelly, WaterPool* pool)
{
	int i;

	WaterCellFlags cellFlags;

	float cellx, celly;
	cellx = icellx * OVERLAP;
	celly = icelly * OVERLAP;

	cellFlags = params.cellInfo[icellx][icelly].flags;

	// generate correct position...
	Vector3 offset;
	offset.Set(cellx * params.cellSizeX, 0.0f, celly * params.cellSizeZ);
	Graphics4x4 rotmtx;
	rotmtx.SetYRotation(params.angle);
	offset = offset * rotmtx;
	mPosition = params.pos + offset;
	mOrientation = params.angle;

	// these nodes should be stripped out before here..
	ASSERT(cellFlags.inUse);

	u32 flags = 0;

	if(cellFlags.clampN)
		flags |= LOCK_TOP_EDGE;
	if(cellFlags.clampS)
		flags |= LOCK_BOTTOM_EDGE;
	if(cellFlags.clampE)
		flags |= LOCK_RIGHT_EDGE;
	if(cellFlags.clampW)
		flags |= LOCK_LEFT_EDGE;
	if(cellFlags.scaleWaveOnDepth)
		flags |= SCALE_WAVES_ON_WATER_DEPTH;
	if(cellFlags.slowWavesOnDepth)
		flags |= SLOW_WAVES_ON_WATER_DEPTH;

	// any of the above flags require the depth field
	bool needsDepthField = true;//(flags != 0);

	int numLayers = 0;
	int layeridxs[MAX_LAYERS];
	for(i = 0; i < MAX_LAYERS; i++)
	{
		if(params.layers[i].mLayerType[0] != '\0')
		{
			if(strcmpi(params.layers[i].mLayerType, "DISTORT") == 0)
			{
				// its ok to call this a bunch of times...
//				mBackBufferCopyHandle = TextureMgr::RequireBackBufferCopy(WaterFuzzFactor);
//				if(!mBackBufferCopyHandle)
//				{
//					// we need to skip this layer because we cant back buffer copy..
//					continue;
//				}
			}
			layeridxs[numLayers++] = i;
		}
	}

	WaterLayerModes* layers = new WaterLayerModes[numLayers];
	for(i = 0; i < numLayers; i++)
	{
		layers[i].mFlags = WaterLayerModes::LAYER_TEXSET_INDEX;
		if(params.layers[layeridxs[i]].mFadeEdges)
			layers[i].mFlags |= WaterLayerModes::LAYER_FADE_EDGES;
		if(params.layers[layeridxs[i]].mFadeCenter)
			layers[i].mFlags |= WaterLayerModes::LAYER_FADE_CENTER;
		if(params.layers[layeridxs[i]].mFadeHeight)
			layers[i].mFlags |= WaterLayerModes::LAYER_FADE_HEIGHT;
		layers[i].mTextureIndex = params.layers[layeridxs[i]].mTextureLayer;
		layers[i].mScaleU = params.layers[layeridxs[i]].mScaleU;
		layers[i].mScaleV = params.layers[layeridxs[i]].mScaleV;
		layers[i].mFlowU = params.layers[layeridxs[i]].mFlowU;
		layers[i].mFlowV = params.layers[layeridxs[i]].mFlowV;
		layers[i].mVtxAdjParameter = params.layers[layeridxs[i]].mVtxAdjParameter;
		layers[i].mNormAdjParameter = params.layers[layeridxs[i]].mNormAdjParameter;
		layers[i].mWorldLight = params.layers[layeridxs[i]].mWorldLight;
		layers[i].mOpacityOverTime = params.layers[layeridxs[i]].mOpacityOverTime;
		layers[i].mOpacity = params.layers[layeridxs[i]].mOpacity;

		if (strcmpi(params.layers[layeridxs[i]].mMethod, "REPLACE") == 0)
			layers[i].mMethod = TS_METHOD_REPLACE;
		else if (strcmpi(params.layers[layeridxs[i]].mMethod, "MODULATE") == 0)
			layers[i].mMethod = TS_METHOD_MODULATE;
		else if (strcmpi(params.layers[layeridxs[i]].mMethod, "DECAL") == 0)
			layers[i].mMethod = TS_METHOD_DECAL;
		else if (strcmpi(params.layers[layeridxs[i]].mMethod, "CONSTANTALPHA") == 0)
			layers[i].mMethod = TS_METHOD_CONSTANTALPHA;
		else if (strcmpi(params.layers[layeridxs[i]].mMethod, "INTERPOLATE") == 0)
			layers[i].mMethod = TS_METHOD_INTERPOLATE;
		else if (strcmpi(params.layers[layeridxs[i]].mMethod, "DOTPRODUCT") == 0)
			layers[i].mMethod = TS_METHOD_DOTPRODUCT;
		else
			layers[i].mMethod = (unsigned int)-1;

		if (strcmpi(params.layers[layeridxs[i]].mBlendFunc, "DIFFUSE") == 0)
			layers[i].mBlendFunc = RS_BLENDFUNC_DIFFUSE;
		else if (strcmpi(params.layers[layeridxs[i]].mBlendFunc, "NONE") == 0)
			layers[i].mBlendFunc = RS_BLENDFUNC_NONE;
		else if (strcmpi(params.layers[layeridxs[i]].mBlendFunc, "SATURATE") == 0)
			layers[i].mBlendFunc = RS_BLENDFUNC_SATURATE;
		else if (strcmpi(params.layers[layeridxs[i]].mBlendFunc, "FIXEDALPHA") == 0)
			layers[i].mBlendFunc = RS_BLENDFUNC_FIXEDALPHA;
		else if (strcmpi(params.layers[layeridxs[i]].mBlendFunc, "ADDITIVE") == 0)
			layers[i].mBlendFunc = RS_BLENDFUNC_ADDITIVE;
		else if (strcmpi(params.layers[layeridxs[i]].mBlendFunc, "ADD") == 0)
			layers[i].mBlendFunc = RS_BLENDFUNC_ADD;
		else
			layers[i].mBlendFunc = RS_BLENDFUNC_DEFAULT;

		if (strcmpi(params.layers[layeridxs[i]].mLayerType, "SPECULAR") == 0)
			layers[i].mLayerType = WaterLayerModes::LAYER_SPECULAR;
		else if (strcmpi(params.layers[layeridxs[i]].mLayerType, "ENVIRONMENT") == 0)
			layers[i].mLayerType = WaterLayerModes::LAYER_ENVIRONMENT;
		else if (strcmpi(params.layers[layeridxs[i]].mLayerType, "DISTORT") == 0)
			layers[i].mLayerType = WaterLayerModes::LAYER_DISTORT;
		else
			layers[i].mLayerType = WaterLayerModes::LAYER_DECAL;

		if(params.layers[layeridxs[i]].mPointSample)
			layers[i].mFilter = TS_FILTER_POINTSAMPLE;
		else
			layers[i].mFilter = TS_FILTER_BILINEAR;
	}

	if(!Init(params.textureFile,
			  pool,
			  flags,
			  params.cellSizeX,
			  params.cellSizeZ,
			  mPosition,
			  mOrientation,
			  numLayers,
			  layers,
			  params.maxSubDivDepth,
			  params.minSubDivDepth,
			  params.subDivNodes,
			  (needsDepthField) ? params.dampenSubDivisions : 0,
			  (cellFlags.receiveShadows) ? params.shadowSubDivisions : 0,
			  (cellFlags.runPhysics) ? params.physicsSubDivisions : 0,
			  params.physicsWaveSpeed,
			  params.physicsElasticity,
			  params.disableVisReg))
		return false;

	// patch these up...
	mXOffset = (float)cellx;
	mZOffset = (float)celly;

	return true;
}

bool Water::Init(
	const char *textureFile,
	WaterPool* pool,
	u32 flags, 
	float xScale, 
	float zScale,
	const Vector3& position,
	float orientation,
	int numLayers,
	WaterLayerModes* layerModes,
	u8 maxSubDivDepth, 
	u8 minSubDivDepth,
	int subDivNodes, 
	int dampenSubDivisions, 
	int shadowSubDivisions,
	int physicsSubDivisions,
	float physicsWaveSpeed,
	float physicsElasticity,
	bool disableVis)
{
	mOrientation = orientation;
	mPosition = position;
	mPool = pool;

	mXOffset = 0.0f;
	mZOffset = 0.0f;

	mXScale = xScale;
	mZScale = zScale;
	mInvXScale = Math::Inverse(mXScale);
	mInvZScale = Math::Inverse(mZScale);

	if(!InitializeInstance(disableVis))
		return false;

	// copy the layer info...
	mLayerModes = layerModes;
	mNumLayers = numLayers;

	mMaxSubDivDepth = maxSubDivDepth;
	mMinSubDivDepth = minSubDivDepth;

#ifdef SUPPORT_GENERIC_RENDERER
	// must allow us at least 1 node as the
	// algortihm requires it
	ASSERT(subDivNodes >= 1);

	mNodes = subDivNodes;
	ASSERT(mNodes < 0xFF); // too many subdivisions?

	// determine how many verts we will need...
//	mVerts = 9 + mNodes * 4;
	mVerts = mNodes * (9 * 4);

	// allocate the node array
	MEM_SET_ALLOC_NAME("Water nodes");
	mNodeArray = (Node*)memAlloc(sizeof(Node) * mNodes);
	ASSERT(mNodeArray);

	// the node array for holding actual mesh vertices...
	MEM_SET_ALLOC_NAME("Water render nodes");
	mRenderNodeArray = (RendNode*)memAlloc(sizeof(RendNode) * mNodes);
	ASSERT(mRenderNodeArray);
#endif

#ifdef PS2
	// the ps2 microcode ONLY supports this subdivision resolution!
	// I probalby should linearly interpolate from a smaller mesh if
	// a smaller mesh is requested to avoid oversampling and not matching
	// the other platforms, but this is simple for now!
	dampenSubDivisions = 4;
	if(physicsSubDivisions > 0)
		physicsSubDivisions = 4;
#endif

	// allocate depth grid...
	MEM_SET_ALLOC_NAME("Water depth field");
	mDepthGrid.Init(dampenSubDivisions, sizeof(float), false, 0);
	InitDepthGrid();

	// allocate wave dampening grid...
	MEM_SET_ALLOC_NAME("Water dampen field");
	mDampeningGrid.Init(dampenSubDivisions, sizeof(u8), true, -1);

	// init the dampening field...
	if(flags & SCALE_WAVES_ON_WATER_DEPTH)
		DepthDampenField(0.0f, 0.5f);
	if(flags & (LOCK_LEFT_EDGE|LOCK_RIGHT_EDGE|LOCK_TOP_EDGE|LOCK_BOTTOM_EDGE))
		ClampEdges(flags);

	// load the texture set
	t_Handle hTexture = dblLoad(textureFile);
//	t_Handle hTexture = dblLoad("Objects\\Water.dbl");
	// if this assert files, the water cant find the dbl file that is specified in the .var file.
	ASSERT(hTexture != INVALID_HANDLE);
	if (hTexture == INVALID_HANDLE)
		return(false);

	// Load the thingie using the thingie
	TextureDatabase* pTextureDatabase = (TextureDatabase*)dblGetChunkData(DB_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, hTexture);
	if(!pTextureDatabase)
		return false;
	ASSERT_PTR(pTextureDatabase);
	mTextureArray = new TextureArray();
	ASSERT_PTR(mTextureArray);
	mTextureArray->AddEntireDatabase(pTextureDatabase);

	s32 layer = 0;
	// shadows never looked good anyway
	if(shadowSubDivisions != 0)
		LOCKUP();

#ifdef SUPPORT_GENERIC_RENDERER
	// texture pass flags
	u32 texFlags[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
// 	for(layer = 0; layer < mNumLayers; layer++)
// 	{
		texFlags[layer] = VtxBuffer::VERT_TEX_DIM2;
// 	}

	// init the vtx buffer

	u32 vtxflags = VtxBuffer::VERT_HAS_DIFFUSE;
	for(layer = 0; layer < mNumLayers; layer++)
	{
		if(!mVtxBuffer[layer].Init(mVerts, vtxflags, texFlags)) return false;
	}

	for(u32 i = 0; i < MAX_SPRITES; i++)
	{
		u32 texFlags[8] = { VtxBuffer::VERT_TEX_DIM2, 0, 0, 0, 0, 0, 0, 0 };
		mSpriteVtxBuffers[i].Init(mVerts, VtxBuffer::VERT_HAS_DIFFUSE, texFlags);
	}

	InitializeStripBuffers(MAX_SPRITES);
#endif

	if(InitializePhysics(physicsSubDivisions, physicsWaveSpeed, physicsElasticity))
	{
		EnablePhysics();
		if(flags & SLOW_WAVES_ON_WATER_DEPTH)
			DepthSlowWaves();
	}

	// move lights into local space...
	for(layer = 0; layer < mNumLayers; layer++)
	{
		if(mLayerModes[layer].mWorldLight.LengthSquared() > 0.0f)
		{
			// inv rotate & translate only... not scale...
			mLayerModes[layer].mLight.FastMultiply(mLayerModes[layer].mWorldLight, mWorldToLocalBasis);
			// normalize it...
			mLayerModes[layer].mLight.Normalize();
		}
	}

	return true;
}

void Water::InitializeStripBuffers(int numSprites)
{
#ifdef SUPPORT_GENERIC_RENDERER
	int layer;

	for(layer = 0; layer < mNumLayers; layer++)
	{
#ifdef WATER_USE_FANS
		if(!mStripBuffer[layer].Init(STRIP_BUFFER_IS_FAN))
#else
		if(!mStripBuffer[layer].Init())
#endif
		{
			ASSERT(false);
		}

		mStripBuffer[layer].SetTextureArray(mTextureArray, 0);
	}

	for(layer = 0; layer < mNumLayers; layer++)
	{
		// get the bind...
		Texture* texture;
#ifdef PS2
		if(mLayerModes[layer].mLayerType == WaterLayerModes::LAYER_DISTORT)
		{
			texture = TextureMgr::GetBackBuffer();
		}
		else
#endif
		{
			if(mLayerModes[layer].mFlags & WaterLayerModes::LAYER_TEX_BIND)
			{
				texture = mLayerModes[layer].mTexture;
			}
			else
			{
				texture = (*mTextureArray)[mLayerModes[layer].mTextureIndex];
			}
		}

		// set it into the strip buffer...
		mStripBuffer[layer].SetPassTexture(0,
													  texture,
													  (u8)mLayerModes[layer].mMethod,
													  TS_WRAP_REPEAT, TS_WRAP_REPEAT, 
													  (u8)mLayerModes[layer].mFilter,
													  (u8)mLayerModes[layer].mBlendFunc);
		mStripBuffer[layer].SetVtxBuffer(&mVtxBuffer[layer]);
	}


	for(int i = 0; i < numSprites; i++)
	{
#ifdef WATER_USE_FANS
		if(!mSpriteStripBuffers[i].Init(STRIP_BUFFER_IS_FAN))
#else
		if(!mSpriteStripBuffers[i].Init())
#endif
		{
			ASSERT(false);
		}

		// init the strip buffer
		mSpriteStripBuffers[i].SetTextureArray(mTextureArray, 0);
		//mSpriteStripBuffers[i].SetPassTexture(0, 0, TS_METHOD_MODULATE, TS_WRAP_CLAMP, TS_WRAP_CLAMP, TS_FILTER_BILINEAR, RS_BLENDFUNC_DEFAULT);
		mSpriteStripBuffers[i].SetVtxBuffer(&mSpriteVtxBuffers[i]);
	}
#endif // SUPPORT_GENERIC_RENDERER
}

void Water::DepthSlowWaves(void)
{
	// run through the depth information to scale the wave speeds down near the edges of the water...
}

void Water::DepthDampenField(float minDepth, float maxDepth)
{
	// make maxDepth the possible depth range...
	maxDepth -= minDepth;
	float invDepthRange = Math::Inverse(maxDepth);

	// run through the depth information to set the dampenfield to dampen water edges..
	for(int x = 0; x < mDampeningGrid.mGridSize; x++)
	{
		for(int z = 0; z < mDampeningGrid.mGridSize; z++)
		{
			u8 temp;
			float depth = mDepthGrid(x*mDampeningGrid.mInvGridSizeMinus1, z*mDampeningGrid.mInvGridSizeMinus1);
			depth -= minDepth;
			if(depth < 0)
				temp = 0;
			else if(depth > maxDepth)
				continue; // leave value be...
			else
				temp = (u8)(mDampeningGrid(x, z) * (depth * invDepthRange));

			// write it back out..
			mDampeningGrid(x, z) = temp;
		}
	}
}

void Water::ClampEdges(u32 flags)
{
	// lock the edges down...
	for(int x = 0; x < mDampeningGrid.mGridSize; x++)
	{
		if(flags & LOCK_LEFT_EDGE)
			mDampeningGrid(0, x) = 0;
		if(flags & LOCK_TOP_EDGE)
			mDampeningGrid(x, 0) = 0;
		if(flags & LOCK_RIGHT_EDGE)
			mDampeningGrid(mDampeningGrid.mGridSizeMinus1, x) = 0;
		if(flags & LOCK_BOTTOM_EDGE)
			mDampeningGrid(x, mDampeningGrid.mGridSizeMinus1) = 0;
	}
}

void Water::InitDepthGrid(void)
{
	// determine depth of water...
	// vertical line through grid point is tested.  2 nearest collisions are
	// kept, one above, and one below.

	// if the collision below is a frontface, 
	//      if the collision above is a frontface, the distance to down determines the depth..
	//      else if no collision above, the distance to down determines the depth..
	//      else, we are below ground... I think (depth = 0.0f)
	// else if there is no collision below
	//      if the collision above is a frontface, we are over nothing (depth = +INF)
	//		  else if the collision above is a backface, we are underground (depth = 0.0f)
	//      else we are free floating (no ceiling, no floor) (depth = +INF)
	// else if the collision below is backface
	//      if the collision above is a frontface, world is backwards! not sure what to do.. print warning (depth = 0.0f)
	//		  else if the collision above is a backface, we are underground (or above ceiling.. hard to tell) (depth = 0.0f)
	//      else we are above ceiling (depth = 0.0f)

	for(int x = 0; x < mDepthGrid.mGridSize; x++)
	{
		for(int z = 0; z < mDepthGrid.mGridSize; z++)
		{
			// determine position in world space of this grid intersection...
			Vector3 pos;

			// first position in local space...
			pos.Set(x * mDepthGrid.mInvGridSizeMinus1, 0.0f, z * mDepthGrid.mInvGridSizeMinus1);

			// now move to world space
			pos = pos * mInstance->Matrix();

			float depth;
			g_collisionSystem.ResetStack();
			CXZEntity entity;
			entity.SetPositiveInclude(WORLD_CATEGORY);
			entity.FindFloor(pos);
			Vector3 normal;
			if ((g_collisionSystem.Test(&entity) != NULL) &&
					CLineQuery::Normal(entity.Line(), normal))
			{
				// was it facing us?
				if(normal.y() >= 0.0f)
				{
					// distance to the hit...
					depth = pos.y() - entity.YVal();

					// backface...
					entity.FindCeiling(pos.y());
					if ((g_collisionSystem.Test(&entity) != NULL) &&
							CLineQuery::Normal(entity.Line(), normal) &&
							(normal.y() >= 0.0f))
						depth = 0.0f;
				}
				else
					// all backface results below us are 0.0f
					depth = 0.0f;
			}
			else
			{
				entity.FindCeiling(pos.y());
				if ((g_collisionSystem.Test(&entity) != NULL) &&
						CLineQuery::Normal(entity.Line(), normal) &&
						(normal.y() >= 0.0f))
					// backface above
					depth = 0.0f;
				else
					depth = Math::PositiveInfinity;
			}

			// we now have the depth, set it...
			mDepthGrid(x, z) = depth;
		}
	}
}


float Water::GetHeightLocal(float x, float z)
{
	float y = 0.0f;

#ifdef DEBUG_WATER
	if(!Water_Dbg.mDisableEquations)
#endif
	{
		// first the equations...
		int eqCount = mPool->Equations().Count();
		
		for(int i = 0; i < eqCount; i++)
		{
			y += mPool->Equations()[i]->Evaluate(mT, x + mXOffset, z+mZOffset);
		}
	}

	// add in noise
#ifdef DEBUG_WATER
	if(!Water_Dbg.mDisableNoise)
#endif
		y += mPool->EvaluateNoise(mT, x + mXOffset, z + mZOffset);
	
	// add in wakes...
#ifdef DEBUG_WATER
	if(!Water_Dbg.mDisableWakes)
#endif
		y += mPool->EvaluateWakes(mT, x + mXOffset, z + mZOffset);

#ifdef DEBUG_WATER
	if(!Water_Dbg.mDisableRings)
#endif
		y += mPool->EvaluateWaterRings(mT, x + mXOffset, z + mZOffset);

	// add in the physics component
#ifdef DEBUG_WATER
	if(mPhysicsEnabled && (!Water_Dbg.mDisablePhysics))
#else
	if(mPhysicsEnabled)
#endif
		y += PhysicsHeight(x, z);

	// apply dampening field...
#ifdef DEBUG_WATER
	if(!Water_Dbg.mDisableDampening)
#endif
		y *= DampenFieldValue(x, z);

	// now we add undampened effects...

	// add in surface height calculation...
#ifdef DEBUG_WATER
	if(!Water_Dbg.mDisableSurfaceHeight)
#endif
		y += mPool->CurrentSurfaceHeight();
	
	// done
#ifdef DEBUG_WATER
	if(!Water_Dbg.mDisableBumps)
#endif
		y += BumpHeight(x+mXOffset, z+mZOffset);
	
	return y;
}

void Water::CreateShadowTexture(int subdivisionLevel)
{
	s16 resolution = 1 << subdivisionLevel;
	u32 pixelcount = resolution * resolution * 2;

#ifdef PS2
	u32 textureblocks = pixelcount / VRAM_BYTES_PER_BLOCK;
	if(textureblocks < 1) textureblocks = 1;
#endif

	LOCKUP();
}
