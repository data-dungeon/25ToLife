// CommunicatorGameCube.h: interface for the CCommunicatorGameCube class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMUNICATORGAMECUBE_H__346C93D7_6EED_4A82_88D5_0E111DC3A0CB__INCLUDED_)
#define AFX_COMMUNICATORGAMECUBE_H__346C93D7_6EED_4A82_88D5_0E111DC3A0CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Communicator.h"

class CCommunicatorGameCube : public CCommunicator  
{
public:
	CCommunicatorGameCube(CString DeviceIPAddress);
	virtual ~CCommunicatorGameCube();

	virtual BOOL Connect();
	virtual BOOL Disconnect();

};

#endif // !defined(AFX_COMMUNICATORGAMECUBE_H__346C93D7_6EED_4A82_88D5_0E111DC3A0CB__INCLUDED_)
