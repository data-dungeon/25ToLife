//============================================================================
//
// CustomPlayer.cpp
//
// Handles switching of player pieces, loading/saving to memcard etc
//
//============================================================================

#include "TTLPCH.h"
#include "frontend/customplayer.h"
#include "platform/MemoryCard/MemoryCard.h"
#include "effects/pain_decal.h"
#include "frontend/savegame.h"
#include "main/PlayerDB.h"
#include "stage/ActorCreator.h"
#include "environ/ActorCreator.h"
#include "camera/generalcam/generalcam.h"
#include "weapons/armory.h"

#ifdef PS2
#include "Display/PS2/Renderer/Code/MasterList.h"
#endif

//============================================================================

namespace CustomPlayer
{
	int				s_visibleProfile = -1;
	int				s_editProfile = -1;
	CCombatant*		s_actor = NULL;
	GeneralCamera*	s_camera = NULL;
	float				s_bodyPartZoom = 0.8f;
	char*				s_bodyPartName = "spine";
	float				s_fov = 0.0f;
	DirCos3x3		s_copBodyToWorld;
	DirCos3x3		s_gangBodyToWorld;
	char*				s_camLocatorName = "BodyCam";
};

#define MINIMUM_FREE_THRESHOLD	( 3000 * 1024 )

//============================================================================

void CustomPlayer::SetEditProfileString( const char* initString )
{
	ASSERT( s_editProfile >= 0 && s_editProfile < PlayerDB::NumCharacters() );

	PlayerDB::SetCharacter( s_editProfile,
									PlayerDB::CharacterName( s_editProfile ),
									initString,
									PlayerDB::CharacterType( s_editProfile ),
									true );

	Show( s_editProfile );
}

//============================================================================

void CustomPlayer::SetEditProfile( int index )
{
	s_editProfile = index;
}

//============================================================================

int CustomPlayer::GetEditProfile()
{
	return s_editProfile;
}

//============================================================================

void CustomPlayer::SaveActor()
{
	CCombatant		*actor; 

	actor = (CCombatant*) g_stage.MasterCast().FindActorByInstanceName( "createcop" );
	s_copBodyToWorld = actor->GetBodyToWorld();
	actor = (CCombatant*) g_stage.MasterCast().FindActorByInstanceName( "creategang" );
	s_gangBodyToWorld = actor->GetBodyToWorld();
}

//============================================================================

void CustomPlayer::RestoreActor()
{
	CCombatant		*actor; 

	actor = (CCombatant*) g_stage.MasterCast().FindActorByInstanceName( "createcop" );
	actor->GetBodyToWorld() = s_copBodyToWorld;
	if ( actor->GetComponentByName("Biped") )
	{
		actor->GetComponentByName("Biped")->SyncActor();
	}

	actor = (CCombatant*) g_stage.MasterCast().FindActorByInstanceName( "creategang" );
	actor->GetBodyToWorld() = s_gangBodyToWorld;
	if ( actor->GetComponentByName("Biped") )
	{
		actor->GetComponentByName("Biped")->SyncActor();
	}
}

//============================================================================

int CustomPlayer::GetVisibleProfile()
{
	ASSERT( s_visibleProfile >= 0 && s_visibleProfile < PlayerDB::NumCharacters() );
	return s_visibleProfile;
}

//============================================================================

