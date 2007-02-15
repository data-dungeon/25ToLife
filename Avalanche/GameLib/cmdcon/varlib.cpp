//============================================================================
//=
//= varlib.cpp - A library of var files.  The library stores files as binary
//=              and is more compact than storing var files individually,
//=              especially when many var files with the same variables are
//=              stored in a single lib.
//=
//=              Create library files with the "varlib.pl" perl script
//=
//============================================================================

#include "cmdcon/CmdConPCH.h"
#include "cmdcon/varlib.h"
#include "EngineHelper/pathfile.h"

//============================================================================

#define VARLIB_FILE_ID			"VLIB"
#define VARLIB_FILE_VERSION	0x100

struct VarLibFileHeader
{
	char	id[ 4 ];
	int	version;
	int	numFiles;
	int	fileDataSize;
	int	numStrings;
	int	stringDataSize;
};

#define VARLIB_UBYTE			0
#define VARLIB_SHORT			1
#define VARLIB_INT			2
#define VARLIB_FLOAT			3
#define VARLIB_STRING		4
#define VARLIB_NOPARM		5
#define VARLIB_VECTOR3UB	6
#define VARLIB_VECTOR4UB	7
#define VARLIB_VECTOR3		8
#define VARLIB_VECTOR4		9

#define VARLIB_CMD_SHIFT	0
#define VARLIB_TYPE_SHIFT	12
#define VARLIB_CMD_MASK		( ( 1 << VARLIB_TYPE_SHIFT ) - 1 )
#define VARLIB_TYPE_MASK	( 0xF << VARLIB_TYPE_SHIFT )

//============================================================================

CVarLib::CVarLib()
{
	d_data    = 0;
	d_files   = 0;
	d_strings = 0;
	Reset();
}

//============================================================================

CVarLib::~CVarLib()
{
	Reset();
}

//============================================================================

void CVarLib::Reset()
{
	if ( d_data )		{ memFree( d_data );    }
	if ( d_files )		{ delete [] d_files;   }
	if ( d_strings )	{ delete [] d_strings; }
	d_data       = 0;
	d_numFiles   = 0;
	d_files      = 0;
	d_numStrings = 0;
	d_strings    = 0;
	d_name[0]    = 0;
}

//============================================================================

const char* CVarLib::Name()
{
	return d_name;
}

//============================================================================

bool CVarLib::Load( const char* libFilename )
{
	// load the raw data
	ASSERT( d_data == NULL );
	s32 size;
	t_Error err;
	d_data = (char*) Media::Load( libFilename, &size, &err );
	if ( !d_data )
		return false;

	// validate our file
	VarLibFileHeader* hdr = (VarLibFileHeader*) d_data;
	if ( memcmp( hdr->id, VARLIB_FILE_ID, sizeof(hdr->id) ) != 0 )
	{
		ASSERTF( false, ( "VarLib::Load %s not a varlib file", libFilename ) );
		memFree( d_data );
		return false;
	}
	if ( hdr->version != VARLIB_FILE_VERSION )
	{
		ASSERTF( false, ( "VarLib::Load %s bad version, got %04x, need %04x", libFilename, hdr->id, VARLIB_FILE_VERSION ) );
		memFree( d_data );
		return false;
	}

	// remember library name
	ASSERT( strlen( libFilename ) < sizeof( d_name ) );
	strcpy( d_name, libFilename );

	// build our data to fast access the lib
	d_numFiles   = hdr->numFiles;
	d_numStrings = hdr->numStrings;
	d_files      = new char* [ d_numFiles ];
	d_strings    = new char* [ d_numStrings ];

	// fill in the file table
	int i;
	unsigned char* fileOffset = (unsigned char*) ( d_data + sizeof(VarLibFileHeader) );
	for ( i = 0 ; i < d_numFiles ; i++ )
	{
		d_files[ i ] = (char*) fileOffset;
		int fileSize = fileOffset[ 0 ] | ( fileOffset[ 1 ] << 8 );
		fileOffset += 2 + fileSize;

		// verify strings are in order, so we can binary search them
		ASSERT( i == 0 || PathFile::FilenameCompare( d_files[i-1] + 2, d_files[i] + 2 ) < 0 );
	}

	// fill in the string table
	char* stringOffset = d_data + sizeof(VarLibFileHeader) + hdr->fileDataSize;
	for ( i = 0 ; i < d_numStrings ; i++ )
	{
		d_strings[ i ] = stringOffset;
		stringOffset += strlen( stringOffset ) + 1;
	}

	return true;
}

//============================================================================

bool CVarLib::Has( const char* varFilename )
{
	return FindFile( varFilename ) >= 0;
}

//============================================================================

