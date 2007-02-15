////////////////////////////////////////////////////////////////////////////
//
// TupElements
//
// helper functions for the TupArray class
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWARE_ELEMENTS_H
#define TUPPERWARE_ELEMENTS_H

#include "string.h"

#undef TEMPLATE_SPECIALIZATION
#ifdef WIN32
// the pc can handle template specialization
#define TEMPLATE_SPECIALIZATION
#endif

template<class TYPE>
inline void TupConstructElements(TYPE* pElements, int nCount)
{
	// first do bit-wise zero initialization
	memset((void*)pElements, 0, nCount * sizeof(TYPE));

	// then call the constructor(s)
	for (; nCount--; pElements++)
		::new(((void*)pElements)) TYPE;
}

template<class TYPE>
inline void TupDestructElements(TYPE* pElements, int nCount)
{
	// call the destructor(s)
	for (; nCount--; pElements++)
		pElements->~TYPE();
}

template<class TYPE>
inline void TupCopyElements(TYPE* pDest, const TYPE* pSrc, int nCount)
{
	// default is element-copy using assignment
	while (nCount--)
		*pDest++ = *pSrc++;
}

template<class TYPE>
inline bool TupCompareElements(const TYPE* pElement1, const TYPE* pElement2)
{
	return *pElement1 == *pElement2;
}

class TupString;

#ifdef TEMPLATE_SPECIALIZATION
template<> void TupConstructElements<TupString> (TupString* pElements, int nCount);
template<> void TupDestructElements<TupString> (TupString* pElements, int nCount);
template<> void TupCopyElements<TupString> (TupString* pDest, const TupString* pSrc, int nCount);
#else
void TupConstructElements(TupString* pElements, int nCount);
void TupDestructElements(TupString* pElements, int nCount);
void TupCopyElements(TupString* pDest, const TupString* pSrc, int nCount);
#endif

#endif