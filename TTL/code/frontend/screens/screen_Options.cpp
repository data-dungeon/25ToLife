#include "TTLPCH.h"
#include "main/TTL.h"

#include "platform/defcontrolmap.h"
#include "platform/MemoryCard/MemoryCard.h"
#include "Display/TextureUtil.h"

#include "frontend/frontend.h"
#include "frontend/savegame.h"
#include "frontend/screens/screen_Options.h"
#include "frontend/screens/screen_Popups.h"
#include "data/screens/pause.h"
#include "main/PlayerDB.h"

#ifdef DIRECTX_PC
#include "data/screens/pcscreens.h"
#include "display/directx/platform.cpp"
#else
#ifdef _XBOX
#include "data_xbox/screens/Options.h"
#else
#include "data/screens/Options.h"
#endif
#endif


// some platforms don't have a gamma option
#ifndef ID_OPTIONS_GAMMA	
#define ID_OPTIONS_GAMMA (0)
#endif

#ifndef ID_OPTIONS_GRADIENTBOX
#define ID_OPTIONS_GRADIENTBOX (0)
#endif

#ifndef ID_OPTIONS_GRADIANT
#define ID_OPTIONS_GRADIANT (0)
#endif

bool	g_OptionsOnScreen = false;

//=============================================================================
// ScreenOptions::ScreenOptions - The default constructor
//=============================================================================

ScreenOptions::ScreenOptions( void )
{
	m_InvertedAiming = false;
	m_VibrationEnabled = true;
	m_LookSensitivity = 3;
	m_LookSpeed = 3;
#ifdef DIRECTX_PC
	m_StereoMode = STEREOMODE_STEREO;
	m_BlurFX = false;
#else
	m_StereoMode = true;
#endif
	m_SoundVolume = 1.0f;
	m_MusicVolume = 1.0f;
	m_GammaValue = PROFILE_DEFAULT_GAMMA;
	m_Blood = true;
	m_KeepSettings = false;
}

//=============================================================================
// ScreenOptions::~ScreenOptions - The default destructor
//=============================================================================

ScreenOptions::~ScreenOptions( void )
{
}

//=============================================================================
// ScreenOptions::ToggleAiming
//=============================================================================

void ScreenOptions::ToggleAiming( void )
{
	m_InvertedAiming = !m_InvertedAiming;

	PlaySoundEvent( Screen::SOUNDEVENT_INVALID );
}

//=============================================================================
// ScreenOptions::ToggleVibration
//=============================================================================

void ScreenOptions::ToggleVibration( void )
{
	m_VibrationEnabled = !m_VibrationEnabled;

	ControllerInputDriver *c = NULL;
	if ( g_input.ControllerLocked(0) )
		c = g_input.GetController(0);


	if (m_VibrationEnabled)
	{
		uint	intensity;
		if (c)
		{
			c->Vibration(m_VibrationEnabled);
	
			if (c->GetVibration(intensity) == false)
				c->SetVibrationDuration(20,c->RUMBLEHIGH);
		}
	}
	else
	{
		if (c)
		{
			c->Vibration(m_VibrationEnabled);
			c->ClearVibration(true);
		}
	}


	PlaySoundEvent( Screen::SOUNDEVENT_INVALID );
}

//=============================================================================
// ScreenOptions::IncreaseLookSensitivity
//=============================================================================

void ScreenOptions::IncreaseLookSensitivity( int N )
{
	int OldValue = m_LookSensitivity;

#ifdef DIRECTX_PC
	m_LookSensitivity = Math::Clamp ( (m_LookSensitivity + N ), 1, 10);
	if (m_LookSensitivity == 0) m_LookSensitivity = 1;
#else
	m_LookSensitivity = Math::Clamp ((m_LookSensitivity + N), 1, 10);
#endif
	if( m_LookSensitivity != OldValue )
		PlaySoundEvent( Screen::SOUNDEVENT_INVALID );

	return;
}

