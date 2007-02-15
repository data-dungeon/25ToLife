/***********************************/
/*                                 */
/* TextureUtil.cpp                 */
/*										     */
/* Paul Blagay  avalanche software */
/* Texture Manipulation Library	  */
/*                                 */
/***********************************/

// includes
#include "Display/DisplayPCH.h"
#include "Display/TextureUtil.h"
#include "Display/ColorConversions.h"
#include "EngineHelper/filestream.h"
#include "Game/Managers/TextureMgr/Image.h"

// defines
#define	DIV5BITS	(255.0f / 31.0f)
#define	DIV6BITS (255.0f / 63.0f)

#define TEXTUREUTIL_CLAMP( value, min, max ) ( (value) <= (min) ? (min) : (value) >= (max) ? (max) : (value) )

// vars
static	int			TextureUtil_BlendMode = TEXTUREUTIL_REPLACE;
static	int			TextureUtil_AlphaMode = TEXTUREUTIL_NO_ALPHA;
static	ts_bRGBA		TextureUtil_TransparencyColor;
static	bool			TextureUtil_CutoutOn = 0;
static	float			TextureUtil_Gamma = 0.0f;

static	float			TextureUtil_GammaTable[256];
bool		g_GammaOnLoad = false;

#if defined (DIRECTX)
D3DGAMMARAMP			DXGammaRamp;
#endif

//----------------------------------------------------------------------------------
// Color Format table.. If a new color format is needed, just add it to the table
//----------------------------------------------------------------------------------

#define MAX_PIXEL_FORMATS	8

ts_PixelFormats	PixelFormatLookup[ MAX_PIXEL_FORMATS + 1] = 
{
	//      Format               Pal   bpp     red       green        blue        alpha    shifts (rgba)
	{ SIZE_32_BIT | TYPE_RGBA , FALSE, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,  0,  8, 16, 24 },		// RGBA_8888
	{ SIZE_24_BIT | TYPE_RGB  , FALSE, 24, 0x000000FF, 0x0000FF00, 0x00FF0000,         -1,  0,  8, 16, -1 },		// RGB_888
	{ SIZE_16_BIT | TYPE_RGB  , FALSE, 16,     0x001F,     0x03E0,     0x7C00,     0x8000,  0,  5, 10, 15 },		// RGB_555
//	{ SIZE_16_BIT | TYPE_RGB  , FALSE, 16,     0x001F,     0x07E0,     0xF800,         -1,  0,  5, 11, -1 },		// RGB_565
	{ SIZE_16_BIT | TYPE_RGBA , FALSE, 16,     0x7C00,     0x03E0,     0x001F,     0x8000, 10,  5,  0, 15 },		// RGBA_1555
	{ SIZE_S3TC               , FALSE, 16,        0x0,        0x0,        0x0,        0x0, 15, 10,  5,  0 },		// S3TC compressed
	{ SIZE_8_BIT  | TYPE_CI   , TRUE , 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,  0,  8, 16, 24 },		// INDEXED_8
	{ SIZE_4_BIT  | TYPE_CI   , TRUE , 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,  0,  8, 16, 24 },		// INDEXED_4

	{ -1, FALSE, -1, -1, -1, -1, -1 }					// UNDEFINED FORMAT
};

//----------------------------------------------------------------------------------

static unsigned char TextureUtil_PS2PaletteSwizzle[256] =
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

//----------------------------------------------------------------------------------
// CLASS TO ACCESS PIXEL DATA
//----------------------------------------------------------------------------------
class PixelAccess
{
public:
	PixelAccess( Image* image )
	{
		// find format
		int i;
		for ( i = 0 ; i < MAX_PIXEL_FORMATS ; i++ )
			if ( PixelFormatLookup[ i ].Format == image->GetFormat() )
				break;
		if ( PixelFormatLookup[ i ].Format == -1 )
		{
			ASSERTS( false, "Unsupported pixel format" );
			memset( this, 0, sizeof(*this) );
		}

		// get format information
		d_image   = image;
		d_format  = image->GetFormat();
		d_data    = image->GetImageBuffer();

		// dont get these if its an RGB/A texture... bad, it leaves an allocated palette pointer
		// so later on it thinks it now has a palette and tries to DMA garbage to VRAM
		// plus *possibly* leaving leaked memory around
		if ( (d_format & TYPE_MASK) == TYPE_CI)
		{
			d_palette = image->GetPaletteBuffer();
			d_paletteSize = image->GetPaletteWidth();
		}
		else
		{
			d_palette = 0;
			d_paletteSize = 0;
		}

		d_width  = image->GetWidth();
		d_height = image->GetHeight();
		d_maskR  = PixelFormatLookup[ i ].RedMask;
		d_maskG  = PixelFormatLookup[ i ].GreenMask;
		d_maskB  = PixelFormatLookup[ i ].BlueMask;
		d_maskA  = PixelFormatLookup[ i ].AlphaMask;
		d_shiftR = PixelFormatLookup[ i ].RedShift;
		d_shiftG = PixelFormatLookup[ i ].GreenShift;
		d_shiftB = PixelFormatLookup[ i ].BlueShift;
		d_shiftA = PixelFormatLookup[ i ].AlphaShift;
		d_bytesPerPixel = PixelFormatLookup[ i ].Bpp / 8;
	}

	~PixelAccess()
	{
		if ( d_image && d_data )
			d_image->ReleaseImageBuffer();

		if ( d_image && d_palette)
			d_image->ReleasePaletteBuffer();
	}

	ts_bRGBA GetPaletteEntry(int index)
	{
		ASSERT_PTR(d_palette);
		ASSERT ( index >= 0 && index < d_paletteSize );
		ASSERT( ( d_format & TYPE_MASK ) == TYPE_CI);
	
		ts_bRGBA	c;

		switch ( d_bytesPerPixel )
		{
			case	4:
				{
					u32 pixel = *( (u32*) ( (int)d_palette + (d_bytesPerPixel * index ) ) );
					c.R = (u8) ( ( pixel & d_maskR ) >> d_shiftR );
					c.G = (u8) ( ( pixel & d_maskG ) >> d_shiftG );
					c.B = (u8) ( ( pixel & d_maskB ) >> d_shiftB );
					c.A = (u8) ( ( pixel & d_maskA ) >> d_shiftA );
				}
				break;
			case 3:
				{
					u8* data = (u8*) ( (int)d_palette + (d_bytesPerPixel * index ) );
					c.R = data[ 0 ];
					c.G = data[ 1 ];
					c.B = data[ 2 ];
					c.A = 0;
				}
				break;

			case 2:
				{
					u16 pixel = *( (u16*) ( (int)d_palette + (d_bytesPerPixel * index) ) );
					c.R = (u8) ( (float)( ( pixel & d_maskR ) >> d_shiftR ) * DIV5BITS );
					c.G = (u8) ( (float)( ( pixel & d_maskG ) >> d_shiftG ) * DIV5BITS );
					c.B = (u8) ( (float)( ( pixel & d_maskB ) >> d_shiftB ) * DIV5BITS );
					c.A = ( d_shiftA >= 0 && ( pixel & d_maskA ) ) ? 255 : 0;
				}
				break;

			default:
				ASSERTS( false, "unsupported pixel depth" );

		}
		return	c;
	}

	void PutPaletteEntry( int index, ts_bRGBA c )
	{
		ASSERT_PTR(d_palette);
		ASSERT ( index >= 0 && index < d_paletteSize );
		ASSERT( ( d_format & TYPE_MASK ) == TYPE_CI);
	
		switch ( d_bytesPerPixel )
		{
			case 4:
				{
					u32* data = (u32*) ( (int)d_palette + (d_bytesPerPixel * index) );
					*data = ( c.R << d_shiftR ) | ( c.G << d_shiftG ) | ( c.B << d_shiftB ) | ( c.A << d_shiftA );
				}
				break;

			case 3:
				{
					u8* data = (u8*) ( (int)d_palette + (d_bytesPerPixel * index ) );
					data[ 0 ] = c.R;
					data[ 1 ] = c.G;
					data[ 2 ] = c.B;
				}
				break;

			case 2:
				{
					u16* data = (u16*) ( (int)d_palette + (d_bytesPerPixel * index) );
					int r = (int) ( (float)c.R * ( 31.0f / 255.0f ) );
					int g = (int) ( (float)c.G * ( 31.0f / 255.0f ) );
					int b = (int) ( (float)c.B * ( 31.0f / 255.0f ) );
					int a = ( d_shiftA >= 0 && c.A >= 0x80 ) ? 1 : 0;
					*data = ( r << d_shiftR ) | ( g << d_shiftG ) | ( b << d_shiftB ) | ( a << d_shiftA );
				}
				break;

			default:
				ASSERTS( false, "unsupported pixel depth" );
		}
	}

	ts_bRGBA GetPixel( int x, int y )
	{
		ASSERT_PTR( d_data );
		ASSERT( x >= 0 && x < d_width );
		ASSERT( y >= 0 && y < d_height );
		ASSERT( ( d_format & TYPE_MASK ) == TYPE_RGB || ( d_format & TYPE_MASK ) == TYPE_RGBA );

		ts_bRGBA c;

		switch ( d_bytesPerPixel )
		{
			case 4:
				{
					u32 pixel = *( (u32*) ( (int)d_data + 4 * ( y * d_width + x ) ) );
					c.R = (u8) ( ( pixel & d_maskR ) >> d_shiftR );
					c.G = (u8) ( ( pixel & d_maskG ) >> d_shiftG );
					c.B = (u8) ( ( pixel & d_maskB ) >> d_shiftB );
					c.A = (u8) ( ( pixel & d_maskA ) >> d_shiftA );
				}
				break;

			case 3:
				{
					u8* data = (u8*) ( (int)d_data + 3 * ( y * d_width + x ) );
					c.R = data[ 0 ];
					c.G = data[ 1 ];
					c.B = data[ 2 ];
					c.A = 0;
				}
				break;

			case 2:
				{
					u16 pixel = *( (u16*) ( (int)d_data + 2 * ( y * d_width + x ) ) );
					c.R = (u8) ( (float)( ( pixel & d_maskR ) >> d_shiftR ) * DIV5BITS );
					c.G = (u8) ( (float)( ( pixel & d_maskG ) >> d_shiftG ) * DIV5BITS );
					c.B = (u8) ( (float)( ( pixel & d_maskB ) >> d_shiftB ) * DIV5BITS );
					c.A = ( d_shiftA >= 0 && ( pixel & d_maskA ) ) ? 255 : 0;
				}
				break;

			default:
				ASSERTS( false, "unsupported pixel depth" );
		}

		return c;
	}

