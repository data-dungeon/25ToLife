// TupTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "TupViewer.h"
#include "TupTreeView.h"
#include "MainFrm.h"

#if (!defined __WND_THIN_FRAME_H)
#include "WndThinFrame.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TupTreeView dialog


TupTreeView::TupTreeView(CWnd* pParent /*=NULL*/)
: CExtResizableDialog(TupTreeView::IDD, pParent)
{
	//{{AFX_DATA_INIT(TupTreeView)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void TupTreeView::DoDataExchange(CDataExchange* pDX)
{
	CExtResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TupTreeView)
	DDX_Control(pDX, IDC_TREEVIEW_TREE, m_wndTree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TupTreeView, CExtResizableDialog)
	//{{AFX_MSG_MAP(TupTreeView)
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREEVIEW_TREE, OnTvnSelchangedTreeviewTree)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TupTreeView message handlers

BOOL TupTreeView::OnInitDialog() 
{
	if( !CExtResizableDialog::OnInitDialog() )
	{
		ASSERT( FALSE );
		return FALSE;
	}

	ShowSizeGrip( FALSE );

	CProfStudioThinFrame * pWndThinFrame = new CProfStudioThinFrame;
	if( ! pWndThinFrame->
		CreateDynamicThinFrame( &m_wndTree )
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}

	RepositionBars(0,0xFFFF,IDC_TREEVIEW_TREE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void TupTreeView::OnSize(UINT nType, int cx, int cy) 
{
	CExtResizableDialog::OnSize(nType, cx, cy);

	RepositionBars(0,0xFFFF,IDC_TREEVIEW_TREE);
}

void TupTreeView::OnSetFocus(CWnd* pOldWnd) 
{
	CExtResizableDialog::OnSetFocus(pOldWnd);

	m_wndTree.SetFocus();
}

void TupTreeView::Refresh(void)
{
	m_wndTree.Refresh();
}

void TupTreeView::OnTvnSelchangedTreeviewTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	HTREEITEM hItem2 = m_wndTree.GetSelectedItem();
	TupperwareAtom *pAtom = (TupperwareAtom *) m_wndTree.GetItemData(hItem);
	if (theApp.m_pSelectedAtom!=pAtom)
	{
		theApp.m_pSelectedAtom = pAtom;
		theApp.RefreshSelected(CMainFrame::WINDOW_PROPERTY_VIEW,NULL);
	}

	*pResult = 0;
}

void TupTreeView::UpdateTreeItem(HTREEITEM hItem)
{
	m_wndTree.UpdateTreeItem(hItem);
}
