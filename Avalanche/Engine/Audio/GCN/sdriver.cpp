/***************************************************************************/
// Sound driver
/***************************************************************************/

/******************* includes ****************************/

#include "Audio/AudioPCH.h"
#include "platform/gameapp.h"

//#define ADJUST_REVERB

#ifdef ADJUST_REVERB
#include "hud/hudlib.h"
#include "platform/systemi.h"
#include "Display/Common/DisplayState.h"

void ProcessReverbAdjust( void);

#endif

static void SoundDriverVoiceCallback( void *pVoid);

static AXPBSRC gDefaultSRC =
{
	1,		//src.ratioHi
   0,		//src.ratioLo
   0,		//src.currentAddressFrac
   0,		//src.last_samples[0]
   0,		//src.last_samples[1]
   0,		//src.last_samples[2]
   0		//src.last_samples[3]
};

float SoundDriverGCN::sampleRate = 32000.0f;

SoundDriverGCN *SoundDriverGCN::m_pSoundDriverGCN = NULL;

static void * axfxMemAlloc( u32 nBytes)
{
	MEM_SET_ALLOC_NAME( "AXFXReverb");
	return( memAlloc( nBytes, 32, MEM_FLG_PERMANENT));
}

static void axfxMemFree( void *pBuffer)
{
	memFree( pBuffer);
}

SoundDriverGCN::ts_EffectMapper SoundDriverGCN::effectMap[ENVEFFECTS] =
{
//		coloration		mix	reverbtime	damping	preDelay
	{	0.1,				0.0,	0.01,			0.0,		0.0}, 	// NONE
	{	0.1,				0.0,	0.01,			0.0,		0.0}, 	// DEFAULT,
	{	0.1,				0.0,	0.01,			0.1,		0.0}, 	// PADDEDCELL,
	{	0.1,				0.0,	0.01,			0.1,		0.0}, 	// ROOM,
	{	0.52,				0.7,	0.51,			0.34,		0.01}, 	// BATHROOM,
	{	0.1,				0.0,	0.01,			0.1,		0.0}, 	// LIVINGROOM,
	{	0.83,				0.5,	0.51,			1.0,		0.01},	// STONEROOM,
	{	0.1,				0.0,	0.01,			0.1,		0.0}, 	// AUDITORIUM,
	{	0.1,				0.0,	0.01,			0.1,		0.0}, 	// CONCERTHALL,
	{	0.96,				0.6,	2.86,			0.73,		0.07}, 	// CAVE,
	{	0.1,				0.0,	0.01,			0.1,		0.0}, 	// ARENA,
	{	0.95,				0.6,	1.86,			0.56,		0.05}, 	// HANGAR,
	{	1.0,				0.87,	0.16,			1.0,		0.0}, 	// CARPETEDHALLWAY,
	{	0.1,				0.0,	0.01,			0.1,		0.0}, 	// HALLWAY,
	{	0.96,				0.35,	1.96,			0.73,		0.03},	// STONECORRIDOR,
	{	0.1,				0.0,	0.01,			0.1,		0.0}, 	// ALLEY,
	{	0.81,				1.0,	0.51,			0.75,		0.03}, 	// FOREST,
	{	0.1,				0.0,	0.01,			0.1,		0.0},		// CITY,
	{	0.1,				0.0,	0.01,			0.1,		0.0},		// MOUNTAINS,
	{	0.83,				0.41,	0.51,			0.85,		0.02},	// QUARRY,
	{	0.1,				0.0,	0.01,			0.1,		0.0}, 	// PLAIN,
	{	0.1,				0.0,	0.01,			0.1,		0.0}, 	// PARKINGLOT,
	{	0.1,				0.0,	0.01,			0.1,		0.0}, 	// SEWERPIPE,
	{	0.1,				0.0,	0.01,			0.1,		0.0} 		// UNDERWATER
};

/***************************************************************************/
// All subclasses of sounddriver must write this function
// must be implemented it in SoundDriver:: namespace
/***************************************************************************/
SoundDriver *SoundDriver::CreateAndSetOutputFormat(
OSCREATEINFO osInfo)
{
	// Create the object
	SoundDriverGCN *driver = new SoundDriverGCN;
	if (!driver)
	{
		ASSERT(false);
		return NULL;
	}

	// Call it's create
	if (!driver->Create(osInfo))
	{
		ASSERT(false);
		delete driver;
		return NULL;
	}

	// Set the default/best output format
	if (!driver->SetOutputFormat(SAMPLE32KHz, true))
	{
		ASSERT(false);
		driver->Destroy();
		delete driver;
		return NULL;
	}

	return driver;
}