void CustomPlayer::Show( int profile )
{
	ASSERT( profile >= 0 && profile < PlayerDB::NumCharacters() );
	s_visibleProfile = profile;

	bool isCop = PlayerDB::CharacterType( profile ) == PLAYERDB_COP;

	// focus our actor on the right guy
	s_actor = (CCombatant*) g_stage.MasterCast().FindActorByInstanceName( isCop ? "createcop" : "creategang" );
	ASSERT_PTR( s_actor );
	ASSERT( s_actor->IsA( CCombatant::ClassType() ) );

	// make new parameter set for actor
	s_actor->Parm().Clear();
	s_actor->Parm().SetFromString( PlayerDB::CharacterInitString( profile ) );
	s_actor->Parm().Set( "sex", "m" );
	s_actor->Parm().Set( "role", isCop ? "cop" : "gang" );

	// motion
	s_actor->LoadMotion( isCop ? "avatars/police.mdb" : "avatars/gangster.mdb", 0, 0 );

	// init from parameter set
	ActorCreator::InitGameSpecificGeometry( s_actor );

	// purge art beyond memory threshhold
	int freeBytes;
	Layers__GetMemoryStats( &freeBytes, NULL, NULL, NULL, NULL );
	if ( freeBytes < MINIMUM_FREE_THRESHOLD )
		ReleaseArtResources();

	// restart anim in case cop/gang changed
	s_actor->StartAnimation( "RoamUpright", true, true );

	// focus the camera on the right guy
	CustomPlayer::UpdateCamera();
}

//============================================================================

void CustomPlayer::UpdateCamera( char *name )
{
	// head targeting...
	if(       !strcmp( "Head" , name ) )	{ s_bodyPartName = "head";				s_camLocatorName = "HeadCam"; s_bodyPartZoom = 0.35f; }
	else if ( !strcmp( "Arms" , name ) )	{ s_bodyPartName = "rightforearm";	s_camLocatorName = "ArmsCam"; s_bodyPartZoom = 0.35f; }
	else if ( !strcmp( "Hands", name ) )	{ s_bodyPartName = "rightforearm";	s_camLocatorName = "HandsCam"; s_bodyPartZoom = 0.45f; }
	else if ( !strcmp( "Torso", name ) )	{ s_bodyPartName = "spine";			s_camLocatorName = "BodyCam"; s_bodyPartZoom = 0.45f; }
	else if ( !strcmp( "Legs" , name ) )	{ s_bodyPartName = "rightupleg";		s_camLocatorName = "LegsCam"; s_bodyPartZoom = 0.45f; }
	else if ( !strcmp( "Feet" , name ) )	{ s_bodyPartName = "rightfoot";		s_camLocatorName = "FeetCam"; s_bodyPartZoom = 0.35f; }
	else                                   { s_bodyPartName = "spine";			s_camLocatorName = "BodyCam"; s_bodyPartZoom = 0.8f;  }

	// now focus
	UpdateCamera();
}

//============================================================================

void CustomPlayer::UpdateCamera()
{
	ASSERT_PTR(s_camera);
	if (s_camera)
	{
		char camName[ 40 ];
		bool isCop = PlayerDB::CharacterType( s_visibleProfile ) == PLAYERDB_COP;

		// get the target
	//	Vector3 target = CustomPlayer::GetBoneInWorld( *s_actor , s_bodyPartName );
	//	target.x( target.x() - 0.5f );

		// get the eye
		strcpy( camName , isCop ? "cop" : "gang" );
		strcat( camName ,s_camLocatorName );
		ts_NamedPosition *np = g_loadSOM.FindNamedPoint( camName );
		strcat( camName , "LookAt" );
		ts_NamedPosition *npLookAt = g_loadSOM.FindNamedPoint( camName );

		// eye
		ASSERT( np );
		Vector3 eye( 0 , 0 , 0 );
		if( np )
		{
			eye = np->vPos;
		}

		// target
		ASSERT( npLookAt ); // might need to get frontend.tup if this assert happens
		Vector3 target( 0 , 0 , 0 );
		if( npLookAt )
		{
			target = npLookAt->vPos;
		}

		s_camera->SetFOV( s_fov * s_bodyPartZoom ); 
		s_camera->SetFocalPointFilter( .5f );
		s_camera->SetEyePointFilter( .5f );
		s_camera->SetPosition( eye );
		s_camera->SetFocalPoint( target );
	}
}

//============================================================================

