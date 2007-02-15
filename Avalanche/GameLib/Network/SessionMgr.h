/*
**
**  File:   SessionMgr.h
**  Date:   October 17, 2004
**  By:     Bryant Collard
**  Desc:   Manages session components.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/SessionMgr.h $
**  $Revision: #3 $
**  $DateTime: 2005/08/25 14:58:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef SESSION_MGR_H
#define SESSION_MGR_H

class CSession;
class CWorld;

class CSessionMgr
{
  public:
	CSessionMgr(void);
	virtual ~CSessionMgr(void);

	virtual bool Open(bool i_lanSession = true);
	virtual void Update(void);
	virtual void Close(void);

  protected:
	virtual bool CreateSessionSingletons(void);
	virtual void UpdateSessionSingletons(void);
	virtual void DestroySessionSingletons(void);

	bool d_initialized;
	CSession* d_session;
	CWorld* d_world;
};

#endif // SESSION_MGR_H
