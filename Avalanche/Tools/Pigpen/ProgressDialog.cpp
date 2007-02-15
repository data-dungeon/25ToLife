/***********************************/
/*                                 */
/* ProgressDialog.cpp              */
/* Pigpen particle tool  2/3/01    */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* simple progress dialog          */
/*                                 */
/***********************************/

/* for use during exporting, mainly */

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

BEGIN_MESSAGE_MAP(CProgressDialog, CDialog)
	//{{AFX_MSG_MAP(CProgressDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressDialog construction/destruction

CProgressDialog::CProgressDialog(CString Title, CString File, int nTotalItems) : CDialog(CProgressDialog::IDD, NULL)
{
	//{{AFX_DATA_INIT(CProgressDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	/* record graphic that is importing images for dialog initialization */

	m_Title = Title;
	m_File = File;
	m_nTotalItems = nTotalItems;
	m_nItems = 0;
}

/* override for some dialog initialization */

BOOL CProgressDialog::OnInitDialog()
{
	SetWindowText(m_Title.GetBuffer(10));
	GetDlgItem(IDC_PROGRESS_FILE)->SetWindowText(m_File.GetBuffer(10));
	GetDlgItem(IDC_PROGRESS_INFO)->SetWindowText("");

	/* done */

	return(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CProgressDialog operations

/* update the progress bar */

void CProgressDialog::UpdateProgress(void)
{
	m_nItems++;
	ASSERT(m_nItems <= m_nTotalItems);
	if (m_nItems > m_nTotalItems)
		m_nItems = m_nTotalItems;
	((CProgressCtrl *) GetDlgItem(IDC_PROGRESS_BAR))->SetPos(m_nItems * 100 / m_nTotalItems);
}

/* update the info field */

void CProgressDialog::UpdateInfo(char *pString)
{
	GetDlgItem(IDC_PROGRESS_INFO)->SetWindowText(pString);
}

/////////////////////////////////////////////////////////////////////////////
// CProgressDialog message handlers


