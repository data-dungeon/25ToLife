//============================================================================
//==
//== Project: soundmod
//== File:    export.cpp
//== Desc:    Builds a composite sound bank file from wav/aiff files and
//==          control modules files.
//== Author:  Rob Nelson
//== Date:    2/6/2003
//== (c)      2003 Avalanche Software, Inc.
//==
//============================================================================

#include "soundmod.h"
#include "Game\Database\DBFile.h"

#define TOOL_HOST_BIG_ENDIAN  false

#define PAD(x,p) ( ( ( (x) % (p) ) == 0 ) ? (x) : ( (x) + (p) - ( (x) % (p) ) ) )

//============================================================================

int getGroupRefCount( ExpandableArray<Group> &groupArr )
{
	int groupRefCount = 0;
	for ( int i = 0 ; i < groupArr.Count() ; i++ )
		groupRefCount += groupArr[ i ].soundList.Count();
	return groupRefCount;
}

//============================================================================

int getSoundBankSize( Target& target, ExpandableArray<Source> &sourceArr )
{
	int soundBankSize = 0;
	for ( int i = 0 ; i < sourceArr.Count() ; i++ )
	{
		// Are we outputting to an external bank?
		if (target.external)
			soundBankSize += PAD( target.headerSize, target.pad );
		else
		{
			soundBankSize += PAD( target.headerSize, target.pad );
			soundBankSize += PAD( sourceArr[ i ].targetSize - target.headerSize, target.pad );
		}
	}
	return soundBankSize;
}

//============================================================================

int getStringTableSize( ExpandableArray<Sound> &soundArr, ExpandableArray<Group> &groupArr )
{
	int stringTableSize = 0;
	for ( int i = 0 ; i < soundArr.Count() ; i++ )
		stringTableSize += strlen( soundArr[ i ].name ) + 1;
	for ( i = 0 ; i < groupArr.Count() ; i++ )
		stringTableSize += strlen( groupArr[ i ].name ) + 1;
	stringTableSize = PAD( stringTableSize, 4 );
	return stringTableSize;
}

//============================================================================

int getDBLHeaderSize( Target& target )
{
	if ( exportDBL )
	{
		int size = sizeof(ts_DBFileHdr) + sizeof(ts_DBChunkHdr);
		size = PAD( size, target.pad );
		return size;
	}
	else
	{
		return 0;
	}
}

//============================================================================

int getHeaderSize(
	Target									&target,
	ExpandableArray<Source>				&sourceArr,
	ExpandableArray<Sound>				&soundArr,
	ExpandableArray<Group>				&groupArr )
{
	int size = 0;
	size += sizeof( SoundModHeaderDef );
	size += sizeof( SourceDef ) * sourceArr.Count();
	size += sizeof( SoundDef  ) * soundArr.Count();
	size += sizeof( GroupDef  ) * groupArr.Count();
	size += sizeof( SoundDef* ) * getGroupRefCount( groupArr );
	size += getStringTableSize( soundArr, groupArr );
	size = PAD( size, target.pad );
	return size;
}

//============================================================================

int computeExportFileSize(
	Target									&target,
	ExpandableArray<Source>				&sourceArr,
	ExpandableArray<Sound>				&soundArr,
	ExpandableArray<Group>				&groupArr )
{
	int size = 0;
	size += getDBLHeaderSize( target );
	size += getHeaderSize( target, sourceArr, soundArr, groupArr );
	size += getSoundBankSize( target, sourceArr );
	return size;
}

//============================================================================

void makeExternalFileName(
	Target									&target,
	const char*								filename,
	char*										externalFile)
{
	if (!target.external)
		return;

	char drive[ _MAX_DRIVE ], dir[ _MAX_DIR ], ext[ _MAX_EXT ];
	_splitpath( filename, drive, dir, externalFile, ext );
}

//============================================================================

