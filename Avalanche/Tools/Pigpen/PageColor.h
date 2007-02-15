/***********************************/
/*                                 */
/* PageColor.h                     */
/* Pigpen particle tool  1/3/01    */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle color property page    */
/*                                 */
/***********************************/

#ifndef __PAGECOLOR_H
#define __PAGECOLOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************* includes ****************************/

#include "resource.h"
#include "ParticleSwoosh.h"

/******************* defines *****************************/

/******************* forward declarations ****************/

class CDocView;
class CParticleSheet;
class CSwooshSheet;

/******************* class *******************************/

class CColorPage : public CPropertyPage
{
public:

	CColorPage(CDocView *pParentView, CParticleSheet *pParentParticleSheet, CSwooshSheet *pParentSwooshSheet);	// constructor

// Dialog Data-- helps class wizard know what controls are available
	//{{AFX_DATA(CColorPage)
	enum { IDD = IDD_PAGE_COLOR };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorPage)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorPage)
	protected:
	//}}AFX_VIRTUAL

// Implementation

public:

	void ShowData(void);

protected:
	
	CDocView *m_pParentView;

	/* this page can handle ownership by a particle OR a swoosh */

	CParticleSheet *m_pParentParticleSheet;
	CSwooshSheet *m_pParentSwooshSheet;
	CParticleSwoosh m_ParticleSwoosh;	// combined particle/swoosh for coding simplicity

	/* dialog control processing */

//	void UpdateEditBox(int nID, int nDelta, float *pfDirectValue, bool bCheckSpecialCases);
//	void UpdateCheckBox(int nID, bool bToggle);
	void UpdateRadioButton(int nID, bool bUpdate);

	/* higher-level message handlers, called from OnNotify and OnCommand overrides */

//	bool OnSpinner(NMHDR* pNMHDR, LRESULT* pResult);
//	bool OnKillFocusEdit(int nID);
//	bool OnSelChange(int nID);
	bool OnButtonClicked(int nID);

	//{{AFX_MSG(CColorPage)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__PAGECOLOR_H
