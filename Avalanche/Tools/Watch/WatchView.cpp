// WatchView.cpp : implementation of the CWatchView class
//

#include "stdafx.h"
#include "Watch.h"

#include "WatchDoc.h"
#include "WatchView.h"
#include "WatchComm.h"
#include "SaveContext.h"

#include "ControlView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WIDTH	640
#define HEIGHT	224*2

/////////////////////////////////////////////////////////////////////////////
// CWatchView

IMPLEMENT_DYNCREATE(CWatchView, CFormView)

BEGIN_MESSAGE_MAP(CWatchView, CFormView)
	//{{AFX_MSG_MAP(CWatchView)
	ON_WM_HSCROLL()
	ON_WM_TIMER()
	ON_COMMAND(ID_CONNECT_PC, OnConnectPc)
	ON_COMMAND(ID_CONNECT_PLAYSTATION, OnConnectPlaystation)
	ON_COMMAND(ID_CONNECT_XBOX, OnConnectXbox)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB, OnSelchangingTab)
	ON_COMMAND(ID_VIEW_DEBUGOUTPUT, (AFX_PMSG)OnViewDebugoutput)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DEBUGOUTPUT, OnUpdateViewDebugoutput)
	ON_COMMAND(ID_VIEW_STRIPCHART, (AFX_PMSG)OnViewStripchart)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STRIPCHART, OnUpdateViewStripchart)
	ON_COMMAND(ID_VIEW_PLOTDIALOG, (AFX_PMSG)OnViewPlotdialog)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PLOTDIALOG, OnUpdateViewPlotdialog)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVEALL, OnFileSaveall)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_CONTEXT_REFRESH, OnContextRefresh)
	ON_LBN_SELCHANGE(1, OnSelchangeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWatchView construction/destruction

CWatchView::CWatchView()
	: CFormView(CWatchView::IDD)
{
	//{{AFX_DATA_INIT(CWatchView)
	//}}AFX_DATA_INIT
	// TODO: add construction code here

	mouseButton = 0;

	controlView = new CControlView;
}

CWatchView::~CWatchView()
{
	for (int i = 0; i < contexts.GetSize(); i++)
		delete contexts[i];
	delete printDlg;
	delete stripDlg;
	delete plotDlg;
}

void CWatchView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWatchView)
	DDX_Control(pDX, IDC_TAB, m_tab);
	DDX_Control(pDX, IDC_SLIDER1, m_slider1);
	//}}AFX_DATA_MAP
}

BOOL CWatchView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CWatchView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit(FALSE);

	//---------------------------------------------
	// Create the view that does the clipping.
	//---------------------------------------------
	RECT screen;
	GetDlgItem(IDC_TAB)->GetWindowRect(&screen);
	ScreenToClient(&screen);
	screen.top += 23;
	screen.bottom -= 3;
	screen.right -= 3;
	screen.left += 3;
	controlView->Create(AfxRegisterWndClass(CS_DBLCLKS,0,GetSysColorBrush(COLOR_BTNFACE),0),
							  "ControlView", WS_VISIBLE | WS_CHILD | WS_VSCROLL, // | WS_BORDER,
							  screen, this, IDD_CONTROL_PAGE);

	VAR_INFO vi;
	strcpy(vi.name, "Rash Counter");
	vi.value.ival = 50;
	vi.type = VARTYPE_INT;
	vi.min = 1;
	vi.max = 100;
	vi.address = 0;
	::ShowWindow(m_slider1.m_hWnd, SW_HIDE);
//	m_slider1.Init(controlView, GetContext("Testing"), &staticFont, vi);
	m_slider1.SetParent(controlView);
