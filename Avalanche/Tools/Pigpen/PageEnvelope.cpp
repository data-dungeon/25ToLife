/***********************************/
/*                                 */
/* PageEnvelope.cpp                */
/* Pigpen particle tool  12/15/00  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle envelope page          */
/*                                 */
/***********************************/

/* this is the dialog box that creates the envelopes to control a particle's behavior, e.g. size or alpha */

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
	// end of list-- must be last
	{	-1}
};

static ts_CheckBox ParticleCheckBoxList[] = {
	//	control id					data id					 index	flag bit						bShowData
	{	IDC_X_SIZE_HIDE,			CParticle::DATA_FLAGS,	-1,	ENVELOPE_HIDE_X_SIZE,	TRUE},
	{	IDC_Y_SIZE_HIDE,			CParticle::DATA_FLAGS,	-1,	ENVELOPE_HIDE_Y_SIZE,	TRUE},
	{	IDC_RED_HIDE,				CParticle::DATA_FLAGS,	-1,	ENVELOPE_HIDE_RED,		TRUE},
	{	IDC_GREEN_HIDE,			CParticle::DATA_FLAGS,	-1,	ENVELOPE_HIDE_GREEN,		TRUE},
	{	IDC_BLUE_HIDE,				CParticle::DATA_FLAGS,	-1,	ENVELOPE_HIDE_BLUE,		TRUE},
	{	IDC_ALPHA_HIDE,			CParticle::DATA_FLAGS,	-1,	ENVELOPE_HIDE_ALPHA,		TRUE},
	
	// end of list-- must be last
	{	-1}
};

static ts_CheckBox SwooshCheckBoxList[] = {
	//	control id					data id					 index	flag bit						bShowData
	{	IDC_X_SIZE_HIDE,			CSwoosh::DATA_FLAGS,		-1,	ENVELOPE_HIDE_X_SIZE,	TRUE},
	{	IDC_Y_SIZE_HIDE,			CSwoosh::DATA_FLAGS,		-1,	ENVELOPE_HIDE_Y_SIZE,	TRUE},
	{	IDC_RED_HIDE,				CSwoosh::DATA_FLAGS,		-1,	ENVELOPE_HIDE_RED,		TRUE},
	{	IDC_GREEN_HIDE,			CSwoosh::DATA_FLAGS,		-1,	ENVELOPE_HIDE_GREEN,		TRUE},
	{	IDC_BLUE_HIDE,				CSwoosh::DATA_FLAGS,		-1,	ENVELOPE_HIDE_BLUE,		TRUE},
	{	IDC_ALPHA_HIDE,			CSwoosh::DATA_FLAGS,		-1,	ENVELOPE_HIDE_ALPHA,		TRUE},
	
	// end of list-- must be last
	{	-1}
};

static ts_RadioButton ParticleRadioButtonList[] = {
	//	control id						data id										value						mask						bShowData
	{	IDC_SPLINE,						CParticle::DATA_ENVELOPE_BITFIELDS,	SPLINE_ENVELOPE,		ENVELOPE_TYPE_MASK,	TRUE},
	{	IDC_ADSR,						CParticle::DATA_ENVELOPE_BITFIELDS,	ADSR_ENVELOPE,			ENVELOPE_TYPE_MASK,	TRUE},
	{	IDC_STRAIGHT,					CParticle::DATA_ENVELOPE_BITFIELDS,	STRAIGHT_ENVELOPE,	ENVELOPE_TYPE_MASK,	TRUE},
	{	IDC_FLAT,						CParticle::DATA_ENVELOPE_BITFIELDS,	FLAT_ENVELOPE,			ENVELOPE_TYPE_MASK,	TRUE},

	{	IDC_X_SIZE_ENVELOPE,			CParticle::DATA_CURRENT_ENVELOPE,	X_SIZE_ENVELOPE,		ENVELOPE_INDEX_MASK,	TRUE},
	{	IDC_Y_SIZE_ENVELOPE,			CParticle::DATA_CURRENT_ENVELOPE,	Y_SIZE_ENVELOPE,		ENVELOPE_INDEX_MASK,	TRUE},
	{	IDC_RED_ENVELOPE,				CParticle::DATA_CURRENT_ENVELOPE,	RED_ENVELOPE,			ENVELOPE_INDEX_MASK,	TRUE},
	{	IDC_GREEN_ENVELOPE,			CParticle::DATA_CURRENT_ENVELOPE,	GREEN_ENVELOPE,		ENVELOPE_INDEX_MASK,	TRUE},
	{	IDC_BLUE_ENVELOPE,			CParticle::DATA_CURRENT_ENVELOPE,	BLUE_ENVELOPE,			ENVELOPE_INDEX_MASK,	TRUE},
	{	IDC_ALPHA_ENVELOPE,			CParticle::DATA_CURRENT_ENVELOPE,	ALPHA_ENVELOPE,		ENVELOPE_INDEX_MASK,	TRUE},

	// end of list-- must be last
	{	-1}
};

