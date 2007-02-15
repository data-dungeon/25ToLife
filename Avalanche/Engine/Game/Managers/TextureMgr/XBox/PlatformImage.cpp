
/****************************************/
/*                                      */
/*   PlatformImage.cpp                  */
/*   Tyler Colbert                      */
/*   DX version - Paul Blagay				 */
/*   DirectX specific code for an image */
/*                                      */
/****************************************/


/******************* includes ****************************/

#include "Game/GamePCH.h"

#include <xgraphics.h>
#include "Game/Managers/TextureMgr/PlatformImage.h"
#ifdef _DEBUG
#include "Layers/ttyColors.h"
#endif

#ifdef _XBOX
extern int g_xboxmotherfuckingpalettehint;
#endif

static unsigned char PS2PaletteSwizzle[256] =
{
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
	0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

D3DVIEWPORT8			PlatformImage::RTTBackupViewport;
IDirect3DSurface8 *	PlatformImage::pRTTBackupRTSurface = 0;
IDirect3DSurface8 *	PlatformImage::pRTTBackupDSSurface = 0;
IDirect3DSurface8 *	PlatformImage::pRTTDstSurface = 0;

/// @{
/// Statistics
static u32 contextChangeCount;
u32 GetContextChangeCount(void)
{
	return contextChangeCount;
}
void ClearContextChangeCount(void)
{
	contextChangeCount = 0;
}
/// @}

/// create the platform specific info given the inputs to the contructor
PlatformImage::PlatformImage( Image * _Owner, void * _Data, void * _Palette, bool FromXPR) :
	m_Owner( _Owner)
{
	m_Flags = 0;

	p_CPUSideImageBuffer = 0;

	m_CPUSideImageRect.top = 0;
	m_CPUSideImageRect.left = 0;
	m_CPUSideImageRect.bottom = m_Owner->GetHeight( );
	m_CPUSideImageRect.right = m_Owner->GetWidth( );

	m_CPUSideImageFormat = D3DFMT_A8R8G8B8;
	m_CPUSideImageBytesPP = 4;

	if( (m_Owner->GetFlags( ) & Image::BACK_BUFFER)  ||  (m_Owner->GetFlags( ) & Image::FRONT_BUFFER) )
	{
		m_MaxMIPLevels = 1;
		m_Flags |= SURFACE_REF;
	}
	else if( m_Owner->GetFlags( ) & Image::RENDER_TARGET )
	{
		m_MaxMIPLevels = 1;
	}
	else
		m_MaxMIPLevels = maxMIPLevels( m_Owner->GetWidth( ), m_Owner->GetHeight( ) );

	m_MIPLevelBuffers = new MIPLevelData [m_MaxMIPLevels];
	ASSERT( m_MIPLevelBuffers );

	for( unsigned int i = 0; i < m_MaxMIPLevels; i++ )
	{
		m_MIPLevelBuffers[i].p_Image = 0;
		m_MIPLevelBuffers[i].p_Palette = 0;
	}

	m_Usage = 0;

	p_MasterPalette = 0;
	p_MasterPaletteData = 0;

	if( (m_Owner->GetFlags( ) & Image::BACK_BUFFER)  ||  (m_Owner->GetFlags( ) & Image::FRONT_BUFFER) )
	{
		ASSERT( _Data == 0 );
		ASSERT( _Palette == 0 );
		ASSERT( FromXPR == false );

		if( m_Owner->GetFlags( ) & Image::BACK_BUFFER )
			createSurfaceRefTexture( 0 );
		else
			createSurfaceRefTexture( -1 );

		b_OwnImages = false;
		b_OwnPalettes = false;
	}
	else if( m_Owner->GetFlags( ) & Image::RENDER_TARGET )
	{
		ASSERT( _Data == 0 );
		ASSERT( _Palette == 0 );
		ASSERT( FromXPR == false );

		b_OwnImages = false;
		b_OwnPalettes = false;

		m_MIPLevels = 1;

		if( isPow2( m_Owner->GetWidth( ) )  &&  isPow2( m_Owner->GetHeight( ) ) )
		{
			m_Format = D3DFMT_A8R8G8B8;

			m_UScale = 1.0f;
			m_VScale = 1.0f;
		}
		else
		{
			m_Format = D3DFMT_LIN_A8R8G8B8;

			m_UScale = m_Owner->GetWidth( );
			m_VScale = m_Owner->GetHeight( );
		}

		HRESULT result = DirectX_Device->CreateTexture( m_Owner->GetWidth( ), m_Owner->GetHeight( ), 1, 0, m_Format, 0, & p_D3DTexture);
		ASSERT( result == D3D_OK);
	}
	else 
	{
		m_UScale = 1.0;
		m_VScale = 1.0;

		p_D3DTexture = 0;

		// grab some info
		if( m_Owner->GetFlags( ) & Image::HAS_PALETTE )
		{
			ASSERT( (m_Owner->GetFormat( ) & Image::TYPE_MASK) == Image::TYPE_CI );
			ASSERT( _Palette != NULL );
		}
		else
		{
			ASSERT( _Palette == NULL );
		}

		bool Convert = internalizeFormat( m_Owner->GetFormat( ), m_Format, m_BytesPP );

		void * pNewImage = 0;
		void * pNewPalette = 0;

		if (Convert && FromXPR == false)
			internalizeImage( m_Owner->GetFormat( ), _Data, _Palette, pNewImage, pNewPalette );

		if( FromXPR )
		{
			pNewImage = _Data;
			p_D3DTexture = (D3DTexture *) _Data;
		}

		if( pNewImage )
		{
			b_OwnImages = true;
			_Data = pNewImage;
		}
		else
			b_OwnImages = false;

		if( pNewPalette )
		{
			b_OwnPalettes = true;
			_Palette = pNewPalette;
		}
		else
			b_OwnPalettes = false;

		if( _Palette )
		{
			p_MasterPaletteData = (PALETTEENTRY *) memAlloc( 256 * sizeof( PALETTEENTRY ) );
#ifdef _XBOX
			int entries = g_xboxmotherfuckingpalettehint;
#else
			int entries = 256;
#endif
			memcpy( p_MasterPaletteData, _Palette, entries * sizeof( PALETTEENTRY ) );
			if (256 - entries != 0)
				memset( (char *)p_MasterPaletteData + entries * sizeof( PALETTEENTRY) , '\0', 256 - entries );
		}

		m_MIPLevelBuffers[0].p_Image = _Data;
		m_MIPLevelBuffers[0].p_Palette = _Palette;

		m_MIPLevels = 1;
	}

	return;
}

void PlatformImage::createSurfaceRefTexture( int bufferIndex )
{
	ASSERT( bufferIndex <= 0 );

	IDirect3DSurface8 * pBackBufferSurface = 0;

	HRESULT result = D3D_OK;

	result = DirectX_Device->GetBackBuffer( bufferIndex, D3DBACKBUFFER_TYPE_MONO, & pBackBufferSurface );
	ASSERT( result == D3D_OK );
	ASSERT( pBackBufferSurface );

	D3DSURFACE_DESC Desc;

	pBackBufferSurface->GetDesc( & Desc );

	if( XGIsSwizzledFormat( Desc.Format ) )
	{
		m_UScale = m_VScale = 1.0f;
	}
	else
	{
		m_UScale = (float) Desc.Width;
		m_VScale = (float) Desc.Height;
	}

	m_Format = Desc.Format;
	m_BytesPP = XGBytesPerPixelFromFormat( Desc.Format );

	p_D3DTexture = new D3DTexture;
	memset( p_D3DTexture, 0, sizeof( D3DTexture ) );

	XGSetTextureHeader( Desc.Width, Desc.Height, 1, 0, Desc.Format, 0, p_D3DTexture, pBackBufferSurface->Data, Desc.Width * m_BytesPP );

	pBackBufferSurface->Release( );
}

void PlatformImage::internalizeImage( unsigned int ownerFormat, const void * pImage, const void * pPalette, void * & pNewImage, void * & pNewPalette )
{
	pNewImage = 0;
	pNewPalette = 0;

	unsigned int imageType = ownerFormat & Image::TYPE_MASK;
	unsigned int imageSize = ownerFormat & Image::SIZE_MASK;

	unsigned int width = m_Owner->GetWidth( );
	unsigned int height = m_Owner->GetHeight( );

	switch( imageType )
	{
	case Image::TYPE_RGB:
		switch( imageSize )
		{
		case Image::SIZE_4_BIT:
		case Image::SIZE_8_BIT:
		case Image::SIZE_32_BIT:
		default:
			ASSERT( 0 );
			break;
		case Image::SIZE_16_BIT:
			{
			if( pImage )
			{
				pNewImage = (void *) memAlloc( width * height * 2 );
				ASSERT( pNewImage );

				convertImagex555to565( pNewImage, pImage, width, height );
			}
			}
			break;
		case Image::SIZE_24_BIT:
			if( pImage )
			{
				pNewImage = (void *) memAlloc( width * height * 4 );
				ASSERT( pNewImage );

				convertImageRGBtoXRGB( pNewImage, pImage, width, height );
			}
			break;
		}
		break;
	case Image::TYPE_RGBA:
		switch( imageSize )
		{
		case Image::SIZE_4_BIT:
		case Image::SIZE_8_BIT:
		case Image::SIZE_24_BIT:
		default:
			ASSERT( 0 );
			break;
		case Image::SIZE_16_BIT:
			break;
		case Image::SIZE_32_BIT:
			{

			if( pImage )
			{
				pNewImage = (void *) memAlloc( width * height * 4 );
				ASSERT( pNewImage );

				convertImageRGBAtoARGB( pNewImage, pImage, width, height );
			}
			break;
			}
		}
		break;
	case Image::TYPE_CI:
		switch( imageSize )
		{
		case Image::SIZE_4_BIT:
			{

			if( pPalette )
			{
				pNewPalette = (void *) memAlloc( 256 * sizeof( PALETTEENTRY ) );
				ASSERT( pNewPalette );

				convertPalette( pNewPalette, pPalette, 16 );
			}

			if( pImage )
			{
				pNewImage = memAlloc( width * height );
				ASSERT( pNewImage );

				convertPalettedImage4to8( pNewImage, pImage, width, height );
			}
			break;
			}
		case Image::SIZE_8_BIT:
			{

			if( pPalette )
			{
				pNewPalette = (void *) memAlloc( 256 * sizeof( PALETTEENTRY ) );
				ASSERT( pNewPalette );

				convertPalette( pNewPalette, pPalette, 256 );
			}

			if( pImage )
			{
				pNewImage = memAlloc( width * height );
				ASSERT( pNewImage );

				convertPalettedImage8to8( pNewImage, pImage, width, height );
			}
			break;
			}
		case Image::SIZE_16_BIT:
		case Image::SIZE_24_BIT:
		case Image::SIZE_32_BIT:
		default:
			ASSERT( 0 );
			break;
		}
		break;
	case Image::TYPE_LUMINANCE:
		break;
	case Image::TYPE_IA:
		break;
	}

}

void PlatformImage::UpdateMyShiznit( void )
{
	if( m_Flags & SURFACE_REF )
	{
		ASSERT( p_D3DTexture );
		ASSERT( m_Owner );

		int bufferIndex = m_Owner->GetFlags( ) & Image::BACK_BUFFER ? 0 : -1;

		IDirect3DSurface8 * pBackBufferSurface = 0;

		HRESULT result = D3D_OK;

		result = DirectX_Device->GetBackBuffer( bufferIndex, D3DBACKBUFFER_TYPE_MONO, & pBackBufferSurface );
		ASSERT( result == D3D_OK );
		ASSERT( pBackBufferSurface );

		p_D3DTexture->Data = pBackBufferSurface->Data;

		pBackBufferSurface->Release( );
	}
}

bool PlatformImage::internalizeFormat( unsigned int ownerFormat, D3DFORMAT & internalFormat, unsigned int & internalBytesPP )
{
	bool bNeedsConversion = false;

	unsigned int imageType = ownerFormat & Image::TYPE_MASK;
	unsigned int imageSize = ownerFormat & Image::SIZE_MASK;

	switch( imageType )
	{
	case Image::TYPE_RGB:
		switch( imageSize )
		{
		case Image::SIZE_4_BIT:
		case Image::SIZE_8_BIT:
		case Image::SIZE_32_BIT:
		default:
			ASSERT( 0 );
			break;
		case Image::SIZE_16_BIT:
			internalFormat = D3DFMT_R5G6B5;
			internalBytesPP = 2;

			bNeedsConversion = true;
			break;
		case Image::SIZE_24_BIT:
			internalFormat = D3DFMT_X8R8G8B8;
			internalBytesPP = 4;

			bNeedsConversion = true;
			break;
		}
		break;
	case Image::TYPE_RGBA:
		switch( imageSize )
		{
		case Image::SIZE_4_BIT:
		case Image::SIZE_8_BIT:
		case Image::SIZE_24_BIT:
		default:
			ASSERT( 0 );
			break;
		case Image::SIZE_16_BIT:
			internalFormat = D3DFMT_A1R5G5B5;
			internalBytesPP = 2;
			break;
		case Image::SIZE_32_BIT:
			internalFormat = D3DFMT_A8R8G8B8;
			internalBytesPP = 4;

			bNeedsConversion = true;
			break;
		}
		break;
	case Image::TYPE_CI:
		switch( imageSize )
		{
		case Image::SIZE_4_BIT:
			internalFormat = D3DFMT_P8;
			internalBytesPP = 1;

			bNeedsConversion = true;
			break;
		case Image::SIZE_8_BIT:
			internalFormat = D3DFMT_P8;
			internalBytesPP = 1;

			bNeedsConversion = true;
			break;
		case Image::SIZE_16_BIT:
		case Image::SIZE_24_BIT:
		case Image::SIZE_32_BIT:
		default:
			ASSERT( 0 );
			break;
		}
		break;
	case Image::TYPE_LUMINANCE:
		internalFormat = D3DFMT_L8;
		internalBytesPP = 1;

		break;
	case Image::TYPE_IA:
		internalFormat = D3DFMT_A8L8;
		internalBytesPP = 1;

		break;
	}

	return bNeedsConversion;
}

bool PlatformImage::isPow2( unsigned int val )
{
	ASSERT( val );

	unsigned int pow2 = 1;

	do
	{
		if( pow2 == val )
			return true;

		pow2 *= 2;

	} while( pow2 <= val );

	return false;
}

void PlatformImage::Init( void )
{

}

void PlatformImage::Cleanup( void )
{

}

void PlatformImage::createD3DTexture( void )
{
	ASSERT( (m_Flags & SURFACE_REF) == 0 );
	ASSERT( p_D3DTexture == 0 );

	D3DPOOL	pool = ( m_Usage == D3DUSAGE_DYNAMIC ) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;

	HRESULT result = DirectX_Device->CreateTexture( m_Owner->GetWidth( ), m_Owner->GetHeight( ), m_MIPLevels,
							m_Usage, m_Format, pool, & p_D3DTexture );
	ASSERT( result == D3D_OK );
	ASSERT( p_D3DTexture );

	RECT SrcRect = { 0 };

	SrcRect.bottom = m_Owner->GetHeight( );
	SrcRect.right = m_Owner->GetWidth( );

	unsigned int currentPitch = m_Owner->GetWidth( ) * m_BytesPP;

	PALETTEENTRY *	pPaletteData = 0;

	for( unsigned int i = 0; i < m_MIPLevels; i++ )
	{
		if( m_MIPLevelBuffers[i].p_Image )
		{
			IDirect3DSurface8 * pSurface = 0;

			result = p_D3DTexture->GetSurfaceLevel( i, & pSurface );
			ASSERT( result == D3D_OK );

			int	temp = m_Format;

			result = D3DXLoadSurfaceFromMemory( pSurface, p_MasterPaletteData, NULL, m_MIPLevelBuffers[i].p_Image,
							m_Format, currentPitch, (PALETTEENTRY *) m_MIPLevelBuffers[i].p_Palette, & SrcRect, D3DX_DEFAULT, 0 );
			ASSERT( result == D3D_OK );

			pSurface->Release( );

			currentPitch >>= 1;
			SrcRect.bottom >>= 1;
			SrcRect.right >>= 1;

			if( b_OwnImages )
				memFree( m_MIPLevelBuffers[i].p_Image );

			if( b_OwnPalettes )
				memFree( m_MIPLevelBuffers[i].p_Palette );

			m_MIPLevelBuffers[i].p_Image = 0;
			m_MIPLevelBuffers[i].p_Palette = 0;

		}
	}

	if( p_MasterPaletteData )
	{
		HRESULT result = DirectX_Device->CreatePalette( D3DPALETTE_256, & p_MasterPalette );
		ASSERT( result == D3D_OK );
		ASSERT( p_MasterPalette );

		D3DCOLOR * pPalettePtr = 0;

		result = p_MasterPalette->Lock( & pPalettePtr, 0 );
		ASSERT( result == D3D_OK );
		ASSERT( pPalettePtr );

		memcpy( pPalettePtr, p_MasterPaletteData, 256 * sizeof( D3DCOLOR ) );

		result = p_MasterPalette->Unlock( );
		ASSERT( result == D3D_OK );

		memFree( p_MasterPaletteData );
		p_MasterPaletteData = 0;
	}


}

void PlatformImage::EndMIPLevels( unsigned int usage )
{
	ASSERT( (m_Flags & SURFACE_REF) == 0 );
	ASSERT( p_D3DTexture == 0 );

	m_Usage = usage;
	createD3DTexture( );
}

/// Make this image available for rendering
void PlatformImage::Lock( void )
{
	// is it already uploaded?
	ASSERT( !(m_Flags & LOCK_OUTSTANDING) );

	UpdateMyShiznit();

	m_Flags |= LOCK_OUTSTANDING;
}

/// we are done rendering with this image (but that doesnt mean we
/// wont do it again soon... )
void PlatformImage::Unlock( void )
{
	ASSERT( m_Flags & LOCK_OUTSTANDING );

	// if we have no cpu side alloc, and this is not a temp image, we
	// need to fetch it back from texmem... ick.. this is slow. we need
	// to be careful about using this.
	if( (m_Flags & TEMPORARY_IMAGE)  &&  p_CPUSideImageBuffer )
		FreeMainRam( );

	m_Flags &= ~LOCK_OUTSTANDING;
}

void PlatformImage::copyMIPLevelToRect( unsigned int level, D3DLOCKED_RECT & Rect )
{
	ASSERT( level < m_MIPLevels );
	ASSERT( m_MIPLevelBuffers[level].p_Image );
	ASSERT( Rect.pBits );
	ASSERT( Rect.Pitch );

	char * pSrcPtr = (char *) m_MIPLevelBuffers[level].p_Image;
	char * pDstPtr = (char *) Rect.pBits;

	const unsigned int width = m_Owner->GetWidth( ) >> (level - 1);
	const unsigned int height = m_Owner->GetHeight( ) >> (level - 1);

	for( unsigned int i = 0; i < height; i++ )
	{
		memcpy( pDstPtr, pSrcPtr, width * m_BytesPP );

		pDstPtr += Rect.Pitch;
		pSrcPtr += width * m_BytesPP;
	}
}

void PlatformImage::copyRectToCPUBuffer( D3DLOCKED_RECT & Rect )
{
	ASSERT( p_CPUSideImageBuffer );
	ASSERT( Rect.pBits );
	ASSERT( Rect.Pitch );

	char * pSrcPtr = (char *) Rect.pBits;
	char * pDstPtr = (char *) p_CPUSideImageBuffer;

	const unsigned int width = m_Owner->GetWidth( );
	const unsigned int height = m_Owner->GetHeight( );

	for( unsigned int r = 0; r < height; r++ )
	{
		memcpy( pDstPtr, pSrcPtr, width * m_CPUSideImageBytesPP );

		pDstPtr += Rect.Pitch;
		pSrcPtr += width * m_CPUSideImageBytesPP;
	}

}

void PlatformImage::SetCPUSideImageFormat( unsigned int format )
{
	bool bNeedsConversion = internalizeFormat( format, m_CPUSideImageFormat, m_CPUSideImageBytesPP );
	ASSERT( bNeedsConversion == false );
}

void PlatformImage::downloadImage( void )
{
	ASSERT( p_D3DTexture );

	if( p_CPUSideImageBuffer == NULL )
		allocCPUImageBuffer( );

	HRESULT result;
	D3DLOCKED_RECT		Rect;

	result = p_D3DTexture->LockRect( 0, & Rect, 0, 0 );
	ASSERT( result == D3D_OK );

	copyRectToCPUBuffer( Rect );

	result = p_D3DTexture->UnlockRect( 0 );
	ASSERT( result == D3D_OK );

	m_Flags |= TEMPORARY_IMAGE;
}

void PlatformImage::LockForRender( void )
{
	/* set up texture as frame buffer. to be a frame buffer, texture must be page aligned */
	ASSERT( m_Flags & LOCK_OUTSTANDING );
	ASSERT( pRTTBackupRTSurface == 0 );
	ASSERT( pRTTBackupDSSurface == 0 );

	HRESULT result = D3D_OK;

	result = DirectX_Device->GetRenderTarget( & pRTTBackupRTSurface );
	ASSERT( result == D3D_OK );
	result = DirectX_Device->GetDepthStencilSurface( & pRTTBackupDSSurface );
	ASSERT( result == D3D_OK );

	result = p_D3DTexture->GetSurfaceLevel( 0, &pRTTDstSurface );
	ASSERT( result == D3D_OK );
	ASSERT( pRTTDstSurface );

	D3DSURFACE_DESC Desc;

	result = pRTTDstSurface->GetDesc( & Desc );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->GetViewport( & RTTBackupViewport );
	ASSERT( result == D3D_OK );

	D3DVIEWPORT8	renderViewport = { 0, 0, Desc.Width, Desc.Height, 0.0, 1.0 };

	result = DirectX_Device->SetViewport( & renderViewport );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->SetRenderTarget( pRTTDstSurface, NULL );
	ASSERT( result == D3D_OK );
}

void PlatformImage::UnlockForRender( void )
{
	ASSERT( pRTTDstSurface );
	ASSERT( pRTTBackupRTSurface );
	ASSERT( pRTTBackupDSSurface );

	HRESULT result = D3D_OK;

	result = DirectX_Device->SetViewport( & RTTBackupViewport );
	ASSERT( result == D3D_OK );

	result = DirectX_Device->SetRenderTarget( pRTTBackupRTSurface, pRTTBackupDSSurface );
	ASSERT( result == D3D_OK );

	pRTTBackupRTSurface->Release( );
	pRTTBackupRTSurface = 0;

	pRTTBackupDSSurface->Release( );
	pRTTBackupDSSurface = 0;

	pRTTDstSurface->Release( );
	pRTTDstSurface = 0;
}

/// platform specific function to get a pointer to the image buffer
void * PlatformImage::GetImageBuffer( void )
{
	HRESULT result;
	D3DLOCKED_RECT		Rect;

	if( p_D3DTexture == 0 )
		createD3DTexture( );

	ASSERT( p_D3DTexture );
	ASSERT( !(m_Flags & BUFFER_FETCHED) );

	// grab the pbits pointer from D3D
	result = p_D3DTexture->LockRect( 0, & Rect, 0, 0 );
	m_Flags |= BUFFER_FETCHED;

	return Rect.pBits;
}

int PlatformImage::GetImageFormat(void)
{
	D3DSURFACE_DESC	Desc;
	IDirect3DSurface8 * pSurface = 0;
	HRESULT result = D3D_OK;

	result = p_D3DTexture->GetLevelDesc( 0, & Desc );

	return (Desc.Format);
}

/// platform specific function to release the image buffer
void PlatformImage::ReleaseImageBuffer( bool modified )
{
	HRESULT result = D3D_OK;
	ASSERT( m_Flags & BUFFER_FETCHED );
	m_Flags &= ~BUFFER_FETCHED;

	result = p_D3DTexture->UnlockRect( 0 );
	ASSERT( result == D3D_OK );
}

/// platform specific function to get a pointer to the palette buffer
void* PlatformImage::GetPaletteBuffer(void)
{
	return (void*)0;
}

void PlatformImage::BitBlt(int x, int y, int w, int h)
{
	struct
	{
		D3DXVECTOR4	Pos;
		D3DXVECTOR2	TCs;
	} QVerts[4];

	ASSERT( p_D3DTexture );

	IDirect3DSurface8 * pRenderTargetSurface = 0;

	HRESULT result = D3D_OK;

	result = DirectX_Device->GetRenderTarget( & pRenderTargetSurface );
	ASSERT( result == D3D_OK );

	D3DSURFACE_DESC		Desc;
	float						fEndU, fEndV;

	result = pRenderTargetSurface->GetDesc( & Desc );
	assert( result == D3D_OK );

	if( XGIsSwizzledFormat( Desc.Format ) )
	{
		fEndU = fEndV = 1.0f;
	}
	else
	{
		fEndU = (float) Desc.Width;
		fEndV = (float) Desc.Height;
	}

	// Vtx 1
	QVerts[0].Pos.x = 0.0f;
	QVerts[0].Pos.y = 0.0f;
	QVerts[0].Pos.z = 0.0f;
	QVerts[0].Pos.w = 1.0f;

	QVerts[0].TCs.x = 0.0f;
	QVerts[0].TCs.y = 0.0f;

	// Vtx 2
	QVerts[1].Pos.x = (float) (Desc.Width - 1);
	QVerts[1].Pos.y = 0.0f;
	QVerts[1].Pos.z = 0.0f;
	QVerts[1].Pos.w = 1.0f;

	QVerts[1].TCs.x = fEndU;
	QVerts[1].TCs.y = 0.0f;

	// Vtx 3
	QVerts[2].Pos.x = (float) (Desc.Width - 1);
	QVerts[2].Pos.y = (float) (Desc.Height - 1);
	QVerts[2].Pos.z = 0.0f;
	QVerts[2].Pos.w = 1.0f;

	QVerts[2].TCs.x = fEndU;
	QVerts[2].TCs.y = fEndV;

	// Vtx 4
	QVerts[3].Pos.x = 0.0;
	QVerts[3].Pos.y = (float) (Desc.Height - 1);
	QVerts[3].Pos.z = 0.0f;
	QVerts[3].Pos.w = 1.0f;

	QVerts[3].TCs.x = 0.0f;
	QVerts[3].TCs.y = fEndV;

	DirectX_Device->SetRenderState( D3DRS_LIGHTING, FALSE );
	DirectX_Device->SetRenderState( D3DRS_FOGENABLE, FALSE );
	_RenderState__Change( RS_FOG, FALSE );

	DWORD		saveAlphaState;
	DWORD		saveCullMode;
	DWORD		saveZWriteEnable;
	DWORD		saveZEnable;

	DirectX_Device->GetRenderState( D3DRS_ALPHABLENDENABLE, &saveAlphaState );
	DirectX_Device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	DirectX_Device->GetRenderState( D3DRS_CULLMODE, &saveCullMode );
	DirectX_Device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	DirectX_Device->GetRenderState( D3DRS_ZWRITEENABLE, &saveZWriteEnable );
	DirectX_Device->SetRenderState( D3DRS_ZWRITEENABLE, FALSE ); 

	DirectX_Device->GetRenderState( D3DRS_ZENABLE, &saveZEnable );
	DirectX_Device->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE ); 
    
	DirectX_Device->SetTexture( 0, p_D3DTexture );
	
	// Textures
	DirectX_Device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	DirectX_Device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	DirectX_Device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	DirectX_Device->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	DirectX_Device->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	DirectX_Device->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );

	DirectX_Device->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	DirectX_Device->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	DirectX_Device->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
	DirectX_Device->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
   DirectX_Device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );

   DirectX_Device->SetTexture( 1, NULL );
   DirectX_Device->SetTexture( 2, NULL );
   DirectX_Device->SetTexture( 3, NULL );
    

	// Set vertex shaders
	result = DirectX_Device->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX1 );
	assert( result == D3D_OK );

