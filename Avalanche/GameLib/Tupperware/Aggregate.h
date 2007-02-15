////////////////////////////////////////////////////////////////////////////
//
// TupperwareAggregate
//
// An aggregate entity (stores a collection of other atoms).
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWAREAGGREGATE_H
#define TUPPERWAREAGGREGATE_H

////////////////////////////////////////////////////////////////////////////

#include <string.h>

#ifndef TUPPERWARE_ATOM_H
#include "Atom.h"
#endif

#ifndef TUPPERWARE_ARRAY_H
#include "tuparray.h"
#endif

#ifndef TUPPERWARE_ITERATOR_H
#include "Iterator.h"
#endif

////////////////////////////////////////////////////////////////////////////

class TupperwareAggregate : public TupperwareAtom 
{
	friend class TupperwareAtom;

public:
	TupperwareAggregate();
	~TupperwareAggregate();
	
	TupperwareAtom::AtomType GetAtomType() const { return TupperwareAtom::ATOM_AGGREGATE; } 	// Returns the actual type of this atom.

	// Returns an iterator that will iterate over every atom in this 
	// aggregate.
	TupperwareIterator FindAll() const;
	
	// Returns an iterator that will iterate over each atom in this
	// aggregate whose key matches the specified key.
	TupperwareIterator FindByKey( const char* key ) const;

	// Returns the atom/aggregate/list/scalar/comment of the specified key.
	// If the element is not found, NULL is returned.  If an element 
	// is found that is not the type requested, a warning is issued and NULL 
	// is returned.
	TupperwareAtom* FindAtomByKey( const char* key) const;
	TupperwareAggregate* FindAggregateByKey( const char* key) const;
	TupperwareList* FindListByKey( const char* key) const;
	TupperwareScalar* FindScalarByKey( const char* key) const;
	TupperwareComment* FindCommentByKey( const char* key) const;
	
	// Returns the atom/aggregate/list/scalar/comment of the specified key and 
	// name.  If the element is not found, NULL is returned.  If an element 
	// is found that is not the type requested, a warning is issued and NULL 
	// is returned.
	TupperwareAtom* FindAtomByKeyAndName( const char* key, const char* name) const;
	TupperwareAggregate* FindAggregateByKeyAndName( const char* key, const char* name) const;
	TupperwareList* FindListByKeyAndName( const char* key, const char* name) const;
	TupperwareScalar* FindScalarByKeyAndName( const char* key, const char* name ) const;
	TupperwareComment* FindCommentByKeyAndName( const char* key, const char* name ) const;
	
	// Returns a new aggregate/list/scalar/comment which is a child of this 
	// aggregate.
	TupperwareAggregate* AddAggregate( const char* key, const char* name = NULL, bool addToEnd = true );
	TupperwareList* AddList( const char* key, const char* name = NULL, bool addToEnd = true );
	TupperwareList* AddListFloat( const char* key, const char* name, const float* values, int num_values, bool addToEnd = true );
	TupperwareList* AddListFloat( const char* key, const char* name, const TupArray<float> &values, bool addToEnd = true );
	TupperwareList* AddListInt( const char* key, const char* name, const int* values, int num_values, bool addToEnd = true );
	TupperwareList* AddListInt( const char* key, const char* name, const TupArray<int> &values, bool addToEnd = true );
	TupperwareList* AddListString( const char* key, const char* name, const char** values, int num_values, bool addToEnd = true );
	TupperwareList* AddListString( const char* key, const char* name, const TupArray<TupString> &strings, bool addToEnd = true );
	TupperwareScalar* AddScalar( const char* key, const char* name = NULL, bool addToEnd = true );
	TupperwareScalar* AddScalarFloat( const char* key, const char* name, float value, bool addToEnd = true );
	TupperwareScalar* AddScalarInt( const char* key, const char* name, int value, bool addToEnd = true );
	TupperwareScalar* AddScalarString( const char* key, const char* name, const char* value, bool addToEnd = true );
	TupperwareScalar* AddScalarBinary( const char* key, const char* name, const void* value, int length, bool addToEnd = true );
	TupperwareComment* AddComment( const char* key, const char* name, const char* value,bool addToEnd = true );
	
	void DeleteAtom( TupperwareAtom* atom );
	void DeleteAtomsByKey(const char * key);
	void DeleteAtomsByKeyAndName(const char * key,const char * name);

	void MoveChildren( TupperwareAggregate *pSrc);
	void CopyChildren(const TupperwareAggregate *pSrc);
	void Copy( const TupperwareAtom* atom );
	
	void AddChild( TupperwareAtom* child, bool addToEnd = true );
	void InsertChild( TupperwareAtom* child, TupperwareAtom* sibling, bool insertAfter = true );
	TupperwareAtom *GetFirstChildAtom(void);
	TupperwareAtom *GetLastChildAtom(void);

	static int strnullcmp(const char *str1, const char *str2);
	
private:
	TupperwareAtom* m_pFirstChild;
	TupperwareAtom* m_pLastChild;
};

////////////////////////////////////////////////////////////////////////////

#include "Aggregate.hpp"

#endif
