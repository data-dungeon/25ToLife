#include "TTLPCH.h"
#include "main/PlayerDB.h"
#include "weapons/armoryview.h"
#include "frontend/inputdriver.h"
#include "frontend/customplayer.h"
#include "frontend/savegame.h"
#include "frontend/screens/screen_Keyboard.h"
#include "frontend/screens/screen_Popups.h"
#include "frontend/screens/screen_CustomPlayer.h"
#include "GameHelper/Translator.h"
#include "data/screens/CustomPlayer.h"

enum
{
	COMMAND_CLEAR_OK = 0x100,
	COMMAND_CLEAR_CANCEL,
	COMMAND_COPY_OK,
	COMMAND_COPY_CANCEL,
};

//=============================================================================
// ScreenCustomBase::HandleInput
//=============================================================================

void ScreenCustomBase::HandleInput( UIHandler* pUIHandler )
{
	ScreenBaseClass::HandleInput( pUIHandler );

	const float rotateSpeed = 4.0f;

	if( pUIHandler )
	{
		float rotateAngle = 0.0f;

	#ifdef PS2 // On the PlayStation2, we check both trigger buttons

		if( pUIHandler->DeviceButtonDown( FluffControlMapper::L1 ) ||
			 pUIHandler->DeviceButtonDown( FluffControlMapper::L2 ) )
			rotateAngle = -rotateSpeed;
		else
		if( pUIHandler->DeviceButtonDown( FluffControlMapper::R1 ) ||
 	 		 pUIHandler->DeviceButtonDown( FluffControlMapper::R2 ) )
			rotateAngle = rotateSpeed;

	#else // on everything else we only check the primary trigger buttons
#if !defined(DIRECTX_PC)
		if( pUIHandler->DeviceButtonDown( FluffControlMapper::L1 ) )
			rotateAngle = -rotateSpeed;
		else
		if( pUIHandler->DeviceButtonDown( FluffControlMapper::R1 ) )
			rotateAngle = rotateSpeed;
#endif // DIRECTX_PC
	#endif

		// adjust for framerate

		rotateAngle *= 60.0f * g_timer.GetFrameSec();

		// sit and spin!

		if( rotateAngle != 0.0f )
			CustomPlayer::RotateBody( rotateAngle );
	}

	return;
}

//=============================================================================
// ScreenCharSelect2::ScreenCharSelect2
//=============================================================================

ScreenCharSelect2::ScreenCharSelect2( void )
{
}

//=============================================================================
// ScreenCharSelect2::GetListBox
//=============================================================================

GuiListBox* ScreenCharSelect2::GetListBox( void )
{
	return (GuiListBox*) GetInterface( ID_CHARSELECT_LISTBOX, GuiListBox::ClassnameStatic() );
}

//=============================================================================
// ScreenCharSelect2::SetCharType
//=============================================================================

void ScreenCharSelect2::SetCharType( int CharType )
{
	int selected = g_GlobalProfile.GetSelectedCharacter( CharType );
	CustomPlayer::Show( selected );
	OnMenuChange();
}

//=============================================================================
// ScreenCharSelect2::ChangeCharacter
//=============================================================================

void ScreenCharSelect2::ChangeCharacter( int n )
{
	int selected = CustomPlayer::GetVisibleProfile();
	int type = PlayerDB::CharacterType( selected );

	selected = PlayerDB::ChangeCharacter( selected, n, type );

	CustomPlayer::Show( selected );
}

//=============================================================================
// ScreenCharSelect2::OnInitialize
//=============================================================================

void ScreenCharSelect2::OnInitialize( void )
{
	ScreenCustomBase::OnInitialize();
	
	SetHelpRect( ID_CHARSELECT_HELPRECT );

	GuiListBox* pListBox = CreateListBox( ID_CHARSELECT_LISTBOX );

	if( pListBox )
	{
		pListBox->InitControl( this );

		pListBox->InitItem( GetTextSprite( ID_CHARSELECT_NAME1 ) );
		pListBox->InitItem( GetTextSprite( ID_CHARSELECT_NAME2 ) );
		pListBox->InitItem( GetTextSprite( ID_CHARSELECT_NAME3 ) );
		pListBox->InitItem( GetTextSprite( ID_CHARSELECT_NAME4 ) );
		pListBox->InitItem( GetTextSprite( ID_CHARSELECT_NAME5 ) );
		pListBox->InitItem( GetTextSprite( ID_CHARSELECT_NAME6 ) );
		pListBox->InitItem( GetTextSprite( ID_CHARSELECT_NAME7 ) );
		pListBox->InitItem( GetTextSprite( ID_CHARSELECT_NAME8 ) );
		pListBox->InitCursor( GetSprite( ID_CHARSELECT_CURSOR ) );
	}

#ifdef DIRECTX_PC
	GetSprite(ID_CHARSELECT_HELPRECT)->HideAll();
#endif
	return;
}

//=============================================================================
// ScreenCharSelect2::OnMenuChange
//=============================================================================

void ScreenCharSelect2::OnMenuChange( void )
{
	int type = PLAYERDB_COP|PLAYERDB_GANG|PLAYERDB_CUSTOM;
	int index = -1;
	int last  = -1;

	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		pListBox->ClearItems();

		while ( ( index = PlayerDB::ChangeCharacter( index, 1, type ) ) > last )
		{
			last = index;
			if ( PlayerDB::CharacterIsCustom( index ) )
			{
				pListBox->AddItem( PlayerDB::CharacterDisplayName( index ), index, 0 );
			}
		}

		pListBox->SelectItem( CustomPlayer::GetVisibleProfile() );
	}

	return;
}

//=============================================================================
// ScreenCharSelect2::GetIcon
//=============================================================================

Sprite* ScreenCharSelect2::GetIcon( int n )
{
	return GetSprite( GetIconId(n) );
}

//=============================================================================
// ScreenCharSelect2::GetIconId
//=============================================================================

int ScreenCharSelect2::GetIconId( int n )
{
	switch( n ) 
	{
	case 0:
		return ID_CHARSELECT_ICON1;
	case 1:
		return ID_CHARSELECT_ICON2;
	case 2:
		return ID_CHARSELECT_ICON3;
	case 3:
		return ID_CHARSELECT_ICON4;
	case 4:
		return ID_CHARSELECT_ICON5;
	case 5:
		return ID_CHARSELECT_ICON6;
	case 6:
		return ID_CHARSELECT_ICON7;
	case 7:
		return ID_CHARSELECT_ICON8;
	}
	return 0;
}


//=============================================================================
// ScreenCharSelect2::OnScreenIntro
//=============================================================================

