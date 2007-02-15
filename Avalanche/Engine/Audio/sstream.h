///////////////////////////////////////////////////////////////////////////////
// SoundStream, a disk or i/o based sound
// Abstract base class
///////////////////////////////////////////////////////////////////////////////
#ifndef SSTREAM_H
#define SSTREAM_H

#include "Layers/Media.h"
#include "Audio/soundattr.h"

class SoundDriver;
class Fader;

class SoundStream
{
public:
	// Setup global defaults for sounds
	static void SetMasterPitch(float masterPitch);
	static float GetMasterPitch();

	// The driver must create the sound, but it can be deleted directly
	virtual ~SoundStream() {}

	// What is our name?
	const char *Name()
		{  return m_name; }

	// Get at our attributes
	const SoundAttr &Attr()
		{ return m_attr; }

	// Repeat the buffer?
	bool Looping()
		{ return m_loop; }

	// What does the underlying hardware support
	enum CAPS
	{
		// Stream is preload only
		STREAMCAP_PRELOAD_ONLY = 0x01,

		// CurrentPlayPos works
		STREAMCAP_POS = 0x02,

		// Seek works
		STREAMCAP_SEEK = 0x02,

		// Rewind works
		STREAMCAP_REWIND = 0x04,

		// Volume works (duh)
		STREAMCAP_VOLUME = 0x10,

		// Pan works
		STREAMCAP_PAN = 0x20,

		// Pitch works
		STREAMCAP_PITCH = 0x40,

		// WetDry works
		STREAMCAP_WETDRY = 0x80,

		// Stream actually reports errors
		STREAMCAP_REALERROR = 0x100
	};
	virtual int GetCaps()
		{ return m_caps; }
	virtual bool GetCap(CAPS cap)
		{ return ((m_caps & cap) != 0); }

	// Play at current pos
	virtual bool Play() = 0;

	// Stop with optional rewind
	virtual bool Stop(bool rewind = false) = 0;

	// Pause
	virtual bool Pause();
	virtual bool Unpause();
	virtual bool Paused()
		{ return m_paused; }

	// Is the stream playing
	virtual bool IsPlaying() = 0;

	// Return the play position, returns -1 if still preloading
	virtual int CurrentPlayPos() = 0;

	// Seek to a position
	virtual bool Seek(int pos) = 0;

	// Rewind
	virtual bool Rewind() = 0;

	// ASsign a fader to this stream (there can be only one!)
	virtual void BindFader(Fader *fader)
		{ m_fader = fader; }
	void ClearFader()
		{ BindFader(NULL); }
	virtual Fader *GetFader()
		{ return m_fader; }

	// Set the volume [0..1], 0.0 is silent 1.0 is full volume
	virtual bool SetVolume(float volume) = 0;
	float GetVolume()
		{ return m_volume; }

	// Set the pan [-1..1] (stereo streams can't be panned)
	virtual bool SetPan(float pan) = 0;
	float GetPan()
		{ return m_pan; }

	// Change the pitch of the sound in semitones
	virtual bool SetPitch(float pitchShift) = 0;
	float GetPitch()
		{ return m_pitch; }

	// Set the mix for this sound stream
	virtual bool SetWetDry(float wet, float dry) = 0;
	float GetWet()
		{ return m_wet; }
	float GetDry()
		{ return m_dry; }

	// Get the error
	virtual t_Error StreamError()
		{ return MED_ERR_NONE; }

protected:
	// The constructor
	SoundStream(const char *name, bool loop);
	SoundStream(const char *name, SoundAttr &attr, bool loop);

	// Our name
	char				m_name[32];

	// Our attributes
	SoundAttr		m_attr;

	// Do we loop
	bool				m_loop;

	// Are we paused?
	bool				m_paused;

	// Our caps
	int				m_caps;

	// stuff
	Fader				*m_fader;
	float				m_volume;
	float				m_pan;
	float				m_pitch;
	float				m_wet, m_dry;

	// Global overrides
	static float	m_masterPitch;
};

#endif
