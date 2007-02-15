/***************************************************************************/
// File i/o wrapper that sits on top of the media stuff
/***************************************************************************/

/******************* includes ****************************/

#include "EngineHelper/EngineHelperPCH.h"
#include "GameHelper/compression/compress2.h"
#include "EngineHelper/NameLibrary.h"
#include "Game/Managers/ResourceManager.h"

#define	WRITE_BUFFER_SIZE		( 8 * 1024)

/***************************************************************************/
// These do nothing, there is no exception handling
/***************************************************************************/
FileStream::FileStream(void)
{
#ifdef USESTDIO
	fh = NULL;
#else
	m_data = NULL;
	m_pos = 0;
	m_mediaFile = NULL;
	m_bMemFree = true;		// filestream is typically responsible for free'ing data
	m_bPermanent = false;	// permanent pre-loaded files will be told that they are permanent
	m_writeBuffer = NULL;
	m_writeBufferPos = 0;
#endif
}

/***************************************************************************/
/***************************************************************************/
FileStream::~FileStream(void)
{
	Close();
}

/***************************************************************************/
// static utilties
/***************************************************************************/
bool FileStream::Exists(
char *filename)
{
	return Media::FileExists(filename);
}

/***************************************************************************/
/***************************************************************************/
FileStream *FileStream::NewAndOpen(
const char *filename,
OPENMODE mode,
bool text)
{
	// Create the object
	MEM_SET_ONE_SHOT_NEW_NAME("FileStream::NewAndOpen");
	FileStream *fs = new FileStream;
	if (fs)
	{
		// Open the file
		bool ret = fs->Open(filename, mode, text);
		if (!ret)
		{
			delete fs;
			return(NULL);
		}
	}

	return(fs);
}

/***************************************************************************/
// Open the file
/***************************************************************************/
bool FileStream::Open(
const char *filename,
OPENMODE mode,
bool text)
{
	// Make sure nothing is open
	Close();

#ifdef USESTDIO
	char *modeString;
	if (mode == READ)
		modeString = text ? "rt" : "rb";
	else
		modeString = text ? "wt" : "wb";

	fh = fopen(filename, modeString);
	return(fh != NULL);

#else //USESTDIO

	t_Error	Error;
	m_mediaFile = NULL;
	m_data = NULL;
	m_size = 0;
	m_pos = 0;
	m_bMemFree = true;	// filestream is typically responsible for free'ing data

	if( mode == WRITE)
	{
		// Just open and get out
		m_mediaFile = Media::Open( filename, &Error, MEDOPEN_FLAG_WRITE);
		m_writeBuffer = (char *)memAllocForIO( WRITE_BUFFER_SIZE );
		m_writeBufferPos = 0;
		return(m_mediaFile != NULL);
	}

	/* if pre-loaded, just point to data and done */

	FileStream *pPreLoadedFileStream = NameLibrary::FindFileStream(filename);
	if (pPreLoadedFileStream)
	{
		m_data = pPreLoadedFileStream->m_data;
		m_size = pPreLoadedFileStream->m_size;
		m_bMemFree = false;	// filestream did not malloc this data
		m_pos = 0;
		return(true);
	}

	// Read the file
	s32 dataSize = 0;
	t_Error err = 0;
	m_data = (char*) Media::Load(filename, &dataSize, &err);
	if (!m_data)
		return false;

	// save these for later
	m_size = dataSize;
	m_pos = 0;
	return(true);

#endif //USESTDIO
}

/***************************************************************************/
// Close the file
/***************************************************************************/
void FileStream::Close(void)
{
#ifdef USESTDIO
	if (fh)
	{
		fclose(fh);
		fh = NULL;
	}
#else
	Flush();
	
	if(m_writeBuffer)
	{
		memFree( m_writeBuffer);
		m_writeBuffer = NULL;
	}

	// If we opened for writing, close it now
	if (m_mediaFile)
	{
		m_mediaFile->Close(NULL);
		m_mediaFile = NULL;
	}

	// Reset these
	m_size = m_pos = 0;

	// Free our data
	if ((m_bMemFree) && (m_data))
		memFree(m_data);
	m_data = NULL;
#endif
}

/***************************************************************************/
// Flush any pending bufferred writes
/***************************************************************************/
void FileStream::Flush(void)
{
#ifdef USESTDIO
	if (fh)
	{
		fflush(fh);
	}
#else
	if( m_writeBuffer)
	{
		if( m_writeBufferPos != 0)
		{
			m_mediaFile->Write( (void *)m_writeBuffer, m_writeBufferPos, NULL);
		}
		m_writeBufferPos = 0;
	}

#endif
}


