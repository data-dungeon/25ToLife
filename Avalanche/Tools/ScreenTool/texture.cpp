#include "pch.h"
#include "texture.h"

TextureSetRecord::TextureSetRecord( void )
{
	m_Handle = INVALID_HANDLE;

	int MaxBind = sizeof(m_BindList)/sizeof(m_BindList[0]);
	
	for( int i = 0; i < MaxBind; i++ )
		m_BindList[i] = 0;

	return;
}

//=============================================================================
// TextureManager::TextureManager
//=============================================================================

TextureManager::TextureManager( void )
{
	for( int i = 0; i < MAX_TEXTURE_BINDS; i++ )
	{
		m_TextureBinds[i].pTexture = 0;
		m_TextureBinds[i].Width = 0;
		m_TextureBinds[i].Height = 0;
		m_TextureBinds[i].Format = D3DFMT_UNKNOWN;
	}

	return;
}

//=============================================================================
// TextureManager::~TextureManager
//=============================================================================

TextureManager::~TextureManager( void )
{
	DestroyAllTextures();
}

//=============================================================================
// TextureManager::RegisterTexture
//=============================================================================

int TextureManager::RegisterTexture( LPDIRECT3DTEXTURE9 pTexture )
{
	int Bind = TextureBind( pTexture );

	if( Bind == 0 )
	{
		Bind = TextureBind(0);

		if( Bind != 0 )
		{
			D3DSURFACE_DESC SurfaceDesc;
			HRESULT hresult = D3DRESULT( pTexture->GetLevelDesc( 0, &SurfaceDesc ) );

			if( !FAILED(hresult) )
			{
				m_TextureBinds[Bind].pTexture = pTexture;
				m_TextureBinds[Bind].Width = SurfaceDesc.Width;
				m_TextureBinds[Bind].Height = SurfaceDesc.Height;
				m_TextureBinds[Bind].Format = SurfaceDesc.Format;
			}
		}
	}

	return Bind;
}

//=============================================================================
// TextureManager::DestroyTexture
//=============================================================================

void TextureManager::DestroyTexture( int Bind )
{
	if( Bind > 0 )
	{
		if( m_TextureBinds[Bind].pTexture )
			m_TextureBinds[Bind].pTexture->Release();

		m_TextureBinds[Bind].pTexture = 0;
		m_TextureBinds[Bind].Width = 0;
		m_TextureBinds[Bind].Height = 0;
		m_TextureBinds[Bind].Format = D3DFMT_UNKNOWN;
	}

	return;
}

//=============================================================================
// TextureManager::DestroyAllTextures
//=============================================================================

void TextureManager::DestroyAllTextures( void )
{
	for( int i = 0; i < MAX_TEXTURE_BINDS; i++ )
		DestroyTexture(i);

	while( m_TextureSetRecords.Child() )
		delete m_TextureSetRecords.Child();

	return;
}

//=============================================================================
// TextureManager::GetBestTextureSize
//=============================================================================

int TextureManager::GetBestTextureSize( int size )
{
	int i;

	int minsize = size;
	int maxsize = size*2-1;

	for( i = 0; minsize > 1; i++ )
		minsize >>= 1;

	minsize = 1<<i;

	for( i = 0; maxsize > 1; i++ )
		maxsize >>= 1;

	maxsize = 1<<i;

	if( minsize > 512 ) minsize = 512;
	if( maxsize > 512 ) maxsize = 512;

	if( maxsize - size < size - minsize )
		size = maxsize;
	else
		size = minsize;

	return size;
}
//=============================================================================
// TextureManager::NewTextureSetRecord
//=============================================================================

TextureSetRecord * TextureManager::NewTextureSetRecord( void )
{
	TextureSetRecord *pTextureSetRecord = new TextureSetRecord;

	if( pTextureSetRecord )
		m_TextureSetRecords.SetChild( pTextureSetRecord );

	return pTextureSetRecord;
}

//=============================================================================
// TextureManager::GetTextureSetRecord
//=============================================================================