#ifdef _DEBUG
//	result = DirectX_Device->SetPixelShaderProgram( (D3DPIXELSHADERDEF *) dwSPGreyscalePixelShader );
//	assert( result == D3D_OK );
#endif
	// Draw vertices
	result = DirectX_Device->DrawVerticesUP( D3DPT_QUADLIST, 4, QVerts, sizeof(QVerts[0]) );
	assert( result == D3D_OK );

	// Restore state
	DirectX_Device->SetRenderState( D3DRS_ALPHABLENDENABLE, saveAlphaState );
	DirectX_Device->SetRenderState( D3DRS_CULLMODE, saveCullMode );

	DirectX_Device->SetRenderState( D3DRS_ZWRITEENABLE, saveZWriteEnable ); 
	DirectX_Device->SetRenderState( D3DRS_ZENABLE, saveZEnable ); 

	result = DirectX_Device->SetPixelShaderProgram( 0 );
	assert( result == D3D_OK );

	result = DirectX_Device->SetTexture( 0, NULL );
	assert( result == D3D_OK );

	pRenderTargetSurface->Release( );

	TextureState__RequestAll( 0, TS_TYPE_NONE, 0, 0, 0, 0, 0 );
}

void PlatformImage::BltGtoA(int x, int y, int w, int h)
{
	D3DLOCKED_RECT		Rect1;
	D3DLOCKED_RECT		Rect2;
	HRESULT result = D3D_OK;

	// lock target
	result = pRTTDstSurface->LockRect( &Rect1, 0, 0);

	// lock source
	result = p_D3DTexture->LockRect( 0, & Rect2, 0, 0 );

	u32*	src = (u32*)Rect2.pBits;
	u32*	dst = (u32*)Rect1.pBits;

	// Joe - maybe its not working because Im not using pitch??
	// wasnt sure exactly how this stuff needs to go on the
	// blit.. - Paul

	// copy over the green to alpha channel only
	for (int i = 0; i < (w * h); i++)
	{
		u32	srcv;
		u32	dstv;

		// get green from src
		srcv = *src & 0x0000FF00;
		// move to alpha channel
		srcv = srcv << 16;

		// get dst pixel
		dstv = *dst;

		// or in the alpha channel
		dstv = dstv | srcv;

		// store off new pixel
		*dst = dstv;

		src++;
		dst++;
	}

	// lock target
	result = pRTTDstSurface->UnlockRect();

	// lock source
	result = p_D3DTexture->UnlockRect(0);
}

