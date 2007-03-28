/***************************************************************************/
// The main game setup stuff
/***************************************************************************/

#include "TTLPCH.h"

// My header
#include "main/TTL.h"

#include "fmv/fmv.h"
#include "environ/LevelScript.h"
#include "Layers/hog.h"
#include "Tupperware/memorymanager.h"	// for USE_TUP_MEMORY_MANAGER define
//#include "main/cardcheck.h"
#include "EngineHelper/sfx.h"
#include "Game/Database/DataMgr.h"
#include "GameHelper/FileLoader.h"

#include "PlayerDB.h"
#include "fluff/SaveData.h"
#include "Layers/MemoryConfig.h"
#include "platform/MemoryCard/MemoryCard.h"
#include "frontend/frontend.h"
#include "frontend/customplayer.h"
#include "frontend/gamesettings.h"
#include "frontend/savegame.h"
#include "Layers/DiscErrorHandler/DiscError.h"
#include "environ/ConfigMgr.h"
#include "main/Lobby.h"
#include "EngineHelper/NameLibrary.h"
#include "Components/weapons/weaplib.h"
#include "GameHelper/Translator.h"
#include "main/NetMonitor.h"
#include "EngineHelper/LoadScreen.h"
#include "EngineHelper/lsspin.h"
#include "Layers/MemPool.h"
#include "display/textureutil.h"
#include "main/lstag.h"
#include "effects/scenesetup.h"
#include "components/say.h"
#include "hud/ipanel/isys.h"

#if defined( PS2 )
#include "platform/PS2/CDllLoader.h"
#endif

#if defined( _XBOX )
#include "Layers/FileCache/cache_xbox.h"
#include "platform/Xbox/XboxLive.h"
#endif

#ifdef NETZ
#include "Network/NetZ/NetZMessage.h"
#endif

#include "Platform/CKeyboard.h"

#ifdef PS2
#include "Network/Connection.h"
#include "layers/mstream/mstream.h"
#endif

extern void Game_RegisterScriptFuncs();
extern void EngineHelper_RegisterScriptFuncs();
extern void GameHelper_RegisterScriptFuncs();
extern void Stage_RegisterScriptFuncs();
extern void Math_RegisterScriptFuncs();
extern void Interfaces_RegisterScriptFuncs();
extern void TTL_RegisterScriptFuncs();
extern void Navigate_RegisterScriptFunc();
extern void Components_RegisterScriptFuncs();
extern void Script_RegisterScriptFuncs();

extern const char *TTL_VERSION;

#define PATCHEXT_NAME "PatchExt.rel"

//set true for DVD, false for CD
#define IS_DVD_BUILD true

#define PS2_FORCE_PAL_UKENGLISH

// Want pal?
extern bool g_wantPAL;

extern void AllocOurMem();

// This registers us as the game
TTL thisGame;
Game *GameApp::game = &thisGame;
TTL *g_ttl = NULL;

// Is there an invite?
bool g_acceptingInvitation = false;

float g_volumeOverride = 1.0f;

// Hog support
// BEWARE!!! PS2 menu.hog is encrypted and has all game data; IT IS A RED HERRING OF A NAME
const char *TTL::s_hogName[] = { "main.hog", "menu.hog" };
int TTL::s_hogs = sizeof(TTL::s_hogName) / sizeof(const char *);

// Directories to cache on XBOX
#if defined( _XBOX )
char*	TTLCacheDirList[] = {
	{"Z:\\DATA_XBOX"},
	{"Z:\\DATA_XBOX\\SOUND"},
	0
};

CXboxLiveSoundNameMap  g_TTLSoundNameMap[] =
{
	{ "PressA",			"menu_select" },
	{ "PressB",			"screen_back" },
	{ "Error",			"popup_show" },
	{ "MenuItemSelect", "menu_down" },
	{ "Popup",			"menu_invalid" },
	{ "PasscodeEntry",	"menu_down" },
	{ "Notification",	"popup_show" }
};

#endif

/***************************************************************************/
// This is the memory configuration structure, it's referenced by the engine
// to initialize memory.  This is necessary because memory needs to be
// initialized before main() is called.
/***************************************************************************/

static CMemPool s_poolConfig[] =
{
	POOLDEF(  16, 5116 ),
	POOLDEF(  32, 8142 ),
	POOLDEF(  48, 4177 ),
	POOLDEF(  64, 1175 ),
	POOLDEF(  80, 1021 ),
	POOLDEF(  96,  452 ),
	POOLDEF( 112,  747 ),
	POOLDEF( 128,  734 ),
	POOLDEF( 144,   73 ),
	POOLDEF( 160, 1478 ),
	POOLDEF( 176,  263 ),
	POOLDEF( 192,   87 ),
	POOLDEF( 208,   29 ),
	POOLDEF( 224,  211 ),
	POOLDEF( 240,   64 ),
	POOLDEF( 256,   88 ),
	POOLDEF(   0,    0 )
	//[[ 15 ]] TOTAL: 1321952 (waste       0, 0%) (rounded waste  112730, 9%)
};