	void PutPixel( int x, int y, ts_bRGBA c )
	{
		ASSERT_PTR( d_data );
		ASSERT( x >= 0 && x < d_width );
		ASSERT( y >= 0 && y < d_height );
		ASSERT( ( d_format & TYPE_MASK ) == TYPE_RGB || ( d_format & TYPE_MASK ) == TYPE_RGBA );

		switch ( d_bytesPerPixel )
		{
			case 4:
				{
					u32* data = (u32*) ( (int)d_data + 4 * ( y * d_width + x ) );
					*data = ( c.R << d_shiftR ) | ( c.G << d_shiftG ) | ( c.B << d_shiftB ) | ( c.A << d_shiftA );
				}
				break;

			case 3:
				{
					u8* data = (u8*) ( (int)d_data + 3 * ( y * d_width + x ) );
					data[ 0 ] = c.R;
					data[ 1 ] = c.G;
					data[ 2 ] = c.B;
				}
				break;

			case 2:
				{
					u16* data = (u16*) ( (int)d_data + 2 * ( y * d_width + x ) );
					int r = (int) ( (float)c.R * ( 31.0f / 255.0f ) );
					int g = (int) ( (float)c.G * ( 31.0f / 255.0f ) );
					int b = (int) ( (float)c.B * ( 31.0f / 255.0f ) );
					int a = ( d_shiftA >= 0 && c.A >= 0x80 ) ? 1 : 0;
					*data = ( r << d_shiftR ) | ( g << d_shiftG ) | ( b << d_shiftB ) | ( a << d_shiftA );
				}
				break;

			default:
				ASSERTS( false, "unsupported pixel depth" );
		}
	}

private:
	Image*	d_image;
	void*		d_data;
	void*		d_palette;
	int		d_paletteSize;
	int		d_width;
	int		d_height;
	int		d_format;
	int		d_maskR;
	int		d_maskG;
	int		d_maskB;
	int		d_maskA;
	int		d_shiftR;
	int		d_shiftG;
	int		d_shiftB;
	int		d_shiftA;
	int		d_bytesPerPixel;
};

//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// FUNCTIONS
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
bool	TextureUtil::GetGammaOnLoadFlag(void)
{
	return (g_GammaOnLoad);
}

void	TextureUtil::SetGammaOnLoadFlag(bool Flag)
{
	g_GammaOnLoad = Flag;
}

//----------------------------------------------------------------------------------

// load binary file into buffer
char* TextureUtil::LoadBinaryTextureSet ( char* Filename )
{
	FileStream	Stream;
	int			Size;
	int			ReadSize = 0;
	char			*Buffer;

	// open file
	if( !Stream.Open( Filename, FileStream::READ ) ) 
		return false; 
	
	// get size of file
	Size = Stream.Size();

	// create a buffer
	Buffer = new char[Size + 1];

	// read data
	Stream.Read( Buffer, Size );
	Stream.Close();

	return Buffer;
}


//----------------------------------------------------------------------------------

bool TextureUtil::Test ( CActor *actor )
{
#if 0
	ts_bRGBA	color;
	int		index;

	// load test box
	ts_Geometry* geom = g_loadSOM.Geometry( "blood\\temp.dbl" );
	ASSERT_PTR( geom );
	actor->SetGeometry( 10, geom );

	CInstance		t		= *actor->Instance( 10 );
	ts_PatchSOM		*Som	= t.pSOM;
	if (!Som) return false;
	TextureArray	*TA	= Som->pTextureArray;
	if (!TA) return false;
	Texture			*TP	= TA->GetTexture(0);
	if (!TP) return false;
	Image				*IP   = TP->GetImage();
	if (!IP) return false;
	void				*ID;


	// test get palette entry
	void	*PalBuf = IP->GetPaletteBuffer();
	color.R = 100;	color.G = 100;	color.B = 2;	color.A = 255;
	PutPaletteEntry ( IP, PalBuf, 0, color);
	GetPaletteEntry ( IP, PalBuf, 0, &color);
	IP->ReleasePaletteBuffer();
	return (true);

	// test blit Color
	color.R = 100;	color.G = 100;	color.B = 2;	color.A = 255;
//	SetBlendMode (TEXTUREUTIL_SUBTRACTIVE);
//	SetBlendMode (TEXTUREUTIL_MULTIPLICATIVE);
//	SetBlendMode (TEXTUREUTIL_ADDITIVE);
	SetBlendMode (TEXTUREUTIL_REPLACE);
	SetAlphaMode (TEXTUREUTIL_NO_ALPHA);
	BlitColor( IP, color, 32, 128, 32, 32);

	// test blit texture
//	SetBlendMode (TEXTUREUTIL_SUBTRACTIVE);
//	SetBlendMode (TEXTUREUTIL_MULTIPLICATIVE);
//	SetBlendMode (TEXTUREUTIL_ADDITIVE);
	SetBlendMode (TEXTUREUTIL_REPLACE);
	SetAlphaMode (TEXTUREUTIL_NO_ALPHA);
//	BlitTexture( src, IP, 32, 128, 32, 32);

	return true;

	// test get pixel
	ID = IP->GetImageBuffer();
	GetPixel ( IP, ID, 0, 0, &color );
	IP->ReleaseImageBuffer(true);

	// test put pixel
	color.R = 127;	color.G = 127;	color.B = 127;	color.A = 255;
	ID = IP->GetImageBuffer();
	PutPixel ( IP, ID, 0, 0, color );
	IP->ReleaseImageBuffer(true);

	// test get palette index
	ID = IP->GetImageBuffer();
	GetPixelIndex ( IP, ID, 0, 0, &index);
	IP->ReleaseImageBuffer(true);

	// test put palette index
	index = 10;
	ID = IP->GetImageBuffer();
	PutPixelIndex ( IP, ID, 0, 0, index);
	IP->ReleaseImageBuffer(true);
#endif
	return true;
}

//----------------------------------------------------------------------------------
void	TextureUtil::SetBlendMode (int BlendMode)
{
	TextureUtil_BlendMode = BlendMode;
}

//----------------------------------------------------------------------------------
void	TextureUtil::SetGamma( float gamma )
{
	if ( !Math::Equal( TextureUtil_Gamma, gamma ) )
	{
		TextureUtil_Gamma = gamma;

		// create the gamma table
		for ( int i = 0 ; i < 256 ; i++ )
		{
			float v = 255.0f * Math::Pow( (float)i / 255.0f, 1.0f / gamma );
			TextureUtil_GammaTable[ i ] = TEXTUREUTIL_CLAMP( v, 0.0f, 255.0f );
		}
	}
}

#define GAMMA_CENTER 0.5f
#ifdef DIRECTX_PC
#define GAMMA_SCALE 2.0f
#else
#define GAMMA_SCALE  1.5f
#endif
void TextureUtil::SetGammaZeroToOne( float gamma )
{
	gamma -= GAMMA_CENTER;
	gamma *= GAMMA_SCALE;
	gamma += 1.0f;

	SetGamma( gamma );
}

float TextureUtil::GetGammaZeroToOne( void )
{
	float gamma = GetGamma();
	gamma -= 1.0f;
	gamma /= GAMMA_SCALE;
	gamma += GAMMA_CENTER;
	return gamma;
}

//----------------------------------------------------------------------------------
void	TextureUtil::UpdateGamma()
{
	#if defined (_XBOX) || defined (DIRECTX_PC)
	SetDXGammaRamp();
#ifdef DIRECTX_PC
	DirectX_Device->SetGammaRamp( D3DSGR_NO_CALIBRATION, &DXGammaRamp );
#else
	DirectX_Device->SetGammaRamp( D3DSGR_IMMEDIATE, &DXGammaRamp );
#endif
	#endif

#if 0 //rhn
	#if defined (DIRECTX) && !defined (_XBOX)
	if (DirectX_PresentParams.Windowed == false)
	{
		SetDXGammaRamp();
		DirectX_Device->SetGammaRamp( D3DSGR_NO_CALIBRATION, &DXGammaRamp );		// TODO
	}
	#else			

	#endif
#endif
}

//----------------------------------------------------------------------------------
// Set Gamma Ramp for Xbox
#if defined (DIRECTX) || defined (_XBOX)
void	TextureUtil::SetDXGammaRamp()
{
	float*	ptr = GetGammaTablePtr();
#ifdef DIRECTX_PC
	int multiplier = (int)(128.0f * TextureUtil_Gamma);
	if (multiplier > 128) multiplier = 128;
	for (int i=0; i<256; i++)
	{
		DXGammaRamp.red[i] = 
		DXGammaRamp.green[i] = 
		DXGammaRamp.blue[i] = (WORD)(i * (TextureUtil_Gamma * multiplier));
	}
#else //!DIRECTX_PC
	for (int i=0; i<256; i++)
	{
		DXGammaRamp.red[i] = (BYTE)ptr[i];
		DXGammaRamp.green[i] = (BYTE)ptr[i];
		DXGammaRamp.blue[i] = (BYTE)ptr[i];
	}
#endif //DIRECTX_PC
}
#endif

//----------------------------------------------------------------------------------
float* TextureUtil::GetGammaTablePtr()
{
	return(&TextureUtil_GammaTable[0]);
}

//----------------------------------------------------------------------------------
float	TextureUtil::GetGamma ()
{
	return (TextureUtil_Gamma);
}

//----------------------------------------------------------------------------------
void	TextureUtil::SetAlphaMode (int AlphaMode)
{
	TextureUtil_AlphaMode = AlphaMode;
}

//----------------------------------------------------------------------------------
void	TextureUtil::CutoutOn (bool flag)
{
	TextureUtil_CutoutOn = flag;
}

//----------------------------------------------------------------------------------
void	TextureUtil::SetTransparencyColor (ts_bRGBA TransparencyColor)
{
	TextureUtil_TransparencyColor = TransparencyColor;
}

//----------------------------------------------------------------------------------
bool	TextureUtil::ColorsEqual ( ts_bRGBA c1, ts_bRGBA c2)
{
	if ( c1.R != c2.R ) return false;
	if ( c1.G != c2.G ) return false;
	if ( c1.B != c2.B ) return false;
	if ( c1.A != c2.A ) return false;
	return true;
}

//----------------------------------------------------------------------------------

