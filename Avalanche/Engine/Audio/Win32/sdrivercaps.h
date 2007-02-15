/***************************************************************************/
// Query a driver about it's capabilities
/***************************************************************************/
#ifndef SDRIVERCAPS_H
#define SDRIVERCAPS_H

#include "Audio/Win32/sdriver.h"

// Query a driver for it's capabilities
class SoundDriverCaps
{
public:
	// Pass in the thing you are querying on the constructor
	SoundDriverCaps(SoundDriverEnumerator::SOUNDDRIVERID deviceID = NULL);
	SoundDriverCaps(SoundDriverWin32 &soundDriver);
	~SoundDriverCaps();

	// Refresh getting the attr (returns Valid())
	bool Refresh();

	// Did something go wrong with getting the attr?
	bool Valid()
		{ return valid; }

	//----------------------------------------------------------------
	// Here are all the attr functions
	// Note that many of these functions return what the driver CAN do
	// not necessarily how it is currently configured.
	//----------------------------------------------------------------

	// Driver is certified by OS manufacturer
	bool Certified();

	// Driver is emulated, which means performance is crappy
	bool Emulated();

	// Driver supports 16/8-bit output
	bool Output16Bit();
	bool Output8Bit();

	// Driver supports mono/stereo output
	bool OutputStereo();
	bool OutputMono();

	// Driver supports 16/8-bit input
	bool Input16Bit();
	bool Input8Bit();

	// Driver supports mono/stereo input
	bool InputStereo();
	bool InputMono();

	// Min/max input sample rates
	unsigned int MaximumInputSampleRate();
	unsigned int MinimumInputSampleRate();

	// Maximum number of hardware mixing buffers
	int MaximumTotalHardwareMixingBuffers();

	// Maximum number of "static" hardware mixing buffers
	int MaximumStaticHardwareMixingBuffers();

	// Maximum number of "streaming" hardware mixing buffers
	int MaximumStreamingHardwareMixingBuffers();

	// Free hardware mixing buffers
	int FreeTotalHardwareMixingBuffers();

	// Free "static" hardware mixing buffers
	int FreeStaticHardwareMixingBuffers();

	// Free "streaming" hardware mixing buffers
	int FreeStreamingHardwareMixingBuffers();

	// Maximum number of hardware mixing buffers
	int MaximumTotalHardware3DMixingBuffers();

	// Maximum number of "static" hardware mixing buffers
	int MaximumStaticHardware3DMixingBuffers();

	// Maximum number of "streaming" hardware mixing buffers
	int MaximumStreamingHardware3DMixingBuffers();

	// Free hardware mixing buffers
	int FreeTotalHardware3DMixingBuffers();

	// Free "static" hardware mixing buffers
	int FreeStaticHardware3DMixingBuffers();

	// Free "streaming" hardware mixing buffers
	int FreeStreamingHardware3DMixingBuffers();

	// How much hardware memory is there?
	unsigned int TotalHardwareMemory();

	// How much free hardware memory is there?
	unsigned int FreeHardwareMemory();

	// Largest free block available
	unsigned int LargestFreeHardwareBlock();

private:
	LPDIRECTSOUND			directSound;	// The direct sound object
	SoundDriverWin32		*driver;			// driver
	bool						valid;			// valid attr
	DSCAPS					caps;				// The caps
};

#endif
