// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "TupViewer.h"

#include "MainFrm.h"
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND_EX(ID_VIEW_MENUBAR, OnBarCheck )
	ON_UPDATE_COMMAND_UI(ID_VIEW_MENUBAR, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_RESIZABLEBAR_FILE_TREE, OnBarCheck )
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESIZABLEBAR_FILE_TREE, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_RESIZABLEBAR_OUTPUT, OnBarCheck )
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESIZABLEBAR_OUTPUT, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_RESIZABLEBAR_PROPERTIES, OnBarCheck )
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESIZABLEBAR_PROPERTIES, OnUpdateControlBarMenu)
END_MESSAGE_MAP()




static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	::memset( &m_dataFrameWP, 0, sizeof(WINDOWPLACEMENT) );
	m_dataFrameWP.showCmd = SW_HIDE;
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
CWinApp * pApp = ::AfxGetApp();
	ASSERT( pApp != NULL );
	ASSERT( pApp->m_pszRegistryKey != NULL );
	ASSERT( pApp->m_pszRegistryKey[0] != _T('\0') );
	ASSERT( pApp->m_pszProfileName != NULL );
	ASSERT( pApp->m_pszProfileName[0] != _T('\0') );
	pApp;



	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;


	VERIFY(
		g_CmdManager->ProfileWndAdd(
			__PROF_UIS_PROJECT_CMD_PROFILE_NAME,
			GetSafeHwnd()
			)
		);
	VERIFY(
		g_CmdManager->UpdateFromMenu(
			__PROF_UIS_PROJECT_CMD_PROFILE_NAME,
			IDR_MAINFRAME
			)
		);
	VERIFY(
		g_CmdManager->UpdateFromMenu(
			__PROF_UIS_PROJECT_CMD_PROFILE_NAME,
			IDR_MAINFRAME
			)
		);
	VERIFY(
		g_CmdManager->UpdateFromMenu(
			__PROF_UIS_PROJECT_CMD_PROFILE_NAME,
			IDR_TUP_VIEWER_TYPE
			)
			//IDR_$$DOC$$TYPE
		);
	
 //PROJTYPE_MDI

	m_wndMenuBar.SetMdiWindowPopupName( _T("Window") );
 //PROJTYPE_MDI
	if(	!m_wndMenuBar.Create(
			NULL, // _T("Menu Bar"),
			this,
			ID_VIEW_MENUBAR,
			WS_CHILD|WS_VISIBLE
				|CBRS_TOP|CBRS_TOOLTIPS|CBRS_GRIPPER
				|CBRS_TOOLTIPS
				|CBRS_FLYBY
				|CBRS_SIZE_DYNAMIC
				|CBRS_HIDE_INPLACE
				)
		)
    {
        TRACE0("Failed to create menubar\n");
        return -1;      // failed to create
    }


	
	if(	!m_wndToolBar.Create(
			NULL, // _T("Main Toolbar"),
			this,
			AFX_IDW_TOOLBAR,
			WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_FLYBY | CBRS_SIZE_DYNAMIC
				| CBRS_TOOLTIPS
					| CBRS_HIDE_INPLACE
			) ||
		!m_wndToolBar.LoadToolBar( IDR_MAINFRAME )
		)
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndResizableBarFileTree.SetInitDesiredSizeVertical(
		CSize( 200, 400 )
		);
	m_wndResizableBarFileTree.SetInitDesiredSizeHorizontal(
		CSize( 400, 200 )
		);
	if(	!m_wndResizableBarFileTree.Create(
			NULL, // _T("Optional control bar caption"),
			this,
			ID_VIEW_RESIZABLEBAR_FILE_TREE,
			WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_GRIPPER
				|CBRS_TOOLTIPS
				|CBRS_FLYBY
				|CBRS_SIZE_DYNAMIC
				|CBRS_HIDE_INPLACE
			)
		)
	{
		TRACE0("Failed to create m_wndResizableBarFileTree\n");
		return -1;		// fail to create
	}
	if( !m_wndDockedCtrlFileTree.Create(
			WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL
			|TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT
			|TVS_INFOTIP|TVS_DISABLEDRAGDROP
			|TVS_SINGLEEXPAND|TVS_SHOWSELALWAYS
			,
			CRect(0,0,0,0),
			&m_wndResizableBarFileTree,
			m_wndResizableBarFileTree.GetDlgCtrlID()
			)
		)
	{
		TRACE0("Failed to create m_wndDockedCtrlOutput\n");
		return -1;		// fail to create
	}
	m_wndDockedCtrlFileTree.SetFont(
		CFont::FromHandle(
			(HFONT)::GetStockObject(DEFAULT_GUI_FONT)
			)
		);
	for( int nTreeIdx0=0; nTreeIdx0<10; nTreeIdx0++ )
	{
		CString sTreeItemText;
		sTreeItemText.Format( _T("Item 1-%d"), nTreeIdx0+1 );
		HTREEITEM htiAtLevel0 =
			m_wndDockedCtrlFileTree.InsertItem(
			LPCTSTR(sTreeItemText)
			);
		ASSERT( htiAtLevel0 != NULL );
		for( int nTreeIdx1=0; nTreeIdx1<5; nTreeIdx1++ )
		{
			sTreeItemText.Format( _T("Item 2-%d"), nTreeIdx1+1 );
			HTREEITEM htiAtLevel1 =
				m_wndDockedCtrlFileTree.InsertItem(
					LPCTSTR(sTreeItemText),
					htiAtLevel0
					);
			ASSERT( htiAtLevel1 != NULL );
			for( int nTreeIdx2=0; nTreeIdx2<3; nTreeIdx2++ )
			{
				sTreeItemText.Format( _T("Item 3-%d"), nTreeIdx2+1 );
				HTREEITEM htiAtLevel2 =
					m_wndDockedCtrlFileTree.InsertItem(
						LPCTSTR(sTreeItemText),
						htiAtLevel1
						);
				ASSERT( htiAtLevel2 != NULL );
				htiAtLevel2;
			} // for( int nTreeIdx2=0; nTreeIdx2<5; nTreeIdx2++ )
		} // for( int nTreeIdx1=0; nTreeIdx1<5; nTreeIdx1++ )
	} // for( int nTreeIdx0=0; nTreeIdx0<10; nTreeIdx0++ )