// This function is called by the memory manager at init time to
// find out how to initialize.  This is called before main().
void GetApplicationMemoryConfig( ts_MemoryConfig* config )
{
	#if defined( PS2 )

		config->Size          = 64 * 1024 * 1024;	// clamp any only allow first 64MB
		config->OSReserve		 = ( 150 * 1024 );	// for Sony heap (needed for network stack)
//		config->Secondary		 = ( 50 * 1024 );
		config->Handles       = 18500;

	#elif defined( _XBOX )

		config->Size			 = 0;	// get max our manager will allow
		config->OSReserve	    = ( 32 * 1024 * 1024 );	// save for DXT textures
//		config->Secondary		 = ( 50 * 1024 );
		config->Handles       = 23000;

	#else

		config->Size          = 0; // get max our manager will allow
		config->OSReserve     = ( 16 * 1024 );		// leave something, just in case
//		config->Secondary		 = ( 50 * 1024 );
		config->Handles       = 23000;

	#endif

	config->MainRamPools  = s_poolConfig;
	config->AudioRamPools = NULL;
}

/***************************************************************************/
// A function that gets a timestamp.
/***************************************************************************/
bool g_sessionTimestamp = true;
int32 GetTimestamp()
{
	if (g_sessionTimestamp && ValidSingleton(CSession))
		return(g_session.GetSessionTimeMS());
	return(g_timer.GetOSTime());
}

/***************************************************************************/
// The game itself must implement this as a static function
/***************************************************************************/
bool GameApp::IsDVD()
{
	return IS_DVD_BUILD;
}

/***************************************************************************/
/***************************************************************************/
void SetDefaultDictionary()
{
	const char* commandLine = GameApp::Application()->CommandLine();
	if ( commandLine && strstr( commandLine, "-lang=" ) != 0 )
	{
		g_translator.SetLanguage( commandLine + 6 );
	}
	else
	{
		#if defined(PS2)

			if ( g_regionUSA ) // no foreign language if we're not on a pal build...
			{
				g_translator.SetLanguage( "english" );	CKeyboard::setLanguage( CKeyboard::KEYBOARD_ENGLISH );
			}
			else
			{
				switch ( sceScfGetLanguage() )
				{
					case SCE_FRENCH_LANGUAGE:		g_translator.SetLanguage( "french"  );	CKeyboard::setLanguage( CKeyboard::KEYBOARD_FRENCH );  break;
					case SCE_SPANISH_LANGUAGE:		g_translator.SetLanguage( "spanish" );	CKeyboard::setLanguage( CKeyboard::KEYBOARD_SPANISH ); break;
					case SCE_ITALIAN_LANGUAGE:		g_translator.SetLanguage( "italian" );	CKeyboard::setLanguage( CKeyboard::KEYBOARD_ITALIAN ); break;
					default:								g_translator.SetLanguage( "uk_english" );	CKeyboard::setLanguage( CKeyboard::KEYBOARD_ENGLISH ); break;
				}

#if defined(PS2_FORCE_PAL_UKENGLISH)
				g_translator.SetLanguage( "uk_english" );
#endif //defined(PS2_FORCE_PAL_UKENGLISH)
			}

		#elif defined (_XBOX)

			switch (XGetLanguage())
			{
				case	XC_LANGUAGE_FRENCH:		g_translator.SetLanguage( "french" );	CKeyboard::setLanguage( CKeyboard::KEYBOARD_FRENCH );  break;
				case	XC_LANGUAGE_ITALIAN:		g_translator.SetLanguage( "italian" );	CKeyboard::setLanguage( CKeyboard::KEYBOARD_ITALIAN ); break;
				default:								g_translator.SetLanguage( "english" );	CKeyboard::setLanguage( CKeyboard::KEYBOARD_ENGLISH ); break;
			}

		#else

			g_translator.SetLanguage( "english" );
			CKeyboard::setLanguage( CKeyboard::KEYBOARD_ENGLISH );

		#endif
	}
}

