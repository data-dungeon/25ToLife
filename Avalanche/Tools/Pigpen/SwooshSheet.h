/***********************************/
/*                                 */
/* SwooshSheet.h                   */
/* Pigpen particle tool  10/29/02  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* property sheet to edit swooshes */
/*                                 */
/***********************************/

#ifndef __SWOOSHSHEET_H
#define __SWOOSHSHEET_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************* includes ****************************/

#include "resource.h"
#include "Swoosh.h"
#include "PageSwooshDisplay.h"
#include "PageColor.h"
#include "PageEnvelope.h"
//#include "SwooshLifetime.h"

/******************* defines *****************************/

/******************* forward declarations ****************/

class CDocView;

/******************* class *******************************/

class CSwooshSheet : public CPropertySheet
{
public:

	CSwooshSheet(CDocView *pParentView, CWnd* pParentWnd);	// constructor
	~CSwooshSheet();	// destructor
	BOOL Create();

// Dialog Data-- helps class wizard know what controls are available
	//{{AFX_DATA(CSwooshSheet)
	//}}AFX_DATA

// Attributes
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSwooshSheet)
	public:
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation

public:

	CSwoosh *GetSwoosh(void) { return(m_pSwoosh); }
	void ShowData(CSwoosh *pSwoosh);
	void ShowActivePage(void);

protected:
	
	CDocView* m_pParentView;
	CWnd* m_pParentWnd;
	CSwoosh *m_pSwoosh;			// current swoosh being edited
//	CSwoosh m_OriginalSwoosh;	// current swoosh before editing, for reverting

	/* property pages-- the dialogs that make up the property sheet */

	CSwooshDisplayPage	*m_pDisplayPage;
	CColorPage				*m_pColorPage;
	CEnvelopePage			*m_pEnvelopePage;
//	CLifetimePage			*m_pLifetimePage;

	//{{AFX_MSG(CSwooshSheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__SWOOSHSHEET_H
