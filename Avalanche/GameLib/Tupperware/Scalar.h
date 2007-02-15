////////////////////////////////////////////////////////////////////////////
//
// TupperwareScalar
//
// A scalar entity (stores a single value of a specific data type).
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWARESCALAR_H
#define TUPPERWARESCALAR_H

////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

#include "Atom.h"

////////////////////////////////////////////////////////////////////////////

class TupperwareScalar : public TupperwareAtom 
{

public:
	TupperwareScalar();
	TupperwareScalar(const char *key,const char *name,const float value);
	TupperwareScalar(const char *key,const char *name,const int value);
	TupperwareScalar(const char *key,const char *name,const char *value);
	TupperwareScalar(const char *key,const char *name,const void *value,int length);
	~TupperwareScalar();

	TupperwareAtom::AtomType GetAtomType() const { return TupperwareAtom::ATOM_SCALAR; } 	// Returns the actual type of this atom.
	
	// Returns the type of data stored in this atom.
	DataType GetDataType() const;
	int GetBinaryLength(void) const;
	
	// Returns the scalar as a float/int/string/binary.  A warning is issued 
	// if the scalar is not of the type requested.
	float GetAsFloat() const;
	int GetAsInt() const;
	const char* GetAsString() const;
	char* GetAsString();
	const void* GetAsBinary( int* length = 0 ) const;
	void* GetAsBinary( int* length = 0 );
	
	// Sets the scalar as a float/int/string/binary scalar.
	void SetAsFloat( float value );
	void SetAsInt( int value );
	void SetAsString( const char* value );
	void SetAsBinary( const void* value, int length);

private:
	void FreeData();

private:
	union {
		float m_float;
		int   m_int;
		char* m_pString;
		void* m_pBinary;
	};

	int m_binaryLength;
	DataType m_dataType;
};

////////////////////////////////////////////////////////////////////////////

#endif
