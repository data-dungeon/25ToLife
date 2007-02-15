///////////////////////////////////////////////////////////////////////////
// RenderState.h - for the PS2
// 
//  Management of graphics pipeline state.
//
//  Ned Martin, May, 2000.
//  Nate Robins, September 2000.
//  Tyler Colbert, April 2002.
//
//  Copyright (c) 2000,2002 Avalanche Software.
//
///////////////////////////////////////////////////////////////////////////

#include "Display/PS2/GSState.h"
#include "Game/Managers/LightMgr.h"
#include "Display/Common/SceneView.h"
#include "Game/Managers/SceneMgr.h"
#include <eestruct.h>

//
// Ok, the basic idea here is to translate calls like:
// RenderState__Request(RS_ZFUNC, value) into calls like:
// RenderState__Request_RS_ZFUNC(value) which can then have inline
// implimentations.
//
// as a secondary idea, we may move the renderstate data into the
// scratchpad during rendering to help aleviate the D$ impact of it
// all...
//

//
// This is an additional RenderState__* function that was PS2 only and
// for cleanliness (of RenderState.h), I moved it here.. perhaps I
// shouldn't have?
//
extern void RenderState__SetParticleLights(void);

//
// To make this work, we need to have the basic functions (ie
// RenderState__Request) be represented by macros instead of functions
// because we will be using the preprocessor to munge the names
// together.
//

#define RenderState__Request( state, value )	  RenderState__Request_ ## state (value)
#define RenderState__Enable( state, value )	  RenderState__Enable_ ## state (value)
#define RenderState__Force( state )				  RenderState__Force_ ## state ()
#define RenderState__Lock( state )				  RenderState__Lock_ ## state ()
#define RenderState__Unlock( state )			  RenderState__Unlock_ ## state ()
#define RenderState__Locked( state )			  RenderState__Locked_ ## state ()
#define RenderState__Get( state )				  RenderState__Get_ ## state ()


//
// Ok, now all calls to those functions will resolve to the required
// RenderState__*_RS_* inline functions.  Therefore we can now build
// versions of those funtions that do their work as fast as possible
// and hopefully fairly cleanly...
//

///////////////////////////////////////////////////////////////////////////
// Some misc funtions inlined while I'm here...
// they come early on in the file so that GCC can resolve the inlining
// of them for the functions later on in the file.
///////////////////////////////////////////////////////////////////////////

// Conversions /////////////////////////////////////////////////////////////

inline int RenderState__ftoi( float value )
{
	return Math::FloatAsInt(value);
}

////////////////////////////////////////////////////////////////////////////

inline float RenderState__itof( int value )
{
	return Math::IntAsFloat(value);
}

////////////////////////////////////////////////////////////////////////////

inline float _RenderState__Clampf( float x, float min, float max )
{
	float f;
	asm("max.s %0, %1, %2" : "=f" (f) : "f" (min), "f" (x));
	asm("min.s %0, %1, %2" : "=f" (f) : "f" (max), "f" (f));
	return f;
}

////////////////////////////////////////////////////////////////////////////

// these could probably use asm versions
inline int RenderState__Colorfv( float* color )
{
	unsigned char r = ( unsigned char )_RenderState__Clampf( color[ 0 ] * 255, 0, 255 );
	unsigned char g = ( unsigned char )_RenderState__Clampf( color[ 1 ] * 255, 0, 255 );
	unsigned char b = ( unsigned char )_RenderState__Clampf( color[ 2 ] * 255, 0, 255 );
	unsigned char a = ( unsigned char )_RenderState__Clampf( color[ 3 ] * 255, 0, 255 );

	unsigned int packed = ( a << 24 ) | ( b << 16 ) | ( g << 8 ) | ( r << 0 );

	return packed;
}

////////////////////////////////////////////////////////////////////////////

// these could probably use asm versions
inline int RenderState__Colorubv( unsigned char* color )
{
	unsigned char r = color[ 0 ];
	unsigned char g = color[ 1 ];
	unsigned char b = color[ 2 ];
	unsigned char a = color[ 3 ];

	unsigned int packed = ( a << 24 ) | ( b << 16 ) | ( g << 8 ) | ( r << 0 );

	return packed;
}

////////////////////////////////////////////////////////////////////////////

// these could probably use asm versions
inline void RenderState__Unpackfv( float* color, int packed )
{
	float a = ( ( packed >> 24 ) & 0xFF ) / 255.0f;
	float b = ( ( packed >> 16 ) & 0xFF ) / 255.0f;
	float g = ( ( packed >>  8 ) & 0xFF ) / 255.0f;
	float r = ( ( packed >>  0 ) & 0xFF ) / 255.0f;

	color[ 0 ] = r;
	color[ 1 ] = g;
	color[ 2 ] = b;
	color[ 3 ] = a;
}

////////////////////////////////////////////////////////////////////////////

