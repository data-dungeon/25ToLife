//-----------------------------------------------------------
//
// SaveGame.cpp
//
// saving of the game
//
//-----------------------------------------------------------

#include "TTLPCH.h"
#include "platform/MemoryCard/MemoryCard.h"
#include "main/PlayerDB.h"
#include "frontend/savegame.h"
#include "environ/ConfigMgr.h"
#include "GameHelper/encryption/Helix/Helix.h"
#include "EngineHelper/LoadScreen.h"
#include "weapons/armoryview.h"
#include "Display/TextureUtil.h"

#ifdef SN_TARGET_PS2
#include <netz.h>
#include <platform/stack/ps2common/ps2networking.h>
#include <platform/stack/ps2common/ps2module.h>
#include <platform/stack/ps2common/ps2profilelist.h>
#endif

#ifdef DIRECTX_PC
#include "main/TTL.h"
#include "Interfaces/InventoryListIF.h"
#endif

ts_SaveProfile		g_GlobalProfile;
ts_SaveGameParams	g_SaveGameParams;

//-----------------------------------------------------------
void	CSaveGame::Init()
{
	// set the ext
	g_MemCardManager->SetExt(SAVEGAME_EXT);

	// set to autosave as default
	g_MemCardManager->SetAutosave(false);

	// set poll cards = 1 since we are only using Port 1
	g_MemCardManager->SetNumCardsToPoll(1);

	// update options
	CGameSettings::LoadSettings();
}

//-----------------------------------------------------------
void	CSaveGame::SetProfileName(char* Profile)
{
	strcpy(g_GlobalProfile.ProfileName, Profile);
}

//-----------------------------------------------------------
char*	CSaveGame::GetProfileName()
{
	return (g_GlobalProfile.ProfileName);
}

//-----------------------------------------------------------
void	CSaveGame::StartupCheck(int Port)
{
}

bool	CSaveGame::Save(char* FileName, int Port, int SaveType)
{
	return true;
}

//-----------------------------------------------------------
bool	CSaveGame::SavePatch(char* FileName, int Port, char* Buffer, int DataSize)
{
	#if defined (PS2)
	g_MemCardManager->SetExt(PATCH_EXT);
	#endif

	g_MemCardManager->SetUsePopups(true);
	g_MemCardManager->SetOverwriteConfirm(true);
	g_MemCardManager->SetAreYouReallySure(false);
	g_MemCardManager->SetUseActualFileSize(true);
	g_MemCardManager->SaveFile(SAVEGAME_FILENAME, FileName, Port, Buffer, DataSize, false, MC_SAVED_PATCH);
	g_MemCardManager->WaitUntilIdle();
	return true;
}

//-----------------------------------------------------------
bool	CSaveGame::LoadPatch(char* FileName, int Port, char* Buffer, int DataSize)
{
	#if defined (PS2)
	g_MemCardManager->SetExt(PATCH_EXT);
	#endif

	g_MemCardManager->SetUsePopups(true);
	g_MemCardManager->LoadFile(SAVEGAME_FILENAME, FileName, Port, Buffer, DataSize, false, MC_SAVED_PATCH);
	g_MemCardManager->WaitUntilIdle();
	return true;
}

//-----------------------------------------------------------
bool	CSaveGame::LoadHeader(char* FileName, int Port, char* Buffer)
{
	#if defined (PS2)
	g_MemCardManager->SetExt(PATCH_EXT);
	#endif

	g_MemCardManager->SetUsePopups(true);
	g_MemCardManager->LoadHeader(SAVEGAME_FILENAME, FileName, Port, Buffer, false, MC_SAVED_PATCH);
	g_MemCardManager->WaitUntilIdle();
	return true;
}

//-----------------------------------------------------------
bool	CSaveGame::AutoSave()
{
	
	char	NewFileName[128];

	// set filename ext
	g_MemCardManager->SetExt(SAVEGAME_EXT);

	// setup fname
	sprintf(NewFileName, "%s%s", SAVEGAME_PREFIX, g_SaveGameParams.ProfileName);

	// set profile name... in this case its the same as what we pass in for a filename
	SetProfileName(g_SaveGameParams.ProfileName);

	// get custom chars from player database
	g_GlobalProfile.SaveCustomChars();

	// Copy game locks from GameLock system.
	g_GlobalProfile.SaveGameLocks();

	// update settings
	CGameSettings::SaveSettings();

	//g_MemCardManager->SetUsePopups(true);
	g_MemCardManager->SetOverwriteConfirm(true);
	g_MemCardManager->SetAreYouReallySure(false);
	g_MemCardManager->SetUseActualFileSize(false);
#ifdef DIRECTX_PC
	g_MemCardManager->SaveFile(SAVEGAME_FILENAME, NewFileName, g_SaveGameParams.Port, (char*)&g_GlobalProfile, sizeof(g_GlobalProfile), false, MC_SAVED_GAME);
#else //DIRECTX_PC
	g_MemCardManager->SaveFile(SAVEGAME_FILENAME, NewFileName, g_SaveGameParams.Port, (char*)&g_GlobalProfile, sizeof(g_GlobalProfile), true, MC_SAVED_GAME);
	g_MemCardManager->WaitUntilIdle();
#endif

	return true;
}

