#include "TTLPCH.h"
#include "frontend/frontend.h"
#include "frontend/savegame.h"
#include "frontend/inputdriver.h"
#include "frontend/components.h"
#include "frontend/texteffects.h"
#include "frontend/screens/screen_BaseClass.h"
#include "frontend/inputdriver.h"
#include "avatar/sobj.h"

#ifdef DIRECTX_PC
#include "main/TTL.h"
#define COMMAND_QUIT_OK 1
#define COMMAND_QUIT_CANCEL 2
// include these for writing the default key bindings
#include <shlobj.h>
#include "platform/keyctrl.h"
#include <direct.h>

#define SAVE_GAME_LOCATION "\\25 to life"


#endif //DIRECTX_PC

#ifdef _XBOX
#include "osi/xbox/osixbox.hpp"
#include "platform/Xbox/XboxLive.h"
#endif


//=============================================================================
// ScreenBaseClass::ScreenParams::ScreenParams
//=============================================================================

ScreenBaseClass::ScreenParams::ScreenParams( void )
{
	Default();
}

//=============================================================================
// ScreenBaseClass::ScreenParams::Default
//=============================================================================

void ScreenBaseClass::ScreenParams::Default( void )
{
	IntroTime = 0.050f;
	ExitTime  = 0.200f;
	HelpRect  = 0;
}

//=============================================================================
// ScreenBaseClass::InterfaceRecord::InterfaceRecord
//=============================================================================

ScreenBaseClass::InterfaceRecord::InterfaceRecord( void )
{
	m_pInterface = 0;
}

//=============================================================================
// ScreenBaseClass::InterfaceRecord::InterfaceRecord
//=============================================================================

ScreenBaseClass::InterfaceRecord::InterfaceRecord( SpriteInterface* pInterface )
{
	SetInterface( pInterface );
}

//=============================================================================
// ScreenBaseClass::InterfaceRecord::SetInterface
//=============================================================================

void ScreenBaseClass::InterfaceRecord::SetInterface( SpriteInterface* pInterface )
{
	m_pInterface = pInterface;
}

//=============================================================================
// ScreenBaseClass::InterfaceRecord::GetInterface
//=============================================================================

SpriteInterface* ScreenBaseClass::InterfaceRecord::GetInterface( void )
{
	return m_pInterface;
}

//=============================================================================
// ScreenBaseClass::InterfaceRecord::GetInterface
//=============================================================================

SpriteInterface* ScreenBaseClass::InterfaceRecord::GetInterface( const char* pInterfaceType )
{
	SpriteInterface* pInterface = GetInterface();

	if( pInterface && 0 == xstrcmp( pInterface->Classname(), pInterfaceType ) )
		return pInterface;

	return 0;
}

ts_SaveProfile ScreenBaseClass::m_LastProfileSaved;

//=============================================================================
// ScreenBaseClass::ScreenBaseClass
//
//		The default constructor
//=============================================================================

ScreenBaseClass::ScreenBaseClass( void )
{
	m_pHighlightInfo = 0;
	m_HighlightCount = 0;

	m_hlRed   = 1.8f;
	m_hlGreen = 1.8f;
	m_hlBlue  = 1.8f;
	m_hlAlpha = 1.8f;

	m_UseHighlighting = false;
	m_ScaleOnIntro = true;

	m_SaveGameState = SAVEGAME_IDLE;
}

//=============================================================================
// ScreenBaseClass::~ScreenBaseClass
//
//		The default destructor
//=============================================================================

ScreenBaseClass::~ScreenBaseClass( void )
{
	if( m_pHighlightInfo )
		delete [] m_pHighlightInfo;

	while( m_InterfaceRecords.Child() )
		delete m_InterfaceRecords.Child();

	return;
}

//=============================================================================
// ScreenBaseClass::UpdateInterfaces
//=============================================================================

void ScreenBaseClass::UpdateInterfaces( void )
{
	InterfaceRecord* pInterfaceRecord = m_InterfaceRecords.Child();

	while( pInterfaceRecord )
	{
		//pInterfaceRecord->OnUpdate();
		pInterfaceRecord = pInterfaceRecord->Next();
	}

	return;
}

//=============================================================================
// ScreenBaseClass::RegisterInterface
//=============================================================================

bool ScreenBaseClass::RegisterInterface( SpriteInterface* pInterface )
{
	InterfaceRecord* pInterfaceRecord = new InterfaceRecord( pInterface );

	if( !pInterfaceRecord )
		return false;

	m_InterfaceRecords.AddChild( pInterfaceRecord );

	return true;
}

//=============================================================================
// ScreenBaseClass::AddInterface
//=============================================================================

SpriteInterface* ScreenBaseClass::AddInterface( int SpriteId, const char* pInterfaceType )
{
	SpriteInterface* pInterface = ScreenHandler::AddInterface( SpriteId, pInterfaceType );

	if( pInterface )
		RegisterInterface( pInterface );

	return pInterface;
}

//=============================================================================
// ScreenBaseClass::CreateListBox
//=============================================================================

GuiListBox* ScreenBaseClass::CreateListBox( int SpriteId )
{
	return (GuiListBox*) AddInterface( SpriteId, GuiListBox::ClassnameStatic() );
}

//=============================================================================
// ScreenBaseClass::CreateScrollBar
//=============================================================================

