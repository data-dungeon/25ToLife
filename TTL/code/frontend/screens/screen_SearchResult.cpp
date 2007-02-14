#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "frontend/screens/screen_SearchResult.h"
#include "data/screens/OnlineScreens.h"
#include "environ/ConfigMgr.h"
#include "Network/SessionSpec.h"
#include "main/Lobby.h"
#include "GameHelper/ErrorStack.h"
#include "OSI/OSI.hpp"

#define SEARCH_REFRESH_PERIOD  5000
#define SEARCH_TIMEOUT         1000

//=============================================================================
// ScreenSearchResult::ScreenSearchResult - The default constructor
//=============================================================================

ScreenSearchResult::ScreenSearchResult( void )
{
}

//=============================================================================
// ScreenSearchResult::~ScreenSearchResult - The default destructor
//=============================================================================

ScreenSearchResult::~ScreenSearchResult( void )
{
}

//=============================================================================
// ScreenSearchResult::SetParam
//=============================================================================

void ScreenSearchResult::SetSessionParam( int ParamId, const char* pParamText )
{
	Text* pText = GetTextSprite( ID_SEARCHRESULT_PARAM, GetSprite( ParamId ) );

	if( pText )
		pText->SetText( pParamText );

	return;
}

//=============================================================================
// ScreenSearchResult::SetTimeLimit
//=============================================================================

void ScreenSearchResult::SetTimeLimit( const char* pTimeLimit )
{
	SetSessionParam( ID_SEARCHRESULT_TIMELIMIT, pTimeLimit );
}

//=============================================================================
// ScreenSearchResult::SetTimeLimit
//=============================================================================

void ScreenSearchResult::SetTimeLimit( int Minutes, int Seconds )
{
	Minutes+= Seconds / 60;
	Seconds = Seconds % 60;

	if( Seconds == 0 )
		SetTimeLimit( ::Printf( "%d Minutes", Minutes ) );
	else
		SetTimeLimit( ::Printf( "%d Min %d Sec", Minutes, Seconds ) );

	return;
}

//=============================================================================
// ScreenSearchResult::SetScoreLimit
//=============================================================================

void ScreenSearchResult::SetScoreLimit( const char* pScoreLimit )
{
	SetSessionParam( ID_SEARCHRESULT_SCORELIMIT, pScoreLimit );
}

//=============================================================================
// ScreenSearchResult::SetScoreLimit
//=============================================================================

void ScreenSearchResult::SetScoreLimit( int ScoreLimit )
{
	SetScoreLimit( ::Printf( "%d Points", ScoreLimit ) );
}

//=============================================================================
// ScreenSearchResult::ClearSearchResults
//=============================================================================

void ScreenSearchResult::ClearSearchResults( void )
{
	GuiTable* pTable = GetTable();

	if( pTable )
		pTable->DeleteAllItems();

	return;
}

//=============================================================================
// ScreenSearchResult::OnInitialize
//=============================================================================

