/***********************************/
/*                                 */
/* SurfaceListDialog.h             */
/* Pigpen particle tool  4/6/02    */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* dialog box for surface list     */
/*                                 */
/***********************************/

#ifndef __SURFACELISTDIALOG_H
#define __SURFACELISTDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************* includes ****************************/

#include "resource.h"

/******************* defines *****************************/

/******************* forward declarations ****************/

class CPigpenDoc;

/******************* class *******************************/

class CSurfaceListDialog : public CDialog
{
public:

	CSurfaceListDialog(CPigpenDoc *pDoc);	// constructor

// Dialog Data-- helps class wizard know what controls are available
	//{{AFX_DATA(CSurfaceListDialog)
	enum { IDD = IDD_SURFACE_LIST };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSurfaceListDialog)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSurfaceListDialog)
	protected:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation

public:

	void ShowData(void);

protected:
	
	CPigpenDoc *m_pDoc;

	/* dialog control processing */

	void UpdateEditBox(int nID, int nDelta, float *pfDirectValue, bool bCheckSpecialCases);
	void UpdateCheckBox(int nID, bool bToggle);
	void UpdateRadioButton(int nID, bool bUpdate);

	/* higher-level message handlers, called from OnNotify and OnCommand overrides */

	bool OnSpinner(NMHDR* pNMHDR, LRESULT* pResult);
	bool OnKillFocusEdit(int nID);
	bool OnSelChange(int nID);
	bool OnButtonClicked(int nID);

	//{{AFX_MSG(CSurfaceListDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__SURFACELISTDIALOG_H
