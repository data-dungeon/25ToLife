#include "TTLPCH.h"
#include "environ/ConfigMgr.h"
#include "frontend/inputdriver.h"
#include "frontend/screens/screen_Popups.h"
#include "frontend/screens/screen_GameSetting.h"
#include "GameHelper/Translator.h"

#include "data/screens/NetworkGames.h"
#include "main/lobby.h"

#ifdef DIRECTX_PC
#define COMMAND_QUIT_OK 1
#define COMMAND_QUIT_CANCEL 2
#endif

enum
{
	COMMAND_CHANGES_CANCEL = 0x100,
	COMMAND_CHANGES_OK,
};

//=============================================================================
// ScreenGameSetting::ScreenGameSetting
//=============================================================================

ScreenGameSetting::ScreenGameSetting()
{
	SetReadOnly( true );
}

//=============================================================================
// ScreenGameSetting::GetListBox
//=============================================================================

GuiListBox* ScreenGameSetting::GetListBox( void )
{
	return (GuiListBox*) GetInterface( ID_GAMESETTING_MENU, GuiListBox::ClassnameStatic() );
}

//=============================================================================
// ScreenGameSetting::SetReadOnly
//=============================================================================

void ScreenGameSetting::SetReadOnly( bool ReadOnly )
{
	m_ReadOnly = ReadOnly;

	if( ReadOnly )
		ShowSprite( ID_GAMESETTING_HELPTEXT2, false );
	else
		ShowSprite( ID_GAMESETTING_HELPTEXT2, true );

	return;
}

//=============================================================================
// ScreenGameSetting::GetReadOnly
//=============================================================================

bool ScreenGameSetting::GetReadOnly( void )
{
	return m_ReadOnly;
}

//=============================================================================
// ScreenGameSetting::GetCursorPos
//=============================================================================

void ScreenGameSetting::GetCursorPos( int n, float &x, float &y )
{
	int CursorId = 0;
	x = 0.0f;
	y = 0.0f;

#ifndef DIRECTX_PC
	switch( n )
	{
	case 0:
		CursorId = ID_GAMESETTING_LOWCURSOR1;
		break;

	case 1:
		CursorId = ID_GAMESETTING_LOWCURSOR2;
		break;

	case 2:
		CursorId = ID_GAMESETTING_LOWCURSOR3;
		break;

	case 3:
		CursorId = ID_GAMESETTING_LOWCURSOR4;
		break;
	}
#endif // DIRECTX_PC

	Sprite* pCursor = GetSprite( CursorId );

	if( pCursor )
		pCursor->GetPosition( x, y );

	return;
}

//=============================================================================
// ScreenGameSetting::UseDefaultSettings
//=============================================================================

void ScreenGameSetting::UseDefaultSettings( void )
{
	g_configmgr.Default();
}

//=============================================================================
// ScreenGameSetting::OnInitialize
//=============================================================================

void ScreenGameSetting::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	SetHelpRect( ID_GAMESETTING_HELPRECT );

	GuiListBox* pListBox = CreateListBox( ID_GAMESETTING_MENU );

	if( pListBox )
	{
		pListBox->InitControl( this );

		Sprite* pMenu = GetSprite( ID_GAMESETTING_MENU );
		
		pListBox->InitItem( GetTextSprite( ID_GAMESETTING_LABEL, GetSprite( ID_GAMESETTING_SETTING1, pMenu ) ) );
		pListBox->InitItem( GetTextSprite( ID_GAMESETTING_LABEL, GetSprite( ID_GAMESETTING_SETTING2, pMenu ) ) );
		pListBox->InitItem( GetTextSprite( ID_GAMESETTING_LABEL, GetSprite( ID_GAMESETTING_SETTING3, pMenu ) ) );
		pListBox->InitItem( GetTextSprite( ID_GAMESETTING_LABEL, GetSprite( ID_GAMESETTING_SETTING4, pMenu ) ) );
	}

	GuiScrollBar* pScrollBar = CreateScrollBar( ID_GAMESETTING_SCROLLBAR );

	if( pScrollBar )
	{
		Sprite* pSprite = pScrollBar->SpriteHost();

		pScrollBar->InitPart( GuiScrollBar::Shaft, pSprite );
		pScrollBar->InitPart( GuiScrollBar::Thumb, GetSprite( ID_GAMESETTING_SBTHUMB, pSprite ) );
		pScrollBar->InitPart( GuiScrollBar::ArrowUp, GetSprite( ID_GAMESETTING_SBARROWUP, pSprite ) );
		pScrollBar->InitPart( GuiScrollBar::ArrowDown, GetSprite( ID_GAMESETTING_SBARROWDOWN, pSprite ) );

		pScrollBar->SetRange( 0.0f, 10.0f );
		pScrollBar->SetPosition( 5.0f );
		pScrollBar->SetSteps( 10 );

		if( pListBox )
			pScrollBar->SetPageSize( pListBox->GetItemDisplayCount() );
	}

