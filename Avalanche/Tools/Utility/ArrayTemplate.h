////////////////////////////////////////////////////////////////////////////////////
// ARRAY TEMPLATED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////

template<class TYPE> extern void CopyArray( TYPE* pDest, const TYPE* pSrc, int nCount );
template<class TYPE> extern void DeconstructArray( TYPE *pElements,int nCount );
template<class TYPE> extern void ConstructArray( TYPE* pElements, int nCount );
template<class TYPE> extern void SetArraySize( TYPE **pElements, UINT nNewSize, UINT &nSize, UINT &nMaxSize, int nGrowBy = 0 );
template<class TYPE> extern void RemoveArrayElements( TYPE *pElements, UINT nIndex, UINT &nSize, UINT nCount = 1 );

template<class TYPE>
extern inline void CopyArray( TYPE* pDest, const TYPE* pSrc, int nCount )
{
   // default is element-copy using assignment
   while (nCount--)
      *pDest++ = *pSrc++;
}

template<class TYPE>
extern inline void ConstructArray( TYPE* pElements, int nCount )
{
   // first do bit-wise zero initialization
   memset((void*)pElements, 0, nCount * sizeof(TYPE));

   // then call the constructor(s)
   for (; nCount--; pElements++)
      ::new(((void*)pElements)) TYPE;
}

template<class TYPE>
extern inline void DeconstructArray( TYPE *pElements,int nCount )
{
   // call the destructor(s)
   for (; nCount--; pElements++)
      pElements->~TYPE();
}

template<class TYPE>
extern inline void SetArraySize( TYPE **pElements, UINT nNewSize, UINT &nSize, UINT &nMaxSize, int nGrowBy )
{
   ASSERT( nNewSize >= 0 );
   ASSERT( nGrowBy >=0 );

   if (nNewSize == 0)
   {
      // shrink to nothing
      if (*pElements != NULL)
      {
         for( UINT i = 0; i < nSize; i++ )
            (*pElements + i)->~TYPE();
         delete[] (BYTE*)*pElements;
         *pElements = NULL;
      }
      nSize = nMaxSize = 0;
   }
   else if (*pElements == NULL)
   {
      // create buffer big enough to hold number of requested elements or
      // m_nGrowBy elements, whichever is larger.
      UINT nAllocSize = max((int)nNewSize, nGrowBy);
      *pElements = (TYPE*) new BYTE[(size_t)nAllocSize * sizeof(TYPE)];
      memset((void*)*pElements, 0, (size_t)nAllocSize * sizeof(TYPE));
      for( UINT i = 0; i < nNewSize; i++ )
         ::new( (void*)( *pElements + i ) ) TYPE;
      nSize = nNewSize;
      nMaxSize = nAllocSize;
   }
   else if (nNewSize <= nMaxSize)
   {
      // it fits
      if (nNewSize > nSize)
      {
         // initialize the new elements
         memset((void*)(*pElements + nSize), 0, (size_t)(nNewSize-nSize) * sizeof(TYPE));
         for( UINT i = 0; i < nNewSize-nSize; i++ )
            ::new( (void*)( *pElements + nSize + i ) ) TYPE;
      }
      else if (nSize > nNewSize)
      {
         // destroy the old elements
         for( UINT i = 0; i < nSize-nNewSize; i++ )
            (*pElements + nNewSize + i)->~TYPE();
      }
      nSize = nNewSize;
   }
   else
   {
      // otherwise, grow array
      if (nGrowBy == 0)
      {
         // heuristically determine growth when nGrowBy == 0
         //  (this avoids heap fragmentation in many situations)
         UINT nGrowBy = nSize / 8;
         nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
      }

      UINT nNewMax;
      if (nNewSize < nMaxSize + nGrowBy)
         nNewMax = nMaxSize + nGrowBy;  // granularity
      else
         nNewMax = nNewSize;  // no slush

      ASSERT(nNewMax >= nMaxSize);  // no wrap around

      TYPE* pNewData = (TYPE*) new BYTE[(size_t)nNewMax * sizeof(TYPE)];

      // copy new data from old
      memcpy(pNewData, *pElements, (size_t)nSize * sizeof(TYPE));

      // construct remaining elements
      ASSERT(nNewSize > nSize);
      memset((void*)(pNewData + nSize), 0, (size_t)(nNewSize-nSize) * sizeof(TYPE));
      for( UINT i = 0; i < nNewSize-nSize; i++ )
         ::new( (void*)( pNewData + nSize + i ) ) TYPE;

      // get rid of old stuff (note: no destructors called)
      delete[] (BYTE*)*pElements;
      *pElements = pNewData;
      nSize = nNewSize;
      nMaxSize = nNewMax;
   }
}

template<class TYPE>
extern inline void RemoveArrayElements( TYPE *pElements, UINT nIndex, UINT &nSize, UINT nCount )
{
   ASSERT(nIndex >= 0);
   ASSERT(nCount >= 0);
   ASSERT(nIndex + nCount <= nSize);

   if(nIndex < 0 || nCount < 0 || (nIndex + nCount > nSize))
      AfxThrowInvalidArgException();

   // just remove a range
   UINT nMoveCount = nSize - (nIndex + nCount);
   for( UINT i = 0; i < nCount; i++ )
      (pElements + nIndex + i)->~TYPE();
   if (nMoveCount)
      memmove( pElements + nIndex, pElements + nIndex + nCount,
      (size_t)nMoveCount * sizeof(TYPE));
   nSize -= nCount;
}

template<class TYPE>
extern inline void FreeArrayExtra(TYPE **pElements, UINT nSize, UINT &nMaxSize)
{
   if (nSize != nMaxSize)
   {
      // shrink to desired size
      TYPE* pNewData = NULL;
      if (nSize != 0)
      {
         pNewData = (TYPE*) new BYTE[nSize * sizeof(TYPE)];
         // copy new data from old
         memcpy(pNewData, *pElements, nSize * sizeof(TYPE));
      }

      // get rid of old stuff (note: no destructors called)
      delete[] (BYTE*)*pElements;
      *pElements = pNewData;
      nMaxSize = nSize;
   }
}

////////////////////////////////////////////////////////////////////////////////////

