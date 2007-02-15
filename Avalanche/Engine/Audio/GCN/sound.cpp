/***************************************************************************/
// Sound and Sound Instance stuff
/***************************************************************************/
#include "Audio/AudioPCH.h"

// statics
const int SoundInstance::MAXVOLUME = 60;		//+6 dB in 0.1 units
const int SoundInstance::DEFVOLUME = 0;		//no volume attenuation
const int SoundInstance::MINVOLUME = -904;	//-90.4 dB in 0.1 units
const int SoundInstance::PANHARDLEFT =	0;
const int SoundInstance::PANHARDRIGHT = 127;
const int SoundInstance::PANCENTER = 64;
const int SoundInstance::SPANSURROUND = 0;
const int SoundInstance::SPANCENTER = 64;
const int SoundInstance::SPANFRONT = 127;
const int SoundInstance::MAXFREQUENCY = 100000;
const int SoundInstance::MINFREQUENCY = 0;
const int SoundInstance::ORIGINALFREQUENCY =	-1;

// Overrides
float SoundInstance::masterVolume = 1.0f;
float SoundInstance::masterPitch = 0.0f;
float SoundInstance::masterPan = 1.0f;

/***************************************************************************/
// Set the overall volume for all sound
/***************************************************************************/
void SoundInstance::SetMasterVolume(
float volume)
{
	masterVolume = Math::Clamp(volume);
}

/***************************************************************************/
// Get the overall volume for all sound
/***************************************************************************/
float SoundInstance::GetMasterVolume()
{
	return masterVolume;
}

/***************************************************************************/
// Set the overall pitch for all sounds
/***************************************************************************/
void SoundInstance::SetMasterPitch(
float pitch)
{
	masterPitch = pitch;
}

/***************************************************************************/
// Get the overall pitch for all sound
/***************************************************************************/
float SoundInstance::GetMasterPitch()
{
	return masterPitch;
}

/***************************************************************************/
/***************************************************************************/
void SoundInstance::SetOutputMode(
SoundDriver::OUTPUTMODE mode)
{
	switch (mode)
	{
		case SoundDriver::OUTPUTMODE_MONO:
			masterPan = 0.0f;
			break;
		case SoundDriver::OUTPUTMODE_STEREO:
			masterPan = 1.0f;
			break;
		case SoundDriver::OUTPUTMODE_STEREOFLIPPED:
			masterPan = -1.0f;
			break;
		case SoundDriver::OUTPUTMODE_SURROUND:
		case SoundDriver::OUTPUTMODE_DOLBY51:
			masterPan = 1.0f;
			break;
	}
}

/***************************************************************************/
// Play the sound
/***************************************************************************/
bool SoundInstance::Play( bool loop)
{
	//see if sound got pulled out from underneath us
	if( pGCNVoice->pAXVPB == NULL)
	{
		return true;
	}
	if( loop)
	{
		ASSERT( pGCNVoice->pAXVPB->pb.addr.loopFlag);
		AXSetVoicePriority( pGCNVoice->pAXVPB, AX_PRIORITY_NODROP);
	}

	AXSetVoiceState( pGCNVoice->pAXVPB, AX_PB_STATE_RUN);
	return true;
}

/***************************************************************************/
// Stop playing the sound
/***************************************************************************/
bool SoundInstance::Stop( bool rewind)
{
	//see if sound got pulled out from underneath us
	if( pGCNVoice->pAXVPB == NULL)
	{
		return true;
	}
	AXSetVoiceState( pGCNVoice->pAXVPB, AX_PB_STATE_STOP);
	return true;
}

/***************************************************************************/
// Is this sound instance currently playing
/***************************************************************************/
bool SoundInstance::IsPlaying( void)
{
	if( pGCNVoice->pAXVPB == NULL)		//voice got pull out from under us
	{
		return false;
	}

	if( !paused && pGCNVoice->pAXVPB->pb.state == AX_PB_STATE_STOP)
	{
		return false;
	}

	return true;
}

/***************************************************************************/
// Rewind the sound (no effect if playing)
/***************************************************************************/
bool SoundInstance::Rewind()
{
	return true;
}

/***************************************************************************/
// Pause this sound
/***************************************************************************/
bool SoundInstance::Pause()
{
	//see if sound got pulled out from underneath us
	if( pGCNVoice->pAXVPB == NULL)
	{
		paused = false;
		return( true);
	}

	//if voice has already stopped, don't pause
	if( pGCNVoice->pAXVPB->pb.state == AX_PB_STATE_STOP)
	{
		return( true);
	}

	// set this state
	paused = true;

	AXSetVoiceState( pGCNVoice->pAXVPB, AX_PB_STATE_STOP);

	return true;
}

