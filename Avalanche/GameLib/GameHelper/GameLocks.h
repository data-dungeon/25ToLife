//============================================================================
//=
//= GameLocks.h - A registery for features that have been unlocked for
//=               the player.
//=
//============================================================================

#ifndef _GAMELOCKS_H
#define _GAMELOCKS_H

//============================================================================

#define GAMELOCKS_MAX_KEY_LENGTH 15
#define GAMELOCKS_MAX_LOCKS		200

namespace GameLocks
{
	struct Lock
	{
		char	key[ GAMELOCKS_MAX_KEY_LENGTH ];
		char	unlocked;
		char*	description;
		char* unlockDescription;
	};

	void Reset();

	bool	IsValidKey( const char* key );
	char*	KeyDescription( const char* key );
	char*	UnlockDescription( const char* key );

	bool IsUnlocked( const char* key );
	void Unlock( const char* key );

	Lock*	AddLock( const char* key, const char* description, const char* unlock );
	Lock*	FindLock( const char* key );

	int	NumLocks();
	Lock*	GetLock( int index );

	void	UnlockAll();
	void  LockAll();
};

//============================================================================

#endif //_GAMELOCKS_H

//============================================================================
