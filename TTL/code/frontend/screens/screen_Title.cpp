#include "TTLPCH.h"
#include "main/TTL.h"
#include "frontend/savegame.h"
#include "frontend/inputdriver.h"
#include "frontend/texteffects.h"
#include "frontend/screens/screen_Title.h"
#include	"frontend\screens\screen_MemCard.h"
#include "data/screens/mainmenu.h"
#include "frontend/frontend.h"

#if defined( _XBOX )
#include "platform/Xbox/XboxLive.h"
#endif

#if defined (PS2)
#include "platform/MemoryCard/memcard_english_ps2.h"
#endif

#include "fmv/fmv.h"

static	bool	m_StartupCheck = false;
static	bool	m_SilentLogin = false;
bool	m_TitleScreenOn = false;

// --- how many seconds before attract mode kicks in
#define ATTRACT_TIMEOUT 30
const char *playList[] = { "%2/attract" };

//=============================================================================
// ScreenTitle::OnInitialize
//=============================================================================

void ScreenTitle::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	Sprite* pScreenSprite = GetScreenSprite();

	if( pScreenSprite )
		pScreenSprite->m_state.familycolor.a = 255;

	Text* pText = GetTextSprite( ID_TITLE_BUILDVERSION );

#if !defined (CONSUMER_BUILD)
	if( pText && g_ttl )
		pText->SetText( ::Printf( "Build %s", g_ttl->GetVersion() ) );
#endif

#ifdef DIRECTX_PC
	GetTextSprite(ID_TITLE_PRESSSTART1)->SetText("Click to Continue");

#endif

#if defined( CONSUMER_BUILD )
	if( pText )
	{
		pText->SetText( NULL );
		pText->Hide();
	}
#endif

	return;
}

//=============================================================================
// ScreenTitle::OnScreenIntro
//=============================================================================

void ScreenTitle::OnScreenIntro( void )
{
	m_TitleScreenOn = true;

	Sprite* pSprite = GetScreenSprite();

	if( pSprite )
	{
		pSprite->InitAnim();

		SpriteAnim& anim = *pSprite->m_anim;

		anim.familycolor.r.SetFrame( 0.000f, 0.0f );
		anim.familycolor.r.NextFrame( 0.600f, 0.0f );
		anim.familycolor.r.NextFrame( 0.300f, 1.0f );
		anim.familycolor.r.Play( false );

		anim.familycolor.g.SetFrame( 0.000f, 0.0f );
		anim.familycolor.g.NextFrame( 0.600f, 0.0f );
		anim.familycolor.g.NextFrame( 0.300f, 1.0f );
		anim.familycolor.g.Play( false );

		anim.familycolor.b.SetFrame( 0.000f, 0.0f );
		anim.familycolor.b.NextFrame( 0.600f, 0.0f );
		anim.familycolor.b.NextFrame( 0.300f, 1.0f );
		anim.familycolor.b.Play( false );

		float StartAlpha = pSprite->m_state.familycolor.a / 255.0f;
		anim.familycolor.a.SetFrame( 0.000f, StartAlpha );
		anim.familycolor.a.SetFrame( 0.300f, 1.0f );
		anim.familycolor.a.Play( false );
	}

	ScreenManager* pScreenManager = GetScreenManager();

	if( pScreenManager )
		pScreenManager->ClearScreenHistory();

	g_input.UnlockAllControllers();

	m_MemCardState = TITLE_IDLE;
	m_StartPressed = 0;

	Sprite* pPressStart = GetSprite( ID_TITLE_PRESSSTART );

	if( pPressStart )
	{
		pPressStart->InitAnim();
		pPressStart->Show();

		SpriteAnim& anim = *pPressStart->m_anim;

		anim.familycolor.a.SetFrame( 0.000f, 0.0f );
		anim.familycolor.a.SetFrame( 0.950f, 0.0f );
		anim.familycolor.a.SetFrame( 1.000f, 1.0f );
		anim.familycolor.a.NextFrame( 0.900f, 1.0f );
		anim.familycolor.a.NextFrame( 0.175f, 0.0f );
		anim.familycolor.a.NextFrame( 0.250f, 0.0f );
		anim.familycolor.a.NextFrame( 0.175f, 1.0f );
		anim.familycolor.a.SetLoopPoint( 1.0f );
		anim.familycolor.a.Play( true );
	}

	// --- Attract Mode setup
	m_attractModeTimer = g_timer.GetOSTime();
	m_attractState = ATTRACT_WAITING;
}

//=============================================================================
// ScreenTitle::OnScreenExit
//=============================================================================

