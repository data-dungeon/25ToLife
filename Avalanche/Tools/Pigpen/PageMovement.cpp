/***********************************/
/*                                 */
/* PageMovment.cpp                 */
/* Pigpen particle tool  12/28/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle movement property page */
/*                                 */
/***********************************/

/* this is movement page of the particle property sheet. each page is a dialog */

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
	//	control id								data id								index		min		max			delta		format	bIsInteger
	{	IDC_PART_XPOS_EDIT,					CParticle::DATA_XPOS,			-1,		-500.0f,	500.0f,		0.2f,		"%.2f",	FALSE},
	{	IDC_PART_YPOS_EDIT,					CParticle::DATA_YPOS,			-1,		-500.0f,	500.0f,		0.2f,		"%.2f",	FALSE},
	{	IDC_PART_ZPOS_EDIT,					CParticle::DATA_ZPOS,			-1,		-500.0f,	500.0f,		0.2f,		"%.2f",	FALSE},
	{	IDC_PART_POS_RAND_EDIT,				CParticle::DATA_POS_RANDOM,	-1,		0.0f,		10.0f,		0.05f,	"%.2f",	FALSE},
	{	IDC_PART_XVEL_EDIT,					CParticle::DATA_XVEL,			-1,		-500.0f,	500.0f,		0.2f,		"%.2f",	FALSE},
	{	IDC_PART_YVEL_EDIT,					CParticle::DATA_YVEL,			-1,		-500.0f,	500.0f,		0.2f,		"%.2f",	FALSE},
	{	IDC_PART_ZVEL_EDIT,					CParticle::DATA_ZVEL,			-1,		-500.0f,	500.0f,		0.2f,		"%.2f",	FALSE},
	{	IDC_PART_VEL_RAND_EDIT,				CParticle::DATA_VEL_RANDOM,	-1,		0.0f,		100.0f,		1.0f,		"%.0f",	TRUE},
	{	IDC_PART_VEL_CONE_EDIT,				CParticle::DATA_VEL_CONE1,		-1,		0.0f,		180.0f,		1.0f,		"%.0f",	TRUE},
	{	IDC_PART_VEL_CONE2_EDIT,			CParticle::DATA_VEL_CONE2,		-1,		0.0f,		180.0f,		1.0f,		"%.0f",	TRUE},
	{	IDC_PART_GRAVITY_EDIT,				CParticle::DATA_GRAVITY,		-1,		-50.0f,	50.0f,		0.2f,		"%.2f",	FALSE},
	{	IDC_PART_ACCEL_EDIT,					CParticle::DATA_ACCEL,			-1,		-500.0f,	500.0f,		0.2f,		"%.2f",	FALSE},
	{	IDC_PART_XROT_EDIT,					CParticle::DATA_XROT_VEL,		-1,		-999.0f,	999.0f,		1.0f,		"%.2f",	FALSE},
	{	IDC_PART_YROT_EDIT,					CParticle::DATA_YROT_VEL,		-1,		-999.0f,	999.0f,		1.0f,		"%.2f",	FALSE},
	{	IDC_PART_ZROT_EDIT,					CParticle::DATA_ZROT_VEL,		-1,		-999.0f,	999.0f,		1.0f,		"%.2f",	FALSE},
	{	IDC_PART_ROT_RAND_EDIT,				CParticle::DATA_ROT_VEL_RANDOM,	-1,	-1.0f,	100.0f,		1.0f,		"%.0f",	TRUE},
	{	IDC_PART_ROT_ACCEL_EDIT,			CParticle::DATA_ROT_ACCEL,		-1,		-999.0f,	999.0f,		1.0f,		"%.2f",	FALSE},
	
	// end of list-- must be last
	{	-1}
};

