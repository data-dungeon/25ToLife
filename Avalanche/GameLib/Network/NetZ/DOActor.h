/*
**
**  File:   DOActor.h
**  Date:   October 14, 2003
**  By:     Bryant Collard
**  Desc:   Implementation of the actor distributed object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DOActor.h $
**  $Revision: #25 $
**  $DateTime: 2005/08/29 16:53:21 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DO_ACTOR_H
#define DO_ACTOR_H

#include <netz.h>
#include "Network/NetZ/DOActorDDL.h"
#include "Network/NetActor.h"
#include "Environ/ActorInitializer.h"
#include "GameHelper/Handle.h"
#include "GameHelper/Callback.h"

class DOHost;
handledef(DOHost) HostHandle;

class DOActor : public DOCLASS(DOActor), public CNetActor
{
  public:
	// Construct.
	DOActor();

	// Override CNetActor. These routines should not be overridden.
	virtual void SetPersistent(bool i_persistent);
	virtual bool IsPersistent(void);
	virtual void Distribute(CActor* i_master, bool i_persistent);
	virtual void Establish(void);
	virtual void Unbind(bool i_kill = false);
	virtual bool Propagate(float i_gameTime);
	virtual void SetMasterDataMinUpdateTime(uint32 i_lowMS, uint32 i_highMS);
	virtual bool RequestDataSlot(const char* i_name, int i_maxSize);
	virtual void SendData(void);
	virtual bool WriteDataSlot(const char* i_name, void* i_data, int i_size);
	virtual void* OpenDataSlot(const char* i_name);
	virtual const void* ReadDataSlot(const char* i_name);
	virtual const char* GetHostName(void);

	// NetZ callbacks
	virtual void InitDO();
	virtual void OperationEnd(DOOperation* i_operation);
	virtual bool ApproveFaultRecovery() {return(IsPersistent());}

	// Called when the dataset changes.
	void StateChanged(void);

	// Get a pointer to the host associated with the actor's master.
	DOHost* GetHost(void);

  protected:
	// Used by a duplica to create the actor that acts as the proxy for the
	// master's actor. Should be overridden by each doclass derived from
	// DOActor.
	virtual CActor* CreateProxy(const char* i_class);
	virtual GameInitGeometry GetInitGeometryFunc(void) {return(NULL);}

	// Copy data between the actor and the distributed object. Derived classes
	// should do their part then call the parent version.
	virtual void InitMaster(void);
	virtual void InitProxy(void);
	virtual void BecomeMaster(void);
	virtual void BecomeProxy(void);
	virtual void PropagateMaster(float i_gameTime);
	virtual void Terminate(void) {}

	// Override DOMessageOwner.
	virtual CMessageOwner* MessageOwner(void);

	// Debugging
	void DumpDataSlots(const char* i_label);

  private:
	// The handle of the host associated with the actor's master.
	HostHandle d_host;

	// Allow the host to override association during migration.
	void SetHost(DOHost* i_host);
	friend class DOHost;

	// Manage the master data buffers.
	void InitializeMasterData(void);
	void RefreshMasterData(void);
	uint32 d_lowMasterDataRefreshDelay;
	uint32 d_highMasterDataRefreshDelay;
	uint32 d_masterDataRefreshTime;

	// Configurable data.
	void* GetDataSlot(unsigned int i_hash, int &o_maxSize);
	void DataReceived(void);
	CCallback<DOActor> d_callback;
};

#endif // DO_ACTOR
