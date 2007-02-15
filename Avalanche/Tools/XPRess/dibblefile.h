#ifndef _DIBBLEFILE_H_
#define _DIBBLEFILE_H_

#include <libwin32.h>
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

		int DibbleFile::GetMaxMips(int width, int height);

	protected:
		BOOL ProcessTextureSet( void *pChunkDataVoid, ts_DBChunkHdr *pChunkHeader, int HeaderSize, int* Mips );
		BOOL ProcessTexture( unsigned int n, struct ts_TextureChunkRecord* pTextureRecord );
		BITMAP2* CreateTextureFromBitmap( BITMAP2 *pBitmap, int Width, int Height, PIXELFORMAT Format );

		void DibbleFile::RGBtoHSV( float r, float g, float b, float *h, float *s, float *v );
		void DibbleFile::HSVtoRGB( float *r, float *g, float *b, float h, float s, float v );

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


//extern	int	MipLevels[4096];		// store off mip counts
//extern	int	CompressionTypes[4096];		// store off mip counts


#endif // _DIBBLEFILE_H_