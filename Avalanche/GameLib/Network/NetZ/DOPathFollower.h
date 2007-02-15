/*
**
**  File:   DOPathFollower.h
**  Date:   March 2, 2005
**  By:     Bryant Collard
**  Desc:   Implementation of the path follower distributed object.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DOPathFollower.h $
**  $Revision: #3 $
**  $DateTime: 2005/05/03 18:53:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DO_PATH_FOLLOWER_H
#define DO_PATH_FOLLOWER_H

#include <netz.h>
#include "Network/NetZ/DOPathFollowerDDL.h"
#include "Components/Component.h"

class DOPathFollower : public DOCLASS(DOPathFollower)
{
  public:
	virtual ~DOPathFollower();

  protected:
	virtual CActor* CreateProxy(const char* i_class);
	virtual void InitMaster(void);
	virtual void InitProxy(void);
	virtual void BecomeMaster(void);
	virtual void BecomeProxy(void);
	virtual void PropagateMaster(float i_gameTime);
	virtual void Terminate(void);
};

class DOPathFollowerComponent : public CActorComponent
{
  public:
	DOPathFollowerComponent(CCompActor &i_actor) : CActorComponent(i_actor) {}
	virtual const char* ComponentName(void) {return("PathFollowerNetActor");}
	virtual CNetActor* CreateNetActor(void);
};

#endif // DO_PATH_FOLLOWER
