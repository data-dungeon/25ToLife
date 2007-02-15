#ifdef ASKLFJDSFHDFH

#include "hud/texture.h"

struct
{
	HTEXTURE Handle;
	TEXTURE Texture;
}
channel[4];

void SetTexelSafeUnknown( int x, int y, COLOR32 c )
{
	return;
}

void SetTexelSafeA8R8G8B8_0( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[0].Texture.Width && y < channel[0].Texture.Height )
		((COLOR32*) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride] = c;

	return;
}

void SetTexelSafeA8R8G8B8_1( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[1].Texture.Width && y < channel[1].Texture.Height )
		((COLOR32*) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride] = c;

	return;
}

void SetTexelSafeA8R8G8B8_2( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[2].Texture.Width && y < channel[2].Texture.Height )
		((COLOR32*) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride] = c;

	return;
}

void SetTexelSafeA8R8G8B8_3( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[3].Texture.Width && y < channel[3].Texture.Height )
		((COLOR32*) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride] = c;

	return;
}

void SetTexelSafeA4R4G4B4_0( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[0].Texture.Width && y < channel[0].Texture.Height )
	{
		((COLOR16*) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride] = 
			(c >>16 & 0xf000)|
			(c >>12 & 0x0f00)|
			(c >> 8 & 0x00f0)|
			(c >> 4 & 0x000f);
	}

	return;
}

void SetTexelSafeA4R4G4B4_1( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[1].Texture.Width && y < channel[1].Texture.Height )
	{
		((COLOR16*) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride] = 
			(c >>16 & 0xf000)|
			(c >>12 & 0x0f00)|
			(c >> 8 & 0x00f0)|
			(c >> 4 & 0x000f);
	}

	return;
}

void SetTexelSafeA4R4G4B4_2( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[2].Texture.Width && y < channel[2].Texture.Height )
	{
		((COLOR16*) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride] = 
			(c >>16 & 0xf000)|
			(c >>12 & 0x0f00)|
			(c >> 8 & 0x00f0)|
			(c >> 4 & 0x000f);
	}

	return;
}

void SetTexelSafeA4R4G4B4_3( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[3].Texture.Width && y < channel[3].Texture.Height )
	{
		((COLOR16*) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride] = 
			(c >>16 & 0xf000)|
			(c >>12 & 0x0f00)|
			(c >> 8 & 0x00f0)|
			(c >> 4 & 0x000f);
	}

	return;
}

void SetTexelSafeX4R4G4B4_0( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[0].Texture.Width && y < channel[0].Texture.Height )
	{
		((COLOR16*) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride] = 
			(c >>12 & 0x0f00)|
			(c >> 8 & 0x00f0)|
			(c >> 4 & 0x000f);
	}

	return;
}

void SetTexelSafeX4R4G4B4_1( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[1].Texture.Width && y < channel[1].Texture.Height )
	{
		((COLOR16*) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride] = 
			(c >>12 & 0x0f00)|
			(c >> 8 & 0x00f0)|
			(c >> 4 & 0x000f);
	}

	return;
}

void SetTexelSafeX4R4G4B4_2( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[2].Texture.Width && y < channel[2].Texture.Height )
	{
		((COLOR16*) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride] = 
			(c >>12 & 0x0f00)|
			(c >> 8 & 0x00f0)|
			(c >> 4 & 0x000f);
	}

	return;
}

void SetTexelSafeX4R4G4B4_3( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[3].Texture.Width && y < channel[3].Texture.Height )
	{
		((COLOR16*) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride] = 
			(c >>12 & 0x0f00)|
			(c >> 8 & 0x00f0)|
			(c >> 4 & 0x000f);
	}

	return;
}

void SetTexelSafeA1R5G5B5_0( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[0].Texture.Width && y < channel[0].Texture.Height )
	{
		((COLOR16*) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride] = 
			(c >> 16 & 0x8000)|
			(c >>  9 & 0x7c00)|
			(c >>  6 & 0x03e0)|
			(c >>  3 & 0x001f);
	}

	return;
}

void SetTexelSafeA1R5G5B5_1( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[1].Texture.Width && y < channel[1].Texture.Height )
	{
		((COLOR16*) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride] = 
			(c >> 16 & 0x8000)|
			(c >>  9 & 0x7c00)|
			(c >>  6 & 0x03e0)|
			(c >>  3 & 0x001f);
	}

	return;
}

void SetTexelSafeA1R5G5B5_2( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[2].Texture.Width && y < channel[2].Texture.Height )
	{
		((COLOR16*) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride] = 
			(c >> 16 & 0x8000)|
			(c >>  9 & 0x7c00)|
			(c >>  6 & 0x03e0)|
			(c >>  3 & 0x001f);
	}

	return;
}

void SetTexelSafeA1R5G5B5_3( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[3].Texture.Width && y < channel[3].Texture.Height )
	{
		((COLOR16*) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride] = 
			(c >> 16 & 0x8000)|
			(c >>  9 & 0x7c00)|
			(c >>  6 & 0x03e0)|
			(c >>  3 & 0x001f);
	}

	return;
}

void SetTexelSafeX1R5G5B5_0( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[0].Texture.Width && y < channel[0].Texture.Height )
	{
		((COLOR16*) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride] = 
			(c >>  9 & 0x7c00)|
			(c >>  6 & 0x03e0)|
			(c >>  3 & 0x001f);
	}

	return;
}

void SetTexelSafeX1R5G5B5_1( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[1].Texture.Width && y < channel[1].Texture.Height )
	{
		((COLOR16*) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride] = 
			(c >>  9 & 0x7c00)|
			(c >>  6 & 0x03e0)|
			(c >>  3 & 0x001f);
	}

	return;
}

void SetTexelSafeX1R5G5B5_2( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[2].Texture.Width && y < channel[2].Texture.Height )
	{
		((COLOR16*) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride] = 
			(c >>  9 & 0x7c00)|
			(c >>  6 & 0x03e0)|
			(c >>  3 & 0x001f);
	}

	return;
}

void SetTexelSafeX1R5G5B5_3( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[3].Texture.Width && y < channel[3].Texture.Height )
	{
		((COLOR16*) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride] = 
			(c >>  9 & 0x7c00)|
			(c >>  6 & 0x03e0)|
			(c >>  3 & 0x001f);
	}

	return;
}

void SetTexelSafeX0R5G6B5_0( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[0].Texture.Width && y < channel[0].Texture.Height )
	{
		((COLOR16*) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride] = 
			(c >>  8 & 0x7800)|
			(c >>  5 & 0x07e0)|
			(c >>  3 & 0x001f);
	}

	return;
}

void SetTexelSafeX0R5G6B5_1( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[1].Texture.Width && y < channel[1].Texture.Height )
	{
		((COLOR16*) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride] = 
			(c >>  8 & 0x7800)|
			(c >>  5 & 0x07e0)|
			(c >>  3 & 0x001f);
	}

	return;
}

void SetTexelSafeX0R5G6B5_2( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[2].Texture.Width && y < channel[2].Texture.Height )
	{
		((COLOR16*) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride] = 
			(c >>  8 & 0x7800)|
			(c >>  5 & 0x07e0)|
			(c >>  3 & 0x001f);
	}

	return;
}

