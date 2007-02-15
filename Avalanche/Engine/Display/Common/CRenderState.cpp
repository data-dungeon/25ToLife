///////////////////////////////////////////////////////////////////////////
//
//  CRenderState.c
//
//  Render State Common Functionality.
//
//  Nate Robins, September 2000.
//
//  Copyright (c) 2000 Avalanche Software.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//  Notes:
//
//  To add a new state,
//
//  1. add a new value to the render state enumeration (d_RenderState.h).
//  2. add a new case to the RenderState__Set() switch statement.
//  3. add a new function that handles the state (platform specific).
//  4. call the new function with a default value from RenderState__Default().
//  5. add a new de-reference table if needed (platform specific).
//  6. add a new enumeration to the header file if a de-reference table was
//     needed.
//
//  ADD THE FUNCTIONALITY TO OTHER PLATFORMS!
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  Includes
////////////////////////////////////////////////////////////////////////////

#include "Display/DisplayPCH.h"





////////////////////////////////////////////////////////////////////////////
//  Defines
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
//  Types
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
//  Globals
////////////////////////////////////////////////////////////////////////////

RenderState _RenderStates[ RS_NUM_STATES ];

int _RenderState_ActiveBegin			= RS_FALSE;
int _RenderState_NumLights				= 0;
int _RenderState_LightPositionDirty	= RS_FALSE;

ts_Light* _RenderState_Lights[ RS_MAXLIGHTS ];

float _RenderState_GlobalAmbient[ 4 ] = { RS_DEFAULT_AMBIENT_RED, RS_DEFAULT_AMBIENT_GREEN, RS_DEFAULT_AMBIENT_BLUE, 1.0f };

////////////////////////////////////////////////////////////////////////////
//  local prototypes
////////////////////////////////////////////////////////////////////////////

static void _RenderState__FixedAlpha( int value );

////////////////////////////////////////////////////////////////////////////
//  Functions
////////////////////////////////////////////////////////////////////////////

void RenderState__Init( void )
{
	Platform__Init();
	TextureState__Init();

	memset( _RenderStates, 0, sizeof ( RenderState ) * RS_NUM_STATES );

	// Force all states to update the first time they are set.
	RenderState__ForceAll();
	RenderState__Default();

	/* we assume that the base alpha ref value is 0 and doesn't need to be multiplied by the current */
	/* constant value. if this changes, we may have to multiply by constant alpha, like with the use of */
	/* RS_ALPHA_REF_NO_HALO_VALUE */

	ASSERT(RS_ALPHA_REF_DEFAULT_VALUE == 0);
}

