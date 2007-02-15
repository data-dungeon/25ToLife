#include "pch.h"
#include <Game/Database/DBFile.h>
#include <EngineHelper/FileStream.h>
#include "dibble.h"
#include "texture.h"

extern bool TextureMgr__ProcessTextureSet( t_Handle Handle, void *pChunkDataVoid, ts_DBChunkHdr *pChunkHeader, void **ppRetData, u32 *pu32DataSize);
extern bool TextureMgr__ProcessBitmap( t_Handle Handle, HBITMAP hbm );
extern ts_TextureSet * TextureMgr__GetTextureSet( t_Handle Handle );

class DibbleManager
{
	public:
		DibbleManager( void );
		~DibbleManager( void );

	public:
		DibbleFile * NewDibbleFile( void );
		DibbleFile * FindDibble( t_Handle FileHandle );
		DibbleFile * FindDibble( char *pFilePath );
		DibbleFile * LoadDibble( char *pFilePath );

	public:
		DibbleFile m_DibbleFiles;
};

DibbleManager dm;

DibbleManager::DibbleManager( void )
{
}

DibbleManager::~DibbleManager( void )
{
	while( m_DibbleFiles.Child() )
		delete m_DibbleFiles.Child();

	return;
}

DibbleFile * DibbleManager::NewDibbleFile( void )
{
	DibbleFile *pDibble = new DibbleFile;

	if( pDibble )
		m_DibbleFiles.SetChild( pDibble );

	return pDibble;
}

DibbleFile * DibbleManager::FindDibble( t_Handle FileHandle )
{
	DibbleFile* pDibble = (DibbleFile*) FileHandle;

	if( pDibble )
	{
		DibbleFile* pNextDibble = (DibbleFile*) m_DibbleFiles.Child();

		while( pNextDibble )
		{
			if( pNextDibble == pDibble )
				return pDibble;

			pNextDibble = (DibbleFile*) m_DibbleFiles.Next();
		}
	}

	return 0;
}

DibbleFile * DibbleManager::FindDibble( char *pFilePath )
{
	DibbleFile *pDibble = (DibbleFile *) m_DibbleFiles.Child();

	while( pDibble )
	{
		if( 0 == pDibble->m_FilePath.stricmp( pFilePath ) )
			return pDibble;

		pDibble = (DibbleFile *) pDibble->Next();
	}

	return pDibble;
}

DibbleFile * DibbleManager::LoadDibble( char *pFilePath )
{
	/*
	char szPath[1024];
	strcpy( szPath, pFilePath );

	int len = strlen(szPath);
	int i, c;
	bool begin = false;
	bool end = false;

	for( i = c = 0; i <= len; i++ )
	{
		if( !strnicmp( &szPath[i], "mapsPS2", 7 ) )
		{
			begin = true;
		}
		else
		if( !strnicmp( &szPath[i], "PS2", 3 ) )
		{
			begin = false;
			i+= 3;
		}

		szPath[c++] = szPath[i];
	}

	pFilePath = szPath;
	*/

	DibbleFile *pDibble = FindDibble( pFilePath );

	if( !pDibble )
	{
		//=============================================================================
		// Open the file.
		//=============================================================================

		FileStream Stream;
		int FileSize;
		u8 *pFileData;

		if( !Stream.Open( (char *) pFilePath, FileStream::READ ) )
		{
			char szRealPath[_MAX_PATH];

			sprintf( szRealPath, "data\\%s", pFilePath );

			if( !Stream.Open( szRealPath, FileStream::READ ) )
				return INVALID_HANDLE;
		}

		FileSize = Stream.Size();

		if( FileSize > 0 )
		{
			//=============================================================================
			// Allocate some memory.
			//=============================================================================

			pFileData = new u8[FileSize];

			if( pFileData )
			{
				//=============================================================================
				// Read the file and stuff it into a temporary buffer.
				//=============================================================================

				Stream.Read( pFileData, FileSize );
				Stream.Close();

				//=============================================================================
				// Process the data.
				//=============================================================================

				pDibble = NewDibbleFile();

				if( pDibble )
					pDibble->ProcessFile( (char *) pFilePath, pFileData, FileSize );

				//=============================================================================
				// Delete the buffer.
				//=============================================================================

				delete [] pFileData;
			}
		}

		//=============================================================================
		// Close the stream.
		//=============================================================================

		Stream.Close();
	}

	return pDibble;
}

