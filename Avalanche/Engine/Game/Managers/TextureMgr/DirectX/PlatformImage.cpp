
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


#include "Game/Managers/TextureMgr/PlatformImage.h"
#include "Display/DirectX/DXPublished.h"

#ifdef _DEBUG
#include "Layers/ttyColors.h"
#endif

bool	PlatformImage::m_ForceDynamic = false;

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
#ifdef MASSIVE_AD
PlatformImage::PlatformImage( Image * _Owner, void * _Data, void * _Palette, bool FromXPR, bool isMassive) :
#else
PlatformImage::PlatformImage( Image * _Owner, void * _Data, void * _Palette, bool FromXPR) :
#endif
	m_Owner( _Owner)
{
	m_Flags = 0;

	b_Dynamic = m_ForceDynamic;

	#ifdef DIRECTX_PC
		p_D3DSurface = 0;
	#endif

	if( m_Owner->GetFlags( ) & (Image::BACK_BUFFER | Image::FRONT_BUFFER | Image::RENDER_TARGET) )
	{
		ASSERT( _Data == 0 );
		ASSERT( _Palette == 0 );
		ASSERT( b_Dynamic == false );

		p_D3DTexture = 0;

		m_MIPLevels = 1;
		m_MaxMIPLevels = 1;
		m_MIPLevelBuffers = 0;

		if( m_Owner->GetFlags( ) & (Image::BACK_BUFFER | Image::FRONT_BUFFER) )
			m_Flags |= SURFACE_COPY;
		else if( m_Owner->GetFlags( ) & Image::RENDER_TARGET )
			m_Flags |= RENDER_TARGET;

#ifdef DIRECTX_PC
		if (m_Owner->GetFlags() & Image::DEPTH_BUFFER)
			m_Flags |= DEPTH_BUFFER;
#endif

		internalizeFormat( m_Owner->GetFormat( ), m_InternalFormat, m_InternalBytesPP, m_PaletteEntries );
		ASSERT( m_PaletteEntries == 0 );

		b_OwnImages = false;
		b_OwnPalettes = false;

		unsigned int m_D3DMIPLevels = 1;

		createD3DTexture( m_D3DMIPLevels );
		ASSERT( m_D3DMIPLevels == 1 );

		if( m_Flags & RENDER_TARGET )
			DXPublished::addLostDeviceCallback( this );
	}
	else
	{
		m_MaxMIPLevels = maxMIPLevels( m_Owner->GetWidth( ), m_Owner->GetHeight( ) );

		m_MIPLevelBuffers = new MIPLevelData [m_MaxMIPLevels];
		ASSERT( m_MIPLevelBuffers );

		for( unsigned int i = 0; i < m_MaxMIPLevels; i++ )
		{
			m_MIPLevelBuffers[i].p_Image = 0;
			m_MIPLevelBuffers[i].p_Palette = 0;
		}

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

		p_D3DTexture = 0;

		void * pNewImage = 0;
		void * pNewPalette = 0;

#ifdef MASSIVE_AD
		internalizeFormat( m_Owner->GetFormat( ), m_InternalFormat, m_InternalBytesPP, m_PaletteEntries, p_ImageConvertor, p_PaletteConvertor, isMassive );
#else
		internalizeFormat( m_Owner->GetFormat( ), m_InternalFormat, m_InternalBytesPP, m_PaletteEntries, p_ImageConvertor, p_PaletteConvertor);
#endif

		if( p_ImageConvertor  ||  !_Data  ||  b_Dynamic )
		{
			void * pNewImage = memAlloc( m_Owner->GetWidth( ) * m_Owner->GetHeight( ) * m_InternalBytesPP );

			if( p_ImageConvertor  &&  _Data )
				(this->*p_ImageConvertor)( pNewImage, _Data, m_Owner->GetWidth( ), m_Owner->GetHeight( ) );

			_Data = pNewImage;
			b_OwnImages = true;
		}
		else
			b_OwnImages = false;

		if( m_PaletteEntries  &&  (p_PaletteConvertor  ||  !_Palette  ||  b_Dynamic) )
		{
			void * pNewPalette = memAlloc( m_PaletteEntries * sizeof( PALETTEENTRY ) );
			ASSERT(pNewPalette);

			if( p_PaletteConvertor  &&  _Palette )
				(this->*p_PaletteConvertor)( pNewPalette, _Palette, m_PaletteEntries );

			_Palette = pNewPalette;
			b_OwnPalettes = true;
		}
		else
			b_OwnPalettes = false;

		m_MIPLevelBuffers[0].p_Image = _Data;
		m_MIPLevelBuffers[0].p_Palette = _Palette;

		m_MasterPalette = (PALETTEENTRY *) _Palette;

		/* success */
		TextureState__Request( 0, TS_TYPE, TS_TYPE_2D );

		m_MIPLevels = 1;
	}

}

