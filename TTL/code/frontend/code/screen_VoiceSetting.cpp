#include "TTLPCH.h"
#include "GameHelper/Translator.h"
#include "frontend/inputdriver.h"
#include "frontend/savegame.h"
#include "frontend/screens/screen_VoiceSetting.h"
#include "data/screens/NetworkGames.h"
#include "Voice/NetZ/CVoiceChat.h"

#define VSPARAM_PITCHWARP			(0x100)
#define VSPARAM_PITCHOFFSET		(0x101)
#define VSPARAM_SPECTRUMWARP		(0x102)
#define VSPARAM_SPECTRUMOFFSET	(0x103)
#define VSPARAM_SPECTRUMLEDGE		(0x104)
#define VSPARAM_ROBOT				(0x105)
#define VSPARAM_WHISPER				(0x106)
#define VSPARAM_ENVIRONMENT		(0x107)

//=============================================================================
// ScreenVoiceSetting::ScreenVoiceSetting
//=============================================================================

ScreenVoiceSetting::ScreenVoiceSetting( void )
{
}

//=============================================================================
// ScreenVoiceSetting::ChangeVoiceOutput
//=============================================================================

void ScreenVoiceSetting::ChangeVoiceOutput( int n )
{
#ifdef _XBOX
	static const int VoiceSettings[] = {VOICEOUTPUT_DISABLED, VOICEOUTPUT_HEADSET, VOICEOUTPUT_SPEAKERS};
#else
	static const int VoiceSettings[] = {VOICEOUTPUT_DISABLED, VOICEOUTPUT_HEADSET};
#endif

	int c = sizeof(VoiceSettings)/sizeof(VoiceSettings[0]);
	int i;

	for( i = 0; i < c; i++ )
	{
		if( VoiceSettings[i] == m_VoiceParams.VoiceOutput )
		{
			m_VoiceParams.VoiceOutput = VoiceSettings[((i + n) % c + c) % c];
			break;
		}
	}

	if( i == c )
		m_VoiceParams.VoiceOutput = VOICEOUTPUT_HEADSET;

	UpdateVoiceChatDevice();
}

//=============================================================================
// ScreenVoiceSetting::AdjustOutputVolume
//=============================================================================

void ScreenVoiceSetting::AdjustOutputVolume(	int n )
{
	m_VoiceParams.OutputVolume = Math::Clamp( m_VoiceParams.OutputVolume + n, 0, 100 );
	UpdateVoiceChatDevice();
}


//=============================================================================
// ScreenVoiceSetting::GetVoiceThruSpeakers
//=============================================================================

bool ScreenVoiceSetting::GetVoiceThruSpeakers( void )
{
	bool VoiceThruSpeakers = false;

	switch( m_VoiceParams.VoiceOutput )
	{
	case VOICEOUTPUT_HEADSET:
		VoiceThruSpeakers = false;
		break;
	case VOICEOUTPUT_SPEAKERS:
		VoiceThruSpeakers = true;
		break;
	}

	return VoiceThruSpeakers;
}

//=============================================================================
// ScreenVoiceSetting::UpdateVoiceChatDevice
//=============================================================================

void ScreenVoiceSetting::UpdateVoiceChatDevice( void )
{
	float Volumef = float(m_VoiceParams.OutputVolume) / 100.0f;

	g_CVoiceChat.channelVoiceThruSpeakers( GetVoiceThruSpeakers() );
	g_CVoiceChat.setOutputVolume( Volumef ); // floating point value zero to one
}

//=============================================================================
// ScreenVoiceSetting::GetProfileVoiceSetting
//=============================================================================

void ScreenVoiceSetting::GetProfileVoiceSettings( void )
{
	m_VoiceParams.VoiceOutput  = g_GlobalProfile.VoiceParams.VoiceOutput;
	m_VoiceParams.OutputVolume = g_GlobalProfile.VoiceParams.OutputVolume;

	UpdateVoiceChatDevice();
}

//=============================================================================
// ScreenVoiceSetting::SetProfileVoiceSetting
//=============================================================================

void ScreenVoiceSetting::SetProfileVoiceSettings( void )
{
	g_GlobalProfile.VoiceParams.VoiceOutput  = m_VoiceParams.VoiceOutput;
	g_GlobalProfile.VoiceParams.OutputVolume = m_VoiceParams.OutputVolume;

	UpdateVoiceChatDevice();
}

//=============================================================================
// ScreenVoiceSetting::OnInitialize
//=============================================================================

void ScreenVoiceSetting::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	UseHighlighting( true );

	SetInitialButton( ID_VOICESETTING_VOICEOUTPUT );
	SetNextButton( ID_VOICESETTING_OUTPUTVOLUME );
	SetNextButton( ID_VOICESETTING_DONE );
	SetCursor( ID_VOICESETTING_CURSOR );

	// prealloc to fix fragmentation
	Sprite* pScreenSprite = GetScreenSprite();
	if( pScreenSprite )
	{
		pScreenSprite->InitAnim();
	}
}

