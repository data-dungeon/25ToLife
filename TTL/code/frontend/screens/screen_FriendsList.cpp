#include "TTLPCH.h"
#include "OSI/OSIMatchList.hpp"
#include "frontend/inputdriver.h"
#include "screen_FriendsList.h"
#include "data/screens/FriendsAndPlayers.h"
#include "frontend/frontend.h"
#include "Network/OnlineSessionSearch.h"
#include "GameHelper/Translator.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is first loaded
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenFriendsList::OnInitialize(void)
{
    // Set up the table
    GuiTable* pTable = CreateTable( ID_FRIENDSLIST_GROUP );

    UseHighlighting(true);

    if( pTable )
	{
		pTable->InitControl( this );

		pTable->InitTable( 10, 5 );

        pTable->InitItem( 0, 0, GetTextSprite( ID_FRIENDSLIST_C0R0 ) );
		pTable->InitItem( 1, 0, GetTextSprite( ID_FRIENDSLIST_C0R1 ) );
		pTable->InitItem( 2, 0, GetTextSprite( ID_FRIENDSLIST_C0R2 ) );
		pTable->InitItem( 3, 0, GetTextSprite( ID_FRIENDSLIST_C0R3 ) );
		pTable->InitItem( 4, 0, GetTextSprite( ID_FRIENDSLIST_C0R4 ) );
		pTable->InitItem( 5, 0, GetTextSprite( ID_FRIENDSLIST_C0R5 ) );
		pTable->InitItem( 6, 0, GetTextSprite( ID_FRIENDSLIST_C0R6 ) );
		pTable->InitItem( 7, 0, GetTextSprite( ID_FRIENDSLIST_C0R7 ) );
        pTable->InitItem( 8, 0, GetTextSprite( ID_FRIENDSLIST_C0R8 ) );
		pTable->InitItem( 9, 0, GetTextSprite( ID_FRIENDSLIST_C0R9 ) );

        pTable->InitItem( 0, 1, GetTextSprite( ID_FRIENDSLIST_C1R0 ) );
		pTable->InitItem( 1, 1, GetTextSprite( ID_FRIENDSLIST_C1R1 ) );
		pTable->InitItem( 2, 1, GetTextSprite( ID_FRIENDSLIST_C1R2 ) );
		pTable->InitItem( 3, 1, GetTextSprite( ID_FRIENDSLIST_C1R3 ) );
		pTable->InitItem( 4, 1, GetTextSprite( ID_FRIENDSLIST_C1R4 ) );
		pTable->InitItem( 5, 1, GetTextSprite( ID_FRIENDSLIST_C1R5 ) );
		pTable->InitItem( 6, 1, GetTextSprite( ID_FRIENDSLIST_C1R6 ) );
		pTable->InitItem( 7, 1, GetTextSprite( ID_FRIENDSLIST_C1R7 ) );
        pTable->InitItem( 8, 1, GetTextSprite( ID_FRIENDSLIST_C1R8 ) );
		pTable->InitItem( 9, 1, GetTextSprite( ID_FRIENDSLIST_C1R9 ) );

        pTable->InitItem( 0, 2, GetTextSprite( ID_FRIENDSLIST_C2R0 ) );
		pTable->InitItem( 1, 2, GetTextSprite( ID_FRIENDSLIST_C2R1 ) );
		pTable->InitItem( 2, 2, GetTextSprite( ID_FRIENDSLIST_C2R2 ) );
		pTable->InitItem( 3, 2, GetTextSprite( ID_FRIENDSLIST_C2R3 ) );
		pTable->InitItem( 4, 2, GetTextSprite( ID_FRIENDSLIST_C2R4 ) );
		pTable->InitItem( 5, 2, GetTextSprite( ID_FRIENDSLIST_C2R5 ) );
		pTable->InitItem( 6, 2, GetTextSprite( ID_FRIENDSLIST_C2R6 ) );
		pTable->InitItem( 7, 2, GetTextSprite( ID_FRIENDSLIST_C2R7 ) );
        pTable->InitItem( 8, 2, GetTextSprite( ID_FRIENDSLIST_C2R8 ) );
		pTable->InitItem( 9, 2, GetTextSprite( ID_FRIENDSLIST_C2R9 ) );

        pTable->InitItem( 0, 3, GetTextSprite( ID_FRIENDSLIST_C3R0 ) );
		pTable->InitItem( 1, 3, GetTextSprite( ID_FRIENDSLIST_C3R1 ) );
		pTable->InitItem( 2, 3, GetTextSprite( ID_FRIENDSLIST_C3R2 ) );
		pTable->InitItem( 3, 3, GetTextSprite( ID_FRIENDSLIST_C3R3 ) );
		pTable->InitItem( 4, 3, GetTextSprite( ID_FRIENDSLIST_C3R4 ) );
		pTable->InitItem( 5, 3, GetTextSprite( ID_FRIENDSLIST_C3R5 ) );
		pTable->InitItem( 6, 3, GetTextSprite( ID_FRIENDSLIST_C3R6 ) );
		pTable->InitItem( 7, 3, GetTextSprite( ID_FRIENDSLIST_C3R7 ) );
        pTable->InitItem( 8, 3, GetTextSprite( ID_FRIENDSLIST_C3R8 ) );
		pTable->InitItem( 9, 3, GetTextSprite( ID_FRIENDSLIST_C3R9 ) );

		pTable->InitItem( 0, 4, GetTextSprite( ID_FRIENDSLIST_C4R0 ) );
		pTable->InitItem( 1, 4, GetTextSprite( ID_FRIENDSLIST_C4R1 ) );
		pTable->InitItem( 2, 4, GetTextSprite( ID_FRIENDSLIST_C4R2 ) );
		pTable->InitItem( 3, 4, GetTextSprite( ID_FRIENDSLIST_C4R3 ) );
		pTable->InitItem( 4, 4, GetTextSprite( ID_FRIENDSLIST_C4R4 ) );
		pTable->InitItem( 5, 4, GetTextSprite( ID_FRIENDSLIST_C4R5 ) );
		pTable->InitItem( 6, 4, GetTextSprite( ID_FRIENDSLIST_C4R6 ) );
		pTable->InitItem( 7, 4, GetTextSprite( ID_FRIENDSLIST_C4R7 ) );
        pTable->InitItem( 8, 4, GetTextSprite( ID_FRIENDSLIST_C4R8 ) );
		pTable->InitItem( 9, 4, GetTextSprite( ID_FRIENDSLIST_C4R9 ) );
    }

		// Hide PS2 help bar on PC
#ifdef DIRECTX_PC
	GetSprite(ID_FRIENDSLIST_HELPGROUP)->HideAll();
#endif
	
	ScreenBaseClass::OnInitialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is displayed
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenFriendsList::OnScreenIntro( void )
{
    _status = RUNNING;

    ScreenBaseClass::OnScreenIntro();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Catch the shoulder buttons and scroll the list if needed.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenFriendsList::OnButtonPressed(int Button)
{
	GuiTable* pTable = GetFriendsTable();
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

void ScreenFriendsList::OnButtonOK()
{
	GuiTable* pTable = GetFriendsTable();
	// if we don't have a table or we don't have any rows, do nothing
	if( pTable == NULL || pTable->GetRows() == 0 ) return;

	// Save off the currently selected member in the table
	int row, col;
	pTable->GetCursorPos( row, col );
	GuiTable::Cell* cell = pTable->GetCell( row, col );
	if( cell )
	{
		_status = FRIEND_SELECTED;
		_selectedFriend = (int)cell->GetParam();
	}
	
	ScreenBaseClass::OnButtonOK();
}

void ScreenFriendsList::OnButtonCancel()
{
	_status = BACK;

	GuiTable* pTable = GetFriendsTable();
	if( pTable )
	{
		pTable->SetCursorPos( 0, 0 );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Get the table containing clan members
///
/// \return A pointer to the table object with the clan members in it
///////////////////////////////////////////////////////////////////////////////////////////////////
GuiTable* ScreenFriendsList::GetFriendsTable( void )
{
	GuiTable* pTable = 0;
	Sprite* pSprite = GetSprite( ID_FRIENDSLIST_GROUP );

	if( pSprite )
    {
		pTable = (GuiTable*) pSprite->GetInterface( GuiTable::ClassnameStatic() );
    }

	return pTable;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when up is pressed: move the selection
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenFriendsList::OnCursorUp()
{
	MoveCursor( -1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when down is pressed: move the selection
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenFriendsList::OnCursorDown()
{
	MoveCursor( 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenFriendsList::SetFriendsList( OSIPlayerList &friendsList, DisplayMode displayMode )
{
	GuiTable* pTable = GetFriendsTable();
	if( !pTable ) return;

	_displayMode = displayMode;

	pTable->DeleteAllItems();

	OSIchar buffer[ 512 ];
//	OSIchar nameBuffer[ OSIClanInfo::MAX_FULL_NAME_LENGTH ];

	if( _displayMode == DISPLAY_CONFIRMED_FRIENDS )
	{
		OSIsnprintf( buffer, NUM_ARRAY_ELEMENTS(buffer), "Friends - Confirmed" );
	}
	else if( _displayMode == DISPLAY_INCOMING_FRIEND_REQUESTS )
	{
		OSIsnprintf( buffer, NUM_ARRAY_ELEMENTS(buffer), "Friends - Incoming Requests" );
	}
	else
	{
		OSIsnprintf( buffer, NUM_ARRAY_ELEMENTS(buffer), "Friends - Outgoing Requests" );
	}

	m_pScreen->GetTextSprite( ID_FRIENDSLIST_TITLE )->SetText( buffer );

	// Fill in the table with our member list.
	OSIPlayer* myFriend = 0;
	unsigned int row = 0, realIndex = 0;
	for( friendsList.begin(); myFriend = friendsList.getCurrent(); friendsList.next(), ++realIndex )
	{
		if( myFriend->isAFriend && _displayMode != DISPLAY_CONFIRMED_FRIENDS )
			continue;
		if( myFriend->hasRequestedMyFriendship && _displayMode != DISPLAY_INCOMING_FRIEND_REQUESTS )
			continue;
		if( myFriend->haveRequestedTheirFriendship && _displayMode != DISPLAY_OUTGOING_FRIEND_REQUESTS )
			continue;

		AddFriendRowToTable( pTable, row++, myFriend, realIndex );
	}
}

char* GetTitleFromStatusString( char* buff, const char* statusString )
{
	int i = 0;
	for( ; statusString[i] != 0 && statusString[i] != '|'; ++i )
	{
		buff[i] = statusString[i];
	}
	buff[i] = 0;

	return buff;
}

char* GetPlatformFromStatusString( char* buff, const char* statusString )
{
	char* statusCursor = OSIstrchr( statusString, '|' );
	if( statusCursor == NULL )
	{
		buff[0] = 0;
		return buff;
	}

	int i = 0;
	for( ++statusCursor; *statusCursor != 0 && *statusCursor != '|'; ++statusCursor, ++i )
	{
		buff[i] = *statusCursor;
	}
	buff[i] = 0;

	return buff;
}

void ScreenFriendsList::AddFriendRowToTable( GuiTable* pTable, int row, OSIPlayer* myFriend, int realIndex )
{
	OSIchar buffer[ 512 ];
	OSIchar nameBuffer[ OSIClanInfo::MAX_FULL_NAME_LENGTH ];

	pTable->AddItem( myFriend->GetFullName( nameBuffer ), row, (void*)realIndex );
	if( _displayMode != DISPLAY_CONFIRMED_FRIENDS )
	{
		pTable->SetItem( row, 1, "Unknown" );
		return;
	}

	if( myFriend->currentMatchList->count() > 0 ) 
	{
		myFriend->currentMatchList->begin();

		OSIMatch* friendsMatch = myFriend->currentMatchList->getCurrent();
		if( friendsMatch->maximumPlayers > 0 && friendsMatch->numberOfPlayers > 0 )
		{
			// member is currently in a game, display info about that game
			OSIsnprintf( buffer, NUM_ARRAY_ELEMENTS(buffer), "%s%s",
				myFriend->lookingToPlay ? g_translator.Translate( "(Invited) " ) : "", friendsMatch->options.description );
			pTable->SetItem( row, 1, buffer ); // Match column
			const char* const modeString = COnlineSessionSearch::GameTypeIndexToStringLUT[friendsMatch->options.gameTypeIndex];
			OSIsnprintf( buffer, NUM_ARRAY_ELEMENTS(buffer), "%s - %s", friendsMatch->options.levelName, modeString );
			pTable->SetItem( row, 2, buffer ); // Game column
			OSIsnprintf( buffer, NUM_ARRAY_ELEMENTS(buffer), "%i / %i", friendsMatch->options.totalConnectedPlayers, friendsMatch->maximumPlayers );
			pTable->SetItem( row, 3, buffer ); // Players column
			return;
		}
	}

	// member is not in a game but is member online?
	if( myFriend->state.isOnline )
	{
		if( myFriend->statusString )
		{
			char title[ 128 ];
			char platform[ 128 ];
			char titleAndPlatform[ 256 ];
			GetTitleFromStatusString( title, myFriend->statusString );
			GetPlatformFromStatusString( platform, myFriend->statusString );
			OSIsnprintf( titleAndPlatform, 256, "%s - %s (%s)", g_translator.Translate( "Online" ), title, platform );
			titleAndPlatform[ 255 ] = 0;
			pTable->SetItem( row, 1, "" );
			pTable->SetItem( row, 2, "" );
			pTable->SetItem( row, 3, "" );
			pTable->SetItem( row, 4, titleAndPlatform );
		}
		else
		{
			pTable->SetItem( row, 1, "Online" );
		}
	}
	else
	{
		pTable->SetItem( row, 1, "Offline" );
	}
}

void ScreenFriendsList::MoveCursor( int numRowsToMove )
{
	GuiTable* pTable = GetFriendsTable();
	if( !pTable ) 
	{
		return; 
	}

	int R1, C1;
	int R2, C2;

	pTable->GetCursorPos( R1, C1 );
	pTable->MoveCursor( 0, numRowsToMove );
	pTable->GetCursorPos( R2, C2 );

	if( R1 != R2 || C1 != C2 )
		PlaySoundEvent( Screen::SOUNDEVENT_NEXTBUTTON );
}