#ifndef __WATER_H__
#define __WATER_H__

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


#ifndef PS2
#define SUPPORT_GENERIC_RENDERER
#endif

#ifndef PS2
#define USE_ANIHF_WATER		// USE FOOD's WATER INSTEAD OF TYLER's
#endif

#ifdef _DEBUG
#define DEBUG_WATER
#endif

//#ifndef PS2
//#define WATER_USE_FANS
//#endif

// headers
#include "Display/Common/StripBuffer.h"
#include "Display/Common/VtxBuffer.h"
#include "container/dynlist.h"
#include "Game/Database/DynamicObject.h"
#include "collide/aabb.h"
#include "geomUtil/clipseg.h"

// forward declares
class Water;
class WaterAffector;
class WaterEquation;
class WaterSprite;
class WaterPool;

#include "Effects/Water/WaterGrid.h"

// WHY??? Why doesnt the engine have a color class that supports operators?
// WHO??? Who is responsible for this affront on the civilized world?
// WHAT??? What does everyone do?  make their own code every time they have to do something?
// WHEN??? When will it all end?
// WHERE??? Where does the blame fall?
class WaterColor
{
	u32 color;
public:
	void R(u8 val)											  { color = (color & 0xFFFFFF00) | (val); }
	void G(u8 val)											  { color = (color & 0xFFFF00FF) | (val << 8); }
	void B(u8 val)											  { color = (color & 0xFF00FFFF) | (val << 16); }
	void A(u8 val)											  { color = (color & 0x00FFFFFF) | (val << 24); }
	u8 R(void) const												  { return (u8)(color & 0xFF); }
	u8 G(void) const												  { return (u8)((color >> 8) & 0xFF); }
	u8 B(void) const												  { return (u8)((color >> 16) & 0xFF); }
	u8 A(void) const												  { return (u8)(color >> 24); }

	WaterColor()													  { }
	WaterColor(u32 _color)										  { color = _color; }
	WaterColor(ts_bRGBA _color)								  { R(_color.R); G(_color.G); B(_color.B); A(_color.A); }
	WaterColor operator*(float rhs) const					  { WaterColor temp; temp.R((u8)(R() * rhs)); temp.G((u8)(G() * rhs)); temp.B((u8)(B() * rhs)); temp.A((u8)(A() * rhs)); return temp; }
	WaterColor operator+(const WaterColor rhs) const	  { WaterColor temp; temp.R(R() + rhs.R()); temp.G(G() + rhs.G()); temp.B(B() + rhs.B()); temp.A(A() + rhs.A()); return temp; }
	operator u32() const											  { return color; }
};

struct WaterLayerModes
{
	unsigned int mFlags; // see layer flags below...
	union {
		Texture *mTexture;
		unsigned int mTextureIndex;
	};
	unsigned int mMethod;
	unsigned int mFilter;
	unsigned int mBlendFunc;
	unsigned int mLayerType;
	float mScaleU;
	float mScaleV;
	float mFlowU;
	float mFlowV;
	Vector3 mVtxAdjParameter;
	Vector3 mNormAdjParameter;
	Vector3 mLight;
	Vector3 mWorldLight;
	float mOpacityOverTime; // note sign determines if increasing or decreasing currently
	float mOpacity;

	enum {
		LAYER_DECAL,
		LAYER_ENVIRONMENT,
		LAYER_SPECULAR,
		LAYER_DISTORT,
		LAYER_SHADOW,

		LAYER_INVALID,
	};

	// layer flags..
	enum {
		// does the texture in the layer refer to an index in the
		// set or is it a bind #?
		LAYER_TEXSET_INDEX		  = 0x00000000,
		LAYER_TEX_BIND				  = 0x00000001,

		LAYER_FADE_EDGES			  = 0x00000002,
		LAYER_FADE_CENTER			  = 0x00000004,
		LAYER_FADE_HEIGHT			  = 0x00000006,
	};
};

// params structure
#define MAX_GRID_SIZE			  10
#define MAX_SPRITES				  10
#define MAX_BUMPS					  1
#define MAX_EQUATIONS			  10
#define MAX_AFFECTORS			  5
#define MAX_LAYERS				  5
#define MAX_WAKES					  50
#define MAX_RINGS					  3

struct WaterCellFlags
{
	u8 inUse;
	u8 receiveShadows;
	u8 runPhysics;
	u8 collides;
	u8 clampN;
	u8 clampS;
	u8 clampE;
	u8 clampW;
	u8 scaleWaveOnDepth;
	u8 slowWavesOnDepth;
};

