#include "TTLPCH.h"
#include "frontend/frontend.h"
#include "frontend/inputdriver.h"
#include "frontend/screens/screen_Popups.h"
#include "data/screens/OnlineScreens.h"
#include "frontend/screens/screen_NetworkError.h"
#include "platform/MemoryCard/MemoryCard.h"
#include "data/screens/mainmenu.h"

#ifdef _XBOX
#include "platform/xbox/XboxLive.h"
#endif

#include "gamehelper/translator.h"

ScreenNetworkError::eErrorCondition ScreenNetworkError::m_error = ERROR_NONE;
extern int g_mainMenuSelection;

//=============================================================================
// ScreenNetworkError::OnInitialize()
//=============================================================================

void ScreenNetworkError::OnInitialize( void )
{
	m_error = ERROR_NONE;

	// Hide PS2 buttons on PC
#ifdef DIRECTX_PC
	GetSprite(ID_LANERROR_HELPICON)->HideAll();
#endif

#ifdef _XBOX
	GetSprite(ID_LANERROR_HELPICON)->HideAll();
	GetSprite(ID_LANERROR_HELPTEXT)->HideAll();
#endif

	ScreenBaseClass::OnInitialize();
}

//=============================================================================
// ScreenNetworkError::SetErrorMessage
//=============================================================================

void ScreenNetworkError::SetErrorMessage( const char* pErrorMessage )
{
	SetText( ID_LANERROR_MESSAGE, pErrorMessage );
}

//=============================================================================
// ScreenNetworkError::OnButtonOK
//=============================================================================

void ScreenNetworkError::OnButtonOK( void )
{
#if defined( _XBOX )
	if( g_mainMenuSelection == ID_MAINMENU_ONLINE )
		CXboxLive::GotoDashboard( CXBOXLIVE_NETWORK_CONFIGURATION );
#endif
	GotoScreen(SCREEN_MAINMENU);
}

//=============================================================================
// ScreenNetworkError::OnButtonCancel
//=============================================================================

void ScreenNetworkError::OnButtonCancel( void )
{
#if defined( _XBOX )
	if( g_mainMenuSelection == ID_MAINMENU_ONLINE )
		GotoScreen(SCREEN_MAINMENU);
#else
	// eat it!
#endif
}

//=============================================================================
// ScreenNetworkError::OnScreenUpdate
//=============================================================================

void ScreenNetworkError::OnScreenUpdate( void )
{
	ScreenBaseClass::OnScreenUpdate();
	switch( m_error )
	{
	case ERROR_NONE:
		SetErrorMessage( NULL );
		break;

	case ERROR_CONNECTION_FAILED:
		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
		{
#if defined( PS2 )
			char tempBuf[ 256 ];
			const char *pConnectionError = CNetZConnection::GetConnectionErrorMessage( );
			if( pConnectionError && *pConnectionError )
				sprintf( tempBuf, "%s.\n%s", pConnectionError, "Could not access the external network device. Please check all connections and reconnect." );
			else
				sprintf( tempBuf, "%s", "Could not access the external network device. Please check all connections and reconnect." );
			SetErrorMessage( tempBuf );

#elif defined( _XBOX )
			if( g_mainMenuSelection == ID_MAINMENU_ONLINE )
			{
				SetErrorMessage( ::Printf( g_translator.Translate( "Your Xbox console cannot connect to Xbox Live. Press %c to start the troubleshooter or %c to cancel." ), 0x11, 0x18 ) );
			}
			else if( g_mainMenuSelection == ID_MAINMENU_SYSTEMLINK )
			{
				SetErrorMessage( ::Printf( g_translator.Translate( "The system link cable might be disconnected. Please check the cable and press %c to continue." ), 0x11 ) );
			}
#else
			SetErrorMessage( "Could not access the external network device. Please check all connections and reconnect." );
#endif
		}
		else
			SetErrorMessage( NULL );
		break;
	}
}