/// platform specific function to release the palette buffer
void PlatformImage::ReleasePaletteBuffer(bool modified)
{
}

unsigned int PlatformImage::maxMIPLevels( unsigned int width, unsigned int height )
{
	unsigned int smallest = (width < height) ? width : height;

	unsigned int maxLevels = 1;

	while( smallest > 1 )
	{
		smallest = smallest >> 1;
		maxLevels++;
	}

	return maxLevels;
}

PlatformImage::~PlatformImage( )
{
	Release( );
	FreeMainRam( );

	if( m_MIPLevelBuffers )
	{
		delete [] m_MIPLevelBuffers;
		m_MIPLevelBuffers = 0;
	}

	if( p_MasterPalette )
	{
      p_MasterPalette->Release( );
		p_MasterPalette = 0;
	}

	if( p_MasterPaletteData )
	{
		memFree( p_MasterPaletteData );
		p_MasterPaletteData = 0;
	}
}

void PlatformImage::SetMipmapDistance( float fDistance )
{
	// I cant see any way to control this under DirectX
}


void PlatformImage::AppendMipLevel( u16 format, u16 width, u16 height, void * _Data, u16 palFmt, u16 palWidth, u16 palHeight, void * _Palette )
{
	ASSERT( (m_Flags & SURFACE_REF) == 0 );
	ASSERT( p_D3DTexture == 0 );
	ASSERT( m_MIPLevels < m_MaxMIPLevels );
	ASSERT( m_MIPLevelBuffers[m_MIPLevels].p_Image == 0 );

	ASSERT( format == m_Owner->GetFormat( ) );
	ASSERT( (m_Owner->GetWidth( ) >> m_MIPLevels ) == width );
	ASSERT( (m_Owner->GetHeight() >> m_MIPLevels ) == height );
	ASSERT( palFmt == m_Owner->GetPaletteFormat( ) );
	ASSERT( palWidth == m_Owner->GetPaletteWidth( ) );
	ASSERT( palHeight == m_Owner->GetPaletteHeight( ) );

	void * pNewImage = 0;
	void * pNewPalette = 0;

//	internalizeImage( m_Owner->GetFormat( ), _Data, _Palette, pNewImage, pNewPalette );

	ASSERT( ( (b_OwnImages == true)  &&  (pNewImage != 0) )  ||  ( (b_OwnImages == false)  &&  (pNewImage == 0) ) );

	if( pNewImage )
		_Data = pNewImage;

	ASSERT( ( (b_OwnPalettes == true)  &&  (pNewPalette != 0) )  ||  ( (b_OwnPalettes == false)  &&  (pNewPalette == 0) ) );

	if( pNewPalette )
		_Palette = pNewPalette;

	m_MIPLevelBuffers[m_MIPLevels].p_Image = _Data;
	m_MIPLevelBuffers[m_MIPLevels].p_Palette = _Palette;

	m_MIPLevels++;
}