////////////////////////////////////////////////////////////////////////////
#ifndef PS2
void RenderState__Request( int state, int value )
{
    RenderState* s = &( _RenderStates[ state ] );

    ASSERT( state >= 0 && state < RS_NUM_STATES );

	 if ( s->locked )
	 {
		 return;
	 }

    if ( s->value != value || s->force )
    {
        // If we're in a begin/end and rendering state needs changing, we
        // have to terminate the begin/end before changing state.
        if ( _RenderState_ActiveBegin )
        {
            RenderState__End();
        }

        switch ( state )
        {
        case RS_POLYGONMODE:
            #ifndef PS2
            _RenderState__PolygonMode( value );
            #endif //PS2
            break;

        case RS_FRONTFACE:
            #ifndef PS2
            _RenderState__FrontFace( value );
            #endif //PS2
            break;

        case RS_BLENDFUNC:
            _RenderState__BlendFunc( value );
            break;

        case RS_ZFUNC:
            _RenderState__ZFunc( value );
            break;

        case RS_ZTEST:
            _RenderState__ZTest( value );
            break;

        case RS_CULL:
            _RenderState__Cull( value );
            break;

        case RS_BASECOLOR:
            _RenderState__BaseColor( value );
            break;

        case RS_PRIMITIVE:
            _RenderState__Primitive( value );
            break;

        case RS_SEND_LIGHTS:
            _RenderState__SendHardwareLights();
            break;

        case RS_FOG:
#ifndef CONSUMER_BUILD
            /* render manager can force all fog off-- but always let first value == FALSE request through */

            if ((RenderMgr__FogEnabled() == FALSE) && (value == RS_TRUE))
					break;
#endif //CONSUMER_BUILD

            _RenderState__Fog( value );
				if (SceneMgr::GetCurrentSceneView())
					SceneMgr::GetCurrentSceneView()->UpdateFog(value == RS_TRUE);
            break;

        case RS_FOGMODE:
            _RenderState__FogMode( value );
            break;

        case RS_FOGCOLOR:
            _RenderState__FogColor( value );
            break;

        case RS_FOGSTART:
//yuch-- temporary			   ASSERT(SceneMgr::GetCurrentSceneView());					// only thing that should be calling this is SceneView::Render-- see note in Display\Common\SceneView.cpp
//yuch-- temporary			   ASSERT(SceneMgr::GetCurrentSceneView()->InRender());	// only thing that should be calling this is SceneView::Render-- see note in Display\Common\SceneView.cpp
            _RenderState__FogStart( value );
            break;

        case RS_FOGEND:
//yuch-- temporary			   ASSERT(SceneMgr::GetCurrentSceneView());					// only thing that should be calling this is SceneView::Render-- see note in Display\Common\SceneView.cpp
//yuch-- temporary			   ASSERT(SceneMgr::GetCurrentSceneView()->InRender());	// only thing that should be calling this is SceneView::Render-- see note in Display\Common\SceneView.cpp
            _RenderState__FogEnd( value );
            break;

        case RS_FOGDENSITY:
            _RenderState__FogDensity( value );
            break;

        case RS_FOG_RENDER_PATH:
#ifdef PS2
            _RenderState__FogRenderPath( value );
#endif //PS2
            break;

        case RS_FAR_CLIPPING:
            _RenderState__FarClipping( value );
            break;

        case RS_ALPHATEST:
            _RenderState__AlphaTest( value );
            break;

        case RS_ALPHAREF:
            _RenderState__AlphaRef( value );
            break;

        case RS_ALPHAFUNC:
            _RenderState__AlphaFunc( value );
            break;

        case RS_FIXEDALPHA:
            _RenderState__FixedAlpha( value );
            break;

        case RS_LIGHTING:
            _RenderState__Lighting( value );
            break;

        case RS_ZWRITE:
            _RenderState__ZWrite( value );
            break;

        case RS_ZBIAS:
            _RenderState__ZBias( value );
            break;

        case RS_ZDIST:
            _RenderState__ZDist( value );
            break;

        case RS_TEXTURE_BRIGHTNESS:
            _RenderState__TextureBrightness( value );
            break;

        case RS_TEXTURE_CONSTANTALPHA:
            _RenderState__TextureConstantAlpha( value );
            break;

        case RS_ENV_MAPPING:
            _RenderState__EnvMapping( value );
            break;

        case RS_OVERRIDE_COLOR:
            _RenderState__OverrideColor( value );
            break;

        #ifdef PS2
        case RS_VU_CLIPPING:
            _RenderState__VUClipping( value );
            break;
        #endif //PS2

        case RS_SKINNING:
            _RenderState__Skinning( value );
            break;

#if defined (GCN)  || defined(DIRECTX)
        case RS_OBJECT_CONSTANTALPHA:
            _RenderState__ObjectConstantAlpha( value );
            break;
#endif
        default:
            ASSERT( 0 );
            break;
        }
    }
}
#endif
////////////////////////////////////////////////////////////////////////////

#ifndef PS2
void RenderState__Force( int state )
{
	RenderState* s = &( _RenderStates[ state ] );

	ASSERT( state >= 0 && state < RS_NUM_STATES );

	if ( s->locked )
	{
		return;
	}

	s->force = RS_TRUE;
}
#endif
////////////////////////////////////////////////////////////////////////////
#ifndef PS2
int RenderState__Get( int state )
{
	RenderState* s = &( _RenderStates[ state ] );

	ASSERT( state >= 0 && state < RS_NUM_STATES );

	return s->value;
}
#endif
////////////////////////////////////////////////////////////////////////////

void RenderState__Begin( void )
{
	if ( ! _RenderState_ActiveBegin )
	{
		int primitive = _RenderStates[ RS_PRIMITIVE ].value;

		if ( primitive != RS_PRIMITIVE_DRAWARRAY && primitive != RS_PRIMITIVE_DISPLAYLIST )
		{
			_RenderState__Begin( primitive );

			_RenderState_ActiveBegin = RS_TRUE;
		}
	}
}

////////////////////////////////////////////////////////////////////////////

void RenderState__End( void )
{
	if ( _RenderState_ActiveBegin )
	{
		_RenderState__End();

		_RenderState_ActiveBegin = RS_FALSE;
	}

	// Some states may become invalid because of a DRAWARRAY or
	// DISPLAYLIST primitive (such as the base color). Force these to
	// update next time around here.
	{
		int primitive = _RenderStates[ RS_PRIMITIVE ].value;

		if ( primitive == RS_PRIMITIVE_DRAWARRAY || primitive == RS_PRIMITIVE_DISPLAYLIST )
		{
			RenderState__Force( RS_BASECOLOR );
		}
	}

    RenderState__CheckForErrors();
}

