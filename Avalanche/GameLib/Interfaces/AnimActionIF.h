/*
**
**  File:   AnimActionIF.h
**  Date:   February 5, 2004
**  By:     Bryant Collard
**  Desc:   An interface to animation actions.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Interfaces/AnimActionIF.h $
**  $Revision: #2 $
**  $DateTime: 2005/03/29 12:30:12 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef ANIM_ACTION_IF_H
#define ANIM_ACTION_IF_H

#include "Interfaces/Interface.h"
#include "GameHelper/Handle.h"

class CActor;
handledef(CActor) ActorHandle;

class CAnimActionIF : public CInterface
{
  public:
	static uint GetID(void) {return(CInterface::GetValidID(s_animActionID));}
	static CAnimActionIF* GetInterface(CActor* i_actor);
	static CAnimActionIF* GetInterface(ActorHandle i_handle);

	virtual bool StartAction(int i_action) {return(false);}
	virtual bool EndAction(int i_action) {return(false);}
	virtual bool ActionInProgress(int i_action) {return(false);}

  private:
	static uint s_animActionID;
};

#endif // ANIM_ACTION_IF_H