static ts_CheckBox CheckBoxList[] = {
	//	control id							data id						 index	flag bit									bShowData
	{	IDC_RANDOM_POS_PLANE_CHECK,	CParticle::DATA_FLAGS,		-1,	PART_RANDOM_POS_PLANE,				false},
	{	IDC_BULLET_CHECK,					CParticle::DATA_FLAGS,		-1,	PART_BULLET,							true},
	{	IDC_STANDARD_GRAVITY_CHECK,	CParticle::DATA_FLAGS,		-1,	PART_STANDARD_GRAVITY,				false},
	{	IDC_ALIGN_TO_VEL_CHECK,			CParticle::DATA_FLAGS,		-1,	PART_ALIGN_TO_VELOCITY,				true},
	{	IDC_PERPENDICULAR_CHECK,		CParticle::DATA_FLAGS,		-1,	PART_PERPENDICULAR_TO_VELOCITY,	false},
	{	IDC_INIT_ROT_RAND_CHECK,		CParticle::DATA_FLAGS,		-1,	PART_INIT_ROT_RANDOM,				false},
	
	// end of list-- must be last
	{	-1}
};

static ts_RadioButton RadioButtonList[] = {
	//	control id						data id							value								mask			bShowData

	// end of list-- must be last
	{	-1}
};

/******************* externs *****************************/

/******************* class functions *********************/

/* i am trying to avoid long message maps, so i have overridden OnNotify and OnCommand. they catch slider bars,
spinners, edit boxes, and button clicks for me and process them more generically. the message map stuff still works,
but in general, use the higher-level intercept routines (like OnKillFocusEdit to catch all edit box commands) */

BEGIN_MESSAGE_MAP(CMovementPage, CPropertyPage)
	//{{AFX_MSG_MAP(CMovementPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMovementPage construction/destruction

