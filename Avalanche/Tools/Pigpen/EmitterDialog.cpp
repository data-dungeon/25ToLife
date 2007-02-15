/***********************************/
/*                                 */
/* EmitterDialog.cpp               */
/* Pigpen particle tool  01/22/01  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* dialog box for editing emitters */
/*                                 */
/***********************************/

/* this is the dialog box that let's the user edit a particle emitter */

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
	//	control id							data id										index		min				max									delta		format	bIsInteger
	{	IDC_EMIT_DELAY_EDIT,				CEmitter::DATA_DELAY,					-1,		MIN_E_DELAY,	MAX_E_DELAY,						0.1f,		"%.2f",	false},
	{	IDC_EMIT_DELAY_RANDOM_EDIT,	CEmitter::DATA_DELAY_RANDOM,			-1,		0.0f,				100.0f,								1.0f,		"%.0f",	true},
	{	IDC_EMIT_SCALE_EDIT,				CEmitter::DATA_SCALE,					-1,		0.0f,				MAX_E_SCALE,						0.1f,		"%.2f",	false},
	{	IDC_EMIT_SCALE_RANDOM_EDIT,	CEmitter::DATA_SCALE_RANDOM,			-1,		0.0f,				100.0f,								1.0f,		"%.0f",	true},
	{	IDC_EMIT_VEL_PCT_EDIT,			CEmitter::DATA_VEL_PCT,					-1,		1.0f,				255.0f,								1.0f,		"%.0f",	true},
	{	IDC_EMIT_ADD_VEL_PCT_EDIT,		CEmitter::DATA_ADD_VEL_PCT,			-1,		0.0f,				255.0f,								1.0f,		"%.0f",	true},
	{	IDC_EMIT_MIN_DIST_EDIT,			CEmitter::DATA_MIN_DISTANCE,			-1,		0.0f,				MAX_E_MIN_DIST,					0.5f,		"%.2f",	false},
	{	IDC_EMIT_RADIUS_EDIT,			CEmitter::DATA_RADIUS,					-1,		0.0f,				MAX_E_RADIUS,						0.2f,		"%.2f",	false},
	{	IDC_EMIT_ANG_VEL_EDIT,			CEmitter::DATA_ANGULAR_VEL,			-1,  -9999.0f,				9999.0f,								10.0f,	"%.0f",	true},
	{	IDC_EMIT_GO_RADIUS_EDIT,		CEmitter::DATA_GO_RADIUS,				-1,		0.0f,				MAX_E_GO_RADIUS,					10.0f,	"%.2f",	false},
	{	IDC_EMIT_LIFE_EDIT,				CEmitter::DATA_LIFE,						-1,		0.0f,				MAX_E_LIFETIME,					0.1f,		"%.3f",	false},
	{	IDC_EMIT_LIFE_RANDOM_EDIT,		CEmitter::DATA_LIFE_RANDOM,			-1,		0.0f,				100.0f,								1.0f,		"%.0f",	true},
	{	IDC_EMIT_LOD1_RADIUS_EDIT,		CEmitter::DATA_LOD1_RADIUS,			-1,		0.0f,				MAX_E_LOD_RADIUS,					10.0f,	"%.2f",	false},
	{	IDC_EMIT_LOD2_RADIUS_EDIT,		CEmitter::DATA_LOD2_RADIUS,			-1,		0.0f,				MAX_E_LOD_RADIUS,					10.0f,	"%.2f",	false},
	{	IDC_EMIT_FLUX_SIZE_EDIT,		CEmitter::DATA_FLUX_SIZE,				-1,		0.0f,				MAX_E_FLUX_HALF_SIZE * 2.0f,	1.0f,		"%.2f",	false},
	{	IDC_EMIT_INVIS_DISTANCE_EDIT,	CEmitter::DATA_INVIS_DISTANCE,		-1,		0.0f,				MAX_E_LOD_RADIUS,					10.0f,	"%.2f",	false},
	{	IDC_EMIT_NEAR_DISTANCE_EDIT,	CEmitter::DATA_NEAR_DISTANCE,			-1,		0.0f,				MAX_E_LOD_RADIUS,					10.0f,	"%.2f",	false},
	{	IDC_EMIT_NEAR_SCALE_EDIT,		CEmitter::DATA_NEAR_SCALE,				-1,		0.0f,				MAX_E_SCALE,						0.1f,		"%.2f",	false},
	{	IDC_EMIT_FAR_DISTANCE_EDIT,	CEmitter::DATA_FAR_DISTANCE,			-1,		0.0f,				MAX_E_LOD_RADIUS,					10.0f,	"%.2f",	false},
	{	IDC_EMIT_FAR_SCALE_EDIT,		CEmitter::DATA_FAR_SCALE,				-1,		0.0f,				MAX_E_SCALE,						0.1f,		"%.2f",	false},

	// end of list-- must be last
	{	-1}
};

