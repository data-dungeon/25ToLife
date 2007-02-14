#include "TTLPCH.h"
#include "main/TTL.h"

#include "Display/TextureUtil.h"
#include "platform/MemoryCard/MemoryCard.h"

#include "frontend/frontend.h"
#include "frontend/inputdriver.h"
#include "frontend/savegame.h"
#include "frontend/texteffects.h"
#include "frontend/screens/screen_Popups.h"
#include "frontend/screens/screen_OnlineProfile.h"

#ifdef DIRECTX_PC
#include "data/screens/pcscreens.h"
#include "frontend/inputdriver.h"
#else
#include "data/screens/mainmenu.h"
#endif // DIRECTX_PC

#include "main/PlayerDB.h"

enum
{
	COMMAND_CREATEPROFILE_OK	= 0x100,
	COMMAND_CREATEPROFILE_CANCEL,
	COMMAND_CREATEPROFILE_CONTINUE,
	COMMAND_DELETEPROFILE_OK,
	COMMAND_DELETEPROFILE_CANCEL,
};

#define PROFILE_CREATE_COMMAND		(0xe775f320)
#define PROFILE_BACK_COMMAND			(0xe775f321)
#define PROFILE_QUIT_COMMAND			(0xe775f322)

extern	bool		g_MainMenuSignOut;

//=============================================================================
// ScreenOnlineProfile::ScreenOnlineProfile - The default constructor
//=============================================================================

ScreenOnlineProfile::ScreenOnlineProfile( void )
{
	m_ProfilesFound = 0;
	m_MemCardState = ONLINEPROFILE_IDLE;
	m_LastMemCardState = ONLINEPROFILE_IDLE;
	m_Selection = PROFILE_CREATE_COMMAND;
}

//=============================================================================
// ScreenOnlineProfile::~ScreenOnlineProfile - The default destructor
//=============================================================================

ScreenOnlineProfile::~ScreenOnlineProfile( void )
{
}

void ScreenOnlineProfile::ShowProfilesPrompt( void )
{
	ScreenPopupDialog* pDialog = GetPopupDialog();

	if( pDialog )
	{
		pDialog->Clear();
		pDialog->SetTitle( "No Profiles Found" );
		pDialog->SetMessage( "You must create a new profile to save your progress." );
		pDialog->AddMenuItem( "OK", COMMAND_CREATEPROFILE_OK, 0 );
		pDialog->AddMenuItem( "Cancel", COMMAND_CREATEPROFILE_CANCEL, 0 );
		pDialog->AddMenuItem( "Continue Without Saving", COMMAND_CREATEPROFILE_CONTINUE, 0 );
		pDialog->SelectMenuItem( COMMAND_CREATEPROFILE_OK );
	}

	ShowPopupDialog();
}

void ScreenOnlineProfile::ShowProfileDeletePrompt(	void )
{
	ScreenPopupDialog* pDialog = GetPopupDialog();
	GuiListBox* pListBox = GetListBox();

	if( pDialog && pListBox )
	{
		if( pListBox->CurrentSelection() != PROFILE_CREATE_COMMAND )
		{
			pDialog->Clear();
			pDialog->SetTitle( pListBox->GetItemText( pListBox->CurrentItem() ) );;
			pDialog->SetMessage( "Are you sure you want to delete this profile?" );
			pDialog->AddMenuItem( "OK", COMMAND_DELETEPROFILE_OK, 0 );
			pDialog->AddMenuItem( "Cancel", COMMAND_DELETEPROFILE_CANCEL, 0 );
			pDialog->SelectMenuItem( COMMAND_DELETEPROFILE_CANCEL );

			ShowPopupDialog();
		}
	}

	return;
}

void ScreenOnlineProfile::DeleteProfile( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		m_MemCardState = ONLINEPROFILE_DELETE;
		m_Selection = pListBox->CurrentSelection();
	}

	return;
}

