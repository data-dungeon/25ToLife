#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "screen_PCKeyBindings.h"
#include "data/screens/PCKeyBindings.h"
#include "frontend/frontend.h"

#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "screen_PCKeyBindings.h"
#include <shlobj.h>



#define SAVE_GAME_LOCATION "\\25 to life"

/*virtual*/ void ScreenPCKeyBindings::OnInitialize(void)
{
	Screen* pScreen = m_pScreen;

    UseHighlighting(true);

	if(pScreen)
	{
		pScreen->SetInitialButton(ID_KEYBIND_FORWARD);
		pScreen->SetNextButton(ID_KEYBIND_BACKWARD);
		pScreen->SetNextButton(ID_KEYBIND_STRAFE_LEFT);
		pScreen->SetNextButton(ID_KEYBIND_STRAFE_RIGHT);
		pScreen->SetNextButton(ID_KEYBIND_JUMP);
		pScreen->SetNextButton(ID_KEYBIND_FIRE);
		pScreen->SetNextButton(ID_KEYBIND_RELOAD);
		pScreen->SetNextButton(ID_KEYBIND_USE);
		pScreen->SetNextButton(ID_KEYBIND_CROUCH);
		pScreen->SetNextButton(ID_KEYBIND_THROWGRENADE);
		pScreen->SetNextButton(ID_KEYBIND_LEANLEFT);
		pScreen->SetNextButton(ID_KEYBIND_LEANRIGHT);
		pScreen->SetNextButton(ID_KEYBIND_SCOREBOARD);
		pScreen->SetNextButton(ID_KEYBIND_TAG);
		pScreen->SetNextButton(ID_KEYBIND_WALK);
		pScreen->SetNextButton(ID_KEYBIND_ZOOM);
		pScreen->SetNextButton(ID_KEYBIND_NEXTWEAPON);
		pScreen->SetNextButton(ID_KEYBIND_MELEE);
		pScreen->SetNextButton(ID_KEYBIND_CENTERVIEW);
		pScreen->SetNextButton(ID_KEYBIND_FIRSTPERSON);
		pScreen->SetNextButton(ID_KEYBIND_TAUNT);
		pScreen->SetNextButton(ID_KEYBIND_DEFAULTS);
		pScreen->SetNextButton(ID_KEYBIND_BACK);

        pScreen->SelectButton(ID_KEYBIND_FIRE);
	}

	ScreenBaseClass::OnInitialize();
}

