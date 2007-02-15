/////////////////////////////////////////////////////////////////////////////////
// MAP CLASS MUST HAVE OVERLOADED > SIGN AND = SIGN
// Single Key Map Class
/////////////////////////////////////////////////////////////////////////////////

template<class T1,class T2> inline Map<T1,T2>::Map()
{
}

template<class T1,class T2> inline Map<T1,T2>::~Map()
{
}

template<class T1,class T2> inline POSITION Map<T1,T2>::Next(POSITION pos)
{
   m_list.GetNext(pos);
   return pos;
}

template<class T1,class T2> inline Pair<T1,T2> &Map<T1,T2>::Get(POSITION pos)
{
   return m_list.GetAt(pos);
}

template<class T1,class T2> inline POSITION Map<T1,T2>::Insert(Pair<T1,T2> value)
{
   POSITION insertat = m_list.GetHeadPosition();

   if (m_list.GetSize()>0)
      insertat = _FindKey(value.GetKey(),0,m_list.GetSize()-1);
      
	if (insertat)
	{
      Pair<T1,T2> pair = m_list.GetAt(insertat);

      if (value.GetKey() >= pair.GetKey())
         insertat = m_list.InsertAfter(insertat,value);
		else
         insertat = m_list.InsertBefore(insertat,value);
	}
	else
	{
      insertat = m_list.AddTail(value);
	}

	return insertat;
}

// finds key equal to
template<class T1,class T2> inline POSITION Map<T1,T2>::FindKey(T1 key)
{
	POSITION pos = _FindKey(key,0,m_list.GetSize()-1);
   Pair<T1,T2> pair = m_list.GetAt(pos);

   if (pair.GetKey() == key)
      return pos;
   else
      return NULL;
}

// finds key or one just closest but less
template<class T1,class T2> inline POSITION Map<T1,T2>::_FindKey(T1 key,const int nBegin,const int nEnd)
{
   POSITION middle;
   
	UINT nMiddleIndex = ((nEnd-nBegin)*0.5f) + nBegin;

   middle = m_list.FindIndex(nMiddleIndex);

   if (nEnd-nBegin==0)
      return middle;

   Pair<T1,T2> pair = m_list.GetAt(middle);

   if (key == pair.GetKey())
      return middle;

   // if not equal and sampling area is 1 return next position
   POSITION nextpos = middle;
	if (nEnd-nBegin == 1)
	{
      Pair<T1,T2> pair = m_list.GetNext(nextpos);
      if (nextpos)
      {
          if (key >= pair.GetKey())
             return nextpos;
      }
      return middle;
   }

   if (key > pair.GetKey())
      return _FindKey(key,nMiddleIndex,nEnd);
   if (key < pair.GetKey())
      return _FindKey(key,nBegin,nMiddleIndex);

	return NULL;
}

template<class T1,class T2> inline void Map<T1,T2>::Clear()
{
	m_list.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////////
// Double Key Map Class
/////////////////////////////////////////////////////////////////////////////////

template<class T1,class T2,class T3> inline DoubleMap<T1,T2,T3>::DoubleMap()
{
}

template<class T1,class T2,class T3> inline DoubleMap<T1,T2,T3>::~DoubleMap()
{
}

template<class T1,class T2,class T3> inline POSITION DoubleMap<T1,T2,T3>::Next(POSITION pos)
{
   m_list.GetNext(pos);
   return pos;
}

template<class T1,class T2,class T3> inline Triple<T1,T2,T3> &DoubleMap<T1,T2,T3>::Get(POSITION pos)
{
   return m_list.GetAt(pos);
}

template<class T1,class T2,class T3> inline void DoubleMap<T1,T2,T3>::Clear()
{
   m_list.Clear();
}

template<class T1,class T2,class T3> inline POSITION DoubleMap<T1,T2,T3>::Insert(Triple<T1,T2,T3> value)
{
   POSITION insertat = m_list.GetHeadPosition();

   if (m_list.GetSize()>0)
      insertat = _FindKey(value.GetKey1(),value.GetKey2(),0,m_list.GetSize()-1);

   if (insertat)
   {
      Triple<T1,T2,T3> triple = m_list.GetAt(insertat);

      if (value.GetKey1() > triple.GetKey1())
      {
         insertat = m_list.InsertAfter(insertat,value);
      }
      else if (value.GetKey1() == triple.GetKey1())
      {
         if (value.GetKey2() >= triple.GetKey2())
            insertat = m_list.InsertAfter(insertat,value);
         else
            insertat = m_list.InsertBefore(insertat,value);
      }
      else
      {
         insertat = m_list.InsertBefore(insertat,value);
      }
   }
   else
   {
      insertat = m_list.AddTail(value);
   }

   return insertat;
}

// finds key equal to
template<class T1,class T2,class T3> inline POSITION DoubleMap<T1,T2,T3>::FindKey(T1 key1,T2 key2)
{
   POSITION pos = _FindKey(key1,key2,0,m_list.GetSize()-1);
   Triple<T1,T2,T3> triple = m_list.GetAt(pos);

   if ((triple.GetKey1() == key1)&&(triple.GetKey2()))
      return pos;
   else
      return NULL;
}

// finds key or one just closest but less
template<class T1,class T2,class T3> inline POSITION DoubleMap<T1,T2,T3>::_FindKey(T1 key1,T2 key2,const int nBegin,const int nEnd)
{
   POSITION middle;

   UINT nMiddleIndex = ((nEnd-nBegin)*0.5f) + nBegin;

   middle = m_list.FindIndex(nMiddleIndex);

   if (nEnd-nBegin==0)
      return middle;

   Triple<T1,T2,T3> triple = m_list.GetAt(middle);

   if ((key1 == triple.GetKey1())&&(key2 == triple.GetKey2()))
      return middle;

   // if not equal and sampling area is 1 return next position
   POSITION nextpos = middle;
   if (nEnd-nBegin == 1)
   {
      Triple<T1,T2,T3> triple = m_list.GetNext(nextpos);

      if (nextpos)
      {
         if (key1 > triple.GetKey1())
         {
            return nextpos;
         }
         else if (key1 == triple.GetKey1())
         {
            if (key2 >= triple.GetKey2())
               return nextpos;
         }
      }
      return middle;
   }

   if (key1 > triple.GetKey1())
      return _FindKey(key1,key2,nMiddleIndex,nEnd);
   if (key1 < triple.GetKey1())
      return _FindKey(key1,key2,nBegin,nMiddleIndex);

   if (key2 > triple.GetKey2())
      return _FindKey(key1,key2,nMiddleIndex,nEnd);
   if (key2 < triple.GetKey2())
      return _FindKey(key1,key2,nBegin,nMiddleIndex);

   return NULL;
}
