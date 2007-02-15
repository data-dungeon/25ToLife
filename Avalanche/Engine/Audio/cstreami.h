///////////////////////////////////////////////////////////////////////////////
// Common stream interface that supports both 2D and 3D stream interfaces
///////////////////////////////////////////////////////////////////////////////
#ifndef CSTREAMI_H
#define CSTREAMI_H

#include "container/dllist.h"

class SoundDriver;
class SoundStream2D;
class SoundStream3D;
class CommonStreamInterface;

class CSIHandle
{
public:
	~CSIHandle();

	// Get interface (returns NULL if dead)
	CommonStreamInterface *GetStream()
		{ return m_stream; }

	// Get the stream, will clean up and NULL handle if lost
	static CommonStreamInterface *GetStream(CSIHandle *&handle);

	// Delete this handle and the stream (if there)
	// If !stopPlaying it will add it to the auto cleanup list
	static void Kill(CSIHandle *&handle, bool stopPlaying = true);
	void Kill(bool stopPlaying = true);

	// Special control since we use a pool
	void *operator new(size_t size);
	void operator delete(void *me);

private:
	CSIHandle(CommonStreamInterface *stream);

	// Our interface
	CommonStreamInterface *m_stream;

	// It's nice to have friends
	friend class CommonStreamInterface;
};

class CommonStreamInterface
{
public:
	~CommonStreamInterface();

	// Call this once per frame
	static void UpdateAutoDelete();
	static int FlushAutoDelete();

	// Create a handle for this stream (you must delete it!)
	// Will assert if you already have it
	CSIHandle *CreateHandle();

	// Just get the handle (DO NOT DELETE THIS)
	CSIHandle *GetHandle()
		{ return m_handle; }

	// What is our name?
	const char *Name();

	// Repeat the buffer?
	bool Looping();

	// Manage the fader
	void BindFader(Fader *fader);
	void ClearFader();
	Fader *GetFader();

	// Set the volume [0..1], 0.0 is silent 1.0 is full volume
	bool SetVolume(float volume);
	float GetVolume();

	// Set the pan [-1..1] (stereo streams can't be panned)
	bool SetPan(float pan);
	float GetPan();

	// Change the pitch of the sound in semitones
	bool SetPitch(float pitchShift);
	float GetPitch();

	// Set the mix for this sound stream
	bool SetWetDry(float wet, float dry);
	float GetWet();
	float GetDry();

	// Playback control
	bool Play();
	bool Stop(bool rewind = false);
  	bool IsPlaying();

	// Return the play position, returns -1 if still preloading
	int CurrentPlayPos();

	// Pause
	bool Pause();
	bool Unpause();
	bool Paused();

	// Detach the stream (make it auto cleanup)
	bool Detach();

	// Get the error
 	t_Error StreamError();

	// Access the streams (one or the other is null)
	SoundStream	*Stream2D()
		{ return m_stream2D; }
	SoundStream3D *Stream3D()
		{ return m_stream3D; }

protected:
	// Only the driver should create us. We own this stream
	// and will call delete on it when done
	CommonStreamInterface(SoundStream &stream, bool autoDelete);
	CommonStreamInterface(SoundStream3D &stream, bool autoDelete);

	// Wee! (set auto-delete to automatically delete when done)
	static CommonStreamInterface *Create2D(SoundDriver &driver, const char *fileName, bool repeat, bool autoDelete = false, bool preLoad = false);
	static CommonStreamInterface *Create3D(SoundDriver &driver, const char *fileName, bool repeat, bool autoDelete = false, SoundStream3D::Priority priority = SoundStream3D::STREAM3D_PRIORITY_NORMAL);

	// Create using a song id
	static CommonStreamInterface *Create2D(SoundDriver &driver, SoundDriver::SONGID id, bool repeat, bool autoDelete = false);
	friend class Audio;

	// One or the other
	SoundStream			*m_stream2D;
	SoundStream3D		*m_stream3D;

	// Our handle
	CSIHandle			*m_handle;
	friend class CSIHandle;

	// The Auto-Delete list
	bool					m_autoDelete;
	static DoublyLinkedList<CommonStreamInterface>	s_autoDelete;

public:
	// We can appear in a list
	CommonStreamInterface		*prev, *next;
};

#endif