/***************************************************************************/
// Just init stuff
/***************************************************************************/
SoundDriverGCN::SoundDriverGCN() :
	autoReclaim( false), activeVoices( false), freeVoices( false)
{
	services = NULL;
	m_pSWStream = NULL;

	outputMode = GetOutputMode();
}

/***************************************************************************/
// Clean everything up
/***************************************************************************/
SoundDriverGCN::~SoundDriverGCN( void)
{
	Destroy();
}

void cppAXFXReverbStdCallback( void *pData, void *pContext)
{
	AXFXReverbStdCallback( (AXFX_BUFFERUPDATE *)pData, (AXFX_REVERBSTD *)pContext);
}

/***************************************************************************/
/* initialize the sound system                                             */
/***************************************************************************/
bool SoundDriverGCN::Create( OSCREATEINFO osInfo)
{
	m_pSoundDriverGCN = this;

	ARQInit( );
	AIInit( NULL);

	AXInit( );
	AXSetCompressor( AX_COMPRESSOR_OFF);
	MIXInit( );

	AXRegisterCallback( SoundDriverFrameCallback);

	AXFXSetHooks( axfxMemAlloc, axfxMemFree);

	m_currentEffect = NONE;

	aramBase = GameApp::IAm( )->GetARAMBase( );

	aramBase = round8( aramBase);

	zeroBuffer = aramBase;

	//set up DSP "zero buffer"
	char *pClear = (char *)memAlloc( 1024, 32);
	ASSERT( pClear != NULL);
	memset( pClear, 0, 1024);
	DCStoreRange( (void *)pClear, 1024);
	ARStartDMAWrite( (u32)pClear, zeroBuffer, 1024);
	while( ARGetDMAStatus( ));
	memFree( pClear);

	aramBase = zeroBuffer + 1024;				//leave plenty of room for dsp zero buffer

	aramTOS = aramBase;			//Initial point for aram stack

	aramSize = round8( ARGetSize( ));
	aramStreamTOS = aramSize;

	// Create our services
	services = new SoundDriverServices( *this);
	if( services == NULL)
		return false;

	for( int iList = 0; iList < MAX_VOICES; iList++)
	{
		myVoices[iList].pAXVPB = NULL;
		freeVoices.AddHead( myVoices + iList);
	}

	// init the hardware stream
	AISetStreamVolLeft( 0);
	AISetStreamVolRight( 0);
	AISetStreamPlayState( AI_STREAM_STOP);

	SoundError::SetLastError( SE_OK);
	return true;
}

/***************************************************************************/
// Destroy it (destructor does the same thing)
/***************************************************************************/
bool SoundDriverGCN::Destroy( void)
{
	if( services)
	{
		delete services;
		services = NULL;
	}

	return true;
}

/***************************************************************************/
// Initialize the output format, will return false with NORMAL priority
/***************************************************************************/
bool SoundDriverGCN::SetOutputFormat(	unsigned int desiredSampleRate,
													bool sixteenBit)
{
	if( desiredSampleRate != SAMPLE32KHz && desiredSampleRate != SAMPLE48KHz)
	{
		return false;
	}

	if( desiredSampleRate == SAMPLE32KHz)
	{
		sampleRate = 32000.0f;
		AISetDSPSampleRate( AI_SAMPLERATE_32KHZ);
	}
	else
	{
		sampleRate = 48000.0f;
		AISetDSPSampleRate( AI_SAMPLERATE_48KHZ);
	}

	// Make sure we get this from the ROM
	outputMode = GetOutputMode();
	SetOutputMode(outputMode);

	return true;
}

/***************************************************************************/
// Get the current mode
/***************************************************************************/
SoundDriver::OUTPUTMODE SoundDriverGCN::GetOutputMode()
{
	u32 soundMode = OSGetSoundMode();
	if (soundMode == OS_SOUND_MODE_MONO)
		return OUTPUTMODE_MONO;
	else
	{
		ASSERT(soundMode == OS_SOUND_MODE_STEREO);
		return OUTPUTMODE_STEREO;
	}
}

/***************************************************************************/
// Set the mode, returns false if not supported
/***************************************************************************/
bool SoundDriverGCN::SetOutputMode(
OUTPUTMODE mode)
{
	// Tell the parent
	if (!SoundDriver::SetOutputMode(mode))
		return false;

	// Tell the sound
	SoundInstance::SetOutputMode(outputMode);

	// Set it in the os
	if (mode == OUTPUTMODE_MONO)
	{
		OSSetSoundMode(OS_SOUND_MODE_MONO);
		MIXSetSoundMode( MIX_SOUND_MODE_MONO);
	}
	else
	{
		ASSERT(mode == OUTPUTMODE_STEREO);
		OSSetSoundMode(OS_SOUND_MODE_STEREO);
		MIXSetSoundMode( MIX_SOUND_MODE_STEREO);
	}

	return true;
}