//__PROFUISAPPWIZ_KEY_RCB3

	m_wndResizableBarOutput.SetInitDesiredSizeVertical(
		CSize( 200, 400 )
		);
	m_wndResizableBarOutput.SetInitDesiredSizeHorizontal(
		CSize( 400, 200 )
		);
	if(	!m_wndResizableBarOutput.Create(
			NULL, // _T("Optional control bar caption"),
			this,
			ID_VIEW_RESIZABLEBAR_OUTPUT,
			WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_GRIPPER
				|CBRS_TOOLTIPS
				|CBRS_FLYBY
				|CBRS_SIZE_DYNAMIC
				|CBRS_HIDE_INPLACE
			)
		)
	{
		TRACE0("Failed to create m_wndResizableBarOutput\n");
		return -1;		// fail to create
	}
	if( !m_wndDockedCtrlOutput.Create(
			WS_CHILD|WS_VISIBLE
				|ES_MULTILINE|ES_LEFT
				|ES_NOHIDESEL|ES_WANTRETURN
				,
			CRect(0,0,0,0),
			&m_wndResizableBarOutput,
			m_wndResizableBarOutput.GetDlgCtrlID()
			)
		)
	{
		TRACE0("Failed to create m_wndDockedCtrlOutput\n");
		return -1;		// fail to create
	}
	m_wndDockedCtrlOutput.SetFont(
		CFont::FromHandle(
			(HFONT)::GetStockObject(DEFAULT_GUI_FONT)
			)
		);
	m_wndDockedCtrlOutput.SetWindowText(
		_T("    Edit control\r\n")
		_T("    inside resizable control bar")
		);

	m_wndResizableBarProperties.SetInitDesiredSizeVertical(
		CSize( 200, 400 )
		);
	m_wndResizableBarProperties.SetInitDesiredSizeHorizontal(
		CSize( 400, 200 )
		);
	if(	!m_wndResizableBarProperties.Create(
			NULL, // _T("Optional control bar caption"),
			this,
			ID_VIEW_RESIZABLEBAR_PROPERTIES,
			WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_GRIPPER
				|CBRS_TOOLTIPS
				|CBRS_FLYBY
				|CBRS_SIZE_DYNAMIC
				|CBRS_HIDE_INPLACE
			)
		)
	{
		TRACE0("Failed to create m_wndResizableBarProperties\n");
		return -1;		// fail to create
	}
	if( !m_wndDockedResizableProperties.Create(
			TupPropertyView::IDD,
			&m_wndResizableBarProperties
			)
		)
	{
		TRACE0("Failed to create m_wndResizableBarProperties\n");
		return -1;		// fail to create
	}

	if(	!m_wndTupTreeViewBar.Create(
		_T("Tupperware Tree"),
		this,
		ID_VIEW_TUP_TREE
		)
		|| !m_wndTupTreeViewChild.Create(
		TupTreeView::IDD,
		&m_wndTupTreeViewBar
		)
		)
	{
		TRACE0("Failed to create ID_VIEW_TUP_TREE\n");
		return -1;      // fail to create
	}


	// TODO: Delete these lines if you don't want the dockable entity
	m_wndMenuBar.EnableDocking( CBRS_ALIGN_ANY );
	m_wndToolBar.EnableDocking( CBRS_ALIGN_ANY );
	m_wndResizableBarFileTree.EnableDocking( CBRS_ALIGN_ANY );
	m_wndResizableBarOutput.EnableDocking( CBRS_ALIGN_ANY );
	m_wndResizableBarProperties.EnableDocking( CBRS_ALIGN_ANY );

	m_wndTupTreeViewBar.EnableDocking(CBRS_ALIGN_ANY);

	// Enable control bars in the frame window to be redocable
 //VERBOSE
	if( !CExtControlBar::FrameEnableDocking(this) )
	{
		ASSERT( FALSE );
		return -1;
	}

	// Enable autohide feature for resizable control bars
 //VERBOSE
	if( !CExtControlBar::FrameInjectAutoHideAreas(this) )
	{
		ASSERT( FALSE );
		return -1;
	}
	DockControlBar( &m_wndMenuBar );
	DockControlBar( &m_wndToolBar );
	RecalcLayout();

	m_wndResizableBarProperties.DockControlBar(AFX_IDW_DOCKBAR_LEFT, 1, this, false );
	m_wndResizableBarProperties.DockControlBar(&m_wndTupTreeViewBar,true,true,this,false);

	m_wndResizableBarOutput.DockControlBar(AFX_IDW_DOCKBAR_BOTTOM, 2, this, false );
	m_wndResizableBarFileTree.DockControlBar(AFX_IDW_DOCKBAR_RIGHT, 1, this, false );
	RecalcLayout();