ts_bRGBA TextureUtil::AdditiveBlend( ts_bRGBA c1, ts_bRGBA c2 )
{
	ts_bRGBA c;
	int v;

	v = (int)c1.R + (int)c2.R;  c.R = ( v <= 255 ) ? v : 255;
	v = (int)c1.G + (int)c2.G;  c.G = ( v <= 255 ) ? v : 255;
	v = (int)c1.B + (int)c2.B;  c.B = ( v <= 255 ) ? v : 255;

	// alpha
	switch (TextureUtil_AlphaMode)
	{
	case	TEXTUREUTIL_NO_ALPHA:
	case	TEXTUREUTIL_DST_ALPHA:
	default:
		c.A = c1.A;
		break;
	case	TEXTUREUTIL_SRC_ALPHA:
		c.A = c2.A;
		break;
	case	TEXTUREUTIL_BLEND_ALPHA:
		v = ( (int)c1.A + (int)c2.A ) / 2;
		c.A = v;
		break;
	}

	return c;
}

//----------------------------------------------------------------------------------

ts_bRGBA TextureUtil::SubtractiveBlend( ts_bRGBA c1, ts_bRGBA c2 )
{
	ts_bRGBA c;
	int v;

	v = (int)c1.R - (int)c2.R;  c.R = ( v >= 0 ) ? v : 0;
	v = (int)c1.G - (int)c2.G;  c.G = ( v >= 0 ) ? v : 0;
	v = (int)c1.B - (int)c2.B;  c.B = ( v >= 0 ) ? v : 0;

	// alpha
	switch (TextureUtil_AlphaMode)
	{
	case	TEXTUREUTIL_NO_ALPHA:
	case	TEXTUREUTIL_DST_ALPHA:
	default:
		c.A = c1.A;
		break;
	case	TEXTUREUTIL_SRC_ALPHA:
		c.A = c2.A;
		break;
	case	TEXTUREUTIL_BLEND_ALPHA:
		v = ( (int)c1.A + (int)c2.A ) / 2;
		c.A = v;
		break;
	}

	return c;
}

//----------------------------------------------------------------------------------

ts_bRGBA TextureUtil::MultiplicativeBlend( ts_bRGBA c1, ts_bRGBA c2 )
{
	ts_bRGBA c;
	int v;

	v = (int)c1.R * (int)c2.R;  c.R = v / 255;
	v = (int)c1.G * (int)c2.G;  c.G = v / 255;
	v = (int)c1.B * (int)c2.B;  c.B = v / 255;

	// alpha
	switch (TextureUtil_AlphaMode)
	{
	case	TEXTUREUTIL_NO_ALPHA:
	case	TEXTUREUTIL_DST_ALPHA:
	default:
		c.A = c1.A;
		break;
	case	TEXTUREUTIL_SRC_ALPHA:
		c.A = c2.A;
		break;
	case	TEXTUREUTIL_BLEND_ALPHA:
		v = ( (int)c1.A + (int)c2.A ) / 2;
		c.A = v;
		break;
	}

	return c;
}

//----------------------------------------------------------------------------------

ts_bRGBA TextureUtil::RGBtoSepia( ts_bRGBA color )
{
	float R,  G,  B;
	float Y, Cr, Cb;

	RGBtoYCrCb( color.R, color.G, color.B, &Y, &Cr, &Cb );
	Y *= ( 1.0f / 255.0f );
	R = Y * 245.0f;
	G = Y * 191.0f;
	B = Y * 137.0f;

	color.R = (u8) TEXTUREUTIL_CLAMP( (int)R, 0, 255 );
	color.G = (u8) TEXTUREUTIL_CLAMP( (int)G, 0, 255 );
	color.B = (u8) TEXTUREUTIL_CLAMP( (int)B, 0, 255 );

	return color;
}

//----------------------------------------------------------------------------------

ts_bRGBA	TextureUtil::AdjustGamma( ts_bRGBA Color, bool monochromeScale )
{
	if ( ! monochromeScale )
	{
		Color.R = (u8) TextureUtil_GammaTable[ Color.R ];
		Color.G = (u8) TextureUtil_GammaTable[ Color.G ];
		Color.B = (u8) TextureUtil_GammaTable[ Color.B ];
	}
	else
	{
		float Y, Cr, Cb;
		float R,  G,  B;
		RGBtoYCrCb( Color.R, Color.G, Color.B, &Y, &Cr, &Cb );
		Y = TextureUtil_GammaTable[ (int) Y ];
		YCrCbtoRGB( Y, Cr, Cb, &R, &G, &B );
		Color.R = (u8) R;
		Color.G = (u8) G;
		Color.B = (u8) B;
	}

	return Color;
}

//----------------------------------------------------------------------------------
#if 0
bool TextureUtil::BlitTexture ( char*  IP_Src,
										  Image* IP_Dst,
										  int x,
										  int y,
										  int width,
										  int height)
{
	ASSERTS( false, "IS THIS USED?" );

	ts_bRGBA		color;
	ts_bRGBA		PixelColor;
	int			c1, c2, newcolor;
	bool			BlitPixel = true;

	// some simple bounds checking
	ASSERTS ( (width  + x) <= IP_Dst->GetWidth(),  "Exceeded Destination Texture Width");
	ASSERTS ( (height + y) <= IP_Dst->GetHeight(), "Exceeded Destination Texture Height");

	// get imagebuffers of src & dst
	void *Dst = IP_Dst->GetImageBuffer();

	// blit baby blit
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			switch (TextureUtil_BlendMode)
			{
			case	TEXTUREUTIL_REPLACE:

				// get pixel from source
				GetPixel	( IP_Src, j, i, IP_Dst->GetWidth(), IP_Dst->GetHeight(), IP_Dst->GetFormat(), &PixelColor );				

				if (TextureUtil_CutoutOn)
				{
					// put pixel in dest only if its not the cutout color
					if ( !ColorsEqual( PixelColor, TextureUtil_TransparencyColor))
						PutPixel( IP_Dst, Dst, x + j, y + i, PixelColor );
				}
				else	// cutouts off - always blit
					PutPixel( IP_Dst, Dst, x + j, y + i, PixelColor );
					break;
			
			case	TEXTUREUTIL_ADDITIVE:
			case	TEXTUREUTIL_SUBTRACTIVE:
			case	TEXTUREUTIL_MULTIPLICATIVE:

				// get source pixel
				GetPixel	( IP_Src, j, i,IP_Dst->GetWidth(), IP_Dst->GetHeight(), IP_Dst->GetFormat(), &PixelColor );				

				if (TextureUtil_CutoutOn)
				{
					// put pixel in dest only if its not the cutout color
					if ( !ColorsEqual( PixelColor, TextureUtil_TransparencyColor))
						BlitPixel = true;
					else
						BlitPixel = false;
				}
				else	// cutouts off - always blit
					BlitPixel = true;

				c1 = TS_RGBA_TO_RGBA8888 (PixelColor);
				c2 = TS_RGBA_TO_RGBA8888 (color);

				// blend the 2 and put back
				if (TextureUtil_BlendMode == TEXTUREUTIL_ADDITIVE)
					newcolor = AdditiveBlend_RGBA8888(c1, c2);

				if (TextureUtil_BlendMode == TEXTUREUTIL_SUBTRACTIVE)
					newcolor = SubtractiveBlend_RGBA8888(c1, c2);

				if (TextureUtil_BlendMode == TEXTUREUTIL_MULTIPLICATIVE)
					newcolor = MultiplicativeBlend_RGBA8888(c1, c2);

				// convert back to color
				RGBA8888_TO_TS_RGBA( newcolor, PixelColor);
	         
				// put the pixel back
				if (BlitPixel)
					PutPixel( IP_Dst, Dst, x + j, y + i, PixelColor );
				break;

			default:
					ASSERTS (false, "Blend Mode Not Supported");
					break;
			}
		}
	}

	IP_Dst->ReleaseImageBuffer(true);

	return true;
}
#endif		
//----------------------------------------------------------------------------------
#if 0
bool TextureUtil::BlitTexture ( char* IP_Src,
										  char* IP_Dst,
										  int x,
										  int y,
										  int	blit_w,
										  int blit_h,
										  int dst_w,
										  int dst_h,
										  int format)
{
	ASSERTS( false, "IS THIS USED?" );

	ts_bRGBA		color;
	ts_bRGBA		PixelColor;
	int			c1, c2, newcolor;
	bool			BlitPixel = true;

	// some simple bounds checking
	ASSERTS ( (blit_w + x) <= dst_w, "Exceeded Destination Texture Width");
	ASSERTS ( (blit_h + y) <= dst_h, "Exceeded Destination Texture Height");

	// get imagebuffers of src & dst
	char *Dst = IP_Dst;

	// blit baby blit
	for (int i = 0; i < blit_h; i++)
	{
		for (int j = 0; j < blit_w; j++)
		{
			switch (TextureUtil_BlendMode)
			{
			case	TEXTUREUTIL_REPLACE:

				// get pixel from source
				GetPixel	( IP_Src, j, i, dst_w, dst_h, format, &PixelColor );				

				if (TextureUtil_CutoutOn)
				{
					// put pixel in dest only if its not the cutout color
					if ( !ColorsEqual( PixelColor, TextureUtil_TransparencyColor))
						PutPixel( Dst, x + j, y + i, dst_w, dst_h, format, PixelColor );
				}
				else	// cutouts off - always blit
					PutPixel( Dst, x + j, y + i, dst_w, dst_h, format, PixelColor );
					break;
			
			case	TEXTUREUTIL_ADDITIVE:
			case	TEXTUREUTIL_SUBTRACTIVE:
			case	TEXTUREUTIL_MULTIPLICATIVE:

				// get source pixel
				GetPixel	( IP_Src, j, i, dst_w, dst_h, format, &PixelColor );				

				if (TextureUtil_CutoutOn)
				{
					// put pixel in dest only if its not the cutout color
					if ( !ColorsEqual( PixelColor, TextureUtil_TransparencyColor))
						BlitPixel = true;
					else
						BlitPixel = false;
				}
				else	// cutouts off - always blit
					BlitPixel = true;

				c1 = TS_RGBA_TO_RGBA8888 (PixelColor);
				c2 = TS_RGBA_TO_RGBA8888 (color);

				// blend the 2 and put back
				if (TextureUtil_BlendMode == TEXTUREUTIL_ADDITIVE)
					newcolor = AdditiveBlend_RGBA8888(c1, c2);

				if (TextureUtil_BlendMode == TEXTUREUTIL_SUBTRACTIVE)
					newcolor = SubtractiveBlend_RGBA8888(c1, c2);

				if (TextureUtil_BlendMode == TEXTUREUTIL_MULTIPLICATIVE)
					newcolor = MultiplicativeBlend_RGBA8888(c1, c2);

				// convert back to color
				RGBA8888_TO_TS_RGBA( newcolor, PixelColor);
	         
				// put the pixel back
				if (BlitPixel)
					PutPixel( Dst, x + j, y + i, dst_w, dst_h, format, PixelColor );
				break;

			default:
					ASSERTS (false, "Blend Mode Not Supported");
					break;
			}
		}
	}
	return true;
}
#endif
//----------------------------------------------------------------------------------
bool TextureUtil::BlitTexture ( Image* IP_Src,
										  Image* IP_Dst,
										  int x,
										  int y,
										  int	blit_w,
										  int blit_h )
{
	ts_bRGBA		srcColor;
	ts_bRGBA		dstColor;
	bool			BlitPixel = true;

	// some simple bounds checking
	ASSERTS ( (blit_w + x) <= IP_Dst->GetWidth(), "Exceeded Destination Texture Width");
	ASSERTS ( (blit_h + y) <= IP_Dst->GetHeight(), "Exceeded Destination Texture Height");

	// get imagebuffers of src & dst
	PixelAccess src( IP_Src );
	PixelAccess dst( IP_Dst );

	// blit baby blit
	for (int i = 0; i < blit_h; i++)
	{
		for (int j = 0; j < blit_w; j++)
		{
			switch (TextureUtil_BlendMode)
			{
			case	TEXTUREUTIL_REPLACE:
				srcColor = src.GetPixel( j, i );
				if ( !TextureUtil_CutoutOn || !ColorsEqual( srcColor, TextureUtil_TransparencyColor ) )
					dst.PutPixel( x + j, y + i, srcColor );
				break;
			
			case	TEXTUREUTIL_ADDITIVE:
				srcColor = src.GetPixel( j, i );
				if ( !TextureUtil_CutoutOn || !ColorsEqual( srcColor, TextureUtil_TransparencyColor ) )
				{
					dstColor = dst.GetPixel( x + j, y + i );
					dstColor = AdditiveBlend( dstColor, srcColor );
					dst.PutPixel( x + j, y + i, dstColor );
				}
				break;

			case	TEXTUREUTIL_SUBTRACTIVE:
				srcColor = src.GetPixel( j, i );
				if ( !TextureUtil_CutoutOn || !ColorsEqual( srcColor, TextureUtil_TransparencyColor ) )
				{
					dstColor = dst.GetPixel( x + j, y + i );
					dstColor = SubtractiveBlend( dstColor, srcColor );
					dst.PutPixel( x + j, y + i, dstColor );
				}
				break;

			case	TEXTUREUTIL_MULTIPLICATIVE:
				srcColor = src.GetPixel( j, i );
				if ( !TextureUtil_CutoutOn || !ColorsEqual( srcColor, TextureUtil_TransparencyColor ) )
				{
					dstColor = dst.GetPixel( x + j, y + i );
					dstColor = MultiplicativeBlend( dstColor, srcColor );
					dst.PutPixel( x + j, y + i, dstColor );
				}
				break;

			default:
				ASSERTS( false, "blend mode not supported" );
				break;
			}
		}
	}

	return true;
}