GuiScrollBar* ScreenBaseClass::CreateScrollBar( int SpriteId )
{
	return (GuiScrollBar*) AddInterface( SpriteId, GuiScrollBar::ClassnameStatic() );
}

//=============================================================================
// ScreenBaseClass::CreateSlider
//=============================================================================

GuiSlider* ScreenBaseClass::CreateSlider( int SpriteId )
{
	return (GuiSlider*) AddInterface( SpriteId, GuiSlider::ClassnameStatic() );
}

//=============================================================================
// ScreenBaseClass::CreateProgressBar
//=============================================================================

GuiProgressBar* ScreenBaseClass::CreateProgressBar( int SpriteId )
{
	return (GuiProgressBar*) AddInterface( SpriteId, GuiProgressBar::ClassnameStatic() );
}

//=============================================================================
// ScreenBaseClass::CreateTable
//=============================================================================

GuiTable* ScreenBaseClass::CreateTable( int SpriteId )
{
	return (GuiTable*) AddInterface( SpriteId, GuiTable::ClassnameStatic() );
}

//=============================================================================
// ScreenBaseClass::CreateCycleButton
//=============================================================================

GuiCycleButton* ScreenBaseClass::CreateCycleButton( int TextId )
{
	GuiCycleButton* pButton = (GuiCycleButton*) AddInterface( TextId, GuiCycleButton::ClassnameStatic() );

	if( pButton )
	{
		Text* pDisplayItem = GetTextSprite( TextId );

		if( pDisplayItem && 0 == strcmp( pDisplayItem->Classname(), Text::ClassnameStatic() ) )
			pButton->SetDisplayItem( pDisplayItem );
	}

	return pButton;
}

//=============================================================================
// ScreenBaseClass::CreateScrollingText
//=============================================================================

GuiScrollingText* ScreenBaseClass::CreateScrollingText( int SpriteId )
{
	return (GuiScrollingText*) AddInterface( SpriteId, GuiScrollingText::ClassnameStatic() );
}

//=============================================================================
// ScreenBaseClass::GetInterfaceRecord
//=============================================================================

ScreenBaseClass::InterfaceRecord* ScreenBaseClass::GetInterfaceRecord( const char* pInterfaceType )
{
	InterfaceRecord* pInterfaceRecord = m_InterfaceRecords.Child();

	while( pInterfaceRecord )
	{
		if( pInterfaceRecord->GetInterface( pInterfaceType ) )
			break;

		pInterfaceRecord = pInterfaceRecord->Next();
	}

	return pInterfaceRecord;
}

//=============================================================================
// ScreenBaseClass::GetInterface
//=============================================================================

SpriteInterface* ScreenBaseClass::GetInterface( const char* pInterfaceType )
{
	InterfaceRecord* pInterfaceRecord = GetInterfaceRecord( pInterfaceType );

	if( !pInterfaceRecord )
		return 0;

	return pInterfaceRecord->GetInterface();
}


//=============================================================================
// ScreenBaseClass::GetInterface
//=============================================================================

SpriteInterface* ScreenBaseClass::GetInterface( int SpriteId, const char* pInterfaceType )
{
	return ScreenHandler::GetInterface( SpriteId, pInterfaceType );
}

//=============================================================================
// ScreenBaseClass::GetScrollBar
//=============================================================================

GuiScrollBar* ScreenBaseClass::GetScrollBar( void )
{
	return (GuiScrollBar*) GetInterface( GuiScrollBar::ClassnameStatic() );
}

//=============================================================================
// ScreenBaseClass::GetListBox
//=============================================================================

GuiListBox* ScreenBaseClass::GetListBox( void )
{
	return (GuiListBox*) GetInterface( GuiListBox::ClassnameStatic() );
}

//=============================================================================
// ScreenBaseClass::GetTable
//=============================================================================

GuiTable* ScreenBaseClass::GetTable( void )
{
	return (GuiTable*) GetInterface( GuiTable::ClassnameStatic() );
}

//=============================================================================
// ScreenBaseClass::GetScrollingText
//=============================================================================

GuiScrollingText* ScreenBaseClass::GetScrollingText( void )
{
	return (GuiScrollingText*) GetInterface( GuiScrollingText::ClassnameStatic() );
}

//=============================================================================
// ScreenBaseClass::ShowScreenIntro
//=============================================================================

void ScreenBaseClass::ShowScreenIntro( void )
{
	//	StartTextEffects();

	Sprite* pScreenSprite = GetScreenSprite();

	if( pScreenSprite )
	{
		pScreenSprite->InitAnim();

		SpriteAnim& anim = *pScreenSprite->m_anim;

		float alpha = float(pScreenSprite->m_state.familycolor.a) / 255.0f;

		anim.familycolor.a.SetFrame( 0.00f, alpha );
		anim.familycolor.a.SetFrame( 0.25f, 1.0f );
		anim.familycolor.a.Deccelerate( true );
		anim.familycolor.a.Play( false );

		if( m_ScaleOnIntro )
		{
			anim.scale.x.SetFrame( 0.00f, 1.050f );
			anim.scale.x.SetFrame( 0.35f, 1.000f );
			anim.scale.x.Deccelerate( true );
			anim.scale.x.Play( false );

			anim.scale.y.SetFrame( 0.00f, 1.050f );
			anim.scale.y.SetFrame( 0.35f, 1.000f );
			anim.scale.y.Deccelerate( true );
			anim.scale.y.Play( false );
		}
	}

	ScreenManager* pScreenManager = GetScreenManager();

	if( pScreenManager && pScreenManager->GetCurrentScreen() == m_pScreen )
		PlaySoundEvent( Screen::SOUNDEVENT_SHOWSCREEN );
	else
		PlaySoundEvent( Screen::SOUNDEVENT_SHOWPOPUP );

	return;
}

