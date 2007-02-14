/*
**
**  File:   TagFrame.h
**  Date:   July 14, 2005
**  By:     Bryant Collard
**  Desc:   Component associated with tag frames.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/components/TagFrame.h $
**  $Revision: #5 $
**  $DateTime: 2005/08/04 16:38:30 $
**    $Author: Bryce $
**
*/

#pragma once

#ifndef TAG_FRAME_H
#define TAG_FRAME_H

#include "Components/Component.h"
#include "InterfacesTTL/ObjectiveIF.h"
#include "GameHelper/Message.h"
#include "sim/TeamingModel.h"
#include "Effects/tizag.h"

class CTagFrame : public CActorComponent, public CObjectiveIF
{
  public:
	CTagFrame(CCompActor &i_actor);
	~CTagFrame();

	// Override CActorComponent
	virtual const char *ComponentName(void) {return("TagFrame");}
	virtual CInterface* GetInterface(uint i_id);
	virtual void SetupNetData(void);
	virtual void LoadNetData(void);
	virtual void Initialize(void);
	virtual void Reset(void);
	virtual void EndUpdate(float i_deltaSec);

	// CObjectiveIF overrides
	virtual EControl GetControl(void);
	virtual uint32 GetTimeOfControlMS(void);
	virtual uint32 GetTimeOfStateMS(void);
	virtual ActorHandle GetMostRecentHolder(void);

	void AssumeRoleOfClosestSpawn(bool i_flag) {d_assumeSpawnRole = i_flag;}
	bool CanInteract(ActorHandle i_handle) const;
	void StartInteraction(ActorHandle i_handle);
	void StopInteraction(void);

	CTeamingModel::ERole GetRole(void) {return(d_role);}

  private:
	struct SState
	{
		uint8 d_control;
		uint32 d_holder;
		int32 d_controlTime;
		uint8 d_role;
	};

	bool InPosition(ActorHandle i_query) const;
	void CompleteInteraction(void);
	void SyncRole(void);
	void Push(void);
	void Pull(void);

	// Our tags
	Tizag* d_currentTag;
	Tizag* d_desiredTag;

	// Local State
	bool d_assumeSpawnRole;
	CTeamingModel::ERole d_decalRole;

	// Network State
	EControl d_control;
	ActorHandle d_holder;
	int32 d_controlTime;
	CTeamingModel::ERole d_role;

	// Messages
	REF_BASIC_MESSAGE_HANDLER(SimpleInteraction, MESSAGE_OWNED_VECTOR3_PTR,
			CTagFrame) d_simpleInteractionHandler;
	REF_SEND_MESSAGE_HANDLER(StopInteraction, CTagFrame)
		d_stopInteractionHandler;
	REF_SEND_MESSAGE_HANDLER(NetDataReceived, CTagFrame) d_netDataReceivedHandler;
};

#endif // TAG_FRAME_H