static ts_RadioButton SwooshRadioButtonList[] = {
	//	control id						data id										value						mask						bShowData
	{	IDC_SPLINE,						CSwoosh::DATA_ENVELOPE_BITFIELDS,	SPLINE_ENVELOPE,		ENVELOPE_TYPE_MASK,	TRUE},
	{	IDC_ADSR,						CSwoosh::DATA_ENVELOPE_BITFIELDS,	ADSR_ENVELOPE,			ENVELOPE_TYPE_MASK,	TRUE},
	{	IDC_STRAIGHT,					CSwoosh::DATA_ENVELOPE_BITFIELDS,	STRAIGHT_ENVELOPE,	ENVELOPE_TYPE_MASK,	TRUE},
	{	IDC_FLAT,						CSwoosh::DATA_ENVELOPE_BITFIELDS,	FLAT_ENVELOPE,			ENVELOPE_TYPE_MASK,	TRUE},

	{	IDC_X_SIZE_ENVELOPE,			CSwoosh::DATA_CURRENT_ENVELOPE,		X_SIZE_ENVELOPE,		ENVELOPE_INDEX_MASK,	TRUE},
	{	IDC_Y_SIZE_ENVELOPE,			CSwoosh::DATA_CURRENT_ENVELOPE,		Y_SIZE_ENVELOPE,		ENVELOPE_INDEX_MASK,	TRUE},
	{	IDC_RED_ENVELOPE,				CSwoosh::DATA_CURRENT_ENVELOPE,		RED_ENVELOPE,			ENVELOPE_INDEX_MASK,	TRUE},
	{	IDC_GREEN_ENVELOPE,			CSwoosh::DATA_CURRENT_ENVELOPE,		GREEN_ENVELOPE,		ENVELOPE_INDEX_MASK,	TRUE},
	{	IDC_BLUE_ENVELOPE,			CSwoosh::DATA_CURRENT_ENVELOPE,		BLUE_ENVELOPE,			ENVELOPE_INDEX_MASK,	TRUE},
	{	IDC_ALPHA_ENVELOPE,			CSwoosh::DATA_CURRENT_ENVELOPE,		ALPHA_ENVELOPE,		ENVELOPE_INDEX_MASK,	TRUE},

	// end of list-- must be last
	{	-1}
};

/* the hide-envelope check boxes */

static int nHideCheckBoxID[MAX_ENVELOPES] = {

	IDC_X_SIZE_HIDE,
	IDC_Y_SIZE_HIDE,
	IDC_RED_HIDE,
	IDC_GREEN_HIDE,
	IDC_BLUE_HIDE,
	IDC_ALPHA_HIDE
};

/******************* externs *****************************/

/******************* class functions *********************/

/* i am trying to avoid long message maps, so i have overridden OnNotify and OnCommand. they catch slider bars,
spinners, edit boxes, and button clicks for me and process them more generically. the message map stuff still works,
but in general, use the higher-level intercept routines (like OnKillFocusEdit to catch all edit box commands) */

BEGIN_MESSAGE_MAP(CEnvelopePage, CPropertyPage)
	//{{AFX_MSG_MAP(CEnvelopePage)
	ON_WM_CONTEXTMENU()
	ON_WM_DRAWITEM()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnvelopePage construction/destruction

CEnvelopePage::CEnvelopePage(CDocView *pParentView, CParticleSheet *pParentParticleSheet, CSwooshSheet *pParentSwooshSheet) : CPropertyPage(CEnvelopePage::IDD)
{
	/* this page can handle ownership by a particle OR a swoosh, but not both */

	m_pParentView = pParentView;
	m_pParentParticleSheet = pParentParticleSheet;
	m_pParentSwooshSheet = pParentSwooshSheet;
	m_ParticleSwoosh.Set(NULL, NULL);
	ASSERT((m_pParentParticleSheet) || (m_pParentSwooshSheet)); // one must be valid
	ASSERT((m_pParentParticleSheet == NULL) || (m_pParentSwooshSheet == NULL)); // must be only one

	/* no control point selected */

	m_nSelectedPoint = NO_CONTROL_POINT;

	/* set window area of graph area */

	m_GraphRect.left = GRAPH_X_OFFSET - GRAPH_BORDER;
	m_GraphRect.right = GRAPH_X_OFFSET + GRAPH_WIDTH + GRAPH_RIGHT_BORDER;
	m_GraphRect.top = GRAPH_Y_OFFSET - GRAPH_BORDER;
	m_GraphRect.bottom = GRAPH_Y_OFFSET + GRAPH_HEIGHT + GRAPH_BORDER;
}

/////////////////////////////////////////////////////////////////////////////
// CEnvelopePage operations

/* fill in the data in the dialog box */

void CEnvelopePage::ShowData(void)
{
	/* get current particle/swoosh from parent sheet */

	if (m_pParentParticleSheet)
		m_ParticleSwoosh.Set(m_pParentParticleSheet->GetParticle(), NULL);
	else
		m_ParticleSwoosh.Set(NULL, m_pParentSwooshSheet->GetSwoosh());

	/* set contents of edit boxes */

#if 0	// no edit boxes currently
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
		}
		pEditBox++;
	}
#endif //0

	/* set contents of checkboxes */

	ts_CheckBox *pCheckBox = m_pParentParticleSheet ? ParticleCheckBoxList : SwooshCheckBoxList;
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
		}
		pCheckBox++;
	}

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
		}
		pRadioButton++;
	}

	/* enable/disable normal buttons */

	if (m_ParticleSwoosh.IsValid() == false)
		GetDlgItem(IDC_PREVIEW_PARTICLE)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_PREVIEW_PARTICLE)->EnableWindow(TRUE);
	if (m_ParticleSwoosh.IsValid())
	{
		/* size envelopes-- if maintaining aspect ratio, y-size envelope is unused */

		if (m_ParticleSwoosh.IsMaintainAspectRatio())
		{
			GetDlgItem(IDC_Y_SIZE_ENVELOPE)->EnableWindow(FALSE);
			GetDlgItem(IDC_Y_SIZE_HIDE)->EnableWindow(FALSE);
			GetDlgItem(IDC_X_SIZE_ENVELOPE)->SetWindowText("Size");
		}
		else
		{
			GetDlgItem(IDC_Y_SIZE_ENVELOPE)->EnableWindow(TRUE);
			GetDlgItem(IDC_Y_SIZE_HIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_X_SIZE_ENVELOPE)->SetWindowText("X-Size");
		}

		/* color envelopes-- if using color pair, green and blue envelopes are unused */

		if (m_ParticleSwoosh.IsColorPair())
		{
			GetDlgItem(IDC_GREEN_ENVELOPE)->EnableWindow(FALSE);
			GetDlgItem(IDC_GREEN_HIDE)->EnableWindow(FALSE);
			GetDlgItem(IDC_BLUE_ENVELOPE)->EnableWindow(FALSE);
			GetDlgItem(IDC_BLUE_HIDE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RED_ENVELOPE)->SetWindowText("Color");
		}
		else
		{
			GetDlgItem(IDC_GREEN_ENVELOPE)->EnableWindow(TRUE);
			GetDlgItem(IDC_GREEN_HIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_BLUE_ENVELOPE)->EnableWindow(TRUE);
			GetDlgItem(IDC_BLUE_HIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_RED_ENVELOPE)->SetWindowText("Red");
		}
	}

	/* disable hide check box for current envelope */

	if (m_ParticleSwoosh.IsValid())
	{
		CButton *pButton = (CButton *) GetDlgItem(nHideCheckBoxID[m_ParticleSwoosh.GetCurrentEnvelopeIndex()]);
		ASSERT(pButton);	// if this hits you have an entry in a dialog box table that isn't in this dialog page
		pButton->SetCheck(0);
		pButton->EnableWindow(FALSE);
	}

	/* redraw graph */

	InvalidateRect(&m_GraphRect, FALSE);
}

