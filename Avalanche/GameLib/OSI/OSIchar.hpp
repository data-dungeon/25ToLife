//-------------------------------------------------------------------------------------------------
// OSIchar.hpp
//
// Controls single-byte / double-byte character switching within the OSI system.
// "OSIchar" is a generic type which represents either char or wchar_t, depending
//	on whether wide character support is enabled.
//
// To enable unicode/double-byte/wide character support, define WIDE_CHAR_SUPPORT (below).
// To use ansi/single-byte characters instead, comment out the WIDE_CHAR_SUPPORT definition.
//-------------------------------------------------------------------------------------------------
#ifndef _include_OSIchar_
#define _include_OSIchar_


//-------------------------------------------------------------------------------------------------
// Included header files
//-------------------------------------------------------------------------------------------------
// #include <wchar.h> // OSIFIXME AV
#include <string.h>

#define NUM_ARRAY_ELEMENTS( A ) ( sizeof( A ) / sizeof( A[0] ) )

//-------------------------------------------------------------------------------------------------
// Wide characters (a.k.a. double-byte, unicode)
//
//#define WIDE_CHAR_SUPPORT
#ifdef WIDE_CHAR_SUPPORT
//-------------------------------------------------------------------------------------------------
typedef wchar_t OSIchar;
//#define OSIstrcpy	wcscpy
#define OSIstrcat	wcscat
#define OSIstrcmp	wcscmp
#define OSIstricmp	_wcsicmp
#define OSIstrcmpi	_wcsicmp
#define OSIsprintf	swprintf
#define OSIstrchr	wcschr
#define OSIstrrchr  wcsrchr
#define OSIstrcspn	wcscspn
#define OSIstrdup	CANNOT_USE_THIS
#define OSIstrlen	wcslen
#define OSIstrncat	wcsncat
#define OSIstrncmp	wcsncmp
//#define OSIstrncpy	wcsncpy
#define OSIstrnicmp	_wcsnicmp
#define OSIstrpbrk	wcspbrk
#define OSIstrrchr	wcsrchr
#define OSIstrspn	wcsspn
#define OSIstrstr	wcsstr
#define OSIstrtok	wcstok
#define OSIvsprintf	vswprintf
#define OSIsnprintf	_snwprintf
#define OSIvsnprintf _vsnwprintf
//#define OSI_STRING_CONVERT	L // When wide chars are enabled, this macro declares hardcoded strings as unicode
#define OSI_STRING_LITERAL( stringConstant )	L##stringConstant

//-------------------------------------------------------------------------------------------------
// Single-byte characters (a.k.a. ANSI)
//
#else // !WIDE_CHAR_SUPPORT
//-------------------------------------------------------------------------------------------------
typedef char OSIchar;
//#define OSIstrcpy	strcpy
#define OSIstrcat	strcat
#define OSIstrcmp	strcmp
#define OSIstricmp	stricmp
#define OSIstrcmpi	stricmp
#define OSIsprintf	sprintf
#define OSIstrchr	strchr
#define OSIstrcspn	strcspn
#define OSIstrdup	CANNOT_USE_THIS
#define OSIstrlen	strlen
#define OSIstrncat	strncat
#define OSIstrncmp	strncmp
//#define OSIstrncpy	strncpy
#define OSIstrnicmp	_strnicmp
#define OSIstrpbrk	strpbrk
#define OSIstrrchr	strrchr
#define OSIstrspn	strspn
#define OSIstrstr	strstr
#define OSIstrtok	strtok
#define OSIvsprintf	vsprintf
#define OSIsnprintf	_snprintf
#define OSIvsnprintf _vsnprintf
//#define OSI_STRING_CONVERT // When wide chars are disabled, this macro does nothing
#define OSI_STRING_LITERAL( stringConstant )	stringConstant


#endif // !WIDE_CHAR_SUPPORT


//-------------------------------------------------------------------------------------------------
// Basic character / string conversion functions
//-------------------------------------------------------------------------------------------------
char* OSIstrcpy( char* destination, const char* source );
char* OSIstrcpy( char* destination, const wchar_t* source );
wchar_t* OSIstrcpy( wchar_t* destination, const char* source );
//wchar_t* OSIstrcpy( wchar_t* destination, const wchar_t* source );

char* OSIstrncpy( char* destination, const char* source, const int destinationMaxLen );
char* OSIstrncpy( char* destination, const wchar_t* source, const int destinationMaxLen  );
wchar_t* OSIstrncpy( wchar_t* destination, const char* source, const int destinationMaxLen  );
//wchar_t* OSIstrncpy( wchar_t* destination, const wchar_t* source, const int destinationMaxLen  );



#endif // _include_OSIchar_