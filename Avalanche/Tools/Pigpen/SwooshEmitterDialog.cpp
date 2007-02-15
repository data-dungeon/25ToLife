/***********************************/
/*                                 */
/* SwooshEmitterDialog.cpp         */
/* Pigpen particle tool  11/25/02  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* dialog box for editing emitters */
/*                                 */
/***********************************/

/* this is the dialog box that let's the user edit a swoosh emitter */

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
	//	control id											data id																	index		min			max			delta		format	bIsInteger
	{	IDC_SWOOSH_EMIT_RADIUS_EDIT,					CPigpenSwooshEmitter::DATA_CYL_RADIUS,							-1,		0.0f,			100.0f,		1.0f,		"%.1f",	false},
	{	IDC_SWOOSH_EMIT_RADIUS_RANDOM_PCT_EDIT,	CPigpenSwooshEmitter::DATA_CYL_RADIUS_RANDOM_PCT,			-1,		0.0f,			100.0f,		1.0f,		"%.0f",	true},
	{	IDC_SWOOSH_EMIT_HEIGHT_EDIT,					CPigpenSwooshEmitter::DATA_CYL_HEIGHT,							-1,		0.0f,			100.0f,		1.0f,		"%.1f",	false},
	{	IDC_SWOOSH_EMIT_YVEL_EDIT,						CPigpenSwooshEmitter::DATA_CYL_YVEL,							-1,		0.0f,			100.0f,		0.1f,		"%.1f",	false},
	{	IDC_SWOOSH_EMIT_YVEL_RANDOM_PCT_EDIT,		CPigpenSwooshEmitter::DATA_CYL_YVEL_RANDOM_PCT,				-1,		0.0f,			100.0f,		1.0f,		"%.0f",	true},
	{	IDC_SWOOSH_EMIT_ROTVEL_EDIT,					CPigpenSwooshEmitter::DATA_CYL_ROTVEL,							-1,		-1000.0f,	1000.0f,		5.0f,		"%.1f",	false},
	{	IDC_SWOOSH_EMIT_POINTS_EDIT,					CPigpenSwooshEmitter::DATA_CYL_CIRCLE_POINTS,				-1,		1.0f,			50.0f,		1.0f,		"%.0f",	true},
	{	IDC_SWOOSH_EMIT_ANGLE_EDIT,					CPigpenSwooshEmitter::DATA_CYL_DELTA_ANGLE,					-1,		-360.0f,		360.0f,		5.0f,		"%.1f",	false},
	{	IDC_SWOOSH_EMIT_TIME_EDIT,						CPigpenSwooshEmitter::DATA_CYL_DELTA_TIME,					-1,		0.1f,			20.0f,		0.1f,		"%.1f",	false},
	{	IDC_SWOOSH_EMIT_WIDTH_EDIT,					CPigpenSwooshEmitter::DATA_CYL_SWOOSH_WIDTH,					-1,		0.01f,		10.0f,		0.1f,		"%.2f",	false},
	{	IDC_SWOOSH_EMIT_WIDTH_RANDOM_PCT_EDIT,		CPigpenSwooshEmitter::DATA_CYL_SWOOSH_WIDTH_RANDOM_PCT,	-1,		0.0f,			100.0f,		1.0f,		"%.0f",	true},
	{	IDC_SWOOSH_EMIT_LIFE_EDIT,						CPigpenSwooshEmitter::DATA_CYL_LIFE,							-1,		0.0f,			100.0f,		0.1f,		"%.1f",	false},
	{	IDC_SWOOSH_EMIT_LIFE_RANDOM_PCT_EDIT,		CPigpenSwooshEmitter::DATA_CYL_LIFE_RANDOM_PCT,				-1,		0.0f,			100.0f,		1.0f,		"%.0f",	true},

	// end of list-- must be last
	{	-1}
};