//-----------------------------------------------------------
// this version is for the PC to load the file and not use the
// fake 'memory card' stuff for the PC 
bool	CSaveGame::LoadImmediately(char* FileName)
{
	int					i;
	int					csum;
	ts_SaveProfile		LoadedProfile;
	MEMCARD_RESULT		result;

	char	NewFileName[128];
	sprintf(NewFileName, "%s%s", SAVEGAME_PREFIX, FileName);

	// set filename ext
	g_MemCardManager->SetExt(SAVEGAME_EXT);

	g_MemCardManager->SetFilesize(sizeof(ts_SaveProfile));
	g_MemCardManager->CreateLoadSaveBuffer(sizeof(ts_SaveProfile));
	g_MemCardManager->SetBuffer((char*)&LoadedProfile);
	g_MemCardManager->SetFilename(SAVEGAME_FILENAME);
	g_MemCardManager->SetSaveType(MC_SAVED_GAME);
	g_MemCardManager->SetComment(NewFileName);
	g_MemCardMgr.SetFileName(g_MemCardManager->GetFilename());
	g_MemCardMgr.UpdateFileComment(g_MemCardManager->GetComment());

	result = g_MemCardMgr.Load( 0, g_MemCardManager->GetLoadSaveBuffer(), g_MemCardManager->GetFilesize() + sizeof(ts_MemcardHeader) + g_helix.GetEncryptionHeaderSize(), READ_ALL);

	if (g_MemCardManager->GetResult() != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->ResetLoadSaveBuffer();
		return false;
	}

	// is our header valid??
	ts_MemcardHeader*	hdr = (ts_MemcardHeader*) g_MemCardManager->GetLoadSaveBuffer();
	if (hdr->version != MEMCARD_VERSION)
	{
		g_MemCardManager->ResetLoadSaveBuffer();
		return false;
	}

	if (g_MemCardManager->GetCompress() == true)
	{
		if (hdr->compressed)
			g_MemCardManager->UncompressData(hdr->compression_type);
		else
		{
			g_MemCardManager->ResetLoadSaveBuffer();
			return false;
		}
	}

	if (g_MemCardManager->GetEncrypt() == true)
	{
		if (hdr->encrypted)
			g_MemCardManager->UnencryptData(hdr->encryption_type);
		else
		{
			g_MemCardManager->ResetLoadSaveBuffer();
			return false;
		}
	}

	g_MemCardManager->SetLoadedAppVersion1(hdr->appVersion1);
	g_MemCardManager->SetLoadedAppVersion2(hdr->appVersion2);

	if (g_MemCardManager->GetCompress() == false && g_MemCardManager->GetEncrypt() == false)
		memcpy(g_MemCardManager->GetBuffer(), g_MemCardManager->GetLoadSaveBuffer() + sizeof(ts_MemcardHeader), g_MemCardManager->GetFilesize());

	// check savegame date version ID.. make ppl use new save games per build
	if (g_MemCardManager->GetLoadedAppVersion2() != SAVEGAME_VER)
	{
		g_MemCardManager->ResetLoadSaveBuffer();
		return(false);
	}

	// check version number
	if (g_MemCardManager->GetLoadedAppVersion1() != PlayerDB::GetVersionID())		
	{
		g_MemCardManager->ResetLoadSaveBuffer();
		return(false);
	}
	
	// check saved player checksums
	for (i = 0; i < CS_MAX_CHARACTERS; i++)
	{
		csum = CustomPlayer::CreateCheckSum(&LoadedProfile.Characters[i]);
		if (csum != LoadedProfile.Characters[i].CheckSum)
		{
			g_MemCardManager->ResetLoadSaveBuffer();
			return(false);
		}
	}

	// copy over the struct now
	memcpy(&g_GlobalProfile, &LoadedProfile, sizeof(ts_SaveProfile));

	// Set the selected characters (call the function with the already set data? YOU BET. We need secondary modules to know too.)
	g_GlobalProfile.SelectCharacter( CPlayerProfile::COP, g_GlobalProfile.GetSelectedCharacter( CPlayerProfile::COP ) );
	g_GlobalProfile.SelectCharacter( CPlayerProfile::GANG, g_GlobalProfile.GetSelectedCharacter( CPlayerProfile::GANG ) );

	// update game locks
	g_GlobalProfile.LoadGameLocks();

	// update options
	CGameSettings::LoadSettings();
	g_MemCardManager->ResetLoadSaveBuffer();

	return true;
}

