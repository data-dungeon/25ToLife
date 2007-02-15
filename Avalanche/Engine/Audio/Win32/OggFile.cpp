/***************************************************************************/
// Ogg file support class
//
// Portions copyright (C) 1995-1997 Microsoft Corporation.
//				All Rights Reserved.
// Portions taken from the Ogg Vorbis decoder example
// (C) 1994-2002 
/***************************************************************************/

/******************* includes ****************************/

#include "Audio/AudioPCH.h"


#ifdef DIRECTX_PC

/***************************************************************************/
// Init stuff
/***************************************************************************/
OggFile::OggFile()
{
	inFile = NULL;
	wfx = NULL;
	wfxSize = 0;
}

/***************************************************************************/
// Make sure the file is closed
/***************************************************************************/
OggFile::~OggFile()
{
	Close();
}

/***************************************************************************/
// This function will open a ogg input file and prepare it for wave reading,
// so the data can be easily read.
/***************************************************************************/
OggFile::WAVEERROR OggFile::Open(
char *fileName)
{
	WAVEERROR		error = OKAY;		// Return value.

	// Make sure a file isn't already open
	if (inFile)
		Close();

	// Open the file
	inFile = ::fopen(fileName, "rb");
	if (!inFile)
	{
		// Try adding an extension to the file
		char waveFileName[128];
		strcpy(waveFileName, fileName);
		strcat(waveFileName, ".wav");

		// Try again
		inFile = fopen(waveFileName, "rb");
	}

	// Did we open a file?
	if (!inFile)
	{
		error = CANNOTOPEN;
		goto ERROR_READING_OGG;
	}

	// decrypt our file

	inFile = encrypter.getDecryptedFile(inFile);
	// could we decrypt it?
	if (!inFile)
	{
		error = CANNOTOPEN;
		goto ERROR_READING_OGG;
	}

	int result =0;
	result = ov_open(inFile, &vorbisFile, 0, 0);
	if (result != 0)
	{
		error = NOTWAVEFILE;
		goto ERROR_READING_OGG;
	}

	vorbis_info* vorbisInfo = ov_info(&vorbisFile, -1);

	// fill out our WAVEFORMATEX struct for the rest of the
	// system
	wfx = (WAVEFORMATEX *)malloc(sizeof(WAVEFORMATEX));
	wfx->nChannels = vorbisInfo->channels;
	wfx->nSamplesPerSec = vorbisInfo->rate;
	wfx->wBitsPerSample = 16;
	wfx->wFormatTag = WAVE_FORMAT_PCM;
	wfx->nBlockAlign = (vorbisInfo->channels * 2);
	wfx->cbSize = 0;
	wfx->nAvgBytesPerSec = vorbisInfo->rate * wfx->nBlockAlign;

	// fill out our specific management stuff
	wordSize = 2;

	// We are done
	return error;

ERROR_READING_OGG:
	// Something bad happened
	Close();
	return error;
}

/***************************************************************************/
// This routine has to be called before Read as it searchs for the chunk to
// descend into for reading, that is, the 'data' chunk.  For simplicity, this
// used to be in the open routine, but was taken out and moved to a separate
// routine so there was more control on the chunks that are before the data
// chunk, such as 'fact', etc...
/***************************************************************************/
OggFile::WAVEERROR OggFile::Start()
{
	//// Duh?
	//if (!mmioIn)
	//	return NOTOPEN;

	//// Do a nice little seek...
	//int seek = mmioSeek(mmioIn, ckInRIFF.dwDataOffset + sizeof(FOURCC), SEEK_SET);
	//if (seek == -1)
	//	return CORRUPTWAVEFILE;

	//// Search the input file for for the 'data' chunk.
	//ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
	//if (mmioDescend(mmioIn, &ckIn, &ckInRIFF, MMIO_FINDCHUNK) != 0)
	//	return NOTWAVEFILE;

	return OKAY;
}

/***************************************************************************/
// This will read wave data from the wave file.  Makre sure we're descended into
//	the data chunk, else this will fail bigtime!
// read          - # of bytes to read.
//	dest          - Destination buffer to put bytes.
//	actualRead- # of bytes actually read.
/***************************************************************************/
OggFile::WAVEERROR OggFile::Read(
UINT read,
char *dest,
UINT *actualRead)
{
	// Do we have a file yet?
	if (!inFile)
		return CANNOTREAD;

	// Assume no errors
	WAVEERROR error = OKAY;
	
	unsigned int bytesRead = 0;
	int bitStream;
	int currentRead;
	do {
		currentRead = ov_read(	&vorbisFile, 
								dest + bytesRead,
								read - bytesRead,
								0,
								wordSize, 
								1, 
								&bitStream
							  );
		bytesRead += currentRead;
	} while (currentRead && bytesRead < read);

	// How much did we really read?
	*actualRead = bytesRead;
	return error;
}

/***************************************************************************/
// This will close the wave file openned with WaveOpenFile.
// Returns OKAY if successful, non-zero if there was a warning.
/***************************************************************************/
OggFile::WAVEERROR OggFile::Close()
{
	if (wfx)
	{
		free(wfx);
		wfx = NULL;
	}

	if (inFile)
	{
		ov_clear(&vorbisFile);
		fclose(inFile);
		inFile = NULL;
	}

	return OKAY;
}

/***************************************************************************/
//	This routine loads a full wave file into memory.  Be careful, wave files
// can get pretty big these days :).
//	Returns OKAY if successful, else the error code.
/***************************************************************************/
OggFile::WAVEERROR OggFile::LoadFile(
char *fileName,
UINT *size,
WAVEFORMATEX **wfx,
BYTE **data)
{
	/*UINT dataSize, wfxSize;*/

//	// init stuff
//	*size = 0;
//	*wfx = NULL;
//	*data = NULL;
//
//	// Open the file
//	WAVEERROR error;
//	OggFile OggRead;
//	if ((error = OggRead.Open(fileName)) != OKAY)
//		goto ERROR_LOADING;
//
//	// Start the read
//	if ((error = OggRead.Start()) != OKAY)
//		goto ERROR_LOADING;
//
//	// Ok, size of wave data is in the chunk info, allocate that buffer.
//	dataSize = OggRead.ChunkInfo().cksize;
//	if ((*data = (BYTE *)malloc(dataSize)) == NULL)
//	{
//		error = MEMORY;
//		goto ERROR_LOADING;
//	}
//
//	// Read it in all at once
//	UINT actualRead;
//	if ((error = OggRead.Read(dataSize, *data, &actualRead)) != OKAY)
//		goto ERROR_LOADING;
//
//	// We are kosher on the read
//	*size = actualRead;
//
//	// Give them a copy of the wfx
//	wfxSize = OggRead.WaveFormatSize();
//	if ((*wfx = (WAVEFORMATEX *)malloc(wfxSize)) == NULL)
//	{
//		error = MEMORY;
//		goto ERROR_LOADING;
//	}
//	memcpy(*wfx, OggRead.WaveFormat(), wfxSize);
//
//	goto DONE_LOADING;
//
//ERROR_LOADING:
//	// Clean up stuff
//	DoneFile(wfx, data);
//
//DONE_LOADING:
//	// Close the wave file.
//	OggRead.Close();
	return OKAY;
}

/***************************************************************************/
// We are done with the file
/***************************************************************************/
void OggFile::DoneFile(
WAVEFORMATEX **wfx,
BYTE **data)
{
	if (*wfx)
	{
		free(*wfx);
		*wfx = NULL;
	}

	if (*data)
	{
		free(*data);
		*data = NULL;
	}
}


#endif