void ScreenCharSelect2::OnScreenIntro( void )
{
	ScreenCustomBase::OnScreenIntro();

	// focus on full player
	CustomPlayer::UpdateCamera( "" );

	int CharType = PlayerDB::CharacterType( CustomPlayer::GetVisibleProfile() );
	if ( CharType == PLAYERDB_COP )
		SetCharType( CPlayerProfile::COP );
	else
	if ( CharType == PLAYERDB_GANG )
		SetCharType( CPlayerProfile::GANG );

	Printf( ID_CHARSELECT_PROFILENAME, g_translator.Translate( "Current Profile: %s" ), g_playerProfile.GetAccountName() );
}

//=============================================================================
// ScreenCharSelect2::OnScreenExit
//=============================================================================

void ScreenCharSelect2::OnScreenExit( void )
{
	ScreenCustomBase::OnScreenExit();
}

//=============================================================================
// ScreenCharSelect2::OnCursorUp
//=============================================================================

void ScreenCharSelect2::OnCursorUp( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int OldSel = pListBox->CurrentSelection();

		pListBox->OnCursorUp();

		int NewSel = pListBox->CurrentSelection();

		if( OldSel != NewSel )
		{
			PlaySoundEvent( Screen::SOUNDEVENT_PREVBUTTON );

			CustomPlayer::Show( NewSel );

			int ProfileType = ~0;

			if( PlayerDB::CharacterType( NewSel ) == PLAYERDB_COP )
				ProfileType = CPlayerProfile::COP;
			else
			if( PlayerDB::CharacterType( NewSel ) == PLAYERDB_GANG )
				ProfileType = CPlayerProfile::GANG;
		}
	}

	return;
}

//=============================================================================
// ScreenCharSelect2::OnCursorDown
//=============================================================================

void ScreenCharSelect2::OnCursorDown( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int OldSel = pListBox->CurrentSelection();

		pListBox->OnCursorDown();

		int NewSel = pListBox->CurrentSelection();

		if( OldSel != NewSel )
		{
#ifndef DIRECTX_PC
			PlaySoundEvent( Screen::SOUNDEVENT_PREVBUTTON );
#endif
			CustomPlayer::Show( NewSel );

			int ProfileType = ~0;

			if( PlayerDB::CharacterType( NewSel ) == PLAYERDB_COP )
				ProfileType = CPlayerProfile::COP;
			else
			if( PlayerDB::CharacterType( NewSel ) == PLAYERDB_GANG )
				ProfileType = CPlayerProfile::GANG;
		}
	}

	return;
}

//=============================================================================
// ScreenCharSelect2::OnCursorLeft
//=============================================================================

void ScreenCharSelect2::OnCursorLeft( void )
{
	ScreenCustomBase::OnCursorLeft();
}

//=============================================================================
// ScreenCharSelect2::OnCursorRight
//=============================================================================

void ScreenCharSelect2::OnCursorRight( void )
{
	ScreenCustomBase::OnCursorRight();
}

//=============================================================================
// ScreenCharSelect2::OnButtonOK
//=============================================================================

void ScreenCharSelect2::OnButtonOK( void )
{
	GuiListBox* pListBox = GetListBox();
	CustomPlayer::SetEditProfile( pListBox->CurrentSelection() );
	GotoScreen( "SCREEN_ID_CUSTOMMENU" );
}

//=============================================================================
// ScreenCharSelect2::OnButtonCancel
//=============================================================================

void ScreenCharSelect2::OnButtonCancel( void )
{
	g_messageDispatcher.SendMessageToAll( "EndCustomPlayer", 0, INVALID_OBJECT_HANDLE );

	ScreenCustomBase::OnButtonCancel();
}

//=============================================================================
// ScreenCharSelect2::OnScreenUpdate
//=============================================================================
void ScreenCharSelect2::OnScreenUpdate( void )
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
			float yDimSpacing = ( y2 - y1 ) * (1.0f/listBox->GetItemDisplayCount());
			for (int c=0; c < numDisplayItems; ++c)
			{
				float newY1 = y1+ ( yDimSpacing * c );
				float newY2 = y1+ yDimSpacing + ( yDimSpacing * c );
				if ( ( x > x1 && x < x2 ) && ( y > newY1 && y < newY2 ) )
				{
					listBox->SelectIndex( c + listBox->GetTopItem() );
					CustomPlayer::Show( listBox->CurrentSelection() );
					if ( g_MenuCursor.CursorButtonPressed(0) )
					{
						SelectMenuItem();
						return;
					}
					break;
				}
			}
		}
		// add support for scroll wheel
		int mouseWheelState = g_MenuCursor.GetMouseWheelState();
		if (mouseWheelState == 1) OnCursorUp();
		else if (mouseWheelState == -1) OnCursorDown();
	}
#endif
	ScreenCustomBase::OnScreenUpdate();

	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		Sprite* pIcon;

		int n = pListBox->GetItemDisplayCount();
		int i;
		int item;

		ts_bRGBA IconColor;

		for( i = 0; i < n; i++ )
		{
			item = pListBox->GetItemId(i + pListBox->GetTopItem());

			if( item == g_GlobalProfile.GetSelectedCharacter( CPlayerProfile::COP ) )
			{
				IconColor.R = 100;
				IconColor.G = 100;
				IconColor.B = 255;
				IconColor.A = 255;
			}
			else
			if( item == g_GlobalProfile.GetSelectedCharacter( CPlayerProfile::GANG ) )
			{
				IconColor.R = 255;
				IconColor.G = 100;
				IconColor.B = 100;
				IconColor.A = 255;
			}
			else
			{
				IconColor.R = 255;
				IconColor.G = 255;
				IconColor.B = 255;
				IconColor.A = 0;
			}

			pIcon = GetIcon(i);

			if( pIcon )
				pIcon->SetColor( IconColor.R, IconColor.G, IconColor.B, IconColor.A );
		}
	}

	return;
}

//=============================================================================
// ScreenCharSelect3::GetListBox
//=============================================================================

GuiListBox* ScreenCharSelect3::GetListBox( void )
{
	return (GuiListBox*) GetInterface( ID_CHARSELECT_LISTBOX, GuiListBox::ClassnameStatic() );
}

//=============================================================================
// ScreenCharSelect3::OnInitialize
//=============================================================================

