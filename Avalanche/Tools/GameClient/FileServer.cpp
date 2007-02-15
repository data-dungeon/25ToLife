// FileServer.cpp: implementation of the CFileServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gameclient.h"
#include "FileServer.h"

#include "FileServerConnection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileServer::CFileServer()
{

}

CFileServer::~CFileServer()
{

}

BOOL CFileServer::Listen(int port, CRuntimeClass* connectionclass, CString root)
{
	m_Root = root;
	return CServer::Listen(port, connectionclass);
}

void OnConnect(CConnection* socket)
{
	socket->SetString1(m_Root);
}

