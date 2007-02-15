// KB  2/24/99  \Ken71\AMViewer\Options.cpp
#include "stdafx.h"
#include "AMViewerDoc.h"
#include "Options.h"
#include "Resource.h"
#include "AMViewerView.h"
#include "Model.h"


IMPLEMENT_DYNAMIC(COptionsDialog, CDialog)
BEGIN_MESSAGE_MAP(COptionsDialog, CDialog)
END_MESSAGE_MAP()

void CAMViewerView::OnOptions()
{
   COptionsDialog dlg(this);
   
   BOOL wasplay = SetPlaying(FALSE);

   if (dlg.DoModal()==IDOK)
      Invalidate();

   SetPlaying(wasplay);
}


COptionsDialog::COptionsDialog( CAMViewerView *view, CWnd* pParentWnd ) :
 CDialog( IDD_OPTIONS, pParentWnd )
{
   m_view = view;
   m_bbilinear = view->m_bbilinear;
   m_bskipframes = view->m_bskipframes;
   m_focallength = view->m_focallength;
}

BOOL COptionsDialog::OnInitDialog()
{
/*#ifdef _MAC
   LOGFONT normal;
   memset( &normal, 0, sizeof(normal) );
   normal.lfHeight = 11;
   strcpy( normal.lfFaceName, "MS Sans Serif");
   HFONT hFont = CreateFontIndirect( &normal );
   SendMessageToDescendants( WM_SETFONT, (WPARAM)hFont, TRUE );
#endif
*/
   CDialog::OnInitDialog();
   return FALSE;
}

void COptionsDialog::DoDataExchange(CDataExchange* pDX)
{
   DDX_Check(pDX, IDC_BILINEAR, m_bbilinear);
   DDX_Check(pDX, IDC_SKIPFRAMES, m_bskipframes);
   DDX_Text(pDX,  IDC_FOCALLENGTH, m_focallength);
   DDV_MinMaxFloat(pDX, m_focallength, 1, 1000);
}

void COptionsDialog::OnOK()
{
   CDialog::OnOK();
   m_view->m_bbilinear = m_bbilinear;
   m_view->m_bskipframes = m_bskipframes;
   m_view->m_focallength = m_focallength;

   AfxGetApp()->WriteProfileInt("Options", "bilinear", m_bbilinear);
   AfxGetApp()->WriteProfileInt("Options", "skip frames", m_bskipframes);
   CString temp;
   temp.Format("%g", m_focallength);
   AfxGetApp()->WriteProfileString("Options", "focallength", temp);
   if (m_view->GetDocument()->m_model)
      m_view->GetDocument()->m_model->RebuildBinds();
}