TextureSetRecord * TextureManager::GetTextureSetRecord( ts_TextureSet *pTextureSet )
{
	TextureSetRecord *pTextureSetRecord = (TextureSetRecord *) m_TextureSetRecords.Child();

	while( pTextureSetRecord )
	{
		if( &pTextureSetRecord->m_TextureSet == pTextureSet )
		{
			m_TextureSetRecords.SetChild( pTextureSetRecord );

			return pTextureSetRecord;
		}
	}

	return 0;
}

//=============================================================================
// TextureManager::GetTextureSetRecord
//=============================================================================

TextureSetRecord * TextureManager::GetTextureSetRecord( t_Handle Handle )
{
	TextureSetRecord *pTextureSetRecord = (TextureSetRecord *) m_TextureSetRecords.Child();

	while( pTextureSetRecord )
	{
		if( pTextureSetRecord->m_Handle == Handle )
		{
			m_TextureSetRecords.SetChild( pTextureSetRecord );

			return pTextureSetRecord;
		}

		pTextureSetRecord = (TextureSetRecord *) pTextureSetRecord->Next();
	}

	return 0;
}

//=============================================================================
// TextureManager::TextureBind
//=============================================================================

int TextureManager::TextureBind( LPDIRECT3DTEXTURE9 pTexture )
{
	for( int i = 1; i < MAX_TEXTURE_BINDS;	i++ )
	{
		if( m_TextureBinds[i].pTexture == pTexture )
			return i;
	}

	return 0;
}

//=============================================================================
// TextureManager::TextureWidth
//=============================================================================

int TextureManager::TextureWidth( int Bind )
{
	if( Bind == 0 )
		return 0;

	return m_TextureBinds[Bind].Width;
}

//=============================================================================
// TextureManager::TextureHeight
//=============================================================================

int TextureManager::TextureHeight( int Bind )
{
	if( Bind == 0 )
		return 0;

	return m_TextureBinds[Bind].Height;
}

//=============================================================================
// TextureManager::TextureFormat
//=============================================================================

D3DFORMAT TextureManager::TextureFormat( int Bind )
{
	if( Bind == 0 )
		return D3DFMT_UNKNOWN;

	return m_TextureBinds[Bind].Format;
}

//=============================================================================
// TextureManager::Texture
//=============================================================================

LPDIRECT3DTEXTURE9 TextureManager::Texture( int Bind )
{
	if( Bind == 0 )
		return 0;

	return m_TextureBinds[Bind].pTexture;
}

//=============================================================================
// TextureManager::CreateTextureFromBitmap
//=============================================================================

int TextureManager::CreateTextureFromBitmap( BITMAP2 *pBitmap, int Width, int Height, D3DFORMAT Format, D3DPOOL Pool )
{
	int Bind = TextureBind(0);

	if( Bind != 0 )
	{
		LPDIRECT3DTEXTURE9 pTexture = d3d.CreateTextureFromBitmap( pBitmap, Width, Height, Format, Pool );

		if( !pTexture )
			return 0;

		Bind = RegisterTexture( pTexture );

		if( Bind == 0 )
			pTexture->Release();
	}

	return Bind;
}

TextureManager tm;

//=============================================================================
// TextureDatabase::TextureDatabase
//=============================================================================

TextureDatabase::TextureDatabase( const char* name )
{
	mFlags = 0;
	mPieces = 0;
	mPieceCount = 0;
	next = 0;
	prev = 0;
}

//=============================================================================
// TextureDatabase::~TextureDatabase
//=============================================================================

TextureDatabase::~TextureDatabase()
{
}

//=============================================================================
// TextureDatabase::GetTexture
//=============================================================================

Texture* TextureDatabase::GetTexture( u16 index )
{
	return mTextures[index];
}

u16 TextureDatabase::AppendTexture(Texture* newTexture)
{
	mTextures.Add(newTexture);
	return mTextures.Count()-1;
}

//=============================================================================
// Texture::Texture - Construct an empty texture
//=============================================================================

