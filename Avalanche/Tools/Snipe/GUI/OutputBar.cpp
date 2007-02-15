// OutputPane.cpp : implementation file
//

#include "stdafx.h"
#include "Snipe.h"
#include "OutputBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(COutputWnd, CRichEditCtrl);

BEGIN_MESSAGE_MAP(COutputWnd, CRichEditCtrl)
	//{{AFX_MSG_MAP(COutputWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

COutputWnd::COutputWnd()
{
}

COutputWnd::~COutputWnd()
{
}

BOOL COutputWnd::Create(CWnd *pParentWnd)
{
   return CRichEditCtrl::Create(WS_CHILD|WS_VISIBLE|WS_HSCROLL| WS_VSCROLL
    |ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_LEFT|ES_MULTILINE|ES_NOHIDESEL|ES_READONLY, 
    CRect(0,0,0,0), pParentWnd, UINT(IDC_STATIC)); 
}

/////////////////////////////////////////////////////////////////////////////
// COutputWnd message handlers

void COutputWnd::AppendLine(const char *line)
{
   long start,stop;
   GetSel(start,stop);
   SetSel(stop,stop+1);
   ReplaceSel(line,false);
   Invalidate();
}

void COutputWnd::ClearText()
{
   int length = GetTextLength();
   SetSel(0,length);
   Clear();
   Invalidate();
}

