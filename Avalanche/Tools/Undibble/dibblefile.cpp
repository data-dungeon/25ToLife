#include "pch.h"
#include <Game/Database/DBFile.h>
#include <Game/Managers/TextureMgr/TextureMgr.h>
#include "dibblefile.h"

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

DibbleFile* DibbleManager::NewDibbleFile( void )
{
	DibbleFile *pDibble = new DibbleFile;

	if( pDibble )
		m_DibbleFiles.SetChild( pDibble );

	return pDibble;
}

DibbleFile* DibbleManager::FindDibble( const char *pFilePath )
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

DibbleFile* DibbleManager::LoadDibble( const char *pFilePath )
{
	//=============================================================================
	// See if the dbl has already been loaded.
	//=============================================================================

	DibbleFile *pDibble = FindDibble( pFilePath );

	if( !pDibble )
	{
		//=============================================================================
		// The dbl wasn't found in memory, so it needs to be loaded from disk.
		//=============================================================================

		HANDLE hFile;
		int FileSize;
		u8 *pFileData;

		hFile = FileOpen( pFilePath );

		if( hFile == INVALID_HANDLE_VALUE )
			return 0;

		FileSize = GetFileSize( hFile, 0 );

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

				FileRead( hFile, pFileData, FileSize );

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

		FileClose( hFile );
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
	m_ppTextures = 0;
	m_TextureCount = 0;
	m_pChunkData = 0;
	m_ChunkSize = 0;
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
	// Allocate some memory. //////////////////////////////////////////////////////
	//=============================================================================

	m_pTrueData = new u8[DataSize];

	if( !m_pTrueData )
		return false;

	m_pData = m_pTrueData;

	//=============================================================================
	// Copy the file into memory. /////////////////////////////////////////////////
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
	// Collapse the header ////////////////////////////////////////////////////////
	//=============================================================================

	Collapse( sizeof(m_Header) + cskip );

	//=============================================================================
	// Parse the data. ////////////////////////////////////////////////////////////
	//=============================================================================

	return Parse();
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
		(strcmp(m_Header.cID, "WIN32") != 0) &&	   // Win32
		(strcmp(m_Header.cID, "PS2"  ) != 0) &&		// Playstation2
		(strcmp(m_Header.cID, "DX"   ) != 0) &&		// DirectX
		(strcmp(m_Header.cID, "XBOX" ) != 0))			// Xbox
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
			ProcessTextureSet( pChunkData, (ts_DBChunkHdr*) pChunkHeader, HeaderSize );
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
// DibbleFile::ProcessTextureSet
//=============================================================================

BOOL DibbleFile::ProcessTextureSet( void *pChunkDataVoid, ts_DBChunkHdr *pChunkHeader, int HeaderSize )
{
	ts_TextureChunkHeader *pTextureHeader;
	ts_TextureChunkRecord *pTextureRecord;

	u8 *pChunkData = (u8 *) pChunkDataVoid;

	pTextureHeader = (ts_TextureChunkHeader *) pChunkData;

	u32 TextureCount = pTextureHeader->u32TextureCount;

	m_ppTextures = new BITMAP2*[TextureCount];

	if( !m_ppTextures )
		return false;

	m_ChunkSize = HeaderSize+pChunkHeader->u32Size;
	m_pChunkData = new u8[m_ChunkSize];

	if( !m_pChunkData )
		return false;

	memcpy( m_pChunkData, pChunkHeader, HeaderSize );
	memcpy( &m_pChunkData[HeaderSize], pChunkDataVoid, pChunkHeader->u32Size );

	m_TextureCount = TextureCount;

	for( unsigned int i = 0; i < TextureCount; i++ )
	{
		pTextureRecord = &pTextureHeader->TextureChunkRecord[i];

		while( pTextureRecord )
		{
			if( pTextureRecord->u32TextureOffset != 0 )
				pTextureRecord->pTexture = (void *) &pChunkData[pTextureRecord->u32TextureOffset];

			if( pTextureRecord->u32PaletteOffset != 0 )
				pTextureRecord->pPalette = (ts_bRGBA *) &pChunkData[pTextureRecord->u32PaletteOffset];

			if( pTextureRecord->u32NextOffset != 0 )
				pTextureRecord->pNext = (ts_TextureChunkRecord *) &pChunkData[pTextureRecord->u32NextOffset];

			ProcessTexture( i, pTextureRecord );

			pTextureRecord = pTextureRecord->pNext;
		}
	}

	return true;
}

//=============================================================================
// DibbleFile::ProcessTexture
//=============================================================================

u8 PaletteSwizzle[256] = {

	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,

	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
	0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
	0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
	0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
	0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,

	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
	0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
	0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
	0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
	0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
	0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,

	0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
	0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
	0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
	0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
	0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
	0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
	0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
	0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,

};

BOOL DibbleFile::ProcessTexture( unsigned int n, ts_TextureChunkRecord *pTextureRecord )
{
	int Bind = 0;

	if( pTextureRecord )
	{
		int BPP = 0;

		switch( pTextureRecord->u32Flags & TEX_FLAG_SIZE_MASK )
		{
		case TEX_FLAG_SIZE_4_BIT:
			BPP = 4;
			break;
		case TEX_FLAG_SIZE_8_BIT:
			BPP = 8;
			break;
		case TEX_FLAG_SIZE_16_BIT:
			BPP = 16;
			break;
		case TEX_FLAG_SIZE_24_BIT:
			BPP = 24;
			break;
		case TEX_FLAG_SIZE_32_BIT:
			BPP = 32;
			break;
		case TEX_FLAG_SIZE_S3TC:
			break;
		}

		BITMAP2 Bitmap;

		Bitmap.Width = (int) pTextureRecord->s16TextureWidth;
		Bitmap.Height = (int) pTextureRecord->s16TextureHeight;
		Bitmap.Format = PixelFormatFromBPP( BPP );
		Bitmap.Stride = GetImageStride( Bitmap.Width, BPP );
		Bitmap.pBits = (BYTE *) pTextureRecord->pTexture;

		switch( Bitmap.Format )
		{
		case PIXELFMT_A8R8G8B8:
			Bitmap.Format = PIXELFMT_A8B8G8R8;
			break;
		case PIXELFMT_X8R8G8B8:
			Bitmap.Format = PIXELFMT_X8B8G8R8;
			break;
		case PIXELFMT_A4R4G4B4:
	//		Bitmap.Format = PIXELFMT_A4B4G4R4;
			break;
		case PIXELFMT_X4R4G4B4:
	//		Bitmap.Format = PIXELFMT_X4B4G4R4;
			break;
		case PIXELFMT_A1R5G5B5:
	//		Bitmap.Format = PIXELFMT_A1B5G5R5;
			break;
		case PIXELFMT_X1R5G5B5:
	//		Bitmap.Format = PIXELFMT_X1B5G5R5;
			break;
		case PIXELFMT_R5G6B5:
	//		Bitmap.Format = PIXELFMT_B5G6R5;
			break;
		}

		u32 Format = pTextureRecord->u32Flags & TEX_FLAG_FMT_MASK;

		BOOL Paletted = FALSE;

		if( Format == TEX_FLAG_FMT_CI || Format == TEX_FLAG_FMT_CI_IA )
			Paletted = TRUE;

		if( Format == TEX_FLAG_FMT_RGB )
		{
			if( Bitmap.Format == PIXELFMT_A8R8G8B8 )
				Bitmap.Format = PIXELFMT_X8R8G8B8;

			if( Bitmap.Format == PIXELFMT_A4R4G4B4 )
				Bitmap.Format = PIXELFMT_X4R4G4B4;

			if( Bitmap.Format == PIXELFMT_A1R5G5B5 )
				Bitmap.Format = PIXELFMT_X1R5G5B5;
		}
		else if( Format == TEX_FLAG_FMT_RGBA )
		{
			if( Bitmap.Format == PIXELFMT_X8R8G8B8 )
				Bitmap.Format = PIXELFMT_A8R8G8B8;

			if( Bitmap.Format == PIXELFMT_X4R4G4B4 )
				Bitmap.Format = PIXELFMT_A4R4G4B4;

			if( Bitmap.Format == PIXELFMT_X1R5G5B5 )
				Bitmap.Format = PIXELFMT_A1R5G5B5;
		}
		else if( Paletted )
		{
			int PaletteSize =
				pTextureRecord->s16PaletteWidth *
				pTextureRecord->s16PaletteHeight;

			int A, R, G, B;
			int i, n;

		//	if( Bitmap.Format == PIXELFMT_PALETTE8 )
		//		Bitmap.Format = PIXELFMT_INDEX8;

		//	if( Bitmap.Format == PIXELFMT_PALETTE4 )
		//		Bitmap.Format = PIXELFMT_INDEX4;

			for( i = 0; i < PaletteSize; i++ )
			{
				if( BPP == 8 )
					n = (int) PaletteSwizzle[i];
				else
					n = i;

				A = (int) pTextureRecord->pPalette[n].A << 24;
				R = (int) pTextureRecord->pPalette[n].R << 16;
				G = (int) pTextureRecord->pPalette[n].G << 8;
				B = (int) pTextureRecord->pPalette[n].B << 0;

				Bitmap.Palette[i] = (COLOR32) (A|R|G|B);
			}
		}
		else
		{
		}

		if( m_ppTextures )
			m_ppTextures[n] = CreateTextureFromBitmap( &Bitmap, Bitmap.Width, Bitmap.Height, PIXELFMT_A8R8G8B8 );
	}

	return Bind;
}

BITMAP2* DibbleFile::CreateTextureFromBitmap( BITMAP2 *pBitmap, int Width, int Height, PIXELFORMAT Format )
{
	if( !pBitmap )
		return 0;

	COLOR32 c;
	int w = pBitmap->Width;
	int h = pBitmap->Height;
	int x, y;
	int A;

	BITMAP2 *newBitmap = CreateBitmap2( w, h, Format, pBitmap->Palette, 256 );

	if( newBitmap )
	{
		SetPalette( 0, newBitmap->Palette, 256 );
		SetPalette( 1, pBitmap->Palette, 256 );

		BitmapToBitmap( newBitmap, 0, 0, w, h, pBitmap, 0, 0, w, h );

		BeginBitmap( 0, newBitmap );

		for( y = 0; y < h; y++ )
		{
			for( x = 0; x < w; x++ )
			{
				c = GetPixelU( 0, x, y );

				A = (c>>24) & 0xff;

				if( A > 0 )
					A = (A * 2 - 1) << 24;

				SetPixelU( 0, x, y, (c&0xffffff)|A );
			}
		}

		FinishBitmap(0);
	}

	return newBitmap;
}