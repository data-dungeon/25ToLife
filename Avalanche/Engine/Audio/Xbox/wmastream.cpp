/////////////////////////////////////////////////////////////////////////////
// WMA based streaming sound, supports soundtracks
/////////////////////////////////////////////////////////////////////////////

#include "Audio/AudioPCH.h"

#include "Layers/Media.h"
#include "Audio/Math/volume.h"
#include "Audio/Xbox/streamlib.h"
#include "Audio/Xbox/wmastream.h"
#include "Layers/DiscErrorHandler/DiscError.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
bool WMAStream::Test(
ImbeddedStreamInfo &info,
const char *fileName)
{
	// Create the decoder
	WMAXMODECODERPARAMETERS params = { 0 };
	params.hFile = info.file;
	params.dwFileOffset = info.offset;
	params.dwLookaheadBufferSize  = WMASTRM_LOOKAHEAD_SIZE;

	// Create the source (wma file)
	XWmaFileMediaObject *inputStream = NULL;
	HRESULT ret = XWmaDecoderCreateMediaObject((LPCWMAXMODECODERPARAMETERS)&params, &inputStream);
	ASSERT(!FAILED(ret));
	if (FAILED(ret))
		return false;

	// doh
	ASSERT(inputStream != NULL);
	if (!inputStream)
		return false;

	// Do some work!
	DWORD status;
	do {
		inputStream->DoWork();
		status = 0;
		ret = inputStream->GetStatus(&status);
		ASSERT(!FAILED(ret));
	} while (FAILED(ret) || (status & XMO_STATUSF_ACCEPT_OUTPUT_DATA) == 0);

	// Check the header
	WMAXMOFileHeader wmaFileHeader = { 0 };
	ret = inputStream->GetFileHeader(&wmaFileHeader);
	inputStream->Release();

	ASSERT(!FAILED(ret));
	if (FAILED(ret))
		return false;

	ASSERT(wmaFileHeader.dwNumChannels == 1 || wmaFileHeader.dwNumChannels == 2);
	return (wmaFileHeader.dwNumChannels == 1 || wmaFileHeader.dwNumChannels == 2);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// Simple controls
/////////////////////////////////////////////////////////////////////////////
bool WMAStream::Play()
{
	Pause(false);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WMAStream::Stop(
bool rewind)
{
	Pause(true);
	if (rewind)
		return Rewind();
	else
		return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WMAStream::IsPlaying()
{
	// Weee
	if (openResult == E_PENDING)
		return true;

	// Failure?
	if (openResult == E_FAIL)
		return false;

	// Anything playing
	if (!outputStream)
		return false;

	// Have we hit eof yet?
	if (!doneRead)
		return true;

	// Any open packets?
	for (int i = 0; i < WMASTRM_PACKET_COUNT; i++)
	{
		if (packet[i].status == XMEDIAPACKET_STATUS_PENDING)
			return true;
	}

	return false;
}

/***************************************************************************/
// Return the play position, returns -1 if still preloading
/***************************************************************************/
int WMAStream::CurrentPlayPos()
{
	if (!outputStream)
		return(-1);

	DWORD	pStatus;
	HRESULT hr = outputStream->GetStatus(&pStatus);

	if (hr != DS_OK)
		return(-1);

	if (pStatus & DSSTREAMSTATUS_PLAYING)
		return(1);
	else
		return(-1);
}

/////////////////////////////////////////////////////////////////////////////
// Rewind the sound
/////////////////////////////////////////////////////////////////////////////
bool WMAStream::Rewind()
{
	Close();
	return Open();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WMAStream::Seek(int pos)
{
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// Set the volume from [0..1]
/////////////////////////////////////////////////////////////////////////////
bool WMAStream::SetVolume(
float volume)
{
	m_volume = Math::Clamp(volume);
	return ApplyMixBins();
}

/////////////////////////////////////////////////////////////////////////////
// Set the pan [-1..1] (stereo streams can't be panned)
/////////////////////////////////////////////////////////////////////////////
bool WMAStream::SetPan(
float pan)
{
	m_pan = Math::Clamp(pan, -1.0f, 1.0f);
	return ApplyMixBins();
}

/////////////////////////////////////////////////////////////////////////////
// Change the pitch of the sound in semitones
/////////////////////////////////////////////////////////////////////////////
bool WMAStream::SetPitch(
float pitchShift)
{
	// duh
	if (!outputStream)
		return false;

	m_pitch = pitchShift;
	int frequency = m_attr.SampleRate();
	int shift = SemitoneMath::ModifyFrequencyBySemitones(frequency, m_pitch + m_masterPitch);
	shift = Math::Clamp(shift, SoundInstance::MINFREQUENCY, SoundInstance::MAXFREQUENCY);

 	HRESULT ret = outputStream->SetFrequency(shift);
 	SoundError::SetLastError(ret);
	return (ret == DS_OK);
}

/////////////////////////////////////////////////////////////////////////////
// Set the mix for this sound stream
/////////////////////////////////////////////////////////////////////////////
bool WMAStream::SetWetDry(
float wet,
float dry)
{
	m_wet = Math::Clamp(wet);
	m_dry = Math::Clamp(dry);
	return ApplyMixBins();
}

/////////////////////////////////////////////////////////////////////////////
// Apply mixbins
/////////////////////////////////////////////////////////////////////////////
bool WMAStream::ApplyMixBins()
{
	if (!outputStream)
		return false;

	float volume = m_volume;
	if (m_fader)
		volume *= m_fader->MixVolume();

	if (wmaFileHeader.dwNumChannels > 1)
		return driver.ApplyMixBins2D(*outputStream, volume, m_wet, m_dry, wmaFileHeader.dwNumChannels == 1);
	else
		return driver.ApplyMixBins3D(*outputStream, volume, m_pan, m_wet, m_dry);
}

/////////////////////////////////////////////////////////////////////////////
// Play a disk/cd based WMA
/////////////////////////////////////////////////////////////////////////////
WMAStream::WMAStream(
SoundDriverServices &driverServices,
const char *fileName,
bool loop) :
SoundStreamXbox(driverServices, fileName, loop)
{
	// What are our caps?
	m_caps = STREAMCAP_POS | STREAMCAP_REWIND | STREAMCAP_VOLUME | STREAMCAP_PAN | STREAMCAP_PITCH;

	// Save this
	repeat = loop;

	// Intiailize the stuff
	Initiailize();

	// Save off the stream stuff
	type = BYFILENAME;
	ASSERT(strlen(fileName) < sizeof(streamFileName) - 1);
	strncpy(streamFileName, fileName, sizeof(streamFileName) - 1);
	streamFileName[sizeof(streamFileName) - 1];
	Media::MakeFileName(fileName, streamFileName);

	// Open it!
	Open();
}

/////////////////////////////////////////////////////////////////////////////
// Play a disk/cd based WMA
/////////////////////////////////////////////////////////////////////////////
WMAStream::WMAStream(
SoundDriverServices &driverServices,
ImbeddedStreamInfo &info,
const char *fileName,
bool loop) :
SoundStreamXbox(driverServices, fileName, loop)
{
	// What are our caps?
	m_caps = STREAMCAP_POS | STREAMCAP_REWIND | STREAMCAP_VOLUME | STREAMCAP_PAN | STREAMCAP_PITCH;

	// Save this
	repeat = loop;

	// Intiailize the stuff
	Initiailize();

	// Save off the stream stuff
	type = BYFILEOFFSET;
	streamFile = info.file;
	streamOffset = info.offset;

	ASSERT(strlen(fileName) < sizeof(streamFileName) - 1);
	strncpy(streamFileName, fileName, sizeof(streamFileName) - 1);
	streamFileName[sizeof(streamFileName) - 1];

	// Open it!
	Open();
}

/////////////////////////////////////////////////////////////////////////////
// Play a soundtrack based WMA
/////////////////////////////////////////////////////////////////////////////
WMAStream::WMAStream(
SoundDriverServices &driverServices,
DWORD soundTrackID,
bool loop) :
SoundStreamXbox(driverServices, "ST", loop)
{
	// doesn't work yet
	ASSERT(false);

	// What are our caps?
	m_caps = STREAMCAP_POS | STREAMCAP_REWIND | STREAMCAP_VOLUME | STREAMCAP_PAN | STREAMCAP_PITCH;

	// Save this
	repeat = loop;

	// Intialize the stuff
	Initiailize();

	// Save off the stream stuff
	type = BYSOUNDTRACK;
	streamFileName[0] = '\0';
	streamSoundTrackID = soundTrackID;

	// Open it!
	Open();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
WMAStream::~WMAStream()
{
	// Close it
	Close();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void WMAStream::Initiailize()
{
	// Not open yet
	openResult = E_PENDING;
	lastDecoderStatus = 0;

	// Don't pause
	paused = false;

	// Make sure we don't appear in the list
	processActive = false;

	// No song
  	soundTrackHandle = INVALID_HANDLE_VALUE;

	// Clear evertying
	inputStream = NULL;
	outputStream = NULL;

	filePos = 0;
	startingDataOffset = 0;

	// Make sure this is clean
	memset(&wmaFileHeader, '\0', sizeof(wmaFileHeader));

	doneRead = false;

#ifdef WMACONTENTDESC
	memset(&wmaContentDescription, '\0', sizeof(wmaContentDescription));
#endif
}

/////////////////////////////////////////////////////////////////////////////
// The WMA media object has been opened, start the playback process
/////////////////////////////////////////////////////////////////////////////
bool WMAStream::Open()
{
	// Create the decoder
	WMAXMODECODERPARAMETERS params = { 0 };
	if (type == BYFILENAME)
		params.pszFileName = streamFileName;
	else if (type == BYFILEOFFSET)
	{
		params.hFile = streamFile;
		params.dwFileOffset = streamOffset;
	}
	else if (type == BYSOUNDTRACK)
	{
		ASSERT(false);
	}
	params.dwLookaheadBufferSize   = WMASTRM_LOOKAHEAD_SIZE;

	// Create the source (wma file)
	HRESULT ret = XWmaDecoderCreateMediaObject((LPCWMAXMODECODERPARAMETERS)&params, &inputStream);

	// This is bad
	SoundError::SetLastError(ret);
	if (FAILED(ret))
	{
		openResult = E_FAIL;
		return false;
	}

	// init this stuff
	lastDecoderStatus = 0;
	for (int i = 0; i < WMASTRM_PACKET_COUNT; i++)
	{
		packet[i].status = XMEDIAPACKET_STATUS_SUCCESS;
		packet[i].size = 0;
	}

	// Register our thread
	processActive = true;
	driver.RegisterStream(this);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void WMAStream::Close()
{
	// Kill our process thread first thing!
	if (processActive)
	{
		driver.UnregisterStream(this);
		processActive = false;
	}

	// Clean up the streams
	if (inputStream)
	{
		inputStream->Release();
		inputStream = NULL;
	}

	if (outputStream)
	{
		outputStream->Release();
		outputStream = NULL;
	}

	// Close the file/source
  	if (soundTrackHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(soundTrackHandle);
	  	soundTrackHandle = INVALID_HANDLE_VALUE;
	}

#ifdef WMACONTENTDESC
	if (wmaContentDescription.pTitle)
	{
		delete [] wmaContentDescription.pTitle;
	 	wmaContentDescription.pTitle = NULL;
	}

	if (wmaContentDescription.pDescription)
	{
		delete [] wmaContentDescription.pDescription;
		wmaContentDescription.pDescription = NULL;
	}

	if (wmaContentDescription.pAuthor)
	{
		delete [] wmaContentDescription.pAuthor;
		wmaContentDescription.pAuthor = NULL;
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WMAStream::CreateStream()
{
	// Get the total wave file duration.  We'll use this to calculate how far
	// along in the file we are during processing.
	HRESULT ret = inputStream->GetFileHeader(&wmaFileHeader);
	SoundError::SetLastError(ret);
	if (FAILED(ret))
		return false;

	// Something is weird here, the GetFileHeader wasn't ready
	if (wmaFileHeader.dwNumChannels == 0)
	{
		TRACE("Create stream %s failed. Trying again!", streamFileName);
		lastDecoderStatus = 0;
		return true;
	}

	// What is the size going to be?
	fileSize = (wmaFileHeader.dwNumChannels * wmaFileHeader.dwSampleRate * wmaFileHeader.dwDuration) / 1000;

	// allocate buffers to hold the wma info
#ifdef WMACONTENTDESC
	wmaContentDescription.wTitleLength = 256;
	wmaContentDescription.wAuthorLength = 256;
	wmaContentDescription.wDescriptionLength = 256;
	wmaContentDescription.pTitle = new WCHAR [wmaContentDescription.wTitleLength];
	wmaContentDescription.pAuthor = new WCHAR [wmaContentDescription.wAuthorLength];
	wmaContentDescription.pDescription = new WCHAR [wmaContentDescription.wDescriptionLength];

	ASSERT(wmaContentDescription.pDescription != NULL);
	ASSERT(wmaContentDescription.pAuthor != NULL);
	ASSERT(wmaContentDescription.pTitle != NULL);

	// get some info about the wma content
	ret = inputStream->GetFileContentDescription(&wmaContentDescription);
	SoundError::SetLastError(ret);
	if (FAILED(ret))
		return false;
#endif

	// Create an appropriate WAVEFORMATEX struct
	XAudioCreatePcmFormat((WORD)wmaFileHeader.dwNumChannels, wmaFileHeader.dwSampleRate, (WORD)16, &wfxSourceFormat);

	// Setup the attr
	m_attr.InferFromWFX(wfxSourceFormat);

	// Create the render (DirectSoundStream) filter
	DSSTREAMDESC dssd;
	memset(&dssd, '\0', sizeof(dssd));
	dssd.dwMaxAttachedPackets = WMASTRM_PACKET_COUNT;
	dssd.lpwfxFormat = &wfxSourceFormat;

	outputStream = driver.CreateStream(dssd, wmaFileHeader.dwNumChannels == 1);
	if (!outputStream)
		return false;

	// Start out paused
	outputStream->Pause(DSSTREAMPAUSE_PAUSE);
	ApplyMixBins();

	// We are go!
	openResult = S_OK;
	doneRead = false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Name: Process()
// Desc: Performs any work necessary to keep the stream playing.
/////////////////////////////////////////////////////////////////////////////
void WMAStream::Process()
{
	// Don't access the stream if we are done
	if (doneRead || openResult == E_FAIL)
		return;

	// Let the decoder do some work - this is where it schedules its
	// asynchronous file I/O.
	inputStream->DoWork();

	// Since the WMA header is read asynchronously, we need to wait
	// until the decoder says it's ready to produce output.  Once
	// the decoder says it's ready, we can create our stream and
	// start playback.  It makes things a little more complex, but
	// it's better than blocking every time we start streaming a
	// new WMA file.
	if ((lastDecoderStatus & XMO_STATUSF_ACCEPT_OUTPUT_DATA) == 0)
	{
		HRESULT ret = inputStream->GetStatus(&lastDecoderStatus);
		if (FAILED(ret))
			return;

		// The decoder still isn't ready - bail
		if ((lastDecoderStatus & XMO_STATUSF_ACCEPT_OUTPUT_DATA) == 0)
			return;

		// If we get here, the decoder is ready to produce output.
		// Create our stream and start processing.
		if (!CreateStream())
		{
			openResult = E_FAIL;
			return;
		}

		// Are we in a retry?
		if ((lastDecoderStatus & XMO_STATUSF_ACCEPT_OUTPUT_DATA) == 0)
			return;
	}

	// Find a free packet.  If there's none free, we don't have anything  to do
	int packetIndex;
	while (FindFreePacket(packetIndex))
	{
		// Read from the source filter
		if (!ProcessSource(packetIndex))
			break;

		// Send the data to the renderer
		if (!ProcessRenderer(packetIndex))
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Name: FindFreePacket()
// Desc: Finds a render packet available for processing.
/////////////////////////////////////////////////////////////////////////////
bool WMAStream::FindFreePacket(
int &packetIndex)
{
	// Is this right?
	if (doneRead)
		return false;

	for (int i = 0; i < WMASTRM_PACKET_COUNT; i++)
	{
		if (packet[i].status != XMEDIAPACKET_STATUS_PENDING)
		{
			packetIndex = i;
			return true;
		}
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
// Name: ProcessSource()
// Desc: Reads data from the source filter.
/////////////////////////////////////////////////////////////////////////////
bool WMAStream::ProcessSource(
int packetIndex)
{
	// We're going to read a full packet's worth of data into the source
	// buffer.  Since we're playing in an infinite loop, we'll just spin
	// until we've read enough data, even if that means wrapping around the
	// end of the file.
	XMEDIAPACKET xmp;
	memset(&xmp, '\0', sizeof(xmp));
	xmp.pvBuffer = sourceBuffer + (packetIndex * WMASTRM_SOURCE_PACKET_BYTES);
	xmp.dwMaxSize = WMASTRM_SOURCE_PACKET_BYTES;
	xmp.pdwCompletedSize = &packet[packetIndex].size;

	DWORD totalSourceUsed = 0;
	while (!doneRead && totalSourceUsed < WMASTRM_SOURCE_PACKET_BYTES)
	{
		// Read from the source
		HRESULT ret = inputStream->Process(NULL, &xmp);
		if (FAILED(ret))
		{
#ifdef WATCHSTREAMERRORS
			char temp[256];
			sprintf(temp, "Failed process on stream %s\n", streamFileName);
			OutputDebugString(temp);
#endif
			//outputStream->Discontinuity();
			return false;
		}

		// Add the amount read to the total
		totalSourceUsed += packet[packetIndex].size;

		// Keep track of how much we read
		filePos += packet[packetIndex].size;

		// Are we done
		if (packet[packetIndex].size < xmp.dwMaxSize)
		{
			// Flush!
			inputStream->Flush();

			// Don't do any more if we are looping
			if (!repeat)
			{
				doneRead = true;
				inputStream->Discontinuity();
			}
			else
			{
				// If we read less than the amount requested, it's because we hit
				// the end of the file.  Seek back to the start and keep going.
				xmp.pvBuffer = (BYTE *)xmp.pvBuffer + packet[packetIndex].size;
				xmp.dwMaxSize = xmp.dwMaxSize - packet[packetIndex].size;

				filePos = 0;
			}
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Name: ProcessRenderer()
// Desc: Sends data to the renderer.
/////////////////////////////////////////////////////////////////////////////
bool WMAStream::ProcessRenderer(
int packetIndex)
{
	// Do we have enough data here?
	if (packet[packetIndex].size > 0)
	{
		// There's a full packet's worth of data ready for us to send to the
		// renderer.  We want to track the status of this packet since the
		// render filter is asychronous and we need to know when the packet is
		// completed.
		XMEDIAPACKET xmp;
		memset(&xmp, '\0', sizeof(xmp));
		xmp.pvBuffer  = sourceBuffer + (packetIndex * WMASTRM_SOURCE_PACKET_BYTES);
		xmp.dwMaxSize = packet[packetIndex].size;
		xmp.pdwStatus = &packet[packetIndex].status;

		if (!paused)
			outputStream->Pause(DSSTREAMPAUSE_RESUME);

		HRESULT ret = outputStream->Process(&xmp, NULL);
		SoundError::SetLastError(ret);
		if (FAILED(ret))
			return false;
	}

	if (doneRead)
		outputStream->Discontinuity();

	return (packet[packetIndex].size > 0);
}

/////////////////////////////////////////////////////////////////////////////
// Name: Pause
// Desc: Pauses and resumes stream playback
/////////////////////////////////////////////////////////////////////////////
void WMAStream::Pause(
bool pause)
{
	if (pause != paused)
	{
		if (outputStream)
			outputStream->Pause(pause ? DSSTREAMPAUSE_PAUSE : DSSTREAMPAUSE_RESUME);
		paused = pause;
	}
}


