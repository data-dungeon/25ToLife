// Dan  4/11/00  \Dan85\AMViewer\ScriptView.cpp

#include "stdafx.h"
#include "AMViewerDoc.h"
#include "AMViewer.h"
#include "AMViewerView.h"
#include "ScriptView.h"
#include "MainFrm.h"
#include "Model.h"
#include "StrClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _MAC
   #define ACTIONWIDTH 131
   #define LENGTHWIDTH 60
#else
   #define ACTIONWIDTH 96
   #define LENGTHWIDTH 60
#endif

extern CMainFrame *g_mainframe;

IMPLEMENT_DYNCREATE(CScriptView, CFormView)

CScriptView::CScriptView()
	: CFormView(CScriptView::IDD)
{
   m_bbuiltcolumns = FALSE;
	//{{AFX_DATA_INIT(CScriptView)
	//}}AFX_DATA_INIT
}

CScriptView::~CScriptView()
{
   g_mainframe->m_scriptview = NULL;
}

void CScriptView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScriptView)
	DDX_Control(pDX, IDC_FPS_EDIT, m_fpsedit);
	DDX_Control(pDX, IDC_OPENMODEL, m_openmodel);
	DDX_Control(pDX, IDC_DELETE, m_deleteaction);
	DDX_Control(pDX, IDC_ADD, m_addaction);
	DDX_Control(pDX, IDC_ACTION_LIST, m_actionlist);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScriptView, CFormView)
	//{{AFX_MSG_MAP(CScriptView)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_ADD, OnAddAction)
	ON_BN_CLICKED(IDC_DELETE, OnDeleteAction)
	ON_BN_CLICKED(IDC_OPENMODEL, OnOpenModel)
	ON_EN_KILLFOCUS(IDC_FPS_EDIT, OnKillfocusFpsEdit)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ACTION_LIST, OnItemchangedActionList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptView diagnostics

#ifdef _DEBUG
void CScriptView::AssertValid() const
{
	CFormView::AssertValid();
}

void CScriptView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CAMViewerDoc* CScriptView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAMViewerDoc)));
	return (CAMViewerDoc*)m_pDocument;
}

#endif //_DEBUG

void CScriptView::DeleteContents()
{
   if (m_actionlist.GetSafeHwnd()) {
      m_actionlist.DeleteAllItems();
      SetDlgItemText(IDC_MODELNAME_EDIT, " ");
   }
}

void CScriptView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);

   if (m_actionlist.GetSafeHwnd()) {
      CRect rect;
      int width;

      // resize list box
      m_actionlist.GetWindowRect(&rect);
      ScreenToClient(&rect);
      rect.right = cx-10;
      rect.bottom = cy - 10;
      m_actionlist.MoveWindow(rect);

      // move open model button
      m_openmodel.GetWindowRect(&rect);
      ScreenToClient(&rect);
      width = rect.Width();
      rect.right = cx-10;
      rect.left = rect.right-width;
      m_openmodel.MoveWindow(rect);

      // size open model text
	  CWnd *wnd;
      wnd = GetDlgItem(IDC_MODELNAME_EDIT);
      wnd->GetWindowRect(&rect);
      ScreenToClient(&rect);
      rect.right = cx - (15 + width);//openbuttonwidth
      wnd->MoveWindow(rect);

      // move FPS text
      wnd = GetDlgItem(IDC_FPS_EDIT);
      wnd->GetWindowRect(&rect);
      ScreenToClient(&rect);
      width = rect.Width();
      rect.right = cx-10;
      rect.left = rect.right-width;
      wnd->MoveWindow(rect);

      // move FPS static text
      wnd = GetDlgItem(IDC_FPS_STATIC);
      wnd->GetWindowRect(&rect);
      ScreenToClient(&rect);
      int staticwidth = rect.Width();
      rect.right = cx - (15 + width);//fps edit width
      rect.left = rect.right-staticwidth;
      wnd->MoveWindow(rect);
      wnd->Invalidate();
   }
}

 void CScriptView::AddActionToListCtrl(Action *action)
{
   int index = m_actionlist.InsertItem(LVIF_TEXT|LVIF_PARAM|LVIF_STATE, 32000, action->m_name, LVIS_SELECTED, 0, 0, (LPARAM)action);
   CString text;
   text.Format("%s", action->m_length.ToString(GetDocument()->m_fps, SMPTE, FALSE).Get());
	m_actionlist.SetItem( index, 1, LVIF_TEXT, text, 0, 0, 0, (LPARAM)0 );
}

