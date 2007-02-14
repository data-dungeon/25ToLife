#include "TTLPCH.h"
#include "main/PlayerDB.h"
#include "frontend/savegame.h"
#include "frontend/inputdriver.h"
#include "frontend/screens/screen_Weapons.h"
#include "data/screens/NetworkGames.h"
#include "GameHelper/Translator.h"

//=============================================================================
// ScreenWeapons::ScreenWeapons
//=============================================================================

ScreenWeapons::ScreenWeapons()
{
	m_MenuMode = MENUMODE_NOTINITIALIZED;
	m_Variant = CPlayerProfile::COP;
}

//=============================================================================
// Update the screen
//=============================================================================

void ScreenWeapons::Update()
{
	for( int s = 0; s < TTLWeapon::MaxSlots(); s++ )
	{
		TTLWeapon::Slot slot = TTLWeapon::Slot(s);
		int buttonId = SlotToButtonId(slot);

		Text* pWeaponLabel = GetTextSprite( GetLabelId(), GetSprite( buttonId ) );
		if (pWeaponLabel)
			pWeaponLabel->SetText( TTLWeapon::SlotName(slot) );

		pWeaponLabel = GetTextSprite( GetParamId(), GetSprite( buttonId ) );
		const Armory::Description* weapon = g_armoryView.GetSelection( GetPlayerType(), slot );

		if( !weapon )
			pWeaponLabel->SetText( g_translator.Translate( "None" ) );
		else
		{
			char title[64];
			if (weapon->MaxAmmo() == -1 && weapon->AmmoPerClip() == -1)
				strcpy(title, weapon->DisplayName());
			else if (weapon->AmmoPerClip() == -1)
				sprintf(title, "%s (%d)", g_translator.Translate(weapon->DisplayName()), weapon->MaxAmmo());
			else
				sprintf(title, "%s (%d/%d)", g_translator.Translate(weapon->DisplayName()), weapon->AmmoPerClip(), weapon->MaxAmmo());

			pWeaponLabel->SetText( title );
		}
	}

	for( int team = 0; team < 2; team++ )
	{
		GuiCycleButton* pCycleButton = GetCycleButton( TeamToButtonId(team) );

		if( pCycleButton )
		{
			int CurSel = pCycleButton->CurrentSelection();

			pCycleButton->ClearItems();
			pCycleButton->AddItem( g_translator.Translate( "Loadout" ), 1, 0 );
			pCycleButton->SelectItem( CurSel );
		}
	}
}

//=============================================================================
// ScreenWeapons::RevertWeaponLoadout
//
//		This function populates the ArmoryView with the currently selected
//		weapon loadout the specified variant based on which character is
//		currently selected for that team (cops or gangsters).
//=============================================================================

void ScreenWeapons::RevertWeaponLoadout(int variant)
{
	int ch = PlayerDB::CharacterId( g_GlobalProfile.GetSelectedCharacter(variant) ) & ~PLAYERDB_CUSTOM;
	const char* weaponLoadout = g_GlobalProfile.GetCustomLoadout(ch);
	ArmoryView::PlayerType playerType = GetPlayerType(variant);

	if (!weaponLoadout && strlen(weaponLoadout) == 0)
		g_armoryView.DefaultSelection(playerType);
	else
	{
		WeaponLoadout loadout(weaponLoadout);
		g_armoryView.SetSelection(playerType, loadout, true);
	}
}

//=============================================================================
// Save the selection
//=============================================================================

void ScreenWeapons::CommitWeaponLoadout(int variant)
{
	WeaponLoadout loadout;
	g_armoryView.CreateFromSelection(GetPlayerType(variant), loadout);
	const char* initString = loadout.GetParameterSet().GetInitializerString();
	if (initString)
	{
		int ch = PlayerDB::CharacterId( g_GlobalProfile.GetSelectedCharacter(variant) ) & ~PLAYERDB_CUSTOM;
		g_GlobalProfile.SetCustomLoadout( ch, initString );
		g_playerProfile.SetWeaponLoadout(CPlayerProfile::EVariant(variant), initString);

		delete [] initString;
	}
}

