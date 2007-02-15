/***********************************/
/*                                 */
/* EffectDialog.cpp                */
/* Pigpen particle tool  12/09/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* dialog box for editing effects  */
/*                                 */
/***********************************/

/* this is the dialog box that let's the user edit a particle effect */

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
	//	control id				data id								index		min		max		delta					format	bIsInteger
	{	IDC_COUNT_E1,			CEffect::DATA_COUNT,					0,		1.0f,		16.0f,	1.0f,					"%.0f",	TRUE},
	{	IDC_COUNT_RANDOM_E1,	CEffect::DATA_COUNT_RANDOM,		0,		0.0f,		16.0f,	1.0f,					"%.0f",	TRUE},
	{	IDC_DELAY_E1,			CEffect::DATA_EMIT_DELAY,			0,		0.0f,		4.0f,		(1.0f / 60.0f),	"%.3f",	FALSE},
	{	IDC_DELAY_RANDOM_E1,	CEffect::DATA_EMIT_DELAY_RANDOM,	0,		0.0f,		4.0f,		(1.0f / 60.0f),	"%.3f",	FALSE},
	{	IDC_TIME_E1,			CEffect::DATA_EMIT_TIME,			0,		0.0f,		4.0f,		(1.0f / 60.0f),	"%.3f",	FALSE},
	{	IDC_TIME_RANDOM_E1,	CEffect::DATA_EMIT_TIME_RANDOM,	0,		0.0f,		4.0f,		(1.0f / 60.0f),	"%.3f",	FALSE},

	// end of list-- must be last
	{	-1}
};

static ts_CheckBox CheckBoxList[] = {
	//	control id					data id					 index	flag bit									bShowData
	{	IDC_TIME_RANDOM_CHECK1,	CEffect::DATA_FLAGS,		0,		CHANNEL_EMIT_TIME_CONT_RANDOM,	FALSE},
	
	{	IDC_SOUND1,					CEffect::DATA_FLAGS,		0,		CHANNEL_IS_SOUND,						TRUE},

	// end of list-- must be last
	{	-1}
};

static ts_RadioButton RadioButtonList[] = {
	//	control id						data id							value								mask			bShowData

	{	IDC_CHANNEL1,					CEffect::DATA_CHANNEL,		0,									0xff,			TRUE},
	{	IDC_CHANNEL2,					CEffect::DATA_CHANNEL,		1,									0xff,			TRUE},
	{	IDC_CHANNEL3,					CEffect::DATA_CHANNEL,		2,									0xff,			TRUE},
	{	IDC_CHANNEL4,					CEffect::DATA_CHANNEL,		3,									0xff,			TRUE},
	{	IDC_CHANNEL5,					CEffect::DATA_CHANNEL,		4,									0xff,			TRUE},
	{	IDC_CHANNEL6,					CEffect::DATA_CHANNEL,		5,									0xff,			TRUE},
	{	IDC_CHANNEL7,					CEffect::DATA_CHANNEL,		6,									0xff,			TRUE},
	{	IDC_CHANNEL8,					CEffect::DATA_CHANNEL,		7,									0xff,			TRUE},
	{	IDC_CHANNEL9,					CEffect::DATA_CHANNEL,		8,									0xff,			TRUE},
	{	IDC_CHANNEL10,					CEffect::DATA_CHANNEL,		9,									0xff,			TRUE},

	// end of list-- must be last
	{	-1}
};

/* diagram buttons-- the timing alignment bars */

static int DiagramButton[MAX_CHANNELS_PER_EFFECT] = {

	IDC_DIAGRAM_B1,
	IDC_DIAGRAM_B2,
	IDC_DIAGRAM_B3,
	IDC_DIAGRAM_B4,
	IDC_DIAGRAM_B5,
	IDC_DIAGRAM_B6,
	IDC_DIAGRAM_B7,
	IDC_DIAGRAM_B8,
	IDC_DIAGRAM_B9,
	IDC_DIAGRAM_B10,
};

/******************* externs *****************************/

/******************* class functions *********************/

/* i am trying to avoid long message maps, so i have overridden OnNotify and OnCommand. they catch slider bars,
spinners, edit boxes, and button clicks for me and process them more generically. the message map stuff still works,
but in general, use the higher-level intercept routines (like OnKillFocusEdit to catch all edit box commands) */

