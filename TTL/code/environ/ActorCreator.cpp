//============================================================================
//=
//= ActorCreator.cpp - creates actors specified in the world maya file
//=                    and initialized them.
//=
//============================================================================

#include "TTLPCH.h"

#include "environ/ActorCreator.h"
#include "mathUtil/MathUtil.h"
#include "GameHelper/SharedBody.h"
#include "main/PlayerDB.h"
#include "stage/ActorCreator.h"
#include "InterfacesTTL/ParticipantIF.h"
#include "Display/TextureUtil.h"

//============================================================================

int ActorCreator::s_combatantLOD = COMBATANT_LOD_HIGH;

#if defined(PS2)
bool ActorCreator::s_useLOD = true;
#else
bool ActorCreator::s_useLOD = false;
#endif

//============================================================================

static void AttachTextureFromDbl( SharedBody* body, const char* slot, const char* filename, int textureIndex );
static void AttachNPCTexture( SharedBody* body, const char* textureSpec );

//============================================================================

bool ActorCreator::InitGameSpecificGeometry( CActor* actor )
{
	if ( InitNPCGeometry( actor ) )
		return true;
	else if ( InitCombatantGeometry( actor ) )
		return true;
	else
		return false;
}

//============================================================================

bool ActorCreator::InitNPCGeometry( CActor* actor )
{
	const char* npcGeom = actor->Parm().GetString( "npc_geom" );
	if ( npcGeom && npcGeom[0] )
	{
		const char* model = actor->Parm().GetString( "npc_model" );
		ASSERT( model && model[0] );

		// load geom
		char filename[ 64 ];
		sprintf( filename, "npc/models/%s.dbl", model );
		t_Handle modelFile = dblLoad( filename );
		ASSERT( modelFile != INVALID_HANDLE );
		ts_Geometry geom;
		dblGetGeometry( &geom, modelFile );
		ASSERT( geom.pSOM );

		// we use the shared body because it supports the swap textures nicely.
		// we can also add accessories.
		ASSERT( !actor->Instance( 0 ) );
		actor->SetGeometry( 0, &geom, -1 );

		// turn into a dynamic instance.  don't clear so it keeps the body.
		SharedBody* body = new SharedBody;
		actor->Instance( 0 )->InitFromDynamicObject( body );
		ASSERT( actor->Instance( 0 )->Body() );

		// fake in a bounding sphere
		actor->Instance( 0 )->BoundingSphere().Radius = 1.0f;	// hack, hardcoded.

		// let the shared body know about the geom.
		body->AddSOM( geom.pSOM, modelFile, "master" );

		// handle swapped textures
		const char* texture = actor->Parm().GetString( "npc_texture" );
		if ( texture && texture[0] )
		{
			AttachNPCTexture( body, texture );
		}

		// is there an accessory?
		const char* accModel = actor->Parm().GetString( "npc_accmodel" );
		if ( accModel && accModel[0] )
		{
			sprintf( filename, "npc/models/%s.dbl", accModel );
			t_Handle accFile = dblLoad( filename );
			ASSERT( accFile != INVALID_HANDLE );
			dblGetGeometry( &geom, accFile );
			ASSERT( geom.pSOM );
			body->AddSOM( geom.pSOM, accFile, "acc" );

			// accessory texture?
			const char* accTexture = actor->Parm().GetString( "npc_acctexture" );
			if ( accTexture && accTexture[0] )
			{
				AttachNPCTexture( body, accTexture );
			}
		}

		body->RefreshTextureSwaps();

		return true;
	}
	else
	{
		return false;
	}
}

//============================================================================

