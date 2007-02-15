/********************************************************************
	created:	2003/03/01
	created:	1:3:2003   11:33
	file base:	PPTooltip
	file ext:	cpp
	author:		Eugene Pustovoyt
	
	purpose:	
*********************************************************************/
#include "stdafx.h"
#include "CustomToolTip.h"
#include "CustomControls.h"

namespace AttrControls
{

/////////////////////////////////////////////////////////////////////////////
// CustomToolTip

CustomToolTip::CustomToolTip(HWND hWnd) : CustomControl(hWnd)
{
	m_hParentWnd = ::GetParent(m_hWnd);

	m_rgnShadow = CreateRectRgn(0, 0, 0, 0);
	m_rgnToolTip = CreateRectRgn(0, 0, 0, 0);

	m_ptOriginal.x = -1;
	m_ptOriginal.y = -1;

	m_nIndexCurrentWnd = ICustomToolTip::TOOLTIP_TOOL_NOEXIST;
	m_nIndexDisplayWnd = ICustomToolTip::TOOLTIP_TOOL_NOEXIST;

	SetDelayTime(TTDT_INITIAL, 500);
	SetDelayTime(TTDT_AUTOPOP, 5000);
	SetDelayTime(TTDT_RESHOW, 200);
	SetNotify(FALSE);
	SetDirection();
	SetBehaviour();
	SetDefaultStyles();
	SetDefaultColors();
	SetDefaultSizes();
	SetDefaultFont();
	SetEffectBk(ICustomToolTip::TOOLTIP_EFFECT_SOLID);
	RemoveAllTools();
	Activate(FALSE);
}

CustomToolTip::~CustomToolTip()
{
	RemoveAllTools();
	RemoveAllNamesOfResource();

	m_nLengthLines.RemoveAll();
	m_nHeightLines.RemoveAll();

	DeleteObject(m_rgnToolTip);
	DeleteObject(m_rgnShadow);
}


/////////////////////////////////////////////////////////////////////////////
// CustomToolTip message handlers

LRESULT CustomToolTip::OnDestroy() 
{
	KillTimers();
	return 0;
}

LRESULT CustomToolTip::OnKillFocus(HWND hNewWnd) 
{
	Pop();
	return 0;
}

LRESULT CustomToolTip::OnEraseBkgnd(HDC hDC) 
{
	return FALSE;
}

void CustomToolTip::Pop()
{
	KillTimers();
	ShowWindow(m_hWnd,SW_HIDE);
}

LRESULT CustomToolTip::SendNotify(LPPOINT pt, ICustomToolTip::ToolTipInfo & ti) 
{ 
//	TRACE(_T("CustomToolTip::SendNotify()\n")); 
 	// Make sure this is a valid window  
	if (!IsWindow(m_hWnd)) 
		return 0L; 
  
	// See if the user wants to be notified  
	if (!GetNotify()) 
		return 0L; 
  	
	ICustomToolTip::ToolTipDisplay lpnm; 
	ICustomToolTip::ToolTipInfo tiCopy = ti; 
	ClientToScreen(ti.m_hWnd,(LPPOINT)&tiCopy.m_rectBounds);
	ClientToScreen(ti.m_hWnd,((LPPOINT)&tiCopy.m_rectBounds)+1);
	ScreenToClient(m_hParentWnd,(LPPOINT)&tiCopy.m_rectBounds);
	ScreenToClient(m_hParentWnd,((LPPOINT)&tiCopy.m_rectBounds)+1);

	lpnm.pt = pt;  
	lpnm.ti = &tiCopy; 
	lpnm.hdr.hwndFrom = m_hWnd; 
	lpnm.hdr.idFrom   = GetDlgCtrlID(m_hWnd); 
	lpnm.hdr.code     = ICustomToolTip::UDM_TOOLTIP_DISPLAY; 
	 
	::SendMessage(m_hNotifyWnd, WM_NOTIFY, lpnm.hdr.idFrom, (LPARAM)&lpnm);  
	RECT rcBound = ti.m_rectBounds;  
	ti = tiCopy; 
	ti.m_rectBounds = rcBound;  
	return 0L; 
}

LRESULT CustomToolTip::OnPaint() 
{
	if (!IsEnabledIndexTool(m_nIndexCurrentWnd))
		return 0;

	m_nIndexDisplayWnd = m_nIndexCurrentWnd;
	m_nIndexCurrentWnd = ICustomToolTip::TOOLTIP_TOOL_NOEXIST;

	PAINTSTRUCT ps;
	BeginPaint(m_hWnd,&ps);

	RECT rect;
	GetClientRect(m_hWnd,&rect);
	rect.right--;
	rect.bottom--;

	// Create a memory device-context. This is done to help reduce
	// screen flicker, since we will paint the entire control to the
	// off screen device context first.CDC memDC;
	HDC hMemDC;
	HBITMAP hBitmap;

	hMemDC = ::CreateCompatibleDC(ps.hdc);

	hBitmap = ::CreateCompatibleBitmap(ps.hdc, rect.right - rect.left, rect.bottom - rect.top);

	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC,hBitmap);

	::BitBlt(hMemDC,0, 0, rect.right - rect.left, rect.bottom - rect.top,ps.hdc, 0,0, SRCCOPY);

	OnDraw(hMemDC, rect);

	//Copy the memory device context back into the original DC via BitBlt().
	::BitBlt(ps.hdc,0, 0, rect.right - rect.left, rect.bottom - rect.top, hMemDC, 0,0, SRCCOPY);
	
	//Cleanup resources.
	SelectObject(hMemDC,hOldBitmap);
	DeleteDC(hMemDC);
   DeleteObject(hBitmap); 

	EndPaint(m_hWnd,&ps);

	return 0;
}

void CustomToolTip::OnDraw(HDC hDC, RECT rect)
{
	HBRUSH brBackground = CreateSolidBrush(m_crColor [TOOLTIP_COLOR_BK_BEGIN]);
	HBRUSH brShadow = CreateSolidBrush(m_crColor [TOOLTIP_COLOR_SHADOW]);
	HBRUSH brBorder = CreateSolidBrush(m_crColor [TOOLTIP_COLOR_BORDER]);
	
	SetBkMode(hDC,TRANSPARENT); 

	//Sets clip region of the tooltip and draws the shadow if you need
	if (m_toolInfo.m_nStyles & ICustomToolTip::TOOLTIP_SHADOW)
	{
		//Draws the shadow for the tooltip
		int nRop2Mode = SetROP2(hDC,R2_MASKPEN);
		FillRgn(hDC,m_rgnShadow,brShadow);
		SetROP2(hDC,nRop2Mode);
		rect.right -= m_nSizes[TOOLTIP_SIZE_SHADOW_CX];
		rect.bottom -= m_nSizes[TOOLTIP_SIZE_SHADOW_CY];
	}
	SelectClipRgn(hDC,m_rgnToolTip);

	OnDrawBackground(hDC, &rect);

	//Draws the main region's border of the tooltip
	FrameRgn(hDC,m_rgnToolTip,brBorder, m_nSizes[TOOLTIP_SIZE_BORDER_CX], m_nSizes[TOOLTIP_SIZE_BORDER_CY]);

	//Gets the rectangle to draw the tooltip text
	::InflateRect(&rect,-((int)m_nSizes[TOOLTIP_SIZE_MARGIN_CX]),-((int)m_nSizes[TOOLTIP_SIZE_MARGIN_CY]));
	if ((m_nLastDirection == ICustomToolTip::TOOLTIP_RIGHT_BOTTOM) || (m_nLastDirection == ICustomToolTip::TOOLTIP_LEFT_BOTTOM))
		rect.top += m_nSizes[TOOLTIP_SIZE_HEIGHT_ANCHOR];
	else
		rect.bottom -= m_nSizes[TOOLTIP_SIZE_HEIGHT_ANCHOR];

	// Draw the icon
	if (m_toolInfo.m_hIcon != NULL)
	{
		POINT ptIcon;
		ptIcon.x = m_nSizes[TOOLTIP_SIZE_MARGIN_CX];
		ptIcon.y = rect.top;
		if (m_toolInfo.m_nStyles & ICustomToolTip::TOOLTIP_ICON_VCENTER_ALIGN)
			ptIcon.y = rect.top + ((rect.bottom-rect.top) - m_szToolIcon.cy) / 2;
		else if (m_toolInfo.m_nStyles & ICustomToolTip::TOOLTIP_ICON_BOTTOM_ALIGN)
			ptIcon.y = rect.bottom - m_szToolIcon.cy;
		//First variant
//		pDC->DrawIcon(m_nSizes[TOOLTIP_SIZE_MARGIN_CX], rect.top + ((rect.bottom - rect.top) - m_szToolIcon.cy) / 2, m_toolInfo.m_hIcon);

		//Second variant
		::DrawState(hDC,NULL,NULL,(LPARAM)m_toolInfo.m_hIcon,0,ptIcon.x,ptIcon.y,m_szToolIcon.cx,m_szToolIcon.cy, DSS_NORMAL|DST_ICON);

		//Third variant
//		DrawIconEx(pDC->m_hDC, ptIcon.x, ptIcon.y, m_toolInfo.m_hIcon, m_szToolIcon.cx, 
//			m_szToolIcon.cy, 0, NULL, DI_NORMAL);

		rect.left += m_szToolIcon.cx + m_nSizes[TOOLTIP_SIZE_MARGIN_CX]; 
	}

	//Aligns tooltip's text
	if (m_toolInfo.m_nStyles & ICustomToolTip::TOOLTIP_BOTTOM_ALIGN)
	{
		rect.top = rect.bottom - m_szTextTooltip.cy;
	}
	else if (m_toolInfo.m_nStyles & ICustomToolTip::TOOLTIP_VCENTER_ALIGN)
	{
		rect.top += ((rect.bottom-rect.top) - m_szTextTooltip.cy) / 2;
	}

	//Prints the tooltip's text
	PrintTitleString(hDC, &rect, m_toolInfo.m_sTooltip, FALSE);

	::DeleteObject(brBackground);
	::DeleteObject(brShadow);
	::DeleteObject(brBorder);
}

void CustomToolTip::OnDrawBackground(HDC hDC, LPRECT pRect)
{
	switch (m_toolInfo.m_nEffect)
	{
	default:
		::SetBkColor(hDC, m_crColor[TOOLTIP_COLOR_BK_BEGIN]);
		::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, pRect, NULL, 0, NULL);
		break;
	case ICustomToolTip::TOOLTIP_EFFECT_HGRADIENT:
		FillGradient(hDC, pRect, m_crColor[TOOLTIP_COLOR_BK_BEGIN], m_crColor [TOOLTIP_COLOR_BK_END], TRUE);
		break;
	case ICustomToolTip::TOOLTIP_EFFECT_VGRADIENT:
		FillGradient(hDC, pRect, m_crColor[TOOLTIP_COLOR_BK_BEGIN], m_crColor [TOOLTIP_COLOR_BK_END], FALSE);
		break;
	case ICustomToolTip::TOOLTIP_EFFECT_HCGRADIENT:
		{
			RECT tempRect(*pRect);
			tempRect.right = pRect->left + ((pRect->right - pRect->left)/2);
			FillGradient(hDC, &tempRect, m_crColor[TOOLTIP_COLOR_BK_BEGIN], m_crColor [TOOLTIP_COLOR_BK_END], TRUE);
			tempRect.left = pRect->left + ((pRect->right - pRect->left)/2);
			tempRect.right = pRect->right;
			FillGradient(hDC, &tempRect, m_crColor[TOOLTIP_COLOR_BK_END], m_crColor [TOOLTIP_COLOR_BK_BEGIN], TRUE);
		}
		break;
	case ICustomToolTip::TOOLTIP_EFFECT_VCGRADIENT:
		{
			RECT tempRect(*pRect);
			tempRect.bottom = pRect->top + ((pRect->bottom - pRect->top)/2);
			FillGradient(hDC, &tempRect, m_crColor[TOOLTIP_COLOR_BK_BEGIN], m_crColor [TOOLTIP_COLOR_BK_END], FALSE);
			tempRect.bottom = pRect->bottom;
			tempRect.top = pRect->top + ((pRect->bottom - pRect->top)/2);
			FillGradient(hDC, &tempRect, m_crColor[TOOLTIP_COLOR_BK_END], m_crColor [TOOLTIP_COLOR_BK_BEGIN], FALSE);
		}
		break;
	case ICustomToolTip::TOOLTIP_EFFECT_3HGRADIENT:
		{
			RECT tempRect(*pRect);
			tempRect.right = pRect->left + ((pRect->right - pRect->left)/2);
			FillGradient(hDC, &tempRect, m_crColor[TOOLTIP_COLOR_BK_BEGIN], m_crColor [TOOLTIP_COLOR_BK_MID], TRUE);
			tempRect.right = pRect->right;
			tempRect.left = pRect->left + ((pRect->right - pRect->left)/2);
			FillGradient(hDC, &tempRect, m_crColor[TOOLTIP_COLOR_BK_MID], m_crColor [TOOLTIP_COLOR_BK_END], TRUE);
		}
		break;
	case ICustomToolTip::TOOLTIP_EFFECT_3VGRADIENT:
		{
			RECT tempRect(*pRect);
			tempRect.bottom = pRect->top + ((pRect->bottom - pRect->top)/2);
			FillGradient(hDC, &tempRect, m_crColor[TOOLTIP_COLOR_BK_BEGIN], m_crColor [TOOLTIP_COLOR_BK_MID], FALSE);
			tempRect.bottom = pRect->bottom;
			tempRect.top = pRect->top + ((pRect->bottom - pRect->top)/2);
			FillGradient(hDC, &tempRect, m_crColor[TOOLTIP_COLOR_BK_MID], m_crColor [TOOLTIP_COLOR_BK_END], FALSE);
		}
		break;
#ifdef TOOLTIP_USE_SHADE
	case ICustomToolTip::TOOLTIP_EFFECT_NOISE:
	case ICustomToolTip::TOOLTIP_EFFECT_DIAGSHADE:
	case ICustomToolTip::TOOLTIP_EFFECT_HSHADE:
	case ICustomToolTip::TOOLTIP_EFFECT_VSHADE:
	case ICustomToolTip::TOOLTIP_EFFECT_HBUMP:
	case ICustomToolTip::TOOLTIP_EFFECT_VBUMP:
	case ICustomToolTip::TOOLTIP_EFFECT_SOFTBUMP:
	case ICustomToolTip::TOOLTIP_EFFECT_HARDBUMP:
	case ICustomToolTip::TOOLTIP_EFFECT_METAL:
		m_dNormal.Draw(hDC,0,0);
		break;
#endif
	}
}

