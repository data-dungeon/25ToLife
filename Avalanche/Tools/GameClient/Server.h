// Server.h: interface for the CServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVER_H__F949E710_168B_4C08_BDC2_8B787693DD68__INCLUDED_)
#define AFX_SERVER_H__F949E710_168B_4C08_BDC2_8B787693DD68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CConnection;

class CServer : public CAsyncSocket
{
public:
	CGameClientPane* GetPane();
	void SetPaneNumber(int pane);
	void CloseConnection(CConnection* connection);
	CServer();
	virtual ~CServer();

	// start and stop listening
	BOOL Listen(int port, CRuntimeClass* connectionClass);
	void StopListen();
	BOOL IsListening();

	virtual void OnAccept(int nErrorCode);
protected:
	int m_PaneNumber;
	// called when a connection is recieved on a listening server
	virtual void OnConnect(CConnection* socket){};

	BOOL m_listening;
	CRuntimeClass* m_ConnectionClass;
};

#endif // !defined(AFX_SERVER_H__F949E710_168B_4C08_BDC2_8B787693DD68__INCLUDED_)