bool ActorCreator::InitCombatantGeometry( CActor* actor )
{
	ParameterSet* parms = NULL;
	bool parmsOwned = false;

	// if we have a "pieces" key then we're built from pieces
	if ( actor->Parm().GetString( "pieces" ) )
	{
		parms = &actor->Parm();
		parmsOwned = false;
	}
	// are we referenceing a prebuilt character?
	else if ( actor->Parm().GetString( "Geometry" ) != NULL )
	{
		int prebuiltIndex = PlayerDB::CharacterIndexFromName( actor->Parm().GetString( "Geometry" ) );
		if ( prebuiltIndex >= 0 )
		{
			parms = new ParameterSet;
			parmsOwned = true;
			parms->SetFromString( PlayerDB::CharacterInitString( prebuiltIndex ) );
		}
	}

	if ( parms )
	{
		// create the root body if we don't have a geometry yet
		if ( !actor->Instance( 0 ) )
		{
			// load (or get handle to already-loaded) master body for its bones
			ts_Geometry master;
			if ( parms->GetString( "model_name" ) == NULL )
				master = *g_loadSOM.Geometry( "avatars\\masterbody.dbl" );
			else
				master = *g_loadSOM.Geometry( parms->GetString( "model_name" ) );

			//
			// create our "body" (the root instance that contains the skeleton)
			//

			// start with a normal geometry and then we'll override it
			actor->SetGeometry( 0, &master, -1 );

			// override. don't clear, so the body and bones that are hanging off the instance will
			// still be valid.  this essentially will remove the geometry pointer and cause
			// all future render calls to use the methods inside SharedBody
			SharedBody* body = new SharedBody;
			actor->Instance( 0 )->InitFromDynamicObject( body );

			// make sure our base model has a body!
			ASSERT( actor->Instance( 0 )->Body() );

			// set bounding sphere
			actor->Instance( 0 )->BoundingSphere().Radius = 1.0f;	// hack, hardcoded.

			// add the master SOM so the first index always has all the bones
			body->AddSOM( master.pSOM, INVALID_HANDLE, "master" );
		}

		// get a handle to the body
		ASSERT( actor->Instance( 0 ) );
		SharedBody* body = (SharedBody*) actor->Instance()->DynamicModel();
		ASSERT_PTR( body );

		// init the pieces
		body->ResetTextureSwaps();
		InitCombatantPieces( body, parms );
		InitCombatantTextures( body, parms );
		body->RefreshTextureSwaps();

		if ( parmsOwned )
		{
			delete parms;
		}

		return true;
	}
	else
	{
		return false;
	}
}

//============================================================================

static void AttachTextureFromDbl( SharedBody* body, const char* slot, const char* filename, int textureIndex )
{
	ASSERT_PTR( body );
	ASSERT_PTR( slot );
	ASSERT_PTR( filename );

	t_Handle fileHandle = dblLoad( filename );
	if ( fileHandle != INVALID_HANDLE )
	{
		TextureDatabase* tdb = dblGetTextures( fileHandle, DEFAULT_CHUNK_KEY_STRING );
		ASSERT( tdb );
		ASSERT( textureIndex < tdb->TextureCount() );
		body->TextureSwap( slot, tdb->GetTexture( textureIndex ), fileHandle );
	}
	else
	{
		ASSERTF( false, ( "can't load file '%s' for body texture '%s'", filename, slot ) );
	}
}

//============================================================================

static void AttachNPCTexture( SharedBody* body, const char* textureSpec )
{
	// texture name is "XXX/YYY", where XXX is the prefix, and XXX_swap is the texture
	// used by the model, and XXX_YYY.dbl is the filaname.
	char* p = strchr( textureSpec, '/' );
	if ( p )
	{
		char swapTexture[ 64 ];
		char filename[ 64 ];

		int len = p - textureSpec;
		ASSERT( len + 6 < sizeof(swapTexture) );
		memcpy( swapTexture, textureSpec, len );
		swapTexture[ len ] = 0;
		sprintf( filename, "npc\\textures\\%s_%s.dbl", swapTexture, textureSpec + len + 1 );
		sprintf( swapTexture + len, "_swap" );

		// attach it
		AttachTextureFromDbl( body, swapTexture, filename, 0 );
	}
}

//============================================================================