//=============================================================================
// ScreenOptions::IncreaseLookSpeed
//=============================================================================

void ScreenOptions::IncreaseLookSpeed( int N )
{
	int OldValue = m_LookSpeed;
#ifdef DIRECTX_PC
	m_LookSpeed = Math::Clamp ( (m_LookSpeed + N ), 1, 10);
	if (m_LookSpeed == 0) m_LookSpeed = 1;
#else
	m_LookSpeed =  Math::Clamp ((m_LookSpeed+N),1,10);
#endif
	if( m_LookSpeed != OldValue )
		PlaySoundEvent( Screen::SOUNDEVENT_INVALID );

	return;
}


//=============================================================================
// ScreenOptions::ToggleStereoMode
//=============================================================================

void ScreenOptions::ToggleStereoMode( void )
{
#ifdef DIRECTX_PC
	m_StereoMode = (++m_StereoMode)%3;
#else
	m_StereoMode = !m_StereoMode;
#endif
	PlaySoundEvent( Screen::SOUNDEVENT_INVALID );
}

//=============================================================================
// ScreenOptions::IncreaseSound
//=============================================================================

void ScreenOptions::IncreaseSound( int Steps )
{
	float OneStep = 1.0f / GetMeterCount( ID_OPTIONS_SOUND );
	float OldVolume = m_SoundVolume;
#ifdef DIRECTX_PC
	m_SoundVolume += OneStep * Steps;
	if (m_SoundVolume >= 1.0f + OneStep) m_SoundVolume = 0.0f;
	else if (m_SoundVolume < 0.0f) m_SoundVolume = 1.0f;
#else
	m_SoundVolume = Math::Clamp( m_SoundVolume + OneStep * Steps );
#endif
	if( m_SoundVolume != OldVolume )
	{
		SetSoundVolume( m_SoundVolume );

		if( Steps < 0 )
			PlaySoundEvent( Screen::SOUNDEVENT_PREVBUTTON );
		else
			PlaySoundEvent( Screen::SOUNDEVENT_NEXTBUTTON );
	}

	return;
}

//=============================================================================
// ScreenOptions::IncreaseMusic
//=============================================================================

void ScreenOptions::IncreaseMusic( int Steps )
{
	float OneStep = 1.0f / GetMeterCount( ID_OPTIONS_MUSIC );
	float OldVolume = m_MusicVolume;

#ifdef DIRECTX_PC
	m_MusicVolume += OneStep * Steps;
	if (m_MusicVolume >= 1.0f + OneStep) m_MusicVolume = 0.0f;
	else if (m_MusicVolume < 0.0f) m_MusicVolume = 1.0f;
#else
	m_MusicVolume = Math::Clamp( m_MusicVolume + OneStep * Steps );
#endif

	if( m_MusicVolume != OldVolume )
	{
		SetMusicVolume( m_MusicVolume );

		if( Steps < 0 )
			PlaySoundEvent( Screen::SOUNDEVENT_PREVBUTTON );
		else
			PlaySoundEvent( Screen::SOUNDEVENT_NEXTBUTTON );
	}

	return;
}

//=============================================================================
// ScreenOptions::GetGammaZeroToOne
//=============================================================================

float ScreenOptions::GetGammaZeroToOne()
{
	return TextureUtil::GetGammaZeroToOne();
}

//=============================================================================
// ScreenOptions::IncreaseGamma
//=============================================================================

void ScreenOptions::IncreaseGamma( int Steps )
{
	float OneStep = 1.0f / GetMeterCount( ID_OPTIONS_GAMMA );

	SetGammaValue( m_GammaValue + OneStep * Steps );
	ShowGammaBar( true );
}

//=============================================================================
// ScreenOptions::ToggleBlood
//=============================================================================

void ScreenOptions::ToggleBlood( void )
{
	m_Blood = !m_Blood;

	PlaySoundEvent( Screen::SOUNDEVENT_INVALID );
}

