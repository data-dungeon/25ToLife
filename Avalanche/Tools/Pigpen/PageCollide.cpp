/***********************************/
/*                                 */
/* PageColllide.cpp                */
/* Pigpen particle tool  1/3/01    */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle colllide property page */
/*                                 */
/***********************************/

/* this is the colllide page of the particle property sheet. each page is a dialog */

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

/******************* macros ******************************/

/* user is allowed to edit the spawn portion of the dialog under certain conditions. usually it is
because he has checked "particle collides" and is working on an active surface type. but end-of-life
"surface types" don't depend on collisions to happen (they happen when the particle dies when it
reaches the end of its lifetime), so the user doesn't have to have checked the "particle collides"
box to provide spawn info */

#define PARTICLE_SPAWN_EDITABLE()	(((m_pParticle->CollidesValid()) && (m_pParticle->GetCurrentSpawn()->UseDefault() == false)) || (m_pParticle->GetCurrentSpawn()->EndOfLife()))
#define PARTICLE_SPAWNS_CHECKED()	((PARTICLE_SPAWN_EDITABLE()) && (m_pParticle->GetCurrentSpawn()->Spawns()))

/******************* structures **************************/

/******************* local variables *********************/

/* dialog item lists */

static ts_EditBox EditBoxList[] = {
	//	control id						data id								 index	min		max				delta		format	bIsInteger
	{	IDC_COLLIDE_BOUNCE_EDIT,	CParticle::DATA_COLLIDE_BOUNCE,	-1,	0.0f,		MAX_P_BOUNCE,	0.1f,		"%.2f",	FALSE},
	{	IDC_SPAWN_BOUNCE_EDIT,		CParticle::DATA_SPAWN_BOUNCE,		-1,	0.0f,		MAX_P_BOUNCE,	0.1f,		"%.2f",	FALSE},
	
	// end of list-- must be last
	{	-1}
};

static ts_CheckBox CheckBoxList[] = {
	//	control id								data id									index	flag bit										bShowData
	{	IDC_PART_COLLIDES,					CParticle::DATA_COLLIDE_FLAGS,	-1,	PART_COLLIDES,								TRUE},
	{	IDC_COLLIDE_STOP_ROTATING,			CParticle::DATA_COLLIDE_FLAGS,	-1,	PART_COLLIDE_STOP_ROTATING,			FALSE},
	{	IDC_COLLIDE_STOP_ANIMATING,		CParticle::DATA_COLLIDE_FLAGS,	-1,	PART_COLLIDE_STOP_ANIMATING,			FALSE},
	{	IDC_COLLIDE_SPAWN,					CParticle::DATA_SPAWN_FLAGS,		-1,	SPAWN_EFFECT_ACTIVE,						TRUE},
	{	IDC_SURFACE_USE_DEFAULT,			CParticle::DATA_SPAWN_FLAGS,		-1,	SPAWN_USE_DEFAULT_SURFACE,				TRUE},
	{	IDC_PART_COLLIDES_WORLD,			CParticle::DATA_COLLIDE_FLAGS,	-1,	PART_COLLIDES_WORLD,						TRUE},
	{	IDC_PART_COLLIDES_ACTORS,			CParticle::DATA_COLLIDE_FLAGS,	-1,	PART_COLLIDES_ACTORS,					TRUE},
	{	IDC_PART_COLLIDES_WATER,			CParticle::DATA_COLLIDE_FLAGS,	-1,	PART_COLLIDES_WATER,						TRUE},
	{	IDC_COLLIDE_CREATE_WATER_SPRITE, CParticle::DATA_COLLIDE_FLAGS,	-1,	PART_COLLIDE_CREATE_WATER_SPRITE,	TRUE},
	
	// end of list-- must be last
	{	-1}
};

