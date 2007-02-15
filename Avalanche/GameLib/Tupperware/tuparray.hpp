////////////////////////////////////////////////////////////////////////////
//
// TupArray
//
// A dynamic array class
//
////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// TupArray<TYPE> inline functions

template<class TYPE>
inline int TupArray<TYPE>::GetSize() const
{
	return m_nSize;
}

template<class TYPE>
inline int TupArray<TYPE>::GetUpperBound() const
{
	return m_nSize-1;
}

template<class TYPE>
inline void TupArray<TYPE>::RemoveAll()
{
	SetSize(0, -1); 
}

template<class TYPE>
inline const TYPE& TupArray<TYPE>::GetAt(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex];
}

template<class TYPE>
inline TYPE& TupArray<TYPE>::GetAt(int nIndex)
{
	assert(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex];
}

template<class TYPE>
inline void TupArray<TYPE>::SetAt(int nIndex,const TYPE & newElement)
{
	assert(nIndex >= 0 && nIndex < m_nSize);
	m_pData[nIndex] = newElement; 
}

template<class TYPE>
inline const TYPE* TupArray<TYPE>::GetData() const
{
	return (const TYPE*)m_pData;
}

template<class TYPE>
inline TYPE* TupArray<TYPE>::GetData()
{
	return (TYPE*)m_pData;
}

template<class TYPE>
inline int TupArray<TYPE>::Add(const TYPE & newElement)
{
	int nIndex = m_nSize;
	SetAtGrow(nIndex, newElement);
	return nIndex; 
}

template<class TYPE>
inline const TYPE& TupArray<TYPE>::operator[](int nIndex) const
{
	return GetAt(nIndex);
}

template<class TYPE>
inline TYPE& TupArray<TYPE>::operator[](int nIndex)
{
	return GetAt(nIndex); 
}

/////////////////////////////////////////////////////////////////////////////
// TupArray<TYPE> out-of-line functions

template<class TYPE>
TupArray<TYPE>::TupArray()
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

// copy constructor
template<class TYPE>
TupArray<TYPE>::TupArray(const TupArray<TYPE>& arraySrc)
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
	Copy(arraySrc);
}

// assignment operator
template<class TYPE>
const TupArray<TYPE>& TupArray<TYPE>::operator=(const TupArray<TYPE>& arraySrc)
{
   // cannot append to itself
	if (this != &arraySrc)
	{
		Copy(arraySrc);
	}
	return (*this);
}

template<class TYPE>
TupArray<TYPE>::~TupArray()
{
	if (m_pData != NULL)
	{
		TupDestructElements<TYPE>(m_pData, m_nSize);
#ifdef USE_TUP_MEMORY_MANAGER
		FreeBuffer(m_pData);
#else
		delete[] (unsigned char*)m_pData;
#endif
	}
}

template<class TYPE>
void TupArray<TYPE>::SetSize(int nNewSize, int nGrowBy)
{
	assert(nNewSize >= 0);

	if (nGrowBy != -1)
		m_nGrowBy = nGrowBy;  // set new size

	if (nNewSize == 0)
	{
		// shrink to nothing
		if (m_pData != NULL)
		{
			TupDestructElements<TYPE>(m_pData, m_nSize);
#ifdef USE_TUP_MEMORY_MANAGER
			FreeBuffer(m_pData);
#else
			delete[] (unsigned char*)m_pData;
#endif
			m_pData = NULL;
		}
		m_nSize = m_nMaxSize = 0;
	}
	else if (m_pData == NULL)
	{
		// create one with exact size
#ifdef USE_TUP_MEMORY_MANAGER
		m_pData = (TYPE*) Allocate(nNewSize * sizeof(TYPE));
#else
		m_pData = (TYPE*) new unsigned char[nNewSize * sizeof(TYPE)];
#endif
		TupConstructElements<TYPE>(m_pData, nNewSize);
		m_nSize = m_nMaxSize = nNewSize;
	}
	else if (nNewSize <= m_nMaxSize)
	{
		// it fits
		if (nNewSize > m_nSize)
		{
			// initialize the new elements
			TupConstructElements<TYPE>(&m_pData[m_nSize], nNewSize-m_nSize);
		}
		else if (m_nSize > nNewSize)
		{
			// destroy the old elements
			TupDestructElements<TYPE>(&m_pData[nNewSize], m_nSize-nNewSize);
		}
		m_nSize = nNewSize;
	}
	else
	{
		// otherwise, grow array
		int nGrowBy = m_nGrowBy;
		if (nGrowBy == 0)
		{
			// heuristically determine growth when nGrowBy == 0
			//  (this avoids heap fragmentation in many situations)
			nGrowBy = m_nSize / 8;
			nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
		}
		int nNewMax;
		if (nNewSize < m_nMaxSize + nGrowBy)
			nNewMax = m_nMaxSize + nGrowBy;  // granularity
		else
			nNewMax = nNewSize;  // no slush

		assert(nNewMax >= m_nMaxSize);  // no wrap around

#ifdef USE_TUP_MEMORY_MANAGER
		TYPE *pNewData = (TYPE*) Allocate(nNewMax * sizeof(TYPE));
#else
		TYPE* pNewData = (TYPE*) new unsigned char[nNewMax * sizeof(TYPE)];
#endif

		// copy new data from old
		memcpy(pNewData, m_pData, m_nSize * sizeof(TYPE));

		// construct remaining elements
		assert(nNewSize > m_nSize);
		TupConstructElements<TYPE>(&pNewData[m_nSize], nNewSize-m_nSize);

		// get rid of old stuff (note: no destructors called)
#ifdef USE_TUP_MEMORY_MANAGER
		FreeBuffer(m_pData);
#else
		delete[] (unsigned char*)m_pData;
#endif
		m_pData = pNewData;
		m_nSize = nNewSize;
		m_nMaxSize = nNewMax;
	}
}