void SetTexelSafeX0R5G6B5_3( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[3].Texture.Width && y < channel[3].Texture.Height )
	{
		((COLOR16*) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride] = 
			(c >>  8 & 0x7800)|
			(c >>  5 & 0x07e0)|
			(c >>  3 & 0x001f);
	}

	return;
}

void SetTexelSafeX0R8G8B8_0( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[0].Texture.Width && y < channel[0].Texture.Height )
	{
		COLOR24 *p = (COLOR24 *)&((char *) channel[0].Texture.Bits)[y * channel[0].Texture.Pitch];

		p[x].Red   = (unsigned char) c >> 16;
		p[x].Green = (unsigned char) c >>  8;
		p[x].Blue  = (unsigned char) c >>  0;
	}

	return;
}

void SetTexelSafeX0R8G8B8_1( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[1].Texture.Width && y < channel[1].Texture.Height )
	{
		COLOR24 *p = (COLOR24 *)&((char *) channel[1].Texture.Bits)[y * channel[1].Texture.Pitch];

		p[x].Red   = (unsigned char) c >> 16;
		p[x].Green = (unsigned char) c >>  8;
		p[x].Blue  = (unsigned char) c >>  0;
	}

	return;
}


void SetTexelSafeX0R8G8B8_2( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[2].Texture.Width && y < channel[2].Texture.Height )
	{
		COLOR24 *p = (COLOR24 *)&((char *) channel[2].Texture.Bits)[y * channel[2].Texture.Pitch];

		p[x].Red   = (unsigned char) c >> 16;
		p[x].Green = (unsigned char) c >>  8;
		p[x].Blue  = (unsigned char) c >>  0;
	}

	return;
}

void SetTexelSafeX0R8G8B8_3( int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < channel[3].Texture.Width && y < channel[3].Texture.Height )
	{
		COLOR24 *p = (COLOR24 *)&((char *) channel[3].Texture.Bits)[y * channel[3].Texture.Pitch];

		p[x].Red   = (unsigned char) c >> 16;
		p[x].Green = (unsigned char) c >>  8;
		p[x].Blue  = (unsigned char) c >>  0;
	}

	return;
}

SETTEXELFUNCTION SetTexelSafe[4] = { SetTexelSafeUnknown, SetTexelSafeUnknown, SetTexelSafeUnknown, SetTexelSafeUnknown };

COLOR32 GetTexelSafeUnknown( int x, int y )
{
	return 0;
}

COLOR32 GetTexelSafeA8R8G8B8_0( int x, int y )
{
	if( (x|y) >= 0 && x < channel[0].Texture.Width && y < channel[0].Texture.Height )
		return ((COLOR32 *) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride];

	return 0;
}

COLOR32 GetTexelSafeA8R8G8B8_1( int x, int y )
{
	if( (x|y) >= 0 && x < channel[1].Texture.Width && y < channel[1].Texture.Height )
		return ((COLOR32 *) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride];

	return 0;
}

COLOR32 GetTexelSafeA8R8G8B8_2( int x, int y )
{
	if( (x|y) >= 0 && x < channel[2].Texture.Width && y < channel[2].Texture.Height )
		return ((COLOR32 *) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride];

	return 0;
}

COLOR32 GetTexelSafeA8R8G8B8_3( int x, int y )
{
	if( (x|y) >= 0 && x < channel[3].Texture.Width && y < channel[3].Texture.Height )
		return ((COLOR32 *) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride];

	return 0;
}

COLOR32 GetTexelSafeA4R4G4B4_0( int x, int y )
{
	if( (x|y) >= 0 && x < channel[0].Texture.Width && y < channel[0].Texture.Height )
	{
		COLOR16 c = ((COLOR16 *) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride];

		return (COLOR32)(c <<16 & 0xf0000000)|
							 (c <<12 & 0x00f00000)|
							 (c << 8 & 0x0000f000)|
							 (c << 4 & 0x000000f0);
	}

	return 0;
}

COLOR32 GetTexelSafeA4R4G4B4_1( int x, int y )
{
	if( (x|y) >= 0 && x < channel[1].Texture.Width && y < channel[1].Texture.Height )
	{
		COLOR16 c = ((COLOR16 *) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride];

		return (COLOR32)(c <<16 & 0xf0000000)|
							 (c <<12 & 0x00f00000)|
							 (c << 8 & 0x0000f000)|
							 (c << 4 & 0x000000f0);
	}

	return 0;
}

COLOR32 GetTexelSafeA4R4G4B4_2( int x, int y )
{
	if( (x|y) >= 0 && x < channel[2].Texture.Width && y < channel[2].Texture.Height )
	{
		COLOR16 c = ((COLOR16 *) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride];

		return (COLOR32)(c <<16 & 0xf0000000)|
							 (c <<12 & 0x00f00000)|
							 (c << 8 & 0x0000f000)|
							 (c << 4 & 0x000000f0);
	}

	return 0;
}

COLOR32 GetTexelSafeA4R4G4B4_3( int x, int y )
{
	if( (x|y) >= 0 && x < channel[3].Texture.Width && y < channel[3].Texture.Height )
	{
		COLOR16 c = ((COLOR16 *) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride];

		return (COLOR32)(c <<16 & 0xf0000000)|
							 (c <<12 & 0x00f00000)|
							 (c << 8 & 0x0000f000)|
							 (c << 4 & 0x000000f0);
	}

	return 0;
}

COLOR32 GetTexelSafeX4R4G4B4_0( int x, int y )
{
	if( (x|y) >= 0 && x < channel[0].Texture.Width && y < channel[0].Texture.Height )
	{
		COLOR16 c = ((COLOR16 *) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride];

		return (COLOR32)(c <<12 & 0x00f00000)|
							 (c << 8 & 0x0000f000)|
							 (c << 4 & 0x000000f0);
	}

	return 0;
}

COLOR32 GetTexelSafeX4R4G4B4_1( int x, int y )
{
	if( (x|y) >= 0 && x < channel[1].Texture.Width && y < channel[1].Texture.Height )
	{
		COLOR16 c = ((COLOR16 *) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride];

		return (COLOR32)(c <<12 & 0x00f00000)|
							 (c << 8 & 0x0000f000)|
							 (c << 4 & 0x000000f0);
	}

	return 0;
}

COLOR32 GetTexelSafeX4R4G4B4_2( int x, int y )
{
	if( (x|y) >= 0 && x < channel[2].Texture.Width && y < channel[2].Texture.Height )
	{
		COLOR16 c = ((COLOR16 *) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride];

		return (COLOR32)(c <<12 & 0x00f00000)|
							 (c << 8 & 0x0000f000)|
							 (c << 4 & 0x000000f0);
	}

	return 0;
}

COLOR32 GetTexelSafeX4R4G4B4_3( int x, int y )
{
	if( (x|y) >= 0 && x < channel[3].Texture.Width && y < channel[3].Texture.Height )
	{
		COLOR16 c = ((COLOR16 *) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride];

		return (COLOR32)(c <<12 & 0x00f00000)|
							 (c << 8 & 0x0000f000)|
							 (c << 4 & 0x000000f0);
	}

	return 0;
}

COLOR32 GetTexelSafeA1R5G5B5_0( int x, int y )
{
	if( (x|y) >= 0 && x < channel[0].Texture.Width && y < channel[0].Texture.Height )
	{
		COLOR16 c = ((COLOR16 *) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride];

		return (COLOR32)(c <<16 & 0x80000000)|
							 (c << 9 & 0x00f80000)|
							 (c << 6 & 0x0000f800)|
							 (c << 3 & 0x000000f8);
	}

	return 0;
}

