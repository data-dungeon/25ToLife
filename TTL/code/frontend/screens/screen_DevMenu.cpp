#include "TTLPCH.h"
#include "frontend/screens/screen_DevMenu.h"
#include "data/screens/mainmenu.h"
#include "main/PlayerDB.h"
#include "environ/ConfigMgr.h"

//=============================================================================
// ScreenDevMenu::ScreenDevMenu
//=============================================================================

ScreenDevMenu::ScreenDevMenu( void )
{
}

//=============================================================================
// ScreenDevMenu::~ScreenDevMenu - The destructor
//=============================================================================

ScreenDevMenu::~ScreenDevMenu( void )
{
}

//=============================================================================
// ScreenDevMenu::OnInitialize
//=============================================================================

void ScreenDevMenu::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	UseHighlighting( true );

	SetInitialButton( ID_DEVMENU_LEVELSELECT );
	SetNextButton( ID_DEVMENU_CHARSELECT );
}

//=============================================================================
// ScreenDevMenu::OnScreenIntro
//=============================================================================

void ScreenDevMenu::OnScreenIntro( void )
{
	m_GameEnv = g_configmgr.GetOptionIndexByName( "gameMap" );
	SetLevelName( g_configmgr.GetDisplayValue( m_GameEnv ) );

	//if( !g_gameEnvironment.Valid() ||
	//		((g_gameEnvironment.GetType() & CGameEnv::TYPE_HIDDEN) != 0))
	//{
	//	m_GameEnv = CGameEnv::GetByType(0, CGameEnv::TYPE_NORMALSP);
	//}
	//else
	//	m_GameEnv = g_gameEnvironment.Get();

	//if( CGameEnv::Valid(m_GameEnv) )
	//	SetLevelName( g_gameEnvironment.GetWorldDisplayName( m_GameEnv ) );

	ScreenBaseClass::OnScreenIntro();

	SetCharacterName( "character", g_playerProfile.GetAnyCharacter() );
}

//=============================================================================
// ScreenDevMenu::OnCursorLeft
//=============================================================================

void ScreenDevMenu::OnCursorLeft( void )
{
	Sprite* pCursor = GetSprite( ID_DEVMENU_LCURSOR );

	if( pCursor )
	{
		pCursor->InitAnim();

		SpriteAnim &anim = *pCursor->m_anim;

		anim.offset.x.SetFrame( 0.000f,-16.0f );
		anim.offset.x.SetFrame( 0.150f, 0.0f );
		anim.offset.x.Deccelerate( true );
		anim.offset.x.Play( false );

		anim.scale.x.SetFrame( 0.000f, 2.0f );
		anim.scale.x.SetFrame( 0.150f, 1.0f );
		anim.scale.x.Deccelerate( true );
		anim.scale.x.Play( false );

		anim.scale.y.SetFrame( 0.000f, 2.0f );
		anim.scale.y.SetFrame( 0.150f, 1.0f );
		anim.scale.y.Deccelerate( true );
		anim.scale.y.Play( false );
	}
	
	OnChangeOption( GetCurrentButtonId(),-1);
}

//=============================================================================
// ScreenDevMenu::OnCursorRight
//=============================================================================

void ScreenDevMenu::OnCursorRight( void )
{
	Sprite* pCursor = GetSprite( ID_DEVMENU_RCURSOR );

	if( pCursor )
	{
		pCursor->InitAnim();

		SpriteAnim &anim = *pCursor->m_anim;

		anim.offset.x.SetFrame( 0.000f, 16.0f );
		anim.offset.x.SetFrame( 0.150f, 0.0f );
		anim.offset.x.Deccelerate( true );
		anim.offset.x.Play( false );

		anim.scale.x.SetFrame( 0.000f, 2.0f );
		anim.scale.x.SetFrame( 0.150f, 1.0f );
		anim.scale.x.Deccelerate( true );
		anim.scale.x.Play( false );

		anim.scale.y.SetFrame( 0.000f, 2.0f );
		anim.scale.y.SetFrame( 0.150f, 1.0f );
		anim.scale.y.Deccelerate( true );
		anim.scale.y.Play( false );
	}

	OnChangeOption( GetCurrentButtonId(), 1);
}

//=============================================================================
// ScreenDevMenu::OnButtonOK
//=============================================================================

void ScreenDevMenu::OnButtonOK( void )
{
	g_configmgr.AcceptChanges();

	PlaySound( "menu_select" );
	//g_gameEnvironment.Set(m_GameEnv);
	g_messageDispatcher.SendMessageToAll( "SetupLaunch", NULL, INVALID_OBJECT_HANDLE );
}

//=============================================================================
// ScreenDevMenu::OnButtonCancel
//=============================================================================

void ScreenDevMenu::OnButtonCancel( void )
{
	g_configmgr.RejectChanges();

	// go back to the InitialLoop
	g_messageDispatcher.SendMessageToAll( "SetupBackup", NULL, INVALID_OBJECT_HANDLE );

	ScreenBaseClass::OnButtonCancel();
}

