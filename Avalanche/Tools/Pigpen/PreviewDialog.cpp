/***********************************/
/*                                 */
/* PreviewDialog.cpp               */
/* Pigpen particle tool  1/25/01   */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* preview dialog                  */
/*                                 */
/***********************************/

/* this is the preview dialog that controls the preview window (the game engine window) */

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
	//	control id								data id				 index		min				max				delta		format	bIsInteger
	{	IDC_PREVIEW_DELAY_EDIT,				PREVIEW_DELAY,			-1,		MIN_E_DELAY,	MAX_E_DELAY,	0.1f,		"%.2f",	false},
	{	IDC_PREVIEW_YPOS_EDIT,				PREVIEW_YPOS,			-1,		-20.0f,			20.0,				1.0f,		"%.2f",	false},
	{	IDC_PREVIEW_MPH_EDIT,				PREVIEW_EMITTER_VEL,	-1,		0.0f,				120.0,			1.0f,		"%.0f",	false},
	{	IDC_PREVIEW_MODEL_XPOS_EDIT,		PREVIEW_MODEL_XPOS,	-1,		-20.0f,			20.0,				0.1f,		"%.2f",	false},
	{	IDC_PREVIEW_MODEL_YPOS_EDIT,		PREVIEW_MODEL_YPOS,	-1,		-20.0f,			20.0,				0.1f,		"%.2f",	false},
	{	IDC_PREVIEW_MODEL_ZPOS_EDIT,		PREVIEW_MODEL_ZPOS,	-1,		-20.0f,			20.0,				0.1f,		"%.2f",	false},
	{	IDC_PREVIEW_MODEL_YROT_EDIT,		PREVIEW_MODEL_YROT,	-1,		-360.0f,			360.0,			1.0f,		"%.0f",	false},
	{	IDC_WIND_MPH_EDIT,					PREVIEW_WIND_MPH,		-1,		0.0f,				120.0,			1.0f,		"%.0f",	false},
	
	// end of list-- must be last
	{	-1}
};

static ts_CheckBox CheckBoxList[] = {
	//	control id							data id			 index	flag bit							bShowData
	{	IDC_PREVIEW_FOLLOW_CHECK,		PREVIEW_FLAGS,		0,		PREVIEW_CAMERA_FOLLOWS,		FALSE},
	{	IDC_PREVIEW_GRAVITY_CHECK,		PREVIEW_FLAGS,		0,		PREVIEW_GRAVITY,				FALSE},
	{	IDC_PREVIEW_PARTICLE_PATH,		PREVIEW_FLAGS,		0,		PREVIEW_PARTICLE_PATH,		FALSE},
	
	// end of list-- must be last
	{	-1}
};

static ts_RadioButton RadioButtonList[] = {
	//	control id						data id							value								mask			bShowData

	// end of list-- must be last
	{	-1}
};

/* list of available timer values */

static int TimerList[] =
{
	CMasterTimer::PAUSE,
	CMasterTimer::ONE_SIXTEENTH,
	CMasterTimer::ONE_EIGHTH,
	CMasterTimer::ONE_FOURTH,
	CMasterTimer::ONE_HALF,
	CMasterTimer::ONE_X,

	/* must be last to terminate list */

	-1
};

/******************* externs *****************************/

/******************* class functions *********************/
 
/* i am trying to avoid long message maps, so i have overridden OnNotify and OnCommand. they catch slider bars,
spinners, edit boxes, and button clicks for me and process them more generically. the message map stuff still works,
but in general, use the higher-level intercept routines (like OnKillFocusEdit to catch all edit box commands) */

BEGIN_MESSAGE_MAP(CPreviewDialog, CDialog)
	//{{AFX_MSG_MAP(CPreviewDialog)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreviewDialog construction/destruction

