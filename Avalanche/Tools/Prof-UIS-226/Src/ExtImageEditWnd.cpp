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

#if (!defined __EXT_MFC_NO_IMAGEEDITWND)

#if (!defined __EXT_IMAGEEDITWND_H)
	#include <ExtImageEditWnd.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __EXT_LOCALIZATION_H)
	#include <../Src/ExtLocalization.h>
#endif

#include <../profuisdll/resource.h>

#ifndef __AFXADV_H__
	#include <AfxAdv.h>
#endif

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtImageEditWnd::IMAGEEDITDATA

void CExtImageEditWnd::IMAGEEDITDATA::Draw(
	CDC & dc,
	UINT nMagnification,
	CBitmap & bmpSelectionCache,
	CRect rcSelection,
	COLORREF clrCacheTransparent,
	bool bSeparatedCacheState
	)
{
	ASSERT( dc.GetSafeHdc() != NULL );
	if( IsEmpty() )
		return;
CDC dcCompat;
	if( !dcCompat.CreateCompatibleDC(&dc) )
	{
		ASSERT( FALSE );
		return;
	}
CSize _size = SizeGet();
	ASSERT( _size.cx > 0 && _size.cy > 0 );
CBitmap * pBmpOld = dcCompat.SelectObject( &m_bmpSurface );
	if( m_clrTransparency == __IED_COLORREF_NO_TRANSPARENCY )
	{
		dc.StretchBlt(
			0,0,
			_size.cx*nMagnification, _size.cy*nMagnification,
			&dcCompat,
			0, 0,
			_size.cx, _size.cy,
			SRCCOPY
			);
	} // if( m_clrTransparency == __IED_COLORREF_NO_TRANSPARENCY )
	else
	{
		CExtPaintManager::stat_TransparentBlt(
			dc.GetSafeHdc(),
			0,							// x-coord of destination upper-left corner
			0,							// y-coord of destination upper-left corner
			_size.cx*nMagnification,	// width of destination rectangle
			_size.cy*nMagnification,	// height of destination rectangle
			dcCompat.GetSafeHdc(),		// handle to source DC
			0,							// x-coord of source upper-left corner
			0,							// y-coord of source upper-left corner
			_size.cx,					// width of source rectangle
			_size.cy,					// height of source rectangle
			m_clrTransparency			// color to make transparent
			);
	} // else from if( m_clrTransparency == __IED_COLORREF_NO_TRANSPARENCY )

	if(		bSeparatedCacheState
		&&	bmpSelectionCache.GetSafeHandle() != NULL
		&&	(! rcSelection.IsRectEmpty() )
		)
	{
		CRect rcTarget(
			rcSelection.left * nMagnification,
			rcSelection.top * nMagnification,
			rcSelection.right * nMagnification,
			rcSelection.bottom * nMagnification
			);
		dcCompat.SelectObject( &bmpSelectionCache );
		if( clrCacheTransparent == ((COLORREF)(-1L)) )
		{
			VERIFY(
				dc.StretchBlt(
					rcTarget.left, rcTarget.top,
					rcTarget.Width(), rcTarget.Height(),
					&dcCompat,
					0, 0,
					rcSelection.Width(), rcSelection.Height(),
					SRCCOPY
					)
				);
		} // if( clrCacheTransparent == ((COLORREF)(-1L)) )
		else
		{
			CExtPaintManager::stat_TransparentBlt(
				dc.GetSafeHdc(),
				rcTarget.left,				// x-coord of destination upper-left corner
				rcTarget.top,				// y-coord of destination upper-left corner
				rcTarget.Width(),			// width of destination rectangle
				rcTarget.Height(),			// height of destination rectangle
				dcCompat.GetSafeHdc(),		// handle to source DC
				0,							// x-coord of source upper-left corner
				0,							// y-coord of source upper-left corner
				rcSelection.Width(),		// width of source rectangle
				rcSelection.Height(),		// height of source rectangle
				clrCacheTransparent			// color to make transparent
				);
		} // else from if( clrCacheTransparent == ((COLORREF)(-1L)) )
	}

	dcCompat.SelectObject( pBmpOld );
}

void CExtImageEditWnd::IMAGEEDITDATA::AttachBitmap( HBITMAP hBitmap )
{
	Empty();
	m_bmpSurface.Attach( hBitmap );
	m_bEmpty = false;
	SizeReset();
}

CSize CExtImageEditWnd::IMAGEEDITDATA::SizeGet() const
{
#ifdef _DEBUG
	if( m_bmpSurface.GetSafeHandle() == NULL )
	{
		ASSERT( m_sizeBmp.cx == 0 && m_sizeBmp.cy == 0 );
	}
	else
	{
		ASSERT( m_sizeBmp.cx != 0 && m_sizeBmp.cy != 0 );
	}
#endif // _DEBUG
	return m_sizeBmp;
}

void CExtImageEditWnd::IMAGEEDITDATA::SizeReset()
{
	if( m_bmpSurface.GetSafeHandle() == NULL )
	{
		m_sizeBmp.cx = m_sizeBmp.cy = 0;
		return;
	}
BITMAP _bmpInfo;
	if( !((CBitmap *)&m_bmpSurface)->GetBitmap( &_bmpInfo ) )
	{
		ASSERT( FALSE );
		m_sizeBmp.cx = m_sizeBmp.cy = 0;
		return;
	}
	m_sizeBmp.cx = _bmpInfo.bmWidth;
	m_sizeBmp.cy = _bmpInfo.bmHeight;
}