//-----------------------------------------------------------
bool	CSaveGame::Delete(char* FileName, int Port, int LoadType)
{		
	char	NewFileName[128];
	sprintf(NewFileName, "%s%s", SAVEGAME_PREFIX, FileName);

	// set filename ext
	g_MemCardManager->SetExt(SAVEGAME_EXT);

	g_MemCardManager->SetUsePopups(true);
	g_MemCardManager->DeleteFile(SAVEGAME_FILENAME, NewFileName, Port, LoadType);
	g_MemCardManager->WaitUntilIdle();
	
	if (g_MemCardManager->GetResult() != MEMCARD_RESULT_OK)
		return false;
	else
		return true;
}

int CSaveGame::LoadNetworkConfiguration( void *list )
{
	return (0);
}



//-----------------------------------------------------------
bool	CSaveGame::Load(char* FileName, int Port, int LoadType)
{
	return true;
}

//=============================================================================
// ts_SaveProfile::ResetToDefaults
//=============================================================================

void ts_SaveProfile::ResetToDefaults( void )
{
	memset( this, 0, sizeof(ts_SaveProfile) );

	// the default profile name should be "DefaultProfile"
	SetName( "DefaultProfile" );

	// default autosave
	SetAutoSave( false );

	// default game settings
	SetDifficulty( PROFILE_DEFAULT_DIFFICULTY );
	SetStereoMode( PROFILE_DEFAULT_STEREO );
	SetGammaValue( PROFILE_DEFAULT_GAMMA );
	TextureUtil::SetGammaOnLoadFlag(false);

	SetMusicVolume( PROFILE_DEFAULT_MUSIC_VOLUME );
	SetSoundVolume( PROFILE_DEFAULT_SOUND_VOLUME );

	// default controller settings
	SetControllerSettings( PROFILE_DEFAULT_INVERTEDAIMING,
								  PROFILE_DEFAULT_VIBRATIONENABLED,
								  PROFILE_DEFAULT_LOOKSENSITIVITY,
								  PROFILE_DEFAULT_LOOKSPEED );

	// default thumbstick/button layout
	SetButtonLayout( PROFILE_DEFAULT_BUTTONLAYOUT );
	SetThumbStickMode( PROFILE_DEFAULT_THUMBSTICKMODE );

	// set a default cop and gangster
	SelectCharacter( CPlayerProfile::COP, PlayerDB::ChangeCharacter( PlayerDB::CharacterIndexFromId( PLAYERDB_CUSTOM ) - 1, 1, PLAYERDB_COP ) );
	SelectCharacter( CPlayerProfile::GANG, PlayerDB::ChangeCharacter( PlayerDB::CharacterIndexFromId( PLAYERDB_CUSTOM ) - 1, 1, PLAYERDB_GANG ) );

	// single player stuff
	SetCurrentLevel(0);
	SetPreviousLevel(0);

	// relock everything
	GameLocks::LockAll();

	SetCustomLoadout( 0, ArmoryView::GetDefaultLoadout( ArmoryView::HumanCop, 0 ) );
	SetCustomLoadout( 1, ArmoryView::GetDefaultLoadout( ArmoryView::HumanCop, 1 ) );
	SetCustomLoadout( 2, ArmoryView::GetDefaultLoadout( ArmoryView::HumanCop, 2 ) );
	SetCustomLoadout( 3, ArmoryView::GetDefaultLoadout( ArmoryView::HumanCop, 3 ) );

	SetCustomLoadout( 4, ArmoryView::GetDefaultLoadout( ArmoryView::HumanGangster, 0 ) );
	SetCustomLoadout( 5, ArmoryView::GetDefaultLoadout( ArmoryView::HumanGangster, 1 ) );
	SetCustomLoadout( 6, ArmoryView::GetDefaultLoadout( ArmoryView::HumanGangster, 2 ) );
	SetCustomLoadout( 7, ArmoryView::GetDefaultLoadout( ArmoryView::HumanGangster, 3 ) );

	// set the default system link match config
	if( ValidSingleton(ConfigMgr) )
		SetSystemLinkMatchConfig( g_configmgr.GetOptions() );
	else
		SetSystemLinkMatchConfig( "" );

	// default tag
	SetTagString( "MyTag" );
	SetTagColor( 0,   0,   0,   0, 255 );
	SetTagColor( 1, 238,  42,  31, 255 );
	SetTagColor( 2, 255, 255, 255, 255 );

	SetDefaultVoiceParams();

	SetBlood( PROFILE_DEFAULT_BLOOD );
}

//=============================================================================
// ts_SaveProfile::SetName
//=============================================================================

void ts_SaveProfile::SetName( const char* pProfileName )
{
	int n = sizeof(ProfileName)/sizeof(ProfileName[0])-1;

	memset( ProfileName, 0, sizeof(ProfileName) );

	if( pProfileName )
		strncpy( ProfileName, pProfileName, n );

	return;
}