void ScreenTitle::OnScreenExit( void )
{
	m_TitleScreenOn = false;

	Sprite* pSprite = GetScreenSprite();

	if( pSprite )
	{
		pSprite->InitAnim();
		pSprite->Show();

		SpriteAnim& anim = *pSprite->m_anim;

		float r, g, b, a;
		pSprite->GetFamilyColor( r, g, b, a );

		anim.familycolor.r.SetFrame( 0.000f, r );
		anim.familycolor.r.SetFrame( 0.300f, 0.0f );
		anim.familycolor.r.SetFrame( 0.800f, 0.0f );
		anim.familycolor.r.Play( false );

		anim.familycolor.g.SetFrame( 0.000f, g );
		anim.familycolor.g.SetFrame( 0.300f, 0.0f );
		anim.familycolor.g.SetFrame( 0.800f, 0.0f );
		anim.familycolor.g.Play( false );

		anim.familycolor.b.SetFrame( 0.000f, b );
		anim.familycolor.b.SetFrame( 0.300f, 0.0f );
		anim.familycolor.b.SetFrame( 0.800f, 0.0f );
		anim.familycolor.b.Play( false );

		anim.familycolor.a.SetFrame( 0.000f, a );
		anim.familycolor.a.SetFrame( 0.600f, a );
		anim.familycolor.a.SetFrame( 0.800f, 0.0f );
		anim.familycolor.a.Play( false );
	}

	return;
}

//=============================================================================
// ScreenHandler::OnScreenUpdate
//
//		Handle the memory card check as it goes through its different states.
//=============================================================================

void ScreenTitle::OnScreenUpdate(void)
{
	u32 curTime = g_timer.GetOSTime();
	u32 timeDelta = curTime - m_attractModeTimer;

	switch( m_attractState )
	{
		case ATTRACT_WAITING:
			if( timeDelta > ( ATTRACT_TIMEOUT * 1000 ) )
			{
				g_FrontEnd.ShowErrorMessage( false );
				g_FrontEnd.m_ControllerDisconnected = false;
				m_attractState = ATTRACT_RUNNING;
			}
			break;

		case ATTRACT_RUNNING:
		{
			g_audio.Pause();
			float saveSFXVol = g_audio.GetVolume(Audio::SFX );
			float saveMusicVol = g_audio.GetVolume(Audio::MUSIC );
			g_audio.SetVolume(Audio::MUSIC, 0.0f);
			g_audio.SetVolume(Audio::SFX, 1.0f);
			const int playCount = sizeof(playList) / sizeof(const char *);
			int which = g_random.UpToExcludeHigh( playCount );
			FMVPlayer::Play( playList[which], true);

			// needed to clear controller removed popup.. will get re-enabled later if
			// controller is in fact gone.
			g_ControllerRemoved = 0;
			g_FrontEnd.ShowErrorMessage( false );
			g_FrontEnd.m_ControllerDisconnected = false;
			if( g_input.ControllerLocked( 0 ) )
				g_inputPtr->GetController(0)->EatButtonPresses();
			g_audio.Unpause();
			g_audio.SetVolume(Audio::SFX, saveSFXVol);
			g_audio.SetVolume(Audio::MUSIC, saveMusicVol);
			m_attractModeTimer = g_timer.GetOSTime();
			m_attractState = ATTRACT_DEBOUNCE;
			break;
		}

		case ATTRACT_DEBOUNCE:
			if( g_input.ControllerLocked( 0 ) )
				g_inputPtr->GetController(0)->EatButtonPresses();
			m_attractState = ATTRACT_WAITING;
			break;
	}

	switch (m_MemCardState)
	{
		case	TITLE_IDLE:		
			break;

		case	TITLE_DETECT:
			g_MemCardManager->SetNoSave(false);			// reset it here.. that way it will re-check if they go back to title screen
			#if defined (PS2)
			g_MemCardManager->DetectCardOnStart(0);
			m_MemCardState = TITLE_DETECT_RESULT;
			#else
			g_MemCardManager->StartupCheck(0, sizeof(g_GlobalProfile), MC_SAVED_PROFILE);
			m_MemCardState = TITLE_DETECT_RESULT;
			#endif
			break;

		case	TITLE_DETECT_RESULT:		
			if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
			{
				if (g_MemCardManager->GetPopupExitResult() == PR_COMPLETED_OK)
				{
#ifdef DIRECTX_PC
				GotoScreen("SCREEN_ID_ONLINEPROFILE_PC");
#else
				GotoScreen("SCREEN_ID_ONLINEPROFILE");
#endif // DIRECTX_PC
					m_MemCardState = TITLE_IDLE;
				}

				int Result = g_MemCardManager->PopupResult();

				if( Result == MC_YES )
				{
#ifdef DIRECTX_PC
				GotoScreen("SCREEN_ID_ONLINEPROFILE_PC");
#else
				GotoScreen("SCREEN_ID_ONLINEPROFILE");
#endif // DIRECTX_PC
					m_MemCardState = TITLE_IDLE;
				}
				else
				{
					m_MemCardState = TITLE_IDLE;
#if !defined (_XBOX)
					m_StartPressed = 0;
#else
				GotoScreen("SCREEN_ID_ONLINEPROFILE");
#endif
				}
			}
			break;

		default:
			break;
	}
#ifdef DIRECTX_PC
	ScreenBaseClass::OnScreenUpdate();
#endif
}