void ScreenCharSelect3::OnInitialize( void )
{
	ScreenCustomBase::OnInitialize();

	SetHelpRect( ID_CHARSELECT_HELPRECT );

	GuiListBox* pListBox = CreateListBox( ID_CHARSELECT_LISTBOX );

	if( pListBox )
	{
		pListBox->InitControl( this );

		pListBox->InitItem( GetTextSprite( ID_CHARSELECT_NAME1 ) );
		pListBox->InitItem( GetTextSprite( ID_CHARSELECT_NAME2 ) );
		pListBox->InitItem( GetTextSprite( ID_CHARSELECT_NAME3 ) );
		pListBox->InitItem( GetTextSprite( ID_CHARSELECT_NAME4 ) );
		pListBox->InitItem( GetTextSprite( ID_CHARSELECT_NAME5 ) );
		pListBox->InitItem( GetTextSprite( ID_CHARSELECT_NAME6 ) );
		pListBox->InitItem( GetTextSprite( ID_CHARSELECT_NAME7 ) );
		pListBox->InitItem( GetTextSprite( ID_CHARSELECT_NAME8 ) );
		pListBox->InitCursor( GetSprite( ID_CHARSELECT_CURSOR ) );
	}

#ifdef DIRECTX_PC
	GetSprite(ID_CHARSELECT_HELPRECT)->HideAll();
#endif
	return;
}

//=============================================================================
// ScreenCharSelect3::OnScreenIntro
//=============================================================================

void ScreenCharSelect3::OnScreenIntro( void )
{
	ScreenCustomBase::OnScreenIntro();

	CustomPlayer::UpdateCamera( "" );

	int type  = PlayerDB::CharacterType( CustomPlayer::GetVisibleProfile() );
	int index = -1;
	int last  = -1;

	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		pListBox->ClearItems();

		while ( ( index = PlayerDB::ChangeCharacter( index, 1, type ) ) > last )
		{
			last = index;
			if ( !PlayerDB::CharacterIsCustom( index ) )
			{
				pListBox->AddItem( PlayerDB::CharacterDisplayName( index ), index, 0 );
			}
		}
	
		pListBox->SelectIndex(0);

		CustomPlayer::Show( pListBox->CurrentSelection() );
	}

	Printf( ID_CHARSELECT_PROFILENAME, g_translator.Translate( "Current Profile: %s" ), g_playerProfile.GetAccountName() );
}

//=============================================================================
// ScreenCharSelect3::OnCursorUp
//=============================================================================

void ScreenCharSelect3::OnCursorUp( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int OldSel = pListBox->CurrentSelection();

		pListBox->OnCursorUp();

		int NewSel = pListBox->CurrentSelection();

		if( OldSel != NewSel )
		{
			PlaySoundEvent( Screen::SOUNDEVENT_PREVBUTTON );

			CustomPlayer::Show( NewSel );

			int ProfileType = ~0;

			if( PlayerDB::CharacterType( NewSel ) == PLAYERDB_COP )
				ProfileType = CPlayerProfile::COP;
			else
			if( PlayerDB::CharacterType( NewSel ) == PLAYERDB_GANG )
				ProfileType = CPlayerProfile::GANG;
		}
	}

	return;
}

//=============================================================================
// ScreenCharSelect3::OnCursorDown
//=============================================================================

void ScreenCharSelect3::OnCursorDown( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int OldSel = pListBox->CurrentSelection();

		pListBox->OnCursorDown();

		int NewSel = pListBox->CurrentSelection();

		if( OldSel != NewSel )
		{
#ifndef DIRECTX_PC
			PlaySoundEvent( Screen::SOUNDEVENT_PREVBUTTON );
#endif
			CustomPlayer::Show( NewSel );

			int ProfileType = ~0;

			if( PlayerDB::CharacterType( NewSel ) == PLAYERDB_COP )
				ProfileType = CPlayerProfile::COP;
			else
			if( PlayerDB::CharacterType( NewSel ) == PLAYERDB_GANG )
				ProfileType = CPlayerProfile::GANG;
		}
	}

	return;
}

//=============================================================================
// ScreenCharSelect3::OnButtonOK
//=============================================================================

void ScreenCharSelect3::OnButtonOK( void )
{
	ScreenPopupDialog* pDialog = GetPopupDialog();

	if( pDialog )
	{
		pDialog->Clear();
		pDialog->SetTitle( "!!! Warning !!!" );
		pDialog->SetMessage( "Selecting a prebuilt character will discard previous changes.  Would you like to continue?" );
		pDialog->AddMenuItem( "OK", COMMAND_COPY_OK, 0 );
		pDialog->AddMenuItem( "CANCEL", COMMAND_COPY_CANCEL, 0 );
		pDialog->SelectMenuItem( COMMAND_COPY_CANCEL );

		ShowPopupDialog();
	}

	ScreenCustomBase::OnButtonOK();
}

//=============================================================================
// ScreenCharSelect3::OnButtonCancel
//=============================================================================

void ScreenCharSelect3::OnButtonCancel( void )
{
	ScreenCustomBase::OnButtonCancel();

	CustomPlayer::Show( CustomPlayer::GetEditProfile() );
}

//=============================================================================
// ScreenCharSelect3::OnDialogCommand
//=============================================================================

void ScreenCharSelect3::OnDialogCommand( int Command )
{
	switch( Command )
	{
	case COMMAND_COPY_OK:
		{
			CustomPlayer::SetEditProfileString( PlayerDB::CharacterInitString( CustomPlayer::GetVisibleProfile() ) );
			EndScreen(0);
			EndScreen(0);
			break;
		}

	case COMMAND_COPY_CANCEL:
		EndScreen(0);
		break;
	}

	return;
}

//=============================================================================
// ScreenCharSelect3::OnPopupShow
//=============================================================================

void ScreenCharSelect3::OnPopupShow( void )
{
	ScreenBaseClass::OnPopupShow();
	OnScreenExit();
}

//=============================================================================
// ScreenCharSelect3::OnPopupHide
//=============================================================================

void ScreenCharSelect3::OnPopupHide( void )
{
	ScreenBaseClass::OnPopupHide();
//	OnScreenIntro();
	ScreenCustomBase::OnScreenIntro();
}

void ScreenCustomMenu::SetDefaultCharacter( void )
{
	int profile = CustomPlayer::GetEditProfile();
	int type = PlayerDB::CharacterType( profile );
	int variant = CPlayerProfile::COP;

	ASSERT( ( type == PLAYERDB_COP ) || ( type == PLAYERDB_GANG ) );

	switch( type )
	{
	case PLAYERDB_COP:
		variant = CPlayerProfile::COP;
		break;
	case PLAYERDB_GANG:
		variant = CPlayerProfile::GANG;
		break;
	}

	g_GlobalProfile.SelectCharacter( variant, profile );
}

//=============================================================================
// ScreenCustomMenu::OnInitialize
//=============================================================================