// these could probably use asm versions
inline void RenderState__Unpackubv( unsigned char* color, int packed )
{
	unsigned char a = ( packed >> 24 ) & 0xFF;
	unsigned char b = ( packed >> 16 ) & 0xFF;
	unsigned char g = ( packed >>  8 ) & 0xFF;
	unsigned char r = ( packed >>  0 ) & 0xFF;

	color[ 0 ] = r;
	color[ 1 ] = g;
	color[ 2 ] = b;
	color[ 3 ] = a;
}

//
// Now because I'm not fond of maintaining 100s of functions, most of
// them will go through the following standard macros to be created.
// This way, if a change is required to all of them, you only need
// change it in one place (except for the functions that have
// exceptions in them, which will have to be handled manually).
//

///////////////////////////////////////////////////////////////////////////

#ifdef SUPPORT_UPDATED_COUNT
#define MARK_UPDATED(s) (s)->updated++
#else
#define MARK_UPDATED(s) ((void)0)
#endif

// this macro is used if we just ignore the renderstate
#define NOP_RenderState_Request_Method( state )			\
inline void RenderState__Request_ ## state(int value)	\
{																		\
}

// useful for getting the basic functionality of a renderstate request
// call, then just adding in the customizations
#define CORE_RenderState_Request( state )				\
	RenderState* s = &( _RenderStates[ state ] );	\
	if(s->locked) return;									\
	if(!s->force && s->value == value) return;		\
	s->value = value;											\
	s->force = RS_FALSE;										\
	MARK_UPDATED(s);


// this macro is used if we just stuff the value into a variable
// somewhere for later use.  I still need to make sure that we even
// need to do this much... I think there is some here that we could
// probably skip.
// Force is pretty pointless if all we do is store the value... so we
// will just ignore it for now...
//	s->force = RS_FALSE;
#define STD_RenderState_Request_Method( state )						\
inline void RenderState__Request_ ## state(int value)				\
{																					\
    RenderState* s = &( _RenderStates[ state ] );					\
	 if(s->locked) return;													\
	 s->value = value;														\
	 MARK_UPDATED(s);															\
}

///////////////////////////////////////////////////////////////////////////

#define STD_RenderState_Force_Method( state ) NOP_RenderState_Force_Method(state)
#define NOP_RenderState_Force_Method( state )	\
inline void RenderState__Force_ ## state()		\
{																\
}
#define REAL_RenderState_Force_Method( state )		\
inline void RenderState__Force_ ## state()			\
{																	\
	RenderState* s = &( _RenderStates[ state ] );	\
	if(s->locked) return;									\
	s->force = RS_TRUE;										\
}

///////////////////////////////////////////////////////////////////////////

#define STD_RenderState_Lock_Method( state )		\
inline void RenderState__Lock_ ## state()			\
{																\
	_RenderStates[ state ].locked = TRUE;			\
}
#define NOP_RenderState_Lock_Method( state )		\
inline void RenderState__Lock_ ## state()			\
{																\
}

///////////////////////////////////////////////////////////////////////////

#define STD_RenderState_Unlock_Method( state )	\
inline void RenderState__Unlock_ ## state()		\
{																\
	_RenderStates[ state ].locked = FALSE;			\
}
#define NOP_RenderState_Unlock_Method( state )	\
inline void RenderState__Unlock_ ## state()		\
{																\
}

///////////////////////////////////////////////////////////////////////////

#define STD_RenderState_Locked_Method( state )	\
inline bool RenderState__Locked_ ## state()		\
{																\
	return _RenderStates[ state ].locked;			\
}
#define NOP_RenderState_Locked_Method( state )	\
inline bool RenderState__Locked_ ## state()		\
{																\
	return FALSE;											\
}

///////////////////////////////////////////////////////////////////////////

#define STD_RenderState_Get_Method( state )		\
inline int RenderState__Get_ ## state()			\
{																\
	return _RenderStates[ state ].value;			\
}
#define NOP_RenderState_Get_Method( state, value )	\
inline int RenderState__Get_ ## state()				\
{																	\
	return value;												\
}

///////////////////////////////////////////////////////////////////////////
// RenderState__Enable
// first the generic ones, special cases should be commented out.
// should come in RS_* enum order so that its easy to see which are missing
///////////////////////////////////////////////////////////////////////////

// Couldn't find any references to RenderState__Enable... cant
// determine what its for... im just ignoring it for now...:)