//=============================================================================
// ScreenOnlineProfile::OnInitialize
//=============================================================================

void ScreenOnlineProfile::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();
#ifndef DIRECTX_PC
	SetHelpRect( ID_ONLINEPROFILE_HELPRECT );
#endif
	GuiListBox *pListBox = CreateListBox( ID_ONLINEPROFILE_PROFILEGROUP );

	if( pListBox )
	{
		pListBox->InitControl( this );
		pListBox->InitItem( GetTextSprite( ID_ONLINEPROFILE_PROFILE1 ) );
		pListBox->InitItem( GetTextSprite( ID_ONLINEPROFILE_PROFILE2 ) );
		pListBox->InitItem( GetTextSprite( ID_ONLINEPROFILE_PROFILE3 ) );
		pListBox->InitItem( GetTextSprite( ID_ONLINEPROFILE_PROFILE4 ) );
		pListBox->InitItem( GetTextSprite( ID_ONLINEPROFILE_PROFILE5 ) );
		pListBox->InitItem( GetTextSprite( ID_ONLINEPROFILE_PROFILE6 ) );
		pListBox->InitCursor( GetSprite( ID_ONLINEPROFILE_CURSOR ) );
	}

	Text* pBigProfile = GetTextSprite( ID_ONLINEPROFILE_BIGPROFILE );

	if( pBigProfile )
		pBigProfile->SetTextEffect( new BassPumpEffect );

	m_MemCardState = ONLINEPROFILE_IDLE;

	GuiScrollBar* pScrollBar = CreateScrollBar( ID_ONLINEPROFILE_SCROLLBAR );

	if( pScrollBar )
	{
		Sprite* pSprite = pScrollBar->SpriteHost();

		pScrollBar->InitPart( GuiScrollBar::Shaft, pSprite );
		pScrollBar->InitPart( GuiScrollBar::ArrowUp, GetSprite( ID_ONLINEPROFILE_UPARROW, pSprite ) );
		pScrollBar->InitPart( GuiScrollBar::ArrowDown, GetSprite( ID_ONLINEPROFILE_DOWNARROW, pSprite ) );

		pScrollBar->SetRange( 0.0f, 10.0f );
		pScrollBar->SetPosition( 5.0f );
		pScrollBar->SetSteps( 10 );
	}

#ifdef DIRECTX_PC
	EnableButton(ID_ONLINEPROFILE_DELETE, true);
	if( pBigProfile )
		pBigProfile->SetText( "" );
#endif
	return;
}

void ScreenOnlineProfile::OnScreenIntro( void )
{
	g_playerProfile.SetAccountName( "Profile1" );
	g_playerProfile.SetDisplayName( "" );

	m_CardID = -1;

	bool	Ignore = false;
	g_MainMenuSignOut = false;

	GuiListBox* pListBox = GetListBox();

	if( pListBox )
		pListBox->ClearItems();

	m_MemCardState = ONLINEPROFILE_DETECT;

	OnCursorUpdate();
}

// This handles async loading of profiles....

static	ts_SaveProfile		LoadedProfile;
static	ts_CMemCardFiles	fi;

