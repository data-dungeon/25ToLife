/***********************************/
/*                                 */
/* PageMovement.h                  */
/* Pigpen particle tool  12/28/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle movement property page */
/*                                 */
/***********************************/

#ifndef __PAGEMOVEMENT_H
#define __PAGEMOVEMENT_H

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

class CMovementPage : public CPropertyPage
{
public:

	CMovementPage(CDocView *pParentView, CParticleSheet *pParentSheet);	// constructor

// Dialog Data-- helps class wizard know what controls are available
	//{{AFX_DATA(CMovementPage)
	enum { IDD = IDD_PAGE_MOVEMENT };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMovementPage)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMovementPage)
	protected:
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
//	void UpdateRadioButton(int nID, bool bUpdate);

	/* higher-level message handlers, called from OnNotify and OnCommand overrides */

	bool OnSpinner(NMHDR* pNMHDR, LRESULT* pResult);
	bool OnKillFocusEdit(int nID);
//	bool OnSelChange(int nID);
	bool OnButtonClicked(int nID);

	//{{AFX_MSG(CMovementPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__PAGEMOVEMENT_H