/// release..  actually release
/// the texture memory allocated by this image.
void PlatformImage::Release( )
{
	// On the PS2, this happens if we ran out of texture memory.
	// too many textures are locked at one time, and the system starts
	// trying to free locked ones from the LRU
	ASSERTS( !(m_Flags & LOCK_OUTSTANDING), "Release locked texture?" );

	for( unsigned int unit = 0; unit < DXBase.DeviceCaps.MaxTextureBlendStages; unit++ )
	{
		IDirect3DBaseTexture8 * pCheckTexture = 0;

		HRESULT result = DirectX_Device->GetTexture( unit, & pCheckTexture );
		ASSERT( result == D3D_OK );

		if( pCheckTexture == p_D3DTexture )
		{
			result = DirectX_Device->SetTexture( unit, 0 );
			ASSERT( result == D3D_OK );
		}

		if( pCheckTexture )
			pCheckTexture->Release( );

		if( p_MasterPalette )
		{
			IDirect3DPalette8	* pCheckPalette = 0;

			result = DirectX_Device->GetPalette( unit, & pCheckPalette );
			ASSERT( result == D3D_OK );

			if( pCheckPalette == p_MasterPalette )
			{
				result = DirectX_Device->SetPalette( unit, 0 );
				ASSERT( result == D3D_OK );
			}

			if( pCheckPalette )
				pCheckPalette->Release( );
		}
	}

	if( p_D3DTexture )
	{
		if( m_Flags & SURFACE_REF )
			delete p_D3DTexture;
		else
		{
			unsigned int useCount = p_D3DTexture->Release( );
		}

		p_D3DTexture = 0;
	}
}