CPreviewDialog::CPreviewDialog(CWnd* pParentWnd) : CDialog(CPreviewDialog::IDD, NULL)
{
	/* save passed parameters */

	m_pParentWnd = pParentWnd;

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

	//{{AFX_DATA_INIT(CPreviewDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

/* notify preview controller that dialog is gone */

CPreviewDialog::~CPreviewDialog()
{
	Preview__DialogClosed();
}

/* create a modeless dialog */

BOOL CPreviewDialog::Create(CPigpenDoc *pPreviewDoc)
{
	int nResult = CDialog::Create(IDD_PREVIEW_DIALOG, m_pParentWnd);

	/* surface combo box has list of available surface types to return from collision detection, to */
	/* test spawning. we don't include end-of-life as selectable surface types here-- they work */
	/* automatically when particle dies */

	if (nResult)
	{
		CComboBox *pSurfaceComboBox = (CComboBox *) GetDlgItem(IDC_PREVIEW_SURFACE_COMBO);
		pSurfaceComboBox->ResetContent();	// clear contents
		pSurfaceComboBox->EnableWindow(TRUE);
		for (int i = pPreviewDoc->GetEndOfLifeSurfaceCount(); i < pPreviewDoc->GetSurfaceCount(); i++)
		{
			CSurface *pSurface = pPreviewDoc->GetSurface(i);
			pSurfaceComboBox->AddString(pSurface->GetName());
		}
		int nSelection = pPreviewDoc->GetSurfaceIndex() - pPreviewDoc->GetEndOfLifeSurfaceCount();
		if (nSelection < 0)
			nSelection = 0;
		pSurfaceComboBox->SetCurSel(nSelection);
		Preview__SetCollisionIndex(nSelection);
	}

	/* set known timer speeds */

	if (nResult)
	{
		CComboBox *pTimerComboBox = (CComboBox *) GetDlgItem(IDC_PREVIEW_TIMER_COMBO);
		pTimerComboBox->ResetContent();	// clear contents
		pTimerComboBox->EnableWindow(TRUE);
		int i = 0;
		int nDefaultIndex = -1;
		while (TimerList[i] != -1)
		{
			switch(TimerList[i])
			{
			case CMasterTimer::PAUSE:
				pTimerComboBox->AddString("Pause");
				break;
			case CMasterTimer::ONE_SIXTEENTH:
				pTimerComboBox->AddString("1/16");
				break;
			case CMasterTimer::ONE_EIGHTH:
				pTimerComboBox->AddString("1/8");
				break;
			case CMasterTimer::ONE_FOURTH:
				pTimerComboBox->AddString("1/4");
				break;
			case CMasterTimer::ONE_HALF:
				pTimerComboBox->AddString("1/2");
				break;
			case CMasterTimer::ONE_X:
				pTimerComboBox->AddString("Normal");
				nDefaultIndex = i;
				break;
			default:
				ASSERT(false);
				pTimerComboBox->AddString("Unknown");
			}
			i++;
		}
		ASSERT(nDefaultIndex != -1);
		pTimerComboBox->SetCurSel(nDefaultIndex);
		g_timer.SetSpeed(CMasterTimer::ONE_X);
	}

	/* done */

	return(nResult != 0);

}

/* set initialial position below preview window */

void CPreviewDialog::InitialPosition(void)
{
	CRect Rect;
	GetWindowRect(&Rect);
	Rect.bottom += 500 - Rect.top;
	Rect.top = 500;
	SetWindowPos(&wndTop, Rect.left, Rect.top, Rect.right - Rect.left, Rect.bottom - Rect.top, 0);
}

/////////////////////////////////////////////////////////////////////////////
// CPreviewDialog operations

/* fill in the data in the dialog box with the current particle */

void CPreviewDialog::ShowData(void)
{
	/* set contents of edit boxes */

	ts_EditBox *pEditBox = EditBoxList;
	while (pEditBox->nID != -1)	// id == -1 marks end of list
	{
		CEdit *pEdit = (CEdit *) GetDlgItem(pEditBox->nID);
		ASSERT(pEdit);		// if this hits you have an entry in a dialog box table that isn't in this dialog page

		/* since this window always accompanies the preview window, all controls are always active */

		pEdit->EnableWindow(TRUE);
		UpdateEditBox(pEditBox->nID, 0, NULL, FALSE);

/************** special cases ***********************/
		/* special cases for disabling a box based on data state */

/************** end special cases *******************/
		pEditBox++;
	}

	/* set contents of checkboxes */

	ts_CheckBox *pCheckBox = CheckBoxList;
	while (pCheckBox->nID != -1)	// id == -1 marks end of list
	{
		CButton *pButton = (CButton *) GetDlgItem(pCheckBox->nID);
		ASSERT(pButton);	// if this hits you have an entry in a dialog box table that isn't in this dialog page

		/* everything enabled all the time for preview dialog */

		pButton->EnableWindow(TRUE);
		UpdateCheckBox(pCheckBox->nID, FALSE);

/************** special cases ***********************/
		/* special cases for disabling a box based on data state */

/************** end special cases *******************/
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

	/* fill in text */

	char Message[100];
	Vector3 *pCameraPos = Preview__GetCameraPos();
	sprintf(Message, "(%.1f, %.1f, %.1f)", pCameraPos->x(), pCameraPos->y(), pCameraPos->z());
	GetDlgItem(IDC_CAMERA_POS)->SetWindowText(Message);

	/* stage name */

	char File[_MAX_PATH];
	char *pFile = File;
	strncpy(File, Preview__GetStageFileAndStatus().GetBuffer(10), _MAX_PATH);
	if (strlen(File) > 75)
	{
		pFile = &File[strlen(File) - 75];
		pFile[0] = '.';
		pFile[1] = '.';
		pFile[2] = '.';
	}
	sprintf(Message, "Stage: %s", pFile);
	GetDlgItem(IDC_PREVIEW_UPDATE_STAGE)->SetWindowText(Message);

	/* emitter model name */

	strncpy(File, Preview__GetEmitterModelFileAndStatus().GetBuffer(10), _MAX_PATH);
	pFile = File;
	if (strlen(File) > 52)
	{
		pFile = &File[strlen(File) - 52];
		pFile[0] = '.';
		pFile[1] = '.';
		pFile[2] = '.';
	}
	if (strlen(pFile) == 0)
		strcpy(Message, "Emitter model: none");
	else
		sprintf(Message, "Emitter model: %s", pFile);
	GetDlgItem(IDC_PREVIEW_UPDATE_EMITTER_MODEL)->SetWindowText(Message);

	/* emitter velocity units */

	if (Preview__EmitterVelIsMPH())
		strcpy(Message, "Emitter mph");
	else
		strcpy(Message, "Emitter m/s");
	GetDlgItem(IDC_PREVIEW_EMITTER_VEL)->SetWindowText(Message);
}

/* update edit box's value. if pfDirectValue is non-NULL, use value directly, ignore delta. for easier
implementation i am using floats for everything that goes in an edit box, even if they will ultimately be ints */

void CPreviewDialog::UpdateEditBox(int nID, int nDelta, float *pfDirectValue, bool bCheckSpecialCases)
{
	float *pfValue = NULL;
	float fOldValue;
	char acString[20];

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

	pfValue = (float *) Preview__GetDataPointer(pEditBox->nDataID);
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
		case IDC_PREVIEW_DELAY_EDIT:
			Preview__UpdateDelay();
			break;
		case IDC_PREVIEW_YPOS_EDIT:
			Preview__UpdateEmitterYPos();
			break;
		case IDC_PREVIEW_MPH_EDIT:
			Preview__UpdateEmitterVel();
			break;
		case IDC_WIND_MPH_EDIT:
			Preview__UpdateWindMPH();
			break;
		case IDC_PREVIEW_MODEL_XPOS_EDIT:
		case IDC_PREVIEW_MODEL_YPOS_EDIT:
		case IDC_PREVIEW_MODEL_ZPOS_EDIT:
		case IDC_PREVIEW_MODEL_YROT_EDIT:
			Preview__BuildEmitterModelTransform(TRUE);
			break;
		}
/************** end special cases *******************/
	}
}