Texture::Texture(TextureDatabase* owner, const char* name)
:	mOwner(owner), 
	mImages(1,1), 
	mFlags(0), 
	mImageNumber(0)
{
	strncpy(mName, name, TEXTURE_NAME_LENGTH);
	mName[TEXTURE_NAME_LENGTH-1] = '\0';
}

//=============================================================================
// Texture::AddImage
//=============================================================================

u32 Texture::AddImage(bool FromXpr, u16 flags, u16 format, u16 width, u16 height, void* data, u16 palFmt, u16 palWidth, u16 palHeight, void* palette /* = NULL*/)
{
	mImages.Add(new Image(flags, format, width, height, data, 
		palFmt, palWidth, palHeight, palette));

	return mImages.Count()-1;
}

Image::Image( u16 imageFlags, u16 imageFormat, u16 width, u16 height, void* data, u16 palFormat, u16 palWidth, u16 palHeight, void* palette, bool FromXPR )
:	mLockCount(0),
	mFlags(imageFlags)
{
	mFormat = imageFormat;
	mWidth = width;
	mHeight = height;

	u16 type = imageFormat & TYPE_MASK;
	if(type == TYPE_CI || type == TYPE_CI_IA)
	{
		mFlags |= HAS_PALETTE;

		if(palFormat == 0)
			palFormat = SIZE_32_BIT|TYPE_RGBA;

		if(palWidth == 0)
			palWidth = 16;

		if(palHeight == 0)
		{
			u16 depth = imageFormat & SIZE_MASK;
			if(depth == SIZE_8_BIT)
				palHeight = 16;
			else if(depth == SIZE_4_BIT)
				palHeight = 1;
			else
				ASSERT(false);
		}
	}

	mPaletteFormat = palFormat;
	mPaletteWidth = palWidth;
	mPaletteHeight = palHeight;
}

//=============================================================================
// TextureBind__GetWidth - Returns the width of a texture
//=============================================================================

u32 TextureBind__GetWidth( u16 u16Bind )
{
	return (u32) tm.TextureWidth( u16Bind );
}

//=============================================================================
// TextureBind__GetHeight - Returns the height of a texture
//=============================================================================

u32 TextureBind__GetHeight( u16 u16Bind )
{
	return (u32) tm.TextureHeight( u16Bind );
}


//=============================================================================
// TextureBind__IsFlagSet - Returns true if the specified flag is set, false
//									 if it is not.
//=============================================================================

bool TextureBind__IsFlagSet( u16 u16Bind, u16 u16Flag )
{
	if( u16Flag == 0x08 ) //TEXTURE_BIND_UVREVERSED
	{
		u32 Width = TextureBind__GetWidth( u16Bind );
		u32 Height = TextureBind__GetHeight( u16Bind );

		if( Height > Width )
			return true;
	}

	return false;
}

//=============================================================================
// TextureMgr__GetBind - Returns the unsigned 16 bind for a texture widthin a
//								 texture set.
//=============================================================================

u16 TextureMgr__GetBind( ts_TextureSet *pTextureSet, u32 u32Index )
{
	if( !pTextureSet )
		return 0;

	TextureSetRecord *pTextureSetRecord = TextureMgr__GetTextureSetRecord( pTextureSet );

	if( pTextureSetRecord )
		return (u16) pTextureSetRecord->m_BindList[u32Index];

	return 0;
}

//=============================================================================
// TextureMgr__GetTexture
//=============================================================================

LPDIRECT3DTEXTURE9 TextureMgr__GetTextureDirect3D( int Bind )
{
	return tm.Texture( Bind );
}

u8 PS2PaletteSwizzle[256] = {

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

}; // PS2PaletteSwizzle

//=============================================================================
// SpriteMgr__ProcessTexture
//=============================================================================