struct WaterLayerModeParams
{
	unsigned int mTextureLayer;
	u8 mPointSample;
	char mMethod[32];
	char mBlendFunc[32];
	char mLayerType[32];
	float mScaleU;
	float mScaleV;
	float mFlowU;
	float mFlowV;
	
	bool mFadeEdges;
	bool mFadeCenter;
	bool mFadeHeight;
	float mOpacityOverTime;
	float mOpacity;

	Vector3 mVtxAdjParameter;
	Vector3 mNormAdjParameter;
	Vector3 mWorldLight;
};
struct WaterParams
{
	bool disableVisReg;
	float cellSizeX, cellSizeZ;
	u8 subDivNodes;
	u8 maxSubDivDepth;
	u8 minSubDivDepth;
	u8 dampenSubDivisions;
	u8 shadowSubDivisions;
	u8 physicsSubDivisions;
	float physicsWaveSpeed;
	float physicsElasticity;
	float physicsForce;
	float physicsTrailAffector;

	float spriteFlowX;
	float spriteFlowZ;
		
	// layer info
	char textureFile[100];
	WaterLayerModeParams layers[MAX_LAYERS];

	// cell grid info
	u8 cellsX;
	u8 cellsZ;
	Vector3 pos;
	float angle;
	struct
	{
		WaterCellFlags flags;
	} cellInfo[MAX_GRID_SIZE][MAX_GRID_SIZE];

	// wakes
	u32 wakePointCount;
	float wakeRadius;
	float wakeHeight;
	float wakeLifetime;
	float wakeForwardContribution;
	float wakePerpendicularContribution;
	float wakeMinSpeedRatio;
	float wakeTrailAffector;
	float wakeDelay;

	// rings
	float ringAttack;
	float ringSustain;
	float ringRelease;
	float ringGrowRate;
	float ringWaveLength;
	float ringWaveStartPosition;
	float ringWaveHeight;
	float ringWavePhaseShift;
	float ringWaveCount;

	// noise field
	Vector3 noiseScale;
	float noiseFrequency;
};

class Water : public GenericDynamicObject
{
public:

	////////////////////////////////////////////////////////////////////////////
	// Initialization and cleanup
	////////////////////////////////////////////////////////////////////////////

	Water(void);
	~Water(void);

	// init flags:
	enum {
		LOCK_TOP_EDGE					= 0x00000001,
		LOCK_BOTTOM_EDGE				= 0x00000002,
		LOCK_LEFT_EDGE					= 0x00000004,
		LOCK_RIGHT_EDGE				= 0x00000008,
		SCALE_WAVES_ON_WATER_DEPTH	= 0x00000010,
		SLOW_WAVES_ON_WATER_DEPTH	= 0x00000020,
	};

	bool InitFromParams(const WaterParams& params, int gridX, int gridY, WaterPool* pool);
	bool Init(
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
		u8 minxSubDivDepth, 
		int subDivNodes, 
		int dampenSubDivisions = 5, 
		int shadowSubDivisions = 0,
		int physicsSubDivisions = 0, 
		float physicsWaveSpeed = 1.26f,
		float elasticity = 0.9f,
		bool disableVis = false);

	////////////////////////////////////////////////////////////////////////////
	// per-frame update & render
	////////////////////////////////////////////////////////////////////////////

	// This must be called once per frame to update the water.
	void Update(float deltaTime, Graphics4x4& cameraMatrix);

	// this is the render call that gets us drawn
	virtual void Render(const CInstance* pInstance, const Graphics4x4* pmLocalToCamera, float fZCamDist);

	////////////////////////////////////////////////////////////////////////////
	// Interact with the physics system
	////////////////////////////////////////////////////////////////////////////

	// enable/disable physics work
	void EnablePhysics(bool enabled = true);

	// touch the water..
	void ApplyForce(const Vector3& point, float force);
	void ApplyDisplacement(Vector3& at, float amount);
	void ApplyDisplacement(int x, int z, float amount);
	void Doink(const Vector3& point, float force);
	void Doink(float force);

	////////////////////////////////////////////////////////////////////////////
	// get information
	////////////////////////////////////////////////////////////////////////////

	// this can be useful... but its kindof expensive..
	float GetHeight(float x, float z); // get height of water above 0.0f (height is local to water origin)
	float GetHeightLocal(float x, float z); // x=[0.0f, 1.0f] y=[0.0f, 1.0f]
	Vector3 GetNormal(float x, float z);
	Vector3 GetNormalLocal(float x, float z); // x=[0.0f, 1.0f] y=[0.0f, 1.0f]