//----------------------------------------------------------------------------------
ts_bRGBA HSVAdditive( ts_bRGBA dstColor, ts_bRGBA color)
{
	int	h1,s1,v1;
	int	h2,s2,v2;
	ts_bRGBA	newcolor;

	h1 = dstColor.R; s1 = dstColor.G; v1 = dstColor.B;
	h2 = color.R; s2 = color.G; v2 = color.B;

	// add them
	newcolor.R = (h1 + h2) % 360;
	newcolor.G = (s1 + s2); 
	if (newcolor.G > 100) 
		newcolor.G = 100;
	newcolor.B = (v1 + v2); 
	if (newcolor.B > 100)
		newcolor.B = 100;
	newcolor.A = dstColor.A;
   
	return newcolor;
}

//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------

bool TextureUtil::BlitColor ( Image* Image,
									  int x,
									  int y,
									  int width,
									  int height )
{
	ts_bRGBA	c;
	c.R = 255;
	c.G = 255;
	c.B = 255;
	c.A = 255;

	return (BlitColor (Image, c, x, y, width, height ));
}

//----------------------------------------------------------------------------------

bool TextureUtil::BlitColor ( Image* Image,
									  ts_bRGBA color,
									  int x,
									  int y,
									  int width,
									  int height )
{
	ts_bRGBA	dstColor;
	ts_bRGBA	newColor;
	ts_bRGBA	hsvColor;
	float		h,s,v;
	float		r,g,b;
	
	// some simple bounds checking
	ASSERTS( (width  + x) <= Image->GetWidth(),  "Exceeded Destination Texture Width");
	ASSERTS( (height + y) <= Image->GetHeight(), "Exceeded Destination Texture Height");

	// lock
	PixelAccess dst( Image );

	// blit baby blit
	for (int i = 0; i < height; i++)
	{
		// put pixel in dest
		for (int j = 0; j < width; j++)
		{
			switch(TextureUtil_BlendMode)
			{
				case	TEXTUREUTIL_REPLACE:
					dst.PutPixel( x + j, y + i, color );
					break;

				case	TEXTUREUTIL_ADDITIVE:
					dstColor = dst.GetPixel( x + j, y + i );
					dstColor = AdditiveBlend( dstColor, color );
					dst.PutPixel( x + j, y + i, dstColor );
					break;

				case	TEXTUREUTIL_SUBTRACTIVE:
					dstColor = dst.GetPixel( x + j, y + i );
					dstColor = SubtractiveBlend( dstColor, color );
					dst.PutPixel( x + j, y + i, dstColor );
					break;

				case	TEXTUREUTIL_MULTIPLICATIVE:
					dstColor = dst.GetPixel( x + j, y + i );
					dstColor = MultiplicativeBlend( dstColor, color );
					dst.PutPixel( x + j, y + i, dstColor );
					break;

				case	TEXTUREUTIL_GAMMA:
					dstColor = dst.GetPixel( x + j, y + i );
					dstColor = AdjustGamma( dstColor, true );
					dst.PutPixel( x + j, y + i, dstColor );
					break;

				case	TEXTUREUTIL_ADDITIVE_HSV:
					dstColor = dst.GetPixel( x + j, y + i );
					RGBtoHSV(dstColor.R, dstColor.G, dstColor.B, &h,&s,&v);
					hsvColor.R = (u8)h; hsvColor.G = (u8)s; hsvColor.B = (u8)v;
					dstColor = HSVAdditive( hsvColor, color);
					HSVtoRGB(&r, &g, &b, dstColor.R, dstColor.G, dstColor.B);
					newColor.R = (u8)r; newColor.G = (u8)g, newColor.B = (u8)b;
					newColor.A = dstColor.A;
					dst.PutPixel( x + j, y + i, newColor );
					break;

				default:
					ASSERTS( false, "Blend Mode Not Supported" );
					break;
			}
		}
	}

	return true;
}

//----------------------------------------------------------------------------------

#if 0
bool TextureUtil::BlitColor ( char *Image,
									  ts_bRGBA color,
									  int x,
									  int y,
									  int blit_w,
									  int blit_h,
									  int	dst_w,
									  int	dst_h,
									  int	format)
{
	ASSERTS( false, "IS THIS USED?" );

	int		c1,c2,newcolor;
	ts_bRGBA	PixelColor;

	// some simple bounds checking
	ASSERTS ( (blit_w + x) <= dst_w, "Exceeded Destination Texture Width");
	ASSERTS ( (blit_h + y) <= dst_h,  "Exceeded Destination Texture Height");

	// blit baby blit
	for (int i = 0; i < blit_h; i++)
	{
		// put pixel in dest
		for (int j = 0; j < blit_w; j++)
		{
			switch(TextureUtil_BlendMode)
			{
				case	TEXTUREUTIL_REPLACE:
					PutPixel( Image, x + j, y + i, dst_w, dst_h, format, color );
					break;

				case	TEXTUREUTIL_ADDITIVE:
				case	TEXTUREUTIL_SUBTRACTIVE:
				case	TEXTUREUTIL_MULTIPLICATIVE:

					// get source pixel
					GetPixel( Image, x + j, y + i, dst_w, dst_h, format, &PixelColor);
					c1 = TS_RGBA_TO_RGBA8888 (PixelColor);
					c2 = TS_RGBA_TO_RGBA8888 (color);

					// blend the 2 and put back
					if (TextureUtil_BlendMode == TEXTUREUTIL_ADDITIVE)
						newcolor = AdditiveBlend_RGBA8888(c1, c2);

					if (TextureUtil_BlendMode == TEXTUREUTIL_SUBTRACTIVE)
						newcolor = SubtractiveBlend_RGBA8888(c1, c2);

					if (TextureUtil_BlendMode == TEXTUREUTIL_MULTIPLICATIVE)
						newcolor = MultiplicativeBlend_RGBA8888(c1, c2);

					// convert back to color
					RGBA8888_TO_TS_RGBA( newcolor, PixelColor);
      		   
					// put the pixel back
					PutPixel( Image, x + j, y + i, dst_w, dst_h, format, PixelColor );
					break;

				default:
					ASSERTS (false, "Blend Mode Not Supported");
					break;
			}
		}
	}

	return true;
}
#endif

//----------------------------------------------------------------------------------

#if 0
bool TextureUtil::BlitPalettedTexture ( char*  IP_Src, 
													 Image* IP_Dst,
													 int x,
													 int y,
													 int width,
													 int height )
{
	ASSERTS( false, "IS THIS USED?" );

	// some simple bounds checking
	ASSERTS ( (width  + x) <= IP_Dst->GetWidth(),  "Exceeded Destination Texture Width");
	ASSERTS ( (height + y) <= IP_Dst->GetHeight(), "Exceeded Destination Texture Height");

	// lock
	void *Dst = IP_Dst->GetImageBuffer();

	// blit baby blit
	for (int i = 0; i < height; i++)
	{
		// put pixel in dest
		for (int j = 0; j < width; j++)
		{
			int	index;
			GetPixelIndex( IP_Src, j, i, IP_Dst->GetWidth(), IP_Dst->GetHeight(), IP_Dst->GetFormat(), &index);
			PutPixelIndex( IP_Dst, Dst, x + j, y + i, index);
		}
	}

	// unlock
	IP_Dst->ReleaseImageBuffer(true);

	return true;
}
#endif