//=============================================================================
// ScreenDevMenu::OnButtonSelect
//=============================================================================

void ScreenDevMenu::OnButtonSelect( Button* pButton, Sprite* pSprite )
{
	ScreenBaseClass::OnButtonSelect( pButton, pSprite );

	if( pSprite )
	{
		Sprite* pParamName = GetSprite( ID_DEVMENU_PARAMNAME, pSprite );
		Sprite* pCursor1;
		Sprite* pCursor2;

		if( pParamName )
		{
			float x1, y1;
			float x2, y2;
			float x3, y3;
			float x4, y4;

			pParamName->GetSpriteRect( x1, y1, x2, y2 );

			x3 = x1 - 8.0f;
			y3 = y1 + (y2 - y1) * 0.5f;

			x4 = x2 + 8.0f;
			y4 = y3;

			pCursor1 = GetSprite( ID_DEVMENU_LCURSOR );

			if( pCursor1 )
				pCursor1->SetScreenPosition( x3, y3 );

			pCursor2 = GetSprite( ID_DEVMENU_RCURSOR );

			if( pCursor2 )
				pCursor2->SetScreenPosition( x4, y4 );
		}
	}

	return;
}

//=============================================================================
// ScreenDevMenu::OnButtonUnselect
//=============================================================================

void ScreenDevMenu::OnButtonUnselect( Button* pButton, Sprite* pSprite )
{
	ScreenBaseClass::OnButtonUnselect( pButton, pSprite );
}

//=============================================================================
// ScreenDevMenu::OnChangeOption
//=============================================================================

void ScreenDevMenu::OnChangeOption( int Option, int N )
{
	switch( Option )
	{
	case ID_DEVMENU_LEVELSELECT:
		OnChangeLevel(N);
		break;
	case ID_DEVMENU_CHARSELECT:
		OnChangeCharacter(N);
		break;
	}

	return;
}

//=============================================================================
// ScreenDevMenu::OnChangeLevel
//=============================================================================

void ScreenDevMenu::OnChangeLevel( int N )
{
	if ( N < 0 )
	{
		g_configmgr.Decrement( m_GameEnv );
	}
	else
	{
		g_configmgr.Increment( m_GameEnv );
	}

	SetLevelName( g_configmgr.GetDisplayValue( m_GameEnv ) );

	//if (N < 0)
	//	m_GameEnv = CGameEnv::GetPreviousByType(m_GameEnv, CGameEnv::TYPE_ALL);
	//else
	//	m_GameEnv = CGameEnv::GetNextByType(m_GameEnv, CGameEnv::TYPE_ALL);

	//ASSERT(CGameEnv::Valid(m_GameEnv));

	//SetLevelName( g_gameEnvironment.GetWorldDisplayName( m_GameEnv ) );

	if( N < 0 )
		PlaySoundEvent( Screen::SOUNDEVENT_PREVBUTTON );
	else
		PlaySoundEvent( Screen::SOUNDEVENT_NEXTBUTTON );

	return;
}

//=============================================================================
// ScreenDevMenu::OnChangeCharacter
//=============================================================================

void ScreenDevMenu::OnChangeCharacter( int N )
{
	// change the character
	int character = g_playerProfile.GetAnyCharacter();
	character = PlayerDB::ChangeCharacter( character, N, PLAYERDB_COP | PLAYERDB_GANG );
	g_playerProfile.SetAllCharacters( character );

	SetCharacterName( "character", character );

	if( N < 0 )
		PlaySoundEvent( Screen::SOUNDEVENT_PREVBUTTON );
	else
		PlaySoundEvent( Screen::SOUNDEVENT_NEXTBUTTON );

	return;
}

//=============================================================================
// ScreenDevMenu::SetLevelName
//=============================================================================

void ScreenDevMenu::SetLevelName( const char* pLevelName )
{
	Text* pText = GetLevelNameText();

	if( pText )
		pText->SetText( pLevelName );

	return;
}

//=============================================================================
// ScreenDevMenu::SetCharacterName
//=============================================================================

void ScreenDevMenu::SetCharacterName( const char* pString, int CharacterId )
{
	Text* pText = GetCharacterNameText();

	if( pText )
		pText->Printf( "%s %d (%s)", pString, CharacterId, PlayerDB::CharacterDisplayName( CharacterId ) );

	return;
}

//=============================================================================
// ScreenDevMenu::GetLevelNameText
//=============================================================================

Text* ScreenDevMenu::GetLevelNameText( void )
{
	return (Text*) GetSprite( ID_DEVMENU_PARAM, GetSprite( ID_DEVMENU_LEVELSELECT) );
}

//=============================================================================
// ScreenDevMenu::GetCharacterNameText
//=============================================================================

Text* ScreenDevMenu::GetCharacterNameText( void )
{
	return (Text*) GetSprite( ID_DEVMENU_PARAM, GetSprite( ID_DEVMENU_CHARSELECT ) );
}
