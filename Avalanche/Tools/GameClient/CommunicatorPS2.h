// CommunicatorPS2.h: interface for the CCommunicatorPS2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMUNICATORPS2_H__260422EA_E1AA_4F23_8618_5F3CA399421B__INCLUDED_)
#define AFX_COMMUNICATORPS2_H__260422EA_E1AA_4F23_8618_5F3CA399421B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Communicator.h"
#include "FileServer.h"

class CCommunicatorPS2 : public CCommunicator  
{
public:
	CCommunicatorPS2(CString DeviceIPAddress);
	virtual ~CCommunicatorPS2();

	virtual BOOL Connect();
	virtual BOOL Disconnect();

protected:
	CServer m_FileServer;
	CServer m_OldFileServer;
	CServer m_OutputServer;
};


#endif // !defined(AFX_COMMUNICATORPS2_H__260422EA_E1AA_4F23_8618_5F3CA399421B__INCLUDED_)