BEGIN_MESSAGE_MAP(CEffectDialog, CDialog)
	//{{AFX_MSG_MAP(CEffectDialog)
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_DRAWITEM()
	ON_COMMAND(IDC_COPY_CHANNEL, OnCopy)
	ON_COMMAND(IDC_PASTE_CHANNEL, OnPaste)
	ON_COMMAND(IDC_CLEAR_CHANNEL, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectDialog construction/destruction

CEffectDialog::CEffectDialog(CDocView *pParentView, CWnd* pParentWnd) : CDialog(IDD_EFFECT_DIALOG, pParentWnd)
{
	//{{AFX_DATA_INIT(CEffectDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	ASSERT(pParentView != NULL);
	ASSERT(pParentWnd != NULL);

	m_pParentView = pParentView;
	m_pParentWnd = pParentWnd;
	m_pEffect = NULL;
	m_nChannelIndex = 0;

	/* register channel data as a clipboard format */

	m_nClipboardFormat = RegisterClipboardFormat("PigpenChannel");
	ASSERT(m_nClipboardFormat != 0);	// if this is 0, RegisterClipboardFormat failed
	m_nClipboardRecordSize = sizeof(ts_EffectChannel);

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

CEffectDialog::~CEffectDialog()
{
	m_pParentView->EditWindowClosed(EDIT_WINDOW0);
}

/* create a modeless dialog */

BOOL CEffectDialog::Create()
{
	return(CDialog::Create(IDD_EFFECT_DIALOG, m_pParentWnd));
}

/////////////////////////////////////////////////////////////////////////////
// CEffectDialog operations

/* fill in the data in the dialog box with the current effect */

void CEffectDialog::ShowData(CEffect *pEffect)
{
	/* record pointer and contents */

	m_pEffect = pEffect;
	if (m_pEffect)
	{
		m_OriginalEffect = *pEffect;
		m_nChannelIndex = pEffect->GetChannelIndex();
	}

	/* title bar is name of current effect */

	if (m_pEffect == NULL)
		SetWindowText("Effect: NONE SELECTED");
	else
		SetWindowText("Effect: " + m_pEffect->GetName());

	/* fill in channel */

	CPigpenDoc *pDoc = m_pParentView->GetDocument();

	/* combo box has list of available particles or sounds */

	CComboBox *pComboBox = (CComboBox *) GetDlgItem(IDC_PARTICLE_C1);
	pComboBox->ResetContent();	// clear contents
	if (m_pEffect == NULL)
		pComboBox->EnableWindow(FALSE);
	else
	{
		pComboBox->EnableWindow(TRUE);
		if (m_pEffect->ChannelIsSound(m_nChannelIndex))
		{
			/* add sounds */

			for (int j = 0; j < GetSoundListCount(); j++)
				pComboBox->AddString(GetSoundName(j));

			/* set current index */

			pComboBox->SetCurSel(GetSoundListIndex(m_pEffect->GetChannelSoundName(m_nChannelIndex)));
		}
		else
		{
			/* add particle names */

			pComboBox->AddString("None");
			for (int j = 0; j < pDoc->GetParticleCount(); j++)
			{
				CParticle *pParticle = pDoc->GetParticle(j);
				pComboBox->AddString(pParticle->GetName());
				if (m_pEffect->GetChannelParticle(m_nChannelIndex) == pParticle)
					pComboBox->SetCurSel(j + 1);	// skip the "none" at start of list
			}

			/* if no particle selected, set current selected to none */

			if (m_pEffect->GetChannelParticle(m_nChannelIndex) == NULL)
				pComboBox->SetCurSel(0);
		}
	}

	/* paint image/diagram buttons */

	CButton *pButton;
	pButton = (CButton *) GetDlgItem(IDC_IMAGE_B1);
	pButton->Invalidate();	// so it gets repainted, intercepted by OnDrawItem
	for (int i = 0; i < MAX_CHANNELS_PER_EFFECT; i++)
	{
		pButton = (CButton *) GetDlgItem(DiagramButton[i]);
		pButton->Invalidate();	// so it gets repainted, intercepted by OnDrawItem
	}

	/* set contents of edit boxes */

	ts_EditBox *pEditBox = EditBoxList;
	while (pEditBox->nID != -1)	// id == -1 marks end of list
	{
		CEdit *pEdit = (CEdit *) GetDlgItem(pEditBox->nID);
		ASSERT(pEdit);		// if this hits you have an entry in a dialog box table that isn't in this dialog page
		if (m_pEffect == NULL)
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
		if (m_pEffect == NULL)
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
		if (m_pEffect == NULL)
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

	/* enable/disable normal button */

	if (m_pEffect == NULL)
		GetDlgItem(IDC_PREVIEW)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_PREVIEW)->EnableWindow(TRUE);
}

/* update edit box's value. if desired, redraws channel's graphic display if count, emit delay, or emit time
was changed. if pfDirectValue is non-NULL, set value directly. for easier implementation i am using floats for
everything that goes in an edit box, even if they will ultimately be ints */

void CEffectDialog::UpdateEditBox(int nID, int nDelta, float *pfDirectValue, bool bCheckSpecialCases)
{
	float *pfValue = NULL;
	float fOldValue;
	char acString[20];

	/* should be working on valid data */

	ASSERT(m_pEffect != NULL);

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

	pfValue = (float *) m_pEffect->GetDataPointer(pEditBox->nDataID, m_nChannelIndex);
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
		Preview__UpdateEffect(m_pEffect);

		/* redraw graphic diagram if desired. changing random values doesn't affect diagram */
	
		if ((pEditBox->nDataID == CEffect::DATA_COUNT) || (pEditBox->nDataID == CEffect::DATA_EMIT_DELAY) || (pEditBox->nDataID == CEffect::DATA_EMIT_TIME))
		{
			CButton *pButton = (CButton *) GetDlgItem(DiagramButton[m_nChannelIndex]);
			pButton->Invalidate();	// so it gets repainted, intercepted by OnDrawItem
		}
	}
}

/* update check box's state. check/uncheck depending on proper bit's state in proper flag unsigned long word */

void CEffectDialog::UpdateCheckBox(int nID, bool bToggle)
{
	unsigned long *pulFlags = NULL;

	/* should be working on valid data */

	ASSERT(m_pEffect != NULL);

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

	pulFlags = (unsigned long *) m_pEffect->GetDataPointer(pCheckBox->nDataID, m_nChannelIndex);
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
		Preview__UpdateEffect(m_pEffect);
	}
}

