//============================================================================
//=
//= GameLocks.cpp - A registry for features that have been unlocked for
//=                 the player.
//=
//============================================================================

#include "GameHelper/GameHelperPCH.h"
#include "GameLocks.h"

//============================================================================

namespace GameLocks
{
	Lock s_locks[ GAMELOCKS_MAX_LOCKS ];
	int  s_numLocks = 0;
}

//============================================================================

void GameLocks::Reset()
{
	memset( s_locks, 0, sizeof( s_locks ) );
	s_numLocks = 0;
}

//============================================================================

GameLocks::Lock* GameLocks::AddLock( const char* key, const char* description, const char* unlock )
{
	Lock* lock = FindLock( key );
	if ( !lock )
	{
		ASSERT( s_numLocks < GAMELOCKS_MAX_LOCKS );
		lock = &s_locks[ s_numLocks++ ];

		ASSERT( strlen( key ) < GAMELOCKS_MAX_KEY_LENGTH );
		strcpy( lock->key, key );

		lock->description = new char[ strlen( description ) + 1 ];
		strcpy( lock->description, description );

		lock->unlockDescription = new char[ strlen( unlock ) + 1 ];
		strcpy( lock->unlockDescription, unlock );

		lock->unlocked = false;
	}
	return lock;
}

//============================================================================

bool GameLocks::IsValidKey( const char* key )
{
	return FindLock( key ) != NULL;
}

//============================================================================

char* GameLocks::KeyDescription( const char* key )
{
	Lock* lock = FindLock( key );
	return lock ? lock->description : NULL;
}

//============================================================================

char* GameLocks::UnlockDescription( const char* key )
{
	Lock* lock = FindLock( key );
	return lock ? lock->unlockDescription : NULL;
}

//============================================================================

bool GameLocks::IsUnlocked( const char* key )
{
	Lock* lock = FindLock( key );
	return lock ? ( lock->unlocked != 0 ) : false;
}

//============================================================================

void GameLocks::Unlock( const char* key )
{
	Lock* lock = FindLock( key );
	ASSERT_PTR( lock );
	if ( lock )
		lock->unlocked = true;
}

//============================================================================

GameLocks::Lock* GameLocks::FindLock( const char* key )
{
	ASSERT_PTR( key );
	ASSERT( key[0] );
	for ( int i = 0 ; i < s_numLocks ; i++ )
		if ( !stricmp( s_locks[ i ].key, key ) )
			return &s_locks[ i ];
	return NULL;
}

//============================================================================

int GameLocks::NumLocks()
{
	return s_numLocks;
}

//============================================================================

GameLocks::Lock* GameLocks::GetLock( int index )
{
	ASSERT( index >= 0 && index < s_numLocks );
	return &s_locks[ index ];
}
	
//============================================================================

void GameLocks::UnlockAll()
{
	for ( int i = 0 ; i < s_numLocks ; i++ )
		s_locks[ i ].unlocked = true;
}

//============================================================================

void GameLocks::LockAll()
{
	for ( int i = 0 ; i < s_numLocks ; i++ )
	{
		if ( stricmp( s_locks[i].key, "A_DEFAULT" ) == 0 )
			s_locks[ i ].unlocked = true;
		else
			s_locks[ i ].unlocked = false;
	}
}

//============================================================================
