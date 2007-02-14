#include "TTLPCH.h"
#include "frontend/screens/screen_clan_popup.h"
#include "data/screens/Multiplayer.h"
#include "frontend/inputdriver.h"
#include "OSI/OSIMatchList.hpp"

//=============================================================================
// Enum and associated string for buttons in the popup
//=============================================================================
enum
{
	PPCHOICE_PROMOTE		= 0,
	PPCHOICE_DEMOTE,
	PPCHOICE_KICK,
	PPCHOICE_TRANSFER_LEADERSHIP,
	PPCHOICE_JOIN,
	PPCHOICE_SEND_FRIEND_INVITE,
	PPCHOICE_CLAN_BACK,
};

const char *clanMemberListPopupStrings[50] =
{
	"Promote",
	"Demote",
	"Kick",
	"Transfer Leadership",
	"Join",
	"Send Friend Invite",
	"Back",
};

//=============================================================================
// Get the list box associated with the popup
//=============================================================================
GuiListBox* ScreenClanListPopup::GetListBox( void )
{
	return (GuiListBox*) GetInterface( ID_CLAN_LIST_POPUP_MENURECT, GuiListBox::ClassnameStatic() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time the screen is brought up.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanListPopup::OnScreenIntro( void )
{
    _status = RUNNING;

    ScreenBaseClass::OnScreenIntro();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is created.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanListPopup::OnInitialize( void )
{
    GuiListBox* pListBox = CreateListBox( ID_CLAN_LIST_POPUP_MENURECT );

	if( pListBox )
	{
		pListBox->InitControl( this );

		pListBox->InitItem( GetTextSprite( ID_CLAN_LIST_POPUP_MENU1 ) );
		pListBox->InitItem( GetTextSprite( ID_CLAN_LIST_POPUP_MENU2 ) );
		pListBox->InitItem( GetTextSprite( ID_CLAN_LIST_POPUP_MENU3 ) );
		pListBox->InitItem( GetTextSprite( ID_CLAN_LIST_POPUP_MENU4 ) );
		pListBox->InitItem( GetTextSprite( ID_CLAN_LIST_POPUP_MENU5 ) );
		pListBox->InitCursor( GetSprite( ID_CLAN_LIST_POPUP_CURSOR ) );
	}

    ScreenBaseClass::OnInitialize();
}

//=============================================================================
// Called when the up button is pressed while the screen is active
//=============================================================================
/* virtual */
void ScreenClanListPopup::OnCursorUp( void )
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
void ScreenClanListPopup::OnCursorDown( void )
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
void ScreenClanListPopup::OnButtonOK()
{
    GuiListBox* pListBox = GetListBox();

	if(pListBox == NULL || pListBox->GetItemCount() == 0)
	{
		return;
	}

	switch( pListBox->GetItemId( pListBox->CurrentItem() ) )
	{
        case PPCHOICE_PROMOTE:
            _status = PROMOTE;
            break;

        case PPCHOICE_DEMOTE:
            _status = DEMOTE;
            break;

		case PPCHOICE_KICK:
            _status = KICK;
            break;

		case PPCHOICE_TRANSFER_LEADERSHIP:
            _status = TRANSFER_LEADERSHIP;
            break;

		case PPCHOICE_JOIN:
            _status = JOIN;
            break;

		case PPCHOICE_SEND_FRIEND_INVITE:
            _status = SEND_FRIEND_INVITE;
            break;

        case PPCHOICE_CLAN_BACK:
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
void ScreenClanListPopup::SetAccessLevel(const OSIClanRole &clanRole, const OSIClanMembership &selectedMember)
{
	GuiListBox* pListBox = GetListBox();
	if( pListBox == 0 ) return;

	pListBox->ClearItems();
	switch (clanRole)
    {
        case OSI_CLAN_ROLE_LEADER:
			if( OSI_CLAN_ROLE_SOLDIER == selectedMember.role )
			{
				pListBox->AddItem( clanMemberListPopupStrings[PPCHOICE_PROMOTE],
					PPCHOICE_PROMOTE, 0 );
				pListBox->AddItem( clanMemberListPopupStrings[PPCHOICE_KICK],
					PPCHOICE_KICK, 0 );
			}
			else if( OSI_CLAN_ROLE_OFFICER == selectedMember.role )
			{
				pListBox->AddItem( clanMemberListPopupStrings[PPCHOICE_TRANSFER_LEADERSHIP],
					PPCHOICE_TRANSFER_LEADERSHIP, 0 );
				pListBox->AddItem( clanMemberListPopupStrings[PPCHOICE_DEMOTE],
					PPCHOICE_DEMOTE, 0 );
			}
            break;

        case OSI_CLAN_ROLE_OFFICER:
			// Officers can only kick out soldiers.
			if( OSI_CLAN_ROLE_SOLDIER == selectedMember.role )
			{
				pListBox->AddItem( clanMemberListPopupStrings[PPCHOICE_KICK],
					PPCHOICE_KICK, 0 );
			}
            break;

		case OSI_CLAN_ROLE_SOLDIER: break;

        default:
            ASSERTS(0, "Invalid clan role!");
    }

	if(!selectedMember.isFriend)
	{
		pListBox->AddItem( clanMemberListPopupStrings[PPCHOICE_SEND_FRIEND_INVITE],
			PPCHOICE_SEND_FRIEND_INVITE, 0 );
	}

	if(selectedMember.currentMatchList && selectedMember.currentMatchList->count() > 0)
	{
		selectedMember.currentMatchList->begin();
		OSIMatch* match = selectedMember.currentMatchList->getCurrent();
		if ( match->options.totalConnectedPlayers < match->maximumPlayers )
		{
			pListBox->AddItem( clanMemberListPopupStrings[PPCHOICE_JOIN], PPCHOICE_JOIN, 0 );
		}
	}

	pListBox->AddItem( clanMemberListPopupStrings[PPCHOICE_CLAN_BACK], PPCHOICE_CLAN_BACK, 0 );

	pListBox->SelectItem( PPCHOICE_CLAN_BACK );
}