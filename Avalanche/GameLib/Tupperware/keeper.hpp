////////////////////////////////////////////////////////////////////////////
//
// Keeper
//
// classes to handle keeping track of unique arrays of information
//
////////////////////////////////////////////////////////////////////////////

// KeeperHashEntry Methods
inline KeeperHashEntry::KeeperHashEntry(void) : 
	m_index(0), m_hashValue(0), m_pNext(0)
{
}

inline KeeperHashEntry::KeeperHashEntry(int index,unsigned int hashValue,KeeperHashEntry *pNext) : 
	m_index(index), m_hashValue(hashValue), m_pNext(pNext)
{
}

inline KeeperHashEntry::KeeperHashEntry(const KeeperHashEntry& src) :
	m_index(src.m_index), m_hashValue(src.m_hashValue)
{
	if (src.m_pNext)
	{
		m_pNext = new KeeperHashEntry(*src.m_pNext);
	}
	else
	{
		m_pNext = NULL;
	}
}

inline const KeeperHashEntry& KeeperHashEntry::operator=(const KeeperHashEntry& src)
{
	m_index = src.m_index;
	m_hashValue = src.m_hashValue;
	delete m_pNext;

	if (src.m_pNext)
	{
		m_pNext = new KeeperHashEntry(*src.m_pNext);
	}
	else
	{
		m_pNext = NULL;
	}
}

inline KeeperHashEntry::~KeeperHashEntry()
{
	delete m_pNext; // delete my siblings
}

inline int KeeperHashEntry::GetIndex(void) const
{
	return m_index;
}

inline unsigned int KeeperHashEntry::GetHash(void) const
{
	return m_hashValue;
}

inline KeeperHashEntry *KeeperHashEntry::GetNextPtr(void) const
{
	return m_pNext;
}

inline void KeeperHashEntry::SetIndex(int index)
{
	m_index = index;
}

inline void KeeperHashEntry::SetHash(unsigned int hashValue)
{
	m_hashValue = hashValue;
}

inline void KeeperHashEntry::SetNextPtr(KeeperHashEntry *pNext)
{
	m_pNext = pNext;
}


// Keeper
template <typename T>
inline Keeper<T>::Keeper(int hashTableSize)
{
	m_hashArray.SetSize(hashTableSize);
	for (int i=0;i<hashTableSize;i++)
	{
		m_hashArray[i] = 0; // clear pointers in array
	}
	SetPowerOf2();
}

// copy constructor
template <typename T>
inline Keeper<T>::Keeper<T>(const Keeper<T>& src) :
	m_vArray(src.m_vArray),
	m_powerOf2(src.m_powerOf2)
{
	int size = src.m_hashArray.GetSize();
	m_hashArray.SetSize(size);
	for (int index=0;index<size;index++)
	{
		if (src.m_hashArray[index])
		{
			m_hashArray[index] = new KeeperHashEntry(*src.m_hashArray[index]);
		}
		else
		{
			m_hashArray[index] = 0;
		}
	}
}
	
// assignment operator
template <typename T>
inline const Keeper<T>& Keeper<T>::operator=(const Keeper<T>& src)
{
	m_vArray = src.m_vArray;
	m_powerOf2 = src.m_powerOf2;
	int size = m_hashArray.GetSize();
	int index;
	for (index=0;index<size;index++)
	{
		delete m_hashArray[index]; // clear hash entries
	}
	size = src.m_hashArray.GetSize();
	m_hashArray.SetSize(size);
	for (index=0;index<size;index++)
	{
		if (src.m_hashArray[index])
		{
			m_hashArray[index] = new KeeperHashEntry(*src.m_hashArray[index]);
		}
		else
		{
			m_hashArray[index] = 0;
		}
	}
	return *this;
}

template <typename T>
inline const T& Keeper<T>::operator[](int nIndex) const
{
	return m_vArray[nIndex];
}

template <typename T>
inline T& Keeper<T>::operator[](int nIndex)
{
	return m_vArray[nIndex];
}

template <typename T>
inline Keeper<T>::~Keeper()
{
	int hashTableSize = m_hashArray.GetSize();
	for (int i=0;i<hashTableSize;i++)
	{
		delete m_hashArray[i]; // clear hash entries
	}
}

