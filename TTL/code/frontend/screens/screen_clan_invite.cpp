#include "TTLPCH.h"
#include "OSI/OSI.hpp"
#include "frontend/inputdriver.h"
#include "screen_clan_invite.h"
#include "data/screens/Multiplayer.h"
#include "frontend/frontend.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is first loaded
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenClanInvite::OnInitialize(void)
{
    // Set up the table
    GuiTable* pTable = CreateTable( ID_CLAN_INVITES_GROUP );

    if( pTable )
	{
		pTable->InitControl( this );

		pTable->InitTable( 6, 4 );
		pTable->SetCursorMode( GuiTable::RowsOnly );

        pTable->InitItem( 0, 0, GetTextSprite( ID_CLANINVITES_C0R0 ) );
		pTable->InitItem( 1, 0, GetTextSprite( ID_CLANINVITES_C0R1 ) );
		pTable->InitItem( 2, 0, GetTextSprite( ID_CLANINVITES_C0R2 ) );
		pTable->InitItem( 3, 0, GetTextSprite( ID_CLANINVITES_C0R3 ) );
		pTable->InitItem( 4, 0, GetTextSprite( ID_CLANINVITES_C0R4 ) );
		pTable->InitItem( 5, 0, GetTextSprite( ID_CLANINVITES_C0R5 ) );

        pTable->InitItem( 0, 1, GetTextSprite( ID_CLANINVITES_C1R0 ) );
		pTable->InitItem( 1, 1, GetTextSprite( ID_CLANINVITES_C1R1 ) );
		pTable->InitItem( 2, 1, GetTextSprite( ID_CLANINVITES_C1R2 ) );
		pTable->InitItem( 3, 1, GetTextSprite( ID_CLANINVITES_C1R3 ) );
		pTable->InitItem( 4, 1, GetTextSprite( ID_CLANINVITES_C1R4 ) );
		pTable->InitItem( 5, 1, GetTextSprite( ID_CLANINVITES_C1R5 ) );

        pTable->InitItem( 0, 2, GetTextSprite( ID_CLANINVITES_C2R0 ) );
		pTable->InitItem( 1, 2, GetTextSprite( ID_CLANINVITES_C2R1 ) );
		pTable->InitItem( 2, 2, GetTextSprite( ID_CLANINVITES_C2R2 ) );
		pTable->InitItem( 3, 2, GetTextSprite( ID_CLANINVITES_C2R3 ) );
		pTable->InitItem( 4, 2, GetTextSprite( ID_CLANINVITES_C2R4 ) );
		pTable->InitItem( 5, 2, GetTextSprite( ID_CLANINVITES_C2R5 ) );

        pTable->InitItem( 0, 3, GetTextSprite( ID_CLANINVITES_C3R0 ) );
		pTable->InitItem( 1, 3, GetTextSprite( ID_CLANINVITES_C3R1 ) );
		pTable->InitItem( 2, 3, GetTextSprite( ID_CLANINVITES_C3R2 ) );
		pTable->InitItem( 3, 3, GetTextSprite( ID_CLANINVITES_C3R3 ) );
		pTable->InitItem( 4, 3, GetTextSprite( ID_CLANINVITES_C3R4 ) );
		pTable->InitItem( 5, 3, GetTextSprite( ID_CLANINVITES_C3R5 ) );
    }

    UseHighlighting(true);

	SetInitialButton(ID_CLANINVITES_INBOX);
	SetNextButton(ID_CLANINVITES_SENT_LIST);
	SetNextButton(ID_CLANINVITES_SEND);
	SetNextButton(ID_CLANINVITES_BACK);
	SetCursor(ID_CLANINVITES_CURSOR);
	if (pTable)
	{
		pTable->InitCursor( GetCursor() );

		// Hide PS2 help bar on PC
#ifdef DIRECTX_PC
		GetSprite(ID_CLANINVITES_HELPGROUP)->HideAll();
#endif
	}

	_screenRole = INBOX_MODE;
	_clanRole = OSI_CLAN_ROLE_UNKNOWN;

	ScreenBaseClass::OnInitialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is displayed
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanInvite::OnScreenIntro( void )
{
    _status = RUNNING;

    ScreenBaseClass::OnScreenIntro();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when user selects a button
///
/// \param Command ID of the selected button
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanInvite::OnCommand( int Command )
{
    switch (Command)
    {
		case ID_CLANINVITES_INBOX:
            _status = INVITE_INBOX;
            break;

		case ID_CLANINVITES_SENT_LIST:
            _status = INVITE_SENT_INVITE_LIST;
            break;

        case ID_CLANINVITES_SEND:
            _status = INVITE_ENTER_INVITEE_NAME;
            break;

        case ID_CLANINVITES_BACK:
            _status = BACK;
            break;
    }

    ScreenBaseClass::OnCommand(Command);
}

void ScreenClanInvite::OnButtonOK( void )
{
	GuiTable* pTable = GetInviteTable();
	// If we have pressed the button over our table
	if(pTable != NULL && GetCurrentButtonId() == 0) {
		// Save off our current highlighted invitation
		OSIClanInvitationList& receivedInviteList = OSI::getSingletonPtr()->getClanInvitationList();
		OSIClanInvitationList& sentInviteList = OSI::getSingletonPtr()->getClanSentInvitationList();
		OSIClanInvitationList& inviteList = ( _screenRole == INBOX_MODE ? receivedInviteList : sentInviteList );
		int row, column;
		pTable->GetCursorPos(row, column);
		_clanInvite = *inviteList[row];

		_status = INVITE_SELECTED;
	}

	ScreenBaseClass::OnButtonOK();
}

void ScreenClanInvite::OnButtonCancel()
{
	_status = BACK;

	GuiTable* pTable = GetInviteTable();
	if( pTable )
	{
		pTable->SetCursorPos( 0, 0 );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when a popup window closes (on this screen, the virtual keyboard)
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenClanInvite::OnKeyboardInput( const char* keyboardString )
{
	// We have closed the keyboard popup for sending an invitation
	OSIstrncpy(_clanInvitee, keyboardString, OSI_PLAYER_NAME_LENGTH );

	// If we didn't fill out the name to send an invitation to or hit cancel, do nothing
	if( ('\0' == _clanInvitee[0]) )
    {
		_status = RUNNING;
	}
	else
    {
		_status = INVITE_SEND;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Get the table containing clan members
///
/// \return A pointer to the table object with the clan members in it
///////////////////////////////////////////////////////////////////////////////////////////////////
GuiTable* ScreenClanInvite::GetInviteTable( void )
{
	GuiTable* pTable = 0;
	Sprite* pSprite = GetSprite( ID_CLAN_INVITES_GROUP );

	if( pSprite )
    {
		pTable = (GuiTable*) pSprite->GetInterface( GuiTable::ClassnameStatic() );
    }

	return pTable;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when up is pressed: move the selection
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanInvite::OnCursorUp()
{
    if (!m_pScreen) return;
    GuiTable* pTable = GetInviteTable();

    if( IsFirstButtonSelected() )
    {
        // If there's a table, select the last item
        if (pTable && pTable->GetRows() > 0)
        {
            int Row, Col;

            pTable->GetCursorPos(Row, Col);
            pTable->SetCursorPos(pTable->GetRows() - 1, Col);
			pTable->MoveCursor( 0, 0 );
            SelectButton(ID_CLAN_INVITES_GROUP);
            PlaySoundEvent(Screen::SOUNDEVENT_PREVBUTTON);
        }
        // Otherwise, just go to the previous button
        else
        {
            SelectPrevButton();
        }
    }
    else if( IsButtonSelected() )
    {
        SelectPrevButton();
    }
    // In table
    else
    {
        int Row, Col;

        if( !pTable ) { SelectPrevButton(); return; }

        pTable->GetCursorPos(Row, Col);

        // If top item, select last button
        if( pTable->GetRows() == 0 || Row == 0 )
        {
			pTable->SetCursorPos( -1, 0 );
            SelectButton( ID_CLANINVITES_BACK );
        }
        // Else select the previous item
        else
        {
            //pTable->SetCursorPos(Row - 1, Col);
			pTable->MoveCursor( 0, -1 );
        }

        PlaySoundEvent(Screen::SOUNDEVENT_PREVBUTTON);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when down is pressed: move the selection
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanInvite::OnCursorDown()
{
    if (!m_pScreen) return;
    GuiTable* pTable = GetInviteTable();

    if( IsLastButtonSelected() )
    {
        // If there's a table, select the first item
        if (pTable && pTable->GetRows() > 0)
        {
            int Row, Col;

            pTable->GetCursorPos(Row, Col);
            pTable->SetCursorPos(0, Col);
			pTable->MoveCursor( 0, 0 );
            SelectButton(ID_CLAN_INVITES_GROUP);
            PlaySoundEvent(Screen::SOUNDEVENT_NEXTBUTTON);
        }
        // Otherwise, just go to the next button
        else
        {
            SelectNextButton();
        }
    }
	else if( IsButtonSelected() )
    {
        SelectNextButton();
    }
    // In table
    else
    {
        int Row, Col;

        if(!pTable) { SelectNextButton(); return; }

        pTable->GetCursorPos(Row, Col);

        // If bottom item, select first button
        if (pTable->GetRows() == 0 || Row == pTable->GetRows() - 1)
        {
			pTable->SetCursorPos( -1, 0 );
			SelectInitialButton();
        }
        // Else select the next item in the table
        else
        {
            //pTable->SetCursorPos(Row + 1, Col);
			pTable->MoveCursor( 0, 1 );
        }

        PlaySoundEvent(Screen::SOUNDEVENT_NEXTBUTTON);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Catch the shoulder buttons and scroll the list if needed.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanInvite::OnButtonPressed(int Button)
{
	GuiTable* pTable = GetInviteTable();
	if( !pTable ) return;

    switch( Button )
	{
		case FluffControlMapper::L1:
			pTable->PageUp();
			break;
		case FluffControlMapper::R1:
			pTable->PageDown();
			break;
	}
	ScreenBaseClass::OnButtonPressed(Button);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanInvite::OnScreenRefresh()
{
	GuiTable*	pTable	= GetInviteTable();
	OSI*		osi		= OSI::getSingletonPtr();
	if( !pTable || !osi )
	{
		return;
	}

	pTable->DeleteAllItems();

	OSIchar buffer[ 80 ];

	if( _screenRole == INBOX_MODE )
	{
		OSIClanInvitationList& inviteList = OSI::getSingletonPtr()->getClanInvitationList();

		unsigned int inviteCount = inviteList.count();
		inviteList.begin();
		for ( unsigned int row = 0; row < inviteCount; ++row )
		{
			const OSIClanInvitation* const invite = inviteList.getCurrent();
			OSIchar nameBuffer[ OSIClanInfo::MAX_FULL_NAME_LENGTH ];
			GetFullPlayerName( nameBuffer, invite->clanInfo.tag, invite->clanInfo.name );
			pTable->AddItem( nameBuffer, row, 0 );

			OSIsnprintf( buffer, NUM_ARRAY_ELEMENTS(buffer), "%i", invite->clanInfo.memberCount );
			pTable->SetItem( row, 1, buffer );
			
			int rank = int(invite->clanInfo.stats.getStat( OSIClanStats::CLANSTAT_RANK ));
			if( rank > 0 )	{ OSIsprintf( buffer, "%i", rank ); }
			else			{ OSIsprintf( buffer, "" ); }
			pTable->SetItem( row, 2, buffer );
			
			pTable->SetItem( row, 3, invite->senderName );

			inviteList.next();
		}
		if( inviteCount == 0 && !IsButtonSelected() )
		{
			pTable->SetCursorPos( -1, 0 );
			SelectInitialButton();
		}
	}
	else
	{
		OSIClanInvitationList& sentInviteList = OSI::getSingletonPtr()->getClanSentInvitationList();

		unsigned int inviteCount = sentInviteList.count();
		sentInviteList.begin();
		for ( unsigned int row = 0; row < inviteCount; ++row )
		{
			const OSIClanInvitation* const invite = sentInviteList.getCurrent();
			pTable->AddItem( invite->receiverName, row, 0 );

			sentInviteList.next();
		}
		if( inviteCount == 0 && !IsButtonSelected() )
		{
			pTable->SetCursorPos( -1, 0 );
			SelectInitialButton();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the access level of the player that brought the screen up based on clan role
///
/// \param clanRole The clan role of the player that brought up the screen.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanInvite::SetAccessLevel(const OSIClanRole &clanRole)
{
	_clanRole = clanRole;

    // Show/hide menu items accordingly
    switch (clanRole)
    {
		case OSI_CLAN_ROLE_UNKNOWN:
		case OSI_CLAN_ROLE_SOLDIER:
			EnableButton(ID_CLANINVITES_SEND, false);
			break;
        case OSI_CLAN_ROLE_LEADER:
		case OSI_CLAN_ROLE_OFFICER:
			EnableButton(ID_CLANINVITES_SEND, true);
            break;

        default:
            ASSERTS(0, "Invalid clan role!");
    }
	GuiTable* pTable = GetInviteTable();
	if( pTable )
		pTable->SetCursorPos( -1, 0 );
	SelectInitialButton();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Change text and functionality of screen
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanInvite::SetScreenRole( const ScreenRole& screenRole )
{
	_screenRole = screenRole;

	switch( _screenRole )
	{
		case INBOX_MODE:
			SetText( ID_CLANINVITES_TITLE, "Incoming Clan Invitations" );
			SetText( ID_CLANINVITES_MSG, "You have been invited to join these clans:" );
			SetText( ID_CLANINVITES_CLAN, "Clan" );
			SetText( ID_CLANINVITES_MEMBER, "Members" );

			// Rank doesn't display on Xbox, don't show it.
			#ifdef _XBOX
			SetText( ID_CLANINVITES_RANK, "" );
			#else
			SetText( ID_CLANINVITES_RANK, "Rank" );
			#endif

			SetText( ID_CLANINVITES_PLAYER, "By Player" );
			break;

		case SENT_INVITE_LIST_MODE:
			SetText( ID_CLANINVITES_TITLE, "Outgoing Clan Invitations" );
			SetText( ID_CLANINVITES_MSG, "You have invited these people to join your clan:" );
			SetText( ID_CLANINVITES_CLAN, "Player Name" );
			SetText( ID_CLANINVITES_MEMBER, "" );
			SetText( ID_CLANINVITES_RANK, "" );
			SetText( ID_CLANINVITES_PLAYER, "" );
			break;
	}

	//EnableButton( ID_CLANINVITES_INBOX, _screenRole == SENT_INVITE_LIST_MODE );
	//EnableButton( ID_CLANINVITES_SENT_LIST, (_screenRole == INBOX_MODE) && (_clanRole != OSI_CLAN_ROLE_UNKNOWN) );
	//SelectInitialButton();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ScreenClanInvite::IsFirstButtonSelected()
{
	Button* pInitialButton = GetInitialButton();
	while( pInitialButton != NULL && !pInitialButton->Enabled() )
	{
		pInitialButton = pInitialButton->GetNextButton();

		// Guard against circular list
		if( pInitialButton == GetInitialButton() ) break;
	}

	return pInitialButton == GetCurrentButton();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ScreenClanInvite::IsButtonSelected()
{
	switch( GetCurrentButtonId() )
	{
		case ID_CLANINVITES_INBOX:
		case ID_CLANINVITES_SENT_LIST:
		case ID_CLANINVITES_SEND:
		case ID_CLANINVITES_BACK:
			return true;

		default:
			return false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ScreenClanInvite::IsLastButtonSelected()
{
	return m_pScreen->GetLastButton() == GetCurrentButton();
}