#ifdef DIRECTX_PC
	GetSprite(ID_GAMESETTING_HELPRECT)->HideAll();
	StartButton.SetText("Continue");
	StartButton.Show(true);
	BackButton.Show(true);
#endif

	m_InitMenu = true;

	m_GameOptionsChangedHandler.Initialize( "GameOptionsChanged", MESSAGE_HANDLE_IF_MASTER, m_messageOwner, this );
}

//=============================================================================
// ScreenGameSetting::OnScreenIntro
//=============================================================================

void ScreenGameSetting::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();

	Printf( ID_GAMESETTING_CURRENTPROFILE, g_translator.Translate( "Current Profile: %s" ), g_playerProfile.GetAccountName() );

	bool ReadOnly = false;

	if( ValidSingleton(CSession) )
	{
		if( !g_session.IsMaster() )
			ReadOnly = true;
	}

	SetReadOnly( ReadOnly );
#ifdef DIRECTX_PC
	StartButton.Show(true);
	BackButton.Show(true);
#endif
}

//=============================================================================
// ScreenGameSetting::OnScreenExit
//=============================================================================

void ScreenGameSetting::OnScreenExit( void )
{
	ScreenBaseClass::OnScreenExit();
}

//=============================================================================
// ScreenGameSetting::OnCursorUp
//=============================================================================

void ScreenGameSetting::OnCursorUp( void )
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
// ScreenGameSetting::OnCursorDown
//=============================================================================

void ScreenGameSetting::OnCursorDown( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int OldPos = pListBox->CurrentItem();

		pListBox->OnCursorDown();

		int NewPos = pListBox->CurrentItem();
#ifndef DIRECTX_PC
		if( OldPos != NewPos )
			PlaySoundEvent( Screen::SOUNDEVENT_PREVBUTTON );

#endif
#ifdef DIRECTX_PC
		RebuildOptionList();
#endif
	}

	return;
}

//=============================================================================
// ScreenGameSetting::OnCursorLeft
//=============================================================================

void ScreenGameSetting::OnCursorLeft( void )
{
	OnButtonCycle(-1);
}

//=============================================================================
// ScreenGameSetting::OnCursorRight
//=============================================================================

void ScreenGameSetting::OnCursorRight( void )
{
	OnButtonCycle(1);
}

//=============================================================================
// ScreenGameSetting::OnButtonOK
//=============================================================================

void ScreenGameSetting::OnButtonOK( void )
{
	if( !GetReadOnly() )
	{
		g_configmgr.AcceptChanges();
		g_messageDispatcher.SendMessageToAll( "SetupCreateSession", NULL, INVALID_OBJECT_HANDLE );
	}


	ScreenManager* pScreenManager = GetScreenManager();
	Screen* pPrevScreen = 0;

	if( pScreenManager )
	{
		ScreenStack* pScreenStack = pScreenManager->GetScreenStack();

		if( pScreenStack  )
			pPrevScreen = pScreenStack->GetScreen();
	}

	if( pPrevScreen == GetScreen( "SCREEN_ID_CREATESESSION" ) )
		GotoScreen(0);
	else
		GotoPrevScreen();

	return;
}

//=============================================================================
// ScreenGameSetting::OnButtonCancel
//=============================================================================

void ScreenGameSetting::OnButtonCancel( void )
{
	g_configmgr.RejectChanges();
	
	ScreenBaseClass::OnButtonCancel();
}

//=============================================================================
// ScreenGameSetting::OnButtonPressed
//=============================================================================

void ScreenGameSetting::OnButtonPressed( int Button )
{
	switch( Button )
	{
	case FluffControlMapper::SQUARE:
		if( !m_ReadOnly )
			UseDefaultSettings();
		break;
	default:
		ScreenBaseClass::OnButtonPressed( Button );
		break;
	}

	return;
}

//=============================================================================
// ScreenGameSetting::OnButtonCycle
//=============================================================================

void ScreenGameSetting::OnButtonCycle( int N )
{
	if( !GetReadOnly() )
	{
		GuiListBox* pListBox = GetListBox();

		if( pListBox )
		{
			int i = pListBox->CurrentSelection();

			const char* pNewValue = 0;
			const char* pOldValue = g_configmgr.GetDisplayValue(i);
			const char* pKey = g_configmgr.GetDisplayKey(i);

			if( N > 0 )
				g_configmgr.Increment(i);
			else
			if( N < 0 )
				g_configmgr.Decrement(i);

			// the option list may have completely changed by altering this option
			RebuildOptionList();
			m_InitMenu = true;

			i = ( int )g_configmgr.GetOptionIndexByDisplayKey( pKey );
			if ( i == -1 )
				PlaySoundEvent( Screen::SOUNDEVENT_INVALID );
			else
			{		
				pNewValue = g_configmgr.GetDisplayValue(i);

				if( pOldValue && pNewValue )
				{
					if( strcmp( pOldValue, pNewValue ) )
						PlaySoundEvent( Screen::SOUNDEVENT_INVALID );
				}
			}
		}
	}

	return;
}