#include "platform/keyctrl.h"
void ScreenPCKeyBindings::UpdateCurrentKeys()
{
	ControllerInputDriver* controller = g_input.GetController( 0 );
	KeyControllerInputDriver* kcid = static_cast<KeyControllerInputDriver*>( controller );

	char keyNameText_Fire[ 64 ];
	char keyNameText_Forward[ 64 ];
	char keyNameText_Backward[ 64 ];
	char keyNameText_StrafeLeft[ 64 ];
	char keyNameText_StrafeRight[ 64 ];
	char keyNameText_Jump[ 64 ];
	char keyNameText_Reload[ 64 ];
	char keyNameText_Use[ 64 ];
	char keyNameText_Crouch[ 64 ];
	char keyNameText_LeanLeft[ 64 ];
	char keyNameText_LeanRight[ 64 ];
	char keyNameText_Scoreboard[ 64 ];
	char keyNameText_Tag[ 64 ];
	char keyNameText_ThrowGrenade[ 64 ];
	char keyNameText_Walk[ 64 ];
	char keyNameText_Zoom[ 64 ];
	char keyNameText_NextWeapon[ 64 ];
	char keyNameText_Melee[ 64 ];
	char keyNameText_CenterView[ 64 ];
	char keyNameText_FirstPerson[ 64 ];
	char keyNameText_Taunt[ 64 ];

	kcid->ButtonMapToString( KeyControllerInputDriver::BUTTONABSTRACT_FIRE,			keyNameText_Fire );
	kcid->ButtonMapToString( KeyControllerInputDriver::BUTTONABSTRACT_MOVE_FORWARD, keyNameText_Forward );
	kcid->ButtonMapToString( KeyControllerInputDriver::BUTTONABSTRACT_MOVE_BACKWARD,keyNameText_Backward );
	kcid->ButtonMapToString( KeyControllerInputDriver::BUTTONABSTRACT_STRAFE_LEFT,	keyNameText_StrafeLeft );
	kcid->ButtonMapToString( KeyControllerInputDriver::BUTTONABSTRACT_STRAFE_RIGHT, keyNameText_StrafeRight );
	kcid->ButtonMapToString( KeyControllerInputDriver::BUTTONABSTRACT_JUMP,			keyNameText_Jump );
	kcid->ButtonMapToString( KeyControllerInputDriver::BUTTONABSTRACT_RELOAD,		keyNameText_Reload );
	kcid->ButtonMapToString( KeyControllerInputDriver::BUTTONABSTRACT_USE ,			keyNameText_Use );
	kcid->ButtonMapToString( KeyControllerInputDriver::BUTTONABSTRACT_CROUCH,		keyNameText_Crouch );	
	kcid->ButtonMapToString( KeyControllerInputDriver::BUTTONABSTRACT_LEAN_LEFT,	keyNameText_LeanLeft );
	kcid->ButtonMapToString( KeyControllerInputDriver::BUTTONABSTRACT_LEAN_RIGHT,	keyNameText_LeanRight );
	kcid->ButtonMapToString( KeyControllerInputDriver::BUTTONABSTRACT_SELECT,		keyNameText_Scoreboard );
	kcid->ButtonMapToString( KeyControllerInputDriver::BUTTONABSTRACT_TAG,			keyNameText_Tag );
	kcid->ButtonMapToString( KeyControllerInputDriver::BUTTONABSTRACT_THROW_GRENADE,keyNameText_ThrowGrenade );
	kcid->ButtonMapToString( KeyControllerInputDriver::BUTTONABSTRACT_WALK,			keyNameText_Walk);
	kcid->ButtonMapToString( KeyControllerInputDriver::BUTTONABSTRACT_ZOOM,			keyNameText_Zoom);
	kcid->ButtonMapToString( KeyControllerInputDriver::BUTTONABSTRACT_NEXT_WEAPON,	keyNameText_NextWeapon );
	kcid->ButtonMapToString( KeyControllerInputDriver::BUTTONABSTRACT_PREVIOUS_WEAPON,	keyNameText_Melee);
	kcid->ButtonMapToString( KeyControllerInputDriver::BUTTONABSTRACT_CENTER_VIEW,	keyNameText_CenterView);
	kcid->ButtonMapToString( KeyControllerInputDriver::BUTTONABSTRACT_FIRST_PERSON, keyNameText_FirstPerson);
	kcid->ButtonMapToString( KeyControllerInputDriver::BUTTONABSTRACT_TAUNT,		keyNameText_Taunt);

	m_pScreen->GetTextSprite(ID_KEYBIND_FIRE_TEXT)->SetText( keyNameText_Fire );
	m_pScreen->GetTextSprite(ID_KEYBIND_FORWARD_TEXT)->SetText( keyNameText_Forward );
	m_pScreen->GetTextSprite(ID_KEYBIND_BACKWARD_TEXT)->SetText( keyNameText_Backward );
	m_pScreen->GetTextSprite(ID_KEYBIND_STRAFE_LEFT_TEXT)->SetText( keyNameText_StrafeLeft );
	m_pScreen->GetTextSprite(ID_KEYBIND_STRAFE_RIGHT_TEXT)->SetText( keyNameText_StrafeRight );
	m_pScreen->GetTextSprite(ID_KEYBIND_JUMP_TEXT)->SetText( keyNameText_Jump );
	m_pScreen->GetTextSprite(ID_KEYBIND_RELOAD_TEXT)->SetText( keyNameText_Reload );
	m_pScreen->GetTextSprite(ID_KEYBIND_USE_TEXT)->SetText( keyNameText_Use );
	m_pScreen->GetTextSprite(ID_KEYBIND_CROUCH_TEXT)->SetText( keyNameText_Crouch );
	m_pScreen->GetTextSprite(ID_KEYBIND_LEANLEFT_TEXT)->SetText( keyNameText_LeanLeft );
	m_pScreen->GetTextSprite(ID_KEYBIND_LEANRIGHT_TEXT)->SetText( keyNameText_LeanRight );
	m_pScreen->GetTextSprite(ID_KEYBIND_SCOREBOARD_TEXT)->SetText( keyNameText_Scoreboard );
	m_pScreen->GetTextSprite(ID_KEYBIND_TAG_TEXT)->SetText( keyNameText_Tag );
	m_pScreen->GetTextSprite(ID_KEYBIND_THROWGRENADE_TEXT)->SetText( keyNameText_ThrowGrenade );
	m_pScreen->GetTextSprite(ID_KEYBIND_WALK_TEXT)->SetText( keyNameText_Walk );
	m_pScreen->GetTextSprite(ID_KEYBIND_ZOOM_TEXT)->SetText( keyNameText_Zoom );
	m_pScreen->GetTextSprite(ID_KEYBIND_NEXTWEAPON_TEXT)->SetText( keyNameText_NextWeapon );
	m_pScreen->GetTextSprite(ID_KEYBIND_MELEE_TEXT)->SetText( keyNameText_Melee);
	m_pScreen->GetTextSprite(ID_KEYBIND_CENTERVIEW_TEXT)->SetText( keyNameText_CenterView );
	m_pScreen->GetTextSprite(ID_KEYBIND_FIRSTPERSON_TEXT)->SetText( keyNameText_FirstPerson );
	m_pScreen->GetTextSprite(ID_KEYBIND_TAUNT_TEXT)->SetText( keyNameText_Taunt );
}