/***************************************************************************/
// how big is the file?
/***************************************************************************/
int FileStream::Size(void)
{
#ifdef USESTDIO
	if (!fh)
		return(0);

	// Seek to the end of the file
	int currentPos = Tell();
	Seek(0, END);
	int size = Tell();
	Seek(currentPos, SET);
	return(size);
#else
	return(m_size);
#endif
}

/***************************************************************************/
// Seek to a position (returns absolute position)
/***************************************************************************/
bool FileStream::Seek(
int seekTo,
SEEKMODE mode)
{
#ifdef USESTDIO
	int ret;
	switch (mode)
	{
		case CUR:
			ret = fseek(fh, seekTo, SEEK_CUR);
			break;
		case END:
			ret = fseek(fh, seekTo, SEEK_END);
			break;
		default:
		case SET:
			ret = fseek(fh, seekTo, SEEK_SET);
			break;
	}

	return(ret == 0);
#else

	Flush();
	
	switch (mode)
	{
		case CUR:
			// Can we seek?
			if (m_pos + seekTo > m_size)
				return(false);

			// move the pos
			m_pos += seekTo;
			break;
		case END:
			// Can't seek past end
			if (m_size + seekTo > m_size || m_size + seekTo < 0)
				return(false);

			// move the pos
			m_pos = m_size + seekTo;
			break;
		default:
		case SET:
			if (seekTo < 0 || seekTo > (int) m_size)
				return(false);

			// move the pos
			m_pos = seekTo;
			break;
	}

	/* if writing, have to really seek-- may want to write over a previously-written section of file */

	if (m_data == NULL)
		m_mediaFile->Seek(m_pos);

	return(true);
#endif
}

/***************************************************************************/
// What position are we at in the file?
/***************************************************************************/
int FileStream::Tell(void)
{
#ifdef USESTDIO
	return(ftell(fh));
#else
	return(m_pos);
#endif
}

/***************************************************************************/
// Write to the file
/***************************************************************************/
bool FileStream::Write(
const void *buf,
int length)
{
#ifdef USESTDIO
	return(fwrite(buf, length, 1, fh) == 1);
#else
  	if (m_data)	// if m_data is non-NULL, file was opened for reading
  	{
  		ASSERT(false);
  		return false;
	}
	ASSERT(m_mediaFile != NULL);
	m_pos += length;
	if (m_pos > m_size)
		m_size = m_pos;

	char	*src = (char *)buf;

	while( length)
	{
		if( m_writeBufferPos == WRITE_BUFFER_SIZE)
		{
			if( !m_mediaFile->Write( (void *)m_writeBuffer, m_writeBufferPos, NULL))
			{
				return( false);
			}
			m_writeBufferPos = 0;
		}

		int	copySize = Math::Min( WRITE_BUFFER_SIZE - m_writeBufferPos, length);

		if( copySize > 0)
		{
			memcpy( m_writeBuffer + m_writeBufferPos, src, copySize);
			m_writeBufferPos += copySize;
			src += copySize;
			length -= copySize;
		}
	}

	return( true);
#endif
}

/***************************************************************************/
// Write to the file
/***************************************************************************/
bool FileStream::Print(
const char *format,
...)
{
	va_list args;
	va_start(args, format);
	char temp[1024];
	vsprintf(temp, format, args);
	va_end(args);
	return(Write(temp, strlen(temp)));
}

/***************************************************************************/
// read from the file (returns length read)
/***************************************************************************/
int FileStream::Read(
void *data,
int length)
{
#ifdef USESTDIO
	return(fread(data, 1, length, fh));
#else
	// Is the file open?
	if (data == NULL)
		return(0);

	if (m_data == NULL)
		return(0);

	// how much data is left?
	int dataLeft = m_size - m_pos;
	if (dataLeft == 0)
		return(0);

	// Are they asking for more than what is left?
	if (length > dataLeft)
		length = dataLeft;

	// Copy it
	unsigned char *buffptr = (unsigned char *) m_data;
	memcpy(data, &buffptr[m_pos], length);

	// advance the pointer
	m_pos += length;

	return(length);
#endif
}

