#include "TTLPCH.h"
#include "frontend/screens/screen_FriendsPopup.h"
#include "data/screens/FriendsAndPlayers.h"

#include "OSI/OSI.hpp"

//=============================================================================
// Enum and associated string for buttons in the popup
//=============================================================================

enum
{
	PPCHOICE_ACCEPT_FRIEND_REQUEST		= 0,
	PPCHOICE_DENY_FRIEND_REQUEST,
	PPCHOICE_BLOCK_FRIEND_REQUEST,
	PPCHOICE_CANCEL_FRIEND_REQUEST,
	PPCHOICE_REMOVE_FRIEND,
	PPCHOICE_JOIN_FRIEND_GAME,
	PPCHOICE_INVITE_FRIEND_TO_GAME,
	PPCHOICE_SEND_CLAN_INVITE,
	PPCHOICE_FRIENDS_BACK,
};

const char *friendsPopupStrings[50] =
{
	"Accept",
	"Deny",
	"Block",
	"Cancel Request",
	"Remove",
	"Join",
	"Invite to game",
	"Send Clan Invite",
	"Back"
};

//=============================================================================
// Get the list box associated with the popup
//=============================================================================
GuiListBox* ScreenFriendsPopup::GetListBox( void )
{
	return (GuiListBox*) GetInterface( ID_FRIENDS_POPUP_MENURECT, GuiListBox::ClassnameStatic() );
}


//=============================================================================
// Called when the screen is allocated
//=============================================================================
/* virtual */
void ScreenFriendsPopup::OnInitialize( void )
{
	GuiListBox* pListBox = CreateListBox( ID_FRIENDS_POPUP_MENURECT );

	if( pListBox )
	{
		pListBox->InitControl( this );

		pListBox->InitItem( GetTextSprite( ID_FRIENDS_POPUP_MENU1 ) );
		pListBox->InitItem( GetTextSprite( ID_FRIENDS_POPUP_MENU2 ) );
		pListBox->InitItem( GetTextSprite( ID_FRIENDS_POPUP_MENU3 ) );
		pListBox->InitItem( GetTextSprite( ID_FRIENDS_POPUP_MENU4 ) );
		pListBox->InitItem( GetTextSprite( ID_FRIENDS_POPUP_MENU5 ) );
		pListBox->InitCursor( GetSprite( ID_FRIENDS_POPUP_CURSOR ) );
	}

	ScreenBaseClass::OnInitialize();
}

//=============================================================================
// Called when the up button is pressed while the screen is active
//=============================================================================
/* virtual */
void ScreenFriendsPopup::OnCursorUp( void )
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
void ScreenFriendsPopup::OnCursorDown( void )
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

//=============================================================================
// Called when the ok button is pressed while the screen is active
//=============================================================================
/* virtual */
void ScreenFriendsPopup::OnButtonOK( void )
{
    GuiListBox* pListBox = GetListBox();

	if(pListBox == NULL || pListBox->GetItemCount() == 0)
	{
		return;
	}

	switch( pListBox->GetItemId( pListBox->CurrentItem() ) )
	{
        case PPCHOICE_ACCEPT_FRIEND_REQUEST:
            _screenState = ACCEPT_FRIEND_REQUEST;
            break;

        case PPCHOICE_DENY_FRIEND_REQUEST:
            _screenState = DENY_FRIEND_REQUEST;
            break;

		case PPCHOICE_BLOCK_FRIEND_REQUEST:
            _screenState = BLOCK_FRIEND_REQUEST;
            break;

		case PPCHOICE_CANCEL_FRIEND_REQUEST:
            _screenState = CANCEL_FRIEND_REQUEST;
            break;

		case PPCHOICE_REMOVE_FRIEND:
            _screenState = REMOVE_FRIEND;
            break;

		case PPCHOICE_JOIN_FRIEND_GAME:
            _screenState = JOIN_FRIEND_GAME;
            break;

		case PPCHOICE_INVITE_FRIEND_TO_GAME:
            _screenState = INVITE_FRIEND_TO_GAME;
            break;

		case PPCHOICE_SEND_CLAN_INVITE:
            _screenState = SEND_CLAN_INVITE;
            break;

        case PPCHOICE_FRIENDS_BACK:
            _screenState = BACK;
            break;

		default:
			ScreenBasePopup::OnButtonOK();
			break;
    }
}

