
#ifndef TUP_PROPERTY_VIEW_H
#define TUP_PROPERTY_VIEW_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class PropertyManager;

/////////////////////////////////////////////////////////////////////////////
// TupPropertyView

class TupPropertyView : public CExtResizableDialog
{
	// Construction
public:
	TupPropertyView(CWnd* pParent = NULL);	// standard constructor
	~TupPropertyView();

	void Refresh(void);
	void BuildProperties(TupperwareAtom *pAtom);

	// Dialog Data
	//{{AFX_DATA(TupPropertyView)
	enum { IDD = IDD_PROPERTYVIEW };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCOptionTreeDemoDlg)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	void OnApplyFont(LPARAM lParam);
	LRESULT WM_ApplyButton(WPARAM wParam, LPARAM lParam);
	void OnOptionItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
	HICON m_hIcon;
	COptionTree m_otTree;
	PropertyManager *m_pPropertyManager;
	UINT m_uApplyFontID;
	// Generated message map functions
	//{{AFX_MSG(CCOptionTreeDemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // TUP_PROPERTY_VIEW_H
