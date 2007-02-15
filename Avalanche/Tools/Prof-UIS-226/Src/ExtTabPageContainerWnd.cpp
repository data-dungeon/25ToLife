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

#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)

#if (! defined __EXT_TAB_PAGE_CONTAINER_WND_H)
	#include <ExtTabPageContainerWnd.h>
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

/////////////////////////////////////////////////////////////////////////////
// CExtTabPageContainerWnd

CExtTabPageContainerWnd::CExtTabPageContainerWnd()
	: m_bDirectCreateCall( false )
	, m_pWndTab( NULL )
{
	VERIFY( RegisterTabCtrlWndClass() );
}

CExtTabPageContainerWnd::~CExtTabPageContainerWnd()
{
}

IMPLEMENT_DYNCREATE(CExtTabPageContainerWnd, CWnd)

BEGIN_MESSAGE_MAP(CExtTabPageContainerWnd, CWnd)
	//{{AFX_MSG_MAP(CExtTabPageContainerWnd)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CExtTabPageContainerWnd message handlers

HCURSOR CExtTabPageContainerWnd::g_hCursor = ::LoadCursor( NULL, IDC_ARROW );
bool CExtTabPageContainerWnd::g_bTabCtrlWndClassRegistered = false;

bool CExtTabPageContainerWnd::RegisterTabCtrlWndClass()
{
	if( g_bTabCtrlWndClassRegistered )
		return true;
	
	WNDCLASS wndclass;
	HINSTANCE hInst = AfxGetInstanceHandle();
	if( ! ::GetClassInfo(
		hInst,
		__EXT_TAB_PAGE_CONTAINER_CLASS_NAME,
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
			: ::LoadCursor(
			NULL, //hInst,
			IDC_ARROW
			)
			;
		ASSERT( wndclass.hCursor != NULL );
		wndclass.hbrBackground = NULL; 
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = __EXT_TAB_PAGE_CONTAINER_CLASS_NAME;
		if( !::AfxRegisterClass( &wndclass ) )
		{
			ASSERT( FALSE );
			//AfxThrowResourceException();
			return false;
		}
	}
	
	g_bTabCtrlWndClassRegistered = true;
	return true;
}

//////////////////////////////////////////////////////////////////////////

BOOL CExtTabPageContainerWnd::Create(
						CWnd * pParentWnd,
						const RECT & rcWnd, // = CRect( 0, 0, 0, 0 )
						UINT nDlgCtrlID, // = UINT( IDC_STATIC )
						DWORD dwWindowStyle, // = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS
						CCreateContext * pContext // = NULL
						)
{
	if( !RegisterTabCtrlWndClass() )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	m_bDirectCreateCall = true;
	if( ! CWnd::Create(
		__EXT_TAB_PAGE_CONTAINER_CLASS_NAME,
		NULL,
		dwWindowStyle,
		rcWnd,
		pParentWnd,
		nDlgCtrlID,
		pContext
		)
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}

	if( m_pWndTab == NULL )
		m_pWndTab = OnTabWndGetTabImpl();
	ASSERT_VALID(m_pWndTab);
	if( m_pWndTab != NULL && m_pWndTab->m_hWnd == NULL )
	{
		VERIFY(
			m_pWndTab->Create(
				this,
				CRect( 0, 0, 0, 0 ),
				0x100,
				WS_CHILD|WS_VISIBLE|WS_TABSTOP,
				__ETWS_ORIENT_BOTTOM,
				NULL
				)
			);	
	} // if( m_pWndTab != NULL && m_pWndTab->m_hWnd == NULL )
	
	_RepositionBarsImpl();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

LRESULT CExtTabPageContainerWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch( message )
	{
	case WM_CLOSE:
		if( (GetStyle() & WS_CHILD) != 0 )
			return 0L;
	break;
	case WM_PAINT:
		{
			CPaintDC dcPaint( this );
			CExtPaintManager::stat_ExcludeChildAreas(
				dcPaint.GetSafeHdc(),
				GetSafeHwnd()
				);
			CRect rcClient;
			GetClientRect( &rcClient );
			CExtMemoryDC dc(
				&dcPaint,
				&rcClient
				);
			dc.FillSolidRect(
				&rcClient,
				g_PaintManager->GetColor(
				CExtPaintManager::CLR_3DFACE_OUT
				)
				);
		}
		return TRUE;
		
	case WM_ERASEBKGND:
		return FALSE;
		
	case WM_SIZE:
	case WM_WINDOWPOSCHANGED:
		_RepositionBarsImpl();
		break;
	case WM_CREATE:
			if( m_pWndTab == NULL )
				m_pWndTab = OnTabWndGetTabImpl();
			ASSERT_VALID(m_pWndTab);
			if( m_pWndTab != NULL && m_pWndTab->m_hWnd == NULL )
			{
				VERIFY(
					m_pWndTab->Create(
						this,
						CRect( 0, 0, 0, 0 ),
						0x100,
						WS_CHILD|WS_VISIBLE|WS_TABSTOP,
						__ETWS_ORIENT_BOTTOM,
						NULL
						)
					);	
			} // if( m_pWndTab != NULL && m_pWndTab->m_hWnd == NULL )
		break;
	} // switch( message )
	
	return CWnd::WindowProc(message, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////

BOOL CExtTabPageContainerWnd::PreTranslateMessage(MSG* pMsg) 
{
	if(		pMsg->message == WM_KEYDOWN
		&&	(	pMsg->wParam == VK_PRIOR
				||	pMsg->wParam == VK_NEXT
		
		)
		&&	CExtPopupMenuWnd::IsKeyPressed( VK_CONTROL )
		&&	(! CExtPopupMenuWnd::IsKeyPressed( VK_SHIFT ) )
		&&	(! CExtPopupMenuWnd::IsKeyPressed( VK_MENU ) )
		)
	{
		PagesNavigate((pMsg->wParam == VK_NEXT));
		return TRUE;
	}
	return CWnd::PreTranslateMessage( pMsg );
}

//////////////////////////////////////////////////////////////////////////

BOOL CExtTabPageContainerWnd::PageInsert(
	HWND hWnd,
	__EXT_MFC_SAFE_LPCTSTR sItemText/* = NULL*/,
	HICON hIcon/* = NULL*/,
	bool bCopyIcon/* = true*/,
	int nPos/* = -1*/, // append
	bool bSelect/* = false*/)
{
	ASSERT_VALID( this );
	if(		hWnd   == NULL || (! ::IsWindow(hWnd)   )
		||	m_hWnd == NULL || (! ::IsWindow(m_hWnd) )
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}
	int nCount = m_pWndTab->ItemGetCount();
	if( nCount == 0 )
		bSelect = true;
	if( nPos < 0 || nPos > nCount )
		nPos = nCount; // append
	if( ::GetParent(hWnd) != m_hWnd )
		::SetParent( hWnd, m_hWnd );
	if( bSelect )
	{
		if( nCount > 0 )
		{
			int i = (int)m_pWndTab->SelectionGet();
			if( i >= 0 )
				::ShowWindow(
					(HWND)m_pWndTab->ItemLParamGet(i),
					SW_HIDE
					);
		} // if( nCount > 0 )
		if( ( ::GetWindowLong( hWnd, GWL_STYLE ) & WS_VISIBLE ) == 0 )
			::ShowWindow( hWnd, SW_SHOW );
	} // if( bSelect )
	else
	{
		if( ( ::GetWindowLong( hWnd, GWL_STYLE ) & WS_VISIBLE ) != 0 )
			::ShowWindow( hWnd, SW_HIDE );
	} // else from if( bSelect )
	
	m_pWndTab->ItemInsert(
		sItemText,
		hIcon,
		bCopyIcon,
		0,
		(LONG)nPos,
		(LPARAM)hWnd,
		false
		);
	
	if( nPos == nCount )
		::SetWindowLong( hWnd, GWL_ID, 0x101+nPos );
	else
	{
		for( int i = 0; i < nCount; i++ )
			::SetWindowLong(
			(HWND)m_pWndTab->ItemLParamGet(i),
			GWL_ID,
			0x101+i
			);
	} // if( nPos == nCount )
	if( bSelect )
		m_pWndTab->SelectionSet( nPos, true, true );
	else
		m_pWndTab->UpdateTabWnd( true );

	_RepositionBarsImpl();
				
	return true;
}

//////////////////////////////////////////////////////////////////////////

BOOL CExtTabPageContainerWnd::PageInsert(
	CWnd * pWnd,
	__EXT_MFC_SAFE_LPCTSTR sItemText,// = NULL,
	HICON hIcon,// = NULL,
	bool bCopyIcon,// = true,
	int nPos,//= -1, // append
	bool bSelect/* = false */)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pWnd );
	return
		PageInsert(
		pWnd->GetSafeHwnd(),
		sItemText,
		hIcon,
		bCopyIcon,
		nPos,
		bSelect
		);
}

//////////////////////////////////////////////////////////////////////////

void CExtTabPageContainerWnd::PreSubclassWindow() 
{
	if( m_bDirectCreateCall )
		return;
	CWnd::PreSubclassWindow();
	if( m_pWndTab == NULL )
		m_pWndTab = OnTabWndGetTabImpl();
	ASSERT_VALID(m_pWndTab);
	if( m_pWndTab != NULL && m_pWndTab->m_hWnd == NULL )
	{
		VERIFY(
			m_pWndTab->Create(
				this,
				CRect( 0, 0, 0, 0 ),
				0x100,
				WS_CHILD|WS_VISIBLE|WS_TABSTOP,
				__ETWS_ORIENT_BOTTOM,
				NULL
				)
			);	
	} // if( m_pWndTab != NULL && m_pWndTab->m_hWnd == NULL )
}

//////////////////////////////////////////////////////////////////////////

bool CExtTabPageContainerWnd::OnHookWndMsg(
	LRESULT & lResult,
	HWND hWndHooked,
	UINT nMessage,
	WPARAM & wParam,
	LPARAM & lParam
	)
{
	__PROF_UIS_MANAGE_STATE;

	ASSERT_VALID( this );
	
	if(		nMessage == WM_KEYDOWN
		&&	(	wParam == VK_PRIOR || wParam == VK_NEXT	)
		&&	CExtPopupMenuWnd::IsKeyPressed( VK_CONTROL )
		&&	(! CExtPopupMenuWnd::IsKeyPressed( VK_SHIFT ) )
		&&	(! CExtPopupMenuWnd::IsKeyPressed( VK_MENU ) )
		)
	{
		PagesNavigate((wParam == VK_NEXT));
		return true;
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

//////////////////////////////////////////////////////////////////////////

BOOL CExtTabPageContainerWnd::PagesNavigate(BOOL bNext)
{
	int nCount =
		(m_pWndTab->GetSafeHwnd() != NULL)
		? m_pWndTab->ItemGetCount()
		: 0;
	if( nCount > 1 )
	{
		HWND hWndFocus = ::GetFocus();
		if( hWndFocus != NULL && ::IsChild(m_hWnd,hWndFocus) )
		{
			int nCurSel = m_pWndTab->SelectionGet();
			if( bNext )
				nCurSel ++;
			else
				nCurSel --;
			if( nCurSel >= nCount )
				nCurSel = 0;
			else if( nCurSel < 0 )
				nCurSel = nCount - 1;
			m_pWndTab->SelectionSet( nCurSel, true, true );
			return TRUE;
		} // if( hWndFocus != NULL && ::IsChild(m_hWnd,hWndFocus) )
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

ULONG CExtTabPageContainerWnd::SetupHookWndSinkToChilds(
	 HWND hWnd,
	 UINT * pDlgCtrlIDs, // = NULL
	 ULONG nCountOfDlgCtrlIDs, // = 0
	 bool bDeep // = false
	 )
{
	ASSERT( hWnd != NULL );
	if( hWnd == NULL )
		return 0;
	
	ASSERT( ::IsWindow(hWnd) );
	if( !::IsWindow(hWnd) )
		return 0;
	ULONG nCountOfHooks = 0;
	hWnd = ::GetWindow( hWnd, GW_CHILD );
	for( ; hWnd != NULL; hWnd = ::GetWindow( hWnd, GW_HWNDNEXT ) )
	{
		// skip 
		CWnd * pWndTestDowncast = CWnd::FromHandle( hWnd );
		ASSERT_VALID( pWndTestDowncast );
		if(	pWndTestDowncast->IsKindOf(RUNTIME_CLASS( CExtTabPageContainerWnd ))){
			continue;
		}

		ASSERT(
			(nCountOfDlgCtrlIDs == 0 && pDlgCtrlIDs == NULL)
			|| (nCountOfDlgCtrlIDs > 0 && pDlgCtrlIDs != NULL)
			);
		bool bSetupHook = true;
		if( nCountOfDlgCtrlIDs > 0 && pDlgCtrlIDs != NULL )
		{
			bSetupHook = false;
			UINT nDlgCtrlID = ::GetDlgCtrlID( hWnd );
			for( ULONG i=0; i<nCountOfDlgCtrlIDs; i++ )
			{
				if( pDlgCtrlIDs[i] == nDlgCtrlID )
				{
					bSetupHook = true;
					break;
				}
			} // for( ULONG i=0; i<nCountOfDlgCtrlIDs; i++ )
		} // if( nCountOfDlgCtrlIDs > 0 && pDlgCtrlIDs != NULL )

		if( bSetupHook )
		{
			if(	SetupHookWndSink( hWnd ) )
				nCountOfHooks++;
			else
			{
				ASSERT( FALSE );
			}
		} // if( bSetupHook )

		
		if( bDeep )
			nCountOfHooks +=
			SetupHookWndSinkToChilds(
			hWnd,
			pDlgCtrlIDs,
			nCountOfDlgCtrlIDs,
			bDeep
			);
	} // for( ; hWnd != NULL; hWnd = ::GetWindow( hWnd, GW_HWNDNEXT ) )
	return nCountOfHooks;
}

//////////////////////////////////////////////////////////////////////////

void CExtTabPageContainerWnd::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	int nIndex = PageSelectionGet();
	if( nIndex != -1 ){
		HWND hWnd = (HWND)m_pWndTab->ItemLParamGet( PageSelectionGet() );
		::SetFocus( hWnd );
	}
}


#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_FLAT_CTRL)

/////////////////////////////////////////////////////////////////////////////
// CExtTabPageContainerFlatWnd
/////////////////////////////////////////////////////////////////////////////

CExtTabPageContainerFlatWnd::CExtTabPageContainerFlatWnd()
{
}

CExtTabPageContainerFlatWnd::~CExtTabPageContainerFlatWnd()
{
}

IMPLEMENT_DYNCREATE(CExtTabPageContainerFlatWnd, CExtTabPageContainerWnd)

BEGIN_MESSAGE_MAP(CExtTabPageContainerFlatWnd, CExtTabPageContainerWnd)
	//{{AFX_MSG_MAP(CExtTabPageContainerFlatWnd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CExtTabPageContainerFlatWnd message handlers

BOOL CExtTabPageContainerFlatWnd::Create(
	CWnd * pParentWnd,
	const RECT & rcWnd,
	UINT nDlgCtrlID,
	DWORD dwWindowStyle,
	CCreateContext * pContext
	)
{
	BOOL bRet = CExtTabPageContainerWnd::Create(
		pParentWnd,
		rcWnd,
		nDlgCtrlID,
		dwWindowStyle,
		pContext);

	CenterTextSet();
	AutoHideScrollSet();

	return bRet;
}

#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_FLAT_CTRL)

#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