///////////////////////////////////////////////////////////////////////////
// RenderState__Lock
// first the generic ones, special cases should be commented out.
// should come in RS_* enum order so that its easy to see which are missing
///////////////////////////////////////////////////////////////////////////
NOP_RenderState_Lock_Method(RS_POLYGONMODE);
NOP_RenderState_Lock_Method(RS_FRONTFACE);
STD_RenderState_Lock_Method(RS_CULL);
STD_RenderState_Lock_Method(RS_BLENDFUNC);
STD_RenderState_Lock_Method(RS_ZFUNC);
STD_RenderState_Lock_Method(RS_ZTEST);
STD_RenderState_Lock_Method(RS_ZWRITE);
NOP_RenderState_Lock_Method(RS_ZBIAS);
NOP_RenderState_Lock_Method(RS_ZDIST);
STD_RenderState_Lock_Method(RS_FOG);
STD_RenderState_Lock_Method(RS_FOGMODE);
STD_RenderState_Lock_Method(RS_FOGCOLOR);
STD_RenderState_Lock_Method(RS_FOGSTART);
STD_RenderState_Lock_Method(RS_FOGEND);
STD_RenderState_Lock_Method(RS_FOGDENSITY);
STD_RenderState_Lock_Method(RS_FOG_RENDER_PATH);
STD_RenderState_Lock_Method(RS_FAR_CLIPPING);
STD_RenderState_Lock_Method(RS_ALPHATEST);
STD_RenderState_Lock_Method(RS_ALPHAFUNC);
STD_RenderState_Lock_Method(RS_ALPHAREF);
STD_RenderState_Lock_Method(RS_FIXEDALPHA);
STD_RenderState_Lock_Method(RS_PRIMITIVE);
STD_RenderState_Lock_Method(RS_BASECOLOR);
STD_RenderState_Lock_Method(RS_LIGHTING);
// we should error out if someone tries to lock this value.
//STD_RenderState_Lock_Method(RS_SEND_LIGHTS);
NOP_RenderState_Lock_Method(RS_TEXTURE_BRIGHTNESS);
STD_RenderState_Lock_Method(RS_TEXTURE_CONSTANTALPHA);
NOP_RenderState_Lock_Method(RS_ENV_MAPPING);
NOP_RenderState_Lock_Method(RS_OVERRIDE_COLOR);
STD_RenderState_Lock_Method(RS_VU_CLIPPING);
NOP_RenderState_Lock_Method(RS_SKINNING);
NOP_RenderState_Lock_Method(RS_OBJECT_CONSTANTALPHA);


///////////////////////////////////////////////////////////////////////////
// RenderState__Unlock
// first the generic ones, special cases should be commented out.
// should come in RS_* enum order so that its easy to see which are missing
///////////////////////////////////////////////////////////////////////////
NOP_RenderState_Unlock_Method(RS_POLYGONMODE);
NOP_RenderState_Unlock_Method(RS_FRONTFACE);
STD_RenderState_Unlock_Method(RS_CULL);
STD_RenderState_Unlock_Method(RS_BLENDFUNC);
STD_RenderState_Unlock_Method(RS_ZFUNC);
STD_RenderState_Unlock_Method(RS_ZTEST);
STD_RenderState_Unlock_Method(RS_ZWRITE);
NOP_RenderState_Unlock_Method(RS_ZBIAS);
NOP_RenderState_Unlock_Method(RS_ZDIST);
STD_RenderState_Unlock_Method(RS_FOG);
STD_RenderState_Unlock_Method(RS_FOGMODE);
STD_RenderState_Unlock_Method(RS_FOGCOLOR);
STD_RenderState_Unlock_Method(RS_FOGSTART);
STD_RenderState_Unlock_Method(RS_FOGEND);
STD_RenderState_Unlock_Method(RS_FOGDENSITY);
STD_RenderState_Unlock_Method(RS_FOG_RENDER_PATH);
STD_RenderState_Unlock_Method(RS_FAR_CLIPPING);
STD_RenderState_Unlock_Method(RS_ALPHATEST);
STD_RenderState_Unlock_Method(RS_ALPHAFUNC);
STD_RenderState_Unlock_Method(RS_ALPHAREF);
STD_RenderState_Unlock_Method(RS_FIXEDALPHA);
STD_RenderState_Unlock_Method(RS_PRIMITIVE);
STD_RenderState_Unlock_Method(RS_BASECOLOR);
STD_RenderState_Unlock_Method(RS_LIGHTING);
// we should error out if someone tries to unlock this value.
//STD_RenderState_Unlock_Method(RS_SEND_LIGHTS);
NOP_RenderState_Unlock_Method(RS_TEXTURE_BRIGHTNESS);
STD_RenderState_Unlock_Method(RS_TEXTURE_CONSTANTALPHA);
NOP_RenderState_Unlock_Method(RS_ENV_MAPPING);
NOP_RenderState_Unlock_Method(RS_OVERRIDE_COLOR);
STD_RenderState_Unlock_Method(RS_VU_CLIPPING);
NOP_RenderState_Unlock_Method(RS_SKINNING);
NOP_RenderState_Unlock_Method(RS_OBJECT_CONSTANTALPHA);



