#include "pch.h"
#include <hud/sprite.h>
#include "spriteproperties.h"

SpriteClassPropertyHandler::SpriteClassPropertyHandler( void )
{
	AddProperty( "Sprite",       PROPERTY_TITLE );
	Branch();
	AddProperty( "Position X",   PROPERTY_POSITIONX );
	AddProperty( "Position Y",   PROPERTY_POSITIONY );
	AddProperty( "Width",        PROPERTY_WIDTH );
	AddProperty( "Height",       PROPERTY_HEIGHT);
	AddProperty( "Scale",        PROPERTY_SCALE );
	AddProperty( "Origin",       PROPERTY_ORIGIN );
	AddProperty( "Rotation",     PROPERTY_ROTATION );
	Return();

	AddProperty( "Color",        PROPERTY_TITLE );
	Branch();
	AddProperty( "Base Color",   PROPERTY_BASECOLOR );
	AddProperty( "Family Color", PROPERTY_FAMILYCOLOR );
	AddProperty( "Vertex 1",     PROPERTY_VERTEX1COLOR );
	AddProperty( "Vertex 2",     PROPERTY_VERTEX2COLOR );
	AddProperty( "Vertex 3",     PROPERTY_VERTEX3COLOR );
	AddProperty( "Vertex 4",     PROPERTY_VERTEX4COLOR );
	Return();

	AddProperty( "Texture",      PROPERTY_TITLE );
	Branch();
	AddProperty( "Path",         PROPERTY_TEXTUREPATH );
	AddProperty( "Name",         PROPERTY_TEXTURENAME );
	AddProperty( "Size",         PROPERTY_TEXTURESIZE );
	AddProperty( "Format",       PROPERTY_TEXTUREFORMAT );
	AddProperty( "Cell",         PROPERTY_TEXTURECELL );
	AddProperty( "Rows",         PROPERTY_TEXTUREROWS );
	AddProperty( "Columns",      PROPERTY_TEXTURECOLUMNS );
	AddProperty( "Texture Coordinates", PROPERTY_TITLE );
	
	Branch();
	AddProperty( "Vertex 1",     PROPERTY_VERTEX1TEXTURE );
	AddProperty( "Vertex 2",     PROPERTY_VERTEX2TEXTURE );
	AddProperty( "Vertex 3",     PROPERTY_VERTEX3TEXTURE );
	AddProperty( "Vertex 4",     PROPERTY_VERTEX4TEXTURE );
	Return();
	Return();

	AddProperty( "Drop Shadow",  PROPERTY_TITLE );
	Branch();
	AddProperty( "Enabled",      PROPERTY_SHADOWENABLED );
	AddProperty( "Intensity",    PROPERTY_SHADOWINTENSITY );
	AddProperty( "Position",     PROPERTY_SHADOWPOSITION );
	Return();
}

const char* SpriteClassPropertyHandler::GetKnownClass( void )
{
	return Sprite::ClassnameStatic();
}

void SpriteClassPropertyHandler::SetProperty( const char* pKey, const char* pValue, int PropertyId )
{
	Sprite* psprite = GetKnownSprite();

	if( psprite )
	{
		int inINT = 0;
		float inFLOAT = 0.0f;
		double inDOUBLE = 0.0;
		BOOL inBOOL = FALSE;
		float inX = 0.0f;
		float inY = 0.0f;
		float inW = 0.0f;
		float inZ = 0.0f;
		int inALPHA = 0;
		int inRED   = 0;
		int inGREEN = 0;
		int inBLUE  = 0;

		inINT = atoi( pValue );
		inDOUBLE = atof( pValue );
		inFLOAT = (float) inDOUBLE;

		sscanf( pValue, "%f %f %f %f", &inX, &inY, &inZ, &inW );
		sscanf( pValue, "%3d %3d %3d %3d", &inALPHA, &inRED, &inGREEN, &inBLUE );

		if( 0 == stricmp( pValue, "TRUE" ) )
			inBOOL = TRUE;
		else
		if( 0 == stricmp( pValue, "FALSE" ) )
			inBOOL = FALSE;
		else
		if( inINT != 0 )
			inBOOL = TRUE;
		else
			inBOOL = FALSE;

		switch( PropertyId )
		{
		case PROPERTY_POSITIONX:
			psprite->m_state.position.x = inFLOAT;
			break;

		case PROPERTY_POSITIONY:
			psprite->m_state.position.y = inFLOAT;
			break;

		case PROPERTY_WIDTH:
			psprite->m_state.size.x = inFLOAT;
			break;

		case PROPERTY_HEIGHT:
			psprite->m_state.size.y = inFLOAT;
			break;

		case PROPERTY_SCALE:
			psprite->SetScale( inX, inY );
			break;
		
		case PROPERTY_ORIGIN:
			psprite->SetOrigin( inX, inY );
			break;
		
		case PROPERTY_ROTATION:
			psprite->SetRotation( inFLOAT );
			break;
		
		case PROPERTY_BASECOLOR:
			psprite->SetColor( inRED, inGREEN, inBLUE, inALPHA );
			break;
		
		case PROPERTY_FAMILYCOLOR:
			psprite->SetFamilyColor( inRED, inGREEN, inBLUE, inALPHA );
			break;
		
		case PROPERTY_VERTEX1COLOR:
			psprite->SetVertexColor( 0, inRED, inGREEN, inBLUE, inALPHA );
			break;
		
		case PROPERTY_VERTEX2COLOR:
			psprite->SetVertexColor( 1, inRED, inGREEN, inBLUE, inALPHA );
			break;
		
		case PROPERTY_VERTEX3COLOR:
			psprite->SetVertexColor( 2, inRED, inGREEN, inBLUE, inALPHA );
			break;
		
		case PROPERTY_VERTEX4COLOR:
			psprite->SetVertexColor( 3, inRED, inGREEN, inBLUE, inALPHA );
			break;
		
		case PROPERTY_TEXTUREPATH:
			break;
		
		case PROPERTY_TEXTURENAME:
			break;
		
		case PROPERTY_TEXTURESIZE:
			break;
		
		case PROPERTY_TEXTUREFORMAT:
			break;
		
		case PROPERTY_TEXTURECELL:
			psprite->m_state.textureset.cell = inINT;
			break;
		
		case PROPERTY_TEXTUREROWS:
			psprite->m_state.textureset.rows = inINT;
			break;
		
		case PROPERTY_TEXTURECOLUMNS:
			psprite->m_state.textureset.cols = inINT;
			break;
		
		case PROPERTY_VERTEX1TEXTURE:
			psprite->SetTextureCoordinates( 0, inX, inY );
			break;
		
		case PROPERTY_VERTEX2TEXTURE:
			psprite->SetTextureCoordinates( 1, inX, inY );
			break;
		
		case PROPERTY_VERTEX3TEXTURE:
			psprite->SetTextureCoordinates( 2, inX, inY );
			break;
		
		case PROPERTY_VERTEX4TEXTURE:
			psprite->SetTextureCoordinates( 3, inX, inY );
			break;
		
		case PROPERTY_SHADOWENABLED:
			psprite->m_state.shadow.enabled = inBOOL;
			break;
		
		case PROPERTY_SHADOWINTENSITY:
			psprite->m_state.shadow.intensity = inINT;
			break;
		
		case PROPERTY_SHADOWPOSITION:
			psprite->m_state.shadow.x = (u8) inX;
			psprite->m_state.shadow.y = (u8) inY;
			break;
		}
	}

	return;
}