/***************************************************************************/
// Test to see if this mode is supported
/***************************************************************************/
bool SoundDriverGCN::OutputModeSupported(
OUTPUTMODE mode)
{
	switch (mode)
	{
		case OUTPUTMODE_MONO:
		case OUTPUTMODE_STEREO:
			return true;

		case OUTPUTMODE_STEREOFLIPPED:
		case OUTPUTMODE_SURROUND:
		case OUTPUTMODE_DOLBY51:
		default:
			return false;
	}
}

/***************************************************************************/
// What is the (approximate) max channels that can be simultaneously played?
/***************************************************************************/
int SoundDriverGCN::MaxChannels()
{
	// This depends on how many streams they have, but this is a good
	// estimate.
	return( 45);
}

/***************************************************************************/
// The Daemon!
/***************************************************************************/
void SoundDriverGCN::Daemon()
{
	// Call the parent
	SoundDriver::Daemon();

	// Manage reclaimable sounds
	SoundInstance *instance = autoReclaim.Head( );
	while (instance)
	{
		SoundInstance *next = instance->next;
		if( !instance->IsPlaying( ))
		{
			autoReclaim.Remove( instance);
			delete instance;
		}
		instance = next;
	}

#ifdef ADJUST_REVERB
	ProcessReverbAdjust( );
#endif
}

/***************************************************************************/
// Does this stream exist?
/***************************************************************************/
bool SoundDriverGCN::StreamExist( const char *fileName)
{

	char streamFile[256];

	PathFile::MakePathForceExt( streamFile, MediaBaseStreamPath( ), fileName, StreamPreloadFormat( ));
	if( Media::FileExists( streamFile))
	{
		return( true);
	}

	PathFile::MakePathForceExt( streamFile, MediaBaseStreamPath( ), fileName, StreamFormat( ));

	return( Media::FileExists( streamFile));
}

/***************************************************************************/
// Stream a platform specific file off of the disk!
/***************************************************************************/
SoundStream *SoundDriverGCN::SetupStream(	const char *fileName,
														SoundAttr &attr,
														bool repeat,
														bool bSoftwareStream)
{

	char					streamFile[256];

	SoundStream 		*pNewStream;

	// Clean up the filename
	if( bSoftwareStream)
	{
		PathFile::MakePathForceExt( streamFile, BaseStreamPath( ), fileName, StreamPreloadFormat( ));
		m_pSWStream = new SWSoundStream( fileName, streamFile, repeat);
		pNewStream = m_pSWStream;
		m_pSWStream->m_pDriver = this;
	}
	else
	{
		PathFile::MakePathForceExt( streamFile, BaseStreamPath( ), fileName, StreamFormat( ));
		pNewStream = new HWSoundStream( streamFile, repeat);
	}

	if( !pNewStream->m_bStreamQueued)
	{
		delete pNewStream;
		if( bSoftwareStream)
		{
			m_pSWStream = NULL;
		}
		return NULL;
	}

	return pNewStream;
}

/***************************************************************************/
// Create a sound from a memory image (image for this driver is a .dsp file)
/***************************************************************************/
Sound *SoundDriverGCN::CreateSound(	SoundBank &bank, void *data, int size)
{

	ADPCM_Hdr		*pHdr;

	char				*pSamples;

	int				waveSize;

	int				aramAddr;

	pHdr = (ADPCM_Hdr *)data;

	waveSize = ( pHdr->num_adpcm_nibbles + 1) >> 1;
	waveSize = round32( waveSize);

	pSamples = (char *)( pHdr + 1);
	ASSERT( ( ( (u32)pSamples) & 31) == 0);

	DCStoreRange( pSamples, waveSize);
	DCInvalidateRange( pSamples, waveSize);

	aramAddr = aramTOS;
	aramTOS += waveSize;

	ARStartDMAWrite( (u32)pSamples, aramAddr, waveSize);

	// Create the sound
	SoundAttr attr(SoundAttr::VOLUMECONTROL | SoundAttr::PANCONTROL | SoundAttr::PITCHCONTROL, SAMPLE22KHz);
	attr.InferFromDSP( *pHdr);

	Sound *sound = new Sound( *services, *pHdr, aramAddr, attr);

	while( ARGetDMAStatus( ));

	return sound;
}