void ScreenCustomMenu::OnInitialize( void )
{
	ScreenCustomBase::OnInitialize();

	UseHighlighting( true );

	SetHelpRect( ID_CUSTOMMENU_HELPRECT );
	SetInitialButton( ID_CUSTOMMENU_EDITCHARACTER );
	SetNextButton( ID_CUSTOMMENU_EDITNAME );
	SetNextButton( ID_CUSTOMMENU_EDITLOADOUT );
	SetCursor( ID_CUSTOMMENU_CURSOR );

	SetNextScreen( ID_CUSTOMMENU_EDITNAME, "SCREEN_ID_CUSTOMNAME" );
	SetNextScreen( ID_CUSTOMMENU_EDITCHARACTER, "SCREEN_ID_CUSTOMPLAYER" );
	SetNextScreen(	ID_CUSTOMMENU_EDITLOADOUT, "SCREEN_ID_CUSTOMWEAPONS" );
}

//=============================================================================
// ScreenCustomMenu::OnScreenIntro
//=============================================================================

void ScreenCustomMenu::OnScreenIntro( void )
{
	ScreenCustomBase::OnScreenIntro();

	Printf( ID_CUSTOMMENU_PROFILENAME, g_translator.Translate( "Current Profile: %s" ), g_playerProfile.GetAccountName() );
	SetText( ID_CUSTOMMENU_NAMETAG, PlayerDB::CharacterDisplayName( CustomPlayer::GetVisibleProfile() ) );

	CustomPlayer::UpdateCamera( "" );

	SetDefaultCharacter();
}

//=============================================================================
// ScreenCustomMenu::OnScreenExit
//=============================================================================

void ScreenCustomMenu::OnScreenExit( void )
{
	ScreenCustomBase::OnScreenExit();
	PlayerDB::ComposerShutdown();
}

//=============================================================================
// ScreenCustomName::OnScreenIntro
//=============================================================================

void ScreenCustomName::OnScreenIntro( void )
{
	ScreenCustomBase::OnScreenIntro();

	ScreenKeyboard* pKeyboard = GetKeyboard();

	if( pKeyboard )
	{
		pKeyboard->Clear();
		pKeyboard->SetMaxString(27);
		pKeyboard->SetString( PlayerDB::CharacterDisplayName( CustomPlayer::GetVisibleProfile() ) );
	}

	ShowKeyboard();
}

//=============================================================================
// ScreenCustomName::OnScreenRefresh
//=============================================================================

void ScreenCustomName::OnScreenRefresh( void )
{
	ScreenCustomBase::OnScreenRefresh();

	SetText( ID_CUSTOMNAME_BIGNAME, GetKeyboardString() );
}

//=============================================================================
// ScreenCustomName::OnKeyboardInput
//=============================================================================

void ScreenCustomName::OnKeyboardInput( const char* pString )
{
	PlayerDB::SetCharacterName( CustomPlayer::GetEditProfile(), pString );
	GotoPrevScreen();
}

//=============================================================================
// ScreenCustomName::OnKeyboardCancel
//=============================================================================

void ScreenCustomName::OnKeyboardCancel( void )
{
	GotoPrevScreen();
}

//=============================================================================
// ScreenCustomEdit::ScreenCustomEdit
//=============================================================================

ScreenCustomEdit::ScreenCustomEdit( void )
{
}

//=============================================================================
// ScreenCustomEdit::SetEditMode
//=============================================================================

void ScreenCustomEdit::SetEditMode(	EditMode Mode )
{
	switch ( Mode )
	{
	case EditMode_Head:	PlayerDB::ComposerSetMenu( "Head" );	CustomPlayer::UpdateCamera( "Head" ); break;
	case EditMode_Body:	PlayerDB::ComposerSetMenu( "Torso" );	CustomPlayer::UpdateCamera( "Torso" );	break;
	case EditMode_Arms:	PlayerDB::ComposerSetMenu( "Arms" );	CustomPlayer::UpdateCamera( "Arms" );	break;
	case EditMode_Hands:	PlayerDB::ComposerSetMenu( "Hands" );	CustomPlayer::UpdateCamera( "Hands" );	break;
	case EditMode_Legs:	PlayerDB::ComposerSetMenu( "Legs" );	CustomPlayer::UpdateCamera( "Legs" );	break;
	case EditMode_Feet:	PlayerDB::ComposerSetMenu( "Feet" );	CustomPlayer::UpdateCamera( "Feet" );	break;
	}

	InitListboxSelections();

	return;
}

//=============================================================================
// ScreenCustomEdit::GetListBox
//=============================================================================

GuiListBox* ScreenCustomEdit::GetListBox( void )
{
	return (GuiListBox*) GetInterface( ID_CUSTOMEDIT_LISTBOX, GuiListBox::ClassnameStatic() );
}

//=============================================================================
// ScreenCustomEdit::ShowIcon
//=============================================================================

void ScreenCustomEdit::ShowIcon( int Item, bool Show )
{
	GuiListBox* pListBox = GetListBox();
	Text* pDisplayItem;
	Sprite* pIcon;

	if( pListBox )
	{
		pDisplayItem = pListBox->GetDisplayItem( Item );
		pIcon = GetSprite( ID_CUSTOMEDIT_ICON, pDisplayItem );

		if( pIcon )
		{
			if( Show )
				pIcon->m_state.familycolor.a = 255;
			else
				pIcon->m_state.familycolor.a = 0;
		}
	}

	return;
}

//=============================================================================
// ScreenCustomEdit::InitListboxSelections
//=============================================================================

void ScreenCustomEdit::InitListboxSelections()
{
	GuiListBox* pListBox = GetListBox();
	if ( pListBox )
	{
		int i;
		pListBox->ClearItems();
		int selected = PlayerDB::ComposerSelected();
		int numMenus = PlayerDB::MenuNumChildren( PlayerDB::ComposerMenu() );
		int numItems = PlayerDB::MenuNumItems( PlayerDB::ComposerMenu() );
		int numUnlockableItems = PlayerDB::MenuNumUnlockableItems( PlayerDB::ComposerMenu() );
		if ( numUnlockableItems > 0 )
			Printf( ID_CUSTOMEDIT_UNLOCKSTATUS, "%d items out of %d unlocked!", numItems, numUnlockableItems );
		else
			SetText( ID_CUSTOMEDIT_UNLOCKSTATUS, 0 );
		ASSERT( numMenus == 0 || numItems == 0 );
		for ( i = 0 ; i < numMenus ; i++ )
		{
			int subMenu = PlayerDB::MenuChild( PlayerDB::ComposerMenu(), i );
			pListBox->AddItem( PlayerDB::MenuName( subMenu ), subMenu, 0 );
		}

		for ( i = 0 ; i < numItems ; i++ )
		{
			int item = PlayerDB::MenuItem( PlayerDB::ComposerMenu(), i );
			pListBox->AddItem( PlayerDB::ItemName( item ), item, 0 );
		}

		if ( selected >= 0 )
		{
			pListBox->SelectItem( selected );
		}
	}
}

