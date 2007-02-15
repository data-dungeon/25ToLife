#if 0
#ifndef _FORMATTEDFILE_H_
#define _FORMATTEDFILE_H_

#include <afxwin.h>

typedef struct
{
	DWORD PreMarker;
	DWORD Marker;
	DWORD Version;
	DWORD Length;
}
FILEMARKER;

class FileMarker : public CTreeNode
{
	public:
		FILEMARKER m_FileMarker;
		int m_FilePosition;
};

#define FILEMARKERCODE (0xcdfe3c5b)

class FormattedFile
{
	public:
		FormattedFile( void );
		virtual ~FormattedFile();

		BOOL OpenFile( const TCHAR *pFileName, DWORD FileAccess, DWORD ShareMode, DWORD OpenMode );
		void CloseFile( void );
		BOOL Read( void *pBuffer, int BufferSize );
		BOOL Read( MemoryBank &m );
		BOOL Write( void *pBuffer, int BufferSize );
		BOOL Write( MemoryBank &m );
		int FileSize( void );
		int GetFilePosition( void );
		BOOL SetFilePosition( int Position );
		FILEMARKER GetFileMarker( void );
		BOOL EndOfFile( void );

		BOOL Branch( DWORD Marker, DWORD Version );
		BOOL Return( void );

	public:
		HANDLE m_FileHandle;
		tstring m_FileName;
		DWORD m_FileAccess;
		DWORD m_ShareMode;
		DWORD m_OpenMode;

	protected:
		FileMarker m_FileMarkers;
};

#endif // _FORMATTEDFILE_H_
#endif // 0