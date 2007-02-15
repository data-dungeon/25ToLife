// ViewHeaderWnd.cpp : implementation file
//

#include "stdafx.h"
#include "Snipe.h"
#include "ViewHeaderWnd.h"
#include ".\viewheaderwnd.h"


// CViewHeaderWnd

IMPLEMENT_DYNAMIC(CViewHeaderWnd, CWnd)
CViewHeaderWnd::CViewHeaderWnd()
{
}

CViewHeaderWnd::~CViewHeaderWnd()
{
}


BEGIN_MESSAGE_MAP(CViewHeaderWnd, CWnd)
   ON_WM_PAINT()
   ON_WM_CREATE()
END_MESSAGE_MAP()



// CViewHeaderWnd message handlers


BOOL CViewHeaderWnd::Create(CWnd * pParent)
{
   //return CEdit::Create( WS_VISIBLE|WS_CHILD, CRect(0,0,10,10), pParent, 0xFFFE );
   return CWnd::Create( NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), pParent, -1 );
}

void CViewHeaderWnd::OnPaint()
{
   CPaintDC dc(this); // device context for painting

   CRect rcClient;
   GetClientRect( &rcClient );

   dc.FillSolidRect( &rcClient, RGB(128,128,128));
}

int CViewHeaderWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CWnd::OnCreate(lpCreateStruct) == -1)
      return -1;

   if (!m_wndViewCombo.Create( WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, CRect( 0,0,100,HEIGHT ), this, 0xABCD ))
      return -1;

   m_wndViewCombo.AddString( "Front" );
   m_wndViewCombo.AddString( "Back" );
   m_wndViewCombo.AddString( "Right" );
   m_wndViewCombo.AddString( "Left" );
   m_wndViewCombo.AddString( "Top" );
   m_wndViewCombo.AddString( "Bottom" );
   m_wndViewCombo.AddString( "Camera" );

   return 0;
}