/***************************************************************************/
// So the fmv paths are as follows:
// %1 is where the in-game movies come from
// %2 is where the pal/ntsc selction is made
// $3 is where the legal screen comes from
//
// on ps2 it should be %1 = ntsc/pal (audio is multilayered)
//							  %2 = ntsc/pal
//							  %3 = <lang>
/***************************************************************************/
void SetupFMVPaths()
{
#if defined(PS2)
	bool inPal = g_wantPAL;

#if defined(PS2_FORCE_PAL_UKENGLISH)
	FMVPlayer::SetSoundTrack(FMVST_DEFAULT);
	FMVPlayer::SetPathKey(3, g_regionUSA ? "usa" : "uk" );
#else //PS2_FORCE_PAL_UKENGLISH
	switch (sceScfGetLanguage())
	{
		case SCE_FRENCH_LANGUAGE:
			FMVPlayer::SetSoundTrack(FMVST_FRENCH);
			FMVPlayer::SetPathKey(3, "french");
			break;
		case SCE_SPANISH_LANGUAGE:
			FMVPlayer::SetSoundTrack(FMVST_SPANISH);
			FMVPlayer::SetPathKey(3, "spanish");
			break;
		case SCE_ITALIAN_LANGUAGE:
			FMVPlayer::SetSoundTrack(FMVST_ITALIAN);
			FMVPlayer::SetPathKey(3, "italian");
			break;
		default:
			FMVPlayer::SetSoundTrack(FMVST_DEFAULT);
			FMVPlayer::SetPathKey(3, g_regionUSA ? "usa" : "uk" );
			break;
	}
#endif //PS2_FORCE_PAL_UKENGLISH

	FMVPlayer::SetPathKey(1, inPal ? "pal" : "ntsc");
#elif defined (_XBOX)
	bool inPal = (XGetVideoStandard() == XC_VIDEO_STANDARD_PAL_I);
	switch (XGetLanguage())
	{
		case	XC_LANGUAGE_FRENCH:
			FMVPlayer::SetPathKey(1, "french");
			break;
		case	XC_LANGUAGE_ITALIAN:
			FMVPlayer::SetPathKey(1, "italian");
			break;
		default:
			FMVPlayer::SetPathKey(1, inPal ? "uk" : "usa" );
			break;
	}
#else
	bool inPal = false;
#endif
#ifdef DIRECTX_PC
	FMVPlayer::SetPathKey(2, "\0");
#else
	FMVPlayer::SetPathKey(2, inPal ? "pal" : "ntsc");
#endif
}

/***************************************************************************/
/***************************************************************************/
int g_playAllFmvs = false;

void PlayStartupFMVS()
{
	g_audio.SetVolume(Audio::SFX, 1.0f);

	// Do the FMV's
	FMVPlayer fmvPlayer;

	// Prepare to play
	fmvPlayer.Begin();

	// DEBUG
	if ( g_playAllFmvs )
	{
		bool aborted = false;
		fmvPlayer.Queue( "%3/legal", true );   fmvPlayer.Play( aborted );
		fmvPlayer.Queue( "%2/attract", true ); fmvPlayer.Play( aborted );
		fmvPlayer.Queue( "%2/begin", true );   fmvPlayer.Play( aborted );
		fmvPlayer.Queue( "%2/credits", true ); fmvPlayer.Play( aborted );
		fmvPlayer.Queue( "%2/eidos", true );   fmvPlayer.Play( aborted );
		fmvPlayer.Queue( "%2/end", true );     fmvPlayer.Play( aborted );
		fmvPlayer.Queue( "%2/endcred", true ); fmvPlayer.Play( aborted );
		fmvPlayer.Queue( "%2/intro", true );   fmvPlayer.Play( aborted );
		fmvPlayer.Queue( "%2/scene_a", true ); fmvPlayer.Play( aborted );
		fmvPlayer.Queue( "%2/scene_b", true ); fmvPlayer.Play( aborted );
		fmvPlayer.Queue( "%2/scene_c", true ); fmvPlayer.Play( aborted );
		fmvPlayer.Queue( "%2/scene_d", true ); fmvPlayer.Play( aborted );
		fmvPlayer.Queue( "%2/scene_e", true ); fmvPlayer.Play( aborted );
		fmvPlayer.Queue( "%2/scene_f", true ); fmvPlayer.Play( aborted );
		fmvPlayer.Queue( "%2/scene_g", true ); fmvPlayer.Play( aborted );
		fmvPlayer.Queue( "%2/scene_h", true ); fmvPlayer.Play( aborted );
	}

	// Legal you can't skip
	bool aborted = false;
#ifdef PS2
	fmvPlayer.Queue("%3/legal", false);
#else
	fmvPlayer.Queue("%1/legal", false);
#endif
	fmvPlayer.Play(aborted);

	// Do all the credits
	fmvPlayer.Queue("%2/eidos", true);
	fmvPlayer.Play(aborted);
	if (!aborted)
	{
		fmvPlayer.Queue("%2/credits", true);
		fmvPlayer.Play(aborted);
	}

	// Now do the intro
	fmvPlayer.Queue("%2/intro", true);
	fmvPlayer.Play(aborted);

	// Cleanup
	fmvPlayer.End();

	g_audio.SetVolume(Audio::SFX, 0.0f);
}

