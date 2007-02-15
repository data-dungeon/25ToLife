// COptionTree
//
// License
// -------
// This code is provided "as is" with no expressed or implied warranty.
// 
// You may use this code in a commercial product with or without acknowledgement.
// However you may not sell this code or any modification of this code, this includes 
// commercial libraries and anything else for profit.
//
// I would appreciate a notification of any bugs or bug fixes to help the control grow.
//
// History:
// --------
//	See License.txt for full history information.
//
//
// Copyright (c) 1999-2002 
// ComputerSmarts.net 
// mattrmiller@computersmarts.net
#include "stdafx.h"
#include "OptionTreeItemList.h"

// Added Headers
#include "OptionTree.h"
#include "OptionTreeListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionTreeItemList

COptionTreeItemList::COptionTreeItemList()
{
	// Initialize variables
	m_dwOptions = 0;	
	m_bAllowNegativeValues = true;
	m_significantDigits = 6;
	// Set item type
	SetItemType(OT_ITEM_LIST);
	SetItemHeight(OT_LIST_HEIGHT);
}

COptionTreeItemList::~COptionTreeItemList()
{
}

void COptionTreeItemList::DrawAttribute(CDC *pDC, const RECT &rcRect)
{
	// Declare variables
	COLORREF crOld;
	int nOldBack;
	CRect rcText, rcClient;
	CString strText = _T("");
	CString strTemp;
	HGDIOBJ hOld;
	CFileFind fFind;
	COLORREF crOldBack;

	// Make sure options aren't NULL
	if (m_otOption == NULL)
	{
		return;
	}

	// Get window rect
	rcClient = rcRect;

	// Select font
	hOld = pDC->SelectObject(m_otOption->GetNormalFont());

	// Set text color
	if (IsReadOnly() == TRUE || m_otOption->IsWindowEnabled() == FALSE)
	{
		crOld = pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
	}
	else
	{
		crOld = pDC->SetTextColor(GetTextColor());
	}

	// Set background mode
	nOldBack = pDC->SetBkMode(TRANSPARENT);

	// Set background color
	crOldBack = pDC->SetBkColor(GetBackgroundColor());	

	// Get text rectangle
	rcText.left  = rcRect.left + 1;
	rcText.right = rcRect.right;
	rcText.top = rcRect.top + 1;
	rcText.bottom = rcRect.bottom;

	// Draw text
	int numLines = m_values.GetSize();
	if (numLines>8)
		numLines=8;
	for (int line=0;line<numLines;line++)
	{
		strText = m_values[line];

		rcText.left  = rcRect.left + 1;
		rcText.right = rcRect.right;

		// Draw text
		pDC->DrawText(strText, rcText, DT_VCENTER);
		pDC->DrawText(strText, rcText, DT_VCENTER | DT_CALCRECT);

		rcText.OffsetRect(0,rcText.Height()+2);
	}


	// Restore GDI ojects
	pDC->SetTextColor(crOld);
	pDC->SetBkMode(nOldBack);
	pDC->SelectObject(hOld);
	pDC->SetBkColor(crOldBack);
}

void COptionTreeItemList::OnCommit()
{
}

void COptionTreeItemList::OnRefresh()
{
}

void COptionTreeItemList::OnMove()
{
}

void COptionTreeItemList::CleanDestroyWindow()
{
}

void COptionTreeItemList::SetNumItems(int numItems)
{
	m_values.SetSize(numItems);
}

void COptionTreeItemList::SetNumColumns(int numColumns)
{
	m_numColumns = numColumns;
}

void COptionTreeItemList::SetItemString(int index,const CString &string)
{
	m_values[index] = string;
}

void COptionTreeItemList::SetItemLong(int index,long value)
{
	m_values[index].Format("%d",value);
}

void COptionTreeItemList::SetItemDword(int index,DWORD value)
{
	m_values[index].Format("%d",value);
}

void COptionTreeItemList::SetItemFloat(int index,float value)
{
	if (m_significantDigits==0)
	{
		m_values[index].Format("%.0f", value);
	}
	else
	{
		char ascii[12];
		itoa(m_significantDigits,ascii,10);
		CString temp = "%.";
		temp += ascii;
		temp += "f";
		m_values[index].Format(temp, value);//AKJ
	}
}

void COptionTreeItemList::SetItemInt(int index,int value)
{
	m_values[index].Format("%d",value);
}