void ScreenOnlineProfile::OnScreenUpdate()
{
	int					i;
	int					csum;
	bool					Ignore = false;

	bool					recheckCard = g_MemCardManager->GetCardChanged(g_MemCardManager->GetPort());

#if defined(PS2)
	if (m_CardID != ((PS2MemCard*)g_MemCardPtr)->CardID(g_MemCardManager->GetPort()))
		recheckCard = true;
#endif

	// check for card change....
	if (recheckCard)
	{
		// make sure we are idle first.. so it doesnt interrupt other states
		if (m_MemCardState == ONLINEPROFILE_IDLE)
		{
			#if defined (PS2)
			Ignore = true;
			#endif
			g_FrontEnd.ClosePopup("SCREEN_ID_POPUPDIALOG");
			g_MemCardManager->GetFilesOnAllCards( SAVEGAME_FILENAME, SAVEGAME_SEARCH_MASK, Ignore );
			m_MemCardState = ONLINEPROFILE_GET_PROFILES_RESULT;
			g_MemCardManager->SetCardChanged(g_MemCardManager->GetPort(), false);
		}
	}

	switch (m_MemCardState)
	{
	case	ONLINEPROFILE_IDLE:		
		break;

	case	ONLINEPROFILE_DELETE:

		// get fileinfo
		g_MemCardManager->GetFileInfo( m_Selection, fi );

			// set filename ext
		g_MemCardManager->SetExt(SAVEGAME_EXT);
		g_MemCardManager->SetAreYouSure(true);

		char	NewFileName[128];
		sprintf(NewFileName, "%s%s", SAVEGAME_PREFIX, fi.LoadFilename);
		g_MemCardManager->SetUsePopups(true);
		g_MemCardManager->DeleteFile(SAVEGAME_FILENAME, NewFileName, 0, MC_SAVED_PROFILE);
		m_MemCardState = ONLINEPROFILE_DELETE_RESULT;
		m_LastMemCardState = m_MemCardState;
		break;

	case	ONLINEPROFILE_DELETE_RESULT:
		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
		{
			m_MemCardState = ONLINEPROFILE_DETECT;
		}
		m_LastMemCardState = m_MemCardState;
		break;

	case	ONLINEPROFILE_DETECT:		
		g_MemCardManager->DetectCard(0);
		m_MemCardState = ONLINEPROFILE_DETECT_RESULT;
		m_LastMemCardState = m_MemCardState;
		break;

	case	ONLINEPROFILE_DETECT_RESULT:		
		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
		{
			int Result = g_MemCardManager->GetResult();

			if( Result == MEMCARD_RESULT_NOCARD )
			{
				//#ifdef DIRECTX_PC
				//	GotoScreen("SCREEN_ID_MAINMENU_PC");
				//#else
					GotoScreen(SCREEN_MAINMENU);
				#ifndef DIRECTX_PC
				// reset to defaults if we have no card, clear levels completed.. set names etc
				g_GlobalProfile.ResetToDefaults();
				CSaveGame::SetProfileName("Profile1");
				g_playerProfile.SetAccountName( "Profile1" );
				memset(&g_GlobalProfile.SinglePlayer, 0, sizeof (ts_SinglePlayer));

				#endif
				g_MemCardManager->ResetLoadSaveBuffer();
				m_MemCardState = ONLINEPROFILE_IDLE;	
			}
			else
			{
#if defined (PS2)
				Ignore = true;
#endif
				g_MemCardManager->GetFilesOnAllCards( SAVEGAME_FILENAME, SAVEGAME_SEARCH_MASK, Ignore );
				m_MemCardState = ONLINEPROFILE_GET_PROFILES_RESULT;
			}
		}
		m_LastMemCardState = m_MemCardState;
		break;

	case	ONLINEPROFILE_GET_PROFILES_RESULT:	
		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
		{
			#if defined(PS2)
			m_CardID = ((PS2MemCard*)g_MemCardPtr)->CardID(g_MemCardManager->GetPort());
			#endif

			m_ProfilesFound = g_MemCardManager->GetNumFilesOnCard();

			if( m_ProfilesFound == 0 )
			{
				g_MemCardManager->SetCardChanged(g_MemCardManager->GetPort(), false);
				ShowProfilesPrompt();
				m_MemCardState = 0x0;			// wait for result of popup
			}
			else
			{
				ScreenBaseClass::OnScreenIntro();

				GuiListBox *pListBox = GetListBox();

				if( pListBox )
				{
					pListBox->ClearItems();

					ts_CMemCardFiles FileInfo;

					for( int i = 0; i < m_ProfilesFound; i++ )
					{
						g_MemCardManager->GetFileInfo( i, FileInfo );

						pListBox->AddItem( FileInfo.DisplayName, i, 0 );
					}

					pListBox->AddItem( "Create Profile", PROFILE_CREATE_COMMAND, 0 );
#ifdef DIRECTX_PC
					pListBox->AddItem( "Back", PROFILE_BACK_COMMAND, 0 );
#endif
					pListBox->SelectIndex(0);
					pListBox->SetItemColor( PROFILE_CREATE_COMMAND, 120, 130, 255, 255 );					

					OnCursorUpdate();
				}
				g_MemCardManager->ResetLoadSaveBuffer();
				m_MemCardState = ONLINEPROFILE_IDLE;
			}
		}
		m_LastMemCardState = m_MemCardState;
		break;

	case	ONLINEPROFILE_LOAD:

		// get fileinfo
		g_MemCardManager->GetFileInfo( m_Selection, fi );

		// set filename ext
		g_MemCardManager->SetExt(SAVEGAME_EXT);

		sprintf(NewFileName, "%s%s", SAVEGAME_PREFIX, fi.LoadFilename);

		g_MemCardManager->SetUsePopups(true);
		g_MemCardManager->SetNoOkConfirmation(true);
		g_MemCardManager->LoadFile(SAVEGAME_FILENAME, NewFileName, 0, (char*)&LoadedProfile, sizeof(ts_SaveProfile), false, MC_SAVED_PROFILE);

		m_MemCardState = ONLINEPROFILE_LOAD_RESULT;
		m_LastMemCardState = m_MemCardState;
		break;

	case	ONLINEPROFILE_LOAD_RESULT:
		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
		{
			g_MemCardManager->SetNoOkConfirmation(false);

			if (g_MemCardManager->GetResult() != MEMCARD_RESULT_OK)
			{
				m_MemCardState = ONLINEPROFILE_LOAD_BADDATA;
				m_LastMemCardState = m_MemCardState;
				break;
			}

			// check savegame date version ID.. make ppl use new save games per build
			if (g_MemCardManager->GetLoadedAppVersion2() != SAVEGAME_VER)
			{
				g_MemCardManager->SetResult(MEMCARD_RESULT_BADDATA);
				g_MemCardManager->HandleResult();
				m_MemCardState = ONLINEPROFILE_LOAD_BADDATA;
				m_LastMemCardState = m_MemCardState;
				break;
			}

			// check version number
			if (g_MemCardManager->GetLoadedAppVersion1() != PlayerDB::GetVersionID())		
			{	
				g_MemCardManager->SetResult(MEMCARD_RESULT_BADDATA);
				g_MemCardManager->HandleResult();
				m_MemCardState = ONLINEPROFILE_LOAD_BADDATA;
				m_LastMemCardState = m_MemCardState;
				break;
			}

			// check saved player checksums
			for (i = 0; i < CS_MAX_CHARACTERS; i++)
			{
				csum = CustomPlayer::CreateCheckSum(&LoadedProfile.Characters[i]);
				if (csum != LoadedProfile.Characters[i].CheckSum)
				{
					// checksum is bad.. display load failed message.. 
					g_MemCardManager->SetResult(MEMCARD_RESULT_BADDATA);
					g_MemCardManager->HandleResult();
					m_MemCardState = ONLINEPROFILE_LOAD_BADDATA;
					m_LastMemCardState = m_MemCardState;
					break;
				}
			}

			// make sure error case breaks case... 
			if (m_MemCardState == ONLINEPROFILE_LOAD_BADDATA)
				break;

			// set to ok prompt
			g_MemCardManager->SetResult(MEMCARD_RESULT_OK);
			g_MemCardManager->HandleResult();
			m_MemCardState = ONLINEPROFILE_LOAD_RESULT_OK;
			m_LastMemCardState = m_MemCardState;
			break;
		}

	case	ONLINEPROFILE_LOAD_RESULT_OK:
		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
		{
			// copy over the struct now
			memcpy( &ScreenBaseClass::m_LastProfileSaved, &LoadedProfile, sizeof(ts_SaveProfile));
			memcpy(&g_GlobalProfile, &LoadedProfile, sizeof(ts_SaveProfile));

			// load the custom characters into the player database
			for (i = 0; i < CS_MAX_CHARACTERS; i++)
			{
				int dbIndex = PlayerDB::CharacterIndexFromId( PLAYERDB_CUSTOM | i );
				bool selectable = true; //strlen( g_GlobalProfile.Characters[ i ].InitString ) > 0;
				PlayerDB::SetCharacter( dbIndex,
												g_GlobalProfile.Characters[ i ].Name,
												g_GlobalProfile.Characters[ i ].InitString,
												g_GlobalProfile.Characters[ i ].CharType,
												selectable );
			}

			// Set the selected characters (call the function with the already set data? YOU BET. We need secondary modules to know too.)
			g_GlobalProfile.SelectCharacter( CPlayerProfile::COP, g_GlobalProfile.GetSelectedCharacter( CPlayerProfile::COP ) );
			g_GlobalProfile.SelectCharacter( CPlayerProfile::GANG, g_GlobalProfile.GetSelectedCharacter( CPlayerProfile::GANG ) );

			// update game locks
			g_GlobalProfile.LoadGameLocks();

			// update options
			CGameSettings::LoadSettings();
			strcpy( g_SaveGameParams.ProfileName, LoadedProfile.ProfileName );
			g_SaveGameParams.Port = 0;
			//----- FROM SAVEGAME

			// rest of dans stuff
			g_playerProfile.SetAccountName( fi.DisplayName );

			g_ttl->SetSoundVolume( Math::Clamp(g_GlobalProfile.GetSoundVolume() ), false );
			g_ttl->SetMusicVolume( Math::Clamp(g_GlobalProfile.GetMusicVolume() ), false );
			g_ttl->SetStereoSound( g_GlobalProfile.GetStereoMode() );
			TextureUtil::SetGammaZeroToOne( g_GlobalProfile.GetGammaValue() );
			TextureUtil::UpdateGamma();

			// set gamma flag
			if ( Math::Equal(g_GlobalProfile.GetGammaValue(),PROFILE_DEFAULT_GAMMA))
				TextureUtil::SetGammaOnLoadFlag(false);
			else
				TextureUtil::SetGammaOnLoadFlag(true);

//			g_MemCardManager->SetAutosave( g_GlobalProfile.GetAutoSave() );
						
			if (g_MemCardManager->GetResult() == MEMCARD_RESULT_OK)
			{
					GotoScreen(SCREEN_MAINMENU);
			}
			g_MemCardManager->ResetLoadSaveBuffer();
			m_MemCardState = ONLINEPROFILE_IDLE;
		}
		break;

		case	ONLINEPROFILE_LOAD_BADDATA:
			if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
			{
				if (g_MemCardManager->GetPopupExitResult() != PR_RETRY_YES)
				{
					g_MemCardManager->ResetLoadSaveBuffer();
					m_MemCardState = ONLINEPROFILE_IDLE;
				}
				else
				{
					g_MemCardManager->ResetLoadSaveBuffer();
					m_MemCardState = ONLINEPROFILE_LOAD;
				}
			}
			m_LastMemCardState = m_MemCardState;
			break;

		default:
			break;
	}
	ScreenBaseClass::OnScreenUpdate();
#ifdef DIRECTX_PC
		WindowsSystemInputDriver* wsid = (WindowsSystemInputDriver*)g_inputPtr;
		if ( wsid->ButtonPressed(BC_DELETE))
		{
			GuiListBox* pListBox = GetListBox();
			if( pListBox && pListBox->GetItemCount() > 0 )
			{
				int selection = pListBox->CurrentSelection();
				if( m_MemCardState == ONLINEPROFILE_IDLE && 
					selection != PROFILE_BACK_COMMAND	 &&
					selection != PROFILE_CREATE_COMMAND     )
				{
					m_Selection = selection;
					m_MemCardState = ONLINEPROFILE_DELETE;
				}
			}
		}
#endif
}