CMovementPage::CMovementPage(CDocView *pParentView, CParticleSheet *pParentSheet) : CPropertyPage(CMovementPage::IDD)
{
	m_pParentView = pParentView;
	m_pParentSheet = pParentSheet;
	m_pParticle = NULL;

	/* look for operator error-- i'm looking for overlap in bitfield masks */

#ifdef _DEBUG
	ts_RadioButton *pRadioButton = RadioButtonList;
	int nLastDataID = -1;
	unsigned long ulLastMask = 0xffffffff;
	unsigned long ulOrBitfield;
	unsigned long ulXorBitfield;
	while (pRadioButton->nID != -1)	// id == -1 marks end of list
	{
		if (pRadioButton->nDataID != nLastDataID)	// in case there is more than one bitfield
		{
			nLastDataID = pRadioButton->nDataID;
			ulOrBitfield = 0;
			ulXorBitfield = 0;
		}
		if (pRadioButton->ulMask != ulLastMask)
		{
			ulLastMask = pRadioButton->ulMask;
			ulOrBitfield |= pRadioButton->ulMask;
			ulXorBitfield ^= pRadioButton->ulMask;

			/* if this assert hits, then a couple of things could be wrong. first, you may have defined */
			/* bitfield masks that overlap-- for example, if one mask is 0x03 and another is 0x06, then */
			/* 0x03 | 0x06 = 0x07, but 0x03 ^ 0x06 = 0x05-- they share bit 1, which is a problem. bitfield */
			/* masks should not overlap. a second, more innocent, cause for this assert is that you have the */
			/* same mask separated by a different mask value: for example, */

//	//	control id						data id							value								mask
//	{	IDC_ORIENT_SCREEN_RADIO,	CParticle::DATA_BITFIELDS,	PARTICLE_ORIENT_SCREEN,		PARTICLE_ORIENT_MASK},
//	{	IDC_DOUBLE_SIDED,				CParticle::DATA_BITFIELDS,	PARTICLE_DOUBLE_SIDED,		PARTICLE_SIDED_MASK},
//	{	IDC_ORIENT_GROUND_RADIO,	CParticle::DATA_BITFIELDS,	PARTICLE_ORIENT_GROUND,		PARTICLE_ORIENT_MASK},

			/* this would cause the assert because PARTICLE_ORIENT_MASK would be tested twice. please group */
			/* all mask references to avoid this problem */

			ASSERT(ulOrBitfield == ulXorBitfield);
		}
		pRadioButton++;
	}
#endif //_DEBUG

	//{{AFX_DATA_INIT(CMovementPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CMovementPage operations

/* fill in the data in the dialog box with the current particle */

void CMovementPage::ShowData(void)
{
	/* get current particle from parent sheet */

	m_pParticle = m_pParentSheet->GetParticle();

	/* set contents of edit boxes */

	ts_EditBox *pEditBox = EditBoxList;
	while (pEditBox->nID != -1)	// id == -1 marks end of list
	{
		CEdit *pEdit = (CEdit *) GetDlgItem(pEditBox->nID);
		ASSERT(pEdit);		// if this hits you have an entry in a dialog box table that isn't in this dialog page
		if (m_pParticle == NULL)
		{
			pEdit->SetWindowText("N/A");
			pEdit->EnableWindow(FALSE);
		}
		else
		{
			pEdit->EnableWindow(TRUE);
			UpdateEditBox(pEditBox->nID, 0, NULL, FALSE);

/************** special cases ***********************/
			/* special cases for disabling a box based on data state */

			switch(pEditBox->nID)
			{
			case IDC_PART_XVEL_EDIT:
			case IDC_PART_YVEL_EDIT:
				if (m_pParticle->IsBullet())
					pEdit->EnableWindow(FALSE);	// x- and y-vel disabled for bullets
				break;
			case IDC_PART_XROT_EDIT:
				if ((m_pParticle->GetOrient() == PART_ORIENT_SCREEN) ||
					 (m_pParticle->GetOrient() == PART_ORIENT_GROUND) ||
					 (m_pParticle->IsAlignToVelocity()))
					pEdit->EnableWindow(FALSE);
				break;
			case IDC_PART_YROT_EDIT:
				if ((m_pParticle->GetOrient() == PART_ORIENT_SCREEN) ||
					 (m_pParticle->IsAlignToVelocity()))
					pEdit->EnableWindow(FALSE);
				break;
			case IDC_PART_ZROT_EDIT:
				if ((m_pParticle->GetOrient() == PART_ORIENT_GROUND) ||
					 (m_pParticle->IsAlignToVelocity()))
					pEdit->EnableWindow(FALSE);
				break;
			case IDC_PART_ROT_RAND_EDIT:
			case IDC_PART_ROT_ACCEL_EDIT:
				if (m_pParticle->IsAlignToVelocity())
					pEdit->EnableWindow(FALSE);
				break;
			}
/************** end special cases *******************/
		}
		pEditBox++;
	}

	/* set contents of checkboxes */

	ts_CheckBox *pCheckBox = CheckBoxList;
	while (pCheckBox->nID != -1)	// id == -1 marks end of list
	{
		CButton *pButton = (CButton *) GetDlgItem(pCheckBox->nID);
		ASSERT(pButton);	// if this hits you have an entry in a dialog box table that isn't in this dialog page
		if (m_pParticle == NULL)
		{
			pButton->SetCheck(0);
			pButton->EnableWindow(FALSE);
		}
		else
		{
			pButton->EnableWindow(TRUE);
			UpdateCheckBox(pCheckBox->nID, FALSE);

/************** special cases ***********************/
			/* special cases for disabling a box based on data state */

			switch(pCheckBox->nID)
			{
			case IDC_INIT_ROT_RAND_CHECK:
				if (m_pParticle->IsAlignToVelocity())
					pButton->EnableWindow(FALSE);
				break;
			case IDC_PERPENDICULAR_CHECK:
				if (m_pParticle->IsAlignToVelocity() == false)
					pButton->EnableWindow(false);
				break;
			case IDC_ALIGN_TO_VEL_CHECK:
				if (m_pParticle->IsFlagSet(PART_FLOATS_ON_WATER))
					pButton->EnableWindow(false);
			}
/************** end special cases *******************/
		}
		pCheckBox++;
	}

#if 0	// no radio buttons currently
	/* set contents of radio buttons */

	ts_RadioButton *pRadioButton = RadioButtonList;
	while (pRadioButton->nID != -1)	// id == -1 marks end of list
	{
		CButton *pButton = (CButton *) GetDlgItem(pRadioButton->nID);
		ASSERT(pButton);	// if this hits you have an entry in a dialog box table that isn't in this dialog page
		if (m_pParticle == NULL)
		{
			pButton->SetCheck(0);
			pButton->EnableWindow(FALSE);
		}
		else
		{
			pButton->EnableWindow(TRUE);
			UpdateRadioButton(pRadioButton->nID, FALSE);

/************** special cases ***********************/
			/* special cases for disabling a box based on data state */

/************** end special cases *******************/
		}
		pRadioButton++;
	}
#endif //0

	/* enable/disable normal buttons */

	if (m_pParticle == NULL)
		GetDlgItem(IDC_PREVIEW_PARTICLE)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_PREVIEW_PARTICLE)->EnableWindow(TRUE);
}