//=============================================================================
// ScreenBaseClass::ShowScreenExit
//=============================================================================

void ScreenBaseClass::ShowScreenExit( void )
{
	Sprite* pScreenSprite = GetScreenSprite();

	if( pScreenSprite )
	{
		//================================================-======================
		// the screen manager automatically hides a screen when it goes away,
		// but we want it to fade out so show the sprite and start the fade
		//================================================-======================

		pScreenSprite->Show();
		pScreenSprite->InitAnim();

		SpriteAnim& anim = *pScreenSprite->m_anim;

		float alpha = float(pScreenSprite->m_state.familycolor.a) / 255.0f;

		anim.familycolor.a.SetFrame( 0.0f, alpha );
		anim.familycolor.a.SetFrame( GetScreenExitTime(), 0.0f );
		anim.familycolor.a.Deccelerate( true );
		anim.familycolor.a.Play( false );
	}		

	return;
}

//=============================================================================
// ScreenBaseClass::UseHighlighting
//=============================================================================

void ScreenBaseClass::UseHighlighting( bool TrueOrFalse )
{
	m_UseHighlighting = TrueOrFalse;
}

//=============================================================================
// ScreenBaseClass::SetHighlightPercentage
//=============================================================================

void ScreenBaseClass::SetHighlightPercentage( float r, float g, float b, float a )
{
	m_hlRed   = r;
	m_hlGreen = g;
	m_hlBlue  = b;
	m_hlAlpha = a;
}

//=============================================================================
// ScreenBaseClass::SetHighlightCount
//=============================================================================

bool ScreenBaseClass::SetHighlightCount( int n )
{
	if( m_HighlightCount < n )
	{
		if( m_pHighlightInfo )
			delete [] m_pHighlightInfo;

		m_pHighlightInfo = 0;
	}

	if( !m_pHighlightInfo )
	{
		m_pHighlightInfo = new HighlightInfo[n];

		if( !m_pHighlightInfo )
			return false;

		m_HighlightCount = n;
	}

	return true;
}

//=============================================================================
// ScreenBaseClass::InitTextEffects
//=============================================================================

void ScreenBaseClass::InitTextEffects( void )
{
	TextFadeEffect* pFadeEffect;
	Sprite* pScreenSprite;
	Sprite* pSprite;
	Text* pText;

	pScreenSprite = GetScreenSprite();

	if( pScreenSprite )
	{
		pSprite = pScreenSprite->Child();

		while( pSprite )
		{
			if( 0 == strcmp( pSprite->Classname(), Text::ClassnameStatic() ) )
			{
				pText = (Text*) pSprite;
				
				pFadeEffect = (TextFadeEffect*) pText->GetTextEffect( TextFadeEffect::ClassnameStatic() );

				if( !pFadeEffect )
					pText->SetTextEffect( new TextFadeEffect );
			}

			if( pSprite->Child() )
				pSprite = pSprite->Child();
			else
			if( pSprite->Next() )
				pSprite = pSprite->Next();
			else
			{
				pSprite = pSprite->Parent();

				while( pSprite && !pSprite->Next() )
				{
					pSprite = pSprite->Parent();

					if( pSprite == pScreenSprite )
						pSprite = 0;
				}

				if( pSprite )
					pSprite = pSprite->Next();
			}
		}
	}

	/*
	Button* pButton = GetInitialButton();
	TextFadeEffect *pFadeEffect;
	Text* pText;

	while( pButton )
	{
		pText = (Text*) pButton->SpriteHost();

		if( pText && 0== strcmp(pText->Classname(), Text::ClassnameStatic() ) )
		{
			pFadeEffect = new TextFadeEffect;

			if( pFadeEffect )
			{
				pText->SetTextEffect( pFadeEffect );
				pText->Tesselate(1);
			}
		}

		pButton = pButton->GetNextButton();
	}
	*/

	return;
}

//=============================================================================
// ScreenBaseClass::StartTextEffects
//=============================================================================

void ScreenBaseClass::StartTextEffects( void )
{
	TextEffect* pTextEffect;
	Sprite* pScreenSprite;
	Sprite* pSprite;
	Text* pText;
	float T = 0.0f;

	pScreenSprite = GetScreenSprite();

	if( pScreenSprite )
	{
		pSprite = pScreenSprite->Child();

		while( pSprite )
		{
			if( 0 == strcmp( pSprite->Classname(), Text::ClassnameStatic() ) )
			{
				pText = (Text*) pSprite;
				
				pTextEffect = pText->GetFirstEffect();

				while( pTextEffect )
				{
					pTextEffect->Start();
					pTextEffect->SetT(T);
					pTextEffect = pTextEffect->Next();

					T = T - 0.01f;
				}
			}

			if( pSprite->Child() )
				pSprite = pSprite->Child();
			else
			if( pSprite->Next() )
				pSprite = pSprite->Next();
			else
			{
				pSprite = pSprite->Parent();

				while( pSprite && !pSprite->Next() )
				{
					pSprite = pSprite->Parent();

					if( pSprite == pScreenSprite )
						pSprite = 0;
				}

				if( pSprite )
					pSprite = pSprite->Next();
			}
		}
	}

	return;
}