/* update check box's state. check/uncheck depending on proper bit's state in proper flag unsigned long word */

void CEnvelopePage::UpdateCheckBox(int nID, bool bToggle)
{
	/* should be working on valid data */

	ASSERT(m_ParticleSwoosh.IsValid());

	/* find correct entry */

	ts_CheckBox *pCheckBox = m_pParentParticleSheet ? ParticleCheckBoxList : SwooshCheckBoxList;
	while ((pCheckBox->nID != nID) && (pCheckBox->nID != -1))	// id == -1 marks end of list
		pCheckBox++;
	if (pCheckBox->nID == -1)
	{
		ASSERT(FALSE);	// should always find it, otherwise edit box won't be supported
		return;
	}

	/* get pointer to data item */

	unsigned long *pulFlags = (unsigned long *) m_ParticleSwoosh.GetDataPointer(pCheckBox->nDataID);
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
		Preview__UpdateParticleOrSwoosh(m_ParticleSwoosh.GetParticle(), m_ParticleSwoosh.GetSwoosh());
	}
}

/* update radio button's. check/uncheck depending on if bitfield value matches radio button's assigned value */

void CEnvelopePage::UpdateRadioButton(int nID, bool bUpdate)
{
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
	unsigned long ulOldBitfield = *pulBitfield;

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
// CEnvelopePage message handlers

/* called when OK is selected, or when user presses enter. i don't want pressing enter to end the dialog, so
override and do nothing */

void CEnvelopePage::OnOK()
{
}

/* called when dialog is closed by sysmenu close button (X) */

void CEnvelopePage::OnCancel()
{
	/* from the help on OnCancel(): "If you implement the Cancel button in a modeless dialog box, you must */
	/* override the OnCancel member function and call DestroyWindow from within it. Don’t call the base-class */
	/* member function, because it calls EndDialog, which will make the dialog box invisible but not destroy it." */

	DestroyWindow();
}

/* called after window is destroyed */

void CEnvelopePage::PostNcDestroy()
{
	delete this;	// dialog was created by parent window using "new"
}

/* handle popup menu for dialog */

void CEnvelopePage::OnContextMenu(CWnd* pWnd, CPoint Point) 
{
#if 0
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
#endif
}

/* called when owner-draw flagged dialog buttons are going to be drawn. used to draw image bitmaps in
image frames, which are button controls that user can click on */

void CEnvelopePage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
}

/* called when mouse moves or buttons are pressed */

