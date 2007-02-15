/***********************************/
/*                                 */
/* GraphicDialog.cpp               */
/* Pigpen particle tool  12/20/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* dialog box for editing graphics */
/*                                 */
/***********************************/

/* this is the dialog box that let's the user edit a particle graphic */

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

#define MAX_IMAGE_FRAMES	5	// number of image frames across top of dialog

/******************* structures **************************/

/******************* local variables *********************/

/* dialog item lists */

static ts_EditBox EditBoxList[] = {
	//	control id				data id	 						index		min		max		delta		format	bIsInteger
	{	IDC_GR_FPS_EDIT,		CGraphic::DATA_FPS,			  -1,		1.0f,		60.0f,	1.0f,		"%.0f",	TRUE},
	{	IDC_GR_XSIZE_EDIT,	CGraphic::DATA_XSIZE,		  -1,		0.0f,		9999.0f,	0.1f,		"%.2f",	FALSE},
	{	IDC_GR_YSIZE_EDIT,	CGraphic::DATA_YSIZE,		  -1,		0.0f,		9999.0f,	0.1f,		"%.2f",	FALSE},
	
	// end of list-- must be last
	{	-1}
};

static ts_CheckBox CheckBoxList[] = {
	//	control id					data id					 index	flag bit									bShowData
	{	IDC_GR_ASPECT_CHECK,		CGraphic::DATA_FLAGS,	-1,	GRAPHIC_MAINTAIN_ASPECT_RATIO,	FALSE},
	{	IDC_ALPHA_MASK_CHECK,	CGraphic::DATA_FLAGS,	-1,	GRAPHIC_SHOW_ALPHA_MASK,			TRUE},
	
	// end of list-- must be last
	{	-1}
};

static ts_RadioButton RadioButtonList[] = {
	//	control id						data id							value								mask			bShowData

	// end of list-- must be last
	{	-1}
};

static int nImageButtonID[MAX_IMAGE_FRAMES] = {
	IDC_IMAGE_BUTTON1,
	IDC_IMAGE_BUTTON2,
	IDC_IMAGE_BUTTON3,
	IDC_IMAGE_BUTTON4,
	IDC_IMAGE_BUTTON5};

/******************* externs *****************************/

/******************* class functions *********************/

/* i am trying to avoid long message maps, so i have overridden OnNotify and OnCommand. they catch slider bars,
spinners, edit boxes, and button clicks for me and process them more generically. the message map stuff still works,
but in general, use the higher-level intercept routines (like OnKillFocusEdit to catch all edit box commands) */

