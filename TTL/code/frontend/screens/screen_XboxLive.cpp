#include "TTLPCH.h"
#include "frontend/screens/screen_XboxLive.h"
#include "data/screens/mainmenu.h"

#ifdef _XBOX
#include "platform/Xbox/XboxLive.h"
#endif

void ScreenXboxLiveLogin::SetXboxLiveGamerTag( const char* pGamerTag )
{
	SetText( ID_LIVELOGIN_GAMERTAG, pGamerTag );
}

void ScreenXboxLiveLogin::SetXboxLiveStatusMessage( const char* pMessage )
{
	SetText( ID_LIVELOGIN_STATUS, pMessage );
}

void ScreenXboxLiveLogin::SetXboxLiveStatusMessagef( const char* fmt, ... )
{
	static char szPrintf[256];

	va_list args;
	va_start( args, fmt );
	vsprintf( szPrintf, fmt, args );
	va_end( args );

	SetXboxLiveStatusMessage( szPrintf );
}


void ScreenXboxLiveLogin::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	m_ScreenSizeShouldBe  = Sprite::SimulatedScreenHeight();
	m_ScreenSizeWantsToBe = Sprite::SimulatedScreenHeight();
}

void ScreenXboxLiveLogin::OnTerminate( void )
{
	float w = Sprite::SimulatedScreenWidth();
	float h = m_ScreenSizeWantsToBe;

	Sprite::SimulateScreenResolution( w, h );
}


void ScreenXboxLiveLogin::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();

	float w, h;
	float x1, y1;
	float x2, y2;

	w = Sprite::SimulatedScreenWidth();
	h = Sprite::SimulatedScreenHeight();

	Sprite* pScreen = GetScreenSprite();

	if( pScreen )
		pScreen->GetSpriteRect( x1, y1, x2, y2 );

	m_ScreenSizeAnim.Clear();
	m_ScreenSizeAnim.SetFrame( 0.000f, h );
	m_ScreenSizeAnim.NextFrame( 0.500f, y2 );
	m_ScreenSizeAnim.TimeCurve( true );
	m_ScreenSizeAnim.Play( false );
}

void ScreenXboxLiveLogin::OnScreenExit( void )
{
	//ScreenBaseClass::OnScreenExit();

	Sprite* pScreenSprite = GetScreenSprite();

	if( pScreenSprite )
	{
		pScreenSprite->InitAnim();

		SpriteAnim& anim = *pScreenSprite->m_anim;

		anim.familycolor.a.SetFrame( 0.000f, 1.0f );
		anim.familycolor.a.NextFrame( 1.499f, 1.0f );
		anim.familycolor.a.NextFrame( 1.500f, 0.0f );
		anim.familycolor.a.Play( false );
	}

	float w = Sprite::SimulatedScreenWidth();
	float h = Sprite::SimulatedScreenHeight();

	m_ScreenSizeAnim.Clear();
	m_ScreenSizeAnim.SetFrame( 0.000f, h );
	m_ScreenSizeAnim.NextFrame( 1.000f, h );
	m_ScreenSizeAnim.NextFrame( 0.500f, m_ScreenSizeWantsToBe );
	m_ScreenSizeAnim.TimeCurve( true );
	m_ScreenSizeAnim.Play( false );
}

void ScreenXboxLiveLogin::OnScreenRefresh( void )
{
	ScreenBaseClass::OnScreenRefresh();

#ifdef _XBOX
	switch( CXboxLive::GetLoginStatus() )
	{
	case CXBOXLIVE_NOT_SIGNED_IN:
		SetXboxLiveStatusMessage( "Xbox Live: Not Signed In" );
		SetXboxLiveGamerTag(0);
		break;

	case CXBOXLIVE_SIGNED_IN:
		SetXboxLiveStatusMessage( "Xbox Live: Signed In" );
		SetXboxLiveGamerTag( CXboxLive::GetUserName() );
		break;

	case CXBOXLIVE_PASSCODE_NEEDED:
		SetXboxLiveStatusMessage( "Xbox Live: Not Signed In: Passcode Needed" );
		SetXboxLiveGamerTag( CXboxLive::GetUserName() );
		break;

	case CXBOXLIVE_SIGNIN_FAILED:
		SetXboxLiveStatusMessage( "Xbox Live: Sign In Failed..." );
		SetXboxLiveGamerTag( CXboxLive::GetUserName() );
		break;

	case CXBOXLIVE_ETHERNET_DISCONNECTED:
		SetXboxLiveStatusMessage( "Xbox Live: Network Cable Disconnected..." );
		SetXboxLiveGamerTag( 0 );
		break;

	case	CXBOXLIVE_NO_NETWORK_CONNECTION:
		SetXboxLiveStatusMessage( "Xbox Live: No Network Connection..." );
		SetXboxLiveGamerTag( 0 );
		break;

	case CXBOXLIVE_SIGNIN_FAILED_UNKNOWN:
		SetXboxLiveStatusMessage( "Xbox Live: Sign In Failed..." );
		SetXboxLiveGamerTag( 0 );
		break;

	case	CXBOXLIVE_SIGNED_IN_MESSAGE_PENDING:
		SetXboxLiveStatusMessage( "Xbox Live: Account has Message Pending." );
		SetXboxLiveGamerTag( 0 );
		break;

	case	CXBOXLIVE_REQUIRES_MANAGEMENT:
		SetXboxLiveStatusMessage( "Xbox Live: Account Requires Management." );
		SetXboxLiveGamerTag( 0 );
		break;

	case	CXBOXLIVE_SERVERS_TOO_BUSY:
		SetXboxLiveStatusMessage( "Xbox Live: Servers are too busy." );
		SetXboxLiveGamerTag( 0 );
		break;

	case	CXBOXLIVE_CONNECTION_LOST:
		SetXboxLiveStatusMessage( "Xbox Live: Connection Lost." );
		SetXboxLiveGamerTag( 0 );
		break;

	}
#endif

	if( m_ScreenSizeShouldBe != Sprite::SimulatedScreenHeight() )
	{
		// someone is trying to change the simulated screen height
		// we must accomodate them

		m_ScreenSizeWantsToBe = Sprite::SimulatedScreenHeight();
	}

	float w = Sprite::SimulatedScreenWidth();
	float h = m_ScreenSizeAnim.Value();

	Sprite::SimulateScreenResolution( w, h );

	m_ScreenSizeShouldBe = Sprite::SimulatedScreenHeight();
}
