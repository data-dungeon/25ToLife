// GridCellNumeric.cpp: implementation of the CGridCellNumeric class.
//
// Written by Andrew Truckle [ajtruckle@wsatkins.co.uk]
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GridCellNumeric.h"
#include "InPlaceEditNumeric.h"

IMPLEMENT_DYNCREATE(CGridCellNumeric, CGridCell)

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGridCellNumeric::CGridCellNumeric()
{
	m_significantDigits = 6;
	m_bAllowNegativeValues = true;
}

// Create a control to do the editing
BOOL CGridCellNumeric::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
{
	m_bEditing = TRUE;

	// CInPlaceEdit auto-deletes itself
	m_pEditWnd = new CInPlaceEditNumeric(GetGrid(), rect, 0, nID, nRow, nCol,
		GetText(), nChar,
		m_significantDigits,m_bAllowNegativeValues);

	return TRUE;
}

// Cancel the editing.
void CGridCellNumeric::EndEdit()
{
	if (m_pEditWnd)
		((CInPlaceEditNumeric*)m_pEditWnd)->EndEdit();
}

void CGridCellNumeric::SetAllowNegativeValues(bool bAllowNegativeValues)
{
	m_bAllowNegativeValues = bAllowNegativeValues;
}

void CGridCellNumeric::SetSignificantDigits(int significantDigits)
{
	m_significantDigits = significantDigits;
}
