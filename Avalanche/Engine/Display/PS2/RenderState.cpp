///////////////////////////////////////////////////////////////////////////
//
//  RenderState.c
//
//  RenderState Rendering Module.
//
//  Ned Martin, May, 2000.
//  Nate Robins, September 2000.
//
//  Copyright (c) 2000 Avalanche Software.
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

////////////////////////////////////////////////////////////////////////////
//  Locals
////////////////////////////////////////////////////////////////////////////

int _RenderState_ParticleLightsDirty = TRUE;

////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  Functions
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

void _RenderState__Begin( int primitive )
{
	// gl-style begin/pass some vertices/end rendering not supported on ps2
	ASSERT(FALSE);
}

////////////////////////////////////////////////////////////////////////////

void _RenderState__End( void )
{
	// gl-style begin/pass some vertices/end rendering not supported on ps2
	ASSERT(FALSE);
}

// Lighting ////////////////////////////////////////////////////////////////

//DJR: This AMBIENT_SCALE was set to 96.0 from some tests done on the Downhill project to get all
//platforms to have similar lighting.  However, in reviewing Rugrats, the lighting appears washed out,
//mainly due to excess amibent light (and too little directional light - see SCALE_CONSTANT below).
//So I decreased this constant down to 64.0 for Rugrats: Royal Ransom.
//DJR: Also note that the directional lights are already doubled in LightMgr.cpp.

#define AMBIENT_SCALE 64.0f

void RenderState__RegisterHardwareAmbientLight(ts_fRGB Color)
{
	_RenderState_GlobalAmbient[ 0 ] = Color.R;
	_RenderState_GlobalAmbient[ 1 ] = Color.G;
	_RenderState_GlobalAmbient[ 2 ] = Color.B;
	_RenderState_GlobalAmbient[ 3 ] = 1.0f;

	// particle lighting info needs updating
	_RenderState_ParticleLightsDirty = TRUE;

	sceVu0FVECTOR light;

	light[0] = Color.R * AMBIENT_SCALE;
	light[1] = Color.G * AMBIENT_SCALE;
	light[2] = Color.B * AMBIENT_SCALE;
	light[3] = 128.0f;

	PS2Renderer::SetAmbientLight(light);
}

////////////////////////////////////////////////////////////////////////////

void RenderState__RegisterHardwareLight( ts_Light* pLight )
{
	if ( _RenderState_NumLights < RS_MAXLIGHTS )
	{
		// is light on?
		if ( ( pLight->u32Flags & LIGHT_ON ) == 0 )
		{
			return;
		}

		// record light
		_RenderState_Lights[ _RenderState_NumLights ] = pLight;

		// particle lighting info needs updating
		_RenderState_ParticleLightsDirty = TRUE;

		PS2Renderer::EnableLight( _RenderState_NumLights, TRUE, pLight->u32Flags & LIGHT_LIGHTING_TYPE );

		// another light added
		_RenderState_NumLights++;
	}
}

////////////////////////////////////////////////////////////////////////////

#define SCALE_CONSTANT 128.0f
static sceVu0FVECTOR blackAmbient = {0.0f, 0.0f, 0.0f, 0.0f};
void RenderState__RegisterLightSetHardwareLight( u32 index, CLightRecord *pLight, float attenuationScale )
{
	int i;

	// anything to do?
	bool bLightColorMultDirty;
	float fLightColorMult;
	bLightColorMultDirty = g_lightSet.getLightColorMult( fLightColorMult );
//	if ((bLightColorMultDirty == FALSE) && (_RenderState_LightPositionDirty == FALSE))
//		return;

	sceVu0FVECTOR Specular, Dir, Color;
	*( ( u128 * )Specular ) = 0;
	// --- get light color
	*( ( Vector4 * )&Color ) = ( ( pLight->getColor( ) * pLight->getIntensity( ) ) * 128.0f );

	/* initial transform into camera space so that CameraPos is valid */
	switch( pLight->getType( ) )
	{
		// --- point light
		case CLightRecord::LIGHT_OMNI:
			// --- get the lights position
			Dir[ 0 ] = pLight->getPosition( ).x();
			Dir[ 1 ] = pLight->getPosition( ).y();
			Dir[ 2 ] = pLight->getPosition( ).z();
			Dir[ 3 ] = 1.0f;
			PS2Renderer::EnableLight( index, TRUE, LIGHT_POINT );
			PS2Renderer::SetLight( index, Dir, blackAmbient, Color, Specular, LIGHT_SPACE_WORLD, LIGHT_TYPE_POINT, 3.0f / ( pLight->getQuadraticAttenuation( ) * attenuationScale) );
			break;

		// --- directional light
		case CLightRecord::LIGHT_FREE_DIRECT:
			Color[ 0 ] *= fLightColorMult;
			Color[ 1 ] *= fLightColorMult;
			Color[ 2 ] *= fLightColorMult;
			Color[ 3 ] = 0;
			Dir[ 0 ] = -pLight->getDirection( ).x();
			Dir[ 1 ] = -pLight->getDirection( ).y();
			Dir[ 2 ] = -pLight->getDirection( ).z();
			Dir[ 3 ] = 0.0f;
			PS2Renderer::EnableLight( index, TRUE, LIGHT_DIRECTIONAL );
			PS2Renderer::SetLight( index, Dir, blackAmbient, Color, Specular, LIGHT_SPACE_WORLD, LIGHT_TYPE_DIRECTIONAL, 0.0f );
			break;

		default:
			ASSERT( 0 );
	}
}
#undef SCALE_CONSTANT

