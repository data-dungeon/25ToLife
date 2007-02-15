/***************************************************************************/
// HWSoundStream, uses GC hardware audio streaming
/***************************************************************************/
#ifndef HWSOUNDSTREAM_H
#define HWSOUNDSTREAM_H

#include "Audio/GCN/soundstream.h"
#include "Audio/GCN/sdriver.h"
#include <dolphin/dvd.h>

class HWSoundStream : public SoundStream
{
public:
	// The driver must create the sound, but it can be deleted directly
	~HWSoundStream();

	// Move instance functions up, as most platforms won't
	// support a SoundInstance interface
	bool Play();
	bool Stop(bool rewind = false);
	bool IsPlaying();

	// Return the play position, returns -1 if still preloading
	int CurrentPlayPos();

	// Rewind the sound
	bool Rewind();

	// Set the volume [0..1], 0.0 is silent 1.0 is full volume
	bool SetVolume( float volume);

	virtual t_Error StreamError( void)
		{ return m_medError; }

private:
	// our constructor
	HWSoundStream( const char *streamName, bool wantRepeat);
	static void PrepareCallback( s32 result, DVDFileInfo *pdvdfi);
	static void StatusCallback( s32 result, DVDCommandBlock *pblock);

	static DVDFileInfo			m_dvdfi;
	static DVDCommandBlock		m_dvdcb;

	bool					m_bOpened;
	bool					m_bRepeat;
	bool					m_bPrepareQueued;
	bool					m_bPrepared;
	bool					m_bPlaying;
	volatile bool		m_bStatusRequestQueued;
	volatile bool		m_bStatusRequestValid;

	s32					m_PrepareResult;
	s32					m_StatusRequest;

	t_Error				m_medError;

	u8						m_volume;

	friend class SoundDriverGCN;
};

#endif