//=============================================================================
// ScreenBaseClass::GetKeyboard
//
//		Returns a pointer to the virtual keyboard.
//=============================================================================

ScreenKeyboard* ScreenBaseClass::GetKeyboard( void )
{
	return g_FrontEnd.GetKeyboard();
}

//=============================================================================
// ScreenBaseClass::GetKeyboardString
//
//		Returns the string typed into the virtual keyboard.
//=============================================================================

const char* ScreenBaseClass::GetKeyboardString( void )
{
	ScreenKeyboard* pKeyboard = GetKeyboard();

	if( pKeyboard )
		return pKeyboard->GetString();

	return 0;
}

//=============================================================================
// ScreenBaseClass::ShowKeyboard
//=============================================================================

void ScreenBaseClass::ShowKeyboard( void )
{
	g_FrontEnd.ShowKeyboard();
}

//=============================================================================
// ScreenBaseClass::GetPopupDialog
//=============================================================================

ScreenPopupDialog* ScreenBaseClass::GetPopupDialog( void )
{
	return g_FrontEnd.GetPopupDialog();
}

//=============================================================================
// ScreenBaseClass::ShowPopupDialog
//=============================================================================

void ScreenBaseClass::ShowPopupDialog( void )
{
	ShowPopup( "SCREEN_ID_POPUPDIALOG" );
}

//=============================================================================
// ScreenBaseClass::SaveProfile
//
//		Call this function when you want to save the profile.
//=============================================================================

void ScreenBaseClass::SaveProfile( void )
{
	if( m_SaveGameState != SAVEGAME_IDLE )
		return;

	//==========================================================================
	// Copy the profile name to the Global Profile thru
	//
	//		CSaveGame::SetProfileName( const char* pProfileName );
	//==========================================================================

	CSaveGame::SetProfileName(g_SaveGameParams.ProfileName);

	//==========================================================================
	// Put together a file name for this profile.
	//==========================================================================

	char FileName[64];
	xstrncpyz( FileName, SAVEGAME_PREFIX, 64 );
	xstrncatz( FileName, CSaveGame::GetProfileName(), 64 );

	//==========================================================================
	// Set the filename extension
	//==========================================================================

	g_MemCardManager->SetExt(SAVEGAME_EXT);

	//==========================================================================
	// Copy custom characters from the PlayerDB
	//==========================================================================

	g_GlobalProfile.SaveCustomChars();

	//==========================================================================
	// Copy game locks from GameLock system.
	//==========================================================================

	g_GlobalProfile.SaveGameLocks();

	//==========================================================================
	// Save out game settings (music, sound, controls etc....)
	//==========================================================================

	CGameSettings::SaveSettings();

	bool ProfileChanged = false;

	if( memcmp( &g_GlobalProfile, &m_LastProfileSaved, sizeof(ts_SaveProfile) ) )
		ProfileChanged = true;

	if( ProfileChanged && !g_MemCardManager->GetNoSave() )
	{
		m_SaveGameState = SAVEGAME_SAVE;

		char* pBuffer = (char*) &g_GlobalProfile;
		int BufferSize = sizeof(g_GlobalProfile);

		g_MemCardManager->SetUsePopups(true);
		g_MemCardManager->SetOverwriteConfirm(true);
		g_MemCardManager->SetAreYouReallySure(false);
		g_MemCardManager->SetUseActualFileSize(false);
		g_MemCardManager->SaveFile(SAVEGAME_FILENAME, FileName, 0, pBuffer, BufferSize, true, MC_SAVED_SETTINGS);
	}
	else
	{
		OnSaveGameResult( PR_COMPLETED_OK );
	}

	return;
}

//=============================================================================
// ScreenBaseClass::SaveProfile
//=============================================================================

void ScreenBaseClass::SaveProfile( const char* pProfileName )
{
	if( xstrlen( pProfileName ) > 0 )
	{
		xstrncpyz( g_SaveGameParams.ProfileName, pProfileName, SG_MAX_PROFILE_NAME );
		SaveProfile();
	}

	return;
}


//=============================================================================
// ScreenBaseClass::SaveNewProfile
//=============================================================================