#ifdef PS2
/***************************************************************************/
// Blocking load servicer
/***************************************************************************/
void TTLBlockingLoadHandler(void *context)
{
	static u32 lastNetUpdate = 0;
	if (ValidSingleton(CConnection) && (g_timer.GetOSTime() - lastNetUpdate) > 50)
	{
	 	g_connection.Service();
	 	lastNetUpdate = g_timer.GetOSTime();
	}
}
#endif

/***************************************************************************/
// Construct the main game
/***************************************************************************/
TTL::TTL(void)
{
	g_ttl = this;

	soundVolume = 0.0f;
	musicVolume = 0.0f;

#ifdef DIRECTX_PC
	stereoSound = STEREOMODE_STEREO;
#else
	stereoSound = true;
#endif

	// set up version info
	InitVersion();

	d_requestExit = false;

	d_loop[SETUP_LOOP]        = &d_setupLoop;
	d_loop[SINGLEPLAYER_LOOP] = &d_standAloneLoop;
#if defined(NETWORKING)
	d_loop[MULTIPLAYER_LOOP]  = &d_multiPlayerLoop;
#endif
}

/***************************************************************************/
// We are going away, use terminate for most stuff
/***************************************************************************/
TTL::~TTL(void)
{
}

/***************************************************************************/
// Initialize all systems that are common to the fluff & game sim
/***************************************************************************/
bool TTL::Initialize(void)
{
	// Init the graphics system
	if (!GraphicsInit())
		return(false);

	// Make sure our title is set
	Media::SetTitle( "TTL" );

#ifdef _XBOX
	// Copy UIX resources to scratch disk for faster loading.
	CopyFile( "d:\\data_xbox\\ttl.uix", "z:\\ttl.uix", FALSE );
#endif

	// Make sure the screen is setup right, also check for widescreen
#if !defined (_XBOX)
	Sprite::SimulateScreenResolution(640.0f, 480.0f);
#else
	if ((DirectX_PresentParams.Flags & D3DPRESENTFLAG_WIDESCREEN) == 0)
		Sprite::SimulateScreenResolution(640.0f, 480.0f);
	else
	{
		// You are mine!
		const float simulateWidth = 480.0f * 16.0f / 9.0f;;
 		Sprite::SimulateScreenResolution(simulateWidth, 480.0f);
 		Sprite::SetViewPortOrigin((640.0f - simulateWidth) * 0.5f, 0.0f);
		if (DirectX_PresentParams.BackBufferWidth == 640)
			InstrumentSystem::SetWidescreenOffset(108.0f);
		else
			InstrumentSystem::SetWidescreenOffset(96.0f);
		InstrumentSystem::SetOverdrawPercentage(10.0f, 10.0f);
	}
#endif

#if defined (_XBOX)
	CFileCache::CreateDirectories(TTLCacheDirList);
	//CFileCache::RegisterUpdate(&LoadingScreen_Render);
#endif

	// Mount the hogs
	for (int hogVolume = 0; hogVolume < s_hogs; hogVolume++)
	{
		bool ret = Media::MountHog(s_hogName[hogVolume]);
#if defined (CDROM)
		if (!ret)
			DiscErrorHandler::DisplayError((char *)s_hogName[hogVolume], DiscErrorHandler::OPEN_FILE_FAILED);
#endif
	}

	// The setup for how we look for local files
#if defined(CDROM) && !defined(_XBOX) && !defined(DIRECTX_PC)
	Hog_SetCheckLocal(CHECKLOCAL_NEVER);
#elif defined(CDROM) && defined(_XBOX)
	Hog_SetCheckLocal(CHECKLOCAL_NEVER);
#else
	Hog_SetCheckLocal(CHECKLOCAL_BEFORE);
#endif

	// set the output debug buffer
#ifdef DEBUG_OUTPUT_ENABLED
	d_debugOutput.SetBuffer(&d_debugBuffer);
	d_debugOutput.SetTimestampFunction(&GetTimestamp);
	SetSingleton(CDebugOutput, &d_debugOutput);
#endif
	g_debugOutput.RegisterCategory("Lag time", "LAG");
	g_debugOutput.RegisterCategory("Loop states", "LOOP");

#if defined(NETWORKING)
	// load patch extractor DLL
	#if defined( PS2 )
	bool success = g_DllLoader.Load( PATCHEXT_NAME, false );
	ASSERTS( success, "FATAL: unable to load " PATCHEXT_NAME );
	#endif //PS2
#endif
  	// initialize the gamma table
  	TextureUtil::SetGammaZeroToOne( PROFILE_DEFAULT_GAMMA );

	// (Pre-)process the command line.
	GameApp* app = GameApp::Application();

	// set the message dispatcher singleton
#if defined(NETZ)
	CNetZMessageDispatcher* netZDispatcher = new CNetZMessageDispatcher;
	SetSingleton(CMessageDispatcher, (CMessageDispatcher*)netZDispatcher);
#else
	NewSingleton(CMessageDispatcher);
#endif

	g_gameEnvironment.Initialize();
	g_gameEnvironment.Clear();

#ifdef DIRECTX_PC
	//// parse our command line for a cd key
	const char* commandLine = GameApp::Application()->CommandLine();
	//if ( commandLine && strstr( commandLine, "-cdkey=" ) != 0 )
	//{
	//	strncpy(g_gameEnvironment.GetCdKeyPtr(), strstr( commandLine, "-cdkey=" )+7, 19);
	//}

	// and then for eax support
	if ( commandLine && strstr( commandLine, "-useEAX" ) !=0 )
	{
		EAX::usingEAX = true;
	}
#endif

	// Make sure we don't go too far on the clip planes
#ifdef _DEBUG
	//SceneSetup::SetMaxClipFar(350.0f);
#endif

	//finish any unfinished platform initialziations
#ifdef DIRECTX_PC
	if(!app->FinishInitialization())
	{
		return false;
	}
#else
	app->FinishInitialization();
#endif
	// Setup the FMV craizapola
	SetupFMVPaths();

	// Play the intros
#ifdef DIRECTX_PC
	// Start COM for DirectShow
	CoInitialize(NULL);
#endif

	// Do the FMV's
	PlayStartupFMVS();

	// Setup for hud.
	Font::LoadSystemFont("gazzstencil");
	Font::LoadFont("GazzRegularB", true);
	Font *font = Font::FindFont("GazzRegularB");
	t_Handle handle = dblLoad("shell/gadgets.dbl");
	dblSetPermanent(handle, true);
	TextureDatabase *s_textureDB = NULL;
	s_textureDB = dblGetTextures( handle , DEFAULT_CHUNK_KEY_STRING );
	Texture *texture;
	int i;
	for( i = 0 ; i<12 ; i++ )
	{
		texture = s_textureDB->GetTexture( i );
		font->SetCustomChar( 15 + i , handle , i , 0 , 0 , texture->GetImage()->GetWidth() , texture->GetImage()->GetHeight() );
	}

	// Try the loadscreen
	g_random.Seed(g_timer.GetOSTime());
#if 0
	LoadScreenSpin::SetModel("objects/ttl_logo.dbl");
	LoadScreen::Setup(LoadScreenSpin::Create);
#else
	LoadScreen::Setup(LoadScreenTag::Create);
#endif
	LoadScreen::Kickoff();

	// load the dictionary
	SetDefaultDictionary();
	g_translator.LoadDictionary();

	// Make sure the audio streams are mounted
	g_audio.MountStreamLibrary();
	g_audio.jukebox.LoadTrackInfoDB("trackdb.txt");

	// register the c-code routines that scripts use
	Game_RegisterScriptFuncs();
	EngineHelper_RegisterScriptFuncs();
	GameHelper_RegisterScriptFuncs();
	Stage_RegisterScriptFuncs();
	Math_RegisterScriptFuncs();
	Interfaces_RegisterScriptFuncs();
	Script_RegisterScriptFuncs();
	TTL_RegisterScriptFuncs();
	Navigate_RegisterScriptFunc();
	Components_RegisterScriptFuncs();

	/* load permanent data files */

	memSetUseBit(MEM_PRELOAD_DATA);
	PreloadPermanentData();
	memClearUseBit(MEM_PRELOAD_DATA);

	// I don't know if this should be here, but oddly this is not initialized to
	// something very useful
	g_GlobalProfile.ResetToDefaults();

	/* init the memory card manager - note: must be after preload permanent data */
	/* contains the .ico file */

#if defined( PS2 )
	g_MemCardMgr.Initialize( "logodat.ico", SAVEGAME_TITLEID , SAVE_DATA_VERSION );
#elif defined( WIN32 )
	g_MemCardMgr.Initialize( NULL, "Bigjuj0_2003.hdr", SAVE_DATA_VERSION );
#endif

	// Init TTL's memory card thread
	int	NumCardsToPoll = 1;
	g_MemCardManager->Init( g_MemCardManager->CMEMORYCARD_UNCOMPRESSED, 0 ,g_MemCardManager->CMEMORYCARD_ENCRYPTED,MC_ENCRYPT_HELIX,
									16, 0, "25 To Life", (char*)SAVEGAME_TITLEID, NumCardsToPoll, 0, PlayerDB::GetVersionID(), SAVEGAME_VER );

	SetSoundVolume( g_GlobalProfile.GetSoundVolume() );
	SetMusicVolume( g_GlobalProfile.GetMusicVolume() );
	SetStereoSound( g_GlobalProfile.GetStereoMode() );

	if( ValidSingleton(Blood) )
		g_blood.Enable( g_GlobalProfile.GetBlood() );

	for ( uint j =0; j<g_input.Controllers(); j++ )
	{
		if ( g_input.GetUnlockedController(j)->Connected() )
			g_input.GetUnlockedController(j)->Vibration( true );
	}

	// Set the session spec singleton.
	SetSingleton(CSessionSpec, (CSessionSpec*)&d_sessionSpec);

	for (int i = 0; i < NUM_LOOPS; i++)
		d_loop[i]->SetEnvironment(&g_gameEnvironment);

	d_state = SETUP_LOOP;

#if defined( WIN32 ) && !defined( _XBOX)
	gfxSetWindowText("25 To Life");
#endif

	// make a game mode configuration manager
	NewSingleton( ConfigMgr );

	// init save game struct
	CSaveGame::Init();
#if defined(NETWORKING)
	// --- network error handling
	g_CNetMonitor.initialize();
#endif
	#if defined (_XBOX)
	if (CXboxLive::GetGameInvite() == true)
	{
		// we need to go straight to multiplayer..
	}

	// Try to silent login to Xbox Live...
	CXboxLive::Init();
	CXboxLive::SetSoundMap(g_TTLSoundNameMap, G_TTL_SOUNDMAP_ENTRIES);
	CXboxLive::InitUIX();
	#endif

	// Should this be here? I dunno
	NewSingleton(SayVoiceSetLibrary);
	g_sayLib.Initialize();

	// Done loading!
	//LoadScreen::Kill();

	// alloc netz memory
#ifdef NETZ
	AllocOurMem();
	CNetZConnection::loadNetworkDLLs();
	CNetZConnection::LoadNetworkConfigSettings();
#endif

#ifdef PS2
	// If we ever block on a load (which should be never) allow
	// the network to still process
	MultiStream::RegisterBlockingCallback(TTLBlockingLoadHandler);
#endif

	return(true);
}

