/***********************************/
/*                                 */
/* ImportDialog.h                  */
/* Pigpen particle tool  12/22/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* dialog box for image import     */
/*                                 */
/***********************************/

#ifndef __IMPORTDIALOG_H
#define __IMPORTDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************* includes ****************************/

#include "resource.h"

/******************* defines *****************************/

/* flags for return value-- use big values to make sure we don't end up with IDCANCEL */

#define IMPORT_REPLACE_EXISTING	0x1000
#define IMPORT_ALL					0x2000
#define IMPORT_SINGLE				0x4000

/******************* forward declarations ****************/

class CGraphic;

/******************* class *******************************/

class CImportDialog : public CDialog
{
public:

	CImportDialog(CGraphic *pGraphic, CString& File, CString& WildcardFile, int nImageCount, bool bReplace, bool bUpdate);	// constructor

// Dialog Data-- helps class wizard know what controls are available
	//{{AFX_DATA(CImportDialog)
	enum { IDD = IDD_IMPORT_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportDialog)
	protected:
	//}}AFX_VIRTUAL

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportDialog)
	protected:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation

public:

protected:
	
	CGraphic *m_pGraphic;
	CString m_File;
	CString m_WildcardFile;
	int m_nImageCount;
	int m_nReplaceExisting;
	bool m_bReplace;
	bool m_bUpdate;

	//{{AFX_MSG(CImportDialog)
	afx_msg void OnImportAll();
	afx_msg void OnImportSingle();
	afx_msg void OnReplaceCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__IMPORTDIALOG_H
