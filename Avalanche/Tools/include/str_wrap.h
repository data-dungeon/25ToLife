#ifndef __STR_WRAP_H
#define __STR_WRAP_H

#include <istream.h>

#define SW_CHAR_BUFFER			256

#define SWGood( ) ((pStream->good( )) ? true : false)


class StreamWrapper
{

	bool				bOwnStream;
	char				cCharBuffer[SW_CHAR_BUFFER];

	void				ResetStreamData( void);
protected:
	istream			*pStream;
	long				lStreamLength;

	bool				ReleaseStream( );
	virtual void	StreamChanged( void) { }

public:
	StreamWrapper( );
	StreamWrapper( const char *pcFName);
	StreamWrapper( istream &str);
	~StreamWrapper( );


	bool				Open( const char *pcFName);
	bool				Attach( istream &str);

	bool 				IsOpen( void);

	long				GetLength( );
	bool				SWGetString( char **ppCharPtr, char delimeter);

} ;

#endif __STR_WRAP_H