//=============================================================================
// ScreenOptions::SetSoundVolume
//=============================================================================

void ScreenOptions::SetSoundVolume( float Volume )
{
	g_ttl->SetSoundVolume( Volume, false );
}

//=============================================================================
// ScreenOptions::SetMusicVolume
//=============================================================================

void ScreenOptions::SetMusicVolume( float Volume )
{
	g_ttl->SetMusicVolume( Volume, false );
}

//=============================================================================
// ScreenOptions::GetSoundVol
//=============================================================================

float ScreenOptions::GetSoundVolume( void )
{
	return g_ttl->GetSoundVolume();
}

//=============================================================================
// ScreenOptions::GetMusicVol
//=============================================================================

float ScreenOptions::GetMusicVolume( void )
{
	return g_ttl->GetMusicVolume();
}

//=============================================================================
// ScreenOptions::GetMeterCount
//=============================================================================

int ScreenOptions::GetMeterCount( int SpriteId )
{
	return GetMeterCount( GetSprite( SpriteId ) );
}

//=============================================================================
// ScreenOptions::GetMeterCount
//=============================================================================

int ScreenOptions::GetMeterCount( Sprite* pSprite )
{
	Sprite* pChild;
	int Count = 0;

	if( pSprite )
	{
		pChild = pSprite->Child();

		while( pChild )
		{
			if( pChild->ObjectId() == ID_OPTIONS_METER )
				Count++;

			pChild = pChild->Next();
		}
	}

	return Count;
}

//=============================================================================
// ScreenOptions::InitMeters
//=============================================================================

void ScreenOptions::InitMeter( int SpriteId )
{
	Sprite* pSprite = GetSprite( SpriteId );
	Sprite* pThisMeter;
	Sprite* pNextMeter;
	float x1, y1;
	float x2, y2;

	if( pSprite )
	{
		pThisMeter = pSprite->Child( ID_OPTIONS_METER );

		if( pThisMeter )
		{
			pNextMeter = pThisMeter->Next( ID_OPTIONS_METER );

			while( pNextMeter )
			{
				pThisMeter->GetPosition( x1, y1 );
				pNextMeter->GetPosition( x2, y2 );

				if( x1 > x2 )
				{
					pThisMeter->Insert( pNextMeter );

					if( pNextMeter->Prev() )
						pThisMeter = pNextMeter->Prev();
					else
						pThisMeter = pNextMeter;

					pNextMeter = pThisMeter->Next();
				}
				else
				{
					pThisMeter = pNextMeter;
					pNextMeter = pNextMeter->Next( ID_OPTIONS_METER );
				}
			}
		}
	}

	return;
}

//=============================================================================
// ScreenOptions::InitAllMeters
//=============================================================================

void ScreenOptions::InitAllMeters( void )
{
	InitMeter( ID_OPTIONS_SOUND );
	InitMeter( ID_OPTIONS_MUSIC );
	InitMeter( ID_OPTIONS_GAMMA );
}

//=============================================================================
// ScreenOptions::UpdateMeter
//=============================================================================

void ScreenOptions::UpdateMeter( int SpriteId, float ZeroToOne )
{
	UpdateMeter( GetSprite( SpriteId ), ZeroToOne );
}

//=============================================================================
// ScreenOptions::UpdateMeter
//=============================================================================

void ScreenOptions::UpdateMeter( Sprite* pSprite, float ZeroToOne )
{
	if( pSprite )
	{
		int n = GetMeterCount( pSprite );
		int c = (int) (n * ZeroToOne + 0.5f);
		int i;

		Sprite* pChild = pSprite->Child( ID_OPTIONS_METER );

		for( i = 0; i < c; i++ )
		{
			if( pChild )
			{
				pChild->m_state.familycolor.a = 255;
				pChild = pChild->Next( ID_OPTIONS_METER );
			}
		}

		for( i = c; i < n; i++ )
		{
			if( pChild )
			{
				pChild->m_state.familycolor.a = 100;
				pChild = pChild->Next( ID_OPTIONS_METER );
			}
		}
	}

	return;
}

