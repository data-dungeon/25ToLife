/***************************************************************************/
// Simple player stats, made to be obscure
/***************************************************************************/
#ifndef SOBJ_H
#define SOBJ_H

#include "Components/Component.h"
#include "container/dllist.h"
#include "GameHelper/Message.h"
#include "weapons/weaponanim.h"


#define MAX_OBJECTIVES_PER_LEVEL 7
#define O_MAX_LEVELS 13

#define NUM_LEVELS 13

#define FORCE_FAIL_MESSAGE			0x00000001
#define FORCE_COMPLETE_MESSAGE	0x00000002

class SecondaryObjective// : public CActorComponent, public HealthIF
{
	public:
		SecondaryObjective();
		~SecondaryObjective(void);
		void Initialize(void);
		void Reset(void); // Reset all the stats to defaults

		void	GetRewardString( int o , char *s );
		void	GetUnlockableString( int o , char *s );

		void GetStatString( int o , char *s );
		void GetStatValue( int o , char *s );

		void GetObjectiveString( int o , char *s );
		void GetObjectiveValue( int o , char *s );
		bool IsObjectiveMet( int o );
		bool IsObjectiveFailed( int o );

		void	SetNumTags( int n ) { numTags = n; }
		void	IncNumTags( void ) { numTags++; }
		int	GetNumTags( void ) { return numTags; }

		void	SetNumKills( int n ) { numKills = n; }
		void	IncNumKills( void ) { numKills++; }
		void	DecNumKills( void ) { numKills--; }
		int	GetNumKills( void ) { return numKills; }

		void	SetNumFriendlyKills( int n ) { numFriendlyKills = n; }
		void	IncNumFriendlyKills( void ) { numFriendlyKills++; }
		int	GetNumFriendlyKills( void ) { return numFriendlyKills; }

		void	SetShotsFired( int n ) { shotsFired = n; }
		void	IncShotsFired( void ) { shotsFired++; }
		void	DecShotsFired( void ) { shotsFired--; }
		int	GetShotsFired( void ) { return shotsFired; }

		void	SetNumApprehended(int n ) { numApprehended = n; }
		void	IncNumApprehended( void ) { numApprehended++; }
		int	GetNumApprehended( void ) { return numApprehended; }

		void	SetNumHostages(int n ) { numHostages = n; }
		void	IncNumHostages( void ) { numHostages++; }
		int	GetNumHostages( void ) { return numHostages; }

		void	SetNumHits( int n ) { numHits = n; }
		void	IncNumHits( void ) { numHits++; }
		void	DecNumHits( void ) { numHits--; }
		int	GetNumHits( void ) { return numHits; }

		void	SetNumFriendlyHits( int n ) { numFriendlyHits = n; }
		void	IncNumFriendlyHits( void ) { numFriendlyHits++; }
		int	GetNumFriendlyHits( void ) { return numFriendlyHits; }

		void	SetNumHeadshots( int n ) { numHeadshots = n; }
		void	IncHeadshots( void ) { numHeadshots++; }
		int	GetNumHeadshots( void ) { return numHeadshots; }

		float	GetTimePlayed( void ) { return timer; }

		void	GetFirstPrimaryObjective( int &i );
		bool	GetNextPrimaryObjective( int &i );

		void	GetFirstBonusObjective( int &i );
		bool	GetNextBonusObjective( int &i );

		void	GetFirstStatObjective( int &i );
		bool	GetNextStatObjective( int &i );

		void	GetTimeString( char *s );

		void	SetLevel( const char *worldName );
		int	GetLevel() { return currentLevel; }

		void	Update( void );
		void	Init( void );
		void	End( void );

		char	GetObjState( int o );
		char	IsObjVisible( int o );

		int	GetObjectiveByType( int t );

		void	SetWatchSneak( bool b ) { watchSneak = b; }
		bool	isWatchingSneak( void ) { return watchSneak; }

		void updateTimer( float dt );

		void markFinished( void ) { finished = true; };

		void	setCopDead( bool cd ) { copDead = cd; }
		bool	getCopDead( void ) { return copDead; }
		void	settCopDead( bool cd ) { tcopDead = cd; }
		bool	gettCopDead( void ) { return tcopDead; }

private:

		bool	copDead;
		bool	tcopDead;

		bool	watchSneak;

		// handle fired bullets and impacts with actors
		int	numTags;
		int	numKills;
		int	shotsFired;
		int	numHeadshots;
		int	numHits;
		int	numTimesHit;
		int	numFriendlyKills;
		int	numFriendlyHits;
		int	numApprehended;
		int	numHostages;
		float	startTime;
		float	timer;
		float	lastTime;
		bool	timerPaused;

		int	currentLevel;
		int	currPrimary;
		int	currBonus;
		int	currStat;

		int	tnumTags;
		int	tnumKills;
		int	tshotsFired;
		int	tnumHeadshots;
		int	tnumHits;
		int	tnumTimesHit;
		int	tnumFriendlyKills;
		int	tnumFriendlyHits;
		int	tnumApprehended;
		int	tnumHostages;
		float	tstartTime;
		float	ttimer;
		float	tlastTime;

