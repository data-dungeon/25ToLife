/***************************************************************************/
// Query a driver about it's capabilities
/***************************************************************************/

/******************* includes ****************************/

#include "Audio/AudioPCH.h"





/***************************************************************************/
// Pass in the thing you are querying on the constructor
/***************************************************************************/
SoundDriverCaps::SoundDriverCaps(
SoundDriverEnumerator::SOUNDDRIVERID deviceID)
{
	// Create the bad boy
	driver = NULL;
	HRESULT ret = DirectSoundCreate(deviceID, &directSound, NULL);
	SoundError::SetLastError(ret);
	valid = (ret == DS_OK);

	// Query the caps
	if (valid)
		Refresh();
}

/***************************************************************************/
// Pass in the thing you are querying on the constructor
/***************************************************************************/
SoundDriverCaps::SoundDriverCaps(
SoundDriverWin32 &soundDriver)
{
	// we don't create on this one
	directSound = NULL;

	// Query the caps
	driver = &soundDriver;
	ASSERT(driver->directSound);
	Refresh();
}

/***************************************************************************/
// Clean up stuff
/***************************************************************************/
SoundDriverCaps::~SoundDriverCaps()
{
	if (directSound)
		directSound->Release();
}

/***************************************************************************/
// Refresh getting the caps (returns Valid())
/***************************************************************************/
bool SoundDriverCaps::Refresh()
{
#ifdef _XBOX
	return false;
#else
	HRESULT ret;

	memset(&caps, '\0', sizeof(caps));
	caps.dwSize = sizeof(caps);
	if (directSound)
		ret = directSound->GetCaps(&caps);
	else if (driver->directSound)
 		ret = driver->directSound->GetCaps(&caps);
	else
		ret = DSERR_NODRIVER;
	SoundError::SetLastError(ret);
	valid = (ret == DS_OK);

	return valid;
#endif
}

/***************************************************************************/
// Driver is certified by OS manufacturer
/***************************************************************************/
bool SoundDriverCaps::Certified()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return ((caps.dwFlags & DSCAPS_CERTIFIED) != 0);
#endif
}

/***************************************************************************/
// Driver is emulated, which means performance is crappy
/***************************************************************************/
bool SoundDriverCaps::Emulated()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return ((caps.dwFlags & DSCAPS_EMULDRIVER) != 0);
#endif
}

/***************************************************************************/
// Driver supports 16-bit output
/***************************************************************************/
bool SoundDriverCaps::Output16Bit()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return ((caps.dwFlags & DSCAPS_PRIMARY16BIT) != 0);
#endif
}

/***************************************************************************/
// Driver supports 8-bit output
/***************************************************************************/
bool SoundDriverCaps::Output8Bit()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return ((caps.dwFlags & DSCAPS_PRIMARY8BIT) != 0);
#endif
}

/***************************************************************************/
// Driver supports stereo output
/***************************************************************************/
bool SoundDriverCaps::OutputStereo()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return ((caps.dwFlags & DSCAPS_PRIMARYSTEREO) != 0);
#endif
}

/***************************************************************************/
// Driver supports mono output
/***************************************************************************/
bool SoundDriverCaps::OutputMono()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return ((caps.dwFlags & DSCAPS_PRIMARYMONO) != 0);
#endif
}

/***************************************************************************/
// Driver supports 16-bit input
/***************************************************************************/
bool SoundDriverCaps::Input16Bit()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return ((caps.dwFlags & DSCAPS_SECONDARY16BIT) != 0);
#endif
}

/***************************************************************************/
// Driver supports 8-bit input
/***************************************************************************/
bool SoundDriverCaps::Input8Bit()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return ((caps.dwFlags & DSCAPS_SECONDARY8BIT) != 0);
#endif
}

/***************************************************************************/
// Driver supports stereo input
/***************************************************************************/
bool SoundDriverCaps::InputStereo()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return ((caps.dwFlags & DSCAPS_SECONDARYSTEREO) != 0);
#endif
}

/***************************************************************************/
// Driver supports mono input
/***************************************************************************/
bool SoundDriverCaps::InputMono()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return ((caps.dwFlags & DSCAPS_SECONDARYMONO) != 0);
#endif
}

