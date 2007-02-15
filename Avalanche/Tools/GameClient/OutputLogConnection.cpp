// OutputLogConnection.cpp: implementation of the COutputLogConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gameclient.h"
#include "OutputLogConnection.h"
#include "OutputLogPane.h"
#include "Server.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(COutputLogConnection, CConnection);

COutputLogConnection::COutputLogConnection()
{

}

COutputLogConnection::~COutputLogConnection()
{

}

COutputLogPane* COutputLogConnection::GetPane()
{
	COutputLogPane* pane = (COutputLogPane*)(m_Server->GetPane());
	ASSERT(pane->IsKindOf(RUNTIME_CLASS(COutputLogPane)));
	return pane;
}

int COutputLogConnection::DataReceived(unsigned long bytes, void* data)
{
	COutputLogPane* pane = GetPane();
	CString text;
	LPSTR str = text.GetBufferSetLength(bytes+1);
	memcpy(str, data, bytes);
	str[bytes] = '\0';
	text.ReleaseBuffer();
	pane->AppendText(text);
	return bytes;
}