/* update radio button's. check/uncheck depending on if bitfield value matches radio button's assigned value */

void CEffectDialog::UpdateRadioButton(int nID, bool bUpdate)
{
	unsigned long *pulBitfield = NULL;
	unsigned long ulOldBitfield;

	/* should be working on valid data */

	ASSERT(m_pEffect != NULL);

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

	pulBitfield = (unsigned long *) m_pEffect->GetDataPointer(pRadioButton->nDataID, 0);
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
		Preview__UpdateEffect(m_pEffect);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEffectDialog message handlers

/* called when OK is selected, or when user presses enter. i don't want pressing enter to end the dialog, so
override and do nothing */

void CEffectDialog::OnOK()
{
}

/* called when dialog is closed by sysmenu close button (X) */

void CEffectDialog::OnCancel()
{
	/* from the help on OnCancel(): "If you implement the Cancel button in a modeless dialog box, you must */
	/* override the OnCancel member function and call DestroyWindow from within it. Don’t call the base-class */
	/* member function, because it calls EndDialog, which will make the dialog box invisible but not destroy it." */

	DestroyWindow();
}

/* called after window is destroyed */

void CEffectDialog::PostNcDestroy()
{
	delete this;	// dialog was created by parent window using "new"
}

/* handle popup menu for dialog */

void CEffectDialog::OnContextMenu(CWnd* pWnd, CPoint Point) 
{
	static int nCopyID[MAX_CHANNELS_PER_EFFECT] = {
		IDC_COPY_CHANNEL1,
		IDC_COPY_CHANNEL2,
		IDC_COPY_CHANNEL3,
		IDC_COPY_CHANNEL4,
		IDC_COPY_CHANNEL5};
	static int nPasteID[MAX_CHANNELS_PER_EFFECT] = {
		IDC_PASTE_CHANNEL1,
		IDC_PASTE_CHANNEL2,
		IDC_PASTE_CHANNEL3,
		IDC_PASTE_CHANNEL4,
		IDC_PASTE_CHANNEL5};
	static int nClearID[MAX_CHANNELS_PER_EFFECT] = {
		IDC_CLEAR_CHANNEL1,
		IDC_CLEAR_CHANNEL2,
		IDC_CLEAR_CHANNEL3,
		IDC_CLEAR_CHANNEL4,
		IDC_CLEAR_CHANNEL5};

	CMenu PopupMenu;
	if (PopupMenu.LoadMenu(IDR_POPUP_MENUS))
	{
		CMenu *pSubMenu = PopupMenu.GetSubMenu(EFFECT_DIALOG_POPUP_MENU);

		/* enable/disable menu items */

		int nPasteState = MF_GRAYED;
		if ((m_pEffect) && (IsClipboardFormatAvailable(m_nClipboardFormat)))
			nPasteState = MF_ENABLED;
		for (int i = 0; i < MAX_CHANNELS_PER_EFFECT; i++)
		{
			pSubMenu->EnableMenuItem(nCopyID[i], MF_ENABLED);
			pSubMenu->EnableMenuItem(nPasteID[i], nPasteState);
			pSubMenu->EnableMenuItem(nClearID[i], (m_pEffect) ? MF_ENABLED : MF_GRAYED);
		}

		/* show the menu */

		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, Point.x, Point.y, this);
	}
}