bool CExtImageEditWnd::IMAGEEDITDATA::SizeSet(
	CSize _size,
	CBrush & brushFillSurface
	)
{
	if( brushFillSurface.GetSafeHandle() == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	if( IsEmpty() )
	{
		ASSERT( FALSE );
		return false;
	}
	ASSERT( m_bmpSurface.GetSafeHandle() != NULL );
	if( !CExtPaintManager::stat_ResizeBitmap(
			m_bmpSurface,
			_size,
			brushFillSurface
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	SizeReset();
	return true;
}

bool CExtImageEditWnd::IMAGEEDITDATA::AssignFromOther(
	const CExtImageEditWnd::IMAGEEDITDATA & other,
	bool bAssignUndoType
	)
{
LPVOID lpvThis = (LPVOID)this;
LPVOID lpvOther = (LPVOID)(&other);
	if( lpvThis == lpvOther )
		return true;
	Empty();
	if( other.m_bEmpty )
		return true;
	ASSERT( other.m_bmpSurface.GetSafeHandle() != NULL );
	if( !CExtPaintManager::stat_CloneBitmap( m_bmpSurface, other.m_bmpSurface ) )
	{
		ASSERT( FALSE );
		return false;
	}
	ASSERT( m_bmpSurface.GetSafeHandle() != NULL );
	m_clrTransparency = other.m_clrTransparency;
	if( bAssignUndoType )
		m_nUndoType = other.m_nUndoType;
	m_bEmpty = false;
	SizeReset();
	return true;
}

void CExtImageEditWnd::IMAGEEDITDATA::MoveFromOther(
	CExtImageEditWnd::IMAGEEDITDATA &other,
	bool bMoveUndoType
	)
{
	Empty();
	if( other.m_bEmpty )
		return;
	m_bEmpty = false;
	m_bmpSurface.Attach( other.m_bmpSurface.Detach() );
	if( bMoveUndoType )
		m_nUndoType = other.m_nUndoType;
	ASSERT( m_bmpSurface.GetSafeHandle() != NULL );
	SizeReset();
}

bool CExtImageEditWnd::IMAGEEDITDATA::IsEmpty() const
{
#ifdef _DEBUG
	if( m_bEmpty )
	{
		ASSERT( m_bmpSurface.GetSafeHandle() == NULL );
		ASSERT( m_sizeBmp.cx == 0 && m_sizeBmp.cy == 0 );
	}
	else
	{
		ASSERT( m_bmpSurface.GetSafeHandle() != NULL );
		ASSERT( m_sizeBmp.cx > 0 && m_sizeBmp.cy > 0 );
	}
#endif // _DEBUG
	return m_bEmpty;
}

void CExtImageEditWnd::IMAGEEDITDATA::Empty()
{
	if( m_bmpSurface.GetSafeHandle() != NULL )
	{
		m_bmpSurface.DeleteObject();
		ASSERT( m_bmpSurface.GetSafeHandle() == NULL );
		m_sizeBmp.cx = m_sizeBmp.cy = 0;
	}
	m_bEmpty = true;
}

CExtImageEditWnd::IMAGEEDITDATA::IMAGEEDITDATA()
	: m_bEmpty( true )
	, m_sizeBmp( 0, 0 )
	, m_clrTransparency( __IED_COLORREF_NO_TRANSPARENCY )
	, m_nUndoType( CExtImageEditWnd::__EUT_UNKNOWN_OPERATION )
{
}

CExtImageEditWnd::IMAGEEDITDATA::IMAGEEDITDATA(
	const IMAGEEDITDATA & other
	)
	: m_bEmpty( true )
	, m_sizeBmp( 0, 0 )
	, m_clrTransparency( __IED_COLORREF_NO_TRANSPARENCY )
	, m_nUndoType( CExtImageEditWnd::__EUT_UNKNOWN_OPERATION )
{
	VERIFY( AssignFromOther( other, false ) );
}

CExtImageEditWnd::IMAGEEDITDATA::~IMAGEEDITDATA()
{
	Empty();
}

CExtImageEditWnd::IMAGEEDITDATA & CExtImageEditWnd::IMAGEEDITDATA::operator=( const IMAGEEDITDATA & other )
{
	VERIFY( AssignFromOther( other, false ) );
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
// CExtImageEditWnd

IMPLEMENT_DYNCREATE( CExtImageEditWnd, CExtScrollWnd );

static const UINT g_nMagnificationMin = 1;
static const UINT g_nMagnificationMax = 16;

CExtImageEditWnd::CExtImageEditWnd()
	: m_dwIedStyles( __EIEDWS_DEFAULT_STYLES )
	, m_eToolSelected( __IEDT_RECT_SELECTION )
	, m_eToolSelPrev( __IEDT_RECT_SELECTION )
	, m_eToolTracked( __IEDT_NONE )
	, m_nMagnification( 1 )
	, m_nTrackStep( 0 )
	, m_bCancelingActions( false )
	, m_rcSelection( 0, 0, 0, 0 )
	, m_rcSelectionTS( 0, 0, 0, 0 )
	, m_rcTrackingSel( 0, 0, 0, 0 )
	, m_clrForeground( RGB(0,0,0) )
	, m_clrBackground( RGB(255,255,255) )
	, m_nUndoDepthMax( 64 )
	, m_nUndoPos( 0 )
	, m_nGripToolHT( -1 )
	, m_sizeResizingCache( 0, 0 )
	, m_ptPicMoveLastOffs( 0, 0 )
	, m_clrLastCacheTransparency( ((COLORREF)(-1L)) )
	, m_bSeparatedCacheState( false )
	, m_bPresubclassDialogMode( true )
	, m_bEraseBkBeforeDraggingSelection( true )
	, m_bEraseBkBeforeResizingSelection( true )
{
	VERIFY( RegisterImageEditWndClass() );
	m_arrImgEditData.Add( new IMAGEEDITDATA );
}

CExtImageEditWnd::~CExtImageEditWnd()
{
	for(	INT nImgIdx = 0;
			nImgIdx < m_arrImgEditData.GetSize();
			nImgIdx++
			)
	{
		IMAGEEDITDATA * pImgEditData =
			m_arrImgEditData[nImgIdx];
		ASSERT( pImgEditData != NULL );
		delete pImgEditData;
	}
	m_arrImgEditData.RemoveAll();
}

bool CExtImageEditWnd::g_bImageEditWndClassRegistered = false;
HCURSOR CExtImageEditWnd::g_hCursor = NULL;

bool CExtImageEditWnd::RegisterImageEditWndClass()
{
	if( g_bImageEditWndClassRegistered )
		return true;

WNDCLASS wndclass;
HINSTANCE hInst = AfxGetInstanceHandle();
	if( ! ::GetClassInfo(
			hInst,
			__EXT_IMAGE_EDIT_WND_CLASS_NAME,
			&wndclass
			)
		)
	{
		// otherwise we need to register a new class
		wndclass.style = CS_GLOBALCLASS|CS_DBLCLKS;
		wndclass.lpfnWndProc = ::DefWindowProc;
		wndclass.cbClsExtra = wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInst;
		wndclass.hIcon = NULL;
		wndclass.hCursor =
			( g_hCursor != NULL )
				? g_hCursor
				: ( ::LoadCursor( NULL, IDC_CROSS ) )
				;
		ASSERT( wndclass.hCursor != NULL );
		wndclass.hbrBackground = NULL; 
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = __EXT_IMAGE_EDIT_WND_CLASS_NAME;
		if( !::AfxRegisterClass( &wndclass ) )
		{
			ASSERT( FALSE );
			//AfxThrowResourceException();
			return false;
		}
	}

	g_bImageEditWndClassRegistered = true;
	return true;
}

bool CExtImageEditWnd::Create(
	CWnd * pWndParent,
	DWORD dwIedStyles, // = __EIEDWS_DEFAULT_STYLES
	DWORD dwStyle, // = WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS
	DWORD dwExStyle, // = 0L,
	UINT nDlgCtrlID, // = ((UINT)(IDC_STATIC))
	__EXT_MFC_SAFE_LPCTSTR strWindowText, // = NULL
	CRect rc // = CRect( 0, 0, 0, 0 )
	)
{
	if( !RegisterImageEditWndClass() )
	{
		ASSERT( FALSE );
		return false;
	}
	m_dwIedStyles = dwIedStyles;
	m_bPresubclassDialogMode = false;
	if( !CWnd::CreateEx(
			dwExStyle,
			__EXT_IMAGE_EDIT_WND_CLASS_NAME,
			(strWindowText == NULL) ? _T("") : strWindowText,
			dwStyle,
			rc,
			pWndParent,
			nDlgCtrlID
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	return true;
}

BEGIN_MESSAGE_MAP( CExtImageEditWnd, CExtScrollWnd )
	//{{AFX_MSG_MAP(CExtImageEditWnd)
	ON_WM_CANCELMODE()
	ON_WM_CAPTURECHANGED()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_CONTEXTMENU()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_SETCURSOR()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

bool CExtImageEditWnd::OnSwDoMouseWheel(
	UINT fFlags,
	short zDelta,
	CPoint point
	)
{
	ASSERT_VALID( this );
	if( (IedGetStyle() & __EIEDWS_NO_MAGNIFY) != 0 )
		return true;
	if( m_nTrackStep > 0 )
		return true;
	if( fFlags & MK_CONTROL )
	{
		UINT nMagnification = m_nMagnification;
		nMagnification +=
			(zDelta < 0) ? 1 : (-1);
		if( nMagnification < g_nMagnificationMin )
			nMagnification = g_nMagnificationMin;
		else if( nMagnification > g_nMagnificationMax )
			nMagnification = g_nMagnificationMax;
		if( nMagnification != m_nMagnification )
			IedMagnificationSet( nMagnification, true );
		return true;
	} // if( fFlags & MK_CONTROL )
	return
		CExtScrollWnd::OnSwDoMouseWheel( fFlags, zDelta, point );
}

CSize CExtImageEditWnd::OnSwGetTotalSize() const
{
	ASSERT_VALID( this );
CSize sizeTotal = IedSizeGet();
	sizeTotal.cx *= (LONG)m_nMagnification;
	sizeTotal.cy *= (LONG)m_nMagnification;
CSize _sizeExt =
		( (IedGetStyle() & __EIEDWS_OUTER_AREA) != 0 )
			? OnIedMeasureToolSize()
			: CSize( 0, 0 )
			;
	sizeTotal.cx += _sizeExt.cx*2;
	sizeTotal.cy += _sizeExt.cy*2;
	return sizeTotal;
}

CSize CExtImageEditWnd::OnSwGetPageSize( int nDirection ) const
{
	ASSERT_VALID( this );
	nDirection;
CRect rcClient = OnSwGetClientRect();
CSize sizePage = rcClient.Size();
CSize sizeTotal = IedSizeGet();
	sizeTotal.cx *= (LONG)m_nMagnification;
	sizeTotal.cy *= (LONG)m_nMagnification;
	if( sizePage.cx > sizeTotal.cx )
		sizePage.cx = 0;
	if( sizePage.cy > sizeTotal.cy )
		sizePage.cy = 0;
	sizePage.cx -= sizePage.cx % ((LONG)m_nMagnification);
	sizePage.cy -= sizePage.cy % ((LONG)m_nMagnification);
	return sizePage;
}

CSize CExtImageEditWnd::OnSwGetLineSize( int nDirection ) const
{
	ASSERT_VALID( this );
	nDirection;
	return CSize( (LONG)m_nMagnification, (LONG)m_nMagnification );
}

CSize CExtImageEditWnd::IedSizeGet() const
{
	ASSERT_VALID( this );
	return m_arrImgEditData[m_nUndoPos]->SizeGet();
}

bool CExtImageEditWnd::IedSizeSet(
	CSize _size,
	CBrush & brushFillSurface,
	bool bRedraw
	)
{
	ASSERT_VALID( this );
	if( _size == IedSizeGet() )
		return true;
	IedPushUndo( __EUT_RESIZING_IMAGE );
	if( !m_arrImgEditData[m_nUndoPos]->SizeSet(
			_size,
			brushFillSurface
			)
		)
		return false;

	IedUndo( 0 );
	
	if( bRedraw )
		_RedrawWnd();
	return true;
}

void CExtImageEditWnd::_RequestFocus()
{
	ASSERT_VALID( this );
	if( (IedGetStyle() & __EIEDWS_REQUEST_FOCUS) == 0 )
		return;
HWND hWndFocus = ::GetFocus();
	if( hWndFocus == m_hWnd )
		return;
	if(		hWndFocus == NULL
		||	(! ::IsChild(m_hWnd,hWndFocus) )
		)
		SetFocus();
}

void CExtImageEditWnd::_RedrawWnd()
{
	ASSERT_VALID( this );
	Invalidate();
	OnSwUpdateScrollBars();
	UpdateWindow();
}

bool CExtImageEditWnd::IedIsTransparencyEnabled() const
{
	ASSERT_VALID( this );
	return (m_arrImgEditData[m_nUndoPos]->TransparencyColorGet() == __IED_COLORREF_NO_TRANSPARENCY) ? false : true;
}

COLORREF CExtImageEditWnd::IedGetTransparencyColor() const
{
	ASSERT_VALID( this );
	return m_arrImgEditData[m_nUndoPos]->TransparencyColorGet();
}

void CExtImageEditWnd::IedEnableTransparency(
	COLORREF clrTransparency // = __IED_COLORREF_NO_TRANSPARENCY // disable
	)
{
	ASSERT_VALID( this );
	if( m_arrImgEditData[m_nUndoPos]->TransparencyColorGet() == clrTransparency )
		return;
	if( m_arrImgEditData[m_nUndoPos]->IsEmpty() )
	{
		m_arrImgEditData[m_nUndoPos]->TransparencyColorSet( clrTransparency );
		return;
	}

	IedPushUndo( __EUT_TRANSPARENCY_MODE );
	m_arrImgEditData[m_nUndoPos]->TransparencyColorSet( clrTransparency );
	
	IedUndo( 0 );
	
	_RedrawWnd();
}

DWORD CExtImageEditWnd::IedGetStyle() const
{
	ASSERT_VALID( this );
	return m_dwIedStyles;
}

DWORD CExtImageEditWnd::IedModifyStyle(
	DWORD dwStyleRemove,
	DWORD dwStyleAdd,
	bool bRepaint
	)
{
	ASSERT_VALID( this );
DWORD dwOldStyles = m_dwIedStyles;
	m_dwIedStyles &= ~dwStyleRemove;
	m_dwIedStyles |= dwStyleAdd;
	if( bRepaint )
		_RedrawWnd();
	return dwOldStyles;
}

bool CExtImageEditWnd::IedIsEmpty() const
{
	ASSERT_VALID( this );
	return m_arrImgEditData[m_nUndoPos]->IsEmpty();
}

INT CExtImageEditWnd::IedUndoTypeGet(
	INT nIndex // = -1 // -1 - current position
	) const
{
	ASSERT_VALID( this );
	if( nIndex < 0 )
		nIndex = m_nUndoPos;
	ASSERT( 0 <= nIndex && nIndex <= m_arrImgEditData.GetSize() );
	return m_arrImgEditData[nIndex]->UndoTypeGet();
}

INT CExtImageEditWnd::IedUndoPosGet() const
{
	ASSERT_VALID( this );
	return m_nUndoPos;
}

INT CExtImageEditWnd::IedUndoDepthGet() const
{
	ASSERT_VALID( this );
INT nDepth = m_arrImgEditData.GetSize() - 1;
	ASSERT( nDepth >= m_nUndoPos && m_nUndoPos >= 0 );
	return nDepth - m_nUndoPos;
}

INT CExtImageEditWnd::IedUndoDepthMaxGet() const
{
	ASSERT_VALID( this );
	ASSERT( m_nUndoDepthMax > 1 );
	return m_nUndoDepthMax-1;
}

void CExtImageEditWnd::IedUndoDepthMaxSet(
	INT nUndoDepthMax // = 1 // 1 - disable undo
	)
{
	ASSERT_VALID( this );
	if( nUndoDepthMax < 1 )
		nUndoDepthMax = 1;
	nUndoDepthMax++;
	m_nUndoDepthMax = nUndoDepthMax;
INT nDepth = m_arrImgEditData.GetSize();
	if( nDepth <= m_nUndoDepthMax )
		return;
	for(	INT nImgIdx = m_nUndoDepthMax;
			nImgIdx < nDepth;
			nImgIdx++
			)
	{
		IMAGEEDITDATA * pImgEditData =
			m_arrImgEditData[nImgIdx];
		ASSERT( pImgEditData != NULL );
		delete pImgEditData;
	}
	m_arrImgEditData.RemoveAt(
		m_nUndoDepthMax,
		m_nUndoDepthMax - nDepth
		);
	if( m_nUndoPos >= m_nUndoDepthMax )
		m_nUndoPos = m_nUndoDepthMax-1;
	ASSERT_VALID( this );
}

void CExtImageEditWnd::_EmptyCache()
{
	ASSERT_VALID( this );
	if( m_bmpResizingCache.GetSafeHandle() != NULL )
	{
		CBitmap	& bmpSurface =
			m_arrImgEditData[m_nUndoPos]->GetBmpSurface();
		if(		m_bSeparatedCacheState
			&&	(! m_rcSelection.IsRectEmpty() )
			&&	bmpSurface.GetSafeHandle() != NULL
			)
		{
			CSurfaceDC dcDst( bmpSurface );
			CSurfaceDC dcSrc( m_bmpResizingCache );
			if( m_clrLastCacheTransparency == __IED_COLORREF_NO_TRANSPARENCY )
			{
				VERIFY(
					dcDst.StretchBlt(
						m_rcSelection.left,
						m_rcSelection.top,
						m_rcSelection.Width(),
						m_rcSelection.Height(),
						&dcSrc,
						0,
						0,
						m_sizeResizingCache.cx,
						m_sizeResizingCache.cy,
						SRCCOPY
						)
					);
			} // if( m_clrLastCacheTransparency == __IED_COLORREF_NO_TRANSPARENCY )
			else
			{
					CExtPaintManager::stat_TransparentBlt(
						dcDst.GetSafeHdc(),
						m_rcSelection.left,
						m_rcSelection.top,
						m_rcSelection.Width(),
						m_rcSelection.Height(),
						dcSrc.GetSafeHdc(),
						0,
						0,
						m_sizeResizingCache.cx,
						m_sizeResizingCache.cy,
						m_clrLastCacheTransparency
						);
			} // else from if( m_clrLastCacheTransparency == __IED_COLORREF_NO_TRANSPARENCY )

			Invalidate();
		} // if( m_bSeparatedCacheState ..
		m_bmpResizingCache.DeleteObject();
		m_sizeResizingCache.cx = m_sizeResizingCache.cy = 0;
		
		IedUndo( 0 );

	} // if( m_bmpResizingCache.GetSafeHandle() != NULL )
	if( !m_rcSelection.IsRectEmpty() )
	{
		m_rcSelection.SetRectEmpty();
		OnIedRectChange( m_rcSelection, false );
	} // if( !m_rcSelection.IsRectEmpty() )
	m_bSeparatedCacheState = false;
	m_rcSelectionTS.SetRectEmpty();
}

INT CExtImageEditWnd::IedUndo( // returns positive number of undo - redo steps
	INT nSteps // if nSteps < 0 - redo
	)
{
	ASSERT_VALID( this );
	if( nSteps == 0 )
		return 0;
	SendMessage( WM_CANCELMODE );
INT nDepth = m_arrImgEditData.GetSize();
	ASSERT( 0 <= m_nUndoPos && m_nUndoPos < nDepth );
	if( nSteps < 0 )
	{
		nSteps = -nSteps;
		if( nSteps > m_nUndoPos )
			nSteps = m_nUndoPos;
		if( nSteps == 0 )
			return 0;
		_EmptyCache();
		m_nUndoPos -= nSteps;
		ASSERT_VALID( this );
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
		return -nSteps;
	}
int nDist =  nDepth - m_nUndoPos - 1;
	if( nSteps > nDist )
		nSteps = nDist;
	if( nSteps == 0 )
		return 0;
	_EmptyCache();
	m_nUndoPos += nSteps;
	ASSERT( 0 <= m_nUndoPos && m_nUndoPos < nDepth );
	ASSERT_VALID( this );
	OnSwUpdateScrollBars();
	OnSwDoRedraw();
	return nDepth;
}

void CExtImageEditWnd::IedPushUndo(
	INT eUndoType
	)
{
	ASSERT_VALID( this );
int nDepth = m_arrImgEditData.GetSize();
	ASSERT( 0 <= m_nUndoPos && m_nUndoPos < nDepth );
	for( int x = 0; x < m_nUndoPos; x++ )
	{
		IMAGEEDITDATA * pImgEditData = m_arrImgEditData[ 0 ];
		ASSERT( pImgEditData != NULL );
		ASSERT( pImgEditData->GetBmpSurface().GetSafeHandle() != NULL );
		delete pImgEditData;
		m_arrImgEditData.RemoveAt( 0, 1 );
	}
	m_nUndoPos = 0;
	ASSERT_VALID( this );
	nDepth = m_arrImgEditData.GetSize();
	ASSERT( nDepth <= m_nUndoDepthMax );
	if( nDepth == m_nUndoDepthMax
		&& m_nUndoDepthMax > 1
		)
	{
		IMAGEEDITDATA * pImgEditData = m_arrImgEditData[ nDepth - 1 ];
		ASSERT( pImgEditData != NULL );
		ASSERT( pImgEditData->GetBmpSurface().GetSafeHandle() != NULL );
		delete pImgEditData;
		m_arrImgEditData.RemoveAt( nDepth - 1, 1 );
	}
IMAGEEDITDATA * pImgEditData = m_arrImgEditData[0];
	ASSERT( pImgEditData != NULL );
	ASSERT( pImgEditData->GetBmpSurface().GetSafeHandle() != NULL );
	pImgEditData = new IMAGEEDITDATA( *pImgEditData );
	pImgEditData->UndoTypeSet( eUndoType );
	m_arrImgEditData.InsertAt( 0, pImgEditData );
	if( m_nUndoDepthMax == 1 )
	{
		ASSERT( m_arrImgEditData.GetSize() == 2 );
		IMAGEEDITDATA * pImgEditData = m_arrImgEditData[ 1 ];
		ASSERT( pImgEditData != NULL );
		ASSERT( pImgEditData->GetBmpSurface().GetSafeHandle() != NULL );
		delete pImgEditData;
		m_arrImgEditData.RemoveAt( 1, 1 );
	}
	ASSERT_VALID( this );
	m_rcSelectionTS.SetRectEmpty();
}

void CExtImageEditWnd::IedEmptyUndoBuffer()
{
	ASSERT_VALID( this );
INT nDepth = m_arrImgEditData.GetSize();
	for( INT nImgIdx = 1; nImgIdx < nDepth; nImgIdx++ )
	{
		IMAGEEDITDATA * pImgEditData =
			m_arrImgEditData[nImgIdx];
		ASSERT( pImgEditData != NULL );
		delete pImgEditData;
	}
	m_arrImgEditData.RemoveAt(
		1,
		nDepth-1
		);
	m_nUndoPos = 0;
	ASSERT_VALID( this );
}

void CExtImageEditWnd::IedEmpty( bool bRedraw )
{
	ASSERT_VALID( this );
	_EmptyCache();
	IedEmptyUndoBuffer();
	ASSERT_VALID( this );
	m_arrImgEditData[m_nUndoPos]->Empty();
	if( bRedraw )
		_RedrawWnd();
}

HBITMAP CExtImageEditWnd::IedBitmapGet()
{
	ASSERT_VALID( this );
HBITMAP hBitmap = (HBITMAP)
		m_arrImgEditData[m_nUndoPos]->
			GetBmpSurface().GetSafeHandle();
	ASSERT( hBitmap != NULL );
	return hBitmap;
}

bool CExtImageEditWnd::IedCopyToClipboard(
	bool bSelectionOnly // = false
	) const
{
	ASSERT_VALID( this );
	if( !IedCanCopy( bSelectionOnly ) )
		return false;
HBITMAP hBitmap = IedSelectionGetHBITMAP();
	if( hBitmap == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	if( ! ::OpenClipboard( GetSafeHwnd() ) )
	{
		ASSERT( FALSE );
		return false;
	}
	if( ! ::EmptyClipboard() )
	{
		ASSERT( FALSE );
		VERIFY( ::CloseClipboard() );
		return false;
	}
HGLOBAL	hGlobal =
		CExtPaintManager::stat_HBITMAPtoHGLOBAL(
			hBitmap
			);
	if( hGlobal == NULL )
	{
		ASSERT( FALSE );
		VERIFY( ::CloseClipboard() );
		return false;
	}
	if( ::SetClipboardData( CF_DIB, hGlobal ) == NULL )
	{
		ASSERT( FALSE );
		VERIFY( ::CloseClipboard() );
		return false;
	}
	VERIFY( ::CloseClipboard() );
	return true;
}

bool CExtImageEditWnd::IedCutToClipboard(
	bool bSelectionOnly, // = false
	bool bRedrawWnd // = true
	)
{
	if( !IedCanCut( bSelectionOnly ) )
		return false;
HBITMAP hBitmap = IedSelectionGetHBITMAP();
	if( hBitmap == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	if( ! ::OpenClipboard( GetSafeHwnd() ) )
	{
		ASSERT( FALSE );
		return false;
	}
	if( ! ::EmptyClipboard() )
	{
		ASSERT( FALSE );
		VERIFY( ::CloseClipboard() );
		return false;
	}
HGLOBAL	hGlobal =
		CExtPaintManager::stat_HBITMAPtoHGLOBAL( hBitmap );
	if( hGlobal == NULL )
	{
		ASSERT( FALSE );
		VERIFY( ::CloseClipboard() );
		return false;
	}
	::SetClipboardData( CF_DIB, hGlobal );
	VERIFY( ::CloseClipboard() );

CRect rcSelection = m_rcSelection;
	if( rcSelection.IsRectEmpty() )
	{
		rcSelection = CRect( CPoint(0,0), IedSizeGet() );
		ASSERT( !rcSelection.IsRectEmpty() );
		IedPushUndo( __EUT_IMAGE_CUT );
	} // if( rcSelection.IsRectEmpty() )
	else
		IedPushUndo( __EUT_SELECTION_CUT );

	_EmptyCache();
	{ // BLOCK: surface DC
		CSurfaceDC dc( m_arrImgEditData[m_nUndoPos]->GetBmpSurface() );
		dc.FillSolidRect( &rcSelection, IedColorBackgroundGet() );
	} // BLOCK: surface DC

	IedUndo( 0 );
	
	if( bRedrawWnd )
	{
		if( (IedGetStyle() & __EIEDWS_OUTER_TOOLS) == 0 )
		{
			rcSelection = OnIedPicToClient( rcSelection, false );
			rcSelection.InflateRect( OnIedMeasureToolSize() );
			rcSelection.InflateRect( 2, 2 );
			InvalidateRect( &rcSelection );
		} // if( (IedGetStyle() & __EIEDWS_OUTER_TOOLS) == 0 )
		else
			Invalidate();
		UpdateWindow();
	} // if( bRedrawWnd )
	return true;
}

bool CExtImageEditWnd::IedPasteFromClipboard(
	bool bRedrawWnd // = true
	)
{
	ASSERT_VALID( this );
	if( !IedCanPaste() )
		return false;
	SendMessage( WM_CANCELMODE );
	if( m_eToolSelected != __IEDT_RECT_SELECTION )
		IedToolSet( __IEDT_RECT_SELECTION );
	if( m_eToolSelected != __IEDT_RECT_SELECTION )
		return false;
	if( ! ::OpenClipboard( GetSafeHwnd() ) )
	{
		ASSERT( FALSE );
		return false;
	}
HBITMAP hBmpClipboard = (HBITMAP)
		::GetClipboardData( CF_BITMAP );
	if( hBmpClipboard == NULL )
	{
		ASSERT( FALSE );
		VERIFY( ::CloseClipboard());
		return false;
	}
HBITMAP hBmp =
		CExtPaintManager::stat_CloneBitmap(
			hBmpClipboard
			);
	VERIFY( ::CloseClipboard());
	if( hBmp == NULL )
	{
		ASSERT( FALSE );
		return false;
	}

	IedPushUndo( __EUT_PASTE );
	_EmptyCache();
	m_bmpResizingCache.Attach( hBmp );
	m_bSeparatedCacheState = true;

BITMAP _bmpInfo;
	VERIFY( m_bmpResizingCache.GetBitmap(&_bmpInfo) );
	ASSERT( _bmpInfo.bmWidth > 0 && _bmpInfo.bmHeight > 0 );
	m_sizeResizingCache.cx = _bmpInfo.bmWidth;
	m_sizeResizingCache.cy = _bmpInfo.bmHeight;
	m_rcSelection =
		CRect(
			OnIedClientToPic( CPoint( 0, 0 ), true ),
			m_sizeResizingCache
			);

	if( IedIsTransparencyEnabled() )
	{
		COLORREF clrTransparentMode = IedGetTransparencyColor();
		ASSERT( clrTransparentMode != __IED_COLORREF_NO_TRANSPARENCY );
		m_clrLastCacheTransparency = clrTransparentMode;
	} // if( IedIsTransparencyEnabled() )
	else
	{
		COLORREF clrTransparentMode = IedColorBackgroundGet();
		if( clrTransparentMode != __IED_COLORREF_NO_TRANSPARENCY )
			m_clrLastCacheTransparency = clrTransparentMode;
	} // else from if( IedIsTransparencyEnabled() )

	IedUndo( 0 );
	OnIedRectChange( m_rcSelection, false );

	if( bRedrawWnd )
	{
		Invalidate();
		UpdateWindow();
	} // if( bRedrawWnd )

	return true;
}

bool CExtImageEditWnd::IedCanCopy(
	bool bSelectionOnly // = false
	) const
{
	ASSERT_VALID( this );
HBITMAP hBitmap = (HBITMAP)
		m_arrImgEditData[m_nUndoPos]->
			GetBmpSurface().GetSafeHandle();
	if( hBitmap == NULL )
		return false;
	if( !bSelectionOnly )
		return true;
	if( m_rcSelection.IsRectEmpty() )
		return false;
	return true;
}

bool CExtImageEditWnd::IedCanCut(
	bool bSelectionOnly // = false
	) const
{
	ASSERT_VALID( this );
	if( (IedGetStyle() & __EIEDWS_READONLY) != 0 )
		return false;
	return IedCanCopy( bSelectionOnly );
}

bool CExtImageEditWnd::IedCanPaste() const
{
	ASSERT_VALID( this );
	if( (IedGetStyle() & __EIEDWS_READONLY) != 0 )
		return false;
	if(		::IsClipboardFormatAvailable( CF_BITMAP )
		||	::IsClipboardFormatAvailable( CF_DIB )
		)
		return true;
	return false;
}

HBITMAP CExtImageEditWnd::IedSelectionGetHBITMAP(
	bool bSelectionOnly, // = false
	bool bReturnClone // = false
	) const
{
	ASSERT_VALID( this );
HBITMAP hBitmap = (HBITMAP)
		m_arrImgEditData[m_nUndoPos]->
			GetBmpSurface().GetSafeHandle();
	if( hBitmap == NULL )
		return NULL;
	if( m_rcSelection.IsRectEmpty() )
	{
		if( bSelectionOnly )
			return NULL;
	} // if( m_rcSelection.IsRectEmpty() )
	else
	{
		if( m_bmpResizingCache.GetSafeHandle() == NULL )
		{
			CSize _sizeSelection = m_rcSelection.Size();
			if( !CExtPaintManager::stat_CloneBitmap(
					(CBitmap &)( m_bmpResizingCache ),
					m_arrImgEditData[m_nUndoPos]->GetBmpSurface(),
					&m_rcSelection
					)
				)
			{
				ASSERT( FALSE );
				return NULL;
			}
			CSize & _sizeMutableRef = (CSize &) m_sizeResizingCache;
			_sizeMutableRef = _sizeSelection;
		} // if( m_bmpResizingCache.GetSafeHandle() == NULL )
		hBitmap = (HBITMAP)m_bmpResizingCache.GetSafeHandle();
	} // else from m_rcSelection.IsRectEmpty() )
	ASSERT( hBitmap != NULL );
	if( !bReturnClone )
		return hBitmap;
	hBitmap =
		CExtPaintManager::stat_CloneBitmap( hBitmap, NULL );
	ASSERT( hBitmap != NULL );
	return hBitmap;
}

CRect CExtImageEditWnd::IedSelectionRectGet(
	bool * p_bAllImage, // = NULL
	bool * p_bSelectionExist // = NULL
	) const
{
	ASSERT_VALID( this );
	if( p_bAllImage != NULL )
		*p_bAllImage = true;
	if( p_bSelectionExist != NULL )
		*p_bSelectionExist = false;
CRect rcSelection( CPoint(0,0), IedSizeGet() );
	if( !m_rcSelection.IsRectEmpty() )
	{
		if( p_bSelectionExist != NULL )
			*p_bSelectionExist = true;
		if(		p_bAllImage != NULL
			&&	rcSelection != m_rcSelection
			)
			*p_bAllImage = false;
		rcSelection = m_rcSelection;
	} // if( !m_rcSelection.IsRectEmpty() )
	return rcSelection;
}

void CExtImageEditWnd::IedSelectionRectSet(
	LPCRECT pRcSelection, // = NULL // NULL - select all
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );

	OnIedCancelActions( false );

CSize _sizeImage = IedSizeGet();
CRect rcSelection( 0, 0, _sizeImage.cx, _sizeImage.cy );
	if( pRcSelection != NULL )
	{
		rcSelection = *pRcSelection;
		rcSelection.NormalizeRect();
		if( rcSelection.left < 0 )
			rcSelection.left = 0;
		if( rcSelection.top < 0 )
			rcSelection.top = 0;
		if( rcSelection.right > _sizeImage.cx )
			rcSelection.right = _sizeImage.cx;
		if( rcSelection.bottom > _sizeImage.cy )
			rcSelection.bottom = _sizeImage.cy;
	} // if( pRcSelection != NULL )

	m_rcSelection = rcSelection;
	m_rcSelectionTS.SetRectEmpty();

	if( bRedraw )
	{
		Invalidate();
		UpdateWindow();
	}
}

void CExtImageEditWnd::IedBitmapSet(
	HBITMAP hBitmap,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	if( hBitmap == NULL )
	{
		ASSERT( FALSE );
		return;
	}
	IedEmpty( false );
	m_arrImgEditData[m_nUndoPos]->AttachBitmap( hBitmap );
	ASSERT_VALID( this );
	if( bRedraw )
		_RedrawWnd();
}

bool CExtImageEditWnd::IedBitmapLoadResource(
	UINT nResourceID,
	bool bRedraw // = true
	)
{
	return
		IedBitmapLoadResource(
			__EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE(nResourceID) ),
			bRedraw
			);
}

bool CExtImageEditWnd::IedBitmapLoadResource(
	__EXT_MFC_SAFE_LPCTSTR sResourceID,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
HBITMAP hBmp =
		CExtPaintManager::stat_LoadBitmapResource( sResourceID );
	if( hBmp == NULL )
	{
		ASSERT( FALSE );
		return false;
	} // if( hBmp == NULL )
	IedBitmapSet( hBmp, bRedraw );
	return true;
}

UINT CExtImageEditWnd::IedMagnificationGet() const
{
	ASSERT_VALID( this );
	return m_nMagnification;
}

void CExtImageEditWnd::IedMagnificationSet( UINT nMagnification, bool bRedraw )
{
	ASSERT_VALID( this );
	ASSERT( g_nMagnificationMin <= nMagnification && nMagnification <= g_nMagnificationMax );
	if( g_nMagnificationMin > nMagnification )
		nMagnification = g_nMagnificationMin;
	else if( g_nMagnificationMax < nMagnification )
		nMagnification = g_nMagnificationMax;
	if( m_nMagnification == nMagnification )
		return;
	m_nMagnification = nMagnification;
	if( bRedraw )
		_RedrawWnd();
}

CExtImageEditWnd::e_tool_t CExtImageEditWnd::IedToolGet() const
{
	ASSERT_VALID( this );
	return m_eToolSelected;
}

CExtImageEditWnd::e_tool_t CExtImageEditWnd::IedToolSet(
	CExtImageEditWnd::e_tool_t eTool // = CExtImageEditWnd::__IEDT_RECT_SELECTION
	)
{
	ASSERT_VALID( this );

	if(		GetSafeHwnd() != NULL
		&&	::IsWindow( GetSafeHwnd() )
		)
		SendMessage( WM_CANCELMODE );
	if( m_eToolSelected == eTool )
		return m_eToolSelected;
	if(		eTool != __IEDT_MAGNIFICATON
		&&	(! m_rcSelection.IsRectEmpty() )
		)
	{
		_EmptyCache();
		Invalidate();
	}
	m_eToolSelPrev = m_eToolSelected;
	m_eToolSelected = eTool;
	if( m_eToolSelected < __IEDT_VALUE_MIN || m_eToolSelected > __IEDT_VALUE_MAX )
	{
		ASSERT( FALSE );
		m_eToolSelected = __IEDT_RECT_SELECTION;
	}
	OnIedSetCursor();
	return m_eToolSelPrev;
}

void CExtImageEditWnd::IedRender( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	m_arrImgEditData[m_nUndoPos]->Draw(
		dc,
		1,
		m_bmpResizingCache,
		m_rcSelection,
		m_clrLastCacheTransparency,
		m_bSeparatedCacheState
		);
}

void CExtImageEditWnd::OnSwPaint( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

CRect rcClipBox;
	dc.GetClipBox( &rcClipBox );
//	dc.FillSolidRect(
//		&rcClipBox,
//		g_PaintManager->GetColor(
//			CExtPaintManager::CLR_3DFACE_OUT
//			)
//		);
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
		dc.FillSolidRect( &rcClipBox, g_PaintManager->GetColor(CExtPaintManager::CLR_3DFACE_OUT) );

CSize _sizeCenterOffset( 0, 0 );
	if( (IedGetStyle() & __EIEDWS_CENTER) != 0 )
	{
		CSize _sizeClient = OnSwGetClientRect().Size();
		CSize _sizeTotal = OnSwGetTotalSize();
		if( _sizeClient.cx > _sizeTotal.cx )
			_sizeCenterOffset.cx =
				(_sizeClient.cx - _sizeTotal.cx)
				/ 2
				;
		if( _sizeClient.cy > _sizeTotal.cy )
			_sizeCenterOffset.cy =
				(_sizeClient.cy - _sizeTotal.cy)
				/ 2
				;
	}

CSize _sizeExt =
		( (IedGetStyle() & __EIEDWS_OUTER_AREA) != 0 )
			? OnIedMeasureToolSize()
			: CSize( 0, 0 )
			;
CPoint ptVP0 = dc.GetViewportOrg();
	dc.SetViewportOrg(
		ptVP0
		+ _sizeExt
		+ _sizeCenterOffset
		);

CSize _sizeClip(
		OnIedPicToClient(IedSizeGet())
		+ _sizeExt
		);
CRect rcClipPaint(
		CPoint(_sizeCenterOffset.cx,_sizeCenterOffset.cy), // CPoint(0,0)
		_sizeClip
		);
	rcClipPaint.InflateRect( 0, 0, 1, 1 );
CRgn rgnClip;
	VERIFY( rgnClip.CreateRectRgnIndirect( &rcClipPaint ) );
	dc.SelectClipRgn( &rgnClip, RGN_COPY );
	
	if( m_arrImgEditData[m_nUndoPos]->TransparencyColorGet() != __IED_COLORREF_NO_TRANSPARENCY )
	{
		CPoint ptVP1 = dc.GetViewportOrg();
		dc.SetViewportOrg( ptVP0 );
		OnIedPaintTransparencyBk( dc );
		dc.SetViewportOrg( ptVP1 );
	}
	
	m_arrImgEditData[m_nUndoPos]->Draw(
		dc,
		m_nMagnification,
		m_bmpResizingCache,
		m_rcSelection,
		m_clrLastCacheTransparency,
		m_bSeparatedCacheState
		);
	OnIedPaintGrid( dc );

bool bPaintSelectionTools = true;
	if( m_nTrackStep == 0 )
	{
	} // if( m_nTrackStep == 0 )
	else
	{
		if( m_eToolTracked == __IEDT_RECT_SELECTION
			&& m_nGripToolHT < 0
			)
		{
			ASSERT( m_nTrackStep == 1 );
			ASSERT( m_arrTrackPoints.GetSize() == 2 );
			bPaintSelectionTools = false;
		} // if( m_eToolTracked == __IEDT_RECT_SELECTION ...
	} // else from if( m_nTrackStep == 0 )

	dc.SelectClipRgn( NULL );

	if( bPaintSelectionTools )
	{
		dc.SetViewportOrg( CPoint(0,0) );
		OnIedPaintSelectionTools( dc, m_rcSelection );
	}

	dc.SetViewportOrg( ptVP0 );
}

CSize CExtImageEditWnd::OnIedMeasureToolSize() const
{
	ASSERT_VALID( this );
	return CSize( 7, 7 );
}

INT CExtImageEditWnd::_HitTestToolRects(
	CPoint ptCursor, // in client
	CRect rcSelection,
	__EXT_MFC_SAFE_LPCTSTR * p_lpszCursorResourceID
	)
{
	ASSERT_VALID( this );
	if( p_lpszCursorResourceID != NULL )
		*p_lpszCursorResourceID = NULL;

bool bFullSizeMode = false;
	if( rcSelection.IsRectEmpty() )
	{
		if(	(	IedGetStyle()
				&	(__EIEDWS_OUTER_AREA|__EIEDWS_OUTER_TOOLS|__EIEDWS_OUTER_RESIZING|__EIEDWS_READONLY)
				) != (__EIEDWS_OUTER_AREA|__EIEDWS_OUTER_TOOLS|__EIEDWS_OUTER_RESIZING)
			)
			return -1;
		CSize _size( IedSizeGet() );
		rcSelection.SetRect( 0, 0, _size.cx, _size.cy );
		bFullSizeMode = true;
	} // if( rcSelection.IsRectEmpty() )

	rcSelection = OnIedPicToClient( rcSelection, false );
	rcSelection.InflateRect( 0, 0, 1, 1 );

	if( (	!bFullSizeMode)
		&&	rcSelection.PtInRect(ptCursor)
		)
	{
		if( p_lpszCursorResourceID != NULL )
			*p_lpszCursorResourceID = IDC_SIZEALL;
		return -2;
	}

CRect arrRcTools[8];
	_CalcToolRects( rcSelection, arrRcTools );

	for( INT i = 0;  i < 8; i++ )
	{
		if( !arrRcTools[i].PtInRect(ptCursor) )
			continue;
		if( bFullSizeMode )
		{
			if( (IedGetStyle() & __EIEDWS_READONLY) != 0 )
				return -1;
			if( (IedGetStyle() & __EIEDWS_OUTER_RESIZING) == 0 )
				return -1;
			if( i==0 || i==1 || i==2 || i==6 || i==7 )
				return -1;
		}
		if( p_lpszCursorResourceID != NULL )
			*p_lpszCursorResourceID =
				stat_GetCurResFromToolHT( i );
		return i;
	} // for( INT i = 0;  i < 8; i++ )

	return -1;
}

void CExtImageEditWnd::_CalcToolRects(
	CRect rcSelection,
	CRect * pRcTools // 8 items
	)
{
	ASSERT_VALID( this );
	ASSERT( pRcTools != NULL );

	if( rcSelection.IsRectEmpty() )
	{
		for( INT i = 0; i< 8; i++ )
			pRcTools[i].SetRectEmpty();
		return;
	}

CSize _sizeExt( OnIedMeasureToolSize() );

	pRcTools[0] = rcSelection;
	pRcTools[0].right = rcSelection.left;
	pRcTools[0].bottom = rcSelection.top;
	pRcTools[0].left -= _sizeExt.cx;
	pRcTools[0].top -= _sizeExt.cy;

	pRcTools[1] = pRcTools[0];
	pRcTools[1].OffsetRect( (rcSelection.Width() + _sizeExt.cx)/2, 0 );

	pRcTools[2] = pRcTools[0];
	pRcTools[2].OffsetRect( rcSelection.Width() + _sizeExt.cx, 0 );

	pRcTools[3] = pRcTools[2];
	pRcTools[3].OffsetRect( 0, (rcSelection.Height() + _sizeExt.cy)/2 );

	pRcTools[7] = pRcTools[0];
	pRcTools[7].OffsetRect( 0, (rcSelection.Height() + _sizeExt.cy)/2 );

	pRcTools[4] = pRcTools[2];
	pRcTools[4].OffsetRect( 0, rcSelection.Height() + _sizeExt.cy );

	pRcTools[6] = pRcTools[0];
	pRcTools[6].OffsetRect( 0, rcSelection.Height() + _sizeExt.cy );

	pRcTools[5] = pRcTools[1];
	pRcTools[5].OffsetRect( 0, rcSelection.Height() + _sizeExt.cy );

//	for( INT i = 0; i< 8; i++ )
//		pRcTools[i].InflateRect( 1, 1 );
}

void CExtImageEditWnd::OnIedPaintSelectionTools(
	CDC & dc,
	CRect _rect
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	
bool bFullSizeMode = false;
	if( _rect.IsRectEmpty() )
	{
		if( (	IedGetStyle()
				&	(__EIEDWS_OUTER_AREA|__EIEDWS_OUTER_TOOLS)
				) != (__EIEDWS_OUTER_AREA|__EIEDWS_OUTER_TOOLS)
			)
			return;
		CSize _size( IedSizeGet() );
		_rect.SetRect( 0, 0, _size.cx, _size.cy );
		bFullSizeMode = true;
	} // if( _rect.IsRectEmpty() )

	_rect = OnIedPicToClient( _rect, false );
	_rect.InflateRect( 0, 0, 1, 1 );

	if(		bFullSizeMode
		&&	(IedGetStyle() & __EIEDWS_BUTTON_FRAME) != 0
		)
	{
		COLORREF
			clr3dHiLight = g_PaintManager->GetColor( COLOR_3DHILIGHT ),
			clr3dFace = g_PaintManager->GetColor( COLOR_3DFACE ),
			clr3dShadow = g_PaintManager->GetColor( COLOR_3DSHADOW ),
			clr3dDkShadow = g_PaintManager->GetColor( COLOR_3DDKSHADOW );
		_rect.InflateRect( 4, 4, 3, 3 );
		dc.Draw3dRect( &_rect, clr3dHiLight, clr3dShadow );
		_rect.InflateRect( 1, 1 );
		dc.Draw3dRect( &_rect, clr3dFace, clr3dDkShadow );
		return;
	}

CSize _sizeExt( OnIedMeasureToolSize() );

CBrush _brushHalftone;
	_brushHalftone.Attach(
		stat_GetHalftoneLightBrush( bFullSizeMode ? 1 : 2 )
		);
	ASSERT( _brushHalftone.GetSafeHandle() != NULL );
CBrush * pBrushOld =
		dc.SelectObject( &_brushHalftone );

CRect arrRcTools[8];
	_CalcToolRects( _rect, arrRcTools );

    dc.PatBlt(
		_rect.left, _rect.top,
		_rect.Width(), _rect.Height(),
		PATINVERT
		);
	_rect.InflateRect( _sizeExt );
    dc.PatBlt(
		_rect.left, _rect.top,
		_rect.Width(), _rect.Height(),
		PATINVERT
		);

COLORREF clrGripBoxInnerFS =
		g_PaintManager->GetColor( COLOR_WINDOW );
COLORREF clrGripBoxOuterFS =
		g_PaintManager->GetColor( COLOR_3DDKSHADOW );
COLORREF clrGripBox =
		g_PaintManager->GetColor( COLOR_HIGHLIGHT );
	for( INT i = 0; i < 8; i++ )
	{
		if(		bFullSizeMode
			&&	(	(i==0 || i==1 || i==2 || i==6 || i==7)
				||	(IedGetStyle() & __EIEDWS_OUTER_RESIZING) == 0
				||	(IedGetStyle() & __EIEDWS_READONLY) != 0
				)
			)
		{
			dc.FillSolidRect(
				arrRcTools[i],
				clrGripBoxInnerFS
				);
			dc.Draw3dRect(
				arrRcTools[i],
				clrGripBoxOuterFS,
				clrGripBoxOuterFS
				);
			continue;
		}
		dc.FillSolidRect(
			arrRcTools[i],
			clrGripBox
			);
	} // for( INT i = 0; i < 8; i++ )
	
    dc.SelectObject(pBrushOld);
}

void CExtImageEditWnd::OnIedPaintTrackingSelectionRect()
{
	ASSERT_VALID( this );
	ASSERT(
			m_eToolTracked == __IEDT_RECT_SELECTION
		||	m_eToolTracked == __IEDT_MAGNIFICATON
		);
	ASSERT( m_nTrackStep == 1 );
	ASSERT( m_arrTrackPoints.GetSize() == 2 );
CRect _rect(
		min( m_arrTrackPoints[0].x, m_arrTrackPoints[1].x ),
		min( m_arrTrackPoints[0].y, m_arrTrackPoints[1].y ),
		max( m_arrTrackPoints[0].x, m_arrTrackPoints[1].x ),
		max( m_arrTrackPoints[0].y, m_arrTrackPoints[1].y )
		);
	_rect.InflateRect( 0, 0, 1, 1 );
	ASSERT( _rect.left < _rect.right );
	ASSERT( _rect.top < _rect.bottom );
	ASSERT( !_rect.IsRectEmpty() );
	m_rcTrackingSel = _rect;
	_rect = OnIedPicToClient( _rect, false );
	_rect.InflateRect( 0, 0, 1, 1 );
	ASSERT( _rect.left < _rect.right );
	ASSERT( _rect.top < _rect.bottom );
CRect rcClient = OnSwGetClientRect();
	_rect.IntersectRect( rcClient, _rect );
	
CDC * pDC = GetDCEx(
		NULL,
		DCX_CACHE|DCX_LOCKWINDOWUPDATE|DCX_CLIPSIBLINGS
		);
	ASSERT( pDC->GetSafeHdc() != NULL );

CBrush _brushHalftone;
	_brushHalftone.Attach(
		stat_GetHalftoneLightBrush( 0 )
		);
	ASSERT( _brushHalftone.GetSafeHandle() != NULL );

CBrush * pBrushOld =
		pDC->SelectObject( &_brushHalftone );

    pDC->PatBlt(
		_rect.left, _rect.top,
		_rect.Width(), _rect.Height(),
		PATINVERT
		);

    pDC->SelectObject(pBrushOld);
	ReleaseDC( pDC );
	
	_rect.right--;
	_rect.bottom--;
	ASSERT( _rect.left <= _rect.right && _rect.top <= _rect.bottom );
	_rect = OnIedClientToPic( _rect, false );
	OnIedRectChange( _rect, true );
}

void CExtImageEditWnd::OnIedPaintGrid( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	if( (IedGetStyle() & __EIEDWS_GRID_LINES) == 0 )
		return;
	if( m_nMagnification < 4 )
		return;

CBrush _brushHalftone;
CGdiObject * pOldPen = NULL;
int nOldBkMode = OPAQUE;
	if( (IedGetStyle() & __EIEDWS_SOLID_GRID) != 0 )
		pOldPen = dc.SelectStockObject( BLACK_PEN );
	else
	{
		_brushHalftone.Attach(
			stat_GetHalftoneLightBrush( 2 )
			);
		ASSERT( _brushHalftone.GetSafeHandle() != NULL );
		nOldBkMode = dc.SetBkMode( TRANSPARENT );
	} // else from if( (IedGetStyle() & __EIEDWS_SOLID_GRID) != 0 )
CSize _size = OnIedPicToClient( IedSizeGet() );
CPoint ptStart( 0, 0 );
CPoint ptScroll = OnSwGetScrollPos();
CRect rcClient = OnSwGetClientRect();
	rcClient.OffsetRect( ptScroll );
	if( _size.cx > rcClient.right )
		_size.cx = rcClient.right;
	if( _size.cy > rcClient.bottom )
		_size.cy = rcClient.bottom;
	for(	INT nX = ptStart.x;
			nX <= _size.cx;
			nX += (INT)m_nMagnification
			)
	{
		if( (IedGetStyle() & __EIEDWS_SOLID_GRID) != 0 )
		{
			dc.MoveTo( nX, 0 );
			dc.LineTo( nX, _size.cy );
		} // if( (IedGetStyle() & __EIEDWS_SOLID_GRID) != 0 )
		else
		{
			CRect rcFill( nX, 0, nX+1, _size.cy );
			dc.FillRect( &rcFill, &_brushHalftone );
		} // else from if( (IedGetStyle() & __EIEDWS_SOLID_GRID) != 0 )
	}
	for(	INT nY = ptStart.y;
			nY <= _size.cy;
			nY += (INT)m_nMagnification
			)
	{
		if( (IedGetStyle() & __EIEDWS_SOLID_GRID) != 0 )
		{
			dc.MoveTo( 0, nY );
			dc.LineTo( _size.cx, nY );
		} // if( (IedGetStyle() & __EIEDWS_SOLID_GRID) != 0 )
		else
		{
			CRect rcFill( 0, nY, _size.cx, nY+1 );
			dc.FillRect( &rcFill, &_brushHalftone );
		} // else from if( (IedGetStyle() & __EIEDWS_SOLID_GRID) != 0 )
	}
	if( (IedGetStyle() & __EIEDWS_SOLID_GRID) != 0 )
		dc.SelectObject( pOldPen );
	else
		dc.SetBkMode( nOldBkMode );
}

void CExtImageEditWnd::OnIedPaintTransparencyBk( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc;
}

COLORREF CExtImageEditWnd::IedColorForegroundSet( COLORREF clr )
{
	ASSERT_VALID( this );
	COLORREF clrRet = m_clrForeground;
	if( m_clrForeground != clr )
		OnIedActiveColorChange(
			true,
			m_clrForeground,
			clr
			);
	return clrRet;
}

COLORREF CExtImageEditWnd::IedColorForegroundGet() const
{
	ASSERT_VALID( this );
	return m_clrForeground;
}

COLORREF CExtImageEditWnd::IedColorBackgroundSet( COLORREF clr )
{
	ASSERT_VALID( this );
	COLORREF clrRet = m_clrBackground;
	if( m_clrBackground != clr )
		OnIedActiveColorChange(
			false,
			m_clrBackground,
			clr
			);
	return clrRet;
}

COLORREF CExtImageEditWnd::IedColorBackgroundGet() const
{
	ASSERT_VALID( this );
	return m_clrBackground;
}

void CExtImageEditWnd::OnIedActiveColorChange(
	bool bForeground, // true - foreground color, false - background color
	COLORREF clrOld,
	COLORREF clrNew
	)
{
	ASSERT_VALID( this );
	clrOld;
	if( bForeground )
		m_clrForeground = clrNew;
	else
	{ // if background color
		m_clrBackground = clrNew;
		if( !m_rcSelection.IsRectEmpty() )
		{
			m_clrLastCacheTransparency = m_clrBackground;
			_RedrawWnd();
		}
	} // if background color
}

void CExtImageEditWnd::_NotifyTrackPoints(
	CPoint pt0,
	CPoint pt1
	)
{
ASSERT_VALID( this );
CRect rcTrack( pt0, pt1 );
	rcTrack.NormalizeRect();
	rcTrack.InflateRect( 0, 0, 1, 1 );
	OnIedRectChange( rcTrack, true );
}

void CExtImageEditWnd::OnIedRectChange(
	const CRect & rcSelection,
	bool bTrackingSelection
	)
{
	ASSERT_VALID( this );
	ASSERT( m_rcSelection == rcSelection || bTrackingSelection );
	rcSelection;
	bTrackingSelection;
}

CPoint CExtImageEditWnd::OnIedClientToPic( CPoint _point, bool bAdjust /*= true*/ )
{
	ASSERT( m_nMagnification >= 1 );
CSize _sizeCenterOffset( 0, 0 );
	if( (IedGetStyle() & __EIEDWS_CENTER) != 0 )
	{
		CSize _sizeClient = OnSwGetClientRect().Size();
		CSize _sizeTotal = OnSwGetTotalSize();
		if( _sizeClient.cx > _sizeTotal.cx )
			_sizeCenterOffset.cx =
				(_sizeClient.cx - _sizeTotal.cx)
				/ 2
				;
		if( _sizeClient.cy > _sizeTotal.cy )
			_sizeCenterOffset.cy =
				(_sizeClient.cy - _sizeTotal.cy)
				/ 2
				;
	}
CSize _sizeExt =
		( (IedGetStyle() & __EIEDWS_OUTER_AREA) != 0 )
			? OnIedMeasureToolSize()
			: CSize( 0, 0 )
			;
CPoint ptScroll = OnSwGetScrollPos();
	_point.x += ptScroll.x - _sizeExt.cx - _sizeCenterOffset.cx;
	_point.y += ptScroll.y - _sizeExt.cy - _sizeCenterOffset.cy;
	_point.x /= (LONG)m_nMagnification;
	_point.y /= (LONG)m_nMagnification;
	if( bAdjust )
	{
		CSize _size( IedSizeGet() );
		if( _point.x < 0 )
			_point.x = 0;
		else if( _point.x >= _size.cx )
			_point.x = _size.cx - 1;

		if( _point.y < 0 )
			_point.y = 0;
		else if( _point.y >= _size.cy )
			_point.y = _size.cy - 1;
	} // if( bAdjust )
	return _point;
}

CPoint CExtImageEditWnd::OnIedPicToClient( CPoint _point, bool bAdjust /*= true*/ )
{
	ASSERT_VALID( this );
	ASSERT( m_nMagnification >= 1 );
CSize _sizeCenterOffset( 0, 0 );
	if( (IedGetStyle() & __EIEDWS_CENTER) != 0 )
	{
		CSize _sizeClient = OnSwGetClientRect().Size();
		CSize _sizeTotal = OnSwGetTotalSize();
		if( _sizeClient.cx > _sizeTotal.cx )
			_sizeCenterOffset.cx =
				(_sizeClient.cx - _sizeTotal.cx)
				/ 2
				;
		if( _sizeClient.cy > _sizeTotal.cy )
			_sizeCenterOffset.cy =
				(_sizeClient.cy - _sizeTotal.cy)
				/ 2
				;
	}
	if( bAdjust )
	{
		CSize _size( IedSizeGet() );
		if( _point.x < 0 )
			_point.x = 0;
		else if( _point.x >= _size.cx )
			_point.x = _size.cx - 1;

		if( _point.y < 0 )
			_point.y = 0;
		else if( _point.y >= _size.cy )
			_point.y = _size.cy - 1;
	} // if( bAdjust )
	_point.x *= (LONG)m_nMagnification;
	_point.y *= (LONG)m_nMagnification;
CSize _sizeExt =
		( (IedGetStyle() & __EIEDWS_OUTER_AREA) != 0 )
			? OnIedMeasureToolSize()
			: CSize( 0, 0 )
			;
CPoint ptScroll = OnSwGetScrollPos();
	_point.x -= ptScroll.x - _sizeExt.cx - _sizeCenterOffset.cx;
	_point.y -= ptScroll.y - _sizeExt.cy - _sizeCenterOffset.cy;
	return _point;
}

CRect CExtImageEditWnd::OnIedClientToPic( CRect _rect, bool bAdjust /*= true*/ )
{
	ASSERT_VALID( this );
CPoint _pt1 = OnIedClientToPic( _rect.TopLeft(), bAdjust );
CPoint _pt2 = OnIedClientToPic( _rect.BottomRight(), bAdjust );
	_rect.SetRect( _pt1, _pt2 );
	return _rect;
}

CRect CExtImageEditWnd::OnIedPicToClient( CRect _rect, bool bAdjust /*= true*/ )
{
CPoint _pt1 = OnIedPicToClient( _rect.TopLeft(), bAdjust );
CPoint _pt2 = OnIedPicToClient( _rect.BottomRight(), bAdjust );
	_rect.SetRect( _pt1, _pt2 );
	return _rect;
}

CSize CExtImageEditWnd::OnIedClientToPic( CSize _size )
{
	ASSERT_VALID( this );
	ASSERT( m_nMagnification >= 1 );
	_size.cx /= (LONG)m_nMagnification;
	_size.cy /= (LONG)m_nMagnification;
	return _size;
}

CSize CExtImageEditWnd::OnIedPicToClient( CSize _size )
{
	ASSERT( m_nMagnification >= 1 );
	_size.cx *= (LONG)m_nMagnification;
	_size.cy *= (LONG)m_nMagnification;
	return _size;
}

void CExtImageEditWnd::OnIedQueryAirbrushTimerParms(
	LPUINT p_nAirbrushTimerID,
	LPUINT p_nAirbrushTimerEllapse, // = NULL
	CExtImageEditWnd::e_tool_t eTool, // = __IEDT_AIRBRUSH_VALUE_MIN
	LPINT p_nRadius, // = NULL
	LPINT p_nPixelsPerStep // = NULL
	)
{
	ASSERT_VALID( this );
	if( p_nAirbrushTimerID != NULL )
		*p_nAirbrushTimerID = __IED_DEF_AIRBRUSH_TIMER_ID;
	if( p_nAirbrushTimerEllapse != NULL )
		*p_nAirbrushTimerEllapse = __IED_DEF_AIRBRUSH_TIMER_ELLAPSE;
	if( p_nRadius == NULL && p_nPixelsPerStep == NULL )
		return;
	ASSERT(
			__IEDT_AIRBRUSH_VALUE_MIN <= eTool
		&&	eTool <= __IEDT_AIRBRUSH_VALUE_MAX
		);
INT nRadius = 0, nPixelsPerStep = 0;
	switch( eTool )
	{
	case __IEDT_AIRBRUSH_SMALL:
		nRadius = 4;
		nPixelsPerStep = 5;
	break;
	case __IEDT_AIRBRUSH_MEDIUM:
		nRadius = 7;
		nPixelsPerStep = 10;
	break;
	case __IEDT_AIRBRUSH_LARGE:
		nRadius = 13;
		nPixelsPerStep = 20;
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( eTool )
	if( p_nRadius != NULL )
		*p_nRadius = nRadius;
	if( p_nPixelsPerStep != NULL )
		*p_nPixelsPerStep = nPixelsPerStep;
}


void CExtImageEditWnd::OnIedCancelActions(
	bool bPopTrackingAction
	)
{
	ASSERT_VALID( this );
	if( m_bCancelingActions )
		return;

	m_bCancelingActions = true;
bool bRedraw = false;

UINT nAirbrushTimerID = __IED_DEF_AIRBRUSH_TIMER_ID;
	OnIedQueryAirbrushTimerParms(
		&nAirbrushTimerID
		);
	KillTimer( nAirbrushTimerID );

	if( m_nTrackStep > 0 )
	{
		m_nTrackStep = 0;
		m_arrTrackPoints.RemoveAll();
		bRedraw = true;
	}

	m_ptPicMoveLastOffs.x = m_ptPicMoveLastOffs.y = 0;
e_tool_t eTool = m_eToolTracked;
	m_eToolTracked = __IEDT_NONE;
	m_nGripToolHT = -1;

//	m_clrLastCacheTransparency = (COLORREF)(-1L);

	ASSERT( m_arrTrackPoints.GetSize() == 0 );

	if( CExtMouseCaptureSink::GetCapture() == m_hWnd )
	{
		CExtMouseCaptureSink::ReleaseCapture();
		bRedraw = true;
	}

	if(		bPopTrackingAction
		&&	eTool != __IEDT_NONE
		&&	eTool != __IEDT_RECT_SELECTION
		&&	eTool != __IEDT_MAGNIFICATON
		&&	eTool != __IEDT_COLOR_SELECTION
		)
		IedUndo( 1 );
	else if( bRedraw )
	{
		IedUndo( 0 );
		_RedrawWnd();
	}
	else
		IedUndo( 0 );

	m_bCancelingActions = false;

	if( m_eToolSelected == __IEDT_COLOR_SELECTION )
		m_eToolSelected = m_eToolSelPrev;
	OnIedSetCursor();

	OnIedRectChange( m_rcSelection, false );
	m_rcSelectionTS.SetRectEmpty();
}

/////////////////////////////////////////////////////////////////////////////
// CExtImageEditWnd message handlers

void CExtImageEditWnd::OnCancelMode() 
{
	ASSERT_VALID( this );
	CExtScrollWnd::OnCancelMode();
	OnIedCancelActions( true );
}

void CExtImageEditWnd::OnCaptureChanged(CWnd *pWnd) 
{
	ASSERT_VALID( this );
	if( pWnd != this )
		OnIedCancelActions( false );
	CExtScrollWnd::OnCaptureChanged(pWnd);
}

void CExtImageEditWnd::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	ASSERT_VALID( this );
	OnIedCancelActions( false );
	CExtScrollWnd::OnContextMenu(pWnd,point);
}
void CExtImageEditWnd::OnSize(UINT nType, int cx, int cy) 
{
	ASSERT_VALID( this );
	if( m_eToolTracked != __IEDT_NONE )
		OnIedCancelActions( false );
	CExtScrollWnd::OnSize(nType, cx, cy);
}
void CExtImageEditWnd::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	ASSERT_VALID( this );
	if( m_eToolTracked != __IEDT_NONE )
		OnIedCancelActions( false );
	CExtScrollWnd::OnWindowPosChanged(lpwndpos);
}
void CExtImageEditWnd::OnKillFocus(CWnd* pNewWnd) 
{
	ASSERT_VALID( this );
	CExtScrollWnd ::OnKillFocus(pNewWnd);
	OnIedCancelActions( false );
}

void CExtImageEditWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	ASSERT( (nFlags & MK_LBUTTON) != 0 );
	_RequestFocus();
	if( !OnIedMouseClick( true, nFlags, point ) )
	{
//		CExtScrollWnd::OnLButtonDown(nFlags, point);
	}
}

void CExtImageEditWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	if( !OnIedMouseClick( false, nFlags|MK_LBUTTON, point ) )
	{
//		CExtScrollWnd::OnLButtonUp(nFlags, point);
	}
}
void CExtImageEditWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	ASSERT( (nFlags & MK_RBUTTON) != 0 );
	_RequestFocus();
	if( !OnIedMouseClick( true, nFlags, point ) )
	{
//		CExtScrollWnd::OnRButtonUp(nFlags, point);
	}
}
void CExtImageEditWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	if( !OnIedMouseClick( false, nFlags|MK_RBUTTON, point ) )
	{
//		CExtScrollWnd::OnRButtonUp(nFlags, point);
	}
}
void CExtImageEditWnd::OnMButtonDown(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	ASSERT( (nFlags & MK_MBUTTON) != 0 );
	_RequestFocus();
	if( !OnIedMouseClick( true, nFlags, point ) )
	{
//		CExtScrollWnd::OnMButtonDown(nFlags, point);
	}
}
void CExtImageEditWnd::OnMButtonUp(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	if( !OnIedMouseClick( false, nFlags|MK_MBUTTON, point ) )
	{
//		CExtScrollWnd::OnMButtonUp(nFlags, point);
	}
}
void CExtImageEditWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	if( !OnIedMouseMove( nFlags, point ) )
	{
//		CExtScrollWnd::OnMouseMove( nFlags, point );
	}
}

