//////////////////////////////////////////////////////////////////////////////////////////
//
// DynamicMatrix Class
//
//////////////////////////////////////////////////////////////////////////////////////////

inline DynamicMatrix::DynamicMatrix()
{
	m_pMatrix = NULL;
	m_nMaxRows = 0;
	m_nMaxCols = 0;
}

inline DynamicMatrix::DynamicMatrix(UINT rows,UINT cols)
{
   if ((rows==0) && (cols==0))
      return;

	m_nMaxCols = cols;
	m_nMaxRows = rows;

	m_pMatrix = new UINT[rows*cols];
}

inline DynamicMatrix::DynamicMatrix(UINT rows,UINT cols,UINT defaultvalue)
{
   if ((rows==0) && (cols==0))
      return;

	m_nMaxRows = rows;
	m_nMaxCols = cols;

	m_pMatrix = new UINT[rows*cols];

	Fill(defaultvalue);
}

inline DynamicMatrix::DynamicMatrix(const DynamicMatrix &other)
{
	m_nMaxRows = other.m_nMaxRows;
	m_nMaxCols = other.m_nMaxCols;

	if (other.m_pMatrix)
	{
		m_pMatrix = new UINT[m_nMaxRows*m_nMaxCols];
		memcpy(m_pMatrix,other.m_pMatrix,sizeof(UINT)*(m_nMaxCols*m_nMaxRows));
	}
}

inline DynamicMatrix::~DynamicMatrix()
{
	Clear();
}

inline void DynamicMatrix::Clear()
{
	if (m_pMatrix)
		delete[] m_pMatrix;
	m_pMatrix = NULL;
	m_nMaxCols = 0;
	m_nMaxRows = 0;
}

inline void DynamicMatrix::Initialize(UINT rows,UINT cols)
{
   if ((m_pMatrix)&&(m_nMaxCols != cols) && (m_nMaxRows != rows))
      Clear();
   if ((rows==0) && (cols==0))
      return;
   if (!m_pMatrix)
      m_pMatrix = SNEW UINT[rows*cols];
 	m_nMaxCols = cols;
	m_nMaxRows = rows;
}


inline void DynamicMatrix::Initialize(UINT rows,UINT cols,UINT defaultvalue)
{
   if ((m_pMatrix)&&(m_nMaxCols != cols) && (m_nMaxRows != rows))
      Clear();
   if ((rows==0) && (cols==0))
      return;

   if (!m_pMatrix)
      m_pMatrix = new UINT[rows*cols];

	m_nMaxCols = cols;
	m_nMaxRows = rows;
	Fill(defaultvalue);
}

inline void DynamicMatrix::Set(UINT index,UINT value)
{
   ASSERT(index < (m_nMaxCols * m_nMaxRows));
	((UINT*)m_pMatrix)[index] = value;
}

inline void DynamicMatrix::Set(UINT row, UINT col,UINT value)
{
   ASSERT(((row * m_nMaxCols) + col) < (m_nMaxCols * m_nMaxRows));
	((UINT*)m_pMatrix)[(row * m_nMaxCols) + col] = value;
}

inline void DynamicMatrix::Get(UINT row, UINT col,UINT &value)
{
	value = ((UINT*)m_pMatrix)[(row * m_nMaxCols) + col];
}

inline void DynamicMatrix::Get(UINT index,UINT &value)
{
	value = ((UINT*)m_pMatrix)[index];
}

inline void DynamicMatrix::Fill(UINT value)
{
//   UINT i;
//	for (i = 0; i < GetSize(); i++)
//		((UINT*)m_pMatrix)[i] = value;
   memset((UINT*)m_pMatrix,(UINT)value,sizeof(UINT)*(m_nMaxCols*m_nMaxRows));
}

inline void DynamicMatrix::FillFirstColumn(UINT value)
{
   UINT i;
	for (i = 0; i < m_nMaxRows; i++)
		Set(i,0,value);
}

