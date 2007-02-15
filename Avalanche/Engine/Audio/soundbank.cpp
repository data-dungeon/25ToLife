/***************************************************************************/
// Sound bank
//
// A SoundBank is sound bank is a group of sounds.  Sounds are loaded from
// a HOG file and put directly into sound ram.  Sounds are then stored
// via an index in the bank, or by the last 16 characters of path/filename.
// There is no mechanism for avoiding duplicated sounds, so try to avoid
// duplication of sounds between banks.
/***************************************************************************/

/******************* includes ****************************/

#include "Audio/AudioPCH.h"
#include "WatchClient/WatchClient.h"
#include "Math/fastrand.h"
#include "Network/Connection.h"


/***************************************************************************/
// The sound bank must have a sound driver
/***************************************************************************/
SoundBank::SoundBank( SoundDriver &soundDriver ) : driver(soundDriver)
{
	markedDeleted = false;
	soundBankData = NULL;
}

/***************************************************************************/
// We count on the sound driver destructor to free up our sounds
/***************************************************************************/
SoundBank::~SoundBank()
{
	// If the driver didn't set this, this is bad!
	ASSERT(markedDeleted);

	// Free the sound list
	FreeSoundList();
}

/***************************************************************************/
// Lookup the engine sound ptr for a soundDef
/***************************************************************************/
Sound* SoundBank::GetEngineSound( SoundDef* s )
{
	ASSERT( s );
	ASSERT( s->source );
	return (Sound*) s->source->engineSndPtr;
}

// Is Looped?
bool SoundBank::IsSoundLooped( SoundDef* s )
{
	ASSERT( s );
	ASSERT( s->source );
	return ( s->source->flags & SOUND_SOURCE_FLAG_LOOPING ) != 0;
}

// 2D sound?
bool SoundBank::IsSound2D( SoundDef* s )
{
	ASSERT( s );
	ASSERT( s->source );
	return ( s->source->flags & SOUND_SOURCE_FLAG_2D ) != 0;
}

// Total number of sources
int SoundBank::Sources()
{
	return soundBankData->sourceCount;
}

// return engine ptr for sources
Sound* SoundBank::GetSourceEngineSound( int index )
{
	return (Sound*) soundBankData->sourceTable[ index ].engineSndPtr;
}

// random sound volume in range
float SoundBank::GetSoundVolumeInRange( SoundDef* s )
{
	ASSERT_PTR( s );
	ASSERT( s->volume2 >= s->volume1 );
	float r = g_random.Unit();
	float v = ( (float)s->volume1 + r * (float)( (int)s->volume2 - (int)s->volume1 ) ) / 100.0f;
	return Math::Clamp( v, 0.0f, 1.0f );
}

// random sound pitch in range
float SoundBank::GetSoundPitchInRange( SoundDef* s )
{
	ASSERT_PTR( s );
	ASSERT( s->pitch2 >= s->pitch1 );
	float r = g_random.Unit();
	float v = ( (float)s->pitch1 + r * (float)( (int)s->pitch2 - (int)s->pitch1 ) ) / SOUND_MOD_PITCH_SCALE;
	return v;
}

// ambience
float SoundBank::GetSoundAmbience( SoundDef* s )
{
	ASSERT_PTR( s );
	return Math::Clamp( (float)s->ambience / 100.0f, 0.0f, 1.0f );
}

// presense
float SoundBank::GetSoundPresence( SoundDef* s )
{
	ASSERT_PTR( s );
	return Math::Clamp( (float)s->presence / 100.0f, 0.0f, 1.0f );
}

// audible distance
float SoundBank::GetSoundAudibleDistance( SoundDef* s )
{
	ASSERT_PTR( s );
	return (float)s->audibleDist;
}


