////////////////////////////////////////////////////////////////////////////
//
// TupperwareList
//
// An list entity (stores a collection of single data type).
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWARELIST_H
#define TUPPERWARELIST_H

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWARE_ATOM_H
#include "Atom.h"
#endif

#ifndef TUPPERWARE_ARRAY_H
#include "tuparray.h"
#endif

#ifndef TUPPERWARE_STRING_H
#include "tupstring.h"
#endif



////////////////////////////////////////////////////////////////////////////

class TupperwareList : public TupperwareAtom 
{
public:
	TupperwareList();
	~TupperwareList();

	TupperwareAtom::AtomType GetAtomType() const { return TupperwareAtom::ATOM_LIST; } 	// Returns the actual type of this atom.

	TupperwareList(const char *key,const char *name,const float *values, int length);
	TupperwareList(const char *key,const char *name,const int *values, int length);
	TupperwareList(const char *key,const char *name,const char **values, int length);

	// Returns the type of data stored in this atom.
	DataType GetDataType(void) const;

	// Returns the length of the list.
	int GetLength(void) const;
	
	// Returns the list as a float/int/string.  A warning is issued if the 
	// list is not of the type requested.
	const float* GetAsFloat(void) const;
	float* GetAsFloat(void);
	void GetAsFloat(TupArray<float> &valueArray) const;

	const int* GetAsInt(void) const;
	int* GetAsInt(void);
	void GetAsInt(TupArray<int> &valueArray) const;

	const char** GetAsString(void) const;
	char** GetAsString(void);
	void GetAsString(TupArray<TupString> &valueArray) const;

	// Sets the list as a float/int/string list.  Clobbers any previous list.
	void SetAsFloat( const float* values, int length );
	void SetAsFloat( const TupArray<float> &valueArray );
	void SetAsInt( const int* values, int length );
	void SetAsInt( const TupArray<int> &valueArray );
	void SetAsString( const char** values, int length );
	void SetAsString( const TupArray<TupString> &values);

private:
	void FreeList();

private:
	int m_length;
	void* m_pList;

	DataType m_dataType;
};

////////////////////////////////////////////////////////////////////////////

#endif
