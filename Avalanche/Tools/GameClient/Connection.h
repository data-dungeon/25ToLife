// Connection.h: interface for the CConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNECTION_H__338520C0_391F_4FA5_AC3F_7553F83F2226__INCLUDED_)
#define AFX_CONNECTION_H__338520C0_391F_4FA5_AC3F_7553F83F2226__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CConnection command target
class CServer;

class CConnection : public CAsyncSocket
{
	DECLARE_DYNAMIC(CConnection);
// Attributes
public:

// Operations
public:
	CConnection();
	virtual ~CConnection();

// Overrides
public:
	void SetServer(CServer* theServer);
	virtual BOOL OnIdle();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnection)
	public:
	virtual void OnReceive(int nErrorCode);
	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);
	virtual void OnClose(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CConnection)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
	CServer* m_Server;
	BOOL m_DebugConnection;
	virtual BOOL DataReceived(unsigned long bytes, void* data);
	int m_TotalBytes;
	int m_BufferSize;
	TCHAR *m_Buffer;

};

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_CONNECTION_H__338520C0_391F_4FA5_AC3F_7553F83F2226__INCLUDED_)
