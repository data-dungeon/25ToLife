/***********************************/
/*                                 */
/* PreviewDialog.h                 */
/* Pigpen particle tool  1/25/01   */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* preview dialog                  */
/*                                 */
/***********************************/

#ifndef __PREVIEWDIALOG_H
#define __PREVIEWDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************* includes ****************************/

#include "resource.h"

/******************* defines *****************************/

/******************* forward declarations ****************/

class CDocView;
class CParticle;
class CPigpenDoc;

/******************* class *******************************/

class CPreviewDialog : public CDialog
{
public:

	CPreviewDialog(CWnd* pParentWnd);	// constructor
	~CPreviewDialog();
	BOOL Create(CPigpenDoc *pPreviewDoc);
	void InitialPosition(void);

// Dialog Data-- helps class wizard know what controls are available
	//{{AFX_DATA(CPreviewDialog)
	enum { IDD = IDD_PREVIEW_DIALOG };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreviewDialog)
	protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation

public:

	void ShowData(void);

protected:
	
	CWnd *m_pParentWnd;

	/* dialog control processing */

	void UpdateEditBox(int nID, int nDelta, float *pfDirectValue, bool bCheckSpecialCases);
	void UpdateCheckBox(int nID, bool bToggle);
//	void UpdateRadioButton(int nID, bool bUpdate);

	/* higher-level message handlers, called from OnNotify and OnCommand overrides */

	bool OnSpinner(NMHDR* pNMHDR, LRESULT* pResult);
	bool OnKillFocusEdit(int nID);
	bool OnSelChange(int nID);
	bool OnButtonClicked(int nID);

	//{{AFX_MSG(CPreviewDialog)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__PREVIEWDIALOG_H
