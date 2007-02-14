//============================================================================
//=
//= CharacterLoader.cpp - responsible for preloading, tracking, and creating
//=                       the main character models (build from pieces).
//=
//============================================================================

#include "TTLPCH.h"
#include "main/CharacterLoader.h"
#include "Game/Managers/ResourceManager.h"
#include "main/PlayerDB.h"
#include "main/TTLConstants.h"
#include "Components/weapons/weaplib.h"
#include "weapons/loadout.h"
#include "weapons/lobbed.h"
#include "environ/ActorCreator.h"

//============================================================================

#define MAX_PIECES_PER_PLAYER		16
#define MAX_TEXTURES_PER_PLAYER	32
#define MAX_WEAPONS_PER_PLAYER	 8

//============================================================================

namespace CharacterLoader
{
	// local structures
	struct PlayerRecord
	{
		short numPieces;
		short numTextures;
		short numWeapons;
		short pieces[ MAX_PIECES_PER_PLAYER ];
		short	textures[ MAX_TEXTURES_PER_PLAYER ];
		short weapons[ MAX_WEAPONS_PER_PLAYER ];
	};

	// local variables
	PlayerRecord	s_players[ MAX_PLAYERS ];
	s8					s_lienCount[ MAX_PLAYERS ];
	bool				s_queueLoads = false;
	bool				s_pauseLoads = false;

	// local functions
	void ConvertInitString( PlayerRecord* rec, const char* initString, const char* weapons );
	void Insert( int value, short* arr, int arrLen );
	void RefreshLoadedArt( int player );

	// message handlers
	CMessageOwner messageOwner("CharacterLoader");
	SEND_MESSAGE_HANDLER(CCharacterAdded) addHandler;
	SEND_MESSAGE_HANDLER(CCharacterRemoved) removeHandler;
	SEND_MESSAGE_HANDLER(CCharacterChanged) changeHandler;
	SEND_MESSAGE_HANDLER(CAvatarParticipantAdded) participantAddedHandler;
	SEND_MESSAGE_HANDLER(CAvatarParticipantRemoved) participantRemovedHandler;
};

//============================================================================

void CharacterLoader::Reset()
{
	for ( int p = 0 ; p < MAX_PLAYERS ; p++ )
	{
		SetPlayerInitString( p, NULL, NULL );
		s_lienCount[ p ] = 0;
	}
	s_queueLoads = false;
	s_pauseLoads = false;
}

//============================================================================

void CharacterLoader::QueueLoads()
{
	s_queueLoads = true;
	for ( int p = 0 ; p < MAX_PLAYERS ; p++ )
		QueueLoads( p );
}

//============================================================================

void CharacterLoader::QueueLoads( int playerId )
{
	ASSERT( s_queueLoads );
	ASSERT( playerId >= 0 && playerId < MAX_PLAYERS );
	PlayerRecord& r = s_players[ playerId ];

	char buf[ 128 ];
	
	int i;
	for ( i = 0 ; i < r.numPieces ; i++ )
	{
		ActorCreator::MakeCombatantPieceFilename( r.pieces[ i ], buf );
      g_resourceManager.LoadFile( buf, playerId );
	}
	for ( i = 0 ; i < r.numTextures ; i++ )
	{
		ActorCreator::MakeCombatantTextureFilename( r.textures[ i ], buf );
      g_resourceManager.LoadFile( buf, playerId );
	}
	for ( i = 0 ; i < r.numWeapons ; i++ )
	{
		TTLWeaponProperties* prop = (TTLWeaponProperties*) g_weaponLib.GetProperties( r.weapons[ i ] );
		ASSERT_PTR( prop );
		if ( prop )
		{
			for ( int m = 0 ; m < WEAPON_MAX_MODELS ; m++ )
			{
				const char* name = prop->m_model[ m ].name;
				if ( name && name[0] )
				{
					sprintf( buf, "%s.dbl", name );
					g_resourceManager.LoadFile( buf, playerId );
				}
			}
			if ( prop->m_weaponClass == WEAPONCLASS_LOBBED )
			{
				LobbedProperties* lobbedProp = (LobbedProperties*) prop;
				const char* name = lobbedProp->m_inAirModel;
				if ( name && name[0] )
				{
					sprintf( buf, "%s.dbl", name );
					g_resourceManager.LoadFile( buf, playerId );
				}
			}
		}
	}
}

//============================================================================

void CharacterLoader::ClearLoads()
{
	// clear loads from the resource manager
	for ( int p = 0 ; p < MAX_PLAYERS ; p++ )
	{
		ASSERT( s_lienCount[ p ] == 0 );
		g_resourceManager.UnloadGroup( p );
	}

	s_queueLoads = false;
}

//============================================================================

bool CharacterLoader::AllPlayersLoaded()
{
	int count, loaded;
	g_resourceManager.GroupInfo( &count, &loaded, ( 1 << MAX_PLAYERS ) - 1 );
	return ( count == loaded );
}

//============================================================================

void CharacterLoader::RefreshLoadedArt( int playerId )
{
	// only tell the resource manager about this if we've already queued loads.
	if ( s_queueLoads && s_lienCount[ playerId ] == 0 && !s_pauseLoads )
	{
		// unload earlier files.  don't purge in case the new stuff has overlap
		// with already loaded stuff
		g_resourceManager.UnloadGroup( playerId, false );

		// queue loads for this player
		QueueLoads( playerId );

		// now purge in case any resources aren't used
		g_resourceManager.Purge();
	}
}

