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

#if (!defined __EXT_EDIT_H)
	#include <ExtEdit.h>
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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE( CExtEditBase, CEdit );

CExtEditBase::CExtEditBase()
	: m_bHandleCtxMenus( true )
{
}

CExtEditBase::~CExtEditBase()
{
}

BEGIN_MESSAGE_MAP( CExtEditBase, CEdit )
	//{{AFX_MSG_MAP(CExtEditBase)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CExtEditBase::OnContextMenu(CWnd* pWnd,CPoint pos )
{
	if( ! m_bHandleCtxMenus )
	{
		CEdit::OnContextMenu( pWnd, pos );
		return;
	} // if( ! m_bHandleCtxMenus )
LPCTSTR strProfileName =
		g_CmdManager->ProfileNameFromWnd( GetSafeHwnd() );
	if( strProfileName == NULL ){
		CEdit::OnContextMenu( pWnd, pos );
		return;
	}
CExtPopupMenuWnd * pPopup = new CExtPopupMenuWnd;
	if( !pPopup->CreatePopupMenu( GetSafeHwnd() ) )
	{
		ASSERT( FALSE );
		delete pPopup;
		CEdit::OnContextMenu( pWnd, pos );
		return;
	}
static struct
{
	UINT m_nCmdID;
	LPCTSTR m_sMenuText;
} arrCmds[] =
{
	{ ID_EDIT_UNDO, _T("&Undo") },
//	{ ID_EDIT_REDO, _T("&Redo") },
	{ ID_SEPARATOR, NULL },
	{ ID_EDIT_CUT, _T("Cu&t") },
	{ ID_EDIT_COPY, _T("&Copy") },
	{ ID_EDIT_PASTE, _T("&Paste") },
	{ ID_EDIT_CLEAR, _T("Cl&ear") },
	{ ID_SEPARATOR, NULL },
	{ ID_EDIT_SELECT_ALL, _T("Select &All") },
};
	for( int i = 0; i < sizeof(arrCmds)/sizeof(arrCmds[0]); i++ )
	{
		if( arrCmds[i].m_nCmdID == ID_SEPARATOR )
		{
			VERIFY( pPopup->ItemInsert( ID_SEPARATOR ) );
			continue;
		} // if( arrCmds[i].m_nCmdID == ID_SEPARATOR )
		CExtCmdItem * pCmdItem =
			g_CmdManager->CmdGetPtr(
				strProfileName,
				arrCmds[i].m_nCmdID
				);
		if( pCmdItem == NULL )
			pCmdItem =
				g_CmdManager->CmdAllocPtr(
					strProfileName,
					arrCmds[i].m_nCmdID
					);
		ASSERT( pCmdItem != NULL );
		if( pCmdItem == NULL )
			continue;
		if( pCmdItem->m_sMenuText.IsEmpty() )
		{
			CExtSafeString sText;
			if( g_ResourceManager->LoadString( sText, arrCmds[i].m_nCmdID ) )
			{
				sText.Replace( _T("\t"), _T(" ") );
				sText.Replace( _T("\r"), _T("") );
				sText.TrimLeft();
				sText.TrimRight();
				if( ! sText.IsEmpty() )
				{
					int nSep = sText.ReverseFind('\n');
					if( nSep < 0 )
					{
						pCmdItem->m_sMenuText = sText;
					} // if( nSep < 0 )
					else
					{
						int nLen = sText.GetLength();
						pCmdItem->m_sMenuText = sText.Right( nLen - nSep );
						pCmdItem->m_sMenuText.TrimLeft();
						pCmdItem->m_sMenuText.TrimRight();
					} // else from if( nSep < 0 )
				} // if( ! sText.IsEmpty() )
			}
			if( pCmdItem->m_sMenuText.IsEmpty() )
				pCmdItem->m_sMenuText = arrCmds[i].m_sMenuText;
		} // if( pCmdItem->m_sMenuText.IsEmpty() )
		pCmdItem->StateSetBasic();
		VERIFY( pPopup->ItemInsert( arrCmds[i].m_nCmdID ) );
	} // for( int i = 0; i < sizeof(arrCmds)/sizeof(arrCmds[0]); i++ )
CRect rcClient;
	GetClientRect( &rcClient );
	ClientToScreen( &rcClient );
	if( !rcClient.PtInRect( pos ) )
		pos = rcClient.CenterPoint();
	SetFocus();
	
	if(	!pPopup->TrackPopupMenu(
			TPMX_OWNERDRAW_FIXED|TPMX_DO_MESSAGE_LOOP,
			pos.x, 
			pos.y
			) 
		)
	{
		ASSERT( FALSE );
		delete pPopup;
		CEdit::OnContextMenu( pWnd, pos );
		return;
	}
	else
	{
		VERIFY(
			RedrawWindow(
			NULL,
			NULL,
			RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW
			|RDW_FRAME|RDW_ALLCHILDREN
			)
			);
	}
}