void CustomToolTip::RelayEvent(MSG* pMsg)
{
	assert(m_hParentWnd);
	HWND hWnd = NULL;
	POINT pt;
	TupString str;
	int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_NOEXIST;

//	ICustomToolTip::ToolTipInfo  Info;
		
	switch(pMsg->message)
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
		case WM_NCLBUTTONDOWN:
		case WM_NCLBUTTONDBLCLK:
		case WM_NCRBUTTONDOWN:
		case WM_NCRBUTTONDBLCLK:
		case WM_NCMBUTTONDOWN:
		case WM_NCMBUTTONDBLCLK:
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			// The user has interupted the current tool - dismiss it
			Pop();
			break;
		case WM_MOUSEMOVE:
//		TRACE (_T("OnMouseMove()\n"));
			if (!m_bActive)
				return;

			if ((m_ptOriginal.x == pMsg->pt.x && m_ptOriginal.y == pMsg->pt.y) || 
				(m_nIndexCurrentWnd == ICustomToolTip::TOOLTIP_TOOL_HELPER))
				return; //Mouse pointer was not move

			//Check Active window
			if (!(m_nStyles & ICustomToolTip::TOOLTIP_SHOW_INACTIVE))
			{
				hWnd = ::GetActiveWindow();
				if (!hWnd)
					return;
			}

			m_ptOriginal = pMsg->pt; //Stores the mouse's coordinates
			
			//Gets the real window under the mouse
			pt = pMsg->pt;
			ScreenToClient(m_hParentWnd,&pt);
		
			nIndexTool = FindTool(pt);

			if (!IsExistTool(nIndexTool))
			{
				Pop();
				m_nIndexCurrentWnd = ICustomToolTip::TOOLTIP_TOOL_NOEXIST;
				m_nIndexDisplayWnd = ICustomToolTip::TOOLTIP_TOOL_NOEXIST;
				return;
			}
			else
			{
				//The window under the mouse is exist
				if (nIndexTool == m_nIndexDisplayWnd)
				{
					if (IsVisible())
					{
						// if we are over the tooltip too then close if requested
						if (IsCursorInToolTip() && !(m_toolInfo.m_nBehaviour & ICustomToolTip::TOOLTIP_NOCLOSE_OVER))
						{
							Pop();
							m_nIndexCurrentWnd = ICustomToolTip::TOOLTIP_TOOL_NOEXIST;
							m_nIndexDisplayWnd = ICustomToolTip::TOOLTIP_TOOL_NOEXIST;
							return;
						}
						//Now the tooltip is visible
						if ((m_toolInfo.m_nBehaviour & ICustomToolTip::TOOLTIP_CLOSE_LEAVEWND))
							return;
					}
					if (m_toolInfo.m_nBehaviour & ICustomToolTip::TOOLTIP_MULTIPLE_SHOW)
					{
						SetNewToolTip(nIndexTool);
					}
					else Pop();
				}
				else
				{
					SetNewToolTip(nIndexTool, !IsVisible());
				}
			}
			break;
	}
}

void CustomToolTip::SetNewToolTip(int nIndexTool, BOOL bWithDelay /* = TRUE */)
{
//	TRACE (_T("CustomToolTip::SetNewToolTip(Index = 0x%X)\n"), nIndexTool);
	
	m_nIndexDisplayWnd = ICustomToolTip::TOOLTIP_TOOL_NOEXIST; //reset the displayed window
	Pop();

	//Gets the info about current tool
	if (!GetTool(nIndexTool, m_toolInfo))
		return;

	//Remembers the pointer to the current window
	m_nIndexCurrentWnd = nIndexTool;

	//Start the show timer
	if (bWithDelay)
		SetTimer(m_hWnd,TOOLTIP_SHOW, m_nTimeInitial, NULL);
	else
		SetTimer(m_hWnd,TOOLTIP_SHOW, m_nTimeReShow, NULL);
}

LRESULT CustomToolTip::OnTimer(UINT nIDEvent) 
{
	POINT pt = m_ptOriginal, point;
	TupString str;
	int nIndexTool = ICustomToolTip::TOOLTIP_TOOL_HELPER;

	switch (nIDEvent)
	{
		case TOOLTIP_SHOW:
//		TRACE(_T("OnTimerShow\n"));
			Pop();
			if (m_nIndexCurrentWnd != ICustomToolTip::TOOLTIP_TOOL_HELPER)
			{
				GetCursorPos(&pt);
				point = pt;
				ScreenToClient(m_hParentWnd,&point);
				nIndexTool = FindTool(point);
			}
			if ((nIndexTool == m_nIndexCurrentWnd) && (pt.x == m_ptOriginal.x && pt.y == m_ptOriginal.y) && IsEnabledIndexTool(nIndexTool))
			{
				PrepareDisplayToolTip(&pt);
				if (m_nTimeAutoPop) //Don't hide window if autopop is 0
					SetTimer(m_hWnd,TOOLTIP_HIDE, m_nTimeAutoPop, NULL);
			}
			break;
		case TOOLTIP_HIDE:
//		TRACE(_T("OnTimerHide\n"));
			if (!IsCursorInToolTip() || 
				!IsVisible() || 
				!(m_toolInfo.m_nBehaviour & ICustomToolTip::TOOLTIP_NOCLOSE_OVER))
				Pop();
			break;
	}
	return 0;
}

BOOL CustomToolTip::IsEnabledIndexTool(int nIndex)
{
	return (BOOL)(IsExistTool(nIndex) || (nIndex == ICustomToolTip::TOOLTIP_TOOL_HELPER));
}

BOOL CustomToolTip::IsCursorInToolTip() const
{
    assert(m_hParentWnd);
	
    // Is tooltip visible?
    if (!IsVisible() || !IsWindow(m_hWnd))
		return FALSE;
	
    POINT pt;
    GetCursorPos(&pt);
	
	HWND hWnd = WindowFromPoint(pt);
	
	return (hWnd == m_hWnd);
}

void CustomToolTip::KillTimers(UINT nIDTimer /* = NULL */)
{
//	TRACE (_T("CustomToolTip::KillTimers\n"));
	if (nIDTimer == NULL)
	{
		KillTimer(m_hWnd,TOOLTIP_SHOW);
		KillTimer(m_hWnd,TOOLTIP_HIDE);
	}
	else if (nIDTimer == TOOLTIP_SHOW)
	{
		KillTimer(m_hWnd,TOOLTIP_SHOW);
	}
	else if (nIDTimer == TOOLTIP_HIDE)
	{
		KillTimer(m_hWnd,TOOLTIP_HIDE);
	}
}

void CustomToolTip::PrepareDisplayToolTip(LPPOINT pt)
{
//	TRACE (_T("CustomToolTip::DisplayToolTip()\n"));
	
	//Fills default members
	if (!(m_toolInfo.m_nMask & ICustomToolTip::TOOLTIP_MASK_STYLES))
	{
		m_toolInfo.m_nStyles = m_nStyles;
	}
	if (!(m_toolInfo.m_nMask & ICustomToolTip::TOOLTIP_MASK_EFFECT))
	{
		m_toolInfo.m_nEffect = m_nEffect;
		m_toolInfo.m_nGranularity = m_nGranularity;
	}
	if (!(m_toolInfo.m_nMask & ICustomToolTip::TOOLTIP_MASK_COLORS))
	{
		m_toolInfo.m_crBegin = m_crColor[TOOLTIP_COLOR_BK_BEGIN];
		m_toolInfo.m_crMid = m_crColor[TOOLTIP_COLOR_BK_MID];
		m_toolInfo.m_crEnd = m_crColor[TOOLTIP_COLOR_BK_END];
	}
	if (!(m_toolInfo.m_nMask & ICustomToolTip::TOOLTIP_MASK_DIRECTION))
	{
		m_toolInfo.m_nDirection = m_nDirection;
	}
	if (!(m_toolInfo.m_nMask & ICustomToolTip::TOOLTIP_MASK_BEHAVIOUR))
	{
		m_toolInfo.m_nBehaviour = m_nBehaviour;
	}
	
	//Send notify
	SendNotify(pt, m_toolInfo);

	//If string and icon are not exist then exit
	if ((m_toolInfo.m_hIcon == NULL) && m_toolInfo.m_sTooltip.IsEmpty())
		return;
	
	//calculate the width and height of the box dynamically
    SIZE sz = GetTooltipSize(m_toolInfo.m_sTooltip);
	m_szTextTooltip = sz; //Stores the real size of the tooltip's text
	
	//Gets size of the current icon
	m_szToolIcon = GetSizeIcon(m_toolInfo.m_hIcon);
	if (m_szToolIcon.cx || m_szToolIcon.cy)
	{
		sz.cx += m_szToolIcon.cx;
		if (m_szTextTooltip.cx != 0)
			sz.cx += m_nSizes[TOOLTIP_SIZE_MARGIN_CX]; //If text is exist then adds separator
		sz.cy = max(m_szToolIcon.cy, sz.cy);
	}

	//Gets size of the tooltip with margins
	sz.cx += m_nSizes[TOOLTIP_SIZE_MARGIN_CX] * 2;
	sz.cy += m_nSizes[TOOLTIP_SIZE_MARGIN_CY] * 2 + m_nSizes[TOOLTIP_SIZE_HEIGHT_ANCHOR];
	if (m_toolInfo.m_nStyles & ICustomToolTip::TOOLTIP_SHADOW)
	{
		sz.cx += m_nSizes[TOOLTIP_SIZE_SHADOW_CX];
		sz.cy += m_nSizes[TOOLTIP_SIZE_SHADOW_CY];
	}
	
	RECT rect;
	rect.left = rect.top = 0;
	rect.right = sz.cx;
	rect.bottom = sz.cy;
	
#ifdef TOOLTIP_USE_SHADE
	//If needed to create the bitmap of the background effect
	switch (m_toolInfo.m_nEffect)
	{
		case ICustomToolTip::TOOLTIP_EFFECT_NOISE:
		case ICustomToolTip::TOOLTIP_EFFECT_DIAGSHADE:
		case ICustomToolTip::TOOLTIP_EFFECT_HSHADE:
		case ICustomToolTip::TOOLTIP_EFFECT_VSHADE:
		case ICustomToolTip::TOOLTIP_EFFECT_HBUMP:
		case ICustomToolTip::TOOLTIP_EFFECT_VBUMP:
		case ICustomToolTip::TOOLTIP_EFFECT_SOFTBUMP:
		case ICustomToolTip::TOOLTIP_EFFECT_HARDBUMP:
		case ICustomToolTip::TOOLTIP_EFFECT_METAL:
			SetShade(rect, m_toolInfo.m_nEffect, m_nGranularity, 5, m_toolInfo.m_crBegin);
	}
#endif

	DisplayToolTip(pt, &rect);
}

void CustomToolTip::DisplayToolTip(LPPOINT pt, LPRECT rect)
{
	//Calculate the placement on the screen
	CalculateInfoBoxRect(pt, rect);

	::SetWindowPos(m_hWnd,NULL, 
                 rect->left, rect->top,
                 (rect->right - rect->left) + 2, (rect->bottom - rect->top) + 2,
                 SWP_SHOWWINDOW|SWP_NOCOPYBITS|SWP_NOACTIVATE|SWP_NOZORDER);

	HRGN rgnCombo;
	rgnCombo = ::CreateRectRgn(0,0,0,0);
	if (m_toolInfo.m_nStyles & ICustomToolTip::TOOLTIP_SHADOW)
	{
		rect->right -= m_nSizes[TOOLTIP_SIZE_SHADOW_CX];
		rect->bottom -= m_nSizes[TOOLTIP_SIZE_SHADOW_CY];
	}
	::DeleteObject(m_rgnToolTip);
	SIZE size;
	size.cx = rect->right - rect->left;
	size.cy = rect->bottom - rect->top;
	GetWindowRegion(m_rgnToolTip, size, *pt);
	CopyRgn(rgnCombo,m_rgnToolTip);
	if (m_toolInfo.m_nStyles & ICustomToolTip::TOOLTIP_SHADOW)
	{
		DeleteObject(m_rgnShadow);
		m_rgnShadow = CreateRectRgn(0, 0, 0, 0);
		CopyRgn(m_rgnShadow,m_rgnToolTip);
		OffsetRgn(m_rgnShadow,m_nSizes[TOOLTIP_SIZE_SHADOW_CX], m_nSizes[TOOLTIP_SIZE_SHADOW_CY]);
		CombineRgn(rgnCombo,rgnCombo,m_rgnShadow, RGN_OR);
	}
	::SetWindowRgn(m_hWnd,rgnCombo, FALSE);
	
	// There is a bug with layered windows and NC changes in Win2k
    // As a workaround, redraw the entire window if the NC area changed.
    // Changing the anchor point is the ONLY thing that will change the
    // position of the client area relative to the window during normal
    // operation.
    // RedrawWindow(NULL, NULL, RDW_UPDATENOW| RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
}

RECT CustomToolTip::GetWindowRegion(HRGN &rgn, SIZE sz, POINT pt)
{
	RECT rect;
	rect.left = rect.top = 0;
	rect.right = sz.cx;
	rect.bottom = sz.cy;
	HRGN rgnRect;
	HRGN rgnAnchor;
	POINT ptAnchor[3];
	ptAnchor[0] = pt;
	::ScreenToClient(m_hWnd,&ptAnchor[0]);
	
	switch (m_nLastDirection)
	{
		case ICustomToolTip::TOOLTIP_LEFT_TOP:
		case ICustomToolTip::TOOLTIP_RIGHT_TOP:
			rect.bottom -= m_nSizes[TOOLTIP_SIZE_HEIGHT_ANCHOR];
			ptAnchor [1].y = ptAnchor [2].y = rect.bottom;
			break;
		case ICustomToolTip::TOOLTIP_LEFT_BOTTOM:
		case ICustomToolTip::TOOLTIP_RIGHT_BOTTOM:
			rect.top += m_nSizes[TOOLTIP_SIZE_HEIGHT_ANCHOR];
			ptAnchor [1].y = ptAnchor [2].y = rect.top;
			break;
	}
	
	//Gets the region for rectangle with the text
	if (m_toolInfo.m_nStyles & ICustomToolTip::TOOLTIP_ROUNDED)
	{
		rgnRect = ::CreateRoundRectRgn(rect.left, rect.top, rect.right + 1, rect.bottom + 1, 
			m_nSizes[TOOLTIP_SIZE_ROUNDED_CX], m_nSizes[TOOLTIP_SIZE_ROUNDED_CY]);
	}
	else 
	{
		rgnRect = ::CreateRectRgn(rect.left, rect.top, rect.right + 1, rect.bottom + 1);
	}
	
	//Gets the region for anchor
	if (m_toolInfo.m_nStyles & ICustomToolTip::TOOLTIP_ANCHOR)
	{
		switch (m_nLastDirection)
		{
			case ICustomToolTip::TOOLTIP_LEFT_TOP:
			case ICustomToolTip::TOOLTIP_LEFT_BOTTOM:
				ptAnchor [1].x = rect.right - m_nSizes[TOOLTIP_SIZE_MARGIN_ANCHOR];
				ptAnchor [2].x = ptAnchor [1].x - m_nSizes[TOOLTIP_SIZE_WIDTH_ANCHOR];
				break;
			case ICustomToolTip::TOOLTIP_RIGHT_TOP:
			case ICustomToolTip::TOOLTIP_RIGHT_BOTTOM:
				ptAnchor [1].x = rect.left + m_nSizes[TOOLTIP_SIZE_MARGIN_ANCHOR];
				ptAnchor [2].x = ptAnchor [1].x + m_nSizes[TOOLTIP_SIZE_WIDTH_ANCHOR];
				break;
		}
		rgnAnchor = ::CreatePolygonRgn(ptAnchor, 3, ALTERNATE);
	}
	else
	{
		rgnAnchor = ::CreateRectRgn(0, 0, 0, 0);
	}
	
	rgn = ::CreateRectRgn(0, 0, 0, 0);
	::CombineRgn(rgn, rgnRect, rgnAnchor, RGN_OR);
	
	::DeleteObject(rgnAnchor);
	::DeleteObject(rgnRect);
	
	return rect;
}