bool CExtImageEditWnd::OnIedMouseClick(
	bool bMousePressed,
	UINT nFlags,
	CPoint point
	)
{
	ASSERT_VALID( this );
	nFlags;
	if( (IedGetStyle() & __EIEDWS_READONLY) != 0 )
	{
		OnIedCancelActions( false );
		return false;
	}
	if(	( nFlags & (MK_LBUTTON|MK_RBUTTON) ) == 0 )
		return false;

CPoint ptPic = OnIedClientToPic( point, false );
	if( bMousePressed )
	{
		bool bFullSizeHitTesting =
			m_rcSelection.IsRectEmpty()
				? true
				: false
			;
		if(		(!bFullSizeHitTesting)
			&&	m_rcSelection.PtInRect( ptPic )
			)
		{
			m_bSeparatedCacheState = false;
			_RequestFocus();
			CSize _sizeSelection = m_rcSelection.Size();
			if( m_bmpResizingCache.GetSafeHandle() == NULL )
			{
				if( !CExtPaintManager::stat_CloneBitmap(
						m_bmpResizingCache,
						m_arrImgEditData[m_nUndoPos]->GetBmpSurface(),
						&m_rcSelection
						)
					)
				{
					ASSERT( FALSE );
					OnIedCancelActions( false );
					return true;
				}
				m_sizeResizingCache = _sizeSelection;
			} // if( m_bmpResizingCache.GetSafeHandle() == NULL )
			else
			{
				ASSERT( m_sizeResizingCache.cx > 0 && m_sizeResizingCache.cy > 0 );
				if( _sizeSelection != m_sizeResizingCache )
				{
					CBitmap bmpDst;
					{ // BLOCK: surfaces
						CSurfaceDC dcSrc( m_bmpResizingCache );
						if( !bmpDst.CreateCompatibleBitmap( &dcSrc, _sizeSelection.cx, _sizeSelection.cy ) )
						{
							ASSERT( FALSE );
							OnIedCancelActions( false );
							return true;
						}
						CSurfaceDC dcDst( bmpDst );
						VERIFY(
							dcDst.StretchBlt(
								0, 0, _sizeSelection.cx, _sizeSelection.cy,
								&dcSrc,
								0, 0, m_sizeResizingCache.cx, m_sizeResizingCache.cy,
								SRCCOPY
								)
							);
					} // BLOCK: surfaces
					ASSERT( bmpDst.GetSafeHandle() != NULL );
					m_bmpResizingCache.DeleteObject();
					m_bmpResizingCache.Attach( bmpDst.Detach() );
					m_sizeResizingCache = _sizeSelection;
				} // if( _sizeSelection != m_sizeResizingCache )
			} // else from if( m_bmpResizingCache.GetSafeHandle() == NULL )
			m_nGripToolHT = 8;
			m_ptPicMoveLastOffs = ptPic;
			CExtMouseCaptureSink::SetCapture( m_hWnd );
			IedPushUndo( __EUT_MOVING_SELECTION );
			return true;
		}
		CRect rcToolsTest =
			bFullSizeHitTesting
				? CRect( CPoint(0,0), IedSizeGet() )
				: m_rcSelection
			;
		m_nGripToolHT =
			_HitTestToolRects(
				point,
				bFullSizeHitTesting
					? CRect( 0, 0, 0, 0 )
					: rcToolsTest
					,
				NULL
				);
		if( m_nGripToolHT >= 0 )
		{
			m_bSeparatedCacheState = false;
			if( bFullSizeHitTesting )
			{
				if( (	IedGetStyle()
						&	(__EIEDWS_OUTER_AREA|__EIEDWS_OUTER_TOOLS|__EIEDWS_OUTER_RESIZING|__EIEDWS_READONLY)
						) != (__EIEDWS_OUTER_AREA|__EIEDWS_OUTER_TOOLS|__EIEDWS_OUTER_RESIZING)
					)
				{
					m_nGripToolHT = -1;
					return true;
				}
				if( m_nGripToolHT==0 || m_nGripToolHT==1 || m_nGripToolHT==2 || m_nGripToolHT==6 || m_nGripToolHT==7 )
				{
					m_nGripToolHT = -1;
					return true;
				}
			} // if( bFullSizeHitTesting )
			_RequestFocus();
			CExtMouseCaptureSink::SetCapture( m_hWnd );
			IedPushUndo(
				bFullSizeHitTesting
					? __EUT_RESIZING_IMAGE
					: __EUT_RESIZING_SELECTION
				);

			if( m_bmpResizingCache.GetSafeHandle() == NULL )
			{
				if( !CExtPaintManager::stat_CloneBitmap(
						m_bmpResizingCache,
						m_arrImgEditData[m_nUndoPos]->GetBmpSurface(),
						&rcToolsTest
						)
					)
				{
					ASSERT( FALSE );
					OnIedCancelActions( true );
					return true;
				}
				m_sizeResizingCache = rcToolsTest.Size();
			} // if( m_bmpResizingCache.GetSafeHandle() == NULL )

			return true;
		} // if( m_nGripToolHT >= 0 )
		
		if( m_nTrackStep == 0 )
		{
			if( ptPic.x < 0 || ptPic.y < 0 )
				return true;
			CSize _sizeImg = IedSizeGet();
			if( ptPic.x >= _sizeImg.cx || ptPic.y >= _sizeImg.cy )
				return true;
		}
		if(		m_eToolSelected == __IEDT_MAGNIFICATON
			&&	( (nFlags & MK_RBUTTON) != 0 )
			)
		{
			if( m_nMagnification > g_nMagnificationMin )
				IedMagnificationSet( m_nMagnification-1, true );
			return true;
		}
		_RequestFocus();
		if( m_eToolSelected == __IEDT_FILL )
		{
			IedPushUndo( __EUT_FILL );
			CSurfaceDC dc( m_arrImgEditData[m_nUndoPos]->GetBmpSurface() );
			COLORREF clrPaint =
				(nFlags & MK_LBUTTON)
					? IedColorForegroundGet()
					: IedColorBackgroundGet()
					;
			CBrush _br( clrPaint );
			CBrush * pOldBrush = dc.SelectObject( &_br );
			dc.ExtFloodFill(
				ptPic.x,
				ptPic.y,
				dc.GetPixel( ptPic ),
				FLOODFILLSURFACE
				);
			dc.SelectObject( pOldBrush );
			return true;
		} // if( m_eToolSelected == __IEDT_FILL )
		if( m_eToolSelected == __IEDT_COLOR_SELECTION )
		{
			CSurfaceDC dc( m_arrImgEditData[m_nUndoPos]->GetBmpSurface() );
			COLORREF clrPixel = dc.GetPixel( ptPic );
			if( (nFlags & MK_LBUTTON) != 0 )
				IedColorForegroundSet( clrPixel );
			else
				IedColorBackgroundSet( clrPixel );
			m_eToolSelected = m_eToolSelPrev;
			OnIedSetCursor();
			return true;
		} // if( m_eToolSelected == __IEDT_COLOR_SELECTION )
		if( m_nTrackStep == 0 )
		{
			OnIedCancelActions( false );
			CExtMouseCaptureSink::SetCapture( m_hWnd );
			ASSERT( m_arrTrackPoints.GetSize() == 0 );
			m_arrTrackPoints.Add( ptPic );
			m_arrTrackPoints.Add( ptPic );
			m_nTrackStep ++;
			m_eToolTracked = m_eToolSelected;
			if(		m_eToolTracked == __IEDT_RECT_SELECTION
				||	m_eToolTracked == __IEDT_MAGNIFICATON
				)
			{
				if( m_eToolTracked == __IEDT_RECT_SELECTION )
					_EmptyCache();
				Invalidate();
				UpdateWindow();
				OnIedPaintTrackingSelectionRect();
			} // if( m_eTool == __IEDT_RECT_SELECTION ...
			else if( m_eToolTracked == __IEDT_PENCIL
				||	(	__IEDT_BRUSH_VALUE_MIN <= m_eToolTracked
					&&	m_eToolTracked <= __IEDT_BRUSH_VALUE_MAX
					)
				)
			{
				IedPushUndo(
					(m_eToolTracked == __IEDT_PENCIL)
						? __EUT_PENCIL
						: __EUT_BRUSH
					);
				OnIedMouseMove( nFlags, point );
			} // else if( m_eToolTracked == __IEDT_PENCIL ...
			else if(	__IEDT_LINE_VALUE_MIN <= m_eToolTracked
					&&	m_eToolTracked <= __IEDT_LINE_VALUE_MAX
					)
			{
				IedPushUndo( __EUT_LINE );
				OnIedMouseMove( nFlags, point );
			}
			else if(	__IEDT_AIRBRUSH_VALUE_MIN <= m_eToolTracked
					&&	m_eToolTracked <= __IEDT_AIRBRUSH_VALUE_MAX
					)
			{
				IedPushUndo( __EUT_AIRBRUSH );
				UINT
					nAirbrushTimerID = __IED_DEF_AIRBRUSH_TIMER_ID,
					nAirbrushTimerEllapse = __IED_DEF_AIRBRUSH_TIMER_ELLAPSE;
				OnIedQueryAirbrushTimerParms(
					&nAirbrushTimerID,
					&nAirbrushTimerEllapse
					);
				SetTimer(
					nAirbrushTimerID,
					nAirbrushTimerEllapse,
					NULL
					);
			}
			else if(	__IEDT_2D_GEOMETRY_VALUE_MIN <= m_eToolTracked
					&&	m_eToolTracked <= __IEDT_2D_GEOMETRY_VALUE_MAX
					)
			{
				if(		__IEDT_RECT_VALUE_MIN <= m_eToolTracked
						&&	m_eToolTracked <= __IEDT_RECT_VALUE_MAX
						)
					IedPushUndo( __EUT_RECTANGLE );
				else if(	__IEDT_RECT_OUTLINED_VALUE_MIN <= m_eToolTracked
						&&	m_eToolTracked <= __IEDT_RECT_OUTLINED_VALUE_MAX
						)
					IedPushUndo( __EUT_RECTANGLE_OUTLINED );
				else if( m_eToolTracked == __IEDT_RECT_FILLED )
					IedPushUndo( __EUT_RECTANGLE_FILLED );
				else if(	__IEDT_ROUNDED_RECT_VALUE_MIN <= m_eToolTracked
						&&	m_eToolTracked <= __IEDT_ROUNDED_RECT_VALUE_MAX
						)
					IedPushUndo( __EUT_ROUNDED_RECTANGLE );
				else if(	__IEDT_ROUNDED_RECT_OUTLINED_VALUE_MIN <= m_eToolTracked
						&&	m_eToolTracked <= __IEDT_ROUNDED_RECT_OUTLINED_VALUE_MAX
						)
					IedPushUndo( __EUT_ROUNDED_RECTANGLE_OUTLINED );
				else if( m_eToolTracked == __IEDT_ROUNDED_RECT_FILLED )
					IedPushUndo( __EUT_ROUNDED_RECTANGLE_FILLED );
				else if(	__IEDT_ELLIPSE_VALUE_MIN <= m_eToolTracked
						&&	m_eToolTracked <= __IEDT_ELLIPSE_VALUE_MAX
						)
					IedPushUndo( __EUT_ELLIPSE );
				else if(	__IEDT_ELLIPSE_OUTLINED_VALUE_MIN <= m_eToolTracked
						&&	m_eToolTracked <= __IEDT_ELLIPSE_OUTLINED_VALUE_MAX
						)
					IedPushUndo( __EUT_ELLIPSE_OUTLINED );
				else if( m_eToolTracked == __IEDT_ELLIPSE_FILLED )
					IedPushUndo( __EUT_ELLIPSE_FILLED  );
#ifdef _DEBUG
				else
				{
					ASSERT( FALSE );
				}
#endif // _DEBUG
				OnIedMouseMove( nFlags, point );
			}
			else
			{
				Invalidate();
				UpdateWindow();
			} // else
			OnIedSetCursor();
			return true;
		} // if( m_nTrackStep == 0 )
	} // if( bMousePressed )
	else
	{
		if( m_nGripToolHT >= 0 )
		{
			if( m_rcSelection.IsRectEmpty() )
				_EmptyCache();
		} // if( m_nGripToolHT >= 0 )
		else if(		m_eToolTracked == __IEDT_RECT_SELECTION
			||	m_eToolTracked == __IEDT_MAGNIFICATON
			)
		{
			OnIedPaintTrackingSelectionRect();
			if( m_eToolTracked == __IEDT_RECT_SELECTION )
			{
				_EmptyCache();
				m_rcSelection = m_rcTrackingSel;
				OnIedRectChange( m_rcSelection, false );
				Invalidate();
				UpdateWindow();
			} // if( m_eToolTracked == __IEDT_RECT_SELECTION )
			else
			{
				ASSERT( m_eToolTracked == __IEDT_MAGNIFICATON );
				m_rcTrackingSel.NormalizeRect();
				if(		m_rcTrackingSel.right > m_rcTrackingSel.left+1
					&&	m_rcTrackingSel.bottom > m_rcTrackingSel.top+1
					)
				{ // if tracking selection rect is not empty
					m_rcTrackingSel.InflateRect( 0, 0, 1, 1 );
					CSize _sizeExt =
						( (IedGetStyle() & __EIEDWS_OUTER_AREA) != 0 )
							? OnIedMeasureToolSize()
							: CSize( 0, 0 )
							;
					CRect rcOnCli = OnIedPicToClient( m_rcTrackingSel, false );
					rcOnCli.InflateRect( _sizeExt );
					CRect rcClient = OnSwGetClientRect();
					SetRedraw( FALSE );
					bool bMagnificationDecreased = false;
					for(	;
								rcOnCli.Width() >= rcClient.Width()
							||	rcOnCli.Height() >= rcClient.Height()
							;
						)
					{
						bMagnificationDecreased = true;
						ASSERT( m_nMagnification > g_nMagnificationMin );
						m_nMagnification--;
						OnSwUpdateScrollBars();
						rcOnCli = OnIedPicToClient( m_rcTrackingSel, false );
						rcOnCli.InflateRect( _sizeExt );
						if( m_nMagnification == g_nMagnificationMin )
							break;
					}
					if( (!bMagnificationDecreased) && m_nMagnification < g_nMagnificationMax )
					{
						for(	;
									rcOnCli.Width() < rcClient.Width()
								&&	rcOnCli.Height() < rcClient.Height()
								;
							)
						{
							ASSERT( m_nMagnification < g_nMagnificationMax );
							m_nMagnification++;
							OnSwUpdateScrollBars();
							rcOnCli = OnIedPicToClient( m_rcTrackingSel, false );
							rcOnCli.InflateRect( _sizeExt );
							if( m_nMagnification == g_nMagnificationMax )
								break;
						}
						if(		m_nMagnification > g_nMagnificationMin
							&&	(
									rcOnCli.Width() >= rcClient.Width()
								||	rcOnCli.Height() >= rcClient.Height()
								)
							)
						{
							m_nMagnification--;
							OnSwUpdateScrollBars();
							rcOnCli = OnIedPicToClient( m_rcTrackingSel, false );
							rcOnCli.InflateRect( _sizeExt );
						}
					} // if( (!bMagnificationDecreased) && m_nMagnification < g_nMagnificationMax )
					OnSwEnsurePointAvail( rcOnCli.BottomRight() );
					OnSwEnsurePointAvail( rcOnCli.TopLeft() );
					SetRedraw( TRUE );
/////m_rcSelection = m_rcTrackingSel; // debug
/////OnIedRectChange( m_rcSelection, false );
					_RedrawWnd();
				} // if tracking selection rect is not empty
			} // else from if( m_eToolTracked == __IEDT_RECT_SELECTION )
		} // else if( m_eToolTracked == __IEDT_RECT_SELECTION ...
		else if( m_eToolTracked == __IEDT_PENCIL )
		{
		}
		else if(	__IEDT_BRUSH_VALUE_MIN <= m_eToolTracked
				&&	m_eToolTracked <= __IEDT_BRUSH_VALUE_MAX
				)
		{
		}
		else if(	__IEDT_LINE_VALUE_MIN <= m_eToolTracked
				&&	m_eToolTracked <= __IEDT_LINE_VALUE_MAX
				)
		{
		}
		else if(	__IEDT_AIRBRUSH_VALUE_MIN <= m_eToolTracked
				&&	m_eToolTracked <= __IEDT_AIRBRUSH_VALUE_MAX
				)
		{
		}
		else if(	__IEDT_2D_GEOMETRY_VALUE_MIN <= m_eToolTracked
				&&	m_eToolTracked <= __IEDT_2D_GEOMETRY_VALUE_MAX
				)
		{
		}
		else
		{
			Invalidate();
			UpdateWindow();
		} // else 
		OnIedCancelActions( false );
	} // else from if( bMousePressed )
	return true;
}

