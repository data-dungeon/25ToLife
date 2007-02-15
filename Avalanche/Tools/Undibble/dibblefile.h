#ifndef _DIBBLEFILE_H_
#define _DIBBLEFILE_H_

#include <Game/Database/DBFile.h>

//=============================================================================
// DibbleFile
//=============================================================================

class DibbleFile : public CTreeNode
{
	public:
		DibbleFile( void );
		virtual ~DibbleFile();

		bool ProcessFile( char *pFilePath, void *pData, int DataSize );
		void Collapse( u32 size );
		bool Parse( void );
		void Destroy( void );

	protected:
		BOOL ProcessTextureSet( void *pChunkDataVoid, ts_DBChunkHdr *pChunkHeader, int HeaderSize );
		BOOL ProcessTexture( unsigned int n, struct ts_TextureChunkRecord* pTextureRecord );
		BITMAP2* CreateTextureFromBitmap( BITMAP2 *pBitmap, int Width, int Height, PIXELFORMAT Format );

	public:
		ts_DBFileHdr m_Header;
		tstring m_FilePath;
		int m_DataSize;
		u8 *m_pTrueData;
		u8 *m_pData;

		BITMAP2** m_ppTextures;
		int m_TextureCount;
		u8* m_pChunkData;
		int m_ChunkSize;
};

class DibbleManager
{
	public:
		DibbleManager( void );
		~DibbleManager( void );

	public:
		DibbleFile* NewDibbleFile( void );
		DibbleFile* FindDibble( const char *pFilePath );
		DibbleFile* LoadDibble( const char *pFilePath );

	public:
		DibbleFile m_DibbleFiles;
};

extern DibbleManager dm;

#endif // _DIBBLEFILE_H_