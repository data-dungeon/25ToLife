///////////////////////////////////////////////////////////////////////////
//
//  CTextureState.c
//
//  Common Texture State Functions.
//
//  Nate Robins, September 2000.
//
//  Copyright (c) 2000 Avalanche Software.
//
////////////////////////////////////////////////////////////////////////////

#ifdef DIRECTX
#include "Display/DirectX/DXBase.h"
#endif

////////////////////////////////////////////////////////////////////////////
//  Includes
////////////////////////////////////////////////////////////////////////////

#include "Display/DisplayPCH.h"


////////////////////////////////////////////////////////////////////////////
//  Defines
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
//  Globals
////////////////////////////////////////////////////////////////////////////

TextureState _TextureStates[ TS_MAX_UNITS ][ TS_NUM_STATES ];

int _TextureState_NumUnits   = 0;
int _TextureState_ActiveUnit = 0;
#ifdef DIRECTX_PC
int _TextureState_AnisotropyLevel = 0;
#endif


////////////////////////////////////////////////////////////////////////////
//  Functions
////////////////////////////////////////////////////////////////////////////

void TextureState__Init( void )
{
#if defined(DIRECTX)
	_TextureState_NumUnits = DXBase.DeviceCaps.MaxTextureBlendStages;
#elif defined(PS2)
	_TextureState_NumUnits = 1;
#elif defined(GCN)
	_TextureState_NumUnits = TS_MAX_UNITS;
#endif

	TextureState__ForceAll( );
	TextureState__Default( );
}

#ifdef DIRECTX
void TextureState__Clear( void )
{
	for( int i = 0; i < TextureState__GetNumUnits( ); i++ )
	{
		if( i )
		{
			TextureState__Force( i, TS_METHOD );
			TextureState__Request( i, TS_METHOD, TS_METHOD_DISABLE );
			TextureState__Force( i, TS_TYPE );
			TextureState__Request( i, TS_TYPE, TS_TYPE_NONE );
		}
		else
		{
			TextureState__Force( i, TS_METHOD );
			TextureState__Request( i, TS_METHOD, TS_METHOD_MODULATE );
		}
	}

	DirectX_Device->SetPixelShader( NULL );
}

#endif


////////////////////////////////////////////////////////////////////////////

int TextureState__GetNumUnits( void )
{
    return _TextureState_NumUnits;
}

////////////////////////////////////////////////////////////////////////////

int TextureState__ActiveUnit( void)
{
    return _TextureState_ActiveUnit;
}

////////////////////////////////////////////////////////////////////////////

