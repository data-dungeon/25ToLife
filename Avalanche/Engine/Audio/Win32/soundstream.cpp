/***************************************************************************/
// SoundStream, a disk or i/o based sound
/***************************************************************************/
#include "Audio/AudioPCH.h"

/***************************************************************************/
// The driver must create the sound, but it can be deleted directly
/***************************************************************************/
SoundStreamWin32::~SoundStreamWin32()
{
	ShutdownThread();

	delete ioSystem;
	delete instance;
}

/***************************************************************************/
/***************************************************************************/
bool SoundStreamWin32::Play()
{
	return instance->Play(false);
}

/***************************************************************************/
/***************************************************************************/
bool SoundStreamWin32::Stop(
bool rewind)
{
	return instance->Stop(rewind);
}

/***************************************************************************/
/***************************************************************************/
bool SoundStreamWin32::IsPlaying()
{
	return instance->IsPlaying();
}

/***************************************************************************/
// Return the play position, returns -1 if still preloading
/***************************************************************************/
int SoundStreamWin32::CurrentPlayPos()
{
	if (buffersPlayed < 0)
		return -1;
	else
	{
		int playOffset = instance->GetPlayPosition();
		return playOffset + ((buffersPlayed / 2) * size);
	}
}

/***************************************************************************/
// Rewind the sound
/***************************************************************************/
bool SoundStreamWin32::Rewind()
{
	return instance->Rewind();
}

/***************************************************************************/
// Volume from 0 to 255.  128 is half volume
/***************************************************************************/
bool SoundStreamWin32::SetVolume(
float volume)
{
	m_volume = Math::Clamp(volume);
	float mixVolume = m_volume;
	if (m_fader)
		mixVolume *= m_fader->MixVolume();
	return instance->SetVolumeDirect(mixVolume);
}

/***************************************************************************/
// Set the pan [-1..1] (stereo streams can't be panned)
/***************************************************************************/
bool SoundStreamWin32::SetPan(
float pan)
{
	m_pan = Math::Clamp(pan, -1.0f, 1.0f);
	return instance->SetPan(m_pan);
}

/***************************************************************************/
// Change the pitch of the sound in semitones
/***************************************************************************/
bool SoundStreamWin32::SetPitch(
float pitchShift)
{
	m_pitch = pitchShift;
	return instance->SetPitch(m_pitch + m_masterPitch);
}

/***************************************************************************/
// Set the i/o methodology
/***************************************************************************/
bool SoundStreamWin32::SetIOSystem(
StreamIOSystem *streamIOSystem)
{
	// Set the io system
	ioSystem = streamIOSystem;
	if (!ioSystem || !ioSystem->ReadValid())
		return false;

	// Create us an instance
	buffer->AddRef();
 	instance = new SoundInstance(this, buffer);
	if (!instance)
	{
		buffer->Release();
		return false;
	}

	// Startup the thread
	if (!StartThread())
		return false;

	// We are ready to go
	return true;
}

/***************************************************************************/
// our constructor
/***************************************************************************/
SoundStreamWin32::SoundStreamWin32(
SoundDriverServices &driverServices,
const char *streamName,
SoundAttr &bufferAttr,
bool loop,
LPDIRECTSOUNDBUFFER theBuffer,
int bufferSize) :
SoundData(driverServices, theBuffer, bufferSize, bufferAttr),
SoundStream(streamName, bufferAttr, loop)
{
	// What are our caps?
	m_caps = STREAMCAP_POS | STREAMCAP_REWIND | STREAMCAP_VOLUME | STREAMCAP_PAN | STREAMCAP_PITCH;

	// Init all the junk
	ioSystem = NULL;
	buffersPlayed = -1;
	instance = NULL;

	// Thread stuff
	InitThread();
}

/***************************************************************************/
// Initialize thread stuff
/***************************************************************************/
void SoundStreamWin32::InitThread()
{
	for (int i = 0; i < STREAMEVENTS; i++)
		hEvent[i] = NULL;
	notify = NULL;

	threadHandle = (DWORD)-1;
	threadExited = TRUE;
	InitializeCriticalSection(&criticalSection);
	stopEvent = -1;
}

