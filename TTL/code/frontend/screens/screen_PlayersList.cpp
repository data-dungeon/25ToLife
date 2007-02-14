#include "TTLPCH.h"
#include "frontend/screens/screen_PlayersList.h"
#include "data/screens/FriendsAndPlayers.h"
#include "OSI/OSI.hpp"
#include "GameHelper/Translator.h"

enum
{
	PPCHOICE_SENDFRIENDREQUEST		= 0,
	PPCHOICE_SENDCLANINVITE,
	PPCHOICE_SENDMATCHINVITE,
	PPCHOICE_CANCEL,
};

const char playersListPopupStrings[6][50] =
{
	"Send Friend Request",
	"Send Clan Invite",
	"Send Match Invite",
	"Cancel"
};

GuiListBox* ScreenPlayersList::GetListBox( void )
{
	return (GuiListBox*) GetInterface( ID_PLAYERSLIST_MENURECT, GuiListBox::ClassnameStatic() );
}


///-------------------------------------------------------------------------------
/// Returns the current screen state.  Sets the current screen state to
/// RUNNING to prevent staying in the last state upon re-entry.
///-------------------------------------------------------------------------------
ScreenPlayersList::ScreenState ScreenPlayersList::GetScreenState( void )
{
	ScreenState screenState = _screenState ;
	_screenState			= RUNNING ;
	return screenState ;
}

void ScreenPlayersList::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	SetHelpRect( ID_PLAYERSLIST_HELPRECT );

	GuiListBox* pListBox = CreateListBox( ID_PLAYERSLIST_MENURECT );

	if( pListBox )
	{
		pListBox->InitControl( this );

		pListBox->InitItem( GetTextSprite( ID_PLAYERSLIST_PLAYER1 ) );
		pListBox->InitItem( GetTextSprite( ID_PLAYERSLIST_PLAYER2 ) );
		pListBox->InitItem( GetTextSprite( ID_PLAYERSLIST_PLAYER3 ) );
		pListBox->InitItem( GetTextSprite( ID_PLAYERSLIST_PLAYER4 ) );
		pListBox->InitItem( GetTextSprite( ID_PLAYERSLIST_PLAYER5 ) );
		pListBox->InitItem( GetTextSprite( ID_PLAYERSLIST_PLAYER6 ) );
		pListBox->InitItem( GetTextSprite( ID_PLAYERSLIST_PLAYER7 ) );
		pListBox->InitItem( GetTextSprite( ID_PLAYERSLIST_PLAYER8 ) );
		pListBox->InitCursor( GetSprite( ID_PLAYERSLIST_CURSOR ) );

	// Hide PS2 help bar on PC
#ifdef DIRECTX_PC
	GetSprite(ID_PLAYERSLIST_HELPRECT)->HideAll();
#endif
	}

	_screenState = RUNNING ;
	return;
}


///---------------------------------------------------------------------
/// This takes a list of OSIPlayer objects and populates the listbox
/// with pertinent information for each player.
///---------------------------------------------------------------------
void ScreenPlayersList::SetRecentPlayersList( OSIPlayerList& recentPlayersList )
{
	GuiListBox*	pListBox	= GetListBox();
	int			count		= 0 ;
	OSIchar nameBuffer[ OSI_PLAYER_FULL_NAME_LENGTH ];

	pListBox->ClearItems();
	for ( recentPlayersList.begin(); recentPlayersList.getCurrent() != NULL; recentPlayersList.next() )
	{
		OSIPlayer* player = recentPlayersList.getCurrent();
		if ( player )
		{
			pListBox->AddItem( player->GetFullName( nameBuffer ), count, 0 );
			count++ ;
		}
	}

	if ( count )
	{
		pListBox->SelectInitialItem();
	}
}

Sprite* ScreenPlayersList::GetFriendIcon( int PlayerIndex )
{
	GuiListBox* pListBox = GetListBox();
	Sprite* pFriendIcon = 0;
	Text* pDisplayItem;

	if( pListBox )
	{
		pDisplayItem = pListBox->GetDisplayItem(PlayerIndex + pListBox->GetTopItem());

		if( pDisplayItem )
			pFriendIcon = GetSprite( ID_PLAYERSLIST_FRIENDICON, pDisplayItem );
	}

	return pFriendIcon;
}

void ScreenPlayersList::ShowFriendIcon( int PlayerIndex, bool Show )
{
	Sprite* pFriendIcon = GetFriendIcon( PlayerIndex );

	if( pFriendIcon )
	{
		if( Show )
			pFriendIcon->Show();
		else
			pFriendIcon->Hide();
	}

	return;
}

