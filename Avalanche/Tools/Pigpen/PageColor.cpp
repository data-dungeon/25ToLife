/***********************************/
/*                                 */
/* PageColor.cpp                   */
/* Pigpen particle tool  1/3/01    */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle color property page    */
/*                                 */
/***********************************/

/* this is the color page of the particle property sheet. each page is a dialog */

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
/*** have to have double lists since this page can be used by particles and swooshes ***/

static ts_EditBox EditBoxList[] = {
	//	control id								data id								index		min		max			delta		format	bIsInteger
	
	// end of list-- must be last
	{	-1}
};

static ts_CheckBox CheckBoxList[] = {
	//	control id							data id						 index	flag bit							bShowData
	
	// end of list-- must be last
	{	-1}
};

static ts_RadioButton ParticleRadioButtonList[] = {
	//	control id						data id							value						mask					bShowData
	{	IDC_COLOR_PAIR,				CParticle::DATA_BITFIELDS,	PART_COLOR_PAIR,		PART_COLOR_MASK,	TRUE},
	{	IDC_COLOR_RGB,					CParticle::DATA_BITFIELDS,	PART_COLOR_RGB,		PART_COLOR_MASK,	TRUE},

	// end of list-- must be last
	{	-1}
};

static ts_RadioButton SwooshRadioButtonList[] = {
	//	control id						data id							value						mask					bShowData
	{	IDC_COLOR_PAIR,				CSwoosh::DATA_BITFIELDS,	SWOO_COLOR_PAIR,		SWOO_COLOR_MASK,	TRUE},
	{	IDC_COLOR_RGB,					CSwoosh::DATA_BITFIELDS,	SWOO_COLOR_RGB,		SWOO_COLOR_MASK,	TRUE},

	// end of list-- must be last
	{	-1}
};

/******************* externs *****************************/

/******************* class functions *********************/
 
/* i am trying to avoid long message maps, so i have overridden OnNotify and OnCommand. they catch slider bars,
spinners, edit boxes, and button clicks for me and process them more generically. the message map stuff still works,
but in general, use the higher-level intercept routines (like OnKillFocusEdit to catch all edit box commands) */

BEGIN_MESSAGE_MAP(CColorPage, CPropertyPage)
	//{{AFX_MSG_MAP(CColorPage)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorPage construction/destruction

