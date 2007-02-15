/***********************************/
/*                                 */
/* PageDisplay.cpp                 */
/* Pigpen particle tool  12/28/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle display property page  */
/*                                 */
/***********************************/

/* this is display page of the particle property sheet. each page is a dialog */

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
	//	control id						data id							index		min				max				delta		format	bIsInteger
	{	IDC_PART_FPS_EDIT,			CParticle::DATA_FPS,				-1,	0.0f,				60.0f,			1.0f,		"%.0f",	true},
	{	IDC_PART_FPS_RANDOM_EDIT,	CParticle::DATA_FPS_RANDOM,	-1,	0.0f,				100.0f,			1.0f,		"%.0f",	true},
	{	IDC_PART_ZBIAS_EDIT,			CParticle::DATA_ZBIAS,			-1,	-MAX_P_ZBIAS,	MAX_P_ZBIAS,	0.1f,		"%.4f",	false},
	
	// end of list-- must be last
	{	-1}
};

static ts_CheckBox CheckBoxList[] = {
	//	control id							data id								 index	flag bit									bShowData
	{	IDC_PART_FPS_RANDOM_CHECK,		CParticle::DATA_RANDOM_FLAGS,		-1,	PART_FPS_CONT_RANDOM,				false},
//obsolete	{	IDC_PART_XMIRROR_CHECK,			CParticle::DATA_FLAGS,				-1,	PART_XMIRROR,					true},
//obsolete	{	IDC_PART_YMIRROR_CHECK,			CParticle::DATA_FLAGS,				-1,	PART_YMIRROR,					true},
	{	IDC_PART_RANDOM_FRAME_CHECK,	CParticle::DATA_RANDOM_FLAGS,		-1,	PART_RANDOM_START_FRAME,			false},
	{	IDC_ALIGN_TO_VEL_CHECK,			CParticle::DATA_FLAGS,				-1,	PART_ALIGN_TO_VELOCITY,				true},
	{	IDC_PART_LIGHTED_CHECK,			CParticle::DATA_FLAGS,				-1,	PART_LIGHTED,							false},
	{	IDC_PART_NO_DISTORTION_CHECK,	CParticle::DATA_FLAGS,				-1,	PART_NO_DISTORTION,					false},
	{	IDC_PART_FORCEFIELD_CHECK,		CParticle::DATA_FLAGS,				-1,	PART_FORCEFIELDS_AFFECT,			false},
	{	IDC_PART_ZBUFFER_CHECK,			CParticle::DATA_FLAGS,				-1,	PART_ZBUFFER_WRITE,					true},
	{	IDC_PART_ADDITIVE_CHECK,		CParticle::DATA_FLAGS,				-1,	PART_ADDITIVE_RENDER,				false},
	{	IDC_PART_FLOATS_CHECK,			CParticle::DATA_FLAGS,				-1,	PART_FLOATS_ON_WATER,				true},
	{	IDC_PERPENDICULAR_CHECK,		CParticle::DATA_FLAGS,				-1,	PART_PERPENDICULAR_TO_VELOCITY,	false},
	
	// end of list-- must be last
	{	-1}
};

static ts_RadioButton RadioButtonList[] = {
	//	control id						data id							value							mask						bShowData
	{	IDC_ORIENT_SCREEN_RADIO,	CParticle::DATA_BITFIELDS,	PART_ORIENT_SCREEN,		PART_ORIENT_MASK,		true},
	{	IDC_ORIENT_FREE_RADIO,		CParticle::DATA_BITFIELDS,	PART_ORIENT_FREE,			PART_ORIENT_MASK,		true},
	{	IDC_ORIENT_GROUND_RADIO,	CParticle::DATA_BITFIELDS,	PART_ORIENT_GROUND,		PART_ORIENT_MASK,		true},
//wall orientation is not enabled yet
//	{	IDC_ORIENT_WALL_RADIO,		CParticle::DATA_BITFIELDS,	PART_ORIENT_WALL,			PART_ORIENT_MASK,		true},

	{	IDC_ANIM_LOOP_RADIO,			CParticle::DATA_BITFIELDS,	PART_ANIM_LOOP,			PART_ANIM_MASK,		false},
	{	IDC_ANIM_PING_PONG_RADIO,	CParticle::DATA_BITFIELDS,	PART_ANIM_PING_PONG,		PART_ANIM_MASK,		false},
	{	IDC_ANIM_ONE_SHOT_RADIO,	CParticle::DATA_BITFIELDS,	PART_ANIM_ONE_SHOT,		PART_ANIM_MASK,		false},

	// end of list-- must be last
	{	-1}
};