/***************************************************************************/
// Run for a frame
/***************************************************************************/
bool TTL::Run(void)
{
	static	bool	SilentLoginDone = false;

	#if defined (_XBOX)
	// update Live Silent Sign in
	if (SilentLoginDone == false)
	{
		if (CXboxLive::AreWeLoggedIn() == true)
		{
			SilentLoginDone = true;
//pab test			CXboxLive::StartFriendsList();
		}
	}
	#endif
#if defined(NETWORKING)
	// --- network error handling
	g_CNetMonitor.update();
#endif

	// update memory card thread
	g_MemCardManager->Update();

	// Update the lobby, if any.
	if (ValidSingleton(CLobby))
		g_lobby.Update();

	if (!d_loop[d_state]->RunLoop())
	{
		if (d_requestExit)
			return(false);

		ResetVolume();

		switch(d_state)
		{
		 case SETUP_LOOP:
			switch(d_setupLoop.GetLoopType())
			{
			 case CSetupLoop::SINGLEPLAYER_LOOP:
				d_state = SINGLEPLAYER_LOOP;
				break;
			 case CSetupLoop::SYSTEMLINK_LOOP:
			 case CSetupLoop::ONLINE_LOOP:
				d_state = MULTIPLAYER_LOOP;
				break;
			 case CSetupLoop::EXIT_LOOP:
				return(false);
			 default:
				ASSERT(false);
				return(false);
			}
			break;

		 case SINGLEPLAYER_LOOP:
			if (!d_standAloneLoop.SwitchingLevels() || g_acceptingInvitation)
				d_state = SETUP_LOOP;
			break;

		 case MULTIPLAYER_LOOP:
			d_state = SETUP_LOOP;
			break;

		 default:
			ASSERT(false);
			break;
		}
	}
#ifdef DIRECTX_PC
	if ( GameApp::Application()->setSoundLevels )
	{
				g_ttl->SetSoundVolume( Math::Clamp(g_GlobalProfile.GetSoundVolume() ), false );
				g_ttl->SetMusicVolume( Math::Clamp(g_GlobalProfile.GetMusicVolume() ), false );
	}
#endif //DIRECTX_PC
	return(true);
}