COLOR32 GetTexelSafeA1R5G5B5_1( int x, int y )
{
	if( (x|y) >= 0 && x < channel[1].Texture.Width && y < channel[1].Texture.Height )
	{
		COLOR16 c = ((COLOR16 *) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride];

		return (COLOR32)(c <<16 & 0x80000000)|
							 (c << 9 & 0x00f80000)|
							 (c << 6 & 0x0000f800)|
							 (c << 3 & 0x000000f8);
	}

	return 0;
}

COLOR32 GetTexelSafeA1R5G5B5_2( int x, int y )
{
	if( (x|y) >= 0 && x < channel[2].Texture.Width && y < channel[2].Texture.Height )
	{
		COLOR16 c = ((COLOR16 *) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride];

		return (COLOR32)(c <<16 & 0x80000000)|
							 (c << 9 & 0x00f80000)|
							 (c << 6 & 0x0000f800)|
							 (c << 3 & 0x000000f8);
	}

	return 0;
}

COLOR32 GetTexelSafeA1R5G5B5_3( int x, int y )
{
	if( (x|y) >= 0 && x < channel[3].Texture.Width && y < channel[3].Texture.Height )
	{
		COLOR16 c = ((COLOR16 *) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride];

		return (COLOR32)(c <<16 & 0x80000000)|
							 (c << 9 & 0x00f80000)|
							 (c << 6 & 0x0000f800)|
							 (c << 3 & 0x000000f8);
	}

	return 0;
}

COLOR32 GetTexelSafeX1R5G5B5_0( int x, int y )
{
	if( (x|y) >= 0 && x < channel[0].Texture.Width && y < channel[0].Texture.Height )
	{
		COLOR16 c = ((COLOR16 *) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride];

		return (COLOR32)(c << 9 & 0x00f80000)|
							 (c << 6 & 0x0000f800)|
							 (c << 3 & 0x000000f8);
	}

	return 0;
}

COLOR32 GetTexelSafeX1R5G5B5_1( int x, int y )
{
	if( (x|y) >= 0 && x < channel[1].Texture.Width && y < channel[1].Texture.Height )
	{
		COLOR16 c = ((COLOR16 *) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride];

		return (COLOR32)(c << 9 & 0x00f80000)|
							 (c << 6 & 0x0000f800)|
							 (c << 3 & 0x000000f8);
	}

	return 0;
}

COLOR32 GetTexelSafeX1R5G5B5_2( int x, int y )
{
	if( (x|y) >= 0 && x < channel[2].Texture.Width && y < channel[2].Texture.Height )
	{
		COLOR16 c = ((COLOR16 *) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride];

		return (COLOR32)(c << 9 & 0x00f80000)|
							 (c << 6 & 0x0000f800)|
							 (c << 3 & 0x000000f8);
	}

	return 0;
}

COLOR32 GetTexelSafeX1R5G5B5_3( int x, int y )
{
	if( (x|y) >= 0 && x < channel[3].Texture.Width && y < channel[3].Texture.Height )
	{
		COLOR16 c = ((COLOR16 *) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride];

		return (COLOR32)(c << 9 & 0x00f80000)|
							 (c << 6 & 0x0000f800)|
							 (c << 3 & 0x000000f8);
	}

	return 0;
}

COLOR32 GetTexelSafeX0R5G6B5_0( int x, int y )
{
	if( (x|y) >= 0 && x < channel[0].Texture.Width && y < channel[0].Texture.Height )
	{
		COLOR16 c = ((COLOR16 *) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride];

		return (COLOR32)(c << 8 & 0x00f80000)|
							 (c << 5 & 0x0000fc00)|
							 (c << 3 & 0x000000f8);
	}

	return 0;
}

COLOR32 GetTexelSafeX0R5G6B5_1( int x, int y )
{
	if( (x|y) >= 0 && x < channel[1].Texture.Width && y < channel[1].Texture.Height )
	{
		COLOR16 c = ((COLOR16 *) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride];

		return (COLOR32)(c << 8 & 0x00f80000)|
							 (c << 5 & 0x0000fc00)|
							 (c << 3 & 0x000000f8);
	}

	return 0;
}

COLOR32 GetTexelSafeX0R5G6B5_2( int x, int y )
{
	if( (x|y) >= 0 && x < channel[2].Texture.Width && y < channel[2].Texture.Height )
	{
		COLOR16 c = ((COLOR16 *) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride];

		return (COLOR32)(c << 8 & 0x00f80000)|
							 (c << 5 & 0x0000fc00)|
							 (c << 3 & 0x000000f8);
	}

	return 0;
}

COLOR32 GetTexelSafeX0R5G6B5_3( int x, int y )
{
	if( (x|y) >= 0 && x < channel[3].Texture.Width && y < channel[3].Texture.Height )
	{
		COLOR16 c = ((COLOR16 *) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride];

		return (COLOR32)(c << 8 & 0x00f80000)|
							 (c << 5 & 0x0000fc00)|
							 (c << 3 & 0x000000f8);
	}

	return 0;
}

COLOR32 GetTexelSafeX0R8G8B8_0( int x, int y )
{
	if( (x|y) >= 0 && x < channel[0].Texture.Width && y < channel[0].Texture.Height )
	{
		COLOR24 *p = (COLOR24*)&((char *) channel[0].Texture.Bits)[y * channel[0].Texture.Pitch];

		return (COLOR32)(p[x].Red   <<16)|
							 (p[x].Green << 8)|
							 (p[x].Blue  << 0);
	}

	return 0;
}

COLOR32 GetTexelSafeX0R8G8B8_1( int x, int y )
{
	if( (x|y) >= 0 && x < channel[1].Texture.Width && y < channel[1].Texture.Height )
	{
		COLOR24 *p = (COLOR24*)&((char *) channel[1].Texture.Bits)[y * channel[1].Texture.Pitch];

		return (COLOR32)(p[x].Red   <<16)|
							 (p[x].Green << 8)|
							 (p[x].Blue  << 0);
	}

	return 0;
}

COLOR32 GetTexelSafeX0R8G8B8_2( int x, int y )
{
	if( (x|y) >= 0 && x < channel[2].Texture.Width && y < channel[2].Texture.Height )
	{
		COLOR24 *p = (COLOR24*)&((char *) channel[2].Texture.Bits)[y * channel[2].Texture.Pitch];

		return (COLOR32)(p[x].Red   <<16)|
							 (p[x].Green << 8)|
							 (p[x].Blue  << 0);
	}

	return 0;
}

COLOR32 GetTexelSafeX0R8G8B8_3( int x, int y )
{
	if( (x|y) >= 0 && x < channel[3].Texture.Width && y < channel[3].Texture.Height )
	{
		COLOR24 *p = (COLOR24*)&((char *) channel[3].Texture.Bits)[y * channel[3].Texture.Pitch];

		return (COLOR32)(p[x].Red   <<16)|
							 (p[x].Green << 8)|
							 (p[x].Blue  << 0);
	}

	return 0;
}

GETTEXELFUNCTION GetTexelSafe[4] = { GetTexelSafeUnknown, GetTexelSafeUnknown, GetTexelSafeUnknown, GetTexelSafeUnknown };

void SetTexelA8R8G8B8_0( int x, int y, COLOR32 c )
{
	((COLOR32*) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride] = c;
}

