#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "screen_EORCreateAccount1.h"
#include "data/screens/EOR.h"
#include "frontend/frontend.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is first loaded
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenEORCreateAccount1::OnInitialize(void)
{
	UseHighlighting(true);

	SetHelpRect(ID_CREATE_HELPRECT);
	SetInitialButton(ID_CREATE_NAME);
	SetNextButton(ID_CREATE_PWD);
	SetNextButton(ID_CREATE_CONFIRM_PWD);
	SetNextButton(ID_CREATE_SAVE_PWD);
	SetNextButton(ID_CREATE_EMAIL);
	SetNextButton(ID_CREATE_EIDOS);
	SetNextButton(ID_CREATE_SHARE);
	SetNextButton(ID_CREATE_CONTINUE);
	SetCursor(ID_CREATE_CURSOR);
	SelectButton(ID_CREATE_NAME);

	// Hide PS2 help bar
#ifdef DIRECTX_PC
	GetSprite(ID_CREATE_HELPRECT)->HideAll();

	// Set back button as next
	SetNextButton(ID_CREATE_BACK);
#else
	GetSprite(ID_CREATE_BACK)->HideAll();
#endif

	_promos = true;
	_shareInfo = true;
	_savePassword = true;
	_inUse = false;
	_name[0] = 0;
	_password[0] = 0;
	_confirm[0] = 0;
	_email[0] = 0;

	ScreenBaseClass::OnInitialize();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is displayed
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount1::OnScreenIntro( void )
{
    _status = RUNNING;
    _textField = NONE;

	if(m_pScreen != NULL && !_inUse) {
        // Make sure we have the right titles
        m_pScreen->GetTextSprite(ID_CREATE_TITLE)->SetText("Create Eidos Online Account (1/2)");

		m_pScreen->GetTextSprite(ID_CREATE_NAME_TEXT)->SetText("");
		m_pScreen->GetTextSprite(ID_CREATE_PWD_TEXT)->SetText("");
		m_pScreen->GetTextSprite(ID_CREATE_SAVE_PWD_TEXT)->SetText("Yes");
		m_pScreen->GetTextSprite(ID_CREATE_CONFIRM_PWD_TEXT)->SetText("");
		m_pScreen->GetTextSprite(ID_CREATE_EMAIL_TEXT)->SetText("");
		_inUse = true;
	}

    ScreenBaseClass::OnScreenIntro();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is ended
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount1::OnPrevScreen( Screen* pPrevScreen )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when user selects a button
///
/// \param Command ID of the selected button
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount1::OnCommand( int Command )
{
    // Set the status flag
    switch (Command)
    {
        case ID_CREATE_NAME:
            _textField = NAME;
			_status = ENTER_NAME;
            break;

        case ID_CREATE_PWD:
            _textField = PASSWORD;
			_status = ENTER_PASSWORD;
            break;

        case ID_CREATE_CONFIRM_PWD:
            _textField = CONFIRM;
			_status = ENTER_CONFIRM_PASSWORD;
            break;

        case ID_CREATE_EMAIL:
            _textField = EMAIL;
            _status = ENTER_EMAIL;
            break;

        case ID_CREATE_CONTINUE:
            _status = CONTINUE;
            break;

#ifdef DIRECTX_PC
		case ID_CREATE_BACK:
			_status = BACK;
			break;
#endif
    }

    ScreenBaseClass::OnCommand(Command);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when a popup window closes (on this screen, the virtual keyboard)
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount1::OnKeyboardInput( const char* pString )
{
    // Fill in the correct text field and value for the value returned by the keyboard.
    switch (_textField)
    {
        case NAME:
            setName(pString);
            break;

        case PASSWORD:
            setPassword(pString);
            break;

        case CONFIRM:
            setPasswordConfirm(pString);
            break;

        case EMAIL:
            setEmail(pString);
            break;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the left DPad button is pressed.  Cycle the appropriate item.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount1::OnCursorLeft( void )
{
    Screen* pScreen = m_pScreen;

    // If we're on a toggle line, toggle it and change the display accordingly
    if (pScreen && pScreen->GetCurrentButton() == pScreen->GetButton(ID_CREATE_EIDOS))
    {
        setPromos(!_promos);
        PlaySoundEvent(Screen::SOUNDEVENT_NEXTBUTTON);
    }
    else if (pScreen && pScreen->GetCurrentButton() == pScreen->GetButton(ID_CREATE_SHARE))
    {
        setShareInfo(!_shareInfo);
        PlaySoundEvent(Screen::SOUNDEVENT_NEXTBUTTON);
    }
    else if (pScreen && pScreen->GetCurrentButton() == pScreen->GetButton(ID_CREATE_SAVE_PWD))
    {
        setSavePassword(!_savePassword);
        PlaySoundEvent(Screen::SOUNDEVENT_NEXTBUTTON);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the right DPad button is pressed.  Cycle the appropriate item.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount1::OnCursorRight( void )
{
    OnCursorLeft();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the current account name and set the screen item
///
/// \param pName The name to set
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount1::setName(const char *pName)
{
    OSIstrncpy(_name, pName, OSI_PLAYER_NAME_LENGTH ); // OSIstrncpy puts a 0 in last byte
    m_pScreen->GetTextSprite(ID_CREATE_NAME_TEXT)->SetTextNoTranslate(_name);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the current password and set the screen item
///
/// \param pPassword The password to set
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount1::setPassword(const char *pPassword)       
{
    OSIstrncpy(_password, pPassword, OSI_PASSWORD_LENGTH ); // OSIstrncpy puts a 0 in last byte

	if(_password[0] == 0)
	{
		m_pScreen->GetTextSprite(ID_CREATE_PWD_TEXT)->SetText("");
	}
	else
	{
		m_pScreen->GetTextSprite(ID_CREATE_PWD_TEXT)->SetText("*****");
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the confirmation password and set the screen item
///
/// \param pConfirm The confirmation password to set
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount1::setPasswordConfirm(const char *pConfirm) 
{
    OSIstrncpy(_confirm, pConfirm, OSI_PASSWORD_LENGTH ); // OSIstrncpy puts a 0 in the last byte

	if(_confirm[0] == 0)
	{
		m_pScreen->GetTextSprite(ID_CREATE_CONFIRM_PWD_TEXT)->SetText("");
	}
	else
	{
		m_pScreen->GetTextSprite(ID_CREATE_CONFIRM_PWD_TEXT)->SetText("*****");
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Toggle the password save value
///
/// \param Save The save value to set
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount1::setSavePassword(bool Save)               
{
    _savePassword = Save; 

    if (_savePassword)
    {
        m_pScreen->GetTextSprite(ID_CREATE_SAVE_PWD_TEXT)->SetText("Yes");
    }
    else
    {
        m_pScreen->GetTextSprite(ID_CREATE_SAVE_PWD_TEXT)->SetText("No");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the email value and screen item.
///
/// \param The new value for the email field.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount1::setEmail(const char *pEmail)             
{
    OSIstrncpy(_email, pEmail, OSI_EMAIL_LENGTH ); // OSIstrncpy puts a 0 in the last byte 

    m_pScreen->GetTextSprite(ID_CREATE_EMAIL_TEXT)->SetText(_email);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the Eidos promotional field value and screen item.
///
/// \param The new value for the Eidos promotional field
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount1::setPromos(bool Promos)                   
{
    _promos = Promos; 

    if (_promos)
    {
        m_pScreen->GetTextSprite(ID_CREATE_EIDOS_TEXT)->SetText("Yes");
    }
    else
    {
        m_pScreen->GetTextSprite(ID_CREATE_EIDOS_TEXT)->SetText("No");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the preferred partner promotional field value and screen item.
///
/// \param The new value for the preferred partner promotional field
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount1::setShareInfo(bool ShareInfo)             
{
    _shareInfo = ShareInfo; 

    if (_shareInfo)
    {
        m_pScreen->GetTextSprite(ID_CREATE_SHARE_TEXT)->SetText("Yes");
    }
    else
    {
        m_pScreen->GetTextSprite(ID_CREATE_SHARE_TEXT)->SetText("No");
    }
}