void CScriptView::RefreshActionList()
{
   int count = m_actionlist.GetItemCount();
   for (int i=0; i < count; i++) {
      Action *action = (Action *)m_actionlist.GetItemData(i);
      CString text;
      text.Format("%s", action->m_length.ToString(GetDocument()->m_fps, SMPTE, FALSE).Get());
      m_actionlist.SetItem( i, 1, LVIF_TEXT, text, 0, 0, 0, (LPARAM)0 );
   }
}

void CScriptView::OnAddAction() 
{
#ifdef _MAC
   static char BASED_CODE szFilter[] = "Motion Files [*.mot,*.kfm]|****||";
#else
    static char BASED_CODE szFilter[] = "Motion Files [*.mot,*.kfm]|*.mot;*.kfm||";
#endif

	CString filename = AfxGetApp()->GetProfileString( "Files", "Last Action File" );
	CString ext = AfxGetApp()->GetProfileString( "Files", "Last Action Extension", "mot" );

	CFileDialog fd(TRUE, ext, filename, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, szFilter);
	
	BOOL wasplay = g_mainframe->m_modelview->SetPlaying(FALSE);

	char bigBuff[4096] = "";
	fd.m_ofn.lpstrFile = bigBuff;
	fd.m_ofn.nMaxFile = sizeof(bigBuff);
	
	int status = fd.DoModal();

	DWORD test = CommDlgExtendedError();

	g_mainframe->m_modelview->SetPlaying(wasplay);

	if (status != IDOK)
		return ;

	POSITION pos = fd.GetStartPosition();

   while (pos) {
	   filename = fd.GetNextPathName(pos);
	   ext = filename.Mid(filename.ReverseFind('.')+1);
	   ext.MakeLower();

	   AfxGetApp()->WriteProfileString( "Files", "Last Action File", filename);
	   AfxGetApp()->WriteProfileString( "Files", "Last Action Extension", ext);

	   Action *action = GetDocument()->LoadAction(filename);
	   if (action == NULL)
		  return;

	   GetDocument()->SetModifiedFlag();
	   GetDocument()->AddAction(action);
	   AddActionToListCtrl(action);
   }
}

void CScriptView::OnDeleteAction() 
{
   GetDocument()->SetModifiedFlag();
   int index=-1;
   while ((index = m_actionlist.GetNextItem(index, LVNI_ALL|LVNI_SELECTED)) != -1) {
      Action *action = (Action *)m_actionlist.GetItemData(index);
	  GetDocument()->RemoveAction(action);
	  m_actionlist.DeleteItem(index);
	  index--;
   }
}