/***************************************************************************/
// Pause all currently playing sounds
/***************************************************************************/
void SoundDriverGCN::Pause( PauseType pauseType)
{
	// Call the parent
	SoundDriver::Pause( pauseType);

	if ((int)pauseType & PAUSESFX)
	{
		// Handle the auto-reclaim list
		SoundInstance *pSound = autoReclaim.Head( );
		while( pSound)
		{
			pSound->Pause( );
			pSound = pSound->next;
		}
	}
}

/***************************************************************************/
// Resume all currently paused sounds
/***************************************************************************/
void SoundDriverGCN::Unpause( PauseType pauseType)
{
	// Call the parent
	SoundDriver::Unpause( pauseType);

	if ((int)pauseType & PAUSESFX)
	{
		// Handle the auto-reclaim list
		SoundInstance *pSound = autoReclaim.Head( );
		while( pSound)
		{
			pSound->Unpause( );
			pSound = pSound->next;
		}
	}
}

/***************************************************************************/
// Our bank managment
/***************************************************************************/
SoundBank *SoundDriverGCN::CreateBank( void )
{
	// Return our platform specific bank
	return new SoundBankGCN( *this );
}

/***************************************************************************/
/***************************************************************************/
void SoundDriverGCN::DestroyBank( SoundBank *bank)
{
	ASSERT(bank);

	// Alias a GCN bank
	SoundBankGCN *gcnBank = (SoundBankGCN *)bank;

	// Free up the space on the heap
	aramTOS = gcnBank->BaseAddress( );

	// We really get to free this one!
	delete bank;
}

/***************************************************************************/
// Set the environmental effect
/***************************************************************************/
bool SoundDriverGCN::SetEnvironmentEffect( ENVEFFECT effect, float volume)
{
	m_effectVolume = volume;

	if( m_currentEffect == effect)
	{
		return( true);
	}

	if( m_currentEffect != NONE)
	{
		AXFXReverbStdShutdown( &m_axfxReverbStd);
		AXRegisterAuxACallback( NULL, NULL);
	}

	m_currentEffect = effect;

	if( m_currentEffect == NONE)
	{
		return( true);
	}

	SetReverbParams( );
	AXFXReverbStdInit( &m_axfxReverbStd);
	AXRegisterAuxACallback( cppAXFXReverbStdCallback, (void *)&m_axfxReverbStd);

	return( true);
}

bool SoundDriverGCN::SetWetDry( AXVPB *pAXVPB, bool wet, bool dry)
{
	if( pAXVPB == NULL)
	{
		return( true);
	}

	if( m_currentEffect == NONE)
	{
		MIXSetAuxA( pAXVPB, SoundInstance::MINVOLUME);
		return( true);
	}

	if( wet)
	{
		MIXSetAuxA( pAXVPB, SoundInstance::HectodBToGCNdB( SoundVolume::UnitToHectodB( m_effectVolume)));
	}
	else
	{
		MIXSetAuxA( pAXVPB, SoundInstance::MINVOLUME);
	}

	if( !dry)
	{
		MIXSetInput( pAXVPB, SoundInstance::MINVOLUME);
	}

	return( true);
}

void SoundDriverGCN::SetReverbParams( void)
{
	m_axfxReverbStd.tempDisableFX = false;
	m_axfxReverbStd.coloration = effectMap[m_currentEffect].coloration;
	m_axfxReverbStd.mix = effectMap[m_currentEffect].mix;
	m_axfxReverbStd.time = effectMap[m_currentEffect].time;
	m_axfxReverbStd.damping = effectMap[m_currentEffect].damping;
	m_axfxReverbStd.preDelay = effectMap[m_currentEffect].preDelay;
}