/***************************************************************************/
// Load the sound bank data and fix the ptrs
/***************************************************************************/
bool SoundBank::LoadSoundBankData( void* data, int dataSize )
{
	int i, j;

	//-- free old data
	if ( soundBankData != NULL )
		delete[] soundBankData;

	//-- make copy of data
	memSetUseBit(MEM_AUDIO_DATA);
	soundBankData = ( SoundModHeaderDef* ) new char[ dataSize ];
	memClearUseBit(MEM_AUDIO_DATA);
	if ( soundBankData == NULL )
	{
		ASSERT( false );
		return false;
	}
	memcpy( soundBankData, data, dataSize );

	//-- fix up the pointers
	SOUND_INIT_PTR( soundBankData->sourceTable  );
	SOUND_INIT_PTR( soundBankData->soundTable   );
	SOUND_INIT_PTR( soundBankData->groupTable   );

	//-- sound ptrs
	for ( i = 0 ; i < soundBankData->soundCount ; i++ )
	{
		SOUND_INIT_PTR( soundBankData->soundTable[ i ].name   );
		SOUND_INIT_PTR( soundBankData->soundTable[ i ].source );
	}

	//-- groups ptrs
	for ( i = 0 ; i < soundBankData->groupCount ; i++ )
	{
		SOUND_INIT_PTR( soundBankData->groupTable[ i ].name     );
		SOUND_INIT_PTR( soundBankData->groupTable[ i ].soundTbl );
		for ( j = 0 ; j < soundBankData->groupTable[ i ].count ; j++ )
		{
			SOUND_INIT_PTR( soundBankData->groupTable[ i ].soundTbl[ j ] );
		}
	}

	return true;
}

/***************************************************************************/
// Load a single
/***************************************************************************/
bool SoundBank::Load( void *pData )
{
	// is this soundmod exported file?
	SoundModHeaderDef* fh = ( SoundModHeaderDef* ) pData;
	if ( fh->id != SOUND_MOD_FILE_ID )
	{
		ASSERT( false );	// not the correct file format
		return false;
	}
	if ( fh->version != SOUND_MOD_FILE_VERSION )
	{
		ASSERT( false );	// not the correct file version
		return false;
	}

	// get our copy of the permanent data
	if ( LoadSoundBankData( fh, fh->soundBankOffset ) == false )
		return false;

	// load the source effects into sound ram
	u32 startTime = g_timer.GetOSTime();
	u32 lastNetUpdate = startTime;
	for ( int i = 0 ; i < soundBankData->sourceCount ; i++ )
	{
		char* data     = (char*)pData + soundBankData->soundBankOffset + soundBankData->sourceTable[ i ].offset;
		int	dataSize = soundBankData->sourceTable[ i ].length;
		memSetUseBit(MEM_AUDIO_DATA);
		soundBankData->sourceTable[ i ].engineSndPtr = driver.CreateSound( *this, data, dataSize );
		memClearUseBit(MEM_AUDIO_DATA);

		// allow network some time!
		if (ValidSingleton(CConnection) && (g_timer.GetOSTime() - lastNetUpdate) > 50)
		{
			g_connection.Service();
			lastNetUpdate = g_timer.GetOSTime();
		}
	}
	u32 endTime = g_timer.GetOSTime();
	u32 elapsed = endTime - startTime;
	dbgPrint( "SOUNDCHUNK: %6dms loading %d sounds\n", elapsed, soundBankData->sourceCount );

	return true;
}

/***************************************************************************/
// Find a sound in the group table by name
// Returns -1 if not found
/***************************************************************************/
int SoundBank::FindGroupIndexByName( const char* name )
{
	int lo = 0;
	int hi = soundBankData->groupCount - 1;

	// it's a sorted list, binary search it
	while ( lo <= hi )
	{
		int md  = ( lo + hi ) / 2;
		int cmp = strcmpi( name, soundBankData->groupTable[ md ].name );
		if      ( cmp < 0 ) hi = md - 1;
		else if ( cmp > 0 ) lo = md + 1;
		else                return md;
	}

	// not found
	return -1;
}

/***************************************************************************/
// Find a sound in the group table by name
// Returns -1 if not found
/***************************************************************************/
int SoundBank::FindSoundIndexByName( const char* name )
{
	int lo = 0;
	int hi = soundBankData->soundCount - 1;

	// it's a sorted list, binary search it
	while ( lo <= hi )
	{
		int md  = ( lo + hi ) / 2;
		int cmp = strcmpi( name, soundBankData->soundTable[ md ].name );
		if      ( cmp < 0 ) hi = md - 1;
		else if ( cmp > 0 ) lo = md + 1;
		else                return md;
	}

	// not found
	return -1;
}