void PlatformImage::allocCPUImageBuffer( void )
{
	ASSERT( p_CPUSideImageBuffer == 0 );
	MEM_SET_ALLOC_NAME( "PlatformImage" );

	p_CPUSideImageBuffer = memAlloc( m_Owner->GetWidth( ) * m_Owner->GetHeight( ) * m_CPUSideImageBytesPP );
	ASSERT( p_CPUSideImageBuffer );

	m_Flags |= OWNS_CPU_BUFFER;
}

/// release the memory used in main memory.  If this image is not
/// in texture memory, its data is lost to the ether.
void PlatformImage::FreeMainRam( void )
{
	ASSERTS( !(m_Flags & BUFFER_FETCHED), "Free while GetImageBuffer outstanding" );

	if( (m_Flags & OWNS_CPU_BUFFER)  &&  p_CPUSideImageBuffer )
	{
		memFree( p_CPUSideImageBuffer );
		p_CPUSideImageBuffer = 0;
	}

	m_Flags &= ~OWNS_CPU_BUFFER;
}

/// prep this image for drawing operations
/// this is rarely called on the ps2
void PlatformImage::Select( void )
{
	int texUnit = TextureState__ActiveUnit( );

	if( TextureState__Get( texUnit, TS_TYPE ) != TS_TYPE_NONE )
	{
		if( p_D3DTexture == 0 )
			createD3DTexture( );
			
		HRESULT result;
		
		if( p_MasterPalette )
		{
			result = DirectX_Device->SetPalette( texUnit , p_MasterPalette );
			ASSERT( result == D3D_OK );
		}

		result = DirectX_Device->SetTexture( texUnit, p_D3DTexture );
		ASSERT( result == D3D_OK );

		contextChangeCount++;
	}
}

