//*************************************************************************
// Main windows code for Road Rash PC app
//*************************************************************************
#include "platform/PlatformPCH.h"



#ifdef MASSIVE_AD
#include "Massive\MassiveClientDriver.h"
#endif

#include "Layers/Memory.h"
#include "EngineHelper/timer.h"
#include "platform/MemoryCard/MemoryCard.h"
#include "Game/Managers/ResourceManager.h"
#include "cmdcon/cmdcon.h"


// We are global
GameApp gameApp;

// Gotta setup this (this is the bootstrapper)
GAMEAPP_BASECLASS	*GAMEAPP_BASECLASS::me = &gameApp;

// Setup some general code timing pages
enum {
	GLOBALSECTION_MAIN,
	GLOBALSECTION_INPUT,
	GLOBALSECTION_SOUND,
	GLOBALSECTION_GAME,
	GLOBALSECTION_CTDISPLAY
};
CODESECTION g_mainCS[] =
{
	{ "Main loop     ", TIMEFORMAT_AUTO, TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST, false },
	{ "Input", TIMEFORMAT_AUTO, TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST, true },
	{ "Sound", TIMEFORMAT_AUTO, TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST, false },
	{ "Game", TIMEFORMAT_AUTO, TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST, false },
	{ "Code Timer", TIMEFORMAT_AUTO, TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST, false }
};
int g_mainCSs = sizeof(g_mainCS) / sizeof(CODESECTION);
TIMERPAGEHANDLE g_mainCSHandle = NULL;

//*************************************************************************
// We are a global variable, no parameters alowed
//*************************************************************************
GameApp::GameApp(void)
{
	// Make sure this is nice
	InitDefaults(init);

	commandLine = NULL;

	// setup the glocal singletons
	g_inputPtr = &input;
	g_audioPtr = &audio;
	g_MemCardPtr = &m_MemCardMgr;
	g_MemCardManager = new CMemoryCard;

	// setup the console
	console = new CommandConsole;
	ASSERT(console != NULL);
	g_consolePtr = console;

	// No display yet
	consoleDisplay = NULL;

	// no text screen
	textScreen = NULL;
}

//*************************************************************************
//*************************************************************************
GameApp::~GameApp(void)
{
	if (g_MemCardManager)
		delete g_MemCardManager;

	// blow away the command line
	delete [] commandLine;
	delete console;
}

//*************************************************************************
// General initialize
//*************************************************************************
bool GameApp::Initialize(
char *theCommandLine)
{
	// Copy in the commandLine
	if (theCommandLine && *theCommandLine)
	{
		commandLine = new char[strlen(theCommandLine) + 1];
		ASSERT(commandLine);
		strcpy(commandLine, theCommandLine);
	}

#if defined(_XBOX)
	// The mother of all good hacks
	if (commandLine && strstr(commandLine, "-nohdtv") != 0)
	{
		extern bool g_allowHDTV; 
		g_allowHDTV = false; 
	}
#endif

	// setup the inits (calls the subclass for this info)
	game->SetupGameInit(init);

	// Setup the console display
	if (console)
	{
		consoleDisplay = new GAMEAPP_CONSOLEDISP (*console);
		ASSERT(consoleDisplay);
		console->SetDisplay(consoleDisplay);
		SetupConsoleDisplay();

		console->SetExecutePath(init.consoleScriptPath);
	}

	// Layers stuff
	Layers__SetMaxOpenFiles(init.maxOpenFiles);

	if (Layers__Init() == FALSE)
		return(false);

	// Setup the display parameters
	if (!DisplayInit())
		return(false);

	// game engine default overrides
	GameEngine__SetMaxParticles(init.maxParticles);
	GameEngine__SetMaxParticleEmitters(init.maxParticleEmitters);
	GameEngine__SetMaxSprites(init.maxSprites);
	GameEngine__SetMaxDistortions(init.maxDistortions);
	GameEngine__SetMaxAttachers(init.maxAttachers);
	GameEngine__SetMaxSwooshes(init.maxSwooshes);
	GameEngine__SetMaxSwooshEmitters(init.maxSwooshEmitters);
	GameEngine__SetMaxSceneNodes(init.maxSceneNodes);
	GameEngine__SetMaxScenes(init.maxScenes);
	GameEngine__SetMaxTextureSets(init.maxTextureSets);
	GameEngine__SetMaxDataOwners(init.maxDataOwners);
	GameEngine__SetMaxDataRecords(init.maxDataRecords);
	GameEngine__SetMaxCustomChunkLoaders(init.maxCustomChunkLoaders);
	GameEngine__SetMaxPoolMatrices(init.maxPoolMatrices);
	GameEngine__SetMaxPoolFrustums(init.maxPoolFrustums);
	GameEngine__SetMaxPoolInstances(init.maxPoolInstances);

	// init the game engine
	if (GameEngine__Init() == FALSE)
		return(false);

#ifdef ENABLE_WATCHCLIENT
	// Startup the watch client, if we can
	WatchClient::Instance()->Start();
#endif //ENABLE_WATCHCLIENT

	// Startup the game
	ASSERT(game);
	if (!game->Initialize())
	{
#ifdef DIRECTX_PC
		Terminate();
#endif
		return(false);
	}
	// Setup the master code section page
	g_mainCSHandle = g_codeTimer.RegisterSections("Main Timings",
																				g_mainCS,
																				g_mainCSs);

	// record "permanent" memory allocations-- malloc's and new's up to this point will last whole game, anything
	// after this has to clean itself up to avoid memory leaks
#ifndef CONSUMER_BUILD
	Layers__MarkPermanentMemory();
#endif

	// okay!
	return(true);
}

