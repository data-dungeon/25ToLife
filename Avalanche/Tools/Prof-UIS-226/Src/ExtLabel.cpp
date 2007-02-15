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

#if (!defined __EXT_LABEL_H)
	#include <ExtLabel.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtLabel

IMPLEMENT_DYNCREATE( CExtLabel, CStatic );

CExtLabel::CExtLabel()
: m_bFontBold(false)
, m_bFontItalic(false)
, m_bFontUnderline(false)
, m_bFontStrikeOut(false)
{
}

CExtLabel::~CExtLabel()
{
}

BEGIN_MESSAGE_MAP(CExtLabel, CStatic)
	//{{AFX_MSG_MAP(CExtLabel)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CExtLabel::OnEraseBkgnd(CDC* pDC) 
{
	pDC;
	return TRUE;
}

void CExtLabel::OnPaint() 
{
CPaintDC dcPaint( this );
CRect rcClient;
	GetClientRect( &rcClient );
	if( ! dcPaint.RectVisible(&rcClient) )
		return;
CExtMemoryDC dc( &dcPaint, &rcClient );
CRgn rgnClient;
	if( rgnClient.CreateRectRgnIndirect( &rcClient ) )
		dc.SelectClipRgn( &rgnClient );
	if( g_PaintManager->GetCb2DbTransparentMode(this) )
	{
		CExtPaintManager::stat_ExcludeChildAreas(
			dc,
			GetSafeHwnd(),
			CExtPaintManager::stat_DefExcludeChildAreaCallback
			);
		g_PaintManager->PaintDockerBkgnd( dc, this );
	} // if( g_PaintManager->GetCb2DbTransparentMode(this) )
	else
		dc.FillSolidRect( &rcClient, g_PaintManager->GetColor(CExtPaintManager::CLR_3DFACE_OUT) );
CString strText;
	GetWindowText( strText );
	if( strText.GetLength() > 0 )
	{
		DWORD dwWndStyle = GetStyle();
		DWORD dwDrawTextFlags = 0;
		switch( (dwWndStyle&SS_TYPEMASK) )
		{
		case SS_RIGHT: 
			dwDrawTextFlags = DT_RIGHT|DT_WORDBREAK; 
			break; 
		case SS_CENTER: 
			dwDrawTextFlags = SS_CENTER|DT_WORDBREAK;
			break;
		case SS_LEFTNOWORDWRAP: 
			dwDrawTextFlags = DT_LEFT; 
			break;
		default: // all the other types assumed as left
		case SS_LEFT: 
			dwDrawTextFlags = DT_LEFT|DT_WORDBREAK; 
			break;
		} // switch( (dwWndStyle&SS_TYPEMASK) )
		if( strText.Find( _T('\t') ) != -1 ) // do tabs expanding
			dwDrawTextFlags |= DT_EXPANDTABS;
		if( (dwWndStyle&SS_CENTERIMAGE) != 0 )
		{ // center vertically
			dwDrawTextFlags = DT_CENTER;
			// DT_VCENTER is for DT_SINGLELINE only
			if( strText.Find( _T("\r\n") ) == -1 )
				dwDrawTextFlags |= DT_VCENTER|DT_SINGLELINE;
		} // center vertically
		dwDrawTextFlags |= (DT_VCENTER|DT_END_ELLIPSIS); 
		bool bEnabled = IsWindowEnabled() ? true : false;
		OnDrawLabelText(
			dc,
			rcClient,
			strText,
			dwDrawTextFlags,
			bEnabled
			);
	} // if( strText.GetLength() > 0 )
	g_PaintManager->OnPaintSessionComplete( this );
	if( rgnClient.GetSafeHandle() != NULL )
		dc.SelectClipRgn( &rgnClient );
}

void CExtLabel::OnDrawLabelText(
	CDC & dc,
	const RECT & rcText,
	__EXT_MFC_SAFE_LPCTSTR strText,
	DWORD dwDrawTextFlags,
	bool bEnabled
	)
{
HFONT hFont = (HFONT)
		::SendMessage( m_hWnd, WM_GETFONT, 0L, 0L );
	if( hFont == NULL )
	{
		HWND hWndParent = ::GetParent( m_hWnd );
		if( hWndParent != NULL )
			hFont = (HFONT)
				::SendMessage( hWndParent, WM_GETFONT, 0L, 0L );
	} // if( hFont == NULL )
	if( hFont == NULL )
	{
		hFont = (HFONT)::GetStockObject( DEFAULT_GUI_FONT );
		if( hFont == NULL )
			hFont = (HFONT)::GetStockObject( SYSTEM_FONT );
	} // if( hFont == NULL )

	LOGFONT lf;
	::GetObject( 
		(HGDIOBJ)hFont, 
        sizeof(lf), 
        (LPVOID) &lf 
		);
	hFont = NULL;
	
	lf.lfWeight = m_bFontBold ? FW_BOLD : FW_NORMAL;
	lf.lfItalic = (BYTE)( m_bFontItalic ? 1 : 0 );
	lf.lfUnderline = (BYTE)( m_bFontUnderline ? 1 : 0 );
	lf.lfStrikeOut = (BYTE)( m_bFontStrikeOut ? 1 : 0 );
	
	hFont = ::CreateFontIndirect( &lf );
	ASSERT( hFont != NULL );
CFont _fontDestructor;
	_fontDestructor.Attach( hFont );
	
HGDIOBJ hOldFont = NULL;
	if( hFont != NULL )
		hOldFont = ::SelectObject( dc, (HGDIOBJ)hFont );
INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
COLORREF clrOldText =
		dc.SetTextColor(
			g_PaintManager->GetColor(
				bEnabled
					? COLOR_BTNTEXT
					: COLOR_3DSHADOW
				)
			);
CRect rc( rcText );
	dc.DrawText(
		LPCTSTR(strText),
		_tcslen(strText),
		rc,
		dwDrawTextFlags
		);
	dc.SetTextColor( clrOldText );
	dc.SetBkMode( nOldBkMode );
	if( hFont != NULL )
		::SelectObject( dc, hOldFont );
}

LRESULT CExtLabel::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(		message == WM_SETTEXT 
		||	message == WM_ENABLE
		)
	{
		SetRedraw( FALSE );
		LRESULT lResult = CStatic::WindowProc(message, wParam, lParam);
		SetRedraw( TRUE );
		if( (GetStyle()&WS_CHILD) != 0 )
		{
			CWnd * pWndParent = GetParent();
			ASSERT_VALID( pWndParent );
			CRect rc;
			GetWindowRect( &rc );
			pWndParent->ScreenToClient( &rc );
			pWndParent->InvalidateRect( &rc );
		} // if( (GetStyle()&WS_CHILD) != 0 )
		Invalidate();
		return lResult;
	}
	return CStatic::WindowProc(message, wParam, lParam);
}
