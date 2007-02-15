// CObjectExplorerPane.cpp : implementation file
//

#include "stdafx.h"
#include "Snipe.h"
#include "ObjectExplorerPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ObjectExplorerPane

CObjectExplorerPane::CObjectExplorerPane()
{
}

CObjectExplorerPane::~CObjectExplorerPane()
{
}


BEGIN_MESSAGE_MAP(CObjectExplorerPane, CWnd)
	//{{AFX_MSG_MAP(CObjectExplorerPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
//	ON_COMMAND_RANGE(ID_SOLUTION_PROPERTIES, ID_SOLUTION_ADDNEWFOLDER, OnEmptyCommand)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CObjectExplorerPane message handlers

int CObjectExplorerPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	VERIFY(m_wndToolBar.CreateToolBar(WS_TABSTOP|WS_VISIBLE|WS_CHILD|CBRS_TOOLTIPS, this));	
	VERIFY(m_wndToolBar.LoadToolBar(IDR_PANE_SOLUTIONEXPLORER));

	
	if (m_wndObjectExplorerView.GetSafeHwnd() == 0)
	{
		if (!m_wndObjectExplorerView.Create( WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,
			CRect(0,0,0,0), this, 0xFF ))
		{
			TRACE0( "Unable to create tree control.\n" );
			return 0;
		}
		m_wndObjectExplorerView.ModifyStyleEx(0, WS_EX_STATICEDGE);

		// load the tree images bitmap and add it to the image list.
		if (!CreateImageList(m_ilSolutionView, IDB_FILETREE))
			return 0;

		// Set the image list for the tree control.
		m_wndObjectExplorerView.SetImageList( &m_ilSolutionView, TVSIL_NORMAL );

		// add the parent item, make it bold
		HTREEITEM htiParent = m_wndObjectExplorerView.InsertItem(_T("Models"));
		HTREEITEM htiChild; // child item

		htiChild = m_wndObjectExplorerView.InsertItem (_T("Tak 2"), 1, 1, htiParent);
		m_wndObjectExplorerView.SetItemState (htiChild, TVIS_BOLD, TVIS_BOLD);

		// add the children of the parent item
		HTREEITEM hti = m_wndObjectExplorerView.InsertItem(_T("Bones"), 2, 3, htiChild);
		m_wndObjectExplorerView.InsertItem(_T("Bone1.cpp"), 4, 4, hti);
		m_wndObjectExplorerView.InsertItem(_T("Bone2.cpp"), 4, 4, hti);
		m_wndObjectExplorerView.InsertItem(_T("Bone3.cpp"), 4, 4, hti);
		m_wndObjectExplorerView.Expand(hti, TVE_EXPAND);
		m_wndObjectExplorerView.Expand(htiParent, TVE_EXPAND);
		m_wndObjectExplorerView.Expand(htiChild, TVE_EXPAND);
	}	
	return 0;
}

void CObjectExplorerPane::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	int nTop = 0;

	if (m_wndToolBar.GetSafeHwnd())
	{		
		CSize sz = m_wndToolBar.CalcDockingLayout(cx, /*LM_HIDEWRAP|*/ LM_HORZDOCK|LM_HORZ | LM_COMMIT);

		m_wndToolBar.MoveWindow(0, nTop, cx, sz.cy);
		m_wndToolBar.Invalidate(FALSE);
		nTop += sz.cy;
	}

	if (m_wndObjectExplorerView.GetSafeHwnd())
	{	
		m_wndObjectExplorerView.MoveWindow(0, nTop, cx, cy - nTop);
		m_wndObjectExplorerView.Invalidate(FALSE);
	}	
}

void CObjectExplorerPane::OnEmptyCommand(UINT)
{

}