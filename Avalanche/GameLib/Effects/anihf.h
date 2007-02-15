/////////////////////////////////////////////////////////////////////////////
// Animated height-field stuff
/////////////////////////////////////////////////////////////////////////////
#ifndef ANIHF_H
#define ANIHF_H

#define VTXBUFFER_NO_INLINES
#include "Display/Common/VtxBuffer.h"
#undef VTXBUFFER_NO_INLINES
#include "Display/Common/StripBuffer.h"
#include "Game/Database/DynamicObject.h"

#include "Math/Matrix.h"
#include "container/dllist.h"
#include "Game/Database/Som.h"
#include "Game/Database/Instance.h"
#include "Effects/siggen.h"
#include "linearAlgebra/rayseg.h"
#include "geomUtil/clipseg.h"
#include "collide/aabb.h"

// Maximum number of textures we support
#define ANIHF_MAXTEXTURES	MAX_SOM_TEXTURES_PER_PATCH

// size of script name field
#define MAX_ANIHF_SCRIPT_NAME_LENGTH 32

struct UVCOORD_TYPE
{
	float u,v;
};

// Forward declaration of an animated height field node
class AHFNode;

class AnimatedHeightField : public GenericDynamicObject
{
    friend class AniHFMgr;

public:
	// We don't do stuff until the initialize
	AnimatedHeightField(void);
	virtual ~AnimatedHeightField(void);

	typedef enum
	{
		DECAL = 0,
		SPECULAR,
		ENVIRONMENT,
		HEIGHT,
		NEGHEIGHT,
		ABSHEIGHT,
		TURBULENCE,
		BUMPMAP,
	} MAPTYPE;

	struct TextureLayerInit
	{
		int		textureIndex;	// Texture index in the file
		int	  	method;			// TS_METHOD_REPLACE, TS_METHOD_MODULATE, TS_METHOD_DECAL, TS_METHOD_CONSTANTALPHA, TS_METHOD_INTERPOLATE, TS_METHOD_DOTPRODUCT
		int		blend;		  	// RS_BLENDFUNC_DEFAULT, RS_BLENDFUNC_DIFFUSE
		int		filter;			// TS_FILTER_POINTSAMPLE, TS_FILTER_BILINEAR, TS_FILTER_NEAREST_MIPMAP_NEAREST, TS_FILTER_NEAREST_MIPMAP_LINEAR, TS_FILTER_LINEAR_MIPMAP_NEAREST, TS_FILTER_LINEAR_MIPMAP_LINEAR
		int		flags;		  	// TEXTURE_IS_DIFFUSE_MAP, etc
		float		uWrap;			// Number of times to wrap u
		float		vWrap;			// Number of times to wrap v
		MAPTYPE	mapType;			// uv calculation
		Vector3	light;			// negated light in world coordinates
		union
		{
			struct
			{
				float low;
				float high;			// for height
			};
			struct					// Turbulence factors
			{
				float turbXFactor;
				float turbYFactor;
			};
		};
	};
	static void DefaultTextureLayerInit(TextureLayerInit &init, int textureIndex);

	// Setup the texturing for the height field
	// baseColor provides the base color for the material
	// if a texture file is provided, the layers are read from it
	// if layers is > 0 layerInit must have an array of TextureLayerInit's
	bool SetMaterial(const ts_bRGBA &baseColor,
								char *textureFile = NULL,
								int layers = 0,
								TextureLayerInit *layerInit = NULL);

	// CALL THIS AFTER you do SetMaterials
	// Setup the initial height-field geometry (use the orientation matrix if you don't want
	// this in the xy plane)
	// returns false on failure
	bool BuildGeometry(int pointsX, int pointsY,
								float sizeX, float sizeY,
								bool lighting = true,
								bool lockEdges = true,
								bool wantVertexColors = false,
								bool isSeeThru = true,
								ts_bRGBA *vertexColor = NULL);

	// Setup the phyics for wave propagation
	// Viscosity is in the range 0.0 to 1.0, with 1.0 meaning the fluid
	// will not move.
	// Calling this ENABLES PHYSICS.  Do not call this function
	// if you don't want physics!!!
	void SetupPhysics(float waveSpeed, float viscosity = 0.2f);

