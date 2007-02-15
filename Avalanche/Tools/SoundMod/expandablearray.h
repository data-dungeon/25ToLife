//============================================================================
//==
//== Project: soundmod
//== File:    expandablearray.h
//== Desc:    Builds a composite sound bank file from wav/aiff files and
//==          control modules files.
//== Author:  Rob Nelson
//== Date:    2/6/2003
//== (c)      2003 Avalanche Software, Inc.
//==
//== This file defines a generic expandable array template class
//==
//============================================================================

#ifndef _EXPANDABLEARRAY_H
#define _EXPANDABLEARRAY_H

template < class T > class ExpandableArray
{
private:
	int m_Count;
	int m_MaxSize;
	T*  m_Data;

public:
	ExpandableArray()
	{
		m_Count   = 0;
		m_MaxSize = 0;
		m_Data    = NULL;
	}

	~ExpandableArray()
	{
	}

	T& operator [] ( int index )
	{
		if ( index >= 0 && index < m_Count )
			return *( m_Data + index );
		else
			throw( "ERROR: array violation\n" );
	}

	int Count() const { return m_Count; }

	void Add( const T& data )
	{
		//-- expand
		if ( m_Count + 1 >= m_MaxSize )
		{
			m_MaxSize = ( m_MaxSize + 2 ) * 7 / 4;	// arbitrary grow ratio...
			m_Data = (T*) realloc( (void*) m_Data, m_MaxSize * sizeof(T) );
		}

		//-- insert new data
		memcpy( m_Data + m_Count, &data, sizeof(T) );
		m_Count++;
	}

	static int CompareAscending( const void* ptr1, const void* ptr2 )
	{
		T* a = (T*) ptr1;
		T* b = (T*) ptr2;
		return ( *a < *b ) ? -1 : ( *a > *b ) ? 1 : 0;
	}

	void Sort()
	{
		if ( m_Count > 1 )
			qsort( m_Data, m_Count, sizeof(T), CompareAscending );
	}
};

#endif //_EXPANDABLEARRAY_H