//=============================================================================
// ts_SaveProfile::GetName
//=============================================================================

const char* ts_SaveProfile::GetName( void )
{
	return ProfileName;
}

//=============================================================================
// ts_SaveProfile::SetAutoSave
//
//		Set "autosaving" to enabled or disabled.
//=============================================================================

void ts_SaveProfile::SetAutoSave( bool Enabled )
{
	GameSettings.AutoSave = Enabled;
}

//=============================================================================
// ts_SaveProfile::GetAutoSave
//
//		Returns "autosave" enabled or disabled.
//=============================================================================

bool ts_SaveProfile::GetAutoSave( void )
{
	return GameSettings.AutoSave;
}

//=============================================================================
// ts_SaveProfile::SetCustomLoadout
//
//		Sets the loadout for the specified custom character profile.
//=============================================================================

void ts_SaveProfile::SetCustomLoadout( int ch, const char* pLoadoutString )
{
	if( ValidCustomChar(ch) )
	{
		unsigned int c = sizeof(Characters[0].Loadout) / sizeof(Characters[0].Loadout[0]) - 1;

		if( pLoadoutString == 0 )
			pLoadoutString = "";

		strncpy( Characters[ch].Loadout, pLoadoutString, c );
		Characters[ch].Loadout[c] = 0;
	}

	return;
}

//=============================================================================
// ts_SaveProfile::GetCustomLoadout
//
//		Returns the loadout for the specified custom character profile.
//=============================================================================

const char* ts_SaveProfile::GetCustomLoadout( int ch )
{
	const char* pLoadoutString = "";

	if( ValidCustomChar(ch) )
		pLoadoutString = Characters[ch].Loadout;

	return pLoadoutString;
}

//=============================================================================
// ts_SaveProfile::ValidVariant
//=============================================================================

bool ts_SaveProfile::ValidVariant( int Variant )
{
	if( Variant == CPlayerProfile::COP || Variant == CPlayerProfile::GANG )
		return true;

	return false;
}

//=============================================================================
// ts_SaveProfile::ValidLevel
//=============================================================================

bool ts_SaveProfile::ValidLevel( int Level )
{
	if( Level < 0 || SG_MAX_LEVELS < Level )
		return false;

	return true;
}

//=============================================================================
// ts_SaveProfile::SetCompletedLevel
//
//		Set the completion status of the specified level
//=============================================================================

void ts_SaveProfile::SetCompletedLevel( int Level, bool Completed )
{
	if( !ValidLevel( Level ) )
		return;

	if( Completed )
		SinglePlayer.CompletedLevels[Level] = 1;
	else
		SinglePlayer.CompletedLevels[Level] = 0;

	return;
}


//=============================================================================
// ts_SaveProfile::GetCompletedLevel
//
//		Returns true if the specified level is complete, otherwise false.
//=============================================================================

bool ts_SaveProfile::GetCompletedLevel( int Level )
{
	if( !ValidLevel( Level ) )
		return false;

	if( SinglePlayer.CompletedLevels[Level] == 0 )
		return false;

	return true;
}

#ifdef DIRECTX_PC
//=============================================================================
// ts_SaveProfile::SetLevelCheckpoint
//
//		set savegame data for the checkpoint passed in on a given level
//		so play can resume from that checkpoint
//=============================================================================

void ts_SaveProfile::SetLevelCheckpoint( int level, int cpNum, ts_Checkpoint& checkpoint)
{
	ASSERT( ValidLevel( level ) || cpNum < SG_MAX_CHECKPOINTS );
	SinglePlayer.LevelCheckpoints[level][cpNum] = checkpoint;
	// set our checkpoint as valid
	SinglePlayer.LevelCheckpoints[level][cpNum].checkpointFlags |= VALID_CHECKPOINT;
}

//=============================================================================
// ts_SaveProfile::GetLevelCheckpoint
//
//		Returns the checkpoint reached by the player for the
//		given level based on the cpNum passed in
//=============================================================================

ts_Checkpoint& ts_SaveProfile::GetLevelCheckpoint( int level, int cpNum )
{
	ASSERT( ValidLevel( level ) || cpNum < SG_MAX_CHECKPOINTS );
	return SinglePlayer.LevelCheckpoints[level][cpNum];
}

//=============================================================================
// ts_SaveProfile::GetNumValidCheckpoints
//
//		Returns the farthest checkpoint reached for a given
//		level (passed in the level input parameter)
//=============================================================================
int ts_SaveProfile::GetNumValidCheckpoints( int level )
{
	ASSERT( ValidLevel( level ));
	for ( int c=SG_MAX_CHECKPOINTS-1; c > 0; --c )
	{
		if(SinglePlayer.LevelCheckpoints[level][c].checkpointFlags & VALID_CHECKPOINT)
			return c;
	}
	return 0;
}