//#include "dev/Avalanche/GameLib/platform/keyctrl.h"
void ScreenPCKeyBindings::OnScreenIntro( void )
{
	ControllerInputDriver* controller = g_input.GetController( 0 );
	KeyControllerInputDriver* kcid = static_cast<KeyControllerInputDriver*>( controller );
	if (kcid)
	{
		char Path[MAX_PATH];
		char myDocumentsLocation[MAX_PATH];
		HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, myDocumentsLocation);
		ASSERT(result == S_OK);
		strcat(myDocumentsLocation, SAVE_GAME_LOCATION);
		sprintf(Path, "%s\\SaveGames\\%s\\%s", myDocumentsLocation, g_GlobalProfile.GetName(), "keymap.ini");
		kcid->LoadButtonMap(Path);
	}

	UpdateCurrentKeys();

    ScreenBaseClass::OnScreenIntro();

	g_MenuCursor.SetHidden(false);
}



#ifdef DIRECTX_PC
extern int g_buttonAbstractWeAreBinding; // SUPER-GHETTO-HACK
#define COMMAND_KEYCONFLICT_OK 1
#endif // DIRECTX_PC

void ScreenPCKeyBindings::OnCommand( int Command )
{
	ControllerInputDriver* controller = g_input.GetController( 0 );
	KeyControllerInputDriver* kcid = static_cast<KeyControllerInputDriver*>( controller );

    switch (Command)
	{
		case ID_KEYBIND_FIRE:
			g_buttonAbstractWeAreBinding = KeyControllerInputDriver::BUTTONABSTRACT_FIRE;
			break;
		case ID_KEYBIND_FORWARD:
			g_buttonAbstractWeAreBinding = KeyControllerInputDriver::BUTTONABSTRACT_MOVE_FORWARD;
			break;
		case ID_KEYBIND_BACKWARD:
			g_buttonAbstractWeAreBinding = KeyControllerInputDriver::BUTTONABSTRACT_MOVE_BACKWARD;
			break;
		case ID_KEYBIND_STRAFE_LEFT:
			g_buttonAbstractWeAreBinding = KeyControllerInputDriver::BUTTONABSTRACT_STRAFE_LEFT;
			break;
		case ID_KEYBIND_STRAFE_RIGHT:
			g_buttonAbstractWeAreBinding = KeyControllerInputDriver::BUTTONABSTRACT_STRAFE_RIGHT;
			break;
		case ID_KEYBIND_JUMP:
			g_buttonAbstractWeAreBinding = KeyControllerInputDriver::BUTTONABSTRACT_JUMP;
			break;
		case ID_KEYBIND_RELOAD:
			g_buttonAbstractWeAreBinding = KeyControllerInputDriver::BUTTONABSTRACT_RELOAD;
			break;
		case ID_KEYBIND_USE:
			g_buttonAbstractWeAreBinding = KeyControllerInputDriver::BUTTONABSTRACT_USE;
			break;
		case ID_KEYBIND_CROUCH:
			g_buttonAbstractWeAreBinding = KeyControllerInputDriver::BUTTONABSTRACT_CROUCH;
			break;
		case ID_KEYBIND_LEANLEFT:
			g_buttonAbstractWeAreBinding = KeyControllerInputDriver::BUTTONABSTRACT_LEAN_LEFT;
			break;
		case ID_KEYBIND_LEANRIGHT:
			g_buttonAbstractWeAreBinding = KeyControllerInputDriver::BUTTONABSTRACT_LEAN_RIGHT;
			break;
		case ID_KEYBIND_SCOREBOARD:
			g_buttonAbstractWeAreBinding = KeyControllerInputDriver::BUTTONABSTRACT_SELECT;
			break;
		case ID_KEYBIND_TAG:
			g_buttonAbstractWeAreBinding = KeyControllerInputDriver::BUTTONABSTRACT_TAG;
			break;
		case ID_KEYBIND_THROWGRENADE:
			g_buttonAbstractWeAreBinding = KeyControllerInputDriver::BUTTONABSTRACT_THROW_GRENADE;
			break;
		case ID_KEYBIND_WALK:
			g_buttonAbstractWeAreBinding = KeyControllerInputDriver::BUTTONABSTRACT_WALK;
			break;
		case ID_KEYBIND_ZOOM:
			g_buttonAbstractWeAreBinding = KeyControllerInputDriver::BUTTONABSTRACT_ZOOM;
			break;
		case ID_KEYBIND_NEXTWEAPON:
			g_buttonAbstractWeAreBinding = KeyControllerInputDriver::BUTTONABSTRACT_NEXT_WEAPON;
			break;
		case ID_KEYBIND_MELEE:
			g_buttonAbstractWeAreBinding = KeyControllerInputDriver::BUTTONABSTRACT_PREVIOUS_WEAPON;
			break;
		case ID_KEYBIND_CENTERVIEW:
			g_buttonAbstractWeAreBinding = KeyControllerInputDriver::BUTTONABSTRACT_CENTER_VIEW;
			break;
		case ID_KEYBIND_FIRSTPERSON:
			g_buttonAbstractWeAreBinding = KeyControllerInputDriver::BUTTONABSTRACT_FIRST_PERSON;
			break;
		case ID_KEYBIND_TAUNT:
			g_buttonAbstractWeAreBinding = KeyControllerInputDriver::BUTTONABSTRACT_TAUNT;
			break;
		case ID_KEYBIND_DEFAULTS:
		case ID_KEYBIND_BACK:
			// grab our profile directory so we can write the
			// keymap.ini to the right place
			char Path[MAX_PATH];
			char myDocumentsLocation[MAX_PATH];
			HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, myDocumentsLocation);
			ASSERT(result == S_OK);
			strcat(myDocumentsLocation, SAVE_GAME_LOCATION);
			sprintf(Path, "%s\\SaveGames\\%s\\%s", myDocumentsLocation, g_GlobalProfile.GetName(), "keymap.ini");

			if (Command == ID_KEYBIND_DEFAULTS)
			{
				if (kcid)
					kcid->WriteDefaultButtonMap(Path);
			}
			else
			{
				// write our stuff
				char errorString[BINDING_CONFLICT_STRING_LENGTH];
				if(kcid->WriteButtonMap(Path, errorString))
					g_FrontEnd.EndScreen(0);
				else // we have a key conflict
				{
					// do popup stuff here
					ScreenPopupDialog* pDialog = GetPopupDialog();
					if( pDialog )
					{
						pDialog->Clear();
						pDialog->SetTitle( "Key Binding Conflict" );
						pDialog->SetMessage( errorString );
						pDialog->AddMenuItem( "OK", COMMAND_KEYCONFLICT_OK, 0 );
						pDialog->SelectMenuItem( COMMAND_KEYCONFLICT_OK );
					}
					ShowPopupDialog();
				}
			}
			break;
	}
    ScreenBaseClass::OnCommand(Command);
}