static ts_CheckBox CheckBoxList[] = {
	//	control id										data id									index		flag bit								bShowData
	{	IDC_SWOOSH_EMIT_SPARSE_CYLINDER_CHECK,	CPigpenSwooshEmitter::DATA_FLAGS, -1,		SWOO_EMITTER_SPARSE_CYLINDER,	true},

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

BEGIN_MESSAGE_MAP(CSwooshEmitterDialog, CDialog)
	//{{AFX_MSG_MAP(CSwooshEmitterDialog)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSwooshEmitterDialog construction/destruction

CSwooshEmitterDialog::CSwooshEmitterDialog(CDocView *pParentView, CWnd* pParentWnd) : CDialog(IDD_SWOOSH_EMITTER_DIALOG, pParentWnd)
{
	//{{AFX_DATA_INIT(CSwooshEmitterDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	ASSERT(pParentView != NULL);
	ASSERT(pParentWnd != NULL);

	m_pParentView = pParentView;
	m_pParentWnd = pParentWnd;
	m_pSwooshEmitter = NULL;

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
}

/* notify parent view that edit window is gone */

CSwooshEmitterDialog::~CSwooshEmitterDialog()
{
	m_pParentView->EditWindowClosed(EDIT_WINDOW0);
}

/* create a modeless dialog */

BOOL CSwooshEmitterDialog::Create()
{
	return(CDialog::Create(IDD_SWOOSH_EMITTER_DIALOG, m_pParentWnd));
}

/////////////////////////////////////////////////////////////////////////////
// CSwooshEmitterDialog operations

/* fill in the data in the dialog box with the current emitter */

void CSwooshEmitterDialog::ShowData(CPigpenSwooshEmitter *pSwooshEmitter)
{
	/* record pointer and contents */

	m_pSwooshEmitter = pSwooshEmitter;
	if (m_pSwooshEmitter)
		m_OriginalSwooshEmitter = *pSwooshEmitter;

	/* title bar is name of current emitter */

	if (m_pSwooshEmitter == NULL)
		SetWindowText("Swoosh emitter: NONE SELECTED");
	else
		SetWindowText("Swoosh emitter: " + m_pSwooshEmitter->GetName());

	/* combo box has list of available swooshes */

	CPigpenDoc *pDoc = m_pParentView->GetDocument();
	CComboBox *pComboBox = (CComboBox *) GetDlgItem(IDC_SWOOSH_COMBO);
	pComboBox->ResetContent();	// clear contents
	if (m_pSwooshEmitter == NULL)
		pComboBox->EnableWindow(FALSE);
	else
	{
		pComboBox->EnableWindow(TRUE);

		/* add swoosh names */

		pComboBox->AddString("None");
		for (int j = 0; j < pDoc->GetSwooshCount(); j++)
		{
			CSwoosh *pSwoosh = pDoc->GetSwoosh(j);
			pComboBox->AddString(pSwoosh->GetName());
			if (m_pSwooshEmitter->GetSwoosh() == pSwoosh)
				pComboBox->SetCurSel(j + 1);	// skip the "none" at start of list
		}

		/* if no swoosh selected, set current selected to none */

		if (m_pSwooshEmitter->GetSwoosh() == NULL)
			pComboBox->SetCurSel(0);
	}

	/* other combo box has list of available emitter types */

	pComboBox = (CComboBox *) GetDlgItem(IDC_SWOOSH_EMIT_TYPE_COMBO);
	pComboBox->ResetContent();	// clear contents
	if (m_pSwooshEmitter == NULL)
		pComboBox->EnableWindow(FALSE);
	else
	{
		pComboBox->EnableWindow(TRUE);

		/* add emitter types */

		int i = 0;
		while (strlen(pSwooshEmitterTypeString[i]) != 0)
		{
			pComboBox->AddString(pSwooshEmitterTypeString[i]);
			if (m_pSwooshEmitter->GetType() == i)
				pComboBox->SetCurSel(i);
			i++;
		}
	}
	
	/* paint image buttons */

	GetDlgItem(IDC_SWOOSH_EMIT_IMAGE_BUTTON)->Invalidate();	// so it gets repainted, intercepted by OnDrawItem
	
	/* set contents of edit boxes */

	ts_EditBox *pEditBox = EditBoxList;
	while (pEditBox->nID != -1)	// id == -1 marks end of list
	{
		CEdit *pEdit = (CEdit *) GetDlgItem(pEditBox->nID);
		ASSERT(pEdit);		// if this hits you have an entry in a dialog box table that isn't in this dialog page
		if (m_pSwooshEmitter == NULL)
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
			case IDC_SWOOSH_EMIT_RADIUS_EDIT:
			case IDC_SWOOSH_EMIT_RADIUS_RANDOM_PCT_EDIT:
			case IDC_SWOOSH_EMIT_HEIGHT_EDIT:
			case IDC_SWOOSH_EMIT_YVEL_EDIT:
			case IDC_SWOOSH_EMIT_YVEL_RANDOM_PCT_EDIT:
			case IDC_SWOOSH_EMIT_ROTVEL_EDIT:
			case IDC_SWOOSH_EMIT_TIME_EDIT:
			case IDC_SWOOSH_EMIT_ANGLE_EDIT:
			case IDC_SWOOSH_EMIT_WIDTH_EDIT:
			case IDC_SWOOSH_EMIT_WIDTH_RANDOM_PCT_EDIT:
			case IDC_SWOOSH_EMIT_POINTS_EDIT:
			case IDC_SWOOSH_EMIT_LIFE_EDIT:
			case IDC_SWOOSH_EMIT_LIFE_RANDOM_PCT_EDIT:
				pEdit->EnableWindow(pSwooshEmitter->GetType() == SWOOSH_EMITTER_CYLINDER);
				if (pSwooshEmitter->GetType() == SWOOSH_EMITTER_CYLINDER)
				{
					switch(pEditBox->nID)
					{
						case IDC_SWOOSH_EMIT_LIFE_EDIT:
						case IDC_SWOOSH_EMIT_LIFE_RANDOM_PCT_EDIT:
							pEdit->EnableWindow(pSwooshEmitter->IsSparseCylinder());
						break;
					}
				}
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
		if (m_pSwooshEmitter == NULL)
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

//			switch(pCheckBox->nID)
//			{
//			case IDC_EMIT_MOVE_ONLY_CHECK:
//				switch(pEmitter->GetType())
//				{
//				case EMITTER_TIRE_SIDE_SPRAY:
//				case EMITTER_TIRE_BACK_SPRAY:
//				case EMITTER_FLUX:
//					pButton->EnableWindow(false);
//					break;
//				}
//				break;
//			case IDC_EMIT_MIN_DIST_CHECK:
//				if (pEmitter->GetType() == EMITTER_FLUX)
//					pButton->EnableWindow(false);
//				break;
//			}
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
		if (m_pSwooshEmitter == NULL)
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
#endif

	/* enable/disable normal button */

	if ((m_pSwooshEmitter) && (m_pSwooshEmitter->GetSwoosh()))
		GetDlgItem(IDC_PREVIEW)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_PREVIEW)->EnableWindow(FALSE);
}

/* update edit box's value. if desired, redraws channel's graphic display if count, emit delay, or emit time
was changed. if pfDirectValue is non-NULL, set value directly. for easier implementation i am using floats for
everything that goes in an edit box, even if they will ultimately be ints */

void CSwooshEmitterDialog::UpdateEditBox(int nID, int nDelta, float *pfDirectValue, bool bCheckSpecialCases)
{
	float *pfValue = NULL;
	float fOldValue;
	char acString[20];

	/* should be working on valid data */

	ASSERT(m_pSwooshEmitter != NULL);

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

	pfValue = (float *) m_pSwooshEmitter->GetDataPointer(pEditBox->nDataID);
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

	/* check min/max, update displayed text */

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
//		switch(pEditBox->nID)
//		{
//		}
/************** end special cases *******************/
	}

	/* has doc changed? */

	if (*pfValue != fOldValue)
	{
		CPigpenDoc *pDoc = m_pParentView->GetDocument();
		pDoc->SetModifiedFlag(MODIFIED_FLAG);
		Preview__UpdateSwooshEmitter(m_pSwooshEmitter);
	}
}

