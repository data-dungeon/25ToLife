//============================================================================
//=
//= ActorInitializer.cpp - creates actors specified in the world maya file
//=                    and initialized them.
//=
//============================================================================

#include "Environ/EnvironPCH.h"

#include "Environ/ActorInitializer.h"
#include "Display/Common/TextureAnimatorArray.h"
#include "Script/ScriptVM.h"
#include "Script/ScriptSystem.h"
#include "Display/TextureUtil.h"

//============================================================================

bool ActorInitializer::InitInstanceGeometry( CActor* actor )
{
	// we need an instance to bind to
	const char* instanceName = actor->Parm().GetString( "instance" );
	if ( !instanceName || !instanceName[0] || stricmp( instanceName, "none" ) == 0 )
		return false;

	return actor->SetGeometryFromWorldInstance( 0, instanceName );
}

//============================================================================

bool ActorInitializer::InitReplaceGeometry( CActor* actor )
{
	const char* modelName = actor->Parm().GetString( "model_name" );
	if ( modelName && modelName[ 0 ] && stricmp( modelName, "none" ) != 0 )
	{
		ts_Geometry* geom = g_loadSOM.Geometry( modelName );
		if ( geom )
		{
			actor->SetGeometry( 0, geom );
			return true;
		}
		else
		{
			ASSERTF( false, ( "can't load specified model_name '%s'", modelName ) );
		}
	}
	return false;
}

//============================================================================
bool ActorInitializer::InitGeometry( CActor* actor, GameInitGeometry gameInit )
{
	bool success = false;

	if      ( InitReplaceGeometry( actor ) )
		success = true;
	else if ( gameInit && (*gameInit)( actor ) )
		success = true;
	else if ( InitInstanceGeometry( actor ) )
		success = true;

	if ( success )
	{
		// fill in the instance name from the parameter set
		const char* instanceName = actor->Parm().GetString( "instance" );
		if ( instanceName && instanceName[ 0 ] && actor->Instance( 0 ) )
		{
			actor->Instance( 0 )->SetName( instanceName );
		}
	}
	else
	{
		ASSERTF( false, ( "couldn't init geometry for '%s' (instance='%s')", actor->Name(), actor->Parm().GetString( "instance" ) ) );
	}

	return success;
}

//============================================================================

bool ActorInitializer::InitFromInstance( CActor* actor )
{
	const char* instanceName = actor->Parm().GetString( "instance" );
	if ( instanceName && instanceName[ 0 ] )
	{
		CInstance* instance = g_loadSOM.FindInstanceByName( instanceName );
		if ( !instance )
		{
			ASSERTF( false, ( "instance '%s' not found in world.\n", instanceName ) );
			return false;
		}

		// pull the instance's matrix and set the actor's position and orientation from it
		float shearXY, shearXZ, shearYZ;
		float scaleX,  scaleY,  scaleZ;
		Vector3 trans;
		DirCos3x3 rot;
		bool ret = instance->Matrix().Deconstruct( rot, trans, scaleX, scaleY, scaleZ, shearXY, shearXZ, shearYZ );
		if (!ret)
		{
			ASSERTF( false, ("Invalid instance matrix detected \"%s\"!!!", instanceName) );
			return false;
		}

		// FUBAR HACK! REMOVE POST TTL! SET ALL PLAYER-TYPE CHARACTERS TO 1.0 SCALE
		const char* actorClass = actor->Parm().GetString( "class" );
		if ( actorClass && stricmp( actorClass, "combatant" ) == 0 )
			scaleX = scaleY = scaleZ = 1.0f;
		// FUBAR HACK! REMOVE POST TTL! SET ALL PLAYER-TYPE CHARACTERS TO 1.0 SCALE

		actor->Place( trans, rot );
		actor->Scale().Set( scaleX, scaleY, scaleZ );

		// adjust instance bounding info to match scale
		float maxScale = Math::Max( scaleX, scaleY, scaleZ );
		actor->Instance()->BoundingSphere().Radius *= maxScale;
		actor->Instance()->BoundingBox().Min.x( actor->Instance()->BoundingBox().Min.x() * scaleX );
		actor->Instance()->BoundingBox().Max.x( actor->Instance()->BoundingBox().Max.x() * scaleX );
		actor->Instance()->BoundingBox().Min.y( actor->Instance()->BoundingBox().Min.y() * scaleY );
		actor->Instance()->BoundingBox().Max.y( actor->Instance()->BoundingBox().Max.y() * scaleY );
		actor->Instance()->BoundingBox().Min.z( actor->Instance()->BoundingBox().Min.z() * scaleZ );
		actor->Instance()->BoundingBox().Max.z( actor->Instance()->BoundingBox().Max.z() * scaleZ );

		return true;
	}
	else
	{
		return false;
	}
}

