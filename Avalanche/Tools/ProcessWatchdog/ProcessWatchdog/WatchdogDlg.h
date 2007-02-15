////////////////////////////////////////////////////////////////////////////
//
// ProcessList
//
// Class to query the current processes in the system
//
////////////////////////////////////////////////////////////////////////////

/*
* $History: WatchdogDlg.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 10/02/03   Time: 12:37p
 * Updated in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * looks like we are here!
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 9/29/03    Time: 2:44p
 * Updated in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * starting applications - killing processes
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 9/24/03    Time: 3:23p
 * Updated in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * updating of watchdogs
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 9/22/03    Time: 3:37p
 * Created in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * watchdog property page
*/

#ifndef PROCESS_WATCHDOG_WATCHDOG_DIALOG_H
#define PROCESS_WATCHDOG_WATCHDOG_DIALOG_H
#include "afxwin.h"

//-----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CGeneralPage dialog

class CGeneralPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGeneralPage)

	// Construction
public:
	CGeneralPage();
	~CGeneralPage();

	// Dialog Data
	//{{AFX_DATA(CGeneralPage)
	enum { IDD = IDD_GENERAL };
	//}}AFX_DATA


	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGeneralPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGeneralPage)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CString m_name;
	float m_checkInterval;
	float m_waitForResponse;
	float m_waitAfterCreation;
	float m_waitForNoProcesses;
};

/////////////////////////////////////////////////////////////////////////////
// CActiveProcessPage dialog

class CActiveProcessPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CActiveProcessPage)

	// Construction
public:
	CActiveProcessPage();
	~CActiveProcessPage();

	// Dialog Data
	//{{AFX_DATA(CActiveProcessPage)
	enum { IDD = IDD_ACTIVE_PROCESS };
	//}}AFX_DATA


	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CActiveProcessPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CActiveProcessPage)
	afx_msg void OnBnClickedNoAction();
	afx_msg void OnBnClickedKillProcess();
	afx_msg void OnBnClickedSetPriority();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void EnableControls(void);

public:
	int m_setting;
	int m_priority;
};

/////////////////////////////////////////////////////////////////////////////
// CNoProcessesPage page

class CNoProcessesPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CNoProcessesPage)

	// Construction
public:
	CNoProcessesPage();
	~CNoProcessesPage();

	// Dialog Data
	//{{AFX_DATA(CNoProcessesPage)
	enum { IDD = IDD_NO_PROCESSES };
	//}}AFX_DATA


	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNoProcessesPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNoProcessesPage)
	afx_msg void OnBnClickedNoAction();
	afx_msg void OnBnClickedOpen();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void EnableControls(void);

public:
	int m_setting;
	CString m_targetPath;
	CString m_startInPath;
	CString m_parameters;
	int m_runType;
	afx_msg void OnBnClickedTargetBrowse();
	afx_msg void OnBnClickedStartInBrowse();
};

/////////////////////////////////////////////////////////////////////////////
// CDefaultUpdateDuplicatesPage dialog

class CNotRepondingPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CNotRepondingPage)

	// Construction
public:
	CNotRepondingPage();
	~CNotRepondingPage();

	// Dialog Data
	//{{AFX_DATA(CNotRepondingPage)
	enum { IDD = IDD_NOT_RESPONDING };
	//}}AFX_DATA

	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNotRepondingPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNotRepondingPage)
	afx_msg void OnBnClickedNoAction();
	afx_msg void OnBnClickedKillProcess();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void EnableControls(void);

public:
	int m_setting;
};

/////////////////////////////////////////////////////////////////////////////
// CWatchFilePage dialog

class CWatchFilePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CWatchFilePage)

	// Construction
public:
	CWatchFilePage();
	~CWatchFilePage();

	// Dialog Data
	//{{AFX_DATA(CDefaultUpdateDuplicatesPage)
	enum { IDD = IDD_WATCH_FILE };
	//}}AFX_DATA


	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWatchFilePage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWatchFilePage)
	afx_msg void OnBnClickedNoAction();
	afx_msg void OnBnClickedKillProcess();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void EnableControls(void);

public:
	int m_setting;
	CString m_watchFilePath;
	afx_msg void OnBnClickedWatchFileBrowse();
};

//-----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CWatchdogSheet

class CWatchdogSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CWatchdogSheet)

	// Construction
public:
	CWatchdogSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CWatchdogSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	// Attributes
public:

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWatchdogSheet)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CWatchdogSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWatchdogSheet)
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
