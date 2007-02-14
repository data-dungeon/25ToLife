#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "frontend/screens/screen_PalMode.h"
#include "data/screens/MainMenu.h"
#include "Platform//gameapp.h"

extern GameApp gameApp;
extern bool g_wantPAL;
bool			g_PalModeDone = false;

extern void SetupFMVPaths();

void ScreenPalMode::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	UseHighlighting( true );

	SetInitialButton( ID_PALMODE_TEST60 );
	SetNextButton( ID_PALMODE_USE60 );
	SetNextButton( ID_PALMODE_USE50 );
	SetCursor( ID_PALMODE_CURSOR );

	SetText( ID_PALMODE_TEST60, "Test NTSC 60hz" );
	SetText( ID_PALMODE_USE60, "Use NTSC 60hz");
	g_PalModeDone = false;
}

void ScreenPalMode::OnCommand( int Command )
{
	switch( Command )
	{
	case ID_PALMODE_TEST60:
		OnCommandTestPal60();
		break;

	case ID_PALMODE_USE60:
		OnCommandUsePal60();
		break;

	case ID_PALMODE_USE50:
		OnCommandUsePal50();
		break;
	}

	return;
}

void ScreenPalMode::OnCommandTestPal60( void )
{
	GotoScreen( "SCREEN_ID_TESTPAL60" );
#if defined (PS2)
	Display__ClearGraphics();
	Display__Shutdown(false);
	g_wantPAL = false;
	gameApp.SetPalMode( g_wantPAL );
	Display__Init();
	Display__ClearGraphics();
#endif
}

void ScreenPalMode::OnCommandUsePal60( void )
{
#if defined (PS2)
	Display__ClearGraphics();
	Display__Shutdown(false);
	g_wantPAL = false;
	gameApp.SetPalMode( g_wantPAL );
	Display__Init();
	Display__ClearGraphics();
#endif

	g_PalModeDone = true;
	SetupFMVPaths();

	GotoScreen( "SCREEN_ID_TITLE" );
}

void ScreenPalMode::OnCommandUsePal50( void )
{
#if defined (PS2)
	Display__ClearGraphics();
	Display__Shutdown(false);
	g_wantPAL = true;
	gameApp.SetPalMode( g_wantPAL );
	Display__Init();
	Display__ClearGraphics();
#endif

	g_PalModeDone = true;
	SetupFMVPaths();

	GotoScreen( "SCREEN_ID_TITLE" );
}

void ScreenTestPal60::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();

	SetText( ID_TESTPAL60_TITLE, "Testing NTSC 60hz . . ." );
	m_TimeOut = g_timer.GetOSTime() + 10000;
}

void ScreenTestPal60::OnScreenUpdate( void )
{
	if( g_timer.GetOSTime() >= m_TimeOut )
	{
#if defined (PS2)
		Display__ClearGraphics();
		Display__Shutdown(false);
		g_wantPAL = false;
		gameApp.SetPalMode( g_wantPAL );
		Display__Init();
		Display__ClearGraphics();
#endif

		GotoPrevScreen();
	}
	return;
}

void ScreenTestPal60::OnButtonPressed( int Button )
{
	// ignore button presses until some time has elapsed so we
	// can't flicker the display back and forth too fast, TRC issue.
	if( g_timer.GetOSTime() < m_TimeOut - 8500 )
		return;

	switch( Button )
	{
	case FluffControlMapper::X:
	case FluffControlMapper::TRIANGLE:
	case FluffControlMapper::CIRCLE:
	case FluffControlMapper::SQUARE:
	case FluffControlMapper::START:
	case FluffControlMapper::SELECT:
#if defined (PS2)
		Display__ClearGraphics();
		Display__Shutdown(false);
		g_wantPAL = false;
		gameApp.SetPalMode( g_wantPAL );
		Display__Init();
		Display__ClearGraphics();
#endif
		// wait 2 sec before going back
		uint32 delay = g_timer.GetOSTime() + 1500;
		while ( g_timer.GetOSTime() < delay )
		{
		}
		GotoPrevScreen();
		break;
	}

	return;
}