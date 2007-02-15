// ConnectingMessage.cpp : implementation file
//

#include "stdafx.h"
#include "gameclient.h"
#include "ConnectingMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectingMessage dialog


CConnectingMessage::CConnectingMessage(CWnd* pParent /*=NULL*/)
	: CDialog(CConnectingMessage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConnectingMessage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CConnectingMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectingMessage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConnectingMessage, CDialog)
	//{{AFX_MSG_MAP(CConnectingMessage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectingMessage message handlers
