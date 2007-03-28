#include "TTLPCH.h"
#include "platform/defcontrolmap.h"
#include "platform/MemoryCard/MemoryCard.h"
#include "Display/TextureUtil.h"

#include "avatar/avcontrolmap.h"
#include "frontend/frontend.h"
#include "frontend/savegame.h"
#include "frontend/screens/screen_GamePadSettings.h"
#include "frontend/screens/screen_Popups.h"

#ifdef _XBOX
#include "data_xbox/screens/Buttonmap.h"
#else
#include "data/screens/Buttonmap.h"
#endif // _XBOX

extern	bool	g_OptionsOnScreen;

//=============================================================================
// ScreenButtonMap::ScreenButtonMap
//=============================================================================

static const DefaultControlMapper::DIGITALBUTTON DigitalButtons[] =
{
	DefaultControlMapper::BUTTON_LEFT,
	DefaultControlMapper::BUTTON_DOWN,
	DefaultControlMapper::BUTTON_RIGHT,
	DefaultControlMapper::BUTTON_UP,
	DefaultControlMapper::BUTTON_SQUARE,
	DefaultControlMapper::BUTTON_X,
	DefaultControlMapper::BUTTON_O,
	DefaultControlMapper::BUTTON_TRI,
	DefaultControlMapper::BUTTON_L1,
	DefaultControlMapper::BUTTON_R1,
	DefaultControlMapper::BUTTON_L2,
	DefaultControlMapper::BUTTON_R2,
	DefaultControlMapper::BUTTON_START,
	DefaultControlMapper::BUTTON_SELECT,
	DefaultControlMapper::BUTTON_STICK1,
	DefaultControlMapper::BUTTON_STICK2,
};

ScreenButtonMap::ScreenButtonMap( void )
{
}

//=============================================================================
// ScreenButtonMap::GetActionId
//=============================================================================

int ScreenButtonMap::GetActionId( DefaultControlMapper::DIGITALBUTTON Button )
{
	switch( Button )
	{
	case DefaultControlMapper::BUTTON_LEFT:
		return ID_BUTTONMAP_ACTIONLEFT;
	case DefaultControlMapper::BUTTON_DOWN:
		return ID_BUTTONMAP_ACTIONDOWN;
	case DefaultControlMapper::BUTTON_RIGHT:
		return ID_BUTTONMAP_ACTIONRIGHT;
	case DefaultControlMapper::BUTTON_UP:
		return ID_BUTTONMAP_ACTIONUP;
	case DefaultControlMapper::BUTTON_SQUARE:
		return ID_BUTTONMAP_ACTIONSQUARE;
	case DefaultControlMapper::BUTTON_X:
		return ID_BUTTONMAP_ACTIONX;
	case DefaultControlMapper::BUTTON_O:
		return ID_BUTTONMAP_ACTIONCIRCLE;
	case DefaultControlMapper::BUTTON_TRI:
		return ID_BUTTONMAP_ACTIONTRIANGLE;
	case DefaultControlMapper::BUTTON_L1:
		return ID_BUTTONMAP_ACTIONL1;
	case DefaultControlMapper::BUTTON_R1:
		return ID_BUTTONMAP_ACTIONR1;
	case DefaultControlMapper::BUTTON_L2:
		return ID_BUTTONMAP_ACTIONL2;
	case DefaultControlMapper::BUTTON_R2:
		return ID_BUTTONMAP_ACTIONR2;
	case DefaultControlMapper::BUTTON_START:
		return ID_BUTTONMAP_ACTIONSTART;
	case DefaultControlMapper::BUTTON_SELECT:
		return ID_BUTTONMAP_ACTIONSELECT;
	case DefaultControlMapper::BUTTON_STICK1:
		return ID_BUTTONMAP_ACTIONLEFTSTICK;
	case DefaultControlMapper::BUTTON_STICK2:
		return ID_BUTTONMAP_ACTIONRIGHTSTICK;
	}

	return 0;
}

//=============================================================================
// ScreenButtonMap::OnInitialize
//=============================================================================

void ScreenButtonMap::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	UseHighlighting( true );

	SetHelpRect( ID_BUTTONMAP_HELPRECT );
	SetInitialButton( ID_BUTTONMAP_SETTING1 );
	SetNextButton( ID_BUTTONMAP_SETTING2 );
	SetNextButton( ID_BUTTONMAP_SETTING3 );
	SetNextButton( ID_BUTTONMAP_SETTING4 );
}

//=============================================================================
// ScreenButtonMap::OnCursorUp
//=============================================================================

void ScreenButtonMap::OnCursorUp( void )
{
}

//=============================================================================
// ScreenButtonMap::OnCursorDown
//=============================================================================