/***************************************************************************/
// Unpause this sound
/***************************************************************************/
bool SoundInstance::Unpause()
{
	// clear the state
	if( paused)
	{
		paused = false;
		
		AXVPB	*pAXVPB = pGCNVoice->pAXVPB;

		//see if sound got pulled out from underneath us
		if( pAXVPB == NULL)
		{
			return true;
		}
		
		//only set to run if not in zero buffer, which is always less than
		//start position in ARAM
		u32	currentPosition = *( (u32 *)&pAXVPB->pb.addr.currentAddressHi);
		u32	startPosition = sound->AramAddr( );

		if( currentPosition >= startPosition)
		{
			AXSetVoiceState( pGCNVoice->pAXVPB, AX_PB_STATE_RUN);
		}
	}

	return true;
}

/***************************************************************************/
/***************************************************************************/
bool SoundInstance::Paused()
{
	return paused;
}

/***************************************************************************/
// Set the volume [0..1], 0.0 is silent 1.0 is full volume
/***************************************************************************/
bool SoundInstance::SetVolume( float newVolume)
{
	volume = newVolume;
	//see if sound got pulled out from underneath us
	if( pGCNVoice->pAXVPB == NULL)
	{
		return true;
	}
	newVolume *= masterVolume;

	int dbVolume = SoundInstance::HectodBToGCNdB( SoundVolume::UnitToHectodB( newVolume));

	MIXSetInput(	pGCNVoice->pAXVPB, dbVolume);
	return true;
}

/***************************************************************************/
// Get the current volume
/***************************************************************************/
float SoundInstance::GetVolume()
{
	return volume;
}

/***************************************************************************/
// Set attenuation in dB, from -100db to 0db.
// Note that this changes the volume...just like SetVolume so don't call both
/***************************************************************************/
bool SoundInstance::SetAttenuationdB( float dB)
{
	return SetVolume( SoundVolume::dBToUnit( dB));
}

/***************************************************************************/
// Panning [-1..1] (-1.0 is hard left, 1.0 is hard right)
/***************************************************************************/
bool SoundInstance::SetPan( float newPan)
{
	pan = newPan;

	//see if sound got pulled out from underneath us
	if( pGCNVoice->pAXVPB == NULL)
	{
		return true;
	}
	MIXSetPan( pGCNVoice->pAXVPB, int( ( pan * masterPan * PANCENTER) + PANCENTER));

	return true;
}

/***************************************************************************/
// Get the current pan
/***************************************************************************/
float SoundInstance::GetPan()
{
	return pan;
}

/***************************************************************************/
// Volume and Pan combined
/***************************************************************************/
bool SoundInstance::SetVolumeAndPan(
float volume,
float pan)
{
	bool result;
	result  = SetVolume( volume );
	result &= SetPan( pan );

	return result;
}

/***************************************************************************/
// Set the pitch shift, given in semitones.  0.0 plays the sound at
// the original pitch (see semitone.h for helpful hints)
/***************************************************************************/
bool SoundInstance::SetPitch(
float semiTones)
{
	pitchShift = semiTones;
	int frequency = sound->Attr().SampleRate();
	int shift = SemitoneMath::ModifyFrequencyBySemitones(frequency, semiTones + masterPitch);
	shift = Math::Clamp(shift, MINFREQUENCY, MAXFREQUENCY);
	return SetFrequency(shift);
}

/***************************************************************************/
/***************************************************************************/
float SoundInstance::GetPitch()
{
	return pitchShift;
}

/***************************************************************************/
// Frequency (in Hz)
/***************************************************************************/
bool SoundInstance::SetFrequency( int newFrequency)
{
	frequency = newFrequency;

	//see if sound got pulled out from underneath us
	if( pGCNVoice->pAXVPB == NULL)
	{
		return true;
	}
	AXSetVoiceSrcRatio( pGCNVoice->pAXVPB, (float)frequency / sound->Driver( ).GetSampleRate( ));

	return true;
}

/***************************************************************************/
// Get the current frequency
/***************************************************************************/
int SoundInstance::GetFrequency()
{
	return frequency;
}

bool SoundInstance::SetWetDry( float wet, float dry)
{
	m_bWet = wet > 0.0f;
	m_bDry = dry > 0.0f;
	
	sound->Driver( ).SetWetDry( pGCNVoice->pAXVPB, m_bWet, m_bDry);
	
	return( true);
}