/* intercept spin control messages to update associated edit box value */

bool CEffectDialog::OnSpinner(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/* not if no effect */

	if (m_pEffect == NULL)
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

/* change selected particle for channel */

bool CEffectDialog::OnSelChange(int nID)
{
	/* point to correct particle within effect */

	switch(nID)
	{
	case IDC_PARTICLE_C1:
		break;
	default:
		ASSERT(FALSE);	// all cases should be handled
		return(FALSE);	// didn't process message
	}
	ts_EffectChannel *pChannel = &m_pEffect->m_Channels[m_nChannelIndex];

	/* get new selection from combo box */

	CComboBox* pComboBox = (CComboBox*) GetDlgItem(nID);
	ASSERT(pComboBox);
	CPigpenDoc *pDoc = m_pParentView->GetDocument();
	int nListIndex = pComboBox->GetCurSel();
	if (m_pEffect->ChannelIsSound(m_nChannelIndex))
	{
		/* we keep track of sound assignment via name */

		m_pEffect->SetChannelSoundName(m_nChannelIndex, GetSoundName(nListIndex));
	}
	else
	{
		if (nListIndex == 0)	// none
			pChannel->pParticle = NULL;
		else
			pChannel->pParticle = pDoc->GetParticle(nListIndex - 1);	// "- 1" because first entry is "none"

		/* update image button */

		CButton *pButton = (CButton *) GetDlgItem(IDC_IMAGE_B1);
		pButton->Invalidate();	// so it gets repainted, intercepted by OnDrawItem
	}

	/* doc has changed, update effects view */

	pDoc->SetModifiedFlag(MODIFIED_FLAG);
	pDoc->UpdateAllViews(NULL, NO_CHANGE_EMITTERS_VIEW | NO_REORDER_EFFECTS_VIEW | NO_CHANGE_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_CHANGE_SWOOSHES_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW);
	Preview__UpdateEffect(m_pEffect);
	return(TRUE);	// processed message
}

/* catch button clicks. this routine does not have to handle all button clicks, but needs to return FALSE if
it gets passed a button it doesn't understand or want to deal with */

bool CEffectDialog::OnButtonClicked(int nID)
{
	/* normal buttons */

	switch(nID)
	{
	case IDC_PREVIEW:
		if (m_pEffect)
			Preview__Create(m_pParentView->GetDocument(), NULL, m_pEffect, NULL, NULL, NULL);
		return(true);	// processed message

	case IDC_IMAGE_B1:
		/* clicking on particle image jumps to that particle */

		if (m_pEffect)
		{
			ts_EffectChannel *pChannel = &m_pEffect->m_Channels[m_nChannelIndex];
			if (pChannel->pParticle)
			{
				CPigpenDoc *pDoc = m_pParentView->GetDocument();

				/* open view if necessary */

				if (pDoc->GetView(PARTICLES) == NULL)
					pDoc->OnView(PARTICLES);

				/* update view */

				CDocView *pView = pDoc->GetView(PARTICLES);
				if (pView)
				{
					/* select correct entry */

					pView->FindAndSelectItem(pChannel->pParticle);

					/* open edit window if necessary */

					pView->OnDoubleClick(NULL, NULL);	// opens window if not open and valid item selected

					/* bring edit window to top */

					if (pView->GetEditWindow(0))
						pView->GetEditWindow(0)->BringWindowToTop();

					/* update particle windows */

					pDoc->UpdateAllViews(NULL, NO_CHANGE_EMITTERS_VIEW | NO_CHANGE_EFFECTS_VIEW | NO_REORDER_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_CHANGE_SWOOSHES_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW);
				}
			}
		}
		return(true);	// processed message

	/* clicking on a diagram box has the same effect as clicking on a radio button */

	case IDC_DIAGRAM_B1:
		nID = IDC_CHANNEL1;
		break;
	case IDC_DIAGRAM_B2:
		nID = IDC_CHANNEL2;
		break;
	case IDC_DIAGRAM_B3:
		nID = IDC_CHANNEL3;
		break;
	case IDC_DIAGRAM_B4:
		nID = IDC_CHANNEL4;
		break;
	case IDC_DIAGRAM_B5:
		nID = IDC_CHANNEL5;
		break;
	case IDC_DIAGRAM_B6:
		nID = IDC_CHANNEL6;
		break;
	case IDC_DIAGRAM_B7:
		nID = IDC_CHANNEL7;
		break;
	case IDC_DIAGRAM_B8:
		nID = IDC_CHANNEL8;
		break;
	case IDC_DIAGRAM_B9:
		nID = IDC_CHANNEL9;
		break;
	case IDC_DIAGRAM_B10:
		nID = IDC_CHANNEL10;
		break;
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

			switch(nID)
			{
			case IDC_SOUND1:
			case IDC_SOUND2:
			case IDC_SOUND3:
			case IDC_SOUND4:
			case IDC_SOUND5:
				/* changed channel from particle to sound or vice versa */

				CPigpenDoc *pDoc = m_pParentView->GetDocument();
				pDoc->SetModifiedFlag(MODIFIED_FLAG);
				pDoc->UpdateAllViews(NULL, NO_CHANGE_EMITTERS_VIEW | NO_REORDER_EFFECTS_VIEW | NO_CHANGE_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_CHANGE_SWOOSHES_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW);
				break;
			}
/************** end special cases *******************/

			/* need to re-show data? this is useful for enabling/disabling buttons after button state changes */

			if (pCheckBox->bShowData)
				ShowData(m_pEffect);

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

			switch(nID)
			{
			case IDC_CHANNEL1:
			case IDC_CHANNEL2:
			case IDC_CHANNEL3:
			case IDC_CHANNEL4:
			case IDC_CHANNEL5:
			case IDC_CHANNEL6:
			case IDC_CHANNEL7:
			case IDC_CHANNEL8:
			case IDC_CHANNEL9:
			case IDC_CHANNEL10:
				if (m_pEffect)
					m_nChannelIndex = m_pEffect->GetChannelIndex();
				break;
			}
/************** end special cases *******************/

			/* need to re-show data? this is useful for enabling/disabling buttons after button state changes */

			if (pRadioButton->bShowData)
				ShowData(m_pEffect);

			return(TRUE);	// processed message
		}
		pRadioButton++;
	}

	/* didn't handle the button */

	return(FALSE);
}