void ScreenBaseClass::SaveNewProfile( const char* pProfileName )
{

	memset(&g_GlobalProfile.SinglePlayer, 0, sizeof (ts_SinglePlayer));
	memset( &m_LastProfileSaved, 0, sizeof(m_LastProfileSaved) );
	g_GlobalProfile.ResetToDefaults();
	g_GlobalProfile.SetName( pProfileName );
	g_GlobalProfile.SetTagString( pProfileName );
	g_GlobalProfile.SetTagColor( 0,   0,   0,   0, 255 );
	g_GlobalProfile.SetTagColor( 1, 238,  42,  31, 255 );
	g_GlobalProfile.SetTagColor( 2, 255, 255, 255, 255 );
	g_secondaryObjective.Reset();
#ifdef DIRECTX_PC
	// save our key bindings out to the save game directory
	// we want to use the "my documents" directory for
	// our PC save game and settings location
	char Path[MAX_PATH];
	char myDocumentsLocation[MAX_PATH];
	HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, myDocumentsLocation);
	ASSERT(result == S_OK);
	strcat(myDocumentsLocation, SAVE_GAME_LOCATION);
	sprintf(Path, "%s\\SaveGames", myDocumentsLocation);
	mkdir(Path);
	sprintf(Path, "%s\\SaveGames\\%s", myDocumentsLocation, pProfileName);
	mkdir(Path);
	sprintf(Path, "%s\\SaveGames\\%s\\%s", myDocumentsLocation, pProfileName, "keymap.ini");

	ControllerInputDriver* controller = g_input.GetController( 0 );
	KeyControllerInputDriver* kcid = static_cast<KeyControllerInputDriver*>( controller );
	kcid->WriteDefaultButtonMap(Path);
#endif
	SaveProfile( pProfileName );
}

//=============================================================================
// ScreenBaseClass::SetHelpRect
//=============================================================================

void ScreenBaseClass::SetHelpRect( int HelpRectId )
{
	m_ScreenParams.HelpRect = HelpRectId;
#ifdef DIRECTX_PC
	GetSprite(HelpRectId)->HideAll();
#endif
}

//=============================================================================
// ScreenBaseClass::GetHelpRect
//=============================================================================

Sprite* ScreenBaseClass::GetHelpRect( void )
{
	return GetSprite(m_ScreenParams.HelpRect);
}

//=============================================================================
// ScreenBaseClass::OnInitialize
//
//		Initialize stuff thats common among all screens here...
//=============================================================================

void ScreenBaseClass::OnInitialize( void )
{
	ScreenHandler::OnInitialize();

	SetSoundEvent( Screen::SOUNDEVENT_INVALID, "menu_invalid" );
	SetSoundEvent( Screen::SOUNDEVENT_DISABLED, "menu_disabled" );
	SetSoundEvent( Screen::SOUNDEVENT_SELECT, "menu_select" );
	SetSoundEvent( Screen::SOUNDEVENT_NEXTBUTTON, "menu_down" );
	SetSoundEvent( Screen::SOUNDEVENT_PREVBUTTON, "menu_up" );
	SetSoundEvent( Screen::SOUNDEVENT_SHOWSCREEN, "screen_show" );
	SetSoundEvent( Screen::SOUNDEVENT_PREVSCREEN, "screen_back" );
	SetSoundEvent( Screen::SOUNDEVENT_SHOWPOPUP, "popup_show" );
	SetSoundEvent( Screen::SOUNDEVENT_HIDEPOPUP, "popup_hide" );

	m_KeyboardKeyPressedHandler.Initialize ( "KeyboardKeyPressed", MESSAGE_HANDLE_IF_MASTER, m_MessageOwner, this );
	m_KeyboardInputHandler.Initialize      ( "KeyboardInput",      MESSAGE_HANDLE_IF_MASTER, m_MessageOwner, this );
	m_KeyboardCancelHandler.Initialize     ( "KeyboardCancel",     MESSAGE_HANDLE_IF_MASTER, m_MessageOwner, this );
	m_DialogCommandHandler.Initialize      ( "DialogCommand",      MESSAGE_HANDLE_IF_MASTER, m_MessageOwner, this );
	m_DialogCancelHandler.Initialize       ( "DialogCancel",       MESSAGE_HANDLE_IF_MASTER, m_MessageOwner, this );

	//==========================================================================
	// The base class does some animation for screen intro/screen exit, so the
	// animator must be initialized ahead of time to prevent fragmentation
	// issues.
	//==========================================================================

	Sprite* pScreenSprite = GetScreenSprite();

	if( pScreenSprite )
	{
		pScreenSprite->InitAnim();
		pScreenSprite->m_state.familycolor.a = 0;
	}

	#ifdef DIRECTX_PC
	// enable our buttons that will be on screen all the time
	StartButton.AddInterface( GuiCycleButton::ClassnameStatic(), 0 );
	StartButton.SetFont("GazzRegularB");
	StartButton.SetText("Start");
	StartButton.SetPosition(100, 420);
	StartButton.SetColor(150,150,150,255);
	StartButton.SetPriority(-1);
	StartButton.Show(false);

	BackButton.AddInterface( GuiCycleButton::ClassnameStatic(), 0 );
	BackButton.SetFont("GazzRegularB");
	BackButton.SetText("Back");
	BackButton.SetPosition(480, 420);
	BackButton.SetColor(150,150,150,255);
	BackButton.SetPriority(-1);
	//BackButton.Show(false);

	QuitButton.AddInterface( GuiCycleButton::ClassnameStatic(), 0 );
	QuitButton.SetFont("GazzRegularB");
	QuitButton.SetText("Quit");
	QuitButton.SetColor(150,150,150,255);
	QuitButton.SetPosition(550, 420);
	QuitButton.SetPriority(-1);
	#endif
}

//=============================================================================
// ScreenBaseClass::OnScreenIntro
//=============================================================================

void ScreenBaseClass::OnScreenIntro( void )
{
	if( !GetCurrentButton() )
		SelectInitialButton();

	ShowScreenIntro();
#ifdef DIRECTX_PC
	QuitButton.Show(true);
	BackButton.Show(true);
#endif
}


