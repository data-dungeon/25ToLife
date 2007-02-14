#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "screen_EORRecoverAccount.h"
#include "data/screens/EOR.h"
#include "frontend/frontend.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is first loaded
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenEORRecoverAccount::OnInitialize(void)
{
	UseHighlighting(true);

	SetHelpRect(ID_RECOVER_HELPRECT);
	SetInitialButton(ID_RECOVER_NAME);
	SetNextButton(ID_RECOVER_PWD);
	SetNextButton(ID_RECOVER_SAVE_PWD);
	SetNextButton(ID_RECOVER_RECOVER);
	SetCursor(ID_RECOVER_CURSOR);
	SelectButton(ID_RECOVER_NAME);

	// Hide the PS2 help bar
#ifdef DIRECTX_PC
	GetSprite(ID_RECOVER_HELPRECT)->HideAll();
	SetNextButton(ID_RECOVER_BACK);
#else
	GetSprite(ID_RECOVER_BACK)->HideAll();
#endif

	_savePassword = true;
	_name[0] = 0;
	_password[0] = 0;

	ScreenBaseClass::OnInitialize();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is displayed
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORRecoverAccount::OnScreenIntro( void )
{
    _status = RUNNING;
    _textField = NONE;

	if(m_pScreen != NULL && !_inUse) {
        // Make sure we have the right titles
        m_pScreen->GetTextSprite(ID_RECOVER_TITLE)->SetText("Create Eidos Online Account (1/2)");

		m_pScreen->GetTextSprite(ID_RECOVER_NAME_TEXT)->SetText("");
		m_pScreen->GetTextSprite(ID_RECOVER_PWD_TEXT)->SetText("");
		m_pScreen->GetTextSprite(ID_RECOVER_SAVE_PWD_TEXT)->SetText("Yes");
		_inUse = true;
	}

    ScreenBaseClass::OnScreenIntro();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is ended
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORRecoverAccount::OnPrevScreen( Screen* pPrevScreen )
{
    _inUse = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when user selects a button
///
/// \param Command ID of the selected button
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORRecoverAccount::OnCommand( int Command )
{
    // Set the status flag
    switch (Command)
    {
        case ID_RECOVER_NAME:
            _textField = NAME;
			_status = ENTER_NAME;
            break;

        case ID_RECOVER_PWD:
            _textField = PASSWORD;
			_status = ENTER_PASSWORD;
            break;

        case ID_RECOVER_RECOVER:
            _status = RECOVER;
            break;

#ifdef DIRECTX_PC
		case ID_RECOVER_BACK:
			_status = BACK;
			break;
#endif
    }

    ScreenBaseClass::OnCommand(Command);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when a popup window closes (on this screen, the virtual keyboard)
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORRecoverAccount::OnKeyboardInput( const char* keyboardString )
{
	char newValue[ OSI_PASSWORD_LENGTH ] ;

    // Fill in the correct text field and value for the value returned by the keyboard.
    switch (_textField)
    {
        case NAME:
            OSIstrncpy(_name, keyboardString, OSI_PLAYER_NAME_LENGTH ); // OSIstrncpy puts a 0 in last byte

            m_pScreen->GetTextSprite(ID_RECOVER_NAME_TEXT)->SetText(_name);
            break;

        case PASSWORD:
            OSIstrncpy(_password, keyboardString, OSI_PASSWORD_LENGTH ); // OSIstrncpy puts a 0 in last byte

            if (strlen(_password) > 0)
            {
                OSIstrcpy(newValue, "*****");
            }
            else
            {
                OSIstrcpy(newValue, "");
            }
            m_pScreen->GetTextSprite(ID_RECOVER_PWD_TEXT)->SetText(newValue);
            break;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the left DPad button is pressed.  Toggle the appropriate item.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORRecoverAccount::OnCursorLeft( void )
{
    Screen* pScreen = m_pScreen;

    // If we're on a toggle line, toggle it and change the display accordingly
    if (pScreen && pScreen->GetCurrentButton() == pScreen->GetButton(ID_RECOVER_SAVE_PWD))
    {
        _savePassword = !_savePassword;

        if (_savePassword)
        {
            pScreen->GetTextSprite(ID_RECOVER_SAVE_PWD_TEXT)->SetText("Yes");
        }
        else
        {
            pScreen->GetTextSprite(ID_RECOVER_SAVE_PWD_TEXT)->SetText("No");
        }
        PlaySoundEvent(Screen::SOUNDEVENT_NEXTBUTTON);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the right DPad button is pressed.  Toggle the appropriate item.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORRecoverAccount::OnCursorRight( void )
{
    // This only works because everything on this screen is yes/no
    OnCursorLeft();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the current account name and set the screen item
///
/// \param pName The name to set
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORRecoverAccount::setName(const char *pName)
{
	_inUse = true;
    OSIstrncpy(_name, pName, OSI_PLAYER_NAME_LENGTH ); // OSIstrncpy puts a 0 in last byte

    m_pScreen->GetTextSprite(ID_RECOVER_NAME_TEXT)->SetText(_name);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the current password and set the screen item
///
/// \param pPassword The password to set
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORRecoverAccount::setPassword(const char *pPassword)       
{
	_inUse = true;
    OSIstrncpy(_password, pPassword, OSI_PASSWORD_LENGTH ); // OSIstrncpy puts a 0 in last byte

	if(_password[0] == 0)
	{
		m_pScreen->GetTextSprite(ID_RECOVER_PWD_TEXT)->SetText("");
	}
	else
	{
		m_pScreen->GetTextSprite(ID_RECOVER_PWD_TEXT)->SetText("*****");
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Toggle the password save value
///
/// \param Save The save value to set
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORRecoverAccount::setSavePassword(bool Save)               
{
	_inUse = true;
    _savePassword = Save; 

    if (_savePassword)
    {
        m_pScreen->GetTextSprite(ID_RECOVER_SAVE_PWD_TEXT)->SetText("Yes");
    }
    else
    {
        m_pScreen->GetTextSprite(ID_RECOVER_SAVE_PWD_TEXT)->SetText("No");
    }
}