static ts_CheckBox CheckBoxList[] = {
	//	control id							data id					 index	flag bit							bShowData
	{	IDC_EMIT_DELAY_RANDOM_CHECK,	CEmitter::DATA_FLAGS, -1,		EMITTER_DELAY_CONT_RANDOM,	false},
	{	IDC_EMIT_SCALE_RANDOM_CHECK,	CEmitter::DATA_FLAGS, -1,		EMITTER_SCALE_CONT_RANDOM,	false},
	{	IDC_EMIT_MIN_DIST_CHECK,		CEmitter::DATA_FLAGS, -1,		EMITTER_MIN_DIST_ENABLED,	true},
	{	IDC_EMIT_MOVE_ONLY_CHECK,		CEmitter::DATA_FLAGS, -1,		EMITTER_MOVE_ONLY,			true},

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

BEGIN_MESSAGE_MAP(CEmitterDialog, CDialog)
	//{{AFX_MSG_MAP(CEmitterDialog)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEmitterDialog construction/destruction

CEmitterDialog::CEmitterDialog(CDocView *pParentView, CWnd* pParentWnd) : CDialog(IDD_EMITTER_DIALOG, pParentWnd)
{
	//{{AFX_DATA_INIT(CEmitterDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	ASSERT(pParentView != NULL);
	ASSERT(pParentWnd != NULL);

	m_pParentView = pParentView;
	m_pParentWnd = pParentWnd;
	m_pEmitter = NULL;

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

CEmitterDialog::~CEmitterDialog()
{
	m_pParentView->EditWindowClosed(EDIT_WINDOW0);
}

/* create a modeless dialog */

BOOL CEmitterDialog::Create()
{
	return(CDialog::Create(IDD_EMITTER_DIALOG, m_pParentWnd));
}

/////////////////////////////////////////////////////////////////////////////
// CEmitterDialog operations

/* fill in the data in the dialog box with the current emitter */

void CEmitterDialog::ShowData(CEmitter *pEmitter)
{
	/* record pointer and contents */

	m_pEmitter = pEmitter;
	if (m_pEmitter)
		m_OriginalEmitter = *pEmitter;

	/* title bar is name of current emitter */

	if (m_pEmitter == NULL)
		SetWindowText("Emitter: NONE SELECTED");
	else
		SetWindowText("Emitter: " + m_pEmitter->GetName());

	/* combo box has list of available effects */

	CPigpenDoc *pDoc = m_pParentView->GetDocument();
	CComboBox *pComboBox = (CComboBox *) GetDlgItem(IDC_EFFECT_COMBO);
	pComboBox->ResetContent();	// clear contents
	if (m_pEmitter == NULL)
		pComboBox->EnableWindow(FALSE);
	else
	{
		pComboBox->EnableWindow(TRUE);

		/* add effect names */

		pComboBox->AddString("None");
		for (int j = 0; j < pDoc->GetEffectCount(); j++)
		{
			CEffect *pEffect = pDoc->GetEffect(j);
			pComboBox->AddString(pEffect->GetName());
			if (m_pEmitter->GetEffect() == pEffect)
				pComboBox->SetCurSel(j + 1);	// skip the "none" at start of list
		}

		/* if no effect selected, set current selected to none */

		if (m_pEmitter->GetEffect() == NULL)
			pComboBox->SetCurSel(0);
	}

	/* other combo box has list of available emitter types */

	pComboBox = (CComboBox *) GetDlgItem(IDC_EMIT_TYPE_COMBO);
	pComboBox->ResetContent();	// clear contents
	if (m_pEmitter == NULL)
		pComboBox->EnableWindow(FALSE);
	else
	{
		pComboBox->EnableWindow(TRUE);

		/* add emitter types */

		int i = 0;
		while (strlen(pEmitterTypeString[i]) != 0)
		{
			pComboBox->AddString(pEmitterTypeString[i]);
			if (m_pEmitter->GetType() == i)
				pComboBox->SetCurSel(i);
			i++;
		}
	}
	
	/* paint image buttons */

	GetDlgItem(IDC_EMIT_IMAGE_BUTTON)->Invalidate();	// so it gets repainted, intercepted by OnDrawItem
	
	/* set contents of edit boxes */

	ts_EditBox *pEditBox = EditBoxList;
	while (pEditBox->nID != -1)	// id == -1 marks end of list
	{
		CEdit *pEdit = (CEdit *) GetDlgItem(pEditBox->nID);
		ASSERT(pEdit);		// if this hits you have an entry in a dialog box table that isn't in this dialog page
		if (m_pEmitter == NULL)
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
			case IDC_EMIT_RADIUS_EDIT:
			case IDC_EMIT_ANG_VEL_EDIT:
				pEdit->EnableWindow((pEmitter->GetType() == EMITTER_ORBITER) || (pEmitter->GetType() == EMITTER_TIRE_CIRCLE));
				break;
			case IDC_EMIT_MIN_DIST_EDIT:
				if (pEmitter->GetType() == EMITTER_FLUX)
					pEdit->EnableWindow(false);
				else
					pEdit->EnableWindow(pEmitter->IsMinDistEnabled());
				break;
			case IDC_EMIT_VEL_PCT_EDIT:
				/* only valid for a couple of emitter types */

				if ((pEmitter->GetType() == EMITTER_TIRE_BACK_SPRAY) || (pEmitter->GetType() == EMITTER_TIRE_SIDE_SPRAY))
					pEdit->EnableWindow(pEmitter->IsMoveOnly());
				else
					pEdit->EnableWindow(false);
				break;
			case IDC_EMIT_LOD1_RADIUS_EDIT:
			case IDC_EMIT_LOD2_RADIUS_EDIT:
				if (pEmitter->GetType() == EMITTER_FLUX)
					pEdit->EnableWindow(false);
				break;
			case IDC_EMIT_ADD_VEL_PCT_EDIT:
				if (pEmitter->GetType() == EMITTER_FLUX)
					pEdit->EnableWindow(false);
				break;
			case IDC_EMIT_FLUX_SIZE_EDIT:
				pEdit->EnableWindow(pEmitter->GetType() == EMITTER_FLUX);
				break;
			case IDC_EMIT_INVIS_DISTANCE_EDIT:
			case IDC_EMIT_NEAR_DISTANCE_EDIT:
			case IDC_EMIT_NEAR_SCALE_EDIT:
			case IDC_EMIT_FAR_DISTANCE_EDIT:
			case IDC_EMIT_FAR_SCALE_EDIT:
				pEdit->EnableWindow(pEmitter->GetType() == EMITTER_DISTANT);
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
		if (m_pEmitter == NULL)
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
			case IDC_EMIT_MOVE_ONLY_CHECK:
				switch(pEmitter->GetType())
				{
				case EMITTER_TIRE_SIDE_SPRAY:
				case EMITTER_TIRE_BACK_SPRAY:
				case EMITTER_FLUX:
					pButton->EnableWindow(false);
					break;
				}
				break;
			case IDC_EMIT_MIN_DIST_CHECK:
				if (pEmitter->GetType() == EMITTER_FLUX)
					pButton->EnableWindow(false);
				break;
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
		if (m_pEmitter == NULL)
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

	if ((m_pEmitter) && (m_pEmitter->GetEffect()))
		GetDlgItem(IDC_PREVIEW)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_PREVIEW)->EnableWindow(FALSE);
}

/* update edit box's value. if desired, redraws channel's graphic display if count, emit delay, or emit time
was changed. if pfDirectValue is non-NULL, set value directly. for easier implementation i am using floats for
everything that goes in an edit box, even if they will ultimately be ints */

void CEmitterDialog::UpdateEditBox(int nID, int nDelta, float *pfDirectValue, bool bCheckSpecialCases)
{
	float *pfValue = NULL;
	float fOldValue;
	char acString[20];

	/* should be working on valid data */

	ASSERT(m_pEmitter != NULL);

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

	pfValue = (float *) m_pEmitter->GetDataPointer(pEditBox->nDataID);
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

	/* check for additional actions when changing an edit box */

	if (bCheckSpecialCases)
	{
/************** special cases ***********************/
		switch(pEditBox->nID)
		{
		case IDC_EMIT_LOD1_RADIUS_EDIT:
			/* lod far radius is always >= than lod near radius */

			if (m_pEmitter->CheckLOD1Radius())
				ShowData(m_pEmitter);
			break;
		case IDC_EMIT_LOD2_RADIUS_EDIT:
			/* lod near radius is always <= than lod far radius */

			if (m_pEmitter->CheckLOD2Radius())
				ShowData(m_pEmitter);
			break;
		case IDC_EMIT_INVIS_DISTANCE_EDIT:
			/* maintain the relationship invis distance <= near distance <= far distance */

			if (m_pEmitter->CheckInvisDistance())
				ShowData(m_pEmitter);
			break;
		case IDC_EMIT_NEAR_DISTANCE_EDIT:
			/* maintain the relationship invis distance <= near distance <= far distance */

			if (m_pEmitter->CheckNearDistance())
				ShowData(m_pEmitter);
			break;
		case IDC_EMIT_FAR_DISTANCE_EDIT:
			/* maintain the relationship invis distance <= near distance <= far distance */

			if (m_pEmitter->CheckFarDistance())
				ShowData(m_pEmitter);
			break;
		}
/************** end special cases *******************/
	}

	/* has doc changed? */

	if (*pfValue != fOldValue)
	{
		CPigpenDoc *pDoc = m_pParentView->GetDocument();
		pDoc->SetModifiedFlag(MODIFIED_FLAG);
		Preview__UpdateEmitter(m_pEmitter);
	}
}

/* update check box's state. check/uncheck depending on proper bit's state in proper flag unsigned long word */

void CEmitterDialog::UpdateCheckBox(int nID, bool bToggle)
{
	unsigned long *pulFlags = NULL;

	/* should be working on valid data */

	ASSERT(m_pEmitter != NULL);

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

	pulFlags = (unsigned long *) m_pEmitter->GetDataPointer(pCheckBox->nDataID);
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
		Preview__UpdateEmitter(m_pEmitter);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEmitterDialog message handlers

/* called when OK is selected, or when user presses enter. i don't want pressing enter to end the dialog, so
override and do nothing */

void CEmitterDialog::OnOK()
{
}

/* called when dialog is closed by sysmenu close button (X) */

void CEmitterDialog::OnCancel()
{
	/* from the help on OnCancel(): "If you implement the Cancel button in a modeless dialog box, you must */
	/* override the OnCancel member function and call DestroyWindow from within it. Don’t call the base-class */
	/* member function, because it calls EndDialog, which will make the dialog box invisible but not destroy it." */

	DestroyWindow();
}

/* called after window is destroyed */

void CEmitterDialog::PostNcDestroy()
{
	delete this;	// dialog was created by parent window using "new"
}

/* intercept spin control messages to update associated edit box value */

bool CEmitterDialog::OnSpinner(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/* not if no emitter */

	if (m_pEmitter == NULL)
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

/* change selected effect for emitter */

bool CEmitterDialog::OnSelChange(int nID)
{
	_ASSERT(m_pEmitter);
	CPigpenDoc *pDoc = m_pParentView->GetDocument();
	CComboBox* pComboBox = (CComboBox*) GetDlgItem(nID);
	ASSERT(pComboBox);
	int nListIndex = pComboBox->GetCurSel();
	switch(nID)
	{
	case IDC_EFFECT_COMBO:
		/* get new selection from combo box */

		if (nListIndex == 0)	// none
			m_pEmitter->SetEffect(NULL);
		else
			m_pEmitter->SetEffect(pDoc->GetEffect(nListIndex - 1));	// "- 1" because first entry is "none"

		/* update image button */

		GetDlgItem(IDC_EMIT_IMAGE_BUTTON)->Invalidate();	// so it gets repainted, intercepted by OnDrawItem
		break;
	case IDC_EMIT_TYPE_COMBO:
		m_pEmitter->SetType(nListIndex);

		/* special cases */

		switch(m_pEmitter->GetType())
		{
		case EMITTER_TIRE_SIDE_SPRAY:
		case EMITTER_TIRE_BACK_SPRAY:
			m_pEmitter->SetIsMoveOnly();	// tire sprays only work when emitter is emit-when-move-only
			break;
		}

		/* re-show data and done */

		ShowData(m_pEmitter);	// re-show to maybe activate/deactivate radius and angular velocity edit boxes
		Preview__UpdateEmitter(m_pEmitter);
		break;
	default:
		_ASSERT(FALSE);
		break;
	}

	/* doc has changed, update emitters view */

	pDoc->SetModifiedFlag(MODIFIED_FLAG);
	pDoc->UpdateAllViews(NULL, NO_REORDER_EMITTERS_VIEW | NO_CHANGE_EFFECTS_VIEW | NO_CHANGE_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_CHANGE_SWOOSHES_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW);
	Preview__UpdateEmitter(m_pEmitter);
	return(TRUE);	// processed message
}

/* catch button clicks. this routine does not have to handle all button clicks, but needs to return FALSE if
it gets passed a button it doesn't understand or want to deal with */

bool CEmitterDialog::OnButtonClicked(int nID)
{
	/* normal buttons */

	switch(nID)
	{
	case IDC_PREVIEW:
		_ASSERT(m_pEmitter);	// should be disabled if no emitter or effect
		_ASSERT(m_pEmitter->GetEffect());
		Preview__Create(m_pParentView->GetDocument(), m_pEmitter, NULL, NULL, NULL, NULL);
		return(TRUE);	// processed message

	case IDC_EMIT_IMAGE_BUTTON:
		/* clicking on effect image jumps to that effect */

		if ((m_pEmitter) && (m_pEmitter->GetEffect()))
		{
			CPigpenDoc *pDoc = m_pParentView->GetDocument();

			/* open view if necessary */

			if (pDoc->GetView(EFFECTS) == NULL)
				pDoc->OnView(EFFECTS);

			/* update view */

			CDocView *pView = pDoc->GetView(EFFECTS);
			if (pView)
			{
				/* select correct entry */

				pView->FindAndSelectItem(m_pEmitter->GetEffect());

				/* open edit window if necessary */

				pView->OnDoubleClick(NULL, NULL);	// opens window if not open and valid item selected

				/* bring edit window to top */

				if (pView->GetEditWindow(0))
					pView->GetEditWindow(0)->BringWindowToTop();

				/* update graphic windows */

				pDoc->UpdateAllViews(NULL, NO_CHANGE_EMITTERS_VIEW | NO_REORDER_EFFECTS_VIEW | NO_CHANGE_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_CHANGE_SWOOSHES_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW);
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
				ShowData(m_pEmitter);

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
				ShowData(m_pEmitter);

			return(TRUE);	// processed message
		}
		pRadioButton++;
	}
#endif //0

	/* didn't handle the button */

	return(FALSE);
}

/* catch user editing box directly */

bool CEmitterDialog::OnKillFocusEdit(int nID) 
{
	CEdit *pEdit = (CEdit *) GetDlgItem(nID);
	if (m_pEmitter == NULL)
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

void CEmitterDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	/* bitmap or empty frame? */

	CImage *pImage = NULL;
	bool bShowAlphaMask;
	if (m_pEmitter)
		pImage = m_pEmitter->GetImage(&bShowAlphaMask);
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

BOOL CEmitterDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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

BOOL CEmitterDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
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