static UINT statBasicCommands[] =
{
	ID_FILE_NEW, 
 //(PROJTYPE_MDI || !NODOCVIEW)
	ID_FILE_OPEN, 
	ID_FILE_SAVE,
 //!NODOCVIEW
	ID_APP_EXIT,
	ID_APP_ABOUT,
// PRINT
//	ID_FILE_PRINT,
//	ID_FILE_PRINT_SETUP, 
	ID_EDIT_COPY, 
	ID_EDIT_CUT, 
	ID_EDIT_PASTE, 
	ID_EDIT_UNDO, 
	ID_VIEW_TOOLBAR, 
 //TOOLBAR
	ID_VIEW_RESIZABLEBAR_FILE_TREE,
	ID_VIEW_RESIZABLEBAR_OUTPUT,
	ID_VIEW_RESIZABLEBAR_PROPERTIES,
	ID_WINDOW_TILE_HORZ,
 //(CRecordView || CDaoRecordView || COleDBRecordView)
 //HELP
	0 // end of commands list
}; // statBasicCommands array

	VERIFY(
		g_CmdManager->SetBasicCommands(
			__PROF_UIS_PROJECT_CMD_PROFILE_NAME,
			statBasicCommands
			)
		);




	CExtControlBar::ProfileBarStateLoad(
		this,
		pApp->m_pszRegistryKey,
		pApp->m_pszProfileName,
		pApp->m_pszProfileName,
		&m_dataFrameWP
		);

	g_CmdManager->SerializeState(
		__PROF_UIS_PROJECT_CMD_PROFILE_NAME,
		pApp->m_pszRegistryKey,
		pApp->m_pszProfileName,
		false
		);


	return 0;
}