/***************************************************************************/
// Terminate the game
/***************************************************************************/
bool TTL::Terminate(void)
{
#if defined(NETWORKING)
	// --- network error handling
	g_CNetMonitor.terminate();
#endif
	// Unmount the hogs
	for (int hogVolume = 0; hogVolume < s_hogs; hogVolume++)
		Media::UnmountHog(s_hogName[hogVolume]);

	// clean up permanent FileStream pre-loads
	FileStream::CleanupPreLoad(true);

	// clean up the message dispatcher
	DeleteSingleton(CMessageDispatcher);

	// clean up the configuration manager
	DeleteSingleton( ConfigMgr );

	// clean up the session spec.
	ClearSingleton(CSessionSpec);

	// delete our render to texture surface
#ifdef DIRECTX_PC
	RenderToTexture::DeleteRenderTexture();
	// shut down EAX
	//(it won't hurt us if we shut down w/o
	// ever having initialized)
	EAX::Shutdown();
	// looks like on the pc we need to explicitly shut down
	// our memory card thread.
	g_MemCardManager->Exit();
#endif
	return(true);
}

/***************************************************************************/
/***************************************************************************/
void TTL::Exit(void)
{
	d_loop[d_state]->RequestExit();
	d_requestExit = true;
}

/***************************************************************************/
/***************************************************************************/
bool TTL::GraphicsInit(void)
{
	// setup some defaults
	float clipNear = 0.05f;
	float clipFar = 500.0f;
	ts_bRGBA ClearColor = { 0, 0, 0, 0 };
	Display__SetNearClip(clipNear);
	Display__SetFarClip(clipFar);
	Display__SetClearColor(ClearColor);
	Display__SetFog(TRUE);
	Display__SetNearFog(clipFar * 0.4f);
	Display__SetFarFog(clipFar * 1.0f);
	Display__SetFogMode(RS_FOGMODE_LINEAR);
	Display__SetFogDensity(0.8f);
	Display__SetFogColor(ClearColor);

	// sprite manager needs clip planes, too
	SpriteMgr__SetNearClip(clipNear);
	SpriteMgr__SetFarClip(clipFar);

	// We always want to use locked fading
	RenderMgr__FadeBlackLock(true);

#ifdef DIRECTX_PC // set up our render to texture texture here for now
	RenderToTexture::CreateRenderTexture(DisplayState.nScreenWidth, DisplayState.nScreenHeight, DisplayState.backBufferFormat );
#endif

	return(true);
}