//*************************************************************************
// Finish any deferred initialization to get first screen visible as fast as possible
//*************************************************************************
bool GameApp::FinishInitialization(void)
{
#ifdef PS2
	//Finish any incomplete platform initialization
	GAMEAPP_BASECLASS::FinishInitialize();
#endif

	// Setup the OS input stuff
	if (!SetupInput())
		return(false);

	// Setup the sound system, let's not care about errors for now
#ifdef DIRECTX_PC
	if (!SetupSound())
	{
		MessageBox(NULL, "Failed to initialize sound system.  \nMake sure sound card is present and working", "error", MB_OK | MB_ICONERROR);
		return false;
	}
#else
	SetupSound();
#endif
	return(true);
}

//*************************************************************************
// Never called on PS2 (except for magazine demos)
//*************************************************************************
void GameApp::Terminate(void)
{

	ASSERT(game);
	game->Terminate();

#ifdef ENABLE_WATCHCLIENT
#if defined(WIN32) && !defined(_XBOX)
	// Shut down watch
	WatchClient::Instance()->End();
	WatchClient::DeleteInstance();
#endif
#endif //ENABLE_WATCHCLIENT

	// Clear this
	audio.Destroy();

	// Release input resources
	input.Terminate();

	// Delete console
	if (consoleDisplay)
	{
		console->SetDisplay(NULL);
		delete consoleDisplay;
		consoleDisplay = NULL;
	}
	console->Clear();

#ifdef _DEBUG
	// Close the main log file
	g_log.Close();
#endif

	// Get rid of the text screen
	if (textScreen)
	{
		delete textScreen;
		textScreen = NULL;
	}

	// Shutdown the game engine
	GameEngine__Shutdown();
}

//*************************************************************************
// Windows specific init
//*************************************************************************
#if defined(WIN32) && !defined(_XBOX)
bool GameApp::InitInstance(
HINSTANCE hPrevInst,
LPSTR cmdLine,
int nCmdShow)
{
	// setup the display system for windows
#ifdef DIRECTX_PC
	applicationMutex = ::CreateMutex(NULL, false, "ttl");
	if (::GetLastError() == ERROR_ALREADY_EXISTS || 
		::GetLastError() == ERROR_ACCESS_DENIED )
	{
		MessageBox(NULL, "Cannot start 25 to life, Another instance is already running.", "Already Running", MB_OK );
		return(false);
	}
#endif
	Display__SetWinInfo(GetDesktopWindow(), hInst, windowProc, nCmdShow, LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON2)));