void ScreenPCKeyBindings::OnDialogCommand( int command )
{
	switch( command )
	{
	case COMMAND_KEYCONFLICT_OK:
		g_FrontEnd.RemoveActivePopup();
	}
}

void ScreenPCKeyBindings::OnScreenUpdate( void )
{
	// don't process screen input when we have
	// a popup
	if (m_pScreen->GetActivePopup())
		return;
   	ControllerInputDriver* controller = g_input.GetController( 0 );
	KeyControllerInputDriver* kcid = static_cast<KeyControllerInputDriver*>( controller );
	
	UpdateCurrentKeys();

	if( g_buttonAbstractWeAreBinding != -1 )
	{
		char backText[ 1024 ];
		sprintf( backText, "Press a key to bind to \"%s\"", kcid->ButtonMapToDescription( g_buttonAbstractWeAreBinding ) );
		m_pScreen->GetTextSprite( ID_KEYBIND_BACK )->SetText( backText );
	}
	else
	{
		m_pScreen->GetTextSprite( ID_KEYBIND_BACK )->SetText( "Back" );
	}
	
	if (g_buttonAbstractWeAreBinding == -1)
	{
		if (g_MenuCursor.IsActive()) {
			float x1, y1, x2, y2;	
			float x = g_MenuCursor.GetX();
			float y = g_MenuCursor.GetY();
			Button* button = GetDefaultButton();
			// functionality for button based menus
			if (!button)
			{
				if (g_MenuCursor.CursorButtonPressed(0))
					OnButtonStart();
			}
			// search through the buttons to see if we're over one
			// if so, check to see if we're clicking the mouse
			while(button) 
			{
				button->SpriteHost()->GetFamilyRect(x1, y1, x2, y2);
				if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) )
				{
					if (!button->Enabled()) break;
					SelectButton(button);
					if ( g_MenuCursor.CursorButtonPressed(0) )
					{
							button->m_Selected = true;
							g_MenuCursor.ForceButtonUp(0);
							OnButtonOK(); 
					}
					else
					{
							button->m_Selected = false;
					}
					break;
				}
				button = button->GetNextButton();
			}
		}
	}
	else 
	{
		/// Check if we are in the pc key bindings menu and have clicked on an action to rebind
		ControllerInputDriver* controller = g_input.GetController( 0 );
		KeyControllerInputDriver* kcid = static_cast<KeyControllerInputDriver*>( controller );

		// skip if we pressed enter to get here (let the input process another frame)
		if (enterPressed)
		{
			enterPressed = false;
			return;
		}
		/// Check if any recognized key has been pressed this frame
		for( int bcNumber = 0; bcNumber < BC_BUTTONCODES; ++ bcNumber )
		{
			ButtonCode bc = (ButtonCode) bcNumber;
			int vk = kcid->GetVKForBC( bc );
			WindowsSystemInputDriver* wsid = (WindowsSystemInputDriver*)g_inputPtr;
			if( GetKeyState( vk ) & 0x8000									||
				( bc == BC_MOUSEWHEELDOWN && wsid->wasMouseWheelMoved() == -1)	||
				( bc == BC_MOUSEWHEELUP && wsid->wasMouseWheelMoved() == 1)
				)
			{
				if (bc == BC_ESCAPE) // don't bind escape
				{
					continue;
				}
				/// Bind that key to that abstract action
				kcid->BindBCToAbstractAction( bc, g_buttonAbstractWeAreBinding );
				g_buttonAbstractWeAreBinding = -1;
				return;
			}
		}
	}
}