//=============================================================================
// DibbleFile::DibbleFile
//=============================================================================

DibbleFile::DibbleFile( void )
{
	m_pTrueData = 0;
	m_pData = 0;
	m_DataSize = 0;
}

//=============================================================================
// DibbleFile::~DibbleFile
//=============================================================================

DibbleFile::~DibbleFile()
{
	Destroy();
}

//=============================================================================
// DibbleFile::ProcessFile
//=============================================================================

bool DibbleFile::ProcessFile( char *pFilePath, void *pData, int DataSize )
{
	//=============================================================================
	// Set the file path and size. ////////////////////////////////////////////////
	//=============================================================================

	m_FilePath.set( pFilePath );
	m_DataSize = DataSize;

	//=============================================================================
	// Check the file extension. //////////////////////////////////////////////////
	//=============================================================================

	TCHAR szType[16];
	strcpy( szType, &pFilePath[strlen(pFilePath)-4] );

	if(tlower(szType[0]) == '.' &&
		tlower(szType[1]) == 'b' &&
		tlower(szType[2]) == 'm' &&
		tlower(szType[3]) == 'p' )
	{
		HBITMAP hbm = (HBITMAP) LoadImage( NULL, pFilePath, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION|LR_LOADFROMFILE );

		if( !hbm )
			hbm = (HBITMAP) LoadImage( NULL, Printf( "data\\%s", pFilePath ), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION|LR_LOADFROMFILE );

		if( hbm )
			TextureMgr__ProcessBitmap( (t_Handle) this, hbm );

		ReleaseDIB( hbm );
	}
	else
	if(tlower(szType[0]) == '.' &&
		tlower(szType[1]) == 't' &&
		tlower(szType[2]) == 'g' &&
		tlower(szType[3]) == 'a' )
	{
		HBITMAP hbm = LoadTGA( pFilePath );

		if( !hbm )
			hbm = (HBITMAP) LoadTGA( Printf( "data\\%s", pFilePath ) );

		if( hbm )
			TextureMgr__ProcessBitmap( (t_Handle) this, hbm );

		ReleaseDIB( hbm );
	}
	else
	{
		//=============================================================================
		// Allocate some memory. //////////////////////////////////////////////////////
		//=============================================================================

		m_pTrueData = new u8[DataSize];

		if( !m_pTrueData )
			return false;

		m_pData = m_pTrueData;

		//=============================================================================
		// Copy the file into it. /////////////////////////////////////////////////////
		//=============================================================================

		memcpy( m_pData, pData, DataSize );

		//=============================================================================
		// Copy in the header, too. ///////////////////////////////////////////////////
		//=============================================================================

		memcpy( &m_Header, pData, sizeof(m_Header) );

		//=============================================================================
		// A dibble may have a skip value coded into the first bytes as a string. /////
		//=============================================================================

		int cskip = atoi( m_Header.cSkipCount);

		//=============================================================================
		// If there is a skip value, copy the header from the appropriate location. ///
		//=============================================================================

		if( cskip != 0 )
			memcpy( &m_Header, (void *)( (u32)pData + cskip), sizeof(m_Header) );

		//=============================================================================
		// Collapse the data. /////////////////////////////////////////////////////////
		//=============================================================================

		Collapse( sizeof(m_Header) + cskip );

		//=============================================================================
		// Parse the data. ////////////////////////////////////////////////////////////
		//=============================================================================

		return Parse();
	}

	return false;
}

//=============================================================================
// DibbleFile::Collapse
//=============================================================================

void DibbleFile::Collapse( u32 Bytes )
{
	m_pData = &m_pData[Bytes];
}

//=============================================================================
// DibbleFile::Parse
//=============================================================================