//=============================================================================
// ScreenWeapons::UseDefaultLoadout
//=============================================================================

void ScreenWeapons::UseDefaultLoadout( int variant )
{
	ArmoryView::PlayerType playerType = ArmoryView::HumanCop;

	switch( variant )
	{
	case CPlayerProfile::COP:
		playerType = ArmoryView::HumanCop;
		break;
	case CPlayerProfile::GANG:
		playerType = ArmoryView::HumanGangster;
		break;
	default:
		ASSERT(0);
		break;
	}

	if( GetCurrentButtonId() != ID_WEAPONS_FINISHED )
	{
		const char* weapons = g_armoryView.GetDefaultLoadout( playerType, 0 );

		if( xstrlen(weapons) > 0 )
		{
			WeaponLoadout loadout(weapons);
			g_armoryView.SetSelection( playerType, loadout );
		}

		Update();
	}

	return;
}

//=============================================================================
// ScreenWeapons::SlotToButtonId
//
//		Returns the corresponding button id for the specified slot.
//		Returns 0 if the slot is invalid.
//=============================================================================

int ScreenWeapons::SlotToButtonId( int Slot )
{
	switch( Slot )
	{
	case 0:
		return ID_WEAPONS_WEAPON1;
	case 1:
		return ID_WEAPONS_WEAPON2;
	case 2:
		return ID_WEAPONS_WEAPON3;
	case 3:
		return ID_WEAPONS_WEAPON4;
	}

	return 0;
}

//=============================================================================
// ScreenWeapons::ButtonIdToSlot
//
//		Returns the corresponding slot for the specified button id.
//		Returns -1 if the button id does not correspond to a slot.
//=============================================================================

int ScreenWeapons::ButtonIdToSlot( int ButtonId )
{
	int Id = SlotToButtonId(0);

	for( int n = 0; Id; n++ )
	{
		Id = SlotToButtonId(n);

		if( Id == ButtonId )
			return n;
	}

	return -1;
}

//=============================================================================
// ScreenWeapons::TeamToButtonId
//
//		Returns the corresponding button id for the specified team.
//		Returns 0 if the team is invalid.
//=============================================================================

int ScreenWeapons::TeamToButtonId( int Team )
{
	switch( Team )
	{
	case 0:
		return ID_WEAPONS_TEAM1;
	case 1:
		return ID_WEAPONS_TEAM2;
	}

	return 0;
}

//=============================================================================
// ScreenWeapons::ButtonIdToTeam
//
//		Returns the corresponding slot for the specified button id.
//		Returns -1 if the button id does not correspond to a slot.
//=============================================================================

int ScreenWeapons::ButtonIdToTeam( int ButtonId )
{
	int Id = TeamToButtonId(0);

	for( int n = 0; Id; n++ )
	{
		Id = TeamToButtonId(n);

		if( Id == ButtonId )
			return n;
	}

	return -1;
}

//=============================================================================
// ScreenWeapons::SetMenuMode
//=============================================================================

void ScreenWeapons::SetMenuMode(	MENUMODE MenuMode )
{
	if( m_MenuMode != MenuMode )
	{
		m_MenuMode = MenuMode;

		switch( MenuMode )
		{
		case MENUMODE_TEAMSELECT:
			ShowSprite( ID_WEAPONS_CURSOR, true );
			EnableButton( ID_WEAPONS_TEAM1, true );
			EnableButton( ID_WEAPONS_TEAM2, true );
			EnableButton( ID_WEAPONS_FINISHED, true );
			EnableButton( ID_WEAPONS_WEAPON1, false );
			EnableButton( ID_WEAPONS_WEAPON2, false );
			EnableButton( ID_WEAPONS_WEAPON3, false );
			EnableButton( ID_WEAPONS_WEAPON4, false );
			SelectInitialButton();
			break;

		case MENUMODE_WEAPONSELECT:
			ShowSprite( ID_WEAPONS_CURSOR, false );
			EnableButton( ID_WEAPONS_TEAM1, false );
			EnableButton( ID_WEAPONS_TEAM2, false );
			EnableButton( ID_WEAPONS_FINISHED, false );
			EnableButton( ID_WEAPONS_WEAPON1, true );
			EnableButton( ID_WEAPONS_WEAPON2, true );
			EnableButton( ID_WEAPONS_WEAPON3, true );
			EnableButton( ID_WEAPONS_WEAPON4, true );
			SelectInitialButton();
			break;
		}
	}
}