BEGIN_MESSAGE_MAP(CGraphicDialog, CDialog)
	//{{AFX_MSG_MAP(CGraphicDialog)
	ON_WM_CONTEXTMENU()
	ON_WM_DRAWITEM()
	ON_COMMAND(ID_MARK_ICON, OnMarkIcon)
	ON_COMMAND(ID_DELETE, OnDelete)
	ON_COMMAND(ID_DELETE_ALL, OnDeleteAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphicDialog construction/destruction

CGraphicDialog::CGraphicDialog(CDocView *pParentView, CWnd* pParentWnd) : CDialog(CGraphicDialog::IDD, pParentWnd)
{
	//{{AFX_DATA_INIT(CGraphicDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	ASSERT(pParentView != NULL);
	ASSERT(pParentWnd != NULL);

	m_pParentView = pParentView;
	m_pParentWnd = pParentWnd;
	m_pGraphic = NULL;
	m_nHighlightIndex = 0;

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

CGraphicDialog::~CGraphicDialog()
{
	m_pParentView->EditWindowClosed(EDIT_WINDOW0);
}

/* create a modeless dialog */

BOOL CGraphicDialog::Create()
{
	return(CDialog::Create(IDD_GRAPHIC_DIALOG, m_pParentWnd));
}

/////////////////////////////////////////////////////////////////////////////
// CGraphicDialog operations

/* fill in the data in the dialog box with the current graphic */

void CGraphicDialog::ShowData(CGraphic *pGraphic)
{
	CStatic *pStatic;

	/* reset highlight index if changing */

	if (pGraphic != m_pGraphic)
		m_nHighlightIndex = 0;

	/* record graphic */

	m_pGraphic = pGraphic;

	/* fill stuff in */

	if (m_pGraphic == NULL)
		SetWindowText("Graphic: NONE SELECTED");
	else
		SetWindowText("Graphic: " + m_pGraphic->GetName());

	/* frames, and indices under image frames */

	UpdateImagesAndIndices();

	/* image count */

	pStatic = (CStatic *) GetDlgItem(IDC_IMAGE_COUNT);
	if (m_pGraphic == NULL)
		pStatic->SetWindowText("N/A");
	else
		pStatic->SetWindowText(ToCString(m_pGraphic->GetImageCount()));

	/* image size */

	pStatic = (CStatic *) GetDlgItem(IDC_IMAGE_SIZE);
	if (m_pGraphic == NULL)
		pStatic->SetWindowText("N/A");
	else
		pStatic->SetWindowText(ToCString(m_pGraphic->GetWidth()) + " x " + ToCString(m_pGraphic->GetHeight()));

	/* highlighted-image-specific info */

	ShowHighlightedImageInfo();

	/* base file name (that was originally used to load in images) */

	pStatic = (CStatic *) GetDlgItem(IDC_BASE_FILE);
	if (m_pGraphic == NULL)
		pStatic->SetWindowText("N/A");
	else
		pStatic->SetWindowText(m_pGraphic->GetBaseSourceFile());

	/* buttons */

	GetDlgItem(IDC_IMPORT_IMAGE)->EnableWindow(m_pGraphic != NULL);
	GetDlgItem(IDC_REPLACE_IMAGE)->EnableWindow((m_pGraphic != NULL) && (m_pGraphic->GetImageCount() != 0));
	GetDlgItem(IDC_UPDATE_IMAGE)->EnableWindow((m_pGraphic != NULL) && (m_pGraphic->GetImageCount() != 0));

	/* set contents of edit boxes */

	ts_EditBox *pEditBox = EditBoxList;
	while (pEditBox->nID != -1)	// id == -1 marks end of list
	{
		CEdit *pEdit = (CEdit *) GetDlgItem(pEditBox->nID);
		ASSERT(pEdit);		// if this hits you have an entry in a dialog box table that isn't in this dialog page
		if (m_pGraphic == NULL)
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
		if (m_pGraphic == NULL)
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

#if 0	// no radio buttons currently
	/* set contents of radio buttons */

	ts_RadioButton *pRadioButton = RadioButtonList;
	while (pRadioButton->nID != -1)	// id == -1 marks end of list
	{
		CButton *pButton = (CButton *) GetDlgItem(pRadioButton->nID);
		ASSERT(pButton);	// if this hits you have an entry in a dialog box table that isn't in this dialog page
		if (m_pGraphic == NULL)
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
}

/* invalidate image buttons so that the images get redrawn, and reset text under image frames */

void CGraphicDialog::UpdateImagesAndIndices(void)
{
	static int nIndexTextID[MAX_IMAGE_FRAMES] = {
		IDC_INDEX_TEXT1,
		IDC_INDEX_TEXT2,
		IDC_INDEX_TEXT3,
		IDC_INDEX_TEXT4,
		IDC_INDEX_TEXT5};

	for (int i = 0; i < MAX_IMAGE_FRAMES; i++)
	{
		/* image */

		GetDlgItem(nImageButtonID[i])->Invalidate();	// so it gets repainted, intercepted by OnDrawItem

		/* index under image */

		char acString[20];
		if (m_pGraphic == NULL)
			sprintf(acString, "%d", i);
		else
		{
			/* is this the icon image? */

			if (i + m_pGraphic->GetGraphicDialogIndex() == m_pGraphic->GetIconIndex())
				sprintf(acString, "%d (Icon)", m_pGraphic->GetGraphicDialogIndex() + i);
			else
				sprintf(acString, "%d", m_pGraphic->GetGraphicDialogIndex() + i);
		}
		GetDlgItem(nIndexTextID[i])->SetWindowText(acString);
	}
}

/* update edit box's value. if pfDirectValue is non-NULL, use value directly, ignore delta. for easier
implementation i am using floats for everything that goes in an edit box, even if they will ultimately be ints.
the bCheckSpecialCases flag is to prevent infinite recursion resulting from handling special cases */

void CGraphicDialog::UpdateEditBox(int nID, int nDelta, float *pfDirectValue, bool bCheckSpecialCases)
{
	float *pfValue = NULL;
	float fOldValue;
	char acString[20];

	/* should be working on valid data */

	ASSERT(m_pGraphic != NULL);

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

	pfValue = (float *) m_pGraphic->GetDataPointer(pEditBox->nDataID);
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
		case IDC_GR_XSIZE_EDIT:
			if ((*pfValue != fOldValue) && (m_pGraphic->IsMaintainAspectRatio()))
			{
				float fNewValue = *pfValue * (float) m_pGraphic->GetHeight() / (float) m_pGraphic->GetWidth();
				UpdateEditBox(IDC_GR_YSIZE_EDIT, 0, &fNewValue, FALSE);
			}
			break;
		case IDC_GR_YSIZE_EDIT:
			if ((*pfValue != fOldValue) && (m_pGraphic->IsMaintainAspectRatio()))
			{
				float fNewValue = *pfValue * (float) m_pGraphic->GetWidth() / (float) m_pGraphic->GetHeight();
				UpdateEditBox(IDC_GR_XSIZE_EDIT, 0, &fNewValue, FALSE);
			}
			break;
		}
/************** end special cases *******************/
	}

	/* has doc changed? */

	if (*pfValue != fOldValue)
	{
		CPigpenDoc *pDoc = m_pParentView->GetDocument();
		pDoc->SetModifiedFlag(MODIFIED_FLAG);
	}
}

/* update check box's state. check/uncheck depending on proper bit's state in proper flag unsigned long word */

void CGraphicDialog::UpdateCheckBox(int nID, bool bToggle)
{
	unsigned long *pulFlags = NULL;

	/* should be working on valid data */

	ASSERT(m_pGraphic != NULL);

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

	pulFlags = (unsigned long *) m_pGraphic->GetDataPointer(pCheckBox->nDataID);
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
	}
}

/* user clicked on an image frame. update index if appropriate */

void CGraphicDialog::SetHighlightIndex(int nIndex)	// range of 0..4-- currently highlighted image
{
	if (m_pGraphic == NULL)
		return;
	if (m_nHighlightIndex == nIndex)
		return;
	if (nIndex + m_pGraphic->GetGraphicDialogIndex() >= m_pGraphic->GetImageCount())
		return;	// ignore clicks on empty frames
	int nOldIndex = m_nHighlightIndex;
	m_nHighlightIndex = nIndex;

	/* repaint image frames that changed */

	CStatic *pStatic;
	pStatic = (CStatic *) GetDlgItem(nImageButtonID[nOldIndex]);
	pStatic->Invalidate();	// so it gets repainted, intercepted by OnDrawItem
	pStatic = (CStatic *) GetDlgItem(nImageButtonID[nIndex]);
	pStatic->Invalidate();	// so it gets repainted, intercepted by OnDrawItem

	/* update highlighted-image-specific info */

	ShowHighlightedImageInfo();
}

/* get index of highlighted image */

int CGraphicDialog::GetHighlightIndex(void)	// range 0..4-- currently highlighted image
{
	ASSERT(m_pGraphic);
	ASSERT(m_pGraphic->GetGraphicDialogIndex() + m_nHighlightIndex < m_pGraphic->GetImageCount());
	return(m_nHighlightIndex);
}

/* get pointer to currently highlighted image */

CImage *CGraphicDialog::GetHighlightedImage(void)
{
	if ((m_pGraphic == NULL) || (m_pGraphic->GetImageCount() == 0))
		return(NULL);
	return(m_pGraphic->GetImage(m_pGraphic->GetGraphicDialogIndex() + GetHighlightIndex()));
}

/* fill in info specific to currently highlighted image */

void CGraphicDialog::ShowHighlightedImageInfo(void)
{
	static char acNA[] = "N/A";
	static char acNone[] = "None";
	char *pTypeString = acNA;
	char *pFileName = acNA;

	if (m_pGraphic != NULL)
	{
		CImage *pImage = GetHighlightedImage();
		if (pImage == NULL)
		{
			pTypeString = acNone;
			pFileName = acNone;
		}
		else
		{
			pTypeString = pImage->GetImageTypeString();
			pFileName = pImage->GetSourceFile().GetBuffer(10);
		}
	}
	GetDlgItem(IDC_IMAGE_TYPE)->SetWindowText(pTypeString);
	GetDlgItem(IDC_CURRENT_FILE)->SetWindowText(pFileName);
}

/////////////////////////////////////////////////////////////////////////////
// CGraphicDialog message handlers

/* called when OK is selected, or when user presses enter. i don't want pressing enter to end the dialog, so
override and do nothing */

void CGraphicDialog::OnOK()
{
}

/* called when dialog is closed by sysmenu close button (X) */

void CGraphicDialog::OnCancel()
{
	/* from the help on OnCancel(): "If you implement the Cancel button in a modeless dialog box, you must */
	/* override the OnCancel member function and call DestroyWindow from within it. Don’t call the base-class */
	/* member function, because it calls EndDialog, which will make the dialog box invisible but not destroy it." */

	DestroyWindow();
}

/* called after window is destroyed */

void CGraphicDialog::PostNcDestroy()
{
	delete this;	// dialog was created by parent window using "new"
}

/* import images into graphic */

void CGraphicDialog::ImportImage(bool bReplace, bool bUpdate) 
{
	ASSERT(m_pGraphic != NULL);
	bool bSuccess = m_pGraphic->ImportImage(bReplace, bUpdate);
	if (bSuccess)
	{
		/* doc has changed */

		CPigpenDoc *pDoc = m_pParentView->GetDocument();
		ASSERT_VALID(pDoc);
		pDoc->SetModifiedFlag(MODIFIED_FLAG);
		pDoc->UpdateAllViews(NULL, NO_REORDER_ALL_VIEWS);	// update all views since they use graphic as icon
	}
}

/* handle popup menu for dialog */

void CGraphicDialog::OnContextMenu(CWnd* pWnd, CPoint Point) 
{
	CMenu PopupMenu;
	if (PopupMenu.LoadMenu(IDR_POPUP_MENUS))
	{
		CMenu *pSubMenu = PopupMenu.GetSubMenu(GRAPHIC_DIALOG_POPUP_MENU);

		/* enable/disable menu items */

		int nState = MF_ENABLED;
		if ((m_pGraphic == NULL) || (m_pGraphic->GetImageCount() == 0))
			nState = MF_GRAYED;
		pSubMenu->EnableMenuItem(ID_MARK_ICON, nState);
		pSubMenu->EnableMenuItem(ID_DELETE, nState);
		pSubMenu->EnableMenuItem(ID_MOVE_LEFT, nState);
		pSubMenu->EnableMenuItem(ID_MOVE_RIGHT, nState);

		/* show the menu */

		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, Point.x, Point.y, this);
	}
}