#ifdef DIRECTX
	if( cmdLine )
		Platform__ProcessCommandLine( cmdLine );
#endif
	// Allow the aux text system to startup
	textScreen = new TextScreenWin32(hInst);
	TextWindow::Setup(textScreen);

	// Open the file and create a window
#ifdef _DEBUG
	g_log.Open("main.log", false);
#endif

	// Call the app init
	if (!Initialize(cmdLine))
		return(false);

	// Stuff worked
	return(true);
}
#endif

//*************************************************************************
// Called every processing "frame"
//*************************************************************************
void GameApp::Process(void)
{
	// Tell the code timing system we have a new frame
	g_codeTimer.FrameStart();

	// Start the timer for main loop
	g_codeTimer.Start(g_mainCSHandle, GLOBALSECTION_MAIN);

	// Freeze the digital input state
	g_codeTimer.Start(g_mainCSHandle, GLOBALSECTION_INPUT);
	input.BeginInput();
	g_codeTimer.End(g_mainCSHandle, GLOBALSECTION_INPUT);

	// Check for switching text window pages,
	// only handled on WIN32 for right now
#if defined(WIN32) && !defined(_XBOX)
#if defined(MEM_USE_WIN32_MANAGER)	//textpages crash when exiting from our memory manager
	if (input.ButtonPressed(BC_PAGEDOWN))
		TextPageSystem::NextPage();
	else if (input.ButtonPressed(BC_PAGEUP))
		TextPageSystem::PreviousPage();
#endif
#endif

	// Let the sounds process
	g_codeTimer.Start(g_mainCSHandle, GLOBALSECTION_SOUND);
	audio.Daemon();
	g_codeTimer.End(g_mainCSHandle, GLOBALSECTION_SOUND);

	// Let the media system process
	Media::Daemon();

	// Run the game loop
	g_codeTimer.Start(g_mainCSHandle, GLOBALSECTION_GAME);

	// Handle background load of files
	g_resourceManager.Update();

#ifdef ENABLE_WATCHCLIENT
	// Give the watch client some processor time
	WatchClient::Instance()->Run();
#endif //ENABLE_WATCHCLIENT

	// New frame!
	g_timer.NewFrame();
		
	// This lets all the important stuff update
	g_controlFocus.Update();

	ASSERT(game);
	if (!game->Run())
		Quit();
	g_codeTimer.End(g_mainCSHandle, GLOBALSECTION_GAME);

	// Done with the digital input
	g_codeTimer.Start(g_mainCSHandle, GLOBALSECTION_INPUT);
	input.DoneInput();
	g_codeTimer.End(g_mainCSHandle, GLOBALSECTION_INPUT);

	// Done with main
	g_codeTimer.End(g_mainCSHandle, GLOBALSECTION_MAIN);

	// End of frame
	g_codeTimer.Start(g_mainCSHandle, GLOBALSECTION_CTDISPLAY);
	g_codeTimer.Display();
	g_codeTimer.End(g_mainCSHandle, GLOBALSECTION_CTDISPLAY);
	g_codeTimer.FrameEnd();
#ifdef DIRECTX_PC
	setSoundLevels = false;
#endif
}

