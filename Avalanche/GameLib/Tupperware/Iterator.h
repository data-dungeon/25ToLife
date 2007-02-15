////////////////////////////////////////////////////////////////////////////
//
// TupperwareIterator
//
// An iterator class to traverse a tupperware structure.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWARE_ITERATOR_H
#define TUPPERWARE_ITERATOR_H

////////////////////////////////////////////////////////////////////////////

class TupperwareAggregate;
class TupperwareAtom;
class TupperwareList;
class TupperwareScalar;
class TupperwareComment;

////////////////////////////////////////////////////////////////////////////

class TupperwareIterator 
{
public:

	TupperwareIterator();
	TupperwareIterator( TupperwareAtom* root );
	TupperwareIterator( const char* key, TupperwareAtom* root );
	~TupperwareIterator();
	
	// Return the current element being iterated as an atom/aggregate/list/
	// scalar.
	TupperwareAtom* GetAsAtom() const;
	TupperwareAggregate* GetAsAggregate() const;
	TupperwareList* GetAsList() const;
	TupperwareScalar* GetAsScalar() const;
	TupperwareComment* GetAsComment() const;
	
	// Go to the next element in the iteration.
	void Next();
	
	// Have all elements been iterated through?
	bool Finished() const;
	
private:
	const char* m_pKey;
	TupperwareAtom* m_pAtom;
};

////////////////////////////////////////////////////////////////////////////

#endif