// helpers for SetLevelSpecificCharacterInfo
void GiveInventoryItemToPlayer( char* key, int value )
{
	CInventoryListIF* iIF = CInventoryListIF::GetInterface( g_players.GetPlayer(0) );
	if( iIF )
	{
		iIF->AddInventory( key, value );
	}
}

void SendMessageToScriptActor( int actorID, char* action, ActorHandle sender )
{
		GetSingleton(CMessageDispatcher).SendMessageToActor( action, NULL, sender, actorID );
}
//=============================================================================
// ts_SaveProfile::SetLevelSpecificCharacterInfo
//
//		sets up level specific events and state based on the level
//	    and checkpoint passed in
//=============================================================================
void ts_SaveProfile::SetLevelSpecificCharacterInfo( int level, int cpNum )
{
	ASSERT( ValidLevel( level ) || cpNum < SG_MAX_CHECKPOINTS );
	// there is probably an easier way to do this, but. . .
	ts_Checkpoint tempCheckpoint = SinglePlayer.LevelCheckpoints[level][cpNum];
	switch(level)
	{
	case 0:
		if (tempCheckpoint.checkpoint >= 6)
			GiveInventoryItemToPlayer("doorkey1", 1);
		break;
	case 2:
		if (tempCheckpoint.checkpoint >= 2)
		{
			// open bank vault
			SendMessageToScriptActor( 107, "CloseSlaveDoor", CActor::ToHandle((CActor*)g_players.GetPlayer(0)) );
		}
		break;
	case 9:
		if (tempCheckpoint.checkpoint >= 2)
		{
			// give the door key
			GiveInventoryItemToPlayer("doorkey3", 1);
		}
	// within each level, we'll check the flags member of the levelCheckpoint
	// struct for the given checkpoint to see if there is anything we need to do
	// specific to this level.
	}

}
#endif

//=============================================================================
// ts_SaveProfile::SetCurrentLevel
//=============================================================================

void ts_SaveProfile::SetCurrentLevel( int Level )
{
	SinglePlayer.CurrentLevel = Level;
}

//=============================================================================
// ts_SaveProfile::SetPreviousLevel
//=============================================================================

void ts_SaveProfile::SetPreviousLevel( int Level )
{
	SinglePlayer.PreviousLevel = Level;
}

//=============================================================================
// ts_SaveProfile::GetCurrentLevel
//=============================================================================

int ts_SaveProfile::GetCurrentLevel( void )
{
	return SinglePlayer.CurrentLevel;
}

//=============================================================================
// ts_SaveProfile::GetPreviousLevel
//=============================================================================

int ts_SaveProfile::GetPreviousLevel( void )
{
	return SinglePlayer.PreviousLevel;
}

//=============================================================================
// ts_SaveProfile::GetLevelUnlocked
//=============================================================================

bool ts_SaveProfile::GetLevelUnlocked( int Level )
{
	if( GetCurrentLevel() == Level )
		return true;

	if( GetCompletedLevel( Level ) )
		return true;

	return false;
}

//=============================================================================
// ts_SaveProfile::SetSystemLinkMatchConfig
//=============================================================================

void ts_SaveProfile::SetSystemLinkMatchConfig( const char* pConfigString )
{
	int n = sizeof(SystemLinkPrefs.MatchConfig)/
			  sizeof(SystemLinkPrefs.MatchConfig[0]) - 1;

	if (ValidSingleton(ConfigMgr))
		strncpy( SystemLinkPrefs.MatchConfig, g_configmgr.GetOptions(), n );
	else
		SystemLinkPrefs.MatchConfig[0] = '\0';
	SystemLinkPrefs.MatchConfig[n] = 0;
}

//=============================================================================
// ts_SaveProfile::GetSystemLinkMatchConfig
//=============================================================================

const char* ts_SaveProfile::GetSystemLinkMatchConfig( void )
{
	return SystemLinkPrefs.MatchConfig;
}

//=============================================================================
// ts_SaveProfile::SetCustomChar
//=============================================================================

void ts_SaveProfile::SetCustomChar( int ch, const char* pName, const char* pInitString, int type )
{
	if( ValidCustomChar(ch) )
	{
		memset( &Characters[ch].InitString, 0, sizeof(Characters[ch].InitString) );
		memset( &Characters[ch].Name,       0, sizeof(Characters[ch].Name) );

		int n = sizeof(Characters[0].InitString)/
			     sizeof(Characters[0].InitString[0]) - 1;
		strncpy( Characters[ch].InitString, pInitString, n );
		Characters[ch].InitString[n] = 0;

		n = sizeof(Characters[0].Name) / sizeof(Characters[0].Name[0]) - 1;
		strncpy( Characters[ch].Name, pName, n );
		Characters[ch].Name[n] = 0;

		Characters[ch].CharType = type;
		Characters[ch].CheckSum = CustomPlayer::CreateCheckSum( &Characters[ch] );
	}

	return;
}