/* catch user editing box directly */

bool CEffectDialog::OnKillFocusEdit(int nID) 
{
	CEdit *pEdit = (CEdit *) GetDlgItem(nID);
	if (m_pEffect == NULL)
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

void CEffectDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	/* point to correct channel */

	int nIndex = -1;
	bool bImage = FALSE;
	switch(nIDCtl)
	{
	case IDC_IMAGE_B1:
		nIndex = m_nChannelIndex;
		bImage = TRUE;
		break;
	case IDC_DIAGRAM_B1:
		nIndex = 0;
		break;
	case IDC_DIAGRAM_B2:
		nIndex = 1;
		break;
	case IDC_DIAGRAM_B3:
		nIndex = 2;
		break;
	case IDC_DIAGRAM_B4:
		nIndex = 3;
		break;
	case IDC_DIAGRAM_B5:
		nIndex = 4;
		break;
	case IDC_DIAGRAM_B6:
		nIndex = 5;
		break;
	case IDC_DIAGRAM_B7:
		nIndex = 6;
		break;
	case IDC_DIAGRAM_B8:
		nIndex = 7;
		break;
	case IDC_DIAGRAM_B9:
		nIndex = 8;
		break;
	case IDC_DIAGRAM_B10:
		nIndex = 9;
		break;
	default:
		ASSERT(FALSE);
		return;
	}
	_ASSERT(nIndex != -1);

	/* image or diagram? */

	if (bImage)
	{
		CImage *pImage = NULL;
		bool bShowAlphaMask;
		if (m_pEffect)
			pImage = m_pEffect->GetChannelImage(nIndex, &bShowAlphaMask);
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
	else
	{
		/* Rectangle gets filled with current brush and outlined with current pen */

		HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_MENU));
		HBRUSH hOldBrush = (HBRUSH) SelectObject(lpDrawItemStruct->hDC, hBrush);
		HPEN hBlackPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
		HPEN hLinePen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
		HPEN hErasePen = CreatePen(PS_SOLID, 2, GetSysColor(COLOR_MENU));
		HPEN hOldPen = (HPEN) SelectObject(lpDrawItemStruct->hDC, hBlackPen);
		Rectangle(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom);

		/* point to correct channel. don't draw anything for empty channels */

		if ((m_pEffect) && (m_pEffect->ChannelIsEmpty(nIndex) == FALSE))
		{
			ts_EffectChannel *pChannel = &m_pEffect->m_Channels[nIndex];

			/* midline */

			SelectObject(lpDrawItemStruct->hDC, hLinePen);
			int nWidth = lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left - 10;
			int OneSecondWidth = nWidth / 10;
			nWidth = OneSecondWidth * 10;	// round off width to even 10 seconds
			int nLeftX = lpDrawItemStruct->rcItem.left + ((lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left - nWidth) / 2);
			int nCenterY = (lpDrawItemStruct->rcItem.top + lpDrawItemStruct->rcItem.bottom) / 2;
			Rectangle(lpDrawItemStruct->hDC, nLeftX, nCenterY - 0, nLeftX + nWidth, nCenterY + 1);

			/* one-second tick marks */

			for (int i = 0; i <= 10; i++)
				Rectangle(lpDrawItemStruct->hDC, nLeftX + (OneSecondWidth * i), nCenterY - 8, nLeftX + (OneSecondWidth * i) + 1, nCenterY + 8);

			/* draw diagram. i want to draw last to first so first particle mark has priority */

			if (pChannel->fCount != 0.0f)
			{
				float fTotalTime = pChannel->fEmitDelay + pChannel->fEmitTime;
				float fSubsequentDelay = 0.0f;
				if (pChannel->fCount > 0.0f)
					fSubsequentDelay = pChannel->fEmitTime / (pChannel->fCount - 1.0f);
				int nCount = (int) pChannel->fCount;
				if (pChannel->fEmitTime == 0.0f)
					nCount = 1;	// all will draw on top of each other, no point in drawing more than one
				for (int i = 0; i < nCount; i++)
				{
					int nX = nLeftX + (int) (fTotalTime * (float) OneSecondWidth);
					fTotalTime -= fSubsequentDelay;	// move back delay between each particle

					/* draw twice, once in erase color, once in black */

					for (int j = 0; j < 2; j++)
					{
						SelectObject(lpDrawItemStruct->hDC, (j == 0) ? hErasePen : hBlackPen);
						MoveToEx(lpDrawItemStruct->hDC, nX, nCenterY - 5, NULL);
						LineTo(lpDrawItemStruct->hDC, nX + 2, nCenterY); 
						LineTo(lpDrawItemStruct->hDC, nX, nCenterY + 5); 
						LineTo(lpDrawItemStruct->hDC, nX - 2, nCenterY);
						LineTo(lpDrawItemStruct->hDC, nX, nCenterY - 5);
					}
				}
			}
		}

		/* restore order */

		SelectObject(lpDrawItemStruct->hDC, hOldBrush);
		SelectObject(lpDrawItemStruct->hDC, hOldPen);
		DeleteObject(hBrush);
		DeleteObject(hBlackPen);
		DeleteObject(hLinePen);
		DeleteObject(hErasePen);
	}
}


