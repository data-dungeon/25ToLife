#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "screen_EOREditAccount1.h"
#include "data/screens/EOR.h"
#include "frontend/frontend.h"

const char UNCHANGED_PASSWORD[] = "<unchanged>";

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is first loaded
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenEOREditAccount1::OnInitialize(void)
{
    Screen* pScreen = m_pScreen;

    UseHighlighting(true);

	if(pScreen)
	{
		pScreen->SetInitialButton(ID_OLD_PWD);
		pScreen->SetNextButton(ID_EDIT_PWD);
        pScreen->SetNextButton(ID_EDIT_CONFIRM_PWD);
        pScreen->SetNextButton(ID_EDIT_SAVE_PWD);
        pScreen->SetNextButton(ID_EDIT_EMAIL);
        pScreen->SetNextButton(ID_EDIT_EIDOS);
        pScreen->SetNextButton(ID_EDIT_SHARE);
        pScreen->SetNextButton(ID_EDIT_CONTINUE);
		pScreen->SetCursor(ID_EDIT_CURSOR);

        pScreen->SelectButton(ID_EDIT_PWD);

		// Hide the PS2 help bar
#ifdef DIRECTX_PC
		GetSprite(ID_EDIT_HELPRECT)->HideAll();
		pScreen->SetNextButton(ID_EDIT_BACK);
#else
		GetSprite(ID_EDIT_BACK)->HideAll();
#endif
	}

    _promos = true;
    _shareInfo = true;
    _savePassword = true;
    _name[0] = 0;
	_password[0] = 0;
    _oldPassword[0] = 0;
    _confirm[0] = 0;
    _email[0] = 0;

	ScreenBaseClass::OnInitialize();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is displayed
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount1::OnScreenIntro( void )
{
    _status = RUNNING;
    _textField = NONE;

    ScreenBaseClass::OnScreenIntro();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is ended
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount1::OnPrevScreen( Screen* pPrevScreen )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when user selects a button
///
/// \param Command ID of the selected button
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount1::OnCommand( int Command )
{
    // Set the status flag
    switch (Command)
    {
        case ID_EDIT_NAME:
            _textField = NAME;
			_status = ENTER_NAME;
            break;

		case ID_OLD_PWD:
            _textField = OLD_PASSWORD;
			_status = ENTER_OLD_PASSWORD;
            break;

        case ID_EDIT_PWD:
            _textField = NEW_PASSWORD;
			_status = ENTER_NEW_PASSWORD;
            break;

        case ID_EDIT_CONFIRM_PWD:
            _textField = NEW_CONFIRM;
			_status = ENTER_NEW_CONFIRM_PASSWORD;
            break;

        case ID_EDIT_EMAIL:
            _textField = EMAIL;
            _status = ENTER_EMAIL;
            break;

        case ID_EDIT_CONTINUE:
            _status = CONTINUE;
            break;

#ifdef DIRECTX_PC
		case ID_EDIT_BACK:
			_status = BACK;
			break;
#endif // DIRECTX_PC
    }

    ScreenBaseClass::OnCommand(Command);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when a popup window closes (on this screen, the virtual keyboard)
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount1::OnKeyboardInput( const char* keyboardString )
{
    // Fill in the correct text field and value for the value returned by the keyboard.
    switch (_textField)
    {
        case NAME:
            setName(keyboardString);
            break;

		case OLD_PASSWORD:
            setOldPassword(keyboardString);
            break;

        case NEW_PASSWORD:
            setNewPassword(keyboardString);
            break;

        case NEW_CONFIRM:
            setNewPasswordConfirm(keyboardString);
            break;

        case EMAIL:
            setEmail(keyboardString);
            break;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the left DPad button is pressed.  Cycle the appropriate item.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount1::OnCursorLeft( void )
{
    Screen* pScreen = m_pScreen;

    // If we're on a toggle line, toggle it and change the display accordingly
    if (pScreen && pScreen->GetCurrentButton() == pScreen->GetButton(ID_EDIT_EIDOS))
    {
        setPromos(!_promos);
        PlaySoundEvent(Screen::SOUNDEVENT_NEXTBUTTON);
    }
    else if (pScreen && pScreen->GetCurrentButton() == pScreen->GetButton(ID_EDIT_SHARE))
    {
        setShareInfo(!_shareInfo);
        PlaySoundEvent(Screen::SOUNDEVENT_NEXTBUTTON);
    }
    else if (pScreen && pScreen->GetCurrentButton() == pScreen->GetButton(ID_EDIT_SAVE_PWD))
    {
        setSavePassword(!_savePassword);
        PlaySoundEvent(Screen::SOUNDEVENT_NEXTBUTTON);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the right DPad button is pressed.  Cycle the appropriate item.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount1::OnCursorRight( void )
{
    OnCursorLeft();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the current account name and set the screen item
///
/// \param pName The name to set
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount1::setName(const char *pName)
{
    OSIstrncpy(_name, pName, OSI_PLAYER_NAME_LENGTH ); // OSIstrncpy puts a 0 in last byte

    m_pScreen->GetTextSprite(ID_EDIT_NAME_TEXT)->SetTextNoTranslate(_name);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the current password and set the screen item
///
/// \param pPassword The password to set
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount1::setOldPassword(const char *pPassword)       
{
    OSIstrncpy(_oldPassword, pPassword, OSI_PASSWORD_LENGTH ); // OSIstrncpy puts a 0 in last byte

	if(_oldPassword[0] == 0)
	{
		m_pScreen->GetTextSprite(ID_OLD_PWD_TEXT)->SetText("");
		EnableButton( ID_OLD_PWD, true );
	}
	else
	{
		m_pScreen->GetTextSprite(ID_OLD_PWD_TEXT)->SetText("*****");
		EnableButton( ID_OLD_PWD, false );
	}
	SelectInitialButton();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the new password and set the screen item
///
/// \param pPassword The password to set
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount1::setNewPassword(const char *pPassword)       
{
    OSIstrncpy(_password, pPassword, OSI_PASSWORD_LENGTH ); // OSIstrncpy puts a 0 in last byte

	if(_password[0] == 0)
	{
		m_pScreen->GetTextSprite(ID_EDIT_PWD_TEXT)->SetText(UNCHANGED_PASSWORD);
	}
	else
	{
		m_pScreen->GetTextSprite(ID_EDIT_PWD_TEXT)->SetText("*****");
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the confirmation password and set the screen item
///
/// \param pConfirm The confirmation password to set
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount1::setNewPasswordConfirm(const char *pConfirm) 
{
    OSIstrncpy(_confirm, pConfirm, OSI_PASSWORD_LENGTH ); // OSIstrncpy puts a 0 in last byte

	if(_confirm[0] == 0)
	{
		m_pScreen->GetTextSprite(ID_EDIT_CONFIRM_PWD_TEXT)->SetText("<unchanged>");
	}
	else
	{
		m_pScreen->GetTextSprite(ID_EDIT_CONFIRM_PWD_TEXT)->SetText("*****");
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Toggle the password save value
///
/// \param Save The save value to set
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount1::setSavePassword(bool Save)               
{
    _savePassword = Save; 

    if (_savePassword)
    {
        m_pScreen->GetTextSprite(ID_EDIT_SAVE_PWD_TEXT)->SetText("Yes");
    }
    else
    {
        m_pScreen->GetTextSprite(ID_EDIT_SAVE_PWD_TEXT)->SetText("No");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the email value and screen item.
///
/// \param The new value for the email field.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount1::setEmail(const char *pEmail)             
{
    OSIstrncpy(_email, pEmail, OSI_EMAIL_LENGTH ); // OSIstrncpy puts a 0 in last byte

    m_pScreen->GetTextSprite(ID_EDIT_EMAIL_TEXT)->SetText(_email);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the Eidos promotional field value and screen item.
///
/// \param The new value for the Eidos promotional field
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount1::setPromos(bool Promos)                   
{
    _promos = Promos; 

    if (_promos)
    {
        m_pScreen->GetTextSprite(ID_EDIT_EIDOS_TEXT)->SetText("Yes");
    }
    else
    {
        m_pScreen->GetTextSprite(ID_EDIT_EIDOS_TEXT)->SetText("No");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the preferred partner promotional field value and screen item.
///
/// \param The new value for the preferred partner promotional field
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount1::setShareInfo(bool ShareInfo)             
{
    _shareInfo = ShareInfo; 

    if (_shareInfo)
    {
        m_pScreen->GetTextSprite(ID_EDIT_SHARE_TEXT)->SetText("Yes");
    }
    else
    {
        m_pScreen->GetTextSprite(ID_EDIT_SHARE_TEXT)->SetText("No");
    }
}