static Texture* MakeColorShiftedTexture( Texture* source, int h, int s, int v )
{
	Texture* tex = NULL;

#if defined(PS2) || defined(_XBOX)
	// clone the image so we can modify it
	Image* image;
	source->GetImage()->Clone( &image );

	// make our blend color
	TextureUtil::SetBlendMode( TEXTUREUTIL_ADDITIVE_HSV );
	ts_bRGBA color;
	color.A = 255;
	color.R = h;
	color.G = s;
	color.B = v;

	// apply blend
	int format = image->GetFormat();
	if ( ( format & TYPE_MASK ) == TYPE_CI )
		TextureUtil::BlendPalette( image, 0, image->GetPaletteHeight(), color );
	else
		TextureUtil::BlitColor( image, color, 0, 0, image->GetWidth(), image->GetHeight() );

	// make our texture
	tex = new Texture( NULL, "colorshifted texture" );
	tex->AddImage( false, 0, image->GetFormat(), image->GetWidth(), image->GetHeight(), image->GetImageBuffer(),
		             image->GetFormat(), image->GetPaletteWidth(), image->GetPaletteHeight(), image->GetPaletteBuffer() );

	// release the locks on the buffers (are these grabbed in blend and not released? eew!)
	image->ReleaseImageBuffer();
	image->ReleasePaletteBuffer();
	delete image;
#endif

	return tex;
}

//============================================================================

void ActorCreator::SetCombatantLOD( int lod )
{
	ASSERT( lod == COMBATANT_LOD_LOW || lod == COMBATANT_LOD_MID || lod == COMBATANT_LOD_HIGH );
	s_combatantLOD = lod;
}

//============================================================================

int ActorCreator::GetCombatantLOD()
{
	return s_combatantLOD;
}

//============================================================================

void ActorCreator::SetUseLOD( bool flag )
{
	s_useLOD = flag;
}

//============================================================================

void ActorCreator::MakeCombatantPieceFilename( int piece, char* filename )
{
	ASSERT_PTR( filename );
	if ( s_useLOD )
	{
		if ( s_combatantLOD == COMBATANT_LOD_LOW )
		{
			int lod = PlayerDB::PieceLODReplacement( piece );
			if ( lod >= 0 )
				piece = lod;
		}
		if ( ( s_combatantLOD == COMBATANT_LOD_MID || s_combatantLOD == COMBATANT_LOD_LOW ) &&
			PlayerDB::PieceHasLores( piece ) )
			sprintf( filename, "avatars\\piecelow\\%s", PlayerDB::GetPieceFilename( piece ) );
		else
			sprintf( filename, "avatars\\pieces\\%s", PlayerDB::GetPieceFilename( piece ) );
	}
	else
	{
		sprintf( filename, "avatars\\pieces\\%s", PlayerDB::GetPieceFilename( piece ) );
	}
}

//============================================================================

void ActorCreator::MakeCombatantTextureFilename( int texture, char* filename )
{
	ASSERT_PTR( filename );
#ifdef PS2
	if ( s_combatantLOD == COMBATANT_LOD_LOW )
		sprintf( filename, "avatars\\texlow\\%s", PlayerDB::GetTextureFilename( texture ) );
	else
#endif
		sprintf( filename, "avatars\\textures\\%s", PlayerDB::GetTextureFilename( texture ) );
}

//============================================================================

bool ActorCreator::InitCombatantPieces( SharedBody* body, ParameterSet* parms )
{
	// remove all non "master" pieces, since we're going to rebuild from scratch
	while ( body->NumSOMs() > 1 )
		body->RemoveSOM( 1 );

	char* pieces = parms->GetString( "pieces" );
	while ( pieces && isdigit( pieces[0] ) )
	{
		int id = atoi( pieces );
		char filename[ 128 ];
		MakeCombatantPieceFilename( id, filename );
		t_Handle fileHandle = dblLoad( filename );
		if ( fileHandle )
		{
			ts_Geometry geom;
			dblGetGeometry( &geom, fileHandle );
			if ( geom.pSOM )
				body->AddSOM( geom.pSOM, fileHandle, PlayerDB::GetPieceSlot( id ) );
			else
				ASSERTF( false, ( "piece '%s' has no geometry", filename ) );
		}
		else
			ASSERTF( false, ( "piece '%s' not found", filename ) );

		pieces = strchr( pieces, ',' );
		if ( pieces ) pieces++;
	}

	// hide eyelids by default.
	body->Hide( "eyelids", true );

	return true;
}