/* handle messages from dialog controls. this is intended to be a more generic approach than having every control
have to show up in the message map */

BOOL CEffectDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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

BOOL CEffectDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
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

/* copy a channel to the clipboard */

void CEffectDialog::OnCopy(void)
{
	DoCopy();
}

BOOL CEffectDialog::DoCopy(void)
{
	/* we should have a valid clipboard format at this point */

	ASSERT(m_nClipboardFormat != 0);
	ASSERT(m_nClipboardRecordSize != 0);
	if (m_nClipboardFormat == 0)
		return(FALSE);
	ASSERT(m_pEffect);	// context menu items not enabled unless m_pEffect is valid

	/* how much room do we need to hold data? */

	int nRecordCount = 1;
	int nBufferSize = sizeof(int);	// room for record count
	nBufferSize += nRecordCount * m_nClipboardRecordSize;

	/* open clipboard */

	if (OpenClipboard() == 0)
		return(FALSE);

	/* empty clipboard-- this clears it out-- only one thing lives in the clipboard at once */

	EmptyClipboard();

	/* allocate memory to hold data */

	HGLOBAL hBuffer = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, nBufferSize);
	if (hBuffer == NULL) 
	{ 
		ASSERT(FALSE);	// shouldn't happen
		CloseClipboard(); 
		return(FALSE);
	} 

	/* add data to buffer */

	char *pBuffer = (char *) GlobalLock(hBuffer);
	ASSERT(pBuffer);
	memcpy(pBuffer, &nRecordCount, sizeof(int));
	pBuffer += sizeof(int);
	memcpy(pBuffer, &m_pEffect->m_Channels[m_nChannelIndex], m_nClipboardRecordSize);
	pBuffer += m_nClipboardRecordSize;
	GlobalUnlock(hBuffer);

	/* copy to clipboard, and close */

	SetClipboardData(m_nClipboardFormat, hBuffer);
	CloseClipboard();

	/* success */

	return(TRUE);
}