/***************************************************************************/
// Setup the thread
/***************************************************************************/
bool SoundStreamWin32::StartThread()
{
	// Setup the events
	for (int i = 0; i < STREAMEVENTS; i++)
		hEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);

	// Setup the notify structures
	posNotify[STREAMEVENT_START].dwOffset = 0;
	posNotify[STREAMEVENT_START].hEventNotify = hEvent[STREAMEVENT_START];
	posNotify[STREAMEVENT_HALFWAY].dwOffset = size / 2;
	posNotify[STREAMEVENT_HALFWAY].hEventNotify = hEvent[STREAMEVENT_HALFWAY];

#ifndef _XBOX
	// Query the notification interface
	if (buffer->QueryInterface(IID_IDirectSoundNotify, (VOID **)&notify) != DS_OK)
		return false;
#endif

	// Set the notifications
	if (notify->SetNotificationPositions(STREAMEVENTS, posNotify) != DS_OK)
		return false;

	// Load the first half
	StreamToBuffer(STREAMEVENT_HALFWAY);

	// Create the thread
	exitThread = FALSE;
	threadExited = FALSE;
	if ((threadHandle = _beginthread(ThreadFunction, 0, this)) == (DWORD)-1)
		return false;

	return true;
}

/***************************************************************************/
// Shutdown the thread
/***************************************************************************/
void SoundStreamWin32::ShutdownThread()
{
	// Shut down the thread
	if (threadHandle != -1)
	{
		DWORD stopAt = timeGetTime() + 1500;

		// Tell the thread to leave
		exitThread = TRUE;

		// We will have to wait for an exit
		SetEvent(hEvent[0]);
		while (!threadExited && timeGetTime() < stopAt)
			Sleep(100);
	}

	// Clean up our events
	for (int i = 0; i < STREAMEVENTS; i++)
		if (hEvent[i])
		{
			CloseHandle(hEvent[i]);
			hEvent[i] = NULL;
		}

	// Release the notification event
	if (notify)
	{
		notify->Release();
		notify = NULL;
	}

	DeleteCriticalSection(&criticalSection);
}

/***************************************************************************/
// Block the thread
/***************************************************************************/
inline void SoundStreamWin32::BlockThread()
{
	EnterCriticalSection(&criticalSection);
}

/***************************************************************************/
// Unblock the thread
/***************************************************************************/
inline void SoundStreamWin32::UnblockThread()
{
	LeaveCriticalSection(&criticalSection);
}

/***************************************************************************/
// The thread itself
/***************************************************************************/
void __cdecl SoundStreamWin32::ThreadFunction(
void *argList)
{
	SoundStreamWin32 *me = (SoundStreamWin32 *)argList;

	while (!me->exitThread)
	{
		// Wait for an event
		DWORD event = WaitForMultipleObjects(STREAMEVENTS, me->hEvent,
															FALSE, INFINITE);
		event -= WAIT_OBJECT_0;

		// Time to exit?
		if (me->exitThread)
			break;

		// Do the actual work!
		me->BlockThread();
		me->StreamToBuffer(event);
		me->UnblockThread();
	}

	// If we are here, kill everything
	me->threadExited = TRUE;
	me->threadHandle = (DWORD)-1;
}

/***************************************************************************/
// handle a stream event
/***************************************************************************/
void SoundStreamWin32::StreamToBuffer(
int event)
{
	// Are we stopping?
	if (stopEvent == event)
	{
		stopEvent = -1;
		instance->Stop();
		if (!m_loop)
			return;
		buffersPlayed = -1;
		ioSystem->Rewind();
		instance->Play();
	}

	// Where do we start the read
	int offset;
	if (event == 0)
		offset = (int)posNotify[STREAMEVENTS - 1].dwOffset;
	else
		offset = (int)posNotify[event - 1].dwOffset;

	// How much to write?
	int write = (int)posNotify[event].dwOffset - offset;
	if (write < 0)
		write += size;

	// We have played one more buffer
	++buffersPlayed;

	// Lock the buffer
	void *data1, *data2;
	int size1, size2;
	if (!Lock(offset, write, &data1, &size1, &data2, &size2))
		return;

	// Spoon in the data
	if (stopEvent != -1)
		memset(data1, '\0', size1);
	else
	{
		// Read the data
		unsigned long read;
		ioSystem->Read(size1, (unsigned char *)data1, &read);

		// Are we at the end?
		if ((int)read < size1)
		{
			// We are done!
			int sizeToFill = size1 - read;
			if (attr.SixteenBit())
				sizeToFill *= 2;
			if (attr.Stereo())
				sizeToFill *= 2;
			memset((unsigned char *)data1 + read, '\0', size1 - read);
			stopEvent = event;
		}
	}

	// Unlock the buffer
	Unlock(data1, size1, data2, size2);
}

