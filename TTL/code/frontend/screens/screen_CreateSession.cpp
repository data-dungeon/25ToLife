#include "TTLPCH.h"
#include "Network/SessionSpec.h"
#include "frontend/frontend.h"
#include "frontend/screens/screen_GameSetting.h"
#include "frontend/screens/screen_CreateSession.h"
#include "frontend/screens/screen_Keyboard.h"
#include "data/screens/OnlineScreens.h"


//=============================================================================
// ScreenCreateSession::ScreenCreateSession
//
//		The default constructor
//=============================================================================

ScreenCreateSession::ScreenCreateSession( void )
{
}

//=============================================================================
// ScreenCreateSession::~ScreenCreateSession
//
//		The default destructor
//=============================================================================

ScreenCreateSession::~ScreenCreateSession( void )
{
}

//=============================================================================
// ScreenCreateSession::SetMatchName
//=============================================================================

void ScreenCreateSession::SetMatchName( const char* matchName )
{
	const size_t matchNameSize = sizeof( _matchName )/sizeof( _matchName[0] );
	strncpy( _matchName, matchName,  matchNameSize );
	_matchName[ matchNameSize-1 ] = 0;

	ScreenKeyboard* pKeyboard = GetKeyboard();

	if( pKeyboard )
	{
		if( _matchName[0] == 0 )
		{
			pKeyboard->SetString( g_playerProfile.GetAccountName() );
		}
		else
		{
			pKeyboard->SetString( _matchName );
		}
	}
}

//=============================================================================
// ScreenCreateSession::OnInitialize
//=============================================================================

void ScreenCreateSession::OnInitialize( void )
{
	_validateMatchName = false;
	_matchName[0] = 0;
	ScreenBaseClass::OnInitialize();
}

void ScreenCreateSession::OnPrevScreen( Screen* pPrevScreen )
{
	ScreenKeyboard* pKeyboard = GetKeyboard();

	if( pKeyboard )
	{
		pKeyboard->SetDefaultKey( ScreenKeyboard::CLEAR_KEY );
	}

	ScreenBaseClass::OnPrevScreen( pPrevScreen );
}

void ScreenCreateSession::OnScreenExit( void )
{
	ScreenKeyboard* pKeyboard = GetKeyboard();

	if( pKeyboard )
	{
		pKeyboard->SetDefaultKey( ScreenKeyboard::CLEAR_KEY );
	}

	ScreenBaseClass::OnScreenExit();
}

//=============================================================================
// ScreenCreateSession::OnScreenIntro
//=============================================================================

void ScreenCreateSession::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();

	ScreenKeyboard* pKeyboard = GetKeyboard();

	if( pKeyboard )
	{
		pKeyboard->SetMaxString(15);
		pKeyboard->RestrictChars( true, "\\/" );
		pKeyboard->RestrictKey( true, 165 );	// ¥
		pKeyboard->RestrictKey( true, 171 );	// «
		pKeyboard->RestrictKey( true, 187 );	// »
		pKeyboard->RestrictKey( true, 191 );	// ¿
		pKeyboard->RestrictKey( true, 253 );	// ý
		pKeyboard->RestrictKey( true, 131 );	// ƒ
		pKeyboard->RestrictKey( true, 132 );	// „

		if( _matchName[0] == 0 )
		{
			pKeyboard->SetString( g_playerProfile.GetAccountName() );
		}
		else
		{
			pKeyboard->SetString( _matchName );
		}
		pKeyboard->SetDefaultKey( ScreenKeyboard::ENTER_KEY );
	}

	ShowKeyboard();
}

//=============================================================================
// ScreenCreateSession::OnKeyboardInput
//=============================================================================

void ScreenCreateSession::OnKeyboardInput( const char* pString )
{
	if( pString && pString[0] != 0 )
	{
		strncpy( _matchName, pString, sizeof( _matchName ) / sizeof( _matchName[0] ) );
		_matchName[ (sizeof( _matchName ) / sizeof( _matchName[0] )) - 1 ] = 0;
		g_sessionSpec.SetName( _matchName );
		g_messageDispatcher.SendMessageToAll( "ValidateMatchName", NULL, INVALID_OBJECT_HANDLE );
	}
	else
	{
		ShowKeyboard();
		return;
	}

	if( !_validateMatchName )
	{
		GotoScreen( "SCREEN_ID_GAMESETTING" );
	}
}

//=============================================================================
// ScreenCreateSession::OnKeyboardCancel
//=============================================================================

void ScreenCreateSession::OnKeyboardCancel( void )
{
	_validateMatchName = false;
	EndScreen(0);

	// Message for Use Cases to catch cancelation of screen
   g_messageDispatcher.SendMessageToAll( "CancelConfigMatch", NULL, INVALID_OBJECT_HANDLE );
}

//=============================================================================
// ScreenCreateSession::OnScreenUpdate()
//=============================================================================

void ScreenCreateSession::OnScreenUpdate()
{
	ScreenKeyboard* pKeyboard = GetKeyboard();

	if( pKeyboard )
		m_pScreen->GetTextSprite( ID_CREATESESSION_BIGSESSION )->SetTextNoTranslate( pKeyboard->GetString() );

	return;
}

#ifdef DIRECTX_PC
void ScreenCreateSession::OnButtonPressed( int Button )
{
	if( Button == FluffControlMapper::TRIANGLE )
	{
		OnKeyboardCancel();
	}
	else
		ScreenBaseClass::OnButtonPressed(Button);
}
#endif