bool DibbleFile::Parse( void )
{
	//=============================================================================
	// Make sure we can handle this type of data
	//=============================================================================

	if((strcmp(m_Header.cID, "DB"   ) != 0) &&		// generic data ok
		(strcmp(m_Header.cID, "WIN32") != 0) &&	   // win32 data
		(strcmp(m_Header.cID, "PS2"  ) != 0))			// win32 can also handle ps2 data
	{
		return false;
	}

	//=============================================================================
	// Check for DB_VERSION
	//=============================================================================

	if( m_Header.u32Version != DB_VERSION )
		return false;

	int nChunks = m_Header.u32Chunks;

	ts_DBBaseChunkHdr *pChunkHeader;
	u8 *pChunkData;
	u32 HeaderSize;

	for( int i = 0; i < nChunks; i++ )
	{
		pChunkHeader = (ts_DBBaseChunkHdr *) m_pData;

		if( pChunkHeader->u16Flags & DBL_HEADER_32BYTE )
		{
			if( pChunkHeader->u16Flags & DBL_NAMED_CHUNK )
				HeaderSize = sizeof(ts_DBChunkHdr);
			else
				HeaderSize = sizeof(ts_DBBaseChunkHdr);
		}
		else
		{
			if( pChunkHeader->u16Flags & DBL_NAMED_CHUNK)
				HeaderSize = sizeof(ts_DBChunkHdrNonAlign);
			else
				HeaderSize = sizeof(ts_DBBaseChunkHdrNonAlign);
		}

		pChunkData = &m_pData[HeaderSize];

		switch( pChunkHeader->u16Type )
		{
		case DB_TEXTURE_SET:
		case DB_MERGED_TEXTURE_SET:
			TextureMgr__ProcessTextureSet( (t_Handle) this, pChunkData, (ts_DBChunkHdr *) pChunkHeader, 0, 0 );
			break;
		}

		Collapse( HeaderSize + pChunkHeader->u32Size );

	}

	return true;
}

//=============================================================================
// DibbleFile::Destroy
//=============================================================================

void DibbleFile::Destroy( void )
{
	delete [] m_pTrueData;

	m_FilePath.set("");
	m_pTrueData = 0;
	m_pData = 0;
	m_DataSize = 0;
}

//=============================================================================
// dblGetChunkData
//=============================================================================

void * dblGetChunkData( u16 u16Type, const char *pChunkName, t_Handle Handle, int *pnSize )
{
	if( Handle )
	{
		switch( u16Type )
		{
		case DB_TEXTURE_SET:
		case DB_MERGED_TEXTURE_SET:
			return TextureMgr__GetTextureSet( Handle );
		}
	}

	return 0;
}

//=============================================================================
// dblLoad
//=============================================================================

u32 dblLoad( const char *pFilePath )
{
	FILENAME tmp;
	FILENAME FileNameTGA;
	FILENAME FileNameBMP;

	//////////////////////////////////////////////////////////////////////////
	// first attempt to load the .dbl from a tga file

	tmp.delext( pFilePath );
	FileNameTGA.set( Printf( "%s.tga", tmp.get() ) );

	//////////////////////////////////////////////////////////////////////////
	// then a .bmp file

	tmp.delext( pFilePath );
	FileNameBMP.set( Printf( "%s.bmp", tmp.get() ) );

	//////////////////////////////////////////////////////////////////////////
	// then try to load the real thing

	DibbleFile *pDibble = dm.LoadDibble( (char *) FileNameTGA.get() );

	if( !pDibble )
	{
		pDibble = dm.LoadDibble( (char *) FileNameBMP.get() );

		if( !pDibble )
		{
			pDibble = dm.LoadDibble( (char *) pFilePath );

			if( !pDibble )
			{
				OutputDebugString( Printf( "*** File %s\n", pFilePath ) );
				return (u32) INVALID_HANDLE;
			}
		}
	}

	return (u32) pDibble;
}

//=============================================================================
// dblRemove
//=============================================================================

bool dblRemove( t_Handle Handle, bool bRemovePermanent )
{
	DibbleFile *pDibble = dm.FindDibble( Handle );

	if( !pDibble )
		return false;

	delete pDibble;

	return true;
}

//=============================================================================
// dblGetFileName
//=============================================================================

char * dblGetFileName( t_Handle Handle )
{
	DibbleFile *pDibble = (DibbleFile *) Handle;

	if( !pDibble )
		return 0;

	return pDibble->m_FilePath.get();
}

//=============================================================================
// dblSetPermanent
//=============================================================================

void dblSetPermanent(t_Handle Handle, bool bPermanent)
{
	// do nothing
}