/*
**
**  File:   TTLSessionMgr.h
**  Date:   October 17, 2004
**  By:     Bryant Collard
**  Desc:   TTL specific session management.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/main/TTLSessionMgr.h $
**  $Revision: #5 $
**  $DateTime: 2005/07/21 15:36:14 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef TTL_SESSION_MGR_H
#define TTL_SESSION_MGR_H

// GameLib includes
#include "Network/SessionMgr.h"
#include "GameHelper/Message.h"

#ifndef CONSUMER_BUILD
#include "GameHelper/ErrorStack.h"
#endif

class Referee;

class CTTLSessionMgr : public CSessionMgr
{
  public:
	CTTLSessionMgr() : d_messageOwner("TTLSessionMgr") {d_referee = NULL;}

	virtual bool Open(bool i_lanSession = true);
	virtual void Close(void);

  protected:
	virtual bool CreateSessionSingletons(void);
	virtual void DestroySessionSingletons(void);
	virtual void UpdateSessionSingletons(void);

	// Session singletons
	Referee* d_referee;

	// Message handling
	CMessageOwner d_messageOwner;
	REF_SEND_MESSAGE_HANDLER(CChangeConfig, CSessionMgr) d_changeConfigHandler;

	// Validation
#ifndef CONSUMER_BUILD
	ErrorHandle d_errorHandle;
	uint32 d_invalidateTime;
#endif
};

#endif // TTL_SESSION_MGR_H
