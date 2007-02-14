#include "TTLPCH.h"
#include "frontend/frontend.h"
#include "frontend/savegame.h"
#include "frontend/inputdriver.h"
#include "frontend/texteffects.h"
#include "frontend/screens/screen_PC_LevelSelect.h"
#include "data/screens/pcscreens.h"
#include "frontend/savegame.h"
#include "environ/ConfigMgr.h"
#include "Network/NetZ/NetZConnection.h"
#include "GameHelper/Translator.h"

//=============================================================================
// ScreenPCLevelSelect::OnInitialize
//=============================================================================

void ScreenPCLevelSelect::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	// Setup the big level name text at the bottom
	Text* pBigProfile = GetTextSprite( ID_LEVELSELECT_BIGPROFILE );

	if( pBigProfile )
	{
		pBigProfile->SetTextEffect( new BassPumpEffect );
	}

	// Set up the buttons
	SetInitialButton(ID_LEVELSELECT_START);
	GetSprite(ID_LEVELSELECT_BACK)->Hide();
	SetNextButton(ID_LEVELSELECT_LCURSOR);
	SetNextButton(ID_LEVELSELECT_RCURSOR);
	SetNextButton(ID_LEVELSELECT_UCURSOR);
	SetNextButton(ID_LEVELSELECT_DCURSOR);

	// Create a cycle button for level names
	AddInterface(ID_LEVELSELECT_PARAM, GuiCycleButton::ClassnameStatic());

	// Create a list for checkpoint names
	m_pListBox = CreateListBox(ID_LEVELSELECT_CHKPT);

	ASSERT(GetCycleButton());
	ASSERT(m_pListBox);

	m_pListBox->InitControl( this );

	m_pListBox->InitItem( GetTextSprite( ID_LEVELSELECT_CHKPT1 ) );
	m_pListBox->InitItem( GetTextSprite( ID_LEVELSELECT_CHKPT2 ) );
	m_pListBox->InitItem( GetTextSprite( ID_LEVELSELECT_CHKPT3 ) );
	m_pListBox->InitItem( GetTextSprite( ID_LEVELSELECT_CHKPT4 ) );
	m_pListBox->InitItem( GetTextSprite( ID_LEVELSELECT_CHKPT5 ) );

	//m_pListBox->InitCursor( GetSprite( ID_LEVELSELECT_CURSOR ) );

	m_pListBox->SetSelectedColor( 224, 224, 224, 255 );

	Text* pText = m_pListBox->GetDisplayItem(0);

	if( pText )
	{
		m_pListBox->SetNormalColor( pText->m_state.color.r,
														pText->m_state.color.g,
														pText->m_state.color.b,
														pText->m_state.color.a );
	}
}