//	sliders.Add(&m_slider1);

	RECT wnd;
	controlView->GetWindowRect(&wnd);
	ScreenToClient(&wnd);
	m_slider1.GetWindowRect(&sliderRect);
	controlView->ScreenToClient(&sliderRect);
	sliderRect.top    -= wnd.top;
	sliderRect.bottom -= wnd.top;
	sliderRect.right  -= wnd.left;
	sliderRect.left   -= wnd.left;

	OnSelchangeTab(0,0);

	//---------------------------------------------
	//  Allocate the dialogs
	//---------------------------------------------
	printDlg = new CPrintDlg(this);
	stripDlg = new CStripDlg(this);
	plotDlg = new CPlotDlg(this);

	//---------------------------------------------
	//  Bring up the dialogs if they were up
	//  when the app was last closed.
	//---------------------------------------------
	CString str;
	str.Format("Dialogs\\%d", CStripDlg::IDD);
	if (AfxGetApp()->GetProfileInt(str, "Visible", 0))
		stripDlg->ShowWindow(SW_SHOW);
	else
		stripDlg->ShowWindow(SW_HIDE);

	str.Format("Dialogs\\%d", CPrintDlg::IDD);
	if (AfxGetApp()->GetProfileInt(str, "Visible", 0))
		printDlg->ShowWindow(SW_SHOW);
	else
		printDlg->ShowWindow(SW_HIDE);

	str.Format("Dialogs\\%d", CPlotDlg::IDD);
	if (AfxGetApp()->GetProfileInt(str, "Visible", 0))
		plotDlg->ShowWindow(SW_SHOW);
	else
		plotDlg->ShowWindow(SW_HIDE);

	SetTimer(2, 100, NULL);

	WatchComm::Instance()->SetView(this);
	if (0 == strcmp(remoteMachine, "PC"))
		OnConnectPc();

}

/////////////////////////////////////////////////////////////////////////////
// CWatchView diagnostics

#ifdef _DEBUG
void CWatchView::AssertValid() const
{
	CFormView::AssertValid();
}

void CWatchView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CWatchDoc* CWatchView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWatchDoc)));
	return (CWatchDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWatchView message handlers

void CWatchView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);

	CWatchSlider* slider = (CWatchSlider*)pScrollBar;
	if (slider == &m_slider1)
	{
		char str[256];
		sprintf(str, "%d", slider->GetPos());
		WatchComm::Instance()->SendPacket(slider->GetPos());
	}
	else
	{
		WatchVar* v = slider->Variable();
		WatchComm::Instance()->ChangeVariable(v->GetPtr(), v->GetVal(), v->Event(), v->EventDest(), v->Type());
	}
	slider->UpdateVar();
}

void CWatchView::OnTimer(UINT nIDEvent)
{
	WatchComm::Instance()->Idle();

	CFormView::OnTimer(nIDEvent);
}

void CWatchView::OnConnectPc()
{
	if (WatchComm::Instance()->ConnectPC())
		KillTimer(1);
}

void CWatchView::OnConnectPlaystation()
{
	if (WatchComm::Instance()->ConnectPSX2())
		KillTimer(1);
}

void CWatchView::OnConnectXbox()
{
	if (WatchComm::Instance()->ConnectXbox())
		KillTimer(1);
}

//----------------------------------------------------------------------
// CWatchView::DebugPrint
//    Print debug messages to the debug print edit box.
//----------------------------------------------------------------------
void CWatchView::DebugPrint(char *format, ...)
{
	va_list args;
	va_start(args, format);

	printDlg->DebugPrint(format, args);
	//	vsprintf(tmp, format, args);
	// call to the dialog's debug print here.
	va_end(args);
}

//----------------------------------------------------------------------
// CWatchView::RegisterVariable
//    Register a variable with the associated context.
//----------------------------------------------------------------------
void CWatchView::RegisterVariable(VAR_INFO& vi)
{
	if (0 == contexts[vi.context])
	{
		DebugPrint("Trying to register variable before the context was registered.\n");
		return;
	}
	contexts[vi.context]->RegisterVariable(vi);
	OnSelchangeTab(0,0);
}

//----------------------------------------------------------------------
// CWatchView::RegisterLabel
//    Register a label for a table.
//----------------------------------------------------------------------
void CWatchView::RegisterLabel(LABEL_INFO& info)
{
	if (0 == contexts[info.context])
	{
		DebugPrint("Trying to register variable before the context was registered.\n");
		return;
	}
	contexts[info.context]->RegisterLabel(info);
	OnSelchangeTab(0,0);
}