BOOL CExtEditBase::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if(		m_bHandleCtxMenus
		&&	(	nCode == CN_UPDATE_COMMAND_UI
			||	nCode == CN_COMMAND
			)
		)
	{
		bool bReadOnly = ( (GetStyle() & ES_READONLY) != 0 ) ? true : false;
		DWORD nSelection = GetSel();
		if( nID == ID_EDIT_UNDO )
		{
			if( nCode == CN_UPDATE_COMMAND_UI )
			{
				CCmdUI * pCmdUI = (CCmdUI *)pExtra;
				ASSERT( pCmdUI != NULL );
				pCmdUI->Enable( (CanUndo() && (!bReadOnly)) ? TRUE : FALSE );
			} // if( nCode == CN_UPDATE_COMMAND_UI )
			else
				Undo();
			return TRUE;
		} // if( nID == ID_EDIT_UNDO )

//		if( nID == ID_EDIT_REDO )
//		{
//			if( nCode == CN_UPDATE_COMMAND_UI )
//			{
//				CCmdUI * pCmdUI = (CCmdUI *)pExtra;
//				ASSERT( pCmdUI != NULL );
//				pCmdUI->Enable(
//					(BOOL) (SendMessage( EM_CANREDO ) && (!bReadOnly)
//					);
//			} // if( nCode == CN_UPDATE_COMMAND_UI )
//			else
//				SendMessage( EM_REDO );
//			return TRUE;
//		} // if( nID == ID_EDIT_REDO )
		if( nID == ID_EDIT_CUT )
		{
			if( nCode == CN_UPDATE_COMMAND_UI )
			{
				CCmdUI * pCmdUI = (CCmdUI *)pExtra;
				ASSERT( pCmdUI != NULL );
				pCmdUI->Enable(
						(LOWORD(nSelection)) != (HIWORD(nSelection))
					&&	(!bReadOnly)
					);
			} // if( nCode == CN_UPDATE_COMMAND_UI )
			else
				SendMessage( WM_CUT );
			return TRUE;
		} // if( nID == ID_EDIT_CUT )
		if( nID == ID_EDIT_COPY )
		{
			if( nCode == CN_UPDATE_COMMAND_UI )
			{
				CCmdUI * pCmdUI = (CCmdUI *)pExtra;
				ASSERT( pCmdUI != NULL );
				pCmdUI->Enable(
					(LOWORD(nSelection)) != (HIWORD(nSelection))
					);
			} // if( nCode == CN_UPDATE_COMMAND_UI )
			else
				SendMessage( WM_COPY );
			return TRUE;
		} // if( nID == ID_EDIT_COPY )
		if( nID == ID_EDIT_PASTE )
		{
			if( nCode == CN_UPDATE_COMMAND_UI )
			{
				CCmdUI * pCmdUI = (CCmdUI *)pExtra;
				ASSERT( pCmdUI != NULL );
				pCmdUI->Enable(
						IsClipboardFormatAvailable(CF_TEXT) 
					&&	(!bReadOnly)
					);
			} // if( nCode == CN_UPDATE_COMMAND_UI )
			else
				SendMessage( WM_PASTE );
			return TRUE;
		} // if( nID == ID_EDIT_PASTE )
		if( nID == ID_EDIT_CLEAR )
		{
			if( nCode == CN_UPDATE_COMMAND_UI )
			{
				CCmdUI * pCmdUI = (CCmdUI *)pExtra;
				ASSERT( pCmdUI != NULL );
				pCmdUI->Enable(
						(LOWORD(nSelection)) != (HIWORD(nSelection))
					&&	(!bReadOnly)
					);
			} // if( nCode == CN_UPDATE_COMMAND_UI )
			else
				SendMessage( WM_CLEAR );
			return TRUE;
		} // if( nID == ID_EDIT_CLEAR )
		if( nID == ID_EDIT_SELECT_ALL )
		{
			if( nCode == CN_UPDATE_COMMAND_UI )
			{
				CCmdUI * pCmdUI = (CCmdUI *)pExtra;
				ASSERT( pCmdUI != NULL );
				
				int nLength = GetWindowTextLength();
				pCmdUI->Enable(
						nLength > 0
					&&	(!(		(LOWORD(nSelection)) == 0 
							&&	(HIWORD(nSelection)) == nLength
						))
					);
			} // if( nCode == CN_UPDATE_COMMAND_UI )
			else
				SetSel( 0, -1 );
			return TRUE;
		} // if( nID == ID_EDIT_SELECT_ALL )
	} // if( m_bHandleCtxMenus ...
	return CEdit::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