//=============================================================================
// ScreenGameSetting::OnScreenUpdate
//=============================================================================
void ScreenGameSetting::OnScreenUpdate( void )
{
#ifdef DIRECTX_PC
	if (g_MenuCursor.IsActive()) 
	{
		float x1, y1, x2, y2;	
		// check for back navigation
		if (g_MenuCursor.CursorButtonPressed(1))
		{
			g_MenuCursor.ForceButtonUp(1);
			OnButtonCancel();
		}
		float x = g_MenuCursor.GetX();
		float y = g_MenuCursor.GetY();
		// functionality for list box based menus
		GuiListBox* listBox = GetListBox();
		if (listBox)
		{
			// search through the number of displayed items
			//int cursorPos = listBox->GetCursorPos();
			int numDisplayItems = listBox->GetItemDisplayCount();
			listBox->GetListBoxRect(x1,y1,x2,y2);
			float yDimSpacing = ( y2 - y1 ) * 0.25f;
			for (int c=0; c < numDisplayItems; ++c)
			{
				float newY1 = y1+ ( yDimSpacing * c );
				float newY2 = y1+ yDimSpacing + ( yDimSpacing * c );
				if ( ( x > x1 && x < x2 ) && ( y > newY1 && y < newY2 ) )
				{
					listBox->SelectItem( c + listBox->GetTopItem() );
					if ( g_MenuCursor.CursorButtonPressed(0) )
						OnCursorRight();
					break;
				}
			}
		}
		Button* button = GetDefaultButton();
		// functionality for buttons
		if (!button)
		{
			if (g_MenuCursor.CursorButtonPressed(0))
				OnButtonStart();
		}
		// search through the buttons to see if we're over one
		// if so, check to see if we're clicking the mouse
		while(button) 
		{
			button->SpriteHost()->GetFamilyRect(x1, y1, x2, y2);
			if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) )
			{
				if (!button->Enabled()) break;
				SelectButton(button);
				if ( g_MenuCursor.CursorButtonPressed(0) )
				{
						g_MenuCursor.ForceButtonUp(0);
						OnButtonOK(); 
				}
				break;
			}
			button = button->GetNextButton();
		}
	}
	// add support for scroll wheel
	int mouseWheelState = g_MenuCursor.GetMouseWheelState();
	if (mouseWheelState == 1) OnCursorUp();
	else if (mouseWheelState == -1) OnCursorDown();

	//we've already got specialized screen handling code here
	// so just cut/paste the start/back button code here for
	// now to avoid redundancy :(
	// draw our start and back buttons, and test
	// to see if our mouse has interacted with them
	float x1, y1, x2, y2;
	float x = g_MenuCursor.GetX();
	float y = g_MenuCursor.GetY();
	if (StartButton.IsVisible())
	{
		StartButton.Update();
		if ( g_MenuCursor.CursorButtonPressed(0) )
		{
			StartButton.GetFamilyRect(x1, y1, x2, y2);
			if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) )
				OnButtonOK();
		}
	}
	if (BackButton.IsVisible())
	{
		BackButton.Update();
		if ( g_MenuCursor.CursorButtonPressed(0) )
		{
			BackButton.GetFamilyRect(x1, y1, x2, y2);
			if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) )
				OnButtonCancel();
		}
	}
	if (QuitButton.IsVisible())
	{
		QuitButton.Update();
		if ( g_MenuCursor.CursorButtonPressed(0) )
		{
			QuitButton.GetFamilyRect(x1, y1, x2, y2);
			if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) )
			{
				// do popup stuff here
				ScreenPopupDialog* pDialog = GetPopupDialog();
				if( pDialog )
				{
					pDialog->Clear();
					pDialog->SetTitle( "Quit" );
					pDialog->SetMessage( "Are you sure you want to Quit?" );
					pDialog->AddMenuItem( "Quit", COMMAND_QUIT_OK, 0 );
					pDialog->AddMenuItem( "Cancel", COMMAND_QUIT_CANCEL, 0 );
					pDialog->SelectMenuItem( COMMAND_QUIT_CANCEL );
				}
				ShowPopupDialog();
			}
				
		}
	}
