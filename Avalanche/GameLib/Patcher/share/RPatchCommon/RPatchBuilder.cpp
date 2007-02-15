
#include "RPatchBuilder.h"

inline int MAX(int a, int b) {return (a > b) ? a : b;}

//---------------------------------------------------------------------------------------
RPatchBuilder::RPatchBuilder()
:	m_error( RPE_OK )
{
	memset( m_data, 0, sizeof( m_data ) );
	memset( m_dataLen, 0, sizeof( m_dataLen ) );
}

//---------------------------------------------------------------------------------------
RPatchBuilder::~RPatchBuilder()
{
	Clear();

	if (m_data[0])
		delete [] m_data[0];
	if (m_data[1])
		delete [] m_data[1];
}
//---------------------------------------------------------------------------------------
void RPatchBuilder::Clear(void)
{
	for( SubstringSequence::const_iterator i = m_patchSubstrings.begin(); i != m_patchSubstrings.end(); i++ )
	{
		if( i->literalData )
		{
			delete [] i->literalData;
		}
	}
	m_patchSubstrings.clear();
}

//---------------------------------------------------------------------------------------
void RPatchBuilder::SetData( DataSource source, const unsigned char* const data, const int numBytes )
{
	assert( source == DATA_ORIGINAL || source == DATA_MODIFIED );

	// If we are reusing this data, delete the old one
	if (m_data[source])
		delete [] m_data[source];

	if( !data || numBytes <= 0 )
	{
		m_data[ source ] = 0;
		m_dataLen[ source ] = 0;
		m_error = RPE_INVALID_INPUT;
	}
	else
	{
		m_data[ source ] = data;
		m_dataLen[ source ] = numBytes;
	}
}

/*
//---------------------------------------------------------------------------------------
void RPatchBuilder::CopyData( DataSource source, const unsigned char* const data, const int numBytes )
{
	assert( source == DATA_ORIGINAL || source == DATA_MODIFIED );
	if( !data || numBytes <= 0 )
	{
		// force error
		SetData( source, 0, 0 );
	}
	else
	{
		unsigned char* const copyData = new unsigned char[ numBytes ];
		memcpy( copyData, data, numBytes );
		SetData( source, copyData, numBytes );
	}
}
*/
//---------------------------------------------------------------------------------------
void RPatchBuilder::LoadData( DataSource source, const char* const filename )
{
	assert( source == DATA_ORIGINAL || source == DATA_MODIFIED );
	FILE* f = fopen( filename, "rb" );
	if( !f )
	{
		SetData( source, 0, 0 );
		m_error = RPE_FILE_LOAD_FAILED;
		return;
	}

	fseek( f, 0, SEEK_END );
	const int size = (int) ftell( f );
	rewind( f );
	
	unsigned char* const data = new unsigned char[ size ];
	fread( data, 1, size, f );
	fclose( f );

	SetData( source, data, size );
}

//---------------------------------------------------------------------------------------
void RPatchBuilder::SetOriginalData( const unsigned char* const data, const int numBytes )
{
	SetData( DATA_ORIGINAL, data, numBytes );
}

//---------------------------------------------------------------------------------------
void RPatchBuilder::SetModifiedData( const unsigned char* const data, const int numBytes )
{
	SetData( DATA_MODIFIED, data, numBytes );
}

/*
//---------------------------------------------------------------------------------------
void RPatchBuilder::CopyOriginalData( const unsigned char* const data, const int numBytes )
{
	CopyData( DATA_ORIGINAL, data, numBytes );
}

//---------------------------------------------------------------------------------------
void RPatchBuilder::CopyModifiedData( const unsigned char* const data, const int numBytes )
{
	CopyData( DATA_MODIFIED, data, numBytes );
}
*/

//---------------------------------------------------------------------------------------
void RPatchBuilder::LoadOriginalData( const char* const filename )
{
	LoadData( DATA_ORIGINAL, filename );
}

//---------------------------------------------------------------------------------------
void RPatchBuilder::LoadModifiedData( const char* const filename )
{
	LoadData( DATA_MODIFIED, filename );
}