////////////////////////////////////////////////////////////////////////////

void RenderState__DisableHardwareLightRange( u32 from )
{
	for( u32 i = from; i < MAX_LIGHTS; i++ )
		PS2Renderer::EnableLight( i, FALSE, 0 );
}

////////////////////////////////////////////////////////////////////////////

void RenderState__RemoveHardwareLight(ts_Light* pLight)
{
	int i;
	int j;

	// find light in active list, remove
	j = 0;
	for ( i = 0; i < _RenderState_NumLights; i++ )
	{
		if (_RenderState_Lights[i] != pLight)
		{
			// copy down
			if (i != j)
				_RenderState_Lights[j] = _RenderState_Lights[i];
			j++;
		}
	}

	// may not find it (light may be off, and so not in hardware light list, even though it's in light manager list)
	if (j == _RenderState_NumLights)
		return;	// no lights removed

	// update light count
	_RenderState_NumLights = j;

	// particle lighting info needs updating
	_RenderState_ParticleLightsDirty = TRUE;

	PS2Renderer::EnableLight( j, FALSE, pLight->u32Flags & LIGHT_LIGHTING_TYPE );
}

////////////////////////////////////////////////////////////////////////////

bool RenderState__HardwareLightsEmpty(void)
{
	return(_RenderState_NumLights == 0);
}

////////////////////////////////////////////////////////////////////////////
//DJR: This SCALE_CONSTANT was set to 96.0 from some tests done on the Downhill project to get all
//platforms to have similar lighting.  However, in reviewing Rugrats, the lighting appears washed out,
//mainly due to excess amibent light (and too little directional light - see AMBIENT_SCALE above).
//So I increased this constant back to it's original 128.0 for Rugrats: Royal Ransom.
//DJR: Also note that the directional lights are already doubled in LightMgr.cpp.

#define SCALE_CONSTANT 128.0f

void _RenderState__SendHardwareLights( void )
{
	int i;

	// any lights?
	if (_RenderState_NumLights == 0
#ifndef CONSUMER_BUILD
		|| RenderMgr__LightingOn() == FALSE 
#endif
		)
	{
		return;
	}

	// anything to do?
	bool bLightColorMultDirty;
	float fLightColorMult;
	bLightColorMultDirty = g_lightSet.getLightColorMult( fLightColorMult );
	if ((bLightColorMultDirty == FALSE) && (_RenderState_LightPositionDirty == FALSE))
		return;

	// loop through active directional lights
	for ( i = 0; i < _RenderState_NumLights; i++ )
	{
		ts_Light *pLight = _RenderState_Lights[i];
		// all lights in list should be local or infinite directional
		ASSERT(pLight->u32Flags & (LIGHT_LIGHTING_TYPE));
		if (pLight->u32Flags & LIGHT_ON)
		{
			// TJC - this scale looks pretty good in comparison to the XBOX version..
			sceVu0FVECTOR Dir, Clr, Ambient, Specular;

			Clr[0] = pLight->Color.R * fLightColorMult * SCALE_CONSTANT;
			Clr[1] = pLight->Color.G * fLightColorMult * SCALE_CONSTANT;
			Clr[2] = pLight->Color.B * fLightColorMult * SCALE_CONSTANT;
			Clr[3] = 0.0f * SCALE_CONSTANT;

			// --- das 2/24/03 why was the ambient forced to a constant?
#if 0
			Ambient[0] = 0.2f * SCALE_CONSTANT;
			Ambient[1] = 0.2f * SCALE_CONSTANT;
			Ambient[2] = 0.2f * SCALE_CONSTANT;
			Ambient[3] = 0.0f * SCALE_CONSTANT;
#else
			Ambient[0] = 0.0f;
			Ambient[1] = 0.0f;
			Ambient[2] = 0.0f;
			Ambient[3] = 0.0f;
#endif
			Specular[0] = pLight->Color.R * fLightColorMult * SCALE_CONSTANT;
			Specular[1] = pLight->Color.G * fLightColorMult * SCALE_CONSTANT;
			Specular[2] = pLight->Color.B * fLightColorMult * SCALE_CONSTANT;
			Specular[3] = 0.0f * SCALE_CONSTANT;
#undef SCALE_CONSTANT

			switch( pLight->u32Flags & ( LIGHT_LIGHTING_TYPE ) )
			{
				case LIGHT_POINT:
					Dir[0] = pLight->Pos.x();
					Dir[1] = pLight->Pos.y();
					Dir[2] = pLight->Pos.z();
					Dir[3] = 1.0f;
					PS2Renderer::SetLight( i, Dir, Ambient, Clr, Specular, LIGHT_SPACE_WORLD, LIGHT_TYPE_POINT, 0.0f );
					break;

				case LIGHT_DIRECTIONAL:
					Dir[0] = -pLight->Pos.x();
					Dir[1] = -pLight->Pos.y();
					Dir[2] = -pLight->Pos.z();
					Dir[3] = 0.0f;
					PS2Renderer::SetLight(i, Dir, Ambient, Clr, Specular, LIGHT_SPACE_WORLD, LIGHT_TYPE_DIRECTIONAL, 0.0f );
					break;

				default:
					ASSERT( false );	// better be a point or directional light
			}
		}
	}

	// light color multipler is no longer dirty
	LightMgr__ClearLightColorMultDirty();

	// light position is no longer dirty
	_RenderState_LightPositionDirty = FALSE;
}