//=============================================================================
// ScreenBaseClass::OnScreenUpdate()
//=============================================================================
void ScreenBaseClass::OnScreenUpdate()
{
#ifdef DIRECTX_PC
	// draw our start and back buttons, and test
	// to see if our mouse has interacted with them
	float x1, y1, x2, y2;
	float x = g_MenuCursor.GetX();
	float y = g_MenuCursor.GetY();
	if (StartButton.IsVisible())
	{
		StartButton.Update();
		if ( g_MenuCursor.CursorButtonPressed(0) )
		{
			StartButton.GetFamilyRect(x1, y1, x2, y2);
			if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) )
				OnButtonOK();
		}
	}
	if (BackButton.IsVisible())
	{
		BackButton.Update();
		if ( g_MenuCursor.CursorButtonPressed(0) )
		{
			BackButton.GetFamilyRect(x1, y1, x2, y2);
			if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) )
				OnButtonCancel();
		}
	}
	if (QuitButton.IsVisible())
	{
		QuitButton.Update();
		if ( g_MenuCursor.CursorButtonPressed(0) )
		{
			QuitButton.GetFamilyRect(x1, y1, x2, y2);
			if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) )
			{
				// do popup stuff here
				ScreenPopupDialog* pDialog = GetPopupDialog();
				if( pDialog )
				{
					pDialog->Clear();
					pDialog->SetTitle( "Quit" );
					pDialog->SetMessage( "Are you sure you want to Quit?" );
					pDialog->AddMenuItem( "Quit", COMMAND_QUIT_OK, 0 );
					pDialog->AddMenuItem( "Cancel", COMMAND_QUIT_CANCEL, 0 );
					pDialog->SelectMenuItem( COMMAND_QUIT_CANCEL );
				}
				ShowPopupDialog();
			}
				
		}
	}
	Sprite* pHelpRect = GetHelpRect();
	if (pHelpRect)
		pHelpRect->HideAll();
#endif

	GuiScrollBar* pScrollBar = GetScrollBar();

	if( pScrollBar )
		OnScrollBarUpdate( pScrollBar );

	UpdateInterfaces();
	OnSaveGameUpdate();

	// call our parent's update function
	ScreenHandler::OnScreenUpdate();
}

//=============================================================================
// ScreenBaseClass::OnScreenExit
//=============================================================================

void ScreenBaseClass::OnScreenExit( void )
{
#ifdef DIRECTX_PC
	// we'll handle getting rid of the buttons, but you have to show them
	StartButton.Show(false);
	StartButton.Update();

	BackButton.Show(false);
	BackButton.Update();

	QuitButton.Show(false);
	QuitButton.Update();
#endif

	ShowScreenExit();
}

//=============================================================================
// ScreenBaseClass::GetScreenIntroTime
//
//		This function returns the minimum amount of time the screen needs to
//		display its intro animation.  The screen manager will wait at least this
//		long before accepting input.
//=============================================================================

float ScreenBaseClass::GetScreenIntroTime( void )
{
	return m_ScreenParams.IntroTime;
}

//=============================================================================
// ScreenBaseClass::GetScreenExitTime
//
//		This function returns the minimum amount of time the screen needs to
//		display its exit animation.  The screen manager will wait at least this
//		long before it attepts to display the next screen.
//=============================================================================

float ScreenBaseClass::GetScreenExitTime( void )
{
	return m_ScreenParams.ExitTime;
}

//=============================================================================
// ScreenBaseClass::OnButtonPressed
//=============================================================================

void ScreenBaseClass::OnButtonPressed( int Button )
{
#ifdef _XBOX
	if( Button == FluffControlMapper::SELECT )
		OnButtonCancel();
#endif
}

//=============================================================================
// ScreenBaseClass::OnButtonReleased
//=============================================================================

void ScreenBaseClass::OnButtonReleased( int Button )
{
}

//=============================================================================
// ScreenBaseClass::OnButtonSelect
//=============================================================================

void ScreenBaseClass::OnButtonSelect( Button* pButton, Sprite* pSprite )
{
	if( m_UseHighlighting )
	{
		if( pSprite )
		{
			int n = pSprite->GetNumChildren()+1;

			if( SetHighlightCount(n) )
			{
				float x, y;
				float r;
				float g;
				float b;
				float a;
				float sx;
				float sy;

				pSprite->GetPosition( x, y );
				SetCursorPos( x, y );

				pSprite->SetOffset( 4.0f, 0.0f );

				for( int i = 0; i < n; i++ )
				{
					if( pSprite )
					{
						pSprite->GetBaseColor( r, g, b, a );
						pSprite->GetScale( sx, sy );

						m_pHighlightInfo[i].m_Red   = r;
						m_pHighlightInfo[i].m_Green = g;
						m_pHighlightInfo[i].m_Blue  = b;
						m_pHighlightInfo[i].m_Alpha = a;
						m_pHighlightInfo[i].m_SpriteId = pSprite->ObjectId();
						m_pHighlightInfo[i].m_ScaleX = sx;
						m_pHighlightInfo[i].m_ScaleY = sy;
						
						r = Math::Clamp( r * m_hlRed );
						g = Math::Clamp( g * m_hlGreen );
						b = Math::Clamp( b * m_hlBlue );
						a = Math::Clamp( a * m_hlAlpha );

						pSprite->SetBaseColor( r, g, b, a );

						if( i == 0 )
							pSprite->SetScale( sx * 1.075f, sy * 1.075f );

						if( i == 0 )
							pSprite = pSprite->Child();
						else
							pSprite = pSprite->Next();
					}
				}
			}
		}
	}

	return;
}

