/***********************************/
/*                                 */
/* SwooshSheet.cpp                 */
/* Pigpen particle tool  10/29/02  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* property sheet to edit swooshes */
/*                                 */
/***********************************/

/* this is the property sheet that let's the user edit a swoosh. property sheets are tabbed dialog
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

BEGIN_MESSAGE_MAP(CSwooshSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CSwooshSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSwooshSheet construction/destruction

CSwooshSheet::CSwooshSheet(CDocView *pParentView, CWnd* pParentWnd) : CPropertySheet()
{
	//{{AFX_DATA_INIT(CSwooshSheet)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	ASSERT(pParentView != NULL);
	ASSERT(pParentWnd != NULL);

	m_pParentView = pParentView;
	m_pParentWnd = pParentWnd;
	m_pSwoosh = NULL;

	/* add pages to property sheet */

	m_pDisplayPage = new CSwooshDisplayPage(pParentView, this);
	AddPage(m_pDisplayPage);
	m_pColorPage = new CColorPage(pParentView, NULL, this);
	AddPage(m_pColorPage);
	m_pEnvelopePage = new CEnvelopePage(pParentView, NULL, this);
	AddPage(m_pEnvelopePage);
//	m_pLifetimePage = new CLifetimePage(pParentView, this);
//	AddPage(m_pLifetimePage);
}

/* notify parent view that edit window is gone */

CSwooshSheet::~CSwooshSheet()
{
	delete m_pDisplayPage;
	delete m_pColorPage;
//	delete m_pEnvelopePage;	//yuch-- this causes a crash when exiting when uncommented!!!
//	delete m_pLifetimePage;
	m_pParentView->EditWindowClosed(EDIT_WINDOW0);
}

/* create a modeless dialog */

BOOL CSwooshSheet::Create()
{
	/* we want context help-- the question mark on the title bar */

	return(CPropertySheet::Create(m_pParentWnd, -1, WS_EX_CONTEXTHELP));
}

/////////////////////////////////////////////////////////////////////////////
// CSwooshSheet operations

/* fill in the data in the dialog box with the current swoosh */

void CSwooshSheet::ShowData(CSwoosh *pSwoosh)
{
	/* record pointer and contents */

	m_pSwoosh = pSwoosh;
//yuch-- not being used, and just causes destructor problems since it is a copy of an existing swoosh
//	if (m_pSwoosh)
//		m_OriginalSwoosh = *pSwoosh;

	/* title bar is name of current swoosh */

	if (m_pSwoosh == NULL)
		SetWindowText("Swoosh: NONE SELECTED");
	else
		SetWindowText("Swoosh: " + m_pSwoosh->GetName());

	/* show active page */

	ShowActivePage();
}

/* show data for the active property page */

void CSwooshSheet::ShowActivePage(void)
{
	CPropertyPage *pPage = GetActivePage();
	if (pPage == m_pDisplayPage)
		m_pDisplayPage->ShowData();
	else if (pPage == m_pColorPage)
		m_pColorPage->ShowData();
	else if (pPage == m_pEnvelopePage)
		m_pEnvelopePage->ShowData();
//	else if (pPage == m_pLifetimePage)
//		m_pLifetimePage->ShowData();
	else
		ASSERT(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CSwooshSheet message handlers

/* called after window is destroyed */

void CSwooshSheet::PostNcDestroy()
{
	delete this;	// dialog was created by parent window using "new"
}

