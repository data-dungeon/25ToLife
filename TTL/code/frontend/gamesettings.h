//-----------------------------------------------------------
//
// CustomPlayer.cpp
//
// Handles switching of player pieces, loading/saving to
// memcard etc
//
//-----------------------------------------------------------
#ifndef _GAMESETTINGS_H_
#define _GAMESETTINGS_H_

#define DIFFICULTY_EASY				0
#define DIFFICULTY_NORMAL			1
#define DIFFICULTY_HARD				2
#define DIFFICULTY_ULTIMATE		3

#define PROFILE_DEFAULT_AUTOSAVE				true
#define PROFILE_DEFAULT_STEREO				true
#define PROFILE_DEFAULT_SOUND_VOLUME		1.0f
#ifdef PS2
#define PROFILE_DEFAULT_MUSIC_VOLUME		0.8f
#else
#define PROFILE_DEFAULT_MUSIC_VOLUME		1.0f
#endif
#define PROFILE_DEFAULT_DIFFICULTY			DIFFICULTY_NORMAL
#define PROFILE_DEFAULT_BLOOD					true

// platform defaults
#if defined (WIN32) && !defined (_XBOX)
#define	PROFILE_DEFAULT_BRIGHTNESS		1.0f
#define	PROFILE_DEFAULT_GAMMA			2.0f

#elif defined (PS2)
#define	PROFILE_DEFAULT_BRIGHTNESS		1.0f
#define	PROFILE_DEFAULT_GAMMA			0.5f

#elif defined (_XBOX)
#define	PROFILE_DEFAULT_BRIGHTNESS		1.0f
#define	PROFILE_DEFAULT_GAMMA			0.5f
#endif

struct ts_ScreenPosition
{
	float		tl_x;
	float		tl_y;
	float		br_x;
	float		br_y;

	float		h_offset;
	float		v_offset;
};

struct ts_Cheats
{
	bool	Cheat1;
	bool	Cheat2;
	bool	Cheat3;
};

struct ts_GameSettings
{
	bool					AutoSave;
#ifdef DIRECTX_PC
	enum {
		STEREOMODE_MONO,
		STEREOMODE_STEREO,
		STEREOMODE_STEREO_REVERSE,
	};
	int						Stereo;
	bool					blurEffects;
#else
	bool					Stereo;
#endif
	float					MusicVolume;
	float					SoundVolume;
	float					Brightness;
	float					Gamma;
	ts_ScreenPosition	Screen;
	ts_Cheats			Cheats;
	int					CheatsMenu;
	int					Difficulty;
	bool					Blood;
};

// class
namespace CGameSettings
{
	// init/load/save
	bool	LoadSettings();
	bool	SaveSettings();
	void	UpdateSettings();
};

#endif // _GAMESETTINGS_H_
