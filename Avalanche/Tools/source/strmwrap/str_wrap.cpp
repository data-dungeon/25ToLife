#include <assert.h>
#include <fstream.h>
#include <stdlib.h>
#include <string.h>

#include "str_wrap.h"


StreamWrapper::StreamWrapper( void)
{
	pStream = NULL;


}

StreamWrapper::StreamWrapper( const char *pcFName)
{

	pStream = NULL;

	Open( pcFName);

}

StreamWrapper::StreamWrapper( istream &str)
{
	pStream = NULL;

	Attach( str);
}


bool StreamWrapper::Open( const char *pcFName)
{

	if( ReleaseStream( ) == false)
		return false;

	bOwnStream = true;

	StreamChanged( );

	ResetStreamData( );

	pStream = new ifstream( pcFName, ios::binary | ios::nocreate);

	return pStream->good( ) ? true : false;
}

bool StreamWrapper::Attach( istream &str)
{

	if( ReleaseStream( ) == false)
		return false;

	bOwnStream = false;

	if( !str.good( ))
		return false;

	StreamChanged( );

	ResetStreamData( );

	pStream = &str;

	return true;
}

long StreamWrapper::GetLength( void)
{
streampos				current;

	current = pStream->tellg( );

	pStream->seekg( 0, ios::end);

	lStreamLength = pStream->tellg( );

	pStream->seekg( current);

	if( !pStream->good( ))
		lStreamLength = -1;

	return lStreamLength;
}


void StreamWrapper::ResetStreamData( void)
{
	lStreamLength = -1;

}

bool StreamWrapper::ReleaseStream( void)
{
	if( pStream == NULL)
		return true;

	if( bOwnStream)
		delete *pStream;

	pStream == NULL;

	return true;
}

bool StreamWrapper::SWGetString( char **pCharPtr, char delimeter)
{

int					i;

	assert( pStream != NULL);
	
	i = 0;

	do
	{
		if( i == (SW_CHAR_BUFFER - 1))
			return false;

		pStream->read( cCharBuffer + i, 1);
	} while( pStream->good( )  && cCharBuffer[i++] != delimeter);

	if( i == 0)
		*pCharPtr == NULL;
	else
	{
		*pCharPtr = (char *) malloc( i);
		memcpy( *pCharPtr, cCharBuffer, i);
	}

	return true;
}


StreamWrapper::~StreamWrapper( )
{
	if( bOwnStream)
		delete pStream;
}