template<class TYPE>
int TupArray<TYPE>::Append(const TupArray& src)
{
	assert(this != &src);   // cannot append to itself

	int nOldSize = m_nSize;
	SetSize(m_nSize + src.m_nSize);
	TupCopyElements<TYPE>(m_pData + nOldSize, src.m_pData, src.m_nSize);
	return nOldSize;
}

template<class TYPE>
void TupArray<TYPE>::Copy(const TupArray& src)
{
	assert(this != &src);   // cannot append to itself

	SetSize(src.m_nSize);
	TupCopyElements<TYPE>(m_pData, src.m_pData, src.m_nSize);
}

template<class TYPE>
void TupArray<TYPE>::FreeExtra()
{
	if (m_nSize != m_nMaxSize)
	{
		// shrink to desired size
		TYPE* pNewData = NULL;
		if (m_nSize != 0)
		{
#ifdef USE_TUP_MEMORY_MANAGER
			pNewData = (TYPE*) Allocate(m_nSize * sizeof(TYPE));
#else
			pNewData = (TYPE*) new unsigned char[m_nSize * sizeof(TYPE)];
#endif
			// copy new data from old
			memcpy(pNewData, m_pData, m_nSize * sizeof(TYPE));
		}

		// get rid of old stuff (note: no destructors called)
#ifdef USE_TUP_MEMORY_MANAGER
		FreeBuffer(m_pData);
#else
		delete[] (unsigned char*)m_pData;
#endif
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

template<class TYPE>
void TupArray<TYPE>::SetAtGrow(int nIndex,const TYPE & newElement)
{
	assert(nIndex >= 0);

	if (nIndex >= m_nSize)
		SetSize(nIndex+1, -1);
	m_pData[nIndex] = newElement;
}

template<class TYPE>
void TupArray<TYPE>::InsertAt(int nIndex,const TYPE & newElement, int nCount /*=1*/)
{
	assert(nIndex >= 0);    // will expand to meet need
	assert(nCount > 0);     // zero or negative size not allowed

	if (nIndex >= m_nSize)
	{
		// adding after the end of the array
		SetSize(nIndex + nCount, -1);   // grow so nIndex is valid
	}
	else
	{
		// inserting in the middle of the array
		int nOldSize = m_nSize;
		SetSize(m_nSize + nCount, -1);  // grow it to new size
		// destroy intial data before copying over it
		TupDestructElements<TYPE>(&m_pData[nOldSize], nCount);
		// shift old data up to fill gap
		memmove(&m_pData[nIndex+nCount], &m_pData[nIndex],
			(nOldSize-nIndex) * sizeof(TYPE));

		// re-init slots we copied from
		TupConstructElements<TYPE>(&m_pData[nIndex], nCount);
	}

	// insert new value in the gap
	assert(nIndex + nCount <= m_nSize);
	while (nCount--)
		m_pData[nIndex++] = newElement;
}

template<class TYPE>
void TupArray<TYPE>::RemoveAt(int nIndex, int nCount)
{
	assert(nIndex >= 0);
	assert(nCount >= 0);
	assert(nIndex + nCount <= m_nSize);

	// just remove a range
	int nMoveCount = m_nSize - (nIndex + nCount);
	TupDestructElements<TYPE>(&m_pData[nIndex], nCount);
	if (nMoveCount)
		memmove(&m_pData[nIndex], &m_pData[nIndex + nCount],
			nMoveCount * sizeof(TYPE));
	m_nSize -= nCount;
}

template<class TYPE>
void TupArray<TYPE>::InsertAt(int nStartIndex,const TupArray* pNewArray)
{
	assert(pNewArray != NULL);
	assert(nStartIndex >= 0);

	if (pNewArray->GetSize() > 0)
	{
		InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
		for (int i = 0; i < pNewArray->GetSize(); i++)
			SetAt(nStartIndex + i, pNewArray->GetAt(i));
	}
}

template<class TYPE>
inline bool operator==(const TupArray<TYPE>& s1, const TupArray<TYPE>& s2)
{
	if (s1.GetSize()!=s2.GetSize())
		return false;
	for (int i=0;i<s1.GetSize();i++)
	{
		if (s1[i] != s2[i]) return false;
	}
	return true;
}

#ifdef USE_TUP_MEMORY_MANAGER
template<class TYPE>
void *TupArray<TYPE>::operator new(size_t nSize)
{
	return TupMemoryManager::Allocate(nSize); // call this one if running in the game engine
}

template<class TYPE>
void TupArray<TYPE>::operator delete(void *pBuffer)
{
	TupMemoryManager::FreeBuffer(pBuffer);
}

template<class TYPE>
void *TupArray<TYPE>::Allocate(size_t nSize)
{
	return TupMemoryManager::Allocate(nSize); // call this one if running in the game engine
}

template<class TYPE>
void TupArray<TYPE>::FreeBuffer(void *pBuffer)
{
	TupMemoryManager::FreeBuffer(pBuffer);
}
#endif
