/*
**
**  File:   DOMessageOwner.cpp
**  Date:   March 1, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the message owner distributed object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DOMessageOwner.cpp $
**  $Revision: #11 $
**  $DateTime: 2005/04/14 12:57:22 $
**    $Author: Rob $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/NetZ/DOMessageOwner.h"

// NetZ includes
#include <netz.h>
#include "Network/NetZ/DOActor.h"
#include "Network/Session.h"

/**********************************************************************/
/**********************************************************************/
ActorHandle DOMessageOwner::GetSender(DOHandle i_sender)
{
	if (i_sender.IsAKindOf(DOCLASSID(DOActor)))
	{
		DOActor::Ref refSender(i_sender);
		if (refSender.IsValid())
			return(CActor::ToHandle(refSender->GetActor()));
	}
	return(INVALID_OBJECT_HANDLE);
}

/**********************************************************************/
/**********************************************************************/
void DOMessageOwner::SendFlag(uint32 i_hash, DOHandle i_sender)
{
	if(!g_session.IsOpen())
		return;

	CMessagePair* pair = g_messageDispatcher.GetMessagePair(i_hash,
			MessageOwner());
	if (pair != NULL)
	{
		if (IsADuplicationMaster())
		{
#ifdef DEBUG_MESSAGE
			CMessageDispatcher &dispatcher = g_messageDispatcher;
			dispatcher.WriteLogEntry(MESSAGE_LOG_SEND_NETWORK,
					"SendFlag Route to Master", *pair, NULL, false, NULL);
			dispatcher.d_sendDepth++;
#endif
			pair->SendMessage(NULL, GetSender(i_sender), MESSAGE_HANDLE_IF_MASTER);
#ifdef DEBUG_MESSAGE
			dispatcher.d_sendDepth--;
#endif
		}
		else
		{
#ifdef DEBUG_MESSAGE
			CMessageDispatcher &dispatcher = g_messageDispatcher;
			dispatcher.WriteLogEntry(MESSAGE_LOG_SEND_NETWORK,
					"SendFlag Route to Duplica", *pair, NULL, false, NULL);
			dispatcher.d_sendDepth++;
#endif
			pair->SendMessage(NULL, GetSender(i_sender), MESSAGE_HANDLE_IF_DUPLICA);
#ifdef DEBUG_MESSAGE
			dispatcher.d_sendDepth--;
#endif
		}
	}
}

/**********************************************************************/
/**********************************************************************/
void DOMessageOwner::PostFlag(uint32 i_hash, DOHandle i_sender)
{
	if(!g_session.IsOpen())
		return;

	CMessagePair* pair = g_messageDispatcher.GetMessagePair(i_hash,
			MessageOwner());
	if (pair != NULL)
	{
		if (IsADuplicationMaster())
		{
#ifdef DEBUG_MESSAGE
			CMessageDispatcher &dispatcher = g_messageDispatcher;
			dispatcher.WriteLogEntry(MESSAGE_LOG_SEND_NETWORK,
					"PostFlag Route to Master", *pair, NULL, false, NULL);
			dispatcher.d_sendDepth++;
#endif
			pair->PostMessage(NULL, GetSender(i_sender), MESSAGE_HANDLE_IF_MASTER);
#ifdef DEBUG_MESSAGE
			dispatcher.d_sendDepth--;
#endif
		}
		else
		{
#ifdef DEBUG_MESSAGE
			CMessageDispatcher &dispatcher = g_messageDispatcher;
			dispatcher.WriteLogEntry(MESSAGE_LOG_SEND_NETWORK,
					"PostFlag Route to Duplica", *pair, NULL, false, NULL);
			dispatcher.d_sendDepth++;
#endif
			pair->PostMessage(NULL, GetSender(i_sender), MESSAGE_HANDLE_IF_DUPLICA);
#ifdef DEBUG_MESSAGE
			dispatcher.d_sendDepth--;
#endif
		}
	}
}

/**********************************************************************/
/**********************************************************************/
void DOMessageOwner::SendBuffer(uint32 i_hash, Buffer i_data, DOHandle i_sender)
{
	if(!g_session.IsOpen())
		return;

	CMessagePair* pair = g_messageDispatcher.GetMessagePair(i_hash,
			MessageOwner());
	if (pair != NULL)
	{
		if (IsADuplicationMaster())
		{
#ifdef DEBUG_MESSAGE
			CMessageDispatcher &dispatcher = g_messageDispatcher;
			dispatcher.WriteLogEntry(MESSAGE_LOG_SEND_NETWORK,
					"SendBuffer Route to Master", *pair, NULL, false, NULL);
			dispatcher.d_sendDepth++;
#endif
			pair->SendMessage(pair->Unpack((void*)i_data.GetContentPtr()),
					GetSender(i_sender), MESSAGE_HANDLE_IF_MASTER);
#ifdef DEBUG_MESSAGE
			dispatcher.d_sendDepth--;
#endif
		}
		else
		{
#ifdef DEBUG_MESSAGE
			CMessageDispatcher &dispatcher = g_messageDispatcher;
			dispatcher.WriteLogEntry(MESSAGE_LOG_SEND_NETWORK,
					"SendBuffer Route to Duplica", *pair, NULL, false, NULL);
			dispatcher.d_sendDepth++;
#endif
			pair->SendMessage(pair->Unpack((void*)i_data.GetContentPtr()),
					GetSender(i_sender), MESSAGE_HANDLE_IF_DUPLICA);
#ifdef DEBUG_MESSAGE
			dispatcher.d_sendDepth--;
#endif
		}
	}
}

/**********************************************************************/
/**********************************************************************/
void DOMessageOwner::PostBuffer(uint32 i_hash, Buffer i_data, DOHandle i_sender)
{
	if(!g_session.IsOpen())
		return;

	CMessagePair* pair = g_messageDispatcher.GetMessagePair(i_hash,
			MessageOwner());
	if (pair != NULL)
	{
		if (IsADuplicationMaster())
		{
#ifdef DEBUG_MESSAGE
			CMessageDispatcher &dispatcher = g_messageDispatcher;
			dispatcher.WriteLogEntry(MESSAGE_LOG_SEND_NETWORK,
					"PostBuffer Route to Master", *pair, NULL, false, NULL);
			dispatcher.d_sendDepth++;
#endif
			pair->PostMessage(pair->Unpack((void*)i_data.GetContentPtr()),
					GetSender(i_sender), MESSAGE_HANDLE_IF_MASTER);
#ifdef DEBUG_MESSAGE
			dispatcher.d_sendDepth--;
#endif
		}
		else
		{
#ifdef DEBUG_MESSAGE
			CMessageDispatcher &dispatcher = g_messageDispatcher;
			dispatcher.WriteLogEntry(MESSAGE_LOG_SEND_NETWORK,
					"PostBuffer Route to Duplica", *pair, NULL, false, NULL);
			dispatcher.d_sendDepth++;
#endif
			pair->PostMessage(pair->Unpack((void*)i_data.GetContentPtr()),
					GetSender(i_sender), MESSAGE_HANDLE_IF_DUPLICA);
#ifdef DEBUG_MESSAGE
			dispatcher.d_sendDepth--;
#endif
		}
	}
}
