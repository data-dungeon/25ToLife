
#include "TTLPCH.h"
#include "XboxUIXAudioPlugin.hpp"
#include "osi/OSIchar.hpp"

//AUDIO_PRESS_A="PressA"
//AUDIO_PRESS_B="PressB"
//AUDIO_ERROR="Error"
//AUDIO_LIST_ITEM_CHANGED="MenuItemSelect"
//AUDIO_POPUP_MESSAGE="Popup"
//AUDIO_PASSCODE="PasscodeEntry"
//AUDIO_NOTIFICATION="Notification"

//SetSoundEvent( Screen::SOUNDEVENT_INVALID, "menu_invalid" );
//SetSoundEvent( Screen::SOUNDEVENT_DISABLED, "menu_disabled" );
//SetSoundEvent( Screen::SOUNDEVENT_SELECT, "menu_select" );
//SetSoundEvent( Screen::SOUNDEVENT_NEXTBUTTON, "menu_down" );
//SetSoundEvent( Screen::SOUNDEVENT_PREVBUTTON, "menu_up" );
//SetSoundEvent( Screen::SOUNDEVENT_SHOWSCREEN, "screen_show" );
//SetSoundEvent( Screen::SOUNDEVENT_PREVSCREEN, "screen_back" );
//SetSoundEvent( Screen::SOUNDEVENT_SHOWPOPUP, "popup_show" );
//SetSoundEvent( Screen::SOUNDEVENT_HIDEPOPUP, "popup_hide" );

struct SoundNameMap { const char* uix; const char* ttl; };
static SoundNameMap sSoundNameMap[] = 
{ 
	{ "PressA",			"menu_select" },
	{ "PressB",			"screen_back" },
	{ "Error",			"popup_show" },
	{ "MenuItemSelect", "menu_down" },
	{ "Popup",			"menu_invalid" },
	{ "PasscodeEntry",	"menu_down" },
	{ "Notification",	"popup_show" }
};


ULONG UIXAudioPlugin::Release()
{
	return 0;
}

HRESULT UIXAudioPlugin::DoWork()
{
	return S_OK; // return value is ignored by UIX according to docs
}

HRESULT UIXAudioPlugin::PlaySound( LPCSTR soundNameUIX )
{
	const char* soundNameTTL = 0;
	for( int i = 0; i < NUM_ARRAY_ELEMENTS( sSoundNameMap ); i++ )
	{
		if( !strcmp( sSoundNameMap[i].uix, soundNameUIX ) )
		{
			soundNameTTL = sSoundNameMap[i].ttl;
			break;
		}
	}

	if( soundNameTTL )
	{
		Sfx::Play2D( soundNameTTL, 1.0f, 0.0f );
	}

	return S_OK; // return value is ignored by UIX according to docs
}