////////////////////////////////////////////////////////////////////////////

void RenderState__PrepareHardwareLights( void )
{
	// tell the ps2 renderer all the little known facts about the world
	PS2Renderer::SetWorldToCamera(*(PS2Matrix4x4*)&g4WorldToCamera);
	_RenderState_LightPositionDirty = TRUE;
}

// Overrides ///////////////////////////////////////////////////////////////
 
// because of the new structure of the ps2 renderstate stuff, these
// functions have to be implimented differently than the default versions
void RenderState__ForceAll( void )
{
	int i;

	for ( i = 0; i < RS_NUM_STATES; i++ )
	{
		if(!_RenderStates[ i ].locked)
			_RenderStates[ i ].force = TRUE;
	}

	TextureState__ForceAll();
}

////////////////////////////////////////////////////////////////////////////

void RenderState__LockAll( void )
{
	int i;

	for ( i = 0; i < RS_NUM_STATES; i++ )
	{
		_RenderStates[ i ].locked = TRUE;
	}
}

////////////////////////////////////////////////////////////////////////////

void RenderState__UnlockAll( void )
{
	int i;

	for ( i = 0; i < RS_NUM_STATES; i++ )
	{
		_RenderStates[ i ].locked = FALSE;
	}
}

////////////////////////////////////////////////////////////////////////////

void RenderState__SetParticleLights(void)
{
	/*** lighting data order matches constants in Display\PS2Renderer\VUCode\newparticle.vcl ***/

	/* first 3 quad words are directional light directions, in world coords */

	int i;
	ASSERT(RS_MAXLIGHTS == 4);	// we assume max lights (not including ambient light) is 4 since quad word has 4 components
	for (i = 0; i < 4; i++)
	{
		if (i < _RenderState_NumLights)
			MasterList::Append(_RenderState_Lights[i]->Pos.x());
		else
			MasterList::Append(0.0f);
	}
	for (i = 0; i < 4; i++)
	{
		if (i < _RenderState_NumLights)
			MasterList::Append(_RenderState_Lights[i]->Pos.y());
		else
			MasterList::Append(0.0f);
	}
	for (i = 0; i < 4; i++)
	{
		if (i < _RenderState_NumLights)
			MasterList::Append(_RenderState_Lights[i]->Pos.z());
		else
			MasterList::Append(0.0f);
	}

	/* then colors for the 4 lights */

	for (i = 0; i < _RenderState_NumLights; i++)
	{
		MasterList::Append(_RenderState_Lights[i]->Color.R);
		MasterList::Append(_RenderState_Lights[i]->Color.G);
		MasterList::Append(_RenderState_Lights[i]->Color.B);
		MasterList::Append(1.0f);	// alpha
	}
	for (i = _RenderState_NumLights; i < 4; i++)
	{
		MasterList::Append(0.0f);
		MasterList::Append(0.0f);
		MasterList::Append(0.0f);
		MasterList::Append(1.0f);	// alpha
	}

	/* then ambient light color */

	MasterList::Append(_RenderState_GlobalAmbient[0]);
	MasterList::Append(_RenderState_GlobalAmbient[1]);
	MasterList::Append(_RenderState_GlobalAmbient[2]);
	MasterList::Append(_RenderState_GlobalAmbient[3]);
}

////////////////////////////////////////////////////////////////////////////

// unused; defined to clean up #ifdef's

void _RenderState__Skinning( int value )
{
	ASSERT( value == RS_TRUE || value == RS_FALSE );
}

////////////////////////////////////////////////////////////////////////////
