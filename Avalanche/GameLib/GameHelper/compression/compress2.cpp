#include "GameHelper/GameHelperPCH.h"
#include "EngineHelper/Hash.h"

//#ifdef GAMECUBE
#if 1

/**********************************************************************
**
** compress.c - Vapor data compressor by Rob Nelson 3/22/94
**
** This program utilizes "chisel" and "compressMethodEclipse" type compression.
** Data will be compressed both ways and the most optimal output
** will be used.  Note that the data output isn't identical to 
** either of these programs because it has been rearranged to be
** faster to decompress on the 68000.
**
** Revision 1.0 (3/22/94) - Initial release
** Revision 1.1 (3/23/94) - Fixed patterns over midpoint boundary
** Revision 1.2 (3/24/94) - Constant screen output so you can abort.
** Revision 1.3 (4/10/94) - Improved compression speed.
** Revision 1.4 (6/05/94) - Commands may be specific per header
** Revision 1.5 (7/08/94) - Labels may start with an underscore
**                          Sections of 0 size will display.
** Revision 1.6 (7/10/94) - Added compressed size and ratio to output
** Revision 1.7 (8/17/94) - Labels may start with a period.
** Revision 1.8 (11/15/94)- Added CNOP 0,2 as align option.
** Revision 1.9m(12/1/94) - Switched to 32bit (mars)
** Revision 1.10m (1/17/95) - Asterisk as comment allowed (mars)
**
** BIG CHANGE:  First word is now compressed length, not uncompressed
** lenght  - MWC  1/18/96
**
** Revision 2.0 (2/6/2002) - changes interface functions.  added
**        new header which contains compressed and uncompressed sizes
**        and checksums.  added checksum calculation to compress code
**        and checksum checking on decompress code.
**            -Rob Nelson (whew, this thing is 8 year old!)
**
** Revision 3.0 (7/11/2002) - switched to quit using the old limited
**        compression.  Now can use gzip (zlib) or bzip2 (libbzip2) algorithms
**        for far superior compression ratios and speed...
**
**********************************************************************/
#if defined( WIN_32 )
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
#endif

	#define VCHeap_Allocate(h,s) memAlloc(s)
	#define VCHeap_Free(p) memFree(p)
	#define VCMem_Set(p,b,l) memset(p,b,l)
	#define VCMem_Compare(a,b,l) memcmp(a,b,l)
	#define VCMem_Copy(a,b,l) memcpy(a,b,l)
	#define VCMEM_ZERO(p) memset((p),0,sizeof(*(p)))
	#define VCASSERT(c,s) { if ( !(c) ) { fprintf( stderr, "ASSERT FAILED %s(%d): '%s' %s\n", __FILE__, __LINE__, #c, s ); exit( 1 ); } }
	#define VCASSERT_POINTER_RETURN_ZERO(p) VCASSERT((p)!=NULL,"POINTER NULL")
//	#define COMPRESS_KEEP_STATS
//	#define COMPRESS_DEBUG_LOG

#include "compress2.h"
#include "zlib.h"



//===== DEFINES =============================================================

#define COMPRESS_HEADER_ID		"CMP1"

#define PACKET_RUN_LENGTH_BITS		5
#define PACKET_RUN_LENGTH_MIN		3
#define PACKET_RUN_LENGTH_MAX		33
#define PACKET_RUN_ZERO_LENGTH_BITS	6
#define PACKET_RUN_ZERO_LENGTH_MIN	1
#define PACKET_RUN_ZERO_LENGTH_MAX	( (1<<PACKET_RUN_ZERO_LENGTH_BITS) - 1 + PACKET_RUN_ZERO_LENGTH_MIN )
#define PACKET_PATTERN_LENGTH_BITS	3
#define PACKET_PATTERN_LENGTH_MIN	3
#define PACKET_PATTERN_LENGTH_MAX	( (1<<PACKET_PATTERN_LENGTH_BITS) - 1 + PACKET_PATTERN_LENGTH_MIN )
#define PACKET_PATTERN_OFFSET_MAX	4096
#define PACKET_UNIQUE_LENGTH_BITS	5
#define PACKET_UNIQUE_LENGTH_MIN	1
#define PACKET_UNIQUE_LENGTH_MAX	( (1<<PACKET_UNIQUE_LENGTH_BITS) - 1 + PACKET_UNIQUE_LENGTH_MIN )


