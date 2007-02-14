#include "TTLPCH.h"
#include "frontend/screens/screen_clan_invite_popup.h"
#include "data/screens/Multiplayer.h"
#include "frontend/inputdriver.h"

//=============================================================================
// Enum and associated string for buttons in the popup
//=============================================================================
enum
{
	PPCHOICE_ACCEPT_INVITE		= 0,
	PPCHOICE_DECLINE_INVITE,
	PPCHOICE_BLOCK_INVITE,
	PPCHOICE_INVITE_BACK,
};

const char *clanInvitePopupStrings[50] =
{
	"Accept",
	"Decline",
	"Block",
	"Back"
};

//=============================================================================
// Get the list box associated with the popup
//=============================================================================
GuiListBox* ScreenClanInvitePopup::GetListBox( void )
{
	return (GuiListBox*) GetInterface( ID_CLAN_INVITE_POPUP_MENURECT, GuiListBox::ClassnameStatic() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time the screen is brought up.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanInvitePopup::OnScreenIntro( void )
{
    _status = RUNNING;

    ScreenBaseClass::OnScreenIntro();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is created.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanInvitePopup::OnInitialize( void )
{
	GuiListBox* pListBox = CreateListBox( ID_CLAN_INVITE_POPUP_MENURECT );

	if( pListBox )
	{
		pListBox->InitControl( this );

		pListBox->InitItem( GetTextSprite( ID_CLAN_INVITE_POPUP_MENU1 ) );
		pListBox->InitItem( GetTextSprite( ID_CLAN_INVITE_POPUP_MENU2 ) );
		pListBox->InitItem( GetTextSprite( ID_CLAN_INVITE_POPUP_MENU3 ) );
		pListBox->InitItem( GetTextSprite( ID_CLAN_INVITE_POPUP_MENU4 ) );
		pListBox->InitCursor( GetSprite( ID_CLAN_INVITE_POPUP_CURSOR ) );
	}

    ScreenBaseClass::OnInitialize();
}

//=============================================================================
// Called when the up button is pressed while the screen is active
//=============================================================================
/* virtual */
void ScreenClanInvitePopup::OnCursorUp( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int OldPos = pListBox->CurrentItem();

		pListBox->OnCursorUp();

		int NewPos = pListBox->CurrentItem();

		if( OldPos != NewPos )
			PlaySoundEvent( Screen::SOUNDEVENT_PREVBUTTON );
	}

	return;
}

//=============================================================================
// Called when the down button is pressed while the screen is active
//=============================================================================
/* virtual */
void ScreenClanInvitePopup::OnCursorDown( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int OldPos = pListBox->CurrentItem();

		pListBox->OnCursorDown();

		int NewPos = pListBox->CurrentItem();

		if( OldPos != NewPos )
			PlaySoundEvent( Screen::SOUNDEVENT_NEXTBUTTON );
	}

	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when a button is selected on the screen
///
/// \param Command The button pressed
///////////////////////////////////////////////////////////////////////////////////////////////////
/* virtual */
void ScreenClanInvitePopup::OnButtonOK()
{
	GuiListBox* pListBox = GetListBox();

	if(pListBox == NULL || pListBox->GetItemCount() == 0)
	{
		return;
	}

    switch( pListBox->GetItemId( pListBox->CurrentItem() ) )
	{
        case PPCHOICE_ACCEPT_INVITE:
            _status = ACCEPT;
            break;

        case PPCHOICE_DECLINE_INVITE:
            _status = DECLINE;
            break;

        case PPCHOICE_BLOCK_INVITE:
            _status = BLOCK;
            break;

        case PPCHOICE_INVITE_BACK:
            _status = BACK;
            break;

		default:
			ScreenBasePopup::OnButtonOK();
			break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the access level of the player that brought the screen up based on clan role
///
/// \param clanRole The clan role of the player that brought up the screen.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanInvitePopup::SetAccessLevel(const OSIClanRole &clanRole)
{
	GuiListBox* pListBox = GetListBox();
	if( pListBox == 0 ) return;

	pListBox->ClearItems();
	if( OSI_CLAN_ROLE_UNKNOWN == clanRole  )
	{
		pListBox->AddItem( clanInvitePopupStrings[PPCHOICE_ACCEPT_INVITE],
			PPCHOICE_ACCEPT_INVITE, 0 );
	}
	pListBox->AddItem( clanInvitePopupStrings[PPCHOICE_DECLINE_INVITE], PPCHOICE_DECLINE_INVITE, 0 );
	pListBox->AddItem( clanInvitePopupStrings[PPCHOICE_BLOCK_INVITE], PPCHOICE_BLOCK_INVITE, 0 );
	pListBox->AddItem( clanInvitePopupStrings[PPCHOICE_INVITE_BACK], PPCHOICE_INVITE_BACK, 0 );

	pListBox->SelectItem( PPCHOICE_INVITE_BACK );
}