inline void DynamicMatrix::FillFirstRow(UINT value)
{
//   UINT i;
//	for (i = 0; i < m_nMaxCols; i++)
//		Set(0,i,value);
   memset(m_pMatrix,(UINT)value,sizeof(UINT)*m_nMaxCols);
}

inline void DynamicMatrix::FillLastColumn(UINT value)
{
   UINT i;
	for (i = 0; i < m_nMaxRows; i++)
		Set(i,m_nMaxCols-1,value);
}

inline void DynamicMatrix::FillLastRow(UINT value)
{
//   UINT i;
//	for (i = 0; i < m_nMaxCols; i++)
//		Set(m_nMaxRows-1,i,value);
   memset((void*)&((UINT*)m_pMatrix)[(m_nMaxRows-1) * m_nMaxCols],(UINT)value,sizeof(UINT)*m_nMaxCols);
}

inline void DynamicMatrix::FillColumn(UINT col,CArray<UINT> &list,bool reverse)
{
	ASSERT(list.GetSize()==m_nMaxRows);
	UINT lastelement = 0;
	if (reverse)
		lastelement = list.GetSize()-1;
   UINT i;
	for (i = 0; i < m_nMaxRows; i++)
 		Set(i,col,list[lastelement-i]);
}

inline void DynamicMatrix::FillRow(UINT row,CArray<UINT> &list,bool reverse)
{
	ASSERT(list.GetSize()==m_nMaxCols);
	UINT lastelement = 0;
	if (reverse)
		lastelement = list.GetSize()-1;
   UINT i;
	for (i = 0; i < m_nMaxCols; i++)
		Set(row,i,list[lastelement-i]);
}

inline void DynamicMatrix::FillColumn(UINT col,UINT value)
{
   UINT i;
	for (i = 0; i < m_nMaxRows; i++)
		Set(i,col,value);
}

inline void DynamicMatrix::FillRow(UINT row, UINT value)
{
   UINT i;
	for (i = 0; i < m_nMaxCols; i++)
		Set(row,i,value);
}

inline void DynamicMatrix::GetColumn(UINT col,CArray<UINT> &list)
{
	UINT value;
	list.RemoveAll();
	list.SetSize(m_nMaxRows,0);
   UINT i;
	for (i = 0; i < m_nMaxRows; i++)
	{
		Get(i,col,value);
		list[i] = value;
	}
}

inline void DynamicMatrix::GetRow(UINT row,CArray<UINT> &list)
{
	UINT value;
	list.RemoveAll();
	list.SetSize(m_nMaxCols,0);
   UINT i;
	for (i = 0; i < m_nMaxCols; i++)
	{
		Get(row,i,value);
		list[i] = value;
	}
}

inline void DynamicMatrix::GetFirstColumn(CArray<UINT> &list)
{
	UINT value;
	list.RemoveAll();
	list.SetSize(m_nMaxRows,0);
   UINT i;
	for (i = 0; i < m_nMaxRows; i++)
	{
		Get(i,0,value);
 		list[i] = value;
	}
}

inline void DynamicMatrix::GetFirstColumnReverse(CArray<UINT> &list)
{
   UINT value;
   list.RemoveAll();
   list.SetSize(m_nMaxRows,0);
   UINT lastelement = list.GetSize()-1;
   UINT i;
   for (i = 0; i < m_nMaxRows; i++)
   {
      Get(i,0,value);
      list[lastelement-i] = value;
   }
}

inline void DynamicMatrix::GetFirstRow(CArray<UINT> &list)
{
/*
	UINT value;
	list.RemoveAll();
	list.SetSize(m_nMaxCols,0);
   UINT i;
	for (i = 0; i < m_nMaxCols; i++)
	{
		Get(0,i,value);
		list[i] = value;
	}
*/
	list.SetSize(m_nMaxCols);
   memcpy((UINT*)list.GetData(),(UINT*)m_pMatrix,sizeof(UINT)*m_nMaxCols);
}