void ScreenButtonMap::OnCursorDown( void )
{
}

//=============================================================================
// ScreenButtonMap::OnCursorLeft
//=============================================================================

void ScreenButtonMap::OnCursorLeft( void )
{
	ScreenBaseClass::OnCursorUp();
}

//=============================================================================
// ScreenButtonMap::OnCursorRight
//=============================================================================

void ScreenButtonMap::OnCursorRight( void )
{
	ScreenBaseClass::OnCursorDown();
}

//=============================================================================
// ScreenButtonMap::OnButtonSelect
//=============================================================================

void ScreenButtonMap::OnButtonSelect( Button* pButton, Sprite* pSprite )
{
	ScreenBaseClass::OnButtonSelect( pButton, pSprite );

	if( pButton )
	{
		int ControllerConfig = 0;

		switch( pButton->GetButtonId() )
		{
		case ID_BUTTONMAP_SETTING1:
			ControllerConfig = 0;
			break;
		case ID_BUTTONMAP_SETTING2:
			ControllerConfig = 1;
			break;
		case ID_BUTTONMAP_SETTING3:
			ControllerConfig = 2;
			break;
		case ID_BUTTONMAP_SETTING4:
			ControllerConfig = 3;
			break;
		}

		m_ActionMap.DefaultActionMap(ControllerConfig);
	}

	return;
}

//=============================================================================
// ScreenButtonMap::OnCommand
//=============================================================================

void ScreenButtonMap::OnCommand( int Command )
{
	int ButtonLayout = 0;

	switch( Command )
	{
	case ID_BUTTONMAP_SETTING1:
		ButtonLayout = AvatarControlMapper::STANDARD;
		break;

	case ID_BUTTONMAP_SETTING2:
		ButtonLayout = AvatarControlMapper::ALT1;
		break;

	case ID_BUTTONMAP_SETTING3:
		ButtonLayout = AvatarControlMapper::ALT2;
		break;

	case ID_BUTTONMAP_SETTING4:
		ButtonLayout = AvatarControlMapper::ALT3;
		break;
	}

	g_GlobalProfile.SetButtonLayout( ButtonLayout );

	// update the avatar
	AvatarControlMapper*	c = NULL;
	c = (AvatarControlMapper*) g_controlFocus.Find("Avatar");

	AvatarControlMapper::MAPPINGSCHEME AvButtonLayout =
		AvatarControlMapper::MAPPINGSCHEME(ButtonLayout );

	if (c)
		c->SetButtonLayout(AvButtonLayout);

	EndScreen(0);
}

//=============================================================================
// ScreenButtonMap::OnButtonCancel
//=============================================================================

void ScreenButtonMap::OnButtonCancel( void )
{
	g_OptionsOnScreen = false;
	PlaySoundEvent( Screen::SOUNDEVENT_PREVSCREEN );
	EndScreen(0);
}

//=============================================================================
// ScreenButtonMap::OnScreenIntro
//=============================================================================

void ScreenButtonMap::OnScreenIntro( void )
{
	g_OptionsOnScreen = true;

	ScreenBaseClass::OnScreenIntro();

	int ButtonLayout = g_GlobalProfile.GetButtonLayout();

	switch( ButtonLayout )
	{
	case AvatarControlMapper::STANDARD:
		SelectButton( ID_BUTTONMAP_SETTING1 );
		break;

	case AvatarControlMapper::ALT1:
		SelectButton( ID_BUTTONMAP_SETTING2 );
		break;

	case AvatarControlMapper::ALT2:
		SelectButton( ID_BUTTONMAP_SETTING3 );
		break;

	case AvatarControlMapper::ALT3:
		SelectButton( ID_BUTTONMAP_SETTING4 );
		break;
	}

	return;
}

//=============================================================================
// ScreenButtonMap::OnScreenUpdate
//=============================================================================

