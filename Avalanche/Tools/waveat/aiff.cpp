//===========================================================================
//==
//== AIFF loader routines
//==
//===========================================================================

#include <math.h>
#include "aiff.h"

//===========================================================================

AIFF::AIFF()
{
	memset( this, 0, sizeof(*this) );
}

//===========================================================================

AIFF::~AIFF()
{
	if ( m_fp != NULL ) fclose( m_fp );
}

//===========================================================================

unsigned long AIFF::GetU32( void* data )
{
	unsigned char* d = (unsigned char*) data;
	return ( (unsigned long)d[0] << 24 ) + ( (unsigned long)d[1] << 16 ) + ( (unsigned long)d[2] << 8 ) + ( (unsigned long)d[3] );
}

//===========================================================================

unsigned short AIFF::GetU16( void* data )
{
	unsigned char* d = (unsigned char*) data;
	return ( (unsigned short)d[0] << 8 ) + ( (unsigned short)d[1] );
}

//===========================================================================

double AIFF::UnsignedToFloat( unsigned long u )
{
	return ( ( (double) ( (long) ( u - 2147483647L - 1 ) ) ) + 2147483648.0 );
}

//===========================================================================

// Convert 10-byte IEEE-754 in big endian to double...
double AIFF::GetExtended( void* data )
{
	unsigned char* d = (unsigned char*) data;
	double			f;
	int				expon;
	unsigned long	hiMant, loMant;

	expon   = ( ( (unsigned long)d[0] & 0x7F ) << 8 ) | ( (unsigned long)d[1] & 0xFF );
	hiMant  = GetU32( d + 2 );
	loMant  = GetU32( d + 6 );

	if ( expon == 0 && hiMant == 0 && loMant == 0 )
	{
		f = 0;
	}
	else
	{
		if ( expon == 0x7FFF )		// Infinity or NaN
		{	
			f = HUGE;
		}
		else
		{
			expon -= 16383;
			f  = ldexp( UnsignedToFloat( hiMant ), expon -= 31 );
			f += ldexp( UnsignedToFloat( loMant ), expon -= 32 );
		}
	}

	if ( d[0] & 0x80 )
		return -f;
	else
		return f;
}

//===========================================================================

bool	AIFF::ReadChunk( const char* ChunkID, int& ChunkLen, char*& ChunkData, int Index )
{
	unsigned char chunkHdr[ 8 ];

	if ( m_fp == NULL )
		return false;

	fseek( m_fp, 12, SEEK_SET );

	while ( fread( chunkHdr, 1, 8, m_fp ) == 8 )
	{
		unsigned long len = GetU32( chunkHdr + 4 );

		if ( memcmp( chunkHdr, ChunkID, 4 ) == 0 && ( Index-- <= 0 ) )
		{
			char* data = (char*) malloc( len );

			if ( data == NULL )
				return false;

			if ( fread( data, 1, len, m_fp ) != len )
			{
				free( data );
				return false;
			}

			ChunkLen  = len;
			ChunkData = data;
			return true;
		}
		else
		{
			//-- skip this chunk
			fseek( m_fp, len, SEEK_CUR );
		}
	}

	return false;
}

//===========================================================================

bool AIFF::Open( const char* filename )
{
	char buf[ 12 ];

	strncpy( m_filename, filename, sizeof( m_filename ) );

	m_fp = fopen( filename, "rb" );

	if ( m_fp == NULL )
		return false;

	// read header
	if ( fread( buf, 1, 12, m_fp ) != 12 )
	{
		fclose( m_fp );
		m_fp = NULL;
		return false;
	}

	// make sure file is correct type
	if ( memcmp( buf, "FORM", 4 ) != 0 || memcmp( buf + 8, "AIFF", 4 ) != 0 )
	{
		fclose( m_fp );
		m_fp = NULL;
		return false;
	}

	return true;
}

//===========================================================================

bool AIFF::GetFormat( short& numChannels, unsigned long& numSampleFrames, short& sampleSize, double& sampleRate )
{
	char* data;
	int len;

	if ( ! ReadChunk( "COMM", len, data, 0 ) )
		return false;

	if ( len != 18 )
	{
		free( data );
		return false;
	}

	numChannels     = GetU16( data + 0 );
	numSampleFrames = GetU32( data + 2 );
	sampleSize      = GetU16( data + 6 );
	sampleRate      = GetExtended( data + 8 );

	free( data );

	return true;
}

//===========================================================================

bool AIFF::GetData( int& DataLen, char*& DataPtr )
{
	int len;
	char* data;

	if ( ! ReadChunk( "SSND", len, data, 0 ) )
		return false;

	unsigned long offset    = GetU32( data + 0 );
	unsigned long blockSize = GetU32( data + 4 );

	if ( len > 8 )
	{
		DataLen = len - 8;
		DataPtr = (char*) malloc( DataLen );
		memcpy( DataPtr, data + 8, DataLen );
	}
	else
	{
		DataPtr = NULL;
		DataLen = 0;
	}

	free( data );
	return ( DataLen > 0 );
}

//===========================================================================

bool AIFF::LoadFile( const char* fileName, unsigned int* dataLen, WAVEFORMATEX** wfx, unsigned char** dataPtr )
{
	int len;
	char* data;
	int i;

	short         numChannels     = 0;
	unsigned long numSampleFrames = 0;
	short         sampleSize      = 0;
	double        sampleRate      = 0;

	if ( ! Open( fileName ) )
		return false;
	if ( ! GetFormat( numChannels, numSampleFrames, sampleSize, sampleRate ) )
		return false;
	if ( ! GetData( len, data ) )
		return false;

	//-- convert endian
	switch ( sampleSize )
	{
		case 8:
			break;
		case 16:
			for ( i = 0 ; i < len / 2 ; i++ )
			{
				unsigned short v = GetU16( data + i * 2 );
				*(unsigned short*)( data + i * 2 ) = v;
			}
			break;
		case 32:
			for ( i = 0 ; i < len / 4 ; i++ )
			{
				unsigned long v = GetU32( data + i * 4 );
				*(unsigned long*)( data + i * 4 ) = v;
			}
			break;
		default:
			free( data );
			return false;
	}

	if ( wfx != NULL )
	{
		*wfx = (WAVEFORMATEX*) malloc( sizeof(WAVEFORMATEX) );
		memset( *wfx, 0x00, sizeof(WAVEFORMATEX) );
		(*wfx)->wFormatTag      = WAVE_FORMAT_PCM;
		(*wfx)->nChannels       = numChannels;
		(*wfx)->wBitsPerSample  = sampleSize;
		(*wfx)->nBlockAlign     = (*wfx)->nChannels * (*wfx)->wBitsPerSample / 8;
		(*wfx)->nSamplesPerSec  = ( unsigned long ) sampleRate;
		(*wfx)->nAvgBytesPerSec = (*wfx)->nSamplesPerSec * (*wfx)->nBlockAlign;
		(*wfx)->cbSize          = 0;
	}
	if ( dataPtr != NULL )
	{
		*dataPtr = (unsigned char*) data;
	}
	if ( dataLen != NULL )
	{
		*dataLen = len;
	}
	return true;
}

//===========================================================================