//=============================================================================
// ScreenPCLevelSelect::OnScreenIntro
//
//		When we go to this screen, the first thing we do is check to see if any
//		of the maps have been unlocked.  If not, we immediately skip the screen
//		and go directly into the game, otherwise a list of all the unlocked maps
//		are displayed and the user can choose to play on any of them.
//=============================================================================
void ScreenPCLevelSelect::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();

	// Get the first level in single player mode...
	int InitialMap = g_gameEnvironment.GetByType( 0, CGameEnv::TYPE_SINGLEPLAYER );
	int NumMaps = 0;

	// Populate the cycle list box with unlocked levels...
	ASSERT(GetCycleButton());
	ASSERT(m_pListBox);

	// Reset pressed button states
	GetButton(ID_LEVELSELECT_START)->m_Selected = false;
	GetButton(ID_LEVELSELECT_LCURSOR)->m_Selected = false;
	GetButton(ID_LEVELSELECT_RCURSOR)->m_Selected = false;

	// First, clear any existing items
	GetCycleButton()->ClearItems();
	
	int GameEnv1 = InitialMap;
	int GameEnv2;

	// Check the first level...
	if( g_GlobalProfile.GetLevelUnlocked( GameEnv1 ) )
	{
		GetCycleButton()->AddItem( g_gameEnvironment.GetWorldDisplayName( GameEnv1 ), GameEnv1, 0 );
	}

	GameEnv2 = g_gameEnvironment.GetNextByType( GameEnv1, CGameEnv::TYPE_SINGLEPLAYER );

	// Check the rest of the levels...
	while( GameEnv1 != GameEnv2 )
	{
		if( g_GlobalProfile.GetLevelUnlocked( GameEnv2 ) )
		{
			InitialMap = GameEnv2;
			GetCycleButton()->AddItem( g_gameEnvironment.GetWorldDisplayName( GameEnv2 ), GameEnv2, 0 );
		}

		GameEnv2 = g_gameEnvironment.GetNextByType( GameEnv2, CGameEnv::TYPE_SINGLEPLAYER );
	}

	// Select the initial map...
	GetCycleButton()->SelectItem( InitialMap );

	// Get the item count...
	NumMaps = GetCycleButton()->GetItemCount();

	Printf( ID_LEVELSELECT_PROFILENAME, g_translator.Translate( "Current Profile: %s" ), g_playerProfile.GetAccountName() );

	// Add checkpoints to the checkpoint list here
	RepopulateCheckpointListBox( InitialMap );

	OnCursorUpdate();

	g_MenuCursor.SetHidden(false);
}

void ScreenPCLevelSelect::RepopulateCheckpointListBox( int level )
{
	// clear what we have right now
	m_pListBox->ClearItems();
	// and add the appropriate checkpoints for the level
	char buffer[2];
	int validCheckpoints = g_GlobalProfile.GetNumValidCheckpoints( level );
	for(int c=1; c <= validCheckpoints; ++c)
	{
		char checkpointString[64] = "Checkpoint ";
		strcat(checkpointString, itoa(c, buffer, 10));
		m_pListBox->AddItem(checkpointString, c, 0);
	}
	m_pListBox->SelectItem(validCheckpoints);
}


//=============================================================================
// ScreenPCLevelSelect::OnCursorUpdate
//=============================================================================

void ScreenPCLevelSelect::OnCursorUpdate( void )
{
	ASSERT(GetCycleButton());
	ASSERT(m_pListBox);

	// Update the bit text thingy at the bottom to have the current level name
	GuiListBox::Item* pItem = GetCycleButton()->GetItem( GetCycleButton()->CurrentItem() );

	if( pItem )
	{
		Text* pBigProfile = GetTextSprite( ID_LEVELSELECT_BIGPROFILE );

		if( pBigProfile )
		{
			pBigProfile->SetText( pItem->GetName() );
			pBigProfile->InitAnim();

			SpriteAnim& anim = *pBigProfile->m_anim;

			anim.offset.x.SetFrame( 0.0f, -100.0f );
			anim.offset.x.SetFrame( 0.2f,  -10.0f );
			anim.offset.x.SetFrame( 1.0f,    0.0f );
			anim.offset.x.Deccelerate( true );
			anim.offset.x.Play( false );

			anim.scale.x.SetFrame( 0.0f, 3.5f );
			anim.scale.x.SetFrame( 1.0f, 1.8f );
			anim.scale.x.Deccelerate( true );
			anim.scale.x.Play( false );

			anim.familycolor.a.SetFrame( 0.0f, 0.0f );
			anim.familycolor.a.SetFrame( 0.3f, 1.0f );
			anim.familycolor.a.Deccelerate( true );
			anim.familycolor.a.Play( false );
		}
	}

	// If the last item on screen is not the last item on the list, show the down arrow
	if (m_pListBox->GetBottomItem() != m_pListBox->GetItemCount() - 1)
	{
		GetSprite(ID_LEVELSELECT_DCURSOR)->Show();
	}

	// If the first item on the screen is not item 0, show the up arrow
	if (m_pListBox->GetBottomItem() >= m_pListBox->GetItemDisplayCount())
	{
		GetSprite(ID_LEVELSELECT_UCURSOR)->Show();
	}
}