//=============================================================================
// ScreenOnlineProfile::OnCursorUp
//=============================================================================

void ScreenOnlineProfile::OnCursorUp( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int OldPos = pListBox->CurrentItem();
			
		pListBox->OnCursorUp();

		int NewPos = pListBox->CurrentItem();

		if( OldPos != NewPos )
		{
#ifndef DIRECTX_PC
			PlaySoundEvent( Screen::SOUNDEVENT_PREVBUTTON );
#endif
			OnCursorUpdate();
		}
	}

	return;
}

//=============================================================================
// ScreenOnlineProfile::OnCursorDown
//=============================================================================

void ScreenOnlineProfile::OnCursorDown( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{

		int OldPos = pListBox->CurrentItem();

		pListBox->OnCursorDown();

		int NewPos = pListBox->CurrentItem();

		if( OldPos != NewPos )
		{
#ifndef DIRECTX_PC
			PlaySoundEvent( Screen::SOUNDEVENT_NEXTBUTTON );
#endif
			OnCursorUpdate();
		}
	}
	return;
}

//=============================================================================
// ScreenOnlineProfile::OnCursorUpdate
//=============================================================================

void ScreenOnlineProfile::OnCursorUpdate( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		GuiListBox::Item* pItem = pListBox->GetItem( pListBox->CurrentItem() );

#ifdef DIRECTX_PC
		float x1, x2, y1, y2, x, y;
		x = g_MenuCursor.GetX();
		y = g_MenuCursor.GetY();

		pListBox->GetDisplayItem(pListBox->CurrentItem())->GetFamilyRect(x1, y1, x2, y2);

		if ( ! (( x > x1 && x < x2 ) && ( y > y1 && y < y2 )) )
			return;
#endif

		const char* pName = "";

		if( pItem )
			pName = pItem->GetName();

		SetText( ID_ONLINEPROFILE_CURRENTPROFILE, pName );

		Text* pBigProfile = GetTextSprite( ID_ONLINEPROFILE_BIGPROFILE );

		if( pBigProfile )
		{
			pBigProfile->SetText( pName );
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

		Text* pHelp = NULL;
#ifndef DIRECTX_PC
		pHelp = GetTextSprite( ID_ONLINEPROFILE_HELPTEXT4 );
#endif // DIRECTX_PC

		if( pHelp )
		{
			if( pListBox->CurrentSelection() == PROFILE_CREATE_COMMAND )
			{
				pHelp->m_state.familycolor.a = 0;
			}
			else
			{
				pHelp->m_state.familycolor.a = 255;
			}
		}
	}

	return;
}