/* update edit box's value. if pfDirectValue is non-NULL, use value directly, ignore delta. for easier
implementation i am using floats for everything that goes in an edit box, even if they will ultimately be ints */

void CMovementPage::UpdateEditBox(int nID, int nDelta, float *pfDirectValue, bool bCheckSpecialCases)
{
	float *pfValue = NULL;
	float fOldValue;
	char acString[20];

	/* should be working on valid data */

	ASSERT(m_pParticle != NULL);

	/* find correct entry */

	ts_EditBox *pEditBox = EditBoxList;
	while ((pEditBox->nID != nID) && (pEditBox->nID != -1))	// id == -1 marks end of list
		pEditBox++;
	if (pEditBox->nID == -1)
	{
		ASSERT(FALSE);	// should always find it, otherwise edit box won't be supported
		return;
	}

	/* get pointer to data item */

	pfValue = (float *) m_pParticle->GetDataPointer(pEditBox->nDataID);
	ASSERT(pfValue);

	/* adjust value, check for setting value directly */

	fOldValue = *pfValue;
	if (pfDirectValue != NULL)
	{
		ASSERT(nDelta == 0);
		*pfValue = *pfDirectValue;
	}
	else
		*pfValue += pEditBox->fDeltaMultiplier * -nDelta;	// passed delta is opposite sign of what i would expect

	/* data that will ultimately be output as integers is stored internally as float for easier processing. */
	/* but make sure that display matches eventual output value */

	if (pEditBox->bIsInteger)
		*pfValue = (float) ((int) *pfValue);

	/* check min/max, update box */

	if (*pfValue < pEditBox->fMinValue)
		*pfValue = pEditBox->fMinValue;
	if (*pfValue > pEditBox->fMaxValue)
		*pfValue = pEditBox->fMaxValue;
	CEdit *pEdit = (CEdit *) GetDlgItem(nID);
	sprintf(acString, pEditBox->acFormat, *pfValue);
	pEdit->SetWindowText(acString);

	/* check for additional actions when changing an edit box */

	if (bCheckSpecialCases)
	{
/************** special cases ***********************/
		switch(pEditBox->nID)
		{
		case IDC_PART_VEL_CONE_EDIT:
			/* vel cone2 is always >= than vel cone1 */

			if (m_pParticle->CheckVelCone2())
				ShowData();
			break;
		case IDC_PART_VEL_CONE2_EDIT:
			/* vel cone1 is always <= than vel cone2 */

			if (m_pParticle->CheckVelCone1())
				ShowData();
			break;
		}
/************** end special cases *******************/
	}

	/* has doc changed? */

	if (*pfValue != fOldValue)
	{
		CPigpenDoc *pDoc = m_pParentView->GetDocument();
		pDoc->SetModifiedFlag(MODIFIED_FLAG);
		Preview__UpdateParticle(m_pParticle);
	}
}

/* update check box's state. check/uncheck depending on proper bit's state in proper flag unsigned long word */