/// unmark this image for drawing operations.
/// this is rarely called on the ps2.
void PlatformImage::Unselect( void )
{

}


u32 PlatformImage::GetBPP( u16 format )
{
	switch( format & Image::SIZE_MASK )
	{
	case Image::SIZE_4_BIT:
		return 4;
	case Image::SIZE_8_BIT:
		return 8;
	case Image::SIZE_16_BIT:
		return 16;
	case Image::SIZE_24_BIT:
		return 24;
	case Image::SIZE_32_BIT:
		return 32;
	default:
		ASSERTS( false, "Unknown Image Depth" );
		return 32;
	}
}

void PlatformImage::correctPaletteAlpha( void *pPalette, const unsigned int entries )
{
	PALETTEENTRY * pPal = (PALETTEENTRY *) pPalette;

	for( unsigned int i = 0; i < entries; i++, pPal++ )
		if( pPal->peFlags != 0 )
			pPal->peFlags = (pPal->peFlags * 2) - 1;
}

void PlatformImage::convertPalette( void *pNewPalette, const void * pOldPalette, const unsigned int entries )
{
	memcpy( pNewPalette, pOldPalette, entries * 4);

	PALETTEENTRY * pPal = (PALETTEENTRY *) pNewPalette;

	for( unsigned int i = 0; i < entries; i++, pPal++ )
	{
		unsigned char temp = pPal->peBlue;

		pPal->peBlue = pPal->peRed;
		pPal->peRed = temp;

		if( pPal->peFlags != 0 )
			pPal->peFlags = (pPal->peFlags * 2) - 1;
	}
}