//---------------------------------------------------------------------------------------
// Build a patch where there is no compare involved; just take literal data from
// the loaded modified file.
void RPatchBuilder::BuildNoCompare( const char* const filename )
{
	// Clear any error conditions
	m_error = RPE_OK;

	// Load the file
	LoadData(DATA_MODIFIED, filename);

	// Make sure it loaded ok
	if (IsError())
		return;

	// Initialize patch data
	m_patchSubstrings.clear();

	// Create convenience pointers to data
	const unsigned char* const modified = m_data[ DATA_MODIFIED ];
	const int modifiedLen = m_dataLen[ DATA_MODIFIED ];

	// Signal error and abort if any of the input data is incomplete
	if( !modified || !modifiedLen )
	{
		m_error = RPE_INVALID_INPUT;
		return;
	}

	m_diffWindowPos = 0;

	// Create an accumulator for literal strings that have no matches in the history text,
	// but make it big enough to hold the entire buffer.
	LiteralAccumulator literalString(modifiedLen);
	
	// Fill the literalString with our buffer. Couldn't find a method to do this
	// at one go, hence the loop
	int i;
	for (i = 0; i < modifiedLen; i++)
		literalString[i] = modified[i];

	AddPatchLiteral( literalString, 0 );
}

//---------------------------------------------------------------------------------------
void RPatchBuilder::Build( const int smallestMatchLen )
{
	assert( smallestMatchLen > 0 );

	// Pw = window position
	// Lw = window length
	// Dn = new data
	// Do = old data
	// Da = (all data) Do + Dn[ 0, Pw-1 ]  AKA "history text"
	// Dw = (window data) Dn[ Pw, Pw + Lw ]
	// M(f) = find best (longest, fastest, etc) match of all substrings starting with f in Da
	// R = substring resulting from M(f)

	// Repeat until Pw > Dn - Lw
		// R = M(Dw)
		// if R is empty, emit Dn[ Pw ], Pw++
		// else emit R, Pw += |R|

	
	// Initialize patch data
	m_patchSubstrings.clear();

	// Create convenience pointers to data
	const unsigned char* const original = m_data[ DATA_ORIGINAL ];
	const int originalLen = m_dataLen[ DATA_ORIGINAL ];
	const unsigned char* const modified = m_data[ DATA_MODIFIED ];
	const int modifiedLen = m_dataLen[ DATA_MODIFIED ];

	// Signal error and abort if any of the input data is incomplete
	if( !original || !modified || !originalLen || !modifiedLen )
	{
		m_error = RPE_INVALID_INPUT;
		return;
	}

	// Initialize the window that passes through the modified data to detect differences
	RelativeSubstring diffWindow;
	m_diffWindowPos = 0;
	diffWindow.source = DATA_MODIFIED;
	diffWindow.len = smallestMatchLen;

	// Create an accumulator for literal strings that have no matches in the history text
	LiteralAccumulator literalString;

	// March a window through the modified data trying to match substrings to the history text
	while( m_diffWindowPos < modifiedLen - smallestMatchLen - 1 )
	{
		RelativeSubstring searchResult;
		diffWindow.offset = m_diffWindowPos;
		HistorySubstringSearch( smallestMatchLen, diffWindow, searchResult );
		
		if( searchResult.len == 0 )
		{
			// Found no match, emit current byte as a literal and move window up one byte
			literalString.push_back( modified[ m_diffWindowPos ] );
			++m_diffWindowPos;
		}
		else
		{
			// Found a match, first emit any accummulated literals, then emit the match
			if( !literalString.empty() )
			{
				AddPatchLiteral( literalString, m_diffWindowPos );
				literalString.clear();
			}

			AddPatchSubstring( searchResult );
			m_diffWindowPos += searchResult.len;
		}
	}

	// Make sure the last few bytes of the file are copied too
	for( int i = m_diffWindowPos; i < modifiedLen; i++ )
	{
		literalString.push_back( modified[ i ] );
	}
	if( !literalString.empty() )
	{
		AddPatchLiteral( literalString, m_diffWindowPos );
	}
}

