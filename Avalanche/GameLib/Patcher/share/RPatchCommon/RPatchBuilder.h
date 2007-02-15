
// 4/19/04 - JM
// Copyright R Entertainment
// Do not use or distribute without the express permission of R Entertainment.

#ifndef __RPATCHBUILDER_H__
#define __RPATCHBUILDER_H__

#include <vector>
#include <list>
#include <string>
#include <cassert>

class RPatchBuilder
{
public:

	RPatchBuilder();
	~RPatchBuilder();
	void Clear(void);

	static void Dump( const char* const filename );

	void SetOriginalData( const unsigned char* const data, const int numBytes );
	void SetModifiedData( const unsigned char* const data, const int numBytes );

	//void CopyOriginalData( const unsigned char* const data, const int numBytes );
	//void CopyModifiedData( const unsigned char* const data, const int numBytes );

	void LoadModifiedData( const char* const filename );
	void LoadOriginalData( const char* const filename );
	
	void BuildNoCompare( const char* const filename );
	void Build( const int smallestMatchLen = 4 );
	void Verify();

	void WriteToFile( const char* const filename );

	int  WritePatch( FILE* f );
	int  WritePatchToBuffer(char* buf, int bufSize);
	int	 CalcPatchSize(void) const;

	enum Error
	{
		RPE_OK,
		RPE_INVALID_INPUT,
		RPE_FILE_LOAD_FAILED,
		RPE_FILE_SAVE_FAILED,
		RPE_VERIFY_FAILED
	};

	bool IsError() const;
	Error GetError() const;

private:

	enum DataSource
	{
		DATA_INVALID	= 0xff,
		DATA_ORIGINAL	= 0,
		DATA_MODIFIED	= 1,
		DATA_LITERAL	= 2
	};
	struct RelativeSubstring
	{
		DataSource		source;
		int				offset;
		int				len;
		unsigned char*	literalData;
		RelativeSubstring() : source( DATA_INVALID ), offset( 0 ), len( 0 ), literalData( 0 ) {}
		~RelativeSubstring() {}
	};
	typedef std::vector< RelativeSubstring > SubstringSequence;
	typedef std::vector< unsigned char > LiteralAccumulator;

	void AddPatchLiteral( const LiteralAccumulator& literal, const int offset );
	void AddPatchSubstring( const RelativeSubstring& substring );
	void SetData( DataSource source, const unsigned char* const data, const int numBytes );
	//void CopyData( DataSource source, const unsigned char* const data, const int numBytes );
	void LoadData( DataSource source, const char* const filename );
	void HistorySubstringSearch( const int substringLen, const RelativeSubstring& searchFor, RelativeSubstring& searchResult ) const;
	void BruteForceSubstringSearch( const int substringLen, const RelativeSubstring& searchFor, RelativeSubstring& searchResult ) const;

	Error					m_error;
	const unsigned char*	m_data[ 2 ];
	int						m_dataLen[ 2 ];
	SubstringSequence		m_patchSubstrings;
	int						m_diffWindowPos;

};



#endif //__RPATCHBUILDER_H__
