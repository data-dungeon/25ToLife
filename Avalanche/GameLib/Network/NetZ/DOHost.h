/*
**
**  File:   DOHost.h
**  Date:   October 14, 2003
**  By:     Bryant Collard
**  Desc:   Implementation of the host distributed object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DOHost.h $
**  $Revision: #26 $
**  $DateTime: 2005/07/29 14:27:29 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DO_HOST_H
#define DO_HOST_H

#include <netz.h>
#include "Network/NetZ/DOHostDDL.h"
#include "Network/NetZ/AvalancheDefines.h"
#include "Network/Host.h"
#include "Network/Session.h"
#include "GameHelper/Handle.h"

#define HOST_HANDLE_MASK_WIDTH  4
handledef(DOHost) HostHandle;

class DOHost : public DOCLASS(DOHost), public CHost
{
  public:
	// Construct/handle
	DOHost();
	HostHandle GetHostHandle(void) const {return(d_handleCurator.Get());}
	static DOHost* FromHandle(HostHandle i_handle)
		{
			return(CHandleCurator<DOHost, HOST_HANDLE_MASK_WIDTH>::Retrieve(
					i_handle));
		}
	static HostHandle ToHandle(const DOHost* i_host)
		{
			return((i_host != NULL) ?
					i_host->GetHostHandle() : (HostHandle)INVALID_OBJECT_HANDLE);
		}

	// Host instance
	void Initialize(void);
	void StateChanged(void);
	bool WantsLock(void);
	bool HasLock(void);
	virtual void OperationEnd(DOOperation* i_operation);

	// CHost overrides
	virtual uint32 GetIPAddress(void) const;
	virtual uint16 GetPortNumber(void) const;
	virtual const char* GetName(void) const;
	virtual uint32 GetFaultStatus(void);

	// NetZ callbacks
	virtual void InitDO();

	// Special message handling
	virtual void SendFlag(uint32 i_hash, DOHandle i_sender);
	virtual void PostFlag(uint32 i_hash, DOHandle i_sender);
	virtual void SendBuffer(uint32 i_hash, Buffer i_data, DOHandle i_sender);
	virtual void PostBuffer(uint32 i_hash, Buffer i_data, DOHandle i_sender);

	// Local host
	static DOHost* CreateLocal(void);
	static DOHost* GetLocal( void );
	static DOHandle GetLocalHandle(void);
	static bool LocalHasLock(void);
	static bool LocalGrantedLock(void);
	static bool LockGrantedToLocal(DOHost* i_host);
	static bool SetLockState(bool i_desired, DOHost* i_grantee);
	static void PropagateUpdate(void);
	static void Terminate(void);

	// Migration
	static bool EmigrationPending(void) {return(s_migrationLock.WantLock());}
	static void AcceptImmigration(bool i_accept);
	static void ComputeAffinity(DOHandle i_actor, uint8* o_affinity,
			TimeInterval* o_waitTime);
	static uint8 ValidateAffinity(DOHandle i_actor, DOHandle i_station,
			uint8 i_affinity);

	// Fault recovery
	bool ApproveFaultRecovery();

	// Time
#ifdef LATENCY_COMPENSATION
	void SetTimeProperties(float i_hostTimePerSessionTime,
			float i_hostVsSessionIntercept);
	float GetHostTimePerSessionTime(void)
			{return(d_state.d_hostTimePerSessionTime);}
	float GetHostVsSessionIntercept(void)
			{return(d_state.d_hostVsSessionIntercept);}
#endif

  protected:
	virtual CMessageOwner* MessageOwner(void);

  private:
	static DOHost* s_localHost;
	static uint8 s_lockId;

	// The handle.
	CHandleCurator<DOHost, HOST_HANDLE_MASK_WIDTH> d_handleCurator;

	// Migration
	bool NeedToManageActors(void);
	bool ManageActors(void);
	static void CountPersistent(void);
	static uint8 FindLoadRank(DOHost* i_host);
	static bool s_acceptImmigration;
	int d_persistentCount;

	static class CMigrationLockRequest : public CSessionLockRequest
	{
	  public:
		CMigrationLockRequest() : d_lockState(LOCK_NONE) {}
		bool WantLock(void) const {return(d_lockState != LOCK_NONE);}
		bool HaveLock(void) const {return(d_lockState == LOCK_ESTABLISHED);}
		void Desire(bool i_desire);
		virtual void LockEstablished(void);

	  private:
		enum
		{
			LOCK_NONE,
			LOCK_REQUESTED,
			LOCK_ESTABLISHED
		} d_lockState;

	} s_migrationLock;

	static bool s_faultDetected;
};

#endif // DO_HOST
