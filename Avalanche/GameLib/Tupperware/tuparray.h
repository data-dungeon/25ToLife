////////////////////////////////////////////////////////////////////////////
//
// TupArray
//
// A dynamic array class
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWARE_ARRAY_H
#define TUPPERWARE_ARRAY_H

#include <Assert.h>
#include <new.h>

#ifndef TUPPERWARE_ELEMENTS_H
#include "elements.h"
#endif

#ifndef TUPPERWARE_MEMORY_MANAGER_H
#include "memorymanager.h"
#endif

template<class TYPE>
class TupArray
{
public:
// Construction
	TupArray();
// copy constructor
	TupArray(const TupArray& arraySrc);
// assignment operator
	const TupArray& operator=(const TupArray& arraySrc);

// Attributes
	int GetSize() const;
	int GetUpperBound() const;
	void SetSize(int nNewSize, int nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	const TYPE& GetAt(int nIndex) const;
	TYPE& GetAt(int nIndex);
	void SetAt(int nIndex,const TYPE & newElement);

	// Direct Access to the element data (may return NULL)
	const TYPE* GetData() const;
	TYPE* GetData();

	// Potentially growing the array
	void SetAtGrow(int nIndex,const TYPE & newElement);
	int Add(const TYPE & newElement);
	int Append(const TupArray& src);
	void Copy(const TupArray& src);

	// overloaded operator helpers
	const TYPE& operator[](int nIndex) const;
	TYPE& operator[](int nIndex);

	// Operations that move elements around
	void InsertAt(int nIndex,const TYPE & newElement, int nCount = 1);
	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex,const TupArray* pNewArray);

// Implementation
protected:
	TYPE* m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount

public:
	~TupArray();

#ifdef USE_TUP_MEMORY_MANAGER
	void *operator new(size_t size);
	void operator delete(void *pBuffer);
	void *Allocate(size_t nSize);
	void FreeBuffer(void *pBuffer);
#endif

};

bool operator==(const TupString& s1, const TupString& s2);

#include "tuparray.hpp"

#endif
