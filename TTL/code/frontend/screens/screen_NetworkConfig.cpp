#include "TTLPCH.h"
#include "frontend/frontend.h"
#include "frontend/savegame.h"
#include "frontend/inputdriver.h"
#include "frontend/screens/screen_NetworkConfig.h"
#include "frontend/screens/screen_NetworkError.h"
#include "frontend/screens/screen_Popups.h"
#include "data/screens/OnlineScreens.h"
#include "frontend/savegame.h"
#include "environ/ConfigMgr.h"
#include "Network/NetZ/NetZConnection.h"

#ifdef PS2
#include <platform/stack/ps2common/ps2networking.h>
#include <platform/stack/ps2common/ps2profilelist.h>
#include "platform/wantpal.h" 
#endif

#include "data/screens/mainmenu.h"

#ifdef SN_TARGET_PS2
#include <netz.h>
#include <platform/stack/ps2common/ps2networking.h>
#include <platform/stack/ps2common/ps2module.h>
#include <platform/stack/ps2common/ps2profilelist.h>
#endif

//PS2ProfileList s_profileList;
extern const char* g_RemoteIPAddress;
static char *AdapterTypes[ 4 ] = { "Ethernet", "Ethernet", "Modem", "Unknown" };

// async vars
int				SN_result = 0;
extern int g_mainMenuSelection;
enum
{
	DIALOG_OK = 0x100,
	DIALOG_CANCEL,
};

//=============================================================================
// ScreenNetworkConfig::ScreenNetworkConfig - The default constructor
//=============================================================================

ScreenNetworkConfig::ScreenNetworkConfig( void )
{
	m_numConfigs = 0;
	m_MemCardState = NETWORKCONFIG_IDLE;
	m_ListBox = NULL;
	SN_result = 0;

	ScreenNetworkError::SetNetworkError( ScreenNetworkError::ERROR_NONE );
}

//=============================================================================
// ScreenNetworkConfig::~ScreenNetworkConfig - The default destructor
//=============================================================================

ScreenNetworkConfig::~ScreenNetworkConfig( void )
{
}


//=============================================================================
// ScreenNetworkConfig::OnInitialize
//=============================================================================

void ScreenNetworkConfig::OnInitialize( void )
{
	UseHighlighting(true);

	SetHelpRect( ID_LANCONFIG_HELPRECT );

	GuiListBox*	pListBox	= CreateListBox( ID_LANCONFIG_LISTBOX );

	if( pListBox )
	{
		pListBox->InitControl( this );
		pListBox->InitItem( GetTextSprite( ID_LANCONFIG_ITEM1 ) );
		pListBox->InitItem( GetTextSprite( ID_LANCONFIG_ITEM2 ) );
		pListBox->InitItem( GetTextSprite( ID_LANCONFIG_ITEM3 ) );
		pListBox->InitItem( GetTextSprite( ID_LANCONFIG_ITEM4 ) );
		pListBox->InitItem( GetTextSprite( ID_LANCONFIG_ITEM5 ) );
		pListBox->InitCursor( GetSprite( ID_LANCONFIG_CURSOR ) );
	}

	for( int i = 0; i < MAX_CONFIGS; i++ )
		m_profileEnabled[ i ] = false;

	// Hide PS2 help bar on PC
#ifdef DIRECTX_PC
	GetSprite(ID_LANCONFIG_HELPGROUP)->HideAll();
#endif

	ScreenBaseClass::OnInitialize();
}

//=============================================================================
// ScreenNetworkConfig::OnScreenIntro
//=============================================================================

void ScreenNetworkConfig::OnScreenIntro( void )
{
	m_MemCardState = NETWORKCONFIG_IDLE;
	m_LastResult = MEMCARD_RESULT_NOCARD;

	ScreenBaseClass::OnScreenIntro();

	int result = -1;
	g_MemCardManager->GetNetworkProfileList()->Clear();
	m_numConfigs = 0;
	for( int i = 0; i < MAX_CONFIGS; i++ ) m_profileEnabled[ i ] = false;

	GuiListBox *pListBox = GetListBox( );

	m_state = STATE_NONE;
	m_timer = 0;
	SetNetworkProduct(0);
	SetNetworkVendor(0);
	SetNetworkType(0);

	if( pListBox )
	{
		pListBox->ClearItems( );
		if ( g_RemoteIPAddress == NULL )	// no atwinmon?
		{
			m_ListBox = pListBox;
			m_MemCardState = NETWORKCONFIG_LOAD;
		}
		else
		{
			pListBox->AddItem( "ATWINMON detected, press X", -1, NULL );
		}
	}

}