//=============================================================================
// ScreenBaseClass::OnButtonUnselect
//=============================================================================

void ScreenBaseClass::OnButtonUnselect( Button* pButton, Sprite* pSprite )
{
	if( m_UseHighlighting && m_pHighlightInfo )
	{
		int n = m_HighlightCount;
		int i;

		HighlightInfo* hi = m_pHighlightInfo;

		if( pSprite )
			pSprite->SetOffset( 0.0f, 0.0f );

		for( i = 0; i < n; i++ )
		{
			if( pSprite && pSprite->ObjectId() == hi[i].m_SpriteId )
			{
				pSprite->SetBaseColor( hi[i].m_Red,
											  hi[i].m_Green,
											  hi[i].m_Blue,
											  hi[i].m_Alpha );

				if( i == 0 )
					pSprite->SetScale( hi[i].m_ScaleX, hi[i].m_ScaleY );

				if( i == 0 )
					pSprite = pSprite->Child();
				else
					pSprite = pSprite->Next();
			}
		}
	}

	return;
}

//=============================================================================
// ScreenBaseClass::OnPopupShow
//
//		This function is called whenever a popup is displayed over the screen.
//		Check if there is a help rect and dim all the buttons.
//=============================================================================

void ScreenBaseClass::OnPopupShow( void )
{
	Sprite* pHelpRect = GetHelpRect();

	if( pHelpRect )
		pHelpRect->Hide();

	return;
}

//=============================================================================
// ScreenBaseClass::OnPopupHide
//
//		This function is called whenever a popup is removed from the screen.
//=============================================================================

void ScreenBaseClass::OnPopupHide( void )
{
#ifndef DIRECTX_PC
	Sprite* pHelpRect = GetHelpRect();

	if( pHelpRect )
		pHelpRect->Show();
#endif
	return;
}

//=============================================================================
// ScreenBaseClass::OnKeyboardKeyPressed
//
//		This function is called whenever a key from the keyboard is entered.
//=============================================================================

void ScreenBaseClass::OnKeyboardKeyPressed( int Key )
{
}

//=============================================================================
// ScreenBaseClass::OnKeyboardInput
//
//		This function is called when a string is entered from the keyboard.  This
//		event is usually triggered by pressing the ENTER key or START button.
//=============================================================================

void ScreenBaseClass::OnKeyboardInput( const char* pString )
{
}

//=============================================================================
// ScreenBaseClass::OnKeyboardCancel
//
//		This function is called when the keyboard is canceled.
//=============================================================================

void ScreenBaseClass::OnKeyboardCancel( void )
{
}

//=============================================================================
// ScreenBaseClass::HandleInput
//=============================================================================

void ScreenBaseClass::HandleInput( UIHandler* pUIHandler )
{
#ifdef _XBOX
	
	OSIXbox* osix = (OSIXbox*)OSI::getSingletonPtr();

	// Fetch input for controller.  According to Dwain, ControllerLocked is actually asking about PLAYER 0 and not CONTROLLER 0.
	if( g_input.ControllerLocked( 0 ) )
	{
		XboxControllerInputDriver* pDriver = reinterpret_cast< XboxControllerInputDriver* >( g_input.GetController(0) );
		if( pDriver && pDriver->Connected() && pDriver->GetHandle() )
		{
			XINPUT_STATE state;
			XInputGetState( pDriver->GetHandle(), &state );
			int controllerPortIndex = g_input.GetController( 0 )->GetHeadSetPort();

			if( osix && osix->isCreated() && osix->uixIsStarted() && osix->uixIsProcessingInput() )
			{
				osix->uixSetInputForControllerPort( controllerPortIndex, &state );
			}
			if( CXboxLive::InUix() )
			{
				CXboxLive::GetLiveEngine()->SetInput( controllerPortIndex, &state );
			}
		}
	}

#endif // _XBOX

	ScreenHandler::HandleInput( pUIHandler );

	int n = FluffControlMapper::MAXBUTTONS;
	bool Pressed = false;
	bool Released = false;

	for( int i = 0; i < n; i++ )
	{
		if( pUIHandler->DeviceButtonPressed( FluffControlMapper::DIGITALBUTTON(i) ) )
		{
			OnButtonPressed(i);
			Pressed = true;
		}

		if( pUIHandler->DeviceButtonReleased( FluffControlMapper::DIGITALBUTTON(i) ) )
		{
			OnButtonReleased(i);
			Released = true;
		}

		// Don't allow multiple button presses in the same frame
		if( Pressed || Released )
			break;
	}
	return;
}

//=============================================================================
// ScreenBaseClass::OnDialogCommand
//
//		This function is called when the user makes a selection in the dialog box.
//=============================================================================

void ScreenBaseClass::OnDialogCommand( int Command )
{
#ifdef DIRECTX_PC
	switch( Command )
	{
	case COMMAND_QUIT_OK:
		g_ttl->Exit();
		break;
	case COMMAND_QUIT_CANCEL:
		g_FrontEnd.RemoveActivePopup();
		break;
	}
#endif
}

