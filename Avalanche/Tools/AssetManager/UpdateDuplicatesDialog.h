#if !defined(AFX_UPDATEDUPLICATESDIALOG_H__3ABE0D5F_E010_4C77_9E21_6B976BADFC30__INCLUDED_)
#define AFX_UPDATEDUPLICATESDIALOG_H__3ABE0D5F_E010_4C77_9E21_6B976BADFC30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UpdateDuplicatesDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// UpdateDuplicatesDialog dialog

class CAssetManagerTreeCtrl;
class AssociatedIconManager;
class CPPToolTip;

class UpdateDuplicatesDialog : public CDialog
{
	UpdateDuplicatesManager *m_pUpdateDuplicatesManager;

// Construction
public:

	class MouseOverData
	{
	public:
		enum Type
		{
			OVER_NONE,
			OVER_FILE,
		};
		
		Type m_type;
		int m_setIndex;
		int m_entryIndex;
		
		MouseOverData(void) : m_type(OVER_NONE),m_setIndex(-1),m_entryIndex(-1) {}
		bool operator==(const MouseOverData &s) 
		{
			return (m_type == s.m_type && m_setIndex == s.m_setIndex && m_entryIndex == s.m_entryIndex);
		}
		bool operator!=(const MouseOverData &s) 
		{
			return (m_type != s.m_type || m_setIndex != s.m_setIndex || m_entryIndex!=s.m_entryIndex);
		}
	};

	UpdateDuplicatesDialog(CWnd* pParent = NULL);   // standard constructor
	~UpdateDuplicatesDialog();

	void SetUpdateDuplicatesManager(UpdateDuplicatesManager *pUpdateDuplicatesManager);
	void TreeMouseMove(UINT nFlags, CPoint point);
	void ShowContextMenu(CPoint point);

// Dialog Data
	//{{AFX_DATA(UpdateDuplicatesDialog)
	enum { IDD = IDD_UPDATE_DUPLICATES };
	CButton	m_cancelButton;
	CStatic	m_dupText2;
	CStatic	m_dupText1;
	CButton	m_updateButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(UpdateDuplicatesDialog)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:

	CAssetManagerTreeCtrl* m_pTreeCtrl;
	AssociatedIconManager* m_pAssociatedIconManager;	// Icon manager for tree controls
	CImageList m_ilTreeCtrls;									// Image list for the left and right tree controls

	// tool tips
	CPPToolTip *m_pDataTip;									// control for tooltips on tree controls
	MouseOverData m_mouseOverData;							// data holding data tooltip info

	CRect m_dialogRect;											// to keep track of dialog size and position

	bool m_bClosing;

	void SetUpTreeControl(void);
	CString GetMouseOverTipText(void);
	void SetupToolTips(void);
	int GetValidActions(void);
	void ChangeAction(UpdateDuplicatesManager::Actions action);
	void GetSelectionEntries(TupArray<int> &setIndexArray,TupArray<int> &entryIndexArray);
	void LoadTreeControl(void);
	void LoadTreeItem(int setIndex);
	void UpdateTreeItems(void);
	void SetTreeItem(int setIndex);
	void UpdateStats(void);

	static CString FormatTimeString(FILETIME fileTime);



	// Generated message map functions
	//{{AFX_MSG(UpdateDuplicatesDialog)
	afx_msg void OnUpdate();
	afx_msg void OnContextSetDefault();
	afx_msg void OnContextExclude();
	afx_msg void OnContextSourceFile();
	afx_msg void OnContextDestFile();
	afx_msg void OnContextDeleteFile();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnUpdateSetDefault(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSourceFile(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExclude(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDestFile(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDeleteFile(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UPDATEDUPLICATESDIALOG_H__3ABE0D5F_E010_4C77_9E21_6B976BADFC30__INCLUDED_)
