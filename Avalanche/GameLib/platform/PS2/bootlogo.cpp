//============================================================================
//= BootLogo - display a splash logo before anything loads
//============================================================================

#include "platform/PlatformPCH.h"
#include "platform/PS2/DisplaySimple.h"
#include "platform/PS2/bootlogo.h"
#include "GameHelper/compression/compress2.h"

//============================================================================

void BootLogo::DisplayImage( PS2DisplaySimple* display, unsigned char* compressedImage )
{
	if ( COMPRESS_IsCompressedFile( compressedImage ) )
	{
		int rawSize = COMPRESS_GetUncompressedSize( (U8*)compressedImage );
		U8* rawData = new U8 [ rawSize ];
		COMPRESS_DecompressBuffer( compressedImage, rawData );

		int	width   = *(int*)( rawData + 0 );
		int	height  = *(int*)( rawData + 4 );
		int	colors  = *(int*)( rawData + 8 );
		U32*	palette = (U32*)( rawData + 12 );
		U8*	image   = (U8*)( palette + colors );

		// fix alpha on palette
		for ( int i = 0 ; i < colors ; i++ )
			palette[ i ] = ( palette[ i ] & 0x00FFFFFF ) | 0x80000000;

		// convert from paletted to raw 32-bit
		U32*	pixels  = new U32 [ width * height ];
		for ( int i = 0 ; i < width * height ; i++ )
			pixels[ i ] = palette[ image[ i ] ];

		display->ClearFrameBuffer( false );
		display->Blit( pixels, width, height, (display->Width() - width) / 2, (display->Height() - height) / 2, false );
		display->Flip();

		display->ClearFrameBuffer( false );
		display->Blit( pixels, width, height, (display->Width() - width) / 2, (display->Height() - height) / 2, false );
		display->Flip();

		delete pixels;
		delete rawData;
	}
}

//============================================================================