void CEnvelopePage::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_ParticleSwoosh.IsValid())
	{
		CEnvelope *pEnvelope = m_ParticleSwoosh.GetCurrentEnvelope();

		/* are they clicking in the graph area? it takes up the top portion of the window */

		ASSERT(m_nSelectedPoint == NO_CONTROL_POINT);	// since left button up clears selected point, should never have one
		if (point.y <= GRAPH_Y_OFFSET + GRAPH_HEIGHT)
		{
			/* are they clicking on a control point? */

#define CONTROL_POINT_EXTENT 8
			int nClosest = 1000000;
			for (int i = 0; i < MAX_CONTROL_POINTS; i++)
			{
				if (pEnvelope->IsValidControlPoint(i))
				{
					int nX = GRAPH_X_ORIGIN + (int) (pEnvelope->m_ControlPoints[i].x() * (float) GRAPH_WIDTH);
					int nY = GRAPH_Y_ORIGIN - (int) (pEnvelope->m_ControlPoints[i].y() * (float) GRAPH_ONE_Y);
					if ((point.x >= nX - CONTROL_POINT_EXTENT) &&
					    (point.x <= nX + CONTROL_POINT_EXTENT) &&
					    (point.y >= nY - CONTROL_POINT_EXTENT) &&
					    (point.y <= nY + CONTROL_POINT_EXTENT))
					{
						nX -= point.x;
						nY -= point.y;
						int nDistSquared = (nX * nX) + (nY * nY);
						if (nDistSquared < nClosest)
						{
							m_nSelectedPoint = i;
							m_nPointOffsetX = nX;	// keep point offset from original click point for easier control
							m_nPointOffsetY = nY;
							nClosest = nDistSquared;
						}
					}
				}
			}

			/* capture mouse for as long as they are moving a control point. for one thing, this allows the update */
			/* the point's position to continue as the mouse is moved over the other buttons in the dialog, each of */
			/* which is a window and will capture the mouse movement otherwise. it also allows the position update to */
			/* continue if the mouse is moved outside the dialog window */

			if (m_nSelectedPoint != NO_CONTROL_POINT)
				SetCapture();
		}
	}
}
void CEnvelopePage::OnLButtonUp(UINT nFlags, CPoint point)
{
	/* release selected point, if any */

	if (m_nSelectedPoint != NO_CONTROL_POINT)
	{
		m_nSelectedPoint = NO_CONTROL_POINT;
		ReleaseCapture();
	}
}
void CEnvelopePage::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_nSelectedPoint != NO_CONTROL_POINT)
	{
		bool bChanged = FALSE;
		ASSERT(m_ParticleSwoosh.IsValid());
		CEnvelope *pEnvelope = m_ParticleSwoosh.GetCurrentEnvelope();

		/* only midpoints can move in x */

		if ((m_nSelectedPoint == SPLINE_POINT1) || (m_nSelectedPoint == SPLINE_POINT2))
		{
			float fX = (float) (point.x - GRAPH_X_ORIGIN) / (float) GRAPH_WIDTH;
			if (fX < 0.0f)
				fX = 0.0f;
			if (fX > 1.0f)
				fX = 1.0f;

			/* for adsr, midpoints can't cross */

			if (pEnvelope->GetType() == ADSR_ENVELOPE)
			{
				if ((m_nSelectedPoint == SPLINE_POINT1) && (fX > pEnvelope->m_ControlPoints[SPLINE_POINT2].x()))
					fX = pEnvelope->m_ControlPoints[SPLINE_POINT2].x();
				else if ((m_nSelectedPoint == SPLINE_POINT2) && (fX < pEnvelope->m_ControlPoints[SPLINE_POINT1].x()))
					fX = pEnvelope->m_ControlPoints[SPLINE_POINT1].x();
			}

			/* update if necessary */

			if (pEnvelope->m_ControlPoints[m_nSelectedPoint].x() != fX)
			{
				pEnvelope->m_ControlPoints[m_nSelectedPoint].x(fX);
				bChanged = TRUE;
			}
		}

		/* move in y */

		float fY = (float) (GRAPH_Y_ORIGIN - point.y) / GRAPH_ONE_Y;
		if (m_nSelectedPoint >= FLOOR_POINT)
		{
			/* floor and ceiling are limited to 0..100 range */

			if (fY < 0.0f)
				fY = 0.0f;
			if (fY > 1.0f)
				fY = 1.0f;
		}
		else
		{
			/* spline control points can be anywhere in graph area */

			if (fY < GRAPH_LOW_Y)
				fY = GRAPH_LOW_Y;
			if (fY > GRAPH_HIGH_Y)
				fY = GRAPH_HIGH_Y;
		}

		/* floor and ceiling can't cross */

		if ((m_nSelectedPoint == FLOOR_POINT) && (fY > pEnvelope->m_ControlPoints[CEILING_POINT].y()))
			fY = pEnvelope->m_ControlPoints[CEILING_POINT].y();
		else if ((m_nSelectedPoint == CEILING_POINT) && (fY < pEnvelope->m_ControlPoints[FLOOR_POINT].y()))
			fY = pEnvelope->m_ControlPoints[FLOOR_POINT].y();

		/* update if necessary */

		if (pEnvelope->m_ControlPoints[m_nSelectedPoint].y() != fY)
		{
			pEnvelope->m_ControlPoints[m_nSelectedPoint].y(fY);
			bChanged = TRUE;

			/* for flat line, can adjust either endpoint */

			if (pEnvelope->GetType() == FLAT_ENVELOPE)
			{
				if (m_nSelectedPoint == SPLINE_POINT0)
					pEnvelope->m_ControlPoints[SPLINE_POINT3].y(pEnvelope->m_ControlPoints[SPLINE_POINT0].y());
				else if (m_nSelectedPoint == SPLINE_POINT3)
					pEnvelope->m_ControlPoints[SPLINE_POINT0].y(pEnvelope->m_ControlPoints[SPLINE_POINT3].y());
			}
		}

		/* if any change, update graph */

		if (bChanged)
		{
			InvalidateRect(&m_GraphRect, FALSE);	// redraw graph
			Preview__UpdateParticleOrSwoosh(m_ParticleSwoosh.GetParticle(), m_ParticleSwoosh.GetSwoosh());
		}
	}
}

