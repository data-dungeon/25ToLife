/*************************************/
/*                                   */
/* PagePiggyback.h                   */
/* Pigpen particle tool  4/16/02     */
/* ned martin  avalanche software    */
/* visual c++ 6.0                    */
/* particle piggyback property page  */
/*                                   */
/*************************************/

#ifndef __PAGEPIGGYBACK_H
#define __PAGEPIGGYBACK_H

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

class CPiggybackPage : public CPropertyPage
{
public:

	CPiggybackPage(CDocView *pParentView, CParticleSheet *pParentSheet);	// constructor

// Dialog Data-- helps class wizard know what controls are available
	//{{AFX_DATA(CPiggybackPage)
	enum { IDD = IDD_PAGE_PIGGYBACK };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPiggybackPage)
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

	//{{AFX_MSG(CPiggybackPage)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__PAGEPIGGYBACK_H
