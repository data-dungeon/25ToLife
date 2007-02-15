// Connection.cpp: implementation of the CConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gameclient.h"
#include "Connection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNAMIC(CConnection, CAsyncSocket);

/////////////////////////////////////////////////////////////////////////////
// CConnection

CConnection::CConnection()
{
	m_Buffer = NULL;
	m_BufferSize = 0;
	m_TotalBytes = 0;
	m_Server = NULL;

	m_DebugConnection = FALSE;
}

CConnection::~CConnection()
{
	if(m_Buffer) free(m_Buffer);
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MbAP(CConnection, CAsyncSocket)
	//{{AFX_MSG_MAP(CConnection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CConnection member functions

BOOL CConnection::OnIdle()
{
	return FALSE;
}

void CConnection::OnReceive(int nErrorCode) 
{
	TCHAR buffer[4096];
	int nRead = Receive(buffer, 4096);
	
	if(m_DebugConnection)
		afxDump.HexDump("R", (unsigned char*)&buffer[0], nRead, 20);

	// grow buffer if nessisary...
	if(m_BufferSize < m_TotalBytes + nRead)
	{
		if(m_Buffer == NULL)
		{
			m_BufferSize = m_TotalBytes + nRead;
			if(m_BufferSize < 4096)
				m_BufferSize = 4096;
			m_Buffer = (TCHAR*)malloc(m_BufferSize);
		}
		else
		{
			m_BufferSize = m_TotalBytes + nRead;
			m_Buffer = (TCHAR*)realloc(m_Buffer, m_BufferSize);
		}
	}
	ASSERT(m_Buffer);

	memcpy(&m_Buffer[m_TotalBytes], buffer, nRead);

	m_TotalBytes += nRead;

	int nUsed;
	while(m_TotalBytes && ((nUsed = DataReceived(m_TotalBytes, m_Buffer)) > 0))
	{
		if(nUsed != m_TotalBytes)
		{
			memmove(m_Buffer, &m_Buffer[nUsed], m_TotalBytes - nUsed);
			m_TotalBytes -= nUsed;
		}
		else
		{
			m_TotalBytes = 0;
		}
	}

	CAsyncSocket::OnReceive(nErrorCode);
}

int CConnection::DataReceived(unsigned long bytes, void* dataBuffer)
{
	// this needs to return how much of the data we used.  If its 0, then the data is queued
	// up for later use.  If its non-zero, we will be called again with the remaining data..
	return bytes;
}

int CConnection::Send(const void* lpBuf, int nBufLen, int nFlags) 
{
	if(m_DebugConnection)
		afxDump.HexDump("S", (unsigned char*)lpBuf, nBufLen, 20);	
	return CAsyncSocket::Send(lpBuf, nBufLen, nFlags);
}

void CConnection::OnClose(int nErrorCode) 
{
	CAsyncSocket::OnClose(nErrorCode);
	delete this;
}

void CConnection::SetServer(CServer *theServer)
{
	m_Server = theServer;
}
