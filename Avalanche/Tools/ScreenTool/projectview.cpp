#include "pch.h"
#include "application.h"
#include "document.h"
#include "projectview.h"
#include "resource.h"

BEGIN_MESSAGE_MAP( ProjectTree, CTreeCtrl )
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
END_MESSAGE_MAP()

#define TIMEREVENT_PROJECTSYNC			(0x153)

ProjectTree::ProjectTree( void )
{
	m_AltKey = FALSE;
	m_ControlKey = FALSE;
	m_ShiftKey = FALSE;
}

void ProjectTree::OnAltKey( BOOL KeyDown )
{
	m_AltKey = KeyDown;
}

void ProjectTree::OnControlKey( BOOL KeyDown )
{
	m_ControlKey = KeyDown;
}

void ProjectTree::OnShiftKey( BOOL KeyDown )
{
	m_ShiftKey = KeyDown;
}

void ProjectTree::OnEnterKey( BOOL KeyDown )
{
	if( KeyDown )
	{
	//	GameView* pGameView = GetGameView();
	//	Sprite* pSprite = (Sprite*) GetItemData( GetSelectedItem() );

	//	if( pGameView )
	//		pGameView->GuiShowProperties( pSprite );
	}

	return;
}

void ProjectTree::CheckSpecialKeys( UINT Flags )
{
	if( Flags & MK_CONTROL )
	{
		if( !m_ControlKey )
			OnControlKey( TRUE );
	}
	else if( m_ControlKey )
	{
		OnControlKey( FALSE );
	}

	if( Flags & MK_SHIFT )
	{
		if( !m_ShiftKey )
			OnShiftKey( TRUE );
	}
	else if( m_ShiftKey )
	{
		OnShiftKey( FALSE );
	}

	return;
}

void ProjectTree::OnMouseMove( UINT Flags, CPoint p )
{
	CheckSpecialKeys( Flags );

	CTreeCtrl::OnMouseMove( Flags, p );
}

void ProjectTree::OnLButtonDown( UINT Flags, CPoint p )
{
	CheckSpecialKeys( Flags );

	CTreeCtrl::OnLButtonDown( Flags, p );
}

void ProjectTree::OnRButtonDown( UINT Flags, CPoint p )
{
	CheckSpecialKeys( Flags );

	SendMessage( WM_LBUTTONDOWN, (WPARAM) Flags, MAKELPARAM(p.x, p.y) );
	SendMessage( WM_CONTEXTMENU, (WPARAM) m_hWnd, MAKELPARAM(p.x, p.y) );
}

void ProjectTree::OnKeyDown( UINT Char, UINT Repeat, UINT Flags )
{
	switch( Char )
	{
	case VK_MENU:
		OnAltKey( TRUE );
		break;
	case VK_CONTROL:
		OnControlKey( TRUE );
		break;
	case VK_SHIFT:
		OnShiftKey( TRUE );
		break;
	case VK_RETURN:
		OnEnterKey( TRUE );
	}

	CTreeCtrl::OnKeyDown( Char, Repeat, Flags );
}

void ProjectTree::OnKeyUp( UINT Char, UINT Repeat, UINT Flags )
{
	switch( Char )
	{
	case VK_MENU:
		OnAltKey( FALSE );
		break;
	case VK_CONTROL:
		OnControlKey( FALSE );
		break;
	case VK_SHIFT:
		OnShiftKey( FALSE );
		break;
	case VK_RETURN:
		OnEnterKey( FALSE );
		break;
	}

	CTreeCtrl::OnKeyUp( Char, Repeat, Flags );
}

void ProjectTree::OnSysKeyDown( UINT Char, UINT Repeat, UINT Flags )
{
	switch( Char )
	{
	case VK_MENU:
		OnAltKey( TRUE );
		break;
	case VK_CONTROL:
		OnControlKey( TRUE );
		break;
	case VK_SHIFT:
		OnShiftKey( TRUE );
		break;
	}

	CTreeCtrl::OnSysKeyDown( Char, Repeat, Flags );
}