void SetTexelA8R8G8B8_1( int x, int y, COLOR32 c )
{
	((COLOR32*) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride] = c;
}

void SetTexelA8R8G8B8_2( int x, int y, COLOR32 c )
{
	((COLOR32*) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride] = c;
}

void SetTexelA8R8G8B8_3( int x, int y, COLOR32 c )
{
	((COLOR32*) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride] = c;
}

void SetTexelA4R4G4B4_0( int x, int y, COLOR32 c )
{
	((COLOR16*) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride] = 
		(c >>16 & 0xf000)|
		(c >>12 & 0x0f00)|
		(c >> 8 & 0x00f0)|
		(c >> 4 & 0x000f);

	return;
}

void SetTexelA4R4G4B4_1( int x, int y, COLOR32 c )
{
	((COLOR16*) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride] = 
		(c >>16 & 0xf000)|
		(c >>12 & 0x0f00)|
		(c >> 8 & 0x00f0)|
		(c >> 4 & 0x000f);

	return;
}

void SetTexelA4R4G4B4_2( int x, int y, COLOR32 c )
{
	((COLOR16*) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride] = 
		(c >>16 & 0xf000)|
		(c >>12 & 0x0f00)|
		(c >> 8 & 0x00f0)|
		(c >> 4 & 0x000f);

	return;
}

void SetTexelA4R4G4B4_3( int x, int y, COLOR32 c )
{
	((COLOR16*) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride] = 
		(c >>16 & 0xf000)|
		(c >>12 & 0x0f00)|
		(c >> 8 & 0x00f0)|
		(c >> 4 & 0x000f);

	return;
}

void SetTexelX4R4G4B4_0( int x, int y, COLOR32 c )
{
	((COLOR16*) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride] = 
		(c >>12 & 0x0f00)|
		(c >> 8 & 0x00f0)|
		(c >> 4 & 0x000f);

	return;
}

void SetTexelX4R4G4B4_1( int x, int y, COLOR32 c )
{
	((COLOR16*) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride] = 
		(c >>12 & 0x0f00)|
		(c >> 8 & 0x00f0)|
		(c >> 4 & 0x000f);

	return;
}

void SetTexelX4R4G4B4_2( int x, int y, COLOR32 c )
{
	((COLOR16*) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride] = 
		(c >>12 & 0x0f00)|
		(c >> 8 & 0x00f0)|
		(c >> 4 & 0x000f);

	return;
}

void SetTexelX4R4G4B4_3( int x, int y, COLOR32 c )
{
	((COLOR16*) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride] = 
		(c >>12 & 0x0f00)|
		(c >> 8 & 0x00f0)|
		(c >> 4 & 0x000f);

	return;
}

void SetTexelA1R5G5B5_0( int x, int y, COLOR32 c )
{
	((COLOR16*) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride] = 
		(c >> 16 & 0x8000)|
		(c >>  9 & 0x7c00)|
		(c >>  6 & 0x03e0)|
		(c >>  3 & 0x001f);

	return;
}

void SetTexelA1R5G5B5_1( int x, int y, COLOR32 c )
{
	((COLOR16*) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride] = 
		(c >> 16 & 0x8000)|
		(c >>  9 & 0x7c00)|
		(c >>  6 & 0x03e0)|
		(c >>  3 & 0x001f);

	return;
}

void SetTexelA1R5G5B5_2( int x, int y, COLOR32 c )
{
	((COLOR16*) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride] = 
		(c >> 16 & 0x8000)|
		(c >>  9 & 0x7c00)|
		(c >>  6 & 0x03e0)|
		(c >>  3 & 0x001f);

	return;
}

void SetTexelA1R5G5B5_3( int x, int y, COLOR32 c )
{
	((COLOR16*) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride] = 
		(c >> 16 & 0x8000)|
		(c >>  9 & 0x7c00)|
		(c >>  6 & 0x03e0)|
		(c >>  3 & 0x001f);

	return;
}

void SetTexelX1R5G5B5_0( int x, int y, COLOR32 c )
{
	((COLOR16*) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride] = 
		(c >>  9 & 0x7c00)|
		(c >>  6 & 0x03e0)|
		(c >>  3 & 0x001f);

	return;
}

void SetTexelX1R5G5B5_1( int x, int y, COLOR32 c )
{
	((COLOR16*) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride] = 
		(c >>  9 & 0x7c00)|
		(c >>  6 & 0x03e0)|
		(c >>  3 & 0x001f);

	return;
}

void SetTexelX1R5G5B5_2( int x, int y, COLOR32 c )
{
	((COLOR16*) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride] = 
		(c >>  9 & 0x7c00)|
		(c >>  6 & 0x03e0)|
		(c >>  3 & 0x001f);

	return;
}

void SetTexelX1R5G5B5_3( int x, int y, COLOR32 c )
{
	((COLOR16*) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride] = 
		(c >>  9 & 0x7c00)|
		(c >>  6 & 0x03e0)|
		(c >>  3 & 0x001f);

	return;
}

void SetTexelX0R5G6B5_0( int x, int y, COLOR32 c )
{
	((COLOR16*) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride] = 
		(c >>  8 & 0x7800)|
		(c >>  5 & 0x07e0)|
		(c >>  3 & 0x001f);

	return;
}

void SetTexelX0R5G6B5_1( int x, int y, COLOR32 c )
{
	((COLOR16*) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride] = 
		(c >>  8 & 0x7800)|
		(c >>  5 & 0x07e0)|
		(c >>  3 & 0x001f);

	return;
}

void SetTexelX0R5G6B5_2( int x, int y, COLOR32 c )
{
	((COLOR16*) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride] = 
		(c >>  8 & 0x7800)|
		(c >>  5 & 0x07e0)|
		(c >>  3 & 0x001f);

	return;
}

void SetTexelX0R5G6B5_3( int x, int y, COLOR32 c )
{
	((COLOR16*) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride] = 
		(c >>  8 & 0x7800)|
		(c >>  5 & 0x07e0)|
		(c >>  3 & 0x001f);

	return;
}

void SetTexelX0R8G8B8_0( int x, int y, COLOR32 c )
{
	COLOR24 *p = (COLOR24 *)&((char *) channel[0].Texture.Bits)[y * channel[0].Texture.Pitch];

	p[x].Red   = (unsigned char) c >> 16;
	p[x].Green = (unsigned char) c >>  8;
	p[x].Blue  = (unsigned char) c >>  0;

	return;
}

void SetTexelX0R8G8B8_1( int x, int y, COLOR32 c )
{
	COLOR24 *p = (COLOR24 *)&((char *) channel[1].Texture.Bits)[y * channel[1].Texture.Pitch];

	p[x].Red   = (unsigned char) c >> 16;
	p[x].Green = (unsigned char) c >>  8;
	p[x].Blue  = (unsigned char) c >>  0;

	return;
}


void SetTexelX0R8G8B8_2( int x, int y, COLOR32 c )
{
	COLOR24 *p = (COLOR24 *)&((char *) channel[2].Texture.Bits)[y * channel[2].Texture.Pitch];

	p[x].Red   = (unsigned char) c >> 16;
	p[x].Green = (unsigned char) c >>  8;
	p[x].Blue  = (unsigned char) c >>  0;

	return;
}

