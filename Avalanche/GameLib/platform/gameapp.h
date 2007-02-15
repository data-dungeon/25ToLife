/*************************************************************************/
// Main windows code a gameloop style game
/*************************************************************************/
#ifndef GAMEAPP_H
#define GAMEAPP_H

// The essentials
#include "platform/game.h"
#include "Audio/audiolib.h"
#include "platform/gameinit.h"

//-------------------
// PS2 specific stuff
//-------------------
#ifdef PS2
#include "platform/PS2/PS2app.h"
#include "platform/PS2/ps2input.h"
#include "platform/PS2/ps2ccd.h"
#include "platform/PS2/ps2card.h"

// Some defines to make this platform specific
#define GAMEAPP_BASECLASS		PS2App
#define GAMEAPP_INPUTDRIVER	PS2SystemInputDriver
#define GAMEAPP_CONSOLEDISP	PS2CommandConsoleDisplay
#define GAMEAPP_MEMCARD			PS2MemCard
#endif

//----------------------
// Win32 specicfic stuff
//----------------------
#if defined (WIN32) && !defined (_XBOX)
#include "platform/Win32/Winapp.h"
#include "platform/Win32/wininput.h"
#include "platform/Win32/winccd.h"
#include "platform/Win32/wincard.h"

// Some defines to make this platform specific
#define GAMEAPP_BASECLASS		WindowsApp
#define GAMEAPP_INPUTDRIVER	WindowsSystemInputDriver
#define GAMEAPP_CONSOLEDISP	WindowsCommandConsoleDisplay
#define GAMEAPP_MEMCARD			WinMemCard
#endif

#if defined (_XBOX)
#include "platform/Xbox/xboxapp.h"
#include "platform/Xbox/xboxinput.h"
#include "platform/Xbox/xboxccd.h"
#include "platform/Xbox/xboxcard.h"

// Some defines to make this platform specific
#define GAMEAPP_BASECLASS		XboxApp
#define GAMEAPP_INPUTDRIVER	XboxSystemInputDriver
#define GAMEAPP_CONSOLEDISP	XboxCommandConsoleDisplay
#define GAMEAPP_MEMCARD			XboxMemCard
#endif

//-------------------
// GCN specific stuff
//-------------------
#ifdef GCN
#include "platform/GCN/gcnapp.h"
#include "platform/GCN/gcninput.h"
#include "platform/GCN/gcnccd.h"
#include "platform/GCN/gcncard.h"

// Some defines to make this platform specific
#define GAMEAPP_BASECLASS		GCNApp
#define GAMEAPP_INPUTDRIVER	GCNSystemInputDriver
#define GAMEAPP_CONSOLEDISP	GCNCommandConsoleDisplay
#define GAMEAPP_MEMCARD			GCNMemCard
#endif

/////////////////////
// PS2 specific stuff
/////////////////////
#ifdef PS2
#define APPLICATION_THREAD_PRIORITY		60
#endif

// Forward declarations
class TextScreen;

// The Game app class
class GameApp : public GAMEAPP_BASECLASS
{
public:
	// We are a global variable, no parameters alowed
	GameApp(void);
	~GameApp(void);

	// Who am i?
	static inline GameApp *Application(void)
		{ return((GameApp *)GAMEAPP_BASECLASS::IAm()); }
	static Game *GetGame() { return(game); }

	// What is the command line?
	const char *CommandLine(void)
		{ return(commandLine); }

	// Startup/shutdown
	bool Initialize(char *theCommandLine);

	//to boot up as fast as possible, some itialialization is deferred - this funciton finishes the deferred work
	bool FinishInitialization(void);
	
	void Terminate(void);

	// Return the game initialization structure.
	const GAMEINIT &GameInit(void)
		{ return(init); }
	
	// Called every processing "frame"
	void Process(void);

	// switch resolutions to the resolutions specified in a new res.var style file.
	// this function only does something on PS2.  It calls Display__Shutdown and
	// Display__Init, which will probably screw up anything that in texmem...
	void ReloadResVars(char *filename);

	// default "checkLocal" behavior of the Hog system
	HOG_CHECKLOCAL HogCheckLocal(void);

#ifdef WIN32
#ifndef _XBOX
	// Setup instance data
	bool		InitInstance(HINSTANCE hPrevInst, LPSTR cmdLine, int nCmdShow);

	// Clean up instance data
	void		TerminateInstance(void)
		{ Terminate(); }

	// Here's were you put your accelerators
	bool		HandleMessage(MSG &msg);

	// Handle some specific messages
	int		OnCreate( LPCREATESTRUCT createStuff );
	void		OnDestroy( void );
	int		HandleSizeChange( void );
	int		OnSize(int sizeType, int width, int height );
	int		OnClose( void );

	// For the input drivers
	void MessageWatch(UINT msg, WPARAM wParam, LPARAM lParam);

#endif
#endif
#ifdef DIRECTX_PC
	void FixMouseCursor();
#endif

	// setup input
	bool SetupInput(void);

	// The game itself must implement this as a static function
	static bool IsDVD();

	void SetPalMode( bool mode );

private:
	// The command line
	char							*commandLine;

	// Our game init
	GAMEINIT						init;
	
	// Input driver
	GAMEAPP_INPUTDRIVER		input;

	// Audio system
	Audio							audio;

	// The console
	CommandConsole				*console;

	// The console display mechanism
	GAMEAPP_CONSOLEDISP		*consoleDisplay;

	// The memory card manager
	GAMEAPP_MEMCARD			m_MemCardMgr;

	// The text output system
	TextScreen					*textScreen;	

	// Our game sequencer
	static Game					*game;

	// DEFAULTS FOR THE GAMEINIT!!!
	static void InitDefaults(GAMEINIT &setMeUp);

	// Setup the display parameters
	bool DisplayInit(void);

	// load a res.var resolution init file
	void LoadResVars(char* filename);

	// Setup the console display
	bool SetupConsoleDisplay(void);


	// setup sound
	bool SetupSound(void);
};

#endif