//=============================================================================
// ScreenBaseClass::OnDialogCancel
//
//		This function is called when the dialog box is canceled.
//=============================================================================

void ScreenBaseClass::OnDialogCancel( void )
{

}

//=============================================================================
// ScreenBaseClass::OnSaveGameUpdate
//=============================================================================

void ScreenBaseClass::OnSaveGameUpdate( void )
{
	switch( g_MemCardManager->GetState() )
	{
	case CMemoryCard::CMEMORYCARD_IDLE:
		
		switch( m_SaveGameState )
		{
		case SAVEGAME_IDLE:
			OnSaveGameIdle();
			break;

		case SAVEGAME_SAVE:
			m_SaveGameState = SAVEGAME_IDLE;
			if( g_MemCardManager->GetPopupExitResult() == PR_COMPLETED_OK )
				memcpy( &m_LastProfileSaved, &g_GlobalProfile, sizeof(ts_SaveProfile) );
			g_MemCardManager->ResetLoadSaveBuffer();
			OnSaveGameResult( g_MemCardManager->GetPopupExitResult() );
			break;
		}
	}

	return;
}

//=============================================================================
// ScreenBaseClass::OnSaveGameIdle
//=============================================================================

void ScreenBaseClass::OnSaveGameIdle( void )
{
}

//=============================================================================
// ScreenBaseClass::OnSaveGameResult
//=============================================================================

void ScreenBaseClass::OnSaveGameResult( int Result )
{
}

//=============================================================================
// ScreenBaseClass::OnScrollBarUpdate
//=============================================================================

void ScreenBaseClass::OnScrollBarUpdate( GuiScrollBar* pScrollBar )
{
	if( pScrollBar )
	{
		Sprite* pSprite = pScrollBar->SpriteHost();
		GuiTable* pTable = GetTable();
		GuiListBox* pListBox = GetListBox();
		GuiScrollingText* pScrollingText = GetScrollingText();
		int numitems = 0;
		int maxitems = 0;
		int item = 0;

		if( pTable )
		{
			int Row, Col;
			pTable->GetCursorPos( Row, Col );
			maxitems = pTable->GetDisplayRows();
			numitems = pTable->GetRows();
			item = Row;
		}
		else
		if( pListBox )
		{
			maxitems = pListBox->GetItemDisplayCount();
			numitems = pListBox->GetItemCount();
			item = pListBox->CurrentItem();
		}
		else
		if( pScrollingText )
		{
			maxitems = pScrollingText->GetDisplayRows();
			numitems = pScrollingText->GetRows();
			item = pScrollingText->GetCursorPos();
		}

		pScrollBar->SetRange( 0.0f, float(numitems - 1) );
		pScrollBar->SetPosition( float(item) );
		pScrollBar->SetSteps( numitems );
		pScrollBar->SetPageSize( maxitems );

		if( pSprite )
		{
			if( numitems > maxitems )
				pSprite->m_state.familycolor.a = 255;
			else
				pSprite->m_state.familycolor.a = 0;
		}

	}

	return;
}

//=============================================================================
// ScreenBaseClass::KeyboardKeyPressed::HandleMessage
//
//		This function responds to keyboard key inputs immediately.
//=============================================================================

void ScreenBaseClass::KeyboardKeyPressed::HandleMessage( void* pData, ActorHandle Sender, bool Posted )
{
	if( d_ref->m_Active )
		d_ref->OnKeyboardKeyPressed( (int) pData );
}

//=============================================================================
// ScreenBaseClass::KeyboardInput::HandleMessage
//
//		This function responds to a keyboard string input when the user accepts
//		the string, usually by pressing the ENTER key or START button.
//=============================================================================

void ScreenBaseClass::KeyboardInput::HandleMessage( void* pData, ActorHandle Sender, bool Posted )
{
	if( d_ref->m_Active )
		d_ref->OnKeyboardInput( (const char*) pData );
}

//=============================================================================
// ScreenBaseClass::KeyboardCancel::HandleMessage
//
//		This function responds to the keyboard being canceled, usually by
//		pressing the ESC key (Triangle on Playstation 2, Circle on the Xbox).
//=============================================================================

void ScreenBaseClass::KeyboardCancel::HandleMessage( void* pData, ActorHandle Sender, bool Posted )
{
	if( d_ref->m_Active )
		d_ref->OnKeyboardCancel();
}

//=============================================================================
// ScreenBaseClass::DialogCommand::HandleMessage
//
//		This function responds to the dialog box sending a command, usually in
//		response to selecting a menu item.
//=============================================================================

void ScreenBaseClass::DialogCommand::HandleMessage( void* pData, ActorHandle Sender, bool Posted )
{
	if( d_ref->m_Active )
		d_ref->OnDialogCommand( (int) pData );
	
	return;
}

//=============================================================================
// ScreenBaseClass::DialogCancel::HandleMessage
//
//		This function responds to the dialog box being canceled.
//=============================================================================

void ScreenBaseClass::DialogCancel::HandleMessage( void* pData, ActorHandle Sender, bool Posted )
{
	if( d_ref->m_Active )
		d_ref->OnDialogCancel();

	return;
}
