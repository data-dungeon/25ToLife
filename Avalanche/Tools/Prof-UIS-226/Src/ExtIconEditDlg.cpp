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

#if (!defined __EXT_MFC_NO_ICONEDITDLG)

#if (!defined __EXT_ICONEDITDLG_H)
	#include <ExtIconEditDlg.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_LOCALIZATION_H)
	#include <../Src/ExtLocalization.h>
#endif

#include <../profuisdll/resource.h>

#define __EXT_ICONEDITDLG_PROFILE_NAME _T("CExtIconEditDlg")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtIconEditDlg::CIconToolsToolbar

CExtIconEditDlg::CIconToolsToolbar::CIconToolsToolbar()
{
}

CExtIconEditDlg::CIconToolsToolbar::~CIconToolsToolbar()
{
}

bool CExtIconEditDlg::CIconToolsToolbar::IsLargeIcons() const
{
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CExtIconEditDlg::CSurfaceEditor

CExtIconEditDlg::CSurfaceEditor::CSurfaceEditor()
	: m_hBrushTransparentBk( NULL )
	, m_bSyncingColorForeground( false )
	, m_bSyncingColorBackground( false )
	, m_ptMouseTrace( -1, -1 )
	, m_sizeSelTrace( 0, 0 )
{
//	m_bEraseBkBeforeDraggingSelection = false;
//	m_bEraseBkBeforeResizingSelection = false;
}

CExtIconEditDlg::CSurfaceEditor::~CSurfaceEditor()
{
}

void CExtIconEditDlg::CSurfaceEditor::_TrackCommandMenu( UINT nCmdID )
{
	ASSERT_VALID( this );
CExtToolControlBar * pWndBar =
		&( ((CExtIconEditDlg*)GetParent())->m_wndIconTools );
	ASSERT_VALID( pWndBar );
INT nIdx = pWndBar->CommandToIndex( nCmdID );
	ASSERT( 0 <= nIdx && nIdx < pWndBar->GetButtonsCount() );
	VERIFY( pWndBar->TrackButtonMenu( nIdx ) );
}

void CExtIconEditDlg::CSurfaceEditor::_SyncViewerSurface()
{
	ASSERT_VALID( this );
	if( IedIsEmpty() )
		return;
HBITMAP hBitmap = IedBitmapGet();
	if( hBitmap == NULL )
	{
		ASSERT( FALSE );
		return;
	}
COLORREF * pClrSurface = NULL;
	hBitmap = CExtPaintManager::stat_CloneBitmap(
		hBitmap,
		NULL,
		&pClrSurface
		);
	if( hBitmap == NULL )
	{
		ASSERT( FALSE );
		return;
	}
	ASSERT( pClrSurface != NULL );
COLORREF clrTransparent = IedGetTransparencyColor();
	if( clrTransparent != ((COLORREF)(-1L)) )
	{
		COLORREF clrQuadTransparent =
			RGB(
				GetBValue(clrTransparent),
				GetGValue(clrTransparent),
				GetRValue(clrTransparent)
				);
		CSize _sizeImg = IedSizeGet();
		LONG nPixelCount = _sizeImg.cx * _sizeImg.cy;
		for( LONG nPixel = 0; nPixel < nPixelCount; nPixel++ )
			pClrSurface[nPixel] = clrQuadTransparent;
		CDC dc;
		if( dc.CreateCompatibleDC( NULL ) )
		{
			HGDIOBJ hBmpOld = ::SelectObject( dc, hBitmap );
			IedRender( dc );
			::SelectObject( dc, hBmpOld );
		} // if( dc.CreateCompatibleDC( NULL ) )
#ifdef _DEBUG
		else
		{
			ASSERT( FALSE );
		} // else from if( dc.CreateCompatibleDC( NULL ) )
#endif // _DEBUG
	} // if( clrTransparent != ((COLORREF)(-1L)) )

CExtImageEditWnd * pWndIconViewer =
		&( ((CExtIconEditDlg*)GetParent())->m_wndIconViewer );
	ASSERT_VALID( pWndIconViewer );
	pWndIconViewer->IedBitmapSet( hBitmap );
}

bool CExtIconEditDlg::CSurfaceEditor::IedPasteFromClipboard(
	bool bRedrawWnd
	)
{
	ASSERT_VALID( this );
	if( IedIsTransparencyEnabled() )
		IedColorBackgroundSet( IedGetTransparencyColor() );
	return CExtImageEditWnd::IedPasteFromClipboard( bRedrawWnd );
}

INT CExtIconEditDlg::CSurfaceEditor::IedUndo(
	INT nSteps
	)
{
	ASSERT_VALID( this );
INT nRetVal = CExtImageEditWnd::IedUndo( nSteps );
	_SyncViewerSurface();
	return nRetVal;
}

void CExtIconEditDlg::CSurfaceEditor::OnIedRectChange(
	const CRect & rcSelection,
	bool bTrackingSelection
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
	bTrackingSelection;
	m_sizeSelTrace = rcSelection.Size();
CExtIconEditDlg * pIconEditDlg = ((CExtIconEditDlg *)GetParent());
	if( pIconEditDlg->GetSafeHwnd() == NULL )
		return;
	pIconEditDlg->OnUpdateInfoWindows();
}

void CExtIconEditDlg::CSurfaceEditor::OnIedActiveColorChange(
	bool bForeground,
	COLORREF clrOld,
	COLORREF clrNew
	)
{
	ASSERT_VALID( this );
	if( bForeground && m_bSyncingColorForeground )
		return;
	if( (!bForeground) && m_bSyncingColorBackground )
		return;
	CExtImageEditWnd::OnIedActiveColorChange(
		bForeground,
		clrOld,
		clrNew
		);
CExtIconEditDlg * pIconEditDlg = ((CExtIconEditDlg *)GetParent());
	if( pIconEditDlg->GetSafeHwnd() == NULL )
		return;
CExtColorPaletteWnd * pWndPalette = &pIconEditDlg->m_wndColorPalette;
	ASSERT_VALID( pWndPalette );
	if( bForeground )
	{
		m_bSyncingColorForeground = true;
		pWndPalette->CpwForegroundColorSet( clrNew );
		m_bSyncingColorForeground = false;
	} // if( bForeground )
	else
	{
		m_bSyncingColorBackground = true;
		pWndPalette->CpwBackgroundColorSet( clrNew );
		m_bSyncingColorBackground = false;
	} // else from if( bForeground )
}

void CExtIconEditDlg::CSurfaceEditor::OnIedPaintTransparencyBk( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( m_hBrushTransparentBk == NULL )
	{
		CExtImageEditWnd::OnIedPaintTransparencyBk( dc );
		return;
	}
CSize _sizeImage = IedSizeGet();
CRect _rcImage( 0, 0, _sizeImage.cx, _sizeImage.cy );
	_rcImage = OnIedPicToClient( _rcImage, false );
	_rcImage.OffsetRect( CExtScrollWnd::OnSwGetScrollPos() );
	::FillRect( dc, &_rcImage, m_hBrushTransparentBk );
}

void CExtIconEditDlg::CSurfaceEditor::OnIedQuerySelParms(
	INT nMouseMsgFlags,
	bool bAllImage,
	bool * p_bStretch,
	bool * p_bHalftone,
	COLORREF * p_clrTransparent,
	COLORREF * p_clrOuterParts
	)
{
	ASSERT_VALID( this );
	CExtImageEditWnd::OnIedQuerySelParms(
		nMouseMsgFlags,
		bAllImage,
		p_bStretch,
		p_bHalftone,
		p_clrTransparent,
		p_clrOuterParts
		);
	if( p_clrTransparent != NULL )
	{
		*p_clrTransparent = IedColorBackgroundGet();
//		if( IedIsTransparencyEnabled() )
//		{
//			COLORREF clrTransparentMode = IedGetTransparencyColor();
//			ASSERT( clrTransparentMode != __IED_COLORREF_NO_TRANSPARENCY );
//			m_clrLastCacheTransparency = clrTransparentMode;
//		} // if( IedIsTransparencyEnabled() )
//		else
//		{
//			COLORREF clrTransparentMode = IedColorBackgroundGet();
//			if( clrTransparentMode != __IED_COLORREF_NO_TRANSPARENCY )
//				m_clrLastCacheTransparency = clrTransparentMode;
//		} // else from if( IedIsTransparencyEnabled() )
	} // if( p_clrTransparent != NULL )
}

LRESULT CExtIconEditDlg::CSurfaceEditor::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID( this );
	if( message == WM_GETDLGCODE )
		return DLGC_WANTALLKEYS;
	if( message == WM_KEYDOWN && int(wParam) == VK_TAB )
	{
		HWND hWndParent = GetParent()->GetSafeHwnd();
		if(		hWndParent != NULL
			&&	::IsWindow( hWndParent )
			//&&	( ((DWORD)::GetWindowLong(hWndParent,GWL_STYLE)) & WS_POPUP ) != 0
			)
			::SendMessage(
				hWndParent,
				WM_NEXTDLGCTL,
				CExtPopupMenuWnd::IsKeyPressed(VK_SHIFT),
				0
				);
		return 0;
	} // if( message == WM_KEYDOWN && int(wParam) == VK_TAB )
	if( message == WM_MOUSEMOVE )
	{
		_TraceMousePoint();
	} // if( message == WM_MOUSEMOVE )
	if( message == WM_TIMER && wParam == ((WPARAM)0x433) )
	{
		_TraceMousePoint();
		return 0;
	} // if( message == WM_TIMER && wParam == ((WPARAM)0x433) )
	return CExtImageEditWnd::WindowProc(message,wParam,lParam);
}

void CExtIconEditDlg::CSurfaceEditor::_TraceMousePoint()
{
	ASSERT_VALID( this );
CPoint ptMouseTrace( -1, -1 );
CPoint ptCursor( 0, 0 );
	if( ! ::GetCursorPos(&ptCursor) )
		return;
	ScreenToClient( &ptCursor );
	ptCursor = OnIedClientToPic( ptCursor, false );
CSize _sizeImage = IedSizeGet();
	if(		0 <= ptCursor.x && ptCursor.x < _sizeImage.cx
		&&	0 <= ptCursor.y && ptCursor.y < _sizeImage.cy
		)
		ptMouseTrace = ptCursor;
	if( m_ptMouseTrace == ptMouseTrace )
		return;
	if( ptMouseTrace.x < 0 )
		KillTimer( 0x433 );
	else if( m_ptMouseTrace.x < 0 )
		SetTimer( 0x433, 50, NULL );
	m_ptMouseTrace = ptMouseTrace;
CExtIconEditDlg * pIconEditDlg = ((CExtIconEditDlg *)GetParent());
	if( pIconEditDlg->GetSafeHwnd() == NULL )
		return;
	pIconEditDlg->OnUpdateInfoWindows();
}

