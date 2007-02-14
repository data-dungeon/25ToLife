//-----------------------------------------------------------
//
//	savegame.h
//
//
//-----------------------------------------------------------
#ifndef _SAVEGAME_H_
#define _SAVEGAME_H_

#include "main/PlayerProfile.h"
#include	"frontend/customplayer.h"
#include	"frontend/gamesettings.h"
#include "platform/MemoryCard/MemoryCard.h"
#include "platform/wantpal.h"
#include "GameHelper/GameLocks.h"
#include "effects/customtag.h"

//	defines
#if defined	(WIN32) && !defined (_XBOX)
#define	SAVEGAME_FILENAME		"ttl.sav"
#define	SAVEGAME_PREFIX		""
#define	SAVEGAME_SEARCH_MASK	"*.sav"
#define	SAVEGAME_EXT			".sav"
#define	SAVEGAME_TITLEID		"25TOLIFE"
#elif	defined (_XBOX)
#define	SAVEGAME_FILENAME		"ttl.sav"
#define	SAVEGAME_PREFIX		""
#define	SAVEGAME_SEARCH_MASK	"*.sav"
#define	SAVEGAME_EXT			".sav"
#define	SAVEGAME_TITLEID		"25TOLIFE"
#elif	defined (PS2)
#define	SAVEGAME_FILENAME		""
#define	SAVEGAME_PREFIX		""
#define	SAVEGAME_SEARCH_MASK	"*.sav"
#define	SAVEGAME_EXT			".sav"
#define	SAVEGAME_TITLEID		(g_regionUSA ? "BASLUS-21016T_T_L" : "BESLES-53199T_T_L" )
#define	PATCH_EXT				".pat"
#endif

#define	SAVEGAME_VER	0xFFFF0019			//	need to update	if	things change

#define	SG_MAX_WEAPONS_PER_PLAYER	4
#define	SG_NO_SINGLE_PLAYER_SAVE	-1
#define	SG_NUM_PAIN_SLOTS				6
#define	SG_MAX_LEVELS					256	//	plenty of room
#define	SG_MAX_PROFILES				12		//	PS2 has a 13 file	limit	per dir.. (one slot used for patch)
#ifdef DIRECTX_PC
#define SG_MAX_CHECKPOINTS			10
#endif

//	current save game	settings: profile, port	etc
#define SG_MAX_PROFILE_NAME		24
struct ts_SaveGameParams
{	
	char	ProfileName[SG_MAX_PROFILE_NAME];
	int	Port;
};

//	weapon stats
struct ts_WeaponInfo
{
	char	WeaponName[16];
	int	Ammo;
};

struct ts_ControllerSettings
{
	bool	InvertedAiming;
	bool	VibrationEnabled;
	int	LookSensitivity;
	int	LookSpeed;
	int   ButtonLayout;
	int   ThumbStickMode;
};

struct ts_GameLocks
{
	char	key[ GAMELOCKS_MAX_KEY_LENGTH ];
	char	unlocked;
};

#define PROFILE_DEFAULT_INVERTEDAIMING			(false)
#define PROFILE_DEFAULT_VIBRATIONENABLED		(true)
#define PROFILE_DEFAULT_LOOKSENSITIVITY		(5)
#define PROFILE_DEFAULT_LOOKSPEED				(5)
#define PROFILE_DEFAULT_BUTTONLAYOUT			(0)
#define PROFILE_DEFAULT_THUMBSTICKMODE			(0)

//	weapon loadout
struct ts_WeaponLoadout
{
	char	InitString[256];
};

//	player stat	struct
struct ts_PlayerStats
{
	int	Character;								//	which	of	the dudes are we using
	char	PlayerName[16];
	int	Health;										
	int	PainStats[SG_NUM_PAIN_SLOTS];		//	pain texture indices
};

#ifdef DIRECTX_PC
struct ts_Checkpoint
{
	Vector3 pos;
	Vector3 facing;
	int checkpoint;
	int checkpointFlags;
	ts_WeaponInfo	checkpointWeapons[SG_MAX_WEAPONS_PER_PLAYER];
	ts_PlayerStats  checkpointStats;
};

enum CheckpointFlags {
	UNIQUE_LEVEL_EVENT_ONE	= 1,
	UNIQUE_LEVEL_EVENT_TWO  = 2,
	VALID_CHECKPOINT		= 1000000

};

#endif