		bool	finished;

		REF_BASIC_MESSAGE_HANDLER( CSetCheckpoint , MESSAGE_VALUE , SecondaryObjective ) d_setCheckpointHandler;
		REF_BASIC_MESSAGE_HANDLER( CDeath , MESSAGE_VALUE , SecondaryObjective ) d_deathHandler;

		REF_BASIC_MESSAGE_HANDLER( CPauseObjTimer , MESSAGE_VALUE , SecondaryObjective ) d_pauseObjTimerHandler;
		REF_BASIC_MESSAGE_HANDLER( CUnpauseObjTimer , MESSAGE_VALUE , SecondaryObjective ) d_unpauseObjTimerHandler;

		REF_BASIC_MESSAGE_HANDLER( CSpawnAvatar , MESSAGE_VALUE , SecondaryObjective ) d_spawnAvatarHandler;
		
		REF_BASIC_MESSAGE_HANDLER( CDropHostage , MESSAGE_VALUE , SecondaryObjective ) d_dropHostageHandler;

		//REF_BASIC_MESSAGE_HANDLER( CheckpointReset , MESSAGE_VALUE, SecondaryObjective ) d_checkpointReset;

};

class PlayerStatsGrabber : public CActorComponent
{
	public:
		PlayerStatsGrabber(CCompActor &actor);
		~PlayerStatsGrabber(void);
		const char *ComponentName() { return "PlayerStatsGrabber"; }
		void Initialize(void);
		void Reset(void); // Reset all the stats to defaults
		virtual void BeginFrame();

		CInterface *GetInterface( uint i_id );

	private:

		REF_SEND_MESSAGE_HANDLER( ObjectTrigger , PlayerStatsGrabber ) objectTriggerHandler;
		
		REF_SEND_MESSAGE_HANDLER( CShaunDead , PlayerStatsGrabber ) shaunDeadHandler;
		REF_SEND_MESSAGE_HANDLER( CBuddyDead , PlayerStatsGrabber ) buddyDeadHandler;
		REF_SEND_MESSAGE_HANDLER( CBuddySafe , PlayerStatsGrabber ) buddySafeHandler;
		REF_SEND_MESSAGE_HANDLER( CLazarusDead , PlayerStatsGrabber ) lazarusDeadHandler;
		REF_SEND_MESSAGE_HANDLER( CLazarusSafe , PlayerStatsGrabber ) lazarusSafeHandler;
		REF_SEND_MESSAGE_HANDLER( CSaragosaDead , PlayerStatsGrabber ) saragosaDeadHandler;
		REF_SEND_MESSAGE_HANDLER( CMendozaDead , PlayerStatsGrabber ) mendozaDeadHandler;

		REF_SEND_MESSAGE_HANDLER( CjdubDead , PlayerStatsGrabber ) jdubDeadHandler;

		REF_SEND_MESSAGE_HANDLER( HostageGrabbed , PlayerStatsGrabber ) d_hostageGrabbedHandler;

		REF_SEND_MESSAGE_HANDLER( CSneakStart , PlayerStatsGrabber ) d_sneakStartHandler;
		REF_SEND_MESSAGE_HANDLER( CSneakStop , PlayerStatsGrabber ) d_sneakStopHandler;

		//REF_SEND_MESSAGE_HANDLER( Pause , PlayerStatsGrabber ) pauseHandler;

		REF_SEND_MESSAGE_HANDLER( MunitionFired , PlayerStatsGrabber ) munitionFiredHandler;
		REF_SEND_MESSAGE_HANDLER( MunitionImpact , PlayerStatsGrabber ) munitionImpactHandler;

		REF_BASIC_MESSAGE_HANDLER( CKill , MESSAGE_VALUE , PlayerStatsGrabber ) d_killsHandler;
		REF_BASIC_MESSAGE_HANDLER( Apprehend , MESSAGE_VALUE , PlayerStatsGrabber ) d_apprehendHandler;
		REF_BASIC_MESSAGE_HANDLER( Hostage , MESSAGE_VALUE , PlayerStatsGrabber ) d_hostageHandler;
		REF_BASIC_MESSAGE_HANDLER( CTag , MESSAGE_VALUE , PlayerStatsGrabber ) d_tagsHandler;
		REF_BASIC_MESSAGE_HANDLER( CGotCash , MESSAGE_VALUE , PlayerStatsGrabber ) d_gotCashHandler;
		REF_BASIC_MESSAGE_HANDLER( CLootPickup , MESSAGE_VALUE , PlayerStatsGrabber ) d_lootPickupHandler;

		REF_BASIC_MESSAGE_HANDLER( CCopDead , MESSAGE_VALUE , PlayerStatsGrabber ) d_copDeadHandler;
		REF_BASIC_MESSAGE_HANDLER( CCopTakerDead , MESSAGE_VALUE , PlayerStatsGrabber ) d_copTakerDeadHandler;
};

