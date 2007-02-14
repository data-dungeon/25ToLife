#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "screen_stats.h"
#include "data/screens/Multiplayer.h"
#include "frontend/frontend.h"
#include "GameHelper/Translator.h"
#include "OSI/OSI.hpp"

const int NUM_STATS = 10; ///< Number of stats displayed on the screen

ScreenStats::ScreenStats()
: _status( RUNNING ),
  _currentStatFilter( STAT_FILTER_SHOW_ALL ),
  _currentStatSource( STAT_SOURCE_PLAYERS ),
  _needsMoreData( false ),
  _moreStats( false ),
  _newStats( 0 ),
  _waitingForUpdate( false )
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is loaded.  Set up the buttons on the screen.
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenStats::OnInitialize(void)
{
    GuiTable* pTable = CreateTable(ID_STATS_GROUP);

    UseHighlighting(true);

	 SetHelpRect( ID_STATS_HELPGROUP );

    // Set up buttons
	SetInitialButton(ID_STATS_PLAYER);
    SetNextButton(ID_STATS_TOP10);
    SetNextButton(ID_STATS_CLAN);
    SetNextButton(ID_STATS_FRIENDS);
    SetNextButton(ID_STATS_TOPCLANS);
	SetCursor(ID_STAT_CURSOR);

    SelectButton(ID_STATS_PLAYER);

    // Set up table
    ASSERT(pTable);
    if (pTable)
    {
        // NOTE: If the number of rows changes, change NUM_STATS
        // at the top of this file.
        pTable->InitTable(NUM_STATS, 2);
        pTable->SetCursorMode(GuiTable::RowsOnly);

        pTable->InitItem(0, 0, GetTextSprite(ID_STATS_C1R1));
		pTable->InitItem(1, 0, GetTextSprite(ID_STATS_C1R2));
        pTable->InitItem(2, 0, GetTextSprite(ID_STATS_C1R3));
        pTable->InitItem(3, 0, GetTextSprite(ID_STATS_C1R4));
        pTable->InitItem(4, 0, GetTextSprite(ID_STATS_C1R5));
        pTable->InitItem(5, 0, GetTextSprite(ID_STATS_C1R6));
		pTable->InitItem(6, 0, GetTextSprite(ID_STATS_C1R7));
		pTable->InitItem(7, 0, GetTextSprite(ID_STATS_C1R8));
		pTable->InitItem(8, 0, GetTextSprite(ID_STATS_C1R9));
		pTable->InitItem(9, 0, GetTextSprite(ID_STATS_C1R10));

        pTable->InitItem(0, 1, GetTextSprite(ID_STATS_C2R1));
		pTable->InitItem(1, 1, GetTextSprite(ID_STATS_C2R2));
        pTable->InitItem(2, 1, GetTextSprite(ID_STATS_C2R3));
        pTable->InitItem(3, 1, GetTextSprite(ID_STATS_C2R4));
        pTable->InitItem(4, 1, GetTextSprite(ID_STATS_C2R5));
        pTable->InitItem(5, 1, GetTextSprite(ID_STATS_C2R6));
		pTable->InitItem(6, 1, GetTextSprite(ID_STATS_C2R7));
		pTable->InitItem(7, 1, GetTextSprite(ID_STATS_C2R8));
		pTable->InitItem(8, 1, GetTextSprite(ID_STATS_C2R9));
		pTable->InitItem(9, 1, GetTextSprite(ID_STATS_C2R10));
    }

	HideWait();

	// Hide PS2 help bar on PC
#ifdef DIRECTX_PC
	GetSprite(ID_STATS_HELPGROUP)->HideAll();
#endif

	ScreenBaseClass::OnInitialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time the screen is initialized.  Set the default screen state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenStats::OnScreenIntro(void)
{
    _status = RUNNING;
	_currentStatFilter = 0;
	_currentStatSource = 0;

    ScreenBaseClass::OnScreenIntro();

    UpdateStatFilter();
	SelectInitialButton();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the screen status based off of the selection.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenStats::OnCommand( int Command )
{
    switch (Command)
    {
        case ID_STATS_PLAYER:
			_currentStatSource = STAT_SOURCE_PLAYERS;
			ClearStats();
			break;
        case ID_STATS_TOP10:
			_currentStatSource = STAT_SOURCE_TOP_PLAYERS;
			ClearStats();
			break;
        case ID_STATS_CLAN:
			_currentStatSource = STAT_SOURCE_CLAN;
			_currentStatFilter = STAT_FILTER_SHOW_ALL;
			ClearStats();
			break;
        case ID_STATS_FRIENDS:
			_currentStatSource = STAT_SOURCE_FRIENDS;
			ClearStats();
			break;
        case ID_STATS_TOPCLANS:
			_currentStatSource = STAT_SOURCE_TOP_CLANS;
			ClearStats();
			break;
		default:
			ASSERTS( 0, "Unknown screen command" );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Decrement the stat filter we're using
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenStats::OnCursorLeft(void)
{
    _status = PREV_STAT;

    // Reset our state and ask for new data
    ClearStats();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Increment the stat type we're using
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenStats::OnCursorRight(void)
{
    _status = NEXT_STAT;

    // Reset our state and ask for new data
    ClearStats();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// If we're waiting for a stats update, cycle the wait icon
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenStats::OnScreenUpdate()
{
    static float Clock = 0.0f;
    static int Frame = 0;

    if (_waitingForUpdate)
    {
        Clock += g_timer.GetOSFrameSec();

        // Timeout has elapsed, go to next frame
        if (Clock > 0.25f)
        {
            // Increment frame counter
            ++Frame;
            Frame = Frame % 4;

            // Show next frame and hide current one
            switch (Frame)
            {
                case 0:
                    GetSprite(ID_STATS_WAIT_BR)->Hide();
                    GetSprite(ID_STATS_WAIT_TR)->Show();
                    break;
                case 1:
                    GetSprite(ID_STATS_WAIT_TR)->Hide();
                    GetSprite(ID_STATS_WAIT_TL)->Show();
                    break;
                case 2:
                    GetSprite(ID_STATS_WAIT_TL)->Hide();
                    GetSprite(ID_STATS_WAIT_BL)->Show();
                    break;
                case 3:
                    GetSprite(ID_STATS_WAIT_BL)->Hide();
                    GetSprite(ID_STATS_WAIT_BR)->Show();
                    break;
            }

            // Reset clock
            Clock = 0.0f;
        }
    }

	ScreenBaseClass::OnScreenUpdate();

	GuiTable* pTable = GetStatTable();
	if( !pTable ) return;

	for( int statTableIndex = 0; statTableIndex < pTable->GetRows(); ++statTableIndex )
	{
		GuiTable::DisplayItem* displayItem = pTable->GetDisplayItem( statTableIndex, 0 );
		if( displayItem == NULL ) continue;
		Text* displayItemText = displayItem->GetDisplayItem();
		if( displayItemText == NULL ) continue;
		char* name = displayItemText->GetText();
		if( name == 0 || name[0] == 0 ) continue;
		char* const playerName = OSI::getSingletonPtr()->getActivePlayer()->name;
		char* nameWithoutClanAbbr = strstr( name, ">" );
		if( nameWithoutClanAbbr ) nameWithoutClanAbbr += 2;
		else nameWithoutClanAbbr = name;
		if( stricmp( nameWithoutClanAbbr, playerName ) == 0 )
		{
			pTable->GetDisplayItem( statTableIndex, 0 )->GetDisplayItem()->SetColor( 0, 0, 150, 255 );
			pTable->GetDisplayItem( statTableIndex, 1 )->GetDisplayItem()->SetColor( 0, 0, 150, 255 );
		}
		else
		{
			pTable->GetDisplayItem( statTableIndex, 0 )->GetDisplayItem()->SetColor( 255, 255, 255, 255 );
			pTable->GetDisplayItem( statTableIndex, 1 )->GetDisplayItem()->SetColor( 255, 255, 255, 255 );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Update the highlight so that it's over the current stat type.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenStats::UpdateStatFilter()
{
    // Wrap _currentStatFilter if needed
    if (_currentStatFilter < 0) _currentStatFilter = NUM_STAT_FILTER - 1;
    if (_currentStatFilter >= NUM_STAT_FILTER) _currentStatFilter = 0;

    switch (_currentStatFilter)
    {
        case STAT_FILTER_SHOW_ALL:
			GetTextSprite(ID_STATS_GROUP_NAME)->SetText( "Stat Group: Total" );
            break;

        case STAT_FILTER_SHOW_COP_ONLY:
            GetTextSprite(ID_STATS_GROUP_NAME)->SetText( "Stat Group: Law Enforcement" );
            break;

        case STAT_FILTER_SHOW_GANGSTA_ONLY:
            GetTextSprite(ID_STATS_GROUP_NAME)->SetText( "Stat Group: Criminal" );
            break;
    }

    // Reset our state and ask for new data
    ClearStats();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Get a pointer to the stat table object
///////////////////////////////////////////////////////////////////////////////////////////////////
GuiTable* ScreenStats::GetStatTable(void)
{
    GuiTable* pTable = 0;
	Sprite* pSprite = GetSprite(ID_STATS_GROUP);

    ASSERT(pSprite);
	if( pSprite )
    {
		pTable = (GuiTable*) pSprite->GetInterface( GuiTable::ClassnameStatic() );
    }

    ASSERT(pTable);
	return pTable;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Get the number of rows displayed on the screen in the stat table
///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int ScreenStats::GetStatTableDisplayRows()
{
	GuiTable *pTable = GetStatTable();

	if( !pTable ) return 0;

	int displayRows = pTable->GetDisplayRows();

	ASSERT( displayRows > 0 );

	return displayRows;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the text for the current stat title we're showing
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenStats::SetStatTitle(const char *pTitle)
{
    ASSERT(GetTextSprite(ID_STATS_STAT_TITLE));
    ASSERT(pTitle);
    if (!GetTextSprite(ID_STATS_STAT_TITLE)) return;
    if (!pTitle) return;

    GetTextSprite(ID_STATS_STAT_TITLE)->SetText(pTitle);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the text for the current stat title we're showing
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenStats::SetCurrentStatName(const char *pCurrentStatName)
{
	ASSERT(GetTextSprite(ID_STATS_TITLE));
	ASSERT(pCurrentStatName);
	if (!GetTextSprite(ID_STATS_TITLE)) return;
	if (!pCurrentStatName) return;

	char buff[256];
	OSIsnprintf( buff, 256, "Stats and Rankings: %s", pCurrentStatName );
	GetTextSprite(ID_STATS_TITLE)->SetText( buff );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Set the current user name on the screen
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenStats::SetUserName(const char *pName)
{
    ASSERT(GetTextSprite(ID_STATS_PROFILENAME));
    ASSERT(pName);
    if (!GetTextSprite(ID_STATS_PROFILENAME)) return;
    if (!pName) return;

	char buff[ 256 ];
	OSIsnprintf( buff, 256, "%s: %s", g_translator.Translate( "Online name" ), pName );
	buff[ 255 ] = 0;
    GetTextSprite(ID_STATS_PROFILENAME)->SetTextNoTranslate( buff );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Add the given row to the table
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenStats::AddRow(const char *pUserName, float value)
{
    char RatingStr[15];
	int ival = (int)value;
	float tmp = value - ival;
	if ( tmp > 0.0f )
		sprintf(RatingStr, "%.3f", value);
	else
		sprintf(RatingStr, "%d", (int)value);

    AddRow(pUserName, RatingStr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Add the given row to the table
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenStats::AddRow(const char *pUserName, int value)
{
    char RatingStr[15];
    sprintf(RatingStr, "%d", value);

    AddRow(pUserName, RatingStr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Add the given row to the table
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenStats::AddRow(const char *pUserName, const char *pValue)
{
	GuiTable* pTable = GetStatTable();
    ASSERT(pTable);
    if (!pTable) return;
    int numRows = pTable->GetRows();

    // Set text in table
    pTable->AddItem(pUserName, numRows, 0);
    pTable->SetItem(numRows, 1, pValue);

    // Increment number of rows added
    ++_newStats;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called by the use case when it's done sending us rows.  Update the stat table and internal
/// values as needed.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenStats::FinishAddingRows()
{
    ASSERT(GetStatTable());
    if (!GetStatTable()) return;

    // Hide the wait icon
    _waitingForUpdate = false;
	_needsMoreData = false;
    HideWait();

    // No new stats added, so the use case doesn't have more data for us
    if (_newStats == 0)
    {
        _moreStats = false;
        return;
    }

    // Page down
    GetStatTable()->MoveCursor(NUM_STATS, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Clear the stats table, reset stat state and ask for new data from the use case.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenStats::ClearStats()
{
    ASSERT(GetStatTable());
    if (!GetStatTable()) return;

	GetStatTable()->MoveCursor( 0, 0 );
    GetStatTable()->DeleteAllItems();

    // Reset our state and ask for new data
    _needsMoreData = true;		// We don't have any data, so we need more
    _newStats = 0;				// Haven't gotten any data yet
    _moreStats = true;			// We can get more stats from the use case
    _waitingForUpdate = false;	// Not waiting for an update

	// Hide square button help if we are on top clans since you can't use it
#ifndef DIRECTX_PC
	if( _currentStatSource == STAT_SOURCE_TOP_CLANS )
		GetSprite( ID_STATS_HELPTEXT4 )->HideAll();
	else
		GetSprite( ID_STATS_HELPTEXT4 )->ShowAll();
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Catch the shoulder buttons and scroll the list if needed.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenStats::OnButtonPressed(int Button)
{
	GuiTable* pTable = GetStatTable();
    ASSERT(pTable);
    if (!pTable) return;

    if (FluffControlMapper::L1 == Button)
    {
		switch( _currentStatSource )
		{
			case STAT_SOURCE_FRIENDS:
			case STAT_SOURCE_PLAYERS:
				if( pTable->GetRows() != 0  )
				{
					ClearStats();
					_status = PAGE_PLAYER_LIST_UP;
				}
				break;

			case STAT_SOURCE_CLAN:
				pTable->PageUp();
				break;
		}
    }
    else if (FluffControlMapper::R1 == Button)
    {
        switch( _currentStatSource )
		{
			case STAT_SOURCE_PLAYERS:
			case STAT_SOURCE_FRIENDS:
				if( pTable->GetRows() != 0 )
				{
					ClearStats();
					_status = PAGE_PLAYER_LIST_DOWN;
				}
				break;

			case STAT_SOURCE_CLAN:
				pTable->PageDown();
				break;
		}
    }
    else if (FluffControlMapper::SQUARE == Button)
    {
		if( _currentStatSource != STAT_SOURCE_TOP_CLANS )
		{
			// Toggle between total cop and gangsta
			++_currentStatFilter;
			UpdateStatFilter();
			_status = CHANGED_FILTER;
		}
    }

    ScreenBaseClass::OnButtonPressed(Button);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Page the table by the given amount
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenStats::Page(int Amount)
{
    ASSERT(GetStatTable());
    if (!GetStatTable()) return;

    // We only need to page, no need to get new stats
    if (Amount < 0)
    {
        GetStatTable()->MoveCursor(Amount, 0);
        return;
    }
    // We may need to page, check and see
    else
    {
        // Don't need to page, no more stats
        if (!_moreStats)
        {
            return;
        }

        // We're in the middle of the list, no need to page
        int R, C;
        GetStatTable()->GetCursorPos(R, C);
        if (GetStatTable()->GetRows() - R >= NUM_STATS)
        {
            GetStatTable()->MoveCursor(Amount, 0);
            return;
        }

        // We need to page down, ask for data
        _needsMoreData = true;
        _newStats = 0; // Reset new stat counter
        _waitingForUpdate = true; // Waiting for a stat update

        // Display the wait icon
        ShowWait();

        // NOTE: We'll do the actual page down in EndStats() if we get data
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Show the wait icon and grey out the table
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenStats::ShowWait()
{
    ASSERT(GetSprite(ID_STATS_WAIT_BG));

    if (!GetSprite(ID_STATS_WAIT_BG)) return;

    // Grey out table items
    GetTextSprite(ID_STATS_C1R1)->SetColor(80, 80, 80, 80);
    GetTextSprite(ID_STATS_C2R1)->SetColor(80, 80, 80, 80);
    GetTextSprite(ID_STATS_C1R2)->SetColor(80, 80, 80, 80);
    GetTextSprite(ID_STATS_C2R2)->SetColor(80, 80, 80, 80);
    GetTextSprite(ID_STATS_C1R3)->SetColor(80, 80, 80, 80);
    GetTextSprite(ID_STATS_C2R3)->SetColor(80, 80, 80, 80);
    GetTextSprite(ID_STATS_C1R4)->SetColor(80, 80, 80, 80);
    GetTextSprite(ID_STATS_C2R4)->SetColor(80, 80, 80, 80);
    GetTextSprite(ID_STATS_C1R5)->SetColor(80, 80, 80, 80);
    GetTextSprite(ID_STATS_C2R5)->SetColor(80, 80, 80, 80);
    GetTextSprite(ID_STATS_C1R6)->SetColor(80, 80, 80, 80);
    GetTextSprite(ID_STATS_C2R6)->SetColor(80, 80, 80, 80);
	GetTextSprite(ID_STATS_C1R7)->SetColor(80, 80, 80, 80);
    GetTextSprite(ID_STATS_C2R7)->SetColor(80, 80, 80, 80);
	GetTextSprite(ID_STATS_C1R8)->SetColor(80, 80, 80, 80);
    GetTextSprite(ID_STATS_C2R8)->SetColor(80, 80, 80, 80);
	GetTextSprite(ID_STATS_C1R9)->SetColor(80, 80, 80, 80);
    GetTextSprite(ID_STATS_C2R9)->SetColor(80, 80, 80, 80);
	GetTextSprite(ID_STATS_C1R10)->SetColor(80, 80, 80, 80);
    GetTextSprite(ID_STATS_C2R10)->SetColor(80, 80, 80, 80);

    // Show wait icon
    GetSprite(ID_STATS_WAIT_BG)->Show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Hide the wait icon and ungrey the table
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenStats::HideWait()
{
    ASSERT(GetSprite(ID_STATS_WAIT_BG));

    if (!GetSprite(ID_STATS_WAIT_BG)) return;

    // Ungrey table
    GetTextSprite(ID_STATS_C1R1)->SetColor(255, 255, 255, 255);
    GetTextSprite(ID_STATS_C2R1)->SetColor(255, 255, 255, 255);
    GetTextSprite(ID_STATS_C1R2)->SetColor(255, 255, 255, 255);
    GetTextSprite(ID_STATS_C2R2)->SetColor(255, 255, 255, 255);
    GetTextSprite(ID_STATS_C1R3)->SetColor(255, 255, 255, 255);
    GetTextSprite(ID_STATS_C2R3)->SetColor(255, 255, 255, 255);
    GetTextSprite(ID_STATS_C1R4)->SetColor(255, 255, 255, 255);
    GetTextSprite(ID_STATS_C2R4)->SetColor(255, 255, 255, 255);
    GetTextSprite(ID_STATS_C1R5)->SetColor(255, 255, 255, 255);
    GetTextSprite(ID_STATS_C2R5)->SetColor(255, 255, 255, 255);
    GetTextSprite(ID_STATS_C1R6)->SetColor(255, 255, 255, 255);
    GetTextSprite(ID_STATS_C2R6)->SetColor(255, 255, 255, 255);
	GetTextSprite(ID_STATS_C1R7)->SetColor(255, 255, 255, 255);
    GetTextSprite(ID_STATS_C2R7)->SetColor(255, 255, 255, 255);
	GetTextSprite(ID_STATS_C1R8)->SetColor(255, 255, 255, 255);
    GetTextSprite(ID_STATS_C2R8)->SetColor(255, 255, 255, 255);
	GetTextSprite(ID_STATS_C1R9)->SetColor(255, 255, 255, 255);
    GetTextSprite(ID_STATS_C2R9)->SetColor(255, 255, 255, 255);
	GetTextSprite(ID_STATS_C1R10)->SetColor(255, 255, 255, 255);
    GetTextSprite(ID_STATS_C2R10)->SetColor(255, 255, 255, 255);

    // Hide wait icon
    GetSprite(ID_STATS_WAIT_BG)->Hide();
    GetSprite(ID_STATS_WAIT_TR)->Hide();
    GetSprite(ID_STATS_WAIT_TL)->Hide();
    GetSprite(ID_STATS_WAIT_BR)->Hide();
    GetSprite(ID_STATS_WAIT_BL)->Hide();
}
