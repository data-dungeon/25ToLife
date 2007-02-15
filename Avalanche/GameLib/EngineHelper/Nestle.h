////////////////////////////////////////////////////////////////////////////
//
// Nestle.h
//
// Crunches 8-bit/byte character strings into 6-bit/byte character strings.
//
// Nate Robins, February 2001.
//
////////////////////////////////////////////////////////////////////////////

#ifndef NESTLE_H
#define NESTLE_H


////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////

class Nestle
{
public:

	// Returns a new crunched string (should be delete []'d).
	static char* Crunch( char* uncrunched_string );

	// Returns a new expanded string (should be delete []'d).
	static char* UnCrunch( char* crunched_string );

	// Returns the number of bytes in the crunched string (not including NULL).
	static int CrunchLength( char* crunched_string );

	// Returns the number of bytes the crunched string would expand into (not including NULL).
	static int UnCrunchLength( char* crunched_string );

private:

	// Returns a crunched (6 bits) character.
	static char CrunchChar( char c );

	// Returns an expanded (8 bits) character.
	static char UnCrunchChar( char c );
};

////////////////////////////////////////////////////////////////////////////

#endif	// NESTLE_H
