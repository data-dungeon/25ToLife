/***********************************/
/*                                 */
/* App.h                           */
/* Pigpen particle tool  12/07/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* application class               */
/*                                 */
/***********************************/

#ifndef __PIGPENAPP_H
#define __PIGPENAPP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************* includes ****************************/

#include "resource.h"       // main symbols
#include "doc.h"

/******************* defines *****************************/

/******************* forward declarations ****************/

/******************* class *******************************/

/////////////////////////////////////////////////////////////////////////////
// CPigpenApp:
// See App.cpp for the implementation of this class
//

class CPigpenApp : public CWinApp
{
public:
	CPigpenApp();
	~CPigpenApp();

	void SetOpenedDoc(CPigpenDoc *pDoc) {m_pOpenedDoc = pDoc;}

// Document view(s).
	CDocTemplate *pDocTemplate;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPigpenApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual void OnFileOpen();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);

	//}}AFX_VIRTUAL

// Implementation

	bool IsCommandLineMode(void)			{ return(m_bCommandLineMode); }
	void SetReturnCode(int nReturnCode)	{ m_nReturnCode = nReturnCode; }
	enum {
		RETURN_CODE_SUCCESS = 0,
		RETURN_CODE_PIG_FILE_NOT_FOUND = 1,
		RETURN_CODE_INVALID_PIG_FILE = 2,
		RETURN_CODE_INVALID_VERSION = 3,
		RETURN_CODE_WRITE_PROTECTED = 4,
	};

	/* window restoring on file load-- this is one-shot data to be used by CDocView::OnInitialUpdate. */
	/* put it in the app, not the document, because new doc views area created via the pDocTemplate, which */
	/* creates a new doc, creates the view, then moves the view to the existing doc */

	bool GetRestoreWindow(void) { return(m_bRestoreWindow); }
	void SetRestoreWindow(bool bValue) { m_bRestoreWindow = bValue; }
	void SetRestoreWindowRect(CRect &Rect) { m_RestoreWindowRect = Rect; }
	CRect &GetRestoreWindowRect(void) { return(m_RestoreWindowRect); }
	void SetRestoreWindowHorzScroll(SCROLLINFO *pScrollInfo) { m_pRestoreWindowHorzScroll = pScrollInfo; }
	void SetRestoreWindowVertScroll(SCROLLINFO *pScrollInfo) { m_pRestoreWindowVertScroll = pScrollInfo; }
	SCROLLINFO *GetRestoreWindowHorzScroll(void) { return(m_pRestoreWindowHorzScroll); }
	SCROLLINFO *GetRestoreWindowVertScroll(void) { return(m_pRestoreWindowVertScroll); }
	CString &GetDefaultStageFile(void) { return(DefaultStageFile); }

protected:

	/* document being loaded */

	CPigpenDoc *m_pOpenedDoc;

	/* command line processing-- so pigpen can be called from a batch file to do data conversion */

	CPigpenDoc *m_pCommandLineDoc;
	bool m_bCommandLineMode;
	char *m_pExportFileName;
	int m_nExportMode;

	/* for restoring window position/status when opening a file. for example, referenced by CDocView::OnInitialUpdate */

	bool m_bRestoreWindow;
	CRect m_RestoreWindowRect;
	SCROLLINFO *m_pRestoreWindowHorzScroll;
	SCROLLINFO *m_pRestoreWindowVertScroll;

	/* default preview stage, set at execution time */

	CString DefaultStageFile;

	/* return code from program-- useful in batch mode */

	int m_nReturnCode;

	//{{AFX_MSG(CPigpenApp)
	afx_msg void OnViewSFX();
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// the application

extern CPigpenApp PigpenApp;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__PIGPENAPP_H