int computeExternalFileSize(
	Target									&target,
	ExpandableArray<Source>				&sourceArr,
	ExpandableArray<Sound>				&soundArr,
	ExpandableArray<Group>				&groupArr )
{
	if ( !target.external )
		return 0;

	int externalSize = 0;
	for ( int i = 0 ; i < sourceArr.Count() ; i++ )
	{
		externalSize += PAD( sourceArr[ i ].targetSize - target.headerSize, target.pad );
	}

	return externalSize;
}

//============================================================================

char* addToStringTable( char** stringTablePtr, char* string )
{
	char* d = *stringTablePtr;
	char* start = d;
	while ( *string != 0 ) { *d++ = *string++; }
	*d++ = 0;
	*stringTablePtr = d;
	return start;
}

//============================================================================

int findSourceIndex( ExpandableArray<Source> &sourceArr, char* sourceName )
{
	for ( int i = 0 ; i < sourceArr.Count() ; i++ )
		if ( stricmp( sourceArr[ i ].name, sourceName ) == 0 )
			return i;
	fprintf( stderr, "ERROR: couldn't find '%s' in list of sources\n", sourceName );
	exit( 1 );
}

//============================================================================

int findSoundIndex( ExpandableArray<Sound> &soundArr, char* soundName )
{
	for ( int i = 0 ; i < soundArr.Count() ; i++ )
		if ( stricmp( soundArr[ i ].name, soundName ) == 0 )
			return i;
	fprintf( stderr, "ERROR: couldn't find '%s' in list of sounds\n", soundName );
	exit( 1 );
}

//============================================================================

void SwapValue( void* v, int N )
{
	char T;

	switch ( N )
	{
		case 0:
		case 1:
			break;	// nothing to do
		case 2:
			T = ( ( char* ) v )[ 0 ];
			( ( char* ) v )[ 0 ] = ( ( char* ) v )[ 1 ];
			( ( char* ) v )[ 1 ] = T;
			break;
		case 4:
			T = ( ( char* ) v )[ 0 ];
			( ( char* ) v )[ 0 ] = ( ( char* ) v )[ 3 ];
			( ( char* ) v )[ 3 ] = T;
			T = ( ( char* ) v )[ 1 ];
			( ( char* ) v )[ 1 ] = ( ( char* ) v )[ 2 ];
			( ( char* ) v )[ 2 ] = T;
			break;
		default:
			fprintf( stderr, "ERROR: don't know how to swap '%d' byte element\n", N );
			break;
	}
}

//============================================================================

#define SWAP( v ) \
{ \
	if ( swapEndian ) \
	{ \
		SwapValue( &(v), sizeof( v ) ); \
	} \
}

//============================================================================