//=============================================================================
// ScreenVoiceSetting::OnCursorUp
//=============================================================================

void ScreenVoiceSetting::OnCursorUp( void )
{
	ScreenBaseClass::OnCursorUp();
}

//=============================================================================
// ScreenVoiceSetting::OnCursorDown
//=============================================================================

void ScreenVoiceSetting::OnCursorDown( void )
{
	ScreenBaseClass::OnCursorDown();
}

//=============================================================================
// ScreenVoiceSetting::OnCursorLeft
//=============================================================================

void ScreenVoiceSetting::OnCursorLeft( void )
{
	OnCursorIncrement( -1 );
}

//=============================================================================
// ScreenVoiceSetting::OnCursorRight
//=============================================================================

void ScreenVoiceSetting::OnCursorRight( void )
{
	OnCursorIncrement( 1 );
}

//=============================================================================
// ScreenVoiceSetting::OnCursorIncrement
//=============================================================================

void ScreenVoiceSetting::OnCursorIncrement( int N )
{
	int ButtonId = GetCurrentButtonId();

	switch( ButtonId )
	{
	case ID_VOICESETTING_VOICEOUTPUT:
		ChangeVoiceOutput(N);
		break;

	case ID_VOICESETTING_OUTPUTVOLUME:
		AdjustOutputVolume(N * 5);
		break;
	}

	return;
}

//=============================================================================
// ScreenVoiceSetting::OnButtonCancel
//=============================================================================

void ScreenVoiceSetting::OnButtonCancel( void )
{
	ScreenBaseClass::OnButtonCancel();
}

//=============================================================================
// ScreenVoiceSetting::OnButtonPressed
//=============================================================================

void ScreenVoiceSetting::OnButtonPressed( int Button )
{
	switch( Button )
	{
	case FluffControlMapper::SQUARE:
		OnCommandDefaultSetting();
		break;

	default:
		ScreenBaseClass::OnButtonPressed( Button );
		break;
	}

	return;
}

//=============================================================================
// ScreenVoiceSetting::OnButtonOk
//=============================================================================
#ifdef DIRECTX_PC
void ScreenVoiceSetting::OnButtonOK()
{
    OnCursorIncrement(1);

}
#endif

//=============================================================================
// ScreenVoiceSetting::OnCommand
//=============================================================================

void ScreenVoiceSetting::OnCommand( int Command )
{
	switch( Command )
	{
	case ID_VOICESETTING_VOICEOUTPUT:
		ChangeVoiceOutput(1);
		break;

	case ID_VOICESETTING_DONE:
		OnCommandDone();
		break;
	}

	return;
}

//=============================================================================
// ScreenVoiceSetting::OnCommandDefaultSetting
//=============================================================================

void ScreenVoiceSetting::OnCommandDefaultSetting( void )
{
	g_GlobalProfile.SetDefaultVoiceParams();
	GetProfileVoiceSettings();
}

//=============================================================================
// ScreenVoiceSetting::OnCommandDone
//=============================================================================

void ScreenVoiceSetting::OnCommandDone( void )
{
	// DAS, don't need to do this anymore since we are not doing voice masking
	//g_CVoiceChat.closeHeadSet( );
	SetProfileVoiceSettings();
	EndScreen(0);
}

//=============================================================================
// ScreenVoiceSetting::OnScreenIntro
//=============================================================================

void ScreenVoiceSetting::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();

	// --- open up our headset
	// DAS - Don't need to do this anymore, since we are not doing voice masking
	//if( !g_CVoiceChat.openHeadSet( ) ) ASSERTS( false, "Failed to open HeadSet" );

	GetProfileVoiceSettings();

	Printf( ID_VOICESETTING_CURRENTPROFILE, g_translator.Translate( "Current Profile: %s" ), g_playerProfile.GetAccountName() );

#ifdef DIRECTX_PC
	GetSprite(ID_VOICESETTING_HELPRECT)->HideAll();
#endif
}

//=============================================================================
// ScreenVoiceSetting::OnScreenUpdate
//=============================================================================

void ScreenVoiceSetting::OnScreenUpdate( void )
{
	ScreenBaseClass::OnScreenUpdate();

	const char* pVoiceOutput = 0;

	switch( m_VoiceParams.VoiceOutput )
	{
	case VOICEOUTPUT_DISABLED:
		pVoiceOutput = "Disabled";
		break;

	case VOICEOUTPUT_HEADSET:
		pVoiceOutput = "Headset";
		break;

	case VOICEOUTPUT_SPEAKERS:
		pVoiceOutput = "Speakers";
		break;
	}

	SetText( ID_VOICESETTING_SETTING1, pVoiceOutput );
	Printf( ID_VOICESETTING_SETTING2, "%d", m_VoiceParams.OutputVolume );
}
