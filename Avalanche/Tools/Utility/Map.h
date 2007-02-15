#ifndef __Map_H
#define __Map_H

#include "list.h"

template <class T1,class T2> class Pair
{
public:
	Pair(){}
	~Pair(){};
	Pair(const T1 key, const T2 value)
	{
		m_key = key;
		m_value = value;
	}

	void Set(const T1 key, const T2 value)
	{
		m_key = key;
		m_value = value;
	}

	T1	&GetKey() { return m_key; }
	T2	&GetValue() { return m_value; }

protected:
	T1 m_key;
	T2 m_value;
};

template <class T1,class T2> class Map
{
public:
	Map();
	~Map();

	void Clear();

   POSITION Begin() { return m_list.GetHeadPosition(); }
   POSITION End() { return m_list.GetTailPosition(); }

   POSITION Next(POSITION pos);

   Pair<T1,T2> &Get(POSITION pos);

	POSITION Insert(Pair<T1,T2> value);
	POSITION FindKey(T1 key);

public:
	POSITION _FindKey(T1 key,const int nBegin,const int nEnd);

	CList< Pair<T1,T2> >	m_list;
};

template <class T1,class T2,class T3> class Triple
{
public:
	Triple(){}
	~Triple(){};
	Triple(const T1 key1, const T2 key2, const T3 value)
	{
		m_key1 = key1;
		m_key2 = key2;
		m_value = value;
	}

	void Set(const T1 key1, const T2 key2, const T3 value)
	{
		m_key1 = key1;
		m_key2 = key2;
		m_value = value;
	}

	T1	&GetKey1() { return m_key1; }
	T2	&GetKey2() { return m_key2; }
	T3	&GetValue() { return m_value; }

protected:
	T1 m_key1;
	T2 m_key2;
	T3 m_value;
};


template <class T1,class T2,class T3> class DoubleMap
{
public:
	DoubleMap();
	~DoubleMap();
	void Clear();

   int GetSize() { return m_list.GetSize(); }

	POSITION Begin() { return m_list.GetHeadPosition(); }
	POSITION End() { return m_list.GetTailPosition(); }	

   POSITION Next(POSITION pos);

   Triple<T1,T2,T3> &Get(POSITION pos);

	POSITION Insert(Triple<T1,T2,T3> value);
	POSITION FindKey(T1 key1,T2 key2);

protected:
	POSITION _FindKey(T1 key1,T2 key2,const int nBegin,const int nEnd);

	CList< Triple<T1,T2,T3> >	m_list;
};

#include "Map.hpp"

#endif