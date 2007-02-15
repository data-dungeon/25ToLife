/***************************************************************************/
// SoundStream, a disk or i/o based sound
/***************************************************************************/
#ifndef SOUNDSTREAM_H
#define SOUNDSTREAM_H

#include "Audio/GCN/sdriver.h"

class SoundStream
{
public:
	// Can't construct us, we are an abstract base class
	SoundStream( const char *streamName)
	{
		strncpy( m_name, streamName, sizeof(m_name) - 1);
		m_name[ sizeof(m_name) - 1] = '\0';
	}

	// The driver must create the sound, but it can be deleted directly
	virtual ~SoundStream( )
	{
	}

	// What is our name?
	const char *Name()
		{ return m_name; }

	// Move instance functions up, as most platforms won't
	// support a SoundInstance interface
	virtual bool Play( void) = 0;
	virtual bool Stop( bool rewind = false) = 0;
	virtual bool IsPlaying( void) = 0;

	// Return the play position, returns -1 if still preloading
	virtual int CurrentPlayPos() = 0;

	// Rewind the sound
	virtual bool Rewind( void) = 0;

	// Set the volume [0..1], 0.0 is silent 1.0 is full volume
	virtual bool SetVolume( float volume) = 0;

	// Set the wet/dry mix
	virtual bool SetWetDry( float wet, float dry)
	{
		return true;
	}

	virtual t_Error StreamError( void) = 0;

protected:
	char			m_name[32];
	bool			m_bStreamQueued;

	s32			m_EntryNum;

	friend class SoundDriverGCN;
};

#endif