	// Use this call to modify the geometry
	typedef enum
	{
		OVERRIDE_LOCKED = 0x01,
		OVERRIDE_POSITION = 0x02,
		OVERRIDE_WAVESPEED = 0x04,
		OVERRIDE_COLOR = 0x08,
		OVERRIDE_SHADOWED = 0x10
	} NODEOVERRIDES;

	struct NodeInfo
	{
		int			override;		// What in this node is set to be overridden?

		bool			locked;			// locked (meaning their geometry is never moved)
		Vector3		position;		// positional override
		float			waveSpeed;		// Wave speed override
		ts_bRGBA		color;			// RGBA
		bool			shadowed;		// in shadow
	};

	// nodeInfo is a pointsX by points Y (in X row order) that modifies the geometry
	// Call this after BuildGeometry and SetupPhysics
	bool ModifyNodes(NodeInfo *nodeInfo);

	// Override a single node
	bool ModifyNode(int x, int y, NodeInfo &info);

	// If you modify a lot of nodes positions, call this!!
	void RecomputeBounds(void);

	// Position the object in the world
	void Place(const Graphics4x4 &m);

	// Setup culling
	void SetLODDistance(float noGeometryAnimate, float noTextureAnimate, float doNothing);

	// Setup the noise parameters for Z
	void SetWave(float waveFreq, float waveAmp, SigGen::TYPE type = SigGen::SIN);

	// Setup the XY positional variance
	void SetXYVariance(float xFreq, float xAmp, SigGen::TYPE xType,
								float yFreq, float yAmp, SigGen::TYPE yType);

	// Setup uv variance for a texture set
	void SetUVVariance(int uvSet,
								float uFreq, float uAmp, SigGen::TYPE uType,
								float vFreq, float vAmp, SigGen::TYPE vType);

	// Surface Height (used for general changes in entire surface height over time..
	void ChangeSurfaceHeight(float targetHeight, float metersPerSecond) { mSurfaceHeightTarget = targetHeight; mSurfaceHeightVelocity = metersPerSecond; }
	float CurrentSurfaceHeight() { return mSurfaceHeight; }

	// Setup the flow
	void SetUVFlow(int uvSet, float uFlow, float vFlow);

	// Apply a displacement
	void ApplyForce(Vector3CRef point, float force);

	// Test function to doink the hf
	void Doink(float force = 1.0f);

	// Test function to doink the hf
	void Doink(Vector3 &point, float force = 1.0f);

	// Update the animation
	void Update(float deltaTime, Graphics4x4 &camera);

	// some simple queries
	int XPoints(void)
		{ return(xVerts); }
	int YPoints(void)
		{ return(yVerts); }
	Vector3 PointInWorld(int x, int y);
	CInstance *Instance()	{ return(instance); }

	// script name support, for finding a particular height field
	char *GetScriptName(void)			{ return(scriptName); }
	void SetScriptName(char *name)	{ strncpy(scriptName, name, MAX_ANIHF_SCRIPT_NAME_LENGTH); }

	// Collide a segment with all the height fields
	// Returns false if no collision
	struct Collision
	{
		Vector3					intersection;	// Point of intersection
		Vector3					normal;
		AnimatedHeightField	*hf;				// Height field collided with
		float						d;					// distance along d
	};
	virtual bool Test(Segment &seg, AnimatedHeightField::Collision &collision);

	// Query some properties based on a point on the height field
	// All return false if the point is off the height field.
	bool GetWaveSpeed(Vector3CRef point, float &waveSpeed);
	bool GetMaterial(Vector3CRef point, int &material);
	bool GetVelocity(Vector3CRef point, Vector3 &velocity);

	virtual void Render(const CInstance* pInstance, const Graphics4x4* pmLocalToCamera, float fZCamDist);

	// pool index
	int	mPoolNumber;
	int	mWaterLayersUsed; 

	// had to move out to public (PAB)
	TextureArray	*textureArray;		// needed for LockTextureSet()
	Graphics4x4		worldToLocal;		// need access for sprite effects
	CInstance				*instance;

protected:
	// Our array of data
	AHFNode					*node;
	int						xVerts, yVerts;
	float						xGrid, yGrid;
	float						invXGrid, invYGrid;
	inline AHFNode *Node(int x, int y);
	inline Vector4 NodePos(int x, int y);
	inline Vector3Packed *SafePoint(int x, int y);
	inline Vector3 SafeNormal(int x, int y);

	// Our tristrips for the height field
	VtxBuffer mVtxBuffer;
	StripBuffer mStripBuffer;

