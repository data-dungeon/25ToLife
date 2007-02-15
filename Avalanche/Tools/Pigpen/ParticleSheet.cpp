/***********************************/
/*                                 */
/* ParticleSheet.cpp               */
/* Pigpen particle tool  12/10/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* property sheet to edit particles*/
/*                                 */
/***********************************/

/* this is the property sheet that let's the user edit a particle particle. property sheets are tabbed dialog
boxes-- that is, they are a bunch of dialog boxes collected into a single window. the modules PageXXX.cpp
run the individual dialog boxes that make up the property sheet */

/******************* includes ****************************/

/*** pre-compiled headers through .h file (visual studio), can't be inside #if statement. ***/
/*** see .h file for more info.for codewarrior, pre-compiled header is handled through    ***/
/*** xcl compile/link interpreter, and doesn't need any explicit include                  ***/

#include <PigpenPCH.h>

/*** includes that don't belong in pre-compiled header (typically due to low frequency) ***/



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/******************* defines *****************************/

/******************* structures **************************/

/******************* local variables *********************/

/******************* externs *****************************/

/******************* class functions *********************/

BEGIN_MESSAGE_MAP(CParticleSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CParticleSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParticleSheet construction/destruction

CParticleSheet::CParticleSheet(CDocView *pParentView, CWnd* pParentWnd) : CPropertySheet()
{
	//{{AFX_DATA_INIT(CParticleSheet)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	ASSERT(pParentView != NULL);
	ASSERT(pParentWnd != NULL);

	m_pParentView = pParentView;
	m_pParentWnd = pParentWnd;
	m_pParticle = NULL;

	/* add pages to property sheet */

	m_pDisplayPage = new CDisplayPage(pParentView, this);
	AddPage(m_pDisplayPage);
	m_pSizePage = new CSizePage(pParentView, this);
	AddPage(m_pSizePage);
	m_pMovementPage = new CMovementPage(pParentView, this);
	AddPage(m_pMovementPage);
	m_pColorPage = new CColorPage(pParentView, this, NULL);
	AddPage(m_pColorPage);
	m_pCollidePage = new CCollidePage(pParentView, this);
	AddPage(m_pCollidePage);
	m_pEnvelopePage = new CEnvelopePage(pParentView, this, NULL);
	AddPage(m_pEnvelopePage);
	m_pLifetimePage = new CLifetimePage(pParentView, this);
	AddPage(m_pLifetimePage);
	m_pPiggybackPage = new CPiggybackPage(pParentView, this);
	AddPage(m_pPiggybackPage);
}

/* notify parent view that edit window is gone */

CParticleSheet::~CParticleSheet()
{
	delete m_pDisplayPage;
	delete m_pSizePage;
	delete m_pMovementPage;
	delete m_pColorPage;
	delete m_pCollidePage;
//	delete m_pEnvelopePage;	//yuch-- this causes a crash when exiting when uncommented!!!
	delete m_pLifetimePage;
	delete m_pPiggybackPage;
	m_pParentView->EditWindowClosed(EDIT_WINDOW0);
}

/* create a modeless dialog */

BOOL CParticleSheet::Create()
{
	/* we want context help-- the question mark on the title bar */

	return(CPropertySheet::Create(m_pParentWnd, -1, WS_EX_CONTEXTHELP));
}

/////////////////////////////////////////////////////////////////////////////
// CParticleSheet operations

/* fill in the data in the dialog box with the current particle */

void CParticleSheet::ShowData(CParticle *pParticle)
{
	/* record pointer and contents */

	m_pParticle = pParticle;
//yuch-- not being used, and just causes destructor problems since it is a copy of an existing particle
//	if (m_pParticle)
//		m_OriginalParticle = *pParticle;

	/* title bar is name of current particle */

	if (m_pParticle == NULL)
		SetWindowText("Particle: NONE SELECTED");
	else
		SetWindowText("Particle: " + m_pParticle->GetName());

	/* show active page */

	ShowActivePage();
}

/* show data for the active property page */

void CParticleSheet::ShowActivePage(void)
{
	CPropertyPage *pPage = GetActivePage();
	if (pPage == m_pDisplayPage)
		m_pDisplayPage->ShowData();
	else if (pPage == m_pSizePage)
		m_pSizePage->ShowData();
	else if (pPage == m_pMovementPage)
		m_pMovementPage->ShowData();
	else if (pPage == m_pColorPage)
		m_pColorPage->ShowData();
	else if (pPage == m_pCollidePage)
		m_pCollidePage->ShowData();
	else if (pPage == m_pEnvelopePage)
		m_pEnvelopePage->ShowData();
	else if (pPage == m_pLifetimePage)
		m_pLifetimePage->ShowData();
	else if (pPage == m_pPiggybackPage)
		m_pPiggybackPage->ShowData();
	else
		ASSERT(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CParticleSheet message handlers

/* called after window is destroyed */

void CParticleSheet::PostNcDestroy()
{
	delete this;	// dialog was created by parent window using "new"
}