static ts_RadioButton RadioButtonList[] = {
	//	control id					data id									value							mask						bShowData
	{	IDC_COLLIDE_DIE,			CParticle::DATA_BITFIELDS,			PART_COLLIDE_DIE,			PART_COLLIDE_MASK,	TRUE},
	{	IDC_COLLIDE_BOUNCE,		CParticle::DATA_BITFIELDS,			PART_COLLIDE_BOUNCE,		PART_COLLIDE_MASK,	TRUE},
	{	IDC_COLLIDE_STICK,		CParticle::DATA_BITFIELDS,			PART_COLLIDE_STICK,		PART_COLLIDE_MASK,	TRUE},
	{	IDC_COLLIDE_CONTINUE,	CParticle::DATA_BITFIELDS,			PART_COLLIDE_CONTINUE,	PART_COLLIDE_MASK,	TRUE},
	{	IDC_SPAWN_DEFAULT,		CParticle::DATA_SPAWN_BITFIELDS,	SPAWN_TYPE_DEFAULT,		SPAWN_TYPE_MASK,		TRUE},
	{	IDC_SPAWN_BOUNCE,			CParticle::DATA_SPAWN_BITFIELDS,	SPAWN_TYPE_BOUNCE,		SPAWN_TYPE_MASK,		TRUE},
	{	IDC_SPAWN_STICK,			CParticle::DATA_SPAWN_BITFIELDS,	SPAWN_TYPE_STICK,			SPAWN_TYPE_MASK,		TRUE},
	{	IDC_SPAWN_CONTINUE,		CParticle::DATA_SPAWN_BITFIELDS,	SPAWN_TYPE_CONTINUE,		SPAWN_TYPE_MASK,		TRUE},

	// end of list-- must be last
	{	-1}
};

/******************* externs *****************************/

/******************* class functions *********************/
 
/* i am trying to avoid long message maps, so i have overridden OnNotify and OnCommand. they catch slider bars,
spinners, edit boxes, and button clicks for me and process them more generically. the message map stuff still works,
but in general, use the higher-level intercept routines (like OnKillFocusEdit to catch all edit box commands) */

BEGIN_MESSAGE_MAP(CCollidePage, CPropertyPage)
	//{{AFX_MSG_MAP(CCollidePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCollidePage construction/destruction

CCollidePage::CCollidePage(CDocView *pParentView, CParticleSheet *pParentSheet) : CPropertyPage(CCollidePage::IDD)
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

	//{{AFX_DATA_INIT(CCollidePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
// CCollidePage operations

/* fill in the data in the dialog box with the current particle */