//===== TYPES ===============================================================

typedef struct _COMPRESS_HEADER
{
	U8  id[4];
	S32	uncompressedSize;
	S32	compressedSize;
	U32	uncompressedChecksum;
	U32	compressedChecksum;
}
COMPRESS_HEADER;


//===== VARIABLES ===========================================================

/*static struct {
	S32		curChar;
	S32*	firstPos;
	S32*	nextChar;
	S32		curEnd;
	S32		curpos;
	S32		scanBufLen;
	S32		compBufLen;
} Compress;*/

//===========================================================================

#if defined( COMPRESS_KEEP_STATS )

static struct {
	S32		numUniquePackets;
	S32		numRunPackets;
	S32		numRunZeroPackets;
	S32		numPatternPackets;

	S32		numUniqueBytes;
	S32		numRunBytes;
	S32		numRunZeroBytes;
	S32		numPatternBytes;

	S32		numUniqueMaxed;
	S32		numRunMaxed;
	S32		numRunZeroMaxed;
	S32		numPatternMaxed;

} CompressStats;


#define COMPRESS_STAT( stat, bytes, max ) \
{ \
	CompressStats.num##stat##Packets += 1; \
	CompressStats.num##stat##Bytes += bytes; \
	if ( (bytes) >= (max) ) { \
		CompressStats.num##stat##Maxed += 1; \
	} \
}

#define SHOW_STAT( stat ) \
{ \
	printf( "COMP_STAT: %-15s packets=%6u bytes=%6u ave=%3d maxed=%6u(%3d%%)\n", \
		#stat, \
		CompressStats.num##stat##Packets, \
		CompressStats.num##stat##Bytes, \
		CompressStats.num##stat##Bytes / ( CompressStats.num##stat##Packets > 0 ? CompressStats.num##stat##Packets : 1 ), \
		CompressStats.num##stat##Maxed, \
		CompressStats.num##stat##Maxed * 100 / ( CompressStats.num##stat##Packets > 0 ? CompressStats.num##stat##Packets : 1 ) \
	); \
}

#else

#define COMPRESS_STAT( stat, bytes, max ) {}

#endif

//===========================================================================

#if defined( COMPRESS_DEBUG_LOG )

#define COMPRESS_LOG( file, string, p1, p2 ) fprintf( file, string, p1, p2 )

#else

#define COMPRESS_LOG( file, string, p1, p2 ) {}

#endif

//===========================================================================

static void* compressAlloc( S32 size )
{
	void* ptr = NULL;

	ptr = VCHeap_Allocate( VCLibrary_GetGlobalHeap(), size );
	if ( ptr == NULL )
	{
		ASSERTS( FALSE, "compress alloc failed" );
		return NULL;
	}

	VCMem_Set( (U8*)ptr, 0, size );

	return ptr;
}

//===========================================================================

static void compressFree( void* ptr )
{
	if ( ptr != NULL )
	{
		VCHeap_Free( ptr );
	}
}

//===========================================================================

//static void compressAdvancePos( U8* src, S32 count )
//{
//	while ( count-- )
//	{
//		Compress.nextChar[ Compress.curpos & ( PACKET_PATTERN_OFFSET_MAX - 1 ) ] = Compress.firstPos[ Compress.curChar ];
//		Compress.firstPos[ Compress.curChar ] = Compress.curpos;
//		Compress.curChar = src[ ++Compress.curpos ];
//	}
//}

//===========================================================================

//static int compressFindBestRun( U8* src, int pos, int max )
//{
//	int count = 1;
//	U8 runbyte = src[ pos ];
//	for ( pos++ ; (count < max) && (pos < Compress.scanBufLen) && (src[ pos ] == runbyte) ; pos++, count++ ) ;
//	return count;
//}

//===========================================================================