//	single player save game	info
struct ts_SinglePlayer
{
	int				CurrentLevel;											//	Next level in single	player
	int				PreviousLevel;											//	Recently	completed level in single player
	int				CompletedLevels[SG_MAX_LEVELS];							//	Levels that	have been completed
#ifdef DIRECTX_PC
	ts_Checkpoint	LevelCheckpoints[SG_MAX_LEVELS][SG_MAX_CHECKPOINTS];	//  For saving at checkpoints in a level
#endif
	ts_WeaponInfo	Weapons[SG_MAX_WEAPONS_PER_PLAYER];						//	weapons
	ts_PlayerStats	Stats;													//	health, pain, etc
};

//	for multiplayer system link, we'll remember the	settings
//	used to create	the most	recent match and store them here.
struct ts_SystemLinkPrefs
{
	char					MatchConfig[1024];							//	Stores the config	manager option	string
};

// voice parameters
struct ts_VoiceParams
{
	u8 VoiceOutput;
	u8 OutputVolume;
};

enum
{
	VOICEOUTPUT_DISABLED,
	VOICEOUTPUT_HEADSET,
	VOICEOUTPUT_SPEAKERS,
	VOICEOUTPUT_MAX,
};

struct ts_SaveGameHeader
{
	int		UncompressedSize;
	int		CompressedSize;
	int		SGVersionID;
	int		DBVersionID;
};

#define MAX_ONLINE_ACCOUNTS 4 // Maximum number of allowed online accounts

// Online account information
struct ts_OnlineAccount
{
    char AccountName[255];     // The account / online name
    char Password[255];        // Account password
    bool SavePassword;         // Should the password be saved?
    bool Changed;              // Has this account been changed since it was loaded?
    char Email[255];           // Account email address
    int BirthMonth;            // Birthday month: 0 = January, 1 = Feburary, etc.
    unsigned char BirthDay;    // Birthday day (1-31)
    unsigned short BirthYear;  // Birthday year
    bool Gender;               // Gender: male = true, female = false
    bool EidosPromos;          // Do they want Eidos promotions in their email?
    bool EidosPartners;        // Do they want Eidos partners to email them?
    int Country;               // Country the account holder is in (0=USA, 1=Europe)
    char ZipCode[10];          // ZipCode the account holder is in
    bool InUse;                // Is this account currently being used?

    // Constructor to initialize needed fields sanely
    ts_OnlineAccount()
    {
        InUse = false;
        Changed = false;
        AccountName[0] = 0;
        Password[0] = 0;
        Email[0] = 0;
        Country = 0;
		ZipCode[0] = 0;
    }
};

//	Profile struct	for saving to memory	card
	// NOTE if you add anything to this struct it must be after 'Header'
struct ts_SaveProfile
{
//	ts_SaveGameHeader		Header;											//	header info	that isnt compressed	or	encrypted
	
	char						ProfileName[32];								//	Profile name
	u8							AutoSave;										// Autosave enabled/disabled
	ts_GameSettings		GameSettings;									//	options etc
	ts_ControllerSettings ControllerSettings;							//	controller settings
	ts_GameLocks			Locks[GAMELOCKS_MAX_LOCKS];				// what the user has unlocked
	ts_CustomProfile		Characters[CS_MAX_CHARACTERS];			//	custom chars
	ts_SinglePlayer		SinglePlayer;									//	single player progress
	int						SelectedCharacter[CPlayerProfile::NUM_VARIANTS];	//	what cop	is	currently selected
	ts_SystemLinkPrefs	SystemLinkPrefs;
	ts_TagParams			TagParams;
	ts_VoiceParams			VoiceParams;
	ts_OnlineAccount		OnlineAccounts[MAX_ONLINE_ACCOUNTS];	// Accounts for logging onto online games
	int						CurrentAccount;							// Currently selected account in the list

	//==========================================================================
	//	Accessor	Functions
	//		
	//		These	functions are here to make	life easier	when working with	a profile.
	//		Do	not make	any of these functions virtual for any	reason as this	would
	//		generate	a virtual table for this struct.
	//==========================================================================