//=============================================================================
// Called to set what the screen displays as it's options
//=============================================================================
void ScreenFriendsPopup::SetScreenRole( OSIPlayer* selectedPlayer )
{
	// Set screens role based on what type of friend request
	if (selectedPlayer->isAFriend)
	{
		_screenRole = FRIEND_REQUEST_CONFIRMED;
	}
	else if (selectedPlayer->haveRequestedTheirFriendship)
	{
		_screenRole = FRIEND_REQUEST_OUTGOING;
	}
	else if (selectedPlayer->hasRequestedMyFriendship)
	{
		_screenRole = FRIEND_REQUEST_INCOMING;
	}

	GuiListBox* pListBox = GetListBox();
	if( pListBox == 0 ) return;

	pListBox->ClearItems();
	switch( _screenRole )
	{
		case FRIEND_REQUEST_INCOMING:
			pListBox->AddItem( friendsPopupStrings[PPCHOICE_ACCEPT_FRIEND_REQUEST],
				PPCHOICE_ACCEPT_FRIEND_REQUEST, 0 );
			pListBox->AddItem( friendsPopupStrings[PPCHOICE_DENY_FRIEND_REQUEST],
				PPCHOICE_DENY_FRIEND_REQUEST, 0 );
			pListBox->AddItem( friendsPopupStrings[PPCHOICE_BLOCK_FRIEND_REQUEST],
				PPCHOICE_BLOCK_FRIEND_REQUEST, 0 );
			break;

		case FRIEND_REQUEST_OUTGOING:
			pListBox->AddItem( friendsPopupStrings[PPCHOICE_CANCEL_FRIEND_REQUEST],
				PPCHOICE_CANCEL_FRIEND_REQUEST, 0 );
			break;

		case FRIEND_REQUEST_CONFIRMED:
			pListBox->AddItem( friendsPopupStrings[PPCHOICE_REMOVE_FRIEND],
				PPCHOICE_REMOVE_FRIEND, 0 );
			if( selectedPlayer->state.isOnline && OSI::getSingletonPtr()->isHost() )
			{
				pListBox->AddItem( friendsPopupStrings[PPCHOICE_INVITE_FRIEND_TO_GAME],
					PPCHOICE_INVITE_FRIEND_TO_GAME, 0 );
			}
			if( selectedPlayer->currentMatchList && selectedPlayer->currentMatchList->count() > 0 )
			{
				selectedPlayer->currentMatchList->begin();
				OSIMatch* match = selectedPlayer->currentMatchList->getCurrent();
				if ( !OSI::getSingletonPtr()->isPlayerInMatch( selectedPlayer ) &&
					match->options.totalConnectedPlayers < match->maximumPlayers )
				{
					pListBox->AddItem( friendsPopupStrings[PPCHOICE_JOIN_FRIEND_GAME], PPCHOICE_JOIN_FRIEND_GAME, 0 );
				}
			}
			break;
	}
	// Does this player have the same clan as me?
	const OSIPlayer* const player = OSI::getSingletonPtr()->getActivePlayer();
	const bool isNotInSameClan = player->clanInfo.tag[0] &&
		OSIstrncmp( selectedPlayer->clanInfo.tag, player->clanInfo.tag, OSIClanInfo::MAX_TAG_LENGTH ) != 0;
	const bool hasPermission = player->clanRole == OSI_CLAN_ROLE_LEADER || player->clanRole == OSI_CLAN_ROLE_OFFICER;
	if( isNotInSameClan && hasPermission ) {
		pListBox->AddItem( friendsPopupStrings[PPCHOICE_SEND_CLAN_INVITE],
			PPCHOICE_SEND_CLAN_INVITE, 0 );
	}
	pListBox->AddItem( friendsPopupStrings[PPCHOICE_FRIENDS_BACK], PPCHOICE_FRIENDS_BACK, 0 );

	pListBox->SelectItem( PPCHOICE_FRIENDS_BACK );
}