void PlatformImage::convertPalettedImage4to8( void * pNewImage, const void * pOldImage, const unsigned int width, const unsigned int height )
{
	char * pOut = (char *) pNewImage;
	char * pIn = (char *) pOldImage;

	for( unsigned int i = 0; i < ((width * height) >> 1); i++, pIn++, pOut += 2 )
	{
		pOut[0] = *pIn & 0x0f;
		pOut[1] = (*pIn >> 4) & 0x0f;
	}

}

void PlatformImage::convertPalettedImage8to8( void * pNewImage, const void * pOldImage, const unsigned int width, const unsigned int height )
{
	unsigned char * pOldIndices = (unsigned char *) pOldImage;
	unsigned char * pNewIndices = (unsigned char *) pNewImage;

	for( unsigned int i = 0; i < (width * height); i++)
		pNewIndices[i] = PS2PaletteSwizzle[ pOldIndices[i] ];
}

void PlatformImage::convertImageRGBAtoARGB( void * pNewImage, const void * pOldImage, const unsigned int width, const unsigned int height )
{
	typedef struct { unsigned char r, g, b, a; } _rgbaPS2;
	typedef struct { unsigned char b, g, r, a; } _argbDWORD;

	_rgbaPS2 * pInPixels = (_rgbaPS2 *) pOldImage;
	_argbDWORD * pOutPixels = (_argbDWORD *) pNewImage;

	for( unsigned int i = 0; i < (width * height); i++ )
	{
		pOutPixels->r = pInPixels->r;
		pOutPixels->g = pInPixels->g;
		pOutPixels->b = pInPixels->b;

		pOutPixels->a = (pInPixels->a == 0 ) ? 0 : (pInPixels->a * 2) - 1;

		pInPixels++;
		pOutPixels++;
	}

}