void ScreenSearchResult::OnInitialize( void )
{
	GuiTable* pTable = CreateTable( ID_SEARCHRESULT_SESSIONLIST );

	if( pTable )
	{
		pTable->InitControl( this );

		pTable->InitTable( 8, 5 );
		pTable->InitItem( 0, 0, GetTextSprite( ID_SEARCHRESULT_SESSION1 ) );
		pTable->InitItem( 1, 0, GetTextSprite( ID_SEARCHRESULT_SESSION2 ) );
		pTable->InitItem( 2, 0, GetTextSprite( ID_SEARCHRESULT_SESSION3 ) );
		pTable->InitItem( 3, 0, GetTextSprite( ID_SEARCHRESULT_SESSION4 ) );
		pTable->InitItem( 4, 0, GetTextSprite( ID_SEARCHRESULT_SESSION5 ) );
		pTable->InitItem( 5, 0, GetTextSprite( ID_SEARCHRESULT_SESSION6 ) );
		pTable->InitItem( 6, 0, GetTextSprite( ID_SEARCHRESULT_SESSION7 ) );
		pTable->InitItem( 7, 0, GetTextSprite( ID_SEARCHRESULT_SESSION8 ) );

		pTable->InitItem( 0, 1, GetTextSprite( ID_SEARCHRESULT_MAP1 ) );
		pTable->InitItem( 1, 1, GetTextSprite( ID_SEARCHRESULT_MAP2 ) );
		pTable->InitItem( 2, 1, GetTextSprite( ID_SEARCHRESULT_MAP3 ) );
		pTable->InitItem( 3, 1, GetTextSprite( ID_SEARCHRESULT_MAP4 ) );
		pTable->InitItem( 4, 1, GetTextSprite( ID_SEARCHRESULT_MAP5 ) );
		pTable->InitItem( 5, 1, GetTextSprite( ID_SEARCHRESULT_MAP6 ) );
		pTable->InitItem( 6, 1, GetTextSprite( ID_SEARCHRESULT_MAP7 ) );
		pTable->InitItem( 7, 1, GetTextSprite( ID_SEARCHRESULT_MAP8 ) );

		pTable->InitItem( 0, 2, GetTextSprite( ID_SEARCHRESULT_MODE1 ) );
		pTable->InitItem( 1, 2, GetTextSprite( ID_SEARCHRESULT_MODE2 ) );
		pTable->InitItem( 2, 2, GetTextSprite( ID_SEARCHRESULT_MODE3 ) );
		pTable->InitItem( 3, 2, GetTextSprite( ID_SEARCHRESULT_MODE4 ) );
		pTable->InitItem( 4, 2, GetTextSprite( ID_SEARCHRESULT_MODE5 ) );
		pTable->InitItem( 5, 2, GetTextSprite( ID_SEARCHRESULT_MODE6 ) );
		pTable->InitItem( 6, 2, GetTextSprite( ID_SEARCHRESULT_MODE7 ) );
		pTable->InitItem( 7, 2, GetTextSprite( ID_SEARCHRESULT_MODE8 ) );

		pTable->InitItem( 0, 3, GetTextSprite( ID_SEARCHRESULT_PLAYERS1 ) );
		pTable->InitItem( 1, 3, GetTextSprite( ID_SEARCHRESULT_PLAYERS2 ) );
		pTable->InitItem( 2, 3, GetTextSprite( ID_SEARCHRESULT_PLAYERS3 ) );
		pTable->InitItem( 3, 3, GetTextSprite( ID_SEARCHRESULT_PLAYERS4 ) );
		pTable->InitItem( 4, 3, GetTextSprite( ID_SEARCHRESULT_PLAYERS5 ) );
		pTable->InitItem( 5, 3, GetTextSprite( ID_SEARCHRESULT_PLAYERS6 ) );
		pTable->InitItem( 6, 3, GetTextSprite( ID_SEARCHRESULT_PLAYERS7 ) );
		pTable->InitItem( 7, 3, GetTextSprite( ID_SEARCHRESULT_PLAYERS8 ) );

		pTable->InitItem( 0, 4, GetTextSprite( ID_SEARCHRESULT_PING1 ) );
		pTable->InitItem( 1, 4, GetTextSprite( ID_SEARCHRESULT_PING2 ) );
		pTable->InitItem( 2, 4, GetTextSprite( ID_SEARCHRESULT_PING3 ) );
		pTable->InitItem( 3, 4, GetTextSprite( ID_SEARCHRESULT_PING4 ) );
		pTable->InitItem( 4, 4, GetTextSprite( ID_SEARCHRESULT_PING5 ) );
		pTable->InitItem( 5, 4, GetTextSprite( ID_SEARCHRESULT_PING6 ) );
		pTable->InitItem( 6, 4, GetTextSprite( ID_SEARCHRESULT_PING7 ) );
		pTable->InitItem( 7, 4, GetTextSprite( ID_SEARCHRESULT_PING8 ) );
	}

	GuiScrollBar* pScrollBar = CreateScrollBar( ID_SEARCHRESULT_SCROLLBAR );

	if( pScrollBar )
	{
		pScrollBar->InitPart( GuiScrollBar::Shaft,     GetSprite( ID_SEARCHRESULT_SCROLLBAR ) );
		pScrollBar->InitPart( GuiScrollBar::Thumb,     GetSprite( ID_SEARCHRESULT_SBTHUMB ) );
		pScrollBar->InitPart( GuiScrollBar::ArrowUp,   GetSprite( ID_SEARCHRESULT_SBARROWUP ) );
		pScrollBar->InitPart( GuiScrollBar::ArrowDown, GetSprite( ID_SEARCHRESULT_SBARROWDOWN ) );

		pScrollBar->SetRange( 0.0f, 1.0f );
		pScrollBar->SetPosition( 0.5f );
		pScrollBar->SetSteps( 10 );
		pScrollBar->SetPageSize(1);
	}

	SetHelpRect( ID_SEARCHRESULT_HELPRECT );

#ifdef DIRECTX_PC
	GetSprite(ID_SEARCHRESULT_HELPRECT)->HideAll();
	ScreenBaseClass::OnInitialize();
	// we're going to hijack our start button on this screen
	// to act as a refresh button for the result list
	StartButton.SetText("Refresh");
	StartButton.SetPosition( 100, 420 );

#endif

	return;
}