///////////////////////////////////////////////////////////////////////////
// RenderState__Locked
// first the generic ones, special cases should be commented out.
// should come in RS_* enum order so that its easy to see which are missing
///////////////////////////////////////////////////////////////////////////
NOP_RenderState_Locked_Method(RS_POLYGONMODE);
NOP_RenderState_Locked_Method(RS_FRONTFACE);
STD_RenderState_Locked_Method(RS_CULL);
STD_RenderState_Locked_Method(RS_BLENDFUNC);
STD_RenderState_Locked_Method(RS_ZFUNC);
STD_RenderState_Locked_Method(RS_ZTEST);
STD_RenderState_Locked_Method(RS_ZWRITE);
NOP_RenderState_Locked_Method(RS_ZBIAS);
NOP_RenderState_Locked_Method(RS_ZDIST);
STD_RenderState_Locked_Method(RS_FOG);
STD_RenderState_Locked_Method(RS_FOGMODE);
STD_RenderState_Locked_Method(RS_FOGCOLOR);
STD_RenderState_Locked_Method(RS_FOGSTART);
STD_RenderState_Locked_Method(RS_FOGEND);
STD_RenderState_Locked_Method(RS_FOGDENSITY);
STD_RenderState_Locked_Method(RS_FOG_RENDER_PATH);
STD_RenderState_Locked_Method(RS_FAR_CLIPPING);
STD_RenderState_Locked_Method(RS_ALPHATEST);
STD_RenderState_Locked_Method(RS_ALPHAFUNC);
STD_RenderState_Locked_Method(RS_ALPHAREF);
STD_RenderState_Locked_Method(RS_FIXEDALPHA);
STD_RenderState_Locked_Method(RS_PRIMITIVE);
STD_RenderState_Locked_Method(RS_BASECOLOR);
STD_RenderState_Locked_Method(RS_LIGHTING);
// we should error out if someone tries to lock this value.
//STD_RenderState_Locked_Method(RS_SEND_LIGHTS);
NOP_RenderState_Locked_Method(RS_TEXTURE_BRIGHTNESS);
STD_RenderState_Locked_Method(RS_TEXTURE_CONSTANTALPHA);
NOP_RenderState_Locked_Method(RS_ENV_MAPPING);
NOP_RenderState_Locked_Method(RS_OVERRIDE_COLOR);
STD_RenderState_Locked_Method(RS_VU_CLIPPING);
NOP_RenderState_Locked_Method(RS_SKINNING);
NOP_RenderState_Locked_Method(RS_OBJECT_CONSTANTALPHA);


///////////////////////////////////////////////////////////////////////////
// RenderState__Get
// first the generic ones, special cases should be commented out.
// should come in RS_* enum order so that its easy to see which are missing
///////////////////////////////////////////////////////////////////////////
NOP_RenderState_Get_Method(RS_POLYGONMODE, RS_POLYGONMODE_FILL);
NOP_RenderState_Get_Method(RS_FRONTFACE, RS_FRONTFACE_CCW);
STD_RenderState_Get_Method(RS_CULL);
STD_RenderState_Get_Method(RS_BLENDFUNC);
STD_RenderState_Get_Method(RS_ZFUNC);
STD_RenderState_Get_Method(RS_ZTEST);
STD_RenderState_Get_Method(RS_ZWRITE);
NOP_RenderState_Get_Method(RS_ZBIAS, 0);
NOP_RenderState_Get_Method(RS_ZDIST, 0);
STD_RenderState_Get_Method(RS_FOG);
STD_RenderState_Get_Method(RS_FOGMODE);
STD_RenderState_Get_Method(RS_FOGCOLOR);
STD_RenderState_Get_Method(RS_FOGSTART);
STD_RenderState_Get_Method(RS_FOGEND);
STD_RenderState_Get_Method(RS_FOGDENSITY);
STD_RenderState_Get_Method(RS_FOG_RENDER_PATH);
STD_RenderState_Get_Method(RS_FAR_CLIPPING);
STD_RenderState_Get_Method(RS_ALPHATEST);
STD_RenderState_Get_Method(RS_ALPHAFUNC);
STD_RenderState_Get_Method(RS_ALPHAREF);
STD_RenderState_Get_Method(RS_FIXEDALPHA);
STD_RenderState_Get_Method(RS_PRIMITIVE);
STD_RenderState_Get_Method(RS_BASECOLOR);
STD_RenderState_Get_Method(RS_LIGHTING);
// we should error out if someone tries to get this value.
//STD_RenderState_Get_Method(RS_SEND_LIGHTS);
NOP_RenderState_Get_Method(RS_TEXTURE_BRIGHTNESS, 0x3F800000); // 1.0f
STD_RenderState_Get_Method(RS_TEXTURE_CONSTANTALPHA);
NOP_RenderState_Get_Method(RS_ENV_MAPPING, FALSE);
NOP_RenderState_Get_Method(RS_OVERRIDE_COLOR, FALSE);
STD_RenderState_Get_Method(RS_VU_CLIPPING);
NOP_RenderState_Get_Method(RS_SKINNING, FALSE);
NOP_RenderState_Get_Method(RS_OBJECT_CONSTANTALPHA, 255);