	bool BuildColumnTriStrip(int x0, int x1);

	// All the stuff to represent the object in the renderer
	bool						wantEngineLighting;
	bool InitializeModel(bool isSeeThru);

	// LOD control
	float						noGeoD;
	float						noTexD;
	float						nothingD;

	// Function generators
	SigGen					waveX, waveY, waveZ;

	// Our texture context
	ts_FullTextureContext	*textureContext;
	int						textureLayers;

	// Name of script that generated heightfield, so it can be found later
	char						scriptName[MAX_ANIHF_SCRIPT_NAME_LENGTH];

	typedef enum
	{
		GROUP_DECAL,
		GROUP_EYEBASEDLIGHTING,
		GROUP_HEIGHT
	} MAPGROUP;
	MAPGROUP MappingGroup(MAPTYPE type);

	// Here is all the information on how we apply a texture
	struct Map
	{
		// Mapping type
		MAPTYPE			type;

		// class of mappping
		MAPGROUP			group;

		// Wrapping data
		float				uWrap;
		float				vWrap;

		// Function generators
		SigGen			waveU, waveV;
		float				flowU, flowV;

		// Data specific to special mapping types
		union
		{
			struct
			{
				Vector3	worldLight;
				Vector3	light;
			};
			struct
			{
				float		heightLow, heightHigh;
				float		halfInvHeightRange;
			};
			struct
			{
				float		turbXFactor;
				float		turbYFactor;
			};
		};
	};
	Map						map[ANIHF_MAXTEXTURES];

	// Do we have eye based lighting?
	bool						needNormals;
	bool						eyeBasedLighting;

	// Is the color table for normals?
	bool						vertexColorsAreNormals;

	// Get the uv for a specific point on the HF
	inline UVCOORD_TYPE GetUV(int layer, int x, int y);

	// Build the vertex colors
	void BuildVertexColors(ts_bRGBA &c);

	// Setup all nodes to defaults
	void SetupNodes(void);

	// Fast random number generator
	static float RandUnit(void)
		{ return((float)(rand() & 0xffff) / (float)0xffff); }

	// We might have a local normal table
	Vector3Packed			*localNormalTable;

	// Compute a normal for a node
	Vector3 ComputeNormal(int x, int y);

	// Fix overflows on u/v
	void FixVOverflow(int layer);
	void FixUOverflow(int layer);
	void FixVUnderflow(int layer);
	void FixUUnderflow(int layer);

	// Compute texture mapping shit
	inline void ComputeSpecularUV(int ix, int iy, Vector3CRef cameraPos, const Map &map, UVCOORD_TYPE &uv);
	inline void ComputeEnvironmentUV(AHFNode &node, Vector3CRef cameraPos, const Map &map, UVCOORD_TYPE &uv);
	inline void ComputeHeightUV(int ix, int iy, AHFNode &node, const Map &map, UVCOORD_TYPE &uv);
	inline void ComputeTurbulenceUV(AHFNode &node, const Map &map, UVCOORD_TYPE &uv);

	// Stuff for physics
	bool 				havePhysics;
	float				elasticity;
	inline float Psi(int x, int y, float &samples);
	inline void ApplyDisplacement(Vector3 &at, float amount);
	inline static void ToGrid(float t, float invGridSize, int &g0, int &g1, float &r);
	inline void ApplyDisplacement(int x, int y, float amount);
	void RunPhysics(void);

	// Stuff for collisions
	bool				collisions;
	ClipSegment		clipper;
	AABB				*aabb;
	bool				TestQuad(Segment &seg, int x, int y, Collision &collision);
	bool 				TestTriangle(Segment &seg, Vector3Packed &a, Vector3Packed &b, Vector3Packed &c, Collision &collision);

	// Debug stuff
	static const uint NO_DEBUG;
	static const uint NO_PHYSICS;
	static const uint NO_RENDER;
	static const uint ALL_DEBUG;
	uint debugState;

	/////////////////////////////
	// surface height
	/////////////////////////////
	float mSurfaceHeight;
	float mSurfaceHeightTarget;
	float mSurfaceHeightVelocity;

	/////////////////////////////
	// time
	/////////////////////////////
	float mT;
	float mDeltaT;

public:
	// We may appear in a list
	AnimatedHeightField	*prev, *next;
};

#endif