//=============================================================================
// ScreenSearchResult::OnCursorUp
//=============================================================================

void ScreenSearchResult::OnCursorUp( void )
{
	GuiTable* pTable = GetTable();

#ifdef DIRECTX_PC
	ScreenBaseClass::OnCursorUp();
#endif

	if( pTable )
	{
		int R1, C1;
		int R2, C2;

		pTable->GetCursorPos( R1, C1 );
		pTable->MoveCursor( 0, -1 );
		pTable->GetCursorPos( R2, C2 );

		if( R1 != R2 || C1 != C2 )
			PlaySoundEvent( Screen::SOUNDEVENT_PREVBUTTON );

		OnCursorUpdate();
	}

	return;
}

//=============================================================================
// ScreenSearchResult::OnCursorDown
//=============================================================================

void ScreenSearchResult::OnCursorDown( void )
{
	GuiTable* pTable = GetTable();

#ifdef DIRECTX_PC
	ScreenBaseClass::OnCursorDown();
#endif

	if( pTable )
	{
		int R1, C1;
		int R2, C2;

		pTable->GetCursorPos( R1, C1 );
		pTable->MoveCursor( 0, 1 );
		pTable->GetCursorPos( R2, C2 );

#ifndef DIRECTX_PC
		if( R1 != R2 || C1 != C2 )
			PlaySoundEvent( Screen::SOUNDEVENT_NEXTBUTTON );
#endif 
		OnCursorUpdate();
	}

	return;
}

//=============================================================================
// ScreenSearchResult::OnCursorLeft
//=============================================================================

void ScreenSearchResult::OnCursorLeft( void )
{
	GuiTable* pTable = GetTable();

	if( pTable )
	{
		int R1, C1;
		int R2, C2;

		pTable->GetCursorPos( R1, C1 );
		pTable->MoveCursor( -1, 0 );
		pTable->GetCursorPos( R2, C2 );

		if( R1 != R2 || C1 != C2 )
			PlaySoundEvent( Screen::SOUNDEVENT_PREVBUTTON );

		OnCursorUpdate();
	}

	return;
}

//=============================================================================
// ScreenSearchResult::OnCursorRight
//=============================================================================

void ScreenSearchResult::OnCursorRight( void )
{
	GuiTable* pTable = GetTable();

	if( pTable )
	{
		int R1, C1;
		int R2, C2;

		pTable->GetCursorPos( R1, C1 );
		pTable->MoveCursor( 1, 0 );
		pTable->GetCursorPos( R2, C2 );

		if( R1 != R2 || C1 != C2 )
			PlaySoundEvent( Screen::SOUNDEVENT_NEXTBUTTON );

		OnCursorUpdate();
	}

	return;
}
//=============================================================================
// ScreenSearchResult::OnCursorUpdate
//=============================================================================

void ScreenSearchResult::OnCursorUpdate( void )
{
	//GuiListBox* pListBox = GetTable();

	//if( pListBox )
	//{
	//	GuiListBox::Item* pItem = pListBox->GetItem( pListBox->CurrentItem() );

	//	if( pItem )
	//	{
	//		// TODO: Set the session details boxes
	//	}
	//}

	UpdateSessionInformation();

	return;
}

//=============================================================================
// ScreenSearchResult::OnButtonOK
//=============================================================================

void ScreenSearchResult::OnButtonOK( void )
{
	GuiTable* pTable = GetTable();

	if( pTable && pTable->GetItemCount() )
	{
		int n = pTable->CurrentSelection();

		if( ( n == ~0 ) || ( g_lobby.SessionSearch().GetConfig( n ) == NULL ) || pTable->IsCurrentSelectionDisabled() )
		{
			PlaySoundEvent( Screen::SOUNDEVENT_INVALID );
		}
		else
		{
			// TODO: If the list is sorted, n will be invalid.  Tag each entry
			// in the list with its original value (which will be correct).
			g_lobby.SessionSearch().SpecifyFoundSession( n, g_sessionSpec );
			updateStatus = END_LAUNCH;
			// wait screen?
		}
	}

	return;
}

