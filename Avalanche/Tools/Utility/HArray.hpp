template<class T> inline HashArray<T>::HashArray()
{
}

template<class T> inline HashArray<T>::HashArray(const HashArray &other)
{
   *this = other;
}

template<class T> inline HashArray<T>::~HashArray()
{
}

template<class T> inline void HashArray<T>::RemoveAll()
{
   CArray<T>::RemoveAll();
   m_hashmap.Clear();
   m_indicies.RemoveAll();
}

//	allocates new reference and new element
template<class T> inline UINT HashArray<T>::Add(const T &value)
{	
   int index;
   int hashvalue = GetHashKey(value);
   bool haskey = m_hashmap.FindKey(hashvalue,index);
   if ((!haskey)||((haskey)&&(value != (*this)[index])))	
   {
      CArray<T>::Add(value);
      index = CArray<T>::GetSize()-1;
      m_hashmap.AddKey(hashvalue,index);
   }
   m_indicies.Add(index);
   return index;
}

template<class T> inline bool HashArray<T>::Find(const T &value, UINT &nIndex)
{
   int hashvalue = GetHashKey(value);
   int result;
   bool haskey = m_hashmap.FindKey(hashvalue,result);
   nIndex = result;
   if (!haskey)
      return false;
   return true;
}

template<class T> inline HashArray<T> &HashArray<T>::operator = ( const HashArray<T> &other )
{
   int i;
   for (i = 0; i < other.m_size; i++)
      Add(other[i]);
   return *this;	
}