void SetTexelX0R8G8B8_3( int x, int y, COLOR32 c )
{
	COLOR24 *p = (COLOR24 *)&((char *) channel[3].Texture.Bits)[y * channel[3].Texture.Pitch];

	p[x].Red   = (unsigned char) c >> 16;
	p[x].Green = (unsigned char) c >>  8;
	p[x].Blue  = (unsigned char) c >>  0;

	return;
}

SETTEXELFUNCTION SetTexel[4] = { SetTexelSafeUnknown, SetTexelSafeUnknown, SetTexelSafeUnknown, SetTexelSafeUnknown };

COLOR32 GetTexelA8R8G8B8_0( int x, int y )
{
	return ((COLOR32 *) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride];
}

COLOR32 GetTexelA8R8G8B8_1( int x, int y )
{
	return ((COLOR32 *) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride];
}

COLOR32 GetTexelA8R8G8B8_2( int x, int y )
{
	return ((COLOR32 *) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride];
}

COLOR32 GetTexelA8R8G8B8_3( int x, int y )
{
	return ((COLOR32 *) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride];
}

COLOR32 GetTexelA4R4G4B4_0( int x, int y )
{
	COLOR16 c = ((COLOR16 *) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride];

	return (COLOR32)(c <<16 & 0xf0000000)|
						 (c <<12 & 0x00f00000)|
						 (c << 8 & 0x0000f000)|
						 (c << 4 & 0x000000f0);
}

COLOR32 GetTexelA4R4G4B4_1( int x, int y )
{
	COLOR16 c = ((COLOR16 *) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride];

	return (COLOR32)(c <<16 & 0xf0000000)|
						 (c <<12 & 0x00f00000)|
						 (c << 8 & 0x0000f000)|
						 (c << 4 & 0x000000f0);
}

COLOR32 GetTexelA4R4G4B4_2( int x, int y )
{
	COLOR16 c = ((COLOR16 *) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride];

	return (COLOR32)(c <<16 & 0xf0000000)|
						 (c <<12 & 0x00f00000)|
						 (c << 8 & 0x0000f000)|
						 (c << 4 & 0x000000f0);
}

COLOR32 GetTexelA4R4G4B4_3( int x, int y )
{
	COLOR16 c = ((COLOR16 *) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride];

	return (COLOR32)(c <<16 & 0xf0000000)|
						 (c <<12 & 0x00f00000)|
						 (c << 8 & 0x0000f000)|
						 (c << 4 & 0x000000f0);
}

COLOR32 GetTexelX4R4G4B4_0( int x, int y )
{
	COLOR16 c = ((COLOR16 *) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride];

	return (COLOR32)(c <<12 & 0x00f00000)|
						 (c << 8 & 0x0000f000)|
						 (c << 4 & 0x000000f0);
}

COLOR32 GetTexelX4R4G4B4_1( int x, int y )
{
	COLOR16 c = ((COLOR16 *) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride];

	return (COLOR32)(c <<12 & 0x00f00000)|
						 (c << 8 & 0x0000f000)|
						 (c << 4 & 0x000000f0);
}

COLOR32 GetTexelX4R4G4B4_2( int x, int y )
{
	COLOR16 c = ((COLOR16 *) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride];

	return (COLOR32)(c <<12 & 0x00f00000)|
						 (c << 8 & 0x0000f000)|
						 (c << 4 & 0x000000f0);
}

COLOR32 GetTexelX4R4G4B4_3( int x, int y )
{
	COLOR16 c = ((COLOR16 *) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride];

	return (COLOR32)(c <<12 & 0x00f00000)|
						 (c << 8 & 0x0000f000)|
						 (c << 4 & 0x000000f0);
}

COLOR32 GetTexelA1R5G5B5_0( int x, int y )
{
	COLOR16 c = ((COLOR16 *) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride];

	return (COLOR32)(c <<16 & 0x80000000)|
						 (c << 9 & 0x00f80000)|
						 (c << 6 & 0x0000f800)|
						 (c << 3 & 0x000000f8);
}

COLOR32 GetTexelA1R5G5B5_1( int x, int y )
{
	COLOR16 c = ((COLOR16 *) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride];

	return (COLOR32)(c <<16 & 0x80000000)|
						 (c << 9 & 0x00f80000)|
						 (c << 6 & 0x0000f800)|
						 (c << 3 & 0x000000f8);
}

COLOR32 GetTexelA1R5G5B5_2( int x, int y )
{
	COLOR16 c = ((COLOR16 *) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride];

	return (COLOR32)(c <<16 & 0x80000000)|
						 (c << 9 & 0x00f80000)|
						 (c << 6 & 0x0000f800)|
						 (c << 3 & 0x000000f8);
}

COLOR32 GetTexelA1R5G5B5_3( int x, int y )
{
	COLOR16 c = ((COLOR16 *) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride];

	return (COLOR32)(c <<16 & 0x80000000)|
						 (c << 9 & 0x00f80000)|
						 (c << 6 & 0x0000f800)|
						 (c << 3 & 0x000000f8);
}

COLOR32 GetTexelX1R5G5B5_0( int x, int y )
{
	COLOR16 c = ((COLOR16 *) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride];

	return (COLOR32)(c << 9 & 0x00f80000)|
						 (c << 6 & 0x0000f800)|
						 (c << 3 & 0x000000f8);
}

COLOR32 GetTexelX1R5G5B5_1( int x, int y )
{
	COLOR16 c = ((COLOR16 *) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride];

	return (COLOR32)(c << 9 & 0x00f80000)|
						 (c << 6 & 0x0000f800)|
						 (c << 3 & 0x000000f8);
}

COLOR32 GetTexelX1R5G5B5_2( int x, int y )
{
	COLOR16 c = ((COLOR16 *) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride];

	return (COLOR32)(c << 9 & 0x00f80000)|
						 (c << 6 & 0x0000f800)|
						 (c << 3 & 0x000000f8);
}

COLOR32 GetTexelX1R5G5B5_3( int x, int y )
{
	COLOR16 c = ((COLOR16 *) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride];

	return (COLOR32)(c << 9 & 0x00f80000)|
						 (c << 6 & 0x0000f800)|
						 (c << 3 & 0x000000f8);
}

COLOR32 GetTexelX0R5G6B5_0( int x, int y )
{
	COLOR16 c = ((COLOR16 *) channel[0].Texture.Bits)[x + y * channel[0].Texture.Stride];

	return (COLOR32)(c << 8 & 0x00f80000)|
						 (c << 5 & 0x0000fc00)|
						 (c << 3 & 0x000000f8);
}

COLOR32 GetTexelX0R5G6B5_1( int x, int y )
{
	COLOR16 c = ((COLOR16 *) channel[1].Texture.Bits)[x + y * channel[1].Texture.Stride];

	return (COLOR32)(c << 8 & 0x00f80000)|
						 (c << 5 & 0x0000fc00)|
						 (c << 3 & 0x000000f8);
}

COLOR32 GetTexelX0R5G6B5_2( int x, int y )
{
	COLOR16 c = ((COLOR16 *) channel[2].Texture.Bits)[x + y * channel[2].Texture.Stride];

	return (COLOR32)(c << 8 & 0x00f80000)|
						 (c << 5 & 0x0000fc00)|
						 (c << 3 & 0x000000f8);
}