void ScreenPCKeyBindings::HandleInput(UIHandler *pUIHandler)
{
	// we DON'T want to call into the base class here
	// so we can't esc out of the screen without testing
	// for binding conflicts
	if ( g_buttonAbstractWeAreBinding != -1)
		return;
	if( pUIHandler->ButtonPressed( UIHandler::BUTTON_CANCEL ) &&
		g_buttonAbstractWeAreBinding == -1 )
		OnButtonCancel();
	if( pUIHandler->ButtonPressed( UIHandler::CURSOR_UP ) )
		OnCursorUp();
	else
	if( pUIHandler->ButtonPressed( UIHandler::CURSOR_DOWN ) )
		OnCursorDown();
	
	if( pUIHandler->ButtonPressed( UIHandler::CURSOR_LEFT ) )
		OnCursorLeft();
	else
	if( pUIHandler->ButtonPressed( UIHandler::CURSOR_RIGHT ) )
		OnCursorRight();

	if( pUIHandler->ButtonPressed( UIHandler::BUTTON_START ) )
	{
		ControllerInputDriver* controller = g_input.GetController( 0 );
		KeyControllerInputDriver* kcid = static_cast<KeyControllerInputDriver*>( controller );
		kcid->ClearPressed();
		enterPressed = true;
		OnButtonOK();
	}

}

