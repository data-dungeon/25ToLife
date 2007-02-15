/***********************************/
/*                                 */
/* TextureUtil.h                   */
/*										     */
/* Paul Blagay  avalanche software */
/*                                 */
/***********************************/

#ifndef __TEXTUREUTIL_H__
#define __TEXTUREUTIL_H__

#include "Layers/LayersStruct.h"

// compiler defines (just for PS2 right now its needed for E3)
#if !defined( DIRECTX)
#define	GAMMA_CORRECTION_ON_LOAD
#endif

// defines
#define RGBA8888_ALPHA_MASK		0xFF000000
#define RGBA8888_RED_MASK			0x00FF0000
#define RGBA8888_GREEN_MASK		0x0000FF00
#define RGBA8888_BLUE_MASK			0x000000FF

enum {
	// blend modes
	TEXTUREUTIL_REPLACE,
	TEXTUREUTIL_ADDITIVE,
	TEXTUREUTIL_SUBTRACTIVE,
	TEXTUREUTIL_MULTIPLICATIVE,
	TEXTUREUTIL_GAMMA,
	TEXTUREUTIL_ADDITIVE_HSV,

	// blit modes
	TEXTUREUTIL_TEXTURE_REPLACE,
	TEXTUREUTIL_TEXTURE_ADDITIVE,
	TEXTUREUTIL_TEXTURE_SUBTRACTIVE,
	TEXTUREUTIL_TEXTURE_MULTIPLICATIVE,
	TEXTUREUTIL_COLOR_REPLACE,
	TEXTUREUTIL_COLOR_ADDITIVE,
	TEXTUREUTIL_COLOR_SUBTRACTIVE,
	TEXTUREUTIL_COLOR_MULTIPLICATIVE,
};

enum {
	// alpha modes
	TEXTUREUTIL_NO_ALPHA,
	TEXTUREUTIL_SRC_ALPHA,
	TEXTUREUTIL_DST_ALPHA,
	TEXTUREUTIL_BLEND_ALPHA,
};

enum {
	// channels
	TEXTUREUTIL_RED		= 0x01,
	TEXTUREUTIL_GREEN		= 0x02,
	TEXTUREUTIL_BLUE		= 0x04,
	TEXTUREUTIL_ALPHA		= 0x08,
};

// used by engine for texture formats
enum {
	SIZE_MASK		= 0x000F,
	SIZE_4_BIT		= 0x0000,
	SIZE_8_BIT		= 0x0001,
	SIZE_16_BIT		= 0x0002,
	SIZE_24_BIT		= 0x0003,
	SIZE_32_BIT		= 0x0004,
	SIZE_S3TC		= 0x0005,

	TYPE_MASK		= 0x00F0,
	TYPE_RGB			= 0x0000,
	TYPE_CI			= 0x0010,
	TYPE_IA			= 0x0020,
	TYPE_RGBA		= 0x0030,
	TYPE_LUMINANCE	= 0x0040,
	TYPE_CI_IA		= 0x0050,
};

// struct for each pixel format with masks
struct ts_PixelFormats
{
	int		Format;					// Pixel format
	int		HasPalette;				// Paletted?
	int		Bpp;						// Bits Per Pixel
	int		RedMask;					//
	int		GreenMask;				//
	int		BlueMask;				//	masks
	int		AlphaMask;				//
	int		RedShift;				//				
	int		GreenShift;				// shifts
	int		BlueShift;				//
	int		AlphaShift;				//
};

// texture manipulation class.. software blending, etc
class TextureUtil
{
public:
		// Loading functions
		static	char* LoadBinaryTextureSet ( char* Filename );		// loads bin file to memory

		// testbed
		static	bool	Test ( CActor *actor );