/***************************************************************************/
// Specify the wave file
/***************************************************************************/
WaveStreamIOSystem::WaveStreamIOSystem(
char *waveFile)
{
	valid = Prepare(waveFile);
}

/***************************************************************************/
// Clean up
/***************************************************************************/
WaveStreamIOSystem::~WaveStreamIOSystem()
{
	Close();
}

/***************************************************************************/
// The read command
/***************************************************************************/
bool WaveStreamIOSystem::Read(
unsigned long request,
BYTE *data,
unsigned long *read)
{
	ASSERT(valid);

	UINT reallyRead;
	if (waveRead.Read(request, data, &reallyRead) != WaveFile::OKAY)
		return false;

	// Save this for them
	*read = (unsigned long)reallyRead;
	seek += *read;
	return true;
}

/***************************************************************************/
// Rewind to the beginning
/***************************************************************************/
bool WaveStreamIOSystem::Rewind()
{
	// Make sure we are in a valid state
	ASSERT(valid);

	// Seek to the beginning
	if (waveRead.Start() != WaveFile::OKAY)
		return false;
	seek = 0;

	// Yeah!
	return true;
}

/***************************************************************************/
// Open/close the wave file
/***************************************************************************/
bool WaveStreamIOSystem::Prepare(
char *file)
{
	// Make a "real" filename
	char realFile[256];
	Media::MakeFileName(file, realFile);

	// Open the file
	if (waveRead.Open(realFile) != WaveFile::OKAY)
		return false;

  	// Seek to the start
  	if (waveRead.Start() != WaveFile::OKAY)
  		return false;
	seek = 0;

	return true;
}

/***************************************************************************/
// Open/close the wave file
/***************************************************************************/
void WaveStreamIOSystem::Close()
{
	waveRead.Close();
}

#ifdef DIRECTX_PC

/***************************************************************************/
// Ogg file accessors
/***************************************************************************/
OggStreamIOSystem::OggStreamIOSystem(
char *waveFile)
{
	valid = Prepare(waveFile);
}

/***************************************************************************/
// Clean up
/***************************************************************************/
OggStreamIOSystem::~OggStreamIOSystem()
{
	Close();
}

/***************************************************************************/
// The read command
/***************************************************************************/
bool OggStreamIOSystem::Read(
unsigned long request,
BYTE *data,
unsigned long *read)
{
	ASSERT(valid);

	UINT reallyRead;
	if (oggRead.Read(request, reinterpret_cast<char*>(data), &reallyRead) != WaveFile::OKAY)
		return false;

	// Save this for them
	*read = (unsigned long)reallyRead;
	seek += *read;
	return true;
}

/***************************************************************************/
// Rewind to the beginning
/***************************************************************************/
bool OggStreamIOSystem::Rewind()
{
	// Make sure we are in a valid state
	ASSERT(valid);

	//// Seek to the beginning
	//if (waveRead.Start() != WaveFile::OKAY)
	//	return false;
	//seek = 0;

	// Yeah!
	return true;
}

/***************************************************************************/
// Open/close the wave file
/***************************************************************************/
bool OggStreamIOSystem::Prepare(
char *file)
{
	// Make a "real" filename
	char realFile[256];
	Media::MakeFileName(file, realFile);

	// Open the file
	if (oggRead.Open(realFile) != WaveFile::OKAY)
		return false;

	return true;
}

/***************************************************************************/
// Open/close the wave file
/***************************************************************************/
void OggStreamIOSystem::Close()
{
	oggRead.Close();
}

#endif // DIRECTX_PC
