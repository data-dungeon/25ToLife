////////////////////////////////////////////////////////////////////////////
//
// TupperwareReader
//
// A reader of tupperware files.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWAREREADER_H
#define TUPPERWAREREADER_H

////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#ifndef TUPPERWARE_MEMORY_MANAGER_H
#include "memorymanager.h"
#endif

#ifndef TUPPERWARE_ARRAY_H
#include "tuparray.h"
#endif

#ifndef TUPPERWARE_STRING_H
#include "tupstring.h"
#endif

////////////////////////////////////////////////////////////////////////////

class TupperwareAggregate;
class TupperwareList;
class TupperwareScalar;

////////////////////////////////////////////////////////////////////////////

class TupperwareReader 
{
public:
	class FileBuffer
	{
	public:
		TupString m_filename;
		const char *m_pBuffer;
		int m_nBufferLength;
		int m_nParentBufferIndex;
	};

	static TupperwareAggregate* Read( const char* buffer, int bufferLength);
	static TupperwareAggregate* Read( const char* file_name,TupArray<FileBuffer> *pFileBuffers=NULL);

private:
	TupperwareReader();
	~TupperwareReader();
};

////////////////////////////////////////////////////////////////////////////

class TupperwareToken 
{
public:
	enum TupperwareTokenType 
	{
		UNKNOWN, KEY, FLOAT, INT, STRING, BINARY_DATA, EQUAL, 
		BEGIN_AGGREGATE, END_AGGREGATE, BEGIN_LIST, END_LIST, END_OF_FILE,
		LIST_SEPARATOR
	};
	
	TupperwareToken(const TupperwareToken& token);
	TupperwareToken& operator=(const TupperwareToken& token);
	TupperwareToken(TupperwareTokenType type);
	TupperwareToken(TupperwareTokenType type, char *string);
	TupperwareToken(TupperwareTokenType type, int value);
	TupperwareToken(TupperwareTokenType type, float value);
	~TupperwareToken();
	
	TupperwareTokenType m_Type;
	union 
	{
		char *m_String;
		int m_Int;
		float m_Float;
	};
	int *m_Ref;
};

////////////////////////////////////////////////////////////////////////////


class TupperwareParser 
{
	friend class TupperwareReader;

	class BufferData
	{
	public:
		TupString m_filename;
		const char *m_pBuffer;
		int m_nBufferLength;
		int m_nBufferPos;
		int m_nParentBufferIndex;
#ifdef TRACK_LINE_NUMBERS
		int m_nBufferLineNum;
		int m_nBufferCharPos;
#endif // TRACK_LINE_NUMBERS
	};

private:
	TupperwareParser(const char *pBuffer, int nBufferLength, const char *pFilename,bool bKeepBufferData=false);
	~TupperwareParser();

	void ResetTokenStr();
	void AddBlockToTokenStr(const char *block, int len );
	void AddToTokenStr( char c );

	int NextBlock( char *block, int len );
	char NextChar(void);
	void AdjustBlockBack(int len);

	TupperwareList *ParseListBody();
	TupperwareAggregate *ParseAggregateBody( bool root_aggregate );
	TupperwareToken NextToken();

	char NextBufferNextChar(void);
	void AddIncludeFile(const char *pFilename);

	TupArray<BufferData> m_bufferDataArray;
	// current buffer info
	const char *m_pBuffer;
	int m_nBufferLength;
	int m_nBufferPos;
#ifdef TRACK_LINE_NUMBERS
	int m_nBufferLineNum;
	int m_nBufferCharPos;
#endif // TRACK_LINE_NUMBERS
	int m_nCurrentBufferIndex;

	char* m_pTokenStr;
	int m_nTokenStrLen;
	int m_nTokenStrPos;

	bool m_bKeepBufferData;

	char* m_StoredChar; 
	int m_StoredLength;
	int m_NextStored;
};

#include "Reader.hpp"

////////////////////////////////////////////////////////////////////////////

#endif