//=============================================================================
// ScreenOptions::UpdateAllMeters
//=============================================================================

void ScreenOptions::UpdateAllMeters( void )
{
	UpdateMeter( ID_OPTIONS_SOUND, m_SoundVolume );
	UpdateMeter( ID_OPTIONS_MUSIC, m_MusicVolume );
	UpdateMeter( ID_OPTIONS_GAMMA, m_GammaValue );
}

void ScreenOptions::SetGammaValue( float Gamma )
{
	float OldGamma = m_GammaValue;
	m_GammaValue = Math::Clamp( Gamma );

	if( m_GammaValue != OldGamma )
		MakeGammaTakeEffect();

	return;
}

void ScreenOptions::ShowGammaBar( bool Show )
{
	Sprite* pGradient = GetSprite( ID_OPTIONS_GRADIENTBOX );

	if( pGradient )
	{
		if( Show )
			pGradient->m_state.familycolor.a = 255;
		else
			pGradient->m_state.familycolor.a = 0;
	}

	return;
}

//=============================================================================
// ScreenOptions::MakeGammaTakeEffect
//=============================================================================

void ScreenOptions::MakeGammaTakeEffect( void )
{
	bool Handled = false;

#if defined (_XBOX)
	TextureUtil::SetGammaZeroToOne( m_GammaValue );
	TextureUtil::UpdateGamma();
	Handled = true;
#endif

#ifdef DIRECTX_PC
	if (m_GammaValue > 1.0f) m_GammaValue = 0.0f;
	if (m_GammaValue < 0.0f) m_GammaValue = 1.0f;
	TextureUtil::SetGammaZeroToOne( m_GammaValue );
	TextureUtil::UpdateGamma();
	Handled = true;
#endif

#if defined (PS2)
	// update the graphic on screen for PS2 so we know what we are changing
	// this works on all platforms.. textures get modified at load time
	TextureUtil::SetGammaZeroToOne( m_GammaValue );
	TextureUtil::SetBlendMode(TEXTUREUTIL_GAMMA);
	TextureUtil::UpdateGamma();
	// delete origtexture
	m_GammaImage->Clone(&m_NewGammaImage);
	
	if ( ( m_NewGammaImage->GetFormat() & TYPE_MASK ) == TYPE_RGB || ( m_NewGammaImage->GetFormat() & TYPE_MASK ) == TYPE_RGBA )
		TextureUtil::BlitColor( m_NewGammaImage, 0, 0, m_NewGammaImage->GetWidth(), m_NewGammaImage->GetHeight() );
	else
		TextureUtil::FastGammaAdjust(m_NewGammaImage);

	// replace the old texture
	Image*	TempImage = m_GammaTexPtr->GetImage();
	delete	TempImage;
	m_NewGammaImage->Clone(&TempImage);
	delete m_NewGammaImage;
	m_NewGammaImage = NULL;
	m_GammaTexPtr->SetImage(TempImage);
	Handled = true;
#endif

	//==========================================================================
	// Each platform should set this value to true when it successfully updates
	// the gamma.  If this value is false by the time we get here, it means we
	// haven't done anything to successfully change the gamma so I'll just ask
	// the texture util class to do what it can, if anything...
	//==========================================================================

	if( !Handled )
	{
		TextureUtil::SetGammaZeroToOne( m_GammaValue );
		TextureUtil::UpdateGamma();
	}

	return;
}

//=============================================================================
// ScreenOptions::GetAvatarMapper
//=============================================================================

AvatarControlMapper* ScreenOptions::GetAvatarMapper( void )
{
	return (AvatarControlMapper*) g_controlFocus.Find( "Avatar" );
}

//=============================================================================
// ScreenOptions::OnInitialize
//=============================================================================