//=============================================================================
// ScreenSearchResult::OnButtonCancel
//=============================================================================

void ScreenSearchResult::OnButtonCancel( void )
{
	updateStatus = END_CANCEL;
}

//=============================================================================
// ScreenSearchResult::OnButtonPressed
//=============================================================================

void ScreenSearchResult::OnButtonPressed( int Button )
{
	switch( Button )
	{
	case FluffControlMapper::SQUARE:
		g_messageDispatcher.SendMessageToAll( "SearchForMatches", (void *)true, INVALID_OBJECT_HANDLE );
		m_reFresh = true;
		break;

	default:
		ScreenBaseClass::OnButtonPressed( Button );
		break;
	}

	return;
}

//=============================================================================
// ScreenSearchResult::UpdateSessionInformation
//=============================================================================

void ScreenSearchResult::UpdateSessionInformation( void )
{
	SetTimeLimit( "" );
	SetScoreLimit( "" );

	GuiTable* pTable = GetTable();
	int iOptionIndex;
	int n;

	if( pTable && pTable->GetItemCount() > 0 )
	{
		n = pTable->CurrentSelection();

		if( ( n != ~0 ) && ( g_lobby.SessionSearch().GetConfig( n ) != NULL ) )
		{
			const char *data;
			g_configmgr.ProposeOptions( g_lobby.SessionSearch().GetConfig( n ) );

			iOptionIndex = g_configmgr.GetOptionIndexByName( "timeLimit" );
			if ( iOptionIndex >= 0 )
			{
				data = g_configmgr.GetDisplayValue( iOptionIndex );
				ASSERTS( data != NULL, "why is this data bad?" );
				if( data )
					SetTimeLimit( data );
			}

			iOptionIndex = g_configmgr.GetOptionIndexByName( "scoreLimit" );
			if ( iOptionIndex >= 0 )
			{
				data = g_configmgr.GetDisplayValue( iOptionIndex );
				ASSERTS( data != NULL, "why is this data bad?" );
				if( data )
					SetScoreLimit( data );
			}

			g_configmgr.RejectChanges();
		}
	}

	return;
}

