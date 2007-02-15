/***************************************************************************/
// Sound driver
/***************************************************************************/
#ifndef SDRIVER_H
#define SDRIVER_H

#include "container/dllist.h"
#include "Audio/GCN/sounderror.h"
#include "Audio/soundattr.h"
#include "Audio/sounddriver.h"
#include "Audio/GCN/dspadpcm.h"

#include <dolphin.h>
#include <dolphin/mix.h>
#include <dolphin/axfx.h>

class Sound;
class SoundInstance;
class SoundStream;
class SoundBank;
class SoundDriverServices;
class SWSoundStream;
class CReverbAdjust;

#ifndef setHiLo
#define round8( val) ( ( ( val) + 7) & ~7)
#define round32( val) ( ( ( val) + 31) & ~31)

#define setHiLo( field, val) \
	field##Hi = (u16)( ( val) >> 16); \
	field##Lo = (u16)( ( val) & 0xFFFF);
#endif

#define SDRIVER_SWSTREAM_ARAMADDR			( 15 * 1024 * 1024)
#define SDRIVER_SWSTREAM_ARAMADDRADPCM		( 15 * 1024 * 1024 * 2)

/***************************************************************************/
// The sound driver class.
//
// Errors are returned via the sounderrorclass.  If you get a false out of
// one of these calls, use the GetLastError call to see what happened.
/***************************************************************************/

struct GCNVoice
{
	GCNVoice		*next;
	GCNVoice		*prev;
	AXVPB			*pAXVPB;
};

class SoundDriverGCN : public SoundDriver
{
public:
	// The usual
	SoundDriverGCN();
	~SoundDriverGCN();

	// Create it
	bool Create( OSCREATEINFO osInfo);

	// Destroy it (destructor does the same thing)
	bool Destroy();

	// Initialize the output format, will return false with NORMAL priority
	bool SetOutputFormat( unsigned int sampleRate, bool sixteenBit = true);

	// Get the current mode
	OUTPUTMODE GetOutputMode();

	// Set the mode, returns false if not supported
	bool SetOutputMode(OUTPUTMODE mode);

	// Test to see if this mode is supported
	bool OutputModeSupported(OUTPUTMODE mode);

	// What is the (approximate) max channels that can be simultaneously played?
	int MaxChannels();

	// Call this frequently.  Threads are nice, but messy
	void Daemon();

	// Pause all currently playing sounds
	void Pause(PauseType pauseType);

	// Resume all currently paused sounds
	void Unpause(PauseType pauseType);

	// Does this stream exist?
	bool StreamExist( const char *fileName);

	// Stream a platform specific file off of the disk!
	// DO NOT PUT AN EXTENSION ON THIS NAME!
	SoundStream *SetupStream( const char *fileName, SoundAttr &attr, bool repeat, bool bSoftwareStream = false);

	// Stream a platform specific file off of the disk w/3D
	// DO NOT PUT AN EXTENSION ON THIS NAME!
	// Some platforms may not support this at all
	SoundStream3D *SetupStream3D( const char *fileName, SoundAttr &attr, bool loop = false) { return NULL; }

	// Create a sound from a memory image (image for this driver is a .vag file)
	Sound *CreateSound( SoundBank &bank, void *data, int size);

	void PreloadStream( const char *pFilename);

	// Set the environmental effect
	bool SetEnvironmentEffect(ENVEFFECT effect, float volume);

	static inline void CompSRC( AXPBSRC *pAXPBSRC, float soundSampleRate);

	// What is the extension for sounds
	const char *FileFormat()
	{
		return(".dsp");
	};

	// What is the default stream extension
	const char * StreamFormat( void)
	{
		return( ".adp");
	}

	const char * StreamPreloadFormat( void)
	{
		return( FileFormat( ));
	}

	// What is our base path for sound?
	const char * BaseDataPath( void)
	{
		return( "sound\\");
	};

	// What is the base path for streams?
	const char * BaseStreamPath( void)
	{
		return( "data_gcn\\stream\\");
	}

	// What is the base path for streams?
	const char * MediaBaseStreamPath( void)
	{
		return( "stream\\");
	}

	u32 GetAramTOS( void)
	{
		return( aramTOS);
	}

	void ClearSWSoundStream( void)
	{
		m_pSWStream = NULL;
	}

	static SoundDriverGCN * GetSoundDriverGCN( void)
	{
		return( m_pSoundDriverGCN);
	}

	bool SetWetDry( AXVPB *pAXVPB, bool wet, bool dry);

protected:
	#define MAX_VOICES	AX_MAX_VOICES

	static void SoundDriverFrameCallback( void);

	SoundDriverServices	*services;		// Services (internal interface)

	static float			sampleRate;

	u32						aramBase,
								zeroBuffer,
								aramTOS,
								aramSize,
								aramStreamTOS;

	GCNVoice					myVoices[MAX_VOICES];

	// Our bank managment
	SoundBank *CreateBank();
	void DestroyBank(SoundBank *bank);

	GCNVoice * AllocateVoice( Sound *pSound);
	void DeallocateVoice( GCNVoice *pGCNVoice);

	u32 GetZeroBuffer( void)
	{
		return( zeroBuffer);
	}

	float GetSampleRate( void)
	{
		return sampleRate;
	}

	void SetReverbParams( void);

	ENVEFFECT				m_currentEffect;
	float						m_effectVolume;

	SWSoundStream			*m_pSWStream;

	static SoundDriverGCN *SoundDriverGCN::m_pSoundDriverGCN;

	struct ts_EffectMapper
	{
		// user params				
		f32	coloration;
		f32	mix;
		f32	time;
		f32	damping;
		f32	preDelay;
	};

	AXFX_REVERBSTD			m_axfxReverbStd;
	
	static ts_EffectMapper effectMap[ENVEFFECTS];

	// We must manage reclaimed sounds
	DoublyLinkedList<SoundInstance>	autoReclaim;
	DoublyLinkedList<GCNVoice>			activeVoices;
	DoublyLinkedList<GCNVoice>			freeVoices;

	friend class SoundDriverServices;
	friend class CReverbAdjust;
};

/***************************************************************************/
/***************************************************************************/
inline void SoundDriverGCN::CompSRC( AXPBSRC *pAXPBSRC, float soundSampleRate)
{

	float		fRatio = soundSampleRate / sampleRate;

	u32		iRatio = (u32)( fRatio * 65536.0f);

	setHiLo( pAXPBSRC->ratio, iRatio);
}

/***************************************************************************/
// Services for the Sound class.  This basically is a private interface to
// the innards to the SoundDriver
/***************************************************************************/
class SoundDriverServices
{
public:
	// Remove this sound when it has stopped playing
	void AutoReclaim( SoundInstance *instance);

	// Allocate a channel for this bad boy
	GCNVoice * AllocateVoice( Sound *pSound)
	{
		return( driver.AllocateVoice( pSound));
	}
	void DeallocateVoice( GCNVoice *pGCNVoice)
	{
		driver.DeallocateVoice( pGCNVoice);
	}

	float GetSampleRate( void)
	{
		return driver.GetSampleRate( );
	}

	u32 GetZeroBuffer( void)
	{
		return( driver.GetZeroBuffer( ));
	}

	bool SetWetDry( AXVPB *pAXVPB, bool wet, bool dry)
	{
		return( driver.SetWetDry( pAXVPB, wet, dry));
	}

private:
	// Only our friends can create us (SoundDriver)
	SoundDriverServices(SoundDriverGCN &parent) : driver(parent) {}
	~SoundDriverServices() {}
	friend class SoundDriverGCN;

	// Our internals
	SoundDriverGCN		&driver;
};

#endif


