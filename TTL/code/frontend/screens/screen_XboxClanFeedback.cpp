#include "TTLPCH.h"
#include "OSI/OSIMatchList.hpp"
#include "frontend/inputdriver.h"
#include "screen_XboxClanFeedback.h"
#include "data/screens/Multiplayer.h"
#include "frontend/frontend.h"
#include "Network/OnlineSessionSearch.h"
#include "OSI/OSI.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is first loaded
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenXboxClanFeedback::OnInitialize(void)
{
    // Set up the table
    GuiTable* pTable = CreateTable( ID_CLANFEEDBACK_GROUP );

    UseHighlighting(true);

    if( pTable )
	{
		pTable->InitControl( this );

		pTable->InitTable( 10, 2 );

        pTable->InitItem( 0, 0, GetTextSprite( ID_CLANFEEDBACK_C0R0 ) );
		pTable->InitItem( 1, 0, GetTextSprite( ID_CLANFEEDBACK_C0R1 ) );
		pTable->InitItem( 2, 0, GetTextSprite( ID_CLANFEEDBACK_C0R2 ) );
		pTable->InitItem( 3, 0, GetTextSprite( ID_CLANFEEDBACK_C0R3 ) );
		pTable->InitItem( 4, 0, GetTextSprite( ID_CLANFEEDBACK_C0R4 ) );
		pTable->InitItem( 5, 0, GetTextSprite( ID_CLANFEEDBACK_C0R5 ) );
		pTable->InitItem( 6, 0, GetTextSprite( ID_CLANFEEDBACK_C0R6 ) );
		pTable->InitItem( 7, 0, GetTextSprite( ID_CLANFEEDBACK_C0R7 ) );
        pTable->InitItem( 8, 0, GetTextSprite( ID_CLANFEEDBACK_C0R8 ) );
		pTable->InitItem( 9, 0, GetTextSprite( ID_CLANFEEDBACK_C0R9 ) );

        pTable->InitItem( 0, 1, GetTextSprite( ID_CLANFEEDBACK_C1R0 ) );
		pTable->InitItem( 1, 1, GetTextSprite( ID_CLANFEEDBACK_C1R1 ) );
		pTable->InitItem( 2, 1, GetTextSprite( ID_CLANFEEDBACK_C1R2 ) );
		pTable->InitItem( 3, 1, GetTextSprite( ID_CLANFEEDBACK_C1R3 ) );
		pTable->InitItem( 4, 1, GetTextSprite( ID_CLANFEEDBACK_C1R4 ) );
		pTable->InitItem( 5, 1, GetTextSprite( ID_CLANFEEDBACK_C1R5 ) );
		pTable->InitItem( 6, 1, GetTextSprite( ID_CLANFEEDBACK_C1R6 ) );
		pTable->InitItem( 7, 1, GetTextSprite( ID_CLANFEEDBACK_C1R7 ) );
        pTable->InitItem( 8, 1, GetTextSprite( ID_CLANFEEDBACK_C1R8 ) );
		pTable->InitItem( 9, 1, GetTextSprite( ID_CLANFEEDBACK_C1R9 ) );
    }

		// Hide PS2 help bar on PC
#ifdef DIRECTX_PC
	GetSprite(ID_CLANFEEDBACK_HELPGROUP)->HideAll();
#endif
	
	ScreenBaseClass::OnInitialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is displayed
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenXboxClanFeedback::OnScreenIntro( void )
{
    _status = RUNNING;

    ScreenBaseClass::OnScreenIntro();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Catch the shoulder buttons and scroll the list if needed.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenXboxClanFeedback::OnButtonPressed(int Button)
{
	GuiTable* pTable = GetClanTable();
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

void ScreenXboxClanFeedback::OnButtonOK()
{
	GuiTable* pTable = GetClanTable();
	// if we don't have a table or we don't have any rows, do nothing
	if( pTable == NULL || pTable->GetRows() == 0 ) return;

	// Save off the currently selected clan in the table
	int row, col;
	pTable->GetCursorPos( row, col );
	GuiTable::Cell* cell = pTable->GetCell( row, col );
	if( cell )
	{
		_status = CLAN_SELECTED;
		_selectedClan = (int)cell->GetParam();
	}
	
	ScreenBaseClass::OnButtonOK();
}

void ScreenXboxClanFeedback::OnButtonCancel()
{
	_status = BACK;

	GuiTable* pTable = GetClanTable();
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
GuiTable* ScreenXboxClanFeedback::GetClanTable( void )
{
	GuiTable* pTable = 0;
	Sprite* pSprite = GetSprite( ID_CLANFEEDBACK_GROUP );

	if( pSprite )
    {
		pTable = (GuiTable*) pSprite->GetInterface( GuiTable::ClassnameStatic() );
    }

	return pTable;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when up is pressed: move the selection
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenXboxClanFeedback::OnCursorUp()
{
	MoveCursor( -1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when down is pressed: move the selection
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenXboxClanFeedback::OnCursorDown()
{
	MoveCursor( 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenXboxClanFeedback::PopulateFromPlayerList( OSIPlayerList& playerList )
{
	GuiTable* pTable = GetClanTable();
	if( !pTable ) return;

	pTable->DeleteAllItems();

	// Fill in the table with our member list.
	// used to make sure screen is only populated with unique entries and that it doesn't include
	// the local player's clan.
	OSIClanIDList tempClanIDList; 
	tempClanIDList.push_back( OSI::getSingletonPtr()->getActivePlayer()->clanInfo.clanID );

	OSIPlayer* player = 0;
	unsigned int row = 0, realIndex = 0;
	for( playerList.begin(); player = playerList.getCurrent(); playerList.next(), ++realIndex )
	{
		if( (player->clanInfo.memberCount <= 0) || (tempClanIDList.find( player->clanInfo.clanID ) ) )
		{
			continue;
		}

		AddClanRowToTable( pTable, row++, &player->clanInfo, realIndex );
	}
}

int ScreenXboxClanFeedback::GetSelectedClan()
{
	GuiTable* pTable = GetClanTable();
	if( !pTable ) return 0;

	GuiTable::Cell* cell = pTable->GetCell( _selectedClan, 0 );
	ASSERT( cell );
    return (int) cell->GetParam();
}

void ScreenXboxClanFeedback::AddClanRowToTable( GuiTable* pTable, int row, OSIClanInfo* clanInfo, int realIndex )
{
	pTable->AddItem( clanInfo->tag, row, (void*)realIndex );
	pTable->SetItem( row, 1, clanInfo->name );
}

void ScreenXboxClanFeedback::MoveCursor( int numRowsToMove )
{
	GuiTable* pTable = GetClanTable();
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