bool CExtImageEditWnd::OnIedMouseMove(
	UINT nFlags,
	CPoint point
	) 
{
	ASSERT_VALID( this );
	if( (IedGetStyle() & __EIEDWS_READONLY) != 0 )
	{
		OnIedCancelActions( false );
		return false;
	}
	if(	( nFlags & (MK_LBUTTON|MK_RBUTTON) ) == 0 )
		return false;
	if(		m_eToolTracked == __IEDT_NONE
		&&	m_nGripToolHT < 0
		)
		return false;

CPoint ptPic = OnIedClientToPic( point, false );

	if( m_nGripToolHT == 8 )
	{ // if moving selection
		ASSERT( m_bmpResizingCache.GetSafeHandle() != NULL );
		ASSERT( m_sizeResizingCache.cx > 0 );
		ASSERT( m_sizeResizingCache.cy > 0 );
		ASSERT( !m_rcSelection.IsRectEmpty() );
		ASSERT( m_rcSelection.Size() == m_sizeResizingCache );
		CRect rcSelectionOld( m_rcSelection );
		m_clrLastCacheTransparency = (COLORREF)(-1L);
		COLORREF clrOuterParts = IedColorBackgroundGet();
		OnIedQuerySelParms(
			nFlags,
			false,
			NULL,
			NULL,
			&m_clrLastCacheTransparency,
			&clrOuterParts
			);
		if( !m_arrImgEditData[m_nUndoPos]->AssignFromOther(
				*m_arrImgEditData[m_nUndoPos+1],
				false
				)
			)
		{
			ASSERT( FALSE );
			OnIedCancelActions( true );
			return true;
		}
		m_rcSelection.OffsetRect( ptPic - m_ptPicMoveLastOffs );
		
		CSize _sizeAdjust = IedSizeGet();
		CPoint ptDiff0 = m_rcSelection.TopLeft();
		CPoint ptDiff1( 0, 0 );
		if( ptDiff0.x >= _sizeAdjust.cx )
			ptDiff1.x = _sizeAdjust.cx - ptDiff0.x - 1;
		else if( ptDiff0.x < (-m_rcSelection.Width()) )
			ptDiff1.x = -ptDiff0.x-m_rcSelection.Width()+1;
		if( ptDiff0.y >= _sizeAdjust.cy )
			ptDiff1.y = _sizeAdjust.cy - ptDiff0.y - 1;
		else if( ptDiff0.y < (-m_rcSelection.Height()) )
			ptDiff1.y = -ptDiff0.y-m_rcSelection.Height()+1;
		m_rcSelection.OffsetRect( ptDiff1 );

		m_ptPicMoveLastOffs = ptPic + ptDiff1;

		{ // BLOCK: surfaces
			CSurfaceDC dcSrc( m_bmpResizingCache );
			CSurfaceDC dcDst( m_arrImgEditData[m_nUndoPos]->GetBmpSurface() );
			CPoint ptOrigin( 0, 0 );
			CRect rcDest( m_rcSelection );
			CSize sizeSrc( m_sizeResizingCache );
			if( rcDest.left < 0 )
			{
				sizeSrc.cx += rcDest.left;
				ptOrigin.x -= rcDest.left;
				rcDest.left = 0;
			} // if( rcDest.left < 0 )
			if( rcDest.top < 0 )
			{
				sizeSrc.cy += rcDest.top;
				ptOrigin.y -= rcDest.top;
				rcDest.top = 0;
			} // if( rcDest.top < 0 )
			if( m_rcSelectionTS.IsRectEmpty() )
				m_rcSelectionTS = CRect( rcSelectionOld.TopLeft(), sizeSrc );
			if( m_bEraseBkBeforeDraggingSelection && (!m_rcSelectionTS.IsRectEmpty()) )
			{
				COLORREF clrEraseSelBk =
					( ( nFlags & MK_LBUTTON ) != 0 )
						? IedColorBackgroundGet()
						: IedColorForegroundGet()
						;
				if( clrEraseSelBk == COLORREF(-1L) )
					clrEraseSelBk =
						( ( nFlags & MK_LBUTTON ) == 0 )
							? IedColorBackgroundGet()
							: IedColorForegroundGet()
							;
				if( clrEraseSelBk != COLORREF(-1L) )
					dcDst.FillSolidRect(
						&m_rcSelectionTS,
						clrEraseSelBk
						);
			} // if( m_bEraseBkBeforeDraggingSelection && (!m_rcSelectionTS.IsRectEmpty()) )
			if( m_clrLastCacheTransparency != ((COLORREF)(-1L)) )
			{ // if transparent moving
				CExtPaintManager::stat_TransparentBlt(
					dcDst,
					rcDest.left, rcDest.top, sizeSrc.cx, sizeSrc.cy,
					dcSrc,
					ptOrigin.x, ptOrigin.y, sizeSrc.cx, sizeSrc.cy,
					m_clrLastCacheTransparency
					);
			} // if transparent moving
			else
			{ // if plain moving
				VERIFY(
					dcDst.BitBlt(
						rcDest.left, rcDest.top, sizeSrc.cx, sizeSrc.cy,
						&dcSrc,
						ptOrigin.x, ptOrigin.y,
						SRCCOPY
						)
					);
			} // if plain moving
		} // BLOCK: surfaces

		IedUndo( 0 );
		OnIedRectChange( m_rcSelection, false );

		Invalidate();
		UpdateWindow();
		return true;
	} // if moving selection

	if( m_nGripToolHT >= 0 )
	{
		CRect rcSelectionOld( m_rcSelection );
		bool bFullSizeHitTesting =
			m_rcSelection.IsRectEmpty()
				? true
				: false
			;
		CRect rcToolsTest =
			bFullSizeHitTesting
				? CRect( CPoint(0,0), IedSizeGet() )
				: m_rcSelection
			;
		switch( m_nGripToolHT )
		{
		case 0:
			rcToolsTest.left = ptPic.x;
			if( rcToolsTest.left >= rcToolsTest.right )
				rcToolsTest.left = rcToolsTest.right-1;
			rcToolsTest.top = ptPic.y;
			if( rcToolsTest.top >= rcToolsTest.bottom )
				rcToolsTest.top = rcToolsTest.bottom-1;
		break;
		case 1:
			rcToolsTest.top = ptPic.y;
			if( rcToolsTest.top >= rcToolsTest.bottom )
				rcToolsTest.top = rcToolsTest.bottom-1;
		break;
		case 2:
			rcToolsTest.right = ptPic.x;
			if( rcToolsTest.right <= rcToolsTest.left )
				rcToolsTest.right = rcToolsTest.left+1;
			rcToolsTest.top = ptPic.y;
			if( rcToolsTest.top >= rcToolsTest.bottom )
				rcToolsTest.top = rcToolsTest.bottom-1;
		break;
		case 3:
			rcToolsTest.right = ptPic.x;
			if( rcToolsTest.right <= rcToolsTest.left )
				rcToolsTest.right = rcToolsTest.left+1;
		break;
		case 4:
			rcToolsTest.right = ptPic.x;
			if( rcToolsTest.right <= rcToolsTest.left )
				rcToolsTest.right = rcToolsTest.left+1;
			rcToolsTest.bottom = ptPic.y;
			if( rcToolsTest.bottom <= rcToolsTest.top )
				rcToolsTest.bottom = rcToolsTest.top+1;
		break;
		case 5:
			rcToolsTest.bottom = ptPic.y;
			if( rcToolsTest.bottom <= rcToolsTest.top )
				rcToolsTest.bottom = rcToolsTest.top+1;
		break;
		case 6:
			rcToolsTest.left = ptPic.x;
			if( rcToolsTest.left >= rcToolsTest.right )
				rcToolsTest.left = rcToolsTest.right-1;
			rcToolsTest.bottom = ptPic.y;
			if( rcToolsTest.bottom <= rcToolsTest.top )
				rcToolsTest.bottom = rcToolsTest.top+1;
		break;
		case 7:
			rcToolsTest.left = ptPic.x;
			if( rcToolsTest.left >= rcToolsTest.right )
				rcToolsTest.left = rcToolsTest.right-1;
		break;
		} // switch( m_nGripToolHT )
		ASSERT( m_bmpResizingCache.GetSafeHandle() != NULL );
		ASSERT( m_sizeResizingCache.cx > 0 );
		ASSERT( m_sizeResizingCache.cy > 0 );
		bool bStretch = false, bHalftone = false;
		m_clrLastCacheTransparency = (COLORREF)(-1L);
		COLORREF clrOuterParts = IedColorBackgroundGet();
		OnIedQuerySelParms(
			nFlags,
			bFullSizeHitTesting,
			&bStretch,
			&bHalftone,
			&m_clrLastCacheTransparency,
			&clrOuterParts
			);
		if( bFullSizeHitTesting )
		{
			CBitmap & bmpSurface = m_arrImgEditData[m_nUndoPos]->GetBmpSurface();
			bmpSurface.DeleteObject();
			{ // BLOCK: surfaces
				CSurfaceDC dcSrc( m_bmpResizingCache );
				CSize _sizeNew = rcToolsTest.Size();
				bmpSurface.CreateCompatibleBitmap( &dcSrc, _sizeNew.cx, _sizeNew.cy );
				CSurfaceDC dcDst( bmpSurface );
				int nOldStretchBltMode = 0;
				if( bStretch )
				{ // if stretching all image
					if( bHalftone )
					{
						nOldStretchBltMode =
							dcDst.SetStretchBltMode(
								g_PaintManager.m_bIsWinNT ? HALFTONE : COLORONCOLOR
								);
						::SetBrushOrgEx( dcDst, 0, 0, NULL );
					}
					else
						nOldStretchBltMode = dcDst.SetStretchBltMode( COLORONCOLOR );
					VERIFY(
						dcDst.StretchBlt(
							0, 0, _sizeNew.cx, _sizeNew.cy,
							&dcSrc,
							0, 0, m_sizeResizingCache.cx, m_sizeResizingCache.cy,
							SRCCOPY
							)
						);
					if( nOldStretchBltMode != 0 )
						dcDst.SetStretchBltMode( nOldStretchBltMode );
				} // if stretching all image
				else
				{ // if resizing all image
					dcDst.FillSolidRect(
						0, 0, _sizeNew.cx, _sizeNew.cy,
						clrOuterParts
						);
					VERIFY(
						dcDst.BitBlt(
							0, 0, _sizeNew.cx, _sizeNew.cy,
							&dcSrc,
							0, 0,
							SRCCOPY
							)
						);
				} // if resizing all image
			} // BLOCK: surfaces
			m_arrImgEditData[m_nUndoPos]->SizeReset();
			
			IedUndo( 0 );
			
			_RedrawWnd();
		} // if( bFullSizeHitTesting )
		else
		{
			if( !m_arrImgEditData[m_nUndoPos]->AssignFromOther(
					*m_arrImgEditData[m_nUndoPos+1],
					false
					)
				)
			{
				ASSERT( FALSE );
				OnIedCancelActions( true );
				return true;
			}
			{ // BLOCK: surfaces
				CSurfaceDC dcSrc( m_bmpResizingCache );
				CSurfaceDC dcDst( m_arrImgEditData[m_nUndoPos]->GetBmpSurface() );
				int nOldStretchBltMode = 0;
				if( bHalftone )
				{
					nOldStretchBltMode =
						dcDst.SetStretchBltMode(
							g_PaintManager.m_bIsWinNT ? HALFTONE : COLORONCOLOR
							);
					::SetBrushOrgEx( dcDst, 0, 0, NULL );
				}
				else
					nOldStretchBltMode = dcDst.SetStretchBltMode( COLORONCOLOR );
				if( m_rcSelectionTS.IsRectEmpty() )
					m_rcSelectionTS = rcSelectionOld;
				if( m_bEraseBkBeforeResizingSelection && (!m_rcSelectionTS.IsRectEmpty()) )
				{
					COLORREF clrEraseSelBk =
						( ( nFlags & MK_LBUTTON ) != 0 )
							? IedColorBackgroundGet()
							: IedColorForegroundGet()
							;
					if( clrEraseSelBk == COLORREF(-1L) )
						clrEraseSelBk =
							( ( nFlags & MK_LBUTTON ) == 0 )
								? IedColorBackgroundGet()
								: IedColorForegroundGet()
								;
					if( clrEraseSelBk != COLORREF(-1L) )
						dcDst.FillSolidRect(
							&m_rcSelectionTS,
							clrEraseSelBk
							);
				} // if( m_bEraseBkBeforeResizingSelection && (!m_rcSelectionTS.IsRectEmpty()) )
				if( m_clrLastCacheTransparency != ((COLORREF)(-1L)) )
				{ // if transparent resizing
					CExtPaintManager::stat_TransparentBlt(
						dcDst,
						rcToolsTest.left, rcToolsTest.top, rcToolsTest.Width(), rcToolsTest.Height(),
						dcSrc,
						0, 0, m_sizeResizingCache.cx, m_sizeResizingCache.cy,
						m_clrLastCacheTransparency
						);
				} // if transparent resizing
				else
				{ // if plain resizing
					VERIFY(
						dcDst.StretchBlt(
							rcToolsTest.left, rcToolsTest.top, rcToolsTest.Width(), rcToolsTest.Height(),
							&dcSrc,
							0, 0, m_sizeResizingCache.cx, m_sizeResizingCache.cy,
							SRCCOPY
							)
						);
				} // if plain resizing
				if( nOldStretchBltMode != 0 )
					dcDst.SetStretchBltMode( nOldStretchBltMode );
			} // BLOCK: surfaces
			m_rcSelection = rcToolsTest;

			IedUndo( 0 );
			OnIedRectChange( m_rcSelection, false );

			Invalidate();
			UpdateWindow();
		} // else from if( bFullSizeHitTesting )
		return true;
	} // if( m_nGripToolHT >= 0 )

	ASSERT( m_nTrackStep >= 0 );
	if( m_nTrackStep == 0 )
		return false;
	ASSERT( ((UINT)m_arrTrackPoints.GetSize()) == (m_nTrackStep+1) );
	
	if(		m_eToolTracked == __IEDT_RECT_SELECTION
		||	m_eToolTracked == __IEDT_MAGNIFICATON
		)
	{
		ASSERT( m_nTrackStep == 1 );
		ASSERT( m_arrTrackPoints.GetSize() == 2 );
		CPoint & ptLast = m_arrTrackPoints.ElementAt( 1 );
		if( m_eToolTracked == __IEDT_RECT_SELECTION )
			ptPic = OnIedClientToPic( point, true );
		if( ptLast != ptPic )
		{
			OnIedPaintTrackingSelectionRect();
			CSize _sizeAdjust( OnIedPicToClient( CSize(1,1) ) );
			if( ptLast.x >= ptPic.x )
				_sizeAdjust.cx = -_sizeAdjust.cx;
			if( ptLast.y >= ptPic.y )
				_sizeAdjust.cy = -_sizeAdjust.cy;
			ptLast = ptPic;
			OnSwEnsurePointAvail(
				point + _sizeAdjust
				);
			OnIedPaintTrackingSelectionRect();
		}
		OnIedSetCursor();
		return true;
	} // if( m_eToolTracked == __IEDT_RECT_SELECTION ...
	
	if( m_eToolTracked == __IEDT_PENCIL )
	{
		ASSERT( m_nTrackStep == 1 );
		ASSERT( m_arrTrackPoints.GetSize() == 2 );
		CPoint & ptLast = m_arrTrackPoints.ElementAt( 1 );
		COLORREF clrPaint =
			(nFlags & MK_LBUTTON)
				? IedColorForegroundGet()
				: IedColorBackgroundGet()
				;
		{ // BLOCK: GDI
			CPen _pen( PS_SOLID, 1, clrPaint );
			ASSERT( _pen.GetSafeHandle() != NULL );
			CSurfaceDC dc( m_arrImgEditData[m_nUndoPos]->GetBmpSurface() );
			CPen * pOldPen = dc.SelectObject( &_pen );
			dc.MoveTo( ptLast );
			dc.LineTo( ptPic );
			dc.SelectObject( pOldPen );
			dc.SetPixel( ptPic, clrPaint );
		} // BLOCK: GDI
		CRect rcPaint( ptPic, ptLast );
		rcPaint.NormalizeRect();
		rcPaint.InflateRect( 1, 1 );
		rcPaint = OnIedPicToClient( rcPaint, false );
		InvalidateRect( &rcPaint );
		ptLast = ptPic;

		IedUndo( 0 );

		return true;
	} // if( m_eToolTracked == __IEDT_PENCIL )

	if( __IEDT_BRUSH_VALUE_MIN <= m_eToolTracked && m_eToolTracked <= __IEDT_BRUSH_VALUE_MAX )
	{ // if brush painting
		COLORREF clrPaint =
			(nFlags & MK_LBUTTON)
				? IedColorForegroundGet()
				: IedColorBackgroundGet()
				;
		ASSERT( m_arrTrackPoints.GetSize() == 2 );
		m_arrTrackPoints.SetAt( 0, m_arrTrackPoints[1] );
		m_arrTrackPoints.SetAt( 1, ptPic );
		stat_IedDrawToolLine(
			m_eToolTracked,
			m_arrImgEditData[m_nUndoPos]->GetBmpSurface(),
			clrPaint,
			m_arrTrackPoints[0],
			m_arrTrackPoints[1]
			);
		CRect rcPaint( m_arrTrackPoints[0], m_arrTrackPoints[1] );
		rcPaint.NormalizeRect();
		rcPaint.InflateRect( 30, 30 );
		rcPaint = OnIedPicToClient( rcPaint, false );
		InvalidateRect( &rcPaint );

		IedUndo( 0 );

		return true;
	} // if brush painting
	
	if(	__IEDT_LINE_VALUE_MIN <= m_eToolTracked
		&&	m_eToolTracked <= __IEDT_LINE_VALUE_MAX
		)
	{ // if line painting
		COLORREF clrPaint =
			(nFlags & MK_LBUTTON)
				? IedColorForegroundGet()
				: IedColorBackgroundGet()
				;
		ASSERT( m_arrTrackPoints.GetSize() == 2 );
		ASSERT( m_arrImgEditData.GetSize() >= 2 && m_nUndoPos < (m_arrImgEditData.GetSize()-1) );
		m_arrTrackPoints.SetAt( 1, ptPic );
		m_arrImgEditData[m_nUndoPos]->AssignFromOther(
			*m_arrImgEditData[m_nUndoPos+1],
			false
			);
		stat_IedDrawToolLine(
			m_eToolTracked,
			m_arrImgEditData[m_nUndoPos]->GetBmpSurface(),
			clrPaint,
			m_arrTrackPoints[0],
			m_arrTrackPoints[1]
			);
		CRect rcPaint( m_arrTrackPoints[0], m_arrTrackPoints[1] );
		rcPaint.NormalizeRect();
		rcPaint.InflateRect( 30, 30 );
		rcPaint = OnIedPicToClient( rcPaint, false );
		InvalidateRect( &rcPaint );

		IedUndo( 0 );
		_NotifyTrackPoints(
			m_arrTrackPoints[0],
			m_arrTrackPoints[1]
			);

		return true;
	} // if line painting

	if(		__IEDT_AIRBRUSH_VALUE_MIN <= m_eToolTracked
		&&	m_eToolTracked <= __IEDT_AIRBRUSH_VALUE_MAX
		)
	{ // if airbrush painting
		m_arrTrackPoints.SetAt( 1, ptPic );
		return true;
	} // if airbrush painting

	if(	__IEDT_2D_GEOMETRY_VALUE_MIN <= m_eToolTracked
		&&	m_eToolTracked <= __IEDT_2D_GEOMETRY_VALUE_MAX
		)
	{ // if 2D geometry
		ASSERT( m_arrTrackPoints.GetSize() == 2 );
		ASSERT( m_arrImgEditData.GetSize() >= 2 && m_nUndoPos < (m_arrImgEditData.GetSize()-1) );
		CRect rcPaint( m_arrTrackPoints[0], m_arrTrackPoints[1] );
		rcPaint.NormalizeRect();
		rcPaint.InflateRect( 0, 0, 1, 1 );
		rcPaint = OnIedPicToClient( rcPaint, false );
		InvalidateRect( &rcPaint );
		m_arrTrackPoints.SetAt( 1, ptPic );
		m_arrImgEditData[m_nUndoPos]->AssignFromOther(
			*m_arrImgEditData[m_nUndoPos+1],
			false
			);
		stat_IedDrawTool2dGeometry(
			m_eToolTracked,
			m_arrImgEditData[m_nUndoPos]->GetBmpSurface(),
			IedColorForegroundGet(),
			IedColorBackgroundGet(),
			m_arrTrackPoints[0],
			m_arrTrackPoints[1],
			( (nFlags & MK_RBUTTON) != 0 ) ? true : false
			);
		rcPaint.SetRect( m_arrTrackPoints[0], m_arrTrackPoints[1] );
		rcPaint.NormalizeRect();
		rcPaint.InflateRect( 30, 30, 31, 31 );
		rcPaint = OnIedPicToClient( rcPaint, false );
		InvalidateRect( &rcPaint );

		IedUndo( 0 );
		_NotifyTrackPoints(
			m_arrTrackPoints[0],
			m_arrTrackPoints[1]
			);

		return true;
	} // if 2D geometry

	return true;
}

