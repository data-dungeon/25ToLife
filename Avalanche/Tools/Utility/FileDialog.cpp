//

#include "stdafx.h"
#include "FileDialog.h"
#include <dlgs.h>
#include ".\filedialog.h"

//////////////////////////////////////////
// SnipeFileDialog

SnipeFileDialog::SnipeFileDialog( BOOL bOpenFileDialog, const String strInitFileName, const String &strFilter, const char *szTitleBar/*=NULL*/, int nInitFilterIndex/*=0*/, const char *szDefExt/*=NULL*/ ) :
   CFileDialog( bOpenFileDialog, szDefExt, strInitFileName.Get(), 0, strFilter.Get())
{
   m_ofn.Flags |= OFN_HIDEREADONLY|OFN_ENABLESIZING;

   if (bOpenFileDialog)
      m_ofn.Flags |= OFN_FILEMUSTEXIST;
   else
      m_ofn.Flags |= OFN_OVERWRITEPROMPT;

   m_ofn.nFilterIndex = nInitFilterIndex;

   if (szTitleBar)
   {
      m_strTitleBar = szTitleBar;
      m_ofn.lpstrTitle = m_strTitleBar.Get();
   }
   m_filterarray = strFilter.SplitToArray('|');
}

SnipeFileDialog::~SnipeFileDialog()
{
   if (m_filterarray)
      delete m_filterarray;
}

void SnipeFileDialog::OnTypeChange()
{
   if (m_bOpenFileDialog)
      return;

   try {
      String strFileName = (LPCTSTR)GetFileTitle();
      int nIndex = m_ofn.nFilterIndex;

      String strExtension = m_filterarray->GetAt((nIndex-1)*2+1);
      if (strExtension.Find('*')==-1) {
         ASSERT(FALSE);
         return; 
      }
      strExtension = strExtension.Right(strExtension.Length()-2);
      nIndex = strExtension.Find(';');
      if (nIndex != -1) 
         strExtension = strExtension.Right(strExtension.Length()-nIndex-1);

      strFileName = strFileName.GetWithoutSuffix()+'.' + strExtension;
      SetControlText(edt1, strFileName.Get());
   }
   catch(...)
   {
      ASSERT(FALSE);
      return;
   }
}

void SnipeFileDialog::OnInitDone()
{
   PostMessage(WM_MYINITDONE, 0, 0);

   CRect rc;
   CWnd *wndDlg = GetParent();
   wndDlg->GetWindowRect(&rc);
   wndDlg->SetWindowPos(NULL, -2000, -2000, rc.Width(), rc.Height(), SWP_NOZORDER);
}

BEGIN_MESSAGE_MAP(SnipeFileDialog, CFileDialog)
   ON_MESSAGE(WM_MYINITDONE, OnMyInitDone)
   ON_WM_DESTROY()
END_MESSAGE_MAP()

LRESULT SnipeFileDialog::OnMyInitDone(WPARAM wParam, LPARAM lParam)
{
   OnTypeChange();

// Move to initial position
   int left   = ::AfxGetApp()->GetProfileInt("FileDialog", "Pos-Left", 70);
   int top    = ::AfxGetApp()->GetProfileInt("FileDialog", "Pos-Top", 50);
   int width  = ::AfxGetApp()->GetProfileInt("FileDialog", "Pos-Width", 500);
   int height = ::AfxGetApp()->GetProfileInt("FileDialog", "Pos-Height", 380);

	int rightofscreen = GetSystemMetrics(SM_CXSCREEN);
	int bottomofscreen = GetSystemMetrics(SM_CYSCREEN);
   
   if (left + width < 0 )
      left = 0;
   else if (left > rightofscreen )
      left = rightofscreen - width;
   if (top + height < 0 )
      top = 0;
   else if (top > bottomofscreen )
      top = bottomofscreen - height;   

   GetParent()->MoveWindow(left, top, width, height);

   return 0;
}

void SnipeFileDialog::OnDestroy()
{
   CWnd *wndDlg = GetParent();

   // Save dialog position
   CRect rc;
   wndDlg->GetWindowRect( &rc );
   if (rc.right >0  && rc.bottom > 0) {
      ::AfxGetApp()->WriteProfileInt("FileDialog", "Pos-Left", (int)rc.left);
      ::AfxGetApp()->WriteProfileInt("FileDialog", "Pos-Top", (int)rc.top);
      ::AfxGetApp()->WriteProfileInt("FileDialog", "Pos-Width", rc.Width());
      ::AfxGetApp()->WriteProfileInt("FileDialog", "Pos-Height", rc.Height());
   }

   CFileDialog::OnDestroy();
}

//////////////////////////////////////////////////////////////////

BOOL GetOpenFileName( const String strInitFileName, const String &strFilter, String &strOutFileName, const char *szTitleBar/*=NULL*/, int *nOutIndex/*=NULL*/  )
{
   SnipeFileDialog dlg( TRUE, strInitFileName, strFilter, szTitleBar );
   if (dlg.DoModal() != IDOK)
      return FALSE;

   strOutFileName = dlg.GetPathName();

   if (nOutIndex)
      *nOutIndex = dlg.m_ofn.nFilterIndex;

   AfxGetMainWnd()->UpdateWindow();

   return TRUE;
}

BOOL GetOpenFileName( const String strInitFileName, const String &strFilter, StringList &strOutFileNames, const char *szTitleBar/*=NULL*/, int *nOutIndex/*=NULL*/  )
{
   SnipeFileDialog dlg( TRUE, strInitFileName, strFilter, szTitleBar );
   dlg.m_ofn.Flags |= OFN_ALLOWMULTISELECT;
   if (dlg.DoModal() != IDOK)
      return FALSE;

   for (POSITION pos=dlg.GetStartPosition(); pos; )
   {
      String strFileName = dlg.GetNextPathName( pos );
      strOutFileNames.AddTail( strFileName );
   }

   if (nOutIndex)
      *nOutIndex = dlg.m_ofn.nFilterIndex;

   AfxGetMainWnd()->UpdateWindow();

   return TRUE;
}

BOOL GetSaveFileName(const String strInitFileName, const String &strFilter, String &strOutFileName, const char *szTitleBar/*=NULL*/, int nInitFilterIndex/*=0*/, int *nOutIndex/*=NULL*/, const String &strAppenExt/*=""*/ )
{
   SnipeFileDialog dlg( FALSE, strInitFileName, strFilter, szTitleBar, nInitFilterIndex, strAppenExt.Get() );
   if (dlg.DoModal() != IDOK)
      return FALSE;

   strOutFileName = dlg.GetPathName();

   if (nOutIndex)
      *nOutIndex = dlg.m_ofn.nFilterIndex;

   AfxGetMainWnd()->UpdateWindow();

   return TRUE;
}
