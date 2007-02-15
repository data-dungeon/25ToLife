/***************************************************************************/
// File i/o wrapper that sits on top of the media stuff
/***************************************************************************/
#ifndef FILESTREAM_H
#define FILESTREAM_H
#include "platform/BaseType.h"

#ifdef WIN32
// Don't do it!
//#define USESTDIO
#endif

#ifdef USESTDIO
#include <stdio.h>
#else
#include "Layers/Media.h"
#include <stdio.h>//so that vsprintf is defined
#endif

// This stuff exists to "stub-out" the file i/o
class FileStream
{
public:
	// These do nothing, there is no exception handling
	FileStream(void);
	~FileStream(void);

	// What are are open modes
	typedef enum
	{
		READ,
		WRITE
	} OPENMODE;

	// static utilties
	static bool Exists(char *filename);

	// new/delete the filestream object
	static FileStream *NewAndOpen(const char *filename, OPENMODE mode, bool text = false);

	// Open the file
	bool Open(const char *filename, OPENMODE mode, bool text = false);

	// Close the file
	void Close(void);

	// how big is the file?
	int Size(void);

	// Seek to a position
	typedef enum
	{
		CUR,	// Current position of file pointer
		END,	// End of file
		SET	// Beginning of file
	} SEEKMODE;
	bool Seek(int seekTo, SEEKMODE mode = SET);

	// What position are we at in the file?
	int Tell(void);

	// Write to the file
	bool Write(const void *buf, int length);
	bool Print(const char *format, ...);
	void Flush(void);

	// read from the file (returns length read)
 	int Read(void *data, int length);

   // get a line from the file (acts like fgets).
   // returns false if an error occurred, true otherwise.
	bool GetLine(char *data, int maxLength);

	// permanent status
	void SetPermanent(const bool bPermanent)	{ m_bPermanent = bPermanent; }
	bool IsPermanent() const						{ return(m_bPermanent); }

	// pre-load a file, for getting file into uberfile log-- called from SimLoop::LoadLevelData
	static FileStream *PreLoad(char *filename, void *pData = NULL, int nSize = 0);
	static void CleanupPreLoad(bool bCleanupPermanent = false);

#ifndef NOGAMELIB
	// Read as a memory block.  Call memFree to get rid of the block
	// once you've called this
	static void *OpenReadClose(const char *filename, int* size = NULL, bool *ownsData = NULL);
#endif

private:
#ifdef USESTDIO
	FILE 			*fh;
#else
	// use the media library
	void							*m_data;			// data in the file
	bool							m_bMemFree;		// filestream should memFree if it memAlloc'd, unless told not to
	bool							m_bPermanent;	// permanent data, loaded at game initialization, should not be free'd until game end
	int							m_size;			// What is the size of the data?
	int							m_pos;			// What is the file ptr
	MediaFile					*m_mediaFile;	// Open file for writes
	char							*m_writeBuffer;
	int							m_writeBufferPos;
#endif
};

#endif
