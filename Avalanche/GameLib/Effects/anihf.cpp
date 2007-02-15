/////////////////////////////////////////////////////////////////////////////
// Animated height-field stuff
/////////////////////////////////////////////////////////////////////////////

/******************* includes ****************************/

#include "Effects/EffectsPCH.h"

#include "Effects/Water/WaterSprite.h"



#include "EngineHelper/timer.h"

const uint AnimatedHeightField::NO_DEBUG   = 0x0;
const uint AnimatedHeightField::NO_PHYSICS = 0x1;
const uint AnimatedHeightField::NO_RENDER  = 0x2;
const uint AnimatedHeightField::ALL_DEBUG  = 0x3;



/////////////////////////////////////////////////////////////////////////////
// We don't do stuff until the initialize
/////////////////////////////////////////////////////////////////////////////
AnimatedHeightField::AnimatedHeightField(void)
{
	node = NULL;
	instance = NULL;

	textureContext = NULL;
	textureLayers = 0;
	textureArray  = NULL;

	noGeoD = 0.0f;
	noTexD = 0.0f;
	nothingD = 0.0f;

	localNormalTable = NULL;

	wantEngineLighting = false;
	eyeBasedLighting = false;
	needNormals = false;
	vertexColorsAreNormals = false;


	/////////////////////////////
	// surface height
	/////////////////////////////
	mSurfaceHeight = mSurfaceHeightTarget = mSurfaceHeightVelocity = 0.0f;			//pab

	scriptName[0] = '\0';

	// init the maps
	for (int m = 0; m < ANIHF_MAXTEXTURES; m++)
		memset(&map[m], '\0', sizeof(Map));

	havePhysics = false;

	mStripBuffer.Init();

	debugState = NO_DEBUG;

	mPoolNumber = -1;			// init so normal Anihf pools work still  (PAB)
	mWaterLayersUsed = 0;		//
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
AnimatedHeightField::~AnimatedHeightField(void)
{
	ASSERT(instance);	//djr: if this hits it probably means the parameter init failed and is aborting

	// Make sure we get out of the renderer
	if (instance)
		SceneMgr::RemoveInstance(instance);

	// Clear the node list
	if (node)
		delete [] node;

	if (localNormalTable)
		delete [] localNormalTable;

	// Clear all the allocated thingies
	if ( instance && instance->VertexColorPtr() )
		memFree( instance->VertexColorPtr() );
	if (instance)
		delete instance;
	if (textureContext)
		memFree(textureContext);

	if (textureArray)
		delete textureArray;
}

/////////////////////////////////////////////////////////////////////////////
// Build defaults for a texture layer
/////////////////////////////////////////////////////////////////////////////
void AnimatedHeightField::DefaultTextureLayerInit(
TextureLayerInit &init,
int textureIndex)
{
	memset(&init, '\0', sizeof(TextureLayerInit));
	init.textureIndex = textureIndex;
	init.method = TS_METHOD_MODULATE;
	init.blend = RS_BLENDFUNC_DEFAULT;
	init.filter = TS_FILTER_BILINEAR;
	init.uWrap = 1.0f;
	init.vWrap = 1.0f;
	init.mapType = DECAL;
	init.light.Set(0.0f, 1.0f, 0.0f);
}

/////////////////////////////////////////////////////////////////////////////
// Setup the texturing for the height field
// baseColor provides the base color for the material
// if a texture file is provided, the layers are read from it
// if layers is > 0 layerInit must have an array of TextureLayerInit's
/////////////////////////////////////////////////////////////////////////////
bool AnimatedHeightField::SetMaterial(
const ts_bRGBA &baseColor,
char *textureFile,
int layers,
TextureLayerInit *layerInit)
{
	// Allocate all the data
	MEM_SET_ALLOC_NAME("AniHF texture");
	textureContext = (ts_FullTextureContext *)memAlloc(sizeof(ts_FullTextureContext));
	ASSERT(textureContext);

	// Clear the context
	memset(textureContext, '\0', sizeof(ts_FullTextureContext));
	textureLayers = 0;

	// Set a base color, even with no texture
	textureContext->BaseColor = baseColor;

	// No special texture mapping techniques
	eyeBasedLighting = false;

	// Did the specify a texture
	if (textureFile)
	{
		// Load the textures
		t_Handle hTexture = dblLoad(textureFile);
		if (hTexture == INVALID_HANDLE)
			return(false);

		// Build texture array from texture database
		textureArray = new TextureArray;
		TextureDatabase* textureDB = dblGetTextures( hTexture, NULL );
		for (int t = 0; t < textureDB->TextureCount(); t++)
			textureArray->AddTexture( textureDB->GetTexture(t) );

		// Set the bind number!!!!!!
		textureContext->TextureCount = layers;
		textureLayers = layers;

		for (int i = 0; i < layers; i++)
		{
			// All of your base are belong to us

			textureContext->TextureLayer[i].textureDatabase = textureDB;
			textureContext->TextureLayer[i].s16TextureIndex = layerInit[i].textureIndex;

			// Hack up a bunch of this shizit
			textureContext->TextureLayer[i].u8Method = layerInit[i].method;
			textureContext->TextureLayer[i].u8Blend = layerInit[i].blend;
			textureContext->TextureLayer[i].u8WrapU = TS_WRAP_REPEAT;
			textureContext->TextureLayer[i].u8WrapV = TS_WRAP_REPEAT;
			textureContext->TextureLayer[i].u8Filter = layerInit[i].filter;
			textureContext->TextureLayer[i].u8Flags = layerInit[i].flags;

			// clamp our sprite layers for effects
			if (i >= mWaterLayersUsed)
			{
				textureContext->TextureLayer[i].u8WrapU = TS_WRAP_CLAMP;
				textureContext->TextureLayer[i].u8WrapV = TS_WRAP_CLAMP;
			}

			// Save the wrap values
			map[i].uWrap = layerInit[i].uWrap;
			map[i].vWrap = layerInit[i].vWrap;

			// Save the mapping type
			map[i].type = layerInit[i].mapType;
			map[i].group = MappingGroup(map[i].type);

			// Do we have a eyeNormal map?
			if (map[i].group == GROUP_EYEBASEDLIGHTING)
			{
				eyeBasedLighting = true;
				needNormals = true;
				if (map[i].type != ENVIRONMENT)
				{
					map[i].light = layerInit[i].light;
					map[i].light.Normalize();
					map[i].worldLight = map[i].light;
				}
			}
			else if (map[i].group == GROUP_HEIGHT)
			{
				map[i].heightLow = layerInit[i].low;
				map[i].heightHigh = layerInit[i].high;
				map[i].halfInvHeightRange = 0.5f / (map[i].heightHigh - map[i].heightLow);
			}
			else if (map[i].type == TURBULENCE)
			{
				map[i].turbXFactor = layerInit[i].turbXFactor;
				map[i].turbYFactor = layerInit[i].turbYFactor;
			}
			else if (map[i].type == BUMPMAP)
			{
				vertexColorsAreNormals = true;
				needNormals = true;
				map[i].light = layerInit[i].light;
				map[i].light.Normalize();
				map[i].worldLight = map[i].light;
			}
		}
	}

	// We are out of here!
	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Setup the initial height-field geometry (use the orientation matrix if you don't want
// this in the xy plane)
// if flags is non-null it is an xPoints * yPoints array of flags
// no checks are made to see if this causes invalid tri-strips.
// returns false on failure
/////////////////////////////////////////////////////////////////////////////
bool AnimatedHeightField::BuildGeometry(
int pointsX,
int pointsY,
float sizeX,
float sizeY,
bool lighting,
bool lockEdges,
bool wantVertexColors,
bool isSeeThru,
ts_bRGBA *vertexColor)
{
	// THEY MUST HAVE THE TEXTURE SET UP!!!
	ASSERT(textureContext);
	if (!textureContext)
		return(false);

	// Duh, must define some triangles
	if (pointsX < 2 || pointsY < 2)
		return(false);

	// Engine based lighting is evil
	wantEngineLighting = lighting;
	if (wantEngineLighting)
		needNormals = true;

	// Setup the node array
	xVerts = pointsX;
	yVerts = pointsY;
	xGrid = sizeX / (float)(pointsX - 1);
	yGrid = sizeY / (float)(pointsY - 1);
	invXGrid = 1.0f / xGrid;
	invYGrid = 1.0f / yGrid;
	node = new AHFNode[xVerts * yVerts];

#if defined(GCN)
	// Must initialize to avoid strict FPU_CHECK of GCN
	for(int i_clear = 0; i_clear< (xVerts*yVerts); i_clear++)
	{
		node[i_clear].toEye.Clear();
	}
#endif

	if (!node)
		return(false);

	if (lockEdges)
	{
		// Auto-lock the edges
		for (int iy = 0; iy < yVerts; iy++)
		{
			AHFNode *n0 = Node(0, iy);
			AHFNode *n1 = Node(xVerts - 1, iy);
			n0->SetFlag(AHFNode::LOCKED);
			n1->SetFlag(AHFNode::LOCKED);
		}
		for (int ix = 1; ix < xVerts - 1; ix++)
		{
			AHFNode *n0 = Node(ix, 0);
			AHFNode *n1 = Node(ix, yVerts - 1);
			n0->SetFlag(AHFNode::LOCKED);
			n1->SetFlag(AHFNode::LOCKED);
		}
	}

	int i,j;
	u32 vtxFlags = VtxBuffer::VERT_HAS_DIFFUSE;
	u32 texFlags[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	if(textureLayers >= 1)
		texFlags[0] = VtxBuffer::VERT_TEX_DIM2;
	if(textureLayers >= 2)
		texFlags[1] = VtxBuffer::VERT_TEX_DIM2;
	if(textureLayers >= 3)
		texFlags[2] = VtxBuffer::VERT_TEX_DIM2;
	if(textureLayers >= 4)
		texFlags[3] = VtxBuffer::VERT_TEX_DIM2;
	if(textureLayers >= 5)
		texFlags[4] = VtxBuffer::VERT_TEX_DIM2;
	if(textureLayers >= 6)
		texFlags[5] = VtxBuffer::VERT_TEX_DIM2;
	if(textureLayers >= 7)
		texFlags[6] = VtxBuffer::VERT_TEX_DIM2;
	if(textureLayers >= 8)
		texFlags[7] = VtxBuffer::VERT_TEX_DIM2;
	ASSERT(textureLayers <= 8);
	
	mVtxBuffer.Init(yVerts*xVerts, vtxFlags, texFlags);
	mVtxBuffer.Lock();
	for(i = 0; i < textureLayers; i++)
	{
		ts_TextureLayer* l = &textureContext->TextureLayer[i];
		TextureDatabase* db = l->textureDatabase;
		Texture* tex = db->GetTexture(Math::Min(l->s16TextureIndex, db->TextureCount() - 1));
		mStripBuffer.SetPassTexture(i, tex, l->u8Method, l->u8WrapU, l->u8WrapV, l->u8Filter, l->u8Blend);
	}
	for(i = 0; i < xVerts; i++)
	{
		for(j = 0; j < yVerts; j++)
		{
			for(int k = 0; k < textureLayers; k++)
				mVtxBuffer.TexCoordv2(k, i + j * xVerts).Set(0.0f, 0.0f);
		}
	}
	mStripBuffer.SetVtxBuffer(&mVtxBuffer);
	
	u16* strip = new u16[xVerts*2];
	for(j = 0; j < yVerts-1; j++)
	{
		for(i = 0; i < xVerts; i++)
		{
			strip[i*2] = i + j * xVerts;
			strip[i*2 + 1] = i + (j+1) * xVerts;
		}
		mStripBuffer.AddStrip(strip, xVerts*2);
	}
	delete[] strip;

	// Build tri strips based on columns
	bool buildOK = true;
	for (int x = 0; x < xVerts - 1 && buildOK; x++)
	{
		if (!BuildColumnTriStrip(x, x + 1))
			buildOK = false;
	}

	// Did the build work
	if (!buildOK)
	{
		delete [] node;
		node = NULL;
		return(false);
	}

	// setup the instance stuff
	if (!InitializeModel(isSeeThru))
		return(false);

	// Do they want vertex colors?
	ts_bRGBA defaultColor = { 255, 255, 255, 255 };
	if (!vertexColor)
		vertexColor = &defaultColor;
	if (wantVertexColors || vertexColorsAreNormals)
		BuildVertexColors(*vertexColor);

	// Iniit some stuff in the nodes
	SetupNodes();

	// Setup the collisions
	collisions = true;
	float minX = Math::IncEpsilon(0.0f);
	float minY = Math::IncEpsilon(0.0f);
	float maxX = Math::DecEpsilon(xGrid * (float)(xVerts - 1));
	float maxY = Math::DecEpsilon(yGrid * (float)(yVerts - 1));

	clipper.SetExtents(minX, minY, maxX, maxY);
	aabb = (AABB *)&instance->BoundingBox();

    mVtxBuffer.UnLock();

	// Somehow all this code actually worked
	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Setup the phyics for wave propagation
/////////////////////////////////////////////////////////////////////////////
void AnimatedHeightField::SetupPhysics(
float waveSpeed,
float viscosity)
{
	// Turn on physics
	havePhysics = true;

	// What is the speed in grids per second?
	float speed = waveSpeed / xGrid;
	float speedSquared = speed * speed;

	// Setup the wave speed for all the nodes
	int x, y;
	for (y = 0; y < yVerts; y++)
		for (x = 0; x < xVerts; x++)
		{
			AHFNode *node = Node(x, y);
			if (!node->Locked())
				node->waveSpeedSquared = speedSquared;
		}

	// Save viscosity
	elasticity = 1.0f - Math::Clamp(viscosity, 0.0f, 1.0f);

	// Compute an inverse matrix for applying displacents
	if (instance)
		 instance->Matrix().Invert(worldToLocal);
}

/////////////////////////////////////////////////////////////////////////////
// nodeInfo is a pointsX by points Y (in X row order) that modifies the geometry
// Call this after BuildGeometry and SetupPhysics
/////////////////////////////////////////////////////////////////////////////
bool AnimatedHeightField::ModifyNodes(
NodeInfo *info)
{
	ASSERT(info);

	// Setup the nodes
	for (int y = 0; y < yVerts; y++)
		for (int x = 0; x < xVerts; x++)
		{
			AHFNode *n = Node(x, y);
			ModifyNode(x, y, *info);
			++info;
		}

	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Override a single node
/////////////////////////////////////////////////////////////////////////////
bool AnimatedHeightField::ModifyNode(
int x,
int y,
NodeInfo &info)
{
	// Duh
	if (x < 0 || x >= xVerts || y < 0 || y >= yVerts)
		return(false);

	// Alias the node
	AHFNode *n = Node(x, y);

	// Are they locking this node down?
	if (info.override & OVERRIDE_LOCKED)
	{
		if (!info.locked)
			n->ClearFlag(AHFNode::LOCKED);
		else
		{
			n->SetFlag(AHFNode::LOCKED);
			n->waveSpeedSquared = 0.0f;
		}
	}

	// Are the overiding the position?
	if (info.override & OVERRIDE_POSITION)
	{
		n->basePos = info.position;
		mVtxBuffer.Vertex(x + y * xVerts) = n->basePos;
	}

	// Wave speed?
	if (info.override & OVERRIDE_WAVESPEED)
		n->waveSpeedSquared = Math::Square(info.waveSpeed);

	// Color?
	if (info.override & OVERRIDE_COLOR)
	{
		union { ts_bRGBA c; u32 i; } col;
		col.c = info.color;
		mVtxBuffer.Diffuse(x + y * xVerts) = VtxBuffer::cvtRGBA(col.i);
	}

	// Are they shadowing this node?
	if (info.override & OVERRIDE_SHADOWED)
	{
		if (info.shadowed)
			n->SetFlag(AHFNode::SHADOWED);
		else
			n->ClearFlag(AHFNode::SHADOWED);
	}

	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// If you modify a lot of nodes positions, call this!!
/////////////////////////////////////////////////////////////////////////////
void AnimatedHeightField::RecomputeBounds(void)
{
	Profiler__Start(PROFILE_ANIHF);

	// recalculate model space bounding box
	BoundingBox__Clear(&BoundingBox);
	for (int i = 1; i < xVerts * yVerts; i++)
		BoundingBox__Include(&BoundingBox, node[i].basePos);

	// recompute model space bounding sphere to circumscribe bounding box. radius is distance from center to any corner
	BoundingSphere.Center = BoundingBox__GetCenter(&BoundingBox);
	BoundingSphere.Radius = (BoundingSphere.Center - BoundingBox.Min).Length();

	// reset instance's bounding sphere center for call to UpdateBoundingVolumesAccurate
	instance->BoundingSphere().Center = BoundingSphere.Center * instance->Matrix();

	// recalculate world-space bounds
	instance->UpdateBoundingVolumesAccurate();
	Profiler__Stop(PROFILE_ANIHF);
}

/////////////////////////////////////////////////////////////////////////////
// Position the object in the world
/////////////////////////////////////////////////////////////////////////////
void AnimatedHeightField::Place(
const Graphics4x4 &m)
{
	if (instance)
	{
		// Default matrix
		instance->Matrix() = m;

		// Compute an inverse matrix for applying displacents
		instance->Matrix().Invert(worldToLocal);

		// setup the bounding data
		instance->UpdateBoundingVolumes();

		// Setup the lights
		for (int i = 0; i < textureLayers; i++)
		{
			if (map[i].group == GROUP_EYEBASEDLIGHTING)
				map[i].light = map[i].worldLight.InvBasisTransform(m);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Setup culling
/////////////////////////////////////////////////////////////////////////////
void AnimatedHeightField::SetLODDistance(
float noGeometryAnimate,
float noTextureAnimate,
float doNothing)
{
	noGeoD = noGeometryAnimate;
	noTexD = noTextureAnimate;
	nothingD = doNothing;
}

/////////////////////////////////////////////////////////////////////////////
// Setup the noise parameters for Z
/////////////////////////////////////////////////////////////////////////////
void AnimatedHeightField::SetWave(
float waveFreq,
float waveAmp,
SigGen::TYPE type)
{
	waveZ.Setup(type, waveFreq, waveAmp);
}

/////////////////////////////////////////////////////////////////////////////
// Setup the XY positional variance
/////////////////////////////////////////////////////////////////////////////
void AnimatedHeightField::SetXYVariance(
float xFreq,
float xAmp,
SigGen::TYPE xType,
float yFreq,
float yAmp,
SigGen::TYPE yType)
{
	waveX.Setup(xType, xFreq, xAmp);
	waveY.Setup(yType, yFreq, yAmp);
}

/////////////////////////////////////////////////////////////////////////////
// Setup uv variance for a texture set
/////////////////////////////////////////////////////////////////////////////
void AnimatedHeightField::SetUVVariance(
int uvSet,
float uFreq,
float uAmp,
SigGen::TYPE uType,
float vFreq,
float vAmp,
SigGen::TYPE vType)
{
	ASSERT(uvSet >= 0 && uvSet < ANIHF_MAXTEXTURES);

	map[uvSet].waveU.Setup(uType, uFreq, uAmp);
	map[uvSet].waveV.Setup(vType, vFreq, vAmp);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AnimatedHeightField::SetUVFlow(
int uvSet,
float uFlow,
float vFlow)
{
	ASSERT(uvSet >= 0 && uvSet < ANIHF_MAXTEXTURES);

	map[uvSet].flowU = uFlow;
	map[uvSet].flowV = vFlow;
}

/////////////////////////////////////////////////////////////////////////////
// Apply a force
/////////////////////////////////////////////////////////////////////////////
void AnimatedHeightField::ApplyForce(
Vector3CRef point,
float force)
{
	// Duh
	if (instance == NULL || !havePhysics)
		return;

	// Are they even inside of us?
	AABB &aabb = (AABB &)instance->BoundingBox();
	if (!aabb.PointIn(point))
		return;

	// Put the point into model space
	Vector3 modelPoint = point * worldToLocal;

	// Apply the force
	ApplyDisplacement(modelPoint, force);
}

/////////////////////////////////////////////////////////////////////////////
// Test function to doink the hf
/////////////////////////////////////////////////////////////////////////////
void AnimatedHeightField::Doink(
float force)
{
	Vector3 modelPoint;
	modelPoint.Set((float)(xVerts / 2) * xGrid,
						(float)(yVerts / 2) * yGrid,
						0.0f);

	ApplyDisplacement(modelPoint, force);
}

/////////////////////////////////////////////////////////////////////////////
// Test function to doink the hf
/////////////////////////////////////////////////////////////////////////////
void AnimatedHeightField::Doink(
Vector3 &point,
float force)
{
	Vector3 modelPoint = point * worldToLocal;
	ApplyDisplacement(modelPoint, force);
}

/////////////////////////////////////////////////////////////////////////////
// Update the animation
/////////////////////////////////////////////////////////////////////////////
void AnimatedHeightField::Update(float deltaTime, Graphics4x4 &camera)
{
	/* just a reality check that we're catching all meaningful routines in correct profiler slot. */
	/* Profiler__CurrentSlot returns PROFILE_INVALID_SLOT is profiling is currently off */

//	ASSERTS((Profiler__CurrentSlot() == PROFILE_ANIHF) || (Profiler__CurrentSlot() == PROFILE_INVALID_SLOT), "Calling AniHF::Update outside of AniHFMgr::Update");


	// Any work to do?
	if (!node || (debugState & NO_PHYSICS) || !instance->RenderedLastFrame())
		return;

	// start of anihf replacement for Tyler's water
	#ifdef USE_ANIHF_WATER

	#ifdef	DEBUG
	// draw our bounding box (PAB)
	gfxDrawBox( &instance->BoundingBox );
	Graphics4x4	M;
	M.Identity();
	DrawUtility::Box(instance->BoundingBox.Min, instance->BoundingBox.Max, M, DrawUtility::RED);
	#endif


	// Add in movement for the Water.. height..
	// it seems that adjusting the OBB isnt really necessary for ANIHF
	// since the box is huge and doesnt affect the collisions with the water
	mDeltaT = deltaTime;
	mT += deltaTime;

	// Get matrix for geom (PAB)
	Graphics4x4 m;						
	m = instance->Matrix();		
	float geom_y = m.ty;							
//pab	float bbox_y = instance->BoundingBox.Max.Y(); //removed.. seems unnecessary

	// update the surface height...
	if(mSurfaceHeight != mSurfaceHeightTarget)
	{
		if(mSurfaceHeightTarget < mSurfaceHeight)
		{
			mSurfaceHeight -= mSurfaceHeightVelocity * mDeltaT;

			// update the geom position
			geom_y -= mSurfaceHeightVelocity * mDeltaT;		

			// update bbox
//pab			instance->BoundingBox.Max.Y(bbox_y - mSurfaceHeightVelocity * mDeltaT );

			if(mSurfaceHeight < mSurfaceHeightTarget)
				mSurfaceHeight = mSurfaceHeightTarget;
		}
		else
		{
			mSurfaceHeight += mSurfaceHeightVelocity * mDeltaT;

			// update the geom position
			geom_y += mSurfaceHeightVelocity * mDeltaT;			//pab

			// update bbox
//pab			instance->BoundingBox.Max.Y(bbox_y + mSurfaceHeightVelocity * mDeltaT );

			if(mSurfaceHeight > mSurfaceHeightTarget)
				mSurfaceHeight = mSurfaceHeightTarget;
		}
	}

	// plug back in the new y for water height
	m.ty = geom_y;			
	instance->Matrix() = m;		// set the matrix back


#if 0
	// update layer opacities...
	WaterLayerModes* layer = mLayerModes;
	for(int i = 0; i < mNumLayers; i++, layer++)
	{
		if(layer->mOpacityOverTime != 0.0f)
		{
			layer->mOpacity += Math::Inverse(layer->mOpacityOverTime) * deltaTime;
			if(layer->mOpacity < 0.0f)
			{
				layer->mOpacity = -layer->mOpacity;
				layer->mOpacityOverTime = -layer->mOpacityOverTime;
			}
			else if(layer->mOpacity > 1.0f)
			{
				layer->mOpacity = 2.0f - layer->mOpacity;
				layer->mOpacityOverTime = -layer->mOpacityOverTime;
			}
		}
	}
#endif

	#endif		// USE_ANIHF_WATER

	// What is the camera position in world coordinates
	Vector3 temp;
	temp = camera.GetTranslation();
	Vector3 worldCameraPos = temp.InvBasisTransform(camera);
	worldCameraPos.Negate();

	// What should we do?
	Vector3 myPos;
	myPos = instance->Matrix().GetTranslation();
	float d = (myPos - worldCameraPos).Length() - instance->BoundingSphere().Radius;
//	if (d > nothingD)
//		return;

#ifdef PS2
	// seutp LOD flags
	bool animateGeometry = (d < noGeoD);
	bool animateTextures = (d < noTexD);
#else
	bool animateGeometry = 1;		// force to always update for now.. PAB
	bool animateTextures = 1;		// for some reason this isnt working right on GCN otherwise
#endif

	if (!animateGeometry && !animateTextures)
		return;

	// What is the camera pos in model space?
	Vector3 modelPos;
	modelPos = instance->Matrix().GetTranslation();
	Vector3 cameraPos = (worldCameraPos - modelPos).InvBasisTransform(instance->Matrix());

	// setup time stuff
	float t = g_timer.GetStartSec();
	float deltaT = g_timer.GetFrameSec();

	// Keep track if u's or v's are hosed
	bool uOverflow[ANIHF_MAXTEXTURES];
	bool vOverflow[ANIHF_MAXTEXTURES];
	bool uUnderflow[ANIHF_MAXTEXTURES];
	bool vUnderflow[ANIHF_MAXTEXTURES];
	for (int overflow = 0; overflow < textureLayers; overflow++)
	{
		uOverflow[overflow] = (map[overflow].flowU != 0.0f);
		vOverflow[overflow] = (map[overflow].flowV != 0.0f);
		uUnderflow[overflow] = (map[overflow].flowU != 0.0f);
		vUnderflow[overflow] = (map[overflow].flowV != 0.0f);
	}

	// Run physics?
	if (animateGeometry && havePhysics)
		RunPhysics();

    mVtxBuffer.Lock();

	// Step through each element, updating
	int ix, iy;
	for (iy = 0; iy < yVerts; iy++)
		for (ix = 0; ix < xVerts; ix++)
		{
			// Get our node pointer
			AHFNode *node = Node(ix, iy);

			// How far are we from the camera?
			float eyeSquared = (node->basePos - cameraPos).LengthSquared();

			// Setup the toEye vector once per node
			if (animateGeometry && eyeBasedLighting)
				node->toEye = (cameraPos - mVtxBuffer.Vertex(ix + iy * xVerts)).Normalized();

			// Do the motion on the geometry
			if (animateGeometry && !node->Locked())
			{
				Vector3 newP = node->basePos;

				// Apply function generators
				if (waveX.Ready())
					newP.x(newP.x() + waveX.Generate(t, node->waveOmega.x()));
				if (waveY.Ready())
					newP.y(newP.y() + waveY.Generate(t, node->waveOmega.y()));
				if (waveZ.Ready())
					newP.z(newP.z() + waveZ.Generate(t, node->waveOmega.z()));

				// Add in the physics computation
				newP.z(newP.z() + node->psi);

				// Update the point
				mVtxBuffer.Vertex(ix + iy * xVerts) = newP;
			}

			// Apply texture ops
			if (animateTextures)
			{
				for (int layer = 0; layer < textureLayers; layer++)
				{

					// Do main layers

					// Apply flow
					if (map[layer].flowU != 0.0f)
					{
						node->baseuv[layer].u += map[layer].flowU * deltaT;
						if (node->baseuv[layer].u <= map[layer].uWrap*-0.5f + 1.0f)
							uOverflow[layer] = false;
						if (node->baseuv[layer].u >= map[layer].uWrap*0.5f - 1.0f)
							uUnderflow[layer] = false;
					}
					if (map[layer].flowV != 0.0f)
					{
						node->baseuv[layer].v += map[layer].flowV * deltaT;
						if (node->baseuv[layer].v <= map[layer].vWrap*-0.5f + 1.0f)
							vOverflow[layer] = false;
						if (node->baseuv[layer].v >= map[layer].vWrap*0.5f - 1.0f)
							vUnderflow[layer] = false;
					}

					// Get the base
					UVCOORD_TYPE uv = node->baseuv[layer];

					// Is this a special mode?
					if (map[layer].type == SPECULAR)
						ComputeSpecularUV(ix, iy, cameraPos, map[layer], uv);
					else if (map[layer].type == ENVIRONMENT)
						ComputeEnvironmentUV(*node, cameraPos, map[layer], uv);
					else if (map[layer].type == TURBULENCE)
						ComputeTurbulenceUV(*node, map[layer], uv);
					else if (map[layer].group == GROUP_HEIGHT)
						ComputeHeightUV(ix, iy, *node, map[layer], uv);

					// Apply uv noise
					if (map[layer].waveU.Ready() || map[layer].waveV.Ready())
					{
						if (map[layer].waveU.Ready())
							uv.u += map[layer].waveU.Generate(t, node->uvOmega[layer].x());
						if (map[layer].waveV.Ready())
							uv.v += map[layer].waveV.Generate(t, node->uvOmega[layer].y());
					}

					// Set it!
					mVtxBuffer.TexCoordv2(layer, ix + iy * xVerts).Set(uv.u, uv.v);
					node->SetUV(layer, uv);
				}
			}
		}

	// Did any of our textures overflow?
	if(animateTextures)
	{
		for (int check = 0; check < textureLayers; check++)
		{
			if (uUnderflow[check])
				FixUUnderflow(check);
			if (vUnderflow[check])
				FixVUnderflow(check);
			if (uOverflow[check])
				FixUOverflow(check);
			if (vOverflow[check])
				FixVOverflow(check);
		}
	}

	// Compute the normals now that we are done
	if (animateGeometry && needNormals)
	{
		for (iy = 0; iy < yVerts; iy++)
			for (ix = 0; ix < xVerts; ix++)
			{
				// Get our node pointer
				AHFNode *node = Node(ix, iy);

				if (!node->Locked())
				{
					Vector3 normal = ComputeNormal(ix, iy);
					node->SetNormal(normal);
					if (vertexColorsAreNormals)
						node->SetColorNormal(normal);
				}
			}
	}

    mVtxBuffer.UnLock();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Vector3 AnimatedHeightField::PointInWorld(
int x,
int y)
{
	// Off the map?
	if (x < 0 || x >= xVerts || y < 0 || y >= yVerts)
	{
		Vector3 poo;
		poo.Clear();
		return(poo);
	}

	// Translate it into the work
	AHFNode *node = Node(x, y);
	return(node->basePos * instance->Matrix());
}

/////////////////////////////////////////////////////////////////////////////
// Collide a segment with all the height fields
// Returns false if no collision
/////////////////////////////////////////////////////////////////////////////
bool AnimatedHeightField::Test(
Segment &seg,
AnimatedHeightField::Collision &collision)
{
	/* just a reality check that we're catching all meaningful routines in correct profiler slot. */
	/* Profiler__CurrentSlot returns PROFILE_INVALID_SLOT is profiling is currently off */

//	ASSERTS((Profiler__CurrentSlot() == PROFILE_ANIHF) || (Profiler__CurrentSlot() == PROFILE_INVALID_SLOT), "Calling AniHF::Test outside of AniHFMgr::Test");

	// Is it in the bounding box?
	if (!aabb->SegmentCrosses(seg.start, seg.end))
		return(false);

	// Put the segment into our local space
	Segment local;
	local.start = seg.start * worldToLocal;
	local.end = seg.end * worldToLocal;

	// Clipper works in xz plane
	float t = local.start.y();
	local.start.y(local.start.z());
	local.start.z(t);
	t = local.end.y();
	local.end.y(local.end.z());
	local.end.z(t);

	// Setup a clipper & clip the incoming segment
	Segment clipped;
	if (!clipper.Clip(local.start, local.end, clipped.start, clipped.end))
		return(false);

	// Clipper works in xz plane
	t = clipped.start.y();
	clipped.start.y(clipped.start.z());
	clipped.start.z(t);
	t = clipped.end.y();
	clipped.end.y(clipped.end.z());
	clipped.end.z(t);

	// Compute the extents
	if (clipped.start.x() > clipped.end.x())
	{
		t = clipped.start.x();
		clipped.start.x(clipped.end.x());
		clipped.end.x(clipped.start.x());
	}
	if (clipped.start.y() > clipped.end.y())
	{
		t = clipped.start.y();
		clipped.start.y(clipped.end.y());
		clipped.end.y(clipped.start.y());
	}

	int startx = (int)(clipped.start.x() * invXGrid);
	int endx = (int)(clipped.end.x() * invXGrid);
	int starty = (int)(clipped.start.y() * invYGrid);
	int endy = (int)(clipped.end.y() * invYGrid);

#if 1  // Create a buffer of quads
	if (startx > 0)
		startx--;
	if (endx < xVerts - 2)
		endx++;
	if (starty > 0)
		starty--;
	if (endy < yVerts - 2)
		endy++;
#endif

	// Init the collision
	collision.d = 10000.0f;
	collision.hf = NULL;

	mVtxBuffer.Lock();

	// Test triangles
	for (int y = starty; y <= endy; y++)
		for (int x = startx; x <= endx; x++)
		{
			// Should we terminate on first hit?
			if (TestQuad(clipped, x, y, collision))
				collision.hf = this;
		}

#if 0  // A place to recheck in the debugger
	if (!collision.hf)
	{
		for (int y = starty; y <= endy; y++)
		{
			for (int x = startx; x <= endx; x++)
			{
				TestQuad(clipped, x, y, collision);
			}
		}
		collision.hf = NULL;
	}
#endif

	mVtxBuffer.UnLock();

	// Did we get a hit?
	if (!collision.hf)
		return(false);

	// Convert quantities back into world space
	collision.intersection = collision.intersection * instance->Matrix();
	collision.normal = collision.normal.BasisTransform(instance->Matrix());

	return(true);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool AnimatedHeightField::TestQuad(
Segment &seg,
int x,
int y,
Collision &collision)
{
	// some asserts for range
	ASSERT(x >= 0);
	ASSERT(x < xVerts - 1);
	ASSERT(y >= 0);
	ASSERT(y < yVerts - 1);

	// Get the points
	Vector3Packed *v[4];
	v[0] = &mVtxBuffer.Vertex(x + y * xVerts);
	v[1] = &mVtxBuffer.Vertex(x+1 + y * xVerts);
	v[2] = &mVtxBuffer.Vertex(x + (y+1) * xVerts);
	v[3] = &mVtxBuffer.Vertex(x+1 + (y+1) * xVerts);

	return(TestTriangle(seg, *v[0], *v[1], *v[2], collision) || 
				TestTriangle(seg, *v[1], *v[2], *v[3], collision));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool AnimatedHeightField::TestTriangle(
Segment &seg,
Vector3Packed& a,
Vector3Packed& b,
Vector3Packed& c,
Collision &collision)
{
	// dammit, we have to compute normals...
	Vector3 n = TriangleTest::Normal(a, b, c);

	// Compute the plane intersections
	Vector3 i;
	float t;
	if (seg.TrueIntersectWithPlane(a, n, i, t) && 
			t < collision.d && 
			TriangleTest::PointInsideXY(i, a, b, c))
	{
		collision.intersection = i;
		collision.normal = n;
		collision.d = t;
		return(true);
	}

	return(false);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool AnimatedHeightField::GetWaveSpeed(
Vector3CRef point,
float &waveSpeed)
{
	return(false);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool AnimatedHeightField::GetMaterial(
Vector3CRef point,
int &material)
{
	return(false);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool AnimatedHeightField::GetVelocity(
Vector3CRef point,
Vector3 &velocity)
{
	return(false);
}

/////////////////////////////////////////////////////////////////////////////
// Return the node pointer for an x,y
/////////////////////////////////////////////////////////////////////////////
inline AHFNode *AnimatedHeightField::Node(
int x,
int y)
{
	return(&node[x + y * xVerts]);
}

/////////////////////////////////////////////////////////////////////////////
// Return the position of a node
/////////////////////////////////////////////////////////////////////////////
inline Vector4 AnimatedHeightField::NodePos(
int x,
int y)
{
	Vector4 p;

	p.x((float)x * xGrid);
	p.y((float)y * yGrid);
	p.z(0.0f);
	p.w(0.0f);

	return(p);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
inline Vector3 AnimatedHeightField::SafeNormal(
int x,
int y)
{
	static Vector3 nil( 0.0f, 0.0f, 0.0f );
	if (x < 0 || x >= xVerts || y < 0 || y >= yVerts)
		return(nil);
	return(*node[x + y * xVerts].GetNormal());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
inline Vector3Packed *AnimatedHeightField::SafePoint(
int x,
int y)
{
	// of edge
	if (x < 0 || x >= xVerts || y < 0 || y >= yVerts)
		return(NULL);
	return &mVtxBuffer.Vertex(x + y * xVerts);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
inline UVCOORD_TYPE AnimatedHeightField::GetUV(
int layer,
int x,
int y)
{
	UVCOORD_TYPE uv;

	uv.u = (float)x / (float)(xVerts - 1) * map[layer].uWrap - map[layer].uWrap * 0.5f;
	uv.v = (float)y / (float)(yVerts - 1) * map[layer].vWrap - map[layer].vWrap * 0.5f;

	return(uv);
}

/////////////////////////////////////////////////////////////////////////////
// Build a tri-strip out of a column of the HF, there may be hidden nodes
/////////////////////////////////////////////////////////////////////////////
bool AnimatedHeightField::BuildColumnTriStrip(
int x0,
int x1)
{
	Vector3 up( 0.0f, 0.0f, 1.0f );

	// Build it in column order
	int index = 0;
	for (int row = 0; row < yVerts; row++)
	{
		// Get the node pointers
		AHFNode *node0 = Node(x0, row);
		AHFNode *node1 = Node(x1, row);

		// Generate the vertices
		// Stick them in the tri strip
		Vector4 point = NodePos(x0, row);
		mVtxBuffer.Vertex(x0 + row * xVerts) = point;

		point = NodePos(x1, row);
		mVtxBuffer.Vertex(x1 + row * xVerts) = point;

		// Stick in the normal
		if (wantEngineLighting)
		{
			node0->AssociateNormal(&mVtxBuffer.Normal(x0 + row * xVerts));
			node1->AssociateNormal(&mVtxBuffer.Normal(x1 + row * xVerts));
			node0->SetNormal(up);
			node1->SetNormal(up);
		}

		// Next set of indices
		index += 2;
	}

	// We are done, create the model
	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Initialize all the instance shizit
/////////////////////////////////////////////////////////////////////////////
bool AnimatedHeightField::InitializeModel(bool isSeeThru)
{
	// Allocate all the data
	instance = new CInstance;
	ASSERT( instance );

	// setup the bounding data
	BoundingBox.Min.Set(0.0f, 0.0f, -1.0f);
	BoundingBox.Max.Set((float)xVerts * xGrid,
							  (float)yVerts * yGrid,
							  1.0f);
	BoundingSphere.Center = BoundingBox__GetCenter(&BoundingBox);
	float radius = Math::Max((float)xVerts * xGrid, (float)yVerts * yGrid);
	BoundingSphere.Radius = radius;

	// Make the instance
	instance->InitFromDynamicObject( this );

	// Set some arbitrary flags if this is an alpha'd height field (eg water)
	instance->SetVertexColor32Bit( true );

	//djr:I don't know why, but on non-seethru anihf's, this flag needs to be set or else the lighting goes full bright from different camera distance/angles
	//djr: so set if for both seethru and non-seethru anihf's
	instance->SetRenderBucket( INSTANCE_ARTIST_RENDER_BUCKET_MAX + 1 );

	if (isSeeThru)
	{
		instance->SetAlphaSort( true );
		instance->SetZWriteDisabled( true );
		instance->SetReceiveShadows( false );
		instance->SetRenderBucket( 1 );
	}

	// Make sure to disable lighting
	instance->SetDynamicallyLit( wantEngineLighting );

	// Default matrix
	instance->Matrix().Identity();

	// calculate initial bounding data
	instance->UpdateBoundingVolumesAccurate();

	// We're out of here!
	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Build the vertex colors
/////////////////////////////////////////////////////////////////////////////
void AnimatedHeightField::BuildVertexColors(
ts_bRGBA &c)
{
	// Compute how large this will be
	int stripSize = Align_RoundTo16((2 * yVerts) * sizeof(ts_bRGBA));
	int totalSize = stripSize * xVerts;

	union { ts_bRGBA c; u32 i; } col;
	col.c = c;

	// Init to default colors
	for (int y = 0; y < yVerts; y++)
	{
		for (int x = 0; x < xVerts; x++)
		{
			AHFNode *node = Node(x, y);
			mVtxBuffer.Diffuse(x + y * xVerts) = VtxBuffer::cvtRGBA(col.i);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Iniit some stuff in the nodes
/////////////////////////////////////////////////////////////////////////////
void AnimatedHeightField::SetupNodes(void)
{
	// Default normal
	Vector3 defaultNormal( 0.0f, 0.0f, 1.0f );

	// Do we have local normals?
	int onNormal = 0;
	if (needNormals && !wantEngineLighting)
	{
		localNormalTable = new Vector3Packed[xVerts * yVerts];
	}

	// Step through each element
	for (int iy = 0; iy < yVerts; iy++)
		for (int ix = 0; ix < xVerts; ix++)
		{
			// Get our node pointer
			AHFNode *node = Node(ix, iy);

			// Setup the point
			Vector4 point = NodePos(ix, iy);
			node->basePos = (Vector3 &)point;

			// setup omega
			node->waveOmega.Set(RandUnit() * Math::TwoPi,
									  RandUnit() * Math::TwoPi,
									  RandUnit() * Math::TwoPi);

			// Clear physics stuff
			node->psi = 0.0f;
			node->psiV = 0.0f;
			node->psiA = 0.0f;
			node->waveSpeedSquared = 0.0f;

			// setup base uv
			for (int layer = 0; layer < textureLayers; layer++)
			{
				node->baseuv[layer] = GetUV(layer, ix, iy);
				node->uvOmega[layer].x( RandUnit() * Math::TwoPi );
				node->uvOmega[layer].y( RandUnit() * Math::TwoPi );
			}

			// Do we need normals for special mapping
			if (needNormals && !wantEngineLighting)
			{
				node->AssociateNormal(&localNormalTable[onNormal++]);
				node->SetNormal(defaultNormal);
			}
		}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
AnimatedHeightField::MAPGROUP AnimatedHeightField::MappingGroup(
MAPTYPE type)
{
	switch (type)
	{
		case SPECULAR:
		case ENVIRONMENT:
			return(GROUP_EYEBASEDLIGHTING);

		case HEIGHT:
		case NEGHEIGHT:
		case ABSHEIGHT:
			return(GROUP_HEIGHT);
	};

	return(GROUP_DECAL);
}

/////////////////////////////////////////////////////////////////////////////
// Compute a normal for a node
/////////////////////////////////////////////////////////////////////////////
Vector3 AnimatedHeightField::ComputeNormal(
int x,
int y)
{
	// The origin
	Vector3Packed *o = &mVtxBuffer.Vertex(x + y * xVerts);

	// Total sum
	Vector3 sum;
	sum.Clear();

	// The neighbors
#define PHATTY_NORMAL
#ifdef PHATTY_NORMAL
	Vector3Packed *e[8];
	e[0] = SafePoint(x + 1, y);
	e[1] = SafePoint(x + 1, y + 1);
	e[2] = SafePoint(x, y + 1);
	e[3] = SafePoint(x - 1, y + 1);
	e[4] = SafePoint(x - 1, y);
	e[5] = SafePoint(x - 1, y - 1);
	e[6] = SafePoint(x, y - 1);
	e[7] = SafePoint(x + 1, y - 1);

	// Point to the neighbors
	Vector3 en[8];
	if (e[0]) en[0] = *e[0] - *o;
	if (e[1]) en[1] = *e[1] - *o;
	if (e[2]) en[2] = *e[2] - *o;
	if (e[3]) en[3] = *e[3] - *o;
	if (e[4]) en[4] = *e[4] - *o;
	if (e[5]) en[5] = *e[5] - *o;
	if (e[6]) en[6] = *e[6] - *o;
	if (e[7]) en[7] = *e[7] - *o;

	// Sum cross products
	if (e[0] && e[1]) sum += Vector3::Cross(en[0], en[1]);
	if (e[1] && e[2]) sum += Vector3::Cross(en[1], en[2]);
	if (e[2] && e[3]) sum += Vector3::Cross(en[2], en[3]);
	if (e[3] && e[4]) sum += Vector3::Cross(en[3], en[4]);
	if (e[4] && e[5]) sum += Vector3::Cross(en[4], en[5]);
	if (e[5] && e[6]) sum += Vector3::Cross(en[5], en[6]);
	if (e[6] && e[7]) sum += Vector3::Cross(en[6], en[7]);
	if (e[7] && e[0]) sum += Vector3::Cross(en[7], en[0]);

#else
	Vector3Packed *e[4];
	e[0] = SafePoint(x + 1, y);
	e[1] = SafePoint(x, y + 1);
	e[2] = SafePoint(x - 1, y);
	e[3] = SafePoint(x, y - 1);

	// Point to the neighbors
	Vector3 en[4];
	if (e[0]) en[0] = *e[0] - *o;
	if (e[1]) en[1] = *e[1] - *o;
	if (e[2]) en[2] = *e[2] - *o;
	if (e[3]) en[3] = *e[3] - *o;

	// Sum cross products
	if (e[0] && e[1]) sum += Vector3::Cross(en[0], en[1]);
	if (e[1] && e[2]) sum += Vector3::Cross(en[1], en[2]);
	if (e[2] && e[3]) sum += Vector3::Cross(en[2], en[3]);
	if (e[3] && e[0]) sum += Vector3::Cross(en[3], en[0]);
#endif

	// Normalize and set it
	sum.Normalize();
	return(sum);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AnimatedHeightField::FixUOverflow(
int layer)
{
	for (int iy = 0; iy < yVerts; iy++)
		for (int ix = 0; ix < xVerts; ix++)
		{
			// Get our node pointer
			AHFNode *node = Node(ix, iy);
			node->baseuv[layer].u -= 1.0f;
		}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AnimatedHeightField::FixVOverflow(
int layer)
{
	for (int iy = 0; iy < yVerts; iy++)
		for (int ix = 0; ix < xVerts; ix++)
		{
			// Get our node pointer
			AHFNode *node = Node(ix, iy);
			node->baseuv[layer].v -= 1.0f;
		}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AnimatedHeightField::FixUUnderflow(
int layer)
{
	for (int iy = 0; iy < yVerts; iy++)
		for (int ix = 0; ix < xVerts; ix++)
		{
			// Get our node pointer
			AHFNode *node = Node(ix, iy);
			node->baseuv[layer].u += 1.0f;
		}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AnimatedHeightField::FixVUnderflow(
int layer)
{
	for (int iy = 0; iy < yVerts; iy++)
		for (int ix = 0; ix < xVerts; ix++)
		{
			// Get our node pointer
			AHFNode *node = Node(ix, iy);
			node->baseuv[layer].v += 1.0f;
		}
}

/////////////////////////////////////////////////////////////////////////////
// Compute the specular for this bad boy
/////////////////////////////////////////////////////////////////////////////
inline void AnimatedHeightField::ComputeSpecularUV(
int ix,
int iy,
Vector3CRef cameraPos,
const Map &map,
UVCOORD_TYPE &uv)
{
	// Get the node
	AHFNode &node = *Node(ix, iy);

	// Get the normal
	Vector3CRef normal = *node.GetNormal();

	// Compute reflection vector
	Vector3 r = normal * (2.0f * Vector3::Dot(normal, map.light)) - map.light;

	// compute the angle
	float a = Vector3::Dot(r, node.toEye);

	// Raise to a power
	// to approximate (N.H)^16 for specular highlights, we use ((N.H)^2 - 0.75)*4
	//float s = Math::Abs(0.5f * Math::Pow(a, 16.0f));
	float s = Math::Clamp((Math::Square(a) - 0.75f) * 2.0f);

	// Map it
	uv.u = s;
	uv.v = s;
}

/////////////////////////////////////////////////////////////////////////////
// Compute the specular for this bad boy
/////////////////////////////////////////////////////////////////////////////
inline void AnimatedHeightField::ComputeEnvironmentUV(
AHFNode &node,
Vector3CRef cameraPos,
const Map &map,
UVCOORD_TYPE &uv)
{
	// Get the normal
	Vector3CRef normal = *node.GetNormal();

	// Compute reflection vector
	Vector3 r = normal * (2.0f * Vector3::Dot(normal, node.toEye)) - node.toEye;

#if 0
	// Map to spherical coordinates
	float m = 2.0f * Math::Sqrt(Math::Square(r.x) +
							Math::Square(r.y) +
							Math::Square(r.z + 1.0f));
	uv.u = r.x / m + 0.5f * map.uWrap * 2 - map.uWrap;
	uv.v = r.y / m + 0.5f * map.vWrap * 2 - map.vWrap;
#else
	// Just do a planar mapping
	uv.u = (r.x() * 0.5f + 0.5f) * map.uWrap * 2 - map.uWrap;
	uv.v = (r.y() * 0.5f + 0.5f) * map.vWrap * 2 - map.vWrap;
#endif
}

#if 0
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
inline void AnimatedHeightField::ComputeVThreshold(
int ix,
int iy,
AHFNode &node,
const Map &map,
UVCOORD_TYPE &uv)
{
	const float hLow = -0.1f;
	const float hHigh = 0.4f;
	const float invRange = 1.0f / hHigh - hLow;

	// What is the height?
	float height = node.psi + mVtxBuffer.Vertex(ix + iy * xVerts).z;
	float h = Math::Clamp(-height, hLow, hHigh);
	float s = (h - hLow) * invRange;

#if 0
	float v = node.psiV * 1.0f;
	static float maxV = 0.0f;
	if (v > maxV)
		maxV = v;
	static float minV = 0.0f;
	if (v < minV)
		minV = v;
	float s = Math::Clamp(Math::Abs(v), 0.0f, 0.5f);
#endif

	// map!
	uv.u = s;
	uv.v = s;
}
#endif

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
inline void AnimatedHeightField::ComputeHeightUV(
int ix,
int iy,
AHFNode &node,
const Map &map,
UVCOORD_TYPE &uv)
{
	// What is the height?
	float height = node.psi + mVtxBuffer.Vertex(ix + iy * xVerts).z();
	float h = 0.0f;
	if (map.type == HEIGHT)
		h = Math::Clamp(height, map.heightLow, map.heightHigh);
	else if (map.type == NEGHEIGHT)
		h = Math::Clamp(-height, map.heightLow, map.heightHigh);
	else if (map.type == ABSHEIGHT)
		h = Math::Clamp(Math::Abs(height), map.heightLow, map.heightHigh);

	// what is UV
	float s = (h - map.heightLow) * map.halfInvHeightRange;

	// map!
	uv.u = s;
	uv.v = s;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
inline void AnimatedHeightField::ComputeTurbulenceUV(
AHFNode &node,
const Map &map,
UVCOORD_TYPE &uv)
{
	// Get the normal
	Vector3CRef n = *node.GetNormal();

	// Screw with it
	float x = Math::Clamp(Math::Abs(n.x()) * map.turbXFactor, 0.0f, 1.0f);
	float y = Math::Clamp(Math::Abs(n.y()) * map.turbYFactor, 0.0f, 1.0f);

	// Just do a planar mapping
	uv.u = (x * 0.5f) * map.uWrap * 2 - map.uWrap;
	uv.v = (y * 0.5f) * map.vWrap * 2 - map.vWrap;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
inline float AnimatedHeightField::Psi(
int x,
int y,
float &samples)
{
	if (x < 0 || x >= xVerts || y < 0 || y >= yVerts)
		return(0.0f);

	samples += 1.0f;
	return(node[x + y * xVerts].psi);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
inline void AnimatedHeightField::ApplyDisplacement(
Vector3 &at,
float amount)
{
	// Compute the four closest affected points on the grid
	int x0, x1;
	float xr;
	ToGrid(at.x(), invXGrid, x0, x1, xr);

	int y0, y1;
	float yr;
	ToGrid(at.y(), invYGrid, y0, y1, yr);

	// Apply the displacement
	float x0a = 1.0f - xr;
	float x1a = xr;
	float y0a = 1.0f - yr;
	float y1a = yr;
	amount *= 0.5f;
	ApplyDisplacement(x0, y0, (x0a + y0a) * amount);
	ApplyDisplacement(x1, y0, (x1a + y0a) * amount);
	ApplyDisplacement(x0, y1, (x0a + y1a) * amount);
	ApplyDisplacement(x1, y1, (x1a + y1a) * amount);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
inline void AnimatedHeightField::ToGrid(
float t,
float invGridSize,
int &g0,
int &g1,
float &r)
{
	float tg = t * invGridSize;
	g0 = (int)Math::Floor(tg);
	g1 = (int)Math::Floor(tg + 1.0f);
	r = tg - (float)g0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
inline void AnimatedHeightField::ApplyDisplacement(
int x,
int y,
float amount)
{
	if (x >= 0 && x < xVerts && y >= 0 && y < yVerts)
	{
		AHFNode &n = node[x + y * xVerts];
		if (!n.Locked())
			n.psi = amount;
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AnimatedHeightField::RunPhysics(void)
{
	// Get delta T
	float deltaT = g_timer.GetFrameSec();

	// Integrate acceleration from forces
	int x, y;
	for (y = 0; y < yVerts; y++)
		for (x = 0; x < xVerts; x++)
		{
			AHFNode *node = Node(x, y);

			// What is our multiplier?
			float samples = 0;

			// Apply forces
			node->psiA += (Psi(x - 2, y, samples) +
								Psi(x, y - 2, samples) +
								Psi(x + 2, y, samples) +
								Psi(x, y + 2, samples) +
								Psi(x - 1, y, samples) +
								Psi(x, y - 1, samples) +
								Psi(x + 1, y, samples) +
								Psi(x, y + 1, samples)) - samples * node->psi;

			node->psiA *= elasticity;
			node->psiV = node->psiA * deltaT;
#if 0
			if (x == 16 && y == 16)
				TRACE("V: %.4f\n", node->psiV);
#endif
		}

	// Integrate height from acceleration
	for (y = 0; y < yVerts; y++)
		for (x = 0; x < xVerts; x++)
		{
			AHFNode *node = Node(x, y);
			node->psi += node->waveSpeedSquared * node->psiA * deltaT;
		}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AnimatedHeightField::Render(
const CInstance* pInstance,
const Graphics4x4* pmLocalToCamera,
float fZCamDist)
{
	WaterPool		*p;
	Graphics4x4		m;
	WaterSprite		*ws;
	int				count = 0;

	// get our pool number
	int poolnum = mPoolNumber;

	// if this is a Tylerized ANIHF pool, setup the sprites
	if (poolnum != -1)
	{	
		// get pointer to this pool
		p = g_waterMgr.GetPool (poolnum);

		// get num sprites active
		count = p->mSprites.Count();
	}

	// render the shite
	if (!(debugState & NO_RENDER))
	{
		TextureMgr::Cache().Lock( textureArray );

		Matrix4x4 tex[8];

		// init all matrices to identity
		tex[0].Identity();
		tex[1].Identity();
		tex[2].Identity();
		tex[3].Identity();
		tex[4].Identity();
		tex[5].Identity();
		tex[6].Identity();	
		tex[7].Identity();

		#ifdef USE_ANIHF_WATER
		// PAB - added to allow sprite effects to work on ANIHF water
		// process the nex matrices for effects
		// if we have any, process them now and set the
		// texture matrices
		if (count != 0)
		{
			int	start = mWaterLayersUsed;
			int	end   = mWaterLayersUsed + count;
			int	index = 0;

			if (end > MAX_LAYERS)
				end = MAX_LAYERS;

			for (int i = start; i < end; i++)
			{
				ws = p->mSprites.Get(index);
				tex[i] = ws->mTexMatrix;
				index++;
			}

			// set number layers to render
			mStripBuffer.SetNumTexturePasses (end);
		}
		else
		{
			// set number layers to render
			mStripBuffer.SetNumTexturePasses (mWaterLayersUsed);
		}
		#endif

		mStripBuffer.Render(*pmLocalToCamera, &tex[0], 0);
	}
}