/* called when owner-draw flagged dialog buttons are going to be drawn. used to draw image bitmaps in
image frames, which are button controls that user can click on */

void CGraphicDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	/* set image frame index */

	int nIndex = -1;
	switch(nIDCtl)
	{
	case IDC_IMAGE_BUTTON1:
		nIndex = 0;
		break;
	case IDC_IMAGE_BUTTON2:
		nIndex = 1;
		break;
	case IDC_IMAGE_BUTTON3:
		nIndex = 2;
		break;
	case IDC_IMAGE_BUTTON4:
		nIndex = 3;
		break;
	case IDC_IMAGE_BUTTON5:
		nIndex = 4;
		break;
	}
	ASSERT(nIndex != -1);	// make sure we're not getting anything unexpected

	/* bitmap or empty frame? */

	if ((m_pGraphic) && (nIndex + m_pGraphic->GetGraphicDialogIndex() < m_pGraphic->GetImageCount()))
	{
		/* frame outline */

		HBRUSH hBrush;
		if (nIndex == m_nHighlightIndex)
			hBrush = CreateSolidBrush(RGB(255, 0, 0));
		else
			hBrush = CreateSolidBrush(RGB(0, 0, 0));
		HBRUSH hOldBrush = (HBRUSH) SelectObject(lpDrawItemStruct->hDC, hBrush);
		FrameRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, hBrush);

		/* clean up */

		SelectObject(lpDrawItemStruct->hDC, hOldBrush);
		DeleteObject(hBrush);

		/* image */

		CImage *pImage = m_pGraphic->GetImage(nIndex + m_pGraphic->GetGraphicDialogIndex());
		pImage->Paint(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, 2, m_pGraphic->IsShowAlphaMask());
	}
	else
	{
		/* Rectangle gets filled with current brush and outlined with current pen */

		HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_MENU));
		HBRUSH hOldBrush = (HBRUSH) SelectObject(lpDrawItemStruct->hDC, hBrush);
		HPEN hPen;
		if (nIndex == m_nHighlightIndex)
		    hPen = CreatePen(PS_SOLID, 1, RGB(255,0,0));
		else
		    hPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
		HPEN hOldPen = (HPEN) SelectObject(lpDrawItemStruct->hDC, hPen);
		Rectangle(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom);
		SelectObject(lpDrawItemStruct->hDC, hOldBrush);
		SelectObject(lpDrawItemStruct->hDC, hOldPen);
		DeleteObject(hBrush);
		DeleteObject(hPen);
	}
}