//*************************************************************************
// DEFAULTS FOR THE GAMEINIT
// DO NOT MODIFY THIS, INSTEAD MODIFY THE GAME SPECIFIC INIT
// (i.e. see rashgame.cpp for the SetupGameInit function)
//*************************************************************************
void GameApp::InitDefaults(
GAMEINIT &setMeUp)
{
	// clear it
	memset(&setMeUp, '\0', sizeof(GAMEINIT));

	// Display stuff (some drivers ignore this)
	setMeUp.hires = true;
	setMeUp.displayHalfHeight = false;
	setMeUp.frameBPP = 16;
	setMeUp.zBPP = 16;
	setMeUp.cheapFSAA = true;
	setMeUp.palMode = false;
	setMeUp.displayBPP = 0; // dont downsample by default
#ifdef _DEBUG
	setMeUp.fullScreen = false;
#else
	setMeUp.fullScreen = true;
#endif

	// Xbox stuff.
	#ifdef _XBOX
	setMeUp.width = 640;
	setMeUp.height = 480;
	setMeUp.format = 0;
	setMeUp.multisampletype = 0;		//none
//	setMeUp.multisampletype = 2;		//2x quincunx
//	setMeUp.multisampletype = 6;		//4x gaussian
	#endif

	// game engine stuff
	setMeUp.maxParticles = 500;
	setMeUp.maxParticleEmitters = 250;
	setMeUp.maxSprites = 250;
	setMeUp.maxDistortions = 0;
	setMeUp.maxAttachers = 0;
	setMeUp.maxSwooshes = 100;
	setMeUp.maxSwooshEmitters = 10;
	setMeUp.maxScenes = 2;
	setMeUp.maxSceneNodes = 200;
	setMeUp.maxScenes = 2;
	setMeUp.maxTextureSets = 32;
	setMeUp.maxDataOwners = 100;
	setMeUp.maxDataRecords = 250;
	setMeUp.maxCustomChunkLoaders = 10;
	setMeUp.maxPoolMatrices = 100;
	setMeUp.maxPoolFrustums = 32;
	setMeUp.maxPoolInstances = 0;

	// layers stuff
	setMeUp.maxOpenFiles = 10;
//	setMeUp.memoryHandles = 1000;
//	setMeUp.poolConfig = NULL;	// must be supplied by game
//#ifdef GCN
//	setMeUp.ARAMPoolConfig = NULL;	// must be supplied by game
//#endif //GCN
//#ifdef CDROM
//	setMeUp.memorySize = 0x1800000;//allows 8MB for static mem and CHeap usage
//#else
//	setMeUp.memorySize = 0x3000000;
//#endif

	// Console stuff
	strcpy(setMeUp.consoleScriptPath, "var\\");
	strcpy(setMeUp.consoleScriptLibrary, "game.vap");
}

void GameApp::ReloadResVars(char* filename)
{
	// only the ps2 allows switching video resolutions...
#ifdef PS2
	Display__Shutdown();
	LoadResVars(filename);
	Display__Init();
#endif // PS2
}