void CCollidePage::ShowData(void)
{
	/* get current particle from parent sheet */

	m_pParticle = m_pParentSheet->GetParticle();
	CPigpenDoc *pDoc = m_pParentView->GetDocument();

	/* point to working spawn record */

	if (m_pParticle)
		m_pParticle->SetCurrentSpawn(pDoc->GetSurfaceIndex());

	/* surface combo box has list of available surface types to define behavior for */

	CComboBox *pSurfaceComboBox = (CComboBox *) GetDlgItem(IDC_SURFACE_COMBO);
	pSurfaceComboBox->ResetContent();	// clear contents
	if (m_pParticle == NULL)
		pSurfaceComboBox->EnableWindow(FALSE);
	else
	{
		pSurfaceComboBox->EnableWindow(TRUE);
		for (int i = 0; i < pDoc->GetSurfaceCount(); i++)
		{
			CSurface *pSurface = pDoc->GetSurface(i);
			pSurfaceComboBox->AddString(pSurface->GetName());
		}
	}
	pSurfaceComboBox->SetCurSel(pDoc->GetSurfaceIndex());

	/* effect combo box has list of available particles to spawn */

	CComboBox *pEffectComboBox = (CComboBox *) GetDlgItem(IDC_SPAWN_COMBO);
	pEffectComboBox->ResetContent();	// clear contents
	if ((m_pParticle == NULL) || (PARTICLE_SPAWNS_CHECKED() == FALSE))
		pEffectComboBox->EnableWindow(FALSE);
	else
	{
		pEffectComboBox->EnableWindow(TRUE);
		pEffectComboBox->AddString("None");
		for (int i = 0; i < pDoc->GetEffectCount(); i++)
		{
			CEffect *pEffect = pDoc->GetEffect(i);
			pEffectComboBox->AddString(pEffect->GetName());
			if (m_pParticle->GetCurrentSpawn()->GetEffect() == pEffect)
				pEffectComboBox->SetCurSel(i + 1);	// skip the "none" at start of list
		}

		/* if no particle selected, set current selected to none */

		if (m_pParticle->GetCurrentSpawn()->GetEffect() == NULL)
			pEffectComboBox->SetCurSel(0);
	}

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
			case IDC_COLLIDE_BOUNCE_EDIT:
				if ((m_pParticle->CollidesValid() == FALSE) ||
					 (pDoc->CurrentSurface()->EndOfLife()) ||
					 ((m_pParticle->GetCollideType() != PART_COLLIDE_BOUNCE) &&
					  (m_pParticle->GetCollideType() != PART_COLLIDE_CONTINUE)))
					pEdit->EnableWindow(FALSE);
				break;
			case IDC_SPAWN_BOUNCE_EDIT:
				if ((PARTICLE_SPAWNS_CHECKED() == FALSE) ||
					 ((m_pParticle->GetCurrentSpawn()->Type() != SPAWN_TYPE_BOUNCE) &&
					  (m_pParticle->GetCurrentSpawn()->Type() != SPAWN_TYPE_CONTINUE)))
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
			case IDC_SURFACE_USE_DEFAULT:
				/* pre-defined surface types don't use default by definition active, can't be changed */

				if (pDoc->CurrentSurface()->Predefined())
					pButton->EnableWindow(FALSE);
				break;
			case IDC_COLLIDE_SPAWN:
				if (PARTICLE_SPAWN_EDITABLE() == false)
					pButton->EnableWindow(FALSE);
				break;
			case IDC_COLLIDE_STOP_ROTATING:
			case IDC_COLLIDE_STOP_ANIMATING:
				/* collision response settings don't make sense for end-of-life spawning */

				if ((m_pParticle->CollidesValid() == FALSE) || (pDoc->CurrentSurface()->EndOfLife()))
					pButton->EnableWindow(FALSE);
				break;
			case IDC_PART_COLLIDES_WORLD:
			case IDC_PART_COLLIDES_ACTORS:
			case IDC_PART_COLLIDES_WATER:
				if (m_pParticle->Collides() == FALSE)
					pButton->EnableWindow(FALSE);
				break;
			case IDC_COLLIDE_CREATE_WATER_SPRITE:
				if ((m_pParticle->Collides() == FALSE) || (m_pParticle->CollidesWithWater() == FALSE))
					pButton->EnableWindow(FALSE);
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
			pButton->EnableWindow(FALSE);
		}
		else
		{
			pButton->EnableWindow(TRUE);
			UpdateRadioButton(pRadioButton->nID, FALSE);

/************** special cases ***********************/
			/* special cases for disabling a box based on data state */

			switch(pRadioButton->nID)
			{
			case IDC_COLLIDE_DIE:
			case IDC_COLLIDE_BOUNCE:
			case IDC_COLLIDE_STICK:
			case IDC_COLLIDE_CONTINUE:
				/* collision response settings don't make sense for end-of-life spawning */

				if ((m_pParticle->CollidesValid() == FALSE) || (pDoc->CurrentSurface()->EndOfLife()))
					pButton->EnableWindow(FALSE);
				break;
			case IDC_SPAWN_DEFAULT:
			case IDC_SPAWN_BOUNCE:
			case IDC_SPAWN_STICK:
			case IDC_SPAWN_CONTINUE:
				if (PARTICLE_SPAWNS_CHECKED() == false)
					pButton->EnableWindow(FALSE);
				break;
			}
/************** end special cases *******************/
		}
		pRadioButton++;
	}

	/* enable/disable normal buttons */

	if (m_pParticle == NULL)
		GetDlgItem(IDC_PREVIEW_PARTICLE)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_PREVIEW_PARTICLE)->EnableWindow(TRUE);
}

/* update edit box's value. if pfDirectValue is non-NULL, use value directly, ignore delta. for easier
implementation i am using floats for everything that goes in an edit box, even if they will ultimately be ints */