void ScreenButtonMap::OnScreenUpdate( void )
{
	ScreenBaseClass::OnScreenUpdate();

	ActionMap::ActionType Action;
	DefaultControlMapper::DIGITALBUTTON ActionButton;
	int n = sizeof(DigitalButtons) / sizeof(DigitalButtons[0]);
	int i;
	Text* pAction;

	int red;
	int green;
	int blue;
	int alpha;

	Text* pTextColor = GetTextSprite( ID_BUTTONMAP_DISABLEDCOLOR );

	if( pTextColor )
		pTextColor->GetBaseColor( red, green, blue, alpha );

	for( i = 0; i < n; i++ )
	{
		pAction = GetTextSprite( GetActionId( DigitalButtons[i] ) );

		if( pAction )
		{
			pAction->SetText( "-----" );
			pAction->SetBaseColor( red, green, blue, alpha );
		}
	}

	pTextColor = GetTextSprite( ID_BUTTONMAP_NORMALCOLOR );

	if( pTextColor )
		pTextColor->GetBaseColor( red, green, blue, alpha );

	Text* pHighlightAction = 0;

	n = m_ActionMap.MaxActions();

	for( i = 0; i < n; i++ )
	{
		Action = ActionMap::ActionType(i);
		ActionButton = m_ActionMap.GetAction( Action );

		pAction = GetTextSprite( GetActionId( ActionButton ) );

	//	if( pHandler && pHandler->ButtonDown( ActionButton ) )
	//		pHighlightAction = pAction;

		if( pAction )
		{
			pAction->SetText( m_ActionMap.GetActionText( Action ) );
			pAction->SetBaseColor( red, green, blue, alpha );
		}
	}

	if( pHighlightAction )
	{
		pTextColor = GetTextSprite( ID_BUTTONMAP_HIGHLIGHTCOLOR );

		if( pTextColor )
			pTextColor->GetBaseColor( red, green, blue, alpha );

		pHighlightAction->SetBaseColor( red, green, blue, alpha );
	}

	return;
}

//=============================================================================
// ScreenThumbSticks::HideThumbStickImages
//=============================================================================

void ScreenThumbSticks::HideThumbStickImages( void )
{
	static const int ImageList[] =
	{
		ID_THUMBSTICKS_STANDARDCONFIG,
		ID_THUMBSTICKS_SOUTHPAWCONFIG,
		ID_THUMBSTICKS_LEGACYCONFIG,
		ID_THUMBSTICKS_LEGACYSOUTHPAWCONFIG,
	};

	Sprite* pSprite;
	int n = sizeof(ImageList)/sizeof(ImageList[0]);

	for( int i = 0; i < n; i++ )
	{
		pSprite = GetSprite( ImageList[i] );

		if( pSprite )
			pSprite->m_state.familycolor.a = 0;
	}

	return;
}

//=============================================================================
// ScreenThumbSticks::OnInitialize
//=============================================================================

void ScreenThumbSticks::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	UseHighlighting( true );

	SetHelpRect( ID_THUMBSTICKS_HELPRECT );
	SetInitialButton( ID_THUMBSTICKS_STANDARD );
	SetNextButton( ID_THUMBSTICKS_SOUTHPAW );
	SetNextButton( ID_THUMBSTICKS_ASSBACKWARDS );
	SetNextButton( ID_THUMBSTICKS_ASSBACKWARDSSOUTHPAW );

	SetCursor( ID_THUMBSTICKS_CURSOR );

	HideThumbStickImages();
}

//=============================================================================
// ScreenThumbSticks::OnScreenIntro
//=============================================================================

void ScreenThumbSticks::OnScreenIntro( void )
{
	g_OptionsOnScreen = true;

	ScreenBaseClass::OnScreenIntro();

	int ThumbStickMode = g_GlobalProfile.GetThumbStickMode();

	switch( ThumbStickMode )
	{
	case AvatarControlMapper::STANDARD:
		SelectButton( ID_THUMBSTICKS_STANDARD );
		break;

	case AvatarControlMapper::ALT1:
		SelectButton( ID_THUMBSTICKS_SOUTHPAW );
		break;

	case AvatarControlMapper::ASSBACKWARDS:
		SelectButton( ID_THUMBSTICKS_ASSBACKWARDS );
		break;

	case AvatarControlMapper::ASSBACKWARDSALT1:
		SelectButton( ID_THUMBSTICKS_ASSBACKWARDSSOUTHPAW );
		break;
	}
}

//=============================================================================
// ScreenThumbSticks::OnScreenUpdate
//=============================================================================

