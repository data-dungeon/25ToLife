/***********************************/
/*                                 */
/* SurfaceListDialog.cpp           */
/* Pigpen particle tool  4/6/02    */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* dialog box for surface list     */
/*                                 */
/***********************************/

/* give the user a way to edit the surface type list-- list of surfaces for spawning */

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

/* dialog item lists */

static ts_EditBox EditBoxList[] = {
	//	control id						data id								 index	min		max				delta		format	bIsInteger
	
	// end of list-- must be last
	{	-1}
};

static ts_CheckBox CheckBoxList[] = {
	//	control id						data id							 index	flag bit						bShowData
	
	// end of list-- must be last
	{	-1}
};

static ts_RadioButton RadioButtonList[] = {
	//	control id						data id									value							mask						bShowData

	// end of list-- must be last
	{	-1}
};

/******************* externs *****************************/

/******************* class functions *********************/

/* i am trying to avoid long message maps, so i have overridden OnNotify and OnCommand. they catch slider bars,
spinners, edit boxes, and button clicks for me and process them more generically. the message map stuff still works,
but in general, use the higher-level intercept routines (like OnKillFocusEdit to catch all edit box commands) */

BEGIN_MESSAGE_MAP(CSurfaceListDialog, CDialog)
	//{{AFX_MSG_MAP(CSurfaceListDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSurfaceListDialog construction/destruction

CSurfaceListDialog::CSurfaceListDialog(CPigpenDoc *pDoc) : CDialog(CSurfaceListDialog::IDD, NULL)
{
	//{{AFX_DATA_INIT(CSurfaceListDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	/* record doc pointer so we have access to list */

	m_pDoc = pDoc;
}

/* override for some dialog initialization */