	// this returns the depth at that point, this is not constant value
	float GetDepth(float x, float z);
	float GetDepthLocal(float x, float z); // x=[0.0f, 1.0f] y=[0.0f, 1.0f]

	// this returns the depth at that point, but its relative to 0.0f, not GetHeight() (ie, its constant)
	// this is MUCH MUCH cheaper than GetDepth()
	float GetRawDepth(float x, float z);
	float GetRawDepthLocal(float x, float z); // x=[0.0f, 1.0f] y=[0.0f, 1.0f]

	// get at my instance data...
	CInstance* GetInstance(void) { return mInstance; }

	// useful for equations to localize their info...
	const Graphics4x4& GetWorldToLocal(void) { return mWorldToLocal; }
	const Graphics4x4& GetWorldToLocalBasis(void) { return mWorldToLocalBasis; }
	float GetXScale(void)	{ return mXScale; }
	float GetZScale(void)	{ return mZScale; }
	float GetTime(void)		{ return mT; }

	////////////////////////////////////////////////////////////////////////////
	// collision testing
	////////////////////////////////////////////////////////////////////////////

	// inaccurate collision testing...
	// these dont take the motion of the water into account, but
	// may be accurate enough for some tests.  They are basically just
	// square tests (as in a 2d box).
	// these assume that the water has not been rotated except around the y axis. (ie, its horizontal)
	bool QuickTestCollision(const Segment& s, Vector3& hitPoint);
	bool QuickTestCollision(const Ray& s, Vector3& hitPoint);
	bool QuickPointInWater(const Vector3& testPoint);

	// accurate collision testing
	bool PointInWater(const Vector3& testPoint);
	// NOTE!!! dont use these to do things like float an object on the water.  Instead use
	// GetHeight() its MUCH faster.  These calls make numerous calls to GetHeight and can be
	// EXTREMELY expensive.
	bool TestCollision(const Segment& s, Vector3& hitPoint);
	bool TestCollision(const Ray& s, Vector3& hitPoint);

	////////////////////////////////////////////////////////////////////////////
	// this is actually protected information.  its up here because MSVC is a
	////////////////////////////////////////////////////////////////////////////
	enum {
		// useful numbers
		NUM_NEIGHBORS = 4,
		NUM_QUADRANTS = 4,
	};


protected:
	/////////////////////////////
	// Pool support
	/////////////////////////////
	friend class WaterPool;
	friend class WaterSprite;
	WaterPool* mPool;


	/////////////////////////////
	// enums used internally...
	/////////////////////////////

	// in terms of Up Down Left Right
	enum {
		// quadrant masks
		Q_OX = 0x1,
		Q_XO = 0x2,
		Q_OL = 0x0,
		Q_OR = 0x1,
		Q_DO = 0x2,
		Q_UO = 0x0,
		Q_XX = 0x3,

		// quadrants
		Q_UL = 0x0,
		Q_UR = 0x1,
		Q_DL = 0x2,
		Q_DR = 0x3,

		// neighbors
		N_L = 0,
		N_R = 1,
		N_U = 2,
		N_D = 3,
	};

	enum {
		// here are the errors that the subdivide can return
		// the higher the # the higher the error priority.
		SUBDIVISION_ERRORS	= 0x100,
		OUT_OF_NODES			= 0x101,
		TOO_DEEP					= 0x102,
	};
	
	// indices for the various strip buffers we create..
	enum {
		SB_PLAIN,
		SB_SPLIT_1,
		SB_SPLIT_2A, // horizontal or vertical
		SB_SPLIT_2B, // right angle
		SB_SPLIT_3,
		SB_SPLIT_4,

		// how many types are there
		NUM_STRIP_LAYOUTS,
	};
	

	/////////////////////////////
	// structures used internally
	/////////////////////////////

	// a cell node
	struct Node
	{
		u8 parent;
		u8 quadrant;
		u8 size;
		u8 quad[NUM_QUADRANTS];
		u8 neighbor[NUM_NEIGHBORS];
		u8 splits[NUM_QUADRANTS][NUM_NEIGHBORS];
		u8 splitcount[NUM_QUADRANTS];
	};

	// fan renderer geometry
	struct RendNode
	{
		// edges of this cell...
		Vector3 mEdges[NUM_NEIGHBORS];
		// centers of the respective cells
		Vector3 mCenters[NUM_QUADRANTS];

