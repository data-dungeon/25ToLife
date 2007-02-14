#include "TTLPCH.h"
#include "frontend/frontend.h"
#include "frontend/savegame.h"
#include "frontend/screens/screen_CreateProfile.h"
#include "frontend/screens/screen_Keyboard.h"
#include "frontend/screens/screen_BasePopup.h"
#include "data/screens/Popups.h"
#include "data/screens/mainmenu.h"
#include "platform/MemoryCard/MemoryCard.h"
#include "main/PlayerDB.h"

#if defined(DIRECTX_PC)
	#define RESTRICTED_PROFILE_CHARS "/\\ *?<>.:\""
#else
	#define RESTRICTED_PROFILE_CHARS "/\\*?<>.:\""
#endif

//=============================================================================
// ScreenCreateProfile::ScreenCreateProfile - The default constructor
//=============================================================================

ScreenCreateProfile::ScreenCreateProfile( void )
{
	m_MemCardState = CREATEPROFILE_IDLE;
}

//=============================================================================
// ScreenCreateProfile::~ScreenCreateProfile - The default destructor
//=============================================================================

ScreenCreateProfile::~ScreenCreateProfile( void )
{
}

//=============================================================================
// ScreenCreateProfile::OnScreenIntro
//=============================================================================

void ScreenCreateProfile::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();

	ScreenKeyboard* pKeyboard = GetKeyboard();

	if( pKeyboard )
	{
		pKeyboard->SetMaxString(SG_MAX_PROFILE_NAME-1);
		pKeyboard->RestrictKeys( true,  0x00, 0x1f );
		pKeyboard->RestrictKeys( false, 0x20, 0x7e );
		pKeyboard->RestrictKeys( true,  0x80, 0xff );
		pKeyboard->RestrictChars( true, RESTRICTED_PROFILE_CHARS );
		pKeyboard->SetString( "Profile1" );

		SetText( ID_CREATEPROFILE_BIGPROFILE, pKeyboard->GetKeyboardString() );
	}

	ShowKeyboard();
}

//=============================================================================
// ScreenCreateProfile::OnKeyboardOK
//=============================================================================

void ScreenCreateProfile::OnKeyboardInput( const char* pString )
{
	SaveNewProfile(pString);
}

//=============================================================================
// ScreenCreateProfile::OnKeyboardCancel
//=============================================================================

void ScreenCreateProfile::OnKeyboardCancel( void )
{
	EndScreen(0);
}

//=============================================================================
// ScreenCreateProfile::OnScreenRefresh
//=============================================================================

void ScreenCreateProfile::OnScreenRefresh()
{
	ScreenKeyboard* pKeyboard = GetKeyboard();

	if( pKeyboard )
		SetText( ID_CREATEPROFILE_BIGPROFILE, pKeyboard->GetString() );

	Screen* pActiveScreen = g_FrontEnd.GetActiveScreen();

	if( pActiveScreen == GetScreen( "SCREEN_ID_MEMCARD" ) )
		ShowSprite( ID_CREATEPROFILE_BIGPROFILE, false );
	else	
		ShowSprite( ID_CREATEPROFILE_BIGPROFILE, true );

	ScreenBaseClass::OnScreenRefresh();
}

//=============================================================================
// ScreenCreateProfile::OnSaveGameResult
//=============================================================================

void ScreenCreateProfile::OnSaveGameResult( int Result )
{
	ScreenBaseClass::OnSaveGameResult( Result );

	switch( Result )
	{
	case PR_CONTINUE_WITHOUT_SAVING:
		GotoScreen( SCREEN_MAINMENU );
		break;

	default:
		GotoPrevScreen();
		break;
	}

	return;
}