//=============================================================================
// ts_SaveProfile::GetCustomChar
//=============================================================================

ts_CustomProfile* ts_SaveProfile::GetCustomChar( int ch )
{
	ts_CustomProfile* pCustomProfile = 0;

	if( ValidCustomChar(ch) )
		 pCustomProfile = &Characters[ch];

	return pCustomProfile;
}

//=============================================================================
// ts_SaveProfile::SaveCustomChars
//
//		Calling this function will grab the custom character init strings from
//		the PlayerDB and stuff them into the save game structure.
//
//		Custom character ids are marked with a PLAYERDB_CUSTOM bit, so here we
//		loop through all the characters checking for this flag and saving off the
//		ones we find.
//=============================================================================

void ts_SaveProfile::SaveCustomChars( void )
{
	for ( int i = 0 ; i < CS_MAX_CHARACTERS ; i++ )
	{
		int dbIndex = PlayerDB::CharacterIndexFromId( PLAYERDB_CUSTOM | i );
		g_GlobalProfile.SetCustomChar( i, PlayerDB::CharacterDisplayName( dbIndex ), PlayerDB::CharacterInitString( dbIndex ),
			PlayerDB::CharacterType( dbIndex ) );
	}
}

//=============================================================================
// ts_SaveProfile::LoadCustomChars
//=============================================================================

void ts_SaveProfile::LoadCustomChars( void )
{
}

//=============================================================================
// ts_SaveProfile::ValidCustomChar
//=============================================================================

bool ts_SaveProfile::ValidCustomChar( int ch )
{
	if( ch >= 0 && ch < MaxCustomChar() )
		return true;

	return false;
}

//=============================================================================
// ts_SaveProfile::MaxCustomChar
//=============================================================================

int ts_SaveProfile::MaxCustomChar( void )
{
	return CS_MAX_CHARACTERS;
}

//=============================================================================
// ts_SaveProfile::SelectCharacter
//=============================================================================

void ts_SaveProfile::SelectCharacter( int ProfileTeam, int n )
{
	ASSERT( n >= 0 && n < PlayerDB::NumCharacters() );
	ASSERT( ProfileTeam == CPlayerProfile::COP || ProfileTeam == CPlayerProfile::GANG );
	
	SelectedCharacter[ProfileTeam] = n;

	if( ProfileTeam == CPlayerProfile::COP )
		g_playerProfile.SetCharacter( CPlayerProfile::COP, n );
	if( ProfileTeam == CPlayerProfile::GANG )
		g_playerProfile.SetCharacter( CPlayerProfile::GANG, n );

	return;
}

//=============================================================================
// ts_SaveProfile::GetSelectedCharacter
//
//		The PlayerDB has an array of prebuilt characters and then it has
//		a certain number of slots for customized characters.  This function will
//		the appropriate PlayerDB character id.
//=============================================================================

int ts_SaveProfile::GetSelectedCharacter( int ProfileTeam )
{
	ASSERT( ProfileTeam == CPlayerProfile::COP || ProfileTeam == CPlayerProfile::GANG );

	return SelectedCharacter[ProfileTeam];
}

//=============================================================================
// ts_SaveProfile::SetTagString
//=============================================================================

void ts_SaveProfile::SetTagString( const char* pText )
{
	memset( TagParams.Text, 0, sizeof(TagParams.Text) );

	int n = sizeof(TagParams.Text)/sizeof(TagParams.Text[0]) - 1;

	strncpy( TagParams.Text, pText, n );
}

//=============================================================================
// ts_SaveProfile::GetTagString
//=============================================================================

const char* ts_SaveProfile::GetTagString( void )
{
	return TagParams.Text;
}

//=============================================================================
// ts_SaveProfile::GetMaxTag
//=============================================================================

int ts_SaveProfile::GetMaxTag( void )
{
	return sizeof(TagParams.Text) / sizeof(TagParams.Text[0]) - 1;
}

//=============================================================================
// ts_SaveProfile::GetMaxTagColors
//=============================================================================

int ts_SaveProfile::GetMaxTagColors( void )
{
	return sizeof(TagParams.Color) / sizeof(TagParams.Color[0]);
}

//=============================================================================
// ts_SaveProfile::SetTagColor
//=============================================================================

void ts_SaveProfile::SetTagColor( int n, int Red, int Green, int Blue, int Alpha )
{
	if( n >= 0 && n < GetMaxTagColors() )
	{
		TagParams.Color[n].R = u8(Red);
		TagParams.Color[n].G = u8(Green);
		TagParams.Color[n].B = u8(Blue);
		TagParams.Color[n].A = u8(Alpha);
	}

	return;
}

//=============================================================================
// ts_SaveProfile::SetTagColor
//=============================================================================

