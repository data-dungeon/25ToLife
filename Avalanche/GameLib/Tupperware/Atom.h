////////////////////////////////////////////////////////////////////////////
//
// TupperwareAtom
//
// Base class for Tupperware entities.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWARE_ATOM_H
#define TUPPERWARE_ATOM_H

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWARE_MEMORY_MANAGER_H
#include "memorymanager.h"
#endif

class TupperwareAggregate;

class TupperwareAtom {
	friend class TupperwareAggregate;
	friend class TupperwareIterator;

public:
	enum AtomType {
		ATOM_UNKNOWN,
		ATOM_AGGREGATE,
		ATOM_LIST,
		ATOM_SCALAR,
		ATOM_COMMENT,
	};
	
	enum DataType {
		DATA_UNKNOWN,
		DATA_STRING,
		DATA_FLOAT,
		DATA_INT,
		DATA_BINARY,
	};
	
	TupperwareAtom();   
	TupperwareAtom(const char* key , const char* name);
	virtual ~TupperwareAtom()=0; // to make this an abstract class

	void Detatch(void);

	// Returns the actual type of this atom.
	virtual AtomType GetAtomType() const { return TupperwareAtom::ATOM_UNKNOWN; }

	// Returns the key/name of this atom.
	const char* GetKey() const;
	const char* GetName() const;

	// Set the key/name of this atom.
	void SetKey( const char* key );
	void SetName( const char* name );

	TupperwareAggregate *GetParent(void) const;
	TupperwareAtom *GetPrevAtom(void) const;
	TupperwareAtom *GetNextAtom(void) const;

#ifdef TUP_EXT
	void *GetData(void) const;
	void SetData(void *pData);
#endif // TUP_EXT

#ifdef USE_TUP_MEMORY_MANAGER
	void *operator new(size_t size);
	void operator delete(void *pBuffer);
#endif

private:
	char* m_pKey;
	char* m_pName;
	
	TupperwareAggregate *m_pParent;
	TupperwareAtom* m_pNext;
	TupperwareAtom* m_pPrev;
#ifdef TUP_EXT
	void *m_pData;
#endif // TUP_EXT
#ifdef TRACK_LINE_NUMBERS
	int m_bufferIndex;
	int m_startingLineNum;
	int m_startingCharPos;
	int m_endingLineNum;
	int m_endingCharPos;
public:
	int GetBufferIndex(void) const { return m_bufferIndex; }
	int GetStartingLineNum(void) const { return m_startingLineNum; }
	int GetEndingLineNum(void) const { return m_endingLineNum; }
	int GetStartingCharPos(void) const { return m_startingCharPos; }
	int GetEndingCharPos(void) const { return m_endingCharPos; }
private:
#endif



};

////////////////////////////////////////////////////////////////////////////

#endif