//----------------------------------------------------------------------
// CWatchView::RegisterTableVariable
//    Register a label for a table.
//----------------------------------------------------------------------
void CWatchView::RegisterTableVariable(VAR_INFO& info)
{
	if (0 == contexts[info.context])
	{
		DebugPrint("Trying to register table variable before the context was registered.\n");
		return;
	}
	contexts[info.context]->RegisterTableVariable(info);
	OnSelchangeTab(0,0);
}

//----------------------------------------------------------------------
// CWatchView::RegisterContext
//    Register a context.
//----------------------------------------------------------------------
void CWatchView::RegisterContext(CONTEXT_INFO& contextInfo)
{
	TCITEM tcitem;
	tcitem.mask = TCIF_TEXT;
	tcitem.pszText = contextInfo.name;
	tcitem.cchTextMax = strlen(contextInfo.name);
	m_tab.InsertItem(contextInfo.context, &tcitem);
	m_tab.Invalidate();
	contexts.InsertAt(contextInfo.context, new Context((CControlView*)controlView,
																		contextInfo.context,
																		contextInfo.name,
																		sliderRect));
}

//----------------------------------------------------------------------
// CWatchView::OnSelchangeTab
//    When a new tab is chosen, show the variables that are part of
//    that context.
//----------------------------------------------------------------------
void CWatchView::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult)
{
	((CScrollView*)controlView)->ScrollToPosition(CPoint(0,0));
	int idx = m_tab.GetCurSel();
	if (idx >= 0 && idx < contexts.GetSize())
	{
		contexts[idx]->Show(SW_SHOW);
		((CScrollView*)controlView)->SetScrollSizes(MM_TEXT, contexts[idx]->ScrollSize());
	}
	controlView->Invalidate();

	if (pResult)
		*pResult = 0;
}

//----------------------------------------------------------------------
// CWatchView::OnSelchangingTab
//    Before the selection is changed on the tab, hide the controls
//    that are associated with that context.
//----------------------------------------------------------------------
void CWatchView::OnSelchangingTab(NMHDR* pNMHDR, LRESULT* pResult)
{
	int idx = m_tab.GetCurSel();
	if (idx >= 0 && idx < contexts.GetSize())
	{
		contexts[idx]->Show(SW_HIDE);
	}

	*pResult = 0;
}

BEGIN_EVENTSINK_MAP(CWatchView, CFormView)
    //{{AFX_EVENTSINK_MAP(CWatchView)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CWatchView::OnViewDebugoutput(bool bSetState /*=true*/)
{
	if (bSetState)
	{
		if (printDlg->IsWindowVisible())
		{
			printDlg->ShowWindow(SW_HIDE);
		}
		else
		{
			printDlg->ShowWindow(SW_SHOW);
		}
	}

	CString str;
	str.Format("Dialogs\\%d", CPrintDlg::IDD);
	AfxGetApp()->WriteProfileInt(str, "Visible", printDlg->IsWindowVisible());
}

void CWatchView::OnUpdateViewDebugoutput(CCmdUI* pCmdUI)
{
	if (printDlg->IsWindowVisible())
		pCmdUI->SetCheck(true);
	else
		pCmdUI->SetCheck(false);
}

void CWatchView::OnViewStripchart(bool bSetState /*=true*/)
{
	if (bSetState)
	{
		if (stripDlg->IsWindowVisible())
		{
			stripDlg->ShowWindow(SW_HIDE);
		}
		else
		{
			stripDlg->ShowWindow(SW_SHOW);
		}
	}

	CString str;
	str.Format("Dialogs\\%d", CStripDlg::IDD);
	AfxGetApp()->WriteProfileInt(str, "Visible", stripDlg->IsWindowVisible());
}

void CWatchView::OnUpdateViewStripchart(CCmdUI* pCmdUI)
{
	if (stripDlg->IsWindowVisible())
		pCmdUI->SetCheck(true);
	else
		pCmdUI->SetCheck(false);
}