void CMovementPage::UpdateCheckBox(int nID, bool bToggle)
{
	unsigned long *pulFlags = NULL;

	/* should be working on valid data */

	ASSERT(m_pParticle != NULL);

	/* find correct entry */

	ts_CheckBox *pCheckBox = CheckBoxList;
	while ((pCheckBox->nID != nID) && (pCheckBox->nID != -1))	// id == -1 marks end of list
		pCheckBox++;
	if (pCheckBox->nID == -1)
	{
		ASSERT(FALSE);	// should always find it, otherwise edit box won't be supported
		return;
	}

	/* get pointer to data item */

	pulFlags = (unsigned long *) m_pParticle->GetDataPointer(pCheckBox->nDataID);
	ASSERT(pulFlags);

	/* toggle if requested */

	if (bToggle)
		*pulFlags ^= pCheckBox->ulFlagBit;

	/* show state of flag bit */

	CButton *pButton = (CButton *) GetDlgItem(nID);
	pButton->SetCheck((*pulFlags & pCheckBox->ulFlagBit) ? TRUE : FALSE);

	/* has doc changed? */

	if (bToggle)
	{
		CPigpenDoc *pDoc = m_pParentView->GetDocument();
		pDoc->SetModifiedFlag(MODIFIED_FLAG);
		Preview__UpdateParticle(m_pParticle);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMovementPage message handlers

/* intercept spin control messages to update associated edit box value */

bool CMovementPage::OnSpinner(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/* not if no particle */

	if (m_pParticle == NULL)
	{
		*pResult = 1;	// non-zero means abort further processing
		return(FALSE);
	}

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

bool CMovementPage::OnKillFocusEdit(int nID) 
{
	CEdit *pEdit = (CEdit *) GetDlgItem(nID);
	if (m_pParticle == NULL)
	{
		pEdit->SetWindowText("N/A");
		pEdit->EnableWindow(FALSE);
	}
	else
	{
		/* get input string, convert to number */

		CString String;
		pEdit->GetWindowText(String);
		float fValue = (float) atof(String);

		/* update edit box. check for invalid string */

		float *pfValue = &fValue;
		if ((fValue == 0.0) && (AllZeros(String) == FALSE))
			pfValue = NULL;	// illegal entry, revert back to value
		UpdateEditBox(nID, 0, pfValue, TRUE);
	}
	return(TRUE);	// processed message
}

/* catch button clicks. this routine does not have to handle all button clicks, but needs to return FALSE if
it gets passed a button it doesn't understand or want to deal with */

bool CMovementPage::OnButtonClicked(int nID)
{
	/* normal buttons */

	switch(nID)
	{
	case IDC_PREVIEW_PARTICLE:
		Preview__Create(m_pParentView->GetDocument(), NULL, NULL, m_pParticle, NULL, NULL);
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

#if 0 //not needed for this dialog at this point
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
#endif //0

	/* didn't handle the button */

	return(FALSE);
}

/* handle messages from dialog controls. this is intended to be a more generic approach than having every control
have to show up in the message map */

BOOL CMovementPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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
#if 0 //not needed for this dialog at this point
		case NM_CUSTOMDRAW:
			/* slider */

			if (OnCustomdrawSlider(pNMHDR, pResult) == TRUE)
				return(TRUE);	// processed message
#endif //0
		}
	}

	/* let base class handle anything else */

	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}

/* this catches other control messages. from the help on CWnd::OnCommand, about wParam:

"The low-order word of wParam identifies the command ID of the menu item, control, or accelerator.
The high-order word of wParam specifies the notification message if the message is from a control.
If the message is from an accelerator, the high-order word is 1. If the message is from a menu,
the high-order word is 0." */

BOOL CMovementPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch((wParam >> 16) & 0xffff)
	{
	case EN_KILLFOCUS:
		/* user has finished editing text in an edit box */

		if (OnKillFocusEdit(wParam & 0xffff) == TRUE)
			return(TRUE);	// processed message
		break;
#if 0 //not needed for this dialog at this point
	case CBN_SELCHANGE:
		/* user has selected a new entry in combo box */

		if (OnSelChange(wParam & 0xffff) == TRUE)
			return(TRUE);	// processed message
		break;
#endif //0
	case BN_CLICKED:
		/* user has clicked a button. check boxes are buttons, too */

		if (OnButtonClicked(wParam & 0xffff) == TRUE)
			return(TRUE);	// processed message
		break;
	}

	/* let base class handle anything else */

	return CPropertyPage::OnCommand(wParam, lParam);
}

/* called when page is chosen as active page */

BOOL CMovementPage::OnSetActive()
{
	/* first call base class */

	CPropertyPage::OnSetActive();

	/* then show page */

	ShowData();
	return(TRUE);
}
