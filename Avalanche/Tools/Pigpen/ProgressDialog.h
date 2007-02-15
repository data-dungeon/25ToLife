/***********************************/
/*                                 */
/* ProgressDialog.h                */
/* Pigpen particle tool  2/3/01    */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* simple progress dialog          */
/*                                 */
/***********************************/

#ifndef __PROGRESSDIALOG_H
#define __PROGRESSDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************* includes ****************************/

#include "resource.h"

/******************* defines *****************************/

/******************* forward declarations ****************/

/******************* class *******************************/

class CProgressDialog : public CDialog
{
public:

	CProgressDialog(CString Title, CString File, int nTotalItems);	// constructor

// Dialog Data-- helps class wizard know what controls are available
	//{{AFX_DATA(CProgressDialog)
	enum { IDD = IDD_PROGRESS };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressDialog)
	protected:
	//}}AFX_VIRTUAL

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressDialog)
	protected:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation

public:

	void UpdateProgress(void);
	void UpdateInfo(char *pString);

protected:
	
	CString m_Title;
	CString m_File;
	int m_nTotalItems;	// number of items to be processed to reach 100%
	int m_nItems;			// number of items done so far

	//{{AFX_MSG(CProgressDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__PROGRESSDIALOG_H