///////////////////////////////////////////////////////////////////////////
// RenderState__Force
// first the generic ones, special cases should be commented out.
// should come in RS_* enum order so that its easy to see which are missing
///////////////////////////////////////////////////////////////////////////

// All states that use the STD version of Request should use the STD
// version of force.  Ditto with NOP, all custom versions of Request
// should probably use the REAL version of Force.
NOP_RenderState_Force_Method(RS_POLYGONMODE);
NOP_RenderState_Force_Method(RS_FRONTFACE);
STD_RenderState_Force_Method(RS_CULL);
STD_RenderState_Force_Method(RS_BLENDFUNC);
REAL_RenderState_Force_Method(RS_ZFUNC);
REAL_RenderState_Force_Method(RS_ZTEST);
REAL_RenderState_Force_Method(RS_ZWRITE);
NOP_RenderState_Force_Method(RS_ZBIAS);
NOP_RenderState_Force_Method(RS_ZDIST);
STD_RenderState_Force_Method(RS_FOG);
STD_RenderState_Force_Method(RS_FOGMODE);
REAL_RenderState_Force_Method(RS_FOGCOLOR);
STD_RenderState_Force_Method(RS_FOGSTART);
STD_RenderState_Force_Method(RS_FOGEND);
STD_RenderState_Force_Method(RS_FOGDENSITY);
STD_RenderState_Force_Method(RS_FOG_RENDER_PATH);
REAL_RenderState_Force_Method(RS_FAR_CLIPPING);
REAL_RenderState_Force_Method(RS_ALPHATEST);
REAL_RenderState_Force_Method(RS_ALPHAFUNC);
REAL_RenderState_Force_Method(RS_ALPHAREF);
REAL_RenderState_Force_Method(RS_FIXEDALPHA);
STD_RenderState_Force_Method(RS_PRIMITIVE);
STD_RenderState_Force_Method(RS_BASECOLOR);
REAL_RenderState_Force_Method(RS_LIGHTING);
REAL_RenderState_Force_Method(RS_SEND_LIGHTS);
NOP_RenderState_Force_Method(RS_TEXTURE_BRIGHTNESS);
STD_RenderState_Force_Method(RS_TEXTURE_CONSTANTALPHA);
NOP_RenderState_Force_Method(RS_ENV_MAPPING);
NOP_RenderState_Force_Method(RS_OVERRIDE_COLOR);
STD_RenderState_Force_Method(RS_VU_CLIPPING);
NOP_RenderState_Force_Method(RS_SKINNING);
NOP_RenderState_Force_Method(RS_OBJECT_CONSTANTALPHA);


///////////////////////////////////////////////////////////////////////////
// RenderState__Request
// first the generic ones, special cases should be commented out.
// should come in RS_* enum order so that its easy to see which are missing
///////////////////////////////////////////////////////////////////////////

NOP_RenderState_Request_Method(RS_POLYGONMODE);
NOP_RenderState_Request_Method(RS_FRONTFACE);
STD_RenderState_Request_Method(RS_CULL);
//STD_RenderState_Request_Method(RS_BLENDFUNC);
//STD_RenderState_Request_Method(RS_ZFUNC);
//STD_RenderState_Request_Method(RS_ZTEST);
//STD_RenderState_Request_Method(RS_ZWRITE);
NOP_RenderState_Request_Method(RS_ZBIAS); // this probably needs a ps2 implimentation
NOP_RenderState_Request_Method(RS_ZDIST); // this probably needs a ps2 implimentation
STD_RenderState_Request_Method(RS_FOG);
STD_RenderState_Request_Method(RS_FOGMODE);
//STD_RenderState_Request_Method(RS_FOGCOLOR);
STD_RenderState_Request_Method(RS_FOGSTART);
STD_RenderState_Request_Method(RS_FOGEND);
STD_RenderState_Request_Method(RS_FOGDENSITY);
STD_RenderState_Request_Method(RS_FOG_RENDER_PATH);
//STD_RenderState_Request_Method(RS_FAR_CLIPPING);
//STD_RenderState_Request_Method(RS_ALPHATEST);
//STD_RenderState_Request_Method(RS_ALPHAFUNC);
//STD_RenderState_Request_Method(RS_ALPHAREF);
//STD_RenderState_Request_Method(RS_FIXEDALPHA);
STD_RenderState_Request_Method(RS_PRIMITIVE);
STD_RenderState_Request_Method(RS_BASECOLOR);
//STD_RenderState_Request_Method(RS_LIGHTING);
//STD_RenderState_Request_Method(RS_SEND_LIGHTS);
NOP_RenderState_Request_Method(RS_TEXTURE_BRIGHTNESS);  // this probably eeds a ps2 implimentation
STD_RenderState_Request_Method(RS_TEXTURE_CONSTANTALPHA);
NOP_RenderState_Request_Method(RS_ENV_MAPPING);  // this probably eeds a ps2 implimentation
NOP_RenderState_Request_Method(RS_OVERRIDE_COLOR);  // this probably eeds a ps2 implimentation
STD_RenderState_Request_Method(RS_VU_CLIPPING);
NOP_RenderState_Request_Method(RS_SKINNING);
NOP_RenderState_Request_Method(RS_OBJECT_CONSTANTALPHA);

