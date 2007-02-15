/***************************************************************************/
// Wave file support class
//
// Portions copyright (C) 1995-1997 Microsoft Corporation.
//				All Rights Reserved.
/***************************************************************************/
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>

#include "wavefile.h"

/***************************************************************************/
// Init stuff
/***************************************************************************/
WaveFile::WaveFile(void)
{
	mmioIn = NULL;
	wfx = NULL;
	wfxSize = 0;
}

/***************************************************************************/
// Make sure the file is closed
/***************************************************************************/
WaveFile::~WaveFile(void)
{
	Close();
}

/***************************************************************************/
// This function will open a wave input file and prepare it for reading,
// so the data can be easily read.
// Jesus, Microsoft programmers love goto's.
/***************************************************************************/
WaveFile::WAVEERROR WaveFile::Open(
char *fileName)
{
	PCMWAVEFORMAT	pcmWaveFormat;		// Temp PCM structure to load in.
	WORD				extraAlloc;			// Extra bytes for waveformatex
	WAVEERROR		error = OKAY;		// Return value.
	int				tempWfxSize;      // true size of the wfx structure

	// Make sure a file isn't already open
	if (mmioIn)
		Close();

	// Open the file
	mmioIn = mmioOpen(fileName, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if (!mmioIn)
	{
		// Try adding an extension to the file
		char waveFileName[128];
		strcpy(waveFileName, fileName);
		strcat(waveFileName, ".wav");

		// Try again
		mmioIn = mmioOpen(waveFileName, NULL, MMIO_ALLOCBUF | MMIO_READ);
	}

	// Did we open a file?
	if (!mmioIn)
	{
		error = CANNOTOPEN;
		goto ERROR_READING_WAVE;
	}

	// Start the descent
	if ((int)mmioDescend(mmioIn, &ckInRIFF, NULL, 0) != 0)
	{
		error = NOTWAVEFILE;
		goto ERROR_READING_WAVE;
	}

	// Wave file?
	if ((ckInRIFF.ckid != FOURCC_RIFF) || (ckInRIFF.fccType != mmioFOURCC('W', 'A', 'V', 'E')))
	{
		error = NOTWAVEFILE;
		goto ERROR_READING_WAVE;
	}

	// Search the input file for for the 'fmt ' chunk.
	ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (mmioDescend(mmioIn, &ckIn, &ckInRIFF, MMIO_FINDCHUNK) != 0)
	{
		error = NOTWAVEFILE;
		goto ERROR_READING_WAVE;
	}

	// Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
	// if there are extra parameters at the end, we'll ignore them
	if (ckIn.cksize < (long)sizeof(PCMWAVEFORMAT))
	{
		error = NOTWAVEFILE;
		goto ERROR_READING_WAVE;
	}

	// Read the 'fmt ' chunk into <pcmWaveFormat>.
	if (mmioRead(mmioIn, (HPSTR)&pcmWaveFormat, (long)sizeof(pcmWaveFormat)) != (long)sizeof(pcmWaveFormat))
	{
		error = CANNOTREAD;
		goto ERROR_READING_WAVE;
	}

	// Ok, allocate the waveformatex, but if its not pcm
	// format, read the next word, and thats how many extra
	// bytes to allocate.
	if (pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM)
		extraAlloc = 0;
	else
	{
		// Read in length of extra bytes.
		if (mmioRead(mmioIn, (LPSTR)&extraAlloc, (long)sizeof(extraAlloc)) != (long)sizeof(extraAlloc))
		{
			error = CANNOTREAD;
			goto ERROR_READING_WAVE;
		}
	}

	// Ok, now allocate that waveformatex structure.
	tempWfxSize = sizeof(WAVEFORMATEX) + extraAlloc;
	if ((wfx = (WAVEFORMATEX *)malloc(tempWfxSize)) == NULL)
	{
		error = MEMORY;
		goto ERROR_READING_WAVE;
	}
	wfxSize = tempWfxSize;

	// Copy the bytes from the pcm structure to the waveformatex structure
	memcpy(wfx, &pcmWaveFormat, sizeof(pcmWaveFormat));
	wfx->cbSize = extraAlloc;

	// Now, read those extra bytes into the structure, if extraAlloc != 0.
	if (extraAlloc != 0)
	{
		LPSTR dest = (LPSTR)(((BYTE *)&(wfx->cbSize)) + sizeof(extraAlloc));
		if (mmioRead(mmioIn, dest, (long)extraAlloc) != (long)extraAlloc)
		{
			error = NOTWAVEFILE;
			goto ERROR_READING_WAVE;
		}
	}

	/* Ascend the input file out of the 'fmt ' chunk. */
	if (mmioAscend(mmioIn, &ckIn, 0) != 0)
	{
		error = NOTWAVEFILE;
		goto ERROR_READING_WAVE;
	}

	// We are done
	return(error);

ERROR_READING_WAVE:
	// Something bad happened
	Close();
	return(error);
}

/***************************************************************************/
// This routine has to be called before Read as it searchs for the chunk to
// descend into for reading, that is, the 'data' chunk.  For simplicity, this
// used to be in the open routine, but was taken out and moved to a separate
// routine so there was more control on the chunks that are before the data
// chunk, such as 'fact', etc...
/***************************************************************************/
WaveFile::WAVEERROR WaveFile::Start(void)
{
	// Duh?
	if (!mmioIn)
		return(NOTOPEN);

	// Do a nice little seek...
	int seek = mmioSeek(mmioIn, ckInRIFF.dwDataOffset + sizeof(FOURCC), SEEK_SET);
	if (seek == -1)
		return(CORRUPTWAVEFILE);

	// Search the input file for for the 'data' chunk.
	ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(mmioIn, &ckIn, &ckInRIFF, MMIO_FINDCHUNK) != 0)
		return(NOTWAVEFILE);

	return(OKAY);
}