void COptionTreeItemList::SetItemDouble(int index,double value)
{
	if (m_significantDigits==0)
	{
		m_values[index].Format("%.0f", value);
	}
	else
	{
		char ascii[12];
		itoa(m_significantDigits,ascii,10);
		CString temp = "%.";
		temp += ascii;
		temp += "f";
		m_values[index].Format(temp, value);//AKJ
	}
}

BOOL COptionTreeItemList::GetNumItems(int &numItems) const
{
	numItems = m_values.GetSize();
	return TRUE;
}

BOOL COptionTreeItemList::GetNumColumns(int &numColumns) const
{
	numColumns = m_numColumns;
	return TRUE;
}

BOOL COptionTreeItemList::GetItemString(int index,CString &value) const
{
	value = m_values[index];
	return TRUE;
}

BOOL COptionTreeItemList::GetItemLong(int index,long &value) const
{
	if (GetOption(OT_LIST_NUMERICAL) == FALSE)
	{
		return FALSE;
	}

	// See if string is numeric
	if (IsStringNumeric(m_values[index]) == FALSE)
	{
		return FALSE;
	}

	// Convert string
	value = atoi(m_values[index]);
	return TRUE;
}

BOOL COptionTreeItemList::GetItemDword(int index,DWORD &value) const
{
	if (GetOption(OT_LIST_NUMERICAL) == FALSE)
	{
		return FALSE;
	}

	// See if string is numeric
	if (IsStringNumeric(m_values[index]) == FALSE)
	{
		return FALSE;
	}

	// Convert string
	value = atoi(m_values[index]);
	return TRUE;
}

BOOL COptionTreeItemList::GetItemFloat(int index,float &value) const
{
	if (GetOption(OT_LIST_NUMERICAL) == FALSE)
	{
		return FALSE;
	}

	// See if string is numeric
	if (IsStringNumeric(m_values[index]) == FALSE)
	{
		return FALSE;
	}

	// Convert string
	value = (float)atof(m_values[index]);
	return TRUE;
}

BOOL COptionTreeItemList::GetItemInt(int index,int &value) const
{
	if (GetOption(OT_LIST_NUMERICAL) == FALSE)
	{
		return FALSE;
	}

	// See if string is numeric
	if (IsStringNumeric(m_values[index]) == FALSE)
	{
		return FALSE;
	}

	// Convert string
	value = atoi(m_values[index]);
	return TRUE;
}

BOOL COptionTreeItemList::GetItemDouble(int index,double &value) const
{
	if (GetOption(OT_LIST_NUMERICAL) == FALSE)
	{
		return FALSE;
	}

	// See if string is numeric
	if (IsStringNumeric(m_values[index]) == FALSE)
	{
		return FALSE;
	}

	// Convert string
	value = atof(m_values[index]);
	return TRUE;
}




void COptionTreeItemList::OnDeSelect()
{
}

void COptionTreeItemList::OnSelect()
{
}

BOOL COptionTreeItemList::GetOption(DWORD dwOption) const
{
	// Return option
	return (m_dwOptions & dwOption) ? TRUE : FALSE;
}

void COptionTreeItemList::SetOption(DWORD dwOption, BOOL bSet)
{
	// Set option
	if (bSet == TRUE)
	{
		m_dwOptions |= dwOption;
	}
	else
	{
		m_dwOptions &= ~dwOption;
	}
}

BOOL COptionTreeItemList::IsStringNumeric(const CString &strString) const
{
	// See if string is numeric or not
	if (strString.FindOneOf("1234567890") == -1 || strString.FindOneOf("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ`~[]{}_=+\\|'/?>,<") != -1)
	{
		return FALSE;
	}

	return TRUE;
}

void COptionTreeItemList::OnActivate()
{
	OptionTreeListDlg dialog;
	dialog.SetNumColumns(m_numColumns);
	dialog.SetAdjustableColumns(GetOption(OT_LIST_ADJUST_COLUMNS)?true:false);
	dialog.SetNumeric(GetOption(OT_LIST_NUMERICAL)?true:false);
	dialog.SetSignificantDigits(m_significantDigits);
	dialog.SetAllowNegativeValues(m_bAllowNegativeValues);
	CStringArray tempValues;
	tempValues.Copy(m_values); // make copy
	dialog.SetStringData(&tempValues);
	if (dialog.DoModal() == IDOK)
	{
		m_values.Copy(tempValues);
		CommitChanges();
	}
}