CColorPage::CColorPage(CDocView *pParentView, CParticleSheet *pParentParticleSheet, CSwooshSheet *pParentSwooshSheet) : CPropertyPage(CColorPage::IDD)
{
	/* this page can handle ownership by a particle OR a swoosh, but not both */

	m_pParentView = pParentView;
	m_pParentParticleSheet = pParentParticleSheet;
	m_pParentSwooshSheet = pParentSwooshSheet;
	m_ParticleSwoosh.Set(NULL, NULL);
	ASSERT((m_pParentParticleSheet) || (m_pParentSwooshSheet)); // one must be valid
	ASSERT((m_pParentParticleSheet == NULL) || (m_pParentSwooshSheet == NULL)); // must be only one

	/* look for operator error-- i'm looking for overlap in bitfield masks */

#ifdef _DEBUG
	ts_RadioButton *pRadioButton = m_pParentParticleSheet ? ParticleRadioButtonList : SwooshRadioButtonList;
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

	//{{AFX_DATA_INIT(CColorPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
// CColorPage operations

/* fill in the data in the dialog box with the current particle */

void CColorPage::ShowData(void)
{
	/* get current particle/swoosh from parent sheet */

	if (m_pParentParticleSheet)
		m_ParticleSwoosh.Set(m_pParentParticleSheet->GetParticle(), NULL);
	else
		m_ParticleSwoosh.Set(NULL, m_pParentSwooshSheet->GetSwoosh());

#if 0	// no edit boxes currently
	/* set contents of edit boxes */

	ts_EditBox *pEditBox = EditBoxList;
	while (pEditBox->nID != -1)	// id == -1 marks end of list
	{
		CEdit *pEdit = (CEdit *) GetDlgItem(pEditBox->nID);
		ASSERT(pEdit);		// if this hits you have an entry in a dialog box table that isn't in this dialog page
		if (m_ParticleSwoosh.IsValid() == false)
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
#endif //0

#if 0	// no check boxes currently
	/* set contents of checkboxes */

	ts_CheckBox *pCheckBox = CheckBoxList;
	while (pCheckBox->nID != -1)	// id == -1 marks end of list
	{
		CButton *pButton = (CButton *) GetDlgItem(pCheckBox->nID);
		ASSERT(pButton);	// if this hits you have an entry in a dialog box table that isn't in this dialog page
		if (m_ParticleSwoosh.IsValid() == false)
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
#endif //0

	/* set contents of radio buttons */

	ts_RadioButton *pRadioButton = m_pParentParticleSheet ? ParticleRadioButtonList : SwooshRadioButtonList;
	while (pRadioButton->nID != -1)	// id == -1 marks end of list
	{
		CButton *pButton = (CButton *) GetDlgItem(pRadioButton->nID);
		ASSERT(pButton);	// if this hits you have an entry in a dialog box table that isn't in this dialog page
		if (m_ParticleSwoosh.IsValid() == false)
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

	/* enable/disable normal buttons */

	if (m_ParticleSwoosh.IsValid() == false)
	{
		GetDlgItem(IDC_PREVIEW_PARTICLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_PICK_COLOR1)->EnableWindow(false);
		GetDlgItem(IDC_PICK_COLOR2)->EnableWindow(false);
	}
	else
	{
		GetDlgItem(IDC_PREVIEW_PARTICLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_PICK_COLOR1)->EnableWindow(m_ParticleSwoosh.IsColorPair());
		GetDlgItem(IDC_PICK_COLOR2)->EnableWindow(m_ParticleSwoosh.IsColorPair());
	}
}

/* update radio button's. check/uncheck depending on if bitfield value matches radio button's assigned value */

void CColorPage::UpdateRadioButton(int nID, bool bUpdate)
{
	unsigned long ulOldBitfield;

	/* should be working on valid data */

	ASSERT(m_ParticleSwoosh.IsValid());

	/* find correct entry */

	ts_RadioButton *pRadioButton = m_pParentParticleSheet ? ParticleRadioButtonList : SwooshRadioButtonList;
	while ((pRadioButton->nID != nID) && (pRadioButton->nID != -1))	// id == -1 marks end of list
		pRadioButton++;
	if (pRadioButton->nID == -1)
	{
		ASSERT(FALSE);	// should always find it, otherwise edit box won't be supported
		return;
	}

	/* get pointer to data item */

	unsigned long *pulBitfield = (unsigned long *) m_ParticleSwoosh.GetDataPointer(pRadioButton->nDataID);
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
		Preview__UpdateParticleOrSwoosh(m_ParticleSwoosh.GetParticle(), m_ParticleSwoosh.GetSwoosh());
	}
}

/////////////////////////////////////////////////////////////////////////////
// CColorPage message handlers

/* catch button clicks. this routine does not have to handle all button clicks, but needs to return FALSE if
it gets passed a button it doesn't understand or want to deal with */

bool CColorPage::OnButtonClicked(int nID)
{
	ASSERT(m_ParticleSwoosh.IsValid());	// buttons shouldn't be enabled if no owning record

	/* normal buttons */

	switch(nID)
	{
	case IDC_PREVIEW_PARTICLE:
		Preview__Create(m_pParentView->GetDocument(), NULL, NULL, m_ParticleSwoosh.GetParticle(), m_ParticleSwoosh.GetSwoosh(), NULL);
		return(TRUE);	// processed message
	case IDC_COLOR1:
	case IDC_COLOR2:
		int nIndex = (nID == IDC_COLOR1) ? 0 : 1;
		COLORREF CurrentColor = m_ParticleSwoosh.GetColor(nIndex);
		CColorDialog ColorDialog(
			CurrentColor,							// COLORREF clrInit = 0
			CC_FULLOPEN | CC_SOLIDCOLOR,		// DWORD dwFlags = 0-- CC_FULLOPEN makes the dialog come up with the custom color picker open
			NULL);									// CWnd* pParentWnd = NULL 
		if (ColorDialog.DoModal() != IDOK)
			return(TRUE);	// processed message

		/* copy new color into proper place */

		if (ColorDialog.GetColor() != CurrentColor)
		{
		 	m_ParticleSwoosh.SetColor(nIndex, ColorDialog.GetColor());

			/* repaint color box */

			GetDlgItem(nID)->Invalidate();	// so it gets repainted, intercepted by OnDrawItem

			/* doc has changed */

			CPigpenDoc *pDoc = m_pParentView->GetDocument();
			pDoc->SetModifiedFlag(MODIFIED_FLAG);
			Preview__UpdateParticleOrSwoosh(m_ParticleSwoosh.GetParticle(), m_ParticleSwoosh.GetSwoosh());
		}
		return(TRUE);	// processed message
	}

#if 0	// no check boxes currently
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
#endif

	/* radio buttons can all be handled by the same routine */

	ts_RadioButton *pRadioButton = m_pParentParticleSheet ? ParticleRadioButtonList : SwooshRadioButtonList;
	while (pRadioButton->nID != -1)	// id == -1 marks end of list
	{
		if (nID == pRadioButton->nID)
		{
			UpdateRadioButton(nID, TRUE);

/************** special cases ***********************/
			/* check for additional actions when changing a check box */

			switch(nID)
			{
			case IDC_COLOR_PAIR:
			case IDC_COLOR_RGB:
				/* redraw color boxes-- they are activated/inactivated by radio buttons */

				GetDlgItem(IDC_COLOR1)->Invalidate();	// so it gets repainted, intercepted by OnDrawItem
				GetDlgItem(IDC_COLOR2)->Invalidate();	// so it gets repainted, intercepted by OnDrawItem

				/* if current envelope index is green or blue and they have turned on color pair, switch */

				if ((m_ParticleSwoosh.IsColorPair()) &&
					 ((m_ParticleSwoosh.GetCurrentEnvelopeIndex() == GREEN_ENVELOPE) ||
					  (m_ParticleSwoosh.GetCurrentEnvelopeIndex() == BLUE_ENVELOPE)))
					m_ParticleSwoosh.SetCurrentEnvelopeIndex(RED_ENVELOPE);
				break;
			}
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

/* called when owner-draw flagged dialog buttons are going to be drawn. used to draw graphic images */

void CColorPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (m_ParticleSwoosh.IsValid())
	{
		int nIndex;
		switch(nIDCtl)
		{
		default:
			ASSERT(FALSE);
			/* fall through */
		case IDC_COLOR1:
			nIndex = 0;
			break;
		case IDC_COLOR2:
			nIndex = 1;
			break;
		}

		/* Rectangle gets filled with current brush and outlined with current pen */

		COLORREF Color;
		Color = m_ParticleSwoosh.GetColor(nIndex);
		if (m_ParticleSwoosh.IsColorRGB())
			Color = GetSysColor(COLOR_MENU);
		HBRUSH hBrush = CreateSolidBrush(Color);
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
}

/* this catches other control messages. from the help on CWnd::OnCommand, about wParam:

"The low-order word of wParam identifies the command ID of the menu item, control, or accelerator.
The high-order word of wParam specifies the notification message if the message is from a control.
If the message is from an accelerator, the high-order word is 1. If the message is from a menu,
the high-order word is 0." */

BOOL CColorPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch((wParam >> 16) & 0xffff)
	{
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

BOOL CColorPage::OnSetActive()
{
	/* first call base class */

	CPropertyPage::OnSetActive();

	/* then show page */

	ShowData();
	return(TRUE);
}
