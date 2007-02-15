//=============================================================================//
//  Copyright ©2001 Avalanche Software.  All Rights Reserved.                  //
//=============================================================================//
//      File: tstring.h                                                        //
//    Author: Daniel Silver                                                    //
//   Created: 04/23/2001                                                       //
//=============================================================================//

#ifndef __TSTRING_H__
#define __TSTRING_H__

//=============================================================================
// Most of the functions here are designed to work with strings that may or may
// not be null.  While strlen() crashes if the string is 0, tstrlen() will just
// return 0, eliminating the need to constantly check for valid pointers when
// working with strings.
//=============================================================================

//=============================================================================
// Printf() - DO NOT SAVE OR REUSE POINTERS RETURNED BY THIS FUNCTION!!!
//			  Used primarily as a way of passing formatted strings to existing
//			  functions which are unable to format the strings on their own.
//			  It employs a rotating buffer to make sure the 8 most recently
//			  returned pointers are valid at any time.
//			  The return value is ALWAYS valid, so there is no need to check
//			  for null under any circumstances.
//
// Example A: SetWindowText(hWnd, Printf("ToadPad - %s", pDoc->GetName()));
// Example B: FILE *f = fopen(Printf("%s%s%s", strDrive, strPath, strFileName), "wb" );
//=============================================================================

char *Printf( const char *fmt, ... );
char *TrueOrFalse( bool Value );
char tlower( char ch );
char tupper( char ch );
int tstrlen( const char* str );
void tstrcpy( char* to, const char* from );
void tstrncpy( char* to, const char* from, int size );
char* tstrdup( const char* str );
int tstrcmp( const char* a, const char* b );
int tstricmp( const char* a, const char* b );
const char* tstrstr( const char* a, const char* b );
const char* tstristr( const char* a, const char* b );


class tstring
{
	public:
		tstring();
		virtual ~tstring();

		int reserve( int size );
		void release( void );
		int length( void );
		void set( const char *pText );
		void set( const char *pText, int MaxSize );
		char *get( void );
		char getch( int i );
		int strcmp( const char *pText );
		int stricmp( const char *pText );
		void tolower( void );
		void toupper( void );
		bool equals( const char* pText );
		bool notequals( const char* pText );
		void _tofilepath( const char *pFullPath );
		void _tofilename( const char *pFullPath );
		void _tofileext( const char *pFileName );

	private:
		char *m_string;
		int m_buffsize;
};

class filename : public tstring
{
public:
	filename( void );
	filename( const char *name );

	void driveletter( void );
	void driveletter( const char *path );

	void filepath( void );
	void filepath( const char *path );

	void relativepath( const char *currentpath );
	void relativepath( const char *path, const char *currentpath );

	void getfilename( void );
	void getfilename( const char *path );

	void getext( void );
	void getext( const char *name );

	void delext( void );
	void delext( const char *name );
};

typedef filename FILENAME;

#endif // __TSTRING_H__