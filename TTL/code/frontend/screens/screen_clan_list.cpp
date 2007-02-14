#include "TTLPCH.h"
#include "OSI/OSI.hpp"
#include "frontend/inputdriver.h"
#include "screen_clan_list.h"
#include "data/screens/Multiplayer.h"
#include "frontend/frontend.h"
#include "Network/OnlineSessionSearch.h"
#include "GameHelper/Translator.h"

#include "environ/ConfigMgr.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is first loaded
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenClanList::OnInitialize(void)
{
	SetHelpRect( ID_CLAN_HELPRECT );

    // Set up the table
    GuiTable* pTable = CreateTable( ID_CLANLIST_MEMBER_GROUP );

    UseHighlighting(true);

    if( pTable )
	{
		pTable->InitControl( this );
		pTable->InitTable( 4, 4 );

      pTable->InitItem( 0, 0, GetTextSprite( ID_CLANLIST_C0R0 ) );
		pTable->InitItem( 1, 0, GetTextSprite( ID_CLANLIST_C0R1 ) );
		pTable->InitItem( 2, 0, GetTextSprite( ID_CLANLIST_C0R2 ) );
		pTable->InitItem( 3, 0, GetTextSprite( ID_CLANLIST_C0R3 ) );

      pTable->InitItem( 0, 1, GetTextSprite( ID_CLANLIST_C1R0 ) );
		pTable->InitItem( 1, 1, GetTextSprite( ID_CLANLIST_C1R1 ) );
		pTable->InitItem( 2, 1, GetTextSprite( ID_CLANLIST_C1R2 ) );
		pTable->InitItem( 3, 1, GetTextSprite( ID_CLANLIST_C1R3 ) );

      pTable->InitItem( 0, 2, GetTextSprite( ID_CLANLIST_C2R0 ) );
		pTable->InitItem( 1, 2, GetTextSprite( ID_CLANLIST_C2R1 ) );
		pTable->InitItem( 2, 2, GetTextSprite( ID_CLANLIST_C2R2 ) );
		pTable->InitItem( 3, 2, GetTextSprite( ID_CLANLIST_C2R3 ) );

      pTable->InitItem( 0, 3, GetTextSprite( ID_CLANLIST_C3R0 ) );
		pTable->InitItem( 1, 3, GetTextSprite( ID_CLANLIST_C3R1 ) );
		pTable->InitItem( 2, 3, GetTextSprite( ID_CLANLIST_C3R2 ) );
		pTable->InitItem( 3, 3, GetTextSprite( ID_CLANLIST_C3R3 ) );
    }

		// Hide PS2 help bar on PC
#ifdef DIRECTX_PC
	GetSprite(ID_CLAN_HELPRECT)->HideAll();
#endif
	
	ScreenBaseClass::OnInitialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is displayed
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanList::OnScreenIntro( void )
{
	_status = RUNNING;

	ScreenBaseClass::OnScreenIntro();

	char buff[ 256 ];
	OSIsnprintf( buff, 256, "%s: %s", g_translator.Translate( "Online name" ), OSI::getSingletonPtr()->getActiveAccount()->player.name );
	buff[ 255 ] = 0;
	GetTextSprite( ID_CLAN_PROFILENAME )->SetTextNoTranslate( buff );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when user selects a button
///
/// \param Command ID of the selected button
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanList::OnCommand( int Command )
{
    ScreenBaseClass::OnCommand(Command);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Catch the shoulder buttons and scroll the list if needed.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanList::OnButtonPressed(int Button)
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

void ScreenClanList::OnButtonOK()
{
	GuiTable* pTable = GetClanTable();
	// if we don't have a table or we don't have any rows, do nothing
	if(pTable == NULL || pTable->GetRows() == 0) return;

	// Set the status flag
	_status = MEMBER_SELECTED;

	// Save off the currently selected member in the table
	int row, col;
	pTable->GetCursorPos(row, col);
	OSIClanMembershipList& memberList = OSI::getSingletonPtr()->getClanMembershipList();
	if( row >= 0 && row < memberList.count() )
	{
		_clanMember = memberList[row];
	}
	else
	{
		_clanMember = NULL;
		_status = RUNNING;
		return;
	}
	
	ScreenBaseClass::OnButtonOK();
}

void ScreenClanList::OnButtonCancel()
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
GuiTable* ScreenClanList::GetClanTable( void )
{
	GuiTable* pTable = 0;
	Sprite* pSprite = GetSprite( ID_CLANLIST_MEMBER_GROUP );

	if( pSprite )
    {
		pTable = (GuiTable*) pSprite->GetInterface( GuiTable::ClassnameStatic() );
    }

	return pTable;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when up is pressed: move the selection
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanList::OnCursorUp()
{
	MoveCursor( -1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when down is pressed: move the selection
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanList::OnCursorDown()
{
	MoveCursor( 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanList::OnScreenRefresh()
{
	GuiTable*	pTable	= GetClanTable();
	OSI*		osi		= OSI::getSingletonPtr();
	if( !pTable || !osi )
	{
		return;
	}

	pTable->DeleteAllItems();

	OSIchar buffer[ 512 ];
	OSIchar nameBuffer[ OSIClanInfo::MAX_FULL_NAME_LENGTH ];

	OSIPlayer* player = OSI::getSingletonPtr()->getActivePlayer();

	// Get our clan membership list from OSI
	OSIClanMembershipList& memberList = OSI::getSingletonPtr()->getClanMembershipList();

	// Fill in the table with our member list.
	unsigned int memberCount = memberList.count();
	memberList.begin();
	for ( unsigned int row = 0; row < memberCount; ++row )
	{
		OSIClanMembership* member = memberList.getCurrent();
		OSIsnprintf( buffer, NUM_ARRAY_ELEMENTS( buffer ), "%s %s", OSIClanRoleToStr( member->role ),
			GetFullPlayerName( nameBuffer, player->clanInfo.tag,  member->memberName ) );
		pTable->AddItem( buffer, row, 0 );
		if( member->currentMatchList->count() > 0 ) 
		{
			member->currentMatchList->begin();
			OSIMatch* memberMatch = member->currentMatchList->getCurrent();
			if( memberMatch->maximumPlayers > 0 && memberMatch->numberOfPlayers > 0 )
			{
				// member is currently in a game, display info about that game
				pTable->SetItem( row, 1, memberMatch->options.description ); // Match column
				const char* const modeString = COnlineSessionSearch::GameTypeIndexToStringLUT[memberMatch->options.gameTypeIndex];
				OSIsnprintf( buffer, NUM_ARRAY_ELEMENTS(buffer), "%s - %s", memberMatch->options.levelName, modeString );
				pTable->SetItem( row, 2, buffer ); // Game column
				OSIsnprintf(buffer, NUM_ARRAY_ELEMENTS(buffer), "%i / %i", memberMatch->options.totalConnectedPlayers, memberMatch->maximumPlayers);
				pTable->SetItem( row, 3, buffer ); // Players column
			}
			else
			{
				//pTable->SetItem( row, 1, "Playing...");
			}
		}
		else 
		{
			// member is not in a game but is member online?
			if( member->isOnline )
			{
				pTable->SetItem( row, 1, "Online" );
			}
			else
			{
				pTable->SetItem( row, 1, "Offline" );
			}
		}

		memberList.next();
	}
}

void ScreenClanList::MoveCursor( int numRowsToMove )
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