//static S32 compressMethodEclipse( U8* src, U8* dest )
//{
//	S32 destlen, runcount, patterncount, patternoffset, runzerocount;
//	S32 uniquecount;
//	S32 i;
//	#if defined( COMPRESS_DEBUG_LOG )
//	FILE* fp = fopen( "comp.txt", "wt" );
//	#endif
//
//	Compress.curEnd = Compress.scanBufLen;
//
//	for ( i = 0; i < 256 ; Compress.firstPos[ i++ ] = ~0 ) ;
//
//	destlen = 0;
//	Compress.curpos = 0;
//	Compress.curChar = src[0];
//
//	runcount     = 0;
//	patterncount = 0;
//	runzerocount = 0;
//
//	while ( Compress.curpos < Compress.curEnd )
//	{
//		uniquecount = 0;
//		while ( Compress.curpos < Compress.curEnd && uniquecount < PACKET_UNIQUE_LENGTH_MAX )
//		{
//			runcount     = compressFindBestRun( src, Compress.curpos, PACKET_RUN_ZERO_LENGTH_MAX );
//			runzerocount = ( Compress.curChar == 0 ) ? runcount : 0;
//			if ( runcount > PACKET_RUN_LENGTH_MAX ) runcount = PACKET_RUN_LENGTH_MAX;
//
//			// try to save some time - if we know this packet excels, skip the pattern match.
//			if ( runzerocount >= PACKET_RUN_LENGTH_MAX && runzerocount >= PACKET_PATTERN_LENGTH_MAX ) {	runcount = patterncount = 0; break;	}
//			if ( runcount >= PACKET_PATTERN_LENGTH_MAX ) { patterncount = 0; break;	}
//
//			patterncount = compressFindBestPattern( src, &patternoffset );
//
//			if ( runzerocount >= PACKET_RUN_ZERO_LENGTH_MIN || runcount >= PACKET_RUN_LENGTH_MIN || patterncount >= PACKET_PATTERN_LENGTH_MIN ) break;
//
//			uniquecount++;
//			compressAdvancePos(src, 1);
//		}
//
//		if ( uniquecount > 0 )
//		{
//			COMPRESS_STAT( Unique, uniquecount, PACKET_UNIQUE_LENGTH_MAX );
//			COMPRESS_LOG( fp, "UNIQUE: L=%d\n", uniquecount, 0 );
//			dest[destlen++] = (U8) (0x20 | (PACKET_UNIQUE_LENGTH_MAX - uniquecount));
//			for (i=Compress.curpos-uniquecount; i<Compress.curpos; i++)
//				dest[destlen++] = src[i];
//		}
//
//		if ( patterncount >= PACKET_PATTERN_LENGTH_MIN && patterncount > runcount && patterncount > runzerocount )
//		{
//			COMPRESS_STAT( Pattern, patterncount, PACKET_PATTERN_LENGTH_MAX );
//			COMPRESS_LOG( fp, "PATTERN: L=%d O=%d\n", patterncount, patternoffset );
//			dest[destlen++] = (U8) (0x80 | ((patternoffset & 0xF00) >> 5) | (PACKET_PATTERN_LENGTH_MAX - patterncount));
//			dest[destlen++] = (U8) (patternoffset & 0xFF);
//			compressAdvancePos(src, patterncount);
//		}
//		else if ( runzerocount >= PACKET_RUN_ZERO_LENGTH_MIN )
//		{
//			COMPRESS_STAT( RunZero, runzerocount, PACKET_RUN_ZERO_LENGTH_MAX );
//			COMPRESS_LOG( fp, "RUN_ZERO: L=%d\n", runzerocount, 0 );
//			dest[ destlen++ ] = (U8) (0x40 | ( PACKET_RUN_ZERO_LENGTH_MAX - runzerocount ));
//			compressAdvancePos(src, runzerocount);
//		}
//		else if ( runcount >= PACKET_RUN_LENGTH_MIN )
//		{
//			COMPRESS_STAT( Run, runcount, PACKET_RUN_LENGTH_MAX );
//			COMPRESS_LOG( fp, "RUN: L=%d B=%d\n", runcount, Compress.curChar );
//			dest[destlen++] = (U8) (0x00 | (PACKET_RUN_LENGTH_MAX + 1 - runcount));
//			dest[destlen++] = (U8)Compress.curChar;
//			compressAdvancePos(src, runcount);
//		}
//		if (Compress.curpos == Compress.curEnd)
//		{
//			dest[destlen++] = 0;
//			Compress.curEnd = Compress.scanBufLen;
//		}
//	}
//
//	#if defined( COMPRESS_DEBUG_LOG )
//	fclose( fp );
//	#endif
//
//	return(destlen);
//}

//===========================================================================