//=============================================================================
// ScreenCustomEdit::OnInitialize
//=============================================================================

void ScreenCustomEdit::OnInitialize( void )
{
	ScreenCustomBase::OnInitialize();

	SetHelpRect( ID_CUSTOMEDIT_HELPRECT );

	GuiListBox* pListBox = (GuiListBox*)
		AddInterface( ID_CUSTOMEDIT_LISTBOX, GuiListBox::ClassnameStatic() );

	if( pListBox )
	{
		pListBox->InitControl( this );

		pListBox->InitItem( GetTextSprite( ID_CUSTOMEDIT_ITEM1 ) );
		pListBox->InitItem( GetTextSprite( ID_CUSTOMEDIT_ITEM2 ) );
		pListBox->InitItem( GetTextSprite( ID_CUSTOMEDIT_ITEM3 ) );
		pListBox->InitItem( GetTextSprite( ID_CUSTOMEDIT_ITEM4 ) );
		pListBox->InitItem( GetTextSprite( ID_CUSTOMEDIT_ITEM5 ) );
		pListBox->InitItem( GetTextSprite( ID_CUSTOMEDIT_ITEM6 ) );
		pListBox->InitItem( GetTextSprite( ID_CUSTOMEDIT_ITEM7 ) );
		pListBox->InitItem( GetTextSprite( ID_CUSTOMEDIT_ITEM8 ) );
		pListBox->InitItem( GetTextSprite( ID_CUSTOMEDIT_ITEM9 ) );
		pListBox->InitItem( GetTextSprite( ID_CUSTOMEDIT_ITEM10) );
		pListBox->InitItem( GetTextSprite( ID_CUSTOMEDIT_ITEM11) );
		pListBox->InitItem( GetTextSprite( ID_CUSTOMEDIT_ITEM12) );
		pListBox->InitItem( GetTextSprite( ID_CUSTOMEDIT_ITEM13) );
		pListBox->InitItem( GetTextSprite( ID_CUSTOMEDIT_ITEM14) );
		pListBox->InitItem( GetTextSprite( ID_CUSTOMEDIT_ITEM15) );
		pListBox->InitItem( GetTextSprite( ID_CUSTOMEDIT_ITEM16) );
		pListBox->InitCursor( GetSprite( ID_CUSTOMEDIT_CURSOR ) );
		pListBox->SetSelectedColor( 120, 130, 255, 255 );

		int n = pListBox->GetItemDisplayCount();
		int i;

		for( i = 0; i < n; i++ )
			ShowIcon( i, false );		
	}

	return;
}

//=============================================================================
// ScreenCustomEdit::OnScreenIntro
//=============================================================================

void ScreenCustomEdit::OnScreenIntro( void )
{
	ScreenCustomBase::OnScreenIntro();

	SetText( ID_CUSTOMEDIT_NAME, PlayerDB::CharacterDisplayName( CustomPlayer::GetEditProfile() ) );

#ifdef DIRECTX_PC
	if (m_pScreen)
	{
		BackButton.Show(true);
	}

#endif
}

//=============================================================================
// ScreenCustomEdit::OnScreenUpdate
//=============================================================================

void ScreenCustomEdit::OnScreenUpdate( void )
{
	ScreenCustomBase::OnScreenUpdate();

	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int c = pListBox->GetItemPos( PlayerDB::ComposerSelected() );
		int n = pListBox->GetItemDisplayCount();
		int i;

		c = c - pListBox->GetTopItem();

		for( i = 0; i < n; i++ )
		{
			if( i == c )
				ShowIcon( i, true );
			else
				ShowIcon( i, false );
		}
	}
	
	return;
}

//=============================================================================
// ScreenCustomEdit::OnCursorUp
//
//		This is called by the framework when the user presses up on the d-pad.
//=============================================================================

void ScreenCustomEdit::OnCursorUp( void )
{
	GuiListBox* pListBox = GetListBox();

	if( !pListBox )
		ScreenCustomBase::OnCursorUp();
	else
	{
		int OldItem = pListBox->CurrentItem();

		pListBox->OnCursorUp();

		int NewItem = pListBox->CurrentItem();

		if( NewItem != OldItem )
			PlaySoundEvent( Screen::SOUNDEVENT_PREVBUTTON );
	}
	
	return;
}

//=============================================================================
// ScreenCustomEdit::OnCursorDown
//
//		This is called by the framework when the user presses down on the d-pad.
//=============================================================================

void ScreenCustomEdit::OnCursorDown( void )
{
	GuiListBox* pListBox = GetListBox();

	if( !pListBox )
		ScreenCustomBase::OnCursorDown();
	else
	{
		int OldItem = pListBox->CurrentItem();

		pListBox->OnCursorDown();

		int NewItem = pListBox->CurrentItem();
#ifndef DIRECTX_PC
		if( NewItem != OldItem )
			PlaySoundEvent( Screen::SOUNDEVENT_NEXTBUTTON );
#endif
	}

	return;
}

//=============================================================================
// ScreenCustomEdit::OnCursorLeft
//
//		This is called by the framework when the user presses left on the d-pad.
//=============================================================================

void ScreenCustomEdit::OnCursorLeft( void )
{
}

//=============================================================================
// ScreenCustomEdit::OnCursorRight
//
//		This is called by the framework when the user presses right on the d-pad.
//=============================================================================

void ScreenCustomEdit::OnCursorRight( void )
{
}

//=============================================================================
// ScreenCustomEdit::OnButtonOK
//=============================================================================

void ScreenCustomEdit::OnButtonOK( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int selection = pListBox->CurrentSelection();

		// set the item or the menu
		PlayerDB::ComposerSelect( selection );
		if ( PlayerDB::MenuNumChildren( PlayerDB::ComposerMenu() ) )
		{
			PlayerDB::ComposerSetMenu( selection );
			InitListboxSelections();
		}
		else
		{
			CustomPlayer::SetEditProfileString( PlayerDB::ComposerGetInitString() );
		}
	}

	return;
}

//=============================================================================
// ScreenCustomEdit::OnButtonCancel
//=============================================================================

void ScreenCustomEdit::OnButtonCancel()
{
	int parent = PlayerDB::MenuParent( PlayerDB::ComposerMenu() );
	if ( parent > 0 )
	{
		PlayerDB::ComposerSetMenu( parent );
		InitListboxSelections();
	}
	else
	{
		ScreenCustomBase::OnButtonCancel();
	}
}

//=============================================================================
// ScreenCustomEdit::OnButtonPressed
//=============================================================================

