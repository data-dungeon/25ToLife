/***************************************************************************/
// Sound bank
//
// A SoundBank is sound bank is a group of sounds.  Sounds are loaded from
// a HOG file and put directly into sound ram.  Sounds are then stored
// via an index in the bank, or by the last 17 characters of path/filename
// (the idea being that the 8 char path + 8 char file uniquely identify the
// sound)
// There is no mechanism for avoiding duplicated sounds, so try to avoid
// duplication of sounds between banks.
/***************************************************************************/
#ifndef SOUNDBANK_H
#define SOUNDBANK_H

#include "Audio/soundmodexport.h"

// forward declarations
class  Sound;
class  SoundDriver;
struct ts_DBChunkHdr;

class SoundBank
{
public:

	// Load the soundbank
	virtual bool LoadSoundBankData( void* data, int dataSize );
	virtual bool Load( void *pData );

	// Access routines for soundbank data
	static Sound*	GetEngineSound( SoundDef* s );
	static bool		IsSoundLooped( SoundDef* s );
	static bool		IsSound2D( SoundDef* s );
	static float	GetSoundVolumeInRange( SoundDef* s );
	static float	GetSoundPitch1( SoundDef* s );
	static float	GetSoundPitch2( SoundDef* s );
	static float	GetSoundPitchInRange( SoundDef* s );
	static float	GetSoundAmbience( SoundDef* s );
	static float	GetSoundPresence( SoundDef* s );
	static float	GetSoundAudibleDistance( SoundDef* s );
	SoundDef* GetSoundByName( const char* name );
	int		Sources();
	Sound*	GetSourceEngineSound( int index );

	static void MakeBankLoadName( const char *bankName, const char *path,  char *bankPathFile);

protected:

	// routines to search the sound bank database
	int FindGroupIndexByName( const char* name );
	int FindSoundIndexByName( const char* name );
	SoundDef* PickRandomSoundFromGroup( GroupDef* group );

	// The sound bank must have a sound driver
	SoundBank(SoundDriver &soundDriver);

	// Need the driver to create the sounds
	SoundDriver 			&driver;

	// Have we been marked for deletion?
	bool						markedDeleted;
	inline void MarkForDelete()
		{ markedDeleted = true; }

	SoundModHeaderDef*	soundBankData;

	// free the list
	void FreeSoundList();

	void CleanName( const char *fileName, char *clean);

	// Only the sound driver can create us
	friend class SoundDriver;

public:
 	// Destroying the bank will let go of our sounds
	// NEVER CALL THIS DIRECTLY!
	virtual ~SoundBank();

	// Innards for garbage collecting
	// Have we been deleted?
	inline bool MarkedDeleted()
		{ return markedDeleted; }
};

#endif
