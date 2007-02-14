//-----------------------------------------------------------
//
// GameSettings.cpp
//
// global options for the game.. saved/loaded to memcard
//
//-----------------------------------------------------------

#include "TTLPCH.h"
#include "frontend/gamesettings.h"
#include "platform/MemoryCard/MemoryCard.h"
#include "Display/TextureUtil.h"
#include "avatar/avcontrolmap.h"
#include "frontend/savegame.h"

//-----------------------------------------------------------
// vars

//-----------------------------------------------------------
bool	CGameSettings::LoadSettings()
{
	UpdateSettings();
	return true;
}
//-----------------------------------------------------------
void	CGameSettings::UpdateSettings()
{
	AvatarControlMapper* pMapper = (AvatarControlMapper*) g_controlFocus.Find( "Avatar" );

	// screen
//	TextureUtil::SetGamma(g_GlobalProfile.GameSettings.Gamma);
	TextureUtil::SetGammaZeroToOne( g_GlobalProfile.GameSettings.Gamma );

#ifdef DIRECTX_PC
	// update our gamma
	//	TextureUtil::UpdateGamma();
	// update our rendering to texture
	RenderToTexture::isRenderingToTextureOn = g_GlobalProfile.GameSettings.blurEffects;
#endif
	// audio
	g_audio.SetVolume( Audio::SFX, g_GlobalProfile.GameSettings.SoundVolume );
	g_audio.SetVolume( Audio::MUSIC, g_GlobalProfile.GameSettings.MusicVolume );

	// vibrate
	ControllerInputDriver *c = NULL;
	if ( g_input.ControllerLocked(0) )
		c = g_input.GetController(0);

	// blood
	if( ValidSingleton(Blood) )
		g_blood.Enable( g_GlobalProfile.GetBlood() );

	if (c)
		c->Vibration(g_GlobalProfile.GetVibration());
}

//-----------------------------------------------------------
bool	CGameSettings::SaveSettings()
{
	AvatarControlMapper* pMapper = (AvatarControlMapper*) g_controlFocus.Find( "Avatar" );

	// set audio
	g_GlobalProfile.GameSettings.SoundVolume = g_audio.GetVolume( Audio::SFX );
	g_GlobalProfile.GameSettings.MusicVolume = g_audio.GetVolume( Audio::MUSIC );

	// screen
//	g_GlobalProfile.GameSettings.Gamma = TextureUtil::GetGamma(); 

	return true;
}

//-----------------------------------------------------------