//=============================================================================
// ScreenOnlineProfile::OnButtonOK
//=============================================================================

void ScreenOnlineProfile::OnButtonOK( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox && pListBox->GetItemCount() > 0 )
	{
		int Selection = pListBox->CurrentSelection();

		if( Selection == PROFILE_CREATE_COMMAND )
		{
			GotoScreen( "SCREEN_ID_CREATEPROFILE" );
		}
#ifdef DIRECTX_PC
		else if ( Selection == PROFILE_BACK_COMMAND)
		{
			OnButtonCancel();
			return;
		}
		else if ( Selection == PROFILE_QUIT_COMMAND)
		{
			g_ttl->Exit();
			return;
		}
#endif
		else
		{
			
#ifdef DIRECTX_PC
			float x1, y1, x2, y2;	
			float x = g_MenuCursor.GetX();
			float y = g_MenuCursor.GetY();
			Button* button = GetButton(ID_ONLINEPROFILE_DELETE);
			// search through the buttons to see if we're over one
			// if so, check to see if we're clicking the mouse
			//HACK HACK HACK
			button->SpriteHost()->GetFamilyRect(x1, y1, x2, y2);
			if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) )
			{
				if( m_MemCardState == ONLINEPROFILE_IDLE )
				{
					m_MemCardState = ONLINEPROFILE_DELETE;
					m_Selection = Selection;
					button->m_Selected = true;
					g_MenuCursor.ForceButtonUp(0);
					return;
				}				
			}