///////////////////////////////////////////////////
//	Gets the size of the current tooltip text
//
//	Parameters:
//		none
//
//	Return value:
//		Size of current tooltip text
///////////////////////////////////////////////////
SIZE CustomToolTip::GetTooltipSize(TupString str)
{
	//Gets max windows rectangle
	RECT rect;
	::GetWindowRect(m_hWnd,&rect);

	//Creates compatibility context device in memory
	HDC wDC = ::GetWindowDC(NULL);

	HDC memDC;
	HBITMAP bitmap;
	memDC = ::CreateCompatibleDC(wDC);
	bitmap = ::CreateCompatibleBitmap(wDC, rect.right - rect.left, rect.bottom - rect.top);
	HBITMAP oldBitmap = (HBITMAP) SelectObject(memDC,bitmap);

	//Prints the string on device context for gets minimal size of the rectangle 
	//of the tooltip
	SIZE sz = PrintTitleString(memDC, &rect, str);

	::SelectObject(memDC,oldBitmap);
	::DeleteDC(memDC);
	::DeleteObject(bitmap);
	::ReleaseDC(NULL,wDC);

	//Returns minimal rectangle of the tooltip
	return sz;
}

SIZE CustomToolTip::GetSizeIcon(HICON hIcon) const
{
	ICONINFO ii;
	SIZE sz;
	sz.cx = 0;
	sz.cy = 0;

	if (hIcon != NULL)
	{
		// Gets icon dimension
		::ZeroMemory(&ii, sizeof(ICONINFO));
		if (::GetIconInfo(hIcon, &ii))
		{
			sz.cx = (DWORD)(ii.xHotspot * 2);
			sz.cy = (DWORD)(ii.yHotspot * 2);
			//release icon mask bitmaps
			if(ii.hbmMask)
				::DeleteObject(ii.hbmMask);
			if(ii.hbmColor)
				::DeleteObject(ii.hbmColor);
		}
	}
	return sz;
}

///////////////////////////////////////////////////
//	Calculates the real rect for the tooltip with margins
//
//	Parameters:
//		pt		[in] - the mouse coordinates (screen coordinates)
//		sz		[in] - the size of the tooltip text
//
//	Return value:
//		The rectangle when the tooltip will draw (screen coordinates)
///////////////////////////////////////////////////
void CustomToolTip::CalculateInfoBoxRect(LPPOINT pt, LPRECT rect)
{
	HMONITOR hMonitor = MonitorFromPoint(*pt,MONITOR_DEFAULTTONEAREST);

	MONITORINFO monitorInfo;
	monitorInfo.cbSize = sizeof(monitorInfo);
	GetMonitorInfo(hMonitor, &monitorInfo);

	RECT rWindow = monitorInfo.rcWork;
/*
	m_szToolIcon = GetSizeIcon(m_toolInfo.m_hIcon);
	if (m_szToolIcon.cx || m_szToolIcon.cy)
	{
		sz.cx += m_szToolIcon.cx + m_nSizes[TOOLTIP_SIZE_MARGIN_CX];
		sz.cy = max(m_szToolIcon.cy, sz.cy);
	}

	//Gets size of the tooltip with margins
	sz.cx += m_nSizes[TOOLTIP_SIZE_MARGIN_CX] * 2;
	sz.cy += m_nSizes[TOOLTIP_SIZE_MARGIN_CY] * 2 + m_nSizes[TOOLTIP_SIZE_HEIGHT_ANCHOR];
	if (m_toolInfo.m_nStyles & TOOLTIP_SHADOW)
	{
		rWindow.right -= m_nSizes[TOOLTIP_SIZE_SHADOW_CX];
		rWindow.bottom -= m_nSizes[TOOLTIP_SIZE_SHADOW_CY];
		sz.cx += m_nSizes[TOOLTIP_SIZE_SHADOW_CX];
		sz.cy += m_nSizes[TOOLTIP_SIZE_SHADOW_CY];
	}
*/	
//	CRect rect;
//	rect.SetRect(0, 0, sz.cx, sz.cy);
//	CRect rectCopy = *rect;
	
	//Offset the rect from the mouse pointer
	m_nLastDirection = m_toolInfo.m_nDirection;
	
	if (!TestHorizDirection(pt->x, rect->right - rect->left, rWindow.right, m_nLastDirection, rect))
	{
		m_nLastDirection = GetNextHorizDirection(m_nLastDirection);
		TestHorizDirection(pt->x, rect->right - rect->left, rWindow.right, m_nLastDirection, rect);
	}
	if (!TestVertDirection(pt->y, rect->bottom - rect->top, rWindow.bottom, m_nLastDirection, rect))
	{
		m_nLastDirection = GetNextVertDirection(m_nLastDirection);
		TestVertDirection(pt->y, rect->bottom - rect->top, rWindow.bottom, m_nLastDirection, rect);
	}

	//Returns the rect of the tooltip
	if ((m_toolInfo.m_nStyles & ICustomToolTip::TOOLTIP_SHADOW) && 
		((m_nLastDirection == ICustomToolTip::TOOLTIP_LEFT_TOP) || (m_nLastDirection == ICustomToolTip::TOOLTIP_LEFT_BOTTOM)))
	{
		::OffsetRect(rect,m_nSizes[TOOLTIP_SIZE_SHADOW_CX], m_nSizes[TOOLTIP_SIZE_SHADOW_CY]);
	}
}

///////////////////////////////////////////////////
//	Gets the next horizontal direction
//
//	Parameters:
//		nDirection		[in] - the current direction
//
//	Return value:
//		The next horizontal direction
///////////////////////////////////////////////////
int CustomToolTip::GetNextHorizDirection(int nDirection) const
{
	switch (nDirection)
	{
	case ICustomToolTip::TOOLTIP_LEFT_TOP:
		nDirection = ICustomToolTip::TOOLTIP_RIGHT_TOP;
		break;
	case ICustomToolTip::TOOLTIP_RIGHT_TOP:
		nDirection = ICustomToolTip::TOOLTIP_LEFT_TOP;
		break;
	case ICustomToolTip::TOOLTIP_LEFT_BOTTOM:
		nDirection = ICustomToolTip::TOOLTIP_RIGHT_BOTTOM;
		break;
	case ICustomToolTip::TOOLTIP_RIGHT_BOTTOM:
		nDirection = ICustomToolTip::TOOLTIP_LEFT_BOTTOM;
		break;
	}
	return nDirection;
}

///////////////////////////////////////////////////
//	Gets the next vertical direction
//
//	Parameters:
//		nDirection		[in] - the current direction
//
//	Return value:
//		The next vertical direction
///////////////////////////////////////////////////
int CustomToolTip::GetNextVertDirection(int nDirection) const
{
	switch (nDirection)
	{
	case ICustomToolTip::TOOLTIP_LEFT_TOP:
		nDirection = ICustomToolTip::TOOLTIP_LEFT_BOTTOM;
		break;
	case ICustomToolTip::TOOLTIP_LEFT_BOTTOM:
		nDirection = ICustomToolTip::TOOLTIP_LEFT_TOP;
		break;
	case ICustomToolTip::TOOLTIP_RIGHT_TOP:
		nDirection = ICustomToolTip::TOOLTIP_RIGHT_BOTTOM;
		break;
	case ICustomToolTip::TOOLTIP_RIGHT_BOTTOM:
		nDirection = ICustomToolTip::TOOLTIP_RIGHT_TOP;
		break;
	}
	return nDirection;
}

BOOL CustomToolTip::TestHorizDirection(int x, int cx, int w_cx, int nDirection, LPRECT rect) const
{
	int left,right;
	
	switch (nDirection)
	{
	case ICustomToolTip::TOOLTIP_LEFT_TOP:
	case ICustomToolTip::TOOLTIP_LEFT_BOTTOM:
		right = ((x + (int)m_nSizes[TOOLTIP_SIZE_MARGIN_ANCHOR]) > w_cx) ? w_cx : (x + m_nSizes[TOOLTIP_SIZE_MARGIN_ANCHOR]);
		left = right - cx;
		break;
	case ICustomToolTip::TOOLTIP_RIGHT_TOP:
	case ICustomToolTip::TOOLTIP_RIGHT_BOTTOM:
		left = (x < (int)m_nSizes[TOOLTIP_SIZE_MARGIN_ANCHOR]) ? 0 : (x - m_nSizes[TOOLTIP_SIZE_MARGIN_ANCHOR]);
		right = left + cx;
		break;
	}

	BOOL bTestOk = ((left >= 0) && (right <= w_cx)) ? TRUE : FALSE;
	if (bTestOk)
	{
		rect->left = left;
		rect->right = right;
	}

	return bTestOk;
}

BOOL CustomToolTip::TestVertDirection(int y, int cy, int w_cy, int nDirection, LPRECT rect) const
{
	int top, bottom;

	switch (nDirection)
	{
	case ICustomToolTip::TOOLTIP_LEFT_TOP:
	case ICustomToolTip::TOOLTIP_RIGHT_TOP:
		bottom = y;
		top = bottom - cy;
		break;
	case ICustomToolTip::TOOLTIP_LEFT_BOTTOM:
	case ICustomToolTip::TOOLTIP_RIGHT_BOTTOM:
		top = y;
		bottom = top + cy;
		break;
	}

	BOOL bTestOk = ((top >= 0) && (bottom <= w_cy)) ? TRUE : FALSE;
	if (bTestOk)
	{
		rect->top = top;
		rect->bottom = bottom;
	}

	return bTestOk;
}

/////////////////////////////////////////////////////////////////
// Gets the system tooltip's logfont
/////////////////////////////////////////////////////////////////
LPLOGFONT CustomToolTip::GetSystemToolTipFont() const
{
    static LOGFONT LogFont;

    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    if (!SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0))
        return FALSE;

    memcpy(&LogFont, &(ncm.lfStatusFont), sizeof(LOGFONT));

    return &LogFont; 
}

