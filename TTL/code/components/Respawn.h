/*
**
**  File:   Respawn.h
**  Date:   June 30, 2005
**  By:     Bryant Collard
**  Desc:   Manage actor respawn.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/components/Respawn.h $
**  $Revision: #4 $
**  $DateTime: 2005/07/23 17:18:10 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef RESPAWN_H
#define RESPAWN_H

#include "Components/Component.h"

class CRespawn : public CActorComponent
{
  public:
	CRespawn(CCompActor &i_actor);
	~CRespawn() {}

	// Override CActorComponent
	virtual const char *ComponentName(void) {return("Respawn");}
	virtual void Initialize(void);
	virtual void BeginFrame(void);

  private:
	enum
	{
		DISABLED,
		INACTIVE,
		WAITING,
		READY,
		BLOCKED
	} d_state;
	bool d_faded;
	bool d_showMessage;
	int d_minSpawnTime;
	int d_spawnWavePeriod;
	int d_fadeTime;
	int d_respawnTime;
	float d_apprehensionMult;

	REF_SEND_MESSAGE_HANDLER(CManDown, CRespawn) d_manDownHandler;
	REF_SEND_MESSAGE_HANDLER(CBlockRespawn, CRespawn) d_blockRespawnHandler;
	REF_SEND_MESSAGE_HANDLER(CUnblockRespawn, CRespawn) d_unblockRespawnHandler;
};

#endif // RESPAWN_H
