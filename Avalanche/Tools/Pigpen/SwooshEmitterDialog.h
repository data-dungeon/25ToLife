/***********************************/
/*                                 */
/* SwooshEmitterDialog.h           */
/* Pigpen particle tool  11/25/02  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* dialog box for editing emitters */
/*                                 */
/***********************************/

#ifndef __SWOOSHEMITTERDIALOG_H
#define __SWOOSHEMITTERDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************* includes ****************************/

#include "resource.h"
#include "SwooshEmitter.h"

/******************* defines *****************************/

/******************* forward declarations ****************/

class CDocView;

/******************* class *******************************/

class CSwooshEmitterDialog : public CDialog
{
public:

	CSwooshEmitterDialog(CDocView *pParentView, CWnd* pParentWnd);	// constructor
	~CSwooshEmitterDialog();	// destructor
	BOOL Create();

// Dialog Data-- helps class wizard know what controls are available
	//{{AFX_DATA(CSwooshEmitterDialog)
	enum { IDD = IDD_SWOOSH_EMITTER_DIALOG };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSwooshEmitterDialog)
	protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation

public:

	void ShowData(CPigpenSwooshEmitter *pSwooshEmitter);

protected:
	
	CDocView* m_pParentView;
	CWnd* m_pParentWnd;
	CPigpenSwooshEmitter *m_pSwooshEmitter;			// current emitter being edited
	CPigpenSwooshEmitter m_OriginalSwooshEmitter;	// current emitter before editing, for reverting

	/* dialog control processing */

	void UpdateEditBox(int nID, int nDelta, float *pfDirectValue, bool bCheckSpecialCases);
	void UpdateCheckBox(int nID, bool bToggle);
//	void UpdateRadioButton(int nID, bool bUpdate);

	/* higher-level message handlers, called from OnNotify and OnCommand overrides */

	bool OnCustomdrawSlider(NMHDR* pNMHDR, LRESULT* pResult); 
	bool OnSpinner(NMHDR* pNMHDR, LRESULT* pResult);
	bool OnKillFocusEdit(int nID);
	bool OnSelChange(int nID);
	bool OnButtonClicked(int nID);

	//{{AFX_MSG(CSwooshEmitterDialog)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__SWOOSHEMITTERDIALOG_H