#ifdef MASSIVE_AD
void PlatformImage::internalizeFormat( unsigned int ownerFormat, D3DFORMAT & internalFormat, unsigned int & internalBytesPP, unsigned int & internalPaletteEntries, PIImageConvertor &imageConvertor, PIPaletteConvertor &paletteConvertor,bool isMassive )
#else
void PlatformImage::internalizeFormat( unsigned int ownerFormat, D3DFORMAT & internalFormat, unsigned int & internalBytesPP, unsigned int & internalPaletteEntries, PIImageConvertor &imageConvertor, PIPaletteConvertor &paletteConvertor)
#endif
{

	unsigned int imageType = ownerFormat & Image::TYPE_MASK;
	unsigned int imageSize = ownerFormat & Image::SIZE_MASK;

	imageConvertor = 0;
	paletteConvertor = 0;

	internalPaletteEntries = 0;

	switch( imageType )
	{
	case Image::TYPE_RGB:
		switch( imageSize )
		{
		case Image::SIZE_4_BIT:
		case Image::SIZE_8_BIT:
#ifdef DIRECTX_PC
		case Image::SIZE_32_BIT:
			internalFormat = D3DFMT_X8R8G8B8;
			internalBytesPP = 4;
			break;
#else
		case Image::SIZE_32_BIT:
#endif
		default:
			ASSERT( 0 );
			break;
		case Image::SIZE_16_BIT:
			if( b_Dynamic )
			{
				internalFormat = D3DFMT_R8G8B8;
				internalBytesPP = 4;

				imageConvertor = PlatformImage::convertImageRGBtoARGB;
			}
			else
			{
				internalFormat = D3DFMT_R5G6B5;
				internalBytesPP = 2;

				imageConvertor = PlatformImage::convertImagex555to565;
			}

			break;
		case Image::SIZE_24_BIT:
			if( b_Dynamic)
			{
				internalFormat = D3DFMT_R8G8B8; //Is this a bug?
				internalBytesPP = 4;

				imageConvertor = PlatformImage::convertImageRGBtoARGB;
			}
#ifdef MASSIVE_AD
			else if(isMassive )
			{
				internalFormat = D3DFMT_A8R8G8B8;
				internalBytesPP = 4;

				imageConvertor = PlatformImage::convertMassiveImageRGBtoARGB;
			}
#endif
			else
			{
			internalFormat = D3DFMT_R8G8B8;
			internalBytesPP = 4;
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
			if( b_Dynamic )
			{
				internalFormat = D3DFMT_A8R8G8B8;
				internalBytesPP = 4;

				imageConvertor = PlatformImage::convertImage1555to8888;
			}
			else
			{
				internalFormat = D3DFMT_A1R5G5B5;
				internalBytesPP = 2;
			}
			break;
		case Image::SIZE_32_BIT:
			internalFormat = D3DFMT_A8R8G8B8;
			internalBytesPP = 4;

			imageConvertor = PlatformImage::convertImageRGBAtoARGB;
			break;
		}
		break;
	case Image::TYPE_CI:
		switch( imageSize )
		{
		case Image::SIZE_4_BIT:
			internalFormat = D3DFMT_P8;
			internalBytesPP = 1;
			internalPaletteEntries = 256;

			imageConvertor = PlatformImage::convertPalettedImage4to8;
			paletteConvertor = PlatformImage::convertPalette;
			break;
		case Image::SIZE_8_BIT:
			internalFormat = D3DFMT_P8;
			internalBytesPP = 1;
			internalPaletteEntries = 256;

			imageConvertor = PlatformImage::convertPalettedImage8to8;
			paletteConvertor = PlatformImage::convertPalette;
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
		ASSERT( b_Dynamic == false );

		internalFormat = D3DFMT_L8;
		internalBytesPP = 1;

		break;
	case Image::TYPE_IA:
		ASSERT( b_Dynamic == false );

		internalFormat = D3DFMT_A8L8;
		internalBytesPP = 1;

		break;
	}

}

void PlatformImage::internalizeFormat( unsigned int ownerFormat, D3DFORMAT & internalFormat, unsigned int & internalBytesPP, unsigned int & internalPaletteEntries )
{

	unsigned int imageType = ownerFormat & Image::TYPE_MASK;
	unsigned int imageSize = ownerFormat & Image::SIZE_MASK;

	internalPaletteEntries = 0;

	switch( imageType )
	{
	case Image::TYPE_RGB:
		switch( imageSize )
		{
		case Image::SIZE_4_BIT:
		case Image::SIZE_8_BIT:
#ifdef DIRECTX_PC
		case Image::SIZE_32_BIT:
			internalFormat = D3DFMT_X8R8G8B8;
			internalBytesPP = 4;
			break;
#else
		case Image::SIZE_32_BIT:
#endif
		default:
			ASSERT( 0 );
			break;
		case Image::SIZE_16_BIT:
			if( b_Dynamic )
			{
				internalFormat = D3DFMT_R8G8B8;
				internalBytesPP = 4;
			}
			else
			{
				internalFormat = D3DFMT_R5G6B5;
				internalBytesPP = 2;
			}

			break;
		case Image::SIZE_24_BIT:
			if( b_Dynamic )
			{
				internalFormat = D3DFMT_R8G8B8;
				internalBytesPP = 4;
			}
			else
			{
				internalFormat = D3DFMT_R8G8B8;
				internalBytesPP = 4;
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
			if( b_Dynamic )
			{
				internalFormat = D3DFMT_A8R8G8B8;
				internalBytesPP = 4;

			}
			else
			{
				internalFormat = D3DFMT_A1R5G5B5;
				internalBytesPP = 2;
			}
			break;
		case Image::SIZE_32_BIT:
			internalFormat = D3DFMT_A8R8G8B8;
			internalBytesPP = 4;
			break;
		}
		break;
	case Image::TYPE_CI:
		switch( imageSize )
		{
		case Image::SIZE_4_BIT:
			internalFormat = D3DFMT_P8;
			internalBytesPP = 1;
			internalPaletteEntries = 16;
			break;
		case Image::SIZE_8_BIT:
			internalFormat = D3DFMT_P8;
			internalBytesPP = 1;
			internalPaletteEntries = 256;
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
		ASSERT( b_Dynamic == false );

		internalFormat = D3DFMT_L8;
		internalBytesPP = 1;
		break;
	case Image::TYPE_IA:
		ASSERT( b_Dynamic == false );

		internalFormat = D3DFMT_A8L8;
		internalBytesPP = 1;
		break;
	}

}

void PlatformImage::Init( void )
{

}

void PlatformImage::Cleanup( void )
{

}

void PlatformImage::releaseDeviceObjects( void )
{
	if( p_D3DTexture )
	{
		ULONG useCount = p_D3DTexture->Release( );
		ASSERT( useCount == 0 );
		p_D3DTexture = 0;
	}
#ifdef DIRECTX_PC
	if( p_D3DSurface )
	{
		ULONG useCount = p_D3DSurface->Release( );
		ASSERT( useCount == 0 );
		p_D3DSurface = 0;
	}
#endif

}

void PlatformImage::restoreDeviceObjects( void )
{
	ASSERT( p_D3DTexture == 0 );

	unsigned int m_D3DMIPLevels = m_MIPLevels;

	createD3DTexture( m_D3DMIPLevels );
	if( !(m_Flags & (SURFACE_COPY | RENDER_TARGET)) )
		loadD3DTexture( m_D3DMIPLevels );
}


void PlatformImage::createD3DTexture( unsigned int & m_D3DMIPLevels )
{
	ASSERT( p_D3DTexture == 0 );

	D3DPOOL					D3DPool = (b_Dynamic || (m_Flags & RENDER_TARGET)) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
	unsigned int			D3DUsage = m_Flags & RENDER_TARGET ? D3DUSAGE_RENDERTARGET : 0;
	unsigned int			D3DWidth = m_Owner->GetWidth( );
	unsigned int			D3DHeight = m_Owner->GetHeight( );

	m_D3DFormat = m_InternalFormat;

	// </begin> Huge Hack Alert
	// HHH  HHH UU    UU GGGGGGGG EEEEEEEE
	// HHH  HHH UU    UU GG       EE
	// HHHHHHHH UU    UU GG       EEEEE
	// HHH  HHH UU    UU GG    GG EE
	// HHH  HHH UUUUUUUU GGGGGGGG EEEEEEEE
	// ----------- Hack alert! -----------
	if (m_D3DFormat == D3DFMT_P8)
		m_D3DFormat = D3DFMT_A8R8G8B8;
	// </end> Huge Hack Alert

	HRESULT result = D3DXCheckTextureRequirements( DirectX_Device, &D3DWidth, &D3DHeight, & m_D3DMIPLevels, D3DUsage, & m_D3DFormat, D3DPool );
	ASSERT( result == D3D_OK );

	if( m_D3DFormat != D3DFMT_P8 )
		m_MasterPalette = 0;

	if( b_Dynamic )
	{
		ASSERT( (m_Flags & RENDER_TARGET) == 0 );
		D3DUsage |= D3DUSAGE_DYNAMIC;
	}

	result = DirectX_Device->CreateTexture( D3DWidth, D3DHeight, m_MIPLevels, D3DUsage, m_D3DFormat, D3DPool, & p_D3DTexture );
	ASSERT( result == D3D_OK );
	ASSERT( p_D3DTexture );

#ifdef DIRECTX_PC
	// create our Z buffer and stencil buffer for render to
	// texture rendering
	if (m_Flags & RENDER_TARGET && m_Flags & DEPTH_BUFFER)
	{
		result = DirectX_Device->CreateDepthStencilSurface(D3DWidth, D3DHeight, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, & p_D3DSurface);
		ASSERT( result == D3D_OK );
		ASSERT( p_D3DSurface );
	}
#endif;

	m_UScale = ((float) D3DWidth / (float) m_Owner->GetWidth( ));
	m_VScale = ((float) D3DHeight / (float) m_Owner->GetHeight( ));
#ifdef DIRECTX_PC
	m_Owner->imageUScale = 1 / m_UScale;
	m_Owner->imageVScale = 1 / m_VScale;
	m_Owner->realImageHeight = D3DHeight;
	m_Owner->realImageWidth = D3DWidth;
#endif

}

void PlatformImage::loadD3DTexture( unsigned int m_D3DMIPLevels )
{
	ASSERT( p_D3DTexture );
	ASSERT( m_MIPLevelBuffers[0].p_Image );

	bool						bFilter = false;
	HRESULT					result = D3D_OK;

	RECT SrcRect = { 0 };

	SrcRect.bottom = m_Owner->GetHeight( );
	SrcRect.right = m_Owner->GetWidth( );

	unsigned int currentPitch = m_Owner->GetWidth( ) * m_InternalBytesPP;

	for( unsigned int i = 0; i < m_D3DMIPLevels; i++ )
	{
		if( m_MIPLevelBuffers[i].p_Image )
		{
			IDirect3DSurface8 * pSurface = 0;

			result = p_D3DTexture->GetSurfaceLevel( i, & pSurface );
			ASSERT( result == D3D_OK );

			result = D3DXLoadSurfaceFromMemory( pSurface, m_MasterPalette, NULL, m_MIPLevelBuffers[i].p_Image,
							m_InternalFormat, currentPitch, (PALETTEENTRY *) m_MIPLevelBuffers[i].p_Palette, & SrcRect, D3DX_DEFAULT, 0 );
			ASSERT( result == D3D_OK );

			pSurface->Release( );

			currentPitch >>= 1;
			SrcRect.bottom >>= 1;
			SrcRect.right >>= 1;

			if( b_OwnImages  &&  ((b_Dynamic == false)  ||  i ) )
			{
				memFree( m_MIPLevelBuffers[i].p_Image );
				m_MIPLevelBuffers[i].p_Image = 0;
			}

			if( b_OwnPalettes  &&  (b_Dynamic == false)  &&  i )
			{
				memFree( m_MIPLevelBuffers[i].p_Palette );
				m_MIPLevelBuffers[i].p_Palette = 0;
			}

		}
		else
		{
			ASSERT( i == 1 );
			bFilter = true;
		}
	}

	for( i = m_D3DMIPLevels; i < m_MIPLevels; i++ )
	{
		if( b_OwnImages  &&  ((b_Dynamic == false)  ||  i ) )
		{
			memFree( m_MIPLevelBuffers[i].p_Image );
			m_MIPLevelBuffers[i].p_Image = 0;
		}

		if( b_OwnPalettes  &&  (b_Dynamic == false)  &&  i )
		{
			memFree( m_MIPLevelBuffers[i].p_Palette );
			m_MIPLevelBuffers[i].p_Palette = 0;
		}
	}

	m_MIPLevels = m_D3DMIPLevels;

	if( bFilter )
	{
		result = D3DXFilterTexture( p_D3DTexture, NULL, 0, D3DX_DEFAULT );
		ASSERT( result == D3D_OK );
	}


}

void PlatformImage::checkOwnedImages( void )
{
	if( b_OwnImages == false )
	{
		unsigned int width = m_Owner->GetWidth( );
		unsigned int height = m_Owner->GetHeight( );

		for( unsigned int i = 0; i < m_MIPLevels; i++ )
		{
			if( m_MIPLevelBuffers[i].p_Image )
			{
				void * pNewImage = memAlloc( width * height * m_InternalBytesPP );
				ASSERT( pNewImage );

				memcpy( pNewImage, m_MIPLevelBuffers[i].p_Image, width * height * m_InternalBytesPP );

				m_MIPLevelBuffers[i].p_Image = pNewImage;
			}

			width >>= 1;
			height >>= 1;
		}

		b_OwnImages = true;
	}

	if( b_OwnPalettes == false )
	{
		for( unsigned int i = 0; i < m_MIPLevels; i++ )
		{
			if( m_MIPLevelBuffers[i].p_Palette )
			{
				void * pNewPalette = memAlloc( m_PaletteEntries * sizeof( PALETTEENTRY ) );
				ASSERT( pNewPalette );

				memcpy( pNewPalette, m_MIPLevelBuffers[i].p_Palette, m_PaletteEntries * sizeof( PALETTEENTRY ) );

				m_MIPLevelBuffers[i].p_Palette = pNewPalette;
			}
		}

		b_OwnPalettes = true;
	}

}

void PlatformImage::EndMIPLevels( bool _Dynamic )
{
	ASSERT( p_D3DTexture == 0 );

	if( _Dynamic )
	{
		b_Dynamic = true;
		DXPublished::addLostDeviceCallback( this );
		checkOwnedImages( );
	}

	unsigned int m_D3DMIPLevels = m_MIPLevels;

	createD3DTexture( m_D3DMIPLevels );
	loadD3DTexture( m_D3DMIPLevels );
}

void PlatformImage::BitBlt(int x, int y, int w, int h)
{
	// not written yet.
	ASSERT(false);
}

void PlatformImage::BltGtoA(int x, int y, int w, int h)
{
	// not written yet.
	ASSERT(false);
}

/// Make this image available for rendering
void PlatformImage::Lock( void )
{
	// is it already uploaded?
	ASSERT( !(m_Flags & LOCK_OUTSTANDING) );

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
		memcpy( pDstPtr, pSrcPtr, width * m_InternalBytesPP );

		pDstPtr += Rect.Pitch;
		pSrcPtr += width * m_InternalBytesPP;
	}
}

void PlatformImage::LockForRender( void )
{
	/* set up texture as frame buffer. to be a frame buffer, texture must be page aligned */
	ASSERT( m_Flags & LOCK_OUTSTANDING );
	ASSERT( m_Flags & RENDER_TARGET );
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

	// if our viewport to set up is bigger than our screen
	// fix it
#ifdef DIRECTX_PC
	if ( (int)Desc.Width > DisplayState.nScreenWidth || (int)Desc.Height > DisplayState.nScreenHeight )
	{
		D3DVIEWPORT8	renderViewport = { 0, 0, DisplayState.nScreenWidth, DisplayState.nScreenHeight, 0.0, 1.0 };
		result = DirectX_Device->SetViewport( & renderViewport );
	}
	else
	{
		D3DVIEWPORT8	renderViewport = { 0, 0, Desc.Width, Desc.Height, 0.0, 1.0 };
		result = DirectX_Device->SetViewport( & renderViewport );
	}
#else
	D3DVIEWPORT8	renderViewport = { 0, 0, Desc.Width, Desc.Height, 0.0, 1.0 };
	result = DirectX_Device->SetViewport( & renderViewport );
#endif

	ASSERT( result == D3D_OK );
#ifdef DIRECTX_PC
	if (m_Flags & DEPTH_BUFFER)
		result = DirectX_Device->SetRenderTarget( pRTTDstSurface, p_D3DSurface);
	else
		result = DirectX_Device->SetRenderTarget( pRTTDstSurface, NULL );
#else
	result = DirectX_Device->SetRenderTarget( pRTTDstSurface, NULL );
#endif
	ASSERT( result == D3D_OK );
}

void PlatformImage::UnlockForRender( void )
{
	ASSERT( pRTTDstSurface );
	ASSERT( pRTTBackupRTSurface );
	ASSERT( pRTTBackupDSSurface );

	HRESULT result = D3D_OK;

	result = DirectX_Device->SetRenderTarget( pRTTBackupRTSurface, pRTTBackupDSSurface );
	ASSERT( result == D3D_OK );

	pRTTBackupRTSurface->Release( );
	pRTTBackupRTSurface = 0;

	pRTTBackupDSSurface->Release( );
	pRTTBackupDSSurface = 0;

	pRTTDstSurface->Release( );
	pRTTDstSurface = 0;

	result = DirectX_Device->SetViewport( & RTTBackupViewport );
	ASSERT( result == D3D_OK );

}

// reload texture of CPU side modification
void PlatformImage::reloadD3DTexture( void * pImage, void * pPalette )
{
	ASSERT( p_D3DTexture );
	ASSERT( (pPalette == NULL)  ||  (m_MasterPalette) );

	HRESULT result = D3D_OK;
	IDirect3DSurface8 * pSurface = 0;

	result = p_D3DTexture->GetSurfaceLevel( 0, & pSurface );
	ASSERT( result == D3D_OK );
	ASSERT( pSurface );

	RECT SrcRect = { 0 };

	SrcRect.bottom = m_Owner->GetHeight( );
	SrcRect.right = m_Owner->GetWidth( );

	result = D3DXLoadSurfaceFromMemory( pSurface, m_MasterPalette, NULL, pImage, m_D3DFormat,
		m_Owner->GetWidth( ) * m_InternalBytesPP, (PALETTEENTRY *) pPalette, & SrcRect, D3DX_DEFAULT, 0 );
	ASSERT( result == D3D_OK );

	if( m_MIPLevels > 1 )
	{
		result = D3DXFilterTexture( p_D3DTexture, NULL, 0, D3DX_DEFAULT );
		ASSERT( result == D3D_OK );
	}

	pSurface->Release( );

}


/// platform specific function to get a pointer to the image buffer
void * PlatformImage::GetImageBuffer( void )
{
	ASSERT( m_MIPLevels == 1 );
	ASSERT( m_MIPLevelBuffers[0].p_Image );

	if( p_D3DTexture == 0 )
	{
		b_Dynamic = true;

		DXPublished::addLostDeviceCallback( this );

		unsigned int m_D3DMIPLevels = 1;
		createD3DTexture( m_D3DMIPLevels );
		ASSERT( m_D3DMIPLevels == 1 );

		loadD3DTexture( m_D3DMIPLevels );
	}

	ASSERT( p_D3DTexture );
	ASSERT( b_Dynamic );

	ASSERT( !(m_Flags & BUFFER_FETCHED) );

	m_Flags |= BUFFER_FETCHED;

	return m_MIPLevelBuffers[0].p_Image;
}

/// platform specific function to release the image buffer
void PlatformImage::ReleaseImageBuffer( bool modified )
{
	ASSERT( m_MIPLevelBuffers[0].p_Image );
	ASSERT( m_Flags & BUFFER_FETCHED );

	m_Flags &= ~BUFFER_FETCHED;

	if( modified   &&  !(m_Flags & PALETTE_FETCHED) )
		reloadD3DTexture( m_MIPLevelBuffers[0].p_Image, m_MIPLevelBuffers[0].p_Palette );

}

/// platform specific function to get a pointer to the palette buffer
void* PlatformImage::GetPaletteBuffer( void )
{
	ASSERT( p_D3DTexture );
	ASSERT( m_MIPLevelBuffers[0].p_Image );
	ASSERT( m_MIPLevelBuffers[0].p_Palette );
	ASSERT( !(m_Flags & PALETTE_FETCHED) );

	m_Flags |= PALETTE_FETCHED;

	return m_MIPLevelBuffers[0].p_Palette;
}

/// platform specific function to release the palette buffer
void PlatformImage::ReleasePaletteBuffer( bool modified )
{
	ASSERT( m_MIPLevelBuffers[0].p_Image );
	ASSERT( m_MIPLevelBuffers[0].p_Palette );
	ASSERT( m_Flags & PALETTE_FETCHED );

	m_Flags &= ~PALETTE_FETCHED;

	if( modified  &&  !(m_Flags & BUFFER_FETCHED) )
		reloadD3DTexture( m_MIPLevelBuffers[0].p_Image, m_MIPLevelBuffers[0].p_Palette );

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

	if( b_Dynamic  ||  (m_Flags & RENDER_TARGET) )
		DXPublished::removeLostDeviceCallback( this );

	if( m_MIPLevelBuffers )
	{
		for( unsigned int i = 0; i < m_MIPLevels; i++ )
		{
			if( m_MIPLevelBuffers[i].p_Image && b_OwnImages )
			{
				memFree( m_MIPLevelBuffers[i].p_Image );
			}
			m_MIPLevelBuffers[i].p_Image = 0;

			if( m_MIPLevelBuffers[i].p_Palette && b_OwnPalettes )
			{
				memFree( m_MIPLevelBuffers[i].p_Palette );
			}
			m_MIPLevelBuffers[i].p_Palette = 0;
		}

		delete [] m_MIPLevelBuffers;
		m_MIPLevelBuffers = 0;
	}

}

void PlatformImage::SetMipmapDistance( float fDistance )
{
	// I cant see any way to control this under DirectX
}


void PlatformImage::AppendMipLevel( u16 format, u16 width, u16 height, void * _Data, u16 palFmt, u16 palWidth, u16 palHeight, void * _Palette )
{
	ASSERT( _Data );
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

	if( p_ImageConvertor )
	{
		ASSERT( b_OwnImages );

		void * pNewImage = memAlloc( width * height * m_InternalBytesPP );

		(this->*p_ImageConvertor)( pNewImage, _Data, width, height );

		_Data = pNewImage;
	}

	if( p_PaletteConvertor )
	{
		ASSERT( b_OwnPalettes );
		ASSERT( _Palette );

		void * pNewPalette = memAlloc( m_PaletteEntries * sizeof( PALETTEENTRY ) );

		(this->*p_PaletteConvertor)( pNewPalette, _Palette, m_PaletteEntries );

		_Palette = pNewPalette;
	}

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

	if( p_D3DTexture )
	{
		ULONG useCount = p_D3DTexture->Release( );
		ASSERT( useCount == 0 );

		p_D3DTexture = 0;
	}
#ifdef DIRECTX_PC
	if( p_D3DSurface )
	{
		ULONG useCount = p_D3DSurface->Release( );
		ASSERT( useCount == 0 );

		p_D3DTexture = NULL;
	}
#endif
}

/// prep this image for drawing operations
/// this is rarely called on the ps2
void PlatformImage::Select( void )
{
	int texUnit = TextureState__ActiveUnit( );

	if( TextureState__Get( texUnit, TS_TYPE ) != TS_TYPE_NONE )
	{
		ASSERT( p_D3DTexture );

		HRESULT result;

		if( m_MasterPalette )
		{
			result = DirectX_Device->SetPaletteEntries( 0 , m_MasterPalette );
			ASSERT( result == D3D_OK );

			result = DirectX_Device->SetCurrentTexturePalette( 0 );
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
#if 0
	TRACE("%x Converting palette of %d entries (%x -> %x).\n", this, entries, pOldPalette, pNewPalette);
#endif
	memcpy( pNewPalette, pOldPalette, entries * sizeof(PALETTEENTRY));

	PALETTEENTRY * pPal = (PALETTEENTRY *) pNewPalette;

	for( unsigned int i = 0; i < entries; i++, pPal++ )
	{
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

void PlatformImage::convertImagex555to565( void * pNewImage, const void * pOldImage, const unsigned int width, const unsigned int height )
{
	unsigned short * pInPixels = (unsigned short *) pOldImage;
	unsigned short * pOutPixels = (unsigned short *) pNewImage;

	for( unsigned int i = 0; i < (width * height); i++ )
	{
		unsigned short	r = (*pInPixels) & 0x1f;
		unsigned short g = (*pInPixels >>  5) & 0x1f;
		unsigned short b = (*pInPixels >> 10) & 0x1f;

		*pOutPixels = (r << 11) | (g << 6) | b;

		pInPixels++;
		pOutPixels++;
	}

}
#ifdef MASSIVE_AD
void PlatformImage::convertMassiveImageRGBtoARGB( void * pNewImage, const void * pOldImage, const unsigned int width, const unsigned int height )
{
	typedef struct { unsigned char r, g, b; } _rgbPS2;
	typedef struct { unsigned char b, g, r, a; } _argbDWORD;

	_rgbPS2 * pInPixels = (_rgbPS2 *) pOldImage+(width*(height-1));
	_argbDWORD * pOutPixels = (_argbDWORD *) pNewImage;

	for( int i = (height-1); i >= 0; i-- )
	{
		pInPixels = (_rgbPS2 *) pOldImage+(width*i);
		for( unsigned int j = 0; j < width; j++ )
		{
			pOutPixels->r = pInPixels->r;
			pOutPixels->g = pInPixels->g;
			pOutPixels->b = pInPixels->b;

			pOutPixels->a = 255;

			pInPixels++;
			pOutPixels++;
		}

	}

}
#endif
void PlatformImage::convertImageRGBtoARGB( void * pNewImage, const void * pOldImage, const unsigned int width, const unsigned int height )
{
	typedef struct { unsigned char r, g, b; } _rgbPS2;
	typedef struct { unsigned char b, g, r, a; } _argbDWORD;

	_rgbPS2 * pInPixels = (_rgbPS2 *) pOldImage;
	_argbDWORD * pOutPixels = (_argbDWORD *) pNewImage;

	for( unsigned int i = 0; i < (width * height); i++ )
	{
		pOutPixels->r = pInPixels->r;
		pOutPixels->g = pInPixels->g;
		pOutPixels->b = pInPixels->b;

		pOutPixels->a = 255;

		pInPixels++;
		pOutPixels++;
	}

}

void PlatformImage::convertImage565toARGB( void * pNewImage, const void * pOldImage, const unsigned int width, const unsigned int height )
{
	typedef struct { unsigned char b, g, r, a; } _argbDWORD;

	unsigned short * pInPixels = (unsigned short *) pOldImage;
	_argbDWORD * pOutPixels = (_argbDWORD *) pNewImage;

	for( unsigned int i = 0; i < (width * height); i++ )
	{
		pOutPixels->r = (*pInPixels) & 0x1f;
		pOutPixels->g = (*pInPixels >>  5) & 0x3f;
		pOutPixels->b = (*pInPixels >> 11) & 0x1f;
		pOutPixels->a = 255;

		pInPixels++;
		pOutPixels++;
	}

}

void PlatformImage::convertImage1555to8888( void * pNewImage, const void * pOldImage, const unsigned int width, const unsigned int height )
{
	typedef struct { unsigned char b, g, r, a; } _argbDWORD;

	unsigned short * pInPixels = (unsigned short *) pOldImage;
	_argbDWORD * pOutPixels = (_argbDWORD *) pNewImage;

	for( unsigned int i = 0; i < (width * height); i++ )
	{
		pOutPixels->r = (*pInPixels) & 0x1f;
		pOutPixels->g = (*pInPixels >>  5) & 0x1f;
		pOutPixels->b = (*pInPixels >> 10) & 0x1f;

		pOutPixels->a = *pInPixels & 0x8000 ? 255 : 0;

		pInPixels++;
		pOutPixels++;
	}

}


void PlatformImage::PausingTextureManager( void )
{

}

void PlatformImage::ResumingTextureManager( void )
{

}
