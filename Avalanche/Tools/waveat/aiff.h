//===========================================================================
//==
//== AIFF loader routines
//==
//===========================================================================

#ifndef _AIFF_H
#define _AIFF_H

#include <windows.h>
#include <stdio.h>

class AIFF
{
public:
	AIFF();
	~AIFF();
	bool Open( const char* filename );
	bool LoadFile( const char* fileName, unsigned int* dataLen, WAVEFORMATEX** wfx, unsigned char** dataPtr );

private:
	FILE*	m_fp;
	char	m_filename[ _MAX_PATH ];

	unsigned long GetU32( void* data );
	unsigned short GetU16( void* data );
	double UnsignedToFloat( unsigned long u );
	double GetExtended( void* data );

	bool ReadChunk( const char* ChunkID, int& ChunkLen, char*& ChunkData, int Index );
	bool GetFormat( short& numChannels, unsigned long& numSampleFrames, short& sampleSize, double& sampleRate );
	bool GetData( int& DataLen, char*& DataPtr );
};

#endif //_AIFF_H