void ScreenOptions::OnInitialize( void )
{
	g_OptionsOnScreen = false;

	ScreenBaseClass::OnInitialize();

	UseHighlighting( true );
#ifndef DIRECTX_PC
	SetHelpRect( ID_OPTIONS_HELPRECT );
#endif

#ifdef DIRECTX_PC
	SetInitialButton( ID_OPTIONS_AIMING );
#else
	SetInitialButton( ID_OPTIONS_BUTTONLAYOUT );
	SetNextButton( ID_OPTIONS_THUMBSTICKSETTINGS );
	SetNextButton( ID_OPTIONS_AIMING );
	SetNextButton( ID_OPTIONS_VIBRATION );
#endif
	SetNextButton( ID_OPTIONS_LOOKSENSITIVITY );
	SetNextButton( ID_OPTIONS_LOOKSPEED );
#ifndef _XBOX
	SetNextButton( ID_OPTIONS_STEREO );
#endif
	SetNextButton( ID_OPTIONS_SOUND );
	SetNextButton( ID_OPTIONS_MUSIC );
	SetNextButton( ID_OPTIONS_GAMMA );
	SetNextButton( ID_OPTIONS_BLOOD );
#ifdef DIRECTX_PC
	SetNextButton( ID_OPTIONS_BLURFX );
	SetNextButton( ID_OPTIONS_KEYBINDINGS );
#endif
	SetNextButton( ID_OPTIONS_DONE );
	SetCursor( ID_OPTIONS_CURSOR );

#ifndef DIRECTX_PC
	SetNextScreen( ID_OPTIONS_BUTTONLAYOUT, "SCREEN_ID_BUTTONMAP" );
	SetNextScreen( ID_OPTIONS_THUMBSTICKSETTINGS, "SCREEN_ID_THUMBSTICKS" );
#endif

	InitAllMeters();

	// Initialize the gamma texture
	m_GammaTexHandle = INVALID_HANDLE;
	m_GammaTexDatabase = NULL;
	m_GammaTexPtr = NULL;
	m_GammaImage = NULL;
	m_NewGammaImage = NULL;

	// make a copy of the texture for gamma changes
	Sprite* pSprite = GetSprite( ID_OPTIONS_GRADIANT );

	if( pSprite )
	{
		m_GammaTexHandle = pSprite->GetTextureSet();

		#if defined (_XBOX) || defined (PS2)
		if( m_GammaTexHandle != INVALID_HANDLE )
		{
			m_GammaTexDatabase  = (TextureDatabase *) dblGetChunkData(DB_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, m_GammaTexHandle);
			m_GammaTexPtr = m_GammaTexDatabase->GetTexture(0);
			Image* OrigImage = m_GammaTexPtr->GetImage();
			OrigImage->Clone(&m_GammaImage);
		}
		#endif
	}

	// Hide PS2 help bar on PC
#ifdef DIRECTX_PC
	//GetSprite(ID_OPTIONS_HELPGROUP)->HideAll();
#endif

	m_KeepSettings = false;
}

//=============================================================================
// ScreenOptions::OnTerminate
//=============================================================================

void ScreenOptions::OnTerminate( void )
{
	if (m_GammaImage)
	{
		delete m_GammaImage;
		m_GammaImage = NULL;
	}
	return;
}

//=============================================================================
// ScreenOptions::OnScreenIntro
//=============================================================================

