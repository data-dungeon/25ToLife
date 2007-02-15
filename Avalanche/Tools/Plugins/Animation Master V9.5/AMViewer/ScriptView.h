// DAN  10/30/98  \DanB\AMViewer\ScriptView.h

#define __SCRIPTVIEW_H

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "NoAccelEdit.h"
#include "Resource.h"

class CAMViewerDoc;
class Action; 

class CScriptView : public CFormView
{
protected:
	CScriptView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CScriptView)

// Form Data
public:
	//{{AFX_DATA(CScriptView)
	enum { IDD = IDD_SCRIPT };
	CNoAccelEdit	m_fpsedit;
	CButton	m_openmodel;
	CButton	m_deleteaction;
	CButton	m_addaction;
	CListCtrl	m_actionlist;
	//}}AFX_DATA
   BOOL m_bbuiltcolumns;

 	CAMViewerDoc* GetDocument();

   void SetActiveActions();
   void DeleteContents();
   void AddActionToListCtrl(Action *action);
   void RefreshActionList();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CScriptView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CScriptView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnAddAction();
	afx_msg void OnDeleteAction();
	afx_msg void OnOpenModel();
	afx_msg void OnKillfocusFpsEdit();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnItemchangedActionList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in CScriptView.cpp
inline CAMViewerDoc* CScriptView::GetDocument()
   { return (CAMViewerDoc*)m_pDocument; }
#endif