/////////////////////////////////////////////////////////////////////////////
// CExtEdit

IMPLEMENT_DYNCREATE( CExtEdit, CExtEditBase );

CExtEdit::CExtEdit()
	: m_bMouseOver( false )
{
}

CExtEdit::~CExtEdit()
{
}

BEGIN_MESSAGE_MAP(CExtEdit, CExtEditBase)
	//{{AFX_MSG_MAP(CExtEdit)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_ERASEBKGND()
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtEdit message handlers
void CExtEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND(
			GetSafeHwnd()
			)
		)
		SetTimer(1, 50, NULL);
	CExtEditBase::OnMouseMove(nFlags, point);
}

void CExtEdit::OnTimer(UINT nIDEvent) 
{
	if( nIDEvent != 1 )
	{
		CExtEditBase::OnTimer( nIDEvent );
		return;
	}
POINT pt;
	if( ! ::GetCursorPos(&pt) )
		return;
	if( IsWindowVisible() )
	{
		bool bOldMouseOver = m_bMouseOver;
		CRect rectItem;
		GetWindowRect( &rectItem );
		if( !rectItem.PtInRect(pt) )
		{
			KillTimer(1);
			m_bMouseOver = false;
		}
		else if( ::WindowFromPoint( pt ) == m_hWnd )
			m_bMouseOver = true;
		if( bOldMouseOver != m_bMouseOver )
			_PostRedraw();
	} // if( IsWindowVisible() )
}

void CExtEdit::OnPaint() 
{
	CExtEditBase::OnPaint();
/*
CRect rectClient;
	GetClientRect( &rectClient );
CPaintDC dcPaint( this );
CExtMemoryDC dcmm( &dcPaint, &rectClient );
	DefWindowProc( WM_PAINT, (WPARAM)dcmm.GetSafeHdc(), (LPARAM)0 );
*/
}

void CExtEdit::_PostRedraw()
{
	VERIFY(
		RedrawWindow(
			NULL,
			NULL,
			RDW_INVALIDATE //|RDW_UPDATENOW
				|RDW_ERASE //|RDW_ERASENOW
				|RDW_FRAME|RDW_ALLCHILDREN
			)
		);
}

void CExtEdit::_DrawEditImpl(
	CRect rectClient,
	CDC * pDC // = NULL
	)
{
bool bNeedReleaseDC = false;
	if( pDC == NULL )
	{
		pDC = GetDC();
		bNeedReleaseDC = true;
	}
	ASSERT_VALID( pDC );
	rectClient.DeflateRect(1,1);
bool bFocus = false;
	if(	GetFocus()->GetSafeHwnd() == m_hWnd
		&& (!CExtPopupMenuWnd::IsMenuTracking())
		)
		bFocus = true;

bool bEnabled = OnQueryWindowEnabledState();

CExtPaintManager::PAINTCONTROLFRAMEDATA _pcfd(
		this,
		rectClient,
		true,
		m_bMouseOver,
		bEnabled,
		bFocus
		);
	g_PaintManager->PaintControlFrame( *pDC, _pcfd );

	if( bNeedReleaseDC )
		ReleaseDC(pDC);
}

void CExtEdit::SetIndent(int margin)
{
	SetMargins(margin, 1);
}

void CExtEdit::OnSetFocus(CWnd* pOldWnd) 
{
	CExtEditBase::OnSetFocus(pOldWnd);
//	m_bMouseOver = true;
	_PostRedraw();
}

void CExtEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CExtEditBase::OnKillFocus(pNewWnd);
//	m_bMouseOver = false;
	_PostRedraw();
}

BOOL CExtEdit::OnEraseBkgnd(CDC* pDC) 
{
	pDC;
	//return TRUE;
	return CExtEditBase::OnEraseBkgnd(pDC);
}

void CExtEdit::OnNcPaint() 
{
CWindowDC dc(this);
CRect rcClient, rcBar;
	GetClientRect(rcClient);
	ClientToScreen(rcClient);
	GetWindowRect(rcBar);
	rcClient.OffsetRect(-rcBar.TopLeft());
	rcBar.OffsetRect(-rcBar.TopLeft());
	// client area is not our bussiness :)
	dc.ExcludeClipRect(rcClient);
CExtMemoryDC dcmm( &dc, &rcBar );
	rcBar.DeflateRect(3,3);
	_DrawEditImpl( rcBar, &dcmm );	
}

void CExtEdit::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	CExtEditBase::OnNcCalcSize(bCalcValidRects, lpncsp);
//CRect rc( lpncsp->rgrc[0] );
//	rc.DeflateRect(4,4);
//	lpncsp->rgrc[0] = rc;
}

