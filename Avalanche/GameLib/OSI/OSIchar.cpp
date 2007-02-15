//-------------------------------------------------------------------------------------------------
// OSIchar.cpp
//
// Controls single-byte / double-byte character switching within the OSI system.
// "OSIchar" is a generic type which represents either char or wchar_t, depending
//	on whether wide character support is enabled.
//
// To enable unicode/double-byte/wide character support, define WIDE_CHAR_SUPPORT (below).
// To use ansi/single-byte characters instead, comment out the WIDE_CHAR_SUPPORT definition.
//-------------------------------------------------------------------------------------------------
#include "OSIchar.hpp"


//-------------------------------------------------------------------------------------------------
//
//		Basic character / string conversion functions
//
//-------------------------------------------------------------------------------------------------
char* OSIstrcpy( char* destination, const char* source )
{
	return strcpy( destination, source );
}


//-------------------------------------------------------------------------------------------------
char* OSIstrcpy( char* destination, const wchar_t* source )
{
	const wchar_t* read = source;
	char* write = destination;

	while( *read )
	{
		*write = (char) *read;
		write++;
		read++;
	}
	*write = '\0';

	return destination;
}


//-------------------------------------------------------------------------------------------------
wchar_t* OSIstrcpy( wchar_t* destination, const char* source )
{
	const char* read = source;
	wchar_t* write = destination;

	while( *read )
	{
		*write = *read;
		write++;
		read++;
	}
	*write = '\0';

	return destination;
}

//-------------------------------------------------------------------------------------------------
//wchar_t* OSIstrcpy( wchar_t* destination, const wchar_t* source )
//{
//	return wcscpy( destination, source );
//}


//=================
//-------------------------------------------------------------------------------------------------
char* OSIstrncpy( char* destination, const char* source, const int destinationMaxLen  )
{
	char* result = strncpy( destination, source, destinationMaxLen );
	destination[ destinationMaxLen - 1 ] = '\0';
	return result;
}


//-------------------------------------------------------------------------------------------------
char* OSIstrncpy( char* destination, const wchar_t* source, const int destinationMaxLen )
{
	const wchar_t* read = source;
	char* write = destination;

	while( *read 
		&& (write - destination) < destinationMaxLen )
	{
		*write = (char) *read;
		write++;
		read++;
	}
	
	if( (write - destination) < destinationMaxLen )
		*write = '\0';
	else
		destination[ destinationMaxLen - 1 ] = '\0';

	return destination;
}


//-------------------------------------------------------------------------------------------------
wchar_t* OSIstrncpy( wchar_t* destination, const char* source, const int destinationMaxLen  )
{
	const char* read = source;
	wchar_t* write = destination;

	while( *read
		&& (write - destination) < destinationMaxLen )
	{
		*write = *read;
		write++;
		read++;
	}
	if( (write - destination) < destinationMaxLen )
		*write = L'\0';
	else
		destination[ destinationMaxLen - 1 ] = L'\0';

	return destination;
}


//-------------------------------------------------------------------------------------------------
//wchar_t* OSIstrncpy( wchar_t* destination, const wchar_t* source, const int destinationMaxLen  )
//{
//	return wcsncpy( destination, source, destinationMaxLen );
//}
