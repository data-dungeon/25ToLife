#ifdef ASKLFJDSFHDFH

#include "hud/texture.h"

static int TextureAlignment = 4;

COLOR32 ColorBlend( COLOR32 A, COLOR32 B )
{
	ColorBlendA( A, B, B>>24);
}

COLOR32 ColorBlendA( COLOR32 A, COLOR32 B, int O )
{
	// TODO: XBOX MMX would be nice here...

	return (((A>>8&0x00ff0000)*O+(256-O)*(B>>8&0x00ff0000))<<0)&0xff000000|
			 (((A>>0&0x00ff0000)*O+(256-O)*(B>>0&0x00ff0000))>>8)&0x00ff0000|
		    (((A>>0&0x0000ff00)*O+(256-O)*(B>>0&0x0000ff00))>>8)&0x0000ff00|
			 (((A>>0&0x000000ff)*O+(256-O)*(B>>0&0x000000ff))>>8)&0x000000ff;
}

COLOR32 ColorAdd( COLOR32 A, COLOR32 B )
{
	COLOR32 Alpha = (A>>1&0x7f800000)+(B>>1&0x7f800000);
	COLOR32 Red   = (A>>0&0x00ff0000)+(B>>0&0x00ff0000);
	COLOR32 Green = (A>>0&0x0000ff00)+(B>>0&0x0000ff00);
	COLOR32 Blue  = (A>>0&0x000000ff)+(B>>0&0x000000ff);

	// Saturated?

	if( Alpha & 0x80000000 ) Alpha = 0x7f800000;
	if( Red   & 0x01000000 ) Red   = 0x00ff0000;
	if( Green & 0x00010000 ) Green = 0x0000ff00;
	if( Blue  & 0x00000100 ) Blue  = 0x000000ff;

	return (Alpha<<1)|Red|Green|Blue;
}

COLOR32 ColorSub( COLOR32 A, COLOR32 B )
{
	COLOR32 Alpha = (A>>1&0x7f800000)-(B>>1&0x7f800000);
	COLOR32 Red   = (A>>0&0x00ff0000)-(B>>0&0x00ff0000);
	COLOR32 Green = (A>>0&0x0000ff00)-(B>>0&0x0000ff00);
	COLOR32 Blue  = (A>>0&0x000000ff)-(B>>0&0x000000ff);

	// Saturated?

	if( Alpha & 0x80000000 ) Alpha = 0;
	if( Red   & 0x80000000 ) Red   = 0;
	if( Green & 0x80000000 ) Green = 0;
	if( Blue  & 0x80000000 ) Blue  = 0;

	return (Alpha<<1)|Red|Green|Blue;
}


int GetBPPFromTexelFormat( TEXELFORMAT Format )
{
	switch( Format )
	{
	case TEXELFMT_A8R8G8B8:
	case TEXELFMT_X8R8G8B8:
		return 32;
	case TEXELFMT_A4R4G4B4:
	case TEXELFMT_X4R4G4B4:
	case TEXELFMT_A1R5G5B5:
	case TEXELFMT_X1R5G5B5:
	case TEXELFMT_X0R5G6B5:
		return 16;
	case TEXELFMT_X0R8G8B8:
		return 24;
	}

	return 0;
}

TEXELFORMAT GetTexelFormatFromBPP( int BPP )
{
	if( BPP == 32 )
		return TEXELFMT_X8R8G8B8;

	if( BPP == 24 )
		return TEXELFMT_X0R8G8B8;

	if( BPP == 16 )
		return TEXELFMT_X1R5G5B5;
	
	return TEXELFMT_UNKNOWN;
}

void SetTextureAlignment( int Alignment )
{
	TextureAlignment = Alignment;
}

int GetTextureAlignment( void )
{
	return TextureAlignment;
}

int TexelFormatBPP( TEXELFORMAT Format )
{
	switch( Format )
	{
	case TEXELFMT_A8R8G8B8:
	case TEXELFMT_X8R8G8B8:
		return 32;
	case TEXELFMT_A4R4G4B4:
	case TEXELFMT_X4R4G4B4:
	case TEXELFMT_A1R5G5B5:
	case TEXELFMT_X1R5G5B5:
	case TEXELFMT_X0R5G6B5:
		return 16;
	case TEXELFMT_X0R8G8B8:
		return 24;
	}

	return 0;
}

HTEXTURE CreateTexture( int Width, int Height, TEXELFORMAT Format )
{
	HTEXTURE Texture = new TEXTURE;

	if( Texture )	
	{
		int BPP = GetBPPFromTexelFormat( Format );

		Texture->Format = Format;
		Texture->Flags  = 0;
		Texture->Width  = Width;
		Texture->Height = Height;
		Texture->BPP    = BPP;
		Texture->Pitch  = (Width * BPP / 8) + (TextureAlignment-1) & -(TextureAlignment);
		Texture->Stride = Texture->Pitch / (BPP / 8);
		Texture->Size   = Texture->Pitch * Texture->Height;

		Texture->Bits = new char[Texture->Size];

		if( !Texture->Bits )
			delete Texture, Texture = 0;
	}

	return Texture;
}

HTEXTURE CreateTexturePow2( int Width, int Height, TEXELFORMAT Format )
{
	Width--, Height--;
	
	int TexWidth;

	for( TexWidth = 1; Width > 0; Width >>= 1 )
		TexWidth <<= 1;

	int TexHeight;

	for( TexHeight = 1; Height > 0; Height >>= 1 )
		TexHeight <<= 1;

	return CreateTexture( TexWidth, TexHeight, Format );
}

HTEXTURE AssumeTexture( char *Bits, int Width, int Height, int Pitch, TEXELFORMAT Format )
{
	HTEXTURE Texture = new TEXTURE;

	if( Texture )
	{
		int BPP = GetBPPFromTexelFormat( Format );

		Texture->Format = Format;
		Texture->Flags |= TEXTUREFLAG_ASSUMED;
		Texture->Width	 = Width;
		Texture->Height = Height;
		Texture->BPP	 = BPP;
		Texture->Pitch  = Pitch;
		Texture->Stride = Pitch * BPP/8;
		Texture->Size   = Pitch * Height;
		Texture->Bits	 = Bits;
	}

	return Texture;
}

HTEXTURE DeleteTexture( HTEXTURE Texture )
{
	if( Texture )
	{
		if( !(Texture->Flags & TEXTUREFLAG_ASSUMED) )
		{
			if( Texture->Bits )
				delete [] Texture->Bits;
		}

		delete Texture;
	}

	return 0;
}

char *GetScanLineAddress( HTEXTURE Texture, int ScanLine, int X )
{
	if( Texture )
		return &Texture->Bits[(X*Texture->BPP>>3) + ScanLine*Texture->Pitch];

	return 0;
}

void CopyToHardwareTexture( t_Handle Handle, HTEXTURE Texture )
{
	// TODO: Do it
}

#endif
