/***************************************************************************/
// SWSoundStream, streams audio via software
/***************************************************************************/
#ifndef SWSOUNDSTREAM_H
#define SWSOUNDSTREAM_H

#include "Audio/GCN/soundstream.h"
#include "Audio/GCN/sdriver.h"
#include "Audio/GCN/dspadpcm.h"
#include <dolphin/dvd.h>
#include <dolphin/ax.h>

#define AUDIO_BUFFER_LENGTH		( 32 * 1024)
#define DISK_BUFFER_LENGTH			( AUDIO_BUFFER_LENGTH / 2)

class SWSoundStream : public SoundStream
{
public:
	// The driver must create the sound, but it can be deleted directly
	~SWSoundStream();

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
	{
		return( m_medError);
	}

	virtual bool SetWetDry( float wet, float dry)
	{
		SoundDriverGCN::GetSoundDriverGCN( )->SetWetDry( m_pAXVPB, ( wet > 0.0f), ( dry > 0.0f));
	}
	
private:

	struct StreamBuffer
	{
		DVDFileInfo					dvdfi;
		u8								*pBuffer;
		ARQRequest					arq;
		u32							requestedSize,
										actualSize,
										aramAddr;
		bool							bDataRead,
										bFileOpened,
										bReadPending,
										bInitialized,
										bFileComplete,
										bLastBufferRead,
										bLastBufferDMA;
		class SWSoundStream		*pThis;
	};

	// our constructor
	SWSoundStream( const char *streamName, char *fileName, bool wantRepeat);

	static void DVDCallback( s32 result, DVDFileInfo *pDVDFI);
	static void AXCallback( void *pVoid);
	static void ARQCallback( u32 pointerToARQRequest);

	static void PostDVDRead( StreamBuffer *pSB, u32 RequestedLength);
	static void StartDMA( StreamBuffer *pSB);

	void CheckStreamError( void);

	void StopLeavePlaying( void);

	bool InitStreamBuffer( s32 fileEntry, int iBuffer, u32 aramAddr)
	{
		m_sb[iBuffer].bFileOpened = false;
		m_sb[iBuffer].bReadPending = false;
		m_sb[iBuffer].bDataRead = false;
		m_sb[iBuffer].bInitialized = false;
		m_sb[iBuffer].bFileComplete = false;
		m_sb[iBuffer].bLastBufferRead = false;
		m_sb[iBuffer].bLastBufferDMA = false;
		m_sb[iBuffer].pBuffer = NULL;

  		if( !DVDFastOpen( fileEntry, &m_sb[iBuffer].dvdfi))
		{
			return( false);
		}

		m_sb[iBuffer].bFileOpened = true;
		DVDSetUserData( (DVDCommandBlock *)&m_sb[iBuffer].dvdfi, m_sb + iBuffer);
		m_sb[iBuffer].pThis = this;
		m_sb[iBuffer].aramAddr = aramAddr;
		m_sb[iBuffer].pBuffer = (u8 *)memAlloc( DISK_BUFFER_LENGTH, MEM_IO_BUFFER_ALIGN);
		if( m_sb[iBuffer].pBuffer == NULL)
		{
			return( false);
		}

		return( true);
	}


	// Our data

	SoundDriverGCN	*m_pDriver;

	StreamBuffer	m_sb[2];

	ADPCM_Hdr		m_ADPCM_Hdr;

	u32				m_StreamLastPos;

	u32				m_SamplesPlayed;

	u32				m_DiskReadPos;

	u32				m_FileLength;

	AXVPB				*m_pAXVPB;

	bool				m_bHeaderRead,
						m_bInitialized,
						m_bPlayQueued,
						m_bPlaying;

	t_Error			m_medError;

	int				m_BuffersNeeded;

	friend class SoundDriverGCN;
};

#endif
