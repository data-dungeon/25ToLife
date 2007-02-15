// CommunicatorPC.cpp: implementation of the CCommunicatorPC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gameclient.h"
#include "CommunicatorPC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommunicatorPC::CCommunicatorPC(CString DeviceIPAddress)
	: CCommunicator(DeviceIPAddress)
{

}

CCommunicatorPC::~CCommunicatorPC()
{

}

BOOL CCommunicatorPC::Connect()
{
	AfxMessageBox("This communication protocol is incomplete");
	return FALSE;
}

BOOL CCommunicatorPC::Disconnect()
{
	return FALSE;
}