COLOR32 GetTexelX0R5G6B5_3( int x, int y )
{
	COLOR16 c = ((COLOR16 *) channel[3].Texture.Bits)[x + y * channel[3].Texture.Stride];

	return (COLOR32)(c << 8 & 0x00f80000)|
						 (c << 5 & 0x0000fc00)|
						 (c << 3 & 0x000000f8);
}

COLOR32 GetTexelX0R8G8B8_0( int x, int y )
{
	COLOR24 *p = (COLOR24*)&((char *) channel[0].Texture.Bits)[y * channel[0].Texture.Pitch];

	return (COLOR32)(p[x].Red   <<16)|
						 (p[x].Green << 8)|
						 (p[x].Blue  << 0);
}

COLOR32 GetTexelX0R8G8B8_1( int x, int y )
{
	COLOR24 *p = (COLOR24*)&((char *) channel[1].Texture.Bits)[y * channel[1].Texture.Pitch];

	return (COLOR32)(p[x].Red   <<16)|
						 (p[x].Green << 8)|
						 (p[x].Blue  << 0);
}

COLOR32 GetTexelX0R8G8B8_2( int x, int y )
{
	COLOR24 *p = (COLOR24*)&((char *) channel[2].Texture.Bits)[y * channel[2].Texture.Pitch];

	return (COLOR32)(p[x].Red   <<16)|
						 (p[x].Green << 8)|
						 (p[x].Blue  << 0);
}

COLOR32 GetTexelX0R8G8B8_3( int x, int y )
{
	COLOR24 *p = (COLOR24*)&((char *) channel[3].Texture.Bits)[y * channel[3].Texture.Pitch];

	return (COLOR32)(p[x].Red   <<16)|
						 (p[x].Green << 8)|
						 (p[x].Blue  << 0);
}

GETTEXELFUNCTION GetTexel[4] = { GetTexelSafeUnknown, GetTexelSafeUnknown, GetTexelSafeUnknown, GetTexelSafeUnknown };

void SelectTexture( int Channel, HTEXTURE Texture )
{
	static GETTEXELFUNCTION table1[9][4] = { GetTexelSafeUnknown, GetTexelSafeUnknown, GetTexelSafeUnknown, GetTexelSafeUnknown,
														  GetTexelA8R8G8B8_0, GetTexelA8R8G8B8_1, GetTexelA8R8G8B8_2, GetTexelA8R8G8B8_3,
														  GetTexelA8R8G8B8_0, GetTexelA8R8G8B8_1, GetTexelA8R8G8B8_2, GetTexelA8R8G8B8_3,
														  GetTexelA4R4G4B4_0, GetTexelA4R4G4B4_1, GetTexelA4R4G4B4_2, GetTexelA4R4G4B4_3,
														  GetTexelX4R4G4B4_0, GetTexelX4R4G4B4_1, GetTexelX4R4G4B4_2, GetTexelX4R4G4B4_3,
														  GetTexelA1R5G5B5_0, GetTexelA1R5G5B5_1, GetTexelA1R5G5B5_2, GetTexelA1R5G5B5_3,
														  GetTexelX1R5G5B5_0, GetTexelX1R5G5B5_1, GetTexelX1R5G5B5_2, GetTexelX1R5G5B5_3,
														  GetTexelX0R5G6B5_0, GetTexelX0R5G6B5_1, GetTexelX0R5G6B5_2, GetTexelX0R5G6B5_3,
														  GetTexelX0R8G8B8_0, GetTexelX0R8G8B8_1, GetTexelX0R8G8B8_2, GetTexelX0R8G8B8_3, };
	
	static SETTEXELFUNCTION table2[9][4] = { SetTexelSafeUnknown, SetTexelSafeUnknown, SetTexelSafeUnknown, SetTexelSafeUnknown,
														  SetTexelA8R8G8B8_0, SetTexelA8R8G8B8_1, SetTexelA8R8G8B8_2, SetTexelA8R8G8B8_3,
														  SetTexelA8R8G8B8_0, SetTexelA8R8G8B8_1, SetTexelA8R8G8B8_2, SetTexelA8R8G8B8_3,
														  SetTexelA4R4G4B4_0, SetTexelA4R4G4B4_1, SetTexelA4R4G4B4_2, SetTexelA4R4G4B4_3,
														  SetTexelX4R4G4B4_0, SetTexelX4R4G4B4_1, SetTexelX4R4G4B4_2, SetTexelX4R4G4B4_3,
														  SetTexelA1R5G5B5_0, SetTexelA1R5G5B5_1, SetTexelA1R5G5B5_2, SetTexelA1R5G5B5_3,
														  SetTexelX1R5G5B5_0, SetTexelX1R5G5B5_1, SetTexelX1R5G5B5_2, SetTexelX1R5G5B5_3,
														  SetTexelX0R5G6B5_0, SetTexelX0R5G6B5_1, SetTexelX0R5G6B5_2, SetTexelX0R5G6B5_3,
														  SetTexelX0R8G8B8_0, SetTexelX0R8G8B8_1, SetTexelX0R8G8B8_2, SetTexelX0R8G8B8_3, };
		
	static GETTEXELFUNCTION table3[9][4] = { GetTexelSafeUnknown, GetTexelSafeUnknown, GetTexelSafeUnknown, GetTexelSafeUnknown,
														  GetTexelSafeA8R8G8B8_0, GetTexelSafeA8R8G8B8_1, GetTexelSafeA8R8G8B8_2, GetTexelSafeA8R8G8B8_3,
														  GetTexelSafeA8R8G8B8_0, GetTexelSafeA8R8G8B8_1, GetTexelSafeA8R8G8B8_2, GetTexelSafeA8R8G8B8_3,
														  GetTexelSafeA4R4G4B4_0, GetTexelSafeA4R4G4B4_1, GetTexelSafeA4R4G4B4_2, GetTexelSafeA4R4G4B4_3,
														  GetTexelSafeX4R4G4B4_0, GetTexelSafeX4R4G4B4_1, GetTexelSafeX4R4G4B4_2, GetTexelSafeX4R4G4B4_3,
														  GetTexelSafeA1R5G5B5_0, GetTexelSafeA1R5G5B5_1, GetTexelSafeA1R5G5B5_2, GetTexelSafeA1R5G5B5_3,
														  GetTexelSafeX1R5G5B5_0, GetTexelSafeX1R5G5B5_1, GetTexelSafeX1R5G5B5_2, GetTexelSafeX1R5G5B5_3,
														  GetTexelSafeX0R5G6B5_0, GetTexelSafeX0R5G6B5_1, GetTexelSafeX0R5G6B5_2, GetTexelSafeX0R5G6B5_3,
														  GetTexelSafeX0R8G8B8_0, GetTexelSafeX0R8G8B8_1, GetTexelSafeX0R8G8B8_2, GetTexelSafeX0R8G8B8_3, };
	
	static SETTEXELFUNCTION table4[9][4] = { SetTexelSafeUnknown, SetTexelSafeUnknown, SetTexelSafeUnknown, SetTexelSafeUnknown,
														  SetTexelSafeA8R8G8B8_0, SetTexelSafeA8R8G8B8_1, SetTexelSafeA8R8G8B8_2, SetTexelSafeA8R8G8B8_3,
														  SetTexelSafeA8R8G8B8_0, SetTexelSafeA8R8G8B8_1, SetTexelSafeA8R8G8B8_2, SetTexelSafeA8R8G8B8_3,
														  SetTexelSafeA4R4G4B4_0, SetTexelSafeA4R4G4B4_1, SetTexelSafeA4R4G4B4_2, SetTexelSafeA4R4G4B4_3,
														  SetTexelSafeX4R4G4B4_0, SetTexelSafeX4R4G4B4_1, SetTexelSafeX4R4G4B4_2, SetTexelSafeX4R4G4B4_3,
														  SetTexelSafeA1R5G5B5_0, SetTexelSafeA1R5G5B5_1, SetTexelSafeA1R5G5B5_2, SetTexelSafeA1R5G5B5_3,
														  SetTexelSafeX1R5G5B5_0, SetTexelSafeX1R5G5B5_1, SetTexelSafeX1R5G5B5_2, SetTexelSafeX1R5G5B5_3,
														  SetTexelSafeX0R5G6B5_0, SetTexelSafeX0R5G6B5_1, SetTexelSafeX0R5G6B5_2, SetTexelSafeX0R5G6B5_3,
														  SetTexelSafeX0R8G8B8_0, SetTexelSafeX0R8G8B8_1, SetTexelSafeX0R8G8B8_2, SetTexelSafeX0R8G8B8_3, };

	channel[Channel].Handle = Texture;
	channel[Channel].Texture = *Texture;

	TEXELFORMAT Format = Texture->Format;

	GetTexel[Channel] = table1[Format][Channel];
	SetTexel[Channel] = table2[Format][Channel];
	GetTexelSafe[Channel] = table3[Format][Channel];
	SetTexelSafe[Channel] = table4[Format][Channel];

	return;
}

