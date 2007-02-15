/*
**
**  File:   DOMessageOwner.h
**  Date:   March 1, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the message owner distributed object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DOMessageOwner.h $
**  $Revision: #6 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#pragma once

#ifndef DO_MESSAGE_OWNER_H
#define DO_MESSAGE_OWNER_H

#include <netz.h>
#include "Network/NetZ/DOMessageOwnerDDL.h"
#include "GameHelper/Handle.h"

class CMessageOwner;
handledef(CActor) ActorHandle;

class DOMessageOwner : public DOCLASS(DOMessageOwner)
{
  public:
	virtual void SendFlag(uint32 i_hash, DOHandle i_sender);
	virtual void PostFlag(uint32 i_hash, DOHandle i_sender);
	virtual void SendBuffer(uint32 i_hash, Buffer i_data, DOHandle i_sender);
	virtual void PostBuffer(uint32 i_hash, Buffer i_data, DOHandle i_sender);

  protected:
	virtual CMessageOwner* MessageOwner(void) {return(NULL);}
	static ActorHandle GetSender(DOHandle i_sender);
};

#endif // DO_MESSAGE_OWNER
