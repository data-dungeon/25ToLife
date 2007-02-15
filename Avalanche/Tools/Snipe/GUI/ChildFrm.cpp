// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "Snipe.h"

#include "ChildFrm.h"
#include ".\childfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
   ON_COMMAND(ID_VIEW_FULLSCREEN, OnViewSplit)
   ON_WM_CREATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
	
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
#if 0
   BOOL rvalue = m_wndSplitter.CreateStatic(this,
		2, 2 );               // TODO: adjust the number of rows, columns
#else
	BOOL rvalue = m_wndSplitter.Create(this,
		2, 2,               // TODO: adjust the number of rows, columns
		CSize(10, 10),      // TODO: adjust the minimum pane size
		pContext);
#endif
	m_wndSplitter.SetScrollStyle( 0 );

//   OnViewSplit();

   return rvalue;
}

void CChildFrame::OnViewSplit()
{
	if (m_wndSplitter.GetColumnCount() > 1) {
		m_wndSplitter.DeleteColumn( 1 );
		m_wndSplitter.DeleteRow( 1 );
	}
	else {
		int width, height, min;
		m_wndSplitter.GetColumnInfo( 0, width, min );
		m_wndSplitter.SplitColumn( width / 2 );
		m_wndSplitter.GetRowInfo( 0, height, min );
		m_wndSplitter.SplitRow( height / 2 );

//      CSnipeView *pView = m_wndSplitter.GetPane( 0, 0 );
//      Vector rotate = pView->m_rotate;
//      pView->m_rotate.Set( 90, 0, 0 );
	}
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
      return -1;

   return 0;
}