//=============================================================================
// ScreenNetworkConfig::OnButtonOK
//=============================================================================

void ScreenNetworkConfig::OnButtonOK( void )
{
	ScreenBaseClass::OnButtonOK();

	if(g_RemoteIPAddress)	// atwinmon
	{
		if( g_mainMenuSelection == ID_MAINMENU_ONLINE )
			ScreenNetworkConnect::MultiPlayerOnline();
		else if( g_mainMenuSelection == ID_MAINMENU_SYSTEMLINK )
			ScreenNetworkConnect::MultiPlayerLAN();
		return;
	}

	GuiListBox *pListBox = GetListBox();
	int index = 0;

	if( pListBox )
	{
		index = pListBox->CurrentSelection();

		if( !pListBox->GetItemEnabled(index) )
		{
			// TODO: must we display a dialog saying this is an invalid configuration?

			ScreenPopupDialog* pDialog = GetPopupDialog();

			if( pDialog )
			{
				pDialog->Clear();
				pDialog->SetMessage( "This is an invalid network configuration." );
				pDialog->AddMenuItem( "OK", DIALOG_OK, 0 );
				ShowPopupDialog();
			}
		}
		else
		{
			if( index >= 0 && m_numConfigs  )
			{
				if( m_profileEnabled[ index ] )
				{
					CNetZConnection::SetNetworkConfiguration( g_MemCardManager->GetNetworkProfileList(), index );
					m_state = STATE_INITIATE_CONNECTION;
					//EndScreen(0);
				}
				else
				{
				//	m_timer = g_timer.GetOSTime() + 3000;

					GotoScreen( "SCREEN_ID_LANERROR" );

				//	m_state = STATE_SHOW_DIALOG;
				}
			}
		}
	}
	return;
}

//=============================================================================
// ScreenNetworkConfig::OnButtonCancel
//=============================================================================

void ScreenNetworkConfig::OnButtonCancel( void )
{
	ScreenBaseClass::OnButtonCancel();
	CNetZConnection::SetNetworkConfiguration( NULL, -1 );
	EndScreen(0);
}

//=============================================================================
// ScreenNetworkConfig::OnCursorUp
//=============================================================================

void ScreenNetworkConfig::OnCursorUp( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int OldPos = pListBox->CurrentItem();

		pListBox->OnCursorUp();

		int NewPos = pListBox->CurrentItem();

		if( OldPos != NewPos )
		{
			PlaySoundEvent( Screen::SOUNDEVENT_PREVBUTTON );
		}
	}

	return;
}

//=============================================================================
// ScreenNetworkConfig::OnCursorDown
//=============================================================================

void ScreenNetworkConfig::OnCursorDown( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int OldPos = pListBox->CurrentItem();

		pListBox->OnCursorDown();

		int NewPos = pListBox->CurrentItem();

		if( OldPos != NewPos )
		{
			PlaySoundEvent( Screen::SOUNDEVENT_NEXTBUTTON );
		}
	}

	return;
}

//=============================================================================
// ScreenNetworkConfig::OnDialogCommand
//=============================================================================

void ScreenNetworkConfig::OnDialogCommand( int Command )
{
	switch( Command )
	{
	case DIALOG_OK:
		EndScreen(0);
		break;

	case DIALOG_CANCEL:
		EndScreen(0);
		break;
	}

	return;
}

//=============================================================================
// ScreenNetworkConfig::OnCommand
//=============================================================================

void ScreenNetworkConfig::OnCommand( int Command )
{
	return;
}

//=============================================================================