/////////////////////////////////////////////////////////////////
// Prints the formating string of the tooltip
//  
// Parameters:
//		pDC			 - [in] The device context to print the string
//      str			 - [in] The formating string for drawing
//      rect         - [in] The rectangle to draws the tooltip
//		bEnableAlign - [in] If TRUE align's operators are enables.
//
// Return values:
//      None.
//---------------------------------------------------------------
// Format of string:
//  <b> text </b> - The bold string
//  <i> text </i> - The italic string 
//  <u> text </u> - The underline string
//  <s> text </s> - The strike out string
//
//  <cb=0x123456> text </cb> - The background color (RGB (12,34,56))
//  <ct=0x123456> text </ct> - The text color (RGB (12,34,56))
//
//  <cbi=1> text </cbi> - The index background color(0 - F)
//  <cti=1> text </cti> - The index text color(0 - F)
//
//	<al> or <al_l> - sets align to left edge
//  <al_c> - sets align to the center 
//  <al_r> - sets align to the right edge
//
//	<hr=100%> - the horizontal line with length 100%
//  <hr=32> - the horizontal line with length 32 pixels.
//
//	<a="link"> text </a> - The hyperlink 
//
//  <h=1> text </h> - hot zone (number of zone)
//
//	<br[=1]> - new line (numbers of the lines)
//  <t[=1]> - tabulation (number of tabulations)
//
//  <bmp> - draws the bitmap
//  <img[=0x0000]> - draws the image from image list
////////////////////////////////////////////////////////////////
SIZE CustomToolTip::PrintTitleString(HDC hDC, LPRECT rect, TupString str, BOOL bCalculate /* = TRUE */)
{
	enum
	{
		CMD_NONE = 0,
		CMD_BOLD,
		CMD_ITALIC,
		CMD_STRIKE,
		CMD_UNDERLINE,
		CMD_COLOR_TEXT,
		CMD_COLOR_TEXT_INDEX,
		CMD_COLOR_BK,
		CMD_COLOR_BK_INDEX,
		CMD_NEW_LINE,
		CMD_TABULATION,
		CMD_HORZ_LINE,
		CMD_HORZ_LINE_PERCENT,
		CMD_DRAW_BITMAP,
		CMD_DRAW_IMAGE_LIST
	};

	enum
	{
		STATE_WAIT_BEGIN_TAG = 0,
		STATE_WAIT_END_TAG,
		STATE_WAIT_TEXT_TAG,
		STATE_WAIT_BEGIN_NUMBER,
		STATE_WAIT_TEXT_NUMBER,
		STATE_WAIT_BEGIN_TEXT,
		STATE_WAIT_TEXT,
		STATE_WAIT_PERCENT,
		STATE_WAIT_BEGIN_RESOURSE,
		STATE_WAIT_END_RESOURCE
	};

	enum
	{
		ALIGN_LEFT = 0,
		ALIGN_CENTER,
		ALIGN_RIGHT
	};

	//Clears the length of the lines
	if (bCalculate)
	{
		m_nLengthLines.RemoveAll();
		m_nHeightLines.RemoveAll();
	}
	
	int nHeightBitmap = 0;
	int nLine = 0;
	int nCmd = CMD_NONE;
	int nState = STATE_WAIT_BEGIN_TAG;
	int nAlign = ALIGN_LEFT;
	BOOL bCloseTag = FALSE;
//	HBITMAP hBitmap = m_toolInfo.hBitmap;

	SIZE sz;
	sz.cx = sz.cy = 0;
	SIZE szLine;
	szLine.cx = szLine.cy = 0;

	if (str.IsEmpty())
		return sz;

	POINT	pt;
	pt.x = rect->left;
	pt.y = rect->top;

	POINT ptCur = pt;
	
	// Copies default logfont's structure
	LOGFONT lf;
	memcpy(&lf, &m_logFont, sizeof(LOGFONT));

	HFONT font = ::CreateFontIndirect(&lf);

	HFONT oldFont = (HFONT) ::SelectObject(hDC,font);

	TEXTMETRIC tm;
	GetTextMetrics(hDC,&tm);
	int nHeight = tm.tmHeight; //The height of the font
	int nWidth = tm.tmAveCharWidth; //The width of the font

	TupString strTag = _T("");  // Tag's name 
	TupString strText = _T(""); // The current text to output
	TupString strRes = _T("");  // The resource's name

	UINT nParam = 0;
	int nLineHeight = bCalculate ? nHeight : m_nHeightLines.GetAt(0); //The height of the current line
	
	TupArray<UINT> percent;
	percent.Add(0);
	
	int nTemp = 0; //the temporary variable
	BOOL bFirstOutput = TRUE;
	
	for (int i = 0; i <= str.GetLength(); i++)
	{
		if (i < str.GetLength())
		{
			//Searches the command and parameters in the string
			switch (nState)
			{
			case STATE_WAIT_BEGIN_TAG:
				//We're wait the begin of the tag or the code - new line and tab.
				switch (str.GetAt(i))
				{
				case _T('<'):
					nState = STATE_WAIT_TEXT_TAG;
					bCloseTag = FALSE;
					strTag = _T("");
					break;
				case _T('\n'):
					nCmd = CMD_NEW_LINE;
					nParam = 1;
					break;
				case _T('\t'):
					nCmd = CMD_TABULATION;
					nParam = 1;
					break;
				case _T('\r'):
					break;
				default: 
					strText += str.GetAt(i);
					break;
				}
				break;
			case STATE_WAIT_TEXT_TAG:
				//We're load the tag's text
				switch (str.GetAt(i))
				{
				default:
					strTag += str.GetAt(i);
					break;
				case _T('/'):
					if (strTag.IsEmpty())
						bCloseTag = TRUE; //Found the char's cancel tag
					break;
				case _T('<'):
					if (strTag.IsEmpty())
					{
						nState = STATE_WAIT_BEGIN_TAG;
						strText += str.GetAt(i);
					}
					else strTag += str.GetAt(i);
					break;
				case _T('='):
				case _T('>'):
					i --;
				case _T(' '):
					//Analyses tags
					if (!strTag.CompareNoCase(_T("b")))
					{
						//Bold text
						nCmd = CMD_BOLD;
						nState = STATE_WAIT_END_TAG;
					}
					else if (!strTag.CompareNoCase(_T("i")))
					{
						//Italic text
						nCmd = CMD_ITALIC;
						nState = STATE_WAIT_END_TAG;
					}
					else if (!strTag.CompareNoCase(_T("s")))
					{
						//Strikeout text
						nCmd = CMD_STRIKE;
						nState = STATE_WAIT_END_TAG;
					}
					else if (!strTag.CompareNoCase(_T("u")))
					{
						//Underline text
						nCmd = CMD_UNDERLINE;
						nState = STATE_WAIT_END_TAG;
					}
					else if (!strTag.CompareNoCase(_T("br")))
					{
						//New line
						nCmd = CMD_NEW_LINE;
						nParam = 1;
						nState = STATE_WAIT_BEGIN_NUMBER;
					}
					else if (!strTag.CompareNoCase(_T("t")))
					{
						//Tabulation
						nCmd = CMD_TABULATION;
						nParam = 1;
						nState = STATE_WAIT_BEGIN_NUMBER;
					}
					else if (!strTag.CompareNoCase(_T("ct")))
					{
						//Color of the text
						nCmd = CMD_COLOR_TEXT;
						nParam = (UINT)m_crColor[TOOLTIP_COLOR_FG];
						nState = STATE_WAIT_BEGIN_NUMBER;
					}
					else if (!strTag.CompareNoCase(_T("cti")))
					{
						//Indexed color of the text
						nCmd = CMD_COLOR_TEXT_INDEX;
						nParam = TOOLTIP_COLOR_FG;
						nState = STATE_WAIT_BEGIN_NUMBER;
					}
					else if (!strTag.CompareNoCase(_T("cb")))
					{
						//Color of the background
						nCmd = CMD_COLOR_BK;
						nParam = (UINT)m_crColor[TOOLTIP_COLOR_BK_BEGIN];
						nState = STATE_WAIT_BEGIN_NUMBER;
					}
					else if (!strTag.CompareNoCase(_T("cbi")))
					{
						//Indexed color of the background
						nCmd = CMD_COLOR_BK_INDEX;
						nParam = TOOLTIP_COLOR_BK_BEGIN;
						nState = STATE_WAIT_BEGIN_NUMBER;
					}
					else if (!strTag.CompareNoCase(_T("al")) || !strTag.CompareNoCase(_T("al_l")))
					{
						//left align
						nAlign = ALIGN_LEFT;
						nState = STATE_WAIT_END_TAG;
					}
					else if (!strTag.CompareNoCase(_T("al_c")))
					{
						//center align
						if (!bCalculate)
							nAlign = bCloseTag ? ALIGN_LEFT : ALIGN_CENTER;
						nState = STATE_WAIT_END_TAG;
					}
					else if (!strTag.CompareNoCase(_T("al_r")))
					{
						//right align
						if (!bCalculate)
							nAlign = bCloseTag ? ALIGN_LEFT : ALIGN_RIGHT;
						nState = STATE_WAIT_END_TAG;
					}
					else if (!strTag.CompareNoCase(_T("hr")))
					{
						//horizontal line
						nCmd = CMD_HORZ_LINE_PERCENT;
						nParam = 100;
						nState = STATE_WAIT_BEGIN_NUMBER;
					}
					else if (!strTag.CompareNoCase(_T("img")))
					{
						//draws bitmap
						nCmd = CMD_DRAW_BITMAP;
						nState = STATE_WAIT_BEGIN_RESOURSE;
						strRes = _T("");
					}
					else if (!strTag.CompareNoCase(_T("ilst")))
					{
						//draws bitmap
						nCmd = CMD_DRAW_IMAGE_LIST;
						nParam = 0;
						nState = STATE_WAIT_BEGIN_NUMBER;
					}
					else nState = STATE_WAIT_END_TAG; //Unknown tag
					break;
				}
				break;
			case STATE_WAIT_END_TAG:
				//We're wait end of the tag
				if (str.GetAt(i) == _T('>'))
					nState = STATE_WAIT_BEGIN_TAG;
				break;
			case STATE_WAIT_BEGIN_NUMBER:
				//We're wait begin of the number
				if (str.GetAt(i) == _T('='))
				{
					strTag = _T("");
					nState = STATE_WAIT_TEXT_NUMBER;
				}
				else if (str.GetAt(i) == _T('>'))
					nState = STATE_WAIT_BEGIN_TAG; //Not number
				break;
			case STATE_WAIT_TEXT_NUMBER:
				//We're wait string of the number
				switch (str.GetAt(i))
				{
				case _T('>'):
					i --;
				case _T(' '):
					if (nCmd == CMD_HORZ_LINE_PERCENT)
						nCmd = CMD_HORZ_LINE;
				case _T('%'):
					//Gets the real number from the string
					if (!strTag.IsEmpty())
						nParam = _tcstoul(strTag, 0, 0);
					nState = STATE_WAIT_END_TAG;
					break;
				default:
					strTag += str.GetAt(i);
					break;
				}
				break;
			case STATE_WAIT_BEGIN_RESOURSE:
				switch (str.GetAt(i))
				{
				case _T('>'):
					i--;
				case _T('='):
					nState = STATE_WAIT_END_RESOURCE;
					break;
				}
				break;
			case STATE_WAIT_END_RESOURCE:
				switch (str.GetAt(i))
				{
				case _T('>'):
					i--;
					nState = STATE_WAIT_END_TAG;
					break;
				default:
					strRes += str.GetAt(i);
					break;
				}
				break;
			}
		}
		else
		{
			//Immitates new line on the end of the string
			nState = STATE_WAIT_BEGIN_TAG;
			nCmd = CMD_NEW_LINE;
			nParam = 1;
		}

		if ((nState == STATE_WAIT_BEGIN_TAG) && (nCmd != CMD_NONE))
		{
			//New Command with full parameters
			if (!strText.IsEmpty())
			{
				//If text to output is exist
				if (bFirstOutput)
				{
					switch (nAlign)
					{
					case ALIGN_CENTER:
						ptCur.x = pt.x + ((rect->right - rect->left) - m_nLengthLines.GetAt(nLine)) / 2;
						break;
					case ALIGN_RIGHT:
						ptCur.x = pt.x + (rect->right - rect->left) - m_nLengthLines.GetAt(nLine);
						break;
					}
				}
				::GetTextExtentPoint32(hDC, strText, (int)strText.GetLength(), &szLine);
				if (bCalculate)
					nLineHeight = max(nLineHeight, szLine.cy);
				else
					::TextOut(hDC,ptCur.x, ptCur.y + m_nHeightLines.GetAt(nLine) - nHeight, strText,(int)strText.GetLength());
				ptCur.x += szLine.cx;
				strText = _T("");
				bFirstOutput = FALSE;
			}
			
			//Executes command
			switch (nCmd)
			{
			case CMD_BOLD:
				//Bold text
				::SelectObject(hDC,oldFont);
				::DeleteObject(font);
				lf.lfWeight = m_logFont.lfWeight;
				if (!bCloseTag)
				{
					lf.lfWeight *= 2;
					if (lf.lfWeight > FW_BLACK)
						lf.lfWeight = FW_BLACK;
				}
				font = ::CreateFontIndirect(&lf);
				::SelectObject(hDC,font);
				break;
			case CMD_ITALIC:
				//Italic text
				::SelectObject(hDC,oldFont);
				::DeleteObject(font);
				lf.lfItalic = bCloseTag ? FALSE : TRUE;
				font = ::CreateFontIndirect(&lf);
				::SelectObject(hDC,font);
				break;
			case CMD_STRIKE:
				//Strikeout text
				::SelectObject(hDC,oldFont);
				::DeleteObject(font);
				lf.lfStrikeOut = bCloseTag ? FALSE : TRUE;
				font = ::CreateFontIndirect(&lf);
				::SelectObject(hDC,font);
				break;
			case CMD_UNDERLINE:
				//Underline text
				::SelectObject(hDC,oldFont);
				::DeleteObject(font);
				lf.lfUnderline = bCloseTag ? FALSE : TRUE;
				font = ::CreateFontIndirect(&lf);
				::SelectObject(hDC,font);
				break;
			case CMD_COLOR_TEXT:
				//Color of the text
				::SetTextColor(hDC,(COLORREF)nParam);
				break;
			case CMD_COLOR_TEXT_INDEX:
				//Indexed color of the text
				if (nParam < TOOLTIP_MAX_COLORS)
					SetTextColor(hDC,m_crColor[nParam]);
				break;
			case CMD_COLOR_BK:
				//Color of the background
				::SetBkColor(hDC,(COLORREF)nParam);
				::SetBkMode(hDC,bCloseTag ? TRANSPARENT : OPAQUE);
				break;
			case CMD_COLOR_BK_INDEX:
				//Indexed color of the background
				if (nParam < TOOLTIP_MAX_COLORS)
				{
					::SetBkColor(hDC,m_crColor[nParam]);
					::SetBkMode(hDC,bCloseTag ? TRANSPARENT : OPAQUE);
				}
				break;
			case CMD_HORZ_LINE_PERCENT:
				//Horizontal line with percent length
				if (bCalculate)
				{
					percent.SetAt(nLine, percent.GetAt(nLine) + nParam);
					nParam = 0;
				}
				else
				{
					nParam = ::MulDiv(rect->right-rect->left, nParam, 100);
				}
			case CMD_HORZ_LINE:
				//Horizontal line with absolute length
				//If text to output is exist
				if (bFirstOutput)
				{
					switch (nAlign)
					{
					case ALIGN_CENTER:
						ptCur.x = pt.x + ((rect->right - rect->left) - m_nLengthLines.GetAt(nLine)) / 2;
						break;
					case ALIGN_RIGHT:
						ptCur.x = pt.x + (rect->right - rect->left) - m_nLengthLines.GetAt(nLine);
						break;
					}
				}
				if (!bCalculate)
				{
					DrawHorzLine(hDC, ptCur.x, ptCur.x + nParam, ptCur.y + m_nHeightLines.GetAt(nLine) / 2);
				}
				ptCur.x += nParam;
				bFirstOutput = FALSE;
				break;
			case CMD_DRAW_BITMAP:
				if (!strRes.IsEmpty())
				{
					if (bFirstOutput)
					{
						switch (nAlign)
						{
						case ALIGN_CENTER:
							ptCur.x = pt.x + ((rect->right - rect->left) - m_nLengthLines.GetAt(nLine)) / 2;
							break;
						case ALIGN_RIGHT:
							ptCur.x = pt.x + (rect->right - rect->left) - m_nLengthLines.GetAt(nLine);
							break;
						}
					}
					if (bCalculate)
					{
						szLine = DrawResource(strRes, hDC, ptCur, 0, bCalculate);
						nLineHeight = max (nLineHeight, szLine.cy);
					}
					else
						szLine = DrawResource(strRes, hDC, ptCur, m_nHeightLines.GetAt(nLine), bCalculate);

					ptCur.x += szLine.cx;
					bFirstOutput = FALSE;
				}
				break;
			case CMD_DRAW_IMAGE_LIST:
				if (m_imgTooltip != NULL)
				{
					if (bFirstOutput)
					{
						switch (nAlign)
						{
						case ALIGN_CENTER:
							ptCur.x = pt.x + ((rect->right - rect->left) - m_nLengthLines.GetAt(nLine)) / 2;
							break;
						case ALIGN_RIGHT:
							ptCur.x = pt.x + (rect->right - rect->left) - m_nLengthLines.GetAt(nLine);
							break;
						}
					}
					nHeightBitmap = szLine.cy;
					if (bCalculate)
					{
						szLine = DrawIconFromImageList(hDC, ptCur, m_szImage, m_imgTooltip, nParam, bCalculate);
						nLineHeight = max (nLineHeight, szLine.cy);
					}
					else
					{
						szLine = DrawIconFromImageList(hDC, ptCur, m_szImage, m_imgTooltip, nParam, bCalculate);
					}
					// If in one line a few bitmap with different height, then store max height
					ptCur.x += m_szImage.cx;
					bFirstOutput = FALSE;
				}
				break;
			case CMD_NEW_LINE:
				//New line
				if (!nParam)
					nParam = 1;
				if (bCalculate)
				{
					sz.cx = max(sz.cx, ptCur.x - pt.x);
					m_nLengthLines.Add(ptCur.x - pt.x); //Adds the real length of the lines
					m_nHeightLines.Add(nLineHeight); //Adds the real height of the lines
				}
				ptCur.y += m_nHeightLines.GetAt(nLine) * nParam;
				nLine ++;
				percent.Add(0);
				bFirstOutput = TRUE;
				ptCur.x = pt.x;
				nLineHeight = nHeight;
//				szLine.cy = nHeight;
				break;
			case CMD_TABULATION:
				//Tabulation
				if (!nParam)
					nParam = 1;
				nTemp = (ptCur.x - pt.x) % (nWidth * 4);
				if (nTemp)
				{
					//aligns with tab
					ptCur.x += (nWidth * 4) - nTemp;
					nParam --;
				}
				ptCur.x += (nParam * nWidth * 4);
				break;
			}
			//Resets the last command
			nCmd = CMD_NONE;
			bCloseTag = FALSE;
		}
	}

	//Gets real height of the tooltip
	sz.cy = ptCur.y - pt.y;

	::SelectObject(hDC,oldFont);
	::DeleteObject(font);

	//Adds the percent's length to the line's length
	for (i = 0; i < percent.GetSize(); i++)
	{
		if (percent.GetAt(i))
			m_nLengthLines.SetAt(i, m_nLengthLines.GetAt(i) + ::MulDiv(percent.GetAt(i), sz.cx, 100));
	}

	return sz;
}

SIZE CustomToolTip::DrawResource(TupString sName, HDC hDC, POINT pt, int nMaxHeight, BOOL bCalculate)
{
	SIZE sz;
	sz.cx = sz.cy = 0;
	
	int nIndex = FindIdOfResource(sName);
	if (nIndex < 0)
		return sz;

	const ICustomToolTip::ToolTipNameRes &nr = m_arrNameRes[nIndex];

	if (nr.m_nID == 0)
		return sz;

	switch (nr.m_nTypeRes)
	{
	case ICustomToolTip::TYPE_RES_ICON:
		sz = DrawIcon(hDC, pt, nMaxHeight, nr.m_nID, bCalculate);
		break;
	case ICustomToolTip::TYPE_RES_BITMAP:
		sz = DrawBitmap(hDC, pt, nMaxHeight, nr.m_nID, FALSE, nr.m_crMask, bCalculate);
		break;
	case ICustomToolTip::TYPE_RES_MASK_BITMAP:
		sz = DrawBitmap(hDC, pt, nMaxHeight, nr.m_nID, TRUE, nr.m_crMask, bCalculate);
		break;
	}

	return sz;
}

SIZE CustomToolTip::DrawBitmap(HDC hDC, POINT pt, int nMaxHeight, UINT nID, BOOL bUseMask, COLORREF crMask, BOOL bCalculate)
{
	SIZE sz;
	sz.cx = sz.cy = 0;

	HBITMAP	hBitmap = GetBitmapFromResources(nID);
	
	int		nRetValue;
	BITMAP	csBitmapSize;
	
	if (hBitmap == NULL)
		return sz;
	
	// Get bitmap size
	nRetValue = ::GetObject(hBitmap, sizeof(csBitmapSize), &csBitmapSize);
	if (nRetValue == 0)
		return sz;
	
	sz.cx = (DWORD)csBitmapSize.bmWidth;
	sz.cy = (DWORD)csBitmapSize.bmHeight;

	if (bCalculate)
		return sz;
	
	pt.y += (nMaxHeight - sz.cy);

	if (bUseMask)
	{
		HIMAGELIST img = ::ImageList_Create(sz.cx,sz.cy, ILC_COLOR32 | ILC_MASK, 1, 1);
		::ImageList_AddMasked(img,hBitmap, crMask);
		DrawIconFromImageList(hDC, pt, sz, img, 0, FALSE);
		::ImageList_Destroy(img);
	}
	else
	{
		HDC memDC;
		memDC = ::CreateCompatibleDC(hDC);
		HBITMAP hOldBitmap = (HBITMAP)::SelectObject(memDC, hBitmap);
		::BitBlt(hDC,pt.x, pt.y, sz.cx, sz.cy, memDC, 0, 0, SRCCOPY);
		::SelectObject(memDC, hOldBitmap);
		::DeleteDC(memDC);
	}
	return sz;
}

