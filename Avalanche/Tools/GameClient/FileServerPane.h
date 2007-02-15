#if !defined(AFX_FILESERVERPANE_H__01ED3C4B_C3AE_4690_86A9_62F5BB1E413D__INCLUDED_)
#define AFX_FILESERVERPANE_H__01ED3C4B_C3AE_4690_86A9_62F5BB1E413D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileServerPane.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileServerPane dialog

#include "GameClientPane.h"

#define MAX_OPENFILES 30
#define MAX_HISTORIES 1000

class CFileServerPane : public CGameClientPane
{
	DECLARE_DYNAMIC(CFileServerPane);

	struct Entry
	{
		int id;
		int mode;
		CString fileName;
		int bytesRead;
		int bytesWritten;
	};


// Construction
public:
	void AddDefunctConnection(CString filename, int mode, int defuncCode, int errorCode);
	void UpdateOpenConnection(int id, int read, int write);
	void RemoveOpenConnection(int id);
	void AddOpenConnection(int id, CString filename, int mode);
	CFileServerPane(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileServerPane)
	enum { IDD = IDD_FILESERVER };
	CListCtrl	m_FileLog;
	BOOL	m_HideErrors;
	//}}AFX_DATA
	virtual int GetDialogIDD() { return IDD; }

	Entry m_Open[MAX_OPENFILES];
	Entry m_History[MAX_HISTORIES];
	int m_NextHistory;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileServerPane)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL IsViewingHistory();
	void InsertItem(const Entry& entry);
	void DeleteItem(const Entry& entry);
	void UpdateItem(int index, const Entry &entry);
	void UpdateItem(const Entry& entry);
	int FindItem(int id);

	// Generated message map functions
	//{{AFX_MSG(CFileServerPane)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnCurrentlyopenfiles();
	afx_msg void OnFileaccesshistory();
	afx_msg void OnClearhistory();
	afx_msg void OnHideerrors();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILESERVERPANE_H__01ED3C4B_C3AE_4690_86A9_62F5BB1E413D__INCLUDED_)