void ScreenNetworkConfig::MemCardState()
{
#if defined (PS2)
	switch (m_MemCardState)
	{
		case	NETWORKCONFIG_IDLE:
			g_MemCardManager->DetectCard(g_MemCardManager->GetPort());
			m_MemCardState = NETWORKCONFIG_IDLE_RESULT;
			break;

		case	NETWORKCONFIG_IDLE_RESULT:
			if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
			{
				if (g_MemCardManager->GetResult() == MEMCARD_RESULT_NOCARD && m_LastResult == MEMCARD_RESULT_OK)
				{
					m_MemCardState = NETWORKCONFIG_IDLE;
					EndScreen( 0 );
				}
				else
				{
					m_MemCardState = NETWORKCONFIG_IDLE;
				}
				m_LastResult = g_MemCardManager->GetResult();
			}
			break;

		case	NETWORKCONFIG_LOAD:
			CNetZConnection::InitPaths();
			g_MemCardManager->GetNetworkProfileList()->Clear();

			// call this... to start the load
			g_MemCardManager->LoadNetwork(g_MemCardManager->GetPort());
			m_MemCardState = NETWORKCONFIG_LOAD_RESULT;
			break;

		case	NETWORKCONFIG_LOAD_RESULT:
			if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
			{
				// --- reset configuration list
				CNetZConnection::SetNetworkConfiguration( NULL, -1 );

				// --- did we succeed loading network configurations?
				SN_result = g_MemCardManager->GetNumNetworkProfiles();

				if( SN_result > 0 )
				{
					m_numConfigs = SN_result;

					// SCEE has max of 4.
					if (g_wantPAL)
					{
						if (m_numConfigs > 4)
							m_numConfigs = 4;
					}

					for( int i = 0; i < SN_result; i++ )
					{
						PS2ConnectionProfile *profile = g_MemCardManager->GetNetworkProfileList()->GetProfile( i );
						char name[ 128 ];

						sprintf( name, "%d - %s", i + 1, profile->GetProfileName() );
						m_ListBox->AddItem( name, i, NULL );
						m_profileEnabled[ i ] = true;
						if( profile->IsAModem() || profile->IsAUSBAdapter() || ( profile->IsPPPoE() && g_mainMenuSelection == ID_MAINMENU_SYSTEMLINK ) )
						{
							m_ListBox->EnableItem( i, false );
							m_profileEnabled[ i ] = false;
						}
					}
					m_MemCardState = NETWORKCONFIG_IDLE;
				}
				else
				{
					m_MemCardState = NETWORKCONFIG_IDLE;
					EndScreen( 0 );
				}	
			}
			break;

		default:
			break;
	}
#endif
}

//=============================================================================
// ScreenNetworkConfig::OnScreenupdate
//=============================================================================
void ScreenNetworkConfig::OnScreenUpdate( )
{
	MemCardState();

	GuiListBox *pListBox = GetListBox( );
	switch( m_state )
	{
		case STATE_NONE:
			if( pListBox )
			{
				int index = pListBox->CurrentSelection( );
				if( m_numConfigs == 0 || index == -1 )
				{
					SetNetworkProduct(0);
					SetNetworkVendor(0);
					SetNetworkType(0);
				}
				else
				{
					PS2ConnectionProfile *pProfile = g_MemCardManager->GetNetworkProfileList()->GetProfile( index );
					PS2ConnectionProfile::AdapterType type = pProfile->GetAdapterType( );

					const char* pProduct = 0;
					const char* pVendor = 0;
					const char* pType = 0;

					if( type <= PS2ConnectionProfile::Unknown )
					{
						if( g_regionUSA )
							pProduct = pProfile->GetProduct();
						else
							pProduct = "network adaptor (for PlayStation®2)";
						pVendor = pProfile->GetVendor();
						pType = AdapterTypes[type];
					}

					SetNetworkProduct( pProduct );
					SetNetworkVendor( pVendor );
					SetNetworkType( pType );
				}
			}
			break;

		case STATE_SHOW_DIALOG:
			if( g_timer.GetOSTime() > m_timer )
			{
				EndScreen( 0 );
				m_state = STATE_NONE;
			}
			break;

		// --- we're trying to start a connection
		case STATE_INITIATE_CONNECTION:
			if( CNetZConnection::IsConfigValid() || g_RemoteIPAddress != NULL )
			{
				if( !CNetZConnection::ConnectionUp() )
				{
					CNetZConnection::Configure();
				}
				m_state = STATE_NONE;
				GotoScreen( "SCREEN_ID_LANCONNECT" );
			}
			else
			{
				EndScreen(0);
			}
			break;

		case STATE_BACKUP:
			EndScreen(0);
			break;

	}
}

