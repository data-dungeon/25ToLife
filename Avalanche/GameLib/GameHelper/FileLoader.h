//============================================================================
//=
//= FileLoader.h - manages loading of dbl's into the game from a
//=                list file loaded from disk.
//=
//============================================================================

#ifndef _FILELOADER_H
#define _FILELOADER_H

#include "container/Darray.h"

//============================================================================

class FileLoader
{
public:
	FileLoader();
	~FileLoader();

	bool LoadSpecificationFile( const char* filename );
	bool LoadPermanentFiles();
	bool LoadWorldFiles( const char* worldName );
	bool LoadWorldFilesAsync( const char* worldName, int groupId );

	void Print();

private:
	struct Section
	{
		char*					name;
		DynamicArray<int> files;
	};

	Section* FindSection( const char* name );
	bool		LoadSection( const char* name, bool permanent );
	bool		LoadSectionAsync( const char* name, int groupId );
	bool     LoadFile( const char* filename, bool permanent );
	bool		LoadMergedSection( const char* section, bool permanent );

	DynamicArray<char*>		d_filenames;
	DynamicArray<Section*>	d_sections;
};

extern FileLoader g_fileLoader;

//============================================================================

#endif //_FILELOADER_H
