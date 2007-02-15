/***************************************************************************/
// SoundStream, a disk or i/o based sound
/***************************************************************************/
#include "Audio/AudioPCH.h"

/***************************************************************************/
// The driver must create the sound, but it can be deleted directly
/***************************************************************************/
SoundStreamPS2::~SoundStreamPS2()
{
	SetVolume(0.0f);
 	MultiStream::DestroyStream(stream);
}

/***************************************************************************/
// Start the sound
/***************************************************************************/
bool SoundStreamPS2::Play()
{
	// First time play has been called?
	if (!queued)
	{
		MIBStream::Volume	volume[2];
		ComputeVolume(volume);
		stream->Start(volume, stream->Channels());

		// Debug
		stream->Debug();

		queued = true;
		return true;
	}

	// Are we paused?
	if (!paused)
		return true;

	// Set the proper pitch
	stream->SetPitch(stream->BasePitch());

	// Set the volume correctly
	SetVolume(m_volume);

	// We aren't paused
	paused = false;

	return true;
}

/***************************************************************************/
// Pause the sound
/***************************************************************************/
bool SoundStreamPS2::Stop(bool rewind)
{
	// We don't support rewind
	ASSERT(!rewind);

	// Already paused?
	if (paused)
		return true;

	// We are now frozen
	stream->Pause();
	paused = true;

	return true;
}

/***************************************************************************/
/***************************************************************************/
bool SoundStreamPS2::IsPlaying()
{
	if (!queued)
		return true;
	else
		return stream->IsPlaying();
}

/***************************************************************************/
// Return the play position, returns -1 if still preloading
/***************************************************************************/
int SoundStreamPS2::CurrentPlayPos()
{
	if (!queued)
		return -1;
	else
		return stream->PlayPosition();
}

/***************************************************************************/
// Set the volume [0..1], 0.0 is silent 1.0 is full volume
/***************************************************************************/
bool SoundStreamPS2::SetVolume(
float wantVolume)
{
	m_volume = wantVolume;

	MIBStream::Volume	volume[2];
	ComputeVolume(volume);

	return stream->SetVolume(volume, stream->Channels());
}

/***************************************************************************/
// Set the pan [-1..1] (stereo streams can't be panned)
/***************************************************************************/
bool SoundStreamPS2::SetPan(
float pan)
{
	m_pan = pan;
	return SetVolume(m_volume);
}

/***************************************************************************/
// Change the pitch of the sound in semitones
/***************************************************************************/
bool SoundStreamPS2::SetPitch(
float pitchShift)
{
	m_pitch = pitchShift;

	int baseFrequency = ((stream->BasePitch() * 48000) / 4096);
	int shift = SemitoneMath::ModifyFrequencyBySemitones(baseFrequency, m_pitch + m_masterPitch);
	int ps2Pitch = Math::Clamp((shift * 4096) / 48000, 0, 0x3fff);

	// Set the proper pitch
	return stream->SetPitch(ps2Pitch);
}

/***************************************************************************/
// Set the wet/dry mix.  Beware, on the PS2 anything over 0.0
// will be 100% on
/***************************************************************************/
bool SoundStreamPS2::SetWetDry(
float newWet,
float newDry)
{
	m_wet = newWet;
	m_dry = newDry;

	// Make sure we are dry
	return stream->SetWetDry(m_wet > 0.0f, m_dry > 0.0f);
}

/***************************************************************************/
// our constructor
/***************************************************************************/
SoundStreamPS2::SoundStreamPS2(
SoundDriverPS2 &parent,
const char *streamName,
SoundAttr &attr,
MIBStream &mibStream,
bool wantRepeat) :
SoundStream(streamName, attr, wantRepeat),
driver(parent)
{
	// Caps
	m_caps = STREAMCAP_POS | STREAMCAP_VOLUME | STREAMCAP_PAN | STREAMCAP_PITCH | STREAMCAP_WETDRY;

	// Save off the info
	stream = &mibStream;

	queued = false;
	paused = false;
}

/***************************************************************************/
/***************************************************************************/
void SoundStreamPS2::ComputeVolume(
MIBStream::Volume volume[2])
{
	int spuVolumeL, spuVolumeR;

	float mixVolume = m_volume;
	if (m_fader)
		mixVolume *= m_fader->MixVolume();
	SoundInstance::ComputeVolume(mixVolume, m_pan, spuVolumeL, spuVolumeR);

	if (stream->Channels() == 1)
	{
		volume[0].left = spuVolumeL;
		volume[0].right = spuVolumeR;
		volume[1] = volume[0];
	}
	else
	{
		switch (driver.GetOutputMode())
		{
			case SoundDriver::OUTPUTMODE_MONO:
				volume[0].left = spuVolumeL >> 1;
				volume[0].right = spuVolumeR >> 1;
				volume[1] = volume[0];
				break;

			case SoundDriver::OUTPUTMODE_STEREOFLIPPED:
				volume[0].left = 0;
				volume[0].right = spuVolumeR;
				volume[1].left = spuVolumeL;
				volume[1].right = 0;
				break;

			default:
				volume[0].left = spuVolumeL;
				volume[0].right = 0;
				volume[1].left = 0;
				volume[1].right = spuVolumeR;
				break;
		}
	}
}
