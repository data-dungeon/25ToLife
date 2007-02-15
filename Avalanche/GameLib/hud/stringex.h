#ifndef __STRINGEX_H__
#define __STRINGEX_H__

//=============================================================================
// Most of the functions here are designed to work with strings that may or may
// not be null.  While strlen() crashes if the string is 0, xstrlen() will just
// return 0, eliminating the need to constantly check for valid pointers when
// working with strings.
//=============================================================================

//=============================================================================
// Printf() - DO NOT SAVE OR REUSE POINTERS RETURNED BY THIS FUNCTION!!!
//				  Used primarily as a way of passing formatted strings to existing
//				  functions which are unable to format strings on their own.
//				  It employs a rotating buffer to make sure the 8 most recently
//				  returned pointers are valid at any time.
//				  The return value is ALWAYS valid, so there is no need to check
//				  for null under any circumstances.
//
// Example A: SetWindowText(hWnd, Printf("ToadPad - %s", pDoc->GetName()));
// Example B: FILE *f = fopen(Printf("%s%s%s", strDrive, strPath, strFileName), "wb" );
//=============================================================================

char *Printf( const char *fmt, ... );
int xlower( int ch );									// TODO: eventually this function will be replaced by a look-up table of the same name
int xupper( int ch );									// TODO: eventually this function will be replaced by a look-up table of the same name
int xstrlen( const char *str );
int xstrcpy( char *to, const char *from );
int xstrncpy( char *to, const char *from, int size );
int xstrncpyz( char* a, const char* b, int size );
int xstrncat( char *to, const char *from, int size );
int xstrncatz( char *to, const char *from, int size );
char *xstrdup( const char *str );
int xstrcmp( const char *a, const char *b );	
int xstricmp( const char *a, const char *b );
int xstrsort( const char *a, const char *b );			// string sort case-sensitive: returns non-zero if the strings need to be swapped
int xstrisort( const char *a, const char *b );			// string sort case-insensitive: returns non-zero if the strings need to be swapped

class xstring
{
	public:
		xstring();
		virtual ~xstring();

		virtual void alloc( int maxstring );			// allocate some memory
		virtual void reAlloc( int maxstring );			// make the buffer larger without discarding the contents
		virtual void optimize( void );					// optimize memory usage
		virtual void release( void );						// free memory
		virtual void set( const char *string );		// set the string
		virtual void set( int i );
		virtual void set( float f );
		virtual char *get( void );							// get a pointer to the string
		virtual char getch( int i );
		virtual void setch( int i, char ch );
		virtual int length( void );						// get the length of the string
		virtual int atoi( void );
		virtual float atof( void );
		virtual int compare( const char *string );
		virtual int compare( xstring &string );
		virtual int xstrcmp( const char *string );
		virtual int xstrcmp( xstring &string );
		virtual int xstricmp( const char *string );
		virtual int xstricmp( xstring &string );
		virtual int same( xstring &string );
		virtual int same( const char *string );
		virtual int diff( xstring &string );
		virtual int diff( const char *string );
		virtual void copy( const char *string );
		virtual void copyn( const char *string, int count );
	
	protected:
		int m_buffsize;
		char *m_string;
};

#endif // __STRINGEX_H__