/* paste a channel from the clipboard to the current effect */

void CEffectDialog::OnPaste(void)
{
	DoPaste();
}

BOOL CEffectDialog::DoPaste(void)
{
	/* should have data available, since paste menu item is disabled if not */

	ASSERT(IsClipboardFormatAvailable(m_nClipboardFormat));
	ASSERT(m_pEffect);	// context menu items not enabled unless m_pEffect is valid

	/* open clipboard */

	if (OpenClipboard() == 0)
		return(FALSE);

	/* get data */

	HGLOBAL hBuffer = GetClipboardData(m_nClipboardFormat);
	if (hBuffer == NULL) 
	{ 
		ASSERT(FALSE);	// shouldn't happen
		CloseClipboard(); 
		return(FALSE);
	} 

	/* get buffer */

	char *pBuffer = (char *) GlobalLock(hBuffer);
	ASSERT(pBuffer);

	/* get item count */

	int nCount = *(int *) pBuffer;
	pBuffer += sizeof(int);
	ASSERT(nCount == 1);	// only one channel at a time

	/* paste data */

	memcpy(&m_pEffect->m_Channels[m_nChannelIndex], pBuffer, m_nClipboardRecordSize);
	pBuffer += m_nClipboardRecordSize;

	/* clean up */

	GlobalUnlock(hBuffer);
	CloseClipboard();

	/* repaint view */

	ShowData(m_pEffect);

	/* doc has changed */

	CPigpenDoc *pDoc = m_pParentView->GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->SetModifiedFlag(MODIFIED_FLAG);

	/* success */

	return(TRUE);
}

/* clear a channel */

void CEffectDialog::OnClear(void)
{
	ASSERT(m_pEffect);	// context menu items not enabled unless m_pEffect is valid
	memset(&m_pEffect->m_Channels[m_nChannelIndex], 0, sizeof(ts_EffectChannel));

	/* repaint view */

	ShowData(m_pEffect);

	/* doc has changed */

	CPigpenDoc *pDoc = m_pParentView->GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->SetModifiedFlag(MODIFIED_FLAG);
	Preview__UpdateEffect(m_pEffect);
}