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


#ifndef OT_ITEMLIST
#define OT_ITEMLIST

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionTreeItemEdit.h : header file
//

// Added Headers
#include "OptionTreeDef.h"
#include "OptionTreeItem.h"

// Classes
class COptionTree;

/////////////////////////////////////////////////////////////////////////////
// COptionTreeItemList window

class COptionTreeItemList : public COptionTreeItem
{
	// Construction
public:
	COptionTreeItemList();

	// Attributes
public:

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionTreeItemList)
	//}}AFX_VIRTUAL

	// Implementation
public:
	BOOL GetOption(DWORD dwOption) const;
	void SetOption(DWORD dwOption, BOOL bSet);

	void SetNumItems(int numItems);
	void SetNumColumns(int numColumns);
	void SetItemString(int index,const CString &string);
	void SetItemLong(int index,long value);
	void SetItemDword(int index,DWORD value);
	void SetItemFloat(int index,float value);
	void SetItemInt(int index,int value);
	void SetItemDouble(int index,double value);

	virtual void OnMove();
	virtual void OnRefresh();
	virtual void OnCommit();
	virtual void OnActivate();
	virtual void CleanDestroyWindow();
	virtual void OnDeSelect();
	virtual void OnSelect();
	virtual void DrawAttribute(CDC *pDC, const RECT &rcRect);

	BOOL GetNumItems(int &numItems) const;
	BOOL GetNumColumns(int &numColumns) const;
	BOOL GetItemString(int index,CString &value) const;
	BOOL GetItemLong(int index,long &value) const;
	BOOL GetItemDword(int index,DWORD &value) const;
	BOOL GetItemFloat(int index,float &value) const;
	BOOL GetItemInt(int index,int &value) const;
	BOOL GetItemDouble(int index,double &value) const;
	virtual ~COptionTreeItemList();

	void SetSignificantDigits(int significantDigits) { m_significantDigits = significantDigits; }//AKJ
	void SetAllowNegativeValues(bool bAllowNegativeValues) { m_bAllowNegativeValues = bAllowNegativeValues; }

protected:
	BOOL IsStringNumeric(const CString &strString) const;

	CStringArray m_values;
	int m_numColumns;
	DWORD m_dwOptions;
	int	m_significantDigits;
	bool	m_bAllowNegativeValues;	// determines whether negative values are allowed
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !OT_ITEMEDIT