/***************************************************************************/
// Max input sample rates
/***************************************************************************/
unsigned int SoundDriverCaps::MaximumInputSampleRate()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return (unsigned int)caps.dwMaxSecondarySampleRate;
#endif
}

/***************************************************************************/
// Min input sample rates
/***************************************************************************/
unsigned int SoundDriverCaps::MinimumInputSampleRate()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return (unsigned int)caps.dwMinSecondarySampleRate;
#endif
}

/***************************************************************************/
// Maximum number of hardware mixing buffers
/***************************************************************************/
int SoundDriverCaps::MaximumTotalHardwareMixingBuffers()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return (int)caps.dwMaxHwMixingAllBuffers;
#endif
}

/***************************************************************************/
// Maximum number of "static" hardware mixing buffers
/***************************************************************************/
int SoundDriverCaps::MaximumStaticHardwareMixingBuffers()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return (int)caps.dwMaxHwMixingStaticBuffers;
#endif
}

/***************************************************************************/
// Maximum number of "streaming" hardware mixing buffers
/***************************************************************************/
int SoundDriverCaps::MaximumStreamingHardwareMixingBuffers()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return (int)caps.dwMaxHwMixingStreamingBuffers;
#endif
}

/***************************************************************************/
// Free hardware mixing buffers
/***************************************************************************/
int SoundDriverCaps::FreeTotalHardwareMixingBuffers()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return (int)caps.dwFreeHwMixingAllBuffers;
#endif
}

/***************************************************************************/
// Free "static" hardware mixing buffers
/***************************************************************************/
int SoundDriverCaps::FreeStaticHardwareMixingBuffers()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return (int)caps.dwFreeHwMixingStaticBuffers;
#endif
}

/***************************************************************************/
// Free "streaming" hardware mixing buffers
/***************************************************************************/
int SoundDriverCaps::FreeStreamingHardwareMixingBuffers()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return (int)caps.dwFreeHwMixingStreamingBuffers;
#endif
}

/***************************************************************************/
// Maximum number of hardware mixing buffers
/***************************************************************************/
int SoundDriverCaps::MaximumTotalHardware3DMixingBuffers()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return (int)caps.dwMaxHw3DAllBuffers;
#endif
}

/***************************************************************************/
// Maximum number of "static" hardware mixing buffers
/***************************************************************************/
int SoundDriverCaps::MaximumStaticHardware3DMixingBuffers()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return (int)caps.dwMaxHw3DStaticBuffers;
#endif
}

/***************************************************************************/
// Maximum number of "streaming" hardware mixing buffers
/***************************************************************************/
int SoundDriverCaps::MaximumStreamingHardware3DMixingBuffers()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return (int)caps.dwMaxHw3DStreamingBuffers;
#endif
}

/***************************************************************************/
// Free hardware mixing buffers
/***************************************************************************/
int SoundDriverCaps::FreeTotalHardware3DMixingBuffers()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return (int)caps.dwFreeHw3DAllBuffers;
#endif
}

/***************************************************************************/
// Free "static" hardware mixing buffers
/***************************************************************************/
int SoundDriverCaps::FreeStaticHardware3DMixingBuffers()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return (int)caps.dwFreeHw3DStaticBuffers;
#endif
}

/***************************************************************************/
// Free "streaming" hardware mixing buffers
/***************************************************************************/
int SoundDriverCaps::FreeStreamingHardware3DMixingBuffers()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return (int)caps.dwFreeHw3DStreamingBuffers;
#endif
}

/***************************************************************************/
// How much hardware memory is there?
/***************************************************************************/
unsigned int SoundDriverCaps::TotalHardwareMemory()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return (unsigned int)caps.dwTotalHwMemBytes;
#endif
}

/***************************************************************************/
// How much free hardware memory is there?
/***************************************************************************/
unsigned int SoundDriverCaps::FreeHardwareMemory()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return (unsigned int)caps.dwFreeHwMemBytes;
#endif
}

/***************************************************************************/
// Largest free block available
/***************************************************************************/
unsigned int SoundDriverCaps::LargestFreeHardwareBlock()
{
#ifdef _XBOX
	return false;
#else
	ASSERT(valid);
	return (unsigned int)caps.dwMaxContigFreeHwMemBytes;
#endif
}