void CExtImageEditWnd::OnIedQuerySelParms(
	INT nMouseMsgFlags, // [in] WM_MOUSEMOVE message flags
	bool bAllImage, // [in] true - all image, false - selection
	bool * p_bStretch, // [out] true - stretch image content, false - only resize image (all image only)
	bool * p_bHalftone, // [out] true - use smooth stretching algorithm
	COLORREF * p_clrTransparent, // [out] ((COLORREF)(-1L)) - no transparent, other - color of transparent pixels when stretching (selection only)
	COLORREF * p_clrOuterParts // [out] color of outer parts (all image only)
	)
{
	ASSERT_VALID( this );
	if( p_bStretch != NULL )
		*p_bStretch = false;
	if( p_bHalftone != NULL )
		*p_bHalftone = false;
	if( p_clrTransparent != NULL )
		*p_clrTransparent = (COLORREF)(-1L);
	if( p_clrOuterParts != NULL )
		*p_clrOuterParts = IedColorBackgroundGet();
	if(		p_bHalftone != NULL
		&&	(nMouseMsgFlags & MK_SHIFT) != 0
		)
		*p_bHalftone = true;
	if( bAllImage )
	{
		if(		p_bStretch != NULL
			&&	(nMouseMsgFlags & MK_CONTROL) != 0
			)
			*p_bStretch = true;
	} // if( bAllImage )
	else
	{
		if(		p_clrTransparent != NULL
			&&	(nMouseMsgFlags & MK_CONTROL) != 0
			)
		{
			*p_clrTransparent = IedColorBackgroundGet();
//			if( IedIsTransparencyEnabled() )
//			{
//				COLORREF clrTransparentMode = IedGetTransparencyColor();
//				ASSERT( clrTransparentMode != __IED_COLORREF_NO_TRANSPARENCY );
//				m_clrLastCacheTransparency = clrTransparentMode;
//			} // if( IedIsTransparencyEnabled() )
//			else
//			{
//				COLORREF clrTransparentMode = IedColorBackgroundGet();
//				if( clrTransparentMode != __IED_COLORREF_NO_TRANSPARENCY )
//					m_clrLastCacheTransparency = clrTransparentMode;
//			} // else from if( IedIsTransparencyEnabled() )
		}

	} // else from if( bAllImage )
}

