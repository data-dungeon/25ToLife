#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "frontend/texteffects.h"
#include "screen_EORLogin.h"
#include "data/screens/EOR.h"
#include "frontend/frontend.h"
#include "frontend/savegame.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is first loaded
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenEORLogin::OnInitialize(void)
{
	Screen* pScreen = m_pScreen;

	UseHighlighting(true);

	SetHelpRect(ID_LOGIN_HELPRECT);
	SetInitialButton(ID_LOGIN_LOGIN);
#ifdef DIRECTX_PC
	// Add arrows to list
	SetNextButton(ID_LOGIN_LBULLETICON);
	SetNextButton(ID_LOGIN_RBULLETICON);
#endif
	SetNextButton(ID_LOGIN_CREATE_ACCOUNT);
	SetNextButton(ID_LOGIN_EDIT_ACCOUNT);
	SetNextButton(ID_LOGIN_RECOVER_ACCOUNT);
	SetNextButton(ID_LOGIN_DELETE);
	//SetNextButton(ID_LOGIN_ONLINE_NAME); // FIXME: enable this if you want to try and make the 'AccountList' popup work...

	SetCursor(ID_LOGIN_CURSOR);

	SelectInitialButton();

	// Set up cycle button for profile names
	AddInterface(ID_LOGIN_PROFILE_LIST, GuiCycleButton::ClassnameStatic());

	Text* pBigProfile = GetTextSprite( ID_LOGIN_BIGPROFILE );

	if( pBigProfile )
		pBigProfile->SetTextEffect( new BassPumpEffect );

#ifdef DIRECTX_PC
	// Hide the PS2 help bar
	GetSprite(ID_LOGIN_HELPRECT)->HideAll();

	// Change the text on the delete account button
	GetTextSprite(ID_LOGIN_DELETE)->SetText("Delete Account");

	// Add back button to list
	SetNextButton(ID_LOGIN_BACK);
#else
	// Hide back button
	GetTextSprite(ID_LOGIN_BACK)->HideAll();
	GetSprite(ID_LOGIN_LOWCURSOR_BACK)->HideAll();
#endif
	ScreenBaseClass::OnInitialize();

	GuiCycleButton* pCycleButton = (GuiCycleButton*)GetInterface(ID_LOGIN_PROFILE_LIST, GuiCycleButton::ClassnameStatic());
	if( pCycleButton )
	{
		pCycleButton->SetTranslateText( false );
	}

	_numAccounts = -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is displayed
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORLogin::OnScreenIntro( void )
{
    _status = RUNNING;

	_VerifyOnlineAccounts();

    // Sets the default button, so must be called before UpdateAccounts or SelectButton.
    ScreenBaseClass::OnScreenIntro();

	// Deselect all buttons (DX only)
#ifdef DIRECTX_PC
	GetButton(ID_LOGIN_BACK)->m_Selected = false;
	GetButton(ID_LOGIN_LBULLETICON)->m_Selected = false;
	GetButton(ID_LOGIN_RBULLETICON)->m_Selected = false;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is displayed
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORLogin::OnScreenUpdate( void )
{
	_VerifyOnlineAccounts();

	ScreenBaseClass::OnScreenUpdate();

	Text* pProfile = GetTextSprite( ID_LOGIN_PROFILE_LIST );

	const char* pProfileName = 0;

	if( pProfile )
		pProfileName = pProfile->m_Text.get();

	Text* pBigProfile = GetTextSprite( ID_LOGIN_BIGPROFILE );

	if( pBigProfile )
	{
		if( 0 == pBigProfile->m_Text.compare( pProfileName ) )
		{
			pBigProfile->SetText( pProfileName );
			pBigProfile->InitAnim();

			SpriteAnim& anim = *pBigProfile->m_anim;

			anim.offset.x.SetFrame( 0.0f, -100.0f );
			anim.offset.x.SetFrame( 0.2f,  -10.0f );
			anim.offset.x.SetFrame( 1.0f,    0.0f );
			anim.offset.x.Deccelerate( true );
			anim.offset.x.Play( false );

			anim.scale.x.SetFrame( 0.0f, 3.5f );
			anim.scale.x.SetFrame( 1.0f, 1.8f );
			anim.scale.x.Deccelerate( true );
			anim.scale.x.Play( false );

			anim.familycolor.a.SetFrame( 0.0f, 0.0f );
			anim.familycolor.a.SetFrame( 0.3f, 1.0f );
			anim.familycolor.a.Deccelerate( true );
			anim.familycolor.a.Play( false );			
		}
	}

	return;
}

#ifdef DIRECTX_PC
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when user selects the OK button
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORLogin::OnButtonOK()
{
	if (GetButton(ID_LOGIN_BACK)->m_Selected)
	{
		GetButton(ID_LOGIN_BACK)->m_Selected = false;
		OnButtonCancel();
		return;
	}
	else if (GetButton(ID_LOGIN_LBULLETICON)->m_Selected)
	{
		GetButton(ID_LOGIN_LBULLETICON)->m_Selected = false;
		OnCursorLeft();
		return;
	}
	else if (GetButton(ID_LOGIN_RBULLETICON)->m_Selected)
	{
		GetButton(ID_LOGIN_RBULLETICON)->m_Selected = false;
		OnCursorRight();
		return;
	}
	else
	{
		ScreenBaseClass::OnButtonOK();
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when user selects a button
///
/// \param Command ID of the selected button
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORLogin::OnCommand( int Command )
{
    // Set the status flag
    switch (Command)
    {
        case ID_LOGIN_LOGIN:
            _status = LOGIN;
            break;

        case ID_LOGIN_CREATE_ACCOUNT:
            _status = CREATE_ACCOUNT;
            break;

        case ID_LOGIN_EDIT_ACCOUNT:
            _status = EDIT_ACCOUNT;
            break;

        case ID_LOGIN_RECOVER_ACCOUNT:
            _status = RECOVER_ACCOUNT;
            break;

        case ID_LOGIN_DELETE:
            _status = DELETE_ACT;
            break;

		case ID_LOGIN_ONLINE_NAME:
			//_status = SOMETHING;
			break;

#ifdef DIRECTX_PC
		case ID_LOGIN_BACK:
			_status = BACK;
			break;
#endif
    }

    ScreenBaseClass::OnCommand(Command);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Add a profile to the profile list by the profile label
///
/// \param pName Name of the profile to add
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORLogin::AddProfile(ts_OnlineAccount *pAccount, int AccountNum)
{
    GuiCycleButton* pCycleButton = (GuiCycleButton*)GetInterface(ID_LOGIN_PROFILE_LIST, GuiCycleButton::ClassnameStatic());

    if (pCycleButton)
    {
        pCycleButton->AddItem(pAccount->AccountName, AccountNum, pAccount);
        _UpdateScreen();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the left DPad button is pressed.  Cycle the appropriate item.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORLogin::OnCursorLeft( void )
{
    GuiCycleButton* pCycleButton = (GuiCycleButton*)GetInterface(ID_LOGIN_PROFILE_LIST, GuiCycleButton::ClassnameStatic());

    Screen* pScreen = m_pScreen;

	if(pScreen)
	{
        // Cycle profiles
#ifdef DIRECTX_PC
        if ((pCycleButton) && (pScreen->GetCurrentButton() == pScreen->GetButton(ID_LOGIN_LOGIN)) ||
			(pScreen->GetCurrentButton() == pScreen->GetButton(ID_LOGIN_LBULLETICON)) ||
			(pScreen->GetCurrentButton() == pScreen->GetButton(ID_LOGIN_RBULLETICON)))
#else
		if ((pCycleButton) && (pScreen->GetCurrentButton() == pScreen->GetButton(ID_LOGIN_LOGIN)))
#endif
        {
            pCycleButton->SelectPrevItem();
			GuiListBox::Item* pItem = pCycleButton->GetItem( pCycleButton->CurrentItem() );
			if( pItem )
			{
				g_GlobalProfile.CurrentAccount = pItem->m_ItemId;
			}
            _UpdateScreen();
            PlaySoundEvent(Screen::SOUNDEVENT_PREVBUTTON);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the right DPad button is pressed.  Cycle the appropriate item.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORLogin::OnCursorRight( void )
{
    GuiCycleButton* pCycleButton = (GuiCycleButton*)GetInterface(ID_LOGIN_PROFILE_LIST, GuiCycleButton::ClassnameStatic());

    Screen* pScreen = m_pScreen;

	if(pScreen)
	{
        // Cycle profiles
#ifdef DIRECTX_PC
        if ((pCycleButton) && (pScreen->GetCurrentButton() == pScreen->GetButton(ID_LOGIN_LOGIN)) ||
			(pScreen->GetCurrentButton() == pScreen->GetButton(ID_LOGIN_LBULLETICON)) ||
			(pScreen->GetCurrentButton() == pScreen->GetButton(ID_LOGIN_RBULLETICON)))
#else
		if ((pCycleButton) && (pScreen->GetCurrentButton() == pScreen->GetButton(ID_LOGIN_LOGIN)))
#endif
        {
            pCycleButton->SelectNextItem();
			GuiListBox::Item* pItem = pCycleButton->GetItem( pCycleButton->CurrentItem() );
			if( pItem )
			{
				g_GlobalProfile.CurrentAccount = pItem->m_ItemId;
			}
            _UpdateScreen();
            PlaySoundEvent(Screen::SOUNDEVENT_NEXTBUTTON);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Get the currently selected profile.
///
/// \return The current profile, or NULL if there's an error.
///////////////////////////////////////////////////////////////////////////////////////////////////
ts_OnlineAccount *ScreenEORLogin::GetCurrentProfile()
{
    GuiCycleButton* pCycleButton = (GuiCycleButton*) 
		GetInterface(ID_LOGIN_PROFILE_LIST, GuiCycleButton::ClassnameStatic());

    if (pCycleButton && pCycleButton->GetItemCount() > 0)
    {
		GuiListBox::Item* pItem = pCycleButton->GetItem(pCycleButton->CurrentItem());
		if( pItem )
		{
			return (ts_OnlineAccount *)pItem->GetParam();
		}
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Get the currently selected profile's offset.
///
/// \return The current profile's offset, or -1 if there's an error.
///////////////////////////////////////////////////////////////////////////////////////////////////
int ScreenEORLogin::GetCurrentProfileOffset()
{
    GuiCycleButton* pCycleButton = (GuiCycleButton*) 
		GetInterface(ID_LOGIN_PROFILE_LIST, GuiCycleButton::ClassnameStatic());

    if (pCycleButton && pCycleButton->GetItemCount() > 0)
    {
        return pCycleButton->CurrentItem();
    }

    return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Update the screen with information for the currently selected profile
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORLogin::_UpdateScreen()
{
	if (!GetCurrentProfile())
	{
		m_pScreen->GetTextSprite(ID_LOGIN_PROFILE_LIST)->SetText("");
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Clear all profiles from the screen
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORLogin::Clear()
{
    GuiCycleButton* pCycleButton = (GuiCycleButton*)GetInterface(ID_LOGIN_PROFILE_LIST, GuiCycleButton::ClassnameStatic());

    if (pCycleButton)
    {
        pCycleButton->ClearItems();
    }

    _UpdateScreen();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Select the given offset
///
/// \param Offset The profile to select
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORLogin::SelectProfile(int Offset)
{
    GuiCycleButton* pCycleButton = (GuiCycleButton*)GetInterface(ID_LOGIN_PROFILE_LIST, GuiCycleButton::ClassnameStatic());

    if (pCycleButton)
    {
        pCycleButton->SelectItem(Offset);
		g_GlobalProfile.CurrentAccount = Offset;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Refreshes the account information and toggles the buttons
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORLogin::UpdateAccounts()
{
    int numAccounts = 0;
	Clear();

    // See how many active accounts there are right now
    for (int i = 0; i < MAX_ONLINE_ACCOUNTS; ++i)
    {
        if (g_GlobalProfile.OnlineAccounts[i].InUse)
        {
			AddProfile(&g_GlobalProfile.OnlineAccounts[i], i);
            ++numAccounts;
        }
    }

    // Can only create or recover account if < 4 accounts
    EnableButton(ID_LOGIN_CREATE_ACCOUNT,	numAccounts < MAX_ONLINE_ACCOUNTS);
    EnableButton(ID_LOGIN_RECOVER_ACCOUNT,	numAccounts < MAX_ONLINE_ACCOUNTS);

    // Can only login, edit accounts, delete accounts if there are accounts
    EnableButton(ID_LOGIN_LOGIN,		numAccounts > 0);
    EnableButton(ID_LOGIN_EDIT_ACCOUNT,	numAccounts > 0);
    EnableButton(ID_LOGIN_DELETE,		numAccounts > 0);

	if( _numAccounts != numAccounts )
	{
		if( _numAccounts == -1 || numAccounts == 0 || numAccounts == MAX_ONLINE_ACCOUNTS )
		{
			SelectInitialButton();
		}
		_numAccounts = numAccounts;
	}

	SelectProfile( g_GlobalProfile.CurrentAccount );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
/* virtual */ void ScreenEORLogin::OnKeyboardInput(const char* keyboardString)
{
	_status = PASSWORD_ENTERED;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Verify that the screen is displaying the correct account information
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORLogin::_VerifyOnlineAccounts()
{
	// Verify our selected account is valid
	if( 0 > g_GlobalProfile.CurrentAccount || g_GlobalProfile.CurrentAccount >= MAX_ONLINE_ACCOUNTS )
		g_GlobalProfile.CurrentAccount = 0;

	if( !g_GlobalProfile.OnlineAccounts[g_GlobalProfile.CurrentAccount].InUse )
	{
		for( int i = 0; i < MAX_ONLINE_ACCOUNTS; ++i )
		{
			if( g_GlobalProfile.OnlineAccounts[i].InUse )
			{
				g_GlobalProfile.CurrentAccount = i;
				break;
			}
		}
	}
	UpdateAccounts();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/// Attempts to reset the screen.  This is to fix errors where the screen updated while a command
/// was processing.
//////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORLogin::Reset( void )
{
	_status = RUNNING ;
	SelectInitialButton();
}