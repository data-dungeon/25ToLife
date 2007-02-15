/***********************************/
/*                                 */
/* ParticleSheet.h                 */
/* Pigpen particle tool  12/10/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* property sheet to edit particles*/
/*                                 */
/***********************************/

#ifndef __PARTICLESHEET_H
#define __PARTICLESHEET_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************* includes ****************************/

#include "resource.h"
#include "Particle.h"
#include "PageDisplay.h"
#include "PageSize.h"
#include "PageMovement.h"
#include "PageColor.h"
#include "PageCollide.h"
#include "PageEnvelope.h"
#include "PageLifetime.h"
#include "PagePiggyback.h"

/******************* defines *****************************/

/******************* forward declarations ****************/

class CDocView;

/******************* class *******************************/

class CParticleSheet : public CPropertySheet
{
public:

	CParticleSheet(CDocView *pParentView, CWnd* pParentWnd);	// constructor
	~CParticleSheet();	// destructor
	BOOL Create();

// Dialog Data-- helps class wizard know what controls are available
	//{{AFX_DATA(CParticleSheet)
	//}}AFX_DATA

// Attributes
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParticleSheet)
	public:
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation

public:

	CParticle *GetParticle(void) { return(m_pParticle); }
	void ShowData(CParticle *pParticle);
	void ShowActivePage(void);

protected:
	
	CDocView* m_pParentView;
	CWnd* m_pParentWnd;
	CParticle *m_pParticle;			// current particle being edited
//	CParticle m_OriginalParticle;	// current particle before editing, for reverting

	/* property pages-- the dialogs that make up the property sheet */

	CDisplayPage	*m_pDisplayPage;
	CSizePage		*m_pSizePage;
	CMovementPage	*m_pMovementPage;
	CColorPage		*m_pColorPage;
	CCollidePage	*m_pCollidePage;
	CEnvelopePage	*m_pEnvelopePage;
	CLifetimePage	*m_pLifetimePage;
	CPiggybackPage	*m_pPiggybackPage;

	//{{AFX_MSG(CParticleSheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__PARTICLESHEET_H