template <typename T>
inline int Keeper<T>::Add(const T& entry)
{
	unsigned int hashValue = GetHashValue(entry);
	unsigned int tableIndex = m_powerOf2 ? 
		(hashValue & (m_hashArray.GetSize()-1)) :
		(hashValue % (m_hashArray.GetSize()));
	KeeperHashEntry *pHashEntry = m_hashArray[tableIndex];
	while (pHashEntry)
	{
		if (pHashEntry->GetHash()==hashValue)
		{
			int arrayIndex = pHashEntry->GetIndex();
			if ((m_vArray[arrayIndex])==entry)
			{
				// found a match
				return arrayIndex;
			}
		}
		pHashEntry = pHashEntry->GetNextPtr();
	}
	// did not find a match
	m_vArray.Add(entry); // add new entry
	KeeperHashEntry *pNewHashEntry = new KeeperHashEntry(m_vArray.GetSize()-1,hashValue,m_hashArray[tableIndex]);
	m_hashArray[tableIndex] = pNewHashEntry;
	return m_vArray.GetSize()-1;
}

template <typename T>
inline void Keeper<T>::Add(const Keeper<T> &keeper)
{
	int numEntries = keeper.GetSize();
	for (int entryIndex = 0;entryIndex<numEntries;entryIndex++)
	{
		Add(keeper[entryIndex]);
	}
}

template <typename T>
inline void Keeper<T>::Add(const TupArray<T> &array)
{
	int numEntries = array.GetSize();
	for (int entryIndex = 0;entryIndex<numEntries;entryIndex++)
	{
		Add(array[entryIndex]);
	}
}

template <typename T>
inline void Keeper<T>::Add(const T *pArray,int numEntries)
{
	for (int entryIndex = 0;entryIndex<numEntries;entryIndex++)
	{
		Add(pArray[entryIndex]);
	}
}

template <typename T>
inline int Keeper<T>::GetIndex(const T& entry) const
{
	unsigned int hashValue = GetHashValue(entry);
	unsigned int tableIndex = m_powerOf2 ? 
		(hashValue & (m_hashArray.GetSize()-1)) :
		(hashValue % (m_hashArray.GetSize()));
	KeeperHashEntry *pHashEntry = m_hashArray[tableIndex];
	while (pHashEntry)
	{
		if (pHashEntry->GetHash()==hashValue)
		{
			int arrayIndex = pHashEntry->GetIndex();
			if (m_vArray[arrayIndex]==entry)
			{
				// found a match
				return arrayIndex;
			}
		}
		pHashEntry = pHashEntry->GetNextPtr();
	}
	return -1;
}

template <typename T>
inline const T *Keeper<T>::GetData(void) const
{
	if (m_vArray.GetSize())
	{
		return &m_vArray[0];
	}
	else
	{
		return NULL;
	}
}

template <typename T>
inline T *Keeper<T>::GetData(void)
{
	if (m_vArray.GetSize())
	{
		return &m_vArray[0];
	}
	else
	{
		return NULL;
	}
}

template <typename T>
inline const TupArray<T> &Keeper<T>::GetArray(void) const
{
	return m_vArray;
}

template <typename T>
inline TupArray<T> &Keeper<T>::GetArray(void)
{
	return m_vArray;
}

template <typename T>
inline void Keeper<T>::RemoveAll(void)
{
	m_vArray.RemoveAll();
	int size = m_hashArray.GetSize();
	for (int index=0;index<size;index++)
	{
		delete m_hashArray[index]; // delete old hash entries
		m_hashArray[index] = NULL;
	}
}


template <typename T>
inline int Keeper<T>::GetSize(void) const
{
	return m_vArray.GetSize();
}

template <typename T>
inline void Keeper<T>::SetPowerOf2(void)
{
	m_powerOf2 = false;

	int hashTableSize = m_hashArray.GetSize();
	int maxValue = 0x40000000;

	for (int value = 2;value<=maxValue;value = value<<1)
	{
		if (hashTableSize == value)
		{
			m_powerOf2 = true;
			return;
		}
		if (hashTableSize<value)
		{
			return;
		}
	}
	m_powerOf2 = false;
}

template <typename T>
inline void Keeper<T>::SetHashTableSize(int newSize)
{
	int size = m_hashArray.GetSize();
	int index;
	for (index=0;index<size;index++)
	{
		delete m_hashArray[index]; // delete old hash entries
	}
	m_hashArray.SetSize(newSize);
	for (index=0;index<newSize;index++)
	{
		m_hashArray[index] = 0; // clear the pointers
	}
	SetPowerOf2();

	// rebuild the hash array
	size = m_vArray.GetSize();
	for (index=0;index<size;index++)
	{
		unsigned int hashValue = GetHashValue(m_vArray[index]);
		unsigned int tableIndex = m_powerOf2 ? 
			(hashValue & (m_hashArray.GetSize()-1)) :
			(hashValue % (m_hashArray.GetSize()));
		KeeperHashEntry *pNewHashEntry = new KeeperHashEntry(index,hashValue,m_hashArray[tableIndex]);
		m_hashArray[tableIndex] = pNewHashEntry;
	}
}

template <typename T>
inline int Keeper<T>::GetHashTableSize(void) const
{
	return m_hashArray.GetSize();
}