/***************************************************************************/
// What is our (approximate) play time (in ms)
/***************************************************************************/
unsigned int SoundInstance::PlayTime( void)
{
	return( 0);
//	return sound->Attr( ).PlayTimeMS( frequency);
}

/***************************************************************************/
// Someone else creates and manages us
/***************************************************************************/
SoundInstance::SoundInstance(Sound *parentSound, GCNVoice *pNewGCNVoice)
{
	paused = false;
	volume = DEFVOLUME;
	pan = PANCENTER;
	frequency = parentSound->Attr().SampleRate();
	sound = parentSound;
	pGCNVoice = pNewGCNVoice;
	userData = 0;
}

/***************************************************************************/
// Someone else creates and manages us
/***************************************************************************/
SoundInstance::SoundInstance(SoundData *parentSound, GCNVoice *pNewGCNVoice)
{
	paused = false;
	volume = DEFVOLUME;
	pan = PANCENTER;
	frequency = parentSound->Attr().SampleRate();
	soundData = parentSound;
	pGCNVoice = pNewGCNVoice;
	userData = 0;
}

/***************************************************************************/
// Clean up stuff
/***************************************************************************/
SoundInstance::~SoundInstance()
{
	sound->Driver().DeallocateVoice( pGCNVoice);
}

/***************************************************************************/
// Create free instances other than the root
/***************************************************************************/
SoundInstance *Sound::NewInstance( const char *name)
{
	// Allocate a voice
	GCNVoice *pGCNVoice = driver.AllocateVoice( this);
	if( pGCNVoice == NULL)
	{
		return NULL;
	}

	// make a new instance
	SoundInstance *pInstance = new SoundInstance( this, pGCNVoice);
	if( !pInstance)
	{
		// Deallocate channel
		driver.DeallocateVoice( pGCNVoice);
		return NULL;
	}

	// Add it to the proper list
	if( attr.AutoReclaim( ))
	{
		driver.AutoReclaim( pInstance);
	}
	else
	{
		instances.AddHead( pInstance);
	}

	return pInstance;
}

/***************************************************************************/
// Free an instance
/***************************************************************************/
void Sound::FreeInstance( SoundInstance *instance)
{
	// Free it
	ASSERT( instance->sound == this);
	instances.Remove( instance);
}

/***************************************************************************/
// Pause this sound instances
/***************************************************************************/
bool Sound::Pause()
{
	SoundInstance *inst = instances.Head();
	while (inst)
	{
		inst->Pause();
		inst = inst->next;
	}

	return true;
}

/***************************************************************************/
// Unpause this sound instances
/***************************************************************************/
bool Sound::Unpause()
{
	SoundInstance *inst = instances.Head();
	while (inst)
	{
		inst->Unpause();
		inst = inst->next;
	}

	return true;
}

/***************************************************************************/
// The sound interface
/***************************************************************************/
SoundData::SoundData(	SoundDriverServices &driverServices,
								ADPCM_Hdr &adpcmHdr,
								u32 aramAddr,
								SoundAttr &bufferAttr) : driver(driverServices)
{
	attr = bufferAttr;

	aramAddr <<= 1;			//make nibble address

	myAramAddr = aramAddr;

	myAXPBADDR.loopFlag = adpcmHdr.loop_flag;
	myAXPBADDR.format = AX_PB_FORMAT_ADPCM;
	if( adpcmHdr.loop_flag != 0)
	{
		setHiLo( myAXPBADDR.loopAddress, adpcmHdr.sa + aramAddr);
	}
	else
	{
		setHiLo( myAXPBADDR.loopAddress, ( driver.GetZeroBuffer( ) << 1) + 2);
	}
	setHiLo( myAXPBADDR.endAddress, adpcmHdr.ea + aramAddr);
	setHiLo( myAXPBADDR.currentAddress, adpcmHdr.ca + aramAddr);

	memcpy( myAXPBADPCM.a, adpcmHdr.coef, sizeof adpcmHdr.coef);
	myAXPBADPCM.gain = adpcmHdr.gain;
	myAXPBADPCM.pred_scale = adpcmHdr.ps;
	myAXPBADPCM.yn1 = adpcmHdr.yn1;
	myAXPBADPCM.yn2 = adpcmHdr.yn2;

	myAXPBADPCMLOOP.loop_pred_scale = adpcmHdr.lps;
	myAXPBADPCMLOOP.loop_yn1 = adpcmHdr.lyn1;
	myAXPBADPCMLOOP.loop_yn2 = adpcmHdr.lyn2;
}

/***************************************************************************/
// Clean stuff up
/***************************************************************************/
SoundData::~SoundData()
{
}
