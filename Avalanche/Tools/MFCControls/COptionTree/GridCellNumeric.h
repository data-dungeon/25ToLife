
#ifndef GRID_CELL_NUMERIC_H
#define GRID_CELL_NUMERIC_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../GridControl/GridCell.h"

class CGridCellNumeric : public CGridCell  
{
	DECLARE_DYNCREATE(CGridCellNumeric)

public:
	CGridCellNumeric(void);
	virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
	virtual void EndEdit();

	void SetAllowNegativeValues(bool bAllowNegativeValues);
	void SetSignificantDigits(int significantDigits);

private:
	bool m_bAllowNegativeValues;
	int m_significantDigits;
};

#endif // GRID_CELL_NUMERIC_H
