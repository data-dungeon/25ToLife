// This is part of the Professional User Interface Suite library.
// Copyright (C) 2001-2004 FOSS Software, Inc.
// All rights reserved.
//
// http://www.prof-uis.com
// http://www.fossware.com
// mailto:support@fossware.com
//
// This source code can be used, modified and redistributed
// under the terms of the license agreement that is included
// in the Professional User Interface Suite package.
//
// Warranties and Disclaimers:
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND
// INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
// IN NO EVENT WILL FOSS SOFTWARE INC. BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES,
// INCLUDING DAMAGES FOR LOSS OF PROFITS, LOSS OR INACCURACY OF DATA,
// INCURRED BY ANY PERSON FROM SUCH PERSON'S USAGE OF THIS SOFTWARE
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

#include "stdafx.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXT_RESIZABLE_DIALOG_H)
	#include <ExtResizableDialog.h>
#endif

#if (!defined __EXTCUSTOMIZE_H)
	#include <ExtCustomize.h>
#endif

#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	#if (!defined __EXT_POPUP_CTRL_MENU_H)
		#include <ExtPopupCtrlMenu.h>
	#endif
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)

#if (!defined __EXT_CMD_ICON_H)
	#include <ExtCmdIcon.h>
#endif

#if (!defined __ExtCmdManager_H)
	#include <ExtCmdManager.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __EXT_MENUCONTROLBAR_H)
	#include <ExtMenuControlBar.h>
#endif

#if (!defined __EXTDOCKBAR_H)
	#include <../Src/ExtDockBar.h>
#endif

#if (!defined __EXT_STATUSCONTROLBAR_H)
	#include <ExtStatusControlBar.h>
#endif

#if( !defined __EXTMINIDOCKFRAMEWND_H)
	#include "ExtMiniDockFrameWnd.h"
#endif

#if (!defined __EXT_LOCALIZATION_H)
	#include <../Src/ExtLocalization.h>
#endif

#if (!defined __EXT_MFC_NO_ICONEDITDLG)
	#if (!defined __EXT_ICONEDITDLG_H)
		#include <ExtIconEditDlg.h>
	#endif
#endif // (!defined __EXT_MFC_NO_ICONEDITDLG)

#include <../profuisdll/resource.h>

/////////////////////////////////////////////////////////////////////////////
// CExtCustomizeSite::CResizerWnd

CExtCustomizeSite::CResizerWnd::CResizerWnd(
	bool bHorz // = true
	)
	: m_hWndBase( NULL )
	, m_bHorz( bHorz )
{
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
CExtLocalResourceHelper _LRH;
CWinApp * pApp = ::AfxGetApp();
	ASSERT_VALID( pApp );
	m_hCursor =
		pApp->LoadCursor(
			m_bHorz
				? ( MAKEINTRESOURCE( IDC_EXT_RESIZE_H1 ) )
				: ( MAKEINTRESOURCE( IDC_EXT_RESIZE_V1 ) )
			);
	if( m_hCursor == NULL )
	{
		ASSERT( FALSE );
		m_hCursor =
			::LoadCursor(
				NULL,
				m_bHorz ? (IDC_SIZEWE) : (IDC_SIZENS)
				);
	} // if( m_hCursor == NULL )
#else // if - (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
	m_hCursor =
			::LoadCursor(
				NULL,
				m_bHorz ? (IDC_SIZEWE) : (IDC_SIZENS)
				);
#endif // if NOT - (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
	ASSERT( m_hCursor != NULL );
	
	m_clrRect = ::GetSysColor( COLOR_BTNTEXT );

	m_sizeRectMetric = g_PaintManager->GetDropDividerMerics();
	m_sizeRectMetric.cx /= 2;
	m_sizeRectMetric.cy /= 2;
	m_sizeRectMetric.cx --;
	m_sizeRectMetric.cy --;
	if( m_sizeRectMetric.cx < 2 )
		m_sizeRectMetric.cx = 2;
	if( m_sizeRectMetric.cy < 2 )
		m_sizeRectMetric.cy = 2;
}

CExtCustomizeSite::CResizerWnd::~CResizerWnd()
{
	ASSERT( m_hCursor != NULL );
	::DestroyCursor( m_hCursor );
}

void CExtCustomizeSite::CResizerWnd::_RecalcCurrentRegion()
{
	if( m_rgnCurrent.GetSafeHandle() != NULL )
		m_rgnCurrent.DeleteObject();
	ASSERT( m_sizeRectMetric.cx >= 2 && m_sizeRectMetric.cy >= 2 );
CRect rc( m_rcScreenCurrent );
	rc.OffsetRect( -rc.TopLeft() );
CRgn rgnInner;
	VERIFY( m_rgnCurrent.CreateRectRgnIndirect( &rc ) );
	rc.DeflateRect( m_sizeRectMetric );
	VERIFY( rgnInner.CreateRectRgnIndirect( &rc ) );
	m_rgnCurrent.CombineRgn( &m_rgnCurrent, &rgnInner, RGN_DIFF );
	ASSERT( m_rgnCurrent.GetSafeHandle() != NULL );
}

void CExtCustomizeSite::CResizerWnd::_ActivateState(
	bool bUseCurrentWindowRect
	)
{
	if( bUseCurrentWindowRect )
		GetWindowRect( &m_rcScreenCurrent );
	if( GetStyle() & WS_VISIBLE )
		ShowWindow( SW_HIDE );
	SetWindowRgn( NULL, FALSE );
	_RecalcCurrentRegion();
	SetWindowRgn( (HRGN)m_rgnCurrent.GetSafeHandle(), FALSE );
	SetWindowPos(
		&wndTopMost,
		m_rcScreenCurrent.left, m_rcScreenCurrent.top,
		m_rcScreenCurrent.Width(), m_rcScreenCurrent.Height(),
		SWP_SHOWWINDOW|SWP_NOACTIVATE
		);
	ASSERT( m_hCursor != NULL );
	::SetCursor( m_hCursor );
	Invalidate();
	UpdateWindow();
	_RepaintBase();
}

void CExtCustomizeSite::CResizerWnd::_RepaintBase()
{
bool bDirectRapaint = true;
CWnd * pWndPermanent = CWnd::FromHandlePermanent(m_hWndBase);
	if( pWndPermanent != NULL )
	{
		pWndPermanent->UpdateWindow();
		CExtPopupBaseWnd * pPopup =
			DYNAMIC_DOWNCAST( CExtPopupBaseWnd, pWndPermanent );
		if( pPopup != NULL )
		{
			bDirectRapaint = false;
			CClientDC dc( pPopup );
			pPopup->_DoPaint( dc );
		} // if( pPopup != NULL )
	} // if( pWndPermanent != NULL )
	if( bDirectRapaint )
	{
		::InvalidateRect( m_hWndBase, NULL, TRUE );
		::UpdateWindow( m_hWndBase );
	} // if( bDirectRapaint )
}

void CExtCustomizeSite::CResizerWnd::_AdjustCurrentRect()
{
int nExtent =
		m_bHorz
			? m_rcScreenCurrent.Width()
			: m_rcScreenCurrent.Height()
			;
	if( m_nExtentMin <= nExtent && nExtent <= m_nExtentMax )
		return;
	if( nExtent < m_nExtentMin )
		nExtent = m_nExtentMin;
	else if( nExtent > m_nExtentMax )
		nExtent = m_nExtentMax;
	if( m_nResizingHT > 0 )
	{
		if( m_bHorz )
			m_rcScreenCurrent.right = m_rcScreenCurrent.left + nExtent;
		else
			m_rcScreenCurrent.bottom = m_rcScreenCurrent.top + nExtent;
	} // if( m_nResizingHT > 0 )
	else
	{
		if( m_bHorz )
			m_rcScreenCurrent.left = m_rcScreenCurrent.right - nExtent;
		else
			m_rcScreenCurrent.top = m_rcScreenCurrent.bottom - nExtent;
	} // else from if( m_nResizingHT > 0 )
}

bool CExtCustomizeSite::CResizerWnd::DoResizerTracking(
	HWND hWndBase,
	CRect & rcItemClient, // in client of hWndParent
	int nResizingHT, // must be -1 or 1
	int nExtentMin,
	int nExtentMax
	)
{
	ASSERT_VALID( this );
	ASSERT( nResizingHT != 0 );
	ASSERT( nExtentMin > 0 );
	ASSERT( nExtentMin <= nExtentMax );
	ASSERT( m_hCursor != NULL );
	if(		m_hWnd != NULL
		||	hWndBase == NULL
		||	(! ::IsWindow(hWndBase) )
		)
	{
		ASSERT( FALSE );
		return false;
	}
	m_hWndBase = hWndBase;
CPoint ptCursorLast;
	if( ! ::GetCursorPos(&ptCursorLast) )
		return false;
	m_nResizingHT = nResizingHT;
	m_nExtentMin = nExtentMin;
	m_nExtentMax = nExtentMax;

static LPCTSTR g_sClassName = _T("ProfUIS-CustomizeSite-ResizerWindow");
WNDCLASS wndclass;
HINSTANCE hInst = AfxGetInstanceHandle();
	if( ! ::GetClassInfo(
			hInst,
			g_sClassName,
			&wndclass
			)
		)
	{
		wndclass.style = CS_GLOBALCLASS;
		wndclass.lpfnWndProc = ::DefWindowProc;
		wndclass.cbClsExtra = wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInst;
		wndclass.hIcon = NULL;
		wndclass.hCursor = m_hCursor;
		wndclass.hbrBackground = NULL; 
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = g_sClassName;
		if( !::AfxRegisterClass( &wndclass ) )
		{
			ASSERT( FALSE );
			return false;
		}
	}

CWnd * pWndParent = CWnd::FromHandle(m_hWndBase);
CRect rcScreenInitial( rcItemClient );
	pWndParent->ClientToScreen( &rcScreenInitial );
	m_rcScreenCurrent = rcScreenInitial;
	if(	!CWnd::CreateEx(
			WS_EX_TOPMOST,
			g_sClassName,
			_T(""),
			WS_POPUP,
			rcScreenInitial.left,
			rcScreenInitial.top,
			rcScreenInitial.Width(),
			rcScreenInitial.Height(),
			m_hWndBase, //pWndParent,
			(HMENU)NULL,
			(LPVOID)NULL
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	_AdjustCurrentRect();
	_ActivateState( true );

int nAdjustVal = 0;
	if( m_bHorz )
	{
		if( m_nResizingHT > 0 )
			nAdjustVal = ptCursorLast.x - m_rcScreenCurrent.right;
		else
			nAdjustVal = m_rcScreenCurrent.left - ptCursorLast.x;
	} // if( m_bHorz )
	else
	{
		if( m_nResizingHT > 0 )
			nAdjustVal = ptCursorLast.y - m_rcScreenCurrent.bottom;
		else
			nAdjustVal = m_rcScreenCurrent.top - ptCursorLast.y;
	} // else from if( m_bHorz )

HWND hWndOwn = m_hWnd;
	::SetCapture( hWndOwn );
bool bRetVal = false, bStop = false;
MSG msg;
	for( ; (!bStop) && (!bRetVal) && (hWndOwn != NULL) && ::IsWindow(hWndOwn); )
	{
		if( ! ::WaitMessage() )
			break;
		for( ; PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE); )
		{
			switch( msg.message )
			{
			case WM_LBUTTONUP:
				bStop = true;
				bRetVal = true;
			break;
			case WM_LBUTTONDOWN:
			case WM_LBUTTONDBLCLK:
			case WM_RBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONDBLCLK:
			case WM_MBUTTONUP:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONDBLCLK:
			case WM_NCLBUTTONUP:
			case WM_NCLBUTTONDOWN:
			case WM_NCLBUTTONDBLCLK:
			case WM_NCRBUTTONUP:
			case WM_NCRBUTTONDOWN:
			case WM_NCRBUTTONDBLCLK:
			case WM_NCMBUTTONUP:
			case WM_NCMBUTTONDOWN:
			case WM_NCMBUTTONDBLCLK:
			case WM_CONTEXTMENU:
			case WM_ACTIVATEAPP:
			case WM_CANCELMODE:
			case WM_SETTINGCHANGE:
			case WM_SYSCOLORCHANGE:
				bStop = true;
			break;
			case WM_CAPTURECHANGED:
				{
					HWND hWndCapture = ::GetCapture();
					if( hWndCapture != hWndOwn )
						bStop = true;
				}
			break;
			case WM_MOUSEMOVE:
				{
					CPoint ptCursor;
					if( ! ::GetCursorPos(&ptCursor) )
					{
						PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
						continue;
					}
					CRect rcScreenPrev( m_rcScreenCurrent );
					if( m_bHorz )
					{
						if( ptCursorLast.x != ptCursor.x )
						{
							if( m_nResizingHT > 0 )
								m_rcScreenCurrent.right = ptCursor.x + nAdjustVal;
							else
								m_rcScreenCurrent.left = ptCursor.x + nAdjustVal;
						} // if( ptCursorLast.x != ptCursor.x )
					} // if( m_bHorz )
					else
					{
						if( ptCursorLast.y != ptCursor.y )
						{
							if( m_nResizingHT > 0 )
								m_rcScreenCurrent.bottom = ptCursor.y + nAdjustVal;
							else
								m_rcScreenCurrent.top = ptCursor.y + nAdjustVal;
						} // if( ptCursorLast.y != ptCursor.y )
					} // else from if( m_bHorz )
					_AdjustCurrentRect();
					if( rcScreenPrev != m_rcScreenCurrent )
						_ActivateState( false );
					ptCursorLast = ptCursor;
					PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
					continue;
				}
			//break;
			default:
				if(	WM_KEYFIRST <= msg.message
					&& msg.message <= WM_KEYLAST
					)
				{
					bStop = true;
					break;
				}
			break;
			}; // switch( msg.message )
			if( bStop || bRetVal )
				break;
			if( !AfxGetThread()->PumpMessage() )
				break;
		} // for( ; PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE); )
	} // for( ; (!bStop) && (!bRetVal) && (hWndOwn != NULL) && ::IsWindow(hWndOwn); )

	::ReleaseCapture();
	if( hWndOwn != NULL && ::IsWindow(hWndOwn) )
	{
		::ShowWindow( hWndOwn, SW_HIDE );
		::SetWindowRgn( hWndOwn, NULL, FALSE );
		::DestroyWindow( hWndOwn );
	}
	if( ! ::IsWindow(m_hWndBase) )
		return false;
	_RepaintBase();

	if( !bRetVal )
		return false;
	
	pWndParent = CWnd::FromHandle(m_hWndBase);
	rcItemClient = m_rcScreenCurrent;
	pWndParent->ScreenToClient( &rcItemClient );
	return true;
}

LRESULT CExtCustomizeSite::CResizerWnd::WindowProc(
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
{
	if(		message == WM_NCCALCSIZE
		||	message == WM_NCPAINT
		)
		return 0L;
	if( message == WM_MOUSEACTIVATE )
		return MA_NOACTIVATE;
	if( message == WM_ERASEBKGND )
		return 1L;
	if( message == WM_PAINT )
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		CPaintDC dcPaint( this );
		CExtMemoryDC dc( &dcPaint, &rcClient );
		dc.FillSolidRect( &rcClient, m_clrRect );
		return 0L;
	} // if( message == WM_PAINT )
	if( message == WM_SETCURSOR )
	{
		ASSERT( m_hCursor != NULL );
		::SetCursor( m_hCursor );
		return TRUE;
	} // if( message == WM_SETCURSOR )
//	if( message == WM_SIZE || message == WM_WINDOWPOSCHANGED )
//	{
//		LRESULT lResult =
//			CWnd::WindowProc( message, wParam, lParam );
//		if( (GetStyle() & WS_VISIBLE) == 0 )
//			_RepaintBase();
//		return lResult;
//	}
	return CWnd::WindowProc( message, wParam, lParam );
}

/////////////////////////////////////////////////////////////////////////////
// CExtCustomizeSite::CCmdKeyInfo

CExtCustomizeSite::CCmdKeyInfo::CCmdKeyInfo( const ACCEL & _dataAccel )
	: m_bVirt( ( (_dataAccel.fVirt&FVIRTKEY) != 0 ) ? true : false )
	, m_bCtrl( ( (_dataAccel.fVirt&FCONTROL) != 0 ) ? true : false )
	, m_bAlt( ( (_dataAccel.fVirt&FALT) != 0 ) ? true : false )
	, m_bShift( ( (_dataAccel.fVirt&FSHIFT) != 0 ) ? true : false )
	, m_nKey( _dataAccel.key )
{
}

CExtCustomizeSite::CCmdKeyInfo::CCmdKeyInfo(
	bool bVirt, // = false
	bool bCtrl, // = false
	bool bAlt, // = false
	bool bShift, // = false
	WORD nKey // = 0
	)
	: m_bVirt( bVirt )
	, m_bCtrl( bCtrl )
	, m_bAlt( bAlt )
	, m_bShift( bShift )
	, m_nKey( nKey )
{
}

CExtCustomizeSite::CCmdKeyInfo::CCmdKeyInfo(
	const CExtCustomizeSite::CCmdKeyInfo & other
	)
	: m_bVirt( other.m_bVirt )
	, m_bCtrl( other.m_bCtrl )
	, m_bAlt( other.m_bAlt )
	, m_bShift( other.m_bShift )
	, m_nKey( other.m_nKey )
{
}

CExtCustomizeSite::CCmdKeyInfo::CCmdKeyInfo( BYTE nVirtFlags, WORD nKey )
	: m_bVirt( ( (nVirtFlags&FVIRTKEY) != 0 ) ? true : false )
	, m_bCtrl( ( (nVirtFlags&FCONTROL) != 0 ) ? true : false )
	, m_bAlt( ( (nVirtFlags&FALT) != 0 ) ? true : false )
	, m_bShift( ( (nVirtFlags&FSHIFT) != 0 ) ? true : false )
	, m_nKey( nKey )
{
}

CExtCustomizeSite::CCmdKeyInfo::CCmdKeyInfo(
	DWORD dwKeyInfo
	)
	: m_bVirt( ( (((WORD)(dwKeyInfo>>16))&FVIRTKEY) != 0 ) ? true : false )
	, m_bCtrl( ( (((WORD)(dwKeyInfo>>16))&FCONTROL) != 0 ) ? true : false )
	, m_bAlt( ( (((WORD)(dwKeyInfo>>16))&FALT) != 0 ) ? true : false )
	, m_bShift( ( (((WORD)(dwKeyInfo>>16))&FSHIFT) != 0 ) ? true : false )
	, m_nKey( (WORD)(dwKeyInfo&0x0FFFF) )
{
}

CExtCustomizeSite::CCmdKeyInfo::~CCmdKeyInfo()
{
}

CExtCustomizeSite::CCmdKeyInfo &
	CExtCustomizeSite::CCmdKeyInfo::operator=(
		const ACCEL & _dataAccel
		)
{
	m_bVirt = ( (_dataAccel.fVirt&FVIRTKEY) != 0 ) ? true : false;
	m_bCtrl = ( (_dataAccel.fVirt&FCONTROL) != 0 ) ? true : false;
	m_bAlt = ( (_dataAccel.fVirt&FALT) != 0 ) ? true : false;
	m_bShift = ( (_dataAccel.fVirt&FSHIFT) != 0 ) ? true : false;
	m_nKey = _dataAccel.key;
	return (*this);
}

CExtCustomizeSite::CCmdKeyInfo &
	CExtCustomizeSite::CCmdKeyInfo::operator=(
		const CExtCustomizeSite::CCmdKeyInfo & other
		)
{
	m_bVirt = other.m_bVirt;
	m_bCtrl = other.m_bCtrl;
	m_bAlt = other.m_bAlt;
	m_bShift = other.m_bShift;
	m_nKey = other.m_nKey;
	return (*this);
}

CExtCustomizeSite::CCmdKeyInfo &
	CExtCustomizeSite::CCmdKeyInfo::operator=(
		DWORD dwKeyInfo
		)
{
	m_bVirt = ( (((WORD)(dwKeyInfo>>16))&FVIRTKEY) != 0 ) ? true : false;
	m_bCtrl = ( (((WORD)(dwKeyInfo>>16))&FCONTROL) != 0 ) ? true : false;
	m_bAlt = ( (((WORD)(dwKeyInfo>>16))&FALT) != 0 ) ? true : false;
	m_bShift = ( (((WORD)(dwKeyInfo>>16))&FSHIFT) != 0 ) ? true : false;
	m_nKey = (WORD)(dwKeyInfo&0x0FFFF);
	return (*this);
}

bool CExtCustomizeSite::CCmdKeyInfo::operator!=(
	const CExtCustomizeSite::CCmdKeyInfo & other
	) const
{
	return ( DWORD(*this) != DWORD(other) ) ? true : false;
}
bool CExtCustomizeSite::CCmdKeyInfo::operator==(
	const CExtCustomizeSite::CCmdKeyInfo & other
	) const
{
	return ( DWORD(*this) == DWORD(other) ) ? true : false;
}
bool CExtCustomizeSite::CCmdKeyInfo::operator<(
	const CExtCustomizeSite::CCmdKeyInfo & other ) const
{
	return ( DWORD(*this) < DWORD(other) ) ? true : false;
}

bool CExtCustomizeSite::CCmdKeyInfo::operator<=(
	const CExtCustomizeSite::CCmdKeyInfo & other
	) const
{
	return ( DWORD(*this) <= DWORD(other) ) ? true : false;
}
bool CExtCustomizeSite::CCmdKeyInfo::operator>(
	const CExtCustomizeSite::CCmdKeyInfo & other
	) const
{
	return ( DWORD(*this) > DWORD(other) ) ? true : false;
}
bool CExtCustomizeSite::CCmdKeyInfo::operator>=(
	const CExtCustomizeSite::CCmdKeyInfo & other
	) const
{
	return ( DWORD(*this) >= DWORD(other) ) ? true : false;
}

CExtCustomizeSite::CCmdKeyInfo::operator DWORD() const
{
DWORD dwVal = ((DWORD)m_nKey);
	if( m_bVirt )
		dwVal |= (DWORD(FVIRTKEY))<<16;
	if( m_bCtrl )
		dwVal |= (DWORD(FCONTROL))<<16;
	if( m_bAlt )
		dwVal |= (DWORD(FALT))<<16;
	if( m_bShift )
		dwVal |= (DWORD(FSHIFT))<<16;
	return dwVal;
}

bool CExtCustomizeSite::CCmdKeyInfo::IsVirt() const
{
	return m_bVirt;
}
void CExtCustomizeSite::CCmdKeyInfo::SetVirt( bool bOn )
{
	m_bVirt = bOn;
}

bool CExtCustomizeSite::CCmdKeyInfo::IsCtrl() const
{
	return m_bCtrl;
}
void CExtCustomizeSite::CCmdKeyInfo::SetCtrl( bool bOn )
{
	m_bCtrl = bOn;
}

bool CExtCustomizeSite::CCmdKeyInfo::IsAlt() const
{
	return m_bAlt;
}
void CExtCustomizeSite::CCmdKeyInfo::SetAlt( bool bOn )
{
	m_bAlt = bOn;
}

bool CExtCustomizeSite::CCmdKeyInfo::IsShift() const
{
	return m_bShift;
}
void CExtCustomizeSite::CCmdKeyInfo::SetShift( bool bOn )
{
	m_bShift = bOn;
}

WORD CExtCustomizeSite::CCmdKeyInfo::GetKey() const
{
	return m_nKey;
}

void CExtCustomizeSite::CCmdKeyInfo::SetKey( WORD nKey )
{
	m_nKey = nKey;
}

bool CExtCustomizeSite::CCmdKeyInfo::IsEmpty() const
{
	return (m_nKey == 0) ? true : false;
}

void CExtCustomizeSite::CCmdKeyInfo::Empty()
{
	m_bVirt = m_bCtrl = m_bAlt = m_bShift = false;
	m_nKey = 0;
}

void CExtCustomizeSite::CCmdKeyInfo::FillAccel( ACCEL & _dataAccel ) const
{
	_dataAccel.fVirt = (BYTE)0;
	if( m_bVirt )
		_dataAccel.fVirt |= BYTE(FVIRTKEY);
	if( m_bCtrl )
		_dataAccel.fVirt |= BYTE(FCONTROL);
	if( m_bAlt )
		_dataAccel.fVirt |= BYTE(FALT);
	if( m_bShift )
		_dataAccel.fVirt |= BYTE(FSHIFT);
	_dataAccel.key = m_nKey;
}

void CExtCustomizeSite::CCmdKeyInfo::Serialize( CArchive & ar )
{
	if( ar.IsStoring() )
	{
		DWORD dwKeyInfo = (*this);
		ar << dwKeyInfo;
	} // if( ar.IsStoring() )
	else
	{
		DWORD dwKeyInfo = 0;
		ar >> dwKeyInfo;
		(*this) = dwKeyInfo;
	} // else from if( ar.IsStoring() )
}

/////////////////////////////////////////////////////////////////////////////
// CExtCustomizeSite::CCmdKeySet

CExtCustomizeSite::CCmdKeySet::CCmdKeySet()
{
}

CExtCustomizeSite::CCmdKeySet::CCmdKeySet(
	const CExtCustomizeSite::CCmdKeySet & other
	)
{
	Append( other );
}

CExtCustomizeSite::CCmdKeySet::~CCmdKeySet()
{
	RemoveAll();
}

CExtCustomizeSite::CCmdKeySet &
	CExtCustomizeSite::CCmdKeySet::operator=(
		const CExtCustomizeSite::CCmdKeySet & other
		)
{
	RemoveAll();
	Append( other );
	return (*this);
}

bool CExtCustomizeSite::CCmdKeySet::IsEmpty() const
{
	return ( GetSize() == 0 ) ? true : false;
}

void CExtCustomizeSite::CCmdKeySet::Empty()
{
	RemoveAll();
}

bool CExtCustomizeSite::CCmdKeySet::KeySetup(
	const CExtCustomizeSite::CCmdKeyInfo & _cmdKeyInfo,
	int nPos // = -1 // append
	)
{
int nPosFound = KeyFindPos( _cmdKeyInfo );
	if( nPosFound >= 0 )
		return false;
int nCount = GetSize();
	if( nPos < 0 || nPos > nCount )
		nPos = nCount;
	InsertAt( nPos, _cmdKeyInfo );
	return true;
}

bool CExtCustomizeSite::CCmdKeySet::KeyRemove(
	const CExtCustomizeSite::CCmdKeyInfo & _cmdKeyInfo
	)
{
int nPosFound = KeyFindPos( _cmdKeyInfo );
	if( nPosFound < 0 )
		return false;
	RemoveAt( nPosFound, 1 );
	return true;
}

int CExtCustomizeSite::CCmdKeySet::KeyFindPos(
	const CExtCustomizeSite::CCmdKeyInfo & _cmdKeyInfo
	) const
{
int nCount = GetSize();
	for( int nPos = 0; nPos < nCount; nPos++ )
	{
		const CExtCustomizeSite::CCmdKeyInfo & _cmdKeyInfoToCmp =
			((CCmdKeySet*)this)->ElementAt( nPos );
		if( _cmdKeyInfoToCmp == _cmdKeyInfo )
			return nPos;
	} // for( int nPos = 0; nPos < nCount; nPos++ )
	return -1;
}

void CExtCustomizeSite::CCmdKeySet::Serialize( CArchive & ar )
{
	if( ar.IsStoring() )
	{
		int nCount = GetSize();
		ar << nCount;
		for( int nPos = 0; nPos < nCount; nPos++ )
		{
			CExtCustomizeSite::CCmdKeyInfo & _cmdKeyInfo =
				ElementAt( nPos );
			_cmdKeyInfo.Serialize( ar );
		} // for( int nPos = 0; nPos < nCount; nPos++ )
	} // if( ar.IsStoring() )
	else
	{
		RemoveAll();
		int nCount;
		ar >> nCount;
		for( int nPos = 0; nPos < nCount; nPos++ )
		{
			CExtCustomizeSite::CCmdKeyInfo _cmdKeyInfo;
			_cmdKeyInfo.Serialize( ar );
			Add( _cmdKeyInfo );
		} // for( int nPos = 0; nPos < nCount; nPos++ )
		ASSERT( nCount == GetSize() );
	} // else from if( ar.IsStoring() )
}

/////////////////////////////////////////////////////////////////////////////
// CExtCustomizeSite::CMapCmd2KeySet

CExtCustomizeSite::CMapCmd2KeySet::CMapCmd2KeySet()
{
}

CExtCustomizeSite::CMapCmd2KeySet::~CMapCmd2KeySet()
{
	RemoveAll();
}

void CExtCustomizeSite::CMapCmd2KeySet::CmdKeySetup(
	UINT nCmdID,
	const CExtCustomizeSite::CCmdKeyInfo & _cmdKeyInfo,
	int nPos // = -1 // append
	)
{
CExtCustomizeSite::CCmdKeySet _cmdKeySet;
	if( Lookup( nCmdID, _cmdKeySet ) )
	{
		if( _cmdKeySet.KeySetup( _cmdKeyInfo, nPos ) )
			SetAt( nCmdID, _cmdKeySet );
		return;
	} // if( Lookup(nCmdID,_cmdKeySet) )
	_cmdKeySet.Empty();
	VERIFY( _cmdKeySet.KeySetup( _cmdKeyInfo, nPos ) );
	SetAt( nCmdID, _cmdKeySet );
}

void CExtCustomizeSite::CMapCmd2KeySet::CmdKeyRemove(
	UINT nCmdID,
	const CExtCustomizeSite::CCmdKeyInfo & _cmdKeyInfo
	)
{
CExtCustomizeSite::CCmdKeySet _cmdKeySet;
	if( !Lookup( nCmdID, _cmdKeySet ) )
		return;
	if( !_cmdKeySet.KeyRemove( _cmdKeyInfo ) )
		return;
	if( _cmdKeySet.IsEmpty() )
		CmdRemove( nCmdID );
	else
		SetAt( nCmdID, _cmdKeySet );
}

void CExtCustomizeSite::CMapCmd2KeySet::CmdRemove(
	UINT nCmdID
	)
{
	RemoveKey( nCmdID );
}

void CExtCustomizeSite::CMapCmd2KeySet::Serialize( CArchive & ar )
{
	if( ar.IsStoring() )
	{
		DWORD dwCount = GetCount();
		ar << dwCount;
		for(	POSITION pos = GetStartPosition();
				pos != NULL;
			)
		{
			UINT nCmdID;
			CCmdKeySet _cmdKeySet;
			GetNextAssoc(
				pos,
				nCmdID,
				_cmdKeySet
				);
			ar << nCmdID;
			_cmdKeySet.Serialize( ar );
		}
	} // if( ar.IsStoring() )
	else
	{
		RemoveAll();
		DWORD dwCount;
		ar >> dwCount;
		for( DWORD dwIdx = 0; dwIdx < dwCount; dwIdx++ )
		{
			UINT nCmdID;
			CCmdKeySet _cmdKeySet;
			ar >> nCmdID;
			_cmdKeySet.Serialize( ar );
			SetAt(
				nCmdID,
				_cmdKeySet
				);
		} // for( DWORD dwIdx = 0; dwIdx < dwCount; dwIdx++ )
		ASSERT( dwCount == DWORD(GetCount()) );
	} // else from if( ar.IsStoring() )
}

/////////////////////////////////////////////////////////////////////////////
// CExtCustomizeSite::CMapKey2Cmd

CExtCustomizeSite::CMapKey2Cmd::CMapKey2Cmd()
{
}

CExtCustomizeSite::CMapKey2Cmd::~CMapKey2Cmd()
{
	RemoveAll();
}

void CExtCustomizeSite::CMapKey2Cmd::KeyCommandSet(
	const CExtCustomizeSite::CCmdKeyInfo & _cmdKeyInfo,
	UINT nCmdID
	)
{
	ASSERT( !_cmdKeyInfo.IsEmpty() );
	ASSERT( CExtCmdManager::IsCommand(nCmdID) );
DWORD dwKeyInfo = _cmdKeyInfo;
	SetAt( dwKeyInfo, nCmdID );
}

UINT CExtCustomizeSite::CMapKey2Cmd::KeyCommandGet(
	const CExtCustomizeSite::CCmdKeyInfo & _cmdKeyInfo
	) const
{
	ASSERT( !_cmdKeyInfo.IsEmpty() );
DWORD dwKeyInfo = _cmdKeyInfo;
UINT nCmdID = 0;
	if( !Lookup(dwKeyInfo,nCmdID) )
		return 0;
	ASSERT( CExtCmdManager::IsCommand(nCmdID) );
	return nCmdID;
}

void CExtCustomizeSite::CMapKey2Cmd::KeyCommandRemove(
	const CExtCustomizeSite::CCmdKeyInfo & _cmdKeyInfo
	)
{
DWORD dwKeyInfo = _cmdKeyInfo;
	RemoveKey( dwKeyInfo );
}

POSITION CExtCustomizeSite::CMapKey2Cmd::KeyGetStartPosition() const
{
	return GetStartPosition();
}

void CExtCustomizeSite::CMapKey2Cmd::KeyGetNext(
	POSITION & pos,
	CExtCustomizeSite::CCmdKeyInfo * pCmdKeyInfo,
	LPUINT p_nCmdID // = NULL
	) const
{
	ASSERT( pos != NULL );
DWORD dwKeyInfo = 0;
UINT nCmdID = 0;
	GetNextAssoc( pos, dwKeyInfo, nCmdID );
	ASSERT( CExtCmdManager::IsCommand(nCmdID) );
	if( pCmdKeyInfo != NULL )
	{
		CExtCustomizeSite::CCmdKeyInfo _cmdKeyInfo( dwKeyInfo );
		ASSERT( !_cmdKeyInfo.IsEmpty() );
		*pCmdKeyInfo = _cmdKeyInfo;
	} // if( pCmdKeyInfo != NULL )
	if( p_nCmdID != NULL )
		*p_nCmdID = nCmdID;
}

void CExtCustomizeSite::CMapKey2Cmd::KeyFillCmdSet(
	CExtCustomizeSite::CMapCmd2KeySet & _mapCmd2KeySet
	)
{
	_mapCmd2KeySet.RemoveAll();
	if( GetCount() == 0 )
		return;
	for(	POSITION pos = KeyGetStartPosition();
			pos != NULL;
		)
	{
		CCmdKeyInfo _cmdKeyInfo;
		UINT nCmdID = 0;
		KeyGetNext( pos, &_cmdKeyInfo, &nCmdID );
		_mapCmd2KeySet.CmdKeySetup( nCmdID, _cmdKeyInfo );
	}
}

HACCEL CExtCustomizeSite::CMapKey2Cmd::KeyGenAccelTable()
{
int nCount = GetCount();
	if( nCount == 0 )
		return NULL;
LPACCEL pAccelArray = new ACCEL[ nCount ];
	::memset( pAccelArray, 0, sizeof(ACCEL)*nCount );
int nAccelIdx = 0;
	for(	POSITION pos = KeyGetStartPosition();
			pos != NULL;
			nAccelIdx++
		)
	{
		CCmdKeyInfo _cmdKeyInfo;
		UINT nCmdID = 0;
		KeyGetNext( pos, &_cmdKeyInfo, &nCmdID );
		ACCEL & _dataAccel = pAccelArray[nAccelIdx];
		_cmdKeyInfo.FillAccel( _dataAccel );
		_dataAccel.cmd = (WORD)nCmdID;
	}
HACCEL hAccelTable = ::CreateAcceleratorTable( pAccelArray, nCount );
	delete [] pAccelArray;
	ASSERT( hAccelTable != NULL );
	return hAccelTable;
}

CExtCustomizeSite::CMapKey2Cmd &
	CExtCustomizeSite::CMapKey2Cmd::operator=(
		const CExtCustomizeSite::CMapKey2Cmd & other
		)
{
	RemoveAll();
int nCount = other.GetCount();
	if( nCount == 0 )
		return (*this);
POSITION pos = other.KeyGetStartPosition();
	ASSERT( pos != NULL );
	for( ; pos != NULL; )
	{
		CCmdKeyInfo _cmdKeyInfo;
		UINT nCmdID = 0;
		other.KeyGetNext( pos, &_cmdKeyInfo, &nCmdID );
		KeyCommandSet( _cmdKeyInfo, nCmdID );
	} // for( ; pos != NULL; )
	return (*this);
}

void CExtCustomizeSite::CMapKey2Cmd::Serialize( CArchive & ar )
{
	if( ar.IsStoring() )
	{
		DWORD dwCount = (DWORD)GetCount();
		ar << dwCount;
		if( dwCount == 0 )
			return;
		POSITION pos = KeyGetStartPosition();
		ASSERT( pos != NULL );
		for( ; pos != NULL; )
		{
			CCmdKeyInfo _cmdKeyInfo;
			UINT nCmdID = 0;
			KeyGetNext( pos, &_cmdKeyInfo, &nCmdID );
			ASSERT( !_cmdKeyInfo.IsEmpty() );
			ASSERT( CExtCmdManager::IsCommand(nCmdID) );
			_cmdKeyInfo.Serialize( ar );
			ar << nCmdID;
		} // for( ; pos != NULL; )
	} // if( ar.IsStoring() )
	else
	{
		RemoveAll();
		DWORD dwCount = 0;
		ar >> dwCount;
		if( dwCount == 0 )
			return;
		for( DWORD dwIdx = 0; dwIdx < dwCount; dwIdx++ )
		{
			CCmdKeyInfo _cmdKeyInfo;
			UINT nCmdID = 0;
			_cmdKeyInfo.Serialize( ar );
			ar >> nCmdID;
			ASSERT( !_cmdKeyInfo.IsEmpty() );
			ASSERT( CExtCmdManager::IsCommand(nCmdID) );
			KeyCommandSet( _cmdKeyInfo, nCmdID );
		} // for( DWORD dwIdx = 0; dwIdx < dwCount; dwIdx++ )
		ASSERT( ((DWORD)GetCount()) == dwCount );
	} // else from if( ar.IsStoring() )
}

/////////////////////////////////////////////////////////////////////////////
// CExtCustomizeSite::CCmdMenuInfo

CExtCustomizeSite::CCmdMenuInfo::CCmdMenuInfo(
	CExtCustomizeSite * pSite,
	__EXT_MFC_SAFE_LPCTSTR strName
	)
	: m_pSite( pSite )
	, m_hAccelTable( NULL )
	, m_bAccelTableSyncronized( false )
{
	ASSERT( m_pSite != NULL );
	ASSERT( strName != NULL && _tcslen(strName) > 0 );
	m_strName = strName;
	m_bDefaultMenu = false;
	m_pNodeI = NULL;
	m_pNodeC = NULL;
}

CExtCustomizeSite::CCmdMenuInfo::~CCmdMenuInfo()
{
	m_listRtcFrames.RemoveAll();
	m_listRtcViews.RemoveAll();
	m_listRtcDocs.RemoveAll();
	if( m_pNodeI != NULL )
		delete m_pNodeI;
	if( m_pNodeC != NULL )
		delete m_pNodeC;
	if( m_hAccelTable != NULL )
		::DestroyAcceleratorTable( m_hAccelTable );
	m_mapCmd2KeySet.RemoveAll();
	m_mapKey2Cmd.RemoveAll();
	m_mapInitialKey2Cmd.RemoveAll();
}

bool CExtCustomizeSite::CCmdMenuInfo::InitializeEmpty(
	CWnd * pWndTop,
	bool bDefaultMenu
	)
{
	pWndTop;
	ASSERT( this != NULL );
	ASSERT_VALID( pWndTop );
	ASSERT( pWndTop->GetSafeHwnd() != NULL );
	ASSERT( m_pSite != NULL );
	ASSERT( !m_strName.IsEmpty() );
	if( IsInitialized() )
	{
		ASSERT( FALSE );
		return false;
	}
	ASSERT( m_pNodeI == NULL );
	ASSERT( m_pNodeC == NULL );
	m_pNodeI = m_pSite->OnCmdNodeCreate();
	ASSERT_VALID( m_pNodeI );
	m_pNodeC = m_pNodeI->CloneNode();
	if( m_pNodeC == NULL )
	{
		ASSERT( FALSE );
		delete m_pNodeI;
		return false;
	}
	ASSERT_VALID( m_pNodeC );
	m_bDefaultMenu = bDefaultMenu;
	return true;
}

bool CExtCustomizeSite::CCmdMenuInfo::Initialize(
	CWnd * pWndTop,
	CMenu * pMenu,
	bool bDefaultMenu,
	bool bPopupMenu
	)
{
	ASSERT( this != NULL );
	ASSERT_VALID( pWndTop );
	ASSERT( pWndTop->GetSafeHwnd() != NULL );
	ASSERT( m_pSite != NULL );
	ASSERT( !m_strName.IsEmpty() );
	if(		IsInitialized()
		||	pMenu->GetSafeHmenu() == NULL
		||	(! ::IsMenu(pMenu->GetSafeHmenu()) )
		)
	{
		ASSERT( FALSE );
		return false;
	}
	ASSERT( m_pNodeI == NULL );
	ASSERT( m_pNodeC == NULL );
	m_pNodeI = m_pSite->OnCmdNodeCreate();
	ASSERT_VALID( m_pNodeI );
	if( !m_pNodeI->LoadMenuTree( pWndTop, m_pSite, pMenu, bPopupMenu ) )
	{
		ASSERT( FALSE );
		delete m_pNodeI;
		return false;
	}
	ASSERT_VALID( m_pNodeI );
	m_pNodeC = m_pNodeI->CloneNode();
	if( m_pNodeC == NULL )
	{
		ASSERT( FALSE );
		delete m_pNodeI;
		return false;
	}
	ASSERT_VALID( m_pNodeC );
	m_bDefaultMenu = bDefaultMenu;
	return true;
}

bool CExtCustomizeSite::CCmdMenuInfo::Initialize(
	CWnd * pWndTop,
	__EXT_MFC_SAFE_LPCTSTR strResourceID,
	bool bDefaultMenu,
	bool bPopupMenu
	)
{
	ASSERT( this != NULL );
	ASSERT_VALID( pWndTop );
	ASSERT( pWndTop->GetSafeHwnd() != NULL );
	ASSERT( m_pSite != NULL );
	ASSERT( !m_strName.IsEmpty() );
	if( IsInitialized() )
	{
		ASSERT( FALSE );
		return false;
	}
CMenu _menu;
	if( ! g_ResourceManager->LoadMenu( _menu, strResourceID ) )
	{
		ASSERT( FALSE );
		return false;
	}
	return Initialize( pWndTop, &_menu, bDefaultMenu, bPopupMenu );
}

bool CExtCustomizeSite::CCmdMenuInfo::Initialize(
	CWnd * pWndTop,
	UINT nResourceID,
	bool bDefaultMenu,
	bool bPopupMenu
	)
{
	ASSERT( this != NULL );
	ASSERT_VALID( pWndTop );
	ASSERT( pWndTop->GetSafeHwnd() != NULL );
	ASSERT( m_pSite != NULL );
	ASSERT( !m_strName.IsEmpty() );
	if( IsInitialized() )
	{
		ASSERT( FALSE );
		return false;
	}
CMenu _menu;
	if( ! g_ResourceManager->LoadMenu( _menu, nResourceID ) )
	{
		ASSERT( FALSE );
		return false;
	}
	return Initialize( pWndTop, &_menu, bDefaultMenu, bPopupMenu );
}

bool CExtCustomizeSite::CCmdMenuInfo::IsInitialized() const
{
	ASSERT( this != NULL );
	ASSERT( m_pSite != NULL );
	ASSERT( !m_strName.IsEmpty() );
	if( m_pNodeI == NULL )
		return false;
	ASSERT_VALID( m_pNodeI );
	ASSERT_VALID( m_pNodeC );
	return true;
}

__EXT_MFC_SAFE_LPCTSTR CExtCustomizeSite::CCmdMenuInfo::GetName() const
{
	ASSERT( this != NULL );
	ASSERT( m_pSite != NULL );
	ASSERT( !m_strName.IsEmpty() );
	return m_strName;
}

bool CExtCustomizeSite::CCmdMenuInfo::AddRuntimeClassFrame(
	CRuntimeClass * pRTC
	)
{
	ASSERT( this != NULL );
	ASSERT( m_pSite != NULL );
	ASSERT( !m_strName.IsEmpty() );
	if( (!IsInitialized()) || pRTC == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	m_listRtcFrames.AddTail( pRTC );
	return true;
}

bool CExtCustomizeSite::CCmdMenuInfo::AddRuntimeClassView(
	CRuntimeClass * pRTC
	)
{
	ASSERT( this != NULL );
	ASSERT( m_pSite != NULL );
	ASSERT( !m_strName.IsEmpty() );
	if( (!IsInitialized()) || pRTC == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	m_listRtcViews.AddTail( pRTC );
	return true;
}

bool CExtCustomizeSite::CCmdMenuInfo::AddRuntimeClassDocument(
	CRuntimeClass * pRTC
	)
{
	ASSERT( this != NULL );
	ASSERT( m_pSite != NULL );
	ASSERT( !m_strName.IsEmpty() );
	if( (!IsInitialized()) || pRTC == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	m_listRtcDocs.AddTail( pRTC );
	return true;
}

bool CExtCustomizeSite::CCmdMenuInfo::IsFrame(
	CObject * pObjTest
	)
{
	ASSERT( this != NULL );
	ASSERT( m_pSite != NULL );
	ASSERT( !m_strName.IsEmpty() );
	if( (!IsInitialized()) || pObjTest == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	ASSERT_VALID( pObjTest );
	if( m_listRtcFrames.GetCount() == 0 )
		return false;
	for(	POSITION pos = m_listRtcFrames.GetHeadPosition();
			pos != NULL;
		)
	{
		CRuntimeClass * pRTC = (CRuntimeClass *)
			m_listRtcFrames.GetNext( pos );
		ASSERT( pRTC != NULL );
		if( pObjTest->IsKindOf(pRTC) )
			return true;
	}
	return false;
}

bool CExtCustomizeSite::CCmdMenuInfo::IsView(
	CObject * pObjTest
	)
{
	ASSERT( this != NULL );
	ASSERT( m_pSite != NULL );
	ASSERT( !m_strName.IsEmpty() );
	if( (!IsInitialized()) || pObjTest == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	ASSERT_VALID( pObjTest );
	if( m_listRtcViews.GetCount() == 0 )
		return false;
	for(	POSITION pos = m_listRtcViews.GetHeadPosition();
			pos != NULL;
		)
	{
		CRuntimeClass * pRTC = (CRuntimeClass *)
			m_listRtcViews.GetNext( pos );
		ASSERT( pRTC != NULL );
		if( pObjTest->IsKindOf(pRTC) )
			return true;
	}
	return false;
}

bool CExtCustomizeSite::CCmdMenuInfo::IsDocument(
	CObject * pObjTest
	)
{
	ASSERT( this != NULL );
	ASSERT( m_pSite != NULL );
	ASSERT( !m_strName.IsEmpty() );
	if( (!IsInitialized()) || pObjTest == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	ASSERT_VALID( pObjTest );
	if( m_listRtcDocs.GetCount() == 0 )
		return false;
	for(	POSITION pos = m_listRtcDocs.GetHeadPosition();
			pos != NULL;
		)
	{
		CRuntimeClass * pRTC = (CRuntimeClass *)
			m_listRtcDocs.GetNext( pos );
		ASSERT( pRTC != NULL );
		if( pObjTest->IsKindOf(pRTC) )
			return true;
	}
	return false;
}

CExtCustomizeCmdTreeNode * CExtCustomizeSite::CCmdMenuInfo::GetNode(
	bool bInitial // = false
	)
{
	ASSERT( this != NULL );
	ASSERT_VALID( m_pNodeI );
	ASSERT_VALID( m_pNodeC );
	return bInitial ? m_pNodeI : m_pNodeC;
}

HACCEL CExtCustomizeSite::CCmdMenuInfo::AccelTableGetSafe( bool bReturnClonedCopy )
{
	if( this == NULL )
		return NULL;
	return AccelTableGet( bReturnClonedCopy );
}

HACCEL CExtCustomizeSite::CCmdMenuInfo::AccelTableGet( bool bReturnClonedCopy )
{
	ASSERT( this != NULL );
	AccelTableSync();
	if(		m_hAccelTable == NULL
		||	(!bReturnClonedCopy)
		)
		return m_hAccelTable;
int nCount = ::CopyAcceleratorTable( m_hAccelTable, NULL, 0 );
	if( nCount <= 0 )
		return NULL;
LPACCEL pAccelArray = new ACCEL[ nCount ];
	::memset( pAccelArray, 0, sizeof(ACCEL)*nCount );
int nCountCopied = ::CopyAcceleratorTable( m_hAccelTable, pAccelArray, nCount );
	if( nCountCopied != nCount )
	{
		ASSERT( FALSE );
		delete [] pAccelArray;
		return NULL;
	} // if( nCountCopied != nCount )
HACCEL hAccelTable = ::CreateAcceleratorTable( pAccelArray, nCount );
	if( hAccelTable == NULL )
	{
		ASSERT( FALSE );
		delete [] pAccelArray;
		return NULL;
	}
	delete [] pAccelArray;
	return hAccelTable;
}

bool CExtCustomizeSite::CCmdMenuInfo::AccelTableSet(
	HACCEL hAccelTable,
	bool bUseTableCopy
	)
{
	if(		m_hAccelTable != NULL
		&&	m_hAccelTable == hAccelTable
		)
		return true;
	if( m_hAccelTable != NULL )
	{
		::DestroyAcceleratorTable( m_hAccelTable );
		m_hAccelTable = NULL;
	} // if( m_hAccelTable != NULL )
	m_mapCmd2KeySet.RemoveAll();
	m_mapKey2Cmd.RemoveAll();
	m_mapInitialKey2Cmd.RemoveAll();
	if( hAccelTable == NULL )
		return true;
	m_bAccelTableSyncronized = true;
int nCount = ::CopyAcceleratorTable( hAccelTable, NULL, 0 );
	if( nCount <= 0 )
		return true;
LPACCEL pAccelArray = new ACCEL[ nCount ];
	::memset( pAccelArray, 0, sizeof(ACCEL)*nCount );
int nCountCopied = ::CopyAcceleratorTable( hAccelTable, pAccelArray, nCount );
	if( nCountCopied != nCount )
	{
		ASSERT( FALSE );
		delete [] pAccelArray;
		return false;
	} // if( nCountCopied != nCount )
	if( bUseTableCopy )
	{
		hAccelTable = ::CreateAcceleratorTable( pAccelArray, nCount );
		if( hAccelTable == NULL )
		{
			ASSERT( FALSE );
			delete [] pAccelArray;
			return false;
		}
	} // if( bUseTableCopy )
	m_hAccelTable = hAccelTable;
	for( int nAccelIdx = 0; nAccelIdx < nCount; nAccelIdx++ )
	{
		ACCEL & _dataAccel = pAccelArray[nAccelIdx];
		ASSERT( CExtCmdManager::IsCommand( (UINT)_dataAccel.cmd ) );
		CCmdKeyInfo _cmdKeyInfo( _dataAccel );
		ASSERT( !_cmdKeyInfo.IsEmpty() );
		m_mapKey2Cmd.KeyCommandSet( _cmdKeyInfo, (UINT)_dataAccel.cmd );
	} // for( int nAccelIdx = 0; nAccelIdx < nCount; nAccelIdx++ )
	m_mapKey2Cmd.KeyFillCmdSet( m_mapCmd2KeySet );
	delete [] pAccelArray;
	ASSERT( m_bAccelTableSyncronized );
	m_mapInitialKey2Cmd = m_mapKey2Cmd;
	return true;
}

bool CExtCustomizeSite::CCmdMenuInfo::AccelTableLoad(
	HINSTANCE hInst,
	__EXT_MFC_SAFE_LPCTSTR strResourceID
	)
{
HACCEL hAccelTable =
		::LoadAccelerators(
			hInst,
			strResourceID
			);
	if( hAccelTable == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	return AccelTableSet( hAccelTable, false );
}

bool CExtCustomizeSite::CCmdMenuInfo::AccelTableLoad(
	HINSTANCE hInst,
	UINT nResourceID
	)
{
HACCEL hAccelTable =
		::LoadAccelerators(
			hInst,
			MAKEINTRESOURCE(nResourceID)
			);
	if( hAccelTable == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	return AccelTableSet( hAccelTable, false );
}

void CExtCustomizeSite::CCmdMenuInfo::AccelTableSync()
{
	if( m_bAccelTableSyncronized )
		return;
	m_hAccelTable = m_mapKey2Cmd.KeyGenAccelTable();
	m_bAccelTableSyncronized = true;
}

bool CExtCustomizeSite::CCmdMenuInfo::AccelTableCmdKeySetGet(
	UINT nCmdID,
	CExtCustomizeSite::CCmdKeySet & _cmdKeySet
	) const
{
	return
		m_mapCmd2KeySet.Lookup( nCmdID, _cmdKeySet )
			? true : false;
}

void CExtCustomizeSite::CCmdMenuInfo::AccelTableCmdKeyAssign(
	UINT nCmdID,
	const CExtCustomizeSite::CCmdKeyInfo & _cmdKeyInfo,
	int nPosInList // = -1 // append
	)
{
CExtCustomizeSite::CCmdKeySet _cmdKeySet;
	if(		m_mapCmd2KeySet.Lookup( nCmdID, _cmdKeySet )
		&&	_cmdKeySet.KeyFindPos(_cmdKeyInfo) >= 0
		)
		return;
	AccelTableCmdKeyRemove( _cmdKeyInfo );
	m_mapCmd2KeySet.CmdKeySetup( nCmdID, _cmdKeyInfo, nPosInList );
	m_mapKey2Cmd.KeyCommandSet( _cmdKeyInfo, nCmdID );
	m_bAccelTableSyncronized = false;
}

UINT CExtCustomizeSite::CCmdMenuInfo::AccelTableCmdFromKey(
	const CExtCustomizeSite::CCmdKeyInfo & _cmdKeyInfo
	) const
{
UINT nCmdID = m_mapKey2Cmd.KeyCommandGet( _cmdKeyInfo );
	return nCmdID;
}

UINT CExtCustomizeSite::CCmdMenuInfo::AccelTableCmdKeyRemove(
	const CExtCustomizeSite::CCmdKeyInfo & _cmdKeyInfo
	)
{
UINT nCmdID = m_mapKey2Cmd.KeyCommandGet( _cmdKeyInfo );
	if( nCmdID == 0 )
		return 0;
	m_mapKey2Cmd.KeyCommandRemove( _cmdKeyInfo );
	m_mapCmd2KeySet.CmdKeyRemove( nCmdID, _cmdKeyInfo );
	m_bAccelTableSyncronized = false;
	return nCmdID;
}

int CExtCustomizeSite::CCmdMenuInfo::AccelTableCmdRemove(
	UINT nCmdID
	)
{
CExtCustomizeSite::CCmdKeySet _cmdKeySet;
	if(	!AccelTableCmdKeySetGet( nCmdID, _cmdKeySet ) )
		return 0;
	m_bAccelTableSyncronized = false;
	ASSERT( !_cmdKeySet.IsEmpty() );
int nCount = _cmdKeySet.GetSize();
	for( int nPos = 0; nPos < nCount; nPos++ )
	{
		CExtCustomizeSite::CCmdKeyInfo & _cmdKeyInfo =
			_cmdKeySet.ElementAt( nPos );
		ASSERT( !_cmdKeyInfo.IsEmpty() );
		ASSERT( m_mapKey2Cmd.KeyCommandGet(_cmdKeyInfo) != 0 );
		m_mapKey2Cmd.KeyCommandRemove( _cmdKeyInfo );
	}; // for( int nPos = 0; nPos < nCount; nPos++ )
	m_mapCmd2KeySet.CmdRemove( nCmdID );
	return nCount;
}

void CExtCustomizeSite::CCmdMenuInfo::AccelTableReset()
{
	if( m_hAccelTable != NULL )
	{
		::DestroyAcceleratorTable( m_hAccelTable );
		m_hAccelTable = NULL;
	} // if( m_hAccelTable != NULL )
	m_bAccelTableSyncronized = false;
	m_mapKey2Cmd = m_mapInitialKey2Cmd;
	m_mapKey2Cmd.KeyFillCmdSet( m_mapCmd2KeySet );
}

void CExtCustomizeSite::CCmdMenuInfo::AccelTableSerialize( CArchive & ar )
{
	m_mapKey2Cmd.Serialize( ar );
	if( !ar.IsStoring() )
	{
		if( m_hAccelTable != NULL )
		{
			::DestroyAcceleratorTable( m_hAccelTable );
			m_hAccelTable = NULL;
		} // if( m_hAccelTable != NULL )
		m_bAccelTableSyncronized = false;
		m_mapKey2Cmd.KeyFillCmdSet( m_mapCmd2KeySet );
	}
}

CExtCustomizeSite::CMapCmd2KeySet &
	CExtCustomizeSite::CCmdMenuInfo::GetMapCmd2KeySet()
{
	ASSERT( this != NULL );
	return m_mapCmd2KeySet;
}

const CExtCustomizeSite::CMapCmd2KeySet &
	CExtCustomizeSite::CCmdMenuInfo::GetMapCmd2KeySet() const
{
	ASSERT( this != NULL );
	return m_mapCmd2KeySet;
}

CExtCustomizeSite::CMapKey2Cmd &
	CExtCustomizeSite::CCmdMenuInfo::GetMapKey2Cmd()
{
	ASSERT( this != NULL );
	return m_mapKey2Cmd;
}

const CExtCustomizeSite::CMapKey2Cmd &
	CExtCustomizeSite::CCmdMenuInfo::GetMapKey2Cmd() const
{
	ASSERT( this != NULL );
	return m_mapKey2Cmd;
}

CExtCustomizeSite::CMapKey2Cmd &
	CExtCustomizeSite::CCmdMenuInfo::GetMapInitialKey2Cmd()
{
	ASSERT( this != NULL );
	return m_mapInitialKey2Cmd;
}

const CExtCustomizeSite::CMapKey2Cmd &
	CExtCustomizeSite::CCmdMenuInfo::GetMapInitialKey2Cmd() const
{
	ASSERT( this != NULL );
	return m_mapInitialKey2Cmd;
}

/////////////////////////////////////////////////////////////////////////////
// CExtCustomizeCmdTreeNode

IMPLEMENT_SERIAL( CExtCustomizeCmdTreeNode, CObject, VERSIONABLE_SCHEMA|1 );

CExtCustomizeCmdTreeNode::CExtCustomizeCmdTreeNode(
	UINT nCmdIdBasic, // = 0L
	UINT nCmdIdEffective, // = 0L
	CExtCustomizeCmdTreeNode * pParentNode, // = NULL
	DWORD dwFlags, // = 0L
	__EXT_MFC_SAFE_LPCTSTR strTextInToolbar, // = NULL
	__EXT_MFC_SAFE_LPCTSTR strTextInMenu, // = NULL
	__EXT_MFC_SAFE_LPCTSTR strTextUser, // = NULL
	LPARAM lParam, // = 0L
	CExtCmdIcon * pIconCustomized // = NULL
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	,
	INT nTextFieldWidth, // = 100
	INT nDropDownWidth, // = -2 // (-1) - auto calc, (-2) - same as button area
	INT nDropDownHeightMax // = 250
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	)
	: m_nCmdIdBasic( nCmdIdBasic )
	, m_nCmdIdEffective( nCmdIdEffective )
	, m_pParentNode( pParentNode )
	, m_dwFlags( dwFlags )
	, m_strTextInToolbar( (strTextInToolbar == NULL) ? _T("") : strTextInToolbar )
	, m_strTextInMenu( (strTextInMenu == NULL) ? _T("") : strTextInMenu )
	, m_strTextUser( (strTextUser == NULL) ? _T("") : strTextUser )
	, m_lParam( lParam )
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	, m_nTextFieldWidth( nTextFieldWidth )
	, m_nDropDownWidth( nDropDownWidth )
	, m_nDropDownHeightMax( nDropDownHeightMax )
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
{
	if(		pIconCustomized != NULL
		&&	(! pIconCustomized->IsEmpty() )
		)
		m_iconCustomized = *pIconCustomized;
}

CExtCustomizeCmdTreeNode::CExtCustomizeCmdTreeNode(
	CExtCustomizeCmdTreeNode & other
	)
	: m_nCmdIdBasic( 0L )
	, m_nCmdIdEffective( 0L )
	, m_pParentNode( NULL )
	, m_dwFlags( 0L )
	, m_strTextInToolbar( _T("") )
	, m_strTextInMenu( _T("") )
	, m_strTextUser( _T("") )
	, m_lParam( 0L )
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	, m_nTextFieldWidth( 100 )
	, m_nDropDownWidth( -2 )
	, m_nDropDownHeightMax( 250 )
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
{
	AssignFromOther( other );
}

CExtCustomizeCmdTreeNode::CExtCustomizeCmdTreeNode(
	CExtCmdItem * pCmdItem, // NOT NULL!
	CExtCmdIcon * pIconCustomized // = NULL
	)
	: m_nCmdIdBasic( 0L )
	, m_nCmdIdEffective( 0L )
	, m_pParentNode( NULL )
	, m_dwFlags( 0L )
	, m_strTextInToolbar( _T("") )
	, m_strTextInMenu( _T("") )
	, m_strTextUser( _T("") )
	, m_lParam( 0L )
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	, m_nTextFieldWidth( 100 )
	, m_nDropDownWidth( -2 )
	, m_nDropDownHeightMax( 250 )
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
{
	AssignCmdProps( pCmdItem, pIconCustomized );
}

CExtCustomizeCmdTreeNode::~CExtCustomizeCmdTreeNode()
{
	RemoveAllNodes();
}

CExtCustomizeCmdTreeNode & CExtCustomizeCmdTreeNode::operator =(
	CExtCustomizeCmdTreeNode & other
	)
{
	AssignFromOther( other );
	return (*this);
}

void CExtCustomizeCmdTreeNode::AssignFromOther(
	CExtCustomizeCmdTreeNode & other
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&other) );
	ASSERT_KINDOF( CExtCustomizeCmdTreeNode, this );
	ASSERT_KINDOF( CExtCustomizeCmdTreeNode, (&other) );
	RemoveAllNodes();
	m_nCmdIdBasic = other.m_nCmdIdBasic;
	m_nCmdIdEffective = other.m_nCmdIdEffective;
	m_dwFlags = other.m_dwFlags;
	m_strTextInToolbar = other.m_strTextInToolbar;
	m_strTextInMenu = other.m_strTextInMenu;
	m_strTextUser = other.m_strTextUser;
	m_lParam = other.m_lParam;
	if( other.m_iconCustomized.IsEmpty() )
		m_iconCustomized.Empty();
	else
		m_iconCustomized = other.m_iconCustomized;
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	m_nTextFieldWidth = other.m_nTextFieldWidth;
	m_nDropDownWidth = other.m_nDropDownWidth;
	m_nDropDownHeightMax = other.m_nDropDownHeightMax;

	m_sDefInplaceEditBuffer = other.m_sDefInplaceEditBuffer;
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)

INT nNodeCount = other.GetNodeCount();
	for( INT nPos = 0; nPos < nNodeCount; nPos++ )
	{
		CExtCustomizeCmdTreeNode * pOtherChild =
			other.ElementAt( nPos );
		ASSERT_VALID( pOtherChild );
		CExtCustomizeCmdTreeNode * pClonedNode =
			pOtherChild->CloneNode();
		ASSERT_VALID( pClonedNode );
		InsertNode( NULL, pClonedNode );
	}
	ASSERT( nNodeCount == GetNodeCount() );
}

void CExtCustomizeCmdTreeNode::AssignCmdProps(
	CExtCmdItem * pCmdItem, // NOT NULL!
	CExtCmdIcon * pIconCustomized // = NULL
	)
{
	ASSERT_VALID( this );
	ASSERT_KINDOF( CExtCustomizeCmdTreeNode, this );
	ASSERT( pCmdItem != NULL );
	m_nCmdIdBasic
		= m_nCmdIdEffective
		= pCmdItem->m_nCmdID;
	m_strTextInToolbar =
		pCmdItem->m_sToolbarText.IsEmpty()
			? _T("")
			: pCmdItem->m_sToolbarText
			;
	m_strTextInMenu =
		pCmdItem->m_sMenuText.IsEmpty()
			? _T("")
			: pCmdItem->m_sMenuText
			;
	if(		pIconCustomized != NULL
		&&	(! pIconCustomized->IsEmpty() )
		)
		m_iconCustomized = *pIconCustomized;
	else
		ResetIcon( pCmdItem );

	m_lParam = pCmdItem->m_nLParamUserData;
	
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	m_nTextFieldWidth = pCmdItem->m_nTextFieldWidth;
	m_nDropDownWidth = pCmdItem->m_nDropDownWidth;
	m_nDropDownHeightMax = pCmdItem->m_nDropDownHeightMax;
	if( pCmdItem->StateIsCombo() )
		ModifyFlags( __ECTN_TBB_COMBO|__ECTN_TBB_TEXT_FIELD, 0 );
	else if( pCmdItem->StateIsTextField() )
		ModifyFlags( __ECTN_TBB_TEXT_FIELD, __ECTN_TBB_COMBO );
	else
		ModifyFlags( 0, __ECTN_TBB_TEXT_FIELD|__ECTN_TBB_COMBO|__ECTN_TBB_RESIZABLE );

	if( pCmdItem->StateIsTextFieldNotEditable() )
		ModifyFlags( __ECTN_TBB_TF_NE, 0 );
	else
		ModifyFlags( 0, __ECTN_TBB_TF_NE );

	if(		( (GetFlags() & (__ECTN_TBB_COMBO|__ECTN_TBB_TEXT_FIELD)) != 0 )
		&&	pCmdItem->StateIsResizable()
		)
		ModifyFlags( __ECTN_TBB_RESIZABLE, 0 );
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)

	if( pCmdItem->StateIsColor() )
		ModifyFlags( __ECTN_TBB_COLOR, 0 );
	else
		ModifyFlags( 0, __ECTN_TBB_COLOR );
	if( pCmdItem->StateIsColorBtnDefault() )
		ModifyFlags( __ECTN_TBB_MCB_DEFAULT, 0 );
	else
		ModifyFlags( 0, __ECTN_TBB_MCB_DEFAULT );
	if( pCmdItem->StateIsColorBtnCustom() )
		ModifyFlags( __ECTN_TBB_MCB_CUSTOM, 0 );
	else
		ModifyFlags( 0, __ECTN_TBB_MCB_CUSTOM );

	if( pCmdItem->StateIsSeparatedDD() )
		ModifyFlags( __ECTN_TBB_SEPARATED_DROPDOWN, 0 );
	if( pCmdItem->StateIsAutochangeID() )
		ModifyFlags( __ECTN_TBB_AUTOCHANGE_ID, 0 );
	if( pCmdItem->StateIsNoRotateVL() )
		ModifyFlags( __ECTN_TBB_NO_ROTATE_VL, 0 );
	if( pCmdItem->StateIsCannotHide() )
		ModifyFlags( __ECTN_TBB_CANNOT_HIDE, 0 );
	if( pCmdItem->StateIsCtrlVisibleVertically() )
		ModifyFlags( __ECTN_TBB_CTRL_VIS_VL, 0 );

	if( pCmdItem->StateIsDynamicPopup() )
		ModifyFlags( __ECTN_AUTO_POPUP, 0 );
}

#ifdef _DEBUG
void CExtCustomizeCmdTreeNode::AssertValid() const
{
	CObject::AssertValid();
}
#endif // _DEBUG

void CExtCustomizeCmdTreeNode::Serialize( CArchive & ar )
{
	CObject::Serialize( ar );
	if( ar.IsStoring() )
	{
		ar << m_nCmdIdBasic;
		ar << m_nCmdIdEffective;
		ar << m_dwFlags;
		ar << m_strTextInToolbar;
		ar << m_strTextInMenu;
		ar << m_strTextUser;
		ar << m_lParam;
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
		ar << m_nTextFieldWidth;
		ar << m_nDropDownWidth;
		ar << m_nDropDownHeightMax;
		ar << m_sDefInplaceEditBuffer;
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
		DWORD dwNodeCount = (DWORD)GetNodeCount();
		ar << dwNodeCount;
		for( DWORD dwNodeIdx = 0; dwNodeIdx < dwNodeCount; dwNodeIdx++ )
		{
			CExtCustomizeCmdTreeNode * pNode = ElementAt( (INT)dwNodeIdx );
			ASSERT_VALID( pNode );
			ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNode );
			CRuntimeClass * pRTC = pNode->GetRuntimeClass();
			ASSERT( pRTC != NULL );
			ar.WriteClass( pRTC );
			pNode->Serialize( ar );
		} // for( DWORD dwNodeIdx = 0; dwNodeIdx < dwNodeCount; dwNodeIdx++ )
	} // if( ar.IsStoring() )
	else
	{
		m_pParentNode = NULL;
		ar >> m_nCmdIdBasic;
		ar >> m_nCmdIdEffective;
		ar >> m_dwFlags;
		ar >> m_strTextInToolbar;
		ar >> m_strTextInMenu;
		ar >> m_strTextUser;
		ar >> m_lParam;
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
		ar >> m_nTextFieldWidth;
		ar >> m_nDropDownWidth;
		ar >> m_nDropDownHeightMax;
		ar >> m_sDefInplaceEditBuffer;
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
		RemoveAllNodes();
		DWORD dwNodeCount = 0;
		ar >> dwNodeCount;
		for( DWORD dwNodeIdx = 0; dwNodeIdx < dwNodeCount; dwNodeIdx++ )
		{
			CRuntimeClass * pRTC = ar.ReadClass();
			ASSERT( pRTC != NULL );
			CExtCustomizeCmdTreeNode * pNode = (CExtCustomizeCmdTreeNode*)pRTC->CreateObject();
			ASSERT_VALID( pNode );
			ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNode );
			pNode->Serialize( ar );
			ASSERT_VALID( pNode );
			InsertNode( NULL, pNode );
		} // for( DWORD dwNodeIdx = 0; dwNodeIdx < dwNodeCount; dwNodeIdx++ )
		ASSERT( GetNodeCount() == ((INT)dwNodeCount) );
	} // else from if( ar.IsStoring() )
	m_iconCustomized.Serialize( ar );
}

#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)

INT CExtCustomizeCmdTreeNode::TextFieldWidthGet() const
{
	ASSERT_VALID( this );
	return m_nTextFieldWidth;
}

void CExtCustomizeCmdTreeNode::TextFieldWidthSet( INT nVal )
{
	ASSERT_VALID( this );
	m_nTextFieldWidth = nVal;
}

INT CExtCustomizeCmdTreeNode::DropDownWidthGet() const
{
	ASSERT_VALID( this );
	return m_nDropDownWidth;
}

void CExtCustomizeCmdTreeNode::DropDownWidthSet( INT nVal )
{
	ASSERT_VALID( this );
	m_nDropDownWidth = nVal;
}

INT CExtCustomizeCmdTreeNode::DropDownHeightMaxGet() const
{
	ASSERT_VALID( this );
	return m_nDropDownHeightMax;
}

void CExtCustomizeCmdTreeNode::DropDownHeightMaxSet( INT nVal )
{
	ASSERT_VALID( this );
	m_nDropDownHeightMax = nVal;
}

#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)

UINT CExtCustomizeCmdTreeNode::GetCmdID(
	bool bEffective // = true
	) const
{
	ASSERT_VALID( this );
	return bEffective ? m_nCmdIdEffective : m_nCmdIdBasic;
}

UINT CExtCustomizeCmdTreeNode::SetCmdID(
	UINT nCmdID,
	bool bEffectiveOnly // = false
	)
{
	ASSERT_VALID( this );
int nCmdID_Old = m_nCmdIdEffective;
	m_nCmdIdEffective = nCmdID;
	if( !bEffectiveOnly )
		m_nCmdIdBasic = nCmdID;
	return nCmdID_Old;
}

LONG CExtCustomizeCmdTreeNode::GetIndent() const
{
	ASSERT_VALID( this );
const CExtCustomizeCmdTreeNode * pParentNode = 
		GetParentNode();
	for( LONG nIndent = 0L; pParentNode != NULL; )
	{
		ASSERT_VALID( pParentNode );
		nIndent ++;
		pParentNode = pParentNode->GetParentNode();
	} // for( LONG nIndent = 0L; pParentNode != NULL; )
	return nIndent;
}

DWORD CExtCustomizeCmdTreeNode::GetFlags() const
{
	ASSERT_VALID( this );
	return m_dwFlags;
}

DWORD CExtCustomizeCmdTreeNode::ModifyFlags(
	DWORD dwAdd,
	DWORD dwRemove // = 0
	)
{
	ASSERT_VALID( this );
DWORD dwOldFlags = m_dwFlags;
	m_dwFlags |= dwAdd;
	m_dwFlags &= ~dwRemove;
	return dwOldFlags;
}

bool CExtCustomizeCmdTreeNode::CanChangeDisplayStyle(
	CExtCmdItem * pCmdItem
	)
{
	ASSERT_VALID( this );
	ASSERT( pCmdItem != NULL );
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	if( ( GetFlags() & (__ECTN_TBB_TEXT_FIELD|__ECTN_TBB_COMBO) ) != 0 )
		return false;
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	if( ( GetFlags() & __ECTN_TBB_COLOR ) != 0 )
		return false;
	if( GetIconPtrInToolbar( pCmdItem, true ) == NULL )
		return false;
	return true;
}

CExtCmdIcon * CExtCustomizeCmdTreeNode::CurrentIconGetPtr(
	bool bAllowEmptyIcon,
	CExtCmdItem * pCmdItem
	)
{
	ASSERT_VALID( this );
	ASSERT( pCmdItem != NULL );
CExtCmdIcon * pIcon = GetIconPtrInToolbar( pCmdItem, true );
	if(		pIcon == NULL
		||	pCmdItem->StateIsPersistentIcon()
		)
		return pIcon;
	if(		(!bAllowEmptyIcon)
		&&	m_iconCustomized.IsEmpty()
		)
		return NULL;
	return (&m_iconCustomized);
}

CExtCmdIcon * CExtCustomizeCmdTreeNode::GetIconPtrInToolbar(
	CExtCmdItem * pCmdItem,
	bool bForceBasic // = false
	)
{
	ASSERT_VALID( this );
	if(		bForceBasic
		&&	pCmdItem != NULL
		&&	pCmdItem->m_nIconIdx >= 0
		&&	pCmdItem->m_pProfile != NULL
		)
	{
		ASSERT( pCmdItem->m_nIconIdx < pCmdItem->m_pProfile->m_icons.GetSize() );
		CExtCmdIcon * pIcon =
			pCmdItem->m_pProfile->m_icons[pCmdItem->m_nIconIdx];
		ASSERT( pIcon != NULL );
		ASSERT( !pIcon->IsEmpty() );
		return pIcon;
	}
DWORD dwDisplayMask = GetFlags() & __ECTN_DISPLAY_MASK;
	if(		dwDisplayMask == __ECTN_DISPLAY_TEXTONLY_ALWAYS
		&&	GetTextInToolbar( pCmdItem ) != NULL
		)
		return NULL;
	if(		pCmdItem != NULL
		&&	pCmdItem->StateIsPersistentIcon()
		)
		bForceBasic = true;
	if(		(! m_iconCustomized.IsEmpty() )
		&&	(! bForceBasic)
		)
		return (&m_iconCustomized);
	if(		pCmdItem == NULL
		||	pCmdItem->m_nIconIdx < 0
		||	pCmdItem->m_pProfile == NULL
		)
		return NULL;
	ASSERT( pCmdItem->m_nIconIdx < pCmdItem->m_pProfile->m_icons.GetSize() );
CExtCmdIcon * pIcon =
		pCmdItem->m_pProfile->m_icons[pCmdItem->m_nIconIdx];
	ASSERT( pIcon != NULL );
	ASSERT( !pIcon->IsEmpty() );
	return pIcon;
}

CExtCmdIcon * CExtCustomizeCmdTreeNode::GetIconPtrInMenu(
	CExtCmdItem * pCmdItem,
	bool bForceBasic // = false
	)
{
	ASSERT_VALID( this );
	if(		bForceBasic
		&&	pCmdItem != NULL
		&&	pCmdItem->m_nIconIdx >= 0
		&&	pCmdItem->m_pProfile != NULL
		)
	{
		ASSERT( pCmdItem->m_nIconIdx < pCmdItem->m_pProfile->m_icons.GetSize() );
		CExtCmdIcon * pIcon =
			pCmdItem->m_pProfile->m_icons[pCmdItem->m_nIconIdx];
		ASSERT( pIcon != NULL );
		ASSERT( !pIcon->IsEmpty() );
		return pIcon;
	}
DWORD dwDisplayMask = GetFlags() & __ECTN_DISPLAY_MASK;
	if(		(	dwDisplayMask == __ECTN_DISPLAY_TEXTONLY_IN_MENU
			||	dwDisplayMask == __ECTN_DISPLAY_TEXTONLY_ALWAYS
			)
		&&	GetTextInMenu( pCmdItem ) != NULL
		)
		return NULL;
	if(		pCmdItem != NULL
		&&	pCmdItem->StateIsPersistentIcon()
		)
		bForceBasic = true;
	if(		(! m_iconCustomized.IsEmpty() )
		&&	(! bForceBasic)
		)
		return (&m_iconCustomized);
	if(		pCmdItem == NULL
		||	pCmdItem->m_nIconIdx < 0
		||	pCmdItem->m_pProfile == NULL
		)
		return NULL;
	ASSERT( pCmdItem->m_nIconIdx < pCmdItem->m_pProfile->m_icons.GetSize() );
CExtCmdIcon * pIcon =
		pCmdItem->m_pProfile->m_icons[pCmdItem->m_nIconIdx];
	ASSERT( pIcon != NULL );
	ASSERT( !pIcon->IsEmpty() );
	return pIcon;
}

void CExtCustomizeCmdTreeNode::ResetIcon(
	CExtCmdItem * pCmdItem
	)
{
	ASSERT_VALID( this );
//CExtCmdIcon * pBasicIcon = GetIconPtrInToolbar( pCmdItem, true );
//	if(		pBasicIcon == NULL
//		||	pBasicIcon->IsEmpty()
//		)
//		m_iconCustomized.Empty();
//	else
//		m_iconCustomized = *pBasicIcon;
	pCmdItem;
	m_iconCustomized.Empty();
}

__EXT_MFC_SAFE_LPCTSTR CExtCustomizeCmdTreeNode::GetTextInToolbar(
	CExtCmdItem * pCmdItem,
	bool bDoNotTestStyles // = false
	)
{
	ASSERT_VALID( this );
	if( !bDoNotTestStyles )
	{
		DWORD dwDisplayMask = GetFlags() & __ECTN_DISPLAY_MASK;
		if(		(	dwDisplayMask == __ECTN_DISPLAY_DEFAULT_STYLE
				||	dwDisplayMask == __ECTN_DISPLAY_TEXTONLY_IN_MENU
				)
			&&	GetIconPtrInToolbar( pCmdItem ) != NULL
			)
			return _T("");
	} // if( !bDoNotTestStyles )
	return
		m_strTextInToolbar.IsEmpty()
			? (	m_strTextInMenu.IsEmpty()
					? (	(pCmdItem == NULL)
							? _T("")
							: (	(!pCmdItem->m_sToolbarText.IsEmpty())
								? pCmdItem->m_sToolbarText
								: (	(!pCmdItem->m_sMenuText.IsEmpty())
										? pCmdItem->m_sMenuText
										: _T("")
										)
								)
						)
					: m_strTextInMenu
				)
			: m_strTextInToolbar
			;
}
void CExtCustomizeCmdTreeNode::SetTextInToolbar(
	__EXT_MFC_SAFE_LPCTSTR strText // = NULL
	)
{
	ASSERT_VALID( this );
	m_strTextInToolbar.Empty();
	if( strText == NULL )
		return;
	m_strTextInToolbar = strText;
}

__EXT_MFC_SAFE_LPCTSTR CExtCustomizeCmdTreeNode::GetTextInMenu(
	CExtCmdItem * pCmdItem,
	bool bDoNotTestStyles // = false
	)
{
	ASSERT_VALID( this );
	bDoNotTestStyles;
	return
		m_strTextInMenu.IsEmpty()
			? (	m_strTextInToolbar.IsEmpty()
					? (	(pCmdItem == NULL)
							? _T("")
							: (	(!pCmdItem->m_sMenuText.IsEmpty())
								? pCmdItem->m_sMenuText
								: (	(!pCmdItem->m_sToolbarText.IsEmpty())
										? pCmdItem->m_sToolbarText
										: _T("")
										)
								)
						)
					: m_strTextInToolbar
				)
			: m_strTextInMenu
			;
}
void CExtCustomizeCmdTreeNode::SetTextInMenu(
	__EXT_MFC_SAFE_LPCTSTR strText // = NULL
	)
{
	ASSERT_VALID( this );
	m_strTextInMenu.Empty();
	if( strText == NULL )
		return;
	m_strTextInMenu = strText;
}

__EXT_MFC_SAFE_LPCTSTR CExtCustomizeCmdTreeNode::GetTextUser() const
{
	ASSERT_VALID( this );
	return m_strTextUser.IsEmpty() ? _T("") : m_strTextUser;
}
void CExtCustomizeCmdTreeNode::SetTextUser(
	__EXT_MFC_SAFE_LPCTSTR strText // = NULL
	)
{
	ASSERT_VALID( this );
	m_strTextUser.Empty();
	if( strText == NULL )
		return;
	m_strTextUser = strText;
}

LPARAM CExtCustomizeCmdTreeNode::GetLParam() const
{
	ASSERT_VALID( this );
	return m_lParam;
}

LPARAM CExtCustomizeCmdTreeNode::SetLParam(
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
LPARAM lParamOld = m_lParam;
	m_lParam = lParam;
	return lParamOld;
}

CExtCustomizeCmdTreeNode * CExtCustomizeCmdTreeNode::GetParentNode()
{
	ASSERT_VALID( this );
	return m_pParentNode;
}

const CExtCustomizeCmdTreeNode * CExtCustomizeCmdTreeNode::GetParentNode() const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtCustomizeCmdTreeNode * > (this) )
			-> GetParentNode();
}

CExtCustomizeCmdTreeNode * CExtCustomizeCmdTreeNode::SetParentNode(
	CExtCustomizeCmdTreeNode * pNewParentNode
	)
{
	ASSERT_VALID( this );
#ifdef _DEBUG
	if( pNewParentNode != NULL )
	{
		ASSERT_VALID( pNewParentNode );
	}
#endif // _DEBUG
CExtCustomizeCmdTreeNode * pOldParentNode = m_pParentNode;
	m_pParentNode = pNewParentNode;
	return pOldParentNode;
}

INT CExtCustomizeCmdTreeNode::InsertClonedChildsNodesFrom(
	CExtCustomizeSite * pSite,
	CExtCustomizeCmdTreeNode * pNode
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pNode );
	RemoveAllNodes();
INT nNodeCount = pNode->m_arrChilds.GetSize();
	if( nNodeCount == 0 )
		return 0;
	for( INT nPos = 0; nPos < nNodeCount; nPos++ )
	{
		CExtCustomizeCmdTreeNode * pOtherChild =
			pNode->ElementAt( nPos );
		ASSERT_VALID( pOtherChild );
		CExtCustomizeCmdTreeNode * pClonedNode =
			pOtherChild->CloneNode( pSite, true );
		ASSERT_VALID( pClonedNode );
		InsertNode( pSite, pClonedNode );
	}
	return nNodeCount;
}

CExtCustomizeCmdTreeNode * CExtCustomizeCmdTreeNode::CloneNode(
	CExtCustomizeSite * pSite,
	bool bCloneChilds // = true
	)
{
	ASSERT_VALID( this );
CExtCustomizeCmdTreeNode * pClonedNode =
		( pSite != NULL )
			? pSite->OnCmdNodeCreate(
				m_nCmdIdBasic,
				m_nCmdIdEffective,
				NULL,
				m_dwFlags,
				m_strTextInToolbar,
				m_strTextInMenu,
				m_strTextUser,
				m_lParam,
				&m_iconCustomized
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
				,
				m_nTextFieldWidth,
				m_nDropDownWidth,
				m_nDropDownHeightMax
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
				)
			: new CExtCustomizeCmdTreeNode(
				m_nCmdIdBasic,
				m_nCmdIdEffective,
				NULL,
				m_dwFlags,
				m_strTextInToolbar,
				m_strTextInMenu,
				m_strTextUser,
				m_lParam,
				&m_iconCustomized
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
				,
				m_nTextFieldWidth,
				m_nDropDownWidth,
				m_nDropDownHeightMax
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
				);
	ASSERT_VALID( pClonedNode );
//	if( pSite != NULL )
//	{
//		CWnd * pWndTop = pSite->GetCustomizeTopWnd();
//		if(		pWndTop->GetSafeHwnd() != NULL
//			&&	(pClonedNode->GetFlags() & __ECTN_TBB_AUTOCHANGE_ID) != 0
//			)
//		{
//			ASSERT_VALID( pWndTop );
//			CExtCmdItem * pCmdItem =
//				g_CmdManager->CmdGetPtr(
//					g_CmdManager->ProfileNameFromWnd( pWndTop->GetSafeHwnd() ),
//					pClonedNode->GetCmdID()
//					);
//			ASSERT( pCmdItem != NULL );
//			pClonedNode->SetTextInMenu( (LPCTSTR)pCmdItem->m_sMenuText );
//			pClonedNode->SetTextInToolbar( (LPCTSTR)pCmdItem->m_sToolbarText );
//		} // if( pWndTop->GetSafeHwnd() != NULL ...
//	} // if( pSite != NULL )
	if( bCloneChilds )
		pClonedNode->InsertClonedChildsNodesFrom( pSite, this );
	return pClonedNode;
}

// complete clone, serialize-based method, no customize site notifications
CExtCustomizeCmdTreeNode * CExtCustomizeCmdTreeNode::CloneNode()
{
	ASSERT_VALID( this );
CExtCustomizeCmdTreeNode * pCloneNode = NULL;
CMemFile _file;
	try
	{
		CRuntimeClass * pRTC = GetRuntimeClass();
		ASSERT( pRTC != NULL );
		CArchive _arOut( &_file,  CArchive::store );
		Serialize( _arOut );
		_arOut.Close();
		_file.Seek( 0, CFile::begin );
		pCloneNode = (CExtCustomizeCmdTreeNode*)pRTC->CreateObject();
		ASSERT_VALID( pCloneNode );
		ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pCloneNode );
		CArchive _arIn( &_file,  CArchive::load );
		pCloneNode->Serialize( _arIn );
		ASSERT_VALID( pCloneNode );
		_arIn.Close();
	} // try
	catch( CException * pXept )
	{
		ASSERT( FALSE );
		pXept->Delete();
		return NULL;
	} // catch( CException * pXept )
	catch( ... )
	{
		ASSERT( FALSE );
		return NULL;
	} // catch( ... )
	ASSERT_VALID( pCloneNode );
	return pCloneNode;
}

bool CExtCustomizeCmdTreeNode::LoadMenuTree(
	CWnd * pWndTop,
	CExtCustomizeSite * pSite,
	CMenu * pMenu, // all the menu commands should be registered in the command manager
	bool bPopupMenu // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( pSite != NULL );
	ASSERT_VALID( pWndTop );
	ASSERT( pWndTop->GetSafeHwnd() != NULL );
	if(		pMenu->GetSafeHmenu() == NULL
		||	(! ::IsMenu(pMenu->GetSafeHmenu()) )
		)
	{
		ASSERT( FALSE );
		return false;
	}
	if( bPopupMenu )
	{
		pMenu = pMenu->GetSubMenu( 0 );
		if(		pMenu == NULL
			|| (! ::IsMenu(pMenu->GetSafeHmenu()) )
			)
		{
			ASSERT( FALSE );
			return false;
		}
	}
__EXT_MFC_SAFE_LPCTSTR strProfileName =
		g_CmdManager->ProfileNameFromWnd(
			pWndTop->GetSafeHwnd()
			);
	if( strProfileName == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	RemoveAllNodes();
INT	nMenuItemCount = pMenu->GetMenuItemCount();
bool bPrevSeparator = false;
	for( int nItemIndex = 0; nItemIndex < nMenuItemCount; nItemIndex++ )
	{
		MENUITEMINFO mii;
		::memset( &mii, 0, sizeof(MENUITEMINFO) );
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_CHECKMARKS|MIIM_DATA|MIIM_ID
			|MIIM_STATE|MIIM_SUBMENU|MIIM_TYPE;
		mii.cch = __MAX_UI_ITEM_TEXT;
		CExtSafeString _sPopupText;
		mii.dwTypeData = _sPopupText.GetBuffer(__MAX_UI_ITEM_TEXT);
		ASSERT( mii.dwTypeData != NULL );
		if( mii.dwTypeData == NULL )
		{
			ASSERT( FALSE );
			return false;
		}
		if( !pMenu->GetMenuItemInfo(
				nItemIndex,
				&mii,
				TRUE
				)
			)
		{
			_sPopupText.ReleaseBuffer();
			ASSERT( FALSE );
			return false;
		}
		_sPopupText.ReleaseBuffer();
		if( (mii.fType&MFT_SEPARATOR) != 0 )
		{
			if( nItemIndex > 0 )
				bPrevSeparator = true;
			continue;
		}
		if( ! _sPopupText.IsEmpty() )
		{
			int nPos = _sPopupText.Find(_T('\t'));
			if( nPos >= 0 )
			{
				CExtSafeString sLeft = _sPopupText.Left( nPos );
				_sPopupText = sLeft;
			} // if( nPos >= 0 )
		} // if( ! _sPopupText.IsEmpty() )
		CExtCustomizeCmdTreeNode * pNode = NULL;
		if( mii.hSubMenu != NULL )
		{
			CExtCmdItem * pCmdItem =
				g_CmdManager->CmdAllocPtr( strProfileName );
			ASSERT( pCmdItem != NULL );
			pCmdItem->StateSetBasic();
			pCmdItem->StateSetDynamicPopup();
			pCmdItem->m_sMenuText
				= pCmdItem->m_sToolbarText
				= _sPopupText;
			pNode =
				pSite->OnCmdNodeCreate(
					pCmdItem->m_nCmdID,
					pCmdItem->m_nCmdID,
					NULL,
					0L,
					_sPopupText,
					_sPopupText
					);
			ASSERT_VALID( pNode );
			pNode->ModifyFlags( __ECTN_DYNAMIC_POPUP );
			InsertNode( pSite, pNode );
			CMenu * pSubMenu = CMenu::FromHandle( mii.hSubMenu );
			ASSERT( pSubMenu != NULL );
			if( !pNode->LoadMenuTree( pWndTop, pSite, pSubMenu, false ) )
			{
				ASSERT( FALSE );
				return false;
			}
		} // if( mii.hSubMenu != NULL )
		else
		{
			if( !CExtCmdManager::IsCommand(mii.wID) )
			{
				ASSERT( FALSE );
				return false;
			}
			CExtCmdItem * pCmdItem =
				g_CmdManager->CmdGetPtr(
					g_CmdManager->ProfileNameFromWnd(
						pWndTop->GetSafeHwnd()
						),
					mii.wID
					);
			ASSERT( pCmdItem != NULL );
			CExtSafeString _strTextInToolbar, _strTextInMenu;
			if( !_sPopupText.IsEmpty() )
			{
				_strTextInMenu = _sPopupText;
				//_strTextInToolbar = _sPopupText;
			} // if( !_sPopupText.IsEmpty() )
			else
			{
				_strTextInToolbar = pCmdItem->m_sToolbarText;
				_strTextInMenu = pCmdItem->m_sMenuText;
			} // else from if( !_sPopupText.IsEmpty() )
			pNode =
				pSite->OnCmdNodeCreate(
					mii.wID,
					mii.wID,
					NULL,
					0L,
					_strTextInToolbar,
					_strTextInMenu
					);
			pNode->AssignCmdProps( pCmdItem );
			ASSERT_VALID( pNode );
			InsertNode( pSite, pNode );
		} // else from if( mii.hSubMenu != NULL )

		if( bPrevSeparator )
		{
			ASSERT( nItemIndex > 0 );
			bPrevSeparator = false;
			pNode->ModifyFlags( __ECTN_GROUP_START );
		} // if( bPrevSeparator )

	} // for( int nItemIndex = 0; nItemIndex < nMenuItemCount; nItemIndex++ )

	return true;
}

bool CExtCustomizeCmdTreeNode::LoadMenuTree(
	CWnd * pWndTop,
	CExtCustomizeSite * pSite,
	UINT nResourceID,
	bool bPopupMenu // = true
	)
{
CMenu _menu;
	if( ! g_ResourceManager->LoadMenu( _menu, nResourceID ) )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	return LoadMenuTree( pWndTop, pSite, &_menu, bPopupMenu );
}

bool CExtCustomizeCmdTreeNode::LoadMenuTree(
	CWnd * pWndTop,
	CExtCustomizeSite * pSite,
	__EXT_MFC_SAFE_LPCTSTR sResourceID,
	bool bPopupMenu // = true
	)
{
CMenu _menu;
	if( ! g_ResourceManager->LoadMenu( _menu, sResourceID ) )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	return LoadMenuTree( pWndTop, pSite, &_menu, bPopupMenu );
}

bool CExtCustomizeCmdTreeNode::IsPopupNode() const
{
	ASSERT_VALID( this );
DWORD dwFlags = GetFlags();
	if( (dwFlags&__ECTN_AUTO_POPUP) != 0 )
		return true;
	if( (dwFlags&__ECTN_TBB_COLOR) != 0 )
		return true;
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	if( (dwFlags&__ECTN_TBB_COMBO) != 0 )
		return true;
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	if( GetNodeCount() > 0 )
		return true;
	return false;
}

void CExtCustomizeCmdTreeNode::InsertBarNode(
	CExtCustomizeSite * pSite,
	CExtCustomizeCmdTreeNode * pNode,
	INT nPos // = -1 // append
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pNode );
	ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNode );
	ASSERT( CExtCmdManager::IsCommand(pNode->GetCmdID(false)) );
	ASSERT( CExtCmdManager::IsCommand(pNode->GetCmdID(true)) );
	ASSERT( SearchNode(pNode->GetCmdID(false)) < 0 );
	InsertNode( pSite, pNode, nPos );
}

void CExtCustomizeCmdTreeNode::InsertNode(
	CExtCustomizeSite * pSite,
	CExtCustomizeCmdTreeNode * pNode,
	INT nPos // = -1 // append
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pNode );
	ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNode );
	ASSERT(
			CExtCmdManager::IsCommand(pNode->GetCmdID(false))
		||	pNode->GetCmdID(false) == UINT(-1)
		||	pNode->GetCmdID(false) == UINT(0)
		);
	ASSERT(
			CExtCmdManager::IsCommand(pNode->GetCmdID(true))
		||	pNode->GetCmdID(true) == UINT(-1)
		||	pNode->GetCmdID(true) == UINT(0)
		);
	ASSERT( pNode->GetParentNode() == NULL );
INT nNodeCount = m_arrChilds.GetSize();
	if( nPos < 0 || nPos > nNodeCount )
		nPos = nNodeCount;
	m_arrChilds.InsertAt( nPos, pNode );
	pNode->SetParentNode( this );
	ModifyFlags( __ECTN_AUTO_POPUP );
	if( pSite != NULL )
		pSite->OnCmdNodeInserted( this );
}

INT CExtCustomizeCmdTreeNode::RemoveNodes( // returns removed count
	INT nPos, // = 0
	INT nCount // = 1 // -1 - remove all
	)
{
	ASSERT_VALID( this );
	ASSERT( nPos >= 0 );
	if( nCount == 0 )
		return 0;
INT nNodeCount = m_arrChilds.GetSize();
	if( nNodeCount == 0 )
		return 0;
	if( nPos >= nNodeCount )
		return 0;
	if( nCount < 0 )
		nCount = nNodeCount;
	if( (nPos + nCount) > nNodeCount )
		nCount = nNodeCount - nPos;
INT nStart = nPos, nEnd = nPos + nCount - 1;
	for( ; nStart <= nEnd; nStart++ )
	{
		CExtCustomizeCmdTreeNode * pNode =
			(CExtCustomizeCmdTreeNode *)
				m_arrChilds.ElementAt( nStart );
		ASSERT_VALID( pNode );
		ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNode );
		//ASSERT( (pNode->GetFlags() & __ECTN_DYNAMIC_POPUP) == 0 );
		delete pNode;
	} // for( ; nStart <= nEnd; nStart++ )
	m_arrChilds.RemoveAt( nPos, nCount );
	return nCount;
}

void CExtCustomizeCmdTreeNode::RemoveSelf(
	CExtCmdItem * pCmdItem,
	bool bAutoSyncNextSeparator // = false
	)
{
	ASSERT_VALID( this );
	ASSERT( pCmdItem != NULL );
	//ASSERT( pCmdItem->m_pProfile != NULL );
	pCmdItem;
#ifdef _DEBUG
	if( GetFlags() & __ECTN_DYNAMIC_POPUP )
	{
		ASSERT( pCmdItem->StateIsDynamicPopup() );
	}
	else
	{
		ASSERT( !pCmdItem->StateIsDynamicPopup() );
	}
#endif // _DEBUG
CExtCustomizeCmdTreeNode * pParent = GetParentNode();
	ASSERT_VALID( pParent );
int nParentNodeCount = pParent->GetNodeCount();
	ASSERT( nParentNodeCount > 0 );
INT nOwnIdx = GetOwnIndex();
	ASSERT( 0 <= nOwnIdx && nOwnIdx < pParent->GetNodeCount() );
	pParent->RemoveNodes( nOwnIdx, 1 );
	if(		bAutoSyncNextSeparator
		&&	nParentNodeCount > (nOwnIdx + 1)
		)
	{
		CExtCustomizeCmdTreeNode * pNodeToSync =
			pParent->ElementAt( nOwnIdx );
		ASSERT_VALID( pNodeToSync );
		ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNodeToSync );
		pNodeToSync->ModifyFlags( 0, __ECTN_GROUP_START );
	} // if( bAutoSyncNextSeparator ...
}

void CExtCustomizeCmdTreeNode::Swap(
	INT nIndex1,
	INT nIndex2,
	DWORD dwFlagsToLeave // = 0
	)
{
	ASSERT( 0 <= nIndex1 && nIndex1 < m_arrChilds.GetSize() );
	ASSERT( 0 <= nIndex2 && nIndex2 < m_arrChilds.GetSize() );
	if( nIndex1 == nIndex2 )
		return;
CExtCustomizeCmdTreeNode * ptr1 = (CExtCustomizeCmdTreeNode *)
		m_arrChilds.GetAt( nIndex1 );
	ASSERT_VALID( ptr1 );
CExtCustomizeCmdTreeNode * ptr2 = (CExtCustomizeCmdTreeNode *)
		m_arrChilds.GetAt( nIndex2 );
	ASSERT_VALID( ptr2 );
	m_arrChilds.SetAt( nIndex1, ptr2 );
	m_arrChilds.SetAt( nIndex2, ptr1 );
	if( dwFlagsToLeave != 0 )
	{
		DWORD dwFlags1 = ptr1->GetFlags() & dwFlagsToLeave;
		DWORD dwFlags2 = ptr2->GetFlags() & dwFlagsToLeave;
		ptr1->ModifyFlags( 0, dwFlagsToLeave );
		ptr2->ModifyFlags( 0, dwFlagsToLeave );
		ptr1->ModifyFlags( dwFlags2, 0 );
		ptr2->ModifyFlags( dwFlags1, 0 );
	} // if( dwFlagsToLeave != 0 )
}

INT CExtCustomizeCmdTreeNode::GetNodeCount() const
{
	ASSERT_VALID( this );
	return m_arrChilds.GetSize();
}

CExtCustomizeCmdTreeNode * CExtCustomizeCmdTreeNode::ElementAt(
	INT nPos
	)
{
	ASSERT_VALID( this );
	ASSERT( 0 <= nPos && nPos < m_arrChilds.GetSize() );
CExtCustomizeCmdTreeNode * pNode =
		(CExtCustomizeCmdTreeNode *)
			m_arrChilds.ElementAt( nPos );
	ASSERT_VALID( pNode );
	ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNode );
	return pNode;
}

INT CExtCustomizeCmdTreeNode::GetNodeIndex(
	CExtCustomizeCmdTreeNode * pNode
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pNode );
	ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNode );
	if( pNode->GetParentNode() != this )
		return -1;
INT nNodeCount = m_arrChilds.GetSize();
	if( nNodeCount == 0 )
		return -1;
	for( INT nPos = 0; nPos < nNodeCount; nPos++ )
	{
		CExtCustomizeCmdTreeNode * pNodeCmp =
			(CExtCustomizeCmdTreeNode *)
				m_arrChilds.ElementAt( nPos );
		ASSERT_VALID( pNodeCmp );
		ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNodeCmp );
		if( pNodeCmp == pNode )
			return nPos;
	} // for( INT nPos = 0; nPos < nNodeCount; nPos++ )
	return -1;
}

INT CExtCustomizeCmdTreeNode::RemoveCommand(
	UINT nCmdID,
	bool bUseEffectiveCmdID, // = false
	bool bDeep // = true
	)
{
	ASSERT_VALID( this );
	return
		RemoveCommandRange(
			nCmdID,
			nCmdID,
			bUseEffectiveCmdID,
			bDeep
			);
}

INT CExtCustomizeCmdTreeNode::RemoveCommandRange(
	UINT nMinCmdID,
	UINT nMaxCmdID,
	bool bUseEffectiveCmdID, // = false
	bool bDeep // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( nMinCmdID <= nMaxCmdID );
INT nNodeCount = m_arrChilds.GetSize();
	if( nNodeCount == 0 )
		return 0;
INT nCountRemoved = 0;
	for( INT nPos = 0; nPos < nNodeCount; )
	{
		CExtCustomizeCmdTreeNode * pNode =
			(CExtCustomizeCmdTreeNode *)
				m_arrChilds.ElementAt( nPos );
		ASSERT_VALID( pNode );
		ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNode );
		UINT nCmdID = pNode->GetCmdID(bUseEffectiveCmdID);
		if( nMinCmdID <= nCmdID && nCmdID <= nMaxCmdID )
		{
			RemoveNodes( nPos, 1 );
			nCountRemoved ++;
			nNodeCount --;
			continue;
		} // if( nMinCmdID <= nCmdID && nCmdID <= nMaxCmdID )
		if( bDeep )
			nCountRemoved +=
				pNode->RemoveCommandRange(
					nMinCmdID,
					nMaxCmdID,
					bUseEffectiveCmdID,
					bDeep
					);
		nPos ++;
	} // for( INT nPos = 0; nPos < nNodeCount; )
	return nCountRemoved;
}

INT CExtCustomizeCmdTreeNode::SearchNode(
	UINT nCmdID,
	INT nPosStartSearch, // = -1 // start from 0
	bool bUseEffectiveCmdID // = false
	)
{
	ASSERT_VALID( this );
INT nNodeCount = m_arrChilds.GetSize();
	if( nNodeCount == 0 )
		return -1;
	for(	INT nPos =
				( nPosStartSearch < 0 || nPosStartSearch >= nNodeCount )
					? 0 : ( nPosStartSearch + 1 );
			nPos < nNodeCount;
			nPos++
		)
	{
		CExtCustomizeCmdTreeNode * pNodeCmp =
			(CExtCustomizeCmdTreeNode *)
				m_arrChilds.ElementAt( nPos );
		ASSERT_VALID( pNodeCmp );
		ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNodeCmp );
		if( pNodeCmp->GetCmdID(bUseEffectiveCmdID) == nCmdID )
			return nPos;
	}
	return -1;
}

INT CExtCustomizeCmdTreeNode::SearchNodeBackward(
	UINT nCmdID,
	INT nPosStartSearch, // = -1 // start from (node_count - 1)
	bool bUseEffectiveCmdID // = false
	)
{
	ASSERT_VALID( this );
INT nNodeCount = m_arrChilds.GetSize();
	if( nNodeCount == 0 )
		return -1;
	for(	INT nPos =
				( nPosStartSearch < 0 || nPosStartSearch >= nNodeCount )
					? (nNodeCount - 1) : ( nPosStartSearch - 1 );
			(nPos + 1) > 0;
			nPos--
		)
	{
		CExtCustomizeCmdTreeNode * pNodeCmp =
			(CExtCustomizeCmdTreeNode *)
				m_arrChilds.ElementAt( nPos );
		ASSERT_VALID( pNodeCmp );
		ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNodeCmp );
		if( pNodeCmp->GetCmdID(bUseEffectiveCmdID) == nCmdID )
			return nPos;
	}
	return -1;
}

INT CExtCustomizeCmdTreeNode::SearchNode(
	__EXT_MFC_SAFE_LPCTSTR strText, // NULL or _T("") - unnamed
	INT nTextType, // 0-toolbar, 1-menu, 2-user
	INT nPosStartSearch // = -1 // start from 0
	)
{
	ASSERT_VALID( this );
INT nNodeCount = m_arrChilds.GetSize();
	if( nNodeCount == 0 )
		return -1;
	if( strText == NULL )
		strText = _T("");
	for(	INT nPos =
				( nPosStartSearch < 0 || nPosStartSearch >= nNodeCount )
					? 0 : ( nPosStartSearch + 1 );
			nPos < nNodeCount;
			nPos++
		)
	{
		CExtCustomizeCmdTreeNode * pNodeCmp =
			(CExtCustomizeCmdTreeNode *)
				m_arrChilds.ElementAt( nPos );
		ASSERT_VALID( pNodeCmp );
		ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNodeCmp );
		__EXT_MFC_SAFE_LPCTSTR strTextCmp =
			( nTextType == 0 )
				? pNodeCmp->GetTextInToolbar( NULL )
				:	(	( nTextType == 1 )
							? pNodeCmp->GetTextInMenu( NULL )
							: pNodeCmp->GetTextUser()
					)
				;
		ASSERT( strTextCmp != NULL );
		if( _tcscmp( strText, strTextCmp ) == 0 )
			return nPos;
	}
	return -1;
}

INT CExtCustomizeCmdTreeNode::SearchNodeBackward(
	__EXT_MFC_SAFE_LPCTSTR strText, // NULL or _T("") - unnamed
	INT nTextType, // 0-toolbar, 1-menu, 2-user
	INT nPosStartSearch // = -1 // start from (node_count - 1)
	)
{
	ASSERT_VALID( this );
INT nNodeCount = m_arrChilds.GetSize();
	if( nNodeCount == 0 )
		return -1;
	if( strText == NULL )
		strText = _T("");
	for(	INT nPos =
				( nPosStartSearch < 0 || nPosStartSearch >= nNodeCount )
					? (nNodeCount - 1) : ( nPosStartSearch - 1 );
			(nPos + 1) > 0;
			nPos--
		)
	{
		CExtCustomizeCmdTreeNode * pNodeCmp =
			(CExtCustomizeCmdTreeNode *)
				m_arrChilds.ElementAt( nPos );
		ASSERT_VALID( pNodeCmp );
		ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNodeCmp );
		__EXT_MFC_SAFE_LPCTSTR strTextCmp =
			( nTextType == 0 )
				? pNodeCmp->GetTextInToolbar( NULL )
				:	(	( nTextType == 1 )
							? pNodeCmp->GetTextInMenu( NULL )
							: pNodeCmp->GetTextUser()
					)
				;
		ASSERT( strTextCmp != NULL );
		if( _tcscmp( strText, strTextCmp ) == 0 )
			return nPos;
	}
	return -1;
}

CExtCustomizeCmdTreeNode *
	CExtCustomizeCmdTreeNode::SearchNodeElement(
		UINT nCmdID,
		INT nPosStartSearch, // = -1 // start from 0
		bool bUseEffectiveCmdID, // = false
		bool bDeep // = false
		)
{
	INT nPos = SearchNode( nCmdID, nPosStartSearch, bUseEffectiveCmdID );
	if( nPos < 0 )
	{
		if( bDeep )
		{
			INT nNodeCount = GetNodeCount();
			for( nPos = 0; nPos < nNodeCount; nPos++ )
			{
				CExtCustomizeCmdTreeNode * pNode =
					ElementAt( nPos );
				ASSERT_VALID( pNode );
				ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNode );
				CExtCustomizeCmdTreeNode * pFoundNode =
					pNode->SearchNodeElement(
						nCmdID,
						-1,
						bUseEffectiveCmdID,
						true
						);
				if( pFoundNode == NULL )
					continue;
				ASSERT_VALID( pFoundNode );
				ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pFoundNode );
				return pFoundNode;
			} // for( nPos = 0; nPos < nNodeCount; nPos++ )
		} // if( bDeep )
		return NULL;
	} // if( nPos < 0 )
CExtCustomizeCmdTreeNode * pFoundNode =
		ElementAt( nPos );
	ASSERT_VALID( pFoundNode );
	ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pFoundNode );
	return pFoundNode;
}

CExtCustomizeCmdTreeNode *
	CExtCustomizeCmdTreeNode::SearchNodeElement(
		__EXT_MFC_SAFE_LPCTSTR strText, // NULL or _T("") - unnamed
		INT nTextType, // 0-toolbar, 1-menu, 2-user
		INT nPosStartSearch // = -1 // start from 0
		)
{
	INT nPos = SearchNode( strText, nTextType, nPosStartSearch );
	if( nPos < 0 )
		return NULL;
	return ElementAt( nPos );
}

bool CExtCustomizeCmdTreeNode::IsChild(
	CExtCustomizeCmdTreeNode * pChildTest
	)
{
	ASSERT_VALID( this );
	if( pChildTest == NULL )
		return false;
	ASSERT_VALID( pChildTest );
	ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pChildTest );
	if( ((LPVOID)this) == ((LPVOID)pChildTest) )
		return false;
	for(	pChildTest = pChildTest->GetParentNode();
			pChildTest != NULL;
			pChildTest = pChildTest->GetParentNode()
		)
	{
		ASSERT_VALID( pChildTest );
		ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pChildTest );
		if( ((LPVOID)this) == ((LPVOID)pChildTest) )
			return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CExtCustomizeSite

UINT CExtCustomizeSite::g_nMsgQueryCustomizeSite =
	::RegisterWindowMessage(
		_T("CExtCustomizeSite::g_nMsgQueryCustomizeSite")
		);
UINT CExtCustomizeSite::g_nMsgAssignAccelTable =
	::RegisterWindowMessage(
		_T("CExtCustomizeSite::g_nMsgAssignAccelTable")
		);
UINT CExtCustomizeSite::g_nMsgSyncAccelTable =
	::RegisterWindowMessage(
		_T("CExtCustomizeSite::g_nMsgSyncAccelTable")
		);
UINT CExtCustomizeSite::g_nMsgSyncContent =
	::RegisterWindowMessage(
		_T("CExtCustomizeSite::g_nMsgSyncContent")
		);
UINT CExtCustomizeSite::g_nMsgDelayCustomization =
	::RegisterWindowMessage(
		_T("CExtCustomizeSite::g_nMsgDelayCustomization")
		);

CExtCustomizeSite::CCmdDropTarget::CCmdDropTarget(
	CExtCustomizeSite::ICustomizeDropTarget * pCustomizeDropTarget,
	CExtCustomizeSite * pSite
	)
	: m_pCustomizeDropTarget( pCustomizeDropTarget )
	, m_pSite( pSite )
{
	ASSERT( m_pCustomizeDropTarget != NULL );
	ASSERT( m_pSite != NULL );
}

CExtCustomizeSite::CCmdDropTarget::~CCmdDropTarget()
{
}

#ifdef _DEBUG
void CExtCustomizeSite::CCmdDropTarget::AssertValid() const
{
	COleDropTarget::AssertValid();
	ASSERT( m_pCustomizeDropTarget != NULL );
	ASSERT( m_pSite != NULL );
}
#endif // _DEBUG

DROPEFFECT CExtCustomizeSite::CCmdDropTarget::OnDragEnter(
	CWnd * pWnd,
	COleDataObject * pDataObject,
	DWORD dwKeyState,
	CPoint point
	)
{
	__PROF_UIS_MANAGE_STATE;

	ASSERT_VALID( this );
	ASSERT( m_pSite != NULL );
	pWnd;
CExtCustomizeSite::CCmdDragInfo _dragInfo( pDataObject, m_pSite->m_cf );
	if( _dragInfo.IsEmpty() )
		return DROPEFFECT_NONE;
DROPEFFECT de =
		m_pCustomizeDropTarget->OnCustomizeTargetEnter(
			_dragInfo,
			point,
			dwKeyState
			);
	if(		de != DROPEFFECT_COPY
		&&	de != DROPEFFECT_MOVE
		)
		return DROPEFFECT_NONE;
	if( de == DROPEFFECT_COPY )
		return DROPEFFECT_COPY;
	ASSERT( de == DROPEFFECT_MOVE );
CExtCustomizeSite::ICustomizeDropSource *
		pCustomizeDropSource =
			m_pSite->GetCustomizeDropSource();
	ASSERT( pCustomizeDropSource != NULL );
	if( !pCustomizeDropSource->OnCustomizeSourceAllowMoveDel() )
		de = DROPEFFECT_COPY;
	return DROPEFFECT_MOVE;
}

void CExtCustomizeSite::CCmdDropTarget::OnDragLeave(
	CWnd * pWnd
	)
{
	__PROF_UIS_MANAGE_STATE;

	ASSERT_VALID( this );
	pWnd;
	m_pCustomizeDropTarget->OnCustomizeTargetLeave();
}

DROPEFFECT CExtCustomizeSite::CCmdDropTarget::OnDragOver(
	CWnd * pWnd,
	COleDataObject * pDataObject,
	DWORD dwKeyState,
	CPoint point
	)
{
	__PROF_UIS_MANAGE_STATE;

	ASSERT_VALID( this );
	ASSERT( m_pSite != NULL );
	pWnd;
CExtCustomizeSite::CCmdDragInfo _dragInfo( pDataObject, m_pSite->m_cf );
	if( _dragInfo.IsEmpty() )
		return DROPEFFECT_NONE;
DROPEFFECT de =
		m_pCustomizeDropTarget->OnCustomizeTargetOver(
			_dragInfo,
			point,
			dwKeyState
			);
	if(		de != DROPEFFECT_COPY
		&&	de != DROPEFFECT_MOVE
		)
		return DROPEFFECT_NONE;
	if( de == DROPEFFECT_COPY )
		return DROPEFFECT_COPY;
	ASSERT( de == DROPEFFECT_MOVE );
CExtCustomizeSite::ICustomizeDropSource *
		pCustomizeDropSource =
			m_pSite->GetCustomizeDropSource();
	ASSERT( pCustomizeDropSource != NULL );
	if( !pCustomizeDropSource->OnCustomizeSourceAllowMoveDel() )
		de = DROPEFFECT_COPY;
	return DROPEFFECT_MOVE;
}

BOOL CExtCustomizeSite::CCmdDropTarget::OnDrop(
	CWnd * pWnd,
	COleDataObject * pDataObject,
	DROPEFFECT dropEffect,
	CPoint point
	)
{
	__PROF_UIS_MANAGE_STATE;

	ASSERT_VALID( this );
	ASSERT( m_pSite != NULL );
	pWnd;
CExtCustomizeSite::CCmdDragInfo _dragInfo( pDataObject, m_pSite->m_cf );
	if( _dragInfo.IsEmpty() )
		return DROPEFFECT_NONE;
BOOL bRetVal =
		m_pCustomizeDropTarget->OnCustomizeTargetDrop(
			_dragInfo,
			point,
			dropEffect
			) ? true : false;
	return bRetVal;
}

CExtCustomizeSite::CCmdDropSource::CCmdDropSource(
	CExtCustomizeSite::ICustomizeDropSource * pCustomizeDropSource,
	CExtCustomizeSite * pSite
	)
	: m_pCustomizeDropSource( pCustomizeDropSource )
	, m_pSite( pSite )
	, m_hCursorDelete( NULL )
	, m_hCursorMove( NULL )
	, m_hCursorCopy( NULL )
	, m_deLast( DROPEFFECT_NONE )
	, m_EscapePressed( false )
{
	ASSERT( m_pCustomizeDropSource != NULL );
	ASSERT( m_pSite != NULL );
CWinApp * pApp = ::AfxGetApp();
	ASSERT_VALID( pApp );
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
CExtLocalResourceHelper _LRH;
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
	m_hCursorDelete = pApp->LoadCursor( IDC_EXT_CMD_DELETE );
	ASSERT( m_hCursorDelete != NULL );
	m_hCursorMove = pApp->LoadCursor( IDC_EXT_CMD_MOVE );
	ASSERT( m_hCursorMove != NULL );
	m_hCursorCopy = pApp->LoadCursor( IDC_EXT_CMD_COPY );
	ASSERT( m_hCursorCopy != NULL );
}

CExtCustomizeSite::CCmdDropSource::~CCmdDropSource()
{
	if( m_hCursorDelete != NULL )
		::DestroyCursor( m_hCursorDelete );
	if( m_hCursorMove != NULL )
		::DestroyCursor( m_hCursorMove );
	if( m_hCursorCopy != NULL )
		::DestroyCursor( m_hCursorCopy );
}

#ifdef _DEBUG
void CExtCustomizeSite::CCmdDropSource::AssertValid() const
{
	COleDropSource::AssertValid();
	ASSERT( m_pCustomizeDropSource != NULL );
	ASSERT( m_pSite != NULL );
}
#endif // _DEBUG

BOOL CExtCustomizeSite::CCmdDropSource::OnBeginDrag(
	CWnd * pWnd
	)
{
	__PROF_UIS_MANAGE_STATE;

	ASSERT_VALID( this );
	return COleDropSource::OnBeginDrag( pWnd );
}

SCODE CExtCustomizeSite::CCmdDropSource::QueryContinueDrag(
	BOOL bEscapePressed,
	DWORD dwKeyState
	)
{
	__PROF_UIS_MANAGE_STATE;

	ASSERT_VALID( this );
SCODE sCode = COleDropSource::QueryContinueDrag( bEscapePressed, dwKeyState );
	if(		sCode == S_OK
		&&	m_deLast == DROPEFFECT_NONE
		&&	m_hCursorDelete != NULL
		)
		::SetCursor( m_hCursorDelete );
	if( bEscapePressed && sCode != S_OK )
		m_EscapePressed = true;
	return sCode;
}

SCODE CExtCustomizeSite::CCmdDropSource::GiveFeedback(
	DROPEFFECT dropEffect
	)
{
	__PROF_UIS_MANAGE_STATE;

	ASSERT_VALID( this );
	m_deLast = dropEffect;
	if( dropEffect == DROPEFFECT_COPY )
	{
		if( m_hCursorCopy == NULL )
			return COleDropSource::GiveFeedback( dropEffect );
		::SetCursor( m_hCursorCopy );
		return S_OK;
	} // if( dropEffect == DROPEFFECT_COPY )
	if( dropEffect == DROPEFFECT_MOVE )
	{
		if( m_hCursorMove == NULL )
			return COleDropSource::GiveFeedback( dropEffect );
		::SetCursor( m_hCursorMove );
		return S_OK;
	} // if( dropEffect == DROPEFFECT_MOVE )
	if( m_hCursorDelete == NULL )
		return COleDropSource::GiveFeedback( dropEffect );
	::SetCursor( m_hCursorDelete );
	return S_OK;
}

CExtCustomizeSite::CCategoryCustomizeInfo::CCategoryCustomizeInfo(
	__EXT_MFC_SAFE_LPCTSTR strName,
	CExtCustomizeSite * pSite
	)
	: m_bVisibleInKeyboardPage( true )
	, m_pSite( pSite )
	, m_pRoot( NULL )
{
	ASSERT( strName != NULL && _tcslen(strName) > 0 );
	ASSERT( m_pSite != NULL );
	m_pRoot =
		m_pSite->OnCmdNodeCreate(
			0,
			0,
			NULL,
			0,
			NULL,
			NULL,
			strName
			);
	ASSERT_VALID( m_pRoot );
}

CExtCustomizeSite::CCategoryCustomizeInfo::~CCategoryCustomizeInfo()
{
	if( m_pRoot != NULL )
		delete m_pRoot;
}

int CExtCustomizeSite::CCategoryCustomizeInfo::MakeCmdsUnique()
{
int nCount = m_pRoot->GetNodeCount();
	if( nCount <= 1 )
		return nCount;
	for( int i = 0; i < (nCount-1); i++ )
	{
		CExtCustomizeCmdTreeNode * pNode =
			m_pRoot->ElementAt( i );
		ASSERT_VALID( pNode );
		UINT nCmdID = pNode->GetCmdID(false);
		for( int j = i+1; j < nCount; )
		{
			CExtCustomizeCmdTreeNode * pNode2 =
				m_pRoot->ElementAt( j );
			ASSERT_VALID( pNode );
			UINT nCmdID2 = pNode2->GetCmdID(false);
			if( nCmdID == nCmdID2 )
			{
				m_pRoot->RemoveNodes( j, 1 );
				nCount --;
			} // if( nCmdID == nCmdID2 )
			else
				j ++;
		} // for( int j = i+1; j < nCount; )
	} // for( int i = 0; i < (nCount-1); i++ )
	ASSERT( nCount == m_pRoot->GetNodeCount() );
	return nCount;
}

CExtCustomizeSite::CToolBarCustomizeInfo::CToolBarCustomizeInfo()
	: m_pHelperBarButtonsI( NULL )
	, m_pHelperBarButtonsC( NULL )
	, m_bUserToolBar( false )
	, m_strDisplayName( _T("") )
{
}

CExtCustomizeSite::CToolBarCustomizeInfo::~CToolBarCustomizeInfo()
{
}

CExtCustomizeSite::mapHelperPopupHooks_t CExtCustomizeSite::g_mapHelperPopupHooks;

CExtCustomizeSite::CExtCustomizeSite()
	: m_dwCustomizeFlags( 0 )
	, m_bInitComplete( false )
	, m_bCustomizeMode( false )
	, m_pWndTop( NULL )
	, m_hWndCustomizeForm( NULL )
	, m_pWndMenuBar( NULL )
	, m_cf( NULL )
	, m_pHelperCustomizeDropSource( NULL )
	, m_pHelperDragSourceNode( NULL )
	, m_pNodeToolbarsI( NULL )
	, m_pNodeToolbarsC( NULL )
	, m_pPopupDropTargetTop( NULL )
	, m_pPopupDropTargetTopSaved( NULL )
	, m_pPopupDropTargetNode( NULL )
	, m_pPopupDropTargetNodeSaved( NULL )
	, m_pMenuInfoActiveCustomize( NULL )
	, m_pBarDefCmdTarget( NULL )
	, m_pNodeCustomized( NULL )
	, m_pActiveItem( NULL )
	, m_hWndActiveItemInvalidate( NULL )
	, m_hWndMdiClient( NULL )
	, m_pHelperResizingNode( NULL )
	, m_hWndHelperPopupHook( NULL )
{
}

CExtCustomizeSite::~CExtCustomizeSite()
{
	_EmptyContainers();
}

void CExtCustomizeSite::_EmptyContainers()
{
	ASSERT( this != NULL );

	if( m_hWndHelperPopupHook != NULL )
	{
		g_CmdManager->ProfileWndRemove( m_hWndHelperPopupHook );
		g_mapHelperPopupHooks.RemoveKey( m_hWndHelperPopupHook );
	} // if( m_hWndHelperPopupHook != NULL )
	
	UnRegisterAllCommandDropTargets();

	for(	POSITION pos = m_mapToolBars.GetStartPosition();
			pos != NULL;
			)
	{
		CExtToolControlBar * pBar = NULL;
		CToolBarCustomizeInfo * pInfo = NULL;
		m_mapToolBars.GetNextAssoc( pos, (void * &)pBar, (void * &)pInfo );
		ASSERT( pBar != NULL );
		ASSERT( pInfo != NULL );
		delete pInfo;
	}
	m_mapToolBars.RemoveAll();

	m_listHelperBarsForListBox.RemoveAll();

	CategoryRemoveAll();

	for( INT i = 0; i < m_arrMenuInfo.GetSize(); i++ )
	{
		CCmdMenuInfo * pCmdMenuInfo = (CCmdMenuInfo *)
			m_arrMenuInfo[i];
		ASSERT( pCmdMenuInfo != NULL );
		delete pCmdMenuInfo;
	} // for( INT i = 0; i < m_arrMenuInfo.GetSize(); i++ )
	m_arrMenuInfo.RemoveAll();

	if( m_pNodeToolbarsI != NULL )
	{
		delete m_pNodeToolbarsI;
		m_pNodeToolbarsI = NULL;
	} // if( m_pNodeToolbarsI != NULL )
	if( m_pNodeToolbarsC != NULL )
	{
		delete m_pNodeToolbarsC;
		m_pNodeToolbarsC = NULL;
	} // if( m_pNodeToolbarsC != NULL )
}

int CExtCustomizeSite::MenuInfoGetCount()
{
	ASSERT( this != NULL );
	return m_arrMenuInfo.GetSize();
}

bool CExtCustomizeSite::MenuInfoAddEmpty(
	CWnd * pWndTop,
	__EXT_MFC_SAFE_LPCTSTR strName, // = _T("Default")
	bool bDefaultMenu, // = true
	CRuntimeClass * pRtcFrame, // = NULL
	CRuntimeClass * pRtcView, // = NULL
	CRuntimeClass * pRtcDoc // = NULL
	)
{
	ASSERT( this != NULL );
	ASSERT_VALID( pWndTop );
	ASSERT( pWndTop->GetSafeHwnd() != NULL );
	if(		strName == NULL
		||	_tcslen(strName) == 0
		||	(	bDefaultMenu
			&&	MenuInfoGetDefault() != NULL
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
CCmdMenuInfo * pCmdMenuInfo = new CCmdMenuInfo( this, strName );
	if( !pCmdMenuInfo->InitializeEmpty( pWndTop, bDefaultMenu ) )
	{
		ASSERT( FALSE );
		return false;
	}
	if( bDefaultMenu )
		m_arrMenuInfo.InsertAt( 0, pCmdMenuInfo );
	else
		m_arrMenuInfo.Add( pCmdMenuInfo );
	if( pRtcFrame != NULL )
	{
		VERIFY(
			pCmdMenuInfo->AddRuntimeClassFrame(
				pRtcFrame
				)
			);
	} // if( pRtcFrame != NULL )
	if( pRtcView != NULL )
	{
		VERIFY(
			pCmdMenuInfo->AddRuntimeClassView(
				pRtcView
				)
			);
	} // if( pRtcView != NULL )
	if( pRtcDoc != NULL )
	{
		VERIFY(
			pCmdMenuInfo->AddRuntimeClassDocument(
				pRtcDoc
				)
			);
	} // if( pRtcDoc != NULL )
	return true;
}

bool CExtCustomizeSite::MenuInfoAdd(
	CWnd * pWndTop,
	__EXT_MFC_SAFE_LPCTSTR strName,
	CMenu * pMenu,
	bool bDefaultMenu,
	bool bPopupMenu, // = false
	CRuntimeClass * pRtcFrame, // = NULL
	CRuntimeClass * pRtcView, // = NULL
	CRuntimeClass * pRtcDoc // = NULL
	)
{
	ASSERT( this != NULL );
	ASSERT_VALID( pWndTop );
	ASSERT( pWndTop->GetSafeHwnd() != NULL );
	if(		strName == NULL
		||	_tcslen(strName) == 0
		||	pMenu->GetSafeHmenu() == NULL
		||	(! ::IsMenu(pMenu->GetSafeHmenu()) )
		||	(	bDefaultMenu
			&&	MenuInfoGetDefault() != NULL
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
CCmdMenuInfo * pCmdMenuInfo = new CCmdMenuInfo( this, strName );
	if( !pCmdMenuInfo->Initialize( pWndTop, pMenu, bDefaultMenu, bPopupMenu ) )
	{
		ASSERT( FALSE );
		return false;
	}
	if( bDefaultMenu )
		m_arrMenuInfo.InsertAt( 0, pCmdMenuInfo );
	else
		m_arrMenuInfo.Add( pCmdMenuInfo );
	if( pRtcFrame != NULL )
	{
		VERIFY(
			pCmdMenuInfo->AddRuntimeClassFrame(
				pRtcFrame
				)
			);
	} // if( pRtcFrame != NULL )
	if( pRtcView != NULL )
	{
		VERIFY(
			pCmdMenuInfo->AddRuntimeClassView(
				pRtcView
				)
			);
	} // if( pRtcView != NULL )
	if( pRtcDoc != NULL )
	{
		VERIFY(
			pCmdMenuInfo->AddRuntimeClassDocument(
				pRtcDoc
				)
			);
	} // if( pRtcDoc != NULL )
	return true;
}

bool CExtCustomizeSite::MenuInfoAdd(
	CWnd * pWndTop,
	__EXT_MFC_SAFE_LPCTSTR strName,
	__EXT_MFC_SAFE_LPCTSTR strResourceID,
	bool bDefaultMenu,
	bool bPopupMenu, // = false
	CRuntimeClass * pRtcFrame, // = NULL
	CRuntimeClass * pRtcView, // = NULL
	CRuntimeClass * pRtcDoc // = NULL
	)
{
	ASSERT( this != NULL );
	ASSERT_VALID( pWndTop );
	ASSERT( pWndTop->GetSafeHwnd() != NULL );
	if(		strName == NULL
		||	_tcslen(strName) == 0
		||	(	bDefaultMenu
			&&	MenuInfoGetDefault() != NULL
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
CCmdMenuInfo * pCmdMenuInfo = new CCmdMenuInfo( this, strName );
	if( !pCmdMenuInfo->Initialize( pWndTop, strResourceID, bDefaultMenu, bPopupMenu ) )
	{
		ASSERT( FALSE );
		return false;
	}
	if( bDefaultMenu )
		m_arrMenuInfo.InsertAt( 0, pCmdMenuInfo );
	else
		m_arrMenuInfo.Add( pCmdMenuInfo );
	if( pRtcFrame != NULL )
	{
		VERIFY(
			pCmdMenuInfo->AddRuntimeClassFrame(
				pRtcFrame
				)
			);
	} // if( pRtcFrame != NULL )
	if( pRtcView != NULL )
	{
		VERIFY(
			pCmdMenuInfo->AddRuntimeClassView(
				pRtcView
				)
			);
	} // if( pRtcView != NULL )
	if( pRtcDoc != NULL )
	{
		VERIFY(
			pCmdMenuInfo->AddRuntimeClassDocument(
				pRtcDoc
				)
			);
	} // if( pRtcDoc != NULL )
	return true;
}

bool CExtCustomizeSite::MenuInfoAdd(
	CWnd * pWndTop,
	__EXT_MFC_SAFE_LPCTSTR strName,
	UINT nResourceID,
	bool bDefaultMenu,
	bool bPopupMenu, // = false
	CRuntimeClass * pRtcFrame, // = NULL
	CRuntimeClass * pRtcView, // = NULL
	CRuntimeClass * pRtcDoc // = NULL
	)
{
	ASSERT( this != NULL );
	ASSERT_VALID( pWndTop );
	ASSERT( pWndTop->GetSafeHwnd() != NULL );
	if(		strName == NULL
		||	_tcslen(strName) == 0
		||	(	bDefaultMenu
			&&	MenuInfoGetDefault() != NULL
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
CCmdMenuInfo * pCmdMenuInfo = new CCmdMenuInfo( this, strName );
	if( !pCmdMenuInfo->Initialize( pWndTop, nResourceID, bDefaultMenu, bPopupMenu ) )
	{
		ASSERT( FALSE );
		return false;
	}
	if( bDefaultMenu )
		m_arrMenuInfo.InsertAt( 0, pCmdMenuInfo );
	else
		m_arrMenuInfo.Add( pCmdMenuInfo );
	if( pRtcFrame != NULL )
	{
		VERIFY(
			pCmdMenuInfo->AddRuntimeClassFrame(
				pRtcFrame
				)
			);
	} // if( pRtcFrame != NULL )
	if( pRtcView != NULL )
	{
		VERIFY(
			pCmdMenuInfo->AddRuntimeClassView(
				pRtcView
				)
			);
	} // if( pRtcView != NULL )
	if( pRtcDoc != NULL )
	{
		VERIFY(
			pCmdMenuInfo->AddRuntimeClassDocument(
				pRtcDoc
				)
			);
	} // if( pRtcDoc != NULL )
	return true;
}

int CExtCustomizeSite::MenuInfoRemove( // returns removed count
	int nPos, // = 0
	int nCount // = -1 // -1 - remove all
	)
{
	ASSERT( this != NULL );
	ASSERT( nPos >= 0 );
	if( nCount == 0 )
		return 0;
INT nMenuInfoCount = m_arrMenuInfo.GetSize();
	if( nMenuInfoCount == 0 )
		return 0;
	if( nPos >= nMenuInfoCount )
		return 0;
	if( nCount < 0 )
		nCount = nMenuInfoCount;
	if( (nPos + nCount) > nMenuInfoCount )
		nCount = nMenuInfoCount - nPos;
INT nStart = nPos, nEnd = nPos + nCount - 1;
	for( ; nStart <= nEnd; nStart++ )
	{
		CCmdMenuInfo * pCmdMenuInfo = (CCmdMenuInfo *)
			m_arrMenuInfo[ nStart ];
		ASSERT( pCmdMenuInfo != NULL );
		delete pCmdMenuInfo;
	} // for( ; nStart <= nEnd; nStart++ )
	m_arrMenuInfo.RemoveAt( nPos, nCount );
	return nCount;
}

CExtCustomizeSite::CCmdMenuInfo *
	CExtCustomizeSite::MenuInfoGetAt(
		int nPos // = 0
		)
{
	ASSERT( this != NULL );
	if( nPos < 0 || nPos >= m_arrMenuInfo.GetSize() )
	{
		ASSERT( FALSE );
		return NULL;
	}
CCmdMenuInfo * pCmdMenuInfo = (CCmdMenuInfo *)
		m_arrMenuInfo[nPos];
	ASSERT( pCmdMenuInfo != NULL );
	ASSERT( pCmdMenuInfo->IsInitialized() );
	return pCmdMenuInfo;
}

CExtCustomizeSite::CCmdMenuInfo *
	CExtCustomizeSite::MenuInfoGetDefault()
{
	ASSERT( this != NULL );
	if( m_arrMenuInfo.GetSize() == 0 )
		return NULL;
CCmdMenuInfo * pCmdMenuInfo = (CCmdMenuInfo *)
		m_arrMenuInfo[0];
	ASSERT( pCmdMenuInfo != NULL );
	ASSERT( pCmdMenuInfo->IsInitialized() );
	if( !pCmdMenuInfo->IsDefaultMenu() )
		return NULL;
	return pCmdMenuInfo;
}

CExtCustomizeSite::CCmdMenuInfo *
	CExtCustomizeSite::MenuInfoGetByName(
		__EXT_MFC_SAFE_LPCTSTR strName
		)
{
	ASSERT( this != NULL );
	if(		strName == NULL
		||	_tcslen(strName) == 0
		)
	{
		ASSERT( FALSE );
		return NULL;
	}
	for( INT i = 0; i < m_arrMenuInfo.GetSize(); i++ )
	{
		CCmdMenuInfo * pCmdMenuInfo = (CCmdMenuInfo *)
			m_arrMenuInfo[i];
		ASSERT( pCmdMenuInfo != NULL );
		ASSERT( pCmdMenuInfo->GetName() != NULL );
		if(	_tcscmp(
				pCmdMenuInfo->GetName(),
				strName
				) == 0
			)
			return pCmdMenuInfo;
	} // for( INT i = 0; i < m_arrMenuInfo.GetSize(); i++ )
	return NULL;
}

CExtCustomizeSite::CCmdMenuInfo * CExtCustomizeSite::MenuInfoFindForMenuBar()
{
	ASSERT( this != NULL );
CCmdMenuInfo * pMenuInfoActiveCustomize = MenuInfoActiveGet();
	if( pMenuInfoActiveCustomize != NULL )
		return pMenuInfoActiveCustomize;
int nMenuInfoCount = MenuInfoGetCount();
	if( nMenuInfoCount == 0 )
		return NULL;
	if(		m_pWndTop->GetSafeHwnd() == NULL
		||	(! m_pWndTop->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
		)
		return MenuInfoGetDefault();
CMDIFrameWnd * pFrame = (CMDIFrameWnd *)m_pWndTop;
CFrameWnd * pFrameA = pFrame->GetActiveFrame();
	ASSERT_VALID( pFrameA );
	if( pFrameA == NULL || ((LPVOID)pFrameA) == ((LPVOID)pFrame) )
		return MenuInfoGetDefault();
CDocument * pDoc = pFrameA->GetActiveDocument();
	if( pDoc != NULL )
	{
		for( INT i = 0; i < nMenuInfoCount; i++ )
		{
			CCmdMenuInfo * pCmdMenuInfo = (CCmdMenuInfo *)
				m_arrMenuInfo[i];
			ASSERT( pCmdMenuInfo != NULL );
			if( pCmdMenuInfo->IsDocument(pDoc) )
				return pCmdMenuInfo;
		} // for( INT i = 0; i < nMenuInfoCount; i++ )
	} // if( pDoc != NULL )
CWnd * pWndView = pFrameA->GetActiveView();
	if( pWndView == NULL )
		pWndView = pFrameA->GetWindow( GW_CHILD );
	if( pWndView == NULL )
		return MenuInfoGetDefault();
	for( INT i = 0; i < nMenuInfoCount; i++ )
	{
		CCmdMenuInfo * pCmdMenuInfo = (CCmdMenuInfo *)
			m_arrMenuInfo[i];
		ASSERT( pCmdMenuInfo != NULL );
		if( pCmdMenuInfo->IsView(pWndView) )
			return pCmdMenuInfo;
	} // for( INT i = 0; i < nMenuInfoCount; i++ )
	for( i = 0; i < nMenuInfoCount; i++ )
	{
		CCmdMenuInfo * pCmdMenuInfo = (CCmdMenuInfo *)
			m_arrMenuInfo[i];
		ASSERT( pCmdMenuInfo != NULL );
		if( pCmdMenuInfo->IsFrame(pFrameA) )
			return pCmdMenuInfo;
	} // for( i = 0; i < nMenuInfoCount; i++ )
	for( i = 0; i < nMenuInfoCount; i++ )
	{
		CCmdMenuInfo * pCmdMenuInfo = (CCmdMenuInfo *)
			m_arrMenuInfo[i];
		ASSERT( pCmdMenuInfo != NULL );
		if( pCmdMenuInfo->IsFrame(pFrame) )
			return pCmdMenuInfo;
	} // for( i = 0; i < nMenuInfoCount; i++ )
	return MenuInfoGetDefault();
}

CExtCustomizeSite::CCmdMenuInfo * CExtCustomizeSite::MenuInfoActiveGet()
{
	ASSERT( this != NULL );
	return m_pMenuInfoActiveCustomize;
}

CExtCustomizeSite::CCmdMenuInfo * CExtCustomizeSite::MenuInfoActiveSet(
	CExtCustomizeSite::CCmdMenuInfo * pCmdMenuInfo
	)
{
	ASSERT( this != NULL );
CCmdMenuInfo * pMenuInfoActiveCustomizeOld = m_pMenuInfoActiveCustomize;
	m_pMenuInfoActiveCustomize = pCmdMenuInfo;
	if( m_pWndMenuBar != NULL )
	{
		ASSERT_VALID( m_pWndMenuBar );
		if( m_pWndMenuBar->GetSafeHwnd() != NULL )
			m_pWndMenuBar->UpdateMenuBar();
	} // if( m_pWndMenuBar != NULL )
	DropTargetPopupCancelEx( -2 );
	OnUpdateAccelGlobalInfo( false );
	return pMenuInfoActiveCustomizeOld;
}

int CExtCustomizeSite::MenuInfoFindIndex(
	CExtCustomizeSite::CCmdMenuInfo * pCmdMenuInfoSearch
	)
{
	ASSERT( this != NULL );
	if( pCmdMenuInfoSearch == NULL )
		return -1;
int nMenuInfoCount = MenuInfoGetCount();
	for( INT i = 0; i < nMenuInfoCount; i++ )
	{
		CCmdMenuInfo * pCmdMenuInfo = (CCmdMenuInfo *)
			m_arrMenuInfo[i];
		ASSERT( pCmdMenuInfo != NULL );
		if( ((LPVOID)pCmdMenuInfo) == ((LPVOID)pCmdMenuInfoSearch) )
			return i;
	} // for( INT i = 0; i < nMenuInfoCount; i++ )
	return -1;
}

bool CExtCustomizeSite::MenuInfoSetAccelTable(
	__EXT_MFC_SAFE_LPCTSTR strName,
	HACCEL hAccelTable,
	bool bUseTableCopy
	)
{
	ASSERT( this != NULL );
CCmdMenuInfo * pMenuInfo = MenuInfoGetByName( strName );
	if( pMenuInfo == NULL )
		return false;
	return
		pMenuInfo->AccelTableSet(
			hAccelTable,
			bUseTableCopy
			);
}

bool CExtCustomizeSite::MenuInfoLoadAccelTable(
	__EXT_MFC_SAFE_LPCTSTR strName,
	__EXT_MFC_SAFE_LPCTSTR strResourceID
	)
{
	ASSERT( this != NULL );
CCmdMenuInfo * pMenuInfo = MenuInfoGetByName( strName );
	if( pMenuInfo == NULL )
		return false;
HINSTANCE hInst =
		::AfxFindResourceHandle(
			strResourceID,
			RT_ACCELERATOR
			);
	if( hInst == NULL )
		return false;
	return
		pMenuInfo->AccelTableLoad(
			hInst, // ::AfxGetInstanceHandle()
			strResourceID
			);
}

bool CExtCustomizeSite::MenuInfoLoadAccelTable(
	__EXT_MFC_SAFE_LPCTSTR strName,
	UINT nResourceID
	)
{
	ASSERT( this != NULL );
CCmdMenuInfo * pMenuInfo = MenuInfoGetByName( strName );
	if( pMenuInfo == NULL )
		return false;
HINSTANCE hInst =
		::AfxFindResourceHandle(
			MAKEINTRESOURCE(nResourceID),
			RT_ACCELERATOR
			);
	if( hInst == NULL )
		return false;
	return
		pMenuInfo->AccelTableLoad(
			hInst, // ::AfxGetInstanceHandle()
			nResourceID
			);
}

void CExtCustomizeSite::OnUpdateAccelGlobalInfo(
	bool bDelay // = false
	)
{
	__PROF_UIS_MANAGE_STATE;

	ASSERT( this != NULL );
	if(		(!m_bInitComplete)
		||	m_pWndTop->GetSafeHwnd() == NULL
		||	(GetCustomizeFlags()&__ECSF_ACCELERATORS) == 0
		||	MenuInfoGetCount() == 0
		)
	{
		//ASSERT( FALSE );
		return;
	}
	ASSERT_VALID( m_pWndTop );
	ASSERT( m_pWndTop->GetSafeHwnd() != NULL && ::IsWindow(m_pWndTop->GetSafeHwnd()) );
	if( bDelay )
	{
		m_pWndTop->PostMessage( g_nMsgSyncAccelTable );
		return;
	} // if( bDelay )

CCmdMenuInfo * pCmdMenuInfo = MenuInfoGetDefault();

	if( (! m_pWndTop->IsKindOf(RUNTIME_CLASS(CFrameWnd)) ) )
	{ // non-frames are not directly supported by the customizesite
		if( pCmdMenuInfo != NULL )
		{
			HACCEL hAccel =
				pCmdMenuInfo->AccelTableGetSafe(false);
			if( hAccel != NULL )
				m_pWndTop->SendMessage(
					g_nMsgAssignAccelTable,
					(WPARAM)hAccel
					);
		} // if( pCmdMenuInfo != NULL )
	} // non-frames are not directly supported by the customizesite
	else
	{ // update frame accel tables
		CFrameWnd * pTopFrame = STATIC_DOWNCAST( CFrameWnd, m_pWndTop );
		if( pTopFrame->m_hAccelTable != NULL )
		{
			// MFC frames does not destroy their accelerators
			::DestroyAcceleratorTable( pTopFrame->m_hAccelTable );
			pTopFrame->m_hAccelTable = NULL;
		} // if( pTopFrame->m_hAccelTable != NULL )

		CDocManager * pDocManager = ::AfxGetApp()->m_pDocManager;
		if( pDocManager != NULL )
		{
			ASSERT_VALID( pDocManager );
			ASSERT_KINDOF( CDocManager, pDocManager );
			for(	POSITION pos = pDocManager->GetFirstDocTemplatePosition();
					pos != NULL;
					)
			{
				CDocTemplate * pDocTemplate =
					pDocManager->GetNextDocTemplate( pos );
				ASSERT_VALID( pDocTemplate );
				ASSERT_KINDOF( CDocTemplate, pDocTemplate );
				CMultiDocTemplate * pMultiDocTemplate =
					DYNAMIC_DOWNCAST( CMultiDocTemplate, pDocTemplate );
				if( pMultiDocTemplate != NULL )
				{
					if( pMultiDocTemplate->m_hAccelTable != NULL )
					{
						::DestroyAcceleratorTable( pMultiDocTemplate->m_hAccelTable );
						pMultiDocTemplate->m_hAccelTable = NULL;
					} // if( pMultiDocTemplate->m_hAccelTable != NULL )
				} // if( pMultiDocTemplate != NULL )
			}
		} // if( pDocManager != NULL )

		if( m_hWndMdiClient != NULL )
		{
			ASSERT_KINDOF( CMDIFrameWnd, m_pWndTop );
			CFrameWnd * pFrameActive = ((CMDIFrameWnd*)m_pWndTop)->GetActiveFrame();
			if(		((LPVOID)pFrameActive) != ((LPVOID)m_pWndTop)
				&&	pFrameActive->m_hAccelTable != NULL
				)
			{
				::DestroyAcceleratorTable( pFrameActive->m_hAccelTable );
				pFrameActive->m_hAccelTable = NULL;
			}
		} // if( m_hWndMdiClient != NULL )

		ASSERT( pTopFrame->m_hAccelTable == NULL );
		pCmdMenuInfo = MenuInfoFindForMenuBar();
		if( pCmdMenuInfo == NULL )
			pCmdMenuInfo = MenuInfoGetDefault();
		if( pCmdMenuInfo != NULL )
		{
			HACCEL hAccel =
				pCmdMenuInfo->AccelTableGetSafe(false);
			if( hAccel != NULL )
			{
				if( pTopFrame->SendMessage(
						g_nMsgAssignAccelTable,
						(WPARAM)hAccel
						) == 0
					)
				{
					pTopFrame->m_hAccelTable =
						pCmdMenuInfo->AccelTableGetSafe(true);
					ASSERT( pTopFrame->m_hAccelTable != NULL );
				}
			} // if( hAccel != NULL )
		} // if( pCmdMenuInfo != NULL )
	} // update frame accel tables

__EXT_MFC_SAFE_LPCTSTR strProfileName = g_CmdManager->ProfileNameFromWnd( m_pWndTop->m_hWnd );
	if( strProfileName == NULL )
		return; // shutdown mode
CExtCmdProfile * pProfile =
		g_CmdManager->ProfileGetPtr( strProfileName );
	ASSERT( pProfile != NULL );
	for(	POSITION pos = pProfile->m_cmds.GetStartPosition();
			pos != NULL;
			)
	{ // walk all the profile commands
		UINT nCmdID = 0;
		CExtCmdItem * pCmdItem = NULL;
		pProfile->m_cmds.GetNextAssoc( pos, nCmdID, pCmdItem );
		ASSERT( CExtCmdManager::IsCommand(nCmdID) );
		ASSERT( pCmdItem != NULL && nCmdID == pCmdItem->m_nCmdID );
		if( CExtCmdManager::IsSystemCommand(nCmdID) )
			continue;
		pCmdItem->m_sAccelText.Empty();
	} // walk all the profile commands

	if( pCmdMenuInfo != NULL )
	{
		CMapCmd2KeySet & _mapCmd2KeySet =
			pCmdMenuInfo->GetMapCmd2KeySet();
		for(	POSITION pos = _mapCmd2KeySet.GetStartPosition();
				pos != NULL;
				)
		{ // walk all the command key sets
			UINT nCmdID = 0;
			CCmdKeySet _cmdKeySet;
			_mapCmd2KeySet.GetNextAssoc( pos, nCmdID, _cmdKeySet );
			ASSERT( CExtCmdManager::IsCommand(nCmdID) );
			ASSERT( !_cmdKeySet.IsEmpty() );
			CExtCmdItem * pCmdItem = pProfile->CmdGetPtr( nCmdID );
			if( pCmdItem == NULL )
				continue;
			int nPos = OnChooseCmdKeyInfo( pCmdItem, _cmdKeySet );
			if( nPos < 0 )
				continue;
			ASSERT( nPos < _cmdKeySet.GetSize() );
			pCmdItem->m_sAccelText =
				OnFormatCmdAccelText(
					_cmdKeySet.ElementAt( nPos )
					);
		} // walk all the command key sets
	} // if( pCmdMenuInfo != NULL )
}

int CExtCustomizeSite::OnChooseCmdKeyInfo(
	CExtCmdItem * pCmdItem,
	const CCmdKeySet & _cmdKeySet
	)
{
	ASSERT( this != NULL );
	ASSERT( pCmdItem != NULL );
	ASSERT( !_cmdKeySet.IsEmpty() );
	pCmdItem;
	_cmdKeySet;
	return 0; // use first accel in list
}

void CExtCustomizeSite::OnChangedToolbarIconSizeOption()
{
	ASSERT( this != NULL );
	ASSERT_VALID( m_pWndTop );
	ASSERT( m_pWndTop->GetSafeHwnd() != NULL && ::IsWindow(m_pWndTop->GetSafeHwnd()) );
	if( m_pWndTop->IsKindOf(RUNTIME_CLASS(CFrameWnd)) )
	{
		((CFrameWnd*)m_pWndTop)->RecalcLayout();
		CExtControlBar::stat_RedrawFloatingFrames(
			(CFrameWnd*)m_pWndTop
			);
	} // if( m_pWndTop->IsKindOf(RUNTIME_CLASS(CFrameWnd)) )
}

bool CExtCustomizeSite::OnCanInsertCommandToAnyCategory(
	CExtCmdItem * pCmdItem
	)
{
	ASSERT( this != NULL );
	ASSERT( pCmdItem != NULL );
	ASSERT( CExtCmdManager::IsCommand(pCmdItem->m_nCmdID) );
	if(		pCmdItem->StateIsCustomToolbar()
		||	pCmdItem->StateIsDynamicPopup()
		||	pCmdItem->StateIsMenubarTemp()
		)
		return false;
	if( ! OnCanDragCustomizeCommand(pCmdItem) )
		return false;
	switch( pCmdItem->m_nCmdID )
	{
	// customize menu extensions
	case ID_EXT_NEW_MENU:
	case ID_EXT_CUSTOMIZE:
	case ID_EXT_RESET_TOOLBAR:
	case ID_EXT_ADD_REMOVE_BUTTONS:
	// button context menu
	case ID_EXT_BTN_RESET:
	case ID_EXT_BTN_DELETE:
	case ID_EXT_BTN_NAME:
	case ID_EXT_BTN_IMAGE_COPY:
	case ID_EXT_BTN_IMAGE_PASTE:
	case ID_EXT_BTN_IMAGE_RESET:
	case ID_EXT_BTN_IMAGE_EDIT:
	case ID_EXT_BTN_STYLE_DEFAULT:
	case ID_EXT_BTN_STYLE_TEXT_ONLY_ALWAYS:
	case ID_EXT_BTN_STYLE_TEXT_ONLY_IN_MENUS:
	case ID_EXT_BTN_STYLE_IMAGE_AND_TEXT:
	case ID_EXT_BTN_BEGIN_A_GROUP:
		return false;
	} // switch( pCmdItem->m_nCmdID )
	return true;
}

bool CExtCustomizeSite::OnCanInsertCommandToAllCommandsCategory(
	CExtCmdItem * pCmdItem
	)
{
	ASSERT( this != NULL );
	ASSERT( pCmdItem != NULL );
	return OnCanInsertCommandToAnyCategory( pCmdItem );
}

bool CExtCustomizeSite::OnCanDragCustomizeCommand(
	CExtCmdItem * pCmdItem
	)
{
	ASSERT( this != NULL );
	ASSERT( pCmdItem != NULL );
	ASSERT( CExtCmdManager::IsCommand(pCmdItem->m_nCmdID) );
	if(			CExtCmdManager::IsSystemCommand(pCmdItem->m_nCmdID)
		||	(	CExtCmdManager::IsAfxOleCommand(pCmdItem->m_nCmdID)
//				&& pCmdItem->m_nCmdID != ID_OLE_VERB_FIRST
				)
		||	(	CExtCmdManager::IsAfxMdiWindowsCommand(pCmdItem->m_nCmdID)
//				&& pCmdItem->m_nCmdID != __ID_MDIWNDLIST_FIRST
				)
		||	(	CExtCmdManager::IsAfxFileMruCommand(pCmdItem->m_nCmdID)
//				&& pCmdItem->m_nCmdID != ID_FILE_MRU_FIRST
				)
		)
		return false;
	return true;
}

static int stat_FindBinaryInsertPos(
	const CExtSafeStringArray & _arr,
	const CExtSafeString & _key
	)
{
int nCount = _arr.GetSize();
	if( nCount == 0 )
		return 0;
	if( nCount == 1 )
	{
		const CExtSafeString & _element =
			((CExtSafeStringArray &)_arr).ElementAt( 0 );
		return ( _key < _element ) ? 0 : 1;
	} // if( nCount == 1 )

int nIdx = nCount-1, nIdx2, nMargine1 = 0, nMargine2 = nCount-1;
	for( ; true; )
	{
		const CExtSafeString & _element =
			((CExtSafeStringArray &)_arr).ElementAt( nIdx );
		int nCompareResult = _tcscmp( _key, _element );
		if( nCompareResult == 0 )
			return nIdx;
		if( nCompareResult > 0 )
			nMargine1 = nIdx;
		else
			nMargine2 = nIdx;
		nIdx2 = (nMargine2 - nMargine1)/2 + nMargine1;
		if( nIdx2 == nIdx )
			break;
		nIdx = nIdx2;
	}
const CExtSafeString & _element =
		((CExtSafeStringArray &)_arr).ElementAt( nIdx );
	return ( _key < _element ) ? nIdx : (nIdx+1);
}

void CExtCustomizeSite::CategoryAppendAllCommands()
{
	ASSERT( this != NULL );
	ASSERT_VALID( m_pWndTop );
	ASSERT( m_pWndTop->GetSafeHwnd() != NULL && ::IsWindow(m_pWndTop->GetSafeHwnd()) );

CExtSafeString strAllCommands;
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
CExtLocalResourceHelper _LRH;
	if( ! g_ResourceManager->LoadString( strAllCommands, IDS_EXT_ALL_COMMANDS ) )
	{
		ASSERT( FALSE );
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		strAllCommands = _T("All commands");
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
	}
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
	CategoryUpdate( strAllCommands, (LPUINT)NULL, 0 );
CExtCustomizeCmdTreeNode * pAllCommandsCategoryNode =
		CategoryGetTreeByName( strAllCommands );
	ASSERT_VALID( pAllCommandsCategoryNode );

CExtSafeStringArray _arrNamedNames;
CPtrArray _arrNamedCommands;
CPtrList _listUnnamedCommands;
CExtCmdProfile::cmd_container_t & _mapProfileCommands =
		g_CmdManager->ProfileGetPtr(
			g_CmdManager->ProfileNameFromWnd( m_pWndTop->m_hWnd )
			) -> m_cmds;
	for(	POSITION pos = _mapProfileCommands.GetStartPosition();
			pos != NULL;
		)
	{ // walk all the profile commands
		UINT nCmdID = 0;
		CExtCmdItem * pCmdItem = NULL;
		_mapProfileCommands.GetNextAssoc( pos, nCmdID, pCmdItem );
		ASSERT( CExtCmdManager::IsCommand(nCmdID) );
		ASSERT( pCmdItem != NULL );
		if( !OnCanInsertCommandToAllCommandsCategory(pCmdItem) )
			continue;
		CExtSafeString _strCommandScriptName = pCmdItem->OnGetCustomizeScriptName();
		_strCommandScriptName.Replace( _T("&"), _T("") );
		if( _strCommandScriptName.IsEmpty() )
		{
			_listUnnamedCommands.AddTail( pCmdItem );
			continue;
		} // if( _strCommandScriptName.IsEmpty() )
		int nInsertPos =
			stat_FindBinaryInsertPos(
				_arrNamedNames,
				_strCommandScriptName
				);
#ifdef _DEBUG
		ASSERT( 0 <= nInsertPos && nInsertPos <= _arrNamedNames.GetSize() );
		if(		_arrNamedNames.GetSize() > 1
			&&	nInsertPos < (_arrNamedNames.GetSize() - 1)
			)
		{
			CExtSafeString _str2cmp = _arrNamedNames[nInsertPos];
			ASSERT( _tcscmp( _strCommandScriptName, _str2cmp ) <= 0 );
		}
		if( nInsertPos > 0 )
		{
			CExtSafeString _str2cmp = _arrNamedNames[nInsertPos-1];
			ASSERT( _tcscmp( _str2cmp, _strCommandScriptName ) <= 0 );
		}
#endif // _DEBUG
		_arrNamedNames.InsertAt( nInsertPos, _strCommandScriptName, 1 );
		_arrNamedCommands.InsertAt( nInsertPos, pCmdItem, 1 );
	} // walk all the profile commands
	for( int i = 0; i < _arrNamedCommands.GetSize(); i++ )
	{ // add all the named commands
		CExtSafeString _strName =
			_arrNamedNames[ i ];
		ASSERT( ! _strName.IsEmpty() );
		CExtCmdItem * pCmdItem = (CExtCmdItem *)
			_arrNamedCommands[ i ];
		ASSERT( pCmdItem != NULL );
		CExtCustomizeCmdTreeNode * pNewNode = OnCmdNodeCreate();
		ASSERT_VALID( pNewNode );
		pNewNode->AssignCmdProps( pCmdItem );
		pNewNode->SetTextInToolbar( _strName );
		pNewNode->SetTextInMenu( _strName );
		pAllCommandsCategoryNode->
			InsertNode( this, pNewNode );
	} // add all the named commands
	for(	pos = _listUnnamedCommands.GetHeadPosition();
			pos != NULL;
		)
	{ // add all the unnamed commands
		CExtCmdItem * pCmdItem = (CExtCmdItem *)
			_listUnnamedCommands.GetNext( pos );
		ASSERT( pCmdItem != NULL );
		CExtCustomizeCmdTreeNode * pNewNode = OnCmdNodeCreate();
		ASSERT_VALID( pNewNode );
		pNewNode->AssignCmdProps( pCmdItem );
		pAllCommandsCategoryNode-> InsertNode( this, pNewNode );
	} // add all the unnamed commands
}

void CExtCustomizeSite::CategoryAppendNewMenu()
{
	ASSERT( this != NULL );
	ASSERT_VALID( m_pWndTop );
	ASSERT( m_pWndTop->GetSafeHwnd() != NULL && ::IsWindow(m_pWndTop->GetSafeHwnd()) );

CExtCmdItem * pCmdItem =
		g_CmdManager->CmdGetPtr(
			g_CmdManager->ProfileNameFromWnd( m_pWndTop->m_hWnd ),
			ID_EXT_NEW_MENU
			);
	ASSERT( pCmdItem != NULL );
	ASSERT( ! pCmdItem->m_sMenuText.IsEmpty() );

UINT _arr[1] = { ID_EXT_NEW_MENU };
	_CategoryUpdateImpl( pCmdItem->m_sMenuText, _arr, 1, true );

CCategoryCustomizeInfo * pCategoryInfo =
		_GetCategoryInfo( pCmdItem->m_sMenuText, false );
	ASSERT( pCategoryInfo != NULL );
	pCategoryInfo->m_bVisibleInKeyboardPage = false;

CExtCustomizeCmdTreeNode * pCategoryNode =
		CategoryGetTreeByName( pCmdItem->m_sMenuText );
	ASSERT_VALID( pCategoryNode );
	pCategoryNode->SearchNodeElement( ID_EXT_NEW_MENU )
		-> ModifyFlags( __ECTN_AUTO_POPUP|__ECTN_DYNAMIC_POPUP );
}

int CExtCustomizeSite::CategoryGetCount()
{
	ASSERT( this != NULL );
	return m_listCategories.GetCount();
}

CExtCustomizeCmdTreeNode * CExtCustomizeSite::CategoryGetTreeAt( int nPos )
{
	ASSERT( this != NULL );
	if( nPos < 0 )
		return NULL;
int nCount = m_listCategories.GetCount();
	if( nPos >= nCount )
		return NULL;
int nCountPassed = 0;
	for(	POSITION pos = m_listCategories.GetHeadPosition();
			pos != NULL;
			nCountPassed++
		)
	{ // walk all categories
		CCategoryCustomizeInfo * pCategoryInfo =
			(CCategoryCustomizeInfo *)
				m_listCategories.GetNext( pos );
		ASSERT( pCategoryInfo != NULL );
		ASSERT_VALID( pCategoryInfo->m_pRoot );
		if( nPos == nCountPassed )
			return pCategoryInfo->m_pRoot;
	} // walk all categories
	ASSERT( FALSE );
	return NULL;
}

CExtCustomizeCmdTreeNode * CExtCustomizeSite::CategoryGetTreeByName(
	__EXT_MFC_SAFE_LPCTSTR strCategoryName
	)
{
	ASSERT( this != NULL );
	if( strCategoryName == NULL )
		return NULL;
CCategoryCustomizeInfo * pCategoryInfo =
		_GetCategoryInfo( strCategoryName, false );
	if( pCategoryInfo == NULL )
		return NULL;
	ASSERT_VALID( pCategoryInfo->m_pRoot );
	return pCategoryInfo->m_pRoot;
}

int CExtCustomizeSite::CategoryEnum(
	CExtCustomizeSite::ICustomizeCategoryEnumSite * pCategoryEnumSite
	)
{
	ASSERT( this != NULL );
	if( pCategoryEnumSite == NULL )
		return 0;
int nCountPassed = 0;
	for(	POSITION pos = m_listCategories.GetHeadPosition();
			pos != NULL;
			nCountPassed++
		)
	{ // walk all categories
		CCategoryCustomizeInfo * pCategoryInfo =
			(CCategoryCustomizeInfo *)
				m_listCategories.GetNext( pos );
		ASSERT( pCategoryInfo != NULL );
		ASSERT_VALID( pCategoryInfo->m_pRoot );
		if( !pCategoryEnumSite->OnCustomizeCategoryEnum(
				nCountPassed,
				pCategoryInfo->m_pRoot,
				pCategoryInfo->m_bVisibleInKeyboardPage
				)
			)
			break;
	} // walk all categories
	return nCountPassed;
}

int CExtCustomizeSite::CategoryMakeCmdsUnique(
	__EXT_MFC_SAFE_LPCTSTR strCategoryName
	)
{
	ASSERT( this != NULL );
	if(		strCategoryName == NULL
		||	_tcslen(strCategoryName) == 0
		)
		return 0;
	ASSERT_VALID( m_pWndTop );
	ASSERT( m_pWndTop->GetSafeHwnd() != NULL && ::IsWindow(m_pWndTop->GetSafeHwnd()) );
CCategoryCustomizeInfo * pCategoryInfo =
		_GetCategoryInfo( strCategoryName, false );
	if( pCategoryInfo == NULL )
		return 0;
	return pCategoryInfo->MakeCmdsUnique();
}

int CExtCustomizeSite::CategoryMakeAllCmdsUnique()
{
	ASSERT( this != NULL );
	ASSERT_VALID( m_pWndTop );
	ASSERT( m_pWndTop->GetSafeHwnd() != NULL && ::IsWindow(m_pWndTop->GetSafeHwnd()) );
int nCmdsPassed = 0;
	for(	POSITION pos = m_listCategories.GetHeadPosition();
			pos != NULL;
		)
	{ // walk all categories
		CCategoryCustomizeInfo * pCategoryInfo =
			(CCategoryCustomizeInfo *)
				m_listCategories.GetNext( pos );
		ASSERT( pCategoryInfo != NULL );
		nCmdsPassed += pCategoryInfo->MakeCmdsUnique();
	} // walk all categories
	return nCmdsPassed;
}

int CExtCustomizeSite::CategoryMakeFromTree(
	CExtCustomizeCmdTreeNode * pNodeCategoryRoot,
	__EXT_MFC_SAFE_LPCTSTR strCategoryName // = NULL
	)
{
	ASSERT_VALID( pNodeCategoryRoot );
	ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNodeCategoryRoot );
CString _strCategoryName;
	if( strCategoryName != NULL )
		_strCategoryName = strCategoryName;
	else
		_strCategoryName = pNodeCategoryRoot->GetTextUser();
	ASSERT( ! _strCategoryName.IsEmpty() );
	_CategoryUpdateImpl(
		LPCTSTR(_strCategoryName),
		NULL,
		0
		);
CCategoryCustomizeInfo * pCategoryInfo =
		_GetCategoryInfo( _strCategoryName );
	ASSERT( pCategoryInfo != NULL );
	pCategoryInfo->m_pRoot->RemoveAllNodes();
int nRetVal =
		pCategoryInfo->m_pRoot->InsertClonedChildsNodesFrom(
			this,
			pNodeCategoryRoot
			);
	return nRetVal;
}

int CExtCustomizeSite::CategoryMakeFromTreeN(
	CExtCustomizeCmdTreeNode * pNodeCategoriesRoot
	)
{
	ASSERT_VALID( pNodeCategoriesRoot );
	ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNodeCategoriesRoot );
int nRetVal = 0;
int nNodeCount = pNodeCategoriesRoot->GetNodeCount();
	for( int i = 0; i < nNodeCount; i++ )
	{
		CExtCustomizeCmdTreeNode * pNodeCategoryRoot =
			pNodeCategoriesRoot->ElementAt( i );
		ASSERT_VALID( pNodeCategoryRoot );
		ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNodeCategoryRoot );
		int nUpdatedCmdCount =
			CategoryMakeFromTree( pNodeCategoryRoot );
		nRetVal += nUpdatedCmdCount;
	} // for( int i = 0; i < nNodeCount; i++ )
	return nRetVal;
}

int CExtCustomizeSite::CategoryUpdate(
	__EXT_MFC_SAFE_LPCTSTR strCategoryName,
	LPUINT pCmdList,
	int nCount // = 1
	)
{
	return
		_CategoryUpdateImpl(
			strCategoryName,
			pCmdList,
			nCount,
			false
			);
}

int CExtCustomizeSite::_CategoryUpdateImpl(
	__EXT_MFC_SAFE_LPCTSTR strCategoryName,
	LPUINT pCmdList,
	int nCount, // = 1
	bool bNoCheckCmdInsertAllowed // = false
	)
{
	ASSERT( this != NULL );
	if(		strCategoryName == NULL
		||	_tcslen(strCategoryName) == 0
		||	nCount < 0
		||	( (pCmdList == NULL) && nCount > 0 )
		)
		return 0;
	ASSERT( ( (pCmdList == NULL) && nCount == 0 ) || nCount > 0 );
	ASSERT_VALID( m_pWndTop );
	ASSERT( m_pWndTop->GetSafeHwnd() != NULL && ::IsWindow(m_pWndTop->GetSafeHwnd()) );
CExtSafeString strProfileName =
		g_CmdManager->ProfileNameFromWnd( m_pWndTop->m_hWnd );
	if( strProfileName.IsEmpty() )
	{
		ASSERT( FALSE );
		return 0;
	}
CCategoryCustomizeInfo * pCategoryInfo =
		_GetCategoryInfo( strCategoryName );
int nCountUpdatedItems = 0;
	for( int nItem = 0; nItem < nCount; nItem++ )
	{
		if(		(! CExtCmdManager::IsCommand(pCmdList[nItem]) )
			||	CExtCmdManager::IsSystemCommand(pCmdList[nItem])
			||	CExtCmdManager::IsAfxOleCommand(pCmdList[nItem])
			||	CExtCmdManager::IsAfxMdiWindowsCommand(pCmdList[nItem])
			||	CExtCmdManager::IsAfxFileMruCommand(pCmdList[nItem])
			)
			continue;
		CExtCmdItem * pCmdItem =
			g_CmdManager->CmdGetPtr(
				strProfileName,
				pCmdList[nItem]
				);
		if( pCmdItem == NULL )
			continue;
		if( !bNoCheckCmdInsertAllowed )
		{
			if( !OnCanInsertCommandToAnyCategory(pCmdItem) )
				continue;
		} // if( !bNoCheckCmdInsertAllowed )
		CExtCustomizeCmdTreeNode * pNewNode = OnCmdNodeCreate();
		ASSERT_VALID( pNewNode );
		pNewNode->AssignCmdProps( pCmdItem );
		pCategoryInfo->m_pRoot->InsertNode( this, pNewNode );
	} // for( int nItem = 0; nItem < nCount; nItem++ )
	return nCountUpdatedItems;
}

int CExtCustomizeSite::CategoryUpdate(
	UINT nMenuResourceID,
	bool bPopupMenu // = false
	)
{
	ASSERT( this != NULL );
	return CategoryUpdate(
		MAKEINTRESOURCE(nMenuResourceID),
		bPopupMenu
		);
}

int CExtCustomizeSite::CategoryUpdate(
	__EXT_MFC_SAFE_LPCTSTR strMenuResourceID,
	bool bPopupMenu // = false
	)
{
	ASSERT( this != NULL );
	if( strMenuResourceID == NULL )
		return 0;
CMenu menu;
	if( ! g_ResourceManager->LoadMenu( menu, strMenuResourceID ) )
	{
		ASSERT( FALSE );
		return 0;
	}
	return CategoryUpdate( &menu, bPopupMenu );
}

bool CExtCustomizeSite::CategoryRemove(
	__EXT_MFC_SAFE_LPCTSTR strCategoryName
	)
{
	ASSERT( this != NULL );
	if(		strCategoryName == NULL
		||	_tcslen(strCategoryName) == 0
		)
		return false;
	for(	POSITION pos = m_listCategories.GetHeadPosition();
			pos != NULL;
		)
	{ // walk all categories
		POSITION posToRemove = pos;
		CCategoryCustomizeInfo * pCategoryInfo =
			(CCategoryCustomizeInfo *)
				m_listCategories.GetNext( pos );
		ASSERT( pCategoryInfo != NULL );
		ASSERT_VALID( pCategoryInfo->m_pRoot );
		if(	_tcscmp(
				pCategoryInfo->m_pRoot->GetTextUser(),
				strCategoryName
				) != 0
			)
			continue;
		delete pCategoryInfo;
		m_listCategories.RemoveAt( posToRemove );
		return true;
	} // walk all categories
	return false;
}

void CExtCustomizeSite::CategoryRemoveAll()
{
	ASSERT( this != NULL );
	for(	POSITION pos = m_listCategories.GetHeadPosition();
			pos != NULL;
		)
	{ // walk all categories
		CCategoryCustomizeInfo * pCategoryInfo =
			(CCategoryCustomizeInfo *)
				m_listCategories.GetNext( pos );
		ASSERT( pCategoryInfo != NULL );
		delete pCategoryInfo;
	} // walk all categories
	m_listCategories.RemoveAll();
}

int CExtCustomizeSite::CategoryUpdate(
	CMenu * pMenu,
	bool bPopupMenu // = false
	)
{
	ASSERT( this != NULL );
	if(		pMenu == NULL
		|| (! ::IsMenu(pMenu->GetSafeHmenu()) )
		)
		return 0;
	if( bPopupMenu )
	{
		pMenu = pMenu->GetSubMenu( 0 );
		if(		pMenu == NULL
			|| (! ::IsMenu(pMenu->GetSafeHmenu()) )
			)
		{
			ASSERT( FALSE );
			return 0;
		}
	} // if( bPopupMenu )
int	nMenuItemCount = pMenu->GetMenuItemCount();
int nCountUpdatedItems = 0;
	for( int nItemIndex=0; nItemIndex<nMenuItemCount; nItemIndex++ )
	{
		INT nMenuItemID = (INT)
			pMenu->GetMenuItemID( nItemIndex );
		if( nMenuItemID != -1 )
			continue;
		MENUITEMINFO mii;
		::memset( &mii, 0, sizeof(MENUITEMINFO) );
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask =
			MIIM_CHECKMARKS|MIIM_DATA|MIIM_ID|MIIM_STATE
			|MIIM_SUBMENU|MIIM_TYPE;
		mii.cch = __MAX_UI_ITEM_TEXT;
		CExtSafeString sMenuItemText;
		mii.dwTypeData = sMenuItemText.GetBuffer( __MAX_UI_ITEM_TEXT );
		ASSERT( mii.dwTypeData != NULL );
		if( mii.dwTypeData == NULL )
		{
			ASSERT( FALSE );
			continue;
		}
		if( !pMenu->GetMenuItemInfo( nItemIndex, &mii, TRUE ) )
		{
			sMenuItemText.ReleaseBuffer();
			ASSERT( FALSE );
			continue;
		} // if( !pMenu->GetMenuItemInfo( nItemIndex, &mii, TRUE ) )
		sMenuItemText.ReleaseBuffer();
		sMenuItemText.Replace( _T("&"), _T("") );
		if( sMenuItemText.IsEmpty() )
		{
			ASSERT( FALSE );
			continue;
		}
		CMenu * pSubMenu = pMenu->GetSubMenu( nItemIndex );
		if(		pSubMenu == NULL
			|| (! ::IsMenu(pSubMenu->GetSafeHmenu()) )
			)
		{
			ASSERT( FALSE );
			continue;
		}
		nCountUpdatedItems +=
			_CategoryUpdateFromMenuImpl(
				sMenuItemText,
				pSubMenu
				);
	} // for( int nItemIndex=0; nItemIndex<nMenuItemCount; nItemIndex++ )
	return nCountUpdatedItems;
}

CExtCustomizeSite::CCategoryCustomizeInfo *
	CExtCustomizeSite::_GetCategoryInfo(
		__EXT_MFC_SAFE_LPCTSTR strCategoryName,
		bool bAllowRegisterNew // = true
		)
{
	ASSERT( this != NULL );
	ASSERT( strCategoryName != NULL && _tcslen(strCategoryName) > 0 );
	for(	POSITION pos = m_listCategories.GetHeadPosition();
			pos != NULL;
		)
	{ // walk all categories
		CCategoryCustomizeInfo * pCategoryInfo =
			(CCategoryCustomizeInfo *)
				m_listCategories.GetNext( pos );
		ASSERT( pCategoryInfo != NULL );
		ASSERT_VALID( pCategoryInfo->m_pRoot );
		if( _tcscmp(
				pCategoryInfo->m_pRoot->GetTextUser(),
				strCategoryName
				) == 0
			)
			return pCategoryInfo;
	} // walk all categories
	if( !bAllowRegisterNew )
		return NULL;
CCategoryCustomizeInfo * pCategoryInfo =
		new CCategoryCustomizeInfo( strCategoryName, this );
	m_listCategories.AddTail( pCategoryInfo );
	return pCategoryInfo;
}

int CExtCustomizeSite::_CategoryUpdateFromMenuImpl(
	__EXT_MFC_SAFE_LPCTSTR strCategoryName,
	CMenu * pMenu
	)
{
	ASSERT( this != NULL );
	ASSERT( strCategoryName != NULL && _tcslen(strCategoryName) > 0 );
	ASSERT( pMenu != NULL );
	ASSERT( ::IsMenu(pMenu->GetSafeHmenu()) );
int	nMenuItemCount = pMenu->GetMenuItemCount();
int nCountUpdatedItems = 0;
	for( int nItemIndex=0; nItemIndex<nMenuItemCount; nItemIndex++ )
	{
		MENUITEMINFO mii;
		::memset( &mii, 0, sizeof(MENUITEMINFO) );
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask =
			MIIM_CHECKMARKS|MIIM_DATA|MIIM_ID|MIIM_STATE
			|MIIM_SUBMENU|MIIM_TYPE;
		mii.cch = __MAX_UI_ITEM_TEXT;
		CExtSafeString sMenuItemText;
		mii.dwTypeData = sMenuItemText.GetBuffer( __MAX_UI_ITEM_TEXT );
		ASSERT( mii.dwTypeData != NULL );
		if( mii.dwTypeData == NULL )
		{
			ASSERT( FALSE );
			continue;
		}
		if( !pMenu->GetMenuItemInfo( nItemIndex, &mii, TRUE ) )
		{
			sMenuItemText.ReleaseBuffer();
			ASSERT( FALSE );
			continue;
		} // if( !pMenu->GetMenuItemInfo( nItemIndex, &mii, TRUE ) )
		sMenuItemText.ReleaseBuffer();
		if( (mii.fType&MFT_SEPARATOR) != 0 )
			continue;
		if( mii.hSubMenu != NULL )
		{
			CMenu * pSubMenu = pMenu->GetSubMenu( nItemIndex );
			if(		pSubMenu == NULL
				|| (! ::IsMenu(pSubMenu->GetSafeHmenu()) )
				)
			{
				ASSERT( FALSE );
				continue;
			}
			nCountUpdatedItems +=
				_CategoryUpdateFromMenuImpl(
					strCategoryName,
					pSubMenu
					);
			continue;
		}
		nCountUpdatedItems +=
			CategoryUpdate(
				strCategoryName,
				&mii.wID,
				1
				);
	} // for( int nItemIndex=0; nItemIndex<nMenuItemCount; nItemIndex++ )
	return nCountUpdatedItems;
}

CWnd * CExtCustomizeSite::GetCustomizeTopWnd()
{
	ASSERT( this != NULL );
	if( !m_bInitComplete )
		return NULL;
	ASSERT_VALID( m_pWndTop );
	ASSERT( m_pWndTop->GetSafeHwnd() != NULL && ::IsWindow(m_pWndTop->GetSafeHwnd()) );
	return m_pWndTop;
}

DWORD CExtCustomizeSite::GetCustomizeFlags() const
{
	ASSERT( this != NULL );
	return m_dwCustomizeFlags;
}

bool CExtCustomizeSite::IsCustomizationInabled() const
{
	ASSERT( this != NULL );
	if( !m_bInitComplete )
		return false;
	return (m_dwCustomizeFlags == 0) ? false : true;
}

bool CExtCustomizeSite::EnableCustomization(
	CWnd * pWndTop,
	DWORD dwCustomizeFlags // = __ECSC_DEFAULT
	)
{
	ASSERT( this != NULL );
	if(		m_bInitComplete
		||	dwCustomizeFlags == 0
		||	pWndTop->GetSafeHwnd() == NULL
		||	(! ::IsWindow( pWndTop->GetSafeHwnd() ) )
		)
	{
		// should not:
		// a) be called twice
		// b) have zero flags
		ASSERT( FALSE );
		return false;
	}
	ASSERT( m_pWndTop == NULL );
	ASSERT( !m_bCustomizeMode );
	if( m_cf == NULL )
	{
		m_cf = _RegisterClipFormat( pWndTop->m_hWnd );
		if( m_cf == NULL )
		{
			ASSERT( FALSE );
			return false;
		}
	} // if( m_cf == NULL )

	CategoryRemoveAll();

CExtSafeString strProfileName =
		g_CmdManager->ProfileNameFromWnd( pWndTop->m_hWnd );
	ASSERT( !strProfileName.IsEmpty() );
	if(		(!	g_CmdManager->UpdateFromMenu(
				strProfileName,
				IDR_MENU_CUSTOMIZE_CMDS
				)
			)
		||	(!	g_CmdManager->UpdateFromMenu(
				strProfileName,
				IDR_MENU_CUSTOMIZE_BTN
				)
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}

	g_CmdManager->CmdGetPtr( strProfileName, ID_EXT_NEW_MENU )->StateSetDynamicPopup();
	
	g_CmdManager->CmdGetPtr( strProfileName, ID_EXT_NEW_MENU )->StateSetBasic();
	g_CmdManager->CmdGetPtr( strProfileName, ID_EXT_CUSTOMIZE )->StateSetBasic();
	g_CmdManager->CmdGetPtr( strProfileName, ID_EXT_RESET_TOOLBAR )->StateSetBasic();
	g_CmdManager->CmdGetPtr( strProfileName, ID_EXT_BTN_RESET )->StateSetBasic();
	g_CmdManager->CmdGetPtr( strProfileName, ID_EXT_BTN_DELETE )->StateSetBasic();
	g_CmdManager->CmdGetPtr( strProfileName, ID_EXT_BTN_NAME )->StateSetBasic();
	g_CmdManager->CmdGetPtr( strProfileName, ID_EXT_BTN_IMAGE_COPY )->StateSetBasic();
	g_CmdManager->CmdGetPtr( strProfileName, ID_EXT_BTN_IMAGE_PASTE )->StateSetBasic();
	g_CmdManager->CmdGetPtr( strProfileName, ID_EXT_BTN_IMAGE_RESET )->StateSetBasic();
	g_CmdManager->CmdGetPtr( strProfileName, ID_EXT_BTN_IMAGE_EDIT )->StateSetBasic();
	g_CmdManager->CmdGetPtr( strProfileName, ID_EXT_BTN_STYLE_DEFAULT )->StateSetBasic();
	g_CmdManager->CmdGetPtr( strProfileName, ID_EXT_BTN_STYLE_TEXT_ONLY_ALWAYS )->StateSetBasic();
	g_CmdManager->CmdGetPtr( strProfileName, ID_EXT_BTN_STYLE_TEXT_ONLY_IN_MENUS )->StateSetBasic();
	g_CmdManager->CmdGetPtr( strProfileName, ID_EXT_BTN_STYLE_IMAGE_AND_TEXT )->StateSetBasic();
	g_CmdManager->CmdGetPtr( strProfileName, ID_EXT_BTN_BEGIN_A_GROUP )->StateSetBasic();

	m_dwCustomizeFlags = dwCustomizeFlags;
	m_pWndTop = pWndTop;
	ASSERT( m_hWndHelperPopupHook == NULL );
	if( (m_pWndTop->GetStyle()&WS_CHILD) != 0 )
	{
		CWnd * pWnd = m_pWndTop->GetParent();
		for( ; pWnd != NULL; pWnd = pWnd->GetParent() )
		{
			if( (pWnd->GetStyle()&WS_CHILD) == 0 )
			{
				m_hWndHelperPopupHook = pWnd->m_hWnd;
				g_mapHelperPopupHooks.SetAt( m_hWndHelperPopupHook, this );
				break;
			} // if( (pWnd->GetStyle()&WS_CHILD) == 0 )
		} // for( ; pWnd != NULL; pWnd = pWnd->GetParent() )
	} // if( (m_pWndTop->GetStyle()&WS_CHILD) != 0 )

	if( pWndTop->IsKindOf(RUNTIME_CLASS(CFrameWnd)) )
	{
		for(	POSITION pos =
					((CFrameWnd*)pWndTop)->m_listControlBars.
						GetHeadPosition();
				pos != NULL;
			)
		{ // walk all frame bars
			CControlBar * pBarTemp = (CControlBar *)
				((CFrameWnd*)pWndTop)->m_listControlBars.
					GetNext( pos );
			ASSERT_VALID( pBarTemp );
			ASSERT_KINDOF( CControlBar, pBarTemp );
			CExtToolControlBar * pToolBar =
				DYNAMIC_DOWNCAST( CExtToolControlBar, pBarTemp );
			if( pToolBar == NULL )
				continue;
			OnRegisterToolBar( pToolBar );
		} // walk all frame bars
	} // if( pWndTop->IsKindOf(RUNTIME_CLASS(CFrameWnd)) )
	else
	{
		for(	HWND hWndChild = ::GetWindow( pWndTop->m_hWnd, GW_CHILD );
				hWndChild != NULL;
				hWndChild = ::GetWindow( hWndChild, GW_HWNDNEXT )
			)
		{ // walk all child windows
			CWnd * pWndTemp = CWnd::FromHandlePermanent( hWndChild );
			if( pWndTemp == NULL )
				continue;
			ASSERT_VALID( pWndTemp );
			CExtToolControlBar * pToolBar =
				DYNAMIC_DOWNCAST( CExtToolControlBar, pWndTemp );
			if( pToolBar == NULL )
				continue;
			OnRegisterToolBar( pToolBar );
		} // walk all child windows
	} // else from if( pWndTop->IsKindOf(RUNTIME_CLASS(CFrameWnd)) )
	OnRegisterAdditionalToolBars();
	if( m_pWndMenuBar != NULL )
	{
		ASSERT_VALID( m_pWndMenuBar );
		m_pWndMenuBar->UpdateMenuBar();
	}
	if( ! CExtHookSink::SetupHookWndSink(pWndTop->m_hWnd) )
	{
		ASSERT( FALSE );
		_EmptyContainers();
		m_pWndTop = NULL;
		m_dwCustomizeFlags = 0L;
		return false;
	} // if( ! CExtHookSink::SetupHookWndSink(pWndTop->m_hWnd) )
	if( m_hWndHelperPopupHook != NULL )
	{
		LPCTSTR strProfileName = g_CmdManager->ProfileNameFromWnd( m_pWndTop->m_hWnd );
		ASSERT( strProfileName != NULL );
		g_CmdManager->ProfileWndAdd( strProfileName, m_hWndHelperPopupHook );
//		if( ! CExtHookSink::SetupHookWndSink(m_hWndHelperPopupHook) )
//		{
//			CExtHookSink::SetupHookWndSink(pWndTop->m_hWnd,true);
//			ASSERT( FALSE );
//			_EmptyContainers();
//			m_pWndTop = NULL;
//			m_dwCustomizeFlags = 0L;
//			return false;
//		} // if( ! CExtHookSink::SetupHookWndSink(m_hWndHelperPopupHook) )
	} // if( m_hWndHelperPopupHook != NULL )

	if( m_pWndTop->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		m_hWndMdiClient = ::GetDlgItem(m_pWndTop->m_hWnd,AFX_IDW_PANE_FIRST);
		ASSERT( m_hWndMdiClient != NULL && ::IsWindow(m_hWndMdiClient) );
		if( !CExtHookSink::SetupHookWndSink(m_hWndMdiClient) )
		{
			ASSERT( FALSE );
			VERIFY( CExtHookSink::SetupHookWndSink(m_pWndTop->m_hWnd,true) );
			_EmptyContainers();
			m_hWndMdiClient = NULL;
			m_pWndTop = NULL;
			m_dwCustomizeFlags = 0L;
			return false;
		} // if( !CExtHookSink::SetupHookWndSink(m_hWndMdiClient) )
	} // if( m_pWndTop->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	m_bInitComplete = true;
	if( (GetCustomizeFlags()&__ECSF_ACCELERATORS) != 0 )
		OnUpdateAccelGlobalInfo( true );
	return true;
}

bool CExtCustomizeSite::IsCustomizeMode() const
{
	ASSERT( this != NULL );
	if( !m_bInitComplete )
		return false;
	return m_bCustomizeMode;
}

HWND CExtCustomizeSite::GetSafeCustomizeFormHWND()
{
	ASSERT( this != NULL );
	if(		m_hWndCustomizeForm == NULL
		||	(! ::IsWindow(m_hWndCustomizeForm) )
		)
		return NULL;
	return m_hWndCustomizeForm;
}

CExtCustomizeSite::CCmdDragInfo::CCmdDragInfo(
	CExtCustomizeCmdTreeNode * pDropNodeI, // = NULL
	CExtCustomizeCmdTreeNode * pDropNodeC // = NULL
	)
	: m_pDropNodeI( pDropNodeI )
	, m_pDropNodeC( pDropNodeC )
{
#ifdef _DEBUG
	if( m_pDropNodeI != NULL  )
	{
		ASSERT_VALID( m_pDropNodeI );
		ASSERT_KINDOF( CExtCustomizeCmdTreeNode, m_pDropNodeI );
	} // if( m_pDropNodeI != NULL  )
	if( m_pDropNodeC != NULL  )
	{
		ASSERT_VALID( m_pDropNodeC );
		ASSERT_KINDOF( CExtCustomizeCmdTreeNode, m_pDropNodeC );
	} // if( m_pDropNodeC != NULL  )
#endif // _DEBUG
}

CExtCustomizeSite::CCmdDragInfo::CCmdDragInfo(
	COleDataObject * pDataObject,
	CLIPFORMAT cf
	)
	: m_pDropNodeI( NULL )
	, m_pDropNodeC( NULL )
{
	if( pDataObject == NULL )
		return;
CFile * pFile = pDataObject->GetFileData( cf );
	if( pFile == NULL )
		return;
CArchive ar( pFile, CArchive::load );
UINT nCmdID = 0;
	try
	{
		ar.Read(
			(LPVOID)&m_pDropNodeI,
			sizeof( CExtCustomizeCmdTreeNode * )
			);
#ifdef _DEBUG
		if( m_pDropNodeI != NULL  )
		{
			ASSERT_VALID( m_pDropNodeI );
			ASSERT_KINDOF( CExtCustomizeCmdTreeNode, m_pDropNodeI );
		} // if( m_pDropNodeI != NULL  )
#endif // _DEBUG
		ar.Read(
			(LPVOID)&m_pDropNodeC,
			sizeof( CExtCustomizeCmdTreeNode * )
			);
		ASSERT_VALID( m_pDropNodeC );
		ASSERT_KINDOF( CExtCustomizeCmdTreeNode, m_pDropNodeC );
	} // try
	catch( CException * pXept )
	{
		ASSERT( FALSE );
		pXept->Delete();
		nCmdID = 0;
	} // catch( CException * pXept )
	catch( ... )
	{
		ASSERT( FALSE );
		nCmdID = 0;
	} // catch( ... )
	ar.Close();
	delete pFile;
}

CExtCustomizeSite::CCmdDragInfo::CCmdDragInfo(
	CExtCustomizeSite::CCmdDragInfo & other
	)
	: m_pDropNodeI( other.m_pDropNodeI )
	, m_pDropNodeC( other.m_pDropNodeC )
{
}

CExtCustomizeSite::CCmdDragInfo &
	CExtCustomizeSite::CCmdDragInfo::operator=(
		CExtCustomizeSite::CCmdDragInfo & other
		)
{
	m_pDropNodeI = other.m_pDropNodeI;
	m_pDropNodeC = other.m_pDropNodeC;
	return (*this);
}

bool CExtCustomizeSite::CCmdDragInfo::IsEmpty() const
{
	if( m_pDropNodeI == NULL && m_pDropNodeC == NULL )
		return true;
#ifdef _DEBUG
	if( m_pDropNodeI != NULL  )
	{
		ASSERT_VALID( m_pDropNodeI );
		ASSERT_KINDOF( CExtCustomizeCmdTreeNode, m_pDropNodeI );
	} // if( m_pDropNodeI != NULL  )
	ASSERT_VALID( m_pDropNodeC );
	ASSERT_KINDOF( CExtCustomizeCmdTreeNode, m_pDropNodeC );
#endif // _DEBUG
	return false;
}

void CExtCustomizeSite::CCmdDragInfo::CacheGlobalData(
	COleDataSource & _oleDataSource,
	CLIPFORMAT cf
	)
{
	CSharedFile _file;
	CArchive ar( &_file, CArchive::store );
#ifdef _DEBUG
	ASSERT( !IsEmpty() );
	if( m_pDropNodeI != NULL  )
	{
		ASSERT_VALID( m_pDropNodeI );
		ASSERT_KINDOF( CExtCustomizeCmdTreeNode, m_pDropNodeI );
	} // if( m_pDropNodeI != NULL  )
	ASSERT_VALID( m_pDropNodeC );
	ASSERT_KINDOF( CExtCustomizeCmdTreeNode, m_pDropNodeC );
#endif // _DEBUG
	ar.Write(
		(LPCVOID)&m_pDropNodeI,
		sizeof( CExtCustomizeCmdTreeNode * )
		);
	ar.Write(
		(LPCVOID)&m_pDropNodeC,
		sizeof( CExtCustomizeCmdTreeNode * )
		);
	ar.Close();
	_oleDataSource.CacheGlobalData(
		cf,
		_file.Detach()
		);
}

CLIPFORMAT CExtCustomizeSite::_RegisterClipFormat( HWND hWndTop )
{
	ASSERT( hWndTop != NULL && ::IsWindow(hWndTop) );
CExtSafeString strClipFormat;
	strClipFormat.Format(
		_T("Prof-UIS-Command-%lX-%lX-%lX-%lX"),
		g_CmdManager.GetVersionDWORD(true),
		(ULONG) ::GetCurrentProcessId(),
		(ULONG) ::GetCurrentThreadId(),
		(ULONG) hWndTop
		);
CLIPFORMAT cf = (CLIPFORMAT) ::RegisterClipboardFormat( strClipFormat );
	ASSERT( cf != NULL );
	return cf;
}

LPVOID CExtCustomizeSite::ActiveItemSet(
	LPVOID pActiveItem, // = NULL
	HWND hWndActiveItemInvalidate // = NULL
	)
{
	ASSERT( this != NULL );
	ASSERT(
		(pActiveItem == NULL && hWndActiveItemInvalidate == NULL)
		||	pActiveItem != NULL
		);
LPVOID pActiveItemOld = m_pActiveItem;
HWND hWnd = m_hWndActiveItemInvalidate;
	m_pActiveItem = pActiveItem;
	m_hWndActiveItemInvalidate = hWndActiveItemInvalidate;
	if(		hWnd != NULL
		&&	::IsWindow( hWnd )
		&&	::IsWindowVisible( hWnd )
		)
		::InvalidateRect( hWnd, NULL, TRUE );
	return pActiveItemOld;
}

LPVOID CExtCustomizeSite::ActiveItemGet(
	HWND * p_hWnd // = NULL
	)
{
	ASSERT( this != NULL );
	ASSERT(
		(m_pActiveItem == NULL && m_hWndActiveItemInvalidate == NULL)
		||	m_pActiveItem != NULL
		);
	if( p_hWnd != NULL )
		*p_hWnd = m_hWndActiveItemInvalidate;
	return m_pActiveItem;
}

bool CExtCustomizeSite::IsActiveItem( LPVOID pItem )
{
	ASSERT( this != NULL );
LPVOID pActiveItem = ActiveItemGet();
	if( pItem == NULL || pActiveItem == NULL )
		return false;
	return (pActiveItem == pItem) ? true : false;
}

CExtToolControlBar * CExtCustomizeSite::BarDefCmdTargetGet()
{
	ASSERT( this != NULL );
#ifdef _DEBUG
	if( m_pBarDefCmdTarget != NULL )
	{
		ASSERT_VALID( m_pBarDefCmdTarget );
	}
#endif // _DEBUG
	return m_pBarDefCmdTarget;
}

CExtToolControlBar * CExtCustomizeSite::BarDefCmdTargetSet(
	CExtToolControlBar * pBar
	)
{
	ASSERT( this != NULL );
	if( ((LPVOID)m_pBarDefCmdTarget) == ((LPVOID)pBar) )
		return pBar;
CExtToolControlBar * pBarOld = m_pBarDefCmdTarget;
	if( BarIsRegistered(pBar) )
		m_pBarDefCmdTarget = pBar;
	else
		m_pBarDefCmdTarget = NULL;
#ifdef _DEBUG
	if( m_pBarDefCmdTarget != NULL )
	{
		ASSERT_VALID( m_pBarDefCmdTarget );
	}
#endif // _DEBUG
	return pBarOld;
}

bool CExtCustomizeSite::BarIsRegistered(
	CExtToolControlBar * pBar
	)
{
	ASSERT( this != NULL );
	ASSERT_VALID( pBar );
	if( ((LPVOID)m_pWndMenuBar) == ((LPVOID)pBar) )
		return true;
CToolBarCustomizeInfo * pInfo = NULL;
	if( !m_mapToolBars.Lookup( (void *)pBar, (void * &)pInfo ) )
		return false;
	ASSERT( pInfo != NULL );
	return true;
}

CExtCustomizeCmdTreeNode *
	CExtCustomizeSite::FindDynamicPopupInitialState(
		UINT nCmdID
		)
{
	ASSERT( this != NULL );
	ASSERT( CExtCmdManager::IsCommand(nCmdID) );
	for( INT i = 0; i < m_arrMenuInfo.GetSize(); i++ )
	{
		CCmdMenuInfo * pCmdMenuInfo = (CCmdMenuInfo *)
			m_arrMenuInfo[i];
		ASSERT( pCmdMenuInfo != NULL );
		if( pCmdMenuInfo->GetNode(true) == NULL )
			continue;
		ASSERT_VALID( pCmdMenuInfo->GetNode(true) );
		ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pCmdMenuInfo->GetNode(true) );
		CExtCustomizeCmdTreeNode * pNodeFound =
			pCmdMenuInfo->GetNode(true)->SearchNodeElement( nCmdID, -1, false, true );
		if(		pNodeFound != NULL
			&&	(pNodeFound->GetFlags() & __ECTN_DYNAMIC_POPUP) != 0
			)
		{
			ASSERT_VALID( pNodeFound );
			ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNodeFound );
			return pNodeFound;
		}
	} // for( INT i = 0; i < m_arrMenuInfo.GetSize(); i++ )
	if( m_pNodeToolbarsI == NULL )
		return NULL;
	ASSERT_VALID( m_pNodeToolbarsI );
	ASSERT_KINDOF( CExtCustomizeCmdTreeNode, m_pNodeToolbarsI );
int nNodeCount = m_pNodeToolbarsI->GetNodeCount();
	for( i = 0; i < nNodeCount; i++ )
	{
		CExtCustomizeCmdTreeNode * pBarNode =
			m_pNodeToolbarsI->ElementAt( i );
		ASSERT_VALID( pBarNode );
		ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pBarNode );
		CExtCustomizeCmdTreeNode * pNodeFound =
			pBarNode->SearchNodeElement( nCmdID, -1, false, true );
		if(		pNodeFound != NULL
			&&	(pNodeFound->GetFlags() & __ECTN_DYNAMIC_POPUP) != 0
			)
		{
			ASSERT_VALID( pNodeFound );
			ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNodeFound );
			return pNodeFound;
		}
	} // for( i = 0; i < nNodeCount; i++ )
	return NULL;
}

bool CExtCustomizeSite::BarReset(
	CExtToolControlBar * pBar,
	bool bRecalcLayout // = true
	)
{
	ASSERT( this != NULL );
	ASSERT_VALID( pBar );
bool bUserBar = false;
CExtCustomizeCmdTreeNode * pNodeC = NULL, * pNodeI = NULL;
	if(		(! BarIsRegistered(pBar) )
		||	(! GetToolbarCustomizeInfo(
				pBar, &pNodeC, &pNodeI, NULL, &bUserBar
				) )
		||	pNodeC == NULL
		||	pNodeI == NULL
		)
	{
		ASSERT( FALSE );
		return false;
	}

	DropTargetPopupCancelEx( -2 );

	if( ! bUserBar )
	{
		ASSERT_VALID( pNodeC );
		ASSERT_VALID( pNodeI );
		pBar->SetButtons();
		pNodeC->RemoveAllNodes();
		pNodeC->InsertClonedChildsNodesFrom( this, pNodeI );
		pBar->SetButtons( pNodeC );
		if( pBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) )
			((CExtMenuControlBar*)pBar)->UpdateMenuBar( bRecalcLayout ? TRUE : FALSE );
	} // if( ! bUserBar )
	else
	{
		ASSERT( ! pBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) );
		ASSERT_VALID( pNodeC );
		pBar->SetButtons();
		pBar->SetButtons( pNodeC );
	} // else from if( ! bUserBar )

	if( !bRecalcLayout )
		return true;
	if( pBar->m_bPresubclassDialogMode )
		return true;
CFrameWnd * pFrame = pBar->GetParentFrame();
	if( pFrame != NULL )
	{
		ASSERT_VALID( pFrame );
		pFrame->DelayRecalcLayout();
		pBar->SetWindowPos(
			NULL, 0, 0, 0, 0,
			SWP_NOSIZE|SWP_NOMOVE
				|SWP_NOZORDER|SWP_NOOWNERZORDER
				|SWP_NOREPOSITION
				|SWP_FRAMECHANGED
			);

	} // if( pFrame != NULL )
	return true;
}

bool CExtCustomizeSite::BarResetAsk(
	CExtToolControlBar * pBar
	)
{
	ASSERT( this != NULL );
	ASSERT_VALID( pBar );
CString strQst, strFmt, strBarName;
	pBar->GetWindowText( strBarName );
	{ // BLOCK
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		CExtLocalResourceHelper _LRH;
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		if( ! g_ResourceManager->LoadString( strFmt, IDS_EXT_RESET_TOOLBAR_FMT ) )
		{
			ASSERT( FALSE );
			strFmt = _T("Reset changes made to '%s' toolbar?");
		}
	} // BLOCK
	strQst.Format( (LPCTSTR)strFmt, strBarName );
	if( ::AfxMessageBox( strQst, MB_YESNO ) != IDYES )
		return false;
	return true;
}

bool CExtCustomizeSite::IsUserBarCommand(
	UINT nCmdID
	)
{
	ASSERT( this != NULL );
CExtToolControlBar * pBar = NULL;
	if( m_mapHelperUserBars.Lookup( (WORD)nCmdID, (void * &)pBar ) )
	{
		ASSERT_VALID( pBar );
		return true;
	}
	return false;
}

CExtCustomizeCmdTreeNode * CExtCustomizeSite::CustomizedNodeGet()
{
	ASSERT( this != NULL && m_bInitComplete );
#ifdef _DEBUG
	if( m_pNodeCustomized != NULL )
	{
		ASSERT_VALID( m_pNodeCustomized );
	}
#endif // _DEBUG
	return m_pNodeCustomized;
}

bool CExtCustomizeSite::CustomizedNodeSet(
	CExtCustomizeCmdTreeNode * pNode // = NULL
	)
{
	ASSERT( this != NULL && m_bInitComplete );
	if(		m_pNodeCustomized != NULL
		&&	pNode != NULL
		)
		return false;
	m_pNodeCustomized = pNode;
#ifdef _DEBUG
	if( m_pNodeCustomized != NULL )
	{
		ASSERT_VALID( m_pNodeCustomized );
	}
#endif // _DEBUG
	return true;
}

CExtCustomizeCmdTreeNode * CExtCustomizeSite::DraggedNodeGet()
{
	ASSERT( this != NULL );
	if( !m_bInitComplete )
		return NULL;
#ifdef _DEBUG
	if( m_pHelperDragSourceNode != NULL )
	{
		ASSERT_VALID( m_pHelperDragSourceNode );
	}
#endif // _DEBUG
	return m_pHelperDragSourceNode;
}

CExtCustomizeCmdTreeNode * CExtCustomizeSite::DraggedNodeSet(
	CExtCustomizeCmdTreeNode * pNode // = NULL
	)
{
	ASSERT( this != NULL );
	if( !m_bInitComplete )
		return NULL;
CExtCustomizeCmdTreeNode * pOldNode = m_pHelperDragSourceNode;
	m_pHelperDragSourceNode = pNode;
#ifdef _DEBUG
	if( m_pHelperDragSourceNode != NULL )
	{
		ASSERT_VALID( m_pHelperDragSourceNode );
	}
#endif // _DEBUG
	return pOldNode;
}

bool CExtCustomizeSite::DoDragCmdNode(
	CExtCustomizeSite::ICustomizeDropSource * pCustomizeDragSource,
	CExtCustomizeCmdTreeNode * pNodeI,
	CExtCustomizeCmdTreeNode * pNodeC,
	RECT & rcItem
	)
{
	ASSERT( this != NULL );
	if(		(!m_bInitComplete)
		||	pCustomizeDragSource == NULL
		||	pNodeC == NULL
		||	(GetCustomizeFlags() & __ECSF_COMMANDS) == 0
		)
		return false;
	if( (GetCustomizeFlags() & __ECSF_COMMANDS) == 0 )
		return true;
#ifdef _DEBUG
	if( pNodeI != NULL )
	{
		ASSERT_VALID( pNodeI );
	}
#endif // _DEBUG
	ASSERT_VALID( pNodeC );
	ASSERT_VALID( m_pWndTop );
	ASSERT( m_pWndTop->GetSafeHwnd() != NULL && ::IsWindow(m_pWndTop->GetSafeHwnd()) );

CExtCmdItem * pCmdItem =
		g_CmdManager->CmdGetPtr(
			g_CmdManager->ProfileNameFromWnd(m_pWndTop->m_hWnd),
			pNodeC->GetCmdID( false )
			);
	if( pCmdItem == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	if( !OnCanDragCustomizeCommand( pCmdItem ) )
		return false;

	if( m_cf == NULL )
	{
		m_cf = _RegisterClipFormat( m_pWndTop->m_hWnd );
		if( m_cf == NULL )
		{
			ASSERT( FALSE );
			return false;
		}
	} // if( m_cf == NULL )

	ASSERT( m_pHelperCustomizeDropSource == NULL );
	m_pHelperCustomizeDropSource = pCustomizeDragSource;
	m_pHelperDragSourceNode = pNodeC;

COleDataSource oleDataSource;
CExtCustomizeSite::CCmdDragInfo _dragInfo( pNodeI, pNodeC );
	_dragInfo.CacheGlobalData( oleDataSource, m_cf );
DROPEFFECT deAllow = DROPEFFECT_COPY;
	if( pCustomizeDragSource->OnCustomizeSourceAllowMoveDel() )
		deAllow |= DROPEFFECT_MOVE;
CCmdDropSource oleDropSource(
		pCustomizeDragSource,
		this
		);
//DROPEFFECT deResult =
		oleDataSource.DoDragDrop(
			deAllow,
			&rcItem,
			&oleDropSource
			);
DROPEFFECT deResult = oleDropSource.GetLastEffect();
bool bNoResetActiveItem = false;
	pCustomizeDragSource->OnCustomizeSourceDragComplete(
		deResult,
		oleDropSource.IsCanceled(),
		&bNoResetActiveItem
		);

	m_pHelperCustomizeDropSource = NULL;
	m_pHelperDragSourceNode = NULL;

	if(		(! IsCustomizeMode() )
		||	oleDropSource.IsCanceled()
		||	( m_pPopupDropTargetTopSaved != NULL )
		)
	{
		if(		m_pPopupDropTargetTopSaved != NULL
			&&	IsCustomizeMode()
			)
			DropTargetPopupCancelEx( -3 );
		else
			DropTargetPopupCancelEx( -2 );
	}
	if( !bNoResetActiveItem )
		ActiveItemSet();
	//CustomizedNodeSet();
	//DraggedNodeSet();

	return (deResult == DROPEFFECT_NONE) ? false : true;
}

void CExtCustomizeSite::DropTargetPopupTrack(
	CExtCustomizeCmdTreeNode * pPopupDropTargetNode,
	const CRect & rcExcludeArea,
	CPoint point,
	DWORD dwTrackFlags
	)
{
	ASSERT( this != NULL );
	if( !m_bInitComplete )
		return;
	ASSERT_VALID( m_pWndTop );
	ASSERT( m_pWndTop->GetSafeHwnd() != NULL && ::IsWindow(m_pWndTop->GetSafeHwnd()) );
	ASSERT_VALID( pPopupDropTargetNode );
	if( m_pPopupDropTargetNode == pPopupDropTargetNode )
		return;
	if(		m_pPopupDropTargetNode != NULL
		&&	m_pPopupDropTargetNode->IsChild( pPopupDropTargetNode )
		)
		return;
	if(		m_pHelperDragSourceNode != NULL
		&&	(	((LPVOID)m_pHelperDragSourceNode) == ((LPVOID)pPopupDropTargetNode)
			||	m_pHelperDragSourceNode->IsChild( pPopupDropTargetNode )
			)
		)
		return;
int nPushPop = 0;
	if(	m_pPopupDropTargetNodeSaved == pPopupDropTargetNode )
	{
		ASSERT_VALID( m_pPopupDropTargetNodeSaved );
		ASSERT_VALID( m_pPopupDropTargetTopSaved );
		ASSERT( m_pPopupDropTargetTopSaved->m_hWnd != NULL );
		ASSERT( IsWindow(m_pPopupDropTargetTopSaved->m_hWnd) );
		nPushPop = -1;
	} // if( m_pPopupDropTargetNodeSaved != NULL )
	else if(	m_pPopupDropTargetNode != NULL
			&&	m_pPopupDropTargetNodeSaved == NULL
			&&	m_pPopupDropTargetNode != pPopupDropTargetNode
		)
		nPushPop = 1;
	else if(	m_pPopupDropTargetNodeSaved != NULL
			&&	m_pPopupDropTargetNodeSaved == pPopupDropTargetNode
		)
		nPushPop -= 1;
	ASSERT(
			m_pPopupDropTargetNodeSaved != m_pPopupDropTargetNode
		||	m_pPopupDropTargetNode == NULL
		);
	DropTargetPopupCancelEx( nPushPop );
	ASSERT(
			m_pPopupDropTargetNodeSaved != m_pPopupDropTargetNode
		||	m_pPopupDropTargetNode == NULL
		);
	if( nPushPop < 0 )
	{
		return;
	}
	m_pPopupDropTargetNode = pPopupDropTargetNode;
	m_pPopupDropTargetTop = new CExtPopupMenuWnd;
	if(		(!	m_pPopupDropTargetTop->UpdateFromCmdTree(
					m_pWndTop->m_hWnd,
					m_pPopupDropTargetNode
				)
			)
		||	(!	m_pPopupDropTargetTop->TrackPopupMenu(
				dwTrackFlags|TPMX_CUSTOMIZE_MODE,
				point.x, point.y,
				&rcExcludeArea
				)
			)
		)
	{
		ASSERT( FALSE );
		delete m_pPopupDropTargetTop;
		m_pPopupDropTargetTop = NULL;
		m_pPopupDropTargetNode = NULL;
		return;
	}
}

void CExtCustomizeSite::DropTargetPopupCancel()
{
	ASSERT( this != NULL );
	if( !m_bInitComplete )
		return;
int nPushPop = 0;
	if(		m_pPopupDropTargetTop != NULL
		&&	m_pPopupDropTargetTopSaved == NULL
		)
		nPushPop = 1;
	else if( m_pPopupDropTargetTop == NULL )
		return;
	ASSERT(
			m_pPopupDropTargetNodeSaved != m_pPopupDropTargetNode
		||	m_pPopupDropTargetNode == NULL
		);
	DropTargetPopupCancelEx( nPushPop );
	ASSERT(
			m_pPopupDropTargetNodeSaved != m_pPopupDropTargetNode
		||	m_pPopupDropTargetNode == NULL
		);
}

void CExtCustomizeSite::DropTargetPopupCancelEx( int nPushPop )
{
	ASSERT( this != NULL );
	if( !m_bInitComplete )
		return;
#ifdef _DEBUG
	if( m_pPopupDropTargetNode == NULL )
	{
		ASSERT( m_pPopupDropTargetTop == NULL );
	}
	else
	{
		ASSERT_VALID( m_pPopupDropTargetTop );
		ASSERT_VALID( m_pPopupDropTargetNode );
	}
#endif // _DEBUG

	ASSERT(
			m_pPopupDropTargetNodeSaved != m_pPopupDropTargetNode
		||	m_pPopupDropTargetNode == NULL
		);
	if( nPushPop == -3 )
	{
		if( m_pPopupDropTargetNodeSaved != NULL )
		{
ASSERT( ((LPVOID)m_pPopupDropTargetTopSaved) != ((LPVOID)m_pHelperCustomizeDropSource) );
			if( m_pPopupDropTargetTopSaved->GetSafeHwnd() != NULL )
				m_pPopupDropTargetTopSaved->DestroyWindow();
			delete m_pPopupDropTargetTopSaved;
			m_pPopupDropTargetTopSaved = NULL;
			m_pPopupDropTargetNodeSaved = NULL;
		} // if( m_pPopupDropTargetNode != NULL ..
		if( m_pPopupDropTargetTop != NULL )
			m_pPopupDropTargetTop->_DoShowChain( SW_SHOWNA );
	} // if( nPushPop == -3 )
	else if( nPushPop == -2 )
	{
		if( m_pPopupDropTargetNode != NULL )
		{
ASSERT( ((LPVOID)m_pPopupDropTargetTop) != ((LPVOID)m_pHelperCustomizeDropSource) );
			if( m_pPopupDropTargetTop->GetSafeHwnd() != NULL )
				m_pPopupDropTargetTop->DestroyWindow();
			delete m_pPopupDropTargetTop;
			m_pPopupDropTargetTop = NULL;
			m_pPopupDropTargetNode = NULL;
		} // if( m_pPopupDropTargetNode != NULL ..
		if( m_pPopupDropTargetNodeSaved != NULL )
		{
ASSERT( ((LPVOID)m_pPopupDropTargetTopSaved) != ((LPVOID)m_pHelperCustomizeDropSource) );
			if( m_pPopupDropTargetTopSaved->GetSafeHwnd() != NULL )
				m_pPopupDropTargetTopSaved->DestroyWindow();
			delete m_pPopupDropTargetTopSaved;
			m_pPopupDropTargetTopSaved = NULL;
			m_pPopupDropTargetNodeSaved = NULL;
		} // if( m_pPopupDropTargetNode != NULL ..
	} // else if( nPushPop == -2 )
	else if( nPushPop > 0 )
	{
		ASSERT( m_pPopupDropTargetNodeSaved == NULL );
		ASSERT( m_pPopupDropTargetTopSaved == NULL );
		m_pPopupDropTargetNodeSaved = m_pPopupDropTargetNode;
		m_pPopupDropTargetTopSaved = m_pPopupDropTargetTop;
		m_pPopupDropTargetTopSaved->_DoShowChain( SW_HIDE );
		m_pPopupDropTargetTop = NULL;
		m_pPopupDropTargetNode = NULL;
	} // else if( nPushPop > 0 )
	else if( nPushPop < 0 )
	{
		ASSERT_VALID( m_pPopupDropTargetTopSaved );
		ASSERT_VALID( m_pPopupDropTargetNodeSaved );
		if( m_pPopupDropTargetNode != NULL )
		{
ASSERT( ((LPVOID)m_pPopupDropTargetTop) != ((LPVOID)m_pHelperCustomizeDropSource) );
			if( m_pPopupDropTargetTop->GetSafeHwnd() != NULL )
				m_pPopupDropTargetTop->DestroyWindow();
			delete m_pPopupDropTargetTop;
		} // if( m_pPopupDropTargetNode != NULL )
		m_pPopupDropTargetNode = m_pPopupDropTargetNodeSaved;
		m_pPopupDropTargetTop = m_pPopupDropTargetTopSaved;
		m_pPopupDropTargetTopSaved = NULL;
		m_pPopupDropTargetNodeSaved = NULL;
		m_pPopupDropTargetTop->_DoShowChain( SW_SHOWNA );
	} // else if( nPushPop < 0 )
	else
	{ // nPushPop == 0
		ASSERT( nPushPop == 0 );
//		ASSERT( m_pPopupDropTargetTopSaved == NULL );
		if( m_pPopupDropTargetTopSaved != NULL )
			m_pPopupDropTargetTopSaved->_DoShowChain( SW_HIDE );
		if(		m_pPopupDropTargetNode != NULL
//			&&	m_pPopupDropTargetNodeSaved == NULL
			)
		{
ASSERT( ((LPVOID)m_pPopupDropTargetTop) != ((LPVOID)m_pHelperCustomizeDropSource) );
			if( m_pPopupDropTargetTop->GetSafeHwnd() != NULL )
				m_pPopupDropTargetTop->DestroyWindow();
			delete m_pPopupDropTargetTop;
			m_pPopupDropTargetTop = NULL;
			m_pPopupDropTargetNode = NULL;
		} // if( m_pPopupDropTargetNode != NULL ..
	} // nPushPop == 0

	// redraw bars
	for(	POSITION pos = m_mapToolBars.GetStartPosition();
			pos != NULL;
			)
	{
		CExtToolControlBar * pBar = NULL;
		CToolBarCustomizeInfo * pInfo = NULL;
		m_mapToolBars.GetNextAssoc( pos, (void * &)pBar, (void * &)pInfo );
		ASSERT( pBar != NULL );
		ASSERT( pInfo != NULL );
		pInfo;
		if(		pBar->GetSafeHwnd() == NULL
			||	(! pBar->IsVisible() )
			)
			continue;
		pBar->Invalidate();
	}
	if(		m_pWndMenuBar->GetSafeHwnd() != NULL
		&&	m_pWndMenuBar->IsVisible()
		)
		m_pWndMenuBar->Invalidate();
}

CExtCustomizeCmdTreeNode * CExtCustomizeSite::GetPopupDropTargetNode()
{
	ASSERT( this != NULL );
#ifdef _DEBUG
	if( m_pPopupDropTargetNode != NULL )
	{
		ASSERT_VALID( m_pPopupDropTargetNode );
	}
#endif // _DEBUG
	return m_pPopupDropTargetNode;
}

CExtPopupMenuWnd * CExtCustomizeSite::GetPopupDropTargetMenu()
{
	ASSERT( this != NULL );
#ifdef _DEBUG
	if( m_pPopupDropTargetTop != NULL )
	{
		ASSERT_VALID( m_pPopupDropTargetTop );
		ASSERT(
				m_pPopupDropTargetTop->GetSafeHwnd() != NULL
			&&	::IsWindow( m_pPopupDropTargetTop->GetSafeHwnd() )
			);
	}
#endif // _DEBUG
	return m_pPopupDropTargetTop;
}

void CExtCustomizeSite::RegisterCommandDropTarget(
	CExtCustomizeSite::ICustomizeDropTarget * pCustomizeDropTarget,
	CWnd * pWnd
	)
{
	ASSERT( this != NULL );
	if(		pCustomizeDropTarget == NULL
		||	pWnd->GetSafeHwnd() == NULL
		||	(! IsWindow(pWnd->GetSafeHwnd()) )
		)
	{
		ASSERT( FALSE );
		return;
	}
CCmdDropTarget * pOleDropTarget = NULL;
	if( m_mapCmdTagets.Lookup(
			(void *)pCustomizeDropTarget,
			(void * &)pOleDropTarget
			)
		)
	{
		ASSERT( pOleDropTarget != NULL );
		return;
	}
	pOleDropTarget =
		new CCmdDropTarget(
			pCustomizeDropTarget,
			this
			);
	if( !pOleDropTarget->Register( pWnd ) )
	{
		ASSERT( FALSE );
		delete pOleDropTarget;
		return;
	} // if( !pOleDropTarget->Register( pWnd ) )
	m_mapCmdTagets.SetAt(
		(void *)pCustomizeDropTarget,
		(void *)pOleDropTarget
		);
}

void CExtCustomizeSite::UnRegisterCommandDropTarget(
	CExtCustomizeSite::ICustomizeDropTarget * pCustomizeDropTarget
	)
{
	ASSERT( this != NULL );
	if( !m_bInitComplete )
	{
		ASSERT( FALSE );
		return;
	}
CCmdDropTarget * pOleDropTarget = NULL;
	if( !m_mapCmdTagets.Lookup(
			(void *)pCustomizeDropTarget,
			(void * &)pOleDropTarget
			)
		)
		return;
	ASSERT_VALID( pOleDropTarget );
	pOleDropTarget->Revoke();
	delete pOleDropTarget;
	m_mapCmdTagets.RemoveKey( (void*)pCustomizeDropTarget );
}

void CExtCustomizeSite::UnRegisterAllCommandDropTargets()
{
	ASSERT( this != NULL );
	if( !m_bInitComplete )
		return;
	for(	POSITION pos = m_mapCmdTagets.GetStartPosition();
			pos != NULL;
		)
	{
		ICustomizeDropTarget * pCustomizeDropTarget = NULL;
		CCmdDropTarget * pOleDropTarget = NULL;
		m_mapCmdTagets.GetNextAssoc(
			pos,
			(void * &)pCustomizeDropTarget,
			(void * &)pOleDropTarget
			);
		ASSERT( pCustomizeDropTarget != NULL );
		ASSERT_VALID( pOleDropTarget );
		pOleDropTarget->Revoke();
		delete pOleDropTarget;
	}
	m_mapCmdTagets.RemoveAll();
}

bool CExtCustomizeSite::DoFormCustomization(
	bool bDelayShow // = false
	)
{
	ASSERT( this != NULL );
	if( (!m_bInitComplete) || IsCustomizeMode() )
		return false;

	ASSERT_VALID( m_pWndTop );
	ASSERT( m_pWndTop->GetSafeHwnd() != NULL && ::IsWindow(m_pWndTop->GetSafeHwnd()) );
	if( bDelayShow )
	{
		m_pWndTop->PostMessage( g_nMsgDelayCustomization );
		return true;
	} // if( bDelayShow )

	if( m_cf == NULL )
	{
		m_cf = _RegisterClipFormat( m_pWndTop->m_hWnd );
		if( m_cf == NULL )
		{
			ASSERT( FALSE );
			return false;
		}
	} // if( m_cf == NULL )
	m_bCustomizeMode = true;
	OnCustomizeModeEnter();

HWND hWndTop = m_pWndTop->m_hWnd;
	m_hWndCustomizeForm = OnCreateCustomizeForm();
	if( m_hWndCustomizeForm != NULL )
	{
		for(	MSG msg;
				::IsWindow( m_hWndCustomizeForm )
					&& IsCustomizeMode();
				)
		{ // do customize form message loop
			::WaitMessage();
			for( ; PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE); )
			{ // process all the messages in the message queue
				if( !AfxGetThread()->PumpMessage() )
				{
					PostQuitMessage(0);
					break; // signal WM_QUIT received
				} // if( !AfxGetThread()->PumpMessage() )
			} // process all the messages in the message queue
		} // do customize form message loop
		if(		m_hWndCustomizeForm != NULL
			&&	::IsWindow(m_hWndCustomizeForm)
			)
			::DestroyWindow( m_hWndCustomizeForm );
		m_hWndCustomizeForm = NULL;
	} // if( m_hWndCustomizeForm != NULL )

	if( ! ::IsWindow(hWndTop) )
		return true;

	DropTargetPopupCancelEx( -3 );

	m_bCustomizeMode = false;
	OnCustomizeModeLeave();
	return true;
}

void CExtCustomizeSite::CancelCustomization()
{
	ASSERT( this != NULL );
	if( !m_bInitComplete )
		return;
	ASSERT_VALID( m_pWndTop );
	ASSERT( m_pWndTop->GetSafeHwnd() != NULL && ::IsWindow(m_pWndTop->GetSafeHwnd()) );
	m_bCustomizeMode = false;
	m_pWndTop->PostMessage( WM_NULL );
}

bool CExtCustomizeSite::UpdateCustomizationFormData()
{
	ASSERT( this != NULL );
	if( ! m_bInitComplete )
		return false;
	if( m_pWndTop->GetSafeHwnd() == NULL )
		return false;
	ASSERT_VALID( m_pWndTop );
	if( ! IsCustomizeMode() )
		return false;
HWND hWndCustomizeForm = GetSafeCustomizeFormHWND();
	if( hWndCustomizeForm == NULL )
		return false;
	::SendMessage(
		hWndCustomizeForm,
		CExtCustomizeSite::g_nMsgSyncContent,
		0L,
		0L
		);
	return true;
}

HWND CExtCustomizeSite::OnCreateCustomizeForm()
{
	ASSERT( this != NULL );
HWND hWndRetVal = NULL;
CExtCustomizeForm * pCustomizeForm = NULL;
CPropertyPage * pPageToolbars = NULL;
CPropertyPage * pPageCommands = NULL;
CPropertyPage * pPageKeyboard = NULL;
CPropertyPage * pPageParms = NULL;
	try
	{
		pCustomizeForm = new CExtCustomizeForm;
		if( GetCustomizeFlags() & __ECSF_BARS )
		{
			pPageToolbars = new CExtCustomizePageToolbars;
			pCustomizeForm->AddPage( pPageToolbars );
		}
		if( GetCustomizeFlags() & __ECSF_COMMANDS )
		{
			pPageCommands = new CExtCustomizePageCommands;
			pCustomizeForm->AddPage( pPageCommands );
		}
		if( GetCustomizeFlags() & __ECSF_ACCELERATORS )
		{
			pPageKeyboard = new CExtCustomizePageKeyboard;
			pCustomizeForm->AddPage( pPageKeyboard );
		}
		if( GetCustomizeFlags() & __ECSF_PARMS )
		{
			pPageParms = new CExtCustomizePageParms;
			pCustomizeForm->AddPage( pPageParms );
		}
		if( pCustomizeForm->Create( m_pWndTop ) )
		{
			hWndRetVal = pCustomizeForm->GetSafeHwnd();
		} // if( pCustomizeForm->Create( m_pWndTop ) )
#ifdef _DEBUG
		else
		{
			ASSERT( FALSE );
		} // else from if( pCustomizeForm->Create( m_pWndTop ) )
#endif // _DEBUG
	} // try
	catch( CException * pExept )
	{
		ASSERT( FALSE );
		pExept->Delete();
	} // catch( CException * pExept )
	catch( ... )
	{
		ASSERT( FALSE );
	} // catch( ... )
	if( hWndRetVal == NULL )
	{
		if( pCustomizeForm->GetSafeHwnd() != NULL )
			pCustomizeForm->DestroyWindow();
		else
		{
			if( pCustomizeForm != NULL )
				delete pCustomizeForm;
			if( pPageToolbars != NULL )
				delete pPageToolbars;
			if( pPageCommands != NULL )
				delete pPageCommands;
			if( pPageKeyboard != NULL )
				delete pPageKeyboard;
			if( pPageParms != NULL )
				delete pPageParms;
		} // else from if( pCustomizeForm->GetSafeHwnd() != NULL )
	} // if( hWndRetVal == NULL )
	return hWndRetVal;
}

bool CExtCustomizeSite::GetToolbarCustomizeInfo(
	CExtToolControlBar * pBar,
	CExtCustomizeCmdTreeNode ** ppNodeC, // may be NULL
	CExtCustomizeCmdTreeNode ** ppNodeI, // = NULL
	CExtSafeString * p_strDisplayName, // = NULL
	bool * p_bUserBar // = NULL
	)
{
	ASSERT( this != NULL );
	ASSERT_VALID( pBar );
	ASSERT( pBar->GetSafeHwnd() != NULL && ::IsWindow(pBar->GetSafeHwnd()) );
	if( ppNodeC != NULL )
		(*ppNodeC) = NULL;
	if( ppNodeI != NULL )
		(*ppNodeI) = NULL;
	if( p_strDisplayName != NULL )
		(*p_strDisplayName) = _T("");
	if( p_bUserBar != NULL )
		(*p_bUserBar) = false;
CToolBarCustomizeInfo * pInfo = NULL;
	if( !m_mapToolBars.Lookup( (void *)pBar, (void * &)pInfo ) )
		return false;
	ASSERT( pInfo != NULL );
	if( ((LPVOID)m_pWndMenuBar) == ((LPVOID)pBar) )
	{ // if menu bar
		CCmdMenuInfo * pMenuInfo =
			m_pWndMenuBar->MenuInfoGet();
		if( pMenuInfo == NULL )
		{
			pMenuInfo = MenuInfoGetDefault();
			//ASSERT( pMenuInfo != NULL );
			if( pMenuInfo == NULL )
				return false;
		} // if( pMenuInfo == NULL )
		ASSERT_VALID( pMenuInfo->GetNode( false ) );
		ASSERT_VALID( pMenuInfo->GetNode( true ) );
		if( ppNodeC != NULL )
			(*ppNodeC) = pMenuInfo->GetNode( false );
		if( ppNodeI != NULL )
			(*ppNodeI) = pMenuInfo->GetNode( true );
	} // if menu bar
	else
	{
		ASSERT_VALID( pInfo->m_pHelperBarButtonsC );
		ASSERT_VALID( pInfo->m_pHelperBarButtonsI );
		if( ppNodeC != NULL )
			(*ppNodeC) = pInfo->m_pHelperBarButtonsC;
		if( ppNodeI != NULL )
			(*ppNodeI) = pInfo->m_pHelperBarButtonsI;
	} // else if menu bar
	if( p_strDisplayName != NULL )
		(*p_strDisplayName) = pInfo->m_strDisplayName;
	if( p_bUserBar != NULL )
		(*p_bUserBar) = pInfo->m_bUserToolBar;
	return true;
}

CExtCustomizeCmdTreeNode * CExtCustomizeSite::GetToolbarCmdNode(
	CExtToolControlBar * pBar,
	bool bInitialNode // = false
	)
{
	__PROF_UIS_MANAGE_STATE;
	ASSERT( this != NULL );
	ASSERT_VALID( pBar );
	ASSERT( pBar->GetSafeHwnd() != NULL && ::IsWindow(pBar->GetSafeHwnd()) );
CExtCustomizeCmdTreeNode * pNode = NULL;
	if(	! GetToolbarCustomizeInfo(
			pBar,
			bInitialNode ? NULL : (&pNode),
			bInitialNode ? (&pNode) : NULL
			)
		)
		return NULL;
	if( pNode == NULL )
		return NULL;
	ASSERT_VALID( pNode );
	return pNode;
}

void CExtCustomizeSite::BuildToolbarLists(
	CPtrList * pListBarsBasic,
	CPtrList * pListBarsUser // NULL
	)
{
	__PROF_UIS_MANAGE_STATE;
	ASSERT( this  != NULL );

	if( pListBarsBasic == NULL && pListBarsUser == NULL )
		return;
	
	if( pListBarsBasic != NULL )
	{
		pListBarsBasic->RemoveAll();
		for(	POSITION pos = m_mapToolBars.GetStartPosition();
				pos != NULL;
				)
		{
			CExtToolControlBar * pBar = NULL;
			CToolBarCustomizeInfo * pInfo = NULL;
			m_mapToolBars.GetNextAssoc( pos, (void * &)pBar, (void * &)pInfo );
			ASSERT_VALID( pBar );
			ASSERT( pInfo != NULL );
			if( pBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) )
			{
				ASSERT( m_pWndMenuBar != NULL );
				ASSERT( ((LPVOID)m_pWndMenuBar) == ((LPVOID)pBar) );
				continue;
			}
			if( !pInfo->m_bUserToolBar )
			{
//				if( pListBarsBasic != NULL )
					pListBarsBasic->AddTail( pBar );
			} // if( !pInfo->m_bUserToolBar )
#ifdef _DEBUG
			else
			{
//				if( pListBarsUser != NULL )
//					pListBarsUser->AddTail( pBar );
				ASSERT( m_listHelperUserBarsOrder.Find( (void*)pBar ) != NULL );
			} // else from if( !pInfo->m_bUserToolBar )
#endif // _DEBUG
		}
	} // if( pListBarsBasic != NULL )

	if( pListBarsUser != NULL )
	{
		pListBarsUser->RemoveAll();
		for(	POSITION pos = m_listHelperUserBarsOrder.GetHeadPosition();
				pos != NULL;
				)
		{
			CExtToolControlBar * pBar = (CExtToolControlBar*)
				m_listHelperUserBarsOrder.GetNext( pos );
			ASSERT_VALID( pBar );
#ifdef _DEBUG
			CToolBarCustomizeInfo * pInfo = NULL;
			VERIFY( m_mapToolBars.Lookup( (void * &)pBar, (void * &)pInfo ) );
			ASSERT( pInfo != NULL );
			ASSERT( pInfo->m_bUserToolBar );
#endif // _DEBUG
			ASSERT( !pBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) );
			pListBarsUser->AddTail( pBar );
		}
	} // if( pListBarsUser != NULL )
			
}

void CExtCustomizeSite::DeleteAllUserBars()
{
	__PROF_UIS_MANAGE_STATE;

	ASSERT( this  != NULL );
CPtrList _listBarsUser;
	BuildToolbarLists( NULL, &_listBarsUser );
	for(	POSITION pos = _listBarsUser.GetHeadPosition();
			pos != NULL;
		)
	{
		CExtToolControlBar * pBar = (CExtToolControlBar *)
			_listBarsUser.GetNext( pos );
		ASSERT_VALID( pBar );
		OnUserBarDelete( pBar );
	}
}

CExtBarButton *  CExtCustomizeSite::OnCreateToolbarButton(
	CExtToolControlBar * pBar,
	CExtCustomizeCmdTreeNode * pNodeI,
	CExtCustomizeCmdTreeNode * pNodeC
	)
{
	ASSERT( this != NULL );
#ifdef _DEBUG
	if( pNodeI != NULL )
	{
		ASSERT_VALID( pNodeI );
		ASSERT( pNodeI->GetCmdID(false) != ID_SEPARATOR );
	}
#endif // _DEBUG
	ASSERT_VALID( pNodeC );
	ASSERT( pNodeC->GetCmdID(false) != ID_SEPARATOR );
	ASSERT_VALID( pBar );
	ASSERT( pBar->GetSafeHwnd() != NULL && ::IsWindow(pBar->GetSafeHwnd()) );
UINT nCmdID = pNodeC->GetCmdID( false );
CExtCmdItem * pCmdItem =
		g_CmdManager->CmdGetPtr(
			g_CmdManager->ProfileNameFromWnd( pBar->m_hWnd ),
			nCmdID
			);
CExtBarButton * pTBB = NULL;
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	if(		pCmdItem != NULL
		&&	(	pCmdItem->StateIsTextField()
			||	pCmdItem->StateIsCombo()
			)
		)
	{
		bool bCombo = pCmdItem->StateIsCombo();
		INT nTextFieldWidth =
			// pCmdItem->m_nTextFieldWidth;
			pNodeC->TextFieldWidthGet();
		CExtBarTextFieldButton * pTextFieldTBB =
			new CExtBarTextFieldButton(
				bCombo,
				nTextFieldWidth,
				pBar,
				nCmdID,
				0 // nStyle
				);
		pTextFieldTBB->m_nDropDownWidth =
			pNodeC->DropDownWidthGet();
		pTextFieldTBB->m_nDropDownHeightMax =
			pNodeC->DropDownHeightMaxGet();
		pTextFieldTBB->m_bTextFieldIsNotEditable =
			( pNodeC->GetFlags() & __ECTN_TBB_TF_NE ) ? true : false;
		pTBB = pTextFieldTBB;
	}
	if( pTBB == NULL )
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	{
		if( pNodeC->GetFlags() & __ECTN_TBB_COLOR )
		{
			if( pCmdItem != NULL )
				pNodeC->ResetIcon( pCmdItem );
			COLORREF clrDefault = RGB(0,0,0);
			COLORREF clrSelected = RGB(0,0,0);
			LPARAM lParamCookie = pNodeC->GetLParam();
			OnColorItemValueGet( NULL, pNodeC, clrDefault, false );
			OnColorItemValueGet( NULL, pNodeC, clrSelected, true );
			pTBB =
				new CExtBarColorButton(
					pBar,
					nCmdID,
					0,
					clrDefault,
					clrSelected,
					lParamCookie
					);
		} // if( pNodeC->GetFlags() & __ECTN_TBB_COLOR )
		else
			pTBB =
				//new CExtBarButton( pBar, nCmdID );
				pBar->OnCreateBarCommandBtn( nCmdID );
	}
	ASSERT_VALID( pTBB );
bool bNoRotateVL = pTBB->GetNoRotateVerticalLayout();
	if( bNoRotateVL )
		pNodeC->ModifyFlags( __ECTN_TBB_NO_ROTATE_VL );
	if( pNodeI != NULL )
	{
		pTBB->SetBasicCmdNode( pNodeI );
		pTBB->OnCustomizeUpdateProps( pNodeI );
	} // if( pNodeI != NULL )
	else
		pTBB->OnCustomizeUpdateProps( pNodeC );
	pTBB->SetCustomizedCmdNode( pNodeC );
	if( pNodeC->GetFlags() & __ECTN_TBB_HIDDEN )
		pTBB->ModifyStyle( TBBS_HIDDEN, 0 );
	else
		pTBB->ModifyStyle( 0, TBBS_HIDDEN );
	if( pTBB->IsKindOf(RUNTIME_CLASS(CExtBarColorButton)) )
		((CExtBarColorButton*)pTBB)->OnSyncIcon();
	return pTBB;
}

void CExtCustomizeSite::OnInsertToolbarCmdNode(
	CExtCustomizeSite::CCmdDragInfo & _dragInfo,
	CExtToolControlBar * pBar,
	INT nInsertPos,
	bool bRecalcLayout
	)
{
	ASSERT( this != NULL );
	ASSERT( !_dragInfo.IsEmpty() );
	ASSERT_VALID( pBar );
	ASSERT( pBar->GetSafeHwnd() != NULL && ::IsWindow(pBar->GetSafeHwnd()) );
	ASSERT( 0 <= nInsertPos && nInsertPos <= pBar->GetButtonsCount() );
bool bReinitGroupStart = true;
INT nCmdNodePos = pBar->OnCustomizeCalcCmdNodePos( nInsertPos, &bReinitGroupStart );
CExtCustomizeCmdTreeNode * pBarNode = GetToolbarCmdNode( pBar );
	if( pBarNode == NULL )
		return;
INT nNodeCount = pBarNode->GetNodeCount();
	ASSERT( 0 <= nCmdNodePos && nCmdNodePos <= nNodeCount );
	if( bReinitGroupStart )
	{
		ASSERT( nCmdNodePos < nNodeCount );
		pBarNode->ElementAt( nCmdNodePos )
			-> ModifyFlags( 0, __ECTN_GROUP_START );
	} // if( bReinitGroupStart )
	ASSERT( _dragInfo.m_pDropNodeC->GetCmdID(false) != ID_SEPARATOR );
	pBarNode->InsertNode(
		this,
		_dragInfo.m_pDropNodeC->CloneNode(this),
		nCmdNodePos
		);
	if( bReinitGroupStart )
		pBarNode->ElementAt( nCmdNodePos )
			-> ModifyFlags( __ECTN_GROUP_START );
	if( nCmdNodePos == 0 )
		pBarNode->ElementAt( 0 )
			-> ModifyFlags( 0, __ECTN_GROUP_START );
	VERIFY(
		pBar->InsertSpecButton(
			nInsertPos,
			OnCreateToolbarButton(
				pBar,
				_dragInfo.m_pDropNodeI,
				pBarNode->ElementAt( nCmdNodePos )
				),
			bRecalcLayout
			)
		);
	if(		(pBarNode->ElementAt(nCmdNodePos)->GetFlags()&__ECTN_GROUP_START) != 0
		&&	nInsertPos > 0
		&&	(! pBar->GetButton(nInsertPos-1)->IsSeparator() )
		)
		pBar->InsertButton(nInsertPos,ID_SEPARATOR);
}

CExtCustomizeCmdTreeNode * CExtCustomizeSite::OnCmdNodeCreate(
	UINT nCmdIdBasic, // = 0L
	UINT nCmdIdEffective, // = 0L
	CExtCustomizeCmdTreeNode * pParentNode, // = NULL
	DWORD dwFlags, // = 0L
	__EXT_MFC_SAFE_LPCTSTR strTextInToolbar, // = NULL
	__EXT_MFC_SAFE_LPCTSTR strTextInMenu, // = NULL
	__EXT_MFC_SAFE_LPCTSTR strTextUser, // = NULL
	LPARAM lParam, // = 0L
	CExtCmdIcon * pIconCustomized // = NULL
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	,
	INT nTextFieldWidth, // = 100
	INT nDropDownWidth, // = -2 // (-1) - auto calc, (-2) - same as button area
	INT nDropDownHeightMax // = 250
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	)
{
	ASSERT( this != NULL );
	return
		new CExtCustomizeCmdTreeNode(
			nCmdIdBasic,
			nCmdIdEffective,
			pParentNode,
			dwFlags,
			strTextInToolbar,
			strTextInMenu,
			strTextUser,
			lParam,
			pIconCustomized
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
			,
			nTextFieldWidth,
			nDropDownWidth,
			nDropDownHeightMax
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
			);
}

void CExtCustomizeSite::OnCmdNodeInserted(
	CExtCustomizeCmdTreeNode * pNode
	)
{
	ASSERT( this != NULL );
	ASSERT_VALID( pNode );
	pNode;
}

void CExtCustomizeSite::OnBarStateChanged(
	CExtControlBar * pExtBar
	)
{
	ASSERT( this != NULL );
	ASSERT_VALID( pExtBar );
	ASSERT( pExtBar->m_hWnd != NULL && ::IsWindow(pExtBar->m_hWnd) );
	if( m_hWndCustomizeForm == NULL )
		return;
CWnd * pWndTemp = CWnd::FromHandlePermanent( m_hWndCustomizeForm );
	if( pWndTemp == NULL )
		return;
	ASSERT_VALID( pWndTemp );
CExtCustomizeForm * pForm =
		DYNAMIC_DOWNCAST( CExtCustomizeForm, pWndTemp );
	if( pForm == NULL )
		return;
	pForm->OnBarStateChanged( pExtBar );
}

void CExtCustomizeSite::OnCustomizeModeEnter()
{
	ASSERT( this != NULL );
	ASSERT( m_bInitComplete );
	ASSERT_VALID( m_pWndTop );
	ASSERT( m_pWndTop->GetSafeHwnd() != NULL && ::IsWindow(m_pWndTop->GetSafeHwnd()) );
	
	m_listHelperBarsForListBox.RemoveAll();
	if( m_pWndTop->IsKindOf(RUNTIME_CLASS(CFrameWnd)) )
	{
		for(	POSITION pos = ((CFrameWnd *)m_pWndTop)->m_listControlBars.GetHeadPosition();
				pos != NULL;
				)
		{ // walk frame bars
			CControlBar * pBar = (CControlBar *)
				((CFrameWnd *)m_pWndTop)->m_listControlBars.GetNext( pos );
			ASSERT_VALID( pBar );
			ASSERT_KINDOF( CControlBar, pBar );
			CExtDockBar * pDocker =
				DYNAMIC_DOWNCAST( CExtDockBar, pBar );
			if( pDocker != NULL )
			{
				pDocker->OnCustomizeModeEnter();
				continue;
			} // if( pDocker != NULL )
			CExtControlBar * pExtBar =
				DYNAMIC_DOWNCAST( CExtControlBar, pBar );
			if( pExtBar != NULL )
			{
				pExtBar->OnCustomizeModeEnter();
				if( pExtBar->m_bVisibleInCustomizeListBox )
					m_listHelperBarsForListBox.AddTail( (LPVOID)pExtBar );
				continue;
			} // if( pExtBar != NULL )
		} // walk frame bars

		CWnd * pWndPaneFirst = m_pWndTop->GetDlgItem(AFX_IDW_PANE_FIRST);
		if( pWndPaneFirst != NULL )
			pWndPaneFirst->EnableWindow( FALSE );
		CExtControlBar::stat_RecalcBarMetrics( ((CFrameWnd *)m_pWndTop) );
		((CFrameWnd *)m_pWndTop)->RecalcLayout();
		((CFrameWnd *)m_pWndTop)->RedrawWindow(
			NULL,
			NULL,
			RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE
				| RDW_FRAME | RDW_ALLCHILDREN
			);
		CExtControlBar::stat_RedrawFloatingFrames( ((CFrameWnd *)m_pWndTop) );
	} // if( m_pWndTop->IsKindOf(RUNTIME_CLASS(CFrameWnd)) )
	else
	{
		for(	POSITION pos = m_mapToolBars.GetStartPosition();
				pos != NULL;
				)
		{
			CExtToolControlBar * pBar = NULL;
			CToolBarCustomizeInfo * pInfo = NULL;
			m_mapToolBars.GetNextAssoc( pos, (void * &)pBar, (void * &)pInfo );
			ASSERT( pBar != NULL );
			ASSERT( pInfo != NULL );
			pBar->OnCustomizeModeEnter();
			if( pBar->m_bVisibleInCustomizeListBox )
				m_listHelperBarsForListBox.AddTail( (LPVOID)pBar );
		}
	} // else from if( m_pWndTop->IsKindOf(RUNTIME_CLASS(CFrameWnd)) )

	if( m_pWndMenuBar != NULL )
	{
		ASSERT_VALID( m_pWndMenuBar );
		CCmdMenuInfo * pCmdMenuInfo = m_pWndMenuBar->MenuInfoGet();
		if( pCmdMenuInfo == NULL )
			pCmdMenuInfo = MenuInfoGetDefault();
		if( pCmdMenuInfo != NULL )
			MenuInfoActiveSet( pCmdMenuInfo );
	} // if( m_pWndMenuBar != NULL )
}

void CExtCustomizeSite::OnCustomizeModeLeave()
{
	ASSERT( this != NULL );
	ASSERT( m_bInitComplete );
	ASSERT_VALID( m_pWndTop );
	ASSERT( m_pWndTop->GetSafeHwnd() != NULL && ::IsWindow(m_pWndTop->GetSafeHwnd()) );

	MenuInfoActiveSet( NULL );
	ActiveItemSet();

	if( m_pWndTop->IsKindOf(RUNTIME_CLASS(CFrameWnd)) )
	{
		((CFrameWnd *)m_pWndTop)->RecalcLayout();
		CWnd * pWndPaneFirst = m_pWndTop->GetDlgItem(AFX_IDW_PANE_FIRST);
		if( pWndPaneFirst != NULL )
			pWndPaneFirst->EnableWindow( TRUE );

		for(	POSITION pos = ((CFrameWnd *)m_pWndTop)->m_listControlBars.GetHeadPosition();
				pos != NULL;
				)
		{ // walk frame bars
			CControlBar * pBar = (CControlBar *)
				((CFrameWnd *)m_pWndTop)->m_listControlBars.GetNext( pos );
			ASSERT_VALID( pBar );
			ASSERT_KINDOF( CControlBar, pBar );
			CExtDockBar * pDocker =
				DYNAMIC_DOWNCAST( CExtDockBar, pBar );
			if( pDocker != NULL )
			{
				pDocker->OnCustomizeModeLeave();
				continue;
			} // if( pDocker != NULL )
			CExtControlBar * pExtBar =
				DYNAMIC_DOWNCAST( CExtControlBar, pBar );
			if( pExtBar != NULL )
			{
				pExtBar->OnCustomizeModeLeave();
				continue;
			} // if( pExtBar != NULL )
		} // walk frame bars

		CExtControlBar::stat_RecalcBarMetrics( ((CFrameWnd *)m_pWndTop) );
		((CFrameWnd *)m_pWndTop)->RecalcLayout();
		((CFrameWnd *)m_pWndTop)->RedrawWindow(
			NULL,
			NULL,
			RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE
				| RDW_FRAME | RDW_ALLCHILDREN
			);
		CExtControlBar::stat_RedrawFloatingFrames( ((CFrameWnd *)m_pWndTop) );
	} // if( m_pWndTop->IsKindOf(RUNTIME_CLASS(CFrameWnd)) )
	else
	{
		for(	POSITION pos = m_mapToolBars.GetStartPosition();
				pos != NULL;
				)
		{
			CExtToolControlBar * pBar = NULL;
			CToolBarCustomizeInfo * pInfo = NULL;
			m_mapToolBars.GetNextAssoc( pos, (void * &)pBar, (void * &)pInfo );
			ASSERT( pBar != NULL );
			ASSERT( pInfo != NULL );
			pBar->OnCustomizeModeLeave();
		}
	} // else from if( m_pWndTop->IsKindOf(RUNTIME_CLASS(CFrameWnd)) )

	m_listHelperBarsForListBox.RemoveAll();

	if( (GetCustomizeFlags()&__ECSF_ACCELERATORS) != 0 )
		OnUpdateAccelGlobalInfo( true );
}

CExtCustomizeSite * CExtCustomizeSite::GetCustomizeSite(
	HWND hWndStartSearch
	)
{
	__PROF_UIS_MANAGE_STATE;

	if(		hWndStartSearch == NULL
		||	(! ::IsWindow(hWndStartSearch) )
		)
		return NULL;

CExtCustomizeSite * pSite = NULL;
	for(	HWND hWnd = hWndStartSearch;
			hWnd != NULL;
			hWnd = ::GetParent( hWnd )
			)
	{ // walk all parents
		if( g_mapHelperPopupHooks.Lookup(
				hWnd,
				pSite
				)
			)
		{
			ASSERT( pSite != NULL );
			return pSite;
		}
		CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
		if( pWnd == NULL )
		{
			::SendMessage(
				hWnd,
				g_nMsgQueryCustomizeSite,
				(WPARAM)(&pSite),
				(LPARAM)0L
				);
			if( pSite != NULL )
			{
				if( CWnd::FromHandlePermanent(pSite->m_pWndTop->m_hWnd) == pSite->m_pWndTop )
					return pSite;
			}
			continue;
		} // if( pWnd == NULL )
		if(		pWnd->IsKindOf(RUNTIME_CLASS(CExtMiniDockFrameWnd))
			||	pWnd->IsKindOf(RUNTIME_CLASS(CControlBar))
			)
			continue; // can not be a customize site
		ASSERT_VALID( pWnd );
		pWnd->SendMessage(
			g_nMsgQueryCustomizeSite,
			(WPARAM)(&pSite),
			(LPARAM)0L
			);
		if( pSite != NULL )
			return pSite;
	} // walk all parents
	return NULL;
}

CPtrList & CExtCustomizeSite::GetBarsForListBox()
{
	ASSERT( this != NULL );
	return m_listHelperBarsForListBox;
}

void CExtCustomizeSite::OnRegisterToolBar(
	CExtToolControlBar * pToolBar
	)
{
	ASSERT( this != NULL );
	ASSERT_VALID( pToolBar );
	ASSERT_KINDOF( CExtToolControlBar, pToolBar );
CToolBarCustomizeInfo * pInfo = NULL;
	if( m_mapToolBars.Lookup( (void *)pToolBar, (void * &)pInfo ) )
	{
		ASSERT( pInfo != NULL );
		return;
	}
	if( !pToolBar->OnCustomizeRegisterBar(this) )
		return; // do not customize this bar
	if( m_pNodeToolbarsI == NULL )
		m_pNodeToolbarsI = OnCmdNodeCreate();
	if( m_pNodeToolbarsC == NULL )
		m_pNodeToolbarsC = OnCmdNodeCreate();
CExtCustomizeCmdTreeNode * pHelperBarButtonsI = NULL;
	if( pToolBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) )
	{
		ASSERT( m_pWndMenuBar == NULL ); // should be one!
		m_pWndMenuBar = (CExtMenuControlBar *)pToolBar;
//		m_pNodeMenusI->InsertBarNode( this, pHelperBarButtons );
	} // if( pToolBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) )
	else
	{
		UINT nCmdID = (UINT)pToolBar->GetDlgCtrlID();
		pHelperBarButtonsI = OnCmdNodeCreate( nCmdID, nCmdID );
		m_pNodeToolbarsI->InsertBarNode( this, pHelperBarButtonsI );
	} // else from if( pToolBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) )
	pInfo = new	CToolBarCustomizeInfo;
	pInfo->m_pHelperBarButtonsI = pHelperBarButtonsI;
	pToolBar->OnCustomizeGetBarInfo( pInfo );
	ASSERT( !pInfo->m_strDisplayName.IsEmpty() );
	m_mapToolBars.SetAt( pToolBar, pInfo );
	RegisterCommandDropTarget( pToolBar, pToolBar );
	if( pInfo->m_pHelperBarButtonsI != NULL )
	{
		ASSERT( pInfo->m_pHelperBarButtonsC == NULL );
		pInfo->m_pHelperBarButtonsC =
			pHelperBarButtonsI->CloneNode( NULL, true );
		ASSERT_VALID( pInfo->m_pHelperBarButtonsC );
		m_pNodeToolbarsC->InsertBarNode( NULL, pInfo->m_pHelperBarButtonsC );
		pToolBar->OnCustomizeUpdateInitialBarState( pInfo->m_pHelperBarButtonsC, false );
		ASSERT( m_pNodeToolbarsC->GetNodeCount() == m_pNodeToolbarsI->GetNodeCount() );
	} // if( pInfo->m_pHelperBarButtonsI != NULL )
}

void CExtCustomizeSite::OnRegisterAdditionalToolBars()
{
	ASSERT( this != NULL );
}

bool CExtCustomizeSite::OnHookWndMsg(
	LRESULT & lResult,
	HWND hWndHooked,
	UINT nMessage,
	WPARAM & wParam,
	LPARAM & lParam
	)
{
	__PROF_UIS_MANAGE_STATE;

	ASSERT( this != NULL );

	if( hWndHooked == m_hWndHelperPopupHook )
	{
		if( nMessage == g_nMsgQueryCustomizeSite )
		{
			CExtCustomizeSite ** pSite = (CExtCustomizeSite **)wParam;
			ASSERT( pSite != NULL );
			(*pSite) = this;
			lResult = 1L;
			return true;
		} // if( nMessage == g_nMsgQueryCustomizeSite )

		if(		nMessage == WM_CLOSE
			||	nMessage == WM_DESTROY
			)
		{
			RemoveAllWndHooks();
			CancelCustomization();
			CExtPopupMenuWnd::PassMsgLoop( false );
			_EmptyContainers();
		}
		return
			CExtHookSink::OnHookWndMsg(
				lResult,
				hWndHooked,
				nMessage,
				wParam,
				lParam
				);
	} // if( hWndHooked == m_hWndHelperPopupHook )

	if( nMessage == WM_DESTROY )
	{
		CancelCustomization();
		CExtPopupMenuWnd::PassMsgLoop( false );
	} // if( nMessage == WM_DESTROY )
	if( nMessage == WM_NCDESTROY )
	{
		_EmptyContainers();
	} // if( nMessage == WM_NCDESTROY )

	if( nMessage == g_nMsgDelayCustomization )
	{
		DoFormCustomization();
		lResult = 1L;
		return true;
	} // if( nMessage == g_nMsgDelayCustomization )
	if( nMessage == g_nMsgQueryCustomizeSite )
	{
		CExtCustomizeSite ** pSite = (CExtCustomizeSite **)wParam;
		ASSERT( pSite != NULL );
		(*pSite) = this;
		lResult = 1L;
		return true;
	} // if( nMessage == g_nMsgQueryCustomizeSite )
	if( nMessage == g_nMsgSyncAccelTable )
	{
		__PROF_UIS_MANAGE_STATE;
		//ASSERT( (GetCustomizeFlags()&__ECSF_ACCELERATORS) != 0 );
		OnUpdateAccelGlobalInfo( false );
		return true;
	} // if( nMessage == g_nMsgSyncAccelTable )

	if(		m_hWndMdiClient != NULL
		&&	m_hWndMdiClient == hWndHooked
		)
	{ // if hooking MDI client area
		if(		nMessage == WM_MDIACTIVATE
			||	nMessage == WM_MDIDESTROY
			||	nMessage == WM_MDICREATE
			||	nMessage == WM_MDINEXT
			)
		{
			if( (GetCustomizeFlags()&__ECSF_ACCELERATORS) != 0 )
			{
				if( m_pWndTop->GetSafeHwnd() != NULL )
					OnUpdateAccelGlobalInfo( true );
			}
		}
		return
			CExtHookSink::OnHookWndMsg(
				lResult,
				hWndHooked,
				nMessage,
				wParam,
				lParam
				);
	} // if hooking MDI client area

	if(		nMessage == WM_COMMAND
		&&	wParam == ID_EXT_CUSTOMIZE
		)
	{
		if( IsCustomizeMode() )
		{
			ASSERT( FALSE );
			lResult = 0L;
			return true;
		}
		DoFormCustomization();
		lResult = 0L;
		return true;
	} // if( nMessage == WM_COMMAND && wParam == ID_EXT_CUSTOMIZE )
	if(		(	nMessage == WM_COMMAND
			&&	(	wParam == ID_APP_EXIT
				||	wParam == ID_CONTEXT_HELP
				||	wParam == ID_FILE_PRINT
				||	wParam == ID_FILE_PRINT_PREVIEW
				||	wParam == ID_FILE_PRINT_SETUP
				)
			)
		||	(nMessage == WM_SYSCOMMAND && wParam == SC_CLOSE)
		||	(	(	nMessage == WM_NCLBUTTONDOWN
				||	nMessage == WM_NCLBUTTONUP
				||	nMessage == WM_NCLBUTTONDBLCLK
				||	nMessage == WM_NCMOUSEMOVE
				)
			&&	(	wParam == HTCLOSE
				||	wParam == HTSYSMENU
				||	(m_pPopupDropTargetTop != NULL && nMessage != WM_NCMOUSEMOVE )
				)
			)
		||	nMessage == WM_CLOSE
		)
	{
		DropTargetPopupCancel();
		if( IsCustomizeMode() )
		{
			lResult = 0L;
			return true;
		}
	}
	if( nMessage == WM_COMMAND )
	{
		if( wParam == ID_EXT_RESET_TOOLBAR )
		{
			CExtToolControlBar * pBar = BarDefCmdTargetGet();
			if( pBar != NULL )
			{
				ASSERT_VALID( pBar );
				ASSERT( BarIsRegistered(pBar) );
				if( BarResetAsk(pBar) )
				{
					VERIFY( BarReset( pBar, TRUE ) );
				}
			} // if( pBar != NULL )
#ifdef _DEBUG
			else
			{
				ASSERT( FALSE );
			} // else from if( pBar != NULL )
#endif // _DEBUG
			lResult = 0L;
			return true;
		} // if( wParam == ID_EXT_RESET_TOOLBAR )
		CExtToolControlBar * pBar = NULL;
		if( m_mapHelperUserBars.Lookup( (WORD)wParam, (void * &)pBar ) )
		{
			ASSERT_VALID( pBar );
			ASSERT_VALID( pBar->m_pDockSite );
			pBar->m_pDockSite->ShowControlBar(
				pBar,
				!pBar->IsVisible(),
				FALSE
				);
			lResult = 0L;
			return true;
		} // if( m_mapHelperUserBars.Lookup( (WORD)wParam, (void * &)pBar ) )
	} // if( nMessage == WM_COMMAND )
	
	if(		nMessage == WM_SIZE
		||	nMessage ==	WM_ACTIVATEAPP
		||	nMessage == WM_SIZEPARENT
		)
	{
		if( m_pHelperDragSourceNode == NULL )
			DropTargetPopupCancelEx( -2 );
	}

	return 
		CExtHookSink::OnHookWndMsg(
			lResult,
			hWndHooked,
			nMessage,
			wParam,
			lParam
			);
}

CExtToolControlBar * CExtCustomizeSite::OnUserBarCreate(
	__EXT_MFC_SAFE_LPCTSTR strName,
	UINT nBarID, // = 0 // 0-alloc any, other-alloc specified
	bool bCreateInvisible, // = false
	CExtToolControlBar * pBarNew // = NULL
	)
{
	__PROF_UIS_MANAGE_STATE;

	ASSERT( this != NULL );
	ASSERT( strName != NULL && _tcslen(strName) > 0 );
	if(		m_pWndTop->GetSafeHwnd() == NULL
		||	(! ::IsWindow(m_pWndTop->GetSafeHwnd()) )
		||	(! m_pWndTop->IsKindOf(RUNTIME_CLASS(CFrameWnd)) )
		)
	{
		ASSERT( FALSE );
		return NULL;
	}
CWnd * pWndDocker = m_pWndTop->GetDlgItem( AFX_IDW_DOCKBAR_TOP );
	if(		pWndDocker == NULL
		||	(! pWndDocker->IsKindOf(RUNTIME_CLASS(CExtDockOuterBar)) )
		)
	{
		ASSERT( FALSE );
		return NULL;
	}
__EXT_MFC_SAFE_LPCTSTR strProfileName =
		g_CmdManager->ProfileNameFromWnd( m_pWndTop->m_hWnd );
	if( strProfileName == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	}
CExtCmdItem * pCmdItem =
		g_CmdManager->CmdAllocPtr(
			strProfileName,
			nBarID
			);
	if( pCmdItem == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	}
	pCmdItem->StateSetBasic();
	pCmdItem->StateSetCustomToolbar();
	pCmdItem->StateEnable( true );
	pCmdItem->m_sMenuText
		= pCmdItem->m_sToolbarText
		= pCmdItem->m_sTipStatus
		= pCmdItem->m_sTipTool
		= strName;
#ifdef _DEBUG
	if( pBarNew != NULL )
	{
		ASSERT_VALID( pBarNew );
		ASSERT_KINDOF( CExtToolControlBar, pBarNew );
		ASSERT( pBarNew->m_hWnd == NULL );
	} // if( pBarNew != NULL )
#endif // _DEBUG
CExtToolControlBar * pBar =
		(pBarNew != NULL)
			? pBarNew
			: (new CExtToolControlBar)
			;
	if( !pBar->Create(
			strName,
			m_pWndTop,
			pCmdItem->m_nCmdID,
			WS_CHILD //|WS_VISIBLE
			|CBRS_TOP|CBRS_GRIPPER|CBRS_TOOLTIPS
			|CBRS_FLYBY|CBRS_SIZE_DYNAMIC
			|CBRS_HIDE_INPLACE
			)
		)
	{
		ASSERT( FALSE );
		delete pBar;
		return NULL;
	}
	pBar->m_bAutoDelete = TRUE;
	pBar->EnableDocking( CBRS_ALIGN_ANY );
	((CFrameWnd*)m_pWndTop)->DockControlBar( pBar );
	((CFrameWnd*)m_pWndTop)->RecalcLayout();
CExtSafeString strWindowText( strName );
	strWindowText.Replace( _T("&"), _T("") );
	pBar->SetWindowText( strWindowText );
	OnRegisterToolBar( pBar );
	ASSERT( ((DWORD)((WORD)pCmdItem->m_nCmdID)) == ((DWORD)pCmdItem->m_nCmdID) );
	m_mapHelperUserBars.SetAt(
		(WORD)pCmdItem->m_nCmdID,
		(LPVOID)pBar
		);
	m_listHelperUserBarsOrder.AddTail( pBar );

CToolBarCustomizeInfo * pInfo = NULL;
	VERIFY( m_mapToolBars.Lookup( (void *)pBar, (void * &)pInfo ) );
	ASSERT( pInfo != NULL );
	pInfo->m_bUserToolBar = true;

	pBar->InitContentExpandButton();

	((CFrameWnd*)m_pWndTop)->ShowControlBar(
		pBar,
		bCreateInvisible ? FALSE : TRUE,
		TRUE
		);
	((CFrameWnd*)m_pWndTop)->RecalcLayout();
	pBar->ShowWindow( SW_SHOWNA );

HWND hWndCustomizeForm = GetSafeCustomizeFormHWND();
	if( hWndCustomizeForm != NULL )
	{
		ASSERT( m_listHelperBarsForListBox.Find( (void *)pBar ) == NULL );
		m_listHelperBarsForListBox.AddTail( pBar );
		::SendMessage(
			hWndCustomizeForm,
			CExtCustomizeSite::g_nMsgSyncContent,
			0L,
			0L
			);
	}
	return pBar;
}

void CExtCustomizeSite::OnUserBarRename(
	CExtToolControlBar * pBar,
	__EXT_MFC_SAFE_LPCTSTR strName
	)
{
	__PROF_UIS_MANAGE_STATE;

	ASSERT( this != NULL );
	ASSERT_VALID( pBar );
	ASSERT( strName != NULL && _tcslen(strName) > 0 );
	ASSERT( pBar->GetSafeHwnd() != NULL && ::IsWindow(pBar->GetSafeHwnd()) );
	ASSERT( pBar->IsKindOf(RUNTIME_CLASS(CExtToolControlBar)) );
	ASSERT( ! pBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) );
__EXT_MFC_SAFE_LPCTSTR strProfileName =
		g_CmdManager->ProfileNameFromWnd( m_pWndTop->m_hWnd );
	if( strProfileName == NULL )
	{
		ASSERT( FALSE );
		return;
	}
CExtCmdItem * pCmdItem =
		g_CmdManager->CmdGetPtr(
			strProfileName,
			(UINT)pBar->GetDlgCtrlID()
			);
	if( pCmdItem == NULL )
	{
		ASSERT( FALSE );
		return;
	}
	ASSERT( pCmdItem->StateIsCustomToolbar() );

CToolBarCustomizeInfo * pInfo = NULL;
	if( !m_mapToolBars.Lookup( (void *)pBar, (void * &)pInfo ) )
	{
		ASSERT( FALSE );
		return;
	}
	ASSERT( pInfo != NULL );
	if( !pInfo->m_bUserToolBar )
	{
		ASSERT( FALSE );
		return;
	}
	pInfo->m_strDisplayName = strName;
CExtSafeString strWindowText( strName );
	strWindowText.Replace( _T("&"), _T("") );
	pBar->SetWindowText( strWindowText );
	pCmdItem->StateEnable( true );
	pCmdItem->m_sMenuText
		= pCmdItem->m_sToolbarText
		= pCmdItem->m_sTipStatus
		= pCmdItem->m_sTipTool
		= strName;

CFrameWnd * pFrame = pBar->GetParentFrame();
	if( pFrame != NULL )
	{
		pFrame->RecalcLayout();
		if( pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) )
			pFrame->SendMessage( WM_NCPAINT );
	}

HWND hWndCustomizeForm = GetSafeCustomizeFormHWND();
	if( hWndCustomizeForm != NULL )
		::SendMessage(
			hWndCustomizeForm,
			CExtCustomizeSite::g_nMsgSyncContent,
			0L,
			0L
			);
}

void CExtCustomizeSite::OnUserBarDelete(
	CExtToolControlBar * pBar,
	bool bBarWindowIsSelfDestroyed // = false
	)
{
	__PROF_UIS_MANAGE_STATE;

	ASSERT( this != NULL );
	ASSERT_VALID( pBar );
	ASSERT( pBar->GetSafeHwnd() != NULL /*&& ::IsWindow(pBar->GetSafeHwnd())*/ );
	ASSERT( pBar->IsKindOf(RUNTIME_CLASS(CExtToolControlBar)) );
	ASSERT( ! pBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) );
	if(		m_pWndTop->GetSafeHwnd() == NULL
		||	(! ::IsWindow(m_pWndTop->GetSafeHwnd()) )
		||	(! m_pWndTop->IsKindOf(RUNTIME_CLASS(CFrameWnd)) )
		)
	{
		ASSERT( FALSE );
		return;
	}
CWnd * pWndDocker = m_pWndTop->GetDlgItem( AFX_IDW_DOCKBAR_TOP );
	if(		pWndDocker == NULL
		||	(! pWndDocker->IsKindOf(RUNTIME_CLASS(CExtDockOuterBar)) )
		)
	{
		ASSERT( FALSE );
		return;
	}
	CExtPopupMenuWnd::CancelMenuTracking();
	DropTargetPopupCancel();
	CWnd::CancelToolTips();
__EXT_MFC_SAFE_LPCTSTR strProfileName =
		g_CmdManager->ProfileNameFromWnd( m_pWndTop->m_hWnd );
	if( strProfileName == NULL )
	{
		ASSERT( FALSE );
		return;
	}
UINT nBarID = pBar->GetDlgCtrlID();
CExtCmdItem * pCmdItem = g_CmdManager->CmdGetPtr( strProfileName, nBarID );
	if( pCmdItem == NULL )
	{
		ASSERT( FALSE );
		return;
	}
	ASSERT(	pCmdItem->StateIsCustomToolbar() );

CToolBarCustomizeInfo * pInfo = NULL;
	if( !m_mapToolBars.Lookup( (void *)pBar, (void * &)pInfo ) )
	{
		ASSERT( FALSE );
		return;
	}
	ASSERT( pInfo != NULL );
	if( !pInfo->m_bUserToolBar )
	{
		ASSERT( FALSE );
		return;
	}

	pBar->SetButtons();
	UnRegisterCommandDropTarget( pBar );

	if( pBar->IsFloating() )
	{
		// ( + 2.24 )
		CMiniDockFrameWnd * pMiniFrame =
			STATIC_DOWNCAST(
				CMiniDockFrameWnd,
				pBar->GetParentFrame()
				);
		ASSERT_VALID( pMiniFrame );
		ASSERT_KINDOF( CMiniDockFrameWnd, pMiniFrame );
		if( ! bBarWindowIsSelfDestroyed )
			pMiniFrame->DestroyWindow();
	} // if( pBar->IsFloating() )
	else
	{
		CDockBar * pDockBarOut = pBar->m_pDockBar;
		if( pDockBarOut != NULL )
		{
			ASSERT_VALID( pDockBarOut );
			ASSERT( pDockBarOut->FindBar( pBar ) >= 0 );
			if(		(! pDockBarOut->m_bFloating )
				&&	pDockBarOut->IsKindOf(RUNTIME_CLASS(CExtDockBar))
				)
			{
				VERIFY(
					((CExtDockBar *)pDockBarOut)->
						RemoveControlBar( pBar, -1, 0, false )
					);
			}
			else
			{
				VERIFY( pDockBarOut->RemoveControlBar(pBar) );
			}
			pBar->m_pDockBar = NULL;
		} // if( pDockBarOut != NULL )
		pBar->m_pDockSite = NULL;
		ASSERT( pBar->m_bAutoDelete );
		if( ! bBarWindowIsSelfDestroyed )
			pBar->DestroyWindow();
	} // else from if( pBar->IsFloating() )

POSITION pos = ((CFrameWnd*)m_pWndTop)->m_listControlBars.Find(pBar);
	if( pos != NULL )
		((CFrameWnd*)m_pWndTop)->m_listControlBars.RemoveAt(pos);

	VERIFY( g_CmdManager->CmdRemove( strProfileName, nBarID ) );
	m_mapHelperUserBars.RemoveKey( (WORD)nBarID );
	delete pInfo;
	m_mapToolBars.RemoveKey( (void *)pBar );
	ASSERT( m_listHelperUserBarsOrder.Find( (void *)pBar ) != NULL );
	m_listHelperUserBarsOrder.RemoveAt(
		m_listHelperUserBarsOrder.Find( (void *)pBar )
		);
	pos = m_listHelperBarsForListBox.Find( (void *)pBar );
	if( pos != NULL )
		m_listHelperBarsForListBox.RemoveAt( pos );

	ASSERT( m_pNodeToolbarsI->SearchNode(nBarID) >= 0 );
	m_pNodeToolbarsI->RemoveNodes(
		m_pNodeToolbarsI->SearchNode( nBarID ),
		1
		);
	ASSERT( m_pNodeToolbarsC->SearchNode(nBarID) >= 0 );
	m_pNodeToolbarsC->RemoveNodes(
		m_pNodeToolbarsC->SearchNode( nBarID ),
		1
		);
	((CFrameWnd*)m_pWndTop)->RecalcLayout();

HWND hWndCustomizeForm = GetSafeCustomizeFormHWND();
	if( hWndCustomizeForm != NULL )
		::SendMessage(
			hWndCustomizeForm,
			CExtCustomizeSite::g_nMsgSyncContent,
			0L,
			0L
			);
}

CExtToolControlBar * CExtCustomizeSite::GetUserBar(
	UINT nBarID
	)
{
	ASSERT( this != NULL );
CExtToolControlBar * pBar = NULL;
	if( !m_mapHelperUserBars.Lookup( (WORD)nBarID, (void * &)pBar ) )
		return NULL;
	ASSERT_VALID( pBar );
	return pBar;
}

bool CExtCustomizeSite::OnCustomizeTreeNode(
	CExtCustomizeCmdTreeNode * pNodeI,
	CExtCustomizeCmdTreeNode * pNodeC,
	CExtCmdItem * pCmdItem,
	bool & bDelete,
	bool bCanAddRemoveSeparator,
	CObject * pObjEventSrc
	)
{
	ASSERT( this != NULL );
#ifdef _DEBUG
	if( pNodeI != NULL )
	{
		ASSERT_VALID( pNodeI );
	}
#endif // _DEBUG
	ASSERT_VALID( pNodeC );
	ASSERT( pCmdItem != NULL );
	ASSERT( m_bInitComplete );
	ASSERT_VALID( m_pWndTop );
	ASSERT( m_pWndTop->GetSafeHwnd() != NULL && ::IsWindow(m_pWndTop->GetSafeHwnd()) );
	ASSERT( m_hWndCustomizeForm != NULL && ::IsWindow(m_hWndCustomizeForm) );
	ASSERT_VALID( pObjEventSrc );

	if(		pNodeI == NULL
		&&	(pNodeC->GetFlags() & __ECTN_DYNAMIC_POPUP)
		)
	{
		pNodeI =
			FindDynamicPopupInitialState(
				pNodeC->GetCmdID( false )
				);
#ifdef _DEBUG
		if( pNodeI != NULL )
		{
			ASSERT_VALID( pNodeI );
		}
#endif // _DEBUG
	} // if( pNodeI == NULL ...

CExtPopupMenuWnd * pPopup = new CExtPopupMenuWnd;
	{ // BLOCK
		if( !pPopup->LoadMenu(
				m_hWndCustomizeForm,
				IDR_MENU_CUSTOMIZE_BTN
				)
			)
		{
			ASSERT( FALSE );
			delete pPopup;
			return false;
		}
	} // BLOCK
CExtCmdIcon _iconPasteFromClipBoard, * pIconCurrent = NULL;
bool bCanChangeDisplayStyle = pNodeC->CanChangeDisplayStyle( pCmdItem );
bool bCanChangeIcon =
		(	(!  pCmdItem->StateIsPersistentIcon() )
			&& (pNodeC->GetFlags()&__ECTN_TBB_AUTOCHANGE_ID) == 0
		) ? true : false
		;
	if( !bCanChangeDisplayStyle )
	{
		pPopup->ItemGetInfo(pPopup->ItemFindPosForCmdID(ID_EXT_BTN_IMAGE_COPY)).Enable(false);
		pPopup->ItemGetInfo(pPopup->ItemFindPosForCmdID(ID_EXT_BTN_IMAGE_PASTE)).Enable(false);
		pPopup->ItemGetInfo(pPopup->ItemFindPosForCmdID(ID_EXT_BTN_IMAGE_EDIT)).Enable(false);
		pPopup->ItemGetInfo(pPopup->ItemFindPosForCmdID(ID_EXT_BTN_IMAGE_RESET)).Enable(false);
		pPopup->ItemGetInfo(pPopup->ItemFindPosForCmdID(ID_EXT_BTN_STYLE_DEFAULT)).Enable(false);
		pPopup->ItemGetInfo(pPopup->ItemFindPosForCmdID(ID_EXT_BTN_STYLE_IMAGE_AND_TEXT)).Enable(false);
		pPopup->ItemGetInfo(pPopup->ItemFindPosForCmdID(ID_EXT_BTN_STYLE_TEXT_ONLY_ALWAYS)).Enable(false);
		pPopup->ItemGetInfo(pPopup->ItemFindPosForCmdID(ID_EXT_BTN_STYLE_TEXT_ONLY_IN_MENUS)).Enable(false);
	} // if( !bCanChangeDisplayStyle )
	else if( bCanChangeIcon )
	{
		pIconCurrent = pNodeC->CurrentIconGetPtr( true, pCmdItem );
		CExtCmdIcon * pIcon = pNodeC->GetIconPtrInToolbar( pCmdItem, true );
		if(		(	::IsClipboardFormatAvailable( CF_BITMAP )
				||	::IsClipboardFormatAvailable( CF_DIB )
				)
			&&	pIcon != NULL
			&&	(! pIcon->IsEmpty() )
			&&	::OpenClipboard( m_hWndCustomizeForm )
			)
		{
			CSize _sizeOriginal( pIcon->GetSize() );
			ASSERT( _sizeOriginal.cx > 0 && _sizeOriginal.cy > 0 );
			HBITMAP hBmpClipboard = (HBITMAP)
				::GetClipboardData( CF_BITMAP );
			if( hBmpClipboard != NULL )
			{
				BITMAP bmpInfo;
				::memset( &bmpInfo, 0, sizeof(BITMAP) );
				::GetObject( hBmpClipboard, sizeof(BITMAP), &bmpInfo );
				ASSERT( bmpInfo.bmWidth > 0 && bmpInfo.bmHeight > 0 );
				if(		bmpInfo.bmWidth >= _sizeOriginal.cx
					&&	bmpInfo.bmHeight >= _sizeOriginal.cy
					)
				{
					COLORREF clrTransparent = RGB( 255, 0, 255 ); // may be, default for MFC
					if( pIcon->IsBitmapBased() )
						clrTransparent = pIcon->GetBitmapTransparentColor();
					CRect rcExtract( 0, 0, _sizeOriginal.cx, _sizeOriginal.cy );
					_iconPasteFromClipBoard.AssignFromHBITMAP(
						hBmpClipboard,
						clrTransparent,
						&rcExtract
						);
				}
			} // if( hBmpClipboard != NULL )
			::CloseClipboard();
		}
		if( _iconPasteFromClipBoard.IsEmpty() )
			pPopup->ItemGetInfo(pPopup->ItemFindPosForCmdID(ID_EXT_BTN_IMAGE_PASTE)).Enable(false);
		if( pIconCurrent == NULL )
			pPopup->ItemGetInfo(pPopup->ItemFindPosForCmdID(ID_EXT_BTN_IMAGE_COPY)).Enable(false);
	} // else if( bCanChangeIcon )

	if( bCanChangeIcon )
	{
		if( pCmdItem->StateIsPersistentIcon() )
			bCanChangeIcon = false;
	} // if( bCanChangeIcon )

	if( ! bCanChangeIcon )
	{
		pPopup->ItemGetInfo(pPopup->ItemFindPosForCmdID(ID_EXT_BTN_IMAGE_COPY)).Enable(false);
		pPopup->ItemGetInfo(pPopup->ItemFindPosForCmdID(ID_EXT_BTN_IMAGE_PASTE)).Enable(false);
		pPopup->ItemGetInfo(pPopup->ItemFindPosForCmdID(ID_EXT_BTN_IMAGE_EDIT)).Enable(false);
		pPopup->ItemGetInfo(pPopup->ItemFindPosForCmdID(ID_EXT_BTN_IMAGE_RESET)).Enable(false);
	} // if( ! bCanChangeIcon )

	if( !bCanAddRemoveSeparator )
		pPopup->ItemGetInfo(pPopup->ItemFindPosForCmdID(ID_EXT_BTN_BEGIN_A_GROUP)).Enable(false);

	switch( (pNodeC->GetFlags() & __ECTN_DISPLAY_MASK) )
	{
	case __ECTN_DISPLAY_DEFAULT_STYLE:
		pPopup->ItemGetInfo(pPopup->ItemFindPosForCmdID(ID_EXT_BTN_STYLE_DEFAULT)).Check(true);
		break;
	case __ECTN_DISPLAY_TEXT_AND_IMAGE:
		pPopup->ItemGetInfo(pPopup->ItemFindPosForCmdID(ID_EXT_BTN_STYLE_IMAGE_AND_TEXT)).Check(true);
		break;
	case __ECTN_DISPLAY_TEXTONLY_ALWAYS:
		pPopup->ItemGetInfo(pPopup->ItemFindPosForCmdID(ID_EXT_BTN_STYLE_TEXT_ONLY_ALWAYS)).Check(true);
		break;
	case __ECTN_DISPLAY_TEXTONLY_IN_MENU:
		pPopup->ItemGetInfo(pPopup->ItemFindPosForCmdID(ID_EXT_BTN_STYLE_TEXT_ONLY_IN_MENUS)).Check(true);
		break;
	} // switch( (pNodeC->GetFlags() & __ECTN_DISPLAY_MASK) )

	if( (pNodeC->GetFlags() & __ECTN_GROUP_START) )
		pPopup->ItemGetInfo(pPopup->ItemFindPosForCmdID(ID_EXT_BTN_BEGIN_A_GROUP)).Check(true);

bool bEditMenuText = true;
	if(		pObjEventSrc->IsKindOf(RUNTIME_CLASS(CExtToolControlBar))
		||	pObjEventSrc->IsKindOf(RUNTIME_CLASS(CExtBarButton))
		)
		bEditMenuText = false;
CExtSafeString strTextToEditInPlace =
		bEditMenuText
			? pNodeC->GetTextInMenu( pCmdItem, true )
			: pNodeC->GetTextInToolbar( pCmdItem, true )
			;
CExtSafeString strTextOriginal( strTextToEditInPlace );
	pPopup->ItemGetInfo(
		pPopup->ItemFindPosForCmdID(ID_EXT_BTN_NAME)).
			SetInplaceEdit( &strTextToEditInPlace );
#if (defined __EXT_MFC_NO_ICONEDITDLG)
	pPopup->ItemRemove(
		pPopup->ItemFindPosForCmdID( ID_EXT_BTN_IMAGE_EDIT )
		);
#endif // (defined __EXT_MFC_NO_ICONEDITDLG)

	if( !bEditMenuText )
		::SetForegroundWindow( m_hWndCustomizeForm );
CPoint ptCursor;
	if( ! ::GetCursorPos(&ptCursor) )
	{
		delete pPopup;
		return false;
	}
UINT nCmdRetVal = 0;
	if(	!	pPopup->TrackPopupMenu(
				TPMX_COMBINE_NONE|TPMX_DO_MESSAGE_LOOP
					|TPMX_NO_WM_COMMAND|TPMX_NO_CMD_UI
					|TPMX_NO_HIDE_RARELY,
				ptCursor.x, ptCursor.y,
				NULL, NULL, NULL,
				&nCmdRetVal
				)
		)
	{
		ASSERT( FALSE );
		delete pPopup;
		return false;
	}

bool bTextChanged =
		( strTextToEditInPlace != strTextOriginal )
			? true : false;

	if( nCmdRetVal == 0 && (!bTextChanged) )
		return false;

	if( bTextChanged )
	{
		if( bEditMenuText )
			pNodeC->SetTextInMenu(
				strTextToEditInPlace.IsEmpty()
					? _T("")
					: strTextToEditInPlace
				);
		else
			pNodeC->SetTextInToolbar(
				strTextToEditInPlace.IsEmpty()
					? _T("")
					: strTextToEditInPlace
				);
	} // if( bTextChanged )

	switch( nCmdRetVal )
	{
	case 0:
		return bTextChanged;
	case ID_EXT_BTN_RESET:
		if( pNodeI != NULL )
		{
			if(		(! pObjEventSrc->IsKindOf(RUNTIME_CLASS(CExtPopupMenuWnd)) )
				||	((CExtPopupMenuWnd*)pObjEventSrc)->GetSafeHwnd() == NULL
				)
				DropTargetPopupCancelEx( -2 );
			(*pNodeC) = (*pNodeI);
		}
		else
		{
			pNodeC->AssignCmdProps( pCmdItem );
			pNodeC->ModifyFlags( 0, __ECTN_DISPLAY_MASK );
			pNodeC->ModifyFlags( __ECTN_DISPLAY_DEFAULT_STYLE );
		} // else from if( pNodeI != NULL )
		return true;
	case ID_EXT_BTN_DELETE:
		bDelete = true;
		return true;
	case ID_EXT_BTN_IMAGE_COPY:
	{
		ASSERT( bCanChangeDisplayStyle );
		ASSERT( pIconCurrent != NULL );
		if( pIconCurrent->IsEmpty() )
		{
			pIconCurrent = pNodeC->GetIconPtrInToolbar( pCmdItem, false );
			ASSERT( !pIconCurrent->IsEmpty() );
		}
		HBITMAP hBmpCopy = NULL;
		COLORREF clrTransparent = RGB( 0, 0, 0 );
		if(		(! pIconCurrent->ExtractEditableBitmap(
					hBmpCopy,
					clrTransparent
					) )
			||	(! ::OpenClipboard( m_hWndCustomizeForm ) )
			)
		{
			ASSERT( FALSE );
			if( hBmpCopy != NULL )
				::DeleteObject( hBmpCopy );
			return true;
		}
		ASSERT( hBmpCopy != NULL );
		if( ::EmptyClipboard() )
		{
			HGLOBAL	hGlobal =
				CExtPaintManager::stat_HBITMAPtoHGLOBAL(
					hBmpCopy
					);
			if( hGlobal != NULL )
			{
				if( ::SetClipboardData( CF_DIB, hGlobal ) == NULL )
				{
					ASSERT( FALSE );
				}
			} // if( hGlobal != NULL )
#ifdef _DEBUG
			else
			{
				ASSERT( FALSE );
			} // else from if( hGlobal != NULL )
#endif // _DEBUG
		} // if( ::EmptyClipboard() )
#ifdef _DEBUG
		else
		{
			ASSERT( FALSE );
		} // else from if( ::EmptyClipboard() )
#endif // _DEBUG
		::DeleteObject( hBmpCopy );
		::CloseClipboard();
		return true;
	}
	case ID_EXT_BTN_IMAGE_PASTE:
		ASSERT( bCanChangeDisplayStyle );
		ASSERT( !_iconPasteFromClipBoard.IsEmpty() );
		ASSERT( pIconCurrent != NULL );
		ASSERT(
				pIconCurrent->IsEmpty()
			||	pIconCurrent->GetSize() == _iconPasteFromClipBoard.GetSize()
			);
		pIconCurrent->AssignFromOther( _iconPasteFromClipBoard );
		ASSERT( !pIconCurrent->IsEmpty() );
		return true;
	case ID_EXT_BTN_IMAGE_RESET:
		ASSERT( bCanChangeDisplayStyle );
		pNodeC->ResetIcon( pCmdItem );
		return true;
#if (!defined __EXT_MFC_NO_ICONEDITDLG)
	case ID_EXT_BTN_IMAGE_EDIT:
	{
		ASSERT( bCanChangeDisplayStyle );
		ASSERT( pIconCurrent != NULL );
		ASSERT( pNodeC->GetIconPtrInToolbar( pCmdItem, true ) != NULL );
		ASSERT( !pNodeC->GetIconPtrInToolbar( pCmdItem, true )->IsEmpty() );
		
		BOOL bMenu = 
			pObjEventSrc->IsKindOf(RUNTIME_CLASS(CExtPopupMenuWnd));

		CExtIconEditDlg dlg(
			bMenu
				? ((CWnd*)pObjEventSrc)
				: CWnd::FromHandle(m_hWndCustomizeForm)
			);
		
		if( bMenu )
		{
			ASSERT_VALID( m_pPopupDropTargetTop );
			ASSERT( m_pPopupDropTargetTop->GetSafeHwnd() != NULL );
			m_pPopupDropTargetTop->_DoShowChain( SW_HIDE );
		}

		dlg.m_icon.AssignFromOther(
			pIconCurrent->IsEmpty()
				? (* pNodeC->GetIconPtrInToolbar( pCmdItem, true ) )
				: (* pIconCurrent )
			);
		ASSERT( ! dlg.m_icon.IsEmpty() );
		{ // BLOCK: load alternate caption string
			CExtSafeString strAlternateCaption;
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
			CExtLocalResourceHelper _LRH;
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
			if( ! g_ResourceManager->LoadString( strAlternateCaption, IDS_EXT_BUTTON_EDITOR_CAPTION ) )
			{
				ASSERT( FALSE );
				strAlternateCaption = _T("Button Editor");
			}
			dlg.m_strAlternateCaption = strAlternateCaption;
		} // BLOCK: load alternate caption string
		
		dlg.m_dwIconViewerStyles |= __EIEDWS_BUTTON_FRAME|__EIEDWS_OUTER_AREA|__EIEDWS_OUTER_TOOLS;
		dlg.m_dwIconEditorStyles &= ~(__EIEDWS_OUTER_TOOLS);
		
		bool bModalOK = ( dlg.DoModal() == IDOK ) ? true : false;

		if( bMenu && m_pPopupDropTargetTop->GetSafeHwnd() != NULL )
			m_pPopupDropTargetTop->_DoShowChain( SW_SHOWNA );

		if( !bModalOK )
			return bTextChanged;
		ASSERT( ! dlg.m_icon.IsEmpty() );
		pIconCurrent->AssignFromOther( dlg.m_icon );
		ASSERT( ! pIconCurrent->IsEmpty() );
		return true;
	}
#endif // (!defined __EXT_MFC_NO_ICONEDITDLG)
	case ID_EXT_BTN_STYLE_DEFAULT:
		ASSERT( bCanChangeDisplayStyle );
		if( (pNodeC->GetFlags() & __ECTN_DISPLAY_MASK)
				== __ECTN_DISPLAY_DEFAULT_STYLE
			)
			return bTextChanged;
		pNodeC->ModifyFlags( 0, __ECTN_DISPLAY_MASK );
		pNodeC->ModifyFlags( __ECTN_DISPLAY_DEFAULT_STYLE );
		return true;
	case ID_EXT_BTN_STYLE_TEXT_ONLY_ALWAYS:
		ASSERT( bCanChangeDisplayStyle );
		if( (pNodeC->GetFlags() & __ECTN_DISPLAY_MASK)
				== __ECTN_DISPLAY_TEXTONLY_ALWAYS
			)
			return bTextChanged;
		pNodeC->ModifyFlags( 0, __ECTN_DISPLAY_MASK );
		pNodeC->ModifyFlags( __ECTN_DISPLAY_TEXTONLY_ALWAYS );
		return true;
	case ID_EXT_BTN_STYLE_TEXT_ONLY_IN_MENUS:
		ASSERT( bCanChangeDisplayStyle );
		if( (pNodeC->GetFlags() & __ECTN_DISPLAY_MASK)
				== __ECTN_DISPLAY_TEXTONLY_IN_MENU
			)
			return bTextChanged;
		pNodeC->ModifyFlags( 0, __ECTN_DISPLAY_MASK );
		pNodeC->ModifyFlags( __ECTN_DISPLAY_TEXTONLY_IN_MENU );
		return true;
	case ID_EXT_BTN_STYLE_IMAGE_AND_TEXT:
		ASSERT( bCanChangeDisplayStyle );
		if( (pNodeC->GetFlags() & __ECTN_DISPLAY_MASK)
				== __ECTN_DISPLAY_TEXT_AND_IMAGE
			)
			return bTextChanged;
		pNodeC->ModifyFlags( 0, __ECTN_DISPLAY_MASK );
		pNodeC->ModifyFlags( __ECTN_DISPLAY_TEXT_AND_IMAGE );
		return true;
	case ID_EXT_BTN_BEGIN_A_GROUP:
		ASSERT( bCanAddRemoveSeparator );
		if( pNodeC->GetFlags() & __ECTN_GROUP_START )
			pNodeC->ModifyFlags( 0, __ECTN_GROUP_START );
		else
			pNodeC->ModifyFlags( __ECTN_GROUP_START );
		return true;
#ifdef _DEBUG
	case ID_EXT_BTN_NAME:
		ASSERT( FALSE );
		return bTextChanged;
	default:
		ASSERT( FALSE );
		//return bTextChanged;
	break;
#endif // _DEBUG
	} // switch( nCmdRetVal )
	return bTextChanged;
}

CExtSafeString CExtCustomizeSite::OnFormatCmdAccelText(
	const CExtCustomizeSite::CCmdKeyInfo & _cmdKeyInfo
	)
{
HKL hKeyboardLayout =
	::GetKeyboardLayout(
		( ::AfxGetThread() ) -> m_nThreadID
		);
static TCHAR strTextBetweenKeys[] = _T("+");
CExtSafeString strAccelText;
	if( _cmdKeyInfo.IsCtrl() )
		strAccelText +=
			CExtCustomizeHotKeyWnd::stat_GetSingleKeyName(
				hKeyboardLayout,
				VK_CONTROL,
				strTextBetweenKeys
				);
	if( _cmdKeyInfo.IsShift() )
		strAccelText +=
			CExtCustomizeHotKeyWnd::stat_GetSingleKeyName(
				hKeyboardLayout,
				VK_SHIFT,
				strTextBetweenKeys
				);
	if( _cmdKeyInfo.IsAlt() )
		strAccelText +=
			CExtCustomizeHotKeyWnd::stat_GetSingleKeyName(
				hKeyboardLayout,
				VK_MENU,
				strTextBetweenKeys
				);
	if( _cmdKeyInfo.IsVirt() )
		strAccelText +=
			CExtCustomizeHotKeyWnd::stat_GetSingleKeyName(
				hKeyboardLayout,
				_cmdKeyInfo.GetKey()
				);
	else
		strAccelText += (TCHAR)_cmdKeyInfo.GetKey();
	return strAccelText;
}

static CExtSafeString productsection2regkeypath(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
	)
{
	return CExtCmdManager::GetSubSystemRegKeyPath(
		__PROF_UIS_REG_CUSTOMIZATION,
		sProfileName,
		sSectionNameCompany,
		sSectionNameProduct
		);
}

bool CExtCustomizeSite::CustomizeStateLoad(
	__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProfile  // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%\%sSectionNameProfile%
	)
{
	ASSERT( this != NULL );
	ASSERT( sSectionNameCompany != NULL );
	ASSERT( sSectionNameProduct != NULL );
	ASSERT( sSectionNameProfile != NULL );
	if( !m_bInitComplete )
	{
		ASSERT( FALSE );
		return false;
	}
	ASSERT_VALID( m_pWndTop );
	ASSERT( m_pWndTop->GetSafeHwnd() != NULL && ::IsWindow(m_pWndTop->GetSafeHwnd()) );
	ASSERT( !IsCustomizeMode() );
	ASSERT( DraggedNodeGet() == NULL );
	ASSERT( CustomizedNodeGet() == NULL );
	ASSERT( MenuInfoActiveGet() == NULL );
	ASSERT_VALID( m_pNodeToolbarsI );
	ASSERT_VALID( m_pNodeToolbarsC );
	DropTargetPopupCancelEx( -2 );
CExtSafeString sRegKeyPath =
		productsection2regkeypath(
			sSectionNameProfile,
			sSectionNameCompany,
			sSectionNameProduct
			);
bool bRetVal = false;
	try
	{
		// prepare memory file and archive,
		// get information from registry
		CMemFile _file;
		if( !CExtCmdManager::FileObjFromRegistry(
				_file,
				sRegKeyPath
				)
			)
			return false;
		CArchive ar(
			&_file,
			CArchive::load
			);
		// do serialization
		CustomizeStateSerialize( ar );
		bRetVal = true;
	} // try
	catch( CException * pXept )
	{
		pXept->Delete();
		ASSERT( FALSE );
	} // catch( CException * pXept )
	catch( ... )
	{
		ASSERT( FALSE );
	} // catch( ... )
	return bRetVal;
}

bool CExtCustomizeSite::CustomizeStateSave(
	__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProfile  // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%\%sSectionNameProfile%\%sSectionNameProfile%
	)
{
	ASSERT( this != NULL );
	ASSERT( sSectionNameCompany != NULL );
	ASSERT( sSectionNameProduct != NULL );
	ASSERT( sSectionNameProfile != NULL );
	if( !m_bInitComplete )
	{
		ASSERT( FALSE );
		return false;
	}
	ASSERT_VALID( m_pWndTop );
	ASSERT( m_pWndTop->GetSafeHwnd() != NULL && ::IsWindow(m_pWndTop->GetSafeHwnd()) );
	ASSERT( !IsCustomizeMode() );
	ASSERT( DraggedNodeGet() == NULL );
	ASSERT( CustomizedNodeGet() == NULL );
	ASSERT( MenuInfoActiveGet() == NULL );
	ASSERT_VALID( m_pNodeToolbarsI );
	ASSERT_VALID( m_pNodeToolbarsC );
	DropTargetPopupCancelEx( -2 );
CExtSafeString sRegKeyPath =
		productsection2regkeypath(
			sSectionNameProfile,
			sSectionNameCompany,
			sSectionNameProduct
			);

bool bRetVal = false;
	try
	{
		// prepare memory file and archive
		CMemFile _file;
		CArchive ar(
			&_file,
			CArchive::store
			);
		// do serialization
		CustomizeStateSerialize( ar );
		// OK, serialization passed
		ar.Flush();
		ar.Close();
		// put information to registry
		bRetVal =
			CExtCmdManager::FileObjToRegistry(
				_file,
				sRegKeyPath
				);
	} // try
	catch( CException * pXept )
	{
		pXept->Delete();
		ASSERT( FALSE );
	} // catch( CException * pXept )
	catch( ... )
	{
		ASSERT( FALSE );
	} // catch( ... )
	return bRetVal;
}

void CExtCustomizeSite::CustomizeStateSerialize( CArchive & ar )
{
	ASSERT( this != NULL );
	ASSERT( m_bInitComplete );
	ASSERT_VALID( m_pWndTop );
	ASSERT( m_pWndTop->GetSafeHwnd() != NULL && ::IsWindow(m_pWndTop->GetSafeHwnd()) );
	ASSERT( !IsCustomizeMode() );
	ASSERT( DraggedNodeGet() == NULL );
	ASSERT( CustomizedNodeGet() == NULL );
	ASSERT( MenuInfoActiveGet() == NULL );
	ASSERT_VALID( m_pNodeToolbarsI );
	ASSERT_VALID( m_pNodeToolbarsC );
	DropTargetPopupCancelEx( -2 );
DWORD dwApiVer0 = g_CmdManager.GetVersionDWORD( false );
DWORD dwApiVer1 = g_CmdManager.GetVersionDWORD( true );
DWORD dwReserved = 0L;
	if( ar.IsStoring() )
	{
		// serialize version info
		CExtSafeString sTmpBuffer;
		CExtSafeString sFriendlyVer;
		sFriendlyVer.Format(
			_T("Prof-UIS (v. %s) customization subsystem state"),
			g_CmdManager.GetVersionString( sTmpBuffer )
			);
		ar << sFriendlyVer;
		ar << dwApiVer0;
		ar << dwApiVer1;
		ar << dwReserved;
		ar << dwReserved;
		ar << dwReserved;
		ar << dwReserved;
		
		// serialize menus
		DWORD dwMenuInfoCount = (DWORD)MenuInfoGetCount();
		ar << dwMenuInfoCount;
		for( DWORD i = 0; i < dwMenuInfoCount; i++ )
		{
			CCmdMenuInfo * pCmdMenuInfo =
				MenuInfoGetAt( i );
			ASSERT( pCmdMenuInfo != NULL );
			CExtCustomizeCmdTreeNode * pMenuNode =
				pCmdMenuInfo->GetNode( false );
			ASSERT_VALID( pMenuNode );
			pMenuNode->Serialize( ar );
			pCmdMenuInfo->AccelTableSerialize( ar );
		} // for( DWORD i = 0; i < dwMenuInfoCount; i++ )

		// serialize toolbars
		CPtrList _listBarsBasic, _listBarsUser;
		BuildToolbarLists( &_listBarsBasic, &_listBarsUser );
		DWORD dwCountBarsBasic = _listBarsBasic.GetCount();
		DWORD dwCountBarsUser = _listBarsUser.GetCount();
		ar << dwCountBarsBasic;
		ar << dwCountBarsUser;
		for(	POSITION pos = _listBarsBasic.GetHeadPosition();
				pos != NULL;
			)
		{
			CExtToolControlBar * pBar = (CExtToolControlBar *)
				_listBarsBasic.GetNext( pos );
			ASSERT_VALID( pBar );
			ASSERT( pBar->GetSafeHwnd() != NULL && ::IsWindow(pBar->GetSafeHwnd()) );
			UINT nBarID = (UINT)pBar->GetDlgCtrlID();
			ar << nBarID;
			CExtCustomizeCmdTreeNode * pBarNode =
				GetToolbarCmdNode( pBar, false );
			if( pBarNode == NULL )
				return;
			ASSERT_VALID( pBarNode );
			pBarNode->Serialize( ar );
		}
		for(	pos = _listBarsUser.GetHeadPosition();
				pos != NULL;
			)
		{
			CExtToolControlBar * pBar = (CExtToolControlBar *)
				_listBarsUser.GetNext( pos );
			ASSERT_VALID( pBar );
			ASSERT( pBar->GetSafeHwnd() != NULL && ::IsWindow(pBar->GetSafeHwnd()) );
			UINT nBarID = (UINT)pBar->GetDlgCtrlID();
			ar << nBarID;
			CString strBarName;
			CExtCmdItem * pCmdItem =
				g_CmdManager->CmdGetPtr(
					g_CmdManager->ProfileNameFromWnd( pBar->m_hWnd ),
					(UINT)pBar->GetDlgCtrlID()
					);
			if(		pCmdItem != NULL
				&&	(! pCmdItem->m_sMenuText.IsEmpty() )
				)
				strBarName = pCmdItem->m_sMenuText;
			else
				pBar->GetWindowText( strBarName );
			ASSERT( !strBarName.IsEmpty() );
			ar << strBarName;
			CExtCustomizeCmdTreeNode * pBarNode =
				GetToolbarCmdNode( pBar, false );
			if( pBarNode == NULL )
				return;
			ASSERT_VALID( pBarNode );
			pBarNode->Serialize( ar );
		}

		if( (GetCustomizeFlags() & __ECSF_PARMS) != 0 )
		{
			if( (GetCustomizeFlags() & __ECSF_PARMS_DISABLE_PERSONALIZED) == 0 )
			{
				DWORD dwPersonalized = 0;
				if( CExtPopupMenuWnd::g_bMenuExpanding )
					dwPersonalized |= 0x00000001;
				if( CExtPopupMenuWnd::g_bMenuDelayExpanding )
					dwPersonalized |= 0x00000002;
				if( CExtPopupMenuWnd::g_bMenuHighlightRarely )
					dwPersonalized |= 0x00000004;
				if( CExtPopupMenuWnd::g_bMenuExpandAnimation )
					dwPersonalized |= 0x00000008;
				if( CExtPopupMenuWnd::g_bMenuWithShadows )
					dwPersonalized |= 0x00000010;
				ar << dwPersonalized;
			} // if( (GetCustomizeFlags() & __ECSF_PARMS_DISABLE_PERSONALIZED) == 0 )
			if( (GetCustomizeFlags() & __ECSF_PARMS_DISABLE_OTHER) == 0 )
			{
				DWORD dwOther = 0;
				if( CExtPopupMenuWnd::g_bMenuLargeIcons )
					dwOther |= 0x00000001;
				if( CExtToolControlBar::g_bToolbarLargeIcons )
					dwOther |= 0x00000002;
				if( CExtPopupMenuWnd::g_bMenuShowCoolTips )
					dwOther |= 0x00000004;
				if( CExtToolControlBar::g_bToolbarScreenTips )
					dwOther |= 0x00000008;
				if( CExtToolControlBar::g_bToolbarShortcutKeysOnScreenTips )
					dwOther |= 0x00000010;
				ar << dwOther;
				DWORD dwMenuAnimationType = CExtPopupMenuWnd::g_DefAnimationType;
				ar << dwMenuAnimationType;
			}
		} // if( (GetCustomizeFlags() & __ECSF_PARMS) != 0 )
	
	} // if( ar.IsStoring() )
	else
	{
		// serialize version info
		DWORD dwApiVer0a = 0, dwApiVer1a = 0;
		CExtSafeString sFriendlyVer;
		ar >> sFriendlyVer;
		ar >> dwApiVer0a;
		ar >> dwApiVer1a;
		ar >> dwReserved;
		ar >> dwReserved;
		ar >> dwReserved;
		ar >> dwReserved;
		if( dwApiVer1 != dwApiVer1a )
		{
			ASSERT( FALSE );
			::AfxThrowArchiveException(
				CArchiveException::generic,
				NULL
				);
		}

		// first of all, delete user bars
		DeleteAllUserBars();

		// serialize menus
		DWORD dwMenuInfoCount = (DWORD)MenuInfoGetCount();
		DWORD dwMenuInfoCountA = 0;
		ar >> dwMenuInfoCountA;
		if( dwMenuInfoCountA != dwMenuInfoCount )
		{
			ASSERT( FALSE );
			::AfxThrowArchiveException(
				CArchiveException::generic,
				NULL
				);
		}
		if( m_pWndMenuBar != NULL )
		{
			ASSERT_VALID( m_pWndMenuBar );
			m_pWndMenuBar->SetButtons();
		}
		for( DWORD i = 0; i < dwMenuInfoCount; i++ )
		{
			CCmdMenuInfo * pCmdMenuInfo =
				MenuInfoGetAt( i );
			ASSERT( pCmdMenuInfo != NULL );
			CExtCustomizeCmdTreeNode * pMenuNode =
				pCmdMenuInfo->GetNode( false );
			ASSERT_VALID( pMenuNode );
			pMenuNode->Serialize( ar );
			pCmdMenuInfo->AccelTableSerialize( ar );
		} // for( DWORD i = 0; i < dwMenuInfoCount; i++ )
		if( m_pWndMenuBar != NULL )
		{
			ASSERT_VALID( m_pWndMenuBar );
			VERIFY( m_pWndMenuBar->UpdateMenuBar( FALSE ) );
		}

		// serialize toolbars
		CPtrList _listBarsBasic;
		BuildToolbarLists( &_listBarsBasic );
		DWORD dwCountBarsBasic = _listBarsBasic.GetCount();
		DWORD dwCountBarsUser = 0;
		DWORD dwCountBarsBasicA = 0;
		ar >> dwCountBarsBasicA;
		ar >> dwCountBarsUser;
		if( dwCountBarsBasicA != dwCountBarsBasic )
		{
			ASSERT( FALSE );
			::AfxThrowArchiveException(
				CArchiveException::generic,
				NULL
				);
		}
		for( i = 0; i < dwCountBarsBasic; i++ )
		{
			UINT nBarID_A = 0;
			ar >> nBarID_A;
			ASSERT( CExtCmdManager::IsCommand(nBarID_A) );
			CExtToolControlBar * pBar = NULL;
			for(	POSITION pos = _listBarsBasic.GetHeadPosition();
					pos != NULL;
				)
			{
				CExtToolControlBar * pBarL = (CExtToolControlBar *)
					_listBarsBasic.GetNext( pos );
				ASSERT_VALID( pBarL );
				ASSERT( pBarL->GetSafeHwnd() != NULL && ::IsWindow(pBarL->GetSafeHwnd()) );
				UINT nBarID = (UINT)pBarL->GetDlgCtrlID();
				if( nBarID_A == nBarID )
				{
					pBar = pBarL;
					break;
				}
			}
			if( pBar == NULL )
			{
				ASSERT( FALSE );
				::AfxThrowArchiveException(
					CArchiveException::generic,
					NULL
					);
			} // if( pBar == NULL )
			bool bRightBtnExist =
				( pBar->GetRightButton() != NULL )
					? true : false;
			pBar->SetButtons();
			CExtCustomizeCmdTreeNode * pBarNode =
				GetToolbarCmdNode( pBar, false );
			if( pBarNode == NULL )
				return;
			ASSERT_VALID( pBarNode );
			pBarNode->Serialize( ar );
			pBar->SetButtons( pBarNode );
			if( bRightBtnExist )
				pBar->InitContentExpandButton();
		} // for( i = 0; i < dwCountBarsBasic; i++ )
		for( i = 0; i < dwCountBarsUser; i++ )
		{
			UINT nBarID = 0;
			ar >> nBarID;
			ASSERT( CExtCmdManager::IsCommand(nBarID) );
			ASSERT(
				g_CmdManager->CmdGetPtr(
					g_CmdManager->ProfileNameFromWnd( m_pWndTop->m_hWnd ),
					nBarID
					) == NULL
				);
			CExtSafeString strBarName;
			ar >> strBarName;
			ASSERT( !strBarName.IsEmpty() );
			CExtToolControlBar * pBar =
				OnUserBarCreate(
					strBarName,
					nBarID,
					true
					);
			if( pBar == NULL )
			{
				ASSERT( FALSE );
				::AfxThrowArchiveException(
					CArchiveException::generic,
					NULL
					);
			} // if( pBar == NULL )
			ASSERT_VALID( pBar );
			ASSERT( pBar->GetSafeHwnd() != NULL && ::IsWindow(pBar->GetSafeHwnd()) );
			CExtCustomizeCmdTreeNode * pBarNode =
				GetToolbarCmdNode( pBar, false );
			if( pBarNode == NULL )
				return;
			ASSERT_VALID( pBarNode );
			pBarNode->Serialize( ar );
			pBar->SetButtons( pBarNode );
			pBar->InitContentExpandButton();
		} // for( i = 0; i < dwCountBarsUser; i++ )

		if( (GetCustomizeFlags()&__ECSF_ACCELERATORS) != 0 )
			OnUpdateAccelGlobalInfo( true );
	
		if( (GetCustomizeFlags() & __ECSF_PARMS) != 0 )
		{
			if( (GetCustomizeFlags() & __ECSF_PARMS_DISABLE_PERSONALIZED) == 0 )
			{
				DWORD dwPersonalized = 0;
				ar >> dwPersonalized;
				CExtPopupMenuWnd::g_bMenuExpanding =
					(dwPersonalized&0x00000001) ? true : false;
				CExtPopupMenuWnd::g_bMenuDelayExpanding =
					(dwPersonalized&0x00000002) ? true : false;
				CExtPopupMenuWnd::g_bMenuHighlightRarely =
					(dwPersonalized&0x00000004) ? true : false;
				CExtPopupMenuWnd::g_bMenuExpandAnimation =
					(dwPersonalized&0x00000008) ? true : false;
				CExtPopupMenuWnd::g_bMenuWithShadows =
					(dwPersonalized&0x00000010) ? true : false;
			} // if( (GetCustomizeFlags() & __ECSF_PARMS_DISABLE_PERSONALIZED) == 0 )
			if( (GetCustomizeFlags() & __ECSF_PARMS_DISABLE_OTHER) == 0 )
			{
				DWORD dwOther = 0;
				ar >> dwOther;
				CExtPopupMenuWnd::g_bMenuLargeIcons =
					(dwOther&0x00000001) ? true : false;
				CExtToolControlBar::g_bToolbarLargeIcons =
					(dwOther&0x00000002) ? true : false;
				CExtPopupMenuWnd::g_bMenuShowCoolTips =
					(dwOther&0x00000004) ? true : false;
				CExtToolControlBar::g_bToolbarScreenTips =
					(dwOther&0x00000008) ? true : false;
				CExtToolControlBar::g_bToolbarShortcutKeysOnScreenTips =
					(dwOther&0x00000010) ? true : false;
				DWORD dwMenuAnimationType = (DWORD)CExtPopupMenuWnd::g_DefAnimationType;
				ar >> dwMenuAnimationType;
				ASSERT( dwMenuAnimationType <= ((DWORD)CExtPopupMenuWnd::__AT_MAX_VALID_VALUE) );
				if( dwMenuAnimationType <= ((DWORD)CExtPopupMenuWnd::__AT_MAX_VALID_VALUE) )
					CExtPopupMenuWnd::g_DefAnimationType =
						(CExtPopupBaseWnd::e_animation_type_t)
							dwMenuAnimationType;
			}
		} // if( (GetCustomizeFlags() & __ECSF_PARMS) != 0 )
	
	} // else from if( ar.IsStoring() )
}

INT CExtCustomizeSite::RedrawCommandItems(
	UINT nCmdID,
	bool bUpdateWindows // = true
	) const
{
	ASSERT( CExtCmdManager::IsCommand(nCmdID) );
INT nUpdateBtnsCount = 0;
	for(	POSITION pos = m_mapToolBars.GetStartPosition();
			pos != NULL;
			)
	{
		CExtToolControlBar * pBar = NULL;
		CToolBarCustomizeInfo * pInfo = NULL;
		m_mapToolBars.GetNextAssoc( pos, (void * &)pBar, (void * &)pInfo );
		ASSERT( pBar != NULL );
		ASSERT( pInfo != NULL );
		if( !pBar->IsVisible() )
			continue;
		INT nBtnsCount = pBar->GetButtonsCount();
		if( nBtnsCount == 0 )
			continue;
		bool bUpdateBar = false;
		for( INT nBtnIdx = 0; nBtnIdx < nBtnsCount; nBtnIdx++ )
		{
			CExtBarButton * pTBB = pBar->GetButton( nBtnIdx );
			ASSERT_VALID( pTBB );
			UINT nTbbCmdID = pTBB->GetCmdID( false );
			if(		nTbbCmdID != nCmdID
				||	(! pTBB->IsVisible() )
				||	(pTBB->GetStyle()&TBBS_HIDDEN) != 0
				)
				continue;
			nUpdateBtnsCount ++;
			bUpdateBar = true;
			if( pTBB->IsKindOf(RUNTIME_CLASS(CExtBarColorButton)) )
				((CExtBarColorButton*)pTBB)->OnSyncIcon();
			pTBB->RedrawButton( false );
		} // for( INT nBtnIdx = 0; nBtnIdx < nBtnsCount; nBtnIdx++ )
		if( bUpdateWindows && bUpdateBar )
			pBar->UpdateWindow();
	}
	return nUpdateBtnsCount;
}

INT CExtCustomizeSite::UpdateAllCommandItems() const
{
INT nUpdatedBarsCount = 0;
	for(	POSITION pos = m_mapToolBars.GetStartPosition();
			pos != NULL;
			)
	{
		CExtToolControlBar * pBar = NULL;
		CToolBarCustomizeInfo * pInfo = NULL;
		m_mapToolBars.GetNextAssoc( pos, (void * &)pBar, (void * &)pInfo );
		ASSERT( pBar != NULL );
		ASSERT( pInfo != NULL );
		if( !pBar->IsVisible() )
			continue;
		nUpdatedBarsCount++;
		pBar->DoCustomModeUpdateCmdUI();
		pBar->UpdateWindow();
	}
	return nUpdatedBarsCount;
}

#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)

CSize CExtCustomizeSite::OnPopupListBoxCalcItemExtraSizes(
	const CExtBarButton * pTBB,
	const CExtCustomizeCmdTreeNode * pNode
	) const
{
#ifdef _DEBUG
	if( pTBB != NULL )
	{
		ASSERT_VALID( pTBB );
		ASSERT_VALID( pTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	pTBB;
	pNode;
	return CSize( 2, 1 ); // default sizes
}

bool CExtCustomizeSite::OnPopupListBoxInitContent(
	CExtBarButton * pTBB,
	CExtCustomizeCmdTreeNode * pNode,
	CListBox & wndListBox
	)
{
#ifdef _DEBUG
	if( pTBB != NULL )
	{
		ASSERT_VALID( pTBB );
		ASSERT_VALID( pTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	ASSERT_VALID( (&wndListBox) );
	pTBB;
	pNode;
	wndListBox;
	return false; // this is not our business, let the bar button initialize it
}

CSize CExtCustomizeSite::OnPopupListBoxMeasureTrackSize(
	const CExtBarButton * pTBB,
	const CExtCustomizeCmdTreeNode * pNode
	) const
{
#ifdef _DEBUG
	if( pTBB != NULL )
	{
		ASSERT_VALID( pTBB );
		ASSERT_VALID( pTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	pTBB;
	pNode;
	return CSize( 0, 0 ); // this is not our business, let the bar button compute it
}

bool CExtCustomizeSite::OnPopupListBoxItemDraw(
	CExtBarButton * pTBB,
	CExtCustomizeCmdTreeNode * pNode,
	CListBox & wndListBox,
	LPDRAWITEMSTRUCT pDIS
	)
{
#ifdef _DEBUG
	if( pTBB != NULL )
	{
		ASSERT_VALID( pTBB );
		ASSERT_VALID( pTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	ASSERT_VALID( (&wndListBox) );
	ASSERT( pDIS != NULL && pDIS->hDC != NULL );
	pTBB;
	pNode;
	wndListBox;
	pDIS;
	return false; // this is not our business, let the bar button paint it
}

bool CExtCustomizeSite::OnPopupListBoxItemMeasure(
	CExtBarButton * pTBB,
	CExtCustomizeCmdTreeNode * pNode,
	CListBox & wndListBox,
	LPMEASUREITEMSTRUCT pMIS
	)
{
#ifdef _DEBUG
	if( pTBB != NULL )
	{
		ASSERT_VALID( pTBB );
		ASSERT_VALID( pTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	ASSERT_VALID( (&wndListBox) );
	ASSERT( pMIS != NULL );
	pTBB;
	pNode;
	wndListBox;
	pMIS;
	return false; // this is not our business, let the bar button measure it
}

bool CExtCustomizeSite::OnPopupListBoxSelChange(
	CExtBarButton * pTBB,
	CExtCustomizeCmdTreeNode * pNode,
	CListBox & wndListBox
	)
{
#ifdef _DEBUG
	if( pTBB != NULL )
	{
		ASSERT_VALID( pTBB );
		ASSERT_VALID( pTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	ASSERT_VALID( (&wndListBox) );
	pTBB;
	pNode;
	wndListBox;
	return false; // this is not our business, let the bar button handle it
}

bool CExtCustomizeSite::OnPopupListBoxSelEndOK(
	CExtBarButton * pTBB,
	CExtCustomizeCmdTreeNode * pNode,
	CListBox & wndListBox
	)
{
#ifdef _DEBUG
	if( pTBB != NULL )
	{
		ASSERT_VALID( pTBB );
		ASSERT_VALID( pTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	ASSERT_VALID( (&wndListBox) );
	if( pTBB != NULL )
		return false; // this is not our business, let the bar button handle it
	if(		(wndListBox.GetStyle() & LBS_HASSTRINGS) != 0
		&&	wndListBox.GetCount() > 0
		)
	{
		int nCurSel = wndListBox.GetCurSel();
		if( nCurSel >= 0 )
		{
			CString sNewText;
			wndListBox.GetText( nCurSel, *((CString*)&sNewText) );
			OnTextFieldInplaceTextSet(
				NULL,
				pNode,
				pNode->m_sDefInplaceEditBuffer,
				(LPCTSTR)sNewText
				);
		}
	}
	return false;
}

bool CExtCustomizeSite::OnPopupListBoxSelEndCancel(
	CExtBarButton * pTBB,
	CExtCustomizeCmdTreeNode * pNode,
	CListBox & wndListBox
	)
{
#ifdef _DEBUG
	if( pTBB != NULL )
	{
		ASSERT_VALID( pTBB );
		ASSERT_VALID( pTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	ASSERT_VALID( (&wndListBox) );
	pTBB;
	pNode;
	wndListBox;
	return false; // this is not our business, let the bar button handle it
}

bool CExtCustomizeSite::OnPopupListBoxGetStyles(
	CExtBarButton * pTBB,
	CExtCustomizeCmdTreeNode * pNode,
	DWORD & dwListBoxStyles
	) const
{
#ifdef _DEBUG
	if( pTBB != NULL )
	{
		ASSERT_VALID( pTBB );
		ASSERT_VALID( pTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	pTBB;
	pNode;
	dwListBoxStyles;
	return false; // this is not our business, let the bar button install required styles
}

bool CExtCustomizeSite::OnTextFieldVerify(
	CExtBarTextFieldButton * pTextFieldTBB,
	CExtCustomizeCmdTreeNode * pNode,
	__EXT_MFC_SAFE_LPCTSTR sTextOld,
	__EXT_MFC_SAFE_LPCTSTR sTextNew
	)
{
#ifdef _DEBUG
	if( pTextFieldTBB != NULL )
	{
		ASSERT_VALID( pTextFieldTBB );
		ASSERT_VALID( pTextFieldTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	ASSERT( sTextOld != NULL );
	ASSERT( sTextNew != NULL );
	pTextFieldTBB;
	pNode;
	sTextOld;
	sTextNew;
	return true;
}

void CExtCustomizeSite::OnTextFieldInplaceTextGet(
	const CExtBarTextFieldButton * pTextFieldTBB,
	const CExtCustomizeCmdTreeNode * pNode,
	CExtSafeString & sTextFieldBuffer
	)
{
#ifdef _DEBUG
	if( pTextFieldTBB != NULL )
	{
		ASSERT_VALID( pTextFieldTBB );
		ASSERT_VALID( pTextFieldTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	pTextFieldTBB;
	pNode;
	sTextFieldBuffer;
}

void CExtCustomizeSite::OnTextFieldInplaceTextSet(
	CExtBarTextFieldButton * pTextFieldTBB,
	CExtCustomizeCmdTreeNode * pNode,
	CExtSafeString & sTextFieldBuffer,
	__EXT_MFC_SAFE_LPCTSTR sTextNew
	)
{
#ifdef _DEBUG
	if( pTextFieldTBB != NULL )
	{
		ASSERT_VALID( pTextFieldTBB );
		ASSERT_VALID( pTextFieldTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	ASSERT( sTextNew != NULL );
	pTextFieldTBB;
	pNode;
	sTextFieldBuffer = sTextNew;
	CExtPopupMenuWnd * pPopup =
		CExtPopupMenuWnd::GetTrackingMenu();
	if(		pPopup->GetSafeHwnd() != NULL
		&&	::IsWindow( pPopup->GetSafeHwnd() )
		)
	{
		::PostMessage(
			pPopup->GetSafeHwnd(),
			WM_CANCELMODE,
			0,
			0
			);
	}
}

bool CExtCustomizeSite::OnTextFieldWndProcHook(
	LRESULT & lResult,
	UINT message,
	WPARAM wParam,
	LPARAM lParam,
	CEdit & wndEdit,
	CExtBarTextFieldButton * pTextFieldTBB,
	CExtCustomizeCmdTreeNode * pNode
	)
{
#ifdef _DEBUG
	if( pTextFieldTBB != NULL )
	{
		ASSERT_VALID( pTextFieldTBB );
		ASSERT_VALID( pTextFieldTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	lResult;
	message;
	wParam;
	lParam;
	wndEdit;
	pTextFieldTBB;
	pNode;
	return false; // normally text field does not needs functionality modification
}

CExtPopupControlMenuWnd * CExtCustomizeSite::OnTextFieldCreateDropPopup(
	CExtBarTextFieldButton * pTextFieldTBB,
	CExtCustomizeCmdTreeNode * pNode,
	HWND hWndCmdReceiver,
	bool bContentExpanding
	)
{
#ifdef _DEBUG
	if( pTextFieldTBB != NULL )
	{
		ASSERT_VALID( pTextFieldTBB );
		ASSERT_VALID( pTextFieldTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	pTextFieldTBB;
	pNode;
	hWndCmdReceiver;
	bContentExpanding;
	return NULL; // allow menus and text fields to create default list-box menus
}

void CExtCustomizeSite::OnGetCmdItemMinMaxSizeH(
	CExtBarButton * pTBB,
	CExtCustomizeCmdTreeNode * pNode,
	int * p_nWidthMin, // = NULL
	int * p_nWidthMax // = NULL
	)
{
#ifdef _DEBUG
	if( pTBB != NULL )
	{
		ASSERT_VALID( pTBB );
		ASSERT_VALID( pTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	ASSERT( (pNode->GetFlags() & __ECTN_TBB_RESIZABLE) != 0 );
	pTBB;
	pNode;
	if( p_nWidthMin != NULL )
		*p_nWidthMin =
			(::GetSystemMetrics( SM_CXHTHUMB ))
			* 2;
	if( p_nWidthMax != NULL )
		*p_nWidthMax = 32767;
}

bool CExtCustomizeSite::OnCmdNodeResizeH(
	int nInitialResizingStateH,
	HWND hWndBase,
	CExtBarButton * pTBB,
	CExtCustomizeCmdTreeNode * pNode,
	int nWidthMin,
	int nWidthMax,
	CRect & rcItem
	)
{
	ASSERT( nInitialResizingStateH != 0 );
	ASSERT( hWndBase != NULL && ::IsWindow(hWndBase) );
#ifdef _DEBUG
	if( pTBB != NULL )
	{
		ASSERT_VALID( pTBB );
		ASSERT_VALID( pTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	ASSERT( (pNode->GetFlags() & __ECTN_TBB_RESIZABLE) != 0 );
	ASSERT( nWidthMin > 0 && nWidthMin <= nWidthMax );
	ASSERT( !rcItem.IsRectEmpty() );
	if( m_pHelperResizingNode != NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	pTBB;
	m_pHelperResizingNode = pNode;
CResizerWnd _resizer( true );
CRect rcItemPrev( rcItem );
bool bRetVal = 
		_resizer.DoResizerTracking(
			hWndBase,
			rcItem,
			nInitialResizingStateH,
			nWidthMin,
			nWidthMax
			);
	m_pHelperResizingNode = NULL;
	if( bRetVal )
	{
		INT nAdjustVal = rcItem.Width() - rcItemPrev.Width();
		if( nAdjustVal == 0 )
			bRetVal = false;
		else
		{
			INT nTextFieldWidth = pNode->TextFieldWidthGet();
			nTextFieldWidth += nAdjustVal;
			if( nTextFieldWidth < nWidthMin )
				nTextFieldWidth = nWidthMin;
			else if( nTextFieldWidth > nWidthMax )
				nTextFieldWidth = nWidthMax;
			pNode->TextFieldWidthSet( nTextFieldWidth );
		} // else from if( nAdjustVal == 0 )
	} // if( bRetVal )
	return bRetVal;
}

CExtCustomizeCmdTreeNode * CExtCustomizeSite::GetResizingNode()
{
#ifdef _DEBUG
	if( m_pHelperResizingNode != NULL )
	{
		ASSERT_VALID( m_pHelperResizingNode );
	}
#endif // _DEBUG
	return m_pHelperResizingNode;
}

#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)

HICON CExtCustomizeSite::OnColorItemGenerateIcon(
	CExtBarColorButton * pTBB,
	CExtCustomizeCmdTreeNode * pNode,
	COLORREF clr
	)
{
#ifdef _DEBUG
	if( pTBB != NULL )
	{
		ASSERT_VALID( pTBB );
		ASSERT_VALID( pTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	pTBB;
	pNode;
HICON hIcon =
		CExtPaintManager::stat_GenerateColorIconBox( clr );
	ASSERT( hIcon != NULL );
	return hIcon;
}

bool CExtCustomizeSite::OnColorItemValueGet(
	CExtBarColorButton * pTBB,
	CExtCustomizeCmdTreeNode * pNode,
	COLORREF & clr,
	bool bSelected
	)
{
#ifdef _DEBUG
	if( pTBB != NULL )
	{
		ASSERT_VALID( pTBB );
		ASSERT_VALID( pTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	pTBB;
	pNode;
	clr;
	bSelected;
	return false;
}

bool CExtCustomizeSite::OnColorItemValueSet(
	CExtBarColorButton * pTBB,
	CExtCustomizeCmdTreeNode * pNode,
	COLORREF & clr,
	bool bSelected
	)
{
#ifdef _DEBUG
	if( pTBB != NULL )
	{
		ASSERT_VALID( pTBB );
		ASSERT_VALID( pTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	pTBB;
	pNode;
	clr;
	bSelected;
	return false;
}

bool CExtCustomizeSite::OnColorItemChanged(
	CExtBarColorButton * pTBB,
	CExtCustomizeCmdTreeNode * pNode,
	bool bFinalChanging,
	COLORREF clr,
	LPARAM lParam
	)
{
#ifdef _DEBUG
	if( pTBB != NULL )
	{
		ASSERT_VALID( pTBB );
		ASSERT_VALID( pTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	lParam;
	if( !bFinalChanging )
		return true;
	return
		OnColorItemValueSet(
			pTBB,
			pNode,
			clr,
			true //false
			);
}

bool CExtCustomizeSite::OnColorItemCustom(
	CExtCustomizeCmdTreeNode * pNode
	)
{
	ASSERT_VALID( pNode );
	pNode;
	return false;
}

bool CExtCustomizeSite::OnColorItemGetBtnTextDefault(
	CExtBarColorButton * pTBB,
	CExtCustomizeCmdTreeNode * pNode,
	CExtSafeString & sBtnText
	)
{
#ifdef _DEBUG
	if( pTBB != NULL )
	{
		ASSERT_VALID( pTBB );
		ASSERT_VALID( pTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	pTBB;
	pNode;
	sBtnText;
	return false;
}

bool CExtCustomizeSite::OnColorItemGetBtnTextCustom(
	CExtBarColorButton * pTBB,
	CExtCustomizeCmdTreeNode * pNode,
	CExtSafeString & sBtnText
	)
{
#ifdef _DEBUG
	if( pTBB != NULL )
	{
		ASSERT_VALID( pTBB );
		ASSERT_VALID( pTBB->GetBar() );
	}
#endif // _DEBUG
	ASSERT_VALID( pNode );
	pTBB;
	pNode;
	sBtnText;
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CExtCustomizeToolbarsListBox window

IMPLEMENT_DYNCREATE(CExtCustomizeToolbarsListBox,CCheckListBox);

CExtCustomizeToolbarsListBox::CExtCustomizeToolbarsListBox()
{
}

CExtCustomizeToolbarsListBox::~CExtCustomizeToolbarsListBox()
{
}

BEGIN_MESSAGE_MAP(CExtCustomizeToolbarsListBox,CCheckListBox)
    //{{AFX_MSG_MAP(CExtCustomizeToolbarsListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_MESSAGE( (WM_USER+100), OnDelayedRescanBars )
END_MESSAGE_MAP()

void CExtCustomizeToolbarsListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	CCheckListBox::OnLButtonDown(nFlags,point);
	SyncBars();
}

void CExtCustomizeToolbarsListBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CCheckListBox::OnLButtonDblClk(nFlags,point);
	SyncBars();
}

void CExtCustomizeToolbarsListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CCheckListBox::OnKeyDown(nChar,nRepCnt,nFlags);
	SyncBars();
}

void CExtCustomizeToolbarsListBox::SyncBars()
{
	ASSERT_VALID( this );
	ASSERT( m_hWnd != NULL && ::IsWindow(m_hWnd) );
int nCount = CCheckListBox::GetCount();
	for( int nPos = 0; nPos < nCount; nPos++ )
		UpdateBarFromCheck( nPos );
}

void CExtCustomizeToolbarsListBox::DelayRescanBars()
{
	ASSERT_VALID( this );
	ASSERT( m_hWnd != NULL && ::IsWindow(m_hWnd) );
	PostMessage( (WM_USER+100) );
}

LRESULT CExtCustomizeToolbarsListBox::OnDelayedRescanBars(WPARAM wParam, LPARAM lParam)
{
	wParam;
	lParam;
	ASSERT_VALID( this );
	ASSERT( m_hWnd != NULL && ::IsWindow(m_hWnd) );
int nCount = CCheckListBox::GetCount();
	for( int nPos = 0; nPos < nCount; nPos++ )
		UpdateCheckFromBar( nPos );
	return 0L;
}

int CExtCustomizeToolbarsListBox::AddBar(
	CExtControlBar * pExtBar,
	__EXT_MFC_SAFE_LPCTSTR strBarName // = NULL // detect automatically
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pExtBar );
	ASSERT( m_hWnd != NULL && ::IsWindow(m_hWnd) );
	ASSERT( pExtBar->m_hWnd != NULL && ::IsWindow(pExtBar->m_hWnd) );

int nPos = FindBarPos( pExtBar );
	if( nPos >= 0 )
	{
		UpdateCheckFromBar( nPos );
		return nPos;
	}

CString _strBarName;
	if(		strBarName != NULL
		&&	_tcslen(strBarName) > 0
		)
	{
		_strBarName = strBarName;
		_strBarName.Replace( _T("&"), _T("") );
	}

	if( _strBarName.IsEmpty() )
	{
		CExtCmdItem * pCmdItem =
			g_CmdManager->CmdGetPtr(
				g_CmdManager->ProfileNameFromWnd( pExtBar->m_hWnd ),
				(UINT)pExtBar->GetDlgCtrlID()
				);
		if( pCmdItem != NULL )
		{
			_strBarName = pCmdItem->OnGetToolBarCustomizeName();
			_strBarName.Replace( _T("&"), _T("") );
		} // if( pCmdItem != NULL )
	} // if( _strBarName.IsEmpty() )

	if( _strBarName.IsEmpty() )
	{
		pExtBar->GetWindowText( _strBarName );
		_strBarName.Replace( _T("&"), _T("") );
		ASSERT( !_strBarName.IsEmpty() );
	} // if( _strBarName.IsEmpty() )
	nPos = CCheckListBox::GetCount();
	CCheckListBox::InsertString( nPos, _strBarName );
	CCheckListBox::SetItemData( nPos, (DWORD)pExtBar );
	UpdateCheckFromBar( nPos );
	return nPos;
}

void CExtCustomizeToolbarsListBox::UpdateBarFromCheck( int nPos )
{
	ASSERT_VALID( this );
	ASSERT( m_hWnd != NULL && ::IsWindow(m_hWnd) );
	ASSERT( 0 <= nPos && nPos < CCheckListBox::GetCount() );
CExtControlBar * pExtBar = (CExtControlBar *)
		CCheckListBox::GetItemData( nPos );
	ASSERT_VALID( pExtBar );
	ASSERT_KINDOF( CExtControlBar, pExtBar );
	ASSERT( pExtBar->m_hWnd != NULL && ::IsWindow(pExtBar->m_hWnd) );

CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
CWnd * pWndTop = pSite->GetCustomizeTopWnd();
	ASSERT_VALID( pWndTop );
	ASSERT( pWndTop->m_hWnd != NULL && ::IsWindow(pWndTop->m_hWnd) );

CExtCmdItem * pCmdItem =
		g_CmdManager->CmdGetPtr(
			g_CmdManager->ProfileNameFromWnd( pExtBar->m_hWnd ),
			(UINT)pExtBar->GetDlgCtrlID()
			);
bool bCheckUpdated = false;
	if(		pCmdItem != NULL
		&&	pCmdItem->DoUpdateCmdUI( pWndTop )
		)
		bCheckUpdated =
			(	pCmdItem->StateGetCheck()
			||	pCmdItem->StateGetRadio()
			) ? true : false;
	else
		bCheckUpdated =
			pExtBar->IsVisible() ? true : false;
bool bCheckReal =
		CCheckListBox::GetCheck( nPos ) ? true : false;
	if( bCheckUpdated == bCheckReal )
		return;

	pWndTop->SendMessage( WM_COMMAND, (UINT)pExtBar->GetDlgCtrlID() );

	pExtBar->SendMessage( WM_CANCELMODE );
	if(		pExtBar->IsVisible()
		&&	pExtBar->IsFloating()
		)
	{
		pExtBar->NcButtons_HitTest( CPoint(0,0) );
		pExtBar->GetParentFrame()->SendMessage( WM_NCPAINT );
	}
}

void CExtCustomizeToolbarsListBox::UpdateCheckFromBar( int nPos )
{
	ASSERT_VALID( this );
	ASSERT( m_hWnd != NULL && ::IsWindow(m_hWnd) );
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
CWnd * pWndTop = pSite->GetCustomizeTopWnd();
	ASSERT_VALID( pWndTop );
	ASSERT( pWndTop->m_hWnd != NULL && ::IsWindow(pWndTop->m_hWnd) );
	ASSERT( 0 <= nPos && nPos < CCheckListBox::GetCount() );
CExtControlBar * pExtBar = (CExtControlBar *)
		CCheckListBox::GetItemData( nPos );
	ASSERT_VALID( pExtBar );
	ASSERT_KINDOF( CExtControlBar, pExtBar );
	ASSERT( pExtBar->m_hWnd != NULL && ::IsWindow(pExtBar->m_hWnd) );

//CExtCmdItem * pCmdItem =
//		g_CmdManager->CmdGetPtr(
//			g_CmdManager->ProfileNameFromWnd( pExtBar->m_hWnd ),
//			(UINT)pExtBar->GetDlgCtrlID()
//			);
bool bCheckUpdated = false;
//	if(		pCmdItem != NULL
//		&&	pCmdItem->DoUpdateCmdUI( pWndTop )
//		)
//		bCheckUpdated =
//			(	pCmdItem->StateGetCheck()
//			||	pCmdItem->StateGetRadio()
//			) ? true : false;
//	else
		bCheckUpdated =
			pExtBar->IsVisible() ? true : false;
bool bCheckReal =
		CCheckListBox::GetCheck( nPos ) ? true : false;
	if( bCheckUpdated == bCheckReal )
		return;
	CCheckListBox::SetCheck(
		nPos,
		bCheckUpdated ? 1 : 0
		);
}

int CExtCustomizeToolbarsListBox::FindBarPos(
	CExtControlBar * pExtBarSearch
	)
{
	ASSERT_VALID( this );
	ASSERT( m_hWnd != NULL && ::IsWindow(m_hWnd) );
	ASSERT_VALID( pExtBarSearch );
	ASSERT_KINDOF( CExtControlBar, pExtBarSearch );
	ASSERT( pExtBarSearch->m_hWnd != NULL && ::IsWindow(pExtBarSearch->m_hWnd) );
int nCount = CCheckListBox::GetCount();
	for( int nPos = 0; nPos < nCount; nPos++ )
	{
		CExtControlBar * pExtBar = (CExtControlBar *)
			CCheckListBox::GetItemData( nPos );
		ASSERT_VALID( pExtBar );
		ASSERT_KINDOF( CExtControlBar, pExtBar );
		ASSERT( pExtBar->m_hWnd != NULL && ::IsWindow(pExtBar->m_hWnd) );
		if( pExtBar == pExtBarSearch )
			return nPos;
	} // for( int nPos = 0; nPos < nCount; nPos++ )
	return -1;
}

/////////////////////////////////////////////////////////////////////////////
// CExtCustomizeCommandListBox window

IMPLEMENT_DYNCREATE(CExtCustomizeCommandListBox,CListBox);

CExtCustomizeCommandListBox::CExtCustomizeCommandListBox()
	: m_nIconAreaWidth( __EXT_MENU_ICON_CX )
	, m_bTrackingDragStart( false )
	, m_bAllowDragStart( true )
{
}

CExtCustomizeCommandListBox::~CExtCustomizeCommandListBox()
{
}

BEGIN_MESSAGE_MAP(CExtCustomizeCommandListBox,CListBox)
    //{{AFX_MSG_MAP(CExtCustomizeCommandListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CANCELMODE()
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CExtCustomizeCommandListBox::AddCommand(
	CExtCustomizeCmdTreeNode * pNode
	)
{
	ASSERT( !m_strProfileName.IsEmpty() );
	ASSERT_VALID( pNode );
UINT nCmdID = pNode->GetCmdID( false );
	ASSERT( CExtCmdManager::IsCommand(nCmdID) );
CExtCmdItem * pCmdItem =
	g_CmdManager->CmdGetPtr(
		m_strProfileName,
		nCmdID
		);
	ASSERT( pCmdItem != NULL );
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
	if( !pSite->OnCanDragCustomizeCommand(pCmdItem) )
		return;
CExtSafeString _strCommandScriptName =
		pCmdItem->OnGetCustomizeScriptName();
	_strCommandScriptName.Replace( _T("&"), _T("") );
	m_arrCmds.Add( pNode );
	AddString( _strCommandScriptName );
	ASSERT( m_arrCmds.GetSize() == CListBox::GetCount() );
}

void CExtCustomizeCommandListBox::ResetContent()
{
	CListBox::ResetContent();
	m_arrCmds.RemoveAll();
	m_nIconAreaWidth = __EXT_MENU_ICON_CX;
}

void CExtCustomizeCommandListBox::PreSubclassWindow() 
{
	CListBox::PreSubclassWindow();
	m_strProfileName =
		g_CmdManager->ProfileNameFromWnd( m_hWnd );
	ASSERT( !m_strProfileName.IsEmpty() );
}

void CExtCustomizeCommandListBox::OnCancelMode() 
{
	CListBox::OnCancelMode();
	if( !m_bTrackingDragStart )
		return;
	m_bTrackingDragStart = false;
	if( GetCapture() == this )
		ReleaseCapture();
}

void CExtCustomizeCommandListBox::OnCaptureChanged(CWnd *pWnd) 
{
	CListBox::OnCaptureChanged(pWnd);
	if( GetCapture() == this )
		return;
	if( !m_bTrackingDragStart )
		return;
	SendMessage( WM_CANCELMODE );
}

void CExtCustomizeCommandListBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CListBox::OnLButtonDown(nFlags, point);
	if( !m_bAllowDragStart )
		return;
BOOL bOutside = TRUE;
int nSel = (int)CListBox::ItemFromPoint( point, bOutside );
	if(		bOutside
		||	nSel < 0
		||	nSel >= m_arrCmds.GetSize()
		)
		return;
	m_bTrackingDragStart = true;
	m_ptTrackingDragStart = point;
	SetCapture();
}

void CExtCustomizeCommandListBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CListBox::OnLButtonUp(nFlags, point);
	if( CListBox::GetCount() != 0 )
		GetParent()->SendMessage(
			WM_COMMAND,
			(WPARAM)MAKELPARAM( GetDlgCtrlID(), LBN_SELCHANGE ),
			(LPARAM)m_hWnd
			);
	if( !m_bTrackingDragStart )
		return;
	SendMessage( WM_CANCELMODE );
}

void CExtCustomizeCommandListBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	CListBox::OnMouseMove(nFlags, point);
	if( !m_bTrackingDragStart )
		return;
CSize sizeDistance(
		abs( m_ptTrackingDragStart.x - point.x ),
		abs( m_ptTrackingDragStart.y - point.y )
		);
	if( sizeDistance.cx < 3 && sizeDistance.cy < 3 )
		return;
	SendMessage( WM_CANCELMODE );

	ASSERT( m_arrCmds.GetSize() >= CListBox::GetCount() );
int nSel = GetCurSel();
	if( nSel < 0 || nSel >= m_arrCmds.GetSize() )
	{
		ASSERT( FALSE );
		return;
	}
CExtCustomizeCmdTreeNode * pNode =
		(CExtCustomizeCmdTreeNode *)
			m_arrCmds[ nSel ];
	ASSERT_VALID( pNode );
	ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNode );
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
CRect rcItem;
	CListBox::GetItemRect( nSel, &rcItem );
	ClientToScreen( &rcItem );
	pSite->DoDragCmdNode( this, NULL, pNode, rcItem );
}

// CExtCustomizeSite::ICustomizeDropSource
bool CExtCustomizeCommandListBox::OnCustomizeSourceAllowMoveDel()
{
	// only copy effect allowed
	// when the command dragged from this listbox
	return false;
}

void CExtCustomizeCommandListBox::MeasureItem(LPMEASUREITEMSTRUCT pMIS) 
{
	ASSERT( pMIS != NULL );
	ASSERT( m_arrCmds.GetSize() >= CListBox::GetCount() );
	if( ((int)pMIS->itemID) < 0 )
		return;
	ASSERT( ((int)pMIS->itemID) < m_arrCmds.GetSize() );

CExtCustomizeCmdTreeNode * pNode =
		(CExtCustomizeCmdTreeNode *)
			m_arrCmds[ (int)pMIS->itemID ];
	ASSERT_VALID( pNode );
	ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNode );

CString sMeasureText;
	CListBox::GetText( ((int)pMIS->itemID), sMeasureText );

CWindowDC dc(NULL);
CFont * pOldFont =
		dc.SelectObject( &(g_PaintManager->m_FontNormal) );
CRect rect( 0, 0, 0, 0 );
	::DrawText(
		dc.GetSafeHdc(),
		sMeasureText,
		sMeasureText.GetLength(),
		&rect,
		DT_CALCRECT|DT_SINGLELINE|DT_LEFT
		);
CSize _sizeItem(
		rect.Width(),
		rect.Height() + __EXT_MENU_IMG_RECT_OUT_GAP*2
		);
	dc.SelectObject( pOldFont );

CSize sizeIcon( 0, 0 );
int nIconHeightMetric = 0;

CExtCmdIcon * pIcon =
		g_CmdManager->CmdGetIconPtr(
			m_strProfileName,
			pNode->GetCmdID( false )
			);
	if( pIcon != NULL )
	{
		sizeIcon = pIcon->GetSize();
		if( sizeIcon.cx < __EXT_MENU_ICON_CX )
			sizeIcon.cx = __EXT_MENU_ICON_CX;
		if( sizeIcon.cy < __EXT_MENU_ICON_CY )
			sizeIcon.cy = __EXT_MENU_ICON_CY;
		if(	sizeIcon.cx <= 18 && sizeIcon.cy < sizeIcon.cx )
			sizeIcon.cy = sizeIcon.cx;
		m_nIconAreaWidth =
			max( m_nIconAreaWidth, sizeIcon.cx );
		nIconHeightMetric = sizeIcon.cy + __EXT_MENU_IMG_RECT_OUT_GAP*2;
	} // if( pIcon != NULL )

	_sizeItem.cy = max( _sizeItem.cy, nIconHeightMetric );
	if( _sizeItem.cy < __EXT_MENU_MIN_HEIGHT )
		_sizeItem.cy = __EXT_MENU_MIN_HEIGHT;

	m_nIconAreaWidth = max( sizeIcon.cx, _sizeItem.cy );
	_sizeItem.cx +=
		__EXT_MENU_GAP * 7;

	_sizeItem.cx +=
		m_nIconAreaWidth
		+ __EXT_MENU_POPUP_ARROW_AREA_DX;
	_sizeItem.cy += __EXT_MENU_VERT_DISTANCE_TOOL;

	pMIS->itemWidth = _sizeItem.cx;
	pMIS->itemHeight = _sizeItem.cy;
}

void CExtCustomizeCommandListBox::DrawItem(LPDRAWITEMSTRUCT pDIS) 
{
	ASSERT( pDIS != NULL );
	ASSERT( m_arrCmds.GetSize() >= CListBox::GetCount() );
	ASSERT( pDIS->hDC != NULL );
	if( ((int)pDIS->itemID) < 0 )
		return;
	ASSERT( ((int)pDIS->itemID) < m_arrCmds.GetSize() );

CExtCustomizeCmdTreeNode * pNode =
		(CExtCustomizeCmdTreeNode *)
			m_arrCmds[ (int)pDIS->itemID ];
	ASSERT_VALID( pNode );
	ASSERT_KINDOF( CExtCustomizeCmdTreeNode, pNode );

CRect rcItem( pDIS->rcItem );

CString sDrawText;
	CListBox::GetText( ((int)pDIS->itemID), sDrawText );

CExtCmdIcon * pIcon =
		g_CmdManager->CmdGetIconPtr(
			m_strProfileName,
			pNode->GetCmdID( false )
			);

bool bSelected =
		( (pDIS->itemState & ODS_FOCUS) != 0 )
			? true : false;

bool bPopup = pNode->IsPopupNode();
CExtPaintManager::PAINTMENUITEMDATA _pmid(
		this,
		rcItem,
		m_nIconAreaWidth + 4,
		sDrawText.IsEmpty() ? _T("") : (LPCTSTR)sDrawText,
		_T(""),
		pIcon,
		bPopup,
		bSelected,
		false,
		false,
		false,
		true,
		false,
		false,
		false,
		pNode->GetCmdID( false )
		);
	_pmid.m_bCustomizeListMode = true;
	_pmid.m_bSeparatedDropDown =
		(pNode->GetFlags() & __ECTN_TBB_SEPARATED_DROPDOWN)
			? true : false;

CDC dc;
	dc.Attach( pDIS->hDC );

#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	if( (pNode->GetFlags() & __ECTN_TBB_TEXT_FIELD) != 0 )
	{
		_pmid.m_rcInPlaceWnd = rcItem;
		_pmid.m_rcInPlaceWnd.DeflateRect(
			1,
			2,
			1,
			2 + __EXT_MENU_VERT_DISTANCE_NORM
			);
		CFont * pOldFont =
			dc.SelectObject( &(g_PaintManager->m_FontNormal) );
		CRect rect( 0, 0, 0, 0 );
		::DrawText(
			dc.GetSafeHdc(),
			sDrawText,
			sDrawText.GetLength(),
			&rect,
			DT_CALCRECT|DT_SINGLELINE|DT_LEFT
			);
		dc.SelectObject( pOldFont );
		int nInplaceEditWidth =
			_pmid.m_rcInPlaceWnd.Width()
			- m_nIconAreaWidth
			- rect.Width()
			- __EXT_MENU_GAP * 5
			;
		if( nInplaceEditWidth < 30 )
			nInplaceEditWidth = 30;
		_pmid.m_rcInPlaceWnd.left =
			_pmid.m_rcInPlaceWnd.right
			- nInplaceEditWidth
			;
		if( bPopup )
			_pmid.m_rcInPlaceWnd.DeflateRect(
				0,
				0,
				::GetSystemMetrics(SM_CXHTHUMB),
				0
				);
	} // if( (pNode->GetFlags() & __ECTN_TBB_TEXT_FIELD) != 0 )
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)

	g_PaintManager->PaintMenuItem( dc, _pmid );
	dc.Detach();
}

/////////////////////////////////////////////////////////////////////////////
// CExtCustomizeForm window

IMPLEMENT_DYNCREATE(CExtCustomizeForm,CExtResPS);

CExtCustomizeForm::CExtCustomizeForm()
	: CExtResPS( IDD_EXT_CUSTUMIZE_FORM_CAPT )
{
	m_psh.dwFlags = PSH_PROPSHEETPAGE|PSH_MODELESS|PSH_NOAPPLYNOW;
}

CExtCustomizeForm::~CExtCustomizeForm()
{
	for(	POSITION pos = m_listHelperPages.GetHeadPosition();
			pos != NULL;
		)
	{ // walk all page pointers
		CPropertyPage * pPage = (CPropertyPage *)
			m_listHelperPages.GetNext( pos );
		ASSERT( pPage != NULL );
		delete pPage;
	} // walk all page pointers
	m_listHelperPages.RemoveAll();
}

BEGIN_MESSAGE_MAP(CExtCustomizeForm,CExtResPS)
    //{{AFX_MSG_MAP(CExtCustomizeForm)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(
		CExtCustomizeSite::g_nMsgSyncContent,
		_OnSyncContent
		)
END_MESSAGE_MAP()

LRESULT CExtCustomizeForm::_OnSyncContent(WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID( this );
	for(	HWND hWnd = ::GetWindow( m_hWnd, GW_CHILD );
			hWnd != NULL;
			hWnd = ::GetWindow( hWnd, GW_HWNDNEXT )
			)
		::SendMessage(
			hWnd,
			CExtCustomizeSite::g_nMsgSyncContent,
			wParam,
			lParam
			);
	return (!0L);
}

BOOL CExtCustomizeForm::OnInitDialog() 
{
	CExtResPS::OnInitDialog();

CWnd * pBtn = GetDlgItem( IDCANCEL );
	if( pBtn->GetSafeHwnd() != NULL )
	{
		pBtn->ModifyStyle( BS_DEFPUSHBUTTON, 0 );
		pBtn->ShowWindow( SW_HIDE );
	}
	
	pBtn = GetDlgItem( IDCANCEL );
	ASSERT( pBtn->GetSafeHwnd() != NULL );
	pBtn->ShowWindow( SW_SHOW );
	pBtn->ModifyStyle( 0, BS_DEFPUSHBUTTON );
	pBtn->EnableWindow();

CRect rcClose, rcSheet;
	pBtn->GetWindowRect( &rcClose );
	GetWindowRect( &rcSheet );
	rcSheet.bottom += ::MulDiv( rcClose.Height(), 3, 2 );
	SetWindowPos(
		NULL,
		0, 0, rcSheet.Width(), rcSheet.Height(),
		SWP_NOMOVE|SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOACTIVATE
		);
	rcClose.OffsetRect(
		0,
		rcSheet.bottom - rcClose.bottom
			- ::MulDiv( rcClose.Height(), 1, 3 )
		);
	ScreenToClient( &rcClose );
	pBtn->SetWindowPos(
		NULL,
		rcClose.left, rcClose.top, rcClose.Width(), rcClose.Height(),
		SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOACTIVATE
		);

CExtSafeString strCloseText;
	{ // BLOCK: Load "Close" string
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		CExtLocalResourceHelper _LRH;
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		if( ! g_ResourceManager->LoadString( strCloseText, IDS_EXT_BTN_CLOSE ) )
		{
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
			ASSERT( FALSE );
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
			strCloseText = _T("Close");
		}
	} // BLOCK: Load "Close" string
	pBtn->SetWindowText( strCloseText );

	return TRUE;
}

LRESULT CExtCustomizeForm::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(		message == WM_COMMAND && wParam == IDCANCEL
		||	message == WM_SYSCOMMAND && wParam == SC_CLOSE
		)
	{
		if( !OnQueryCloseForm() )
			return 0L;
		CExtCustomizeSite * pSite =
			CExtCustomizeSite::GetCustomizeSite( m_hWnd );
		if( pSite != NULL )
			pSite->CancelCustomization();
		PostMessage( WM_CLOSE );
		return 0L;
	} // if( message == WM_COMMAND && wParam == IDCANCEL )

	return CExtResPS::WindowProc(message, wParam, lParam);
}

bool CExtCustomizeForm::OnQueryCloseForm()
{
	return true;
}

void CExtCustomizeForm::PostNcDestroy() 
{
	CExtResPS::PostNcDestroy();
	delete this;
}

void CExtCustomizeForm::AddPage( CPropertyPage * pPage )
{
	CExtResPS::AddPage( pPage );
	m_listHelperPages.AddTail( pPage );
}

void CExtCustomizeForm::OnBarStateChanged(
	CExtControlBar * pExtBar
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pExtBar );
	pExtBar;
	for(	POSITION pos = m_listHelperPages.GetHeadPosition();
			pos != NULL;
		)
	{ // walk all page pointers
		CPropertyPage * pPageTemp = (CPropertyPage *)
			m_listHelperPages.GetNext( pos );
		ASSERT_VALID( pPageTemp );
		CExtCustomizePageToolbars * pPageToolbars =
			DYNAMIC_DOWNCAST( CExtCustomizePageToolbars, pPageTemp );
		if( pPageToolbars != NULL )
		{
//			int nPos = pPageToolbars->m_wndToolbarsList.FindBarPos( pExtBar );
//			if( nPos < 0 )
//				return;
//			pPageToolbars->m_wndToolbarsList.UpdateCheckFromBar( nPos );
			pPageToolbars->m_wndToolbarsList.DelayRescanBars();
			return;
		} // if( pPageToolbars != NULL )
	} // walk all page pointers
}

/////////////////////////////////////////////////////////////////////////////
// CExtCustomizeDlgToolbarName dialog

class CExtCustomizeDlgToolbarName : public CExtResDlg
{
// Construction
public:
	CExtCustomizeDlgToolbarName(
		UINT nIdDlgResource,
		CWnd* pParent = NULL
		);

// Dialog Data
	//{{AFX_DATA(CExtCustomizeDlgToolbarName)
	CString	m_strToolbarName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtCustomizeDlgToolbarName)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExtCustomizeDlgToolbarName)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CExtCustomizeDlgToolbarName::CExtCustomizeDlgToolbarName(
	UINT nIdDlgResource,
	CWnd* pParent // = NULL
	)
	: CExtResDlg(nIdDlgResource, pParent)
{
	//{{AFX_DATA_INIT(CExtCustomizeDlgToolbarName)
	m_strToolbarName = _T("");
	//}}AFX_DATA_INIT
}


void CExtCustomizeDlgToolbarName::DoDataExchange(CDataExchange* pDX)
{
	CExtResDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtCustomizeDlgToolbarName)
	DDX_Text(pDX, IDC_EXT_EDIT_TOOLBAR_NAME, m_strToolbarName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExtCustomizeDlgToolbarName, CExtResDlg)
	//{{AFX_MSG_MAP(CExtCustomizeDlgToolbarName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtCustomizeDlgToolbarName message handlers

BOOL CExtCustomizeDlgToolbarName::OnInitDialog() 
{
	CExtResDlg::OnInitDialog();
	UpdateData( FALSE );
	return TRUE;
}

void CExtCustomizeDlgToolbarName::OnOK() 
{
	UpdateData( TRUE );
	m_strToolbarName.TrimLeft( _T(" ") );
	m_strToolbarName.TrimRight( _T(" ") );
	if( m_strToolbarName.IsEmpty() )
	{
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		CExtLocalResourceHelper _LRH;
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		::AfxMessageBox( IDS_EXT_EMPTY_TOOLBAR_NAME );
		GetDlgItem(IDC_EXT_EDIT_TOOLBAR_NAME)->SetFocus();
		return;
	} // if( m_strToolbarName.IsEmpty() )
	
	CExtResDlg::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CExtCustomizePageToolbars window

IMPLEMENT_DYNCREATE(CExtCustomizePageToolbars,CExtResPP);

CExtCustomizePageToolbars::CExtCustomizePageToolbars()
	: CExtResPP( IDD_EXT_CUST_PAGE_TOOLBARS, IDS_EXT_CUST_PAGE_TOOLBARS )
	, m_bUserBar( false )
	, m_pToolBar( NULL )
{
}

CExtCustomizePageToolbars::~CExtCustomizePageToolbars()
{
}

void CExtCustomizePageToolbars::DoDataExchange(CDataExchange* pDX)
{
	CExtResPP::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtCustomizePageToolbars)
	DDX_Control(pDX, IDC_EXT_BUTTON_RESET_TOOLBAR, m_btnTbReset);
	DDX_Control(pDX, IDC_EXT_BUTTON_RENAME_TOOLBAR, m_btnTbRename);
	DDX_Control(pDX, IDC_EXT_BUTTON_DELETE_TOOLBAR, m_btnTbDelete);
	DDX_Control(pDX, IDC_EXT_BUTTON_CREATE_TOOLBAR, m_btnTbCreate);
	DDX_Control(pDX, IDC_EXT_COMBO_SHOW_MENU, m_wndComboMenu);
	DDX_Control(pDX, IDC_EXT_LIST_TOOLBARS, m_wndToolbarsList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CExtCustomizePageToolbars,CExtResPP)
    //{{AFX_MSG_MAP(CExtCustomizePageToolbars)
	//}}AFX_MSG_MAP
	ON_LBN_SELCHANGE(IDC_EXT_LIST_TOOLBARS, OnSelchangeToolbarList)
	ON_CBN_SELENDOK(IDC_EXT_COMBO_SHOW_MENU, OnSelendokComboShowMenu)
	ON_BN_CLICKED(IDC_EXT_BUTTON_CREATE_TOOLBAR, OnClickedToolbarCreate)
	ON_BN_CLICKED(IDC_EXT_BUTTON_RENAME_TOOLBAR, OnClickedToolbarRename)
	ON_BN_CLICKED(IDC_EXT_BUTTON_DELETE_TOOLBAR, OnClickedToolbarDelete)
	ON_BN_CLICKED(IDC_EXT_BUTTON_RESET_TOOLBAR, OnClickedToolbarReset)
	ON_REGISTERED_MESSAGE(
		CExtCustomizeSite::g_nMsgSyncContent,
		_OnSyncContent
		)
END_MESSAGE_MAP()

LRESULT CExtCustomizePageToolbars::_OnSyncContent(WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID( this );

CString strMenuSel;
	if( m_wndComboMenu.GetCount() > 0 )
	{
		INT nCurSel =  m_wndComboMenu.GetCurSel();
		if( nCurSel >= 0 )
			m_wndComboMenu.GetLBText( nCurSel, strMenuSel );
		m_wndComboMenu.ResetContent();
	}
	m_wndToolbarsList.ResetContent();
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
INT nMenuCount = pSite->MenuInfoGetCount();
	if( nMenuCount >= 1 )
	{
		for( INT i = 0; i < nMenuCount; i++ )
		{
			CExtCustomizeSite::CCmdMenuInfo * pCmdMenuInfo =
				pSite->MenuInfoGetAt( i );
			ASSERT( pCmdMenuInfo != NULL );
			ASSERT( pCmdMenuInfo->IsInitialized() );
			ASSERT( pCmdMenuInfo->GetName() != NULL );
			m_wndComboMenu.AddString( pCmdMenuInfo->GetName() );
		} // for( INT i = 0; i < nMenuCount; i++ )
		int nCurSel = 0;
		if( ! strMenuSel.IsEmpty() )
			nCurSel =
			m_wndComboMenu.FindStringExact(
				-1,
				strMenuSel
				);
		if( nCurSel < 0 )
		{
			nCurSel = 0;
			CExtCustomizeSite::CCmdMenuInfo * pCmdMenuInfo =
				pSite->MenuInfoActiveGet();
			if( pCmdMenuInfo != NULL )
			{
				nCurSel = pSite->MenuInfoFindIndex( pCmdMenuInfo );
				ASSERT( 0 <= nCurSel && nCurSel < nMenuCount );
			} // if( pCmdMenuInfo != NULL )
		} // if( nCurSel < 0 )
		m_wndComboMenu.SetCurSel( nCurSel );
	} // if( nMenuCount >= 1 )
	
	if( nMenuCount <= 1 )
	{
		GetDlgItem( IDC_EXT_STATIC_SHOW_MENU )->ShowWindow( SW_HIDE );
		m_wndComboMenu.ShowWindow( SW_HIDE );
	} // if( nMenuCount <= 1 )
	else
		m_wndComboMenu.ShowWindow( SW_SHOW );

CPtrList & listBarsForListBox = pSite->GetBarsForListBox();
	for(	POSITION pos = listBarsForListBox.GetHeadPosition();
			pos != NULL;
		)
	{
		CExtControlBar * pExtBar = (CExtControlBar*)
			listBarsForListBox.GetNext( pos );
		ASSERT_VALID( pExtBar );
		ASSERT_KINDOF( CExtControlBar, pExtBar );
		ASSERT( pExtBar->m_hWnd != NULL && ::IsWindow(pExtBar->m_hWnd) );
		pExtBar->OnCustomizeListBoxUpdate( &m_wndToolbarsList );
	}
	if( m_wndToolbarsList.GetCount() > 0 )
		m_wndToolbarsList.SetCurSel( 0 );

	for(	HWND hWnd = ::GetWindow( m_hWnd, GW_CHILD );
			hWnd != NULL;
			hWnd = ::GetWindow( hWnd, GW_HWNDNEXT )
			)
		::SendMessage(
			hWnd,
			CExtCustomizeSite::g_nMsgSyncContent,
			wParam,
			lParam
			);

	OnSelchangeToolbarList();
			
	return (!0L);
}

BOOL CExtCustomizePageToolbars::OnInitDialog() 
{
	__PROF_UIS_MANAGE_STATE;

	CExtResPP::OnInitDialog();
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
	if( (pSite->GetCustomizeFlags() & __ECSF_USER_BARS) == 0 )
	{
		m_btnTbRename.ShowWindow( SW_HIDE );
		m_btnTbDelete.ShowWindow( SW_HIDE );
		m_btnTbCreate.ShowWindow( SW_HIDE );
		CRect rcBtn;
		m_btnTbCreate.GetWindowRect( &rcBtn );
		ScreenToClient( &rcBtn );
		m_btnTbReset.MoveWindow( &rcBtn );
	}
	if( (pSite->GetCustomizeFlags() & __ECSF_COMMANDS) == 0 )
		m_btnTbReset.ShowWindow( SW_HIDE );
	SendMessage( CExtCustomizeSite::g_nMsgSyncContent );
	return TRUE;
}

void CExtCustomizePageToolbars::OnSelendokComboShowMenu() 
{
	ASSERT_VALID( this );
	if( !m_wndComboMenu.IsWindowVisible() )
		return;
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
INT nMenuCount = pSite->MenuInfoGetCount();
	if( nMenuCount == 0 )
		return;
int nCurSel = m_wndComboMenu.GetCurSel();
	if( nCurSel < 0 )
		return;
	ASSERT( nCurSel < nMenuCount );
CExtCustomizeSite::CCmdMenuInfo * pCmdMenuInfoActive =
		pSite->MenuInfoActiveGet();
CExtCustomizeSite::CCmdMenuInfo * pCmdMenuInfoSel =
		pSite->MenuInfoGetAt( nCurSel );
	if( ((LPVOID)pCmdMenuInfoActive) != ((LPVOID)pCmdMenuInfoSel) )
		pSite->MenuInfoActiveSet( pCmdMenuInfoSel );
}

void CExtCustomizePageToolbars::OnSelchangeToolbarList() 
{
BOOL bEnabledCreate = TRUE, bEnabledRename = FALSE,
	bEnabledDelete = FALSE, bEnabledReset = FALSE;
	m_pToolBar = NULL;
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
int nCurSel = m_wndToolbarsList.GetCurSel();
	m_strSelBarName = _T("");
	if( nCurSel >= 0 && nCurSel < m_wndToolbarsList.GetCount() )
	{
		CExtControlBar * pExtBar = (CExtControlBar *)
			m_wndToolbarsList.GetItemData( nCurSel );
		if( pExtBar != NULL )
		{
			ASSERT_VALID( pExtBar );
			m_pToolBar =
				DYNAMIC_DOWNCAST( CExtToolControlBar, pExtBar );
			if(		m_pToolBar != NULL
				&&	pSite->BarIsRegistered( m_pToolBar )
				)
			{
				CExtCustomizeCmdTreeNode * pNodeC = NULL, * pNodeI = NULL;
				if(	pSite->GetToolbarCustomizeInfo(
						m_pToolBar,
						&pNodeC,
						&pNodeI,
						&m_strSelBarName,
						&m_bUserBar
						)
					)
				{
					if( m_bUserBar )
						bEnabledDelete = bEnabledRename = TRUE;
					else
						bEnabledReset = TRUE;
				} // if( pSite->GetToolbarCustomizeInfo( ...
				else
				{
					//ASSERT( FALSE );
					m_strSelBarName = _T("");
					m_bUserBar = false;
					m_pToolBar = NULL;
				} // else from if( pSite->GetToolbarCustomizeInfo( ...
			} // if( pToolBar != NULL ...
			else
				m_pToolBar = NULL;
		} // if( pExtBar != NULL )
	} // if( nCurSel >= 0 && nCurSel < m_wndToolbarsList.GetCount() )
	m_btnTbCreate.EnableWindow( bEnabledCreate );
	m_btnTbRename.EnableWindow( bEnabledRename );
	m_btnTbDelete.EnableWindow( bEnabledDelete );
	m_btnTbReset.EnableWindow( bEnabledReset );
}

void CExtCustomizePageToolbars::OnClickedToolbarCreate()
{
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
	ASSERT( (pSite->GetCustomizeFlags() & __ECSF_USER_BARS) != 0 );

CExtCustomizeDlgToolbarName dlg( IDD_EXT_DIALOG_TOOLBAR_CREATE, this );
	if( dlg.DoModal() != IDOK )
		return;
	ASSERT( !dlg.m_strToolbarName.IsEmpty() );

CExtToolControlBar * pBar =
		pSite->OnUserBarCreate(
			(LPCTSTR)dlg.m_strToolbarName
			);
	if( pBar == NULL )
	{
		ASSERT( FALSE );
		return;
	}
	pBar->OnCustomizeListBoxUpdate( &m_wndToolbarsList );
	m_wndToolbarsList.SyncBars();
int nCount = m_wndToolbarsList.GetCount();
	if( nCount > 0 )
		m_wndToolbarsList.SetCurSel( nCount - 1 );
	OnSelchangeToolbarList();
}

void CExtCustomizePageToolbars::OnClickedToolbarRename()
{
	if( m_pToolBar == NULL )
		return;
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
	ASSERT( (pSite->GetCustomizeFlags() & __ECSF_USER_BARS) != 0 );
CExtCmdItem * pCmdItem =
		g_CmdManager->CmdGetPtr(
			g_CmdManager->ProfileNameFromWnd( m_hWnd ),
			(UINT)m_pToolBar->GetDlgCtrlID()
			);
	ASSERT( pCmdItem != NULL );
	if( pCmdItem == NULL )
		return;

CExtCustomizeDlgToolbarName dlg( IDD_EXT_DIALOG_TOOLBAR_RENAME, this );
	if( pCmdItem->m_sMenuText.IsEmpty() )
	{
		int nTextLen = m_pToolBar->GetWindowTextLength();
		if( nTextLen > 0 )
		{
			m_pToolBar->GetWindowText( dlg.m_strToolbarName.GetBuffer( nTextLen+2 ), nTextLen+1 );
			dlg.m_strToolbarName.ReleaseBuffer();
		} // if( nTextLen > 0 )
	}
	else
		dlg.m_strToolbarName = pCmdItem->m_sMenuText;
	if( dlg.DoModal() != IDOK )
		return;
	ASSERT( !dlg.m_strToolbarName.IsEmpty() );

int nCurSel = m_wndToolbarsList.GetCurSel();
	ASSERT( 0 <= nCurSel && nCurSel < m_wndToolbarsList.GetCount() );
	ASSERT( ((LPVOID)m_wndToolbarsList.GetItemData(nCurSel)) == ((LPVOID)m_pToolBar) );
	m_wndToolbarsList.SetRedraw( FALSE );
	m_wndToolbarsList.DeleteString( nCurSel );
CExtSafeString strLbText( dlg.m_strToolbarName );
	strLbText.Replace( _T("&"), _T("") );
	m_wndToolbarsList.InsertString( nCurSel, strLbText );
	m_wndToolbarsList.SetItemData( nCurSel, (DWORD)(m_pToolBar) );
	m_wndToolbarsList.SetCurSel( nCurSel );
	m_wndToolbarsList.SetRedraw( TRUE );
	m_wndToolbarsList.Invalidate();

CExtControlBar * pExtBarResetPos = m_pToolBar;
	pSite->OnUserBarRename(
		m_pToolBar,
		(LPCTSTR)dlg.m_strToolbarName
		);
int nPos = m_wndToolbarsList.FindBarPos( pExtBarResetPos );
	if( nPos >= 0 )
		m_wndToolbarsList.SetCurSel( nPos );
	OnSelchangeToolbarList();
	m_wndToolbarsList.SendMessage( WM_USER+100 );
}

void CExtCustomizePageToolbars::OnClickedToolbarDelete()
{
	if( m_pToolBar == NULL )
		return;
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
	ASSERT( (pSite->GetCustomizeFlags() & __ECSF_USER_BARS) != 0 );

CString strQst, strFmt, strBarName;
	m_pToolBar->GetWindowText( strBarName );
	{ // BLOCK
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		CExtLocalResourceHelper _LRH;
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		if( ! g_ResourceManager->LoadString( strFmt, IDS_EXT_DEL_TOOLBAR_FMT ) )
		{
			ASSERT( FALSE );
			strFmt = _T("Are you sure you want to delete the '%s' toolbar?");
		}
	} // BLOCK
	strQst.Format( (LPCTSTR)strFmt, strBarName );
	if( ::AfxMessageBox( strQst, MB_YESNO ) != IDYES )
		return;

int nCurSel = m_wndToolbarsList.GetCurSel();
	ASSERT( 0 <= nCurSel && nCurSel < m_wndToolbarsList.GetCount() );
	ASSERT( ((LPVOID)m_wndToolbarsList.GetItemData(nCurSel)) == ((LPVOID)m_pToolBar) );
	m_wndToolbarsList.DeleteString( nCurSel );
	
	pSite->OnUserBarDelete( m_pToolBar );
	m_pToolBar = NULL;

	if( m_wndToolbarsList.GetCount() > 0 )
		m_wndToolbarsList.SetCurSel( 0 );
	OnSelchangeToolbarList();
}

void CExtCustomizePageToolbars::OnClickedToolbarReset()
{
	if( m_pToolBar == NULL )
		return;
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
	if( !pSite->BarResetAsk(m_pToolBar) )
		return;
	VERIFY( pSite->BarReset( m_pToolBar ) );
}

/////////////////////////////////////////////////////////////////////////////
// CExtCustomizePageCommands window

IMPLEMENT_DYNCREATE(CExtCustomizePageCommands,CExtResPP);

CExtCustomizePageCommands::CExtCustomizePageCommands()
	: CExtResPP( IDD_EXT_CUST_PAGE_COMMANDS, IDS_EXT_CUST_PAGE_COMMANDS )
	, m_pItems( NULL )
{
}

CExtCustomizePageCommands::~CExtCustomizePageCommands()
{
}

void CExtCustomizePageCommands::DoDataExchange(CDataExchange* pDX)
{
	CExtResPP::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtCustomizePageCommands)
	DDX_Control(pDX, IDC_EXT_LIST_CMD_CATEGORIES, m_wndCategoryList);
	DDX_Control(pDX, IDC_EXT_LIST_COMMANDS, m_wndCommandList);
	DDX_Control(pDX, IDC_EXT_STATIC_CMD_DESCRIPTION, m_wndStaticCmdDescription);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CExtCustomizePageCommands,CExtResPP)
    //{{AFX_MSG_MAP(CExtCustomizePageCommands)
	ON_LBN_SELCHANGE(IDC_EXT_LIST_CMD_CATEGORIES, OnSelChangeCategoryList)
	ON_LBN_SELCHANGE(IDC_EXT_LIST_COMMANDS, OnSelChangeCommandList)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(
		CExtCustomizeSite::g_nMsgSyncContent,
		_OnSyncContent
		)
END_MESSAGE_MAP()

LRESULT CExtCustomizePageCommands::_OnSyncContent(WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID( this );

	m_wndCategoryList.ResetContent();
	m_wndCommandList.ResetContent();

	m_strProfileName =
		g_CmdManager->ProfileNameFromWnd( m_hWnd );
	ASSERT( !m_strProfileName.IsEmpty() );

CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
	pSite->CategoryEnum( this );
int nCategoryCount = m_wndCategoryList.GetCount();
	if( nCategoryCount > 0 )
	{
		m_wndCategoryList.SetCurSel( 0 );
		OnSelChangeCategoryList();
	}

	for(	HWND hWnd = ::GetWindow( m_hWnd, GW_CHILD );
			hWnd != NULL;
			hWnd = ::GetWindow( hWnd, GW_HWNDNEXT )
			)
		::SendMessage(
			hWnd,
			CExtCustomizeSite::g_nMsgSyncContent,
			wParam,
			lParam
			);
	return (!0L);
}

BOOL CExtCustomizePageCommands::OnInitDialog() 
{
	CExtResPP::OnInitDialog();
	SendMessage( CExtCustomizeSite::g_nMsgSyncContent );
	return TRUE;
}

void CExtCustomizePageCommands::OnSelChangeCategoryList() 
{
	m_wndStaticCmdDescription.SetWindowText( _T("") );
int nSel = m_wndCategoryList.GetCurSel();
	ASSERT( nSel >= 0 );
	m_pItems = (CExtCustomizeCmdTreeNode *)
		m_wndCategoryList.GetItemData( nSel );
	ASSERT_VALID( m_pItems );
	ASSERT( !m_strProfileName.IsEmpty() );
	m_wndCommandList.SetRedraw( FALSE );
	m_wndCommandList.ResetContent();
int nCmdCount = m_pItems->GetNodeCount();
	for( int nCmdItem = 0; nCmdItem < nCmdCount; nCmdItem++ )
	{
		CExtCustomizeCmdTreeNode * pNode =
			m_pItems->ElementAt( nCmdItem );
		ASSERT_VALID( pNode );
		m_wndCommandList.AddCommand( pNode );
	} // for( int nCmdItem = 0; nCmdItem < nCmdCount; nCmdItem++ )
	m_wndCommandList.SetRedraw( TRUE );
	m_wndCommandList.Invalidate();
	m_wndCommandList.UpdateWindow();
}

void CExtCustomizePageCommands::OnSelChangeCommandList() 
{
int nSel = m_wndCommandList.GetCurSel();
	if( nSel < 0 )
	{
		m_wndStaticCmdDescription.SetWindowText( _T("") );
		return;
	} // if( nSel < 0 )
	ASSERT_VALID( m_pItems );
	ASSERT( !m_strProfileName.IsEmpty() );
int nCmdCount = m_pItems->GetNodeCount();
	ASSERT( nSel < nCmdCount );
CExtCustomizeCmdTreeNode * pNode = m_pItems->ElementAt(nSel);
	ASSERT_VALID( pNode );
CExtCmdItem * pCmdItem =
		g_CmdManager->CmdGetPtr(
			m_strProfileName,
			pNode->GetCmdID( false )
			);
	if( pCmdItem == NULL )
	{
		m_wndStaticCmdDescription.SetWindowText( _T("") );
		return;
	} // if( pCmdItem == NULL )
CExtSafeString strCommandDescription =
		pCmdItem->OnGetCustomizeCommandDescription();
	m_wndStaticCmdDescription.SetWindowText(
		strCommandDescription
		);
}

// CExtCustomizeSite::ICustomizeCategoryEnumSite::OnCustomizeCategoryEnum
bool CExtCustomizePageCommands::OnCustomizeCategoryEnum(
	int nPos,
	CExtCustomizeCmdTreeNode * pRoot,
	bool bVisibleInKeyboardPage
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pRoot );
	bVisibleInKeyboardPage;
__EXT_MFC_SAFE_LPCTSTR strCategoryName = pRoot->GetTextUser();
	ASSERT( strCategoryName != NULL && _tcslen(strCategoryName) > 0 );
	m_wndCategoryList.InsertString(
		nPos,
		strCategoryName
		);
	m_wndCategoryList.SetItemData(
		nPos,
		(DWORD)pRoot
		);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CExtCustomizePageKeyboard window

IMPLEMENT_DYNCREATE(CExtCustomizeHotKeyWnd,CEdit);

CExtCustomizeHotKeyWnd::CExtCustomizeHotKeyWnd()
	: m_bAllowTabKeyInput( false )
	, m_nSimpleKeyModifiers( (BYTE)0 )
{
	::memset( &m_dataAccel, 0, sizeof(ACCEL) );
}

CExtCustomizeHotKeyWnd::~CExtCustomizeHotKeyWnd()
{
}

BEGIN_MESSAGE_MAP(CExtCustomizeHotKeyWnd,CEdit)
    //{{AFX_MSG_MAP(CExtCustomizeHotKeyWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CExtSafeString CExtCustomizeHotKeyWnd::stat_GetSingleKeyName(
	HKL hKeyboardLayout,
	WORD nKey,
	__EXT_MFC_SAFE_LPCTSTR strAppendIfNonEmpty, // = NULL
	bool bMakeOnlyFirstLettersUpper // = true
	)
{
UINT nScan = ::MapVirtualKeyEx( (UINT)nKey, 0, hKeyboardLayout );
	nScan &= 0x0FFFF;
	switch( nKey )
	{
	case VK_INSERT:
	case VK_DELETE:
	case VK_PRIOR:
	case VK_NEXT:
	case VK_END:
	case VK_HOME:
	case VK_LEFT:
	case VK_UP:
	case VK_RIGHT:
	case VK_DOWN:
	case VK_SELECT:
	case VK_PRINT:
	case VK_EXECUTE:
	case VK_SNAPSHOT:
	case VK_HELP:
		nScan |= 0x0300;
		break;
	} // switch( nKey )
LPARAM lParam = MAKELPARAM( 1, nScan );
const int nMaxKeyName = 256;
CExtSafeString strKeyText;
LPTSTR pBuffer = strKeyText.GetBuffer(nMaxKeyName);
	ASSERT( pBuffer != NULL );
	::memset( pBuffer, 0, nMaxKeyName*sizeof(TCHAR) );
	::GetKeyNameText( lParam, pBuffer, nMaxKeyName );
	strKeyText.ReleaseBuffer();
	if( strKeyText.IsEmpty())
		return strKeyText;
	if( ! bMakeOnlyFirstLettersUpper )
	{
		if( strAppendIfNonEmpty != NULL )
			strKeyText += strAppendIfNonEmpty;
		return strKeyText;
	}
	strKeyText.MakeLower();
bool bPrevSpace = true;
	for( int i = 0; i < strKeyText.GetLength(); i++ )
	{
		TCHAR chr = strKeyText[i];
		if( _istspace(chr) )
		{
			bPrevSpace = true;
			continue;
		} // if( _istspace(chr) )
		if( _istalpha(chr) && bPrevSpace )
		{
			chr = (TCHAR)_totupper( chr );
			strKeyText.SetAt( i, chr );
		} // if( _istalpha(chr) && bPrevSpace )
		bPrevSpace = false;
	} // for( int i = 0; i < strKeyText.GetLength(); i++ )
	if( strAppendIfNonEmpty != NULL )
		strKeyText += strAppendIfNonEmpty;
	return strKeyText;
}

CExtSafeString CExtCustomizeHotKeyWnd::stat_FormatAccelText(
	const ACCEL & _dataAccel
	)
{
HKL hKeyboardLayout =
	::GetKeyboardLayout(
		( ::AfxGetThread() ) -> m_nThreadID
		);
static TCHAR strTextBetweenKeys[] = _T("+");
CExtSafeString strAccelText;
	if( _dataAccel.fVirt&FCONTROL )
		strAccelText +=
			CExtCustomizeHotKeyWnd::stat_GetSingleKeyName(
				hKeyboardLayout,
				VK_CONTROL,
				strTextBetweenKeys
				);
	if( _dataAccel.fVirt&FSHIFT )
		strAccelText +=
			CExtCustomizeHotKeyWnd::stat_GetSingleKeyName(
				hKeyboardLayout,
				VK_SHIFT,
				strTextBetweenKeys
				);
	if( _dataAccel.fVirt&FALT )
		strAccelText +=
			CExtCustomizeHotKeyWnd::stat_GetSingleKeyName(
				hKeyboardLayout,
				VK_MENU,
				strTextBetweenKeys
				);
	if( _dataAccel.key == 0 )
		return strAccelText;
	if( _dataAccel.fVirt&FVIRTKEY )
		strAccelText +=
			CExtCustomizeHotKeyWnd::stat_GetSingleKeyName(
				hKeyboardLayout,
				_dataAccel.key
				);
	else
		strAccelText += (TCHAR)_dataAccel.key;
	return strAccelText;
}

void CExtCustomizeHotKeyWnd::_SyncEditText()
{
	ASSERT_VALID( this );
	if(		GetSafeHwnd() == NULL
		||	( ! ::IsWindow(GetSafeHwnd()) )
		)
		return;
CExtSafeString strText = OnGetAccelText();
	SetRedraw( FALSE );
	CEdit::SetSel( 0, -1 );
	CEdit::ReplaceSel( strText );
	CEdit::SetSel( -1, -1 );
	SetRedraw( TRUE );
	Invalidate();
}

bool CExtCustomizeHotKeyWnd::IsAccelDefined() const
{
	if( KeyGet() != 0 )
		return true;
	return false;
}

bool CExtCustomizeHotKeyWnd::IsEmpty() const
{
	ASSERT_VALID( this );
	if( KeyGet() == 0 && ModifiersGet() == 0 )
		return true;
	return false;
}

void CExtCustomizeHotKeyWnd::Empty()
{
	_Empty();
	_SyncEditText();
}

void CExtCustomizeHotKeyWnd::_Empty()
{
	ASSERT_VALID( this );
	::memset( &m_dataAccel, 0, sizeof(ACCEL) );
}

const ACCEL & CExtCustomizeHotKeyWnd::AccelGet() const
{
	ASSERT_VALID( this );
	return m_dataAccel;
}

void CExtCustomizeHotKeyWnd::AccelSet( const ACCEL & _dataAccel )
{
	ASSERT_VALID( this );
	::memcpy( &m_dataAccel, &_dataAccel, sizeof(ACCEL) );
	_SyncEditText();
}

WORD CExtCustomizeHotKeyWnd::KeyGet() const
{
	ASSERT_VALID( this );
	return m_dataAccel.key;
}

void CExtCustomizeHotKeyWnd::KeySet( WORD nKey )
{
	ASSERT_VALID( this );
	if( m_dataAccel.key == nKey )
		return;
	m_dataAccel.key = nKey;
	_SyncEditText();
	OnAccelChanged();
	if( IsAccelDefined() )
		OnAccelDefined();
}

BYTE CExtCustomizeHotKeyWnd::ModifiersGet() const
{
	ASSERT_VALID( this );
	return m_dataAccel.fVirt;
}

void CExtCustomizeHotKeyWnd::ModifiersSet( BYTE nModifiers )
{
	ASSERT_VALID( this );
	if( m_dataAccel.fVirt == nModifiers )
		return;
	m_dataAccel.fVirt = nModifiers;
	_SyncEditText();
	OnAccelChanged();
	if( IsAccelDefined() )
		OnAccelDefined();
}

BYTE CExtCustomizeHotKeyWnd::SimpleKeyModifiersGet() const
{
	ASSERT_VALID( this );
	return m_nSimpleKeyModifiers;
}

void CExtCustomizeHotKeyWnd::SimpleKeyModifiersSet( BYTE nModifiers )
{
	ASSERT_VALID( this );
	m_nSimpleKeyModifiers = nModifiers;
	m_nSimpleKeyModifiers &= FCONTROL|FALT|FSHIFT;
}

bool CExtCustomizeHotKeyWnd::AllowTabKeyInputGet() const
{
	ASSERT_VALID( this );
	return m_bAllowTabKeyInput;
}

void CExtCustomizeHotKeyWnd::AllowTabKeyInputSet( bool bAllowTabKeyInput )
{
	ASSERT_VALID( this );
	m_bAllowTabKeyInput = bAllowTabKeyInput;
}

CExtSafeString CExtCustomizeHotKeyWnd::OnGetEmptyText()
{
	ASSERT_VALID( this );
	return CExtSafeString( _T("") );
}

CExtSafeString CExtCustomizeHotKeyWnd::OnGetAccelText()
{
	ASSERT_VALID( this );
	if( IsEmpty() ) 
		return OnGetEmptyText();
CExtSafeString strText = stat_FormatAccelText( m_dataAccel );
	return strText;
}

bool CExtCustomizeHotKeyWnd::OnTestEraseAccel( const ACCEL & _dataAccel )
{
	ASSERT_VALID( this );
	if(		(	_dataAccel.key == VK_BACK
			||	_dataAccel.key == VK_DELETE
			||	_dataAccel.key == VK_ESCAPE
			)
		&&	( _dataAccel.fVirt&(FCONTROL|FALT|FSHIFT) ) == 0
		)
		return true;
	return false;
}

bool CExtCustomizeHotKeyWnd::OnAccelChanging(
	const ACCEL & _dataAccelOld,
	const ACCEL & _dataAccelNew
	)
{
	ASSERT_VALID( this );
	_dataAccelOld;
	_dataAccelNew;
	return true; // allow any changes
}

void CExtCustomizeHotKeyWnd::OnAccelChanged()
{
	ASSERT_VALID( this );
}

void CExtCustomizeHotKeyWnd::OnAccelDefined()
{
	ASSERT_VALID( this );
}

bool CExtCustomizeHotKeyWnd::OnTestSimpleKey( WORD nKey )
{
	ASSERT_VALID( this );
	if(		( 0x30 <= nKey && nKey <= 0x39 ) // Numbers
		||	( 0x41 <= nKey && nKey <= 0x5A ) // Letters
		||	nKey == VK_SPACE
		||	nKey == 0xC0 // '`'
		||	nKey == 0xBD // '-'
		||	nKey == 0xBB // '='
		||	nKey == 0xDC // '\\'
		||	nKey == 0xDB // '['
		||	nKey == 0xDD // ']'
		||	nKey == 0xBA // ';'
		||	nKey == 0xDE // '\''
		||	nKey == 0xBC // ','
		||	nKey == 0xBE // '.'
		||	nKey == 0xBF // '/'
		||	nKey == VK_LEFT
		||	nKey == VK_RIGHT
		||	nKey == VK_UP
		||	nKey == VK_DOWN
		||	nKey == VK_INSERT
		||	nKey == VK_DELETE
		||	nKey == VK_HOME
		||	nKey == VK_END
		||	nKey == VK_PRIOR
		||	nKey == VK_NEXT
		)
		return true;
	return false;
}

bool CExtCustomizeHotKeyWnd::OnTestDisabledKey( WORD nKey )
{
	ASSERT_VALID( this );
	if(		nKey == VK_NUMLOCK
		||	nKey == VK_PRINT
		||	nKey == VK_PAUSE
		)
		return true;
	return false;
}

BOOL CExtCustomizeHotKeyWnd::PreTranslateMessage( MSG * pMsg )
{
	ASSERT_VALID( this );
	if(		pMsg->message != WM_KEYDOWN
		&&	pMsg->message != WM_KEYUP
		&&	pMsg->message != WM_SYSKEYDOWN
		&&	pMsg->message != WM_SYSKEYUP
		)
		return CEdit::PreTranslateMessage( pMsg );
	if( OnTestDisabledKey( (WORD)pMsg->wParam ) )
		return TRUE;
HWND hWndFocus = ::GetFocus();
	if( hWndFocus == NULL || hWndFocus != m_hWnd )
		return CEdit::PreTranslateMessage( pMsg );
bool bDown =
		(	pMsg->message == WM_KEYDOWN
		||	pMsg->message == WM_SYSKEYDOWN
		)	? true : false;
	if( !bDown )
		return TRUE;
bool bAlt = ( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 ) ? true : false;
bool bCtrl = ( (::GetAsyncKeyState(VK_CONTROL)&0x8000) != 0 ) ? true : false;
bool bShift = ( (::GetAsyncKeyState(VK_SHIFT)&0x8000) != 0 ) ? true : false;
	if( pMsg->wParam == VK_TAB && (!AllowTabKeyInputGet()) )
	{
		if( bAlt || bCtrl )
			return TRUE;
		::SendMessage(
			::GetParent(m_hWnd),
			WM_NEXTDLGCTL,
			bShift ? 1 : 0,
			0
			);
		return TRUE;
	} // if( pMsg->wParam == VK_TAB && (!AllowTabKeyInputGet()) )
ACCEL _dataAccelSave;
	::memcpy( &_dataAccelSave, &m_dataAccel, sizeof(ACCEL) );
	_Empty();
	m_dataAccel.fVirt &= ~(FCONTROL|FALT|FSHIFT);
	if(		pMsg->wParam != VK_CONTROL
		&&	pMsg->wParam != VK_MENU
		&&	pMsg->wParam != VK_SHIFT
		)
	{
		m_dataAccel.fVirt |= FVIRTKEY;
		m_dataAccel.key = (WORD)pMsg->wParam;
	}
	else
		m_dataAccel.key = 0;
	if( bCtrl )
		m_dataAccel.fVirt |= FCONTROL;
	if( bAlt )
		m_dataAccel.fVirt |= FALT;
	if( bShift )
		m_dataAccel.fVirt |= FSHIFT;
	if(		( m_dataAccel.fVirt&(FCONTROL|FALT|FSHIFT) ) == 0
		&&	SimpleKeyModifiersGet() != 0
		&&	OnTestSimpleKey( m_dataAccel.key )
		)
		m_dataAccel.fVirt |= SimpleKeyModifiersGet();
	if( OnTestEraseAccel(m_dataAccel) )
		_Empty();
	if( ::memcmp( &_dataAccelSave, &m_dataAccel, sizeof(ACCEL) ) == 0 )
		return TRUE;
	if( ! OnAccelChanging( _dataAccelSave, m_dataAccel ) )
	{
		::memcpy( &m_dataAccel, &_dataAccelSave, sizeof(ACCEL) );
		return TRUE;
	}
	_SyncEditText();
	OnAccelChanged();
	if( IsAccelDefined() )
		OnAccelDefined();
	return TRUE;
}

LRESULT CExtCustomizeHotKeyWnd::WindowProc(
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
{
	if( WM_KEYFIRST <= message && message <= WM_KEYLAST )
		return 0;
	if(		message == WM_COPY
		||	message == WM_CUT
		||	message == WM_PASTE
		||	message == WM_CLEAR
		||	message == EM_UNDO
		)
		return 0;
	if(		message == WM_RBUTTONUP
		||	message == WM_RBUTTONDOWN
		||	message == WM_RBUTTONDBLCLK
		)
	{
		if( ::GetFocus() != m_hWnd )
			SetFocus();
		return 0;
	}
	return CEdit::WindowProc( message, wParam, lParam );
}

/////////////////////////////////////////////////////////////////////////////
// CExtCustomizePageKeyboard window

void CExtCustomizePageKeyboard::CInternalHotKeyWnd::OnAccelChanged()
{
	ASSERT_VALID( this );
	CExtCustomizeHotKeyWnd::OnAccelDefined();
CExtCustomizePageKeyboard * pPage =
		STATIC_DOWNCAST( CExtCustomizePageKeyboard, GetParent() );
	ASSERT_VALID( pPage );
	pPage->_UpdateCmdButtons();
}

IMPLEMENT_DYNCREATE(CExtCustomizePageKeyboard,CExtResPP);

CExtCustomizePageKeyboard::CExtCustomizePageKeyboard()
	: CExtResPP( IDD_EXT_CUST_PAGE_KEYBOARD, IDS_EXT_CUST_PAGE_KEYBOARD )
	, m_pItems( NULL )
	, m_pMenuInfo( NULL )
	, m_nCmdID( 0 )
{
	m_wndCommandList.m_bAllowDragStart = false;
}

CExtCustomizePageKeyboard::~CExtCustomizePageKeyboard()
{
}

void CExtCustomizePageKeyboard::DoDataExchange(CDataExchange* pDX)
{
	CExtResPP::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtCustomizePageKeyboard)
	DDX_Control(pDX, IDC_EXT_STATIC_CMD_USED, m_wndStaticCmdUsed);
	DDX_Control(pDX, IDC_EXT_STATIC_CMD_DESCRIPTION, m_wndStaticCmdDescription);
	DDX_Control(pDX, IDC_EXT_STATIC_ACCEL_GROUP, m_wndStaticAccelGroup);
	DDX_Control(pDX, IDC_EXT_LIST_COMMANDS, m_wndCommandList);
	DDX_Control(pDX, IDC_EXT_LIST_ACCEL_KEYS, m_wndListAccelKeys);
	DDX_Control(pDX, IDC_EXT_HOTKEY_NEW_ACCEL, m_wndHotKey);
	DDX_Control(pDX, IDC_EXT_COMBO_ACCEL_GROUP, m_wndComboAccelGroup);
	DDX_Control(pDX, IDC_EXT_COMBO_ACCEL_CATEGORY, m_wndComboAccelCategory);
	DDX_Control(pDX, IDC_EXT_BUTTON_ACCEL_RESET_ALL, m_wndBtnResetAll);
	DDX_Control(pDX, IDC_EXT_BUTTON_ACCEL_REMOVE, m_wndBtnRemove);
	DDX_Control(pDX, IDC_EXT_BUTTON_ACCEL_ASSIGN, m_wndBtnAssign);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CExtCustomizePageKeyboard,CExtResPP)
    //{{AFX_MSG_MAP(CExtCustomizePageKeyboard)
	ON_CBN_SELENDOK(IDC_EXT_COMBO_ACCEL_CATEGORY, OnSelendOkAccelCategoryCombo)
	ON_CBN_SELENDOK(IDC_EXT_COMBO_ACCEL_GROUP, OnSelendOkAccelGroupCombo)
	ON_LBN_SELCHANGE(IDC_EXT_LIST_ACCEL_KEYS, OnSelchangeAccelKeysList)
	ON_LBN_SELCHANGE(IDC_EXT_LIST_COMMANDS, OnSelChangeCommandList)
	ON_BN_CLICKED(IDC_EXT_BUTTON_ACCEL_ASSIGN, OnAccelAssign)
	ON_BN_CLICKED(IDC_EXT_BUTTON_ACCEL_REMOVE, OnAccelRemove)
	ON_BN_CLICKED(IDC_EXT_BUTTON_ACCEL_RESET_ALL, OnAccelResetAll)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(
		CExtCustomizeSite::g_nMsgSyncContent,
		_OnSyncContent
		)
END_MESSAGE_MAP()

LRESULT CExtCustomizePageKeyboard::_OnSyncContent(WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID( this );

	m_wndComboAccelGroup.ResetContent();
	m_wndComboAccelCategory.ResetContent();
	m_wndCommandList.ResetContent();
	m_wndListAccelKeys.ResetContent();
	m_wndHotKey.Empty();
	m_wndBtnAssign.EnableWindow( FALSE );

	m_strProfileName =
		g_CmdManager->ProfileNameFromWnd( m_hWnd );
	ASSERT( !m_strProfileName.IsEmpty() );

CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
	pSite->CategoryEnum( this );
int nCategoryCount = m_wndComboAccelCategory.GetCount();
	if( nCategoryCount > 0 )
	{
		m_wndComboAccelCategory.SetCurSel( 0 );
		OnSelendOkAccelCategoryCombo();
	}

INT nMenuCount = pSite->MenuInfoGetCount();
	if( nMenuCount >= 1 )
	{
		for( INT i = 0; i < nMenuCount; i++ )
		{
			CExtCustomizeSite::CCmdMenuInfo * pCmdMenuInfo =
				pSite->MenuInfoGetAt( i );
			ASSERT( pCmdMenuInfo != NULL );
			ASSERT( pCmdMenuInfo->IsInitialized() );
			ASSERT( pCmdMenuInfo->GetName() != NULL );
			m_wndComboAccelGroup.AddString( pCmdMenuInfo->GetName() );
		} // for( INT i = 0; i < nMenuCount; i++ )
		int nCurSel = 0;
		CExtCustomizeSite::CCmdMenuInfo * pCmdMenuInfo =
			pSite->MenuInfoActiveGet();
		if( pCmdMenuInfo != NULL )
		{
			nCurSel = pSite->MenuInfoFindIndex( pCmdMenuInfo );
			ASSERT( 0 <= nCurSel && nCurSel < nMenuCount );
		} // if( pCmdMenuInfo != NULL )
		m_wndComboAccelGroup.SetCurSel( nCurSel );
		OnSelendOkAccelGroupCombo();
	} // if( nMenuCount >= 1 )
	
	if( nMenuCount <= 1 )
	{
		m_wndStaticAccelGroup.ShowWindow( SW_HIDE );
		m_wndComboAccelGroup.ShowWindow( SW_HIDE );
	} // if( nMenuCount <= 1 )
	if( (pSite->GetCustomizeFlags()&__ECSF_ALLOW_UNMODIFIED_ACCELERATORS) == 0 )
		m_wndHotKey.SimpleKeyModifiersSet( FCONTROL );

	for(	HWND hWnd = ::GetWindow( m_hWnd, GW_CHILD );
			hWnd != NULL;
			hWnd = ::GetWindow( hWnd, GW_HWNDNEXT )
			)
		::SendMessage(
			hWnd,
			CExtCustomizeSite::g_nMsgSyncContent,
			wParam,
			lParam
			);
	return (!0L);
}

// CExtCustomizeSite::ICustomizeCategoryEnumSite::OnCustomizeCategoryEnum
bool CExtCustomizePageKeyboard::OnCustomizeCategoryEnum(
	int nPos,
	CExtCustomizeCmdTreeNode * pRoot,
	bool bVisibleInKeyboardPage
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pRoot );
	if( !bVisibleInKeyboardPage )
		return true;
__EXT_MFC_SAFE_LPCTSTR strCategoryName = pRoot->GetTextUser();
	ASSERT( strCategoryName != NULL && _tcslen(strCategoryName) > 0 );
	nPos = m_wndComboAccelCategory.GetCount();
	m_wndComboAccelCategory.InsertString(
		nPos,
		strCategoryName
		);
	m_wndComboAccelCategory.SetItemData(
		nPos,
		(DWORD)pRoot
		);
	return true;
}

BOOL CExtCustomizePageKeyboard::OnInitDialog() 
{
	CExtResPP::OnInitDialog();
	SendMessage( CExtCustomizeSite::g_nMsgSyncContent );
	return TRUE;
}

void CExtCustomizePageKeyboard::OnSelendOkAccelCategoryCombo() 
{
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
	pSite->DropTargetPopupCancelEx( -2 );

	m_wndStaticCmdDescription.SetWindowText( _T("") );
int nSel = m_wndComboAccelCategory.GetCurSel();
	ASSERT( nSel >= 0 );
	m_pItems = (CExtCustomizeCmdTreeNode *)
		m_wndComboAccelCategory.GetItemData( nSel );
	ASSERT_VALID( m_pItems );
	ASSERT( !m_strProfileName.IsEmpty() );
	m_wndCommandList.SetRedraw( FALSE );
	m_wndCommandList.ResetContent();
int nCmdCount = m_pItems->GetNodeCount();
	for( int nCmdItem = 0; nCmdItem < nCmdCount; nCmdItem++ )
	{
		CExtCustomizeCmdTreeNode * pNode =
			m_pItems->ElementAt( nCmdItem );
		ASSERT_VALID( pNode );
		m_wndCommandList.AddCommand( pNode );
	} // for( int nCmdItem = 0; nCmdItem < nCmdCount; nCmdItem++ )
	m_wndCommandList.SetRedraw( TRUE );
	m_wndCommandList.Invalidate();
	m_wndCommandList.UpdateWindow();

	OnSelChangeCommandList();
}

void CExtCustomizePageKeyboard::OnSelendOkAccelGroupCombo() 
{
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
	pSite->DropTargetPopupCancelEx( -2 );

	m_pMenuInfo = NULL;
	m_nCmdID = 0;
	m_cmdKeySet.Empty();
	if( m_wndComboAccelGroup.GetCount() == 0 )
	{
		OnSelendOkAccelCategoryCombo();
		return;
	}
int nCurSel = m_wndComboAccelGroup.GetCurSel();
	if( nCurSel < 0 )
	{
		OnSelendOkAccelCategoryCombo();
		return;
	}
CExtSafeString strMenuInfoName;
int nLbTextLen = m_wndComboAccelGroup.GetLBTextLen( nCurSel );
	if( nLbTextLen > 0 )
	{
		m_wndComboAccelGroup.GetLBText(
			nCurSel,
			strMenuInfoName.GetBuffer(nLbTextLen+1)
			);
		strMenuInfoName.ReleaseBuffer();
	}
	if( strMenuInfoName.IsEmpty() )
	{
		OnSelendOkAccelCategoryCombo();
		return;
	}
	m_pMenuInfo = pSite->MenuInfoGetByName( strMenuInfoName );
	ASSERT( m_pMenuInfo != NULL );
	OnSelendOkAccelCategoryCombo();
}

void CExtCustomizePageKeyboard::OnSelchangeAccelKeysList() 
{
	_UpdateCmdButtons();
}

void CExtCustomizePageKeyboard::OnSelChangeCommandList() 
{
	m_wndListAccelKeys.ResetContent();
	m_nCmdID = 0;
	m_cmdKeySet.Empty();
int nSel = m_wndCommandList.GetCurSel();
	if( nSel < 0 )
	{
		m_wndStaticCmdDescription.SetWindowText( _T("") );
		_UpdateCmdButtons();
		return;
	} // if( nSel < 0 )
	ASSERT_VALID( m_pItems );
	ASSERT( !m_strProfileName.IsEmpty() );
int nCmdCount = m_pItems->GetNodeCount();
	ASSERT( nSel < nCmdCount );
CExtCustomizeCmdTreeNode * pNode = m_pItems->ElementAt(nSel);
	ASSERT_VALID( pNode );
UINT nCmdID = pNode->GetCmdID( false );
CExtCmdItem * pCmdItem =
		g_CmdManager->CmdGetPtr(
			m_strProfileName,
			nCmdID
			);
	if( pCmdItem == NULL )
	{
		m_wndStaticCmdDescription.SetWindowText( _T("") );
		_UpdateCmdButtons();
		return;
	} // if( pCmdItem == NULL )
CExtSafeString strCommandDescription =
		pCmdItem->OnGetCustomizeCommandDescription();
	m_wndStaticCmdDescription.SetWindowText(
		strCommandDescription
		);

	if( m_pMenuInfo == NULL )
	{
		_UpdateCmdButtons();
		return;
	}

	m_nCmdID = nCmdID;
	if( ! m_pMenuInfo->AccelTableCmdKeySetGet( nCmdID, m_cmdKeySet ) )
	{
		m_cmdKeySet.Empty();
		_UpdateCmdButtons();
		return;
	} // if( ! m_pMenuInfo->AccelTableCmdKeySetGet( nCmdID, m_cmdKeySet ) )
int nCount = m_cmdKeySet.GetSize();
	if( nCount == 0 )
	{
		_UpdateCmdButtons();
		return;
	}
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
	m_wndListAccelKeys.SetRedraw( FALSE );
	for( int i = 0; i < nCount; i++ )
	{
		CExtCustomizeSite::CCmdKeyInfo & _cmdKeyInfo =
			m_cmdKeySet.ElementAt( i );
		ASSERT( !_cmdKeyInfo.IsEmpty() );
		CExtSafeString strText =
			pSite->OnFormatCmdAccelText(
				_cmdKeyInfo
				);
		ASSERT( !strText.IsEmpty() );
		m_wndListAccelKeys.InsertString( i, strText );
		m_wndListAccelKeys.SetItemData( i, (DWORD)_cmdKeyInfo );
	} // for( int i = 0; i < nCount; i++ )
	ASSERT( m_wndListAccelKeys.GetCount() == nCount );
	m_wndListAccelKeys.SetRedraw( TRUE );
	m_wndListAccelKeys.Invalidate();

	m_wndHotKey.Empty();
	_UpdateCmdButtons();
}

void CExtCustomizePageKeyboard::_UpdateCmdButtons()
{
CExtCustomizeSite::CCmdKeyInfo _cmdKeyInfo;
	if( m_wndHotKey.IsAccelDefined() )
		_cmdKeyInfo = m_wndHotKey.AccelGet();
	if( _cmdKeyInfo != m_lastCmdKeyInfo )
	{
		m_lastCmdKeyInfo = _cmdKeyInfo;
		CExtSafeString strUsedCmdDescription( _T("") );
		CExtCmdItem * pCmdItem = NULL;
		if( (!m_lastCmdKeyInfo.IsEmpty()) && m_pMenuInfo != NULL ) 
		{
			UINT nCmdID =
				m_pMenuInfo->AccelTableCmdFromKey( m_lastCmdKeyInfo );
			if( nCmdID != 0 )
				pCmdItem =
					g_CmdManager->CmdGetPtr(
						g_CmdManager->ProfileNameFromWnd( m_hWnd ),
						nCmdID
						);
		} // if( (!m_lastCmdKeyInfo.IsEmpty()) && m_pMenuInfo != NULL ) 
		if( pCmdItem != NULL )
		{
			strUsedCmdDescription =
				pCmdItem->OnGetCustomizeScriptName();
			CExtSafeString strAdditionalComment =
				pCmdItem->OnGetCustomizeCommandDescription();
			if( !strAdditionalComment.IsEmpty() )
			{
				strUsedCmdDescription += _T("\r\n(");
				strUsedCmdDescription += strAdditionalComment;
				strUsedCmdDescription += _T(")");
			} // if( !strAdditionalComment.IsEmpty() )
		} // if( pCmdItem != NULL )
		m_wndStaticCmdUsed.SetWindowText( strUsedCmdDescription );
	} // if( _cmdKeyInfo != m_lastCmdKeyInfo )
	
	if( (!m_lastCmdKeyInfo.IsEmpty()) && m_nCmdID != 0 )
	{
		if( !m_wndBtnAssign.IsWindowEnabled() )
			m_wndBtnAssign.EnableWindow( TRUE );
	} // if( (!m_lastCmdKeyInfo.IsEmpty()) && m_nCmdID != 0 )
	else
	{
		if( m_wndBtnAssign.IsWindowEnabled() )
			m_wndBtnAssign.EnableWindow( FALSE );
	} // else from if( (!m_lastCmdKeyInfo.IsEmpty()) && m_nCmdID != 0 )
	
	if(		m_wndListAccelKeys.GetCount() > 0
		&&	m_wndListAccelKeys.GetCurSel() >= 0
		)
	{
		if( !m_wndBtnRemove.IsWindowEnabled() )
			m_wndBtnRemove.EnableWindow( TRUE );
	}
	else
	{
		if( m_wndBtnRemove.IsWindowEnabled() )
			m_wndBtnRemove.EnableWindow( FALSE );
	}
	
	if( m_pMenuInfo != NULL )
	{
		if( !m_wndBtnResetAll.IsWindowEnabled() )
			m_wndBtnResetAll.EnableWindow( TRUE );
	}
	else
	{
		if( m_wndBtnResetAll.IsWindowEnabled() )
			m_wndBtnResetAll.EnableWindow( FALSE );
	}
}

void CExtCustomizePageKeyboard::OnAccelAssign() 
{
	if(		m_pMenuInfo == NULL
		||	m_nCmdID == 0
		||	(! m_wndHotKey.IsAccelDefined() )
		)
		return;
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
	pSite->DropTargetPopupCancelEx( -2 );
const ACCEL & _dataAccel = m_wndHotKey.AccelGet();
CExtCustomizeSite::CCmdKeyInfo _cmdKeyInfo( _dataAccel );
	ASSERT( !_cmdKeyInfo.IsEmpty() );
	if( m_cmdKeySet.KeyFindPos(_cmdKeyInfo) >= 0 )
		return;
	VERIFY( m_cmdKeySet.KeySetup(_cmdKeyInfo) );
	m_pMenuInfo->AccelTableCmdKeyAssign( m_nCmdID, _cmdKeyInfo );
CExtSafeString strText =
		pSite->OnFormatCmdAccelText(
			_cmdKeyInfo
			);
	ASSERT( !strText.IsEmpty() );
int nInsertPos = m_wndListAccelKeys.GetCount();
	m_wndListAccelKeys.InsertString( nInsertPos, strText );
	m_wndListAccelKeys.SetItemData( nInsertPos, (DWORD)_cmdKeyInfo );
	m_wndListAccelKeys.SetCurSel( nInsertPos );
	m_lastCmdKeyInfo.Empty();
	m_wndStaticCmdUsed.SetWindowText( _T("") );
	m_wndHotKey.Empty();
	_UpdateCmdButtons();
}

void CExtCustomizePageKeyboard::OnAccelRemove() 
{
	if(		m_pMenuInfo == NULL
		||	m_nCmdID == 0
		||	m_wndListAccelKeys.GetCount() == 0
		)
		return;
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
	pSite->DropTargetPopupCancelEx( -2 );
int nCurSel = m_wndListAccelKeys.GetCurSel();
	if( nCurSel < 0 )
		return;
DWORD dwKeyInfo = m_wndListAccelKeys.GetItemData( nCurSel );
CExtCustomizeSite::CCmdKeyInfo _cmdKeyInfo( dwKeyInfo );
	ASSERT( !_cmdKeyInfo.IsEmpty() );
	m_pMenuInfo->AccelTableCmdKeyRemove( _cmdKeyInfo );
	m_lastCmdKeyInfo.Empty();
	m_wndStaticCmdUsed.SetWindowText( _T("") );
	VERIFY( m_cmdKeySet.KeyRemove(_cmdKeyInfo) );
	m_wndListAccelKeys.DeleteString( nCurSel );
	_UpdateCmdButtons();
}

void CExtCustomizePageKeyboard::OnAccelResetAll() 
{
	if( m_pMenuInfo == NULL )
		return;
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
	pSite->DropTargetPopupCancelEx( -2 );
CExtSafeString strQst;
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
CExtLocalResourceHelper _LRH;
	if( ! g_ResourceManager->LoadString( strQst, IDS_EXT_RESET_ALL_ACCELERATORS ) )
	{
		ASSERT( FALSE );
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		strQst = _T("Are you sure you want to reset the key assignments?");
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
	}
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
	if( ::AfxMessageBox( strQst, MB_YESNO ) != IDYES )
		return;
	m_pMenuInfo->AccelTableReset();
	OnSelChangeCommandList();
}

/////////////////////////////////////////////////////////////////////////////
// CExtCustomizePageParms window

IMPLEMENT_DYNCREATE(CExtCustomizePageParms,CExtResPP);

CExtCustomizePageParms::CExtCustomizePageParms()
	: CExtResPP( IDD_EXT_CUST_PAGE_PARMS, IDS_EXT_CUST_PAGE_PARMS )
	, m_bPageInitComplete( false )
{
}

CExtCustomizePageParms::~CExtCustomizePageParms()
{
}

void CExtCustomizePageParms::DoDataExchange(CDataExchange* pDX)
{
	CExtResPP::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtCustomizePageParms)
	DDX_Control(pDX, IDC_EXT_CHECK_MENU_ANIMATE_EXPANDING, m_wndCheckMenuExpandingAnimation);
	DDX_Control(pDX, IDC_EXT_CHECK_MENU_SHADOWS, m_wndCheckDisplayMenuShadows);
	DDX_Control(pDX, IDC_EXT_CHECK_HIGHLIGHT_RARELY, m_wndCheckHighlightRarelyMenuItems);
	DDX_Control(pDX, IDC_EXT_CHECK_TT_SC, m_wndCheckShortcutsInTT);
	DDX_Control(pDX, IDC_EXT_CHECK_TIPS_IN_TOOLBARS, m_wndCheckTipsInToolbars);
	DDX_Control(pDX, IDC_EXT_CHECK_TIPS_IN_MENUS, m_wndCheckTipsInMenus);
	DDX_Control(pDX, IDC_EXT_CHECK_LARGE_ICONS_M, m_wndCheckLargeIconsInMenu);
	DDX_Control(pDX, IDC_EXT_CHECK_LARGE_ICONS_T, m_wndCheckLargeIconsInToolbar);
	DDX_Control(pDX, IDC_EXT_CHECK_MENU_DELAY_EXPANDING, m_wndCheckMenuDelayExpanding);
	DDX_Control(pDX, IDC_EXT_CHECK_FULL_MENUS, m_wndCheckFullMenus);
	DDX_Control(pDX, IDC_EXT_COMBO_MENU_ANIMATION_TYPE, m_wndComboMenuAnimationType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CExtCustomizePageParms,CExtResPP)
    //{{AFX_MSG_MAP(CExtCustomizePageParms)
	ON_BN_CLICKED(IDC_EXT_BUTTON_RESET_CMD_STATS, OnResetCmdStats)
	ON_BN_CLICKED(IDC_EXT_CHECK_MENU_ANIMATE_EXPANDING, OnCheckMenuExpandingAnimation)
	ON_BN_CLICKED(IDC_EXT_CHECK_MENU_SHADOWS, OnCheckDisplayMenuShadows)
	ON_BN_CLICKED(IDC_EXT_CHECK_HIGHLIGHT_RARELY, OnCheckHighlightRarelyMenuItems)
	ON_BN_CLICKED(IDC_EXT_CHECK_FULL_MENUS, OnCheckFullMenus)
	ON_BN_CLICKED(IDC_EXT_CHECK_MENU_DELAY_EXPANDING, OnCheckMenuDelayExpanding)
	ON_BN_CLICKED(IDC_EXT_CHECK_LARGE_ICONS_M, OnCheckLargeIconsInMenu)
	ON_BN_CLICKED(IDC_EXT_CHECK_LARGE_ICONS_T, OnCheckLargeIconsInToolbar)
	ON_BN_CLICKED(IDC_EXT_CHECK_TIPS_IN_MENUS, OnCheckTipsInMenus)
	ON_BN_CLICKED(IDC_EXT_CHECK_TIPS_IN_TOOLBARS, OnCheckTipsInToolbars)
	ON_BN_CLICKED(IDC_EXT_CHECK_TT_SC, OnCheckToolbarTipsWithShortcuts)
	ON_CBN_SELENDOK(IDC_EXT_COMBO_MENU_ANIMATION_TYPE, OnSelendOkComboMenuAnimationType)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(
		CExtCustomizeSite::g_nMsgSyncContent,
		_OnSyncContent
		)
END_MESSAGE_MAP()

LRESULT CExtCustomizePageParms::_OnSyncContent(WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID( this );

	m_bPageInitComplete = false;

	m_wndCheckFullMenus.SetCheck(
		CExtPopupMenuWnd::g_bMenuExpanding ? 0 : 1
		);
	m_wndCheckMenuDelayExpanding.SetCheck(
		CExtPopupMenuWnd::g_bMenuDelayExpanding ? 1 : 0
		);
	m_wndCheckHighlightRarelyMenuItems.SetCheck(
		CExtPopupMenuWnd::g_bMenuHighlightRarely ? 1 : 0
		);
	m_wndCheckMenuExpandingAnimation.SetCheck(
		CExtPopupMenuWnd::g_bMenuExpandAnimation ? 1 : 0
		);
	m_wndCheckDisplayMenuShadows.SetCheck(
		CExtPopupMenuWnd::g_bMenuWithShadows ? 1 : 0
		);

	m_wndCheckLargeIconsInMenu.SetCheck(
		CExtPopupMenuWnd::g_bMenuLargeIcons ? 1 : 0
		);
	m_wndCheckLargeIconsInToolbar.SetCheck(
		CExtToolControlBar::g_bToolbarLargeIcons ? 1 : 0
		);
	m_wndCheckTipsInMenus.SetCheck(
		CExtPopupMenuWnd::g_bMenuShowCoolTips ? 1 : 0
		);
	m_wndCheckTipsInToolbars.SetCheck(
		CExtToolControlBar::g_bToolbarScreenTips ? 1 : 0
		);
	m_wndCheckShortcutsInTT.SetCheck(
		CExtToolControlBar::g_bToolbarShortcutKeysOnScreenTips ? 1 : 0
		);
	
	m_wndComboMenuAnimationType.ResetContent();
	ASSERT( m_wndComboMenuAnimationType.GetCount() == 0 );
	for( int i = 0; i <= int(CExtPopupBaseWnd::__AT_MAX_VALID_VALUE); i++ )
		m_wndComboMenuAnimationType.AddString(
			CExtPopupBaseWnd::GetAnimaptionName(
				CExtPopupBaseWnd::e_animation_type_t(i)
				)
			);
	ASSERT(
		m_wndComboMenuAnimationType.GetCount() ==
			(((int)CExtPopupBaseWnd::__AT_MAX_VALID_VALUE)+1)
		);
	ASSERT(
			0 <= ((int)CExtPopupBaseWnd::g_DefAnimationType)
		&&	((int)CExtPopupBaseWnd::g_DefAnimationType) <= ((int)CExtPopupBaseWnd::__AT_MAX_VALID_VALUE)
		);
	m_wndComboMenuAnimationType.SetCurSel(
		((int)CExtPopupBaseWnd::g_DefAnimationType)
		);

	m_bPageInitComplete = true;

	for(	HWND hWnd = ::GetWindow( m_hWnd, GW_CHILD );
			hWnd != NULL;
			hWnd = ::GetWindow( hWnd, GW_HWNDNEXT )
			)
		::SendMessage(
			hWnd,
			CExtCustomizeSite::g_nMsgSyncContent,
			wParam,
			lParam
			);
	return (!0L);
}

BOOL CExtCustomizePageParms::OnInitDialog() 
{
	CExtResPP::OnInitDialog();
	ASSERT( !m_bPageInitComplete );
	SendMessage( CExtCustomizeSite::g_nMsgSyncContent );
	_UpdateParmsItems();
	return TRUE;
}

void CExtCustomizePageParms::_UpdateParmsItems()
{
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
	if( (pSite->GetCustomizeFlags()&__ECSF_PARMS_DISABLE_PERSONALIZED) != 0 )
	{
		if( m_wndCheckFullMenus.IsWindowEnabled() )
			m_wndCheckFullMenus.EnableWindow( FALSE );
		if( m_wndCheckMenuDelayExpanding.IsWindowEnabled() )
			m_wndCheckMenuDelayExpanding.EnableWindow( FALSE );
		if( m_wndCheckMenuExpandingAnimation.IsWindowEnabled() )
			m_wndCheckMenuExpandingAnimation.EnableWindow( FALSE );
		if( m_wndCheckHighlightRarelyMenuItems.IsWindowEnabled() )
			m_wndCheckHighlightRarelyMenuItems.EnableWindow( FALSE );
		if( m_wndCheckDisplayMenuShadows.IsWindowEnabled() )
			m_wndCheckDisplayMenuShadows.EnableWindow( FALSE );
		CWnd * pWnd = GetDlgItem(IDC_EXT_BUTTON_RESET_CMD_STATS);
		ASSERT( pWnd != NULL );
		if( pWnd->IsWindowEnabled() )
			pWnd->EnableWindow( FALSE );
	} // if( (pSite->GetCustomizeFlags()&__ECSF_PARMS_DISABLE_PERSONALIZED) != 0 )
	else
	{
		if( CExtPopupMenuWnd::g_bMenuExpanding )
		{
			if( !m_wndCheckMenuDelayExpanding.IsWindowEnabled() )
				m_wndCheckMenuDelayExpanding.EnableWindow( TRUE );
			if( !m_wndCheckMenuExpandingAnimation.IsWindowEnabled() )
				m_wndCheckMenuExpandingAnimation.EnableWindow( TRUE );
		} // if( CExtPopupMenuWnd::g_bMenuExpanding )
		else
		{
			if( m_wndCheckMenuDelayExpanding.IsWindowEnabled() )
				m_wndCheckMenuDelayExpanding.EnableWindow( FALSE );
			if( m_wndCheckMenuExpandingAnimation.IsWindowEnabled() )
				m_wndCheckMenuExpandingAnimation.EnableWindow( FALSE );
		} // else from if( CExtPopupMenuWnd::g_bMenuExpanding )
	} // else from if( (pSite->GetCustomizeFlags()&__ECSF_PARMS_DISABLE_PERSONALIZED) != 0 )

	if( (pSite->GetCustomizeFlags()&__ECSF_PARMS_DISABLE_OTHER) != 0 )
	{
		if( m_wndCheckLargeIconsInMenu.IsWindowEnabled() )
			m_wndCheckLargeIconsInMenu.EnableWindow( FALSE );
		if( m_wndCheckLargeIconsInToolbar.IsWindowEnabled() )
			m_wndCheckLargeIconsInToolbar.EnableWindow( FALSE );
		if( m_wndCheckTipsInMenus.IsWindowEnabled() )
			m_wndCheckTipsInMenus.EnableWindow( FALSE );
		if( m_wndCheckTipsInToolbars.IsWindowEnabled() )
			m_wndCheckTipsInToolbars.EnableWindow( FALSE );
		if( m_wndCheckShortcutsInTT.IsWindowEnabled() )
			m_wndCheckShortcutsInTT.EnableWindow( FALSE );
		if( m_wndComboMenuAnimationType.IsWindowEnabled() )
			m_wndComboMenuAnimationType.EnableWindow( FALSE );
	} // if( (pSite->GetCustomizeFlags()&__ECSF_PARMS_DISABLE_OTHER) != 0 )
	else
	{
		if( CExtToolControlBar::g_bToolbarScreenTips )
		{
			if( !m_wndCheckShortcutsInTT.IsWindowEnabled() )
				m_wndCheckShortcutsInTT.EnableWindow( TRUE );
		} // if( CExtToolControlBar::g_bToolbarScreenTips )
		else
		{
			if( m_wndCheckShortcutsInTT.IsWindowEnabled() )
				m_wndCheckShortcutsInTT.EnableWindow( FALSE );
		} // else from if( CExtToolControlBar::g_bToolbarScreenTips )
	} // else from if( (pSite->GetCustomizeFlags()&__ECSF_PARMS_DISABLE_OTHER) != 0 )
}

void CExtCustomizePageParms::OnResetCmdStats() 
{
	if( !m_bPageInitComplete )
		return;
CExtSafeString strQst( _T("") );
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
	{ // BLOCK
		CExtLocalResourceHelper _LRH;
		if( ! g_ResourceManager->LoadString( strQst, IDS_EXT_RESET_ALL_CMD_STATS ) )
		{
			ASSERT( FALSE );
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
			strQst = _T("This will delete the record of the commands you've used in this application and restore the default set of visible commands to the menus and toolbars. It will not undo any explicit customizations. Are you sure you want to do this?");
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		}
	} // BLOCK
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
	if( ::AfxMessageBox( strQst, MB_YESNO ) != IDYES )
		return;
	VERIFY(
		g_CmdManager->ProfileResetUsageStatistics(
			g_CmdManager->ProfileNameFromWnd( m_hWnd )
			)
		);
}

void CExtCustomizePageParms::OnCheckFullMenus() 
{
	if( !m_bPageInitComplete )
		return;
	CExtPopupMenuWnd::g_bMenuExpanding =
		m_wndCheckFullMenus.GetCheck() ? false : true;
	_UpdateParmsItems();
}
void CExtCustomizePageParms::OnCheckMenuDelayExpanding() 
{
	if( !m_bPageInitComplete )
		return;
	CExtPopupMenuWnd::g_bMenuDelayExpanding =
		m_wndCheckMenuDelayExpanding.GetCheck() ? true : false;
}
void CExtCustomizePageParms::OnCheckHighlightRarelyMenuItems() 
{
	if( !m_bPageInitComplete )
		return;
	CExtPopupMenuWnd::g_bMenuHighlightRarely =
		m_wndCheckHighlightRarelyMenuItems.GetCheck() ? true : false;
}
void CExtCustomizePageParms::OnCheckMenuExpandingAnimation()
{
	if( !m_bPageInitComplete )
		return;
	CExtPopupMenuWnd::g_bMenuExpandAnimation =
		m_wndCheckMenuExpandingAnimation.GetCheck() ? true : false;
}
void CExtCustomizePageParms::OnCheckDisplayMenuShadows()
{
	if( !m_bPageInitComplete )
		return;
	CExtPopupMenuWnd::g_bMenuWithShadows =
		m_wndCheckDisplayMenuShadows.GetCheck() ? true : false;
}
void CExtCustomizePageParms::OnCheckLargeIconsInMenu() 
{
	if( !m_bPageInitComplete )
		return;
	CExtPopupMenuWnd::g_bMenuLargeIcons =
		m_wndCheckLargeIconsInMenu.GetCheck() ? true : false;
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
	pSite->DropTargetPopupCancelEx( -2 );
}
void CExtCustomizePageParms::OnCheckLargeIconsInToolbar() 
{
	if( !m_bPageInitComplete )
		return;
	CExtToolControlBar::g_bToolbarLargeIcons =
		m_wndCheckLargeIconsInToolbar.GetCheck() ? true : false;
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	ASSERT( pSite != NULL );
	pSite->DropTargetPopupCancelEx( -2 );
	pSite->OnChangedToolbarIconSizeOption();
}
void CExtCustomizePageParms::OnCheckTipsInMenus() 
{
	if( !m_bPageInitComplete )
		return;
	CExtPopupMenuWnd::g_bMenuShowCoolTips =
		m_wndCheckTipsInMenus.GetCheck() ? true : false;
}
void CExtCustomizePageParms::OnCheckTipsInToolbars() 
{
	if( !m_bPageInitComplete )
		return;
	CExtToolControlBar::g_bToolbarScreenTips =
		m_wndCheckTipsInToolbars.GetCheck() ? true : false;
	_UpdateParmsItems();
}
void CExtCustomizePageParms::OnCheckToolbarTipsWithShortcuts() 
{
	if( !m_bPageInitComplete )
		return;
	CExtToolControlBar::g_bToolbarShortcutKeysOnScreenTips =
		m_wndCheckShortcutsInTT.GetCheck() ? true : false;
}

void CExtCustomizePageParms::OnSelendOkComboMenuAnimationType() 
{
	if( !m_bPageInitComplete )
		return;
	ASSERT(
		m_wndComboMenuAnimationType.GetCount() ==
			(((int)CExtPopupBaseWnd::__AT_MAX_VALID_VALUE)+1)
		);
int nCurSel = m_wndComboMenuAnimationType.GetCurSel();
	if( nCurSel < 0 )
		return;
	ASSERT( nCurSel <= ((int)CExtPopupBaseWnd::__AT_MAX_VALID_VALUE) );
	CExtPopupBaseWnd::g_DefAnimationType =
		(CExtPopupBaseWnd::e_animation_type_t)
			nCurSel;
}

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