		// normals that correspond to verts above...
		Vector3 mEdgeNormals[NUM_NEIGHBORS];
		Vector3 mCenterNormals[NUM_QUADRANTS];
	};
	struct RendMasterNode
	{
		// positions of the primary meshes corners & center...
		Vector3 mCorners[NUM_QUADRANTS];
		Vector3 mCenter;

		// normals that correspond to verts above...
		Vector3 mCornerNormals[NUM_QUADRANTS];
		Vector3 mCenterNormal;
	};

	/////////////////////////////
	// overall initialization & shutdown
	/////////////////////////////

	// handle the instance
	bool InitializeInstance(bool disableVis = false);
	void CleanupInstance(void);

	// the instance info for this thing
	CInstance*	mInstance;

	// sometimes we need to go from world to local space
	Graphics4x4 mWorldToLocal;
	Graphics4x4 mWorldToLocalBasis;
	Graphics4x4 mLocalToWorldBasis;

	/////////////////////////////
	// position in space, etc..
	/////////////////////////////

	// position
	Vector3 mPosition;
	float mOrientation;

	// offset to position in local space
	float mXOffset, mZOffset;

	// dimensions
	float mXScale, mInvXScale;
	float mZScale, mInvZScale;

	/////////////////////////////
	// subdivision system
	/////////////////////////////

	// track subdivisions
	u8 mMinSubDivDepth;
	u8 mMaxSubDivDepth;

#ifdef SUPPORT_GENERIC_RENDERER
	int mNodes;
	int mCurrNodeIdx;
	Node *mCurrNode;
	Node *mNodeArray;

	// main subdivision entry point
	void ReSubDivide(void);

	// internal subdivision methods
	void AdaptiveSubDivide(void);
	void UniformSubDivide(int depth, int cellNumber = 0);

	// subdivision helpers
	int SubDivide(int cellNumber, int quadrant_to_divide);
	int SubDivideToRadius(int cellIdx, float centerX, float centerZ, float shift, float minPosX, float minPosZ, float maxPosX, float maxPosZ, int depth);
	void DisplayCellGraph(int cellNumber, int depth);

	// accessors
	Node* Cell(int cellNumber) { return &mNodeArray[cellNumber]; }
#endif

	/////////////////////////////
	// update
	/////////////////////////////

	// current time... ick...
	float mT;

	/////////////////////////////
	// collide tests
	/////////////////////////////

	bool TestCollisionLocal(const Segment& seg, Vector3& dir, Vector3& hitPoint);

	// used for collision testing
	ClipSegment mSegClipper;

	/////////////////////////////
	// Rendering system
	/////////////////////////////
	
	// normal renderer
	void RenderGeneric(const CInstance* pInstance, const Graphics4x4* pmLocalToCamera, float fZCamDist);
	// platform specific renderer (may just call generic...)
	void RenderPlatform(const CInstance* pInstance, const Graphics4x4* pmLocalToCamera, float fZCamDist);

	// fan renderer geometry
#ifdef SUPPORT_GENERIC_RENDERER
	RendNode *mRenderNodeArray;
	RendMasterNode mRendMasterNode;
#endif

	// layer setup
	WaterLayerModes *mLayerModes;
	int mNumLayers;

	// for specular & env calcs..
	Vector3 mEyePoint;

	// initialization / shutdown for rendering
	void InitializeStripBuffers(int numSprites);

	// rendering entry & helpers
	void RenderRecurse(int cellNumber, float shift);
	void FillBaseTile(int rotations, int offset, int cellNumber, Node* cell, u8 quad, u8 quadrant);
	int FillTile(u8 cellNumber, Node* cell, u8 quad, int offset, u8& buffertype);
	void EvaluateRenderTile(u8 index, float centerx, float centerz, float shift);
	void EvaluateMesh();
	void BuildSplitInfo(u8 cellNumber, Node* cell, u8 quad);
	void EvaluateRenderTileNormals(int index);
	void FillTileNormals(u8 cellNumber, Node* cell, u8 quad);
	void MergeDuplicateNormals(int cellNumber);
	void SetSpecularCoords(int layer, int index, Vector3& vtx, Vector3& normal, Vector3& toEye);
	void SetEnvironmentCoords(int layer, int index, Vector3& normal, Vector3& toEye);
	void DO_VERTEX(int vtx, Vector3& srcvtx, Vector3& normalvtx); // temp function

