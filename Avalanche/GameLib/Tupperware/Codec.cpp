////////////////////////////////////////////////////////////////////////////
//
// TupperwareCodec
//
// A class for encoding/decoding binary data into/from ascii printable data.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "TupperwarePch.h"
#include "Codec.h"

////////////////////////////////////////////////////////////////////////////

#define BASE  ( ' ' + 1 )

////////////////////////////////////////////////////////////////////////////

TupperwareCodec::TupperwareCodec() {
}


TupperwareCodec::~TupperwareCodec() {
}


char* TupperwareCodec::BinaryToAscii( const void* binary_data, int size_in_bytes ) {
	int length = size_in_bytes*2;
	char* string = new char[ length + 1 ];
	const char* data = ( const char* )binary_data;
	unsigned char a,b,dh,dl;

	for ( int i = 0, j = 0; i < size_in_bytes; i++, j+=2) {
		
		dh = data[i]>>4;
		dl = data[i]&0xf;
		a = dh<0xa ? dh+'0' : dh+('a'-0xa);
		b = dl<0xa ? dl+'0' : dl+('a'-0xa);
		string[j] = a;
		string[j+1] = b;
	}

	string[ length ] = '\0';

	char* s = Split( string, 64 );

	delete [] string;

	return s;
}


void* TupperwareCodec::AsciiToBinary( const char* string, int* length_ret ) {
	int length;
	char* s = UnSplit( string, &length );

	int size_in_bytes = length/2;
	char* data = new char[ size_in_bytes ];

	unsigned char a,b;
	for ( int i = 0, j = 0; i < size_in_bytes; i++, j += 2 ) {

		a = s[j];
		b = s[j+1];
		if (a>='0'&&b<='9')
			a-='0';
		else if (a>='a'&&a<='f')
			a-='a'-0xa;
		else if (a>='A'&&a<='F')
			a-='A'-0xa;
		else
			a=0;
		if (b>='0'&&b<='9')
			b-='0';
		else if (b>='a'&&b<='f')
			b-='a'-0xa;
		else if (b>='A'&&b<='F')
			b-='A'-0xa;
		else
			b=0;
		data[i] = a<<4|b;
	}

	delete [] s;

	if ( length_ret != NULL ) {
		*length_ret = size_in_bytes;
	}

	return data;
}


char* TupperwareCodec::Split( const char* string, int split ) {
	int length = strlen( string );
	int num_new_lines = length / split;
	char* t = new char[ length + num_new_lines + 1 ];

	num_new_lines = 0;
	for ( int n = 0, i = 0; i < length; i++, n++ ) {
		if ( n > split ) {
			t[ num_new_lines + i ] = '\n';
			t[ num_new_lines + i + 1 ] = string[ i ];
			num_new_lines++;
			n = 0;
		} else {
			t[ num_new_lines + i ] = string[ i ];
		}
	}

	t[ num_new_lines + length ] = '\0';

	return t;
}


char* TupperwareCodec::UnSplit( const char* string, int *length ) {
	char* t = new char[strlen(string) + 1];

	const char* scan = string;
	char* paste = t;
	while (*scan) {
		while (*scan && *scan != '\n') {
			scan++;
		}

		memcpy(paste, string, scan - string);
		paste += scan - string;

		if (*scan == '\n') {
			scan++;
			string = scan;
		}
	}
	*paste = 0;

	*length = paste - t;
	return t;
}

////////////////////////////////////////////////////////////////////////////