void CExtImageEditWnd::stat_IedDrawTool2dGeometry(
	CExtImageEditWnd::e_tool_t eTool,
	CBitmap & bmpSurface,
	COLORREF clrForward,
	COLORREF clrBackward,
	CPoint ptFrom,
	CPoint ptTo,
	bool bRightButton // = false
	)
{
	ASSERT( bmpSurface.GetSafeHandle() != NULL );
	if( ptTo.x >= ptFrom.x )
		ptTo.x++;
	else
		ptFrom.x++;
	if( ptTo.y >= ptFrom.y )
		ptTo.y++;
	else
		ptFrom.y++;
INT nOutlineWidth = 0;
COLORREF clrOutline = clrForward;
COLORREF clrFill = ((COLORREF)(-1L));
	switch( eTool )
	{
	case __IEDT_RECT_SMALLER:
		nOutlineWidth = 1;
	break;
	case __IEDT_RECT_SMALL:
		nOutlineWidth = 2;
	break;
	case __IEDT_RECT_MEDIUM:
		nOutlineWidth = 3;
	break;
	case __IEDT_RECT_LARGE:
		nOutlineWidth = 4;
	break;
	case __IEDT_RECT_LARGER:
		nOutlineWidth = 5;
	break;
	case __IEDT_RECT_OUTLINED_SMALLER:
		nOutlineWidth = 1;
		clrFill = clrBackward;
	break;
	case __IEDT_RECT_OUTLINED_SMALL:
		nOutlineWidth = 2;
		clrFill = clrBackward;
	break;
	case __IEDT_RECT_OUTLINED_MEDIUM:
		nOutlineWidth = 3;
		clrFill = clrBackward;
	break;
	case __IEDT_RECT_OUTLINED_LARGE:
		nOutlineWidth = 4;
		clrFill = clrBackward;
	break;
	case __IEDT_RECT_OUTLINED_LARGER:
		nOutlineWidth = 5;
		clrFill = clrBackward;
	break;
	case __IEDT_ROUNDED_RECT_SMALLER:
		nOutlineWidth = 1;
	break;
	case __IEDT_ROUNDED_RECT_SMALL:
		nOutlineWidth = 2;
	break;
	case __IEDT_ROUNDED_RECT_MEDIUM:
		nOutlineWidth = 3;
	break;
	case __IEDT_ROUNDED_RECT_LARGE:
		nOutlineWidth = 4;
	break;
	case __IEDT_ROUNDED_RECT_LARGER:
		nOutlineWidth = 5;
	break;
	case __IEDT_ROUNDED_RECT_OUTLINED_SMALLER:
		nOutlineWidth = 1;
		clrFill = clrBackward;
	break;
	case __IEDT_ROUNDED_RECT_OUTLINED_SMALL:
		nOutlineWidth = 2;
		clrFill = clrBackward;
	break;
	case __IEDT_ROUNDED_RECT_OUTLINED_MEDIUM:
		nOutlineWidth = 3;
		clrFill = clrBackward;
	break;
	case __IEDT_ROUNDED_RECT_OUTLINED_LARGE:
		nOutlineWidth = 4;
		clrFill = clrBackward;
	break;
	case __IEDT_ROUNDED_RECT_OUTLINED_LARGER:
		nOutlineWidth = 5;
		clrFill = clrBackward;
	break;
	case __IEDT_ELLIPSE_SMALLER:
		nOutlineWidth = 1;
	break;
	case __IEDT_ELLIPSE_SMALL:
		nOutlineWidth = 2;
	break;
	case __IEDT_ELLIPSE_MEDIUM:
		nOutlineWidth = 3;
	break;
	case __IEDT_ELLIPSE_LARGE:
		nOutlineWidth = 4;
	break;
	case __IEDT_ELLIPSE_LARGER:
		nOutlineWidth = 5;
	break;
	case __IEDT_ELLIPSE_OUTLINED_SMALLER:
		nOutlineWidth = 1;
		clrFill = clrBackward;
	break;
	case __IEDT_ELLIPSE_OUTLINED_SMALL:
		nOutlineWidth = 2;
		clrFill = clrBackward;
	break;
	case __IEDT_ELLIPSE_OUTLINED_MEDIUM:
		nOutlineWidth = 3;
		clrFill = clrBackward;
	break;
	case __IEDT_ELLIPSE_OUTLINED_LARGE:
		nOutlineWidth = 4;
		clrFill = clrBackward;
	break;
	case __IEDT_ELLIPSE_OUTLINED_LARGER:
		nOutlineWidth = 5;
		clrFill = clrBackward;
	break;
	case __IEDT_RECT_FILLED:
	case __IEDT_ROUNDED_RECT_FILLED:
	case __IEDT_ELLIPSE_FILLED:
		{
			CPoint ptSwapMin(
				min( ptFrom.x, ptTo.x ),
				min( ptFrom.y, ptTo.y )
				);
			CPoint ptSwapMax(
				max( ptFrom.x, ptTo.x ) + 1,
				max( ptFrom.y, ptTo.y ) + 1
				);
			ptFrom = ptSwapMin;
			ptTo = ptSwapMax;
			clrFill = clrForward;
			clrOutline = ((COLORREF)(-1L));
		}
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( eTool )

COLORREF clrOutlineEffective = clrOutline;
COLORREF clrFillEffective = clrFill;
	if( bRightButton )
	{
		if( clrOutline != ((COLORREF)(-1L)) )
		{
			if( clrOutline == clrForward )
				clrOutlineEffective = clrBackward;
			else
				clrOutlineEffective = clrForward;
		}
		if( clrFill != ((COLORREF)(-1L)) )
		{
			if( clrFill == clrForward )
				clrFillEffective = clrBackward;
			else
				clrFillEffective = clrForward;
		}
	}

CSurfaceDC dc( bmpSurface );
HGDIOBJ hOldPen = NULL, hOldBrush = NULL;
CPen _pen;
CBrush _br;
	if( nOutlineWidth == 0 )
		hOldPen = ::SelectObject( dc, ::GetStockObject(NULL_PEN) );
	else
	{
		VERIFY( _pen.CreatePen( PS_SOLID, nOutlineWidth, clrOutlineEffective ) );
		hOldPen = ::SelectObject( dc, _pen.GetSafeHandle() );
	}
	if( clrFill == ((COLORREF)(-1L)) )
		hOldBrush = ::SelectObject( dc, ::GetStockObject(NULL_BRUSH) );
	else
	{
		VERIFY( _br.CreateSolidBrush( clrFillEffective ) );
		hOldBrush = ::SelectObject( dc, _br.GetSafeHandle() );
	}

CRect rc2D( ptFrom, ptTo );
	rc2D.NormalizeRect();
	if(			(	(	__IEDT_RECT_VALUE_MIN <= eTool
					&&	eTool <= __IEDT_RECT_VALUE_MAX
					)
				||	(	__IEDT_RECT_OUTLINED_VALUE_MIN <= eTool
					&&	eTool <= __IEDT_RECT_OUTLINED_VALUE_MAX
					)
				||	eTool == __IEDT_RECT_FILLED
				)
		)
		dc.Rectangle( &rc2D );
	else if(	(	(	__IEDT_ROUNDED_RECT_VALUE_MIN <= eTool
					&&	eTool <= __IEDT_ROUNDED_RECT_VALUE_MAX
					)
				||	(	__IEDT_ROUNDED_RECT_OUTLINED_VALUE_MIN <= eTool
					&&	eTool <= __IEDT_ROUNDED_RECT_OUTLINED_VALUE_MAX
					)
				||	eTool == __IEDT_ROUNDED_RECT_FILLED
				)
		)
	{
		CPoint ptRound(
			rc2D.Width() / 4,
			rc2D.Height() / 4
			);
		dc.RoundRect( &rc2D, ptRound );
	}
	else if(	(	(	__IEDT_ELLIPSE_VALUE_MIN <= eTool
					&&	eTool <= __IEDT_ELLIPSE_VALUE_MAX
					)
				||	(	__IEDT_ELLIPSE_OUTLINED_VALUE_MIN <= eTool
					&&	eTool <= __IEDT_ELLIPSE_OUTLINED_VALUE_MAX
					)
				||	eTool == __IEDT_ELLIPSE_FILLED
				)
		)
		dc.Ellipse( &rc2D );
#ifdef _DEBUG
	else
	{
		ASSERT( FALSE );
	}
#endif // _DEBUG

	::SelectObject( dc, hOldBrush );
	::SelectObject( dc, hOldPen );
}

void CExtImageEditWnd::stat_IedDrawToolLine(
	CExtImageEditWnd::e_tool_t eTool,
	CBitmap & bmpSurface,
	COLORREF clrPaint,
	CPoint ptFrom,
	CPoint ptTo
	)
{
	ASSERT( bmpSurface.GetSafeHandle() != NULL );

INT nPenWidth =
		(		eTool == __IEDT_BRUSH_CIRCLE_SMALL
			||	eTool == __IEDT_BRUSH_SQUARE_SMALL
			||	eTool == __IEDT_LINE_CIRCLE_SMALL
			||	eTool == __IEDT_LINE_SQUARE_SMALL
		)
			? 2
			: (	(		eTool == __IEDT_BRUSH_CIRCLE_MEDIUM
					||	eTool == __IEDT_BRUSH_SQUARE_MEDIUM
					||	eTool == __IEDT_LINE_CIRCLE_MEDIUM
					||	eTool == __IEDT_LINE_SQUARE_MEDIUM
					)
					? 4
					: 6
				)
		;
	if(		eTool == __IEDT_BRUSH_CIRCLE_SMALL
		||	eTool == __IEDT_BRUSH_CIRCLE_MEDIUM
		||	eTool == __IEDT_BRUSH_CIRCLE_LARGE
		||	eTool == __IEDT_LINE_CIRCLE_SMALL
		||	eTool == __IEDT_LINE_CIRCLE_MEDIUM
		||	eTool == __IEDT_LINE_CIRCLE_LARGE
		)
	{
		if(		eTool == __IEDT_LINE_CIRCLE_SMALL
			||	eTool == __IEDT_LINE_CIRCLE_MEDIUM
			||	eTool == __IEDT_LINE_CIRCLE_LARGE
			)
			nPenWidth /= 2;
		CPen _pen(
			PS_SOLID,
			nPenWidth,
			clrPaint
			);
		CSurfaceDC dc( bmpSurface );
		CPen * pOldPen = dc.SelectObject( &_pen );
		dc.MoveTo( ptFrom );
		dc.LineTo( ptTo );
		dc.SelectObject( pOldPen );
		if( eTool == __IEDT_LINE_CIRCLE_SMALL )
			dc.SetPixel( ptTo, clrPaint );
		return;
	}

bool bVert = true;
CPoint ptDist(
		abs( ptFrom.x - ptTo.x ),
		abs( ptFrom.y - ptTo.y )
		);
	if( ptDist.x > ptDist.y )
		bVert = false;
CPoint ptOffset(
		min(ptFrom.x,ptTo.x),
		min(ptFrom.y,ptTo.y)
		);
INT nEnd = bVert ? ptDist.y : ptDist.x;
bool bInv = (
			eTool == __IEDT_BRUSH_LINE_LD_SMALL
		||	eTool == __IEDT_BRUSH_LINE_LD_MEDIUM
		||	eTool == __IEDT_BRUSH_LINE_LD_LARGE
		||	eTool == __IEDT_LINE_LINE_LD_SMALL
		||	eTool == __IEDT_LINE_LINE_LD_MEDIUM
		||	eTool == __IEDT_LINE_LINE_LD_LARGE
		) ? false : true;
CPoint ptInvAdjust( 0, 0 ), ptPrev;
	if( bInv && ptDist.x != 0 && ptDist.y != 0 )
		ptInvAdjust =
			CPoint(
				(ptTo.x - ptFrom.x) / ( abs( ptFrom.x - ptTo.x ) ),
				(ptTo.y - ptFrom.y) / ( abs( ptFrom.y - ptTo.y ) )
				);

	if(		eTool == __IEDT_BRUSH_SQUARE_SMALL
		||	eTool == __IEDT_BRUSH_SQUARE_MEDIUM
		||	eTool == __IEDT_BRUSH_SQUARE_LARGE
		||	eTool == __IEDT_LINE_SQUARE_SMALL
		||	eTool == __IEDT_LINE_SQUARE_MEDIUM
		||	eTool == __IEDT_LINE_SQUARE_LARGE
		)
	{
		CSurfaceDC dc( bmpSurface );
		for( INT nStep = 0; true; nStep++ )
		{
			CPoint pt0;
			if( bVert )
			{
				if( ptDist.x == 0 )
					pt0.x = 0;
				else
					pt0.x = ::MulDiv(
						nStep,
						ptDist.x,
						(ptDist.y == 0) ? 1 : ptDist.y
						);
				pt0.y = nStep;
			} // if( bVert )
			else
			{
				pt0.x = nStep;
				if( ptDist.y == 0 )
					pt0.y = 0;
				else
					pt0.y = ::MulDiv(
						nStep,
						ptDist.y,
						(ptDist.x == 0) ? 1 : ptDist.x
						);
			} // else from if( bVert )
			if( ptFrom.x > ptTo.x )
				pt0.x = ptDist.x - pt0.x;
			if( ptFrom.y > ptTo.y )
				pt0.y = ptDist.y - pt0.y;
			CPoint ptCurr( ptOffset + pt0 );
			CSize _sizeSquare( nPenWidth, nPenWidth );
			CRect _rcSquare( ptCurr, _sizeSquare );
			dc.FillSolidRect( &_rcSquare, clrPaint );
			if( nStep == nEnd )
				break;
			ptPrev = ptCurr;
		} // for( INT nStep = nStart; true; nStep++ )
		return;
	}

	ASSERT(
			eTool == __IEDT_BRUSH_LINE_LD_SMALL
		||	eTool == __IEDT_BRUSH_LINE_LD_MEDIUM
		||	eTool == __IEDT_BRUSH_LINE_LD_LARGE
		||	eTool == __IEDT_BRUSH_LINE_RD_SMALL
		||	eTool == __IEDT_BRUSH_LINE_RD_MEDIUM
		||	eTool == __IEDT_BRUSH_LINE_RD_LARGE
		||	eTool == __IEDT_LINE_LINE_LD_SMALL
		||	eTool == __IEDT_LINE_LINE_LD_MEDIUM
		||	eTool == __IEDT_LINE_LINE_LD_LARGE
		||	eTool == __IEDT_LINE_LINE_RD_SMALL
		||	eTool == __IEDT_LINE_LINE_RD_MEDIUM
		||	eTool == __IEDT_LINE_LINE_RD_LARGE
		);

CSurfaceDC dc( bmpSurface );
CPen _pen( PS_SOLID, 1, clrPaint );
CPen * pOldPen = dc.SelectObject( &_pen );
	for( INT nStep = 0; true; nStep++ )
	{
		CPoint pt0;
		if( bVert )
		{
			if( ptDist.x == 0 )
				pt0.x = 0;
			else
				pt0.x = ::MulDiv(
					nStep,
					ptDist.x,
					(ptDist.y == 0) ? 1 : ptDist.y
					);
			pt0.y = nStep;
		} // if( bVert )
		else
		{
			pt0.x = nStep;
			if( ptDist.y == 0 )
				pt0.y = 0;
			else
				pt0.y = ::MulDiv(
					nStep,
					ptDist.y,
					(ptDist.x == 0) ? 1 : ptDist.x
					);
		} // else from if( bVert )
		if( ptFrom.x > ptTo.x )
			pt0.x = ptDist.x - pt0.x;
		if( ptFrom.y > ptTo.y )
			pt0.y = ptDist.y - pt0.y;
		CPoint pt1( 0, 0 );
		switch( eTool )
		{
		case __IEDT_BRUSH_LINE_LD_SMALL:
		case __IEDT_LINE_LINE_LD_SMALL:
			pt1.x = +1;
			pt1.y = -1;
		break;
		case __IEDT_BRUSH_LINE_LD_MEDIUM:
		case __IEDT_LINE_LINE_LD_MEDIUM:
			pt1.x = +2;
			pt1.y = -2;
		break;
		case __IEDT_BRUSH_LINE_LD_LARGE:
		case __IEDT_LINE_LINE_LD_LARGE:
			pt1.x = +3;
			pt1.y = -3;
		break;
		case __IEDT_BRUSH_LINE_RD_SMALL:
		case __IEDT_LINE_LINE_RD_SMALL:
			pt1.x = -1;
			pt1.y = -1;
		break;
		case __IEDT_BRUSH_LINE_RD_MEDIUM:
		case __IEDT_LINE_LINE_RD_MEDIUM:
			pt1.x = -2;
			pt1.y = -2;
		break;
		case __IEDT_BRUSH_LINE_RD_LARGE:
		case __IEDT_LINE_LINE_RD_LARGE:
			pt1.x = -3;
			pt1.y = -3;
		break;
#ifdef _DEBUG
		default:
			ASSERT( FALSE );
		break;
#endif // _DEBUG
		} // switch( eTool )
		CPoint ptCurr( ptOffset + pt0 );
		dc.MoveTo( ptCurr.x - pt1.x, ptCurr.y - pt1.y );
		dc.LineTo( ptCurr.x + pt1.x, ptCurr.y + pt1.y );
		if( nStep > 0 )
		{
			CPoint ptDistToPrev(
				abs( ptPrev.x - ptCurr.x ),
				abs( ptPrev.y - ptCurr.y )
				);
			ASSERT( ptDistToPrev.x <= 1 && ptDistToPrev.y <= 1 );
			if( ptDistToPrev.x == 1 && ptDistToPrev.y == 1 )
			{
				CPoint ptShifted( ptCurr );
				if( bVert )
				{
					if( bInv )
					{
						ptShifted.x -= ptInvAdjust.x;
					} // if( bInv )
					else
					{
						if( ptPrev.y > ptShifted.y )
							ptShifted.y++;
						else
							ptShifted.y--;
					} // else from if( bInv )
				} // if( bVert )
				else
				{
					if( bInv )
					{
						ptShifted.y -= ptInvAdjust.y;
					} // if( bInv )
					else
					{
						if( ptPrev.x > ptShifted.x )
							ptShifted.x++;
						else
							ptShifted.x--;
					} // else from if( bInv )
				} // else from if( bVert )
				dc.MoveTo( ptShifted.x - pt1.x, ptShifted.y - pt1.y );
				dc.LineTo( ptShifted.x + pt1.x, ptShifted.y + pt1.y );
			} // if( ptDistToPrev.x == 1 && ptDistToPrev.y == 1 )
		} // if( nStep > 0 )
		if( nStep == nEnd )
			break;
		ptPrev = ptCurr;
	} // for( INT nStep = nStart; true; nStep++ )
	dc.SelectObject( pOldPen );
}

__EXT_MFC_SAFE_LPCTSTR CExtImageEditWnd::stat_GetCurResFromToolHT(
	INT nToolHT
	)
{
	if( nToolHT == 8 )
		return IDC_SIZEALL;
	if( nToolHT==0 || nToolHT==4 )
		return IDC_SIZENWSE;
	if( nToolHT==1 || nToolHT==5 )
		return IDC_SIZENS;
	if( nToolHT==2 || nToolHT==6 )
		return IDC_SIZENESW;
	ASSERT( nToolHT==3 || nToolHT==7 );
	return IDC_SIZEWE;
}

HBRUSH CExtImageEditWnd::stat_GetHalftoneLightBrush(
	int nLighterSpec
	)
{
WORD _patternChashHalftone[8];
	if( nLighterSpec == 0 )
	{
		for( int nHTIdx = 0; nHTIdx < 8; nHTIdx++ )
			_patternChashHalftone[nHTIdx] =
				( (nHTIdx & 1) == 0 )
					? ( (WORD) 0 )
					: ( (WORD) (0x1111 << (nHTIdx & 0x03)) )
				;
	} // if( nLighterSpec == 0 )
	else if( nLighterSpec == 1 )
	{
		for( int nHTIdx = 0; nHTIdx < 8; nHTIdx++ )
			_patternChashHalftone[nHTIdx] =
				( (WORD) (0x1111 << (nHTIdx & 0x03)) )
				;
	} // else if( nLighterSpec == 1 )
	else
	{
		for( int nHTIdx = 0; nHTIdx < 8; nHTIdx++ )
			_patternChashHalftone[nHTIdx] =
				( (WORD) (0x5555 << (nHTIdx & 0x01)) )
				;
	} // else 
HBITMAP _bitmapChashHalftone =
		::CreateBitmap(
			8, 8, 1, 1,
			&_patternChashHalftone
			);
	if( _bitmapChashHalftone == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	}
HBRUSH hHalftoneBrush =
		::CreatePatternBrush( _bitmapChashHalftone );
	::DeleteObject( _bitmapChashHalftone );
	if( hHalftoneBrush == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	}
	return hHalftoneBrush;
}

bool CExtImageEditWnd::_OnKeyEsc()
{
	ASSERT_VALID( this );
	OnIedCancelActions( true );
	if( m_rcSelection.IsRectEmpty() )
		return false;
	_EmptyCache();
	Invalidate();
	UpdateWindow();
	return true;
}

bool CExtImageEditWnd::_OnKeyDel()
{
	ASSERT_VALID( this );
bool bCanceledAction = false;
	IedClear( true, true, &bCanceledAction );
	return bCanceledAction;
}

void CExtImageEditWnd::IedClear(
	bool bSelectionOnly, // = true
	bool bRedraw, // = true
	bool * p_bCanceledAction // = NULL
	)
{
	ASSERT_VALID( this );
	if( p_bCanceledAction != NULL )
		(*p_bCanceledAction) = (m_nTrackStep == 0) ? false : true;
	if( (IedGetStyle() & __EIEDWS_READONLY) != 0 )
	{
		OnIedCancelActions( false );
		return;
	}
	OnIedCancelActions( true );
CRect rcErase = m_rcSelection;
	if( m_rcSelection.IsRectEmpty() )
	{
		if( bSelectionOnly )
			return;
		rcErase = CRect( CPoint(0,0), IedSizeGet() );
		ASSERT( !rcErase.IsRectEmpty() );
	}
	if( p_bCanceledAction != NULL )
		(*p_bCanceledAction) = true;
	IedPushUndo( __EUT_SELECTION_DELETE );
	_EmptyCache();
	{ // BLOCK: surface DC
		CSurfaceDC dc( m_arrImgEditData[m_nUndoPos]->GetBmpSurface() );
		dc.FillSolidRect( &rcErase, IedColorBackgroundGet() );
	} // BLOCK: surface DC

	IedUndo( 0 );

	if( bRedraw )
	{
		if( (IedGetStyle() & __EIEDWS_OUTER_TOOLS) == 0 )
		{
			rcErase = OnIedPicToClient( rcErase, false );
			rcErase.InflateRect( OnIedMeasureToolSize() );
			rcErase.InflateRect( 2, 2 );
			InvalidateRect( &rcErase );
		} // if( (IedGetStyle() & __EIEDWS_OUTER_TOOLS) == 0 )
		else
			Invalidate();
		UpdateWindow();
	} // if( bRedraw )
}

void CExtImageEditWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	ASSERT_VALID( this );
	if( nChar == VK_ESCAPE )
	{
		_OnKeyEsc();
		return;
	} // if( nChar == VK_ESCAPE )
	if(		nChar == VK_DELETE
		&&	(! CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
		&&	(! CExtPopupMenuWnd::IsKeyPressed(VK_SHIFT) )
		&&	(! CExtPopupMenuWnd::IsKeyPressed(VK_MENU) )
		)
	{
		_OnKeyDel();
		return;
	}
	CExtScrollWnd ::OnKeyDown(nChar, nRepCnt, nFlags);
}

bool CExtImageEditWnd::OnIedSetCursor()
{
	ASSERT_VALID( this );
CPoint ptCursor;
	if( ! ::GetCursorPos(&ptCursor) )
		return false;
	if( ::WindowFromPoint(ptCursor) != m_hWnd )
		return false;
	if( (IedGetStyle() & __EIEDWS_READONLY) != 0 )
	{
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
		return true;
	} // if( (IedGetStyle() & __EIEDWS_READONLY) != 0 )
	ScreenToClient( &ptCursor );
HCURSOR hCursor = NULL;
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		CExtLocalResourceHelper _LRH;
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
e_tool_t eToolHelper = m_eToolSelected;
	if( m_nGripToolHT >= 0 )
	{
		hCursor =
			::LoadCursor(
				NULL,
				stat_GetCurResFromToolHT( m_nGripToolHT )
				);
		ASSERT( hCursor != NULL );
	} // if( m_nGripToolHT >= 0 )
	else if( m_eToolTracked == __IEDT_NONE )
	{
		__EXT_MFC_SAFE_LPCTSTR lpszCursorResourceID = NULL;
		INT nHT =
			_HitTestToolRects(
				ptCursor,
				m_rcSelection,
				&lpszCursorResourceID
				);
		if(		( (nHT >= 0 && nHT < 8) || nHT == (-2) )
			&&	(lpszCursorResourceID != NULL)
			)
		{
			hCursor =
				::LoadCursor(
					NULL,
					lpszCursorResourceID
					);
			ASSERT( hCursor != NULL );
		}
	} // else if( m_eToolTracked == __IEDT_NONE )
	else
		eToolHelper = m_eToolTracked;
	
	if( hCursor == NULL )
	{
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		LONG nResourceID = 0;
		if( eToolHelper == __IEDT_RECT_SELECTION )
			nResourceID = IDC_IED_TOOL_RS;
		else if( eToolHelper == __IEDT_COLOR_SELECTION )
			nResourceID = IDC_IED_TOOL_CP;
		else if( eToolHelper == __IEDT_FILL )
			nResourceID = IDC_IED_TOOL_FILL;
		else if( eToolHelper == __IEDT_MAGNIFICATON )
			nResourceID = IDC_IED_TOOL_MAGNIFY;
		else if( eToolHelper == __IEDT_PENCIL )
			nResourceID = IDC_IED_TOOL_PENCIL;
		else if(
				__IEDT_BRUSH_VALUE_MIN <= eToolHelper
			&&	eToolHelper <= __IEDT_BRUSH_VALUE_MAX
			)
			nResourceID = IDC_IED_TOOL_BRUSH;
		else if(
				__IEDT_AIRBRUSH_VALUE_MIN <= eToolHelper
			&&	eToolHelper <= __IEDT_AIRBRUSH_VALUE_MAX
			)
			nResourceID = IDC_IED_TOOL_AIRBRUSH;
		else if(
				__IEDT_LINE_VALUE_MIN <= eToolHelper
			&&	eToolHelper <= __IEDT_LINE_VALUE_MAX
			)
			nResourceID = IDC_IED_TOOL_LINE;
		else if(
				(	__IEDT_RECT_VALUE_MIN <= eToolHelper
					&&	eToolHelper <= __IEDT_RECT_VALUE_MAX
				)
			||	(	__IEDT_RECT_OUTLINED_VALUE_MIN <= eToolHelper
					&&	eToolHelper <= __IEDT_RECT_OUTLINED_VALUE_MAX
				)
			||	eToolHelper == __IEDT_RECT_FILLED
			)
			nResourceID = IDC_IED_TOOL_RECT;
		else if(
				(	__IEDT_ROUNDED_RECT_VALUE_MIN <= eToolHelper
					&&	eToolHelper <= __IEDT_ROUNDED_RECT_VALUE_MAX
				)
			||	(	__IEDT_ROUNDED_RECT_OUTLINED_VALUE_MIN <= eToolHelper
					&&	eToolHelper <= __IEDT_ROUNDED_RECT_OUTLINED_VALUE_MAX
				)
			||	eToolHelper == __IEDT_ROUNDED_RECT_FILLED
			)
			nResourceID = IDC_IED_TOOL_RR;
		else if(
				(	__IEDT_ELLIPSE_VALUE_MIN <= eToolHelper
					&&	eToolHelper <= __IEDT_ELLIPSE_VALUE_MAX
				)
			||	(	__IEDT_ELLIPSE_OUTLINED_VALUE_MIN <= eToolHelper
					&&	eToolHelper <= __IEDT_ELLIPSE_OUTLINED_VALUE_MAX
				)
			||	eToolHelper == __IEDT_ELLIPSE_FILLED
			)
			nResourceID = IDC_IED_TOOL_ELLIPSE;
		if( nResourceID != 0 )
		{
			HINSTANCE hInstResource =
				::AfxFindResourceHandle(
					MAKEINTRESOURCE( nResourceID ),
					RT_ICON
					);
			if( hInstResource != NULL )
				hCursor =
					::LoadCursor(
						hInstResource,
						MAKEINTRESOURCE( nResourceID )
						);
		} // if( nResourceID != 0 )
		if( hCursor == NULL )
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		{
			hCursor =
				(g_hCursor == NULL)
					? ( ::LoadCursor( NULL, IDC_CROSS ) )
					: g_hCursor
					;
			ASSERT( hCursor != NULL );
		} // if( hCursor == NULL )
	} // if( hCursor == NULL )

	if( hCursor == NULL )
	{
		CSize _size = IedSizeGet();
		CRect _rect( 0, 0, _size.cx, _size.cy );
		_rect = OnIedPicToClient( _rect, false );
		if( _rect.PtInRect(ptCursor) )
		{
			hCursor =
				(g_hCursor == NULL)
					? ( ::LoadCursor( NULL, IDC_CROSS ) )
					: g_hCursor
					;
			ASSERT( hCursor != NULL );
		} // if( _rect.PtInRect(ptCursor) )
	} // if( hCursor == NULL )

	if( hCursor != NULL )
	{
		::SetCursor( hCursor );
		return true;
	}

	return false;
}

BOOL CExtImageEditWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	ASSERT_VALID( this );
	if( nHitTest == HTCLIENT )
	{
		if( OnIedSetCursor() )	
			return TRUE;
	} // if( nHitTest == HTCLIENT )
	return CExtScrollWnd ::OnSetCursor(pWnd, nHitTest, message);
}