	void AddStrips(int buffertype, StripBuffer& stripBuffer, int offset);
	void AddStrip(StripBuffer& stripBuffer, const u16* indexlist, int count, int offset);
	void ResetStripBuffers(int numSprites);


	// our graphics data
#ifdef SUPPORT_GENERIC_RENDERER
	int mVerts;
	VtxBuffer mVtxBuffer[MAX_LAYERS];

	// a variety of strip buffers for drawing various configurations
	// of verts
	StripBuffer mStripBuffer[MAX_LAYERS];
#endif

	// texture data base info...
	u32 mBackBufferCopyHandle;
	TextureArray *mTextureArray;

	/////////////////////////////
	// sprites
	/////////////////////////////

	// temp sprite support...
#ifdef SUPPORT_GENERIC_RENDERER
	StripBuffer mSpriteStripBuffers[MAX_SPRITES];
	VtxBuffer mSpriteVtxBuffers[MAX_SPRITES];
#endif


	/////////////////////////////
	// physics info...
	/////////////////////////////

	// internal methods
	bool InitializePhysics(int subdivs, float physicsWaveSpeed, float elasticity);
	void CleanupPhysics(void);
	void RunPhysics(float deltaT);
	float PhysicsHeight(float x, float z);
	float PhysicsPsi(int x, int y, float& samples);
	void DepthSlowWaves(void);
	
	// physics grid info
	WaterGrid<float> mPhysicsGrid;

	// physics data
	float *mPsi;
	float *mPsiA;
	float *mPsiV;
	float *mWaveSpeedSquared;

	// how elastic should we be?
	float mElasticity;

	// is it on?
	bool mPhysicsEnabled;

	/////////////////////////////
	// dampen grid...
	/////////////////////////////
	void DepthDampenField(float minDepth, float maxDepth);
	void ClampEdges(u32 flags); // flags from init... (should be done after depth)
	inline float DampenFieldValue(float x, float z);
	// dampen all waves (including equation waves) around these areas...
	// this is a mDampenResolution X mDampenResolution grid of bytes
	// that gets multiplied by the y value before its stored... where
	// 0 == 0.0f && 255 == 1.0f
	WaterGrid<u8> mDampeningGrid;

	/////////////////////////////
	// shadow texture
	/////////////////////////////
	void CreateShadowTexture(int subdivisionLevel);
	StripBuffer mShadowStripBuffer;
	VtxBuffer mShadowVtxBuffer;


	/////////////////////////////
	// depth grid...
	/////////////////////////////
	void InitDepthGrid(void);
	WaterGrid<float> mDepthGrid;

	/////////////////////////////
	// Bumps
	/////////////////////////////

	struct BumpSegment
	{
		Vector3PackedData mEndPoint;
	};
	struct BumpSemiCircle
	{
		float mRadius;
		float mStartAngle;
		float mEndAngle;
	};
	enum { BUMP_SEGMENT, BUMP_SEMICIRCLE };
	struct Bump
	{
		float mRange;
		float mHeight;
		Vector3PackedData mReferencePoint;
		int type;
		union DATA
		{
			BumpSegment mSegment;
			BumpSemiCircle mSemiCircle;
		} mData;
	};

	typedef DynamicArray<Bump> BumpList;
	BumpList mBumps;
	Bump* AddBump(float x, float z, float range, float height);
	float BumpSegmentHeight(Bump* bump, float x, float z);
	float BumpSemiCircleHeight(Bump* bump, float x, float z);
	float BumpHeight(float x, float z);

};

#include "Effects/Water/Water.hpp"

#ifdef DEBUG_WATER
extern struct WaterDEBUG
{
	// physics...
	bool mDisablePhysics;
	float mPhysicsScaleWaveSpeed; // used like (it+1.0f) * ____________ (so that 0 == 1)
	float mPhysicsDampeningValue; // used like (it+1.0f) * ____________ (so that 0 == 1)

	// other stuff
	bool mDisableEquations;
	bool mDisableSurfaceHeight;
	bool mDisableNoise;
	bool mDisableWakes;
	bool mDisableBumps;
	bool mDisableRings;
	bool mDisableDampening;
	bool mUseGenericCode;

	// drawing extra info...
	bool mDrawSubDivisionRings;
	bool mDrawDampeningGrid;
	bool mDrawPhysicsGrid;
	bool mDrawColorGrid;
	bool mDrawNormalLines;
	bool mDisableDrawWater;
} Water_Dbg;
#endif //DEBUG_WATER

#endif __WATER_H__
