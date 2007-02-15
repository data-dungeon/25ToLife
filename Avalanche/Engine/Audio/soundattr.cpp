/***************************************************************************/
// Sound buffer attributes
/***************************************************************************/

/******************* includes ****************************/

#include "Audio/AudioPCH.h"

// statics
const unsigned int SoundAttr::VOLUMECONTROL = 	0x00000001;
const unsigned int SoundAttr::PANCONTROL = 		0x00000002;
const unsigned int SoundAttr::PITCHCONTROL = 	0x00000004;
const unsigned int SoundAttr::AUTORECLAIM = 		0x00000010;
const unsigned int SoundAttr::SIXTEENBIT = 		0x00010000;
const unsigned int SoundAttr::STEREO = 			0x00020000;
const unsigned int SoundAttr::ADPCM = 				0x10000000;
const unsigned int SoundAttr::STREAMING = 		0x80000000;

const unsigned int SoundAttr::NORMAL =
	(SoundAttr::VOLUMECONTROL | SoundAttr::PANCONTROL |
		SoundAttr::PITCHCONTROL | SoundAttr::SIXTEENBIT);

const unsigned int SoundAttr::STREAM =
	(SoundAttr::VOLUMECONTROL | SoundAttr::SIXTEENBIT | SoundAttr::STREAMING);

const unsigned int SoundAttr::STEREONORMAL =
	(SoundAttr::VOLUMECONTROL | SoundAttr::SIXTEENBIT | SoundAttr::STEREO);

/***************************************************************************/
// How many millseconds would it take to play this sample at standard frequency?
/***************************************************************************/
uint SoundAttr::PlayTimeMS(
uint size) const
{
	// What is the default playtime?
	return PlayTimeMS(size, SampleRate());
}

/***************************************************************************/
// How many millseconds would it take to play this sample the specified frequency?
/***************************************************************************/
uint SoundAttr::PlayTimeMS(
uint size,
uint frequency) const
{
	// How many samples do we have?
	uint samples = size;

	// If we are compressed, we really have way more data (3.5:1 on PS2)
	if (Compressed())
		samples = (samples * 3) + (samples / 2);

	// But we could have more bytes per DSP clock
	if (SixteenBit())
		samples /= 2;
	if (Stereo())
		samples /= 2;

	// Now divide by the frequency of playback (in MS)
	return samples * 1000 / frequency;
}

#if defined(WIN32)
/***************************************************************************/
// Translate the attr into DSBufferDesc dwFlags member
/***************************************************************************/
DWORD	SoundAttr::DSBufferDescFlags() const
{
#ifdef _XBOX
	// Setup the default flags
	DWORD flags = 0;

	// Test the things we support
	if (VolumeControl())
		flags |= DSBCAPS_CTRLVOLUME;
	if (PitchControl())
		flags |= DSBCAPS_CTRLFREQUENCY;
	if (Streaming())
		flags |= DSBCAPS_CTRLPOSITIONNOTIFY;
#else
	// Setup the default flags
	DWORD flags = DSBCAPS_GETCURRENTPOSITION2; // | DSBCAPS_CTRLPOSITIONNOTIFY;

	// Test the things we support
	if (VolumeControl())
		flags |= DSBCAPS_CTRLVOLUME;
	if (PanControl())
		flags |= DSBCAPS_CTRLPAN;
	if (PitchControl())
		flags |= DSBCAPS_CTRLFREQUENCY;
	if (Streaming())
		flags |= DSBCAPS_CTRLPOSITIONNOTIFY;
	else
		flags |= DSBCAPS_STATIC;
#endif

	return flags;
}

/***************************************************************************/
// Fill out a wave format thingie with what we know
/***************************************************************************/
void SoundAttr::WaveFormatEx(
WAVEFORMATEX *wfx) const
{
	wfx->wFormatTag = WAVE_FORMAT_PCM;
	wfx->nChannels = Stereo() ? 2 : 1;
	wfx->nSamplesPerSec = sampleRate;
	wfx->wBitsPerSample = SixteenBit() ? 16 : 8;
  	wfx->nBlockAlign = (wfx->nChannels * wfx->wBitsPerSample) / 8;
	wfx->nAvgBytesPerSec = wfx->nSamplesPerSec * wfx->nBlockAlign;
	wfx->cbSize = 0;
}

/***************************************************************************/
// Fill out the attr from a wave format ex
/***************************************************************************/
void SoundAttr::InferFromWFX(
const WAVEFORMATEX &wfx)
{
	sampleRate = wfx.nSamplesPerSec;
	Stereo(wfx.nChannels == 2);
	SixteenBit(wfx.wBitsPerSample == 16);
}
#elif defined(PS2)
/***************************************************************************/
// Fill out the attr from a VAG header
/***************************************************************************/
void SoundAttr::InferFromVAG(
const VAGHeader &vag)
{
	// Is this in the wrong integer format?
	bool PC = (strncmp(vag.ID, "VAGp", 4) == 0);
	if (PC)
		sampleRate = ConvertEndian(vag.sampleFreq);
	else
		sampleRate = vag.sampleFreq;

	// VAG's are always this way
	Stereo(false);
	SixteenBit(true);
	Compressed(true);
}
#elif defined(GCN)
/***************************************************************************/
// Fill out the attr from a VAG header
/***************************************************************************/
void SoundAttr::InferFromDSP(	const ADPCM_Hdr &adpcmHdr)
{
	sampleRate = adpcmHdr.sample_rate;

	Stereo(false);
	SixteenBit(true);
	Compressed(true);
}
#endif