//=============================================================================
// ScreenWeapons::GetMenuMode
//=============================================================================

ScreenWeapons::MENUMODE ScreenWeapons::GetMenuMode()
{
	return m_MenuMode;
}

//=============================================================================
// ScreenWeapons::SetCycleButton
//=============================================================================

GuiCycleButton* ScreenWeapons::SetCycleButton( int ButtonId )
{
	Sprite* pParent = GetSprite( ButtonId );
	Text* pParam = GetTextSprite( GetParamId(), pParent );
	Text* pLabel = GetTextSprite( GetLabelId(), pParent );
	GuiCycleButton* pCycleButton = 0;

	if( pParam )
		pCycleButton = (GuiCycleButton*) pParam->AddInterface( GuiCycleButton::ClassnameStatic(), 0 );

	return pCycleButton;
}

//=============================================================================
// ScreenWeapons::GetCycleButton
//=============================================================================

GuiCycleButton* ScreenWeapons::GetCycleButton( int ButtonId )
{
	Sprite* pParent = GetSprite( ButtonId );
	Text* pParam = GetTextSprite( GetParamId(), pParent );
	GuiCycleButton* pCycleButton = 0;

	if( pParam )
		pCycleButton = (GuiCycleButton*) pParam->GetInterface( GuiCycleButton::ClassnameStatic() );

	return pCycleButton;
}

//=============================================================================
// ScreenWeapons::SetButtonLabel
//=============================================================================

void ScreenWeapons::SetButtonLabel(	int ButtonId, const char* pLabel )
{
	Sprite* pParent = GetSprite( ButtonId );
	Text* pText = GetTextSprite( GetParamId(), pParent );

	if( pText )
		pText->SetText( pLabel );
}

//=============================================================================
// ScreenWeapons::ShowWeapon
//=============================================================================

void ScreenWeapons::ShowWeapons( bool Show )
{
	Sprite* pWeaponRect = GetSprite( ID_WEAPONS_LOADOUTS );

	if( pWeaponRect )
	{
		if( Show )
			pWeaponRect->m_state.familycolor.a = 255;
		else
			pWeaponRect->m_state.familycolor.a = 0;
	}

	return;
}

//=============================================================================
// ScreenWeapons::OnInitialize
//=============================================================================

void ScreenWeapons::OnInitialize()
{
	ScreenBaseClass::OnInitialize();

	UseHighlighting( true );

	SetHelpRect( ID_WEAPONS_HELPRECT );
	SetInitialButton( ID_WEAPONS_TEAM1 );
	SetNextButton( ID_WEAPONS_TEAM2 );
	SetNextButton( ID_WEAPONS_FINISHED );
	SetNextButton( ID_WEAPONS_WEAPON1 );
	SetNextButton( ID_WEAPONS_WEAPON2 );
	SetNextButton( ID_WEAPONS_WEAPON3 );
	SetNextButton( ID_WEAPONS_WEAPON4 );
	SetCursor( ID_WEAPONS_CURSOR );

	SetCycleButton( ID_WEAPONS_TEAM1 );
	SetCycleButton( ID_WEAPONS_TEAM2 );

#ifdef DIRECTX_PC
	GetSprite(ID_WEAPONS_HELPRECT)->HideAll();
#endif
}


//=============================================================================
// ScreenWeapons::OnScreenIntro
//=============================================================================

