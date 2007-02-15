// AMViewerSplitter.cpp : implementation file
//

#include "stdafx.h"
#include "AMViewer.h"
#include "AMViewerSplitter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAMViewerSplitter

CAMViewerSplitter::CAMViewerSplitter()
{
}

CAMViewerSplitter::~CAMViewerSplitter()
{
}

BEGIN_MESSAGE_MAP(CAMViewerSplitter, CSplitterWnd)
	//{{AFX_MSG_MAP(CAMViewerSplitter)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAMViewerSplitter message handlers

void CAMViewerSplitter::OnSize(UINT nType, int cx, int cy) 
{
	if (GetDlgItem(IdFromRowCol(0, 1)))
   	CSplitterWnd::OnSize(nType, cx, cy);
}