void CEnvelopePage::OnPaint()
{
	int i;

	/* if window needs refreshing, then we need to re-paint graph */

	bool bGraphChanged = FALSE;
	if (GetUpdateRect(NULL) != 0)
		bGraphChanged = TRUE;

	/* let base class fire first */

	CPropertyPage::OnPaint();

	/* any custom drawing to do? */

	if (bGraphChanged == FALSE)
		return;

	/* documentation indicates that I should call BeginPaint and EndPaint here, but it doesn't seem to work */

//	PAINTSTRUCT PaintStruct;
//	CDC *pDC = BeginPaint(&PaintStruct);

	/* set up brushes i need */

	CDC *pDC = GetDC();	// client area DC
	HDC hDC = pDC->m_hDC;
	HBRUSH hEraseBrush = CreateSolidBrush(GetSysColor(COLOR_MENU));
	HPEN hErasePen = CreatePen(PS_SOLID, 2, GetSysColor(COLOR_MENU));
	HPEN hDarkGreenPen = CreatePen(PS_SOLID, 1, RGB(0,96,0));
	HPEN hGreenPen = CreatePen(PS_SOLID, 1, RGB(0,192,0));
	HPEN hRedPen = CreatePen(PS_SOLID, 1, RGB(192,0,0));
	HPEN hThickBlackPen = CreatePen(PS_SOLID, 3, RGB(0,0,0));
	HPEN hShadowPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));

	/* Rectangle gets filled with current brush and outlined with current pen. this is the extent of the graph area */

	HPEN hOldPen = (HPEN) SelectObject(hDC, hShadowPen);
	HBRUSH hOldBrush = (HBRUSH) SelectObject(hDC, hEraseBrush);
	Rectangle(hDC, GRAPH_X_OFFSET, GRAPH_Y_OFFSET, GRAPH_X_OFFSET + GRAPH_WIDTH + 1, GRAPH_Y_OFFSET + GRAPH_HEIGHT + 1);

	/* erase border areas to clear up portions of control points that hang over edge. i could, of course, clear the whole */
	/* area at once, but doing it this way makes the graph border look more persistent, since it never gets erased */

	SelectObject(hDC, hErasePen);
	/* left */	Rectangle(hDC, GRAPH_X_OFFSET - GRAPH_BORDER, GRAPH_Y_OFFSET - GRAPH_BORDER, GRAPH_X_OFFSET, GRAPH_Y_OFFSET + GRAPH_HEIGHT + GRAPH_BORDER);
	/* right */	Rectangle(hDC, GRAPH_X_OFFSET + GRAPH_WIDTH + 2, GRAPH_Y_OFFSET - GRAPH_BORDER, GRAPH_X_OFFSET + GRAPH_WIDTH + GRAPH_RIGHT_BORDER, GRAPH_Y_OFFSET + GRAPH_HEIGHT + GRAPH_BORDER);
	/* top */	Rectangle(hDC, GRAPH_X_OFFSET - GRAPH_BORDER, GRAPH_Y_OFFSET - GRAPH_BORDER, GRAPH_X_OFFSET + GRAPH_WIDTH + GRAPH_BORDER, GRAPH_Y_OFFSET);
	/* bottom */Rectangle(hDC, GRAPH_X_OFFSET - GRAPH_BORDER, GRAPH_Y_OFFSET + GRAPH_HEIGHT + 2, GRAPH_X_OFFSET + GRAPH_WIDTH + GRAPH_BORDER, GRAPH_Y_OFFSET + GRAPH_HEIGHT + GRAPH_BORDER);

	if (m_ParticleSwoosh.IsValid())
	{
		CEnvelope *pEnvelope;

		/* draw other visible envelopes */

		for (i = 0; i < MAX_ENVELOPES; i++)
		{
			CEnvelope *pEnvelope = m_ParticleSwoosh.GetEnvelope(i);
			if (pEnvelope == m_ParticleSwoosh.GetCurrentEnvelope())
				continue;	// don't draw current one-- it gets drawn below

			/* check hidden flag status */

			if (m_ParticleSwoosh.EnvelopeHidden(i))
				continue;

			/* maybe hide envelopes in certain situations */

			switch(i)
			{
			case Y_SIZE_ENVELOPE:
				/* hide y-size envelope if maintaining aspect ratio */

				if (m_ParticleSwoosh.IsMaintainAspectRatio())
					continue;
				break;
			case GREEN_ENVELOPE:
			case BLUE_ENVELOPE:
				/* hide green and blue envelopes if color control is color pair */

				if (m_ParticleSwoosh.IsColorPair())
					continue;
				break;
			}
	
			/* draw it in shadow color */

			DrawEnvelope(hDC, pEnvelope, hShadowPen, hThickBlackPen, 1);	// doesn't draw a clipped version
		}

		/* now working on current envelope */

		pEnvelope = m_ParticleSwoosh.GetCurrentEnvelope();

		/* draw floor and ceiling (clip lines) */

		SelectObject(hDC, hRedPen);
		m_nFloorY = GRAPH_Y_ORIGIN - (int) (pEnvelope->m_ControlPoints[FLOOR_POINT].y() * (float) GRAPH_ONE_Y);
		Rectangle(hDC, GRAPH_X_ORIGIN - GRAPH_CLIP_EXTEND_LEFT, m_nFloorY + 1, GRAPH_X_ORIGIN + GRAPH_WIDTH, m_nFloorY);
		m_nCeilingY = GRAPH_Y_ORIGIN - (int) (pEnvelope->m_ControlPoints[CEILING_POINT].y() * (float) GRAPH_ONE_Y);
		Rectangle(hDC, GRAPH_X_ORIGIN + 1, m_nCeilingY + 1, GRAPH_X_ORIGIN + GRAPH_WIDTH + GRAPH_CLIP_EXTEND_RIGHT, m_nCeilingY);

		/* draw 0%, 100% lines */

		SelectObject(hDC, hShadowPen);
		Rectangle(hDC, GRAPH_X_ORIGIN + 1, GRAPH_Y_ORIGIN + 1, GRAPH_X_ORIGIN + GRAPH_WIDTH, GRAPH_Y_ORIGIN);
		Rectangle(hDC, GRAPH_X_ORIGIN + 1, GRAPH_Y_ORIGIN - GRAPH_ONE_Y + 1, GRAPH_X_ORIGIN + GRAPH_WIDTH, GRAPH_Y_ORIGIN - GRAPH_ONE_Y);

		/* draw bias handles for spline envelope */

		if (pEnvelope->GetType() == SPLINE_ENVELOPE)
		{
			SelectObject(hDC, hGreenPen);
			MoveToEx(hDC, GRAPH_X_ORIGIN + (int) (pEnvelope->m_ControlPoints[SPLINE_POINT0].x() * (float) GRAPH_WIDTH), GRAPH_Y_ORIGIN - (int) (pEnvelope->m_ControlPoints[SPLINE_POINT0].y() * (float) GRAPH_ONE_Y), NULL);
			LineTo(hDC, GRAPH_X_ORIGIN + (int) (pEnvelope->m_ControlPoints[SPLINE_POINT1].x() * (float) GRAPH_WIDTH), GRAPH_Y_ORIGIN - (int) (pEnvelope->m_ControlPoints[SPLINE_POINT1].y() * (float) GRAPH_ONE_Y)); 
			MoveToEx(hDC, GRAPH_X_ORIGIN + (int) (pEnvelope->m_ControlPoints[SPLINE_POINT2].x() * (float) GRAPH_WIDTH), GRAPH_Y_ORIGIN - (int) (pEnvelope->m_ControlPoints[SPLINE_POINT2].y() * (float) GRAPH_ONE_Y), NULL);
			LineTo(hDC, GRAPH_X_ORIGIN + (int) (pEnvelope->m_ControlPoints[SPLINE_POINT3].x() * (float) GRAPH_WIDTH), GRAPH_Y_ORIGIN - (int) (pEnvelope->m_ControlPoints[SPLINE_POINT3].y() * (float) GRAPH_ONE_Y)); 
		}

		/* draw correct envelope type */

		DrawEnvelope(hDC, pEnvelope, hDarkGreenPen, hThickBlackPen, 2); 

		/* draw the control points */

#define DIAMOND_WIDTH	4
#define DIAMOND_HEIGHT	4
		SelectObject(hDC, hDarkGreenPen);
		for (i = 0; i < MAX_CONTROL_POINTS; i++)
		{
			if (i >= FLOOR_POINT)
			{
				/* change to red pen for clip line control points */

				SelectObject(hDC, hRedPen);
			}

			/* draw this one? */

			if (pEnvelope->IsValidControlPoint(i))
			{
				int nX = GRAPH_X_ORIGIN + (int) (pEnvelope->m_ControlPoints[i].x() * (float) GRAPH_WIDTH);
				int nY = GRAPH_Y_ORIGIN - (int) (pEnvelope->m_ControlPoints[i].y() * (float) GRAPH_ONE_Y);
				MoveToEx(hDC, nX, nY - DIAMOND_HEIGHT, NULL);
				LineTo(hDC, nX + DIAMOND_WIDTH, nY); 
				LineTo(hDC, nX, nY + DIAMOND_HEIGHT); 
				LineTo(hDC, nX - DIAMOND_WIDTH, nY);
				LineTo(hDC, nX, nY - DIAMOND_HEIGHT);
			}
		}
	}

	/* restore order */

	SelectObject(hDC, hOldBrush);
	SelectObject(hDC, hOldPen);
	DeleteObject(hEraseBrush);
	DeleteObject(hErasePen);
	DeleteObject(hDarkGreenPen);
	DeleteObject(hGreenPen);
	DeleteObject(hShadowPen);
	DeleteObject(hRedPen);
	DeleteObject(hThickBlackPen);
	ReleaseDC(pDC);
}