void ScreenWeapons::OnScreenIntro()
{
	ScreenBaseClass::OnScreenIntro();

	RevertWeaponLoadout( CPlayerProfile::GANG );
	RevertWeaponLoadout( CPlayerProfile::COP );

	SetMenuMode( MENUMODE_TEAMSELECT );

	Printf( ID_WEAPONS_CURRENTPROFILE, g_translator.Translate( "Current Profile: %s" ), g_playerProfile.GetAccountName() );

	CTeamingModel& teamingModel = g_referee.GetTeamingModel();
	CPlayerProfile::EVariant variant = teamingModel.GetPlayerProfileVariant( g_referee.GetLocalPlayerAgentId() );

	switch( variant )
	{
	case CPlayerProfile::COP:
		SelectButton( ID_WEAPONS_TEAM1 );
		break;

	case CPlayerProfile::GANG:
		SelectButton( ID_WEAPONS_TEAM2 );
		break;

	default:
		SelectButton( ID_WEAPONS_FINISHED );
		break;
	}

	m_PreviousButton = GetCurrentButtonId();

	// Update
	Update(); 
}

//=============================================================================
// ScreenWeapons::OnCursorLeft
//=============================================================================

void ScreenWeapons::OnCursorLeft()
{
	OnButtonCycle(-1);
}

//=============================================================================
// ScreenWeapons::OnCursorRight
//=============================================================================

void ScreenWeapons::OnCursorRight()
{
	OnButtonCycle(1);
}

//=============================================================================
// ScreenWeapons::OnButtonCycle
//=============================================================================

void ScreenWeapons::OnButtonCycle( int direction )
{
	int WeaponIndex = ButtonIdToSlot( GetCurrentButtonId() );

	if( WeaponIndex >= 0 )
	{
		TTLWeapon::Slot slot = TTLWeapon::Slot( WeaponIndex );
		ArmoryView::PlayerType playerType = GetPlayerType();
		const Armory::Description *current = g_armoryView.GetSelection( playerType, slot );

		// Make some noise...
		if( direction < 0 )
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
		g_armoryView.SetSelection( GetPlayerType(), slot, current );

		// Update
		Update(); 
	}
}

//=============================================================================
// ScreenWeapons::OnButtonCancel
//=============================================================================

void ScreenWeapons::OnButtonCancel()
{
	switch( GetMenuMode() )
	{
	case MENUMODE_TEAMSELECT:
		EndScreen(0);
		break;

	case MENUMODE_WEAPONSELECT:
		SetMenuMode( MENUMODE_TEAMSELECT );
		SelectButton( m_PreviousButton );
		break;
	}

	return;
}

//=============================================================================
// ScreenWeapons::OnButtonSelect
//=============================================================================

void ScreenWeapons::OnButtonSelect( Button* pButton, Sprite* pSprite )
{
	bool HelpText3 = true;
	bool Show = true;

	if( pButton )
	{
		switch( pButton->GetButtonId() )
		{
		case ID_WEAPONS_TEAM1:
			m_Variant = CPlayerProfile::COP;
			break;

		case ID_WEAPONS_TEAM2:
			m_Variant = CPlayerProfile::GANG;
			break;

		case ID_WEAPONS_FINISHED:
			HelpText3 = false;
			break;
		}

		if( pButton->GetButtonId() == ID_WEAPONS_FINISHED )
			Show = false;
	}

	Sprite* pHelpText = GetSprite( ID_WEAPONS_HELPTEXT3 );

	if( pHelpText )
	{
		if( HelpText3 )
			pHelpText->m_state.familycolor.a = 255;
		else
			pHelpText->m_state.familycolor.a = 0;
	}

	ShowWeapons( Show );

	Update();

	ScreenBaseClass::OnButtonSelect( pButton, pSprite );
}

//=============================================================================
// ScreenWeapons::OnButtonPressed
//=============================================================================

void ScreenWeapons::OnButtonPressed( int Button )
{
	switch( Button )
	{
	case FluffControlMapper::SQUARE:
		UseDefaultLoadout( m_Variant );
		break;
	default:
		ScreenBaseClass::OnButtonPressed( Button );
		break;
	}

	return;
}


//=============================================================================
// ScreenWeapons::OnCommand
//=============================================================================