void prepareForExport( SoundModHeaderDef* fh, bool swapEndian )
{
	int i, j;

	//-- sources
	for ( i = 0 ; i < fh->sourceCount ; i++ )
	{
		SWAP( fh->sourceTable[ i ].offset );
		SWAP( fh->sourceTable[ i ].length );
		SWAP( fh->sourceTable[ i ].flags  );
	}

	//-- sounds
	for ( i = 0 ; i < fh->soundCount ; i++ )
	{
		SOUND_DEINIT_PTR( fh->soundTable[ i ].name   );  SWAP( fh->soundTable[ i ].name   );
		SOUND_DEINIT_PTR( fh->soundTable[ i ].source );  SWAP( fh->soundTable[ i ].source );
		SWAP( fh->soundTable[ i ].volume1  );
		SWAP( fh->soundTable[ i ].volume2  );
		SWAP( fh->soundTable[ i ].pitch1   );
		SWAP( fh->soundTable[ i ].pitch2   );
		SWAP( fh->soundTable[ i ].ambience );
		SWAP( fh->soundTable[ i ].presence );
		SWAP( fh->soundTable[ i ].wet      );
		SWAP( fh->soundTable[ i ].dry      );
		SWAP( fh->soundTable[ i ].audibleDist );
	}

	//-- groups
	for ( i = 0 ; i < fh->groupCount ; i++ )
	{
		for ( j = 0 ; j < fh->groupTable[ i ].count ; j++ )
		{
			SOUND_DEINIT_PTR( fh->groupTable[ i ].soundTbl[ j ] );	SWAP( fh->groupTable[ i ].soundTbl[ j ] );
		}
		SOUND_DEINIT_PTR( fh->groupTable[ i ].name     );  SWAP( fh->groupTable[ i ].name );
		SOUND_DEINIT_PTR( fh->groupTable[ i ].soundTbl );  SWAP( fh->groupTable[ i ].soundTbl );
		SWAP( fh->groupTable[ i ].type  );
		SWAP( fh->groupTable[ i ].count );
	}

	//-- main header
	SWAP( fh->id              );
	SWAP( fh->version         );
	SWAP( fh->sourceCount     );
	SWAP( fh->soundCount      );
	SWAP( fh->groupCount      );
	SWAP( fh->soundBankOffset );
	SWAP( fh->soundBankSize   );
	SWAP( fh->soundBankSize   );
	SWAP( fh->externalFileSize );
	SOUND_DEINIT_PTR( fh->sourceTable  );  SWAP( fh->sourceTable  );
	SOUND_DEINIT_PTR( fh->soundTable   );  SWAP( fh->soundTable   );
	SOUND_DEINIT_PTR( fh->groupTable   );  SWAP( fh->groupTable   );
}

//============================================================================

void changeExtension( char* filename, char* newExt )
{
	char drive[ _MAX_DRIVE ], dir[ _MAX_DIR ], fname[ _MAX_FNAME ], ext[ _MAX_EXT ];
	_splitpath( filename, drive, dir, fname, ext );
	if (*newExt != '.')
		sprintf( filename, "%s%s%s.%s", drive, dir, fname, newExt );
	else
		sprintf( filename, "%s%s%s%s", drive, dir, fname, newExt );
}

//============================================================================