/***************************************************************************/
/***************************************************************************/
GCNVoice * SoundDriverGCN::AllocateVoice( Sound *pSound)
{

	GCNVoice			*pNewVoice;

	AXVPB				*pAXVPB;

	AXPBADDR			axpbaddr;

	pNewVoice = freeVoices.Head( );

	if( pNewVoice == NULL)
	{
		return NULL;
	}

	pAXVPB = AXAcquireVoice( AX_PRIORITY_LOWEST, SoundDriverVoiceCallback, (u32)pNewVoice);

	if( pAXVPB == NULL)
	{
		return NULL;
	}

	freeVoices.Unlink( pNewVoice);

	pNewVoice->pAXVPB = pAXVPB;

	CompSRC( &gDefaultSRC, pSound->attr.SampleRate( ));

   MIXInitChannel(	pAXVPB,
   						MIX_MODE_AUXA_PREFADER,
							SoundInstance::HectodBToGCNdB( SoundVolume::UnitToHectodB( SoundInstance::GetMasterVolume( ))),
							SoundInstance::MINVOLUME,
   						SoundInstance::MINVOLUME,
   						SoundInstance::PANCENTER,
   						SoundInstance::SPANFRONT,
   						0);
	AXSetVoiceAddr( pAXVPB, &pSound->myAXPBADDR);
	AXSetVoiceSrcType( pAXVPB, AX_SRC_TYPE_LINEAR);
	AXSetVoiceSrc( pAXVPB, &gDefaultSRC);
	AXSetVoiceAdpcm( pAXVPB, &pSound->myAXPBADPCM);
	AXSetVoiceAdpcmLoop( pAXVPB, &pSound->myAXPBADPCMLOOP);
	AXSetVoiceState( pAXVPB, AX_PB_STATE_STOP);

	activeVoices.AddHead( pNewVoice);

	return pNewVoice;
}

/***************************************************************************/
/***************************************************************************/
void SoundDriverGCN::DeallocateVoice( GCNVoice *pGCNVoice)
{

	AXVPB				*pAXVPB;

	pAXVPB = pGCNVoice->pAXVPB;

	if( pAXVPB != NULL)
	{
		MIXReleaseChannel( pAXVPB);
		AXFreeVoice( pAXVPB);

		pGCNVoice->pAXVPB = NULL;
	}

	activeVoices.Unlink( pGCNVoice);

	freeVoices.AddHead( pGCNVoice);
}

/***************************************************************************/
// Remove this sound when it has stopped playing
/***************************************************************************/
void SoundDriverServices::AutoReclaim(	SoundInstance *instance)
{
	// Duh
	ASSERT(instance);

	// Just add it to our list
	driver.autoReclaim.AddTail(instance);
}

void SoundDriverGCN::SoundDriverFrameCallback( void)
{

	int		old = OSEnableInterrupts( );

	MIXUpdateSettings( );

	if( m_pSoundDriverGCN != NULL)
	{
		if( SoundDriverGCN::GetSoundDriverGCN( )->m_pSWStream != NULL)
		{
			SWSoundStream		*pStream = SoundDriverGCN::GetSoundDriverGCN( )->m_pSWStream;

			u32					currentPosition = *( (u32 *)&pStream->m_pAXVPB->pb.addr.currentAddressHi);

			u16 state = AX_PB_STATE_STOP;

			if( pStream->m_pAXVPB != NULL)
			{
				state = pStream->m_pAXVPB->pb.state;
			}

			if( state == AX_PB_STATE_RUN && currentPosition < pStream->m_StreamLastPos)
			{
				//we just finished buffer 1, if it was last buffer, we're done
				if( pStream->m_sb[1].bLastBufferDMA)
				{
					pStream->m_bPlaying = false;
				}
				//we have wrapped around, load buffer 1
				SWSoundStream::StartDMA( pStream->m_sb + 1);
			}
			else
			if(	( state == AX_PB_STATE_RUN) &&
					( currentPosition >= ( pStream->m_sb[1].aramAddr * 2)) &&
					( pStream->m_StreamLastPos < ( pStream->m_sb[1].aramAddr * 2)))
			{
				//we just finished buffer 0, if it was last buffer, we're done
				if( pStream->m_sb[0].bLastBufferDMA)
				{
					pStream->m_bPlaying = false;
				}
				//we have read past the first buffer, load buffer 0
				SWSoundStream::StartDMA( pStream->m_sb + 0);
			}
			if( state == AX_PB_STATE_RUN)
			{
				if( currentPosition < pStream->m_StreamLastPos)
				{
					pStream->m_SamplesPlayed += ( AUDIO_BUFFER_LENGTH - pStream->m_StreamLastPos) + ( currentPosition - pStream->m_sb[0].aramAddr);
				}
				else
				{
					pStream->m_SamplesPlayed += ( currentPosition - pStream->m_StreamLastPos);
				}
			}

			pStream->m_StreamLastPos = currentPosition;
		}
	}

	OSRestoreInterrupts( old);
}

static void SoundDriverVoiceCallback( void *pVoid)
{

	AXVPB				*pAXVPB;

	GCNVoice			*pGCNVoice;

	pAXVPB = (AXVPB *)pVoid;

	pGCNVoice = (GCNVoice *)pAXVPB->userContext;

	MIXReleaseChannel( pAXVPB);
	AXFreeVoice( pAXVPB);
	pGCNVoice->pAXVPB = NULL;
}