/* draw correct envelope type */

void CEnvelopePage::DrawEnvelope(HDC hDC, CEnvelope *pEnvelope, HPEN hPen1, HPEN hPen2, int nLoopCount)
{
	int i;
	int j;
	int nPointCount;
	float fT;
	float fOneMinusT;
	float fX;
	float fY;
	float fAdjustedSplinePoint1Y;
	float fAdjustedSplinePoint2Y;

	/* what type of envelope? */

	switch(pEnvelope->GetType())
	{
	case SPLINE_ENVELOPE:
		/* to give the user greater control, the area above and below the 0..100 area of the graph */
		/* is treated differently for the "bias handle" control points-- they are progressively exaggerated */
		/* as the point gets farther outside the 0..100 area */

		fAdjustedSplinePoint1Y = pEnvelope->AdjustSplinePoint(pEnvelope->m_ControlPoints[SPLINE_POINT1].y());
		fAdjustedSplinePoint2Y = pEnvelope->AdjustSplinePoint(pEnvelope->m_ControlPoints[SPLINE_POINT2].y());

		/* fill in graph points */

		for (i = 0, fT = 0.0f; i < SPLINE_SEGMENTS + 1; i++, fT += SPLINE_INTERVAL)
		{
			fOneMinusT = 1.0f - fT;
			fX = (fOneMinusT * fOneMinusT * fOneMinusT * pEnvelope->m_ControlPoints[SPLINE_POINT0].x()) + 
				  (3.0f * fOneMinusT * fOneMinusT * fT * pEnvelope->m_ControlPoints[SPLINE_POINT1].x()) +
				  (3.0f * fOneMinusT * fT * fT * pEnvelope->m_ControlPoints[SPLINE_POINT2].x()) +
				  (fT * fT * fT * pEnvelope->m_ControlPoints[SPLINE_POINT3].x());
			fY = (fOneMinusT * fOneMinusT * fOneMinusT * pEnvelope->m_ControlPoints[SPLINE_POINT0].y()) + 
				  (3.0f * fOneMinusT * fOneMinusT * fT * fAdjustedSplinePoint1Y) +
				  (3.0f * fOneMinusT * fT * fT * fAdjustedSplinePoint2Y) +
				  (fT * fT * fT * pEnvelope->m_ControlPoints[SPLINE_POINT3].y());
//			m_GraphPoint[i].x = GRAPH_X_ORIGIN + (int) (fX * (float) GRAPH_WIDTH);
//yuch-- we are just plotting the curve y = f(t), which looks like it will expressive enough, and is a lot
// easier to calculate in the game engine
			m_GraphPoint[i].x = GRAPH_X_ORIGIN + (int) (fT * (float) GRAPH_WIDTH);
			m_GraphPoint[i].y = GRAPH_Y_ORIGIN - (int) (fY * (float) GRAPH_ONE_Y);
		}
		nPointCount = SPLINE_SEGMENTS + 1;
		break;
	case ADSR_ENVELOPE:
		/* fill in graph points */

		for (i = 0; i <= SPLINE_POINT3; i++)
		{
			m_GraphPoint[i].x = GRAPH_X_ORIGIN + (int) (pEnvelope->m_ControlPoints[i].x() * (float) GRAPH_WIDTH);
			m_GraphPoint[i].y = GRAPH_Y_ORIGIN - (int) (pEnvelope->m_ControlPoints[i].y() * (float) GRAPH_ONE_Y);
		}
		nPointCount = 4;
		break;
	case STRAIGHT_ENVELOPE:
		/* fill in graph points */

		m_GraphPoint[0].x = GRAPH_X_ORIGIN;
		m_GraphPoint[0].y = GRAPH_Y_ORIGIN - (int) (pEnvelope->m_ControlPoints[SPLINE_POINT0].y() * (float) GRAPH_ONE_Y);
		m_GraphPoint[1].x = GRAPH_X_ORIGIN + GRAPH_WIDTH;
		m_GraphPoint[1].y = GRAPH_Y_ORIGIN - (int) (pEnvelope->m_ControlPoints[SPLINE_POINT3].y() * (float) GRAPH_ONE_Y);
		nPointCount = 2;
		break;
	case FLAT_ENVELOPE:
		/* fill in working points */

		m_GraphPoint[0].x = GRAPH_X_ORIGIN;
		m_GraphPoint[0].y = GRAPH_Y_ORIGIN - (int) (pEnvelope->m_ControlPoints[SPLINE_POINT0].y() * (float) GRAPH_ONE_Y);
		m_GraphPoint[1].x = GRAPH_X_ORIGIN + GRAPH_WIDTH;
		m_GraphPoint[1].y = m_GraphPoint[0].y;
		nPointCount = 2;
		break;
	default:
		ASSERT(FALSE);	// unknown envelope type
		break;
	}

	/* draw the line segments to plot the envelope-- first clipped to whole graph area, then to clip lines */

	SelectObject(hDC, hPen1);
	m_nTopClipY = GRAPH_Y_OFFSET;
	m_nBottomClipY = GRAPH_Y_OFFSET + GRAPH_HEIGHT;
	for (i = 0; i < nLoopCount; i++)
	{
		/* switch to second pen for clip-line version */

		if (i > 0)
		{
			SelectObject(hDC, hPen2);
			m_nTopClipY = m_nCeilingY;
			m_nBottomClipY = m_nFloorY;
		}

		/* draw segments */

		for (j = 0; j < nPointCount - 1; j++)	// segment count is point count - 1
			DrawClippedLine(hDC, j);
	}
}