void ts_SaveProfile::SetTagColor( int n, float Red, float Green, float Blue, float Alpha )
{
	int r = int(Red  *255.0f);
	int g = int(Green*255.0f);
	int b = int(Blue *255.0f);
	int a = int(Alpha*255.0f);

	SetTagColor( n, r, g, b, a );
}

//=============================================================================
// ts_SaveProfile::GetTagColor
//=============================================================================

void ts_SaveProfile::GetTagColor( int n, int &Red, int &Green, int &Blue, int &Alpha )
{
	if( n >= 0 && n < GetMaxTagColors() )
	{
		Red   = int(TagParams.Color[n].R);
		Green = int(TagParams.Color[n].G);
		Blue  = int(TagParams.Color[n].B);
		Alpha = int(TagParams.Color[n].A);
	}
	else
	{
		Red = 0;
		Green = 0;
		Blue = 0;
		Alpha = 0;
	}

	return;
}

//=============================================================================
// ts_SaveProfile::GetTagColor
//=============================================================================

void ts_SaveProfile::GetTagColor( int n, float &Red, float &Green, float &Blue, float &Alpha )
{
	int r, g, b, a;
	GetTagColor( n, r, g, b, a );

	Red   = float(r) / 255.0f;
	Green = float(g) / 255.0f;
	Blue  = float(b) / 255.0f;
	Alpha = float(a) / 255.0f;
}

//=============================================================================
// ts_SaveProfile::GetTagParams
//=============================================================================

void ts_SaveProfile::GetTagParams( ts_TagParams &TagParams )
{
	TagParams = this->TagParams;
}

//=============================================================================
// ts_SaveProfile::SetLookSensitivity
//=============================================================================

void ts_SaveProfile::SetLookSensitivity( int LookSensitivity )
{
	ControllerSettings.LookSensitivity = LookSensitivity;
}


//=============================================================================
// ts_SaveProfile::SetLookSpeed
//=============================================================================

void ts_SaveProfile::SetLookSpeed( int LookSpeed )
{
	ControllerSettings.LookSpeed = LookSpeed;
}

//=============================================================================
// ts_SaveProfile::SetInvertedAiming
//=============================================================================

void ts_SaveProfile::SetInvertedAiming( bool InvertedAiming )
{
	ControllerSettings.InvertedAiming = InvertedAiming;
}

//=============================================================================
// ts_SaveProfile::SetVibration
//=============================================================================

void ts_SaveProfile::SetVibration( bool VibrationEnabled )
{
	ControllerSettings.VibrationEnabled = VibrationEnabled;
}

//=============================================================================
// ts_SaveProfile::SetControllerSettings
//=============================================================================

void ts_SaveProfile::SetControllerSettings( bool Inverted, bool Vibration, int LookSensitivity, int LookSpeed )
{
	SetInvertedAiming( Inverted );
	SetVibration( Vibration );
	SetLookSensitivity( LookSensitivity  );
	SetLookSpeed(LookSpeed );
}

//=============================================================================
// ts_SaveProfile::SetButtonLayout
//=============================================================================

void ts_SaveProfile::SetButtonLayout( int ButtonLayout )
{
	ControllerSettings.ButtonLayout = ButtonLayout;
}

//=============================================================================
// ts_SaveProfile::SetThumbStickMode
//=============================================================================

void ts_SaveProfile::SetThumbStickMode( int ThumbStickMode )
{
	ControllerSettings.ThumbStickMode = ThumbStickMode;
}

//=============================================================================
// ts_SaveProfile::GetLookSensitivity
//=============================================================================

int ts_SaveProfile::GetLookSensitivity( void )
{
	return ControllerSettings.LookSensitivity;
}

//=============================================================================
// ts_SaveProfile::GetLookSpeed
//=============================================================================

int ts_SaveProfile::GetLookSpeed( void )
{
	return ControllerSettings.LookSpeed;
}

//=============================================================================
// ts_SaveProfile::GetInvertedAiming
//=============================================================================

bool ts_SaveProfile::GetInvertedAiming( void )
{
	return ControllerSettings.InvertedAiming;
}

//=============================================================================
// ts_SaveProfile::GetVibration
//=============================================================================

bool ts_SaveProfile::GetVibration( void )
{
	return ControllerSettings.VibrationEnabled;
}

//=============================================================================
// ts_SaveProfile::GetButtonLayout
//=============================================================================

int ts_SaveProfile::GetButtonLayout( void )
{
	return ControllerSettings.ButtonLayout;
}

//=============================================================================
// ts_SaveProfile::GetThumbStickMode
//=============================================================================

int ts_SaveProfile::GetThumbStickMode( void )
{
	return ControllerSettings.ThumbStickMode;
}

//=============================================================================
// ts_SaveProfile::SetDifficulty
//=============================================================================

void ts_SaveProfile::SetDifficulty( int Difficulty )
{
	GameSettings.Difficulty = Math::Clamp( Difficulty, DIFFICULTY_EASY, DIFFICULTY_ULTIMATE );
}