#ifdef ADJUST_REVERB

static char *apEffectNames[SoundDriver::ENVEFFECTS] = 
{
	"NONE",
	"DEFAULT",
	"PADDEDCELL",
	"ROOM",
	"BATHROOM",
	"LIVINGROOM",
	"STONEROOM",
	"AUDITORIUM",
	"CONCERTHALL",
	"CAVE",
	"ARENA",
	"HANGAR",
	"CARPETEDHALLWAY",
	"HALLWAY",
	"STONECORRIDOR",
	"ALLEY",
	"FOREST",
	"CITY",
	"MOUNTAINS",
	"QUARRY",
	"PLAIN",
	"PARKINGLOT",
	"SEWERPIPE",
	"UNDERWATER"
};

class CReverbParam
{
	public:
		typedef enum
		{
			REVERBPARAM_EFFECTNAME,
			REVERBPARAM_COLORATION,
			REVERBPARAM_MIX,
			REVERBPARAM_REVERBTIME,
			REVERBPARAM_DAMPING,
			REVERBPARAM_PREDELAY,
			REVERBPARAM_DEPTH,
			MAX_REVERBPARAMS
		} REVERBPARAM;

		CReverbParam( REVERBPARAM paramType)
		{
			m_paramType = paramType;

			switch( m_paramType)
			{
				case REVERBPARAM_EFFECTNAME:
					m_paramName = "Effect Name";
					m_minValue = 0.0f;
					m_maxValue = SoundDriver::ENVEFFECTS - 1.0f;
					m_paramDelta = 1.0f;
					break;

				case REVERBPARAM_COLORATION:
					m_paramName = "Coloration";
					m_minValue = 0.0f;
					m_maxValue = 1.0f;
					m_paramDelta = 0.01f;
					break;

				case REVERBPARAM_MIX:
					m_paramName = "Mix";
					m_minValue = 0.0f;
					m_maxValue = 1.0f;
					m_paramDelta = 0.01f;
					break;
					
				case REVERBPARAM_REVERBTIME:
					m_paramName = "Reverb Time";
					m_minValue = 0.01f;
					m_maxValue = 10.0f;
					m_paramDelta = 0.05f;
					break;
					
				case REVERBPARAM_DAMPING:
					m_paramName = "Damping";
					m_minValue = 0.0f;
					m_maxValue = 1.0f;
					m_paramDelta = 0.01f;
					break;
					
				case REVERBPARAM_PREDELAY:
					m_paramName = "Predelay";
					m_minValue = 0.0f;
					m_maxValue = 0.1f;
					m_paramDelta = 0.01f;
					break;
					
				case REVERBPARAM_DEPTH:
					m_paramName = "Depth";
					m_minValue = 0.0f;
					m_maxValue = 1.0f;
					m_paramDelta = 0.01f;
					break;
					
				default:
					ASSERT( 0);
			}

			m_paramValue = m_minValue;
		}

		const char *ParamName( void)
		{
			return( m_paramName);
		}

		float ParamValue( void)
		{
			return( m_paramValue);
		}

		float ParamDelta( void)
		{
			return( m_paramDelta);
		}

		void SetParam( float value)
		{
			m_paramValue = value;
		}

		void AdjustParamValue( float delta)
		{
			m_paramValue += delta * m_paramDelta;
			if( m_paramValue < m_minValue)
			{
				m_paramValue = m_minValue;
			}
			else
			if( m_paramValue > m_maxValue)
			{
				m_paramValue = m_maxValue;
			}
		}
		
	protected:
		
	private:
		REVERBPARAM		m_paramType;
		const char		*m_paramName;

		float				m_minValue;
		float				m_maxValue;
		float				m_paramDelta;
		float				m_paramValue;
};

class CReverbAdjust
{
	public:
		CReverbAdjust( )
		{
			m_currParam = CReverbParam::REVERBPARAM_EFFECTNAME;

			m_window.SetTextureSet( Font::SystemFont( )->m_texture.handle, TS_NO_TEXTURE);
			m_window.SetVertexColor( 0, 0.0f, 0.0f, 0.0f, 0.35f);
			m_window.SetVertexColor( 1, 0.0f, 0.0f, 0.0f, 0.35f);
			m_window.SetVertexColor( 2, 0.0f, 0.0f, 0.0f, 0.35f);
			m_window.SetVertexColor( 3, 0.0f, 0.0f, 0.0f, 0.35f);
			m_window.SetPosition( 0.0f, 0.0f);
			m_window.SetWidth( Display__GetScreenWidth( ));
			m_window.SetHeight( Display__GetScreenHeight( ));
			m_window.SetPriority( 0xf00d);

			for( int i = 0; i < CReverbParam::MAX_REVERBPARAMS; i++)
			{
				m_apReverbParams[i] = new CReverbParam( (CReverbParam::REVERBPARAM)i);

				m_window.AddChild( m_reverbValue + i);
				m_reverbValue[i].SetPosition( 60.00f, 50.0f + ( i * 30.0f));
				m_reverbValue[i].SetColor( 1.0f, 1.0f, 1.0f, 1.0f);
				m_reverbValue[i].SetPriority( 1);
				UpdateValueDisplay( i);
			}

			m_bDirty = true;
			AdjustCurrParam( 0);
		}

