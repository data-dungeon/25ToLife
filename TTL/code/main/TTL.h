/***************************************************************************/
// The main game stuff
/***************************************************************************/
#ifndef BIGJUJU_H
#define BIGJUJU_H

#include "platform/gameapp.h"

#include "main/TTLConstants.h"
#include "main/GameVersion.h"
#include "GameHelper/PersistentData.h"
#include "Network/SessionSpec.h"
#include "GameHelper/DebugOutput.h"

// game loops
#include "main/GameEnv.h"
#include "sim/SetupLoop.h"
#include "sim/NetworkLoop.h"
#include "sim/StandAloneLoop.h"

class CBaseLoop;

#define G_TTL_SOUNDMAP_ENTRIES	7

//////////////////////////////////////////////////////////////////////////////
#define DIALOG_VOLUME_ADJUST				0.8f	// adjustment from sfx volume

// How the simulator gets it's startup data
// The game class (this is used by gameapp.cpp
class TTL : public Game
{
public:
	TTL(void);
	~TTL(void);

	// setup the defaults for init'ing all the systems
	// Comes with the current defaults in it
	virtual void SetupGameInit(GAMEINIT &init);

	// Initialize
	virtual bool Initialize(void);

	// Run
	virtual bool Run(void);

	// Terminate the game
	virtual bool Terminate(void);

	// Trigger an exit to the system
	virtual void Exit(void);

	// Since user audio settings should apply throughout the game and since they
	// have nothing to do with gameplay, track them here.
	void ResetVolume(void);
	float GetSoundVolume(void) {return(soundVolume);}
	float GetMusicVolume(void) {return(musicVolume);}
	void SetSoundVolume(float volume, float fadeTime);
	void SetSoundVolume(float volume, bool smooth = true);
	void SetMusicVolume(float volume, float fadeTime);
	void SetMusicVolume(float volume, bool smooth = true);
#ifdef DIRECTX_PC
	void SetStereoSound(int stereo);
	int GetStereoSound(void) {return(stereoSound);}
#else
	void SetStereoSound(bool stereo);
	bool GetStereoSound(void) {return(stereoSound);}

#endif
	void AdjustSoundVolume(float deltaVolume, bool smooth = true)
		{SetSoundVolume(soundVolume + deltaVolume, smooth);}
	void AdjustMusicVolume(float deltaVolume, bool smooth = true)
		{SetMusicVolume(musicVolume + deltaVolume, smooth);}
	void VolumeMute();
	void VolumeFadeIn(float time);
	void VolumeFadeOut(float time);

	// Get at world database
	PersistentData& GetPersistentData()	{ return(d_persistentData); }

	char *GetVersion( void );
	void InitVersion( void );

	bool IsSinglePlayer(void) {return(d_state == SINGLEPLAYER_LOOP);}
	bool IsMultiPlayer(void) {return(d_state == MULTIPLAYER_LOOP);}

private:
	// Game loops
	enum
	{
		SETUP_LOOP,
		SINGLEPLAYER_LOOP,
		MULTIPLAYER_LOOP,
		NUM_LOOPS
	} d_state;

	// The game loops
	CBaseLoop* d_loop[NUM_LOOPS];
	CSetupLoop        d_setupLoop;
	CStandAloneLoop   d_standAloneLoop;
	CNetworkLoop      d_multiPlayerLoop;

	// The current game parameters
	float				soundVolume;
	float				musicVolume;
#ifdef DIRECTX_PC
	enum {
		STEREOMODE_MONO,
		STEREOMODE_STEREO,
		STEREOMODE_STEREO_REVERSE,
	};
	int					stereoSound;
#else
	bool				stereoSound;
#endif
	// Exit flag
	bool d_requestExit;

	// Some tidy init's
	bool GraphicsInit(void);

	// Preloaded, permanent file support
	void PreloadPermanentData();
	void PreloadFileStreamFile(char *pFileName, void *pData = NULL, int nSize = 0);
	
	// Track things that need to persist for the entire game
	PersistentData	d_persistentData;

	// Our hogs
	static const char *s_hogName[];
	static int s_hogs;

	// Declare a session spec of the appropriate size.
	CSessionSpecInstance<256, MAX_PLAYERS> d_sessionSpec;

	char m_version[ 48 ];

	// Debug Output
#ifdef DEBUG_OUTPUT_ENABLED
	CFixedDebugBuffer<20000> d_debugBuffer;
	CDebugOutput d_debugOutput;
#endif

};

extern TTL *g_ttl;
extern bool g_acceptingInvitation;
extern bool g_sessionTimestamp;

#endif