//=============================================================================
// ScreenNetworkConfig::GetListBox
//=============================================================================

GuiListBox* ScreenNetworkConfig::GetListBox( void )
{
	return (GuiListBox*) GetInterface( ID_LANCONFIG_LISTBOX, GuiListBox::ClassnameStatic() );
}

//=============================================================================
// ScreenNetworkConfig::SetNetworkField()
//=============================================================================

void ScreenNetworkConfig::SetNetworkField( int Id, const char* pString )
{
	int alpha = 255;

	if( pString == 0 || strlen(pString) == 0 )
		alpha = 0;

	Sprite* pSprite = GetSprite( Id );

	if( pSprite )
		pSprite->m_state.familycolor.a = alpha;

	Text* pText = GetTextSprite( ID_LANCONFIG_FIELD, pSprite );

	if( pText )
		pText->SetText( pString );

	return;
}

//=============================================================================
// ScreenNetworkConfig::SetNetworkProduct()
//=============================================================================

void ScreenNetworkConfig::SetNetworkProduct( const char* pString )
{
	SetNetworkField( ID_LANCONFIG_PRODUCT, pString );
}

//=============================================================================
// ScreenNetworkConfig::SetNetworkVendor()
//=============================================================================

void ScreenNetworkConfig::SetNetworkVendor( const char* pString )
{
	SetNetworkField( ID_LANCONFIG_VENDOR, pString );
}

//=============================================================================
// ScreenNetworkConfig::SetNetworkType()
//=============================================================================

void ScreenNetworkConfig::SetNetworkType( const char* pString )
{
	SetNetworkField( ID_LANCONFIG_TYPE, pString );
}


//=============================================================================
// ScreenNetworkConnect::OnInitialize()
//=============================================================================

void ScreenNetworkConnect::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();
}

//=============================================================================
// ScreenNetworkConnect::OnScreenIntro()
//=============================================================================

void ScreenNetworkConnect::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();

	Sprite* pPowerLed = GetSprite( ID_LANCONNECT_POWERLED );

	if( pPowerLed )
	{
		pPowerLed->InitAnim();

		SpriteAnim& anim = *pPowerLed->m_anim;

		anim.color.a.SetFrame( 0.000f, 1.0f );
		anim.color.a.SetFrame( 1.500f, 1.0f );
		anim.color.a.SetFrame( 1.501f, 0.3f );
		anim.color.a.SetFrame( 1.899f, 0.3f );
		anim.color.a.SetFrame( 2.000f, 1.0f );
		anim.color.a.Play( true );
	}

	m_LedAnim.SetFrame( 0.0f, 0.0f );
	m_LedAnim.SetFrame( 1.3f, 1.0f );
	m_LedAnim.TimeCurve( true );
	m_LedAnim.Play( true );

	m_state = STATE_NONE;
	ScreenNetworkError::SetNetworkError( ScreenNetworkError::ERROR_NONE );
	g_FrontEnd.ClearScreenHistory();
	return;
}

//=============================================================================
// ScreenNetworkConnect::OnScreenUpdate()
//=============================================================================

