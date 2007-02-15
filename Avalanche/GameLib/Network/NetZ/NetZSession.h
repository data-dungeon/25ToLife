/*
**
**  File:   NetZSession.h
**  Date:   November 21, 2003
**  By:     Bryant Collard
**  Desc:   The NetZ network session.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/NetZSession.h $
**  $Revision: #50 $
**  $DateTime: 2005/08/24 17:01:26 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef NETZ_SESSION_H
#define NETZ_SESSION_H

// GameLib includes
#include "Network/Session.h"
#include "GameHelper/LogUtil.h"

// NetZ includes
#include <netz.h>
#include "Network/NetZ/SessionTime.h"
#include "Network/NetZ/NetZConnection.h"

class DOHost;

class CNetZSession : public CSession
{
  public:
	// Construction
	CNetZSession();
	virtual ~CNetZSession();

	// Management
	virtual bool Open(void);
	virtual bool Reopen(void);
	virtual void Close(void);
	virtual void Update(void);
	virtual bool IsNetworked(void) {return(d_sessionOpen);}
	virtual bool IsMaster(void);
	virtual void EnablePopulate(bool i_enable);

	// Session Time
	virtual int32 GetSessionTimeMS(void);
	virtual float GetSessionTimeSec(void);
	virtual void SetSessionTimeDebug(bool i_active);
	virtual void ToggleSessionTimeDebug(void);

	// Session Locking
	virtual bool RequestLock(CSessionLockRequest* i_lockRequest);
	virtual void RequestUnlock(CSessionLockRequest* i_lockRequest);
	virtual void ClearLockRequests(void);
	virtual bool IsLockedBy(CSessionLockRequest* i_lockRequest);
	virtual void UpdateLock(void);
	virtual bool IsUnlocked(void);
	virtual void DisableLock(void);
	virtual void EnableLock(void);
	virtual bool IsLockResolved(void);

	// Configuration
	virtual void SetConfig(const char* i_config);
	virtual const char* GetConfig(void);
	static void SetConfig(SessionDescription* i_sessionDescription,
			const char* i_config);
	static const char* GetConfig(SessionDescription* i_sessionDescription);

	// Migration
	virtual bool EmigrationPending(void);
	virtual void AcceptImmigration(bool i_accept);

	// Generic actor ids that are valid across the session.
	virtual uint32 ToSessionID(CActor* i_actor);
	virtual uint32 ToSessionID(ActorHandle i_handle);
	virtual CActor* FromSessionIDToActor(uint32 i_id);
	virtual ActorHandle FromSessionIDToHandle(uint32 i_id);

	virtual bool GetFaultStatus( void );

  protected:
	// Installation
	virtual void RegisterCallbacks(void);
	virtual void UnregisterCallbacks(void);
	virtual void InstallMessageTranslators(void);
	virtual const char* GetLocalName(void) {return(NULL);}

  private:
	void PrintNetZError(void);
	CNetZConnection d_connection;
	Time ConvertToSessionTime(float i_time);
	CSessionTime d_sessionTime;

	// Session Locking
	void BeginConnectedLock(void);
	void EndConnectedLock(void);
	void UpdateConnectedLock(void);
	bool ValidateApplicant(DOHandle i_applicant);
	bool d_connectedLock;

  // Debugging
  public:
	static int GetHostIdFromObject(DOHandle i_handle);

  private:
	CLogFile d_logFile;
	CMessageOwner d_messageOwner;
	REF_SEND_MESSAGE_HANDLER(CLog, CNetZSession) d_logHandler;
};

#endif // NETZ_SESSION_H