/***************************************************************************/
/***************************************************************************/
void TTL::ResetVolume(void)
{
	g_audio.SetVolume(Audio::SFX, soundVolume * g_volumeOverride, Audio::smoothVolumeInstant);
	g_audio.SetVolume(Audio::DIALOG, soundVolume * g_volumeOverride * DIALOG_VOLUME_ADJUST, Audio::smoothVolumeInstant);
	g_audio.SetVolume(Audio::MUSIC, musicVolume * g_volumeOverride, Audio::smoothVolumeInstant);
}

/***************************************************************************/
/***************************************************************************/
void TTL::SetSoundVolume(float volume, bool smooth)
{
	const float t = smooth ? Audio::smoothVolumeSlow : Audio::smoothVolumeInstant;
	SetSoundVolume(volume, t);
}

/***************************************************************************/
/***************************************************************************/
void TTL::SetSoundVolume(float volume, float fadeTime)
{
	soundVolume = Math::Clamp(volume) * g_volumeOverride;
	g_audio.SetVolume(Audio::SFX, soundVolume, fadeTime);
	g_audio.SetVolume(Audio::DIALOG, soundVolume, fadeTime);
}

/***************************************************************************/
/***************************************************************************/
void TTL::SetMusicVolume(float volume, bool smooth)
{
	const float t = smooth ? Audio::smoothVolumeVerySlow : Audio::smoothVolumeInstant;
	SetMusicVolume(volume * g_volumeOverride, t);
}

/***************************************************************************/
/***************************************************************************/
void TTL::SetMusicVolume(float volume, float fadeTime)
{
	musicVolume = Math::Clamp(volume);
	g_audio.SetVolume(Audio::MUSIC, musicVolume, fadeTime);
}

/***************************************************************************/
/***************************************************************************/
void TTL::VolumeMute()
{
	g_audio.SetVolume(Audio::MUSIC, 0.0f, 0.0f);
	g_audio.SetVolume(Audio::SFX, 0.0f, 0.0f);
	g_audio.SetVolume(Audio::DIALOG, 0.0f, 0.0f);
}

/***************************************************************************/
/***************************************************************************/
void TTL::VolumeFadeIn(float fadeTime)
{
	g_audio.SetVolume(Audio::MUSIC, musicVolume, fadeTime);
	g_audio.SetVolume(Audio::SFX, soundVolume, fadeTime);
	g_audio.SetVolume(Audio::DIALOG, soundVolume, fadeTime);
}

/***************************************************************************/
/***************************************************************************/
void TTL::VolumeFadeOut(float fadeTime)
{
	g_audio.SetVolume(Audio::MUSIC, 0.0f, fadeTime);
	g_audio.SetVolume(Audio::SFX, 0.0f, fadeTime);
	g_audio.SetVolume(Audio::DIALOG, 0.0f, fadeTime);
}