void ScreenNetworkConnect::OnScreenUpdate( void )
{
	ScreenBaseClass::OnScreenUpdate();

	Sprite* pRouterIcon = GetSprite( ID_LANCONNECT_ROUTERICON );
	Sprite* pLed;
	int i, n, c;
	int result;

	if( pRouterIcon )
	{
		pLed = pRouterIcon->Child( ID_LANCONNECT_LED );
		n = pRouterIcon->GetNumChildren( ID_LANCONNECT_LED );
		c = int(m_LedAnim.Value() * n);
	
		for( i = 0; pLed; i++ )
		{
			if( i <= c )
				pLed->m_state.color.a = 255;
			else
				pLed->m_state.color.a = 100;

			pLed = pLed->Next( ID_LANCONNECT_LED );
		}
	}

	switch( m_state )
	{
		case STATE_NONE:
			m_state = STATE_CONNECTING;
			m_time = g_timer.GetOSTime() + ( 10 * 1000 );
			break;

		// --- monitor connection status
		case STATE_CONNECTING:
			result = CNetZConnection::UpdateConnectionStatus( );
			if( result )
			{
				if( !CNetZConnection::GetLinkStatus() )
				{
					GotoScreen(SCREEN_MAINMENU);
					break;
				}
				GotoScreen(0);
				if( g_mainMenuSelection == ID_MAINMENU_ONLINE )
					MultiPlayerOnline();
				else if( g_mainMenuSelection == ID_MAINMENU_SYSTEMLINK )
					MultiPlayerLAN();
				m_state = STATE_DONE;
				break;
			}
			if( g_timer.GetOSTime() > m_time )
			{
				m_state = STATE_FAILED;
				break;
			}
			break;

		case STATE_FAILED:
			CNetZConnection::TearDown( );
			ScreenNetworkError::SetNetworkError( ScreenNetworkError::ERROR_CONNECTION_FAILED );
			GotoScreen( "SCREEN_ID_LANERROR" );
			break;

		case STATE_DONE:
			break;
	}
	return;
}

//=============================================================================
// ScreenNetworkConnect::OnButtonCancel()
//=============================================================================

void ScreenNetworkConnect::OnButtonCancel( void )
{
	// eat it, don't let the user go back
}


//=============================================================================
// ScreenNetworkConnect::OnButtonPressed()
//=============================================================================

void ScreenNetworkConnect::OnButtonPressed( int Button )
{
	switch( Button ) 
	{
	case FluffControlMapper::SELECT:
		if( m_Button1Pressed && !m_Button2Pressed )
		{
			m_Button2Pressed = true;

			ShowMessage( "Connecting, please wait..." );
		}

		break;

	case FluffControlMapper::R1:
		m_Button1Pressed = true;
		break;

	default:
		ScreenBaseClass::OnButtonPressed( Button );
		break;
	}

	return;
}

//=============================================================================
// ScreenNetworkConnect::ShowMessage
//=============================================================================

void ScreenNetworkConnect::ShowMessage( const char* pMessage )
{
	Text* pText = GetTextSprite( ID_LANCONNECT_HOOXPLOW );

	if( pText )
	{
		pText->InitAnim();
		
		SpriteAnim& anim = *pText->m_anim;

		anim.scale.x.SetFrame( 0.0f, 1.0f );
		anim.scale.x.SetFrame( 0.1f, 1.75f );
		anim.scale.x.SetFrame( 2.0f, 1.65f );
		anim.scale.x.SetFrame( 2.1f, 1.0f );
		anim.scale.x.Deccelerate( true );
		anim.scale.x.Play( false );

		anim.scale.y.SetFrame( 0.0f, 1.0f );
		anim.scale.y.SetFrame( 0.1f, 1.75f );
		anim.scale.y.SetFrame( 2.0f, 1.6f );
		anim.scale.y.SetFrame( 2.1f, 1.0f );
		anim.scale.y.Deccelerate( true );
		anim.scale.y.Play( false );
	}

	return;
}

//=============================================================================
// ScreenNetworkConnect:MultiPlayerLAN
//=============================================================================

void ScreenNetworkConnect::MultiPlayerLAN( void )
{
	// We are in single player
	g_configmgr.Initialize();
	g_configmgr.SetOptionByName("gameModes", "war");
	g_configmgr.AcceptChanges();
	g_messageDispatcher.SendMessageToAll( "NextLoop", (void*)"MultiPlayer_LAN", INVALID_OBJECT_HANDLE );
}

//=============================================================================
// ScreenNetworkConnect:MultiPlayerOnline
//=============================================================================
void ScreenNetworkConnect::MultiPlayerOnline( void )
{
	// We are in single player
	g_configmgr.Initialize();
	g_configmgr.SetOptionByName("gameModes", "war");
	g_configmgr.AcceptChanges();

	g_messageDispatcher.SendMessageToAll( "NextLoop", ( void* )"MultiPlayer_Internet", INVALID_OBJECT_HANDLE);
}
