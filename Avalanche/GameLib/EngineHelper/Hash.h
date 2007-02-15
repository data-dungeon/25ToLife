//============================================================================
//=
//= hash.h - Hash an arbitrary length series of bytes to a 32-bit number
//=
//============================================================================

#ifndef _HASH_H
#define _HASH_H

class Hash
{
public:
	static unsigned int CalcChecksum( const void* buf, int length, unsigned int initVal = 0 );
};

#endif //_HASH_H