//=============================================================================
// ts_SaveProfile::GetDifficulty
//=============================================================================

int ts_SaveProfile::GetDifficulty( void )
{
	return GameSettings.Difficulty;
}

//=============================================================================
// ts_SaveProfile::SetStereoMode
//=============================================================================
#ifdef DIRECTX_PC
void ts_SaveProfile::SetStereoMode( int Enabled )
{
	GameSettings.Stereo = Enabled;
	if (g_ttl)
		g_ttl->SetStereoSound(GameSettings.Stereo);
}
#else
void ts_SaveProfile::SetStereoMode( bool Enabled )
{
	GameSettings.Stereo = Enabled;
}
#endif

//=============================================================================
// ts_SaveProfile::GetStereoMode
//=============================================================================

#ifdef DIRECTX_PC
int ts_SaveProfile::GetStereoMode( void )
#else
bool ts_SaveProfile::GetStereoMode( void )
#endif
{
	return GameSettings.Stereo;
}

#ifdef DIRECTX_PC
//=============================================================================
// ts_SaveProfile::SetBlurFX
//=============================================================================
void ts_SaveProfile::SetBlurFX( bool BlurFX )
{
	g_GlobalProfile.GameSettings.blurEffects = 
	RenderToTexture::isRenderingToTextureOn	 = BlurFX;
	
}

//=============================================================================
// ts_SaveProfile::GetBlurFX
//=============================================================================
bool ts_SaveProfile::GetBlurFX()
{
	return g_GlobalProfile.GameSettings.blurEffects;
}
#endif // DIRECTX_PC

//=============================================================================
// ts_SaveProfile::SetSoundVolume
//=============================================================================

void ts_SaveProfile::SetSoundVolume( float Volume )
{
	GameSettings.SoundVolume = Math::Clamp( Volume );
}

//=============================================================================
// ts_SaveProfile::SetMusicVolume
//=============================================================================

void ts_SaveProfile::SetMusicVolume( float Volume )
{
	GameSettings.MusicVolume = Math::Clamp( Volume );
}

//=============================================================================
// ts_SaveProfile::GetSoundVolume
//=============================================================================

float ts_SaveProfile::GetSoundVolume( void )
{
	return GameSettings.SoundVolume;
}

//=============================================================================
// ts_SaveProfile::GetMusicVolume
//=============================================================================

float ts_SaveProfile::GetMusicVolume( void )
{
	return GameSettings.MusicVolume;
}

//=============================================================================
// ts_SaveProfile::SetGammaValue
//=============================================================================

void ts_SaveProfile::SetGammaValue( float Value )
{
	GameSettings.Gamma = Math::Clamp( Value );
}

//=============================================================================
// ts_SaveProfile::GetGammaValue
//=============================================================================

float ts_SaveProfile::GetGammaValue( void )
{
	return GameSettings.Gamma;
}

//=============================================================================
// ts_SaveProfile::SetDefaultVoiceParams
//=============================================================================

void ts_SaveProfile::SetDefaultVoiceParams( void )
{
	VoiceParams.VoiceOutput    = VOICEOUTPUT_HEADSET;
	VoiceParams.OutputVolume   = 100;
}

//=============================================================================
// ts_SaveProfile::SaveGameLocks
//=============================================================================

void ts_SaveProfile::SaveGameLocks( void )
{
	int numOutput = 0;
	ASSERT( GameLocks::NumLocks() <= GAMELOCKS_MAX_LOCKS );
	memset( &Locks, 0, sizeof(Locks) );
	for ( int i = 0 ; i < GameLocks::NumLocks() && i < GAMELOCKS_MAX_LOCKS ; i++ )
	{
		GameLocks::Lock* lock = GameLocks::GetLock( i );
		if ( lock->unlocked )
		{
			strcpy( Locks[ numOutput ].key, lock->key );
			Locks[ numOutput ].unlocked = true;
			numOutput++;
		}
	}
}

//=============================================================================
// ts_SaveProfile::LoadGameLocks
//=============================================================================

void ts_SaveProfile::LoadGameLocks( void )
{
	int i;
	ASSERT( GameLocks::NumLocks() <= GAMELOCKS_MAX_LOCKS );

	// reset all the locks
	GameLocks::LockAll();

	// now unlock what this profile has unlocked
	for (i = 0; i < GAMELOCKS_MAX_LOCKS; i++)
		if (strlen(Locks[i].key) > 0 && Locks[i].unlocked)
			GameLocks::Unlock(Locks[i].key);
}


//=============================================================================
// ts_SaveProfile::SetBlood
//=============================================================================

void ts_SaveProfile::SetBlood( bool Enabled )
{
	GameSettings.Blood = Enabled;
}

//=============================================================================
// ts_SaveProfile::GetBlood
//=============================================================================

bool ts_SaveProfile::GetBlood( void )
{
	return GameSettings.Blood;
}