////////////////////////////////////////////////////////////////////////////
#ifndef PS2
void RenderState__ForceAll( void )
{
	int i;

	for ( i = 0; i < RS_NUM_STATES; i++ )
	{
		RenderState__Force( i );
	}

	TextureState__ForceAll();
}
#endif
////////////////////////////////////////////////////////////////////////////
#ifndef PS2
void RenderState__Lock( int state )
{
	ASSERT( state >= 0 && state < RS_NUM_STATES );
	
	_RenderStates[ state ].locked = TRUE;
}
#endif
////////////////////////////////////////////////////////////////////////////
#ifndef PS2
void RenderState__Unlock( int state )
{
	ASSERT( state >= 0 && state < RS_NUM_STATES );
	
	_RenderStates[ state ].locked = FALSE;
}
#endif
////////////////////////////////////////////////////////////////////////////
#ifndef PS2
void RenderState__LockAll( void )
{
	int i;

	for ( i = 0; i < RS_NUM_STATES; i++ )
	{
		RenderState__Lock( i );
	}
}
#endif
////////////////////////////////////////////////////////////////////////////
#ifndef PS2
void RenderState__UnlockAll( void )
{
	int i;

	for ( i = 0; i < RS_NUM_STATES; i++ )
	{
		RenderState__Unlock( i );
	}
}
#endif
////////////////////////////////////////////////////////////////////////////
#ifndef PS2
bool RenderState__Locked( int state )
{
	ASSERT( state >= 0 && state < RS_NUM_STATES );
	
	return _RenderStates[ state ].locked;
}
#endif

////////////////////////////////////////////////////////////////////////////

void RenderState__Default( void )
{
	RenderState__Request( RS_BASECOLOR, 0xFFFFFFFF );
	RenderState__Request( RS_CULL, RS_CULL_BACK );
	RenderState__Request( RS_BLENDFUNC, RS_BLENDFUNC_DEFAULT );
	RenderState__Request( RS_FOG, RS_TRUE );
	RenderState__Request( RS_FOG_RENDER_PATH, RS_FALSE );
	RenderState__Request( RS_FAR_CLIPPING, RS_TRUE );
	RenderState__Request( RS_TEXTURE_BRIGHTNESS, RenderState__ftoi( 1.0f ) );
	RenderState__Request( RS_TEXTURE_CONSTANTALPHA, RenderState__ftoi( 1.0f ) );
#ifdef GCN
	RenderState__Request( RS_OBJECT_CONSTANTALPHA, RenderState__ftoi( 1.0f ) );
#endif
	RenderState__Request( RS_LIGHTING, RS_FALSE );
	RenderState__Request( RS_ZWRITE, RS_TRUE );

	RenderState__Request( RS_ZBIAS, RenderState__ftoi( 0.0f ) );
	RenderState__Request( RS_ZDIST, RenderState__ftoi( 1.0f ) );

	#ifdef WIN32
	RenderState__Request( RS_POLYGONMODE, RS_POLYGONMODE_FILL );
	RenderState__Request( RS_FRONTFACE, RS_FRONTFACE_CW );
	RenderState__Request( RS_ZFUNC, RS_ZFUNC_LEQUAL );
	#endif
	#ifdef PS2
	RenderState__Request( RS_ZFUNC, RS_ZFUNC_GEQUAL );
	RenderState__Request( RS_VU_CLIPPING, RS_CLIPPING_CLIP );
	#endif
	#ifdef GCN
	RenderState__Request( RS_POLYGONMODE, RS_POLYGONMODE_FILL );
	RenderState__Request( RS_FRONTFACE, RS_FRONTFACE_CCW );
	RenderState__Request( RS_ZFUNC, RS_ZFUNC_LEQUAL );
	#endif
	#ifdef DIRECTX
	RenderState__Request( RS_CULL, RS_CULL_NONE );
	#endif

	RenderState__Request( RS_ZTEST, RS_TRUE );

	RenderState__Request( RS_ENV_MAPPING, RS_FALSE );
	RenderState__Request( RS_OVERRIDE_COLOR, RS_FALSE );

#ifndef CONSUMER_BUILD
	if ( RenderMgr__ForceAlphaTest() )
	{
		RenderState__Request( RS_ALPHAFUNC, RS_ALPHAFUNC_GREATER );
		RenderState__Request( RS_ALPHAREF,	RenderMgr__ForceAlphaTestValue() );
		RenderState__Request( RS_ALPHATEST, RS_TRUE );
	}
	else
#endif // CONSUMER_BUILD
	{
		RenderState__Request( RS_ALPHAFUNC, RS_ALPHAFUNC_GREATER );
		RenderState__Request( RS_ALPHAREF,	RS_ALPHA_REF_DEFAULT_VALUE );
		RenderState__Request( RS_ALPHATEST, RS_TRUE );
	}
   RenderState__Request( RS_FIXEDALPHA,   RenderState__ftoi( 1.0f ) );

	// Default the texture state.
	TextureState__Default();

	// Default the draw array subsystem.
	DrawArray__Disable( DA_VECTOR );
	DrawArray__Disable( DA_NORMAL );
	DrawArray__Disable( DA_COLOR );

	DrawArray__TextureUnit( 1 );
	DrawArray__Disable( DA_TEXTURE );
	DrawArray__TextureUnit( 0 );
	DrawArray__Disable( DA_TEXTURE );
}