void CScriptView::OnOpenModel() 
{
#ifdef _MAC
   static char BASED_CODE szFilter[] = "Model Files (.av2;.av3;.av4;.ply)|****|Avatar 2 Files (.av2)|****|Avatar 3 Files (.av3)|****|Avatar 4 Files (.av4)|****|Polygon File (.ply)|****||";
#else
   static char BASED_CODE szFilter[] = "Model Files (.av2;.av3;.av4;.ply)|*.av2;*.av3;*.av4;*.ply|Avatar 2 Files (.av2)|*.av2|Avatar 3 Files (.av3)|*.av3||Avatar 4 Files (.av4)|*.av4|Polygon File (.ply)|*.ply||";
#endif

   CString filename;
   CString ext;

   Model *model = GetDocument()->m_model;

   if (model) {
      filename = model->m_name;
      ext = filename.Mid(filename.ReverseFind('.')+1);
      ext.MakeLower();
   }
   else {
      filename = AfxGetApp()->GetProfileString( "Files", "Last Model File" );
      ext = AfxGetApp()->GetProfileString( "Files", "Last Model Extension", "av2" );
   }

   CFileDialog fd(TRUE, ext, filename, OFN_HIDEREADONLY, szFilter);

   BOOL wasplay = g_mainframe->m_modelview->SetPlaying(FALSE);

   int status = fd.DoModal();

   g_mainframe->m_modelview->SetPlaying(wasplay);

   if (status != IDOK)
      return;

   filename = fd.GetPathName();
   ext = filename.Mid(filename.ReverseFind('.')+1);
   ext.MakeLower();

   AfxGetApp()->WriteProfileString( "Files", "Last Model File", filename);
   AfxGetApp()->WriteProfileString( "Files", "Last Model Extension", ext);

   if (!GetDocument()->LoadModel(filename))
      return;

   g_mainframe->m_modelview->InitView();
   g_mainframe->m_modelview->InvalidateRect(NULL, FALSE);
   g_mainframe->m_modelview->UpdateWindow();
   GetDocument()->SetModifiedFlag();
}

void CScriptView::OnKillfocusFpsEdit() 
{
   int fps = min(1000, max(1, GetDlgItemInt(IDC_FPS_EDIT)));
   SetDlgItemInt(IDC_FPS_EDIT, fps);
   GetDocument()->m_fps = fps;
   GetDocument()->SetModifiedFlag();
   RefreshActionList();
}

void CScriptView::OnInitialUpdate() 
{
   CFormView::OnInitialUpdate();
   SetDlgItemInt(IDC_FPS_EDIT, GetDocument()->m_fps);

   if (!m_bbuiltcolumns) {
      m_bbuiltcolumns = TRUE;
      m_actionlist.InsertColumn( 0, "Action", LVCFMT_LEFT, 
       AfxGetApp()->GetProfileInt( "Action List", "Action Width", ACTIONWIDTH ));
      m_actionlist.InsertColumn( 1, "Length", LVCFMT_LEFT, 
       AfxGetApp()->GetProfileInt( "Action List", "Length Width", LENGTHWIDTH ));
   }
}

int CScriptView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
   return 0;
}

void CScriptView::OnDestroy() 
{
   CFormView::OnDestroy();
	
   AfxGetApp()->WriteProfileInt( "Action List", "Action Width", m_actionlist.GetColumnWidth(0));
   AfxGetApp()->WriteProfileInt( "Action List", "Length Width", m_actionlist.GetColumnWidth(1));
}

BOOL CScriptView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
   if (CFormView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
      return TRUE;
   else if (g_mainframe->m_modelview)
      return g_mainframe->m_modelview->MyCmdMsg(nID, nCode, pExtra, pHandlerInfo);
   return FALSE;
}

void CScriptView::OnItemchangedActionList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

   SetActiveActions();
   g_mainframe->m_modelview->Invalidate(FALSE);
   g_mainframe->m_modelview->UpdateWindow();
	
	*pResult = 0;
}

void CScriptView::SetActiveActions()
{
   int index =-1;
   Time length = 0;
   while ((index = m_actionlist.GetNextItem(index, LVNI_ALL)) != -1) {
      Action *action = (Action *)m_actionlist.GetItemData(index);
      if (m_actionlist.GetItemState(index, LVIS_SELECTED) & LVNI_SELECTED) {
         action->m_isactive = TRUE;
         length+=action->m_length;
      }
      else
         action->m_isactive = FALSE;
   }

   GetDocument()->m_lasttime.SetTagged(TRUE);
   GetDocument()->m_scriptlength = length;
   g_mainframe->m_modelview->m_time=0;
}
