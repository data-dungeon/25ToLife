/***********************************/
/*                                 */
/* ImportDialog.cpp                */
/* Pigpen particle tool  12/22/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* dialog box for image import     */
/*                                 */
/***********************************/

/* after user has selected files to import, choose how to apply them */

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

BEGIN_MESSAGE_MAP(CImportDialog, CDialog)
	//{{AFX_MSG_MAP(CImportDialog)
	ON_BN_CLICKED(ID_IMPORT_ALL, OnImportAll)
	ON_BN_CLICKED(ID_IMPORT_SINGLE, OnImportSingle)
	ON_BN_CLICKED(IDC_REPLACE_CHECK, OnReplaceCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportDialog construction/destruction

CImportDialog::CImportDialog(CGraphic *pGraphic, CString& File, CString& WildcardFile, int nImageCount, bool bReplace, bool bUpdate) : CDialog(CImportDialog::IDD, NULL)
{
	//{{AFX_DATA_INIT(CImportDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	/* record graphic that is importing images for dialog initialization */

	ASSERT(pGraphic);
	m_pGraphic = pGraphic;
	m_File = File;
	m_WildcardFile = WildcardFile;
	m_nImageCount = nImageCount;
	m_nReplaceExisting = (bReplace) ? IMPORT_REPLACE_EXISTING : 0;	// may be forcing a replace
	m_bReplace = bReplace;
	m_bUpdate = bUpdate;
}

/* override for some dialog initialization */

BOOL CImportDialog::OnInitDialog()
{
	/* if importing graphic has no images, disable "delete existing images" check box */

	CButton *pButton = (CButton *) GetDlgItem(IDC_REPLACE_CHECK);
	pButton->EnableWindow(m_pGraphic->GetImageCount() != 0);

	/* if forcing a replace, disable "replace" button */

	if (m_bReplace)
	{
		pButton->SetCheck(TRUE);
		pButton->EnableWindow(FALSE);
	}

	/* if doing an update, disable "single" button. this is because import path string often */
	/* is a wildcard, so there is no single image that matches it, so nothing gets loaded in */

	if (m_bUpdate)
		GetDlgItem(ID_IMPORT_SINGLE)->EnableWindow(FALSE);
	
	/* fill in text */

	CStatic *pStatic;
	pStatic = (CStatic *) GetDlgItem(IDC_IMPORT_IMAGE_COUNT);
	pStatic->SetWindowText(ToCString(m_nImageCount));
	pStatic = (CStatic *) GetDlgItem(IDC_IMPORT_IMAGE_SIZE);
	pStatic->SetWindowText(ToCString(m_pGraphic->GetWidth()) + " x " + ToCString(m_pGraphic->GetHeight()));
	pStatic = (CStatic *) GetDlgItem(IDC_IMPORT_FILE);
	pStatic->SetWindowText(m_File);
	pStatic = (CStatic *) GetDlgItem(IDC_IMPORT_WILDCARD_FILE);
	pStatic->SetWindowText(m_WildcardFile);

	/* done */

	return(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CImportDialog operations

/////////////////////////////////////////////////////////////////////////////
// CImportDialog message handlers


void CImportDialog::OnImportAll() 
{
	EndDialog(IMPORT_ALL | m_nReplaceExisting);
}
void CImportDialog::OnImportSingle() 
{
	EndDialog(IMPORT_SINGLE | m_nReplaceExisting);
}
void CImportDialog::OnReplaceCheck() 
{
	m_nReplaceExisting ^= IMPORT_REPLACE_EXISTING;
}