/***************************************************************************/
/***************************************************************************/
#ifdef DIRECTX_PC
void TTL::SetStereoSound(int stereo)
{
	stereoSound = stereo;
	if ( stereo == STEREOMODE_MONO )
		g_audio.SetOutputMode( SoundDriver::OUTPUTMODE_MONO );
	else if ( stereo == STEREOMODE_STEREO )
		g_audio.SetOutputMode( SoundDriver::OUTPUTMODE_STEREO );
	else
		g_audio.SetOutputMode( SoundDriver::OUTPUTMODE_STEREOFLIPPED );
}
#else
void TTL::SetStereoSound(bool stereo)
{
	stereoSound = stereo;
	g_audio.SetOutputMode(stereo ?
		SoundDriver::OUTPUTMODE_STEREO : SoundDriver::OUTPUTMODE_STEREO);
}
#endif

/***************************************************************************/
/***************************************************************************/
void TTL::PreloadPermanentData()
{
	// load permanent sound
	memSetUseBit(MEM_AUDIO_DATA);
	Sfx::Initialize2D( "menu", 4 );	// this loads data -and- initializes 2d audio playback
	memClearUseBit(MEM_AUDIO_DATA);

	// load the specification file for things to preload
	g_fileLoader.LoadSpecificationFile( "FileList.txt" );
	g_fileLoader.LoadSpecificationFile( "LevelPreload.txt" );

//#ifdef CDROM
	g_fileLoader.LoadPermanentFiles();
//#endif

	t_Handle boneInfo = dblLoad( "objects\\boneinfo.dbl" );
	if ( boneInfo != INVALID_HANDLE )
		dblSetPermanent( boneInfo, true );
	else
		ASSERTS( false, "couldn't load objects\\boneinfo.dbl" );

//	g_FrontEnd.Shutdown();

	// player database (of geometry and textures to create players)
	PlayerDB::Load( "PlayerDB.txt" );

#ifdef PS2
	PreloadFileStreamFile("logodat.ico");
#endif //PS2

	// AI Var files
	g_consolePtr->LoadLibrary( "var\\ai_var.lib" );

	DataMgr::MarkPermanent();
}

/***************************************************************************/
/***************************************************************************/
void TTL::PreloadFileStreamFile(char *pFileName, void *pData /*= NULL*/, int nSize /*= 0*/)
{
	FileStream *pFile = FileStream::PreLoad(pFileName, pData, nSize);
	if (pFile)
		pFile->SetPermanent(true);
}

/***************************************************************************/
// setup the defaults for init'ing all the systems
// Comes with the current defaults in it
//
// CALM DOWN ON THE BLOCK COPY HERE, THIS IS ONLY FOR PARAMETERS YOU WANT TO
// OVERRIDE FROM THE DEFAULT GAME INIT STRUCTURE.
/***************************************************************************/
void TTL::SetupGameInit(
GAMEINIT &init)
{
	// Display stuff (some drivers ignore this)
	init.hires = true;
	init.frameBPP = 32;
	init.displayBPP = 32;
	init.zBPP = 24;
	init.cheapFSAA = true;
	init.palMode = g_wantPAL;

	// game engine stuff
	init.maxParticles = 3000;
	init.maxParticleEmitters = 500;
	init.maxSprites = 1200; // bumped from 1000 to 1200; we need more sprites for the lobby - Dan Silver
	init.maxDistortions = 10;
	init.maxAttachers = 50;
	init.maxSwooshes = 1000;
	init.maxSwooshEmitters = 50;
	init.maxScenes = 3;//one main scene, another for mttop2 dance banner, one more for tak animating loading screen
	init.maxSceneNodes = 2000;
	init.maxTextureSets = 90;
	init.maxDataOwners = 750;
	init.maxDataRecords = 1000;
	init.maxPoolInstances = 50;	// for instance-based attachers

	// layers stuff
	init.maxOpenFiles = 12;

	// Console stuff
	strcpy(init.consoleScriptPath, "var\\");
	strcpy(init.consoleScriptLibrary, "bigjuju.vap");

#ifdef USE_TUP_MEMORY_MANAGER
	/* turn on tupperware memory manager that mallocs lots of little buffers out a smaller number of larger buffers. */
	/* this is to avoid having to have zillions of memory handles available, and for faster free'ing. adam says that */
	/* it should be ok to enable this once and leave it */

	TupMemoryManager::Enable();
#endif //USE_TUP_MEMORY_MANAGER
}

/***************************************************************************/
/***************************************************************************/
char *TTL::GetVersion( void )
{
	return m_version;
}

void TTL::InitVersion( void )
{
	strcpy( m_version, TTL_VERSION );
}
