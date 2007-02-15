///////////////////////////////////////////////////////////////////////////
//
//  RenderState.h
//
//  Management of graphics pipeline state.
//
//  Ned Martin, May, 2000.
//  Nate Robins, September 2000.
//
//  Copyright (c) 2000 Avalanche Software.
//
////////////////////////////////////////////////////////////////////////////

#ifndef RENDERSTATE_H
#define RENDERSTATE_H

////////////////////////////////////////////////////////////////////////////
//  Includes
////////////////////////////////////////////////////////////////////////////

#ifdef GCN
#include "Display/GCN/GCN.h"
#endif

#include "Layers/LayersStruct.h"
#include "platform/CompilerControl.h"	// for NAMED_SECTION

#ifdef __cplusplus
extern "C" {
#endif

/******************* forward declarations ****************/

struct ts_Light;				// in g_LightMgr.h
class CLightRecord;

////////////////////////////////////////////////////////////////////////////
//  Defines
////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#define RS_MAXLIGHTS    8
#endif
#ifdef PS2
#define RS_MAXLIGHTS    4
#endif //PS2
#ifdef GCN
#define RS_MAXLIGHTS    4
#endif


/* default ambient light */

#define RS_DEFAULT_AMBIENT_RED	0.2f
#define RS_DEFAULT_AMBIENT_GREEN	0.2f
#define RS_DEFAULT_AMBIENT_BLUE	0.2f

/* buffer clearing-- if opengl or directx, set up so we can pass value directly to appropriate routines */

#define RS_COLOR_BUFFER_BIT	( 1 << 0 )
#define RS_DEPTH_BUFFER_BIT	( 1 << 1 )

/* alpha reference values for the alpha test. the alpha test is a hardware test that rejects pixels based on their
alpha value. for example, you typically set up the alpha test to reject pixels with alpha = 0, since they will have
no effect on the frame buffer. notice that this is on a PIXEL basis, not a texel basis. when bilinear filtering is on, 
the alpha values will be interpolated between the alpha values of the source texels (assuming the texture is being enlarged).

models like trees that have a lot of dense polygons. typically we can't sort the polys every frame so that they are
rendered back-to-front because they have baked into a display list. we end up with a "halo" problem. consider a tree
being rendered against a light background (like the sky). assume that the leaf polys use a texture with a 1-bit alpha.
when the edge of a leaf close to the camera gets rendered, we get a spectrum of alpha values as the alpha gets
interpolated between 0 and 255 across. the intermediate alpha values get blended with the sky in the frame buffer,
and the z-buffer gets updated. now when a leaf farther from the camera gets rendered, the z-test fails for the interpolated
alpha pixels and the front leaf gets a sky-colored halo that the back leaves can't show through.

one solution for this is to turn off bilinear filtering. then the alpha values are either 0 or 255, and we don't have any
blending of the sky color into the rendered pixels. but this is a cheesy fix. a better fix is to crank the alpha test reference
value up in this situation, so that only high-opacity pixels get rendered into the frame buffer. high opacity pixels will
blend in only a tiny bit of the sky color, so the halo problem is drastically reduced */

#define RS_ALPHA_REF_DEFAULT_VALUE	0		// the "usual" value, to reject completely transparent pixels
#define RS_ALPHA_REF_NO_HALO_VALUE	0x60	// the value to use for fighting halos

////////////////////////////////////////////////////////////////////////////
//  Types
////////////////////////////////////////////////////////////////////////////

enum
{
    RS_FALSE = 0,
    RS_TRUE  = 1,
};

////////////////////////////////////////////////////////////////////////////

enum
{
	RS_POLYGONMODE,

	RS_FRONTFACE,
	RS_CULL,

	RS_BLENDFUNC,
	RS_ZFUNC,
	RS_ZTEST,
	RS_ZWRITE,
	RS_ZBIAS,
	RS_ZDIST,

	RS_FOG,
	RS_FOGMODE,
	RS_FOGCOLOR,
	RS_FOGSTART,
	RS_FOGEND,
	RS_FOGDENSITY,

	RS_FOG_RENDER_PATH,					// ps2 only, request fog microcode for texture matrix support

	RS_FAR_CLIPPING,						// whether far clip plane is at far clip distance or pushed back really far

	RS_ALPHATEST,
	RS_ALPHAFUNC,
	RS_ALPHAREF,
	RS_FIXEDALPHA,							// just a stored value, no effect on state machine. used for ps2 env mapping

	RS_PRIMITIVE,

	RS_BASECOLOR,

	RS_LIGHTING,
	RS_SEND_LIGHTS,						//yuch-- necessary? lights always sent in immediate mode?

	RS_TEXTURE_BRIGHTNESS,
	RS_TEXTURE_CONSTANTALPHA,

	/* these are for control bit setting (in vu.c) in ps2 opengl implementation */

	RS_ENV_MAPPING,
	RS_OVERRIDE_COLOR,

	#ifdef PS2
	RS_VU_CLIPPING,						// ps2 only-- whether vu runs clipping checks or not
	#endif

	RS_SKINNING,

	// ignored on PS2, but still defined to avoid #ifdefs...
	#if defined(GCN)  ||  defined(PS2)  ||  defined(DIRECTX)
	RS_OBJECT_CONSTANTALPHA,
	#endif

	RS_NUM_STATES
};

////////////////////////////////////////////////////////////////////////////

enum
{
	RS_BLENDFUNC_UNDEFINED = 0,	// rendered as DEFAULT, included to support old-style data where u8Blend is always filled in
	RS_BLENDFUNC_DEFAULT,			// src_alpha, 1-src_alpha
	RS_BLENDFUNC_DIFFUSE,      	// dst_color, zero
	RS_BLENDFUNC_SATURATE,     	// src *   1 + dst * src
	RS_BLENDFUNC_ADD,					// src *   1 + dst *   1
	RS_BLENDFUNC_DOUBLE,				// src * dst + dst * src == 2 * src * dst
	RS_BLENDFUNC_NONE,         	// no alpha blending
	RS_BLENDFUNC_FIXEDALPHA,		// ps2 only. like default, but use a fixed alpha instead of source texel alpha (on directx/opengl acts like default)
	RS_BLENDFUNC_ADDITIVE,			// frame texel = src_texel * src_alpha + frame_texel * 1-- for additive particle effects
	RS_BLENDFUNC_SUBTRACTIVE,		// frame texel = frame texel - source texel, currently ps2 only for shadow rendering
	RS_BLENDFUNC_NEGATIVE,			// ps2/directx only.   frame texel = -frame texel.  requires a white texture to work right crossplatform
											// thats because PS2 does frame=source-frame and directx does frame=source*(1-frame)
	RS_BLENDFUNC_CONSTANTALPHA,
	RS_BLENDFUNC_GLOSS,				//	src = zero, dst = srcolor
	RS_BLENDFUNC_DOT3,				// Xbox bump maps

};

////////////////////////////////////////////////////////////////////////////

enum
{
	RS_FOGMODE_LINEAR,
	RS_FOGMODE_EXP,
	RS_FOGMODE_EXP2,
	RS_FOGMODE_NONE,
};

////////////////////////////////////////////////////////////////////////////

enum
{
	RS_ALPHAFUNC_NEVER,
	RS_ALPHAFUNC_ALWAYS,
	RS_ALPHAFUNC_EQUAL,
	RS_ALPHAFUNC_NOTEQUAL,
	RS_ALPHAFUNC_LESS,
	RS_ALPHAFUNC_LEQUAL,
	RS_ALPHAFUNC_GREATER,
	RS_ALPHAFUNC_GEQUAL,
};

////////////////////////////////////////////////////////////////////////////

enum
{
	RS_ZFUNC_NEVER,
	RS_ZFUNC_ALWAYS,
	RS_ZFUNC_EQUAL,
	RS_ZFUNC_NOTEQUAL,
	RS_ZFUNC_LESS,
	RS_ZFUNC_LEQUAL,
	RS_ZFUNC_GREATER,
	RS_ZFUNC_GEQUAL,
};

////////////////////////////////////////////////////////////////////////////

enum
{
	RS_CULL_FRONT,
	RS_CULL_BACK,
	RS_CULL_NONE,
};

////////////////////////////////////////////////////////////////////////////

enum
{
	RS_CLIPPING_NONE,
	RS_CLIPPING_CLIP,
	RS_CLIPPING_CULL,
};

////////////////////////////////////////////////////////////////////////////

enum
{
	RS_FRONTFACE_CW,
	RS_FRONTFACE_CCW,
};

////////////////////////////////////////////////////////////////////////////

enum
{
	RS_POLYGONMODE_POINT,
	RS_POLYGONMODE_LINE,
	RS_POLYGONMODE_FILL,
};

////////////////////////////////////////////////////////////////////////////

enum
{
	RS_PRIMITIVE_POINTS,
	RS_PRIMITIVE_LINES,
	RS_PRIMITIVE_LINESTRIP,
	RS_PRIMITIVE_LINELOOP,
	RS_PRIMITIVE_TRIANGLES,
	RS_PRIMITIVE_TRIANGLESTRIP,
	RS_PRIMITIVE_TRIANGLEFAN,
	RS_PRIMITIVE_QUADS,
	RS_PRIMITIVE_QUADSTRIP,
	RS_PRIMITIVE_POLYGON,
	RS_PRIMITIVE_DRAWARRAY,
	RS_PRIMITIVE_DISPLAYLIST,
};


////////////////////////////////////////////////////////////////////////////
//  Macros
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////


void RenderState__Init( void );
void RenderState__CheckForErrors( void );

////////////////////////////////////////////////////////////////////////////

// TJC - for optimization purposes, the PS2 version has been moved over to a set
// TJC - of fairly complex macros.  The purpose is to strip out those calls which
// TJC - are unnessisary, remove I$ missees, & leave the API identical to the old
// TJC - API.
#ifndef PS2
void RenderState__Request( int state, int value );
#endif

////////////////////////////////////////////////////////////////////////////

void RenderState__PrepareHardwareLights( void );
void RenderState__RegisterHardwareAmbientLight( ts_fRGB Color );
void RenderState__RegisterHardwareLight( ts_Light* pLight );
void RenderState__RemoveHardwareLight( ts_Light* pLight );
void RenderState__UpdateHardwareLightColor(int index, ts_fRGB *pColor);
bool RenderState__HardwareLightsEmpty(void);
void RenderState__RegisterLightSetHardwareLight( u32 index, CLightRecord *pLightRecord, float attenuationScale );
void _RenderState__SendHardwareLights( void );
void _RenderState__SendLightSetHardwareLights( );
void RenderState__DisableHardwareLightRange( u32 from );

////////////////////////////////////////////////////////////////////////////

void RenderState__Begin( void );
void RenderState__End  ( void );

////////////////////////////////////////////////////////////////////////////

// TJC - for optimization purposes, the PS2 version has been moved over to a set
// TJC - of fairly complex macros.  The purpose is to strip out those calls which
// TJC - are unnessisary, remove I$ missees, & leave the API identical to the old
// TJC - API.
#ifndef PS2
void RenderState__Enable( int state, int value );
void RenderState__Force( int state );
void RenderState__Lock( int state );
void RenderState__Unlock( int state );
bool RenderState__Locked( int state );
#endif
void RenderState__ForceAll( void );
void RenderState__Default( void );
void RenderState__LockAll( void );
void RenderState__UnlockAll( void );

////////////////////////////////////////////////////////////////////////////

// TJC - for optimization purposes, the PS2 version has been moved over to a set
// TJC - of fairly complex macros.  The purpose is to strip out those calls which
// TJC - are unnessisary, remove I$ missees, & leave the API identical to the old
// TJC - API.
#ifndef PS2
int   RenderState__Get( int state );
#endif
float RenderState__GetConstantAlpha( void );

////////////////////////////////////////////////////////////////////////////

// TJC - for optimization purposes, the PS2 version has been moved over to a set
// TJC - of fairly complex macros.  The purpose is to strip out those calls which
// TJC - are unnessisary, remove I$ missees, & leave the API identical to the old
// TJC - API.
#ifndef PS2
int   RenderState__Colorfv( float* color );
int   RenderState__Colorubv( unsigned char* color );
void  RenderState__Unpackfv( float* color, int packed );
void  RenderState__Unpackubv( unsigned char* color, int packed );
int   RenderState__ftoi( float value );
float RenderState__itof( int value );
#endif

////////////////////////////////////////////////////////////////////////////
// platform independent
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// Implemented per Platform (OpenGL/DirectX)
////////////////////////////////////////////////////////////////////////////

void _RenderState__Begin( int primitive );
void _RenderState__End( void );

// TJC - for optimization purposes, the PS2 version has been moved over to a set
// TJC - of fairly complex macros.  The purpose is to strip out those calls which
// TJC - are unnessisary, remove I$ missees, & leave the API identical to the old
// TJC - API.
#ifndef PS2
void _RenderState__Primitive( int value );

void _RenderState__BlendFunc( int value );
void _RenderState__ZFunc( int value );
void _RenderState__ZTest( int value );
void _RenderState__PolygonMode( int value );
void _RenderState__FrontFace( int value );
void _RenderState__Cull( int value );
void _RenderState__BaseColor( int value );
void _RenderState__FarClipping( int value );
void _RenderState__ZWrite( int value );
void _RenderState__ZBias( int value );
void _RenderState__ZDist( int value );
void _RenderState__Lighting( int value );

#ifdef DIRECTX
void _RenderState__FixedAlpha( int value );
void _RenderState__SetTFactor( float X, float Y, float Z, float W );
#endif

void _RenderState__Fog( int value );
void _RenderState__FogMode( int value );
void _RenderState__FogColor( int value );
void _RenderState__FogStart( int value );
void _RenderState__FogEnd( int value );
void _RenderState__FogDensity( int value );

void _RenderState__AlphaTest( int value );
void _RenderState__AlphaFunc( int value );
void _RenderState__AlphaRef( int value );

void _RenderState__TextureBrightness( int value );
void _RenderState__TextureConstantAlpha( int value );

void _RenderState__EnvMapping( int value );
void _RenderState__OverrideColor( int value );

void _RenderState__Skinning( int value );

#if defined(GCN)  ||  defined(DIRECTX)
void _RenderState__ObjectConstantAlpha( int value );
#endif
#endif // PS2

////////////////////////////////////////////////////////////////////////////
// Private !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
////////////////////////////////////////////////////////////////////////////

#ifndef PS2
float _RenderState__Clampf( float x, float min, float max );
void  _RenderState__Change( int state, int value );
#endif // PS2

extern int _RenderState_ActiveBegin;
extern int _RenderState_NumLights;
extern int _RenderState_LightPositionDirty;
extern float _RenderState_GlobalAmbient[ 4 ];
extern float _RenderState_ZBias;
extern float _RenderState_ZDist;
extern ts_Light* _RenderState_Lights[ RS_MAXLIGHTS ];


// this is disabled in NDEBUG for speed reasons...
#ifdef _DEBUG
#define SUPPORT_UPDATED_COUNT
#endif

typedef struct
{
	int  force;
	int  value;
#ifdef SUPPORT_UPDATED_COUNT
	int  updated;
#endif
	bool locked;

} RenderState;
extern RenderState _RenderStates[ RS_NUM_STATES ];

////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

// TJC - for optimization purposes, the PS2 version of many of these
// functions has been moved over to a set of fairly complex macros.
// The purpose is to strip out those calls which are unnessisary,
// remove I$ missees, & leave the API identical to the old API.
#ifdef PS2
#include "Display/PS2/RenderState.hpp"
#endif



#endif  // RENDERSTATE_H