void CCollidePage::UpdateEditBox(int nID, int nDelta, float *pfDirectValue, bool bCheckSpecialCases)
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

void CCollidePage::UpdateCheckBox(int nID, bool bToggle)
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

/* update radio button's. check/uncheck depending on if bitfield value matches radio button's assigned value */

void CCollidePage::UpdateRadioButton(int nID, bool bUpdate)
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
		ASSERT(FALSE);	// should always find it, otherwise edit box won't be supported
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
	pButton->SetCheck(((*pulBitfield & pRadioButton->ulMask) == pRadioButton->ulValue) ? TRUE : FALSE);

	/* has doc changed? */

	if (*pulBitfield != ulOldBitfield)
	{
		CPigpenDoc *pDoc = m_pParentView->GetDocument();
		pDoc->SetModifiedFlag(MODIFIED_FLAG);
		Preview__UpdateParticle(m_pParticle);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCollidePage message handlers

/* intercept spin control messages to update associated edit box value */

bool CCollidePage::OnSpinner(NMHDR* pNMHDR, LRESULT* pResult) 
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

bool CCollidePage::OnKillFocusEdit(int nID) 
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

/* change selected effect to spawn for particle */

bool CCollidePage::OnSelChange(int nID)
{
	/* get new selection from combo box */

	ASSERT(m_pParticle);
	CComboBox* pComboBox = (CComboBox*) GetDlgItem(nID);
	ASSERT(pComboBox);
	CPigpenDoc *pDoc = m_pParentView->GetDocument();
	int nListIndex = pComboBox->GetCurSel();
	switch(nID)
	{
	case IDC_SURFACE_COMBO:
		pDoc->SetSurfaceIndex(nListIndex);
		ShowData();	// may enable/disable fields
		break;
	case IDC_SPAWN_COMBO:
		CEffect *pNewEffect = NULL;
		if (nListIndex != 0)
			pNewEffect = pDoc->GetEffect(nListIndex - 1);	// "- 1" because first entry is "none"

		/* remember old graphic, set new one */

		CEffect *pOldEffect = m_pParticle->GetCurrentSpawn()->GetEffect();
		m_pParticle->GetCurrentSpawn()->SetEffect(pNewEffect);

		/* if doc has changed, update effects view */

		if (pOldEffect != pNewEffect)
		{
			pDoc->SetModifiedFlag(MODIFIED_FLAG);
			pDoc->UpdateAllViews(NULL, NO_CHANGE_EMITTERS_VIEW | NO_CHANGE_EFFECTS_VIEW | NO_REORDER_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_CHANGE_SWOOSHES_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW);
			Preview__UpdateParticle(m_pParticle);
		}
		break;
	}
	return(TRUE);	// processed message
}

/* catch button clicks. this routine does not have to handle all button clicks, but needs to return FALSE if
it gets passed a button it doesn't understand or want to deal with */

bool CCollidePage::OnButtonClicked(int nID)
{
	/* normal buttons */

	switch(nID)
	{
	case IDC_PREVIEW_PARTICLE:
		Preview__Create(m_pParentView->GetDocument(), NULL, NULL, m_pParticle, NULL, NULL);
		return(TRUE);	// processed message
	case IDC_SURFACE_LIST:
		CSurfaceListDialog SurfaceListDialog(m_pParentView->GetDocument());
		SurfaceListDialog.DoModal();
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

BOOL CCollidePage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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

	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}

/* this catches other control messages. from the help on CWnd::OnCommand, about wParam:

"The low-order word of wParam identifies the command ID of the menu item, control, or accelerator.
The high-order word of wParam specifies the notification message if the message is from a control.
If the message is from an accelerator, the high-order word is 1. If the message is from a menu,
the high-order word is 0." */

BOOL CCollidePage::OnCommand(WPARAM wParam, LPARAM lParam) 
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

	return CPropertyPage::OnCommand(wParam, lParam);
}

/* called when page is chosen as active page */

BOOL CCollidePage::OnSetActive()
{
	/* first call base class */

	CPropertyPage::OnSetActive();

	/* then show page */

	ShowData();
	return(TRUE);
}
