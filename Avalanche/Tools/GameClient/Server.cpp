// Server.cpp: implementation of the CServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gameclient.h"
#include "Server.h"

#include "Connection.h"
#include "Communicator.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServer::CServer()
{
	m_listening = FALSE;
}

CServer::~CServer()
{
	if(m_listening)
		StopListen();
}

// start and stop listening
BOOL CServer::Listen(int port, CRuntimeClass* connectionClass)
{
	ASSERT(!m_listening);

	if(!Create(port))
	{
		AfxMessageBox("Cant start server! " + CCommunicator::GetSocketErrorString(GetLastError()));
		return FALSE;
	}
	if(!CAsyncSocket::Listen())
	{
		AfxMessageBox("Cant start server! " + CCommunicator::GetSocketErrorString(GetLastError()));
		return FALSE;
	}
	m_listening = TRUE;
	m_ConnectionClass = connectionClass;
	return TRUE;
}

void CServer::StopListen()
{
	ASSERT(m_listening);

	Close();
	m_listening = FALSE;
}

BOOL CServer::IsListening()
{
	return m_listening;
}

// called occationally by the system...
// be sure to call this implimentation if you override
void CServer::OnAccept(int nErrorCode)
{
	ASSERT(IsListening());

	CConnection* connection = (CConnection*)m_ConnectionClass->CreateObject();
	ASSERT(connection->IsKindOf(RUNTIME_CLASS(CConnection)));

	if(Accept(*connection))
	{
		connection->SetServer(this);
		OnConnect(connection);
	}
	else
	{
		delete connection;
	}

}

void CServer::SetPaneNumber(int pane)
{
	m_PaneNumber = pane;
}

CGameClientPane* CServer::GetPane()
{
	return theApp.m_GameClientDlg->GetPane(m_PaneNumber);
}

