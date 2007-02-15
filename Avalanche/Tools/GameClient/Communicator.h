// Communicator.h: interface for the CCommunicator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMUNICATOR_H__FEAD1523_CA6C_462A_AEB5_F48A10E76DA5__INCLUDED_)
#define AFX_COMMUNICATOR_H__FEAD1523_CA6C_462A_AEB5_F48A10E76DA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGameClientPane;
class CServer;

class CCommunicator  
{
public:
	static CString GetSocketErrorString(int socketError);
	CString GetDeviceIPAddress();
	virtual BOOL Connect() = 0L;
	virtual BOOL Disconnect() = 0L;
	CCommunicator(CString deviceIPAddress);
	virtual ~CCommunicator();

	
protected:
	struct PaneArray
	{
		CGameClientPane* pane;
		const char* name;
		CServer* server;
		int port;
		CRuntimeClass* client;
	};

	BOOL AddPanes(PaneArray* panes);
	CString m_DeviceIPAddress;

};

#endif // !defined(AFX_COMMUNICATOR_H__FEAD1523_CA6C_462A_AEB5_F48A10E76DA5__INCLUDED_)
