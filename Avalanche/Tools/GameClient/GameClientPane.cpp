// GameClientPane.cpp : implementation file
//

#include "stdafx.h"
#include "gameclient.h"
#include "GameClientPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGameClientPane dialog

IMPLEMENT_DYNAMIC(CGameClientPane, CDialog);

CGameClientPane::CGameClientPane(int idd, CWnd* pParent /*=NULL*/)
	: CDialog(idd, pParent)
{
}

/////////////////////////////////////////////////////////////////////////////
// CGameClientPane message handlers


BOOL CGameClientPane::Connect(CString ipAddress)
{
	return TRUE;
}

BOOL CGameClientPane::Disconnect()
{

	return TRUE;
}

