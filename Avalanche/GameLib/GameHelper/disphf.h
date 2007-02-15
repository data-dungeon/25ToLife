/////////////////////////////////////////////////////////////////////////////
// Display height-field stuff
/////////////////////////////////////////////////////////////////////////////
#ifndef DISPHF_H
#define DISPHF_H

#include "Math/Matrix.h"
#include "container/dllist.h"
#include "GameHelper/ctristrip.h"
#include "Game/Database/Som.h"
#include "Game/Database/Instance.h"
#include "Effects/siggen.h"
#include "linearAlgebra/rayseg.h"
#include "geomUtil/clipseg.h"
#include "collide/aabb.h"

// Maximum number of textures we support
#define DISPHF_MAXTEXTURES	MAX_SOM_TEXTURES_PER_PATCH

// Forward declaration of an height field node
class DHFNode;

class DisplayHeightField
{
public:
	// We don't do stuff until the initialize
	DisplayHeightField(void);
	virtual ~DisplayHeightField(void);
	typedef struct
	{
		int		textureIndex;	// Texture index in the file
		int	  	method;			// TS_METHOD_REPLACE, TS_METHOD_MODULATE, TS_METHOD_DECAL, TS_METHOD_CONSTANTALPHA, TS_METHOD_INTERPOLATE, TS_METHOD_DOTPRODUCT
		int		blend;		  	// RS_BLENDFUNC_DEFAULT, RS_BLENDFUNC_DIFFUSE
		int		filter;			// TS_FILTER_POINTSAMPLE, TS_FILTER_BILINEAR, TS_FILTER_NEAREST_MIPMAP_NEAREST, TS_FILTER_NEAREST_MIPMAP_LINEAR, TS_FILTER_LINEAR_MIPMAP_NEAREST, TS_FILTER_LINEAR_MIPMAP_LINEAR
		int		flags;		  	// TEXTURE_IS_DIFFUSE_MAP, etc
		float		uWrap;			// Number of times to wrap u
		float		vWrap;			// Number of times to wrap v
	} TextureLayerInit;
	static void DefaultTextureLayerInit(TextureLayerInit &init, int textureIndex);

	// Setup the texturing for the height field
	// baseColor provides the base color for the material
	// if a texture file is provided, the layers are read from it
	// if layers is > 0 layerInit must have an array of TextureLayerInit's
	virtual bool SetMaterial(const ts_bRGBA &baseColor,
										char *textureFile = NULL,
										int layers = 0,
										TextureLayerInit *layerInit = NULL);

	// CALL THIS AFTER you do SetMaterials
	// Setup the initial height-field geometry (use the orientation matrix if you don't want
	// this in the xy plane)
	// returns false on failure
	virtual bool BuildGeometry(int pointsX, int pointsY,
										float sizeX, float sizeY,
										bool lighting = false,
										bool wantVertexColors = false,
										ts_bRGBA *vertexColor = NULL);

	// Use this call to modify the geometry
	typedef enum
	{
		OVERRIDE_POSITION = 0x01,
		OVERRIDE_COLOR = 0x02,
	} NODEOVERRIDES;

	typedef struct
	{
		int			override;		// What in this node is set to be overridden?

		Vector3		position;		// positional override
		ts_bRGBA		color;			// RGBA
	} NodeInfo;

	// nodeInfo is a pointsX by points Y (in X row order) that modifies the geometry
	// Call this after BuildGeometry and SetupPhysics
	virtual bool ModifyNodes(NodeInfo *nodeInfo);

	// Override a single node
	virtual bool ModifyNode(int x, int y, NodeInfo &info);

	// If you modify a lot of nodes positions, call this!!
	virtual void RecomputeBounds(void);

	// Position the object in the world
	virtual void Place(const Graphics4x4 &m);

	// Update the animation
	virtual void Update(Graphics4x4 &camera);

	// some simple queries
	int XPoints(void)
		{ return(xSize); }
	int YPoints(void)
		{ return(ySize); }
	Vector3 PointInWorld(int x, int y);

	// Collide a segment with all the height fields
	// Returns false if no collision
	typedef struct
	{
		Vector3					intersection;	// Point of intersection
		Vector3					normal;
		DisplayHeightField	*hf;				// Height field collided with
		float						d;					// distance along d
	} Collision;
	bool Test(Segment &seg, DisplayHeightField::Collision &collision);

private:
	// Our array of data
	DHFNode					*node;
	int						xSize, ySize;
	float						xGrid, yGrid;
	float						invXGrid, invYGrid;
	inline DHFNode *Node(int x, int y);
	inline Vector4 NodePos(int x, int y);
	inline Vector3 *SafePoint(int x, int y);
	inline Vector3 SafeNormal(int x, int y);

	// Our tristrips for the height field
	DoublyLinkedList<TriStripListElement>	triStripList;
	bool BuildColumnTriStrip(int x0, int x1);

	// All the stuff to represent the object in the renderer
	CInstance					*instance;
	ts_PatchSOM					*som;
	Graphics4x4					worldToLocal;
	bool InitializeModel(void);

	// Our texture context
	float							uWrap[DISPHF_MAXTEXTURES];
	float							vWrap[DISPHF_MAXTEXTURES];
	ts_FullTextureContext	*textureContext;
	int							textureLayers;

	// Do we neednormals
	bool							needNormals;
	bool							wantEngineLighting;

	// Get the uv for a specific point on the HF
	inline UVCOORD GetUV(int layer, int x, int y);

	// Build the vertex colors
	void BuildVertexColors(ts_bRGBA &c);
	inline void SetupColor(int x, int y, DHFNode &node);

	// Compute a normal for a node
	Vector3 ComputeNormal(int x, int y);

	// Stuff for collisions
	bool				collisions;
	ClipSegment		clipper;
	AABB				*aabb;
	bool				TestQuad(Segment &seg, int x, int y, Collision &collision);
	bool 				TestTriangle(Segment &seg, Vector3 &a, Vector3 &b, Vector3 &c, Collision &collision);

public:
	// We may appear in a list
	DisplayHeightField	*prev, *next;
};

#endif
