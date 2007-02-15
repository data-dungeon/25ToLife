// ControlView.cpp : implementation file
//

#include "stdafx.h"
#include "watch.h"
#include "ControlView.h"
#include "watchcomm.h"
#include "WatchSlider.h"
#include "WatchDoc.h"
#include "Watchview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControlView

IMPLEMENT_DYNCREATE(CControlView, CFormView)

CControlView::CControlView()
	: CFormView(CControlView::IDD)
{
	//{{AFX_DATA_INIT(CControlView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CControlView::~CControlView()
{
}

void CControlView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControlView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CControlView, CFormView)
	//{{AFX_MSG_MAP(CControlView)
	ON_WM_HSCROLL()
	ON_COMMAND(ID_CONNECT_PC, OnConnectPc)
	ON_COMMAND(ID_CONNECT_PLAYSTATION, OnConnectPlaystation)
	ON_COMMAND(ID_CONNECT_XBOX, OnConnectXbox)
	ON_COMMAND(ID_VIEW_DEBUGOUTPUT, OnViewDebugoutput)
	ON_COMMAND(ID_VIEW_PLOTDIALOG, OnViewPlotdialog)
	ON_COMMAND(ID_VIEW_STRIPCHART, OnViewStripchart)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DEBUGOUTPUT, OnUpdateViewDebugoutput)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PLOTDIALOG, OnUpdateViewPlotdialog)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STRIPCHART, OnUpdateViewStripchart)
	ON_COMMAND(ID_FILE_SAVEALL, OnFileSaveall)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_CONTEXT_REFRESH, OnContextRefresh)
	ON_LBN_SELCHANGE(1, OnSelchangeList)
	ON_BN_CLICKED(ID_ROW_NEW, OnRowNewBtn)
	ON_BN_CLICKED(ID_ROW_FILL, OnRowFillBtn)
	ON_BN_CLICKED(ID_COLUMN_NEW, OnColumnNewBtn)
	ON_BN_CLICKED(ID_COLUMN_FILL, OnColumnFillBtn)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlView diagnostics

#ifdef _DEBUG
void CControlView::AssertValid() const
{
	CFormView::AssertValid();
}

void CControlView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CControlView message handlers

void CControlView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);

	switch (nSBCode)
	{
		case SB_THUMBPOSITION:
			TRACE("Thumbposition\n");
			break;
		case SB_ENDSCROLL:
			TRACE("ENdscroll\n");
			break;
		case SB_THUMBTRACK:
			TRACE("ThumbTrack\n");
			break;
		default:
			TRACE("%d\n", nSBCode);
	}
	if (nSBCode != SB_THUMBPOSITION && nSBCode != SB_ENDSCROLL)
		// Update the watch variable
		((CWatchSlider*)pScrollBar)->UpdateVar();

	if (nSBCode == SB_ENDSCROLL)
	{
		((CWatchSlider*)pScrollBar)->UpdateSlider();
	}
}

void CControlView::OnConnectPc()
{
	((CWatchView*)GetParent())->OnConnectPc();
}

void CControlView::OnConnectPlaystation()
{
	((CWatchView*)GetParent())->OnConnectPlaystation();
}

void CControlView::OnConnectXbox()
{
	((CWatchView*)GetParent())->OnConnectXbox();
}

void CControlView::OnViewDebugoutput()
{
	((CWatchView*)GetParent())->OnViewDebugoutput();
}

void CControlView::OnViewPlotdialog()
{
	((CWatchView*)GetParent())->OnViewPlotdialog();
}

void CControlView::OnViewStripchart()
{
	((CWatchView*)GetParent())->OnViewStripchart();
}

void CControlView::OnUpdateViewDebugoutput(CCmdUI* pCmdUI)
{
	((CWatchView*)GetParent())->OnUpdateViewDebugoutput(pCmdUI);
}

void CControlView::OnUpdateViewPlotdialog(CCmdUI* pCmdUI)
{
	((CWatchView*)GetParent())->OnUpdateViewPlotdialog(pCmdUI);
}

void CControlView::OnUpdateViewStripchart(CCmdUI* pCmdUI)
{
	((CWatchView*)GetParent())->OnUpdateViewStripchart(pCmdUI);
}

void CControlView::OnFileSaveall()
{
	((CWatchView*)GetParent())->OnFileSaveall();
}

void CControlView::OnFileSave()
{
	((CWatchView*)GetParent())->OnFileSave();
}

void CControlView::OnFileOpen()
{
	((CWatchView*)GetParent())->OnFileOpen();
}

void CControlView::OnContextRefresh()
{
	((CWatchView*)GetParent())->OnContextRefresh();
}

void CControlView::OnSelchangeList()
{
	((CWatchView*)GetParent())->OnSelchangeList();
}

void CControlView::OnRowNewBtn()
{
	((CWatchView*)GetParent())->OnListButton(ID_ROW_NEW);
}

void CControlView::OnRowFillBtn()
{
	((CWatchView*)GetParent())->OnListButton(ID_ROW_FILL);
}

void CControlView::OnColumnNewBtn()
{
	((CWatchView*)GetParent())->OnListButton(ID_COLUMN_NEW);
}

void CControlView::OnColumnFillBtn()
{
	((CWatchView*)GetParent())->OnListButton(ID_COLUMN_FILL);
}


HBRUSH CControlView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor == CTLCOLOR_EDIT)
	{
		pDC->SetBkColor(::GetSysColor(COLOR_3DFACE));
		return ::GetSysColorBrush(COLOR_3DFACE);
	}
	else
		return CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
}