//=============================================================================
// ScreenTitle::GetScreenIntroTime
//=============================================================================

float ScreenTitle::GetScreenIntroTime( void )
{
	return 1.0f;
}

//=============================================================================
// ScreenTitle::GetScreenExitTime
//=============================================================================

float ScreenTitle::GetScreenExitTime( void )
{
	return 0.8f;
}

//=============================================================================
// ScreenTitle::OnButtonStart
//=============================================================================

void ScreenTitle::OnButtonStart( void )
{
	// don't do anything if the startup memcard detect is in progress
	if( g_FrontEnd.m_MemCardState != STARTUPCHECK_IDLE )
		return;

	// --- eat the button press if we're in attract mode
	if( m_attractState == ATTRACT_RUNNING || m_attractState == ATTRACT_DEBOUNCE ) return;

#if defined (PS2)
	if (m_StartPressed == 0)
	{
		if (m_StartupCheck == true)
		{
			m_MemCardState = TITLE_DETECT;
			m_StartPressed = 1;
		}
		else
		{
#ifdef DIRECTX_PC
				GotoScreen("SCREEN_ID_ONLINEPROFILE_PC");
#else
				GotoScreen("SCREEN_ID_ONLINEPROFILE");
#endif // DIRECTX_PC
			m_StartupCheck = true;
		}
	}
#endif

#if defined (_XBOX)

	if (m_StartPressed == 0)
	{
		m_MemCardState = TITLE_DETECT;
		m_StartPressed = 1;
	}

	// start silent login for Xbox
	if (m_SilentLogin == false)
	{
		if (CXboxLive::GetAutoLogonStatus() == true)
		{
			if (CXboxLive::GetLoginState() == 0)
			{
				CXboxLive::SetSilentLogin(true);
				CXboxLive::LoginUIX();	
			}
			else
				CXboxLive::SetSilentLogin(false);
		}
		else
			CXboxLive::SetSilentLogin(false);
	}
	else
	{
		CXboxLive::SetSilentLogin(false);
	}

	if (m_SilentLogin == false)
		m_SilentLogin = true;
#endif

#define KEEPONWORKING
#if defined(KEEPONWORKING) && defined(WIN32) && !defined(_XBOX)
#ifdef DIRECTX_PC
				GotoScreen("SCREEN_ID_ONLINEPROFILE_PC");
#else
				GotoScreen("SCREEN_ID_ONLINEPROFILE");
#endif // DIRECTX_PC
#endif
}


//=============================================================================
// ScreenTitle::OnButtonOK
//=============================================================================

void ScreenTitle::OnButtonOK( void )
{
	OnButtonStart();
}

//=============================================================================
// ScreenTitle::OnButtonPressed
//=============================================================================

void ScreenTitle::OnButtonPressed( int Button )
{
	switch( Button )
	{
	case FluffControlMapper::SQUARE:
		break;

	default:
		ScreenBaseClass::OnButtonPressed( Button );
	}

	return;
}

//=============================================================================
// ScreenTitle::OnPopupShow
//=============================================================================

void ScreenTitle::OnPopupShow( void )
{
	ScreenBaseClass::OnPopupShow();
	ShowSprite( ID_TITLE_PRESSSTART, false );
}

//=============================================================================
// ScreenTitle::OnPopupHide
//=============================================================================

void ScreenTitle::OnPopupHide( void )
{
	ScreenBaseClass::OnPopupHide();
	ShowSprite( ID_TITLE_PRESSSTART, true );
}

#ifdef DIRECTX_PC
//=============================================================================
// ScreenTitle::OnButtonPressed
//=============================================================================

void ScreenTitle::OnButtonPressed( void )
{
	OnButtonStart();
}
#endif

 //=============================================================================
// ScreenHandler::HandleInput
//=============================================================================

void ScreenTitle::HandleInput( UIHandler* pHandler )
{
	if ( g_input.GetNumLockedControllers() == 0 )
	{
		bool gotInput = false;
		// check the start button on all 4 devices
		for ( uint i=0; i<g_input.Controllers(); i++ )
		{
			ControllerInputDriver *device = g_input.GetUnlockedController(i);
			if ( device->ButtonPressed( CB_START ) || device->ButtonPressed( CB_X ) )
			{
				gotInput = true;
				g_input.LockPlayerToController( 0, i );

				FluffControlMapper* pMapper = (FluffControlMapper*) g_controlFocus.Find("Fluff", 0);

				if( pMapper )
				{
					g_controlFocus.Remove( pMapper );

				//	delete pMapper;

					pMapper = new FluffControlMapper;

					if( pMapper )
					{
						g_controlFocus.Add( pMapper, 0 );
						g_controlFocus.Push( pMapper, 0 );
					}
				}
			}
		}
		if ( gotInput )
		{
			// remap the mappsers
			ScreenTitle::OnButtonStart();
		}
		else
		{
			ScreenBaseClass::HandleInput( pHandler );
		}
	}
	else
	{
		ScreenBaseClass::HandleInput( pHandler );
	}

	return;
}