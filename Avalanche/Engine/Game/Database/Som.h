/***********************************/
/*                                 */
/* l_Som.h                         */
/* avalanche patchwork engine      */
/* visual c++ 6.0                  */
/* layers project                  */
/*                                 */
/***********************************/

/* SOM model structures. there are currently two flavors-- poly- and patch-based SOM's */

#ifndef __L_SOM_H
#define __L_SOM_H


/******************* includes ****************************/

/* system includes */

/* engine includes */

#include "Layers/LayersStruct.h"			// low-level include
#include "Game/Database/DataTypes.h"	// for dh field in ts_PatchSOM
#include "Game/Misc/Bounds.h"				// for BoundingSphere, BoundingBox fields in ts_PatchSOM

class TextureArray;
class TextureDatabase;

#if defined(__cplusplus)
extern "C" {
#endif

/******************* forward declarations ****************/

struct ts_Body;	// in g_Bone.h

/******************* defines *****************************/

/* flag bits for poly and patch SOM models */

#define SOMM_IS_LOD								0x0001		// this model is a lower LOD of another model
#define SOMM_ALPHA_SORT							0x0002		// model needs to be sorted with other alpha-containing models
//available											0x0004
//available											0x0008
#define SOMM_HAS_COLORS							0x0010		// for gamecube, som contains vertex color indices
#define SOMM_HAS_NAME							0x0020		// to protect against conflicting database versions
#define SOMM_HAS_BONES							0x0040		// this model has a model bone array after it
//available											0x0080
#define SOMM_TRI_STRIP_NORMALS				0x0100		// tri strips include normal data (follows vertex data)
//avaiable											0x0200
#define SOMM_BONES_SLAVE						0x0400		// does not output bones after the som - uses bones from a master som
#define SOMM_VISIBLE_INTERNALS				0x0800		// doesn't have an opaque perimeter: draw LOD with Z-buffer writes OFF
#define SOMM_DYNAMIC_GEOMETRY					0x1000		// model's geometry gets dynamically changed (like tire tracks, but not like animated models)
#define SOMM_NEEDS_ENV_MAPPING				0x2000		// some geometry in model is environment mapped
#define SOMM_GCN_CAN_RECEIVE_SHADOWS		0x4000		// for gamecube, whether a model has any strips marked can-receive-shadows

/* database vector type may vary depending on whether we need them quad word aligned for vu0 processing on ps2 */

#define VCT_TYPE	Vector4

/* for ps2, ADC bit marks vertices that do not generate a draw kick. i use them to splice disjoint tri-strips together
for better DMA performance */

#define ADC_BIT									0x8000

/* number of color entries in a ps2 quad word-- each color buffer dma must be quad word aligned, so we have 
to pad each tri-strip's vertex color buffer so that the next one is alogned */

#define COLORS16_PER_QUAD_WORD				8	// 16-bit, each color is 5-5-5-1
#define COLORS32_PER_QUAD_WORD				4	// 32-bit, each color is 8-8-8-8

/* length of name field in SOM */

#define SOM_NAME_LENGTH							32

/* number of textures we allow (in a SOMPatch and HashPatch), for now anyway */

#define MAX_SOM_TEXTURES_PER_PATCH			6

/* value that says that texture to use is not in current .dbl file (like white texture) */

#define TEXTURE_NOT_IN_DBL						-1

/* data types for SOM to record miscellaneous data */

#define SOM_DATA_REFLECT_BIND					0
#define SOM_DATA_ANIMATED_TEXTURE			1

/******************* macros ******************************/

/* macro for cutting down rgba range for ps2-- 255 becomes 0x80 */

#define PS2_HALF_COLOR(Color)	(u8) (((int) Color + 1) / 2)

/******************* structures **************************/

/***************************************** patch-based SOM's ***********************************/

/* for usControlVertexIndex field */

#define NO_CONTROL_VERTEX			65535

/* for sTextureIndex array in patch */

#define NO_TEXTURE					-1
#define LIGHT_MAP_TEXTURE			-2

/* for sColorIndex field in patch */

#define NO_COLOR						-1

/* a patch consists of vertices, control vertices, normals, and has some other appearance data */

/* next 4 flags serve a dual purpose. for an original patch, they signal whether edge is perfectly straight. during tesselation,
they signal that edge has passed straightness tolerance test. if original patch is not deformed, original patch flags are
passed to tesselation routine to avoid flatness test on straight edges */

#define PATCH_TOP_EDGE_STRAIGHT					0x0001		// top edge is considered perfectly straight in tesselation
#define PATCH_RIGHT_EDGE_STRAIGHT				0x0002		// right edge is considered perfectly straight in tesselation
#define PATCH_BOTTOM_EDGE_STRAIGHT				0x0004		// bottom edge is considered perfectly straight in tesselation
#define PATCH_LEFT_EDGE_STRAIGHT					0x0008		// left edge is considered perfectly straight in tesselation
#define PATCH_ANY_EDGE_STRAIGHT					(PATCH_TOP_EDGE_STRAIGHT | PATCH_RIGHT_EDGE_STRAIGHT | PATCH_BOTTOM_EDGE_STRAIGHT | PATCH_LEFT_EDGE_STRAIGHT)

/* shape of edge spline ("s" vs "c" curve) */

#define PATCH_TOP_EDGE_S_SHAPED					0x0010		// top edge is s-shaped, use alternate flatness/normal calculations
#define PATCH_RIGHT_EDGE_S_SHAPED				0x0020		// right edge is s-shaped, use alternate flatness/normal calculations
#define PATCH_BOTTOM_EDGE_S_SHAPED				0x0040		// bottom edge is s-shaped, use alternate flatness/normal calculations
#define PATCH_LEFT_EDGE_S_SHAPED					0x0080		// left edge is s-shaped, use alternate flatness/normal calculations
#define PATCH_ANY_EDGE_S_SHAPED					(PATCH_TOP_EDGE_S_SHAPED | PATCH_RIGHT_EDGE_S_SHAPED | PATCH_BOTTOM_EDGE_S_SHAPED | PATCH_LEFT_EDGE_S_SHAPED)

/* more patch-related flags */

#define PATCH_3_POINT								0x0100		// 3-point patch
#define PATCH_IS_POLY								0x0200		// all edges are straight, patch is a poly
#define PATCH_TWO_SIDED								0x0400		// patch is visible from both sides, never gets backface culled
#define PATCH_ALPHA_SORT							0x0800		// first texture is cookie-cut and has blendable alpha (not just 0 and 255)-- needs to be put in sorted alpha list
#define PATCH_OVERRIDE_COLOR						0x1000		// override patch base color with instance color

/* for use during the export process-- if a patch is converted into strips, 
#define PATCH_STRIPPED								0x2000		// used for patch does not tessellate at default collision split zoom value (can be treated as a single quad for collision)

/* next flags are used only by tesselation code at run-time. flag word for tesselation is long, so put them in upper 2 bytes.
they are generally flags for the entire patch SOM (model), not for individual patches */

#define PATCH_RENDER_DO_LIGHTING					0x00010000	// patch needs real-time lighting
#define PATCH_RENDER_TRANSFORM_NORMALS			0x00020000	// transform models normals (for subdividing normals or for backface culling)
#define PATCH_TOP_RIGHT_MIDPOINT_COLOR			0x00080000	// top or right edge should use patch's pre-calculated midpoint color when subdivided
#define PATCH_BOTTOM_LEFT_MIDPOINT_COLOR		0x00100000	// bottom or left edge should use patch's pre-calculated midpoint color when subdivided
#define PATCH_RENDER_HAS_VERTEXCOLORS			0x00400000	// patch has a valid color settings for each vertex

/* these flags are used by export tool for dividing UV coords more accurately (since data is created off-line). they could
be overloaded for engine use */

#define PATCH_TOP_EDGE_PROPORTIONAL_UV			0x10000000	// top edge uv's should be divided proportionally
#define PATCH_RIGHT_EDGE_PROPORTIONAL_UV		0x20000000	// right edge uv's should be divided proportionally
#define PATCH_BOTTOM_EDGE_PROPORTIONAL_UV		0x40000000	// bottom edge uv's should be divided proportionally
#define PATCH_LEFT_EDGE_PROPORTIONAL_UV		0x80000000	// left edge uv's should be divided proportionally

/* correspondence between control vertex id's in diagrams and position in patch's usControlVertexIndex array */

#define CV0		0
#define CV1		1
#define CV2		2
#define CV5		3
#define CV6		4
#define CV9		5
#define CV10	6
#define CV11	7

/* entries in pre-calculated s-shaped normal/color lists */

#define S_SHAPED_TOP_EDGE			0
#define S_SHAPED_RIGHT_EDGE		1
#define S_SHAPED_BOTTOM_EDGE		2
#define S_SHAPED_LEFT_EDGE			3
#define S_SHAPED_PATCH_MIDPOINT	4
#define S_SHAPED_MAX_ENTRIES		5

/* flags for texture context record */

#define TEXTURE_IS_DIFFUSE_MAP		0x01
#define TEXTURE_IS_ENVIRONMENT_MAP	0x02
#define TEXTURE_HAS_ALPHA				0x04
#define TEXTURE_HAS_ZBIAS				0x08
#define TEXTURE_USE_WHITE_TEXTURE	0x10	// for ps2-- to support environment maps over a base color
#define TEXTURE_BASE_COLOR_SHOWS		0x20	// base color shows through alpha channel-- in hash terms, "cookie-cut" is off
#define TEXTURE_ALPHA_REF				0x40	// use a high alpha reference value for this texture to eliminate halo effect
#define TEXTURE_MULTITEXTURE_OK		0x80	// ok to render in multi-texture

/* The highest value of MaterialIndex is considered uninitialized */
#define INVALID_MATERIALINDEX		0xff
#define MAX_OBBS_PER_MATERIAL		8

struct ts_MaterialBitfield
{
	u16	MaterialIndex			: 8,
			IsTerrain				: 1,
			IsOBB						: 1,
			OBBIndex					: 3,	// if this changes, update MAX_OBBS_PER_MATERIAL, above
			IsVisibleInternals	: 1,
			Pad						: 2;
};

/* a texture context. this allows quick setting of the appropriate texture states. for development, we want this to be
affected by switches (e.g. lighting on/off), so when one of these things change we have to go back through and reset the
values */

class Texture;
struct ts_TextureLayer
{
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
	s16	s16TextureIndex;	// signed because unresolved bind can be -1 (no texture)
	u8		u8Method;			// TS_METHOD_REPLACE, TS_METHOD_MODULATE, TS_METHOD_DECAL, TS_METHOD_CONSTANTALPHA, TS_METHOD_INTERPOLATE
	u8		u8Blend;				// RS_BLENDFUNC_DEFAULT, RS_BLENDFUNC_DIFFUSE
	u8		u8WrapU;				// TS_WRAP_CLAMP, TS_WRAP_REPEAT
	u8		u8WrapV;				// TS_WRAP_CLAMP, TS_WRAP_REPEAT
	u8		u8Filter;			// TS_FILTER_POINTSAMPLE, TS_FILTER_BILINEAR, TS_FILTER_NEAREST_MIPMAP_NEAREST, TS_FILTER_NEAREST_MIPMAP_LINEAR, TS_FILTER_LINEAR_MIPMAP_NEAREST, TS_FILTER_LINEAR_MIPMAP_LINEAR
	u8		u8Flags;				// TEXTURE_IS_DIFFUSE_MAP, etc

	TextureDatabase* textureDatabase;
	#define TEXTURE_LAYER_NAME_LENGTH 32
	char	TextureName[TEXTURE_LAYER_NAME_LENGTH];	// for debugging
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
};

struct ts_TextureContext
{
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	

	ts_bRGBA				BaseColor;
	int					TextureCount;
	ts_TextureLayer	TextureLayer[1];	// at least one texture layer (actually, might be 0 in texture context chunk)

/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
};

struct ts_FullTextureContext
{
	ts_bRGBA				BaseColor;
	int					TextureCount;
	ts_TextureLayer	TextureLayer[MAX_SOM_TEXTURES_PER_PATCH];	// the maximum number of texture layers
};

/* a patch, indexed style */
//yuch-- variable length??? PATCH_IS_POLY doesn't need control vertices or extent indices, 3-point patches don't need as much
// data, patch may have less than MAX_SOM_TEXTURES_PER_PATCH textures

struct ts_Patch
{
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	

	/* these fields are grouped together with an eye towards sending them off as a "global texture info" packet to */
	/* accompany a filled-in, ready-for-tesselation patch */

	unsigned short			usFlags;

	/* the "texture context" for the patch-- may be multiple layers */

	union
	{
		u32					u32TextureContextIndex;
		ts_TextureContext *pTextureContext;
	};

	/* the material field is for non-rendering stuff, like collision */

	ts_MaterialBitfield	MaterialBitfield;

	/* the indices into the vertex/control vertex/etc buffers */

	unsigned short			usVertexIndex[4];
	unsigned short			usNormalIndex[4];
	unsigned short			usControlVertexIndex[8];
	unsigned short			usUV0Index[MAX_SOM_TEXTURES_PER_PATCH];
	unsigned short			usUV1Index[MAX_SOM_TEXTURES_PER_PATCH];
	unsigned short			usUV2Index[MAX_SOM_TEXTURES_PER_PATCH];
	unsigned short			usUV3Index[MAX_SOM_TEXTURES_PER_PATCH];

	/* the bounding box for a patch can be built by taking the average of the corners and adding x-, y-, and z-extents */
	/* to it. these indices are into a look-up table of floats */

	unsigned char			ucBoundingBoxXIndex;
	unsigned char			ucBoundingBoxYIndex;
	unsigned char			ucBoundingBoxZIndex;

	/* opacity, 0..100-- 100 is opaque */

	u8							u8Opacity;

	/* pre-calculated normals for s-shaped edges-- potentially one for each edge and the patch midpoint */

	unsigned short			usSShapedNormalIndex[S_SHAPED_MAX_ENTRIES];

	/* name for easier debugging */
	#define PATCH_NAME_LENGTH 8
	char Name[PATCH_NAME_LENGTH];

/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
};

/* flags for ts_TriStripHeader.u8Flags */

#define TRI_STRIP_HAS_NORMALS				0x01
#define TRI_STRIP_TWO_SIDED				0x02
#define TRI_STRIP_TEXTURE_LIT				0x04
#define TRI_STRIP_OVERRIDE_COLOR			0x08		// override patch base color with instance color
#define TRI_STRIP_ENVIRONMENT_MAP		0x10
#define TRI_STRIP_GCN_RECEIVE_SHADOWS	0x20		// for gamecube, to only allow shadows for mostly-vertical-face-normal geometry

/* a triangle strip is self-explanatory */

struct ts_TriStripHeader
{
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	

	union
	{
		u32							u32NextOffset;	
		ts_TriStripHeader			*pNext;			// pointer to next tri strip
	};
	union
	{
		u32					u32TextureContextIndex;
		ts_TextureContext *pTextureContext;
	};
	u32						u32BoneIndex;			// if upper 3 bytes are non-zero, then this is 4 bone indices for vertex weighting
	u32						u32ID;					// debugging only, can be used to trace data back to source patches
	u16						u16PointCount;
	u16						u16SkipPointCount;	// number of points to skip while processing data. typically == u16PointCount, but could be greater for user-created data
	u8							u8Opacity;				// opacity for environment maps, 0..100, 100 is opaque
	u8							u8Flags;
//yuch-- this record must be quad-word (16 byte) aligned for PS2, since vertex data (and other data) starts immediately after the header
#if defined(WIN32) || defined(PS2)
	u32							u32StoredTexContextIndex;
	u8							Pad2[2];	// if this reaches 16 or more, can chop a quad word off record
#else
	u8							Pad2[10];
#endif //ifdef win32 || ps2

/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
};

class ShaderInst;

//this record must be 32 aligned for GameCube, since DisplayList starts immediately after the header
struct ts_GCNTriStripHeader
{
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	

	union
	{
		u32								u32NextOffset;	
		ts_GCNTriStripHeader			*pNext;			// pointer to next tri strip
	};
	union
	{
		u32					u32TextureContextIndex;
		ts_TextureContext *pTextureContext;
	};
	ShaderInst				*pShader;
	u32						u32BoneIndex;			// if upper 3 bytes are non-zero, then this is 4 bone indices for vertex weighting
	u32						u32ID;					// debugging only, can be used to trace data back to source patches
	u32						u32DisplayListSize;
	u16						u16VertexCount;
	u16						u16TriangleCount;
	u8							u8Opacity;				// opacity for environment maps, 0..100, 100 is opaque
	u8							u8Flags;
	u8							au8Pad[2];
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
};

/* for vertex weighting, treat the 32-bit bone index as an array of bone indices. the bone indices are sorted within
this array, so if a tri-strip is multi-vertex, then one of the upper 3 bytes is guaranteed non-0, and this is how we can
identify vertex weighting-- that's why the test value is 0xffffff00 */

#define VERTEX_WEIGHTING			0xffffff00	// if any bit in upper 3 bytes is set, then it's multiple bone indices
struct ts_VertexWeightingBones
{
	u8		u8BoneIndex[4];
};

/* for ps2 display lists. animated models have a display list per bone. instead of allocating memory in the instance
structure for the required number of display list records, i have a pool of records to be used for animated models. as
each bone's display list is built up, it grabs one of the records from the pool. in this case, the instance's display
list record works as follows: the pDisplayList pointer points to the first pool record. the u32QuadWordCount field is
the number of display lists to render (which equals the number of bones in the model). the u16TriStripCount and 
u16PolyCount fields are the totals for all display lists. then, in the bone pool display list records, the u16TriStripCount
field is the bone index for that display list. a bit ugly, but there you go */

/* this one is the entry in the bone display list pool */

struct ts_BoneDisplayListRecord
{
	char	*pDisplayList;
	/*** leave u16QuadWordCount here, please */
	u16	u16QuadWordCount;		// size of display list in quad words (a quad word is 16 bytes)
	u8		u8TextureLitMode;		// for supporting texture-lit stuff
	u8		u8Pad;
	u32	u32BoneIndex;
};

/****************** these next two must match!!! the unioned version was just too ugly ************************/

struct ts_DisplayListRecord
{
	char	*pDisplayList;
	u16	u16QuadWordCount;		// size of display list in quad words (a quad word is 16 bytes)
	u16	u16PolyCount;			// stats-- once display list is built, i have no way of getting stats
};

/* this one is the instance's display list record for an animated model */

struct ts_AnimatedDisplayListRecord
{
	ts_BoneDisplayListRecord *pBoneDisplayListRecords;	// pointer into the bone display list pool
	u16	u16BoneDisplayListCount;							// number of bone display lists-- number of consecutive records in bone display list pool
	u16	u16PolyCount;			// stats-- once display list is built, i have no way of getting stats
};

/* patch-based SOM model */

struct ts_PatchSOM
{
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	

	/* all SOM models start off with a data header, a bounding box, a bounding sphere, and a bone structure pointer for animation */

	ts_FlagsDataHeader	dh;			// data header, includes type, sub type, flags, priority-- MUST BE FIRST-- see comments in datatypes.h
	ts_BoundingSphere		BoundingSphere;
	ts_BoundingBox			BoundingBox;
	ts_Body					*pBody;		// only used for animating models

	/* name for easier debugging */
	char Name[SOM_NAME_LENGTH];

	/* vertex and patches. incoming data has offset fields set. _FixPatchSOM converts to pointer fields */

	u32						u32VertexCount;
	union
	{
		u32					u32VertexOffset;
		VCT_TYPE				*pVertexBuffer;
	};

	u32						u32PatchCount;
	union
	{
		u32					u32PatchOffset;
		ts_Patch				*pPatchBuffer;
	};

	/* texture set that model uses-- needed for dynamic texture sets. void because this file can't include */
	/* d_TextureMgr.h without causing a massive circular dependency */

	union {
		TextureDatabase*		pTextureDatabase;
		u32						u32TextureSubSetIndex; // TJC - which subset to use in this set.
	};
	TextureArray*			pTextureArray;

	/* space to record display lists for ps2 */

	ts_DisplayListRecord	DisplayListRecord;

	/* size of vertex color buffer for tri strips. basically, the total number of vertices in all */
	/* tri strips, adjusted for quad word-alignment on ps2 */

	u32						u32TriStripVertexColorCount;

	/* buffers */

	u32						u32TriStripCount;
	union
	{
		u32					u32TriStripOffset;
		ts_TriStripHeader	*pTriStripBuffer;
	};

	u32						u32ControlVertexCount;

	union
	{
		u32					u32ControlVertexOffset;	
		VCT_TYPE				*pControlVertexBuffer;
	};

	u32						u32NormalCount;
	union
	{
		u32					u32NormalOffset;
		VCT_TYPE				*pNormalBuffer;
	};

	/**************** this must be last for _GetSOMEnd routine to work ***************/

	u32						u32UVCount;
	union
	{
		u32					u32UVOffset;
		Vector2				*pUVBuffer;
	};

/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
};

#ifdef WIN32
/* a collision SOM is just geometry data for generating collision data with terraintoola and obbworld. this is the model
data found in a .dbc file, which is generated at the same time as a renderable .dbl file */

#define SOURCE_PATCH_NAME_LENGTH 16
struct ts_CollisionPoly
{
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	

	u16 u16VertexIndex[4];
	u16 u16NormalIndex[4];
	u16 u16VertexCount;	// 3 or 4
	ts_MaterialBitfield MaterialBitfield;
	char SourcePatchName[SOURCE_PATCH_NAME_LENGTH];

/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
};

struct ts_CollisionSOM
{
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	

	/* all SOM models start off with a data header, a bounding box, a bounding sphere, and a bone structure pointer for animation */

	ts_FlagsDataHeader	dh;			// data header, includes type, sub type, flags, priority-- MUST BE FIRST-- see comments in datatypes.h
	ts_BoundingSphere		BoundingSphere;
	ts_BoundingBox			BoundingBox;

	/* name for easier debugging */

	char Name[SOM_NAME_LENGTH];

	/* vertex and normal buffers */

	u32						u32VertexCount;
	union
	{
		u32					u32VertexOffset;
		Vector3Packed		*pVertexBuffer;
	};

	u32						u32NormalCount;
	union
	{
		u32					u32NormalOffset;
		Vector3Packed		*pNormalBuffer;
	};

	/* poly records */

	u32						u32PolyCount;
	union
	{
		u32					u32PolyOffset;
		ts_CollisionPoly	*pPolyBuffer;
	};

/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
};
#endif //WIN32


#if defined(__cplusplus)
}
#endif

#endif //__L_SOM_H