//static S32 compressDecompressEclipse( U8* src, U8* dest, S32 compSize )
//{
//	U8	tmp;
//	S32	offset;
//	S32	length;
//	U8*	pattern;
//	U8*	orgDest = dest;
//
//	#if defined( COMPRESS_DEBUG_LOG )
//	FILE* fp = fopen( "decomp.txt", "wt" );
//	#endif
//
//	while (1)
//	{
//		tmp = *src++;
//
//		if (tmp & 0x80)
//		{
//			length = tmp & 0x07;
//			offset = ( ( (S32)tmp & 0x78 ) << 5 ) | ( *src++ );
//			pattern = dest - offset - 1;
//			COMPRESS_LOG( fp, "PATTERN: L=%d O=%d\n", PACKET_PATTERN_LENGTH_MAX - length, offset );
//			for ( ; length < PACKET_PATTERN_LENGTH_MAX ; length++ )
//			{
//				*dest++ = *pattern++;
//			}
//		}
//		else if (tmp & 0x40)
//		{
//			length = tmp & 0x3f;
//			COMPRESS_LOG( fp, "RUN_ZERO: L=%d\n", PACKET_RUN_ZERO_LENGTH_MAX - length, 0 );
//			for ( ; length < PACKET_RUN_ZERO_LENGTH_MAX ; length++ ) { *dest++ = 0; }
//		}
//		else if (tmp & 0x20)
//		{
//			length = tmp & 0x1f;
//			COMPRESS_LOG( fp, "UNIQUE: L=%d\n", PACKET_UNIQUE_LENGTH_MAX - length, 0 );
//			for ( ; length < PACKET_UNIQUE_LENGTH_MAX ; length++ )
//			{
//				*dest++ = *src++;
//			}
//		}
//		else if (tmp)
//		{
//			length = tmp & 0x1f;
//			COMPRESS_LOG( fp, "RUN: L=%d B=%d\n", PACKET_RUN_LENGTH_MAX + 1 - length, *src );
//			for ( ; length < PACKET_RUN_LENGTH_MAX + 1 ; length++ )
//			{
//				*dest++ = *src;
//			}
//			src++;
//		}
//		else
//		{
//			break;
//		}
//	}
//
//	#if defined( COMPRESS_DEBUG_LOG )
//	fclose( fp );
//	#endif
//
//	return dest - orgDest;	//-- return actual length.
//}

//===========================================================================

/*
static S32 compressMethodRLE( U8* src, U8* dest )
{
	int i = 0;
	int last = -1;
	int uniquestart = -1;
	int run;
	int unique = 0;
	U8* orgDest = dest;

	#if defined( COMPRESS_DEBUG_LOG )
	FILE* fp = fopen( "comp.txt", "wt" );
	#endif

	while ( i < Compress.scanBufLen )
	{
		if ( i + 1 < Compress.scanBufLen && src[i] == 0 && src[i+1] == 0 )
		{
			run = 2;

			if ( unique ) {
				COMPRESS_LOG( fp, "UNI C=%d\n", unique, 0 );
				*dest++ = 0x00 | ( unique - 1 );
				while ( unique > 0 ) { *dest++ = src[ uniquestart++ ]; unique--; }
			}

			for ( i = i + run ; i < Compress.scanBufLen && src[i] == 0 && run < 128 ; i++, run++ ) ;
			COMPRESS_LOG( fp, "RUNZERO C=%d\n", run, 0 );
			*dest++ = 0x80 | ( run - 2 );
		}
		else
		{
			if ( unique == 0 ) uniquestart = i;
			unique++;
			i++;
			if ( unique == 128 || i >= Compress.scanBufLen ) {
				COMPRESS_LOG( fp, "UNI C=%d\n", unique, 0 );
				*dest++ = 0x00 | ( unique - 1 );
				while ( unique > 0 ) { *dest++ = src[ uniquestart++ ]; unique--; }
			}
		}
	}

	*dest++ = 0xFF;

	#if defined( COMPRESS_DEBUG_LOG )
	fclose( fp );
	#endif

	return dest - orgDest;
}
*/

//===========================================================================

/*
static S32 compressDecompressRLE( U8* src, U8* dest, S32 compSize )
{
	U8* orgDest = dest;
	int count;

	#if defined( COMPRESS_DEBUG_LOG )
	FILE* fp = fopen( "decomp.txt", "wt" );
	#endif

	while ( 1 )
	{
		U8 b = *src++;
		if ( b & 0x80 )
		{
			if ( b == 0xFF )
				break;
			count = ( b & 0x7F ) + 2;
			COMPRESS_LOG( fp, "RUNZERO C=%d\n", count, 0 );
			while ( count-- ) { *dest++ = 0; }
		}
		else
		{
			count = b + 1;
			COMPRESS_LOG( fp, "UNI C=%d\n", count, 0 );
			while ( count-- ) { *dest++ = *src++; }
		}
	}

	#if defined( COMPRESS_DEBUG_LOG )
	fclose( fp );
	#endif

	return dest - orgDest;
}
*/