//----------------------------------------------------------------------------------
#if 0
bool TextureUtil::GetPixel (char* Texture, int x, int y, int dst_w, int dst_h, int format, ts_bRGBA* RGBA)
{
	ASSERTS( false, "IS THIS USED?" );

	u16		PixelFormat;
	int*		Pixel_32;
	short*	Pixel_16;
	char*		Pixel_8;
	int		Pixel;
	int		i;

	// check bounds
	ASSERTS ( x <= dst_w,  "X > Width of Texture" );
	ASSERTS ( y <= dst_h,  "Y > Height of Texture" );

	// Determine format and set up masks to get pixel
	PixelFormat = format;

	ASSERTS (	(PixelFormat != (SIZE_8_BIT | TYPE_CI)) && 
					(PixelFormat != (SIZE_4_BIT | TYPE_CI)), 
					"ERROR: Blitting into a paletted texture" );

	if ( PixelFormat == (SIZE_8_BIT | TYPE_CI) || 
		  PixelFormat == (SIZE_4_BIT | TYPE_CI) )
		return (false);


	// Lookup format in list
	for ( i = 0; i < MAX_PIXEL_FORMATS; i++)
	{
		if ( PixelFormatLookup[i].Format == PixelFormat )
			break;
	}

	// bad format?
	if (PixelFormatLookup[i].Format == -1)
	{
		ASSERTS ( true, "Unsupported Pixel Format");
		return (false);
	}

	// Setup Masks and grab the pixel
	int	RedMask   = PixelFormatLookup[i].RedMask;
	int	GreenMask = PixelFormatLookup[i].GreenMask;
	int	BlueMask  = PixelFormatLookup[i].BlueMask;
	int	AlphaMask = PixelFormatLookup[i].AlphaMask;
	int	RedShift   = PixelFormatLookup[i].RedShift;
	int	GreenShift = PixelFormatLookup[i].GreenShift;
	int	BlueShift  = PixelFormatLookup[i].BlueShift;
	int	AlphaShift = PixelFormatLookup[i].AlphaShift;
	int	bpp = PixelFormatLookup[i].Bpp;

	// calculate index into the texture
	char*	SrcPixel = (char *) Texture;		
	SrcPixel += ( ( x * (bpp / 8) ) + ( ( y * dst_w ) * (bpp / 8) ) );		

	// get the pixel
	switch (bpp)
	{
	// 32 bit pixels
	case	32:
		Pixel_32 = (int *) SrcPixel;
		Pixel = *Pixel_32;
		RGBA->R = (Pixel & RedMask) >> RedShift;
		RGBA->G = (Pixel & GreenMask) >> GreenShift;
		RGBA->B = (Pixel & BlueMask) >> BlueShift;
		RGBA->A = (Pixel & AlphaMask) >> AlphaShift;
		break;

	// 24 bit pixels
	case	24:		// RGB888
		Pixel_8 = (char *) SrcPixel;			
		
		// grab the 3 byte for this 24 bit val
		int	p8,p16,p24;
		p8  = *Pixel_8++;
		p16 = *Pixel_8++;
		p24 = *Pixel_8;

		RGBA->R = p8;
		RGBA->G = p16;
		RGBA->B = p24;
		RGBA->A = 0;
		break;

	// 16 bit pixels
	case	16:			// RGB565, RGB555
		Pixel_16 = (short *) SrcPixel; 
		Pixel = *Pixel_16 & 0xFFFF;
		RGBA->R = (u8)( ((Pixel & RedMask) >> RedShift ) * DIV5BITS );
		RGBA->G = (u8)( ((Pixel & GreenMask) >> GreenShift ) * DIV5BITS );
		RGBA->B = (u8)( ((Pixel & BlueMask) >> BlueShift ) * DIV5BITS );
		if ( AlphaShift >= 0 )
			RGBA->A = ( Pixel & AlphaMask ) ? 255 : 0;
		else
			RGBA->A = 0;
		break;

	default:
		ASSERTS ( true, "Unsupported Pixel Depth");
		return(false);
	}

	return (true);
}
#endif
//----------------------------------------------------------------------------------
#if 0
bool TextureUtil::PutPixel (char* Texture, int x, int y, int dst_w, int dst_h, int format, ts_bRGBA RGBA)
{
	ASSERTS( false, "IS THIS USED?" );

	u16	PixelFormat;
	int*		Pixel_32;
	short*	Pixel_16;
	char*		Pixel_8;
	int		i;

	// check bounds
	ASSERTS ( x <= dst_w,  "X > Width of Texture" );
	ASSERTS ( y <= dst_h, "Y > Height of Texture" );

	// Determine format and set up masks to get pixel
	PixelFormat = format;

	ASSERTS (	(PixelFormat != (SIZE_8_BIT | TYPE_CI)) && 
					(PixelFormat != (SIZE_4_BIT | TYPE_CI)), 
					"ERROR: Blitting into Paletted Texture" );

	if ( PixelFormat == (SIZE_8_BIT | TYPE_CI) || 
		  PixelFormat == (SIZE_4_BIT | TYPE_CI) )
		return (false);

	// Lookup format in list
	for ( i = 0; i < MAX_PIXEL_FORMATS; i++)
	{
		if ( PixelFormatLookup[i].Format == PixelFormat )
			break;
	}

	// bad format?
	if (PixelFormatLookup[i].Format == -1)
	{
		ASSERTS ( PixelFormatLookup[i].Format != -1, "Unsupported Pixel Format");
		return (false);
	}

	// Setup Masks and grab the pixel
	int	RedShift   = PixelFormatLookup[i].RedShift;
	int	GreenShift = PixelFormatLookup[i].GreenShift;
	int	BlueShift  = PixelFormatLookup[i].BlueShift;
	int	AlphaShift = PixelFormatLookup[i].AlphaShift;
	int	bpp = PixelFormatLookup[i].Bpp;

	// calculate index into the texture
	char*	DstPixel = (char *) Texture;
	DstPixel += ( ( x * (bpp / 8) ) + ( ( y * dst_w ) * (bpp / 8) ) );

	// get float versions of colors
	float r = RGBA.R;
	float g = RGBA.G;
	float b = RGBA.B;
	int	a = 0;

	// put the pixel
	switch (bpp)
	{
	case	32:
		Pixel_32 = (int *) DstPixel;
		*Pixel_32 = (RGBA.A << AlphaShift) |
						(RGBA.B << BlueShift) | 
						(RGBA.G << GreenShift) | 
						(RGBA.R << RedShift);
		break;

	case	24:		// RGB888
		Pixel_8 = (char *) DstPixel;

		*Pixel_8++ = RGBA.B;
		*Pixel_8++ = RGBA.G;
		*Pixel_8	  = RGBA.R;
		break;

	case	16:	// RGB555, RGB565
		Pixel_16 = (short *) DstPixel;
		r /= DIV5BITS;
		g /= DIV5BITS;
		b /= DIV5BITS;
		a  = ( AlphaShift >= 0 && RGBA.A >= 0x80 ) ? 1 : 0;
		*Pixel_16 = ((int)b << BlueShift) + 
						((int)g << GreenShift) + 
						((int)r << RedShift) +
						((int)a << AlphaShift);
		break;

	default:
		ASSERTS ( true, "Unsupported Pixel Depth");
		return(false);
	}

	return (true);
}
#endif
//----------------------------------------------------------------------------------
// get a pixel from a texture
#if 0
bool TextureUtil::GetPixel ( Image* IP, void* ImageBuffer, int x, int y, ts_bRGBA *RGBA )
{
	ASSERTS( false, "IS THIS USED?" );

	u16		PixelFormat;
	int*		Pixel_32;
	short*	Pixel_16;
	char*		Pixel_8;
	int		Pixel;
	int		i;

	// check bounds
	ASSERTS ( x <= IP->GetWidth(),  "X > Width of Texture" );
	ASSERTS ( y <= IP->GetHeight(), "Y > Height of Texture" );

	// Determine format and set up masks to get pixel
	PixelFormat = IP->GetFormat();

	ASSERTS (	(PixelFormat != (SIZE_8_BIT | TYPE_CI)) && 
					(PixelFormat != (SIZE_4_BIT | TYPE_CI)), 
					"ERROR: Blitting into a paletted texture" );

	if ( PixelFormat == (SIZE_8_BIT | TYPE_CI) || 
		  PixelFormat == (SIZE_4_BIT | TYPE_CI) )
		return (false);

	// Lookup format in list
	for ( i = 0; i < MAX_PIXEL_FORMATS; i++)
	{
		if ( PixelFormatLookup[i].Format == PixelFormat )
			break;
	}

	// bad format?
	if (PixelFormatLookup[i].Format == -1)
	{
		ASSERTS ( true, "Unsupported Pixel Format");
		return (false);
	}

	// Setup Masks and grab the pixel
	int	RedMask   = PixelFormatLookup[i].RedMask;
	int	GreenMask = PixelFormatLookup[i].GreenMask;
	int	BlueMask  = PixelFormatLookup[i].BlueMask;
	int	AlphaMask = PixelFormatLookup[i].AlphaMask;
	int	RedShift   = PixelFormatLookup[i].RedShift;
	int	GreenShift = PixelFormatLookup[i].GreenShift;
	int	BlueShift  = PixelFormatLookup[i].BlueShift;
	int	AlphaShift = PixelFormatLookup[i].AlphaShift;
	int	bpp = PixelFormatLookup[i].Bpp;

	// calculate index into the texture
	char*	SrcPixel = (char *) ImageBuffer;
	SrcPixel += ( ( x * (bpp / 8) ) + ( ( y * IP->GetWidth() ) * (bpp / 8) ) );

	// get the pixel
	switch (bpp)
	{
	// 32 bit pixels
	case	32:
		Pixel_32 = (int *) SrcPixel;
		Pixel = *Pixel_32;
		RGBA->R = (Pixel & RedMask) >> RedShift;
		RGBA->G = (Pixel & GreenMask) >> GreenShift;
		RGBA->B = (Pixel & BlueMask) >> BlueShift;
		RGBA->A = (Pixel & AlphaMask) >> AlphaShift;
		break;

	// 24 bit pixels
	case	24:		// RGB888
		Pixel_8 = (char *) SrcPixel;			
		
		// grab the 3 byte for this 24 bit val
		int	p8,p16,p24;
		p8  = *Pixel_8++;
		p16 = *Pixel_8++;
		p24 = *Pixel_8;

		RGBA->R = p8;
		RGBA->G = p16;
		RGBA->B = p24;
		RGBA->A = 0;
		break;

	// 16 bit pixels
	case	16:			// RGB565, RGB555
		Pixel_16 = (short *) SrcPixel; 
		Pixel = *Pixel_16 & 0xFFFF;
		RGBA->R = (u8)( ((Pixel & RedMask) >> RedShift ) * DIV5BITS );
		RGBA->G = (u8)( ((Pixel & GreenMask) >> GreenShift ) * DIV5BITS );
		RGBA->B = (u8)( ((Pixel & BlueMask) >> BlueShift ) * DIV5BITS );
		if ( AlphaShift >= 0 )
			RGBA->A = ( Pixel & AlphaMask ) ? 255 : 0;
		else
			RGBA->A = 0;
		break;

	default:
		ASSERTS ( true, "Unsupported Pixel Depth");
		return(false);
	}

	return (true);
}
#endif
//----------------------------------------------------------------------------------