void CustomPlayer::RotateBody( float angle )
{
	s_actor->GetBodyToWorld().CatYRotation( angle * (2.0f*3.1415927f/360.0f) );
	if ( s_actor->GetComponentByName("Biped") )
	{
		s_actor->GetComponentByName("Biped")->SyncActor();
	}
}

//============================================================================

Vector3 CustomPlayer::GetBoneInWorld( CActor &actor, const char *boneName )
{
	Vector3 pos;

	if( !boneName || !boneName[0] )
		pos = actor.GetBodyInWorld();
	else
	{
		BoneID boneID;
		int ret = boneGetBoneID( actor.Instance()->Body() , &boneID , (char *)boneName );
		if( !ret )
			pos = actor.GetBodyInWorld();
		else
		{
			boneGetStartPosition( actor.Instance()->Body() , boneID , &pos );
			pos = pos * actor.Instance()->Matrix();
		}
	}

	return pos;
}

//============================================================================

void	CustomPlayer::Intro( void )
{
	CustomPlayer::SaveActor();
	ASSERT_PTR(s_camera);
	if (s_camera)
	{
		s_fov = s_camera->GetFOV();
	}
	Show( g_GlobalProfile.GetSelectedCharacter( CPlayerProfile::GANG ) );
	Show( g_GlobalProfile.GetSelectedCharacter( CPlayerProfile::COP ) );
}

//============================================================================

void	CustomPlayer::Exit( void )
{
	ASSERT_PTR(s_camera);
	if (s_camera)
	{
		s_camera->SetHither( 1.0f );
		s_camera->SetFOV( s_fov );
	}
	CustomPlayer::RestoreActor();

	// flush dbl pieces we loaded while we were here
	ReleaseArtResources();
}

//============================================================================

void CustomPlayer::ReleaseArtResources()
{
	#if defined(PS2)
	// make sure we're done using them!
	MasterList::Flush();
	#endif

	// free unused dbls
	dblPurgeUnlocked();
}

//============================================================================

void	CustomPlayer::EquipLoadout( const char* loadoutString )
{	
	CCombatant		*actor;

	const char* actorName = "invalid";

	if( PlayerDB::CharacterType( s_editProfile ) & PLAYERDB_COP )
		actorName = "createcop";
	else
	if( PlayerDB::CharacterType( s_editProfile ) & PLAYERDB_GANG )
		actorName = "creategang";

	actor = (CCombatant*) g_stage.MasterCast().FindActorByInstanceName( actorName );

	if( actor )
	{
		WeaponInventory *inv = (WeaponInventory *) actor->GetComponentByName( "WeaponInventory" );
		if (inv)
		{
			WeaponLoadout loadout( loadoutString );
			loadout.Build(*inv, true);
			WeaponLoadout::DebugDump(*inv);
		}
	}

	return;
}

//============================================================================

void	CustomPlayer::EquipWeapon( int n )
{	
	CCombatant		*actor;

	const char* actorName = "invalid";

	if( PlayerDB::CharacterType( s_editProfile ) & PLAYERDB_COP )
		actorName = "createcop";
	else
	if( PlayerDB::CharacterType( s_editProfile ) & PLAYERDB_GANG )
		actorName = "creategang";

	actor = (CCombatant*) g_stage.MasterCast().FindActorByInstanceName( actorName );

	if( actor )
	{
		WeaponInventory* inv = (WeaponInventory*) actor->GetComponentByName( "WeaponInventory" );

		if( inv )
			inv->Select(n);
	}

	return;
}


//============================================================================

u32 CustomPlayer::CreateCheckSum( ts_CustomProfile* buf )
{
	// +4 to skip checksum at beginning of block
	return Hash::CalcChecksum( (char*)buf + 4, sizeof(*buf) - 4 );
}

//============================================================================

void CustomPlayer::SetCamera( GeneralCamera* cam )
{
	s_camera = cam;
	if (s_camera)
	{
		s_camera->SetHither( .1f );
	}
}

//============================================================================