void ScreenThumbSticks::OnScreenUpdate( void )
{
	ScreenBaseClass::OnScreenUpdate();

	HideThumbStickImages();

	int ImageId = 0;

	const char* pMoveForward  = "Move Forward";
	const char* pMoveBackward = "Move Backward";
	const char* pMoveLeft     = "Move Left";
	const char* pMoveRight    = "Move Right";

	const char* pLookUp    = "Look Up";
	const char* pLookDown  = "Look Down";
	const char* pTurnLeft  = "Turn Left";
	const char* pTurnRight = "Turn Right";

	const char* pLeftUp    = 0;
	const char* pLeftDown  = 0;
	const char* pLeftLeft  = 0;
	const char* pLeftRight = 0;

	const char* pRightUp   = 0;
	const char* pRightDown = 0;
	const char* pRightLeft = 0;
	const char* pRightRight= 0;

	switch( GetCurrentButtonId() )
	{
	case ID_THUMBSTICKS_STANDARD:
		ImageId = ID_THUMBSTICKS_STANDARDCONFIG;
		pLeftUp     = pMoveForward;
		pLeftDown   = pMoveBackward;
		pLeftLeft   = pMoveLeft;
		pLeftRight  = pMoveRight;

		pRightUp    = pLookUp;
		pRightDown  = pLookDown;
		pRightLeft  = pTurnLeft;
		pRightRight = pTurnRight;
		break;

	case ID_THUMBSTICKS_SOUTHPAW:
		ImageId = ID_THUMBSTICKS_SOUTHPAWCONFIG;
		pLeftUp     = pLookUp;
		pLeftDown   = pLookDown;
		pLeftLeft   = pTurnLeft;
		pLeftRight  = pTurnRight;
		pRightUp    = pMoveForward;
		pRightDown  = pMoveBackward;
		pRightLeft  = pMoveLeft;
		pRightRight = pMoveRight;
		break;

	case ID_THUMBSTICKS_ASSBACKWARDS:
		ImageId = ID_THUMBSTICKS_LEGACYCONFIG;
		pLeftUp     = pMoveForward;
		pLeftDown   = pMoveBackward;
		pLeftLeft   = pTurnLeft;
		pLeftRight  = pTurnRight;
		pRightUp    = pLookUp;
		pRightDown  = pLookDown;
		pRightLeft  = pMoveLeft;
		pRightRight = pMoveRight;
		break;

	case ID_THUMBSTICKS_ASSBACKWARDSSOUTHPAW:
		ImageId = ID_THUMBSTICKS_LEGACYSOUTHPAWCONFIG;
		pLeftUp     = pLookUp;
		pLeftDown   = pLookDown;
		pLeftLeft   = pMoveLeft;
		pLeftRight  = pMoveRight;
		pRightUp    = pMoveForward;
		pRightDown  = pMoveBackward;
		pRightLeft  = pTurnLeft;
		pRightRight = pTurnRight;
		break;
	}

	Sprite* pSprite = GetSprite( ImageId );

	if( pSprite )
		pSprite->m_state.familycolor.a = 255;

	Sprite* pLeftStick = GetSprite( ID_THUMBSTICKS_LEFTSTICK );

	if( pLeftStick )
	{
		SetText( ID_THUMBSTICKS_TEXTUP,    pLeftStick, pLeftUp );
		SetText( ID_THUMBSTICKS_TEXTDOWN,  pLeftStick, pLeftDown );
		SetText( ID_THUMBSTICKS_TEXTLEFT,  pLeftStick, pLeftLeft );
		SetText( ID_THUMBSTICKS_TEXTRIGHT, pLeftStick, pLeftRight );
	}

	Sprite* pRightStick = GetSprite( ID_THUMBSTICKS_RIGHTSTICK );

	if( pRightStick )
	{
		SetText( ID_THUMBSTICKS_TEXTUP,    pRightStick, pRightUp );
		SetText( ID_THUMBSTICKS_TEXTDOWN,  pRightStick, pRightDown );
		SetText( ID_THUMBSTICKS_TEXTLEFT,  pRightStick, pRightLeft );
		SetText( ID_THUMBSTICKS_TEXTRIGHT, pRightStick, pRightRight );
	}

	return;
}

//=============================================================================
// ScreenThumbSticks::OnCommand
//=============================================================================

void ScreenThumbSticks::OnCommand( int Command )
{
	int ThumbStickMode = 0;

	switch( Command ) 
	{
	case ID_THUMBSTICKS_STANDARD:
		ThumbStickMode = AvatarControlMapper::STANDARD;
		break;

	case ID_THUMBSTICKS_SOUTHPAW:
		ThumbStickMode = AvatarControlMapper::ALT1;
		break;

	case ID_THUMBSTICKS_ASSBACKWARDS:
		ThumbStickMode = AvatarControlMapper::ASSBACKWARDS;
		break;

	case ID_THUMBSTICKS_ASSBACKWARDSSOUTHPAW:
		ThumbStickMode = AvatarControlMapper::ASSBACKWARDSALT1;
		break;
	}

	g_GlobalProfile.SetThumbStickMode( ThumbStickMode );

	// update the avatar
	AvatarControlMapper*	c = NULL;
	c = (AvatarControlMapper*)g_controlFocus.Find("Avatar");

	AvatarControlMapper::MAPPINGSCHEME AvThumbStickMode =
		AvatarControlMapper::MAPPINGSCHEME( ThumbStickMode );

	if (c)
		c->SetThumbStickMode(AvThumbStickMode);

	g_OptionsOnScreen = false;
	EndScreen(0);
}