// get a pixel from a texture
#if 0
bool TextureUtil::PutPixel ( Image* IP, void* ImageBuffer, int x, int y, ts_bRGBA RGBA )
{
	ASSERTS( false, "IS THIS USED?" );

	u16	PixelFormat;
	int*		Pixel_32;
	short*	Pixel_16;
	char*		Pixel_8;
	int		i;

	// check bounds
	ASSERTS ( x <= IP->GetWidth(),  "X > Width of Texture" );
	ASSERTS ( y <= IP->GetHeight(), "Y > Height of Texture" );

	// Determine format and set up masks to get pixel
	PixelFormat = IP->GetFormat();

	ASSERTS (	(PixelFormat != (SIZE_8_BIT | TYPE_CI)) && 
					(PixelFormat != (SIZE_4_BIT | TYPE_CI)), 
					"ERROR: Blitting into Paletted Texture" );

	if ( PixelFormat == (SIZE_8_BIT | TYPE_CI) || 
		  PixelFormat == (SIZE_4_BIT | TYPE_CI) )
		return (false);

	// Lookup format in list
	for ( i = 0; i < MAX_PIXEL_FORMATS; i++)
	{
		if ( PixelFormatLookup[i].Format == PixelFormat )
			break;
	}

	// bad format?
	if (PixelFormatLookup[i].Format == -1)
	{
		ASSERTS ( PixelFormatLookup[i].Format != -1, "Unsupported Pixel Format");
		return (false);
	}

	// Setup Masks and grab the pixel
	int	RedShift   = PixelFormatLookup[i].RedShift;
	int	GreenShift = PixelFormatLookup[i].GreenShift;
	int	BlueShift  = PixelFormatLookup[i].BlueShift;
	int	AlphaShift = PixelFormatLookup[i].AlphaShift;
	int	bpp = PixelFormatLookup[i].Bpp;

	// calculate index into the texture
	char*	DstPixel = (char *) ImageBuffer;
	DstPixel += ( ( x * (bpp / 8) ) + ( ( y * IP->GetWidth() ) * (bpp / 8) ) );

	// get float versions of colors
	float r = RGBA.R;
	float g = RGBA.G;
	float b = RGBA.B;
	int	a = 0;

	// put the pixel
	switch (bpp)
	{
	case	32:
		Pixel_32 = (int *) DstPixel;
		*Pixel_32 = (RGBA.A << AlphaShift) |
						(RGBA.B << BlueShift) | 
						(RGBA.G << GreenShift) | 
						(RGBA.R << RedShift);
		break;

	case	24:		// RGB888
		Pixel_8 = (char *) DstPixel;

		*Pixel_8++ = RGBA.B;
		*Pixel_8++ = RGBA.G;
		*Pixel_8	  = RGBA.R;
		break;

	case	16:	// RGB555, RGB565
		Pixel_16 = (short *) DstPixel;
		r /= DIV5BITS;
		g /= DIV5BITS;
		b /= DIV5BITS;
		a  = ( AlphaShift >= 0 && RGBA.A >= 0x80 ) ? 1 : 0;
		*Pixel_16 = ((int)b << BlueShift) + 
						((int)g << GreenShift) + 
						((int)r << RedShift) +
						((int)a << AlphaShift);
		break;

	default:
		ASSERTS ( true, "Unsupported Pixel Depth");
		return(false);
	}

	return (true);
}
#endif
//----------------------------------------------------------------------------------
// get a pixel index from a paletted texture
#if 0
bool TextureUtil::GetPixelIndex ( Image* IP, void* ImageBuffer, int x, int y,	int *Index )
{
	ASSERTS( false, "IS THIS USED?" );

	u16	PixelFormat;
	int	i;

	// check bounds
	ASSERTS ( x <= IP->GetWidth(),  "X > Width of Texture" );
	ASSERTS ( y <= IP->GetHeight(), "Y > Height of Texture" );

	// Determine format and set up masks to get pixel
	PixelFormat = IP->GetFormat();

	ASSERTS ( ((PixelFormat & TYPE_MASK) != TYPE_RGB) && ((PixelFormat & TYPE_MASK) != TYPE_RGBA), "No Palette, this is an RGB/RGBA Texture" );

	// Lookup format in list
	for ( i = 0; i < MAX_PIXEL_FORMATS; i++)
	{
		if ( PixelFormatLookup[i].Format == PixelFormat )
			break;
	}

	// Setup Masks etc and grab the pixel
	int	bpp = PixelFormatLookup[i].Bpp;

	// calculate index into the texture
	char*	SrcPixel = (char*) ImageBuffer;

	// get index
	if (bpp == SIZE_8_BIT)
	{
		SrcPixel += ( ( x * (bpp / 8) ) + ( ( y * IP->GetWidth() ) * (bpp / 8) ) );
		*Index = *SrcPixel;
	}
	else	// do nibble
	{
		ASSERTS (false, "4-bit Not implemented yet");
		SrcPixel += ( ( x * (bpp / 8) ) + ( ( y * IP->GetWidth() ) * (bpp / 8) ) );
		*Index = *SrcPixel;
	}
   
	return (true);
}
#endif
//----------------------------------------------------------------------------------
// get a pixel index from a paletted texture
#if 0
bool TextureUtil::GetPixelIndex ( char *Src, int x, int y, int dst_w, int dst_h, int format,	int *Index )
{
	ASSERTS( false, "IS THIS USED?" );

	u16	PixelFormat;
	int	i;
	int	width;

	// check bounds
	ASSERTS ( x <= dst_w,  "X > Width of Texture" );
	ASSERTS ( y <= dst_h, "Y > Height of Texture" );

	// Determine format and set up masks to get pixel
	PixelFormat = format;

	ASSERTS ( ((PixelFormat & TYPE_MASK) != TYPE_RGB) && ((PixelFormat & TYPE_MASK) != TYPE_RGBA), "No Palette, this is an RGB/RGBA Texture" );

	// Lookup format in list
	for ( i = 0; i < MAX_PIXEL_FORMATS; i++)
	{
		if ( PixelFormatLookup[i].Format == PixelFormat )
			break;
	}

	// Setup Masks etc and grab the pixel
	int	bpp = PixelFormatLookup[i].Bpp;

	// calculate index into the texture
	char*	SrcPixel = (char*) Src;	

	// get index
	if (bpp == SIZE_8_BIT)
	{
		SrcPixel += ( ( x * (bpp / 8) ) + ( ( y * width ) * (bpp / 8) ) );
		*Index = *SrcPixel;
	}
	else	// do nibble
	{
		ASSERTS (false, "4-bit Not implemented yet");
		SrcPixel += ( ( x * (bpp / 8) ) + ( ( y * width ) * (bpp / 8) ) );
		*Index = *SrcPixel;
	}
   
	return (true);
}
#endif
//----------------------------------------------------------------------------------
// put a pixel index into a paletted texture
#if 0
bool TextureUtil::PutPixelIndex ( Image* IP, void* ImageBuffer, int x, int y,	int Index )
{
	ASSERTS( false, "IS THIS USED?" );

	u16	PixelFormat;
	int	i;

	// check bounds
	ASSERTS ( x <= IP->GetWidth(),  "X > Width of Texture" );
	ASSERTS ( y <= IP->GetHeight(), "Y > Height of Texture" );

	// Determine format and set up masks to get pixel
	PixelFormat = IP->GetFormat();

	ASSERTS ( ((PixelFormat & TYPE_MASK) != TYPE_RGB) && ((PixelFormat & TYPE_MASK) != TYPE_RGBA), "No Palette, this is an RGB/RGBA Texture" );

	// Lookup format in list
	for ( i = 0; i < MAX_PIXEL_FORMATS; i++)
	{
		if ( PixelFormatLookup[i].Format == PixelFormat )
			break;
	}

	// Setup Masks etc and grab the pixel
	int	bpp = PixelFormatLookup[i].Bpp;

	// calculate index into the texture
	char*	DstPixel = (char *) ImageBuffer;

	// put pixel index
	if (bpp == SIZE_8_BIT)
	{
		DstPixel += ( ( x * (bpp / 8) ) + ( ( y * IP->GetWidth() ) * (bpp / 8) ) );
		*DstPixel = Index;
	}
	else		// do nibble
	{
		ASSERTS (false, "4-bit Not implemented yet");
		DstPixel += ( ( x * (bpp / 8) ) + ( ( y * IP->GetWidth() ) * (bpp / 8) ) );
		*DstPixel = Index;
	}

	return (true);
}
#endif
//----------------------------------------------------------------------------------
// put a pixel index into a paletted texture
#if 0
bool TextureUtil::PutPixelIndex ( char* Dst, int x, int y, int dst_w, int dst_h, int format, int Index )
{
	ASSERTS( false, "IS THIS USED?" );

	u16	PixelFormat;
	int	i;
	int	width;

	// check bounds
	ASSERTS ( x <= dst_w,  "X > Width of Texture" );
	ASSERTS ( y <= dst_h,  "Y > Height of Texture" );

	// Determine format and set up masks to get pixel
	PixelFormat = format;	

	ASSERTS ( ((PixelFormat & TYPE_MASK) != TYPE_RGB) && ((PixelFormat & TYPE_MASK) != TYPE_RGBA), "No Palette, this is an RGB/RGBA Texture" );

	// Lookup format in list
	for ( i = 0; i < MAX_PIXEL_FORMATS; i++)
	{
		if ( PixelFormatLookup[i].Format == PixelFormat )
			break;
	}

	// Setup Masks etc and grab the pixel
	int	bpp = PixelFormatLookup[i].Bpp;

	// calculate index into the texture
	char*	DstPixel = (char *) Dst;

	// put pixel index
	if (bpp == SIZE_8_BIT)
	{
		DstPixel += ( ( x * (bpp / 8) ) + ( ( y * width ) * (bpp / 8) ) );
		*DstPixel = Index;
	}
	else		// do nibble
	{
		ASSERTS (false, "4-bit Not implemented yet");
		DstPixel += ( ( x * (bpp / 8) ) + ( ( y * width ) * (bpp / 8) ) );
		*DstPixel = Index;
	}

	return (true);
}
#endif