//---------------------------------------------------------------------------------------
int RPatchBuilder::WritePatchToBuffer(char* buf, int bufSize)
{
	if (bufSize < CalcPatchSize())
		return -1;

	char* dstPtr = buf;

	// write each substring command 
	for( SubstringSequence::const_iterator i = m_patchSubstrings.begin(); i != m_patchSubstrings.end(); i++ )
	{
		const RelativeSubstring& s = *i;

		assert( s.source == DATA_LITERAL || s.source == DATA_ORIGINAL || s.source == DATA_MODIFIED );
		assert( s.len > 0 );
		assert( s.offset >= 0 );
		assert( s.source == DATA_LITERAL || s.offset < m_dataLen[ s.source ] );
		assert( s.source == DATA_LITERAL || s.offset + s.len <= m_dataLen[ s.source ] );

		// This will fall apart when we no longer have 4 byte ints...
		assert(sizeof(int) == 4);

		const unsigned int source = s.source;
		const unsigned int sourceAndOffset = ((int)source & 3) | ((s.offset & 0x3fffffff) << 2);
		memcpy(dstPtr, &sourceAndOffset, sizeof(sourceAndOffset));
		dstPtr += sizeof(sourceAndOffset);
		memcpy(dstPtr, &s.len, sizeof(s.len));
		dstPtr += sizeof(s.len);

		if( s.source == DATA_LITERAL )
		{
			memcpy(dstPtr, s.literalData, s.len);
			dstPtr += s.len;
		}
	}

	return (int)(dstPtr - buf);
}

//---------------------------------------------------------------------------------------
int RPatchBuilder::WritePatch( FILE* f )
{
	size_t written = 0;

	// write each substring command 
	for( SubstringSequence::const_iterator i = m_patchSubstrings.begin(); i != m_patchSubstrings.end(); i++ )
	{
		const RelativeSubstring& s = *i;

		assert( s.source == DATA_LITERAL || s.source == DATA_ORIGINAL || s.source == DATA_MODIFIED );
		assert( s.len > 0 );
		assert( s.offset >= 0 );
		assert( s.source == DATA_LITERAL || s.offset < m_dataLen[ s.source ] );
		assert( s.source == DATA_LITERAL || s.offset + s.len <= m_dataLen[ s.source ] );

		const unsigned int source = s.source;
		const unsigned int sourceAndOffset = ((int)source & 3) | ((s.offset & 0x3fffffff) << 2);
		written += fwrite( &sourceAndOffset, 1, sizeof( sourceAndOffset ), f );
		written += fwrite( &s.len, 1, sizeof( s.len ), f );

		if( s.source == DATA_LITERAL )
		{
			written += fwrite( s.literalData, 1, s.len, f );
		}
	}
	return (int)written;
}

//---------------------------------------------------------------------------------------
int RPatchBuilder::CalcPatchSize( void ) const
{
	int size = 0;

	// Check each substring command 
	for( SubstringSequence::const_iterator i = m_patchSubstrings.begin(); i != m_patchSubstrings.end(); i++ )
	{
		const RelativeSubstring& s = *i;

		size += 8;	// Source, offset, and length = 8 bytes

		if( s.source == DATA_LITERAL )
			size += s.len;
	}
	return size;
}

//---------------------------------------------------------------------------------------
void RPatchBuilder::WriteToFile( const char* const filename )
{
	FILE* f = fopen( filename, "wb" );
	if( !f )
	{
		m_error = RPE_FILE_SAVE_FAILED;
		return;
	}

	WritePatch(f);
	fclose( f );
}

//---------------------------------------------------------------------------------------
void RPatchBuilder::Verify()
{
	int compResult;
	const unsigned char* modified = m_data[ DATA_MODIFIED ];
	
	//FILE* f = fopen( "verify.xxx", "wb" );
	
	for( SubstringSequence::const_iterator i = m_patchSubstrings.begin(); i != m_patchSubstrings.end(); i++ )
	{
		const RelativeSubstring& s = *i;

		assert( s.source == DATA_LITERAL || s.source == DATA_ORIGINAL || s.source == DATA_MODIFIED );
		assert( s.len > 0 );
		assert( s.offset >= 0 );
		assert( s.source == DATA_LITERAL || s.offset < m_dataLen[ s.source ] );
		assert( s.source == DATA_LITERAL || s.offset + s.len <= m_dataLen[ s.source ] );

		if( s.source == DATA_LITERAL )
		{
			compResult = memcmp( s.literalData, modified, s.len );
			//fwrite( s.literalData, 1, s.len, f );
		}
		else
		{
			compResult = memcmp( &m_data[ s.source ][ s.offset ], modified, s.len );
			//fwrite( &m_data[ s.source ][ s.offset ], 1, s.len, f );
		}

		if( compResult != 0 )
		{
			m_error = RPE_VERIFY_FAILED;
			return;
		}

		//static const char* names[ 3 ] = { "ORIGINAL", "MODIFIED", "LITERAL" };
		//printf( "%i bytes from %s starting at offset %i\n", s.len, names[ s.source ], s.offset );

		modified += s.len;
	}
	
	//fclose( f );
}