HTEXTURE Tmp = 0;

void CopyScanLine( HTEXTURE Dst, int Dx, int Dy, HTEXTURE Src, int Sx, int Sy, int Pixels )
{
	char *pDst = GetScanLineAddress( Dst, Dy, Dx );
	char *pSrc = GetScanLineAddress( Src, Sy, Sx );

	memcpy( pDst, pSrc, Pixels * Dst->BPP/8 );
}

typedef void (*SCANLINEFUNCTION)( HTEXTURE, int, int, int );

COLOR32 UnpackA4R4G4B4( COLOR16 c )
{
	return (COLOR32) ((c & 0xf000) << 16)|
						  ((c & 0x0f00) << 12)|
						  ((c & 0x00f0) <<  8)|
						  ((c & 0x000f) <<  4);
}

COLOR32 UnpackX4R4G4B4( COLOR16 c )
{
	return (COLOR32) ((c & 0xf000) << 16)|
						  ((c & 0x0f00) << 12)|
						  ((c & 0x00f0) <<  8)|
						  ((c & 0x000f) <<  4);
}

COLOR32 UnpackA1R5G5B5( COLOR16 c )
{
	return (COLOR32) ((c & 0x8000) << 16)|
						  ((c & 0x7c00) <<  9)|
						  ((c & 0x03e0) <<  6)|
						  ((c & 0x001f) <<  3);
}

COLOR32 UnpackX1R5G5B5( COLOR16 c )
{
	return (COLOR32) ((c & 0x7c00) <<  9)|
						  ((c & 0x03e0) <<  6)|
						  ((c & 0x001f) <<  3);
}

COLOR32 UnpackX0R5G6B5( COLOR16 c )
{
	return (COLOR32) ((c & 0xf800) << 10)|
						  ((c & 0x07e0) <<  6)|
						  ((c & 0x001f) <<  3);
}

COLOR32 UnpackX0R8G8B8( COLOR24 c )
{
	return ((COLOR32) c.Red   << 16)|
			 ((COLOR32) c.Green <<  8)|
			 ((COLOR32) c.Blue  <<  0);
}

void ReadScanLineUnknown( HTEXTURE Src, int Sx, int Sy, int Pixels )
{
}

void ReadScanLineA8R8G8B8( HTEXTURE Src, int Sx, int Sy, int Pixels )
{
	memcpy( Tmp->Bits, GetScanLineAddress( Src, Sy, Sx ), Pixels * Src->BPP/8 );
}

void ReadScanLineA4R4G4B4( HTEXTURE Src, int Sx, int Sy, int Pixels )
{
	COLOR16 *pSrc = (COLOR16 *) Src->Bits;
	COLOR32 *pTmp = (COLOR32 *) Tmp->Bits;
	COLOR16 c;

	for( int x = 0; x < Pixels; x++ )
	{
		c = pSrc[Sx+x];

		pTmp[x] = ((c & 0xf000)<<16)|
					 ((c & 0x0f00)<<12)|
					 ((c & 0x00f0)<< 8)|
					 ((c & 0x000f)<< 4);
	}

	return;
}

void ReadScanLineX4R4G4B4( HTEXTURE Src, int Sx, int Sy, int Pixels )
{
	COLOR16 *pSrc = (COLOR16 *) Src->Bits;
	COLOR32 *pTmp = (COLOR32 *) Tmp->Bits;
	COLOR16 c;

	for( int x = 0; x < Pixels; x++ )
	{
		c = pSrc[Sx+x];

		pTmp[x] = ((c & 0x0f00)<<12)|
					 ((c & 0x00f0)<< 8)|
					 ((c & 0x000f)<< 4);
	}

	return;
}

void ReadScanLineA1R5G5B5( HTEXTURE Src, int Sx, int Sy, int Pixels )
{
	COLOR16 *pSrc = (COLOR16 *) Src->Bits;
	COLOR32 *pTmp = (COLOR32 *) Tmp->Bits;
	COLOR16 c;

	for( int x = 0; x < Pixels; x++ )
	{
		c = pSrc[Sx+x];

		pTmp[x] = ((c & 0x8000)<<16)|
					 ((c & 0x7c00)<< 9)|
					 ((c & 0x03e0)<< 6)|
					 ((c & 0x001f)<< 3);
	}

	return;
}

void ReadScanLineX1R5G5B5( HTEXTURE Src, int Sx, int Sy, int Pixels )
{
	COLOR16 *pSrc = (COLOR16 *) Src->Bits;
	COLOR32 *pTmp = (COLOR32 *) Tmp->Bits;
	COLOR16 c;

	for( int x = 0; x < Pixels; x++ )
	{
		c = pSrc[Sx+x];

		pTmp[x] = ((c & 0x7c00)<< 9)|
					 ((c & 0x03e0)<< 6)|
					 ((c & 0x001f)<< 3);
	}

	return;
}

void ReadScanLineX0R5G6B5( HTEXTURE Src, int Sx, int Sy, int Pixels )
{
	COLOR16 *pSrc = (COLOR16 *) Src->Bits;
	COLOR32 *pTmp = (COLOR32 *) Tmp->Bits;
	COLOR16 c;

	for( int x = 0; x < Pixels; x++ )
	{
		c = pSrc[Sx+x];

		pTmp[x] = ((c & 0xf800)<< 8)|
					 ((c & 0x07e0)<< 5)|
					 ((c & 0x001f)<< 3);
	}

	return;
}

void ReadScanLineX0R8G8B8( HTEXTURE Src, int Sx, int Sy, int Pixels )
{
	COLOR24 *pSrc = (COLOR24 *) Src->Bits;
	COLOR32 *pTmp = (COLOR32 *) Tmp->Bits;
	COLOR24 *p;

	for( int x = 0; x < Pixels; x++ )
	{
		p = &pSrc[Sx+x];

		pTmp[x] = (COLOR32)(p->Red  )<<16|
					 (COLOR32)(p->Green)<< 8|
					 (COLOR32)(p->Blue )<< 0;
	}

	return;
}