//============================================================================

bool ActorInitializer::LoadAIScript( CActor* actor, const char* scriptFilename )
{
	if ( actor->Script( AI_SCRIPT ) )
	{
		ASSERTS( false, "trying to reload an AI script" );
		g_ScriptSystem.DestroyVM( actor->Script( AI_SCRIPT ) );
		actor->SetScript( AI_SCRIPT, NULL );
	}

	if ( scriptFilename && scriptFilename[ 0 ] && stricmp( scriptFilename, "none" ) != 0 )
	{
		char fullFilename[ 512 ];
		sprintf( fullFilename, "scripts\\%s", scriptFilename );

		ScriptExecutable* exe = g_ScriptSystem.LoadExecutable( fullFilename );
		if ( exe )
		{
			actor->SetScript( AI_SCRIPT, g_ScriptSystem.CreateVM( exe ) );
			actor->Script( AI_SCRIPT )->SetOwner( actor );
			actor->Script( AI_SCRIPT )->RegisterMessageHandlers( &actor->GetMessageOwner() );
			return true;
		}
		else
		{
			ASSERTF( false, ( "error loading AI script '%s'", fullFilename ) );
			return false;
		}
	}
	else
	{
		return false;
	}
}

//============================================================================

bool ActorInitializer::InitVisualPropertiesFromInstance( CActor* actor )
{
	// if we don't have a geometry associated with us, there's nothing
	// to control.
	if ( ! actor->Instance() )
		return false;

	// no vertex colors
	actor->Instance()->SetVertexColorPtr( NULL );

	// get instance
	const char* instanceName = actor->Parm().GetString( "instance" );
	if ( !instanceName || !instanceName[0] || stricmp( instanceName, "none" ) == 0 )
		return false;
	CInstance* instance = g_loadSOM.FindInstanceByName( instanceName );
	if ( !instance )
	{
		ASSERTF( false, ( "instance '%s' not found in world.\n", instanceName ) );
		return false;
	}

	// clone visual properties from instance in world
	actor->Instance()->SetVisualPropertiesFromInstance( instance );

	// Let the actor have a crack at it
	actor->InitializeVisualProperties();

	return true;
}