void CExtImageEditWnd::PreSubclassWindow() 
{
	CExtScrollWnd ::PreSubclassWindow();

	if( m_bPresubclassDialogMode )
	{
		DWORD dwStyle = ::GetWindowLong( m_hWnd, GWL_STYLE );
		m_dwIedStyles = dwStyle & __EXTMFC_ALL_FORM_MOVABLE_WND_STYLES;
		::SetWindowLong( m_hWnd, GWL_STYLE, dwStyle & (~__EXTMFC_ALL_FORM_MOVABLE_WND_STYLES) );
	} // if( m_bPresubclassDialogMode )

	if(		(IedGetStyle() & __EIEDWS_REQUEST_FOCUS) == 0
		&&	( GetStyle() & WS_TABSTOP ) != 0
		)
		IedModifyStyle( 0, __EIEDWS_REQUEST_FOCUS, false );
}

void CExtImageEditWnd::PostNcDestroy() 
{
	for(	INT nImgIdx = 1;
			nImgIdx < m_arrImgEditData.GetSize();
			nImgIdx++
			)
	{
		IMAGEEDITDATA * pImgEditData =
			m_arrImgEditData[nImgIdx];
		ASSERT( pImgEditData != NULL );
		delete pImgEditData;
	}
	m_arrImgEditData.RemoveAt( 1, m_arrImgEditData.GetSize()-1 );
	m_arrImgEditData[0]->Empty();
	m_eToolSelected = m_eToolSelPrev = __IEDT_RECT_SELECTION;
	m_eToolTracked = __IEDT_NONE;
	m_nMagnification = 1;
	m_nTrackStep = 0;
	m_nUndoPos = 0;
	m_arrTrackPoints.RemoveAll();
	_EmptyCache();
	m_nGripToolHT = -1;
	ASSERT_VALID( this );
	
	m_bPresubclassDialogMode = true;
	CExtScrollWnd ::PostNcDestroy();
}

