// CommunicatorXBOX.h: interface for the CCommunicatorXBOX class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMUNICATORXBOX_H__730141BB_6B65_409E_843D_F0FF9F28DEB5__INCLUDED_)
#define AFX_COMMUNICATORXBOX_H__730141BB_6B65_409E_843D_F0FF9F28DEB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Communicator.h"

class CCommunicatorXBOX : public CCommunicator  
{
public:
	CCommunicatorXBOX(CString DeviceIPAddress);
	virtual ~CCommunicatorXBOX();
	virtual BOOL Connect();
	virtual BOOL Disconnect();

};

#endif // !defined(AFX_COMMUNICATORXBOX_H__730141BB_6B65_409E_843D_F0FF9F28DEB5__INCLUDED_)