void PlatformImage::convertImageRGBtoXRGB( void * pNewImage, const void * pOldImage, const unsigned int width, const unsigned int height )
{
	typedef struct { unsigned char r, g, b, a; } _rgbaPS2;
	typedef struct { unsigned char b, g, r, x; } _argbDWORD;

	_rgbaPS2 * pInPixels = (_rgbaPS2 *) pOldImage;
	_argbDWORD * pOutPixels = (_argbDWORD *) pNewImage;

	for( unsigned int i = 0; i < (width * height); i++ )
	{
		pOutPixels->r = pInPixels->r;
		pOutPixels->g = pInPixels->g;
		pOutPixels->b = pInPixels->b;

		pInPixels++;
		pOutPixels++;
	}

}

void PlatformImage::convertImagex555to565( void * pNewImage, const void * pOldImage, const unsigned int width, const unsigned int height )
{
	unsigned short * pOldPixels = (unsigned short *) pOldImage;
	unsigned short * pNewPixels = (unsigned short *) pNewImage;

	for( unsigned int i = 0; i < (width * height); i++ )
	{
		unsigned short	r = (*pOldPixels) & 0x1f;
		unsigned short g = (*pOldPixels >>  5) & 0x1f;
		unsigned short b = (*pOldPixels >> 10) & 0x1f;

		*pNewPixels = (b << 11) | (g << 6) | r;

		pOldPixels++;
		pNewPixels++;
	}

}

void PlatformImage::PausingTextureManager( void )
{

}

void PlatformImage::ResumingTextureManager( void )
{

}
