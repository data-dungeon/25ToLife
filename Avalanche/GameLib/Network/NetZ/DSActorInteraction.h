/*
**
**  File:   DSActorInteraction.h
**  Date:   April 29, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the actor interaction dataset.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DSActorInteraction.h $
**  $Revision: #3 $
**  $DateTime: 2005/08/25 14:58:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DS_ACTOR_INTERACTION_H
#define DS_ACTOR_INTERACTION_H

#include <netz.h>
#include "Network/NetZ/DSActorInteractionDDL.h"

class DSActorInteraction : public DATASET(DSActorInteraction)
{
  public:
	void OperationEnd(DOOperation* i_operation);
};

#endif // DS_ACTOR_INTERACTION
