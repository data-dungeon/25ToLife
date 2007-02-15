//===========================================================================
//=
//= ScriptDebug.cpp
//=
//===========================================================================

#include "ScriptPCH.h"

#ifdef SCRIPT_COMPILER

struct LineNode
{
	const char*		line;
	LineNode*		next;
};

struct FileNode
{
	const char*		filename;
	LineNode*		lines;
	FileNode*		next;
};

//===========================================================================

ScriptDebug::ScriptDebug()
{
	m_files = NULL;
}

//===========================================================================

ScriptDebug::~ScriptDebug()
{
	FileNode* n;
	FileNode* nextFile;

	for ( n = m_files ; n ; n = nextFile )
	{
		nextFile = n->next;

		LineNode* l;
		LineNode* nextLine;
		for ( l = n->lines ; l ; l = nextLine )
		{
			nextLine = l->next;
			delete l;
		}
		delete n;
	}
}

//===========================================================================

const char* ScriptDebug::GetLine( const char* filename, int line )
{
#ifdef WIN32
	FileNode* file = GetFile( filename );
	if ( file )
	{
		int thisline;
		LineNode* n;
		for ( n = file->lines, thisline = 1 ; n ; n = n->next, thisline++ )
		{
			if ( thisline == line )
				return n->line;
		}
	}
#endif //WIN32

	return "";
}

//===========================================================================

FileNode* ScriptDebug::GetFile( const char* filename )
{
	FileNode* n = NULL;

#ifdef WIN32
	if ( filename && filename[ 0 ] )
	{
		// find out if this file is already loaded
		for ( n = m_files ; n ; n = n->next )
			if ( stricmp( n->filename, filename ) == 0 )
				break;

		// load the file if we don't have it.
		if ( !n )
		{
			LineNode*  lines = NULL;
			LineNode** lastLine = &lines;
			FILE* fp = fopen( filename, "rt" );
			if ( fp )
			{
				char buf[ 1024 ];
				while ( fgets( buf, sizeof( buf ), fp ) != NULL )
				{
					char* p;
					if ( ( p = strchr( buf, '\n' ) ) != NULL )
						*p = 0;
					LineNode* newLine = new LineNode;
					newLine->line     = strdup( buf );
					newLine->next     = NULL;
					*lastLine = newLine;
					lastLine  = &newLine->next;
				}
				fclose( fp );

				n = new FileNode;
				n->filename = strdup( filename );
				n->lines    = lines;
				n->next     = m_files;
				m_files     = n;
			}
		}
	}
#endif //WIN32

	return n;
}

//===========================================================================

#endif //SCRIPT_COMPILER