inline void DynamicMatrix::GetFirstRowReverse(CArray<UINT> &list)
{
   UINT value;
   list.RemoveAll();
   list.SetSize(m_nMaxCols,0);
   UINT lastelement = list.GetSize()-1;
   UINT i;
   for (i = 0; i < m_nMaxCols; i++)
   {
      Get(0,i,value);
      list[lastelement-i] = value;
   }
}

inline void DynamicMatrix::GetLastColumn(CArray<UINT> &list)
{
	UINT value;
	list.RemoveAll();
	list.SetSize(m_nMaxRows,0);
   UINT i;
	for (i = 0; i < m_nMaxRows; i++)
	{
		Get(i,m_nMaxCols-1,value);
		list[i] = value;
	}
}

inline void DynamicMatrix::GetLastColumnReverse(CArray<UINT> &list)
{
   UINT value;
   list.RemoveAll();
   list.SetSize(m_nMaxRows,0);
   UINT lastelement = list.GetSize()-1;
   UINT i;
   for (i = 0; i < m_nMaxRows; i++)
   {
      Get(i,m_nMaxCols-1,value);
      list[lastelement-i] = value;
   }
}

inline void DynamicMatrix::GetLastRow(CArray<UINT> &list)
{
/*
	UINT value;
	list.RemoveAll();
	list.SetSize(m_nMaxCols,0);
   UINT i;
	for (i = 0; i < m_nMaxCols; i++)
	{
		Get(m_nMaxRows-1,i,value);
		list[i] = value;
	}
*/
   list.SetSize(m_nMaxCols);
   memcpy((UINT*)list.GetData(),(void*)&((UINT*)m_pMatrix)[(m_nMaxRows-1) * m_nMaxCols],sizeof(UINT)*m_nMaxCols);
}

inline void DynamicMatrix::GetLastRowReverse(CArray<UINT> &list)
{
   UINT value;
   list.RemoveAll();
   list.SetSize(m_nMaxCols,0);
   UINT lastelement = list.GetSize()-1;
   UINT i;
   for (i = 0; i < m_nMaxCols; i++)
   {
      Get(m_nMaxRows-1,i,value);
      list[lastelement-i] = value;
   }
}

inline void DynamicMatrix::FillFirstColumn(const CArray<UINT> &list)
{
   ASSERT(list.GetSize()==m_nMaxRows);
   UINT i;
	for (i = 0; i < m_nMaxRows; i++)
 		Set(i,0,list[i]);
}

inline void DynamicMatrix::FillFirstColumnReverse(const CArray<UINT> &list)
{
   ASSERT(list.GetSize()==m_nMaxRows);
   UINT lastelement = list.GetSize()-1;
   UINT i;
   for (i = 0; i < m_nMaxRows; i++)
      Set(i,0,list[lastelement-i]);
}

inline void DynamicMatrix::FillFirstRow(const CArray<UINT> &list)
{
/*
   ASSERT(list.GetSize()==m_nMaxCols);
   UINT i;
	for (i = 0; i < m_nMaxCols; i++)
 		Set(0,i,list[i]);
*/
   memcpy((UINT*)m_pMatrix,(UINT*)list.GetData(),sizeof(UINT)*m_nMaxCols);
}

inline void DynamicMatrix::FillFirstRowReverse(const CArray<UINT> &list)
{
   ASSERT(list.GetSize()==m_nMaxCols);
   UINT lastelement = list.GetSize()-1;
   UINT i;
   for (i = 0; i < m_nMaxCols; i++)
      Set(0,i,list[lastelement-i]);
}

inline void DynamicMatrix::FillLastColumn(const CArray<UINT> &list)
{
   ASSERT(list.GetSize()==m_nMaxRows);
   UINT i;
	for (i = 0; i < m_nMaxRows; i++)
		Set(i,m_nMaxCols-1,list[i]);
}