/* update check box's state. check/uncheck depending on proper bit's state in proper flag unsigned long word */

void CSwooshEmitterDialog::UpdateCheckBox(int nID, bool bToggle)
{
	unsigned long *pulFlags = NULL;

	/* should be working on valid data */

	ASSERT(m_pSwooshEmitter != NULL);

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

	pulFlags = (unsigned long *) m_pSwooshEmitter->GetDataPointer(pCheckBox->nDataID);
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
		Preview__UpdateSwooshEmitter(m_pSwooshEmitter);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSwooshEmitterDialog message handlers

/* called when OK is selected, or when user presses enter. i don't want pressing enter to end the dialog, so
override and do nothing */

void CSwooshEmitterDialog::OnOK()
{
}

/* called when dialog is closed by sysmenu close button (X) */

void CSwooshEmitterDialog::OnCancel()
{
	/* from the help on OnCancel(): "If you implement the Cancel button in a modeless dialog box, you must */
	/* override the OnCancel member function and call DestroyWindow from within it. Don’t call the base-class */
	/* member function, because it calls EndDialog, which will make the dialog box invisible but not destroy it." */

	DestroyWindow();
}

/* called after window is destroyed */

void CSwooshEmitterDialog::PostNcDestroy()
{
	delete this;	// dialog was created by parent window using "new"
}

/* intercept spin control messages to update associated edit box value */

bool CSwooshEmitterDialog::OnSpinner(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/* not if no emitter */

	if (m_pSwooshEmitter == NULL)
	{
		*pResult = 1;	// non-zero means abort further processing
		return(FALSE);	// didn't process message
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

/* change selected swoosh for emitter */

bool CSwooshEmitterDialog::OnSelChange(int nID)
{
	_ASSERT(m_pSwooshEmitter);
	CPigpenDoc *pDoc = m_pParentView->GetDocument();
	CComboBox* pComboBox = (CComboBox*) GetDlgItem(nID);
	ASSERT(pComboBox);
	int nListIndex = pComboBox->GetCurSel();
	switch(nID)
	{
	case IDC_SWOOSH_COMBO:
		/* get new selection from combo box */

		if (nListIndex == 0)	// none
			m_pSwooshEmitter->SetSwoosh(NULL);
		else
			m_pSwooshEmitter->SetSwoosh(pDoc->GetSwoosh(nListIndex - 1));	// "- 1" because first entry is "none"

		/* update image button */

		GetDlgItem(IDC_SWOOSH_EMIT_IMAGE_BUTTON)->Invalidate();	// so it gets repainted, intercepted by OnDrawItem
		break;
	case IDC_SWOOSH_EMIT_TYPE_COMBO:
		m_pSwooshEmitter->SetType(nListIndex);

		/* re-show data and done */

		ShowData(m_pSwooshEmitter);	// re-show to maybe activate/deactivate radius and angular velocity edit boxes
		Preview__UpdateSwooshEmitter(m_pSwooshEmitter);
		break;
	default:
		_ASSERT(FALSE);
		break;
	}

	/* doc has changed, update emitters view */

	pDoc->SetModifiedFlag(MODIFIED_FLAG);
	pDoc->UpdateAllViews(NULL, NO_REORDER_EMITTERS_VIEW | NO_CHANGE_EFFECTS_VIEW | NO_CHANGE_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_CHANGE_SWOOSHES_VIEW | NO_REORDER_SWOOSH_EMITTERS_VIEW);
	Preview__UpdateSwooshEmitter(m_pSwooshEmitter);
	return(TRUE);	// processed message
}

/* catch button clicks. this routine does not have to handle all button clicks, but needs to return FALSE if
it gets passed a button it doesn't understand or want to deal with */

bool CSwooshEmitterDialog::OnButtonClicked(int nID)
{
	/* normal buttons */

	switch(nID)
	{
	case IDC_PREVIEW:
		_ASSERT(m_pSwooshEmitter);	// should be disabled if no emitter or swoosh
		_ASSERT(m_pSwooshEmitter->GetSwoosh());
		Preview__Create(m_pParentView->GetDocument(), NULL, NULL, NULL, NULL, m_pSwooshEmitter);
		return(TRUE);	// processed message

	case IDC_SWOOSH_EMIT_IMAGE_BUTTON:
		/* clicking on swoosh image jumps to that swoosh */

		if ((m_pSwooshEmitter) && (m_pSwooshEmitter->GetSwoosh()))
		{
			CPigpenDoc *pDoc = m_pParentView->GetDocument();

			/* open view if necessary */

			if (pDoc->GetView(SWOOSHES) == NULL)
				pDoc->OnView(SWOOSHES);

			/* update view */

			CDocView *pView = pDoc->GetView(SWOOSHES);
			if (pView)
			{
				/* select correct entry */

				pView->FindAndSelectItem(m_pSwooshEmitter->GetSwoosh());

				/* open edit window if necessary */

				pView->OnDoubleClick(NULL, NULL);	// opens window if not open and valid item selected

				/* bring edit window to top */

				if (pView->GetEditWindow(0))
					pView->GetEditWindow(0)->BringWindowToTop();

				/* update graphic windows */

				pDoc->UpdateAllViews(NULL, NO_CHANGE_EMITTERS_VIEW | NO_CHANGE_EFFECTS_VIEW | NO_CHANGE_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_REORDER_SWOOSHES_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW);
			}
		}
		return(true);	// processed message
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

			/* need to re-show data? this is useful for enabling/disabling buttons after button state changes */

			if (pCheckBox->bShowData)
				ShowData(m_pSwooshEmitter);

			return(TRUE);	// processed message
		}
		pCheckBox++;
	}

#if 0	// no radio buttons currently
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
				ShowData(m_pSwooshEmitter);

			return(TRUE);	// processed message
		}
		pRadioButton++;
	}
#endif //0

	/* didn't handle the button */

	return(FALSE);
}