// Custom versions...
inline u64 _RenderState__GetALPHA1Settings_BLENDFUNC(int value)
{
	switch ( value )
	{
	default:
		LOCKUP();

		/* and fall through to default */
	case RS_BLENDFUNC_DEFAULT:
		return SCE_GS_SET_ALPHA(		// alpha-blending formula is (((A - B) * C) >> 7) + D
			0,			// A (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			1,			// B (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			0,			// C (0 = As (texel alpha), 1 = Ad (frame buffer alpha), 2 = FIX)
			1,			// D (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			128);		// FIX-- used when C == 2. 128 = 1.0
		break;

	case RS_BLENDFUNC_NONE:
		return SCE_GS_SET_ALPHA(
			0,			// A (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			0,			// B (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			2,			// C (0 = As (texel alpha), 1 = Ad (frame buffer alpha), 2 = FIX)
			0,			// D (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			0);		// fixed value (set to 0 for no impact)
		break;

	case RS_BLENDFUNC_ADD:
		return SCE_GS_SET_ALPHA(		// alpha-blending formula is (((A - B) * C) >> 7) + D
			0,			// A (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			2,			// B (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			2,			// C (0 = As (texel alpha), 1 = Ad (frame buffer alpha), 2 = FIX)
			1,			// D (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			128);		// FIX-- used when C == 2. 128 = 1.0
		break;

	case RS_BLENDFUNC_DIFFUSE:
		return SCE_GS_SET_ALPHA(		// alpha-blending formula is (((A - B) * C) >> 7) + D
			1,			// A (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			2,			// B (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			0,			// C (0 = As (texel alpha), 1 = Ad (frame buffer alpha), 2 = FIX)
			2,			// D (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			128);		// FIX-- used when C == 2. 128 = 1.0
		break;

	case RS_BLENDFUNC_FIXEDALPHA:	// used for semi-transparent environment mapping on ps2
		return SCE_GS_SET_ALPHA(		// alpha-blending formula is (((A - B) * C) >> 7) + D
			0,			// A (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			1,			// B (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			2,			// C (0 = As (texel alpha), 1 = Ad (frame buffer alpha), 2 = FIX)
			1,			// D (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			(int) (RenderState__itof( RenderState__Get( RS_FIXEDALPHA )) * 128.0f));	// FIX-- used when C == 2. 128 = 1.0
		break;

	case RS_BLENDFUNC_NEGATIVE:
		// frame texel = src_texel - frame_texel -- for inverting the frame buffer
		return SCE_GS_SET_ALPHA(		// alpha-blending formula is (((A - B) * C) >> 7) + D
			0,			// A (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			1,			// B (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			2,			// C (0 = As (texel alpha), 1 = Ad (frame buffer alpha), 2 = FIX)
			2,			// D (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			128);		// FIX-- used when C == 2. 128 = 1.0
		break;

	case RS_BLENDFUNC_ADDITIVE:
		// frame texel = src_texel * src_alpha + frame_texel * 1-- for additive particle effects
		return SCE_GS_SET_ALPHA(		// alpha-blending formula is (((A - B) * C) >> 7) + D
			0,			// A (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			2,			// B (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			0,			// C (0 = As (texel alpha), 1 = Ad (frame buffer alpha), 2 = FIX)
			1,			// D (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			128);		// FIX-- used when C == 2. 128 = 1.0
		break;

	case RS_BLENDFUNC_SUBTRACTIVE:
		// frame texel = frame texel - source texel, for shadow rendering
		// frame texel = (((frame texel - source texel) * 1.0) - 0
		return SCE_GS_SET_ALPHA(		// alpha-blending formula is (((A - B) * C) >> 7) + D
			1,			// A (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			0,			// B (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			2,			// C (0 = As (texel alpha), 1 = Ad (frame buffer alpha), 2 = FIX)
			2,			// D (0 = Cs (texel color), 1 = Cd (frame buffer color), 2 = 0)
			128);		// FIX-- used when C == 2. 128 = 1.0
		break;
	}
}

// this is the body of the blendfunc setting code... it gets
// called by both RS_BLENDFUNC and RS_FIXEDALPHA, so it
// needs to be split out.
inline void _RenderState__Request_RS_BLENDFUNC(int value)
{
	GSState.ALPHA_1 = _RenderState__GetALPHA1Settings_BLENDFUNC(value);
	GSState__Set(SCE_GS_ALPHA_1, GSState.ALPHA_1);
}

inline void RenderState__Request_RS_FIXEDALPHA(int value)
{
	// do the basic stuff...
	RenderState* s = &( _RenderStates[ RS_FIXEDALPHA ] );
	if(s->locked) return;
	s->value = value;
	MARK_UPDATED(s);
	//CORE_RenderState_Request(RS_FIXEDALPHA);

	if(_RenderStates[RS_BLENDFUNC].value == RS_BLENDFUNC_FIXEDALPHA)
		_RenderState__Request_RS_BLENDFUNC(RS_BLENDFUNC_FIXEDALPHA);
}

inline void RenderState__Request_RS_BLENDFUNC(int value)
{
	RenderState* s = &( _RenderStates[ RS_BLENDFUNC ] );									
	if(s->locked) return;																	
	s->value = value;																		
	MARK_UPDATED(s);
//	CORE_RenderState_Request(RS_BLENDFUNC);
	_RenderState__Request_RS_BLENDFUNC(value);
}

inline void RenderState__Request_RS_ZFUNC(int value)
{
//	CORE_RenderState_Request(RS_ZFUNC);

	/* record last value passed to this routine for use in RenderState__Request_RS_ZTEST */

	GSState.u16LastZFUNC = value;

	// determine bits to set in register value. record the value for possible later use by RenderState__Request_RS_ZTEST
	u64 u64Bits;
	switch(value)
	{
	case RS_ZFUNC_NEVER:
		u64Bits = (0L << GS_TEST_ZTST_O);
		break;
	case RS_ZFUNC_ALWAYS:
		u64Bits = (1L << GS_TEST_ZTST_O);
		break;
	case RS_ZFUNC_GEQUAL:
		u64Bits = (2L << GS_TEST_ZTST_O);
		break;
	case RS_ZFUNC_GREATER:
		u64Bits = (3L << GS_TEST_ZTST_O);
		break;
	case RS_ZFUNC_EQUAL:
	case RS_ZFUNC_NOTEQUAL:
	case RS_ZFUNC_LESS:
	case RS_ZFUNC_LEQUAL:
	default:
		ASSERT(FALSE);	// no other modes supported on ps2
		return;
	}

	// set bits in register value
	GSState.TEST_1 &= ~GS_TEST_ZTST_M;
	GSState.TEST_1 |= u64Bits;

	// send register value to gs-- gets buffered up with other gs commands
	GSState__Set(SCE_GS_TEST_1, GSState.TEST_1);
}

inline void RenderState__Request_RS_ZWRITE(int value)
{
	CORE_RenderState_Request(RS_ZWRITE);

	ASSERT( value == RS_TRUE || value == RS_FALSE );

	/* record last value passed to this routine for use in RenderState__Request_RS_ZTEST */

	GSState.u16LastZWRITE = value;

	// set bits in register value
	if ( value == RS_TRUE )
		GSState.ZBUF_1 &= ~GS_ZBUF_ZMSK_M;			// enable z-buffer writes
	else
		GSState.ZBUF_1 |= (1L << GS_ZBUF_ZMSK_O);	// disable z-buffer writes

	// send register value to gs-- gets buffered up with other gs commands
	GSState__Set(SCE_GS_ZBUF_1, GSState.ZBUF_1);
}

inline void RenderState__Request_RS_ZTEST(int value)
{
	//CORE_RenderState_Request(RS_ZTEST);

	ASSERT( value == RS_TRUE || value == RS_FALSE );

	/* the sony trc (technical requirements checklist) says that instead of turning off the buffer by clearing */
	/* the GS_TEST_ZTE bit, keep it set but change the ZTST bit to ALWAYS PASS */

	ASSERT(GSState.TEST_1 & (1L << GS_TEST_ZTE_O));	// trc says: ZTE bit must always be set
	if (value)
	{
		/* turning z-buffer on, restore last zfunc/zwrite settings */

		RenderState__Request_RS_ZFUNC(GSState.u16LastZFUNC);
	}
	else
	{
		/* turning z-buffer off, set zfunc to ALWAYS, disable writes to z-buffer. save and restore */
		/* current values since they get changed by calls to RenderState__Request_RS_ZFUNC and */
		/* RenderState__Request_RS_ZWRITE */

		u16 u16LastZFUNC = GSState.u16LastZFUNC;
		RenderState__Request_RS_ZFUNC(RS_ZFUNC_ALWAYS);
		GSState.u16LastZFUNC = u16LastZFUNC;
	}
}

inline void RenderState__Request_RS_ALPHAREF(int value)
{
	CORE_RenderState_Request(RS_ALPHAREF);

	ASSERT( value >= 0 && value < 256 );

	// set bits in register value
	GSState.TEST_1 &= ~GS_TEST_AREF_M;
	GSState.TEST_1 |= (((u64) value) << GS_TEST_AREF_O);

	// send register value to gs-- gets buffered up with other gs commands
	GSState__Set(SCE_GS_TEST_1, GSState.TEST_1);
}

inline void RenderState__Request_RS_ALPHAFUNC(int value)
{
	CORE_RenderState_Request(RS_ALPHAFUNC);

	u64 u64Bits;

	// determine bits to set in register value
	switch(value)
	{
	case RS_ALPHAFUNC_NEVER:
		u64Bits = (0L << GS_TEST_ATST_O);
		break;
	case RS_ALPHAFUNC_ALWAYS:
		u64Bits = (1L << GS_TEST_ATST_O);
		break;
	case RS_ALPHAFUNC_LESS:
		u64Bits = (2L << GS_TEST_ATST_O);
		break;
	case RS_ALPHAFUNC_LEQUAL:
		u64Bits = (3L << GS_TEST_ATST_O);
		break;
	case RS_ALPHAFUNC_EQUAL:
		u64Bits = (4L << GS_TEST_ATST_O);
		break;
	case RS_ALPHAFUNC_GEQUAL:
		u64Bits = (5L << GS_TEST_ATST_O);
		break;
	case RS_ALPHAFUNC_GREATER:
		u64Bits = (6L << GS_TEST_ATST_O);
		break;
	case RS_ALPHAFUNC_NOTEQUAL:
		u64Bits = (7L << GS_TEST_ATST_O);
		break;
	default:
		ASSERT(FALSE);	// invalid function value
		return;
	}

	// set bits in register value
	GSState.TEST_1 &= ~GS_TEST_ATST_M;
	GSState.TEST_1 |= u64Bits;

	// send register value to gs-- gets buffered up with other gs commands
	GSState__Set(SCE_GS_TEST_1, GSState.TEST_1);
}

inline void RenderState__Request_RS_ALPHATEST(int value)
{
	CORE_RenderState_Request(RS_ALPHATEST);
	
	ASSERT( value == RS_TRUE || value == RS_FALSE );

	// update register value
	if ( value )
		GSState.TEST_1 |= (1L << GS_TEST_ATE_O);
	else
		GSState.TEST_1 &= ~GS_TEST_ATE_M;

	// send register value to gs-- gets buffered up with other gs commands
	GSState__Set(SCE_GS_TEST_1, GSState.TEST_1);
}

inline void RenderState__Request_RS_FOGCOLOR(int value)
{
	CORE_RenderState_Request(RS_FOGCOLOR);

	unsigned char color[ 4 ];
	
	RenderState__Unpackubv( color, value );
	
	GSState__Set(SCE_GS_FOGCOL, ((int)color[0]) | ((int)color[1] << 8) | ((int)color[2] << 16));
}

inline void RenderState__Request_RS_LIGHTING(int value)
{
	// If the render manager says no lights, disable them regardless.
	// patchdisp.cpp does a separate query for this...
//	if(!RenderMgr__LightingOn())
//		value = RS_FALSE;

	CORE_RenderState_Request(RS_LIGHTING);
}

inline void RenderState__Request_RS_SEND_LIGHTS(int /*value*/)
{
	// this is really fairly stupid since we are tracking value...  But
	// then again, this function shouldnt really be a render state, but
	// rather something else entirely.
	//CORE_RenderState_Request(RS_SEND_LIGHTS);

	_RenderState__SendHardwareLights();
}

inline void RenderState__Request_RS_FAR_CLIPPING(int value)
{
	CScene *pScene = SceneMgr::GetCurrentScene();
	if (pScene)
	{
		// only update the value if we will change the projection
		// matrix... dont ask me, its what the old code did... :)
		CORE_RenderState_Request(RS_FAR_CLIPPING);

		float fFarClip;
		if (value)
			fFarClip = pScene->d_sceneView.GetFarClip();
		else
			fFarClip = 10000.0f;	// Disable by setting the far clip WAY out there.
		MatrixStack__Mode(MS_PROJECTION);
		MatrixStack__Identity();
		MatrixStack__Perspective(
			pScene->d_sceneView.GetFieldOfViewY(),
			pScene->d_sceneView.GetAspectRatio(),
			pScene->d_sceneView.GetNearClip(),
			fFarClip);
	}
}


///////////////////////////////////////////////////////////////////////////
// Some misc funtions inlined while I'm here...
// these ones arent used above, so they come last.
///////////////////////////////////////////////////////////////////////////

inline float RenderState__GetConstantAlpha( void )
{
	return RenderState__itof( RenderState__Get(RS_TEXTURE_CONSTANTALPHA) );
}



///////////////////////////////////////////////////////////////////////////
// undef this
///////////////////////////////////////////////////////////////////////////
#undef MARK_UPDATED