//---------------------------------------------------------------------------------------
void RPatchBuilder::AddPatchLiteral( const LiteralAccumulator& literal, const int offset )
{
	m_patchSubstrings.push_back( RelativeSubstring() );
	RelativeSubstring& s = m_patchSubstrings.back();
	s.source = DATA_LITERAL;
	s.offset = offset;
	s.len = (int) literal.size();
	s.literalData = new unsigned char[ s.len ];
	copy( literal.begin(), literal.end(), s.literalData );
}

//---------------------------------------------------------------------------------------
void RPatchBuilder::AddPatchSubstring( const RelativeSubstring& substring )
{
	m_patchSubstrings.push_back( substring );
}

//------------------------------------------------------------------------------
void RPatchBuilder::BruteForceSubstringSearch( const int substringLen, const RelativeSubstring& searchFor, RelativeSubstring& searchResult ) const
{
	// Sanity check
	assert( searchFor.source == DATA_ORIGINAL || searchFor.source == DATA_MODIFIED );
	assert( searchFor.len > 0 );
	assert( searchFor.offset >= 0 && searchFor.offset <= m_dataLen[ searchFor.source ] );

	// Initialize result to nothing
	searchResult.source = DATA_INVALID;
	searchResult.offset = 0;
	searchResult.len = 0;

	// Make convenience pointer to the string being searched for
	const unsigned char* const searchForString = &m_data[ searchFor.source ][ searchFor.offset ];
	const int searchForLen = m_dataLen[ searchFor.source ] - searchFor.offset;

	// Iterate through both searchable texts
	for( int sourceIter = 0; sourceIter < 2; sourceIter++ )
	{
		// Search for match string (the string being searched for) inside source.
		// If searching inside the modified data, only search up to the current diff window position since the patch process
		//  can only copy substrings that have already been reconstructed (i.e. data that has already been passed by the 
		//  diff window)
		int windowPos = 0;
		const unsigned char* const searchInString = m_data[ sourceIter ];
		const int searchInLen = (sourceIter == DATA_MODIFIED) ? (m_diffWindowPos - 1) : (m_dataLen[ sourceIter ]);

		// Are there enough characters to search in?
		if( searchInLen < 1 )
			continue;

		// Brute force search starting at first character
		for (windowPos = 0; windowPos < (searchInLen - 1); windowPos++)
		{
			int i;
			for (i = 0; i < searchForLen; i++)
			{
				if (searchInString[windowPos + i] != searchForString[i])
					break;
			}

			// Now i is the length of the string we matched 
			if (i > 0)
			{
				// Matched at least some characters
				if ((i > substringLen) && (i > searchResult.len))
				{
					searchResult.len = i;
					searchResult.offset = windowPos;
					searchResult.source = (DataSource) sourceIter;
				}
			}
		}
	}
}

