// CommunicatorXBOX.cpp: implementation of the CCommunicatorXBOX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gameclient.h"
#include "CommunicatorXBOX.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommunicatorXBOX::CCommunicatorXBOX(CString DeviceIPAddress)
	: CCommunicator(DeviceIPAddress)
{

}

CCommunicatorXBOX::~CCommunicatorXBOX()
{

}

BOOL CCommunicatorXBOX::Connect()
{
	AfxMessageBox("This communication protocol is incomplete");
	return FALSE;
}

BOOL CCommunicatorXBOX::Disconnect()
{
	return FALSE;
}