inline void DynamicMatrix::FillLastColumnReverse(const CArray<UINT> &list)
{
   ASSERT(list.GetSize()==m_nMaxRows);
   UINT lastelement = list.GetSize()-1;
   UINT i;
   for (i = 0; i < m_nMaxRows; i++)
      Set(i,m_nMaxCols-1,list[lastelement-i]);
}

inline void DynamicMatrix::FillLastRow(const CArray<UINT> &list)
{
/*
   ASSERT(list.GetSize()==m_nMaxCols);
   UINT i;
	for (i = 0; i < m_nMaxCols; i++)
		Set(m_nMaxRows-1,i,list[i]);
*/
   memcpy((void*)&((UINT*)m_pMatrix)[(m_nMaxRows-1) * m_nMaxCols],(UINT*)list.GetData(),sizeof(UINT)*m_nMaxCols);
}

inline void DynamicMatrix::FillLastRowReverse(const CArray<UINT> &list)
{
   ASSERT(list.GetSize()==m_nMaxCols);
   UINT lastelement = list.GetSize()-1;
   UINT i;
   for (i = 0; i < m_nMaxCols; i++)
      Set(m_nMaxRows-1,i,list[lastelement-i]);
}

inline void DynamicMatrix::AddToFirstColumn(CArray<UINT> &list,bool reverse,bool subtract)
{
	UINT value;
	UINT lastelement = 0;
	if (reverse)
		lastelement = list.GetSize()-1;
   UINT i;
	for (i = 0; i < m_nMaxRows; i++)
	{
		Get(i,0,value);
		if (subtract)
			Set(i,0,value - list[lastelement-i]);
		else
			Set(i,0,value + list[lastelement-i]);
	}
}

inline void DynamicMatrix::AddToFirstRow(CArray<UINT> &list,bool reverse,bool subtract)
{
	UINT value;
	UINT lastelement = 0;
	if (reverse)
		lastelement = list.GetSize()-1;
   UINT i;
	for (i = 0; i < m_nMaxCols; i++)
	{
		Get(0,i,value);
 		if (subtract)
			Set(0,i,value - list[lastelement-i]);
		else
			Set(0,i,value + list[lastelement-i]);
	}
}

inline void DynamicMatrix::AddToLastColumn(CArray<UINT> &list,bool reverse,bool subtract)
{
	UINT value;
	UINT lastelement = 0;
	if (reverse)
		lastelement = list.GetSize()-1;
   UINT i;
	for (i = 0; i < m_nMaxRows; i++)
	{
		Get(i,m_nMaxCols-1,value);
 		if (subtract)
			Set(i,m_nMaxCols-1,value - list[lastelement-i]);
		else
			Set(i,m_nMaxCols-1,value + list[lastelement-i]);
	}
}

inline void DynamicMatrix::AddToLastRow(CArray<UINT> &list,bool reverse,bool subtract)
{
	UINT value;
	UINT lastelement = 0;
	if (reverse)
		lastelement = list.GetSize()-1;
   UINT i;
	for (i = 0; i < m_nMaxCols; i++)
	{
		Get(m_nMaxRows-1,i,value);
		if (subtract)
			Set(m_nMaxRows-1,i,value - list[lastelement-i]);
		else
			Set(m_nMaxRows-1,i,value + list[lastelement-i]);
	}
}

inline DynamicMatrix &DynamicMatrix::operator = ( const DynamicMatrix &other ){
	Clear();
	m_nMaxCols = other.m_nMaxCols;
	m_nMaxRows = other.m_nMaxRows;

	m_pMatrix = new UINT[m_nMaxRows*m_nMaxCols];

	memcpy(m_pMatrix,other.m_pMatrix,sizeof(UINT)*(m_nMaxCols*m_nMaxRows));

	return *this;
}