u16 TextureMgr__ProcessTexture( ts_TextureChunkRecord *pTextureRecord )
{
	int Bind = 0;

	if( pTextureRecord )
	{
		BITMAP2 Bitmap;

		Bitmap.Width = (int) pTextureRecord->s16TextureWidth;
		Bitmap.Height = (int) pTextureRecord->s16TextureHeight;
		Bitmap.Format = PIXELFMT_UNKNOWN;
		Bitmap.Stride = (int) pTextureRecord->s16TextureWidth;
		Bitmap.pBits = 0;

		u32 TexFmt = pTextureRecord->u32Flags & TEX_FLAG_FMT_MASK;
		u32 TexBPP = pTextureRecord->u32Flags & TEX_FLAG_SIZE_MASK;

		if( TexFmt == TEX_FLAG_FMT_RGB )
		{
		}
		else if( TexFmt == TEX_FLAG_FMT_RGBA )
		{
		}
		else if( TexFmt == TEX_FLAG_FMT_CI )
		{
			if( TexBPP == TEX_FLAG_SIZE_8_BIT )
			{
				int PaletteSize =
					pTextureRecord->s16PaletteWidth *
					pTextureRecord->s16PaletteHeight;

				int A, R, G, B;
				int i, n;

				for( i = 0; i < PaletteSize; i++ )
				{
					n = (int) PS2PaletteSwizzle[i];

					R = (int) pTextureRecord->pPalette[n].R << 16;
					G = (int) pTextureRecord->pPalette[n].G << 8;
					B = (int) pTextureRecord->pPalette[n].B << 0;
					A = (int) pTextureRecord->pPalette[n].A << 0;

					if( A > 0 )
						A = (A * 2 - 1) << 24;

					Bitmap.Palette[i] = (COLOR32) (A|R|G|B);
				}

				Bitmap.Format = PIXELFMT_PALETTE8;
				Bitmap.pBits = (BYTE *) pTextureRecord->pTexture;
			}
		}
		else
		{
		}

		Bind = tm.CreateTextureFromBitmap( &Bitmap, Bitmap.Width, Bitmap.Height, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED );
	}

	return Bind;
}

bool TextureMgr__ProcessTextureSet( t_Handle Handle, void *pChunkDataVoid, ts_DBChunkHdr *pChunkHeader, void **ppRetData, u32 *pu32DataSize)
{
	TextureSetRecord *pTextureSetRecord = tm.NewTextureSetRecord();

	if( !pTextureSetRecord )
		return false;

	pTextureSetRecord->m_Handle = Handle;

	//////////////////////////////////////////////////////////////////////////
	
	ts_TextureSet *pTextureSet = &pTextureSetRecord->m_TextureSet;
	ts_TextureChunkHeader *pTextureHeader;
	ts_TextureChunkRecord *pTextureRecord;

	u8 *pChunkData = (u8 *) pChunkDataVoid;

	pTextureHeader = (ts_TextureChunkHeader *) pChunkData;


	u32 TextureCount = pTextureHeader->u32TextureCount;

	TextureDatabase* pTextureDatabase = pTextureSet;

	pTextureDatabase->next = 0;
	pTextureDatabase->prev = 0;
	pTextureDatabase->pTextureContextList = 0;

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

			pTextureRecord = pTextureRecord->pNext;
		}

		Texture* pTexture = new Texture( pTextureSet, pTextureHeader->Name );

		if( pTexture )
		{
			pTextureRecord = &pTextureHeader->TextureChunkRecord[i];

			pTextureSetRecord->m_BindList[i] =
				TextureMgr__ProcessTexture( &pTextureHeader->TextureChunkRecord[i] );

			u16 Bind = pTextureSetRecord->m_BindList[i];
			u16 TexWidth = (u16) TextureBind__GetWidth(Bind);
			u16 TexHeight = (u16) TextureBind__GetHeight(Bind);

			pTexture->AddImage( false, 0, 0, TexWidth, TexHeight, 0 );
			pTextureDatabase->AppendTexture( pTexture );
		}
	}

	return true;
}