#define g_secondaryObjective GetSingleton( SecondaryObjective )

class ObjectiveDataSave
{
	public:
		char	currentObjState; 
		float	numItemsObtained;		
		bool	visible;
};

class ObjectiveData 
{
	public:
		int	id;						// unique id for this objectve
		char	*objectiveString;		// descriptor string
		float	numItems;				// number of items to obtain for this objective
		float	numItemsObtained;		// number of items already obtained for this objective
		float	numItemsObtainedSave;// number of items already obtained for this objective
		char	objectiveClass;		// O_PRIMARY, O_BONUS, O_STAT
		int	nextObjectiveID;		// if this objective opens another objective this is the id of that objective
		char	displayType;			// O_BOOL, O_INT, O_STRING, O_FLOAT, O_TIME, O_NO_TYPE
		char 	*unlockable;			// the unlockable associated with this objective
		bool	visible;					// is this objective visible yet?  can we start working on it?
		char	op;						// O_GTE, O_EQ, O_LTE, O_LT, O_GT, O_NOP
		char	objectiveType;			// O_HEADSHOTS, O_KILLS, O_GAMETIME, O_ACCURACY, O_APPREHEND, O_BONUS1, O_PRI1, O_PRI2, O_PRI3, O_PRI4, O_UNDEF
		char	defaultObjState; 
		char	currentObjState; 
		bool	forceUpdateMessage;
		long	flags;
		unsigned short fixedFlags;

	public:
		void	setID( int tid ) { id = tid; }
		int	getID( void ) { return id; }

		char	*getObjectiveString( void ) { return objectiveString; }

		void	setVisible( bool b ) { visible = b; }
		bool	isVisible( void ) { return visible; }

		void	setNumItems( float t ) { numItems = t; }
		float	getNumItems( void ) { return numItems; }

		void	setOperator( int t ) { op = (char)t; }
		int	getOperator( void ) { return (int)op; }

		void	setNumItemsObtained( float t ) { numItemsObtained = t; }
		float	getNumItemsObtained( void ) { return numItemsObtained; }

		void	setObjectiveClass( int t ) { objectiveClass = t; }
		int	getObjectiveClass( void ) { return objectiveClass; }

		void	setNextObjectiveID( int t ) { nextObjectiveID = t; }
		int	getNextObjectiveID( void ) { return nextObjectiveID; }

		void	setObjectiveType( int t ) { objectiveType = (char)t; }
		int	getObjectiveType( void ) { return (int)objectiveType; }

		void	setDisplayType( int t ) { displayType = (char)t; }
		int	getDisplayType( void ) { return (int)displayType; }

		void	setUnlockable( char *t ) { unlockable = t; }
		char	*getUnlockable( void ) { return unlockable; }

		void	setDefaultObjState( char t ) { defaultObjState = t; }
		char	getDefaultObjState( void ) { return defaultObjState; }

		void	setCurrentObjState( char t ) { currentObjState = t; }
		char	getCurrentObjState( void ) { return currentObjState; }

		void	setFixedFlags( unsigned short t ) { fixedFlags = t; }
		unsigned short	getFixedFlags( void ) { return fixedFlags; }
};

enum ObjectState
{
	O_COMPLETE = 1,
	O_FAIL,
	O_UPDATE,
	O_NO_UPDATE,
	O_WAIT
};

enum ObjectClass
{
	O_NO_OBJ = -1,
	O_PRIMARY = 1,
	O_BONUS,
	O_STAT
};

enum ObjectTypes
{
	O_UNDEF = -1,
	O_PRI1 = 1,
	O_PRI2,
	O_PRI3,
	O_PRI4,
	O_BONUS1,
	O_ACCURACY,
	O_HEADSHOTS,
	O_KILLS,
	O_CIVILIANS,
	O_APPREHEND,
	O_GAMETIME,
	O_HOSTAGE,
	O_HUMAN_SHIELD,
	O_CASH,
	O_LOOT,
	O_STONER,
	O_MARK_AT_END,
	O_TAGS,
	O_SHAUNDEAD,
	O_BUDDYDEAD,
	O_LAZARUSDEAD,
	O_SARAGOSADEAD,
	O_SNEAK,
	O_FREEACOP,
	O_MENDOZADEAD,
	O_DEATH,
	O_JUSTICE
};

enum ObjectOperators
{
	O_NOP = -1,
	O_LT = 1,
	O_GT,
	O_LTE,
	O_GTE,
	O_EQ
};

enum DisplayTypes
{
	O_NO_TYPE = -1, 
	O_INT = 1,
	O_TIME,
	O_BOOL
};

#define FF_NO_COMPLETE_MSG 0x0001
#define FF_NO_FAIL_MSG		0x0002
#define FF_DONT_RESET		0x0004

extern ObjectiveData objectives[ NUM_LEVELS ][ MAX_OBJECTIVES_PER_LEVEL ];

#endif