void ScreenCustomEdit::OnButtonPressed( int Button )
{
	switch( Button )
	{
	case FluffControlMapper::SQUARE:
//		PlayerDB::ComposerSetColorshift(1,2,3);
//		PlayerDB::ComposerSetColorshift(180,100,100);
		CustomPlayer::SetEditProfileString( PlayerDB::ComposerGetInitString() );
		break;

	case FluffControlMapper::CIRCLE:
		// TODO: do something here!
		break;

	default:
		ScreenBaseClass::OnButtonPressed( Button );
		break;
	}

	return;
}

//=============================================================================
// ScreenCustomEdit::OnGuiMessage
//=============================================================================

void ScreenCustomEdit::OnGuiMessage( GuiControl* pControl, int message, void* Param )
{
	GuiListBox* pListBox = GetListBox();

	if( pControl == pListBox )
	{
		switch( message )
		{
		case GuiListBox::Message_UnselectItem:
			{
//				int OldItem = (int) Param;
			}
			break;
		case GuiListBox::Message_SelectItem:
			{
//				int NewItem = (int) Param;
			}
			break;
		}
		/*
			{
				GuiListBox::Param_SelChanged* pParam = (GuiListBox::Param_SelChanged *) Param;

				if( pParam )
				{
					int OldItem = pParam->OldItem;
					int NewItem = pParam->NewItem;

					Text* pOldItem = pListBox->GetDisplayItem( OldItem - pListBox->GetTopItem() );
					Text* pNewItem = pListBox->GetDisplayItem( NewItem - pListBox->GetTopItem() );
					Sprite* pOldBar = GetSprite( ID_CUSTOMEDIT_BAR, pOldItem );
					Sprite* pNewBar = GetSprite( ID_CUSTOMEDIT_BAR, pNewItem );

					int red;
					int green;
					int blue;
					int alpha;

					if( pOldBar != pNewBar )
					{
						if( pNewBar )
							pNewBar->GetBaseColor( red, green, blue, alpha );
						else
							if( pOldBar )
								pOldBar->GetBaseColor( red, green, blue, alpha );

						if( pOldBar )
							pOldBar->SetBaseColor( red, green, blue, alpha );

						if( pNewBar )
							pNewBar->SetBaseColor( 100, 100, 100, 100 );
					}
				}
			}
			break;
		}
		*/
	}

	return;
}

//=============================================================================
// ScreenCustomPlayer::ScreenCustomPlayer - The default constructor
//=============================================================================

ScreenCustomPlayer::ScreenCustomPlayer( void )
{
	m_ShowCustomScreen = false;
}

//=============================================================================
// ScreenCustomPlayer::~ScreenCustomPlayer - The default destructor
//=============================================================================

ScreenCustomPlayer::~ScreenCustomPlayer( void )
{
}

//=============================================================================
// ScreenCustomPlayer::GotoCustomScreen
//=============================================================================

void ScreenCustomPlayer::GotoCustomScreen( const char* pScreenName, ScreenCustomEdit::EditMode Mode )
{
	m_ShowCustomScreen = true;

	ScreenCustomEdit* pCustomEdit = (ScreenCustomEdit*) GetScreenHandler( "SCREEN_ID_CUSTOMEDIT" );

	if( pCustomEdit )
	{
		pCustomEdit->SetEditMode( Mode );
	}

	GotoScreen( pScreenName );
}

//=============================================================================
// ScreenCustomPlayer::OnInitialize
//=============================================================================

void ScreenCustomPlayer::OnInitialize( void )
{
	ScreenCustomBase::OnInitialize();

	UseHighlighting( true );

	SetHelpRect( ID_CUSTOMPLAYER_HELPRECT );
	SetInitialButton( ID_CUSTOMPLAYER_HEAD );
	SetNextButton( ID_CUSTOMPLAYER_BODY );
	SetNextButton( ID_CUSTOMPLAYER_ARMS );
	SetNextButton( ID_CUSTOMPLAYER_HANDS );
	SetNextButton( ID_CUSTOMPLAYER_LEGS );
	SetNextButton( ID_CUSTOMPLAYER_FEET );
	SetNextButton( ID_CUSTOMPLAYER_PREBUILT );
	SetNextButton( ID_CUSTOMPLAYER_CLEAR );
	SetCursor( ID_CUSTOMPLAYER_CURSOR );

	return;
}

//=============================================================================
// ScreenCustomPlayer::OnScreenIntro
//=============================================================================

void ScreenCustomPlayer::OnScreenIntro( void )
{
	ScreenCustomBase::OnScreenIntro();
	CustomPlayer::UpdateCamera( "" );

	if( m_ShowCustomScreen )
		m_ShowCustomScreen = false;
	else
	{
		PlayerDB::ComposerInit( PlayerDB::CharacterType( CustomPlayer::GetEditProfile() ),
			PlayerDB::CharacterInitString( CustomPlayer::GetEditProfile() ) );

		CustomPlayer::SetEditProfileString( PlayerDB::ComposerGetInitString() );
	}

#ifdef DIRECTX_PC
	BackButton.Show(true);
#endif

	Printf( ID_CUSTOMPLAYER_PROFILENAME, g_translator.Translate( "Current Profile: %s" ), g_playerProfile.GetAccountName() );
}


//=============================================================================
// ScreenCustomPlayer::OnScreenExit
//=============================================================================

void ScreenCustomPlayer::OnScreenExit( void )
{
	ScreenCustomBase::OnScreenExit();

	if( !m_ShowCustomScreen )
		PlayerDB::ComposerShutdown();

	return;
}

//=============================================================================
// ScreenCustomPlayer::OnScreenRefresh
//=============================================================================

void ScreenCustomPlayer::OnScreenRefresh( void )
{
	SetText( ID_CUSTOMPLAYER_NAMETAG, PlayerDB::CharacterDisplayName( CustomPlayer::GetVisibleProfile() ) );
}

//=============================================================================
// ScreenCustomPlayer::OnCommand
//=============================================================================

void ScreenCustomPlayer::OnCommand( int Command )
{
	switch( Command ) 
	{
	case ID_CUSTOMPLAYER_HEAD:
		OnCommandHead();
		break;
	case ID_CUSTOMPLAYER_BODY:
		OnCommandBody();
		break;
	case ID_CUSTOMPLAYER_ARMS:
		OnCommandArms();
		break;
	case ID_CUSTOMPLAYER_HANDS:
		OnCommandHands();
		break;
	case ID_CUSTOMPLAYER_LEGS:
		OnCommandLegs();
		break;
	case ID_CUSTOMPLAYER_FEET:
		OnCommandFeet();
		break;
	case ID_CUSTOMPLAYER_PREBUILT:
		OnCommandPrebuilt();
		break;
	case ID_CUSTOMPLAYER_CLEAR:
		OnCommandClear();
		break;
	}

	return;
}