		void UpdateValueDisplay( int param)
		{
			
			char		strValue[100];
			
			if( param == CReverbParam::REVERBPARAM_EFFECTNAME)
			{
				sprintf( strValue, "%s: %s", m_apReverbParams[param]->ParamName( ), apEffectNames[(int)m_apReverbParams[param]->ParamValue( )]);
			}
			else
			{
				sprintf( strValue, "%s: %4.2f", m_apReverbParams[param]->ParamName( ), m_apReverbParams[param]->ParamValue( ));
			}

			m_reverbValue[param].SetText( strValue);
			m_reverbValue[param].Update( );
		}

		void AdjustCurrParam( int delta)
		{
			m_currParam += delta;
			if( m_currParam < 0)
			{
				m_currParam = 0;
			}
			else
			if( m_currParam >= CReverbParam::MAX_REVERBPARAMS)
			{
				m_currParam = CReverbParam::MAX_REVERBPARAMS - 1;
			}

			for( int i = 0; i < CReverbParam::MAX_REVERBPARAMS; i++)
			{
				SetCaptionColor( i, false);
			}
			SetCaptionColor( m_currParam, true);

			m_window.Update( );
		}

		void AdjustParamValue( float delta)
		{
			m_apReverbParams[m_currParam]->AdjustParamValue( delta);
			UpdateValueDisplay( m_currParam);
			if( m_currParam == CReverbParam::REVERBPARAM_EFFECTNAME)
			{
				Restore( );
			}
			else
			{
				m_bDirty = true;
			}
			AdjustCurrParam( 0);
		}

		void SetCaptionColor( int param, bool bActive)
		{
			if( bActive)
			{
				m_reverbValue[param].SetColor( 0.50f, 1.0f, 0.50f, 1.0f);
			}
			else
			if( m_bDirty)
			{
				m_reverbValue[param].SetColor( 1.0f, 1.0f, 0.25f, 1.0f);
			}
			else
			{
				m_reverbValue[param].SetColor( 1.0f, 1.0f, 1.0f, 1.0f);
			}
		}

		void Show( void)
		{
			m_window.ShowAll( );
			m_window.Update( );
		}

		void Hide( void)
		{
			m_window.HideAll( );
			m_window.Update( );
		}

		void Apply( void)
		{

			int	effect = (int)m_apReverbParams[CReverbParam::REVERBPARAM_EFFECTNAME]->ParamValue( );

			SoundDriverGCN::effectMap[effect].coloration = m_apReverbParams[CReverbParam::REVERBPARAM_COLORATION]->ParamValue( );
			SoundDriverGCN::effectMap[effect].mix = m_apReverbParams[CReverbParam::REVERBPARAM_MIX]->ParamValue( );
			SoundDriverGCN::effectMap[effect].time = m_apReverbParams[CReverbParam::REVERBPARAM_REVERBTIME]->ParamValue( );
			SoundDriverGCN::effectMap[effect].damping = m_apReverbParams[CReverbParam::REVERBPARAM_DAMPING]->ParamValue( );
			SoundDriverGCN::effectMap[effect].preDelay = m_apReverbParams[CReverbParam::REVERBPARAM_PREDELAY]->ParamValue( );

			SoundDriverGCN::GetSoundDriverGCN( )->SetEnvironmentEffect( SoundDriver::NONE, m_apReverbParams[CReverbParam::REVERBPARAM_DEPTH]->ParamValue( ));
			SoundDriverGCN::GetSoundDriverGCN( )->SetEnvironmentEffect( (SoundDriver::ENVEFFECT)effect, m_apReverbParams[CReverbParam::REVERBPARAM_DEPTH]->ParamValue( ));
			m_bDirty = false;
			AdjustCurrParam( 0);
		}
		