/* intercept slider moves */

bool CGraphicDialog::OnCustomdrawSlider(NMHDR* pNMHDR, LRESULT* pResult) 
{
	ASSERT(pNMHDR->idFrom == IDC_SLIDER1);
	if (m_pGraphic)
	{
		CSliderCtrl *pSlider = (CSliderCtrl *) GetDlgItem(pNMHDR->idFrom);
		int nPos;
		int nMin;
		int nMax;
		pSlider->GetRange(nMin, nMax);
		nPos = pSlider->GetPos();

		/* calculate new start index. basically, we want to see how many frames can't be displayed and */
		/* divide the slider range by that number */

		int nIndex = (nPos * (m_pGraphic->GetImageCount() - MAX_IMAGE_FRAMES + 1)) / nMax;
		if (nIndex <= 0)
			nIndex = 0;
		else if (nIndex > m_pGraphic->GetImageCount() - MAX_IMAGE_FRAMES)
			nIndex = m_pGraphic->GetImageCount() - MAX_IMAGE_FRAMES;
		if (nIndex != m_pGraphic->GetGraphicDialogIndex())
		{
			m_pGraphic->SetGraphicDialogIndex(nIndex);

			/* we are scrolling-- invalidate all image frames, and reset indices under frames */

			UpdateImagesAndIndices();

			/* update highlighted-image-specific info */

			ShowHighlightedImageInfo();
		}
	}

	/* return value */
	
	*pResult = 0;
	return(TRUE);	// processed message
}