// Conversions /////////////////////////////////////////////////////////////
#ifndef PS2
int RenderState__ftoi( float value )
{
	return *( ( int* )&value );
}
#endif
////////////////////////////////////////////////////////////////////////////
#ifndef PS2
float RenderState__itof( int value )
{
	return *( ( float* )&value );
}
#endif
////////////////////////////////////////////////////////////////////////////
#ifndef PS2
int RenderState__Colorfv( float* color )
{
	unsigned char r = ( unsigned char )_RenderState__Clampf( color[ 0 ] * 255, 0, 255 );
	unsigned char g = ( unsigned char )_RenderState__Clampf( color[ 1 ] * 255, 0, 255 );
	unsigned char b = ( unsigned char )_RenderState__Clampf( color[ 2 ] * 255, 0, 255 );
	unsigned char a = ( unsigned char )_RenderState__Clampf( color[ 3 ] * 255, 0, 255 );

	unsigned int packed = ( a << 24 ) | ( b << 16 ) | ( g << 8 ) | ( r << 0 );

	return packed;
}
#endif
////////////////////////////////////////////////////////////////////////////
#ifndef PS2
int RenderState__Colorubv( unsigned char* color )
{
	unsigned char r = color[ 0 ];
	unsigned char g = color[ 1 ];
	unsigned char b = color[ 2 ];
	unsigned char a = color[ 3 ];

	unsigned int packed = ( a << 24 ) | ( b << 16 ) | ( g << 8 ) | ( r << 0 );

	return packed;
}
#endif
////////////////////////////////////////////////////////////////////////////
#ifndef PS2
void RenderState__Unpackfv( float* color, int packed )
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
#endif
////////////////////////////////////////////////////////////////////////////
#ifndef PS2
void RenderState__Unpackubv( unsigned char* color, int packed )
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
#endif
// Accessors ///////////////////////////////////////////////////////////////
#ifndef PS2
float RenderState__GetConstantAlpha( void )
{
	return RenderState__itof( _RenderStates[ RS_TEXTURE_CONSTANTALPHA ].value );
}
#endif

////////////////////////////////////////////////////////////////////////////

void RenderState__CheckForErrors( void )
{
}

////////////////////////////////////////////////////////////////////////////
// Private
////////////////////////////////////////////////////////////////////////////
#ifndef PS2
void _RenderState__Change( int state, int value )
{
	RenderState* s = &( _RenderStates[ state ] );

	s->value   = value;
	s->force   = RS_FALSE;
#ifdef SUPPORT_UPDATED_COUNT
	s->updated++;
#endif

	RenderState__CheckForErrors();
}
#endif
////////////////////////////////////////////////////////////////////////////
#ifndef PS2
float _RenderState__Clampf( float x, float min, float max )
{
	if ( x < min )
	{
		return min;
	}
	else if ( x > max )
	{
		return max;
	}
	else
	{
		return x;
	}
}
#endif

////////////////////////////////////////////////////////////////////////////

/* just a place to store a float value, no affect on state machine. used for ps2 env mapping */
#ifndef PS2
static void _RenderState__FixedAlpha( int value )
{
	_RenderState__Change( RS_FIXEDALPHA, value );

	/* the passed value gets used by the routine _RenderState__BlendFunc to set the fixed-alpha blend function */
	/* for the PS2. if that blend function is already active, then we want to update the fixed alpha that */
	/* it is using */

#ifdef PS2
	if (_RenderStates[RS_BLENDFUNC].value == RS_BLENDFUNC_FIXEDALPHA)
		_RenderState__BlendFunc(RS_BLENDFUNC_FIXEDALPHA);
#endif // PS2
}
#endif
////////////////////////////////////////////////////////////////////////////