//===========================================================================


static S32 compressMethodZlib( U8* src, U8* dest, S32 srcSize, S32 destBufferSize )
{
	uLongf compLen = destBufferSize;
	int err = compress2( dest, &compLen, src, srcSize, Z_BEST_COMPRESSION );
	if ( err != Z_OK )
	{
		ASSERTS( FALSE, "compress failed" );
		return 0;
	}
	return compLen;
}

//===========================================================================

static S32 compressDecompressZlib( U8* src, U8* dest, S32 compSize, s32 destBufferSize )
{
	unsigned int decompLen = destBufferSize;

	int err = uncompress( dest, &decompLen, src, compSize );
	if ( err != Z_OK )
	{
		ASSERTS( 0, "decompress failed" );
		//VCASSERT( FALSE, "decompress failed" );
		return 0;
	}
	return decompLen;
}


//===========================================================================
/*
static S32 compressMethodBzip2( U8* src, U8* dest, S32 srcSize, S32 destBufferSize )
{
	unsigned int compLen = destBufferSize;
	int err = BZ2_bzBuffToBuffCompress( dest, &compLen, src, srcSize, 9, 0, 30 );
	if ( err != BZ_OK )
	{
		VCASSERT( FALSE, "compress failed" );
		return 0;
	}
	return compLen;
}

//===========================================================================

static S32 compressDecompressBzip2( U8* src, U8* dest, S32 srcSize, S32 destBufferSize )
{
	unsigned int decompLen = destBufferSize;
	int err = BZ2_bzBuffToBuffDecompress( dest, &decompLen, src, srcSize, 0, 0 );
	if ( err != BZ_OK )
	{
		VCASSERT( FALSE, "decompress failed" );
		return 0;
	}
	return decompLen;
}
*/
//===========================================================================

S32 COMPRESS_CompressBuffer( U8* srcData, S32 srcSize, U8* compBuffer, S32 compBufferSize )
{
	COMPRESS_HEADER*	hdr;
	S32					compLength;

	ASSERT( srcData );
	ASSERT( compBuffer );

	//-- keep a pointer to where the compressed header will go, and mode the compBuffer just after it.
	hdr        = ( COMPRESS_HEADER* ) compBuffer;
	compBuffer = compBuffer + sizeof(COMPRESS_HEADER);

	#if defined( COMPRESS_KEEP_STATS )
	VCMEM_ZERO( &CompressStats );
	#endif

	//-- compress
//	compLength = compressMethodEclipse( srcData, compBuffer );
//	compLength = compressMethodRLE( srcData, compBuffer );
	compLength = compressMethodZlib( srcData, compBuffer, srcSize, compBufferSize - sizeof(COMPRESS_HEADER) );
//	compLength = compressMethodBzip2( srcData, compBuffer, srcSize, compBufferSize - sizeof(COMPRESS_HEADER) );

	//-- setup header
	VCMEM_ZERO( hdr );
	VCMem_Copy( hdr->id, COMPRESS_HEADER_ID, sizeof(hdr->id) );
	hdr->uncompressedSize     = srcSize;
	hdr->uncompressedChecksum = Hash::CalcChecksum( srcData, srcSize );
	hdr->compressedSize       = compLength;
	hdr->compressedChecksum   = Hash::CalcChecksum( compBuffer, compLength );

	#if defined( COMPRESS_KEEP_STATS ) && defined( TARGET_PC_WINCON )
	SHOW_STAT( Unique );
	SHOW_STAT( Run );
	SHOW_STAT( RunZero );
	SHOW_STAT( Pattern );
	#endif

//	// free up mem
//	compressFree( Compress.firstPos   );
//	compressFree( Compress.nextChar   );
	return compLength + sizeof( COMPRESS_HEADER );
}