/* draw a line clipped to the floor and ceiling values. intersection points are calculated by similar triangles */

void CEnvelopePage::DrawClippedLine(HDC hDC, int nIndex)
{
	int nPointCount;

	/* point to segment to draw */

	CPoint *pPoint = &m_GraphPoint[nIndex];

	/* check first point status */

	if (pPoint[0].y < m_nTopClipY)
	{
		/* first point above top clip line, check second point status */

		if (pPoint[1].y < m_nTopClipY)
		{
			/* both points above top clip line */

			m_ClippedPoint[0].x = pPoint[0].x;
			m_ClippedPoint[0].y = m_nTopClipY;
			m_ClippedPoint[1].x = pPoint[1].x;
			m_ClippedPoint[1].y = m_nTopClipY;
			pPoint = m_ClippedPoint;	// redirect to clipped point buffer
			nPointCount = 2;
		}
		else if (pPoint[1].y <= m_nBottomClipY)
		{
			/* first point above top, second between clip lines */

			m_ClippedPoint[0].x = pPoint[0].x;
			m_ClippedPoint[0].y = m_nTopClipY;
			m_ClippedPoint[1].x = pPoint[0].x + ((pPoint[1].x - pPoint[0].x) * (m_nTopClipY - pPoint[0].y) / (pPoint[1].y - pPoint[0].y));
			m_ClippedPoint[1].y = m_nTopClipY;
			m_ClippedPoint[2] = pPoint[1];
			pPoint = m_ClippedPoint;	// redirect to clipped point buffer
			nPointCount = 3;
		}
		else
		{
			/* first point above top, second below bottom */

			m_ClippedPoint[0].x = pPoint[0].x;
			m_ClippedPoint[0].y = m_nTopClipY;
			m_ClippedPoint[1].x = pPoint[0].x + ((pPoint[1].x - pPoint[0].x) * (m_nTopClipY - pPoint[0].y) / (pPoint[1].y - pPoint[0].y));
			m_ClippedPoint[1].y = m_nTopClipY;
			m_ClippedPoint[2].x = pPoint[1].x + ((pPoint[1].x - pPoint[0].x) * (m_nBottomClipY - pPoint[1].y) / (pPoint[1].y - pPoint[0].y));
			m_ClippedPoint[2].y = m_nBottomClipY;
			m_ClippedPoint[3].x = pPoint[1].x;
			m_ClippedPoint[3].y = m_nBottomClipY;
			pPoint = m_ClippedPoint;	// redirect to clipped point buffer
			nPointCount = 4;
		}
	}
	else if (pPoint[0].y <= m_nBottomClipY)
	{
		/* first point between clip lines, check second point status */

		if (pPoint[1].y < m_nTopClipY)
		{
			/* first point between clip lines, second above top */

			m_ClippedPoint[0] = pPoint[0];
			m_ClippedPoint[1].x = pPoint[1].x + ((pPoint[1].x - pPoint[0].x) * (m_nTopClipY - pPoint[1].y) / (pPoint[1].y - pPoint[0].y));
			m_ClippedPoint[1].y = m_nTopClipY;
			m_ClippedPoint[2].x = pPoint[1].x;
			m_ClippedPoint[2].y = m_nTopClipY;
			pPoint = m_ClippedPoint;	// redirect to clipped point buffer
			nPointCount = 3;
		}
		else if (pPoint[1].y <= m_nBottomClipY)
		{
			/* both points between clip lines, use original data */

			nPointCount = 2;
		}
		else
		{
			/* first point between clip lines, second below bottom */

			m_ClippedPoint[0] = pPoint[0];
			m_ClippedPoint[1].x = pPoint[1].x + ((pPoint[1].x - pPoint[0].x) * (m_nBottomClipY - pPoint[1].y) / (pPoint[1].y - pPoint[0].y));
			m_ClippedPoint[1].y = m_nBottomClipY;
			m_ClippedPoint[2].x = pPoint[1].x;
			m_ClippedPoint[2].y = m_nBottomClipY;
			pPoint = m_ClippedPoint;	// redirect to clipped point buffer
			nPointCount = 3;
		}
	}
	else
	{
		/* first point below bottom clip lines, check second point status */

		if (pPoint[1].y < m_nTopClipY)
		{
			/* first point below bottom, second above top */

			m_ClippedPoint[0].x = pPoint[0].x;
			m_ClippedPoint[0].y = m_nBottomClipY;
			m_ClippedPoint[1].x = pPoint[0].x + ((pPoint[1].x - pPoint[0].x) * (m_nBottomClipY - pPoint[0].y) / (pPoint[1].y - pPoint[0].y));
			m_ClippedPoint[1].y = m_nBottomClipY;
			m_ClippedPoint[2].x = pPoint[1].x + ((pPoint[1].x - pPoint[0].x) * (m_nTopClipY - pPoint[1].y) / (pPoint[1].y - pPoint[0].y));
			m_ClippedPoint[2].y = m_nTopClipY;
			m_ClippedPoint[3].x = pPoint[1].x;
			m_ClippedPoint[3].y = m_nTopClipY;
			pPoint = m_ClippedPoint;	// redirect to clipped point buffer
			nPointCount = 4;
		}
		else if (pPoint[1].y <= m_nBottomClipY)
		{
			/* first point below bottom, second between clip lines */

			m_ClippedPoint[0].x = pPoint[0].x;
			m_ClippedPoint[0].y = m_nBottomClipY;
			m_ClippedPoint[1].x = pPoint[0].x + ((pPoint[1].x - pPoint[0].x) * (m_nBottomClipY - pPoint[0].y) / (pPoint[1].y - pPoint[0].y));
			m_ClippedPoint[1].y = m_nBottomClipY;
			m_ClippedPoint[2] = pPoint[1];
			pPoint = m_ClippedPoint;	// redirect to clipped point buffer
			nPointCount = 3;
		}
		else
		{
			/* both points below bottom */

			m_ClippedPoint[0].x = pPoint[0].x;
			m_ClippedPoint[0].y = m_nBottomClipY;
			m_ClippedPoint[1].x = pPoint[1].x;
			m_ClippedPoint[1].y = m_nBottomClipY;
			pPoint = m_ClippedPoint;	// redirect to clipped point buffer
			nPointCount = 2;
		}
	}

	/* draw line segments. if this is the first segment of the graph, we have to do an initial MoveTo, otherwise */
	/* assume that cursor was left in the correct place after plotting previous line segment (which means that */
	/* calling routine always has to start with index 0) */

	if (nIndex == 0)
		MoveToEx(hDC, pPoint[0].x, pPoint[0].y, NULL);
	for (int i = 1; i < nPointCount; i++)
		LineTo(hDC, pPoint[i].x, pPoint[i].y);
}