SIZE CustomToolTip::DrawIcon(HDC hDC, POINT pt, int nMaxHeight, UINT nID, BOOL bCalculate)
{
	SIZE sz;
	sz.cx = sz.cy = 0;

	HICON hIcon = GetIconFromResources(nID);
	if (hIcon != NULL)
	{
		sz = GetSizeIcon(hIcon);
		if (!bCalculate)
		{
			pt.y += (nMaxHeight - sz.cy);
			::DrawState(hDC, NULL, NULL, (LPARAM)hIcon, 0, pt.x, pt.y, sz.cx, sz.cy, DSS_NORMAL|DST_ICON);
		}
	}
	return sz;
}

SIZE CustomToolTip::DrawIconFromImageList(HDC hDC, POINT pt, SIZE sz, HIMAGELIST img, int nIndex /* = 0 */, BOOL bCalculate /* = TRUE */)
{
	SIZE nullSize;
	nullSize.cx = nullSize.cy = 0;

	if (m_imgTooltip == NULL)
	{
		return nullSize;
	}

	int nCount = ImageList_GetImageCount(img);
	if (nIndex >= nCount)
		return nullSize;

	if (bCalculate)
		return sz;
	
	HICON hIcon = ImageList_ExtractIcon(NULL,img,nIndex);
	::DrawState(hDC, NULL, NULL, (LPARAM)hIcon, 0, pt.x, pt.y, sz.cx, sz.cy, DSS_NORMAL|DST_ICON);
	return sz;
}

void CustomToolTip::DrawHorzLine(HDC hDC, int xStart, int xEnd, int y) const
{
	HPEN pen = ::CreatePen(PS_SOLID, 1, ::GetTextColor(hDC));
	HPEN penOld = (HPEN) SelectObject(hDC,pen);

	POINT point;
	::MoveToEx(hDC, xStart, y, &point);
	::LineTo(hDC, xEnd, y);
	::SelectObject(hDC,penOld);
	DeleteObject(pen);
}

void CustomToolTip::FillGradient (HDC hDC, LPRECT rect, 
								COLORREF colorStart, COLORREF colorFinish, 
								BOOL bHorz/* = TRUE*/)
{
    // this will make 2^6 = 64 fountain steps
	int nShift = 6;
	int nSteps = 1 << nShift;

	for (int i = 0; i < nSteps; i++)
	{
		// do a little alpha blending
		BYTE bR = (BYTE) ((GetRValue(colorStart) * (nSteps - i) +
			GetRValue(colorFinish) * i) >> nShift);
		BYTE bG = (BYTE) ((GetGValue(colorStart) * (nSteps - i) +
			GetGValue(colorFinish) * i) >> nShift);
		BYTE bB = (BYTE) ((GetBValue(colorStart) * (nSteps - i) +
			GetBValue(colorFinish) * i) >> nShift);

		HBRUSH br = ::CreateSolidBrush(RGB(bR, bG, bB));

		// then paint with the resulting color
		RECT r2 = *rect;
		if (!bHorz)
		{
			r2.top = rect->top + ((i * (rect->bottom - rect->top)) >> nShift);
			r2.bottom = rect->top + (((i + 1) * (rect->bottom - rect->top)) >> nShift);
			if ((r2.bottom - r2.top) > 0)
			{
				::FillRect(hDC,&r2,br);
			}
		}
		else
		{
			r2.left = rect->left + ((i * (rect->right - rect->left)) >> nShift);
			r2.right = rect->left + (((i + 1) * (rect->right - rect->left)) >> nShift);
			if ((r2.right-r2.left) > 0)
			{
				::FillRect(hDC,&r2, br);
			}
		}
		::DeleteObject(br);
	}
}

#ifdef TOOLTIP_USE_SHADE
void CustomToolTip::SetShade(RECT rect, UINT shadeID /* = 0 */, BYTE granularity /* = 8 */, 
						  BYTE coloring /* = 0 */, COLORREF color /* = 0 */)
{
	long	sXSize,sYSize,bytes,j,i,k,h;
	BYTE	*iDst ,*posDst;
	
	sYSize= (rect.bottom - rect.top); //rect.bottom-rect.top;
	sXSize= (rect.right - rect.left); //rect.right-rect.left ;

	m_dh.Create(max(1,sXSize /*-2*m_FocusRectMargin-1*/ ),1,8);	//create the horizontal focus bitmap
	m_dv.Create(1,max(1,sYSize /*-2*m_FocusRectMargin*/),8);	//create the vertical focus bitmap

	m_dNormal.Create(sXSize,sYSize,8);					//create the default bitmap

	COLORREF hicr = m_toolInfo.m_crBegin; //GetSysColor(COLOR_BTNHIGHLIGHT);		//get the button base colors
	COLORREF midcr = m_toolInfo.m_crMid;  //GetSysColor(COLOR_BTNFACE);
	COLORREF locr = m_toolInfo.m_crEnd;   //GetSysColor(COLOR_BTNSHADOW);
	long r,g,b;											//build the shaded palette
	for(i=0;i<129;i++)
	{
		r=((128-i)*GetRValue(locr)+i*GetRValue(midcr))/128;
		g=((128-i)*GetGValue(locr)+i*GetGValue(midcr))/128;
		b=((128-i)*GetBValue(locr)+i*GetBValue(midcr))/128;
		m_dNormal.SetPaletteIndex((BYTE)i,(BYTE)r,(BYTE)g,(BYTE)b);
		m_dh.SetPaletteIndex((BYTE)i,(BYTE)r,(BYTE)g,(BYTE)b);
		m_dv.SetPaletteIndex((BYTE)i,(BYTE)r,(BYTE)g,(BYTE)b);
	}
	for(i=1;i<129;i++)
	{
		r=((128-i)*GetRValue(midcr)+i*GetRValue(hicr))/128;
		g=((128-i)*GetGValue(midcr)+i*GetGValue(hicr))/128;
		b=((128-i)*GetBValue(midcr)+i*GetBValue(hicr))/128;
		m_dNormal.SetPaletteIndex((BYTE)(i+127),(BYTE)r,(BYTE)g,(BYTE)b);
		m_dh.SetPaletteIndex((BYTE)(i+127),(BYTE)r,(BYTE)g,(BYTE)b);
		m_dv.SetPaletteIndex((BYTE)(i+127),(BYTE)r,(BYTE)g,(BYTE)b);
	}

	m_dNormal.BlendPalette(color,coloring);	//color the palette

	iDst=m_dh.GetBits();		//build the horiz. dotted focus bitmap
	j=(long)m_dh.GetWidth();
	for(i=0;i<j;i++)
	{
//		iDst[i]=64+127*(i%2);	//soft
		iDst[i]=(BYTE)(255*(i%2));		//hard
	}

	iDst=m_dv.GetBits();		//build the vert. dotted focus bitmap
	j=(long)m_dv.GetHeight();
	for(i=0;i<j;i++)
	{
//		*iDst=64+127*(i%2);		//soft
		*iDst=(BYTE)(255*(i%2));		//hard
		iDst+=4;
	}

	bytes = m_dNormal.GetLineWidth();
	iDst = m_dNormal.GetBits();
	posDst =iDst;
	long a,x,y,d,xs,idxmax,idxmin;

	int grainx2=RAND_MAX/(max(1,2*granularity));
	idxmax=255-granularity;
	idxmin=granularity;

	switch (shadeID)
	{
//----------------------------------------------------
	case ICustomToolTip::TOOLTIP_EFFECT_METAL:
		m_dNormal.Clear();
		// create the strokes
		k=40;	//stroke granularity
		for(a=0;a<200;a++)
		{
			x=rand()/(RAND_MAX/sXSize); //stroke postion
			y=rand()/(RAND_MAX/sYSize);	//stroke position
			xs=rand()/(RAND_MAX/min(sXSize,sYSize))/2; //stroke lenght
			d=rand()/(RAND_MAX/k);	//stroke color
			for(i=0;i<xs;i++)
			{
				if (((x-i)>0)&&((y+i)<sYSize))
					m_dNormal.SetPixelIndex(x-i,y+i,(BYTE)d);
				if (((x+i)<sXSize)&&((y-i)>0))
					m_dNormal.SetPixelIndex(sXSize-x+i,y-i,(BYTE)d);
			}
		}
		//blend strokes with SHS_DIAGONAL
		posDst =iDst;
		a=(idxmax-idxmin-k)/2;
		for(i = 0; i < sYSize; i++) 
		{
			for(j = 0; j < sXSize; j++) 
			{
				d=posDst[j]+((a*i)/sYSize+(a*(sXSize-j))/sXSize);
				posDst[j]=(BYTE)d;
				posDst[j]+=rand()/grainx2;
			}
			posDst+=bytes;
		}

		break;
//----------------------------------------------------
	case ICustomToolTip::TOOLTIP_EFFECT_HARDBUMP:	// 
		//set horizontal bump
		for(i = 0; i < sYSize; i++) 
		{
			k=(255*i/sYSize)-127;
			k=(k*(k*k)/128)/128;
			k=(k*(128-granularity*2))/128+128;
			for(j = 0; j < sXSize; j++) 
			{
				posDst[j]=(BYTE)k;
				posDst[j]+=rand()/grainx2-granularity;
			}
			posDst+=bytes;
		}
		//set vertical bump
		d=min(16,sXSize/6);	//max edge=16
		a=sYSize*sYSize/4;
		posDst =iDst;
		for(i = 0; i < sYSize; i++) 
		{
			y=i-sYSize/2;
			for(j = 0; j < sXSize; j++) 
			{
				x=j-sXSize/2;
				xs=sXSize/2-d+(y*y*d)/a;
				if (x>xs) posDst[j]=(BYTE)(idxmin+(BYTE)(((sXSize-j)*128)/d));
				if ((x+xs)<0) posDst[j]=(BYTE)(idxmax-(BYTE)((j*128)/d));
				posDst[j]+=rand()/grainx2-granularity;
			}
			posDst+=bytes;
		}
		break;
//----------------------------------------------------
	case ICustomToolTip::TOOLTIP_EFFECT_SOFTBUMP: //
		for(i = 0; i < sYSize; i++) 
		{
			h=(255*i/sYSize)-127;
			for(j = 0; j < sXSize; j++) 
			{
				k=(255*(sXSize-j)/sXSize)-127;
				k=(h*(h*h)/128)/128+(k*(k*k)/128)/128;
				k=k*(128-granularity)/128+128;
				if (k<idxmin) k=idxmin;
				if (k>idxmax) k=idxmax;
				posDst[j]=(BYTE)k;
				posDst[j]+=rand()/grainx2-granularity;
			}
			posDst+=bytes;
		}
		break;
//----------------------------------------------------
	case ICustomToolTip::TOOLTIP_EFFECT_VBUMP: // 
		for(j = 0; j < sXSize; j++) 
		{
			k=(255*(sXSize-j)/sXSize)-127;
			k=(k*(k*k)/128)/128;
			k=(k*(128-granularity))/128+128;
			for(i = 0; i < sYSize; i++) 
			{
				posDst[j+i*bytes]=(BYTE)k;
				posDst[j+i*bytes]+=rand()/grainx2-granularity;
			}
		}
		break;
//----------------------------------------------------
	case ICustomToolTip::TOOLTIP_EFFECT_HBUMP: //
		for(i = 0; i < sYSize; i++) 
		{
			k=(255*i/sYSize)-127;
			k=(k*(k*k)/128)/128;
			k=(k*(128-granularity))/128+128;
			for(j = 0; j < sXSize; j++) 
			{
				posDst[j]=(BYTE)k;
				posDst[j]+=rand()/grainx2-granularity;
			}
			posDst+=bytes;
		}
		break;
//----------------------------------------------------
	case ICustomToolTip::TOOLTIP_EFFECT_DIAGSHADE:	//
		a=(idxmax-idxmin)/2;
		for(i = 0; i < sYSize; i++) 
		{
			for(j = 0; j < sXSize; j++) 
			{
				posDst[j]=(BYTE)(idxmin+a*i/sYSize+a*(sXSize-j)/sXSize);
				posDst[j]+=rand()/grainx2-granularity;
			}
			posDst+=bytes;
		}
		break;
//----------------------------------------------------
	case ICustomToolTip::TOOLTIP_EFFECT_HSHADE:	//
		a=idxmax-idxmin;
		for(i = 0; i < sYSize; i++) 
		{
			k=a*i/sYSize+idxmin;
			for(j = 0; j < sXSize; j++) 
			{
				posDst[j]=(BYTE)k;
				posDst[j]+=rand()/grainx2-granularity;
			}
			posDst+=bytes;
		}
		break;
//----------------------------------------------------
	case ICustomToolTip::TOOLTIP_EFFECT_VSHADE:	//:
		a=idxmax-idxmin;
		for(j = 0; j < sXSize; j++) 
		{
			k=a*(sXSize-j)/sXSize+idxmin;
			for(i = 0; i < sYSize; i++) 
			{
				posDst[j+i*bytes]=(BYTE)k;
				posDst[j+i*bytes]+=rand()/grainx2-granularity;
			}
		}
		break;
//----------------------------------------------------
	case ICustomToolTip::TOOLTIP_EFFECT_NOISE:
		for(i = 0; i < sYSize; i++) 
		{
			for(j = 0; j < sXSize; j++) 
			{
				posDst[j]=128+rand()/grainx2-granularity;
			}
			posDst+=bytes;
		}
	}
//----------------------------------------------------
}
#endif

HICON CustomToolTip::GetIconFromResources(UINT nID) const
{
	// Find correct resource handle
	HINSTANCE hInstResource = CustomControls::GetHInstance();
	// Set icon when the mouse is IN the button
	HICON hIcon = (HICON)::LoadImage(hInstResource, MAKEINTRESOURCE(nID), IMAGE_ICON, 0, 0, 0);
	return hIcon;
}

HBITMAP CustomToolTip::GetBitmapFromResources(UINT nID) const
{
	// Find correct resource handle
	HINSTANCE hInstResource = CustomControls::GetHInstance();
	// Load bitmap
	HBITMAP hBitmap = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(nID), IMAGE_BITMAP, 0, 0, 0);

	return hBitmap;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::SetStyles  (public member function)
