// CommunicatorPS2.cpp: implementation of the CCommunicatorPS2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gameclient.h"
#include "CommunicatorPS2.h"
#include "FileServerConnection.h"
#include "OldFileServerConnection.h"
#include "OutputLogConnection.h"
#include "PS2TestPane.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommunicatorPS2::CCommunicatorPS2(CString DeviceIPAddress)
	: CCommunicator(DeviceIPAddress)
{
}

CCommunicatorPS2::~CCommunicatorPS2()
{

}

BOOL CCommunicatorPS2::Connect()
{
	// add the panes to the display
	struct PaneArray panes[] = 
	{
		{ new CPS2TestPane(),		"System Control",		NULL,					0,		NULL },
		{ new COutputLogPane(),		"Output Log",			&m_OutputServer,	9124,	RUNTIME_CLASS(COutputLogConnection) },
		{ new CFileServerPane(),	"Old File Server",	&m_OldFileServer,	8420,	RUNTIME_CLASS(COldFileServerConnection) },
		{ new CFileServerPane(),	"New File Server",	&m_FileServer,		9123,	RUNTIME_CLASS(CFileServerConnection) },
		{ NULL,							"",						NULL,					0,		NULL }
	};

	return AddPanes(panes);
}

BOOL CCommunicatorPS2::Disconnect()
{
	m_FileServer.StopListen();
	m_OldFileServer.StopListen();

	theApp.ResetPanes();

	return TRUE;
}