void ScreenOptions::OnScreenIntro( void )
{
	g_OptionsOnScreen = true;

	ScreenBaseClass::OnScreenIntro();

	if( m_KeepSettings )
		m_KeepSettings = false;
	else
	{
		m_InvertedAiming   = g_GlobalProfile.GetInvertedAiming();
		m_VibrationEnabled = g_GlobalProfile.GetVibration();
		m_LookSensitivity	 = g_GlobalProfile.GetLookSensitivity();
		m_LookSpeed			 = g_GlobalProfile.GetLookSpeed();
		m_StereoMode       = g_GlobalProfile.GetStereoMode();
		m_SoundVolume      = g_GlobalProfile.GetSoundVolume();
		m_MusicVolume      = g_GlobalProfile.GetMusicVolume();
		m_GammaValue       = g_GlobalProfile.GetGammaValue();
		m_Blood				 = g_GlobalProfile.GetBlood();
	#ifdef DIRECTX_PC
		m_BlurFX           = g_GlobalProfile.GetBlurFX();
		// show our back button
		BackButton.Show(true);
		g_MenuCursor.SetHidden(false);
	#endif

		// enable/disable gamma on load
		if ( Math::Equal(m_GammaValue,PROFILE_DEFAULT_GAMMA))
			TextureUtil::SetGammaOnLoadFlag(false);
		else
			TextureUtil::SetGammaOnLoadFlag(true);
	}

	Sprite* pSprite = GetScreenSprite();

	if( pSprite && pSprite->m_anim )
	{
		SpriteAnim& anim = *pSprite->m_anim;

		anim.familycolor.r.SetFrame( 0.000f, 0.0f );
		anim.familycolor.r.NextFrame( 0.600f, 0.0f );
		anim.familycolor.r.NextFrame( 0.200f, 1.0f );
		anim.familycolor.r.Play( false );

		anim.familycolor.g.SetFrame( 0.000f, 0.0f );
		anim.familycolor.g.NextFrame( 0.600f, 0.0f );
		anim.familycolor.g.NextFrame( 0.200f, 1.0f );
		anim.familycolor.g.Play( false );

		anim.familycolor.b.SetFrame( 0.000f, 0.0f );
		anim.familycolor.b.NextFrame( 0.600f, 0.0f );
		anim.familycolor.b.NextFrame( 0.200f, 1.0f );
		anim.familycolor.b.Play( false );

		anim.familycolor.a.SetFrame( 0.000f, 0.0f );
		anim.familycolor.a.NextFrame( 0.200f, 1.0f );
		anim.familycolor.a.Play( false );
	}
#ifdef DIRECTX_PC
	if ( UserSettings.Fullscreen != true )
		GetSprite(ID_OPTIONS_GAMMA)->SetAlpha(0.2);
#endif

	return;
}


//=============================================================================
// ScreenOptions::OnScreenExit
//=============================================================================

void ScreenOptions::OnScreenExit( void )
{
	g_OptionsOnScreen = false;

	ScreenBaseClass::OnScreenExit();

	Sprite* pSprite = GetScreenSprite();

	if( pSprite )
	{
		pSprite->InitAnim();

		float Red;
		float Green;
		float Blue;
		float Alpha;

		pSprite->GetFamilyColor( Red, Green, Blue, Alpha );

		SpriteAnim& anim = *pSprite->m_anim;

		anim.familycolor.r.SetFrame( 0.000f, Red );
		anim.familycolor.r.NextFrame( 0.200f, 0.0f );
		anim.familycolor.r.Play( false );

		anim.familycolor.g.SetFrame( 0.000f, Green );
		anim.familycolor.g.NextFrame( 0.200f, 0.0f );
		anim.familycolor.g.Play( false );

		anim.familycolor.b.SetFrame( 0.000f, Blue );
		anim.familycolor.b.NextFrame( 0.200f, 0.0f );
		anim.familycolor.b.Play( false );

		anim.familycolor.a.SetFrame( 0.000f, Alpha );
		anim.familycolor.a.NextFrame( 0.600f, Alpha );
		anim.familycolor.a.NextFrame( 0.200f, 0.0f );
		anim.familycolor.a.Play( false );
	}

	ShowGammaBar( false );
}

//=============================================================================
// ScreenOptions::OnCursorLeft
//=============================================================================

void ScreenOptions::OnCursorLeft( void )
{
	OnChangeOption( GetCurrentButtonId(),-1 );
}

//=============================================================================
// ScreenOptions::OnCursorRight
//=============================================================================

void ScreenOptions::OnCursorRight( void )
{
	OnChangeOption( GetCurrentButtonId(), 1 );
}

