//*************************************************************************
// Xbox application framework.
//*************************************************************************
#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#include <string.h>

#include "platform/PlatformPCH.h"

#include "Layers/Debug.h"
#include "platform/Xbox/xboxapp.h"
#include "platform/Xbox/xbfont.h"

#include <xfont.h>

#if defined (REPLAY)
#include "Replaylib.h"
#endif

//#define	XBOX_24BIT_FP			// use 24-bit precision instead of 53.

CXBFont			g_Font;
XFONT*			g_pDefaultTrueTypeFont;  // Pointer to the Default Arial TrueTypeFont

//*************************************************************************
// Here it is, our very own WinMain
//*************************************************************************
int main(int argc, char* argv[])
{

	#if defined (REPLAY)
	ReplayStaticInit();
	#endif

	// set Xbox FP precision to 24 bits instead of 53..
	#if defined (XBOX_24BIT_FP)
	_controlfp(_PC_24, _MCW_PC);
	#endif

	XboxApp *app = XboxApp::IAm();

	char* cmdline = NULL;

	DWORD type;
	LAUNCH_DATA data;
	if (SUCCEEDED(XGetLaunchInfo(&type, &data)))
		if (type == LDT_FROM_DEBUGGER_CMDLINE)
			cmdline = ((PLD_FROM_DEBUGGER_CMDLINE)&data)->szCmdLine;


	#ifndef DISABLE_ASSERTS
	Assert::SetHandler( Assert::XboxAssertHandler );
	#endif

	// Setup everything
#ifndef CONSUMER_BUILD
	dbgPrint("Command line: %s\n", cmdline);
#endif
	bool ret = app->Initialize(cmdline);
	ASSERT(ret);


	// Process the game
	while (app->KeepProcessing())
		app->Process();

	// In a non-debug build we should never get here
#ifndef _DEBUG
	ASSERT(false);
#endif


	// Shutdown everything
	app->Terminate();

	return(0);
}

//*************************************************************************
//*************************************************************************
XboxApp::XboxApp(void)
{
	quit = false;
}

//*************************************************************************
// We are quitting! (does nothing on Xbox)
//*************************************************************************
void XboxApp::Quit(void)
{
	quit = true;
}
