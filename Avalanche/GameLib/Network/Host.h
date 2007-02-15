/*
**
**  File:   Host.h
**  Date:   January 21, 2004
**  By:     Bryant Collard
**  Desc:   A implementation independent interface to hosts in a session.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/Host.h $
**  $Revision: #13 $
**  $DateTime: 2005/08/25 14:58:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef HOST_H
#define HOST_H

#include "GameHelper/Message.h"

class CSession;

class CHost
{
  public:
	CHost(void) {d_prev = NULL; d_next = NULL;}
	virtual ~CHost() {}
	virtual uint32 GetIPAddress(void) const {return(0);}
	virtual uint16 GetPortNumber(void) const {return(0);}
	virtual const char* GetName(void) const {return(NULL);}

	CMessageOwner &GetMessageOwner(void) {return(d_messageOwner);}

  protected:
	CHost* d_prev;
	CHost* d_next;

  private:
	CMessageOwner d_messageOwner;

	friend class CSession;
};

#endif // HOST_H