BOOL CSurfaceListDialog::OnInitDialog()
{
	ShowData();
	return(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CSurfaceListDialog operations

/* fill in the data in the dialog box */

void CSurfaceListDialog::ShowData(void)
{
	/* surface combo box has list of available surface types to define behavior for */

	CComboBox *pSurfaceComboBox = (CComboBox *) GetDlgItem(IDC_SURFACE_LIST_COMBO);
	pSurfaceComboBox->ResetContent();	// clear contents
	for (int i = 0; i < m_pDoc->GetSurfaceCount(); i++)
		pSurfaceComboBox->AddString(m_pDoc->GetSurface(i)->GetName());
	pSurfaceComboBox->SetCurSel(m_pDoc->GetSurfaceIndex());

	/* set contents of edit boxes */

	ts_EditBox *pEditBox = EditBoxList;
	while (pEditBox->nID != -1)	// id == -1 marks end of list
	{
		CEdit *pEdit = (CEdit *) GetDlgItem(pEditBox->nID);
		ASSERT(pEdit);		// if this hits you have an entry in a dialog box table that isn't in this dialog page
		pEdit->EnableWindow(TRUE);
		UpdateEditBox(pEditBox->nID, 0, NULL, FALSE);

/************** special cases ***********************/
		/* special cases for disabling a box based on data state */

//		switch(pEditBox->nID)
//		{
//		}
/************** end special cases *******************/
		pEditBox++;
	}

	/* set contents of checkboxes */

	ts_CheckBox *pCheckBox = CheckBoxList;
	while (pCheckBox->nID != -1)	// id == -1 marks end of list
	{
		CButton *pButton = (CButton *) GetDlgItem(pCheckBox->nID);
		ASSERT(pButton);	// if this hits you have an entry in a dialog box table that isn't in this dialog page
		pButton->EnableWindow(TRUE);
		UpdateCheckBox(pCheckBox->nID, FALSE);

/************** special cases ***********************/
		/* special cases for disabling a box based on data state */

//		switch(pCheckBox->nID)
//		{
//		}
/************** end special cases *******************/
		pCheckBox++;
	}

	/* set contents of radio buttons */

	ts_RadioButton *pRadioButton = RadioButtonList;
	while (pRadioButton->nID != -1)	// id == -1 marks end of list
	{
		CButton *pButton = (CButton *) GetDlgItem(pRadioButton->nID);
		ASSERT(pButton);	// if this hits you have an entry in a dialog box table that isn't in this dialog page
		pButton->EnableWindow(TRUE);
		UpdateRadioButton(pRadioButton->nID, FALSE);

/************** special cases ***********************/
		/* special cases for disabling a box based on data state */

//		switch(pRadioButton->nID)
//		{
//		}
/************** end special cases *******************/
		pRadioButton++;
	}

	/* enable/disable normal buttons */

	GetDlgItem(IDC_DELETE_SURFACE)->EnableWindow(m_pDoc->CurrentSurface()->Predefined() == false);
}

/* update edit box's value. if pfDirectValue is non-NULL, use value directly, ignore delta. for easier
implementation i am using floats for everything that goes in an edit box, even if they will ultimately be ints */

void CSurfaceListDialog::UpdateEditBox(int nID, int nDelta, float *pfDirectValue, bool bCheckSpecialCases)
{
}

/* update check box's state. check/uncheck depending on proper bit's state in proper flag unsigned long word */

void CSurfaceListDialog::UpdateCheckBox(int nID, bool bToggle)
{
}

/* update radio button's. check/uncheck depending on if bitfield value matches radio button's assigned value */

void CSurfaceListDialog::UpdateRadioButton(int nID, bool bUpdate)
{
}

/////////////////////////////////////////////////////////////////////////////
// CSurfaceListDialog message handlers

/* intercept spin control messages to update associated edit box value */

bool CSurfaceListDialog::OnSpinner(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/* find id of edit box associated with this spinner */

	CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*) GetDlgItem(((NM_UPDOWN*) pNMHDR)->hdr.idFrom);
	ASSERT(pSpin);
	CEdit *pEdit = (CEdit *) pSpin->GetBuddy();
	ASSERT(pEdit);
	int nID = pEdit->GetDlgCtrlID();

	/* update that edit box */

	UpdateEditBox(nID, ((NM_UPDOWN*) pNMHDR)->iDelta, NULL, TRUE);

	/* done */

	*pResult = 0;
	return(TRUE);	// processed message
}

/* catch user editing box directly */

bool CSurfaceListDialog::OnKillFocusEdit(int nID) 
{
	CEdit *pEdit = (CEdit *) GetDlgItem(nID);

	/* get input string, convert to number */

	CString String;
	pEdit->GetWindowText(String);
	float fValue = (float) atof(String);

	/* update edit box. check for invalid string */

	float *pfValue = &fValue;
	if ((fValue == 0.0) && (AllZeros(String) == FALSE))
		pfValue = NULL;	// illegal entry, revert back to value
	UpdateEditBox(nID, 0, pfValue, TRUE);
	return(TRUE);	// processed message
}

/* change selected effect to spawn for particle */

bool CSurfaceListDialog::OnSelChange(int nID)
{
	/* get new selection from combo box */

	CComboBox* pComboBox = (CComboBox*) GetDlgItem(nID);
	ASSERT(pComboBox);
	int nListIndex = pComboBox->GetCurSel();
	switch(nID)
	{
	case IDC_SURFACE_LIST_COMBO:
		m_pDoc->SetSurfaceIndex(nListIndex);
		ShowData();	// may enable/disable fields
		break;
	}
	return(TRUE);	// processed message
}

/* catch button clicks. this routine does not have to handle all button clicks, but needs to return FALSE if
it gets passed a button it doesn't understand or want to deal with */

bool CSurfaceListDialog::OnButtonClicked(int nID)
{
	/* normal buttons */

	switch(nID)
	{
	case IDC_DELETE_SURFACE:
		ASSERT(m_pDoc->CurrentSurface()->Predefined() == false);	// can't delete pre-defined surface types
		m_pDoc->DeleteSurface();
		ShowData();
		return(TRUE);	// processed message
	case IDC_ADD_SURFACE:
		/* get contents of edit box-- that's the new name */

		CEdit *pEdit = (CEdit *) GetDlgItem(IDC_SURFACE_NAME_EDIT);
		CString String;
		pEdit->GetWindowText(String);
		pEdit->SetWindowText("");	// and clear box

		/* empty or all whitespace names not allowed */

		String.TrimLeft();
		String.TrimRight();
		if (String == "")
			return(TRUE);	// processed message

		/* if already exists in list, make it current selection so user thinks something happened */

		int nIndex = m_pDoc->SurfaceExists(String);
		if (nIndex != -1)
		{
			m_pDoc->SetSurfaceIndex(nIndex);
			ShowData();
			return(TRUE);	// processed message
		}

		/* add a new surface type */

		CSurface *pSurface = new CSurface;
		pSurface->SetName(String.GetBuffer(10));
		m_pDoc->AddSurface(pSurface);
		m_pDoc->SetSurfaceIndex(m_pDoc->GetSurfaceCount() - 1);
		ShowData();
		return(TRUE);	// processed message
	}

	/* check boxes can all be handled by the same routine */

	ts_CheckBox *pCheckBox = CheckBoxList;
	while (pCheckBox->nID != -1)	// id == -1 marks end of list
	{
		if (nID == pCheckBox->nID)
		{
			UpdateCheckBox(nID, TRUE);

/************** special cases ***********************/
			/* check for additional actions when changing a check box */

//			switch(nID)
//			{
//			}
/************** end special cases *******************/

			/* need to re-show data? this is useful for enabling/disabling buttons after check box state changes */

			if (pCheckBox->bShowData)
				ShowData();

			return(TRUE);	// processed message
		}
		pCheckBox++;
	}

	/* radio buttons can all be handled by the same routine */

	ts_RadioButton *pRadioButton = RadioButtonList;
	while (pRadioButton->nID != -1)	// id == -1 marks end of list
	{
		if (nID == pRadioButton->nID)
		{
			UpdateRadioButton(nID, TRUE);

/************** special cases ***********************/
			/* check for additional actions when changing a check box */

//			switch(nID)
//			{
//			}
/************** end special cases *******************/

			/* need to re-show data? this is useful for enabling/disabling buttons after button state changes */

			if (pRadioButton->bShowData)
				ShowData();

			return(TRUE);	// processed message
		}
		pRadioButton++;
	}

	/* didn't handle the button */

	return(FALSE);
}

/* handle messages from dialog controls. this is intended to be a more generic approach than having every control
have to show up in the message map */

BOOL CSurfaceListDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	/* if wParam != 0, wParam is ID of control that sent message */

	if (wParam != 0)
	{
		NMHDR *pNMHDR = (NMHDR *) lParam;
		switch(pNMHDR->code)
		{
		case UDN_DELTAPOS:
			/* spinner */

			if (OnSpinner(pNMHDR, pResult) == TRUE)
				return(TRUE);	// processed message
//not needed for this dialog at this point
//		case NM_CUSTOMDRAW:
//			/* slider */
//
//			if (OnCustomdrawSlider(pNMHDR, pResult) == TRUE)
//				return(TRUE);	// processed message
		}
	}

	/* let base class handle anything else */

	return CDialog::OnNotify(wParam, lParam, pResult);
}

/* this catches other control messages. from the help on CWnd::OnCommand, about wParam:

"The low-order word of wParam identifies the command ID of the menu item, control, or accelerator.
The high-order word of wParam specifies the notification message if the message is from a control.
If the message is from an accelerator, the high-order word is 1. If the message is from a menu,
the high-order word is 0." */

BOOL CSurfaceListDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch((wParam >> 16) & 0xffff)
	{
	case EN_KILLFOCUS:
		/* user has finished editing text in an edit box */

		if (OnKillFocusEdit(wParam & 0xffff) == TRUE)
			return(TRUE);	// processed message
		break;
	case CBN_SELCHANGE:
		/* user has selected a new entry in combo box */

		if (OnSelChange(wParam & 0xffff) == TRUE)
			return(TRUE);	// processed message
		break;
	case BN_CLICKED:
		/* user has clicked a button. check boxes are buttons, too */

		if (OnButtonClicked(wParam & 0xffff) == TRUE)
			return(TRUE);	// processed message
		break;
	}

	/* let base class handle anything else */

	return CDialog::OnCommand(wParam, lParam);
}