//=============================================================================
// ScreenPCLevelSelect::OnButtonOK
//=============================================================================

void ScreenPCLevelSelect::OnButtonOK( void )
{
	ASSERT(GetCycleButton());
	ASSERT(m_pListBox);

	ScreenBaseClass::OnButtonOK();

	// Start button pressed, play
	if (GetButton(ID_LEVELSELECT_START)->m_Selected)
	{
		StartCurrentMap();
	}

	// Left arrow button hit, go to previous level
	else if (GetButton(ID_LEVELSELECT_LCURSOR)->m_Selected)
	{
		GetCycleButton()->SelectNextItem();
		RepopulateCheckpointListBox( GetCycleButton()->CurrentSelection() );
	}

	// Right arrow button hit, go to next level
	else if (GetButton(ID_LEVELSELECT_RCURSOR)->m_Selected)
	{
		GetCycleButton()->SelectPrevItem();
		RepopulateCheckpointListBox( GetCycleButton()->CurrentSelection() );
	}

	else if (GetButton(ID_LEVELSELECT_DCURSOR)->m_Selected)
	{
		// select next checkpoint
		OnCursorDown();
		
	}

	else if (GetButton(ID_LEVELSELECT_UCURSOR)->m_Selected)
	{
		// select previous checkpoint
		OnCursorUp();
	}

	// Start button pressed, play
	else if (GetButton(ID_LEVELSELECT_START)->m_Selected)
	{
		StartCurrentMap();
	}
}

//=============================================================================
// ScreenPCLevelSelect::GetCycleButton
//=============================================================================

GuiCycleButton* ScreenPCLevelSelect::GetCycleButton()
{
	return (GuiCycleButton*)GetInterface(ID_LEVELSELECT_PARAM, GuiCycleButton::ClassnameStatic());
}

void ScreenPCLevelSelect::OnCursorDown( void )
{
	ASSERT(m_pListBox);

	m_pListBox->OnCursorDown();

	OnCursorUpdate();
}

void ScreenPCLevelSelect::OnCursorUp( void )
{
	ASSERT(m_pListBox);

	m_pListBox->OnCursorUp();

	OnCursorUpdate();
}

void ScreenPCLevelSelect::OnButtonPressed( int Button )
{
	if (Button == FluffControlMapper::START)
	{
		StartCurrentMap();
		return;
	}
	ScreenBaseClass::OnButtonPressed( Button );
}

void ScreenPCLevelSelect::OnCursorRight( void )
{
	// grab the next level and refresh the checkpoint list box
	GetCycleButton()->SelectNextItem();
	RepopulateCheckpointListBox( GetCycleButton()->CurrentSelection() );
}

void ScreenPCLevelSelect::OnCursorLeft( void )
{
	// grab the next level and refresh the checkpoint list box
	GetCycleButton()->SelectPrevItem();
	RepopulateCheckpointListBox( GetCycleButton()->CurrentSelection() );
}

void ScreenPCLevelSelect::StartCurrentMap()
{
	// TODO: Set checkpoint and such here
	// You can get the checkpoint number by getting the ItemId from the list
	int GameEnv = GetCycleButton()->CurrentSelection();
	if ( g_gameEnvironment.Get() != GameEnv )
			g_gameEnvironment.Set( GameEnv );
	if (g_GlobalProfile.GetNumValidCheckpoints( GameEnv ) == 0)
		g_gameEnvironment.SetCheckpoint( 0 );
	else
		g_gameEnvironment.SetCheckpoint( m_pListBox->CurrentSelection() );
	g_gameEnvironment.SetCheckpointFlag(true);
	g_messageDispatcher.SendMessageToAll( "SetupLaunch", NULL, INVALID_OBJECT_HANDLE );

	GotoScreen(0);
}

void ScreenPCLevelSelect::OnScreenUpdate()
{
	ScreenBaseClass::OnScreenUpdate();
}