///////////////////////////////////////////////////////////////////////////
//
// U8* COMPRESS_DecompressBuffer( U8* src )
//
//	pass a source buffer to decompress, this routine will memory allocate
// a decompression buffer, and will return a pointer to it.
//
//	the caller must remember to free the source buffer, and the 
// returned output buffer.
//
// Returns NULL if the decompression fails for any of these reasons:
//		* header doens't match
//		* checksum of compressed data doesn't match header checksum
//		* checksum of decompressed data doesn't match header checksum
//		* memory alloc fails for decompressed data
//
///////////////////////////////////////////////////////////////////////////
S32 length;

U8* COMPRESS_DecompressBuffer( U8* compBuffer, U8 *decompBuffer )
{
	COMPRESS_HEADER*	hdr;
	U32		compressedChecksum;
	U32		decompChecksum;
	//U8*		decompBuffer;
	S32		decompSize;
	static S32 PS2Junk;

	//-- Verify
	//VCASSERT_POINTER_RETURN_ZERO( compBuffer );
	ASSERT( compBuffer );

	//-- get pointer to header and move the compBuffer pointer beyond it
	hdr        = ( COMPRESS_HEADER* ) compBuffer;
	compBuffer = compBuffer + sizeof(COMPRESS_HEADER);

	//-- make sure ID matches
	if ( VCMem_Compare( hdr->id, COMPRESS_HEADER_ID, sizeof(hdr->id) ) != 0 )
	{
		ASSERTS( FALSE, "compressed ID doesn't match" );
		return NULL;
	}

	//-- Make sure the compressed checksum matches...
	compressedChecksum = Hash::CalcChecksum( compBuffer, hdr->compressedSize );
	if ( compressedChecksum != hdr->compressedChecksum )
	{
		ASSERTS( FALSE, "compressed checksum doesn't match data" );
		return NULL;
	}

/*	//-- Allocate buffer for decompressed data
	decompBuffer = (unsigned char * )compressAlloc( hdr->uncompressedSize );
	if ( decompBuffer == NULL )
	{
		VCASSERT( FALSE, "out of memory" );
		return NULL;
	}*/

//	decompSize = compressDecompressEclipse( compBuffer, decompBuffer, hdr->compressedSize );
//	decompSize = compressDecompressRLE( compBuffer, decompBuffer, hdr->compressedSize );
	decompSize = compressDecompressZlib( compBuffer, decompBuffer, hdr->compressedSize, hdr->uncompressedSize );
//	decompSize = compressDecompressBzip2( compBuffer, decompBuffer, hdr->compressedSize, hdr->uncompressedSize );

	if ( decompSize != hdr->uncompressedSize )
	{
		ASSERTS( FALSE, "decompressed file size doesn't match header" );
		compressFree( decompBuffer );
		return NULL;
	}

	//--- PS2 SN builds have some link issue
	PS2Junk++;
	if( PS2Junk != 0 ) PS2Junk++;

	//-- verify the checksum of the decompressed buffer
	decompChecksum = Hash::CalcChecksum( decompBuffer, hdr->uncompressedSize );
	
	if ( decompChecksum != hdr->uncompressedChecksum )
	{
		ASSERTS( FALSE, "uncompressed checksum doesn't match data" );
		compressFree( decompBuffer );
		return NULL;
	}
	return decompBuffer;
}

//===========================================================================

BOOL COMPRESS_IsCompressedFile( U8* compBuffer )
{
	COMPRESS_HEADER*	hdr;

	//-- Verify
	if ( compBuffer == NULL )
	{
		ASSERTS( FALSE, "bad ptr" );
		return 0;
	}

	//-- Get pointer
	hdr = ( COMPRESS_HEADER* )compBuffer;

	//-- make sure ID matches
	if ( VCMem_Compare( hdr->id, COMPRESS_HEADER_ID, sizeof(hdr->id) ) != 0 )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

//===========================================================================

S32 COMPRESS_GetUncompressedSize( U8* compBuffer )
{
	COMPRESS_HEADER*	hdr;

	//-- Verify
	if ( compBuffer == NULL )
	{
		ASSERTS( FALSE, "bad ptr" );
		return 0;
	}

	//-- Get pointer
	hdr = ( COMPRESS_HEADER* )compBuffer;

	//-- make sure ID matches
	if ( VCMem_Compare( hdr->id, COMPRESS_HEADER_ID, sizeof(hdr->id) ) != 0 )
	{
		ASSERTS( FALSE, "compressed ID doesn't match" );
		return 0;
	}

	return hdr->uncompressedSize;
}

//===========================================================================

#endif // GAMECUBE
