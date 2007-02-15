#ifndef _FILESTREAM_H_
#define _FILESTREAM_H_

#include <EngineHelper/FileStream.h>

//=============================================================================
// FILESTREAMHANDLE
//=============================================================================

typedef struct
{
	HANDLE Handle;
	FileStream *pStream;
}
FILESTREAMHANDLE;

#define MAX_FILE_HANDLES	(32)

//=============================================================================
// FileManager
//=============================================================================

class FileManager
{
	public:
		FileManager( void );
		~FileManager( void );

	public:
		void CloseAllFiles( void );

		FILESTREAMHANDLE * RegisterFile( HANDLE FileHandle, FileStream *pStream );
		FILESTREAMHANDLE * FileStreamHandle( HANDLE FileHandle );
		FILESTREAMHANDLE * FileStreamHandle( FileStream *pStream );
		HANDLE AsFileHandle( FileStream *pStream );
		FileStream * AsFileStream( HANDLE FileHandle );

		void CloseFile( FILESTREAMHANDLE *pStreamHandle );
		void CloseFile( FileStream *pStream );
		void CloseFile( HANDLE FileHandle );

	public:
		FILESTREAMHANDLE m_FileHandles[MAX_FILE_HANDLES];
};

#endif // _FILESTREAM_H_