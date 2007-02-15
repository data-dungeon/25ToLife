////////////////////////////////////////////////////////////////////////////
//
// TupperwareIterator
//
// An iterator class to traverse a tupperware structure.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "TupperwarePch.h"
#include "Iterator.h"
#include "Atom.h"
#include "Log.h"

////////////////////////////////////////////////////////////////////////////

TupperwareIterator::TupperwareIterator() {
	m_pKey = NULL;
	m_pAtom = NULL;
}

TupperwareIterator::TupperwareIterator( TupperwareAtom* root ) {
	m_pKey = NULL;
	m_pAtom = root;
}


TupperwareIterator::TupperwareIterator( const char* key, TupperwareAtom* root ) {
	m_pKey = key;
	m_pAtom = root;

	if ( m_pAtom != NULL && strcmp( m_pAtom->m_pKey, key ) != 0 ) {
		Next();
	}
}


TupperwareIterator::~TupperwareIterator() {
	m_pKey = NULL;
	m_pAtom = NULL;
}


TupperwareAtom* TupperwareIterator::GetAsAtom() const {
	return m_pAtom;
}


TupperwareAggregate* TupperwareIterator::GetAsAggregate() const {
	if ( m_pAtom->GetAtomType() != TupperwareAtom::ATOM_AGGREGATE ) {
		TupperwareLog::Write( "GetAsAggregate(): Atom type does not match requested type.\n" );
		return NULL;
	}

	return (TupperwareAggregate*)m_pAtom;
}


TupperwareList* TupperwareIterator::GetAsList() const {
	if ( m_pAtom->GetAtomType() != TupperwareAtom::ATOM_LIST ) {
		TupperwareLog::Write( "GetAsList(): Atom type does not match requested type.\n" );
		return NULL;
	}

	return (TupperwareList*)m_pAtom;
}


TupperwareScalar* TupperwareIterator::GetAsScalar() const {
	if ( m_pAtom->GetAtomType() != TupperwareAtom::ATOM_SCALAR ) {
		TupperwareLog::Write( "GetAsScalar(): Atom type does not match requested type.\n" );
		return NULL;
	}

	return (TupperwareScalar*)m_pAtom;
}

TupperwareComment* TupperwareIterator::GetAsComment() const {
	if ( m_pAtom->GetAtomType() != TupperwareAtom::ATOM_COMMENT ) {
		TupperwareLog::Write( "GetAsComment(): Atom type does not match requested type.\n" );
		return NULL;
	}

	return (TupperwareComment*)m_pAtom;
}



void TupperwareIterator::Next() {
	m_pAtom = m_pAtom->m_pNext;

	if ( m_pKey != NULL )
	{
		while ( m_pAtom != NULL ) {
			if ( strcmp( m_pAtom->m_pKey, m_pKey ) == 0 ) {
				break;
			}

			m_pAtom = m_pAtom->m_pNext;
		}
	}
}


bool TupperwareIterator::Finished() const {
	return m_pAtom == NULL;
}

////////////////////////////////////////////////////////////////////////////
