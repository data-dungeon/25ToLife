/***************************************************************************/
// Wave file support class
/***************************************************************************/
#ifndef WAVEFILE_H
#define WAVEFILE_H

#undef WIN32_LEAN_AND_MEAN
#ifdef _XBOX
#else
#include <windows.h>
#include <mmsystem.h>
#endif

class WaveFile
{
public:
	// These really don't do that much
	WaveFile();
	~WaveFile();

	// Error returns
	typedef enum
	{
		OKAY = 0,
		MEMORY,
		CANNOTOPEN,
		NOTWAVEFILE,
		CANNOTREAD,
		CORRUPTWAVEFILE,
#ifdef DIRECTX_PC
		UNOPEN
#else
		NOTOPEN
#endif
	} WAVEERROR;

	// Open, start, read, then close
	WAVEERROR Open(char *);
	WAVEERROR Start();
	WAVEERROR Read(UINT read, BYTE *dest, UINT *actualRead);
	WAVEERROR Close();

	// After opening, you can query the structures
	const WAVEFORMATEX *WaveFormat()
		{ return wfx; }
	int WaveFormatSize()
		{ return wfxSize; }

	// Return the info on the current chunk
#ifndef _XBOX
	const MMCKINFO &ChunkInfo()
		{ return ckIn; }
#endif

	// A nice handy utility funciton to load a wave file
	static WAVEERROR LoadFile(char *fileName, UINT *size, WAVEFORMATEX **wfx, BYTE **data);

	// Call this to free up the memory created with LoadFile
	static void DoneFile(WAVEFORMATEX **wfx, BYTE **data);

private:
	WAVEFORMATEX   *wfx;			// Wave format info
	int				wfxSize;		// Size of the wfx
#ifndef _XBOX
	HMMIO 			mmioIn;		// File hadnle for multimedia i/o
	MMCKINFO 		ckInRIFF;    // RIFF chunk
	MMCKINFO       ckIn;			// chunk info. for general use.
#endif
};

#endif