/******************* externs *****************************/

/******************* class functions *********************/
 
/* i am trying to avoid long message maps, so i have overridden OnNotify and OnCommand. they catch slider bars,
spinners, edit boxes, and button clicks for me and process them more generically. the message map stuff still works,
but in general, use the higher-level intercept routines (like OnKillFocusEdit to catch all edit box commands) */

BEGIN_MESSAGE_MAP(CDisplayPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDisplayPage)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDisplayPage construction/destruction

CDisplayPage::CDisplayPage(CDocView *pParentView, CParticleSheet *pParentSheet) : CPropertyPage(CDisplayPage::IDD)
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

	//{{AFX_DATA_INIT(CDisplayPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
// CDisplayPage operations

/* fill in the data in the dialog box with the current particle */

void CDisplayPage::ShowData(void)
{
	/* get current particle from parent sheet */

	m_pParticle = m_pParentSheet->GetParticle();

	/* combo box has list of available particles */

	CPigpenDoc *pDoc = m_pParentView->GetDocument();
	CComboBox *pComboBox = (CComboBox *) GetDlgItem(IDC_GRAPHIC);
	pComboBox->ResetContent();	// clear contents
	if (m_pParticle == NULL)
		pComboBox->EnableWindow(false);
	else
	{
		pComboBox->EnableWindow(true);

		/* add hardwired choices */

		pComboBox->AddString("None");
		pComboBox->AddString("Front buffer (distort)");
		pComboBox->AddString("Back buffer (distort)");
		ASSERT(pComboBox->GetCount() == PARTICLE_HARDWIRED_GRAPHIC_LIST_COUNT);

		/* add currently available graphics, set current selection if valid graphic */

		for (int i = 0; i < pDoc->GetGraphicCount(); i++)
		{
			CGraphic *pGraphic = pDoc->GetGraphic(i);
			pComboBox->AddString(pGraphic->GetName());
			if (m_pParticle->GetGraphic() == pGraphic)
				pComboBox->SetCurSel(i + PARTICLE_HARDWIRED_GRAPHIC_LIST_COUNT);	// skip the hardwired choices at start of list
		}

		/* if no valid graphic, set current hardwired graphic selection correctly */

		if (m_pParticle->GetGraphic() == NULL)
		{
			switch(m_pParticle->GetHardwiredGraphic())
			{
			default:
				ASSERT(false);	// unknown case
				/* and fall through */
			case PARTICLE_HARDWIRED_GRAPHIC_NONE:
				pComboBox->SetCurSel(PARTICLE_HARDWIRED_GRAPHIC_NONE_IN_LIST);
				break;
			case PARTICLE_HARDWIRED_GRAPHIC_FRONT_BUFFER:
				pComboBox->SetCurSel(PARTICLE_HARDWIRED_GRAPHIC_FRONT_BUFFER_IN_LIST);
				break;
			case PARTICLE_HARDWIRED_GRAPHIC_BACK_BUFFER:
				pComboBox->SetCurSel(PARTICLE_HARDWIRED_GRAPHIC_BACK_BUFFER_IN_LIST);
				break;
			}
		}
	}

	/* invalidate image frames so they get re-drawn by OnDrawItem */

	CStatic *pStatic = (CStatic *) GetDlgItem(IDC_IMAGE_BUTTON);
	ASSERT(pStatic);
	pStatic->Invalidate();	// so it gets repainted, intercepted by OnDrawItem

	/* set contents of edit boxes */

	ts_EditBox *pEditBox = EditBoxList;
	while (pEditBox->nID != -1)	// id == -1 marks end of list
	{
		CEdit *pEdit = (CEdit *) GetDlgItem(pEditBox->nID);
		ASSERT(pEdit);		// if this hits you have an entry in a dialog box table that isn't in this dialog page
		if (m_pParticle == NULL)
		{
			pEdit->SetWindowText("N/A");
			pEdit->EnableWindow(false);
		}
		else
		{
			pEdit->EnableWindow(true);
			UpdateEditBox(pEditBox->nID, 0, NULL, false);

/************** special cases ***********************/
			/* special cases for disabling a box based on data state */

//			switch(pEditBox->nID)
//			{
//			}
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
			pButton->EnableWindow(false);
		}
		else
		{
			pButton->EnableWindow(true);
			UpdateCheckBox(pCheckBox->nID, false);

/************** special cases ***********************/
			/* special cases for disabling a box based on data state */

			switch(pCheckBox->nID)
			{
			case IDC_PART_LIGHTED_CHECK:
				if (m_pParticle->GetOrient() == PART_ORIENT_SCREEN)	// no real-time lighting for screen-aligned particles since they don't have a 3D orientation
					pButton->EnableWindow(false);
				else if (m_pParticle->IsDistortionGraphic())	// no real-time lighting for distortion graphic particles
					pButton->EnableWindow(false);
				break;
			case IDC_PERPENDICULAR_CHECK:
				/* only enabled for free orient and aligned to velocity */

				if ((m_pParticle->GetOrient() != PART_ORIENT_FREE) || (m_pParticle->IsAlignToVelocity() == false))
					pButton->EnableWindow(false);
				break;
			case IDC_ALIGN_TO_VEL_CHECK:
				if (m_pParticle->IsFlagSet(PART_FLOATS_ON_WATER))
					pButton->EnableWindow(false);
				break;
			}
/************** end special cases *******************/
		}
		pCheckBox++;
	}

	/* set contents of radio buttons */

	ts_RadioButton *pRadioButton = RadioButtonList;
	while (pRadioButton->nID != -1)	// id == -1 marks end of list
	{
		CButton *pButton = (CButton *) GetDlgItem(pRadioButton->nID);
		ASSERT(pButton);	// if this hits you have an entry in a dialog box table that isn't in this dialog page
		if (m_pParticle == NULL)
		{
			pButton->SetCheck(0);
			pButton->EnableWindow(false);
		}
		else
		{
			pButton->EnableWindow(true);
			UpdateRadioButton(pRadioButton->nID, false);

/************** special cases ***********************/
			/* special cases for disabling a box based on data state */

			switch(pRadioButton->nID)
			{
			case IDC_ORIENT_SCREEN_RADIO:
			case IDC_ORIENT_FREE_RADIO:
			case IDC_ORIENT_GROUND_RADIO:
				if ((m_pParticle) && (m_pParticle->IsFlagSet(PART_FLOATS_ON_WATER)))
					pButton->EnableWindow(false);
				break;
			}
/************** end special cases *******************/
		}
		pRadioButton++;
	}

	/* enable/disable normal buttons */

	if (m_pParticle == NULL)
		GetDlgItem(IDC_PREVIEW_PARTICLE)->EnableWindow(false);
	else
		GetDlgItem(IDC_PREVIEW_PARTICLE)->EnableWindow(true);
}

