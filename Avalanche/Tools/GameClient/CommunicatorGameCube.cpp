// CommunicatorGameCube.cpp: implementation of the CCommunicatorGameCube class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gameclient.h"
#include "CommunicatorGameCube.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommunicatorGameCube::CCommunicatorGameCube(CString DeviceIPAddress)
	: CCommunicator(DeviceIPAddress)
{

}

CCommunicatorGameCube::~CCommunicatorGameCube()
{

}

BOOL CCommunicatorGameCube::Connect()
{
	AfxMessageBox("This communication protocol is incomplete");
	return FALSE;
}

BOOL CCommunicatorGameCube::Disconnect()
{
	return FALSE;
}
