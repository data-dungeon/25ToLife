/* assert.c */

/******************* includes ****************************/

#include "Layers/LayersPCH.h"
#include "Display/DirectX/DirectX.h"

#include "EngineHelper/drawutil.h"
#include "Display/ColorConversions.h"
#include "platform/systemi.h"
#include "platform/gameapp.h"
#include "GameHelper/StringHelper.h"
#include "EngineHelper/LoadScreen.h"

extern	GameApp gameApp;
#include "platform/Xbox/xboxapp.h"

#if !defined(CONSUMER_BUILD)
void LOCKUP()
{ 
	Sleep(201); 
	__asm { int 3 } 
}

void LOCKUP_MEM()
{ 
	Sleep(201); 
	__asm { int 3 } 
}
#endif

#ifndef DISABLE_ASSERTS

#include "Display/Common/DisplayPlatform.h"

#define ASSERT_SUPPORT_IGNORE
#define ASSERT_SUPPORT_IGNORE_ALWAYS
#define ASSERT_SUPPORT_ABORT
#define ASSERT_SUPPORT_QUIT

#define ASSERT_NOASSERT 0
#define ASSERT_ABORT 1
#define ASSERT_QUIT 2
#define ASSERT_IGNORE 3
#define ASSERT_IGNORE_ALWAYS 4


#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define OFFX (((4096-SCREEN_WIDTH)/2)<<4)
#define OFFY (((4096-SCREEN_HEIGHT)/2)<<4)

#define PAUSETIME 10
#define PAUSETIMESTR "10"

#define CONSWIDTH 38
#define CONSHEIGHT 27

#define CONSOLE_BLACK 0
#define CONSOLE_BLUE 1
#define CONSOLE_RED 2
#define CONSOLE_MAGENTA 3
#define CONSOLE_GREEN 4
#define CONSOLE_CYAN 5
#define CONSOLE_YELLOW 6
#define CONSOLE_WHITE 7


extern "C" 
{
	extern char _end[];
	extern char ENTRYPOINT[];
};

// dx stuff

ControllerInputDriver *controller;

#define	COLOR_RED     RGBA_TO_COLOR(255,0,0,255)
#define	COLOR_GREEN   RGBA_TO_COLOR(0,255,0,255)
#define	COLOR_BLUE    RGBA_TO_COLOR(0,0,255,255)
#define	COLOR_WHITE   RGBA_TO_COLOR(255,255,255,255)
#define	COLOR_YELLOW  RGBA_TO_COLOR(255,0,255,255)

namespace Assert
{

	// we need to disable asserts while we cleanup.. otherwise, we 
	// may assert forever..
	static bool disableAsserts = false;

	// controller support
	static int hasControllerIdled;
	static bool hasControllerSupport;
	
	
	static void SetupDisplay()
	{
		if (LoadScreen::IsRunning())
			LoadScreen::Kill();

		DirectX_Device->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
										D3DCOLOR_XRGB( (int)200, (int)0, (int)0 ), 1.0f, 0L );
		DirectX_Device->Present( NULL, NULL, NULL, NULL );
	}

	static void RestoreDisplay()
	{
	}