/* update edit box's value. if pfDirectValue is non-NULL, use value directly, ignore delta. for easier
implementation i am using floats for everything that goes in an edit box, even if they will ultimately be ints */

void CDisplayPage::UpdateEditBox(int nID, int nDelta, float *pfDirectValue, bool bCheckSpecialCases)
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
		ASSERT(false);	// should always find it, otherwise edit box won't be supported
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

//	if (bCheckSpecialCases)
//	{
///************** special cases ***********************/
//		switch(pEditBox->nID)
//		{
//		}
///************** end special cases *******************/
//	}

	/* has doc changed? */

	if (*pfValue != fOldValue)
	{
		CPigpenDoc *pDoc = m_pParentView->GetDocument();
		pDoc->SetModifiedFlag(MODIFIED_FLAG);
		Preview__UpdateParticle(m_pParticle);
	}
}

/* update check box's state. check/uncheck depending on proper bit's state in proper flag unsigned long word */

void CDisplayPage::UpdateCheckBox(int nID, bool bToggle)
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
		ASSERT(false);	// should always find it, otherwise edit box won't be supported
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
	pButton->SetCheck((*pulFlags & pCheckBox->ulFlagBit) ? true : false);

	/* has doc changed? */

	if (bToggle)
	{
		CPigpenDoc *pDoc = m_pParentView->GetDocument();
		pDoc->SetModifiedFlag(MODIFIED_FLAG);
		Preview__UpdateParticle(m_pParticle);
	}
}