BOOL CMainFrame::DestroyWindow() 
{
CWinApp * pApp = ::AfxGetApp();
	ASSERT( pApp != NULL );
	ASSERT( pApp->m_pszRegistryKey != NULL );
	ASSERT( pApp->m_pszRegistryKey[0] != _T('\0') );
	ASSERT( pApp->m_pszProfileName != NULL );
	ASSERT( pApp->m_pszProfileName[0] != _T('\0') );
	pApp;

	VERIFY(
		CExtControlBar::ProfileBarStateSave(
			this,
			pApp->m_pszRegistryKey,
			pApp->m_pszProfileName,
			pApp->m_pszProfileName,
			&m_dataFrameWP
			)
		);
	VERIFY(
		g_CmdManager->SerializeState(
			__PROF_UIS_PROJECT_CMD_PROFILE_NAME,
			pApp->m_pszRegistryKey,
			pApp->m_pszProfileName,
			true
			)
		);

	g_CmdManager->ProfileWndRemove( GetSafeHwnd() );
	
	return CMDIFrameWnd::DestroyWindow();
}

void CMainFrame::ActivateFrame(int nCmdShow) 
{
	if( m_dataFrameWP.showCmd != SW_HIDE )
	{
		SetWindowPlacement( &m_dataFrameWP );
		CMDIFrameWnd::ActivateFrame( m_dataFrameWP.showCmd );
		m_dataFrameWP.showCmd = SW_HIDE;
		return;
	}
	CMDIFrameWnd::ActivateFrame(nCmdShow);
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		| WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers



 //CONTAINER || CONTAINER_SERVER
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	if(		(! m_bStandardMDI ) // this complex condition disables MDI child frame menu
		&&	(	pMsg->message == WM_SYSKEYDOWN
		||	pMsg->message == WM_SYSKEYUP
		||	pMsg->message == WM_KEYDOWN
		||	pMsg->message == WM_KEYUP
		)
		&&	( HIWORD(pMsg->lParam) & KF_ALTDOWN ) != 0
		&&	(	((TCHAR)pMsg->wParam) == VK_SUBTRACT
		// disable warning C4310: cast truncates constant value
#pragma warning( push )
#pragma warning( disable : 4310 )
		||	((TCHAR)pMsg->wParam) == ((TCHAR)0xbd) // '-' near '0'
		// rollback warning C4310: cast truncates constant value
#pragma warning( pop )
		)
		)
		return TRUE; // (+ v.2.23)

	if( m_wndMenuBar.TranslateMainFrameMessage(pMsg) )
		return TRUE;

//	if( pMsg->message == WM_MOUSEWHEEL )
//	{
//		if( m_wndToolboxChild.PreTranslateMessage(pMsg) )
//			return TRUE;
//	}

	return CMDIFrameWnd::PreTranslateMessage(pMsg);
}




void CMainFrame::OnUpdateControlBarMenu(CCmdUI* pCmdUI)
{
	CExtControlBar::DoFrameBarCheckUpdate(
		this,
		pCmdUI,
		false
		);
}

BOOL CMainFrame::OnBarCheck(UINT nID)
{
	return
		CExtControlBar::DoFrameBarCheckCmd(
			this,
			nID,
			false
			);
}

void MdiTabSwitcher::OnTabWndSyncVisibility()
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;

	CMainFrame * pMainFrame =
		STATIC_DOWNCAST( CMainFrame, GetParent() );
	ASSERT_VALID( pMainFrame );
	if( !pMainFrame->m_bStandardMDI )
	{
		CExtTabMdiWnd::OnTabWndSyncVisibility();
		return;
	}

	DWORD dwWndStyle = GetStyle();
	if( (dwWndStyle & WS_VISIBLE) == 0 )
		return;

	::SetWindowPos(
		m_hWnd,
		NULL, 0, 0, 0, 0,
		SWP_NOSIZE|SWP_NOMOVE
		|SWP_NOZORDER|SWP_NOOWNERZORDER
		|SWP_NOREPOSITION
		|SWP_HIDEWINDOW
		);
}

void CMainFrame::RefreshAll(void)
{
	m_wndTupTreeViewChild.Refresh();
}

void CMainFrame::RefreshSelected(int windowFlags,void *pData)
{
	if (windowFlags & WINDOW_TUP_TREE_VIEW)
	{
		m_wndTupTreeViewChild.Refresh();
	}
	if (windowFlags & WINDOW_PROPERTY_VIEW)
	{
		m_wndDockedResizableProperties.Refresh();
	}
	if (windowFlags & WINDOW_TUP_TREE_VIEW_ITEM)
	{
		m_wndTupTreeViewChild.UpdateTreeItem((HTREEITEM)pData);
	}
}