//    Sets the new styles of the control
//
//  Parameters :
//		nStyle		[in] - new style
//
//  Returns :
//		Old styles
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::SetStyles(DWORD m_nStyles, int nIndexTool /* = ICustomToolTip::TOOLTIP_TOOL_NOEXIST */)
{
//	TRACE(_T("CustomToolTip::SetStyles()\n"));

	ModifyStyles(m_nStyles, -1, nIndexTool);
}  // End of SetStyles

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::ModifyStyles  (public member function)
//    Modify the styles of the control
//
//  Parameters :
//		nAddStyle	 [in] - The styles to add
//		nRemoveStyle [in] - The styles to remove
//
//  Returns :
//		Old styles
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::ModifyStyles(DWORD nAddStyles, DWORD nRemoveStyles, int nIndexTool /* = ICustomToolTip::TOOLTIP_TOOL_NOEXIST */)
{
	if (!IsExistTool(nIndexTool))
	{
		m_nStyles &= ~nRemoveStyles;
		m_nStyles |= nAddStyles;
	}
	else
	{
		ICustomToolTip::ToolTipInfo ti;
		GetTool(nIndexTool, ti);
		if (!(ti.m_nMask & ICustomToolTip::TOOLTIP_MASK_STYLES))
			ti.m_nStyles = m_nStyles;
		ti.m_nStyles &= ~nRemoveStyles;
		ti.m_nStyles |= nAddStyles;
		ti.m_nMask |= ICustomToolTip::TOOLTIP_MASK_STYLES;
		SetAtTool(nIndexTool, ti);
	}
}  // End of ModifyStyles

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::GetStyles (public member function)
//    Gets the current styles of the control
//
//  Parameters :
//
//  Returns :
//		Current styles
//
/////////////////////////////////////////////////////////////////////////////
DWORD CustomToolTip::GetStyles(int nIndexTool /* = ICustomToolTip::TOOLTIP_TOOL_NOEXIST */)
{
//	TRACE(_T("CustomToolTip::GetStyles()\n"));

	if (IsExistTool(nIndexTool))
	{
		ICustomToolTip::ToolTipInfo ti;
		GetTool(nIndexTool, ti);
		if (ti.m_nMask & ICustomToolTip::TOOLTIP_MASK_STYLES)
			return ti.m_nStyles;
	}
	return m_nStyles;
}  // End of GetStyles

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::SetDefaultStyles  (public member function)
//    Sets the new styles of the control
//
//  Parameters :
//
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::SetDefaultStyles(int nIndexTool /* = ICustomToolTip::TOOLTIP_TOOL_NOEXIST */)
{
//	TRACE(_T("CustomToolTip::SetDefaultStyles()\n"));

	SetStyles(ICustomToolTip::TOOLTIP_BALLOON | ICustomToolTip::TOOLTIP_ICON_VCENTER_ALIGN, nIndexTool);
}  // End of SetDefaultStyles