BOOL CExtIconEditDlg::CSurfaceEditor::PreTranslateMessage(MSG* pMsg)
{
BOOL bRetVal = CExtImageEditWnd::PreTranslateMessage( pMsg );
	if( pMsg->message == WM_KEYDOWN && (!bRetVal) )
	{
		bool bCopy = false, bCut = false, bPaste = false;
		INT nUndoSteps = 0;
		INT nMaginficationOffset = 0;
		UINT nMaginficationExact = 0;
		switch( int(pMsg->wParam) )
		{
		case VK_RETURN:
			if(		CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL)
				||	CExtPopupMenuWnd::IsKeyPressed(VK_SHIFT)
				)
				break;
			GetParent()->PostMessage(WM_COMMAND,IDOK);
			return TRUE;
		case VK_ESCAPE:
			if( IedToolGet() != __IEDT_RECT_SELECTION )
				IedToolSet( __IEDT_RECT_SELECTION );
			else
				GetParent()->PostMessage(WM_COMMAND,IDCANCEL);
			return TRUE;
		case VK_DELETE:
			if( CExtPopupMenuWnd::IsKeyPressed(VK_SHIFT) )
			{
				bCut = true;
				break;
			}
			GetParent()->PostMessage(WM_COMMAND,ID_IED_CLEAR);
			return TRUE;
		case VK_INSERT:
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				bCopy = true;
			else if( CExtPopupMenuWnd::IsKeyPressed(VK_SHIFT) )
				bPaste = true;
			break;
		case VK_BACK:
			nUndoSteps = 1;
			break;
		case VK_ADD:
			nMaginficationOffset = 1;
			break;
		case VK_SUBTRACT:
			nMaginficationOffset = -1;
			break;
		case _T('S'):
			GetParent()->PostMessage(WM_COMMAND,ID_IED_TOOL_RECT_SELECTION);
			return TRUE;
		case _T('Z'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				nUndoSteps = 1;
			break;
		case _T('Y'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				nUndoSteps = -1;
			break;
		case _T('X'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				bCut = true;
			break;
		case _T('V'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				bPaste = true;
			break;
		case _T('G'):
			GetParent()->PostMessage(WM_COMMAND,ID_IED_TOGGLE_GRID);
			return TRUE;
		case _T('M'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				_TrackCommandMenu( ID_IED_TOOL_MAGNIFICATION );
			else
				GetParent()->PostMessage(WM_COMMAND,ID_IED_TOOL_MAGNIFICATION);
			return TRUE;
		case _T('C'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
			{
				bCopy = true;
				break;
			}
			GetParent()->PostMessage(WM_COMMAND,ID_IED_TOOL_COLOR_SELECTION);
			return TRUE;
		case _T('A'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				GetParent()->PostMessage(WM_COMMAND,ID_EDIT_SELECT_ALL);
			return TRUE;
		case _T('I'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				_TrackCommandMenu( ID_IED_TOOL_AIR_BRUSH );
			else
				GetParent()->PostMessage(WM_COMMAND,ID_IED_TOOL_AIR_BRUSH);
			return TRUE;
		case _T('F'):
			GetParent()->PostMessage(WM_COMMAND,ID_IED_TOOL_FILL);
			return TRUE;
		case _T('P'):
			GetParent()->PostMessage(WM_COMMAND,ID_IED_TOOL_PENCIL);
			return TRUE;
		case _T('B'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				_TrackCommandMenu( ID_IED_TOOL_BRUSH );
			else
				GetParent()->PostMessage(WM_COMMAND,ID_IED_TOOL_BRUSH);
			return TRUE;
		case _T('L'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				_TrackCommandMenu( ID_IED_TOOL_LINE );
			else
				GetParent()->PostMessage(WM_COMMAND,ID_IED_TOOL_LINE);
			return TRUE;
		case _T('R'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				GetParent()->PostMessage(WM_COMMAND,ID_IED_TOOL_FILLED_RECTANGLE);
			else if( CExtPopupMenuWnd::IsKeyPressed(VK_SHIFT) )
				GetParent()->PostMessage(WM_COMMAND,ID_IED_TOOL_OUTLINED_RECTANGLE);
			else
				GetParent()->PostMessage(WM_COMMAND,ID_IED_TOOL_RECTANGLE);
			return TRUE;
		case _T('O'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				GetParent()->PostMessage(WM_COMMAND,ID_IED_TOOL_ROUNDED_FILLED_RECTANGLE);
			else if( CExtPopupMenuWnd::IsKeyPressed(VK_SHIFT) )
				GetParent()->PostMessage(WM_COMMAND,ID_IED_TOOL_ROUNDED_OUTLINED_RECTANGLE);
			else
				GetParent()->PostMessage(WM_COMMAND,ID_IED_TOOL_ROUNDED_RECTANGLE);
			return TRUE;
		case _T('E'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				GetParent()->PostMessage(WM_COMMAND,ID_IED_TOOL_FILLED_ELLIPSE);
			else if( CExtPopupMenuWnd::IsKeyPressed(VK_SHIFT) )
				GetParent()->PostMessage(WM_COMMAND,ID_IED_TOOL_OUTLINED_ELLIPSE);
			else
				GetParent()->PostMessage(WM_COMMAND,ID_IED_TOOL_ELLIPSE);
			return TRUE;
		case _T('1'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				nMaginficationExact = 1;
			break;
		case _T('2'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				nMaginficationExact = 2;
			break;
		case _T('3'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				nMaginficationExact = 3;
			break;
		case _T('4'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				nMaginficationExact = 4;
			break;
		case _T('5'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				nMaginficationExact = 5;
			break;
		case _T('6'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				nMaginficationExact = 6;
			break;
		case _T('7'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				nMaginficationExact = 7;
			break;
		case _T('8'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				nMaginficationExact = 8;
			break;
		case _T('9'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				nMaginficationExact = 9;
			break;
		case _T('0'):
			if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
				nMaginficationExact = 10;
			break;
		} // switch( int(pMsg->wParam) )
		if( bCopy && IedCanCopy() )
		{
			GetParent()->PostMessage(WM_COMMAND,ID_IED_EDIT_COPY);
			return TRUE;
		} // if( bCopy && IedCanCopy() )
		if( bCut && IedCanCut() )
		{
			GetParent()->PostMessage(WM_COMMAND,ID_IED_EDIT_CUT);
			return TRUE;
		} // if( bCut && IedCanCut() )
		if( bPaste && IedCanPaste() )
		{
			GetParent()->PostMessage(WM_COMMAND,ID_IED_EDIT_PASTE);
			return TRUE;
		} // if( bPaste && IedCanPaste() )
		if( nUndoSteps < 0 )
		{
			GetParent()->PostMessage(WM_COMMAND,ID_IED_EDIT_REDO);
			return TRUE;
		} // if( nUndoSteps < 0 )
		if( nUndoSteps > 0 )
		{
			GetParent()->PostMessage(WM_COMMAND,ID_IED_EDIT_UNDO);
			return TRUE;
		} // if( nUndoSteps > 0 )
		if( nMaginficationOffset != 0 )
		{
			INT nMaginficationOld = (INT)IedMagnificationGet();
			INT nMaginficationNew =
				nMaginficationOld + nMaginficationOffset;
			if( nMaginficationNew <= 0 || nMaginficationNew >= 16 )
				return TRUE;
			IedMagnificationSet( (UINT)nMaginficationNew, true );
			return TRUE;
		} // if( nMaginficationOffset != 0 )
		if( nMaginficationExact != 0 )
		{
			UINT nMaginficationOld = IedMagnificationGet();
			if( nMaginficationExact != nMaginficationOld )
				IedMagnificationSet( nMaginficationExact, true );
		} // if( nMaginficationExact != 0 )
	} // if( pMsg->message == WM_KEYDOWN && (!bRetVal) )
	return bRetVal;
}

/////////////////////////////////////////////////////////////////////////////
// CExtIconEditDlg::CSurfaceViewer

CExtIconEditDlg::CSurfaceViewer::CSurfaceViewer()
{
}

CExtIconEditDlg::CSurfaceViewer::~CSurfaceViewer()
{
}

LRESULT CExtIconEditDlg::CSurfaceViewer::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID( this );
	return CExtImageEditWnd::WindowProc(message,wParam,lParam);
}

/////////////////////////////////////////////////////////////////////////////
// CExtIconEditDlg::CPaletteViewer

CExtIconEditDlg::CPaletteViewer::CPaletteViewer()
	: m_hBrushTransparentBk( NULL )
	, m_clrTransparentOption( ((COLORREF)(-1L)) )
	, m_bSyncingColorForeground( false )
	, m_bSyncingColorBackground( false )
{
}

CExtIconEditDlg::CPaletteViewer::~CPaletteViewer()
{
}

bool CExtIconEditDlg::CPaletteViewer::OnCpwActiveColorChange(
	bool bForeground,
	COLORREF clrOld,
	COLORREF clrNew,
	bool bRepaint
	)
{
	ASSERT_VALID( this );
	if( bForeground && m_bSyncingColorForeground )
		return false;
	if( (!bForeground) && m_bSyncingColorBackground )
		return false;
	if(	!CExtColorPaletteWnd::OnCpwActiveColorChange(
			bForeground,
			clrOld,
			clrNew,
			bRepaint
			)
		)
		return false;
CExtIconEditDlg * pIconEditDlg = ((CExtIconEditDlg *)GetParent());
	if( pIconEditDlg->GetSafeHwnd() == NULL )
		return false;
CExtImageEditWnd * pImageEditWnd = &pIconEditDlg->m_wndIconEditor;
	ASSERT_VALID( pImageEditWnd );
	if( bForeground )
	{
		m_bSyncingColorForeground = true;
		pImageEditWnd->IedColorForegroundSet( clrNew );
		m_bSyncingColorForeground = false;
	} // if( bForeground )
	else
	{
		m_bSyncingColorBackground = true;
		pImageEditWnd->IedColorBackgroundSet( clrNew );
		m_bSyncingColorBackground = false;
	} // else from if( bForeground )
	return true;
}

CExtSafeString CExtIconEditDlg::CPaletteViewer::OnCpwGetTooltipText(
	INT nItem,
	CPoint ptClient,
	const CRect & rcItem,
	COLORREF clr
	) const
{
	ASSERT_VALID( this );
	nItem;
	ptClient;
	rcItem;
CExtSafeString strTooltipText;
	if( clr == m_clrTransparentOption )
	{
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		CExtLocalResourceHelper _LRH;
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		if( ! g_ResourceManager->LoadString( strTooltipText, IDS_IED_TRANSPARENT_CELL_NAME ) )
		{
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
			ASSERT( FALSE );
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
			strTooltipText = _T("Transparent");
		}
	} // if( clr == m_clrTransparentOption )
	else
		strTooltipText.Format(
			_T("#%02X%02X%02X"),
			(INT)GetRValue(clr),
			(INT)GetGValue(clr),
			(INT)GetBValue(clr)
			);
	return strTooltipText;
//	return
//		CExtColorPaletteWnd::OnCpwGetTooltipText(
//			nItem,
//			ptClient,
//			rcItem,
//			clr
//			);
}

void CExtIconEditDlg::CPaletteViewer::OnCpwPaintItem(
	CDC & dc,
	INT nItemID,
	const CRect & rcItem,
	COLORREF clrItem,
	COLORREF clrBorder
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( !rcItem.IsRectEmpty() );
	ASSERT(
			nItemID == __ECPW_CELL_INDEX_FOREGROUND
		||	nItemID == __ECPW_CELL_INDEX_BACKGROUND
		||	(	0 <= nItemID
			&&	nItemID < CpwColorArrayGetCount()
			)
		);
	nItemID;
	if(		m_hBrushTransparentBk == NULL
		||	clrItem != m_clrTransparentOption
		)
	{
		CExtColorPaletteWnd::OnCpwPaintItem(
			dc,
			nItemID,
			rcItem,
			clrItem,
			clrBorder
			);
		return;
	}
	::FillRect( dc, &rcItem, m_hBrushTransparentBk );
	dc.Draw3dRect( &rcItem, clrBorder, clrBorder );
}

/////////////////////////////////////////////////////////////////////////////
// CExtIconEditDlg::CInfoWnd

CExtIconEditDlg::CInfoWnd::CInfoWnd(
	UINT nIdResource, // = 0
	INT cx, // = 16
	INT cy // = 16
	)
{

static __EXT_MFC_SAFE_LPCTSTR strClassName = _T("ProfUIS-IconEditDlg-InfoWnd");
static bool g_bClassRegistered = false;
	if( !g_bClassRegistered )
	{
		WNDCLASS wndclass;
		HINSTANCE hInst = AfxGetInstanceHandle();
		if( ! ::GetClassInfo(
				hInst,
				strClassName,
				&wndclass
				)
			)
		{
			// otherwise we need to register a new class
			wndclass.style = CS_GLOBALCLASS;
			wndclass.lpfnWndProc = ::DefWindowProc;
			wndclass.cbClsExtra = wndclass.cbWndExtra = 0;
			wndclass.hInstance = hInst;
			wndclass.hIcon = NULL;
			wndclass.hCursor = ::LoadCursor( NULL, IDC_ARROW );
			ASSERT( wndclass.hCursor != NULL );
			wndclass.hbrBackground = NULL; 
			wndclass.lpszMenuName = NULL;
			wndclass.lpszClassName = strClassName;
			if( !::AfxRegisterClass( &wndclass ) )
			{
				ASSERT( FALSE );
				AfxThrowResourceException();
			}
		}
		g_bClassRegistered = true;
	} // if( !g_bClassRegistered )


	if(		nIdResource != 0
		&&	cx > 0
		&&	cy > 0
		)
	{ // if init icon in constructor
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		CExtLocalResourceHelper _LRH;
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		HINSTANCE hInstResource =
			::AfxFindResourceHandle(
				MAKEINTRESOURCE( nIdResource ),
				RT_ICON
				);
		ASSERT( hInstResource != NULL );
		if( hInstResource != NULL )
		{
			HICON hIcon = (HICON)
				::LoadImage(
					hInstResource,
					MAKEINTRESOURCE( nIdResource ),
					IMAGE_ICON,
					cx,
					cy,
					0
					);
			ASSERT( hIcon != NULL );
			if( hIcon != NULL )
				m_icon.AssignFromHICON( hIcon, false );
		} // if( hInstResource != NULL )
	} // if init icon in constructor
}

CExtIconEditDlg::CInfoWnd::~CInfoWnd()
{
	if( !m_icon.IsEmpty() )
		m_icon.Empty();
}

LRESULT CExtIconEditDlg::CInfoWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_ERASEBKGND )
		return 1;
	if( message == WM_PAINT )
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		CPaintDC dcPaint( this );
		CExtMemoryDC dc( &dcPaint, &rcClient );
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
		CRect rcText( rcClient );
		if( !m_icon.IsEmpty() )
		{
			CSize _sizeIcon = m_icon.GetSize();
			CPoint ptIcon = rcClient.TopLeft();
			ptIcon.y += (rcClient.Height() - _sizeIcon.cy) / 2;
			m_icon.Paint( dc, ptIcon );
			rcText.left += _sizeIcon.cx + 3;
		} // if( !m_icon.IsEmpty() )
		if( rcText.left < rcText.right )
		{
			CString strWindowText;
			GetWindowText( strWindowText );
			if( !strWindowText.IsEmpty() )
			{
				int nOldBkMode = dc.SetBkMode( TRANSPARENT );
				COLORREF clrOldText =
					dc.SetTextColor(
						g_PaintManager->GetColor( COLOR_BTNTEXT )
						);
				CFont * pOldFont = dc.SelectObject( &g_PaintManager->m_FontNormal );
				dc.DrawText(
					strWindowText,
					&rcText,
					DT_SINGLELINE|DT_LEFT|DT_VCENTER
					);
				dc.SelectObject( pOldFont );
				dc.SetTextColor( clrOldText );
				dc.SetBkMode( nOldBkMode );
			} // if( !strWindowText.IsEmpty() )
		} // if( rcText.left < rcText.right )
		return 0;
	} // if( message == WM_PAINT )
	return CWnd::WindowProc(message,wParam,lParam);
}

void CExtIconEditDlg::CInfoWnd::PreSubclassWindow()
{
	CWnd::PreSubclassWindow();
	if( m_strInitialText.IsEmpty() )
	{
		int nTextLen = GetWindowTextLength();
		if( nTextLen > 0 )
		{
			GetWindowText( m_strInitialText.GetBuffer( nTextLen+2 ), nTextLen+1 );
			m_strInitialText.ReleaseBuffer();
		} // if( nTextLen > 0 )
	}
}

/////////////////////////////////////////////////////////////////////////////
// CExtIconEditDlg dialog

CExtIconEditDlg::CExtIconEditDlg(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(IDD_EXT_ICON_EDIT_DIALOG, pParent)
	, m_eLastToolBrush( CExtImageEditWnd::__IEDT_BRUSH_CIRCLE_LARGE )
	, m_eLastToolAirBrush( CExtImageEditWnd::__IEDT_AIRBRUSH_MEDIUM )
	, m_eLastToolLine( CExtImageEditWnd::__IEDT_LINE_CIRCLE_SMALL )
	, m_eLastToolRectangle( CExtImageEditWnd::__IEDT_RECT_SMALLER )
	, m_eLastToolOutlinedRectangle( CExtImageEditWnd::__IEDT_RECT_OUTLINED_SMALLER )
	, m_eLastToolRoundedRectangle( CExtImageEditWnd::__IEDT_ROUNDED_RECT_SMALLER )
	, m_eLastToolOutlinedRoundedRectangle( CExtImageEditWnd::__IEDT_ROUNDED_RECT_OUTLINED_SMALLER )
	, m_eLastToolEllipse( CExtImageEditWnd::__IEDT_ELLIPSE_SMALLER )
	, m_eLastToolOutlinedEllipse( CExtImageEditWnd::__IEDT_ELLIPSE_OUTLINED_SMALLER )
	, m_bEatTransparentColor( true )
	, m_dwIconEditorStyles(
		__EIEDWS_CENTER
			|__EIEDWS_REQUEST_FOCUS
			|__EIEDWS_OUTER_AREA|__EIEDWS_OUTER_TOOLS
			|__EIEDWS_GRID_LINES|__EIEDWS_SOLID_GRID
		)
	, m_dwIconViewerStyles(
		__EIEDWS_CENTER
			|__EIEDWS_READONLY
			|__EIEDWS_NO_MAGNIFY
			//|__EIEDWS_OUTER_AREA|__EIEDWS_OUTER_TOOLS|__EIEDWS_BUTTON_FRAME
		)
	, m_dwColorPaletteStyles(
		__ECPWS_SHOW_ACTIVE_COLOR_AREA
			|__ECPWS_SHOW_TWO_COLORS
			|__ECPWS_UNIQUE_COLORS
			|__ECPWS_HCENTER
			|__ECPWS_TOOLTIPS
		)
	, m_wndInfoSelectionSize(
		IDD_EXT_ICON_SELECTION_SIZE,
		16,
		16
		)
	, m_wndInfoMousePos(
		IDD_EXT_ICON_MOUSE_POS,
		16,
		16
		)
{
	//{{AFX_DATA_INIT(CExtIconEditDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_hIcon = AfxGetApp()->LoadIcon(IDD_EXT_ICON_EDIT_DIALOG);
	ASSERT( m_hIcon != NULL );
}

CExtIconEditDlg::~CExtIconEditDlg()
{
	ASSERT( m_hIcon != NULL );
	if( m_hIcon != NULL )
		::DestroyIcon( m_hIcon );
}

void CExtIconEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CExtResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtIconEditDlg)
	DDX_Control(pDX, IDCANCEL, m_BtnCancel);
	DDX_Control(pDX, IDOK, m_BtnOK);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_IMAGE_EDITOR, m_wndIconEditor);
	DDX_Control(pDX, IDC_IMAGE_VIEWER, m_wndIconViewer);
	DDX_Control(pDX, IDC_IMAGE_PALETTE, m_wndColorPalette);
	DDX_Control(pDX, IDC_TOOLBAR_ICON_TOOLS, m_wndIconTools);
	DDX_Control(pDX, IDC_STATIC_SELECTION_SIZE, m_wndInfoSelectionSize);
	DDX_Control(pDX, IDC_STATIC_MOUSE_POS, m_wndInfoMousePos);
}

BEGIN_MESSAGE_MAP(CExtIconEditDlg, CExtResizableDialog)
	//{{AFX_MSG_MAP(CExtIconEditDlg)
	ON_WM_SIZE()
	ON_COMMAND(ID_IED_TOGGLE_GRID, OnIedToggleGrid)
	ON_UPDATE_COMMAND_UI(ID_IED_TOGGLE_GRID, OnUpdateIedToggleGrid)
	ON_COMMAND(ID_IED_CLEAR, OnIedClear)
	//}}AFX_MSG_MAP

	ON_COMMAND(ID_EDIT_SELECT_ALL, OnIedEditSelectAll)

	ON_COMMAND(ID_IED_EDIT_UNDO, OnIedEditUndo)
	ON_UPDATE_COMMAND_UI(ID_IED_EDIT_UNDO, OnUpdateIedEditUndo)
	ON_COMMAND(ID_IED_EDIT_REDO, OnIedEditRedo)
	ON_UPDATE_COMMAND_UI(ID_IED_EDIT_REDO, OnUpdateIedEditRedo)

	ON_COMMAND(ID_IED_EDIT_COPY, OnIedEditCopy)
	ON_UPDATE_COMMAND_UI(ID_IED_EDIT_COPY, OnUpdateIedEditCopy)
	ON_COMMAND(ID_IED_EDIT_CUT, OnIedEditCut)
	ON_UPDATE_COMMAND_UI(ID_IED_EDIT_CUT, OnUpdateIedEditCut)
	ON_COMMAND(ID_IED_EDIT_PASTE, OnIedEditPaste)
	ON_UPDATE_COMMAND_UI(ID_IED_EDIT_PASTE, OnUpdateIedEditPaste)

	ON_COMMAND(ID_IED_TOOL_RECT_SELECTION, OnIedToolRectSelection)
	ON_UPDATE_COMMAND_UI(ID_IED_TOOL_RECT_SELECTION, OnUpdateIedToolRectSelection)

	ON_COMMAND(ID_IED_TOOL_COLOR_SELECTION, OnIedToolColorSelection)
	ON_UPDATE_COMMAND_UI(ID_IED_TOOL_COLOR_SELECTION, OnUpdateIedToolColorSelection)

	ON_COMMAND(ID_IED_TOOL_FILL, OnIedToolFill)
	ON_UPDATE_COMMAND_UI(ID_IED_TOOL_FILL, OnUpdateIedToolFill)

	ON_COMMAND(ID_IED_TOOL_MAGNIFICATION, OnIedToolMagnification)
	ON_UPDATE_COMMAND_UI(ID_IED_TOOL_MAGNIFICATION, OnUpdateIedToolMagnification)
	ON_COMMAND_RANGE( ID_IED_TOOL_MAGNIFICATION_01X, ID_IED_TOOL_MAGNIFICATION_16X, OnIedCmdRangeMagnification )
	ON_UPDATE_COMMAND_UI_RANGE( ID_IED_TOOL_MAGNIFICATION_01X, ID_IED_TOOL_MAGNIFICATION_16X, OnUpdateIedRangeMagnification )
	
	ON_COMMAND(ID_IED_TOOL_PENCIL, OnIedToolPencil)
	ON_UPDATE_COMMAND_UI(ID_IED_TOOL_PENCIL, OnUpdateIedToolPencil)

	ON_COMMAND(ID_IED_TOOL_BRUSH, OnIedToolBrush)
	ON_UPDATE_COMMAND_UI(ID_IED_TOOL_BRUSH, OnUpdateIedToolBrush)
	ON_COMMAND_RANGE( ID_IED_TOOL_BRUSH_CIRCLE_SMALL, ID_IED_TOOL_BRUSH_LINE_RD_LARGE, OnIedCmdRangeBrush )
	ON_UPDATE_COMMAND_UI_RANGE( ID_IED_TOOL_BRUSH_CIRCLE_SMALL, ID_IED_TOOL_BRUSH_LINE_RD_LARGE, OnUpdateIedToolBrushRange )

	ON_COMMAND(ID_IED_TOOL_AIR_BRUSH, OnIedToolAirBrush)
	ON_UPDATE_COMMAND_UI(ID_IED_TOOL_AIR_BRUSH, OnUpdateIedToolAirBrush)
	ON_COMMAND_RANGE( ID_IED_TOOL_AIR_BRUSH_SMALL, ID_IED_TOOL_AIR_BRUSH_LARGE, OnIedCmdRangeAirBrush )
	ON_UPDATE_COMMAND_UI_RANGE( ID_IED_TOOL_AIR_BRUSH_SMALL, ID_IED_TOOL_AIR_BRUSH_LARGE, OnUpdateIedToolAirBrushRange )

	ON_COMMAND(ID_IED_TOOL_LINE, OnIedToolLine)
	ON_UPDATE_COMMAND_UI(ID_IED_TOOL_LINE, OnUpdateIedToolLine)
	ON_COMMAND_RANGE( ID_IED_TOOL_LINE_CIRCLE_SMALL, ID_IED_TOOL_LINE_LINE_RD_LARGE, OnIedCmdRangeLine )
	ON_UPDATE_COMMAND_UI_RANGE( ID_IED_TOOL_LINE_CIRCLE_SMALL, ID_IED_TOOL_LINE_LINE_RD_LARGE, OnUpdateIedToolLineRange )

	ON_COMMAND(ID_IED_TOOL_RECTANGLE, OnIedToolRectangle)
	ON_UPDATE_COMMAND_UI(ID_IED_TOOL_RECTANGLE, OnUpdateIedToolRectangle)
	ON_COMMAND_RANGE( ID_IED_TOOL_RECTANGLE_SMALLER, ID_IED_TOOL_RECTANGLE_LARGER, OnIedCmdRangeRectangle )
	ON_UPDATE_COMMAND_UI_RANGE( ID_IED_TOOL_RECTANGLE_SMALLER, ID_IED_TOOL_RECTANGLE_LARGER, OnUpdateIedToolRectangleRange )

	ON_COMMAND(ID_IED_TOOL_OUTLINED_RECTANGLE, OnIedToolOutlinedRectangle)
	ON_UPDATE_COMMAND_UI(ID_IED_TOOL_OUTLINED_RECTANGLE, OnUpdateIedToolOutlinedRectangle)
	ON_COMMAND_RANGE( ID_IED_TOOL_OUTLINED_RECTANGLE_SMALLER, ID_IED_TOOL_OUTLINED_RECTANGLE_LARGER, OnIedCmdRangeOutlinedRectangle )
	ON_UPDATE_COMMAND_UI_RANGE( ID_IED_TOOL_OUTLINED_RECTANGLE_SMALLER, ID_IED_TOOL_OUTLINED_RECTANGLE_LARGER, OnUpdateIedToolOutlinedRectangleRange )

	ON_COMMAND(ID_IED_TOOL_FILLED_RECTANGLE, OnIedToolFilledRectangle)
	ON_UPDATE_COMMAND_UI(ID_IED_TOOL_FILLED_RECTANGLE, OnUpdateIedToolFilledRectangle)

	ON_COMMAND(ID_IED_TOOL_ROUNDED_RECTANGLE, OnIedToolRoundedRectangle)
	ON_UPDATE_COMMAND_UI(ID_IED_TOOL_ROUNDED_RECTANGLE, OnUpdateIedToolRoundedRectangle)
	ON_COMMAND_RANGE( ID_IED_TOOL_ROUNDED_RECTANGLE_SMALLER, ID_IED_TOOL_ROUNDED_RECTANGLE_LARGER, OnIedCmdRangeRoundedRectangle )
	ON_UPDATE_COMMAND_UI_RANGE( ID_IED_TOOL_ROUNDED_RECTANGLE_SMALLER, ID_IED_TOOL_ROUNDED_RECTANGLE_LARGER, OnUpdateIedToolRoundedRectangleRange )

	ON_COMMAND(ID_IED_TOOL_ROUNDED_OUTLINED_RECTANGLE, OnIedToolOutlinedRoundedRectangle)
	ON_UPDATE_COMMAND_UI(ID_IED_TOOL_ROUNDED_OUTLINED_RECTANGLE, OnUpdateIedToolOutlinedRoundedRectangle)
	ON_COMMAND_RANGE( ID_IED_TOOL_OUTLINED_ROUNDED_RECTANGLE_SMALLER, ID_IED_TOOL_OUTLINED_ROUNDED_RECTANGLE_LARGER, OnIedCmdRangeOutlinedRoundedRectangle )
	ON_UPDATE_COMMAND_UI_RANGE( ID_IED_TOOL_OUTLINED_ROUNDED_RECTANGLE_SMALLER, ID_IED_TOOL_OUTLINED_ROUNDED_RECTANGLE_LARGER, OnUpdateIedToolOutlinedRoundedRectangleRange )

	ON_COMMAND(ID_IED_TOOL_ROUNDED_FILLED_RECTANGLE, OnIedToolFilledRoundedRectangle)
	ON_UPDATE_COMMAND_UI(ID_IED_TOOL_ROUNDED_FILLED_RECTANGLE, OnUpdateIedToolFilledRoundedRectangle)

	ON_COMMAND(ID_IED_TOOL_ELLIPSE, OnIedToolEllipse)
	ON_UPDATE_COMMAND_UI(ID_IED_TOOL_ELLIPSE, OnUpdateIedToolEllipse)
	ON_COMMAND_RANGE( ID_IED_TOOL_ELLIPSE_SMALLER, ID_IED_TOOL_ELLIPSE_LARGER, OnIedCmdRangeEllipse )
	ON_UPDATE_COMMAND_UI_RANGE( ID_IED_TOOL_ELLIPSE_SMALLER, ID_IED_TOOL_ELLIPSE_LARGER, OnUpdateIedToolEllipseRange )

	ON_COMMAND(ID_IED_TOOL_OUTLINED_ELLIPSE, OnIedToolOutlinedEllipse)
	ON_UPDATE_COMMAND_UI(ID_IED_TOOL_OUTLINED_ELLIPSE, OnUpdateIedToolOutlinedEllipse)
	ON_COMMAND_RANGE( ID_IED_TOOL_OUTLINED_ELLIPSE_SMALLER, ID_IED_TOOL_OUTLINED_ELLIPSE_LARGER, OnIedCmdRangeOutlinedEllipse )
	ON_UPDATE_COMMAND_UI_RANGE( ID_IED_TOOL_OUTLINED_ELLIPSE_SMALLER, ID_IED_TOOL_OUTLINED_ELLIPSE_LARGER, OnUpdateIedToolOutlinedEllipseRange )

	ON_COMMAND(ID_IED_TOOL_FILLED_ELLIPSE, OnIedToolFilledEllipse)
	ON_UPDATE_COMMAND_UI(ID_IED_TOOL_FILLED_ELLIPSE, OnUpdateIedToolFilledEllipse)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtIconEditDlg message handlers

BOOL CExtIconEditDlg::OnInitDialog() 
{
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
CExtLocalResourceHelper _LRH;
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
	if(		(!	CExtResizableDialog::OnInitDialog() )
		||	(! 	g_CmdManager->ProfileSetup(
					__EXT_ICONEDITDLG_PROFILE_NAME,
					m_hWnd
					)
				)
		||	(!	g_CmdManager->UpdateFromToolBar(
					__EXT_ICONEDITDLG_PROFILE_NAME,
					IDR_EXT_TOOLBAR_IED_TOOLS
					)
				)
		||	(!	g_CmdManager->UpdateFromToolBar(
					__EXT_ICONEDITDLG_PROFILE_NAME,
					IDR_EXT_TOOLBAR_IED_HELPER
					)
				)
		||	(!	g_CmdManager->UpdateFromMenu(
					__EXT_ICONEDITDLG_PROFILE_NAME,
					IDR_IED_MENU_AIR_BRUSH
					)
				)
		||	(!	g_CmdManager->UpdateFromMenu(
					__EXT_ICONEDITDLG_PROFILE_NAME,
					IDR_IED_MENU_BRUSH
					)
				)
		||	(!	g_CmdManager->UpdateFromMenu(
					__EXT_ICONEDITDLG_PROFILE_NAME,
					IDR_IED_MENU_ELLIPSE
					)
				)
		||	(!	g_CmdManager->UpdateFromMenu(
					__EXT_ICONEDITDLG_PROFILE_NAME,
					IDR_IED_MENU_LINE
					)
				)
		||	(!	g_CmdManager->UpdateFromMenu(
					__EXT_ICONEDITDLG_PROFILE_NAME,
					IDR_IED_MENU_MAGNIFICATION
					)
				)
		||	(!	g_CmdManager->UpdateFromMenu(
					__EXT_ICONEDITDLG_PROFILE_NAME,
					IDR_IED_MENU_OUTLINED_ELLIPSE
					)
				)
		||	(!	g_CmdManager->UpdateFromMenu(
					__EXT_ICONEDITDLG_PROFILE_NAME,
					IDR_IED_MENU_OUTLINED_RECTANGLE
					)
				)
		||	(!	g_CmdManager->UpdateFromMenu(
					__EXT_ICONEDITDLG_PROFILE_NAME,
					IDR_IED_MENU_RECTANGLE
					)
				)
		||	(!	g_CmdManager->UpdateFromMenu(
					__EXT_ICONEDITDLG_PROFILE_NAME,
					IDR_IED_MENU_ROUNDED_OUTLINED_RECTANGLE
					)
				)
		||	(!	g_CmdManager->UpdateFromMenu(
					__EXT_ICONEDITDLG_PROFILE_NAME,
					IDR_IED_MENU_ROUNDED_RECTANGLE
					)
				)
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}

	ASSERT( m_hIcon != NULL );
	SetIcon( m_hIcon, TRUE );
	SetIcon( m_hIcon, FALSE );

CExtCmdProfile * pProfile =
		g_CmdManager->ProfileGetPtr( __EXT_ICONEDITDLG_PROFILE_NAME );
	ASSERT( pProfile != NULL );
POSITION pos = pProfile->m_cmds.GetStartPosition();
	ASSERT( pos != NULL );
	while( pos != NULL )
	{
		UINT nCmdID = 0;
		CExtCmdItem * pCmd = NULL;
		pProfile->m_cmds.GetNextAssoc( pos, nCmdID, pCmd );
		ASSERT( pCmd != NULL );
		pCmd->StateSetBasic();
	} // while( pos != NULL )

	if( m_brushTransparentBk.GetSafeHandle() == NULL )
	{
		if( m_bmpTransparentBk.GetSafeHandle() == NULL )
		{
			HDC hDC = ::CreateCompatibleDC( NULL );
			if( hDC == NULL )
			{
				ASSERT( FALSE );
				return FALSE;
			}
			
			static const SIZE _sizeBmp = { 8, 8 };
			static const LONG _nBmpPixelCount = _sizeBmp.cx * _sizeBmp.cy;
			BITMAPINFOHEADER bih;
			::memset( &bih, 0, sizeof(BITMAPINFOHEADER) );
			bih.biSize = sizeof(BITMAPINFOHEADER);
			bih.biWidth = _sizeBmp.cx;
			bih.biHeight = _sizeBmp.cy;
			bih.biPlanes = 1;
			bih.biBitCount = 32;
			bih.biCompression = BI_RGB;
			bih.biSizeImage = _nBmpPixelCount;
			COLORREF * pClrSurface = NULL;
			HBITMAP hDIB =
				::CreateDIBSection(
					hDC,
					(LPBITMAPINFO)&bih,
					DIB_RGB_COLORS,
					(void **)&pClrSurface,
					NULL,
					NULL
					);
			if( hDIB == NULL )
			{
				ASSERT( FALSE );
				::DeleteDC( hDC );
				return FALSE;
			}
			ASSERT( pClrSurface != NULL );
			::DeleteDC( hDC );

			for( LONG nPixel = 0L; nPixel < _nBmpPixelCount; nPixel++ )
				pClrSurface[nPixel] = RGB(255,255,255);

			static const POINT arrPtHatch[] =
			{
				{ 0, 0 },	{ 1, 0 },	{ 2, 0 },
				{ 0, 1 },	{ 1, 1 },	{ 7, 1 },
				{ 0, 2 },	{ 6, 2 },	{ 7, 2 },
				{ 5, 3 },	{ 6, 3 },	{ 7, 3 },
				{ 4, 4 },	{ 5, 4 },	{ 6, 4 },
				{ 3, 5 },	{ 4, 5 },	{ 5, 5 },
				{ 2, 6 },	{ 3, 6 },	{ 4, 6 },
				{ 1, 7 },	{ 2, 7 },	{ 3, 7 },
			};
			for( nPixel = 0L; nPixel < sizeof(arrPtHatch)/sizeof(arrPtHatch[0]); nPixel++ )
			{
				ASSERT( 0 <= arrPtHatch[nPixel].x && arrPtHatch[nPixel].x < _sizeBmp.cx );
				ASSERT( 0 <= arrPtHatch[nPixel].y && arrPtHatch[nPixel].y < _sizeBmp.cy );
				LONG nOffset =
					( arrPtHatch[nPixel].y * _sizeBmp.cx )
					+ arrPtHatch[nPixel].x
					;
				ASSERT( 0 <= nOffset && nOffset < _nBmpPixelCount );
				pClrSurface[ _nBmpPixelCount - nOffset - 1 ] = RGB(192,192,192);
			}

			m_bmpTransparentBk.Attach( hDIB );
		} // if( m_bmpTransparentBk.GetSafeHandle() == NULL )
		if( !m_brushTransparentBk.CreatePatternBrush( &m_bmpTransparentBk ) )
		{
			if(	! m_brushTransparentBk.CreateHatchBrush(
					HS_FDIAGONAL,
					RGB(192,192,192)
					)
				)
			{
				ASSERT( FALSE );
				return FALSE;
			}
		}
	} // if( m_brushTransparentBk.GetSafeHandle() == NULL )
	ASSERT( m_brushTransparentBk.GetSafeHandle() != NULL );
	m_wndIconEditor.m_hBrushTransparentBk =
		(HBRUSH)m_brushTransparentBk.GetSafeHandle();
	m_wndColorPalette.m_hBrushTransparentBk =
		(HBRUSH)m_brushTransparentBk.GetSafeHandle();

	m_wndIconTools.m_bPaletteMode = true;
	if( !m_wndIconTools.LoadToolBar(IDR_EXT_TOOLBAR_IED_TOOLS) )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	m_wndIconTools.GetButton(3)->SetWrap( CExtBarButton::__EVT_FLOAT );
	m_wndIconTools.GetButton(8)->SetWrap( CExtBarButton::__EVT_FLOAT );
	m_wndIconTools.GetButton(12)->SetWrap( CExtBarButton::__EVT_FLOAT );
	m_wndIconTools.GetButton(15)->SetWrap( CExtBarButton::__EVT_FLOAT );
	m_wndIconTools.GetButton(18)->SetWrap( CExtBarButton::__EVT_FLOAT );
	m_wndIconTools.GetButton(22)->SetWrap( CExtBarButton::__EVT_FLOAT );
	m_wndIconTools.GetButton(27)->SetWrap( CExtBarButton::__EVT_FLOAT );

CMenu _menu;
CExtBarButton * pTBB;
int nBtnIdx;
	
	VERIFY( g_ResourceManager->LoadMenu( _menu, IDR_IED_MENU_MAGNIFICATION ) );
	nBtnIdx = m_wndIconTools.CommandToIndex(ID_IED_TOOL_MAGNIFICATION);
	m_wndIconTools.SetButtonMenu( nBtnIdx, _menu.Detach() );
	pTBB = m_wndIconTools.GetButton(nBtnIdx);
	ASSERT_VALID( pTBB );
	pTBB->SetNoRotateVerticalLayout();
	pTBB->SetSeparatedDropDown();

	VERIFY( g_ResourceManager->LoadMenu( _menu, IDR_IED_MENU_BRUSH ) );
	nBtnIdx = m_wndIconTools.CommandToIndex(ID_IED_TOOL_BRUSH);
	m_wndIconTools.SetButtonMenu( nBtnIdx, _menu.Detach() );
	pTBB = m_wndIconTools.GetButton(nBtnIdx);
	ASSERT_VALID( pTBB );
	pTBB->SetNoRotateVerticalLayout();
	pTBB->SetSeparatedDropDown();

	VERIFY( g_ResourceManager->LoadMenu( _menu, IDR_IED_MENU_AIR_BRUSH ) );
	nBtnIdx = m_wndIconTools.CommandToIndex(ID_IED_TOOL_AIR_BRUSH);
	m_wndIconTools.SetButtonMenu( nBtnIdx, _menu.Detach() );
	pTBB = m_wndIconTools.GetButton(nBtnIdx);
	ASSERT_VALID( pTBB );
	pTBB->SetNoRotateVerticalLayout();
	pTBB->SetSeparatedDropDown();

	VERIFY( g_ResourceManager->LoadMenu( _menu, IDR_IED_MENU_LINE ) );
	nBtnIdx = m_wndIconTools.CommandToIndex(ID_IED_TOOL_LINE);
	m_wndIconTools.SetButtonMenu( nBtnIdx, _menu.Detach() );
	pTBB = m_wndIconTools.GetButton(nBtnIdx);
	ASSERT_VALID( pTBB );
	pTBB->SetNoRotateVerticalLayout();
	pTBB->SetSeparatedDropDown();

	VERIFY( g_ResourceManager->LoadMenu( _menu, IDR_IED_MENU_RECTANGLE ) );
	nBtnIdx = m_wndIconTools.CommandToIndex(ID_IED_TOOL_RECTANGLE);
	m_wndIconTools.SetButtonMenu( nBtnIdx, _menu.Detach() );
	pTBB = m_wndIconTools.GetButton(nBtnIdx);
	ASSERT_VALID( pTBB );
	pTBB->SetNoRotateVerticalLayout();
	pTBB->SetSeparatedDropDown();

	VERIFY( g_ResourceManager->LoadMenu( _menu, IDR_IED_MENU_OUTLINED_RECTANGLE ) );
	nBtnIdx = m_wndIconTools.CommandToIndex(ID_IED_TOOL_OUTLINED_RECTANGLE);
	m_wndIconTools.SetButtonMenu( nBtnIdx, _menu.Detach() );
	pTBB = m_wndIconTools.GetButton(nBtnIdx);
	ASSERT_VALID( pTBB );
	pTBB->SetNoRotateVerticalLayout();
	pTBB->SetSeparatedDropDown();

	VERIFY( g_ResourceManager->LoadMenu( _menu, IDR_IED_MENU_ROUNDED_RECTANGLE ) );
	nBtnIdx = m_wndIconTools.CommandToIndex(ID_IED_TOOL_ROUNDED_RECTANGLE);
	m_wndIconTools.SetButtonMenu( nBtnIdx, _menu.Detach() );
	pTBB = m_wndIconTools.GetButton(nBtnIdx);
	ASSERT_VALID( pTBB );
	pTBB->SetNoRotateVerticalLayout();
	pTBB->SetSeparatedDropDown();

	VERIFY( g_ResourceManager->LoadMenu( _menu, IDR_IED_MENU_ROUNDED_OUTLINED_RECTANGLE ) );
	nBtnIdx = m_wndIconTools.CommandToIndex(ID_IED_TOOL_ROUNDED_OUTLINED_RECTANGLE);
	m_wndIconTools.SetButtonMenu( nBtnIdx, _menu.Detach() );
	pTBB = m_wndIconTools.GetButton(nBtnIdx);
	ASSERT_VALID( pTBB );
	pTBB->SetNoRotateVerticalLayout();
	pTBB->SetSeparatedDropDown();

	VERIFY( g_ResourceManager->LoadMenu( _menu, IDR_IED_MENU_ELLIPSE ) );
	nBtnIdx = m_wndIconTools.CommandToIndex(ID_IED_TOOL_ELLIPSE);
	m_wndIconTools.SetButtonMenu( nBtnIdx, _menu.Detach() );
	pTBB = m_wndIconTools.GetButton(nBtnIdx);
	ASSERT_VALID( pTBB );
	pTBB->SetNoRotateVerticalLayout();
	pTBB->SetSeparatedDropDown();

	VERIFY( g_ResourceManager->LoadMenu( _menu, IDR_IED_MENU_OUTLINED_ELLIPSE ) );
	nBtnIdx = m_wndIconTools.CommandToIndex(ID_IED_TOOL_OUTLINED_ELLIPSE);
	m_wndIconTools.SetButtonMenu( nBtnIdx, _menu.Detach() );
	pTBB = m_wndIconTools.GetButton(nBtnIdx);
	ASSERT_VALID( pTBB );
	pTBB->SetNoRotateVerticalLayout();
	pTBB->SetSeparatedDropDown();


CRect rcWndIconTools, rcWndIconEditor;
	m_wndIconTools.GetWindowRect( &rcWndIconTools );
	ScreenToClient( &rcWndIconTools );
	m_wndIconEditor.GetWindowRect( &rcWndIconEditor );
	ScreenToClient( &rcWndIconEditor );
ASSERT( rcWndIconTools.right < rcWndIconEditor.left );
int nDistBefore = rcWndIconEditor.left - rcWndIconTools.right;
	CWnd::RepositionBars(0,0xFFFF,0);
	m_wndIconTools.GetWindowRect( &rcWndIconTools );
	ScreenToClient( &rcWndIconTools );
int nDistAfter = rcWndIconEditor.left - rcWndIconTools.right;
	if( nDistBefore != nDistAfter )
	{
		int nShift = nDistBefore - nDistAfter;
		rcWndIconEditor.left += nShift;
		m_wndIconEditor.MoveWindow( &rcWndIconEditor );
	}

	ASSERT( !m_icon.IsEmpty() );
CArray <COLORREF,COLORREF> arrColors;
COLORREF clrTransparent = RGB( 0, 0, 0 );
HBITMAP hBitmap = NULL;
COLORREF * pClrSurface = NULL;
	if( !m_icon.ExtractEditableBitmap(
			hBitmap,
			clrTransparent,
			&arrColors,
			m_bEatTransparentColor ? NULL : &pClrSurface
			)
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}
	ASSERT( hBitmap != NULL );
#ifdef _DEBUG
	if( !m_bEatTransparentColor )
	{
		ASSERT( pClrSurface != NULL );
	}
#endif // _DEBUG
	ASSERT( arrColors.GetSize() >= 1 && arrColors[0] == clrTransparent );
BITMAP bi;
	::memset( &bi, 0, sizeof(BITMAP) );
	::GetObject( hBitmap, sizeof(BITMAP), &bi );
	ASSERT( bi.bmWidth > 0 && bi.bmHeight > 0 );
INT nMagnification = 8;
	if( ( bi.bmWidth > 16 && bi.bmHeight > 16 ) )
		nMagnification = 4;
	if( ( bi.bmWidth > 32 && bi.bmHeight > 32 ) )
		nMagnification = 2;
	if( ( bi.bmWidth > 64 && bi.bmHeight > 64 ) )
		nMagnification = 1;

	m_wndColorPalette.m_clrTransparentOption =
		clrTransparent;

	m_wndIconEditor.SetRedraw( FALSE );
	m_wndIconViewer.SetRedraw( FALSE );
	m_wndColorPalette.SetRedraw( FALSE );

	m_wndIconEditor.IedEnableTransparency( clrTransparent );
	m_wndIconViewer.IedEnableTransparency( clrTransparent );
	m_wndIconEditor.IedBitmapSet( hBitmap, false );
	m_wndIconEditor._SyncViewerSurface();
	m_wndIconEditor.IedMagnificationSet( nMagnification, false );
	
	m_wndColorPalette.CpwColorArraySet( arrColors );
	
	m_wndColorPalette.CpwModifyStyle(
		0xFFFFFFFF,
		m_dwColorPaletteStyles
		);
	m_wndIconEditor.IedModifyStyle(
		0xFFFFFFFF,
		m_dwIconEditorStyles,
		true
		);
	m_wndIconViewer.IedModifyStyle(
		0xFFFFFFFF,
		m_dwIconViewerStyles,
		true
		);
	CExtResizableDialog::AddAnchor( IDC_IMAGE_EDITOR, __RDA_LT, __RDA_RB );
	CExtResizableDialog::AddAnchor( IDC_IMAGE_VIEWER, __RDA_RB );
	CExtResizableDialog::AddAnchor( IDC_IMAGE_PALETTE, __RDA_RT, __RDA_RB );
	CExtResizableDialog::AddAnchor( IDOK, __RDA_RB );
	CExtResizableDialog::AddAnchor( IDCANCEL, __RDA_RB );
	CExtResizableDialog::AddAnchor( IDC_STATIC_SELECTION_SIZE, __RDA_LB );
	CExtResizableDialog::AddAnchor( IDC_STATIC_MOUSE_POS, __RDA_LB );
	
	CWnd::RepositionBars(0,0xFFFF,0);

	if( !m_strAlternateCaption.IsEmpty() )
		SetWindowText( m_strAlternateCaption );
	if(		( !m_strProfileSection.IsEmpty() )
		&&	( !m_strProfileEntryWindow.IsEmpty() )
		)
		CExtResizableDialog::EnableSaveRestore(
			m_strProfileSection,
			m_strProfileEntryWindow
			);

	m_wndIconEditor.IedEmptyUndoBuffer();
	m_wndIconViewer.IedEmptyUndoBuffer();

	m_wndIconEditor.SetRedraw( TRUE );
	m_wndIconViewer.SetRedraw( TRUE );
	m_wndColorPalette.SetRedraw( TRUE );
	m_wndIconEditor.Invalidate();
	m_wndIconViewer.Invalidate();
	m_wndColorPalette.Invalidate();
	OnUpdateInfoWindows();

	return TRUE;
}

void CExtIconEditDlg::OnUpdateInfoWindows()
{
	ASSERT_VALID( this );
	if( !m_wndInfoSelectionSize.m_strInitialText.IsEmpty() )
	{
		CSize _size = m_wndIconEditor.m_sizeSelTrace;
		if( _size.cx == 0 && _size.cy == 0 )
		{
			bool bSelectionExist = false;
			CRect rcSelection =
				m_wndIconEditor.IedSelectionRectGet(
					NULL,
					&bSelectionExist
					);
			_size = bSelectionExist
				? rcSelection.Size()
				: m_wndIconEditor.IedSizeGet()
				;
		} // if( _size.cx == 0 && _size.cy == 0 )
		CExtSafeString s;
		s.Format(
			(LPCTSTR)m_wndInfoSelectionSize.m_strInitialText,
			_size.cx,
			_size.cy
			);
		m_wndInfoSelectionSize.Invalidate();
		m_wndInfoSelectionSize.SetWindowText( s );
	} // if( !m_wndInfoSelectionSize.m_strInitialText.IsEmpty() )
	if( !m_wndInfoMousePos.m_strInitialText.IsEmpty() )
	{
		CSize sizeIcon = m_wndIconEditor.IedSizeGet();
		CExtSafeString s;
		if( m_wndIconEditor.m_ptMouseTrace.x < 0 )
			s = _T("");
		else
			s.Format(
				(LPCTSTR)m_wndInfoMousePos.m_strInitialText,
				(int)m_wndIconEditor.m_ptMouseTrace.x,
				(int)m_wndIconEditor.m_ptMouseTrace.y
				);
		m_wndInfoMousePos.Invalidate();
		m_wndInfoMousePos.SetWindowText( s );
	} // if( !m_wndInfoMousePos.m_strInitialText.IsEmpty() )
}

void CExtIconEditDlg::SaveWindowRect() const
{
	ASSERT_VALID( this );
	ASSERT( !m_strProfileSection.IsEmpty() );
	ASSERT( !m_strProfileEntryWindow.IsEmpty() );
	CExtResizableDialog::SaveWindowRect();
	if( !m_strProfileEntryIconEditor.IsEmpty() )
	{
		CExtSafeString strVar;
		INT nMagnification = (INT)m_wndIconEditor.IedMagnificationGet();
		CPoint ptScroll = m_wndIconEditor.OnSwGetScrollPos();
		LONG nClrForeground = (LONG)m_wndIconEditor.IedColorForegroundGet();
		LONG nClrBackground = (LONG)m_wndIconEditor.IedColorBackgroundGet();
		LONG nActiveTool = (LONG)m_wndIconEditor.IedToolGet();
		LONG nLastToolBrush = (LONG)m_eLastToolBrush;
		LONG nLastToolAirBrush = (LONG)m_eLastToolAirBrush;
		LONG nLastToolLine = (LONG)m_eLastToolLine;
		LONG nLastToolRectangle = (LONG)m_eLastToolRectangle;
		LONG nLastToolOutlinedRectangle = (LONG)m_eLastToolOutlinedRectangle;
		LONG nLastToolRoundedRectangle = (LONG)m_eLastToolRoundedRectangle;
		LONG nLastToolOutlinedRoundedRectangle = (LONG)m_eLastToolOutlinedRoundedRectangle;
		LONG nLastToolEllipse = (LONG)m_eLastToolEllipse;
		LONG nLastToolOutlinedEllipse = (LONG)m_eLastToolOutlinedEllipse;
		LONG nFlags = 0L;
		if( (m_wndIconEditor.IedGetStyle() & __EIEDWS_GRID_LINES) != 0 )
			nFlags |= 0x00000001L;
		strVar.Format(
			_T("%d,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld"),
			nMagnification,
			ptScroll.x,
			ptScroll.y,
			nClrForeground,
			nClrBackground,
			nActiveTool,
			nLastToolBrush,
			nLastToolAirBrush,
			nLastToolLine,
			nLastToolRectangle,
			nLastToolOutlinedRectangle,
			nLastToolRoundedRectangle,
			nLastToolOutlinedRoundedRectangle,
			nLastToolEllipse,
			nLastToolOutlinedEllipse,
			nFlags
			);
		::AfxGetApp()->WriteProfileString(
			m_strProfileSection,
			m_strProfileEntryIconEditor,
			strVar
			);
	} // if( !m_strProfileEntryIconEditor.IsEmpty() )
}

void CExtIconEditDlg::LoadWindowRect(
	bool bForceHideWindow
	)
{
	ASSERT_VALID( this );
	ASSERT( !m_strProfileSection.IsEmpty() );
	ASSERT( !m_strProfileEntryWindow.IsEmpty() );
	CExtResizableDialog::LoadWindowRect( bForceHideWindow );
	if( !m_strProfileEntryIconEditor.IsEmpty() )
	{
		CExtSafeString strVar =
			::AfxGetApp()->GetProfileString(
				m_strProfileSection,
				m_strProfileEntryIconEditor
				);
		if( !strVar.IsEmpty() )
		{
			INT nMagnification = 0;
			CPoint ptScroll( -1, -1 );
			LONG nClrForeground(-1L), nClrBackground(-1L), nActiveTool(-1L),
				nLastToolBrush(-1L), nLastToolAirBrush(-1L), nLastToolLine(-1L),
				nLastToolRectangle(-1L), nLastToolOutlinedRectangle(-1L),
				nLastToolRoundedRectangle(-1L), nLastToolOutlinedRoundedRectangle(-1L),
				nLastToolEllipse(-1L), nLastToolOutlinedEllipse(-1L), nFlags(0L);
			if( _stscanf(
					strVar,
					_T("%d,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld"),
					&nMagnification,
					&ptScroll.x,
					&ptScroll.y,
					&nClrForeground,
					&nClrBackground,
					&nActiveTool,
					&nLastToolBrush,
					&nLastToolAirBrush,
					&nLastToolLine,
					&nLastToolRectangle,
					&nLastToolOutlinedRectangle,
					&nLastToolRoundedRectangle,
					&nLastToolOutlinedRoundedRectangle,
					&nLastToolEllipse,
					&nLastToolOutlinedEllipse,
					&nFlags
					) == 16
				)
			{ // if scan successful
				if(		1 <= nMagnification
					&&	nMagnification <= 16
					&&	((UINT)nMagnification) != m_wndIconEditor.IedMagnificationGet()
					)
					m_wndIconEditor.IedMagnificationSet(
						(UINT)nMagnification,
						true
						);
				if(		ptScroll.x >= 0
					&&	ptScroll.y >= 0
					&&	ptScroll != m_wndIconEditor.OnSwGetScrollPos()
					)
					m_wndIconEditor.OnSwSetScrollPos( ptScroll );
				if( (nClrForeground & 0xFF000000) == 0 )
				{
					INT nColorIdx =
						m_wndColorPalette.CpwColorGetIndexOf(
							(COLORREF)nClrForeground
							);
					if( nColorIdx >= 0 )
						m_wndColorPalette.CpwForegroundColorSet(
							(COLORREF)nClrForeground
							);
				}
				if( (nClrBackground & 0xFF000000) == 0 )
				{
					INT nColorIdx =
						m_wndColorPalette.CpwColorGetIndexOf(
							(COLORREF)nClrBackground
							);
					if( nColorIdx >= 0 )
						m_wndColorPalette.CpwBackgroundColorSet(
							(COLORREF)nClrBackground
							);
				}
				if(		((LONG)CExtImageEditWnd::__IEDT_VALUE_MIN) <= nActiveTool
					&&	nActiveTool <= ((LONG)CExtImageEditWnd::__IEDT_VALUE_MAX)
					)
					m_wndIconEditor.IedToolSet( (CExtImageEditWnd::e_tool_t)nActiveTool );
				if(		((LONG)CExtImageEditWnd::__IEDT_VALUE_MIN) <= nLastToolBrush
					&&	nLastToolBrush <= ((LONG)CExtImageEditWnd::__IEDT_VALUE_MAX)
					)
					m_eLastToolBrush = (CExtImageEditWnd::e_tool_t)nLastToolBrush;
				if(		((LONG)CExtImageEditWnd::__IEDT_VALUE_MIN) <= nLastToolAirBrush
					&&	nLastToolAirBrush <= ((LONG)CExtImageEditWnd::__IEDT_VALUE_MAX)
					)
					m_eLastToolAirBrush = (CExtImageEditWnd::e_tool_t)nLastToolAirBrush;
				if(		((LONG)CExtImageEditWnd::__IEDT_VALUE_MIN) <= nLastToolLine
					&&	nLastToolLine <= ((LONG)CExtImageEditWnd::__IEDT_VALUE_MAX)
					)
					m_eLastToolLine = (CExtImageEditWnd::e_tool_t)nLastToolLine;
				if(		((LONG)CExtImageEditWnd::__IEDT_VALUE_MIN) <= nLastToolRectangle
					&&	nLastToolRectangle <= ((LONG)CExtImageEditWnd::__IEDT_VALUE_MAX)
					)
					m_eLastToolRectangle = (CExtImageEditWnd::e_tool_t)nLastToolRectangle;
				if(		((LONG)CExtImageEditWnd::__IEDT_VALUE_MIN) <= nLastToolOutlinedRectangle
					&&	nLastToolOutlinedRectangle <= ((LONG)CExtImageEditWnd::__IEDT_VALUE_MAX)
					)
					m_eLastToolOutlinedRectangle = (CExtImageEditWnd::e_tool_t)nLastToolOutlinedRectangle;
				if(		((LONG)CExtImageEditWnd::__IEDT_VALUE_MIN) <= nLastToolRoundedRectangle
					&&	nLastToolRoundedRectangle <= ((LONG)CExtImageEditWnd::__IEDT_VALUE_MAX)
					)
					m_eLastToolRoundedRectangle = (CExtImageEditWnd::e_tool_t)nLastToolRoundedRectangle;
				if(		((LONG)CExtImageEditWnd::__IEDT_VALUE_MIN) <= nLastToolOutlinedRoundedRectangle
					&&	nLastToolOutlinedRoundedRectangle <= ((LONG)CExtImageEditWnd::__IEDT_VALUE_MAX)
					)
					m_eLastToolOutlinedRoundedRectangle = (CExtImageEditWnd::e_tool_t)nLastToolOutlinedRoundedRectangle;
				if(		((LONG)CExtImageEditWnd::__IEDT_VALUE_MIN) <= nLastToolEllipse
					&&	nLastToolEllipse <= ((LONG)CExtImageEditWnd::__IEDT_VALUE_MAX)
					)
					m_eLastToolEllipse = (CExtImageEditWnd::e_tool_t)nLastToolEllipse;
				if(		((LONG)CExtImageEditWnd::__IEDT_VALUE_MIN) <= nLastToolOutlinedEllipse
					&&	nLastToolOutlinedEllipse <= ((LONG)CExtImageEditWnd::__IEDT_VALUE_MAX)
					)
					m_eLastToolOutlinedEllipse = (CExtImageEditWnd::e_tool_t)nLastToolOutlinedEllipse;
				if( (nFlags & 0x00000001L) != 0 )
				{
					if( (m_wndIconEditor.IedGetStyle() & __EIEDWS_GRID_LINES) == 0 )
						m_wndIconEditor.IedModifyStyle( 0, __EIEDWS_GRID_LINES, true );
				}
				else
				{
					if( (m_wndIconEditor.IedGetStyle() & __EIEDWS_GRID_LINES) != 0 )
						m_wndIconEditor.IedModifyStyle( __EIEDWS_GRID_LINES,  0,true );
				}
			} // if scan successful
		} // if( !strVar.IsEmpty() )
	} // if( !m_strProfileEntryIconEditor.IsEmpty() )
}

void CExtIconEditDlg::OnSize(UINT nType, int cx, int cy) 
{
	CExtResizableDialog::OnSize(nType, cx, cy);
	
	if( nType != SIZE_MINIMIZED )
		CWnd::RepositionBars(0,0xFFFF,0);

	if( m_wndColorPalette.GetSafeHwnd() != NULL )
		m_wndColorPalette.Invalidate();
	if( m_wndIconViewer.GetSafeHwnd() != NULL )
		m_wndIconViewer.Invalidate();
	if( m_wndIconEditor.GetSafeHwnd() != NULL )
		m_wndIconEditor.Invalidate();
}

int CExtIconEditDlg::DoModal() 
{
	if( m_icon.IsEmpty() )
	{
		ASSERT( FALSE );
		return IDCANCEL;
	}
	return CExtResizableDialog::DoModal();
}

void CExtIconEditDlg::OnOK()
{
	m_wndIconEditor.OnIedCancelActions( false );
	m_wndIconEditor._EmptyCache();
	if( m_wndIconEditor.IedUndoDepthGet() == 0 )
	{
		SendMessage( WM_COMMAND, IDCANCEL );
		return;
	}
HBITMAP hBitmap = m_wndIconEditor.IedBitmapGet();
	ASSERT( hBitmap != NULL );
COLORREF clrTransparent =
		m_wndIconEditor.IedGetTransparencyColor();
	m_icon.AssignFromHBITMAP( hBitmap, clrTransparent );
	if( m_icon.IsEmpty() )
	{
		ASSERT( FALSE );
		SendMessage( WM_COMMAND, IDCANCEL );
		return;
	}

	VERIFY( g_CmdManager->ProfileWndRemove( m_hWnd ) );
	CExtResizableDialog::OnOK();
}

void CExtIconEditDlg::OnAskSaveChanges(
	bool & bAllowCloseDialog,
	bool & bReturnOkCode
   )
{
	ASSERT_VALID( this );
	bAllowCloseDialog = true;
	bReturnOkCode = false;
//
// SAMPLE:
//
//	switch( AfxMessageBox(_T("Save Changes?"),MB_YESNOCANCEL) )
//	{
//	case IDYES:
//		bAllowCloseDialog = true;
//		bReturnOkCode = true;
//		break;
//	case IDNO:
//		bAllowCloseDialog = true;
//		bReturnOkCode = false;
//		break;
//	default:
//		bAllowCloseDialog = false;
//		break;
//	} // switch( AfxMessageBox(_T("Save Changes?"),MB_YESNOCANCEL) )
//
}

void CExtIconEditDlg::OnCancel()
{
	if( m_wndIconEditor.IedUndoDepthGet() != 0 )
	{
		bool bAllowCloseDialog = true, bReturnOkCode = false;
		OnAskSaveChanges(
			bAllowCloseDialog,
			bReturnOkCode
			);
		if( !bAllowCloseDialog )
			return;
		if( bReturnOkCode )
		{
			SendMessage( WM_COMMAND, IDOK );
			return;
		}
	} // if( m_wndIconEditor.IedUndoDepthGet() != 0 )
	VERIFY( g_CmdManager->ProfileWndRemove( m_hWnd ) );
	CExtResizableDialog::OnCancel();
}

BOOL CExtIconEditDlg::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN )
	{
		if( m_wndIconEditor.PreTranslateMessage(pMsg) )
			return TRUE;
	} // if( pMsg->message == WM_KEYDOWN )
	else if( pMsg->message == WM_MOUSEWHEEL )
	{
		if( m_wndIconEditor.PreTranslateMessage(pMsg) )
			return TRUE;
		if( m_wndColorPalette.PreTranslateMessage(pMsg) )
			return TRUE;
		if( m_wndIconViewer.PreTranslateMessage(pMsg) )
			return TRUE;
	} // else if( pMsg->message == WM_MOUSEWHEEL )
	return CExtResizableDialog::PreTranslateMessage(pMsg);
}

CExtSafeString CExtIconEditDlg::OnGetUndoTypeDescription(
	INT nUndoType
	)
{
	ASSERT_VALID( this );
CExtSafeString strUndoDescription( _T("") );
	switch( nUndoType )
	{
	case CExtImageEditWnd::__EUT_UNKNOWN_OPERATION:
	break;
	case CExtImageEditWnd::__EUT_TRANSPARENCY_MODE:
		strUndoDescription = _T("Transparency");
	break;
	case CExtImageEditWnd::__EUT_RESIZING_IMAGE:
		strUndoDescription = _T("Image Resizing");
	break;
	case CExtImageEditWnd::__EUT_RESIZING_SELECTION:
		strUndoDescription = _T("Selection Resizing");
	break;
	case CExtImageEditWnd::__EUT_FILL:
		strUndoDescription = _T("Fill");
	break;
	case CExtImageEditWnd::__EUT_PENCIL:
		strUndoDescription = _T("Pencil Drawing");
	break;
	case CExtImageEditWnd::__EUT_BRUSH:
		strUndoDescription = _T("Brush Drawing");
	break;
	case CExtImageEditWnd::__EUT_AIRBRUSH:
		strUndoDescription = _T("Airbrush Drawing");
	break;
	case CExtImageEditWnd::__EUT_LINE:
		strUndoDescription = _T("Line Drawing");
	break;
	case CExtImageEditWnd::__EUT_RECTANGLE:
	case CExtImageEditWnd::__EUT_RECTANGLE_OUTLINED:
	case CExtImageEditWnd::__EUT_RECTANGLE_FILLED:
		strUndoDescription = _T("Rectangle Drawing");
	break;
	case CExtImageEditWnd::__EUT_ROUNDED_RECTANGLE:
	case CExtImageEditWnd::__EUT_ROUNDED_RECTANGLE_OUTLINED:
	case CExtImageEditWnd::__EUT_ROUNDED_RECTANGLE_FILLED:
		strUndoDescription = _T("Rounded Rectangle Drawing");
	break;
	case CExtImageEditWnd::__EUT_ELLIPSE:
	case CExtImageEditWnd::__EUT_ELLIPSE_OUTLINED:
	case CExtImageEditWnd::__EUT_ELLIPSE_FILLED:
		strUndoDescription = _T("Ellipse Drawing");
	break;
	case CExtImageEditWnd::__EUT_MOVING_SELECTION:
		strUndoDescription = _T("Selection Moving");
	break;
	case CExtImageEditWnd::__EUT_SELECTION_DELETE:
		strUndoDescription = _T("Selection Delete");
	break;
	case CExtImageEditWnd::__EUT_IMAGE_CUT:
		strUndoDescription = _T("Image Cut To Clipboard");
	break;
	case CExtImageEditWnd::__EUT_SELECTION_CUT:
		strUndoDescription = _T("Selection Cut To Clipboard");
	break;
	case CExtImageEditWnd::__EUT_PASTE:
		strUndoDescription = _T("Paste From Clipboard");
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( nUndoType )
	return strUndoDescription;
}

void CExtIconEditDlg::OnIedEditUndo() 
{
	m_wndIconEditor.IedUndo( 1 );
}
void CExtIconEditDlg::OnUpdateIedEditUndo(CCmdUI* pCmdUI) 
{
	ASSERT( pCmdUI != NULL );
	ASSERT( m_wndIconEditor.IedUndoDepthGet() >= 0 );
	if( m_wndIconEditor.IedUndoDepthGet() == 0 )
	{
		pCmdUI->Enable( FALSE );
		return;
	}
	pCmdUI->Enable( TRUE );
CExtSafeString strUndoDescription(
		OnGetUndoTypeDescription(
			m_wndIconEditor.IedUndoTypeGet()
			)
		);
CExtSafeString str( _T("&Undo") );
	if( !strUndoDescription.IsEmpty() )
	{
		str += _T(" ");
		str += strUndoDescription;
	}
	pCmdUI->SetText( str );
}

void CExtIconEditDlg::OnIedEditRedo() 
{
	m_wndIconEditor.IedUndo( -1 );
}
void CExtIconEditDlg::OnUpdateIedEditRedo(CCmdUI* pCmdUI) 
{
	ASSERT( pCmdUI != NULL );
	ASSERT( m_wndIconEditor.IedUndoDepthGet() >= 0 );
INT nUndoPos = m_wndIconEditor.IedUndoPosGet();
	if( nUndoPos == 0 )
	{
		pCmdUI->Enable( FALSE );
		return;
	}
	pCmdUI->Enable( TRUE );
CExtSafeString strUndoDescription(
		OnGetUndoTypeDescription(
			m_wndIconEditor.IedUndoTypeGet( nUndoPos - 1 )
			)
		);
CExtSafeString str( _T("&Redo") );
	if( !strUndoDescription.IsEmpty() )
	{
		str += _T(" ");
		str += strUndoDescription;
	}
	pCmdUI->SetText( str );
}

void CExtIconEditDlg::OnIedEditSelectAll() 
{
	m_wndIconEditor.IedSelectionRectSet();
}

void CExtIconEditDlg::OnIedEditCopy() 
{
	VERIFY( m_wndIconEditor.IedCopyToClipboard() );
}
void CExtIconEditDlg::OnUpdateIedEditCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_wndIconEditor.IedCanCopy() ? TRUE : FALSE );
}

void CExtIconEditDlg::OnIedEditCut() 
{
	VERIFY( m_wndIconEditor.IedCutToClipboard() );
}
void CExtIconEditDlg::OnUpdateIedEditCut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_wndIconEditor.IedCanCut() ? TRUE : FALSE );
}

void CExtIconEditDlg::OnIedEditPaste() 
{
	VERIFY( m_wndIconEditor.IedPasteFromClipboard() );
}
void CExtIconEditDlg::OnUpdateIedEditPaste(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_wndIconEditor.IedCanPaste() ? TRUE : FALSE );
}

void CExtIconEditDlg::OnIedToolRectSelection() 
{
	m_wndIconEditor.IedToolSet( CExtImageEditWnd::__IEDT_RECT_SELECTION );
}
void CExtIconEditDlg::OnUpdateIedToolRectSelection(CCmdUI* pCmdUI) 
{
CExtImageEditWnd::e_tool_t eTool = m_wndIconEditor.IedToolGet();
	pCmdUI->SetCheck(
		(eTool == CExtImageEditWnd::__IEDT_RECT_SELECTION) ? TRUE : FALSE
		);
}

void CExtIconEditDlg::OnIedToolColorSelection() 
{
	m_wndIconEditor.IedToolSet( CExtImageEditWnd::__IEDT_COLOR_SELECTION );
}
void CExtIconEditDlg::OnUpdateIedToolColorSelection(CCmdUI* pCmdUI) 
{
CExtImageEditWnd::e_tool_t eTool = m_wndIconEditor.IedToolGet();
	pCmdUI->SetCheck(
		(eTool == CExtImageEditWnd::__IEDT_COLOR_SELECTION) ? TRUE : FALSE
		);
}

void CExtIconEditDlg::OnIedToolFill() 
{
	m_wndIconEditor.IedToolSet( CExtImageEditWnd::__IEDT_FILL );
}
void CExtIconEditDlg::OnUpdateIedToolFill(CCmdUI* pCmdUI) 
{
CExtImageEditWnd::e_tool_t eTool = m_wndIconEditor.IedToolGet();
	pCmdUI->SetCheck(
		(eTool == CExtImageEditWnd::__IEDT_FILL) ? TRUE : FALSE
		);
}


void CExtIconEditDlg::OnIedToolMagnification() 
{
	m_wndIconEditor.IedToolSet( CExtImageEditWnd::__IEDT_MAGNIFICATON );
}
void CExtIconEditDlg::OnUpdateIedToolMagnification(CCmdUI* pCmdUI) 
{
CExtImageEditWnd::e_tool_t eTool = m_wndIconEditor.IedToolGet();
	pCmdUI->SetCheck(
		(eTool == CExtImageEditWnd::__IEDT_MAGNIFICATON) ? TRUE : FALSE
		);
}
void CExtIconEditDlg::OnIedCmdRangeMagnification( UINT nCmdID )
{
	ASSERT(
			ID_IED_TOOL_MAGNIFICATION_01X <= nCmdID
		&&	nCmdID <= ID_IED_TOOL_MAGNIFICATION_16X
		);
	ASSERT( (ID_IED_TOOL_MAGNIFICATION_16X - ID_IED_TOOL_MAGNIFICATION_01X) == 15 );
	m_wndIconEditor.IedMagnificationSet( nCmdID - ID_IED_TOOL_MAGNIFICATION_01X + 1, true );
}
void CExtIconEditDlg::OnUpdateIedRangeMagnification(CCmdUI* pCmdUI)
{
	ASSERT(
			ID_IED_TOOL_MAGNIFICATION_01X <= pCmdUI->m_nID
		&&	pCmdUI->m_nID <= ID_IED_TOOL_MAGNIFICATION_16X
		);
	ASSERT( (ID_IED_TOOL_MAGNIFICATION_16X - ID_IED_TOOL_MAGNIFICATION_01X) == 15 );
UINT nMagnifcation = m_wndIconEditor.IedMagnificationGet();
UINT nMagnifcationTest = pCmdUI->m_nID - ID_IED_TOOL_MAGNIFICATION_01X + 1;
	pCmdUI->SetCheck(
		( nMagnifcation == nMagnifcationTest ) ? TRUE : FALSE
		);
}

void CExtIconEditDlg::OnIedToolPencil() 
{
	m_wndIconEditor.IedToolSet( CExtImageEditWnd::__IEDT_PENCIL );
}
void CExtIconEditDlg::OnUpdateIedToolPencil(CCmdUI* pCmdUI) 
{
CExtImageEditWnd::e_tool_t eTool = m_wndIconEditor.IedToolGet();
	pCmdUI->SetCheck(
		(eTool == CExtImageEditWnd::__IEDT_PENCIL) ? TRUE : FALSE
		);
}

void CExtIconEditDlg::OnIedToolBrush() 
{
	ASSERT(
			CExtImageEditWnd::__IEDT_BRUSH_VALUE_MIN <= m_eLastToolBrush
		&&	m_eLastToolBrush <= CExtImageEditWnd::__IEDT_BRUSH_VALUE_MAX
		);
	m_wndIconEditor.IedToolSet( m_eLastToolBrush );
}
void CExtIconEditDlg::OnUpdateIedToolBrush(CCmdUI* pCmdUI) 
{
CExtImageEditWnd::e_tool_t eTool = m_wndIconEditor.IedToolGet();
	pCmdUI->SetCheck(
		(	CExtImageEditWnd::__IEDT_BRUSH_VALUE_MIN <= eTool
		&&	eTool <= CExtImageEditWnd::__IEDT_BRUSH_VALUE_MAX
		) ? TRUE : FALSE
		);
}
void CExtIconEditDlg::OnIedCmdRangeBrush( UINT nCmdID )
{
	ASSERT(
			ID_IED_TOOL_BRUSH_CIRCLE_SMALL <= nCmdID
		&&	nCmdID <= ID_IED_TOOL_BRUSH_LINE_RD_LARGE
		);
	ASSERT( (ID_IED_TOOL_BRUSH_LINE_RD_LARGE - ID_IED_TOOL_BRUSH_CIRCLE_SMALL) == 11 );
	m_eLastToolBrush = (CExtImageEditWnd::e_tool_t)
		(nCmdID - ID_IED_TOOL_BRUSH_CIRCLE_SMALL + (UINT)CExtImageEditWnd::__IEDT_BRUSH_VALUE_MIN);
	OnIedToolBrush();
}
void CExtIconEditDlg::OnUpdateIedToolBrushRange(CCmdUI* pCmdUI) 
{
	ASSERT(
			ID_IED_TOOL_BRUSH_CIRCLE_SMALL <= pCmdUI->m_nID
		&&	pCmdUI->m_nID <= ID_IED_TOOL_BRUSH_LINE_RD_LARGE
		);
	ASSERT( (ID_IED_TOOL_BRUSH_LINE_RD_LARGE - ID_IED_TOOL_BRUSH_CIRCLE_SMALL) == 11 );
CExtImageEditWnd::e_tool_t eToolTest = (CExtImageEditWnd::e_tool_t)
		(pCmdUI->m_nID - ID_IED_TOOL_BRUSH_CIRCLE_SMALL + (UINT)CExtImageEditWnd::__IEDT_BRUSH_VALUE_MIN);
	pCmdUI->SetCheck(
		( m_eLastToolBrush == eToolTest ) ? TRUE : FALSE
		);
}

void CExtIconEditDlg::OnIedToolAirBrush() 
{
	ASSERT(
			CExtImageEditWnd::__IEDT_AIRBRUSH_VALUE_MIN <= m_eLastToolAirBrush
		&&	m_eLastToolAirBrush <= CExtImageEditWnd::__IEDT_AIRBRUSH_VALUE_MAX
		);
	m_wndIconEditor.IedToolSet( m_eLastToolAirBrush );
}
void CExtIconEditDlg::OnUpdateIedToolAirBrush(CCmdUI* pCmdUI) 
{
CExtImageEditWnd::e_tool_t eTool = m_wndIconEditor.IedToolGet();
	pCmdUI->SetCheck(
		(	CExtImageEditWnd::__IEDT_AIRBRUSH_VALUE_MIN <= eTool
		&&	eTool <= CExtImageEditWnd::__IEDT_AIRBRUSH_VALUE_MAX
		) ? TRUE : FALSE
		);
}
void CExtIconEditDlg::OnIedCmdRangeAirBrush( UINT nCmdID )
{
	ASSERT(
			ID_IED_TOOL_AIR_BRUSH_SMALL <= nCmdID
		&&	nCmdID <= ID_IED_TOOL_AIR_BRUSH_LARGE
		);
	ASSERT( (ID_IED_TOOL_AIR_BRUSH_LARGE - ID_IED_TOOL_AIR_BRUSH_SMALL) == 2 );
	m_eLastToolAirBrush = (CExtImageEditWnd::e_tool_t)
		(nCmdID - ID_IED_TOOL_AIR_BRUSH_SMALL + (UINT)CExtImageEditWnd::__IEDT_AIRBRUSH_VALUE_MIN);
	OnIedToolAirBrush();
}
void CExtIconEditDlg::OnUpdateIedToolAirBrushRange(CCmdUI* pCmdUI) 
{
	ASSERT(
			ID_IED_TOOL_AIR_BRUSH_SMALL <= pCmdUI->m_nID
		&&	pCmdUI->m_nID <= ID_IED_TOOL_AIR_BRUSH_LARGE
		);
	ASSERT( (ID_IED_TOOL_AIR_BRUSH_LARGE - ID_IED_TOOL_AIR_BRUSH_SMALL) == 2 );
CExtImageEditWnd::e_tool_t eToolTest = (CExtImageEditWnd::e_tool_t)
		(pCmdUI->m_nID - ID_IED_TOOL_AIR_BRUSH_SMALL + (UINT)CExtImageEditWnd::__IEDT_AIRBRUSH_VALUE_MIN);
	pCmdUI->SetCheck(
		( m_eLastToolAirBrush == eToolTest ) ? TRUE : FALSE
		);
}

void CExtIconEditDlg::OnIedToolLine() 
{
	ASSERT(
			CExtImageEditWnd::__IEDT_LINE_VALUE_MIN <= m_eLastToolLine
		&&	m_eLastToolLine <= CExtImageEditWnd::__IEDT_LINE_VALUE_MAX
		);
	m_wndIconEditor.IedToolSet( m_eLastToolLine );
}
void CExtIconEditDlg::OnUpdateIedToolLine(CCmdUI* pCmdUI) 
{
CExtImageEditWnd::e_tool_t eTool = m_wndIconEditor.IedToolGet();
	pCmdUI->SetCheck(
		(	CExtImageEditWnd::__IEDT_LINE_VALUE_MIN <= eTool
		&&	eTool <= CExtImageEditWnd::__IEDT_LINE_VALUE_MAX
		) ? TRUE : FALSE
		);
}
void CExtIconEditDlg::OnIedCmdRangeLine( UINT nCmdID )
{
	ASSERT(
			ID_IED_TOOL_LINE_CIRCLE_SMALL <= nCmdID
		&&	nCmdID <= ID_IED_TOOL_LINE_LINE_RD_LARGE
		);
	ASSERT( (ID_IED_TOOL_LINE_LINE_RD_LARGE - ID_IED_TOOL_LINE_CIRCLE_SMALL) == 11 );
	m_eLastToolLine = (CExtImageEditWnd::e_tool_t)
		(nCmdID - ID_IED_TOOL_LINE_CIRCLE_SMALL + (UINT)CExtImageEditWnd::__IEDT_LINE_VALUE_MIN);
	OnIedToolLine();
}
void CExtIconEditDlg::OnUpdateIedToolLineRange(CCmdUI* pCmdUI) 
{
	ASSERT(
			ID_IED_TOOL_LINE_CIRCLE_SMALL <= pCmdUI->m_nID
		&&	pCmdUI->m_nID <= ID_IED_TOOL_LINE_LINE_RD_LARGE
		);
	ASSERT( (ID_IED_TOOL_LINE_LINE_RD_LARGE - ID_IED_TOOL_LINE_CIRCLE_SMALL) == 11 );
CExtImageEditWnd::e_tool_t eToolTest = (CExtImageEditWnd::e_tool_t)
		(pCmdUI->m_nID - ID_IED_TOOL_LINE_CIRCLE_SMALL + (UINT)CExtImageEditWnd::__IEDT_LINE_VALUE_MIN);
	pCmdUI->SetCheck(
		( m_eLastToolLine == eToolTest ) ? TRUE : FALSE
		);
}

void CExtIconEditDlg::OnIedToolRectangle() 
{
	ASSERT(
			CExtImageEditWnd::__IEDT_RECT_VALUE_MIN <= m_eLastToolRectangle
		&&	m_eLastToolRectangle <= CExtImageEditWnd::__IEDT_RECT_VALUE_MAX
		);
	m_wndIconEditor.IedToolSet( m_eLastToolRectangle );
}
void CExtIconEditDlg::OnUpdateIedToolRectangle(CCmdUI* pCmdUI) 
{
CExtImageEditWnd::e_tool_t eTool = m_wndIconEditor.IedToolGet();
	pCmdUI->SetCheck(
		(	CExtImageEditWnd::__IEDT_RECT_VALUE_MIN <= eTool
		&&	eTool <= CExtImageEditWnd::__IEDT_RECT_VALUE_MAX
		) ? TRUE : FALSE
		);
}
void CExtIconEditDlg::OnIedCmdRangeRectangle( UINT nCmdID )
{
	ASSERT(
			ID_IED_TOOL_RECTANGLE_SMALLER <= nCmdID
		&&	nCmdID <= ID_IED_TOOL_RECTANGLE_LARGER
		);
	ASSERT( (ID_IED_TOOL_RECTANGLE_LARGER - ID_IED_TOOL_RECTANGLE_SMALLER) == 4 );
	m_eLastToolRectangle = (CExtImageEditWnd::e_tool_t)
		(nCmdID - ID_IED_TOOL_RECTANGLE_SMALLER + (UINT)CExtImageEditWnd::__IEDT_RECT_VALUE_MIN);
	OnIedToolRectangle();
}
void CExtIconEditDlg::OnUpdateIedToolRectangleRange(CCmdUI* pCmdUI) 
{
	ASSERT(
			ID_IED_TOOL_RECTANGLE_SMALLER <= pCmdUI->m_nID
		&&	pCmdUI->m_nID <= ID_IED_TOOL_RECTANGLE_LARGER
		);
	ASSERT( (ID_IED_TOOL_RECTANGLE_LARGER - ID_IED_TOOL_RECTANGLE_SMALLER) == 4 );
CExtImageEditWnd::e_tool_t eToolTest = (CExtImageEditWnd::e_tool_t)
		(pCmdUI->m_nID - ID_IED_TOOL_RECTANGLE_SMALLER + (UINT)CExtImageEditWnd::__IEDT_RECT_VALUE_MIN);
	pCmdUI->SetCheck(
		( m_eLastToolRectangle == eToolTest ) ? TRUE : FALSE
		);
}

void CExtIconEditDlg::OnIedToolOutlinedRectangle() 
{
	ASSERT(
			CExtImageEditWnd::__IEDT_RECT_OUTLINED_VALUE_MIN <= m_eLastToolOutlinedRectangle
		&&	m_eLastToolOutlinedRectangle <= CExtImageEditWnd::__IEDT_RECT_OUTLINED_VALUE_MAX
		);
	m_wndIconEditor.IedToolSet( m_eLastToolOutlinedRectangle );
}
void CExtIconEditDlg::OnUpdateIedToolOutlinedRectangle(CCmdUI* pCmdUI) 
{
CExtImageEditWnd::e_tool_t eTool = m_wndIconEditor.IedToolGet();
	pCmdUI->SetCheck(
		(	CExtImageEditWnd::__IEDT_RECT_OUTLINED_VALUE_MIN <= eTool
		&&	eTool <= CExtImageEditWnd::__IEDT_RECT_OUTLINED_VALUE_MAX
		) ? TRUE : FALSE
		);
}
void CExtIconEditDlg::OnIedCmdRangeOutlinedRectangle( UINT nCmdID )
{
	ASSERT(
			ID_IED_TOOL_OUTLINED_RECTANGLE_SMALLER <= nCmdID
		&&	nCmdID <= ID_IED_TOOL_OUTLINED_RECTANGLE_LARGER
		);
	ASSERT( (ID_IED_TOOL_OUTLINED_RECTANGLE_LARGER - ID_IED_TOOL_OUTLINED_RECTANGLE_SMALLER) == 4 );
	m_eLastToolOutlinedRectangle = (CExtImageEditWnd::e_tool_t)
		(nCmdID - ID_IED_TOOL_OUTLINED_RECTANGLE_SMALLER + (UINT)CExtImageEditWnd::__IEDT_RECT_OUTLINED_VALUE_MIN);
	OnIedToolOutlinedRectangle();
}
void CExtIconEditDlg::OnUpdateIedToolOutlinedRectangleRange(CCmdUI* pCmdUI) 
{
	ASSERT(
			ID_IED_TOOL_OUTLINED_RECTANGLE_SMALLER <= pCmdUI->m_nID
		&&	pCmdUI->m_nID <= ID_IED_TOOL_OUTLINED_RECTANGLE_LARGER
		);
	ASSERT( (ID_IED_TOOL_OUTLINED_RECTANGLE_LARGER - ID_IED_TOOL_OUTLINED_RECTANGLE_SMALLER) == 4 );
CExtImageEditWnd::e_tool_t eToolTest = (CExtImageEditWnd::e_tool_t)
		(pCmdUI->m_nID - ID_IED_TOOL_OUTLINED_RECTANGLE_SMALLER + (UINT)CExtImageEditWnd::__IEDT_RECT_OUTLINED_VALUE_MIN);
	pCmdUI->SetCheck(
		( m_eLastToolOutlinedRectangle == eToolTest ) ? TRUE : FALSE
		);
}

void CExtIconEditDlg::OnIedToolFilledRectangle() 
{
	m_wndIconEditor.IedToolSet( CExtImageEditWnd::__IEDT_RECT_FILLED );
}
void CExtIconEditDlg::OnUpdateIedToolFilledRectangle(CCmdUI* pCmdUI) 
{
CExtImageEditWnd::e_tool_t eTool = m_wndIconEditor.IedToolGet();
	pCmdUI->SetCheck(
		(eTool == CExtImageEditWnd::__IEDT_RECT_FILLED) ? TRUE : FALSE
		);
}

void CExtIconEditDlg::OnIedToolRoundedRectangle() 
{
	ASSERT(
			CExtImageEditWnd::__IEDT_ROUNDED_RECT_VALUE_MIN <= m_eLastToolRoundedRectangle
		&&	m_eLastToolRoundedRectangle <= CExtImageEditWnd::__IEDT_ROUNDED_RECT_VALUE_MAX
		);
	m_wndIconEditor.IedToolSet( m_eLastToolRoundedRectangle );
}
void CExtIconEditDlg::OnUpdateIedToolRoundedRectangle(CCmdUI* pCmdUI) 
{
CExtImageEditWnd::e_tool_t eTool = m_wndIconEditor.IedToolGet();
	pCmdUI->SetCheck(
		(	CExtImageEditWnd::__IEDT_ROUNDED_RECT_VALUE_MIN <= eTool
		&&	eTool <= CExtImageEditWnd::__IEDT_ROUNDED_RECT_VALUE_MAX
		) ? TRUE : FALSE
		);
}
void CExtIconEditDlg::OnIedCmdRangeRoundedRectangle( UINT nCmdID )
{
	ASSERT(
			ID_IED_TOOL_ROUNDED_RECTANGLE_SMALLER <= nCmdID
		&&	nCmdID <= ID_IED_TOOL_ROUNDED_RECTANGLE_LARGER
		);
	ASSERT( (ID_IED_TOOL_ROUNDED_RECTANGLE_LARGER - ID_IED_TOOL_ROUNDED_RECTANGLE_SMALLER) == 4 );
	m_eLastToolRoundedRectangle = (CExtImageEditWnd::e_tool_t)
		(nCmdID - ID_IED_TOOL_ROUNDED_RECTANGLE_SMALLER + (UINT)CExtImageEditWnd::__IEDT_ROUNDED_RECT_VALUE_MIN);
	OnIedToolRoundedRectangle();
}
void CExtIconEditDlg::OnUpdateIedToolRoundedRectangleRange(CCmdUI* pCmdUI) 
{
	ASSERT(
			ID_IED_TOOL_ROUNDED_RECTANGLE_SMALLER <= pCmdUI->m_nID
		&&	pCmdUI->m_nID <= ID_IED_TOOL_ROUNDED_RECTANGLE_LARGER
		);
	ASSERT( (ID_IED_TOOL_ROUNDED_RECTANGLE_LARGER - ID_IED_TOOL_ROUNDED_RECTANGLE_SMALLER) == 4 );
CExtImageEditWnd::e_tool_t eToolTest = (CExtImageEditWnd::e_tool_t)
		(pCmdUI->m_nID - ID_IED_TOOL_ROUNDED_RECTANGLE_SMALLER + (UINT)CExtImageEditWnd::__IEDT_ROUNDED_RECT_VALUE_MIN);
	pCmdUI->SetCheck(
		( m_eLastToolRoundedRectangle == eToolTest ) ? TRUE : FALSE
		);
}

void CExtIconEditDlg::OnIedToolOutlinedRoundedRectangle() 
{
	ASSERT(
			CExtImageEditWnd::__IEDT_ROUNDED_RECT_OUTLINED_VALUE_MIN <= m_eLastToolOutlinedRoundedRectangle
		&&	m_eLastToolOutlinedRoundedRectangle <= CExtImageEditWnd::__IEDT_ROUNDED_RECT_OUTLINED_VALUE_MAX
		);
	m_wndIconEditor.IedToolSet( m_eLastToolOutlinedRoundedRectangle );
}
void CExtIconEditDlg::OnUpdateIedToolOutlinedRoundedRectangle(CCmdUI* pCmdUI) 
{
CExtImageEditWnd::e_tool_t eTool = m_wndIconEditor.IedToolGet();
	pCmdUI->SetCheck(
		(	CExtImageEditWnd::__IEDT_ROUNDED_RECT_OUTLINED_VALUE_MIN <= eTool
		&&	eTool <= CExtImageEditWnd::__IEDT_ROUNDED_RECT_OUTLINED_VALUE_MAX
		) ? TRUE : FALSE
		);
}
void CExtIconEditDlg::OnIedCmdRangeOutlinedRoundedRectangle( UINT nCmdID )
{
	ASSERT(
			ID_IED_TOOL_OUTLINED_ROUNDED_RECTANGLE_SMALLER <= nCmdID
		&&	nCmdID <= ID_IED_TOOL_OUTLINED_ROUNDED_RECTANGLE_LARGER
		);
	ASSERT( (ID_IED_TOOL_OUTLINED_ROUNDED_RECTANGLE_LARGER - ID_IED_TOOL_OUTLINED_ROUNDED_RECTANGLE_SMALLER) == 4 );
	m_eLastToolOutlinedRoundedRectangle = (CExtImageEditWnd::e_tool_t)
		(nCmdID - ID_IED_TOOL_OUTLINED_ROUNDED_RECTANGLE_SMALLER + (UINT)CExtImageEditWnd::__IEDT_ROUNDED_RECT_OUTLINED_VALUE_MIN);
	OnIedToolOutlinedRoundedRectangle();
}
void CExtIconEditDlg::OnUpdateIedToolOutlinedRoundedRectangleRange(CCmdUI* pCmdUI) 
{
	ASSERT(
			ID_IED_TOOL_OUTLINED_ROUNDED_RECTANGLE_SMALLER <= pCmdUI->m_nID
		&&	pCmdUI->m_nID <= ID_IED_TOOL_OUTLINED_ROUNDED_RECTANGLE_LARGER
		);
	ASSERT( (ID_IED_TOOL_OUTLINED_ROUNDED_RECTANGLE_LARGER - ID_IED_TOOL_OUTLINED_ROUNDED_RECTANGLE_SMALLER) == 4 );
CExtImageEditWnd::e_tool_t eToolTest = (CExtImageEditWnd::e_tool_t)
		(pCmdUI->m_nID - ID_IED_TOOL_OUTLINED_ROUNDED_RECTANGLE_SMALLER + (UINT)CExtImageEditWnd::__IEDT_ROUNDED_RECT_OUTLINED_VALUE_MIN);
	pCmdUI->SetCheck(
		( m_eLastToolOutlinedRoundedRectangle == eToolTest ) ? TRUE : FALSE
		);
}

void CExtIconEditDlg::OnIedToolFilledRoundedRectangle() 
{
	m_wndIconEditor.IedToolSet( CExtImageEditWnd::__IEDT_ROUNDED_RECT_FILLED );
}
void CExtIconEditDlg::OnUpdateIedToolFilledRoundedRectangle(CCmdUI* pCmdUI) 
{
CExtImageEditWnd::e_tool_t eTool = m_wndIconEditor.IedToolGet();
	pCmdUI->SetCheck(
		(eTool == CExtImageEditWnd::__IEDT_ROUNDED_RECT_FILLED) ? TRUE : FALSE
		);
}

void CExtIconEditDlg::OnIedToolEllipse() 
{
	ASSERT(
			CExtImageEditWnd::__IEDT_ELLIPSE_VALUE_MIN <= m_eLastToolEllipse
		&&	m_eLastToolEllipse <= CExtImageEditWnd::__IEDT_ELLIPSE_VALUE_MAX
		);
	m_wndIconEditor.IedToolSet( m_eLastToolEllipse );
}
void CExtIconEditDlg::OnUpdateIedToolEllipse(CCmdUI* pCmdUI) 
{
CExtImageEditWnd::e_tool_t eTool = m_wndIconEditor.IedToolGet();
	pCmdUI->SetCheck(
		(	CExtImageEditWnd::__IEDT_ELLIPSE_VALUE_MIN <= eTool
		&&	eTool <= CExtImageEditWnd::__IEDT_ELLIPSE_VALUE_MAX
		) ? TRUE : FALSE
		);
}
void CExtIconEditDlg::OnIedCmdRangeEllipse( UINT nCmdID )
{
	ASSERT(
			ID_IED_TOOL_ELLIPSE_SMALLER <= nCmdID
		&&	nCmdID <= ID_IED_TOOL_ELLIPSE_LARGER
		);
	ASSERT( (ID_IED_TOOL_ELLIPSE_LARGER - ID_IED_TOOL_ELLIPSE_SMALLER) == 4 );
	m_eLastToolEllipse = (CExtImageEditWnd::e_tool_t)
		(nCmdID - ID_IED_TOOL_ELLIPSE_SMALLER + (UINT)CExtImageEditWnd::__IEDT_ELLIPSE_VALUE_MIN);
	OnIedToolEllipse();
}
void CExtIconEditDlg::OnUpdateIedToolEllipseRange(CCmdUI* pCmdUI) 
{
	ASSERT(
			ID_IED_TOOL_ELLIPSE_SMALLER <= pCmdUI->m_nID
		&&	pCmdUI->m_nID <= ID_IED_TOOL_ELLIPSE_LARGER
		);
	ASSERT( (ID_IED_TOOL_ELLIPSE_LARGER - ID_IED_TOOL_ELLIPSE_SMALLER) == 4 );
CExtImageEditWnd::e_tool_t eToolTest = (CExtImageEditWnd::e_tool_t)
		(pCmdUI->m_nID - ID_IED_TOOL_ELLIPSE_SMALLER + (UINT)CExtImageEditWnd::__IEDT_ELLIPSE_VALUE_MIN);
	pCmdUI->SetCheck(
		( m_eLastToolEllipse == eToolTest ) ? TRUE : FALSE
		);
}

void CExtIconEditDlg::OnIedToolOutlinedEllipse() 
{
	ASSERT(
			CExtImageEditWnd::__IEDT_ELLIPSE_OUTLINED_VALUE_MIN <= m_eLastToolOutlinedEllipse
		&&	m_eLastToolOutlinedEllipse <= CExtImageEditWnd::__IEDT_ELLIPSE_OUTLINED_VALUE_MAX
		);
	m_wndIconEditor.IedToolSet( m_eLastToolOutlinedEllipse );
}
void CExtIconEditDlg::OnUpdateIedToolOutlinedEllipse(CCmdUI* pCmdUI) 
{
CExtImageEditWnd::e_tool_t eTool = m_wndIconEditor.IedToolGet();
	pCmdUI->SetCheck(
		(	CExtImageEditWnd::__IEDT_ELLIPSE_OUTLINED_VALUE_MIN <= eTool
		&&	eTool <= CExtImageEditWnd::__IEDT_ELLIPSE_OUTLINED_VALUE_MAX
		) ? TRUE : FALSE
		);
}
void CExtIconEditDlg::OnIedCmdRangeOutlinedEllipse( UINT nCmdID )
{
	ASSERT(
			ID_IED_TOOL_OUTLINED_ELLIPSE_SMALLER <= nCmdID
		&&	nCmdID <= ID_IED_TOOL_OUTLINED_ELLIPSE_LARGER
		);
	ASSERT( (ID_IED_TOOL_OUTLINED_ELLIPSE_LARGER - ID_IED_TOOL_OUTLINED_ELLIPSE_SMALLER) == 4 );
	m_eLastToolOutlinedEllipse = (CExtImageEditWnd::e_tool_t)
		(nCmdID - ID_IED_TOOL_OUTLINED_ELLIPSE_SMALLER + (UINT)CExtImageEditWnd::__IEDT_ELLIPSE_OUTLINED_VALUE_MIN);
	OnIedToolOutlinedEllipse();
}
void CExtIconEditDlg::OnUpdateIedToolOutlinedEllipseRange(CCmdUI* pCmdUI) 
{
	ASSERT(
			ID_IED_TOOL_OUTLINED_ELLIPSE_SMALLER <= pCmdUI->m_nID
		&&	pCmdUI->m_nID <= ID_IED_TOOL_OUTLINED_ELLIPSE_LARGER
		);
	ASSERT( (ID_IED_TOOL_OUTLINED_ELLIPSE_LARGER - ID_IED_TOOL_OUTLINED_ELLIPSE_SMALLER) == 4 );
CExtImageEditWnd::e_tool_t eToolTest = (CExtImageEditWnd::e_tool_t)
		(pCmdUI->m_nID - ID_IED_TOOL_OUTLINED_ELLIPSE_SMALLER + (UINT)CExtImageEditWnd::__IEDT_ELLIPSE_OUTLINED_VALUE_MIN);
	pCmdUI->SetCheck(
		( m_eLastToolOutlinedEllipse == eToolTest ) ? TRUE : FALSE
		);
}

void CExtIconEditDlg::OnIedToolFilledEllipse() 
{
	m_wndIconEditor.IedToolSet( CExtImageEditWnd::__IEDT_ELLIPSE_FILLED );
}
void CExtIconEditDlg::OnUpdateIedToolFilledEllipse(CCmdUI* pCmdUI) 
{
CExtImageEditWnd::e_tool_t eTool = m_wndIconEditor.IedToolGet();
	pCmdUI->SetCheck(
		(eTool == CExtImageEditWnd::__IEDT_ELLIPSE_FILLED) ? TRUE : FALSE
		);
}

void CExtIconEditDlg::OnIedToggleGrid() 
{
DWORD dwIedStyle = m_wndIconEditor.IedGetStyle();
	if( (dwIedStyle & __EIEDWS_GRID_LINES) != 0 )
		m_wndIconEditor.IedModifyStyle(
			__EIEDWS_GRID_LINES, 0, true );
	else
		m_wndIconEditor.IedModifyStyle(
			0, __EIEDWS_GRID_LINES, true );
}
void CExtIconEditDlg::OnUpdateIedToggleGrid(CCmdUI* pCmdUI) 
{
DWORD dwIedStyle = m_wndIconEditor.IedGetStyle();
	pCmdUI->SetCheck(
		( (dwIedStyle & __EIEDWS_GRID_LINES) != 0 ) ? TRUE : FALSE
		);
}

void CExtIconEditDlg::OnIedClear() 
{
	m_wndIconEditor.IedClear( false, true );
}

#endif // (!defined __EXT_MFC_NO_ICONEDITDLG)