//============================================================================
// For RGBA blits, BlendTexture = NULL..
// TODO: vals should be pulled from params eventually
#if 0
bool ActorInitializer::TextureBlit ( Image*		IP,
												int			BlitType,
												Image*		BaseTexture,
												char*			BlendTexture,
												int			x,					// offsets
												int			y,					
												int			blit_w,					// blitsize
												int			blit_h,
												ts_Hsv		HSV)
{
	float		R,G,B;
	ts_bRGBA	c;
	Image*	ImageCopy;
	int		endindex = 0;

   void	*IPB  = IP->GetImageBuffer();
	void	*IPP  = IP->GetPaletteBuffer();
	if ( !IPB ) return false;

	// get texture info
	int width  = IP->GetWidth();
	int height = IP->GetHeight();
	int size = IP->GetSize();
	int palsize = ( IP->GetPaletteWidth() * IP->GetPaletteHeight() ) * ( TextureUtil::GetBPP(IP) / 8);
	int format = IP->GetFormat();

	// which blit
	switch (BlitType)
	{

	// texture replace
	case	TEXTUREUTIL_TEXTURE_REPLACE:		// TOFIX

		// copy the texture
//		TextureCopy = new char [ size ];
//		memcpy (TextureCopy, BaseTexture, size );
//		TextureUtil::SetBlendMode (TEXTUREUTIL_REPLACE);
//		TextureUtil::SetAlphaMode (TEXTUREUTIL_DST_ALPHA);
//		TextureUtil::BlitTexture ( TextureCopy, BlendTexture, x, y, blit_w, blit_h, width, height, format );
//		memcpy (IPB, TextureCopy, size );
//		IP->ReleaseImageBuffer(true);
//		delete [] TextureCopy;
		return true;

	// texture additive
	case	TEXTUREUTIL_TEXTURE_ADDITIVE:		// TOFIX

		// copy the texture
//		TextureCopy = new char [ size ];
//		memcpy (TextureCopy, BaseTexture, size );
//		TextureUtil::SetBlendMode (TEXTUREUTIL_ADDITIVE);
//		TextureUtil::SetAlphaMode (TEXTUREUTIL_DST_ALPHA);
//		TextureUtil::BlitTexture ( TextureCopy, BlendTexture, x, y, blit_w, blit_h, width, height, format );
//		memcpy (IPB, TextureCopy, size );
//		IP->ReleaseImageBuffer(true);
//		delete [] TextureCopy;
		return true;

	// color additive
	case	TEXTUREUTIL_COLOR_ADDITIVE:			// working

		TextureUtil::HSVtoRGB(&R, &G, &B, HSV.H, HSV.S, HSV.V);
		c.R = (u8)R;	
		c.G = (u8)G;   
		c.B = (u8)B; 
		#if !defined (PS2)
		c.A = 255;		
		#else
		c.A = 128;
		#endif

		// modify the texture (16/24/32)
		if ( (format & TYPE_MASK) != TYPE_CI)
		{
			BaseTexture->Clone(&ImageCopy);
			TextureUtil::SetBlendMode (TEXTUREUTIL_ADDITIVE);
			TextureUtil::SetAlphaMode (TEXTUREUTIL_DST_ALPHA);
			TextureUtil::BlitColor(ImageCopy, c, x, y, blit_w, blit_h);
			memcpy (IPB, ImageCopy->GetImageBuffer(), size);
			ImageCopy->ReleaseImageBuffer(true);
			delete ImageCopy;
			break;
		}
		else	// modify the palette
		{
			BaseTexture->Clone(&ImageCopy);
			TextureUtil::SetBlendMode (TEXTUREUTIL_ADDITIVE);
			TextureUtil::SetAlphaMode (TEXTUREUTIL_DST_ALPHA);

			if ( (format & SIZE_MASK) == SIZE_8_BIT)
				endindex = 255;
			else if ( (format & SIZE_MASK) == SIZE_4_BIT )
				endindex = 15;

			TextureUtil::BlendPalette(ImageCopy, 0, endindex, c);
			
			memcpy (IPP, ImageCopy->GetPaletteBuffer(), palsize);
			ImageCopy->ReleasePaletteBuffer(true);
			delete ImageCopy;

			break;
		}

	// color replace
	case	TEXTUREUTIL_COLOR_REPLACE:				// working

		TextureUtil::HSVtoRGB(&R, &G, &B, HSV.H, HSV.S, HSV.V);
		c.R = (u8)R;	
		c.G = (u8)G;   
		c.B = (u8)B;   
		#if !defined (PS2)
		c.A = 255;		
		#else
		c.A = 128;
		#endif

		// modify the texture (16/24/32)
		if ( (format & TYPE_MASK) != TYPE_CI)
		{
			BaseTexture->Clone(&ImageCopy);
			TextureUtil::SetBlendMode (TEXTUREUTIL_REPLACE);
			TextureUtil::SetAlphaMode (TEXTUREUTIL_DST_ALPHA);
			TextureUtil::BlitColor(ImageCopy, c, x, y, blit_w, blit_h);
			memcpy (IPB, ImageCopy->GetImageBuffer(), size);
			ImageCopy->ReleaseImageBuffer();
			delete ImageCopy;
			break;
		}
		else	// modify the palette
		{
			BaseTexture->Clone(&ImageCopy);
			TextureUtil::SetBlendMode (TEXTUREUTIL_REPLACE);
			TextureUtil::SetAlphaMode (TEXTUREUTIL_DST_ALPHA);

			if ( (format & SIZE_MASK) == SIZE_8_BIT)
				endindex = 255;
			else if ( (format & SIZE_MASK) == SIZE_4_BIT )
				endindex = 15;

			TextureUtil::BlendPalette(ImageCopy, 0, endindex, c);
			memcpy (IPP, ImageCopy->GetPaletteBuffer(), palsize);
			ImageCopy->ReleasePaletteBuffer();
			delete ImageCopy;
			break;
		}

	default:
		return false;
		break;
	}

	IP->ReleaseImageBuffer(true);
	IP->ReleasePaletteBuffer();

	#if defined (PS2)
	IP->Release();
	#endif

	return true;
}
#endif