void GameApp::LoadResVars(char* filename)
{
#if defined(_XBOX) && !defined(CDROM)
	// Let the console script override this stuff
	if (console)
	{
		console->CreateVar(NULL, "width", init.width);
		console->CreateVar(NULL, "height", init.height);
		console->CreateVar(NULL, "format", init.format);
		console->CreateVar(NULL, "multisampletype", init.multisampletype);
		console->ExecuteFile(filename);

		console->Echo("xbox.var: width = %d", init.width);
		console->Echo("xbox.var: height = %d", init.height);
		console->Echo("xbox.var: format = %d", init.format);
		console->Echo("xbox.var: multisampletype = %d", init.multisampletype);
	}

	Display__SetXboxOptions(init.width, init.height, init.format, init.multisampletype);
#endif //_XBOX

#if defined(PS2)

	// Let the console script override this stuff
	if (console && filename != NULL)
	{
		console->CreateVar(NULL, "hires", init.hires);
		console->CreateVar(NULL, "frameBPP", init.frameBPP);
		console->CreateVar(NULL, "zBPP", init.zBPP);
		console->CreateVar(NULL, "2circuit", init.cheapFSAA);
		console->CreateVar(NULL, "PAL", init.palMode);
		console->CreateVar(NULL, "displayBPP", init.displayBPP);
		console->CreateVar(NULL, "displayHalfHeight", init.displayHalfHeight);
		console->ExecuteFile(filename);
	}

	//NOTE: I think Steve Anderson's OpenGL apparnetly is hardcoded to NTSC screen widths/heights???...PAL displays incorrectly
	//SO, for now, use same display, but letterbox it
	
	//ALSO NOTE: a note from SCEE says to allow NTSC/60Hz operation at the PAL user's request (many PAL TV's handle this, evidently).
	//ALSO NOTE 2: the same note from SCEE says to allow the user to fine-adjust the screen image placement due to PAL cables having different behaviors.

	//video parameters differ according to what type of TV system we're on
	int nDisplayWidth, nDisplayHeight;
	int nRenderWidth, nRenderHeight;

	if (init.palMode)
	{
		//PAL/SECAM
		Display__SetNTSCMode(false);
		nDisplayWidth = 512;
		nDisplayHeight = 512;
		nRenderWidth = 512;
		nRenderHeight = 416;
	}
	else
	{
		//NTSC
		Display__SetNTSCMode(true);
		nDisplayWidth = 640;
		nDisplayHeight = 448;
		nRenderWidth = 640;
		nRenderHeight = 448;
	}

	// is the game requesting a hires draw buffer?
	if (init.hires)
	{
		// is the game requesting a half height display buffer?
		if(init.displayHalfHeight)
		{
			//set up for 512x448->512x224
			Display__SetDisplayDimensions(nDisplayWidth, nDisplayHeight / 2);
			Display__SetScreenWidth(nRenderWidth);//display buffer width
			Display__SetScreenHeight(nRenderHeight);//display buffer height
			Display__SetInterlace(TRUE);//true -> interlace
			Display__SetFrameMode(TRUE);//true -> repeat display each field, false -> alternate odd/even
			Display__Set2Circuit(init.cheapFSAA);
			if (console)
				console->Echo("Running in %dx%d -> %dx%d", nRenderWidth, nRenderHeight, nDisplayWidth, nDisplayHeight / 2);
		}
		else
		{
			//set up for 512x448->512x448
			Display__SetDisplayDimensions(nDisplayWidth, nDisplayHeight);
			Display__SetScreenWidth(nRenderWidth);//display buffer width
			Display__SetScreenHeight(nRenderHeight);//display buffer height
			Display__SetInterlace(TRUE);//true -> interlace
			Display__SetFrameMode(FALSE);//true -> repeat display each field, false -> alternate odd/even
			Display__Set2Circuit(init.cheapFSAA);
			if (console)
				console->Echo("Running in %dx%d -> %dx%d", nRenderWidth, nRenderHeight, nDisplayWidth, nDisplayHeight);
		}
	}
	else
	{
		//set up for 512x224->512x224
		Display__SetDisplayDimensions(nDisplayWidth, nDisplayHeight / 2);
		Display__SetScreenWidth(nRenderWidth);//display buffer width
		Display__SetScreenHeight(nRenderHeight/2);//display buffer height
		Display__SetInterlace(FALSE);//true -> interlace
		Display__SetFrameMode(TRUE);//true -> repeat display each field, false -> alternate odd/even
		if (console)
				console->Echo("Running in %dx%d -> %dx%d", nRenderWidth, nRenderHeight / 2, nDisplayWidth, nDisplayHeight / 2);
	}

	Display__SetFrameBufferBPP(init.frameBPP, init.displayBPP);
	Display__SetZBufferBPP(init.zBPP);
	if (console)
	{
		console->Echo("Frame-buffer BPP %d", init.frameBPP);
		console->Echo("display-buffer BPP %d", init.displayBPP);
		console->Echo("Z-buffer BPP %d", init.zBPP);
		console->Echo("2Circuit %d", init.cheapFSAA);
		console->Echo("PAL %d", init.palMode);
	}
#endif //PS2
}

//*************************************************************************
// setup all the display stuff
//*************************************************************************
bool GameApp::DisplayInit(void)
{

#ifdef _XBOX
	LoadResVars("xbox");
#endif

#ifdef PS2
	LoadResVars( NULL);
#endif

	return(true);
}

//*************************************************************************
// Setup the OS input stuff
//*************************************************************************
bool GameApp::SetupInput(void)
{
	static bool IsInitialized = false;

	if (!IsInitialized)
	{
#ifdef PS2
		if (!input.Initialize())
			return(false);
#endif
#ifdef GCN
		if (!input.Initialize())
			return(false);
#endif
#ifdef WIN32
#ifdef _XBOX
		if (!input.Initialize())
			return(false);
#else
		if (!input.Initialize(hInst, Display__GetWnd()))
			return(false);
#endif
#endif
		IsInitialized = true;
	}
	return(true);
}