int ScreenPlayersList::CurrentSelection( void )
{
	GuiListBox* pListBox = GetListBox();

	if( !pListBox )
		return -1;

	return pListBox->CurrentSelection();
}

void ScreenPlayersList::OnScreenIntro( void )
{
	_selectedPlayerIndex		= 0 ;
	_screenState				= RUNNING ;
	ScreenBaseClass::OnScreenIntro();

	Printf( ID_PLAYERSLIST_CURRENTPROFILE, g_translator.Translate( "Current Profile: %s" ), g_playerProfile.GetAccountName() );

	return;
}

void ScreenPlayersList::OnScreenUpdate( void )
{
	ScreenBaseClass::OnScreenUpdate();

	GuiListBox* pListBox = GetListBox();

	bool RecentPlayers = false;

	if( pListBox )
	{
		int n = pListBox->GetItemDisplayCount();
		int i;

		for( i = 0; i < n; i++ )
			ShowFriendIcon( i, false );

		n = pListBox->GetItemCount();

		if( n > 0 )
			RecentPlayers = true;

		for( i = 0; i < n; i++ )
		{
			//===================================================================
			// TODO: Do it for real, get the player, check his friend status
			//       and show the friend icon.
			//ShowFriendIcon( i, true or false );
			//===================================================================
		}
	}

	ShowSprite( ID_PLAYERSLIST_CURSOR, RecentPlayers );
	ShowSprite( ID_PLAYERSLIST_NOPLAYERSMSG, !RecentPlayers );

	return;
}

void ScreenPlayersList::OnCursorUp( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int OldPos = pListBox->CurrentItem();

		pListBox->OnCursorUp();

		int NewPos = pListBox->CurrentItem();

		if( OldPos != NewPos )
			PlaySoundEvent( Screen::SOUNDEVENT_PREVBUTTON );

		OnCursorUpdate();
	}

	return;
}

void ScreenPlayersList::OnCursorDown( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int OldPos = pListBox->CurrentItem();

		pListBox->OnCursorDown();

		int NewPos = pListBox->CurrentItem();

		if( OldPos != NewPos )
			PlaySoundEvent( Screen::SOUNDEVENT_NEXTBUTTON );

		OnCursorUpdate();
	}

	return;
}

void ScreenPlayersList::OnCursorUpdate( void )
{
	ScreenHandler* pScreenHandler = GetScreenHandler( "SCREEN_ID_PLAYERSPOPUP" );

	if( pScreenHandler && 0 == strcmp( pScreenHandler->Classname(), ScreenPlayersPopup::ClassnameStatic() ) )
	{
		ScreenPlayersPopup* pPlayersPopup = (ScreenPlayersPopup*) pScreenHandler;

		int CurSel = CurrentSelection();
		//pPlayersPopup->SetTitle( GetPlayerName(CurSel) );
		pPlayersPopup->SetTitle( "Selected Player Name" );
	}

	return;
}

//=============================================================================
// ScreenPlayersList::OnButtonOK
//=============================================================================
void ScreenPlayersList::OnButtonOK( void )
{
	GuiListBox *pListBox = GetListBox();

	if(pListBox == NULL || pListBox->GetItemCount() == 0)
	{
		return;
	}

	_selectedPlayerIndex = pListBox->CurrentSelection();
	_screenState = PLAYER_SELECTED;
}


//============================================================================
//
//============================================================================
void ScreenPlayersList::OnButtonCancel( void )
{
	_screenState = CANCEL ;
}


//=============================================================================
// ScreenPlayersList::OnPopupShow
//=============================================================================

void ScreenPlayersList::OnPopupShow( void )
{
	ScreenBaseClass::OnPopupShow();

	Sprite* pMenuRect = GetSprite( ID_PLAYERSLIST_MENURECT );

	if( pMenuRect )
	{
		pMenuRect->InitAnim();

		pMenuRect->m_anim->familycolor.a.SetFrame( 0.0f, pMenuRect->m_state.familycolor.a / 255.0f );
		pMenuRect->m_anim->familycolor.a.SetFrame( 0.3f, 0.333f );
		pMenuRect->m_anim->familycolor.a.Deccelerate( true );
		pMenuRect->m_anim->familycolor.a.Play( false );
	}

	return;
}

//=============================================================================
// ScreenPlayersList::OnPopupHide
//=============================================================================

