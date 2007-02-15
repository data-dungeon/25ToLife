#include "stdafx.h"
#include "GLDialogWnd.h"
#include "GLListControl.h"
#include "GLHotKeyControl.h"
#include "MainFrm.h"
#include "Resource.h"

BEGIN_MESSAGE_MAP(CGLDialogWnd, CGLRenderWnd)
   ON_WM_CREATE()
   ON_WM_SIZE()
END_MESSAGE_MAP()

CGLDialogWnd::CGLDialogWnd()
{
}

CGLDialogWnd::~CGLDialogWnd()
{
}

BOOL CGLDialogWnd::Create( CWnd *pParent, LPCTSTR lpszWindowName, const CRect &rcDlg )
{
   LPCTSTR strClass = AfxRegisterWndClass( CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW));
   return CWnd::CreateEx( 0, strClass, lpszWindowName, WS_POPUP|WS_BORDER, rcDlg, pParent, NULL );
}

BOOL CGLDialogWnd::Create()
{
   return Create( AfxGetMainWnd() );
}

INT_PTR CGLDialogWnd::DoModal()
{
   if (!Create())
      return -1;

   CGLMainRenderWnd *pMainWnd = GetMainFrame()->GetMainRenderWnd();
   pMainWnd->m_glExplorerSplitter.SetEnabled( FALSE );

	m_nModalResult = -1;
	m_nFlags |= WF_CONTINUEMODAL;

   DWORD dwFlags = MLF_SHOWONIDLE;
	if (GetStyle() & DS_NOIDLEMSG)
		dwFlags |= MLF_NOIDLEMSG;

   VERIFY(RunModalLoop(dwFlags) == m_nModalResult);

   pMainWnd->m_glExplorerSplitter.SetEnabled( TRUE );

   DestroyWindow();

   return m_nModalResult;
}

void CGLDialogWnd::InsertControl( CGLControl *pControl, const CRect &rcControl, UINT nID/*=-1*/ )
{
   pControl->Init( GetContentControl(), NULL, rcControl.TopLeft(), rcControl.Size(), nID );
}

int CGLDialogWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CGLRenderWnd::OnCreate(lpCreateStruct) == -1)
      return -1;

   m_glTitleBar.Init( NULL, this );
   m_glContentControl.Init( NULL, this );
   m_glContentControl.LoadBitmap( IDB_BACKGROUND );

   return 0;
}

void CGLDialogWnd::OnSize(UINT nType, int cx, int cy)
{
   CGLRenderWnd::OnSize(nType, cx, cy);

   CPoint pt(0,0);

   m_glTitleBar.Move( pt, CSize( cx, 0 ));
   pt.y += m_glTitleBar.m_szControl.cy;

   m_glContentControl.Move( pt, CSize( cx, cy-pt.y ) );
}

CSize CGLControl::CreateControlsFromDialogTemplate( UINT nIDTemplate )
{
   CDialog dlg;
   if (!dlg.Create( nIDTemplate ))
      return CSize(0,0);

   TCHAR szClassName[MAX_CLASS_NAME];
   CRect rcViewport = GetViewportRect();

   CFont *pFont;
   pFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
   if (!pFont)
      pFont = CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT));

   for (CWnd *pChild=dlg.GetWindow(GW_CHILD); pChild; pChild=pChild->GetWindow(GW_HWNDNEXT))
   {
      ::GetClassName(pChild->m_hWnd, (LPTSTR)szClassName, MAX_CLASS_NAME);

      CGLControl *pControl = NULL;
      if (lstrcmpi(szClassName, _T("Static"))==0)
         pControl = SNEW CGLStaticTextControl;
      else if (lstrcmpi(szClassName, _T("Button"))==0)
         pControl = SNEW CGLButtonControl;
      else if (lstrcmpi(szClassName, _T("Edit"))==0)
         pControl = SNEW CGLEditControl;
      else if (lstrcmpi(szClassName, _T("ListBox"))==0)
         pControl = SNEW CGLListControl;
      else if (lstrcmpi(szClassName, HOTKEY_CLASS)==0)
         pControl = SNEW CGLHotKeyControl;

      CRect rcControl;
      pChild->GetWindowRect( &rcControl );
      dlg.ScreenToClient( &rcControl );

      CString strText;
      pChild->GetWindowText( strText );

      UINT nID = pChild->GetDlgCtrlID();

      if (pControl)
      {
         pControl->Init( this, NULL, rcControl.TopLeft(), rcControl.Size(), nID );
         pControl->SetText( (LPCTSTR)strText );
      }
      else
      {
         //rcControl += rcViewport.TopLeft();
         //if (lstrcmpi(szClassName, HOTKEY_CLASS)==0)
         //{
         //   CHotKeyCtrl *pHotKey = SNEW CHotKeyCtrl;
         //   pHotKey->Create( WS_CHILD|WS_VISIBLE, rcControl, m_pGLRenderWnd, nID );
         //   pHotKey->SetFont( pFont );
         //}
      }
   }

   CRect rcClient;
   dlg.GetClientRect( &rcClient );
   dlg.DestroyWindow();
   return rcClient.Size();
}
