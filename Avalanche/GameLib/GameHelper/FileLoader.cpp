//============================================================================
//=
//= FileLoader.cpp - manages loading of dbl's into the game from a
//=                  list file loaded from disk.
//=
//============================================================================

#include "GameHelper/GameHelperPCH.h"
#include "GameHelper/FileLoader.h"
#include "Script/ScriptSystem.h"
#include "Game/Managers/ResourceManager.h"
#include "EngineHelper/pathfile.h"

FileLoader g_fileLoader;

//============================================================================

FileLoader::FileLoader() : d_filenames( 0, 8 ), d_sections( 0, 8 )
{
}

//============================================================================

FileLoader::~FileLoader()
{
	int i;

	// free file names
	for ( i = 0 ; i < d_filenames.Count() ; i++ )
		delete [] d_filenames[ i ];

	// free sections
	for ( i = 0 ; i < d_sections.Count() ; i++ )
	{
		delete [] d_sections[ i ]->name;
		delete d_sections[ i ];
	}
}

//============================================================================

bool FileLoader::LoadSpecificationFile( const char* filename )
{
	s32		size;
	t_Error	error;
	char*		buffer = (char*) Media::Load( filename, &size, &error );

	char		sectionName[ 128 ] = "";
	Section*	section = NULL;

	if ( !buffer )
		return false;

	char* bufPtr = buffer;
	char* end = buffer + size;
	while ( bufPtr < end && *bufPtr != 0 )
	{
		char lineBuf[ 1024 ];

		// skip leading whitespace
		while ( bufPtr < end && *bufPtr != 0 && isspace( *bufPtr ) )
			bufPtr++;

		// copy line into line buffer, lowercase as we're doing it
		char* dest = lineBuf;
		while ( bufPtr < end && *bufPtr != 0 && *bufPtr != '\n' && *bufPtr != '\r' )
			*dest++ = tolower( *bufPtr++ );
		*dest = 0;

		// remove comments
		char* p;
		if ( ( p = strstr( lineBuf, "//" ) ) != NULL )	*p = 0;
		if ( ( p = strstr( lineBuf, ";"  ) ) != NULL )	*p = 0;

		// remove trailing whitespace
		p = lineBuf + strlen( lineBuf ) - 1;
		while ( p >= lineBuf && isspace( *p ) )
			*p-- = 0;

		// get section name if this is a section
		if ( lineBuf[ 0 ] == '[' )
		{
			strcpy( sectionName, lineBuf + 1 );
			if ( ( p = strstr( sectionName, "]" ) ) != NULL )  *p = 0;
			section = FindSection( sectionName );
		}
		else if ( strlen( lineBuf ) > 0 )
		{
			// add the filename to the filenames, uniquely
			int fileNum;
			for ( fileNum = 0 ; fileNum < d_filenames.Count() ; fileNum++ )
				if ( PathFile::FilenamesMatch( lineBuf, d_filenames[ fileNum ] ) )
					break;
			if ( fileNum == d_filenames.Count() )
			{
				d_filenames.Add( new char[ strlen( lineBuf ) + 1 ] );
				strcpy( d_filenames[ fileNum ], lineBuf );
			}

			// add the section if we don't have it
			if ( !section )
			{
				ASSERT( *sectionName != 0 );
				section = new Section;
				section->name = new char[ strlen( sectionName ) + 1 ];
				strcpy( section->name, sectionName );
				d_sections.Add( section );
			}

			// add the file to the section file list
			section->files.Add( fileNum );
		}
	}

	memFree(buffer);

	return true;
}

//============================================================================

bool FileLoader::LoadPermanentFiles()
{
	return LoadSection( "permanent", true );
}

//============================================================================

bool FileLoader::LoadWorldFiles( const char* worldName )
{
	int fails = 0;

	if ( ! LoadSection( "all", false ) )
		fails++;
	if ( ! LoadSection( worldName, false ) )
		fails++;

	return( fails == 0 );
}

//============================================================================

bool FileLoader::LoadWorldFilesAsync( const char* worldName, int groupId )
{
	int fails = 0;

	if ( ! LoadSectionAsync( worldName, groupId ) )
		fails++;
	if ( ! LoadSectionAsync( "all", groupId ) )
		fails++;

	return( fails == 0 );
}

//============================================================================

void FileLoader::Print()
{
	int i;
	for ( i = 0 ; i < d_filenames.Count() ; i++ )
		dbgPrint( "FileLoader::Filename[%3d] = '%s'\n", i, d_filenames[ i ] );

	for ( i = 0 ; i < d_sections.Count() ; i++ )
	{
		int j;
		for ( j = 0 ; j < d_sections[ i ]->files.Count() ; j++ )
			dbgPrint( "FileLoader::Section[%3d '%s'] File[%3d] = '%s'\n", i, d_sections[ i ]->name, j, d_filenames[ d_sections[ i ]->files[ j ] ] );
	}
}

//============================================================================

FileLoader::Section* FileLoader::FindSection( const char* name )
{
	for ( int i = 0 ; i < d_sections.Count() ; i++ )
		if ( stricmp( d_sections[ i ]->name, name ) == 0 )
			return d_sections[ i ];
	return NULL;
}

//============================================================================

bool FileLoader::LoadSection( const char* name, bool permanent )
{
	int fails = 0;
	Section* section = FindSection( name );

	if ( section )
	{
		//#ifdef CDROM
		//// "uber" file, try to load a merged file for quicker loads
		//LoadMergedSection( name, permanent );
		//#endif

		for ( int i = 0 ; i < section->files.Count() ; i++ )
		{
			if ( ! LoadFile( d_filenames[ section->files[ i ] ], permanent ) )
				fails++;
		}
	}

	return( fails == 0 );
}

//============================================================================

bool FileLoader::LoadSectionAsync( const char* name, int groupId )
{
	Section* section = FindSection( name );

	if ( section )
	{
		//#ifdef CDROM
		//// "uber" file, try to load a merged file for quicker loads
		//LoadMergedSection( name, permanent );
		//#endif

		for ( int i = 0 ; i < section->files.Count() ; i++ )
		{
			g_resourceManager.LoadFile( d_filenames[ section->files[ i ] ], groupId );
		}
	}

	return true;
}

//============================================================================

bool FileLoader::LoadFile( const char* filename, bool permanent )
{
	if ( strstr( filename, ".var" ) != NULL || strstr( filename, ".tbd" ) != NULL )
	{
		FileStream::PreLoad( (char*) filename );
		return true;
	}
	else if ( strstr( filename, ".sx" ) != NULL )
	{
		g_ScriptSystem.LoadExecutable( filename );
		return true;
	}
	else
	{
		t_Handle handle = dblLoad( filename );

		if ( handle != INVALID_HANDLE )
		{
			if ( permanent )
				dblSetPermanent( handle, true );

			if ( strstr( filename, ".mdb" ) )
				motLoadMotionChunk( filename );

			return true;
		}
		else
		{
			ASSERTF( false, ( "Error loading file '%s'", filename ) );
			return false;
		}
	}
}

//============================================================================

bool FileLoader::LoadMergedSection( const char* section, bool permanent )
{
	char filename[ 256 ];
	sprintf( filename, "%s.dbl", section );

	t_Handle handle = dblLoad( filename );

	if ( handle != INVALID_HANDLE )
	{
		if ( permanent )
			dblSetPermanent( handle, true );
		return true;
	}
	else
		return false;
}

//============================================================================