void ScreenPlayersList::OnPopupHide( void )
{
	ScreenBaseClass::OnPopupHide();

	Sprite* pMenuRect = GetSprite( ID_PLAYERSLIST_MENURECT );

	if( pMenuRect )
	{
		pMenuRect->InitAnim();
		pMenuRect->m_anim->familycolor.a.SetFrame( 0.0f, pMenuRect->m_state.familycolor.a / 255.0f );
		pMenuRect->m_anim->familycolor.a.SetFrame( 0.3f, 1.0f );
		pMenuRect->m_anim->familycolor.a.Accelerate( true );
		pMenuRect->m_anim->familycolor.a.Play( false );
	}
	return;
}


//=============================================================================
// ScreenPlayersPopup::GetListBox
//=============================================================================
GuiListBox* ScreenPlayersPopup::GetListBox( void )
{
	return (GuiListBox*) GetInterface( ID_PLAYERSPOPUP_MENURECT, GuiListBox::ClassnameStatic() );
}


//=============================================================================
// ScreenPlayersPopup::SetTitle
//=============================================================================
void ScreenPlayersPopup::SetTitle( const char* pText )
{
	SetText( ID_PLAYERSPOPUP_TITLE, pText );
}

//=============================================================================
// ScreenPlayersPopup::OnInitialize
//=============================================================================
void ScreenPlayersPopup::OnInitialize( void )
{
	GuiListBox* pListBox = CreateListBox( ID_PLAYERSPOPUP_MENURECT );

	if( pListBox )
	{
		pListBox->InitControl( this );

		pListBox->InitItem( GetTextSprite( ID_PLAYERSPOPUP_MENU1 ) );
		pListBox->InitItem( GetTextSprite( ID_PLAYERSPOPUP_MENU2 ) );
		pListBox->InitItem( GetTextSprite( ID_PLAYERSPOPUP_MENU3 ) );
		pListBox->InitItem( GetTextSprite( ID_PLAYERSPOPUP_MENU4 ) );
		pListBox->InitItem( GetTextSprite( ID_PLAYERSPOPUP_MENU5 ) );
		pListBox->InitItem( GetTextSprite( ID_PLAYERSPOPUP_MENU6 ) );
		pListBox->InitCursor( GetSprite( ID_PLAYERSPOPUP_CURSOR ) );
	}

	return;
}


void ScreenPlayersPopup::OnCursorUp( void )
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



void ScreenPlayersPopup::OnCursorDown( void )
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
/// Called when the OK button is pressed
///////////////////////////////////////////////////////////////////////////////////////////////////
/* virtual */ void ScreenPlayersPopup::OnButtonOK( void )
{
    GuiListBox* pListBox = GetListBox();

	if(pListBox == NULL || pListBox->GetItemCount() == 0)
	{
		return;
	}

	switch( pListBox->GetItemId( pListBox->CurrentItem() ) )
	{
        case PPCHOICE_SENDFRIENDREQUEST:
            _screenState = SEND_FRIEND_REQUEST;
            break;

        case PPCHOICE_SENDCLANINVITE:
            _screenState = SEND_CLAN_INVITE;
            break;

        case PPCHOICE_SENDMATCHINVITE:
            _screenState = SEND_MATCH_INVITE;
            break;

        case PPCHOICE_CANCEL:
            _screenState = CANCEL;
            break;

		default:
			ScreenBasePopup::OnButtonOK();
			break;
    }
}


void ScreenPlayersPopup::SetOptionList( OSIClanRole myClanRole, bool isSelectedPlayerInMatch )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		pListBox->ClearItems();
		pListBox->AddItem( playersListPopupStrings[PPCHOICE_SENDFRIENDREQUEST],
			PPCHOICE_SENDFRIENDREQUEST, 0 );
		// Only display invite to clan option if user is in a clan and has permission
		if(myClanRole == OSI_CLAN_ROLE_LEADER || myClanRole == OSI_CLAN_ROLE_OFFICER)
		{
			pListBox->AddItem( playersListPopupStrings[PPCHOICE_SENDCLANINVITE],
				PPCHOICE_SENDCLANINVITE, 0 );
		}
		
		if ( OSI::getSingletonPtr()->isHost() && !isSelectedPlayerInMatch )
		{
			pListBox->AddItem( playersListPopupStrings[PPCHOICE_SENDMATCHINVITE],
				PPCHOICE_SENDMATCHINVITE, 0 );
		}

		pListBox->AddItem( playersListPopupStrings[PPCHOICE_CANCEL],
			PPCHOICE_CANCEL, 0 );
	}
}
