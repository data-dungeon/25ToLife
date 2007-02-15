/***************************************************************************/
// Sound buffer attributes
/***************************************************************************/
#ifndef SOUNDATTR_H
#define SOUNDATTR_H

#include "platform/BaseType.h"
#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#ifdef _XBOX
#else
#include <windows.h>
#include <mmsystem.h>
#endif
#include <dsound.h>
#elif defined(PS2)
#include "Audio/PS2/vag.h"
#elif defined(GCN)
#include "Audio/GCN/dspadpcm.h"
#endif

// The common sample rates
#define SAMPLE8KHz		8000
#define SAMPLE11KHz		11025
#define SAMPLE22KHz		22050
#define SAMPLE32KHz		32000
#define SAMPLE44KHz		44100
#define SAMPLE48KHz		48000

class SoundAttr
{
public:
	// Sound attr flags
	static const unsigned int VOLUMECONTROL;
	static const unsigned int PANCONTROL;
	static const unsigned int PITCHCONTROL;
	static const unsigned int AUTORECLAIM;
	static const unsigned int SIXTEENBIT;
	static const unsigned int STEREO;
	static const unsigned int STREAMING;
	static const unsigned int ADPCM;

	// composites
	static const unsigned int NORMAL;			// volume, pan, pitch, 16bit
	static const unsigned int STREAM;			// volume, 16bit, streaming
	static const unsigned int STEREONORMAL;	// volume, 16bit, stereo

	// Set the flags directly
	void Set(unsigned int requestedFlags)
		{ flags = requestedFlags; }

	// What is our sample rate?
	void SampleRate(unsigned int theSampleRate)
		{ sampleRate = theSampleRate; }
	unsigned int SampleRate() const
		{ return sampleRate; }

	// Constructors
	SoundAttr()
		{ Set(NORMAL); SampleRate(SAMPLE22KHz); }
	SoundAttr(unsigned int requestedFlags, int theSampleRate)
		{ Set(requestedFlags); SampleRate(theSampleRate); }
	~SoundAttr() {}

	// What are the flags?
	unsigned int Get() const
		{ return flags; }

	// Get/Modify flags with methods
	bool VolumeControl() const
		{ return TestFlag(VOLUMECONTROL); }
	void VolumeControl(bool set)
		{ SetFlag(VOLUMECONTROL, set); }

	bool PanControl() const
		{ return TestFlag(PANCONTROL); }
	void PanControl(bool set)
		{ SetFlag(PANCONTROL, set); }

	bool PitchControl() const
		{ return TestFlag(PITCHCONTROL); }
	void PitchControl(bool set)
		{ SetFlag(PITCHCONTROL, set); }

	bool AutoReclaim() const
		{ return TestFlag(AUTORECLAIM); }
	void AutoReclaim(bool set)
		{ SetFlag(AUTORECLAIM, set); }

	bool SixteenBit() const
		{ return TestFlag(SIXTEENBIT); }
	void SixteenBit(bool set)
		{ SetFlag(SIXTEENBIT, set); }

	bool Stereo() const
		{ return TestFlag(STEREO); }
	void Stereo(bool set)
		{ SetFlag(STEREO, set); }

	bool Streaming() const
		{ return TestFlag(STREAMING); }
	void Streaming(bool set)
		{ SetFlag(STREAMING, set); }

	bool Compressed() const
		{ return TestFlag(ADPCM); }
	void Compressed(bool set)
		{ SetFlag(ADPCM, set); }

	// How many millseconds would it take to play this sample at standard frequency?
	uint PlayTimeMS(uint size) const;

	// How many millseconds would it take to play this sample the specified frequency?
	uint PlayTimeMS(uint size, uint frequency) const;

#if defined(WIN32)
	// Translate the attr into DSBufferDesc dwFlags member
	virtual DWORD DSBufferDescFlags() const;

	// Fill out a wave format thingie with what we know
	void WaveFormatEx(WAVEFORMATEX *wfx) const;

	// Fill out the attr from a wave format ex
	void InferFromWFX(const WAVEFORMATEX &wfx);
#elif	defined(PS2)
	// Fill out the attr from a VAG header
	void InferFromVAG(const VAGHeader &vag);
#elif	defined(GCN)
	// Fill out the attr from a DSP header
	void InferFromDSP(const ADPCM_Hdr &adpcmHdr);
#endif

	// operators
	int operator==(const SoundAttr &c) const
		{ return (flags == c.flags && sampleRate == c.sampleRate); }
	int operator!=(const SoundAttr &c) const
		{ return (flags != c.flags || sampleRate != c.sampleRate); }
	SoundAttr &operator=(const SoundAttr &c)
		{ flags = c.flags; sampleRate = c.sampleRate; return *this; }

	// If you ever change the ordering of my code again, I will kill you
	// If you think my shit is unreadable, just think what I think of your shit.
protected:
	// Our flags
	unsigned int	flags;
	unsigned int	sampleRate;

	// Test to see if a flag is on
	bool TestFlag(unsigned int flag) const
		{ return ((flags & flag) != 0); }

	// Clear/set a flag
	void SetFlag(unsigned int flag, bool set)
		{ if (set) flags |= flag; else flags &= ~flag; }

	// swap bytes 0 and 4, 1 and 2
	uint ConvertEndian(uint inp)
		{ return ((inp & 0xFF) << 24) | ((inp & 0xFF00) << 8) | ((inp & 0xFF0000) >> 8) | ((inp & 0xFF000000) >> 24); }
};

#endif
