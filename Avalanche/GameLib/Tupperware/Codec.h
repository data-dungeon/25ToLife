////////////////////////////////////////////////////////////////////////////
//
// TupperwareCodec
//
// A class for encoding/decoding tupperware data.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWARECODEC_H
#define TUPPERWARECODEC_H

////////////////////////////////////////////////////////////////////////////

#include <stdarg.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////

class TupperwareCodec {
private:
	TupperwareCodec();
	~TupperwareCodec();

public:
	// Returns a new encoded string containing data in printable form.  The 
	// string should be delete[]'d by the caller.
	static char* BinaryToAscii( const void* data, int size_in_bytes );

	// Returns a new decoded data containing the binary expansion of string.  
	// The data should be delete[]'d by the caller.
	static void* AsciiToBinary( const char* string, int* length = NULL );

private:
	// Splits the string into printable lines of split characters each.
	static char* Split( const char* string, int split );

	// Removes new line characters inserted by Split() from the string.
	static char* UnSplit( const char* string, int *length );
};

////////////////////////////////////////////////////////////////////////////

#endif