/* update radio button's. check/uncheck depending on if bitfield value matches radio button's assigned value */

void CDisplayPage::UpdateRadioButton(int nID, bool bUpdate)
{
	unsigned long *pulBitfield = NULL;
	unsigned long ulOldBitfield;

	/* should be working on valid data */

	ASSERT(m_pParticle != NULL);

	/* find correct entry */

	ts_RadioButton *pRadioButton = RadioButtonList;
	while ((pRadioButton->nID != nID) && (pRadioButton->nID != -1))	// id == -1 marks end of list
		pRadioButton++;
	if (pRadioButton->nID == -1)
	{
		ASSERT(false);	// should always find it, otherwise edit box won't be supported
		return;
	}

	/* get pointer to data item */

	pulBitfield = (unsigned long *) m_pParticle->GetDataPointer(pRadioButton->nDataID);
	ASSERT(pulBitfield);
	ulOldBitfield = *pulBitfield;

	/* set bitfield to proper value */

	if (bUpdate)
	{
		ASSERT((pRadioButton->ulValue & ~pRadioButton->ulMask) == 0);	// value should never peek out from behind mask
		*pulBitfield &= ~pRadioButton->ulMask;
		*pulBitfield |= pRadioButton->ulValue;
	}

	/* show state of flag bit */

	CButton *pButton = (CButton *) GetDlgItem(nID);
	pButton->SetCheck(((*pulBitfield & pRadioButton->ulMask) == pRadioButton->ulValue) ? true : false);

	/* has doc changed? */

	if (*pulBitfield != ulOldBitfield)
	{
		CPigpenDoc *pDoc = m_pParentView->GetDocument();
		pDoc->SetModifiedFlag(MODIFIED_FLAG);
		Preview__UpdateParticle(m_pParticle);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDisplayPage message handlers

/* intercept spin control messages to update associated edit box value */

bool CDisplayPage::OnSpinner(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/* not if no particle */

	if (m_pParticle == NULL)
	{
		*pResult = 1;	// non-zero means abort further processing
		return(false);
	}

	/* find id of edit box associated with this spinner */

	CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*) GetDlgItem(((NM_UPDOWN*) pNMHDR)->hdr.idFrom);
	ASSERT(pSpin);
	CEdit *pEdit = (CEdit *) pSpin->GetBuddy();
	ASSERT(pEdit);
	int nID = pEdit->GetDlgCtrlID();

	/* update that edit box */

	UpdateEditBox(nID, ((NM_UPDOWN*) pNMHDR)->iDelta, NULL, true);

	/* done */

	*pResult = 0;
	return(true);	// processed message
}

/* catch user editing box directly */

bool CDisplayPage::OnKillFocusEdit(int nID) 
{
	CEdit *pEdit = (CEdit *) GetDlgItem(nID);
	if (m_pParticle == NULL)
	{
		pEdit->SetWindowText("N/A");
		pEdit->EnableWindow(false);
	}
	else
	{
		/* get input string, convert to number */

		CString String;
		pEdit->GetWindowText(String);
		float fValue = (float) atof(String);

		/* update edit box. check for invalid string */

		float *pfValue = &fValue;
		if ((fValue == 0.0) && (AllZeros(String) == false))
			pfValue = NULL;	// illegal entry, revert back to value
		UpdateEditBox(nID, 0, pfValue, true);
	}
	return(true);	// processed message
}

/* change selected particle for channel */

bool CDisplayPage::OnSelChange(int nID)
{
	/* get new selection from combo box */

	ASSERT(m_pParticle);
	CComboBox* pComboBox = (CComboBox*) GetDlgItem(nID);
	ASSERT(pComboBox);
	int nListIndex = pComboBox->GetCurSel();
	CGraphic *pNewGraphic = NULL;
	CPigpenDoc *pDoc = m_pParentView->GetDocument();
	int nOldHardwiredGraphic = m_pParticle->GetHardwiredGraphic();
	switch(nListIndex)
	{
	case PARTICLE_HARDWIRED_GRAPHIC_NONE_IN_LIST:
		m_pParticle->SetHardwiredGraphic(PARTICLE_HARDWIRED_GRAPHIC_NONE);
		break;
	case PARTICLE_HARDWIRED_GRAPHIC_FRONT_BUFFER_IN_LIST:
		m_pParticle->SetHardwiredGraphic(PARTICLE_HARDWIRED_GRAPHIC_FRONT_BUFFER);
		m_pParticle->ClearFlag(PART_LIGHTED);	// no real-time lighting for distortion particles
		break;
	case PARTICLE_HARDWIRED_GRAPHIC_BACK_BUFFER_IN_LIST:
		m_pParticle->SetHardwiredGraphic(PARTICLE_HARDWIRED_GRAPHIC_BACK_BUFFER);
		m_pParticle->ClearFlag(PART_LIGHTED);	// no real-time lighting for distortion particles
		break;
	default:
		pNewGraphic = pDoc->GetGraphic(nListIndex - PARTICLE_HARDWIRED_GRAPHIC_LIST_COUNT);	// first entries are hardwired
		break;
	}

	/* remember old graphic, set new one */

	CGraphic *pOldGraphic = m_pParticle->GetGraphic();
	m_pParticle->SetGraphic(pNewGraphic);

	/* transfer default values if no old graphic */

	if ((pOldGraphic == NULL) && (pNewGraphic != NULL))
	{
		m_pParticle->SetFPS(pNewGraphic->GetDefaultFPS());
		m_pParticle->SetXSize(pNewGraphic->GetDefaultXSize());
		m_pParticle->SetYSize(pNewGraphic->GetDefaultYSize());
		m_pParticle->SetMaintainAspectRatio(pNewGraphic->IsMaintainAspectRatio());
	}

	/* if doc has changed, update effects view */

	if ((pOldGraphic != pNewGraphic) || (nOldHardwiredGraphic != m_pParticle->GetHardwiredGraphic()))
	{
		pDoc->SetModifiedFlag(MODIFIED_FLAG);
		pDoc->UpdateAllViews(NULL, NO_CHANGE_EMITTERS_VIEW | NO_CHANGE_EFFECTS_VIEW | NO_REORDER_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_CHANGE_SWOOSHES_VIEW);
		Preview__UpdateParticle(m_pParticle);
	}
	return(true);	// processed message
}

/* catch button clicks. this routine does not have to handle all button clicks, but needs to return false if
it gets passed a button it doesn't understand or want to deal with */

bool CDisplayPage::OnButtonClicked(int nID)
{
	/* normal buttons */

	switch(nID)
	{
	case IDC_PREVIEW_PARTICLE:
		Preview__Create(m_pParentView->GetDocument(), NULL, NULL, m_pParticle, NULL, NULL);
		return(true);	// processed message

	case IDC_IMAGE_BUTTON:
		/* clicking on graphic image jumps to that graphic */

		if ((m_pParticle) && (m_pParticle->GetGraphic()))
		{
			CPigpenDoc *pDoc = m_pParentView->GetDocument();

			/* open view if necessary */

			if (pDoc->GetView(GRAPHICS) == NULL)
				pDoc->OnView(GRAPHICS);

			/* update view */

			CDocView *pView = pDoc->GetView(GRAPHICS);
			if (pView)
			{
				/* select correct entry */

				pView->FindAndSelectItem(m_pParticle->GetGraphic());

				/* open edit window if necessary */

				pView->OnDoubleClick(NULL, NULL);	// opens window if not open and valid item selected

				/* bring edit window to top */

				if (pView->GetEditWindow(0))
					pView->GetEditWindow(0)->BringWindowToTop();

				/* update graphic windows */

				pDoc->UpdateAllViews(NULL, NO_CHANGE_EMITTERS_VIEW | NO_CHANGE_EFFECTS_VIEW | NO_CHANGE_PARTICLES_VIEW | NO_REORDER_GRAPHICS_VIEW | NO_CHANGE_SWOOSHES_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW);
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
			UpdateCheckBox(nID, true);

/************** special cases ***********************/
			/* check for additional actions when changing a check box */

			switch(nID)
			{
			case IDC_ALIGN_TO_VEL_CHECK:
				if ((m_pParticle) && (m_pParticle->IsFlagSet(PART_ALIGN_TO_VELOCITY) == false))
					m_pParticle->ClearFlag(PART_PERPENDICULAR_TO_VELOCITY);
				break;
			case IDC_PART_FLOATS_CHECK:
				if ((m_pParticle) && (m_pParticle->IsFlagSet(PART_FLOATS_ON_WATER)))
				{
					m_pParticle->SetOrient(PART_ORIENT_FREE);
					m_pParticle->ClearFlag(PART_ALIGN_TO_VELOCITY);
				}
				break;
			}
/************** end special cases *******************/

			/* need to re-show data? this is useful for enabling/disabling buttons after check box state changes */

			if (pCheckBox->bShowData)
				ShowData();

			return(true);	// processed message
		}
		pCheckBox++;
	}

	/* radio buttons can all be handled by the same routine */

	ts_RadioButton *pRadioButton = RadioButtonList;
	while (pRadioButton->nID != -1)	// id == -1 marks end of list
	{
		if (nID == pRadioButton->nID)
		{
			UpdateRadioButton(nID, true);

/************** special cases ***********************/
			/* check for additional actions when changing a radio button */

			switch(nID)
			{
			case IDC_ORIENT_SCREEN_RADIO:
				if (m_pParticle)
				{
					m_pParticle->ClearFlag(PART_LIGHTED);
					m_pParticle->ClearFlag(PART_PERPENDICULAR_TO_VELOCITY);
				}
				break;
			case IDC_ORIENT_GROUND_RADIO:
				m_pParticle->ClearFlag(PART_PERPENDICULAR_TO_VELOCITY);
				break;
			}
/************** end special cases *******************/

			/* need to re-show data? this is useful for enabling/disabling buttons after button state changes */

			if (pRadioButton->bShowData)
				ShowData();

			return(true);	// processed message
		}
		pRadioButton++;
	}

	/* didn't handle the button */

	return(false);
}

/* called when owner-draw flagged dialog buttons are going to be drawn. used to draw graphic images */

void CDisplayPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	/* bitmap or empty frame? */

	CImage *pImage = NULL;
	bool bShowAlphaMask;
	if (m_pParticle)
		pImage = m_pParticle->GetImage(&bShowAlphaMask);
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

BOOL CDisplayPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	/* if wParam != 0, wParam is ID of control that sent message */

	if (wParam != 0)
	{
		NMHDR *pNMHDR = (NMHDR *) lParam;
		switch(pNMHDR->code)
		{
		case UDN_DELTAPOS:
			/* spinner */

			if (OnSpinner(pNMHDR, pResult) == true)
				return(true);	// processed message
//not needed for this dialog at this point
//		case NM_CUSTOMDRAW:
//			/* slider */
//
//			if (OnCustomdrawSlider(pNMHDR, pResult) == true)
//				return(true);	// processed message
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

BOOL CDisplayPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch((wParam >> 16) & 0xffff)
	{
	case EN_KILLFOCUS:
		/* user has finished editing text in an edit box */

		if (OnKillFocusEdit(wParam & 0xffff) == true)
			return(true);	// processed message
		break;
	case CBN_SELCHANGE:
		/* user has selected a new entry in combo box */

		if (OnSelChange(wParam & 0xffff) == true)
			return(true);	// processed message
		break;
	case BN_CLICKED:
		/* user has clicked a button. check boxes are buttons, too */

		if (OnButtonClicked(wParam & 0xffff) == true)
			return(true);	// processed message
		break;
	}

	/* let base class handle anything else */

	return CPropertyPage::OnCommand(wParam, lParam);
}

/* called when page is chosen as active page */

BOOL CDisplayPage::OnSetActive()
{
	/* first call base class */

	CPropertyPage::OnSetActive();

	/* then show page */

	ShowData();
	return(true);
}