//=============================================================================
// ScreenCustomPlayer::OnCommandHead
//=============================================================================

void ScreenCustomPlayer::OnCommandHead( void )
{
	GotoCustomScreen( "SCREEN_ID_CUSTOMEDIT", ScreenCustomEdit::EditMode_Head );
}

//=============================================================================
// ScreenCustomPlayer::OnCommandBody
//=============================================================================

void ScreenCustomPlayer::OnCommandBody( void )
{
	GotoCustomScreen( "SCREEN_ID_CUSTOMEDIT", ScreenCustomEdit::EditMode_Body );
}

//=============================================================================
// ScreenCustomPlayer::OnCommandArms
//=============================================================================

void ScreenCustomPlayer::OnCommandArms( void )
{
	GotoCustomScreen( "SCREEN_ID_CUSTOMEDIT", ScreenCustomEdit::EditMode_Arms );
}

//=============================================================================
// ScreenCustomPlayer::OnCommandHands
//=============================================================================

void ScreenCustomPlayer::OnCommandHands( void )
{
	GotoCustomScreen( "SCREEN_ID_CUSTOMEDIT", ScreenCustomEdit::EditMode_Hands );
}

//=============================================================================
// ScreenCustomPlayer::OnCommandLegs
//=============================================================================

void ScreenCustomPlayer::OnCommandLegs( void )
{
	GotoCustomScreen( "SCREEN_ID_CUSTOMEDIT", ScreenCustomEdit::EditMode_Legs );
}

//=============================================================================
// ScreenCustomPlayer::OnCommandFeet
//=============================================================================

void ScreenCustomPlayer::OnCommandFeet( void )
{

	GotoCustomScreen( "SCREEN_ID_CUSTOMEDIT", ScreenCustomEdit::EditMode_Feet );
}

//=============================================================================
// ScreenCustomPlayer::OnCommandPrebuilt
//=============================================================================

void ScreenCustomPlayer::OnCommandPrebuilt( void )
{
	GotoScreen( "SCREEN_ID_CHARSELECT3" );
}


//=============================================================================
// ScreenCustomPlayer::OnCommandClear
//=============================================================================

void ScreenCustomPlayer::OnCommandClear( void )
{
	ScreenPopupDialog* pDialog = GetPopupDialog();

	if( pDialog )
	{
		pDialog->Clear();
		pDialog->SetTitle( "Attention" );
		pDialog->SetMessage( "This action will erase all changes and revert to the default character.  Would you like to continue?" );
		pDialog->AddMenuItem( "OK", COMMAND_CLEAR_OK, 0 );
		pDialog->AddMenuItem( "Cancel", COMMAND_CLEAR_CANCEL, 0 );
		pDialog->SelectMenuItem( COMMAND_CLEAR_CANCEL );

		ShowPopupDialog();
	}

	return;
}

//=============================================================================
// ScreenCustomPlayer::OnDialogCommand
//=============================================================================

void ScreenCustomPlayer::OnDialogCommand( int Command )
{
	switch( Command )
	{
	case COMMAND_CLEAR_OK:
		PlayerDB::ComposerInit( PlayerDB::CharacterType( CustomPlayer::GetVisibleProfile() ), "" );
		CustomPlayer::SetEditProfileString( PlayerDB::ComposerGetInitString() );
		PlayerDB::ComposerShutdown();
		EndScreen(0);
		break;

	case COMMAND_CLEAR_CANCEL:
		EndScreen(0);
		break;
	}

	return;
}

//=============================================================================
// ScreenCustomPlayer::OnPopupShow
//=============================================================================

void ScreenCustomPlayer::OnPopupShow( void )
{
	ScreenBaseClass::OnPopupShow();
	OnScreenExit();
}

//=============================================================================
// ScreenCustomPlayer::OnPopupHide
//=============================================================================

void ScreenCustomPlayer::OnPopupHide( void )
{
	ScreenBaseClass::OnPopupHide();
	OnScreenIntro();
}

//=============================================================================
// ScreenCustomWeapons::UpdateWeaponInHand
//=============================================================================

void ScreenCustomWeapons::UpdateWeaponInHand( void )
{
	// todo: the weapon being held by the on-screen character needs to be updated.

	if( m_Active )
	{
		ArmoryView::PlayerType playerType = GetPlayerType();

		WeaponLoadout loadout;
		g_armoryView.CreateFromSelection( playerType, loadout);

		const char* initString = loadout.GetParameterSet().GetInitializerString();
		if (initString)
		{
			CustomPlayer::EquipLoadout( initString );
			delete [] initString;
		}

		CustomPlayer::EquipWeapon( GetCurrentButtonIndex() );
	}

	return;
}

void ScreenCustomWeapons::ChangeWeaponInHand( int n )
{
	int i = GetCurrentButtonIndex();

	TTLWeapon::Slot slot = TTLWeapon::Slot(i);
	ArmoryView::PlayerType playerType = GetPlayerType();
	const Armory::Description *current = g_armoryView.GetSelection( playerType, slot );

	// Make some noise...
	if( n < 0 )
	{
		current = g_armoryView.FindPrevious( playerType, slot, current, true );
		if (current) 
			PlaySoundEvent( Screen::SOUNDEVENT_PREVBUTTON );
	}
	else
	{
		current = g_armoryView.FindNext( playerType, slot, current, true );
		if (current)
			PlaySoundEvent( Screen::SOUNDEVENT_NEXTBUTTON );
	}

	// Set it
	g_armoryView.SetSelection( playerType, slot, current );

	// Update
	UpdateWeaponInHand();
}

void ScreenCustomWeapons::CommitWeaponLoadout( void )
{
	ArmoryView::PlayerType playerType = GetPlayerType();
	int variant = GetVariant();

	WeaponLoadout loadout;
	g_armoryView.CreateFromSelection( playerType, loadout);

	const char* initString = loadout.GetParameterSet().GetInitializerString();
	if (initString)
	{
		g_GlobalProfile.SetCustomLoadout(GetCharacterIndex(), initString );
		g_playerProfile.SetWeaponLoadout(CPlayerProfile::EVariant(variant), initString);

		delete [] initString;
	}

	return;
}

void ScreenCustomWeapons::RevertWeaponLoadout( void )
{
	const char* weaponLoadout = g_GlobalProfile.GetCustomLoadout(GetCharacterIndex());
	ArmoryView::PlayerType playerType = GetPlayerType();

	if (!weaponLoadout || strlen(weaponLoadout) == 0)
		g_armoryView.DefaultSelection(playerType);
	else
	{
		WeaponLoadout loadout(weaponLoadout);
		g_armoryView.SetSelection(playerType, loadout, true);
	}

	return;
}