DWORD	err;

	static int GetControllerResults(int assertType)
	{
		if(!hasControllerSupport)
		{
			hasControllerIdled++;
			if((hasControllerIdled >= 60*PAUSETIME) && (assertType & ASSERT_IGNORE))
				return ASSERT_IGNORE;
			if((hasControllerIdled >= 60*PAUSETIME) && (assertType & ASSERT_ABORT))
				return ASSERT_ABORT;
			if((hasControllerIdled >= 60*PAUSETIME) && (assertType & ASSERT_QUIT))
				return ASSERT_QUIT;
			return ASSERT_NOASSERT;
		}


		// pad?
		if (g_inputPtr == NULL)
			return ASSERT_NOASSERT;

		controller = g_inputPtr->GetAssertController();

		// ignore
		if( controller->ButtonPressed( CB_X))
			return ASSERT_IGNORE;

		// ignore always
		if( controller->ButtonPressed( CB_START))
			return ASSERT_IGNORE_ALWAYS;

		// debug
		if( controller->ButtonPressed( CB_SELECT))
			return ASSERT_ABORT;

		// quit
		if( controller->ButtonPressed( CB_L2) && controller->ButtonPressed(CB_R2))
			return ASSERT_QUIT;

		return ASSERT_NOASSERT;
	}

	static void leftString(int line, int color, const char* string, int offset = 0)
	{
		line *= 18;
		offset = (offset* 14) + 50;
		g_Font.DrawText( (float)offset, (float)line, color , (WCHAR*) CStringHelper::AsciiToUnicode((char*)string) );
	}

	static void centerMsg(int line, int color, const char* string)
	{
		line *= 18;
		int x = (430 - strlen(string)) / 2;
		g_Font.DrawText( (float)x, (float)line, color , (WCHAR*) CStringHelper::AsciiToUnicode((char*)string));
	}

	static int DisplayAssert(const char* expr, const char* file, int line, int assertType, const char* artistString)
	{
		char	buffer[250];
		int	result;

		// get ready
		SetupDisplay();

		while(1)
		{

			LPDIRECT3DSURFACE8 pFrontBuffer;
			DirectX_Device->GetBackBuffer(-1,D3DBACKBUFFER_TYPE_MONO,&pFrontBuffer);
			g_Font.Begin();

			g_Font.SetScaleFactors (2.0f, 2.0f );

			int position = 1;
			leftString( position++, COLOR_RED, "Assertion failure!", 7);
			position+=2;

			g_Font.SetScaleFactors (1.0f, 1.0f );

			if(artistString)
			{
				centerMsg(position, COLOR_GREEN, "Hopefully Readable String");
				leftString(position+2, COLOR_YELLOW, StringLookup(artistString));
				position += 7;
			}

			centerMsg(position, COLOR_GREEN, "Expression and Location");
			position += 2;

			leftString(position,COLOR_GREEN,"Expr");
			leftString(position,COLOR_YELLOW, StringLookup(expr), 5);
			position += 2;

			leftString(position, COLOR_GREEN, "Line");
			sprintf(buffer, "%d", line);
			leftString(position,COLOR_YELLOW, buffer, 5);

			position += 2;
			leftString(position, COLOR_GREEN, "File");
			leftString(position, COLOR_YELLOW, StringLookup(file), 5);
			position += 7;

			if(assertType & ASSERT_QUIT)
				leftString( position--, COLOR_WHITE, "L1 + R1 - Exit the application", 8);
		
			if(assertType & ASSERT_ABORT)
				leftString(position--, COLOR_WHITE, "Back - Stop in debugger", 8);
		
			if(assertType & ASSERT_IGNORE_ALWAYS)
				leftString(position--,COLOR_WHITE,  "Start - Always ignore this assert", 8);
		
			if(assertType & ASSERT_IGNORE)
				leftString(position--,COLOR_WHITE,  "A - Try to continue", 8);

			position--;

			centerMsg(position,COLOR_GREEN,  "Controller Commands");

			// loop until we get input...
			result = ASSERT_NOASSERT;
			int toggle = 0;

			g_Font.End();
			pFrontBuffer->Release();

			hasControllerIdled = false;

			result = GetControllerResults(assertType);

			DirectX_Device->Present( NULL, NULL, NULL, NULL );
			DirectX_Device->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
											D3DCOLOR_XRGB( (int)0, (int)0, (int)0 ), 1.0f, 0L );

			g_inputPtr->BeginInput();
			g_inputPtr->DoneInput();

			if (result != ASSERT_NOASSERT)
				break;
		}	
		
		// put things back
		RestoreDisplay();

		return result;
	}
	
	bool XboxAssertHandler(const char *expr, const char *file, int line, const char* string)
	{
		// provide asserts on screen with abort-ignore functionality...
	
		// we only get in here if an assert has failed... so search the table of file/line
		// pairs so we can see if ignore has happened for this assert...
	
		int assertType = 0;

		if(disableAsserts)
			return true;

		hasControllerSupport = 1;

		gameApp.SetupInput();
	
		controller = g_inputPtr->GetAssertController();

#ifdef ASSERT_SUPPORT_ABORT
		assertType |= ASSERT_ABORT;
#endif

#ifdef ASSERT_SUPPORT_IGNORE
		assertType |= ASSERT_IGNORE;
#endif

#ifdef ASSERT_SUPPORT_IGNORE_ALWAYS
		assertType |= ASSERT_IGNORE_ALWAYS;
		if(IgnoreAlways(expr, file, line, string))
			return true;
#endif

		const char* loadedstring;
		// this must be split up because StringLookup can only handle 1 string at a time
		loadedstring = StringLookup(file);
		dbgPrint( "Assertion failed.  file: %s, line: %d\n", loadedstring, line);
		loadedstring = StringLookup(expr);
		dbgPrint( " expr: %s\n", loadedstring);
		if(string)
		{
			loadedstring = StringLookup(string);
			dbgPrint( "String: %s\n",  loadedstring);
		}

		int assert = DisplayAssert(expr, file, line, assertType, string);

#ifdef ASSERT_SUPPORT_IGNORE_ALWAYS
		if(assert == ASSERT_IGNORE_ALWAYS)
			SetIgnoreAlways(expr, file, line, string, true);
#endif // ASSERT_SUPPORT_IGNORE

#ifdef ASSERT_SUPPORT_QUIT
		if(assert == ASSERT_QUIT)
			exit(0);
#endif

#ifdef ASSERT_SUPPORT_ABORT
		if(assert == ASSERT_ABORT)
			return false;
#endif
		// clear screen
		DirectX_Device->Present( NULL, NULL, NULL, NULL );
		DirectX_Device->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
											D3DCOLOR_XRGB( (int)0, (int)0, (int)0 ), 1.0f, 0L );

		return true;
	}


};

#endif // DISABLE_ASSERTS