/* update check box's state. check/uncheck depending on proper bit's state in proper flag unsigned long word */

void CPreviewDialog::UpdateCheckBox(int nID, bool bToggle)
{
	unsigned long *pulFlags = NULL;

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

	pulFlags = (unsigned long *) Preview__GetDataPointer(pCheckBox->nDataID);
	ASSERT(pulFlags);

	/* toggle if requested */

	if (bToggle)
		*pulFlags ^= pCheckBox->ulFlagBit;

	/* show state of flag bit */

	CButton *pButton = (CButton *) GetDlgItem(nID);
	pButton->SetCheck((*pulFlags & pCheckBox->ulFlagBit) ? TRUE : FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CPreviewDialog message handlers

/* called when dialog is closed by sysmenu close button (X) */

void CPreviewDialog::OnCancel()
{
	/* from the help on OnCancel(): "If you implement the Cancel button in a modeless dialog box, you must */
	/* override the OnCancel member function and call DestroyWindow from within it. Don’t call the base-class */
	/* member function, because it calls EndDialog, which will make the dialog box invisible but not destroy it." */

	DestroyWindow();
}

/* called after window is destroyed */

void CPreviewDialog::PostNcDestroy()
{
	delete this;	// dialog was created by parent window using "new"
}

/* intercept spin control messages to update associated edit box value */

bool CPreviewDialog::OnSpinner(NMHDR* pNMHDR, LRESULT* pResult) 
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

bool CPreviewDialog::OnKillFocusEdit(int nID) 
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

/* handle messages from dialog controls. this is intended to be a more generic approach than having every control
have to show up in the message map */

BOOL CPreviewDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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

/* called when combo box selection is changed */

bool CPreviewDialog::OnSelChange(int nID)
{
	/* get new selection from combo box */

	CComboBox* pComboBox = (CComboBox*) GetDlgItem(nID);
	switch(nID)
	{
	case IDC_PREVIEW_SURFACE_COMBO:
		Preview__SetCollisionIndex(pComboBox->GetCurSel());
		break;
	case IDC_PREVIEW_TIMER_COMBO:
		g_timer.SetSpeed((CMasterTimer::ETimerSpeed) TimerList[pComboBox->GetCurSel()]);
		break;
	}
	return(TRUE);	// processed message
}

/* catch button clicks. this routine does not have to handle all button clicks, but needs to return FALSE if
it gets passed a button it doesn't understand or want to deal with */

bool CPreviewDialog::OnButtonClicked(int nID)
{
	/* normal buttons */

	switch(nID)
	{
	case IDC_BKG_COLOR:
	{
		CColorDialog ColorDialog(
			Preview__GetBackgroundColor(),				// COLORREF clrInit = 0
			CC_FULLOPEN | CC_SOLIDCOLOR,		// DWORD dwFlags = 0-- CC_FULLOPEN makes the dialog come up with the custom color picker open
			NULL);									// CWnd* pParentWnd = NULL 
		if (ColorDialog.DoModal() != IDOK)
			return(TRUE);	// processed message

		/* copy new color into proper place */

		if (ColorDialog.GetColor() != Preview__GetBackgroundColor())
		{
			Preview__SetBackgroundColor(ColorDialog.GetColor());

			/* repaint color box */

			GetDlgItem(IDC_BKG_COLOR)->Invalidate();	// so it gets repainted, intercepted by OnDrawItem
		}
		return(TRUE);	// processed message
	}
	case IDC_CURRENT_CAMERA_POS:
		Preview__SetCameraInitPos();
		ShowData();	// re-show data to fill in camera position text
		return(TRUE);	// processed message
	case IDC_PREVIEW_STOP:
		Preview__ClearEmitterVel();
		ShowData();
		return(TRUE);
	case IDC_PREVIEW_RESET_EMITTER:
		Preview__ResetEmitter();	// start it at (0, 0, 0) again but leave speed alone
		return(TRUE);
	case IDC_PREVIEW_RESET_CAMERA:
		Preview__ResetCamera();
		return(TRUE);
	case IDC_PREVIEW_RESET_ALL:
		Preview__ClearEmitterVel();
		ShowData();
		Preview__ResetEmitter();
		Preview__ResetCamera();
		return(TRUE);
	case IDC_PREVIEW_UPDATE_STAGE:
		Preview__LoadNewStage();
		ShowData();
		return(TRUE);
	case IDC_PREVIEW_UPDATE_EMITTER_MODEL:
		Preview__LoadNewEmitterModel();
		ShowData();
		return(TRUE);
	case IDC_PREVIEW_CLEAR_EMITTER_MODEL:
		Preview__ClearEmitterModel();
		ShowData();
		return(TRUE);
	case IDC_PREVIEW_EMITTER_VEL:
		Preview__ToggleEmitterVelUnits();
		ShowData();
		return(true);
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
				ShowData();

			return(TRUE);	// processed message
		}
		pRadioButton++;
	}
#endif

	/* didn't handle the button */

	return(FALSE);
}

/* called when owner-draw flagged dialog buttons are going to be drawn. used to draw graphic images */

void CPreviewDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	int nIndex;
	switch(nIDCtl)
	{
	default:
		ASSERT(FALSE);
		/* fall through */
	case IDC_BKG_COLOR:
		nIndex = 0;
		break;
	}

	/* Rectangle gets filled with current brush and outlined with current pen */

	HBRUSH hBrush = CreateSolidBrush(Preview__GetBackgroundColor());
	HBRUSH hOldBrush = (HBRUSH) SelectObject(lpDrawItemStruct->hDC, hBrush);
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
	HPEN hOldPen = (HPEN) SelectObject(lpDrawItemStruct->hDC, hPen);
	Rectangle(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom);

	/* clean up */

	SelectObject(lpDrawItemStruct->hDC, hOldBrush);
	SelectObject(lpDrawItemStruct->hDC, hOldPen);
	DeleteObject(hBrush);
	DeleteObject(hPen);
}

/* this catches other control messages. from the help on CWnd::OnCommand, about wParam:

"The low-order word of wParam identifies the command ID of the menu item, control, or accelerator.
The high-order word of wParam specifies the notification message if the message is from a control.
If the message is from an accelerator, the high-order word is 1. If the message is from a menu,
the high-order word is 0." */

BOOL CPreviewDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch((wParam >> 16) & 0xffff)
	{
	case BN_CLICKED:
		/* user has clicked a button. check boxes are buttons, too */

		if (OnButtonClicked(wParam & 0xffff) == TRUE)
			return(TRUE);	// processed message
		break;
	case CBN_SELCHANGE:
		/* user has selected a new entry in combo box */

		if (OnSelChange(wParam & 0xffff) == TRUE)
			return(TRUE);	// processed message
		break;
	case EN_KILLFOCUS:
		/* user has finished editing text in an edit box */

		if (OnKillFocusEdit(wParam & 0xffff) == TRUE)
			return(TRUE);	// processed message
		break;
	}

	/* let base class handle anything else */

	return CDialog::OnCommand(wParam, lParam);
}