int ScreenCustomWeapons::GetCharacterIndex( void )
{
	return PlayerDB::CharacterId(CustomPlayer::GetEditProfile()) & ~PLAYERDB_CUSTOM;
}

int ScreenCustomWeapons::GetVariant( void )
{
	int type = PlayerDB::CharacterType( CustomPlayer::GetEditProfile() );

	if( type == PLAYERDB_COP )
		return CPlayerProfile::COP;

	if( type == PLAYERDB_GANG )
		return CPlayerProfile::GANG;

	ASSERT(0);
	return CPlayerProfile::INVALID;
}

ArmoryView::PlayerType ScreenCustomWeapons::GetPlayerType( void )
{
	int type = PlayerDB::CharacterType( CustomPlayer::GetEditProfile() );

	if( type == PLAYERDB_COP )
		return ArmoryView::HumanCop;

	if( type == PLAYERDB_GANG )
		return ArmoryView::HumanGangster;

	ASSERT(0);
	return ArmoryView::HumanCop;
}

//=============================================================================
// ScreenCustomWeapons::OnInitialize
//=============================================================================

void ScreenCustomWeapons::OnInitialize( void )
{
	ScreenCustomBase::OnInitialize();

	UseHighlighting( true );

	SetHelpRect( ID_CUSTOMWEAPONS_HELPRECT );
	SetInitialButton( ID_CUSTOMWEAPONS_WEAPON1 );
	SetNextButton( ID_CUSTOMWEAPONS_WEAPON2 );
	SetNextButton( ID_CUSTOMWEAPONS_WEAPON3 );
	SetNextButton( ID_CUSTOMWEAPONS_WEAPON4 );
	SetCursor( ID_CUSTOMWEAPONS_CURSOR );
}

//=============================================================================
// ScreenCustomWeapons::OnScreenIntro
//=============================================================================

void ScreenCustomWeapons::OnScreenIntro( void )
{
	ScreenCustomBase::OnScreenIntro();

	Printf( ID_CUSTOMWEAPONS_PROFILENAME, g_translator.Translate( "Current Profile: %s" ), g_playerProfile.GetAccountName() );
	SetText( ID_CUSTOMWEAPONS_NAMETAG, PlayerDB::CharacterDisplayName( CustomPlayer::GetVisibleProfile() ) );

	// get in close :)
	CustomPlayer::UpdateCamera( "-Torso" );

	RevertWeaponLoadout();
	UpdateWeaponInHand();
}

//=============================================================================
// ScreenCustomWeapons::OnScreenUpdate
//=============================================================================

void ScreenCustomWeapons::OnScreenUpdate( void )
{
	ScreenCustomBase::OnScreenUpdate();

	char WeaponTitle[64];
	const Armory::Description* pWeapon;
	TTLWeapon::Slot s;

	Sprite* pButton;
	Text* pLabel;
	Text* pParam;

	int c = TTLWeapon::MaxSlots();
	int i;

	for( i = 0; i < c; i++ )
	{
		s = TTLWeapon::Slot(i);

		pButton = GetSprite( GetNthButtonId(i) );
		pLabel = GetTextSprite( ID_CUSTOMWEAPONS_LABEL, pButton );
		pParam = GetTextSprite( ID_CUSTOMWEAPONS_PARAM, pButton );
		pWeapon = g_armoryView.GetSelection( GetPlayerType(), s );

		if( pLabel )
			pLabel->SetText( TTLWeapon::SlotName(s) );

		if( !pWeapon )
			strcpy( WeaponTitle, "None" );
		else
		{
			if (pWeapon->MaxAmmo() == -1 && pWeapon->AmmoPerClip() == -1)
				strcpy(WeaponTitle, pWeapon->DisplayName());
			else if (pWeapon->AmmoPerClip() == -1)
				sprintf(WeaponTitle, "%s (%d)", g_translator.Translate(pWeapon->DisplayName()), pWeapon->MaxAmmo());
			else
				sprintf(WeaponTitle, "%s (%d/%d)", g_translator.Translate(pWeapon->DisplayName()), pWeapon->AmmoPerClip(), pWeapon->MaxAmmo());
		}

		if( pParam )
			pParam->SetText( WeaponTitle );
	}

	return;
}

//=============================================================================
// ScreenCustomWeapons::OnCursorLeft
//=============================================================================

void ScreenCustomWeapons::OnCursorLeft( void )
{
	ChangeWeaponInHand(-1);
}

//=============================================================================
// ScreenCustomWeapons::OnCursorRight
//=============================================================================

void ScreenCustomWeapons::OnCursorRight( void )
{
	ChangeWeaponInHand(1);
}

//=============================================================================
// ScreenCustomWeapons::OnButtonOK
//=============================================================================

void ScreenCustomWeapons::OnButtonCancel( void )
{
	CommitWeaponLoadout();

	ScreenCustomBase::OnButtonCancel();
}

//=============================================================================
// ScreenCustomWeapons::OnButtonSelect
//=============================================================================

void ScreenCustomWeapons::OnButtonSelect( Button* pButton, Sprite* pSprite )
{
	ScreenCustomBase::OnButtonSelect( pButton, pSprite );

	Text* pWeaponLabel = GetTextSprite( ID_CUSTOMWEAPONS_LABEL, pSprite );

	if( pWeaponLabel )
	{
		TextOutput to;
		float x1, y1;
		float x2, y2;
		float ox, oy;
		float x, y;
		float w, h;

		pWeaponLabel->GetSize( w, h );
		pWeaponLabel->GetPosition( x, y );
		pWeaponLabel->GetOrigin( ox, oy );
		pWeaponLabel->SetSize( 0.0f, h );
		pWeaponLabel->ComputeTextOutput( &to );
		pWeaponLabel->SetSize( w, h );

		x1 = x - (to.Width * ox) - 12.0f;
		x2 = x1 + to.Width + 24.0f;
		y1 = y - (to.Height * oy) + to.Height * 0.5f;
		y2 = y1;


		Sprite* pCursor1 = GetSprite(	ID_CUSTOMWEAPONS_CURSOR1 );

		if( pCursor1 )
		{
			pCursor1->SetParent( pWeaponLabel->Parent() );
			pCursor1->SetPosition( x1, y1 );
		}

		Sprite* pCursor2 = GetSprite( ID_CUSTOMWEAPONS_CURSOR2 );

		if( pCursor2 )
		{
			pCursor2->SetParent( pWeaponLabel->Parent() );
			pCursor2->SetPosition( x2, y2 );
		}
	}

	UpdateWeaponInHand();
}