/* catch user editing box directly */

bool CSwooshEmitterDialog::OnKillFocusEdit(int nID) 
{
	CEdit *pEdit = (CEdit *) GetDlgItem(nID);
	if (m_pSwooshEmitter == NULL)
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

/* called when owner-draw flagged dialog buttons are going to be drawn. used to draw channel diagrams */

void CSwooshEmitterDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	/* bitmap or empty frame? */

	CImage *pImage = NULL;
	bool bShowAlphaMask;
	if (m_pSwooshEmitter)
		pImage = m_pSwooshEmitter->GetImage(&bShowAlphaMask);
	if (pImage)
	{
		/* frame outline */

		HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
		HBRUSH hOldBrush = (HBRUSH) SelectObject(lpDrawItemStruct->hDC, hBrush);
		FrameRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, hBrush);
		SelectObject(lpDrawItemStruct->hDC, hOldBrush);
		DeleteObject(hBrush);

		/* image */

		pImage->Paint(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, 2, bShowAlphaMask);
	}
	else
	{
		/* Rectangle gets filled with current brush and outlined with current pen */

		HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_MENU));
		HBRUSH hOldBrush = (HBRUSH) SelectObject(lpDrawItemStruct->hDC, hBrush);
		HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
		HPEN hOldPen = (HPEN) SelectObject(lpDrawItemStruct->hDC, hPen);
		Rectangle(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom);
		SelectObject(lpDrawItemStruct->hDC, hOldBrush);
		SelectObject(lpDrawItemStruct->hDC, hOldPen);
		DeleteObject(hBrush);
		DeleteObject(hPen);
	}
}

/* handle messages from dialog controls. this is intended to be a more generic approach than having every control
have to show up in the message map */

BOOL CSwooshEmitterDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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

BOOL CSwooshEmitterDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
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
		/* user has clicked a button */

		if (OnButtonClicked(wParam & 0xffff) == TRUE)
			return(TRUE);	// processed message
		break;
	}

	/* let base class handle anything else */

	return CDialog::OnCommand(wParam, lParam);
}

