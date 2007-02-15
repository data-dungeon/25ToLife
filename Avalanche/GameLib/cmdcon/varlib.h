//============================================================================
//=
//= varlib.h - A library of var files.  The library stores files as binary
//=            and is more compact than storing var files individually,
//=            especially when many var files with the same variables are
//=            stored in a single lib.
//=
//= 				Create library files with the "varlib.pl" perl script
//=
//============================================================================

#ifndef _VARLIB_H
#define _VARLIB_H

class CVarLib
{
public:
	CVarLib();
	~CVarLib();

	bool Load( const char* libFilename );
	void Reset();

	const char* Name();

	bool Has( const char* varFilename );
	bool Execute( const char* varFilename );
	bool FindValueQuick( const char* varFilename, const char *varName, char *value, int len );

private:
	int  FindFile( const char* varFilename );

	char		d_name[ 64 ];
	int  		d_numFiles;
	char**	d_files;
	int		d_numStrings;
	char**	d_strings;
	char*		d_data;
};

#endif //_VARLIB_H

//============================================================================