//---------------------------------------------------------------------------------------
void RPatchBuilder::HistorySubstringSearch( const int substringLen, const RelativeSubstring& searchFor, RelativeSubstring& searchResult ) const
{
	// Use brute force if min substring len is 1
	if (substringLen == 1)
	{
		BruteForceSubstringSearch(substringLen, searchFor, searchResult);
		return;
	}

	// http://www-sr.informatik.uni-tuebingen.de/~buehler/BM/BM.html
	// Sanity check
	assert( searchFor.source == DATA_ORIGINAL || searchFor.source == DATA_MODIFIED );
	assert( searchFor.len > 0 );
	assert( searchFor.offset >= 0 && searchFor.offset <= m_dataLen[ searchFor.source ] );

	// Initialize result to nothing
	searchResult.source = DATA_INVALID;
	searchResult.offset = 0;
	searchResult.len = 0;

	// Make convenience pointer to the string being searched for
	const unsigned char* const searchForString = &m_data[ searchFor.source ][ searchFor.offset ];
	const int searchForLen = m_dataLen[ searchFor.source ] - searchFor.offset;

	// Initialize the simplified Boyer-Moore offset lookup table for the string being searched for
	int i;
	int offsets[ 256 ];
	memset( offsets, 0, sizeof( offsets ) );
    for( i = 0; i < substringLen; i++ )
	{
		const unsigned char c = searchForString[ i ];
		offsets[ c ] = i + 1;
	}

    // Iterate through both searchable texts
	for( int sourceIter = 0; sourceIter < 2; sourceIter++ )
	{
		// Search for match string (the string being searched for) inside source.
		// If searching inside the modified data, only search up to the current diff window position since the patch process
		//  can only copy substrings that have already been reconstructed (i.e. data that has already been passed by the 
		//  diff window)
		int windowPos = 0;
		const unsigned char* const searchInString = m_data[ sourceIter ];
		const int searchInLen = (sourceIter == DATA_MODIFIED) ? (m_diffWindowPos - 1) : (m_dataLen[ sourceIter ]);

		// Are there enough characters to search in?
		if( searchInLen < substringLen )
		{
			continue;
		}

		// Move a simplified Boyer-Moore window across the source text (the string to search in).
		while( windowPos < searchInLen - substringLen )
		{
			assert( windowPos >= 0 );
			
			// Compare byte by byte looking for mismatch
			for( i = substringLen - 1; i >= 0; --i )
			{
				if( searchInString[ windowPos + i ] != searchForString[ i ] )
				{
					// Found mismatch, offset window using Boyer-Moore
 					windowPos += MAX(1, i + 1 - offsets[ searchInString[ windowPos + i ] ]);
					assert( windowPos > 0 );
					assert( windowPos < searchInLen );
					break;
				}
			}
			
			// Found a match if i == -1
			if( i == -1 )
			{
				// Keep trying to match characters until the end of either texts is hit
				int matchRun;
				for( matchRun = substringLen; 
					(matchRun < searchInLen - windowPos) && (matchRun < searchForLen); 
					++matchRun )
				{
					if( searchInString[ windowPos + matchRun ] != searchForString[ matchRun ] )
					{
						break;
					}
				}

				// Rate the new match and see if it is the best so far
				if( matchRun > searchResult.len )
				{
					searchResult.len = matchRun;
					searchResult.offset = windowPos;
					searchResult.source = (DataSource) sourceIter;
				}

				// Offset the window to the end of the just-matched string
				windowPos += matchRun;
			}
		}
	}
}

//---------------------------------------------------------------------------------------
bool RPatchBuilder::IsError() const
{
	return m_error != RPE_OK;
}

//---------------------------------------------------------------------------------------
RPatchBuilder::Error RPatchBuilder::GetError() const
{
	return m_error;
}


//---------------------------------------------------------------------------------------
void RPatchBuilder::Dump( const char* const filename )
{
	FILE* f = fopen( filename, "rb" );
	if( !f ) return;

	for( ;; )
	{
		unsigned int sourceAndOffset;
		unsigned int len;
		DataSource source;
		unsigned int offset;
		
		fread( &sourceAndOffset, 1, sizeof( sourceAndOffset ), f );
		if( feof( f ) )
			break;

		fread( &len, 1, sizeof( len ), f );

		source = (DataSource) (sourceAndOffset & 3);
		offset = sourceAndOffset >> 2;

		static const char* names[ 3 ] = { "ORIGINAL", "MODIFIED", "LITERAL" };
		printf( "%i bytes from %s ", len, names[ source ] );
		if( source == DATA_LITERAL )
			printf( "\n" );
		else
			printf( "starting at offset %i\n", offset );
		
		if( source == DATA_LITERAL )
		{
			fseek( f, len, SEEK_CUR );
		}
	}
	fclose( f );

}