bool TextureMgr__ProcessBitmap( t_Handle Handle, HBITMAP hbm )
{
	if( hbm )
	{
		TextureSetRecord *pTextureSetRecord = tm.NewTextureSetRecord();

		if( !pTextureSetRecord )
			return false;

		pTextureSetRecord->m_Handle = Handle;

		TextureDatabase *pTextureDatabase = &pTextureSetRecord->m_TextureSet;

		pTextureDatabase->next = 0;
		pTextureDatabase->prev = 0;
		pTextureDatabase->pTextureContextList = 0;

		BITMAP2 b2 = GetBitmap2Struct( hbm, TRUE );

		int TexWidth  = TextureManager::GetBestTextureSize( b2.Width );
		int TexHeight = TextureManager::GetBestTextureSize( b2.Height );

		pTextureSetRecord->m_BindList[0] = 
			tm.CreateTextureFromBitmap( &b2, TexWidth, TexHeight, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED );

		Texture *pTexture = new Texture( pTextureDatabase, "HBITMAP" );

		if( pTexture )
		{
			int Bind = pTextureSetRecord->m_BindList[0];
			int TexWidth = (u16) TextureBind__GetWidth( Bind );
			int TexHeight = (u16) TextureBind__GetHeight( Bind );

			pTexture->AddImage( false, 0, 0, TexWidth, TexHeight, 0 );
			pTextureDatabase->AppendTexture( pTexture );
			//pTextureSetRecord->m_pTexture = pTexture;

			return true;
		}
	}

	return false;
}

ts_TextureSet * TextureMgr__GetTextureSet( t_Handle Handle )
{
	TextureSetRecord *pTextureSetRecord = tm.GetTextureSetRecord( Handle );

	if( !pTextureSetRecord )
		return 0;

	return &pTextureSetRecord->m_TextureSet;
}

bool TextureMgr__SaveTexture( char *pFileName, t_Handle Handle, int Texture )
{
	LPDIRECT3DTEXTURE9 pTexture = TextureMgr__GetTextureDirect3D( Handle, Texture );

	if( !pTexture )
		return false;

	LPDIRECT3DSURFACE9 pSurface;
	HRESULT hresult = pTexture->GetSurfaceLevel( 0, &pSurface );

	if( FAILED(hresult) )
		return false;

	D3DSURFACE_DESC SurfaceDesc;
	D3DLOCKED_RECT SurfaceLock;
	HBITMAP hbm = 0;
	bool Result = false;

	hresult = pSurface->GetDesc( &SurfaceDesc );

	if( !FAILED(hresult) )
	{
		hresult = pSurface->LockRect( &SurfaceLock, 0, D3DLOCK_READONLY );
		
		if( !FAILED(hresult) )
		{
			int Width = SurfaceDesc.Width;
			int Height = SurfaceDesc.Height;
			int x, y;

			hbm = CreateDIB( Width, Height, 32 );

			if( hbm )
			{
				BITMAP2 b2 = GetBitmap2Struct( hbm, TRUE );

				BeginBitmap( 0, &b2 );
				BeginBitmap( 1, Width, Height, PixelFormatFromDirectX( SurfaceDesc.Format ), SurfaceLock.Pitch, SurfaceLock.pBits );

				for( y = 0; y < Height; y++ )
					for( x = 0; x < Width; x++ )
						SetPixelU( 0, x, y, GetPixelU( 1, x, y ) );

				FinishBitmap(0);
				FinishBitmap(1);

				Result = SaveDIB( pFileName, hbm, 0 );

				ReleaseDIB( hbm );
			}

			pSurface->UnlockRect();
		}
	}

	pSurface->Release();

	return Result;
}

TextureSetRecord * TextureMgr__GetTextureSetRecord( ts_TextureSet *pTextureSet )
{
	return tm.GetTextureSetRecord( pTextureSet );
}

LPDIRECT3DTEXTURE9 TextureMgr__GetTextureDirect3D( t_Handle Handle, int Texture )
{
	LPDIRECT3DTEXTURE9 pTexture = 0;

	if( Handle != INVALID_HANDLE )
	{
		u16 Bind = TextureMgr__GetBind( TextureMgr__GetTextureSet( Handle ), Texture );

		if( Bind != 0 )
			return TextureMgr__GetTextureDirect3D( Bind );
	}

	return pTexture;
}