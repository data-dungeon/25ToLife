/*
**
**  File:   Session.h
**  Date:   October 9, 2003
**  By:     Bryant Collard
**  Desc:   The network session.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/Session.h $
**  $Revision: #47 $
**  $DateTime: 2005/08/24 17:01:26 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef SESSION_H
#define SESSION_H

// GameLib includes //
#include "EngineHelper/Singleton.h"
#include "GameHelper/Handle.h"

// DEFINE FOR STORING THE SESSION URL FOR A GIVEN SESSION
#define MAX_SESSION_URL_LENGTH 256 // this could be done better--ksh

class CHost;
class CSessionLockRequest;
class CActor;
handledef(CActor) ActorHandle;

class CSession
{
  public:
	// Construction
	CSession(void);
	virtual ~CSession(void);

	// Management
	virtual bool Open(void);
	virtual bool Reopen(void);
	virtual void Close(void);
	virtual void Update(void);
	virtual bool IsNetworked(void) {return(false);}
	virtual bool IsMaster(void) {return(IsOpen());}
	virtual void EnablePopulate(bool i_enable) {d_populateEnabled = i_enable;}
	bool IsPopulateEnabled(void) {return(d_populateEnabled);}
	bool IsOpen(void) {return(d_sessionOpen);}
	void SetLanSession(bool i_lanSession) {d_lanSession = i_lanSession;}

	// Hosts
	CHost* GetLocalHost(void) const {return(d_localHost);}
	const CHost* GetNextRemoteHost(const CHost* i_lastHost) const;
	void AddRemoteHost(CHost* i_host);
	void RemoveRemoteHost(CHost* i_host);
	int32 GetNumRemoteHosts();

	// Session URL
	virtual const char* GetSessionURL( void ) { return (d_sessionURL); }

	// Session Time
	virtual int32 GetSessionTimeMS(void);
	virtual float GetSessionTimeSec(void);
	virtual void SetSessionTimeDebug(bool i_active) {}
	virtual void ToggleSessionTimeDebug(void) {}

	// Session Locking
	virtual bool RequestLock(CSessionLockRequest* i_lockRequest);
	virtual void RequestUnlock(CSessionLockRequest* i_lockRequest);
	virtual void ClearLockRequests(void);
	virtual bool IsLockedBy(CSessionLockRequest* i_lockRequest);
	virtual void UpdateLock(void) {}
	virtual bool IsUnlocked(void);
	virtual void DisableLock(void);
	virtual void EnableLock(void);
	virtual bool IsLockResolved(void) {return(true);}
	static void EnableLockLog(bool i_logLocking) {s_logLocking = i_logLocking;}

	// Configuration
	virtual void SetConfig(const char* i_config) {}
	virtual const char* GetConfig(void) {return(NULL);}

	// Migration
	virtual bool EmigrationPending(void) {return(false);}
	virtual void AcceptImmigration(bool i_accept) {}

	// Generic actor ids that are valid across the session.
	virtual uint32 ToSessionID(CActor* i_actor);
	virtual uint32 ToSessionID(ActorHandle i_handle);
	virtual CActor* FromSessionIDToActor(uint32 i_id);
	virtual ActorHandle FromSessionIDToHandle(uint32 i_id);

	virtual bool GetFaultStatus( void ){ return false; }

  protected:
	// Session state
	bool d_lanSession;
	bool d_sessionOpen;
	CHost* d_localHost;
	CHost* d_remoteHostHead;

	// Session URL
	char d_sessionURL[ MAX_SESSION_URL_LENGTH + 1 ];

	// Session Locking
	CSessionLockRequest* d_lockRequestHead;
	enum
	{
		ALLOW_LOCK,
		DISALLOW_LOCK,
		LATCH_LOCK
	} d_lockPermission;
	static bool s_logLocking;

	// Actor creation
	bool d_populateEnabled;
};

class CSessionLockRequest
{
  public:
	// Construction / Destruction
	CSessionLockRequest();
	virtual ~CSessionLockRequest();

	// Add to the end of the request list. The argument is a pointer to the head
	// of the list to which the request is added. If the list is empty, this
	// request will be made the head of the list.
	bool Add(CSessionLockRequest** i_head);

	// Remove the request from the list. Return true if the removed request
	// was the head request.
	bool Remove(CSessionLockRequest** i_head);

	// Override to be notified when the lock is established.
	virtual void LockEstablished(void) {}

  private:
	CSessionLockRequest** d_head;
	CSessionLockRequest* d_prev;
	CSessionLockRequest* d_next;
};

DefineSingleton(CSession)
#define g_session GetSingleton(CSession)

#endif // SESSION_H
