////////////////////////////////////////////////////////////////////////////
//
// TupperwareWriter
//
// A writer of tupperware files.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWAREWRITER_H
#define TUPPERWAREWRITER_H

////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

////////////////////////////////////////////////////////////////////////////

class TupperwareAggregate;
class TupperwareList;
class TupperwareScalar;
class TupperwareComment;

////////////////////////////////////////////////////////////////////////////

class TupperwareWriter {
public:
	static int Write( const char* file_name, const TupperwareAggregate* root );

private:
	TupperwareWriter( FILE* file, const char* file_name );
	~TupperwareWriter();

	void Indent( int indent_level );

	void WriteScalar( const TupperwareScalar* scalar );
	void WriteComment( const TupperwareComment* comment );
	void WriteList( const TupperwareList* list, int indent_level );
	void WriteAggregate( const TupperwareAggregate* aggregate, bool root_aggregate, int indent_level );

private:
	void PrintString( const char* string );
	void WriteBinaryFile( const TupperwareScalar* scalar );
	void PrintBinaryData( const TupperwareScalar* scalar );
	char* MakeRefFileName( const char* file_name, const char* file_name_hint );

	FILE* m_File;
	char* m_FileName;
};

////////////////////////////////////////////////////////////////////////////

#endif