/* handle up/down spinners-- they change contents of edit boxes */

bool CGraphicDialog::OnSpinner(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/* not if no graphic */

	if (m_pGraphic == NULL)
	{
		*pResult = 1;	// non-zero means abort further processing
		return(FALSE);	// did not process message
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

bool CGraphicDialog::OnKillFocusEdit(int nID) 
{
	CEdit *pEdit = (CEdit *) GetDlgItem(nID);
	if (m_pGraphic == NULL)
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
		if ((fValue == 0.0f) && (AllZeros(String) == FALSE))
			pfValue = NULL;	// illegal entry, revert back to value
		UpdateEditBox(nID, 0, pfValue, TRUE);
	}
	return(TRUE);	// processed message
}

/* catch button clicks. this routine does not have to handle all button clicks, but needs to return FALSE if
it gets passed a button it doesn't understand or want to deal with */

bool CGraphicDialog::OnButtonClicked(int nID)
{
	/* normal buttons */

	switch(nID)
	{
	case IDC_IMPORT_IMAGE:
		ImportImage(FALSE, FALSE);
		return(TRUE);	// processed message
	case IDC_REPLACE_IMAGE:
		ImportImage(TRUE, FALSE);
		return(TRUE);	// processed message
	case IDC_UPDATE_IMAGE:
		ImportImage(TRUE, TRUE);
		return(TRUE);	// processed message
	case IDC_IMAGE_BUTTON1:
		SetHighlightIndex(0);
		return(TRUE);	// processed message
	case IDC_IMAGE_BUTTON2:
		SetHighlightIndex(1);
		return(TRUE);	// processed message
	case IDC_IMAGE_BUTTON3:
		SetHighlightIndex(2);
		return(TRUE);	// processed message
	case IDC_IMAGE_BUTTON4:
		SetHighlightIndex(3);
		return(TRUE);	// processed message
	case IDC_IMAGE_BUTTON5:
		SetHighlightIndex(4);
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

			switch(nID)
			{
			case IDC_ALPHA_MASK_CHECK:
				/* repaint to reflect new IsShowAlphaMask state */

				m_pParentView->GetDocument()->UpdateAllViews(NULL, NO_REORDER_ALL_VIEWS);	// update all views since they use graphic as icon
				break;
			}
/************** end special cases *******************/

			/* need to re-show data? this is useful for enabling/disabling buttons after check box state changes */

			if (pCheckBox->bShowData)
				ShowData(m_pGraphic);

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
				ShowData(m_pGraphic);

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

BOOL CGraphicDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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
		case NM_CUSTOMDRAW:
			/* slider */

			if (OnCustomdrawSlider(pNMHDR, pResult) == TRUE)
				return(TRUE);	// processed message
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

BOOL CGraphicDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch((wParam >> 16) & 0xffff)
	{
	case EN_KILLFOCUS:
		/* user has finished editing text in an edit box */

		if (OnKillFocusEdit(wParam & 0xffff) == TRUE)
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

/* mark current graphic image as the icon for this graphic */

void CGraphicDialog::OnMarkIcon() 
{
	ASSERT(m_pGraphic);
	ASSERT(m_pGraphic->GetImageCount() != 0);
	int nOldIconIndex = m_pGraphic->GetIconIndex();
	m_pGraphic->SetIconIndex(m_pGraphic->GetGraphicDialogIndex() + m_nHighlightIndex);

	/* if doc has changed, update views */

	if (nOldIconIndex != m_pGraphic->GetIconIndex())
	{
		CPigpenDoc *pDoc = m_pParentView->GetDocument();
		ASSERT_VALID(pDoc);
		pDoc->SetModifiedFlag(MODIFIED_FLAG);
		pDoc->UpdateAllViews(NULL, NO_REORDER_ALL_VIEWS);	// update all views since they use graphic as icon
	}
}

/* delete current graphic image */

void CGraphicDialog::OnDelete() 
{
	ASSERT(m_pGraphic);
	ASSERT(m_pGraphic->GetImageCount() != 0);
	if (AfxMessageBox("Delete highlighted image?", MB_YESNO, 0) != IDYES)
		return;

	/* remove image */

	int nHighlightIndex = GetHighlightIndex();
	int nImageIndex = m_pGraphic->GetGraphicDialogIndex() + nHighlightIndex;
	m_pGraphic->RemoveImage(nImageIndex);

	/* adjust indices */

	if (m_pGraphic->GetImageCount() == 0)
	{
		/* no more images */

		SetHighlightIndex(0);
		m_pGraphic->SetIconIndex(NO_ICON);
	}
	else
	{
		/* check icon index */

		if ((m_pGraphic->GetIconIndex() > nImageIndex) || (m_pGraphic->GetIconIndex() == m_pGraphic->GetImageCount()))
			m_pGraphic->SetIconIndex(m_pGraphic->GetIconIndex() - 1);

		/* try to keep the image slots full */

		if (m_pGraphic->GetImageCount() - m_pGraphic->GetGraphicDialogIndex() < MAX_IMAGE_FRAMES)
		{
			/* we have at least one empty slot at right. try to fill it */

			if (m_pGraphic->GetGraphicDialogIndex() > 0)
				m_pGraphic->SetGraphicDialogIndex(m_pGraphic->GetGraphicDialogIndex() - 1);	// bring on a new image from left
			else if (nImageIndex == m_pGraphic->GetImageCount())
			{
				ASSERT(nHighlightIndex > 0);
				SetHighlightIndex(nHighlightIndex - 1);	// slide highlight left one
			}
		}
	}

	/* doc has been modified */

	CPigpenDoc *pDoc = m_pParentView->GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->SetModifiedFlag(MODIFIED_FLAG);
	pDoc->UpdateAllViews(NULL, NO_REORDER_ALL_VIEWS);	// update all views since they use graphic as icon
}

/* delete all graphic images */

void CGraphicDialog::OnDeleteAll() 
{
	ASSERT(m_pGraphic);
	ASSERT(m_pGraphic->GetImageCount() != 0);
	if (AfxMessageBox("Delete all images?", MB_YESNO, 0) != IDYES)
		return;

	/* remove all images */

	while (m_pGraphic->GetImageCount() != 0)
		m_pGraphic->RemoveImage(0);

	/* reset indices */

	SetHighlightIndex(0);
	m_pGraphic->SetIconIndex(NO_ICON);

	/* doc has been modified */

	CPigpenDoc *pDoc = m_pParentView->GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->SetModifiedFlag(MODIFIED_FLAG);
	pDoc->UpdateAllViews(NULL, NO_REORDER_ALL_VIEWS);	// update all views since they use graphic as icon
}
