// Communicator.cpp: implementation of the CCommunicator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gameclient.h"
#include "Communicator.h"

#include "Server.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommunicator::CCommunicator(CString deviceIPAddress)
{
	m_DeviceIPAddress = deviceIPAddress;
}

CCommunicator::~CCommunicator()
{

}



CString CCommunicator::GetDeviceIPAddress()
{
	return m_DeviceIPAddress;
}

BOOL CCommunicator::AddPanes(CCommunicator::PaneArray *panes)
{
	int paneIdx;
	BOOL result = TRUE;

	for(paneIdx = 0; panes[paneIdx].pane != NULL || panes[paneIdx].server != NULL; paneIdx++)
	{
		int paneTab = -1;

		if(panes[paneIdx].pane)
		{
			paneTab = theApp.AddPane(panes[paneIdx].pane, panes[paneIdx].name);

			if(paneTab == -1)
				result = FALSE;
		}

		if(panes[paneIdx].server)
		{
			if(!panes[paneIdx].server->Listen(panes[paneIdx].port, panes[paneIdx].client))
			{
				CString msg;
				msg.Format("%s couldnt listen on IP port %d.  Perhaps something is already listening on it?",
					panes[paneIdx].name, panes[paneIdx].port);
				AfxMessageBox(msg);
				result = FALSE;
			}
			panes[paneIdx].server->SetPaneNumber(paneTab);
		}
	}

	return result;
}

CString CCommunicator::GetSocketErrorString(int socketError)
{
	const char* msg;
	switch(socketError)
	{
	case WSANOTINITIALISED:
		msg = "Windows Sockets Not Initialized";
		break;
	case WSAENETDOWN:
		msg = "Network subsystem failure";
		break;
	case WSAEADDRINUSE:
		msg = "Specified address in use";
		break;
	case WSAEINPROGRESS:
		msg = "Blocking socket call in progress";
		break;
	case WSAEADDRNOTAVAIL:
		msg = "Specified address is not available locally";
		break;
	case WSAEAFNOSUPPORT:
		msg = "Addresses in that family cant be used with this socket";
		break;
	case WSAECONNREFUSED:
		msg = "Connection Refused";
		break;
	case WSAEDESTADDRREQ:
		msg = "Destination address is required";
		break;
	case WSAEFAULT:
		msg = "Internal fault";
		break;
	case WSAEINVAL:
		msg = "Invalid host address";
		break;
	case WSAEISCONN:
		msg = "Socket already connected";
		break;
	case WSAEMFILE:
		msg = "Out of file descriptors";
		break;
	case WSAENETUNREACH:
		msg = "Net unreachable";
		break;
	case WSAENOBUFS:
		msg = "Out of mem";
		break;
	case WSAENOTSOCK:
		msg = "Not a socket";
		break;
	case WSAETIMEDOUT:
		msg = "Command timed out";
		break;
	case WSAEWOULDBLOCK:
		msg = "Command would block";
		break;
	default:
		msg = "Unknown error occured";
		break;
	}
	return msg;
}