//============================================================================

void CharacterLoader::SetPlayerInitString( int playerId, const char* initString, const char* weapons )
{
	ConvertInitString( &s_players[ playerId ], initString, weapons );
	RefreshLoadedArt( playerId );
}

//============================================================================

void CharacterLoader::ConvertInitString( PlayerRecord* rec, const char* initString, const char* weapons )
{
	ASSERT_PTR( rec );

	// clear out the record
	memset( rec, 0, sizeof(*rec) );

	if ( initString && initString[0] )
	{
		char* p;
		p = strstr( initString, "pieces=" );
		ASSERT_PTR( p );
		if ( p )
		{
			p += 7;	// skip 'pieces='
			while ( isdigit( p[0] ) )
			{
				if ( rec->numPieces < MAX_PIECES_PER_PLAYER )
				{
					int v = atoi( p );
					Insert( v, rec->pieces, rec->numPieces++ );
				}
				else
					ASSERTS( false, "too many pieces" );

				while ( isdigit( p[0] ) )
					p++;
				if ( p[0] == ',' )
					p++;
			}
		}

		p = strstr( initString, "tex=" );
		ASSERT_PTR( p );
		if ( p )
		{
			p += 4;	// skip 'tex='
			while ( isdigit( p[0] ) )
			{
				if ( rec->numTextures < MAX_TEXTURES_PER_PLAYER )
				{
					int v = atoi( p );
					Insert( v, rec->textures, rec->numTextures++ );
				}
				else
					ASSERTS( false, "too many textures" );

				while ( isdigit( p[0] ) )
					p++;
				if ( p[0] == ',' )
					p++;
			}
		}
	}

	if ( weapons && weapons[0] )
	{
		WeaponLoadout loadout( weapons );
		for ( int i = loadout.Weapons() ; i >= 0 ; i-- )
		{
			const char* name = loadout.GetWeapon( i );
			if ( name && name[0] )
			{
				TTLWeaponProperties* prop = (TTLWeaponProperties*) g_weaponLib.GetProperties( name );
				ASSERT( rec->numWeapons < MAX_WEAPONS_PER_PLAYER );
				if ( prop && rec->numWeapons < MAX_WEAPONS_PER_PLAYER )
					rec->weapons[ rec->numWeapons++ ] = prop->GetIndex();
			}
		}
	}
}

//============================================================================

void CharacterLoader::Insert( int value, short* arr, int arrLen )
{
	// insert into the list sorted
	int i;
	for ( i = arrLen - 1 ; i >= 0 && arr[ i ] > value ; i-- )
		arr[ i + 1 ] = arr[ i ];
	arr[ i + 1 ] = value;
}

//============================================================================

void CharacterLoader::Initialize()
{
	addHandler.Initialize("PlayerAdded", MESSAGE_HANDLE_IF_MASTER, messageOwner);
	removeHandler.Initialize("PlayerRemoved", MESSAGE_HANDLE_IF_MASTER, messageOwner);
	changeHandler.Initialize("PlayerChanged", MESSAGE_HANDLE_IF_MASTER, messageOwner);
	participantAddedHandler.Initialize("AvatarParticipantAdded", MESSAGE_HANDLE_IF_MASTER, messageOwner);
	participantRemovedHandler.Initialize("AvatarParticipantRemoved", MESSAGE_HANDLE_IF_MASTER, messageOwner);
	Reset();
}

//============================================================================

void CharacterLoader::Terminate()
{
	messageOwner.UnregisterAll();
}

//============================================================================

void CharacterLoader::PauseLoads()
{
	s_pauseLoads = true;
}

//============================================================================

void CharacterLoader::ResumeLoads()
{
	s_pauseLoads = false;
	for ( int i = 0 ; i < MAX_PLAYERS ; i++ )
		RefreshLoadedArt( i );
}

//============================================================================

void CharacterLoader::CCharacterAdded::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	if ( ValidSingleton( Referee ) )
	{
		uint8 id = (uint8)i_data;
		CPlayerAgent* agent = g_referee.GetPlayerAgent( id );
		if ( agent != NULL )
		{
			SetPlayerInitString( id, agent->GetCurrentCharacter(), agent->GetCurrentWeaponLoadout() );
		}
	}
}

//============================================================================

void CharacterLoader::CCharacterChanged::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	if ( ValidSingleton( Referee ) )
	{
		uint8 id = (uint8)i_data;
		CPlayerAgent* agent = g_referee.GetPlayerAgent( id );
		if ( agent != NULL )
		{
			SetPlayerInitString( id, agent->GetCurrentCharacter(), agent->GetCurrentWeaponLoadout() );
		}
	}
}

//============================================================================

void CharacterLoader::CCharacterRemoved::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	uint8 id = (uint8)i_data;
	if ( id < MAX_PLAYERS )
	{
		SetPlayerInitString( id, NULL, NULL );
	}
}

//============================================================================

void CharacterLoader::CAvatarParticipantAdded::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	uint8 id = (uint8)i_data;
	if ( id < MAX_PLAYERS )
	{
		s_lienCount[ id ]++;
		ASSERT( s_lienCount[ id ] <= 1 );
	}
}

//============================================================================

void CharacterLoader::CAvatarParticipantRemoved::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	uint8 id = (uint8)i_data;
	if ( id < MAX_PLAYERS )
	{
		s_lienCount[ id ]--;
		ASSERT( s_lienCount[ id ] >= 0 );
		RefreshLoadedArt( id );
	}
}

//============================================================================
