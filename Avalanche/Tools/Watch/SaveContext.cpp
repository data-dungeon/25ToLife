// SaveContext.cpp : implementation file
//

#include "stdafx.h"
#include "watch.h"
#include "SaveContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSaveContext dialog


CSaveContext::CSaveContext(int id, CWnd* pParent /*=NULL*/)
	: CDialog(id, pParent)
{
	//{{AFX_DATA_INIT(CSaveContext)
	m_filename = _T("");
	//}}AFX_DATA_INIT
}


void CSaveContext::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSaveContext)
	DDX_Text(pDX, IDC_FILENAME, m_filename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSaveContext, CDialog)
	//{{AFX_MSG_MAP(CSaveContext)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaveContext message handlers