//=============================================================================
// ScreenOptions::OnButtonOk
//=============================================================================
#ifdef DIRECTX_PC
void ScreenOptions::OnButtonOK()
{
	OnChangeOption( GetCurrentButtonId(), 1);
}
#endif

//=============================================================================
// ScreenOptions::OnChangeOption
//=============================================================================

void ScreenOptions::OnChangeOption( int Option, int N )
{
	switch( Option )
	{
	case ID_OPTIONS_AIMING:
		ToggleAiming();
		break;
#ifndef DIRECTX_PC
	case ID_OPTIONS_VIBRATION:
		ToggleVibration();
		break;
#endif
	case ID_OPTIONS_LOOKSENSITIVITY:
		IncreaseLookSensitivity(N);
		break;
	case ID_OPTIONS_LOOKSPEED:
		IncreaseLookSpeed(N);
		break;
#ifndef _XBOX
	case ID_OPTIONS_STEREO:
		ToggleStereoMode();
		break;
#endif
	case ID_OPTIONS_SOUND:
		IncreaseSound(N);
		break;
	case ID_OPTIONS_MUSIC:
		IncreaseMusic(N);
		break;
	case ID_OPTIONS_GAMMA:
#ifdef DIRECTX_PC
		if ( UserSettings.Fullscreen == true )
#endif
		IncreaseGamma(N);
		break;
	case ID_OPTIONS_BLOOD:
		ToggleBlood();
		break;
#ifdef DIRECTX_PC
	case ID_OPTIONS_KEYBINDINGS:
		g_FrontEnd.GotoScreen("SCREEN_ID_PCKEYBINDINGS");
		break;

	case ID_OPTIONS_BLURFX:
		// TODO: Toggle blur effects here
		m_BlurFX = !m_BlurFX;
		PlaySoundEvent( Screen::SOUNDEVENT_INVALID );
		break;

	case ID_OPTIONS_DONE:
		OnCommandDone();
		break;
#endif
	}

	return;
}

//=============================================================================
// ScreenOptions::OnButtonCancel
//=============================================================================

void ScreenOptions::OnButtonCancel( void )
{
	ScreenBaseClass::OnButtonCancel();

	SetSoundVolume( g_GlobalProfile.GetSoundVolume() );
	SetMusicVolume( g_GlobalProfile.GetMusicVolume() );
	SetGammaValue( g_GlobalProfile.GetGammaValue() );
}

//=============================================================================
// ScreenOptions::OnButtonSelect
//=============================================================================

void ScreenOptions::OnButtonSelect( Button* pButton, Sprite* pSprite )
{
	ScreenBaseClass::OnButtonSelect( pButton, pSprite );

	ShowGammaBar( false );
}

//=============================================================================
// ScreenOptions::OnButtonUnselect
//=============================================================================

void ScreenOptions::OnButtonUnselect( Button* pButton, Sprite* pSprite )
{
	ScreenBaseClass::OnButtonUnselect( pButton, pSprite );
}

//=============================================================================
// ScreenOptions::OnScreenUpdate
//=============================================================================

