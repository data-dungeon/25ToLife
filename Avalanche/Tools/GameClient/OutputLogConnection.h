// OutputLogConnection.h: interface for the COutputLogConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUTLOGCONNECTION_H__3017B30C_8557_470B_B60B_8CCFF61C9469__INCLUDED_)
#define AFX_OUTPUTLOGCONNECTION_H__3017B30C_8557_470B_B60B_8CCFF61C9469__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Connection.h"

class COutputLogPane;

class COutputLogConnection : public CConnection  
{
	DECLARE_DYNCREATE(COutputLogConnection);

protected:
	COutputLogPane* GetPane();
	virtual int DataReceived(unsigned long bytes, void* data);

public:
	COutputLogConnection();
	virtual ~COutputLogConnection();

};

#endif // !defined(AFX_OUTPUTLOGCONNECTION_H__3017B30C_8557_470B_B60B_8CCFF61C9469__INCLUDED_)