	void	ResetToDefaults( void );
	void	SetName(	const	char*	pProfileName );
	const	char*	GetName(	void );
	void  SetAutoSave( bool Enabled );
	bool  GetAutoSave( void );
	void  SetCustomLoadout( int ch, const char* pLoadoutString );
	const char* GetCustomLoadout( int ch );
	int	GetLoadoutSlots( void );
	bool	ValidVariant( int Variant );
	void	SetCompletedLevel( int Level,	bool Completed	);
	bool	GetCompletedLevel( int Level );
#ifdef DIRECTX_PC
	void			SetLevelCheckpoint( int level, int cpNum, ts_Checkpoint& checkpoint );
	ts_Checkpoint&	GetLevelCheckpoint( int level, int checkpoint );
	int				GetNumValidCheckpoints( int level );
	void			SetLevelSpecificCharacterInfo( int level, int cpNum );
#endif
	static bool	ValidLevel(	int Level );
	void	SetCurrentLevel( int	Level	);
	int	GetCurrentLevel( void );
	void	SetPreviousLevel(	int Level );
	int	GetPreviousLevel(	void );
	bool	GetLevelUnlocked(	int Level );
	void	SetSystemLinkMatchConfig( const char* pConfigString );
	const	char*	GetSystemLinkMatchConfig( void );
	void	SetCustomChar(	int ch, const char* pName,	const	char*	pInitString, int type );
	ts_CustomProfile*	GetCustomChar(	int ch );
	void  SaveCustomChars( void );
	void  LoadCustomChars( void );
	bool	ValidCustomChar( int	ch	);
	int	MaxCustomChar(	void );
	void  SelectCharacter( int ProfileTeam, int n );
	int   GetSelectedCharacter( int ProfileTeam );
	void	SetTagString( const char* pText );
	const	char*	GetTagString( void );
	int   GetMaxTag( void );
	int   GetMaxTagColors( void );
   void  SetTagColor( int n, int Red, int Green, int Blue, int Alpha );
   void  SetTagColor( int n, float Red, float Green, float Blue, float Alpha );
   void  GetTagColor( int n, int &Red, int &Green, int &Blue, int &Alpha );
   void  GetTagColor( int n, float &Red, float &Green, float &Blue, float &Alpha );
	void  GetTagParams( ts_TagParams &TagParams );
	void	SetLookSensitivity( int LookSensitivity	);
	void	SetLookSpeed( int LookSpeed	);
	void	SetInvertedAiming( bool	Inverted	);
	void	SetVibration( bool Vibration );
	void	SetControllerSettings( bool Inverted, bool Vibration,	int	LookSensitivity, int LookSpeed	);
	void  SetButtonLayout( int ButtonLayout );
	void  SetThumbStickMode( int ThumbStickMode );
	int	GetLookSensitivity( void );
	int	GetLookSpeed( void );
	bool	GetInvertedAiming( void	);
	bool	GetVibration( void );
	int   GetButtonLayout( void );
	int   GetThumbStickMode( void );
	void  SetDifficulty( int Difficulty );
	int   GetDifficulty( void );
#ifdef DIRECTX_PC
	void  SetStereoMode( int Enabled );
	int   GetStereoMode( void );
	void  SetBlurFX( bool BlurFX );
	bool  GetBlurFX();
#else
	void  SetStereoMode( bool Enabled );
	bool  GetStereoMode( void );
#endif
	void	SetSoundVolume( float Volume );
	void	SetMusicVolume( float Volume );
	float	GetSoundVolume( void	);
	float	GetMusicVolume( void	);
	void	SetGammaValue(	float	Value	);
	float	GetGammaValue(	void );
	void  SetDefaultVoiceParams( void );
	void	SaveGameLocks( void );
	void	LoadGameLocks( void );
	void	SetBlood( bool Enabled );
	bool	GetBlood( void );
};

class	CSaveGame
{
public:
	static	void	Init();
	static	bool	Save(char* FileName,	int Port, int SaveType);
	static	bool	Load(char* FileName,	int Port, int LoadType);
	static	bool	LoadHeader(char* FileName, int Port, char* Buffer);
	static	bool	LoadImmediately(char* FileName);		//	for debugging only
	static	bool	Delete(char* FileName, int Port, int LoadType);
	static	bool	SavePatch(char* FileName, int	Port,	char*	Buffer, int	DataSize);
	static	bool	LoadPatch(char* FileName, int	Port,	char*	Buffer, int	DataSize);
	static	void	StartupCheck(int Port);

	static	bool	AutoSave();									//	note filename and	port must be setup ahead of time
	static	void	SetProfileName(char*	Profile);
	static	char*	GetProfileName();
	static	int	LoadNetworkConfiguration( void *list );
	static	void	AsyncStateMachine();

};

extern	ts_SaveProfile		g_GlobalProfile;
extern	ts_SaveGameParams	g_SaveGameParams;

#endif	//	_SAVEGAME_H_