//=============================================================================
// ScreenSearchResult::OnScreenUpdate
//=============================================================================
void ScreenSearchResult::OnScreenUpdate( void )
{
	GuiTable* pTable = GetTable();

	if ( g_errorStack.IsFatal() )
		updateStatus = END_CANCEL;

	if( updateStatus != UPDATE )
	{
		CSessionSearch::EEndStatus result = g_lobby.SessionSearch().End();
		if (result != CSessionSearch::SEARCH_IN_PROGRESS)
		{
			if( updateStatus == END_LAUNCH )
				g_messageDispatcher.SendMessageToAll( "SetupLaunch", NULL, INVALID_OBJECT_HANDLE );
			else
				g_messageDispatcher.SendMessageToAll( "CancelSearchResults", NULL, INVALID_OBJECT_HANDLE );
			// failure?
		}
	}
	else if( g_lobby.SessionSearch().Update( m_reFresh ) )
	{
		GuiTable* pTable = GetTable();

		if( pTable )
		{
			pTable->DeleteAllItems();

		//	for( int nn = 0; nn < 50; nn++ )
		//		pTable->AddItem( ::Printf( "Just a quickie %d", nn ), -1, 0 );

			unsigned int iResultCount = g_lobby.SessionSearch().GetNumFound();
			for ( unsigned int iResult = 0; iResult < iResultCount; ++iResult )
			{
				// Disable bad matches
				bool disable = false;
				if( ( strcmp( g_lobby.SessionSearch().GetPing(iResult), "9999" ) == 0 ) || g_lobby.SessionSearch().GetFilled(iResult) )
					disable = true;
				// Make sure a session can provide a config string.
				if( g_lobby.SessionSearch().GetConfig( iResult ) != NULL )
				{
					pTable->AddItem( g_lobby.SessionSearch().GetName(iResult), iResult, 0, disable );

					// If the mapName is blank, use the map index.  There's no need to transmit
					// a full string for the map name when an index will do the trick.
					const char* const sessionMapName = g_lobby.SessionSearch().GetMapName( iResult );
					if( sessionMapName && sessionMapName[ 0 ] )
					{
						pTable->SetItem( iResult, 1, sessionMapName, disable );
					}
					else
					{
						const int mapIndex = g_lobby.SessionSearch().GetMapIndex( iResult );
						if (mapIndex < 0)
							pTable->SetItem( iResult, 1, "Unknown", disable );
						else
						{				
							const char* const convertedMapName = g_gameEnvironment.GetWorldDisplayName( mapIndex );
							pTable->SetItem( iResult, 1, convertedMapName, disable );
						}
					}
				
					pTable->SetItem( iResult, 2, g_lobby.SessionSearch().GetMode(iResult), disable );
					pTable->SetItem( iResult, 3, g_lobby.SessionSearch().GetPlayers(iResult), disable );
					pTable->SetItem( iResult, 4, g_lobby.SessionSearch().GetPing(iResult), disable );
				}
				else
				{
					char entry[80];
					sprintf( entry, "%s[Invalid]", g_lobby.SessionSearch().GetName(iResult) );
					pTable->AddItem( entry, iResult, 0 );
					pTable->SetItem( iResult, 1, "" );
					pTable->SetItem( iResult, 2, "" );
					pTable->SetItem( iResult, 3, "" );
					pTable->SetItem( iResult, 4, "" );
				}
			}

			// reset the session detail display
			UpdateSessionInformation();
		}
	}
#ifdef DIRECTX_PC
	if (g_MenuCursor.IsActive()) {
		float x1, y1, x2, y2;	
		// check for back navigation
		if (g_MenuCursor.CursorButtonPressed(1))
		{
			g_MenuCursor.ForceButtonUp(1);
			OnButtonCancel();
		}
		float x = g_MenuCursor.GetX();
		float y = g_MenuCursor.GetY();
		if (StartButton.IsVisible())
		{
			StartButton.Update();
			if ( g_MenuCursor.CursorButtonPressed(0) )
			{
				StartButton.GetFamilyRect(x1, y1, x2, y2);
				if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) )
				{
					OnButtonPressed(FluffControlMapper::SQUARE);
					return;
				}
			}
		}
		if (BackButton.IsVisible())
		{
			BackButton.Update();
			if ( g_MenuCursor.CursorButtonPressed(0) )
			{
				BackButton.GetFamilyRect(x1, y1, x2, y2);
				if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) )
				{
					OnButtonCancel();
					return;
				}
			}
		}
		// functionality for list box based menus
		GuiTable* table = GetTable();
		if (table)
		{
			int rows = table->GetRows();
			//int rows = table->GetNumRows();
			for (int r=0; r < rows; ++r)
			{
				int cols = table->GetColumns(r);
				//int cols = table->GetNumColumns(r);
				for (int c=0; c < cols; ++c)
				{
					table->GetDisplayItem(r,c)->GetDisplayItem()->GetFamilyRect(x1,y1,x2,y2);
					if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) && ( table->GetItemCount() > r ))
					{
						table->SetCursorPos(r,0);
						if ( g_MenuCursor.CursorButtonPressed(0) )
							OnButtonOK();
						break;
					}
				}
			}
		}
	}
#endif
	ScreenBaseClass::OnScreenUpdate( );
	m_reFresh = false;
}

//=============================================================================
// ScreenSearchResult::OnScreenIntro
//=============================================================================
void ScreenSearchResult::OnScreenIntro( void )
{
	if( OSI::getSingletonPtr() == NULL )
	{
		// We are in system link, disable refresh help text
		if( m_pScreen )
		{
			//m_pScreen->GetSprite( ID_SEARCHRESULT_HELPTEXT3 )->HideAll();
		}
	}
	else
	{
		if( m_pScreen )
		{
			//m_pScreen->GetSprite( ID_SEARCHRESULT_HELPTEXT3 )->ShowAll();
		}
	}
	ScreenBaseClass::OnScreenIntro();
	ClearSearchResults();
	g_lobby.SessionSearch().SetQueryPeriodMS(SEARCH_REFRESH_PERIOD);
	g_lobby.SessionSearch().SetQueryTimeoutMS(SEARCH_TIMEOUT);
	g_lobby.SessionSearch().Begin();
	updateStatus = UPDATE;
	UpdateSessionInformation();
	m_reFresh = true;
#ifdef DIRECTX_PC
	BackButton.Show(true);
	StartButton.Show(true);
#endif
	return;
} 

//=============================================================================
// ScreenSearchResult::OnScreenExit
//=============================================================================
void ScreenSearchResult::OnScreenExit( void )
{
	ScreenBaseClass::OnScreenExit();
	g_lobby.SessionSearch().End();
	m_reFresh = false;
} 