/***************************************************************************/
// grab a line from a file
/***************************************************************************/
bool FileStream::GetLine(
char *data,
int maxLength)
{
#ifdef USESTDIO
	if (fgets(data, maxLength, fh) != NULL)
		return(true);
	else
	{
		if (feof(fh) || ferror(fh))
			return(false);
		else
			return(true);
	}
#else
	// Is the file open?
	if (data == NULL)
		return(false);

	if (m_data == NULL)
		return(false);

	// how much data is left?
	int dataLeft = m_size - m_pos;
	if (dataLeft == 0)
		return(false);

	// scan until the first \n.
	char* t = &((char*) m_data)[m_pos];
	char* s = t;
	int length = 0;
	while (length < maxLength - 1 && length < dataLeft )
	{
		data[length] = *s;

		s++;
		length++;

		if (data[length-1] == '\n')
			break;
	}

	// Copy it
	memcpy(data, t, length);
	data[length] = '\0';

	// advance the pointer
	m_pos += length;

	return(true);
#endif
}

#ifndef NOGAMELIB
/***************************************************************************/
// Read as a memory block. Call memFree to get rid of the block once you've called this
/*** static function ***/
/***************************************************************************/
void *FileStream::OpenReadClose(
const char *filename,
int *size /*= NULL*/,
bool *ownsData /*= NULL*/)
{
	/* if pre-loaded, just point to data and done */

	FileStream *pPreLoadedFileStream = NameLibrary::FindFileStream(filename);
	if (pPreLoadedFileStream)
	{
		if (size)
			*size = pPreLoadedFileStream->m_size;
		if (ownsData)
			*ownsData = false;
		return(pPreLoadedFileStream->m_data);
	}

	/* open and read file */

	static FileStream File;	// static, instead of new'ing each time. we rely on Open/Close to be clean
	File.Open(filename, READ, false);

	/* grab data to return. m_data is NULL if Open failed. must grab this before call to Close */

	void *data = File.m_data;

	/* did they want the size? m_size is 0 if Open failed. must grab this before call to Close */

	if (size)
		*size = File.m_size;
	if (ownsData)
		*ownsData = true;

	/* close it without free'ing buffer */

	File.m_bMemFree = false;	// user is responsible for data
	File.Close();

	/* Return the file data */

	return(data);
}
#endif

/***************************************************************************/
// pre-load a file in order to get it into uberfile log, and register it so
// that actual load doesn't trigger ASSERT in FileStream::Open. called from
// SimLoop::LoadLevelData, which is responsible for pre-loading all files
// (both .dbl and otherwise) that are necessary for a level. most files that
// a level needs are loaded as part of SimLoop::Initialize, but some are
// loaded during level gameplay. the intent of the ASSERT in FileStream::Open
// is to identify those files so they can be added to LoadLevelData
/*** static function ***/
/***************************************************************************/
FileStream *FileStream::PreLoad(char *filename, void *pData /*= NULL*/, int nSize /*= 0*/)
{
#ifdef USESTDIO
	/* under normal circumstances, calling FileStream::Open reads in the entire file. in USESTDIO, */
	/* we read the file in a bit at a time, so can't preload in that case */

	ASSERTS(false, "FileStream::PreLoad doesn't work in USESTDIO build");
	return(NULL);
#else //USESTDIO
	/* must call this during SimLoop::Initialize, typically from SimLoop::LoadLevelData */

	/* should be no harm if preload called twice on same file */

	FileStream *pPreLoadedFileStream = NameLibrary::FindFileStream(filename);
	if (pPreLoadedFileStream)
		return(pPreLoadedFileStream);

	/* load the file-- it may be code resident */

	FileStream *pFileStream = NULL;
	if (pData)
	{
		/* code resident */

		ASSERT(nSize != 0);	// must supply a size for code-resident files
		pFileStream = new FileStream;
		pFileStream->m_data = pData;
		pFileStream->m_size = nSize;
		pFileStream->m_bMemFree = false;	// filestream did not malloc this data
		pFileStream->m_pos = 0;
	}
	else
	{
		/* read in the file */

		pFileStream = NewAndOpen(filename, READ);
	}
	if (pFileStream)
	{
		/* record this file, by name, with the data manager. this is so FileStream::Open */
		/* can find pre-loaded files before trying to load from disk */

		NameLibrary::RegisterFileStream(pFileStream, filename);
		return(pFileStream);
	}
	else
	{
		ASSERTF(false && "all files requested for pre-load should be found",("%s", filename));	// all files requested for pre-load should be found
		return(NULL);
	}
#endif //USESTDIO
}

/* remove any pre-loaded file records from data manager */
/*** static function ***/

void FileStream::CleanupPreLoad(bool bCleanupPermanent /*= false*/)
{
	NameLibrary::ReleaseFileStreams( bCleanupPermanent );
}
