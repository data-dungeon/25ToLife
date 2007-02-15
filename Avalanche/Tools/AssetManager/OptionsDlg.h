//-----------------------------------------------------------------------------
//	File:		OptionsDlg.h
//	Created:	Sep 30, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The COptionsDlg class defines behavior for the options dialog.
//
//	History:	9/30/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------

#ifndef __OPTIONSDLG__
#define __OPTIONSDLG__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CMyListBox window

class CMyListBox : public CListBox
{
	// Construction
public:
	CMyListBox();
	
	// Attributes
public:
	
	// Operations
public:
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyListBox)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	virtual ~CMyListBox();
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CMyListBox)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

//-----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CDefaultUpdateDuplicatesPage dialog

class CDefaultUpdateDuplicatesPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDefaultUpdateDuplicatesPage)
		
		// Construction
public:
	CDefaultUpdateDuplicatesPage();
	~CDefaultUpdateDuplicatesPage();
	
	// Dialog Data
	//{{AFX_DATA(CDefaultUpdateDuplicatesPage)
	enum { IDD = IDD_UPDATE_DUPLICATES_OPTIONS };
	BOOL	m_includeFrozenDuplicates;
	BOOL	m_includeParentDirectory;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDefaultUpdateDuplicatesPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDefaultUpdateDuplicatesPage)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
};

/////////////////////////////////////////////////////////////////////////////
// CDefaultAutoFreezePage dialog

class CDefaultAutoFreezePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDefaultAutoFreezePage)
		
		// Construction
public:
	CDefaultAutoFreezePage();
	~CDefaultAutoFreezePage();
	
	// Dialog Data
	//{{AFX_DATA(CDefaultAutoFreezePage)
	enum { IDD = IDD_DEFAULT_AUTO_FREEZE_OPTIONS };
	CString	m_fileExtensions;
	CString	m_filePrefixes;
	CString	m_folderPrefixes;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDefaultAutoFreezePage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDefaultAutoFreezePage)
	afx_msg void OnResetButton();
	afx_msg void OnLoadButton();
	afx_msg void OnSaveButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
};

/////////////////////////////////////////////////////////////////////////////
// CFolderSetAutoFreezePage page

class CFolderSetAutoFreezePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CFolderSetAutoFreezePage)
		
		// Construction
public:
	CFolderSetAutoFreezePage();
	~CFolderSetAutoFreezePage();
	
	// Dialog Data
	//{{AFX_DATA(CFolderSetAutoFreezePage)
	enum { IDD = IDD_FOLDER_SET_AUTO_FREEZE_OPTIONS };
	CString	m_fileExtensions;
	CString	m_filePrefixes;
	CString	m_folderPrefixes;
	BOOL	m_bUseDefaultAutoExclude;
	//}}AFX_DATA
	

	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFolderSetAutoFreezePage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	void EnableControls(BOOL bEnable);
	
	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFolderSetAutoFreezePage)
	afx_msg void OnResetButton();
	afx_msg void OnLoadButton();
	afx_msg void OnSaveButton();
	afx_msg void OnUseAutoFreezeClicked();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
};

/////////////////////////////////////////////////////////////////////////////
// CDefaultUpdateDuplicatesPage dialog

class CAutoFreezeListPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CAutoFreezeListPage)
		
		// Construction
public:
	CAutoFreezeListPage();
	~CAutoFreezeListPage();
	
	// Dialog Data
	//{{AFX_DATA(CAutoFreezeListPage)
	enum { IDD = IDD_AUTO_FREEZE_LIST };
	CMyListBox	m_list;
	//}}AFX_DATA
	
	TupArray<TupString> m_entries;
	TupArray<bool> m_entryUsed;
	bool m_changed;
	
	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAutoFreezeListPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAutoFreezeListPage)
	afx_msg void OnClearAll();
	afx_msg void OnClearUnused();
	afx_msg void OnLoad();
	afx_msg void OnSave();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void CheckClearUnused(void);
		
};

/////////////////////////////////////////////////////////////////////////////
// CScanSettingsPage dialog

class CScanSettingsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CScanSettingsPage)
		
		// Construction
public:
	CScanSettingsPage();
	~CScanSettingsPage();
	
	// Dialog Data
	//{{AFX_DATA(CDefaultUpdateDuplicatesPage)
	enum { IDD = IDD_SCAN_SETTINGS };
	BOOL	m_bDoCRCCheck;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CScanSettingsPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CScanSettingsPage)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
public:
	int m_threadPriority;
};

/////////////////////////////////////////////////////////////////////////////
// CSyncSettingsPage dialog

class CSyncSettingsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSyncSettingsPage)

	// Construction
public:
	CSyncSettingsPage();
	~CSyncSettingsPage();

	// Dialog Data
	//{{AFX_DATA(CDefaultUpdateDuplicatesPage)
	enum { IDD = IDD_SYNC_SETTINGS };
	BOOL m_bUseSyncDates;
	BOOL m_bVerifyCopiedFiles;
	BOOL m_bCloseWhenCompleted;
	BOOL m_bExitWhenClosed;
	//}}AFX_DATA


	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSyncSettingsPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSyncSettingsPage)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int m_threadPriority;
};


//-----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// COptionSheet

class COptionSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(COptionSheet)
		
		// Construction
public:
	COptionSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	COptionSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	
	// Attributes
public:
	
	// Operations
public:
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionSheet)
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	virtual ~COptionSheet();
	
	// Generated message map functions
protected:
	//{{AFX_MSG(COptionSheet)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
