#ifndef __HashArray_H
#define __HashArray_H

#include "IntHash.h"
#include "Hash.h"

template <class T> class HashArray : public CArray<T>
{
public:
   HashArray();
   HashArray(const HashArray &other);
   ~HashArray();

   void RemoveAll();

   UINT Add(const T &value);

   CArray<UINT> *GetIndices() { return &m_indicies; }

   bool Find(const T &value,UINT &nIndex);

   HashArray<T> &operator = ( const HashArray<T> &other );

public:
   IntHash m_hashmap;
   CArray<UINT> m_indicies;
};

#include "HArray.hpp"

#endif