/* catch button clicks. this routine does not have to handle all button clicks, but needs to return FALSE if
it gets passed a button it doesn't understand or want to deal with */

bool CEnvelopePage::OnButtonClicked(int nID)
{
	/* normal buttons */

	switch(nID)
	{
	case IDC_PREVIEW_PARTICLE:
		Preview__Create(m_pParentView->GetDocument(), NULL, NULL, m_ParticleSwoosh.GetParticle(), m_ParticleSwoosh.GetSwoosh(), NULL);
		return(TRUE);	// processed message
	}

	/* check boxes can all be handled by the same routine */

	ts_CheckBox *pCheckBox = m_pParentParticleSheet ? ParticleCheckBoxList : SwooshCheckBoxList;
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

	ts_RadioButton *pRadioButton = m_pParentParticleSheet ? ParticleRadioButtonList : SwooshRadioButtonList;
	while (pRadioButton->nID != -1)	// id == -1 marks end of list
	{
		if (nID == pRadioButton->nID)
		{
			UpdateRadioButton(nID, TRUE);

/************** special cases ***********************/
			/* check for additional actions when changing a check box */

			CEnvelope *pEnvelope = m_ParticleSwoosh.GetCurrentEnvelope();
			switch(nID)
			{
			case IDC_SPLINE:
			case IDC_STRAIGHT:
				Preview__UpdateParticleOrSwoosh(m_ParticleSwoosh.GetParticle(), m_ParticleSwoosh.GetSwoosh());
				break;
			case IDC_ADSR:
				/* make sure control points don't cross */

				if (pEnvelope->m_ControlPoints[SPLINE_POINT1].x() > pEnvelope->m_ControlPoints[SPLINE_POINT2].x())
					pEnvelope->m_ControlPoints[SPLINE_POINT1].x(pEnvelope->m_ControlPoints[SPLINE_POINT2].x());
				Preview__UpdateParticleOrSwoosh(m_ParticleSwoosh.GetParticle(), m_ParticleSwoosh.GetSwoosh());
				break;
			case IDC_FLAT:
				/* right endpoint should be same height as left */

				pEnvelope->m_ControlPoints[SPLINE_POINT3].y(pEnvelope->m_ControlPoints[SPLINE_POINT0].y());
				Preview__UpdateParticleOrSwoosh(m_ParticleSwoosh.GetParticle(), m_ParticleSwoosh.GetSwoosh());
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

bool CEnvelopePage::OnButtonUnpushed(int nID)
{
	m_nSelectedPoint = NO_CONTROL_POINT;

	/* didn't really handle the button */

	return(FALSE);
}

/* handle messages from dialog controls. this is intended to be a more generic approach than having every control
have to show up in the message map */

BOOL CEnvelopePage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	/* if wParam != 0, wParam is ID of control that sent message */

#if 0 //not used for this dialog
	if (wParam != 0)
	{
		NMHDR *pNMHDR = (NMHDR *) lParam;
		switch(pNMHDR->code)
		{
		}
	}
#endif

	/* let base class handle anything else */

	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}

/* this catches other control messages. from the help on CWnd::OnCommand, about wParam:

"The low-order word of wParam identifies the command ID of the menu item, control, or accelerator.
The high-order word of wParam specifies the notification message if the message is from a control.
If the message is from an accelerator, the high-order word is 1. If the message is from a menu,
the high-order word is 0." */

BOOL CEnvelopePage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (m_ParticleSwoosh.IsValid())
	{
		switch((wParam >> 16) & 0xffff)
		{
		case BN_CLICKED:
			/* user has clicked a button */

			if (OnButtonClicked(wParam & 0xffff) == TRUE)
				return(TRUE);	// processed message
			break;
		case BN_UNPUSHED:
			/* user has released a button */

			if (OnButtonUnpushed(wParam & 0xffff) == TRUE)
				return(TRUE);	// processed message
			break;
		}
	}

	/* let base class handle anything else */

	return CPropertyPage::OnCommand(wParam, lParam);
}

/* called when page is chosen as active page */

BOOL CEnvelopePage::OnSetActive()
{
	/* first call base class */

	CPropertyPage::OnSetActive();

	/* then show page */

	ShowData();
	return(TRUE);
}