void CustomToolTip::Activate(BOOL bActive)
{
	if (bActive)
	{
		m_bActive = TRUE;
//		TRACE("Active\n");
	}
	else
	{
		if (IsWindow(m_hWnd))
		{
			Pop();
			m_nIndexCurrentWnd = ICustomToolTip::TOOLTIP_TOOL_NOEXIST;
			m_nIndexDisplayWnd = ICustomToolTip::TOOLTIP_TOOL_NOEXIST;
//			TRACE("InActive\n");
		}
		m_bActive = FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::SetColor (public member function)
//    Set the color
//
//  Parameters :
//		nIndex  [in] - index of the color
//		crColor [in] - new color
//
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::SetColor(int nIndex, COLORREF crColor)
{
//	TRACE (_T("CustomToolTip::SetColor(nIndex = %d)\n"), nIndex);
	
	if (nIndex >= TOOLTIP_MAX_COLORS)
		return;

	m_crColor [nIndex] = crColor;
}  // End of SetColor

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::GetColor (public member function)
//    Set the color
//
//  Parameters :
//		nIndex  [in] - index of the color
//
//  Returns :
//		Current color
//
/////////////////////////////////////////////////////////////////////////////
COLORREF CustomToolTip::GetColor(int nIndex)
{
//	TRACE (_T("CustomToolTip::GetColor(nIndex = %d)\n"), nIndex);

	if (nIndex >= TOOLTIP_MAX_COLORS)
		nIndex = TOOLTIP_COLOR_FG;

	return m_crColor [nIndex];
}  // End of GetColor

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::SetDefaultColors (public member function)
//    Set the color as default
//
//  Parameters :
//		None
//
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::SetDefaultColors()
{
//	TRACE (_T("CustomToolTip::SetDefaultColors\n"));
	
	SetColor(TOOLTIP_COLOR_0, RGB (0, 0, 0));
	SetColor(TOOLTIP_COLOR_1, RGB (0, 0, 128));
	SetColor(TOOLTIP_COLOR_2, RGB (0, 128, 0));
	SetColor(TOOLTIP_COLOR_3, RGB (0, 128, 128));
	SetColor(TOOLTIP_COLOR_4, RGB (128, 0, 0));
	SetColor(TOOLTIP_COLOR_5, RGB (128, 0, 128));
	SetColor(TOOLTIP_COLOR_6, RGB (128, 128, 0));
	SetColor(TOOLTIP_COLOR_7, RGB (128, 128, 128));
	SetColor(TOOLTIP_COLOR_8, RGB (0, 0, 255));
	SetColor(TOOLTIP_COLOR_9, RGB (0, 255, 0));
	SetColor(TOOLTIP_COLOR_10, RGB (0, 255, 255));
	SetColor(TOOLTIP_COLOR_11, RGB (255, 0, 0));
	SetColor(TOOLTIP_COLOR_12, RGB (255, 0, 255));
	SetColor(TOOLTIP_COLOR_13, RGB (255, 255, 0));
	SetColor(TOOLTIP_COLOR_14, RGB (192, 192, 192));
	SetColor(TOOLTIP_COLOR_15, RGB (255, 255, 255));
	SetColor(TOOLTIP_COLOR_FG, ::GetSysColor(COLOR_INFOTEXT));
	SetColor(TOOLTIP_COLOR_BK_BEGIN, ::GetSysColor(COLOR_INFOBK));
	SetColor(TOOLTIP_COLOR_BK_MID, ::GetSysColor(COLOR_INFOBK));
	SetColor(TOOLTIP_COLOR_BK_END, ::GetSysColor(COLOR_INFOBK));
	SetColor(TOOLTIP_COLOR_LINK, RGB(0, 0, 238));
	SetColor(TOOLTIP_COLOR_VISITED, RGB(85, 26, 139));
	SetColor(TOOLTIP_COLOR_HOVER, RGB(255, 0, 0));
	SetColor(TOOLTIP_COLOR_SHADOW, ::GetSysColor(COLOR_3DSHADOW));
	SetColor(TOOLTIP_COLOR_BORDER, ::GetSysColor(COLOR_INFOTEXT));
}  // End of SetDefaultColors

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::SetGradientColors (public member function)
//    Set the gradient colors
//
//  Parameters :
//		crStart [in] - start color
//		crEnd [in] - end color
//
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::SetGradientColors(COLORREF crBegin, COLORREF crMid, COLORREF crEnd, int nIndexTool /* = ICustomToolTip::TOOLTIP_TOOL_NOEXIST */)
{
//	TRACE (_T("CustomToolTip::SetGradientColors\n"));
	if (!IsExistTool(nIndexTool))
	{
		SetColor(TOOLTIP_COLOR_BK_BEGIN, crBegin);
		SetColor(TOOLTIP_COLOR_BK_MID, crMid);
		SetColor(TOOLTIP_COLOR_BK_END, crEnd);
	}
	else
	{
		ICustomToolTip::ToolTipInfo ti;
		GetTool(nIndexTool, ti);
		ti.m_crBegin = crBegin;
		ti.m_crMid = crMid;
		ti.m_crEnd = crEnd;
		ti.m_nMask |= ICustomToolTip::TOOLTIP_MASK_COLORS;
		SetAtTool(nIndexTool, ti);
	}
} // End of SetGradientColors

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::GetGradientColors (public member function)
//    Set the gradient colors
//
//  Parameters :
//		None
//
//  Returns :
//		crStart [out] - start color
//		crEnd [out] - end color
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::GetGradientColors(COLORREF & crBegin, COLORREF & crMid, COLORREF & crEnd, int nIndexTool /* = -1 */)
{
//	TRACE (_T("CustomToolTip::GetGradientColors\n"));
	if (IsExistTool(nIndexTool))
	{
		ICustomToolTip::ToolTipInfo ti;
		GetTool(nIndexTool, ti);
		if (ti.m_nMask & ICustomToolTip::TOOLTIP_MASK_COLORS)
		{
			crBegin = ti.m_crBegin;
			crMid = ti.m_crMid;
			crEnd = ti.m_crEnd;
			return;
		}
	}
	crBegin = GetColor(TOOLTIP_COLOR_BK_BEGIN);
	crMid = GetColor(TOOLTIP_COLOR_BK_MID);
	crEnd = GetColor(TOOLTIP_COLOR_BK_END);
} // End of GetGradientColors

void CustomToolTip::SetMaskTool(int nIndexTool, UINT nMask /* = 0 */)
{
	ModifyMaskTool(nIndexTool, nMask, -1);
}

void CustomToolTip::ModifyMaskTool(int nIndexTool, UINT nAddMask, UINT nRemoveMask)
{
	if (!IsExistTool(nIndexTool))
		return;
	ICustomToolTip::ToolTipInfo ti;
	GetTool(nIndexTool, ti);
	ti.m_nMask &= ~nRemoveMask;
	ti.m_nMask |= nAddMask;
	SetAtTool(nIndexTool, ti);
}

UINT CustomToolTip::GetMaskTool(int nIndexTool)
{
	if (!IsExistTool(nIndexTool))
		return 0;
	ICustomToolTip::ToolTipInfo ti;
	GetTool(nIndexTool, ti);
	return ti.m_nMask;
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::SetEffectBk (public member function)
//    sets the background's effect
//
//  Parameters :
//		nEffect	[in]  - the background's effect 
//
//  Returns :
//		None 
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::SetEffectBk(UINT nEffect, BYTE nGranularity /* = 2 */, int nIndexTool /* = ICustomToolTip::TOOLTIP_TOOL_NOEXIST */)
{
//	TRACE (_T("CustomToolTip::SetEffectBk\n"));
	
	if (!IsExistTool(nIndexTool))
	{
		m_nEffect = nEffect;
		m_nGranularity = nGranularity;
	}
	else
	{
		ICustomToolTip::ToolTipInfo ti;
		GetTool(nIndexTool, ti);
		ti.m_nEffect = nEffect;
		ti.m_nGranularity = nGranularity;
		ti.m_nMask |= ICustomToolTip::TOOLTIP_MASK_EFFECT;
		SetAtTool(nIndexTool, ti);
	}
} // End SetEffectBk

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::GetEffectBk (public member function)
//    gets the background's effect
//
//  Parameters :
//		None 
//
//  Returns :
//		the background's effect 
//
/////////////////////////////////////////////////////////////////////////////
UINT CustomToolTip::GetEffectBk(int nIndexTool /* = ICustomToolTip::TOOLTIP_TOOL_NOEXIST */)
{
	BYTE nGranularity = 0;
	
	return GetEffectBk(nGranularity, nIndexTool);
} // End SetEffectBk

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::GetEffectBk (public member function)
//    gets the background's effect
//
//  Parameters :
//		nGranularity	[out] - effect's granularity
//
//  Returns :
//		the background's effect 
//
/////////////////////////////////////////////////////////////////////////////
UINT CustomToolTip::GetEffectBk(BYTE & nGranularity, int nIndexTool /* = ICustomToolTip::TOOLTIP_TOOL_NOEXIST */)
{
//	TRACE (_T("CustomToolTip::GetEffectBk\n"));
	
	if (IsExistTool(nIndexTool))
	{
		ICustomToolTip::ToolTipInfo ti;
		GetTool(nIndexTool, ti);
		if (ti.m_nMask & ICustomToolTip::TOOLTIP_MASK_EFFECT)
		{
			nGranularity = ti.m_nGranularity;
			return ti.m_nEffect;
		}
	}
	nGranularity = m_nGranularity;
	return m_nEffect;
} // End SetEffectBk

/////////////////////////////////////////////////////////////////////
// CustomToolTip::SetNotify
// This function sets or removes the notification messages from the control before display.
//
// Parameters:
//	bParentNotify [in] - If TRUE the control will be send the notification 
//				   to parent window
//				   Else the notification will not send
///////////////////////////////////////////////////////////////////////
void CustomToolTip::SetNotify(BOOL bParentNotify /* = TRUE */)
{
	HWND hWnd = NULL;

	if (bParentNotify)
		hWnd = m_hParentWnd;

	SetNotify(hWnd);
} //End SetNotify

/////////////////////////////////////////////////////////////////////
// CustomToolTip::SetNotify
// This function sets or removes the notification messages from the control before display.
//
// Parameters:
//	hWnd [in] -    If non-NULL the control will be send the notification 
//				   to specified window
//				   Else the notification will not send
///////////////////////////////////////////////////////////////////////
void CustomToolTip::SetNotify(HWND hWnd)
{
//	TRACE(_T("CustomToolTip::SetNotify\n"));

	m_hNotifyWnd = hWnd;
} //End SetNotify

/////////////////////////////////////////////////////////////////////
// CustomToolTip::GetNotify
// This function determines will be send the notification messages from 
// the control or not before display.
//
// Return value:
//	TRUE if the control will be notified the specified window
///////////////////////////////////////////////////////////////////////
BOOL CustomToolTip::GetNotify()
{
//	TRACE(_T("CustomToolTip::GetNotify\n"));

	return (m_hNotifyWnd != NULL);
}  //End GetNotify

/////////////////////////////////////////////////////////////////////
// CustomToolTip::SetDelayTime
// Call this function to set the delay time for a tool tip control. 
// The delay time is the length of time the cursor must remain on a tool 
// before the tool tip window appears. The default delay time is 500 milliseconds.
//
// Parameters:
//   dwDuration	[in]  - Flag that specifies which duration value will be retrieved. 
//						This parameter can be one of the following values:
//			
//			TTDT_AUTOPOP  - Retrieve the length of time the tool tip 
//							window remains visible if the pointer is 
//							stationary within a tool's bounding rectangle. 
//			TTDT_INITIAL  - Retrieve the length of time the pointer 
//							must remain stationary within a tool's bounding 
//							rectangle before the tool tip window appears. 
//			TTDT_RESHOW   - Retrieve the length of time it takes for 
//							subsequent tool tip windows to appear as the 
//							pointer moves from one tool to another.
//	 nTime [in] - The specified delay time, in milliseconds.
//
/////////////////////////////////////////////////////////////////////
void CustomToolTip::SetDelayTime(DWORD dwDuration, UINT nTime)
{
	switch (dwDuration)
	{
	case TTDT_AUTOPOP:
		m_nTimeAutoPop = nTime;
		break;
	case TTDT_INITIAL :
		m_nTimeInitial = nTime;
		break;
	case TTDT_RESHOW:
		m_nTimeReShow = nTime;
		break;
	}
} // End SetDelayTime

/////////////////////////////////////////////////////////////////////
// CustomToolTip::GetDelayTime
// Retrieves the initial, pop-up, and reshow durations currently set 
// for a CustomToolTip control
//
// Parameters:
//   dwDuration	[in] - Flag that specifies which duration value will 
//					   be retrieved. This parameter can be one of the 
//					   following values:
//			
//			TTDT_AUTOPOP  - Retrieve the length of time the tool tip 
//							window remains visible if the pointer is 
//							stationary within a tool's bounding rectangle. 
//			TTDT_INITIAL  - Retrieve the length of time the pointer 
//							must remain stationary within a tool's bounding 
//							rectangle before the tool tip window appears. 
//			TTDT_RESHOW   - Retrieve the length of time it takes for 
//							subsequent tool tip windows to appear as the 
//							pointer moves from one tool to another. 
//
// Return value:
//	The specified delay time, in milliseconds
///////////////////////////////////////////////////////////////////////
UINT CustomToolTip::GetDelayTime(DWORD dwDuration) const
{
	UINT nTime = 0;
	switch (dwDuration)
	{
		case TTDT_AUTOPOP:
			nTime = m_nTimeAutoPop;
			break;
		case TTDT_INITIAL:
			nTime = m_nTimeInitial;
			break;
		case TTDT_RESHOW:
			nTime = m_nTimeReShow;
			break;
	}

	return nTime;
} // End GetDelayTime

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::SetSize (public member function)
//    Sets the specified size
//
//  Parameters :
//		nSizeIndex  [in] - index of the size
//		nValue [in] - size's value
//
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::SetSize(int nSizeIndex, UINT nValue)
{
//	TRACE(_T("CustomToolTip::SetSize(nSizeIndex = %d, nValue = %d)\n"), nSizeIndex, nValue);
	if (nSizeIndex >= TOOLTIP_SIZE_MAX_SIZES)
		return;

	m_nSizes [nSizeIndex] = nValue;
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::GetSize (public member function)
//    Gets the specified size
//
//  Parameters :
//		nSizeIndex  [in] - index of the size
//
//  Returns :
//		size's value
//
/////////////////////////////////////////////////////////////////////////////
UINT CustomToolTip::GetSize(int nSizeIndex)
{
//	TRACE(_T("CustomToolTip::GetSize(nSizeIndex = %d)\n"), nSizeIndex);
	if (nSizeIndex >= TOOLTIP_SIZE_MAX_SIZES)
		return 0;

	return m_nSizes [nSizeIndex];
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::SetDefaultSizes (public member function)
//    Sets all sizes to default values
//
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::SetDefaultSizes(BOOL bBalloonSize /* = TRUE */)
{
//	TRACE(_T("CustomToolTip::SetDefaultSizes()\n"));

	if (bBalloonSize)
	{
		SetSize(TOOLTIP_SIZE_ROUNDED_CX, 16);
		SetSize(TOOLTIP_SIZE_ROUNDED_CY, 16);
		SetSize(TOOLTIP_SIZE_MARGIN_CX, 12);
		SetSize(TOOLTIP_SIZE_MARGIN_CY, 12);
		SetSize(TOOLTIP_SIZE_SHADOW_CX, 4);
		SetSize(TOOLTIP_SIZE_SHADOW_CY, 4);
		SetSize(TOOLTIP_SIZE_WIDTH_ANCHOR, 12);
		SetSize(TOOLTIP_SIZE_HEIGHT_ANCHOR, 16);
		SetSize(TOOLTIP_SIZE_MARGIN_ANCHOR, 16);
		SetSize(TOOLTIP_SIZE_BORDER_CX, 1);
		SetSize(TOOLTIP_SIZE_BORDER_CY, 1);
	}
	else
	{
		SetSize(TOOLTIP_SIZE_ROUNDED_CX, 0);
		SetSize(TOOLTIP_SIZE_ROUNDED_CY, 0);
		SetSize(TOOLTIP_SIZE_MARGIN_CX, 3);
		SetSize(TOOLTIP_SIZE_MARGIN_CY, 1);
		SetSize(TOOLTIP_SIZE_SHADOW_CX, 0);
		SetSize(TOOLTIP_SIZE_SHADOW_CY, 0);
		SetSize(TOOLTIP_SIZE_WIDTH_ANCHOR, 0);
		SetSize(TOOLTIP_SIZE_HEIGHT_ANCHOR, 0);
		SetSize(TOOLTIP_SIZE_MARGIN_ANCHOR, 0);
		SetSize(TOOLTIP_SIZE_BORDER_CX, 1);
		SetSize(TOOLTIP_SIZE_BORDER_CY, 1);
	}
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::SetDirection (public member function)
//    Sets the tooltip's direction
//
//  Parameters :
//		nDirection  [in] - direction of the tooltip
//
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::SetDirection(UINT nDirection /* = TOOLTIP_RIGHT_BOTTOM */, int nIndexTool /* = ICustomToolTip::TOOLTIP_TOOL_NOEXIST */)
{
//	TRACE(_T("CustomToolTip::SetDirection(nDirection = %d)\n"), nDirection);

	if (nDirection >= ICustomToolTip::TOOLTIP_MAX_DIRECTIONS)
		return;

	if (!IsExistTool(nIndexTool))
	{
		m_nDirection = nDirection;
	}
	else
	{
		ICustomToolTip::ToolTipInfo ti;
		GetTool(nIndexTool, ti);
		ti.m_nDirection = nDirection;
		ti.m_nMask |= ICustomToolTip::TOOLTIP_MASK_DIRECTION;
		SetAtTool(nIndexTool, ti);
	}
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::GetDirection (public member function)
//    Gets the tooltip's direction
//
//  Returns :
//		tooltip's direction
//
/////////////////////////////////////////////////////////////////////////////
UINT CustomToolTip::GetDirection(int nIndexTool /* = ICustomToolTip::TOOLTIP_TOOL_NOEXIST */)
{
//	TRACE(_T("CustomToolTip::GetDirection()\n"));

	if (IsExistTool(nIndexTool))
	{
		ICustomToolTip::ToolTipInfo ti;
		GetTool(nIndexTool, ti);
		if (ti.m_nMask & ICustomToolTip::TOOLTIP_MASK_DIRECTION)
			return ti.m_nDirection;
	}
	return m_nDirection;
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::SetBehaviour (public member function)
//    Sets the tooltip's direction
//
//  Parameters :
//		m_nBehaviour  [in] - direction of the tooltip
//
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////

void CustomToolTip::SetBehaviour(UINT m_nBehaviour /* = 0 */, int nIndexTool /* = ICustomToolTip::TOOLTIP_TOOL_NOEXIST */)
{
//	TRACE(_T("CustomToolTip::SetBehaviour(m_nBehaviour = 0x%X)\n"), m_nBehaviour);

	if (IsExistTool(nIndexTool))
	{
		ICustomToolTip::ToolTipInfo ti;
		GetTool(nIndexTool, ti);
		ti.m_nBehaviour = m_nBehaviour;
		ti.m_nMask |= ICustomToolTip::TOOLTIP_MASK_BEHAVIOUR;
		SetAtTool(nIndexTool, ti);
	}
	else m_nBehaviour = m_nBehaviour;
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::GetBehaviour (public member function)
//    Gets the tooltip's direction
//
//  Returns :
//		tooltip's direction
//
/////////////////////////////////////////////////////////////////////////////
UINT CustomToolTip::GetBehaviour(int nIndexTool /* = ICustomToolTip::TOOLTIP_TOOL_NOEXIST */)
{
//	TRACE(_T("CustomToolTip::GetBehaviour()\n"));

	if (IsExistTool(nIndexTool))
	{
		ICustomToolTip::ToolTipInfo ti;
		GetTool(nIndexTool, ti);
		if (ti.m_nMask & ICustomToolTip::TOOLTIP_MASK_BEHAVIOUR)
			return ti.m_nBehaviour;
	}
	return m_nBehaviour;
}

/*
/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::SetMultipleShow (public member function)
//    Sets the multiple show for the tooltip
//
//  Parameters :
//		bMultiple	[in] - 
//		pWnd		[in] - the pointer to the window
//
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::SetMultipleShow(BOOL bMultiple  = TRUE , CWnd * pWnd  = NULL )
{
	if (bMultiple)
		ModifyStyles(TOOLTIP_MULTIPLE_SHOW, 0, pWnd);
	else
		ModifyStyles(0, TOOLTIP_MULTIPLE_SHOW, pWnd);
} // End of SetMultipleShow

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::IsMultipleShow (public member function)
//    
//
//  Parameters :
//		pWnd		[in] - the pointer to the window
//
//  Returns :
//		TRUE if for window sets the multiple show
//
/////////////////////////////////////////////////////////////////////////////
BOOL CustomToolTip::IsMultipleShow(CWnd * pWnd  = NULL )
{
	return (BOOL)(TOOLTIP_MULTIPLE_SHOW & GetStyles(pWnd));
} // End of IsMultipleShow
*/


/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::SetFont  (public member function)
//    Sets the new font to the control
//
//  Parameters :
//		lf			[in] - structure LOGFONT for the new font
//
//  Returns :
//		Nonzero if successful; otherwise 0.
//
/////////////////////////////////////////////////////////////////////////////
BOOL CustomToolTip::SetFont(LPLOGFONT lf)
{
//	TRACE(_T("CustomToolTip::SetFont()\n"));

//	m_font.DeleteObject();

	// Store font as the global default
    memcpy(&m_logFont, lf, sizeof(LOGFONT));

	return TRUE; //m_font.CreateFontIndirect(lf);
}  // End of SetFont

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::SetFont  (public member function)
//    Sets the new font to the control
//
//  Parameters :
//		font		[in] - new font
//
//  Returns :
//		Nonzero if successful; otherwise 0.
//
/////////////////////////////////////////////////////////////////////////////
BOOL CustomToolTip::SetFont(LPCTSTR lpszFaceName, int nSizePoints /* = 8 */,
									BOOL bUnderline /* = FALSE */, BOOL bBold /* = FALSE */,
									BOOL bStrikeOut /* = FALSE */, BOOL bItalic /* = FALSE */)
{
//	TRACE(_T("CustomToolTip::SetFont()\n"));

	HDC hDC = ::GetDC(m_hWnd);
	LOGFONT lf;
	memset (&lf, 0, sizeof(LOGFONT));

	_tcscpy (lf.lfFaceName, lpszFaceName);
	lf.lfHeight = -MulDiv (nSizePoints, GetDeviceCaps (hDC, LOGPIXELSY), 72);
	lf.lfUnderline = bUnderline;
	lf.lfWeight = bBold ? FW_BOLD : FW_NORMAL;
	lf.lfStrikeOut = bStrikeOut;
	lf.lfItalic = bItalic;

	if (hDC)
		::ReleaseDC(m_hWnd,hDC);

	return SetFont(&lf);
}  // End of SetFont

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::SetDefaultFonts  (public member function)
//    Sets default fonts of the control
//
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::SetDefaultFont()
{
//	TRACE(_T("CustomToolTip::SetDefaultFont()\n"));

	LPLOGFONT lpSysFont = GetSystemToolTipFont();

	SetFont(lpSysFont);
} // End of SetDefaultFonts

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::GetFont  (public member function)
//    Sets the new font to the control
//
//  Parameters :
//		lf		[out] - the current font's structure
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::GetFont(LPLOGFONT lf)
{
	memcpy(lf, &m_logFont, sizeof(LOGFONT));
}

static POINT mousePoint;
static HWND tooltipHWND;
static HWND parentHWND;
static UINT testFlags;
static HWND foundChildHWnd;

// new function to find a child that is over the cursor but not the tooltip window
BOOL CALLBACK EnumChildProc(HWND hwndChild, LPARAM lParam) 
{ 
	if (hwndChild==tooltipHWND) // don't count my tooltip window
	{
//		TRACE("Found ToolTip %08x\n",hwndChild);
		return TRUE;
	}
	if (GetParent(hwndChild)!=parentHWND) // don't do sub children
		return TRUE; 

	if (((DWORD)GetWindowLong(hwndChild, GWL_STYLE) & WS_VISIBLE) != WS_VISIBLE)
		return TRUE;
	
	if (testFlags&CWP_SKIPDISABLED && !IsWindowEnabled(hwndChild))
		return TRUE;
	
	RECT myRect; 
	
	GetWindowRect(hwndChild,&myRect);
	if (::PtInRect(&myRect,mousePoint))
	{
		foundChildHWnd = hwndChild;

//		TRACE("Found Child %08x\n",hwndChild);
		return FALSE;
	}
	return TRUE; 
} 

///////////////////////////////////////////////////////////////////
//
//
// Parameters:
//		pt		[in] - the mouse's coordinates in client coordinates
//
// Return values:
//      CWnd*	[out] - the pointer to the window under the mouse. Returns
//                      NULL if under the mouse no control.
///////////////////////////////////////////////////////////////////
HWND CustomToolTip::GetWndFromPoint(POINT & pt, BOOL bGetDisabled /* = TRUE */) const
{
	assert(m_hParentWnd);
	
	POINT point = pt;
	// Find the window under the cursor
	ClientToScreen(m_hParentWnd,&point);
//	TRACE("Checking Point %d,%d\n",point.x,point.y);
	mousePoint = point;
	tooltipHWND = m_hWnd;
	parentHWND = m_hParentWnd;
	testFlags = CWP_ALL;
	if (!bGetDisabled)
		testFlags |= CWP_SKIPDISABLED;
	foundChildHWnd = NULL;
	EnumChildWindows(m_hParentWnd,EnumChildProc,0);
	return foundChildHWnd;
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::AddTool (public member function)
//    adds or updates the tool
//
//  Parameters :
//		pWnd	[in] - the pointer to the window
//		nIdText  [in] - the tooltip's text id 
//		nIdIcon  [in] - the icon's identificator 
//
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::AddTool(HWND hWnd, UINT nIdText, HICON hIcon /* = NULL */,
						 LPCRECT lpRectTool /* = NULL */, UINT nIDTool /* = 0 */)
{
	TupString str;
	if (nIdText)
	{
		// Max size 512 from resource
		TCHAR chBuf[512];
		int nLen = ::LoadString(CustomControls::GetHInstance(), nIdText, &chBuf[0], 512);
		if(nLen != 0)
		{
			str = chBuf;
		}
	}
	AddTool(hWnd, str, hIcon, lpRectTool, nIDTool);
} // End AddTool

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::AddTool (public member function)
//    adds or updates the tool
//
//  Parameters :
//		pWnd	[in] - the pointer to the window
//		nIdText  [in] - the tooltip's text id 
//		nIdIcon  [in] - the icon's identificator 
//
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::AddTool(HWND hWnd, UINT nIdText, UINT nIdIcon,
						 LPCRECT lpRectTool /* = NULL */, UINT nIDTool /* = 0 */)
{
	TupString str;
	if (nIdText)
	{
		// Max size 512 from resource
		TCHAR chBuf[512];
		int nLen = ::LoadString(CustomControls::GetHInstance(), nIdText, &chBuf[0], 512);
		if(nLen != 0)
		{
			str = chBuf;
		}
	}
	AddTool(hWnd, str, nIdIcon, lpRectTool, nIDTool);
} // End AddTool

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::AddTool (public member function)
//    adds or updates the tool
//
//  Parameters :
//		pWnd	[in] - the pointer to the window
//		sTooltipText [in] - the tooltip's text 
//		nIdIcon  [in] - the icon's identificator 
//
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::AddTool(HWND hWnd, TupString sTooltipText, UINT nIdIcon,
						 LPCRECT lpRectTool /* = NULL */, UINT nIDTool /* = 0 */)
{
	HICON hIcon	= GetIconFromResources (nIdIcon);

	AddTool(hWnd, sTooltipText, hIcon, lpRectTool, nIDTool);
} // End AddTool

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::AddTool (public member function)
//    adds or updates the tool
//
//  Parameters :
//		pWnd	[in] - the pointer to the window
//		sTooltipText [in] - the tooltip's text 
//		hIcon   [in] - the icon's handle 
//
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::AddTool(HWND hWnd, TupString sTooltipText, HICON hIcon /* = NULL */,
						 LPCRECT lpRectTool /* = NULL */, UINT nIDTool /* = 0 */)
{
	// Store the tool information
	ICustomToolTip::ToolTipInfo ti;
	ti.m_hWnd = hWnd;
	ti.m_nIDTool = nIDTool;
	ti.m_hIcon = hIcon;
	ti.m_sTooltip = sTooltipText;
	ti.m_nMask = 0; //All values as default
	
    if (lpRectTool)
		ti.m_rectBounds = *lpRectTool;
	else
		::SetRectEmpty(&ti.m_rectBounds);

	AddTool(ti);
} // End AddTool

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::AddTool (public member function)
//    adds or updates the tool
//
//  Parameters :
//		ti		[in] - the tooltip's structure 
//
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::AddTool(ICustomToolTip::ToolTipInfo ti) 
{ 
	assert (ti.m_hWnd);  
//	TRACE (_T("CustomToolTip::AddTool = 0x%X\n"), ti.hWnd); 
	// Get bounding region for tooltip info 
	if (::IsRectEmpty(&ti.m_rectBounds))
    {  
		RECT rect; 
		::GetClientRect(ti.m_hWnd,&rect);
		//m_pParentWnd->ScreenToClient(rect); 
		ti.m_rectBounds = rect; 
    } 
	int nIndexTool = FindTool(ti.m_hWnd, &ti.m_rectBounds);
	if (!IsExistTool(nIndexTool)) 
		m_arrTools.Add(ti); 
	else 
		m_arrTools.SetAt(nIndexTool, ti); 
} // End AddTool

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::FindTool (public member function)
//    searches the tool under cursor point
//
//  Parameters :
//		pt	[in] - cursor point in client coordinates
//
//  Returns :
//		< 0  = Not available tool
//      >= 0 = Index of the found tool
//
/////////////////////////////////////////////////////////////////////////////
int CustomToolTip::FindTool(POINT & pt) 
{ 
	HWND hWnd = GetWndFromPoint(pt, m_nStyles & ICustomToolTip::TOOLTIP_SHOW_DISABLED); 
	
	//*!* Этот блок необходимо удалить
//	TupString str;
//	str.Format(_T("Find tool: 0x%X"), hWnd);
//	m_pParentWnd->SetWindowText(str);
//	TRACE("Find tool: 0x%X\n",hWnd);

	ICustomToolTip::ToolTipInfo pToolInfo;  
	int nSize = m_arrTools.GetSize(); 
	POINT ptClient;  
	// Find the window under the cursor 
	ClientToScreen(m_hParentWnd,&pt);
	for (int i = 0; i < nSize; i++) 
	{ 
		const ICustomToolTip::ToolTipInfo &pToolInfo = m_arrTools[i];
		if (hWnd == pToolInfo.m_hWnd)  
		{	
			ptClient = pt; 
			::ScreenToClient(hWnd, &ptClient); 
			if (::PtInRect(&pToolInfo.m_rectBounds,ptClient)) 
			{
				return i;  
			}
		}
	}  
	return ICustomToolTip::TOOLTIP_TOOL_NOEXIST;
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::FindTool (public member function)
//    searches the tool with window's pointer and rectangle
//
//  Parameters :
//		pWnd	[in] - the pointer to the window
//		lpRect	[in] - rectangle of the hot area. Or NULL for any window
//
//  Returns :
//		< 0  = Not available tool
//      >= 0 = Index of the found tool
//
/////////////////////////////////////////////////////////////////////////////
int CustomToolTip::FindTool(HWND hWnd, LPCRECT lpRect /* = NULL */)
{
	int nSize = m_arrTools.GetSize();
	for (int i = 0; i < nSize; i++)
	{
		 const ICustomToolTip::ToolTipInfo &toolInfo = m_arrTools[i];

		 if (hWnd == toolInfo.m_hWnd) 
		 {
			 if ((NULL == lpRect) || EqualRect(lpRect,&toolInfo.m_rectBounds))
				 return i;
		 }
	}
	return ICustomToolTip::TOOLTIP_TOOL_NOEXIST;
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::FindTool (public member function)
//    searches the tool with id
//
//  Parameters :
//		nIDTool	[in] - id of the window
//
//  Returns :
//		< 0  = Not available tool
//      >= 0 = Index of the found tool
//
/////////////////////////////////////////////////////////////////////////////
int CustomToolTip::FindTool(UINT nIDTool)
{
	int nSize = m_arrTools.GetSize();
    for (int i = 0; i < nSize; i++)
    {
		 const ICustomToolTip::ToolTipInfo &toolInfo = m_arrTools[i];
        if (nIDTool == toolInfo.m_nIDTool)
			return i;
    }
	return ICustomToolTip::TOOLTIP_TOOL_NOEXIST;
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::GetTool (public member function)
//    gets the tooltip's info
//
//  Parameters :
//		nIndexTool	[in]  - the index of the tool
//		ti		[out] - the tooltip's structure 
//
//  Returns :
//		FALSE - tool not found
//
/////////////////////////////////////////////////////////////////////////////
BOOL CustomToolTip::GetTool(int nIndexTool, ICustomToolTip::ToolTipInfo & ti)
{
	if (!IsExistTool(nIndexTool))
		return FALSE;
	ti = m_arrTools[nIndexTool];
	return TRUE;
} // End GetTool

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::RemoveTool (public member function)
//    removes the tool
//
//  Parameters :
//		pWnd	[in]  - the pointer to the window
//
//  Returns :
//		None 
//
/////////////////////////////////////////////////////////////////////////////
BOOL CustomToolTip::RemoveTool(int nIndexTool)
{
	//TRACE (_T("CustomToolTip::RemoveTool\n"));

	if (!IsExistTool(nIndexTool))
		return FALSE;
	
	m_arrTools.RemoveAt(nIndexTool);

	return TRUE;
} // End RemoveTool

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::RemoveAllTools (public member function)
//    removes the tool
//
//  Parameters :
//		None
//
//  Returns :
//		None 
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::RemoveAllTools()
{
//	TRACE (_T("CustomToolTip::RemoveAllTools\n"));
	
	m_arrTools.RemoveAll();
} // End RemoveAllTools

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::IsExistTool (public member function)
//    removes the tool
//
//  Parameters :
//		nIndexTool	[in]  - the index of the tool
//
//  Returns :
//		TRUE - the tool is exist
//
/////////////////////////////////////////////////////////////////////////////
BOOL CustomToolTip::IsExistTool(int nIndexTool)
{
	return (BOOL)((nIndexTool < m_arrTools.GetSize()) && (nIndexTool >= 0));
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::SetAtTool (public member function)
//    adds or updates the tool
//
//  Parameters :
//		nIndexTool	[in] - the index of the tool
//		ti		[in] - the tooltip's structure 
//
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::SetAtTool(int nIndexTool, ICustomToolTip::ToolTipInfo & ti)
{
	if (!IsExistTool(nIndexTool))
		return;
	m_arrTools.SetAt(nIndexTool, ti);
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::ShowHelpTooltip (public member function)
//    shows the tooltip as help window
//
//  Parameters :
//		pt	[in] - the point of the tooltip's anchor in client coordinates
//		nIdText  [in] - the tooltip's text id 
//		nIdIcon  [in] - the icon's identificator 
//
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::ShowHelpTooltip(POINT & pt, UINT nIdText, HICON hIcon /* = NULL */)
{
	TupString str;
	// Max size 512 from resource
	TCHAR chBuf[512];
	int nLen = ::LoadString(CustomControls::GetHInstance(), nIdText, &chBuf[0], 512);
	if(nLen != 0)
	{
		str = chBuf;
	}
	ShowHelpTooltip(pt, str, hIcon);
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::ShowHelpTooltip (public member function)
//    shows the tooltip as help window
//
//  Parameters :
//		pt	[in] - the point of the tooltip's anchor in client coordinates
//		nIdText  [in] - the tooltip's text id 
//		nIdIcon  [in] - the icon's identificator 
//
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::ShowHelpTooltip(POINT & pt, UINT nIdText, UINT nIdIcon)
{
	TupString str;
	// Max size 512 from resource
	TCHAR chBuf[512];
	int nLen = ::LoadString(CustomControls::GetHInstance(), nIdText, &chBuf[0], 512);
	if(nLen != 0)
	{
		str = chBuf;
	}
	ShowHelpTooltip(pt, str, nIdIcon);
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::ShowHelpTooltip (public member function)
//    shows the tooltip as help window
//
//  Parameters :
//		pt	[in] - the point of the tooltip's anchor in client coordinates
//		sTooltipText [in] - the tooltip's text 
//		nIdIcon  [in] - the icon's identificator 
//
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::ShowHelpTooltip(POINT & pt, TupString sTooltipText, UINT nIdIcon)
{
	HICON hIcon	= GetIconFromResources(nIdIcon);
	ShowHelpTooltip(pt, sTooltipText, hIcon);
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::ShowHelpTooltip (public member function)
//    shows the tooltip as help window
//
//  Parameters :
//		pt	[in] - the point of the tooltip's anchor in client coordinates
//		sTooltipText [in] - the tooltip's text 
//		hIcon   [in] - the icon's handle 
//
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::ShowHelpTooltip(POINT & pt, TupString sTooltipText, HICON hIcon /* = NULL */)
{
	ICustomToolTip::ToolTipInfo ti;
	ti.m_hWnd = m_hParentWnd;
	ti.m_hIcon = hIcon;
	ti.m_sTooltip = sTooltipText;
	ti.m_nMask = 0; //All values as default
	ShowHelpTooltip(pt, ti);
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::ShowHelpTooltip (public member function)
//    shows the tooltip as help window
//
//  Parameters :
//		pt		[in] - the point of the tooltip's anchor in client coordinates
//		ti		[in] - the tooltip's structure 
//
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::ShowHelpTooltip(POINT & pt, ICustomToolTip::ToolTipInfo & ti)
{
//	TRACE(_T("ShowHelpTooltip()\n"));
	m_nIndexDisplayWnd = ICustomToolTip::TOOLTIP_TOOL_NOEXIST;
	m_nIndexCurrentWnd = ICustomToolTip::TOOLTIP_TOOL_HELPER;
	m_toolInfo = ti;

	m_ptOriginal = pt;
	ClientToScreen(m_hParentWnd,&m_ptOriginal);
	
	//Start the show timer
	OnTimer(TOOLTIP_SHOW);
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::SetImageList (public member function)
//    sets the image list to tooltip
//
//  Parameters :
//		nIdBitmap	[in] - Resource IDs of the bitmap to be associated with the image list
//		cx			[in] - Dimensions of each image, in pixels.
//		cy			[in] - Dimensions of each image, in pixels.
//		nCount		[in] - Number of images that the image list initially contains.
//		crMask		[in] - Color used to generate a mask. Each pixel of this color in the 
//						   specified bitmap is changed to black, and the corresponding 
//						   bit in the mask is set to one.
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::SetImageList(UINT nIdBitmap, int cx, int cy, int nCount, COLORREF crMask /* = RGB(255, 0, 255) */)
{
	// Load bitmap
	HBITMAP hBitmap = GetBitmapFromResources(nIdBitmap);
	SetImageList(hBitmap, cx, cy, nCount, crMask);
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::SetImageList (public member function)
//    sets the image list to tooltip
//
//  Parameters :
//		hBitmap		[in] - Handle of the bitmap to be associated with the image list
//		cx			[in] - Dimensions of each image, in pixels.
//		cy			[in] - Dimensions of each image, in pixels.
//		nCount		[in] - Number of images that the image list initially contains.
//		crMask		[in] - Color used to generate a mask. Each pixel of this color in the 
//						   specified bitmap is changed to black, and the corresponding 
//						   bit in the mask is set to one.
//  Returns :
//		None
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::SetImageList(HBITMAP hBitmap, int cx, int cy, int nCount, COLORREF crMask /* = RGB(255, 0, 255) */)
{
	if (m_imgTooltip != NULL)
	{
		ImageList_Destroy(m_imgTooltip);
	}

	m_imgTooltip = ImageList_Create(cx,cy,ILC_COLOR32 | ILC_MASK, nCount, 1);

	ImageList_AddMasked(m_imgTooltip,hBitmap,crMask);
	m_szImage.cx = cx;
	m_szImage.cy = cy;
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::GetImageList (public member function)
//    gets the image list from tooltip
//
//  Parameters :
//		sz		   [out] - Dimensions of each image, in pixels.
//  Returns :
//		A pointer to a CImageList object
//
/////////////////////////////////////////////////////////////////////////////
HIMAGELIST CustomToolTip::GetImageList(SIZE & sz)
{
	sz = m_szImage;
	return m_imgTooltip;
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::AddNameOfResource (public member function)
//    Associates the resource name with him ID
//
//  Parameters :
//		sName		[in] - A resource name in format string of the tooltip
//		nID			[in] - A resource ID associated with name.
//		nTypeRes	[in] - A resource type:
//							TYPE_RES_ICON - a resource is a icon
//							TYPE_RES_BITMAP - a resource is a bitmap
//							TYPE_RES_MASK_BITMAP - a resource is a transparent bitmap
//		crMask		[in] - Color used to generate a mask. Each pixel of this color in the 
//						   specified bitmap is changed to black, and the corresponding 
//						   bit in the mask is set to one.
//  Returns :
//		None
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::AddNameOfResource(TupString sName, UINT nID, BYTE nTypeRes /* = TYPE_RES_TRAN_BITMAP */, COLORREF crMask /* = RGB(255, 0, 255) */)
{
	if (sName.IsEmpty() || (nID == 0) || (nTypeRes >= ICustomToolTip::MAX_TYPES_RES))
		return;
	
	ICustomToolTip::ToolTipNameRes nr;
	nr.m_sName = sName;
	nr.m_nID = nID;
	nr.m_nTypeRes = nTypeRes;
	nr.m_crMask = crMask;

	m_arrNameRes.Add(nr);
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::FindIdOfResource (public member function)
//    Search the ID resource by his name
//
//  Parameters :
//		sName		[in] - A resource name in format string of the tooltip
//  Returns :
//		The array's index with ID resource associated with name or -1 if ID not found
/////////////////////////////////////////////////////////////////////////////
int CustomToolTip::FindIdOfResource(TupString sName)
{
	for (int i = 0; i < m_arrNameRes.GetSize(); i++)
	{
		const ICustomToolTip::ToolTipNameRes &nr = m_arrNameRes[i];
		if (!sName.CompareNoCase(nr.m_sName))
			return i;
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::FindIdOfResource (public member function)
//    Search the resource name by his ID
//
//  Parameters :
//		nID		[in] - A ID resource
//  Returns :
//		The array's index with resource name associated with ID or -1 if name not found
/////////////////////////////////////////////////////////////////////////////
int CustomToolTip::FindNameOfResource(UINT nID)
{
	for (int i = 0; i < m_arrNameRes.GetSize(); i++)
	{
		const ICustomToolTip::ToolTipNameRes &nr = m_arrNameRes[i];
		if (nr.m_nID == nID)
			return i;
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::FindIdOfResource (public member function)
//    Removes resource name by index of the array
//
//  Parameters :
//		nIndex		[in] - A The index of the resource name in the array
//  Returns :
//		None
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::RemoveNameOfResource(int nIndex)
{
	if (nIndex < m_arrNameRes.GetSize())
		m_arrNameRes.RemoveAt(nIndex);
}

/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::FindIdOfResource (public member function)
//    Removes all resource names from the array
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::RemoveAllNamesOfResource()
{
	m_arrNameRes.RemoveAll();
}


/////////////////////////////////////////////////////////////////////////////
//  CustomToolTip::SetParent (public member function)
//    Sets the hwnd of the parent window
//
//  Parameters:
//    hParentWnd [in] - the HWND of the new parent window
//
/////////////////////////////////////////////////////////////////////////////
void CustomToolTip::SetParentWnd(HWND hParentWnd)
{
	m_hParentWnd = hParentWnd;
}



} // namespace AttrControls