void CExtImageEditWnd::OnTimer(UINT nIDEvent) 
{
	ASSERT_VALID( this );
UINT nAirbrushTimerID = __IED_DEF_AIRBRUSH_TIMER_ID;
	OnIedQueryAirbrushTimerParms(
		&nAirbrushTimerID
		);
	if( nIDEvent == nAirbrushTimerID )
	{
		if(		__IEDT_AIRBRUSH_VALUE_MIN <= m_eToolTracked
			&&	m_eToolTracked <= __IEDT_AIRBRUSH_VALUE_MAX
			)
		{
			ASSERT( m_arrTrackPoints.GetSize() == 2 );
			BOOL bLB = CExtPopupMenuWnd::IsKeyPressed( VK_LBUTTON );
			BOOL bRB = CExtPopupMenuWnd::IsKeyPressed( VK_RBUTTON );
			if( bLB || bRB )
			{
				INT nRadius = 0, nPixelsPerStep = 0;
				OnIedQueryAirbrushTimerParms(
					NULL,
					NULL,
					m_eToolTracked,
					&nRadius,
					&nPixelsPerStep
					);
				if( nRadius > 0 && nPixelsPerStep > 0 )
				{
					COLORREF clrPaint = bLB
						? IedColorForegroundGet()
						: IedColorBackgroundGet()
						;
					CRect rcPaint( m_arrTrackPoints[1], m_arrTrackPoints[1] );
					rcPaint.InflateRect( nRadius+1, nRadius+1 );
					{ // BLOCK: for surface DC
						CSurfaceDC dc( m_arrImgEditData[m_nUndoPos]->GetBmpSurface() );
						for( int nPixelIdx = 0; nPixelIdx < nPixelsPerStep; nPixelIdx++ )
						{
							double lfR = double( ::rand() % nRadius );
							INT nA = ::rand() % 1024;
							double lfA = ( (2.0 * 3.1415926535897932384626433832795) * double(nA) ) / 1024.0;
							double nX = lfR * ::cos( lfA );
							double nY = lfR * ::sin( lfA );
							CPoint pt(
								m_arrTrackPoints[1].x + INT(nX),
								m_arrTrackPoints[1].y + INT(nY)
								);
							dc.SetPixel( pt, clrPaint );
						}
					} // BLOCK: for surface DC
					rcPaint = OnIedPicToClient( rcPaint, false );

					IedUndo( 0 );
					
					InvalidateRect( &rcPaint );
					UpdateWindow();
				} // if( nRadius > 0 && nPixelsPerStep > 0 )
				return;
			} // if( bLB || bRB )
		}
		KillTimer( nAirbrushTimerID );
		OnIedCancelActions( false );
		return;
	} // if( nIDEvent == nAirbrushTimerID )
	
	CExtScrollWnd ::OnTimer(nIDEvent);
}

BOOL CExtImageEditWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	if(		(! RegisterImageEditWndClass() )
		||	(! CExtScrollWnd::PreCreateWindow(cs) )
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}
	cs.lpszClass = __EXT_IMAGE_EDIT_WND_CLASS_NAME;
	return TRUE;
}

BOOL CExtImageEditWnd::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN )
	{
		if( int(pMsg->wParam) == VK_ESCAPE )
			return _OnKeyEsc() ? TRUE : FALSE;
		if(		int(pMsg->wParam) == VK_DELETE
			&&	(! CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
			&&	(! CExtPopupMenuWnd::IsKeyPressed(VK_SHIFT) )
			&&	(! CExtPopupMenuWnd::IsKeyPressed(VK_MENU) )
			)
			return _OnKeyDel() ? TRUE : FALSE;
	} // if( pMsg->message == WM_KEYDOWN )
	
	return CExtScrollWnd ::PreTranslateMessage(pMsg);
}

#endif // (!defined __EXT_MFC_NO_IMAGEEDITWND)