void ScreenOptions::OnScreenUpdate( void )
{
	ScreenBaseClass::OnScreenUpdate();

	if( m_InvertedAiming )
		SetText( ID_OPTIONS_AIMINGMODE, "Inverted" );
	else
		SetText( ID_OPTIONS_AIMINGMODE, "Normal" );

#ifndef DIRECTX_PC
	if( m_VibrationEnabled )
		SetText( ID_OPTIONS_VIBRATIONMODE, "Enabled" );
	else
		SetText( ID_OPTIONS_VIBRATIONMODE, "Disabled" );
#endif

	Printf( ID_OPTIONS_CURRENTSENSITIVITY, "%d", m_LookSensitivity );
	Printf( ID_OPTIONS_CURRENTSPEED, "%d", m_LookSpeed );

#ifdef DIRECTX_PC
	if( m_StereoMode == STEREOMODE_MONO)
		SetText( ID_OPTIONS_STEREOMODE, "Mono" );
	else if ( m_StereoMode == STEREOMODE_STEREO )
		SetText( ID_OPTIONS_STEREOMODE, "Stereo" );
	else
		SetText( ID_OPTIONS_STEREOMODE, "Reversed Stereo" );

	if (m_BlurFX)
		SetText(ID_OPTIONS_BLURMODE, "Enabled");
	else
		SetText(ID_OPTIONS_BLURMODE, "Disabled");
#else
#ifndef _XBOX
	if( m_StereoMode )
		SetText( ID_OPTIONS_STEREOMODE, "Stereo" );
	else
		SetText( ID_OPTIONS_STEREOMODE, "Mono" );
#endif
#endif

	if( m_Blood )
		SetText( ID_OPTIONS_BLOODMODE, "Enabled" );
	else
		SetText( ID_OPTIONS_BLOODMODE, "Disabled" );

	UpdateAllMeters();
}

//=============================================================================
// ScreenOptions::OnNextScreen
//=============================================================================

void ScreenOptions::OnNextScreen( Screen* pNextScreen )
{
	m_KeepSettings = true;
}

//=============================================================================
// ScreenOptions::OnCommand
//=============================================================================

void ScreenOptions::OnCommand(int Command )
{
	switch( Command )
	{
	case ID_OPTIONS_AIMING:
		ToggleAiming();
		break;
#ifndef DIRECTX_PC
	case ID_OPTIONS_VIBRATION:
		ToggleVibration();
		break;
#endif

	case ID_OPTIONS_LOOKSENSITIVITY:
		break;

	case ID_OPTIONS_LOOKSPEED:
		break;

#ifndef _XBOX
	case ID_OPTIONS_STEREO:
		ToggleStereoMode();
		break;
#endif

	case ID_OPTIONS_SOUND:
		break;

	case ID_OPTIONS_MUSIC:
		break;

	case ID_OPTIONS_GAMMA:
		IncreaseGamma(0);
		break;

	case ID_OPTIONS_BLOOD:
		ToggleBlood();
		break;

#ifdef DIRECTX_PC
	case ID_OPTIONS_KEYBINDINGS:
		g_FrontEnd.GotoScreen("SCREEN_ID_PCKEYBINDINGS");
		break;

	case ID_OPTIONS_BLURFX:
		m_BlurFX = !m_BlurFX;
		PlaySoundEvent( Screen::SOUNDEVENT_INVALID );
		break;
#endif

	case ID_OPTIONS_DONE:
		OnCommandDone();
		break;
	}
}

//=============================================================================
// ScreenOptions::OnCommandDone
//=============================================================================

void ScreenOptions::OnCommandDone( void )
{
	g_GlobalProfile.SetControllerSettings( m_InvertedAiming, m_VibrationEnabled, m_LookSensitivity, m_LookSpeed );
	g_GlobalProfile.SetStereoMode( m_StereoMode );
	g_GlobalProfile.SetSoundVolume( m_SoundVolume );
	g_GlobalProfile.SetMusicVolume( m_MusicVolume );
	g_GlobalProfile.SetGammaValue( m_GammaValue );
	g_GlobalProfile.SetAutoSave( false );
	g_GlobalProfile.SetBlood( m_Blood );

#ifdef DIRECTX_PC
	g_GlobalProfile.SetBlurFX( m_BlurFX );
#endif

	if (ValidSingleton(Blood))
		g_blood.Enable( m_Blood );

	if (ValidSingleton(CPlayers))
		g_players.ControllerSetupChanged();

	SaveProfile();
}

//=============================================================================
// ScreenOptions::OnSaveGameResult
//=============================================================================

void ScreenOptions::OnSaveGameResult( int Result )
{
	switch( Result )
	{
	case PR_FAILED:
		break;

	default:
		GotoPrevScreen();
		break;
	}

	return;
}