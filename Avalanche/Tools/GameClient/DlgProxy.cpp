// DlgProxy.cpp : implementation file
//

#include "stdafx.h"
#include "GameClient.h"
#include "DlgProxy.h"
#include "GameClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGameClientDlgAutoProxy

IMPLEMENT_DYNCREATE(CGameClientDlgAutoProxy, CCmdTarget)

CGameClientDlgAutoProxy::CGameClientDlgAutoProxy()
{
	EnableAutomation();
	
	// To keep the application running as long as an automation 
	//	object is active, the constructor calls AfxOleLockApp.
	AfxOleLockApp();

	// Get access to the dialog through the application's
	//  main window pointer.  Set the proxy's internal pointer
	//  to point to the dialog, and set the dialog's back pointer to
	//  this proxy.
	ASSERT (AfxGetApp()->m_pMainWnd != NULL);
	ASSERT_VALID (AfxGetApp()->m_pMainWnd);
	ASSERT_KINDOF(CGameClientDlg, AfxGetApp()->m_pMainWnd);
	m_pDialog = (CGameClientDlg*) AfxGetApp()->m_pMainWnd;
	m_pDialog->m_pAutoProxy = this;
}

CGameClientDlgAutoProxy::~CGameClientDlgAutoProxy()
{
	// To terminate the application when all objects created with
	// 	with automation, the destructor calls AfxOleUnlockApp.
	//  Among other things, this will destroy the main dialog
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void CGameClientDlgAutoProxy::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CGameClientDlgAutoProxy, CCmdTarget)
	//{{AFX_MSG_MAP(CGameClientDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CGameClientDlgAutoProxy, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CGameClientDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IGameClient to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {8CCC8BBA-F068-4C27-96B5-2C334E38CE96}
static const IID IID_IGameClient =
{ 0x8ccc8bba, 0xf068, 0x4c27, { 0x96, 0xb5, 0x2c, 0x33, 0x4e, 0x38, 0xce, 0x96 } };

BEGIN_INTERFACE_MAP(CGameClientDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CGameClientDlgAutoProxy, IID_IGameClient, Dispatch)
END_INTERFACE_MAP()

// The IMPLEMENT_OLECREATE2 macro is defined in StdAfx.h of this project
// {8AB45716-DBDD-466B-BEA2-11C9601A2083}
IMPLEMENT_OLECREATE2(CGameClientDlgAutoProxy, "GameClient.Application", 0x8ab45716, 0xdbdd, 0x466b, 0xbe, 0xa2, 0x11, 0xc9, 0x60, 0x1a, 0x20, 0x83)

/////////////////////////////////////////////////////////////////////////////
// CGameClientDlgAutoProxy message handlers
