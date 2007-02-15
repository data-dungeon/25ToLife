////////////////////////////////////////////////////////////////////////////
//
// compareator
//
// A class for doing quick compares
//
////////////////////////////////////////////////////////////////////////////

template<class T>
inline TupCompareator<T>::TupCompareator(void)
{
}

template<class T>
inline TupCompareator<T>::TupCompareator(const TupArray<T> &array) : m_array(array)
{
}

template<class T>
inline bool TupCompareator<T>::operator==(const TupCompareator<T>& other) const
{
	int size1 = m_array.GetSize();
	int size2 = other.m_array.GetSize();
	int numElements = size1<size2 ? size1 : size2;
	for (int elementIndex=0;elementIndex<numElements;elementIndex++)
	{
		if (m_array[elementIndex]!=other.m_array[elementIndex])
		{
			return false;
		}
	}
	return true;
}

template<class T>
inline bool TupCompareator<T>::operator<=(const TupCompareator<T>& other) const
{
	int size1 = m_array.GetSize();
	int size2 = other.m_array.GetSize();
	int numElements = size1<size2 ? size1 : size2;
	for (int elementIndex=0;elementIndex<numElements;elementIndex++)
	{
		if (m_array[elementIndex]>other.m_array[elementIndex])
		{
			return false;
		}
		else if (m_array[elementIndex]<other.m_array[elementIndex])
		{
			return true;
		}
	}
	return true;
}

template<class T>
inline bool TupCompareator<T>::operator>=(const TupCompareator<T>& other) const
{
	int size1 = m_array.GetSize();
	int size2 = other.m_array.GetSize();
	int numElements = size1<size2 ? size1 : size2;
	for (int elementIndex=0;elementIndex<numElements;elementIndex++)
	{
		if (m_array[elementIndex]<other.m_array[elementIndex])
		{
			return false;
		}
		else if (m_array[elementIndex]>other.m_array[elementIndex])
		{
			return true;
		}
	}
	return true;
}

template<class T>
inline bool TupCompareator<T>::operator<(const TupCompareator<T>& other) const
{
	int size1 = m_array.GetSize();
	int size2 = other.m_array.GetSize();
	int numElements = size1<size2 ? size1 : size2;
	for (int elementIndex=0;elementIndex<numElements;elementIndex++)
	{
		if (m_array[elementIndex]>other.m_array[elementIndex])
		{
			return false;
		}
		else if (m_array[elementIndex]<other.m_array[elementIndex])
		{
			return true;
		}
	}
	return false;
}

template<class T>
inline bool TupCompareator<T>::operator>(const TupCompareator<T>& other) const
{
	int size1 = m_array.GetSize();
	int size2 = other.m_array.GetSize();
	int numElements = size1<size2 ? size1 : size2;
	for (int elementIndex=0;elementIndex<numElements;elementIndex++)
	{
		if (m_array[elementIndex]<other.m_array[elementIndex])
		{
			return false;
		}
		else if (m_array[elementIndex]>other.m_array[elementIndex])
		{
			return true;
		}
	}
	return false;
}