#endif
			if( m_MemCardState == ONLINEPROFILE_IDLE )
			{
				m_MemCardState = ONLINEPROFILE_LOAD;
				m_Selection = Selection;
			}
		}
	}

	return;
}

//=============================================================================
// ScreenOnlineProfile::OnButtonCancel
//=============================================================================

void ScreenOnlineProfile::OnButtonCancel( void )
{
	if( m_MemCardState == ONLINEPROFILE_IDLE )
	{
		ScreenBaseClass::OnButtonCancel();
		g_MainMenuSignOut = true;
	}

	return;
}


//=============================================================================
// ScreenOnlineProfile::OnButtonPressed
//=============================================================================

void ScreenOnlineProfile::OnButtonPressed( int Button )
{
	switch( Button )
	{
	case FluffControlMapper::SQUARE:
		GotoScreen( "SCREEN_ID_CREATEPROFILE" );
		break;

#ifndef DIRECTX_PC
	case FluffControlMapper::CIRCLE:
		ShowProfileDeletePrompt();
		break;
#endif //DIRECTX_PC

	default:
		ScreenBaseClass::OnButtonPressed( Button );
		break;
	}

	return;
}

//=============================================================================
// ScreenOnlineProfile::OnDialogCommand
//=============================================================================

void ScreenOnlineProfile::OnDialogCommand( int Command )
{
	switch( Command )
	{
	case COMMAND_CREATEPROFILE_OK:
		GotoScreen( "SCREEN_ID_CREATEPROFILE" );
		break;

	case COMMAND_CREATEPROFILE_CANCEL:
		OnDialogCancel();
		break;

	case COMMAND_CREATEPROFILE_CONTINUE:
		//#ifdef DIRECTX_PC
		//	GotoScreen("SCREEN_ID_MAINMENU_PC");
		//#else
			GotoScreen(SCREEN_MAINMENU);
		//#endif
		break;

	case COMMAND_DELETEPROFILE_OK:
		EndScreen(0);
		DeleteProfile();
		break;

	case COMMAND_DELETEPROFILE_CANCEL:
		EndScreen(0);
		break;
	}

	return;
}