		// Get/Put functions for pixels & palettes
		static	bool	GetPixel ( Image* IP, void* ImageBuffer, int x, int y, ts_bRGBA *RGBA );
		static	bool	GetPixel (char* Texture, int x, int y, int dst_w, int dst_h, int format, ts_bRGBA* RGBA);
		static	bool	PutPixel ( Image* IP, void* ImageBuffer, int x, int y, ts_bRGBA RGBA );
		static	bool	PutPixel (char* Texture, int x, int y, int dst_w, int dst_h, int format,  ts_bRGBA RGBA);
		static	bool	GetPixelIndex ( Image* IP, void* ImageBuffer, int x, int y, int *Index );
		static	bool	GetPixelIndex ( char *Src, int x, int y, int dst_w, int dst_h, int format,	int *Index );
		static	bool	PutPixelIndex ( Image* IP, void* ImageBuffer, int x, int y, int Index );
		static	bool	PutPixelIndex ( char *Src, int x, int y, int dst_w, int dst_h, int format,	int Index );
		static	bool	GetPaletteEntry ( Image* IP, void *PalBuffer , int Index, ts_bRGBA* Color );
		static	bool	GetPaletteEntry ( int format, void* PalBuffer, int Index, ts_bRGBA* Color );
		static	bool	PutPaletteEntry ( Image* IP, void *PalBuffer , int Index, ts_bRGBA Color );
		static	bool	PutPaletteEntry ( int format, void *PalBuffer , int Index, ts_bRGBA Color );

		// blit functions
		static	bool	BlitTexture ( char* IP_Src, char* IP_Dst, int x, int y, int blit_w, int blit_h, int dst_w, int dst_h, int format );
		static	bool	BlitTexture ( char* IP_Src, Image* IP_Dst, int x, int y, int width, int height );
		static	bool	BlitTexture ( Image* IP_Src, Image* IP_Dst, int x, int y, int	blit_w, int blit_h );

		static	bool	BlitColor ( char* Image, ts_bRGBA color, int x, int y, int blit_w, int blit_h, int dst_w, int dst_h, int format );
		static	bool	BlitColor ( Image* Image, ts_bRGBA color, int x, int y, int width, int height );
		static	bool	BlitColor ( Image* Image, int x, int y, int width, int height );

		static	bool	BlitPalettedTexture ( char* IP_Src, char* IP_Dst, int x, int y, int blit_w, int blit_h, int dst_w, int dst_h, int format );
		static	bool	BlitPalettedTexture ( char* IP_Src, Image* IP_Dst, int x, int y, int width, int height );
		static	bool	BlendPalette ( Image* IP, int StartIndex, int EndIndex, ts_bRGBA color);
		static	bool	BlendPalette ( Image* IP, int StartIndex, int EndIndex);

		// utility functions
		static	void	SetBlendMode (int BlendMode);
		static	void	SetAlphaMode (int AlphaMode);
		static	void	SetGamma (float gamma);
		static	float	GetGamma ();
		static   void  SetGammaZeroToOne( float gamma );
		static   float GetGammaZeroToOne( void );
		static	float* GetGammaTablePtr();
		static	void	UpdateGamma();

		#if defined (DIRECTX) || defined (_XBOX)
		static	void	SetDXGammaRamp();
		#endif

		static	void	CutoutOn (bool flag);
		static	void	SetTransparencyColor ( ts_bRGBA TransparencyColor);
		static	bool	ColorsEqual( ts_bRGBA c1, ts_bRGBA c2);
		static	int	GetBPP( Image* IP );
		static	ts_bRGBA	BlendColours ( ts_bRGBA c1, ts_bRGBA c2);
		static	int	FindColor(Image* IP, ts_bRGBA color);
		static	int	FindColorThreshold(Image* IP, int channel, int threshold);
		static	ts_bRGBA RGBtoSepia( ts_bRGBA color );
		static	ts_bRGBA	AdjustGamma ( ts_bRGBA Color, bool monochromeScale );
		static	bool	FastGammaAdjust (Image * IP);

		// colorspace conversion utility funcs..
		static	void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v );		// RGB 0-255, h 0-360, s,v 0-100
		static	void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v );		// RGB 0-255, h 0-360, s,v 0-100
		static	void RGBtoYCrCb( float R, float G, float B, float* Y, float* Cr, float* Cb );
		static	void YCrCbtoRGB( float Y, float Cr, float Cb, float* R, float* G, float* B );

		// blend functions
		static	ts_bRGBA AdditiveBlend( ts_bRGBA c1, ts_bRGBA c2 );
		static	ts_bRGBA SubtractiveBlend( ts_bRGBA c1, ts_bRGBA c2 );
		static	ts_bRGBA MultiplicativeBlend( ts_bRGBA c1, ts_bRGBA c2 );
	
		// enable / disable
		static	bool	GetGammaOnLoadFlag(void);
		static	void	SetGammaOnLoadFlag(bool Flag);
};

#endif // __TEXTUREUTIL_H__