/***************************************************************************/
// Pick a random sound from a group
/***************************************************************************/
SoundDef* SoundBank::PickRandomSoundFromGroup( GroupDef* group )
{
	SoundDef* sound = NULL;

	ASSERT( group->count > 0 );

	switch ( group->type )
	{
		case SOUND_GROUP_TYPE_RANDOM:
		{
			int r = g_random.UpToExcludeHigh( (int)group->count );
			sound = group->soundTbl[ r ];
		}
		break;

		case SOUND_GROUP_TYPE_RANDOM_UNIQUE:
		{
			ASSERT( group->count <= 32 );
			int unusedList[ 32 ], unusedCount = 0;
			int r;

			for ( int i = 0 ; i < group->count ; i++ )
				if ( ( group->engineData & ( 1 << i ) ) == 0 )
					unusedList[ unusedCount++ ] = i;

			if ( unusedCount == 0 )
			{
				group->engineData = 0;
				r = g_random.UpToExcludeHigh( (int)group->count );
			}
			else
			{
				r = unusedList[ g_random.UpToExcludeHigh( unusedCount ) ];
			}

			group->engineData |= ( 1 << r );
			sound = group->soundTbl[ r ];
		}
		break;

		case SOUND_GROUP_TYPE_RANDOM_SKIP:
		{
			//-- plays the sound in incrementing order, sometimes skiping
			//-- to a random sound to break the rhythm.
			int r;
			if ( g_random.UpToExcludeHigh( 100 ) < 20 )
				r = g_random.UpToExcludeHigh( (int)group->count );
			else
				r = ( group->engineData + 1 ) % group->count;
			group->engineData = r;
			sound = group->soundTbl[ r ];
		}
		break;

		default:
			ASSERT( false );	// unhandled group type
	}

	return sound;
}

/***************************************************************************/
// Find a sound based on it's name
// Returns -1 if not found
/***************************************************************************/
SoundDef* SoundBank::GetSoundByName( const char* name )
{
	if ( soundBankData == NULL )
	{
		ASSERT( false );
		return NULL;
	}

	// Is it in the group table?
	int groupIndex = FindGroupIndexByName( name );
	if ( groupIndex >= 0 )
	{
		return PickRandomSoundFromGroup( &soundBankData->groupTable[ groupIndex ] );
	}

	// Is it in the sound table?
	int soundIndex = FindSoundIndexByName( name );
	if ( soundIndex >= 0 )
	{
		return &soundBankData->soundTable[ soundIndex ];
	}

	// not found
	return NULL;
}

/***************************************************************************/
/***************************************************************************/
void SoundBank::MakeBankLoadName(
const char *bankName,
const char *path,
char *bankPathFile)
{
	if (!path)
		strcpy(bankPathFile, bankName);
	else
	{
		strcpy(bankPathFile, path);
		int len = strlen(bankPathFile);
		if (len > 0 && bankPathFile[len - 1] != '\\')
			strcat(bankPathFile, "\\");
		strcat(bankPathFile, bankName);
	}

	// Do we have .dbl?
	int ext = strlen(bankPathFile) - 4;
	if (ext < 0 || strcmpi(&bankPathFile[ext], ".dba") != 0)
		strcat(bankPathFile, ".dba");
}

/***************************************************************************/
// Free the sound list
/***************************************************************************/
void SoundBank::FreeSoundList()
{
	if ( soundBankData != NULL )
	{
		for ( int i = 0 ; i < soundBankData->sourceCount ; i++ )
		{
			if ( soundBankData->sourceTable[ i ].engineSndPtr != NULL )
			{
				delete soundBankData->sourceTable[ i ].engineSndPtr;
				soundBankData->sourceTable[ i ].engineSndPtr = NULL;
			}
		}
		delete[] soundBankData;
		soundBankData = NULL;
	}
}