/***************************************************************************/
// This will read wave data from the wave file.  Makre sure we're descended into
//	the data chunk, else this will fail bigtime!
// read          - # of bytes to read.
//	dest          - Destination buffer to put bytes.
//	actualRead- # of bytes actually read.
/***************************************************************************/
WaveFile::WAVEERROR WaveFile::Read(
UINT read,
BYTE *dest,
UINT *actualRead)
{
	// Do we have a file yet?
	if (!mmioIn)
		return(NOTOPEN);

	// Assume no errors
	WAVEERROR error = OKAY;
	UINT dataIn, i;

	// Get status
	MMIOINFO	mmioInfo;
	if (mmioGetInfo(mmioIn, &mmioInfo, 0) != 0)
	{
		error = CANNOTREAD;
		goto ERROR_CANNOT_READ;
	}

	// How much to they want?
	dataIn = read;
	if (dataIn > ckIn.cksize)
		dataIn = ckIn.cksize;
	ckIn.cksize -= dataIn;

	// Do the read
	for (i = 0; i < dataIn; i++)
	{
		// Copy the bytes from the io to the buffer.
		if (mmioInfo.pchNext == mmioInfo.pchEndRead)
		{
			if (mmioAdvance(mmioIn, &mmioInfo, MMIO_READ) != 0)
			{
				error = CANNOTREAD;
				goto ERROR_CANNOT_READ;
			}
			if (mmioInfo.pchNext == mmioInfo.pchEndRead)
			{
				error = CORRUPTWAVEFILE;
				goto ERROR_CANNOT_READ;
			}
		}

		// Actual copy.
		dest[i] = (BYTE)*mmioInfo.pchNext;
		++mmioInfo.pchNext;
	}

	// Set the info
	if (mmioSetInfo(mmioIn, &mmioInfo, 0) != 0)
	{
		error = CANNOTREAD;
		goto ERROR_CANNOT_READ;
	}

	// How much did we really read?
	*actualRead = dataIn;
	return(error);

ERROR_CANNOT_READ:
	// Something really bad happened
	*actualRead = 0;
	return(error);
}

/***************************************************************************/
// This will close the wave file openned with WaveOpenFile.
// Returns OKAY if successful, non-zero if there was a warning.
/***************************************************************************/
WaveFile::WAVEERROR WaveFile::Close(void)
{
	if (wfx)
	{
		free(wfx);
		wfx = NULL;
	}

	if (mmioIn)
	{
		mmioClose(mmioIn, 0);
		mmioIn = NULL;
	}

	return(OKAY);
}

/***************************************************************************/
//	This routine loads a full wave file into memory.  Be careful, wave files
// can get pretty big these days :).
//	Returns OKAY if successful, else the error code.
/***************************************************************************/
WaveFile::WAVEERROR WaveFile::LoadFile(
char *fileName,
UINT *size,
WAVEFORMATEX **wfx,
BYTE **data)
{
	UINT dataSize, wfxSize;

	// init stuff
	*size = 0;
	*wfx = NULL;
	*data = NULL;

	// Open the file
	WAVEERROR error;
	WaveFile waveRead;
	if ((error = waveRead.Open(fileName)) != OKAY)
		goto ERROR_LOADING;

	// Start the read
	if ((error = waveRead.Start()) != OKAY)
		goto ERROR_LOADING;

	// Ok, size of wave data is in the chunk info, allocate that buffer.
	dataSize = waveRead.ChunkInfo().cksize;
	if ((*data = (BYTE *)malloc(dataSize)) == NULL)
	{
		error = MEMORY;
		goto ERROR_LOADING;
	}

	// Read it in all at once
	UINT actualRead;
	if ((error = waveRead.Read(dataSize, *data, &actualRead)) != OKAY)
		goto ERROR_LOADING;

	// We are kosher on the read
	*size = actualRead;

	// Give them a copy of the wfx
	wfxSize = waveRead.WaveFormatSize();
	if ((*wfx = (WAVEFORMATEX *)malloc(wfxSize)) == NULL)
	{
		error = MEMORY;
		goto ERROR_LOADING;
	}
	memcpy(*wfx, waveRead.WaveFormat(), wfxSize);

	goto DONE_LOADING;

ERROR_LOADING:
	// Clean up stuff
	DoneFile(wfx, data);

DONE_LOADING:
	// Close the wave file.
	waveRead.Close();
	return(error);
}

/***************************************************************************/
// We are done with the file
/***************************************************************************/
void WaveFile::DoneFile(
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