//=============================================================================
// ScreenOnlineProfile::OnDialogCancel
//=============================================================================

void ScreenOnlineProfile::OnDialogCancel( void )
{
	ScreenBaseClass::OnDialogCancel();

	GuiListBox* pListBox = GetListBox();
	int c = 0;

	if( pListBox )
		c = pListBox->GetItemCount();

	if( c < 2 || m_ProfilesFound < 1 )
		GotoPrevScreen();

	return;
}

//=============================================================================
// ScreenOnlineProfile::OnPopupShow
//=============================================================================

void ScreenOnlineProfile::OnPopupShow( void )
{
	ScreenBaseClass::OnPopupShow();
	ScreenBaseClass::OnScreenExit();
}

//=============================================================================
// ScreenOnlineProfile::OnPopupHide
//=============================================================================

void ScreenOnlineProfile::OnPopupHide( void )
{
	ScreenBaseClass::OnPopupHide();
	ScreenBaseClass::OnScreenIntro();
}

//=============================================================================
// ScreenOnlineProfile::HandleInput
//=============================================================================

void ScreenOnlineProfile::HandleInput( UIHandler* pUIHandler )
{
	if( m_MemCardState == ONLINEPROFILE_IDLE )
		ScreenBaseClass::HandleInput( pUIHandler );
	
	return;
}