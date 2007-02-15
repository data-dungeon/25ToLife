#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#ifdef ASKLFJDSFHDFH

#include "Layers/LayersStruct.h"

typedef unsigned int COLOR32;
typedef unsigned short COLOR16;
typedef struct
{
	unsigned char Red;
	unsigned char Green;
	unsigned char Blue;
}
COLOR24;

typedef enum
{
	TEXELFMT_UNKNOWN = 0,
	TEXELFMT_A8R8G8B8,
	TEXELFMT_X8R8G8B8,
	TEXELFMT_A4R4G4B4,
	TEXELFMT_X4R4G4B4,
	TEXELFMT_A1R5G5B5,
	TEXELFMT_X1R5G5B5,
	TEXELFMT_X0R5G6B5,
	TEXELFMT_X0R8G8B8,
}
TEXELFORMAT;

#define TEXTUREFLAG_ASSUMED (1<<0)

typedef struct
{
	TEXELFORMAT Format;
	int	Flags;
	int	Width;
	int	Height;
	int	BPP;
	int	Pitch;
	int	Stride;
	int	Size;
	char *Bits;
}
TEXTURE, *HTEXTURE;

typedef void (*SETTEXELFUNCTION)(int, int, COLOR32 );
typedef COLOR32 (*GETTEXELFUNCTION)(int, int);

extern SETTEXELFUNCTION SetTexel[4];
extern GETTEXELFUNCTION GetTexel[4];

extern SETTEXELFUNCTION SetTexelSafe[4];
extern GETTEXELFUNCTION GetTexelSafe[4];

COLOR32 ColorBlend( COLOR32 A, COLOR32 B );
COLOR32 ColorBlendA( COLOR32 A, COLOR32 B, int Alpha );
COLOR32 ColorAdd( COLOR32 A, COLOR32 B );
COLOR32 ColorSub( COLOR32 A, COLOR32 B );
int GetBPPFromTexelFormat( TEXELFORMAT Format );
TEXELFORMAT GetTexelFormatFromBPP( int BPP );
void SetTextureAlignment( int Alignment );
int GetTextureAlignment( void );
HTEXTURE CreateTexture( int Width, int Height, TEXELFORMAT Format );
HTEXTURE CreateTexturePow2( int Width, int Height, TEXELFORMAT Format );
HTEXTURE AssumeTexture( char *Bits, int Width, int Height, int Pitch, TEXELFORMAT Format );
HTEXTURE DeleteTexture( HTEXTURE Texture );
char *GetScanLineAddress( HTEXTURE Texture, int ScanLine, int X );
void SelectTexture( int Channel, HTEXTURE Texture );
void CopyTexture( HTEXTURE Dst, int Dx, int Dy, int Dw, int Dh, HTEXTURE Src, int Sx, int Sy );
void StretchTexture( HTEXTURE Dst, int Dx, int Dy, int Dw, int Dh, HTEXTURE Src, int Sx, int Sy, int Sw, int Sh );
void CopyToHardwareTexture( t_Handle Handle, HTEXTURE Texture );

#endif

#endif // __TEXTURE_H__