//----------------------------------------------------------------------------------
// puts a palette entry into the texture palette
bool TextureUtil::PutPaletteEntry ( Image *IP, void *PalBuffer , int Index, ts_bRGBA Color )
{
	ASSERTS( false, "IS THIS USED?" );

	u16		PixelFormat;
	int*		Pixel_32;
	short*	Pixel_16;
	char*		Pixel_8;
	int		i;

	// get palette
	if (!PalBuffer)
		return (false);

	// Determine format and set up masks to get pixel
	PixelFormat = IP->GetFormat();

	ASSERTS ( ((PixelFormat & TYPE_MASK) != TYPE_RGB) && ((PixelFormat & TYPE_MASK)!= TYPE_RGBA), "ERROR: Not a paletted texture" );

	// get format
	int	PaletteFormat = IP->GetPaletteFormat();

		// Lookup format in list
	for ( i = 0; i < MAX_PIXEL_FORMATS; i++)
	{
		if ( PixelFormatLookup[i].Format == PaletteFormat )
			break;
	}

	// bad format?
	if (PixelFormatLookup[i].Format == -1)
	{
		ASSERTS ( true, "Unsupported Pixel Format");
		return (false);
	}

	// Setup Masks and grab the pixel
	int	RedShift   = PixelFormatLookup[i].RedShift;
	int	GreenShift = PixelFormatLookup[i].GreenShift;
	int	BlueShift  = PixelFormatLookup[i].BlueShift;
	int	AlphaShift = PixelFormatLookup[i].AlphaShift;
	int	bpp = PixelFormatLookup[i].Bpp;

		// 8 bit palette
	if (PixelFormat == (SIZE_8_BIT | TYPE_CI) )
	{
		ASSERTS (Index <= 255, "Palette Index > 255");

		// unswizzle - TODO
	}

	if (PixelFormat == (SIZE_4_BIT | TYPE_CI) )
		ASSERTS (Index <= 15, "Palette Index > 15");

	// calculate index into the texture
	char*	DstPixel = (char *) PalBuffer;

	// unswizzle for ps2
	#if defined (PS2)
	Index = TextureUtil_PS2PaletteSwizzle[Index];
	#endif

	DstPixel += Index * (bpp / 8);

	// get float versions of colors
	float r = Color.R;
	float g = Color.G;
	float b = Color.B;
	int	a = Color.A;

	// put the pixel
	switch (bpp)
	{
	case	32:
		Pixel_32 = (int *) DstPixel;
		*Pixel_32 = (Color.A << AlphaShift) |
						(Color.B << BlueShift) | 
						(Color.G << GreenShift) | 
						(Color.R << RedShift);
		break;

	case	24:		// RGB888
		Pixel_8 = (char *) DstPixel;

		*Pixel_8++ = Color.B;
		*Pixel_8++ = Color.G;
		*Pixel_8	  = Color.R;
		break;

	case	16:	// RGB555, RGB565
		Pixel_16 = (short *) DstPixel;
		r /= DIV5BITS;
		g /= DIV5BITS;
		b /= DIV5BITS;
		*Pixel_16 = ((int)b << BlueShift) + 
						((int)g << GreenShift) + 
						((int)r << RedShift);
						((int)a << AlphaShift);
		break;

	default:
		ASSERTS ( true, "Unsupported Pixel Depth");
		return(false);
	}

	return (true);
}

//----------------------------------------------------------------------------------
// Blend two colours
#if 0
ts_bRGBA TextureUtil::BlendColours ( ts_bRGBA c1, ts_bRGBA c2)
{
	ASSERTS( false, "IS THIS USED?" );

	int	color, color1, color2;
	ts_bRGBA newcolor;

	color1 = TS_RGBA_TO_RGBA8888(c1);
	color2 = TS_RGBA_TO_RGBA8888(c2);

	if (TextureUtil_BlendMode == TEXTUREUTIL_ADDITIVE)
		color = AdditiveBlend_RGBA8888(color1, color2);

	if (TextureUtil_BlendMode == TEXTUREUTIL_SUBTRACTIVE)
		color = SubtractiveBlend_RGBA8888(color1, color2);

	if (TextureUtil_BlendMode == TEXTUREUTIL_MULTIPLICATIVE)
		color = MultiplicativeBlend_RGBA8888(color1, color2);

	RGBA8888_TO_TS_RGBA(color, newcolor);

	return (newcolor);
}
#endif

//----------------------------------------------------------------------------------
// get BPP for a given IP*
int TextureUtil::GetBPP( Image* IP )
{
	u16		PixelFormat;
	int		i;

	// Determine format and set up masks to get pixel
	PixelFormat = IP->GetFormat();

	// Lookup format in list
	for ( i = 0; i < MAX_PIXEL_FORMATS; i++)
	{
		if ( PixelFormatLookup[i].Format == PixelFormat )
			break;
	}

	if (PixelFormatLookup[i].Format == -1)
		return (-1);
	else
		return (PixelFormatLookup[i].Bpp);
}

//----------------------------------------------------------------------------------
bool TextureUtil::FastGammaAdjust (Image * IP)
{
	int		PaletteFormat = IP->GetPaletteFormat();
	int		colors = ( ( IP->GetFormat() & SIZE_MASK ) == SIZE_4_BIT ) ? 16 : 256;
	ts_bRGBA	Color;
	ts_bRGBA	gammacolor;
	int		i, j;

	// Lookup format in list
	for ( j = 0; j < MAX_PIXEL_FORMATS; j++)
	{
		if ( PixelFormatLookup[j].Format == PaletteFormat )
			break;
	}

	int	bpp = PixelFormatLookup[j].Bpp;
	int	Alpha;

	if (bpp == 32)
	{
		int*	SrcPixel = (int*)IP->GetPaletteBuffer();

		// loop through and modify the palette entry
		for ( i = 0; i < colors; i++)
		{
			// get pixel
			Color.R = (*SrcPixel & 0xFF) >> 0;
			Color.G = (*SrcPixel & 0xFF00) >> 8;
			Color.B = (*SrcPixel & 0xFF0000) >> 16;
			Alpha = (*SrcPixel & 0xFF000000) >> 24;

			// adjust gamma
			gammacolor = AdjustGamma (Color, true);

			// put pixel
			*SrcPixel = (Alpha << 24) | 
							(gammacolor.B << 16) |
							(gammacolor.G << 8)  |
							(gammacolor.R << 0);
         
			// increment pointer
			SrcPixel++;
		}
	}
	else if (bpp == 16)
	{
		short*	SrcPixel = (short*)IP->GetPaletteBuffer();

		// get the masks
		int	RedMask   = PixelFormatLookup[j].RedMask;
		int	GreenMask = PixelFormatLookup[j].GreenMask;
		int	BlueMask  = PixelFormatLookup[j].BlueMask;
		int	AlphaMask = PixelFormatLookup[j].AlphaMask;
		int	RedShift   = PixelFormatLookup[j].RedShift;
		int	GreenShift = PixelFormatLookup[j].GreenShift;
		int	BlueShift  = PixelFormatLookup[j].BlueShift;
		int	AlphaShift = PixelFormatLookup[j].AlphaShift;

		// loop through and modify the palette entry
		for ( i = 0; i < colors; i++)
		{
			// get pixel
			Color.R = (u8)(((*SrcPixel & RedMask) >> RedShift) * DIV5BITS);
			Color.G = (u8)(((*SrcPixel & GreenMask) >> GreenShift ) * DIV5BITS);
			Color.B = (u8)(((*SrcPixel & BlueMask) >> BlueShift ) * DIV5BITS);
			if ( AlphaShift >= 0 )
				Alpha = ( *SrcPixel & AlphaMask ) ? 255 : 0;
			else
				Alpha = 0;

			// adjust gamma
			gammacolor = AdjustGamma (Color, true);

			// put pixel
			*SrcPixel = ((int)(gammacolor.B / DIV5BITS) << BlueShift) +
							((int)(gammacolor.G / DIV5BITS) << GreenShift) +
							((int)(gammacolor.R / DIV5BITS) << RedShift) +
							(int) (Alpha << AlphaShift);

			// increment pointer
			SrcPixel++;
		}
	}
	IP->ReleasePaletteBuffer();

	return true;
}