#endif

	//
	// this screen requires specialized cursor code, fun fun...
	//

	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		float cx, cy;
		int n = pListBox->GetCursorPos();

		GetCursorPos( n, cx, cy );

		Sprite* pCursor = GetSprite( ID_GAMESETTING_CURSOR );

		if( pCursor )
			pCursor->SetPosition( cx, cy );

		Text* pDisplayItem = pListBox->GetDisplayItem(n);
		Sprite* pCursorLeft = GetSprite( ID_GAMESETTING_LCURSOR );
		Sprite* pCursorRight = GetSprite( ID_GAMESETTING_RCURSOR );

		if( pDisplayItem )
		{
			if( pCursorLeft )
				pCursorLeft->SetParent( pDisplayItem->Parent() );

			if( pCursorRight )
			{
				float x1, y1;
				float x2, y2;
				float x3 = pDisplayItem->GetWidth();
				float w;

				pDisplayItem->GetSpriteRect( x1, y1, x2, y2 );

				w = x2 - x1;

				if( w > x3 )
					w = x3;

				pCursorRight->SetParent( pDisplayItem->Parent() );
				pCursorRight->SetPositionX( w + 6.0f );
			}
		}
	}

	if( pListBox )
		SetText( ID_GAMESETTING_DESCRIPTION, g_configmgr.GetDisplayDescription( pListBox->CurrentSelection() ) );
	else
		SetText( ID_GAMESETTING_DESCRIPTION, "" );

	RebuildOptionList();

//	ScreenBaseClass::OnScreenUpdate();
}

//=============================================================================
// ScreenGameSetting::OnDialogCommand
//=============================================================================

void ScreenGameSetting::OnDialogCommand( int Command )
{
	switch( Command )
	{
	case COMMAND_CHANGES_OK:
		GotoPrevScreen();
		break;

	case COMMAND_CHANGES_CANCEL:
		EndScreen(0);	
		break;
	}

	return;
}

//=============================================================================
// ScreenGameSetting::OnPopupShow
//=============================================================================

void ScreenGameSetting::OnPopupShow( void )
{
	ShowScreenExit();
}

//=============================================================================
// ScreenGameSetting::OnPopupHide
//=============================================================================

void ScreenGameSetting::OnPopupHide( void )
{
	ShowScreenIntro();
}

//=============================================================================
// ScreenGameSetting::RebuildOptionList
//=============================================================================

void ScreenGameSetting::RebuildOptionList( void )
{
	GuiListBox* pListBox = GetListBox();
	GuiListBox::Item* pItem;
	int i, n;

	if( pListBox )
	{
		if( m_InitMenu )
		{
			const char* pItemName;
			int CursorPos;

			// remember what option was selected
			xstring szCurSel;
			szCurSel.set( pListBox->GetItemText( pListBox->CurrentItem() ) );

			// get the cursor pos
			CursorPos = pListBox->GetCursorPos();

			// clear existing items
			pListBox->ClearItems();

			// add new items to the list box
			n = g_configmgr.GetCount();

			for( i = 0; i < n; i++ )
			{
				const char *pItem = g_configmgr.GetDisplayKey(i);

				// ---DAS, what a major hack but Sam wants this option to be disabled once we are in the lobby for online
				bool add = true;
				if( g_session.IsNetworked() && g_session.IsOpen() && !g_lobby.IsLAN() && strcmp( pItem, "Reserved Slots" ) == 0 )
					add = false;

				if( add )
				{
					pListBox->AddItem( pItem, i, 0 );
				}
			}

			n = pListBox->GetItemCount();

			// try and re-select the old option
			for( i = 0; i < n; i++ )
			{
				pItemName = pListBox->GetItemText(i);

				if( szCurSel.get() && pItemName )
				{
					if( 0 == strcmp( szCurSel.get(), pItemName ) )
					{
						pListBox->SelectIndex(i);
						pListBox->SetCursorPos(CursorPos);
						break;
					}
				}
			}

			m_InitMenu = false;
		}

		// sync up the item labels
		n = pListBox->GetItemDisplayCount();

		pItem = pListBox->GetItem( pListBox->GetTopItem() );

		Sprite* pParent;
		Text* pLabel;
		Text* pParam;

		for( i = 0; i < n; i++ )
		{
			pParent = 0;
			pLabel = 0;
			pParam = 0;

			pLabel = pListBox->GetDisplayItem(i);

			if( pLabel )
				pParent = pLabel->Parent();

			if( pParent )
				pParam = GetTextSprite( ID_GAMESETTING_PARAM, pParent );

			if( pParam )
			{
				if( pItem )
					pParam->SetText( g_configmgr.GetDisplayValue( pItem->GetItemId() ) );
				else
					pParam->SetText(0);
			}

			if( pItem )
				pItem = pItem->Next();
		}
	}

	return;
}

//=============================================================================
// ScreenGameSetting::GameOptionsChanged::HandleMessage
//=============================================================================

void ScreenGameSetting::GameOptionsChanged::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	d_ref->m_InitMenu = true;
}