void ScreenWeapons::OnCommand( int Command )
{
	switch( Command )
	{
	case ID_WEAPONS_TEAM1:
		SetMenuMode( MENUMODE_WEAPONSELECT );
		break;
	case ID_WEAPONS_TEAM2:
		SetMenuMode( MENUMODE_WEAPONSELECT );
		break;
	case ID_WEAPONS_FINISHED:
		OnCommandDone();
		break;
	case ID_WEAPONS_WEAPON1:
	case ID_WEAPONS_WEAPON2:
	case ID_WEAPONS_WEAPON3:
	case ID_WEAPONS_WEAPON4:
		SetMenuMode( MENUMODE_TEAMSELECT );
		SelectButton( m_PreviousButton );
		break;
	}

	m_PreviousButton = Command;
}

//=============================================================================
// ScreenWeapons::OnCommandDone
//=============================================================================

void ScreenWeapons::OnCommandDone()
{
	CommitWeaponLoadout(CPlayerProfile::COP);
	CommitWeaponLoadout(CPlayerProfile::GANG);
	EndScreen(0);
}

//=============================================================================
//=============================================================================

ArmoryView::PlayerType ScreenWeapons::GetPlayerType(
int variant)
{
	ASSERT(variant == CPlayerProfile::COP || variant == CPlayerProfile::GANG);

	if (variant == CPlayerProfile::COP)
		return ArmoryView::HumanCop;
	else
		return ArmoryView::HumanGangster;
}

#ifdef DIRECTX_PC
//=============================================================================
//=============================================================================
void ScreenWeapons::OnScreenUpdate()
{
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
		// functionality for list box based menus
		GuiListBox* listBox = GetListBox();
		if (listBox)
		{
			int items = listBox->GetItemCount();
			for (int c=0; c < items; ++c)
			{
				listBox->GetCursorRect(x1,y1,x2,y2);
				if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) )
				{
					if ( g_MenuCursor.CursorButtonPressed(0) )
						SelectMenuItem();
					break;
				}
				//listBox->OnCursorDown();
				OnCursorDown();
			}
		}
		Button* button = GetDefaultButton();
		// functionality for button based menus
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
				SelectButton(button);
				if ( g_MenuCursor.CursorButtonPressed(0) )
				{
						g_MenuCursor.ForceButtonUp(0);
						OnCursorRight();
						if (GetCurrentButtonId() == ID_WEAPONS_FINISHED)
							OnButtonOK();
				}
				break;
			}
			button = button->GetNextButton();
		}
	}
}
#endif

//=============================================================================
// ScreenWeapons::SlotToButtonId
//
//		Returns the corresponding button id for the specified slot.
//		Returns 0 if the slot is invalid.
//=============================================================================

int ScreenWeaponsInGame::SlotToButtonId( int Slot )
{
	switch( Slot )
	{
	case 0:
		return ID_WEAPONS2_WEAPON1;
	case 1:
		return ID_WEAPONS2_WEAPON2;
	case 2:
		return ID_WEAPONS2_WEAPON3;
	case 3:
		return ID_WEAPONS2_WEAPON4;
	}

	return 0;
}

//=============================================================================
// ScreenWeaponsInGame::OnInitialize
//=============================================================================

void ScreenWeaponsInGame::OnInitialize()
{
	ScreenBaseClass::OnInitialize();

	UseHighlighting( true );

	SetInitialButton( ID_WEAPONS2_WEAPON1 );
	SetNextButton( ID_WEAPONS2_WEAPON2 );
	SetNextButton( ID_WEAPONS2_WEAPON3 );
	SetNextButton( ID_WEAPONS2_WEAPON4 );
	SetCursor( ID_WEAPONS2_CURSOR );
}

//=============================================================================
// ScreenWeaponsInGame::OnButtonOK
//=============================================================================

void ScreenWeaponsInGame::OnButtonOK()
{
	if (g_players.GetPlayer(0) != NULL)
	{
		WeaponInventory *inv = (WeaponInventory *)g_players.GetPlayer(0)->GetComponentByName( "WeaponInventory" );
		if (inv)
		{
			WeaponLoadout loadout;
			g_armoryView.CreateFromSelection( GetPlayerType(), loadout );
			loadout.Build(*inv, true);
		}
	}

	EndScreen(0);
}

//=============================================================================
// ScreenWeaponsInGame::OnButtonCancel
//=============================================================================

void ScreenWeaponsInGame::OnButtonCancel()
{
	EndScreen(0);
}