//----------------------------------------------------------------------------------
// Adjust a Palette with a given blend color
bool TextureUtil::BlendPalette ( Image* IP, int StartIndex, int EndIndex, ts_bRGBA color)
{
	ts_bRGBA	dstColor;
	ts_bRGBA	newColor;
	ts_bRGBA	hsvColor;
	float		h,s,v;
	float		r,g,b;

	PixelAccess dst( IP );

	// loop through and get modify palette entries
	for (int i = StartIndex; i < EndIndex; i++)
	{
		switch(TextureUtil_BlendMode)
		{
			case	TEXTUREUTIL_REPLACE:
				dst.PutPaletteEntry( i, color );
				break;

				case	TEXTUREUTIL_ADDITIVE:
					dstColor = dst.GetPaletteEntry( i );
					dstColor = AdditiveBlend( dstColor, color );
					dst.PutPaletteEntry( i, dstColor );
					break;

				case	TEXTUREUTIL_SUBTRACTIVE:
					dstColor = dst.GetPaletteEntry( i );
					dstColor = SubtractiveBlend( dstColor, color );
					dst.PutPaletteEntry( i, dstColor );
					break;

				case	TEXTUREUTIL_MULTIPLICATIVE:
					dstColor = dst.GetPaletteEntry( i );
					dstColor = MultiplicativeBlend( dstColor, color );
					dst.PutPaletteEntry( i, dstColor );
					break;

				case	TEXTUREUTIL_GAMMA:
					dstColor = dst.GetPaletteEntry( i );
					dstColor = AdjustGamma( dstColor, true );
					dst.PutPaletteEntry( i, dstColor );
					break;

				case	TEXTUREUTIL_ADDITIVE_HSV:
					dstColor = dst.GetPaletteEntry( i );
					RGBtoHSV(dstColor.R, dstColor.G, dstColor.B, &h,&s,&v);
					hsvColor.R = (u8)h; hsvColor.G = (u8)s; hsvColor.B = (u8)v;
					dstColor = HSVAdditive( hsvColor, color);
					HSVtoRGB(&r, &g, &b, dstColor.R, dstColor.G, dstColor.B);
					newColor.R = (u8)r; newColor.G = (u8)g, newColor.B = (u8)b;
					newColor.A = dstColor.A;
					dst.PutPaletteEntry( i, newColor );
					break;

				default:
					ASSERTS( false, "Blend Mode Not Supported" );
					break;
		}
	}

	return true;
}
//----------------------------------------------------------------------------------
// find a colour in the palette and return the index
#if 0
int TextureUtil::FindColor(Image* IP, ts_bRGBA color)
{
	int		num_colours;
	int		i;
	short*	Pixel_16;
	int*		Pixel_32;

	ASSERTS ( ((IP->GetFormat() & TYPE_MASK) != TYPE_RGBA) && ((IP->GetFormat() & TYPE_MASK) != TYPE_RGBA), "Can't find a colour index on an RGB(A) texture");

	void*	PalBuf = IP->GetPaletteBuffer();
	if (!PalBuf)
		return (false);

	// how many colours
	if ( (IP->GetFormat() & SIZE_MASK) == SIZE_8_BIT)
		num_colours = 255;
	else if ( (IP->GetFormat() & SIZE_MASK) == SIZE_4_BIT )
		num_colours = 15;
   
	// get format
	int	PaletteFormat = IP->GetPaletteFormat();

		// Lookup format in list
	for ( i = 0; i < MAX_PIXEL_FORMATS; i++)
	{
		if ( PixelFormatLookup[i].Format == PaletteFormat )
			break;
	}

	// bad format?
	if (PixelFormatLookup[i].Format == -1)
	{
		ASSERTS ( true, "Unsupported Pixel Format");
		return (false);
	}

	// Setup Masks and grab the pixel
	int	RedMask   = PixelFormatLookup[i].RedMask;
	int	GreenMask = PixelFormatLookup[i].GreenMask;
	int	BlueMask  = PixelFormatLookup[i].BlueMask;
	int	AlphaMask = PixelFormatLookup[i].AlphaMask;
	int	RedShift   = PixelFormatLookup[i].RedShift;
	int	GreenShift = PixelFormatLookup[i].GreenShift;
	int	BlueShift  = PixelFormatLookup[i].BlueShift;
	int	AlphaShift = PixelFormatLookup[i].AlphaShift;
	int	bpp = PixelFormatLookup[i].Bpp;

	Pixel_16 = (short*)PalBuf;		
	Pixel_32 = (int*)PalBuf;		
	ts_bRGBA	pal_colour;

	if (bpp == 32)
	{
		// 32 bit colour entries
		for ( i = 0 ; i < num_colours; i++)
		{	
			// get a colour
			RGBA8888_TO_TS_RGBA(*Pixel_32++, pal_colour);

			if (TextureUtil::ColorsEqual( pal_colour, color) )
			{
				IP->ReleasePaletteBuffer();
				return (i);
			}
		}
	}
	else
	{
		// 16 bit colour entries
		for ( i = 0 ; i < num_colours; i++)
		{	
			ts_bRGBA	Color;
			int		pixel;

			// get a colour
			pixel = *Pixel_16++;
			Color.R = (pixel & RedMask) >> RedShift;
			Color.G = (pixel & GreenMask) >> GreenShift;
			Color.B = (pixel & BlueMask) >> BlueShift;
			Color.A = (pixel & AlphaMask) >> AlphaShift;

			if (TextureUtil::ColorsEqual( Color, color) )
			{
				IP->ReleasePaletteBuffer();
				return (i);
			}
		}
	}


	IP->ReleasePaletteBuffer();
	return (-1);
}
#endif
//----------------------------------------------------------------------------------

// find a colour threshold in a given channel in the palette and return the index
// threshold 0-255.. channel see enums TEXTUREUTIL_RED, etc
#if 0
int TextureUtil::FindColorThreshold(Image* IP, int channelmask, int threshold)
{
	int		num_colours;
	int		i;
	short*	Pixel_16;
	int*		Pixel_32;

	ASSERTS ( ((IP->GetFormat() & TYPE_MASK) != TYPE_RGBA) && ((IP->GetFormat() & TYPE_MASK) != TYPE_RGBA), "Can't find a colour index on an RGB(A) texture");

	void*	PalBuf = IP->GetPaletteBuffer();
	if (!PalBuf)
		return (false);

	if (channelmask == 0)
		return (false);

	// how many colours
	if ( (IP->GetFormat() & SIZE_MASK) == SIZE_8_BIT)
		num_colours = 255;
	else if ( (IP->GetFormat() & SIZE_MASK) == SIZE_4_BIT )
		num_colours = 15;
   
	Pixel_16 = (short*)PalBuf;		
	Pixel_32 = (int*)PalBuf;		

	// get format
	int	PaletteFormat = IP->GetPaletteFormat();

	// Lookup format in list
	for ( i = 0; i < MAX_PIXEL_FORMATS; i++)
	{
		if ( PixelFormatLookup[i].Format == PaletteFormat )
			break;
	}

	// bad format?
	if (PixelFormatLookup[i].Format == -1)
	{
		ASSERTS ( true, "Unsupported Pixel Format");
		return (false);
	}

	// Setup Masks and grab the pixel
	int	RedMask   = PixelFormatLookup[i].RedMask;
	int	GreenMask = PixelFormatLookup[i].GreenMask;
	int	BlueMask  = PixelFormatLookup[i].BlueMask;
	int	AlphaMask = PixelFormatLookup[i].AlphaMask;
	int	RedShift   = PixelFormatLookup[i].RedShift;
	int	GreenShift = PixelFormatLookup[i].GreenShift;
	int	BlueShift  = PixelFormatLookup[i].BlueShift;
	int	AlphaShift = PixelFormatLookup[i].AlphaShift;
	int	bpp = PixelFormatLookup[i].Bpp;


	// 32 bit colour entries
	for ( i = 0 ; i < num_colours; i++)
	{	
		int		pixel;
		ts_bRGBA	Color;
		int	threshold_found = 0;

		// get a colour
		if (bpp == 32)
			pixel = *Pixel_32++;
		else
			pixel = *Pixel_16++;

		Color.R = (pixel & RedMask) >> RedShift;
		Color.G = (pixel & GreenMask) >> GreenShift;
		Color.B = (pixel & BlueMask) >> BlueShift;
		Color.A = (pixel & AlphaMask) >> AlphaShift;

		// check thresholds
		if (channelmask & TEXTUREUTIL_RED)
		{
			if (Color.R < threshold)
				break;
			else
				threshold_found += TEXTUREUTIL_RED;
		}

		if (channelmask & TEXTUREUTIL_GREEN)
		{
			if (Color.G < threshold)
				break;
			else
				threshold_found += TEXTUREUTIL_GREEN;
		}

		if (channelmask & TEXTUREUTIL_BLUE)
		{
			if (Color.B < threshold)
				break;
			else
				threshold_found += TEXTUREUTIL_BLUE;
		}

		if (channelmask & TEXTUREUTIL_ALPHA)
		{
			if (Color.A < threshold)
				break;
			else
				threshold_found += TEXTUREUTIL_ALPHA;
		}

		if (threshold_found == channelmask)
		{
			IP->ReleasePaletteBuffer();
			return(i);
		}
	}

	IP->ReleasePaletteBuffer();
	return (-1);
}
#endif
//----------------------------------------------------------------------------------

//
// COLORSPACE CONVERSION FUNCTIONS
// 

// RGB 0-255
// h = 0-360 s,v 0 - 100
//	if s == 0, then h = -1 (undefined)

void TextureUtil::RGBtoHSV( float r, float g, float b, float *h, float *s, float *v )
{
	float min;
	float max;
	float delta;

	// get a val between 0 and 1
	r /= 255.0f;
	g /= 255.0f;
	b /= 255.0f;

	min = Math::Min( r, g, b );
	max = Math::Max( r, g, b );

	*v = max;					// v
	delta = max - min;
	
	if( max != 0 )
		*s = delta / max;		// s
	else 
	{
		// r = g = b = 0		
		// s = 0, v is undefined
		*s = 0;
		*h = -1;
		return;
	}
	
	if( r == max )
		*h = ( g - b ) / delta;			// between yellow & magenta
	else if( g == max )
		*h = 2 + ( b - r ) / delta;	// between cyan & yellow
	else
		*h = 4 + ( r - g ) / delta;	// between magenta & cyan
	
	*h *= 60;								// degrees
	if( *h < 0 )
		*h += 360;

	// set to 0-100 scale
	*s *= 100.0f;
	*v *= 100.0f;
}

//----------------------------------------------------------------------------------

// RGB 0-255
// h = 0-360 s,v 0 - 100

void TextureUtil::HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
	int i;
	float f, p, q, t;

	// get a val between 0 and 1
	s /= 100.0f;
	v /= 100.0f;

	if( s == 0 ) 
	{
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}

	h /= 60;					// sector 0 to 5
	i = (int)Math::Floor( h );
	f = h - i;				// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	
	switch( i ) 
	{
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;

		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
	
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;

		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;

		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;

		default:		// case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}

	// get scale back to 0-100
	*r *= 255.0f;
	*g *= 255.0f;
	*b *= 255.0f;
}

//----------------------------------------------------------------------------------

void TextureUtil::RGBtoYCrCb( float R, float G, float B, float* Y, float* Cr, float* Cb )
{
	if ( Y  ) { float y  =  0.2990f*R + 0.5870f*G + 0.1140f*B         ; *Y  = TEXTUREUTIL_CLAMP( y , 0.0f, 255.0f ); }
	if ( Cr ) { float cr =  0.5000f*R - 0.4187f*G - 0.0813f*B + 128.0f; *Cr = TEXTUREUTIL_CLAMP( cr, 0.0f, 255.0f ); }
	if ( Cb ) { float cb = -0.1687f*R - 0.3313f*G + 0.5000f*B + 128.0f; *Cb = TEXTUREUTIL_CLAMP( cb, 0.0f, 255.0f ); }
}
 
//============================================================================
 
void TextureUtil::YCrCbtoRGB( float Y, float Cr, float Cb, float* R, float* G, float* B )
{
	if ( R ) { float r = Y                        + 1.40200f*(Cr-128.0f); *R = TEXTUREUTIL_CLAMP( r, 0.0f, 255.0f ); }
	if ( G ) { float g = Y - 0.34414f*(Cb-128.0f) - 0.71414f*(Cr-128.0f); *G = TEXTUREUTIL_CLAMP( g, 0.0f, 255.0f ); }
	if ( B ) { float b = Y + 1.77200f*(Cb-128.0f)                       ; *B = TEXTUREUTIL_CLAMP( b, 0.0f, 255.0f ); }
}



