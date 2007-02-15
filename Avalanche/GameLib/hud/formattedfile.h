#ifndef _FORMATTEDFILE_H_
#define _FORMATTEDFILE_H_

#include "EngineHelper/filestream.h"
#include "hud/memorybank.h"
#include "hud/stringex.h"
#include "hud/treenode.h"

typedef struct
{
	unsigned int PreMarker;
	unsigned int Marker;
	unsigned int Version;
	unsigned int Length;
}
FILEMARKER;

class FileMarker : public TreeNode
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

		bool OpenFile( const char *pFileName, FileStream::OPENMODE Mode );
		void CloseFile( void );
		bool Read( void *pBuffer, int BufferSize );
		bool Read( MemoryBank &m );
		bool Write( void *pBuffer, int BufferSize );
		bool Write( MemoryBank &m );
		int FileSize( void );
		int GetFilePosition( void );
		bool SetFilePosition( int Position );
		FILEMARKER GetFileMarker( void );
		bool EndOfFile( void );

		bool Branch( unsigned int Marker, unsigned int Version );
		bool Return( void );

	public:
		FileStream m_FileStream;
		xstring m_FileName;
		FileStream::OPENMODE m_OpenMode;

	protected:
		FileMarker m_FileMarkers;
};

#endif // _FORMATTEDFILE_H_