bool CVarLib::Execute( const char* varFilename )
{
	int index = FindFile( varFilename );
	if ( index >= 0 )
	{
		char* buf      = d_files[ index ];
		char* filename = buf + 2;
		char* end      = filename + ( ((u8*)buf)[0] | ((u8*)buf)[1] << 8 );
		buf            = filename + strlen( filename ) + 1;

		while ( buf < end )
		{
			unsigned short typeCmd = ( ((u8*)buf)[0] | ((u8*)buf)[1] << 8 );	buf += 2;
			int type = ( typeCmd & VARLIB_TYPE_MASK ) >> VARLIB_TYPE_SHIFT;
			int cmd  = ( typeCmd & VARLIB_CMD_MASK  ) >> VARLIB_CMD_SHIFT;

			// turn the binary command data into an ascii string to execute.
			//  -- yes, this is not the most efficient way to do this, but it reduces
			//     the code complexity considerably, and makes good reuse of
			//     well tested code.

			ASSERT( cmd >= 0 && cmd < d_numStrings );
			char  outCommand[ 128 ];
			char* p = outCommand;
			p += sprintf( p, "%s ", d_strings[ cmd ] );

			switch ( type )
			{
				case VARLIB_UBYTE:		{ u8* v = (u8*) buf; buf += 1; p += sprintf( p, "%d", v[0] ); break; }
				case VARLIB_SHORT:		{ short v;         memcpy( &v, buf, 2 ); buf += 2; p += sprintf( p, "%d", v ); break; }
				case VARLIB_INT:			{ int v;				 memcpy( &v, buf, 4 ); buf += 4; p += sprintf( p, "%d", v ); break; }
				case VARLIB_FLOAT:		{ float v;			 memcpy( &v, buf, 4 ); buf += 4; p += sprintf( p, "%f", v ); break; }
				case VARLIB_STRING:		{ while ( ( *p++ = *buf++ ) != 0 ) {}; break; }
				case VARLIB_NOPARM:     { break; }
				case VARLIB_VECTOR3UB:	{ u8* v = (u8*) buf; buf += 3; p += sprintf( p, "[%d %d %d]", v[0], v[1], v[2] ); break; }
				case VARLIB_VECTOR4UB:	{ u8* v = (u8*) buf; buf += 4; p += sprintf( p, "[%d %d %d %d]", v[0], v[1], v[2], v[3] ); break; }
				case VARLIB_VECTOR3:		{ Vector3Packed v; memcpy( &v, buf, 12 ); buf += 12; p += sprintf( p, "[%f %f %f]", v.x(), v.y(), v.z() ); break; }
				case VARLIB_VECTOR4:		{ Vector4Packed v; memcpy( &v, buf, 16 ); buf += 16; p += sprintf( p, "[%f %f %f %f]", v.x(), v.y(), v.z(), v.w() ); break; }
				default: ASSERT( false );
			}

			g_console.Process( outCommand );
		}

		return true;
	}
	else
	{
		return false;
	}
}

//============================================================================

bool CVarLib::FindValueQuick( const char* varFilename, const char *varName, char *value, int len )
{
	int index = FindFile( varFilename );
	if ( index < 0 )
		return false;

	char* buf      = d_files[ index ];
	char* filename = buf + 2;
	char* end      = filename + ( ((u8*)buf)[0] | ((u8*)buf)[1] << 8 );
	buf            = filename + strlen( filename ) + 1;

	while ( buf < end )
	{
		unsigned short typeCmd = ( ((u8*)buf)[0] | ((u8*)buf)[1] << 8 );	buf += 2;
		int type = ( typeCmd & VARLIB_TYPE_MASK ) >> VARLIB_TYPE_SHIFT;
		int cmd  = ( typeCmd & VARLIB_CMD_MASK  ) >> VARLIB_CMD_SHIFT;

		// turn the binary command data into an ascii string to execute.
		//  -- yes, this is not the most efficient way to do this, but it reduces
		//     the code complexity considerably, and makes good reuse of
		//     well tested code.

		ASSERT( cmd >= 0 && cmd < d_numStrings );

		char valueBuffer[ 128 ];
		valueBuffer[0] = '\0';
		switch ( type )
		{
			case VARLIB_UBYTE:		{ u8* v = (u8*) buf; buf += 1; sprintf( valueBuffer, "%d", v[0] ); break; }
			case VARLIB_SHORT:		{ short v;         memcpy( &v, buf, 2 ); buf += 2; sprintf( valueBuffer, "%d", v ); break; }
			case VARLIB_INT:			{ int v;				 memcpy( &v, buf, 4 ); buf += 4; sprintf( valueBuffer, "%d", v ); break; }
			case VARLIB_FLOAT:		{ float v;			 memcpy( &v, buf, 4 ); buf += 4; sprintf( valueBuffer, "%f", v ); break; }
			case VARLIB_STRING:		{ ASSERT(strlen(buf) < sizeof(valueBuffer) - 1); strcpy(valueBuffer, buf); buf += strlen(buf) + 1; break; }
			case VARLIB_NOPARM:     { break; }
			case VARLIB_VECTOR3UB:	{ u8* v = (u8*) buf; buf += 3; sprintf( valueBuffer, "[%d %d %d]", v[0], v[1], v[2] ); break; }
			case VARLIB_VECTOR4UB:	{ u8* v = (u8*) buf; buf += 4; sprintf( valueBuffer, "[%d %d %d %d]", v[0], v[1], v[2], v[3] ); break; }
			case VARLIB_VECTOR3:		{ Vector3Packed v; memcpy( &v, buf, 12 ); buf += 12; sprintf( valueBuffer, "[%f %f %f]", (float)v.x(), (float)v.y(), (float)v.z() ); break; }
			case VARLIB_VECTOR4:		{ Vector4Packed v; memcpy( &v, buf, 16 ); buf += 16; sprintf( valueBuffer, "[%f %f %f %f]", (float)v.x(), (float)v.y(), (float)v.z(), (float)v.w() ); break; }
			default: ASSERT( false );
		}

		if (strcmpi(d_strings[ cmd ], varName) == 0)
		{
			strncpy(value, valueBuffer, len);
			value[len] = '\0';
			return true;
		}
	}

	return false;
}

//============================================================================

int CVarLib::FindFile( const char* varFilename )
{
	// binary search the sorted list.
	int lo = 0;
	int hi = d_numFiles - 1;
	while ( lo <= hi )
	{
		int md  = ( lo + hi ) / 2;
		int cmp = PathFile::FilenameCompare( varFilename, d_files[ md ] + 2 );
		if      ( cmp < 0 ) hi = md - 1;
		else if ( cmp > 0 ) lo = md + 1;
		else                return md;
	}

	// not found
	return -1;
}

//============================================================================