void TextureState__Request( int unit, int state, int value )
{
    TextureState* texture;

    ASSERT( unit >= 0 && unit < TextureState__GetNumUnits() );
    ASSERT( state >= 0 && state < TS_NUM_STATES );

    texture = &( _TextureStates[ unit ][ state ] );

	 _TextureState__Unit( unit );

	 if ( texture->locked)
	 {
		 return;
	 }

    if ( texture->value != value  || texture->force )
    {
        // If we're in a begin/end and rendering state needs changing, we 
        // have to terminate the begin/end before changing state. 
        if ( _RenderState_ActiveBegin )
        {
            RenderState__End();
        }

        switch ( state )
        {
        case TS_TYPE:
            _TextureState__Type( unit, value );
            break;
            
        case TS_BIND:
            _TextureState__Bind( unit, value );
            break;
            
        case TS_METHOD:
            _TextureState__Method( unit, value );
            break;
            
        case TS_WRAPU:
            _TextureState__WrapU( unit, value );
            break;
            
        case TS_WRAPV:
            _TextureState__WrapV( unit, value );
            break;
				
        case TS_FILTER:
            _TextureState__Filter( unit, value );
            break;
            
        case TS_EFFECT:
            _TextureState__Effect( unit, value );
            break;
            
        default:
            ASSERT( 0 );
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////

void TextureState__RequestAll( int unit, int type, Texture* bind, int method, int wrapu, int wrapv, int filter )
{
	#ifndef SHIP_IT
	if( unit >= TextureState__GetNumUnits( ))
	{
		return;
	}
	#endif // SHIP_IT

	ASSERT( unit < TextureState__GetNumUnits( ));

	ASSERT(	type == TS_TYPE_NONE || 
				type == TS_TYPE_2D    );


	ASSERT(	method == TS_METHOD_PASSTHRU			||
				method == TS_METHOD_DECAL				||
				method == TS_METHOD_REPLACE			||
				method == TS_METHOD_MODULATE			||
				method == TS_METHOD_MODULATESELECT	||
				method == TS_METHOD_INTERPOLATE		||
				method == TS_METHOD_DOTPRODUCT		||
				method == TS_METHOD_DISABLE			||
				method == TS_METHOD_CONSTANTALPHA	||
				method == TS_METHOD_NONE				||
				method == TS_METHOD_GLOSS				||
				method == TS_METHOD_DOT3 );

	ASSERT(	wrapu == TS_WRAP_CLAMP || 
				wrapu == TS_WRAP_REPEAT );

	ASSERT(	wrapv == TS_WRAP_CLAMP || 
				wrapv == TS_WRAP_REPEAT );

	ASSERT(	filter == TS_FILTER_POINTSAMPLE					|| 
				filter == TS_FILTER_BILINEAR						||
				filter == TS_FILTER_NEAREST_MIPMAP_NEAREST	||
				filter == TS_FILTER_NEAREST_MIPMAP_LINEAR		||
				filter == TS_FILTER_LINEAR_MIPMAP_NEAREST		||
				filter == TS_FILTER_LINEAR_MIPMAP_LINEAR		 );

	// Enable/Disable.
	TextureState__Request( unit, TS_TYPE, type );

	// Bind.
	{
		int oldBind = TextureState__Get( unit, TS_BIND );

		TextureState__Request( unit, TS_BIND, (int)bind );

		// The repeat mode appears to be associated with the texture 
		// itself, and not the texture unit. So set the repeat mode 
		// whenever we bind a new texture.
		if ( (int)bind != oldBind )
		{
			TextureState__Force( unit, TS_WRAPU );
			TextureState__Force( unit, TS_WRAPV );
			TextureState__Force( unit, TS_FILTER );

			#ifdef PS2
			//yuch-- on ps2, these states are encoded in the texture state, not the texture unit state, so we have to accept all
			// requests. could we keep track on a texture by texture basis? as it is, the penalty isn't too bad since this
			// routine is only called when building a display list. so anyway, if texture changes, force these other states to update
			// too
			TextureState__Force( unit, TS_TYPE );
			TextureState__Force( unit, TS_METHOD );
			#endif
		}
	}
	
	// Repeat state (repeat, clamp).
	TextureState__Request( unit, TS_WRAPU, wrapu );
	TextureState__Request( unit, TS_WRAPV, wrapv );
	
	// Method (modulate, decal, etc).
	TextureState__Request( unit, TS_METHOD, method );
	
	// Filter (pointsample, bilinear).
	TextureState__Request( unit, TS_FILTER, filter );
}

////////////////////////////////////////////////////////////////////////////

void TextureState__RequestUnit( int unit )
{
	if ( unit != _TextureState_ActiveUnit )
	{
		_TextureState__Unit( unit );
	}
}

////////////////////////////////////////////////////////////////////////////

void TextureState__Force( int unit, int state )
{
	
	TextureState *s = &( _TextureStates[ unit ][ state ] );

	ASSERT( unit >= 0 && unit < TextureState__GetNumUnits() );
	ASSERT( state >= 0 && state < TS_NUM_STATES );

	if( s->locked )
	{
		return;
	}

	s->force = TRUE;
}

////////////////////////////////////////////////////////////////////////////

void TextureState__ForceAll( void )
{
    
	int		u,
				s;

	for( u = 0; u < TextureState__GetNumUnits( ); u++ )
	{
		for( s = 0; s < TS_NUM_STATES; s++ )
		{
			TextureState__Force( u, s );
		}
	}
}

////////////////////////////////////////////////////////////////////////////

void TextureState__Default( void )
{
    
#ifdef DIRECTX
	DirectX_Device->SetPixelShader( NULL );

	for( int i = 0; i < TextureState__GetNumUnits( ); i++ )
	{
		if( i == 0 )
			TextureState__RequestAll( i, TS_TYPE_NONE, 0, 0, 0, 0, 0 );
		else
			TextureState__RequestAll( i, TS_TYPE_NONE, 0, TS_METHOD_DISABLE, 0, 0, 0 );

		TextureState__Request( i, TS_EFFECT, TS_EFFECT_NONE );
	}
#else
	for( int i = 0; i < TextureState__GetNumUnits( ); i++ )
	{
		TextureState__RequestAll( i, TS_TYPE_NONE, 0, 0, 0, 0, 0 );
		TextureState__Request( i, TS_EFFECT, TS_EFFECT_NONE );
	}
#endif
}

////////////////////////////////////////////////////////////////////////////

#ifndef PS2 //PS2 version is macroized
int TextureState__Get( int unit, int state )
{
    
	TextureState* s = &( _TextureStates[ unit ][ state ] );

	ASSERT( unit >= 0 && unit < TextureState__GetNumUnits( ) );
	ASSERT( state >= 0 && state < TS_NUM_STATES );

	return s->value;
}
#endif //PS2

////////////////////////////////////////////////////////////////////////////

void _TextureState__Change( int unit, int state, int value )
{

	TextureState* s = &( _TextureStates[ unit ][ state ] );

	s->value   = value;
	s->force   = FALSE;

	RenderState__CheckForErrors();
}

////////////////////////////////////////////////////////////////////////////

void TextureState__Lock( int unit, int state )
{
	ASSERT( unit >= 0 && unit < TextureState__GetNumUnits() );
	ASSERT( state >= 0 && state < TS_NUM_STATES );

	_TextureStates[ unit ][ state ].locked = TRUE;
}

////////////////////////////////////////////////////////////////////////////

void TextureState__Unlock( int unit, int state )
{
	ASSERT( unit >= 0 && unit < TextureState__GetNumUnits() );
	ASSERT( state >= 0 && state < TS_NUM_STATES );

	_TextureStates[ unit ][ state ].locked = FALSE;
}

////////////////////////////////////////////////////////////////////////////

void TextureState__LockAll( void )
{

	int	u,
			s;

	for ( u = 0; u < TextureState__GetNumUnits(); u++ )
	{
		for ( s = 0; s < TS_NUM_STATES; s++ )
		{
			TextureState__Lock( u, s );
		}
	}
}

////////////////////////////////////////////////////////////////////////////

void TextureState__UnlockAll( void )
{

	int	u,
			s;

	for ( u = 0; u < TextureState__GetNumUnits(); u++ )
	{
		for ( s = 0; s < TS_NUM_STATES; s++ )
		{
			TextureState__Unlock( u, s );
		}
	}
}

////////////////////////////////////////////////////////////////////////////

#ifndef PS2 //PS2 version is macroized
bool TextureState__Locked( int unit, int state )
{
	ASSERT( state >= 0 && state < TS_NUM_STATES );

	return _TextureStates[ unit ][ state ].locked != 0;
}
#endif //PS2

////////////////////////////////////////////////////////////////////////////
