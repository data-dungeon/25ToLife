/***********************************/
/*                                 */
/* EffectDialog.h                  */
/* Pigpen particle tool  12/07/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* dialog box for editing effects  */
/*                                 */
/***********************************/

#ifndef __EFFECTDIALOG_H
#define __EFFECTDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************* includes ****************************/

#include "resource.h"
#include "Effect.h"

/******************* defines *****************************/

/******************* forward declarations ****************/

class CDocView;

/******************* class *******************************/

class CEffectDialog : public CDialog
{
public:

	CEffectDialog(CDocView *pParentView, CWnd* pParentWnd);	// constructor
	~CEffectDialog();	// destructor
	BOOL Create();

// Dialog Data-- helps class wizard know what controls are available?
	//{{AFX_DATA(CEffectDialog)
	enum { IDD = IDD_EFFECT_DIALOG };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffectDialog)
	protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation

public:

	void ShowData(CEffect *pEffect);

protected:
	
	CDocView* m_pParentView;
	CWnd* m_pParentWnd;
	CEffect *m_pEffect;				// current effect being edited
	CEffect m_OriginalEffect;		// current effect before editing, for reverting
	int m_nClipboardFormat;			// format for clipboard data
	int m_nClipboardRecordSize;	// size of a single item (e.g. sizeof(CParticle))
	int m_nChannelIndex;				// which channel is currently shown

	/* dialog control processing */

	void UpdateEditBox(int nID, int nDelta, float *pfDirectValue, bool bCheckSpecialCases);
	void UpdateCheckBox(int nID, bool bToggle);
	void UpdateRadioButton(int nID, bool bUpdate);
	
	/* higher-level message handlers, called from OnNotify and OnCommand overrides */

	bool OnCustomdrawSlider(NMHDR* pNMHDR, LRESULT* pResult); 
	bool OnSpinner(NMHDR* pNMHDR, LRESULT* pResult);
	bool OnKillFocusEdit(int nID);
	bool OnSelChange(int nID);
	bool OnButtonClicked(int nID);

	/* copy/paste */

	void OnCopy(void);
	void OnPaste(void);
	void OnClear(void);

	BOOL DoCopy(void);
	BOOL DoPaste(void);

	//{{AFX_MSG(CEffectDialog)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);		// right mouse button brings up popup menu
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__EFFECTDIALOG_H