		void Restore( bool bRestoreEffect = false)
		{
			
			int	effect;

			if( bRestoreEffect)
			{
				effect = SoundDriverGCN::GetSoundDriverGCN( )->m_currentEffect;
			}
			else
			{
				effect = (int)m_apReverbParams[CReverbParam::REVERBPARAM_EFFECTNAME]->ParamValue( );
			}
			
			m_apReverbParams[CReverbParam::REVERBPARAM_EFFECTNAME]->SetParam( effect);
			m_apReverbParams[CReverbParam::REVERBPARAM_COLORATION]->SetParam( SoundDriverGCN::effectMap[effect].coloration);
			m_apReverbParams[CReverbParam::REVERBPARAM_MIX]->SetParam( SoundDriverGCN::effectMap[effect].mix);
			m_apReverbParams[CReverbParam::REVERBPARAM_REVERBTIME]->SetParam( SoundDriverGCN::effectMap[effect].time);
			m_apReverbParams[CReverbParam::REVERBPARAM_DAMPING]->SetParam( SoundDriverGCN::effectMap[effect].damping);
			m_apReverbParams[CReverbParam::REVERBPARAM_PREDELAY]->SetParam( SoundDriverGCN::effectMap[effect].preDelay);
			
			m_apReverbParams[CReverbParam::REVERBPARAM_DEPTH]->SetParam( SoundDriverGCN::GetSoundDriverGCN( )->m_effectVolume);

			for( int i = 0; i < CReverbParam::MAX_REVERBPARAMS; i++)
			{
				UpdateValueDisplay( i);
			}

			m_bDirty = false;
			AdjustCurrParam( 0);
		}
		
	private:
		CReverbParam						*m_apReverbParams[CReverbParam::MAX_REVERBPARAMS];
		Text									m_window;
		Text									m_reverbValue[CReverbParam::MAX_REVERBPARAMS];

		int									m_currParam;
		bool									m_bDirty;
};

static CReverbAdjust		*pReverbAdjust = NULL;

static bool					bHidden = true;

static float				fKeyTimer = 0.0f;

#define KEYS_PER_SEC(keys) ( 1.0f / (float)( keys))

void ProcessReverbAdjust( void)
{
	ControllerInputDriver *pController = g_input.GetController( 3);

	Math::MoveToZero( &fKeyTimer, g_timer.GetOSFrameSec( ));
	if( pController != NULL)
	{
		if( pController->ButtonPressed( CB_R1))
		{
			if( pReverbAdjust != NULL)
			{
				delete pReverbAdjust;
				pReverbAdjust = NULL;
			}
		}
		else
		if( pController->ButtonPressed( CB_L1))
		{
			fKeyTimer = 0.0f;
			bHidden = !bHidden;
			if( bHidden)
			{
				if( pReverbAdjust != NULL)
				{
					pReverbAdjust->Hide( );
				}
			}
			else
			{
				if( pReverbAdjust == NULL)
				{
					pReverbAdjust = new CReverbAdjust;
					pReverbAdjust->Restore( true);
				}
				if( pReverbAdjust != NULL)
				{
					pReverbAdjust->AdjustCurrParam( 0);
					pReverbAdjust->Show( );
				}
			}
		}
		else
		if( pController->ButtonPressed( CB_UP))
		{
			fKeyTimer = 0.0f;
			if( pReverbAdjust != NULL)
			{
				pReverbAdjust->AdjustCurrParam( -1);
			}
		}
		else
		if( pController->ButtonPressed( CB_DOWN))
		{
			fKeyTimer = 0.0f;
			if( pReverbAdjust != NULL)
			{
				pReverbAdjust->AdjustCurrParam( 1);
			}
		}
		else
		if( pController->ButtonDown( CB_LEFT))
		{
			if( pReverbAdjust != NULL)
			{
				if( Math::Zero( fKeyTimer))
				{
					pReverbAdjust->AdjustParamValue( -1.0f);
					fKeyTimer = KEYS_PER_SEC( 5);
				}
			}
		}
		else
		if( pController->ButtonDown( CB_RIGHT))
		{
			if( pReverbAdjust != NULL)
			{
				if( Math::Zero( fKeyTimer))
				{
					pReverbAdjust->AdjustParamValue( 1.0f);
					fKeyTimer = KEYS_PER_SEC( 5);
				}
			}
		}
		else
		if( pController->ButtonPressed( CB_X))
		{
			if( pReverbAdjust != NULL)
			{
				pReverbAdjust->Apply( );
			}
		}
		else
		if( pController->ButtonPressed( CB_SQUARE))
		{
			if( pReverbAdjust != NULL)
			{
				pReverbAdjust->Restore( true);
			}
		}
	}
}

#endif //ADJUST_REVERB