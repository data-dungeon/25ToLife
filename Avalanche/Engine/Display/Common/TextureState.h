///////////////////////////////////////////////////////////////////////////
//
//  TextureState.h
//
//  Texture State Management Module.
//
//  Nate Robins, September 2000.
//
//  Copyright (c) 2000 Avalanche Software.
//
////////////////////////////////////////////////////////////////////////////

#ifndef TEXTURESTATE_H
#define TEXTURESTATE_H

#include "platform/CompilerControl.h"	// for NAMED_SECTION

class Texture;

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////
//  Types
////////////////////////////////////////////////////////////////////////////

enum
{
	TS_TYPE,
	TS_BIND,
	TS_METHOD,
	TS_WRAPU,
	TS_WRAPV,
	TS_FILTER,
	TS_EFFECT,
	
	TS_NUM_STATES
};
	
////////////////////////////////////////////////////////////////////////////

enum
{
	TS_TYPE_NONE,    // disable a texture
	TS_TYPE_2D,
};

////////////////////////////////////////////////////////////////////////////

enum
{
	TS_METHOD_PASSTHRU,
	TS_METHOD_DECAL,
	TS_METHOD_REPLACE,
	TS_METHOD_MODULATE,
	TS_METHOD_MODULATESELECT,
	TS_METHOD_INTERPOLATE,
	TS_METHOD_CONSTANTALPHA,
	TS_METHOD_DOTPRODUCT,
	TS_METHOD_DISABLE,

	// xbox for gloss mapping and bumps
	TS_METHOD_NONE  = 0x20,
	TS_METHOD_GLOSS = 0x40,
	TS_METHOD_DOT3	 = 0x80,
	
	TS_METHOD_UNKNOWN,
};


////////////////////////////////////////////////////////////////////////////

enum
{
	TS_WRAP_CLAMP,
	TS_WRAP_REPEAT,
};

////////////////////////////////////////////////////////////////////////////

enum
{
	TS_FILTER_POINTSAMPLE,
	TS_FILTER_BILINEAR,
	TS_FILTER_NEAREST_MIPMAP_NEAREST,	// point-sampled, choose nearest mipmap texture
	TS_FILTER_NEAREST_MIPMAP_LINEAR,		// "trilinear", but between point-sampled textures
	TS_FILTER_LINEAR_MIPMAP_NEAREST,		// linear, but choose nearest mipmap texture to use
	TS_FILTER_LINEAR_MIPMAP_LINEAR,		// trilinear
};

////////////////////////////////////////////////////////////////////////////

enum
{
	TS_EFFECT_NONE,
	TS_EFFECT_ENVMAP,
	TS_EFFECT_SHADOWMAP,
};


////////////////////////////////////////////////////////////////////////////
//  Defines
////////////////////////////////////////////////////////////////////////////
#ifdef GCN
#define TS_MAX_UNITS		8
#elif defined(DIRECTX)
#define TS_MAX_UNITS		8
#else
#define TS_MAX_UNITS		2
#endif

#define TS_NO_TEXTURE	0x12345678	// a texture index that means untextured, for example in registering sprites


////////////////////////////////////////////////////////////////////////////
//  Functions
////////////////////////////////////////////////////////////////////////////

void TextureState__Init( void );

////////////////////////////////////////////////////////////////////////////

void TextureState__Update( void );

////////////////////////////////////////////////////////////////////////////

#ifndef PS2
int  TextureState__Get( int unit, int state );
#endif

int  TextureState__GetNumUnits( void );
int  TextureState__ActiveUnit( void);

////////////////////////////////////////////////////////////////////////////

void TextureState__Request( int unit, int state, int value ) NAMED_SECTION("text.rend1");
void TextureState__RequestAll( int unit, int type, Texture* texture, int method, int wrapu, int wrapv, int filter );
void TextureState__RequestUnit( int unit ) NAMED_SECTION("text.rend1");
void TextureState__Force( int unit, int state );
void TextureState__ForceAll( void );
void TextureState__Default( void );
void TextureState__Lock( int unit, int state );
void TextureState__Unlock( int unit, int state );
void TextureState__LockAll( void );
void TextureState__UnlockAll( void );

#ifndef PS2
bool TextureState__Locked( int unit, int state );
#endif

#ifdef DIRECTX
void TextureState__Effect( int unit, int value, int index);
#endif
////////////////////////////////////////////////////////////////////////////
// Implemented per Platform
////////////////////////////////////////////////////////////////////////////

void _TextureState__Unit( int value );
void _TextureState__Type( int unit, int value );
void _TextureState__Bind( int unit, int value );
void _TextureState__Method( int unit, int value );
void _TextureState__WrapU( int unit, int value );
void _TextureState__WrapV( int unit, int value );
void _TextureState__Filter( int unit, int value );
void _TextureState__Effect( int unit, int value );


////////////////////////////////////////////////////////////////////////////
// Private !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
////////////////////////////////////////////////////////////////////////////

void _TextureState__Change( int unit, int state, int value );

////////////////////////////////////////////////////////////////////////////

extern int _TextureState_NumUnits;
extern int _TextureState_ActiveUnit;
#ifdef DIRECTX_PC
extern int _TextureState_AnisotropyLevel;
#endif

////////////////////////////////////////////////////////////////////////////

struct TextureState 
{
	int	value;
	char	force;
	char	locked;
};

////////////////////////////////////////////////////////////////////////////

// optimize queries with macros
#ifdef PS2
extern TextureState _TextureStates[ TS_MAX_UNITS ][ TS_NUM_STATES ];
#define TextureState__Locked( unit, state ) _TextureStates[ unit ][ state ].locked
#define TextureState__Get( unit, state )    _TextureStates[ unit ][ state ].value
#endif //PS2

////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif  // TEXTURESTATE_H
