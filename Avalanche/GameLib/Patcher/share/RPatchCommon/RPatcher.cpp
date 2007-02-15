
#include <io.h>
#include "RPatcher.h"

//---------------------------------------------------------------------------------------
RPatcher::RPatcher()
:	m_patchedFiles( 0 ), 
	m_numPatchedFiles( 0 )
{
}

//---------------------------------------------------------------------------------------
RPatcher::~RPatcher()
{
	for( unsigned int i = 0; i < m_numPatchedFiles; i++ )
	{
		if( m_patchedFiles[ i ].filename )
		{
			delete [] m_patchedFiles[ i ].filename;
		}
		if( m_patchedFiles[ i ].patchFile )
		{
			fclose( m_patchedFiles[ i ].patchFile );
		}
	}

	delete [] m_patchedFiles;
	m_patchedFiles = 0;
	m_numPatchedFiles = 0;
}

//---------------------------------------------------------------------------------------
void RPatcher::LoadAllPatchInfo()
{
	intptr_t handle;
	struct _finddata_t filestruct;
	int counter;

	assert( !m_patchedFiles );
	assert( !m_numPatchedFiles );
	
	// count files
	handle = _findfirst("*.patch", &filestruct);
	if( handle == -1 )
		return;
	do
	{
		if ( !(filestruct.attrib & _A_SUBDIR) ) 
		{
			FILE* f = fopen( filestruct.name, "rb" );
			fread( &counter, 1, sizeof( counter ), f );
			fclose( f );
			m_numPatchedFiles += counter;
		}
	} while( !_findnext( handle, &filestruct ) );
	_findclose( handle );

	// no patches?
	if( !m_numPatchedFiles )
		return;

	// allocate memory for patch info
	m_patchedFiles = new RPatchInfo[ m_numPatchedFiles ];

	RPatchInfo* curPatch = m_patchedFiles;
	handle = _findfirst( "*.patch", &filestruct );
	do // go through all the files
	{
		if ( !(filestruct.attrib & _A_SUBDIR) ) 
		{
			FILE* f = fopen( filestruct.name, "rb" );
			fseek( f, sizeof( counter ), SEEK_SET ); // skip the count
			for( int i = 0; i < counter; i++ )
			{
				LoadPatchInfo( f, curPatch );
				curPatch++;
			}
			// don't close the file
		}
	} while(!_findnext(handle, &filestruct));

	_findclose(handle);
}

//---------------------------------------------------------------------------------------
void RPatcher::LoadPatchInfo( FILE* patchFile, RPatchInfo* result )
{
	unsigned char nameLen;
	fread( &result->patchDataSize, sizeof(result->patchDataSize), 1, patchFile );
	fread( &result->patchDataOffset, sizeof(result->patchDataOffset), 1, patchFile );
	fread( &result->patchedResultSize, sizeof(result->patchedResultSize), 1, patchFile );
	fread( &nameLen, sizeof(nameLen), 1, patchFile );
	result->filename = new char[ nameLen + 1 ];
	result->filename[ nameLen ] = 0;
	fread( result->filename, nameLen, 1, patchFile );
	result->patchFile = patchFile;
}

//---------------------------------------------------------------------------------------
bool RPatcher::GetPatchInfo( const char* const filename, const RPatchInfo*& patchInfo ) const
{
	for( unsigned int i = 0; i < m_numPatchedFiles; i++ )
	{
		assert( m_patchedFiles[ i ].filename );
		if( !stricmp( filename, m_patchedFiles[ i ].filename ) )
		{
			patchInfo = &m_patchedFiles[ i ];
			return true;
		}
	}
	patchInfo = 0;
	return false;
}

//---------------------------------------------------------------------------------------
void RPatcher::LoadAndPatch( const RPatchInfo* const patchInfo, unsigned char*& destinationMemory ) const
{
	destinationMemory = new unsigned char[ patchInfo->patchedResultSize ];
	unsigned char* patchMemory = new unsigned char[ patchInfo->patchDataSize ];
	unsigned int* patchIter = (unsigned int*) patchMemory;
	unsigned char* destIter = (unsigned char*) destinationMemory;
	
	fseek( patchInfo->patchFile, patchInfo->patchDataOffset, SEEK_SET );
	fread( patchMemory, 1, patchInfo->patchDataSize, patchInfo->patchFile );

	FILE* originalFile = fopen( patchInfo->filename, "rb" );
	assert( originalFile );

	const unsigned int* const patchIterEnd = (const unsigned int* const) (((char*)patchIter) + patchInfo->patchDataSize);
	DataSource source;
	unsigned int offset;
	unsigned int len;
	while( patchIter < patchIterEnd )
	{
		source = (DataSource) ((*patchIter) & 3) ;
		offset = (*patchIter) >> 2;
		++patchIter;

		len = *patchIter;
		++patchIter;

		assert( source == DATA_LITERAL || source == DATA_ORIGINAL || source == DATA_MODIFIED );
		if( source == DATA_LITERAL )
		{
			memcpy( destIter, patchIter, len );
			
			const char* byteSkip = (const char*)patchIter;
			byteSkip += len;
			patchIter = (unsigned int*) byteSkip;

			destIter += len;
		}
		else if( source == DATA_ORIGINAL )
		{
			fseek( originalFile, offset, SEEK_SET );
			fread( destIter, 1, len, originalFile );
			destIter += len;
		}
		else if( source == DATA_MODIFIED )
		{
			memmove( destIter, (char*)destinationMemory + offset, len );
			destIter += len;
		}
	}

	fclose( originalFile );
	delete [] patchMemory;
}