void WriteScanLineUnknown( HTEXTURE Dst, int Dx, int Dy, int Pixels )
{
}

void WriteScanLineA8R8G8B8( HTEXTURE Dst, int Dx, int Dy, int Pixels )
{
	memcpy( &((COLOR32 *) Dst->Bits)[Dx + Dy * Dst->Stride], Tmp->Bits, Pixels * Dst->BPP/8 );
}

void WriteScanLineA4R4G4B4( HTEXTURE Dst, int Dx, int Dy, int Pixels )
{
	COLOR32 *pTmp = (COLOR32 *) Tmp->Bits;
	COLOR16 *pDst = (COLOR16 *) Dst->Bits;
	COLOR16 c;

	for( int x = 0; x < Pixels; x++ )
	{
		c = pTmp[x];

		pDst[Dx+x] = (COLOR16)(c>>16&0xf000)|
									 (c>>12&0x0f00)|
									 (c>> 8&0x00f0)|
									 (c>> 4&0x000f);
	}

	return;
}

void WriteScanLineX4R4G4B4( HTEXTURE Dst, int Dx, int Dy, int Pixels )
{
	COLOR32 *pTmp = (COLOR32 *) Tmp->Bits;
	COLOR16 *pDst = (COLOR16 *) Dst->Bits;
	COLOR16 c;

	for( int x = 0; x < Pixels; x++ )
	{
		c = pTmp[x];

		pDst[Dx+x] = (COLOR16)(c>>12&0x0f00)|
									 (c>> 8&0x00f0)|
									 (c>> 4&0x000f);
	}

	return;
}

void WriteScanLineA1R5G5B5( HTEXTURE Dst, int Dx, int Dy, int Pixels )
{
	COLOR32 *pTmp = (COLOR32 *) Tmp->Bits;
	COLOR16 *pDst = (COLOR16 *) Dst->Bits;
	COLOR16 c;

	for( int x = 0; x < Pixels; x++ )
	{
		c = pTmp[x];

		pDst[Dx+x] = (COLOR16)(c>>16&0x8000)|
									 (c>> 9&0x7c00)|
									 (c>> 6&0x03e0)|
									 (c>> 3&0x001f);
	}

	return;
}

void WriteScanLineX1R5G5B5( HTEXTURE Dst, int Dx, int Dy, int Pixels )
{
	COLOR32 *pTmp = (COLOR32 *) Tmp->Bits;
	COLOR16 *pDst = (COLOR16 *) Dst->Bits;
	COLOR16 c;

	for( int x = 0; x < Pixels; x++ )
	{
		c = pTmp[x];

		pDst[Dx+x] = (COLOR16)(c>> 9&0x7c00)|
									 (c>> 6&0x03e0)|
									 (c>> 3&0x001f);
	}

	return;
}


void WriteScanLineX0R5G6B5( HTEXTURE Dst, int Dx, int Dy, int Pixels )
{
	COLOR32 *pTmp = (COLOR32 *) Tmp->Bits;
	COLOR16 *pDst = (COLOR16 *) Dst->Bits;
	COLOR16 c;

	for( int x = 0; x < Pixels; x++ )
	{
		c = pTmp[x];

		pDst[Dx+x] = (COLOR16)(c>> 8&0xf800)|
									 (c>> 5&0x07e0)|
									 (c>> 3&0x001f);
	}

	return;
}

void WriteScanLineX0R8G8B8( HTEXTURE Dst, int Dx, int Dy, int Pixels )
{
	COLOR32 *pTmp = (COLOR32 *) Tmp->Bits;
	COLOR24 *pDst = (COLOR24 *) Dst->Bits;
	COLOR24 *p;
	COLOR32 c;

	for( int x = 0; x < Pixels; x++ )
	{
		c = pTmp[x];
		p = &pDst[Dx+x];

		p->Red   = (char) c >> 16;
		p->Green = (char) c >> 8;
		p->Blue  = (char) c >> 0;
	}

	return;
}

SCANLINEFUNCTION ReadScanLine = ReadScanLineUnknown;
SCANLINEFUNCTION WriteScanLine = WriteScanLineUnknown;

// Copy a texture one scanline at a time.  If the texel formats of the source
// and destination differ, the scanline is converted to 32-bit A8R8G8B8 and then
// converted to the destination format.

// TODO: Image resizing.

void CopyTexture( HTEXTURE Dst, int Dx, int Dy, int Dw, int Dh, HTEXTURE Src, int Sx, int Sy )
{
	static SCANLINEFUNCTION table1[9] = { ReadScanLineUnknown,			// TEXELFMT_UNKNOWN
													  ReadScanLineA8R8G8B8,			// TEXELFMT_A8R8G8B8
													  ReadScanLineA8R8G8B8,			// TEXELFMT_X8R8G8B8
													  ReadScanLineA4R4G4B4,			// TEXELFMT_A4R4G4B4
													  ReadScanLineX4R4G4B4,			// TEXELFMT_X4R4G4B4
													  ReadScanLineA1R5G5B5,			// TEXELFMT_A1R5G5B5
													  ReadScanLineX1R5G5B5,			// TEXELFMT_X1R5G5B5
													  ReadScanLineX0R5G6B5,			// TEXELFMT_X0R5G6B5,
													  ReadScanLineX0R8G8B8, };		// TEXELFMT_X8R8G8B8
	
	static SCANLINEFUNCTION table2[9] = { WriteScanLineUnknown,			// TEXELFMT_UNKNOWN
													  WriteScanLineA8R8G8B8,		// TEXELFMT_A8R8G8B8
													  WriteScanLineA8R8G8B8,		// TEXELFMT_X8R8G8B8
													  WriteScanLineA4R4G4B4,		// TEXELFMT_A4R4G4B4
													  WriteScanLineX4R4G4B4,		// TEXELFMT_X4R4G4B4
													  WriteScanLineA1R5G5B5,		// TEXELFMT_A1R5G5B5
													  WriteScanLineX1R5G5B5,		// TEXELFMT_X1R5G5B5
													  WriteScanLineX0R5G6B5,		// TEXELFMT_X0R5G6B5,
													  WriteScanLineX0R8G8B8, };	// TEXELFMT_X8R8G8B8

	ReadScanLine = table1[Src->Format];
	WriteScanLine = table2[Dst->Format];

	int ScanLines = Dh;

	if( ScanLines > Dst->Height - Dy ) ScanLines = Dst->Height - Dy;
	if( ScanLines > Src->Height - Sy ) ScanLines = Src->Height - Sy;

	int Width = Dw;

	if( Width > Dst->Width - Dx ) Width = Dst->Width - Dx;
	if( Width > Src->Width - Sx )	Width = Src->Width - Sx;

	if( Dst->Format == Src->Format )
	{
		for( int Y = 0; Y < ScanLines; Y++ )
			CopyScanLine( Dst, Y + Dy, Dx, Src, Y + Sy, Sx, Width );
	}
	else
	{
		Tmp = CreateTexture( Width, 1, TEXELFMT_A8R8G8B8 );

		if( Tmp )
		{
			for( int Y = 0; Y < Dh; Y++ )
			{
				ReadScanLine( Src, Sx, Y + Sy, Width );
				WriteScanLine( Dst, Dx, Y + Dy, Width );
			}
		}

		DeleteTexture( Tmp );
	}

	return;
}

#endif