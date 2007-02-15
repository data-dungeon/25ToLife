// OptionTreeListDlg.h : header file
//

#ifndef OPTION_TREE_LIST_DLG_H
#define OPTION_TREE_LIST_DLG_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// OptionTreeListDlg dialog

class OptionTreeListDlg : public CDialog
{
	// Construction
public:
	OptionTreeListDlg(CWnd* pParent = NULL);	// standard constructor

	void SetNumColumns(int numColumns);
	void SetAdjustableColumns(bool bAdjustColumns);
   void SetNumeric(bool bNumeric);
	void SetSignificantDigits(int significantDigits);
	void SetAllowNegativeValues(bool bAllowNegativeValues);
	void SetStringData(CStringArray *values);
	void PopulateGrid(void);

	// Dialog Data
	//{{AFX_DATA(OptionTreeListDlg)
	enum { IDD = IDD_GRID_DLG };
	CStatic m_numColumnsStatic;
	CComboBox m_numColumnsCombo;
	//}}AFX_DATA
	CStringArray *m_pValues;
	CGridCtrl m_grid;
	CSize m_OldSize;
	int m_nCols;
	int m_nRows;
	bool m_bAdjustColumns;
	bool m_bNumeric;
	bool m_bAllowNegativeValues;
	int m_significantDigits;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OptionTreeListDlg)
public:
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(OptionTreeListDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void OnGridCellChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeNumColumnsCombo();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(OPTION_TREE_LIST_DLG_H)