void exportBankFile(
	char*										filename,
	Target									&target,
	ExpandableArray<Source>				&sourceArr,
	ExpandableArray<Sound>				&soundArr,
	ExpandableArray<Group>				&groupArr )
{
	int i;

	u32	fileLen    = computeExportFileSize( target, sourceArr, soundArr, groupArr );
	char* fileBuffer = (char*) malloc( fileLen );
	if ( fileBuffer == NULL )
	{
		fprintf( stderr, "ERROR: couldn't alloc %d bytes to create bank buffer\n", fileLen );
		exit( 1 );
	}
	memset( fileBuffer, 0, fileLen );

	// Create the external file
	u32 	externalLen = computeExternalFileSize( target, sourceArr, soundArr, groupArr );
	char* externalBuffer = NULL;
	if (externalLen)
	{
		externalBuffer = (char*) malloc( externalLen );
		if (externalBuffer == NULL)
		{
			fprintf( stderr, "ERROR: couldn't alloc %d bytes to create bank buffer\n", fileLen );
			exit( 1 );
		}
	}

	//-- output a log file, as well
	char logFilename[ _MAX_PATH ];
	strcpy( logFilename, filename );
	changeExtension( logFilename, ".log" );
	FILE* logFp = fopen( logFilename, "wt" );
	if ( logFp == NULL )
	{
		fprintf( stderr, "ERROR: couldn't open log file '%s' for write\n", logFilename );
		exit( 1 );
	}

	//-- set up dbl header
	if ( getDBLHeaderSize( target ) )
	{
		// dbl's have a funky "skip" chunk at the start that we need to use to ensure alignment
		int skip = getDBLHeaderSize( target ) - sizeof(ts_DBFileHdr) - sizeof(ts_DBChunkHdr);
		sprintf( fileBuffer, "%d", skip );
		ts_DBFileHdr* dblHeader = (ts_DBFileHdr*) ( fileBuffer + skip );
		dblHeader->u32Version      = DB_VERSION;
		dblHeader->u32Chunks       = 1;
		dblHeader->u32MaxChunkSize = fileLen - getDBLHeaderSize( target );
		dblHeader->u32FileFlags    = DBL_FILE_NO_COLLAPSE_HEADERS | DBL_FLUSHABLE;
		strcpy( dblHeader->cID, "DB" );

		// set up the chunk header
		ts_DBChunkHdr* chunk = (ts_DBChunkHdr*) &dblHeader[1];
		chunk->u16Type = DB_SOUNDBANK_MOD;
		chunk->u16Flags = DBL_NAMED_CHUNK | DBL_HEADER_32BYTE;
		chunk->u16Version = DB_SOUNDBANK_MOD_VERSION;
		chunk->u32Size    = dblHeader->u32MaxChunkSize;
		strcpy( chunk->cName, DEFAULT_CHUNK_KEY_STRING );
	}

	//-- set up the header
	SoundModHeaderDef* fileHeader = ( SoundModHeaderDef* ) ( fileBuffer + getDBLHeaderSize( target ) );
	fileHeader->id          = SOUND_MOD_FILE_ID;
	fileHeader->version     = SOUND_MOD_FILE_VERSION;
	fileHeader->sourceCount = sourceArr.Count();
	fileHeader->soundCount  = soundArr.Count();
	fileHeader->groupCount  = groupArr.Count();
	fileHeader->sourceTable = ( SourceDef* ) &fileHeader[ 1 ];
	fileHeader->soundTable  = ( SoundDef*  ) &fileHeader->sourceTable[ fileHeader->sourceCount ];
	fileHeader->groupTable  = ( GroupDef*  ) &fileHeader->soundTable[ fileHeader->soundCount ];
	SoundDef** groupRefStart  = ( SoundDef** ) &fileHeader->groupTable[ fileHeader->groupCount ];
	char* stringTableStart    = ( char* )      &groupRefStart[ getGroupRefCount( groupArr ) ];
	fileHeader->soundBankOffset = getHeaderSize( target, sourceArr, soundArr, groupArr );
	fileHeader->soundBankSize   = getSoundBankSize( target, sourceArr );
	makeExternalFileName( target, filename, fileHeader->externalFile );
	fileHeader->externalFileSize = externalLen;

	fprintf( logFp, "SoundMod LOG file\n" );
	fprintf( logFp, "\n" );
	fprintf( logFp, "Bank File : %s\n", filename );
	if ( target.external  )
		fprintf( logFp, "External File : %s\n", fileHeader->externalFile );
	fprintf( logFp, "\n" );
	fprintf( logFp, "Target Big Endian    : %d\n", target.bigEndian );
	fprintf( logFp, "Target Max Bank Size : %d\n", target.maxBankSize );
	fprintf( logFp, "Target Pad           : %d\n", target.pad );
	fprintf( logFp, "Target Header Size   : %d\n", target.headerSize );
	fprintf( logFp, "\n" );
	int soundDataSize = ( target.external ) ? fileHeader->externalFileSize : soundDataSize = fileHeader->soundBankSize;
	fprintf( logFp, "Sound Ram Size       : %d\n", soundDataSize );
	fprintf( logFp, "Memory Resident Size : %d\n", fileHeader->soundBankOffset );

	if ( soundDataSize > target.maxBankSize )
	{
		fprintf( stderr, "ERROR: %s bank data will exceed specific maximum bank size in config file ( %d bytes, max is %d bytes )\n",
			filename, soundDataSize, target.maxBankSize );
		for ( i = 0 ; i < sourceArr.Count() ; i++ )
			printf( "  %-50s %8d bytes\n", sourceArr[ i ].targetName, sourceArr[ i ].targetSize );
		exit( 1 );
	}
	else
	{
		printf( "bank size is %d, max is %d, %d bytes available\n", soundDataSize, target.maxBankSize, target.maxBankSize - fileHeader->soundBankSize );
	}

	//-- fill the all the sources
	fprintf( logFp, "\n========== SOURCES : %d ==========================\n", sourceArr.Count() );
	int curOffset = 0;
	int externalOffset = 0;
	for ( i = 0 ; i < sourceArr.Count() ; i++ )
	{
		fileHeader->sourceTable[ i ].offset = curOffset;
		fileHeader->sourceTable[ i ].length = PAD( sourceArr[ i ].targetSize - target.headerSize, target.pad );
		fileHeader->sourceTable[ i ].flags  = 0;
		if ( sourceArr[ i ].looping )
			fileHeader->sourceTable[ i ].flags |= SOUND_SOURCE_FLAG_LOOPING;
		if ( sourceArr[ i ].is2D )
			fileHeader->sourceTable[ i ].flags |= SOUND_SOURCE_FLAG_2D;

		if ( target.external )
			fprintf( logFp, "%3d - %08X %08X %s (external)\n", i, externalOffset, fileHeader->sourceTable[ i ].length, sourceArr[ i ].name );
		else
			fprintf( logFp, "%3d - %08X %08X %s\n", i, fileHeader->sourceTable[ i ].offset, fileHeader->sourceTable[ i ].length, sourceArr[ i ].name );

		memcpy( (char*)fileHeader + fileHeader->soundBankOffset + curOffset, sourceArr[ i ].targetData, target.headerSize );
		curOffset += PAD( target.headerSize, target.pad );

		// Copy the actual data
		int waveSize = sourceArr[ i ].targetSize - target.headerSize;
		if ( waveSize <= 0 )
		{
			fprintf( stderr, "ERROR: wave %s is smaller than header size (%d)\n", sourceArr[ i ].name, target.headerSize );
			exit( 1 );
		}

		if ( target.external )
		{
			memcpy( (char*)externalBuffer + externalOffset, sourceArr[ i ].targetData + target.headerSize, waveSize );
			externalOffset += PAD( waveSize, target.pad );
		}
		else
		{
			memcpy( (char*)fileHeader + fileHeader->soundBankOffset + curOffset, sourceArr[ i ].targetData + target.headerSize, waveSize );
			curOffset += PAD( waveSize, target.pad );
		}
	}

	//-- fill in the sounds
	fprintf( logFp, "\n========== SOUNDS: %d ==========================\n", soundArr.Count() );
	for ( i = 0 ; i < soundArr.Count() ; i++ )
	{
		fileHeader->soundTable[ i ].name     = addToStringTable( &stringTableStart, soundArr[ i ].name );
		fileHeader->soundTable[ i ].source   = &fileHeader->sourceTable[ findSourceIndex( sourceArr, soundArr[ i ].sourceName ) ];
		fileHeader->soundTable[ i ].volume1  = (signed char)(int)soundArr[ i ].volume1;
		fileHeader->soundTable[ i ].volume2  = (signed char)(int)soundArr[ i ].volume2;
		fileHeader->soundTable[ i ].pitch1   = (signed char)(int)( soundArr[ i ].pitch1 * SOUND_MOD_PITCH_SCALE );
		fileHeader->soundTable[ i ].pitch2   = (signed char)(int)( soundArr[ i ].pitch2 * SOUND_MOD_PITCH_SCALE );
		fileHeader->soundTable[ i ].ambience = (signed char)(int)soundArr[ i ].ambience;
		fileHeader->soundTable[ i ].presence = (signed char)(int)soundArr[ i ].presence;
		fileHeader->soundTable[ i ].wet      = ( soundArr[ i ].wet < 0 ) ? -1 : (signed char)(int)soundArr[ i ].wet;
		fileHeader->soundTable[ i ].dry      = ( soundArr[ i ].dry < 0 ) ? -1 : (signed char)(int)soundArr[ i ].dry;
		fileHeader->soundTable[ i ].audibleDist = (unsigned char)(int)soundArr[ i ].audibleDist;

		fprintf( logFp, "%3d - v=%-3d..%-3d p=%-3d..%-3d amb=%-3d pres=%-3d wet=%-3d dry=%-3d audDist=%-3d source=%-3d %s\n", i,
			fileHeader->soundTable[ i ].volume1,     fileHeader->soundTable[ i ].volume2,
			fileHeader->soundTable[ i ].pitch1,      fileHeader->soundTable[ i ].pitch2,
			fileHeader->soundTable[ i ].ambience,    fileHeader->soundTable[ i ].presence,
			fileHeader->soundTable[ i ].wet,         fileHeader->soundTable[ i ].dry,
			fileHeader->soundTable[ i ].audibleDist,
			fileHeader->soundTable[ i ].source - &fileHeader->sourceTable[ 0 ],
			soundArr[ i ].name
		);
	}

	//-- fill in the groups
	fprintf( logFp, "\n========== GROUPS: %d ==========================\n", groupArr.Count() );
	for ( i = 0 ; i < groupArr.Count() ; i++ )
	{
		fprintf( logFp, "%3d - %s\n", i, groupArr[ i ].name );
		fileHeader->groupTable[ i ].name     = addToStringTable( &stringTableStart, groupArr[ i ].name );
		fileHeader->groupTable[ i ].type     = groupArr[ i ].type;
		fileHeader->groupTable[ i ].count    = groupArr[ i ].soundList.Count();
		fileHeader->groupTable[ i ].soundTbl = groupRefStart;
		for ( int j = 0 ; j < groupArr[ i ].soundList.Count() ; j++ )
		{
			fprintf( logFp, "\t%3d - %s\n", j, groupArr[ i ].soundList[ j ] );
			fileHeader->groupTable[ i ].soundTbl[ j ] = &fileHeader->soundTable[ findSoundIndex( soundArr, groupArr[ i ].soundList[ j ] ) ];
		}
		groupRefStart += groupArr[ i ].soundList.Count();
	}

	//-- sanity check
	if ( PAD( (int)stringTableStart - (int)fileHeader, target.pad ) != fileHeader->soundBankOffset )
	{
		fprintf( stderr, "INTERNAL ERROR: string table doesn't end where it should. (%d, %d)\n",
			PAD( (int)stringTableStart - (int)fileHeader, target.pad ), fileHeader->soundBankOffset );
		exit(1);
	}
	if ( curOffset != fileHeader->soundBankSize )
	{
		fprintf( stderr, "INTERNAL ERROR: sound bank size incorrect. (%d,%d)\n",
			curOffset, fileHeader->soundBankSize );
		exit(1);
	}

	//-- dereference ptrs and swap endianness
	prepareForExport( fileHeader, target.bigEndian != TOOL_HOST_BIG_ENDIAN );

	//-- close log
	fclose( logFp );

	//-- write it out
	FILE* fp = fopen( filename, "wb" );
	if ( fp == NULL )
	{
		fprintf( stderr, "ERROR: couldn't open '%s' for write\n", filename );
		exit( 1 );
	}
	if ( fwrite( fileBuffer, 1, fileLen, fp ) != fileLen )
	{
		fprintf( stderr, "ERROR: couldn't write to '%s'\n", filename );
		exit( 1 );
	}
	fclose( fp );

	// Was there an external
	if ( externalBuffer )
	{
		char externalFile[ _MAX_PATH ];
		strcpy( externalFile, filename );
		changeExtension( externalFile, target.external );
		FILE *fp = fopen( externalFile, "wb" );
		if ( fp == NULL )
		{
			fprintf( stderr, "ERROR: couldn't open '%s' for write\n", externalFile );
			exit( 1 );
		}
		if ( fwrite( externalBuffer, 1, externalLen, fp ) != externalLen )
		{
			fprintf( stderr, "ERROR: couldn't write to '%s'\n", externalFile );
			exit( 1 );
		}
		fclose( fp );
	}
}

//============================================================================
