//////////////////////////////////////////////////////////////////////////////////////////
//
// UINT DynamicMatrix Class
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DynamicMatrix_H
#define __DynamicMatrix_H

class DynamicMatrix
{
public:	
	DynamicMatrix();
	DynamicMatrix(UINT rows,UINT cols);
	DynamicMatrix(UINT rows,UINT cols,UINT defaultvalue);

	DynamicMatrix(const DynamicMatrix &other);

	~DynamicMatrix();

	UINT			IsEmpty() { if (!m_pMatrix) return 1; else return 0; }

//////////////////////////////////////////////////////////////////////////////////////////
//	Int Methods
//////////////////////////////////////////////////////////////////////////////////////////

	void		Clear();

	void		Initialize(UINT rows,UINT cols);
	void		Initialize(UINT rows,UINT cols,UINT defaultvalue);

	void		Set(UINT index,UINT value);
	void		Get(UINT index,UINT &value);

	void		Set(UINT row, UINT col, UINT value);
	void		Get(UINT row, UINT col,UINT &value);

	void		Fill(UINT value);
	void		FillFirstColumn(UINT value);
	void		FillLastColumn(UINT value);
	void		FillFirstRow(UINT value);
	void		FillLastRow(UINT value);

	void		FillRow(UINT row,CArray<UINT> &list,bool reverse = false);
	void		FillColumn(UINT col,CArray<UINT> &list,bool reverse = false);

	void		FillRow(UINT row,UINT value);
	void		FillColumn(UINT col,UINT value);

	void		GetColumn(UINT col,CArray<UINT> &list);
	void		GetRow(UINT row,CArray<UINT> &list);

	void		GetFirstColumn(CArray<UINT> &list);
   void		GetFirstColumnReverse(CArray<UINT> &list);
	void		GetFirstRow(CArray<UINT> &list);
   void		GetFirstRowReverse(CArray<UINT> &list);
	void		GetLastColumn(CArray<UINT> &list);
   void		GetLastColumnReverse(CArray<UINT> &list);
	void		GetLastRow(CArray<UINT> &list);
   void		GetLastRowReverse(CArray<UINT> &list);

	void		FillFirstColumn(const CArray<UINT> &list);
   void     FillFirstColumnReverse(const CArray<UINT> &list);
	void		FillFirstRow(const CArray<UINT> &list);
   void		FillFirstRowReverse(const CArray<UINT> &list);
	void		FillLastColumn(const CArray<UINT> &list);
   void		FillLastColumnReverse(const CArray<UINT> &list);
	void		FillLastRow(const CArray<UINT> &list);
   void		FillLastRowReverse(const CArray<UINT> &list);

	void		AddToFirstColumn(CArray<UINT> &list,bool reverse = false,bool subtract = false);
	void		AddToFirstRow(CArray<UINT> &list,bool reverse = false,bool subtract = false);
	void		AddToLastColumn(CArray<UINT> &list,bool reverse = false,bool subtract = false);
	void		AddToLastRow(CArray<UINT> &list,bool reverse = false,bool subtract = false);

	UINT		GetSize() { return m_nMaxCols * m_nMaxRows; }

   UINT     GetNumCols() { return m_nMaxCols; }
   UINT     GetNumRows() { return m_nMaxRows; }

	DynamicMatrix		&operator = ( const DynamicMatrix &other );

   UINT      m_nMaxCols,m_nMaxRows;
   void     *m_pMatrix;
};


#include "DynamicMatrix.hpp"

#endif