//============================================================================

bool ActorCreator::InitCombatantTextures( SharedBody* body, ParameterSet* parms )
{
	char* tex   = parms->GetString( "tex" );

	while ( tex && isdigit( tex[0] ) )
	{
		char filename[ 128 ], swapTexture[ 64 ];
		int id = atoi( tex );
		MakeCombatantTextureFilename( id, filename );
		sprintf( swapTexture, "%s_swap", PlayerDB::GetTexturePrefix( id ) );

		// load the image
		t_Handle texHandle = dblLoad( filename );
		if ( texHandle != INVALID_HANDLE )
		{
			TextureDatabase* tdb = dblGetTextures( texHandle, DEFAULT_CHUNK_KEY_STRING );
			ASSERT_PTR( tdb );
			Texture* texture = tdb->GetTexture( 0 );
			if ( texture )
			{
				//// is this color shifted?
				//char shiftKey[ 16 ];
				//sprintf( shiftKey, "tex%d", id );
				//char* shiftVal = parms->GetString( shiftKey );
				//if ( shiftVal && shiftVal[0] )
				//{
				//	int h, s, v;
				//	sscanf( shiftVal, "%04X%02X%02X", &h, &s, &v );
				//	Texture* shifted = MakeColorShiftedTexture( texture, h, s, v );
				//	if ( shifted )
				//	{
				//		texture = shifted;
				//		body->AddManagedTexture( texture );
				//	}
				//}

				// apply swap
				body->TextureSwap( swapTexture, texture, texHandle );
			}
		}

		tex = strchr( tex, ',' );
		if ( tex ) tex++;
	}

	return true;
}

//============================================================================

CActor* ActorCreator::CreateFromParameters( ParameterSet& parms, Vector3CRef pos, float heading )
{
	// create actor
	const char* actorClass = parms.GetString( "class" );
	ASSERT_PTR( actorClass );
	CActor* actor = CActorCreator::Create( actorClass );
	if ( actor )
	{
		// set the name (used for debugging and for basic identity checking)
		const char* name = parms.GetString( "name" );
		if ( name )
			actor->SetName( name );

		// set the message owner's name
		const char* instanceName = actor->Parm().GetString( "instance" );
		if ( instanceName && ( instanceName[0] != '\0' ) )
			actor->GetMessageOwner().SetOwnerName( "FromParmActor", instanceName );
		else
			actor->GetMessageOwner().SetOwnerName( "FromParmActor", name );

		// pass ownership of the parameter set to the actor
		actor->Parm().SetFromParameterSet( parms );

		// set up the geometry
		ActorInitializer::InitGeometry( actor, ActorCreator::InitGameSpecificGeometry );

		// place in world
		actor->Place( pos, heading );

		// initialize for real
	 	if ( actor->Initialize() )
		{
			// assign ids if the actor is a participant
			if ( actor->GetInterface(CParticipantIF::GetID()) != NULL )
				g_referee.RegisterParticipant(actor);

			// put in the stage
			if ( actor->IsA( CProtagonist::ClassType() ) && actor->Parm().GetInt( "interacts" ) != false )
				g_stage.AddProtagonist( (CProtagonist*) actor );
			else
				g_stage.AddNonProtagonist( actor );

			// delete automatically when level ends
			actor->SetDeleteWhenNoCast( true );

			// load AI script
			ActorInitializer::LoadAIScript( actor, actor->Parm().GetString( "ai" ) );
		}
		else
		{
			// initialize failed
			delete actor;
			actor = NULL;
		}
	}

	return actor;
}

//============================================================================
