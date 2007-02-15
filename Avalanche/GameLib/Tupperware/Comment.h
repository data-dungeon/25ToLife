////////////////////////////////////////////////////////////////////////////
//
// TupperwareComment
//
// An atom that consists of a comment string
// used for adding comments that will be saved out in the ascii version of the file
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWARECOMMENT_H
#define TUPPERWARECOMMENT_H

////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

#ifndef TUPPERWARE_ATOM_H
#include "Atom.h"
#endif

////////////////////////////////////////////////////////////////////////////

class TupperwareComment : public TupperwareAtom 
{

public:
	TupperwareComment();
	TupperwareComment(const char *key,const char *name,const char *value);
	~TupperwareComment();

	TupperwareAtom::AtomType GetAtomType() const { return TupperwareAtom::ATOM_COMMENT; } 	// Returns the actual type of this atom.
	
	const char* GetValue() const;
	char* GetValue();

	void SetValue( const char* value );

private:
	void FreeData();

private:
	char* m_pValue;
};

////////////////////////////////////////////////////////////////////////////

#endif