//*************************************************************************
// setup sound
//*************************************************************************
bool GameApp::SetupSound(void)
{
#if !defined(WIN32) || defined(_XBOX)
	if (!audio.Create(0))
#else
	if (!audio.Create(Display__GetWnd()))
#endif
		return(false);

	return(true);
}

//*************************************************************************
// Setup the console display
//*************************************************************************
bool GameApp::SetupConsoleDisplay(void)
{
#if defined(WIN32) && !defined(_XBOX)
	// Create the window for the console
	if (consoleDisplay)
		consoleDisplay->Create(hInst, hWnd);
#endif

	return(true);
}

//*************************************************************************
// ALL THE WIN32 MESSAGING STUFF FOLLOWS
//*************************************************************************

#ifdef WIN32
#ifndef _XBOX
//*************************************************************************
// Here's were you put your accelerators
//*************************************************************************
bool GameApp::HandleMessage(
MSG &msg)
{
	if (consoleDisplay)
		return(consoleDisplay->HandleMessage(msg));
	else
		return(false);
}

//*************************************************************************
// Create message
//*************************************************************************
int GameApp::OnCreate(
LPCREATESTRUCT createStuff)
{
	int success = gfxOnCreate(hWnd);
	if (success)
		return gfxOnCreated(hWnd);
	else
		return false;
}

//*************************************************************************
// Destroy message
//*************************************************************************
void GameApp::OnDestroy(void)
{


#ifdef MASSIVE_AD
	MassiveClientReleaseDriver();
#endif


	Display__DestroyWindow();	// clean after window is closed
	gfxOnDestroy();

#ifdef DIRECTX_PC
	//release our mutex
	::ReleaseMutex(applicationMutex);
#endif
}

//*************************************************************************
// Window is sizing
//*************************************************************************
int GameApp::OnSize(
int sizeType,
int width,
int height)
{
	gfxOnResize(width, height);
	return(0);
}
//*************************************************************************
// Window size may have changed
//*************************************************************************
int GameApp::HandleSizeChange( void )
{
RECT			NewRect;

	GetClientRect( hWnd, &NewRect );
	gfxOnResize( (short) ( NewRect.right - NewRect.left ), (short) ( NewRect.bottom - NewRect.top ) );
	return(0);
}

//*************************************************************************
// Window is being closed
//*************************************************************************
int GameApp::OnClose(void)
{
	if (game)
		game->Exit();
	else
		Quit();

	return(0);
}

//*************************************************************************
// For the input drivers
//*************************************************************************
void GameApp::MessageWatch(
UINT msg,
WPARAM wParam,
LPARAM lParam)
{
	// Let the digital input driver get at OS messages
	input.HandleMessage(hWnd, msg, wParam, lParam);
}
#endif
#endif

//*************************************************************************
// Change the "checkLocal" behavior of the Hog system. this is called once
// by GameApp::Initialize. can change its state later in runtime by calls
// Hog_SetCheckLocal. typically, for a cd build that uses hog files you
// don't want to check outside the hog file-- all files should be included
//*************************************************************************
HOG_CHECKLOCAL GameApp::HogCheckLocal(void)
{
#ifdef CDROM
	// Check after the hog for file not founds
	return(CHECKLOCAL_AFTER);
#else
	// Check for updated files before we check hog
	return(CHECKLOCAL_BEFORE);
#endif	
}

#ifdef DIRECTX_PC
void GameApp::FixMouseCursor()
{
	RECT	newRect;
	GetClientRect( hWnd, &newRect );
	DisplayState.nScreenWidth = newRect.right - newRect.left;
	DisplayState.nScreenHeight = newRect.bottom - newRect.top;
}
#endif


void GameApp::SetPalMode( bool mode )
{
	init.palMode = mode;
	LoadResVars( NULL );
}