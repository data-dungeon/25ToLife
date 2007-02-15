//===========================================================================
//=
//= compress.h - compression and decompression routines
//=
//===========================================================================

#ifndef _COMPRESS_H
#define _COMPRESS_H

	typedef unsigned char U8;
	typedef   signed int S32;
	typedef unsigned int U32;
	typedef		 int BOOL;

S32 COMPRESS_CompressBuffer( U8* srcData, S32 srcSize, U8* compBuffer, S32 compBufferSize );
U8* COMPRESS_DecompressBuffer( U8* compBuffer, U8 *decompBuffer );
BOOL COMPRESS_IsCompressedFile( U8* compBuffer );
S32 COMPRESS_GetUncompressedSize( U8* compBuffer );

#endif //_COMPRESS_H