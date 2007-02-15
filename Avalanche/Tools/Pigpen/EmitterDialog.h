/***********************************/
/*                                 */
/* EmitterDialog.h                 */
/* Pigpen particle tool  01/22/01  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* dialog box for editing emitters */
/*                                 */
/***********************************/

#ifndef __EMITTERDIALOG_H
#define __EMITTERDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************* includes ****************************/

#include "resource.h"
#include "Emitter.h"

/******************* defines *****************************/

/******************* forward declarations ****************/

class CDocView;

/******************* class *******************************/

class CEmitterDialog : public CDialog
{
public:

	CEmitterDialog(CDocView *pParentView, CWnd* pParentWnd);	// constructor
	~CEmitterDialog();	// destructor
	BOOL Create();

// Dialog Data-- helps class wizard know what controls are available
	//{{AFX_DATA(CEmitterDialog)
	enum { IDD = IDD_EMITTER_DIALOG };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEmitterDialog)
	protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation

public:

	void ShowData(CEmitter *pEmitter);

protected:
	
	CDocView* m_pParentView;
	CWnd* m_pParentWnd;
	CEmitter *m_pEmitter;			// current emitter being edited
	CEmitter m_OriginalEmitter;	// current emitter before editing, for reverting

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

	//{{AFX_MSG(CEmitterDialog)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__EMITTERDIALOG_H
