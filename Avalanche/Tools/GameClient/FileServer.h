// FileServer.h: interface for the CFileServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILESERVER_H__07FA4845_5A20_460D_8838_93022F4E4BFF__INCLUDED_)
#define AFX_FILESERVER_H__07FA4845_5A20_460D_8838_93022F4E4BFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Server.h"

class CFileServer : public CServer  
{
public:
	CFileServer();
	virtual ~CFileServer();
	BOOL Listen(int port, CString root);

protected:
	virtual void OnConnect(CConnection* socket);
	CString m_Root;
};

#endif // !defined(AFX_FILESERVER_H__07FA4845_5A20_460D_8838_93022F4E4BFF__INCLUDED_)