void ProjectTree::OnSysKeyUp( UINT Char, UINT Repeat, UINT Flags )
{
	switch( Char )
	{
	case VK_MENU:
		OnAltKey( FALSE );
		break;
	case VK_CONTROL:
		OnControlKey( FALSE );
		break;
	case VK_SHIFT:
		OnShiftKey( FALSE );
		break;
	}

	CTreeCtrl::OnSysKeyUp( Char, Repeat, Flags );
}

IMPLEMENT_DYNCREATE( ProjectView, CView );

BEGIN_MESSAGE_MAP( ProjectView, CView )
	ON_WM_CREATE()
	ON_WM_ACTIVATE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_EX_RANGE( TVN_SELCHANGED, 0, 0x7fffffff, OnSelChanged )
	ON_WM_TIMER()
END_MESSAGE_MAP()

ProjectView *ProjectView::m_pProjectViews = 0;

ProjectView::ProjectView( void )
{
	m_ToolBarSize = 4;

	if( !m_pProjectViews )
		m_pProjectViews = this;
	else
	{
		ProjectView *pProjectView = m_pProjectViews;

		while( pProjectView->m_pNextProjectView )
			pProjectView = pProjectView->m_pNextProjectView;

		pProjectView->m_pNextProjectView = this;
	}

	m_pNextProjectView = 0;
	m_AllowSelChange = TRUE;
}

ProjectView::~ProjectView()
{
	if( m_pProjectViews == this )
		m_pProjectViews = m_pNextProjectView;
	else
	{
		ProjectView *pProjectView = m_pProjectViews;

		while( pProjectView->m_pNextProjectView != this )
			pProjectView = pProjectView->m_pNextProjectView;

		pProjectView->m_pNextProjectView = m_pNextProjectView;
	}

	return;
}

void ProjectView::SyncProjectViews( CDocument *pDocument )
{
	ProjectView *pProjectView = m_pProjectViews;

	while( pProjectView )
	{
		if( pProjectView->GetDocument() == pDocument )
			pProjectView->SyncToProject();

		pProjectView = pProjectView->m_pNextProjectView;
	}

	return;
}

ProjectView* ProjectView::GetFirst( void )
{
	return m_pProjectViews;
}

ProjectView* ProjectView::GetNext( ProjectView* pProjectView )
{
	if( pProjectView )
		return pProjectView->m_pNextProjectView;

	return 0;
}


RECT ProjectView::GetToolBarRect( void )
{
	RECT rc;
	GetClientRect( &rc );

	rc.right = rc.left + m_ToolBarSize;

	return rc;
}

RECT ProjectView::GetProjectRect( void )
{
	RECT rcToolBar = GetToolBarRect();

	RECT rc;
	GetClientRect( &rc );

	rc.left = rcToolBar.right;

	return rc;
}

void ProjectView::SyncToProject( void )
{
	KillTimer( TIMEREVENT_PROJECTSYNC );
	UINT_PTR Result = SetTimer( TIMEREVENT_PROJECTSYNC, 100, 0 );

	if( Result != TIMEREVENT_PROJECTSYNC )
		SyncToProjectEvent();

	return;
}

void ProjectView::SyncToProjectEvent( void )
{
	KillTimer( TIMEREVENT_PROJECTSYNC );

	m_ProjectTree.SetImageList( &App.m_ImageList, TVSIL_NORMAL );

	SevenupDoc* pDocument = (SevenupDoc*) GetDocument();

	if( pDocument )
	{
		Sprite* pSprite = pDocument->GetSceneList();

		if( pSprite )
		{
			SyncChildren( TVI_ROOT, pSprite );
			pSprite = pSprite->Child();
		}

		while( pSprite )
		{
			SyncSceneList( TVI_ROOT, pSprite );

			pSprite = pSprite->Next();
		}
	}

	return;
}

void ProjectView::ResizeProjectView( void )
{
	RECT rc = GetProjectRect();
	m_ProjectTree.MoveWindow( &rc, FALSE );
}

