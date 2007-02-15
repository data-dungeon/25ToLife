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
	WaveFile(void);
	~WaveFile(void);

	// Error returns
	enum WAVEERROR
	{
		OKAY = 0,
		MEMORY,
		CANNOTOPEN,
		NOTWAVEFILE,
		CANNOTREAD,
		CORRUPTWAVEFILE,
		NOTOPEN
	};

	// Open, start, read, then close
	WAVEERROR Open(char *);
	WAVEERROR Start(void);
	WAVEERROR Read(UINT read, BYTE *dest, UINT *actualRead);
	WAVEERROR Close(void);

	// After opening, you can query the structures
	const WAVEFORMATEX *WaveFormat(void)
		{ return(wfx); }
	int WaveFormatSize(void)
		{ return(wfxSize); }

	// Return the info on the current chunk
#ifndef _XBOX
	const MMCKINFO &ChunkInfo(void)
		{ return(ckIn); }
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

