/*
**
**  File:   NetZMessage.h
**  Date:   February 25, 2004
**  By:     Bryant Collard
**  Desc:   NetZ specific message functionality.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/NetZMessage.h $
**  $Revision: #6 $
**  $DateTime: 2005/03/03 19:40:35 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef NETZ_MESSAGE_H
#define NETZ_MESSAGE_H

#include "GameHelper/Message.h"
#include "Network/NetZ/DOMessageOwner.h"

class CNetZMessageDispatcher : public CMessageDispatcher
{
  public:
	virtual bool InstallBasicDataTypeTranslator(const char* i_name,
			EMessageBasicDataType i_type);

  protected:
	virtual void SendToMaster(void* i_data, ActorHandle i_sender,
			CMessagePair* i_pair);
	virtual void SendToDuplicas(void* i_data, ActorHandle i_sender,
			CMessagePair* i_pair);
	virtual void SendToNeighbors(void* i_data, ActorHandle i_sender,
			CMessagePair* i_pair);
	virtual void SendToSession(const char* i_name, void* i_data,
			ActorHandle i_sender);
	virtual void SendToHost(const char* i_name, void* i_data,
			ActorHandle i_sender, CHost* i_host);
	virtual void PostToMaster(void* i_data, ActorHandle i_sender,
			CMessagePair* i_pair);
	virtual void PostToDuplicas(void* i_data, ActorHandle i_sender,
			CMessagePair* i_pair);
	virtual void PostToNeighbors(void* i_data, ActorHandle i_sender,
			CMessagePair* i_pair);
	virtual void PostToSession(const char* i_name, void* i_data,
			ActorHandle i_sender);
	virtual void PostToHost(const char* i_name, void* i_data,
			ActorHandle i_sender, CHost* i_host);

  private:
	bool GetDORef(CMessagePair* i_pair, DOMessageOwner::Ref &o_doRef);
	void SendMessage(void* i_data, ActorHandle i_sender, CMessagePair* i_pair,
			DOMessageOwner::Ref &i_doRef, qUnsignedInt32 i_target);
	void PostMessage(void* i_data, ActorHandle i_sender, CMessagePair* i_pair,
			DOMessageOwner::Ref &i_doRef, qUnsignedInt32 i_target);
};

#endif // NETZ_MESSAGE_H