void ProjectView::ResizeToolBar( void )
{
	RECT rc = GetToolBarRect();
	m_ToolBar.MoveWindow( &rc, FALSE );
}

GameView* ProjectView::GetGameView( void )
{
	SevenupDoc* pDocument = (SevenupDoc*) GetDocument();

	if( !pDocument )
		return 0;

	SevenupView* pView = pDocument->m_pPrimaryView;

	if( !pView )
		return 0;

	return pView->m_pGameView;
}

Sprite* ProjectView::GetItemData( HTREEITEM hTreeItem )
{
	return (Sprite*) m_ProjectTree.GetItemData( hTreeItem );
}

HTREEITEM ProjectView::FindItem( HTREEITEM hTreeItem, Sprite* pSprite )
{
	if( pSprite )
	{
		CTreeCtrl &c = m_ProjectTree;
		HTREEITEM hNextItem;
		Sprite* pParent;

		if( hTreeItem != TVI_ROOT )
		{
			if( pSprite == GetItemData( hTreeItem ) )
				return hTreeItem;
		}

		hNextItem = c.GetChildItem( hTreeItem );

		while( hNextItem )
		{
			pParent = pSprite;
	
			while( pParent )
			{
				if( pParent == GetItemData( hNextItem ) )
					return FindItem( hNextItem, pSprite );

				pParent = pParent->Parent();
			}

			hNextItem = c.GetNextItem( hNextItem, TVGN_NEXT );
		}
	}

	return 0;
}

void ProjectView::RemoveSprite( Sprite* pSprite )
{
	m_AllowSelChange = FALSE;

	if( pSprite )
	{
		HTREEITEM hTreeItem = FindItem( TVI_ROOT, pSprite );

		if( hTreeItem )
			m_ProjectTree.DeleteItem( hTreeItem );
	}

	m_AllowSelChange = TRUE;
}

void ProjectView::SyncChildren( HTREEITEM hTreeItem, Sprite* pSprite )
{
	if( pSprite && hTreeItem )
	{
		CTreeCtrl &c = m_ProjectTree;

		HTREEITEM hChildItem = c.GetChildItem( hTreeItem );
		Sprite* pNextChild;
		Sprite* pChildItem;

		while( hChildItem )
		{
			pChildItem = GetItemData( hChildItem );

			pNextChild = pSprite->Child();

			while( pNextChild )
			{
				if( pNextChild == pChildItem )
					break;

				pNextChild = pNextChild->Next();
			}

			if( pNextChild == pChildItem )
			{
				SyncChildren( hChildItem, pChildItem );
				hChildItem = c.GetNextItem( hChildItem, TVGN_NEXT );
			}
			else
			{
				c.DeleteItem( hChildItem );
				hChildItem = c.GetChildItem( hTreeItem );
			}
		}
	}

	return;
}

void ProjectView::SyncSceneList( HTREEITEM hRoot, Sprite *pSprite )
{
	ProjectItem* pProjectItem = Project.GetProjectItem( pSprite );

	if( pSprite && pProjectItem )
	{
		CTreeCtrl &c = m_ProjectTree;

		HTREEITEM hTreeItem = FindItem( hRoot, pSprite );

		LPCTSTR pItemName = pProjectItem->m_Name.get();

		if( hTreeItem )
			c.SetItemText( hTreeItem, pItemName );
		else
		{
			hTreeItem = c.InsertItem( pItemName, hRoot );

			if( hTreeItem )
			{
				int BaseImage = 2;

				if( tstricmp( pSprite->Classname(), "Text" ) )
					BaseImage = 4;

				if( pSprite->GetInterface( "Screen" ) )
					BaseImage = 0;

				m_ProjectTree.SetItemImage( hTreeItem, BaseImage, BaseImage+1 );
				m_ProjectTree.SetItemData( hTreeItem, (DWORD_PTR) pSprite );
				m_ProjectTree.SetCheck( hTreeItem, TRUE );
			}
		}


		pSprite = pSprite->Child();

		while( pSprite )
		{
			SyncSceneList( hTreeItem, pSprite );
			pSprite = pSprite->Next();
		}
	}

	return;
}

