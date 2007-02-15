/***********************************/
/*                                 */
/* PageSize.h                      */
/* Pigpen particle tool  12/28/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle display property page  */
/*                                 */
/***********************************/

#ifndef __PAGESIZE_H
#define __PAGESIZE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************* includes ****************************/

#include "resource.h"

/******************* defines *****************************/

/******************* forward declarations ****************/

class CDocView;
class CParticleSheet;
class CParticle;

/******************* class *******************************/

class CSizePage : public CPropertyPage
{
public:

	CSizePage(CDocView *pParentView, CParticleSheet *pParentSheet);	// constructor

// Dialog Data-- helps class wizard know what controls are available
	//{{AFX_DATA(CSizePage)
	enum { IDD = IDD_PAGE_SIZE };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSizePage)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL

// Implementation

public:

	void ShowData(void);

protected:
	
	CDocView *m_pParentView;
	CParticleSheet *m_pParentSheet;
	CParticle *m_pParticle;

	/* dialog control processing */

	void UpdateEditBox(int nID, int nDelta, float *pfDirectValue, bool bCheckSpecialCases);
	void UpdateCheckBox(int nID, bool bToggle);
	void UpdateRadioButton(int nID, bool bUpdate);

	/* higher-level message handlers, called from OnNotify and OnCommand overrides */

	bool OnSpinner(NMHDR* pNMHDR, LRESULT* pResult);
	bool OnKillFocusEdit(int nID);
	bool OnSelChange(int nID);
	bool OnButtonClicked(int nID);

	//{{AFX_MSG(CSizePage)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__PAGESIZE_H
