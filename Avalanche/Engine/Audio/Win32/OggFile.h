/***************************************************************************/
// Ogg file support class
/***************************************************************************/
#ifndef OGGFILE_H
#define OGGFILE_H
#include <vorbis\vorbisfile.h>

// the OggFile class is designed to be functionally equivalent to the 
// waveFile class to ease integration with the existing sound system
// 
#include "Audio/Encryption/fileEncrypter.h"

class OggFile
{
public:
	// These really don't do that much
	OggFile();
	~OggFile();

	// Error returns
	// keep these the same as the existing wave file
	// stuff to save us time and effort
	typedef enum
	{
		OKAY = 0,
		MEMORY,
		CANNOTOPEN,
		NOTWAVEFILE,
		CANNOTREAD,
		CORRUPTWAVEFILE
		//NOTOPEN
	} WAVEERROR;
	
	// Open, start, read, then close
	WAVEERROR Open(char *);
	WAVEERROR Start();
	WAVEERROR Read(UINT read, char *dest, UINT *actualRead);
	WAVEERROR Close();

	// After opening, you can query the structures
	const WAVEFORMATEX *WaveFormat()
		{ return wfx; }
	int WaveFormatSize()
		{ return wfxSize; }


	// A nice handy utility funciton to load a wave file
	static WAVEERROR LoadFile(char *fileName, UINT *size, WAVEFORMATEX **wfx, BYTE **data);

	// Call this to free up the memory created with LoadFile
	static void DoneFile(WAVEFORMATEX **wfx, BYTE **data);

private:
	WAVEFORMATEX		*wfx;			// Wave format info
	int					wfxSize;		// Size of the wfx
	FILE*				inFile;			// binary data stream for vorbis file	
	OggVorbis_File		vorbisFile;

	char*				buffer;			/* some space for us to play in */
	int					bytes;
	int					convSize;
	int					wordSize;

	FileEncrypter		encrypter;

};

#endif