void ProjectView::OnDraw( CDC *pdc )
{
	SevenupDoc* pDocument = (SevenupDoc*) GetDocument();

	if( pDocument )
	{
		Sprite* pSprite = pDocument->GetSelectedSprite();

		if( pSprite )
		{
			HTREEITEM hTreeItem = FindItem( TVI_ROOT, pSprite );

			m_AllowSelChange = FALSE;

			if( hTreeItem )
				m_ProjectTree.SelectItem( hTreeItem );

			m_AllowSelChange = TRUE;
		}
	}

	return;
}

void ProjectView::OnActivate( UINT State, CWnd *pOtherWnd, BOOL Minimized )
{
	CView::OnActivate( State, pOtherWnd, Minimized );
}

int ProjectView::OnMouseActivate( CWnd* pDesktopWnd, UINT HitTest, UINT message )
{
	return MA_NOACTIVATE;
}

int ProjectView::OnCreate(	LPCREATESTRUCT pcs )
{
	if( -1 == CView::OnCreate( pcs ) )
		return -1;

	RECT rc = GetProjectRect();

	DWORD dwStyle = WS_VISIBLE|WS_CHILD|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_DISABLEDRAGDROP|TVS_CHECKBOXES;

	if( m_ProjectTree.CreateEx( 0, dwStyle, rc, this, 0 ) )
	{
	}

	rc = GetToolBarRect();

	if( m_ToolBar.CreateEx( 0, 0, "ProjectBar", WS_VISIBLE|WS_CHILD, rc, this, 0 ) )
	{
	}

	return 0;
}

void ProjectView::OnSize( UINT Type, int x, int y )
{
	ResizeProjectView();
	ResizeToolBar();
}

void ProjectView::OnContextMenu( CWnd* pWnd, CPoint p )
{
	CWnd* pMainWnd = AfxGetMainWnd();
	GameView* pGameView = GetGameView();

	if( pMainWnd && pGameView )
	{
		CTreeCtrl &c = m_ProjectTree;

		HTREEITEM hCurSel = c.GetSelectedItem();

		if( hCurSel )
		{
			Sprite* pSprite = GetItemData( c.GetSelectedItem() );

			if( pSprite )
			{
				pGameView->SelectSprite( pSprite );
	
				HMENU hTopMenu = ::GetMenu( pMainWnd->m_hWnd );
				HMENU hSubMenu = ::GetSubMenu( hTopMenu, 3 );

				ClientToScreen( &p );

				HWND hWnd = 0;

				if( pMainWnd )
					hWnd = pMainWnd->m_hWnd;

				TrackPopupMenuEx( hSubMenu, TPM_LEFTALIGN|TPM_TOPALIGN, p.x, p.y, hWnd, 0 );
			}
		}
	}

	return;
}

BOOL ProjectView::OnSelChanged( UINT Id, NMHDR *pNotify, LRESULT *pResult )
{
	CTreeCtrl &c = m_ProjectTree;

	if( pNotify->hwndFrom != c.m_hWnd )
		return FALSE;

	GameView* pGameView = GetGameView();
	Sprite* pSprite = GetItemData( c.GetSelectedItem() );

	if( m_AllowSelChange && pGameView && pSprite )
	{	
		if( !m_ProjectTree.m_ControlKey )
		{
			if( pGameView->IsSelected( pSprite ) )
				pGameView->ReselectSprite( pSprite );
			else
			{
				pGameView->DeselectAll();
				pGameView->SelectSprite( pSprite );
			}
		}
		else
		{
			if( pGameView->IsSelected( pSprite ) )
				pGameView->DeselectSprite( pSprite );
			else
				pGameView->SelectSprite( pSprite );
		}

		pGameView->Invalidate();
	}

	return TRUE;
}

void ProjectView::OnTimer( UINT_PTR TimerEvent )
{
	switch( TimerEvent )
	{
	case TIMEREVENT_PROJECTSYNC:
		SyncToProjectEvent();
		break;
	}

	return;
}