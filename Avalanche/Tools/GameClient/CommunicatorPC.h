// CommunicatorPC.h: interface for the CCommunicatorPC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMUNICATORPC_H__E1BB581D_E674_4984_9F57_244032EADF3F__INCLUDED_)
#define AFX_COMMUNICATORPC_H__E1BB581D_E674_4984_9F57_244032EADF3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Communicator.h"

class CCommunicatorPC : public CCommunicator  
{
public:
	CCommunicatorPC(CString DeviceIPAddress);
	virtual ~CCommunicatorPC();
	virtual BOOL Connect();
	virtual BOOL Disconnect();

};

#endif // !defined(AFX_COMMUNICATORPC_H__E1BB581D_E674_4984_9F57_244032EADF3F__INCLUDED_)