LRESULT CWatchView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  	if (WM_STRIPDLG_DONE == message)
  		OnViewStripchart(false);
  	else if (WM_PRINTDLG_DONE == message)
  		OnViewDebugoutput(false);
  	else if (WM_PLOTDLG_DONE == message)
  		OnViewPlotdialog(false);

	return CFormView::WindowProc(message, wParam, lParam);
}

void CWatchView::OnViewPlotdialog(bool bSetState /*=true*/)
{
	if (bSetState)
	{
		if (plotDlg->IsWindowVisible())
		{
			plotDlg->ShowWindow(SW_HIDE);
		}
		else
		{
			plotDlg->ShowWindow(SW_SHOW);
		}
	}

	CString str;
	str.Format("Dialogs\\%d", CPlotDlg::IDD);
	AfxGetApp()->WriteProfileInt(str, "Visible", plotDlg->IsWindowVisible());
}

void CWatchView::OnUpdateViewPlotdialog(CCmdUI* pCmdUI)
{
	if (plotDlg->IsWindowVisible())
		pCmdUI->SetCheck(true);
	else
		pCmdUI->SetCheck(false);
}

void CWatchView::OnFileSave()
{
	CSaveContext dlg(IDD_CONTEXT_SAVE);
	dlg.m_filename.Format("%s.var", contexts[m_tab.GetCurSel()]->Name());
	if (IDOK == dlg.DoModal())
	{
		PKT pkt;
		pkt.pktType = PTYPE_SAVE_CONTEXT;
		pkt.contextInfo.context = m_tab.GetCurSel();
		pkt.contextInfo.table = contexts[pkt.contextInfo.context]->GetTable();
		strncpy(pkt.contextInfo.name, dlg.m_filename, sizeof(pkt.contextInfo.name) - 1);
		WatchComm::Instance()->SendPacket(&pkt);
	}
}

void CWatchView::OnFileSaveall()
{
	if (IDOK == AfxMessageBox("Save all contexts?", MB_OKCANCEL))
	{
		PKT pkt;
		pkt.pktType = PTYPE_SAVE_CONTEXT;
		for (int i = 0; i < m_tab.GetItemCount(); i++)
		{
			pkt.contextInfo.context = i;
			pkt.contextInfo.table = contexts[i]->GetTable();
			sprintf(pkt.contextInfo.name, "%s.var", contexts[i]->Name());
			WatchComm::Instance()->SendPacket(&pkt);
		}
	}
}

void CWatchView::RefreshVariable(VAR_INFO& var)
{
	contexts[var.context]->RefreshVariable(var);
	controlView->Invalidate();
}


void CWatchView::OnFileOpen()
{
	CSaveContext dlg(IDD_CONTEXT_OPEN);
	dlg.m_filename.Format("%s.var", contexts[m_tab.GetCurSel()]->Name());

	if (IDOK == dlg.DoModal())
	{
		contexts[m_tab.GetCurSel()]->Clear();
		PKT pkt;
		pkt.pktType = PTYPE_OPEN_CONTEXT;
		pkt.contextInfo.context = m_tab.GetCurSel();
		pkt.contextInfo.table = contexts[pkt.contextInfo.context]->GetTable();
		strncpy(pkt.contextInfo.name, dlg.m_filename, sizeof(pkt.contextInfo.name) - 1);
		WatchComm::Instance()->SendPacket(&pkt);
		if (pkt.contextInfo.table)
			contexts[pkt.contextInfo.context]->Disable();
	}
}

void CWatchView::OnContextRefresh()
{
	PKT pkt;
	pkt.pktType = PTYPE_CONTEXT_REFRESH;
	pkt.contextInfo.context = m_tab.GetCurSel();
	pkt.contextInfo.table = contexts[pkt.contextInfo.context]->GetTable();
	WatchComm::Instance()->SendPacket(&pkt);
}

void CWatchView::OnSelchangeList()
{
	ASSERT(contexts.GetSize() > m_tab.GetCurSel());
	contexts[m_tab.GetCurSel()]->OnSelchangeList();
}

void CWatchView::TableData(TABLE_DATA& data)
{
	ASSERT(contexts.GetSize() > m_tab.GetCurSel());
	contexts[m_tab.GetCurSel()]->Data(data);
}

